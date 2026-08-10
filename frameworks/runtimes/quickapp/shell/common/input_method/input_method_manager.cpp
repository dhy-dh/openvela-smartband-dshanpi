/*
 * Copyright (C) 2024 Xiaomi Corporation
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

#include "common/input_method/input_method_manager.h"

#include "ash/logging/logging.h"
#include "ash/memory/global_variable.h"
#include "uikit/uikit.h"

namespace shell {

namespace {
    vg_input_context_t* InputContextCreate(void* user_data)
    {
        ASH_DCHECK(InputMethodManager::GetInstance());
        return reinterpret_cast<vg_input_context_t*>(InputMethodManager::GetInstance()->CreateInputMethod(user_data));
    }

    void InputContextDestroy(vg_input_context_t* context)
    {
        ASH_DCHECK(InputMethodManager::GetInstance());
        InputMethodManager::GetInstance()->DestroyInputMethod(reinterpret_cast<InputMethod*>(context));
    }

    void InputContextRequire(vg_input_context_t* context, const char* initial_text)
    {
        ASH_DCHECK(InputMethodManager::GetInstance());
        InputMethodManager::GetInstance()->Require(reinterpret_cast<InputMethod*>(context), initial_text ? initial_text : "");
    }

    void* InputContextGetUserData(vg_input_context_t* context)
    {
        return reinterpret_cast<InputMethod*>(context)->GetUserData();
    }

    void InputContextSetTextChanged(vg_input_context_t* context, vg_input_text_changed_callback callback)
    {
        reinterpret_cast<InputMethod*>(context)->SetTextChanged([context, callback](const std::string& text) {
            callback(context, text.c_str());
        });
    }

    void InputContextSetFocusLost(vg_input_context_t* context, vg_input_focus_lost_callback callback)
    {
        reinterpret_cast<InputMethod*>(context)->SetFocusLost([context, callback]() {
            callback(context);
        });
    }

    vg_input_context_ops_t input_context_ops = {
        .create = InputContextCreate,
        .destroy = InputContextDestroy,
        .require = InputContextRequire,
        .get_user_data = InputContextGetUserData,
        .set_text_changed_callback = InputContextSetTextChanged,
        .set_focus_lost_callback = InputContextSetFocusLost,
    };

    extern "C" {
    // TODO(xuyan): There is no way to include header file where these functions
    // are declared. Pre-declare them here directly.
    int quickapp_launch_app(const char* package, void* fingerprint);
    int quickapp_stop_app(const char* package);
    }

    ash::GlobalVariable<InputMethodManager*> instance = nullptr;

} // namespace

InputMethod::InputMethod(void* user_data)
    : user_data_(user_data)
{
}

InputMethod::~InputMethod() = default;

void* InputMethod::GetUserData()
{
    return user_data_;
}

void InputMethod::SetTextChanged(TextChangedCB text_changed)
{
    text_changed_ = text_changed;
}

void InputMethod::SetFocusLost(FocusLostCB focus_lost)
{
    ASH_LOG("IMM", INFO) << "SetFocusLost";
    focus_lost_ = focus_lost;
}

void InputMethod::OnTextChanged(const std::string& text)
{
    if (text_changed_) {
        text_changed_(text);
    }
}

void InputMethod::OnFocusLost()
{
    ASH_LOG("IMM", INFO) << "OnFocusLost";
    if (focus_lost_) {
        focus_lost_();
    }
}

InputMethodManager::InputMethodManager()
    : current_input_method_(nullptr)
    , current_input_method_controller_(nullptr)
    , input_method_application_started_(false)
{
    ASH_CHECK(instance.Get() == nullptr);
    instance.Get() = this;
#if defined(UIKIT_INPUT_METHOD) && UIKIT_INPUT_METHOD
    vg_set_input_context_ops(&input_context_ops);
#endif // defined(UIKIT_INPUT_METHOD) && UIKIT_INPUT_METHOD
}

InputMethodManager::~InputMethodManager()
{
    ASH_CHECK(instance.Get() == this);
    instance.Get() = nullptr;
#if defined(UIKIT_INPUT_METHOD) && UIKIT_INPUT_METHOD
    vg_set_input_context_ops(nullptr);
#endif // defined(UIKIT_INPUT_METHOD) && UIKIT_INPUT_METHOD
    if (current_input_method_) {
        current_input_method_->OnFocusLost();
    }

    if (current_input_method_controller_) {
        current_input_method_controller_->Unbind();
    }
}

InputMethod* InputMethodManager::CreateInputMethod(void* user_data)
{
    return new InputMethod(user_data);
}

void InputMethodManager::DestroyInputMethod(InputMethod* input_method)
{
    if (current_input_method_ == input_method) {
        current_input_method_ = nullptr;
        StopInputMethodApplicationIfNeeded();
    }

    delete input_method;
}

void InputMethodManager::Require(InputMethod* input_method, const std::string& initial_text)
{
    ASH_LOG("IMM", INFO) << "InputMethodManager::Require";
    if (current_input_method_ && current_input_method_ != input_method) {
        current_input_method_->OnFocusLost();
    }

    current_input_method_ = input_method;
    cached_initial_text_ = initial_text;

    StartInputMethodApplicationIfNeeded();
    UpdateInitialText();
}

void InputMethodManager::BindController(InputMethodController* controller)
{
    if (current_input_method_controller_ == controller)
        return;

    if (current_input_method_controller_) {
        current_input_method_controller_->Unbind();
    }

    current_input_method_controller_ = controller;

    if (current_input_method_controller_) {
        current_input_method_controller_->Bind();
    }

    UpdateInitialText();
}

void InputMethodManager::UnbindController(InputMethodController* controller)
{
    if (current_input_method_controller_ != controller) {
        return;
    }

    current_input_method_controller_->Unbind();
    current_input_method_controller_ = nullptr;

    if (current_input_method_) {
        current_input_method_->OnFocusLost();
    }

    input_method_application_started_ = false;
}

void InputMethodManager::UpdateInputText(const std::string& text)
{
    if (current_input_method_) {
        cached_initial_text_ = text;
        current_input_method_->OnTextChanged(text);
    }
}

InputMethodManager* InputMethodManager::GetInstance()
{
    return instance.Get();
}

void InputMethodManager::StartInputMethodApplicationIfNeeded()
{
    if (input_method_application_started_)
        return;
    input_method_application_started_ = true;
    quickapp_launch_app("com.vela.system.inputmethod", nullptr);
}

void InputMethodManager::StopInputMethodApplicationIfNeeded()
{
    if (!input_method_application_started_)
        return;
    input_method_application_started_ = false;
    quickapp_stop_app("com.vela.system.inputmethod");
}

void InputMethodManager::UpdateInitialText()
{
    if (current_input_method_controller_) {
        current_input_method_controller_->UpdateInitialText(cached_initial_text_);
    }
}

} // namespace ferry
