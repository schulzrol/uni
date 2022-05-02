%include "rw32-2018.inc"

section .data
string db "Hello World!",EOL,0

section .text
CMAIN:
    mov esi, string
    call WriteString
    
    
    mov edi, string
    mov al, '!'
    mov ecx, 13
    repne scasb
    mov eax, 13
    sub eax, ecx
    dec eax
    call WriteInt32
    call WriteNewLine
    cmp eax, 12
    jne je_in
    mov eax, 0
    call WriteInt32
    xor eax, eax
    ret
    
je_in:
    mov eax, 1
    call WriteInt32
    xor eax, eax
    ret