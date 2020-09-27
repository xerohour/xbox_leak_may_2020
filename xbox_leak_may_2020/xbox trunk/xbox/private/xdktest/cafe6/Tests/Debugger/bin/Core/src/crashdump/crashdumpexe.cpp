#include <windows.h>

__declspec(dllimport) void dllfunc(void);

int FAR PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	dllfunc();	
	return 0;
}

