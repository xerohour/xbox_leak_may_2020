// nlgmain.cpp 
#include <windows.h>
#include "nlgmain.h"

int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)
{
	eh01();		// C++ exception handling cases
	//seh01();	// Structured exception handling cases
	ljmp01();		// Setjmp/Longjmp cases

	return 0;
}
