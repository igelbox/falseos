package fos.drv.blk;

import fos.drv.chr.Tty;
import fos.sys.IOPorts;
import fos.sys.Interrupts;

/**
 *
 * @author igel
 */
public class Floppy {

    protected static final int FDC_DOR = 0x3f2;
    protected static final int FDC_MSR = 0x3f4;
    protected static final int FDC_DRS = 0x3f4;
    protected static final int FDC_DATA = 0x3f5;
    protected static final int FDC_DIR = 0x3f7;
    protected static final int CMD_SPECIFY = 0x03;
    protected static final int CMD_RECAL = 0x07;
    protected static final int CMD_SENSEI = 0x08;
    protected static final int CMD_SEEK = 0x0F;
    protected static final int CMD_VERSION = 0x10;
    protected static int timeLeft, statusSize, sr0, track;
    protected static boolean done, changed, motor;

    protected static void delay() {
        IOPorts.in( (short) 0x80 );
    }

    protected static void sendByte( int b ) {
        for ( int tmo = 0; tmo < 128; tmo++ ) {
            int msr = IOPorts.in( FDC_MSR );
            if ( (msr & 0xc0) == 0x80 ) {
                IOPorts.out( FDC_DATA, b );
                return;
            }
            delay();
        }
    }

    protected static int recvByte() {
        for ( int tmo = 0; tmo < 128; tmo++ ) {
            int msr = IOPorts.in( FDC_MSR );
            if ( (msr & 0xd0) == 0xd0 )
                return IOPorts.in( FDC_DATA );
            delay();
        }
        return -1;
    }

    protected static boolean wait_done_ms( int ms ) {
        //done = false;
        timeLeft = ms * 18 / 1000;
        while ( !done && (timeLeft > 0) );//            Interrupts.halt();
        return done;
    }

    protected static void wait_ms( int ms ) {
        timeLeft = ms * 18 / 1000;
        while ( timeLeft > 0 );//            Interrupts.halt();
    }

    protected static boolean wait( boolean sensei ) {
        boolean d = wait_done_ms( 1000 );
        statusSize = 0;
        while ( (statusSize < 7) && ((IOPorts.in( FDC_MSR ) & 16) != 0) ) {
            recvByte();
            statusSize++;
        }
        if ( sensei ) {
            sendByte( CMD_SENSEI );
            sr0 = recvByte();
            track = recvByte();
        }
        done = false;
        if ( !d ) {
            if ( (IOPorts.in( FDC_DIR ) & 0x80) != 0 )
                changed = true;
            Tty.print( "Wait floppy irq error!\n" );
            return false;
        }
        return true;
    }

    protected static void motor( boolean on ) {
        if ( motor == on )
            return;
        if ( on ) {
            IOPorts.out( FDC_DOR, 0x1c );
            motor = true;
//            wait_ms( 500 );
        }
    }

    protected static boolean seek( int trk ) {
        if ( track == trk )
            return true;
        motor( true );
        sendByte( CMD_SEEK );
        sendByte( 0 );
        sendByte( trk );
        if ( !wait( true ) )
            return false;
//        wait_ms( 100 );
        motor( false );
        return sr0 == 0x20;
    }

    protected static void recalibrate() {
        motor( true );
        sendByte( CMD_RECAL );
        sendByte( 0 );
        wait( true );
        motor( false );
    }

    protected static void reset() {
        IOPorts.out( FDC_DOR, 0 );
        IOPorts.out( FDC_DRS, 0 );
        IOPorts.out( FDC_DOR, 0x0c );
        done = true;
        wait( true );
        sendByte( CMD_SPECIFY );
        sendByte( 0xdf );
        sendByte( 0x02 );
        seek( 1 );
        recalibrate();
        changed = false;
    }

    public static void handler() {
        done = true;
    }

    public static void handlert() {
        if ( timeLeft > 0 )
            timeLeft--;
    }

    public static void init() {
        Tty.print( "testing floppy..." );
        reset();
        sendByte( CMD_VERSION );
        int i = recvByte();
        if ( i == 0x80 )
            Tty.print( "NEC765 controller found\n" );
        else
            Tty.print( "enhanced controller found\n" );
    }
}
