EXTERN on_keyboard
EXTERN on_mouse
EXTERN on_syscall
EXTERN on_timer_tick
EXTERN on_page_fault

EXTERN mouse_handler

extern os_dump_regs

GLOBAL _load_handler
GLOBAL _get_idtr

GLOBAL _page_fault_handler
GLOBAL _keyboard_interrupt_handler
GLOBAL _mouse_interrupt_handler
GLOBAL _backup_interrupt_handler
GLOBAL _syscall_interrupt_handler
GLOBAL _tt_interrupt_handler

extern drop_to_kernel
extern back_to_user

extern int80_drop_to_kernel
extern int80_back_to_user

global _sim_interrupt
global _ctx_switch

extern scheduler_switch
extern on_timer_step

extern on_gpf
global _gpf_handler

%macro PUSHALL 0
  push rax      ;save current rax
  push rbx      ;save current rbx
  push rcx      ;save current rcx
  push rdx      ;save current rdx
  push rbp      ;save current rbp
  push rdi      ;save current rdi
  push rsi      ;save current rsi
  push r8       ;save current r8
  push r9       ;save current r9
  push r10      ;save current r10
  push r11      ;save current r11
  push r12      ;save current r12
  push r13      ;save current r13
  push r14      ;save current r14
  push r15      ;save current r15
  push fs
  push gs
%endmacro

%macro POPALL 0
  pop gs
	pop fs
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rsi
  pop rdi
  pop rbp
  pop rdx
  pop rcx
  pop rbx
  pop rax
%endmacro

%macro ENTER 0
    push    rbp
    mov     rbp, rsp
    PUSHALL
%endmacro

%macro LEAVE 0
    POPALL
    mov     rsp, rbp
    pop     rbp
    ret
%endmacro

section .text

;=========================
; _get_idtr
;=========================
; void _get_idtr(void*)
;=========================
; Carga el contenido del
; registro IDTR en rdi
;
; Argumentos
;	- Puntero: rdi
_get_idtr:
	sidt [rdi]
	ret

;=========================
; _load_handler
;=========================
; void _load_handler(byte, ddword, ddword)
;=========================
; Carga un handler en la idt
;
_load_handler:

	mov rax, rsi

	shl rdi, 4			; quickly multiply rdi by 16 to go to the corresponding place in the table
	add rdi, rdx		; adds the base address of the idt table to the place in the table
	stosw						; store the low word (15..0)
	shr rax, 16
	add rdi, 4			; skip the gate marker
	stosw						; store the high word (31..16)
	shr rax, 16
	stosd						; store the high dword (63..32)

	ret

; Handlers

;=======
; page fault interrupt handler
;=======
_page_fault_handler:
  mov     rdi, cr2
  mov     rsi, [esp+8]
  mov     rdx, [esp+16]
  call    on_page_fault
  cli
  hlt
	iretq

;=======
; timer tick interrupt handler
;=======
_tt_interrupt_handler:
	PUSHALL

	mov     rdi, rsp; pass rsp as a parameter

	call 		drop_to_kernel

	mov     rsp, rax; rax has kernel stack stored

	call		on_timer_tick

	call 		back_to_user

	cmp 		rax, 0; rax has user stack or 0
	je 			_do_not_set
  mov 		rsp, rax
_do_not_set:
	mov 		al, 0x20
	out			0x20, al
	POPALL
	iretq

;=======
; keyboard interrupt handler
;=======
_keyboard_interrupt_handler:
  call    on_keyboard
	mov     al, 0x20
	out     0x20, al
  iretq

_backup_interrupt_handler:
  PUSHALL
  mov rdi, rsp
  call drop_to_kernel
  mov rsp, rax

  call back_to_user
  mov rsp, rax
  POPALL

  iretq

;=======
; syscall interrupt handler
;=======
_syscall_interrupt_handler:
; rdi, rsi, rdx, r10, r8  

  call on_syscall

	iretq

;=======
; mouse interrupt handler
;=======
_mouse_interrupt_handler:
;	in al, 60h
;	mov	bl,al

	call		on_mouse

	mov	al,0x20			; Envio de EOI generico al PIC
	out	0xA0, al 		; y al slave
	out	0x20, al
	
	iretq

;=======
; general protection interrupt handler
;=======
_gpf_handler:

  call os_dump_regs

  call on_gpf

  mov rdi, [esp]
  mov rsi, [esp+8]
  mov rdx, [esp+16]
  mov r10, [esp+32]
  mov r8, [esp+40]
  mov r9, [esp+48]
  mov r11, [esp+56]
  mov r12, [esp+64]


  call on_gpf

  cli
  hlt

  iretq

;
; https://github.com/sebikul/UnicOS/blob/master/Kernel/loader.asm#L225
;
_sim_interrupt:

  pop         QWORD[ret_addr]             ;Direccion de retorno

  mov         QWORD[ss_addr],     ss      ;Stack Segment
  push        QWORD[ss_addr]

  push        rsp
  pushf                                   ;Se pushean los flags
  mov         QWORD[cs_addr],     cs      ;Code Segment
  push        QWORD[cs_addr]
  push        QWORD[ret_addr]             ;Direccion de retorno

  PUSHALL
  mov rdi, rsp
  call drop_to_kernel

  mov rsp, rax

  call scheduler_switch

  ; schedule, get new process's RSP and load it
  call back_to_user
  mov rsp, rax

  POPALL
  iretq

;
; https://github.com/sebikul/UnicOS/blob/master/Kernel/loader.asm#L225
;
_ctx_switch:

  pop         QWORD[ret_addr]             ;Direccion de retorno

  mov         QWORD[ss_addr],     ss      ;Stack Segment
  push        QWORD[ss_addr]

  push        rsp
  pushf                                   ;Se pushean los flags
  mov         QWORD[cs_addr],     cs      ;Code Segment
  push        QWORD[cs_addr]
  push        QWORD[ret_addr]             ;Direccion de retorno

  PUSHALL
  mov rdi, rsp
  call drop_to_kernel

  mov rsp, rax

  ; schedule, get new process's RSP and load it
  call back_to_user
  mov rsp, rax

  POPALL
  iretq

section .bss

ret_addr:
        resq 1
cs_addr:
        resq 1
ss_addr:
        resq 1