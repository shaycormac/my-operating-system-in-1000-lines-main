// �Լ�д�Ĳ���
#include "fuck_kernel.h"
#include "fuck_common.h"

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];

extern char __free_ram[], __free_ram_end[];

extern char __kernel_base[];

// �������
#define PROCS_MAX 8
#define PROC_UNUSED 0   // δʹ�õĽ��̿��ƽṹ
#define PROC_RUNNABLE 1 // �����еĽ���

// �û�ģʽ,Ӧ�ó�����Ļ��������ַ������Ҫ�� `user.ld` �ж������ʼ��ַƥ��
#define USER_BASE 0x1000000
// ���������ʹ��Ƕ��� fuck_shell.bin.o ԭʼ�������ļ�
extern char _binary_fuck_shell_bin_start[], _binary_fuck_shell_bin_size[];

// �û���ں��������ڴ��ں�ģʽ�л����û�ģʽ
__attribute__((naked)) void user_entry(void)
{
    // PANIC("not yet implemented");

    // ʹ�����������ִ��һЩ�ؼ��Ĳ���
    __asm__ __volatile__(
        "csrw sepc, %[sepc]        \n" // ������һ��ָ��ĵ�ַ�����û��������ʼ��ַ
        "csrw sstatus, %[sstatus]  \n" // ����sstatus�Ĵ������������ж����û�ģʽ�±�����
        "sret                      \n" // ���쳣������򷵻أ�ʵ�����Ǵ��ں�ģʽ�л����û�ģʽ
        :
        : [sepc] "r"(USER_BASE),      // ��USER_BASE��ַ���ص�sepc�Ĵ���
          [sstatus] "r"(SSTATUS_SPIE) // ��SSTATUS_SPIEֵ���ص�sstatus�Ĵ������������ж�ʹ��
    );
}

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,
                       long arg5, long fid, long eid)
{
    // ��ÿ���ֲ�����������ʹ�õ� register �� __asm__("register name") Ҫ���������ֵ
    // ����ָ���ļĴ����С�����ϵͳ���õ��ù�������һ�������÷�
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

/**
 * �ӿ���̨��ȡһ���ַ�
 * 
 * �ú���ͨ��ϵͳ���ýӿڣ�SBI����ײ����ϵͳ�������Ի�ȡ�û�ͨ������������ַ�
 * ���������κβ�����������һ��������ֵ�������ȡ�����ַ�
 * 
 * @return long �����û�������ַ����Գ����ͱ�ʾ����������󣬷���ֵ�����Ǹ���
 * 
 * ע�⣺�ϸ���˵��SBI ���ǴӼ��̶�ȡ�ַ������ǴӴ��ڶ�ȡ����֮�����ܹ���������Ϊ���̣��� QEMU �ı�׼���룩���ӵ����ڡ�
 */
long getchar(void){
    // ����SBI�ӿڣ�����Ϊ0��ʾ����̨��ȡ�ַ���������������Ϊ0��ʾû�ж���Ĳ�������
    // ����ֵ������sbiret�ṹ���У����е�error�ֶα�ʾ�����Ľ����������
    struct sbiret ret = sbi_call(0, 0, 0, 0, 0, 0, 0, 2 /* Console Getchar */);
    return ret.error;
}

// �������л�
__attribute__((naked)) void switch_context(uint32_t *prev_sp,
                                           uint32_t *next_sp)
{
    __asm__ __volatile__(
        // ���������߱���Ĵ������浽��ǰ���̵�ջ��
        "addi sp, sp, -13 * 4\n" // Ϊ13��4�ֽڼĴ�������ջ�ռ�
        "sw ra,  0  * 4(sp)\n"   // �����汻�����߱���ļĴ���
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

        // �л�ջָ��
        "sw sp, (a0)\n" // *prev_sp = sp;
        "lw sp, (a1)\n" // �������л�ջָ��(sp)

        // ����һ�����̵�ջ�лָ��������߱���ļĴ���
        "lw ra,  0  * 4(sp)\n" // ���ָ��������߱���ļĴ���
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
        "addi sp, sp, 13 * 4\n" // �����Ѵ�ջ�е���13��4�ֽڼĴ���
        "ret\n");
}

/**
 * next_paddr ������Ϊ static ����������ζ�ţ���ֲ�������ͬ������ֵ�ں�������֮�䱣�ֲ��䡣Ҳ����˵��������Ϊ��ȫ�ֱ�����
   next_paddr ָ����һ��Ҫ�������򡱣��������򣩵���ʼ��ַ���ڷ���ʱ��next_paddr ǰ�������С�ľ��롣
   next_paddr ������� __free_ram �ĵ�ַ������ζ���ڴ�� __free_ram ��ʼ˳����䡣
   �����������ű��е� ALIGN(4096)��__free_ram �������� 4KB �߽��ϡ���ˣ�alloc_pages �������Ƿ����� 4KB ����ĵ�ַ��
   �������ͼ���䳬�� __free_ram_end ���ڴ棬���仰˵������ڴ�ľ����ͻᷢ���ں˿ֻš�
   memset ����ȷ��������ڴ���������������䡣����Ϊ�˱���δ��ʼ���ڴ浼�µ����Ե��Ե����⡣
   �ܼ򵥣�������Ȼ��������ڴ�����㷨��һ�������⣺������ڴ治�ܱ��ͷţ�������ˣ��������Ǽ򵥵�ҵ�����ϵͳ��˵�����Ѿ��㹻��
 */
paddr_t alloc_pages(uint32_t n)
{
    static paddr_t next_paddr = (paddr_t)__free_ram;
    paddr_t paddr = next_paddr;
    next_paddr += n * PAGE_SIZE;

    if (next_paddr > (paddr_t)__free_ram_end)
    {
        PANIC("fuck!out of memory");
    }
    memset((void *)paddr, 0, n * PAGE_SIZE);
    return paddr;
}

// ӳ��ҳ��
// ���� map_page ��������һ��ҳ��table1���������ַ��vaddr���������ַ��paddr����ҳ�����־��flags����
void map_page(uint32_t *table1, uint32_t vaddr, paddr_t paddr, uint32_t flags)
{
    if (!(is_aligned(vaddr, PAGE_SIZE)))
    {
        PANIC("unalign vaddr %x", vaddr);
    }

    if (!is_aligned(paddr, PAGE_SIZE))
    {
        PANIC("unalign paddr %x", paddr);
    }

    uint32_t vpn1 = (vaddr >> 22) & 0x3ff;
    if ((table1[vpn1] & PAGE_V) == 0)
    {
        // ���������ڵĶ���ҳ��
        uint32_t pt_paddr = alloc_pages(1);
        table1[vpn1] = ((pt_paddr / PAGE_SIZE) << 10) | PAGE_V;
    }
    // ���ö���ҳ������ӳ������ҳ��
    uint32_t vpn0 = (vaddr >> 12) & 0x3ff;
    uint32_t *table0 = (uint32_t *)((table1[vpn1] >> 10) * PAGE_SIZE);
    table0[vpn0] = ((paddr / PAGE_SIZE) << 10) | flags | PAGE_V;
}

struct process procs[PROCS_MAX]; // ���н��̿��ƽṹ

/**
 * create_process ������ӳ���ں�ҳ�档�ں�ҳ��� __kernel_base ��Խ�� __free_ram_end��
 * ���ַ���ȷ���ں�ʼ�տ��Է��ʾ�̬����������� .text������ alloc_pages ����Ķ�̬��������
 *
 *
 * �޸��� create_process �Խ���ִ�о����ָ��(image)�;����С(image_size)��Ϊ������
 * ����ָ����С��ҳ����ִ�о��񲢽���ӳ�䵽���̵�ҳ���С�ͬʱ��������һ���������л�����תĿ������Ϊ user_entry
 */
struct process *create_process(const void *image, size_t image_size)
{
    struct process *proc = NULL;
    int i;
    for (i = 0; i < PROCS_MAX; i++)
    {
        if (procs[i].state == PROC_UNUSED)
        {
            // �õ���ǰ�������Ӧ�ĵ�ַ
            proc = &procs[i];
            break;
        }
    }
    if (!proc)
    {
        PANIC("no free process control block");
    }
    // ��ʼͨ�������д���
    // ���ñ������߱���ļĴ�������Щ�Ĵ���ֵ���� switch_context
    // �еĵ�һ���������л�ʱ���ָ���
    uint32_t *sp = (uint32_t *)&proc->stack[sizeof(proc->stack)];
    *--sp = 0;                    // s11
    *--sp = 0;                    // s10
    *--sp = 0;                    // s9
    *--sp = 0;                    // s8
    *--sp = 0;                    // s7
    *--sp = 0;                    // s6
    *--sp = 0;                    // s5
    *--sp = 0;                    // s4
    *--sp = 0;                    // s3
    *--sp = 0;                    // s2
    *--sp = 0;                    // s1
    *--sp = 0;                    // s0
    *--sp = (uint32_t)user_entry; // ra

    // �������̵��ں˷���,ӳ���ں�ҳ��
    // ��һ���ȷ����ڴ棬�ڶ������ڴ���и� page_table ����ӳ��
    uint32_t *page_table = (uint32_t *)alloc_pages(1);
    // ӳ���ں�ҳ
    for (paddr_t paddr = (paddr_t)__kernel_base; paddr < (paddr_t)__free_ram_end; paddr += PAGE_SIZE)
    {
        map_page(page_table, paddr, paddr, PAGE_R | PAGE_W | PAGE_X);
    }
    // ӳ���û�ҳ
    for (uint32_t off = 0; off < image_size; off += PAGE_SIZE)
    {
        paddr_t page = alloc_pages(1);
        // ����Ҫ���Ƶ�����С��ҳ���С�����
        size_t remaining = image_size - off;
        size_t copy_size = PAGE_SIZE <= remaining ? PAGE_SIZE : remaining;
        // ��䲢ӳ��ҳ��
        memcpy((void *)page, image + off, copy_size);
        map_page(page_table, USER_BASE + off, page, PAGE_U | PAGE_R | PAGE_W | PAGE_X);
    }

    // ��ʼ���ֶ�
    proc->pid = i + 1;
    proc->state = PROC_RUNNABLE;
    proc->sp = (uint32_t)sp;
    proc->page_table = page_table;
    return proc;
}

void delay(void)
{
    for (int i = 0; i < 30000000; i++)
    {
        __asm__ __volatile__("nop"); // ʲô������
    }
}

struct process *proc_a;
struct process *proc_b;

void proc_a_entry(void)
{
    printf("starting process aThread\n");
    while (1)
    {
        putchar('A');
        switch_context(&proc_a->sp, &proc_b->sp);
        delay();
    }
}

void proc_b_entry(void)
{
    printf("starting process bThread\n");
    while (1)
    {
        putchar('B');
        switch_context(&proc_b->sp, &proc_a->sp);
        delay();
    }
}

// ���ܵ���
struct process *current_proc; // ��ǰ���еĽ���
struct process *idle_proc;    // ���н���
/**
 * �����������������ȫ�ֱ�����current_procָ��ǰ���еĽ��̡�idle_procָ����н��̣�������û�п����н���ʱҪ���еĽ��̡���
 * idle_proc������ʱ(kernal_main������)����Ϊ���� ID Ϊ-1�Ľ��̣�
 */
void yield(void)
{
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
    // ������˵�ǰ������û�п����еĽ��̣����ز���������
    if (next == current_proc)
    {
        return;
    }
    // �������л�
    struct process *prev = current_proc;
    current_proc = next;
    // �쳣�������ı��
    // �ڽ����л��ڼ���sscratch�Ĵ��������õ�ǰִ�н��̵��ں�ջ�ĳ�ʼֵ��
    __asm__ __volatile__(
        "sfence.vma\n"
        "csrw satp, %[satp]\n"
        "sfence.vma\n"
        "csrw sscratch, %[sscratch]\n"
        :
        // ��Ҫ����β�涺�ţ�
        : [satp] "r"(SATP_SV32 | ((uint32_t)next->page_table / PAGE_SIZE)),
          [sscratch] "r"((uint32_t)&next->stack[sizeof(next->stack)]));
    // �л�
    switch_context(&prev->sp, &next->sp);
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
__attribute__((aligned(4))) void
kernel_entry(void)
{
    __asm__ __volatile__(
        // ��sscratch�л�ȡ���н��̵��ں�ջ
        "csrrw sp, sscratch, sp\n"
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

        // ��ȡ�������쳣����ʱ��sp
        "csrr a0, sscratch\n"
        "sw a0,  4 * 30(sp)\n"

        // �����ں�ջ
        "addi a0, sp, 4 * 31\n"
        "csrw sscratch, a0\n"

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
        "sret\n");
}

/**
 * ����ϵͳ����
 * 
 * �˺�������trap_frame�е�ϵͳ���úŴ�����Ӧ��ϵͳ����
 * �����ϵͳ���úţ������ݵ��õ�����ִ����Ӧ�Ĳ���
 * 
 * @param f ָ��trap_frame�ṹ���ָ�룬������ϵͳ���õĲ����͵��ú�
 */
void handle_syscall(struct trap_frame *f)
{
    // ����ϵͳ���ú�ѡ����Ӧ�Ĳ���
    switch (f->a3)
    {
    case SYS_PUTCHAR:
        // ִ��putcharϵͳ���ã�����ַ�
        putchar(f->a0);
        break;
    case SYS_GETCHAR:
        while (1)
        {
            long ch = getchar(); // getchar ϵͳ���õ�ʵ���ظ����� SBI ֱ������һ���ַ������ǣ��򵥵��ظ������������ֹ�����������У��������ǵ��� yield ϵͳ�������� CPU �ø��������̡�
            if (ch >= 0)
            {
                f->a0 = ch;
                break;
            }
            yield();
        }
        break;
        case SYS_EXIT:
        printf("process %d exited\n",current_proc->pid);// Ϊ�˼����������ֻ�ǽ����̱��Ϊ���˳���PROC_EXITED����
        // ������빹��һ��ʵ�õĲ���ϵͳ����Ҫ�ͷŽ��̳��е���Դ������ҳ��ͷ�����ڴ�����
        current_proc->state = PROC_EXITED;
        yield();
        PANIC("unreachable code");
        break;
    default:
        // ����δ֪��ϵͳ���ú�ʱ����PANIC
        PANIC("unexpected syscall a3=%x\n", f->a3);
    }
}

/**
 * �����ж�����
 * �������жϻ��쳣ʱ��Ӳ�����Զ���ת������Ĵ������
 *
 * @param f ָ���жϿ�ܽṹ��ָ�룬�����жϷ���ʱ�ļĴ���״̬
 */
void handle_trap(struct trap_frame *f)
{
    // ��ȡ�����жϵ�ԭ��
    uint32_t scause = READ_CSR(scause);
    // ��ȡ�ж���ص�ֵ��������ϵ�ַ
    uint32_t stval = READ_CSR(stval);
    // ��ȡ�û������ж�ʱ�ĳ��������ֵ
    uint32_t user_pc = READ_CSR(sepc);

    // ����ж�ԭ���Ƿ�Ϊϵͳ����
    if (scause == SCAUSE_ECALL)
    {
        // ����ϵͳ���ô�����
        handle_syscall(f);
        // ϵͳ���ú󣬳������������4�����������жϵ�ָ��
        user_pc += 4;
    }
    else
    {
        // ���ڷ�ϵͳ���õ��жϣ���ӡ������Ϣ�������ֻ�ģʽ
        PANIC("unexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval, user_pc);
    }

    // ���³�����������Ա���жϴ�����򷵻غ����ִ��
    WRITE_CSR(sepc, user_pc);
}

void kernel_main(void)
{
    // �����ں�Pannic
    //  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
    //  PANIC("booted! worry~~~");
    // ��ӡ̨����� PANIC: fuck_kernel.c:52: booted! worry~~~
    //  printf("unrreachable here!\n");

    // �����쳣
    // memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
    // WRITE_CSR(stvec,(uint32_t) kernel_entry);
    // __asm__ __volatile__("unimp");

    // �����ڴ����
    // memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

    //  paddr_t paddr0 = alloc_pages(2);
    // paddr_t paddr1 = alloc_pages(1);
    // printf("alloc_pages test: paddr0=%x\n", paddr0);
    //  printf("alloc_pages test: paddr1=%x\n", paddr1);
    //  PANIC("test memory allocation end!");

    // ���Խ����л�
    // memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
    // WRITE_CSR(stvec,(uint32_t) kernel_entry);
    // proc_a = create_process((uint32_t)proc_a_entry);
    // proc_b = create_process((uint32_t)proc_b_entry);
    // ִ��
    // proc_a_entry();
    // PANIC("test switch_context end! below is unreachable");

    // ���Խ��̵���
    // memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
    // printf("\n\n");
    // WRITE_CSR(stvec, (uint32_t)kernel_entry);

    // idle_proc = create_process(0);
    // idle_proc->pid = -1; // idle
    // current_proc = idle_proc;
    // ������������
    // proc_a = create_process((uint32_t)proc_a_entry);
    // proc_b = create_process((uint32_t)proc_b_entry);
    // yield();
    // PANIC("switched to idle process, unreachable!!");

    // �����û�ģʽ
    memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
    printf("\n\n");
    WRITE_CSR(stvec, (uint32_t)kernel_entry);
    idle_proc = create_process(NULL, 0);
    idle_proc->pid = 0;
    current_proc = idle_proc;
    // �����û�����
    create_process(_binary_fuck_shell_bin_start, (size_t)_binary_fuck_shell_bin_size);
    yield();
    PANIC("switched to idle process, unreachable!!");

    // �����Զ��������ӡ
    printf("\n\nHello, %s\n", "RISC-V!");
    printf("1 + 2 = %d,%x\n", 1 + 2, 0x1234abcd);

    const char *s = "\n\nHello, RISC-V!\n";
    for (size_t i = 0; s[i] != '\0'; i++)
    {
        putchar(s[i]);
    }
    for (;;)
    {
        // ����ѭ��
        __asm__ __volatile__("wfi");
    }

    // ��һ�����Խ���
    //  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
    // ��ʼ������κ����ݶ�
    //  for (;;)
    //  {
    // ����ѭ��
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
