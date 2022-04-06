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
#include "hash-match.h"
#include <rtthread.h>

/* test1 */
void hash_match_test1func(void *t)
{
    rt_kprintf("test1\n");
}
const uint8_t hash_match_test1key[] = {'a', 'b', 'c', 'd', 'e', 'f', 232, 'g', 168, 192};
/* test upper and lower compare. */
const uint8_t hash_match_test1keyu[] = {'A', 'B', 'C', 'd', 'e', 'f', 232, 'g', 168, 192};
HASH_MATCH_EXPORT(hash_match_test, hash_match_test1, hash_match_test1key, sizeof(hash_match_test1key), hash_match_test1func, "this is test 1");

/* different section group with same key, for test. */
void hash_match_test1func1(void *t)
{
    rt_kprintf("test11\n");
}
HASH_MATCH_EXPORT(hash_match_test1, hash_match_test11, hash_match_test1key, sizeof(hash_match_test1key), hash_match_test1func1, "this is test 11");

/* test2 */
void hash_match_test2func(void *t)
{
    rt_kprintf("test2\n");
}
const uint8_t hash_match_test2key[] = {51, 135, 10, 0, 33, 67, 45, 123, 172, 8, 0};
HASH_MATCH_EXPORT(hash_match_test, hash_match_test2, hash_match_test2key, sizeof(hash_match_test2key), hash_match_test2func, "this is test 2");

/* different section group with same key, for test. */
void hash_match_test2func1(void *t)
{
    rt_kprintf("test21\n");
}
HASH_MATCH_EXPORT(hash_match_test1, hash_match_test21, hash_match_test2key, sizeof(hash_match_test2key), hash_match_test2func1, "this is test 21");

/* test3 */
void hash_match_test3func(void *t)
{
    rt_kprintf("test3\n");
}
const uint8_t hash_match_test3key[] = {8, 99, 23, 170, 234, 7, 212, 65, 20, 88, 19, 122};
HASH_MATCH_EXPORT(hash_match_test, hash_match_test3, hash_match_test3key, sizeof(hash_match_test3key), hash_match_test3func, "this is test 3");

/* different section group with same key, for test. */
void hash_match_test3func1(void *t)
{
    rt_kprintf("test31\n");
}
HASH_MATCH_EXPORT(hash_match_test1, hash_match_test31, hash_match_test3key, sizeof(hash_match_test3key), hash_match_test3func1, "this is test 31");

static void hash_match_test_task(void *arg)
{
    /* init hash_match_test section group. */
    HASH_MATCH_INIT(hash_match_test);
    HASH_MATCH_LIST(hash_match_test);

    /* init hash_match_test1 section group. */
    HASH_MATCH_INIT(hash_match_test1);
    HASH_MATCH_LIST(hash_match_test1);
    while (1)
    {
        HASH_MATCH(hash_match_test, hash_match_test1key, sizeof(hash_match_test1key), NULL);
        HASH_MATCH(hash_match_test, hash_match_test2key, sizeof(hash_match_test2key), NULL);
        HASH_MATCH(hash_match_test, hash_match_test3key, sizeof(hash_match_test3key), NULL);
        /* test upper and lower compare. */
        HASH_MATCH(hash_match_test, hash_match_test1keyu, sizeof(hash_match_test1keyu), NULL);
        /* mix up the length, to try. */
        HASH_MATCH(hash_match_test, hash_match_test1key, sizeof(hash_match_test2key), NULL);
        HASH_MATCH(hash_match_test, hash_match_test2key, sizeof(hash_match_test3key), NULL);
        HASH_MATCH(hash_match_test, hash_match_test3key, sizeof(hash_match_test1key), NULL);
        /* try with different section group. */
        HASH_MATCH(hash_match_test1, hash_match_test1key, sizeof(hash_match_test1key), NULL);
        HASH_MATCH(hash_match_test1, hash_match_test2key, sizeof(hash_match_test2key), NULL);
        HASH_MATCH(hash_match_test1, hash_match_test3key, sizeof(hash_match_test3key), NULL);
        rt_thread_mdelay(1000);
    }
}

int hash_match_test_main(void)
{
    rt_thread_t tid = RT_NULL;

    /* Create background ticks thread */
    tid = rt_thread_create("hash-match", hash_match_test_task, RT_NULL, 1024, 10, 10);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }

    return 0;
}

INIT_APP_EXPORT(hash_match_test_main);
