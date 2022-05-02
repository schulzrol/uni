%include "rw32-2018.inc"

section .data
    result_task_1_0 dw 0xAF6E
    result_task_1_1 dw 0x2B6A
    result_task_1_2 db 0xAE
    result_arr_task_1 dw 0xD495, 0xAF6E, 0x0
    
    result_task_2_0 dw 0xB1A0
    result_task_2_1 dd 0x0CA15480
    result_task_2_2 dd 0xFD2811A6
    result_task_2_3 dq 0xACDCB105DEADBEEF
    
    result_task_3_0 dd 8
    result_task_3_1 dw 0xF4C9
    
    
    var_task_1_0 dw 0x8F0E
    
    var_task_2_0 dw 0xB1BF
    var_task_2_1 dw 0x1234
    var_task_2_2 dd 0xA50234D5
    var_task_2_3 dq 0xEFDEDCAC05ADB1BE 
    
    task_var_3_0 dw 0x7A15
    task_arr_3_0 dw 10281, 32429, -24586, 897, 1239, -14489, 22826, 12345, 4567, -2853, -30108
                 dw 6541, 9874, -5924, -15235, 18369, 14798, 21489, -3245, -16876, -11233, -1284
                 dw -6541, 4283, 13789, 31000, 25221, -26321, 27000, -28000, 123, 486, 29015, -28308
    task_arr_3_0_end dw 0xFFFF

section .bss

std_arr_task_1 resw 5
std_var_task_2_0 resd 1


section .text
main:
    mov ebp, esp; for correct debugging
;CMAIN: ;pri NASM
  push ebp
  mov ebp,esp
  
  xor eax, eax
; Ukol 1
; Cilem tohoto ukolu je vyzkouset si logicke instrukce: AND, OR, XOR a NOT                              
; a pochopit jejich typicke zpusoby vyuziti. Budete potrebovat pole o velikosti 5
; pro 16b hodnoty, nazvete jej std_arr_task_1, budete tam ukladat postupne 
; vysledky jednotlivych prikladu.
;
; --> prvni priklad
; Nactete si promennou var_task_1_0 z pameti a provedte nastaveni bitu 5, 6 a 13
; (cislujeme od 0) na jednicku. Pote vyuzijte instrukci CMP pro porovnani teto hodnoty s 
; vysledkem result_task_1_0 ulozenym v pameti (instrukce jump je uz pripravena). 
; Vysledek ulozte do pole, ktere jste si pro tento ukol pripravili.
;
.task_1_1:
; ============ Zde studenti doplnuji svuj kod ============ 
  mov ax, [var_task_1_0]
  or ax, 0b0010000001100000
  cmp ax, [result_task_1_0]  
  mov [std_arr_task_1], ax
  jne .task_1_1
    
; --> druhy priklad
; Pouzijte vysledek z predchoziho prikladu a nastavte v nem bity 15, 10 a 2 (cislujeme od 0) na
; nulu. Pote vyuzijte instrukci CMP pro porovnani teto hodnoty s vysledkem result_task_1_1
; ulozenym v pameti (instrukce jump je uz pripravena). Vysledek ulozte do pole, ktere jste 
; si pro tento ukol pripravili.
;
.task_1_2:    
; ============ Zde studenti doplnuji svuj kod ============
  mov ax, [std_arr_task_1]
  and ax, 0b0111101111111011
  cmp ax, [result_task_1_1]  
  mov [std_arr_task_1+1*2], ax
  jne .task_1_2


; Ukol 2
; Cilem tohoto ukolu je vyzkouset si logicke instrukce: SHL, SHR, SAR, ROL, RCL, ROR, RCR.
; a pochopit jejich typicke zpusoby vyuziti.
;
; --> prvni priklad
; Nactete si promennou var_task_2_0 z pameti. Nasledne vyuzijte instrukce posuvu aby jste vynulovali
; spodnich 5 bitu teto promenne. Po uprave ulozte novou promennou na misto puvodni (var_task_2_0).
; Vysledek zkontrolujte provedenim porovnani pomoci instrukce CMP s promennou result_task_2_0
;
task_2_1:    
; ============ Zde studenti doplnuji svuj kod ============ 
  mov ax, [var_task_2_0]
  shr ax, 5
  shl ax, 5
  mov [var_task_2_0], ax
  cmp ax, [result_task_2_0]
  jne task_2_1

; --> druhy priklad
; Nactete promenne var_task_2_0 a var_task_2_1 a provedte operaci NEznamenkoveho nasobeni. Vysledek
; vypiste pomoci JEDINEHO registru - vysledek bude obsazen v JEDNOM registru, viz instrukce NEznamenkoveho
; nasobeni. Vysledek ulozte do promenne std_var_task_2_0, kterou si SAMI musite nejdrive vytvorit.
; Vysledek zkontrolujte provedenim porovnani pomoci instrukce CMP s promennou result_task_2_1
;
task_2_2:    
; ============ Zde studenti doplnuji svuj kod ============ 
  mov ax, [var_task_2_0]
  mov bx, [var_task_2_1]
  mul bx
  shl eax, 16
  mov ax, dx
  rol eax, 16
  mov [std_var_task_2_0], eax
  cmp eax, [result_task_2_1]
  jne task_2_2
  
; --> treti priklad
; Pomoci instrukce posuvu vydelte var_task_2_2, 32ti. Spravny vysledek vypiste dekadicky
; Vysledek porovnejte s promennou result_task_2_2
task_2_3: 
; ============ Zde studenti doplnuji svuj kod ============ 
  mov eax, [var_task_2_2]
  sar eax, 5
  cmp eax, [result_task_2_2]
  jne task_2_3

; Ukol 3
; Cilem tohoto ukolu je vyzkouset podminene skoky JCC a pochopit
; princip podminenych instrukci.
;
; --> prvni priklad
; nactete si promennou task_var_3_0 a vytvorte smycku, ktera spocita
; pocet jednicek v nactenem cisle. Vypiste pocet jednicek a porovnejte jej
; s hodnotou ulozenou v promenne result_task_3_0.
;
task_3_1:
; ============ Zde studenti doplnuji svuj kod ============ 
  mov ax, [task_var_3_0]
  xor bx, bx
  mov cx, 16
  .repeat:
  shl ax, 1
  jnc .skip_inc
  inc bx  
  .skip_inc:
  dec cx
  cmp cx, 0
  ja .repeat
  
  cmp bx, [result_task_3_0]
  jne task_3_1



; Ukoly bonusove
; Rozsireni predchozich ukolu, pro ty co jsou moc rychli (nebo si chcou procvicit vic)

; --> prvni priklad
; Nyni pomoci logickych instrukci vypocitejte nasledujici priklady:
;
; Vysledky postupne porovnavejte s result_arr_task_1[0], result_arr_task_1[1], result_arr_task_1[2], pomoci
; instrukce TEST. Smite pouzit POUZE 1 registr!!!
;
.task_B_1:    
; std_arr_task_1[2] = std_arr_task_1[0] AND std_arr_task_1[1]
; ============ Zde studenti doplnuji svuj kod ============   
  mov ax, [std_arr_task_1]
  and ax, [std_arr_task_1+1*2]  
  mov [std_arr_task_1+2*2], ax
  test ax, [result_arr_task_1]
  jnz .task_B_1

; std_arr_task_1[3] = NOT ( std_arr_task_1[0] OR std_arr_task_1[2] )
; ============ Zde studenti doplnuji svuj kod ============   
  jnz .task_B_1
  
; std_arr_task_1[4] = NOT( std_arr_task_1[3] AND ( std_arr_task_1[0] XOR std_arr_task_1[1] ) )
; ============ Zde studenti doplnuji svuj kod ============   
  jnz .task_B_1
  
; --> druhy priklad
; Vymaskujte prostrednich 8 bitu (4 - 11) cisla result_task_1_0. Pomoci logicke instrukce pak vyrobte
; osmibitove cislo, ktere porovnejte pomoci instrukce CMP s result_task_1_2 a nasledne vytisknete.
;
.task_B_2:    
; ============ Zde studenti doplnuji svuj kod ============
  jne .task_B_2

; --> treti priklad
; Nactete 64b promenne var_task_2_3 do registru tak, aby hodnota byla ulozena EAX:EBX.
; Pomoci posuvu/rotaci provedte preusporadani slabik (bytu) X0 - X7 v registrech EAX a EBX
; takto:
;
; puvodni hodnota:    
;                    EAX = X7 X6 X5 X4
;                    EBX = X3 X2 X1 X0
;                    
; vysledna hodnota:  EAX = X4 X5 X1 X3
;                    EBX = X6 X2 X0 X7
; 
; Nejnizsi vyznamova slabika(byte) je X0, nejvyssi vyznamova slabika (byte) je X7.
;
; Vysledek vypiste na standardni vystup v HEXADECIMALNI podobe na samostatne radky, 
; kdy na vystupu budou registry vypsany v poradi EAX, EBX. Vysledek porovnejte s
; promennou result_task_2_3.
;
task_B_3:    
; ============ Zde studenti doplnuji svuj kod ============
  jne task_B_3

; --> ctvrty priklad
; Nejdrive pomoci smycky spocitejte pocet prvku v poli task_arr_3_0. 
; Nasledne napiste smycku, ktera projde pole task_arr_3_0 a najde nejvetsi cislo,
; pak najdete stejnym zpusobem nejmensi cislo. Nejmensi a nejvetsi cislo 
; ulozte do promennych, ktere si sami vytvorite (std_max_task_3, 
; std_min_task_3). Nad temito dvema cisly provedte instrukci XOR a 
; vysledek porovnejte s result_task_3_1.
;
task_B_4:
; ============ Zde studenti doplnuji svuj kod ============
  jne task_B_4

; konec
  mov esp, ebp
  pop ebp
  
  ret
