[BITS 16]
[ORG 0x7C00]
entry:
    cli
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, entry
    sti

    mov ax, 0x0FE0
    mov ds, ax
    mov es, ax
    mov ax, 2
    mov cx, 1
    mov si, msg_readmft
    call kputs
    call read	;читаем MFT
    mov si, msg_ok
    call kputs
    mov si, 32
    xor di, di
    xor dx, dx
    .loop:
        add si, 7
        lodsb
        test al, al
        jz short .endloop
        cmp al, 1
        je short .boot
        cmp al, 2
        je short .boot
        .boot_cont:
        add si, 24
        jmp short .loop
    .endloop:
        mov si, msg_notfound
        call kputs
        .lp:
            hlt
        jmp short .lp
    .boot:
    pusha
    mov cl, [si+7];начальный сектор
    mov ch, [si+7+8];кол-во секторов
    cmp al, 1
    je short .mkrn
        mov si, msg_iramfs
        mov ax, 0x4000
        jmp short .mout
    .mkrn:
        mov si, msg_kernel
        mov ax, 0x1000
    .mout:
    mov es, ax
    call kputs
    mov si, msg_found
    call kputs
    mov al, cl
    call kputn
    mov al, ':'
    call kputc
    mov al, ch
    call kputn
    mov si, msg_founde
    call kputs

    xor ax, ax
    mov al, cl
    inc ax
    mov cl, ch
    xor ch, ch
    dec cx
    call read
    mov si, msg_ok
    call kputs
    popa

    inc dx
    cmp dx, 2
    jne .boot_cont

    mov ax, cs
    mov ds, ax
    mov es, ax

    mov si, msg_boot
    call kputs

    mov ah, 0x03
    xor bx, bx
    int 0x10
    push ds
    mov ax, 0x999
    mov ds, ax
    mov [0x0], dx
    pop ds

    ; уберем курсор
    mov ah, 1
    mov ch, 0x20
    int 0x10

    ;; Установим базовый вектор контроллера прерываний в 0x20
    mov al,00010001b
    out 0x20,al
    mov al,0x20
    out 0x21,al
    mov al,00000100b
    out 0x21,al
    mov al,00000001b
    out 0x21,al

    ;; Отключим прерывания
    cli

    ;; Загрузка регистра GDTR:
    lgdt [gd_reg]

    ;; Включение A20:
    in al, 0x92
    or al, 2
    out 0x92, al

    ;; Установка бита PE регистра CR0
    mov eax, cr0
    or al, 1
    mov cr0, eax

    ;; С помощью длинного прыжка мы загружаем
    ;; селектор нужного сегмента в регистр CS
    ;; (напрямую это сделать нельзя)
    jmp 0x8: _protected

    ;; Следующий код - 32-битный
[BITS 32]
    ;; Сюда будет передано управление
    ;; после входа в PM
_protected:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    jmp 0x10000

[BITS 16]
kputs:
    push ax
    push ds
    mov ax, cs
    mov ds, ax
.loop:
    lodsb
    test al, al
    jz .quit
    mov ah, 0x0E
    int 0x10
    jmp short .loop
.quit:
    pop ds
    pop ax
    ret

kputc:
    push ax
    mov ah, 0x0E
    int 0x10
    pop ax
    ret

kputh:  ;al - число 4 бит
    push ax
    cmp al, 10
    jge .gef
    add al, '0'
    jmp short .out
    .gef:
    add al, ('A' - 10)
    .out:
    mov ah, 0x0E
    int 0x10
    pop ax
    ret

kputn:  ;al - число 8 бит
    push ax
    cmp al, 0x0F
    jle short .l
        mov ah, al
        shr al, 4
        and al, 0x0F
        call kputh
        mov al, ah
        and al, 0x0F
    .l:
    call kputh
    pop ax
    ret

read:  ;ax - № сектора, cx - количество, es - буффер
    pusha
    xor bx, bx
    .rs:
        push cx
        push ax
        mov al, '.'
        mov ah, 0x0E
        int 0x10

        pop ax
        push ax
        mov dl, 18
        div dl  ;№ / 18
        inc ah
        mov cl, ah ;START_SECTOR    № % 18 + 1

        pop ax
        push ax
        mov dl, 36
        div dl  ;№ / 36
        mov ch, al ;START_TRACK     № / 36
        mov al, ah
        xor ah, ah
        mov dl, 18
        div dl
        mov dh, al ;START_HEAD      (№ % 36) / 18
        mov dl, 0 ;FLOPPY_ID
        mov ah, 2
        mov al, 1 ;COUNT
        int 0x13
        add bx, 512
        pop ax
        inc ax
        pop cx
    loop .rs
    popa
    ret

[BITS 32]
gdt:
	dw 0, 0, 0, 0	; Нулевой дескриптор

	db 0xFF		; Сегмент кода с DPL=0
	db 0xFF		; Базой=0 и Лимитом=4 Гб
	db 0x00
	db 0x00
	db 0x00
	db 10011010b
	db 0xCF
	db 0x00

	db 0xFF		; Сегмент данных с DPL=0
	db 0xFF		; Базой=0 и Лимитом=4Гб
	db 0x00
	db 0x00
	db 0x00
	db 10010010b
	db 0xCF
	db 0x00

    ;; Значение, которое мы загрузим в GDTR:
gd_reg:
    dw 8192
    dd gdt

[BITS 16]
msg_readmft:    db "reading MFT..", 0
msg_notfound:   db "somewhat not found!", 0
msg_kernel:     db "kernel", 0
msg_iramfs:     db "initramfs", 0
msg_found:      db " found [", 0
msg_founde:     db "] loading..", 0
msg_ok:         db "ok", 0x0A, 0x0D, 0
msg_boot:	db "booting kernel...", 0
TIMES 510 - ($-$$) db 0
db 0xAA, 0x55
