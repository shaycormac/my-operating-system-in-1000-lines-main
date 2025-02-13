#pragma once

#include "fuck_common.h"

struct sbiret {
    long error;
    long value;
};

// 定义为宏，这样做的原因是为了正确显示源文件名（__FILE__）和行号（__LINE__）。
// 如果我们将其定义为函数，__FILE__ 和 __LINE__ 将显示 PANIC 被定义的文件名和行号，而不是它被调用的位置。
#define PANIC(fmt, ...)                                                        \
    do {                                                                       \
        printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);  \
        while (1) {}                                                           \
    } while (0)
// 第一个惯用语是 do-while 语句。由于它是 while (0)，这个循环只执行一次。这是定义由多个语句组成的宏的常见方式。
//简单地用 { ...} 封装可能会在与 if 等语句组合时导致意外的行为（参见这个清晰的例子）。
// 另外，注意每行末尾的反斜杠（\）。虽然宏是在多行上定义的，但在展开时换行符会被忽略。

// 第二个惯用语是 ##__VA_ARGS__。这是一个用于定义接受可变数量参数的宏的有用编译器扩展（参考：GCC 文档）。
// 当可变参数为空时，## 会删除前面的 ,。这使得即使只有一个参数，如 PANIC("booted!")，编译也能成功。


// trap_frame 结构体表示在 kernel_entry 中保存的程序状态
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


//READ_CSR 和 WRITE_CSR 宏是用于读写 CSR 寄存器的便捷宏。
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

