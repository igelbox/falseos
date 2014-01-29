package apps;

import fos.drv.blk.Floppy;
import fos.drv.chr.Tty;
import java.io.IOException;

/**
 *
 * @author igel
 */
public class hello {

    protected static String about = "falseOS is a true operating system based on java\n\t[writted by Vakhurin Sergey aka igel]\n";
    protected static String uc = "Unknown command: ";
    protected static String help = "\tone of these command expected:\n\t\tclear - clear the screen\n\t\tfree  - show memory usage\n\t\tfloppy- testing floppy controller\n\t\treboot - rebooting computer\n\t\tabout - show greeting\n";
    protected static String promt = "[fos]$ ";
    protected static String s = new String();

    protected static void printm( int m ) {
        if ( m > 5 * 1024 )
            if ( m > 5 * 1024 * 1024 ) {
                System.out.print( m / (1024 * 1024) );
                System.out.print( 'M' );
            } else {
                System.out.print( m / 1024 );
                System.out.print( 'K' );
            }
        else
            System.out.print( m );
    }

    public static void main( String[] args ) throws IOException {
        Tty.print( promt );
        int c;
        while ( (c = System.in.read()) >= 0 ) {
            System.out.print( (char) c );
            if ( c == '\b' )
                s.remove( 1 );
            else if ( c == '\n' ) {
                if ( s.length() > 0 ) {
                    if ( s.equals( "about" ) )
                        Tty.print( about );
                    else if (s.equals("reboot"))
                        reboot();
                    else if ( s.equals( "clear" ) )
                        Tty.clear();
                    else if ( s.equals( "floppy" ) )
                        Floppy.init();
                    else if ( s.equals( "free" ) ) {
                        Runtime rt = Runtime.getRuntime();
                        long free = rt.freeMemory();
                        long used = rt.maxMemory() - free;
                        printm( (int) used );
                        Tty.print( '/' );
                        printm( (int) free );
                        Tty.print( " used/free\n" );
                    } else {
                        Tty.print( uc );
                        Tty.print( s );
                        Tty.print( '\n' );
                        Tty.print( help );
                    }
                    s.clear();
                }
                Tty.print( promt );
            } else
                s.append( (char) c );
        }
    }

    private static void reboot() {
        int good = 0x02;
        while ((good & 0x02) != 0)
            good = fos.sys.IOPorts.in(0x64);
        fos.sys.IOPorts.out(0x64, 0xFE);
        fos.sys.Interrupts.halt();
    }
}
