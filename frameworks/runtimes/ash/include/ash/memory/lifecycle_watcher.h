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
#ifndef ASH_MEMORY_LIFECYCLE_WATCHER_H_
#define ASH_MEMORY_LIFECYCLE_WATCHER_H_

#include <memory>
#include "ash/macros/disallow_copy.h"

namespace ash {

class LifecycleWatcher {
 public:
  LifecycleWatcher();
  virtual ~LifecycleWatcher();

  virtual bool IsAlive() = 0;
  ASH_DISALLOW_COPY_AND_MOVE(LifecycleWatcher);
};

using LifecycleWatcherPtr = std::shared_ptr<LifecycleWatcher>;

}  // namespace ash

#endif  // ASH_MEMORY_LIFECYCLE_WATCHER_H_
