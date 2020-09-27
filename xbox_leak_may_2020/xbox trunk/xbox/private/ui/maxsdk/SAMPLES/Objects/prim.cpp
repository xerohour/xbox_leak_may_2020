/**********************************************************************
 *<
	FILE: prim.cpp

	DESCRIPTION:   DLL implementation of primitives

	CREATED BY: Dan Silva

	HISTORY: created 12 December 1994

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "prim.h"
#include "3dsurfer.h"
#include "buildver.h"

HINSTANCE hInstance;
int controlsInit = FALSE;
SurferPatchDataReaderCallback patchReader;
SurferSplineDataReaderCallback splineReader;

/** public functions **/
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) {
	hInstance = hinstDLL;
	static call=0;

	if ( !controlsInit ) {
		controlsInit = TRUE;
		
		// jaguar controls
		InitCustomControls(hInstance);

#ifdef OLD3DCONTROLS
		// initialize 3D controls
		Ctl3dRegister(hinstDLL);
		Ctl3dAutoSubclass(hinstDLL);
#endif
		
		// initialize Chicago controls
		InitCommonControls();
		}

	if (call==0) {
		RegisterObjectAppDataReader(&patchReader);
		RegisterObjectAppDataReader(&splineReader);
		call++;
		}

	switch(fdwReason) {
		case DLL_PROCESS_ATTACH:
			//MessageBox(NULL,_T("PRIM.DLL: DllMain)",_T("Prim"),MB_OK);
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
// This is the interface to Jaguar:
//------------------------------------------------------

__declspec( dllexport ) const TCHAR *
LibDescription() { return GetString(IDS_RB_PRIMLIB); }

#ifndef DESIGN_VER

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses() {return 40;}

__declspec( dllexport ) ClassDesc*
LibClassDesc(int i) {
	switch(i) {
		case 0: return GetBoxobjDesc();
		case 1: return GetSphereDesc();
		case 2: return GetCylinderDesc();
		case 3: return GetLookatCamDesc();
		case 4: return GetSimpleCamDesc();
		case 5: return GetTargetObjDesc();

		case 6: return GetTSpotLightDesc();
		case 7: return GetFSpotLightDesc();
		case 8: return GetTDirLightDesc();
		case 9: return GetDirLightDesc();
		case 10: return GetOmniLightDesc();

		case 11: return GetSplineDesc();
		case 12: return GetNGonDesc();
		case 13: return GetDonutDesc();
		case 14: return GetBonesDesc();
		case 15: return GetRingMasterDesc();
		case 16: return GetSlaveControlDesc();
		case 17: return GetQuadPatchDesc();
		case 18: return GetTriPatchDesc();
		case 19: return GetTorusDesc();
		case 20: return GetMorphObjDesc();
		case 21: return GetCubicMorphContDesc();
		case 22: return GetRectangleDesc();
		case 23: return GetBoolObjDesc();
		case 24: return GetTapeHelpDesc();
		case 25: return GetTubeDesc();
		case 26: return GetConeDesc();
		case 27: return GetHedraDesc();
		case 28: return GetCircleDesc();
		case 29: return GetEllipseDesc();
		case 30: return GetArcDesc();
		case 31: return GetStarDesc();
		case 32: return GetHelixDesc();
		case 33: return GetRainDesc();
		case 34: return GetSnowDesc();
		case 35: return GetTextDesc();
		case 36: return GetTeapotDesc();
		case 37: return GetBaryMorphContDesc();
		case 38: return GetProtHelpDesc();
		case 39: return GetGridobjDesc();
		default: return 0;
		}

	}

#else

//
// DESIGN VERSION EXCLUDES SOME PLUG_INS
//

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses() {return 32;}


__declspec( dllexport ) ClassDesc*
LibClassDesc(int i) {
	switch(i) {
		case 0: return GetBoxobjDesc();
		case 1: return GetSphereDesc();
		case 2: return GetCylinderDesc();
		case 3: return GetLookatCamDesc();
		case 4: return GetSimpleCamDesc();
		case 5: return GetTargetObjDesc();

		case 6: return GetTSpotLightDesc();
		case 7: return GetFSpotLightDesc();
		case 8: return GetTDirLightDesc();
		case 9: return GetDirLightDesc();
		case 10: return GetOmniLightDesc();

		case 11: return GetSplineDesc();
		case 12: return GetNGonDesc();
		case 13: return GetDonutDesc();
		//case 14: return GetBonesDesc();
		//case 15: return GetRingMasterDesc();
		//case 16: return GetSlaveControlDesc();
		case 14: return GetQuadPatchDesc();
		case 15: return GetTriPatchDesc();
		case 16: return GetTorusDesc();
		//case 20: return GetMorphObjDesc();
		//case 21: return GetCubicMorphContDesc();
		case 17: return GetRectangleDesc();
		case 18: return GetBoolObjDesc();
		case 19: return GetTapeHelpDesc();
		case 20: return GetTubeDesc();
		case 21: return GetConeDesc();
		case 22: return GetHedraDesc();
		case 23: return GetCircleDesc();
		case 24: return GetEllipseDesc();
		case 25: return GetArcDesc();
		case 26: return GetStarDesc();
		case 27: return GetHelixDesc();
		//case 33: return GetRainDesc();
		//case 34: return GetSnowDesc();
		case 28: return GetTextDesc();
		//case 36: return GetTeapotDesc();
		case 29: return GetBaryMorphContDesc();
		case 30: return GetProtHelpDesc();
		case 31: return GetGridobjDesc();

		default: return 0;
		}

	}

#endif



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
