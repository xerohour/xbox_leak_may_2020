// apwiztes.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "apwiztes.h"
#include "apwizaw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE apwiztesDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("APWIZTES.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(apwiztesDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(apwiztesDLL);
		//Register the CustomAppwizard with MFCAPWZ.DLL
		SetCustomAppWizClass(&apwiztesaw) ;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("APWIZTES.DLL Terminating!\n");
	}
	return 1;   // ok
}


