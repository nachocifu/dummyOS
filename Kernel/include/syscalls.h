#define SYSCALL_WRITE 0
#define SYSCALL_READ 1
#define DESCRIPTOR_CLI 0
#define DESCRIPTOR_NET 1
#define SYSCALL_MALLOC 2
#define SYSCALL_CALLOC 3
#define SYSCALL_TIME 2


int syscall(int a, int b, int c, int d);