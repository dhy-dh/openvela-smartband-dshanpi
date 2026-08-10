/* //device/libs/telephony/ril_unsol_commands.h
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
    { RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, responseVoid },
    { RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, responseCallList },
    { RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED, responseVoid },
    { RIL_UNSOL_RESPONSE_NEW_SMS, responseString },
    { RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT, responseString },
    { RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM, responseInts },
    { RIL_UNSOL_ON_USSD, responseStrings },
    { RIL_UNSOL_ON_USSD_REQUEST, responseVoid },
    { RIL_UNSOL_NITZ_TIME_RECEIVED, responseString },
    { RIL_UNSOL_SIGNAL_STRENGTH, responseRilSignalStrength },
    { RIL_UNSOL_DATA_CALL_LIST_CHANGED, responseDataCallList },
    { RIL_UNSOL_SUPP_SVC_NOTIFICATION, responseSsn },
    { RIL_UNSOL_STK_SESSION_END, responseVoid },
    { RIL_UNSOL_STK_PROACTIVE_COMMAND, responseString },
    { RIL_UNSOL_STK_EVENT_NOTIFY, responseString },
    { RIL_UNSOL_STK_CALL_SETUP, responseInts },
    { RIL_UNSOL_SIM_SMS_STORAGE_FULL, responseVoid },
    { RIL_UNSOL_SIM_REFRESH, responseSimRefresh },
    { 1018, responseVoid },
    { RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, responseVoid },
    { 1020, responseVoid },
    { RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS, responseRaw },
    { 1022, responseVoid },
    { RIL_UNSOL_RESTRICTED_STATE_CHANGED, responseInts },
    { RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE, responseVoid },
    { 1025, responseVoid },
    { 1026, responseVoid },
    { 1027, responseVoid },
    { RIL_UNSOL_OEM_HOOK_RAW, responseRaw },
    { RIL_UNSOL_RINGBACK_TONE, responseInts },
    { RIL_UNSOL_RESEND_INCALL_MUTE, responseVoid },
    { 1031, responseVoid },
    { 1032, responseVoid },
    { RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE, responseVoid },
    { RIL_UNSOL_RIL_CONNECTED, responseInts },
    { RIL_UNSOL_VOICE_RADIO_TECH_CHANGED, responseInts },
    { RIL_UNSOL_CELL_INFO_LIST, responseCellInfoList },
    // 1037
    { RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED, responseVoid },
    { RIL_UNSOL_SIM_INVALID, responseVoid },
    { 1039, responseVoid },
    { 1040, responseVoid },
    { 1041, responseVoid },
    { 1042, responseVoid },
    { 1043, responseVoid },
    { 1044, responseVoid },
    { 1045, responseVoid },
    { 1046, responseVoid },
    // 1047 responseVoid
    { RIL_UNSOL_MODEM_RESTART, responseVoid },
    { 1048, responseVoid },
    { 1049, responseVoid },
    { 1050, responseVoid },
    { 1051, responseVoid },
    { RIL_UNSOL_EMERGENCY_NUMBER_LIST, responseEccList },
    { 1053, responseVoid },
    { RIL_UNSOL_ABNORMAL_EVENT, responseAbnormalInfo },
    { RIL_UNSOL_MODEM_UPGRADE_STATE_CHANGED, responseInts },
