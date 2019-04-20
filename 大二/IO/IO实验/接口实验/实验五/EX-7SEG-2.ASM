.8086
.model small
.stack
.data
 arr byte 3fh, 06h, 5bh, 4fh,66h,6dh,7dh,07h,7fh,6fh,77h,7ch,39h,5eh,79h,71h
.code
start:
mov ax,@data
mov ds,ax
mov al,10001001B
mov dx,206h
out dx,al
lea di,arr
lp:mov dx,204h
in al,dx
not al
mov ah,0
mov dl,16
div dl
mov si,ax
mov al,01h
mov dx,202h
out dx,al
mov bx,si
mov bh,0
mov dx,200h
mov al,[di+bx]
out dx,al
mov cx,64h
loop $
mov al,02h
mov dx,202h
out dx,al
mov bx,si
mov bl,bh
mov bh,0
mov dx,200h
mov al,[di+bx]
out dx,al
mov cx,64h
loop $
jmp lp
end start
