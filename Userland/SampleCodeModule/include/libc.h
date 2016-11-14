#include <syscalls_defines.h>
#include <stdint.h>
#include <stdarg.h>

#define TRUE 1
#define FALSE 0
#define MAX_PARAMS_SHELL 5


void printf(char *format, ...);
void scanf(char *format, ...);
char** str_split(char *str);