#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>
//placeholders for types
long g_long=1;
int  g_int=2;
unsigned int g_unsigned=3;
__int64 g__int64=4;
double  g_double=5.0;

enum CALLTYPE{ __INT64=0, UNSIGNED, SIGNEDLONG};


__int64 fibon(__int64 n)
{
	if(n==0)
		return 1;
	if(n==1)
		return 1;
	return fibon(n-2) + fibon(n-1);
}


long fibon(unsigned int u)
{ //funny thing: return the -fib(u)
	if(u==0)
		return -1;
	if(u==1)
		return -1;
	return -(fibon(u-2) + fibon(u-1));
}


long fibon(long m)
{
	if(m==0)
		return 1;
	int fib0=0;
	int fib1=1;

	for(int i=0; i<m; i++)
	{
		long temp = fib0;
		fib0 = fib1;
		fib1 = temp + fib1;
		/* fib0 += fib1; fib1 = fib0 - fib1 */
	}
	return fib1;
}


__int64 fibontype(__int64 n, CALLTYPE type = UNSIGNED)
{
	__int64 result=0;
	switch(type)
	{
	case __INT64: result = fibon(__int64(n));
		break;
	case UNSIGNED: result = fibon(unsigned int(n));
		break;
	case SIGNEDLONG: result = fibon(long(n));
	}
	return result;
}


//****************************************************************************
// Purpose:     function returning address and has ... parameter
int average(int first, ...)
{
    int count = 0, sum = 0, i = first;
    va_list marker;

    va_start(marker, first);
    while (i != INT_MIN)
    {
        sum += i;
        count++;
        i = va_arg(marker, int);
    }
    va_end(marker);
    return (sum ? (sum/count):(0));
}

/*template <long nn>
long factorial(long  nn)
{ 
	return nn*factorial<nn-1>(nn-1); 
}

template<> long factorial<0>(long n)
{ return 1; }*/
double factorial(double nn)
{
	return (nn<=1)?1:factorial(nn-1)*nn;
}


//long fact(long)
//	long fact = factorial<40>(40);
//	double fact = factorial(m);

void fiboncases(int argc, char *argv[])
{

	__int64 nint64=(argc>1)?atoi(*(argv+1)):0; //arg for recursive fibon
	long nlong=1;
	long m=(argc>2)?atoi(*(argv+2)):0; //arg for norec fibon

	long tlong = fibon(5u);
	__int64 fib=0;
	fib=fibon(4i64);
	fib=fibon(g_unsigned);
	fib=fibon(nlong);
	fib=fibontype(fibon(4u), SIGNEDLONG);
	double dresult = factorial( fibon(nlong));
	printf("fibon(%d):%d\n",nint64,fib);
	fib=fibon(m);
	printf("no rec fibon(%d):%d\n",m,fib);
	printf("Average:%d\n",average(0,fibon(g_unsigned),3,4,5,6,7,8, INT_MIN) );
}


void main(int argc, char *argv[])
{
	for(int i=0;i<10;i++)
		fiboncases(argc,argv);
}