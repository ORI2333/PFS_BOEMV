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
#include "ext_plug.h"

/**
    加载指定插件动态库.
*/
int ExtPlugLoad(ExtPlugLib** self, const char* path)
{
    HI_ASSERT(path && *path);
    HI_ASSERT(self);
    int ret;

    ExtPlugLib *plug = (ExtPlugLib*)malloc(sizeof(*plug));
    if (!plug) { // 引用指针前进行判空处理
        HI_ASSERT(0);
    }
    if (memset_s(plug, sizeof(*plug), 0, sizeof(*plug)) != EOK) {
        HI_ASSERT(0);
    }
    INIT_LIST_HEAD(&plug->lnode);

    void *hnd = dlopen(path, RTLD_LAZY);
    HI_EXP_GOTO(!hnd, FAIL, "load '%s' FAIL, err='%s, %d'\n", path, strerror(errno), errno);

    dlerror();
    plug->init = (ExtPlugInitFunc)dlsym(hnd, "ExtPlugInit");
    HI_EXP_GOTO(dlerror() || !plug->init, FAIL,
        "load '%s' 'VoPlugItf' FAIL, err='%s, %d'\n", path, strerror(errno), errno);

    dlerror();
    plug->exit = (ExtPlugExitFunc)dlsym(hnd, "ExtPlugExit");
    HI_EXP_GOTO(dlerror() || !plug->init, FAIL,
        "load '%s' 'ExtPlugExit' FAIL, err='%s, %d'\n", path, strerror(errno), errno);

    ret = plug->init(&plug->userItf, &plug->userObj);
    HI_EXP_GOTO(ret || !plug->userItf, FAIL,
        "exec init of '%s' FAIL, ret=%d\n", path, ret);

    plug->hnd = hnd;
    *self = plug;
    return 0;

    FAIL:
        if (hnd) {
            dlclose(hnd);
        }
        if (plug) {
            free(plug);
        }
        *self = NULL;
        return -1;
}

/**
    卸载指定插件动态库.
*/
void ExtPlugUnload(ExtPlugLib* plug)
{
    HI_ASSERT(plug);
    int ret;

    HI_ASSERT(plug->exit);
    plug->exit();

    HI_ASSERT(plug->hnd);
    ret = dlclose(plug->hnd);
    HI_ASSERT(!ret);

    ret = list_empty(&plug->lnode);
    HI_ASSERT(ret);
    if (memset_s(plug, sizeof(*plug), 0, sizeof(*plug)) != EOK) {
        HI_ASSERT(0);
    }
    free(plug);
}

