/* 
 * File:   jrt_base.h
 * Author: igel
 *
 * Created on 21 Май 2010 г., 23:26
 */

#ifndef _JRT_BASE_H
#define	_JRT_BASE_H

#include "jrt.h"
#include "jrt_arrays.h"
#include "jrt_int.h"
#include "jrt_long.h"
#include "jrt_cond.h"

typedef struct {
    JRType type;
    ui08 offs;
} JRTBTypeOffs;

void jrtf_base_aload_null( JRFrame* frame, void* x ) {
    JRObjRef *c = jr_push( frame->stack );
    c->type = OBJECT;
    c->ref = NULL;
}

void jrtf_base_pop( JRFrame* frame, void* x ) {
    jr_pop( frame->stack );
}

void jrtf_base_dup( JRFrame* frame, void* x ) {
    JRObjRef *e = jr_peek( frame->stack );
    *jr_push( frame->stack ) = *e;
}

void jrtf_base_goto( JRFrame* frame, void* x ) {
    int d_addr = (frame->code[1] << 8) | frame->code[2];
    frame->code += d_addr - 1;
}

void jrtf_base_const( JRFrame* frame, void* x ) {
    JRTBTypeOffs* to = (JRTBTypeOffs*) x;
    JRObjRef *c = jr_push( frame->stack );
    c->type = to->type;
    c->value = *frame->code - to->offs;
}

void jrtf_base_dconst( JRFrame* frame, void* x ) {
    JRObjRef *c = jr_push( frame->stack );
    c->type = DOUBLE;
    double *d = (double*) & c->value;
    *d = *frame->code - 0x0E;
}

void jrtf_base_load( JRFrame* frame, void* x ) {
    JRTBTypeOffs* to = (JRTBTypeOffs*) x;
    JRObjRef *e = jropeek( frame->locals, *frame->code - to->offs );
    assert_op_t( frame, e->type, to->type );
    *jr_push( frame->stack ) = *e;
}

void jrtf_base_xload( JRFrame* frame, void* x ) {
    JRType t = (JRType) x;
    JRObjRef *e = jropeek( frame->locals, frame->code[1] );
    assert_op_t( frame, e->type, t );
    *jr_push( frame->stack ) = *e;
    frame->code++;
}

void jrtf_base_store( JRFrame* frame, void* x ) {
    JRTBTypeOffs* to = (JRTBTypeOffs*) x;
    JRObjRef *e = jr_pop( frame->stack );
    assert_op_t( frame, e->type, to->type );
    *jropeek( frame->locals, *frame->code - to->offs ) = *e;
}

void jrtf_base_xstore( JRFrame* frame, void* x ) {
    JRType t = (JRType) x;
    JRObjRef *e = jr_pop( frame->stack );
    assert_op_t( frame, e->type, t );
    frame->code++;
    *jropeek( frame->locals, *frame->code ) = *e;
}

void jrtf_base_xpush( JRFrame* frame, void* x ) {
    int d = (int) x;
    JRObjRef *e = jr_push( frame->stack );
    e->type = INT;
    e->value = 0;
    while ( d > 0 ) {
        frame->code++;
        e->value = (e->value << 8) | *frame->code;
        d--;
    }
}

void jrtf_base_ldc( JRFrame* frame, void* x ) {
    frame->code++;
    int c_idx = *frame->code;
    JRObjRef *e = jr_push( frame->stack );
    *e = *jropeek( frame->consts, c_idx - 1 );
}

void jrtf_base_ldc2w( JRFrame* frame, void* x ) {
    ui08 op = *frame->code;
    frame->code++;
    int c_idx = *frame->code;
    frame->code++;
    c_idx = (c_idx << 8) | *frame->code;
    JRObjRef *e = jr_push( frame->stack );
    *e = *jropeek( frame->consts, c_idx - 1 );
    switch ( e->type ) {
        case DOUBLE:
        {
            double *d = (double*) & e->value;
            break;
        }
        case LONG:
            break;
        default:
            throw (false, "%s for not DOUBLE or LONG type", jrt_get_op( op ));
    }
}

JRTBTypeOffs RTBTO_C_I = { INT, 0x03 }, RTBTO_C_L = { LONG, 0x09 };
JRTBTypeOffs RTBTO_L_I = { INT, 0x1A }, RTBTO_L_L = { LONG, 0x1E }, RTBTO_L_D = { DOUBLE, 0x26 }, RTBTO_L_A = { OBJECT, 0x2A };
JRTBTypeOffs RTBTO_S_I = { INT, 0x3B }, RTBTO_S_L = { LONG, 0x3F }, RTBTO_S_D = { DOUBLE, 0x47 }, RTBTO_S_A = { OBJECT, 0x4B };

void jrt_base_init( ) {
    jrt_register_op( 0x01, "ALOAD_NULL", jrtf_base_aload_null, NULL );
    for ( int i = 0x02; i < 0x09; i++ )
        jrt_register_op( i, "ICONST", jrtf_base_const, &RTBTO_C_I );
    for ( int i = 0x09; i < 0x0C; i++ )
        jrt_register_op( i, "LCONST", jrtf_base_const, &RTBTO_C_L );
    for ( int i = 0x0C; i < 0x10; i++ )
        jrt_register_op( i, "DCONST", jrtf_base_dconst, NULL );
    jrt_register_op( 0x10, "BIPUSH", jrtf_base_xpush, (void*) 1 );
    jrt_register_op( 0x11, "SIPUSH", jrtf_base_xpush, (void*) 2 );
    jrt_register_op( 0x12, "LDC", jrtf_base_ldc, NULL );

    jrt_register_op( 0x14, "LDC2W", jrtf_base_ldc2w, NULL );
    jrt_register_op( 0x15, "ILOAD", jrtf_base_xload, (void*) INT );
    jrt_register_op( 0x16, "LLOAD", jrtf_base_xload, (void*) LONG );

    for ( int i = 0x1A; i < 0x1E; i++ )
        jrt_register_op( i, "ILOAD", jrtf_base_load, &RTBTO_L_I );
    for ( int i = 0x1E; i < 0x23; i++ )
        jrt_register_op( i, "LLOAD", jrtf_base_load, &RTBTO_L_L );

    for ( int i = 0x26; i < 0x29; i++ )
        jrt_register_op( i, "DLOAD", jrtf_base_load, &RTBTO_L_D );

    for ( int i = 0x2A; i < 0x2E; i++ )
        jrt_register_op( i, "ALOAD", jrtf_base_load, &RTBTO_L_A );
    jrt_register_op( 0x2E, "IALOAD", jrtf_arr_xload, (void*) AINT );

    jrt_register_op( 0x33, "BALOAD", jrtf_arr_xload, (void*) ABYTE );
    jrt_register_op( 0x34, "CALOAD", jrtf_arr_xload, (void*) ACHAR );

    jrt_register_op( 0x36, "ISTORE", jrtf_base_xstore, (void*) INT );
    jrt_register_op( 0x37, "LSTORE", jrtf_base_xstore, (void*) LONG );

    for ( int i = 0x3B; i < 0x3F; i++ )
        jrt_register_op( i, "ISTORE", jrtf_base_store, &RTBTO_S_I );
    for ( int i = 0x3F; i < 0x44; i++ )
        jrt_register_op( i, "LSTORE", jrtf_base_store, &RTBTO_S_L );

    for ( int i = 0x48; i < 0x4C; i++ )
        jrt_register_op( i, "DSTORE", jrtf_base_store, &RTBTO_S_D );
    for ( int i = 0x4C; i < 0x4F; i++ )
        jrt_register_op( i, "ASTORE", jrtf_base_store, &RTBTO_S_A );
    jrt_register_op( 0x4F, "IASTORE", jrtf_arr_xstore, (void*) AINT );

    jrt_register_op( 0x54, "BASTORE", jrtf_arr_xstore, (void*) ABYTE );
    jrt_register_op( 0x55, "CASTORE", jrtf_arr_xstore, (void*) ACHAR );

    jrt_register_op( 0x57, "POP", jrtf_base_pop, NULL );

    jrt_register_op( 0x59, "DUP", jrtf_base_dup, NULL );
    jrt_register_op( 0x5C, "DUP", jrtf_base_dup, NULL );

    jrt_register_op( 0x60, "IADD", jrtf_int_op, (void*) I_OP_ADD );

    jrt_register_op( 0x64, "ISUB", jrtf_int_op, (void*) I_OP_SUB );
    jrt_register_op( 0x65, "LSUB", jrtf_long_op, (void*) L_OP_SUB );

    jrt_register_op( 0x68, "IMUL", jrtf_int_op, (void*) I_OP_MUL );
    jrt_register_op( 0x69, "LMUL", jrtf_long_op, (void*) L_OP_MUL );

    jrt_register_op( 0x6C, "IDIV", jrtf_int_op, (void*) I_OP_DIV );

    jrt_register_op( 0x70, "IREM", jrtf_int_op, (void*) I_OP_REM );

    jrt_register_op( 0x7E, "IAND", jrtf_int_op, (void*) I_OP_AND );

    jrt_register_op( 0x80, "IOR", jrtf_int_op, (void*) I_OP_OR );

    jrt_register_op( 0x88, "L2I", jrtf_long_to, (void*) AINT );

    jrt_register_op( 0x8A, "L2D", jrtf_long_to, (void*) ADOUBLE );

    jrt_register_op( 0x91, "I2B", jrtf_int_to, (void*) ABYTE );
    jrt_register_op( 0x92, "I2C", jrtf_int_to, (void*) ACHAR );

    jrt_register_op( 0x94, "LCMP", jrtf_long_op, (void*) L_OP_CMP );

    jrt_register_op( 0x99, "IFEQ", jrtf_cond_op, (void*) COND_EQ );
    jrt_register_op( 0x9A, "IFNE", jrtf_cond_op, (void*) COND_NE );
    jrt_register_op( 0x9B, "IFLT", jrtf_cond_op, (void*) COND_LT );
    jrt_register_op( 0x9C, "IFGE", jrtf_cond_op, (void*) COND_GE );
    jrt_register_op( 0x9D, "IFGT", jrtf_cond_op, (void*) COND_GT );
    jrt_register_op( 0x9E, "IFLE", jrtf_cond_op, (void*) COND_LE );

    jrt_register_op( 0xA0, "IF_ICMPNE", jrtf_cond_cmp, (void*) COND_NE );
    jrt_register_op( 0xA1, "IF_ICMPLT", jrtf_cond_cmp, (void*) COND_LT );
    jrt_register_op( 0xA2, "IF_ICMPGE", jrtf_cond_cmp, (void*) COND_GE );
    jrt_register_op( 0xA3, "IF_ICMPGT", jrtf_cond_cmp, (void*) COND_GT );
    jrt_register_op( 0xA4, "IF_ICMPLE", jrtf_cond_cmp, (void*) COND_LE );

    jrt_register_op( 0xA7, "GOTO", jrtf_base_goto, NULL );
}

#endif	/* _JRT_BASE_H */

