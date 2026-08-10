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

#pragma once

#include "../common/shell_app.h"
#include "app/Activity.h"

typedef struct MessageLoop MessageLoop;

namespace os {
namespace app {
    class QuickActivity : public Activity {
    public:
        QuickActivity();
        ~QuickActivity();
        void onCreate() override;
        void onStart() override;
        void onResume() override;
        void onPause() override;
        void onStop() override;
        void onDestroy() override;
        void onRestart() override;
        void onBackPressed() override;
        void onNewIntent(const Intent& intent) override;
        void onReceiveIntent(const Intent& intent) override;

    private:
        std::shared_ptr<shell::ShellApp> shell_app_;
        MessageLoop* ui_message_loop_;
        bool is_started_;
        bool is_destroying_;
        bool is_new_intent_;
    };

} // namespace app
} // namespace os
