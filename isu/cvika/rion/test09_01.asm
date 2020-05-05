; TEST PROSEL
%include "rw32-2018.inc"

section .data

section .text
;__cdecl int schedule(unsigned char netflix, short chill);
; vystup v EAX
%define netflix ebp+8
%define chill ebp+12
schedule:
    push ebp
    mov ebp, esp
    
    xor eax, eax
    xor ecx, ecx
    mov ax, [ebp+8]
    mov cl, [ebp+12]
    and eax, ecx
    
    pop ebp
    ret

CMAIN:
    mov ebp, esp; for correct debugging
    push ebp
    mov ebp, esp
    
    push dword 1 ; chill
    push dword 3 ; netflix
    call schedule    
    add esp, 4*2
    
    pop ebp
    ret