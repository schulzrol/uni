; TEST PROSEL
%include "rw32-2018.inc"

section .data

section .text
CMAIN:
    mov ebp, esp; for correct debugging
    push ebp
    mov ebp,  esp

    mov ax, -5

    mov bx, ax
    bylo_kladne:
    neg bx
    jl bylo_kladne
    
    pop ebp
    ret