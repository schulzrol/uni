%include "rw32-2018.inc"

section .text
jedna  db "jedna",0
nula  db "nula",0
bezny db ", ",0
predposledniT db " a ",0

CMAIN:
    mov ebp, esp; for correct debugging
	push ebp
	mov ebp,esp

	; zde muzete psat vas kod
        xor eax, eax
        call ReadUInt8 ; vysledek v al
        mov bl, al          ; bl => T
        call ReadUInt8 ;      al => X
        
        cmp bl, 0 ; T == 0
        je obraceni
        
        cmp bl, 1 ; T == 1
        je parita

        
        cmp bl, 2 ; T == 2
        jne konec
        cmp al, 0
        je konec_nula
        prozacatek:
        shl al, 1
        inc ah
        jnc prozacatek ; nula nazacatku
        dec ah
        jmp jednicka
        
        fullon:
        cmp ah, 8
        je konec
        cmp ah, 7
        je predposledni
        mov esi, bezny
        posetnuti:
        call WriteString
        shl al, 1
        jc jednicka
        
        inc ah
        mov esi, nula
        call WriteString
        jmp fullon
        
        predposledni:
        mov esi, predposledniT
        jmp posetnuti
        
        jednicka:
        inc ah
        mov esi, jedna
        call WriteString
        jmp fullon
        
        parita:
        ;P = sudy -> 0
        ;P = lichy -> 1
        cmp al, 0
        jpe peven
        ; lichy
        mov al, '1'
        call WriteChar
        jmp konec
        
        peven:
        mov al, '0'
        call WriteChar
        jmp konec
        
        datjedna:
        or bl, dl
        jmp dal
        
        obraceni:
        xor ebx, ebx
        mov ecx, 8
        mov dl, 1
        dalsi_bit:
            shl al, 1
            jc datjedna
            dal:
            shl dl, 1

        loop dalsi_bit
        mov al, bl
        call WriteUInt8
        jmp konec
        konec_nula:
        mov esi, nula
        call WriteString
        konec:
        
        ;konec

	mov esp, ebp
	pop ebp
	mov eax, 0
	ret