package java.io;

/**
 *
 * @author igel
 */
public class PrintStream implements Appendable, Closeable, Flushable {

    protected OutputStream out;

    public PrintStream( OutputStream out ) {
        this.out = out;
    }

    @Override
    public void flush() {
        try {
            out.flush();
        } catch ( IOException e ) {
            throw new RuntimeException( e );
        }
    }

    @Override
    public void close() {
        try {
            out.close();
        } catch ( IOException e ) {
            throw new RuntimeException( e );
        }
    }

    public void write( int b ) {
        try {
            out.write( b );
        } catch ( IOException e ) {
            throw new RuntimeException( e );
        }
    }

    public void write( byte buf[], int off, int len ) {
        try {
            out.write( buf, off, len );
        } catch ( IOException e ) {
            throw new RuntimeException( e );
        }
    }

    protected void write( char buf[] ) {
        for ( int i = 0; i < buf.length; i++ )
            write( buf[i] );
    }

    protected void write( String s ) {
        byte[] sb = s.getBytes();
        write( sb, 0, sb.length );
    }

    protected void newLine() {
        write( '\n' );
    }

    public void print( boolean b ) {
        write( b ? "true" : "false" );
    }

    public void print( char c ) {
        write( String.valueOf( c ) );
    }

    public void print( int i ) {
        write( String.valueOf( i ) );
    }

    public void print( long l ) {
        write( String.valueOf( l ) );
    }

    public void print( float f ) {
        write( String.valueOf( f ) );
    }

    public void print( double d ) {
        write( String.valueOf( d ) );
    }

    public void print( char s[] ) {
        write( s );
    }

    public void print( String s ) {
        if ( s == null )
            s = "null";
        write( s );
    }

    public void print( Object obj ) {
        write( String.valueOf( obj ) );
    }

    public void println() {
        newLine();
    }

    public void println( boolean x ) {
        print( x );
        newLine();
    }

    public void println( char x ) {
        print( x );
        newLine();
    }

    public void println( int x ) {
        print( x );
        newLine();
    }

    public void println( long x ) {
        print( x );
        newLine();
    }

    public void println( float x ) {
        print( x );
        newLine();
    }

    public void println( double x ) {
        print( x );
        newLine();
    }

    public void println( char x[] ) {
        print( x );
        newLine();
    }

    public void println( String x ) {
        print( x );
        newLine();
    }

    public void println( Object x ) {
        String s = String.valueOf( x );
        print( s );
        newLine();
    }

    public PrintStream append( CharSequence csq ) {
        if ( csq == null )
            print( "null" );
        else
            print( csq.toString() );
        return this;
    }

    public PrintStream append( CharSequence csq, int start, int end ) {
        CharSequence cs = (csq == null ? "null" : csq);
        write( cs.subSequence( start, end ).toString() );
        return this;
    }

    public PrintStream append( char c ) {
        print( c );
        return this;
    }
}
