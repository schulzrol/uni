%include "rw32-2018.inc"

section .data
    task21A dd 3072,1792,-2304,-125,2304,0,-15360,-13824
    task21B dd 2030612559,-1703288326,-1606038169,-531205017,1794362603,791513907,-34536631,1782939213
    task22A dw 192,112,-144,-64,144,0,-960,-864
    task22B dw 26322,-1646,6021,-26035,4723,-8267,-17578,30333
    task23A dw 384,304,48,128,336,192,-768,-672

section .text
CMAIN:
    mov ebp, esp; for correct debugging
    push ebp
    mov ebp,esp
                
    mov eax,task21A
    mov EBX,-125
    mov ecx,8
    call task21
    add esp, 4*3

    
    ; eax = task22(task22A,8,0)
    push 0
    push 8
    push task22A
    call task22
    add esp, 4*3
    call WriteInt32
        
    mov ecx,5
    push ecx
    call task23
    add esp, 4
    
    pop ebp
    ret    
;
;--- Task 1 ---
;
; Create a function 'task21' to find a value in an array of the 32bit signed values.  
; Pointer to the array is in the register EAX, the value to be found is in the register EBX 
; and the count of the elements of the array is in the register ECX.
;
; Function parameters:
;   EAX = pointer to the array of the 32bit signed values (EAX is always a valid pointer)
;   EBX = 32bit signed value to be found
;   ECX = count of the elements of the array (ECX is an unsigned 32bit value, always greater than 0)
;
; Return values:
;   EAX = 1, if the value has been found in the array, otherwise EAX = 0
;
; Important:
;   - the function does not have to preserve content of any register
;
task21:
    ; insert your code here
    push ebp
    mov ebp, esp

.loopjump:
    cmp ebx, [eax]
    je nasli
    lea eax, [eax+4]
    loop .loopjump
    
    mov eax, 0
    jmp konec
    
nasli:
    mov eax, 1
konec:
    mov esp, ebp
    pop ebp
    ret
;
;--- Task 2 ---
;
; Create a function: void* task22(const short *pA, int N, short x) to search an array pA of N 16bit signed 
; values for the last occurrence of the value x. The function returns pointer to the value in the array.
; The parameters are passed, the stack is cleaned and the result is returned according to the CDECL calling convention.
;
; Function parameters:
;   pA: pointer to the array A to search in
;    N: length of the array A
;    x: value to be searched for
;
; Return values:
;   EAX = 0 if the pointer pA is invalid (pA == 0) or N <= 0 or the value x has not been found in the array
;   EAX = pointer to the value x in the array (the array elements are indexed from 0)
;
; Important:
;   - the function MUST preserve content of all the registers except for the EAX and flags registers.
;
task22:
    
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx
    ; v pohode
    mov ebx, [ebp+8] ; pa
    mov ecx, [ebp+12]; N
    mov ax, [ebp+16] ; x


    cmp ebx, 0 ; kontrola pA
    je zero_return
    cmp ecx, 0 ; kontrola N
    jle zero_return
    

    
loopjump2:
    cmp ax, [ebx]
    jne nenasli
    mov edx, [ebp+12]
    sub edx, ecx
nenasli:
    lea ebx, [ebx+2]
    loop loopjump2

value_return:
    mov eax, edx
    
    pop edx
    pop ecx
    pop ebx
    mov esp, ebp
    pop ebp
    ret

zero_return:
    mov eax, 0
    pop edx
    pop ecx
    pop ebx
    mov esp, ebp
    pop ebp
    ret
    

;
;--- Task 3 ---
;
; Create a function 'task23' to allocate and fill an array of the 16bit unsigned elements by the
; Fibonacci numbers F(0), F(1), ... , F(N-1). Requested count of the Fibonacci numbers is 
; in the register ECX (32bit signed integer) and the function returns a pointer to the array  
; allocated using the 'malloc' function from the standard C library in the register EAX.
;
; Fibonacci numbers are defined as follows:
;
;   F(0) = 0
;   F(1) = 1
;   F(n) = F(n-1) + F(n-2)
;
; Function parameters:
;   ECX = requested count of the Fibonacci numbers (32bit signed integer).
;
; Return values:
;   EAX = 0, if ECX <= 0, do not allocate any memory and return value 0 (NULL),
;   EAX = 0, if memory allocation by the 'malloc' function fails ('malloc' returns 0),
;   EAX = pointer to the array of N 16bit unsigned integer elements of the Fibonacci sequence.
;
; Important:
;   - the function MUST preserve content of all the registers except for the EAX and flags registers,
;   - the 'malloc' function may change the content of the ECX and EDX registers.
;
; The 'malloc' function is defined as follows: 
;
;   void* malloc(size_t N)
;     N: count of bytes to be allocated (32bit unsigned integer),
;     - in the EAX register it returns the pointer to the allocated memory,
;     - in the EAX register it returns 0 (NULL) in case of a memory allocation error,
;     - the function may change the content of the ECX and EDX registers.
task23:
CEXTERN malloc
    push ebp
    mov ebp, esp
    push ecx
    push edx
    
    cmp ecx, 0
    jle chyba
    
    shl ecx, 1
    push ecx
    call malloc
    add esp, 4
    
    cmp eax, 0
    je chyba
    
    mov [eax], word 0
    cmp ecx, 1
    jle koneckoncu
    mov [eax+2], word 1

koneckoncu: 
    pop edx
    pop ecx
    mov esp, ebp
    pop ebp
    ret
    
chyba:
    mov eax, 0
    
    pop edx
    pop ecx
    mov esp, ebp
    pop ebp
    ret
