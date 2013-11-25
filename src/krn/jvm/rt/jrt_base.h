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
#include "jrt_fld.h"
#include "jrt_dbl.h"
#include "jrt_method.h"

typedef struct {
    JRType type;
    ui08 offs;
} JRTBTypeOffs;

void jrtf_base_aload_null( JRFrame* frame, void* x ) {
    JRObjRef *c = jr_push( frame->stack );
    c->type = OBJECT;
    c->ref = NULL;
}

void jrtf_base_new( JRFrame* frame, void* x ) {
    int cls_idx = (short) ((frame->code[1] << 8) | frame->code[2]);
    JRObjRef* cls_name_idx = jropeek( frame->consts, cls_idx - 1 );
    char* cls_name = (char*)((JStringUtf*) jropeek( frame->consts, cls_name_idx->value - 1 )->ref)->data;
    JClass cls = jcf_load_class( cls_name );
    JRObjRef *e = jr_push( frame->stack );
    e->type = OBJECT;
    e->ref = jo_new( cls, false );
    frame->code += 2;
}

void jrtf_base_pop( JRFrame* frame, void* x ) {
    jr_pop( frame->stack );
}

void jrtf_base_dup( JRFrame* frame, void* x ) {
    JRObjRef *e = jr_peek( frame->stack );
    int d = (int) x;
    while ( d-- > 0 )
        *jr_push( frame->stack ) = *e;
}

void jrtf_base_swap( JRFrame* frame, void* x ) {
    JRObjRef *e0 = jr_pop( frame->stack );
    JRObjRef *e1 = jr_pop( frame->stack );
    *jr_push( frame->stack ) = *e0;
    *jr_push( frame->stack ) = *e1;
}

void jrtf_base_goto( JRFrame* frame, void* x ) {
    int d_addr = (short) ((frame->code[1] << 8) | frame->code[2]);
    frame->code += d_addr - 1;
}

void jrtf_base_return( JRFrame* frame, void* x ) {
    if ( (JRType) x == VOID )
        return;
    JRObjRef *e = jr_peek( frame->stack );
    assert_op_t( frame, e->type, (JRType) x );
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
    int d = (int) x;
    int c_idx = 0;
    while ( d > 0 ) {
        frame->code++;
        c_idx = (c_idx << 8) | *frame->code;
        d--;
    }
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
            //double *d = (double*) & e->value;
            break;
        }
        case LONG:
            break;
        default:
            throw (false, "%s for not DOUBLE or LONG type", jrt_get_op( op ));
    }
}

JRTBTypeOffs RTBTO_C_I = { INT, 0x03 }, RTBTO_C_L = { LONG, 0x09 };
JRTBTypeOffs RTBTO_L_I = { INT, 0x1A }, RTBTO_L_L = { LONG, 0x1E }, RTBTO_L_F = { FLOAT, 0x22 }, RTBTO_L_D = { DOUBLE, 0x26 }, RTBTO_L_A = { OBJECT, 0x2A };
JRTBTypeOffs RTBTO_S_I = { INT, 0x3B }, RTBTO_S_L = { LONG, 0x3F }, RTBTO_S_F = { FLOAT, 0x43 }, RTBTO_S_D = { DOUBLE, 0x47 }, RTBTO_S_A = { OBJECT, 0x4B };

void jrt_base_init( ) {
    jrt_register_op( 0x01, "ALOAD_NULL", jrtf_base_aload_null );
    for ( int i = 0x02; i < 0x09; i++ )
        jrt_register_op_sprm( i, "ICONST", jrtf_base_const, &RTBTO_C_I );
    for ( int i = 0x09; i < 0x0C; i++ )
        jrt_register_op_sprm( i, "LCONST", jrtf_base_const, &RTBTO_C_L );
    for ( int i = 0x0C; i < 0x10; i++ )
        jrt_register_op( i, "DCONST", jrtf_base_dconst );
    jrt_register_op_sprm( 0x10, "BIPUSH", jrtf_base_xpush, (void*) 1 );
    jrt_register_op_sprm( 0x11, "SIPUSH", jrtf_base_xpush, (void*) 2 );
    jrt_register_op_sprm( 0x12, "LDC", jrtf_base_ldc, (void*) 1 );
    jrt_register_op_sprm( 0x13, "LDCW", jrtf_base_ldc, (void*) 2 );
    jrt_register_op( 0x14, "LDC2W", jrtf_base_ldc2w );
    jrt_register_op_sprm( 0x15, "ILOAD", jrtf_base_xload, (void*) INT );
    jrt_register_op_sprm( 0x16, "LLOAD", jrtf_base_xload, (void*) LONG );
    jrt_register_op_sprm( 0x17, "FLOAD", jrtf_base_xload, (void*) FLOAT );
    jrt_register_op_sprm( 0x18, "DLOAD", jrtf_base_xload, (void*) DOUBLE );
    //reserved
    for ( int i = 0x1A; i < 0x1E; i++ )
        jrt_register_op_sprm( i, "ILOAD", jrtf_base_load, &RTBTO_L_I );
    for ( int i = 0x1E; i < 0x22; i++ )
        jrt_register_op_sprm( i, "LLOAD", jrtf_base_load, &RTBTO_L_L );
    for ( int i = 0x22; i < 0x26; i++ )
        jrt_register_op_sprm( i, "FLOAD", jrtf_base_load, &RTBTO_L_F );
    for ( int i = 0x26; i < 0x29; i++ )
        jrt_register_op_sprm( i, "DLOAD", jrtf_base_load, &RTBTO_L_D );
    for ( int i = 0x2A; i < 0x2E; i++ )
        jrt_register_op_sprm( i, "ALOAD", jrtf_base_load, &RTBTO_L_A );
    jrt_register_op_sprm( 0x2E, "IALOAD", jrtf_arr_load, (void*) AINT );
    jrt_register_op_sprm( 0x2F, "LALOAD", jrtf_arr_load, (void*) ALONG );
    jrt_register_op_sprm( 0x30, "FALOAD", jrtf_arr_load, (void*) AFLOAT );
    jrt_register_op_sprm( 0x31, "DALOAD", jrtf_arr_load, (void*) ADOUBLE );
    jrt_register_op_sprm( 0x32, "AALOAD", jrtf_arr_load, (void*) AOBJECT );
    jrt_register_op_sprm( 0x33, "BALOAD", jrtf_arr_load, (void*) ABYTE );
    jrt_register_op_sprm( 0x34, "CALOAD", jrtf_arr_load, (void*) ACHAR );
    jrt_register_op_sprm( 0x35, "SALOAD", jrtf_arr_load, (void*) ASHORT );
    jrt_register_op_sprm( 0x36, "ISTORE", jrtf_base_xstore, (void*) INT );
    jrt_register_op_sprm( 0x37, "LSTORE", jrtf_base_xstore, (void*) LONG );
    jrt_register_op_sprm( 0x38, "FSTORE", jrtf_base_xstore, (void*) FLOAT );
    jrt_register_op_sprm( 0x39, "DSTORE", jrtf_base_xstore, (void*) DOUBLE );
    jrt_register_op_sprm( 0x3A, "ASTORE", jrtf_base_xstore, (void*) OBJECT );
    for ( int i = 0x3B; i < 0x3F; i++ )
        jrt_register_op_sprm( i, "ISTORE", jrtf_base_store, &RTBTO_S_I );
    for ( int i = 0x3F; i < 0x43; i++ )
        jrt_register_op_sprm( i, "LSTORE", jrtf_base_store, &RTBTO_S_L );
    for ( int i = 0x43; i < 0x47; i++ )
        jrt_register_op_sprm( i, "FSTORE", jrtf_base_store, &RTBTO_S_F );
    for ( int i = 0x47; i < 0x4B; i++ )
        jrt_register_op_sprm( i, "DSTORE", jrtf_base_store, &RTBTO_S_D );
    for ( int i = 0x4B; i < 0x4F; i++ )
        jrt_register_op_sprm( i, "ASTORE", jrtf_base_store, &RTBTO_S_A );
    jrt_register_op_sprm( 0x4F, "IASTORE", jrtf_arr_store, (void*) AINT );
    jrt_register_op_sprm( 0x50, "LASTORE", jrtf_arr_store, (void*) ALONG );
    jrt_register_op_sprm( 0x51, "FASTORE", jrtf_arr_store, (void*) AFLOAT );
    jrt_register_op_sprm( 0x52, "DASTORE", jrtf_arr_store, (void*) ADOUBLE );
    jrt_register_op_sprm( 0x53, "AASTORE", jrtf_arr_store, (void*) AOBJECT );
    jrt_register_op_sprm( 0x54, "BASTORE", jrtf_arr_store, (void*) ABYTE );
    jrt_register_op_sprm( 0x55, "CASTORE", jrtf_arr_store, (void*) ACHAR );
    jrt_register_op_sprm( 0x56, "SASTORE", jrtf_arr_store, (void*) ASHORT );
    jrt_register_op( 0x57, "POP", jrtf_base_pop );
    jrt_register_op( 0x58, "POP2", jrtf_base_pop );
    jrt_register_op_sprm( 0x59, "DUP", jrtf_base_dup, (void*) 1 );
    jrt_register_op_sprm( 0x5A, "DUP_X1", jrtf_base_dup, (void*) 2 );
    jrt_register_op_sprm( 0x5B, "DUP_X2", jrtf_base_dup, (void*) 3 );
    jrt_register_op_sprm( 0x5C, "DUP2", jrtf_base_dup, (void*) 1 );
    jrt_register_op_sprm( 0x5D, "DUP2_X1", jrtf_base_dup, (void*) 2 );
    jrt_register_op_sprm( 0x5E, "DUP2_X2", jrtf_base_dup, (void*) 3 );
    jrt_register_op( 0x5F, "SWAP", jrtf_base_swap );
    jrt_register_op_sprm( 0x60, "IADD", jrtf_int_op, (void*) I_OP_ADD );
    jrt_register_op_sprm( 0x61, "LADD", jrtf_long_op, (void*) L_OP_ADD );
//    jrt_register_op_sprm( 0x62, "FADD", jrtf_flt_op, (void*) F_OP_ADD );
    jrt_register_op_sprm( 0x63, "DADD", jrtf_dbl_op, (void*) D_OP_ADD );
    jrt_register_op_sprm( 0x64, "ISUB", jrtf_int_op, (void*) I_OP_SUB );
    jrt_register_op_sprm( 0x65, "LSUB", jrtf_long_op, (void*) L_OP_SUB );
//    jrt_register_op_sprm( 0x66, "FSUB", jrtf_flt_op, (void*) F_OP_SUB );
    jrt_register_op_sprm( 0x67, "DSUB", jrtf_dbl_op, (void*) D_OP_SUB );
    jrt_register_op_sprm( 0x68, "IMUL", jrtf_int_op, (void*) I_OP_MUL );
    jrt_register_op_sprm( 0x69, "LMUL", jrtf_long_op, (void*) L_OP_MUL );
//    jrt_register_op_sprm( 0x6A, "FMUL", jrtf_flt_op, (void*) F_OP_MUL );
    jrt_register_op_sprm( 0x6B, "DMUL", jrtf_dbl_op, (void*) D_OP_MUL );
    jrt_register_op_sprm( 0x6C, "IDIV", jrtf_int_op, (void*) I_OP_DIV );
    jrt_register_op_sprm( 0x6D, "LDIV", jrtf_long_op, (void*) L_OP_DIV );
//    jrt_register_op_sprm( 0x6E, "FDIV", jrtf_flt_op, (void*) F_OP_DIV );
    jrt_register_op_sprm( 0x6F, "DDIV", jrtf_dbl_op, (void*) D_OP_DIV );
    jrt_register_op_sprm( 0x70, "IREM", jrtf_int_op, (void*) I_OP_REM );
    jrt_register_op_sprm( 0x71, "LREM", jrtf_long_op, (void*) L_OP_REM );
//    jrt_register_op_sprm( 0x72, "FREM", jrtf_flt_op, (void*) F_OP_REM );
//    jrt_register_op_sprm( 0x73, "DREM", jrtf_dbl_op, (void*) D_OP_REM );

    jrt_register_op_sprm( 0x7E, "IAND", jrtf_int_op, (void*) I_OP_AND );
    jrt_register_op_sprm( 0x7F, "LAND", jrtf_long_op, (void*) L_OP_AND );
    jrt_register_op_sprm( 0x80, "IOR", jrtf_int_op, (void*) I_OP_OR );
    jrt_register_op_sprm( 0x81, "LOR", jrtf_long_op, (void*) L_OP_OR );
    jrt_register_op_sprm( 0x82, "IXOR", jrtf_int_op, (void*) I_OP_XOR );
    jrt_register_op_sprm( 0x83, "LXOR", jrtf_long_op, (void*) L_OP_XOR );
    jrt_register_op( 0x84, "IINC", jrtf_int_inc );
    jrt_register_op_sprm( 0x85, "I2L", jrtf_int_to, (void*) ALONG );
    jrt_register_op_sprm( 0x86, "I2F", jrtf_int_to, (void*) AFLOAT );
    jrt_register_op_sprm( 0x87, "I2D", jrtf_int_to, (void*) ADOUBLE );
    jrt_register_op_sprm( 0x88, "L2I", jrtf_long_to, (void*) AINT );
    jrt_register_op_sprm( 0x89, "L2F", jrtf_long_to, (void*) AFLOAT );
    jrt_register_op_sprm( 0x8A, "L2D", jrtf_long_to, (void*) ADOUBLE );
//    jrt_register_op_sprm( 0x8B, "F2I", jrtf_flt_to, (void*) AINT );
//    jrt_register_op_sprm( 0x8C, "F2L", jrtf_flt_to, (void*) ALONG );
//    jrt_register_op_sprm( 0x8D, "F2D", jrtf_flt_to, (void*) ADOUBLE );
    jrt_register_op_sprm( 0x8E, "D2I", jrtf_dbl_to, (void*) AINT );
    jrt_register_op_sprm( 0x8F, "D2L", jrtf_dbl_to, (void*) ALONG );
    jrt_register_op_sprm( 0x90, "D2F", jrtf_dbl_to, (void*) AFLOAT );
    jrt_register_op_sprm( 0x91, "I2B", jrtf_int_to, (void*) ABYTE );
    jrt_register_op_sprm( 0x92, "I2C", jrtf_int_to, (void*) ACHAR );
    jrt_register_op_sprm( 0x93, "I2S", jrtf_int_to, (void*) ASHORT );
    jrt_register_op_sprm( 0x94, "LCMP", jrtf_long_op, (void*) L_OP_CMP );

    jrt_register_op_sprm( 0x99, "IFEQ", jrtf_cond_op, (void*) COND_EQ );
    jrt_register_op_sprm( 0x9A, "IFNE", jrtf_cond_op, (void*) COND_NE );
    jrt_register_op_sprm( 0x9B, "IFLT", jrtf_cond_op, (void*) COND_LT );
    jrt_register_op_sprm( 0x9C, "IFGE", jrtf_cond_op, (void*) COND_GE );
    jrt_register_op_sprm( 0x9D, "IFGT", jrtf_cond_op, (void*) COND_GT );
    jrt_register_op_sprm( 0x9E, "IFLE", jrtf_cond_op, (void*) COND_LE );
    jrt_register_op_sprm( 0x9F, "IF_ICMPEQ", jrtf_cond_cmp, (void*) COND_EQ );
    jrt_register_op_sprm( 0xA0, "IF_ICMPNE", jrtf_cond_cmp, (void*) COND_NE );
    jrt_register_op_sprm( 0xA1, "IF_ICMPLT", jrtf_cond_cmp, (void*) COND_LT );
    jrt_register_op_sprm( 0xA2, "IF_ICMPGE", jrtf_cond_cmp, (void*) COND_GE );
    jrt_register_op_sprm( 0xA3, "IF_ICMPGT", jrtf_cond_cmp, (void*) COND_GT );
    jrt_register_op_sprm( 0xA4, "IF_ICMPLE", jrtf_cond_cmp, (void*) COND_LE );

    jrt_register_op( 0xA7, "GOTO", jrtf_base_goto );

    jrt_register_op( 0xAB, "LKSW", jrtf_cond_lksw );
    jrt_register_op_sprm_flags( 0xAC, "IRETURN", jrtf_base_return, (void*) INT, F_RTO_RETURN );
    jrt_register_op_sprm_flags( 0xAD, "LRETURN", jrtf_base_return, (void*) LONG, F_RTO_RETURN );

    jrt_register_op_sprm_flags( 0xAF, "DRETURN", jrtf_base_return, (void*) DOUBLE, F_RTO_RETURN );
    jrt_register_op_sprm_flags( 0xB0, "ARETURN", jrtf_base_return, (void*) OBJECT, F_RTO_RETURN );
    jrt_register_op_sprm_flags( 0xB1, "RETURN", jrtf_base_return, (void*) VOID, F_RTO_RETURN );

    jrt_register_op_sprm( 0xB2, "GETSTAT", jrtf_fld_op, (void*) FLD_GS );
    jrt_register_op_sprm( 0xB3, "PUTSTAT", jrtf_fld_op, (void*) FLD_PS );
    jrt_register_op_sprm( 0xB4, "GETFIELD", jrtf_fld_op, (void*) FLD_GF );
    jrt_register_op_sprm( 0xB5, "PUTFIELD", jrtf_fld_op, (void*) FLD_PF );
    jrt_register_op_sprm( 0xB6, "INVOKE_VIRTUAL", jrtf_mth_op, (void*) MTH_INVIRT );
    jrt_register_op_sprm( 0xB7, "INVOKE_SPECIAL", jrtf_mth_op, (void*) MTH_INSPEC );
    jrt_register_op_sprm( 0xB8, "INVOKE_STATIC", jrtf_mth_op, (void*) MTH_INSTAT );

    jrt_register_op( 0xBB, "NEW", jrtf_base_new );
    jrt_register_op( 0xBC, "NEWARRAY", jrtf_arr_new );

    jrt_register_op( 0xBE, "ARRLEN", jrtf_arr_len );

    jrt_register_op( 0xC7, "IFNONNULL", jrtf_cond_nonnull );
}

#endif	/* _JRT_BASE_H */

