# hash-match

哈希匹配算法在单片机上的应用

## hash-match设计前言

在单片机日常开发中，总会遇到根据收到的不同数据执行不同的程序，通常都会使用`memcmp`或者`strcmp`进行比较。

在匹配数据情况不多时，的确很合适。

但是一旦工程量变大，运行速度下降明显。比如有200个数据数组等待比较，每次都要比较很多次。

经常可以看到有的工程里`if(memcmp()) {}`后边有上百个`else if(memcmp()) {}`，极度影响代码运行效率。

此时，将key的数组特征化，计算出特有的hash值，来代替比较，就可以给运行速度带来很大的提升。

## HASH算法

hash-match库中提供了两种哈希算法：

### [Murmurhash3](https://github.com/aappleby/smhasher)

MurmurHash是一种非加密函数的哈希函数，它能够产生出32-bit或128-bit哈希值。自称超级快的hash算法，是FNV的4-5倍。

MurmurHash算法由Austin Appleby创建于2008年，现已应用到Hadoop、libstdc 、nginx、libmemcached,Redis，Memcached，Cassandra，HBase，Lucene等开源系统。

用户可以移植其他匹配算法，通过更改`hash_match_caculate`宏定义决定使用哪一种。

### Simplehash

Simplehash是一种非常简单的哈希函数，它能够产生出32-bit哈希值。运行速度很快，更适合低速单片机使用。

## 使用方法

库中头文件参照了rt-thread中针对不同的编译器设置了不同的宏定义，目前支持MDK、IAR和GNU gcc。其他编译器暂未移植，一般来说已经满足使用。如果有其他编译器需求，请自行移植。

使用中只需要2个API函数和一个数据定义宏

```c
HASH_MATCH_EXPORT(GROUP, NAME, hash_key_src, hash_key_len, handler, ...);
```

使用`HASH_MATCH_EXPORT`定义数据参数定义到指定代码段

```c
HASH_MATCH_INIT(GROUP);
```

使用`HASH_MATCH_INIT`将初始化整个组内的哈希值

```c
HASH_MATCH(GROUP, SRC, LEN, PARAMS);
```

使用`HASH_MATCH`将对提供的key进行哈希匹配

### MDK和IAR编译器使用方法

MDK和IAR中都在软件内部进行更改了代码编译后的链接操作，可以很方便的使用，无需修改工程文件。

#### GROUP命名

选择一个GROUP名字，例如hash_match_test

#### 注册命令

使用该名字定义该组下的所有命令，定义应当为全局变量定义，即放在函数外部

```c
HASH_MATCH_EXPORT(hash_match_test, hash_match_test1, hash_match_test1key, sizeof(hash_match_test1key), hash_match_test1func, "this is test 1");
```

#### 初始化GROUP

```c
HASH_MATCH_INIT(hash_match_test);
```

#### 匹配key

```c
HASH_MATCH(hash_match_test, hash_match_test1key, sizeof(hash_match_test1key), NULL);
```

### GUN gcc编译器使用方法

GUN gcc工程需要在工程的ld文件中找到非零变量的初始化链接代码。

hash-match具有分组信息，即使用时的GROUP参数。

在gnu gcc编译器时，通过宏定义展开，每个GROUP定义的const常量都会被放到__attribute__((section("GROUP")))中

例如，应用代码中串口有命令需要处理，将GROUP参数传入为uart，则const常量会被放到__attribute__((section("uart")))中

而gnu gcc编译器并没有在链接代码操作中有自定义的操作

所以根据不同的GROUP，在ld文件的`.text`段中添加代码，本例子将以GROUP参数为hash_match_test为例子

```ld
. = ALIGN(4);
PROVIDE(hash_match_test_start = .);
KEEP(*(hash_match_test))
PROVIDE(hash_match_test_end = .);
. = ALIGN(4);
```

例如，提供的例程中就可以将ld文件修改如下：

```ld
.text :
{
    . = ALIGN(4);
    KEEP(*(SORT_NONE(.handle_reset)))
    *(.text)
    *(.text.*)
    *(.rodata)
    *(.rodata*)
    *(.sdata2.*)
    *(.glue_7)
    *(.glue_7t)
    *(.gnu.linkonce.t.*)
    
    /* this is for GROUP hash_match_test of hash_match library. */
    . = ALIGN(4);
    PROVIDE(hash_match_test_start = .);
    KEEP(*(hash_match_test))
    . = ALIGN(4);
    PROVIDE(hash_match_test_end = .);
    
    /* this is for GROUP hash_match_test1 of hash_match library. */
    . = ALIGN(4);
    PROVIDE(hash_match_test1_start = .);
    KEEP(*(hash_match_test))
    . = ALIGN(4);
    PROVIDE(hash_match_test1_end = .);

    . = ALIGN(4);
} >FLASH AT>FLASH 
```

代码中使用了几个GROUP，就需要在ld中添加几个相关代码，其他步骤和MDK和IAR编译器使用方法一致

## 配置选项

配置选项可以在`hash-match.h`中修改

```c
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
    #define hash_match_memcmp
    #define HASH_MATCH_MEMCMP_SAME
#endif
```

`hash_match_caculate`宏定义决定了`hash-match`采用的哈希算法，移植如需要改为自己的算法，请按照`uint32_t murmurhash3_caculate32(const void *hash_key, uint32_t len);`函数接口实现。

`HASH_MATCH_SAVE_DESC`宏定义决定了是否会保存描述，设置为非0就会启用。

`HASH_MATCH_INIT_CHECK`宏定义决定在初始化时，是否和已经计算得到的其他哈希值进行比较，如果有相同的哈希值，则报错。

`HASH_MATCH_PRINTF`宏定义是在有报错信息时候调用的打印函数。

`HASH_MATCH_COMPARE_KEY`宏定义决定了在哈希值匹配成功后，是否继续匹配数据内容。一般来说，长度一样的数据内容，哈希值一样的概率很小，所以可以根据安全性要求来决定是否继续匹配数据内容。

`HASH_MATCH_USE_STRING_H`宏定义决定了匹配数据内容使用的函数，如果不准备使用，可以改为其他函数。

## 大小写通用匹配

`hash-match.c`中并未直接处理大小写可以通用的转换，需要在hash计算的函数中处理。

目前在`murmurhash3.c`中提供了转换大小写的处理函数，如需使用，需要把配置项改为如下：

```c
#define hash_match_caculate         murmurhash3_upper_caculate32

/* whether save description of hash source or not, set 0 will not save description. */
#define HASH_MATCH_SAVE_DESC        0

/* set HASH_MATCH_INIT_CHECK to 1 will check all hash values in one group during init a group, report if some hash value is duplicated. */
#define HASH_MATCH_INIT_CHECK       0

/* change to your own printf function, or don't use it. */
#define HASH_MATCH_PRINTF           printf

/* whether compare key when hash_code is same. */
#define HASH_MATCH_COMPARE_KEY      1

/* use string.h or self functions to compare key. */
#define HASH_MATCH_USE_STRING_H     0

#if HASH_MATCH_USE_STRING_H
    #include "string.h"
    #define hash_match_memcmp       memcmp
    #define HASH_MATCH_MEMCMP_SAME  0
#else
    #define hash_match_memcmp       murmurhash3_lower_char_upper_memcmp
    #define HASH_MATCH_MEMCMP_SAME  0
#endif
```

`murmurhash3_upper_caculate32`函数会将小写字母转换为大写字母再进行哈希值计算

`murmurhash3_lower_char_upper_memcmp`函数会将小写字母转换为大写字母后再比较是否相同

## 通过宏定义来简化定义过程

例如，你现在打算为每个AT指令创建函数，可以在头文件中增加如下宏定义：

```c
#define _AT_HASH_MATCH_EXPORT(A, B)         static void A##_handler(at_uart_data_t *p);                             \
                                            HASH_MATCH_EXPORT(uart_at_cmd, A, B, (sizeof(B) - 1), A##_handler);     \
                                            static void A##_handler(at_uart_data_t *p)

#define AT_HASH_MATCH_EXPORT(X)             _AT_HASH_MATCH_EXPORT(at_##X, #X)
```

那么就可以在c文件里很快的定义出函数：

```c
/*
 * AT+CONNECT命令，启用连接
 */
AT_HASH_MATCH_EXPORT(connect)
{
    printf("+CONNECT:1" "\r\nOK\r\n");
}
```

上述函数展开后：

```c
/*
 * AT+CONNECT命令，启用连接
 */
static void at_connect_handler(at_uart_data_t *p);
static uint32_t at_connect_hash_code = 0;
__attribute__((used)) const hash_match_t at_connect __attribute__((section("uart_at_cmd"))) =
{
    "connect",
    (sizeof("connect") - 1),
    &at_connect_hash_code,
    (hash_match_handler) &at_connect_handler,
};

static void at_connect_handler(at_uart_data_t *p)
{
    printf("+CONNECT:1" "\r\nOK\r\n");
}
```

## 注意事项

虽然头文件中提供了函数，但是一般不直接使用那些函数，因为需要根据不同编译器有不同的配置，所以此时需要使用宏定义的函数来对不同编译器产生适用性。

库中头文件参照了rt-thread中针对不同的编译器设置了不同的宏定义，目前支持MDK、IAR和GNU gcc。其他编译器暂未移植，一般来说已经满足使用。如果有其他编译器需求，请自行移植。

hash-match没有使用可变参数传递，匹配回调函数只携带了一个参数`void *`。

## 示例程序

rt-thread移植例程：

[examples/hash_match_demo.c](https://github.com/smartmx/hash-match/tree/main/examples/hash_match_demo.c)。

可以通过`RT-Thread menuconfig`方式，将文件添加至工程：

```c
RT-Thread online packages  --->
    tools packages  --->
        [*] hash-match: using hashmap on MCUs  --->
        [*]   Enable hash match demo
              version (latest)  --->
```

## 版权许可

hash-match 遵循 Apache License v2.0 开源协议。

欢迎大家提交PR，并在change log中附上自己的修改日志。

## [博客主页](https://blog.maxiang.vip/)
