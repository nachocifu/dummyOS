GLOBAL syscall


 ; syscall: Esta funcion en asembler se utiliza para, desde C, llamar a las syscalls con ints. 
 ; parametros: rdi, rsi, rdx y rcx.
 ; retorna nada.

syscall:

	push rbp
	mov rbp, rsp

	; El orden de las syscalls es rax, rbx, rcx y rdx.
	; El orden de las funciones en C son rdi, rsi, rdx, rcx, r8 y r9.
	; Aca solamente tengo que redistribuir los registros.

	mov rdi, rdx ; Pongo rdx en rdi para alternarlo con rcx.

	mov rdx, rcx 
	mov rcx, rdi

	mov rax, rdi
	mov rbx, rsi

	mov rsp, rbp
	pop rbp

	int 80h
	ret
