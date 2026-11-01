.data
print_fmt:.string "%ld\n" 
.text
.globl main
mayor:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -16(%rbp)
movq $0, -24(%rbp)
movq $0, -32(%rbp)
movq -8(%rbp), %rax
pushq %rax
movq $0, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
movq %rax, -32(%rbp)
movq $1, %rax
movq %rax, -24(%rbp)
while_0:
movq -24(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
movq %rax, %rcx
popq %rax
cmpq %rcx, %rax
setl %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_0
movq -8(%rbp), %rax
pushq %rax
movq -24(%rbp), %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
pushq %rax
movq -32(%rbp), %rax
movq %rax, %rcx
popq %rax
cmpq %rcx, %rax
setg %al
movzbq %al, %rax
cmpq $0, %rax
je else_1
movq -8(%rbp), %rax
pushq %rax
movq -24(%rbp), %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
movq %rax, -32(%rbp)
jmp endif_1
else_1:
endif_1:
movq -24(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
addq %rcx, %rax
movq %rax, -24(%rbp)
jmp while_0
endwhile_0:
movq -32(%rbp), %rax
jmp .Lfin_mayor
movq $0, %rax
.Lfin_mayor:
leave
ret
main:
pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movq $0, -8(%rbp)
movq $4, %rax
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
movq $3, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $9, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $2, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $4, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $3, %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
pushq %rax
movq $1, %rax
popq %rcx
movq %rax, (%rcx)
movq -8(%rbp), %rax
pushq %rax
movq $4, %rax
pushq %rax
popq %rsi
popq %rdi
call mayor
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -8(%rbp), %rax
movq %rax, %rdi
call free@PLT
movq $0, %rax
.Lfin_main:
movq $0, %rax
leave
ret
.section .note.GNU-stack,"",@progbits
