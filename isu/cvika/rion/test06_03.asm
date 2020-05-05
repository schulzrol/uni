%include "rw32-2018.inc"

section .data

section .text
CMAIN:
    mov ebp, esp; for correct debugging
    push ebp
    mov ebp,  esp

    ;bordel
    mov ecx, 3
    mov edx, 0
    mov eax, -1

    ;predpoklad
    mov ax, 50
    mov bx, 10

    
    ;reseni pouze instrukce ADD, SUB a LOOP
    sub ecx, ecx
    sub edx, edx
    add dx, ax
    sub eax, eax
    add ax, dx
    sub edx, edx

    add cx, bx
    ;add cx, 1
    ;zadam:
    ;   0 - nice
    ;   1 - nice
    ;  -1 - nice
    loop nula_nebo_nenulove_krom_1
    ; 1 ; ecx == 0
    add ecx, 2 ; ecx == 2
    loop nasobeni
    
    nula_nebo_nenulove_krom_1:
    add ecx, 2 ; -1 +2 = 1 / -2 +2 = 0
    loop nasobeni
    ;je nula
    add ecx, 2
    sub edx, 1
    loop konec
    ;sub edx, edx
    nasobeni: ; nenulove krom 1 / 1
      add edx, eax
      loop nasobeni
    
    konec:
    add ecx, edx

    ; konec
    mov eax, ecx

    call WriteInt32
    call WriteNewLine
    pop ebp
    ret