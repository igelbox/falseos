inline void outportb( unsigned short port, unsigned char value ) {
    asm( "outb %b0,%w1"::"a"(value), "d"(port) );
}

inline unsigned char inportb( unsigned short port ) {
    char value;
    asm( "inb %w1, %b0" : "=a"(value) : "d"(port) );
    return value;
}

#define IRQ_HANDLER(func) void func (void);\
 asm(#func ": pusha \n call _" #func " \n movb $0x20, %al \n outb %al, $0x20 \n popa \n iret \n");\
 void _ ## func(void)

unsigned int last_used = 0;

IRQ_HANDLER( irq_timer ) {
    tty_blink( 1000 / 18 );
    kjvm_fd_handle_t( );
}

IRQ_HANDLER( irq_floppy ) {
    kjvm_fd_handle( );
    outportb( 0x20, 0x20 );
}

IRQ_HANDLER( irq_keyboard ) {
    kjvm_handle_kdb( );
}

void kernel_main( ) {
    tty_init( );
    i_install( 0x20, &irq_timer, 0x8e );
    i_install( 0x21, &irq_keyboard, 0x8e );
    i_install( 0x26, &irq_floppy, 0x8e );
    i_setup( );
    i_enable( );

    tty_puts( "ok\n" );

    kjvm_start( );

    for (;; )
        asm( "hlt" );
}
