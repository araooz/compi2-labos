.data
print_fmt:.string "%ld\n" 
.text
.globl main
main:
pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movq $16, %rdi
call malloc@PLT
movq $0, (%rax)
movq $0, 8(%rax)
movq %rax, -8(%rbp)
movq -8(%rbp), %rax
pushq %rax
movq $4, %rax
pushq %rax
movq $16, %rdi
call malloc@PLT
popq %rcx
popq %rdx
movq %rcx, (%rax)
movq (%rdx), %rcx
movq %rcx, 8(%rax)
movq %rax, (%rdx)
incq 8(%rdx)
movq -8(%rbp), %rax
pushq %rax
movq $5, %rax
pushq %rax
movq $16, %rdi
call malloc@PLT
popq %rcx
popq %rdx
movq %rcx, (%rax)
movq (%rdx), %rcx
movq %rcx, 8(%rax)
movq %rax, (%rdx)
incq 8(%rdx)

movq -8(%rbp), %rax
pushq %rax
movq $6, %rax
pushq %rax
movq $16, %rdi
call malloc@PLT
popq %rcx
popq %rdx
movq %rcx, (%rax)
movq (%rdx), %rcx
movq %rcx, 8(%rax)
movq %rax, (%rdx)
incq 8(%rdx)
movq -8(%rbp), %rax
movq (%rax), %rdi
cmpq $0, %rdi
je .Lpila_vacia
movq 8(%rdi), %rcx
movq %rcx, (%rax)
decq 8(%rax)
pushq (%rdi)
subq $8, %rsp
call free@PLT
addq $8, %rsp
popq %rax
movq -8(%rbp), %rax
movq (%rax), %rax
cmpq $0, %rax
je .Lpila_vacia
movq (%rax), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq $0, %rax
.Lfin_main:
movq $0, %rax
leave
ret
.Lpila_vacia:
andq $-16, %rsp
movq stderr@GOTPCREL(%rip), %rax
movq (%rax), %rdi
leaq pila_vacia_msg(%rip), %rsi
movl $0, %eax
call fprintf@PLT
movl $1, %edi
call exit@PLT
.data
pila_vacia_msg:.string "Error: pop/top sobre una pila vacia\n"
.section .note.GNU-stack,"",@progbits
