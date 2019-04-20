.8086
.model small
.stack
.data
.code
start:
mov ax,@data
mov ds,ax
mov al,00110111B
mov dx,226h
out dx,al
mov ax,1000h
mov dx,220h
out dx,al
mov al,ah
out dx,al
mov al,01110111B
mov dx,226h
out dx,al
mov ax,1000h
mov dx,222h
out dx,al
mov al,ah
out dx,al
mov al,10010111B
mov dx,226h
out dx,al
mov al,5h
mov dx,224h
out dx,al
end start
