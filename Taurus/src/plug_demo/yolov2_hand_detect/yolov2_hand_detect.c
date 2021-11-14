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
#include <errno.h>

#include "sample_comm_nnie.h"
#include "nnie_sample_plug.h"

#include "hi_ext_util.h"
#include "mpp_help.h"
#include "ai_plug.h"

#define PLUG_UUID          "\"hi.yolov2_hand_detect\""
#define PLUG_DESC          "\"手部检测(yolov2)\""     // UTF8 encode

#define FRM_WIDTH          640
#define FRM_HEIGHT         384
#define MODEL_FILE_HAND    "./plugs/darknetYolo.wk" // darknet framework wk model

#define THRESH_MIN         0.25
#define DETECT_OBJ_MAX     32 // todo:need test
#define PIRIOD_NUM_MAX     49 // Logs are printed when the number of targets is detected
#define DRAW_RETC_THICK    2  // Draw the width of the line

static uintptr_t g_handModel = 0;

static const char YOLO2_FD_PROF[] = "{"
    "\"uuid\": " PLUG_UUID ","
    "\"desc\": " PLUG_DESC ","
    "\"frmWidth\": " HI_TO_STR(FRM_WIDTH) ","
    "\"frmHeight\": " HI_TO_STR(FRM_HEIGHT) ","
    "\"butt\": 0"
"}";

static const char* Yolo2FdProf(void)
{
    return YOLO2_FD_PROF;
}

static HI_S32 Yolo2FdLoad(uintptr_t* model, OsdSet* osds)
{
    SAMPLE_SVP_NNIE_CFG_S *self = NULL;
    HI_S32 ret;

    ret = Yolo2Create(&self, MODEL_FILE_HAND);
    *model = ret < 0 ? 0 : (uintptr_t)self;
    LOGI("Yolo2FdLoad ret:%d\n", ret);
    return ret;
}

HI_S32 HandDetectInit()
{
    return Yolo2FdLoad(&g_handModel, NULL);
}

static HI_S32 Yolo2FdUnload(uintptr_t model)
{
    Yolo2Destory((SAMPLE_SVP_NNIE_CFG_S*)model);
    return 0;
}

HI_S32 HandDetectExit()
{
    return Yolo2FdUnload(g_handModel);
}

//NNIE推理接口函数
static HI_S32 Yolo2FdCal(uintptr_t model,
    VIDEO_FRAME_INFO_S *srcFrm, VIDEO_FRAME_INFO_S *dstFrm, HI_CHAR** resJson)
{
    SAMPLE_SVP_NNIE_CFG_S *self = (SAMPLE_SVP_NNIE_CFG_S*)model; // reference to SDK sample_comm_nnie.h
    IVE_IMAGE_S img; // referece to SDK hi_comm_ive.h
    DetectObjInfo objs[DETECT_OBJ_MAX] = {0};
    RectBox boxs[DETECT_OBJ_MAX] = {0};
    int objNum;
    int ret;

    // performance statistical variable
    static int64_t yuv2RgbCost = 0;
    static int64_t calCost = 0;
    static int piriodNum = 0;
    int64_t begTime;

    // YUV to RGB
    begTime = HiClockMs();
    ret = FrmToOrigImg((VIDEO_FRAME_INFO_S*)srcFrm, &img);
    HI_EXP_RET(ret != HI_SUCCESS, ret, "hand_detect_cal FAIL, for YUV2RGB FAIL, ret=%#x\n", ret);
    yuv2RgbCost += (HiClockMs() - begTime);

    // calculate
    begTime = HiClockMs();
    ret = Yolo2CalImg(self, &img, THRESH_MIN, objs, HI_ARRAY_SIZE(objs), &objNum);
    IveImgDestroy(&img);
    HI_EXP_RET(ret < 0, ret, "hand_detect_cal FAIL, for cal FAIL, ret=%d\n", ret);
    calCost += (HiClockMs() - begTime);
    LOGI("objNum:%d\n", objNum);
    for (int i = 0; i < objNum; i++) {
        RectBox *box = &objs[i].box;
        RectBoxTran(box, FRM_WIDTH, FRM_HEIGHT,
            dstFrm->stVFrame.u32Width, dstFrm->stVFrame.u32Height);
        LOGI("yolo2_out: {%d, %d, %d, %d}\n",
            box->xmin, box->ymin, box->xmax, box->ymax);


        boxs[i] = *box;
    }

    // The result of the package calculation is resJSON
    *resJson = DetectObjsToJson(objs, objNum, NULL);

    // Log performance statistics
    if (++piriodNum > PIRIOD_NUM_MAX) {
        LOGD("yolo: num=%d, pcvt=%jd, pcal=%jd\n",
            piriodNum, yuv2RgbCost / piriodNum, calCost / piriodNum);
        yuv2RgbCost = 0;
        calCost = 0;
        piriodNum = 0;
    }

    // When an object is detected, a rectangle is drawn in the DSTFRM
    if (objNum > 0) {
        MppFrmDrawRects(dstFrm, boxs, objNum, RGB888_RED, DRAW_RETC_THICK);
    }

    return ret;
}

static HI_S32 HandDetect(uintptr_t model, IVE_IMAGE_S *srcYuv, DetectObjInfo boxs[])
{
    SAMPLE_SVP_NNIE_CFG_S *self = (SAMPLE_SVP_NNIE_CFG_S*)model;
    int objNum;
    int ret;
    ret = Yolo2CalImg(self, srcYuv, THRESH_MIN, boxs, DETECT_OBJ_MAX, &objNum);
    HI_EXP_RET(ret < 0, ret, "Hand detect Yolo2CalImg FAIL, for cal FAIL\n");

    return objNum;
}

HI_S32 HandDetectCal(IVE_IMAGE_S *srcYuv, DetectObjInfo resArr[])
{
    int ret;
    ret = HandDetect(g_handModel, srcYuv, resArr);
    return ret;
}

static const AiPlug G_HAND_DETECT_ITF = {
    .Prof = Yolo2FdProf,
    .Load = Yolo2FdLoad,
    .Unload = Yolo2FdUnload,
    .Cal = Yolo2FdCal,
};