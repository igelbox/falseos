package java.lang;

/**
 *
 * @author igel
 */
public class String implements CharSequence {

    protected char[] data;
    protected int length;

    public void clear() {
        length = 0;
    }

    public void append( char c ) {
        int cap = data == null ? 0 : data.length;
        if ( (length + 1) > cap ) {
            char[] _data = new char[(cap + 1) * 2];
            for ( int i = 0; i < length; i++ )
                _data[i] = data[i];
            data = _data;
        }
        data[length] = c;
        length++;
    }

    public void remove( int cnt ) {
        length -= cnt;
        if ( length < 0 )
            length = 0;
    }

    public int length() {
        return length;
    }

    public int indexOf( char c ) {
        for ( int i = 0; i < length; i++ )
            if ( data[i] == c )
                return i;
        return -1;
    }

    public String reverse() {
        String r = new String();
        r.data = new char[length];
        r.length = length;
        for ( int i = 0; i < length; i++ )
            r.data[i] = data[length - i - 1];
        return r;
    }

    public static String valueOf( char c ) {
        String r = new String();
        r.append( c );
        return r;
    }

    public static String valueOf( int i ) {
        String r = new String();
        while ( i >= 10 ) {
            r.append( (char) ('0' + (i % 10)) );
            i = i / 10;
        }
        r.append( (char) ('0' + i) );
        return r.reverse();
    }

    public static String valueOf( long l ) {
        throw new UnsupportedOperationException();
    }

    public static String valueOf( float f ) {
        throw new UnsupportedOperationException();
    }

    public static String valueOf( double d ) {
        throw new UnsupportedOperationException();
    }

    public static String valueOf( Object o ) {
        throw new UnsupportedOperationException();
    }

    public native boolean equals( String s );

    public native char charAt( int idx );

    public native String substring( int idx );

    public native String substring( int idx, int end );

    public native boolean contains( CharSequence s );

    public native boolean endsWith( String s );

    public byte[] getBytes() {
        int len = length();
        byte[] r = new byte[len];
        for ( int i = 0; i < len; i++ )
            r[i] = (byte) data[i];
        return r;
    }

    public native CharSequence subSequence( int start, int end );
}
