/* 
 * File:   jutils.h
 * Author: igel
 *
 * Created on 25 Апрель 2010 г., 1:36
 */

#ifndef _JUTILS_H
#define	_JUTILS_H

#include "jcore.h"

void memcpy( void* dst, void* src, ui32 size ) {
    ui08 *d = (ui08*) dst, *s = (ui08*) src;
    for ( ui32 i = 0; i < size; i++ )
        d[i] = s[i];
}

void memset( void* dst, ui32 size, ui08 value ) {
    ui08 *d = (ui08*) dst;
    for ( ui32 i = 0; i < size; i++ )
        d[i] = value;
}

int strcmp( const char* s1, const char* s2 ) {
    int i = 0;
    while ( s1[i] ) {
        if ( s1[i] != s2[i] )
            return 1;
        i++;
    }
    return s2[i] != 0;
}

int strlen( const char* s ) {
    int r = 0;
    while ( s[r] )
        r++;
    return r;
}

char* strcpy( char* dst, const char* src ) {
    int i = 0;
    do {
        dst[i] = src[i];
        i++;
    } while ( src[i] );
    return dst;
}

#endif	/* _JUTILS_H */

