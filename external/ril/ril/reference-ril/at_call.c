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

#define LOG_TAG "AT_CALL"
#define NDEBUG 1

#include <assert.h>
#include <stdio.h>
#include <sys/cdefs.h>

#include <telephony/ril_log.h>

#include "at_call.h"
#include "at_modem.h"
#include "at_ril.h"
#include "at_sim.h"
#include "at_tok.h"
#include "atchannel.h"
#include "misc.h"

#define MAX_PARTICIPANTS 5
#define MAX_TEL_DIGITS 15
#define MAX_ECC_COUNT 50

static int clccStateToRILState(int state, RIL_CallState* p_state)
{
    switch (state) {
    case 0:
        *p_state = RIL_CALL_ACTIVE;
        return 0;
    case 1:
        *p_state = RIL_CALL_HOLDING;
        return 0;
    case 2:
        *p_state = RIL_CALL_DIALING;
        return 0;
    case 3:
        *p_state = RIL_CALL_ALERTING;
        return 0;
    case 4:
        *p_state = RIL_CALL_INCOMING;
        return 0;
    case 5:
        *p_state = RIL_CALL_WAITING;
        return 0;
    case 6:
        *p_state = RIL_CALL_DISCONNECT;
        return 0;
    default:
        return -1;
    }
}

/**
 * Note: directly modified line and has *p_call point directly into
 * modified line
 */
static int callFromCLCCLine(char* line, RIL_Call* p_call)
{
    // +CLCC: 1,0,2,0,0,\"+18005551212\",145
    //     index,isMT,state,mode,isMpty(,number,TOA)?

    int err;
    int state;
    int mode;

    err = at_tok_start(&line);
    if (err < 0) {
        RLOGE("Failed to parse line in %s", __func__);
        goto error;
    }

    err = at_tok_nextint(&line, &(p_call->index));
    if (err < 0) {
        RLOGE("Failed to parse index in %s", __func__);
        goto error;
    }

    err = at_tok_nextbool(&line, &(p_call->isMT));
    if (err < 0) {
        RLOGE("Failed to parse isMT in %s", __func__);
        goto error;
    }

    err = at_tok_nextint(&line, &state);
    if (err < 0) {
        RLOGE("Failed to parse state in %s", __func__);
        goto error;
    }

    err = clccStateToRILState(state, &(p_call->state));
    if (err < 0) {
        RLOGE("Failed to parse call state in %s", __func__);
        goto error;
    }

    err = at_tok_nextint(&line, &mode);
    if (err < 0) {
        RLOGE("Failed to parse mode in %s", __func__);
        goto error;
    }

    p_call->isVoice = (mode == 0);

    err = at_tok_nextbool(&line, &(p_call->isMpty));
    if (err < 0) {
        RLOGE("Failed to parse isMpty in %s", __func__);
        goto error;
    }

    if (at_tok_hasmore(&line)) {
        err = at_tok_nextstr(&line, &(p_call->number));

        /* tolerate null here */
        if (err < 0) {
            RLOGE("tolerate null here");
            return 0;
        }

        // Some lame implementations return strings
        // like "NOT AVAILABLE" in the CLCC line
        if (p_call->number != NULL
            && 0 == strspn(p_call->number, "+0123456789")) {
            p_call->number = NULL;
        }

        err = at_tok_nextint(&line, &p_call->toa);
        if (err < 0) {
            RLOGE("Failed to parse toa in %s", __func__);
            goto error;
        }
    }

    p_call->uusInfo = NULL;

    return 0;

error:
    RLOGE("invalid CLCC line");
    return -1;
}

static void requestCallFailCause(void* data, size_t datalen, RIL_Token t)
{
    (void)data;
    (void)datalen;

    char* line = NULL;
    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int err = AT_ERROR_GENERIC;
    int cause = 0;

    err = at_send_command_singleline("AT+CEER?", "+CEER:", &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("%s: Failure occurred in sending %s, ret: %s, p_response: %p, final response: %s",
            __func__, "AT+CEER?", at_io_err_str(err), p_response,
            p_response ? p_response->finalResponse : "null");
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    line = p_response->p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) {
        RLOGE("Failed to parse line in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    err = at_tok_nextint(&line, &cause);
    if (err < 0) {
        RLOGE("Failed to parse fail cause in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, ril_err == RIL_E_SUCCESS ? &cause : NULL,
        ril_err == RIL_E_SUCCESS ? sizeof(cause) : 0);
    at_response_free(p_response);
}

// Hang up, reject, conference, call waiting
static void requestCallSelection(void* data, size_t datalen, RIL_Token t, int request)
{
    (void)data;
    (void)datalen;

    // 3GPP 22.030 6.5.5
    char hangupWaiting[] = "AT+CHLD=0";
    char hangupForeground[] = "AT+CHLD=1";
    char switchWaiting[] = "AT+CHLD=2";
    char conference[] = "AT+CHLD=3";
    char transfer[] = "AT+CHLD=4";
    char reject[] = "ATH";

    char* atCommand = NULL;
    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int err = AT_ERROR_GENERIC;

    if (getSIMStatus() == SIM_ABSENT) {
        RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
        return;
    }

    switch (request) {
    case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND:
        // "Releases all held calls or sets User Determined User Busy
        //  (UDUB) for a waiting call."
        atCommand = hangupWaiting;
        break;
    case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND:
        // "Releases all active calls (if any exist) and accepts
        //  the other (held or waiting) call."
        atCommand = hangupForeground;
        break;
    case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
        // "Places all active calls (if any exist) on hold and accepts
        //  the other (held or waiting) call."
        atCommand = switchWaiting;
        break;
    case RIL_REQUEST_CONFERENCE:
        // "Adds a held call to the conversation"
        atCommand = conference;
        break;
    case RIL_REQUEST_UDUB:
        // User determined user busy (reject)
        atCommand = reject;
        break;
    case RIL_REQUEST_EXPLICIT_CALL_TRANSFER:
        // "Transfers call"
        atCommand = transfer;
        break;
    default:
        assert(0);
    }

    err = at_send_command(atCommand, &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("%s: Failure occurred in sending %s, ret: %s, p_response: %p, final response: %s",
            __func__, atCommand, at_io_err_str(err), p_response,
            p_response ? p_response->finalResponse : "null");
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

on_exit:
    // Success or failure is ignored by the upper layer here.
    // It will call GET_CURRENT_CALLS and determine success that way.
    RIL_onRequestComplete(t, ril_err, NULL, 0);
    at_response_free(p_response);
}

static void requestGetCurrentCalls(void* data, size_t datalen, RIL_Token t)
{
    (void)data;
    (void)datalen;

    int err = AT_ERROR_GENERIC;
    ATResponse* p_response = NULL;
    ATLine* p_cur = NULL;
    int countCalls = 0;
    int countValidCalls = 0;
    RIL_Call* p_calls = NULL;
    RIL_Call** pp_calls = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;

    err = at_send_command_multiline("AT+CLCC", "+CLCC:", &p_response);

    /* CLCC allows empty line if no calls found */
    if (err == AT_ERROR_INVALID_RESPONSE) {
        RLOGW("No current calls found");
        goto on_exit;
    }

    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("%s: Failure occurred in sending %s, ret: %s, p_response: %p, final response: %s",
            __func__, "AT+CLCC", at_io_err_str(err), p_response,
            p_response ? p_response->finalResponse : "null");
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    /* count the calls */
    for (p_cur = p_response->p_intermediates; p_cur != NULL; p_cur = p_cur->p_next) {
        countCalls++;
    }

    /* yes, there's an array of pointers and then an array of structures */

    pp_calls = (RIL_Call**)alloca(countCalls * sizeof(RIL_Call*));
    p_calls = (RIL_Call*)alloca(countCalls * sizeof(RIL_Call));
    memset(p_calls, 0, countCalls * sizeof(RIL_Call));

    /* init the pointer array */
    for (int i = 0; i < countCalls; i++) {
        pp_calls[i] = &(p_calls[i]);
    }

    for (countValidCalls = 0, p_cur = p_response->p_intermediates; p_cur != NULL; p_cur = p_cur->p_next) {
        err = callFromCLCCLine(p_cur->line, p_calls + countValidCalls);

        if (err != 0) {
            RLOGE("Failed to parse the CLCC line");
            continue;
        }

        countValidCalls++;
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, ril_err == RIL_E_SUCCESS ? pp_calls : NULL,
        ril_err == RIL_E_SUCCESS ? countValidCalls * sizeof(RIL_Call*) : 0);
    at_response_free(p_response);
}

static void requestDtmfStart(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;

    char c_key;
    char* cmd = NULL;
    ATResponse* p_response = NULL;
    int err = AT_ERROR_GENERIC;
    RIL_Errno ril_err = RIL_E_SUCCESS;

    if (NULL == data) {
        RLOGE("%s: Data is NULL!", __func__);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    c_key = ((char*)data)[0];
    if (!(c_key >= '0' && c_key <= '9')
        && c_key != '#' && c_key != '*'
        && !(c_key >= 'A' && c_key <= 'D')) {
        RLOGE("Invalid argument in RIL");
        ril_err = RIL_E_INVALID_ARGUMENTS;
        goto on_exit;
    }

    if (asprintf(&cmd, "AT+VTS=%c", c_key) < 0) {
        RLOGE("Failed to allocate memory");
        ril_err = RIL_E_NO_MEMORY;
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

static void requestDtmfStop(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;

    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int err = -1;

    if (NULL != data) {
        RLOGE("%s: Data is NULL!", __func__);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    err = at_send_command("AT+VTS=", &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("%s: Failure occurred in sending %s, ret: %s, p_response: %p, final response: %s",
            __func__, "AT+VTS=", at_io_err_str(err), p_response,
            p_response ? p_response->finalResponse : "null");
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, NULL, 0);
    at_response_free(p_response);
}

static void requestDial(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;

    RIL_Dial* p_dial = NULL;
    char* cmd = NULL;
    const char* clir = NULL;
    int err = AT_ERROR_GENERIC;
    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;

    if (data == NULL) {
        RLOGD("req_dial data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    p_dial = (RIL_Dial*)data;

    switch (p_dial->clir) {
    case 1:
        clir = "I";
        break; /* invocation */
    case 2:
        clir = "i";
        break; /* suppression */
    default:
    case 0:
        clir = "";
        break; /* subscription default */
    }

    if (asprintf(&cmd, "ATD%s%s;", p_dial->address, clir) < 0) {
        RLOGE("Failed to allocate memory");
        ril_err = RIL_E_NO_MEMORY;
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
    /* success or failure is ignored by the upper layer here.
     * it will call GET_CURRENT_CALLS and determine success that way */
    RIL_onRequestComplete(t, ril_err, NULL, 0);
    at_response_free(p_response);
    free(cmd);
}

static void requestHangup(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;

    int* p_line = NULL;
    char* cmd = NULL;
    int err = AT_ERROR_GENERIC;
    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;

    if (data == NULL) {
        RLOGE("%s: req_dial data is null!", __func__);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    p_line = (int*)data;

    // 3GPP 22.030 6.5.5
    // "Releases a specific active call X"
    if (asprintf(&cmd, "AT+CHLD=1%d", p_line[0]) < 0) {
        RLOGE("Failed to allocate memory");
        ril_err = RIL_E_NO_MEMORY;
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
    /* success or failure is ignored by the upper layer here.
     * it will call GET_CURRENT_CALLS and determine success that way */
    RIL_onRequestComplete(t, ril_err, NULL, 0);
    at_response_free(p_response);
    free(cmd);
}

static void requestEccDial(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;

    char cmd[64] = { 0 };
    const char* clir = NULL;
    int err = AT_ERROR_GENERIC;
    RIL_EmergencyDial* p_eccDial = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    ATResponse* p_response = NULL;

    if (data == NULL) {
        RLOGE("req_emergency_dial data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    p_eccDial = (RIL_EmergencyDial*)data;

    switch (p_eccDial->dialInfo.clir) {
    case 0: /* subscription default */
        clir = "";
        break;
    case 1: /* invocation */
        clir = "I";
        break;
    case 2: /* suppression */
        clir = "i";
        break;
    default:
        break;
    }

    // In the goldfish emulator, the modem_simulator handles emergency calls
    // using the AT command format: ATD<number>@[category],#[clir];
    snprintf(cmd, sizeof(cmd), "ATD%s@,#%s;", p_eccDial->dialInfo.address, clir);
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
}

static void requestSetEmergencyNumbers(void* data, size_t datalen, RIL_Token t)
{
    ATResponse* p_response = NULL;
    RIL_EmergencyInfo* ecc_info = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int err = AT_ERROR_GENERIC;
    char cmd[512] = { 0 };
    int n = 0;

    if (data == NULL) {
        RLOGE("requestSetEmergencyNumbers: data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    ecc_info = (RIL_EmergencyInfo*)data;
    n = ecc_info->count;
    if (n == 0) {
        snprintf(cmd, sizeof(cmd), "AT+EMERGENCYNUM=0");
    } else {
        snprintf(cmd, sizeof(cmd), "AT+EMERGENCYNUM=%d,", n);
        for (int i = 0; i < n; i++) {
            snprintf(cmd + strlen(cmd), sizeof(cmd) - strlen(cmd), "%s,%d,%d,",
                ecc_info->numbers[i].eccNumber, ecc_info->numbers[i].category,
                ecc_info->numbers[i].condition);
        }
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
}

static void requestHandleConference(int request, void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;

    ATResponse* p_response = NULL;
    RIL_ConferenceInvite* cinfo = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int err = AT_ERROR_GENERIC;
    char* cmd = NULL;

    if (data == NULL) {
        RLOGE("requestHandleConference data is invalid");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    cinfo = (RIL_ConferenceInvite*)data;
    int nmembers = cinfo->nparticipants;

    if (request == RIL_REQUEST_DIAL_CONFERENCE) {
        if (nmembers < 2 || nmembers > 5) {
            RLOGE("Invalid number of members");
            ril_err = RIL_E_INVALID_ARGUMENTS;
            goto on_exit;
        }
    } else {
        if (nmembers < 1 || nmembers > 5) {
            RLOGE("Invalid number of members");
            ril_err = RIL_E_INVALID_ARGUMENTS;
            goto on_exit;
        }
    }

    char* numbers = cinfo->numbers;

    if (numbers == NULL) {
        RLOGE("Invalid number string!");
        ril_err = RIL_E_INVALID_ARGUMENTS;
        goto on_exit;
    }

    int len = strlen("AT+MPC=1,") + MAX_TEL_DIGITS * MAX_PARTICIPANTS + /* tel numbers */
        MAX_PARTICIPANTS * 2 + /* double qoutes */
        MAX_PARTICIPANTS - 1 + /* commas */
        +1; /* null terminator */

    cmd = malloc(len);

    if (!cmd) {
        RLOGE("Failed to allocate memory");
        ril_err = RIL_E_NO_MEMORY;
        goto on_exit;
    }

    memset(cmd, 0, len);

    if (request == RIL_REQUEST_DIAL_CONFERENCE) {
        strcpy(cmd, "AT+MPC=1,");
    } else {
        strcpy(cmd, "AT+MPC=0,");
    }

    char* token = strtok(numbers, ";");

    for (int i = 0; i < nmembers && token; ++i) {
        /* get first number and add to the cmd buffer */
        strcat(cmd, "\"");
        strcat(cmd, token);
        strcat(cmd, "\"");

        if (i < nmembers - 1)
            strcat(cmd, ",");

        token = strtok(NULL, ";");
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

/**
 * @brief send the RIL_UNSOL_SUPP_SVC_NOTIFICATION to ofono
 *
 * @param notificationType        0 = MO intermediate result code; 1 = MT unsolicited result code
 * @param code      See 27.007 7.17,"code1" for MO , "code2" for MT.
 * @param index     CUG index. See 27.007 7.17.
 * @param type      "type" from 27.007 7.17 (MT only).
 * @param number    "number" from 27.007 7.17( MT only, may be NULL.)
 */
static void unsolicitedSuppSvcNotification(int notificationType,
    int code, int index, int type, const char* number)
{
    RIL_SuppSvcNotification response = { 0 };

    if ((notificationType != 0) && (notificationType != 1)) {
        RLOGW("unsolicitedSuppSvcNotification notificationType is out of range!");
        return;
    }

    RLOGD("unsolicitedSuppSvcNotification notification code is [%d]!", code);

    response.notificationType = notificationType;
    response.code = code;
    response.index = index;
    response.type = type;

    if (number != NULL) {
        RLOGD("unsolicitedSuppSvcNotification response number [%s]!", number);
        response.number = (char*)number;
    } else {
        response.number = NULL;
    }

    RIL_onUnsolicitedResponse(RIL_UNSOL_SUPP_SVC_NOTIFICATION, &response, sizeof(RIL_SuppSvcNotification));
}

static void suppSvcChanged(const char* s)
{
    char *line = NULL, *p = NULL;
    int call_index = -1;

    line = p = strdup(s);
    if (!line) {
        RLOGE("%s: Unable to allocate memory for %s.", __func__, s);
        return;
    }

    if (at_tok_start(&p) < 0) {
        RLOGE("%s: invalid response string (%s)", __func__, s);
        free(line);
        return;
    }

    if (at_tok_nextint(&p, &call_index) < 0) {
        RLOGE("%s: invalid string args (%s).", __func__, s);
        free(line);
        return;
    }

    if (strStartsWith(s, "HOLD: ")) {
        unsolicitedSuppSvcNotification(1, 2, call_index, 0, NULL);
    } else if (strStartsWith(s, "UNHOLD: ")) {
        unsolicitedSuppSvcNotification(1, 3, call_index, 0, NULL);
    } else if (strStartsWith(s, "MPTY: ")) {
        unsolicitedSuppSvcNotification(1, 4, call_index, 0, NULL);
    } else if (strStartsWith(s, "UNMPTY: ")) {
        unsolicitedSuppSvcNotification(1, 10, call_index, 0, NULL);
    }

    free(line);
}

static void unsolicitedEccListChanged(const char* s)
{
    char* ecc_list[MAX_ECC_COUNT] = { NULL };
    int count = 0;
    char *line = NULL, *p = NULL;

    line = p = strdup(s);
    if (!line) {
        RLOGE("+ECCL: Unable to allocate memory");
        return;
    }

    if (at_tok_start(&p) < 0) {
        RLOGE("%s: invalid response string", __func__);
        free(line);
        return;
    }

    if (at_tok_nextint(&p, &count) < 0) {
        RLOGE("invalid ecc list count");
        free(line);
        return;
    }

    for (int i = 0; i < count; i++) {
        if (at_tok_nextstr(&p, &ecc_list[i]) < 0) {
            RLOGE("invalid ecc_number[%d]", i);
            free(line);
            return;
        }
    }

    RIL_onUnsolicitedResponse(RIL_UNSOL_EMERGENCY_NUMBER_LIST, ecc_list, count * sizeof(char*));
    free(line);
}

static void unsolicitedCallStateChanged(const char* s)
{
    char *line = NULL, *p = NULL;
    int count = 0;
    int state;
    int mode;
    RIL_Call* p_calls = NULL;
    RIL_Call** pp_calls = NULL;
    int err = -1;

    line = p = strdup(s);
    if (!line) {
        RLOGE("+CRING: Unable to allocate memory.");
        return;
    }

    if (at_tok_start(&p) < 0) {
        RLOGE("%s: invalid response string.", __func__);
        free(line);
        return;
    }

    if (at_tok_nextint(&p, &count) < 0) {
        RLOGE("%s: Invalid call list count.", __func__);
        free(line);
        return;
    }

    pp_calls = (RIL_Call**)alloca(count * sizeof(RIL_Call*));
    p_calls = (RIL_Call*)alloca(count * sizeof(RIL_Call));
    memset(p_calls, 0, count * sizeof(RIL_Call));

    for (int i = 0; i < count; i++) {
        pp_calls[i] = &(p_calls[i]);
        err = at_tok_nextint(&p, &(p_calls[i].index));
        if (err < 0) {
            RLOGE("%s: Failed to parse call index (%d).", __func__, i);
            goto error;
        }

        err = at_tok_nextbool(&p, &(p_calls[i].isMT));
        if (err < 0) {
            RLOGE("%s: Failed to parse isMT (%d).", __func__, i);
            goto error;
        }

        err = at_tok_nextint(&p, &state);
        if (err < 0) {
            RLOGE("%s, Failed to parse call state (%d)", __func__, i);
            goto error;
        }

        err = clccStateToRILState(state, &(p_calls[i].state));
        if (err < 0) {
            RLOGE("%s: Failed to transform call state (%d).", __func__, i);
            goto error;
        }

        err = at_tok_nextint(&p, &mode);
        if (err < 0) {
            RLOGE("%s: Failed to parse call mode (%d).", __func__, i);
            goto error;
        }

        p_calls[i].isVoice = (mode == 0);

        err = at_tok_nextbool(&p, &(p_calls[i].isMpty));
        if (err < 0) {
            RLOGE("%s: Failed to parse call mpty (%d).", __func__, i);
            goto error;
        }

        if (at_tok_hasmore(&p)) {
            err = at_tok_nextstr(&p, &(p_calls[i].number));
            if (err < 0) {
                RLOGW("%s: Tolerate null here (%d).", __func__, i);
            }

            // Some lame implementations return strings
            // like "NOT AVAILABLE" in this line
            if (p_calls[i].number != NULL
                && 0 == strspn(p_calls[i].number, "+0123456789")) {
                p_calls[i].number = NULL;
            }

            err = at_tok_nextint(&p, &(p_calls[i].toa));
            if (err < 0) {
                RLOGE("%s: Failed to parse call toa (%d).", __func__, i);
                goto error;
            }
        }

        p_calls[i].uusInfo = NULL;
    }

    RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, pp_calls, count * sizeof(RIL_Call*));
    free(line);
    return;

error:
    RLOGE("%s: call state changed unsolicited response error.", __func__);
    free(line);
}

static void requestChangeBarringPassword(char** data, size_t datalen, RIL_Token t)
{
    int err = AT_ERROR_GENERIC;
    char* cmd = NULL;
    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;

    if (data == NULL) {
        RLOGE("requestChangeBarringPassword data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    if (datalen != 3 * sizeof(char*)
        || data[0] == NULL || data[1] == NULL
        || data[2] == NULL || strlen(data[0]) == 0
        || strlen(data[1]) == 0 || strlen(data[2]) == 0) {
        RLOGE("Invalid arguments");
        RIL_onRequestComplete(t, RIL_E_INVALID_ARGUMENTS, NULL, 0);
        return;
    }

    if (asprintf(&cmd, "AT+CPWD=\"%s\",\"%s\",\"%s\"", data[0], data[1], data[2]) < 0) {
        RLOGE("Failed to allocate memory");
        ril_err = RIL_E_NO_MEMORY;
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

static void requestSetCallWaiting(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;

    ATResponse* p_response = NULL;
    int err = AT_ERROR_GENERIC;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    char cmd[32] = { 0 };
    int enable;
    int serviceClass;

    if (data == NULL) {
        RLOGE("requestSetCallWaiting data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    enable = ((int*)data)[0];
    serviceClass = ((int*)data)[1];

    if (serviceClass == 0) {
        snprintf(cmd, sizeof(cmd), "AT+CCWA=1,%d", enable);
    } else {
        snprintf(cmd, sizeof(cmd), "AT+CCWA=1,%d,%d", enable, serviceClass);
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
}

static void requestQueryCallWaiting(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;

    ATResponse* p_response = NULL;
    int err = AT_ERROR_GENERIC;
    int mode = 0;
    int response[2] = { 0, 0 };
    char cmd[32] = { 0 };
    char* line = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int serviceClass;

    if (data == NULL) {
        RLOGE("requestQueryCallWaiting data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    serviceClass = ((int*)data)[0];
    if (serviceClass == 0) {
        snprintf(cmd, sizeof(cmd), "AT+CCWA=1,2");
    } else {
        snprintf(cmd, sizeof(cmd), "AT+CCWA=1,2,%d", serviceClass);
    }

    err = at_send_command_multiline(cmd, "+CCWA:", &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("%s: Failure occurred in sending %s, ret: %s, p_response: %p, final response: %s",
            __func__, cmd, at_io_err_str(err), p_response,
            p_response ? p_response->finalResponse : "null");
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    for (ATLine* p_cur = p_response->p_intermediates; p_cur != NULL; p_cur = p_cur->p_next) {
        line = p_cur->line;
        err = at_tok_start(&line);
        if (err < 0) {
            RLOGE("Failed to parse line in %s", __func__);
            ril_err = RIL_E_GENERIC_FAILURE;
            goto on_exit;
        }

        err = at_tok_nextint(&line, &mode);
        if (err < 0) {
            RLOGE("Fail to parse mode in %s", __func__);
            ril_err = RIL_E_GENERIC_FAILURE;
            goto on_exit;
        }

        err = at_tok_nextint(&line, &serviceClass);
        if (err < 0) {
            RLOGE("Fail to parse service class in %s", __func__);
            ril_err = RIL_E_GENERIC_FAILURE;
            goto on_exit;
        }

        response[0] = mode;
        response[1] |= serviceClass;
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, ril_err == RIL_E_SUCCESS ? response : NULL,
        ril_err == RIL_E_SUCCESS ? sizeof(response) : 0);
    at_response_free(p_response);
}

static int forwardFromCCFCULine(char* line, RIL_CallForwardInfo* p_forward)
{
    int err = AT_ERROR_GENERIC;
    int i = 0;

    if (line == NULL || p_forward == NULL) {
        RLOGE("Line inivalid");
        goto error;
    }

    err = at_tok_start(&line);
    if (err < 0) {
        RLOGE("Failed to parse line in %s", __func__);
        goto error;
    }

    err = at_tok_nextint(&line, &(p_forward->status));
    if (err < 0) {
        RLOGE("Failed to parse status in %s", __func__);
        goto error;
    }

    err = at_tok_nextint(&line, &(p_forward->serviceClass));
    if (err < 0) {
        RLOGE("Failed to parse class in %s", __func__);
        goto error;
    }

    if (at_tok_hasmore(&line)) {
        int numberType = 0;
        err = at_tok_nextint(&line, &numberType);
        if (err < 0) {
            RLOGE("Failed to parse numberType in %s", __func__);
            goto error;
        }

        err = at_tok_nextint(&line, &p_forward->toa);
        if (err < 0) {
            RLOGE("Failed to parse toa in %s", __func__);
            goto error;
        }

        err = at_tok_nextstr(&line, &(p_forward->number));

        /* tolerate null here */
        if (err < 0) {
            RLOGD("Number is null!");
            return 0;
        }

        if (at_tok_hasmore(&line)) {
            for (i = 0; i < 2; i++) {
                skipNextComma(&line);
            }

            if (at_tok_hasmore(&line)) {
                err = at_tok_nextint(&line, &p_forward->timeSeconds);
                if (err < 0) {
                    p_forward->timeSeconds = 0;
                }
            }
        }
    }

    return 0;

error:
    return -1;
}

static void requestQueryCallForward(void* data, size_t datalen, RIL_Token t)
{
    int err = AT_ERROR_GENERIC;
    char cmd[128] = { 0 };
    ATResponse* p_response = NULL;
    ATLine* p_cur = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int validCount = 0;
    int forwardCount = 0;
    RIL_CallForwardInfo* info = NULL;

    if (data == NULL || datalen < sizeof(RIL_CallForwardInfo)) {
        RLOGD("requestQueryCallForward data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    info = (RIL_CallForwardInfo*)data;

    if (info->reason < 0 || info->reason > 5) {
        RLOGE("requestQueryCallForward reason is invalid!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    snprintf(cmd, sizeof(cmd), "AT+CCFCU=%d,2,%d,%d,\"%s\",%d",
        info->reason, 2, info->toa, info->number ? info->number : "", info->serviceClass);
    err = at_send_command_multiline(cmd, "+CCFCU:", &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("%s: Failure occurred in sending %s, ret: %s, p_response: %p, final response: %s",
            __func__, cmd, at_io_err_str(err), p_response,
            p_response ? p_response->finalResponse : "null");
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    RIL_CallForwardInfo **forwardList = NULL, *forwardPool = NULL;

    for (p_cur = p_response->p_intermediates; p_cur != NULL;
         p_cur = p_cur->p_next, forwardCount++) {
    }

    forwardList = (RIL_CallForwardInfo**)
        alloca(forwardCount * sizeof(RIL_CallForwardInfo*));

    forwardPool = (RIL_CallForwardInfo*)
        alloca(forwardCount * sizeof(RIL_CallForwardInfo));

    memset(forwardPool, 0, forwardCount * sizeof(RIL_CallForwardInfo));

    /* init the pointer array */
    for (int i = 0; i < forwardCount; i++) {
        forwardList[i] = &(forwardPool[i]);
    }

    for (p_cur = p_response->p_intermediates;
         p_cur != NULL; p_cur = p_cur->p_next) {
        err = forwardFromCCFCULine(p_cur->line, forwardList[validCount]);
        forwardList[validCount]->reason = info->reason;
        if (err == 0)
            validCount++;
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, validCount ? forwardList : NULL,
        validCount * sizeof(RIL_CallForwardInfo*));
    at_response_free(p_response);
}

static void requestSetCallForward(void* data, size_t datalen, RIL_Token t)
{
    int err = AT_ERROR_GENERIC;
    char cmd[256] = { 0 };
    ATResponse* p_response = NULL;
    RIL_CallForwardInfo* info = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;

    if (data == NULL || datalen != sizeof(RIL_CallForwardInfo)) {
        RLOGE("Invalid input: data is null or datalen mismatch");
        RIL_onRequestComplete(t, RIL_E_INVALID_ARGUMENTS, NULL, 0);
        return;
    }

    info = (RIL_CallForwardInfo*)data;
    if ((info->status == 3 && info->number == NULL) || info->reason < 0 || info->reason > 5) {
        RLOGE("Invalid parameters: status=%d, number=%s, reason=%d",
            info->status, info->number, info->reason);
        RIL_onRequestComplete(t, RIL_E_INVALID_ARGUMENTS, NULL, 0);
        return;
    }

    int len = snprintf(cmd, sizeof(cmd), "AT+CCFCU=%d,%d,%d,%d,\"%s\",%d",
        info->reason, info->status, 2, info->toa,
        info->number ? info->number : "", info->serviceClass);
    if (info->reason == 2 && info->status == 3 && info->timeSeconds > 0) {
        len += snprintf(cmd + len, sizeof(cmd) - len, ",\"\",\"\",,%d", info->timeSeconds);
    } else {
        len += snprintf(cmd + len, sizeof(cmd) - len, ",\"\"");
    }

    if (len >= (int)sizeof(cmd)) {
        RLOGE("Command buffer overflow");
        ril_err = RIL_E_NO_MEMORY;
        goto on_exit;
    }

    err = at_send_command_multiline(cmd, "+CCFCU:", &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("%s: Failure occurred in sending %s, ret: %s, p_response: %p, final response: %s",
            __func__, cmd, at_io_err_str(err), p_response,
            p_response ? p_response->finalResponse : "null");
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    RLOGD("requestSetCallForward reason = %d, status = %d, number = %s, toa = %d, class = %d",
        info->reason, info->status, info->number, info->toa, info->serviceClass);

on_exit:
    RIL_onRequestComplete(t, ril_err, NULL, 0);
    at_response_free(p_response);
}

static void requestSetClir(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;

    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int err = AT_ERROR_GENERIC;
    char cmd[16] = { 0 };
    int clir;

    if (data == NULL) {
        RLOGE("requestSetClir: data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    clir = ((int*)data)[0];
    if (clir < 0 || clir > 2) {
        RLOGE("clir is invalid!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    snprintf(cmd, sizeof(cmd), "AT+CLIR=%d", clir);
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
}

static void requestQueryClir(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;
    (void)data;

    int err = AT_ERROR_GENERIC;
    int response[2] = { 1, 1 };
    char* line = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    ATResponse* p_response = NULL;

    if (getSIMStatus() == SIM_ABSENT) {
        RIL_onRequestComplete(t, RIL_E_MODEM_ERR, NULL, 0);
        return;
    }

    err = at_send_command_singleline("AT+CLIR?", "+CLIR:", &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("%s: Failure occurred in sending %s, ret: %s, p_response: %p, final response: %s",
            __func__, "AT+CLIR?", at_io_err_str(err), p_response,
            p_response ? p_response->finalResponse : "null");
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    line = p_response->p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) {
        RLOGE("Failed to parse line in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    err = at_tok_nextint(&line, &response[0]);
    if (err < 0) {
        RLOGE("Failed to parse response[0] in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    err = at_tok_nextint(&line, &response[1]);
    if (err < 0) {
        RLOGE("Failed to parse response[0] in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, ril_err == RIL_E_SUCCESS ? response : NULL,
        ril_err == RIL_E_SUCCESS ? sizeof(response) : 0);
    at_response_free(p_response);
}

static void requestQueryClip(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;
    (void)data;

    int err = AT_ERROR_GENERIC;
    int skip = 0;
    int response = 0;
    char* line = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    ATResponse* p_response = NULL;

    if (getSIMStatus() == SIM_ABSENT) {
        RIL_onRequestComplete(t, RIL_E_MODEM_ERR, NULL, 0);
        return;
    }

    err = at_send_command_singleline("AT+CLIP?", "+CLIP:", &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("%s: Failure occurred in sending %s, ret: %s, p_response: %p, final response: %s",
            __func__, "AT+CLIP?", at_io_err_str(err), p_response,
            p_response ? p_response->finalResponse : "null");
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    line = p_response->p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) {
        RLOGE("Failed to parse line in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    err = at_tok_nextint(&line, &skip);
    if (err < 0) {
        RLOGE("Failed to parse integer in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    err = at_tok_nextint(&line, &response);
    if (err < 0) {
        RLOGE("Failed to parse clip in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, ril_err == RIL_E_SUCCESS ? &response : NULL,
        ril_err == RIL_E_SUCCESS ? sizeof(response) : 0);
    at_response_free(p_response);
}

static void requestGetMute(void* data, size_t datalen, RIL_Token t)
{
    (void)data;
    (void)datalen;

    int err = AT_ERROR_GENERIC;
    int muteResponse = 0; // Mute disabled
    char* line = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    ATResponse* p_response = NULL;

    err = at_send_command_singleline("AT+CMUT?", "+CMUT:", &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("%s: Failure occurred in sending %s, ret: %s, p_response: %p, final response: %s",
            __func__, "AT+CMUT?", at_io_err_str(err), p_response,
            p_response ? p_response->finalResponse : "null");
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    line = p_response->p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) {
        RLOGE("Failed to parse line in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

    err = at_tok_nextint(&line, &muteResponse);
    if (err < 0) {
        RLOGE("Failed to parse mute in %s", __func__);
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

on_exit:
    RIL_onRequestComplete(t, ril_err, ril_err == RIL_E_SUCCESS ? &muteResponse : NULL,
        ril_err == RIL_E_SUCCESS ? sizeof(muteResponse) : 0);
    at_response_free(p_response);
}

static void requestSetMute(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;

    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int err = AT_ERROR_GENERIC;
    char cmd[16] = { 0 };
    int mute;

    if (data == NULL) {
        RLOGE("requestSetMute data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    mute = ((int*)data)[0];
    if (mute < 0 || mute > 1) {
        RLOGE("Invalid mute value: %d", mute);
        RIL_onRequestComplete(t, RIL_E_INVALID_ARGUMENTS, NULL, 0);
        return;
    }

    snprintf(cmd, sizeof(cmd), "AT+CMUT=%d", mute);
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
}

static void requestAnswer(void* data, size_t datalen, RIL_Token t)
{
    (void)data;
    (void)datalen;

    int err = AT_ERROR_GENERIC;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    ATResponse* p_response = NULL;

    err = at_send_command("ATA", &p_response);
    if (err != AT_ERROR_OK || !p_response || p_response->success != AT_OK) {
        RLOGE("%s: Failure occurred in sending %s, ret: %s, p_response: %p, final response: %s",
            __func__, "ATA", at_io_err_str(err), p_response,
            p_response ? p_response->finalResponse : "null");
        ril_err = RIL_E_GENERIC_FAILURE;
        goto on_exit;
    }

on_exit:
    // Success or failure is ignored by the upper layer here.
    // It will call GET_CURRENT_CALLS and determine success that way.
    RIL_onRequestComplete(t, ril_err, NULL, 0);
    at_response_free(p_response);
}

static void requestSeparateConnection(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;

    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int err = AT_ERROR_GENERIC;
    char cmd[16] = { 0 };
    int party;

    if (data == NULL) {
        RLOGE("requestSeparateConnection data is null!");
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    party = ((int*)data)[0];

    // Make sure that party is in a valid range.
    // (Note: The Telephony middle layer imposes a range of 1 to 7.
    // It's sufficient for us to just make sure it's single digit.)
    if (party <= 0 || party > 9) {
        RLOGE("requestSeparateConnection party：%d is invalid!", party);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    snprintf(cmd, sizeof(cmd), "AT+CHLD=2%d", party);
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
}

static void requestExitEmergencyCallbackMode(void* data, size_t datalen, RIL_Token t)
{
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
}

static void requestGetVoiceRadioTech(void* data, size_t datalen, RIL_Token t)
{
    int tech = techFromModemType(TECH(getModemInfo()));
    if (tech < 0)
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    else
        RIL_onRequestComplete(t, RIL_E_SUCCESS, &tech, sizeof(tech));
}

static void requestDeflectCall(void* data, size_t datalen, RIL_Token t)
{
    (void)datalen;

    ATResponse* p_response = NULL;
    RIL_Errno ril_err = RIL_E_SUCCESS;
    int err = AT_ERROR_GENERIC;
    char cmd[32] = { 0 };

    if (data == NULL) {
        RLOGE("data in %s is NULL!", __func__);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    snprintf(cmd, sizeof(cmd), "AT+CTFR=%s", (char*)data);
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
}

static void requestGetTtyMode(void* data, size_t datalen, RIL_Token t)
{
    (void)data;
    (void)datalen;

    int ttyModeResponse;
    // 1 is TTY Full, 0 is TTY Off
    ttyModeResponse = (getSIMStatus() == SIM_READY) ? 1 : 0;
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &ttyModeResponse, sizeof(ttyModeResponse));
}

void on_request_call(int request, void* data, size_t datalen, RIL_Token t)
{
    switch (request) {
    case RIL_REQUEST_GET_CURRENT_CALLS:
        requestGetCurrentCalls(data, datalen, t);
        break;
    case RIL_REQUEST_DIAL:
        requestDial(data, datalen, t);
        break;
    case RIL_REQUEST_HANGUP:
        requestHangup(data, datalen, t);
        break;
    case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND:
    case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND:
    case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
    case RIL_REQUEST_CONFERENCE:
    case RIL_REQUEST_UDUB:
    case RIL_REQUEST_EXPLICIT_CALL_TRANSFER:
        requestCallSelection(data, datalen, t, request);
        break;
    case RIL_REQUEST_LAST_CALL_FAIL_CAUSE:
        requestCallFailCause(data, datalen, t);
        break;
    case RIL_REQUEST_DTMF:
        requestDtmfStart(data, datalen, t);
        break;
    case RIL_REQUEST_GET_CLIR:
        requestQueryClir(data, datalen, t);
        break;
    case RIL_REQUEST_SET_CLIR:
        requestSetClir(data, datalen, t);
        break;
    case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS:
        requestQueryCallForward(data, datalen, t);
        break;
    case RIL_REQUEST_SET_CALL_FORWARD:
        requestSetCallForward(data, datalen, t);
        break;
    case RIL_REQUEST_QUERY_CALL_WAITING:
        requestQueryCallWaiting(data, datalen, t);
        break;
    case RIL_REQUEST_SET_CALL_WAITING:
        requestSetCallWaiting(data, datalen, t);
        break;
    case RIL_REQUEST_ANSWER:
        requestAnswer(data, datalen, t);
        break;
    case RIL_REQUEST_CHANGE_BARRING_PASSWORD:
        requestChangeBarringPassword(data, datalen, t);
        break;
    case RIL_REQUEST_DTMF_START:
        requestDtmfStart(data, datalen, t);
        break;
    case RIL_REQUEST_DTMF_STOP:
        requestDtmfStop(data, datalen, t);
        break;
    case RIL_REQUEST_SEPARATE_CONNECTION:
        requestSeparateConnection(data, datalen, t);
        break;
    case RIL_REQUEST_SET_MUTE:
        requestSetMute(data, datalen, t);
        break;
    case RIL_REQUEST_GET_MUTE:
        requestGetMute(data, datalen, t);
        break;
    case RIL_REQUEST_QUERY_CLIP:
        requestQueryClip(data, datalen, t);
        break;
    case RIL_REQUEST_SET_TTY_MODE:
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
        break;
    case RIL_REQUEST_QUERY_TTY_MODE:
        requestGetTtyMode(data, datalen, t);
        break;
    case RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE:
        requestExitEmergencyCallbackMode(data, datalen, t);
        break;
    case RIL_REQUEST_VOICE_RADIO_TECH:
        requestGetVoiceRadioTech(data, datalen, t);
        break;
    case RIL_REQUEST_DEFLECT_CALL:
        requestDeflectCall(data, datalen, t);
        break;
    case RIL_REQUEST_EMERGENCY_DIAL:
        requestEccDial(data, datalen, t);
        break;
    case RIL_REQUEST_ADD_PARTICIPANT:
    case RIL_REQUEST_DIAL_CONFERENCE:
        requestHandleConference(request, data, datalen, t);
        break;
    case RIL_REQUEST_SET_EMERGENCY_NUMBER:
        requestSetEmergencyNumbers(data, datalen, t);
        break;
    default:
        RLOGE("Request not supported");
        RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
        break;
    }

    RLOGD("On request call end\n");
}

bool try_handle_unsol_call(const char* s)
{
    bool ret = false;
    char *line = NULL, *p;

    RLOGD("unsol call string: %s", s);

    if (strStartsWith(s, "RING")
        || strStartsWith(s, "NO CARRIER")
        || strStartsWith(s, "+CCWA")
        || strStartsWith(s, "ALERTING")) {
        RLOGI("Receive call state changed URC");
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, NULL, 0);
        ret = true;
    } else if (strStartsWith(s, "+CRING: ")) {
        RLOGI("Receive call state changed URC with data");
        unsolicitedCallStateChanged(s);
        ret = true;
    } else if (strStartsWith(s, "HOLD: ")
        || strStartsWith(s, "UNHOLD: ")
        || strStartsWith(s, "MPTY: ")
        || strStartsWith(s, "UNMPTY: ")) {
        RLOGI("Receive supplementary service URC(%s)", s);
        suppSvcChanged(s);
        ret = true;
    } else if (strStartsWith(s, "+WSOS: ")) {
        RLOGI("Receive emergency mode changed URC");
        char state = 0;
        int unsol;
        line = p = strdup(s);
        if (!line) {
            RLOGE("+WSOS: Unable to allocate memory");
            return true;
        }
        if (at_tok_start(&p) < 0) {
            RLOGE("invalid response string");
            free(line);
            return true;
        }
        if (at_tok_nextbool(&p, &state) < 0) {
            RLOGE("invalid +WSOS response: %s", line);
            free(line);
            return true;
        }
        free(line);

        unsol = state ? RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE : RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE;

        RIL_onUnsolicitedResponse(unsol, NULL, 0);
        ret = true;
    } else if (strStartsWith(s, "+ECCL: ")) {
        RLOGI("Receive emergency number list URC");
        unsolicitedEccListChanged(s);
        ret = true;
    } else {
        RLOGD("Can't match any unsol call handlers");
    }

    return ret;
}
