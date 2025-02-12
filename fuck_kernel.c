// 自己写的测试
#include "fuck_kernel.h"
#include "fuck_common.h"

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,
                       long arg5, long fid, long eid)
{
    // 在每个局部变量声明中使用的 register 和 __asm__("register name") 要求编译器将值
    // 放在指定的寄存器中。这在系统调用调用过程中是一个常见用法
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid;
    register long a7 __asm__("a7") = eid;

    __asm__ __volatile__("ecall"
                         : "=r"(a0), "=r"(a1)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                           "r"(a6), "r"(a7)
                         : "memory");
    return (struct sbiret){.error = a0, .value = a1};
}

void putchar(char c)
{
    sbi_call(c, 0, 0, 0, 0, 0, 0, 1 /* Console Putchar */);
}

void *memset(void *buf, char c, size_t n)
{
    uint8_t *p = (uint8_t *)buf;
    while (n--)
    {
        *p++ = c;
    }
    return buf;
}

void kernel_main(void)
{
    // 测试自定义输出打印
    printf("\n\nHello, %s\n","RISC-V!");
    printf("1 + 2 = %d,%x\n",1+2,0x1234abcd);

    const char *s = "\n\nHello, RISC-V!\n";
    for (size_t i = 0; s[i] !='\0'; i++)
    {
        putchar(s[i]);
    }
    for (;;)
    {
        // 无限循环
        __asm__ __volatile__("wfi");
    }

    
  // 第一个测试结束
  //  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
    // 初始化代码段和数据段
  //  for (;;)
  //  {
        // 无限循环
  //  }
}

__attribute__((section(".text.boot")))
__attribute__((naked)) void
boot(void)
{
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"
        "j kernel_main\n"
        :
        : [stack_top] "r"(__stack_top));
}
