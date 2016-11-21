#define SYSCALL_WRITE 0
#define SYSCALL_READ 1
#define DESCRIPTOR_CLI 0
#define DESCRIPTOR_NET 1

int syscall(int a, int b, int c, int d);