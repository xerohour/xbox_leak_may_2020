#ifndef _ODASSERT_
#define _ODASSERT_

#if DBG

#define assert(exp) { \
    if (!(exp)) { \
        LBAssert( #exp, __FILE__, __LINE__); \
    } \
}

#else

#define assert(exp)

#endif /* DBG */


#endif /* _ODASSERT_ */
