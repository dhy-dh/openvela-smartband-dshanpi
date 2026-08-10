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

/**
 * @file global_variable.h
 *
 * Provides classes to support task scoped variables both on nuttx and non
 * nuttx.
 *
 * GlobalVariable class is used to define a task scoped variable.
 * Example:
 *   GlobalVariable<int> var = 10;
 *   ...
 *   var.Get() = 100;  // Set
 *   int v = var.Get(); // Get
 *
 * On nuttx, GlobalVariable is task scoped, every task will have its own
 * instance of GlobalVariable. On non nuttx, GlobalVariable is process scoped,
 * like a normal global variable.
 *
 * GlobalVariable can only be used as a global variable, a static local
 * variable or a static member variable. Don't use it as any other form like
 * a local variable and so on, or it will cause undefined behavior.
 *
 * Example:
 *   GloablVariable<int> a = 20; // OK, global variable.
 *   void Foo() {
 *     GlobalVariable<bool> var = false;  // Error, don't use it as a local
 *                                        // variable.
 *     static GlobalVariable<int> var = 10;  // OK, static local variable.
 *       ...
 *   }
 *
 *   class Bar {
 *    private:
 *     GlobalVariable<float> var_ = 10.0f;  // Error, don't use it as a static
 *                                          // member variable.
 *     static GlobalVariable<AClass> static_var_; // OK, static member
 *                                                // variable.
 *   };
 *
 * GlobalVariable can only be initialized with const arguments. Don't use the
 * result of a function call as the initializer of a GlobalVariable, because
 * on nuttx, the initializer of a GlobalVariable is evaluated only once,
 * the initial value of GlobalVariable in different tasks will be the same,
 * same, which may cause unexpected behavior.
 *
 * Example:
 *   GlobalVariable<int> a = 10;  // OK, const argument.
 *   GlobalVariable<Foo> b(20, "Hello");  // OK, multiple const arguments.
 *   GlobalVariable<int> b = Foo();  // Error, don't use the result of a
 *                                   // function.
 *
 * If you want to use the result of a function call as the initializer of
 * GlobalVariable, you can use GlobalVariableWithInitializer.
 *
 * Example:
 *   GloablVariableWithInitializer<int> a = [] { return Foo(); }
 *
 * Or more concise:
 *   GlobalVariableWithInitializer<int> a = Foo;
 *
 * On nuttx, GlobalVariableWithInitializer can make sure that the initializer
 * called once in every task.
 *
 * Like normal global variable, the initialzation order of GlobalVariables
 * (GlobalVariableWithInitializers) is not guaranteed. Accessing a
 * GlobalVariable in the initializer of another GlobalVariable is dangerous.
 */
#ifndef ASH_MEMORY_GLOBAL_VARIABLE_H_
#define ASH_MEMORY_GLOBAL_VARIABLE_H_

#include "ash/macros/compiler_macros.h"
#include "ash/macros/disallow_copy.h"
#include "ash/memory/variable_segment.h"

namespace ash {

#if defined(ASH_OS_NUTTX) && !defined(CONFIG_BUILD_KERNEL)

VariableSegmentDefination* GetGlobalVariableSegmentDefination();
uint8_t* GetGlobalVariableSegment();

template <typename T>
class GlobalVariable {
 public:
  template <typename... Args>
  GlobalVariable(Args&&... args)
      : offset_(
            GetGlobalVariableSegmentDefination()->RegisterVariableWithArgs<T>(
                std::forward<Args>(args)...)) {}
  ~GlobalVariable() = default;

  T& Get() {
    return *reinterpret_cast<T*>(GetGlobalVariableSegment() + offset_);
  }

 private:
  int offset_;
  ASH_DISALLOW_COPY_AND_MOVE(GlobalVariable);
};

template <typename T>
class GlobalVariableWithInitializer {
 public:
  template <typename F>
  GlobalVariableWithInitializer(F initializer)
      : offset_(
            GetGlobalVariableSegmentDefination()
                ->RegisterVariableWithInitializer<T>(std::move(initializer))) {}
  ~GlobalVariableWithInitializer() = default;
  T& Get() {
    return *reinterpret_cast<T*>(GetGlobalVariableSegment() + offset_);
  }

 private:
  int offset_;
  ASH_DISALLOW_COPY_AND_MOVE(GlobalVariableWithInitializer);
};

#else

template <typename T>
class GlobalVariable {
 public:
  template <typename... Args>
  GlobalVariable(Args&&... args) : value_(std::forward<Args>(args)...) {}
  ~GlobalVariable() = default;

  T& Get() { return value_; }

 private:
  T value_;
  ASH_DISALLOW_COPY_AND_MOVE(GlobalVariable);
};

template <typename T>
class GlobalVariableWithInitializer {
 public:
  template <typename F>
  GlobalVariableWithInitializer(F initializer) : value_(initializer()) {}
  ~GlobalVariableWithInitializer() = default;

  T& Get() { return value_; }

 private:
  T value_;
  ASH_DISALLOW_COPY_AND_MOVE(GlobalVariableWithInitializer);
};

#endif  // defiend(ASH_OS_NUTTX)

}  // namespace ash

#endif  // ASH_MEMORY_GLOBAL_VARIABLE_H_
