package java.io;

/**
 *
 * @author igel
 */
public abstract class OutputStream implements Flushable, Closeable {

    public abstract void write( int b ) throws IOException;

    public void write( byte buf[], int off, int len ) throws IOException {
        for ( int i = off; i < off + len; i++ )
            write( buf[i] );
    }

    public void flush() throws IOException {
//        throw new UnsupportedOperationException( "Not supported yet." );
    }

    public void close() throws IOException {
//        throw new UnsupportedOperationException( "Not supported yet." );
    }
}
