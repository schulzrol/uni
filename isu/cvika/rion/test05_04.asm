; TEST PROSEL
%include "rw32-2018.inc"

section .data

section .text
CMAIN:
    mov ebp, esp; for correct debugging
    push ebp
    mov ebp,  esp
    ; predpoklad
    mov ax, 5 ; 16 bitu, maska je n>>15 aka 2^15
    
    ; pouze MOV, ADD, XOR, SUB, NOT, AND, OR a NEG
    mov bx, ax ; zkopirovani n -> bx
    
    ; potrebuji pro:
    ; zaporne    masku -1
    ; nezaporne  masku  0
    
    xor edx, edx
    mov dx, bx ; maska v dx
    
    and dx, 0b1000000000000000 ;                    1111111111111111
        ; -> pokud v dx je zaporne cislo,   dx == 0b1000000000000000
        ;    pokud v dx je NEzaporne cislo, dx == 0b0000000000000000
        
        
    xor dl, dh
    xor dh, dl
    xor dl, dh
    
    not dx
    add dx, 1
    
    or dl, dh
    
    ; potrebuji pro:
    ; zaporne    masku 0b1111111111111111
    ; nezaporne  masku 0b0000000000000000
    ;(mask + num)^mask 
    add bx, dx
    xor bx, dx
    
    ; konec
    movzx eax, bx
    call WriteInt32
    call WriteNewLine
    
    pop ebp
    ret