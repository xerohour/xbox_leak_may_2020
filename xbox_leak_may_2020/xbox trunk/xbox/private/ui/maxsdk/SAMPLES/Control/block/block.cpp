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

#include "block.h"
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
LibDescription() { return _T("Block controller (Kinetix)"); }


#ifndef DESIGN_VER

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses() {return 8;} // mjm 9.15.98

__declspec( dllexport ) ClassDesc*
LibClassDesc(int i) {
    switch(i) {
        case 0: return GetMasterBlockDesc();
        case 1: return GetBlockControlDesc();
        case 2: return GetSlaveFloatDesc();
        case 3: return GetSlavePosDesc();
		case 4: return GetControlContainerDesc();
        case 5: return GetSlaveRotationDesc();
        case 6: return GetSlaveScaleDesc();
        case 7: return GetSlavePoint3Desc();
        default: return 0;
        }
    }

#else

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses() { return 8; } // mjm 9.15.98

__declspec( dllexport ) ClassDesc*
LibClassDesc(int i) {
    switch(i) {
        case 0: return GetMasterBlockDesc();
        case 1: return GetBlockControlDesc();
        case 2: return GetSlaveFloatDesc();
        case 3: return GetSlavePosDesc();
		case 4: return GetControlContainerDesc();
        case 5: return GetSlaveRotationDesc();
        case 6: return GetSlaveScaleDesc();
        case 7: return GetSlavePoint3Desc();
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

