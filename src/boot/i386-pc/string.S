.code32
.section .stage2.util, "ax"

.global root_memcpy
.global root_memset
.global root_memsetw
.global root_memsetdw

root_memcpy:
    sub $8, %esp
    mov %esi, 0(%esp)
    mov %edi, 4(%esp)
    mov 12(%esp), %edi
    mov 16(%esp), %esi
    mov 20(%esp), %ecx
    rep movsb
    mov 0(%esp), %esi
    mov 4(%esp), %edi
    add $8, %esp
    ret

root_memset:
    sub $4, %esp
    mov %edi, 0(%esp)
    mov 8(%esp), %edi
    mov 12(%esp), %eax
    mov 16(%esp), %ecx
    rep stosb
    mov 0(%esp), %edi
    add $4, %esp
    ret

root_memsetw:
    sub $4, %esp
    mov %edi, 0(%esp)
    mov 8(%esp), %edi
    mov 12(%esp), %eax
    mov 16(%esp), %ecx
    rep stosw
    mov 0(%esp), %edi
    add $4, %esp
    ret

root_memsetdw:
    sub $4, %esp
    mov %edi, 0(%esp)
    mov 8(%esp), %edi
    mov 12(%esp), %eax
    mov 16(%esp), %ecx
    rep stosl
    mov 0(%esp), %edi
    add $4, %esp
    ret