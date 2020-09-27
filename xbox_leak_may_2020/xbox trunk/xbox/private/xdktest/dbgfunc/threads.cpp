// threads.cpp
//
// based on VC6 QA debugger test code
// \\vsqaauto\vc6snaps\debugger\core\src\threads

#include "dbgtest.h"

static HANDLE hSecondaryThread = NULL;
static DWORD dwMainThreadID, dwSecondaryThreadID;
static int intGlobal = 0;
static BOOL bInfiniteLoop = TRUE;

void FuncCalledBySecondaryThreadFunc(void)
{
    // first line of FuncCalledBySecondaryThreadFunc().
    intGlobal++;
}


unsigned long __stdcall SecondaryThreadFunc(void * pdata)
{
    // first line of SecondaryThreadFunc().

	int intLocalSecondaryThreadFunc = 1;
	intGlobal = 1;
	// line after intGlobal changed.
	while(0);
	
	FuncCalledBySecondaryThreadFunc();
	// line after call to FuncCalledBySecondaryThreadFunc().
	while(0);							

    bInfiniteLoop = TRUE;

	// loop inside SecondaryThreadFunc().
	while(bInfiniteLoop);

    hSecondaryThread = NULL;
	return 1;
}


// If a second thread already exists, CreateThreadFunc() will
// not create a new one.  To kill the secondary thread, set
// bInfiniteLoop to FALSE
void CreateThreadFunc(void)
{
    if (hSecondaryThread != NULL)
        return;

	hSecondaryThread = CreateThread(NULL, 0, SecondaryThreadFunc, 0, 0, &dwSecondaryThreadID);
	// sleeping will ensure that the thread is created before this function returns.
	Sleep(3000);
}


void Test_Threads()
{
	dwMainThreadID = GetCurrentThreadId();
	int intLocalWinMain = 0;
	CreateThreadFunc();
	return;
}
