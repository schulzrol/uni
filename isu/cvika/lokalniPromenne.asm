%include "rw32-2018.inc"

section .data
string db "Hello World!",EOL,0

section .text
CMAIN:
    push ebp
    mov ebp, esp
    
    call mojesuma
    call WriteInt32
    call WriteNewLine
    call mojesuma
    call WriteInt32

    pop ebp
    ret
    
mojesuma:
    push ebp
    mov ebp, esp
    
    ; 3 lokalni promenne
    sub esp, 12
    
    call ReadInt32
    mov [ebp-4], eax
    call ReadInt32
    mov [ebp-8], eax
    call ReadInt32
    mov [ebp-12], eax
    

    xor eax, eax
    add eax, [ebp-4]
    add eax, [ebp-8]
    add eax, [ebp-12]
    
    ; navraceni kvuli lokalnim promennym
    add esp, 12
    
    pop ebp
    ret