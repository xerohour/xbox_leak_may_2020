#include <windows.h>

extern "C"
{

__declspec(dllexport) void DllFunc(void)
{											// first line of DllFunc()
}

}


int WINAPI DllMain(HANDLE hInst, ULONG ulRbc, LPVOID lpReserved)
{
	return 1;
}
