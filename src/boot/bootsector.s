.code16
.section .text
.global _start
_start:
    cli
    movw %cs, %ax
    movw %ax, %ss
    movw $0x7c00, %sp
    sti

    movw $0x0FE0, %ax
    movw %ax, %ds
    movw %ax, %es
    movw $0x2, %ax
    movw $0x1, %cx
    movw $msg_readmft, %si
    call kputs
    call read   ;//reading MFT
    movw $msg_ok, %si
    call kputs

    movw $32, %si
    xorw %di, %di
    xorw %dx, %dx
    .loop:
        addw $07, %si
        lodsb
        test %al, %al
        jz .endloop
        cmpb $01, %al
        je .boot
        cmpb $02, %al
        je .boot
        .boot_cont:
        addw $24, %si
        jmp .loop
    .endloop:
        movw $msg_notfound, %si
        call kputs
        .lp:
            hlt
        jmp .lp
    .boot:
    pusha
    movb 07(%si), %cl   ;//sector offset
    movb 15(%si), %ch   ;//sector count
    cmpb $01, %al
    je .mkrn
        movw $msg_iramfs, %si
        movw $0x4000, %ax
        jmp .mout
    .mkrn:
        movw $msg_kernel, %si
        movw $0x1000, %ax
    .mout:
    call kputs
    movw $msg_found, %si
    call kputs
    movw %ax, %es
    movb %cl, %al
    call kputn
    movb $':', %al
    call kputc
    movb %ch, %al
    call kputn
    movw $msg_founde, %si
    call kputs

    xorw %ax, %ax
    movb %cl, %al
    incw %ax
    movb %ch, %cl
    xorb %ch, %ch
    decw %cx
    call read
    movw $msg_ok, %si
    call kputs
    popa

    incw %dx
    cmpw $02, %dx
    jne .boot_cont

    movw %cs, %ax
    movw %ax, %ds
    movw %ax, %es

    movw $msg_boot, %si
    call kputs

    //get cursor coords
    movb $0x03, %ah
    xorw %bx, %bx
    int  $0x10
    //store coords
    push %ds
    movw $0x0999, %ax
    movw %ax, %ds
    movw %dx, 0
    popw %ds

    //hide cursor
    movb $0x01, %ah
    movb $0x20, %ch
    int  $0x10

    //setup base interrupt vector 0x20
    movb $0b00010001, %al
    outb %al, $0x20
    movb $0b00100000, %al
    outb %al, $0x21
    movb $0b00000100, %al
    outb %al, $0x21
    movb $0b00000001, %al
    outb %al, $0x21

    cli

    lgdt gd_reg

    //enable A20
    inb  $0x92, %al
    orb  $02, %al
    outb %al, $0x92

    //set PE-bit
    movl %cr0, %eax
    orb  $01, %al
    movl %eax, %cr0

    //loading segment selector into CS
    jmpl $0x8, $_protected

.code32
_protected: //protected mode here
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    jmpl $0x08, $0x10000

.code16
kputs:
    push %ax
    push %ds
    movw %cs, %ax
    movw %ax, %ds
.kputs_loop:
    lodsb
    test %al, %al
    jz   .kputs_quit
    movb $0x0E, %ah
    int  $0x10
    jmp  .kputs_loop
.kputs_quit:
    popw %ds
    popw %ax
    ret
kputc:
    push %ax
    movb $0x0E, %ah
    int  $0x10
    popw %ax
    ret
kputh:  ;//al - 4-bit number
    push %ax
    cmpb $10, %al
    jge  .kputh_gef
    addb $'0', %al
    jmp .kputh_out
    .kputh_gef:
    addb $'7', %al
    .kputh_out:
    movb $0x0E, %ah
    int  $0x10
    popw %ax
    ret
kputn:  ;//al - 8-bit number
    push %ax
    cmpb $0x0F, %al
    jle .kputn_l
        movb %al, %ah
        shrb $04, %al
        andb $0x0F, %al
        call kputh
        movb %ah, %al
        andb $0x0F, %al
    .kputn_l:
    call kputh
    popw %ax
    ret

read:  //ax - sector N, cx - count, es - buffer
    pusha
    xorw %bx, %bx
    .read_rs:
        push %cx
        push %ax
        movb $'.', %al
        movb $0x0E, %ah
        int $0x10

        popw %ax
        push %ax
        movb $18, %dl
        divb %dl     ;//N / 18
        incb %ah
        movb %ah, %cl;//START_SECTOR    N % 18 + 1

        popw %ax
        push %ax
        movb $36, %dl
        divb %dl     ;//N / 36
        movb %al, %ch;//START_TRACK     N / 36
        movb %ah, %al
        xorb %ah, %ah
        movb $18, %dl
        divb %dl
        movb %al, %dh;//START_HEAD      (N % 36) / 18
        movb $00, %dl;//FLOPPY_ID
        movb $02, %ah
        movb $01, %al;//COUNT
        int $0x13
        addw $512, %bx
        popw %ax
        incw %ax
        popw %cx
    loop .read_rs
    popa
    ret
gdt:
   .word 0, 0, 0, 0 ;//zero descriptor

   .word 0xFFFF       ;//code segment DPL=0 base=0 limit=4G
   .byte 0
   .byte 0
   .byte 0
   .byte 0b10011010
   .byte 0xCF
   .byte 0x00

   .word 0xFFFF       ;//data segment DPL=0 base=0 limit=4G
   .byte 0
   .byte 0
   .byte 0
   .byte 0b10010010
   .byte 0xCF
   .byte 0x00

gd_reg:
   .word 8192
   .long gdt

msg_readmft:    .asciz "reading MFT.."
msg_notfound:   .asciz "somewhat not found!"
msg_kernel:     .asciz "kernel"
msg_iramfs:     .asciz "initramfs"
msg_found:      .asciz " found ["
msg_founde:     .asciz "] loading.."
msg_ok:         .asciz "ok\r\n"
msg_boot:	.asciz "booting kernel..."

.org 510
.byte 0x55, 0xAA
