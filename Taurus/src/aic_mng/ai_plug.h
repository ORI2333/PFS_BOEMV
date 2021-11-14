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

#ifndef AI_PLUG_H
#define AI_PLUG_H

#include <stdint.h>
#include <stdbool.h>
#include "hi_comm_video.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
    插件应遵循的约定.
*/
#define AI_PLUG_MAGIC       0x3D2E837B // 插件应返回的magic
#define AI_PLUG_EXTNAME     "plug" // 插件文件名后缀

/*
    避免#include "mmp_img.h"而直接申明要使用的OsdSet类型.
*/
typedef struct OsdSet OsdSet;

/**
    AIC插件接口.

    插件应实现该接口，并通过aic_plug_itf对外暴露。

    hiopenais在获得IAicPlug后，会调用load()加载模型，并调用cal()执行一帧图片的计算。
    当不再需要使用插件时，hiopenais会调用unlock()卸载模型。

    这里的模型时抽象概念，由插件来定义其含义。插件可以创建任意类型的对象，通过
    load()返回给hiopenais，hiopenais会透明使用它来调用插件的接口接口。
*/
typedef struct AiPlug {
    /**
        获取插件profile.

        插件应维持返回的profile的有效性，hiopenais不会复制它。

        @return 返回插件的profile.
    */
    const HI_PCHAR (*Prof)(void);

    /**
        加载模型.

        需要使用插件的计算服务时，hiopenais会调用此函数。

        目前的定义，在插件的生命周期内，hiopenais不会在unload已load的模型前，再次调用
        此函数重新load。

        @param model[out]: 输出插件创建的模型对象。
        @param osds[in]: hiopenais创建的OsdSet，插件可维持它，并用其在resFrm中叠加文字。
        @return 成功则返回0，否则返回负数，其值表示错误码。
    */
    int (*Load)(uintptr_t* model, OsdSet* osds);

    /**
        卸载模型.

        hiopenais调用此函数来卸载模型。此后hiopenais可能会重新调用load()加载模型。

        @param model[in]: load()返回的@param model。
        @return 成功则返回0，否则返回负数，其值表示错误码。
    */
    int (*Unload)(uintptr_t model);

    /**
        计算一张图片.

        hiopenais传入一帧图片@param srcFrm，插件进行AI计算。插件可以将计算结果以文字、
        图形(如矩形)叠加到@param resFrm中，并生成结构化JSON数据以表示计算结果。

        hiopenais已经把@param srcFrm的分辨率按插件profile中的要求予以调整。但像素格式，
        如YUV/RGB，以及单通道/多通道等hiopenais未做转换，需要插件处理。将来hiopenais会做
        更进一步的处理，以减轻插件开发的负担。

        需要注意的是@srcFrm与@dstFrm的分辨率可能是不同的，插件在resFrm中叠加信息，
        及生成resJson时需要做坐标转换。插件可以用工具函数rect_box_tran()来转换。

        resJson由插件负责分配，由hiopenais负责释放。

        @param model[in]: load()返回的@param model。
        @param srcFrm[in]: 待计算的图片帧。
        @param resFrm[in|out]: 结果帧，插件可在其上叠加图形、文字等信息。
        @param resJson[out]: 结构化结果数据，JSON格式。其结构由插件自定义。
        @return 成功则返回0，否则返回负数，其值表示错误码。
    */
    int (*Cal)(uintptr_t model, VIDEO_FRAME_INFO_S* srcFrm,
        VIDEO_FRAME_INFO_S* resFrm, char** resJson);

    /**
        启动/终止插件管理服务.

        插件可以自定义管理服务，hiopenais会在启动/终止时自动启动/终止该服务。

        插件管理服务的含义由插件定义，hiopenais不做解释。典型的案例时识别类算法的底库管理，
        插件如果需要在Camera中支持底库管理，可以定义服务来管理底库。

        插件可以使用lite_httpd提供的http服务，在host端通过http与插件管理服务通信。

        startSvc()启动服务。
        stopSvc()终止服务。

        @return 成功则返回0，否则返回负数，其值表示错误码。
    */
    int (*StartSvc)(void);
    int (*StopSvc)(void);
}   AiPlug;

/**
    获取插件接口.

    @param magic[out]: 插件通过它返回AIC_PLUG_MAGIC。
    @return 返回插件实现的IAicPlug接口。
*/
const AiPlug* AiPlugItf(uint32_t* magic);
typedef const AiPlug* (*AIPlugItfFunc)(uint32_t* magic);

#ifdef __cplusplus
}
#endif

#endif // AI_PLUG_H
