#if defined(_WINDOWS)
#include <windows.h>
#endif

#include <limits.h>
#include <float.h>
#include <stdarg.h>
//#include "testg.h"

//***************************************************************************
// prototypes
int                     Calls(void);
int                     TestG(void);
void					Based(void);

void                    VoidProcNoParam(void);
char                    CharProcNoParam(void);
unsigned char           UCharProcNoParam(void);
int                     IntProcNoParam(void);
unsigned int            UIntProcNoParam(void);
__int64                 Int64ProcNoParam(void);
unsigned __int64        UInt64ProcNoParam(void);
short                   ShortProcNoParam(void);
unsigned short          UShortProcNoParam(void);
long                    LongProcNoParam(void);
unsigned long           ULongProcNoParam(void);
float                   FloatProcNoParam(void);
double                  DoubleProcNoParam(void);
long double             LDoubleProcNoParam(void);

void                    VoidProc1Param(int i);
char                    CharProc1Param(char c);
unsigned char           UCharProc1Param(unsigned char uc);
int                     IntProc1Param(int i);
unsigned int            UIntProc1Param(unsigned int ui);
__int64                 Int64Proc1Param(__int64 i64);
unsigned __int64        UInt64Proc1Param(unsigned __int64 ui64);
short                   ShortProc1Param(short s);
unsigned short          UShortProc1Param(unsigned short us);
long                    LongProc1Param(long l);
unsigned long           ULongProc1Param(unsigned long ul);
float                   FloatProc1Param(float f);
double                  DoubleProc1Param(double d);
long double             LDoubleProc1Param(long double ld);

short *            		PShortProc1Param(short * ps);
int * 					LPIntProc1Param(int * lpi);

int                     average(int first, ...);
unsigned long           factorial(int i);

long		            MultiParam(char c, int i, short s, long l, double d, long double ld, float f);
void 					check(int i, char *pc);


// for mips
#if defined( _M_MRX000 ) || defined(_M_ALPHA)
#define MIPS_DMY_PROLOG int mip_dummy=0;
#else
#define MIPS_DMY_PROLOG 
#endif 
