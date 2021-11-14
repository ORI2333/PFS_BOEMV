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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <math.h>
#include <assert.h>

#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_svp.h"
#include "sample_comm.h"
#include "sample_comm_svp.h"
#include "sample_comm_nnie.h"
#include "sample_comm_ive.h"
#include "sample_svp_nnie_software.h"
#include "sample_nnie_main.h"

#include "nnie_sample_plug.h"

/******************************************************************************
    nnie sample to HiOpenAIS plugs
******************************************************************************/
#define SP_NNIE_PLUG

#include <hi_ext_util.h>

char* DetectObjsToJson(const DetectObjInfo items[], int itemNum, int* resBytes)
{
    int jsonSize = TINY_BUF_SIZE + itemNum * SMALL_BUF_SIZE; // 每一个obj占用SMALL_BUF_SIZE空间
    char *jsonBuf = (char*)malloc(jsonSize);
    HI_ASSERT(jsonBuf);
    int offset = 0;

    offset += snprintf_s(jsonBuf + offset, jsonSize - offset, jsonSize - offset - 1, "[");
    for (int i = 0; i < itemNum; i++) {
        const DetectObjInfo *item = &items[i];
        offset += snprintf_s(jsonBuf + offset, jsonSize - offset, jsonSize - offset - 1,
            "%s\n  { \"class\": %d, \"score\": %.2f, \"box\": "
            "{ \"xmin\": %d, \"ymin\": %d, \"xmax\": %d, \"ymax\": %d }}",
            (i == 0 ? "" : ","), item->cls, item->score,
            item->box.xmin, item->box.ymin, item->box.xmax, item->box.ymax);
        HI_ASSERT(offset < jsonSize);
    }
    offset += snprintf_s(jsonBuf + offset, jsonSize - offset, jsonSize - offset - 1, "]");
    HI_ASSERT(offset < jsonSize);

    if (resBytes) {
        *resBytes = offset;
    }
    return jsonBuf;
}

static HI_S32 FillNnieByImg(SAMPLE_SVP_NNIE_CFG_S* pstNnieCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, int segId, int nodeId, const IVE_IMAGE_S *img)
{
    HI_U32 i;
    HI_U32 j;
    HI_U32 n;
    HI_U32 u32Height = 0;
    HI_U32 u32Width = 0;
    HI_U32 u32Chn = 0;
    HI_U32 u32Stride = 0;
    HI_U32 u32VarSize;
    HI_U8 *pu8PicAddr = NULL;

    /* get data size */
    if (SVP_BLOB_TYPE_U8 <= pstNnieParam->astSegData[segId].astSrc[nodeId].enType &&
        SVP_BLOB_TYPE_YVU422SP >= pstNnieParam->astSegData[segId].astSrc[nodeId].enType) {
        u32VarSize = sizeof(HI_U8);
    } else {
        u32VarSize = sizeof(HI_U32);
    }

    /* fill src data */
    if (SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[segId].astSrc[nodeId].enType) {
        assert(0);
    } else {
        u32Height = pstNnieParam->astSegData[segId].astSrc[nodeId].unShape.stWhc.u32Height;
        u32Width = pstNnieParam->astSegData[segId].astSrc[nodeId].unShape.stWhc.u32Width;
        u32Chn = pstNnieParam->astSegData[segId].astSrc[nodeId].unShape.stWhc.u32Chn;
        u32Stride = pstNnieParam->astSegData[segId].astSrc[nodeId].u32Stride;
        pu8PicAddr = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U8,
            pstNnieParam->astSegData[segId].astSrc[nodeId].u64VirAddr);

        if (SVP_BLOB_TYPE_YVU420SP == pstNnieParam->astSegData[segId].astSrc[nodeId].enType) {
            assert(pstNnieParam->astSegData[segId].astSrc[nodeId].u32Num == 1);
            for (n = 0; n < pstNnieParam->astSegData[segId].astSrc[nodeId].u32Num; n++) {
                // Y
                const uint8_t *srcData = (const uint8_t*)(uintptr_t)img->au64VirAddr[0];
                assert(srcData);
                for (j = 0; j < u32Height; j++) {
                    if (memcpy_s(pu8PicAddr, u32Width * u32VarSize, srcData, u32Width * u32VarSize) != EOK) {
                        HI_ASSERT(0);
                    }
                    pu8PicAddr += u32Stride;
                    srcData += img->au32Stride[0];
                }
                // UV
                srcData = (const uint8_t*)(uintptr_t)img->au64VirAddr[1];
                assert(srcData);
                for (j = 0; j < u32Height / 2; j++) { // 2: 1/2Height
                    if (memcpy_s(pu8PicAddr, u32Width * u32VarSize, srcData, u32Width * u32VarSize) != EOK) {
                        HI_ASSERT(0);
                    }
                    pu8PicAddr += u32Stride;
                    srcData += img->au32Stride[1];
                }
            }
        } else if (SVP_BLOB_TYPE_YVU422SP == pstNnieParam->astSegData[segId].astSrc[nodeId].enType) {
            assert(0);
        } else {
            for (n = 0; n < pstNnieParam->astSegData[segId].astSrc[nodeId].u32Num; n++) {
                for (i = 0; i < u32Chn; i++) {
                    const uint8_t *srcData = (const uint8_t*)(uintptr_t)img->au64VirAddr[i];
                    assert(srcData);
                    for (j = 0; j < u32Height; j++) {
                        if (memcpy_s(pu8PicAddr, u32Width * u32VarSize, srcData, u32Width * u32VarSize) != EOK) {
                            HI_ASSERT(0);
                        }
                        pu8PicAddr += u32Stride;
                        srcData += img->au32Stride[i];
                    }
                }
            }
        }

        SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[segId].astSrc[nodeId].u64PhyAddr,
            SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID, pstNnieParam->astSegData[segId].astSrc[nodeId].u64VirAddr),
            pstNnieParam->astSegData[segId].astSrc[nodeId].u32Num*u32Chn*u32Height*u32Stride);
    }

    return HI_SUCCESS;
}

/*
    create CNN model based mode file
*/
int CnnCreate(SAMPLE_SVP_NNIE_CFG_S **model, const char* modelFile)
{
    SAMPLE_SVP_NNIE_CFG_S *self;
    HI_U32 u32PicNum = 1;
    HI_S32 s32Ret;

    self = (SAMPLE_SVP_NNIE_CFG_S*)malloc(sizeof(*self));
    assert(self);
    if (memset_s(self, sizeof(*self), 0x00, sizeof(*self)) != EOK) {
        HI_ASSERT(0);
    }

    // Set configuration parameter
    self->pszPic = NULL;
    self->u32MaxInputNum = u32PicNum; // max input image num in each batch
    self->u32MaxRoiNum = 0;
    self->aenNnieCoreId[0] = SVP_NNIE_ID_0; // set NNIE core
    s_stCnnSoftwareParam.u32TopN = 5; // 5: value of the u32TopN

    // Sys init
    // CNN Load model
    SAMPLE_SVP_TRACE_INFO("Cnn Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel((char*)modelFile, &s_stCnnModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CNN_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    // CNN parameter initialization
    // Cnn software parameters are set in SAMPLE_SVP_NNIE_Cnn_SoftwareParaInit,
    // if user has changed net struct, please make sure the parameter settings in
    // SAMPLE_SVP_NNIE_Cnn_SoftwareParaInit function are correct
    SAMPLE_SVP_TRACE_INFO("Cnn parameter initialization!\n");
    s_stCnnNnieParam.pstModel = &s_stCnnModel.stModel;
    s32Ret = SAMPLE_SVP_NNIE_Cnn_ParamInit(self, &s_stCnnNnieParam, &s_stCnnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CNN_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Cnn_ParamInit failed!\n");

    // 模型关键信息
    LOGI("model={ type=%x, frmNum=%u, chnNum=%u, w=%u, h=%u, stride=%u }\n",
        s_stCnnNnieParam.astSegData[0].astSrc[0].enType,
        s_stCnnNnieParam.astSegData[0].astSrc[0].u32Num,
        s_stCnnNnieParam.astSegData[0].astSrc[0].unShape.stWhc.u32Chn,
        s_stCnnNnieParam.astSegData[0].astSrc[0].unShape.stWhc.u32Width,
        s_stCnnNnieParam.astSegData[0].astSrc[0].unShape.stWhc.u32Height,
        s_stCnnNnieParam.astSegData[0].astSrc[0].u32Stride);

    // record tskBuf
    s32Ret = HI_MPI_SVP_NNIE_AddTskBuf(&(s_stCnnNnieParam.astForwardCtrl[0].stTskBuf));
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CNN_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,HI_MPI_SVP_NNIE_AddTskBuf failed!\n");
    *model = self;
    return 0;

    CNN_FAIL_0:
        SAMPLE_SVP_NNIE_Cnn_Deinit(&s_stCnnNnieParam, &s_stCnnSoftwareParam, &s_stCnnModel);
        *model = NULL;
        return -1;
}

/*
    destroy CNN model
*/
void CnnDestroy(SAMPLE_SVP_NNIE_CFG_S *self)
{
    HI_S32 s32Ret;

    /* Remove TskBuf */
    s32Ret = HI_MPI_SVP_NNIE_RemoveTskBuf(&(s_stCnnNnieParam.astForwardCtrl[0].stTskBuf));
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CNN_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,HI_MPI_SVP_NNIE_RemoveTskBuf failed!\n");

    CNN_FAIL_0:
        SAMPLE_SVP_NNIE_Cnn_Deinit(&s_stCnnNnieParam, &s_stCnnSoftwareParam, &s_stCnnModel);
        free(self);
}

void CnnFetchRes(SVP_BLOB_S *pstGetTopN, HI_U32 u32TopN, RecogNumInfo resBuf[], int resSize, int* resLen)
{
    assert(pstGetTopN);
    HI_U32 i;
    HI_U32 j = 0;
    HI_U32 *pu32Tmp = NULL;
    HI_U32 u32Stride = pstGetTopN->u32Stride;
    if (memset_s(resBuf, resSize * sizeof(resBuf[0]), 0x00, resSize * sizeof(resBuf[0])) != EOK) {
        HI_ASSERT(0);
    }

    int resId = 0;
    pu32Tmp = (HI_U32*)((HI_UL)pstGetTopN->u64VirAddr + j * u32Stride);
    for (i = 0; i < u32TopN * 2 && resId < resSize; i += 2, resId++) { // 2: u32TopN*2
        resBuf[resId].num = pu32Tmp[i];
        resBuf[resId].score = pu32Tmp[i + 1];
    }
    *resLen = resId;
}

void CnnDumpRes(SVP_BLOB_S *pstGetTopN, HI_U32 u32TopN)
{
    assert(pstGetTopN);
    HI_U32 i;
    HI_U32 j;
    HI_U32 *pu32Tmp = NULL;
    HI_U32 u32Stride = pstGetTopN->u32Stride;

    printf("{ ");
    for (j = 0; j < pstGetTopN->u32Num; j++) {
        pu32Tmp = (HI_U32*)((HI_UL)pstGetTopN->u64VirAddr + j * u32Stride);
        for (i = 0; i < u32TopN * 2; i += 2) { // 2: u32TopN*2
            printf("%s%u:%u", (i == 0 ? "" : ", "), pu32Tmp[i], pu32Tmp[i + 1]);
        }
    }
    printf(" }");
}

/*
    计算一张U8C1 image
*/
int CnnCalU8c1Img(SAMPLE_SVP_NNIE_CFG_S* self,
    const IVE_IMAGE_S *img, RecogNumInfo resBuf[], int resSize, int* resLen)
{
    HI_S32 s32Ret;
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};

    /* Fill src data */
    self->pszPic = NULL;
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = FillNnieByImg(self, &s_stCnnNnieParam, 0, 0, img);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CNN_FAIL_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /* NNIE process(process the 0-th segment) */
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stCnnNnieParam, &stInputDataIdx, &stProcSegIdx, HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CNN_FAIL_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

    /* Software process */
    /* if user has changed net struct, please make sure SAMPLE_SVP_NNIE_Cnn_GetTopN
     function's input datas are correct */
    s32Ret = SAMPLE_SVP_NNIE_Cnn_GetTopN(&s_stCnnNnieParam, &s_stCnnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CNN_FAIL_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_CnnGetTopN failed!\n");

    /* Print result */
    CnnFetchRes(&s_stCnnSoftwareParam.stGetTopN, s_stCnnSoftwareParam.u32TopN, resBuf, resSize, resLen);
    return 0;

    CNN_FAIL_1:
        return -1;
}

/*
    计算一张U8C1 file
*/
int CnnCalU8c1File(SAMPLE_SVP_NNIE_CFG_S* self, const char* picFileName)
{
    HI_S32 s32Ret;
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};

    /* Fill src data */
    SAMPLE_SVP_TRACE_INFO("cnn_cal_yun start!\n");
    self->pszPic = (char*)picFileName;
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData(self, &s_stCnnNnieParam, &stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CNN_FAIL_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /* NNIE process(process the 0-th segment) */
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stCnnNnieParam, &stInputDataIdx, &stProcSegIdx, HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CNN_FAIL_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

    /* Software process */
    /* if user has changed net struct, please make sure SAMPLE_SVP_NNIE_Cnn_GetTopN
     function's input datas are correct */
    s32Ret = SAMPLE_SVP_NNIE_Cnn_GetTopN(&s_stCnnNnieParam, &s_stCnnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CNN_FAIL_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_CnnGetTopN failed!\n");

    /* Print result */
    DOLOGI((printf("cnnRes="),
        CnnDumpRes(&s_stCnnSoftwareParam.stGetTopN, s_stCnnSoftwareParam.u32TopN),
        putchar('\n')));

    SAMPLE_SVP_TRACE_INFO("cnn_cal_yun done\n");
    return 0;

    CNN_FAIL_1:
        return -1;
}

/*
    create rfcn model based mode file
*/
int RfcnCreate(SAMPLE_SVP_NNIE_CFG_S **model, const char* modelFile, LoadModelFunc loadFunc)
{
    SAMPLE_SVP_NNIE_CFG_S *self;
    HI_S32 s32Ret;

    *model = NULL;
    self = (SAMPLE_SVP_NNIE_CFG_S*)malloc(sizeof(*self));
    assert(self);
    if (memset_s(self, sizeof(*self), 0x00, sizeof(*self)) != EOK) {
        HI_ASSERT(0);
    }
    if (memset_s(&s_stRfcnModel, sizeof(s_stRfcnModel), 0, sizeof(s_stRfcnModel)) != EOK) {
        HI_ASSERT(0);
    }
    if (memset_s(&s_stRfcnNnieParam, sizeof(s_stRfcnNnieParam), 0, sizeof(s_stRfcnNnieParam)) != EOK) {
        HI_ASSERT(0);
    }
    if (memset_s(&s_stRfcnSoftwareParam, sizeof(s_stRfcnSoftwareParam), 0, sizeof(s_stRfcnSoftwareParam))) {
        HI_ASSERT(0);
    }

    // Sys init
    self->pszPic = NULL;
    self->u32MaxInputNum = 1; // max input image num in each batch
    self->u32MaxRoiNum = 300; // 300: value of MaxPoiNum
    self->aenNnieCoreId[0] = SVP_NNIE_ID_0; // set NNIE core for 0-th Seg
    self->aenNnieCoreId[1] = SVP_NNIE_ID_0; // set NNIE core for 1-th Seg
    self->aenNnieCoreId[2] = SVP_NNIE_ID_0; // set NNIE core for 2-th Seg

    LOGI("Rfcn Load model!\n");
    if (loadFunc) {
        s32Ret = loadFunc(&s_stRfcnModel, modelFile);
    } else {
        s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel((char*)modelFile, &s_stRfcnModel);
    }
    HI_CHK_GOTO(s32Ret, FAIL_0, "Load failed!\n");

    // apcRpnDataLayerName is used to set RPN data layer name
    // and search RPN input data,if user has changed network struct, please
    // make sure the data layer names are correct
    LOGI("Rfcn parameter initialization!\n");
    s_stRfcnNnieParam.pstModel = &s_stRfcnModel.stModel;
    s_stRfcnSoftwareParam.apcRpnDataLayerName[0] = "rpn_cls_score";
    s_stRfcnSoftwareParam.apcRpnDataLayerName[1] = "rpn_bbox_pred";
    s32Ret = SAMPLE_SVP_NNIE_Rfcn_ParamInit(self, &s_stRfcnNnieParam, &s_stRfcnSoftwareParam);
    HI_CHK_GOTO(s32Ret, FAIL_0, "SAMPLE_SVP_NNIE_Rfcn_ParamInit failed!\n");
    s_bNnieStopSignal = HI_FALSE;

    // 模型关键信息
    LOGI("model.base={ type=%x, frmNum=%u, chnNum=%u, w=%u, h=%u, stride=%u }\n",
        s_stRfcnNnieParam.astSegData[0].astSrc[0].enType,
        s_stRfcnNnieParam.astSegData[0].astSrc[0].u32Num,
        s_stRfcnNnieParam.astSegData[0].astSrc[0].unShape.stWhc.u32Chn,
        s_stRfcnNnieParam.astSegData[0].astSrc[0].unShape.stWhc.u32Width,
        s_stRfcnNnieParam.astSegData[0].astSrc[0].unShape.stWhc.u32Height,
        s_stRfcnNnieParam.astSegData[0].astSrc[0].u32Stride);
    LOGI("model.soft={ class=%u, ori.w=%u, ori.h=%u, nmsThresh=%u }\n",
        s_stRfcnSoftwareParam.u32ClassNum,
        s_stRfcnSoftwareParam.u32OriImWidth,
        s_stRfcnSoftwareParam.u32OriImHeight,
        s_stRfcnSoftwareParam.u32NmsThresh);

    *model = self;
    return 0;

    FAIL_0:
        SAMPLE_SVP_NNIE_Rfcn_Deinit(&s_stRfcnNnieParam, &s_stRfcnSoftwareParam, &s_stRfcnModel);
        *model = NULL;
        return -1;
}

/*
    destroy rfcn model
*/
void RfcnDestroy(SAMPLE_SVP_NNIE_CFG_S *self)
{
    SAMPLE_SVP_NNIE_Rfcn_Deinit(&s_stRfcnNnieParam, &s_stRfcnSoftwareParam, &s_stRfcnModel);
    free(self);
}

/*
    fetch result
*/
void RfcnFetchRes(
    SVP_BLOB_S *pstDstScore, SVP_BLOB_S *pstDstRoi, SVP_BLOB_S *pstClassRoiNum,
    HI_FLOAT f32PrintResultThresh,
    DetectObjInfo resBuf[], int resSize, int* resLen)
{
    HI_U32 i;
    HI_U32 j;
    HI_U32 u32RoiNumBias = 0;
    HI_U32 u32ScoreBias;
    HI_U32 u32BboxBias;
    HI_FLOAT f32Score;
    HI_S32* ps32Score = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32, pstDstScore->u64VirAddr);
    HI_S32* ps32Roi = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32, pstDstRoi->u64VirAddr);
    HI_S32* ps32ClassRoiNum = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32, pstClassRoiNum->u64VirAddr);
    HI_U32 u32ClassNum = pstClassRoiNum->unShape.stWhc.u32Width;

    // yolo1_face只有2类，实际为1类
    /* print result, this sample has 21 classes:
     class 0:background     class 1:plane           class 2:bicycle
     class 3:bird           class 4:boat            class 5:bottle
     class 6:bus            class 7:car             class 8:cat
     class 9:chair          class10:cow             class11:diningtable
     class 12:dog           class13:horse           class14:motorbike
     class 15:person        class16:pottedplant     class17:sheep
     class 18:sofa          class19:train           class20:tvmonitor */
    assert(u32ClassNum == 2); // 2: the size of ClassNum
    assert(resSize > 0);
    int resId = 0;
    *resLen = 0;

    if (memset_s(resBuf, resSize * sizeof(resBuf[0]), 0x00, resSize * sizeof(resBuf[0])) != EOK) {
        HI_ASSERT(0);
    }

    u32RoiNumBias += ps32ClassRoiNum[0];
    for (i = 1; i < u32ClassNum; i++) {
        u32ScoreBias = u32RoiNumBias;
        u32BboxBias = u32RoiNumBias * SAMPLE_SVP_NNIE_COORDI_NUM;

        // if the confidence score greater than result threshold, the result will be printed
        if ((HI_FLOAT)ps32Score[u32ScoreBias] / SAMPLE_SVP_NNIE_QUANT_BASE >=
            f32PrintResultThresh && ps32ClassRoiNum[i] != 0) {
        }

        for (j = 0; j < (HI_U32)ps32ClassRoiNum[i]; j++) {
            f32Score = (HI_FLOAT)ps32Score[u32ScoreBias + j] / SAMPLE_SVP_NNIE_QUANT_BASE;
            if (f32Score < f32PrintResultThresh) {
                break;
            }
            if (resId >= resSize) {
                LOGE("rfcn resBuf full\n");
                break;
            }

            resBuf[resId].cls = 1; // class 1
            resBuf[resId].score = f32Score;

            RectBox *box = &resBuf[resId].box;
            box->xmin = ps32Roi[u32BboxBias + j * SAMPLE_SVP_NNIE_COORDI_NUM];
            box->ymin = ps32Roi[u32BboxBias + j * SAMPLE_SVP_NNIE_COORDI_NUM + 1];
            box->xmax = ps32Roi[u32BboxBias + j * SAMPLE_SVP_NNIE_COORDI_NUM + 2]; // 2: 数组下标组成
            box->ymax = ps32Roi[u32BboxBias + j * SAMPLE_SVP_NNIE_COORDI_NUM + 3]; // 3: 数组下标组成
            if (box->xmin >= box->xmax || box->ymin >= box->ymax) {
                LOGD("rfcn_orig: {%d, %d, %d, %d}, %f, discard for coord ERR\n",
                    box->xmin, box->ymin, box->xmax, box->ymax, f32Score);
            } else {
                LOGD("rfcn_orig: {%d, %d, %d, %d}, %f\n",
                    box->xmin, box->ymin, box->xmax, box->ymax, f32Score);
                resId++;
            }
        }
        u32RoiNumBias += ps32ClassRoiNum[i];
    }
    *resLen = resId;
}

/*
    计算一张U8C3 image
*/
int RfcnCalImg(SAMPLE_SVP_NNIE_CFG_S* self,
    const IVE_IMAGE_S *img, float thresh, DetectObjInfo resBuf[], int resSize, int* resLen)
{
    thresh = thresh < 0 ? 0.6f : thresh;
    HI_S32 s32Ret;

    self->pszPic = NULL;

    s32Ret = FillNnieByImg(self, &s_stRfcnNnieParam, 0, 0, img);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    // NNIE process(process the 0-th segment)
    // 此函数已经调用了GetResult
    s32Ret = SAMPLE_SVP_NNIE_Rfcn_Proc(&s_stRfcnNnieParam, &s_stRfcnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Rfcn_Proc failed!\n");

    RfcnFetchRes(&s_stRfcnSoftwareParam.stDstScore,
        &s_stRfcnSoftwareParam.stDstRoi, &s_stRfcnSoftwareParam.stClassRoiNum,
        thresh, resBuf, resSize, resLen);
    return 0;

    FAIL_0:
        return -1;
}

/*
    create yolo1 model based mode file
*/
int Yolo1Create(SAMPLE_SVP_NNIE_CFG_S **model, const char* modelFile)
{
    SAMPLE_SVP_NNIE_CFG_S *self;
    HI_U32 u32PicNum = 1;
    HI_S32 s32Ret;

    self = (SAMPLE_SVP_NNIE_CFG_S*)malloc(sizeof(*self));
    assert(self);

    if (memset_s(self, sizeof(*self), 0x00, sizeof(*self))) {
        HI_ASSERT(0);
    }

    // Set configuration parameter
    self->pszPic = NULL;
    self->u32MaxInputNum = u32PicNum; // max input image num in each batch
    self->u32MaxRoiNum = 0;
    self->aenNnieCoreId[0] = SVP_NNIE_ID_0; // set NNIE core

    // Sys init
    // Yolov1 Load model*/
    SAMPLE_SVP_TRACE_INFO("Yolov1 Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel((char*)modelFile, &s_stYolov1Model);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    // Yolov1 parameter initialization
    // Yolov1 software parameters are set in SAMPLE_SVP_NNIE_Yolov1_SoftwareInit,
    //  if user has changed net struct, please make sure the parameter settings in
    //  SAMPLE_SVP_NNIE_Yolov1_SoftwareInit function are correct
    SAMPLE_SVP_TRACE_INFO("Yolov1 parameter initialization!\n");
    s_stYolov1NnieParam.pstModel = &s_stYolov1Model.stModel;
    s32Ret = SAMPLE_SVP_NNIE_Yolov1_ParamInit(self, &s_stYolov1NnieParam, &s_stYolov1SoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Yolov1_ParamInit failed!\n");

    // 模型关键信息
    LOGI("model.base={ type=%x, frmNum=%u, chnNum=%u, w=%u, h=%u, stride=%u }\n",
        s_stYolov1NnieParam.astSegData[0].astSrc[0].enType,
        s_stYolov1NnieParam.astSegData[0].astSrc[0].u32Num,
        s_stYolov1NnieParam.astSegData[0].astSrc[0].unShape.stWhc.u32Chn,
        s_stYolov1NnieParam.astSegData[0].astSrc[0].unShape.stWhc.u32Width,
        s_stYolov1NnieParam.astSegData[0].astSrc[0].unShape.stWhc.u32Height,
        s_stYolov1NnieParam.astSegData[0].astSrc[0].u32Stride);
    LOGI("model.soft={ class=%u, ori.w=%u, ori.h=%u, bnum=%u, grid.w=%u, grid.h=%u, nmsThresh=%u, confThresh=%u }\n",
        s_stYolov1SoftwareParam.u32ClassNum,
        s_stYolov1SoftwareParam.u32OriImWidth,
        s_stYolov1SoftwareParam.u32OriImHeight,
        s_stYolov1SoftwareParam.u32BboxNumEachGrid,
        s_stYolov1SoftwareParam.u32GridNumWidth,
        s_stYolov1SoftwareParam.u32GridNumHeight,
        s_stYolov1SoftwareParam.u32NmsThresh,
        s_stYolov1SoftwareParam.u32ConfThresh);

    *model = self;
    return 0;

    FAIL_0:
        SAMPLE_SVP_NNIE_Yolov1_Deinit(&s_stYolov1NnieParam, &s_stYolov1SoftwareParam, &s_stYolov1Model);
        *model = NULL;
        return -1;
}

/*
    destroy yolo1 model
*/
void Yolo1Destroy(SAMPLE_SVP_NNIE_CFG_S *self)
{
    SAMPLE_SVP_NNIE_Yolov1_Deinit(&s_stYolov1NnieParam, &s_stYolov1SoftwareParam, &s_stYolov1Model);
    free(self);
}

/*
    fetch result
*/
static void Yolo1FetchRes(
    SVP_BLOB_S *pstDstScore, SVP_BLOB_S *pstDstRoi, SVP_BLOB_S *pstClassRoiNum,
    HI_FLOAT f32PrintResultThresh,
    DetectObjInfo resBuf[], int resSize, int* resLen)
{
    HI_U32 i;
    HI_U32 j;
    HI_U32 u32RoiNumBias = 0;
    HI_U32 u32ScoreBias;
    HI_U32 u32BboxBias;
    HI_FLOAT f32Score;
    HI_S32* ps32Score = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32, pstDstScore->u64VirAddr);
    HI_S32* ps32Roi = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32, pstDstRoi->u64VirAddr);
    HI_S32* ps32ClassRoiNum = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32, pstClassRoiNum->u64VirAddr);
    HI_U32 u32ClassNum = pstClassRoiNum->unShape.stWhc.u32Width;

    // yolo1_face只有2类，实际为1类
    /* print result, this sample has 21 classes:
     class 0:background     class 1:plane           class 2:bicycle
     class 3:bird           class 4:boat            class 5:bottle
     class 6:bus            class 7:car             class 8:cat
     class 9:chair          class10:cow             class11:diningtable
     class 12:dog           class13:horse           class14:motorbike
     class 15:person        class16:pottedplant     class17:sheep
     class 18:sofa          class19:train           class20:tvmonitor */
    assert(u32ClassNum == 2); // 2: the size of ClassNum
    assert(resSize > 0);
    int resId = 0;
    *resLen = 0;

    if (memset_s(resBuf, resSize * sizeof(resBuf[0]), 0x00, resSize * sizeof(resBuf[0])) != EOK) {
        HI_ASSERT(0);
    }

    u32RoiNumBias += ps32ClassRoiNum[0];
    for (i = 1; i < u32ClassNum; i++) {
        u32ScoreBias = u32RoiNumBias;
        u32BboxBias = u32RoiNumBias * SAMPLE_SVP_NNIE_COORDI_NUM;

        // if the confidence score greater than result threshold, the result will be printed
        if ((HI_FLOAT)ps32Score[u32ScoreBias] / SAMPLE_SVP_NNIE_QUANT_BASE >=
            f32PrintResultThresh && ps32ClassRoiNum[i] != 0) {
        }

        for (j = 0; j < (HI_U32)ps32ClassRoiNum[i]; j++) {
            f32Score = (HI_FLOAT)ps32Score[u32ScoreBias + j] / SAMPLE_SVP_NNIE_QUANT_BASE;
            if (f32Score < f32PrintResultThresh) {
                break;
            }
            if (resId >= resSize) {
                LOGE("yolo1 resBuf full\n");
                break;
            }

            resBuf[resId].cls = 1; // class 1
            resBuf[resId].score = f32Score;

            RectBox *box = &resBuf[resId].box;
            box->xmin = ps32Roi[u32BboxBias + j * SAMPLE_SVP_NNIE_COORDI_NUM];
            box->ymin = ps32Roi[u32BboxBias + j * SAMPLE_SVP_NNIE_COORDI_NUM + 1];
            box->xmax = ps32Roi[u32BboxBias + j * SAMPLE_SVP_NNIE_COORDI_NUM + 2]; // 2：数组下标组成
            box->ymax = ps32Roi[u32BboxBias + j * SAMPLE_SVP_NNIE_COORDI_NUM + 3]; // 3: 数组下标组成
            if (box->xmin >= box->xmax || box->ymin >= box->ymax) {
                LOGD("yolo1_orig: {%d, %d, %d, %d}, %f, discard for coord ERR\n",
                    box->xmin, box->ymin, box->xmax, box->ymax, f32Score);
            } else {
                LOGD("yolo1_orig: {%d, %d, %d, %d}, %f\n",
                    box->xmin, box->ymin, box->xmax, box->ymax, f32Score);
                resId++;
            }
        }
        u32RoiNumBias += ps32ClassRoiNum[i];
    }
    *resLen = resId;
}

/*
    dump result
*/
void Yolo1DumpRes(SVP_BLOB_S *pstDstScore,
    SVP_BLOB_S *pstDstRoi, SVP_BLOB_S *pstClassRoiNum, HI_FLOAT f32PrintResultThresh)
{
    HI_U32 i;
    HI_U32 j;
    HI_U32 u32RoiNumBias = 0;
    HI_U32 u32ScoreBias;
    HI_U32 u32BboxBias;
    HI_FLOAT f32Score;
    HI_S32* ps32Score = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32, pstDstScore->u64VirAddr);
    HI_S32* ps32Roi = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32, pstDstRoi->u64VirAddr);
    HI_S32* ps32ClassRoiNum = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32, pstClassRoiNum->u64VirAddr);
    HI_U32 u32ClassNum = pstClassRoiNum->unShape.stWhc.u32Width;
    HI_S32 s32XMin;
    HI_S32 s32YMin;
    HI_S32 s32XMax;
    HI_S32 s32YMax;

    /* print result, this sample has 21 classes:
     class 0:background     class 1:plane           class 2:bicycle
     class 3:bird           class 4:boat            class 5:bottle
     class 6:bus            class 7:car             class 8:cat
     class 9:chair          class10:cow             class11:diningtable
     class 12:dog           class13:horse           class14:motorbike
     class 15:person        class16:pottedplant     class17:sheep
     class 18:sofa          class19:train           class20:tvmonitor */
    u32RoiNumBias += ps32ClassRoiNum[0];
    for (i = 1; i < u32ClassNum; i++) {
        u32ScoreBias = u32RoiNumBias;
        u32BboxBias = u32RoiNumBias * SAMPLE_SVP_NNIE_COORDI_NUM;
        // if the confidence score greater than result threshold, the result will be printed
        if ((HI_FLOAT)ps32Score[u32ScoreBias] / SAMPLE_SVP_NNIE_QUANT_BASE >=
            f32PrintResultThresh && ps32ClassRoiNum[i] != 0) {
            LOGD("yolo1_face detect %d\n", (int)ps32ClassRoiNum[i]);
        }
        for (j = 0; j < (HI_U32)ps32ClassRoiNum[i]; j++) {
            f32Score = (HI_FLOAT)ps32Score[u32ScoreBias + j] / SAMPLE_SVP_NNIE_QUANT_BASE;
            if (f32Score < f32PrintResultThresh) {
                break;
            }
            s32XMin = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM];
            s32YMin = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 1];
            s32XMax = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 2]; // 2：数组下标组成
            s32YMax = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 3]; // 3: 数组下标组成
            LOGD("yolo1_face: %f, {%d, %d, %d, %d}\n", f32Score, s32XMin, s32YMin, s32XMax, s32YMax);
        }
        u32RoiNumBias += ps32ClassRoiNum[i];
    }
}

/*
    计算一张U8C3 image
*/
int Yolo1CalImg(SAMPLE_SVP_NNIE_CFG_S* self,
    const IVE_IMAGE_S *img, float thresh, DetectObjInfo resBuf[], int resSize, int* resLen)
{
    thresh = thresh < 0 ? 0.025f : thresh;
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};
    HI_S32 s32Ret;

    self->pszPic = NULL;
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;

    s32Ret = FillNnieByImg(self, &s_stYolov1NnieParam, 0, 0, img);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    // NNIE process(process the 0-th segment)
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stYolov1NnieParam, &stInputDataIdx, &stProcSegIdx, HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

    // software process
    // if user has changed net struct, please make sure SAMPLE_SVP_NNIE_Yolov1_GetResult
    // function input datas are correct
    s32Ret = SAMPLE_SVP_NNIE_Yolov1_GetResult(&s_stYolov1NnieParam, &s_stYolov1SoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Yolov1_GetResult failed!\n");

    Yolo1FetchRes(&s_stYolov1SoftwareParam.stDstScore,
        &s_stYolov1SoftwareParam.stDstRoi, &s_stYolov1SoftwareParam.stClassRoiNum,
        thresh, resBuf, resSize, resLen);
    return 0;

    FAIL_0:
        return -1;
}


/******************************************************************************
* function : Yolov2 software deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Yolov2_SoftwareDeinit(SAMPLE_SVP_NNIE_YOLOV2_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_CHECK_EXPR_RET(NULL == pstSoftWareParam, HI_INVALID_VALUE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error, pstSoftWareParam can't be NULL!\n");
    if (pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr != 0 && pstSoftWareParam->stGetResultTmpBuf.u64VirAddr != 0) {
        SAMPLE_SVP_MMZ_FREE(pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr,
            pstSoftWareParam->stGetResultTmpBuf.u64VirAddr);
        pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr = 0;
        pstSoftWareParam->stGetResultTmpBuf.u64VirAddr = 0;
        pstSoftWareParam->stDstRoi.u64PhyAddr = 0;
        pstSoftWareParam->stDstRoi.u64VirAddr = 0;
        pstSoftWareParam->stDstScore.u64PhyAddr = 0;
        pstSoftWareParam->stDstScore.u64VirAddr = 0;
        pstSoftWareParam->stClassRoiNum.u64PhyAddr = 0;
        pstSoftWareParam->stClassRoiNum.u64VirAddr = 0;
    }
    return s32Ret;
}

/******************************************************************************
* function : Yolov2 Deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Yolov2_Deinit(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_YOLOV2_SOFTWARE_PARAM_S* pstSoftWareParam, SAMPLE_SVP_NNIE_MODEL_S *pstNnieModel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /* hardware deinit */
    if (pstNnieParam != NULL) {
        s32Ret = SAMPLE_COMM_SVP_NNIE_ParamDeinit(pstNnieParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_COMM_SVP_NNIE_ParamDeinit failed!\n");
    }
    /* software deinit */
    if (pstSoftWareParam != NULL) {
        s32Ret = SAMPLE_SVP_NNIE_Yolov2_SoftwareDeinit(pstSoftWareParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_Yolov2_SoftwareDeinit failed!\n");
    }
    /* model deinit */
    if (pstNnieModel != NULL) {
        s32Ret = SAMPLE_COMM_SVP_NNIE_UnloadModel(pstNnieModel);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_COMM_SVP_NNIE_UnloadModel failed!\n");
    }
    return s32Ret;
}

/******************************************************************************
* function : Yolov2 software para init
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Yolov2_SoftwareInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_YOLOV2_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret;
    HI_U32 u32ClassNum = 0;
    HI_U32 u32BboxNum;
    HI_U32 u32TotalSize = 0;
    HI_U32 u32DstRoiSize;
    HI_U32 u32DstScoreSize;
    HI_U32 u32ClassRoiNumSize;
    HI_U32 u32TmpBufTotalSize;
    HI_U64 u64PhyAddr = 0;
    HI_U8* pu8VirAddr = NULL;

    pstSoftWareParam->u32OriImHeight = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Height;
    pstSoftWareParam->u32OriImWidth = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Width;
    pstSoftWareParam->u32BboxNumEachGrid = 5;
    pstSoftWareParam->u32ClassNum = 1;
    pstSoftWareParam->u32GridNumHeight = 12;
    pstSoftWareParam->u32GridNumWidth = 20;
    pstSoftWareParam->u32NmsThresh = (HI_U32)(0.3f*SAMPLE_SVP_NNIE_QUANT_BASE);
    pstSoftWareParam->u32ConfThresh = (HI_U32)(0.25f*SAMPLE_SVP_NNIE_QUANT_BASE);
    pstSoftWareParam->u32MaxRoiNum = 10;
    pstSoftWareParam->af32Bias[0] = 0.52;
    pstSoftWareParam->af32Bias[1] = 0.61;
    pstSoftWareParam->af32Bias[2] = 1.05;
    pstSoftWareParam->af32Bias[3] = 1.12;
    pstSoftWareParam->af32Bias[4] = 1.85;
    pstSoftWareParam->af32Bias[5] = 2.05;
    pstSoftWareParam->af32Bias[6] = 4.63;
    pstSoftWareParam->af32Bias[7] = 4.49;
    pstSoftWareParam->af32Bias[8] = 7.15;
    pstSoftWareParam->af32Bias[9] = 7.56;

    /* Malloc assist buffer memory */
    u32ClassNum = pstSoftWareParam->u32ClassNum + 1;
    u32BboxNum = pstSoftWareParam->u32BboxNumEachGrid*pstSoftWareParam->u32GridNumHeight*
        pstSoftWareParam->u32GridNumWidth;
    u32TmpBufTotalSize = SAMPLE_SVP_NNIE_Yolov2_GetResultTmpBuf(pstNnieParam, pstSoftWareParam);
    u32DstRoiSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*u32BboxNum*sizeof(HI_U32)*SAMPLE_SVP_NNIE_COORDI_NUM);
    u32DstScoreSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*u32BboxNum*sizeof(HI_U32));
    u32ClassRoiNumSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    u32TotalSize = u32TotalSize+u32DstRoiSize+u32DstScoreSize+u32ClassRoiNumSize + u32TmpBufTotalSize;
    s32Ret = SAMPLE_COMM_SVP_MallocCached("SAMPLE_YOLOV2_INIT", NULL, (HI_U64*)&u64PhyAddr,
        (void**)&pu8VirAddr,u32TotalSize);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret, s32Ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,Malloc memory failed!\n");
    memset(pu8VirAddr, 0, u32TotalSize);
    SAMPLE_COMM_SVP_FlushCache(u64PhyAddr, (void*)pu8VirAddr, u32TotalSize);

   /* set each tmp buffer addr */
    pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr = u64PhyAddr;
    pstSoftWareParam->stGetResultTmpBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr);

    /* set result blob */
    pstSoftWareParam->stDstRoi.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstRoi.u64PhyAddr = u64PhyAddr + u32TmpBufTotalSize;
    pstSoftWareParam->stDstRoi.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr + u32TmpBufTotalSize);
    pstSoftWareParam->stDstRoi.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*
        u32BboxNum*sizeof(HI_U32)*SAMPLE_SVP_NNIE_COORDI_NUM);
    pstSoftWareParam->stDstRoi.u32Num = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Width = u32ClassNum*
        u32BboxNum*SAMPLE_SVP_NNIE_COORDI_NUM;

    pstSoftWareParam->stDstScore.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstScore.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize + u32DstRoiSize;
    pstSoftWareParam->stDstScore.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32TmpBufTotalSize + u32DstRoiSize);
    pstSoftWareParam->stDstScore.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*
        u32BboxNum*sizeof(HI_U32));
    pstSoftWareParam->stDstScore.u32Num = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Width = u32ClassNum*u32BboxNum;

    pstSoftWareParam->stClassRoiNum.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stClassRoiNum.u64PhyAddr = u64PhyAddr + u32TmpBufTotalSize+
        u32DstRoiSize + u32DstScoreSize;
    pstSoftWareParam->stClassRoiNum.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr + u32TmpBufTotalSize +
        u32DstRoiSize + u32DstScoreSize);
    pstSoftWareParam->stClassRoiNum.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    pstSoftWareParam->stClassRoiNum.u32Num = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Width = u32ClassNum;

    return s32Ret;
}

/******************************************************************************
* function : Yolov1 init
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Yolov2_ParamInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_YOLOV2_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret;
    /* init hardware para */
    s32Ret = SAMPLE_COMM_SVP_NNIE_ParamInit(pstCfg, pstNnieParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, INIT_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_COMM_SVP_NNIE_ParamInit failed!\n", s32Ret);

    /* init software para */
    s32Ret = SAMPLE_SVP_NNIE_Yolov2_SoftwareInit(pstCfg, pstNnieParam,
        pstSoftWareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, INIT_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_SVP_NNIE_Yolov1_SoftwareInit failed!\n", s32Ret);
    return s32Ret;
INIT_FAIL_0:
    s32Ret = SAMPLE_SVP_NNIE_Yolov2_Deinit(pstNnieParam, pstSoftWareParam,NULL);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret, s32Ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_SVP_NNIE_Yolov1_Deinit failed!\n", s32Ret);
    return HI_FAILURE;

}

/******************************************************************************
* function : creat yolo2 model basad mode file
******************************************************************************/
int Yolo2Create(SAMPLE_SVP_NNIE_CFG_S **model, const char* modelFile)
{
    SAMPLE_SVP_NNIE_CFG_S *self;
    HI_U32 u32PicNum = 1;
    HI_S32 s32Ret;

    self = (SAMPLE_SVP_NNIE_CFG_S*)malloc(sizeof(*self));
    assert(self);
    memset(self, 0x00, sizeof(*self));

    // Set configuration parameter
    self->pszPic = NULL;
    self->u32MaxInputNum = u32PicNum; // max input image num in each batch
    self->u32MaxRoiNum = 0;
    self->aenNnieCoreId[0] = SVP_NNIE_ID_0; // set NNIE core
 
    // Yolov2 Load model
    SAMPLE_SVP_TRACE_INFO("Yolov2 Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel((char*)modelFile, &s_stYolov2Model);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, YOLOV2_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error, SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /* Yolov2 parameter initialization */
    /* Yolov2 software parameters are set in SAMPLE_SVP_NNIE_Yolov2_SoftwareInit,
      if user has changed net struct, please make sure the parameter settings in
      SAMPLE_SVP_NNIE_Yolov2_SoftwareInit function are correct */
    SAMPLE_SVP_TRACE_INFO("Yolov2 parameter initialization!\n");
    s_stYolov2NnieParam.pstModel = &s_stYolov2Model.stModel;
    s32Ret = SAMPLE_SVP_NNIE_Yolov2_ParamInit(self, &s_stYolov2NnieParam, &s_stYolov2SoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, YOLOV2_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Yolov2_ParamInit failed!\n");

    // model important info
    LOGI("model.base={ type=%x, frmNum=%u, chnNum=%u, w=%u, h=%u, stride=%u }\n",
        s_stYolov2NnieParam.astSegData[0].astSrc[0].enType,
        s_stYolov2NnieParam.astSegData[0].astSrc[0].u32Num,
        s_stYolov2NnieParam.astSegData[0].astSrc[0].unShape.stWhc.u32Chn,
        s_stYolov2NnieParam.astSegData[0].astSrc[0].unShape.stWhc.u32Width,
        s_stYolov2NnieParam.astSegData[0].astSrc[0].unShape.stWhc.u32Height,
        s_stYolov2NnieParam.astSegData[0].astSrc[0].u32Stride);
    LOGI("model.soft={ class=%u, ori.w=%u, ori.h=%u, bnum=%u, grid.w=%u, grid.h=%u, nmsThresh=%u, confThresh=%u, u32MaxRoiNum=%u }\n",
        s_stYolov2SoftwareParam.u32ClassNum,
        s_stYolov2SoftwareParam.u32OriImWidth,
        s_stYolov2SoftwareParam.u32OriImHeight,
        s_stYolov2SoftwareParam.u32BboxNumEachGrid,
        s_stYolov2SoftwareParam.u32GridNumWidth,
        s_stYolov2SoftwareParam.u32GridNumHeight,
        s_stYolov2SoftwareParam.u32NmsThresh,
        s_stYolov2SoftwareParam.u32ConfThresh,
        s_stYolov2SoftwareParam.u32MaxRoiNum);

    *model = self;
    return 0;

    YOLOV2_FAIL_0:
        LOGI("Yolo2Create SAMPLE_SVP_NNIE_Yolov2_Deinit\n");
        SAMPLE_SVP_NNIE_Yolov2_Deinit(&s_stYolov2NnieParam, &s_stYolov2SoftwareParam, &s_stYolov2Model);
        *model = NULL;
        return -1;
}

/******************************************************************************
* function : destory yolo2 model
******************************************************************************/
void Yolo2Destory(SAMPLE_SVP_NNIE_CFG_S *self)
{
    SAMPLE_SVP_NNIE_Yolov2_Deinit(&s_stYolov2NnieParam, &s_stYolov2SoftwareParam, &s_stYolov2Model);
    SAMPLE_COMM_SVP_CheckSysExit();
    free(self);
}

/******************************************************************************
* function : fetch result
******************************************************************************/
static void Yolo2FetchRes(
    SVP_BLOB_S *pstDstScore, SVP_BLOB_S *pstDstRoi, SVP_BLOB_S *pstClassRoiNum, 
    HI_FLOAT f32PrintResultThresh,
    DetectObjInfo resBuf[], int resSize, int* resLen)
{
    HI_U32 i;
    HI_U32 j;
    HI_U32 u32RoiNumBias = 0;
    HI_U32 u32ScoreBias = 0;
    HI_U32 u32BboxBias = 0;
    HI_FLOAT f32Score = 0.0f;
    HI_S32* ps32Score = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstDstScore->u64VirAddr);
    HI_S32* ps32Roi = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstDstRoi->u64VirAddr);
    HI_S32* ps32ClassRoiNum = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstClassRoiNum->u64VirAddr);
    HI_U32 u32ClassNum = pstClassRoiNum->unShape.stWhc.u32Width;

    assert(u32ClassNum == 2);
    assert(resSize > 0);
    int resId = 0;
    *resLen = 0;
    memset(resBuf, 0x00, resSize * sizeof(resBuf[0]));

    u32RoiNumBias += ps32ClassRoiNum[0];
    for (i = 1; i < u32ClassNum; i++) {
        u32ScoreBias = u32RoiNumBias;
        u32BboxBias = u32RoiNumBias * SAMPLE_SVP_NNIE_COORDI_NUM;
        /* if the confidence score greater than result threshold, the result will be printed */
        if((HI_FLOAT)ps32Score[u32ScoreBias] / SAMPLE_SVP_NNIE_QUANT_BASE >=
            f32PrintResultThresh && ps32ClassRoiNum[i] != 0) {
        }
        for (j = 0; j < (HI_U32)ps32ClassRoiNum[i]; j++) {
            f32Score = (HI_FLOAT)ps32Score[u32ScoreBias + j] / SAMPLE_SVP_NNIE_QUANT_BASE;
            if (f32Score < f32PrintResultThresh) {
                LOGI("f32Score:%.2f\n", f32Score);
                break;
            }
            if (resId >= resSize) {
                LOGE("yolo2 resBuf full\n");
                break;
            }
            resBuf[resId].cls = 1; // class 1
            resBuf[resId].score = f32Score;

            RectBox *box = &resBuf[resId].box;
            box->xmin = ps32Roi[u32BboxBias + j * SAMPLE_SVP_NNIE_COORDI_NUM];
            box->ymin = ps32Roi[u32BboxBias + j * SAMPLE_SVP_NNIE_COORDI_NUM + 1];
            box->xmax = ps32Roi[u32BboxBias + j * SAMPLE_SVP_NNIE_COORDI_NUM + 2];
            box->ymax = ps32Roi[u32BboxBias + j * SAMPLE_SVP_NNIE_COORDI_NUM + 3];
            if (box->xmin >= box->xmax || box->ymin >= box->ymax) {
                LOGD("yolo1_orig: {%d, %d, %d, %d}, %f, discard for coord ERR\n", 
                    box->xmin, box->ymin, box->xmax, box->ymax, f32Score);
            } else {
                LOGD("yolo1_orig: {%d, %d, %d, %d}, %f\n", 
                    box->xmin, box->ymin, box->xmax, box->ymax, f32Score);
                resId++;
            }
        }
        u32RoiNumBias += ps32ClassRoiNum[i];
    }

    *resLen = resId;
}

/******************************************************************************
* function : cal U8C3 image
******************************************************************************/
int Yolo2CalImg(SAMPLE_SVP_NNIE_CFG_S* self, 
    const IVE_IMAGE_S *img, float thresh, DetectObjInfo resBuf[], int resSize, int* resLen)
{
    thresh = thresh < 0 ? 0.025f : thresh;
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};
    HI_S32 s32Ret;

    // Fill src data
    self->pszPic = NULL;
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;

    s32Ret = FillNnieByImg(self, &s_stYolov2NnieParam, 0, 0, img);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, YOLOV2_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    // NNIE process(process the 0-th segment)
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stYolov2NnieParam, &stInputDataIdx, &stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, YOLOV2_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");
    
    /* Software process */
    /* if user has changed net struct, please make sure SAMPLE_SVP_NNIE_Yolov2_GetResult
     function input datas are correct */
    s32Ret = SAMPLE_SVP_NNIE_Yolov2_GetResult(&s_stYolov2NnieParam,&s_stYolov2SoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV2_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Yolov2_GetResult failed!\n");

    Yolo2FetchRes(&s_stYolov2SoftwareParam.stDstScore,
        &s_stYolov2SoftwareParam.stDstRoi, &s_stYolov2SoftwareParam.stClassRoiNum,
        thresh, resBuf, resSize, resLen);
    return 0;

    YOLOV2_FAIL_0:
        return -1;
}