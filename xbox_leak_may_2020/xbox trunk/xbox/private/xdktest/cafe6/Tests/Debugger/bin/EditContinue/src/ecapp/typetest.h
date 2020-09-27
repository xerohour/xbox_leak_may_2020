
#define NEARSIZE    10000
#define FARSIZE     32767
#define HUGESIZE    75000

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef long double ldouble;



void	_cdecl CFncVV (void);
char	_cdecl CFncCC (char);
uchar	_cdecl CFncUCUC(uchar);
int		_cdecl CFncInt (int);
uint	_cdecl CFncUInt(uint);
__int64	_cdecl CFncInt64(__int64);
short	_cdecl CFncSS(short);
ushort	_cdecl CFncUSUS(ushort);
long	_cdecl CFncLL(long);
ulong	_cdecl CFncULUL(ulong);
float	_cdecl CFncFF(float);
double	_cdecl CFncDD(double);
ldouble _cdecl CFncLDLD(ldouble);
short	_cdecl CFncSSRecurse(short);
void	_cdecl CFncAsm(void);
