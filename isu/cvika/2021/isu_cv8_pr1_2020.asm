%include "rw32-2018.inc"

extern _strlen ; size_t strlen( const char *str );
extern _putchar ; void* malloc( size_t size );
extern _printf ; int printf( const char *format, ... );

; Cviceni c.8 Priklad 1
; Kod muzete jakkoliv menit a hrat si s nim
; Muzete si radit, diskutovat problem a pouzivat materialy z prednasek a cviceni.

section .data
    text1       db  "Sofistikovany ISU text", 0
    text2       db  "Jiny text", 0    
    
    dlouhy_exit_str db "dlouhy", 0
    kratky_exit_str db "kratky", 0
    
section .bss
    
    
section .text
main:
    mov ebp, esp; for correct debugging
    push ebp
    mov ebp, esp
    
    push text1
    call dlouhy_text
    add esp, 4*2
    call WriteNewLine
    
    push text2
    call dlouhy_text
    add esp, 4*2
    call WriteNewLine
    
    push text1
    call kopiruj_nez_X
    add esp, 4*2
    call WriteNewLine
    
    push text2
    call kopiruj_nez_X
    add esp, 4*2
    call WriteNewLine
    
    push text1
    call nahrad_i
    add esp, 4*2
    call WriteNewLine
    
    push text2
    call nahrad_i
    add esp, 4*2
    call WriteNewLine
    
    
    
    ; Postupny ukol - zde postupne doplnujte volani funkci
    ; dlouhy_text(text1)
    ; dlouhy_text(text2)
    ; kopiruj_nez_X(text1)
    ; kopiruj_nez_X(text2)
    ; nahrad_i(text1)
    ; nahrad_i(text2)
    ; kopiruj_nez_X(text1)
    ; kopiruj_nez_X(text2)
    ; ultimatni(text1)
    ; ultimatni(text2)
    
    xor eax, eax
    pop ebp
    ret
  
    ; (1) Ukol - if-else
    ; Implementujte nasledujici pseudokod:
    ; Zpusob predavani parametru funkce je na Vas. Implementace vypis je na Vas
    ; (napr. vyuzijte knihovnu rw32). Ceckove funkce volejte spravne.
    ;
    ; dlouhy_text(*text) {
    ; if (strlen(text)>=15)
    ;   vypis("dlouhy text")
    ; else
    ;   vypis("kratky text")
    ; }
dlouhy_text:
    push ebp
    mov ebp, esp
    push eax
    
    push dword [ebp+8]
    call strlen
    ; v eax je delka stringu
    cmp eax, 15
    jae dlouhy_text_true
    mov esi, kratky_exit_str
    
  dlouhy_text_true:
    mov esi, dlouhy_exit_str
    call WriteString
    
    pop eax
    mov esp, ebp
    pop ebp
    ret

    ; (2) Ukol - while
    ; Implementujte nasledujici pseudokod:
    ; Zpusob predavani parametru funkce je na Vas. Ceckove funkce volejte spravne.
    ;
    ; kopiruj_nez_X(*text) {
    ; while (text[i] != 'X')
    ;   putchar(text[i])
    ;   i++
    ; }
    
kopiruj_nez_X:
    push ebp
    mov ebp, esp
    push ecx
    push eax
    push esi
    mov esi, [ebp+8]
    mov ah, 'X'
    xor ecx, ecx

    while:
      mov al, [esi+ecx]
      cmp ah, al
      je while_end
      call WriteChar
      inc ecx
      jmp while
    while_end:
    
    pop esi
    pop eax
    pop ecx
    mov esp, ebp
    pop ebp
    ret
    
    ; (3) Ukol - for
    ; Implementujte nasledujici pseudokod:
    ; Zpusob predavani parametru funkce je na Vas. Ceckove funkce volejte spravne.
    ;
    ; nahrad_i(*text) {
    ; for (int c = strlen(text); c != 0; c--)
    ;   if (text[c]='i' || text[c]='I')
    ;       text[c]='x'
    ; }
nahrad_i:
    push ebp
    mov ebp, esp
    push eax
    
    push dword [ebp+8]
    call strlen  
    
    pop eax
    mov esp, ebp
    pop ebp
    ret
    ; (4) Ukol - ultimatni test
    ; Upravte predchazejici funkce tak, ze budou mit navratovou hodnotu.
    ; dlouhy_text - true ('a') nebo false ('n')
    ; kopiruj_nez_X - pocet zkopirovanych znaku,
    ; nahrad_i - pocet nahrazenych znaku.
    ; Implementujte nasledujici pseudokod:
    ; Zpusob predavani parametru funkce je na Vas.
    ;
    ; ultimatni(*text) {
    ; printf("Text je dlouhy %c. Je v nem %d znaku k prvnimu x. Celkove v nem
    ; bylo %d znaku i.", dlouhy_text(text), kopiruj_nez_X(text), nahrad_i(text))
    ; }
    