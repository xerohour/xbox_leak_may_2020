/* The Plum Hall Validation Suite for C
 * Unpublished copyright (c) 1986-1988, Chiron Systems Inc and Plum Hall Inc.
 * VERSION: 1.09
 * DATE: 88/09/20
 * Each distribution of the Suite is unique in distinct but inconsequential ways.
 * As per your license agreement, your distribution is not to be moved or copied outside the Designated Site
 * without specific permission from Plum Hall Inc.
 */
/*
 * this source must be compiled with FUNCTION=d4_# defined where # is a
 * number between 1 and 12
 */

/*
 * LIB - standard library tests
 */

/* 14-Dec-1989 mrw added this to prevent unresolved external from d41a.c */
int errno_init = 0;  /* set in lib.c */

#define LIB_TEST 1      /* prevents inclusion of limits.h and float.h */
#include "defs.h"

extern void d4_1();

d41_main(int argc, char ** argv)
    {
    extern int Debug;

    if (argc > 1)
        Debug = TRUE;
    Filename = __FILE__;
    d4_1();
    report("d41");
    }
/*
 * For "almost-ANSI" compilers that are missing parts of the latest ANSI library,
 * include stubs for these functions here.
 */
#if (ANSI && !LIB_NOV_86)
#include "stubs.c"
#endif
