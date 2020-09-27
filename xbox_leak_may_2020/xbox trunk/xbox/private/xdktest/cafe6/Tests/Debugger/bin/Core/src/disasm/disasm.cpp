#include <windows.h>

// DAM window test
#include <math.h>

extern "C" 
{
LPTHREAD_START_ROUTINE ThreadRoutine(LPVOID pArg);
}

void OverloadedFunc(int nArg)
{
	return;
}

void OverloadedFunc(void)
{
	return;
}

int Func(double dArg)
{
	return (int) dArg;
}

int FuncWithArg(int nArg)
{
	return nArg + 2;
}

// Template 
template <class T> T TemplateFoo(T tArg)
{
	return ++tArg;					/* TemplateFoo body */
}

int FAR PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	char	chJustChar = 'a';
	int		nJustInt = 7;
	int		*pJustInt = &nJustInt;
	double	dJustDouble  =9.37;
	float  fJustFloat = 1.00;

	HANDLE hThread;
	DWORD dwThreadId;

	// The next two line should be normally commented.  They are used to ccause GPF and invoke default debugger
	//int *popa;
	//*popa = 65700;

	__asm {
		push eax;
		pop eax;
	}

	int *p = &nJustInt;
	*p = 0;

	TemplateFoo <char>(chJustChar);		/* First line for tests */
	TemplateFoo <int>(nJustInt);

	goto Ku_Ku;
		nJustInt = 7;
	Ku_Ku:
		nJustInt = 17;

	FuncWithArg(1);						/* Second line for tests */
	Func(dJustDouble);

	OverloadedFunc();
	OverloadedFunc(nJustInt);

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ThreadRoutine, (void*) pJustInt, 0L,&dwThreadId);
	ResumeThread(hThread);

	while(1)							/* Last line for tests */
		Sleep(5000);

	return 1;							
}
