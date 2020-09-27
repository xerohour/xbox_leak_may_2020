/**********************************************************************
 *<
	FILE: mods2.cpp

	DESCRIPTION:  Converts a mesh to a lattice Utility Files
	              Also Contains SplineSelect
				  This version contains RandMatl Modifier

	CREATED BY: Audrey Peterson
				RandMatl by Eric Peterson

	HISTORY: created 6 January, 1997

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/
#include "mods2.h"

HINSTANCE hInstance;
static int controlsInit = FALSE;


//------------------------------------------------------
// This is the interface to Jaguar:
//------------------------------------------------------

__declspec( dllexport ) const TCHAR *
LibDescription() { return GetString (IDS_LIB_DESCRIPTION); }

// This function returns the number of plug-in classes this DLL implements
__declspec( dllexport ) int 
LibNumberClasses() { return 4; }

// This function return the ith class descriptor.
__declspec( dllexport ) ClassDesc* 
LibClassDesc(int i) {
	switch(i){
	case 0:return GetLatticeModDesc();
	case 1:return GetSplineSelModDesc();
	case 2:return GetRandMatlModDesc();
	case 3:return GetSpindleModDesc();
    default:return 0;}
 }

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
	{	
	// Hang on to this DLL's instance handle.
	hInstance = hinstDLL;

  	if (!controlsInit) {
		controlsInit = TRUE;		

	// Initialize MAX's custom controls
	InitCustomControls(hInstance);
		
	// Initialize Win95 controls
	InitCommonControls();
	}
	
	return(TRUE);
	}


// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG 
LibVersion() { return VERSION_3DSMAX; }

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 1;
}

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

int FloatEQ0(float number)
{return((FLOAT_EPSILON>=number)&&(number>=-FLOAT_EPSILON));
}
