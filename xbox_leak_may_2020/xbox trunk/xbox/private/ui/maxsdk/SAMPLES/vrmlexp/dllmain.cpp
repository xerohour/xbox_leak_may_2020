/**********************************************************************
 *<
	FILE: dllmain.cpp

	DESCRIPTION:   DLL implementation of primitives

	CREATED BY: Charles Thaeler

        BASED on helpers.cpp

	HISTORY: created 12 February 1996

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "vrml.h"

//extern ClassDesc* GetMrBlueDesc();
extern ClassDesc* GetLODDesc();
extern ClassDesc* GetVRBLDesc();
extern ClassDesc* GetVRMLInsertDesc();
extern ClassDesc* GetVRMLMtlDesc();
extern ClassDesc *GetOmniLightDesc();
extern ClassDesc *GetTSpotLightDesc();
extern ClassDesc *GetDirLightDesc();
extern ClassDesc *GetFSpotLightDesc();
extern ClassDesc* GetPolyCounterDesc();
extern ClassDesc* GetTimeSensorDesc();
extern ClassDesc* GetNavInfoDesc();
extern ClassDesc* GetBackgroundDesc();
extern ClassDesc* GetFogDesc();
extern ClassDesc* GetAudioClipDesc();
extern ClassDesc* GetSoundDesc();
extern ClassDesc* GetTouchSensorDesc();
extern ClassDesc* GetProxSensorDesc();
extern ClassDesc* GetAnchorDesc();
extern ClassDesc* GetBillboardDesc();
extern ClassDesc* GetCppOutDesc();

HINSTANCE hInstance;
int controlsInit = FALSE;

TCHAR
*GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
}


/** public functions **/
BOOL WINAPI
DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
	hInstance = hinstDLL;

	if ( !controlsInit ) {
		controlsInit = TRUE;
		
		// jaguar controls
		InitCustomControls(hInstance);

		// initialize Chicago controls
		InitCommonControls();
		}

	switch(fdwReason) {
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		}
	return(TRUE);
}

//------------------------------------------------------
// This is the interface to MAX:
//------------------------------------------------------

__declspec( dllexport ) const TCHAR *
LibDescription() {
	return GetString(IDS_DLL_INFO);
}

#define NUM_BASE_CLASSES 14
#ifdef _DEBUG
#define NUM_CLASSES (NUM_BASE_CLASSES+1)
#else
#define NUM_CLASSES NUM_BASE_CLASSES
#endif

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses()
{
	return NUM_CLASSES;
}

__declspec( dllexport ) ClassDesc*
LibClassDesc(int i)
{
    switch(i) {
    case 0: return GetAnchorDesc();
    case 1: return GetTouchSensorDesc();
    case 2: return GetProxSensorDesc();
    case 3: return GetTimeSensorDesc();
    case 4: return GetNavInfoDesc();
    case 5: return GetBackgroundDesc();
    case 6: return GetFogDesc();
    case 7: return GetAudioClipDesc();
    case 8: return GetSoundDesc();
    case 9: return GetBillboardDesc();
    case 10: return GetLODDesc();
    case 11: return GetVRBLDesc();
    case 12: return GetVRMLInsertDesc();
    case 13: return GetPolyCounterDesc();
    //case 14: return GetMrBlueDesc();
#ifdef _DEBUG
    case NUM_BASE_CLASSES: return GetCppOutDesc();
#endif
    default: return 0;
    }
    
}

// Return version so can detect obsolete DLLs -- NOTE THIS IS THE API VERSION NUMBER
//                                               NOT THE VERSION OF THE DLL.
__declspec( dllexport ) ULONG 
LibVersion() { return VERSION_3DSMAX; }

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 1;
}

