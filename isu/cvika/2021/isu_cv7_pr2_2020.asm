%include "Rw32-2020.inc"

; Cviceni c.7 Ukol 2
; Kod muzete jakkoliv menit a hrat si s nim
; Muzete si radit, diskutovat problem a pouzivat materialy z prednasek a cviceni.

section .data

section .bss

section .text
_main:
    push ebp
    mov ebp, esp
    
    ; Ukol 2
    ; Cilem tohoto ukolu je procvicit cteni ze vstupu pomoci funkci a take
    ; praci se zasobnikem. Ke vsem funkcim nacitejte retezec ze VSTUPU.
    ;
    ; --> prvni priklad
    ; nejdrive si vytvorte dostatecne velkou promennou pro nacteni retezce
    ; (vaseho) loginu ze vstupu. Nasledne nactete svuj login ze vstupu
    ; pomoci odpovidajici funkce z knihovny Rw32-2020.inc. Po nacteni
    ; loginu jej vypiste.
    ;
    
    xor eax, eax
    pop ebp
    ret
    
    ; --> druhy priklad
    ; vytvorte funkci to_upper_case, ktera bude mit na vstupu retezec
    ; a prevede vsechny jeho znaky na jejich velke ekvivalenty (a -> A).
    ; Funkci zavolejte z tela hlavni funkce (main) a nasledne vypiste vysledek.
    ;
    ; Vstup: karel
    ; Vystup: KAREL
    ;
    ; Vstupni parametry jsou definovany takto:
    ;
    ; Parametr 1 - delka retezce (pocet znaku)
    ; Parametr 2 - adresa retezce
    ;
    ; DULEZITE:
    ; - Funkce musi zachovat obsah registru EAX, EBX, ECX, EDX.
    ; - Funkce uklizi zasobnik
    ; - Funkce nic nevraci, vysledek bude ulozen na stejnem miste.
    ;
    
    
    ; --> treti priklad
    ; vytvorte funkci search_string, ktera v zadanem retezci bude hledat
    ; zadany znak a odstrani vsechny jeho vyskyty.
    ; Funkci zavolejte z tela hlavni funkce (main) a nasledne vypiste vysledek.
    ;
    ; Vstup
    ;   - retezec: Rob22ert
    ;   - znak: 2
    ; Vystup: Robert
    ;
    ; Vstupni parametry jsou definovany takto:
    ;
    ; Parametr 1 - delka retezce (pocet znaku)
    ; Parametr 2 - adresa retezce
    ; Parametr 3 - adresa mista v pameti pro ulozeni vysledku
    ; Parametr 4 - odstranovany znak
    ;
    ; DULEZITE:
    ; - Funkce musi zachovat obsah registru EAX, EBX, ECX, EDX.
    ; - Funkce uklizi zasobnik
    ; - Funkce nic nevraci, vysledek bude ulozen v pameti.
    ;

    
    ; --> ctvry priklad
    ; vytvorte funkci reverse_string, ktera bude mit na vstupu retezec
    ; a vypise jej obracene.
    ; Funkci zavolejte z tela hlavni funkce (main).
    ;
    ; Vstup: atrapa
    ; Vystup: aparta
    ;
    ; Vstupni parametry jsou definovany takto:
    ;
    ; Parametr 1 - delka retezce (pocet znaku)
    ; Parametr 2 - adresa retezce
    ;
    ; DULEZITE:
    ; - Funkce musi zachovat obsah registru EAX, EBX, ECX, EDX.
    ; - Funkce uklizi zasobnik
    ; - Funkce nic nevraci, vysledek pouze vypise.
    ;