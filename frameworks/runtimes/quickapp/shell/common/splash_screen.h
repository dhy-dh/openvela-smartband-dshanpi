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
#pragma once
#include <string>

#include "quickapp.h"

#include <animengine/anim_api.h>
#include <lvgl/lvgl.h>

namespace shell {
class SplashScreen {
public:
    SplashScreen(NativeWidgetHandle rootWidget, anim_engine_handle_t anim_engine);
    ~SplashScreen();
    lv_obj_t* GetContainerWidget();
    void Show(const std::string& icon_path);
    void Finish();

private:
    class FadeOutAnimation {
    public:
        FadeOutAnimation(anim_engine_handle_t anim_engine, lv_obj_t* target, int duration, int opacity)
            : anim_engine_(anim_engine)
            , animationID_(-1)
            , target_(target)
            , duration_(duration)
            , opacity_(opacity)
        {
        }

        ~FadeOutAnimation() = default;
        void Start();
        void Stop();

    private:
        anim_engine_handle_t anim_engine_;
        ANIMID animationID_;
        lv_obj_t* target_;
        int duration_;
        int opacity_;
    };

    std::unique_ptr<FadeOutAnimation> fadeout_animation_;
    anim_engine_handle_t anim_engine_;
    lv_obj_t* root_widget_; // 顶层根控件，由 miwear 模块创建，由 SplashScreen模块销毁, 生命周期与应用主界面绑定
    lv_obj_t* container_root_; // QuickApp 的根容器，作为 root_widget_的直接子对象，用于管理 QuickApp 的业务控件
    lv_obj_t* image_root_; // 启动 logo 图片控件，作为 root_widget_ 的直接子对象，在container_root_ 之后中创建，用于显示启动 logo 图片
};
} // namespace ferry
