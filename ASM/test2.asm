.486
data segment use16

    Val1 db 101b
    Valb1 db 10
    Valb2 db 0Ah
    Valb3 db 12+12*(6-4)/2

    val2 dw 0111b
    valw1 dw -16
    valw2 dw 45d
    ABCDE dw 32h 

   
    val3 dd 12345678h
    vald1 dd 1101b
    vald2 dd 45d
    vald3 dd 12+6*(6-4)/4

    x db -128
    y dw 2*5*5/5-4+10
    z dd 0FFFFFFFFh

    data ends
code segment use16
assume ds:data, cs:code
 cli
  jmp l1
 jmp L2
    CodeV dw 0ABh

    push CodeV      
    push cs:CodeV   
    push ds:CodeV 

    push val2 
    push es:val3 
    push ds:vald3

        push CodeV      
    push cs:CodeV   
    push ds:CodeV 

    push val2 
    push es:val3 
    push ds:vald3

    bt edi, ebx     
    bt di, ax 

    lea dx, CodeV[di] 
    lea bx, vald1 
    lea eax, val3[ebp] 

    out 10h, al
    out 20h, ax
    out 001b, eax
   
    cmp vald1, 0DEADh     
    cmp val3, 0101b
    cmp vald3,12+6*(6-4)/4
    cmp ABCDE, 32h
 
    jnz mark2
    mark2: 
    jnz mark2

    L1:
    jmp l1
    L2:
code ends
END
