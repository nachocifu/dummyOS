GLOBAL two
GLOBAL interumpitest

two:
	mov rax, 2
	ret

interumpitest:
	mov eax, 1
	mov ebx, 8
	mov ecx, 3
	mov edx, 7

	int 80h

	ret