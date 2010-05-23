#define V_WIDTH 80
#define V_HEIGHT 25
#define V_MEMORY 0xB8000

#include <stdarg.h>

int tty_cursor, blink, blinktmout;
char tty_color;

int tty_get_cursor( ) {
    return tty_cursor;
}

void tty_set_cursor( int c ) {
    tty_cursor = c;
}

char tty_get_color( ) {
    return tty_color;
}

void tty_set_color( char c ) {
    tty_color = c;
}

void tty_init( ) {
    short dx = *((short*) 0x9990);
    tty_cursor = (dx >> 8) * V_WIDTH + (dx & 0xFF);
    //    scancodes = "0-=\b\tqwertyuiop[]\n\0asdfghjkl;'`\0\\zxcvbnm,./";
    //    scancodes_shifted = "!@#$%^&*()_+\b\tQWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?";
    tty_color = 7;
}

void tty_outchar( char c, char clr ) {
    char *video = (char*) V_MEMORY;
    *(video + tty_cursor * 2) = c;
    *(video + tty_cursor * 2 + 1) = clr;
}

void tty_blink_ctl( int enable ) {
    blink = enable ? 0 : -1;
    tty_outchar( enable ? '#' : ' ', 7 );
}

void tty_blink_sync( ) {
    tty_outchar( blink ? '#' : ' ', 7 );
}

void tty_blink( int ms ) {
    if ( blink < 0 )
        return;
    blinktmout -= ms;
    if ( blinktmout < 0 ) {
        blinktmout = 300;
        tty_blink_sync( );
        blink = blink ? 0 : 1;
    }
}

void tty_clear( ) {
    char *video = (char*) V_MEMORY;
    int i;
    for ( i = 0; i < V_HEIGHT * V_WIDTH; i++ ) {
        *(video + i * 2) = ' ';
    }
    tty_cursor = 0;
}

void tty_putchar( char c ) {
    char *video = (char*) V_MEMORY;
    int i;
    switch ( c ) {
        case '\b':
            if ( !tty_cursor )
                return;
            tty_blink_ctl( 0 );
            tty_cursor--;
            tty_blink_ctl( 1 );
            return;
        case '\t':
            tty_putchar( ' ' );
            tty_putchar( ' ' );
            break;
        case '\n':
            tty_blink_ctl( 0 );
            tty_cursor += V_WIDTH;
            tty_cursor -= tty_cursor % V_WIDTH;
            tty_blink_ctl( 1 );
            break;
        default:
            tty_outchar( c, tty_color );
            tty_cursor++;
            tty_blink_ctl( 1 );
            break;
    }
    if ( tty_cursor >= (V_WIDTH * V_HEIGHT) ) {
        tty_blink_ctl( 0 );
        for ( i = V_WIDTH * 2; i <= V_WIDTH * V_HEIGHT * 2 + V_WIDTH * 2; i++ )
            *(video + i - V_WIDTH * 2) = *(video + i);
        tty_cursor -= V_WIDTH;
        tty_blink_ctl( 1 );
    }
}

void tty_puts( const char *s ) {
    while ( *s ) {
        tty_putchar( *s );
        s++;
    }
}

char tty_hex( unsigned char b ) {
    if ( b < 10 )
        return '0' + b;
    return 'a' + (b - 10);
}

void tty_put_ui32_hex( unsigned int v ) {
    char *buff = "0x00000000";
    for ( int i = 0; i < 8; i++ )
        buff[2 + (7 - i)] = tty_hex( (v >> (i * 4)) & 0xF );
    tty_puts( buff );
}

void tty_put_ui32( unsigned int v ) {
    char buff[11];
    int idx = 10;
    buff[idx] = 0;
    while ( v >= 10 ) {
        buff[--idx] = '0' + (v % 10);
        v = v / 10;
    }
    buff[--idx] = '0' + v;
    tty_puts( &buff[idx] );
}

/*void tty_put_ui64( unsigned long long v ) {
    if ( v <= 0xFFFFFFFF ) {
        tty_put_ui32( (unsigned int) v );
        return;
    }
    char buff[21];
    int idx = 20;
    buff[idx] = 0;
    while ( v >= 10 ) {
        buff[--idx] = '0' + (v % 10);
        v = v / 10;
    }
    buff[--idx] = '0' + v;
    tty_puts( &buff[idx] );
}*/

void tty_put_i32( int v ) {
    if ( v < 0 ) {
        tty_putchar( '-' );
        v = -v;
    }
    tty_put_ui32( v );
}

void tty_put_i64( long long v ) {
    if ( v < 0 ) {
        tty_putchar( '-' );
        v = -v;
    }
    tty_put_ui32( v );
}

void vprintf( const char* fmt, va_list args ) {
    while ( *fmt ) {
        switch ( *fmt ) {
            case '%':
                fmt++;
                switch ( *fmt ) {
                    case 's':
                        tty_puts( va_arg( args, char * ) );
                        break;
                    case 'c':
                        tty_putchar( va_arg( args, unsigned int ) );
                        break;
                    case 'i':
                        tty_put_ui32( va_arg( args, unsigned int ) );
                        break;
                    case 'x':
                        tty_put_ui32_hex( va_arg( args, unsigned int ) );
                        break;
                }
                break;
            default:
                tty_putchar( *fmt );
                break;
        }
        fmt++;
    }
}

void printf( const char* fmt, ... ) {
    va_list args;
    va_start( args, fmt );
    vprintf( fmt, args );
    va_end( args );
}
