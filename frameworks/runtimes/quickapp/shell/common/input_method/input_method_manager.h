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

#ifndef QUICKAPP_INPUT_METHOD_INPUT_METHOD_MANAGER_H_
#define QUICKAPP_INPUT_METHOD_INPUT_METHOD_MANAGER_H_

#include <functional>
#include <memory>

namespace shell {

class InputMethod {
public:
    InputMethod(void* user_data);
    ~InputMethod();

    void* GetUserData();

    using TextChangedCB = std::function<void(const std::string&)>;
    void SetTextChanged(TextChangedCB text_changed);

    using FocusLostCB = std::function<void()>;
    void SetFocusLost(FocusLostCB focus_lost);

    void OnTextChanged(const std::string& text);
    void OnFocusLost();

private:
    void* user_data_;
    TextChangedCB text_changed_;
    FocusLostCB focus_lost_;
};

class InputMethodController {
public:
    InputMethodController() = default;
    ~InputMethodController() = default;

    virtual void UpdateInitialText(const std::string& initial_text) = 0;
    virtual void Bind() = 0;
    virtual void Unbind() = 0;
};

class InputMethodManager {
public:
    InputMethodManager();
    ~InputMethodManager();

    InputMethod* CreateInputMethod(void* user_data);
    void DestroyInputMethod(InputMethod* InputMethod);

    void Require(InputMethod* input_method, const std::string& initial_text);
    void BindController(InputMethodController* controller);
    void UnbindController(InputMethodController* controller);
    void UpdateInputText(const std::string& text);

    static InputMethodManager* GetInstance();

private:
    void StartInputMethodApplicationIfNeeded();
    void StopInputMethodApplicationIfNeeded();
    void UpdateInitialText();

    InputMethod* current_input_method_;
    InputMethodController* current_input_method_controller_;
    std::string cached_initial_text_;
    bool input_method_application_started_;
};

} // namespace ferry

#endif // QUICKAPP_INPUT_METHOD_INPUT_METHOD_MANAGER_H_
