.8086
.model small
.stack
.data
count byte 0h
.code
start:
mov ax,@data
mov ds,ax
mov dx,226h
mov al,00110111B
out dx,al
mov ax,1000h
mov dx,220h
out dx,al
mov al,ah
out dx,al  ;c0
mov dx,226h
mov al,01110111B
out dx,al
mov ax,1000h
mov dx,222h
out dx,al
mov al,ah
out dx,al  ;c1
mov al,10000000B
mov dx,206h
out dx,al
mov al,10000001B
mov dx,200h
out dx,al
cli
mov ax,0
mov es,ax
mov si,0
mov cx,255
l:  mov ax,offset int0
mov es:[si],ax
mov ax, seg int0
mov es:[si+2],ax
add si,4
loop l
mov ax,offset int0
mov es:[si],ax
mov ax,seg int0
mov es:[si+2],ax
mov ax,@data
mov ds,ax 
sti
mov al,00010011b  ;  init  8259
mov dx,210h
out dx,al         ; ICW1
MOV AL,00001000B;00001000B
MOV DX,212H
OUT DX,AL
MOV AL,01H
OUT DX,AL
jmp $
int0 proc
PUSH SI
PUSH AX
PUSH DX
PUSH DS
mov ax,@data
mov ds,ax
mov al,count
inc al
mov count,al
cmp al,10
jz do1
cmp al,11
jz do2
cmp al,12
jz do1
cmp al,13
jz do2
cmp al,14
jz do3
cmp al,24
jz do4
cmp al,25
jz do5
cmp al,26
jz do4
cmp al,27
jz do5
cmp al,28
jz do6
jmp exit
do6:
mov al,10000001B ;zuo lv, you hong
mov dx,200h
out dx,al
mov al,0
mov count,al
jmp exit
do1:
mov al,01000001B ;zuo huang deng liang
mov dx,200h
out dx,al
jmp exit
do2:
mov al,00000001B ;zuo huang deng an
mov dx,200h
out dx,al
jmp exit
do3:
mov al,00100100B ;zuo hong, you lv
mov dx,200h
out dx,al
jmp exit
do4:
mov al,00100010B;you huang deng liang
mov dx,200h
out dx,al
jmp exit
do5:
mov al,00100000B ;you huang deng an
mov dx,200h
out dx,al
jmp exit
exit:
MOV AL,20H
MOV DX,210H
OUT DX,AL
POP DS
POP DX
POP AX
POP SI
iret
int0 endp
end start
