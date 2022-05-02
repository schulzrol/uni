%include "rw32-2018.inc"

section .data


; Ukol 1: Definujte spravny datovy typ (nejmensi mozny). Proc jste ho pouzil(a)?
cislo1   db   235    ; 1 byte max 255
cislo2   dw   64789  ; word 2B max 65535
cislo3   dw   257    ; word 2B max 65535 ale jen 1B max 255

cisla    dw   7541,541,42
retez1   db   "oxxx", 0
retez2   db   "xxxd",0



section .bss
vysledek resw 1
den resb 1
mesic resb 1
rok resw 1

section .text
main:
    mov ebp, esp; for correct debugging
  push ebp
  mov ebp,esp

  ; Ukol 2: ulozte do vyhrazene pameti "vysledek" vypoctu cisla[0] - cisla[1] + cisla[2]
  ; potrebne instrukce: mov, add, sub (muzete pouzit cheat sheet) 
  ; povsimnete si, ze sirka cisel je 16b

  ; zde doplnte vas kod
  mov ax, [cisla]
  sub ax, [cisla+1*2]
  add ax, [cisla+2*2]
  mov [vysledek], ax

  ; vypis vysledku pro kontrolu, kod prosim nemente
  mov ax,[vysledek]
  call WriteUInt16
  call WriteNewLine

  ; Ukol 3: upravte retezec "retez" tak, aby obsahoval 4 po sobe nasledujici pismena, tzn.
  ;  jestlize "retez1" je na pocatku "axyz" upravte jej na "abcd", tzn. nactete prvni pismeno a opravte stavajici 3
  ; v tomhle cviceni prosim nepouzivejte cykly
  ; potrebne instrukce: mov, add / inc
  ; Napoveda: ASCII tabulka
  mov al, [retez1]
  inc al
  mov [retez1+1*1], al
  inc al
  mov [retez1+2*1], al
  inc al
  mov [retez1+3*1], al
  
  ; zde doplnte vas kod

  ; vypis vysledku pro kontrolu, kod prosim nemente
  mov esi,retez1
  call WriteString

  ; Ukol 4: Upravte retezec "retez2" tak, aby obsahoval 4 po sobe nasledujici pismena, tzn.
  ; jestlize "retez2" je na zacatku "xxxz" upravte jej na "wxyz", tzn. nactete posledni pismeno a opravte stavajici 3
  ; v tomto cviceni prosim nepouzivejte cykly
  ; potrebne instrukce: mov, sub / dec

  ; zde doplnte vas kod
  mov al, [retez2+3*1]
  dec al
  mov [retez2+2*1], al
  dec al
  mov [retez2+1*1], al
  dec al
  mov [retez2], al


  ; Otazka: Muzete pouzit 32bit registr? Zduvodnete. 
  ; nevlezla by se mi tam terminating nula na konci
  ; mohl bych si ulozit obsah pole a inkrementovat al/ah casti kdybych s eax rotoval,
  ; jinak ne, prepsal bych si i to oc nechci v retez2

  ; vypis vysledku pro kontrolu, kod prosim nemente
  mov esi,retez2
  call WriteString
  
  ; Ukol 5: Vytvorte si v pameti misto na ulozeni dnesniho data. Jakym zpusobem zalezi
  ; na Vas, nicmene velikosti musi odpovidat ocekavanym hodnotam. Vyuzitim jedne instrukce
  ; naplnte den, dalsi instrukci mesic a dalsi instrukci rok na dnesni datum. Vysledek
  ; vytisknete pomoci knihovny.
  ; potrebne intrukce: mov (+knihovna Rw32-2020)
  
  ; popiste jakym zpusobem jste si rezerovavli pamet
  ; zde doplnte vas kod 
  
  mov [den], byte 25
  mov [mesic], byte 2
  mov [rok], word 2021
  
  call WriteNewLine
  mov al, [den]
  call WriteInt8
  mov al, [mesic]
  call WriteInt8
  mov ax, [rok]
  call WriteInt16
  
    
  ; konec
  xor eax, eax
    
  mov esp, ebp
  pop ebp

  ret
