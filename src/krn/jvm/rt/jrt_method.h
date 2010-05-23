/* 
 * File:   jrt_method.h
 * Author: igel
 *
 * Created on 22 Май 2010 г., 02:44
 */

#ifndef _JRT_METHOD_H
#define	_JRT_METHOD_H

#include "jrt.h"

typedef enum {
    MTH_INVIRT,
    MTH_INSPEC,
    MTH_INSTAT
} MTH_OP;

void jrtf_mth_op( JRFrame* frame, void* x ) {
    int m_idx = (frame->code[1] << 8) | frame->code[2];
    char *cls_name, *method_name, *method_type;
    jr_get_fm_info( frame->class, m_idx, &cls_name, &method_name, &method_type );
    bool isObjInit = (strcmp( cls_name, "java/lang/Object" ) == 0) && (strcmp( method_name, "<init>" ) == 0);
    if ( !isObjInit ) {
        JRType types[16];
        int itc = jm_fill_input_types_( method_type, &types[0] );
        JRObjRef _args[itc + 2];
        JClass cls = NULL;
        JRObjRef *o = NULL;
        switch ( (MTH_OP) x ) {
            case MTH_INVIRT:
                for ( int j = 0; j < itc; j++ ) {
                    _args[itc - j] = *jr_pop( frame->stack );
                    assert_op_t( frame, _args[itc - j].type, types[itc - j - 1] );
                }
                o = jr_pop( frame->stack );
                assert_op_t( frame, o->type, OBJECT );
                _args[0] = *o;
                _args[itc + 1] = EVOID;
                cls = jo_get_class( (JObject) o->ref );
                if ( (ui32) cls < 0xFFFF )
                    cls = jcf_load_class( cls_name );
                break;
            case MTH_INSPEC:
                for ( int j = 0; j < itc; j++ )
                    _args[j + 1] = *jr_pop( frame->stack );
                o = jr_pop( frame->stack );
                assert_op_t( frame, o->type, OBJECT );
                _args[0] = *o;
                _args[itc + 1] = EVOID;
                cls = jcf_load_class( cls_name );
                break;
            case MTH_INSTAT:
                for ( int j = 0; j < itc; j++ )
                    _args[j] = *jr_pop( frame->stack );
                _args[itc] = EVOID;
                cls = jcf_load_class( cls_name );
                break;
            default:
                throw ("unknown MTH_INVOCATION: %i", (int) x);
        }
        JMethod m = jc_find_method( cls, method_name, method_type, true );
        if ( m == NULL )
            throw ("%s: method not found", jrt_get_op( *frame->code ).name);
        JRObjRef r = jr_invoke( m, &_args[0] );
        if ( r.type != VOID )
            *jr_push( frame->stack ) = r;
    } else
        jr_pop( frame->stack );
    frame->code += 2;
}

#endif	/* _JRT_METHOD_H */

