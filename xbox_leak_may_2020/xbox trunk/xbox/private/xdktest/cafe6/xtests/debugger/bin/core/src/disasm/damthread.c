#include <xtl.h> //xbox #include <windows.h>
#include <math.h>

double FuncAcA(int nArg)
{
	return (double) (nArg*12);
}

LPTHREAD_START_ROUTINE ThreadRoutine(LPVOID pArg)
{
	int *nArg = (int*) pArg;
	*nArg += 25;
	
	SetLastError(1);
	while(1)
		sqrt(FuncAcA(*nArg));

	return 0;
}


