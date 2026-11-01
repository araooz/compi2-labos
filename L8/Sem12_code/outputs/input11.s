.data
print_fmt:.string "%ld\n" 
.text
.globl main
sumarYVaciar:
pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movq %rdi, -8(%rbp)
movq $0, -16(%rbp)
movq $0, %rax
movq %rax, -16(%rbp)
while_0:
movq -8(%rbp), %rax
cmpq $0, 8(%rax)
sete %al
movzbq %al, %rax
pushq %rax
movq $0, %rax
movq %rax, %rcx
popq %rax
cmpq %rcx, %rax
sete %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_0
movq -16(%rbp), %rax
pushq %rax
movq -8(%rbp), %rax
movq (%rax), %rdi
cmpq $0, %rdi
je .Lpila_vacia
movq 8(%rdi), %rcx
movq %rcx, (%rax)
decq 8(%rax)
pushq (%rdi)
call free@PLT
popq %rax
movq %rax, %rcx
popq %rax
addq %rcx, %rax
movq %rax, -16(%rbp)
jmp while_0
endwhile_0:
movq -16(%rbp), %rax
jmp .Lfin_sumarYVaciar
movq $0, %rax
.Lfin_sumarYVaciar:
leave
ret
main:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movq $16, %rdi
call malloc@PLT
movq $0, (%rax)
movq $0, 8(%rax)
movq %rax, -8(%rbp)
movq $16, %rdi
call malloc@PLT
movq $0, (%rax)
movq $0, 8(%rax)
movq %rax, -16(%rbp)
movq $0, -24(%rbp)
movq $1, %rax
movq %rax, -24(%rbp)
while_1:
movq -24(%rbp), %rax
pushq %rax
movq $5, %rax
movq %rax, %rcx
popq %rax
cmpq %rcx, %rax
setle %al
movzbq %al, %rax
cmpq $0, %rax
je endwhile_1
movq -8(%rbp), %rax
pushq %rax
movq -24(%rbp), %rax
pushq %rax
movq $10, %rax
movq %rax, %rcx
popq %rax
imulq %rcx, %rax
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
movq -24(%rbp), %rax
pushq %rax
movq $1, %rax
movq %rax, %rcx
popq %rax
addq %rcx, %rax
movq %rax, -24(%rbp)
jmp while_1
endwhile_1:
movq -8(%rbp), %rax
movq 8(%rax), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -8(%rbp), %rax
movq (%rax), %rax
cmpq $0, %rax
je .Lpila_vacia
movq (%rax), %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
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
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -8(%rbp), %rax
pushq %rax
popq %rdi
call sumarYVaciar
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -8(%rbp), %rax
cmpq $0, 8(%rax)
sete %al
movzbq %al, %rax
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -16(%rbp), %rax
pushq %rax
movq $1, %rax
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
movq -16(%rbp), %rax
pushq %rax
movq $3, %rax
pushq %rax
movq $4, %rax
movq %rax, %rcx
popq %rax
cmpq %rcx, %rax
setg %al
movzbq %al, %rax
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
movq -16(%rbp), %rax
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
movq %rax, %rsi
leaq print_fmt(%rip), %rdi
movl $0, %eax
call printf@PLT
movq -16(%rbp), %rax
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
