#include "fuck_common.h"

void putchar(char ch);

void printf(const char *fmt, ...)
{
    va_list vargs;
    va_start(vargs, fmt);

    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case '\0':
                putchar('%');
                goto end;
            case '%':
                putchar('%');
                break;
            case 's':
            {
                const char *s = va_arg(vargs, const char *);
                while (*s)
                {
                    putchar(*s);
                    s++;
                }
                break;
            }
            case 'd':
            {
                int value = va_arg(vargs, int);
                if (value < 0)
                {
                    putchar('-');
                    value = -value;
                }

                int divisor = 1;
                while (value / divisor > 9)
                    divisor *= 10;

                while (divisor > 0)
                {
                    putchar('0' + value / divisor);
                    value %= divisor;
                    divisor /= 10;
                }

                break;
            }
            case 'x':
            {
                int value = va_arg(vargs, int);
                for (int i = 7; i >= 0; i--)
                {
                    int nibble = (value >> (i * 4)) & 0xf;
                    putchar("0123456789abcdef"[nibble]);
                }
            }
            }
        }
        else
        {
            putchar(*fmt);
        }
        fmt++;
    }

end:
    va_end(vargs);
}

void *memcpy(void *dst, const void *src, size_t n)
{
    unit8_t *d = (unit8_t *)dst;             // ���д��뽫dstָ��ת��Ϊuint8_t *���͵�ָ�룬����ֵ������d
    const unit8_t *s = (const unit8_t *)src; // ���д��뽫srcָ��ת��Ϊconst uint8_t *���͵�ָ�룬����ֵ������s
    while (n--)
    {
        // ��srcָ��ָ����ֽڸ��Ƶ�dstָ��ָ����ֽڣ���������ָ�붼����ƶ�һ�ֽ�
        *d++ = *s++;
    };
    // ��������Ŀ���ڴ������ָ��dst��
    return dst;
}

/**
 * @param src The string to copy. const �ؼ��ֱ�ʾ src ������ֻ���ģ����ᱻ�����޸ġ�
 * @return The buffer dst. �������������� char *����ʾ����һ���ַ�ָ��
 * strcpy ������ʹ�� src �� dst ���ڴ�����ʱҲ��������ơ�������׵��� bug ��©��
 *
 */
char *strcpy(char *dst, const char *src)
{
    char *d = dst; // ���д��봴����һ����ʱָ�� d��ָ�򻺳��� dst �Ŀ�ʼλ�á�
    while (*src)   // ���д��뿪ʼ��һ��ѭ���������� src ָ����ָ���ַ���Ϊ null��
    {
        *d++ = *src++; // ��ѭ�����ڣ����뽫 src ָ����ָ���ַ����Ƶ� d ָ����ָ��λ�ã�Ȼ������ָ�붼����ƶ�һλ��
    }
    *d = '\0';  // ѭ�������󣬴��뽫 null �ַ� \0 д�� d ָ����ָ��λ�ã��Ա�־�ַ����Ľ�����
    return dst; // �������ػ����� dst �Ŀ�ʼλ�á�
}

/**
 * @brief Compares two strings. �Ƚ������ַ���
 * @param s1 The first string.
 * @param s2 The second string.
 * @return 0 if the strings are equal, a positive value if s1 is greater than s2, or a negative value if s1 is less than s2.
 *  const char *s1����һ���ַ�����ָ�룬�����ǳ����ַ�ָ�룬��ʾ�ַ�����ֻ���ģ����ᱻ�����޸ġ�
 */
int strcmp(const char *s1, const char *s2)
{
    // ����һ��ѭ����䣬������˵��
    // *s1 �� *s2 ��ָ������ò������ֱ��ȡ��һ���ַ����͵ڶ����ַ����ĵ�ǰ�ַ���
    // && ���߼�����������������������Ϊ�棬��ѭ��������
    // ѭ�������������ַ����ĵ�ǰ�ַ����ǿգ��������ַ�����û�н�����
    while (*s1 && *s2)
    {
        // ����ѭ���壬������˵��
        // if (*s1 != *s2) ��һ��������䣬��������ַ����ĵ�ǰ�ַ�����ȣ�������ѭ����
        // break ������ѭ������䡣
        // s1++ �� s2++ ��ָ�������������ֱ�ָ����һ���ַ���
        if (*s1 != *s2)
        {
            break;
        }
        s1++;
        s2++;
    }
    // ���Ǻ����ķ�����䣬������˵��

    // *(unsigned char *)s1 �� *(unsigned char *)s2 ��ָ������ò������ֱ��ȡ��һ���ַ����͵ڶ����ַ����ĵ�ǰ�ַ���������ת��Ϊ�޷����ַ����͡�
    //- �Ǽ�������������������ַ��Ĳ�ֵ��
    // ����ֵ�������ַ��Ĳ�ֵ����ʾ�����ַ����ıȽϽ��
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

/**
 * strcmp �����������ڼ�������ַ����Ƿ���ͬ�����е㷴ֱ�������� !strcmp(s1, s2) Ϊ��ʱ����������������ʱ�����ַ�������ͬ�ģ�


if (!strcmp(s1, s2))
    printf("s1 == s2\n");
else
    printf("s1 != s2\n");
 *
 *
 */