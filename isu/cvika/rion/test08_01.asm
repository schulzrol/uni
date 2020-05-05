; TEST PROSEL
%include "rw32-2018.inc"

section .text

CMAIN:
    mov ebp, esp; for correct debugging
    push ebp
    mov ebp,esp
    ; zacatek 
    xor eax, eax
    call ReadUInt8 ; vysledek v al
    xor ecx, ecx
    cmp al,0 ; pokud by to byla hned nula
        je konec
    mov cl, al
    cmp cl, 4
    jle mensi
    
    ; vetsi, nutno delit
    mov bx, 4
    cwd
    div bx
    cmp dx,0
    je bez_mezery
    jg do_sudosti
    for:
        mov bx, 4
        cwd
        div bx
        cmp dx, 0
        jg do_sudosti
        je mezera
        
        do_sudosti:
            mov al, '#'
            call WriteChar
            dec cl
            mov al, cl
            jmp for
            
        mezera:
            mov al, ' '
            call WriteChar
        bez_mezery: ; pokud sme hned delitelni
            mov bx, 4
            
        mensi_for:
            dec bx
            mov al, '#'
            call WriteChar
            dec cl
            cmp bx, 0
            jg mensi_for
            cmp cl, 0
            jg mezera
            je konec
                
                
    mensi:
        dec cl
        mov al, '#'
        call WriteChar
        cmp cl, 0
        jg mensi
    ; zde bude vas kod
    konec:
    mov esp, ebp
    pop ebp
    mov eax, 0 ; navratova hodnota musi byt 0
    ret