#include "drv/tty.h"
#include "intslib.h"

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
    extern void tty_blink(int ms);
    tty_blink( 1000 / 18 );
    extern void kjvm_fd_handle_t();
    kjvm_fd_handle_t( );
}

IRQ_HANDLER( irq_floppy ) {
    extern void kjvm_fd_handle();
    kjvm_fd_handle( );
    outportb( 0x20, 0x20 );
}

IRQ_HANDLER( irq_keyboard ) {
    extern void kjvm_handle_kdb();
    kjvm_handle_kdb( );
}

void kernel_main( ) {
    extern void tty_init();
    tty_init( );

    extern void i_install(unsigned char vector, void (*func)(), unsigned char type);
    i_install( 0x20, &irq_timer, 0x8e );
    i_install( 0x21, &irq_keyboard, 0x8e );
    i_install( 0x26, &irq_floppy, 0x8e );

    extern void i_setup();
    i_setup( );
    i_enable( );

    printf( "ok\n" );

    extern void kjvm_start();
    kjvm_start( );

    for (;; )
        asm( "hlt" );
}
