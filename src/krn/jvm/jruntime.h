/* 
 * File:   jruntime.h
 * Author: igel
 *
 * Created on 25 Апрель 2010 г., 1:56
 */

#ifndef _JRUNTIME_H
#define	_JRUNTIME_H
//#define _DEBUG

#include "jcore.h"
#include "rt/jrt.h"
#include "jmethod.h"
#include "jclass.h"
#include "jarray.h"

void assertop( const char* op_name, JRType type, JRType tgt_type ) {
    if ( type != tgt_type )
        throw ("%s required %s found %s", op_name, jr_get_type_name( tgt_type ), jr_get_type_name( type ));
}

void assertaop( const char* op_name, JRAType type, JRAType tgt_type ) {
    if ( type != tgt_type )
        throw ("%s required %s found %s", op_name, jr_get_atype_name( tgt_type ), jr_get_atype_name( type ));
}

void jr_get_fm_info( JClass c, int idx, char** cls_name, char** name, char** type ) {
    JMethodAndFieldInfo *fi = (JMethodAndFieldInfo*) c->consts[idx - 1].ref;
    int class_name_idx = c->consts[fi->class_info_idx - 1].value;
    *cls_name = (char*)((JStringUtf*) c->consts[class_name_idx - 1].ref)->data;
    JNameAndTypeInfo *nat = (JNameAndTypeInfo*) c->consts[fi->nat_idx - 1].ref;
    *name = (char*)((JStringUtf*) c->consts[nat->name_idx - 1].ref)->data;
    *type = (char*)((JStringUtf*) c->consts[nat->type_idx - 1].ref)->data;
}

char* jr_get_string_ref_data( void* o ) {
    JStringUtf* s = (JStringUtf*) o;
    if ( s->length <= 0xFFFF )
        return (char*) s->data;
    JObject obj = (JObject) o;
    JClass c = jo_get_class( obj );
    assert( c != NULL, "String[C]" );
    JMethod m = jc_find_method( c, "getBytes", "()[B", false );
    assert( m != NULL, "String[BM]" );
    JRObjRef args[2];
    args[0].type = OBJECT;
    args[0].ref = obj;
    args[1] = EVOID;
    JObjectField f = jo_find_field( obj, "data", "[C" );
    assert( f != NULL, "String[DF]" );
    JRObjRef mv = jr_invoke( m, args );
    JRObjRef* v = /*&f->val;*/&mv;
    if ( v->ref ) {
        assertop( "String[DF]", v->type, OBJECT );
        JArray a = (JArray) v->ref;
        assert( a->class == NULL, "String[DFA]" );
        assertaop( "String[DF]", a->type, ABYTE );
        return (char*) a->data;
    }
    return NULL;
}

JRObjRef jr_invoke( JMethod m, JRObjRef *args ) {
#ifdef _DEBUG
    printf( "--%s.%s:%s\n", jc_get_name( m->class ), jm_get_name( m ), jm_get_type( m ) );
#endif
    JMethodCodeAttribute acode = m->code_attribute;
    if ( !acode ) {
        char* cname = jc_get_name( jm_get_class( m ) );
        if ( strcmp( cname, "fos/drv/chr/Tty" ) == 0 ) {
            char* mname = jm_get_name( m );
            if ( strcmp( mname, "print" ) == 0 ) {
                char* mtype = jm_get_type( m );
                if ( strcmp( mtype, "(Ljava/lang/String;)V" ) == 0 ) {
                    char* sd = jr_get_string_ref_data( args[0].ref );
                    if ( sd )
                        tty_puts( sd );
                    return EVOID;
                } else if ( strcmp( mtype, "(C)V" ) == 0 ) {
                    tty_putchar( args[0].value );
                    return EVOID;
                } else if ( strcmp( mtype, "(I)V" ) == 0 ) {
                    tty_put_i32( args[0].value );
                    return EVOID;
                } else if ( strcmp( mtype, "(J)V" ) == 0 ) {
                    tty_put_i64( args[0].value );
                    return EVOID;
                }
            } else if ( (strcmp( mname, "getColor" ) == 0) && (strcmp( jm_get_type( m ), "()C" ) == 0) ) {
                JRObjRef r;
                r.type = INT;
                r.value = tty_get_color( );
                return r;
            } else if ( (strcmp( mname, "setColor" ) == 0) && (strcmp( jm_get_type( m ), "(C)V" ) == 0) ) {
                tty_set_color( args[0].value );
                return EVOID;
            } else if ( (strcmp( mname, "clear" ) == 0) && (strcmp( jm_get_type( m ), "()V" ) == 0) ) {
                tty_clear( );
                return EVOID;
            }
        } else if ( strcmp( cname, "fos/sys/IOPorts" ) == 0 ) {
            char* mname = jm_get_name( m );
            char* mtype = jm_get_type( m );
            if ( (strcmp( mname, "in" ) == 0) && (strcmp( mtype, "(I)I" ) == 0) ) {
                JRObjRef r;
                r.type = INT;
                r.value = inportb( args[0].value );
                return r;
            } else if ( (strcmp( mname, "out" ) == 0) && (strcmp( mtype, "(II)V" ) == 0) ) {
                outportb( args[1].value, args[0].value );
                return EVOID;
            }
        } else if ( strcmp( cname, "fos/sys/Interrupts" ) == 0 ) {
            char* mname = jm_get_name( m );
            char* mtype = jm_get_type( m );
            if ( (strcmp( mname, "halt" ) == 0) && (strcmp( mtype, "()V" ) == 0) ) {
                __asm( "hlt" );
                return EVOID;
            }
        } else if ( strcmp( cname, "java/lang/Runtime" ) == 0 ) {
            char* mname = jm_get_name( m );
            char* mtype = jm_get_type( m );
            JRObjRef r;
            r.type = LONG;
            if ( (strcmp( mname, "freeMemory" ) == 0) && (strcmp( mtype, "()J" ) == 0) ) {
                r.value = jmm_avail( );
                return r;
            } else if ( (strcmp( mname, "maxMemory" ) == 0) && (strcmp( mtype, "()J" ) == 0) ) {
                r.value = jmm_max( );
                return r;
            }
        } else if ( strcmp( cname, "java/lang/String" ) == 0 ) {
            char* mname = jm_get_name( m );
            char* mtype = jm_get_type( m );
            JRObjRef r;
            r.type = INT;
            if ( (strcmp( mname, "charAt" ) == 0) && (strcmp( mtype, "(I)C" ) == 0) ) {
                r.value = ((JStringUtf*) args[0].ref)->data[args[1].value];
                return r;
            } else if ( (strcmp( mname, "equals" ) == 0) && (strcmp( mtype, "(Ljava/lang/String;)Z" ) == 0) ) {
                char* s0 = jr_get_string_ref_data( args[0].ref );
                char* s1 = jr_get_string_ref_data( args[1].ref );
                if ( s0 && s1 )
                    r.value = strcmp( s0, s1 ) == 0;
                else
                    r.value = !s0 && !s1;
                return r;
            }
        }
        throw ("JNI %s.%s:%s", cname, jm_get_name( m ), jm_get_type( m ));
    }

    JRObjPool locals;
    locals.size = acode->max_locals + 1;
    JRObjRef ldata[locals.size];
    locals.data = ldata;
    bool args_end = !args;
    for ( int i = 0; i < acode->max_locals; i++ ) {
        if ( !args_end && (args[i].type == VOID) )
            args_end = true;
        if ( args_end )
            ldata[i] = EVOID;
        else
            ldata[i] = args[i];
    }

    JRStack stack;
    stack.pos = 0;
    stack.size = acode->max_stack + 1;
    JRObjRef sdata[stack.size];
    stack.data = sdata;
    JRFrame frm;
    frm.class = jm_get_class( m );
    frm.method = m;
    JRObjPool consts;
    consts.data = frm.class->consts;
    consts.size = frm.class->consts_count;
    frm.stack = &stack;
    frm.locals = &locals;
    frm.consts = &consts;
    frm.code = acode->code;
    ui08* code_end = &acode->code[acode->code_length];
    while ( frm.code < code_end ) {
        JRTOp rop = jrt_get_op( *frm.code );
        if ( rop.func ) {
            rop.func( &frm, rop.sprm );
            if ( rop.flgs & F_RTO_RETURN )
                goto exit;
        } else
            throw ("unknown op.code %x", *frm.code);
        frm.code++;
    }
exit:
    if ( stack.pos == 0 )
        return EVOID;
    JRObjRef r = *jr_pop( &stack );
    return r;
}

#endif	/* _JRUNTIME_H */

