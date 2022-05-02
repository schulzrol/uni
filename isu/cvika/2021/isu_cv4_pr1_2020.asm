; (1x) Vytvorte zakladni strukturu programu (sekce + include knihovny Rw32-2020.inc) + nezapomente na instrukci ret na konci
; (2x) Ve spravnych sekcich zadefinuje nasledujici promenne:
;     x- memw1 a memw2 s hodnotou 0x01FF a 0x0001 (urcete nejmensi datovy typ a definujte s nim tyto promenne)
;     x- promenna vypis obsahujici retezec "Vysledek: " zakonceny znakem 0  - datovy typ byte
;     - pole membarr tak, aby bylo ekvivaletni predpisu: 
;        C ekvivalent:
;        char byte_array[2] = {-100, 5};
;     - pole memwarr tak, aby bylo ekvivaletni predpisu: 
;        C ekvivalent:
;        short word_array[2] = {-200, 10};
;     - neinicializovana promenna vysledek o velikosti 2 wordy
; (3x) Provedte soucet memw1 + memw2, a to nasledovne: 
;     - Hodnoty obou promennych si nactete z pameti do registru (napr. AX, DX).
;     - Predstavte si nyni, ze umime scitat pouze 8-bit cisla, tzn. soucet budete muset rozdelit!
;     - Nejprve tedy sectete spodni bity a az potom bity horni.
;     - Kod napiste pod globalnim navestim ukol3.
;     - Pouzijte instrukce: MOV, ADD, ADC
;     - Pouzijte funkce: WriteString, WriteHex16, WriteNewLine
;     - Pozorujte v debuggeru - jake flagy byly nastaveny a co znamenaji?
;     - Stejnym postupem zkuste memw1 - memw2 za pouziti funkci MOV, SUB, SBB. Byl nastaven CF?
; (4x) Do 16bit registru nactete obe hodnoty z pole membarr (pomoci debuggeru se ujistete, jaka hodnota je na spodnich 8 bitech a jaka na hornich)
;     - pro presun do registru pouzijte pouze jeden mov!
;     - Provedte membarr[0]/membarr[1] - pracujte vsak s registrem, do ktereho jste si cisla ulozili, ne s promennou.
;     - Instrukce DIV vyzaduje pro delenec pouzit min. 16bit registr. Vy vsak pracujete s 8bit hodnotami.
;     - Je nutne tedy, abyste zaporne cislo (delenec) rozsirili do vetsiho registru (pozor na propagaci znamenka).
;     - Kod napiste pod globalnim navestim ukol4.
;     - Pouzijte instrukce: MOV, MOVSX/CBW, IDIV
;     - Pouzijte funkce: WriteInt8, WriteNewLine. Vypiste podil i zbytek po deleni.
; (5) Provedte memwarr[0] * memwarr[1].
;     - Vysledne hodnoty v registrech presunte do pripravene promenne vysledek - pozorujte v debuggeru.
;     - Hodnotu z promenne "vysledek" pote presunte do vhodneho registru a vypiste.
;     - Kod napiste pod globalnim navestim ukol5.
;     - Pouzijte instrukce: MOV, IMUL
;     - Pouzijte funkce: WriteInt32, WriteNewLine
; ! Vzdy si uvedomte, zda pracujete s hodnotou ([], zde musite zohlednit datovy typ) nebo s adresou (ukazatel, vzdy 32 bitu).

%include "rw32-2018.inc"

section .data
memw1 dd 0x01FF
memw2 db 0x0001
vypis db "Vysledek: ",0
membarr db -100,5
memwarr dw -200, 10

section .bss
vysledek resw  2

section .text
main:
    mov ebp, esp; for correct debugging
  push ebp
  mov ebp,esp

ukol3:
  xor edx, edx
  xor eax, eax
  mov ax, [memw1]
  mov dl, [memw2]
  add al, dl
  adc ah, dh

  mov esi, vypis
  call WriteString
  call WriteInt16
  call WriteNewLine
  
    xor edx, edx
  xor eax, eax
  mov ax, [memw1]
  mov dl, [memw2]
  sub al, dl
  sbb ah, dh
  sbb ax, 0
  sbb eax,0
  mov esi, vypis
  call WriteString
  call WriteInt16
  call WriteNewLine
    
ukol4:
  mov ax, [membarr]
  ; al = -100, ah = 5
  mov dl, ah
  cbw
  idiv dl
  
  mov esi, vypis
  call WriteString
  call WriteInt8
  call WriteNewLine
  
ukol5:
  mov ax, [memwarr] ; ax = -200
  imul word [memwarr+2] ; dx:ax = -200*10
  
  mov [vysledek], ax
  mov [vysledek+2], dx
  
  mov eax, [vysledek]

  mov esi, vypis
  call WriteString
  call WriteInt32
  call WriteNewLine
  
  mov esp, ebp
  pop ebp

  ret
