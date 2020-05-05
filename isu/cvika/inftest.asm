%include "rw32-2018.inc"

section .data
inf dq __Infinity__
    

section .text
CMAIN:
                mov ebp, esp; for correct debugging
    push ebp
    mov ebp,  esp
    
    finit
    fld qword [inf]
    call WriteFloat

    pop ebp
    ret