// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2014, STMicroelectronics International N.V.
 * All rights reserved.
 */

#include <tee_ta_api.h>
#include <ta_storage.h>

#include "storage.h"

#include <user_ta_wasm_header.h>

/*
 * Trusted Application Entry Points
 */

/* Called each time a new instance is created */
TEE_Result TA_CreateEntryPoint(void)
{
	return TEE_SUCCESS;
}

/* Called each time an instance is destroyed */
void TA_DestroyEntryPoint(void)
{
}

/* Called each time a session is opened */
TEE_Result TA_OpenSessionEntryPoint(uint32_t nParamTypes,
				    TEE_Param pParams[4],
				    void **ppSessionContext)
{
	(void)nParamTypes;
	(void)pParams;
	(void)ppSessionContext;
	return TEE_SUCCESS;
}

/* Called each time a session is closed */
void TA_CloseSessionEntryPoint(void *pSessionContext)
{
	(void)pSessionContext;
}

/* Called when a command is invoked */
TEE_Result TA_InvokeCommandEntryPoint(void *pSessionContext,
				      uint32_t nCommandID, uint32_t nParamTypes,
				      TEE_Param pParams[4])
{
	(void)pSessionContext;

	switch (nCommandID) {
	case TA_STORAGE_CMD_OPEN:
	case TA_STORAGE_CMD_OPEN_ID_IN_SHM:
		return ta_storage_cmd_open(nCommandID, nParamTypes, pParams);

	case TA_STORAGE_CMD_CLOSE:
		return ta_storage_cmd_close(nParamTypes, pParams);

	case TA_STORAGE_CMD_READ:
		return ta_storage_cmd_read(nParamTypes, pParams);

	case TA_STORAGE_CMD_WRITE:
		return ta_storage_cmd_write(nParamTypes, pParams);

	case TA_STORAGE_CMD_CREATE:
	case TA_STORAGE_CMD_CREATE_ID_IN_SHM:
		return ta_storage_cmd_create(nCommandID, nParamTypes, pParams);

	case TA_STORAGE_CMD_CREATE_OVERWRITE:
	case TA_STORAGE_CMD_CREATEOVER_ID_IN_SHM:
		return ta_storage_cmd_create_overwrite(nCommandID, nParamTypes, pParams);

	case TA_STORAGE_CMD_SEEK:
		return ta_storage_cmd_seek(nParamTypes, pParams);

	case TA_STORAGE_CMD_UNLINK:
		return ta_storage_cmd_unlink(nParamTypes, pParams);

	case TA_STORAGE_CMD_RENAME:
	case TA_STORAGE_CMD_RENAME_ID_IN_SHM:
		return ta_storage_cmd_rename(nCommandID, nParamTypes, pParams);

	case TA_STORAGE_CMD_TRUNC:
		return ta_storage_cmd_trunc(nParamTypes, pParams);

	case TA_STORAGE_CMD_ALLOC_ENUM:
		return ta_storage_cmd_alloc_enum(nParamTypes, pParams);

	case TA_STORAGE_CMD_FREE_ENUM:
		return ta_storage_cmd_free_enum(nParamTypes, pParams);

	case TA_STORAGE_CMD_RESET_ENUM:
		return ta_storage_cmd_reset_enum(nParamTypes, pParams);

	case TA_STORAGE_CMD_START_ENUM:
		return ta_storage_cmd_start_enum(nParamTypes, pParams);

	case TA_STORAGE_CMD_NEXT_ENUM:
		return ta_storage_cmd_next_enum(nParamTypes, pParams);

	case TA_STORAGE_CMD_KEY_IN_PERSISTENT:
		return ta_storage_cmd_key_in_persistent(nParamTypes, pParams);

	case TA_STORAGE_CMD_LOOP:
		return ta_storage_cmd_loop(nParamTypes, pParams);

	case TA_STORAGE_CMD_RESTRICT_USAGE:
		return ta_storage_cmd_restrict_usage(nParamTypes, pParams);

	case TA_STORAGE_CMD_ALLOC_OBJ:
		return ta_storage_cmd_alloc_obj(nParamTypes, pParams);

	case TA_STORAGE_CMD_FREE_OBJ:
		return ta_storage_cmd_free_obj(nParamTypes, pParams);

	case TA_STORAGE_CMD_RESET_OBJ:
		return ta_storage_cmd_reset_obj(nParamTypes, pParams);

	case TA_STORAGE_CMD_GET_OBJ_INFO:
		return ta_storage_cmd_get_obj_info(nParamTypes, pParams);

	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}

#if defined(CONFIG_USER_TA_WASM) || defined(CONFIG_USER_TA_ELF)
struct user_ta_head storage2_user_ta_head = {
	.uuid = TA_STORAGE2_UUID,
	.name = "storage2",
	.flags = TA_FLAG_USER_MODE,
	.create_entry_point = TA_CreateEntryPoint,
	.destroy_entry_point = TA_DestroyEntryPoint,
	.open_session_entry_point = TA_OpenSessionEntryPoint,
	.close_session_entry_point = TA_CloseSessionEntryPoint,
	.invoke_command_entry_point = TA_InvokeCommandEntryPoint
};

__attribute__((visibility("default"))) struct user_ta_head *user_ta = &storage2_user_ta_head;
#else
user_ta_register(.uuid = TA_STORAGE2_UUID, .name = "storage2",
		.flags = TA_FLAG_USER_MODE,
		.create_entry_point = TA_CreateEntryPoint,
		.destroy_entry_point = TA_DestroyEntryPoint,
		.open_session_entry_point = TA_OpenSessionEntryPoint,
		.close_session_entry_point = TA_CloseSessionEntryPoint,
		.invoke_command_entry_point = TA_InvokeCommandEntryPoint);
#endif