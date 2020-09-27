 //+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctperf8.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// ctperf8.cpp
//
// Test harness implementation of IDirectMusicPerformance8
//
// Functions:
//   CtIDirectMusicPerformance8::InitAudio()
//   CtIDirectMusicPerformance8::PlaySegmentEx()
//   CtIDirectMusicPerformance8::StopEx()
//   CtIDirectMusicPerformance8::ClonePMsg()
//   CtIDirectMusicPerformance8::CreateAudioPath()
//   CtIDirectMusicPerformance8::CreateStandardAudioPath()
//   CtIDirectMusicPerformance8::SetDefaultAudioPath()
//   CtIDirectMusicPerformance8::GetDefaultAudioPath()
//   CtIDirectMusicPerformance8::SetParamHook()
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================

#include "globals.h"
#include "DMClient.h"
#include "trans.h"
#include "create.h"

//---------------------------------------------------------------------------




//===========================================================================
// CtIDirectMusicPerformance8::CtIDirectMusicPerformance8()
///
// Default constructor
//===========================================================================
CtIDirectMusicPerformance8::CtIDirectMusicPerformance8(void)

{
    //Say my name!    
    strcpy(m_szClassName, &(typeid(this).name())[8]);
    m_szClassName[strlen(m_szClassName) - 2] = NULL;  //cut off the " *"
} // *** end CtIDirectMusicPerformance8::CtIDirectMusicPerformance8()



/*
//===========================================================================
//===========================================================================
CtIDirectMusicPerformance8::~CtIDirectMusicPerformance8(void)
{






} // *** end CtIDirectMusicPerformance8::~CtIDirectMusicPerformance8()
*/


//===========================================================================
// CtIDirectMusicPerformance8::InitTestClass
//===========================================================================
/*
HRESULT CtIDirectMusicPerformance8::InitTestClass(void)
{

    DWORD dwParameters = 0;
    LPVOID *ppv = NULL;
    HRESULT hr = S_OK;
    HRESULT hrCall = S_OK;
    LPDATABLOCK *ppDataBlocks = NULL;

    //Write to the file notifying it of this test class's creation.
    TRANS *pTrans = NULL;
    ALLOCATEANDCHECK(pTrans, TRANS);
    CHECKRUN(pTrans->Client_CreateParameterDataFile());
    
    //No in parameters, one "out" parameter. No THIS ptr
    CHECKRUN(pTrans->WriteMainBlockHeader(NULL, "Create_IDirectMusicPerformance8", 0));  
    
    //No data chunks, since we're just creating.
    CHECKRUN(pTrans->WriteParamBlockHeader(0));  

    //Close out the file.  This will cause it to be sent to the XBox.
    CHECKRUN(pTrans->CloseDataFile());

    //Now wait for the returned datafile.
    CHECKRUN(pTrans->Client_ReadReturnedDataFile());    

    //Get the number of parameters
    CHECKRUN(pTrans->ReadMainBlockHeader(NULL, NULL, &dwParameters));

    if (2!=dwParameters)
        Output("IDirectMusicPerformance8 creation didn't returned unexpected number of paramters");

    //Get a pointer to all the parameters.
    CHECKRUN(pTrans->ReadParameters(dwParameters, &ppDataBlocks));

    //Store the XBox pointer and return code here.
    //Parameter         (TYPE)                         [blk]
    //--------------------------------------------------------------------
    hrCall          = *(HRESULT *)ppDataBlocks         [0]    [0].pvData;
    m_pvXBoxPointer = *(LPVOID *) ppDataBlocks         [1]    [0].pvData;

    CHECKRUN(pTrans->DeleteAllocatedParameterList(dwParameters, ppDataBlocks));

    return hrCall;


} // *** end CtIDirectMusicPerformance8::InitTestClass()


*/
//===========================================================================
// CtIDirectMusicPerformance8::InitAudio()
//
// Encapsulates calls to InitAudio
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::InitAudioX(
	DWORD dwDefaultPathType,                 // Requested default audio path type, also optional.
	DWORD dwPChannelCount)                   // Number of PChannels, if default audio path to be created.
{
    DWORD dwParameters = 0;
    LPDATABLOCK *ppDataBlocks = NULL;
    HRESULT hr = S_OK;
    HRESULT hrCall = S_OK;

    //Write to the file notifying it of this test class's creation.
    TRANS *pTrans = NULL;
    ALLOCATEANDCHECK(pTrans, TRANS);
    CHECKRUN(pTrans->Client_CreateParameterDataFile());
    
    //3 In-parameters (we're ignoring the first 3 parameters).
    CHECKRUN(pTrans->WriteMainBlockHeader(m_pvXBoxPointer, "IDirectMusicPerformance8::InitAudio", 5));  
    
    //PARAMETER 1: THIS!
    //1 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(LPVOID), &m_pvXBoxPointer));  

    //PARAMETER 2: dwDefaultPathType
    //1 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(dwDefaultPathType), &dwDefaultPathType));  

    //PARAMETER 3: dwPChannelCount
    //1 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(dwPChannelCount), &dwPChannelCount));  

    //Close out the file.  This will cause it to be sent to the XBox.
    CHECKRUN(pTrans->CloseDataFile());

    //Now wait for the returned datafile.
    CHECKRUN(pTrans->Client_ReadReturnedDataFile());    

    //There should be 1 out-paramter
    //  1) Return Value
    CHECKRUN(pTrans->ReadMainBlockHeader(NULL, NULL, &dwParameters));

    if (1!=dwParameters)
        Output("IDirectMusicPerformance8::InitAudio returned %d paramters instead of 1", dwParameters);

    //Get a pointer to all the parameters.
    CHECKRUN(pTrans->ReadParameters(dwParameters, &ppDataBlocks));

    //Store the return code here.
    hrCall          = *(HRESULT *)ppDataBlocks         [0]    [0].pvData;

    CHECKRUN(pTrans->DeleteAllocatedParameterList(dwParameters, ppDataBlocks));
    CHECKRUN(pTrans->CloseDataFile());
    SAFEDELETE(pTrans);
    return hrCall;

} // *** end CtIDirectMusicPerformance8::InitAudio()


//===========================================================================
// CtIDirectMusicPerformance8::PlaySegmentEx()
//
// Encapsulates calls to PlaySegmentEx
//
// History:
//  10/13/1999 - kcraven - created
//  01/17/2000 - kcraven - updated
//  02/02/2000 - kcraven - fixed logging string
//===========================================================================
HRESULT CtIDirectMusicPerformance8::PlaySegmentEx(CtIUnknown* ptSource,                  // Segment to play. Alternately, could be an IDirectMusicSong.
                                           CHAR *pzSegmentName,                       // If song, which segment in the song.
                                           CtIUnknown* ptTransition,                     // Optional template segment to compose transition with.
                                           DWORD dwFlags,                               // DMUS_SEGF_ flags.
                                           __int64 i64StartTime,                        // Time to start playback.
                                           CtIDirectMusicSegmentState** pptSegmentState, // Returned Segment State.
                                           CtIUnknown *ptFrom,                           // Optional segmentstate or audiopath to replace.
                                           CtIUnknown *ptAudioPath)                      // Optional audioPath to play on.
{
// ********************* SEND THE DATA DOWN
    DWORD dwParameters = 0;
    LPDATABLOCK *ppDataBlocks = NULL;
    HRESULT hr = S_OK;
    HRESULT hrCall = S_OK;
    CtIDirectMusicSegmentState *ptSegmentState = NULL;
    IDirectMusicSegmentState *pSegmentState     = NULL;

    IUnknown *pSource       = NULL;
    IUnknown *pTransition   = NULL;
    IUnknown *pFrom         = NULL;
    IUnknown *pAudioPath    = NULL;

    //Get the real pointers.
    if (ptSource)
        ptSource->GetRealObjPtr((void **)&pSource);
    if (ptTransition)
        ptTransition->GetRealObjPtr((void **)&pTransition);
    if (ptFrom)
        ptFrom->GetRealObjPtr((void **)&pFrom);
    if (ptAudioPath)
        ptAudioPath->GetRealObjPtr((void **)&pAudioPath);

    //Write to the file notifying it of this test class's creation.
    TRANS *pTrans = NULL;
    ALLOCATEANDCHECK(pTrans, TRANS);
    CHECKRUN(pTrans->Client_CreateParameterDataFile());
    
    //9 In-parameters .
    CHECKRUN(pTrans->WriteMainBlockHeader(m_pvXBoxPointer, "IDirectMusicPerformance8::PlaySegmentEx", 9));  
    
    //PARAMETER 1: THIS!
    //1 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(LPVOID), &m_pvXBoxPointer));  

    //PARAMETER 2: pSource
    //1 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(pSource), &pSource));  

    //PARAMETER 3: pwzSegmentName
    //2 data chunks.
    CHECKRUN(pTrans->WriteParamBlockHeader(2));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(pzSegmentName), &pzSegmentName));  
    if (pzSegmentName)
    {
        CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, strlen(pzSegmentName) + 1, pzSegmentName));  
    }
    else
    {
        CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, 8, (void *)"XXXXXXXX"));  
    }


    //PARAMETER 4: pTransition
    //1 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(pTransition), &pTransition));  

    //PARAMETER 5: dwFlags
    //1 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(dwFlags), &dwFlags));  

    //PARAMETER 6: i64StartTime
    //1 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(i64StartTime), &i64StartTime));  

    //PARAMETER 7: ppSegmentState (the only purpose of this parameter is to tell the host whether to return one or not).
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(pSegmentState), &pptSegmentState));  

    //PARAMETER 8: pFrom
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(pFrom), &pFrom));  

    //PARAMETER 9: pAudioPath
    //BUGBUG: NULL for now, until we wrap the audiopath interface.
    //PARAMETER 9: pAudioPath
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(pAudioPath), &pAudioPath));  

    //Close out the file.  This will cause it to be sent to the XBox.
    CHECKRUN(pTrans->CloseDataFile());

    //Now wait for the returned datafile.
    CHECKRUN(pTrans->Client_ReadReturnedDataFile());    

    CHECKRUN(pTrans->ReadMainBlockHeader(NULL, NULL, &dwParameters));

    //There should be 2 out-paramters
    if (2!=dwParameters)
        Output("IDirectMusicPerformance8::InitAudio returned %d paramters instead of 2", dwParameters);

    //Get a pointer to all the parameters.
    CHECKRUN(pTrans->ReadParameters(dwParameters, &ppDataBlocks));

    //  1) Return Value
    //Store the return code here.
    hrCall          = *(HRESULT *)ppDataBlocks         [0]    [0].pvData;

    //  2) SegmentState interface.  If we passed in NULL, this should be NULL to signify that a value couldn't be returned.
    //     Of course it could also be NULL due to an error.
    pSegmentState = *(IDirectMusicSegmentState **)ppDataBlocks[1][0].pvData;

    //Wrap the segment state interface if necessary.
    if (pptSegmentState)
    {

        hr = CreateInstance(pSegmentState, &ptSegmentState);
        *pptSegmentState = ptSegmentState;
    }
    else
    {
        if (pSegmentState)
        {
            Output("CtIDirectMusicPerformance8::PlaySegmentEx() Error: A segment state was returned but none was asked for.");
        }
    }

    CHECKRUN(pTrans->DeleteAllocatedParameterList(dwParameters, ppDataBlocks));
    CHECKRUN(pTrans->CloseDataFile());
    SAFEDELETE(pTrans);
    return hrCall;

} //*** end CtIDirectMusicPerformance8::PlaySegmentEx()



HRESULT CtIDirectMusicPerformance8::CloseDown()
{
    DWORD dwParameters = 0;
    LPVOID *ppv = NULL;
    HRESULT hr = S_OK;
    HRESULT hrCall = S_OK;
    LPDATABLOCK *ppDataBlocks = NULL;
    char szString[MAX_PATH];


    //Write to the file notifying it of this test class's creation.
    TRANS *pTrans = NULL;
    ALLOCATEANDCHECK(pTrans, TRANS);
    CHECKRUN(pTrans->Client_CreateParameterDataFile());
    
    //One parameter - the this pointer.
    sprintf(szString, "IDirectMusicPerformance8::CloseDown", m_szClassName);
    CHECKRUN(pTrans->WriteMainBlockHeader(NULL, szString, 1));  
    
    //PARAMETER 1: THIS!
    //One data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(LPVOID), &m_pvXBoxPointer));  

    //Close out the file.  This will cause it to be sent to the XBox.
    CHECKRUN(pTrans->CloseDataFile());

    //Now wait for the returned datafile.
    CHECKRUN(pTrans->Client_ReadReturnedDataFile());    

    //Get the number of parameters
    CHECKRUN(pTrans->ReadMainBlockHeader(NULL, NULL, &dwParameters));

    if (1!=dwParameters)
        Output("IUnknown::CloseDown returned %d parameters intead of 1", dwParameters);

    //Get a pointer to all the parameters.
    CHECKRUN(pTrans->ReadParameters(dwParameters, &ppDataBlocks));

    //Store the XBox pointer and return code here.
    //Parameter         (TYPE)                         [blk]
    //--------------------------------------------------------------------
    hrCall = *(DWORD *)  ppDataBlocks         [0]    [0].pvData;
    CHECKRUN(pTrans->DeleteAllocatedParameterList(dwParameters, ppDataBlocks));

    CHECKRUN(pTrans->CloseDataFile());
    SAFEDELETE(pTrans);

return hrCall;

}



/*

//===========================================================================
// CtIDirectMusicPerformance8::StopEx()
//
// Encapsulates calls to StopEx
//
// History:
//  01/17/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::StopEx
(
	CtIUnknown *pObjectToStop,
	__int64 i64StopTime,
	DWORD dwFlags
)
{
    HRESULT     hRes					= E_NOTIMPL;
    BOOL		fValid_pObjectToStop	= TRUE;
	IUnknown 	*pdmObjectToStop 		= NULL;

    // validate pObjectToStop
    if(!helpIsValidPtr((void*)pObjectToStop, sizeof(CtIUnknown), FALSE))
    {
        fValid_pObjectToStop = FALSE;
        pdmObjectToStop = (IUnknown*)pObjectToStop;
    }
    else
    {
        // good pointer, get the real object
        hRes = pObjectToStop->GetRealObjPtr(&pdmObjectToStop);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }


    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::StopEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pObjectToStop == %p   %s",
            pdmObjectToStop,
            fValid_pObjectToStop ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "i64StopTime   == %016Xh", //BUGBUG - can't print 64 bit numbers in fnshell
            i64StopTime);
    fnsLog(PARAMLOGLEVEL, "dwFlags       == %08Xh",
            dwFlags);

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->StopEx(pdmObjectToStop,i64StopTime,dwFlags);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::StopEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
 END:
    if(fValid_pObjectToStop && pdmObjectToStop)
    {
        pdmObjectToStop->Release();
    }
   return hRes;

} // *** end CtIDirectMusicPerformance8::StopEx()




//===========================================================================
// CtIDirectMusicPerformance8::ClonePMsg()
//
// Encapsulates calls to ClonePMsg
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::ClonePMsg
(
	DMUS_PMSG* pSourcePMSG,
	DMUS_PMSG** ppCopyPMSG
)
{
    HRESULT     hRes						= E_NOTIMPL;
    BOOL		fValid_pSourcePMSG				= TRUE;
    BOOL		fValid_ppCopyPMSG				= TRUE;

    // validate pSourcePMSG
    if(!helpIsValidPtr((void*)pSourcePMSG, sizeof(DMUS_PMSG*), FALSE))
    {
        fValid_pSourcePMSG = FALSE;
    }

    // validate ppCopyPMSG
    if(!helpIsValidPtr((void*)ppCopyPMSG, sizeof(DMUS_PMSG*), FALSE))
    {
        fValid_ppCopyPMSG = FALSE;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::ClonePMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSourcePMSG == %p   %s",
            pSourcePMSG,
            fValid_pSourcePMSG ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "ppCopyPMSG  == %p   %s",
            ppCopyPMSG,
            fValid_ppCopyPMSG ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->ClonePMsg(pSourcePMSG,ppCopyPMSG);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::ClonePMsg()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
    return hRes;

} // *** end CtIDirectMusicPerformance8::ClonePMsg()




//===========================================================================
// CtIDirectMusicPerformance8::CreateAudioPath()
//
// Encapsulates calls to CreateAudioPath
//
// History:
//  01/24/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::CreateAudioPath(
	CtIUnknown *pSourceConfig, 
	BOOL fActivate,
	CtIDirectMusicAudioPath **ppNewPath)
{
    HRESULT					hRes					= E_NOTIMPL;
    BOOL					fValid_pSourceConfig	= TRUE;
    BOOL					fValid_ppNewPath		= TRUE;
	IUnknown*				pdmSourceConfig 		= NULL;
    IDirectMusicAudioPath*	pdmNewPath				= NULL;
    IDirectMusicAudioPath**	ppdmNewPath				= NULL;


    // validate pSourceConfig
    if(!helpIsValidPtr((void*)pSourceConfig, sizeof(CtIUnknown), FALSE))
    {
        fValid_pSourceConfig = FALSE;
        pdmSourceConfig = (IUnknown*)pSourceConfig;
    }
    else
    {
        // good pointer, get the real object
        hRes = pSourceConfig->GetRealObjPtr(&pdmSourceConfig);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    // validate ppNewPath
    if(!helpIsValidPtr((void*)ppNewPath, sizeof(CtIDirectMusicAudioPath*),
                        FALSE))
    {
        // bogus pointer, use as such
        fValid_ppNewPath = FALSE;
        ppdmNewPath = (IDirectMusicAudioPath**)ppNewPath;

    }
    else
    {
        // valid pointer, create a real object
        ppdmNewPath = &pdmNewPath;

        // just in case we fail, init test object ptr to NULL
        *ppNewPath = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::CreateAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pSourceConfig == %p   %s",
            pdmSourceConfig,
            fValid_pSourceConfig ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "fActivate     == %d",
            fActivate);
    fnsLog(PARAMLOGLEVEL, "ppNewPath     == %p   %s",
            ppdmNewPath,
            fValid_ppNewPath ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->CreateAudioPath(pdmSourceConfig,fActivate,ppdmNewPath);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::CreateAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    if(fValid_ppNewPath)
    {
        fnsLog(PARAMLOGLEVEL, "*ppNewPath == %p",
                pdmNewPath);
		// create the test object
		if(SUCCEEDED(hRes) && pdmNewPath)
		{
			hRes = dmthCreateTestWrappedObject(pdmNewPath, ppNewPath);
			if(FAILED(hRes))
			{
				fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", 
						hRes);
			}
		}
	}

    fnsDecrementIndent();

END:
    // done
    if(fValid_ppNewPath && pdmNewPath)
    {
        pdmNewPath->Release();
    }

    //Added 7/13/00 - Danhaff
    if (fValid_pSourceConfig && pdmSourceConfig)
    {
        pdmSourceConfig->Release();
    }
    return hRes;

} // *** end CtIDirectMusicPerformance8::CreateAudioPath()




//===========================================================================
// CtIDirectMusicPerformance8::CreateStandardAudioPath()
//
// Encapsulates calls to CreateStandardAudioPath
//
// History:
//  01/17/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::CreateStandardAudioPath
(
	DWORD dwType,
	DWORD dwPChannelCount,
	BOOL fActivate,
	CtIDirectMusicAudioPath **ppNewPath
)
{
    HRESULT			hRes				= E_NOTIMPL;
    BOOL			fValid_ppNewPath	= TRUE;
    IDirectMusicAudioPath	*pdmPath	= NULL;
    IDirectMusicAudioPath	**ppdmPath	= NULL;

    // validate pptdmPort
    if(!helpIsValidPtr((void*)ppNewPath, sizeof(CtIDirectMusicAudioPath*),
                        FALSE))
    {
        // bogus pointer, use as such
        fValid_ppNewPath = FALSE;
        ppdmPath = (IDirectMusicAudioPath**)ppNewPath;

    }
    else
    {
        // valid pointer, create a real object
        ppdmPath = &pdmPath;

        // just in case we fail, init test object ptr to NULL
        *ppNewPath = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::CreateStandardAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "dwType          == %08Xh (%s)",
            dwType,
            dmthXlatDMUS_APATH(dwType));
    fnsLog(PARAMLOGLEVEL, "dwPChannelCount == %08Xh",
            dwPChannelCount);
    fnsLog(PARAMLOGLEVEL, "fActivate == %d",
            fActivate);
    fnsLog(PARAMLOGLEVEL, "ppNewPath       == %p   %s",
            ppdmPath,
            fValid_ppNewPath ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->CreateStandardAudioPath(dwType,dwPChannelCount,fActivate,ppdmPath);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::CreateStandardAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    if(fValid_ppNewPath)
    {
        fnsLog(PARAMLOGLEVEL, "*ppNewPath == %p",
                pdmPath);
		// create the test object
		if(SUCCEEDED(hRes) && pdmPath)
		{
			hRes = dmthCreateTestWrappedObject(pdmPath, ppNewPath);
			if(FAILED(hRes))
			{
				fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", 
						hRes);
			}
		}
	}

    fnsDecrementIndent();

    // done
    if(fValid_ppNewPath && pdmPath)
    {
        pdmPath->Release();
    }
    return hRes;

} // *** end CtIDirectMusicPerformance8::CreateStandardAudioPath()




//===========================================================================
// CtIDirectMusicPerformance8::SetDefaultAudioPath()
//
// Encapsulates calls to SetDefaultAudioPath
//
// History:
//  01/17/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::SetDefaultAudioPath
(
	CtIDirectMusicAudioPath *pAudioPath
)
{
    HRESULT		hRes				= E_NOTIMPL;
    BOOL		fValid_pAudioPath	= TRUE;
    IDirectMusicAudioPath*	pdmPath	= NULL;

    // validate pTemplate
    if(!helpIsValidPtr((void*)pAudioPath, sizeof(CtIDirectMusicAudioPath),
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pAudioPath = FALSE;
        pdmPath = (IDirectMusicAudioPath*)pAudioPath;

    }
    else
    {
        // good pointer, get the real object
        hRes = pAudioPath->GetRealObjPtr(&pdmPath);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::SetDefaultAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pAudioPath       == %p   %s",
            pdmPath,
            fValid_pAudioPath ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->SetDefaultAudioPath(pdmPath);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::SetDefaultAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();
END:
    // done
    if(fValid_pAudioPath && pdmPath)
    {
        pdmPath->Release();
    }
    return hRes;

} // *** end CtIDirectMusicPerformance8::SetDefaultAudioPath()




//===========================================================================
// CtIDirectMusicPerformance8::GetDefaultAudioPath()
//
// Encapsulates calls to GetDefaultAudioPath
//
// History:
//  01/17/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::GetDefaultAudioPath
(
	CtIDirectMusicAudioPath **ppAudioPath
)
{
    HRESULT			hRes				= E_NOTIMPL;
    BOOL			fValid_ppAudioPath	= TRUE;
    IDirectMusicAudioPath	*pdmPath	= NULL;
    IDirectMusicAudioPath	**ppdmPath	= NULL;

    // validate pptdmPort
    if(!helpIsValidPtr((void*)ppAudioPath, sizeof(CtIDirectMusicAudioPath*),
                        FALSE))
    {
        // bogus pointer, use as such
        fValid_ppAudioPath= FALSE;
        ppdmPath = (IDirectMusicAudioPath**)ppAudioPath;

    }
    else
    {
        // valid pointer, create a real object
        ppdmPath = &pdmPath;

        // just in case we fail, init test object ptr to NULL
        *ppAudioPath = NULL;
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::GetDefaultAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "ppAudioPath       == %p   %s",
            ppdmPath,
            fValid_ppAudioPath ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->GetDefaultAudioPath(ppdmPath);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::GetDefaultAudioPath()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    if(fValid_ppAudioPath)
    {
        fnsLog(PARAMLOGLEVEL, "*ppAudioPath == %08Xh",
                pdmPath);
		// create the test object
		if(SUCCEEDED(hRes) && pdmPath)
		{
			hRes = dmthCreateTestWrappedObject(pdmPath, ppAudioPath);
			if(FAILED(hRes))
			{
				fnsLog(MINLOGLEVEL, "**** Unable to create wrapped object %08Xh", 
						hRes);
			}
		}
	}

    fnsDecrementIndent();

    // done
    if(fValid_ppAudioPath && pdmPath)
    {
        pdmPath->Release();
    }
    return hRes;

} // *** end CtIDirectMusicPerformance8::GetDefaultAudioPath()




//===========================================================================
// CtIDirectMusicPerformance8::SetParamHook()
//
// Encapsulates calls to SetParamHook
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
/*
HRESULT CtIDirectMusicPerformance8::SetParamHook(CtIDirectMusicParamHook* pIHook)
{
    HRESULT             hRes				= E_NOTIMPL;
    BOOL		        fValid_pIHook    = TRUE;
    IDirectMusicParamHook   *pdmIHook           = NULL;

    // validate pSegment
    if(!helpIsValidPtr((void*)pIHook, sizeof(CtIDirectMusicGraph),
                    FALSE))
    {
        // bogus pointer, use as such
        fValid_pIHook = FALSE;
        pdmIHook = (IDirectMusicParamHook*)pIHook;

    }
    else
    {
        // good pointer, get the real object
        hRes = pIHook->GetRealObjPtr(&pdmIHook);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to retrieve real "
                    "object pointer (%s == %08Xh)",
                    tdmXlatHRESULT(hRes), hRes);
            goto END;
        }
    }

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::SetParamHook()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "pGraph == %p   %s",
            pdmIHook,
            fValid_pIHook ? "" : "BAD");

    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->SetParamHook(pdmIHook);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::SetParamHook()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);

    fnsDecrementIndent();

    // done
END:
    if(fValid_pIHook && pdmIHook)
    {
        pdmIHook->Release();
    }
    return hRes;

} // *** end CtIDirectMusicPerformance8::SetParamHook()
*/
/*
//===========================================================================
// CtIDirectMusicPerformance8::GetParamEx()
//
// Encapsulates calls to GetParamEx
//
// History:
//  04/24/2000 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicPerformance8::GetParamEx(
	REFGUID rguidType, 
	DWORD dwTrackID,
	DWORD dwGroupBits, 
	DWORD dwIndex, 
	MUSIC_TIME mtTime, 
	MUSIC_TIME* pmtNext, 
	void* pData)
{
    HRESULT hRes				= E_NOTIMPL;
	BOOL	fValid_pmtNext		= TRUE;
	BOOL	fValid_pData		= TRUE;
    char    szGuid[MAX_LOGSTRING];

    // validate pmtNext
    if(!helpIsValidPtr((void*)pmtNext, sizeof(MUSIC_TIME), FALSE))
    {
        fValid_pmtNext = FALSE;
    }
    
    // validate pData
    if(!helpIsValidPtr((void*)pData, 1, FALSE))
    {
        fValid_pData = FALSE;
    }

    dmthGUIDtoString(rguidType, szGuid);

    fnsIncrementIndent();

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling %s::GetParamEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "rguidType    == %s (%s)",
			szGuid, dmthXlatGUID(rguidType));
    fnsLog(PARAMLOGLEVEL, "dwTrackID    == %08Xh",
            dwTrackID);
    fnsLog(PARAMLOGLEVEL, "dwGroupBits  == %08Xh",
            dwGroupBits);
    fnsLog(PARAMLOGLEVEL, "dwIndex      == %08Xh",
            dwIndex);
	fnsLog(PARAMLOGLEVEL, "mtTime       == %08Xh",
            mtTime);
	fnsLog(PARAMLOGLEVEL, "pmtNext      == %p   %s",
            pmtNext,
            fValid_pmtNext ? "" : "BAD");
    fnsLog(PARAMLOGLEVEL, "pData        == %p   %s",
            pData,
            fValid_pData ? "" : "BAD");
     
    // call the real function
    hRes = ((IDirectMusicPerformance8*)m_pUnk)->GetParamEx(
		rguidType, 
		dwTrackID,
		dwGroupBits, 
		dwIndex, 
		mtTime, 
		pmtNext, 
		pData);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from %s::GetParamEx()",
            m_szInterfaceName);
    fnsLog(PARAMLOGLEVEL, "hRes       == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_pmtNext)
    {
        fnsLog(PARAMLOGLEVEL, "*pmtNext   == %08Xh",
                *pmtNext);
    }

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end CtIDirectMusicPerformance8::GetParam()
*/