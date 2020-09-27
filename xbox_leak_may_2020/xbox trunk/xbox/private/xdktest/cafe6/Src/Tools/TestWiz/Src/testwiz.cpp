// TestWiz.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "TestWiz.h"
#include "chooser.h"
#include "testawx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE TestWizDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("TestWiz.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(TestWizDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(TestWizDLL);
		SetCustomAppWizClass(&awx);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("TestWiz.DLL Terminating!\n");
	}
	return 1;   // ok
}

/*HINSTANCE GetResourceHandle()
{
	return TestWizDLL.hModule;
}
 */
/*extern "C" APWZ_EXPORT CTestWizAppWizExtension* GetCustomAppWizClass()
{
	return &awx;
} */

