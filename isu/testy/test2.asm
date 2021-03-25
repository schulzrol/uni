%include "rw32-2018.inc"

section .data
string db "Hello World!",EOL,0

section .text
CMAIN:
    mov ebp, esp; for correct debugging
    
    
    mov al, 96
    mov bl, 7 
    mov cl, 10  
    mul cl ;ax = 960
    ; 1111000000 960
    
    ; ah = 3
    add ah, bl
    mov DL, 100
    div dl ;ax = 13339   ; al = ax/100; ah = ax%bl
    ; 00110100 00011011  13339
    ;       52 27
    ; 00001010 11000000 2752
    ; 00000111
    ; 00000011 11000000 960
    
    inc ax
    ; ax = 13340
    ; 00110100 00011100
    ;       52 28
    add al, ah
    ;al = 52+28 = 80
    ; 01010000 80 v al
    sub al, bl
    ; al = 80-7=73
    ; 1001001 73 v al
    cmp al, 0 ;!!!!!!!!!!!!
    
    jae hop1
    add al, 0
hop1:
    ; 01001001 73
    ;+10000000
    ;=11001001 -55
    ;al = -55

    xor eax, eax
    ret