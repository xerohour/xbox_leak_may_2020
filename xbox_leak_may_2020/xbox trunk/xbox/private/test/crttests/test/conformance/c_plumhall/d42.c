/* The Plum Hall Validation Suite for C
 * Unpublished copyright (c) 1986-1990, Chiron Systems Inc and Plum Hall Inc.
 * VERSION: 2.00
 * DATE: 90/04/05
 * The "ANSI" mode of this suite corresponds to official ANSI C, X3.159-1989.
 * As per your license agreement, your distribution is not to be moved or copied outside the Designated Site
 * without specific permission from Plum Hall Inc.
 */

#ifndef SKIP42
#define LIB_TEST 1
/*
 * 4.2 - Diagnostics <assert.h>
 */
#include "defs.h"
#undef NDEBUG
#include <assert.h>
#if !ANSI
#include <stdio.h>
#endif
void d4_2();
static void d4_2_1();
#if ANSI
void d4_2_1b();
#endif

/* NDEBUG is not defined by <assert.h>. Force an error if it is */
#if defined(NDEBUG)
    complain(__LINE__);
#endif

void d4_2()
    {
    d4_2_1();
    }
















/*
 * 4.2.1 - Program Diagnostics
 * Since assert() must abort when the argument is FALSE, we can't
 * test that here. We can, however, test that NDEBUG disables it.
 */
static void d4_2_1()
    {
    int i = ivalue(1);
    int j = ivalue(0);

    /* 4.2.1.1 assert */
#if ANSI8712
    /* assert is an expression */
    assert(i == 1), assert(j == 0);
#else
    assert(i == 1);
#endif
#if ANSI
    d4_2_1b();
#endif
    }

#else /* if SKIP42 */
void d4_2() { pr_err("d4_2: SKIPPED ENTIRELY\n"); }
#endif
