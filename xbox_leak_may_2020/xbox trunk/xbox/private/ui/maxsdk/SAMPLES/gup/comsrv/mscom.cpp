//-----------------------------------------------------------------------------
// --------------------
// File ....: mscom.cpp
// --------------------
// Author...: Gus J Grubba
// Date ....: October 1998
// Descr....: MS COM/DCOM Server for 3D Studio MAX
//
// History .: Oct, 1 1998 - Started
//            
//-----------------------------------------------------------------------------

//-- Include files

#include <Max.h>
#include <bmmlib.h>
#include <guplib.h>

#define _IN_MSCOM_
#include "mscom.h"

//-- Globals ------------------------------------------------------------------

HINSTANCE hInst = NULL;

extern bool StartServer	( HINSTANCE hInstance, HINSTANCE MaxhInstance, int registerCOM );
extern void StopServer	( );
extern LPCTSTR FindToken(LPCTSTR p1);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-- DLL Declaration

BOOL WINAPI DllMain(HINSTANCE hDLLInst, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
		 case DLL_PROCESS_ATTACH:
				if (hInst)
					return(FALSE);
				hInst = hDLLInst;
				break;
		 case DLL_PROCESS_DETACH:
				hInst  = NULL;
				break;
		 case DLL_THREAD_ATTACH:
				break;
		 case DLL_THREAD_DETACH:
				break;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
//-- Resource String Helper

TCHAR *GetString(int id) {
	static TCHAR buf[256];
	if (hInst)
		return LoadString(hInst, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
}

int             mClassDesc::IsPublic     ( )		{ return 1; }
void           *mClassDesc::Create       ( BOOL )	{ return new GUP_MSCOM; }
const TCHAR    *mClassDesc::ClassName    ( )		{ return _T("MaxRenderer COM Server"); }
SClass_ID       mClassDesc::SuperClassID ( )		{ return GUP_CLASS_ID; }
Class_ID        mClassDesc::ClassID      ( )		{ return Class_ID(470000002,0); }
const TCHAR    *mClassDesc::Category     ( )		{ return _T("Global Utility PlugIn"); }

static mClassDesc cdesc;

//-----------------------------------------------------------------------------
// Interface

DLLEXPORT const TCHAR * LibDescription ( )  { 
	return _T("MaxRenderer COM server global utility (Kinetix)"); 
}

DLLEXPORT int LibNumberClasses ( ) { 
	return 1; 
}

DLLEXPORT ClassDesc *LibClassDesc(int i) {
	switch(i) {
		case  0: return &cdesc;	break;
		default: return 0;		break;
	}
}

DLLEXPORT ULONG LibVersion ( )  { 
	return ( VERSION_3DSMAX ); 
}

//-----------------------------------------------------------------------------
// #> GUP_MSCOM::GUP_MSCOM()

GUP_MSCOM::GUP_MSCOM( ) { 

}

GUP_MSCOM::~GUP_MSCOM ( ) {

}

//-----------------------------------------------------------------------------
// #> GUP_MSCOM::Start()

DWORD GUP_MSCOM::Start( ) {
	
	bool toregister		= false;
	bool tounregister	= false;

	LPTSTR lpCmdLine;
	lpCmdLine = GetCommandLine();
	LPCTSTR lpszToken = FindToken(lpCmdLine);
	while (lpszToken != NULL) {
		if (lstrcmpi(lpszToken,_REGISTERCOM)==0) {
			toregister = true;
			break;
		}
		if (lstrcmpi(lpszToken,_UNREGISTERCOM)==0) {
			tounregister = true;
			break;
		}
		lpszToken = FindToken(lpszToken);
	}

	//-----------------------------------------------------
	//-- If not registered

	if (!IsCOMRegistered()) {

		//-- We are registering ourselves
		
		if (toregister) {
			StartServer(hInst,MaxInst(),2);
			return GUPRESULT_ABORT;
		}
		
		//-- Not registered and not registering, no need to
		//   stay around.

		return GUPRESULT_NOKEEP;
	
	//-----------------------------------------------------
	//-- If registered
	
	} else {

		//-- Unregistering COM/DCOM server

		if (tounregister) {
			StartServer(hInst,MaxInst(),1);
			return GUPRESULT_ABORT;
		}

	}

	//-----------------------------------------------------
	//-- Normal Operation
	
	StartServer(hInst,MaxInst(),0);	
	return GUPRESULT_KEEP;
}

//-----------------------------------------------------------------------------
// #> GUP_MSCOM::Stop()

void GUP_MSCOM::Stop( ) {
	if (IsCOMRegistered())
		StopServer();
}

//-----------------------------------------------------------------------------
// #> GUP_MSCOM::Control()

DWORD GUP_MSCOM::Control( DWORD parameter ) {
	switch (parameter) {
		case 0:	return (DWORD)IsCOMRegistered();
		case 1:	return (DWORD)RegisterCOM();
		case 2:	return (DWORD)UnRegisterCOM();
	}
	return 0;
}

//-----------------------------------------------------------------------------
// #> GUP_MSCOM::IsCOMRegistered()

bool GUP_MSCOM::IsCOMRegistered( ) {
	TCHAR szKeyName[256];
	TCHAR clsidApp[] = {"{4AD72E6E-5A4B-11D2-91CB-0060081C257E}"};
	wsprintf(szKeyName,"CLSID\\%s",clsidApp);
	HKEY	hKey;
	long retVal = RegOpenKeyEx(HKEY_CLASSES_ROOT,szKeyName,0,KEY_READ,&hKey);
	if (retVal == ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// #> GUP_MSCOM::RegisterCOM()

bool GUP_MSCOM::RegisterCOM( ) {
	if (IsCOMRegistered())
		return false;
	bool res = StartServer(hInst,MaxInst(),2);
	return res;
}

//-----------------------------------------------------------------------------
// #> GUP_MSCOM::UnRegisterCOM()

bool GUP_MSCOM::UnRegisterCOM( ) {
	bool res = StartServer(hInst,MaxInst(),1);
	return res;
}

//-- EOF: mscom.cpp -----------------------------------------------------------
