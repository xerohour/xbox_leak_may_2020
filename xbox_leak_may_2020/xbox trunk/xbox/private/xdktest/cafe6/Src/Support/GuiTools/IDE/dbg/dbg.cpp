/////////////////////////////////////////////////////////////////////////////
// dbg.cpp
//
// email	date		change
// briancr	01/11/95	created
//
// copyright 1994 Microsoft

#include "stdafx.h"
#include "afxdllx.h"

#define new DEBUG_NEW

///////////////////////////////////////////////////////////////////////////////
//	DLL support

static AFX_EXTENSION_MODULE extensionDLL = { NULL, NULL };

extern "C" __declspec(dllexport) int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("Debugger DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(extensionDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(extensionDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("Debugger DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(extensionDLL);
	}
	return 1;   // ok

	UNREFERENCED_PARAMETER(lpReserved);
}

