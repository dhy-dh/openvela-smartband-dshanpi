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

#include "quick_application.h"

#include "common/shell_log.h"
#include "quick_activity.h"

#include "quickapp_inspector.h"

namespace os {
namespace app {
    QuickApplication::QuickApplication()
    {
        InspectStartServer();
    }

    QuickApplication::~QuickApplication()
    {
        InspectStopServer();
    }

    void QuickApplication::onCreate()
    {
        SHELL_LOG_INFO("Application onCreate");
        REGISTER_ACTIVITY(QuickActivity);
    }

    void QuickApplication::onForeground()
    {
        SHELL_LOG_INFO("Application onForeground");
    }

    void QuickApplication::onBackground()
    {
        SHELL_LOG_INFO("Applicaiton onBackground");
    }

    void QuickApplication::onDestroy()
    {
        SHELL_LOG_INFO("Application onDestroy");
    }

#define APPLICATION QuickApplication
#include <app/AppMain.h>
} // namespace app
} // namespace os
