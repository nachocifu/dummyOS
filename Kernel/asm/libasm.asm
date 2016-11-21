GLOBAL cpuVendor
GLOBAL sti
GLOBAL setPicMaster
GLOBAL setPicSlave
GLOBAL irq0Handler
GLOBAL irq1Handler
GLOBAL irq2Handler
GLOBAL sysCallHandler
GLOBAL in_b
GLOBAL out_b
GLOBAL in_l
GLOBAL out_l
GLOBAL sysOutLong
GLOBAL sysInLong
GLOBAL sysInByte
GLOBAL sysInWord
GLOBAL sysOutWord
GLOBAL sysOutByte

GLOBAL os_pci_read_reg
GLOBAL os_pci_write_reg

EXTERN irqDispatcher
EXTERN sysCallDispacher

%include "./asm/macros.m"

section .text

; -----------------------------------------------------------------------------
; os_pci_read_reg -- Read a register from a PCI device
;  IN:	BL  = Bus number
;	CL  = Device/Function number
;	DL  = Register number
; OUT:	EAX = Register information
;	All other registers preserved
; Data form is binary 10000000 bbbbbbbb dddddfff rrrrrr00
os_pci_read_reg:
	push rdx
	push rcx
	push rbx

	shl ebx, 16			; Move Bus to bits 23 - 16
	shl ecx, 8			; Move Device/Function to bits 15 - 8
	mov bx, cx
	shl edx, 2			; Move Register to bits 7 - 2
	mov bl, dl
	and ebx, 0x00ffffff		; Clear bits 31 - 24
	or ebx, 0x80000000		; Set bit 31
	mov eax, ebx
	mov dx, PCI_CONFIG_ADDRESS
	out dx, eax
	mov dx, PCI_CONFIG_DATA
	in eax, dx

	pop rbx
	pop rcx
	pop rdx
	ret
; -----------------------------------------------------------------------------


os_pci_write_reg:
	push rbx
	push rcx

	mov rbx, rdi
	mov rcx, rsi

	shl ebx, 16			; Move Bus to bits 23 - 16
	shl ecx, 8			; Move Device/Function to bits 15 - 8
	mov bx, cx
	mov bl, dl
	and ebx, 0x00ffffff		; Clear bits 31 - 24
	or ebx, 0x80000000		; Set bit 31
	mov eax, ebx
	mov dx, PCI_CONFIG_ADDRESS
	out dx, eax
	mov dx, PCI_CONFIG_DATA
	pop rax
	out dx, eax

	pop rbx
	ret

sysOutLong:
	push rbx
	push rax 

	mov rbx, rdi
	and rbx, 0xFFFF ;16 bits

	mov rax, rsi

	mov dx, bx
	out dx, eax
	

	pop rax
	pop rbx
	ret


;    sysOutLong( uint16_t addr, uint32 data);
;                      RDI          RSI
sysOutWord:
	push rbx
	push rax 

	mov rbx, rdi
	and rbx, 0xFFFF ;16 bits

	mov rax, rsi

	mov dx, bx
	out dx, ax
	

	pop rax
	pop rbx
	ret

;    sysOutLong( uint16_t addr, uint32 data);
;                      RDI          RSI
sysOutByte:
	push rbx
	push rax 

	mov rbx, rdi
	and rbx, 0xFFFF ;16 bits

	mov rax, rsi

	mov dx, bx
	out dx, al
	

	pop rax
	pop rbx
	ret




;sysInLong( uint16_t addr);
;                 RDI
;
sysInLong:
	push rbx

	mov rbx, rdi
	and rbx, 0xFFFF ;16 bits

	mov rax, rsi

	mov dx, bx
	in eax, dx

	pop rbx

	ret


;sysInByte( uint16_t addr);
;                 RDI
;
sysInByte:
	push rbx

	mov rbx, rdi
	and rbx, 0xFFFF ;16 bits

	mov rax, rsi

	mov dx, bx
	in al, dx
	and rax, 0xff;
	
	pop rbx

	ret


;sysInByte( uint16_t addr);
;                 RDI
;
sysInWord:
	push rbx

	mov rbx, rdi
	and rbx, 0xFFFF ;16 bits

	mov rax, rsi

	mov dx, bx
	in ax, dx
	and rax, 0xffff;
	
	pop rbx

	ret

irq0Handler:
	irqHandler 0
	
irq1Handler:
	irqHandler 1

irq2Handler:
	irqHandlerSlave 2

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

; parametros 1ro registro
; retorna en al

in_l:
	push rbp
	mov rbp, rsp

	xor rax, rax

	mov rdx, rdi

	in eax, dx

	mov rsp, rbp
	pop rbp
	ret

; parametros 1ro registro, 2do el valor
; retorna nada

out_l:
	push rbp
	mov rbp, rsp

	xor rax, rax

	mov rdx, rdi
	mov rax, rsi

	out DX, EAX

	mov rsp, rbp
	pop rbp
	ret

sti:
	sti
	ret

setPicSlave:
	push rbp
	mov rbp, rsp
	
	mov rax, rdi
	out 0A1h, al
	
	mov rsp, rbp
	pop rbp
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

PCI_CONFIG_ADDRESS	EQU	0x0CF8
PCI_CONFIG_DATA		EQU	0x0CFC

section .data
	result dd 0

