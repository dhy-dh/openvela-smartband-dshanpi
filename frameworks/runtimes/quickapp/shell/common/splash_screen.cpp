/*
 * Copyright (C) 2025 Xiaomi Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <nuttx/config.h>

#ifdef CONFIG_QUICKAPP_SPLASH_SCREEN_ENABLE
#include "splash_screen.h"

#include <limits.h>

#include <filesystem>
#include <rapidjson/allocators.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <string>
#include <system_error>

#include "common/shell_log.h"

namespace fs = std::filesystem;

namespace shell {
#ifdef CONFIG_SPLASH_SCREEN_FADE_OUT_TIME
#define FADE_OUT_DURATION_MS CONFIG_SPLASH_SCREEN_FADE_OUT_TIME
#else
#define FADE_OUT_DURATION_MS 500
#endif

#ifdef CONFIG_SPLASH_SCREEN_FADE_OUT_OPACITY
#define FADE_OUT_DURATION_OPACITY CONFIG_SPLASH_SCREEN_FADE_OUT_OPACITY
#else
#define FADE_OUT_DURATION_OPACITY 0
#endif

SplashScreen::SplashScreen(NativeWidgetHandle root_widget, anim_engine_handle_t anim_engine)
    : anim_engine_(anim_engine)
    , root_widget_((lv_obj_t*)root_widget)
    , image_root_(nullptr)
{
    // 创建quickapp root容器节点
    container_root_ = lv_obj_create((lv_obj_t*)root_widget);
    lv_obj_set_size(container_root_, lv_display_get_horizontal_resolution(nullptr),
        lv_display_get_vertical_resolution(nullptr));
}

SplashScreen::~SplashScreen()
{
    if (fadeout_animation_) {
        fadeout_animation_->Stop();
    }

    if (image_root_) {
        lv_obj_del(image_root_);
        image_root_ = nullptr;
    }

    if (root_widget_) {
        lv_obj_del(root_widget_);
        root_widget_ = nullptr;
    }
}

void SplashScreen::Show(const std::string& icon)
{
    // const std::string& path = IconPath();
    const char* icon_path = icon.c_str();
    auto ec = std::error_code();
    if (!fs::exists(icon_path, ec)) {
        SHELL_LOG_ERROR("%s: file path not exist, skip show app icon image...",
            icon_path);
        return;
    }

    // 显示启动logo页
    image_root_ = lv_img_create((lv_obj_t*)root_widget_);
    lv_img_set_src(image_root_, icon_path);
    lv_obj_set_size(image_root_, lv_display_get_horizontal_resolution(nullptr),
        lv_display_get_vertical_resolution(nullptr));
    lv_obj_align(image_root_, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(image_root_, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(image_root_, LV_OPA_COVER, 0);
}

void SplashScreen::Finish()
{
    if (image_root_) {
        // 渐隐动画
        fadeout_animation_ = std::make_unique<FadeOutAnimation>(anim_engine_,
            image_root_, FADE_OUT_DURATION_MS, FADE_OUT_DURATION_OPACITY);
        fadeout_animation_->Start();
    }
}

lv_obj_t* SplashScreen::GetContainerWidget() { return container_root_; }

static std::string toPrettyString(const rapidjson::Document& doc)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

void SplashScreen::FadeOutAnimation::Start()
{
    rapidjson::Document doc(rapidjson::kObjectType);
    doc.SetObject();

    /*----- config.ease 数组 -----*/
    rapidjson::Value easeVal(rapidjson::kArrayType);
    easeVal.PushBack("linear", doc.GetAllocator())
        .PushBack(duration_ / 1000.0, doc.GetAllocator());

    /*----- config 对象 -----*/
    rapidjson::Value configVal(rapidjson::kObjectType);
    configVal.AddMember("ease", easeVal.Move(), doc.GetAllocator());

    /*----- toState 对象 -----*/
    rapidjson::Value toStateVal(rapidjson::kObjectType);
    toStateVal.AddMember("opacity", opacity_, doc.GetAllocator());

    doc.AddMember("config", configVal.Move(), doc.GetAllocator());
    doc.AddMember("toState", toStateVal.Move(), doc.GetAllocator());

    const std::string& jsonStr = toPrettyString(doc);
    SHELL_LOG_DEBUG("splash screen fade out animation json config:%s",
        jsonStr.c_str());

    anim_engine_handle_t animHandle = anim_engine_;
    anim_create(animHandle, &animationID_, jsonStr.c_str());
    anim_listener(
        animHandle, animationID_,
        [](anim_layer_t* layer, const anim_status_type_t status) {
            if (status == ANIM_ST_END) {
                SHELL_LOG_INFO("splash screen fade out animation finished");
                // 动画完成后隐藏节点
                lv_obj_add_flag((lv_obj_t*)layer->user_data, LV_OBJ_FLAG_HIDDEN);
            }
        },
        [](anim_layer_t* layer, const anim_value_t* value) {
            lv_obj_set_style_bg_opa((lv_obj_t*)layer->user_data,
                value->v.fv * LV_OPA_COVER, 0);
        },
        (anim_user_data_t)target_);
    anim_start(animHandle, animationID_, (anim_layer_object_t)target_,
        ANIM_LT_IMAGE);
}

void SplashScreen::FadeOutAnimation::Stop()
{
    anim_engine_handle_t animHandle = anim_engine_;
    anim_remove(animHandle, animationID_);
}

} // namespace ferry
#endif // CONFIG_QUICKAPP_SPLASH_SCREEN_ENABLE
