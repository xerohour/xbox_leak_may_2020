/*
**
** xboxdbg.c
**
** Utility functions to talk to the xbox debug monitor
**
*/

#include "precomp.h"
HINSTANCE hXboxdbg;
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID lpv)
{
	SOCKET s;

	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
        hXboxdbg = hinst;
		InitNotificationEvents();
		break;
	case DLL_PROCESS_DETACH:
		DestroyNotificationEvents();
		break;
	case DLL_THREAD_DETACH:
		CloseThreadConnection();
		break;
	}

	return TRUE;
}
