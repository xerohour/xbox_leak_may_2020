//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctseg8.cpp
//
//--------------------------------------------------------------------------

#include "globals.h"
#include "DMClient.h"
#include "trans.h"


//===========================================================================
// CtIDirectMusicSegment8::CtIDirectMusicSegment8()
//
// Default constructor
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
CtIDirectMusicSegment8::CtIDirectMusicSegment8(void)
{
    strcpy(m_szClassName, &(typeid(this).name())[8]);
    m_szClassName[strlen(m_szClassName) - 2] = NULL;  //cut off the " *"
} // *** end CtIDirectMusicSegment8::CtIDirectMusicSegment8()





HRESULT CtIDirectMusicSegment8::Download(CtIUnknown *ptUnk) //note: pUnk can either be an audiopath or a performance!!!!
{
    DWORD dwParameters = 0;
    LPDATABLOCK *ppDataBlocks = NULL;
    HRESULT hr = S_OK;
    HRESULT hrCall = S_OK;
    LPVOID      pvRealPtr = NULL;

    //Get the IUnknown * from this.
    hr = ptUnk->GetRealObjPtr(&pvRealPtr);

    //Write to the file notifying it of this test class's creation.
    TRANS *pTrans = NULL;
    ALLOCATEANDCHECK(pTrans, TRANS);
    CHECKRUN(pTrans->Client_CreateParameterDataFile());
    
    //2 In-parameters 
    CHECKRUN(pTrans->WriteMainBlockHeader(m_pvXBoxPointer, "IDirectMusicSegment8::Download", 2));  
    
    //PARAMETER 1: THIS!
    //1 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(LPVOID), &m_pvXBoxPointer));  

    //PARAMETER 2: CtIUnknown
    //2 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(CtIUnknown *), &pvRealPtr));  

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
};






HRESULT CtIDirectMusicSegment8::Unload(CtIUnknown *ptUnk)
{
    DWORD dwParameters = 0;
    LPDATABLOCK *ppDataBlocks = NULL;
    HRESULT hr = S_OK;
    HRESULT hrCall = S_OK;
    LPVOID      pvRealPtr = NULL;

    //Get the IUnknown * from this.
    hr = ptUnk->GetRealObjPtr(&pvRealPtr);

    //Write to the file notifying it of this test class's creation.
    TRANS *pTrans = NULL;
    ALLOCATEANDCHECK(pTrans, TRANS);
    CHECKRUN(pTrans->Client_CreateParameterDataFile());
    
    //2 In-parameters 
    CHECKRUN(pTrans->WriteMainBlockHeader(m_pvXBoxPointer, "IDirectMusicSegment8::Unload", 2));  
    
    //PARAMETER 1: THIS!
    //1 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(LPVOID), &m_pvXBoxPointer));  

    //PARAMETER 2: CtIUnknown
    //2 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(CtIUnknown *), &pvRealPtr));  

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
};


