#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../krn/types.h"

void init_frec( char* data, long id, long start, long count ) {
    *((long*) (&data[id * 32 + 0])) = sw64( id );
    *((long*) (&data[id * 32 + 8])) = sw64( start );
    *((long*) (&data[id * 32 + 16])) = sw64( count );
}

long file_size( const char *fname ) {
    long result = 0;
    FILE* f = fopen( fname, "rb" );
    char buff[1024];
    for (;; ) {
        size_t cnt = fread( buff, 1, 1024, f );
        if ( cnt > 0 )
            result += cnt;
        else
            break;
    };
    fclose( f );
    return result;
}

int main( int argc, char** argv ) {
    if ( argc != 5 ) {
        printf( "usage: imgmake <bootsector.bin> <kernel.bin> <initramfs> <floppy.img>\n" );
        exit( EXIT_FAILURE );
    }
    char data[512];

    FILE* boot = fopen( argv[1], "rb" );
    fread( data, 512, 1, boot );
    fclose( boot );

    FILE* img = fopen( argv[4], "wb" );
    fwrite( data, 512, 1, img ); //boot-sector
    memset( data, 0, 512 );
    fwrite( data, 512, 1, img ); //first MFT extent sector
    init_frec( data, 0, 1, 2 );
    long krn_sz = file_size( argv[2] );
    int krn_scnt = krn_sz / 512;
    if ( krn_sz % 512 )
        krn_scnt++;
    printf( "kernel size:%ld byte(s) or %d sector(s)\n", krn_sz, krn_scnt );
    long irfs_sz = file_size( argv[3] );
    int irfs_scnt = irfs_sz / 512;
    if ( irfs_sz % 512 )
        irfs_scnt++;
    printf( "initramfs size:%ld byte(s) or %d sector(s)\n", irfs_sz, irfs_scnt );
    init_frec( data, 1, 3, krn_scnt + 1 );
    init_frec( data, 2, 3 + krn_scnt + 1, irfs_scnt + 1 );
    fwrite( data, 512, 1, img ); //MFT

    memset( data, 0, 512 );
    fwrite( data, 512, 1, img ); //first KERNEL extent sector
    char krn_data[krn_sz];
    FILE* krn = fopen( argv[2], "rb" );
    fread( krn_data, krn_sz, 1, krn );
    fclose( krn );
    fwrite( krn_data, krn_sz, 1, img );
    fwrite( data, krn_scnt * 512 - krn_sz, 1, img );

    memset( data, 0, 512 );
    fwrite( data, 512, 1, img ); //first INITRAMFS extent sector
    char irfs_data[irfs_sz];
    FILE* irfs = fopen( argv[3], "rb" );
    fread( irfs_data, irfs_sz, 1, irfs );
    fclose( irfs );
    fwrite( irfs_data, irfs_sz, 1, img );
    fwrite( data, irfs_scnt * 512 - irfs_sz, 1, img );
    for ( int i = 6 + krn_scnt; i < 1440 * 2; i++ )
        fwrite( data, 512, 1, img );
    fclose( img );
    printf( "img builded\n" );
    return EXIT_SUCCESS;
}
