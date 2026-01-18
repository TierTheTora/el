.global write
write:
mov $1, %rax
syscall
ret

.section .note.GNU-stack
