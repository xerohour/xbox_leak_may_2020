#include <windows.h>
#include <stdio.h>
#include "hello1.h"

int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)
{
	shared_func();
    printf("How are you doing!");
	return 0;
}
