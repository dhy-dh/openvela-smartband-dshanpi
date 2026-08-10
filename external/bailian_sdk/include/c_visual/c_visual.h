/*
* Copyright 2025 Alibaba Group Holding Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*     http: *www.apache.org/licenses/LICENSE-2.0
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef __LIB_C_VISUAL_H_
#define __LIB_C_VISUAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "c_utils.h"
#include "c_mmi_cmd.h"

// VISUAL模块支持分辨率    
enum {
    C_VISUAL_FRAMESIZE_96x96,
    C_VISUAL_FRAMESIZE_160x120,
    C_VISUAL_FRAMESIZE_128x128,
    C_VISUAL_FRAMESIZE_176x144,
    C_VISUAL_FRAMESIZE_240x176,
    C_VISUAL_FRAMESIZE_240x240,
    C_VISUAL_FRAMESIZE_320x240,
    C_VISUAL_FRAMESIZE_320x320,
    C_VISUAL_FRAMESIZE_400x296,
    C_VISUAL_FRAMESIZE_480x320,
    C_VISUAL_FRAMESIZE_640x480,
    C_VISUAL_FRAMESIZE_800x600,
    C_VISUAL_FRAMESIZE_1024x768,
    C_VISUAL_FRAMESIZE_1280x720,
    C_VISUAL_FRAMESIZE_1280x1024,
    C_VISUAL_FRAMESIZE_1600x1200,
    C_VISUAL_FRAMESIZE_1920x1080,
    C_VISUAL_FRAMESIZE_720x1280,
    C_VISUAL_FRAMESIZE_864x1536,
    C_VISUAL_FRAMESIZE_2560x1440,
    C_VISUAL_FRAMESIZE_2560x1600,
    C_VISUAL_FRAMESIZE_1080x1920,
    C_VISUAL_FRAMESIZE_2560x1920,
    C_VISUAL_FRAMESIZE_2592x1944,
    C_VISUAL_FRAMESIZE_INVALID,
};

// VISUAL模块支持图片格式
enum {
    C_VISUAL_PIC_FORMAT_JPEG,
    C_VISUAL_PIC_FORMAT_BMP,
    C_VISUAL_PIC_FORMAT_PNG,
    C_VISUAL_PIC_FORMAT_TIF,
    C_VISUAL_PIC_FORMAT_WEBP,
    C_VISUAL_PIC_FORMAT_HEIC,
    C_VISUAL_PIC_FORMAT_JPG = C_VISUAL_PIC_FORMAT_JPEG,
    C_VISUAL_PIC_FORMAT_JPE = C_VISUAL_PIC_FORMAT_JPEG,
    C_VISUAL_PIC_FORMAT_TIFF = C_VISUAL_PIC_FORMAT_TIF
};

// VISUAL模块工作模式
enum {
    C_VISUAL_MODE_NONE = 1 << 0,
    C_VISUAL_MODE_VQA = 1 << 1,
    C_VISUAL_MODE_LIVE_AI = 1 << 2,
    C_VISUAL_MODE_OMNI = 1 << 3,
    C_VISUAL_MODE_TRANSLATE = 1 << 4,
    C_VISUAL_MODE_PROACTOR = 1 << 5,
    C_VISUAL_MODE_REMINDER = 1 << 6,
    C_VISUAL_MODE_GUIDE = 1 << 7,
};

// VISUAL模块传入数据类型
enum {
    C_VISUAL_DATA_BASE64,
    C_VISUAL_DATA_URL,
};

// VISUAL模块回调事件
enum {
    C_VISUAL_EVENT_VQA_START,       // 此事件在开启拍照问答时触发，可以在该事件回调中激活摄像头并开始拍照
    C_VISUAL_EVENT_VQA_END,         // 此事件在结束拍照问答时触发，建议在该事件回调关闭摄像头
    C_VISUAL_EVENT_LIVEAI_START,    // 此事件在开启视频通话时触发（含极速），建议在该事件回调触开启摄像头
    C_VISUAL_EVENT_LIVEAI_ACTION,   // 此事件在开始视频通话抽帧时触发，可以在该事件回调中进行图片采集
    C_VISUAL_EVENT_LIVEAI_STOP,     // 此事件在关闭视频通话时触发，建议在该事件回调关闭摄像头
    C_VISUAL_EVENT_PROACTOR_START,  // 此事件在开始主动交互时触发，建议在该事件回调触开启摄像头
    C_VISUAL_EVENT_PROACTOR_STOP,   // 此事件在退出主动交互时触发，建议在该事件回调关闭摄像头
};

typedef struct _c_visual_config_t {
    uint8_t frame_size;
    uint8_t image_format;
    uint8_t visual_mode;
    uint8_t data_type;
    uint32_t image_size;
    uint32_t fps;
    c_mmi_cmd_event_callback event_callback;
} c_visual_config_t;

/**
 * @brief 设置VISUAL普通模式下的参数
 *
 * @param config 	
 *        视觉交互模式设置参数，此参数以最后一次设置为准，并且此参数只有在新一轮对话开始时生效
 * @return 0代表成功，非0代表失败
 */
int32_t c_visual_config(c_visual_config_t* config);

int32_t c_visual_update_url(char *url, uint32_t url_len);
int32_t c_visual_task_handle(void);

// uint32_t c_visual_update_image(uint8_t *img, uint32_t size);
int32_t c_visual_data_reset(void);
uint8_t *c_visual_image_get_buffer(uint32_t *size);
int32_t c_visual_image_action(uint32_t image_size);

#ifdef __cplusplus
}
#endif
#endif