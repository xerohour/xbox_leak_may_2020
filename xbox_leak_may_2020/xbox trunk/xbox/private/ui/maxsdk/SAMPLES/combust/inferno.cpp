/**********************************************************************
 *<
	FILE: inferno.cpp

	DESCRIPTION: DllMain is in here

	CREATED BY: Rolf Berteig

	HISTORY: created 4/15/96

 *>	Copyright (c) 1996 Rolf Berteig, All Rights Reserved.
 **********************************************************************/

#include "inferno.h"

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
LibDescription() { return GetString(IDS_RB_COMBUST_INFO); }

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses() {return 1;}


__declspec( dllexport ) ClassDesc*
LibClassDesc(int i) {
	switch(i) {
		case 0: return GetExplodeDesc();		
		default: return 0;
		}

	}

// Return version so can detect obsolete DLLs
__declspec( dllexport ) ULONG 
LibVersion() { return VERSION_3DSMAX; }


TCHAR *GetString(int id)
	{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
	}
