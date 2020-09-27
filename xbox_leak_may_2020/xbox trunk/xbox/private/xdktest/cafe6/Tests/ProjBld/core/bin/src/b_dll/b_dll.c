#if defined( WIN )
#include <windows.h>
#endif //WIN
#include <sys\types.h>
#include "b_dll.h"

double WINAPI dPow(int base, int power)
{
	int i;
	double result;
	/* note: don't handle zero's */
	if (power==0)
		return 1;
	if (base==0)
		return 0;
	for (result=base,i=1;i<power;i++)
		result *= base;
	
	return result;
} 


INT WINAPI DllMain(HANDLE hInst, ULONG ulRbc, LPVOID lpReserved)
{
	return 1;
} 

