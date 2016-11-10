GLOBAL cpuVendor
GLOBAL sti
GLOBAL setPicMaster
GLOBAL irq0Handler
GLOBAL irq1Handler
GLOBAL sysCallHandler
GLOBAL in_b
GLOBAL out_b

EXTERN irqDispatcher
EXTERN sysCallDispacher

%include "./asm/macros.m"

section .text

irq0Handler:
	irqHandler 0
	
irq1Handler:
	irqHandler 1

sysCallHandler:

	; El orden de las syscalls es rax, rbx, rcx y rdx.
	; El orden de las funciones en C son rdi, rsi, rdx, rcx, r8 y r9.
	; Aca solamente tengo que redistribuir los registros.

	pushaq
	mov r8, rdx

	mov rdi, rax
	mov rsi, rbx
	mov rdx, rcx
	mov rcx, r8
	
	call sysCallDispacher
	
	mov [result], rax ; Como 'popaq' pone todos los registros en su valor original, guardo 'rax' (mi valor de retorno)
					  ; en una variable en memoria para despues de llamar a 'popaq' volver a asignarlo a 'rax'.
	popaq
	mov rax, [result]

	iretq
	

; parametros 1ro registro
; retorna en al

in_b:
	push rbp
	mov rbp, rsp

	xor rax, rax

	mov rdx, rdi

	in al, dx

	mov rsp, rbp
	pop rbp
	ret

; parametros 1ro registro, 2do el valor
; retorna nada

out_b:
	push rbp
	mov rbp, rsp

	xor rax, rax

	mov rdx, rdi
	mov rax, rsi

	out dx, al

	mov rsp, rbp
	pop rbp
	ret


sti:
	sti
	ret
	
setPicMaster:
	push rbp
	mov rbp, rsp
	
	mov rax, rdi
	out 21h, al
	
	mov rsp, rbp
	pop rbp
	ret

cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid

	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

section .data
	result dd 0

