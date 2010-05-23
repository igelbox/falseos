/* 
 * File:   jrt_long.h
 * Author: igel
 *
 * Created on 22 Май 2010 г., 01:24
 */

#ifndef _JRT_LONG_H
#define	_JRT_LONG_H

#include "jrt.h"

typedef enum {
    L_OP_ADD,
    L_OP_SUB,
    L_OP_MUL,
    L_OP_DIV,
    L_OP_REM,
    L_OP_AND,
    L_OP_OR,
    L_OP_XOR,
    L_OP_CMP
} L_OP;

void jrtf_long_op( JRFrame* frame, void* x ) {
    JRObjRef *v0 = jr_pop( frame->stack );
    assert_op_t( frame, v0->type, LONG );
    JRObjRef *v1 = jr_pop( frame->stack );
    assert_op_t( frame, v1->type, LONG );
    JRObjRef *r = jr_push( frame->stack );
    r->type = LONG;
    switch ( (L_OP) x ) {
        case L_OP_ADD:
            r->value = (long long) v0->value + (long long) v1->value;
            break;
        case L_OP_SUB:
            r->value = (long long) v1->value - (long long) v0->value;
            break;
        case L_OP_MUL:
            r->value = (long long) v0->value * (long long) v1->value;
            break;
            //        case L_OP_DIV:
            //            r->value = (long long) v1->value / (long long) v0->value;
            //            break;
            //        case L_OP_REM:
            //            r->value = v1->value % v0->value;
            //            break;
        case L_OP_AND:
            r->value = v0->value & v1->value;
            break;
        case L_OP_OR:
            r->value = v0->value | v1->value;
            break;
        case L_OP_XOR:
            r->value = v0->value ^ v1->value;
            break;
        case L_OP_CMP:
            r->type = INT;
            r->value = v1->value == v0->value ? 0 : (v1->value > v0->value ? 1 : -1);
            break;
        default:
            throw ("unknown LONG operation: %i", (int) x);
    }
}

void jrtf_long_to( JRFrame* frame, void* x ) {
    JRObjRef *v = jr_peek( frame->stack );
    assert_op_t( frame, v->type, LONG );
    switch ( (JRAType) x ) {
        case AINT:
            v->type = INT;
            break;
        case ADOUBLE:
            v->type = DOUBLE;
            double* d = (double*) & v->value;
            *d = v->value;
            break;
        default:
            throw ("unknown L2 operation: %i", (int) x);
    }
}
#endif	/* _JRT_LONG_H */

