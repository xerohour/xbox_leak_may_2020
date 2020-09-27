/*
 * Our development environment does not have the ANSI C remove
 * library routine defined. Therefore this header file defines
 * remove to be unlink; this makes the test Unix dependent.
 *
 * When compiling with ANSI C libraries this may be commented out.
 */

//#include <osfcn.h>

#if 0

//ANSI to NON-ANSI function definitions:
#ifndef remove
#define	remove(x) unlink(x)
#endif

#else

//NON-ANSI to ANSI function definitions:
#ifndef open
#define	open _open
#endif

#endif
