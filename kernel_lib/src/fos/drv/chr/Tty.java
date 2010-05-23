package fos.drv.chr;

import fos.sys.IOPorts;
import fos.sys.Interrupts;

/**
 *
 * @author igel
 */
public class Tty {

    private static final String scancodes = "0-=\b\tqwertyuiop[]\n\rasdfghjkl;'`\r\\zxcvbnm,./";
    private static final String scancodes_shifted = "!@#$%^&*()_+\b\tQWERTYUIOP{}\n\rASDFGHJKL:\"~\r|ZXCVBNM<>?";
    private static boolean shift;
    private static char[] buffer = new char[4096];
    private static int buffer_len = 0;

    public static native void clear();

    public static native char getColor();

    public static native void setColor( char color );

    public static native void print( String s );

    public static native void print( char c );

    public static native void print( int i );

    public static native void print( long l );

    protected static char getchar() {
        int scancode = IOPorts.in( 0x60 );
        switch ( scancode ) {
            //Скан-коды нажатого шифта
            case 0x36:
            case 0x2A:
                shift = true;
                break;
            //Скан-коды отпущенного шифта
            case 0x36 + 0x80:
            case 0x2A + 0x80:
                shift = false;
                break;
            default:
                if ( scancode < 0x80 )
                    //Если клавиша нажата...
                    switch ( scancode ) {
                        case 55:
                            return '*';
                        case 57:
                            return ' ';
                        case 74:
                            return '-';
                        case 78:
                            return '+';
                        default:
                            if ( scancode > 53 )
                                return 0;
                            else if ( scancode > 10 )
                                return shift ? scancodes_shifted.charAt( scancode - 2 ) : scancodes.charAt( scancode - 11 );
                            else if ( scancode > 1 )
                                return shift ? scancodes_shifted.charAt( scancode - 2 ) : (char) ('1' + scancode - 2);
                            else
                                return 0;
                    }
                break;
        }
        return 0;
    }

    public static void handle() {
        char c = getchar();
        if ( c != 0 )
            buffer[buffer_len++] = c;
        int v = IOPorts.in( 0x61 );
        v |= 1;
        IOPorts.out( 0x61, v );
    }

    public static char readchar() {
        while ( buffer_len <= 0 )
            Interrupts.halt();
        char r = buffer[0];
        for ( int i = 1; i < buffer_len; i++ )
            buffer[i - 1] = buffer[i];
        buffer_len--;
        return r;
    }
}
