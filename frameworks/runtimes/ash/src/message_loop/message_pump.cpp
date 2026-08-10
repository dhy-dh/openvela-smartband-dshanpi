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
#include "ash/message_loop/message_pump.h"
#include "ash/message_loop/message_queue.h"

namespace ash {

MessagePump::MessagePump() = default;

MessagePump::~MessagePump() = default;

Duration MessagePump::Drive() {
  return queue_->Drive();
}

void MessagePump::AddListener(MessageLoopListener* listener) {
  listeners_.push_back(listener);
}

void MessagePump::RemoveListener(MessageLoopListener* listener) {
  auto it = std::find(listeners_.begin(), listeners_.end(), listener);
  if (it != listeners_.end()) {
    listeners_.erase(it);
  }
}

void MessagePump::OnPreTask() {
  std::vector<MessageLoopListener*> listeners = listeners_;
  for (MessageLoopListener* listener : listeners) {
    listener->OnPreTask();
  }
}

void MessagePump::OnPostTask() {
  std::vector<MessageLoopListener*> listeners = listeners_;
  for (MessageLoopListener* listener : listeners) {
    listener->OnPostTask();
  }
}

std::size_t MessagePump::GetTaskSize() {
  return queue_->GetTaskSize();
}

}  // namespace ash
