/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	evnthdlr.h

	Author: Sean Christofferson

	24 July 1997 - Created

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// COM headers
#ifndef __EVENT_HANDLER_H__
#define __EVENT_HANDLER_H__

#define _WIN32_DCOM
#include <objbase.h>
#include <ocidl.h>

// Automation headers
#include <objmodel\appauto.h>
#include <objmodel\appguid.h>
//review (hardcoded GUID for debug MSDEV CLSID, hope it won't chnage, otherwise we will need to adjust it here)
// {E8BEA480-DDE4-11CF-9C4A-00A0C90A632C}
DEFINE_GUID(CLSID_DebugApp, 
0xE8BEA480, 0xDDE4, 0x11CF, 0x9C, 0x4A, 0x0, 0xA0, 0xC9, 0xA, 0x63, 0x2C);

#include <objmodel\bldauto.h>
#include <objmodel\bldguid.h>

#include <objmodel\dbgauto.h>
#include <objmodel\dbgguid.h>

#include <objmodel\textauto.h>
#include <objmodel\textguid.h>

// C Library headers
#include <assert.h>


// Define an inline function for HRESULT checking if one does not already exist
#ifndef VERIFY_OK
#define VERIFY_OK(x) {hres = x; assert(SUCCEEDED(hres));}
#endif

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
class __declspec(dllexport) CEventHandler : public IApplicationEvents
{
	public:
		// IUnknown Methods
		HRESULT __stdcall QueryInterface(const IID&, void**);
		ULONG	__stdcall AddRef();
		ULONG	__stdcall Release();

		// IDispatch Methods
		HRESULT __stdcall GetIDsOfNames(
			const IID&,
			OLECHAR**, 
			unsigned int, 
			LCID, 
			DISPID*
			);
		HRESULT __stdcall GetTypeInfo(
			unsigned int,
			LCID,
			ITypeInfo**
			);
		HRESULT __stdcall GetTypeInfoCount(
			unsigned int*
			);
		HRESULT __stdcall Invoke(
			DISPID,
			REFIID,
			LCID,
			WORD,
			DISPPARAMS*,
			VARIANT*,
			EXCEPINFO*,
			unsigned int*
			);

		// IApplicationEvents Methods
		HRESULT __stdcall BeforeBuildStart();
		HRESULT __stdcall BuildFinish(long nNumErrors, long nNumWarnings);
		HRESULT __stdcall BeforeApplicationShutDown() ;
		HRESULT __stdcall DocumentOpen(IDispatch* theDocument) ;
		HRESULT __stdcall BeforeDocumentClose(IDispatch* theDocument) ;
		HRESULT __stdcall DocumentSave(IDispatch* theDocument) ;
		HRESULT __stdcall NewDocument(IDispatch* theDocument) ;
		HRESULT __stdcall WindowActivate(IDispatch* theWindow) ;
		HRESULT __stdcall WindowDeactivate(IDispatch* theWindow) ;
		HRESULT __stdcall WorkspaceOpen() ;
		HRESULT __stdcall WorkspaceClose() ;
		HRESULT __stdcall NewWorkspace() ;

		// Constructors and Destructors
		CEventHandler();
		~CEventHandler();

		// MsDev Communication
		void Connect(BOOL isDebug);
		void Disconnect();

		// Wait methods
		void WaitForBuildStart();
		bool WaitForBuildFinish(int TimeoutInMintues);
		void WaitForApplicationShutDown();
		void WaitForDocumentOpen();
		void WaitForDocumentClose();
		void WaitForDocumentSave();
		void WaitForNewDocument();
		void WaitForWindowActivate();
		void WaitForWindowDeactivate();
		void WaitForWorkspaceOpen();
		void WaitForWorkspaceClose();
		void WaitForNewWorkspace();

//interface pointers to be kept while tests running
	public:
	//add more Target-lifetime interfaces as they're needed here.
	//do not add short-lived objects (like IWindow)!!!
		IApplication	*m_pApp;
		IDebugger		*m_pDbg;
	private:
		IConnectionPoint*m_pICP;

		// Connection point cookie
		DWORD m_dwConnectionID;

		// Event Handles
		HANDLE m_hevntBuildStart;
		HANDLE m_hevntBuildFinish;
		HANDLE m_hevntAppShutdown;
		HANDLE m_hevntDocOpen;
		HANDLE m_hevntDocClose;
		HANDLE m_hevntDocSave;
		HANDLE m_hevntNewDoc;
		HANDLE m_hevntWndActivate;
		HANDLE m_hevntWndDeactivate;
		HANDLE m_hevntWorkspaceOpen;
		HANDLE m_hevntWorkspaceClose;
		HANDLE m_hevntNewWorkspace;

		// Reference Count
		long m_cRef;
}; 

#endif
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	file evnthdlr.h
*/