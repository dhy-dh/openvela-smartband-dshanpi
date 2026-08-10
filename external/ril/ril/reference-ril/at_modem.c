/*
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

#define LOG_TAG "AT_MODEM"
#define NDEBUG 1

#include <assert.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <unistd.h>

#include <telephony/librilutils.h>
#include <telephony/ril_log.h>

#include "at_modem.h"
#include "at_ril.h"
#include "at_sim.h"
#include "at_tok.h"
#include "atchannel.h"
#include "misc.h"

#define GSM (RAF_GSM | RAF_GPRS | RAF_EDGE)
#define CDMA (RAF_IS95A | RAF_IS95B | RAF_1xRTT)
#define EVDO (RAF_EVDO_0 | RAF_EVDO_A | RAF_EVDO_B | RAF_EHRPD)
#define WCDMA (RAF_HSUPA | RAF_HSDPA | RAF_HSPA | RAF_HSPAP | RAF_UMTS)
#define LTE (RAF_LTE | RAF_LTE_CA)
#define NR (RAF_NR)

static ModemInfo* sMdmInfo;
static int s_modem_enabled = 0;

static const char HEX_CHARS[] = "0123456789ABCDEF";

extern uint8_t* convertHexStringToBytes(void* response, size_t responseLen);

char* convertBytesToHexString(const uint8_t* bytes, size_t len)
{
    if (bytes == NULL || len == 0) {
        return NULL;
    }

    char* hexString = (char*)malloc(2 * len + 1);
    if (hexString == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < len; ++i) {
        uint8_t byte = bytes[i];
        hexString[2 * i] = HEX_CHARS[(byte >> 4) & 0x0F];
        hexString[2 * i + 1] = HEX_CHARS[byte & 0x0F];
    }

    hexString[2 * len] = '\0';

    return hexString;
}

static void requestRadioPower(void* data, size_t datalen, RIL_Token t)
{
    int onOff;
    int err;
    ATResponse* p_response = NULL;

    if (data == NULL) {
        RLOGE("requestRadioPower data is null");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    assert(datalen >= sizeof(int*));
    onOff = ((int*)data)[0];

    if (onOff == 0 && getRadioState() != RADIO_STATE_OFF) {
        err = at_send_command("AT+CFUN=0", &p_response);
        if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
            RLOGE("Failure occurred in sending %s due to: %s", "AT+CFUN=0", at_io_err_str(err));
            goto error;
        }
        setRadioState(RADIO_STATE_OFF);
    } else if (onOff > 0 && getRadioState() == RADIO_STATE_OFF) {
        err = at_send_command("AT+CFUN=1", &p_response);
        if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
            RLOGE("Failure occurred in sending %s due to: %s", "AT+CFUN=1", at_io_err_str(err));
            // Some stacks return an error when there is no SIM,
            // but they really turn the RF portion on
            // So, if we get an error, let's check to see if it
            // turned on anyway

            if (isRadioOn() != 1) {
                goto error;
            }
        }

        setRadioState(RADIO_STATE_ON);
    }

    at_response_free(p_response);
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    return;

error:
    at_response_free(p_response);
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

static void requestBaseBandVersion(void* data, size_t datalen, RIL_Token t)
{
    (void)data;
    (void)datalen;

    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int err = -1;
    char* line = NULL;
    char* responseStr = NULL;

    err = at_send_command_singleline("AT+CGMR", "+CGMR:", &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("Failure occurred in sending %s due to: %s", "AT+CGMR", at_io_err_str(err));
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    line = p_response->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0) {
        RLOGE("Fail to parse line in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    err = at_tok_nextstr(&line, &responseStr);
    if (err < 0) {
        RLOGE("Fail to parse base band version in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, ril_err == RIL_E_SUCCESS ? responseStr : NULL,
        ril_err == RIL_E_SUCCESS ? sizeof(responseStr) : 0);
    at_response_free(p_response);
}

static void requestDeviceIdentity(void* data, size_t datalen, RIL_Token t)
{
    (void)data;
    (void)datalen;

    int err;
    char* responseStr[4];
    ATResponse* p_response = NULL;
    int count = 4;

    // Fixed values. TODO: Query modem
    responseStr[0] = "358240051111110";
    responseStr[1] = "";
    responseStr[2] = "77777777";
    responseStr[3] = ""; // default empty for non-CDMA

    err = at_send_command_numeric("AT+CGSN", &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("Failure occurred in sending %s due to: %s", "AT+CGSN", at_io_err_str(err));
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    } else {
        if (TECH_BIT(sMdmInfo) == MDM_CDMA) {
            responseStr[3] = p_response->p_intermediates->line;
        } else {
            responseStr[0] = p_response->p_intermediates->line;
        }
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, responseStr, count * sizeof(char*));
    at_response_free(p_response);
}

static void requestGetHardwareConfig(void* data, size_t datalen, RIL_Token t)
{
    (void)data;
    (void)datalen;

    ATResponse* p_response = NULL;
    RIL_HardwareConfig* hwCfg = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    ATLine* cur = NULL;
    char* line = NULL;
    int d = 0;
    int config_num = 0;
    int err = -1;
    int i = 0;

    err = at_send_command_multiline("AT^CHWCFG", "^CHWCFG:", &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("Failure occurred in sending %s, ret: %s, p_response: %p, final response: %s",
            "AT^CHWCFG", at_io_err_str(err), p_response,
            p_response ? p_response->finalResponse : "null");
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    for (cur = p_response->p_intermediates; cur; cur = cur->p_next) {
        config_num++;
    }

    hwCfg = calloc(config_num, sizeof(RIL_HardwareConfig));
    if (hwCfg == NULL) {
        RLOGE("Fail to allocate memory for hwCfg");
        ril_err = RIL_E_NO_MEMORY;
        goto on_exit;
    }

    for (i = 0, cur = p_response->p_intermediates; cur; cur = cur->p_next, i++) {
        line = cur->line;
        err = at_tok_start(&line);
        if (err < 0) {
            RLOGE("Fail to parse line");
            goto on_exit;
        }

        err = at_tok_nextint(&line, &d);
        if (err < 0) {
            RLOGE("Failed to parse type (%d) in index (%d)", d, i);
            ril_err = RIL_E_GENERIC_FAILURE;
            goto on_exit;
        }
        hwCfg[i].type = (RIL_HardwareConfig_Type)d;

        char* uuid = NULL;
        err = at_tok_nextstr(&line, &uuid);
        if (err < 0) {
            RLOGE("Failed to parse uuid (%s) in index (%d)", uuid, i);
            ril_err = RIL_E_GENERIC_FAILURE;
            goto on_exit;
        }
        strncpy(hwCfg[i].uuid, uuid, MAX_UUID_LENGTH);

        err = at_tok_nextint(&line, &d);
        if (err < 0) {
            RLOGE("Failed to parse state (%d) in index (%d)", d, i);
            ril_err = RIL_E_GENERIC_FAILURE;
            goto on_exit;
        }
        hwCfg[i].state = (RIL_HardwareConfig_State)d;

        if (hwCfg[i].type == RIL_HARDWARE_CONFIG_MODEM) {
            err = at_tok_nextint(&line, &d);
            if (err < 0) {
                RLOGE("Failed to parse rilModel (%d) in index (%d)", d, i);
                ril_err = RIL_E_GENERIC_FAILURE;
                goto on_exit;
            }
            hwCfg[i].cfg.modem.rilModel = d;

            err = at_tok_nextint(&line, &d);
            if (err < 0) {
                RLOGE("Failed to parse rat (%d) in index (%d)", d, i);
                ril_err = RIL_E_GENERIC_FAILURE;
                goto on_exit;
            }
            hwCfg[i].cfg.modem.rat = (uint32_t)d;

            err = at_tok_nextint(&line, &d);
            if (err < 0) {
                RLOGE("Failed to parse maxVoice (%d) in index (%d)", d, i);
                ril_err = RIL_E_GENERIC_FAILURE;
                goto on_exit;
            }
            hwCfg[i].cfg.modem.maxVoice = d;

            err = at_tok_nextint(&line, &d);
            if (err < 0) {
                RLOGE("Failed to parse maxData (%d) in index (%d)", d, i);
                ril_err = RIL_E_GENERIC_FAILURE;
                goto on_exit;
            }
            hwCfg[i].cfg.modem.maxData = d;

            err = at_tok_nextint(&line, &d);
            if (err < 0) {
                RLOGE("Failed to parse maxStandby (%d) in index (%d)", d, i);
                ril_err = RIL_E_GENERIC_FAILURE;
                goto on_exit;
            }
            hwCfg[i].cfg.modem.maxStandby = d;
        } else if (hwCfg[i].type == RIL_HARDWARE_CONFIG_SIM) {
            char* simUuid = NULL;
            err = at_tok_nextstr(&line, &simUuid);
            if (err < 0) {
                RLOGE("Failed to parse modemUuid (%s) in index (%d)", simUuid, i);
                ril_err = RIL_E_GENERIC_FAILURE;
                goto on_exit;
            }
            strncpy(hwCfg[i].cfg.sim.modemUuid, simUuid, MAX_UUID_LENGTH);
        } else {
            RLOGE("Invalid config type.");
            ril_err = RIL_E_GENERIC_FAILURE;
            goto on_exit;
        }
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, ril_err == RIL_E_SUCCESS ? hwCfg : NULL,
        ril_err == RIL_E_SUCCESS ? config_num * sizeof(RIL_HardwareConfig) : 0);
    at_response_free(p_response);
    free(hwCfg);
}

static void unsolicitedRingBackTone(const char* s)
{
    char *line, *p;
    int cid, action, type;

    line = p = strdup(s);
    if (!line) {
        RLOGE("unsolicitedRingBackTone line is null");
        return;
    }

    if (at_tok_start(&p) < 0) {
        RLOGE("Fail to parse line in %s", __func__);
        free(line);
        return;
    }

    if (at_tok_nextint(&p, &cid) < 0) {
        RLOGE("Fail to parse cid in %s", __func__);
        free(line);
        return;
    }

    if (at_tok_nextint(&p, &action) < 0) {
        RLOGE("Fail to parse action in %s", __func__);
        free(line);
        return;
    }

    if (at_tok_nextint(&p, &type) < 0) {
        RLOGE("Fail to parse tyoe in %s", __func__);
        free(line);
        return;
    }

    RLOGD("On Ringback tone URC, cid: %d, action: %s, type: %s", cid,
        action == 1 ? "START" : "STOP", type == 1 ? "RINGBACK" : "CALL HOLDING");
    RIL_onUnsolicitedResponse(RIL_UNSOL_RINGBACK_TONE, &action, sizeof(int));
    free(line);
}

static void requestScreenState(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;

    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int err = -1;
    int status;

    if (data == NULL) {
        RLOGE("requestScreenState data is null");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    status = *((int*)data);

    if (!status) {
        /* Suspend */
        err = at_send_command("AT+CEREG=1", &p_response);
        if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
            RLOGE("Failure occurred in sending %s due to: %s", "AT+CEREG=1", at_io_err_str(err));
            ril_err = RIL_E_GENERIC_FAILURE;
            goto on_exit;
        }

        at_response_free(p_response);
        p_response = NULL;

        err = at_send_command("AT+CREG=1", &p_response);
        if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
            RLOGE("Failure occurred in sending %s due to: %s", "AT+CREG=1", at_io_err_str(err));
            ril_err = RIL_E_GENERIC_FAILURE;
            goto on_exit;
        }

        at_response_free(p_response);
        p_response = NULL;

        err = at_send_command("AT+CGREG=1", &p_response);
        if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
            RLOGE("Failure occurred in sending %s due to: %s", "AT+CGREG=1", at_io_err_str(err));
            ril_err = RIL_E_GENERIC_FAILURE;
            goto on_exit;
        }
    } else {
        /* Resume */
        err = at_send_command("AT+CEREG=2", &p_response);
        if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
            RLOGE("Failure occurred in sending %s due to: %s", "AT+CEREG=2", at_io_err_str(err));
            ril_err = RIL_E_GENERIC_FAILURE;
            goto on_exit;
        }

        at_response_free(p_response);
        p_response = NULL;

        err = at_send_command("AT+CREG=2", &p_response);
        if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
            RLOGE("Failure occurred in sending %s due to: %s", "AT+CREG=2", at_io_err_str(err));
            ril_err = RIL_E_GENERIC_FAILURE;
            goto on_exit;
        }

        at_response_free(p_response);
        p_response = NULL;

        err = at_send_command("AT+CGREG=2", &p_response);
        if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
            RLOGE("Failure occurred in sending %s due to: %s", "AT+CGREG=2", at_io_err_str(err));
            ril_err = RIL_E_GENERIC_FAILURE;
            goto on_exit;
        }
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, NULL, 0);
    at_response_free(p_response);
}

static void requestGetModemStatus(void* data, size_t datalen, RIL_Token t)
{
    (void)data;
    (void)datalen;

    int modemState = s_modem_enabled;
    RLOGI("response RIL_REQUEST_GET_MODEM_STATUS, status is [%d]", modemState);
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &modemState, sizeof(modemState));
    return;
}

static void requestSuppressMessageReport(void* data, size_t datalen, RIL_Token t)
{
    int msg_list;
    int msg_list_value;
    char cmd[50] = { 0 };
    ATResponse* p_response = NULL;
    int err = -1;
    RIL_Errno ril_err = RIL_E_SUCCESS;

    if (data == NULL) {
        RLOGE("requestSuppressMessageReport data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    if (datalen != sizeof(int) * 4) {
        RLOGE("requestSuppressMessageReport data len is wrong!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    msg_list = ((int*)data)[0];
    msg_list_value = ((int*)data)[1];

    snprintf(cmd, sizeof(cmd), "AT+SUPPRESSMSG=%d,%d", msg_list, msg_list_value);
    err = at_send_command(cmd, &p_response);

    if (err < 0 || !p_response || p_response->success != AT_OK) {
        RLOGE("Failure occurred in sending %s due to: %s", cmd, at_io_err_str(err));
        ril_err = RIL_E_GENERIC_FAILURE;
    }

    RIL_onRequestComplete(t, ril_err, NULL, 0);
    at_response_free(p_response);
    p_response = NULL;
}

static void requestSetSignalThreshold(void* data, size_t datalen, RIL_Token t)
{
    int type;
    int signal_threshold[4];
    char cmd[50] = { 0 };
    ATResponse* p_response = NULL;
    int err = -1;
    RIL_Errno ril_err = RIL_E_SUCCESS;

    if (data == NULL) {
        RLOGE("requestSetSignalThreshold data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    if (datalen != sizeof(int) * 5) {
        RLOGE("requestSetSignalThreshold data len is wrong!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    type = ((int*)data)[0];
    signal_threshold[0] = ((int*)data)[1];
    signal_threshold[1] = ((int*)data)[2];
    signal_threshold[2] = ((int*)data)[3];
    signal_threshold[3] = ((int*)data)[4];

    snprintf(cmd, sizeof(cmd), "AT+SIGNALTHRESHOLD=%d,%d,%d,%d,%d", type, signal_threshold[0],
        signal_threshold[1], signal_threshold[2], signal_threshold[3]);
    err = at_send_command(cmd, &p_response);

    if (err < 0 || !p_response || p_response->success != AT_OK) {
        RLOGE("Failure occurred in sending %s due to: %s", cmd, at_io_err_str(err));
        ril_err = RIL_E_GENERIC_FAILURE;
    }

    RIL_onRequestComplete(t, ril_err, NULL, 0);
    at_response_free(p_response);
    p_response = NULL;
}

static void requestSetModemStationary(void* data, size_t datalen, RIL_Token t)
{
    int enable;
    char cmd[50] = { 0 };
    ATResponse* p_response = NULL;
    int err = -1;
    RIL_Errno ril_err = RIL_E_SUCCESS;

    if (data == NULL) {
        RLOGE("requestSetModemStationary data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    if (datalen != sizeof(int)) {
        RLOGE("requestSetModemStationary data len is wrong!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    enable = ((int*)data)[0];

    snprintf(cmd, sizeof(cmd), "AT+STATIONARY=%d", enable);
    err = at_send_command(cmd, &p_response);

    if (err < 0 || !p_response || p_response->success != AT_OK) {
        RLOGE("Failure occurred in sending %s due to: %s", cmd, at_io_err_str(err));
        ril_err = RIL_E_GENERIC_FAILURE;
    }

    RIL_onRequestComplete(t, ril_err, NULL, 0);
    at_response_free(p_response);
    p_response = NULL;
}

static void requestSetModemStationaryThreshold(void* data, size_t datalen, RIL_Token t)
{
    int value;
    char cmd[50] = { 0 };
    ATResponse* p_response = NULL;
    int err = -1;
    RIL_Errno ril_err = RIL_E_SUCCESS;

    if (data == NULL) {
        RLOGE("requestSetModemStationaryThreshold data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    if (datalen != sizeof(int)) {
        RLOGE("requestSetModemStationaryThreshold data len is wrong!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    value = ((int*)data)[0];

    snprintf(cmd, sizeof(cmd), "AT+AUTOSTATIONARYTHRESHOLD=%d", value);
    err = at_send_command(cmd, &p_response);

    if (err < 0 || !p_response || p_response->success != AT_OK) {
        RLOGE("Failure occurred in sending %s due to: %s", cmd, at_io_err_str(err));
        ril_err = RIL_E_GENERIC_FAILURE;
    }

    RIL_onRequestComplete(t, ril_err, NULL, 0);
    at_response_free(p_response);
    p_response = NULL;
}

static void requestEnableAbnormalEvents(void* data, size_t datalen, RIL_Token t)
{
    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    char* cmd = NULL;
    int enable, module_mask, from_event_id, to_event_id;
    int err = -1;

    if (data == NULL) {
        RLOGE("%s: data is null!", __func__);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    if (datalen != sizeof(int) * 4) {
        RLOGE("%s: data len is wrong!", __func__);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    enable = ((int*)data)[0];
    module_mask = ((int*)data)[1];
    from_event_id = ((int*)data)[2];
    to_event_id = ((int*)data)[3];

    syslog(LOG_DEBUG, "%s: enable: %d, module_mask: %d, from_event_id: %d, to_event_id: %d",
        __func__, enable, module_mask, from_event_id, to_event_id);

    if (asprintf(&cmd, "AT^MDBGINFOEN=%d,%d,%d,%d",
            enable, module_mask, from_event_id, to_event_id)
        < 0) {
        RLOGE("%s: Failed to allocate memory.", __func__);
        RIL_onRequestComplete(t, RIL_E_NO_MEMORY, NULL, 0);
        goto on_exit;
    }

    err = at_send_command(cmd, &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("%s: Failure occurred in sending %s, ret: %s, p_response: %p, final response: %s",
            __func__, cmd, at_io_err_str(err), p_response,
            p_response ? p_response->finalResponse : "null");
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, NULL, 0);
    at_response_free(p_response);
    free(cmd);
}

static void requestCheckModemUpgradeStatus(void* data, size_t datalen, RIL_Token t)
{
    char cmd[50] = { 0 };
    ATResponse* p_response = NULL;
    int err = -1;
    char* line = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int upgrade_state = 0;
    int operation_id = 1; // default 1=success,0 = fail

    snprintf(cmd, sizeof(cmd), "AT+MDUPGRADECHECK=%d", operation_id);
    err = at_send_command_singleline(cmd, "+MDUPGRADECHECK: ", &p_response);

    if (err < 0 || !p_response || p_response->success != AT_OK) {
        RLOGE("Failure occurred in sending %s due to: %s", cmd, at_io_err_str(err));
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    line = p_response->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0) {
        RLOGE("Fail to parse line in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    err = at_tok_nextint(&line, &upgrade_state);
    if (err < 0) {
        RLOGE("Fail to parse upgrade_state in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, ril_err == RIL_E_SUCCESS ? &upgrade_state : NULL,
        ril_err == RIL_E_SUCCESS ? sizeof(upgrade_state) : 0);
    at_response_free(p_response);
    p_response = NULL;
}

static void requestSendModemUpgradeCmd(void* data, size_t datalen, RIL_Token t)
{
    int cmd_id;
    int operation_id = 1; // default 1=success,0 = fail
    char cmd[50] = { 0 };
    ATResponse* p_response = NULL;
    int err = -1;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int error_code = -1;

    if (data == NULL) {
        RLOGE("requestSendModemUpgradeCmd data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, &err, 0);
        return;
    }

    if (datalen != sizeof(int)) {
        RLOGE("requestSendModemUpgradeCmd data len is wrong!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, &err, 0);
        return;
    }

    cmd_id = ((int*)data)[0];

    snprintf(cmd, sizeof(cmd), "AT+MDUPGRADECMD=%d,%d", operation_id, cmd_id);
    err = at_send_command_singleline(cmd, "+MDUPGRADECMD: ", &p_response);

    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        char* line = NULL;
        RLOGE("Failure occurred in sending %s due to: %s", cmd, at_io_err_str(err));
        ril_err = RIL_E_GENERIC_FAILURE;
        if (p_response) {
            line = p_response->p_intermediates->line;

            err = at_tok_start(&line);
            if (err < 0) {
                RLOGE("Fail to parse line in %s", __func__);
                goto on_exit;
            }

            err = at_tok_nextint(&line, &error_code);
            if (err < 0) {
                RLOGE("Fail to parse error_code in %s", __func__);
                goto on_exit;
            }
        }
    }
on_exit:
    RIL_onRequestComplete(t, ril_err, ril_err == RIL_E_GENERIC_FAILURE ? &error_code : NULL,
        ril_err == RIL_E_GENERIC_FAILURE ? sizeof(error_code) : 0);
    at_response_free(p_response);
    p_response = NULL;
}

static void requestGetActivityInfo(void* data, size_t datalen, RIL_Token t)
{
    (void)data;
    (void)datalen;

    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    RIL_ActivityStatsInfo stats = { 0 };
    int temp = 0;
    char* line = NULL;
    int err = -1;

    // respinse: +GETACTIVITYINFO: <sleep_mode_time>,<idle_mode_time>,<tx_mode_time_1>,
    //                             <tx_mode_time_2>,<tx_mode_time_3>,
    //                             <tx_mode_time_4>,<tx_mode_time_5>,<rx_mode_time>
    err = at_send_command_singleline("AT+GETACTIVITYINFO", "+GETACTIVITYINFO:", &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("Failure occurred in sending %s due to: %s", "AT+GETACTIVITYINFO", at_io_err_str(err));
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    line = p_response->p_intermediates->line;
    if (!line) {
        RLOGE("Received empty line in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    err = at_tok_start(&line);
    if (err < 0) {
        RLOGE("Failed to parse line in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    err = at_tok_nextint(&line, &temp);
    if (err < 0) {
        RLOGE("Failed to parse sleep_mode_time_ms in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }
    stats.sleep_mode_time_ms = (uint32_t)temp;

    err = at_tok_nextint(&line, &temp);
    if (err < 0) {
        RLOGE("Failed to parse idle_mode_time in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }
    stats.idle_mode_time_ms = (uint32_t)temp;

    for (int i = 0; i < RIL_NUM_TX_POWER_LEVELS; i++) {
        err = at_tok_nextint(&line, &temp);
        if (err < 0) {
            RLOGE("Failed to parse tx_mode_time_%d in %s", i, __func__);
            ril_err = RIL_E_GENERIC_FAILURE;
            goto on_exit;
        }

        stats.tx_mode_time_ms[i] = (uint32_t)temp;
    }

    err = at_tok_nextint(&line, &temp);
    if (err < 0) {
        RLOGE("Failed to parse rx_mode_time in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }
    stats.rx_mode_time_ms = (uint32_t)temp;

on_exit:
    RIL_onRequestComplete(t, ril_err, ril_err == RIL_E_SUCCESS ? &stats : NULL,
        ril_err == RIL_E_SUCCESS ? sizeof(stats) : 0);
    at_response_free(p_response);
}

static void requestGetIMEI(void* data, size_t datalen, RIL_Token t)
{
    (void)data;
    (void)datalen;

    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int err = -1;

    err = at_send_command_numeric("AT+CGSN", &p_response);

    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("Failure occurred in sending %s due to: %s", "AT+CGSN", at_io_err_str(err));
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, ril_err == RIL_E_SUCCESS ? p_response->p_intermediates->line : NULL,
        ril_err == RIL_E_SUCCESS ? sizeof(char*) : 0);
    at_response_free(p_response);
}

static void requestGetIMEISV(void* data, size_t datalen, RIL_Token t)
{
    (void)data;
    (void)datalen;

    ATResponse* p_response = NULL;
    int err = at_send_command_numeric("AT+CGSN=2", &p_response);

    if (err < 0 || p_response->success == 0) {
        RLOGE("Failure occurred in sending %s due to: %s", "AT+CGSN=2", at_io_err_str(err));
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    } else {
        RIL_onRequestComplete(t, RIL_E_SUCCESS,
            p_response->p_intermediates->line, sizeof(char*));
    }

    at_response_free(p_response);
}

static void requestOemHookStrings(void* data, size_t datalen, RIL_Token t)
{
    int i;
    int num_strings;
    const char** cur;
    ATResponse* p_response;
    char** responseStr = NULL;

    RLOGD("got OEM_HOOK_STRINGS: 0x%8p %lu", data, (long)datalen);

    num_strings = datalen / sizeof(char*);
    responseStr = malloc(num_strings * sizeof(char*));
    if (responseStr == NULL) {
        RLOGE("Failed to allocate memory");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    memset(responseStr, 0, num_strings * sizeof(char*));
    for (i = 0, cur = (const char**)data;
         i < num_strings; cur++, i++) {
        p_response = NULL;
        at_send_command(*cur, &p_response);

        if (p_response && p_response->finalResponse) {
            if (asprintf(&responseStr[i], "%s", p_response->finalResponse) < 0) {
                RLOGE("Failed to allocate memory");
            }
        } else {
            if (asprintf(&responseStr[i], "%s", "ERROR") < 0) {
                RLOGE("Failed to allocate memory");
            }
        }

        at_response_free(p_response);
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, responseStr, num_strings * sizeof(responseStr));
    for (i = 0; i < num_strings; i++) {
        if (responseStr[i]) {
            free(responseStr[i]);
            responseStr[i] = NULL;
        }
    }
    free(responseStr);
    responseStr = NULL;
}

static void requestOemHookRaw(void* data, size_t datalen, RIL_Token t)
{
    RIL_Errno ril_err = RIL_E_SUCCESS;
    ATResponse* p_response = NULL;
    int err = AT_ERROR_GENERIC;
    char* cmd = NULL;
    char* req_data = NULL;
    char* line = NULL;
    char* resp_str = NULL;
    uint8_t* resp_data = NULL;

    if (data == NULL) {
        RLOGE("%s: data is null", __func__);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    req_data = convertBytesToHexString(data, datalen);
    if (asprintf(&cmd, "AT+CLED=%s", req_data) < 0) {
        RLOGE("Failed to allocate memory");
        ril_err = RIL_E_NO_MEMORY;
        goto on_exit;
    }

    err = at_send_command_singleline(cmd, "+CLED:", &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("%s: Failure occurred in sending %s, ret: %s, p_response: %p, final response: %s",
            __func__, cmd, at_io_err_str(err), p_response,
            p_response ? p_response->finalResponse : "null");
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    line = p_response->p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) {
        RLOGE("%s: Failed to parse line.", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    err = at_tok_nextstr(&line, &resp_str);
    if (err < 0) {
        RLOGE("%s: Failed to parse resp.", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    resp_data = convertHexStringToBytes(resp_str, strlen(resp_str));
    if (resp_data == NULL) {
        RLOGE("%s: Failed to convert hex string to bytes.", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, ril_err == RIL_E_SUCCESS ? resp_data : NULL,
        ril_err == RIL_E_SUCCESS ? strlen(resp_str) / 2 : 0);
    at_response_free(p_response);
    free(req_data);
    free(resp_data);
    free(cmd);
}

static void requestEnableModem(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;

    int err;
    ATResponse* p_response = NULL;

    if (data == NULL) {
        RLOGE("requestEnableModem data is null");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    sleep(3); // for emulator;
    s_modem_enabled = *(int*)data;
    if (s_modem_enabled == 0) {
        err = at_send_command("AT+CFUN=0", &p_response);
        if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
            RLOGE("Failure occurred in sending %s due to: %s", "AT+CFUN=0", at_io_err_str(err));
            at_response_free(p_response);
            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        } else {
            setRadioState(RADIO_STATE_UNAVAILABLE);
        }
    } else if (s_modem_enabled == 1) {
        setRadioState(RADIO_STATE_OFF);
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
}

ModemInfo* getModemInfo(void)
{
    return sMdmInfo;
}

void initModem(void)
{
    sMdmInfo = calloc(1, sizeof(ModemInfo));

    if (!sMdmInfo) {
        RLOGE("Unable to alloc memory for ModemInfo");
    }
}

// TODO: Use all radio types
int techFromModemType(int mdmtype)
{
    int ret = -1;
    switch (mdmtype) {
    case MDM_CDMA:
        ret = RADIO_TECH_1xRTT;
        break;
    case MDM_EVDO:
        ret = RADIO_TECH_EVDO_A;
        break;
    case MDM_GSM:
        ret = RADIO_TECH_GPRS;
        break;
    case MDM_WCDMA:
        ret = RADIO_TECH_HSPA;
        break;
    case MDM_LTE:
        ret = RADIO_TECH_LTE;
        break;
    case MDM_NR:
        ret = RADIO_TECH_NR;
        break;
    }

    return ret;
}

void setRadioTechnology(ModemInfo* mdm, int newtech)
{
    RLOGD("setRadioTechnology(%d)", newtech);

    int oldtech = TECH(mdm);

    if (newtech != oldtech) {
        RLOGD("Tech change (%d => %d)", oldtech, newtech);
        TECH(mdm) = newtech;
        if (techFromModemType(newtech) != techFromModemType(oldtech)) {
            int tech = techFromModemType(TECH(sMdmInfo));
            if (tech > 0) {
                RIL_onUnsolicitedResponse(RIL_UNSOL_VOICE_RADIO_TECH_CHANGED,
                    &tech, sizeof(tech));
            }
        }
    }
}

/**
 * Parse the response generated by a +CTEC AT command
 * The values read from the response are stored in current and preferred.
 * Both current and preferred may be null. The corresponding value is ignored in that case.
 *
 * @return: -1 if some error occurs (or if the modem doesn't understand the +CTEC command)
 *          1 if the response includes the current technology only
 *          0 if the response includes both current technology and preferred mode
 */
int parse_technology_response(const char* response, int* current, int32_t* preferred)
{
    int err;
    char *line, *p;
    int ct;
    int pt = 0;

    line = p = strdup(response);
    RLOGD("Response: %s", line);
    err = at_tok_start(&p);
    if (err || !at_tok_hasmore(&p)) {
        RLOGE("err: %d. p: %s", err, p);
        free(line);
        return -1;
    }

    err = at_tok_nextint(&p, &ct);
    if (err) {
        RLOGE("Fail to parse ct");
        free(line);
        return -1;
    }

    if (current)
        *current = ct;
    RLOGD("line remaining after int: %s", p);

    err = at_tok_nexthexint(&p, &pt);
    if (err) {
        RLOGE("Fail to parse pt");
        free(line);
        return 1;
    }

    if (preferred) {
        *preferred = pt;
    }

    free(line);

    return 0;
}

/**
 * query_ctec. Send the +CTEC AT command to the modem to query the current
 * and preferred modes. It leaves values in the addresses pointed to by
 * current and preferred. If any of those pointers are NULL, the corresponding value
 * is ignored, but the return value will still reflect if retrieving and parsing of the
 * values succeeded.
 *
 * @mdm Currently unused
 * @current A pointer to store the current mode returned by the modem. May be null.
 * @preferred A pointer to store the preferred mode returned by the modem. May be null.
 * @return -1 on error (or failure to parse)
 *          1 if only the current mode was returned by modem (or failed to parse preferred)
 *          0 if both current and preferred were returned correctly
 */
int query_ctec(ModemInfo* mdm, int* current, int32_t* preferred)
{
    (void)mdm;

    ATResponse* p_response = NULL;
    int err;
    int res;

    RLOGD("query_ctec. current: %p, preferred: %p", current, preferred);
    err = at_send_command_singleline("AT+CTEC?", "+CTEC:", &p_response);
    if (!err && p_response && p_response->success) {
        res = parse_technology_response(p_response->p_intermediates->line, current, preferred);
        at_response_free(p_response);
        return res;
    }

    RLOGE("Error executing command: %d. response: %p. status: %d", err, p_response, p_response ? p_response->success : -1);
    at_response_free(p_response);
    return -1;
}

/* returns 1 if on, 0 if off, and -1 on error */
int isRadioOn(void)
{
    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int err = -1;
    char* line = NULL;
    char ret = 0;

    err = at_send_command_singleline("AT+CFUN?", "+CFUN:", &p_response);

    if (err != AT_ERROR_OK || !p_response || p_response->success == 0) {
        RLOGE("Failure occurred in sending %s due to: %s", "AT+CFUN?", at_io_err_str(err));
        ril_err = RIL_E_GENERIC_FAILURE;
        // assume radio is off
        goto on_exit;
    }

    line = p_response->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0) {
        RLOGE("Fail to parse line in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    err = at_tok_nextbool(&line, &ret);
    if (err < 0) {
        RLOGE("Fail to parse ret in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

on_exit:
    at_response_free(p_response);
    return ril_err == RIL_E_SUCCESS ? ret : -1;
}

int isModemEnable(void)
{
    return s_modem_enabled;
}

void on_request_modem(int request, void* data, size_t datalen, RIL_Token t)
{
    switch (request) {
    case RIL_REQUEST_RADIO_POWER:
        requestRadioPower(data, datalen, t);
        break;
    case RIL_REQUEST_GET_IMEI:
        requestGetIMEI(data, datalen, t);
        break;
    case RIL_REQUEST_GET_IMEISV:
        requestGetIMEISV(data, datalen, t);
        break;
    case RIL_REQUEST_BASEBAND_VERSION:
        requestBaseBandVersion(data, datalen, t);
        break;
    case RIL_REQUEST_OEM_HOOK_RAW:
        requestOemHookRaw(data, datalen, t);
        break;
    case RIL_REQUEST_OEM_HOOK_STRINGS:
        requestOemHookStrings(data, datalen, t);
        break;
    case RIL_REQUEST_SCREEN_STATE:
        requestScreenState(data, datalen, t);
        break;
    case RIL_REQUEST_DEVICE_IDENTITY:
        requestDeviceIdentity(data, datalen, t);
        break;
    case RIL_REQUEST_GET_HARDWARE_CONFIG:
        requestGetHardwareConfig(data, datalen, t);
        break;
    case RIL_REQUEST_GET_ACTIVITY_INFO:
        requestGetActivityInfo(data, datalen, t);
        break;
    case RIL_REQUEST_ENABLE_MODEM:
        requestEnableModem(data, datalen, t);
        break;
    case RIL_REQUEST_GET_MODEM_STATUS:
        requestGetModemStatus(data, datalen, t);
        break;
    case RIL_REQUEST_SUPPRESS_MESSAGE_REPORT:
        requestSuppressMessageReport(data, datalen, t);
        break;
    case RIL_REQUEST_SET_SIGNAL_THRESHOLD:
        requestSetSignalThreshold(data, datalen, t);
        break;
    case RIL_REQUEST_SET_DEVICE_STATIONARY:
        requestSetModemStationary(data, datalen, t);
        break;
    case RIL_REQUEST_SET_DEVICE_STATIONARY_JUDGE_SCOPE:
        requestSetModemStationaryThreshold(data, datalen, t);
        break;
    case RIL_REQUEST_ENABLE_ABNORMAL_EVENT:
        requestEnableAbnormalEvents(data, datalen, t);
        break;
    case RIL_REQUEST_MODEM_UPGRADE_CHECK:
        requestCheckModemUpgradeStatus(data, datalen, t);
        break;
    case RIL_REQUEST_MODEM_UPGRADE_CMD:
        requestSendModemUpgradeCmd(data, datalen, t);
        break;
    default:
        RLOGE("Request not supported");
        RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
        break;
    }

    RLOGD("On request modem end");
}

static void on_modem_debug_info_unsol_resp(const char* s)
{
    assert(s);

    char* line = (char*)s;

    int err;

    /* according to Marconi AT, the format is:
     * ^MDBGINFO: <sn>, <eid>, <page>,<pages>,<buf><CR><LF>
     */
    int sn;
    int eventid;
    int page;
    int page_cont;
    int len;
    char* buf;
    static RIL_ModemInfo modem_info;
    bool ret = true;

    buf = NULL;

    RLOGI("abnormal:%s:%s", __func__, line);

    err = at_tok_start(&line);

    if (err != 0) {
        RLOGE("Failed to start parsing token");
        ret = false;
        goto on_exit;
    }

    err = at_tok_nextint(&line, &sn);

    if (err != 0) {
        RLOGE("Failed to parse token");
        ret = false;
        goto on_exit;
    }

    err = at_tok_nextint(&line, &eventid);

    if (err != 0) {
        RLOGE("Failed to parse token");
        ret = false;
        goto on_exit;
    }

    err = at_tok_nextint(&line, &page);

    if (err != 0) {
        RLOGE("Failed to parse token");
        ret = false;
        goto on_exit;
    }

    err = at_tok_nextint(&line, &page_cont);

    if (err != 0) {
        RLOGE("Failed to parse token");
        ret = false;
        goto on_exit;
    }

    err = at_tok_nextstr(&line, &buf);

    if (err != 0) {
        RLOGE("Failed to parse token");
        ret = false;
        goto on_exit;
    }

    if (page < 1 || page_cont < 1 || page_cont > 10) {
        RLOGE("MDBGINFO:page or pages error");
        ret = false;
        goto on_exit;
    }

    if (page == 1) {
        memset(&modem_info, 0, sizeof(RIL_ModemInfo));

        modem_info.abnormal_type_id = eventid;
        len = strlen(buf) * page_cont;
        modem_info.len = strlen(buf);
        modem_info.st = malloc(len + 1);

        memset(modem_info.st, 0, len + 1);
        memcpy(modem_info.st, buf, strlen(buf));
    } else if ((page <= page_cont) && (modem_info.abnormal_type_id == eventid)) {
        memcpy(modem_info.st + modem_info.len, buf, strlen(buf));
        modem_info.len = modem_info.len + strlen(buf);
    } else {
        ret = false;
        RLOGE("MDBGINFO:memcpy buf error");
        goto on_exit;
    }

    RLOGD("On modem debug info URC, sn: %d, eventid: %d, page: %d, pages: %d, buf: %s", sn,
        eventid, page, page_cont, buf);

    if (page == page_cont) {
        RIL_onUnsolicitedResponse(RIL_UNSOL_ABNORMAL_EVENT, &modem_info, sizeof(RIL_ModemInfo));
        RLOGI("MDBGINFO:report ofono");
        free(modem_info.st);
        memset(&modem_info, 0, sizeof(RIL_ModemInfo));
    }

on_exit:
    if (ret == false) {
        free(modem_info.st);
        memset(&modem_info, 0, sizeof(RIL_ModemInfo));
        RLOGE("Failed to handle ringback tone URC");
    }
}

static void on_modem_oem_hook_raw_indication(const char* s)
{
    char *line, *p;
    char* response;
    uint8_t* resp_data;

    response = NULL;
    resp_data = NULL;
    line = p = strdup(s);

    if (line == NULL) {
        RLOGE("%s: Failed to allocate memory", __func__);
        return;
    }

    if (at_tok_start(&p) < 0) {
        RLOGE("%s: invalid response string.", __func__);
        free(line);
        return;
    }

    if (at_tok_nextstr(&p, &response) < 0) {
        RLOGE("%s: invalid oem unsol data.", __func__);
        free(line);
        return;
    }

    resp_data = convertHexStringToBytes(response, strlen(response));
    if (resp_data == NULL) {
        RLOGE("%s: Failed to convert hex string to bytes.", __func__);
        free(line);
        return;
    }

    RIL_onUnsolicitedResponse(RIL_UNSOL_OEM_HOOK_RAW, resp_data, strlen(response) / 2);
    free(resp_data);
    free(line);
}

static void modem_upgrade_state_change(const char* s)
{
    char *line, *p;
    int state_value, ext_info, param_num;

    line = p = strdup(s);

    if (line == NULL) {
        RLOGE("%s: Failed to allocate memory", __func__);
        return;
    }

    if (at_tok_start(&p) < 0) {
        RLOGE("%s: invalid response string.", __func__);
        free(line);
        return;
    }

    if (at_tok_nextint(&p, &param_num) < 0) {
        RLOGE("Fail to parse param_num in %s", __func__);
        free(line);
        return;
    }

    RLOGD("param number =%d in %s", param_num, __func__);

    if (at_tok_nextint(&p, &state_value) < 0) {
        RLOGE("Fail to parse state_value in %s", __func__);
        free(line);
        return;
    }

    if (param_num == 2) {
        RIL_ModemUpgradeState modem_upgrade_value;
        if (at_tok_nextint(&p, &ext_info) < 0) {
            RLOGE("Fail to parse ext_info in %s", __func__);
            free(line);
            return;
        }

        modem_upgrade_value.state_value = state_value;
        modem_upgrade_value.ext_info = ext_info;
        RIL_onUnsolicitedResponse(RIL_UNSOL_MODEM_UPGRADE_STATE_CHANGED, &modem_upgrade_value, sizeof(RIL_ModemUpgradeState));
    } else {
        RIL_onUnsolicitedResponse(RIL_UNSOL_MODEM_UPGRADE_STATE_CHANGED, &state_value, sizeof(int));
    }
    free(line);
}

bool try_handle_unsol_modem(const char* s)
{
    bool ret = false;

    RLOGD("unsol modem string: %s", s);

    if (strStartsWith(s, "+CTEC: ")) {
        RLOGI("Receive technology URC");
        int tech, mask;
        switch (parse_technology_response(s, &tech, NULL)) {
        case -1: // no argument could be parsed.
            RLOGE("invalid CTEC line %s\n", s);
            break;
        case 1: // current mode correctly parsed
        case 0: // preferred mode correctly parsed
            mask = 1 << tech;
            if (mask != MDM_GSM && mask != MDM_CDMA && mask != MDM_WCDMA && mask != MDM_LTE) {
                RLOGE("Unknown technology %d\n", tech);
            } else {
                setRadioTechnology(sMdmInfo, tech);
            }
            break;
        }

        ret = true;
    } else if (strStartsWith(s, "^MRINGTONE: ")) {
        RLOGI("Receive ring tone URC");
        unsolicitedRingBackTone(s);
        ret = true;
    } else if (strStartsWith(s, "+CFUN: 0")) {
        RLOGI("Receive radio off URC");
        setRadioState(RADIO_STATE_OFF);
        ret = true;
    } else if (strStartsWith(s, "^MDBGINFO")) {
        RLOGI("Receive modem debug info URC");

        on_modem_debug_info_unsol_resp(s);

        ret = true;
    } else if (strStartsWith(s, "^UHOOKRAW: ")) {
        RLOGI("Receive modem oem hook raw URC");
        on_modem_oem_hook_raw_indication(s);
        ret = true;
    } else if (strStartsWith(s, "^MDRESTART")) {
        RLOGI("Receive modem restart URC");
        RIL_onUnsolicitedResponse(RIL_UNSOL_MODEM_RESTART, NULL, 0);
        ret = true;
    } else if (strStartsWith(s, "^MDUPGRADESTATECHANGE")) {
        RLOGI("Receive modem upgrade state change URC");
        modem_upgrade_state_change(s);
    } else {
        RLOGD("Can't match any unsol modem handlers");
    }

    return ret;
}
