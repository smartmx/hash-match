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
#ifndef _HASH_MACTH_H_
#define _HASH_MACTH_H_

#include "stdio.h"
#include "stdint.h"
#include "murmurhash3.h"

/*
 * @Note: hash-match use murmur3 hash algorithm in default: https://github.com/aappleby/smhasher.
 *        you can use your own hash algorithm by change the definition "hash_match_caculate".
 */
#define hash_match_caculate         murmurhash3_caculate32

/* whether save description of hash source or not, set 0 will not save description. */
#define HASH_MATCH_SAVE_DESC        0

/* set HASH_MATCH_INIT_CHECK to 1 will check all hash values in one group during init a group, report if some hash value is duplicated. */
#define HASH_MATCH_INIT_CHECK       0

/* change to your own printf function, or don't use it. */
#define HASH_MATCH_PRINTF           printf

/* whether compare key when hash_code is same. */
#define HASH_MATCH_COMPARE_KEY      1

/* use string.h or self functions to compare key. */
#define HASH_MATCH_USE_STRING_H     1

#if HASH_MATCH_USE_STRING_H
    #include "string.h"
    #define hash_match_memcmp       memcmp
    #define HASH_MATCH_MEMCMP_SAME  0
#else
    #define hash_match_memcmp       murmurhash3_lower_char_upper_memcmp
    #define HASH_MATCH_MEMCMP_SAME  0
#endif

typedef void (*hash_match_handler)(void *);

typedef struct _hash_match_struct
{
    const uint8_t       *hash_key_src;
    uint32_t            hash_key_len;
    uint32_t            *hash_code;
    hash_match_handler  handler;
#if HASH_MATCH_SAVE_DESC
    const char          *hash_desc;
#endif
} hash_match_t;

#if HASH_MATCH_SAVE_DESC
#define HASH_MATCH_EXPORT(GROUP, NAME, hash_key_src, hash_key_len, handler, hash_desc)      \
    static const char NAME##_hash_desc[] = hash_desc;                                       \
    static uint32_t NAME##_hash_code = 0;                                                   \
    HASH_MATCH_USED const hash_match_t NAME HASH_MATCH_SECTION(#GROUP)=                     \
    {                                                                                       \
        hash_key_src,                                                                       \
        hash_key_len,                                                                       \
        &NAME##_hash_code,                                                                  \
        &handler,                                                                           \
        NAME##_hash_desc,                                                                   \
    }
#else
/* use va_args to adapt from codes which has HASH_MATCH_SAVE_DESC enabled. */
#define HASH_MATCH_EXPORT(GROUP, NAME, hash_key_src, hash_key_len, handler, ...)            \
    static uint32_t NAME##_hash_code = 0;                                                   \
    HASH_MATCH_USED const hash_match_t NAME HASH_MATCH_SECTION(#GROUP)=                     \
    {                                                                                       \
        hash_key_src,                                                                       \
        hash_key_len,                                                                       \
        &NAME##_hash_code,                                                                  \
        &handler,                                                                           \
    }
#endif

/* we do not use these functions directly, but use macro definitions functions instead. */
extern void hash_match_group_init(const hash_match_t *start, const hash_match_t *end);

extern void hash_match_group(const hash_match_t *start, const hash_match_t *end, const void *src, uint32_t len, void *param);

extern void hash_match_group_list(const hash_match_t *start, const hash_match_t *end);

/* Compiler Related Definitions */
#if defined(__CC_ARM) || defined(__CLANG_ARM)           /* ARM Compiler */

#define HASH_MATCH_SECTION(x)                   __attribute__((section(x)))
#define HASH_MATCH_USED                         __attribute__((used))

#define HASH_MATCH_INIT(GROUP)  do          \
    {                                       \
        extern const int GROUP##$$Base;     \
        extern const int GROUP##$$Limit;    \
        hash_match_group_init((const hash_match_t*)&GROUP##$$Base, (const hash_match_t*)&GROUP##$$Limit);   \
    } while(0)

#define HASH_MATCH(GROUP, SRC, LEN, PARAMS)    do       \
    {                                                   \
        extern const int GROUP##$$Base;                 \
        extern const int GROUP##$$Limit;                \
        hash_match_group((const hash_match_t*)&GROUP##$$Base, (const hash_match_t*)&GROUP##$$Limit, SRC, LEN, PARAMS); \
    } while(0)

#define HASH_MATCH_LIST(GROUP)  do          \
    {                                       \
        extern const int GROUP##$$Base;     \
        extern const int GROUP##$$Limit;    \
        hash_match_group_list((const hash_match_t*)&GROUP##$$Base, (const hash_match_t*)&GROUP##$$Limit);  \
    } while(0)

#elif defined (__IAR_SYSTEMS_ICC__)                     /* for IAR Compiler */

#define HASH_MATCH_SECTION(x)                   @ x
#define HASH_MATCH_USED                         __root

#define HASH_MATCH_INIT(GROUP)  do  \
    {                               \
        hash_match_group_init((const hash_match_t*)(__section_begin(#GROUP)), (const hash_match_t*)(__section_end(#GROUP)));  \
    } while(0)

#define HASH_MATCH(GROUP, SRC, LEN, PARAMS)    do   \
    {                                               \
        hash_match_group((const hash_match_t*)(__section_begin(#GROUP)), (const hash_match_t*)(__section_end(#GROUP)), SRC, LEN, PARAMS); \
    } while(0)

#define HASH_MATCH_LIST(GROUP)  do              \
    {                                           \
        hash_match_group_list((const hash_match_t*)(__section_begin(#GROUP)), (const hash_match_t*)(__section_end(#GROUP)));  \
    } while(0)

#elif defined (__GNUC__)                                /* GNU GCC Compiler */

#define HASH_MATCH_SECTION(x)                       __attribute__((section(x)))
#define HASH_MATCH_USED                             __attribute__((used))

#define HASH_MATCH_INIT(GROUP)  do      \
    {                                   \
        extern const int GROUP##_start; \
        extern const int GROUP##_end;   \
        hash_match_group_init((const hash_match_t*)&GROUP##_start, (const hash_match_t*)&GROUP##_end);  \
    } while(0)

#define HASH_MATCH(GROUP, SRC, LEN, PARAMS)    do   \
    {                                               \
        extern const int GROUP##_start;             \
        extern const int GROUP##_end;               \
        hash_match_group((const hash_match_t*)&GROUP##_start, (const hash_match_t*)&GROUP##_end, SRC, LEN, PARAMS);    \
    } while(0)

#define HASH_MATCH_LIST(GROUP)  do          \
    {                                       \
        extern const int GROUP##_start;     \
        extern const int GROUP##_end;       \
        hash_match_group_list((const hash_match_t*)&GROUP##_start, (const hash_match_t*)&GROUP##_end);  \
    } while(0)
#else
#error not supported tool chain
#endif

#endif
