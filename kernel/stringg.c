#include "stringg.h"
// some manual string functions (cannot include "string.h")
int strlen(const char *str)
{
    const char *s = str;
    while (*s)
        ++s;
    return s - str;
}

char *strcpy(char *dest, const char *src)
{
    char *save = dest;
    while ((*dest++ = *src++))
        ;
    return save;
}

int strcmp(const char *str1, const char *str2)
{
    while (*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

int strncmp(const char *str1, const char *str2, int n)
{
    while (n && *str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
        n--;
    }
    if (n == 0)
        return 0;
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}
char *strtok(char *str, const char *delim)
{
    static char *next_token = NULL;
    char *token_start;

    // If the input string is NULL, continue tokenizing the previous string
    if (str == NULL)
    {
        str = next_token;
    }

    // If the string is NULL or an empty string, return NULL
    if (str == NULL || *str == '\0')
    {
        next_token = NULL;
        return NULL;
    }

    // Tokenize and skip any leading delimiters
    token_start = str + strspn(str, delim);
    if (*token_start == '\0')
    {
        next_token = NULL;
        return NULL;
    }

    // Find the end of the token
    next_token = token_start + strcspn(token_start, delim);
    if (*next_token == '\0')
    {
        next_token = NULL;
    }
    else
    {
        // Replace the following delimiter with a null terminator
        *next_token = '\0';
        next_token++;
    }

    return token_start;
}

int strspn(const char *str, const char *set)
{
    int len = 0;
    while (*str && strchr(set, *str))
    {
        str++;
        len++;
    }
    return len;
}

int strcspn(const char *str, const char *set)
{
    int len = 0;
    while (*str && !strchr(set, *str))
    {
        str++;
        len++;
    }
    return len;
}

char *strchr(const char *str, int c)
{
    while (*str)
    {
        if (*str == (char)c)
        {
            return (char *)str;
        }
        str++;
    }
    if (*str == (char)c)
    {
        return (char *)str;
    }
    return NULL;
}

int convert(char s[])
{
    int i, n = 0;

    for (i = 0; s[i] >= '0' && s[i] <= '9'; i++)
        n = 10 * n + (s[i] - '0');

    return n;
}