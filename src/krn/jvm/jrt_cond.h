/* 
 * File:   jrt_cond.h
 * Author: igel
 *
 * Created on 22 Май 2010 г., 01:34
 */

#ifndef _JRT_COND_H
#define	_JRT_COND_H

#include "jrt.h"

typedef enum {
    COND_EQ, //==
    COND_NE, //!=
    COND_LE, //<=
    COND_LT, //<
    COND_GE, //>=
    COND_GT//>
} COND_OP;

void jrtf_cond_op( JRFrame* frame, void* x ) {
    JRObjRef *e = jr_pop( frame->stack );
    assert_op_t( frame, e->type, INT );
    int d_addr = (frame->code[1] << 8) | frame->code[2];
    bool ok;
    switch ( (COND_OP) x ) {
        case COND_EQ:
            ok = e->value == 0;
            break;
        case COND_NE:
            ok = e->value != 0;
            break;
        case COND_LE:
            ok = e->value <= 0;
            break;
        case COND_LT:
            ok = e->value < 0;
            break;
        case COND_GE:
            ok = e->value >= 0;
            break;
        case COND_GT:
            ok = e->value > 0;
            break;
        default:
            throw ("unknown COND operation: %i", (int) x);
    }
    if ( ok )
        frame->code += d_addr - 1;
    else
        frame->code += 2;
}

void jrtf_cond_cmp( JRFrame* frame, void* x ) {
    JRObjRef *v0 = jr_pop( frame->stack );
    assert_op_t( frame, v0->type, INT );
    JRObjRef *v1 = jr_pop( frame->stack );
    assert_op_t( frame, v1->type, INT );
    int d_addr = (frame->code[1] << 8) | frame->code[2];
    bool ok;
    switch ( (COND_OP) x ) {
        case COND_EQ:
            ok = v1->value == v0->value;
            break;
        case COND_NE:
            ok = v1->value != v0->value;
            break;
        case COND_LE:
            ok = (int) v1->value <= (int) v0->value;
            break;
        case COND_LT:
            ok = (int) v1->value < (int) v0->value;
            break;
        case COND_GE:
            ok = (int) v1->value >= (int) v0->value;
            break;
        case COND_GT:
            ok = (int) v1->value > (int) v0->value;
            break;
        default:
            throw ("unknown COND operation: %i", (int) x);
    }
    if ( ok )
        frame->code += d_addr - 1;
    else
        frame->code += 2;
}

#endif	/* _JRT_COND_H */

