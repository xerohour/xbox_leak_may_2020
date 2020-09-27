// autoapp.cpp : implementation file
//

#include "stdafx.h"
#include <srcapi.h>	 // For the output window
#include <srcguid.h> // Don't Initguid this!!

#include <utilauto.h>
#include "autoguid.h"
#include "autodocs.h"
#include "autoprjs.h"
#include <aut1gui_.h>	// For IAddIn
#include <aut1api_.h>	// For IAddIn
#include <bldguid_.h>	// For IAutoBld
#include <bldapi_.h>	// For IAutoBld
#include <dlgbase.h>
#include "cmduiena.h"
#include "tap.h"
#include "toolexpt.h"

#include "resource.h"
#include "autostat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef _DEBUG
/*
See UTILAUTO.H
*/
#if 0 
	// {9FD2DF21-190D-11CF-8E4D-00AA004254C4}
	static const GUID CLSID_ApplicationDebug =  
	{ 0x9fd2df21, 0x190d, 0x11cf, { 0x8e, 0x4d, 0x0, 0xaa, 0x0, 0x42, 0x54, 0xc4 }};
#endif

// {E8BEA480-DDE4-11cf-9C4A-00A0C90A632C}
static const GUID CLSID_ApplicationDebug  = 
{ 0xe8bea480, 0xdde4, 0x11cf, { 0x9c, 0x4a, 0x0, 0xa0, 0xc9, 0xa, 0x63, 0x2c } };

#define APP_CLSID CLSID_ApplicationDebug
#else
#define APP_CLSID CLSID_Application
#endif

class CAutoUIDisableRestore {
public:
	CAutoUIDisableRestore(){
		m_bState = g_AutomationState.DisplayUI();
		g_AutomationState.DisableUI();
	}
	~CAutoUIDisableRestore(){
		if(m_bState){
			g_AutomationState.EnableUI();
		}
	}
private:
	BOOL m_bState;
};

/////////////////////////////////////////////////////////////////////////////
// CApplication


IMPLEMENT_DYNCREATE(CApplication, CAutoObj)

CApplication* CApplication::m_pAppObj = NULL;
DWORD CApplication::m_dwCookie = 0xFFFFFFFF;

// Static create member creates at most one instance
CApplication* CApplication::Create()
{
	if (!m_pAppObj)
		return (m_pAppObj = new CApplication());

	m_pAppObj->ExternalAddRef();
	return m_pAppObj;
}

CApplication::CApplication()
{
	// This object should be created using the Create static
	//  member function, not by directly calling new or allocating
	//  one on the stack.
	ASSERT (m_pAppObj == NULL);

	EnableDualAutomation();
	EnableConnections();

	m_pAddIn = NULL;
	m_pBld = NULL;
	m_bZombified = FALSE;
	m_nEnableModelessLevels = 0;
	EventQueueInit();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();

	VERIFY(SUCCEEDED(RegisterActiveObject((IUnknown *) GetIDispatch(FALSE),
			APP_CLSID, 0, &m_dwCookie)));
}


void CApplication::Destroy()
{
	if (m_dwCookie != 0xFFFFFFFF)
		RevokeActiveObject(m_dwCookie, 0);
	ExternalRelease();
}


CApplication::~CApplication()
{
	// If we were already zombified, we've already unlocked the app.
	//  If we weren't zombified, we should unlock it now.  Just
	//  call Zombify and it'll know what to do.
	Zombify();

	theApp.m_pAutoApp = NULL;
	EventQueueDestroy();
	
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	AfxOleUnlockApp();
}

// Provides access to IAddIn (defined in pkgs\include\aut1api.h)
IAddIn* CApplication::GetAddInInterface()
{
	if (m_pAddIn != NULL)
		return m_pAddIn;
	
	if (FAILED(theApp.FindInterface(IID_IAddIn, (LPVOID*) &m_pAddIn))
		|| m_pAddIn == NULL)
	{
		m_pAddIn = NULL;
		return NULL;
	}
	
	return m_pAddIn;
}

// Provides access to IAddIn (defined in pkgs\include\aut1api.h)
IAutoBld* CApplication::GetBldInterface()
{
	if (m_pBld != NULL)
		return m_pBld;
	
	if (FAILED(theApp.FindInterface(IID_IAutoBld, (LPVOID*) &m_pBld))
		|| m_pBld == NULL)
	{
		m_pBld = NULL;
		return NULL;
	}
	
	return m_pBld;
}

void CApplication::Zombify()
{
	// We move the unlock app call here from the destructor, b/c before,
	//  it was never called since the order was:
	// destructor -> unlock -> allows app to exit -> package OnExit -> VbaTerminate
	//  -> destroy app object (last arrow to follow vbapp sample) = destructor.
	if (m_bZombified == TRUE)
		return;
	m_bZombified = TRUE;

	if (m_pAddIn != NULL)
		m_pAddIn->Release();

	if (m_pBld != NULL)
		m_pBld->Release();

	while (m_nEnableModelessLevels != 0)
	{
		// If a naughty macro or add-in doesn't re-enable us enough
		//  times (i.e., not calling EnableModeless(TRUE) for each
		//  EnableModeless(FALSE)), we'll do it ourselves here
		EnableModeless(TRUE);
	}
}


void CApplication::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CAutoObj::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CApplication, CAutoObj)
	//{{AFX_MSG_MAP(CApplication)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CApplication, CAutoObj)
	//{{AFX_DISPATCH_MAP(CApplication)
	DISP_PROPERTY_EX(CApplication, "Height", GetHeight, SetHeight, VT_I4)
	DISP_PROPERTY_EX(CApplication, "Width", GetWidth, SetWidth, VT_I4)
	DISP_PROPERTY_EX(CApplication, "Top", GetTop, SetTop, VT_I4)
	DISP_PROPERTY_EX(CApplication, "Left", GetLeft, SetLeft, VT_I4)
	DISP_PROPERTY_EX(CApplication, "Name", GetName, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CApplication, "Debugger", GetDebugger, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CApplication, "Version", GetVersion, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CApplication, "Path", GetPath, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CApplication, "CurrentDirectory", GetCurrentDirectory, SetCurrentDirectory, VT_BSTR)
	DISP_PROPERTY_EX(CApplication, "FullName", GetFullName, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CApplication, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CApplication, "Parent", GetParent, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CApplication, "ActiveDocument", GetActiveDocument, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CApplication, "Windows", GetWindows, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CApplication, "Documents", GetDocuments, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CApplication, "ActiveWindow", GetActiveWindow, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CApplication, "WindowState", GetWindowState, SetWindowState, VT_I4)
	DISP_PROPERTY_EX(CApplication, "TextEditor", GetTextEditor, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CApplication, "Projects", GetProjects, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CApplication, "ActiveConfiguration", GetActiveConfiguration, SetActiveConfiguration, VT_DISPATCH)
	DISP_FUNCTION(CApplication, "GetPackageExtension", GetPackageExtension, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CApplication, "Quit", Quit, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CApplication, "PrintToOutputWindow", PrintToOutputWindow, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CApplication, "ExecuteCommand", ExecuteCommand, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CApplication, "AddCommandBarButton", AddCommandBarButton, VT_EMPTY, VTS_I4 VTS_BSTR VTS_I4)
	DISP_FUNCTION(CApplication, "AddKeyBinding", AddKeyBinding, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CApplication, "Build", Build, VT_EMPTY, VTS_VARIANT)
	DISP_FUNCTION(CApplication, "RebuildAll", RebuildAll, VT_EMPTY, VTS_VARIANT)
	DISP_DEFVALUE(CApplication, "Name")
	DISP_PROPERTY_EX(CApplication, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_PROPERTY_EX(CApplication, "ActiveProject", GetActiveProject, SetActiveProject, VT_DISPATCH)
	DISP_FUNCTION(CApplication, "ExecuteConfiguration", ExecuteConfiguration, VT_EMPTY, VTS_VARIANT)
	DISP_FUNCTION(CApplication, "Clean", Clean, VT_EMPTY, VTS_VARIANT)
	DISP_PROPERTY_EX(CApplication, "Errors", GetErrors, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CApplication, "Warnings", GetWarnings, SetNotSupported, VT_I4)
	DISP_FUNCTION(CApplication, "AddProject", AddProject, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BOOL )
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CApplication, "SetAddInInfo", 0x00020001, SetAddInInfo, VT_EMPTY, VTS_I4 VTS_DISPATCH VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(CApplication, "AddCommand", 0x00020002, AddCommand, VT_BOOL, VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
	DISP_PROPERTY_EX_ID(CApplication, "Active", 0x00020003, GetActive, SetActive, VT_BOOL)
	DISP_FUNCTION_ID(CApplication, "EnableModeless", 0x00020004, EnableModeless, VT_EMPTY, VTS_BOOL)
END_DISPATCH_MAP()


BEGIN_INTERFACE_MAP(CApplication, CAutoObj)
	INTERFACE_PART(CApplication, IID_IDispApplication, Dispatch)
	INTERFACE_PART(CApplication, IID_IApplication, Dispatch)
	INTERFACE_PART(CApplication, IID_IConnectionPointContainer, ConnPtContainer)
    DUAL_ERRORINFO_PART(CApplication)
	INTERFACE_PART(CApplication, IID_IServiceProvider, ServiceProvider)
END_INTERFACE_MAP()

// Implement ISupportErrorInfo to indicate we support the 
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CApplication, IID_IApplication)
DS_IMPLEMENT_ENABLE_DUAL(CApplication, Application)
DS_DELEGATE_DUAL_INTERFACE(CApplication, Application)

BEGIN_CONNECTION_MAP(CApplication, CAutoObj)
    CONNECTION_PART(CApplication, IID_IApplicationEvents, ApplicationCP)
    CONNECTION_PART(CApplication, IID_IDispApplicationEvents, DispApplicationCP)
END_CONNECTION_MAP()

// Note: we add support for IID_DApplication to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

/* REMOVED see dualimpl.doc

// {9FD2DF20-190D-11CF-8E4D-00AA004254C4}
static const IID IID_DApplication =
{ 0x9fd2df20, 0x190d, 0x11cf, { 0x8e, 0x4d, 0x0, 0xaa, 0x0, 0x42, 0x54, 0xc4 } };
*/

/* REMOVED see dualimpl.doc                                 
BEGIN_INTERFACE_MAP(CApplication, CAutoObj)
	INTERFACE_PART(CApplication, IID_DApplication, Dispatch)
	INTERFACE_PART(CApplication, IID_IApplication, Application)
END_INTERFACE_MAP()*/

#ifdef _DEBUG

#if 0 
// {9FD2DF21-190D-11CF-8E4D-00AA004254C4}
IMPLEMENT_OLECREATE(CApplication, "MSDEVD.APPLICATION", 
					0x9fd2df21, 0x190d, 0x11cf, 0x8e, 0x4d, 0x0, 0xaa, 0x0, 0x42, 0x54, 0xc4)
#endif

// {E8BEA480-DDE4-11cf-9C4A-00A0C90A632C}
DS_IMPLEMENT_OLECREATE(CApplicationFactory, CApplication, "MSDEVD.APPLICATION", 
					0xe8bea480, 0xdde4, 0x11cf, 0x9c, 0x4a, 0x0, 0xa0, 0xc9, 0xa, 0x63, 0x2c)

#else
// {FB7FDAE2-89B8-11cf-9BE8-00A0C90A632C}
DS_IMPLEMENT_OLECREATE(CApplicationFactory, CApplication, "MSDEV.APPLICATION", 
	0xfb7fdae2, 0x89b8, 0x11cf, 0x9b, 0xe8, 0x0, 0xa0, 0xc9, 0xa, 0x63, 0x2c);
#endif

BOOL CApplicationFactory::Register()
{
	if (theApp.m_bRunInvisibly)
		return COleObjectFactory::Register();

	// Deliberately don't register class object with active running
	//  class object table, since we always want CoGetClassObject
	//  (& thus CoCreateInstance) to launch new instance of the server each time.
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Implementation of IApplication interface

DELEGATE_GET(CApplication, Application, Left,long)
DELEGATE_GET(CApplication, Application, Top,long)
DELEGATE_GET(CApplication, Application, Height,long)
DELEGATE_GET(CApplication, Application, Width,long)
DELEGATE_GET(CApplication, Application, Errors,long)
DELEGATE_GET(CApplication, Application, Warnings,long)

DELEGATE_PUT(CApplication, Application,Left,long)
DELEGATE_PUT(CApplication, Application,Top,long)
DELEGATE_PUT(CApplication, Application,Height,long)
DELEGATE_PUT(CApplication, Application,Width,long)

DELEGATE_BSTR_GET(CApplication, Application, Name, Name) 
DELEGATE_BSTR_GET(CApplication, Application, Version, Version) 
DELEGATE_BSTR_GET(CApplication, Application, Path, Path) 
DELEGATE_BSTR_GET(CApplication, Application, CurrentDirectory, CurrentDirectory) 
DELEGATE_BSTR_PUT(CApplication, Application, CurrentDirectory, CurrentDirectory) 

DELEGATE_BSTR_GET(CApplication, Application, FullName, FullName) 

DELEGATE_GET(CApplication,Application,Application,LPDISPATCH)
DELEGATE_GET(CApplication,Application,Parent,LPDISPATCH)

DELEGATE_GET(CApplication,Application,ActiveDocument,LPDISPATCH)
DELEGATE_GET(CApplication,Application,ActiveWindow,LPDISPATCH)

DELEGATE_GET(CApplication,Application,Windows,LPDISPATCH)
DELEGATE_GET(CApplication,Application,Documents,LPDISPATCH)
DELEGATE_GET(CApplication,Application,Projects,LPDISPATCH)

DELEGATE_PUT(CApplication, Application, WindowState, DsWindowState)
DELEGATE_GET(CApplication, Application, WindowState, DsWindowState)

DELEGATE_GET(CApplication,Application,Debugger,LPDISPATCH)
DELEGATE_GET(CApplication,Application,TextEditor,LPDISPATCH)

DELEGATE_GET(CApplication,Application,ActiveConfiguration,LPDISPATCH)
DELEGATE_PUT(CApplication,Application,ActiveConfiguration,LPDISPATCH)

DELEGATE_BOOL_GET(CApplication,Application,Visible)
DELEGATE_BOOL_PUT(CApplication,Application,Visible)

DELEGATE_NO_PARAMS(CApplication, Application, Quit)

DELEGATE_GET(CApplication,Application,ActiveProject,LPDISPATCH)
DELEGATE_PUT(CApplication,Application,ActiveProject,LPDISPATCH)

DELEGATE_BOOL_PUT(CApplication,Application,Active)
DELEGATE_BOOL_GET(CApplication,Application,Active)

STDMETHODIMP CApplication::XApplication::GetPackageExtension(THIS_ BSTR szExtensionName, IDispatch * FAR* pExt)
{
	METHOD_PROLOGUE(CApplication, Dispatch)
	
	TRY_DUAL(IID_IApplication)
	{
		CString strName = szExtensionName ;
		*pExt = pThis->GetPackageExtension(strName) ;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CApplication::XApplication::PrintToOutputWindow(THIS_ BSTR Message)
{
	METHOD_PROLOGUE(CApplication, Dispatch)
	
	TRY_DUAL(IID_IApplication)
	{
		CString strMessage = Message ;
		pThis->PrintToOutputWindow(strMessage) ;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CApplication::XApplication::ExecuteCommand(THIS_ BSTR szCommandName)
{
	METHOD_PROLOGUE(CApplication, Dispatch)

	CAutoUIDisableRestore disable;
	TRY_DUAL(IID_IApplication)
	{
		CString strCommandName = szCommandName ;
		pThis->ExecuteCommand(strCommandName) ;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CApplication::XApplication::AddCommandBarButton(THIS_ long nButtonType, BSTR szCmdName, long dwCookie)
{
	METHOD_PROLOGUE(CApplication, Dispatch)
	
	TRY_DUAL(IID_IApplication)
	{
		CString strCmdName = szCmdName ;
		pThis->AddCommandBarButton(nButtonType, strCmdName, dwCookie);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CApplication::XApplication::AddKeyBinding(THIS_ BSTR szKey, BSTR szCommandName, BSTR szEditor)
{
	METHOD_PROLOGUE(CApplication, Dispatch)
	
	TRY_DUAL(IID_IApplication)
	{
		CString strKey = szKey;
		CString strCommandName = szCommandName;
		CString strEditor = szEditor;
		pThis->AddKeyBinding(strKey, strCommandName, strEditor);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CApplication::XApplication::Build(THIS_ VARIANT configuration)
{
	METHOD_PROLOGUE(CApplication, Dispatch)
	
	TRY_DUAL(IID_IApplication)
	{
		pThis->Build(configuration) ;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CApplication::XApplication::RebuildAll(THIS_ VARIANT configuration)
{
	METHOD_PROLOGUE(CApplication, Dispatch)
	
	TRY_DUAL(IID_IApplication)
	{
		pThis->RebuildAll(configuration) ;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CApplication::XApplication::Clean(THIS_ VARIANT configuration)
{
	METHOD_PROLOGUE(CApplication, Dispatch)
	
	TRY_DUAL(IID_IApplication)
	{
		pThis->Clean(configuration);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CApplication::XApplication::AddProject(THIS_ BSTR szName, BSTR szPath, BSTR szType, VARIANT_BOOL bAddDefaultFolders)
{
	METHOD_PROLOGUE(CApplication, Dispatch)
	
	TRY_DUAL(IID_IApplication)
	{
		CString strProjName(szName);
		CString strProjPath(szPath);
		CString strProjType(szType);
		pThis->AddProject(strProjName, strProjPath, strProjType, bAddDefaultFolders);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CApplication::XApplication::ExecuteConfiguration(THIS_ VARIANT Reserved)
{
	METHOD_PROLOGUE(CApplication, Dispatch)
	
	TRY_DUAL(IID_IApplication)
	{
		pThis->ExecuteConfiguration(Reserved) ;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CApplication::XApplication::SetAddInInfo(THIS_ long nInstanceHandle, IDispatch * pCmdDispatch, long nIDBitmapResourceMedium, long nIDBitmapResourceLarge, long dwCookie)
{
	METHOD_PROLOGUE(CApplication, Dispatch)
	
	TRY_DUAL(IID_IApplication)
	{
		pThis->SetAddInInfo(nInstanceHandle, pCmdDispatch, 
			nIDBitmapResourceMedium, nIDBitmapResourceLarge, dwCookie);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CApplication::XApplication::AddCommand(THIS_ BSTR szCmdName, BSTR szMethodName, long nBitmapOffset, long dwCookie, VARIANT_BOOL FAR* pbResult)
{
	METHOD_PROLOGUE(CApplication, Dispatch)
	
	TRY_DUAL(IID_IApplication)
	{
		CString strCmdName = szCmdName;
		CString strMethodName = szMethodName;
		*pbResult = (pThis->AddCommand(strCmdName, strMethodName, nBitmapOffset, dwCookie)) ? -1 : 0;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CApplication::XApplication::EnableModeless(THIS_ VARIANT_BOOL bEnable)
{
	METHOD_PROLOGUE(CApplication, Dispatch)
	
	TRY_DUAL(IID_IApplication)
	{
		BOOL bEnable2 = (bEnable == VARIANT_TRUE);
		pThis->EnableModeless(bEnable2);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

/////////////////////////////////////////////////////////////////////////////
// Implementation of CApplication dispatch handlers



LPDISPATCH CApplication::GetPackageExtension(LPCTSTR szExtensionName) 
{
	return theApp.GetPackageExtension(szExtensionName);
}

LPDISPATCH CApplication::GetTextEditor() 
{
	return GetPackageExtension("TextEditor") ;
}

LPDISPATCH CApplication::GetDebugger() 
{
	return GetPackageExtension("debugger") ;
}

long CApplication::GetHeight() 
{
	CWnd* pWnd = AfxGetMainWnd(); 
	if (pWnd != NULL) 
	{
		pWnd->AssertValid();
		CRect rect;
		pWnd->GetWindowRect(&rect);
		return rect.Height();
	}
	return 0 ;
}

void CApplication::SetHeight(long nNewValue) 
{
	CWnd* pWnd = AfxGetMainWnd(); 
	if ((pWnd != NULL) &&
		(nNewValue > 0)  && 
		(nNewValue < GetSystemMetrics(SM_CYSCREEN)))
	{
		pWnd->AssertValid();
		CRect rect ;
		pWnd->GetWindowRect(&rect) ;
		pWnd->MoveWindow(rect.left, rect.top, 
					rect.Width(), nNewValue, TRUE);
	}
}

long CApplication::GetWidth() 
{
	CWnd* pWnd = AfxGetMainWnd(); 
	if (pWnd != NULL)
	{
		pWnd->AssertValid();
		CRect rect;
		pWnd->GetWindowRect(&rect);
		return rect.Width();
	}
	return 0;
}

void CApplication::SetWidth(long nNewValue) 
{
	CWnd* pWnd = AfxGetMainWnd(); 
	if ((pWnd != NULL) &&
		(nNewValue > 0)  && 
		(nNewValue < GetSystemMetrics(SM_CXSCREEN)))
	{
		pWnd->AssertValid();
		CRect rect ;
		pWnd->GetWindowRect(&rect) ;
		pWnd->MoveWindow(rect.left, rect.top, 
					nNewValue, rect.Height(), TRUE);
	}
}

long CApplication::GetTop() 
{
	CWnd* pWnd = AfxGetMainWnd(); 
	if (pWnd != NULL) 
	{
		pWnd->AssertValid();
		CRect rect;
		pWnd->GetWindowRect(&rect);
		return rect.top;
	}
	return 0 ;
}

void CApplication::SetTop(long nNewValue) 
{
	CWnd* pWnd = AfxGetMainWnd(); 
	if ((pWnd != NULL) &&
		(nNewValue > 0)  && 
		(nNewValue < GetSystemMetrics(SM_CYSCREEN)))
	{
		pWnd->AssertValid();
		CRect rect ;
		pWnd->GetWindowRect(&rect) ;
		pWnd->MoveWindow(rect.left, nNewValue, 
					rect.Width(), rect.Height(), TRUE);
	}
}

long CApplication::GetLeft() 
{
	CWnd* pWnd = AfxGetMainWnd(); 
	if (pWnd != NULL) 
	{
		pWnd->AssertValid();
		CRect rect;
		pWnd->GetWindowRect(&rect);
		return rect.left;
	}
	return 0 ;
}

void CApplication::SetLeft(long nNewValue) 
{
	CWnd* pWnd = AfxGetMainWnd(); 
	if ((pWnd != NULL) &&
		(nNewValue > 0)  && 
		(nNewValue < GetSystemMetrics(SM_CXSCREEN)))
	{
		pWnd->AssertValid();
		CRect rect ;
		pWnd->GetWindowRect(&rect) ;
		pWnd->MoveWindow(nNewValue, rect.top, 
					rect.Width(), rect.Height(), TRUE) ;
	}
}

BSTR CApplication::GetName() 
{
	CString strResult;
	//strResult = "Microsoft Developer Studio" ;
	if(strResult.LoadString(IDS_AUTOAPP_NAME))
	{
		return strResult.AllocSysString();
	}
	return NULL ;
}



BSTR CApplication::GetVersion() 
{
	CString strResult;

	//5.0
	if(strResult.LoadString(IDS_AUTOAPP_VERSION))
	{
		return strResult.AllocSysString();
	}
	return NULL ;
}


BSTR CApplication::GetPath() 
{
	const int MAX_LENGTH = 512 ;

	CString strResult;
	LPTSTR str = strResult.GetBufferSetLength(MAX_LENGTH) ;

	DWORD dwNum = ::GetModuleFileName(AfxGetApp()->m_hInstance,
									str, MAX_LENGTH);
	strResult.ReleaseBuffer() ;

	if (dwNum == 0)
	{
		TRACE0("CApplication: GetFullName failed.\r\n") ;
		return NULL ;
	}
	else
	{
		// Find ending "\" ;
		int index = strResult.ReverseFind('\\');
		if (index != -1)
		{
			// ReverseFind returns the zero based index,
			// which happens to be the count of items we want.
			strResult = strResult.Left(index) ;
		}
		return strResult.AllocSysString();
	}

}


BSTR CApplication::GetCurrentDirectory() 
{
	const int MAX_LENGTH = 512 ;
	CString strResult;

	LPTSTR str = strResult.GetBufferSetLength(MAX_LENGTH) ;

	DWORD dwNum = ::GetCurrentDirectory(MAX_LENGTH, str);
	strResult.ReleaseBuffer() ;

	if (dwNum == 0)
		TRACE0("CApplication: GetFullName failed.\r\n") ;

	return strResult.AllocSysString();
}

void CApplication::SetCurrentDirectory(LPCTSTR lpszNewValue) 
{
	if (lpszNewValue != NULL)
	{
		::SetCurrentDirectory(lpszNewValue);
	}
}

BSTR CApplication::GetFullName() 
{
	const int MAX_LENGTH = 512 ;

	CString strResult;
	LPTSTR str = strResult.GetBufferSetLength(MAX_LENGTH) ;

	DWORD dwNum = ::GetModuleFileName(AfxGetApp()->m_hInstance,
									str, MAX_LENGTH);
	strResult.ReleaseBuffer() ;

	if (dwNum == 0)
	{
		TRACE0("CApplication: GetFullName failed.\r\n") ;
		return NULL ;
	}
	
	return strResult.AllocSysString();
}


LPDISPATCH CApplication::GetApplication() 
{
	ASSERT(m_pAppObj != NULL) ;
    return m_pAppObj->GetIDispatch(TRUE);
}


LPDISPATCH CApplication::GetParent() 
{
	ASSERT(m_pAppObj != NULL) ;
    return m_pAppObj->GetIDispatch(TRUE);
}


LPDISPATCH CApplication::GetActiveDocument() 
{
	// See - PSS ID Number: Q108587

	CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)(AfxGetApp()->m_pMainWnd);
	if (pMainFrame != NULL)
	{
		ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) ;

		CMDIChildWnd* pDocFrame = pMainFrame->MDIGetActive() ;
		if (pDocFrame != NULL)
		{
			CPartDoc* pDoc = (CPartDoc*)pDocFrame->GetActiveDocument() ;
			if (pDoc != NULL)
			{
				ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CPartDoc))) ;
				pDoc->AssertValid() ;
				return pDoc->GetAutomationObject(TRUE) ;
			}
		}
	}
	return NULL;

}


LPDISPATCH CApplication::GetWindows() 
{
	// Create the Windows automation object.
	CAutoWindows* pAutoWins = new CAutoWindows(NULL);

	// Get and return the IDispatch pointer for CAutoDocuments, but don't addref.
	return pAutoWins->GetIDispatch(FALSE);
}

LPDISPATCH CApplication::GetDocuments() 
{
	// Create the Documents automation object.
	CAutoDocuments* pAutoDocs = new CAutoDocuments();

	// Get and return the IDispatch pointer for CAutoDocuments, but don't addref.
	return pAutoDocs->GetIDispatch(FALSE);
}


LPDISPATCH CApplication::GetActiveWindow() 
{
	// get a pointer to the main frame
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));

	CPartFrame* pChildWnd = (CPartFrame*)pMainFrame->MDIGetActive() ;
	if (pChildWnd != NULL)
	{
		ASSERT(pChildWnd->IsKindOf(RUNTIME_CLASS(CPartFrame))) ;
		pChildWnd->AssertValid() ;
		return pChildWnd->GetAutomationObject(TRUE) ;
	}

	return NULL;
}

void CApplication::Quit() 
{
	// See filecmds.cpp | CVshellPackage::OnFileSaveAllExit

	// Save all files and then quit.
	theApp.SaveAll(FALSE, FILTER_DEBUG);

	// Quit
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0L);
}

DsWindowState CApplication::GetWindowState() 
{
	// Main window can't docking or float.
	DsWindowState state = dsWindowStateNormal ;

	CWnd* pMainWnd = AfxGetMainWnd();
	if (pMainWnd->IsIconic())
	{
		state = dsWindowStateMinimized ;
	}
	else if (pMainWnd->IsZoomed())
	{
		state = dsWindowStateMaximized ;
	}

	return state;
}


void CApplication::SetWindowState(DsWindowState nNewValue) 
{
	int nCmdShow ;

	switch(nNewValue)
	{
	case dsWindowStateMaximized :
	    nCmdShow = SW_SHOWMAXIMIZED ;
		break;
	case dsWindowStateMinimized :
		nCmdShow = SW_MINIMIZE ;
		break;
	case dsWindowStateNormal:
		nCmdShow = SW_SHOWNORMAL;
		break;
	default:
		// Main window cannot be docked or floated.
		// TODO: ERROR
		return ;
	}
	AfxGetMainWnd()->ShowWindow(nCmdShow) ;
}

UINT CApplication::s_idOutputWindow = 0;
BOOL CApplication::s_bOutputWindow = FALSE;

void CApplication::PrintToOutputWindow(LPCTSTR strMessage) 
{
	LPOUTPUTWINDOW pI;
	HRESULT hr = theApp.FindInterface(IID_IOutputWindow, (void**)&pI);
	if (SUCCEEDED(hr))
	{
		if (!s_bOutputWindow)
		{
			// Create the output window tab.
			pI->OutputWindowAddVwin("Macro", FALSE, &s_idOutputWindow);
			s_bOutputWindow = TRUE ;
		}
		
		// Output the string.
		pI->OutputWindowQueueHit(s_idOutputWindow, strMessage, TRUE, TRUE, FALSE);
		pI->Release() ;
	}
}

// Execute any arbitrary command, by name
void CApplication::ExecuteCommand(LPCTSTR szCommandName) 
{
	ASSERT_VALID(this);

	UINT nCmdID=0;
	CAutoUIDisableRestore disable;

	// search the command well (inefficient, but essential)
	if(theCmdCache.GetCommandID(szCommandName, &nCmdID))
	{
		// Special case: For DevStudio 96 bug 21643, we're rerouting 
		//  ExecuteCommand("ApplicationExit") to Application::Quit.  Otherwise
		//  we crash thrice.
		if (nCmdID == ID_APP_EXIT)
		{
			Quit();
			return;
		}

		// Check to see if the command has been disabled before calling its update handler
		CTE* pCTE = theCmdCache.GetCommandEntry(nCmdID);
		BOOL bEnabled = (pCTE == NULL || (pCTE->flags & (CT_NOUI | CT_NOKEY)) == 0);
		
		if (bEnabled)
		{
			CCmdUIEnable state;
			state.m_nID = nCmdID;
			state.DoUpdate(AfxGetApp()->m_pMainWnd, TRUE);
			bEnabled = state.m_bEnabled;
		}

		if (bEnabled)
		{
			// Shouldn't be able to ExecuteCommand while we're in the
			//  middle of an existing asynchronous command
			ASSERT (theApp.GetCurrentAsyncCmdLevel() == 0);

			// Fire off command
			theApp.m_pMainWnd->SendMessage(WM_COMMAND, MAKEWPARAM((WORD)nCmdID, 1));

			// If this is an asynchronous command, we need to push our
			//  own message pump until the command is really finished.
			//  For example, ToolsCustomize displays a modeless window, disables
			//  the IDE, and then returns.  We want ExecuteCommand to wait until
			//  the window is dismissed before it returns, though.  So we must
			//  push our own message pump onto the stack here, and when we
			//  receive word that the dialog was dismissed, only then can we
			//  return to the caller.

			// Pump until command is finished (for synchronous commands, this
			//  won't even loop)
			while (theApp.GetCurrentAsyncCmdLevel() != 0)
				theApp.IdleAndPump();
		}
	}
	else
	{
		DsThrowOleDispatchException(E_INVALIDARG, IDS_AUTOAPP_INVALIDCOMMAND);
	}

	// FUTURE: We should consider a FindCommandId/ExecuteCommandId pair. FindCommand could find the
	// command id that goes with the string, and then ExecuteCommandId could execute it. 
}

void CApplication::AddKeyBinding(LPCTSTR szKey, LPCTSTR szCommandName, LPCTSTR szEditor) 
{
	ASSERT_VALID(this);

	UINT nCmdID=0;

	// search the command well 
	if(theCmdCache.GetCommandID(szCommandName, &nCmdID))
	{
		// work out which keymap is being modified
		POSITION pos = CKeymap::c_keymaps.GetHeadPosition();
		BOOL bFound=FALSE;
		CKeymap* pKeymap=NULL;
		while (pos != NULL)
		{
			pKeymap = (CKeymap*)(void*)CKeymap::c_keymaps.GetNext(pos);
			if (pKeymap->m_strName==szEditor)
			{
				bFound=TRUE;
				break;
			}
		}
		
		if(!bFound)
		{
			DsThrowOleDispatchException(E_INVALIDARG, IDS_AUTOAPP_INVALIDEDITOR);
		}

		//translate the key binding into a key code 
		WORD kcPrime=0;
		WORD kcChord=0;
		BOOL bChord=FALSE;
		int iOffset=0;

		kcPrime=::GetKeyCodeFromName(szKey,&iOffset);

		if(kcPrime==0)
		{
			DsThrowOleDispatchException(E_INVALIDARG, IDS_AUTOAPP_BADFIRSTKEY);
		}
		
		if(szKey[iOffset]!=(TCHAR)0) {
			bChord=TRUE;
			
			kcChord=GetKeyCodeFromName(szKey,&iOffset);

			if(kcChord==0)
			{
				DsThrowOleDispatchException(E_INVALIDARG, IDS_AUTOAPP_BADSECONDKEY);
			}
		
			if(szKey[iOffset]!=(TCHAR)0)
			{
				DsThrowOleDispatchException(E_INVALIDARG, IDS_AUTOAPP_TOOMANYKEYS);
				// error
			}
		}

		// bind the command to the code
		if (bChord)
		{
			pKeymap->SetKey(kcPrime, kcChord, nCmdID);
		}
		else
		{
			pKeymap->SetKey(kcPrime, nCmdID);
		}
		
		pKeymap->m_bDirty = TRUE;
	}
	else
	{
		DsThrowOleDispatchException(E_INVALIDARG, IDS_AUTOAPP_INVALIDCOMMAND);
	}
}


//
//
//

BOOL CApplication::GetActive() 
{
	CWnd* pWnd = AfxGetMainWnd(); 
	HWND hWndFore = ::GetForegroundWindow() ;
	return (hWndFore == pWnd->m_hWnd) ;
}

void CApplication::SetActive(BOOL bNewValue) 
{
	BOOL bActive = GetActive() ;
	if (bActive == bNewValue)
	{
		// Already in the correct state.
		return ;
	}

	// Change state.
	CWnd* pWnd = AfxGetMainWnd(); 
	if (bNewValue)
	{
		// Activate the window.
		VERIFY(pWnd->SetForegroundWindow()) ;
	}
	else
	{
		// Deactivate the window by activating the next window in the Z-order
		CWnd* pNextWnd = pWnd->GetWindow(GW_HWNDNEXT);
		if(pNextWnd == NULL) // we got to the end of the list
		{
			// Get the first window.
			pNextWnd  = pWnd->GetWindow(GW_HWNDFIRST);
		}

		// Only activate if it is different than the current window.
		if (pNextWnd != pWnd)
		{
			VERIFY(pNextWnd->SetForegroundWindow()) ;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// Add-In methods delegate to devaut1 via IAddIn...

void CApplication::SetAddInInfo(long nInstanceHandle, LPDISPATCH pCmdDispatch, long nIDBitmapResourceMedium, long nIDBitmapResourceLarge, long dwCookie) 
{
	// Get access to devaut1 package
	IAddIn* pAddIn = GetAddInInterface();
	if (pAddIn == NULL)
	{
		DsThrowOleDispatchException(E_UNEXPECTED, IDS_AUTOAPP_NODEVAUT1);
	}

	// If this fails, it will call DsThrowOleDispatchException for us
	pAddIn->SetAddInInfo(nInstanceHandle, pCmdDispatch,
		nIDBitmapResourceMedium, nIDBitmapResourceLarge, dwCookie);
}

BOOL CApplication::AddCommand(LPCTSTR szCmdName, LPCTSTR szMethodName, long nBitmapOffset, long dwCookie) 
{
	// Get access to devaut1 package
	IAddIn* pAddIn = GetAddInInterface();
	if (pAddIn == NULL)
	{
		DsThrowOleDispatchException(E_UNEXPECTED, IDS_AUTOAPP_NODEVAUT1);
	}

	// If this fails, it will call DsThrowOleDispatchException for us
	return (pAddIn->AddCommand(szCmdName, szMethodName, nBitmapOffset, dwCookie) == S_OK);
}

void CApplication::EnableModeless(BOOL bEnable)
{
	// Orion Bug 8416 - CMainFrame::OnActivateApp is asserting when we should down
	// with an BeforeDocumentClose macro which contains a MsgBox. This is because
	// this message attempts to activate an app which is hidden and which we want
	// to stay hidden.
	if (theApp.m_pMainWnd->IsWindowVisible())
	{
		if (bEnable)
		{
			if (m_nEnableModelessLevels == 0)
				// An Enable w/out a Disable?  I don't THINK so.s
				return;

			AfxGetApp()->EnableModeless(TRUE);
			// Reset the dockmgr state
			PostModalWindow();

			m_nEnableModelessLevels--;

			// If we're down to zero, make sure any queued events are
			//  thrown now.
			if (m_nEnableModelessLevels == 0)
				EventQueueRemoveAll();
		}
		else
		{
			PreModalWindow();
			// allow OLE servers to disable themselves
			AfxGetApp()->EnableModeless(FALSE);

			// Remember we're being disabled so if a naughty macro or
			//  add-in doesn't re-enable us enough times, we can do that
			//  on zombification.
			m_nEnableModelessLevels++;
		}
	}
}

void CApplication::AddCommandBarButton(long nButtonType, LPCTSTR szCmdName, long dwCookie) 
{
	// Get access to devaut1 package
	IAddIn* pAddIn = GetAddInInterface();
	if (pAddIn == NULL)
	{
		DsThrowOleDispatchException(E_UNEXPECTED, IDS_AUTOAPP_NODEVAUT1);
	}

	// If this fails, it will call DsThrowOleDispatchException for us
	pAddIn->AddCommandBarButton(nButtonType, szCmdName, dwCookie);
}


IMPLEMENT_ADDREF		(CApplication, ServiceProvider)
IMPLEMENT_RELEASE		(CApplication, ServiceProvider)
IMPLEMENT_QUERYINTERFACE(CApplication, ServiceProvider)

HRESULT CApplication::XServiceProvider::QueryService(
	REFGUID guidService, REFIID iid, void ** ppvObj)
{
	ASSERT(NULL != ppvObj);
	METHOD_PROLOGUE(CApplication, ServiceProvider)
	return theApp.GetService(guidService, iid, ppvObj);
}

// BUILD SYSTEM members defer to build package via IAutoBld

long CApplication::GetErrors()
{
	IAutoBld* pBld = GetBldInterface();
	if (pBld == NULL)
	{
		DsThrowOleDispatchException(E_UNEXPECTED, IDS_AUTOAPP_NOBLD);
	}

	long nErrs;
	pBld->Errors(nErrs);
	return nErrs;
}

long CApplication::GetWarnings()
{
	IAutoBld* pBld = GetBldInterface();
	if (pBld == NULL)
	{
		DsThrowOleDispatchException(E_UNEXPECTED, IDS_AUTOAPP_NOBLD);
	}

	long nWarns;
	pBld->Warnings(nWarns);
	return nWarns;
}

LPDISPATCH CApplication::GetProjects() 
{
	LPDISPATCH pRet;
	CAutoProjects *pProjects = CAutoProjects::Create();
	pRet = pProjects->GetIDispatch(FALSE);
	return pRet;
}

void CApplication::Build(const VARIANT FAR& configuration) 
{
	IAutoBld* pBld = GetBldInterface();
	if (pBld == NULL)
	{
		DsThrowOleDispatchException(E_UNEXPECTED, IDS_AUTOAPP_NOBLD);
	}
	
	HRESULT hr = pBld->Build(configuration, FALSE);
	if (FAILED(hr))
		DsThrowOleDispatchException(hr, IDS_AUTOAPP_NOBLDNOW);
}

void CApplication::RebuildAll(const VARIANT FAR& configuration) 
{
	IAutoBld* pBld = GetBldInterface();
	if (pBld == NULL)
	{
		DsThrowOleDispatchException(E_UNEXPECTED, IDS_AUTOAPP_NOBLD);
	}
	
	HRESULT hr = pBld->Build(configuration, TRUE);
	if (FAILED(hr))
		DsThrowOleDispatchException(hr, IDS_AUTOAPP_NOBLDNOW);
}

void CApplication::Clean(const VARIANT FAR& configuration)
{
	IAutoBld* pBld = GetBldInterface();
	if (pBld == NULL)
	{
		DsThrowOleDispatchException(E_UNEXPECTED, IDS_AUTOAPP_NOBLD);
	}
	
	HRESULT hr = pBld->Clean(configuration);
	if (FAILED(hr))
		DsThrowOleDispatchException(hr, IDS_AUTOAPP_NOBLDNOW);
}

void CApplication::AddProject(LPCTSTR strName, LPCTSTR strPath, LPCTSTR strType, VARIANT_BOOL bAddDefaultFolders)
{
	IAutoBld* pBld = GetBldInterface();
	if (pBld == NULL)
	{
		DsThrowOleDispatchException(E_UNEXPECTED, IDS_AUTOAPP_NOBLD);
	}
	
	HRESULT hr = pBld->AddProject(strName, strPath, strType, bAddDefaultFolders);
	if (FAILED(hr))
		DsThrowOleDispatchException(hr, IDS_AUTOAPP_CANTADDPROJECT);

}

LPDISPATCH CApplication::GetActiveConfiguration() 
{
	IAutoBld* pBld = GetBldInterface();
	if (pBld == NULL)
	{
		DsThrowOleDispatchException(E_UNEXPECTED, IDS_AUTOAPP_NOBLD);
	}
	
	LPDISPATCH pRet;
	pBld->GetDefaultConfiguration(&pRet);
	return pRet;
}

void CApplication::SetActiveConfiguration(LPDISPATCH newValue) 
{
	IAutoBld* pBld = GetBldInterface();
	if (pBld == NULL)
	{
		DsThrowOleDispatchException(E_UNEXPECTED, IDS_AUTOAPP_NOBLD);
	}
	
	pBld->SetDefaultConfiguration(newValue);
}


LPDISPATCH CApplication::GetActiveProject() 
{
	IProjectWorkspace *pProjWks = g_IdeInterface.GetProjectWorkspace();
	IPkgProject *pPP;
	VERIFY(SUCCEEDED(pProjWks->GetActiveProject(&pPP)));
	if (pPP == NULL)
	{
		DsThrowOleDispatchException(E_UNEXPECTED, IDS_AUTOAPP_NOPROJECT);
	}
	LPDISPATCH pDisp = NULL;
	pPP->GetAutomationObject(&pDisp);
	pPP->Release();
	return pDisp;
}

void CApplication::SetActiveProject(LPDISPATCH newValue) 
{
	if(newValue==NULL)
		DsThrowOleDispatchException(E_INVALIDARG, IDS_AUTOAPP_INVALID_PROJ);

	// Let's make sure that we have a project automation object
	COleRef<IGenericProject> pProject;
	if(FAILED(newValue->QueryInterface(IID_IGenericProject, (void **)&pProject)))
		DsThrowOleDispatchException(E_INVALIDARG, IDS_AUTOAPP_INVALID_PROJ);

	// We are going to compare the names of the internal projects and find
	// the one that we should make active. This is OK because the names of the
	// projects in a workspace are unique.
	BSTR bstrNewProjName;
	VERIFY(SUCCEEDED(pProject->get_Name(&bstrNewProjName)));
	IProjectWorkspace *pPW = g_IdeInterface.GetProjectWorkspace();
	ASSERT(pPW != NULL);
	CADWORD PkgProjects;
	IPkgProject *pProjMatch = NULL;
	VERIFY(SUCCEEDED(pPW->GetLoadedProjects(&PkgProjects)));
	for(int i=0; i<PkgProjects.cElems; i++)
	{
		LPCOLESTR wstrName, wstrPath;
		DWORD dwFlags, dwPackID;
		((IPkgProject *)PkgProjects.pElems[i])->GetProjectInfo(&wstrName, 
							&wstrPath, &dwPackID, &dwFlags);
		if(wcscmp(bstrNewProjName, wstrName) == 0)
		{
			pProjMatch = (IPkgProject *)PkgProjects.pElems[i];
			pProjMatch->AddRef();
		}

		((IPkgProject *)PkgProjects.pElems[i])->Release();
		::CoTaskMemFree((void *)wstrName);
		::CoTaskMemFree((void *)wstrPath);
		if(pProjMatch != NULL)
			break;
	}
	CoTaskMemFree((void *)PkgProjects.pElems);
	::SysFreeString(bstrNewProjName);
	
	if(pProjMatch == NULL)
		DsThrowOleDispatchException(E_UNEXPECTED, IDS_AUTOAPP_INVALID_PROJ);

	pPW->SetActiveProject(pProjMatch, FALSE);
	pProjMatch->Release();
}

void CApplication::ExecuteConfiguration(const VARIANT FAR& configuration) 
{
	if (configuration.vt == VT_ERROR)
	{
		// Default passes no params--just build active config
		CWnd* pWnd = AfxGetMainWnd();
		if (pWnd == NULL)
		{
			ASSERT(FALSE);
			return;
		}

		pWnd->SendMessage(WM_COMMAND, MAKEWPARAM(IDM_PROJECT_EXECUTE, 1));
	}

	// FUTURE: If configuration is non-empty, then execute that specific configuration
	//  instead of the active one.
}

BOOL CApplication::GetVisible() 
{
	CWnd* pWnd = AfxGetMainWnd();
	if (pWnd == NULL)
		DsThrowCannedOleDispatchException(E_UNEXPECTED);

	return pWnd->IsWindowVisible();
}

void CApplication::SetVisible(BOOL bNewValue) 
{
	CMainFrame* pWnd = static_cast<CMainFrame*>(AfxGetMainWnd());
	ASSERT_KINDOF(CMainFrame, pWnd) ;

	if (pWnd == NULL)
	{
		DsThrowCannedOleDispatchException(E_UNEXPECTED);
	}

	if (bNewValue == FALSE && pWnd->IsWindowVisible() == TRUE)
	{
		// If we're becoming invisible, remember if we were just maximized
		m_bMaximizeApp = theApp.m_bMaximizeApp;
	}
	else if (bNewValue == TRUE)
	{
		// If launched by automation but suddenly made visible, then user
		//  is in control
		AfxOleSetUserCtrl(TRUE);
	}

	// If we're making the app visible, be sure to get its maximized state right,
	//  by seeing whether it was maximized when it was last made invisible.
	pWnd->ShowWindow(bNewValue ? (m_bMaximizeApp ? SW_SHOWMAXIMIZED : SW_SHOW)
							   : SW_HIDE);

	// Bug 14790 --- The stupid docking windows are WM_POPUPs so they do not hide when the 
	// main frame hides. Therefore, we need to explicitly hide and unhide them.
	// If m_bRunInvisibly is TRUE, ShowManager will not call ShowPalettes and the
	// palette (ie floating windows) will not get updated.
	theApp.m_bRunInvisibly = FALSE; // Always set to false before the next call.
	pWnd->m_pManager->ShowManager(bNewValue, updNow, TRUE);
	theApp.m_bRunInvisibly = !bNewValue; 
}


// BEGIN_PATHETICNESS [davbr]
// The following code is required to convince the linker/compiler/whatever
//  to reference the Application's connection point interfaces.  Without
//  this code, we'll get unresolved externals for the vector deleteting
//  destructors.  Don't ask, I have no idea.
void ForceVectors()
{
	CApplication::XApplicationCP *pClass = new CApplication::XApplicationCP[2];
	CApplication::XDispApplicationCP *pClass2 = new CApplication::XDispApplicationCP[2];
}

static void (*pFunc)() = ForceVectors;
// END_PATHETICNESS
