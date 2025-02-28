#include "user.h"

void main(void)
{
    // *((volatile int *) 0x80200000) = 0x1234;// 这个 0x80200000 是内核使用的内存区域，它在页表上映射。然而，由于它是一个未设置页表项中 U 位的内核页，应该会发生异常(页面错误)，内核应该会 panic。
    // for(;;);
    // printf("Hello World from Shell!\n");
    while (1)
    {
    prompt:
        printf("HaHashell> ");
        char cmd[128];
        for (int i = 0;; i++)
        {
            char ch = getchar();
            putchar(ch);
            if (i == sizeof(cmd) - 1)
            {
                printf("cmd too long\n");
                goto prompt;
            }
            // 注意在调试控制台上，换行符是（'\r'）。
            else if (ch == '\r')
            {
                printf("\n");
                cmd[i] = '\0';
                break;
            }
            else
            {
                cmd[i] = ch;
            }
        }
        if (strcmp(cmd, "hello") == 0)
        {
            printf("hello world from shell!!\n");
        }else if (strcmp(cmd, "exit") == 0)
        {
            exit();
        }
        
        else
        {
            printf("unknown command: %s\n", cmd);
        }
    }
}
