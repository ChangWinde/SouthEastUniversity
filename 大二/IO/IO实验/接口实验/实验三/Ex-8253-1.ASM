.8086
.model small
.stack
.data
.code
start:
mov ax,@data
mov ds,ax
mov al,00010001B
mov dx,226h
out dx,al
mov al,5
mov dx,220h
out dx,al
end start