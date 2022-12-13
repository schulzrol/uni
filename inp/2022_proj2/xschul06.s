; Autor reseni: Roland Schulz xschul06

; Projekt 2 - INP 2022
; Vernamova sifra na architekture MIPS64

; DATA SEGMENT
                .data
login:          .asciiz "xschul06"  ; sem doplnte vas login
even_key:       .byte   19 ;s
odd_key:        .byte   -3 ;c

a_offset:       .byte   97 ; ord(a)

cipher:         .space  17  ; misto pro zapis sifrovaneho loginu

params_sys5:    .space  8   ; misto pro ulozeni adresy pocatku
                            ; retezce pro vypis pomoci syscall 5
                            ; (viz nize "funkce" print_string)

; CODE SEGMENT
                .text

main:
                
                ;[x] 1. ulozit sc jako klic
                ;[x] 2. cyklit skrz login

                xor r1, r1, r1
            while_start:
                lb   r4, login(r1) ; r4 = login[r1]
                addi r3, r4, -97   ; r3 = r1 - 97
                bgez r3, pismeno
                b    while_end

            pismeno:
                ; zjistit jestli pricist nebo odecist podle indexu
                xor r0, r0, r0     ; r0 = 0
                addi r3, r0, 2     ; r3 = 2
                div	r1, r3         ; r1 / r3
                mfhi r3	           ; r0 = r4 % r0 
                ; nebo andi r0, r4, 0x1

                bnez r3, liche      
                b sude
            ;[x] 3. vypocitat sifru
            ;  [x] - bude treba pamatovat jestli odcitame/pricitame (jednim vzdy pricitame a druhy vzdy odcitame - ulozit uz se znamenkem)
            ;  [] - zajistit podtekani a pretekani TODO: zajistit spravne podtekani/pretekani
            liche:
                xor r0, r0, r0     ; r0 = 0
                lb r3, odd_key(r0)     ; r3 = ord('c')
            b ulozeni_na_vystup
            sude:
                xor r0, r0, r0     ; r0 = 0
                lb r3, even_key(r0)    ; r3 = ord('s')
            ulozeni_na_vystup: ; predpoklada posun v r3
                add r3, r3, r4 ; r3 = r3 + r4 ; aplikace posunu sifrou
                ; kontroly podteceni a preteceni
                slti r11, r3, 97 ; r11 = (r3 < 97) ? 1 : 0
                bnez r11, podteceni

                addi r11, r0, 122 
                slt r11, r11, r3  ; r11 = (122 < r3) ? 1 : 0
                bnez r11, preteceni
                b v_rozsahu
                
            podteceni:
                addi r3, r3, 26
                b v_rozsahu
            preteceni:
                addi r3, r3, -26
            v_rozsahu:
                sb r3, cipher(r1)
                addi r1, r1, 1     ; r1 = r1 + 1
                b while_start

            while_end:
                addi r1, r1, 1     ; r1 = r1 + 1
                xor r0, r0, r0     ; r0 = 0
                sb r0, cipher(r1)  ; zakoncovaci 0 znak
                
                daddi   r4, r0, cipher   ; vozrovy vypis: adresa login: do r4
                jal     print_string    ; vypis pomoci print_string - viz nize
                syscall 0   ; halt

print_string:   ; adresa retezce se ocekava v r4
                sw      r4, params_sys5(r0)
                daddi   r14, r0, params_sys5    ; adr pro syscall 5 musi do r14
                syscall 5   ; systemova procedura - vypis retezce na terminal
                jr      r31 ; return - r31 je urcen na return address
