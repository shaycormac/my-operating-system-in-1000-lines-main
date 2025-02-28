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
 * ִ��ϵͳ����
 * 
 * ������ͨ���������ָ��ʵ��ϵͳ���ã�������Ȩת��������ϵͳ�ں�
 * ��ʹ����RISC-V�ܹ����ض��Ĵ�����ecallָ����ִ��ϵͳ����
 * 
 * @param sysno ϵͳ���úţ�����ָ�������ϵͳ���ò���
 * @param arg0 ϵͳ���õĵ�һ������
 * @param arg1 ϵͳ���õĵڶ�������
 * @param arg2 ϵͳ���õĵ���������
 * @return ����ϵͳ���õĽ�������庬��ȡ����ϵͳ���ò���
 * 
 * ע�⣺������ֱ����Ӳ��������ʹ��ǰ��ȷ�����ϵͳ���õĻ��ƺͺ��
 */
int syscall(int sysno, int arg0, int arg1, int arg2) {
    // ��arg0��ֵ��a0�Ĵ��������ڴ���ϵͳ���õĵ�һ������
    register int a0 __asm__("a0") = arg0;
    // ��arg1��ֵ��a1�Ĵ��������ڴ���ϵͳ���õĵڶ�������
    register int a1 __asm__("a1") = arg1;
    // ��arg2��ֵ��a2�Ĵ��������ڴ���ϵͳ���õĵ���������
    register int a2 __asm__("a2") = arg2;
    // ��sysno��ֵ��a3�Ĵ��������ڴ���ϵͳ���ú�
    register int a3 __asm__("a3") = sysno;

    // ִ��ecallָ�����ϵͳ�����쳣��������Ȩת�����ں�
    // ecallָ����RISC-V�ܹ������ڷ���ϵͳ���õ�ָ��
    // ����ʹ����������࣬__volatile__�ؼ���ȷ����λ����벻�ᱻ�������Ż���
    // "=r"(a0)��ʾa0�Ĵ����������������"r"(a0)�ȱ�ʾa0�ȼĴ��������������
    // "memory"��ʾ��δ�����ܻ�����ڴ棬��ֹ����������ĳЩ�Ż�
    __asm__ __volatile__("ecall"
                         : "=r"(a0)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3)
                         : "memory");

    // ����ϵͳ���õĽ�������������a0�Ĵ�����
    return a0;
}

/**
 * �˳�����
 * 
 * ͨ��ϵͳ����(SYS_EXIT)����ֹ�����ִ�С��ú������᷵�أ����ʹ��noreturn���Խ���������
 * ��ϵͳ����֮�󣬰���һ������ѭ�����Է�ϵͳ����ʧ�ܻ�ĳЩδԤ����������º�������ִ�С�
 * 
 * ����: ��
 * ����ֵ: ��
 */
__attribute__((noreturn)) void exit(void) {
    syscall(SYS_EXIT, 0, 0, 0); // ִ��ϵͳ�������˳�����
    for (;;); // �Է���һ��ȷ�����򲻻����ִ��
}