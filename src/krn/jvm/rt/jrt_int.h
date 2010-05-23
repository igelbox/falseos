/* 
 * File:   jrt_int.h
 * Author: igel
 *
 * Created on 22 Май 2010 г., 01:09
 */

#ifndef _JRT_INT_H
#define	_JRT_INT_H

#include "jrt.h"

typedef enum {
    I_OP_ADD,
    I_OP_SUB,
    I_OP_MUL,
    I_OP_DIV,
    I_OP_REM,
    I_OP_AND,
    I_OP_OR,
    I_OP_XOR
} I_OP;

void jrtf_int_op( JRFrame* frame, void* x ) {
    JRObjRef *v0 = jr_pop( frame->stack );
    assert_op_t( frame, v0->type, INT );
    JRObjRef *v1 = jr_pop( frame->stack );
    assert_op_t( frame, v1->type, INT );
    JRObjRef *r = jr_push( frame->stack );
    r->type = INT;
    switch ( (I_OP) x ) {
        case I_OP_ADD:
            r->value = (int) v0->value + (int) v1->value;
            break;
        case I_OP_SUB:
            r->value = (int) v1->value - (int) v0->value;
            break;
        case I_OP_MUL:
            r->value = (int) v0->value * (int) v1->value;
            break;
        case I_OP_DIV:
            r->value = (int) v1->value / (int) v0->value;
            break;
        case I_OP_REM:
            r->value = (int) v1->value % (int) v0->value;
            break;
        case I_OP_AND:
            r->value = (int) v0->value & (int) v1->value;
            break;
        case I_OP_OR:
            r->value = (int) v0->value | (int) v1->value;
            break;
        case I_OP_XOR:
            r->value = (int) v0->value ^ (int) v1->value;
            break;
        default:
            throw ("unknown INT operation: %i", (int) x);
    }
}

void jrtf_int_inc( JRFrame* frame, void* x ) {
    JRObjRef *e = jropeek( frame->locals, frame->code[1] );
    assert_op_t( frame, e->type, INT );
    int d = (char) frame->code[2];
    e->value += d;
    frame->code += 2;
}

void jrtf_int_to( JRFrame* frame, void* x ) {
    JRObjRef *v = jr_peek( frame->stack );
    assert_op_t( frame, v->type, INT );
    //    switch ( (JRAType) x ) {
    //        case ABYTE:
    //            v->value &= 0xFF;
    //            break;
    //    }
}
#endif	/* _JRT_INT_H */

