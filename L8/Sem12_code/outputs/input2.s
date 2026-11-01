.data
print_fmt:.string "%ld\n" 
.text
.globl main
intercambiar:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -16(%rbp)
movq $0, -24(%rbp)
movq -8(%rbp), %rax
movq (%rax), %rax
movq %rax, -24(%rbp)
movq -8(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
movq (%rax), %rax
popq %rcx
movq %rax, (%rcx)
movq -16(%rbp), %rax
pushq %rax
movq -24(%rbp), %rax
popq %rcx
movq %rax, (%rcx)
movq $0, %rax
jmp .Lfin_intercambiar
movq $0, %rax
.Lfin_intercambiar:
leave
ret
main:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movq $0, -8(%rbp)
movq $0, -16(%rbp)
movq $0, -24(%rbp)
movq $1, %rax
movq %rax, -8(%rbp)
movq $2, %rax
movq %rax, -16(%rbp)
leaq -8(%rbp), %rax
pushq %rax
leaq -16(%rbp), %rax
pushq %rax
popq %rsi
popq %rdi
call intercambiar
movq %rax, -24(%rbp)
movq -8(%rbp), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -16(%rbp), %rax
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
