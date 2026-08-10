/****************************************************************************
 * apps/crypto/mbedtls/source/psa_crypto_se_alt.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <psa_crypto_se_alt.h>
#include <psa/crypto.h>
#include <psa/crypto_se_driver.h>

#include <dev_alt.h>

#define KEYID_SIZE 4U

static psa_status_t psa_alt_generate_key(
                      FAR psa_drv_se_context_t *drv_context,
                      psa_key_slot_number_t key_slot,
                      FAR const psa_key_attributes_t *attributes,
                      FAR uint8_t *pubkey,
                      size_t pubkey_size,
                      FAR size_t *pubkey_length)
{
  cryptodev_context_t ctx;
  psa_key_type_t type;
  int op;
  int ret;

  type = psa_get_key_type(attributes);
  switch (type)
    {
      case PSA_KEY_TYPE_AES:
        op = CRK_GENERATE_AES_KEY;
        break;
      case PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1):
        if (psa_get_key_bits(attributes) == 256)
          {
            op = CRK_GENERATE_SECP256R1_KEY;
          }
        else
          {
            return PSA_ERROR_NOT_SUPPORTED;
          }

        break;
      case PSA_KEY_TYPE_RSA_KEY_PAIR:
        if (psa_get_key_bits(attributes) == 1024)
          {
            op = CRK_GENERATE_RSA_KEY;
          }
        else
          {
            return PSA_ERROR_NOT_SUPPORTED;
          }

        break;
      default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

  ret = cryptodev_init(&ctx);
  if (ret < 0)
    {
      return ret;
    }

  ctx.cryptk.crk_op = op;
  ctx.cryptk.crk_iparams = 1;
  ctx.cryptk.crk_oparams = 0;
  ctx.cryptk.crk_param[0].crp_p = (caddr_t)&key_slot;
  ctx.cryptk.crk_param[0].crp_nbits = KEYID_SIZE * 8;
  ret = cryptodev_cryptk(&ctx);
  if (ret < 0)
    {
      cryptodev_free(&ctx);
      return ret;
    }

  ret = ctx.cryptk.crk_status;
  cryptodev_free(&ctx);
  return ret;
}

psa_status_t psa_alt_allocate_key(FAR psa_drv_se_context_t *drv_context,
                                  FAR void *persistent_data,
                                  FAR const psa_key_attributes_t *attributes,
                                  psa_key_creation_method_t method,
                                  FAR psa_key_slot_number_t *key_slot)
{
  cryptodev_context_t ctx;
  int ret;

  ret = cryptodev_init(&ctx);
  if (ret < 0)
    {
      return ret;
    }

  ctx.cryptk.crk_op = CRK_ALLOCATE_KEY;
  ctx.cryptk.crk_iparams = 0;
  ctx.cryptk.crk_oparams = 1;
  ctx.cryptk.crk_param[0].crp_p = (caddr_t)key_slot;
  ctx.cryptk.crk_param[0].crp_nbits = KEYID_SIZE * 8;
  ret = cryptodev_cryptk(&ctx);
  if (ret < 0)
    {
      cryptodev_free(&ctx);
      return ret;
    }

  ret = ctx.cryptk.crk_status;
  cryptodev_free(&ctx);
  return ret;
}

static psa_status_t psa_alt_export_key(FAR psa_drv_se_context_t *drv_context,
                                       psa_key_slot_number_t key,
                                       FAR uint8_t *p_data,
                                       size_t data_size,
                                       FAR size_t *p_data_length)
{
  cryptodev_context_t ctx;
  int ret;

  ret = cryptodev_init(&ctx);
  if (ret < 0)
    {
      return ret;
    }

  ctx.cryptk.crk_op = CRK_EXPORT_KEY;
  ctx.cryptk.crk_iparams = 1;
  ctx.cryptk.crk_oparams = 1;
  ctx.cryptk.crk_param[0].crp_p = (caddr_t)&key;
  ctx.cryptk.crk_param[0].crp_nbits = KEYID_SIZE * 8;
  ctx.cryptk.crk_param[1].crp_p = (caddr_t)p_data;
  ctx.cryptk.crk_param[1].crp_nbits = data_size * 8;
  ret = cryptodev_cryptk(&ctx);
  if (ret < 0)
    {
      cryptodev_free(&ctx);
      return ret;
    }

  ret = ctx.cryptk.crk_status;
  cryptodev_free(&ctx);
  return ret;
}

static psa_status_t psa_alt_export_public_key(
                      FAR psa_drv_se_context_t *drv_context,
                      psa_key_slot_number_t key,
                      FAR uint8_t *p_data,
                      size_t data_size,
                      FAR size_t *p_data_length)
{
  cryptodev_context_t ctx;
  int ret;

  ret = cryptodev_init(&ctx);
  if (ret < 0)
    {
      return ret;
    }

  ctx.cryptk.crk_op = CRK_EXPORT_PUBLIC_KEY;
  ctx.cryptk.crk_iparams = 1;
  ctx.cryptk.crk_oparams = 1;
  ctx.cryptk.crk_param[0].crp_p = (caddr_t)&key;
  ctx.cryptk.crk_param[0].crp_nbits = KEYID_SIZE * 8;
  ctx.cryptk.crk_param[1].crp_p = (caddr_t)p_data;
  ctx.cryptk.crk_param[1].crp_nbits = data_size * 8;
  ret = cryptodev_cryptk(&ctx);
  if (ret < 0)
    {
      cryptodev_free(&ctx);
      return ret;
    }

  ret = ctx.cryptk.crk_status;
  cryptodev_free(&ctx);
  return ret;
}

psa_status_t psa_alt_destroy_key(FAR psa_drv_se_context_t *drv_context,
                                 FAR void *persistent_data,
                                 psa_key_slot_number_t key_slot)
{
  cryptodev_context_t ctx;
  int ret;

  ret = cryptodev_init(&ctx);
  if (ret < 0)
    {
      return ret;
    }

  ctx.cryptk.crk_op = CRK_DELETE_KEY;
  ctx.cryptk.crk_iparams = 1;
  ctx.cryptk.crk_oparams = 0;
  ctx.cryptk.crk_param[0].crp_p = (caddr_t)&key_slot;
  ctx.cryptk.crk_param[0].crp_nbits = KEYID_SIZE * 8;
  ret = cryptodev_cryptk(&ctx);
  if (ret < 0)
    {
      cryptodev_free(&ctx);
      return ret;
    }

  ret = ctx.cryptk.crk_status;
  cryptodev_free(&ctx);
  return ret;
}

static psa_status_t psa_alt_import_key(
                      FAR psa_drv_se_context_t *drv_context,
                      psa_key_slot_number_t key_slot,
                      FAR const psa_key_attributes_t *attributes,
                      FAR const uint8_t *data,
                      size_t data_length,
                      FAR size_t *bits)
{
  cryptodev_context_t ctx;
  int ret;

  ret = cryptodev_init(&ctx);
  if (ret < 0)
    {
      return ret;
    }

  ctx.cryptk.crk_op = CRK_IMPORT_KEY;
  ctx.cryptk.crk_iparams = 2;
  ctx.cryptk.crk_oparams = 0;
  ctx.cryptk.crk_param[0].crp_p = (caddr_t)&key_slot;
  ctx.cryptk.crk_param[0].crp_nbits = KEYID_SIZE * 8;
  ctx.cryptk.crk_param[1].crp_p = (caddr_t)data;
  ctx.cryptk.crk_param[1].crp_nbits = data_length * 8;
  ret = cryptodev_cryptk(&ctx);
  if (ret < 0)
    {
      cryptodev_free(&ctx);
      return ret;
    }

  ret = ctx.cryptk.crk_status;
  cryptodev_free(&ctx);
  return ret;
}

static psa_status_t psa_alt_validate_slot_number(
                      FAR psa_drv_se_context_t *drv_context,
                      FAR void *persistent_data,
                      FAR const psa_key_attributes_t *attributes,
                      psa_key_creation_method_t method,
                      psa_key_slot_number_t key_slot)
{
  cryptodev_context_t ctx;
  int ret;

  ret = cryptodev_init(&ctx);
  if (ret < 0)
    {
      return ret;
    }

  ctx.cryptk.crk_op = CRK_VALIDATE_KEYID;
  ctx.cryptk.crk_iparams = 1;
  ctx.cryptk.crk_oparams = 0;
  ctx.cryptk.crk_param[0].crp_p = (caddr_t)&key_slot;
  ctx.cryptk.crk_param[0].crp_nbits = KEYID_SIZE * 8;
  ret = cryptodev_cryptk(&ctx);
  if (ret < 0)
    {
      cryptodev_free(&ctx);
      return ret;
    }

  ret = ctx.cryptk.crk_status;
  cryptodev_free(&ctx);
  return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

psa_status_t psa_alt_register_se_driver(void)
{
  psa_drv_se_t *ctx;
  psa_drv_se_key_management_t *key_management;

  ctx = (psa_drv_se_t *)malloc(sizeof(psa_drv_se_t));
  if (ctx == NULL)
    {
      return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

  memset(ctx, 0, sizeof(psa_drv_se_t));
  ctx->hal_version = PSA_DRV_SE_HAL_VERSION;
  key_management =
    (psa_drv_se_key_management_t *)
      malloc(sizeof(psa_drv_se_key_management_t));
  if (key_management == NULL)
    {
      free(ctx);
      return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

  memset(key_management, 0, sizeof(psa_drv_se_key_management_t));
  key_management->p_generate             = psa_alt_generate_key;
  key_management->p_allocate             = psa_alt_allocate_key;
  key_management->p_export               = psa_alt_export_key;
  key_management->p_export_public        = psa_alt_export_public_key;
  key_management->p_destroy              = psa_alt_destroy_key;
  key_management->p_import               = psa_alt_import_key;
  key_management->p_validate_slot_number = psa_alt_validate_slot_number;
  ctx->key_management = key_management;

  return psa_register_se_driver(PSA_KEY_LOCATION_LOCAL_CRYPTODEV, ctx);
}
