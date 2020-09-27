/*
\\vsqaauto\VC6Snaps\Debugger\Core\src\stack01\cstack.c
*/
#include "stack.h"

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
void Test_Stack()
{
    // no parameters
    VoidProcNoParam();
    check(!(CharProcNoParam() == SCHAR_MIN),	TEXT("CharProcNoParam()"));
    check(!(UCharProcNoParam() == UCHAR_MAX),	TEXT("UCharProcNoParam()"));
    check(!(IntProcNoParam() == INT_MIN),		TEXT("IntProcNoParam()"));
    check(!(UIntProcNoParam() == UINT_MAX),		TEXT("UIntProcNoParam()"));
    check(!(ShortProcNoParam() == SHRT_MIN),	TEXT("ShortProcNoParam()"));
    check(!(UShortProcNoParam() == USHRT_MAX),	TEXT("UShortProcNoParam()"));
    check(!(LongProcNoParam() == LONG_MIN),		TEXT("LongProcNoParam()"));
    check(!(ULongProcNoParam() == ULONG_MAX),	TEXT("ULongProcNoParam()"));
    check(!(DoubleProcNoParam() == DBL_MIN),	TEXT("DoubleProcNoParam()"));
    check(!(LDoubleProcNoParam() == LDBL_MAX),	TEXT("LDoubleProcNoParam()"));
    check(!(FloatProcNoParam() == FLT_MIN),		TEXT("FloatProcNoParam()"));

    // at least one parameter
    VoidProc1Param(INT_MAX);
    check(!(CharProc1Param(SCHAR_MIN) == SCHAR_MIN),	TEXT("CharProc1Param"));
    check(!(UCharProc1Param(UCHAR_MAX) == UCHAR_MAX),	TEXT("UCharProc1Param"));
    check(!(IntProc1Param(INT_MIN) == INT_MIN),			TEXT("IntProc1Param"));
    check(!(UIntProc1Param(UINT_MAX) == UINT_MAX),		TEXT("UIntProc1Param"));
    check(!(ShortProc1Param(SHRT_MIN) == SHRT_MIN),		TEXT("ShortProc1Param"));
    check(!(UShortProc1Param(USHRT_MAX) == USHRT_MAX),	TEXT("UShortProc1Param"));
    check(!(LongProc1Param(LONG_MIN) == LONG_MIN),		TEXT("LongProc1Param"));
    check(!(ULongProc1Param(ULONG_MAX) == ULONG_MAX),	TEXT("ULongProc1Param"));
    check(!(DoubleProc1Param(DBL_MAX) == DBL_MAX),		TEXT("DoubleProc1Param"));
    check(!(LDoubleProc1Param(LDBL_MIN) == LDBL_MIN),	TEXT("LDoubleProc1Param"));
    check(!(FloatProc1Param(FLT_MAX) == FLT_MAX),		TEXT("FloatProc1Param"));

    // parameter passed on using global variables
    VoidProc1Param(intmax);
    check(!(CharProc1Param(scharmin) == SCHAR_MIN),		TEXT("CharProc1Param"));
    check(!(UCharProc1Param(ucharmax) == UCHAR_MAX),	TEXT("UCharProc1Param"));
    check(!(IntProc1Param(intmin) == INT_MIN),			TEXT("IntProc1Param"));
    check(!(UIntProc1Param(uintmax) == UINT_MAX),		TEXT("UIntProc1Param"));
    check(!(ShortProc1Param(shrtmin) == SHRT_MIN),		TEXT("ShortProc1Param"));
    check(!(UShortProc1Param(ushrtmax) == USHRT_MAX),	TEXT("UShortProc1Param"));
    check(!(LongProc1Param(longmin) == LONG_MIN),		TEXT("LongProc1Param"));
    check(!(ULongProc1Param(ulongmax) == ULONG_MAX),	TEXT("ULongProc1Param"));
    check(!(DoubleProc1Param(dblmax) == DBL_MAX),		TEXT("DoubleProc1Param"));
    check(!(LDoubleProc1Param(ldblmin) == LDBL_MIN),	TEXT("LDoubleProc1Param"));
    check(!(FloatProc1Param(fltmax) == FLT_MAX),		TEXT("FloatProc1Param"));

    // parameter passed on using pointers
    VoidProc1Param(*pintmax);
    check(!(CharProc1Param(*pscharmin) == SCHAR_MIN),	TEXT("CharProc1Param*"));
    check(!(UCharProc1Param(*pucharmax) == UCHAR_MAX),	TEXT("UCharProc1Param*"));
    check(!(IntProc1Param(*pintmin) == INT_MIN),		TEXT("IntProc1Param*"));
    check(!(UIntProc1Param(*puintmax) == UINT_MAX),		TEXT("UIntProc1Param*"));
    check(!(ShortProc1Param(*pshrtmin) == SHRT_MIN),	TEXT("ShortProc1Param*"));
    check(!(UShortProc1Param(*pushrtmax) == USHRT_MAX),	TEXT("UShortProc1Param*"));
    check(!(LongProc1Param(*plongmin) == LONG_MIN),		TEXT("LongProc1Param*"));
    check(!(ULongProc1Param(*pulongmax) == ULONG_MAX),	TEXT("ULongProc1Param*"));
    check(!(DoubleProc1Param(*pdblmax) == DBL_MAX),		TEXT("DoubleProc1Param*"));
    check(!(LDoubleProc1Param(*pldblmin) == LDBL_MIN),	TEXT("LDoubleProc1Param*"));
    check(!(FloatProc1Param(*pfltmax) == FLT_MAX),		TEXT("FloatProc1Param*"));

    // parameter passed on using near/far/ellipses
    check(!(PShortProc1Param(&shrtmin) == pshrtmin),			TEXT("PShortProc1Param*"));
    check(!(LPIntProc1Param((int *)&intmin) == (int *)pintmin),	TEXT("LPIntProc1Param*"));

    // variable parameters and recursive
    check(!(average(1,2,3,4,5,6,7,8,9, -1) == 5),	TEXT("average"));
    check(!(factorial(9) == (long)362880),			TEXT("factorial"));

    check(!(MultiParam(CHAR_MIN, INT_MAX, SHRT_MAX, LONG_MAX, DBL_MAX, LDBL_MAX, FLT_MAX) == LONG_MAX), TEXT("MultiParam"));

    // C++ object parameters and template parameters
    check(!(Cxx_Tests() == 0), TEXT("C++ objects"));
}

//****************************************************************************
// Purpose:     function returning void and has no parameter
void VoidProcNoParam(void)
{
	return;
}

// Purpose:     function returning char and has no parameter
char CharProcNoParam(void)
{
	return SCHAR_MIN;
}

// Purpose:     function returning unsigned char and has no parameter
unsigned char UCharProcNoParam(void)
{
	return UCHAR_MAX;
}

// Purpose:     function returning int and has no parameter
int IntProcNoParam(void)
{
	return INT_MIN;
}

// Purpose:     function returning unsigned int and has no parameter
unsigned int UIntProcNoParam(void)
{
	return UINT_MAX;
}

// Purpose:     function returning short and has no parameter
short ShortProcNoParam(void)
{
	return SHRT_MIN;
}

// Purpose:     function returning unsigned short and has no parameter
unsigned short UShortProcNoParam(void)
{
	return USHRT_MAX;
}

// Purpose:     function returning long and has no parameter
long LongProcNoParam(void)
{
	return LONG_MIN;
}

// Purpose:     function returning unsigned long and has no parameter
unsigned long ULongProcNoParam(void)
{
	return ULONG_MAX;
}

// Purpose:     function returning float and has no parameter
float FloatProcNoParam(void)
{
	return FLT_MIN;
}

// Purpose:     function returning Double and has no parameter
double DoubleProcNoParam(void)
{
	return DBL_MIN;
}

// Purpose:     function returning long double and has no parameter
long double LDoubleProcNoParam(void)
{
	return LDBL_MAX;
}

// this variable is modified by dummy code in the following
// functions, this prevents the generated code from becoming
// identical and getting merged (see MSDN documentation on 
// linker /OPT:ICF switch)

static int g_iDummy = 0;

//****************************************************************************
// Purpose:     function returning void and has 1 parameter
void VoidProc1Param(int i)
{
    g_iDummy = 1;
	return;
}

// Purpose:     function returning char and has 1 parameter
char CharProc1Param(char c)
{
    g_iDummy = 2;
	return c;
}
// Purpose:     function returning unsigned char and has 1 parameter
unsigned char UCharProc1Param(unsigned char uc)
{
    g_iDummy = 3;
	return uc;
}

// Purpose:     function returning int and has 1 parameter
int IntProc1Param(int i)
{
    g_iDummy = 4;
	return i;
}

// Purpose:     function returning unsigned int and has 1 parameter
unsigned int UIntProc1Param(unsigned int ui)
{
    g_iDummy = 5;
	return ui;
}

// Purpose:     function returning short and has 1 parameter
short ShortProc1Param(short s)
{
    g_iDummy = 6;
	return s;
}

// Purpose:     function returning unsigned short and has 1 parameter
unsigned short UShortProc1Param(unsigned short us)
{
    g_iDummy = 7;
	return us;
}

// Purpose:     function returning long and has 1 parameter
long LongProc1Param(long l)
{
    g_iDummy = 8;
	return l;
}

// Purpose:     function returning unsigned long and has 1 parameter
unsigned long ULongProc1Param(unsigned long ul)
{
    g_iDummy = 9;
	return ul;
}

// Purpose:     function returning float and has 1 parameter
float FloatProc1Param(float f)
{
    g_iDummy = 10;
	return f;
}

// Purpose:     function returning Double and has 1 parameter
double DoubleProc1Param(double d)
{
    g_iDummy = 11;
	return d;
}

// Purpose:     function returning long double and has 1 parameter
long double LDoubleProc1Param(long double ld)
{
    g_iDummy = 12;
	return ld;
}

//****************************************************************************
// Purpose:     function returning address and has 1 parameter
//              as pointer
short * PShortProc1Param(short * ps)
{
    g_iDummy = 13;
	return ps;
}

int * LPIntProc1Param(int * lpi)
{
    g_iDummy = 14;
	return lpi;
}

//****************************************************************************
// Purpose:     function returning address and has ... parameter
int average(int first, ...)
{
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
{
	if (i == 0)
        return 1;
    else
        return (i * factorial(i-1));
}

//****************************************************************************
// Purpose:     function returning long and has many parameters
long MultiParam(char c, int i, short s, long l, double d, long double ld, float f)
{
	long double ldbl = (long double)c + (long double)i + (long double)s + (long double)l + (long double)d + (long double)ld + (long double)f;
	return l;
}

void check(int i, TCHAR *pc)
{
	if( !i ) return;
	OutputDebugString(TEXT("FAILED: "));
	OutputDebugString(pc);
}
