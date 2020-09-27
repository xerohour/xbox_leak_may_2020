#include <windows.h>


extern "C"
{

__declspec(dllexport) void DllFunc2(void)
{	// first line of DllFunc2()
}


__declspec(dllexport) void DllFunc1(void)
{	// first line of DllFunc1()
}

__declspec(dllexport) void DefaultDllFunc(void)
{	// first line of DefaultDllFunc()
}

}


int WINAPI DllMain(HANDLE hInst, ULONG ulRbc, LPVOID lpReserved)
{
	return 1;
}
