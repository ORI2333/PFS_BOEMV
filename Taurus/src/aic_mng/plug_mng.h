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

#ifndef AIC_PLUG_MNG_H
#define AIC_PLUG_MNG_H

#include <stdint.h>
#include <stdbool.h>
#include "ai_plug.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
    �����Ϣ.
*/
typedef struct AiPlugInfo {
    struct list_head lnode;
    char *uuid; // xx.yy.zz
    char *desc; // description
    char *prof; // plug profile
    char *path; // plug file path
    bool mngSvc; // �Ƿ��ṩ�˹������
}   AiPlugInfo;

/**
    AiPlugInfo create.
*/
AiPlugInfo* AiPlugInfoNew(
    const char* uuid, const char* desc, const char* prof, const char* path, bool mngSvc);

/**
    AiPlugInfo destroy.
*/
void AiPlugInfoDelete(AiPlugInfo* self);

/**
    AiPlugInfo clone.
*/
AiPlugInfo* AiPlugInfoClone(AiPlugInfo* self);

/**
    �����̬�������Ϣ.
*/
typedef struct AiPlugLib {
    struct list_head lnode;

    void* hnd; // lib handle
    const AiPlug* itf; // plug interface

    cJSON *prof;
    const char* uuid;
    const char* desc;
    int width;
    int height;

    uintptr_t model; // ��appʹ�ã�pm_load_plug_libʱ����loadģ��
    const char* profTxt; // profileԭʼ�ı�
}   AiPlugLib;

/**
    ɨ��ָ��Ŀ¼.
*/
int PmScanPlug(const char* dir, struct list_head* plugInfoList);

/**
    ����ָ�������̬��.
*/
int PmLoadPlugLib(AiPlugLib* plug, const char* path);

/**
    ж��ָ�������̬��.
*/
int PmUnloadPlugLib(AiPlugLib* plug);

#ifdef __cplusplus
}
#endif

#endif // AIC_PLUG_MNG_H
