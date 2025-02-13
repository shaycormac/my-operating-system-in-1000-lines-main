#pragma once

#include "fuck_common.h"

struct sbiret {
    long error;
    long value;
};

// ����Ϊ�꣬��������ԭ����Ϊ����ȷ��ʾԴ�ļ�����__FILE__�����кţ�__LINE__����
// ������ǽ��䶨��Ϊ������__FILE__ �� __LINE__ ����ʾ PANIC ��������ļ������кţ��������������õ�λ�á�
#define PANIC(fmt, ...)                                                        \
    do {                                                                       \
        printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);  \
        while (1) {}                                                           \
    } while (0)
// ��һ���������� do-while ��䡣�������� while (0)�����ѭ��ִֻ��һ�Ρ����Ƕ����ɶ�������ɵĺ�ĳ�����ʽ��
//�򵥵��� { ...} ��װ���ܻ����� if ��������ʱ�����������Ϊ���μ�������������ӣ���
// ���⣬ע��ÿ��ĩβ�ķ�б�ܣ�\������Ȼ�����ڶ����϶���ģ�����չ��ʱ���з��ᱻ���ԡ�

// �ڶ����������� ##__VA_ARGS__������һ�����ڶ�����ܿɱ����������ĺ�����ñ�������չ���ο���GCC �ĵ�����
// ���ɱ����Ϊ��ʱ��## ��ɾ��ǰ��� ,����ʹ�ü�ʹֻ��һ���������� PANIC("booted!")������Ҳ�ܳɹ���


// trap_frame �ṹ���ʾ�� kernel_entry �б���ĳ���״̬
struct trap_frame
{
    uint32_t ra;
    uint32_t gp;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t s0;
    uint32_t s1;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t sp;
} __attribute__((packed));


//READ_CSR �� WRITE_CSR �������ڶ�д CSR �Ĵ����ı�ݺꡣ
#define READ_CSR(reg) \
({ \
    unsigned long __tmp; \
    __asm__ __volatile__("csrr %0, " #reg : "=r"(__tmp)); \
    __tmp; \
})


#define WRITE_CSR(reg, value)                                                  \
    do {                                                                       \
        uint32_t __tmp = (value);                                              \
        __asm__ __volatile__("csrw " #reg ", %0" ::"r"(__tmp));                \
    } while (0)

