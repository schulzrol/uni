%include "rw32-2018.inc"

section .data
S  db "Hello",0


section .text
CMAIN:
    push ebp
    mov ebp,  esp
    
    mov eax, S
    ; zacatek
    mov edx, eax
    
    mov ebx, 0
    mov ecx, -1
    
    cld
    
    mov edi, eax
    mov eax, 0
    
    ; detekovat nulu
    
    repne scasb
    jz naslo
    
    naslo:
    
    mov eax, edx
    ;konec

    pop ebp
    ret