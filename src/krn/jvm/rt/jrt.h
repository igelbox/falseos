/* 
 * File:   jrt.h
 * Author: igel
 *
 * Created on 21 Май 2010 г., 23:13
 */

#ifndef _JRT_H
#define	_JRT_H

#include "../jcore.h"
#include "../jutils.h"

typedef struct {
    int size, pos;
    JRObjRef *data;
} JRStack;

JRObjRef* jr_push( JRStack *stack ) {
    assert( stack->pos < stack->size, "stack overflow" );
    return &stack->data[stack->pos++];
}

JRObjRef* jr_pop( JRStack *stack ) {
    assert( stack->pos > 0, "stack underflow" );
    JRObjRef* r = &stack->data[--stack->pos];
    return r;
}

JRObjRef* jr_peek( JRStack *stack ) {
    assert( stack->pos > 0, "stack underflow" );
    JRObjRef* r = &stack->data[stack->pos - 1];
    return r;
}

typedef struct {
    int size;
    JRObjRef *data;
} JRObjPool;

JRObjRef* jropeek( JRObjPool *pool, int idx ) {
    assert( (idx >= 0) && (idx < pool->size), "pool index %i out o bounds", idx );
    return &pool->data[idx];
}

typedef struct {
    JClass class;
    JMethod method;
    JRStack* stack;
    JRObjPool* locals;
    JRObjPool* consts;
    ui08* code;
} JRFrame;

typedef void (*JRT_OP_F)(JRFrame*, void*);

#define F_RTO_RETURN 0x01

typedef struct {
    const char* name;
    ui08 flgs;
    JRT_OP_F func;
    void* sprm;
} JRTOp;

JRTOp ops[256];

void jrt_register_op_sprm_flags( ui08 code, const char* name, JRT_OP_F func, void* sprm, ui08 flgs ) {
    ops[code].name = name;
    ops[code].func = func;
    ops[code].sprm = sprm;
    ops[code].flgs = flgs;
}

void jrt_register_op_sprm( ui08 code, const char* name, JRT_OP_F func, void* sprm ) {
    jrt_register_op_sprm_flags( code, name, func, sprm, 0 );
}

void jrt_register_op( ui08 code, const char* name, JRT_OP_F func ) {
    jrt_register_op_sprm( code, name, func, NULL );
}

JRTOp jrt_get_op( ui08 code ) {
    return ops[code];
}

void jrt_init( ) {
    memset( &ops[0], sizeof (ops), 0 );
}

char* jr_get_type_name( JRType type ) {
    switch ( type ) {
        case VOID:
            return "VOID";
        case INT:
            return "INT";
        case LONG:
            return "LONG";
        case FLOAT:
            return "FLOAT";
        case DOUBLE:
            return "DOUBLE";
        case OBJECT:
            return "OBJECT";
        default:
            return "???";
    }
}

char* jr_get_atype_name( JRAType type ) {
    switch ( type ) {
        case ABYTE:
            return "BYTE";
        case ACHAR:
            return "CHAR";
        case AINT:
            return "INT";
        case ALONG:
            return "LONG";
        case AFLOAT:
            return "FLOAT";
        case ADOUBLE:
            return "DOUBLE";
        case AOBJECT:
            return "OBJECT";
        default:
            return "???";
    }
}

void assert_op_t( JRFrame* frame, JRType type, JRType tgt_type ) {
    if ( type != tgt_type )
        throw ("%s required %s found %s", jrt_get_op( *frame->code ).name, jr_get_type_name( tgt_type ), jr_get_type_name( type ));
}

void assert_aop_t( JRFrame* frame, JRAType type, JRAType tgt_type ) {
    if ( type != tgt_type )
        throw ("%s required %s found %s", jrt_get_op( *frame->code ).name, jr_get_atype_name( tgt_type ), jr_get_atype_name( type ));
}

#endif	/* _JRT_H */

