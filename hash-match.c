/*
 * Copyright (c) 2022-2023, smartmx <smartmx@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-04-03     smartmx      the first version
 * 2023-03-02     smartmx      hash_match_group function will return the pointer to index
 *
 */
#include "hash-match.h"

/**
 * check all calculated hash code in the group, compare with hash_code.
 *
 * @param start the hash match group start address.
 * @param end the hash match group end address.
 * @param hash_code the hash code which is need be checked.
 *
 * @return None.
 */
void hash_match_check(const hash_match_t *start, const hash_match_t *end, uint32_t hash_code)
{
    const hash_match_t *check_start = (const hash_match_t *)start;
    HASH_MATCH_PRINTF("check hash_code:'%08x'\n", hash_code);
    while (1)
    {
        if (end <= check_start)
        {
            break;
        }
        if (*(check_start->hash_code) == hash_code)
        {
            HASH_MATCH_PRINTF("hash match check error, hash_code %08x is duplicated!\n", (uint32_t)check_start);
            break;
        }
        check_start++;
    }
}

/**
 * init a hash code group.
 *
 * @param start the hash match group start address.
 * @param end the hash match group end address.
 *
 * @return None.
 */
void hash_match_group_init(const hash_match_t *start, const hash_match_t *end)
{
    const hash_match_t *init_start = (const hash_match_t *)start;
    while (1)
    {
        if (end <= init_start)
        {
            break;
        }
        *(init_start->hash_code) = hash_match_caculate(init_start->hash_key_src, init_start->hash_key_len);
#if HASH_MATCH_INIT_CHECK
        hash_match_check(start, init_start, *(init_start->hash_code));
#endif
        init_start++;
    }

}

/**
 * check all calculated hash code in the group, compare with hash_code.
 *
 * @param start the hash match group start address.
 * @param end the hash match group end address.
 * @param src the hash key source pointer.
 * @param len the length of hash key source.
 * @param (void*) parameter when calling handler.
 *
 * @return None.
 */
void* hash_match_group(const hash_match_t *start, const hash_match_t *end, const void *src, uint32_t len, void *param)
{
    const hash_match_t *find_start = (const hash_match_t *)start;
    uint32_t hash_code;
    hash_code = hash_match_caculate(src, len);
    while (1)
    {
        if (end <= find_start)
        {
            break;
        }
        if ((*(find_start->hash_code) == hash_code) && (len == find_start->hash_key_len))
        {
#if HASH_MATCH_COMPARE_KEY
            if (hash_match_memcmp(src, find_start->hash_key_src, len) == HASH_MATCH_MEMCMP_SAME)
            {
#endif
                if (find_start->handler != NULL)
                {
                    find_start->handler(param);
                }
                return find_start;
#if HASH_MATCH_COMPARE_KEY
            }
#endif
        }
        find_start++;
    }
    return NULL;
}

/**
 * list all hash key info in a hash code group.
 *
 * @param start the hash match group start address.
 * @param end the hash match group end address.
 *
 * @return None.
 */
void hash_match_group_list(const hash_match_t *start, const hash_match_t *end)
{
    const hash_match_t *list_start = (const hash_match_t *)start;
    uint32_t len;
    while (1)
    {
        if (end <= list_start)
        {
            break;
        }
        len = 0;
        HASH_MATCH_PRINTF("hash match key:%02x", list_start->hash_key_src[len]);
        len++;
        while (len < list_start->hash_key_len)
        {
            HASH_MATCH_PRINTF(" %02x", list_start->hash_key_src[len]);
            len++;
        }
        HASH_MATCH_PRINTF("\nhash match key len:%d\n", list_start->hash_key_len);
        HASH_MATCH_PRINTF("hash match hash code:%08x\n", *(list_start->hash_code));
#if HASH_MATCH_SAVE_DESC
        HASH_MATCH_PRINTF("hash match description: %s\n", list_start->hash_desc);
#endif
        list_start++;
    }
}

