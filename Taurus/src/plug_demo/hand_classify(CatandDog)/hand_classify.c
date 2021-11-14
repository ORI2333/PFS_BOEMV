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
#include "yolov2_hand_detect.h"

#include "hi_ext_util.h"
#include "mpp_help.h"
#include "ai_plug.h"
#include "ive_img.h"
#include "audio_test.h"

#include "hisignalling.h"

#define PLUG_UUID          "\"hi.hand_classify\""
#define PLUG_DESC          "\"猫狗检测分类(darknet+resnet)\""     // UTF8 encode

#define FRM_WIDTH          640
#define FRM_HEIGHT         384
#define MODEL_FILE_GESTURE    "./plugs/classify.wk" // darknet framework wk model

#define RET_NUM_MAX        4
#define SCORE_MAX          4096
#define THRESH_MIN         0.25
#define DETECT_OBJ_MAX     32
#define PIRIOD_NUM_MAX     49   // Logs are printed when the number of targets is detected
#define DRAW_RETC_THICK    2    // Draw the width of the line
#define IMAGE_WIDTH		   224  // The resolution of the model IMAGE sent to the classification is 224*224
#define IMAGE_HEIGHT	   224
#define OSD_FONT_WIDTH	   16
#define OSD_FONT_HEIGHT	   24
#define GESTURE_MIN        30   // confidence threshold
#define WIDTH_LIMIT        32
#define HEIGHT_LIMIT       32

#define AUDIO_CASE_TWO     2
#define AUDIO_SCORE        90 		// 置信度可自行配置
#define AUDIO_FRAME        14 		// 每隔15帧识别一次，可自行配置

static OsdSet* g_osdsGesture = NULL;
static HI_S32 g_osd0Gesture = -1;

static int g_num = 108;
static int g_count = 0;
static pthread_t g_thrdId = 0;
static int g_supportAudio = 0;

double sum = 0;
double New_Num = 0;   

static HI_CHAR *gesture_name = NULL;

int Flag1 = 0;

static int uart_fd ;
static SkPair g_stmChn = {
    .in = -1,
    .out = -1
};

static void PlayAudio(const RecogNumInfo items)
{
	if (g_count < AUDIO_FRAME) {
		g_count++;
		return;
	}

	const RecogNumInfo *item = &items;
	uint32_t score = item->score * 100 / 4096;
	if ((score > AUDIO_SCORE) && (g_num != item->num)) {
		g_num = item->num;
		audio_test(AUDIO_CASE_TWO, g_num, -1);
	}
	g_count = 0;
}

static void* GetAudioFileName(void* arg)
{
	RecogNumInfo resBuf = {0};
	int ret;

	while(1) {
		ret = FdReadMsg(g_stmChn.in, &resBuf, sizeof(RecogNumInfo));
		if (ret == sizeof(RecogNumInfo)) {
			PlayAudio(resBuf);
		}
	}
}



static const char YOLO2_HAND_DETECT_RESNET_CLASSIFY[] = "{"
    "\"uuid\": " PLUG_UUID ","
    "\"desc\": " PLUG_DESC ","
    "\"frmWidth\": " HI_TO_STR(FRM_WIDTH) ","
    "\"frmHeight\": " HI_TO_STR(FRM_HEIGHT) ","
    "\"butt\": 0"
"}";

static const char* Yolo2HandDetectResnetClassifyProf(void)
{
    return YOLO2_HAND_DETECT_RESNET_CLASSIFY;
}

static HI_S32 Yolo2HandDetectResnetClassifyLoad(uintptr_t* model, OsdSet* osds)
{
    SAMPLE_SVP_NNIE_CFG_S *self = NULL;
    HI_S32 ret;

    g_osdsGesture = osds;
    HI_ASSERT(g_osdsGesture);
    g_osd0Gesture = OsdsCreateRgn(g_osdsGesture);
    HI_ASSERT(g_osd0Gesture >= 0);

    ret = CnnCreate(&self, MODEL_FILE_GESTURE);
    *model = ret < 0 ? 0 : (uintptr_t)self;
    HandDetectInit(); // Initialize the hand detection model

    if (GetCfgBool("audio_player:support_audio", true)) {
		ret = SkPairCreate(&g_stmChn);
		HI_ASSERT(ret == 0);
		if (pthread_create(&g_thrdId, NULL, GetAudioFileName, NULL) < 0) {
			HI_ASSERT(0);
		}
		g_supportAudio = 1;
	}



    uart_fd =  uartOpenInit();//qi dong  UART
    if (uart_fd < 0) {
        LOGI("uart open failed");
    } else {
        LOGI("uart open successed\n");
    }

    return ret;
}

static HI_S32 Yolo2HandDetectResnetClassifyUnload(uintptr_t model)
{
    CnnDestroy((SAMPLE_SVP_NNIE_CFG_S*)model);
    if (g_osdsGesture) {
        OsdsClear(g_osdsGesture);
        g_osdsGesture = NULL;
    }

    if (g_supportAudio == 1) {
		SkPairDestroy(&g_stmChn);
		pthread_join(g_thrdId, NULL);
	}

    HandDetectExit(); // Uninitialize the hand detection model

    return 0;
}

/**
	Get the maximum hand
*/
static HI_S32 GetBiggestHandIndex(RectBox boxs[], int detectNum)
{
    HI_S32 handIndex = 0;
    HI_S32 biggestBoxIndex = handIndex;
    HI_S32 biggestBoxWidth = boxs[handIndex].xmax - boxs[handIndex].xmin + 1;
    HI_S32 biggestBoxHeight = boxs[handIndex].ymax - boxs[handIndex].ymin + 1;
    HI_S32 biggestBoxArea = biggestBoxWidth * biggestBoxHeight;

    for (handIndex = 1; handIndex < detectNum; handIndex++) {
        HI_S32 boxWidth = boxs[handIndex].xmax - boxs[handIndex].xmin + 1;
        HI_S32 boxHeight = boxs[handIndex].ymax - boxs[handIndex].ymin + 1;
        HI_S32 boxArea = boxWidth * boxHeight;
        if (biggestBoxArea < boxArea) {
            biggestBoxArea = boxArea;
            biggestBoxIndex = handIndex;
        }
        biggestBoxWidth = boxs[biggestBoxIndex].xmax - boxs[biggestBoxIndex].xmin + 1;
        biggestBoxHeight = boxs[biggestBoxIndex].ymax - boxs[biggestBoxIndex].ymin + 1;
    }

    if ((biggestBoxWidth == 1) || (biggestBoxHeight == 1) || (detectNum == 0)) {
        biggestBoxIndex = -1;
    }

    return biggestBoxIndex;
}



/**
    Add gesture recognition information next to the rectangle
*/
static void HandDetectAddTxt(const RectBox box, const RecogNumInfo resBuf, uint32_t color)
{
    HI_OSD_ATTR_S osdRgn;
    char osdTxt[TINY_BUF_SIZE]; 
    HI_ASSERT(g_osdsGesture);
    Flag1++;
    sum += resBuf.num;  
    if(Flag1 == 3){
    New_Num = sum/3;
    if(New_Num < 0.9){
        gesture_name = "Cat";
        //usbUartSendRead(uart_fd, CAT);
         // usleep(100*1000);
    }else{
        gesture_name = "Gog";
        //usbUartSendRead(uart_fd, DOG);
       // usleep(100*1000);
    }   
    // switch (resBuf.num) {
    //     case 0u:
    //         gesture_name = "gesture cat";
    //         usbUartSendRead(uart_fd, CAT);
    //         break;
    //     case 1u:
    //         gesture_name = "gesture gog";
    //         usbUartSendRead(uart_fd, DOG);
    //         usleep(100*1000);
    //         break;
    //     default:
    //         gesture_name = "background";
    //         usbUartSendRead(uart_fd, UNKNOWN);
    //         usleep(100*1000);
    //         break;
    // }
    uint32_t score = (resBuf.score) * HI_PER_BASE / SCORE_MAX;
	int res = snprintf_s(osdTxt, sizeof(osdTxt), sizeof(osdTxt) - 1, "%.2f_%s,%d %%",New_Num, gesture_name, score);
	HI_ASSERT(res > 0);
	int osdId = OsdsCreateRgn(g_osdsGesture);
	HI_ASSERT(osdId >= 0);

    int x = box.xmin / HI_OVEN_BASE * HI_OVEN_BASE;
    int y = (box.ymin - 30) / HI_OVEN_BASE * HI_OVEN_BASE; // 30: empirical value
    if (y < 0) {
        LOGD("osd_y < 0, y=%d\n", y);
        OsdsDestroyRgn(g_osdsGesture, osdId);
    } else {
        TxtRgnInit(&osdRgn, osdTxt, x, y, color, OSD_FONT_WIDTH, OSD_FONT_HEIGHT);
        OsdsSetRgn(g_osdsGesture, osdId, &osdRgn);
    }
    Flag1 = 0;
    New_Num = 0;
    sum = 0;
    }
}


/**
    将计算结果打包为resJson.
*/
static HI_CHAR* CnnGestureClassifyToJson(const RecogNumInfo items[], HI_S32 itemNum)
{
    HI_S32 jsonSize = TINY_BUF_SIZE + itemNum * TINY_BUF_SIZE; // 每个item的打包size为TINY_BUF_SIZE
    HI_CHAR *jsonBuf = (HI_CHAR*)malloc(jsonSize);
    HI_ASSERT(jsonBuf);
    HI_S32 offset = 0;

    offset += snprintf_s(jsonBuf + offset, jsonSize - offset, jsonSize - offset - 1, "[");
    for (HI_S32 i = 0; i < itemNum; i++) {
        const RecogNumInfo *item = &items[i];
        uint32_t score = item->score * HI_PER_BASE / SCORE_MAX;
        if (score < GESTURE_MIN) {
            break;
        }
        offset += snprintf_s(jsonBuf + offset, jsonSize - offset, jsonSize - offset - 1,
            "%s{ \"classify num\": %u, \"score\": %u }", (i == 0 ? "\n  " : ", "), (uint)item->num, (uint)score);
        HI_ASSERT(offset < jsonSize);
    }
    offset += snprintf_s(jsonBuf + offset, jsonSize - offset, jsonSize - offset - 1, "]");
    HI_ASSERT(offset < jsonSize);
    return jsonBuf;
}


static HI_S32 Yolo2HandDetectResnetClassifyCal(uintptr_t model,
    VIDEO_FRAME_INFO_S *srcFrm, VIDEO_FRAME_INFO_S *dstFrm, HI_CHAR** resJson)
{
    SAMPLE_SVP_NNIE_CFG_S *self = (SAMPLE_SVP_NNIE_CFG_S*)model;
    IVE_IMAGE_S img;
    DetectObjInfo objs[DETECT_OBJ_MAX] = {0};
    RectBox boxs[DETECT_OBJ_MAX] = {0};
    RectBox objBoxs[DETECT_OBJ_MAX] = {0};
    RectBox remainingBoxs[DETECT_OBJ_MAX] = {0};
    RectBox cnnBoxs[DETECT_OBJ_MAX] = {0}; // Store the results of the classification network
    RecogNumInfo numInfo[RET_NUM_MAX] = {0};
    HI_S32 resLen = 0;
    int objNum;
    int ret;
    int biggestBoxIndex;
    int num = 0;

    OsdsClear(g_osdsGesture);
    ret = FrmToOrigImg((VIDEO_FRAME_INFO_S*)srcFrm, &img);
    HI_EXP_RET(ret != HI_SUCCESS, ret, "hand_detect_cal FAIL, for YUV Frm to Img FAIL, ret=%#x\n", ret);

    objNum = HandDetectCal(&img, objs); // Send IMG to the detection net for reasoning
    for (int i = 0; i < objNum; i++) {
        cnnBoxs[i] = objs[i].box;
        RectBox *box = &objs[i].box;

        RectBoxTran(box, FRM_WIDTH, FRM_HEIGHT,
            dstFrm->stVFrame.u32Width, dstFrm->stVFrame.u32Height);

        LOGI("yolo2_out: {%d, %d, %d, %d}\n",
            box->xmin, box->ymin, box->xmax, box->ymax);
        //xmax MAX为1920，ymax MAX为1080    
        /**
        /* usbUartSendRead(uart_fd, DOG);
        /* usleep(100*1000);
        **/
       if(gesture_name == "Gog"){
       if(box->xmin <= 0){
        usbUartSendRead(uart_fd, 0x04);
        usleep(1000);
       }
       else if(box->xmax >= 1920){
        usbUartSendRead(uart_fd, 0x06);
        usleep(1000);
       }
       else if(box->ymin <= 0){
        usbUartSendRead(uart_fd, 0x03);
        usleep(1000);
       }
       else if(box->ymax >= 1080){
        usbUartSendRead(uart_fd, 0x08);
        usleep(1000);
       }
       else
        usbUartSendRead(uart_fd, 0x02);
        usleep(1000);
       }

       if(gesture_name == "Cat"){
       if(box->xmin <= 0){
        usbUartSendRead(uart_fd, 0x14);
        usleep(1000);
       }
       else if(box->xmax >= 1920){
        usbUartSendRead(uart_fd, 0x16);
        usleep(1000);
       }
       else if(box->ymin <= 0){
        usbUartSendRead(uart_fd, 0x13);
        usleep(1000);
       }
       else if(box->ymax >= 1080){
        usbUartSendRead(uart_fd, 0x18);
        usleep(1000);
       }
       else
        usbUartSendRead(uart_fd, 0x01);
        usleep(1000);
    }
       
        boxs[i] = *box;
    }
    biggestBoxIndex = GetBiggestHandIndex(boxs, objNum);
    LOGI("biggestBoxIndex:%d, objNum:%d\n", biggestBoxIndex, objNum);

    // When an object is detected, a rectangle is drawn in the DSTFRM
    if (biggestBoxIndex >= 0) {
        objBoxs[0] = boxs[biggestBoxIndex];
        MppFrmDrawRects(dstFrm, objBoxs, 1, RGB888_GREEN, DRAW_RETC_THICK); // Target hand objnum is equal to 1

        for (int j = 0; (j < objNum) && (objNum > 1); j++) {
            if (j != biggestBoxIndex) {
                remainingBoxs[num++] = boxs[j];
                // others hand objnum is equal to objnum -1
                MppFrmDrawRects(dstFrm, remainingBoxs, objNum - 1, RGB888_RED, DRAW_RETC_THICK);
            }
        }

        IVE_IMAGE_S imgIn;
        IVE_IMAGE_S imgDst;
        VIDEO_FRAME_INFO_S frmIn;
        VIDEO_FRAME_INFO_S frmDst;

        ret = ImgYuvCrop(&img, &imgIn, &cnnBoxs[biggestBoxIndex]); // Crop the image to classification network
        HI_EXP_LOGE(ret < 0, "ImgYuvCrop FAIL, ret = %d\n", ret);

        if ((imgIn.u32Width >= WIDTH_LIMIT) && (imgIn.u32Height >= HEIGHT_LIMIT)) {
            COMPRESS_MODE_E enCompressMode = srcFrm->stVFrame.enCompressMode;
			ret = OrigImgToFrm(&imgIn, &frmIn);
            frmIn.stVFrame.enCompressMode = enCompressMode;
			LOGI("crop u32Width = %d, img.u32Height = %d\n", imgIn.u32Width, imgIn.u32Height);
            ret = MppFrmResize(&frmIn, &frmDst, IMAGE_WIDTH, IMAGE_HEIGHT);
            ret = FrmToOrigImg(&frmDst, &imgDst);

            ret = CnnCalU8c1Img(self,  &imgDst, numInfo, HI_ARRAY_SIZE(numInfo), &resLen);
			HI_EXP_LOGE(ret < 0, "CnnCalU8c1Img FAIL, ret = %d\n", ret);
			HI_ASSERT(resLen <= sizeof(numInfo) / sizeof(numInfo[0]));
            RectBoxTran(&cnnBoxs[biggestBoxIndex], FRM_WIDTH, FRM_HEIGHT,
                dstFrm->stVFrame.u32Width, dstFrm->stVFrame.u32Height);
            HandDetectAddTxt(cnnBoxs[biggestBoxIndex], numInfo[0], ARGB1555_WHITE);

            MppFrmDestroy(&frmDst);
        }
        IveImgDestroy(&img);
        IveImgDestroy(&imgIn);
    }

    HI_CHAR *jsonBuf = CnnGestureClassifyToJson(numInfo, resLen);
    *resJson = jsonBuf;

    if (g_supportAudio == 1) {
		if (FdWriteMsg(g_stmChn.out, &numInfo[0], sizeof(RecogNumInfo)) != sizeof(RecogNumInfo)) {
			LOGE("FdWriteMsg FAIL\n");
		}
	}

    return ret;
}

static const AiPlug G_HAND_CLASSIFY_ITF = {
    .Prof = Yolo2HandDetectResnetClassifyProf,
    .Load = Yolo2HandDetectResnetClassifyLoad,
    .Unload = Yolo2HandDetectResnetClassifyUnload,
    .Cal = Yolo2HandDetectResnetClassifyCal,
};

const AiPlug* AiPlugItf(uint32_t* magic)
{
    if (magic) {
        *magic = AI_PLUG_MAGIC;
    }

    return (AiPlug*)&G_HAND_CLASSIFY_ITF;
}