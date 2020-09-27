#include <xtl.h> //xbox #include <windows.h>


DWORD dwMainThreadID;
DWORD dwSecondaryThreadID;


LPTHREAD_START_ROUTINE ThreadRoutine(LPVOID pArg)
{
	while(1)
		Sleep(5000);		/* Thread proc */		
	return 0;
}

int Foo(int nArg)
{
	return nArg*2;
}							/* Return form Foo */


void __cdecl main()
{

	HANDLE hThread;
	int res = Foo(7);		/* First line for tests */

	double y = 1e-19;
	_asm fld y;
	
	float f1;

	/* check «Overflow» exception */
	f1 = (float) 1.234567e38;
	f1 *= f1;
	f1 *= f1;	/* Exception should be here for CTRL 277 */
	f1 *= f1;

	/* check «divide by zero» exception */
	f1 = 10.;
	f1 /= 0;	/* Exception should be here for CTRL 27B */


	dwMainThreadID = GetCurrentThreadId();
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ThreadRoutine, (void*) &res, 0L, &dwSecondaryThreadID);
	ResumeThread(hThread);

	while(1)						
		Sleep(5000);

	while (1); /* never exit */ return;							
}
