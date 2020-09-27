	/**********************************************************************
	 *<
		FILE: ffdmod.cpp

		DESCRIPTION: DllMain is in here

		CREATED BY: Rolf Berteig

		HISTORY: created 7/22/96

	 *>	Copyright (c) 1996 Rolf Berteig, All Rights Reserved.
	 **********************************************************************/

////////////////////////////////////////////////////////////////////
//
// Free Form Deformation Patent #4,821,214 licensed 
// from Viewpoint DataLabs Int'l, Inc., Orem, UT
// www.viewpoint.com
// 
////////////////////////////////////////////////////////////////////

#include "ffdmod.h"

HINSTANCE hInstance;
static int controlsInit = FALSE;


/** public functions **/
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) {
	hInstance = hinstDLL;

	if (!controlsInit) {
		controlsInit = TRUE;		
		
		// MAXontrols
		InitCustomControls(hInstance);
		
		// initialize Chicago controls
		InitCommonControls();		
		}	

	return(TRUE);
	}


//------------------------------------------------------
// This is the interface to MAX
//------------------------------------------------------

__declspec( dllexport ) const TCHAR *
LibDescription() { return GetString(IDS_RB_FFDMOD); }

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses() {return 9;}


__declspec( dllexport ) ClassDesc*
LibClassDesc(int i) {
	switch(i) {
		case 0: return GetFFDDesc44();		
		case 1: return GetFFDDesc33();
		case 2: return GetFFDDesc22();
		case 3: return GetFFDNMSquareOSDesc();
		case 4: return GetFFDNMSquareWSDesc();
		case 5: return GetFFDNMSquareWSModDesc();
		case 6: return GetFFDNMCylOSDesc();
		case 7: return GetFFDNMCylWSDesc();
		case 8: return GetFFDSelModDesc();
		default: return 0;
		}

	}

// Return version so can detect obsolete DLLs
__declspec( dllexport ) ULONG 
LibVersion() { return VERSION_3DSMAX; }


// Loads a string from the resource into a static buffer.
TCHAR *GetString(int id)
	{
	static TCHAR buf[256];
	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
	}

BOOL CALLBACK DefaultSOTProc(
		HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
	IObjParam *ip = (IObjParam*)GetWindowLong(hWnd,GWL_USERDATA);

	switch (msg) {
		case WM_INITDIALOG:
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			if (ip) ip->RollupMouseMessage(hWnd,msg,wParam,lParam);
			return FALSE;

		default:
			return FALSE;
		}
	return TRUE;
	}
