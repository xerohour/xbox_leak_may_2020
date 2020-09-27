/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	evnthdlr.cpp

	Author: Sean Christofferson (t-seanc)

	24 July 1997 - Created

	CEventHandler implments a COM object that handles events from the Developer Studio
	IApplicationEvents connection point.  It currently does not handle the debugger 
	events.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// Precompiled header
#include "StdAfx.h"

// Project includes
#define INITGUID
#include "evnthdlr.h"

// Global HRESULT
HRESULT hres;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
CEventHandler::CEventHandler()
{
	// Initialize COM for multithreading
//	VERIFY_OK(CoInitializeEx(NULL, COINIT_MULTITHREADED));
	VERIFY_OK(CoInitialize(NULL)); //REVIEW(chriskoz): I don't believe we need free threaded model here
	// Reset the reference count to one since we are passing out a pointer
	m_cRef = 1;

	// Set up the events
	m_hevntBuildFinish		= CreateEvent(NULL, TRUE, FALSE, "Build Finish");
	m_hevntBuildStart		= CreateEvent(NULL, TRUE, FALSE, "Build Start");	
	m_hevntAppShutdown		= CreateEvent(NULL, TRUE, FALSE, "Application Shutdown");
	m_hevntDocOpen			= CreateEvent(NULL, TRUE, FALSE, "Document Open");
	m_hevntDocClose			= CreateEvent(NULL, TRUE, FALSE, "Document Close");
	m_hevntDocSave			= CreateEvent(NULL, TRUE, FALSE, "Document Save");
	m_hevntNewDoc			= CreateEvent(NULL, TRUE, FALSE, "New Document");
	m_hevntWndActivate		= CreateEvent(NULL, TRUE, FALSE, "Window Activate");
	m_hevntWndDeactivate	= CreateEvent(NULL, TRUE, FALSE, "Window Deactivate");
	m_hevntWorkspaceOpen	= CreateEvent(NULL, TRUE, FALSE, "Workspace Opened");
	m_hevntWorkspaceClose	= CreateEvent(NULL, TRUE, FALSE, "Workspace Closed");
	m_hevntNewWorkspace		= CreateEvent(NULL, TRUE, FALSE, "New Workspace");

	// Set the interfaces to NULL to indicate the event handler is disconnected
	m_pICP = NULL;
	m_pApp = NULL;
	m_pDbg = NULL;
	m_dwConnectionID=0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
CEventHandler::~CEventHandler()
{
	// Uninitialize COM
	CoUninitialize();

	// Destroy the events
	CloseHandle(m_hevntBuildFinish);
	CloseHandle(m_hevntBuildStart);
	CloseHandle(m_hevntAppShutdown);
	CloseHandle(m_hevntDocOpen);
	CloseHandle(m_hevntDocClose);
	CloseHandle(m_hevntDocSave);
	CloseHandle(m_hevntNewDoc);
	CloseHandle(m_hevntWndActivate);
	CloseHandle(m_hevntWndDeactivate);
	CloseHandle(m_hevntWorkspaceOpen);
	CloseHandle(m_hevntWorkspaceClose);
	CloseHandle(m_hevntNewWorkspace);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include "guiv1.h" //for god damn global CLog object, I don't have time to remove them here
void CEventHandler::Connect(BOOL isDebug)
{//TODO: better HRESULT handling (throwing excepctions?)
	IUnknown*					pIUnknown=NULL;
	IConnectionPointContainer*	pICPC=NULL;

	// Get the MsDev object if we can, otherwise, fail
	// emmang@xbox - at xbox, we dont have access to debug builds of msdev
	// hres=GetActiveObject(isDebug?CLSID_DebugApp:CLSID_Application, NULL, &pIUnknown);
	hres=GetActiveObject(CLSID_Application, NULL, &pIUnknown);
	VERIFY_OK(hres);
	if(FAILED(hres) || pIUnknown==NULL)//REVIEW(chriskoz): god damn globals, I don't have time to remove them here
	{
		gpLog->RecordInfo("WARNING: Error accessing object model: %08lx", hres);
		return;
	}
#if 0 //REVIEW(chriskoz) events should be on separate thread. Otherwise "server busy" dialog pops up when test thread is stopped
	// Get an IConnectionPointContainer interface
	hres = pIUnknown->QueryInterface(
		IID_IConnectionPointContainer, 
		(void**)&pICPC
		);
	VERIFY_OK(hres);

	// Find the connection point supporting the IApplicationEvents interface
	hres = pICPC->FindConnectionPoint(
		IID_IApplicationEvents,
		&m_pICP
		);
	VERIFY_OK(hres);

	// Connect to the connection point
	VERIFY_OK(m_pICP->Advise(this, &m_dwConnectionID));
	pICPC->Release();
#endif
	VERIFY_OK(pIUnknown->QueryInterface(IID_IApplication, (void**)&m_pApp));
	pIUnknown->Release();
	pIUnknown = NULL; // emmang@xbox

	if(FAILED(hres) || m_pApp==NULL)//REVIEW(chriskoz): god damn globals, I don't have time to remove them here
	{
		// REVIEW(michma - 3/24/99): for some reason this is failing but we don't want it to cause a failure
		// in the test results. so we'll just log an error for now.
		//gpLog->RecordFailure("Error accessing IApplication object: %08lx", hres);
		gpLog->RecordInfo("ERROR accessing IApplication object: %08lx", hres);
		return;
	}

	m_pDbg = NULL;

	// when debugging a test we need to disable use of the debugger's object model by cafe.
	// otherwise any stepping operations in the target debugger will hang up cafe in the host debugger.

	// xbox - IID_Debugger->get_State() is bad
	// if (!CMDLINE->GetBooleanValue("NoDbgObjectModel", FALSE))
	if (0)
	{
		VERIFY_OK(m_pApp->get_Debugger((IDispatch**)&pIUnknown));
		if(!FAILED(hres) && pIUnknown!=NULL)
			hres=pIUnknown->QueryInterface(IID_IDebugger, (void**)&m_pDbg);
		VERIFY_OK(hres);
		if(FAILED(hres) || m_pDbg==NULL)//REVIEW(chriskoz): god damn globals, I don't have time to remove them here
			gpLog->RecordFailure("Error accessing IDebugger from object model: %08lx", hres);
	}

	if(pIUnknown)
		pIUnknown->Release();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void CEventHandler::Disconnect()
{
	// Disconnect and release all interfaces
	if(m_dwConnectionID!=0)
		VERIFY_OK(m_pICP->Unadvise(m_dwConnectionID));
	m_dwConnectionID=0;
	// Release the connection point & debugger
	if(m_pDbg){m_pDbg->Release();m_pDbg=NULL;}
	if(m_pApp){m_pApp->Release();m_pApp=NULL;}
	if(m_pICP){m_pICP->Release();m_pICP=NULL;}
	// DevStudio needs a little bit of time to become stable after disconnecting
	// If you start getting random AVs on the shutdown of MsDev, you might want
	// to try increasing this delay.
	Sleep(3000); //REVIEW(chriskoz): really such a problem? I disabled the events anyway in Connect().
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::QueryInterface
	(
	const IID& iid, 
	void** ppv
	)

	{
	// Return the appropriate interface pointer
	if(iid == IID_IUnknown)
		{
		*ppv = static_cast<IApplicationEvents*>(this);
		}
	else if(iid == IID_IDispatch)
		{
		*ppv = static_cast<IDispatch*>(this);
		}
	else if(iid == IID_IApplicationEvents)
		{
		*ppv = static_cast<IApplicationEvents*>(this);
		}
	else
		{
		// Set the interface pointer to NULL and return an error
		*ppv = NULL;
		return E_NOINTERFACE;
		}

	static_cast<IUnknown*>(*ppv)->AddRef();
	return S_OK;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
ULONG __stdcall CEventHandler::AddRef()
	{
	// Increment the reference count
	InterlockedIncrement(&m_cRef);

	return m_cRef;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
ULONG __stdcall CEventHandler::Release()
	{
	// Decrement the reference count and die if it gets to zero
	if(InterlockedDecrement(&m_cRef) == 0)
		{
		delete this;
		return 0;
		}

	return m_cRef;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	IDispatch Implementation

	This implementation DOES NOT WORK.  It is just here because IApplicationEvents 
	inherits from it.
	
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::GetIDsOfNames(
	const IID&		iid,
	OLECHAR**		szNames, 
	unsigned int	cNames, 
	LCID			lcid, 
	DISPID*			rgDispId
	)

	{
	return E_FAIL;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT  __stdcall CEventHandler::GetTypeInfo(
	unsigned int,
	LCID,
	ITypeInfo**
	)

	{
	return E_FAIL;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::GetTypeInfoCount(
	unsigned int*
	)

	{
	return E_FAIL;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::Invoke(
	DISPID,
	REFIID,
	LCID,
	WORD,
	DISPPARAMS*,
	VARIANT*,
	EXCEPINFO*,
	unsigned int*
	)

	{
	return E_FAIL;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Event handler functions

	I have used PulseEvent to signal the events by default which should work for
	most events.  If more complicated signaling logic is needed, SetEvent/ResetEvent
	pairs can be used, as they are in BuildFinish.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::BeforeBuildStart()
	{
	// In case there was a lingering build finish event set
	ResetEvent(m_hevntBuildFinish);
		
	// Signal the start of a build
	SetEvent(m_hevntBuildStart);
	return S_OK;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::BuildFinish(long nNumErrors, long nNumWarnings)
	{
	// Signal the end of a build
	SetEvent(m_hevntBuildFinish);

	return S_OK;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::BeforeApplicationShutDown()
	{
	PulseEvent(m_hevntAppShutdown);
	return S_OK;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::DocumentOpen(IDispatch* theDocument)
	{
	PulseEvent(m_hevntDocOpen);
	return S_OK;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::BeforeDocumentClose(IDispatch* theDocument)
	{
	PulseEvent(m_hevntDocClose);
	return S_OK;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::DocumentSave(IDispatch* theDocument)
	{
	PulseEvent(m_hevntDocSave);
	return S_OK;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::NewDocument(IDispatch* theDocument)
	{
	PulseEvent(m_hevntNewDoc);
	return S_OK;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::WindowActivate(IDispatch* theWindow)
	{
	PulseEvent(m_hevntWndActivate);
	return S_OK;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::WindowDeactivate(IDispatch* theWindow)
	{
	PulseEvent(m_hevntWndDeactivate);
	return S_OK;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::WorkspaceOpen()
	{
	PulseEvent(m_hevntWorkspaceOpen);
	return S_OK;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::WorkspaceClose()
	{
	PulseEvent(m_hevntWorkspaceClose);
	return S_OK;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
HRESULT __stdcall CEventHandler::NewWorkspace()
	{
	PulseEvent(m_hevntNewWorkspace);
	return S_OK;
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Wait functions

	Adding extra delays in these functions is sometimes necessary in order to allow
	things to become stable.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void CEventHandler::WaitForBuildStart()
	{
	// Collect threads here until the Build Start event is set
	WaitForSingleObjectEx(m_hevntBuildStart, INFINITE, FALSE);
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
bool CEventHandler::WaitForBuildFinish(int TimeoutInMinutes)
	{
	// Collect threads here until the Build Finish event is set
	DWORD dwResult = WaitForSingleObjectEx(
		m_hevntBuildFinish, 
		TimeoutInMinutes * 60 * 1000,  // Convert the timeout interval to milliseconds
		FALSE
		);
	ResetEvent(m_hevntBuildFinish);
	
	// Wait for everything to become consistent
	Sleep(2000);
	
	// If there was a timeout, return an error
	if(dwResult == WAIT_FAILED && GetLastError() == WAIT_TIMEOUT)
		{
		return false;
		}
	else
		{
		return true;
		}
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void CEventHandler::WaitForApplicationShutDown()
	{
	// Collect threads here until the Application Shutdown event is set
	WaitForSingleObjectEx(m_hevntAppShutdown, INFINITE, FALSE);
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void CEventHandler::WaitForDocumentOpen()
	{
	// Collect threads here until the Open Document event is set
	WaitForSingleObjectEx(m_hevntDocOpen, INFINITE, FALSE);
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void CEventHandler::WaitForDocumentClose()
	{
	// Collect threads here until the Close Document event is set
	WaitForSingleObjectEx(m_hevntDocClose, INFINITE, FALSE);
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void CEventHandler::WaitForDocumentSave()
	{
	// Collect threads here until Save Document is set
	WaitForSingleObjectEx(m_hevntDocSave, INFINITE, FALSE);
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void CEventHandler::WaitForNewDocument()
	{
	// Collect threads here until New Document is set
	WaitForSingleObjectEx(m_hevntNewDoc, INFINITE, FALSE);
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void CEventHandler::WaitForWindowActivate()
	{
	// Collect threads here until Active Window is set
	WaitForSingleObjectEx(m_hevntWndActivate, INFINITE, FALSE);
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void CEventHandler::WaitForWindowDeactivate()
	{
	// Collect threads here until Deactivate Window is set
	WaitForSingleObjectEx(m_hevntWndDeactivate, INFINITE, FALSE);
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void CEventHandler::WaitForWorkspaceOpen()
	{
	// Collect threads here until Open Workspace is set
	WaitForSingleObjectEx(m_hevntWorkspaceOpen, INFINITE, FALSE);
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void CEventHandler::WaitForWorkspaceClose()
	{
	// Collect threads here until Close Workspace is set
	WaitForSingleObjectEx(m_hevntWorkspaceClose, INFINITE, FALSE);
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void CEventHandler::WaitForNewWorkspace()
	{
	// Collect threads here until New Workspace is set
	WaitForSingleObjectEx(m_hevntNewWorkspace, INFINITE, FALSE);
	}
