; TEST PROSEL
%include "rw32-2018.inc"

section .data

section .text
;_stdcall int secti(int x, int y, int z);
; navratova v eax
%define x ebp+8
%define y ebp+12
%define z ebp+16
secti:
    push ebp
    mov ebp,  esp

    mov eax, [x]
    add eax, [y]
    add eax, [z]

    pop ebp
    ret 4*3

CMAIN:
    push ebp
    mov ebp,  esp

    push dword 10
    push dword 20
    push dword 30
    call secti
    call WriteInt32
    call WriteNewLine

    pop ebp
    ret