#define ISPOWER2( n )     ((((ULONG)(n)) & (((ULONG)(n)) - 1 )) == 0 )

#define _ISALIGN2( x, n ) ((((ULONG)(x)) & (((ULONG)(n)) - 1 )) == 0 )

#define _ROUNDUP2( x, n ) ((((ULONG)(x)) + (((ULONG)(n)) - 1 )) & ~(((ULONG)(n)) - 1 ))

#define ISPOWER2_64( n )     ((((DWORDLONG)(n)) & (((DWORDLONG)(n)) - 1 )) == 0 )

#define _ISALIGN2_64( x, n ) ((((DWORDLONG)(x)) & (((DWORDLONG)(n)) - 1 )) == 0 )

#define _ROUNDUP2_64( x, n ) ((((DWORDLONG)(x)) + (((DWORDLONG)(n)) - 1 )) & ~(((DWORDLONG)(n)) - 1 ))

#define UNREFERENCED( x ) ((x)=(x))

#define _STRINGIZE( x ) #x
#define STRINGIZE( x ) _STRINGIZE( x )

#define SWAP( type, a, b ) { type t = (a); (a) = (b); (b) = (t); }

#ifdef DEBUG
    #define ASSERT( a ) (( a ) ? 1 : Assert( #a, __FILE__, __LINE__ ))
    #define ROUNDUP2( x, n ) ( ASSERT( ISPOWER2( n )) ? _ROUNDUP2((x),(n)) : 0 )
    #define ISALIGN2( x, n ) ( ASSERT( ISPOWER2( n )) ? _ISALIGN2((x),(n)) : 0 )
    #define ROUNDUP2_64( x, n ) ( ASSERT( ISPOWER2_64( n )) ? _ROUNDUP2_64((x),(n)) : 0 )
    #define ISALIGN2_64( x, n ) ( ASSERT( ISPOWER2_64( n )) ? _ISALIGN2_64((x),(n)) : 0 )
    #define DEBUGCODE( code ) code
    #define INLINE
#else
    #define ASSERT( a )
    #define ROUNDUP2( x, n ) _ROUNDUP2((x),(n))
    #define ISALIGN2( x, n ) _ISALIGN2((x),(n))
    #define ROUNDUP2_64( x, n ) _ROUNDUP2_64((x),(n))
    #define ISALIGN2_64( x, n ) _ISALIGN2_64((x),(n))
    #define DEBUGCODE( code )
    #define INLINE __inline
#endif


#if defined(_X86_) || defined(_MIPS_) || defined(_PPC_)
    #define PAGE_SIZE   0x1000
#elif defined(_ALPHA_)
    #define PAGE_SIZE   0x2000
#else
    #error
#endif

unsigned __inline UMIN( unsigned a, unsigned b ) {
    return ( a < b ) ? a : b;
    }

unsigned __inline UMAX( unsigned a, unsigned b ) {
    return ( a > b ) ? a : b;
    }

#define MAX_NAME_LENGTH     256
#define MAX_PATH_LENGTH   65536

#define FX_BLOCKSIZE    0x10000     // 64K transfer size, buffer granularity

#ifndef STATUS_CANCELLED
#define STATUS_CANCELLED ((LONG)0xC0000120)     // from ntstatus.h
#endif


