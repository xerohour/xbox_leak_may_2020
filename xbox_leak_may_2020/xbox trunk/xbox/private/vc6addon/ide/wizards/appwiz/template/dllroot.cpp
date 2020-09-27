// $$root$$.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
$$IF(EXTDLL)
#include <afxdllx.h>
$$ELSE // !EXTDLL
#include "$$root$$.h"
$$ENDIF //!EXTDLL

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

$$IF(EXTDLL)
$$ELSE
//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//
$$ENDIF

$$IF(EXTDLL)
static AFX_EXTENSION_MODULE $$safe_root$$DLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
$$IF(VERBOSE)
	// Remove this if you use lpReserved
$$ENDIF
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("$$ROOT$$.DLL Initializing!\n");
		
$$IF(VERBOSE)
		// Extension DLL one-time initialization
$$ENDIF
		if (!AfxInitExtensionModule($$safe_root$$DLL, hInstance))
			return 0;

$$IF(VERBOSE)
		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

$$ENDIF
		new CDynLinkLibrary($$safe_root$$DLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("$$ROOT$$.DLL Terminating!\n");
$$IF(VERBOSE)
		// Terminate the library before destructors are called
$$ENDIF
		AfxTermExtensionModule($$safe_root$$DLL);
	}
$$IF(VERBOSE)
	return 1;   // ok
$$ELSE
	return 1;
$$ENDIF
}
$$ELSE //!EXTDLL
/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$

BEGIN_MESSAGE_MAP($$APP_CLASS$$, $$APP_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$APP_CLASS$$)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$ construction

$$APP_CLASS$$::$$APP_CLASS$$()
{
$$IF(VERBOSE)
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
$$ENDIF
}

/////////////////////////////////////////////////////////////////////////////
// The one and only $$APP_CLASS$$ object

$$APP_CLASS$$ theApp;
$$IF(AUTOMATION || SOCKETS)

/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$ initialization

BOOL $$APP_CLASS$$::InitInstance()
{
$$IF(SOCKETS)
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

$$ENDIF //SOCKETS
$$IF(AUTOMATION)
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleObjectFactory::RegisterAll();

$$ENDIF //AUTOMATION
	return TRUE;
}
$$ENDIF //AUTOMATION || SOCKETS
$$IF(AUTOMATION)

/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll();
	return S_OK;
}
$$ENDIF //AUTOMATION
$$ENDIF //EXTDLL
