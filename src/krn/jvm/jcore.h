#ifndef _JCORE_H
#define	_JCORE_H

#include <stdarg.h>
#include "../drv/tty.h"
#include "jclass.h"

typedef unsigned long long ui64;
typedef unsigned int ui32;
typedef unsigned short ui16;
typedef unsigned char ui08;
typedef unsigned int bool;
#define false 0
#define true 1
#define NULL 0

typedef enum {
    VOID,
    INT,
    LONG,
    FLOAT,
    DOUBLE,
    OBJECT,
    STRING_REF,
    _OTHER
} JRType;

typedef struct {
    JRType type;
    ui64 value;
    void* ref;
} JRObjRef;

JRObjRef EVOID;

/*typedef struct {
    ui16 name_idx;
    ui32 length;
    ui08 *data;
} _JMethodAttribute;
typedef _JMethodAttribute* JMethodAttribute;*/

typedef struct {
    ui32 length;
    ui16 max_stack, max_locals;
    ui32 code_length;
    ui08 *code;
} _JMethodCodeAttribute;
typedef _JMethodCodeAttribute* JMethodCodeAttribute;

typedef struct {
    void *class;
    ui16 attributes_flags;
    ui16 name_idx, type_idx;
    //    ui16 attributes_count;
    //    void **attributes;
    void *code_attribute;
} _JMethod;
typedef _JMethod* JMethod;

#define JFAF_STATIC 0x8;

typedef struct {
    void *class;
    ui16 attributes_flags;
    ui16 name_idx, type_idx;
    //    ui16 attributes_count;
    //    void **attributes;
} _JField;
typedef _JField* JField;

typedef struct {
    JRObjRef val;
    JField field;
} _JObjectField;
typedef _JObjectField* JObjectField;

typedef struct {
    void *class;
    ui16 fields_count;
    JObjectField *fields;
} _JObject;
typedef _JObject* JObject;

typedef enum {
    ABYTE,
    ACHAR,
    ASHORT,
    AINT,
    ALONG,
    AFLOAT,
    ADOUBLE,
    AOBJECT
} JRAType;

typedef struct {
    void *class;
    JRAType type;
    ui32 size;
    void *data;
} _JArray;
typedef _JArray* JArray;

typedef struct {
    ui32 magic;
    ui16 major, minor;
    JRObjRef *consts;
    ui32 consts_count;
    ui16 attributes;
    ui16 this_class_idx, super_class_idx;
    ui16 interfaces_count;
    ui16 fields_count;
    JField *fields;
    JObject class_fields;
    ui16 methods_count;
    JMethod *methods;
} _JClass;
typedef _JClass* JClass;

typedef struct {
    ui16 name_idx, type_idx;
} JNameAndTypeInfo;

typedef struct {
    ui16 class_info_idx, nat_idx;
} JMethodAndFieldInfo;

typedef struct {
    ui32 length;
    ui08 *data;
} JStringUtf;

void tty_clear( );
char tty_get_color( );
void tty_set_color( char c );
void tty_puts( const char* s );
void tty_putchar( char c );
void tty_put_i32( int v );
void tty_put_i64( int v );
unsigned char inportb( unsigned short port );
void outportb( unsigned short port, unsigned char value );

JClass jc_get_super( JClass );
char* jc_get_name( JClass c );

void vthrow( const char* fmt, va_list args ) {
    vprintf( fmt, args );
    while ( true )
        __asm( "hlt" );
}

void throw ( const char *msg, ...) {
    va_list args;
    va_start( args, msg );
    vthrow( msg, args );
    va_end( args );
}

void assert( bool cond, const char *msg, ... ) {
    if ( !cond ) {
        tty_puts( "ASS!" );
        va_list args;
        va_start( args, msg );
        vthrow( msg, args );
        va_end( args );
    }
}

void jcr_init( ) {
    EVOID.type = VOID;
    EVOID.value = 0;
}

JRObjRef jr_invoke( JMethod m, JRObjRef *args );
#endif	/* _JCORE_H */

