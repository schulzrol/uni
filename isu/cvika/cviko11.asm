%include "rw32-2018.inc"

section .data
    valA dd 2.0
    valC dd 6.0
    valV dd 4.0
    S dd 0
    dva dd 2.0

section .text
%define f32(x) __float32__(x)

;__cdecl double objem_koule(float r){return pi*(r^3)*4.0/3.0}
%define r ebp+8
objem_koule:
    enter 0, 0
    
    finit
    fld dword [r] ; r
    fld dword [r] ; r r
    fmul st0, st0 ; r^2 r
    fmulp st1, st0 ; r^3
    fldpi ; pi r^3
    fmulp st1, st0 ; pi*(r^3)
    push dword f32(4.0)
    push dword f32(3.0)

    fld dword [ebp-4] ; 4 pi*(r^3)
    fld dword [ebp-8] ; 3 4 pi*(r^3)
    fdivr ; 4/3 pi*(r^3)
    fmulp ;st0, st1 ;4/3*pi*(r^3)
            
    leave
    ret

; __cdecl double square(double x)
square:
    enter 0, 0
    finit
    fld qword [ebp+8] ; x
    fmul st0, st0 ; x*x
    leave
    ret

obsah_lichobezniku:
; S = ((a+c)/2)*v
; __cdecl int obsah_lichobezniku(float a, float c, float v)
%define a ebp+8
%define c ebp+12
%define v ebp+16
    enter 4, 0 ; 1 lokalni promenna
    ; push ebp
    ; mov ebp, esp
    ; sub esp, 4
    
    push dword f32(2.0)
    ;push dword 2 ; pro fild
    
    finit
    
;    fld dword [a] ; a  
;    fld dword [c] ; c a
;    faddp         ; c+a
    
    fld dword [a]  ; a
    fadd dword [c] ; a+c
    
    fld1           ; 1 a+c
    fadd st0, st0  ; 2 a+c
    ; fld dword [dva]; 2 a+c
    ; fld dword [ebp-4]; 2 a+c
    ; fild dword [ebp-4]
    fdivp st1, st0 ;(a+c)/2
    
    fmul dword [v] ;v*(a+c)/2
    
    fistp dword [ebp-4]
    mov eax, [ebp-4]
    
    ; mov esp, ebp
    ; pop ebp
    leave
    ret

CMAIN:
    mov ebp, esp; for correct debugging
    push ebp
    mov ebp,  esp
    
    push dword [valV]
    push dword [valC]
    push dword [valA]
    call obsah_lichobezniku  
    add esp, 12
    
    call WriteInt32
    call WriteNewLine
    
    ; nahrani 2.5 pres dva argumenty
    finit
    push eax
    push dword f32(2.5)
    fld dword [esp]
    fstp qword [esp]
    
    call square
    add esp, 8 ; 8 protoze qword jako dva parametry dword o 4
    call WriteDouble
    call WriteNewLine
    
    push dword f32(5.0)
    call objem_koule
    add esp, 4
    call WriteDouble
    call WriteNewLine

    pop ebp
    ret
