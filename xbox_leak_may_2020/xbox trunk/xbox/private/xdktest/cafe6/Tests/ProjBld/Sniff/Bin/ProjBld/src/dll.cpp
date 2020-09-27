#include "..\hello1.h"

__declspec(dllexport) void dll_func(void)
{
	shared_func();
}
