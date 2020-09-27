#include <windows.h>
#include "hello1.h"

int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)
{
//        dll_func();
	shared_func();
	return 0;
}
