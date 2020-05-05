%include "rw32-2018.inc"

section .data

section .text
CMAIN:
    push ebp
    mov ebp, esp
    
    call ReadInt32
    push eax
    call fib
    call WriteInt32

    add esp, 4
    pop ebp
    ret
    
fib:
    push ebp
    mov ebp, esp

    sub esp, 4
        
    mov eax, [ebp+8]
    ; argument v eax
    cmp eax, 0
    je konec
    cmp eax, 1
    je konec
    ; n-1
    dec eax 
    push eax
    call fib
    mov [ebp-4], eax
    pop eax
    dec eax
    push eax
    call fib
    add eax, [ebp-4]
    add esp,4    
konec:
    
    add esp, 4  
    pop ebp
    ret