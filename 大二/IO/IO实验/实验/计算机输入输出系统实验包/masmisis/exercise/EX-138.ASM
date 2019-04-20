.8086
.MODEL SMALL
.stack
.data
  address  word 240h
.code
start:
    mov ax,@data
    mov ds,ax
    mov dx,address
    mov al,0
    out dx,al
    jmp $
END start
