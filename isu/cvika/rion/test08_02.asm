; TEST PROSEL
%include "rw32-2018.inc"

section .text

CMAIN:
    mov ebp, esp; for correct debugging
	push ebp
	mov ebp,esp

	; zde muzete psat vas kod
        xor eax, eax
        mov edx, 1 ; write access 1 => yes, cokoliv jine => no
        nacteni_znaku:
            push edx
            call ReadChar ; AL = hodnota znaku
            pop edx
            cmp eax, 'X'
            je konec_write
            cmp eax, -1 ; konec vstupu
            je konec
            cmp al, 's'
            je nacteni_znaku
            cmp al, 'w'
            je write_access_not
            vypsani:
                cmp edx, 1 ; muzu psat?
                jne nacteni_znaku ; nemuzu
                call WriteChar
            
            jmp nacteni_znaku

        write_access_not:
        not edx
        jmp vypsani
        konec_write:
        cmp edx, 1
        jne konec
        call WriteChar
        konec:
	mov esp, ebp
	pop ebp
	mov eax, 0 ; navratova hodnota musi byt 0
	ret