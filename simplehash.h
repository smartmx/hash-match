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
#ifndef _SIMPLEHASH_H_
#define _SIMPLEHASH_H_

#include "stdio.h"
#include "stdint.h"

/* The seed value, you can change as you want. */
#define SIMPLEHASH_SEED_VALUE       0x30393235

extern uint32_t simplehash_caculate32(const void *hash_key, uint32_t len);

extern uint32_t simplehash_upper_caculate32(const void *hash_key, uint32_t len);

extern uint8_t simplehash_lower_char_upper_memcmp(const void *src1, const void *src2, uint32_t len);

#endif /* _SIMPLEHASH_H_ */
