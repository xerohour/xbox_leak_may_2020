#include <windows.h>


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
	Sleep(3000);	// line after call to CreateThread().
}	// last line of CreateThreadFunc().


int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR lszCmdLine, int nCmdShow)
{
	dwMainThreadID = GetCurrentThreadId();
	int intLocalWinMain = 0;
	CreateThreadFunc();
	while(bInfiniteLoop);	// loop inside WinMain().
	return 0;
}
