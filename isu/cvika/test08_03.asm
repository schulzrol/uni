; TEST PROSEL

%include "rw32-2018.inc"
section .data
ano  db "ano",0
temp dw 0
section .text

CMAIN:
    mov ebp, esp; for correct debugging
	push ebp
	mov ebp,esp

	; zde muzete psat vas kod
        ; n-th root = 2^(log2(x) / n)
        ;fyl2x ; st0 = log2(st0)
        ;f2xm1 ; raises 2 to the power of ST0, subtracts one, and stores the result back into ST0
        
        xor eax, eax
        call ReadUInt16 ; AX = cislo z intervalu <0, 65535>
        mov ebx, eax
        
        ; reseni 1
        cmp ax, 1
        jz konec ; neni prime
        
        ; reseni 2
        cmp ax, 2; je prime
        jz yes
        
        ; testovani sudosti
        test ax, 1 ; je liche?
        jz konec
        
        ;vypocet square root
        mov [temp], eax
        finit
        fild dword [temp] ; eax
        fsqrt           ; eax^(1/2)
        fistp dword [temp]
        mov ecx, [temp]
        
        .loop1:
            ; nenasli sme nic a uz sme na jednicce
            cmp ecx, 1
            je yes
             
            mov eax, ebx ; puvodni input
            xor edx, edx
            
            div ecx
            cmp edx, 0 ; jestli je REST 0 ...
            je konec   ; ... neni prime
            
            dec ecx
            jmp .loop1
        
        yes: 
        mov esi, ano
        call WriteString ;Vstup:  ESI = ukazatel na retezec zakonceny hodnotou 0
        konec:
        mov esp, ebp
	pop ebp
	mov eax, 0 ; navratova hodnota musi byt 0
	ret