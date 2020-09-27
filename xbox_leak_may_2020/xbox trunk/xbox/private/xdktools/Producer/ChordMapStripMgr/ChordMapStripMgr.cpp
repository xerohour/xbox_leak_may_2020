// ChordMapStripMgr.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//		To build a separate proxy/stub DLL, 
//		run nmake -f ChordMapStripMgrps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include "initguid.h"
#include "ChordMapStripMgr.h"

#include "ChordMapMgr.h"
#include <ChordMapDesigner.h>
#include <SegmentGuids.h>
#include "AFXCTL.H"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_ChordMapMgr, CChordMapMgr)
END_OBJECT_MAP()

/*class CChordMapStripMgrApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};
*/

CChordMapStripMgrApp theApp;

BOOL CChordMapStripMgrApp::InitInstance()
{
	_Module.Init(ObjectMap, m_hInstance);
	AfxEnableControlContainer();
	return CWinApp::InitInstance();
}

int CChordMapStripMgrApp::ExitInstance()
{
	_Module.Term();
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	// registers object
	return _Module.RegisterServer(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//AfxOleUnregisterTypeLib( LIBID_ChordMapSTRIPMGRLib );
	_Module.UnregisterServer(FALSE);
	return S_OK;
}


