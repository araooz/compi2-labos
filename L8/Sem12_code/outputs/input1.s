.data
print_fmt:.string "%ld\n" 
.text
.globl main
main:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movq $0, -8(%rbp)
movq $0, -16(%rbp)
movq $0, -24(%rbp)
movq $0, -32(%rbp)
movq $5, %rax
movq %rax, -8(%rbp)
leaq -8(%rbp), %rax
movq %rax, -24(%rbp)
movq $100, %rax
movq %rax, -16(%rbp)
leaq -16(%rbp), %rax
movq %rax, -32(%rbp)
movq -24(%rbp), %rax
movq (%rax), %rax
pushq %rax
movq -32(%rbp), %rax
movq (%rax), %rax
movq %rax, %rcx
popq %rax
addq %rcx, %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq $0, %rax
.Lfin_main:
movq $0, %rax
leave
ret
.section .note.GNU-stack,"",@progbits
