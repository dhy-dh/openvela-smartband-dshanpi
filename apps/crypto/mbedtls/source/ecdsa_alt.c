/****************************************************************************
 * apps/crypto/mbedtls/source/ecdsa_alt.c
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

#include <dev_alt.h>
#include <mbedtls/bignum.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/error.h>

#define ECDSA_SECP256R1_KEYLEN 32

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int mbedtls_ecdsa_sign(FAR mbedtls_ecp_group *grp, FAR mbedtls_mpi *r,
                       FAR mbedtls_mpi *s, FAR const mbedtls_mpi *d,
                       FAR const unsigned char *buf, size_t blen,
                       FAR int (*f_rng)(void *, unsigned char *, size_t),
                       FAR void *p_rng)
{
  cryptodev_context_t ctx;
  int ret;

  ret = cryptodev_init(&ctx);
  if (ret != 0)
    {
      return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }

  switch (grp->id)
    {
      case MBEDTLS_ECP_DP_SECP256R1:
        ctx.cryptk.crk_op = CRK_ECDSA_SECP256R1_SIGN;
        mbedtls_mpi_grow(r, ECDSA_SECP256R1_KEYLEN /
                            sizeof(mbedtls_mpi_uint));
        mbedtls_mpi_grow(s, ECDSA_SECP256R1_KEYLEN /
                            sizeof(mbedtls_mpi_uint));
        break;
      default:
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        goto cleanup;
    }

  ctx.cryptk.crk_iparams = 2;
  ctx.cryptk.crk_oparams = 2;
  ctx.cryptk.crk_param[0].crp_p = (caddr_t)d->p;
  ctx.cryptk.crk_param[0].crp_nbits = d->n * sizeof(mbedtls_mpi_uint) * 8;
  ctx.cryptk.crk_param[1].crp_p = (caddr_t)buf;
  ctx.cryptk.crk_param[1].crp_nbits = blen * sizeof(mbedtls_mpi_uint) * 8;
  ctx.cryptk.crk_param[2].crp_p = (caddr_t)r->p;
  ctx.cryptk.crk_param[2].crp_nbits = r->n * sizeof(mbedtls_mpi_uint) * 8;
  ctx.cryptk.crk_param[3].crp_p = (caddr_t)s->p;
  ctx.cryptk.crk_param[3].crp_nbits = s->n * sizeof(mbedtls_mpi_uint) * 8;

  ret = cryptodev_cryptk(&ctx);
  if (ret < 0)
    {
      ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
      goto cleanup;
    }

  ret = ctx.cryptk.crk_status;

cleanup:
  cryptodev_free(&ctx);
  return ret;
}

int mbedtls_ecdsa_verify(FAR mbedtls_ecp_group *grp,
                         FAR const unsigned char *buf, size_t blen,
                         FAR const mbedtls_ecp_point *Q,
                         FAR const mbedtls_mpi *r,
                         FAR const mbedtls_mpi *s)
{
  cryptodev_context_t ctx;
  int ret;

  ret = cryptodev_init(&ctx);
  if (ret != 0)
    {
      return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }

  switch (grp->id)
    {
      case MBEDTLS_ECP_DP_SECP256R1:
        ctx.cryptk.crk_op = CRK_ECDSA_SECP256R1_VERIFY;
        mbedtls_mpi_shrink(r, ECDSA_SECP256R1_KEYLEN /
                              sizeof(mbedtls_mpi_uint));
        mbedtls_mpi_shrink(s, ECDSA_SECP256R1_KEYLEN /
                              sizeof(mbedtls_mpi_uint));
        break;
      default:
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        goto cleanup;
    }

  ctx.cryptk.crk_iparams = 6;
  ctx.cryptk.crk_oparams = 0;
  ctx.cryptk.crk_param[0].crp_p = (caddr_t)Q->X.p;
  ctx.cryptk.crk_param[0].crp_nbits = Q->X.n * sizeof(mbedtls_mpi_uint) * 8;
  ctx.cryptk.crk_param[1].crp_p = (caddr_t)Q->Y.p;
  ctx.cryptk.crk_param[1].crp_nbits = Q->Y.n * sizeof(mbedtls_mpi_uint) * 8;
  ctx.cryptk.crk_param[2].crp_p = (caddr_t)Q->Z.p;
  ctx.cryptk.crk_param[2].crp_nbits = Q->Z.n * sizeof(mbedtls_mpi_uint) * 8;
  ctx.cryptk.crk_param[3].crp_p = (caddr_t)r->p;
  ctx.cryptk.crk_param[3].crp_nbits = r->n * sizeof(mbedtls_mpi_uint) * 8;
  ctx.cryptk.crk_param[4].crp_p = (caddr_t)s->p;
  ctx.cryptk.crk_param[4].crp_nbits = s->n * sizeof(mbedtls_mpi_uint) * 8;
  ctx.cryptk.crk_param[5].crp_p = (caddr_t)buf;
  ctx.cryptk.crk_param[5].crp_nbits = blen * sizeof(mbedtls_mpi_uint) * 8;

  ret = cryptodev_cryptk(&ctx);
  if (ret < 0)
    {
      ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
      goto cleanup;
    }

  ret = ctx.cryptk.crk_status;

cleanup:
  cryptodev_free(&ctx);
  return ret;
}

int mbedtls_ecdsa_genkey(FAR mbedtls_ecdsa_context *ctx,
                         mbedtls_ecp_group_id gid,
                         FAR int (*f_rng)(void *, unsigned char *, size_t),
                         FAR void *p_rng)
{
  cryptodev_context_t dev;
  int ret;

  ret = mbedtls_ecp_group_load(&ctx->grp, gid);
  if (ret != 0)
    {
      return ret;
    }

  ret = cryptodev_init(&dev);
  if (ret != 0)
    {
      return MBEDTLS_ERR_ECP_ALLOC_FAILED;
    }

  switch (gid)
    {
      case MBEDTLS_ECP_DP_SECP256R1:
        dev.cryptk.crk_op = CRK_ECDSA_SECP256R1_GENKEY;
        mbedtls_mpi_grow(&ctx->d, ECDSA_SECP256R1_KEYLEN /
                                  sizeof(mbedtls_mpi_uint));
        mbedtls_mpi_grow(&ctx->Q.X, ECDSA_SECP256R1_KEYLEN /
                                    sizeof(mbedtls_mpi_uint));
        mbedtls_mpi_grow(&ctx->Q.Y, ECDSA_SECP256R1_KEYLEN /
                                    sizeof(mbedtls_mpi_uint));
        break;
      default:
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        goto cleanup;
    }

  dev.cryptk.crk_iparams = 0;
  dev.cryptk.crk_oparams = 4;
  dev.cryptk.crk_param[0].crp_p = (caddr_t)ctx->d.p;
  dev.cryptk.crk_param[0].crp_nbits = ctx->d.n *
                                      sizeof(mbedtls_mpi_uint) * 8;
  dev.cryptk.crk_param[1].crp_p = (caddr_t)ctx->Q.X.p;
  dev.cryptk.crk_param[1].crp_nbits = ctx->Q.X.n *
                                      sizeof(mbedtls_mpi_uint) * 8;
  dev.cryptk.crk_param[2].crp_p = (caddr_t)ctx->Q.Y.p;
  dev.cryptk.crk_param[2].crp_nbits = ctx->Q.Y.n *
                                      sizeof(mbedtls_mpi_uint) * 8;
  dev.cryptk.crk_param[3].crp_p = (caddr_t)ctx->Q.Z.p;
  dev.cryptk.crk_param[3].crp_nbits = ctx->Q.Z.n *
                                      sizeof(mbedtls_mpi_uint) * 8;

  ret = cryptodev_cryptk(&dev);
  if (ret < 0)
    {
      ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
      goto cleanup;
    }

  ret = dev.cryptk.crk_status;

cleanup:
  cryptodev_free(&dev);
  return ret;
}
