%include "rw32-2018.inc"

section .data
string db "Hello World!",EOL,0

section .text
CMAIN:
    push ebp
    mov ebp,  esp
    
    call WriteString

    pop ebp
    ret