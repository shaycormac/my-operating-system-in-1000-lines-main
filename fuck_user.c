#include "user.h"

extern char __stack_top[];

//__attribute__((noreturn)) void exit(void){
 //   for(;;);
//}

void putchar(char ch){
   syscall(SYS_PUTCHAR,ch,0,0);
}

int getchar(void){
    return syscall(SYS_GETCHAR,0,0,0);
}

__attribute__((section(".text.start")))
__attribute__((naked))
void start(void){
    __asm__ __volatile__(
        "mv sp, %[stack_top] \n"
        "call main           \n"
        "call exit           \n"
        :: [stack_top] "r" (__stack_top)
    );
}


/**
 * 执行系统调用
 * 
 * 本函数通过汇编语言指令实现系统调用，将控制权转交给操作系统内核
 * 它使用了RISC-V架构的特定寄存器和ecall指令来执行系统调用
 * 
 * @param sysno 系统调用号，用于指定所需的系统调用操作
 * @param arg0 系统调用的第一个参数
 * @param arg1 系统调用的第二个参数
 * @param arg2 系统调用的第三个参数
 * @return 返回系统调用的结果，具体含义取决于系统调用操作
 * 
 * 注意：本函数直接与硬件交互，使用前请确保理解系统调用的机制和后果
 */
int syscall(int sysno, int arg0, int arg1, int arg2) {
    // 将arg0赋值给a0寄存器，用于传递系统调用的第一个参数
    register int a0 __asm__("a0") = arg0;
    // 将arg1赋值给a1寄存器，用于传递系统调用的第二个参数
    register int a1 __asm__("a1") = arg1;
    // 将arg2赋值给a2寄存器，用于传递系统调用的第三个参数
    register int a2 __asm__("a2") = arg2;
    // 将sysno赋值给a3寄存器，用于传递系统调用号
    register int a3 __asm__("a3") = sysno;

    // 执行ecall指令，触发系统调用异常，将控制权转交给内核
    // ecall指令是RISC-V架构中用于发起系统调用的指令
    // 这里使用了内联汇编，__volatile__关键字确保这段汇编代码不会被编译器优化掉
    // "=r"(a0)表示a0寄存器是输出操作数，"r"(a0)等表示a0等寄存器是输入操作数
    // "memory"表示这段代码可能会访问内存，防止编译器进行某些优化
    __asm__ __volatile__("ecall"
                         : "=r"(a0)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3)
                         : "memory");

    // 返回系统调用的结果，结果保存在a0寄存器中
    return a0;
}

/**
 * 退出程序。
 * 
 * 通过系统调用(SYS_EXIT)来终止程序的执行。该函数不会返回，因此使用noreturn属性进行声明。
 * 在系统调用之后，包含一个无限循环，以防系统调用失败或某些未预见的情况导致函数继续执行。
 * 
 * 参数: 无
 * 返回值: 无
 */
__attribute__((noreturn)) void exit(void) {
    syscall(SYS_EXIT, 0, 0, 0); // 执行系统调用以退出程序
    for (;;); // 以防万一！确保程序不会继续执行
}