/* The Plum Hall Validation Suite for C
 * Unpublished copyright (c) 1986-1990, Chiron Systems Inc and Plum Hall Inc.
 * VERSION: 2.00
 * DATE: 90/04/05
 * The "ANSI" mode of this suite corresponds to official ANSI C, X3.159-1989.
 * As per your license agreement, your distribution is not to be moved or copied outside the Designated Site
 * without specific permission from Plum Hall Inc.
 */

/*
 * UTILS - various utility routines.
 */
#include <stdio.h>
#include <math.h>
#include "defs.h"
//!mmalone: #ifdef __STDC__
#include <stdlib.h>
#include <string.h>
// #endif

void setzero();
void setremark();
void trace(char * routine, int line);
int Nerrs = 0;
int Nremarks = 0;
long Nsuccess = 0;
int Debug = FALSE;
char *Filename = NULL;
static char details[BUFSIZ] = {0};
static size_t trace_msg_len = 0; //LX:10131999 /int/size_t for 64-bit friendliness
static char trace_routine[20] = "";
static char trace_filename[BUFSIZ] = "";
static int remarks = FALSE;

/*
 * ERRMSG - print and tabulate each message
 */
static void errmsg(char * msg, int line)
    {
    if (trace_msg_len != 0)
        {
        xprintf("\n");
        trace_msg_len = 0;
        strcpy(trace_routine, "");
        }
    if (line > 0 || remarks)
        xprintf("%s in %s at line %d%s\n",
            line > 0 ? "ERROR" : "REMARK",
            Filename,
            line > 0 ? line : -line, msg);
    fflush(stdout);
    if (line > 0)
        ++Nerrs;
    else
        ++Nremarks;
    }

/*
 * IEQUALS - 'int' quality check.  If val1 != val2, then report an error.
 */
int iequals(
    int line,
#ifdef _M_IA64
    size_t val1, size_t val2 /*LX:10131999 made vals size_t for 64-bit friendliness*/
#else
    int val1, int val2
#endif
    )
    {
    if (val1 != val2)
        {
        sprintf(details, ": (%d) != (%d)",  val1, val2);
        errmsg(details, line);
        return (0);
        }
    else
        trace("iequals", line);
    return (1);
    }

/*
 * INOTEQUALS - 'int' non-equality check.  If val1 == val2, then
 * report an error.
 */
int inotequals(int line, int val1, int val2)
    {
    if (val1 == val2)
        {
        sprintf(details, ": (%d) == (%d)",  val1, val2);
        errmsg(details, line);
        return (0);
        }
    else
        trace("inotequals", line);
    return (1);
    }

/*
 * LEQUALS - 'long' quality check.  If val1 != val2, then
 * report an error.
 */
int lequals(int line, long val1, long val2)
    {
    if (val1 != val2)
        {
        sprintf(details, ": (%ld) != (%ld)",  val1, val2);
        errmsg(details, line);
        return (0);
        }
    else
        trace("lequals", line);
    return (1);
    }

/*
 * COMPLAIN - unconditional failure.
 */
int complain(int line)
    {
    errmsg("", line);
    return 0;
    }

/*
 * STEQUALS - string equality.
 */
int stequals(int line, char * val1, char * val2)
    {
    if (strcmp(val1, val2))
        {
        sprintf(details, ": \"%s\" != \"%s\"",  val1, val2);
        errmsg(details, line);
        return (0);
        }
    else
        trace("stequals", line);
    return (1);
    }

/*
 * AEQUALS - 'address' equality check.  If val1 != val2, then
 * report an error.
 */
int aequals( int line,
#if ANSI
    const void * val1, const void * val2
#else
    char * val1, char * val2
#endif
     )
    {
    if (val1 != val2)
        {
#if ANSI
        sprintf(details, ": (%p) != (%p)",  val1, val2);
#else
        sprintf(details, ": (%lx) != (%lx)",  (long)val1, (long)val2);
#endif
        errmsg(details, line);
        return (0);
        }
    else
        trace("aequals", line);
    return (1);
    }

/*
 * ARBHEX - convert an arbitrary byte-sequence into hex codes
 */
#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif
#define NIBBLES_PER_BYTE ((CHAR_BIT + 3) / 4)
char *arbhex(char * str, char * p, int n)
    /*char *str;   where to store the target string */
    /*char *p;     where to find the source bytes */
    /*int n;       how many bytes */
    {
    int i, nib, hex_dig;
    static char codes[] = "0123456789ABCDEF";

    for (i = 0; i < n; ++i, ++p)
        for (nib = NIBBLES_PER_BYTE - 1; nib >= 0; --nib)
            {
            hex_dig = (*p & (unsigned int)(0xF << (nib*4))) >> (nib*4);
            *str++ = codes[hex_dig];
            }
    *str = '\0';
    return str;
    }

/*
 * FAEQUALS - function address equality check.  If val1 != val2, then
 * report an error. The address of a function is not necessarily the same
 * size/type as the address of data.
 */
int faequals(int line, int (*val1)() , int (*val2)() )
    /*int (*val1)(), (*val2)();
    int line;*/
    {
    char buf1[sizeof(val1)*NIBBLES_PER_BYTE + 1];
    char buf2[sizeof(val2)*NIBBLES_PER_BYTE + 1];

    if (val1 != val2)
        {
        arbhex(buf1, (char *)&val1, (int)sizeof(val1));
        arbhex(buf2, (char *)&val2, (int)sizeof(val2));
        sprintf(details, ": (%s) != (%s)", buf1, buf2);
        errmsg(details, line);
        return (0);
        }
    else
        trace("faequals", line);
    return (1);
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
int dequals(int line, double val1, double val2)
    /*double val1, val2;
    int line;*/
    {
    double *pd;

    if (Delta == 0.0)
        Delta = 0.5 / pow(10.0, DIGITS_MAX-1.0);
    if (val1 == val2)
        {
        trace("dequals", line);
        return (1);
        }
    pd = &val1;
    if (val1 == 0.0)
        pd = &val2;

    /* special cases to handle zero against very small numbers */
    if (fabs(val1) == 0.0 && fabs(val2) < Delta)
        ;
    else if (fabs(val2) == 0.0 && fabs(val1) < Delta)
        ;
    else if ((fabs(val1 - val2) / fabs(*pd)) > Delta)
        {
        sprintf(details, ": (%.*G) != (%.*G)",
            DIGITS_MAX+2, val1, DIGITS_MAX+2, val2);
        errmsg(details, line);
        return (0);
        }
    trace("dequals", line);
    return (1);
    }

#if ANSI
/*
 * LDEQUALS - Long double equality ... more of the same.
 */
#define ldabs(ld) ((ld) < 0.0 ? -(ld) : (ld))
long double LDelta = 0.0;
int ldequals(int line, long double val1, long double val2)
    /*long double val1, val2;
    int line;*/
    {
    long double *pd;

    if (LDelta == 0.0)
        {
        LDelta = 0.5L / pow(10.0, LDIGITS_MAX-1.0);
        }
    if (val1 == val2)
        {
        trace("ldequals", line);
        return (1);
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
        sprintf(details, ": (%.*LE) != (%.*LE)",
            LDIGITS_MAX+2, val1, LDIGITS_MAX+2, val2);
        errmsg(details, line);
        return (0);
        }
    trace("ldequals", line);
    return (1);
    }
#endif

/*
 * FEQUALS - same as DEQUALS, but to FDIGITS_MAX instead of DIGITS_MAX.
 */
double FDelta = 0.0;
int fequals(int line, double in1, double in2)
    /*float in1, in2;
    int line;*/
    {
    float *pf;
    float val1 = (float)in1;
    float val2 = (float)in2;

    if (FDelta == 0.0)
        FDelta = 0.5 / pow(10.0, FDIGITS_MAX-1.0);
    if (val1 == val2)
        {
        trace("fequals", line);
        return (1);
        }
    pf = &val1;
    if (val1 == 0.0)
        pf = &val2;

    /* special cases to handle zero against very small numbers */
    if (fabs(val1) == 0.0 && fabs(val2) < FDelta)
        ;
    else if (fabs(val2) == 0.0 && fabs(val1) < FDelta)
        ;
    else if ((fabs(val1 - val2) / fabs(*pf)) > FDelta)
        {
        sprintf(details, ": (%.*G) != (%.*G)",
            FDIGITS_MAX+2, val1, FDIGITS_MAX+2, val2);
        errmsg(details, line);
        return (0);
        }
    trace("fequals", line);
    return (1);
    }

/*
 * CHECKTHAT - simple condition check.  If val1 == 0, then
 * report an error.
 */
int checkthat(int line, int cond)
    /*int cond;
    int line;*/
    {
    if (!cond)
        {
        errmsg("", line);
        return (0);
        }
    else
        trace("checkthat", line);
    return (1);
    }

/*
 * VALUE - the value routines are used to defeat value propagation in optimizing compilers.
 * We want to make sure that we are testing what we think we are testing, not what the compiler transformed it to.
 * 1988: Some compilers "open-code" all small functions.  Now we have to hide the constants still further.
 */
static int Zero = 0;    /* See  setzero()  below */
int ivalue(int i)
    /*int i;*/
    {
    return i + Zero;
    }
long lvalue(long i)
    /*long i;*/
    {
    return i + Zero;
    }
double dvalue(double i)
    /*double i;*/
    {
    return i + Zero;
    }
float fvalue( float i)
    /*float i;*/
    {
    return (float)(i + Zero);
    }
generic_ptr avalue(generic_ptr i)
    /*generic_ptr i;*/
    {
    return (char *)i + Zero;
    }

/*
 * SCHECK - check both values and side effects.
 */
int Side = 0;
int scheck(int line, int val1, int se, int val2)
    /*int val1, se, val2, line;*/
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
    return (status == 1 && iequals(line, val1, val2));
    }

/*
 * DO_NOTHING - this is also intended to defeat optimizers by passing
 * the addresses of variables for which we want to stop any value propagation.
 */
#if ANSI
#if NEW_STYLE_FN_DEF
int do_nothing(void *p, ...) { *(char *)p = *((char *)p + Zero); return 0; }
#else
int do_nothing(void * p)  { *(char *)p = *((char *)p + Zero); return 0; }
#endif
#else
int do_nothing(){ return 0; }
#endif

/*
 * REPORT - summary report at end of testing.
 */
void report(char * program)
    /*char *program;*/
    {
    if (trace_msg_len != 0)
        xprintf("\n");
    xprintf("***** %ld successful test%s in %s *****\n", Nsuccess,
        (Nsuccess != 1) ? "s" : "", program);
    xprintf("***** %d error%s detected in %s *****\n", Nerrs,
        (Nerrs != 1) ? "s" : "", program);
    if (remarks && Nremarks != 0)
        xprintf("***** %d remark%s detected in %s *****\n", Nremarks,
            (Nremarks != 1) ? "s" : "", program);
#ifndef _XBOX
    exit(Nerrs);
#else
    Nsuccess = 0;
    Nerrs = 0;
    Nremarks = 0;
#endif
    }

/*
 * DBPRINT - print the message if the Debug flag is on.
 */
void dbprint(char * s)
    /*char *s;*/
    {
    if (Debug)
        {
        printf("***DEBUG***");
        printf(s);
        }
    }

/*
 * TRACE - print a line-number trace for debugging
 * Also count successful tests.
 */
void trace( char * routine, int line)
    /*char *routine;
    int line;*/
    {
    char buf[10];
    static int first = 1;

    if (first)
        {
        setzero();
        setremark();
        first = 0;
        xprintf("***** Reached first test *****\n");
        }
    if (Debug)
        {
        if (strcmp(trace_routine, routine) != 0 ||
            strcmp(trace_filename, Filename) != 0 ||
            trace_msg_len > 60)
            {
            xprintf("\nTRACE: %s at %s ", routine, Filename);
            strcpy(trace_routine, routine);
            strcpy(trace_filename, Filename);
            trace_msg_len = 11 + strlen(routine) + strlen(Filename);
            }
        sprintf(buf, "%d ", abs(line));
        xprintf("%s", buf);
        trace_msg_len += strlen(buf);
        fflush(stdout);
        }
    if (line > 0)   /* don't count successful warnings, for certifying */
        ++Nsuccess;
    }

/*
 * DIGITS - calculate the number of digits after the decimal
 * place so that the number is printed to DIGITS_MAX significant
 * digits.
 */
int digits( double d )
    /*double d;*/
    {
    int i;

    if (d == 0.0)
        i = 0;
    else
        i = (int)log10(fabs(d));
    return(DIGITS_MAX - (i < 0 ? 0 : i+1));
    }

int fdigits( double d)
    /*double d;*/
    {
    int i;

    if (d == 0.0)
        i = 0;
    else
        i = (int)log10(fabs(d));
    return(FDIGITS_MAX - (i < 0 ? 0 : i+1));
    }

/*
 * SETZERO - appear, to a compiler, as though Zero is unpredictably set
 */
void setzero()
    {
    FILE *fp;
    int save;

    save = errno;
    fp = fopen("nonexistent", "r");
    if (fp != 0)
        {
        fscanf(fp, "ignore format", &Zero);
        fclose(fp);
        }
    errno = save;
    }

/*
 * PR_OK - printf a text line in ok situation (no error, just text)
 */
void pr_ok(char * s)
    /*char *s;*/
    {
    fputs(s, stdout);
    }

/*
 * PR_ERR - printf a text line in error situation (such as "SKIP'ed")
 */
void pr_err(char * s)
    /*char *s;*/
    {
    ++Nerrs;
    pr_ok(s);
    }

/*
 * SETREMARK - determine whether "remark" messages should be printed
 */
void setremark()
    {
#ifndef _XBOX
    FILE *fp;

    if (getenv("SUITE_REMARK") != 0)
        remarks = TRUE;
    else if ((fp = fopen("REMARK", "r")) == 0)
        remarks = FALSE;
    else
        {
        remarks = TRUE;
        fclose(fp);
        }
#else
    //
    // BUGBUG: how do we set remark under Xbox?
    //
#endif
    }

