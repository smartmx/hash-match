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
#ifndef _MURMURHASH3_H_
#define _MURMURHASH3_H_

#include "stdio.h"
#include "stdint.h"

/* The seed value, you can change as you want. */
#define MURMURHASH3_SEED_VALUE       0x31313137

/* The hash calculate parameters value, don't change it unless it's necessary. */
#define MURMURHASH3_C1_VALUE         0xcc9e2d51
#define MURMURHASH3_C2_VALUE         0x1b873593
#define MURMURHASH3_R1_VALUE         15
#define MURMURHASH3_R2_VALUE         13
#define MURMURHASH3_M_VALUE          5
#define MURMURHASH3_N_VALUE          0xe6546b64

extern uint32_t murmurhash3_caculate32(const void *hash_key, uint32_t len);

#endif /* _MURMURHASH3_H_ */
