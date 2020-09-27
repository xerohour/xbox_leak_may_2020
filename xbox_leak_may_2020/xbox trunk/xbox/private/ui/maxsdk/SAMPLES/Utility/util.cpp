/**********************************************************************
 *<
	FILE: util.cpp

	DESCRIPTION:   Sample utilities

	CREATED BY: Rolf Berteig

	HISTORY: created 23 December 1995

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "util.h"

HINSTANCE hInstance;
int controlsInit = FALSE;

/** public functions **/
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) {
	hInstance = hinstDLL;

	if (!controlsInit) {
		controlsInit = TRUE;
		
		// jaguar controls
		InitCustomControls(hInstance);
		
		// initialize Chicago controls
		InitCommonControls();
		}
			
	return (TRUE);
	}


//------------------------------------------------------
// This is the interface to Jaguar:
//------------------------------------------------------

__declspec( dllexport ) const TCHAR *
LibDescription() { return GetString(IDS_RB_DEFUTIL); }

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses() {
#ifdef _DEBUG
	return 13;
#else
	return 10;
#endif
	}


__declspec( dllexport ) ClassDesc*
LibClassDesc(int i) {
	switch(i) {
		case 0: return GetColorClipDesc();
		case 1: return GetAsciiOutDesc();
		case 2: return GetCollapseUtilDesc();
		case 3: return GetRandKeysDesc();
		case 4: return GetORTKeysDesc();
		case 5: return GetSelKeysDesc();
		case 6: return GetLinkInfoUtilDesc();
		case 7: return GetCellTexDesc();
		/*case 8: return GetPipeMakerDesc();*/ //RK: 07/02/99 Removing this from Shiva
		case 8: return GetRescaleDesc();
		case 9: return GetShapeCheckDesc();
#ifdef _DEBUG
		case 10: return GetUtilTestDesc();
		case 11: return GetAppDataTestDesc();
		case 12: return GetTestSoundObjDescriptor();
#endif
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
