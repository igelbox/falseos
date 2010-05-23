#include "jvm/jclass.h"
#include "jvm/jruntime.h"
#include "jvm/jruntime.h"
#include "jvm/rt/jrt.h"
#include "jvm/rt/jrt_base.h"

JMethod handler, handler_fd, handler_fd_t;

void kjvm_start( ) {
    jmm_init( );
    jcr_init( );
    jrt_init( );
    jrt_base_init( );
    jcf_init( "." );
    JClass c = jcf_load_class( "fos/drv/chr/Tty" );
    handler = jc_find_method( c, "handle", "()V", false );
    assert( handler != NULL, "handler method not found for class fos/drv/chr/Tty" );

    c = jcf_load_class( "fos/drv/blk/Floppy" );
    handler_fd = jc_find_method( c, "handler", "()V", false );
    assert( handler_fd != NULL, "handler method not found for class fos/drv/blk/Floppy" );
    handler_fd_t = jc_find_method( c, "handlert", "()V", false );
    assert( handler_fd_t != NULL, "handlert method not found for class fos/drv/blk/Floppy" );
    //    JMethod m = jc_find_method( c, "init", "()V", false );
    //    jr_invoke( m, NULL );

    c = jcf_load_class( "apps/hello" );
    JMethod m = jc_main_method( c );
    assert( m != NULL, "main method not found for class class apps/hello" );
    jr_invoke( m, NULL );
}

void kjvm_handle_kdb( ) {
    if ( handler )
        jr_invoke( handler, NULL );
}

void kjvm_fd_handle_t( ) {
    if ( handler_fd_t )
        jr_invoke( handler_fd_t, 0 );
}

void kjvm_fd_handle( ) {
    if ( handler_fd )
        jr_invoke( handler_fd, 0 );
}
