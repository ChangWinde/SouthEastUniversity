; An asm sample for the student
;  YQS  2012.1
;  Assemble by masm 6.14
;  link by link16
.8086
.MODEL SMALL
.STACK   
.DATA   
        BUFF  WORD  3C6DH
        STRING BYTE 'PRINT HEX.',0AH,0DH,'$'
.CODE  
START:  MOV  AX,@DATA
        MOV  DS,AX
        MOV  DX, OFFSET  STRING
        MOV  AH,09H
        INT  21H
        LEA  SI,BUFF
        MOV  BX,[SI]
        MOV  CH,4
A1:     MOV  CL,4
        ROL  BX,CL
        MOV  AL,BL
        AND  AL,0FH
        ADD  AL,30H
        CMP  AL,3AH
        JL   A2
        ADD  AL,7H
A2:     MOV  DL,AL
        MOV  AH,2
        INT  21H
        DEC  CH
        JNZ  A1
        MOV  AH,4CH
        INT  21H
END  START
