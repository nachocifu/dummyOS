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

	pushaq

	mov rdi, rdx

	mov rdx, rcx
	mov rcx, rdi

	mov rdi, rax
	mov rsi, rbx

	call sysCallDispacher
	
	
	popaq

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
