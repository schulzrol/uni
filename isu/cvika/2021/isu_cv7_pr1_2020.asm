%include "rw32-2018.inc"

; Cviceni c.7 Ukol 1
; Kod muzete jakkoliv menit a hrat si s nim
; Muzete si radit, diskutovat problem a pouzivat materialy z prednasek a cviceni.

section .data
    operands_add db 12, -100
    operands_sub dw -256, -20321
    
    operands_mul dd 23561, 823125
    operands_div dd -42586, 2308
    result_add dw 0
section .bss

section .text
main:
    mov ebp, esp; for correct debugging
    push ebp
    mov ebp, esp
    
    ; Ukol 1
    ; Cilem tohoto ukolu je procvicit si praci se zasobnikem a tvorbu vlastnich
    ; funkci. Budete postupne implementovat funkce pro scitani, odcitani
    ; nasobeni a deleni. Popis jednotlivych funkci mate nize.
    ;
    push operands_add ; predani prvniho parametru
    push result_add   ; predani druheho parametru
    
    call addition
    
    xor eax, eax
    pop ebp
    ret
      
    ; --> prvni priklad
    ; Nejdrive pripravte promennou result_add odpovidajici velikosti
    ; kam bude ulozen vysledek.
    ;
    ; Dale implementujte funkci addition, ktera secte dva operandy SE ZNAMENKEM.
    ; Funkce bude pracovat s 8 bitovymi hodnotami.
    ;
    ; Vstupni hodnoty budete predavat pres zasobnik!!! Konkretne tedy
    ; pres zasobnik predate ADRESU operandu a mista kam ulozite vysledek.
    ; Operandy byly jiz definovany operands_add, pro vysledek jste si vytvorili
    ; promennou result_add SAMI. Funkci zavolejte z tela hlavni funkce (main) a 
    ; nasledne vypiste vysledek.
    ; 
    ; Vstupni parametry jsou definovany takto:
    ;
    ; Parametr 1 - adresa operandu
    ; Parametr 2 - adresa mista v pameti pro ulozeni vysledku
    ;
    ; DULEZITE:
    ; - Funkce musi zachovat obsah registru EAX, EBX, ECX, EDX.
    ; - Funkce uklizi zasobnik
    ; - Funkce nic nevraci, vysledek bude ulozen v pameti.
    ;
    
addition:
; def addition(signed char[2] operandy, signed char* vysledek) -> short/word

    push ebp          ; zásobníkový rámec
    mov ebp, esp      ; -//-
    
    push eax          ; Funkce musi zachovat obsah registru EAX, EBX, ECX, EDX.
    push ebx          ; -//-
    
    mov eax, [ebp+12] ; prvni parametr
    mov bl, [eax]     ; prvni operand prvniho parametru v bl
    mov al, [eax+1]   ; druhy operand prvniho parametru v al
    add al, bl        ; samotne scitani prvniho a druheho operandu
    cbw               ; rozsirim na 16b
    mov ebx, [ebp+8]  ; druhy parametr
    mov [ebx], ax     ; vracim vysledek
    
    pop ebx           ; Funkce musi zachovat obsah registru EAX, EBX, ECX, EDX.
    pop eax           ; -//-
    
    pop ebp           ; zásobníkový rámec
    ret 8             ; Funkce uklizi zasobnik
    
    ; --> druhy priklad
    ; Nejdrive pripravte promennou result_sub odpovidajici velikosti
    ; kam bude ulozen vysledek.
    ;
    ; Dale implementujte funkci subtraction, ktera odecte dva operandy SE ZNAMENKEM.
    ; Funkce bude pracovat s 16 bitovymi hodnotami.
    ;
    ; Vstupni hodnoty budete predavat pres zasobnik!!! Konkretne tedy
    ; pres zasobnik predate ADRESU operandu a mista kam ulozite vysledek.
    ; Operandy byly jiz definovany operands_sub, pro vysledek jste si vytvorili
    ; promennou result_sub SAMI. Funkci zavolejte z tela hlavni funkce (main) a 
    ; nasledne vypiste vysledek.
    ; 
    ; Vstupni parametry jsou definovany takto:
    ;
    ; Parametr 1 - adresa operandu
    ; Parametr 2 - adresa mista v pameti pro ulozeni vysledku
    ;
    ; DULEZITE:
    ; - Funkce musi zachovat obsah registru EAX, EBX, ECX, EDX.
    ; - Funkce uklizi zasobnik
    ; - Funkce nic nevraci, vysledek bude ulozen v pameti.
    ;     
  
    
    ; --> treti priklad
    ; Nejdrive pripravte promennou result_mul odpovidajici velikosti
    ; kam bude ulozen vysledek.
    ;
    ; Dale implementujte funkci multiplication, ktera vynasobi dva operandy SE ZNAMENKEM.
    ; Funkce bude pracovat s 32 bitovymi hodnotami.
    ;
    ; Vstupni hodnoty budete predavat pres zasobnik!!! Konkretne tedy
    ; pres zasobnik predate ADRESU operandu a mista kam ulozite vysledek.
    ; Operandy byly jiz definovany operands_mul, pro vysledek jste si vytvorili
    ; promennou result_mul SAMI. Funkci zavolejte z tela hlavni funkce (main) a 
    ; nasledne vypiste vysledek.
    ; 
    ; Vstupni parametry jsou definovany takto:
    ;
    ; Parametr 1 - adresa operandu
    ; Parametr 2 - adresa mista v pameti pro ulozeni vysledku
    ;
    ; DULEZITE:
    ; - Funkce musi zachovat obsah registru EAX, EBX, ECX, EDX.
    ; - Funkce uklizi zasobnik
    ; - Funkce nic nevraci, vysledek bude ulozen v pameti.
    ;     

    ; --> ctvrty priklad
    ; Nejdrive pripravte promennou result_div odpovidajici velikosti
    ; kam bude ulozen vysledek.
    ;
    ; Dale implementujte funkci division, ktera vydeli dva operandy SE ZNAMENKEM.
    ; Funkce bude pracovat s 32 bitovymi hodnotami.
    ;
    ; Vstupni hodnoty budete predavat pres zasobnik!!! Konkretne tedy
    ; pres zasobnik predate ADRESU operandu a mista kam ulozite vysledek.
    ; Operandy byly jiz definovany operands_div, pro vysledek jste si vytvorili
    ; promennou result_div SAMI. Funkci zavolejte z tela hlavni funkce (main) a 
    ; nasledne vypiste vysledek.
    ; 
    ; Vstupni parametry jsou definovany takto:
    ;
    ; Parametr 1 - adresa operandu
    ; Parametr 2 - adresa mista v pameti pro ulozeni vysledku
    ;
    ; DULEZITE:
    ; - Funkce musi zachovat obsah registru EAX, EBX, ECX, EDX.
    ; - Funkce uklizi zasobnik
    ; - Funkce nic nevraci, vysledek bude ulozen v pameti.
    ;
    