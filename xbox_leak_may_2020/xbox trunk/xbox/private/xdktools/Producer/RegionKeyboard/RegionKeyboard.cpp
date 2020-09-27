// RegionKeyboard.cpp : Implementation of CRegionKeyboardApp and DLL registration.

#include "stdafx.h"
#include "RegionKeyboard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CRegionKeyboardApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x36cd3184, 0xee61, 0x11d0, { 0x87, 0x6a, 0, 0xaa, 0, 0xc0, 0x81, 0x46 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardApp::InitInstance - DLL initialization

BOOL CRegionKeyboardApp::InitInstance()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardApp::ExitInstance - DLL termination

int CRegionKeyboardApp::ExitInstance()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
