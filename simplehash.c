/*
 * Copyright (c) 2023, smartmx <smartmx@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-03-03     smartmx      the first version
 *
 */
#include "simplehash.h"

/**
 * calculate hash_code.
 *
 * @param hash_key the hash_key start address.
 * @param len the length of the hash_key.
 *
 * @return type uint32_t, the result of calculated value.
 */
uint32_t simplehash_caculate32(const void *hash_key, uint32_t len)
{
    const uint8_t *hash_key_ptr = (const uint8_t *)hash_key;
    uint32_t clen, data;
    uint32_t hash = 0;
    uint32_t seed = SIMPLEHASH_SEED_VALUE;

    /* body */
    for (clen = 0; clen < len; clen++)
    {
        data = *hash_key_ptr;
        hash = (hash ^ seed) + data;
        hash_key_ptr++;
    }

    return hash;
}

/**
 * upper all lower letters in hash key and calculate the hash_code.
 *
 * @param c the char value.
 *
 * @return type uint8_t, the upper char value.
 */
static uint8_t simplehash_lower_char_upper(uint8_t c)
{
    if ((c >= 'a') && (c <= 'z'))
        return c + ('A' - 'a');
    return c;
}

/**
 * upper all lower letters in hash key and calculate the hash_code.
 *
 * @param c the char value.
 *
 * @return type uint8_t, the upper char value.
 */
uint8_t simplehash_lower_char_upper_memcmp(const void *src1, const void *src2, uint32_t len)
{
    uint8_t *src1_char = (uint8_t *)src1;
    uint8_t *src2_char = (uint8_t *)src2;
    uint32_t all_len = len;
    while (all_len)
    {
        if (simplehash_lower_char_upper(*src1_char) == simplehash_lower_char_upper(*src2_char))
        {
            src1_char++;
            src2_char++;
            all_len--;
        }
        else
        {
            return (len - all_len);
        }

    }
    return 0;
}

/**
 * upper all lower letters in hash key and calculate the hash_code.
 *
 * @param hash_key the hash_key start address.
 * @param len the length of the hash_key.
 *
 * @return type uint32_t, the result of calculated value.
 */
uint32_t simplehash_upper_caculate32(const void *hash_key, uint32_t len)
{
    const uint8_t *hash_key_ptr = (const uint8_t *)hash_key;
    uint32_t clen, data;
    uint32_t hash = 0;
    uint32_t seed = SIMPLEHASH_SEED_VALUE;

    /* body */
    for (clen = 0; clen < len; clen++)
    {
        data = simplehash_lower_char_upper(*hash_key_ptr);
        hash = (hash ^ seed) + data;
        hash_key_ptr++;
    }

    return hash;
}

