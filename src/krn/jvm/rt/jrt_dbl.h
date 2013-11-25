/* 
 * File:   jrt_dbl.h
 * Author: igel
 *
 * Created on 22 Май 2010 г., 02:37
 */

#ifndef _JRT_DBL_H
#define	_JRT_DBL_H

#include "jrt.h"

typedef enum {
    D_OP_ADD,
    D_OP_SUB,
    D_OP_MUL,
    D_OP_DIV
} D_OP;

void jrtf_dbl_op( JRFrame* frame, void* x ) {
    JRObjRef *v0 = jr_pop( frame->stack );
    assert_op_t( frame, v0->type, DOUBLE );
    JRObjRef *v1 = jr_pop( frame->stack );
    assert_op_t( frame, v1->type, DOUBLE );
    JRObjRef *r = jr_push( frame->stack );
    r->type = DOUBLE;
    double* d0 = (double*) (&v0->value);
    double* d1 = (double*) (&v1->value);
    double* d = (double*) (&r->value);
    switch ( (I_OP) x ) {
        case D_OP_ADD:
            *d = *d1 + *d0;
            break;
        case D_OP_SUB:
            *d = *d1 - *d0;
            break;
        case D_OP_MUL:
            *d = *d1 * *d0;
            break;
        case D_OP_DIV:
            *d = *d1 / *d0;
            break;
        default:
            throw ("unknown DOUBLE operation: %i", (int) x);
    }
}

void jrtf_dbl_to( JRFrame* frame, void* x ) {
    JRObjRef *v = jr_peek( frame->stack );
    assert_op_t( frame, v->type, DOUBLE );
    typedef union { ui64 u; double d; } U64D;
    switch ( (JRAType) x ) {
        case ABYTE:
            v->type = INT;
            v->value = (char) ((U64D*) (&v->value))->d;
            break;
        case ACHAR:
            v->type = INT;
            v->value = (short) ((U64D*) (&v->value))->d;
            break;
        case AINT:
            v->type = INT;
            v->value = (int) ((U64D*) (&v->value))->d;
            break;
        case ALONG:
            v->type = LONG;
            v->value = (long long) ((U64D*) (&v->value))->d;
            break;
        default:
            throw ("unknown D2 operation: %i", (int) x);
    }
}
#endif	/* _JRT_DBL_H */

