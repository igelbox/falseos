#include "jmemory.h"
#include "../drv/tty.h"
#include "../intslib.h"

void assert( int cond, const char* msg );
extern void tty_put_ui32_hex( unsigned int v );

unsigned int min, max;
void *ptr;

void jmm_test_cell_s( unsigned int ptr ) {
    *(unsigned short*) ptr = 0x5678;
    *(unsigned short*) (ptr + 2) = 0x1234;
}

unsigned int jmm_test_cell_g( unsigned int ptr ) {
    return *(unsigned int*) ptr;
}

int jmm_test_cell( unsigned int ptr ) {
    extern void tty_blink_ctl(int enable);
    tty_blink_ctl( 0 );
    jmm_test_cell_s( ptr );
    tty_blink_ctl( 1 );
    return jmm_test_cell_g( ptr ) == 0x12345678;
}

unsigned int jmm_test_mem( unsigned int low ) {
    i_disable( );
    unsigned int high = low + 0x100000;
    while ( high > low ) {
        /*        tty_putchar( ':' );
                tty_put_ui32( low );
                tty_putchar( ':' );
                tty_put_ui32( high );
                tty_putchar( '\n' );*/
        if ( jmm_test_cell( high ) ) {
            unsigned int l = high - low;
            low = high + 1;
            high = high + l * 2;
        } else {
            unsigned int m = (high + low) / 2;
            if ( jmm_test_cell( m ) )
                low = m + 1;
            else
                high = m - 1;
        }
    }
    i_enable( );
    return low;
}

void jmm_init( ) {
    min = 0x80000;//(unsigned int)&ptr + 16;
    ptr = (void*)min;
    printf("testing memory... ");
    max = jmm_test_mem( min );
    printf("%iMiB found\n", (max + 16) / 1024 / 1024);
}

void* jmm_alloc( int size ) {
    if ( size <= 0 )
        return 0;
    assert( size <= jmm_avail(), "Heap overflow" );
    void *r = ptr;
    ptr += size;
    return r;
}

unsigned int jmm_avail( ) {
    return max - (unsigned int) ptr;
}

unsigned int jmm_max( ) {
    return max - min;
}

unsigned int jmm_used( ) {
    return (unsigned int) ptr - min;
}
