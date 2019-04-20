.8086
.model small
.stack
.data
arr byte 3fh, 06h, 5bh, 4fh,66h,6dh,7dh,07h,7fh,6fh,77h,7ch,39h,5eh,79h,71h
.code
start:
mov ax,@data
mov ds,ax
mov al,10000001B
mov dx,206h
out dx,al
mov dx,204h
in al,dx
not al
mov ah,0h
mov dl,16
div dl
mov al,ah
mov ah,0h
mov bx,ax
lea di,arr
mov al,[di+bx]
mov dx,200h
out dx,al
;jmp $
end start