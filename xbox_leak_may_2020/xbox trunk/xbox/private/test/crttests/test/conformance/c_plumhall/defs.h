/* The Plum Hall Validation Suite for C
 * Unpublished copyright (c) 1986-1990, Chiron Systems Inc and Plum Hall Inc.
 * VERSION: 2.00
 * DATE: 90/04/05
 * The "ANSI" mode of this suite corresponds to official ANSI C, X3.159-1989.
 * As per your license agreement, your distribution is not to be moved or copied outside the Designated Site
 * without specific permission from Plum Hall Inc.
 */

#if defined(TEST_WIN32S)
#include "wintest.h"
#endif

#ifdef _M_IA64
#pragma warning( disable : 4244)
#endif

#ifndef DEFS_H

   #define DEFS_H
   #include "compil.h"

#if !ANSI || !LIB_TEST
   #include "machin.h"
#endif

/* THE REMAINING DEFINITIONS SHOULD NOT BE MODIFIED */

#define TRUE 1
#define FALSE 0

 /* Where possible, we will try to make the compiler have
 * as many registers as possible tied up.  This usually
 * forces more error-prone paths.
 */

#define REGISTERS register int r1,r2,r3,r4,r5;register char *r6,*r7,*r8,*r9,*r10
#define USEREGISTERS r1=r2=r3=r4=r5=0;r6=r7=r8=r9=r10=0

#if !VOID_OK
   #define void int
#endif

extern char *Filename; /* will always get set to the current source file */

#if HAS_PROTOTYPES
   #define PARMS(x) x
#else
   #define PARMS(X) ()
#endif  /* HAS_PROTOTYPES */

#ifdef _WIN64
int iequals PARMS((int, size_t, size_t)); /*LX:10131999 made params size_t for 64-bit friendiness */
#else
int iequals PARMS((int,int, int));
#endif
int inotequals PARMS((int, int, int));
int aequals PARMS((int, const void *, const void *));
int faequals PARMS((int, int (*_val1)(), int (*_val2)() ));
int lequals PARMS((int, long, long));
int dequals PARMS((int, double, double));
int fequals PARMS((int, double, double));
int checkthat PARMS((int, int));
int stequals PARMS((int, char*, char*));
int scheck PARMS((int, int, int, int));
int do_nothing PARMS((void *, ...));
int complain PARMS((int));

#ifdef NT
   int ldequals PARMS((int, double, double));
#else
   int ldequals PARMS((int, long double, long double));
#endif

int ivalue PARMS((int));
long lvalue PARMS((long));
double dvalue PARMS((double));
generic_ptr avalue PARMS((generic_ptr));
void report PARMS((char *));
void pr_ok PARMS((char *));
void pr_err PARMS((char *));

#ifdef _XBOX
int _cdecl DbgPrint(const char *, ...);
#define xprintf DbgPrint
#else
#define xprintf printf
#endif

#if !ANSI

#ifndef DBL_DIG
   #define DBL_DIG DIGITS_MAX
#endif

#ifndef FLT_DIG
   #define FLT_DIG FDIGITS_MAX
#endif

#ifndef LDBL_DIG
   #define LDBL_DIG DIGITS_MAX
#endif

#ifndef DBL_MAX_10_EXP
   #define DBL_MAX_10_EXP 37    /* a portable minimum value */
#endif

#ifndef DBL_MIN_10_EXP
   #define DBL_MIN_10_EXP -37   /* a portable minimum value */
#endif

#ifndef LDBL_MIN_10_EXP
   #define LDBL_MIN_10_EXP -37  /* a portable minimum value */
#endif

#endif  /* of !ANSI */

#endif  /* of DEFS_H */
