/**********************************************************************
 *<
    FILE: ctrl.cpp

    DESCRIPTION:   DLL implementation of some controllers

    CREATED BY: Rolf Berteig

    HISTORY: created 13 June 1995

	         added independent scale controller (ScaleXYZ)
			 see file "./indescale.cpp"
			   mjm 9.15.98

 *> Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "ctrl.h"
#include "buildver.h"

HINSTANCE hInstance;
int controlsInit = FALSE;

/** public functions **/
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) {
    hInstance = hinstDLL;

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

        SetDefaultBoolController(GetBoolControlDesc());
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
// This is the interface to Jaguar:
//------------------------------------------------------

__declspec( dllexport ) const TCHAR *
LibDescription() { return
 GetString(IDS_RB_DEFCONTROLLERS); }


#ifndef DESIGN_VER

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses() {return 23;} // mjm 9.15.98

__declspec( dllexport ) ClassDesc*
LibClassDesc(int i) {
    switch(i) {
        case 0: return GetPathCtrlDesc();
        case 1: return GetEulerCtrlDesc();
        case 2: return GetLocalEulerCtrlDesc();
        case 3: return GetFloatNoiseDesc();
        case 4: return GetExprPosCtrlDesc();
        case 5: return GetExprP3CtrlDesc();
        case 6: return GetExprFloatCtrlDesc();
        case 7: return GetExprScaleCtrlDesc();
        case 8: return GetPositionNoiseDesc();
        case 9: return GetPoint3NoiseDesc();
        case 10: return GetRotationNoiseDesc();
        case 11: return GetScaleNoiseDesc();
        case 12: return GetBoolControlDesc();
        case 13: return GetIPosCtrlDesc();
        case 14: return GetAttachControlDesc();
        case 15: return GetIPoint3CtrlDesc();
        case 16: return GetIColorCtrlDesc();
        case 17: return GetLinkCtrlDesc();
        case 18: return GetFollowUtilDesc();
        case 19: return GetSurfCtrlDesc();
        case 20: return GetLODControlDesc();
        case 21: return GetLODUtilDesc();
		case 22: return GetIScaleCtrlDesc(); // mjm 9.15.98

//      case 7: return GetExprRotCtrlDesc();
        default: return 0;
        }
    }

#else

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses() { return 19; } // mjm 9.15.98

__declspec( dllexport ) ClassDesc*
LibClassDesc(int i) {
    switch(i) {
        case 0: return GetPathCtrlDesc();
        case 1: return GetEulerCtrlDesc();
        case 2: return GetLocalEulerCtrlDesc();
        case 3: return GetFloatNoiseDesc();
//      case 4: return GetExprPosCtrlDesc();
//      case 5: return GetExprP3CtrlDesc();
//      case 6: return GetExprFloatCtrlDesc();
//      case 7: return GetExprScaleCtrlDesc();
        case 4: return GetPositionNoiseDesc();
        case 5: return GetPoint3NoiseDesc();
        case 6: return GetRotationNoiseDesc();
        case 7: return GetScaleNoiseDesc();
        case 8: return GetBoolControlDesc();
        case 9: return GetIPosCtrlDesc();
        case 10: return GetAttachControlDesc();
        case 11: return GetIPoint3CtrlDesc();
        case 12: return GetIColorCtrlDesc();
        case 13: return GetLinkCtrlDesc();
        case 14: return GetFollowUtilDesc();
        case 15: return GetSurfCtrlDesc();
        case 16: return GetLODControlDesc();
        case 17: return GetLODUtilDesc();
		case 18: return GetIScaleCtrlDesc(); // mjm 9.15.98

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

    if(hInstance)
        return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
    return NULL;
}

