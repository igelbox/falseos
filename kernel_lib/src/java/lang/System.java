package java.lang;

import fos.drv.chr.Tty;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;

/**
 *
 * @author igel
 */
public class System {

    public static final InputStream in = new InputStream() {

        @Override
        public int read() throws IOException {
            return Tty.readchar();
        }
    };
    public static final PrintStream out = new PrintStream( new OutputStream() {

        @Override
        public void write( int b ) throws IOException {
            Tty.print( (char) b );
        }
    } );
}
