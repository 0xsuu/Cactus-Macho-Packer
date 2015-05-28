
bits 64

global start

start:
    mov rax, 0x200001a
    xor rdi, rdi
    mov dil, 0x1f
    xor rsi, rsi
    xor rdx, rdx
    xor rcx, rcx
    syscall
    
