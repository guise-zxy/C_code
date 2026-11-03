DATA SEGMENT
    ARY1 DB 6 DUP(0)
    CC1 EQU $-ARY1
    SUM1 DW ?
    PKEY DB "PRESS ANY KEY...$"
ENDS                

SST SEGMENT
    SPAE DB 200 DUP(0)
    TOP EQU 200
ENDS

CODE SEGMENT
    ASSUME CS:CODE,DS:DATA,SS:SST
START:

    MOV AX,DATA
    MOV DS,AX
    
    MOV AX,SST
    MOV SS,AX
    
    MOV SP,TOP
    
    MOV CX,6
MYLOOP1:
    MOV AH,1H
    INT 21H
    SUB AL,30H
    MOV [BX],AL
    INC BX
    LOOP MYLOOP1    
    
    MOV AX,CC1
    PUSH AX
    LEA AX,ARY1
    PUSH AX
    CALL SUM
             
    MOV AX,4CH
    INT 21H 
    
SUM PROC NEAR
    PUSH AX
    PUSH BX
    PUSH CX
    MOV BP,SP
    PUSHF
    MOV CX,[BP+10]
    MOV BX,[BP+8]
    XOR AX,AX
    A1:ADD AL,[BX]
    INC BX
    ADC AH,0
    LOOP A1
    ;MOV [BX],AX
    
    MOV CX,10
    DIV CX  ;用除10操作将得到的结果十位数和个位数分开到ax和dx中
       
    MOV BH,AL
  
    MOV BL,DL ;整合到BX中  
    
    MOV DL,BH 
    
    ADD DL,'0' ;BH和BL分别输出
    MOV AH,2H
    INT 21H
           
    MOV DL,BL 
    
    ADD DL,'0'
    MOV AH,2H
    INT 21H
           
    POPF
    POP CX
    POP BX
    POP AX
    RET 4
SUM ENDP   
ENDS

END START 
