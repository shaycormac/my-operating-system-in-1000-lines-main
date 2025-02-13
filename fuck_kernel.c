// 自己写的测试
#include "fuck_kernel.h"
#include "fuck_common.h"

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];

extern char __free_ram[], __free_ram_end[];

// 定义进程
#define PROCS_MAX 8
#define PROC_UNUSED 0 // 未使用的进程控制结构
#define PROC_RUNNABLE 1 // 可运行的进程
/**
 * 内核栈包含保存的 CPU 寄存器、返回地址（从何处调用）和局部变量。
 * 通过为每个进程准备一个内核栈，我们可以通过保存和恢复 CPU 寄存器并切换栈指针来实现上下文切换。
 */
struct process{
    int pid;// 进程 ID
    int state;// 进程状态: PROC_UNUSED 或 PROC_RUNNABLE
    vaddr_t sp;// 栈指针
    uint8_t stack[8192];// 内核栈
};

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

// 上下文切换
__attribute__((naked)) void switch_context(uint32_t *prev_sp,
    uint32_t *next_sp) {
__asm__ __volatile__(
// 将被调用者保存寄存器保存到当前进程的栈上
"addi sp, sp, -13 * 4\n" // 为13个4字节寄存器分配栈空间 
"sw ra,  0  * 4(sp)\n"   // 仅保存被调用者保存的寄存器
"sw s0,  1  * 4(sp)\n"
"sw s1,  2  * 4(sp)\n"
"sw s2,  3  * 4(sp)\n"
"sw s3,  4  * 4(sp)\n"
"sw s4,  5  * 4(sp)\n"
"sw s5,  6  * 4(sp)\n"
"sw s6,  7  * 4(sp)\n"
"sw s7,  8  * 4(sp)\n"
"sw s8,  9  * 4(sp)\n"
"sw s9,  10 * 4(sp)\n"
"sw s10, 11 * 4(sp)\n"
"sw s11, 12 * 4(sp)\n"

// 切换栈指针
"sw sp, (a0)\n"         // *prev_sp = sp;
"lw sp, (a1)\n"         // 在这里切换栈指针(sp)

// 从下一个进程的栈中恢复被调用者保存的寄存器
"lw ra,  0  * 4(sp)\n"  // 仅恢复被调用者保存的寄存器
"lw s0,  1  * 4(sp)\n"
"lw s1,  2  * 4(sp)\n"
"lw s2,  3  * 4(sp)\n"
"lw s3,  4  * 4(sp)\n"
"lw s4,  5  * 4(sp)\n"
"lw s5,  6  * 4(sp)\n"
"lw s6,  7  * 4(sp)\n"
"lw s7,  8  * 4(sp)\n"
"lw s8,  9  * 4(sp)\n"
"lw s9,  10 * 4(sp)\n"
"lw s10, 11 * 4(sp)\n"
"lw s11, 12 * 4(sp)\n"
"addi sp, sp, 13 * 4\n"  // 我们已从栈中弹出13个4字节寄存器
"ret\n"
);
}

struct process procs[PROCS_MAX];// 所有进程控制结构

struct process *create_process(uint32_t pc){
 struct process *proc = NULL;
 int i;
 for ( i = 0; i < PROCS_MAX; i++)
 {
    if (procs[i].state == PROC_UNUSED)
    {
        // 拿到当前的数组对应的地址
        proc = &procs[i];
        break;
    }
 }
 if (!proc)
 {
    PANIC("no free process control block");
 }
 // 开始通过汇编进行处理
// 设置被调用者保存的寄存器。这些寄存器值将在 switch_context 
    // 中的第一次上下文切换时被恢复。
    uint32_t *sp = (uint32_t *)&proc->stack[sizeof(proc->stack)];
    *--sp = 0;                      // s11
    *--sp = 0;                      // s10
    *--sp = 0;                      // s9
    *--sp = 0;                      // s8
    *--sp = 0;                      // s7
    *--sp = 0;                      // s6
    *--sp = 0;                      // s5
    *--sp = 0;                      // s4
    *--sp = 0;                      // s3
    *--sp = 0;                      // s2
    *--sp = 0;                      // s1
    *--sp = 0;                      // s0
    *--sp = (uint32_t) pc;          // ra

    // 初始化字段
    proc->pid = i+1;
    proc->state = PROC_RUNNABLE;
    proc->sp = (uint32_t) sp;
    return proc;
}

void delay(void){
    for (int i = 0; i < 30000000; i++)
    {
        __asm__ __volatile__("nop"); // 什么都不做
    }
}

struct process *proc_a;
struct process *proc_b;

void proc_a_entry(void){
    printf("starting process aThread\n");
    while (1)
    {
        putchar('A');
        switch_context(&proc_a->sp,&proc_b->sp);
        delay();
    }
}

void proc_b_entry(void){
    printf("starting process bThread\n");
    while (1)
    {
        putchar('B');
        switch_context(&proc_b->sp,&proc_a->sp);
        delay();
    }
}

// 智能调度
struct process *current_proc; // 当前运行的进程
struct process *idle_proc; // 空闲进程
/**
 * 这里，我们引入了两个全局变量。current_proc指向当前运行的进程。idle_proc指向空闲进程，即“当没有可运行进程时要运行的进程”。
 * idle_proc在启动时(kernal_main方法中)创建为进程 ID 为-1的进程，
 */
void yield(void){
    struct process *next = idle_proc;
    for (int i = 0; i < PROCS_MAX; i++)
    {
        struct process *proc = &procs[(current_proc->pid + i) % PROCS_MAX];
        if (proc->state == PROC_RUNNABLE && proc->pid > 0)
        {
            next = proc;
            break;
        }
    }
    // 如果除了当前进程外没有可运行的进程，返回并继续处理
    if (next == current_proc)
    {
        return;
    }
    
    // 上下文切换
    struct process *prev = current_proc;
    current_proc = next;
    // 切换
    switch_context(&prev->sp,&next->sp);
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


__attribute__((naked))
__attribute__((aligned(4)))
void kernel_entry(void){
    __asm__ __volatile__(
        "csrw sscratch, sp\n"
        "addi sp, sp, -4 * 31\n"
        "sw ra,  4 * 0(sp)\n"
        "sw gp,  4 * 1(sp)\n"
        "sw tp,  4 * 2(sp)\n"
        "sw t0,  4 * 3(sp)\n"
        "sw t1,  4 * 4(sp)\n"
        "sw t2,  4 * 5(sp)\n"
        "sw t3,  4 * 6(sp)\n"
        "sw t4,  4 * 7(sp)\n"
        "sw t5,  4 * 8(sp)\n"
        "sw t6,  4 * 9(sp)\n"
        "sw a0,  4 * 10(sp)\n"
        "sw a1,  4 * 11(sp)\n"
        "sw a2,  4 * 12(sp)\n"
        "sw a3,  4 * 13(sp)\n"
        "sw a4,  4 * 14(sp)\n"
        "sw a5,  4 * 15(sp)\n"
        "sw a6,  4 * 16(sp)\n"
        "sw a7,  4 * 17(sp)\n"
        "sw s0,  4 * 18(sp)\n"
        "sw s1,  4 * 19(sp)\n"
        "sw s2,  4 * 20(sp)\n"
        "sw s3,  4 * 21(sp)\n"
        "sw s4,  4 * 22(sp)\n"
        "sw s5,  4 * 23(sp)\n"
        "sw s6,  4 * 24(sp)\n"
        "sw s7,  4 * 25(sp)\n"
        "sw s8,  4 * 26(sp)\n"
        "sw s9,  4 * 27(sp)\n"
        "sw s10, 4 * 28(sp)\n"
        "sw s11, 4 * 29(sp)\n"

        "csrr a0, sscratch\n"
        "sw a0, 4 * 30(sp)\n"

        "mv a0, sp\n"
        "call handle_trap\n"

        "lw ra,  4 * 0(sp)\n"
        "lw gp,  4 * 1(sp)\n"
        "lw tp,  4 * 2(sp)\n"
        "lw t0,  4 * 3(sp)\n"
        "lw t1,  4 * 4(sp)\n"
        "lw t2,  4 * 5(sp)\n"
        "lw t3,  4 * 6(sp)\n"
        "lw t4,  4 * 7(sp)\n"
        "lw t5,  4 * 8(sp)\n"
        "lw t6,  4 * 9(sp)\n"
        "lw a0,  4 * 10(sp)\n"
        "lw a1,  4 * 11(sp)\n"
        "lw a2,  4 * 12(sp)\n"
        "lw a3,  4 * 13(sp)\n"
        "lw a4,  4 * 14(sp)\n"
        "lw a5,  4 * 15(sp)\n"
        "lw a6,  4 * 16(sp)\n"
        "lw a7,  4 * 17(sp)\n"
        "lw s0,  4 * 18(sp)\n"
        "lw s1,  4 * 19(sp)\n"
        "lw s2,  4 * 20(sp)\n"
        "lw s3,  4 * 21(sp)\n"
        "lw s4,  4 * 22(sp)\n"
        "lw s5,  4 * 23(sp)\n"
        "lw s6,  4 * 24(sp)\n"
        "lw s7,  4 * 25(sp)\n"
        "lw s8,  4 * 26(sp)\n"
        "lw s9,  4 * 27(sp)\n"
        "lw s10, 4 * 28(sp)\n"
        "lw s11, 4 * 29(sp)\n"
        "lw sp,  4 * 30(sp)\n"
        "sret\n"
    );
}

/**
 * next_paddr 被定义为 static 变量。这意味着，与局部变量不同，它的值在函数调用之间保持不变。也就是说，它的行为像全局变量。
   next_paddr 指向“下一个要分配区域”（空闲区域）的起始地址。在分配时，next_paddr 前进分配大小的距离。
   next_paddr 最初保存 __free_ram 的地址。这意味着内存从 __free_ram 开始顺序分配。
   由于链接器脚本中的 ALIGN(4096)，__free_ram 被放置在 4KB 边界上。因此，alloc_pages 函数总是返回与 4KB 对齐的地址。
   如果它试图分配超出 __free_ram_end 的内存，换句话说，如果内存耗尽，就会发生内核恐慌。
   memset 函数确保分配的内存区域总是用零填充。这是为了避免未初始化内存导致的难以调试的问题。
   很简单，不是吗？然而，这个内存分配算法有一个大问题：分配的内存不能被释放！话虽如此，对于我们简单的业余操作系统来说，这已经足够了
 */
paddr_t alloc_pages(uint32_t n){
    static paddr_t next_paddr = (paddr_t) __free_ram;
    paddr_t paddr = next_paddr;
    next_paddr += n * PAGE_SIZE;

    if (next_paddr >(paddr_t) __free_ram_end)
    {
        PANIC("fuck!out of memory");
    }
     memset((void *)paddr,0,n*PAGE_SIZE);
     return paddr;
    
}



void handle_trap(struct trap_frame *f){
    uint32_t scause = READ_CSR(scause);
    uint32_t stval = READ_CSR(stval);
    uint32_t user_pc = READ_CSR(sepc);

    PANIC("unexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval, user_pc);
}

void kernel_main(void)
{
    // 测试内核Pannic
  //  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
  //  PANIC("booted! worry~~~");
    // 打印台会输出 PANIC: fuck_kernel.c:52: booted! worry~~~
  //  printf("unrreachable here!\n");

  // 测试异常
  //memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
 // WRITE_CSR(stvec,(uint32_t) kernel_entry);
 // __asm__ __volatile__("unimp");

  // 测试内存分配
 // memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

//  paddr_t paddr0 = alloc_pages(2);
 // paddr_t paddr1 = alloc_pages(1);
 // printf("alloc_pages test: paddr0=%x\n", paddr0);
//  printf("alloc_pages test: paddr1=%x\n", paddr1);
//  PANIC("test memory allocation end!");

  // 测试进程切换
 // memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
 // WRITE_CSR(stvec,(uint32_t) kernel_entry);
 // proc_a = create_process((uint32_t)proc_a_entry);
 // proc_b = create_process((uint32_t)proc_b_entry);
// 执行
 // proc_a_entry();
 // PANIC("test switch_context end! below is unreachable"); 



  // 测试进程调度
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
  printf("\n\n");
  WRITE_CSR(stvec, (uint32_t) kernel_entry);

    idle_proc = create_process(0);
    idle_proc->pid = -1; // idle
    current_proc = idle_proc;
   // 创建两个进程
    proc_a = create_process((uint32_t)proc_a_entry);
    proc_b = create_process((uint32_t)proc_b_entry);
    yield();
    PANIC("switched to idle process, unreachable!!");




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
