.set MEM, 0x1badb002
.set FLG, (1 << 0 | 1 << 1)
.set CKS, -(MEM + FLG)

.section .multiboot
    .long MEM
    .long FLG
    .long CKS

.section .text
.extern kernelMain
.extern callConstructors
.global boot

boot:
    mov $kernel_stack, %esp
    call callConstructors
    push %eax
    push %ebx
    call kernelMain

_stop:
    cli
    hlt
    jmp _stop

.section .bss
.space 2 * 1024 * 1024  ; # 2 MiB
kernel_stack:
