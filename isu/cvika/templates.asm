; .DATA SEKCE
section .data
string db "Hello World!",EOL,0
valA dd 2.0

; ZASOBNIKOVY RAMEC
push ebp
mov ebp, esp
; sub esp, 4 ; uvolni misto pro 1 dword lokalni promennou
...
pop ebp
ret ;u pascal volani se ret predava pocet bytu co uvolnit
; NEBO
enter 0, 0 ; prvni argument udava pocet bytu pro lokalni promenne
; takze enter 4,0 znamena 1 lokalni promennou dword
...
leave
ret

; NAHRANI DO FPU CISLA MAKREM
%define f32(x) __float32__(x)
push dword f32(2.0)
;push dword 2 ; pro fild

; LOKALNI PROMENNE
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
    
; VOLANI CDECL
CMAIN: neco jako 
    call ReadInt32
    push eax
    call ReadInt32
    push eax
    call ReadInt32
    push eax
    
    call MaxIndex
    add esp, 4*3
    
MaxIndex:
    push ebp
    mov ebp, esp
    ; predavani CDECL
    mov eax, [ebp+8]
    mov ebx, [ebp+12]
    mov ecx, [ebp+16]
    
    cmp eax, ebx
    jl bGreater
    cmp eax, ecx
    jl cGreater
    mov eax, 1
    jmp konec
 bGreater:
    cmp ebx, ecx
    jl cGreater
    mov eax, 2
    jmp konec
 cGreater:    
    mov eax, 3
 konec:
    pop ebp
    ret
    
; CDECL VOLANI
; __cdecl int obsah_lichobezniku(float a, float c, float v)
%define a ebp+8
%define c ebp+12
%define v ebp+16