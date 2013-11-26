#include "drv/tty.h"
#include "intslib.h"

void outportb( unsigned short port, unsigned char value ) {
    __asm__( "outb %b0,%w1"::"a"(value), "d"(port) );
}

unsigned char inportb( unsigned short port ) {
    char value;
    __asm__( "inb %w1, %b0" : "=a"(value) : "d"(port) );
    return value;
}

#define IRQ_HANDLER(func) void func (void);\
 __asm__(#func ": pusha \n call _" #func " \n movb $0x20, %al \n outb %al, $0x20 \n popa \n iret \n");\
 void _ ## func(void)

#define IRQ_TRAP(name) void __attribute__((cdecl)) irq_##name(unsigned cs, unsigned ip) { printf("irq_"#name"!%x:%x", cs, ip); halt(); }

void halt() { for (;;) __asm__( "hlt" ); }
IRQ_TRAP(xxx)
IRQ_TRAP(dvz)//divizion by zero
IRQ_TRAP(nmi)//non-maskable interrupt
IRQ_TRAP(bpe)//breakpoint exception
IRQ_TRAP(iop)//invalid opcode
IRQ_TRAP(snp)//segment not present
IRQ_TRAP(ssf)//stack-segment fault
IRQ_TRAP(gpf)//general protection fault
IRQ_TRAP(_pf)//page fault
IRQ_TRAP(_mc)//machine check

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
    unsigned i;
    for (i = 0; i < 0xFF; i++)
        i_install(i, &irq_xxx, 0x8e);
    i_install( 0x00, &irq_dvz, 0x8e );
    i_install( 0x02, &irq_nmi, 0x8e );
    i_install( 0x03, &irq_bpe, 0x8e );
    i_install( 0x06, &irq_iop, 0x8e );
    i_install( 0x0B, &irq_snp, 0x8e );
    i_install( 0x0C, &irq_ssf, 0x8e );
    i_install( 0x0D, &irq_gpf, 0x8e );
    i_install( 0x0E, &irq__pf, 0x8e );
    i_install( 0x12, &irq__mc, 0x8e );
    i_install( 0x20, &irq_timer, 0x8e );
    i_install( 0x21, &irq_keyboard, 0x8e );
    i_install( 0x26, &irq_floppy, 0x8e );

    extern void i_setup();
    i_setup( );
    i_enable( );

    printf( "ok\n" );

    extern void kjvm_start();
    kjvm_start( );

    halt();
}
