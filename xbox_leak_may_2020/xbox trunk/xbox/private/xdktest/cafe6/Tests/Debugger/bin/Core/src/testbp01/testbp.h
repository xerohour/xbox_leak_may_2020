//**********************************************************************
//  Header for Testbp01 app : Testing of Breakpoints
//***********************************************************************

#define HI 0
#define LO 1

#define FALSE 0
#define TRUE  1

#define NEARSIZE    10000
#define FARSIZE     32767
#define HUGESIZE    75000


typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef long double ldouble;


void	_cdecl CFncVV (void);
char	_cdecl CFncCC (char);
uchar	_cdecl CFncUCUC(uchar);
short	_cdecl CFncSS(short);
ushort	_cdecl CFncUSUS(ushort);
long	_cdecl CFncLL(long);
ulong	_cdecl CFncULUL(ulong);
float	_cdecl CFncFF(float);
double	_cdecl CFncDD(double);
ldouble _cdecl CFncLDLD(ldouble);
short	_cdecl CFncSSRecurse(short);
void	_cdecl CFncNearArray(void);
void	_cdecl Eval( ushort , char *);

//v-katsuf - for mips dmy prolog
#if defined(_M_MRX000) || defined(_M_ALPHA)
#define MIPS_DMY_PROLOG int mip_dummy=0;
#else
#define MIPS_DMY_PROLOG 
#endif
