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
    unit8_t *d = (unit8_t *)dst;             // 这行代码将dst指针转换为uint8_t *类型的指针，并赋值给变量d
    const unit8_t *s = (const unit8_t *)src; // 这行代码将src指针转换为const uint8_t *类型的指针，并赋值给变量s
    while (n--)
    {
        // 将src指针指向的字节复制到dst指针指向的字节，并将两个指针都向后移动一字节
        *d++ = *s++;
    };
    // 函数返回目标内存区域的指针dst。
    return dst;
}

/**
 * @param src The string to copy. const 关键字表示 src 参数是只读的，不会被函数修改。
 * @return The buffer dst. 函数返回类型是 char *，表示返回一个字符指针
 * strcpy 函数即使在 src 比 dst 的内存区域长时也会继续复制。这很容易导致 bug 和漏洞
 *
 */
char *strcpy(char *dst, const char *src)
{
    char *d = dst; // 这行代码创建了一个临时指针 d，指向缓冲区 dst 的开始位置。
    while (*src)   // 这行代码开始了一个循环，条件是 src 指针所指的字符不为 null。
    {
        *d++ = *src++; // 在循环体内，代码将 src 指针所指的字符复制到 d 指针所指的位置，然后将两个指针都向后移动一位。
    }
    *d = '\0';  // 循环结束后，代码将 null 字符 \0 写入 d 指针所指的位置，以标志字符串的结束。
    return dst; // 函数返回缓冲区 dst 的开始位置。
}

/**
 * @brief Compares two strings. 比较两个字符串
 * @param s1 The first string.
 * @param s2 The second string.
 * @return 0 if the strings are equal, a positive value if s1 is greater than s2, or a negative value if s1 is less than s2.
 *  const char *s1：第一个字符串的指针，类型是常量字符指针，表示字符串是只读的，不会被函数修改。
 */
int strcmp(const char *s1, const char *s2)
{
    // 这是一个循环语句，具体来说：
    // *s1 和 *s2 是指针解引用操作，分别获取第一个字符串和第二个字符串的当前字符。
    // && 是逻辑与运算符，如果两个条件都为真，则循环继续。
    // 循环条件是两个字符串的当前字符都非空，即两个字符串都没有结束。
    while (*s1 && *s2)
    {
        // 这是循环体，具体来说：
        // if (*s1 != *s2) 是一个条件语句，如果两个字符串的当前字符不相等，则跳出循环。
        // break 是跳出循环的语句。
        // s1++ 和 s2++ 是指针自增操作，分别指向下一个字符。
        if (*s1 != *s2)
        {
            break;
        }
        s1++;
        s2++;
    }
    // 这是函数的返回语句，具体来说：

    // *(unsigned char *)s1 和 *(unsigned char *)s2 是指针解引用操作，分别获取第一个字符串和第二个字符串的当前字符，并将其转换为无符号字符类型。
    //- 是减法运算符，计算两个字符的差值。
    // 返回值是两个字符的差值，表示两个字符串的比较结果
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

/**
 * strcmp 函数经常用于检查两个字符串是否相同。这有点反直觉，但当 !strcmp(s1, s2) 为真时（即当函数返回零时），字符串是相同的：


if (!strcmp(s1, s2))
    printf("s1 == s2\n");
else
    printf("s1 != s2\n");
 *
 *
 */