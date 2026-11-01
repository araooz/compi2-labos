.data
print_fmt:.string "%ld\n" 
.text
.globl main
main:
pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movq $0, -8(%rbp)
movq $2, %rax
pushq %rax
movq $8, %rax
movq %rax, %rcx
popq %rax
imulq %rcx, %rax
movq %rax, %rdi
call malloc@PLT
movq %rax, -8(%rbp)
movq -8(%rbp), %rax
pushq %rax
movq $0, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $2, %rax
pushq %rax
movq $8, %rax
movq %rax, %rcx
popq %rax
imulq %rcx, %rax
movq %rax, %rdi
subq $8, %rsp
call malloc@PLT
addq $8, %rsp
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $2, %rax
pushq %rax
movq $8, %rax
movq %rax, %rcx
popq %rax
imulq %rcx, %rax
movq %rax, %rdi
subq $8, %rsp
call malloc@PLT
addq $8, %rsp
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $0, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
pushq %rax
movq $0, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $1, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $0, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $2, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
pushq %rax
movq $0, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $3, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $4, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $0, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
pushq %rax
movq $0, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
pushq %rax
movq -8(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
movq %rax, %rcx
popq %rax
addq %rcx, %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -8(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
pushq %rax
movq $0, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -8(%rbp), %rax
pushq %rax
movq $0, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
movq %rax, %rdi
call free@PLT
movq -8(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
movq %rax, %rdi
call free@PLT
movq -8(%rbp), %rax
movq %rax, %rdi
call free@PLT
movq $0, %rax
.Lfin_main:
movq $0, %rax
leave
ret
.section .note.GNU-stack,"",@progbits
