/*
 * Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "psa_crypto_client_key_attributes.h"


void psa_crypto_client_translate_key_attributes(struct ts_crypto_key_attributes *proto_attributes,
                            const psa_key_attributes_t *psa_attributes)
{
    proto_attributes->type = psa_get_key_type(psa_attributes);
    proto_attributes->key_bits = psa_get_key_bits(psa_attributes);
    proto_attributes->lifetime = psa_get_key_lifetime(psa_attributes);
    proto_attributes->id = psa_get_key_id(psa_attributes);

    proto_attributes->policy.usage = psa_get_key_usage_flags(psa_attributes);
    proto_attributes->policy.alg = psa_get_key_algorithm(psa_attributes);
 }

/*
 * The key attributes structure used on the client API doesn't
 * contain an dynamically allocated members so resetting it is
 * a nop.
 */
void psa_reset_key_attributes(psa_key_attributes_t *attributes)
{
    /* Nothing to free for client structure */
}
