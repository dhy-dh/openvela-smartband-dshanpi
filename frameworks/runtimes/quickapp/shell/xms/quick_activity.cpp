/*
 * Copyright (C) 2023 Xiaomi Corporation
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

#include "quick_activity.h"

#ifdef CONFIG_FEATURE_RUST_MODULES
#include "internal/feature_rust.h"
#endif

#include <ash/message_loop/cmessage_loop.h>

#include "BaseWindow.h"
#include "common/shell_log.h"
#include "common/shell_profile.h"
#include "pm/PackageManager.h"

#define HAP_PREFIX "hap://app/"

namespace os {
namespace app {
    class ActivityDelegate : public shell::ShellApp::Delegate {
    public:
        ActivityDelegate(Activity* context)
            : context_(context)
        {
        }

        bool onNavigateToApp(const QAppRouteInfo* info)
        {
            std::string uri_str(info->uri);
            std::string query_str(info->querystring ? info->querystring : "");
            if (uri_str.find(HAP_PREFIX) != 0) {
                SHELL_LOG_WARN("uri should start with %s!", HAP_PREFIX);
                return false;
            }

            size_t pos_pkg = strlen(HAP_PREFIX);
            size_t pos_path = uri_str.find('/', pos_pkg);
            size_t pos_query = uri_str.find('?', pos_pkg);
            std::string pkg;

            if (pos_path == std::string::npos && pos_query == std::string::npos) {
                pkg = uri_str.substr(pos_pkg);
            } else if (pos_path == std::string::npos && pos_query != std::string::npos) {
                pkg = uri_str.substr(pos_pkg, pos_query - pos_pkg);
            } else {
                pkg = uri_str.substr(pos_pkg, pos_path - pos_pkg);
            }

            os::app::Intent intent;
            intent.setTarget(pkg);
            std::string path_with_params;

            if (pos_path != std::string::npos) {
                if (pos_query != std::string::npos) {
                    path_with_params = uri_str.substr(pos_path, pos_query - pos_path);
                } else {
                    path_with_params = uri_str.substr(pos_path);
                }
            }

            if (!query_str.empty() || pos_query != std::string::npos) {
                path_with_params += (!query_str.empty() ? "?" + query_str : uri_str.substr(pos_query));
            }
            intent.setData(path_with_params);

            context_->startActivity(intent);
            return true;
        }

    private:
        Activity* context_;
    };

    class ActivityAppObserver : public shell::ShellApp::Observer {
    public:
        ActivityAppObserver(QuickActivity* activity)
            : context_(activity)
        {
        }
        void onNotifyEvent(shell::ShellApp* app, int event_id, void* params) override
        {
            switch (event_id) {
            case QAPP_EVENT_REQ_EXIT:
                context_->stopApplication();
                break;
            default:
                break;
            }
        }

    private:
        Activity* context_;
    };

    std::string handle_package_name(const char* pkg)
    {
        std::string_view pkgName(pkg);

        // 移除 HAP_PREFIX 前缀
        auto pos = pkgName.find(HAP_PREFIX);
        if (pos != std::string_view::npos) {
            pkgName.remove_prefix(pos + strlen(HAP_PREFIX));
        }

        // 查找分隔符位置
        auto endPos = std::min(pkgName.find('/'), pkgName.find('?'));

        // 如果找到分隔符，截取到该位置
        if (endPos != std::string_view::npos) {
            return std::string(pkgName.substr(0, endPos));
        }

        return std::string(pkgName);
    }

    QuickActivity::QuickActivity()
        : is_started_(false)
        , is_destroying_(false)
        , is_new_intent_(true)
    {
    }

    QuickActivity::~QuickActivity()
    {
    }

    void QuickActivity::onCreate()
    {
        PROFILE_LOG_BEGIN("QuickActivity::launchQuickApp", getPackageName().c_str());
        SHELL_LOG_INFO("Activity onCreate");
        uv_loop_t* main_loop = getMainLoop()->get();
        ui_message_loop_ = MessageLoop_CreateForUV(main_loop);
        GUIWidgetInit();
        bool async_mode = false;
#ifdef CONFIG_QUICKAPP_ACTIVITY_ASYNC
        async_mode = true;
#endif
        std::string uri = getContext()->getPackageName();
        if (!getIntent().mData.empty()) {
            uri.append(getIntent().mData);
        }
        shell_app_ = shell::ShellApp::Create(uri.c_str(), ui_message_loop_, std::make_unique<ActivityDelegate>(this), async_mode);
        is_new_intent_ = false;

        shell_app_->setUILoop(main_loop);

        shell_app_->addFeatureManagerUserData("nativeContext", this);

        shell_app_->addObserver(std::make_unique<ActivityAppObserver>(this));

        shell_app_->create((NativeWidgetHandle)getWindow()->getRoot());
        PROFILE_LOG_BEGIN("QuickActivity::launchQuickApp", getPackageName().c_str());
    }

    void QuickActivity::onStart()
    {
        SHELL_LOG_INFO("Activity onStart");
    }

    void QuickActivity::onResume()
    {
        SHELL_LOG_INFO("Activity onResume");
        if (!is_started_) {
            is_started_ = true;
        } else if (is_new_intent_) {
            std::string url = std::string(HAP_PREFIX) + handle_package_name(getPackageName().c_str());
            if (!getIntent().mData.empty()) {
                url.append(getIntent().mData);
            }
            SHELL_LOG_INFO("new intent url: %s", url.c_str());
            shell_app_->route(url.c_str());
            is_new_intent_ = false;
        } else {
            shell_app_->show();
        }
    }

    void QuickActivity::onRestart()
    {
        SHELL_LOG_INFO("Activity onRestart");
        shell_app_->show();
    }

    void QuickActivity::onNewIntent(const Intent& intent)
    {
        SHELL_LOG_INFO("Activity onNewIntent");
        // need re route uri
        is_new_intent_ = true;
    }

    void QuickActivity::onReceiveIntent(const Intent& intent)
    {
        SHELL_LOG_INFO("Activity onReceiveIntent");
        if (intent.mAction == "action.system.BOOT_READY") {
            /* TODO: */
#ifdef USE_INSPECTOR
        } else if (intent.mAction == "action.app.HELP") {
            SHELL_LOG_INFO("Heap Profiling Tools Usage:    \
            \naction.app.HEAP_DUMP                          \
            \n:Dump js heap to a file in /data/app          \
            \naction.app.MEMORY_DUMP                        \
            \n:Print the js heap basic info                 \
            \naction.app.RUN_GC                             \
            \n:Trigger a gc for the js heap                 \
            \naction.app.START_HEAP_TIMELINE                \
            \naction.app.STOP_HEAP_TIMELINE                 \
            \n:Dump js heap time line to a file in /data/app\
            \naction.app.START_CPU_PROFILING                \
            \naction.app.STOP_CPU_PROFILING                 \
            \n:Dump js cpu profiling to a file in /data/app");

        } else if (intent.mAction == "action.app.HEAP_DUMP") {
            InspectorDumpAppHeap(getQuickApp(), getPackageName().c_str());
        } else if (intent.mAction == "action.app.MEMORY_DUMP") {
            InspectorDumpJSMem(getQuickApp());
        } else if (intent.mAction == "action.app.RUN_GC") {
            SHELL_LOG_INFO("MemoryDump Before GC:\n");
            InspectorDumpJSMem(getQuickApp());
            QApplicationRunGC(getQuickApp());
            SHELL_LOG_INFO("MemoryDump After GC:\n");
            InspectorDumpJSMem(getQuickApp());
        } else if (intent.mAction == "action.app.START_HEAP_TIMELINE") {
            InspectorStartAppHeapTimeline(getQuickApp(), getPackageName().c_str());
        } else if (intent.mAction == "action.app.STOP_HEAP_TIMELINE") {
            InspectorStopAppHeapTimeline(getQuickApp());
        } else if (intent.mAction == "action.app.START_CPU_PROFILING") {
            InspectorStartAppCPUProfiling(getQuickApp(), getPackageName().c_str());
        } else if (intent.mAction == "action.app.STOP_CPU_PROFILING") {
            InspectorStopAppHeapTimeline(getQuickApp());
#endif
        } else {
            /* TODO: others action or target */
        }
    }

    void QuickActivity::onPause()
    {
        SHELL_LOG_INFO("Activity onPause");
    }

    void QuickActivity::onStop()
    {
        SHELL_LOG_INFO("Activity onStop");
        shell_app_->hide();
    }

    void QuickActivity::onDestroy()
    {
        if (is_destroying_) {
            SHELL_LOG_INFO("QuickActivity is destroying");
            return;
        }

        is_destroying_ = true;
        SHELL_LOG_INFO("Activity onDestroy");
        shell_app_->stop();
        shell_app_->waitUIDestroy();
        shell_app_->release();
        shell_app_.reset();
        MessageLoop_Destroy(ui_message_loop_);
        GUIWidgetUninit();
    }

    void QuickActivity::onBackPressed()
    {
        SHELL_LOG_INFO("Activity onBackPressed");
        shell_app_->backpress();
    }

} // namespace app
} // namespace os
