%include "Rw32-2020.inc"

extern _strcmp ; int strcmp (const char * str1, const char * str2);
CEXTERN strchr ; char * strchr (char * str, int character);
extern _strcpy ; char * strcpy (char * destination, const char * source);
CEXTERN strstr ; char * strstr (const char *, const char *);

; Cviceni c.9 Ukol 1
; Kod muzete jakkoliv menit a hrat si s nim
; Muzete si radit, diskutovat problem a pouzivat materialy z prednasek a cviceni.

section .data
    str_1_0 db "Vitejte studente na devatem cviceni.", EOL, 0
    len_1_0 equ $-str_1_0
    str_1_1 db "Vitejte studente na ctvrtem cviceni.", EOL, 0
    len_1_1 equ $-str_1_1
    
    str_1_2 db "Vitejte studente na devatem cviceni.", EOL, 0
    len_1_2 equ $-str_1_0
    
    str_2_0 db "Tohle je text pro vas druhy priklad. nejedna se o nic genialniho. Bylo potreba" 
    db " jen pripravit nekolik vet. tyto vety jsou jen pro cviceni. na testu ma byt prace s retezci.", EOL, 0
    len_2_0 equ $-str_2_0
    
    str_3_0 db "Ahoj Kamile!", EOL, 0
    str_3_1 db "Mateji"
    len_3_1 equ $-str_3_1
    
    str_3_2 db "xloginXX", EOL, 0
    len_3_2 equ $-str_3_2
    
    str_4_0 db "Tohle je text k ctvrtemu prikladu. Predmet Asemblery je povinnym predmetem v prvnim"
    db " semestru studia a proto nemuze navazovat na zadny jiny predmet. Ziskane znalosti vsak budou"
    db " vychozim a zakladnim predpokladem pro absolvovani cele rady predmetu.", EOL, 0
    len_4_0 equ $-str_4_0
    
    str_4_1 db "prikladu"
    len_4_1 equ $-str_4_1
    
    str_4_2 db " asembl"
    len_4_2 equ $-str_4_2
    
    str_4_3 db "absolvovani"
    len_4_3 equ $-str_4_3
    
    
section .bss   
    str_3_3 resb 9
  
        
section .text
_main:
    push ebp
    mov ebp, esp
    std 
    ; Ukol 1
    ; Cilem tohoto ukolu je procvicit si praci se zasobnikem a tvorbu vlastnich funkci.
    ; Dale je cilem procvicit si praci s retezcovymi instrukcemi a volanim jazyka C. 
    ; Doplnte zde vzdy alespon 2 vhodne volani Vasich funkci nize s jiz definovanymi
    ; retezci v sekci data. Overte spravnost chovani Vasich funkci volanim jejich
    ; protejsku z jazyka C (funkce uz mate natahnute pomoci extern v kodu vyse).
    ;
    
    cld
    xor eax, eax
    pop ebp
    ret
    
    ; --> prvni priklad
    ; Naimplementujte funkci asm_strcmp, ktera porovna dva retezce. Vstupni hodnoty
    ; budete predavat pres zasobnik. Funkce vraci 0 v pripade ze se retezce shoduji,
    ; jinak se vraci pozice rozdilu. V pripade neshody vypise pozici prvniho nalezeneho rozdilu.
    ; Deklarace teto funkce vypada nasledovne:
    ; 
    ; uint32_t asm_strcmp (uint32_t del_ret, char* retezec_1, char* retezec_2)
    ;
    ; Vstupni parametry jsou definovany takto:
    ; 
    ; del_ret   - Delka retezcu, zde je pouze jedna delka, nebot se predpoklada, ze
    ;             uzivatel nebude zadavat retezce ruzne delky, ktere jsou samozrejme
    ;             rozdilne 
    ; retezec_1 - Adresa prvniho retezce
    ; retezec_2 - Adresa druheho retezce
    ;
    ; DULEZITE:
    ; - Funkce musi zachovat obsah vsech registru az na registr EAX, ve kterem je vracen
    ;   vysledek porvnani. (zachovani obsahu registru se tyka hlavne registru, ktere
    ;   pouzivate v dane funkci)
    ; - Funkce uklizi zasobnik
    ; - Funkce vraci vysledek v registru EAX.
    ;
    ; Poznamka k volani C funkce strcmp:
    ; - Pri neshode vraci to zda je znak mensi nebo vetsi a ne pozici. 
    ; 
    
    ; --> druhy priklad
    ; Naimplementujte funkci asm_find_char, ktera hleda znak v retezci o zadane delce. 
    ; Vstupni hodnoty budete predavat pres zasobnik. Funkce vraci pozici, kde byl zadany
    ; znak nalezen, jinak vraci 0. Deklarace teto funkce vypada nasledovne:
    ; 
    ; uint32_t asm_find_char (uint32_t del_ret, char* retezec, char hl_znak)
    ;
    ; Vstupni parametry jsou definovany takto:
    ; 
    ; del_ret - Delka retezce
    ; retezec - Adresa retezce
    ; hl_znak - Znak, ktery se hleda v zadanem retezci
    ;
    ; DULEZITE:
    ; - Funkce musi zachovat obsah vsech registru az na registr EAX, ve kterem je vracen
    ;   vysledek hledani. (zachovani obsahu registru se tyka hlavne registru, ktere
    ;   pouzivate v dane funkci)
    ; - Funkce se zastavi pri nalezeni PRVNIHO vyskytu zadaneho znaku.
    ; - Funkce uklizi zasobnik
    ; - Funkce vraci vysledek v registru EAX.
    ; 
    ; Poznamka k volani C funkce strchr:
    ; - Vraci pointer na hledany znak pro zjisteni pozice je potreba odecist puvodni pointer.
    ; - 0 zde odpovida null pointeru - tedy v tomto pripade bude vse hned ok.
    ;
            
    ; --> treti priklad
    ; Naimplemetujte funkci asm_strcpy, ktera kopiruje jeden retezec do druheho od zadane pozice.
    ; Vstupni hodnoty budete predavat pres zasobnik. Funkce nic nevraci. Deklarace teto funkce 
    ; vypada nasledovne:
    ;
    ; void asm_strcpy (uint32_t del_ret, char* men_ret, char* kop_ret, uint32_t pozice)
    ;
    ; Vstupni parametry jsou definovany takto:
    ;
    ; del_ret - Delka KOPIROVANEHO retezce
    ; men_ret - Retezec, ktery se meni, tedy do ktereho bude nakopirovan druhy retezec
    ; kop_ret - Retezec, ktery se nakopiruje do druheho od zadane pozice
    ; pozice  - Index mista odkud se bude retezec kopirovat do druheho
    ;
    ; DULEZITE:
    ; - Predpoklada se ze zadane hodnoty jsou spravne, tedy men_ret je dostatecne dlouhy, aby se
    ;   tam vesel kopirovany, ze pozice existuje (neni zaporne cislo) atd.
    ; - Funkce musi zachovat obsah vsech registru. (zachovani obsahu registru se tyka hlavne 
    ;   registru, ktere pouzivate v dane funkci)
    ; - Funkce uklizi zasobnik
    ;                            
    ; Poznamka k volani C funkce strcpy:
    ; - Vraci pointer na vysledek - zde je nutne overit vysledek vypisem.
    ;
    
    ; --> ctvrty priklad
    ; Naimplementuje funkci asm_find_string, ktera hleda podretezec v retezci. Vstupni hodnoty budete 
    ; predavat pres zasobnik. Funkce vraci pozici, kde byl zadany podretezec nalezen, jinak vraci 0.
    ; Deklarace teto funkce vypada nasledovne:
    ;
    ; uint32_t asm_find_string (uint32_t del_ret, uint32_t del_hl_ret, char* ret, char* hl_ret)
    ;
    ; Vstupni parametry jsou definovany takto:
    ; 
    ; del_ret     - Delka prohledavaneho retezce
    ; del_hl_ret  - Delka hledaneho podretezce
    ; ret         - Adresa prohledavaneho retezce
    ; hl_ret      - Adresa hledaneho podretezce
    ;
    ; DULEZITE:
    ; - Funkce musi zachovat obsah vsech registru az na registr EAX, ve kterem je vracen
    ;   vysledek hledani. (zachovani obsahu registru se tyka hlavne registru, ktere
    ;   pouzivate v dane funkci)
    ; - Funkce se zastavi pri nalezeni PRVNIHO vyskytu zadaneho retezce.
    ; - Funkce uklizi zasobnik
    ; - Funkce vraci vysledek v registru EAX.
    ;
    ; Poznamka k volani C funkce strstr:
    ; - Vraci pointer na zacatek podretezce pro zjisteni pozice je potreba odecist puvodni pointer.
    ; - 0 zde odpovida null pointeru - tedy v tomto pripade bude vse hned ok.
    ;
    
