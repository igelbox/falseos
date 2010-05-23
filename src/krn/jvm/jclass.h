/* 
 * File:   jclass.h
 * Author: igel
 *
 * Created on 24 Апрель 2010 г., 21:41
 */

#ifndef _JCLASS_H
#define	_JCLASS_H

#include "jcore.h"
#include "jmemory.h"
#include "jstream.h"

#include "jmethod.h"
#include "jfield.h"
#include "jobject.h"

#define MAGIC 0xCAFEBABE

JClass jcf_load_class( const char *name );

char* jc_get_name( JClass c ) {
    ui16 class_name_idx = c->consts[c->this_class_idx - 1].value;
    return ((JStringUtf*) c->consts[class_name_idx - 1].ref)->data;
}

char* jc_get_super_name( JClass c ) {
    ui16 class_name_idx = c->consts[c->super_class_idx - 1].value;
    return ((JStringUtf*) c->consts[class_name_idx - 1].ref)->data;
}

JClass jc_get_super( JClass c ) {
    return jcf_load_class( jc_get_super_name( c ) );
}

JObjectField jc_find_field( JClass c, const char *name, const char *type, bool inh ) {
    JObjectField f = jo_find_field( c->class_fields, name, type );
    if ( f )
        return f;
    if ( !inh || (strcmp( "java/lang/Object", jc_get_super_name( c ) ) == 0) )
        return NULL;
    return jc_find_field( jc_get_super( c ), name, type, inh );
}

JMethod jc_find_method( JClass c, const char *name, const char *type, bool inh ) {
    for ( int i = 0; i < c->methods_count; i++ ) {
        JMethod m = c->methods[i];
        char *mname = jm_get_name( m );
        char *mtype = jm_get_type( m );
        if ( strcmp( name, mname ) == 0 )
            if ( strcmp( type, mtype ) == 0 )
                return m;
    }
    if ( !inh || (strcmp( "java/lang/Object", jc_get_super_name( c ) ) == 0) )
        return NULL;
    return jc_find_method( jc_get_super( c ), name, type, inh );
}

JMethod jc_main_method( JClass c ) {
    JMethod r = jc_find_method( c, "main", "([Ljava/lang/String;)V", false );
    if ( !r || (r->attributes_flags != 9) )
        return NULL;
    return r;
}

JMethod jc_clinit_method( JClass c ) {
    for ( int i = 0; i < c->methods_count; i++ ) {
        JMethod m = c->methods[i];
        if ( (m->attributes_flags & 9) && (strcmp( "<clinit>", jm_get_name( m ) ) == 0) )
            return m;
    }
    return NULL;
}

JMethod jc_init_method( JClass c ) {
    for ( int i = 0; i < c->methods_count; i++ ) {
        JMethod m = c->methods[i];
        if ( strcmp( "<init>", jm_get_name( m ) ) == 0 )
            if ( strcmp( "()V", jm_get_type( m ) ) == 0 )
                return m;
    }
    return NULL;
}

JNameAndTypeInfo* jc_load_nat( FStream f ) {
    JNameAndTypeInfo *ni = jmm_alloc( sizeof (JNameAndTypeInfo) );
    ni->name_idx = js_readw( f );
    ni->type_idx = js_readw( f );
    return ni;
}

JMethodAndFieldInfo* jc_load_mfi( FStream f ) {
    JMethodAndFieldInfo *mi = jmm_alloc( sizeof (JMethodAndFieldInfo) );
    mi->class_info_idx = js_readw( f );
    mi->nat_idx = js_readw( f );
    return mi;
}

JStringUtf* jc_load_str( FStream f ) {
    JStringUtf *s = jmm_alloc( sizeof (JStringUtf) );
    s->length = js_readw( f );
    s->data = jmm_alloc( s->length + 1 );
    fs_read( f, s->data, s->length );
    s->data[s->length] = 0;
    return s;
}

JClass jc_load( FStream fs ) {
    JClass r = jmm_alloc( sizeof (_JClass) );
    r->magic = js_readd( fs );
    assert( r->magic == MAGIC, "wrong class magic %x", r->magic );
    r->minor = js_readw( fs );
    r->major = js_readw( fs );
    ui16 ver = (r->major << 8) | r->minor;
    assert( ver <= 0x3200, "wrong class version %i.%i", r->major, r->minor );
    ui16 cleft = js_readw( fs );
    r->consts = jmm_alloc( sizeof (JRObjRef) * cleft );
    r->consts_count = 0;
    cleft--;
    for (; cleft > 0; cleft-- ) {
        ui08 tag = js_readb( fs );
        JRObjRef *c = &r->consts[r->consts_count];
        switch ( tag ) {
            case 1:
                c->type = OBJECT;
                c->ref = jc_load_str( fs );
                break;
            case 3:
                c->type = INT;
                c->value = js_readd( fs );
                break;
            case 4:
                c->type = FLOAT;
                c->value = js_readd( fs );
                break;
            case 5:
                c->type = LONG;
                c->value = js_readq( fs );
                cleft--;
                r->consts_count++;
                break;
            case 6:
                c->type = DOUBLE;
                c->value = js_readq( fs );
                cleft--;
                r->consts_count++;
                break;
            case 7://ClassInfo
                c->type = _OTHER;
                c->value = js_readw( fs );
                break;
            case 8://StringRef
                c->type = STRING_REF;
                c->value = js_readw( fs );
                break;
            case 9://FieldInfo
                c->type = OBJECT;
                c->ref = jc_load_mfi( fs );
                break;
            case 10://MethodInfo
                c->type = OBJECT;
                c->ref = jc_load_mfi( fs );
                break;
            case 11://InterfaceMethodInfo
                c->type = OBJECT;
                c->ref = jc_load_mfi( fs );
                break;
            case 12:
                c->type = OBJECT;
                c->ref = jc_load_nat( fs );
                break;
            default:
                throw ("unknown const tag %x", tag);
        }
        r->consts_count++;
    }
    for ( int i = 0; i < r->consts_count; i++ )
        if ( r->consts[i].type == STRING_REF ) {
            JRObjRef *c = &r->consts[r->consts[i].value - 1];
            r->consts[i] = *c;
        }
    r->attributes = js_readw( fs );
    r->this_class_idx = js_readw( fs );
    r->super_class_idx = js_readw( fs );
    r->interfaces_count = js_readw( fs );
    fs_skip( fs, r->interfaces_count * 2 );
    r->fields_count = js_readw( fs );
    r->fields = jmm_alloc( sizeof (void*) * r->fields_count );
    for ( int i = 0; i < r->fields_count; i++ )
        r->fields[i] = jf_load( fs, r );
    r->class_fields = jo_new( r, true );
    r->methods_count = js_readw( fs );
    r->methods = jmm_alloc( sizeof (void*) * r->methods_count );
    for ( int i = 0; i < r->methods_count; i++ )
        r->methods[i] = jm_load( fs, r );
    return r;
}

typedef struct {
    char *name;
    JClass class;
} JClassRef;

JClassRef *classes;
;
int class_count = 0;
const char *class_path;

void jcf_init( const char *cls_path ) {
    classes = jmm_alloc( sizeof (JClassRef) * 16 );
    class_path = cls_path;
}

JClass jcf_load_class( const char *name ) {
    if ( strcmp( name, "java/lang/Object" ) == 0 )
        return NULL;
    for ( int i = 0; i < class_count; i++ )
        if ( strcmp( classes[i].name, name ) == 0 )
            return classes[i].class;
//    printf( "loading class \"%s\"\n", name );
    FStream f = fs_open( name );
    assert( fs_exists( f ), "class \"%s\" not found", name );
    JClass class = jc_load( f );
    fs_close( f );
    char *_name = jmm_alloc( strlen( name ) + 1 );
    strcpy( _name, name );
    classes[class_count].name = _name;
    classes[class_count].class = class;
    class_count++;
    JMethod clinit = jc_clinit_method( class );
    if ( clinit )
        jr_invoke( clinit, NULL );
    return class;
}

#endif	/* _JCLASS_H */

