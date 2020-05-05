; TEST PROSEL
%include "rw32-2018.inc"

section .data

section .text
CMAIN:
    mov ebp, esp; for correct debugging
    push ebp
    mov ebp,  esp

    
    mov eax, 0x44332211
    ;na      0x33441122
    ; pouze instrukce ROL nebo ROR
    ror ax, 8
    ror eax, 16
    ror ax, 8
    ror eax, 16

    pop ebp
    ret