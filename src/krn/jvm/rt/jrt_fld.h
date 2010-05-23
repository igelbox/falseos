/* 
 * File:   jrt_fld.h
 * Author: igel
 *
 * Created on 22 Май 2010 г., 02:00
 */

#ifndef _JRT_FLD_H
#define	_JRT_FLD_H

#include "jrt.h"

typedef enum {
    FLD_GS,
    FLD_PS,
    FLD_GF,
    FLD_PF
} FLD_OP;

void jrtf_fld_op( JRFrame* frame, void* x ) {
    FLD_OP op = (FLD_OP) x;
    ui16 f_idx = (frame->code[1] << 8) | frame->code[2];
    char *cls_name, *field_name, *field_type;
    jr_get_fm_info( frame->class, f_idx, &cls_name, &field_name, &field_type );
    JRObjRef *e;
    if ( (op == FLD_PS) || (op == FLD_PF) )
        e = jr_pop( frame->stack );
    JObjectField f;
    if ( (op == FLD_GF) || (op == FLD_PF) ) {
        JRObjRef *o = jr_pop( frame->stack );
        assert_op_t( frame, o->type, OBJECT );
        f = jo_find_field( (JObject) o->ref, field_name, field_type );
    } else {
        JClass cls = jcf_load_class( cls_name );
        f = jc_find_field( cls, field_name, field_type, true );
    }
    assert( f != NULL, "%s: field not found", jrt_get_op( *frame->code ).name );
    if ( (op == FLD_GS) || (op == FLD_GF) )
        *jr_push( frame->stack ) = f->val;
    else
        f->val = *e;
    frame->code += 2;
}

#endif	/* _JRT_FLD_H */

