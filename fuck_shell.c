#include "user.h"

void main(void)
{
    // *((volatile int *) 0x80200000) = 0x1234;// ��� 0x80200000 ���ں�ʹ�õ��ڴ���������ҳ����ӳ�䡣Ȼ������������һ��δ����ҳ������ U λ���ں�ҳ��Ӧ�ûᷢ���쳣(ҳ�����)���ں�Ӧ�û� panic��
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
            // ע���ڵ��Կ���̨�ϣ����з��ǣ�'\r'����
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
