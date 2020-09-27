/**********************************************************************
 *<
	FILE: mtl.cpp

	DESCRIPTION:   DLL implementation of material and textures

	CREATED BY: Dan Silva

	HISTORY: created 12 December 1994

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "mtlhdr.h"
#include "stdmat.h"
#include "mtlres.h"

HINSTANCE hInstance;
int controlsInit = FALSE;

/** public functions **/
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) {
	hInstance = hinstDLL;

	if ( !controlsInit ) {
		controlsInit = TRUE;
		
		// jaguar controls
		InitCustomControls(hInstance);

		// initialize Chicago controls
		InitCommonControls();

		// register SXP readers
		RegisterSXPReader(_T("MARBLE_I.SXP"), Class_ID(MARBLE_CLASS_ID,0));
		RegisterSXPReader(_T("NOISE_I.SXP"),  Class_ID(NOISE_CLASS_ID,0));
		RegisterSXPReader(_T("NOISE2_I.SXP"), Class_ID(NOISE_CLASS_ID,0));
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
// This is the interface to Max:
//------------------------------------------------------

__declspec( dllexport ) const TCHAR *
LibDescription() { return GetString(IDS_DS_MTLDESC); }

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses() 
{
#ifndef DESIGN_VER
	return 31;
#else
	return 29;
#endif // !DESIGN_VER
}

__declspec( dllexport ) ClassDesc*
LibClassDesc(int i) {
	switch(i) {
#ifdef USE_STDMTL2_AS_STDMTL
		case  0: return GetStdMtl2Desc();
#else
		case  0: return GetStdMtlDesc();
#endif
		case  1: return GetMultiDesc();
		case  2: return GetCMtlDesc();
		case  3: return GetBMTexDesc();
		case  4: return GetMaskDesc();
		case  5: return GetTintDesc();
		case  6: return GetCheckerDesc();
		case  7: return GetMixDesc();
		case  8: return GetMarbleDesc();
		case  9: return GetNoiseDesc();
		case 10: return GetTexmapsDesc();
		case 11: return GetDoubleSidedDesc();
		case 12: return GetMixMatDesc();
		case 13: return GetACubicDesc();
		case 14: return GetMirrorDesc();
		case 15: return GetGradientDesc();
		case 16: return GetCompositeDesc();
		case 17: return GetMatteDesc();
		case 18: return GetRGBMultDesc();
		case 19: return GetOutputDesc();
		case 20: return GetFalloffDesc();
		case 21: return GetVColDesc();
		
//		case 22: return GetConstantShaderCD();
		case 22: return GetPhongShaderCD();
		case 23: return GetBlinnShaderCD();
		case 24: return GetOldBlinnShaderCD();
		case 25: return GetMetalShaderCD();

#ifndef DESIGN_VER
		case 26: return GetPartBlurDesc();
		case 27: return GetPlateDesc();
		case 28: return GetPartAgeDesc();
		case 29: return GetOldTexmapsDesc();
		case 30: return GetCompositeMatDesc();

#else // Design Version
		case 26: return GetPlateDesc();
		case 27: return GetOldTexmapsDesc();
		case 28: return GetCompositeMatDesc();

#endif // DESIGN_VER

		default: return 0;
		}
	}

// Return version so can detect obsolete DLLs
__declspec( dllexport ) ULONG 
LibVersion() { return VERSION_3DSMAX; }

TCHAR *GetString(int id)
{
	static TCHAR buf[256];
	if(hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
}
