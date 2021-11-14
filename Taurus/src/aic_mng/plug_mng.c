/*
 * Copyright (c) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "hi_ext_util.h"
#include "cjson_help.h"
#include "plug_mng.h"

/**
    AiPlugInfo create.
*/
AiPlugInfo* AiPlugInfoNew(
    const char* uuid, const char* desc, const char* prof, const char* path, bool mngSvc)
{
    AiPlugInfo *self = NULL;

    self = (AiPlugInfo*)malloc(sizeof(*self));
    HI_ASSERT(self);

    INIT_LIST_HEAD(&self->lnode);
    HI_ASSERT(uuid && *uuid);
    self->uuid = HiStrdup(uuid);
    HI_ASSERT(self->uuid);
    HI_ASSERT(desc && *desc);
    self->desc = HiStrdup(desc);
    HI_ASSERT(self->desc);
    HI_ASSERT(prof && *prof);
    self->prof = HiStrdup(prof);
    HI_ASSERT(self->prof);
    HI_ASSERT(path && *path);
    self->path = HiStrdup(path);
    HI_ASSERT(self->path);
    self->mngSvc = mngSvc;
    return self;
}

/**
    AiPlugInfo destroy.
*/
void AiPlugInfoDelete(AiPlugInfo* self)
{
    HI_ASSERT(self);

    if (self->path) {
        free(self->path);
    }
    if (self->prof) {
        free(self->prof);
    }
    if (self->desc) {
        free(self->desc);
    }
    if (self->uuid) {
        free(self->uuid);
    }
    free(self);
}

/**
    AiPlugInfo clone.
*/
AiPlugInfo* AiPlugInfoClone(AiPlugInfo* self)
{
    return AiPlugInfoNew(self->uuid, self->desc, self->prof, self->path, self->mngSvc);
}

/**
    扫描指定目录.
*/
int PmScanPlug(const char* dir, struct list_head* plugInfoList)
{
    HI_ASSERT(dir && *dir);
    HI_ASSERT(plugInfoList);
    int ret = list_empty(plugInfoList);
    HI_ASSERT(ret);
    struct dirent *entry = NULL;
    struct stat statbuf;
    AiPlugLib plug;
    char plugPath[PATH_MAX];
    const char *lastSep = dir[strlen(dir) - 1] == '/' ? "" : "/";

    DIR *dp = opendir(dir);
    HI_EXP_RET(!dp, -errno, "open dir '%s' FAIL, err='%s, %d'\n", dir, strerror(errno), errno);

    // 用可重入的readdir_r()替代
    while ((entry = readdir(dp)) != NULL) {
        if (snprintf_s(plugPath, sizeof(plugPath), sizeof(plugPath) - 1, "%s%s%s", dir, lastSep, entry->d_name) < 0) {
            HI_ASSERT(0);
        }
        if (lstat(plugPath, &statbuf) < 0) {
            LOGW("lstat(%s) FAIL, err='%s, %d'\n", entry->d_name, strerror(errno), errno);
            continue;
        }
        if (S_ISDIR(statbuf.st_mode)) { // 跳过子目录，不递归
            continue;
        }

        const char* extName = HiPathExt(entry->d_name);
        if (!extName || strcmp(extName, AI_PLUG_EXTNAME) != 0) {
            continue;
        }

        if (PmLoadPlugLib(&plug, plugPath) < 0) {
            continue;
        }

        if (!*plug.uuid || !*plug.desc) {
            LOGW("plug prof ERR, prof.uuid='%s', desc='%s'\n", plug.uuid, plug.desc);
            PmUnloadPlugLib(&plug);
            continue;
        }

        bool mngSvc = plug.itf && plug.itf->StartSvc && plug.itf->StopSvc;
        AiPlugInfo *plugInfo = AiPlugInfoNew(plug.uuid, plug.desc, plug.profTxt, plugPath, mngSvc);
        list_add_tail(&plugInfo->lnode, plugInfoList);
        PmUnloadPlugLib(&plug);
    }

    closedir(dp);
    return 0;
}

/**
    加载指定插件动态库.
*/
int PmLoadPlugLib(AiPlugLib* plug, const char* path)
{
    HI_ASSERT(path && *path);
    HI_ASSERT(plug);
    uint32_t magic = 0;
    AIPlugItfFunc rootFunc;
    int ret = -1;

    if (memset_s(plug, sizeof(*plug), 0, sizeof(*plug)) != EOK) {
        HI_ASSERT(0);
    }
    INIT_LIST_HEAD(&plug->lnode);

    void *hnd = dlopen(path, RTLD_LAZY);
    HI_EXP_RET(!hnd, -errno, "load '%s' FAIL, err='%s, %d'\n", path, strerror(errno), errno);
    plug->hnd = hnd;

    dlerror();
    rootFunc = (AIPlugItfFunc)dlsym(hnd, "AiPlugItf");
    HI_EXP_GOTO(dlerror() || !rootFunc, ERR_END,
        "load '%s' 'AiPlugItf' FAIL, err='%s, %d'\n", path, strerror(errno), errno);

    plug->itf = rootFunc(&magic);
    HI_EXP_GOTO(!plug->itf, ERR_END, "AiPlugItf() return NULL\n");

    HI_EXP_GOTO(magic != AI_PLUG_MAGIC, ERR_END,
        "AiPlugItf() MAGIC mismatch, res=%08X, accept=%08X\n", magic, AI_PLUG_MAGIC);
    HI_EXP_GOTO(!plug->itf->Prof || !plug->itf->Load || !plug->itf->Unload || !plug->itf->Cal,
        ERR_END, "AiPlug has NULL func\n");

    plug->profTxt = plug->itf->Prof();
    HI_EXP_GOTO(!plug->profTxt || !*plug->profTxt, ERR_END, "AiPlug::prof() ret NULL\n");

    plug->prof = cJSON_Parse(plug->profTxt);
    HI_EXP_GOTO(!plug->prof, ERR_END, "parse JSON prof FAIL\n");
    plug->uuid = JsonStr(plug->prof, "uuid", "");
    plug->desc = JsonStr(plug->prof, "desc", "");
    plug->width = JsonInt(plug->prof, "frmWidth", -1);
    plug->height = JsonInt(plug->prof, "frmHeight", -1);
    plug->model = 0;
    return 0;

    ERR_END:
        dlclose(hnd);
        if (memset_s(plug, sizeof(*plug), 0, sizeof(*plug)) != EOK) {
            HI_ASSERT(0);
        }
        return ret;
}

/**
    卸载指定插件动态库.
*/
int PmUnloadPlugLib(AiPlugLib* plug)
{
    HI_ASSERT(plug && plug->hnd);
    int ret;

    if (plug->prof) {
        cJSON_Delete(plug->prof);
    }
    ret = dlclose(plug->hnd);
    HI_ASSERT(!ret);

    if (memset_s(plug, sizeof(*plug), 0, sizeof(*plug)) != EOK) {
        HI_ASSERT(0);
    }
    return ret;
}

