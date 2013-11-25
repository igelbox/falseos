/* 
 * File:   jrt_arrays.h
 * Author: igel
 *
 * Created on 22 Май 2010 г., 01:01
 */

#ifndef _JRT_ARRAYS_H
#define	_JRT_ARRAYS_H

#include "jrt.h"

void jrtf_arr_load( JRFrame* frame, void* x ) {
    JRAType t = (JRAType) x;
    JRObjRef *idx = jr_pop( frame->stack );
    assert_op_t( frame, idx->type, INT );
    JRObjRef *arr = jr_pop( frame->stack );
    assert_op_t( frame, arr->type, OBJECT );
    JArray a = (JArray) arr->ref;
    const char* op_name = jrt_get_op( *frame->code ).name;
    assert( a != NULL, "%s[NP]", op_name );
    assert( a->class == NULL, "%s[AA]", op_name );
    assert_op_t( frame, a->type, t );
    assert( idx->value < a->size, "%s[IDX OOB]", op_name );
    JRObjRef * e = jr_push( frame->stack );
    e->type = INT;
    switch ( t ) {
        case ABYTE:
            e->value = ((char*) a->data)[idx->value];
            break;
        case ACHAR:
            e->value = ((short*) a->data)[idx->value];
            break;
        case AINT:
            e->value = ((int*) a->data)[idx->value];
            break;
        default:
            throw("jrtf_arr_load.switch");
    }
}

void jrtf_arr_store( JRFrame* frame, void* x ) {
    JRAType t = (JRAType) x;
    JRObjRef *val = jr_pop( frame->stack );
    assert_op_t( frame, val->type, INT );
    JRObjRef *idx = jr_pop( frame->stack );
    assert_op_t( frame, idx->type, INT );
    JRObjRef *arr = jr_pop( frame->stack );
    assert_op_t( frame, arr->type, OBJECT );
    JArray a = (JArray) arr->ref;
    const char* op_name = jrt_get_op( *frame->code ).name;
    assert( a != NULL, "%s[NP]", op_name );
    assert( a->class == NULL, "%s[AA]", op_name );
    assert_op_t( frame, a->type, t );
    assert( idx->value < a->size, "%s[IDX OOB]", op_name );
    switch ( t ) {
        case ABYTE:
            ((char*) a->data)[idx->value] = val->value;
            break;
        case ACHAR:
            ((short*) a->data)[idx->value] = val->value;
            break;
        case AINT:
            ((int*) a->data)[idx->value] = val->value;
            break;
        default:
            throw("jrtf_arr_store.switch");
    }
}

void jrtf_arr_new( JRFrame* frame, void* x ) {
    JRObjRef *o = jr_pop( frame->stack );
    assert_op_t( frame, o->type, INT );
    JRAType typ;
    switch ( frame->code[1] ) {
        case 0x05:
            typ = ACHAR;
            break;
        case 0x08:
            typ = ABYTE;
            break;
        case 0x0A:
            typ = AINT;
            break;
        default:
            throw ("unknown JRAType: %i", frame->code[1]);
    }
    JRObjRef *e = jr_push( frame->stack );
    e->type = OBJECT;
    e->ref = ja_new( typ, o->value );
    frame->code++;
}

void jrtf_arr_len( JRFrame* frame, void* x ) {
    JRObjRef *arr = jr_pop( frame->stack );
    assert_op_t( frame, arr->type, OBJECT );
    JArray a = (JArray) arr->ref;
    if ( a->class != NULL )
        throw ("%s[AA]", jrt_get_op( *frame->code ).name);
    JRObjRef * e = jr_push( frame->stack );
    e->type = INT;
    e->value = a->size;
}

#endif	/* _JRT_ARRAYS_H */

