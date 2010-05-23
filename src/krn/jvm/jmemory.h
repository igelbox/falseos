/* 
 * File:   jmemory.h
 * Author: igel
 *
 * Created on 24 Апрель 2010 г., 22:20
 */

#ifndef _JMEMORY_H
#define	_JMEMORY_H

void jmm_init( );

void* jmm_alloc( int size );

unsigned int jmm_avail( );

unsigned int jmm_max( );

unsigned int jmm_used( );

#endif	/* _JMEMORY_H */

