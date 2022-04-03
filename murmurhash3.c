/*
 * Copyright (c) 2022, smartmx <smartmx@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-04-03     smartmx      the first version
 *
 */
/*
 * @Note:
 * This is murmur3 hash algorithm: https://github.com/aappleby/smhasher.
 * But coded with c instead.
 * This repository only accomplish 32bit hash algorithm.
 */
#include "murmurhash3.h"

#define ROTL32(x,y) ((x << y) | (x >> (32 - y)))

uint32_t murmurhash3_caculate32(const void *hash_key, uint32_t len)
{
    const uint8_t *hash_key_ptr = (const uint8_t *)hash_key;
    int nblocks = len / 4;
    uint32_t hash = MURMURHASH3_SEED_VALUE;
    uint32_t data;

    /* body */
    while (nblocks > 0)
    {
        /* get 32bit data */
        data = (hash_key_ptr[0] << 24) | (hash_key_ptr[1] << 16) | (hash_key_ptr[2] << 8) | (hash_key_ptr[3]);

        data *= MURMURHASH3_C1_VALUE;
        data = ROTL32(data, MURMURHASH3_R1_VALUE);
        data *= MURMURHASH3_C2_VALUE;

        hash ^= data;
        hash = ROTL32(hash, MURMURHASH3_R2_VALUE);
        hash = hash * MURMURHASH3_M_VALUE + MURMURHASH3_N_VALUE;

        hash_key_ptr += 4;
        nblocks--;
    }
    /* tail */
    data = 0;
    switch (len & 3)
    {
    case 3:
        data ^= (hash_key_ptr[2] << 16);/* @suppress("No break at end of case") */
    case 2:
        data ^= (hash_key_ptr[1] << 8); /* @suppress("No break at end of case") */
    case 1:
        data ^= hash_key_ptr[0];
        data *= MURMURHASH3_C1_VALUE;
        data = ROTL32(data, MURMURHASH3_R1_VALUE);
        data *= MURMURHASH3_C2_VALUE;
        hash ^= data;
    }

    /* finalization */

    hash ^= len;

    /* Finalization mix - force all bits of a hash block to avalanche */
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;

    return hash;
}
