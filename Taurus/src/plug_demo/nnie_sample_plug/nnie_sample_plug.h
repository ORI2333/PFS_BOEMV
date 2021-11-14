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

#ifndef NNIE_SAMPLE_PLUG_H
#define NNIE_SAMPLE_PLUG_H

#include <stdint.h>
#include "hi_type.h"
#include "sample_comm_nnie.h"
#include "mpp_help.h"
#include "ai_plug.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

/******************************************************************************
    ���������
******************************************************************************/
/**
    ʶ������ֵ���Ϣ.
*/
typedef struct RecogNumInfo {
    uint32_t num; // ʶ�������ֵ��0~9
    uint32_t score; // ���ֵĿ��Ŷȵ÷֣���ȡֵ��Χ�ɾ����ģ�Ͷ���
}   RecogNumInfo;

/**
    ��⵽���������Ϣ.
*/
typedef struct DetectObjInfo {
    int cls; // ��������>0������
    RectBox box; // ����ľ�������(����)
    float score; // ����Ŀ��Ŷȵ÷�
}   DetectObjInfo;

/******************************************************************************
    �����������ΪԼ����ʽ��JSON�Ĺ��ߺ���.
******************************************************************************/
char* DetectObjsToJson(const DetectObjInfo items[], int itemNum, int* resBytes);

/******************************************************************************
    nnie sample to HiOpenAIS plugs
******************************************************************************/
/*
    create CNN model based mode file
*/
int CnnCreate(SAMPLE_SVP_NNIE_CFG_S **model, const char* modelFile);

/*
    destroy CNN model
*/
void CnnDestroy(SAMPLE_SVP_NNIE_CFG_S *self);

/*
    ����һ��U8C1 image
*/
int CnnCalU8c1Img(SAMPLE_SVP_NNIE_CFG_S* self,
    const IVE_IMAGE_S *img, RecogNumInfo resBuf[], int resSize, int* resLen);

/*
    ����һ��U8C1 file
*/
int CnnCalU8c1File(SAMPLE_SVP_NNIE_CFG_S* self, const char* picFileName);

/**
    load model��������.
*/
typedef int (*LoadModelFunc)(SAMPLE_SVP_NNIE_MODEL_S *model, const char* modelFile);

/*
    create rfcn model based mode file
*/
int RfcnCreate(SAMPLE_SVP_NNIE_CFG_S **model, const char* modelFile, LoadModelFunc loadFunc);

/*
    destroy rfcn model
*/
void RfcnDestroy(SAMPLE_SVP_NNIE_CFG_S *self);

/*
    ����һ��U8C3 image
*/
int RfcnCalImg(SAMPLE_SVP_NNIE_CFG_S* self,
    const IVE_IMAGE_S *img, float thresh, DetectObjInfo resBuf[], int resSize, int* resLen);

/*
    create yolo1 model based mode file
*/
int Yolo1Create(SAMPLE_SVP_NNIE_CFG_S **model, const char* modelFile);

/*
    destroy yolo1 model
*/
void Yolo1Destroy(SAMPLE_SVP_NNIE_CFG_S *self);

/*
    ����һ��image
*/
int Yolo1CalImg(SAMPLE_SVP_NNIE_CFG_S* self,
    const IVE_IMAGE_S *img, float thresh, DetectObjInfo resBuf[], int resSize, int* resLen);


/*
    create yolo2 model based mode file
*/
int Yolo2Create(SAMPLE_SVP_NNIE_CFG_S **model, const char* modelFile);

/*
    destory yolo2 model
*/
void Yolo2Destory(SAMPLE_SVP_NNIE_CFG_S *self);

/*
    cal U8C3 image
*/
int Yolo2CalImg(SAMPLE_SVP_NNIE_CFG_S* self,
    const IVE_IMAGE_S *img, float thresh, DetectObjInfo resBuf[], int resSize, int* resLen);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NNIE_SAMPLE_PLUG_H */
