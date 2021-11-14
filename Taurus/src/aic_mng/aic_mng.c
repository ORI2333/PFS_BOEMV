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
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#include "hi_ext_util.h"
#include "mpp_help.h"
#include "plug_mng.h"
#include "aic_mng.h"

#define PLUG_DIR_DEF            "./plugs" // 默认插件目录
#define PLUG_UUID_MAX           256 // 插件uuid的最大字节长度
#define PREPLUG_ID_MAX          9 // 预先加载的插件序号的最大值，从0开始

#define VENC_SIZET_DEF          PIC_720P // 编码的默认size
#define PIRIOD_LOG_INTVAL       (5 * 1000) // 期间统计信息输出间隔

#define VDEC_WIDTH_MAX          2688 // 解码支持的最大width
#define VDEC_HEIGHT_MAX         2160 // 解码支持的最大height
#define VDEC_FPS_DEF            25 // 解码支持的默认fps

#define SCENE_PARAM_FILE_DEF    "./scene_auto/param/sensor_imx335" // snssor参数文件
#define INIT_PIC_FILE_DEF       "./res/black_640x360.jpg" // 用于UVC的初始图片

#define STATUS_INFO_MAX         (128 * 1024) // status info buf的size, 暂时采用定长buf
#define STATUS_EVT_NAME         "status" // status事件名称

#define STRM_USER_MAX           8 // stream user的最大数目

/*
    AIC使用的MPP资源ID的默认值，将来可通过配置文件配置
*/
#define AIC_VDEC_CHN            0 // 默认使用的VDEC chn
#define AIC_VENC_CHN            0 // 默认使用的VENC chn
#define AIC_VPSS_GRP            0 // 默认使用的VPSS group
#define AIC_VPSS_ZIN_CHN        0 // 默认使用的VPSS 放大通道
#define AIC_VPSS_ZOUT_CHN       1 // 默认使用的VPSS 缩小通道

#define SIZE_TO_MPP_WIDTH_1920    1920
#define SIZE_TO_MPP_HEIGHT_1080   1080
#define SIZE_TO_MPP_WIDTH_1280    1280
#define SIZE_TO_MPP_HEIGHT_720    720
#define SIZE_TO_MPP_WIDTH_640     640
#define SIZE_TO_MPP_HEIGHT_360    360
#define SIZE_TO_MPP_WIDTH_3840    3840
#define SIZE_TO_MPP_HEIGHT_2160   2160
#define SIZE_TO_MPP_WIDTH_2592    2592
#define SIZE_TO_MPP_HEIGHT_1536   1536
#define SIZE_TO_MPP_HEIGHT_1944   1944

/**
    订阅AIC事件的user的信息.
*/
typedef struct AicEvtUser {
    struct list_head lnode;

    uintptr_t cltId;
    char evtName[TINY_BUF_SIZE];

    AicEvtProc proc;
    void *user;
}   AicEvtUser;

/**
    订阅AI服务的user的信息.
*/
typedef struct AiSvcUser {
    struct list_head lnode;

    uintptr_t cltId;
    char plugUuid[PLUG_UUID_MAX];

    AiResProc proc;
    void *user;
}   AiSvcUser;

/**
    AI计算job.
    表示一帧计算任务。
*/
typedef struct AiCalJob {
    bool busy; // 是否正在被使用
    int sigFd; // 完成通知信号

    int res; // 执行结果
    int grpId; // VPSS group ID
    int chnId; // VPSS chn ID
    VIDEO_FRAME_INFO_S frm; // [in|out]帧，待计算的帧，计算后叠加图形到该帧

    char *resJson; // 插件计算后返回的resJson
    char plugUuid[PLUG_UUID_MAX]; // 执行此job的plugUuid，由svc负责赋值
}   AiCalJob;

/**
    AI计算服务.
*/
typedef struct AiCalSvc {
    EvtMon *evtMon; // 关联的EvtMon，EvtMon关联了一个后台线程
    int sigFd; // 指示由job添加的信号
}   AiCalSvc;

/**
    期间统计信息.
*/
typedef struct PiriodStatis {
    int64_t begTime; // 期间起始时间(ms)

    int vpssOutNum; // VPSS输出的帧数，也等于VPSS输入的帧数
    int vencOutNum; // VENC输出的帧数，也等于VENC输入的帧数
    int aiCalNum; // AI计算的帧数

    int64_t aiCalCost; // AI插件计算耗时
    int64_t fmtFrmCost; // AI计算前帧处理(resize等)耗时
    int64_t cvtVoFrmCost; // VENC输出帧转换为vo格式的耗时
}   PiriodStatis;

/**
    aic stream user node.
*/
typedef struct AicStrmUser {
    const IAicStrmUser *itf;
    void *user;
}   AicStrmUser;

/**
    AI Camera Manager.
*/
typedef struct AicMng {
    bool inited; // 对象是否inited
    bool initing; // 是否正在初始化

    EvtMon *evtMon; // MainEvtMon()的快捷值
    // UvcDevT *uvcDev; // 使用的UvcDev

    // mpp可监视fd
    int vpssFd; // 指示VPSS有输出的fd
    int vencFd; // 指示VENC有输出的fd

    // 当前MppSess使用的资源ID
    VPSS_GRP vpssGrp;
    VPSS_CHN vpssChn0;
    VPSS_CHN vpssChn1;
    VDEC_CHN vdecChn;
    VENC_CHN vencChn;

    // MppSess config
    ViCfg viCfg;
    VdecCfg vdecCfg;
    VpssCfg vpssCfg;
    VencCfg vencCfg;

    // MppSess
    MppSess *viSess; // VI(sensor)+VPSS | VDEC+VPSS
    MppSess *vencSess; // VENC

    // MppSess相关
    OsdSet *osdSet; // OSD set，供plug在resFrm中输出OSD
    uint32_t inFrmIdc; // 输入frame的ID计数器，用于生成mpp frame的timeRef

    // vi相关
    AicViType viType; // vi type，同时只能支持一种type
    bool viByHost; // VI是否被host启动的
    int64_t rxPicNum; // 一次HOST-VI接收到的图片数目
    int viOpenRef; // sensor被打开的计数，用来表示当前有多少个user正在打开sensor

    // vo相关
    uint32_t voTypes; // vo types，可同时支持多个type
    int voOpenRef; // vo被打开的计数，用来表示当前有多少个user正在打开vo

    AiCalSvc aiCalSvc; // AI计算服务，会关联一个后台线程
    AiCalJob aiCalJob; // AI计算任务，包含计算一帧图片的相关信息
    char *prevResJson; // 前次AI计算返回的resJson

    pthread_mutex_t plugMutex; // AiCalSvc与PlugLoadJob会同时访问插件，需要保护
    AiPlugLib workPlug; // 当前加载的插件

    struct list_head aiSvcUsers; // 当前插件AI服务的user list

    struct list_head plugInfos; // 插件信息列表，元素为AicPlugInfo
    struct list_head svcPlugs; // 正在运行管理服务的插件列表，元素为AicPlugInfo

    struct list_head evtUsers; // 订阅事件的user list

    int statusChgFd; // 指示状态变更的fd

    int blkCallNum; // 此对象上正在执行的耗时函数的数量，并不精确，仅用于检查错误
    PiriodStatis piriodStatis; // 期间统计信息

    // 用于规避UVC问题的初始图片发送相关数据
    bool inTxInitPic; // 正在发送初始图片
    int initPicSent; // 这次初始图片发送操作，已发送的图片数量，仅用于log
    uint8_t *initPicBuf; // 存储初始图片的buf
    int initPicLen; // 初始图片的字节长度

    AicStrmUser strmUserTab[STRM_USER_MAX]; // 存储stream user的table
    size_t strmUserNum; // node number in strmUserTab
}   AicMng;

/**
    为简化其他模块的使用，以及uvc histrm接口的限制，AicMng全局只有一个实例.
*/
static AicMng g_aic = { 0 };

/*************************************************************************************************
    util
*************************************************************************************************/
#define SP_AIC_UTIL

/**
    将UVC的size type转换为MPP的.
*/
static PIC_SIZE_E SizeToMpp(int width, int height)
{
    if (width == SIZE_TO_MPP_WIDTH_1920 && height == SIZE_TO_MPP_HEIGHT_1080) {
        return PIC_1080P;
    } else if (width == SIZE_TO_MPP_WIDTH_1280 && height == SIZE_TO_MPP_HEIGHT_720) {
        return PIC_720P;
    } else if (width == SIZE_TO_MPP_WIDTH_640 && height == SIZE_TO_MPP_HEIGHT_360) {
        return PIC_360P;
    } else if (width == SIZE_TO_MPP_WIDTH_3840 && height == SIZE_TO_MPP_HEIGHT_2160) {
        return PIC_3840x2160;
    } else if (width == SIZE_TO_MPP_WIDTH_2592 && height == SIZE_TO_MPP_HEIGHT_1536) {
        return PIC_2592x1536;
    } else if (width == SIZE_TO_MPP_WIDTH_2592 && height == SIZE_TO_MPP_HEIGHT_1944) {
        return PIC_2592x1944;
    } else {
        LOGE("cannot map {%dx%d} to sizeType, regard 1080p\n", width, height);
        return PIC_1080P;
    }
}

/**
    创建AicEvtUser.
*/
static AicEvtUser* AicEvtUserNew(uintptr_t cltId, const char* evtName, AicEvtProc proc, void* user)
{
    AicEvtUser *self = (AicEvtUser*)malloc(sizeof(*self));
    HI_ASSERT(self);
    self->cltId = cltId;
    HiStrxfrm(self->evtName, evtName ? evtName : "", sizeof(self->evtName));
    self->proc = proc;
    self->user = user;
    return self;
}

/**
    创建AiSvcUser.
*/
static AiSvcUser* AiSvcUserNew(uintptr_t cltId, const char* plugUuid, AiResProc proc, void* user)
{
    AiSvcUser *self = (AiSvcUser*)malloc(sizeof(*self));
    HI_ASSERT(self);
    self->cltId = cltId;
    HiStrxfrm(self->plugUuid, plugUuid ? plugUuid : "", sizeof(self->plugUuid));
    self->proc = proc;
    self->user = user;
    return self;
}

/*************************************************************************************************
    MPP session 数据处理
*************************************************************************************************/
#define SP_AIC_INNER

/**
    为frame分配id.
*/
static inline uint32_t AicGenFrmId(void)
{
    // 替换原来的timeRef，这样venc生存期内，vi重启，timeRef也会单向递增，避免MPP编码错误
    // 先加2，以避免是用0，从mpp实际输出的timeRef看，其从2开始
    while ((g_aic.inFrmIdc += 2) == 0) {}
    return g_aic.inFrmIdc;
}

/**
    向user分发StrmOn.
*/
static inline void AicDispStrmOn(PAYLOAD_TYPE_E codecType, int outWidth, int outHeight)
{
    if (g_aic.strmUserNum > 0) {
        for (size_t i = 0; i < HI_ARRAY_SIZE(g_aic.strmUserTab); i++) {
            AicStrmUser *node = &g_aic.strmUserTab[i];
            if (node->itf && node->itf->OnStrmOn) {
                node->itf->OnStrmOn(node->user, g_aic.vencChn, codecType, outWidth, outHeight);
            }
        }
    }
}

/**
    向user分发StrmOff.
*/
static inline void AicDispStrmOff(void)
{
    if (g_aic.strmUserNum > 0) {
        for (size_t i = 0; i < HI_ARRAY_SIZE(g_aic.strmUserTab); i++) {
            AicStrmUser *node = &g_aic.strmUserTab[i];
            if (node->itf && node->itf->OnStrmOff) {
                node->itf->OnStrmOff(node->user);
            }
        }
    }
}

/**
    向user分发VencFrm.
*/
static inline void AicDispVencFrm(VencFrm* frm)
{
    if (g_aic.strmUserNum > 0) {
        for (size_t i = 0; i < HI_ARRAY_SIZE(g_aic.strmUserTab); i++) {
            AicStrmUser *node = &g_aic.strmUserTab[i];
            if (node->itf && node->itf->OnVencFrm) {
                node->itf->OnVencFrm(node->user, frm);
            }
        }
    }
}

/**
    向user分发VideoFrm.
*/
static inline void AicDispVideoFrm(VIDEO_FRAME_INFO_S* frm)
{
    if (g_aic.strmUserNum > 0) {
        for (size_t i = 0; i < HI_ARRAY_SIZE(g_aic.strmUserTab); i++) {
            AicStrmUser *node = &g_aic.strmUserTab[i];
            if (node->itf && node->itf->OnVideoFrm) {
                node->itf->OnVideoFrm(node->user, frm);
            }
        }
    }
}

/**
    查找uuid对应的AI Plug.
*/
static AiPlugInfo* AicFindPlug(const char* uuid)
{
    AiPlugInfo *info = NULL;
    struct list_head *node = NULL;

    HiGlbLock();
    list_for_each(node, &g_aic.plugInfos) {
        info = list_entry(node, AiPlugInfo, lnode);
        if (strcmp(uuid, info->uuid) == 0) {
            HiGlbUnlock();
            return info;
        }
    }
    HiGlbUnlock();
    return NULL;
}

/**
    load plug only.
*/
static int AicLoadPlug(const char* uuid, const AiPlugInfo *info)
{
    HI_ASSERT(!g_aic.workPlug.itf);
    AiPlugLib newPlug;
    int ret;

    // load插件
    ret = PmLoadPlugLib(&newPlug, info->path);
    if (ret < 0) {
        LOGE("load plug FAIL, ret=%d\n", ret);
        return ret;
    }

    // load模型
    HI_ASSERT(g_aic.osdSet);
    HI_ASSERT(newPlug.itf); // 增加newPlug.itf判空处理
    HI_ASSERT(newPlug.itf->Load);
    ret = newPlug.itf->Load(&newPlug.model, g_aic.osdSet);
    if (ret < 0) {
        LOGE("load plug model FAIL, ret=%d\n", ret);
        PmUnloadPlugLib(&newPlug);
        return ret;
    }
    HI_ASSERT(newPlug.model);

    // 加锁更新workPlug
    HiGlbLock();
    g_aic.workPlug = newPlug;
    HiGlbUnlock();
    return 0;
}

/**
    unload当前工作的AI plug.
    需要在加锁plugMutex后调用此函数。
*/
static int AicUnloadPlug(void)
{
    AiPlugLib curPlug; // 用以记录当前working plug
    int ret;

    if (!g_aic.workPlug.itf) {
        return 0;
    }

    // 加锁移出workPlug
    HiGlbLock();
    curPlug = g_aic.workPlug;
    if (memset_s(&g_aic.workPlug, sizeof(g_aic.workPlug), 0, sizeof(g_aic.workPlug)) != EOK) {
        HI_ASSERT(0);
    }
    HiGlbUnlock();

    LOGI("AIC: unload plug '%s' ...\n", curPlug.uuid);
    if (curPlug.model && (ret = curPlug.itf->Unload(curPlug.model)) < 0) {
        LOGE("unload plug model FAIL, ret=%d\n", ret);
    }
    ret = PmUnloadPlugLib(&curPlug);
    LOGI("AIC: unload plug done\n");
    return ret;
}

/**
    加载配置的plugs.
    目前暂时只支持一个plug的加载，当成功加载到一个plug后，忽略后续的plug
*/
static int AicPreloadPlugs(void)
{
    char key[SMALL_BUF_SIZE];
    int plugNum = 0;
    int ret;

    // 枚举conf中需要为0~REPLUG_ID_MAX的配置项，获取其plugUuid，加载插件
    for (int i = 0; i <= PREPLUG_ID_MAX; i++) {
        // 从conf中plug[i]对应的uuid
        if (snprintf_s(key, sizeof(key), sizeof(key) - 1, "work_plugs:plug%d", i) < 0) {
            HI_ASSERT(0);
        }
        const char* plugUuid = GetCfgStr(key, NULL);
        if (!plugUuid || !*plugUuid) {
            continue;
        }

        // 加载插件， 若失败，则简单忽略
        ret = AicSetAiPlug(plugUuid, NULL, false);
        if (ret) {
            LOGE("load plug '%s' FAIL, ret=%d\n", plugUuid, ret);
            continue;
        }

        // 目前暂时只支持一个plug,加载成功后即退出
        plugNum++;
        break;
    }
    return plugNum;
}

/**
    生成status事件，并发送给订阅者.
*/
int AicGenStatusEvt(void)
{
    MemBlk *evt = MemBlkNew2(HUGE_BUF_SIZE, TINY_BUF_SIZE);
    HI_ASSERT(evt);

    int ret = AicGetStatus(evt);
    HI_EXP_GOTO(ret < 0, END, "AicGetStatus FAIL, ret=%d\n", ret);

    struct list_head *node = NULL;
    list_for_each(node, &g_aic.evtUsers) {
        AicEvtUser *user = list_entry(node, AicEvtUser, lnode);
        if (!user->evtName[0] || strcmp(user->evtName, STATUS_EVT_NAME) == 0) {
            HI_ASSERT(user->proc);
            user->proc(user->user, user->cltId, STATUS_EVT_NAME, (char*)MemBlkData(evt), MemBlkLen(evt));
        }
    }
    ret = 0;

    END:
        MemBlkDelete(evt);
        return ret;
}

/**
    从文件加载初始图片.
*/
static int AicLoadInitPic(void)
{
    const char* initPicFile = GetCfgStr("aic_mng:init_pic_file", INIT_PIC_FILE_DEF);
    LOGI("AIC: load init pic '%s' ...\n", initPicFile);

    HI_ASSERT(!g_aic.initPicBuf);
    long ret = FileLoadToBuf(initPicFile, &g_aic.initPicBuf, true);
    HI_EXP_RET(ret < 0, ret, "AIC: load init pic '%s' FAIL, err=%ld\n", initPicFile, ret);
    HI_ASSERT(ret > 0); // 不会等于0
    g_aic.initPicLen = ret;

    LOGI("AIC: load init pic done, size=%ld\n", ret);
    return ret;
}

/**
    释放初始图片.
*/
static void AicFreeInitPic(void)
{
    if (g_aic.initPicBuf) {
        free(g_aic.initPicBuf);
        g_aic.initPicBuf = NULL;
        g_aic.initPicLen = 0;
    }
}

/**
    发送初始图片.
*/
int AicTxInitPic(void)
{
    VDEC_STREAM_S vdecStrm;
    int ret;

    // 目前仅支持在vitype为BY_HOST时发送初始图片，可以根据需要调整
    HI_ASSERT(g_aic.viOpenRef > 0 && g_aic.viType == AIC_VIT_HOST);

    // 只load一次file
    if (!g_aic.initPicBuf && AicLoadInitPic() < 0) {
        return -1;
    }

    LOGI("AIC: tx init pic %d ...\n", g_aic.initPicSent + 1);
    HI_ASSERT(g_aic.initPicBuf && g_aic.initPicLen > 0);

    // 提交给VDEC
    if (memset_s(&vdecStrm, sizeof(vdecStrm), 0, sizeof(vdecStrm)) != EOK) {
        HI_ASSERT(0);
    }

    vdecStrm.u64PTS = 0;
    vdecStrm.pu8Addr = g_aic.initPicBuf;
    vdecStrm.u32Len = g_aic.initPicLen;
    vdecStrm.bEndOfFrame = HI_TRUE;
    vdecStrm.bEndOfStream = HI_FALSE;

    ret = HI_MPI_VDEC_SendStream(g_aic.vdecChn, &vdecStrm, 0);
    if (ret) {
        LOGE("HI_MPI_VDEC_SendStream FAIL, ret=%#x\n", ret);
        return -1;
    }

    g_aic.inTxInitPic = true;
    g_aic.initPicSent++;
    return 0;
}

/**
    添加AiCalJob.
    填充job，唤醒AI计算服务线程处理。
*/
static void AicAddAiJob(const VIDEO_FRAME_INFO_S* frm)
{
    AiCalSvc *svc = &g_aic.aiCalSvc;
    AiCalJob *job = &g_aic.aiCalJob;

    // 要先判断AiCalSvc是否ready，及是否有job正在处理
    if (!svc->evtMon || job->busy) {
        LOGD("discard ai-job for %s\n", (!svc->evtMon) ? "svc-offline" : "in-job");
        int ret = HI_MPI_VPSS_ReleaseChnFrame(g_aic.vpssGrp, g_aic.vpssChn0, frm);
        HI_EXP_LOGE(ret, "HI_MPI_VPSS_ReleaseChnFrame FAIL, err=%#x\n", ret);
        return;
    }

    // 填充job
    job->busy = true;
    job->res = -1;
    job->grpId = g_aic.vpssGrp;
    job->chnId = g_aic.vpssChn0;
    job->frm = *frm;
    HI_ASSERT(!job->resJson);
    job->resJson = NULL;
    job->plugUuid[0] = 0;

    // 唤醒AI计算服务线程处理
    HI_ASSERT(svc->sigFd >= 0);
    EventFdInc(svc->sigFd);
}

/**
    执行AI计算任务.

    此函数由AiCalSvc线程执行。
    由于插件可以动态加载/卸载，此函数在执行时需要加锁plugMutex。

    @param frm [in|out]，输入待计算的frame，输出叠加了结果图形的frame
*/
static int AicExecAiJob(AiCalJob* job)
{
    VIDEO_FRAME_INFO_S* frm = &job->frm;
    VIDEO_FRAME_INFO_S srcFrm; // 满足plug需要的输入frame
    int ret;

    MutexLock(&g_aic.plugMutex);

    // 先判断是否加载了插件，未加载插件是允许的，此时表示不做AI计算，透传frame
    if (!g_aic.workPlug.itf || !g_aic.workPlug.model) { // no plug loaded
        ret = 0;
        goto END;
    } else {
        HiStrxfrm(job->plugUuid, g_aic.workPlug.uuid, sizeof(job->plugUuid));
    }

    // resize frame为插件需要的格式/像素，frm不会被修改
    HI_ASSERT(g_aic.workPlug.width > 0 && g_aic.workPlug.height > 0);
    ret = MppFrmResize(frm, &srcFrm, g_aic.workPlug.width, g_aic.workPlug.height);
    HI_CHK_GOTO(ret, END, "AIC: for resize FAIL, ret=%d\n", ret);

    // 调用插件计算，插件会将结果帧通过resFrm(也即frm)返回
    VIDEO_FRAME_INFO_S *resFrm = frm; // 仅强调通过@param frm输出resFrm
    ret = g_aic.workPlug.itf->Cal(g_aic.workPlug.model, &srcFrm, resFrm, &job->resJson);
    MppFrmDestroy(&srcFrm);
    HI_EXP_LOGE(ret < 0, "plug cal FAIL, ret=%d\n", ret);

    END:
        MutexUnlock(&g_aic.plugMutex);
        job->res = ret;
        return ret;
}

/**
    AiCalSvc监听到有AI计算任务新增.

    此函数由AiCalSvc服务线程执行。
    主线程通过job->sigFd通知AiCalSvc服务线程。
*/
static void AicOnJobAdd(void* user, int fd, uint32_t evts)
{
    AiCalSvc *svc = &g_aic.aiCalSvc;
    AiCalJob *job = &g_aic.aiCalJob;

    HI_ASSERT(fd == svc->sigFd);
    pthread_t ret = pthread_self();
    pthread_t res = EmThrdId(svc->evtMon);
    HI_ASSERT(ret == res);

    EvtChkRet(evts, FDE_IN, fd);
    LOGV("detect job-add-fd IN, fd=%d\n", fd);
    EventFdClear(svc->sigFd);

    // NOTE: 无需也不能加锁
    HI_ASSERT(job->busy);
    AicExecAiJob(job);
    HI_ASSERT(job->sigFd >= 0);
    EventFdInc(job->sigFd);
}

/**
    监听到有AI计算任务完成.
    将结果frame提交给VENC编码，将结果json发送给host。
*/
static void AicOnJobEnd(void* user, int fd, uint32_t evts)
{
    AiCalJob *job = &g_aic.aiCalJob;
    int ret;

    HI_ASSERT(fd == job->sigFd);
    EvtChkRet(evts, FDE_IN, evts);
    LOGV("detect job-end-fd IN, fd=%d\n", fd);
    EventFdClear(job->sigFd);

    // 通过busy标志来控制job的add，此函数不用也不能加锁
    HI_ASSERT(job->busy);

    // 将resFrm提交给VENC编码
    if (g_aic.vencSess) {
        ret = HI_MPI_VENC_SendFrame(g_aic.vencChn, &job->frm, 0);
        HI_EXP_LOGE(ret, "HI_MPI_VENC_SendFrame failed! ERR=%#x, discard the frm\n", ret);
    } else {
        LOGW("venc discard, for VENC not ready\n");
    }

    // 提交resJson给订阅的user, 仅当resJson与prevResJson不同时才提交给user
    if (job->resJson && *job->resJson &&
        (!g_aic.prevResJson || strcmp(g_aic.prevResJson, job->resJson) != 0)) {
        int resLen = strlen(job->resJson);
        struct list_head *node = NULL;
        list_for_each(node, &g_aic.aiSvcUsers) {
            AiSvcUser *user = list_entry(node, AiSvcUser, lnode);
            if (!user->plugUuid[0] || strcmp(user->plugUuid, job->plugUuid) == 0) {
                HI_ASSERT(user->proc);
                user->proc(user->user, user->cltId, job->plugUuid, job->resJson, resLen);
            }
        }
    }

    // 重置prevResJson
    // NOTE: 插件可能会被卸载，但其返回的resJson在插件卸载后仍然可用，只是插件的__FILE__等
    // NOTE: string常量会变为无效，resJson若泄漏，dump其__FILE__时会异常。
    if (g_aic.prevResJson) {
        free(g_aic.prevResJson);
    }
    g_aic.prevResJson = job->resJson;
    job->resJson = NULL;

    // 将frame提交给注册的user
    AicDispVideoFrm(&job->frm);

    ret = HI_MPI_VPSS_ReleaseChnFrame(job->grpId, job->chnId, &job->frm);
    HI_EXP_LOGE(ret, "HI_MPI_VPSS_ReleaseChnFrame FAIL, ret=%#x\n", ret);
    job->busy = false; // 执行完处理后才能reset busy标志
}

/**
    监听到有VPSS有输出可用(有frame处理完成).
*/
static void AicOnVpssOut(void* user, int fd, uint32_t evts)
{
    HI_ASSERT(fd == g_aic.vpssFd);
    EvtChkRet(evts, FDE_IN, fd);
    LOGD("detect vpss-fd IN, fd=%d\n", fd);

    VIDEO_FRAME_INFO_S frm;
    int ret;

    // 从VPSS读取frame, 读取到的frame没有map虚拟地址，后续处理需要注意
    ret = HI_MPI_VPSS_GetChnFrame(g_aic.vpssGrp, g_aic.vpssChn0, &frm, 0);
    HI_EXP_RET_NONE(ret, "HI_MPI_VPSS_GetChnFrame FAIL, err=%#x, grp=%d, chn=%d\n",
        ret, g_aic.vpssGrp, g_aic.vpssChn0);

    // VO未开启时，discard frame
    if (g_aic.voOpenRef <= 0) {
        LOGD("discard vpss-out frm, for no VO working\n");
        ret = HI_MPI_VPSS_ReleaseChnFrame(g_aic.vpssGrp, g_aic.vpssChn0, &frm);
        HI_EXP_LOGE(ret, "HI_MPI_VPSS_ReleaseChnFrame FAIL, err=%#x\n", ret);
        return;
    }

    // 替换原来的timeRef，这样venc生存期内，vi重启，timeRef也会单向递增，避免MPP 编码错误
    // 先加2，以避免是用0，从mpp实际输出的timeRef看，其从2开始
    while ((g_aic.inFrmIdc += 2) == 0) {}
    frm.stVFrame.u32TimeRef = g_aic.inFrmIdc;

    // 可以优化为在这里检查是否加载了插件，未加载时，直接通过venc输出
    AicAddAiJob(&frm);
}

/**
    监听到VENC有输出可用(有frame编码完成).
*/
static void AicOnVencOut(void* user, int fd, uint32_t evts)
{
    HI_ASSERT(fd == g_aic.vencFd);
    EvtChkRet(evts, FDE_IN, fd);
    LOGV("detect venc-fd IN, fd=%d\n", fd);

    VencFrm *vstrm = NULL;
    int ret;

    // 从VENC get frame
    ret = VencGetFrmx(&vstrm, g_aic.vencChn);
    HI_EXP_GOTO(ret, END, "VencGetFrm FAIL, ret=%#x\n", ret);

    // 先复位此标志，user在处理此frame时可能会调用AicInitTxReq，而重置该标志.
    g_aic.inTxInitPic = false;

    // 将frame提交给注册的user
    AicDispVencFrm(vstrm);

    END:
        if (vstrm) {
            // 移动到VencStrmDefRef中了
            VencFrmDelRef(vstrm);
        }
}

/**
    监听到status改变.
*/
static void AicOnStatusChg(void* user, int fd, uint32_t evts)
{
    HI_ASSERT(fd == g_aic.statusChgFd);
    EvtChkRet(evts, FDE_IN, fd);
    LOGV("detect status-chg IN, fd=%d\n", fd);

    EventFdClear(fd);
    AicGenStatusEvt();
}

/**
    启动AI计算服务.
*/
static void AicStartAiSvc(void)
{
    // create AlCalJob
    AiCalJob *job = &g_aic.aiCalJob;
    HI_ASSERT(!job->busy);
    HI_ASSERT(job->sigFd < 0);
    job->sigFd = EventFdCreate();
    HI_ASSERT(job->sigFd >= 0);
    if (EmAddFd(g_aic.evtMon, job->sigFd, FDE_IN, AicOnJobEnd, &g_aic) < 0) {
        HI_ASSERT(0);
    }
    HI_ASSERT(!job->resJson);

    // create AlCalSvc
    AiCalSvc *svc = &g_aic.aiCalSvc;
    HI_ASSERT(!svc->evtMon);
    HI_ASSERT(svc->sigFd < 0);
    svc->sigFd = EventFdCreate();
    HI_ASSERT(svc->sigFd >= 0);

    // start AlCalSvc
    if (EmCreate(&svc->evtMon) < 0) {
        HI_ASSERT(0);
    }
    if (EmAddFd(svc->evtMon, svc->sigFd, FDE_IN, AicOnJobAdd, &g_aic) < 0) {
        HI_ASSERT(0);
    }
    EmStart(svc->evtMon); // 其会创建后台线程，并开始服务
}

/**
    终止AI计算服务.
*/
static void AicStopAiSvc(void)
{
    // stop and destroy AlCalSvc ...
    AiCalSvc *svc = &g_aic.aiCalSvc;
    EmStop(svc->evtMon); // 其会请求终止并等待后台线程结束

    // NOTE: 需要先stop，再DelFd
    HI_ASSERT(svc->sigFd >= 0);
    if (EmDelFd(svc->evtMon, svc->sigFd) < 0) {
        HI_ASSERT(0);
    }
    if (close(svc->sigFd) < 0) {
        HI_ASSERT(0);
    }
    svc->sigFd = -1;

    EmDestroy(svc->evtMon);
    svc->evtMon = NULL;

    // destroy AlCalJob ...
    AiCalJob *job = &g_aic.aiCalJob;

    // destroy未执行的AiCalJob
    if (job->busy) {
        int ret = HI_MPI_VPSS_ReleaseChnFrame(job->grpId, job->chnId, &job->frm);
        if (ret != HI_SUCCESS) {
            LOGE("HI_MPI_VPSS_ReleaseChnFrame FAIL, ret=%#x\n", ret);
        }
        job->busy = false;
    }

    HI_ASSERT(job->sigFd >= 0);
    if (EmDelFd(g_aic.evtMon, job->sigFd) < 0) {
        HI_ASSERT(0);
    }
    if (close(job->sigFd) < 0) {
        HI_ASSERT(0);
    }
    job->sigFd = -1;

    job->res = -1;
    job->grpId = -1;
    job->chnId = -1;
    if (memset_s(&job->frm, sizeof(job->frm), 0, sizeof(job->frm)) != EOK) {
        HI_ASSERT(0);
    }

    // 有可能当前有已经完成的AI计算，但主线程还未来得及处理，需要释放相应资源
    if (job->resJson) {
        LOGW("free job.resJson for it not proc by main thrd\n");
        free(job->resJson);
        job->resJson = NULL;
    }
}

/**
    启动插件服务.
    加载需要运行管理服务的插件，并启动其服务。
    加载插件无需锁保护。
    plug.startSvc也无需加锁，提供了管理服务的插件会自己保护资源。
*/
static int AicStartPlugSvcs(void)
{
    struct list_head *node = NULL;
    AiPlugLib *plug = NULL;
    int svcNum = 0;
    int ret;

    list_for_each(node, &g_aic.plugInfos) {
        AiPlugInfo *info = list_entry(node, AiPlugInfo, lnode);
        if (!info->mngSvc) {
            continue;
        }

        LOGI("AIC: start mng svc of plug '%s' ...\n", info->uuid);

        // 加载插件
        plug = (AiPlugLib*)malloc(sizeof(*plug));
        HI_ASSERT(plug);
        ret = PmLoadPlugLib(plug, info->path);
        if (ret < 0) {
            LOGE("load ai plug '%s' FAIL, ret=%d\n", info->uuid, ret);
            free(plug);
            plug = NULL; // ensure by meeting
            continue;
        }

        // 启动服务
        HI_ASSERT(plug->itf && plug->itf->StartSvc);
        ret = plug->itf->StartSvc();
        if (ret != 0) {
            LOGE("start plug svc of '%s' FAIL, ignore, ret=%d\n", info->uuid, ret);
            PmUnloadPlugLib(plug);
            free(plug);
            plug = NULL; // ensure by meeting
            continue;
        }

        // 添加到svcPlus中，目前是不需要加锁保护
        list_add_tail(&plug->lnode, &g_aic.svcPlugs);
        svcNum++;
    }
    return svcNum;
}

/**
    终止已启动的插件服务.
*/
static int AicStopPlugSvcs(void)
{
    struct list_head *node = NULL;
    struct list_head *next = NULL;
    int svcNum = 0;
    int ret;

    // 将svcInfos中记录的所有插件的服务终止，并卸载插件
    list_for_each_safe(node, next, &g_aic.svcPlugs) {
        AiPlugLib *plug = list_entry(node, AiPlugLib, lnode);
        LOGI("AIC: stop mng svc of plug '%s' ...\n", plug->uuid);

        // 终止服务
        HI_ASSERT(plug->itf && plug->itf->StopSvc);
        ret = plug->itf->StopSvc();
        if (ret != 0) {
            LOGE("stop plug svc of '%s' FAIL, ignore, ret=%d\n", plug->uuid, ret);
        }

        // 从svcPlus中删除，目前是不需要加锁保护
        list_del(&plug->lnode);

        // 卸载插件
        PmUnloadPlugLib(plug);
        free(plug);
        svcNum++;
    }
    return svcNum;
}

/**
    获取VPSS的out chn fd，并监听之.
*/
static void AicMonVpss(void)
{
    HI_ASSERT(g_aic.vpssFd < 0);
    g_aic.vpssFd = HI_MPI_VPSS_GetChnFd(g_aic.vpssGrp, g_aic.vpssChn0);
    if (g_aic.vpssFd < 0) {
        LOGE("HI_MPI_VPSS_GetChnFd, ret=%x\n", g_aic.vpssFd);
    } else {
        if (EmAddFd(g_aic.evtMon, g_aic.vpssFd, FDE_IN | FDE_ET, AicOnVpssOut, NULL) < 0) {
            HI_ASSERT(0);
        }
    }
}

/**
    撤销对VPSS的out chn的监听.
*/
static void AicUnmonVpss(void)
{
    if (g_aic.vpssFd < 0) {
        return;
    }

    if (EmDelFd(g_aic.evtMon, g_aic.vpssFd) < 0) {
        HI_ASSERT(0);
    }
    int ret = HI_MPI_VPSS_CloseFd();
    if (ret != 0) {
        LOGE("close VPSS fd FAIL, err=%#x\n", ret);
    }
    g_aic.vpssFd = -1;
}

/**
    获取VENC的out chn fd，并监听之.
*/
static void AicMonVenc(void)
{
    HI_ASSERT(g_aic.vencFd < 0);
    g_aic.vencFd = HI_MPI_VENC_GetFd(g_aic.vencChn);
    if (g_aic.vencFd < 0) {
        LOGE("HI_MPI_VENC_GetFd FAIL, ret=%x\n", g_aic.vencFd);
    } else {
        if (EmAddFd(g_aic.evtMon, g_aic.vencFd, FDE_IN | FDE_ET, AicOnVencOut, NULL) < 0) {
            HI_ASSERT(0);
        }
    }
}

/**
    撤销对VENC的out chn的监听.
*/
static void AicUnmonVenc(void)
{
    if (g_aic.vencFd < 0) {
        return;
    }

    if (EmDelFd(g_aic.evtMon, g_aic.vencFd) < 0) {
        HI_ASSERT(0);
    }
    int ret = HI_MPI_VENC_CloseFd(g_aic.vencChn);
    if (ret) {
        LOGE("close VENC fd FAIL, err=%#x\n", ret);
    }
    g_aic.vencFd = -1;
}

/**
    create VI(SENSOR)+VPSS session.

    NOTE: MPP中的VI指基于SENSOR的video input，与HiOpenais中的vi有一定差异。
    HiOpenais中vi包含但不限于SENSOR输入。
*/
static HI_S32 AicCreateSnsVpss(int outWidth, int outHeight, PIXEL_FORMAT_E pixFormat)
{
    LOGI("start sensor->vpss, size={%dx%d}\n", outWidth, outHeight);
    VPSS_GRP vpssGrp = AIC_VPSS_GRP;
    VPSS_CHN vpssChn = AIC_VPSS_ZOUT_CHN; // 缩小通道
    int ret;

    // VI config
    ViCfgInit(&g_aic.viCfg);
    ViCfgSetDev(&g_aic.viCfg, 0, -1);
    ViCfgSetPipe(&g_aic.viCfg, 0, -1, -1, -1, -1);
    ViCfgSetChn(&g_aic.viCfg, 0, pixFormat, -1, -1, COMPRESS_MODE_SEG);

    // VPSS config
    SIZE_S snsMaxSize = CurSnsMaxSize(ViCfgSnsType(&g_aic.viCfg));
    LOGI("AIC: snsMaxSize=%ux%u\n", snsMaxSize.u32Width, snsMaxSize.u32Height);
    VpssCfgInit(&g_aic.vpssCfg);
    VpssCfgSetGrp(&g_aic.vpssCfg, vpssGrp, NULL, snsMaxSize.u32Width, snsMaxSize.u32Height);
    g_aic.vpssCfg.grpAttr.enPixelFormat = pixFormat;
    VpssCfgAddChn(&g_aic.vpssCfg, vpssChn, NULL, outWidth, outHeight);

    HI_ASSERT(!g_aic.viSess);
    ret = ViVpssCreate(&g_aic.viSess, &g_aic.viCfg, &g_aic.vpssCfg);
    if (ret != 0) {
        LOGE("ViVpss Sess create FAIL, ret=%#x\n", ret);
        return ret;
    }

    const char* sceneParamFile = GetCfgStr("aic_mng:scene_param_file", SCENE_PARAM_FILE_DEF);
    ret = SceneInit(sceneParamFile);
    if (ret != 0) {
        LOGE("SceneInit FAIL, ret=%#x\n", ret);
        MppSessDestroy(g_aic.viSess);
        g_aic.viSess = NULL;
        return ret;
    }

    g_aic.vpssGrp = vpssGrp;
    g_aic.vpssChn0 = vpssChn;
    return 0;
}

/**
    create VDEC+VPSS session.
*/
HI_S32 AicCreateVdecVpss(int outWidth, int outHeight, PIXEL_FORMAT_E pixFormat)
{
    VPSS_GRP vpssGrp = AIC_VPSS_GRP;
    VPSS_CHN vpssChn = AIC_VPSS_ZIN_CHN; // 放大通道
    int ret;

    // VPSS config
    VpssCfgInit(&g_aic.vpssCfg);
    VpssCfgSetGrp(&g_aic.vpssCfg, vpssGrp, NULL, VDEC_WIDTH_MAX, VDEC_HEIGHT_MAX);
    g_aic.vpssCfg.grpAttr.enPixelFormat = pixFormat;
    g_aic.vpssCfg.grpAttr.bNrEn = HI_FALSE;
    VpssCfgAddChn(&g_aic.vpssCfg, vpssChn, NULL, outWidth, outHeight);

    // 创建MppSess
    HI_ASSERT(!g_aic.viSess);
    ret = VdecVpssCreate(&g_aic.viSess, &g_aic.vdecCfg, &g_aic.vpssCfg, true);
    HI_CHK_RET(ret, "VdecVpss Sess create FAIL, ret=%#x\n", ret);

    g_aic.vpssGrp = vpssGrp;
    g_aic.vpssChn0 = vpssChn;
    return 0;
}

/**
    create VENC session.
*/
static int AicCreateVenc(PAYLOAD_TYPE_E codecType, int width, int height)
{
    VENC_GOP_MODE_E enGopMode = VENC_GOPMODE_NORMALP;
    SAMPLE_RC_E enRcMode = SAMPLE_RC_CBR;
    PIC_SIZE_E sizeType;
    int ret;

    sizeType = SizeToMpp(width, height);
    if (codecType == PT_H264 || codecType == PT_H265) {
        enRcMode = SAMPLE_RC_AVBR;
    }

    VencCfgInit(&g_aic.vencCfg, g_aic.vencChn, sizeType, codecType, enGopMode, enRcMode);
    HI_ASSERT(!g_aic.vencSess);
    ret = VencCreate(&g_aic.vencSess, &g_aic.vencCfg);
    HI_CHK_RET(ret, "VencCreate FAIL, ret=%#x\n", ret);
    return 0;
}

/**
    start vi.
*/
int AicStartVi(AicViType viType, bool byHost,
    int outWidth, int outHeight, PIXEL_FORMAT_E pixFormat)
{
    HI_ASSERT(viType > AIC_VIT_NONE && viType < AIC_VIT_BUTT);
    int ret;

    pixFormat = (int)pixFormat < 0 ? PIXEL_FORMAT_YVU_SEMIPLANAR_420 : pixFormat;

    if (g_aic.viOpenRef > 0) {
        LOGW("AIC: start VI{%d} ignore, for viOpenRef=%d\n", viType, g_aic.viOpenRef);
        g_aic.viOpenRef++;
        return 0;
    }

    // create vi+vpss sess
    if (viType == AIC_VIT_SENSOR) {
        ret = AicCreateSnsVpss(outWidth, outHeight, pixFormat);
    } else if (viType == AIC_VIT_HOST) {
        ret = AicCreateVdecVpss(outWidth, outHeight, pixFormat);
    } else {
        HI_ASSERT(0);
        ret = -1;
    }
    if (ret != 0) {
        g_aic.viOpenRef = 0;
        return ret;
    }

    g_aic.viOpenRef = 1;
    g_aic.viType = viType;
    g_aic.viByHost = byHost;
    g_aic.rxPicNum = 0; // 实际仅对VIT_HOST有意义

    AicMonVpss();
    AicStartAiSvc(); // ai service随VI启动

    // 随vi启动初始化统计信息
    if (memset_s(&g_aic.piriodStatis, sizeof(g_aic.piriodStatis), 0, sizeof(g_aic.piriodStatis)) != EOK) {
        HI_ASSERT(0);
    }
    g_aic.piriodStatis.begTime = HiClockMs();

    EventFdInc(g_aic.statusChgFd); // 通知status变更
    return 0;
}

/**
    stop vi.
*/
int AicStopVi(AicViType viType, bool byHost)
{
    HI_ASSERT(viType > AIC_VIT_NONE && viType < AIC_VIT_BUTT);

    if (g_aic.viOpenRef <= 0) {
        LOGW("AIC: stop VO{%x} ignore, for no vi opened\n", viType);
        return 0;
    }
    if (--g_aic.viOpenRef > 0) {
        LOGW("AIC: stop VO{%x} ignore, for viOpenRef=%d\n", viType, g_aic.voOpenRef);
        return 0;
    }

    // 终止VI
    LOGI("AIC: stop VI{%d} ...\n", g_aic.viType);
    HI_ASSERT(g_aic.viType > 0);

    // 要先终止ai计算服务，再destroy MppSess，因为需要先destroy从MppSess中获得的buf
    AicStopAiSvc();

    AicUnmonVpss();
    if (g_aic.viType == AIC_VIT_SENSOR) {
        SceneExit();
    }
    MppSessDestroy(g_aic.viSess);
    g_aic.viSess = NULL;

    g_aic.vpssGrp = -1;
    g_aic.vpssChn0 = -1;
    g_aic.vpssChn1 = -1;

    g_aic.viType = AIC_VIT_NONE;
    g_aic.viByHost = false;

    EventFdInc(g_aic.statusChgFd); // 通知status变更
    LOGI("AIC: stop VI{%d} done\n", g_aic.viType);
    return 0;
}

/**
    start vo.
*/
int AicStartVo(uint32_t voType, int outWidth, int outHeight, PAYLOAD_TYPE_E codecType)
{
    HI_ASSERT(voType == AIC_VOT_UVC || voType == AIC_VOT_IP || voType == AIC_VOT_RTSPD);

    // 支持多个VO并存
    if (g_aic.voOpenRef > 0) {
        LOGW("AIC: start VO{%x} ignore, for VO{%x} exist\n", voType, g_aic.voTypes);
        g_aic.voTypes |= voType;
        g_aic.voOpenRef++;
        EventFdInc(g_aic.statusChgFd); // 通知status变更
        return 0;
    }

    // 启动VENC
    LOGI("AIC: start VENC ...\n");
    HI_ASSERT(!g_aic.vencSess);

    int ret = AicCreateVenc(codecType, outWidth, outHeight);
    HI_CHK_RET(ret, "create VencSess FAIL, ret=%#x\n", ret);
    g_aic.voTypes |= voType;
    g_aic.inFrmIdc = 0; // 可不加
    g_aic.voOpenRef = 1;

    AicMonVenc(); // 获取VENC的out chn fd，并监听之
    EventFdInc(g_aic.statusChgFd); // 通知status变更

    // 通知注册的user
    AicDispStrmOn(codecType, outWidth, outHeight);

    LOGI("AIC: start VENC done\n");
    return 0;
}

/**
    stop vo.
*/
int AicStopVo(uint32_t voType)
{
    HI_ASSERT(voType == AIC_VOT_UVC || voType == AIC_VOT_IP || voType == AIC_VOT_RTSPD);

    if (g_aic.voOpenRef <= 0) {
        LOGW("AIC: stop VO{%x} ignore, for voOpenRef=%d\n", voType, g_aic.voOpenRef);
        return 0;
    }

    g_aic.voTypes &= ~voType;
    if (--g_aic.voOpenRef > 0) {
        LOGW("AIC: stop VO{%x} ignore, for voOpenRef=%d\n", voType, g_aic.voOpenRef);
        EventFdInc(g_aic.statusChgFd); // 通知status变更
        return 0;
    }

    // 终止VO
    LOGI("AIC: stop VO{%x} ...\n", g_aic.voTypes);
    HI_ASSERT(g_aic.vencSess);
    HI_ASSERT(g_aic.vencFd >= 0);

    AicUnmonVenc();
    MppSessDestroy(g_aic.vencSess);
    g_aic.vencSess = NULL;
    g_aic.inFrmIdc = 0;

    EventFdInc(g_aic.statusChgFd); // 通知status变更

    // 不释放，以提升UVC开关性能
    // 需要复位此标志，否则可能没有机会复位它, 当前正在发送init pic也无妨
    g_aic.inTxInitPic = false;
    g_aic.initPicSent = 0;

    // 通知注册的user
    AicDispStrmOff();

    LOGI("AIC: stop VO{%x} done\n", voType);
    return 0;
}

/**
    stop 所有的vi, vo.
    NOTE: 不stop AIC_VOT_UVC，这由UVC触发.
*/
int AicStopVios(bool byHost)
{
    int ret = 0;

    if (g_aic.viType && AicStopVi(g_aic.viType, byHost) < 0) {
        ret = -1;
    }
    if ((g_aic.voTypes & AIC_VOT_IP) && AicStopVo(AIC_VOT_IP) < 0) {
        ret = -1;
    }
    return ret;
}

/**
    AicMng constructor.
*/
static void AicCreate(AicMng* self)
{
    self->inited = true;

    self->evtMon = MainEvtMon();
    self->vpssFd = -1;
    self->vencFd = -1;
    self->vpssGrp = -1;
    self->vpssChn0 = -1;
    self->vpssChn1 = -1;
    self->vdecChn = AIC_VDEC_CHN; // vdecChn是固定的
    self->vencChn = AIC_VENC_CHN; // vencChn是固定的
    self->osdSet = NULL;
    self->viSess = NULL;
    self->vencSess = NULL;
    self->viType = AIC_VIT_NONE;
    self->voTypes = 0;
    self->viByHost = false;
    self->viOpenRef = 0;
    self->voOpenRef = 0;

    VdecCfgInit(&self->vdecCfg, g_aic.vdecChn, PT_MJPEG, VIDEO_MODE_FRAME, VB_SOURCE_MODULE, NULL);
    self->vdecCfg.fps = VDEC_FPS_DEF;

    self->inFrmIdc = 0;
    self->aiCalSvc.evtMon = NULL;
    self->aiCalSvc.sigFd = -1;

    self->aiCalJob.busy = false;
    self->aiCalJob.sigFd = -1;
    self->aiCalJob.res = -1;
    self->aiCalJob.grpId = -1;
    self->aiCalJob.chnId = -1;
    self->aiCalJob.resJson = NULL;
    self->prevResJson = NULL;
    RecurMutexInit(&self->plugMutex);

    INIT_LIST_HEAD(&self->plugInfos);
    INIT_LIST_HEAD(&self->svcPlugs);
    INIT_LIST_HEAD(&self->aiSvcUsers);
    INIT_LIST_HEAD(&self->evtUsers);

    self->statusChgFd = EventFdCreate();
    HI_ASSERT(self->statusChgFd >= 0);
    if (EmAddFd(self->evtMon, self->statusChgFd, FDE_IN, AicOnStatusChg, NULL) < 0) {
        HI_ASSERT(0);
    }

    self->blkCallNum = 0;

    self->inTxInitPic = false;
    self->initPicSent = 0;
    self->initPicBuf = NULL;
    self->initPicLen = 0;
}

/**
    AicMng destructor.
*/
static void AicDestroy(AicMng* self)
{
    struct list_head *node = NULL;
    struct list_head *next = NULL;

    self->inited = false;

    list_for_each_safe(node, next, &self->plugInfos) {
        AiPlugInfoDelete(list_entry(node, AiPlugInfo, lnode));
    }
    int ret = list_empty(&self->svcPlugs);
    HI_ASSERT(ret);

    if (EmDelFd(self->evtMon, self->statusChgFd) < 0) {
        HI_ASSERT(0);
    }
    self->statusChgFd = -1;

    HI_ASSERT(!self->workPlug.itf);
    HI_ASSERT(!self->aiCalSvc.evtMon);
    HI_ASSERT(self->aiCalJob.sigFd < 0);
    HI_ASSERT(!self->aiCalJob.resJson);

    if (self->prevResJson) {
        free(self->prevResJson);
        self->prevResJson = NULL;
    }

    HI_EXP_LOGE(!list_empty(&self->evtUsers), "%d evtUsers leak\n", list_size(&self->evtUsers));
    while (!list_empty(&self->evtUsers)) {
        AicEvtUser *user = list_entry(self->evtUsers.next, AicEvtUser, lnode);
        free(user);
    }

    HI_EXP_LOGE(!list_empty(&self->aiSvcUsers), "%d aiSvcUsers leak\n", list_size(&self->aiSvcUsers));
    while (!list_empty(&self->aiSvcUsers)) {
        AiSvcUser *user = list_entry(self->aiSvcUsers.next, AiSvcUser, lnode);
        free(user);
    }

    HI_ASSERT(!self->viSess);
    HI_ASSERT(!self->vencSess);

    AicFreeInitPic();
    MutexDestroy(&self->plugMutex);

    if (memset_s(self, sizeof(*self), 0, sizeof(*self)) != EOK) {
        HI_ASSERT(0);
    }
}

/*************************************************************************************************
    主要直接对外接口
*************************************************************************************************/
#define SP_AIC_API

/**
    初始化.
*/
void AicInit(void)
{
    HI_ASSERT(!g_aic.inited);

    AicCreate(&g_aic);
    g_aic.initing = true;

    // 创建与venc关联的OsdSet，供AiCalJob使用
    HI_ASSERT(!g_aic.osdSet);
    g_aic.osdSet = OsdsCreate(HI_OSD_BINDMOD_VENC, 0, g_aic.vencChn);
    HI_ASSERT(g_aic.osdSet);

    // 初始化时扫描一次插件
    const char* plugDir = GetCfgStr("aic_mng:plug_dir", PLUG_DIR_DEF);
    PmScanPlug(plugDir, &g_aic.plugInfos);
    LOGI("AIC: detect %d plugs\n", list_size(&g_aic.plugInfos));

    // 加载需要启动管理服务的插件，并启动其服务
    if (GetCfgBool("aic_mng:start_plug_svc", false)) {
        AicStartPlugSvcs();
    }

    // 加载配置的plugs，因此函数总是在app初始化时执行，这里不创建临时线程执行加载
    AicPreloadPlugs();
    g_aic.initing = false;
}

/**
    去初始化.
*/
void AicExit(void)
{
    bool ret = IsMainThrd();
    HI_ASSERT(ret);
    HI_ASSERT(g_aic.inited);
    HI_ASSERT(!g_aic.blkCallNum); // 不能还有耗时操作在执行，由APP确保

    // 手动终止vi/vo，终止vi时会终止AiCalSvc
    if (g_aic.viType) {
        AicStopVi(g_aic.viType, g_aic.viByHost);
    }
    if (g_aic.voTypes) {
        AicStopVo(g_aic.voTypes);
    }

    // 卸载当前插件，由于后台线程均已终止，这里不加锁unload插件也没有问题
    MutexLock(&g_aic.plugMutex);
    AicUnloadPlug();
    MutexUnlock(&g_aic.plugMutex);

    // 终止插件的管理服务，并卸载其插件
    AicStopPlugSvcs();

    // destroy OsdSet
    HI_ASSERT(g_aic.osdSet);
    OsdsClear(g_aic.osdSet);
    OsdsDestroy(g_aic.osdSet);

    AicDestroy(&g_aic);
}

/**
    是否处于UVC strm模式.
*/
bool AicInStrm(void)
{
    bool ret = IsMainThrd();
    HI_ASSERT(ret);
    HI_ASSERT(g_aic.inited);
    return (g_aic.inited && (g_aic.voTypes & AIC_VOT_UVC));
}

/**
    submit 1 jpg.
*/
int AicSubmitJpg(uint8_t* data, int len, bool endOfStrm)
{
    bool res = IsMainThrd();
    HI_ASSERT(res);
    HI_ASSERT(g_aic.inited);
    HI_ASSERT(data);
    VDEC_STREAM_S vdecStrm;
    HI_S32 ret;

    g_aic.rxPicNum++;

    if (g_aic.viType != AIC_VIT_HOST || g_aic.inTxInitPic) {
        if (g_aic.rxPicNum > VDEC_FPS_DEF) { // 避免无意义的log，当discard pic数量较大时才log，相当于每秒log一次
            LOGW("dicard host jpeg, for vdec not ready\n");
        }
        ret = -1;
        goto END;
    }

    if (memset_s(&vdecStrm, sizeof(vdecStrm), 0, sizeof(vdecStrm)) != EOK) {
        HI_ASSERT(0);
    }
    vdecStrm.u64PTS = 0;
    vdecStrm.pu8Addr = data;
    vdecStrm.u32Len = len;
    vdecStrm.bEndOfFrame = HI_TRUE;
    vdecStrm.bEndOfStream = endOfStrm ? HI_TRUE : HI_FALSE;

    ret = HI_MPI_VDEC_SendStream(g_aic.vdecChn, &vdecStrm, 0);
    if (ret != HI_SUCCESS) {
        LOGE("HI_MPI_VDEC_SendStream FAIL, ret=%#x\n", ret);
    }

    END:
        return ret;
}

/**
    get vi type.
*/
AicViType AicGetViType(void)
{
    bool ret = IsMainThrd();
    HI_ASSERT(ret);
    HI_ASSERT(g_aic.inited);
    return g_aic.viType;
}

/**
    get vo type.
*/
uint32_t AicGetVoTypes(void)
{
    bool ret = IsMainThrd();
    HI_ASSERT(ret);
    HI_ASSERT(g_aic.inited);
    return g_aic.voTypes;
}

/**
    设置AI插件.
*/
int AicSetAiPlug(const char* uuid, AiPlugLib* plugLib, bool reload)
{
    uuid = uuid ? uuid : "";
    AiPlugInfo *info = NULL;
    int ret;

    __sync_add_and_fetch(&g_aic.blkCallNum, 1);
    MutexLock(&g_aic.plugMutex);

    const char* curUuid = g_aic.workPlug.uuid ? g_aic.workPlug.uuid : "null";
    const char* curDesc = g_aic.workPlug.desc ? g_aic.workPlug.desc : "null";

    if (!*uuid) { // uuid为null表示unload操作
        LOGI("AIC: set plug {'%s', '%s'} => null ...\n", curUuid, curDesc);
    } else { // load / unload+load操作
        // 查找plug, 找到则说明是正常的load操作
        info = AicFindPlug(uuid);
        HI_EXP_CMD_GOTO(!info, (ret = -1), END, "set plug FAIL, for plug '%s' not found\n", uuid);
        LOGI("AIC: set plug {'%s', '%s'} => {'%s', '%s'} ...\n", curUuid, curDesc, info->uuid, info->desc);
    }

    // 如果要load的插件已经被load，且@param reload为false，则忽略该请求
    bool loaded = g_aic.workPlug.itf && g_aic.workPlug.uuid && strcmp(uuid, g_aic.workPlug.uuid) == 0;
    if (loaded && !reload) {
        LOGW("load plug '%s' ignore, for it loaded\n", uuid);
        ret = 0;
        goto END;
    }

    // 如果请求为unload，而当前没有插件被load，则忽略该请求
    if (!*uuid && !g_aic.workPlug.itf) {
        LOGW("unload plug ignore, for no working plug\n");
        ret = 0;
        goto END;
    }

    // 先unload当前插件
    if (g_aic.workPlug.itf) {
        AicUnloadPlug();
    }

    // 没有指定uuid，说明只是unload插件
    ret = *uuid ? AicLoadPlug(uuid, info) : 0;

    END:
        if (plugLib) {
            if (ret >= 0) {
                *plugLib = g_aic.workPlug;
            } else if (memset_s(plugLib, sizeof(*plugLib), 0, sizeof(*plugLib)) != EOK) {
                HI_ASSERT(0);
            }
            INIT_LIST_HEAD(&plugLib->lnode);
        }

        // 指示status变更，此函数由后台线程执行，不能直接调用AicGenStatusEvt
        if (!g_aic.initing) {
            EventFdInc(g_aic.statusChgFd);
        }

        LOGI("AIC: set plug '%s' %s\n", uuid, (ret < 0 ? "FAIL" : "done"));
        MutexUnlock(&g_aic.plugMutex);
        __sync_sub_and_fetch(&g_aic.blkCallNum, 1);
        return ret;
}

/**
    列表ai插件信息.
*/
int AicListAiPlugs(MemBlk** resJson, int rsvSize)
{
    bool ret = IsMainThrd();
    HI_ASSERT(ret);
    HI_ASSERT(g_aic.inited);
    HI_ASSERT(resJson);
    *resJson = NULL;
    AiPlugInfo *info = NULL;
    struct list_head *node = NULL;
    size_t size = 0;
    int n;

    if (list_empty(&g_aic.plugInfos)) {
        // 仍然继续后面的打包处理，此时ret=0，表示没有插件
        LOGW("list ai plugs with 0 plug\n");
    }

    // 计算输出json所需空间
    size += rsvSize > 0 ? rsvSize : 0;
    size += TINY_BUF_SIZE; // header, 实际24就够了
    n = 0;
    list_for_each(node, &g_aic.plugInfos) {
        info = list_entry(node, AiPlugInfo, lnode);
        HI_ASSERT(info->prof);
        size += strlen(info->prof);
        size += TINY_BUF_SIZE; // 组合为大json时的额外开销，实际16就够了
        n++;
    }

    HI_ASSERT(size > 0);
    *resJson = MemBlkNew2(size, rsvSize);
    HI_ASSERT(*resJson);
    char *buf = (char*)(*resJson)->data;

    // 组合为大json
    int len = (*resJson)->len;
    len += snprintf_s(buf + len, size - len, size - len - 1, "[\n");
    n = 0;
    list_for_each(node, &g_aic.plugInfos) {
        info = list_entry(node, AiPlugInfo, lnode);
        HI_ASSERT(info->prof);

        if (n > 0) {
            len += snprintf_s(buf + len, size - len, size - len - 1, ",\n");
            HI_ASSERT(len < size);
        }
        n++;
        len += snprintf_s(buf + len, size - len, size - len - 1, "%s", info->prof);
        HI_ASSERT(len < size);
    }
    len += snprintf_s(buf + len, size - len, size - len - 1, "\n]");

    HI_ASSERT(len < size);
    (*resJson)->len = len;
    return n; // number of plugs
}

/**
    列表working plugs.
*/
int AicListWorkPlugs(struct list_head* plugInfos)
{
    bool ret = IsMainThrd();
    HI_ASSERT(ret);
    HI_ASSERT(g_aic.inited);
    HI_ASSERT(plugInfos);
    int num = 0;

    // 用小锁来保护对workPlugs的访问，而不能用plugMutex，否则此函数会耗时，增加user使用难度
    HiGlbLock();

    if (g_aic.workPlug.itf) {
        HI_ASSERT(g_aic.workPlug.uuid && *g_aic.workPlug.uuid);
        AiPlugInfo *info = AicFindPlug(g_aic.workPlug.uuid);
        HI_ASSERT(info); // working plug的信息必然还存在

        // clone一份，并添加到plugInfos中
        info = AiPlugInfoClone(info);
        list_add_tail(&info->lnode, plugInfos);
        num++;
    }

    HiGlbUnlock();
    return num;
}

/**
    订阅AI计算结果事件.
*/
int AicSubsAiSvc(uintptr_t cltId, const char* plugUuid, AiResProc proc, void* user)
{
    bool res = IsMainThrd();
    HI_ASSERT(res);
    HI_ASSERT(g_aic.inited);
    int ret = -1;

    // 未指定plugUuid表示订阅所有plugs的服务
    if (!plugUuid || !*plugUuid) {
        AiSvcUser *svcUser = AiSvcUserNew(cltId, "", proc, user); // plugUuid设为null,ensure by xiao
        list_add_tail(&svcUser->lnode, &g_aic.aiSvcUsers);
        return 0;
    }

    // 用小锁来保护对workPlugs的访问，而不能用plugMutex，否则此函数会耗时，增加user使用难度
    HiGlbLock();

    // 判断plugUuid对应的插件是否处于working状态
    if (!g_aic.workPlug.itf) {
        LOGE("subs ai svc FAIL, for no plug in working\n");
        goto END;
    }
    HI_ASSERT(g_aic.workPlug.uuid && *g_aic.workPlug.uuid);
    if (strcmp(g_aic.workPlug.uuid, plugUuid) != 0) {
        LOGE("subs ai svc FAIL, for plug '%s' not in working\n", plugUuid);
        goto END;
    }

    // 创建AiSvcUser并添加到aiSvcUsers中
    // 仍然在锁内执行是为了确保注册期间workPlug不会变更，
    // 以便于将来支持workPlug变更时，注册状态与之保持同步
    AiSvcUser *svcUser = AiSvcUserNew(cltId, plugUuid, proc, user);
    list_add_tail(&svcUser->lnode, &g_aic.aiSvcUsers);
    ret = 0;

    END:
        HiGlbUnlock();
        return ret;
}

/**
    去订阅AI计算结果事件.
    因为只有主线程才会执行对aiSvcUsers的修改，且这里不访问workPlug，因此无需加锁.
*/
int AicUnsubsAiSvc(uintptr_t cltId, const char* plugUuid)
{
    bool ret = IsMainThrd();
    HI_ASSERT(ret);
    struct list_head *node = NULL;
    struct list_head *next = NULL;

    // 匹配后不退出循环，以容错user的重复订阅
    list_for_each_safe(node, next, &g_aic.aiSvcUsers) {
        AiSvcUser *svcUser = list_entry(node, AiSvcUser, lnode);
        if (svcUser->cltId == cltId &&
            (!plugUuid || !*plugUuid || strcmp(svcUser->plugUuid, plugUuid) == 0)) {
            list_del(node);
            free(svcUser);
        }
    }
    return 0;
}

/**
    订阅AIC事件(不含AI服务事件).
*/
int AicSubsEvt(uintptr_t cltId, const char* evtName, AicEvtProc proc, void* user)
{
    bool ret = IsMainThrd();
    HI_ASSERT(ret);
    HI_ASSERT(g_aic.inited);

    AicEvtUser *evtUser = AicEvtUserNew(cltId, evtName, proc, user); // ensure by xiao
    list_add_tail(&evtUser->lnode, &g_aic.evtUsers);

    return 0;
}

/**
    去订阅AIC事件(不含AI服务事件).
*/
int AicUnsubsEvt(uintptr_t cltId, const char* evtName)
{
    bool ret = IsMainThrd();
    HI_ASSERT(ret);
    struct list_head *node = NULL;
    struct list_head *next = NULL;

    // 匹配后不退出循环，以容错user的重复订阅
    list_for_each_safe(node, next, &g_aic.evtUsers) {
        AicEvtUser *evtUser = list_entry(node, AicEvtUser, lnode);
        if (evtUser->cltId == cltId &&
            (!evtName || !*evtName || strcmp(evtUser->evtName, evtName) == 0)) {
            list_del(node);
            free(evtUser);
        }
    }
    return 0;
}

/**
    获得当前状态.
*/
int AicGetStatus(MemBlk* statusJson)
{
    bool res = IsMainThrd();
    HI_ASSERT(res);
    HI_ASSERT(statusJson);
    struct list_head plugInfos = LIST_HEAD_INIT(plugInfos); // working plug info list
    int ret;

    MemBlk *blk = statusJson;
    char *buf = (char*)blk->data;
    int len = blk->len;

    // 编码begin, VI, VO
    len += snprintf_s(buf + len, blk->size - len, blk->size - len - 1, "{ \"vi\": %d", g_aic.viType);
    len += snprintf_s(buf + len, blk->size - len, blk->size - len - 1, ", \"vo\": %u", g_aic.voTypes);

    // 获得workPlugs信息，并编码为JSON数组
    ret = AicListWorkPlugs(&plugInfos);
    HI_EXP_RET(ret < 0, ret, "AicListWorkPlugs() FAIL, ret=%d\n", ret);
    len += snprintf_s(buf + len, blk->size - len, blk->size - len - 1, ", \"workPlugs\": [");
    bool firstField = true;
    while (!list_empty(&plugInfos)) {
        AiPlugInfo *plug = list_entry(plugInfos.next, AiPlugInfo, lnode);
        len += snprintf_s(buf + len, blk->size - len, blk->size - len - 1, "%s{\"uuid\": \"%s\"}",
            (firstField ? " " : ", "), plug->uuid);
        firstField = false;
        list_del(plugInfos.next);
        AiPlugInfoDelete(plug);
    }
    len += snprintf_s(buf + len, blk->size - len, blk->size - len - 1, " ]}");

    blk->len = len;
    return 0;
}

/**
    注册stream user.
*/
bool AicAddStrmUser(const IAicStrmUser* itf, void* user)
{
    bool ret = IsMainThrd();
    HI_ASSERT(ret);
    HI_ASSERT(itf);

    for (size_t i = 0; i < HI_ARRAY_SIZE(g_aic.strmUserTab); i++) {
        AicStrmUser *node = &g_aic.strmUserTab[i];
        if (!node->itf) {
            node->itf = itf;
            node->user = user;
            g_aic.strmUserNum++;
            return true;
        }
    }

    LOGE("add StrmUser FAIL, for tab full, size=%uz\n", g_aic.strmUserNum);
    return false;
}

/**
    注销stream user.
*/
bool AicDelStrmUser(const IAicStrmUser* itf, void* user)
{
    bool ret = IsMainThrd();
    HI_ASSERT(ret);
    HI_ASSERT(itf);

    for (size_t i = 0; i < HI_ARRAY_SIZE(g_aic.strmUserTab); i++) {
        AicStrmUser *node = &g_aic.strmUserTab[i];
        if (node->itf == itf && node->user == user) {
            node->itf = NULL;
            node->user = NULL;
            g_aic.strmUserNum--;
            return true;
        }
    }

    LOGE("del StrmUser FAIL, for user not found\n");
    return false;
}

