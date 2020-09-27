//+-------------------------------------------------------------------------
//  Microsoft Windows

// History:
//  03/11/2000  danhaff     created.
//  05/04/2000 - danhaff - split into separate file.
//===========================================================================
#pragma once
 
#include "globals.h"

/********************************************************************************
********************************************************************************/
template <class T>
HRESULT tmplGetObjectInPath(CtIDirectMusicPerformance8 *ptPerf8,                          
                          DWORD dwPathType,
                          DWORD dwStage,
                          GUID *pguidObject,
                          GUID *pguidInterface,
                          T* pDoNotUseDoNotRemove = NULL)
{
	HRESULT						hr				    = S_OK;
    void                        *pVoid              = NULL;    
    IUnknown                    *pUnknown           = NULL;
    IUnknown                    *pUnkVerify         = NULL;
    T                           *ptTestInterface    = NULL;
    CICMusic                     Music;   
    DWORD                        dwBuffer           = 0;

    CHECKRUN(Music.Init(ptPerf8, g_szDefaultMedia, dwPathType));
    CHECKRUN(Music.GetInterface(&ptTestInterface));
    dwBuffer = 0;
    
    // Get the object out of the audio path.
    CHECKRUN(ptTestInterface->GetObjectInPath(
            IgnorePChannel(dwStage),   
            dwStage,           
            dwBuffer,          
            *pguidObject,    
            0,               //dwIndex
            *pguidInterface, 
            (void **)&pVoid));

    //Wait a sec, to catch any mixer problems.
    CHECK(Sleep(1000));

  
    //TODO: Call the last method on each interface.  This test sucks, write another one.


    //Verify it's the correct type by QI'ing
    CHECK(pUnknown = (IUnknown *)pVoid);

    // Cleanup objects and uninitialize and Return results
    SAFE_RELEASE(pUnkVerify);
    SAFE_RELEASE(pUnknown);
    SAFE_RELEASE(ptTestInterface);
	return hr;
}	


