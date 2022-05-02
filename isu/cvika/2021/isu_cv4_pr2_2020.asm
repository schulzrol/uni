%include "Rw32-2020.inc"

section .data


section .bss
memba   resb  1
membb   resb  1

section .text
_main:
;CMAIN: ;pri NASM
  push ebp
  mov ebp,esp
  
  xor eax, eax
; Pouzijte funkce ReadUInt8 pro cteni z klavesnice. Znaky nactene z klavesnice si muzete
; ulozit do pripravenych promennych v sekci .bss - jen pozor, abyste pri praci nemeli oba
; operandy typu pamet! Kdyztak pouzijte registr.
; Vypocet sumy z intervalu <a, b>:
; suma = (((b-a)+1)*(a+b))/2
; Pouzijte instrukce: mov, add, sub, mul, div, (inc)
; Pouzijte funkce: ReadUInt8

; ============ Zde studenti doplnuji svuj kod ============
; Hint: Zde jen nactete znaky z klavesnice - vstup do okenka Input pred spustenim programu!
; Jedno cislo ulozte do pameti (memba), druhe ponechte v registru AL (viz kod nize)



; =============== Toto prosim nemenit ====================
  cmp al, [memba]
  jge noswapping
  xchg [memba], al

noswapping:
; =========== Zde muzete pokracovat s implementaci =======
; Hint: Do membb ulozte druhe cislo az ted.
; Zde provedte vypocet vzorce (viz vyse). A vypiste reseni.


; Otazka - v jakych registrech je ulozen vysledek nasobeni? 
; Otazka - v jakych registrech je ulozen vysledek deleni? 
; Zkuste hranicni hodnoty (napr. 250 a 255) funguji korektne?
; ========================================================

  ; konec
  mov esp, ebp
  pop ebp
  
  ret
