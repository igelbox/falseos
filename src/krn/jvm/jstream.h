/* 
 * File:   jstream.h
 * Author: igel
 *
 * Created on 24 Апрель 2010 г., 21:53
 */

#ifndef _JSTREAM_H
#define	_JSTREAM_H

#include "fstream.h"

ui08 js_readb( FStream f ) {
    ui08 tmp;
    fs_read( f, &tmp, 1 );
    return tmp;
}

ui16 js_readw( FStream f ) {
    ui16 tmp;
    fs_read( f, &tmp, 2 );
    return ((tmp & 0xFF) << 8) | ((tmp & 0xFF00) >> 8);
}

ui32 js_readd( FStream f ) {
    ui32 tmp;
    fs_read( f, &tmp, 4 );
    return ((tmp & 0xFF) << 24) | ((tmp & 0xFF00) << 8) | ((tmp & 0xFF0000) >> 8) | ((tmp & 0xFF000000) >> 24);
}

ui64 js_readq( FStream f ) {
    ui64 tmp;
    fs_read( f, &tmp, 8 );
    return ((tmp & 0xFF) << 56) | ((tmp & 0xFF00) << 40) | ((tmp & 0xFF0000) << 24) | ((tmp & 0xFF000000) << 8) | ((tmp & 0xFF00000000) >> 8) | ((tmp & 0xFF0000000000) >> 24) | ((tmp & 0xFF000000000000) >> 40) | ((tmp & 0xFF00000000000000) >> 56);
}

#endif	/* _JSTREAM_H */

