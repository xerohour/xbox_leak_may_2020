/* Suite++:    The Plum Hall Validation Suite for C++
 * LibSuite++: The Plum Hall Validation Suite for Standard C++ Library
 * AND         The Plum Hall Validation Suite for C
 * Unpublished copyright (c) 1994, Plum Hall Inc
 * DATE: 1994-07-20
 * As per your license agreement, your distribution is not
 * to be moved or copied outside the Designated Site
 * without specific permission from Plum Hall Inc.
 */

#ifndef UTIL_INCLUDED /* util.c can be #include'd via defs.h */
#define UTIL_INCLUDED

#ifndef DEFS_H
#include "defs.h"
#endif

#ifndef FREESTANDING
#include <stdio.h>
#endif

#define BUFSIZE 256

void setzero(void);
void setremark(void);
void trace(const char *msg, int line);

long Buffer_zone = 0;
long Nerrs = 0;
long Nerr_items = 0;
long Nerrs_this_case = 0;
long Nskips = 0;
long Nremarks = 0;
long Nsuccess = 0;
long Nsuccess_items = 0;
int Debug = FALSE;
int Tall_skinny = TRUE;
int Count_cases = FALSE;
int Score_items = FALSE;
char *Filename = 0;
char last_char_printed = 0;
static char details[BUFSIZE] = {0};
static int trace_msg_len = 0;
static char trace_routine[20] = "";
static char trace_filename[BUFSIZE] = "";
static int remarks = FALSE;
static int first = 1;

#include "clib.h"

/*
 * STR_REV - reverse a string
 */
char *str_rev(
        char *s)
        {
        char c;
        char *p, *q;

        for (p = s, q = s + str_len(s) - 1; p < q; ++p, --q)
                {
                c = *p;
                *p = *q;
                *q = c;
                }
        return s;
        }

/*
 * IABS - absolute value of an integer
 */
int iabs(
        int i)
        {
        return (i >= 0) ? i : -i;
        }

/*
 * DABS - absolute value of a double
 */
double dabs(
        double d)
        {
        return (d >= 0.0) ? d : -d;
        }

/*
 * DPOWU - returns a double d raised to the power of unsigned u
 */
double dpowu(
        double d,
        unsigned u)
        {
        double p = 1.0;

        while (u-- > 0)
                p *= d;
        return p;
        }

/*
 * ULTOSTR - convert unsigned long u to a string in base b, return the
 * address of the null terminator
 */
char *ultostr(
        char *s,
        ULONG u,
        unsigned b)
        {
        char *p = s;

        do
                {
                if ((*p = (char)(u % b)) < 10)
                        *p += '0';
                else
                        *p += 'A' - 10;
                ++p;
                }
        while ((u /= b) != 0);
        *p = '\0';
        str_rev(s);
        return p;
        }

/*
 * LTOSTR - convert long i to a string in base b
 */
char *ltostr(
        char *s,
        long i,
        unsigned b)
        {
        if (i < 0)
                {
                *s++ = '-';
                i = -i;
                }
        return ultostr(s, i, b);
        }

#define utostr(s, u, b) ultostr((s), (ULONG)(u), (b))

#define itostr(s, i, b) ltostr((s), (long)(i), (b))

#define ULONG_DIG 64

/*
 * PR_UL - print unsigned long u as a string in base b
 */
void pr_ul(
        ULONG lu,
        unsigned b)
        {
        char buf[ULONG_DIG + 1];

        ultostr(buf, lu, b);
        pr_ok(buf);
        }

/*
 * PR_L - print long li as a string in base b
 */
void pr_l(
        long li,
        unsigned b)
        {
        char buf[ULONG_DIG + 2];

        ltostr(buf, li, b);
        pr_ok(buf);
        }

#define pr_i(i, b) pr_l((long)(i), (b))

/*
 * ENTRY_FN - in Debug mode, record entry to a function
 */
void entry_fn(
        const char *s)
        {
        if (Debug)
                {
                if (last_char_printed != '\n')
                        pr_ok("\n");
                pr_ok("ENTERING: ");
                pr_ok(s);
                pr_ok("\n");
                }
        }

/*
 * ERRMSG - print and tabulate each message
 */
static void errmsg(
        char *msg,
        int line)
        {
        if (first)
                {
                setzero();
                setremark();
                first = 0;
                pr_ok("***** Reached first test *****\n");
                }
        if (trace_msg_len != 0 )
                {
                if (last_char_printed != '\n')
                        pr_ok("\n");
                trace_msg_len = 0;
                str_cpye(trace_routine, "");
                }
        if (line > 0 || remarks)
                {
                pr_ok(line > 0 ? "ERROR" : "REMARK");
                pr_ok(" in ");
                pr_ok(Filename);
                pr_ok(" at line ");
                pr_i(iabs(line), 10);
                pr_ok(msg);
                pr_ok("\n");
                }
        if (line > 0)
                ++Nerr_items;
        else
                ++Nremarks;
        }

/*
 * IEQUALS - 'int' equality check.  If val1 != val2, then report an error.
 */
int iequals(
        int line,
        int val1,
        int val2)
        {
        char *p;

        if (val1 != val2)
                {
                p = str_cpye(details, ": (");
                p = itostr(p, val1, 10);
                p = str_cpye(p, ") != (");
                p = itostr(p, val2, 10);
                p = str_cpye(p, ")");
                errmsg(details, line);
                return 0;
                }
        else
                trace("iequals", line);
        return 1;
        }

/*
 * INOTEQUALS - 'int' non-equality check.  If val1 == val2, then
 * report an error.
 */
int inotequals(
        int line,
        int val1,
        int val2)
        {
        char *p;

        if (val1 == val2)
                {
                p = str_cpye(details, ": (");
                p = itostr(p, val1, 10);
                p = str_cpye(p, ") == (");
                p = itostr(p, val2, 10);
                p = str_cpye(p, ")");
                errmsg(details, line);
                return 0;
                }
        else
                trace("inotequals", line);
        return 1;
        }

/*
 * ILEQUAL - 'int' less-or-equal check.  If val1 > val2, then
 * report an error.
 */
int ilequal(
        int line,
        int val1,
        int val2)
        {
        char *p;

        if (val1 > val2)
                {
                p = str_cpye(details, ": (");
                p = itostr(p, val1, 10);
                p = str_cpye(p, ") > (");
                p = itostr(p, val2, 10);
                p = str_cpye(p, ")");
                errmsg(details, line);
                return 0;
                }
        else
                trace("ilequal", line);
        return 1;
        }

/*
 * LEQUALS - 'long' equality check.  If val1 != val2, then
 * report an error.
 */
int lequals(
        int line,
        long val1,
        long val2)
        {
        char *p;

        if (val1 != val2)
                {
                p = str_cpye(details, ": (");
                p = ltostr(p, val1, 10);
                p = str_cpye(p, ") != (");
                p = ltostr(p, val2, 10);
                p = str_cpye(p, ")");
                errmsg(details, line);
                return 0;
                }
        else
                trace("lequals", line);
        return 1;
        }

/*
 * ULEQUALS - 'unsigned long' equality check.  If val1 != val2, then
 * report an error.
 */
int ulequals(
        int line,
        ULONG val1,
        ULONG val2)
        {
        char *p;

        if (val1 != val2)
                {
                p = str_cpye(details, ": (");
                p = ultostr(p, val1, 10);
                p = str_cpye(p, ") != (");
                p = ultostr(p, val2, 10);
                p = str_cpye(p, ")");
                errmsg(details, line);
                return 0;
                }
        else
                trace("ulequals", line);
        return 1;
        }

/*
 * STEQUALS - string equality.
 */
int stequals(
        int line,
        const char *val1,
        const char *val2)
        {
        char *p;

        if (str_cmp(val1, val2))
                {
                p = str_cpye(details, ": \"");
                p = str_cpye(p, val1);
                p = str_cpye(p, "\" != \"");
                p = str_cpye(p, val2);
                p = str_cpye(p, "\"");
                errmsg(details, line);
                return 0;
                }
        else
                trace("stequals", line);
        return 1;
        }

/*
 * COMPLAIN - unconditional failure.
 */
int complain(
        int line)
        {
        errmsg("", line);
        return 0;
        }

/*
 * ARBHEX - convert an arbitrary byte-sequence into hex codes
 */
#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif
#define NIBBLES_PER_BYTE ((CHAR_BIT + 3) / 4)

char *arbhex(
        char *str,
        const char *p,
        int n)
        {
        int i, nib, hex_dig;
        static char codes[] = "0123456789ABCDEF";

        for (i = 0; i < n; ++i, ++p)
                for (nib = NIBBLES_PER_BYTE - 1; nib >= 0; --nib)
                        {
                        hex_dig = (*p & (unsigned)(0xF << (nib * 4))) >> (nib * 4);
                        *str++ = codes[hex_dig];
                        }
        *str = '\0';
        return str;
        }

/*
 * AEQUALS - 'address' equality check.  If val1 != val2, then
 * report an error.
 */
int aequals(
        int line,
        const volatile void *val1,
        const volatile void *val2)
        {
#ifdef FREESTANDING
        char *p;
#endif

        if (val1 != val2)
                {
#ifdef FREESTANDING
                p = str_cpye(details, ": (");
                p = arbhex(p, (const char *)&val1, (int)sizeof(val1));
                p = str_cpye(p, ") != (");
                p = arbhex(p, (const char *)&val2, (int)sizeof(val2));
                p = str_cpye(p, ")");
#else
                sprintf(details, ": (%p) != (%p)",  val1, val2);
#endif
                errmsg(details, line);
                return 0;
                }
        else
                trace("aequals", line);
        return 1;
        }

/*
 * FAEQUALS - function address equality check.  If val1 != val2, then
 * report an error. The address of a function is not necessarily the same
 * size/type as the address of data.
 */
int faequals(
        int line,
        int (*val1)(),
        int (*val2)())
        {
        char *p;

        if (val1 != val2)
                {
                p = str_cpye(details, ": (");
                p = arbhex(p, (char *)&val1, (int)sizeof(val1));
                p = str_cpye(p, ") != (");
                p = arbhex(p, (char *)&val2, (int)sizeof(val2));
                p = str_cpye(p, ")");
                errmsg(details, line);
                return 0;
                }
        else
                trace("faequals", line);
        return 1;
        }

/*
 * DEQUALS - 'double' equality check.  If val1 != val2, then
 * report an error. This is computed using an equality approximation
 * that verifies that the two numbers are equal to R digits whenever
 *
 *    |x - y|    1    1-R
 *    ------- <= - 10
 *      |x|      2
 *
 * DIGITS_MAX  is defined in defs.h
 */
double Delta = 0.0;

int dequals(
        int line,
        double val1,
        double val2)
        {
#ifdef FREESTANDING
        char *p;
#endif
        double *pd;

        if (line < 0)   /* no "remarks" for floating point */
                return 0;
        if (Delta == 0.0)
                Delta = 0.5 / dpowu(10.0, DIGITS_MAX - 1);
        if (val1 == val2)
                {
                trace("dequals", line);
                return 1;
                }
        pd = &val1;
        if (val1 == 0.0)
                pd = &val2;

        /* special cases to handle zero against very small numbers */
        if (dabs(val1) == 0.0 && dabs(val2) < Delta)
                ;
        else if (dabs(val2) == 0.0 && dabs(val1) < Delta)
                ;
        else if ((dabs(val1 - val2) / dabs(*pd)) > Delta)
                {
#ifdef FREESTANDING
                p = str_cpye(details, ": (");
                p = arbhex(p, (const char *)&val1, (int)sizeof(val1));
                p = str_cpye(p, ") != (");
                p = arbhex(p, (const char *)&val2, (int)sizeof(val2));
                p = str_cpye(p, ")");
#else
                sprintf(details, ": (%.*G) != (%.*G)",
                        DIGITS_MAX + 2, val1, DIGITS_MAX + 2, val2);
#endif
                errmsg(details, line);
                return 0;
                }
        trace("dequals", line);
        return 1;
        }

#if ANSI
/*
 * LDEQUALS - Long double equality ... more of the same.
 */
#define ldabs(ld) ((ld) < 0.0 ? -(ld) : (ld))

long double LDelta = 0.0;

int ldequals(
        int line,
        long double val1,
        long double val2)
        {
#ifdef FREESTANDING
        char *p;
#endif
        long double *pd;

        if (line < 0)   /* no "remarks" for floating point */
                return 0;
        if (LDelta == 0.0)
                {
                LDelta = 0.5L / dpowu(10.0, LDIGITS_MAX - 1);
                }
        if (val1 == val2)
                {
                trace("ldequals", line);
                return 1;
                }
        pd = &val1;
        if (val1 == 0.0)
                pd = &val2;

        /* special cases to handle zero against very small numbers */
        if (ldabs(val1) == 0.0 && ldabs(val2) < LDelta)
                ;
        else if (ldabs(val2) == 0.0 && ldabs(val1) < LDelta)
                ;
        else if ((ldabs(val1 - val2) / ldabs(*pd)) > LDelta)
                {
#ifdef FREESTANDING
                p = str_cpye(details, ": (");
                p = arbhex(p, (char *)&val1, (int)sizeof(val1));
                p = str_cpye(p, ") != (");
                p = arbhex(p, (char *)&val2, (int)sizeof(val2));
                p = str_cpye(p, ")");
#else
                sprintf(details, ": (%.*LE) != (%.*LE)",
                        LDIGITS_MAX + 2, val1, LDIGITS_MAX + 2, val2);
#endif
                errmsg(details, line);
                return 0;
                }
        trace("ldequals", line);
        return 1;
        }
#endif

/*
 * FEQUALS - same as DEQUALS, but to FDIGITS_MAX instead of DIGITS_MAX.
 */
double FDelta = 0.0;

int fequals(
        int line,
        double in1,
        double in2)
        {
#ifdef FREESTANDING
        char *p;
#endif
        float *pf;
        float val1 = (float)in1;
        float val2 = (float)in2;

        if (line < 0)   /* no "remarks" for floating point */
                return 0;
        if (FDelta == 0.0)
                FDelta = 0.5 / dpowu(10.0, FDIGITS_MAX - 1);
        if (val1 == val2)
                {
                trace("fequals", line);
                return 1;
                }
        pf = &val1;
        if (val1 == 0.0)
                pf = &val2;

        /* special cases to handle zero against very small numbers */
        if (dabs(val1) == 0.0 && dabs(val2) < FDelta)
                ;
        else if (dabs(val2) == 0.0 && dabs(val1) < FDelta)
                ;
        else if ((dabs(val1 - val2) / dabs(*pf)) > FDelta)
                {
#ifdef FREESTANDING
                p = str_cpye(details, ": (");
                p = arbhex(p, (char *)&val1, (int)sizeof(val1));
                p = str_cpye(p, ") != (");
                p = arbhex(p, (char *)&val2, (int)sizeof(val2));
                p = str_cpye(p, ")");
#else
                sprintf(details, ": (%.*G) != (%.*G)",
                        FDIGITS_MAX+2, val1, FDIGITS_MAX+2, val2);
#endif
                errmsg(details, line);
                return 0;
                }
        trace("fequals", line);
        return 1;
        }

/*
 * CHECKTHAT - simple condition check.  If val1 == 0, then
 * report an error.
 */
int checkthat(
        int line,
        int cond)
        {
        if (!cond)
                {
                errmsg("", line);
                return 0;
                }
        else
                trace("checkthat", line);
        return 1;
        }

/*
 * VALUE - the value routines are used to defeat value propagation in
 * optimizing compilers.  We want to make sure that we are testing what we
 * think we are testing, not what the compiler transformed it to.
 * 1988: Some compilers "open-code" all small functions.  Now we have to hide
 * the constants still further.
 */
extern int Zero = 0;    /* See  setzero()  below */

int ivalue(
        int i)
        {
        return i + Zero;
        }

long lvalue(
        long i)
        {
        return i + Zero;
        }

double dvalue(
        double i)
        {
        return i + Zero;
        }

float fvalue(
        double i)
        {
        return (float)i + Zero;
        }

void * avalue(
        void * i)
        {
        if (i == 0)
                return 0;
        else
                return (char *)i + Zero;
        }

/*
 * SCHECK - check both values and side effects.
 */
int Side = 0;

int scheck(
        int line,
        int val1,
        int se,
        int val2)
        {
        int status = 1;

        if (Side != se)
                {
                errmsg(": incorrect side effect", line);
                status = 0;
                }
        else
                trace("scheck", line);
        Side = 0;
        return status == 1 && iequals(line, val1, val2);
        }

/*
 * DO_NOTHING - this is also intended to defeat optimizers by passing
 * the addresses of variables for which we want to stop any value propagation.
 */
int do_nothing(
        const volatile void *p, ...)
        {
        if (p != 0)
                *(char *)p = *((char *)p + Zero);
        return 0;
        }

/*
 * REPORT - summary report at end of testing.
 */
int report(
        const char *program)
        {
        if (trace_msg_len != 0 || last_char_printed != '\n')
                pr_ok("\n");

        if (Score_items || !Count_cases)
                {
                Nsuccess = Nsuccess_items;
                Nerrs = Nerr_items;
                }
        else
                {
                pr_ok("***** ");
                pr_l(Nsuccess_items, 10);
                pr_ok(" individual successful item");
                pr_ok(Nsuccess_items != 1 ? "s" : "");
                pr_ok(" in ");
                pr_ok(program);
                pr_ok(" *****\n");
                }

        pr_ok("***** ");
        pr_l(Nsuccess, 10);
        pr_ok(" successful test case");
        pr_ok(Nsuccess != 1 ? "s" : "");
        pr_ok(" in ");
        pr_ok(program);
        pr_ok(" *****\n");

        pr_ok("***** ");
        pr_l(Nerrs, 10);
        pr_ok(" error");
        pr_ok(Nerrs != 1 ? "s" : "");
        pr_ok(" detected in ");
        pr_ok(program);
        pr_ok(" *****\n");

        pr_ok("***** ");
        pr_l(Nskips, 10);
        pr_ok(" skipped section");
        pr_ok(Nskips != 1 ? "s" : "");
        pr_ok(" in ");
        pr_ok(program);
        pr_ok(" *****\n");

        if (remarks && Nremarks != 0)
                {
                pr_ok("***** ");
                pr_l(Nremarks, 10);
                pr_ok(" remark");
                pr_ok(Nremarks != 1 ? "s" : "");
                pr_ok(" detected in ");
                pr_ok(program);
                pr_ok(" *****\n");
                }

        if (Buffer_zone != 0)
                {
                pr_ok("WARNING: Data over-write (wild store) occurred\n");
                }

//!: (don't want skip tests to register as errors)        Nerrs += Nskips;
        return Nerrs > MAX_INT ? MAX_INT : (int)Nerrs;
        }

/*
 * DBPRINT - print the message if the Debug flag is on.
 */
void dbprint(
        const char *s)
        {
        if (Debug)
                {
                pr_ok("***DEBUG***");
                pr_ok(s);
                }
        }

/*
 * TRACE - print a line-number trace for debugging
 * Also count successful tests.
 */
void trace(
        const char *routine,
        int line)
        {
        char buf[10];

        if (first)
                {
                setzero();
                setremark();
                first = 0;
                pr_ok("***** Reached first test *****\n");
                }
        if (Debug && line > 0)
                {
                if (str_cmp(trace_routine, routine) != 0 ||
                        str_cmp(trace_filename, Filename) != 0 ||
                        last_char_printed == '\n' ||
                        Tall_skinny ||
                        trace_msg_len > 60)
                        {
                        if (last_char_printed != '\n')
                                pr_ok("\n");
                        pr_ok("TRACE: ");
                        pr_ok(routine);
                        pr_ok(" at ");
                        pr_ok(Filename);
                        pr_ok(" ");
                        str_cpye(trace_routine, routine);
                        str_cpye(trace_filename, Filename);
                        trace_msg_len = 11 + str_len(routine) + str_len(Filename);
                        }
                str_cpye(itostr(buf, iabs(line), 10), " ");
                pr_ok(buf);
                trace_msg_len += str_len(buf);
                }
        if (line > 0)
                ++Nsuccess_items; /* don't count remarks in score */
        }

/*
 * PR_SKIP - printf a text line in SKIP situation
 */
void pr_skip(
        const char *s)
        {
        ++Nskips;
        if (last_char_printed != '\n')
                pr_ok("\n");
        pr_ok("#SKIPPED: ");
        pr_ok(s);
        pr_ok("\n");
        }

/*
 * BEGIN_CASE - start a new test case
 */
void begin_case(
        const char *s)
        {
        Nerrs_this_case = Nerr_items;
        ignore(&s);
        }
/*
 * END_CASE - complete a test case
 */
void end_case(
        const char *s)
        {
        int i;
        static char buf[2] = {0};
        Count_cases = TRUE;
        if (last_char_printed != '\n')
                pr_ok("\n");
        if (Nerrs_this_case == Nerr_items)
                {
                ++Nsuccess;
                pr_ok("#PASSED:  ");  /* no new errors */
                for (i = 0; s[i] != ' ' && s[i] != '\0'; ++i)
                        buf[0] = s[i], pr_ok(buf);
                }
        else
                {
                static char m[] = "FAIL";
                ++Nerrs;
                pr_ok("#FAILED:  ");  /* new errors */
                for (i = 0; s[i] != ' ' && s[i] != '\0'; ++i)
                        buf[0] = s[i], pr_ok(buf);
                ++i;
                pr_ok(" ");
                if (s[i+2]!=m[0]||s[i+3]!=m[1]||s[i+4]!=m[2]||s[i+5]!=m[3])
                        pr_ok(s+i);
                }
        pr_ok("\n");
        }

/*
 * PR_ERR - printf a text line in error situation
 */
void pr_err(
        const char *s)
        {
        ++Nerr_items;
        pr_ok(s);
        }
/*
 * SETREMARK - determine whether "remark" messages should be printed
 */
void setremark()
        {
#ifdef SUITE_REMARK
        remarks = TRUE;
#else
        remarks = FALSE;
#endif
        }

/*
 * ignore - appear to use a value, but don't
 * (Purpose: shut up noisy "value not used" messages)
 */
void ignore(
        const volatile void *addr)
        {
        if (Zero)
                pr_ok((char *)addr);
        }

/* new utility functions for widechar library tests */

/*
 * WCS_CMP
 */
int wcs_cmp(
        const wchar_t *s1,
        const wchar_t *s2)
        {
        for (; *s1 == *s2; ++s1, ++s2)
                if (*s1 == 0)
                        return 0;
        if (*s1 < *s2)
                return -1;
        else 
                return 1;
        }

/*
 * WCS_MBSE - copy widechar string s2 to (narrow) string s1
 *    (not an exact replacement for wcstombs)
 */
char *wcs_mbse(
        char *s1,
        const wchar_t *s2)
        {
        do      {
                if ((char)*s2 == *s2)
                        *s1++ = (char)*s2++;
                else
                        *s1++ = '?', ++s2;
                } while ((*s1) != '\0');
        return s1;
        }

/*
 * WCS_CPYE - copy widechar string s2 to s1
 *    (not an exact replacement for wcscpy)
 */
wchar_t *wcs_cpye(
        wchar_t *s1,
        const wchar_t *s2)
        {
        while ((*s1 = *s2++) != 0)
                ++s1;
        return s1;
        }

/*
 * WCSEQUALS - widechar string equality.
 */
int wcsequals(
        int line,
        const wchar_t *val1,
        const wchar_t *val2)
        {
        char *p;

        if (wcs_cmp(val1, val2))
                {
                p = str_cpye(details, ": \"");
                p = wcs_mbse(p, val1);
                p = str_cpye(p, "\" != \"");
                p = wcs_mbse(p, val2);
                p = str_cpye(p, "\"");
                errmsg(details, line);
                return 0;
                }
        else
                trace("wcsequals", line);
        return 1;
        }

/*
 * PR_CHKN - print a NOTE with value of n
 */
int pr_chkn(int n)
        {
        pr_ok("NOTE: The actual value was: ");
        pr_l((long)n, 10);
        pr_ok("\n");
        return 0;   /* presumably called only in ERROR situations */
        }

#include "sdutil.h"

#endif /* UTIL_INCLUDED */
