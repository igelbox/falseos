/* 
 * File:   jarray.h
 * Author: igel
 *
 * Created on 14 Май 2010 г., 22:00
 */

#ifndef _JARRAY_H
#define	_JARRAY_H

#include "jcore.h"
#include "jmemory.h"
#include "jutils.h"

ui32 ja_type_size( JRAType type ) {
    switch( type ) {
        case ABYTE:
            return 1;
        case ACHAR:
        case ASHORT:
            return 2;
        case AINT:
        case AFLOAT:
        case AOBJECT:
            return 4;
        case ALONG:
        case ADOUBLE:
            return 8;
        default:
            assert( false, "ja_type_size!" );
    }
}

JArray ja_new( JRAType type, ui32 size ) {
    JArray r = jmm_alloc( sizeof (_JArray) );
    r->class = NULL;
    r->type = type;
    r->size = size;
    r->data = jmm_alloc( ja_type_size(type) * size );
    return r;
}

#endif	/* _JARRAY_H */

