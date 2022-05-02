%include "Rw32-2020.inc"

; Cviceni c.9 Ukol 2
; Kod muzete jakkoliv menit a hrat si s nim
; Muzete si radit, diskutovat problem a pouzivat materialy z prednasek a cviceni.

section .data
    str_1 db "Vitejte studente xloginxx na devatem cviceni.", EOL, 0
    len_1 equ $-str_1
    substr_1 db "xloginxx"
    substr_len_1 equ $-substr_1
    
    str_2 db "Tohle je text pro vas druhy priklad. nejedna se o nic genialniho. Bylo potreba" 
    db " jen pripravit nekolik vet. tyto vety jsou jen pro cviceni. na testu ma byt prace s retezci.", EOL, 0
    len_2 equ $-str_2
    
    str_3_0 db "Tohle je text k tretimu prikladu. Predmet Asemblery je povinnym predmetem v prvnim"
    db " semestru studia a proto nemuze navazovat na zadny jiny predmet. Ziskane znalosti vsak budou"
    db " vychozim a zakladnim predpokladem pro absolvovani cele rady predmetu.", EOL, 0
    len_3_0 equ $-str_3_0
    
    str_3_1 db "Tohl3 je text k tretimu pr1kladu. Predmet Asemb2ery je povinnym predmetem v prvnim"
    db " semestru studi4 a proto nemuze navazovat na zadny jiny p4edme7. Ziskane znalo5ti vsak b2dou"
    db " vychozim a zakladnim predpokl4dem pro absolvovani cele rady predmetu.", EOL, 0
    len_3_1 equ $-str_3_1
    
section .bss

section .text
_main:
    mov ebp, esp; for correct debugging
    push ebp
    mov ebp, esp
    std
    ; Ukol 2
    ; Cilem tohoto ukolu je procvicit si praci se zasobnikem a tvorbu vlastnich
    ; funkci. Dale je cilem procvicit si praci s retezcovymi instrukcemi.
    ;


    ; --> prvni priklad
    ; Nactete ze vstupu vas login a nakopirujte jej na spravne misto v textu str_1 (xloginxx).
    ; Tipy: 
    ;  - pro login a delku nacteneho retezce si vytvorte promennou napriklad my_login
    ;    a my_login_len.
    ;  - muzete zde napriklad vyuzit funkce asm_find_string nebo asm_find_char, ktere 
    ;    jste vytvareli v predchozim ukolu
    ;
  
    ; --> druhy priklad
    ; Opravte chyby v textu str_2. Nektere vety zacinaji malymi pismeny, za pouziti 
    ; retezcovych instrukci tyto pismena naleznete a prepiste je na velka.
    ;
    ; Tipy: 
    ;  - muzete zde napriklad vyuzit funkci asm_find_char, kterou jste
    ;    vytvareli v predchozim ukolu
    ;
    
    ; --> treti priklad
    ; Porovnejte texty ulozene v promennych str_3_0 a str_3_1. Tyto texty se lisi na nekolika
    ; mistech. Na pozicich, kde se texty lisi, ma str_3_0 pismena, ktera nejdrive vypisete
    ; v poradi v jakem se nachazeji v textu a nasledne i v opacnem poradi, kdy by vam
    ; melo vyjit konkretni slovo.
    ; Tipy:
    ;  - nalezene znaky si ulozte do promenne, staci delka 10
    ;  - pro vypis slova v reverzovane podobe staci vyuzit retezcovych instrukci,
    ;    pokud jej budete chtit i ulozit, pouzijte zasobnik
    ;  - muzete zde napriklad vyuzit funkci asm_strcmp, kterou jste
    ;    vytvareli v predchozim ukolu
    ;

    cld
    xor eax, eax
    pop ebp
    ret
