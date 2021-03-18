; Knihovna pro zakladni pracu s terminalem
; funkce:
;   - vycisteni terminalu
;   - ziskani stlacene klavesy
; implementace:
;   - Linux:   curses
;   - Windows: conio / system
; preklad:
;   - Linux:   pridat curses kniznicu k linkovani (-lcurses)
;   - Windows: modlit se
;

; Vsechny funkce jsou bez parametru a vraceji hodnotu v eax. Zalohuji registre ecx a edx.
; Nektere z funkci nedelaji nic na platforme Windows.
;
; TermInit     - Inicializace curses. Na Windows nedela nic a vraci vzdy 0. Na linuxe vraci ukazatel na WINDOW.
; TermDeinit   - Deinicializace curses.
; TermClear    - Vycisteni terminalu. V curses implementovano pomoci clear, ve Windows pomoci system("cls").
; TermRefresh  - Refresh v curses. Potrebne pro reseni problemu s (ne)aktualizaci okna terminalu.
; TermGetKey   - Volani blokuje dokud uzivatel nestlaci klavesu. Vraci kod nebo hodnotu znaku.

%ifidn __OUTPUT_FORMAT__, win32

%define KEY_ESC    0x001B
%define KEY_ENTER  0x000D
%define KEY_BKSP   0x0008
%define KEY_UP     0x48E0
%define KEY_DOWN   0x50E0
%define KEY_LEFT   0x4BE0
%define KEY_RIGHT  0x4DE0
%define KEY_INS    0x52E0
%define KEY_DEL    0x53E0
%define KEY_HOME   0x47E0
%define KEY_END    0x4FE0
%define KEY_PGUP   0x49E0
%define KEY_PGDOWN 0x51E0

%ifnmacro CEXTERN
%define CEXTERN extern
%endif

CEXTERN _kbhit
CEXTERN _getch
CEXTERN system

section .data
	tk_cls_s db "cls",0

section .text

TermInit:
	xor eax,eax
	ret

TermDeinit:
	xor eax,eax
	ret

TermClear:
	push ecx
    push edx
	push tk_cls_s
	call system
	add esp,4
	pop edx
	pop ecx
	ret

TermRefresh:
	xor eax,eax
	ret

TermGetKey:
	push ecx
	push edx
.retry:
	call _kbhit
	test eax,eax
	jz .retry
	call _getch
	test eax,eax
	jz .extra
	cmp eax,0xE0
	jne .end
.extra:
	call _getch
	shl eax,8
	mov al,0xE0
.end:
	pop edx
	pop ecx
	ret

%elifidn __OUTPUT_FORMAT__, elf32

%define CURSES_ERR -1
%define CURSES_OK  0

%define KEY_ESC    0x001B
%define KEY_ENTER  0x000A
%define KEY_BKSP   0x007F
%define KEY_UP     0x0103
%define KEY_DOWN   0x0102
%define KEY_LEFT   0x0104
%define KEY_RIGHT  0x0105
%define KEY_INS    0x014B
%define KEY_DEL    0x014A
%define KEY_HOME   0x0106
%define KEY_END    0x0168
%define KEY_PGUP   0x0153
%define KEY_PGDOWN 0x0152

extern initscr
extern cbreak
extern noecho
extern keypad
extern endwin
extern getch
extern clear
extern refresh

section .data
	tk_wnd dd 0

section .text

TermInit:
	push ecx
	push edx
	mov eax,[tk_wnd]
	test eax,eax
	jz .initialize
	ret
.initialize:
	call initscr
	test eax,eax
	jz .end
	mov [tk_wnd],eax
	call cbreak
	call noecho
	push dword 1
	push dword [tk_wnd]
	call keypad
	pop eax
	add esp,4
.end:
	pop edx
	pop ecx
	ret

TermDeinit:
	push ecx
	push edx
	mov eax,[tk_wnd]
	test eax,eax
	jz .end
	call endwin
	cmp eax,CURSES_OK
	jne .end
	mov dword [tk_wnd],0
.end:
	pop edx
	pop ecx
	ret

TermClear:
	push ecx
	push edx
	call clear
	pop edx
	pop ecx
	ret

TermRefresh:
	push ecx
	push edx
	call refresh
	pop edx
	pop ecx
	ret

TermGetKey:
	push ecx
	push edx
	call getch
	pop edx
	pop ecx
	ret

%else
	%fatal Zlyhalo zjisteni cilove platformy
%endif
