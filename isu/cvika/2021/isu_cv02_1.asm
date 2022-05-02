%include "rw32-2018.inc"

; Cviceni c.2
; Vyzkousejte prelozit, spustit a zdebuggovat nasledujici kod
; Kod muzete jakkoliv menit a hrat si s nim
; U nekterych radku jsou otazky. Odpovezte na ne a potom je projdeme spolecne. 
; Muzete si radit, diskutovat problem a pouzivat materialy z prednasek a cviceni.

; K cemu slouzi sekce .data?
section .data
    sMessage     db "Hello World!",EOL,0
    sMessage2    db "Vitejte na druhem cviceni.",EOL,0
    print_num    dd 123456789
    good_msg     db "GOOD!",EOL,0
    wrong_msg    db "WRONG!",EOL,0
    expected_res dw 0xB105, 0x4B1D
    reg_a_value  dw 0xFEED
 
; K cemu slouzi sekce .bss?
section .bss
    result resw 2 ;pole o 2 prvcich, rezervace mista v pameti pomoci velikosti a poctu
         
section .text
CMAIN:
    mov ebp, esp; for correct debugging
    push ebp
    mov ebp,esp

    ; priklad vypisu retezce
    ; zkuste si v debuggeru zobrazit celou zpravu sMessage
    ; pak si zkuste v debuggeru zobrazit tuto zpravu pomoci registru esi
    mov esi,sMessage ; proc msg do esi? Podivejte se, jak funguje funkce WriteString
    call WriteString
    mov esi,sMessage2
    call WriteString
    
    ; priklad vypisu cisla
    mov eax, [print_num]
    call WriteUInt32
    call WriteNewLine
    
    ; Vase dnesni uloha
    
    ;Prvni cast vysledku
    ;prvni operand dostanete po prevedeni 2koveho cisla 0101101101001010 do 10kove soustavy    
    mov ax, 0101101101001010b ;nahradte cislo 0 prvnim operandem
    
    ;druhy operand dostanete po prevedeni 16koveho cisla 55BB do 10kove soustavy
    add ax, 55BBh ;nahradte cislo 0 druhym operandem
    mov [result], ax ;ulozeni vysledku do pole na index 0
    
    ;Druha cast vysledku
    ;prvni operand - prace s debuggerem, podivejte co se stane s hodnotou v registru ax,
    ; po ulozeni hodnoty 74 do registru al
    mov ax, [reg_a_value]
    mov al, 74

    ;prvni operand ziskate tak, ze na 8 az 15 bit registru eax ulozite cislo 37
    ;Napoveda: $ah, $al. Pozorujte zmeny v registrech, jak se meni hodnoty v registrech (eax,ax,ah,al)?    
    ;Zde doplnte kod
    mov ah, 37
    
    ;druhy operand - prevedte 2koveho cislo z doplnkoveho kodu 1101101000101101 do 10kove soustavy
    sub ax, -9683 ;nahradte cislo 0 druhym operandem
    mov [result+2*1], ax ;ulozeni vysledku do pole na index 1
      
    call CheckResult
    
    ;Zde doplnte kod pro vypsani vysledku v hexa.
    ;Napoveda: Podivejte se po odpovidajicich funkcich v rw32-2018.inc
    call WriteHex16

    xor eax, eax
    
    mov esp, ebp
    pop ebp

    ret
    
    
CheckResult: 
    pusha           ; odlozeni registru
       
    mov ax, [result]
    mov bx, [expected_res]
    cmp ax, bx
    jne .wrong
    mov ax, [result+2*1]
    mov bx, [expected_res+2*1]
    cmp ax, bx
    je .good
.wrong:
    mov esi,wrong_msg
    call WriteString
    
    jmp .return
.good:
    mov esi,good_msg
    call WriteString
    
.return:
    popa           ; obnoveni registru
    ret
    
