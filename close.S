.global close
close:
mov $3, %rax
syscall
ret
.section .note.GNU-stack
