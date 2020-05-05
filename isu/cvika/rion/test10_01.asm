%include "rw32-2018.inc"

section .data

section .text
CMAIN:
    mov ebp, esp; for correct debugging
    push ebp
    mov ebp,  esp
    
    mov ah, 256 ; vyska
    mov al, 256 ; polomer
    
    ; obvod podstavy = 2*pi*r => kdyz obs_pl > obs_po
    ; obsah plaste   = 2*pi*r*v => prvni
    ; obsah podstavy = pi*(r*r) => prvni
    ; objem valce    = pi*(r*r)*v => kdyz ne obvod_podstavy
    
    xor ebx, ebx
    mov bl, al
    push ebx
    
    mov bl, ah
    push ebx
    
    %define vyska ebp-8
    %define polomer ebp-4
    finit
    ; vypocet obsahu plaste
    fild dword [vyska]    ; v
    fimul dword [polomer] ; r*v
    fld1                  ; 1 r*v
    fadd st0, st0         ; 2 r*v
    fmulp st1, st0        ; 2*r*v
    fldpi                 ; pi 2*r*v
    fmulp st1, st0        ; pi*2*r*v
    
    ; vypocet obsahu podstavy
    fild dword [polomer]  ; r pi*2*r*v
    fimul dword [polomer] ; r*r pi*2*r*v
    fldpi                 ; pi r*r pi*2*r*v
    fmulp st1, st0        ; pi*r*r pi*2*r*v
    ; st0 => obsah podstavy
    ; st1 => obsah plaste
    
    ;st0 < st1 ? True: vypsat obvod podstavy
    ;            False: vypsat objem valce
    
    ;fxch st1 ; DEBUGGING
    fcomi st1
    jnc p_objvalce; st0 vetsi nebo rovno st1
    ; mensi
    p_obvpod:
    fxch st1            ; pi*2*r*v pi*r*r
    fidiv dword [vyska] ; pi*2*r pi*r*r
    jmp konec
    
    p_objvalce:
    ; vetsi
    fimul dword [vyska] ; pi*r*r*v pi*2*r*v

    konec:
    call WriteDouble
    fstp st1
    add esp, 4*2
    
    pop ebp
    ret