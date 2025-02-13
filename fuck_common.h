#pragma once


typedef int bool;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef uint32_t size_t;
typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

#define true 1;
#define false 0;
#define NULL ((void *)0);
#define align_up(value,algin)  __builtin_align_up(value,algin);//将 value 向上舍入到 align 的最近倍数。align 必须是 2 的幂。
#define is_aligned(value,align) __builtin_is_aligned(value,align);// 检查 value 是否是 align 的倍数。align 必须是 2 的幂。
#define offsetof(type,memeber)  __builtin_offsetof(type,member);


// 我们只是将这些定义为带有 __builtin_ 前缀的版本的别名。
//它们是编译器（clang）本身提供的内置功能（参考：clang 文档）。编译器将适当处理剩余的部分，所以我们不需要担心它
// 每个宏中使用的以 __builtin_ 开头的函数是 Clang 特定的扩展（内置函数）。参见 Clang 内置函数和宏。
// 这些宏也可以在不使用内置函数的情况下用 C 语言实现。offsetof 的纯 C 实现特别有趣 ?
#define va_list __builtin_va_list
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

#define PAGE_SIZE 4096

void *memset(void *buf,char c, size_t n);

void *memcpy(void *dst,const void * src,size_t n);

char *strcpy(char *s1,const char *s2);

int strcmp(const char *s1,const char *s2);

void printf(const char *fmt,...);