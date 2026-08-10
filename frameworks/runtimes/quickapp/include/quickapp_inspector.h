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
#ifndef _QUICKAPP_INSPECTOR_H_
#define _QUICKAPP_INSPECTOR_H_

#include "quickapp.h"

#ifdef __cplusplus
extern "C" {
#endif

void InspectHostNetRequest(void* req, const char* method, int64_t request_id);
void InspectHostNetResponse(void* req, void* response, int64_t request_id, const char* header);
void InspectHostNetLoadingFailed(bool ret);
int64_t InspectHostNetGetReqId();
int64_t InspectHostNetGetCurrentReqId();

void InspectHostStorageInit(void* db);
void InspectHostStorageUpdate();

void InspectStartServer();
void InspectStopServer();

#ifdef __cplusplus
}
#endif

#endif
