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
movq $3, %rax
movq %rax, -16(%rbp)
movq -16(%rbp), %rax
pushq %rax
movq $8, %rax
movq %rax, %rcx
popq %rax
imulq %rcx, %rax
movq %rax, %rdi
call malloc@PLT
movq %rax, -8(%rbp)
movq $0, %rax
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
pushq %rax
movq -16(%rbp), %rax
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
movq $0, %rax
movq %rax, -32(%rbp)
while_1:
movq -32(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
movq %rax, %rcx
popq %rax
cmpq %rcx, %rax
setl %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_1
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
leaq (%rax,%rcx,8), %rax
pushq %rax
movq -24(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
movq %rax, %rcx
popq %rax
imulq %rcx, %rax
pushq %rax
movq -32(%rbp), %rax
movq %rax, %rcx
popq %rax
addq %rcx, %rax
popq %rcx
movq %rax, (%rcx)
movq -32(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
addq %rcx, %rax
movq %rax, -32(%rbp)
jmp while_1
endwhile_1:
movq -24(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
addq %rcx, %rax
movq %rax, -24(%rbp)
jmp while_0
endwhile_0:
movq $0, %rax
movq %rax, -24(%rbp)
while_2:
movq -24(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
movq %rax, %rcx
popq %rax
cmpq %rcx, %rax
setl %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_2
movq $0, %rax
movq %rax, -32(%rbp)
while_3:
movq -32(%rbp), %rax
pushq %rax
movq -16(%rbp), %rax
movq %rax, %rcx
popq %rax
cmpq %rcx, %rax
setl %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_3
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
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -32(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
addq %rcx, %rax
movq %rax, -32(%rbp)
jmp while_3
endwhile_3:
movq -8(%rbp), %rax
pushq %rax
movq -24(%rbp), %rax
movq %rax, %rcx
popq %rax
leaq (%rax,%rcx,8), %rax
movq (%rax), %rax
movq %rax, %rdi
call free@PLT
movq -24(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
addq %rcx, %rax
movq %rax, -24(%rbp)
jmp while_2
endwhile_2:
movq -8(%rbp), %rax
movq %rax, %rdi
call free@PLT
movq $0, %rax
.Lfin_main:
movq $0, %rax
leave
ret
.section .note.GNU-stack,"",@progbits
