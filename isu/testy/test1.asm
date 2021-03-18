%include "rw32-2018.inc"

section .data
    a db 10
    b dd 20
    c dw 10
    d dw 7
    e dw 7
    q dd 0
    r dd 0

section .text
CMAIN:
    mov ebp, esp; for correct debugging
    push ebp
    mov ebp,esp
        
        mov eax,0x44332211
    call task11
    
    call task12
    
    pop ebp
    ret
    
;
;--- Task 1 ---
;
; Create a function "task11" to swap bytes X1, X2, X3, X4 within the register EAX this way:
;
; original value: EAX = X4 X3 X2 X1
; result        : EAX = X2 X4 X1 X3 
;
; The argument to the function is passed in the register EAX and it returns the result 
; in the register EAX. The least significant byte is X1, the most significant byte is X4.
;
; Arguments:
;    - EAX = 32bit value
;
; Result:
;    - EAX = result
;    - the function does not have to preserve content of any register
;
task11:
    ; insert your code here
    ; X4 X3 X2 X1 - zadani
    ; X4 X3 X1 X2 - xchg al ah
    ; x2 x4 x3 x1 - ror 8
    ; x2 x4 x1 x3 - xchg al ah
    ; X2 X4 X1 X3 - konec
    xchg al, ah
    ror eax, 8
    xchg al, ah
    ret
;
;--- Task 2 ---
;
; Create a function "task12" to evaluate the following expression using SIGNED integer arithmetic:
;
; q = (a + b*c - 25)/(10*d + e - 115) ... division quotient
; r = (a + b*c - 25)%(10*d + e - 115) ... division remainder 
;
; The arguments a, b, c, d and e are stored in the memory and are defined as follows:
;
;    [a] ...  8bit signed integer
;    [b] ... 32bit signed integer
;    [c] ... 16bit signed integer
;    [d] ... 16bit signed integer
;    [e] ... 16bit signed integer
;
; Store the result to the memory at the addresses q and r this way:
;
;    [q] ... lower 32 bits of the division quotient (32bit signed integer)
;    [r] ... division remainder (32bit signed integer)
;
; Important notes:
;  - do NOT take into account division by zero 
;  - the function does not have to preserve the original content of the registers
task12:
    ; insert your code here
    movsx ebx, word [e]   ; ebx = e
    movsx eax, word [d]   ; eax = d
    xor ecx, ecx
    mov ecx, 10           ; ecx = 10
    imul ecx              ; edx:eax = d*10
    add eax, ebx          ; edx:eax ~= d*10+e
    adc edx, 0            ; edx:eax = d*10+e with carry
    sub eax, 115          ; edx:eax ~= d*10+e-115
    sbb edx, 0            ; edx:eax = d*10+e-115 with borrow
    ; vysledek se realne vejde jen do eax
    mov ebx, eax          ; ebx = d*10+e-115
        
    mov eax, [b]          ; eax = b
    movsx ecx, word [c]   ; ecx = c
    imul ecx              ; edx:eax = b*c
    movsx ecx, byte [a]   ; ecx = a
    add eax, ecx          ; edx:eax ~= b*c+a
    adc edx, 0            ; edx:eax = b*c+a with carry
    sub eax, 25           ; edx:eax ~= b*c+a-25
    sbb edx, 0            ; edx:eax ~= b*c+a-25 with borrow
    
    idiv ebx              ; eax:=b*c+a-25 / d*10+e-115
                          ; zbytek v edx
    
    mov [q], eax
    mov [r], edx
    
    ret

