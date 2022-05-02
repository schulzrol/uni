task22:
    
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx

    mov eax, [ebp+8] ; kontrola pA
    cmp eax, 0
    je zero_return
    mov eax, [ebp+12]
    cmp eax, 0 ; kontrola N
    jle zero_return
    
    ; v pohode
    mov ebx, [ebp+8] ; pa
    mov ecx, [ebp+12]; N
    mov ax, [ebp+16] ; x
    
loopjump2:
    cmp ax, [ebx]
    je nenasli
    mov edx, ebx
nenasli:
    lea ebx, [ebx+2]
    loop loopjump2

zero_return:
    mov eax, 0
    pop edx
    pop ecx
    pop ebx
    mov esp, ebp
    pop ebp
    ret
    
value_return:
    mov eax, edx
    
    pop edx
    pop ecx
    pop ebx
    mov esp, ebp
    pop ebp
    ret