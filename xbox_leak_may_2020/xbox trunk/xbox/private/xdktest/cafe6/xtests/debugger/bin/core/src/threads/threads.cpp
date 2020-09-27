#include <xtl.h> //xbox #include <windows.h>


HANDLE hSecondaryThread;
DWORD dwSecondaryThreadID, dwMainThreadID;
int intGlobal = 0;
BOOL bInfiniteLoop = TRUE;


void FuncCalledBySecondaryThreadFunc(void)
{	// first line of FuncCalledBySecondaryThreadFunc().
}


unsigned long __stdcall SecondaryThreadFunc(void * pdata)

{	// first line of SecondaryThreadFunc().

	int intLocalSecondaryThreadFunc = 1;
	intGlobal = 1;
	while(0);							// line after intGlobal changed.
	
	FuncCalledBySecondaryThreadFunc();
	while(0);							// line after call to FuncCalledBySecondaryThreadFunc().

	while(bInfiniteLoop);				// loop inside SecondaryThreadFunc().
	return 1;
}


void CreateThreadFunc(void)
{
	hSecondaryThread = CreateThread(NULL, 0, SecondaryThreadFunc, 0, 0, &dwSecondaryThreadID);
	// sleeping will enure that the thread is created before this function returns.
	Sleep(500);	// line after call to CreateThread().
}	// last line of CreateThreadFunc().


void __cdecl main()
{
	dwMainThreadID = GetCurrentThreadId();
	int intLocalMain = 0;
	CreateThreadFunc();
	while(bInfiniteLoop);	// loop inside main().
	while (1); /* never exit main func */ return;
}
