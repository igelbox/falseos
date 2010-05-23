/* 
 * File:   jobject.h
 * Author: igel
 *
 * Created on 01 Май 2010 г., 22:44
 */

#ifndef _JOBJECT_H
#define	_JOBJECT_H

#include "jcore.h"
#include "jmemory.h"
#include "jstream.h"
#include "jclass.h"
#include "jutils.h"

JClass jo_get_class( JObject o ) {
    return (JClass) o->class;
}

JObjectField jo_find_field( JObject o, const char *name, const char *type ) {
//    printf( "FFLD %s.%s:%s\n", jc_get_name( jo_get_class( o ) ), name, type );
//    printf( "FCNT:%i\n", o->fields_count );
    for ( int i = 0; i < o->fields_count; i++ ) {
        JObjectField f = o->fields[i];
//        printf( "F %s:%s\n", jf_get_name( f->field ), jf_get_type( f->field ) );
        if ( strcmp( name, jf_get_name( f->field ) ) == 0 )
//            if ( strcmp( type, jf_get_type( f->field ) ) == 0 )
                return f;
    }
    return NULL;
}

JObject jo_new( JClass c, bool stat ) {
    JObject r = jmm_alloc( sizeof (_JObject) );
    r->class = c;
    r->fields_count = 0;
    //    printf( "NEW CFC:%i\n", c->fields_count );
    for ( int i = 0; i < c->fields_count; i++ ) {
        bool s = c->fields[i]->attributes_flags & JFAF_STATIC;
        if ( (s && stat) || (!s && !stat) )
            r->fields_count++;
    }
    r->fields = jmm_alloc( sizeof (void*) * r->fields_count );
    int f_idx = 0;
    for ( int i = 0; i < c->fields_count; i++ ) {
        bool s = c->fields[i]->attributes_flags & JFAF_STATIC;
        if ( (s && stat) || (!s && !stat) ) {
            JObjectField f = jmm_alloc( sizeof (_JObjectField) );
            f->field = c->fields[i];
            f->val.type = jf_get_rtype( f->field );
            r->fields[f_idx] = f;
            f_idx++;
        }
    }
    return r;
}

#endif	/* _JOBJECT_H */

