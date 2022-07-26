/*
 * Copyright (c) 2020, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef UUID_H
#define UUID_H

#include <stddef.h>
#include <stdint.h>

#define UUID_OCTETS_LEN			        (16)
#define UUID_CANONICAL_FORM_LEN			(36)


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Structure for holding an octet representation of a UUID.
 */
struct uuid_octets
{
    uint8_t octets[UUID_OCTETS_LEN];
};

/*
 * Structure for holding an canonical string representation of a UUID.
 */
struct uuid_canonical
{
    char characters[UUID_CANONICAL_FORM_LEN + 1];
};

/*
 * Check if uuid string in canonical form is valid.  Returns the number of
 * valid characters.  This will either be UUID_CANONICAL_FORM_LEN or zero
 * if the string is invalid in some way.
 */
size_t uuid_is_valid(const char *canonical_form);

/*
 * Parses a uuid string in canonical string form, outputing as an array of bytes.
 * Returns the number of characters parsed from the input string.  Returns zero
 * if there is a parsing error.
 */
size_t uuid_parse_to_octets(const char *canonical_form, uint8_t *buf, size_t buf_size);

/*
 * Parses a uuid string in canonical string form but instead of outputting octets
 * in standard byte order, octets from each section of the canonical uuid are
 * reversed.
 */
size_t uuid_parse_to_octets_reversed(const char *canonical_form, uint8_t *buf, size_t buf_size);


#ifdef __cplusplus
}
#endif

#endif /* UUID_H */
