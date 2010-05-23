/* 
 * File:   fstream.h
 * Author: igel
 *
 * Created on 24 Апрель 2010 г., 21:56
 */

#ifndef _FSTREAM_H
#define	_FSTREAM_H

#include "jcore.h"
#include "jmemory.h"
#include "jutils.h"
#include "../types.h"

typedef struct {
    char *buffer;
    int pos;
} _FStream;
typedef _FStream* FStream;

extern char fd_brw( int block, char* blockbuff, char read );

FStream fs_open( const char* name ) {
    char* p = (char*)0x40000;
    while ( *p ) {
	char* c = &p[1 + *p + 1];
	if ( strcmp(&p[1], name) == 0 ) {
	    FStream r = jmm_alloc( sizeof (_FStream) );
	    r->buffer = &c[2];
	    r->pos = 0;
	    return r;
	}
	p = c + sw16(*(short*)c) + 2;
    }
    return 0;
}

bool fs_exists( FStream f ) {
    return f != NULL;
}

void fs_close( FStream f ) {
}

ui32 fs_read( FStream f, void* data, ui32 len ) {
    memcpy( data, &f->buffer[f->pos], len );
    f->pos += len;
    return len;
}

void fs_skip( FStream f, ui32 len ) {
    f->pos += len;
}

#endif	/* _FSTREAM_H */

