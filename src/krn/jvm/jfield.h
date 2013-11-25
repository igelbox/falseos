/* 
 * File:   jfield.h
 * Author: igel
 *
 * Created on 01 Май 2010 г., 21:33
 */

#ifndef _JFIELD_H
#define	_JFIELD_H

#include "jcore.h"
#include "jmemory.h"
#include "jstream.h"
#include "jclass.h"
#include "jutils.h"

JClass jf_get_class( JField f ) {
    return (JClass) f->class;
}

char* jf_get_name( JField f ) {
    return (char*)((JStringUtf*) jf_get_class( f )->consts[f->name_idx - 1].ref)->data;
}

char* jf_get_type( JField f ) {
    return (char*)((JStringUtf*) jf_get_class( f )->consts[f->type_idx - 1].ref)->data;
}

JRType jf_get_rtype( JField f ) {
    char *type = jf_get_type( f );
    switch ( type[0] ) {
        case 'I':
        case 'C':
        case 'S':
        case 'B':
        case 'Z':
            return INT;
        case 'J':
            return LONG;
        case 'F':
            return FLOAT;
        case 'D':
            return DOUBLE;
        case 'L':
        case '[':
            return OBJECT;
        case 'V':
            return VOID;
        default:
            tty_puts( type );
            throw( "jf_get_rtype" );
            return VOID;//suppress warning
    }
}

JField jf_load( FStream f, JClass c ) {
    JField r = jmm_alloc( sizeof (_JField) );
    r->class = c;
    r->attributes_flags = js_readw( f );
    r->name_idx = js_readw( f );
    r->type_idx = js_readw( f );
    ui16 ac /*r->attributes_count*/ = js_readw( f );
//    tty_put_ui32_hex(r->attributes_count);
//    r->attributes = jmm_alloc( sizeof (void*) * r->attributes_count );
    for ( int i = 0; i < ac/*r->attributes_count*/; i++ ) {
            ui16 name_idx = js_readw( f );
            JRObjRef *n = &c->consts[name_idx - 1];
            assert( n->type == OBJECT, NULL );
//            JStringUtf *nn = (JStringUtf*) n->ref;
            ui32 l = js_readd( f );
            fs_skip( f, l );
/*            if ( strcmp( (char*) nn->data, "Code" ) == 0 )
                r->attributes[i] = r->code_attribute = jma_load_code( f );
            else
                r->attributes[i] = jma_load( f, name_idx );*/
    }
    return r;
}

#endif	/* _JFIELD_H */

