#ifndef _TYPES_H
#define	_TYPES_H

short sw16( short v ) {
    return ((v & 0xFF) << 8) | ((v & 0xFF00) >> 8);
}

int sw32( int v ) {
    return ((v & 0xFF) << 24) | ((v & 0xFF00) << 8) | ((v & 0xFF0000) >> 8) | ((v & 0xFF000000) >> 24);
}

/*long long sw64( long long v ) {
    return ((v & 0xFF) << 56)
 | ((v & 0xFF00) << 40)
 | ((v & 0xFF0000) << 24)
 | ((v & 0xFF000000) << 8)
 | ((v & 0xFF00000000LL) >> 8)
 | ((v & 0xFF0000000000LL) >> 24)
 | ((v & 0xFF000000000000LL) >> 40)
 | ((v & 0xFF00000000000000LL) >> 56);
}*/

long long sw64( long long v ) {
    return ((v & 0xFF) << 56)
 | ((v & 0xFF00) << 40)
 | ((v & 0xFF0000) << 24)
 | ((v & 0xFF000000) << 8)
 | ((v >> 8) & 0xFF000000)
 | ((v >> 24) & 0xFF0000)
 | ((v >> 40) & 0xFF00)
 | ((v >> 56) & 0xFF);
}

#endif	/* _TYPES_H */

