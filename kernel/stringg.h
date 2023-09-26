#define NULL ((void *)0)
// manual string functions declaration
int strlen(const char *str);
char *strcpy(char *dest, const char *src);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, int n);
char *strtok(char *str, const char *delim);
int strspn(const char *str, const char *set);
int strcspn(const char *str, const char *set);
char *strchr(const char *str, int c);
int convert(char s[]);