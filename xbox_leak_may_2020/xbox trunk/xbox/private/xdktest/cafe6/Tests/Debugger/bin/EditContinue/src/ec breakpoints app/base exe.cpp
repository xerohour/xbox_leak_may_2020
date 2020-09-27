#include <windows.h>


void ExeFunc1(void);
void ExeFunc2(void);


int FAR PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{	// first line of WinMain().
	ExeFunc1();
	while(0);	// line after call to ExeFunc1().
	return 0;	// last line of WinMain().
}


void ExeFunc1(void)
{	// first line of ExeFunc1().
	ExeFunc2();
	while(0);	// line after call to ExeFunc2().
}	// last line of ExeFunc1().


void ExeFunc2(void)
{	// first line of ExeFunc2().
	while(0);
}	// last line of ExeFunc2().


