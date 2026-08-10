#include "ril_parcel_ctor_verifier.h"
#include <cstring>
#include <jstring.h>
#include <syslog.h>
#include <telephony/ril.h>

static bool construct_void(Parcel& p);
static bool construct_enable_switch(Parcel& p);
static bool construct_disable_switch(Parcel& p);
static bool construct_oem_hook_strings(Parcel& p);
static bool construct_dial(Parcel& p);
static bool construct_hangup(Parcel& p);
static bool construct_emergency_dial(Parcel& p);
static bool construct_set_ecc_number(Parcel& p);
static bool construct_network_select_manual(Parcel& p);
static bool construct_set_pref_network_type(Parcel& p);
static bool construct_setup_data_call(Parcel& p);
static bool construct_deactive_data_call(Parcel& p);
static bool construct_set_data_profile(Parcel& p);
static bool construct_get_imsi(Parcel& p);
static bool construct_SIMIO(Parcel& p);
static bool construct_send_sms(Parcel& p);
static bool construct_set_callforwarding(Parcel& p);
static bool construct_cancel_callforwarding(Parcel& p);
static bool construct_transmit_apdu_basic(Parcel& p);
static bool construct_query_callforward_status(Parcel& p);
static bool construct_set_callwaiting(Parcel& p);
static bool construct_get_callwaiting(Parcel& p);
static bool construct_set_ims_cap(Parcel& p);

static bool verify_sim_status(int token, Parcel& p);
static bool verify_void(int token, Parcel& p);
static bool verify_baseband_version(int token, Parcel& p);
static bool verify_oem_hook_strings(int token, Parcel& p);
static bool verify_get_modem_status(int token, Parcel& p);
static bool verify_get_current_calls(int token, Parcel& p);
static bool verify_disconnect_reason(int token, Parcel& p);
static bool verify_registration_state(int token, Parcel& p);
static bool verify_operator(int token, Parcel& p);
static bool verify_IMEI(int token, Parcel& p);
static bool verify_IMEISV(int token, Parcel& p);
static bool verify_available_networks(int token, Parcel& p);
static bool verify_pref_network_type(int token, Parcel& p);
static bool verify_cellinfo_list(int token, Parcel& p);
static bool verify_signal_strength(int token, Parcel& p);
static bool verify_setup_data_call(int token, Parcel& p);
static bool verify_data_call_list(int token, Parcel& p);
static bool verify_imsi(int token, Parcel& p);
static bool verify_SIMIO(int token, Parcel& p);
static bool verify_send_sms(int token, Parcel& p);
static bool verify_transmit_apdu_basic(int token, Parcel& p);
static bool verify_query_callforward_status(int token, Parcel& p);
static bool verify_get_callwaiting(int token, Parcel& p);
static bool verify_ims_registration(int token, Parcel& p);
static bool verify_hardware_config(int token, Parcel& p);

static const char* requestToString(int request);

static char* strdupReadString(Parcel& p)
{
    size_t l;
    const char* s;

    s = p.readString8Inplace(&l);
    return s ? strndup(s, l) : NULL;
}

static void writeStringToParcel(Parcel& p, const char* s)
{
    p.writeString8(s, s ? strlen(s) : 0);
}

static struct ril_test_case cases[] = {
    { 0, "EnableModem", RIL_REQUEST_ENABLE_MODEM, construct_enable_switch, verify_void },
    { 1, "DisableModem", RIL_REQUEST_ENABLE_MODEM, construct_disable_switch, verify_void },
    { 2, "GetSimStatus", RIL_REQUEST_GET_SIM_STATUS, construct_void, verify_sim_status },
    { 3, "GetBaseBandVersion", RIL_REQUEST_BASEBAND_VERSION, construct_void, verify_baseband_version },
    { 4, "OemHookStrings", RIL_REQUEST_OEM_HOOK_STRINGS, construct_oem_hook_strings, verify_oem_hook_strings },
    { 5, "GetModemStatus", RIL_REQUEST_GET_MODEM_STATUS, construct_void, verify_get_modem_status },
    { 6, "EnableScreenState", RIL_REQUEST_SCREEN_STATE, construct_enable_switch, verify_void },
    { 7, "DisableScreenState", RIL_REQUEST_SCREEN_STATE, construct_disable_switch, verify_void },
    { 8, "RadioPowerOn", RIL_REQUEST_RADIO_POWER, construct_enable_switch, verify_void },
    { 9, "RadioPowerOff", RIL_REQUEST_RADIO_POWER, construct_disable_switch, verify_void },
    { 10, "Dial", RIL_REQUEST_DIAL, construct_dial, verify_void },
    { 11, "GetCurrentCall", RIL_REQUEST_GET_CURRENT_CALLS, construct_void, verify_get_current_calls },
    { 12, "Hangup", RIL_REQUEST_HANGUP, construct_hangup, verify_void },
    { 13, "DisconnectReason", RIL_REQUEST_LAST_CALL_FAIL_CAUSE, construct_void, verify_disconnect_reason },
    { 14, "Answer", RIL_REQUEST_ANSWER, construct_void, verify_void },
    { 15, "EmergencyDial", RIL_REQUEST_EMERGENCY_DIAL, construct_emergency_dial, verify_void },
    { 16, "SetEccNumber", RIL_REQUEST_SET_EMERGENCY_NUMBER, construct_set_ecc_number, verify_void },
    { 17, "GetVoiceRegistrationState", RIL_REQUEST_VOICE_REGISTRATION_STATE, construct_void, verify_registration_state },
    { 18, "GetDataRegistrationState", RIL_REQUEST_DATA_REGISTRATION_STATE, construct_void, verify_registration_state },
    { 19, "GetOperator", RIL_REQUEST_OPERATOR, construct_void, verify_operator },
    { 20, "GetIMEI", RIL_REQUEST_GET_IMEI, construct_void, verify_IMEI },
    { 21, "GetIMEISV", RIL_REQUEST_GET_IMEISV, construct_void, verify_IMEISV },
    { 22, "SetNetworkSelectionAutomatic", RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC, construct_void, verify_void },
    { 23, "SetNetworkSelectionManual", RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL, construct_network_select_manual, verify_void },
    { 24, "QueryAvailableNetworks", RIL_REQUEST_QUERY_AVAILABLE_NETWORKS, construct_void, verify_available_networks },
    { 25, "SetPrefNetworkType", RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE, construct_set_pref_network_type, verify_void },
    { 26, "GetPrefNetworkType", RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE, construct_void, verify_pref_network_type },
    { 27, "GetCellInfoList", RIL_REQUEST_GET_CELL_INFO_LIST, construct_void, verify_cellinfo_list },
    { 28, "GetSignalStrength", RIL_REQUEST_SIGNAL_STRENGTH, construct_void, verify_signal_strength },
    { 29, "SetupDataCall", RIL_REQUEST_SETUP_DATA_CALL, construct_setup_data_call, verify_setup_data_call },
    { 30, "DeactiveDataCall", RIL_REQUEST_DEACTIVATE_DATA_CALL, construct_deactive_data_call, verify_void },
    { 31, "GetDataCallList", RIL_REQUEST_DATA_CALL_LIST, construct_void, verify_data_call_list },
    { 32, "SetDataProfile", RIL_REQUEST_SET_DATA_PROFILE, construct_set_data_profile, verify_void },
    { 33, "GetIMSI", RIL_REQUEST_GET_IMSI, construct_get_imsi, verify_imsi },
    { 34, "SIMIO", RIL_REQUEST_SIM_IO, construct_SIMIO, verify_SIMIO },
    { 35, "SendSms", RIL_REQUEST_SEND_SMS, construct_send_sms, verify_send_sms },
    { 36, "SetCallforwarding", RIL_REQUEST_SET_CALL_FORWARD, construct_set_callforwarding, verify_void },
    { 37, "CancelCallforwarding", RIL_REQUEST_SET_CALL_FORWARD, construct_cancel_callforwarding, verify_void },
    { 38, "GetNeighboringCellIDs", RIL_REQUEST_GET_NEIGHBORING_CELL_IDS, construct_void, verify_cellinfo_list },
    { 39, "TransmitAPDUBasic", RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC, construct_transmit_apdu_basic, verify_transmit_apdu_basic },
    { 40, "GetCallforwardStatus", RIL_REQUEST_QUERY_CALL_FORWARD_STATUS, construct_query_callforward_status, verify_query_callforward_status },
    { 41, "SetCallWaiting", RIL_REQUEST_SET_CALL_WAITING, construct_set_callwaiting, verify_void },
    { 42, "GetCallWaiting", RIL_REQUEST_QUERY_CALL_WAITING, construct_get_callwaiting, verify_get_callwaiting },
    { 43, "EnableIMS", RIL_REQUEST_IMS_REG_STATE_CHANGE, construct_enable_switch, verify_void },
    { 44, "GetIMSRegistration", RIL_REQUEST_IMS_REGISTRATION_STATE, construct_void, verify_ims_registration },
    { 45, "SetIMSCap", RIL_REQUEST_IMS_SET_SERVICE_STATUS, construct_set_ims_cap, verify_void },
    { 46, "GetHardwareConfig", RIL_REQUEST_GET_HARDWARE_CONFIG, construct_void, verify_hardware_config },
};

static int ntest_cases = sizeof(cases) / sizeof(cases[0]);

void print_usage(const char* prog)
{
    syslog(LOG_ERR, "Usage: %s <sim_id> <test_case_id>", prog);
    syslog(LOG_ERR, "We have %d testcases in total", ntest_cases);
    for (int i = 0; i < ntest_cases; i++) {
        syslog(LOG_ERR, "testcase id: %d, name: %s", cases[i].id, cases[i].case_name);
    }
}

int case_num(void)
{
    return ntest_cases;
}

struct ril_test_case* get_test_case(int id)
{
    return cases + id;
}

static bool is_valid_sol_parcel_header(int token, Parcel& p)
{
    int32_t i;

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "%s: Failed to read resp type.", __func__);
        return false;
    }

    syslog(LOG_INFO, "resp type: %ld", i);
    if (i != RESPONSE_SOLICITED) {
        syslog(LOG_DEBUG, "%s: resp type is not RIL_RESPONSE_SOLICITED.", __func__);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "%s: Failed to read token.", __func__);
        return false;
    }

    syslog(LOG_INFO, "token: %ld", i);
    if (i != token) {
        syslog(LOG_DEBUG, "%s: token(%ld) is not %d.", __func__, i, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "%s: Failed to read ril_err.", __func__);
        return false;
    }

    syslog(LOG_INFO, "ril_err: %ld", i);
    if (i != RIL_E_SUCCESS) {
        syslog(LOG_DEBUG, "%s: ril_err is not RIL_E_SUCCESS.", __func__);
        return false;
    }

    syslog(LOG_INFO, "sol parcel header verified.");

    return true;
}

bool verify_unsolicited(Parcel& p)
{
    int32_t i;
    syslog(LOG_INFO, "Unsolicited response");
    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "%s: Failed to read resp type.", __func__);
        return false;
    }

    if (i != RESPONSE_UNSOLICITED) {
        syslog(LOG_ERR, "%s: resp type is not RIL_RESPONSE_UNSOLICITED.", __func__);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "%s: Failed to read unsol resp type.", __func__);
        return false;
    }

    syslog(LOG_INFO, "unsol_resp_type: %s (%ld)", requestToString(i), i);

    // TODO: verify unsolicited response

    return true;
}

static bool verify_sim_status(int token, Parcel& p)
{
    int32_t i;
    char* str = NULL;
    int num_applications;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    syslog(LOG_INFO, "%s: Parcel data starts:", __func__);
    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read card state.");
        return false;
    }

    syslog(LOG_INFO, "card state: %ld", i);
    if (i < RIL_CARDSTATE_ABSENT || i > RIL_CARDSTATE_RESTRICTED) {
        syslog(LOG_ERR, "Invalid card state");
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read pin state.");
        return false;
    }

    syslog(LOG_INFO, "pin state: %ld", i);
    if (i < RIL_PINSTATE_UNKNOWN || i > RIL_PINSTATE_ENABLED_PERM_BLOCKED) {
        syslog(LOG_ERR, "Invalid pin state");
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read gsm_umts_subscription_app_index.");
        return false;
    }

    syslog(LOG_INFO, "gsm_umts_subscription_app_index: %ld", i);
    if (i < -1 || i >= RIL_CARD_MAX_APPS) {
        syslog(LOG_ERR, "Invalid gsm_umts_subscription_app_index");
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read cdma_subscription_app_index.");
        return false;
    }

    syslog(LOG_INFO, "cdma_subscription_app_index: %ld", i);
    if (i < -1 || i >= RIL_CARD_MAX_APPS) {
        syslog(LOG_ERR, "Invalid cdma_subscription_app_index");
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read ims_subscription_app_index.");
        return false;
    }

    syslog(LOG_INFO, "ims_subscription_app_index: %ld", i);
    if (i < -1 || i >= RIL_CARD_MAX_APPS) {
        syslog(LOG_ERR, "Invalid ims_subscription_app_index");
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read num_applications.");
        return false;
    }

    syslog(LOG_INFO, "num_applications: %ld", i);
    if (i < 0 || i > RIL_CARD_MAX_APPS) {
        syslog(LOG_ERR, "Invalid num_applications");
        return false;
    }

    num_applications = i;

    syslog(LOG_INFO, "applications start: ");
    for (int index = 0; index < num_applications; index++) {
        syslog(LOG_INFO, "\n");
        syslog(LOG_INFO, "applocation %d: ", index);

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read app type.");
            return false;
        }

        syslog(LOG_INFO, "app type: %ld", i);
        if (i < RIL_APPTYPE_UNKNOWN || i > RIL_APPTYPE_ISIM) {
            syslog(LOG_ERR, "Invalid app type");
            return false;
        }

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read app state.");
            return false;
        }

        syslog(LOG_INFO, "app state: %ld", i);
        if (i < RIL_APPSTATE_UNKNOWN || i > RIL_APPSTATE_READY) {
            syslog(LOG_ERR, "Invalid app state");
            return false;
        }

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read perso_substate.");
            return false;
        }

        syslog(LOG_INFO, "perso_substate: %ld", i);
        if (i < RIL_PERSOSUBSTATE_UNKNOWN || i > RIL_PERSOSUBSTATE_RUIM_RUIM_PUK) {
            syslog(LOG_ERR, "Invalid perso_substate");
            return false;
        }

        str = strdupReadString(p);
        syslog(LOG_INFO, "aid ptr: %s", str);
        free(str);
        str = NULL;

        str = strdupReadString(p);
        syslog(LOG_INFO, "app_label_ptr: %s", str);
        free(str);
        str = NULL;

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read pin1_replaced.");
            return false;
        }

        syslog(LOG_INFO, "pin1_replaced: %ld", i);

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read pin1.");
            return false;
        }

        syslog(LOG_INFO, "pin1: %ld", i);
        if (i < RIL_PINSTATE_UNKNOWN || i > RIL_PINSTATE_ENABLED_PERM_BLOCKED) {
            syslog(LOG_ERR, "Invalid pin1");
            return false;
        }

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read pin2");
            return false;
        }

        syslog(LOG_INFO, "pin2: %ld", i);
        if (i < RIL_PINSTATE_UNKNOWN || i > RIL_PINSTATE_ENABLED_PERM_BLOCKED) {
            syslog(LOG_ERR, "Invalid pin2");
            return false;
        }
    }

    return true;
}

static bool verify_baseband_version(int token, Parcel& p)
{
    char* str = NULL;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    str = strdupReadString(p);
    if (str == NULL) {
        syslog(LOG_ERR, "Failed to read baseband version.");
        return false;
    }

    syslog(LOG_INFO, "baseband version: %s", str);
    free(str);

    return true;
}

static bool verify_oem_hook_strings(int token, Parcel& p)
{
    char* resp_str = NULL;
    int32_t count = 0;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&count) != 0) {
        syslog(LOG_ERR, "Failed to read oem hook strings count.");
        return false;
    }

    syslog(LOG_INFO, "count: %ld", count);

    resp_str = strdupReadString(p);
    if (resp_str == NULL) {
        syslog(LOG_ERR, "Failed to read oem hook strings.");
        return false;
    }

    syslog(LOG_INFO, "oem hook strings: %s", resp_str);
    free(resp_str);

    return true;
}

static bool verify_get_modem_status(int token, Parcel& p)
{
    int32_t i = 0;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read args count.");
        return false;
    }

    if (i != 1) {
        syslog(LOG_ERR, "Invalid args count (%ld).", i);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read modem status.");
        return false;
    }

    syslog(LOG_INFO, "modem status: %ld", i);
    if (i != 0 && i != 1) {
        syslog(LOG_ERR, "Invalid modem status (%ld).", i);
        return false;
    }

    return true;
}

static bool verify_void(int token, Parcel& p)
{
    if (is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_DEBUG, "sol parcel header is valid");
        return true;
    } else {
        syslog(LOG_DEBUG, "sol parcel header is invalid");
        return false;
    }
}

static bool verify_get_current_calls(int token, Parcel& p)
{
    int32_t i = 0;
    char* str = NULL;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read call count.");
        return false;
    }

    syslog(LOG_INFO, "call count: %ld", i);
    if (i < 0) {
        syslog(LOG_ERR, "Invalid call count (%ld).", i);
        return false;
    }

    int count = (int)i;
    for (int index = 0; index < count; index++) {
        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read call state.");
            return false;
        }

        syslog(LOG_INFO, "call state: %ld", i);
        if (i < RIL_CALL_ACTIVE || i > RIL_CALL_WAITING) {
            syslog(LOG_ERR, "Invalid call state (%ld).", i);
            return false;
        }

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read call index.");
            return false;
        }

        syslog(LOG_INFO, "call index: %ld", i);
        if (i < 0) {
            syslog(LOG_ERR, "Invalid call index (%ld).", i);
            return false;
        }

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read toa.");
            return false;
        }

        syslog(LOG_INFO, "toa: %ld", i);
        if (i != 145 && i != 129) {
            syslog(LOG_ERR, "Invalid toa (%ld).", i);
            return false;
        }

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read mpty");
            return false;
        }

        syslog(LOG_INFO, "mpty: %ld", i);

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read isMT.");
            return false;
        }

        syslog(LOG_INFO, "isMT: %ld", i);
        if (i != 0 && i != 1) {
            syslog(LOG_ERR, "Invalid isMT (%ld).", i);
            return false;
        }

        if (p.readInt32(&i) != 0) {
            syslog(LOG_INFO, "Failed to read als.");
            return false;
        }

        syslog(LOG_INFO, "als: %ld", i);

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read call type.");
            return false;
        }

        if (i == 0) {
            syslog(LOG_ERR, "Invalid call type (%ld).", i);
            return false;
        }

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read isVoicePrivacy.");
            return false;
        }

        str = strdupReadString(p);
        if (str == NULL) {
            syslog(LOG_ERR, "Failed to read call number.");
            return false;
        }

        syslog(LOG_INFO, "call number: %s", str);
        free(str);
        str = NULL;

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read numberPresentation.");
            return false;
        }

        strdupReadString(p);

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read namePresentation.");
            return false;
        }

        if (p.readInt32(&i) != 0) {
            syslog(LOG_INFO, "Failed to read uusInfo");
            return false;
        }
    }

    return true;
}

static bool verify_disconnect_reason(int token, Parcel& p)
{
    int32_t i = 0;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read call index.");
        return false;
    }

    syslog(LOG_INFO, "disconnect reason: %ld", i);
    if (i != 1 && i != 16 && i != 17 && i != 34 && i != 68 && i != 240
        && i != 241 && i != 242 && i != 243 && i != 0xffff) {
        syslog(LOG_ERR, "Invalid disconnect reason (%ld).", i);
        return false;
    }

    return true;
}

static bool verify_registration_state(int token, Parcel& p)
{
    int32_t i = 0;
    int count = 0;
    char* str = NULL;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read string count.");
        return false;
    }

    syslog(LOG_INFO, "string count: %ld", i);
    count = (int)i;

    for (int index = 0; index < count; index++) {
        str = strdupReadString(p);
        syslog(LOG_INFO, "reg str: %s", str);
        free(str);
        str = NULL;
    }

    return true;
}

static bool verify_operator(int token, Parcel& p)
{
    int32_t i = 0;
    char* str = NULL;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read str count");
        return false;
    }

    syslog(LOG_INFO, "str count: %ld", i);
    if (i != 3) {
        syslog(LOG_ERR, "Invalid str count (%ld).", i);
        return false;
    }

    str = strdupReadString(p);
    if (str == NULL) {
        syslog(LOG_ERR, "Failed to read lalpha");
        return false;
    }

    syslog(LOG_INFO, "lalpha: %s", str);
    free(str);
    str = NULL;

    str = strdupReadString(p);
    if (str == NULL) {
        syslog(LOG_ERR, "Failed to read salpha");
        return false;
    }

    syslog(LOG_INFO, "salpha: %s", str);
    free(str);
    str = NULL;

    str = strdupReadString(p);
    if (str == NULL) {
        syslog(LOG_ERR, "Failed to read numeric.");
        return false;
    }

    syslog(LOG_INFO, "salpha: %s", str);
    free(str);
    str = NULL;

    return true;
}

static bool verify_IMEI(int token, Parcel& p)
{
    char* str = NULL;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    str = strdupReadString(p);
    if (str == NULL) {
        syslog(LOG_ERR, "Failed to read IMEI.");
        return false;
    }

    syslog(LOG_INFO, "IMEI: %s", str);
    if (strlen(str) != 15) {
        syslog(LOG_ERR, "IMEI is invalid.");
        return false;
    }

    return true;
}

static bool verify_IMEISV(int token, Parcel& p)
{
    char* str = NULL;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    str = strdupReadString(p);
    if (str == NULL) {
        syslog(LOG_ERR, "Failed to read IMEISV.");
        return false;
    }

    syslog(LOG_INFO, "IMEISV: %s", str);
    if (strlen(str) != 17) {
        syslog(LOG_ERR, "IMEISV is invalid.");
        return false;
    }

    return true;
}

static bool verify_available_networks(int token, Parcel& p)
{
    int32_t i = 0;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read num_strings.");
        return false;
    }

    if (i % 4) {
        syslog(LOG_ERR, "invalid QUERY_AVAIL_NETWORKS reply.");
        return false;
    }

    int count = i / 4;

    for (; count; count--) {
        char* lalpha = strdupReadString(p);
        syslog(LOG_INFO, "lalpha: %s", lalpha);
        free(lalpha);
        char* salpha = strdupReadString(p);
        syslog(LOG_INFO, "salpha: %s", salpha);
        free(salpha);
        char* numeric = strdupReadString(p);
        syslog(LOG_INFO, "numeric: %s", numeric);
        free(numeric);
        char* status = strdupReadString(p);
        syslog(LOG_INFO, "status: %s", status);
        free(status);
    }

    return true;
}

static bool verify_pref_network_type(int token, Parcel& p)
{
    int32_t i = 0;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read args count.");
        return false;
    }

    if (i != 1) {
        syslog(LOG_ERR, "args count is invalid.");
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read pref network type.");
        return false;
    }

    syslog(LOG_INFO, "pref network type: %ld", i);
    if (i < 0 || i > 12) {
        syslog(LOG_ERR, "pref network type is invalid.");
        return false;
    }

    return true;
}

static bool verify_cellinfo_list(int token, Parcel& p)
{
    int32_t i = 0;
    int cell_type;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read cell_info_cnt.");
        return false;
    }

    int cell_info_cnt = (int)i;

    for (int index = 0; index < cell_info_cnt; index++) {
        syslog(LOG_INFO, "\n");
        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read cell type.");
            return false;
        }

        syslog(LOG_INFO, "cell type: %ld", i);
        if (i < RIL_CELL_INFO_TYPE_NONE || i > RIL_CELL_INFO_TYPE_NR) {
            syslog(LOG_ERR, "cell type is invalid.");
            return false;
        }

        cell_type = (int)i;

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read registration.");
            return false;
        }

        syslog(LOG_INFO, "registration: %ld", i);
        if (i != 0 && i != 1) {
            syslog(LOG_ERR, "registration is invalid.");
            return false;
        }

        /* skipping unneeded timeStampType in Ril cell info */
        p.readInt32(&i);

        /*skipping timeStamp which is a uint64_t type */
        p.readInt32(&i);
        p.readInt32(&i);

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read mcc.");
            return false;
        }

        syslog(LOG_INFO, "mcc: %ld", i);
        if (i < 0 || i > 999) {
            syslog(LOG_ERR, "mcc is invalid.");
            return false;
        }

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read mnc.");
            return false;
        }

        syslog(LOG_INFO, "mnc: %ld", i);
        if (i < 0 || i > 999) {
            syslog(LOG_ERR, "Failed to read mnc");
            return false;
        }

        if (cell_type == RIL_CELL_INFO_TYPE_GSM) {
            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read lac.");
                return false;
            }

            syslog(LOG_INFO, "lac: %ld", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read ci.");
                return false;
            }

            syslog(LOG_INFO, "ci: %ld", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read strength.");
                return false;
            }

            syslog(LOG_INFO, "strength: %ld", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read ber.");
                return false;
            }

            syslog(LOG_INFO, "ber: %ld", i);
        } else if (cell_type == RIL_CELL_INFO_TYPE_WCDMA) {
            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read lac.");
                return false;
            }

            syslog(LOG_INFO, "lac: %ld", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read ci.");
                return false;
            }

            syslog(LOG_INFO, "ci: %ld", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read psc.");
                return false;
            }

            syslog(LOG_INFO, "psc: %ld", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read strength.");
                return false;
            }

            syslog(LOG_INFO, "strength: %ld", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "ber: %ld", i);
                return false;
            }

            syslog(LOG_INFO, "ber: %ld.", i);
        } else if (cell_type == RIL_CELL_INFO_TYPE_LTE) {
            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read ci.");
                return false;
            }

            syslog(LOG_INFO, "ci: %ld.", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read pci.");
                return false;
            }

            syslog(LOG_INFO, "pci: %ld.", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read tac.");
                return false;
            }

            syslog(LOG_INFO, "tac: %ld.", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read strength.");
                return false;
            }

            syslog(LOG_INFO, "strength: %ld", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read rsrp.");
                return false;
            }

            syslog(LOG_INFO, "rsrp: %ld.", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read rsrq.");
                return false;
            }

            syslog(LOG_INFO, "rsrq: %ld.", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read snr.");
                return false;
            }

            syslog(LOG_INFO, "snr: %ld.", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read cqi.");
                return false;
            }

            syslog(LOG_INFO, "cqi: %ld.", i);

            if (p.readInt32(&i) != 0) {
                syslog(LOG_ERR, "Failed to read tadv.");
                return false;
            }

            syslog(LOG_INFO, "tadv: %ld.", i);
        } else {
            syslog(LOG_ERR, "cell type is not supported.");
            return false;
        }
    }

    return true;
}

static bool verify_signal_strength(int token, Parcel& p)
{
    int32_t i = 0;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read gw_sigstr.");
        return false;
    }

    syslog(LOG_INFO, "gw_sigstr: %ld.", i);

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read gw_ber.");
        return false;
    }

    syslog(LOG_INFO, "gw_ber: %ld.", i);

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read cdma_dbm.");
        return false;
    }

    syslog(LOG_INFO, "cdma_dbm: %ld.", i);

    p.readInt32(&i); // ecio

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read evdo_dbm.");
        return false;
    }

    syslog(LOG_ERR, "evdo_dbm: %ld.", i);

    p.readInt32(&i); // ecio
    p.readInt32(&i); // signalNoiseRatio

    if (p.readInt32(&i) != 0) {
        syslog(LOG_INFO, "Failed to read lte_sigstr.");
        return false;
    }

    syslog(LOG_INFO, "lte_sigstr: %ld.", i);

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read lte_rsrp.");
        return false;
    }

    syslog(LOG_INFO, "lte_rsrp: %ld.", i);

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read lte_rsrq.");
        return false;
    }

    syslog(LOG_INFO, "lte_rsrq: %ld.", i);

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "lte_rssnr: %ld.", i);
        return false;
    }

    syslog(LOG_INFO, "lte_rssnr: %ld.", i);

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read lte_cqi.");
        return false;
    }

    syslog(LOG_INFO, "lte_cqi: %ld.", i);

    return true;
}

static bool verify_setup_data_call(int token, Parcel& p)
{
    int32_t i = 0;
    char* str = NULL;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read version.");
        return false;
    }

    syslog(LOG_INFO, "version: %ld.", i);

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read num_calls.");
        return false;
    }

    syslog(LOG_INFO, "num_calls: %ld.", i);

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read status.");
        return false;
    }

    syslog(LOG_INFO, "status: %ld.", i);

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read retry.");
        return false;
    }

    syslog(LOG_INFO, "retry: %ld.", i);

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read cid.");
        return false;
    }

    syslog(LOG_INFO, "cid: %ld.", i);

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read active.");
        return false;
    }

    syslog(LOG_INFO, "active: %ld.", i);

    str = strdupReadString(p);
    if (str == NULL) {
        syslog(LOG_ERR, "Failed to read type.");
        return false;
    }

    syslog(LOG_INFO, "type: %s.", str);
    free(str);
    str = NULL;

    str = strdupReadString(p);
    if (str == NULL) {
        syslog(LOG_ERR, "Failed to read ifname.");
        return false;
    }

    syslog(LOG_INFO, "ifname: %s.", str);
    free(str);
    str = NULL;

    str = strdupReadString(p);
    if (str == NULL) {
        syslog(LOG_ERR, "Failed to read raw_addrs.");
        return false;
    }

    syslog(LOG_INFO, "raw_addrs: %s.", str);
    free(str);
    str = NULL;

    str = strdupReadString(p);
    if (str == NULL) {
        syslog(LOG_ERR, "Failed to read raw_dns.");
        return false;
    }

    syslog(LOG_INFO, "raw_dns: %s.", str);
    free(str);
    str = NULL;

    str = strdupReadString(p);
    if (str == NULL) {
        syslog(LOG_ERR, "Failed to read raw_gws.");
        return false;
    }

    syslog(LOG_INFO, "raw_gws: %s", str);
    free(str);
    str = NULL;

    str = strdupReadString(p);
    if (str == NULL) {
        syslog(LOG_ERR, "Failed to read pcscf_addrs.");
        return false;
    }

    syslog(LOG_INFO, "pcscf_addrs: %s.", str);
    free(str);
    str = NULL;

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read mtu.");
        return false;
    }

    syslog(LOG_INFO, "mtu: %ld.", i);

    return true;
}

static bool verify_data_call_list(int token, Parcel& p)
{
    int32_t i = 0;
    char* str = NULL;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read version.");
        return false;
    }

    syslog(LOG_INFO, "version: %ld.", i);

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read num_calls.");
        return false;
    }

    syslog(LOG_INFO, "num_calls: %ld.", i);
    int num_calls = (int)i;

    for (int index = 0; index < num_calls; index++) {
        syslog(LOG_INFO, "\n");
        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read status.");
            return false;
        }

        syslog(LOG_INFO, "status: %ld.", i);

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read suggestedRetryTime.");
            return false;
        }

        syslog(LOG_INFO, "suggestedRetryTime: %ld", i);

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read cid.");
            return false;
        }

        syslog(LOG_INFO, "cid: %ld.", i);

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read active.");
            return false;
        }

        syslog(LOG_INFO, "active: %ld.", i);

        str = strdupReadString(p);
        if (str == NULL) {
            syslog(LOG_ERR, "Failed to read type.");
            return false;
        }

        syslog(LOG_INFO, "type: %s.", str);
        free(str);
        str = NULL;

        str = strdupReadString(p);
        if (str == NULL) {
            syslog(LOG_ERR, "Failed to read ifname.");
            return false;
        }

        syslog(LOG_INFO, "ifname: %s.", str);
        free(str);
        str = NULL;

        str = strdupReadString(p);
        if (str == NULL) {
            syslog(LOG_ERR, "Failed to read addresses.");
            return false;
        }

        syslog(LOG_INFO, "addresses: %s.", str);
        free(str);
        str = NULL;

        str = strdupReadString(p);
        if (str == NULL) {
            syslog(LOG_ERR, "Failed to read dns.");
            return false;
        }

        syslog(LOG_INFO, "dns: %s.", str);
        free(str);
        str = NULL;

        str = strdupReadString(p);
        if (str == NULL) {
            syslog(LOG_ERR, "Failed to read gateways.");
            return false;
        }

        syslog(LOG_INFO, "gateways: %s.", str);
        free(str);
        str = NULL;

        str = strdupReadString(p);
        if (str == NULL) {
            syslog(LOG_ERR, "Failed to read pcscf.");
            return false;
        }

        syslog(LOG_INFO, "pcscf: %s.", str);
        free(str);
        str = NULL;

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read mtu.");
            return false;
        }

        syslog(LOG_INFO, "mtu: %ld.", i);
    }

    return true;
}

static bool verify_imsi(int token, Parcel& p)
{
    char* str = NULL;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    str = strdupReadString(p);
    if (str == NULL) {
        syslog(LOG_ERR, "Failed to read IMSI.");
        return false;
    }

    syslog(LOG_INFO, "IMSI: %s.", str);
    if (strlen(str) != 15) {
        syslog(LOG_ERR, "IMSI is invalid.");
        return false;
    }

    free(str);
    str = NULL;

    return true;
}

static bool verify_SIMIO(int token, Parcel& p)
{
    int32_t i = 0;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read sw1");
        return false;
    }

    syslog(LOG_INFO, "sw1: %ld.", i);
    if (i != 144) {
        syslog(LOG_ERR, "sw1 is error.");
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read sw2.");
        return false;
    }

    syslog(LOG_INFO, "sw2: %ld.", i);
    if (i != 0) {
        syslog(LOG_ERR, "sw2 is error.");
        return false;
    }

    strdupReadString(p);

    return true;
}

static bool verify_send_sms(int token, Parcel& p)
{
    int32_t i = 0;
    char* str = NULL;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read mr.");
        return false;
    }

    syslog(LOG_INFO, "mr: %ld.", i);

    str = strdupReadString(p);
    syslog(LOG_INFO, "ack_pdu: %s.", str);

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read error.");
        return false;
    }

    syslog(LOG_INFO, "error: %ld.", i);

    return true;
}

static bool verify_transmit_apdu_basic(int token, Parcel& p)
{
    int32_t i = 0;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read sw1");
        return false;
    }

    syslog(LOG_INFO, "sw1: %ld.", i);
    if (i != 0x90) {
        syslog(LOG_ERR, "sw1 is error.");
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read sw2.");
        return false;
    }

    syslog(LOG_INFO, "sw2: %ld.", i);
    if (i != 0x00) {
        syslog(LOG_ERR, "sw2 is error.");
        return false;
    }

    return true;
}

static bool verify_query_callforward_status(int token, Parcel& p)
{
    int32_t i = 0;
    char* str = NULL;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read list_size.");
        return false;
    }

    int list_size = (int)i;
    for (int index = 0; index < list_size; index++) {
        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read status.");
            return false;
        }

        syslog(LOG_INFO, "status: %ld", i);

        /* skip reason */
        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read reason.");
            return false;
        }

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read cls.");
            return false;
        }

        syslog(LOG_INFO, "cls: %ld.", i);

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read phone_number type.");
            return false;
        }

        syslog(LOG_INFO, "phone_number type: %ld.", i);

        str = strdupReadString(p);
        syslog(LOG_INFO, "phone_number: %s.", str);

        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read time.");
            return false;
        }

        syslog(LOG_INFO, "time: %ld.", i);
    }

    return true;
}

static bool verify_get_callwaiting(int token, Parcel& p)
{
    int32_t i = 0;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read num_params.");
        return false;
    }

    syslog(LOG_INFO, "num_params: %ld.", i);
    if (i < 1) {
        syslog(LOG_ERR, "num_params is error.");
        return false;
    }

    int num_params = (int)i;

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read enable.");
        return false;
    }

    syslog(LOG_INFO, "enable: %ld.", i);
    if (i != 0 && num_params < 2) {
        syslog(LOG_ERR, "enable is error.");
        return false;
    }

    if (i > 0) {
        if (p.readInt32(&i) != 0) {
            syslog(LOG_ERR, "Failed to read service_class.");
            return false;
        }
    }

    return true;
}

static bool verify_ims_registration(int token, Parcel& p)
{
    int32_t i = 0;
    char* str = NULL;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read reg_info.");
        return false;
    }

    syslog(LOG_INFO, "reg_info: %ld.", i);

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "Failed to read ext_info.");
        return false;
    }

    syslog(LOG_INFO, "exit_info: %ld.", i);
    if (i != 1 && i != 4 && i != 5) {
        syslog(LOG_ERR, "exit_info is error.");
        return false;
    }

    str = strdupReadString(p);
    if (str == NULL) {
        syslog(LOG_ERR, "Failed to read subscriber_uri.");
        return false;
    }

    syslog(LOG_INFO, "subscriber_uri: %s.", str);
    free(str);

    return true;
}

static bool verify_hardware_config(int token, Parcel& p)
{
    int32_t i = 0;
    int num = 0;
    int type = 0;
    char* str = NULL;

    if (!is_valid_sol_parcel_header(token, p)) {
        syslog(LOG_ERR, "%s: Testcase(%d)'s parcel header is invalid.", __func__, token);
        return false;
    }

    if (p.readInt32(&i) != 0) {
        syslog(LOG_ERR, "%s: Failed to read num.", __func__);
        return false;
    }

    num = i;
    for (int j = 0; j < num; j++) {
        syslog(LOG_INFO, "\n\n index: %d", j);
        p.readInt32(&i);
        if (i != RIL_HARDWARE_CONFIG_MODEM && i != RIL_HARDWARE_CONFIG_SIM) {
            syslog(LOG_ERR, "%s: Failed to read type.", __func__);
            return false;
        }
        syslog(LOG_INFO, "type: %ld", i);
        type = i;

        str = strdupReadString(p);
        syslog(LOG_INFO, "uuid: %s", str);
        free(str);

        p.readInt32(&i);
        if (i < RIL_HARDWARE_CONFIG_STATE_ENABLED || RIL_HARDWARE_CONFIG_STATE_DISABLED > 2) {
            syslog(LOG_ERR, "%s: Failed to read state.", __func__);
            return false;
        }
        syslog(LOG_INFO, "state: %ld", i);

        if (type == RIL_HARDWARE_CONFIG_MODEM) {
            p.readInt32(&i);
            syslog(LOG_INFO, "rilModel: %ld", i);
            p.readInt32(&i);
            syslog(LOG_INFO, "rat: %ld", i);
            p.readInt32(&i);
            syslog(LOG_INFO, "maxVoice: %ld", i);
            p.readInt32(&i);
            syslog(LOG_INFO, "maxData: %ld", i);
            p.readInt32(&i);
            syslog(LOG_INFO, "maxStandby: %ld", i);
        } else {
            str = strdupReadString(p);
            syslog(LOG_INFO, "modemUuid: %s", str);
            free(str);
        }
    }

    return true;
}

static bool construct_void(Parcel& p)
{
    if (p.writeInt32(0) != 0) {
        syslog(LOG_ERR, "%s: Failed to write int32.", __func__);
        return false;
    }

    return true;
}

static bool construct_enable_switch(Parcel& p)
{
    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write args count.", __func__);
        return false;
    }

    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write enable.", __func__);
        return false;
    }

    return true;
}

static bool construct_disable_switch(Parcel& p)
{
    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write args count.", __func__);
        return false;
    }

    if (p.writeInt32(0) != 0) {
        syslog(LOG_ERR, "%s: Failed to write disable.", __func__);
        return false;
    }

    return true;
}

static bool construct_oem_hook_strings(Parcel& p)
{
    const char* str = "AT+REMOTESMS=00110005810180F60000A705E8329BFD06";

    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write args count.", __func__);
        return false;
    }

    writeStringToParcel(p, str);

    return true;
}

static bool construct_dial(Parcel& p)
{
    const char* str = "10086";
    writeStringToParcel(p, str);

    if (p.writeInt32(0) != 0) {
        syslog(LOG_ERR, "%s: Failed to wirte clir.", __func__);
        return false;
    }

    /* skip UUS args. */

    if (p.writeInt32(0) != 0) {
        return false;
    }

    if (p.writeInt32(0) != 0) {
        return false;
    }

    return true;
}

static bool construct_hangup(Parcel& p)
{
    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write args count.", __func__);
        return false;
    }

    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write call_id.", __func__);
        return false;
    }

    return true;
}

static bool construct_emergency_dial(Parcel& p)
{
    const char* str = "911";
    writeStringToParcel(p, str);

    if (p.writeInt32(0) != 0) {
        syslog(LOG_ERR, "%s: Failed to write clir.", __func__);
        return false;
    }

    /* skip UUS args. */

    if (p.writeInt32(0) != 0) {
        return false;
    }

    if (p.writeInt32(0) != 0) {
        return false;
    }

    return true;
}

static bool construct_set_ecc_number(Parcel& p)
{
    const char* str = "122";

    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write number length.", __func__);
        return false;
    }

    writeStringToParcel(p, str);

    if (p.writeInt32(0) != 0) {
        syslog(LOG_ERR, "%s: Failed to wite category.", __func__);
        return false;
    }

    if (p.writeInt32(2) != 0) {
        syslog(LOG_ERR, "%s: Failed to write condition.", __func__);
        return false;
    }

    return true;
}

static bool construct_network_select_manual(Parcel& p)
{
    const char* imsi = "46000";

    writeStringToParcel(p, imsi);

    if (p.writeInt32(7) != 0) {
        syslog(LOG_ERR, "%s: Failed to write registration tech.", __func__);
        return false;
    }

    return true;
}

static bool construct_set_pref_network_type(Parcel& p)
{
    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write args count.", __func__);
        return false;
    }

    if (p.writeInt32(12) != 0) {
        syslog(LOG_ERR, "%s: Failed to write network type.", __func__);
        return false;
    }

    return true;
}

static bool construct_setup_data_call(Parcel& p)
{
    if (p.writeInt32(7) != 0) {
        syslog(LOG_ERR, "%s: Failed to write args count.", __func__);
        return false;
    }

    writeStringToParcel(p, "16");
    writeStringToParcel(p, "0");
    writeStringToParcel(p, "fast.t-mobile.com");
    writeStringToParcel(p, "");
    writeStringToParcel(p, "");
    writeStringToParcel(p, "0");
    writeStringToParcel(p, "IPV4V6");

    return true;
}

static bool construct_deactive_data_call(Parcel& p)
{
    if (p.writeInt32(2) != 0) {
        syslog(LOG_ERR, "%s: Failed to write args count.", __func__);
        return false;
    }

    writeStringToParcel(p, "1");
    writeStringToParcel(p, "0");

    return true;
}

static bool construct_set_data_profile(Parcel& p)
{
    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write length.", __func__);
        return false;
    }

    if (p.writeInt32(0) != 0) {
        syslog(LOG_ERR, "%s: Failed to write profile id.", __func__);
        return false;
    }

    writeStringToParcel(p, "fast.t-mobile.com");
    writeStringToParcel(p, "IPV4V6");

    if (p.writeInt32(2) != 0) {
        syslog(LOG_ERR, "%s: Failed to write auth_method.", __func__);
        return false;
    }

    writeStringToParcel(p, "");
    writeStringToParcel(p, "");

    // fixed value

    if (p.writeInt32(0) != 0) {
        return false;
    }

    if (p.writeInt32(0) != 0) {
        return false;
    }

    if (p.writeInt32(0) != 0) {
        return false;
    }

    if (p.writeInt32(0) != 0) {
        return false;
    }

    if (p.writeInt32(1) != 0) {
        return false;
    }

    return true;
}

static bool construct_get_imsi(Parcel& p)
{
    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write args count.", __func__);
        return false;
    }

    writeStringToParcel(p, NULL);

    return true;
}

static bool construct_SIMIO(Parcel& p)
{
    if (p.writeInt32(192) != 0) {
        syslog(LOG_ERR, "%s: Failed to write CMD_GET_RESPONSE.", __func__);
        return false;
    }

    if (p.writeInt32(12258) != 0) {
        syslog(LOG_ERR, "%s: Failed to write fileid.", __func__);
        return false;
    }

    writeStringToParcel(p, "3f00");

    if (p.writeInt32(0) != 0) {
        syslog(LOG_ERR, "%s: Failed to write P1.", __func__);
        return false;
    }

    if (p.writeInt32(0) != 0) {
        syslog(LOG_ERR, "%s: Failed to write P2.", __func__);
        return false;
    }

    if (p.writeInt32(15) != 0) {
        syslog(LOG_ERR, "%s: Failed to write P3.", __func__);
        return false;
    }

    writeStringToParcel(p, NULL);
    writeStringToParcel(p, NULL);
    writeStringToParcel(p, NULL);

    return true;
}

static bool construct_send_sms(Parcel& p)
{
    if (p.writeInt32(2) != 0) {
        syslog(LOG_ERR, "%s: Failed to write string count.", __func__);
        return false;
    }

    writeStringToParcel(p, NULL);
    writeStringToParcel(p, "110005810180F60000A705E8329BFD06");

    return true;
}

static bool construct_set_callforwarding(Parcel& p)
{
    if (p.writeInt32(3) != 0) {
        syslog(LOG_ERR, "%s: Failed to write registration.", __func__);
        return false;
    }

    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write type.", __func__);
        return false;
    }

    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write cls.", __func__);
        return false;
    }

    if (p.writeInt32(129) != 0) {
        syslog(LOG_ERR, "%s: Failed to write phone number type.", __func__);
        return false;
    }

    writeStringToParcel(p, "13312345678");

    if (p.writeInt32(20) != 0) {
        syslog(LOG_DEBUG, "%s: Failed to write time.", __func__);
        return false;
    }

    return true;
}

static bool construct_cancel_callforwarding(Parcel& p)
{
    if (p.writeInt32(4) != 0) {
        syslog(LOG_ERR, "%s: Failed to write erasure.", __func__);
        return false;
    }

    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write type.", __func__);
        return false;
    }

    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write cls.", __func__);
        return false;
    }

    if (p.writeInt32(0x81) != 0) {
        syslog(LOG_ERR, "%s: Failed to write filled.", __func__);
        return false;
    }

    writeStringToParcel(p, "1234567890");

    if (p.writeInt32(60) != 0) {
        syslog(LOG_ERR, "%s: Failed to write time.", __func__);
        return false;
    }

    return true;
}

static bool construct_transmit_apdu_basic(Parcel& p)
{
    if (p.writeInt32(0) != 0) {
        syslog(LOG_ERR, "%s: Failed to write session id.", __func__);
        return false;
    }

    if (p.writeInt32(160) != 0) {
        syslog(LOG_ERR, "%s: Failed to write cla.", __func__);
        return false;
    }

    if (p.writeInt32(176) != 0) {
        syslog(LOG_ERR, "%s: Failed to write ins.", __func__);
        return false;
    }

    if (p.writeInt32(0) != 0) {
        syslog(LOG_ERR, "%s: Failed to write P1.", __func__);
        return false;
    }

    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write p2.", __func__);
        return false;
    }

    if (p.writeInt32(4) != 0) {
        syslog(LOG_ERR, "%s: Failed to write P3.", __func__);
        return false;
    }

    writeStringToParcel(p, "73656e669000");

    return true;
}

static bool construct_query_callforward_status(Parcel& p)
{
    if (p.writeInt32(2) != 0) {
        syslog(LOG_ERR, "%s: Failed to write Interrogation.", __func__);
        return false;
    }

    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write type.", __func__);
        return false;
    }

    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write cls.", __func__);
        return false;
    }

    if (p.writeInt32(0x81) != 0) {
        syslog(LOG_ERR, "%s: Failed to write filled.", __func__);
        return false;
    }

    writeStringToParcel(p, "1234567890");

    if (p.writeInt32(60) != 0) {
        syslog(LOG_ERR, "%s: Failed to write dummy time.", __func__);
        return false;
    }

    return true;
}

static bool construct_set_callwaiting(Parcel& p)
{
    if (p.writeInt32(2) != 0) {
        syslog(LOG_ERR, "%s: Failed to write args count.", __func__);
        return false;
    }

    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write mode.", __func__);
        return false;
    }

    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write cls.", __func__);
        return false;
    }

    return true;
}

static bool construct_get_callwaiting(Parcel& p)
{
    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write args count.", __func__);
        return false;
    }

    if (p.writeInt32(0) != 0) {
        syslog(LOG_ERR, "%s: Failed to write cls.", __func__);
        return false;
    }

    return true;
}

static bool construct_set_ims_cap(Parcel& p)
{
    if (p.writeInt32(1) != 0) {
        syslog(LOG_ERR, "%s: Failed to write args count.", __func__);
        return false;
    }

    if (p.writeInt32(5) != 0) {
        syslog(LOG_ERR, "%s: Failed to write cap.", __func__);
        return false;
    }

    return true;
}

static const char* requestToString(int request)
{
    switch (request) {
    case RIL_REQUEST_GET_SIM_STATUS:
        return "RIL_REQUEST_GET_SIM_STATUS";
    case RIL_REQUEST_ENTER_SIM_PIN:
        return "RIL_REQUEST_ENTER_SIM_PIN";
    case RIL_REQUEST_ENTER_SIM_PUK:
        return "RIL_REQUEST_ENTER_SIM_PUK";
    case RIL_REQUEST_ENTER_SIM_PIN2:
        return "RIL_REQUEST_ENTER_SIM_PIN2";
    case RIL_REQUEST_ENTER_SIM_PUK2:
        return "RIL_REQUEST_ENTER_SIM_PUK2";
    case RIL_REQUEST_CHANGE_SIM_PIN:
        return "RIL_REQUEST_CHANGE_SIM_PIN";
    case RIL_REQUEST_CHANGE_SIM_PIN2:
        return "RIL_REQUEST_CHANGE_SIM_PIN2";
    case RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION:
        return "RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION";
    case RIL_REQUEST_GET_CURRENT_CALLS:
        return "RIL_REQUEST_GET_CURRENT_CALLS";
    case RIL_REQUEST_DIAL:
        return "RIL_REQUEST_DIAL";
    case RIL_REQUEST_GET_IMSI:
        return "RIL_REQUEST_GET_IMSI";
    case RIL_REQUEST_HANGUP:
        return "RIL_REQUEST_HANGUP";
    case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND:
        return "RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND";
    case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND:
        return "RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND";
    case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
        return "RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE";
    case RIL_REQUEST_CONFERENCE:
        return "RIL_REQUEST_CONFERENCE";
    case RIL_REQUEST_UDUB:
        return "RIL_REQUEST_UDUB";
    case RIL_REQUEST_LAST_CALL_FAIL_CAUSE:
        return "RIL_REQUEST_LAST_CALL_FAIL_CAUSE";
    case RIL_REQUEST_SIGNAL_STRENGTH:
        return "RIL_REQUEST_SIGNAL_STRENGTH";
    case RIL_REQUEST_VOICE_REGISTRATION_STATE:
        return "RIL_REQUEST_VOICE_REGISTRATION_STATE";
    case RIL_REQUEST_DATA_REGISTRATION_STATE:
        return "RIL_REQUEST_DATA_REGISTRATION_STATE";
    case RIL_REQUEST_OPERATOR:
        return "RIL_REQUEST_OPERATOR";
    case RIL_REQUEST_RADIO_POWER:
        return "RIL_REQUEST_RADIO_POWER";
    case RIL_REQUEST_DTMF:
        return "RIL_REQUEST_DTMF";
    case RIL_REQUEST_SEND_SMS:
        return "RIL_REQUEST_SEND_SMS";
    case RIL_REQUEST_SEND_SMS_EXPECT_MORE:
        return "RIL_REQUEST_SEND_SMS_EXPECT_MORE";
    case RIL_REQUEST_SETUP_DATA_CALL:
        return "RIL_REQUEST_SETUP_DATA_CALL";
    case RIL_REQUEST_SIM_IO:
        return "RIL_REQUEST_SIM_IO";
    case RIL_REQUEST_SEND_USSD:
        return "RIL_REQUEST_SEND_USSD";
    case RIL_REQUEST_CANCEL_USSD:
        return "RIL_REQUEST_CANCEL_USSD";
    case RIL_REQUEST_GET_CLIR:
        return "RIL_REQUEST_GET_CLIR";
    case RIL_REQUEST_SET_CLIR:
        return "RIL_REQUEST_SET_CLIR";
    case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS:
        return "RIL_REQUEST_QUERY_CALL_FORWARD_STATUS";
    case RIL_REQUEST_SET_CALL_FORWARD:
        return "RIL_REQUEST_SET_CALL_FORWARD";
    case RIL_REQUEST_QUERY_CALL_WAITING:
        return "RIL_REQUEST_QUERY_CALL_WAITING";
    case RIL_REQUEST_SET_CALL_WAITING:
        return "RIL_REQUEST_SET_CALL_WAITING";
    case RIL_REQUEST_SMS_ACKNOWLEDGE:
        return "RIL_REQUEST_SMS_ACKNOWLEDGE";
    case RIL_REQUEST_GET_IMEI:
        return "RIL_REQUEST_GET_IMEI";
    case RIL_REQUEST_GET_IMEISV:
        return "RIL_REQUEST_GET_IMEISV";
    case RIL_REQUEST_ANSWER:
        return "RIL_REQUEST_ANSWER";
    case RIL_REQUEST_DEACTIVATE_DATA_CALL:
        return "RIL_REQUEST_DEACTIVATE_DATA_CALL";
    case RIL_REQUEST_QUERY_FACILITY_LOCK:
        return "RIL_REQUEST_QUERY_FACILITY_LOCK";
    case RIL_REQUEST_SET_FACILITY_LOCK:
        return "RIL_REQUEST_SET_FACILITY_LOCK";
    case RIL_REQUEST_CHANGE_BARRING_PASSWORD:
        return "RIL_REQUEST_CHANGE_BARRING_PASSWORD";
    case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE:
        return "RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE";
    case RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC:
        return "RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC";
    case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL:
        return "RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL";
    case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS:
        return "RIL_REQUEST_QUERY_AVAILABLE_NETWORKS ";
    case RIL_REQUEST_DTMF_START:
        return "RIL_REQUEST_DTMF_START";
    case RIL_REQUEST_DTMF_STOP:
        return "RIL_REQUEST_DTMF_STOP";
    case RIL_REQUEST_BASEBAND_VERSION:
        return "RIL_REQUEST_BASEBAND_VERSION";
    case RIL_REQUEST_SEPARATE_CONNECTION:
        return "RIL_REQUEST_SEPARATE_CONNECTION";
    case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE:
        return "RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE";
    case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE:
        return "RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE";
    case RIL_REQUEST_GET_NEIGHBORING_CELL_IDS:
        return "RIL_REQUEST_GET_NEIGHBORING_CELL_IDS";
    case RIL_REQUEST_SET_MUTE:
        return "RIL_REQUEST_SET_MUTE";
    case RIL_REQUEST_GET_MUTE:
        return "RIL_REQUEST_GET_MUTE";
    case RIL_REQUEST_QUERY_CLIP:
        return "RIL_REQUEST_QUERY_CLIP";
    case RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE:
        return "RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE";
    case RIL_REQUEST_DATA_CALL_LIST:
        return "RIL_REQUEST_DATA_CALL_LIST";
    case RIL_REQUEST_RESET_RADIO:
        return "RIL_REQUEST_RESET_RADIO";
    case RIL_REQUEST_OEM_HOOK_RAW:
        return "RIL_REQUEST_OEM_HOOK_RAW";
    case RIL_REQUEST_OEM_HOOK_STRINGS:
        return "RIL_REQUEST_OEM_HOOK_STRINGS";
    case RIL_REQUEST_SET_BAND_MODE:
        return "RIL_REQUEST_SET_BAND_MODE";
    case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE:
        return "RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE";
    case RIL_REQUEST_STK_GET_PROFILE:
        return "RIL_REQUEST_STK_GET_PROFILE";
    case RIL_REQUEST_STK_SET_PROFILE:
        return "RIL_REQUEST_STK_SET_PROFILE";
    case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND:
        return "RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND";
    case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE:
        return "RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE";
    case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM:
        return "RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM";
    case RIL_REQUEST_SCREEN_STATE:
        return "RIL_REQUEST_SCREEN_STATE";
    case RIL_REQUEST_EXPLICIT_CALL_TRANSFER:
        return "RIL_REQUEST_EXPLICIT_CALL_TRANSFER";
    case RIL_REQUEST_SET_LOCATION_UPDATES:
        return "RIL_REQUEST_SET_LOCATION_UPDATES";
    case RIL_REQUEST_SET_TTY_MODE:
        return "RIL_REQUEST_SET_TTY_MODE";
    case RIL_REQUEST_QUERY_TTY_MODE:
        return "RIL_REQUEST_QUERY_TTY_MODE";
    case RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG:
        return "RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG";
    case RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG:
        return "RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG";
    case RIL_REQUEST_DEVICE_IDENTITY:
        return "RIL_REQUEST_DEVICE_IDENTITY";
    case RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE:
        return "RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE";
    case RIL_REQUEST_GET_SMSC_ADDRESS:
        return "RIL_REQUEST_GET_SMSC_ADDRESS";
    case RIL_REQUEST_SET_SMSC_ADDRESS:
        return "RIL_REQUEST_SET_SMSC_ADDRESS";
    case RIL_REQUEST_REPORT_SMS_MEMORY_STATUS:
        return "RIL_REQUEST_REPORT_SMS_MEMORY_STATUS";
    case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING:
        return "RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING";
    case RIL_REQUEST_ISIM_AUTHENTICATION:
        return "RIL_REQUEST_ISIM_AUTHENTICATION";
    case RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU:
        return "RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU";
    case RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS:
        return "RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS";
    case RIL_REQUEST_VOICE_RADIO_TECH:
        return "RIL_REQUEST_VOICE_RADIO_TECH";
    case RIL_REQUEST_WRITE_SMS_TO_SIM:
        return "RIL_REQUEST_WRITE_SMS_TO_SIM";
    case RIL_REQUEST_GET_CELL_INFO_LIST:
        return "RIL_REQUEST_GET_CELL_INFO_LIST";
    case RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE:
        return "RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE";
    case RIL_REQUEST_SET_INITIAL_ATTACH_APN:
        return "RIL_REQUEST_SET_INITIAL_ATTACH_APN";
    case RIL_REQUEST_IMS_REGISTRATION_STATE:
        return "RIL_REQUEST_IMS_REGISTRATION_STATE";
    case RIL_REQUEST_IMS_SEND_SMS:
        return "RIL_REQUEST_IMS_SEND_SMS";
    case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC:
        return "RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC";
    case RIL_REQUEST_SIM_OPEN_CHANNEL:
        return "RIL_REQUEST_SIM_OPEN_CHANNEL";
    case RIL_REQUEST_SIM_CLOSE_CHANNEL:
        return "RIL_REQUEST_SIM_CLOSE_CHANNEL";
    case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL:
        return "RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL";
    case RIL_REQUEST_SET_DATA_PROFILE:
        return "RIL_REQUEST_SET_DATA_PROFILE";
    case RIL_REQUEST_GET_ACTIVITY_INFO:
        return "RIL_REQUEST_GET_ACTIVITY_INFO";
    case RIL_REQUEST_GET_MODEM_STATUS:
        return "RIL_REQUEST_GET_MODEM_STATUS";
    case RIL_REQUEST_DEFLECT_CALL:
        return "RIL_REQUEST_DEFLECT_CALL";
    case RIL_REQUEST_EMERGENCY_DIAL:
        return "RIL_REQUEST_EMERGENCY_DIAL";
    case RIL_REQUEST_ENABLE_MODEM:
        return "RIL_REQUEST_ENABLE_MODEM";
    case RIL_REQUEST_IMS_REG_STATE_CHANGE:
        return "RIL_REQUEST_IMS_REG_STATE_CHANGE";
    case RIL_REQUEST_IMS_SET_SERVICE_STATUS:
        return "RIL_REQUEST_IMS_SET_SERVICE_STATUS";
    case RIL_REQUEST_DIAL_CONFERENCE:
        return "RIL_REQUEST_DIAL_CONFERENCE";
    case RIL_REQUEST_SET_EMERGENCY_NUMBER:
        return "RIL_REQUEST_SET_EMERGENCY_NUMBER";
    case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED:
        return "RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED";
    case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED:
        return "RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED";
    case RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED:
        return "RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED";
    case RIL_UNSOL_RESPONSE_NEW_SMS:
        return "RIL_UNSOL_RESPONSE_NEW_SMS";
    case RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT:
        return "RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT";
    case RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM:
        return "RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM";
    case RIL_UNSOL_ON_USSD:
        return "RIL_UNSOL_ON_USSD";
    case RIL_UNSOL_ON_USSD_REQUEST:
        return "RIL_UNSOL_ON_USSD_REQUEST(obsolete)";
    case RIL_UNSOL_NITZ_TIME_RECEIVED:
        return "RIL_UNSOL_NITZ_TIME_RECEIVED";
    case RIL_UNSOL_SIGNAL_STRENGTH:
        return "RIL_UNSOL_SIGNAL_STRENGTH";
    case RIL_UNSOL_SUPP_SVC_NOTIFICATION:
        return "RIL_UNSOL_SUPP_SVC_NOTIFICATION";
    case RIL_UNSOL_STK_SESSION_END:
        return "RIL_UNSOL_STK_SESSION_END";
    case RIL_UNSOL_STK_PROACTIVE_COMMAND:
        return "RIL_UNSOL_STK_PROACTIVE_COMMAND";
    case RIL_UNSOL_STK_EVENT_NOTIFY:
        return "RIL_UNSOL_STK_EVENT_NOTIFY";
    case RIL_UNSOL_STK_CALL_SETUP:
        return "RIL_UNSOL_STK_CALL_SETUP";
    case RIL_UNSOL_SIM_SMS_STORAGE_FULL:
        return "RIL_UNSOL_SIM_SMS_STORAGE_FULL";
    case RIL_UNSOL_SIM_REFRESH:
        return "RIL_UNSOL_SIM_REFRESH";
    case RIL_UNSOL_DATA_CALL_LIST_CHANGED:
        return "RIL_UNSOL_DATA_CALL_LIST_CHANGED";
    case RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED:
        return "RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED";
    case RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS:
        return "RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS";
    case RIL_UNSOL_RESTRICTED_STATE_CHANGED:
        return "RIL_UNSOL_RESTRICTED_STATE_CHANGED";
    case RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE:
        return "RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE";
    case RIL_UNSOL_OEM_HOOK_RAW:
        return "RIL_UNSOL_OEM_HOOK_RAW";
    case RIL_UNSOL_RINGBACK_TONE:
        return "RIL_UNSOL_RINGBACK_TONE";
    case RIL_UNSOL_RESEND_INCALL_MUTE:
        return "RIL_UNSOL_RESEND_INCALL_MUTE";
    case RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE:
        return "RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE";
    case RIL_UNSOL_RIL_CONNECTED:
        return "RIL_UNSOL_RIL_CONNECTED";
    case RIL_UNSOL_VOICE_RADIO_TECH_CHANGED:
        return "RIL_UNSOL_VOICE_RADIO_TECH_CHANGED";
    case RIL_UNSOL_CELL_INFO_LIST:
        return "RIL_UNSOL_CELL_INFO_LIST";
    case RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED:
        return "RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED";
    case RIL_UNSOL_MODEM_RESTART:
        return "RIL_UNSOL_MODEM_RESTART";
    case RIL_UNSOL_EMERGENCY_NUMBER_LIST:
        return "RIL_UNSOL_EMERGENCY_NUMBER_LIST";
    default:
        return "<unknown request>";
    }
}