#include "cstack.h"

//****************************************************************************
// globals

char            test[]      = "CallStack";

char            scharmax    = SCHAR_MAX;
char            scharmin    = SCHAR_MIN;
unsigned char   ucharmax    = UCHAR_MAX;
unsigned char   ucharmin    = 0;
int             intmax      = INT_MAX;
int             intmin      = INT_MIN;
unsigned int    uintmax     = UINT_MAX;
unsigned int    uintmin     = 0;
short           shrtmax     = SHRT_MAX;
short           shrtmin     = SHRT_MIN;
unsigned short  ushrtmax    = USHRT_MAX;
unsigned short  ushrtmin    = 0;
long            longmax     = LONG_MAX;
long            longmin     = LONG_MIN;
unsigned long   ulongmax    = ULONG_MAX;
unsigned long   ulongmin    = 0;
double          dblmax      = DBL_MAX;
double          dblmin      = DBL_MIN;
long double     ldblmax     = LDBL_MAX;
long double     ldblmin     = LDBL_MIN;
float           fltmax      = FLT_MAX;
float           fltmin      = FLT_MIN;

char            *pscharmax  = &scharmax;
char            *pscharmin  = &scharmin;
unsigned char   *pucharmax  = &ucharmax;
unsigned char   *pucharmin  = &ucharmin;
int             *pintmax    = &intmax;
int             *pintmin    = &intmin;
unsigned int    *puintmax   = &uintmax;
unsigned int    *puintmin   = &uintmin;
short           *pshrtmax   = &shrtmax;
short           *pshrtmin   = &shrtmin;
unsigned short  *pushrtmax  = &ushrtmax;
unsigned short  *pushrtmin  = &ushrtmin;
long            *plongmax   = &longmax;
long            *plongmin   = &longmin;
unsigned long   *pulongmax  = &ulongmax;
unsigned long   *pulongmin  = &ulongmin;
double          *pdblmax    = &dblmax;
double          *pdblmin    = &dblmin;
long double     *pldblmax   = &ldblmax;
long double     *pldblmin   = &ldblmin;
float           *pfltmax    = &fltmax;
float           *pfltmin    = &fltmin;

//****************************************************************************
// Purpose:     main program
int main(void)
{
    int     i_main = 4;

    // no parameters
    VoidProcNoParam();
    check(!(CharProcNoParam() == SCHAR_MIN), "CharProcNoParam()");
    check(!(UCharProcNoParam() == UCHAR_MAX), "UCharProcNoParam()");
    check(!(IntProcNoParam() == INT_MIN), "IntProcNoParam()");
    check(!(UIntProcNoParam() == UINT_MAX), "UIntProcNoParam()");
    check(!(ShortProcNoParam() == SHRT_MIN), "ShortProcNoParam()");
    check(!(UShortProcNoParam() == USHRT_MAX), "UShortProcNoParam()");
    check(!(LongProcNoParam() == LONG_MIN), "LongProcNoParam()");
    check(!(ULongProcNoParam() == ULONG_MAX), "ULongProcNoParam()");
    check(!(DoubleProcNoParam() == DBL_MIN), "DoubleProcNoParam()");
    check(!(LDoubleProcNoParam() == LDBL_MAX), "LDoubleProcNoParam()");
    check(!(FloatProcNoParam() == FLT_MIN), "FloatProcNoParam()");

    // at least one parameter
    VoidProc1Param(INT_MAX);
    check(!(CharProc1Param(SCHAR_MIN) == SCHAR_MIN), "CharProc1Param");
    check(!(UCharProc1Param(UCHAR_MAX) == UCHAR_MAX), "UCharProc1Param");
    check(!(IntProc1Param(INT_MIN) == INT_MIN), "IntProc1Param");
    check(!(UIntProc1Param(UINT_MAX) == UINT_MAX), "UIntProc1Param");
    check(!(ShortProc1Param(SHRT_MIN) == SHRT_MIN), "ShortProc1Param");
    check(!(UShortProc1Param(USHRT_MAX) == USHRT_MAX), "UShortProc1Param");
    check(!(LongProc1Param(LONG_MIN) == LONG_MIN), "LongProc1Param");
    check(!(ULongProc1Param(ULONG_MAX) == ULONG_MAX), "ULongProc1Param");
    check(!(DoubleProc1Param(DBL_MAX) == DBL_MAX), "DoubleProc1Param");
    check(!(LDoubleProc1Param(LDBL_MIN) == LDBL_MIN), "LDoubleProc1Param");
    check(!(FloatProc1Param(FLT_MAX) == FLT_MAX), "FloatProc1Param");

    // parameter passed on using global variables
    VoidProc1Param(intmax);
    check(!(CharProc1Param(scharmin) == SCHAR_MIN), "CharProc1Param");
    check(!(UCharProc1Param(ucharmax) == UCHAR_MAX), "UCharProc1Param");
    check(!(IntProc1Param(intmin) == INT_MIN), "IntProc1Param");
    check(!(UIntProc1Param(uintmax) == UINT_MAX), "UIntProc1Param");
    check(!(ShortProc1Param(shrtmin) == SHRT_MIN), "ShortProc1Param");
    check(!(UShortProc1Param(ushrtmax) == USHRT_MAX), "UShortProc1Param");
    check(!(LongProc1Param(longmin) == LONG_MIN), "LongProc1Param");
    check(!(ULongProc1Param(ulongmax) == ULONG_MAX), "ULongProc1Param");
    check(!(DoubleProc1Param(dblmax) == DBL_MAX), "DoubleProc1Param");
    check(!(LDoubleProc1Param(ldblmin) == LDBL_MIN), "LDoubleProc1Param");
    check(!(FloatProc1Param(fltmax) == FLT_MAX), "FloatProc1Param");

    // parameter passed on using pointers
    VoidProc1Param(*pintmax);
    check(!(CharProc1Param(*pscharmin) == SCHAR_MIN), "CharProc1Param*");
    check(!(UCharProc1Param(*pucharmax) == UCHAR_MAX), "UCharProc1Param*");
    check(!(IntProc1Param(*pintmin) == INT_MIN), "IntProc1Param*");
    check(!(UIntProc1Param(*puintmax) == UINT_MAX), "UIntProc1Param*");
    check(!(ShortProc1Param(*pshrtmin) == SHRT_MIN), "ShortProc1Param*");
    check(!(UShortProc1Param(*pushrtmax) == USHRT_MAX), "UShortProc1Param*");
    check(!(LongProc1Param(*plongmin) == LONG_MIN), "LongProc1Param*");
    check(!(ULongProc1Param(*pulongmax) == ULONG_MAX), "ULongProc1Param*");
    check(!(DoubleProc1Param(*pdblmax) == DBL_MAX), "DoubleProc1Param*");
    check(!(LDoubleProc1Param(*pldblmin) == LDBL_MIN), "LDoubleProc1Param*");
    check(!(FloatProc1Param(*pfltmax) == FLT_MAX), "FloatProc1Param*");

    // parameter passed on using near/far/ellipses
    check(!(PShortProc1Param(&shrtmin) == pshrtmin), "PShortProc1Param*");
    check(!(LPIntProc1Param((int *)&intmin) == (int *)pintmin), "LPIntProc1Param*");

    // variable parameters and recursive
    check(!(average(1,2,3,4,5,6,7,8,9, -1) == 5), "average");
    check(!(factorial(9) == (long)362880), "factorial");

    check(!(MultiParam(CHAR_MIN, INT_MAX, SHRT_MAX, LONG_MAX, DBL_MAX, LDBL_MAX, FLT_MAX) == LONG_MAX), "MultiParam");

    // C++ object parameters and template parameters
    check(!(Cxx_Tests() == 0), "C++ objects");

	return i_main;
}

//****************************************************************************
// Purpose:     function returning void and has no parameter
void VoidProcNoParam(void)
{	MIPS_DMY_PROLOG
    return;
}

// Purpose:     function returning char and has no parameter
char CharProcNoParam(void)
{	MIPS_DMY_PROLOG
    return SCHAR_MIN;
}

// Purpose:     function returning unsigned char and has no parameter
unsigned char UCharProcNoParam(void)
{	MIPS_DMY_PROLOG
    return UCHAR_MAX;
}

// Purpose:     function returning int and has no parameter
int IntProcNoParam(void)
{	MIPS_DMY_PROLOG
    return INT_MIN;
}

// Purpose:     function returning unsigned int and has no parameter
unsigned int UIntProcNoParam(void)
{	MIPS_DMY_PROLOG
    return UINT_MAX;
}

// Purpose:     function returning short and has no parameter
short ShortProcNoParam(void)
{	MIPS_DMY_PROLOG
    return SHRT_MIN;
}

// Purpose:     function returning unsigned short and has no parameter
unsigned short UShortProcNoParam(void)
{	MIPS_DMY_PROLOG
    return USHRT_MAX;
}

// Purpose:     function returning long and has no parameter
long LongProcNoParam(void)
{	MIPS_DMY_PROLOG
    return LONG_MIN;
}

// Purpose:     function returning unsigned long and has no parameter
unsigned long ULongProcNoParam(void)
{	MIPS_DMY_PROLOG
    return ULONG_MAX;
}

// Purpose:     function returning float and has no parameter
float FloatProcNoParam(void)
{	MIPS_DMY_PROLOG
    return FLT_MIN;
}

// Purpose:     function returning Double and has no parameter
double DoubleProcNoParam(void)
{	MIPS_DMY_PROLOG
    return DBL_MIN;
}

// Purpose:     function returning long double and has no parameter
long double LDoubleProcNoParam(void)
{	MIPS_DMY_PROLOG
    return LDBL_MAX;
}

//****************************************************************************
// Purpose:     function returning void and has 1 parameter
void VoidProc1Param(int i)
{	MIPS_DMY_PROLOG
    return;
}

// Purpose:     function returning char and has 1 parameter
char CharProc1Param(char c)
{	MIPS_DMY_PROLOG
    return c;
}
// Purpose:     function returning unsigned char and has 1 parameter
unsigned char UCharProc1Param(unsigned char uc)
{	MIPS_DMY_PROLOG
    return uc;
}

// Purpose:     function returning int and has 1 parameter
int IntProc1Param(int i)
{	MIPS_DMY_PROLOG
    return i;
}

// Purpose:     function returning unsigned int and has 1 parameter
unsigned int UIntProc1Param(unsigned int ui)
{	MIPS_DMY_PROLOG
    return ui;
}

// Purpose:     function returning short and has 1 parameter
short ShortProc1Param(short s)
{	MIPS_DMY_PROLOG
    return s;
}

// Purpose:     function returning unsigned short and has 1 parameter
unsigned short UShortProc1Param(unsigned short us)
{	MIPS_DMY_PROLOG
    return us;
}

// Purpose:     function returning long and has 1 parameter
long LongProc1Param(long l)
{	MIPS_DMY_PROLOG
    return l;
}

// Purpose:     function returning unsigned long and has 1 parameter
unsigned long ULongProc1Param(unsigned long ul)
{	MIPS_DMY_PROLOG
    return ul;
}

// Purpose:     function returning float and has 1 parameter
float FloatProc1Param(float f)
{	MIPS_DMY_PROLOG
    return f;
}

// Purpose:     function returning Double and has 1 parameter
double DoubleProc1Param(double d)
{	MIPS_DMY_PROLOG
    return d;
}

// Purpose:     function returning long double and has 1 parameter
long double LDoubleProc1Param(long double ld)
{	MIPS_DMY_PROLOG
    return ld;
}

//****************************************************************************
// Purpose:     function returning address and has 1 parameter
//              as pointer
short * PShortProc1Param(short * ps)
{	MIPS_DMY_PROLOG
    return ps;
}

int * LPIntProc1Param(int * lpi)
{	MIPS_DMY_PROLOG
    return lpi;
}

//****************************************************************************
// Purpose:     function returning address and has ... parameter
int average(int first, ...)
{	MIPS_DMY_PROLOG
    int count = 0, sum = 0, i = first;
    va_list marker;

    va_start(marker, first);
    while (i != -1)
    {
        sum += i;
        count++;
        i = va_arg(marker, int);
    }
    va_end(marker);
    return (sum ? (sum/count):(0));
}

//****************************************************************************
// Purpose:     recursive function
unsigned long factorial(int i)
{	MIPS_DMY_PROLOG
    if (i == 0)
        return 1;
    else
        return (i * factorial(i-1));
}

//****************************************************************************
// Purpose:     function returning long and has many parameters
long MultiParam(char c, int i, short s, long l, double d, long double ld, float f)
{	MIPS_DMY_PROLOG
	long double ldbl = (long double)c + (long double)i + (long double)s + (long double)l + (long double)d + (long double)ld + (long double)f;
	return l;
}

void check(int i, char *pc)
{	MIPS_DMY_PROLOG
	if( !i ) return;
	printf("%s FAILED", pc);
}
