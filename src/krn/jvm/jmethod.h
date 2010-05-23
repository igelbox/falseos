/* 
 * File:   jmethod.h
 * Author: igel
 *
 * Created on 25 Апрель 2010 г., 1:22
 */

#ifndef _JMETHOD_H
#define	_JMETHOD_H

#include "jcore.h"
#include "jmemory.h"
#include "jstream.h"
#include "jclass.h"
#include "jutils.h"

JClass jm_get_class( JMethod m ) {
    return (JClass) m->class;
}

char* jm_get_name( JMethod m ) {
    return ((JStringUtf*) jm_get_class( m )->consts[m->name_idx - 1].ref)->data;
}

char* jm_get_type( JMethod m ) {
    return ((JStringUtf*) jm_get_class( m )->consts[m->type_idx - 1].ref)->data;
}

int jm_fill_input_types_( char* type, JRType *types ) {
    int i = 1, l = strlen( type ), cnt = 0;
    while ( (i < l) && (type[i] != ')') ) {
        switch ( type[i] ) {
            case 'I'://int
            case 'C'://char
            case 'S'://short
            case 'B'://byte
            case 'Z'://boolean
                types[cnt++] = INT;
                break;
            case 'J':
                types[cnt++] = LONG;
                break;
            case 'F':
                types[cnt++] = FLOAT;
                break;
            case 'D':
                types[cnt++] = DOUBLE;
                break;
            case 'L':
                types[cnt++] = OBJECT;
                while ( (i < l) && (type[i] != ';') )
                    i++;
                break;
            case '[':
                types[cnt++] = OBJECT;
                while ( type[i] == '[' )
                    i++;
                if ( type[i] == 'L' )
                    while ( (i < l) && (type[i] != ';') )
                        i++;
                break;
            default:
                throw ("jm_fill_input_types:%s:%i", type, i);
        }
        i++;
    }
    return cnt;
}

int jm_fill_input_types( JMethod m, JRType *types ) {
    return jm_fill_input_types_( jm_get_type( m ), types );
}

JRType jm_get_result_type( JMethod m ) {
    char *type = jm_get_type( m );
    int i = 1, l = strlen( type );
    while ( (i < l) && (type[i] != ')') )
        i++;
    switch ( type[i + 1] ) {
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
            throw ("jm_get_result_type:%s:%i", type, i + 1);
    }
}

/*JMethodAttribute jma_load( FStream f, ui16 name_idx ) {
    JMethodAttribute r = jmm_alloc( sizeof (_JMethodAttribute) );
    r->name_idx = name_idx;
    r->length = js_readd( f );
    r->data = jmm_alloc( r->length + 1 );
    fs_read( f, r->data, r->length );
    r->data[r->length] = 0;
    return r;
}*/

JMethodCodeAttribute jma_load_code( FStream f ) {
    JMethodCodeAttribute r = jmm_alloc( sizeof (_JMethodCodeAttribute) );
    r->length = js_readd( f );
    r->max_stack = js_readw( f );
    r->max_locals = js_readw( f );
    r->code_length = js_readd( f );
    r->code = jmm_alloc( r->code_length + 1 );
    fs_read( f, r->code, r->code_length );
    r->code[r->code_length] = 0;
    fs_skip( f, r->length - 8 - r->code_length );
    return r;
}

JMethod jm_load( FStream f, JClass c ) {
    JMethod r = jmm_alloc( sizeof (_JMethod) );
    r->class = c;
    r->attributes_flags = js_readw( f );
    r->name_idx = js_readw( f );
    r->type_idx = js_readw( f );
    ui16 ac /*r->attributes_count*/ = js_readw( f );
    //    r->attributes = jmm_alloc( sizeof (void*) * r->attributes_count );
    for ( int i = 0; i < ac/*r->attributes_count*/; i++ ) {
        ui16 name_idx = js_readw( f );
        //	tty_put_ui32_hex( name_idx );
        JRObjRef *n = &c->consts[name_idx - 1];
        assert( n->type == OBJECT, NULL );
        JStringUtf *nn = (JStringUtf*) n->ref;
        //	tty_puts( (char*)nn->data );
        //	while(true);
        if ( strcmp( (char*) nn->data, "Code" ) == 0 )
            /*r->attributes[i] = */r->code_attribute = jma_load_code( f );
        else {
            ui32 l = js_readd( f );
            fs_skip( f, l );
            //            r->attributes[i] = jma_load( f, name_idx );
        }
    }
    return r;
}

#endif	/* _JMETHOD_H */

