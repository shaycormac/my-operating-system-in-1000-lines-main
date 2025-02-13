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
#define align_up(value,algin)  __builtin_align_up(value,algin);//�� value �������뵽 align �����������align ������ 2 ���ݡ�
#define is_aligned(value,align) __builtin_is_aligned(value,align);// ��� value �Ƿ��� align �ı�����align ������ 2 ���ݡ�
#define offsetof(type,memeber)  __builtin_offsetof(type,member);


// ����ֻ�ǽ���Щ����Ϊ���� __builtin_ ǰ׺�İ汾�ı�����
//�����Ǳ�������clang�������ṩ�����ù��ܣ��ο���clang �ĵ��������������ʵ�����ʣ��Ĳ��֣��������ǲ���Ҫ������
// ÿ������ʹ�õ��� __builtin_ ��ͷ�ĺ����� Clang �ض�����չ�����ú��������μ� Clang ���ú����ͺꡣ
// ��Щ��Ҳ�����ڲ�ʹ�����ú������������ C ����ʵ�֡�offsetof �Ĵ� C ʵ���ر���Ȥ ?
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