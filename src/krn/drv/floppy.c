typedef unsigned char byte;

typedef struct {
    byte page; /* page register */
    byte offset; /* offset register */
    byte length; /* length register */
} DmaChannel;

#define FALSE 0
#define TRUE 1
/* drive geometries */
#define DG144_HEADS       2     /* heads per drive (1.44M) */
#define DG144_TRACKS     80     /* number of tracks (1.44M) */
#define DG144_SPT        18     /* sectors per track (1.44M) */
#define DG144_GAP3FMT  0x54     /* gap3 while formatting (1.44M) */
#define DG144_GAP3RW   0x1b     /* gap3 while reading/writing (1.44M) */
/* IO ports */
#define FDC_DOR  (0x3f2)   /* Digital Output Register */
#define FDC_MSR  (0x3f4)   /* Main Status Register (input) */
#define FDC_DRS  (0x3f4)   /* Data Rate Select Register (output) */
#define FDC_DATA (0x3f5)   /* Data Register */
#define FDC_DIR  (0x3f7)   /* Digital Input Register (input) */
#define FDC_CCR  (0x3f7)   /* Configuration Control Register (output) */
/* command bytes (these are 765 commands + options such as MFM, etc) */
#define CMD_SPECIFY (0x03)  /* specify drive timings */
#define CMD_WRITE   (0xc5)  /* write data (+ MT,MFM) */
#define CMD_READ    (0xe6)  /* read data (+ MT,MFM,SK) */
#define CMD_RECAL   (0x07)  /* recalibrate */
#define CMD_SENSEI  (0x08)  /* sense interrupt status */
#define CMD_FORMAT  (0x4d)  /* format track (+ MFM) */
#define CMD_SEEK    (0x0f)  /* seek track */
#define CMD_VERSION (0x10)  /* FDC version */

const static DmaChannel dmainfo[] = {
    {0x87, 0x00, 0x01},
    {0x83, 0x02, 0x03},
    {0x81, 0x04, 0x05},
    {0x82, 0x06, 0x07}
};

byte done = FALSE, motor = FALSE;
byte status[7] = {0};
byte status_size = 0;
byte sr0 = 0;
byte track = 0;
byte changed = FALSE;
volatile byte mofftimeleft = 0;
volatile byte fd_timeleft = 0;
byte floppy_buffer[512];

void tty_puts( const char *s );
void outportb( unsigned short port, unsigned char value );
char inportb( unsigned short port );
void i_enable( );
void i_disable( );
byte fd_brw( int block, byte *blockbuff, byte read );

byte wait_condition_ms( long ms, byte* cond ) {
    fd_timeleft = ms * 18 / 1000;
    while ( !(cond && *cond) && fd_timeleft );
    return fd_timeleft;
}

byte wait_ms( long ms ) {
    return wait_condition_ms( ms, 0 );
}

byte wait_done_ms( long ms ) {
    return wait_condition_ms( ms, &done );
}

/* sendbyte() routine from intel manual */
void fd_sendbyte( int count ) {
    for ( int tmo = 0; tmo < 128; tmo++ ) {
        int msr = inportb( FDC_MSR );
        if ( (msr & 0xc0) == 0x80 ) {
            outportb( FDC_DATA, count );
            return;
        }
        inportb( 0x80 ); /* delay */
    }
}

/* getbyte() routine from intel manual */
int fd_getbyte( ) {
    for ( int tmo = 0; tmo < 128; tmo++ ) {
        int msr = inportb( FDC_MSR );
        if ( (msr & 0xd0) == 0xd0 )
            return inportb( FDC_DATA );
        inportb( 0x80 ); /* delay */
    }
    return -1; /* read timeout */
}

/* this waits for FDC command to complete */
byte fd_wait( byte sensei ) {
    /* wait for IRQ6 handler to signal command finished */
    byte _done = wait_done_ms( 1000 );
    /* read in command result bytes */
    status_size = 0;
    while ( (status_size < 7) && (inportb( FDC_MSR ) & (1 << 4)) )
        status[status_size++] = fd_getbyte( );
    if ( sensei ) {
        /* send a "sense interrupt status" command */
        fd_sendbyte( CMD_SENSEI );
        sr0 = fd_getbyte( );
        track = fd_getbyte( );
    }
    done = FALSE;
    if ( !_done ) {
        /* timed out! */
        if ( inportb( FDC_DIR ) & 0x80 ) /* check for diskchange */
            changed = TRUE;
        tty_puts( "Wait floppy irq error!\n" );
        return FALSE;
    }
    return TRUE;
}

/* this turns the motor on */
void fd_motor( byte on ) {
    if ( motor == on )
        return;
    if ( on ) {
        mofftimeleft = 0;
        outportb( FDC_DOR, 0x1c );
        motor = TRUE;
        wait_ms( 500 ); /* delay for motor to spin up */
    } else
        mofftimeleft = 36;
}

/* seek to track */
byte fd_seek( int trk ) {
    if ( track == trk ) /* already there? */
        return TRUE;
    fd_motor( TRUE );
    /* send actual command bytes */
    fd_sendbyte( CMD_SEEK );
    fd_sendbyte( 0 );
    fd_sendbyte( trk );
    /* wait until seek finished */
    if ( !fd_wait( TRUE ) )
        return FALSE; /* timeout! */
    /* now let head settle */
    wait_ms( 100 );
    fd_motor( FALSE );
    /* check that seek worked */
    if ( (sr0 != 0x20) || (track != trk) )
        return FALSE;
    else
        return TRUE;
}

/* recalibrate the drive */
void fd_recalibrate( void ) {
    /* turn the motor on */
    fd_motor( TRUE );

    /* send actual command bytes */
    fd_sendbyte( CMD_RECAL );
    fd_sendbyte( 0 );

    /* wait until seek finished */
    fd_wait( TRUE );

    /* turn the motor off */
    fd_motor( FALSE );
}

/* this gets the FDC to a known state */
void fd_reset( ) {
    /* stop the motor and disable IRQ/DMA */
    outportb( FDC_DOR, 0 );
    motor = FALSE;
    /* program data rate (500K/s) */
    outportb( FDC_DRS, 0 );
    /* re-enable interrupts */
    outportb( FDC_DOR, 0x0c );
    /* resetting triggered an interrupt - handle it */
    done = TRUE;
    fd_wait( TRUE );
    /* specify drive timings (got these off the BIOS) */
    fd_sendbyte( CMD_SPECIFY );
    fd_sendbyte( 0xdf ); /* SRT = 3ms, HUT = 240ms */
    fd_sendbyte( 0x02 ); /* HLT = 16ms, ND = 0 */
    /* clear "disk change" status */
    fd_seek( 1 );
    fd_recalibrate( );
    changed = FALSE;
}

/* init driver */
void fd_init( ) {
    fd_reset( );
    /* get floppy controller version */
    fd_sendbyte( CMD_VERSION );
    byte i = fd_getbyte( );

    if ( i == 0x80 )
        tty_puts( "NEC765 controller found\n" );
    else
        tty_puts( "enhanced controller found\n" );
//    tty_put_i32( i );
    fd_brw( 0, floppy_buffer, 1 );
}

/*
 * converts linear block address to head/track/sector
 *
 * blocks are numbered 0..heads*tracks*spt-1
 * blocks 0..spt-1 are serviced by head #0
 * blocks spt..spt*2-1 are serviced by head 1
 *
 * WARNING: garbage in == garbage out
 */
void fd_block2hts( int block, int *head, int *track, int *sector ) {
    *head = (block % (DG144_SPT * DG144_HEADS)) / (DG144_SPT);
    *track = block / (DG144_SPT * DG144_HEADS);
    *sector = block % DG144_SPT + 1;
}

/*
 * this sets up a DMA trasfer between a device and memory.  Pass the DMA
 * channel number (0..3), the physical address of the buffer and transfer
 * length.  If 'read' is TRUE, then transfer will be from memory to device,
 * else from the device to memory.
 */
void dma_xfer( int channel, long physaddr, int length, byte read ) {
    /* calculate dma page and offset */
    unsigned page = physaddr >> 16;
    unsigned offset = physaddr & 0xffff;
    length -= 1; /* with dma, if you want k bytes, you ask for k - 1 */
    i_disable( ); /* disable irq's */
    /* set the mask bit for the channel */
    outportb( 0x0a, channel | 4 );
    /* clear flipflop */
    outportb( 0x0c, 0 );
    /* set DMA mode (write+single+r/w) */
    outportb( 0x0b, (read ? 0x48 : 0x44) + channel );
    /* set DMA page */
    outportb( dmainfo[channel].page, page );
    /* set DMA offset */
    outportb( dmainfo[channel].offset, offset & 0xff ); /* low byte */
    outportb( dmainfo[channel].offset, offset >> 8 ); /* high byte */
    /* set DMA length */
    outportb( dmainfo[channel].length, length & 0xff ); /* low byte */
    outportb( dmainfo[channel].length, length >> 8 ); /* high byte */
    /* clear DMA mask bit */
    outportb( 0x0a, channel );
    i_enable( ); /* enable irq's */
}

/*
 * since reads and writes differ only by a few lines, this handles both.  This
 * function is called by read_block() and write_block()
 */
byte fd_brw( int block, byte *blockbuff, byte read ) {
    int head, track, sector;
    /* convert logical address into physical address */
    fd_block2hts( block, &head, &track, &sector );
    /* spin up the disk */
    fd_motor( TRUE );
    if ( !read && blockbuff )
        /* copy data from data buffer into track buffer */
        for ( int i = 0; i < 512; i++ )
            floppy_buffer[i] = blockbuff[i];
    for ( int tries = 0; tries < 3; tries++ ) {
        /* check for diskchange */
        if ( inportb( FDC_DIR ) & 0x80 ) {
            changed = TRUE;
            fd_seek( 1 ); /* clear "disk change" status */
            fd_recalibrate( );
            fd_motor( FALSE );
            return FALSE;
        }
        /* move head to right track */
        if ( !fd_seek( track ) ) {
            fd_motor( FALSE );
            tty_puts( "Floppy seek error!\n" );
            return FALSE;
        }
        /* program data rate (500K/s) */
        outportb( FDC_CCR, 0 );
        /* send command */
        dma_xfer( 2, (long) & floppy_buffer, 512, !read );
        fd_sendbyte( read ? CMD_READ : CMD_WRITE );
        fd_sendbyte( head << 2 );
        fd_sendbyte( track );
        fd_sendbyte( head );
        fd_sendbyte( sector );
        fd_sendbyte( 2 ); /* 512 bytes/sector */
        fd_sendbyte( DG144_SPT );
        fd_sendbyte( DG144_GAP3RW ); /* gap 3 size for 1.44M read/write */
        fd_sendbyte( 0xff ); /* DTL = unused */
        /* wait for command completion */
        /* read/write don't need "sense interrupt status" */
        if ( !fd_wait( FALSE ) )
            return FALSE; /* timed out! */
        if ( (status[0] & 0xc0) == 0 ) break; /* worked! outta here! */
        fd_recalibrate( ); /* oops, try again... */
    }
    /* stop the motor */
    fd_motor( FALSE );
    if ( read && blockbuff )
        /* copy data from track buffer into data buffer */
        for ( int i = 0; i < 512; i++ )
            blockbuff[i] = floppy_buffer[i];
    return 0;
}

void fd_timer( ) {
    if ( fd_timeleft > 0 )
        fd_timeleft--;
    if ( mofftimeleft > 0 ) {
        mofftimeleft--;
        if ( mofftimeleft == 0 ) {
            char v = inportb( FDC_DOR );
            v &= !0x10;
            outportb( FDC_DOR, v );
            motor = FALSE;
        }
    }
}

void fd_handler( ) {
    done = TRUE;
}
