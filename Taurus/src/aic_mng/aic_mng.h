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

#ifndef AIC_MNG_H
#define AIC_MNG_H

#include <stdint.h>
#include <stdbool.h>

#include "mpp_sess.h"
#include "plug_mng.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
    video input type.
    同一时刻，只支持一个来源。
*/
typedef enum AicViType {
    AIC_VIT_NONE = 0,
    AIC_VIT_SENSOR, // from local sensor, the default
    AIC_VIT_HOST, // from host (PC/TV)
    AIC_VIT_FILE, // from file, 暂未支持
    AIC_VIT_CLOUD, // from cloud，暂未支持
    AIC_VIT_BUTT
}AicViType;

/**
    video output type.
    非互斥关系，可共存。
*/
#define AIC_VOT_UVC     HI_BIT0 // output via UVC
#define AIC_VOT_IP      HI_BIT1 // output via IP
#define AIC_VOT_RTSPD   HI_BIT4 // output via RTSPD

/**
    AI计算结果处理函数类型.
*/
typedef void (*AiResProc)(void* user,
    uint32_t cltId, const char* plugUuid, const char* resJson, int resLen);

/**
    事件处理函数类型.
*/
typedef void (*AicEvtProc)(void* user,
    uint32_t cltId, const char* evtName, const char* evtJson, int evtLen);

/**
    aic stream user接口.
*/
typedef struct IAicStrmUser {
    bool (*OnStrmOn)(void* user, int vencChn, PAYLOAD_TYPE_E codecType, int width, int height);
    void (*OnStrmOff)(void* user);
    void (*OnVencFrm)(void* user, VencFrm* frm); // VENC out frame
    void (*OnVideoFrm)(void* user, VIDEO_FRAME_INFO_S* frm); // VI/VPSS out frame
}IAicStrmUser;

/**
    前向申明.
*/
struct uvc_device_ext; // aic user会包含此头文件，前向申明，以避免此头文件include <uvc_dev.h>.

/**
    AicMng init/exit.
*/
void AicInit(void);
void AicExit(void);

/**
    是否处于strm传输状态，供histm_aic使用.
    用于替换原有的HiStmCheckUvcStatus()。
*/
bool AicInStrm(void);

/**
    submit 1 jpg to venc.
*/
int AicSubmitJpg(uint8_t* data, int len, bool endOfStrm);

/**
    get vi type.
*/
AicViType AicGetViType(void);

/**
    get vo types.
*/
uint32_t AicGetVoTypes(void);

/**
    start/stop VI.
*/
int AicStartVi(AicViType viType, bool byHost,
    int outWidth, int outHeight, PIXEL_FORMAT_E pixFormat);
int AicStopVi(AicViType viType, bool byHost);

/**
    start/stop VO.

    @param voType 不能为AIC_VOT_UVC。
    @param codecType 目前仅支持MJPEG。
*/
int AicStartVo(uint32_t voType, int width, int height, PAYLOAD_TYPE_E codecType);
int AicStopVo(uint32_t voType);

/**
    stop 所有的vi, vo.
    NOTE: 不stop AIC_VOT_UVC，这由UVC触发.
*/
int AicStopVios(bool byHost);

/**
    发送初始frame.
    目前仅供UVC user使用，以绕开UVC必须通过发送数帧以turn strm on的问题.
*/
int AicTxInitPic(void);

/**
    设置ai插件.

    @param uuid 为null string表示unload当前插件，否则表示加载@param uuid指定的插件。
    @param reload 为true时，即便设置的新插件当前已加载，也重新加载之。
    @param plug 返回加载的插件的信息，如果是加载操作的话。
*/
int AicSetAiPlug(const char* uuid, AiPlugLib* plug, bool reload);

/**
    列表ai插件信息.

    @param resJson[out]: 返回json格式的插件列表。
    @param rsvSize[in]: 此函数在创建resJson对象时，在其头部保留的空间(字节长度).
    @return 成功则返回plug的数量，否则返回负值.
*/
int AicListAiPlugs(MemBlk** resJson, int rsvSize);

/**
    列表working plugs.

    @param plugInfos[out]: 输出working plugs信息。链表节点为AiPlugInfo。
    @return 成功，则返回输出的working plugs的数目，可能为0。否则返回负数，其值表示错误码。
*/
int AicListWorkPlugs(struct list_head* plugInfos);

/**
    订阅AI服务事件.

    订阅后，当AI计算产生结果时，会调用proc()输出计算结果.

    @param cltId[in]: client ID.
    @param plugUuid[in]: 表示特定AI服务的插件的ID.
    @param proc[in]: 输出计算结果的处理函数.
    @param user[in]: 作为user参数传入aiResProc()的任意类型的指针.
    @return 成功返回0，否则返回负数，其值表示错误码.
*/
int AicSubsAiSvc(uintptr_t cltId, const char* plugUuid, AiResProc proc, void* user);

/**
    去订阅AI服务事件.

    去订阅此前通过AicSubsAiSvc()订阅的AI服务.

    @param cltId[in]: client ID.
    @param plugUuid[in]: 表示特定AI服务的插件的ID.
    @return 成功返回0，否则返回负数，其值表示错误码.
*/
int AicUnsubsAiSvc(uintptr_t cltId, const char* plugUuid);

/**
    订阅AIC事件(不含AI服务事件).

    订阅后，当AIC产生事件时，会调用proc()向用户提交事件.

    @param cltId[in]: client ID.
    @param evtName[in]: 表示待订阅的事件名称，置为NULL表示订阅所有事件.
    @param proc[in]: 事件处理函数.
    @param user[in]: 作为user参数传入aicEvtProc()的任意类型的指针.
    @return 成功返回0，否则返回负数，其值表示错误码.
*/
int AicSubsEvt(uintptr_t cltId, const char* evtName, AicEvtProc proc, void* user);

/**
    去订阅AIC事件(不含AI服务事件).

    去订阅此前通过AicSubsEvt()订阅的事件.

    @param cltId[in]: client ID.
    @param evtName[in]: 表示去订阅的事件名称，置为NULL表示去订阅所有事件.
    @return 成功返回0，否则返回负数，其值表示错误码.
*/
int AicUnsubsEvt(uintptr_t cltId, const char* evtName);

/**
    获得当前状态.
    @param resJson[out]: 返回JSON格式的状态。
*/
int AicGetStatus(MemBlk* resJson);

/**
    注册stream user.
*/
bool AicAddStrmUser(const IAicStrmUser* itf, void* user);

/**
    注销stream user.
*/
bool AicDelStrmUser(const IAicStrmUser* itf, void* user);

#ifdef __cplusplus
}
#endif

#endif // AIC_MNG_H
