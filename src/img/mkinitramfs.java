
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;

/* 
 * File:   mkinitramfs.java
 * Author: igel
 *
 * Created on 2 Май 2010 г., 15:30
 */
public class mkinitramfs {

    protected static String rel_path( File f, File r ) throws IOException {
        return f.getCanonicalPath().substring( r.getCanonicalPath().length() + 1 );
    }

    protected static void scan_r( File d, File r, DataOutputStream dos ) throws IOException {
        File[] files = d.listFiles( new FilenameFilter() {

            public boolean accept( File dir, String name ) {
                return name.endsWith( ".class" );
            }
        } );
        if ( files != null )
            for ( File f : files ) {
                String name = rel_path( f, r );
                name = name.substring( 0, name.length() - ".class".length() );
                dos.writeByte( (byte)name.length() );
                dos.write( name.getBytes() );
                dos.writeByte( 0 );
                dos.writeShort( (short) f.length() );
                byte[] data = new byte[(int)f.length()];
                FileInputStream fis = new FileInputStream( f );
                fis.read( data );
                fis.close();
                dos.write( data );
                System.out.println( name );
            }
    }

    public static void main( String[] args ) throws IOException {
        if ( args.length < 2 )
            throw new IllegalArgumentException();
        File f = new File( args[0] );
        DataOutputStream dos = new DataOutputStream( new FileOutputStream( args[1] ) );
        scan_r( f, f, dos );
        dos.writeShort( 0 );
        dos.close();
    }
}
