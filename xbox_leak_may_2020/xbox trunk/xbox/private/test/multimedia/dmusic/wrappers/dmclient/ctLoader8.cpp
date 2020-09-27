 //+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       ctLoader8.cpp
//
//--------------------------------------------------------------------------


#include "globals.h"
#include "DMClient.h"
#include "create.h"
#include "trans.h"
#include "copy.h"

//---------------------------------------------------------------------------



//===========================================================================
// CtIDirectMusicLoader8::CtIDirectMusicLoader8()
///
// Default constructor
//===========================================================================
CtIDirectMusicLoader8::CtIDirectMusicLoader8(void)

{
    strcpy(m_szClassName, &(typeid(this).name())[8]);
    m_szClassName[strlen(m_szClassName) - 2] = NULL;  //cut off the " *"
} // *** end CtIDirectMusicLoader8::CtIDirectMusicLoader8()



//===========================================================================
//===========================================================================
/*
CtIDirectMusicLoader8::~CtIDirectMusicLoader8(void)
{


} // *** end CtIDirectMusicLoader8::~CtIDirectMusicLoader8()



//===========================================================================
// CtIDirectMusicLoader8::InitTestClass
//===========================================================================
HRESULT CtIDirectMusicLoader8::InitTestClass(void)
{


} // *** end CtIDirectMusicLoader8::InitTestClass()


*/





//===========================================================================
// CtIDirectMusicLoader8::InitAudio()
//
// Encapsulates calls to InitAudio
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicLoader8::LoadObjectFromFile(REFGUID rguidClassID,
                                                  REFIID iidInterfaceID,
                                                  CHAR *pzFilePath,
                                                  void ** ppObject)
{
    DWORD dwParameters = 0;
    LPDATABLOCK *ppDataBlocks = NULL;
    HRESULT hr = S_OK;
    HRESULT hrCall = S_OK;
    void *pObject = NULL;
    CHAR *szFileName = NULL;

    //Copy the appropriate files over!!!  Es muy importante this is done prior to writing the actual data file.
    CHECKRUN(CopyDMusicFileToXBox(pzFilePath));

    //Chop the file path down to the current directory.
    szFileName = PathFindFileName(pzFilePath);

    //Write to the file notifying it of this test class's creation.
    TRANS *pTrans = NULL;
    ALLOCATEANDCHECK(pTrans, TRANS);
    CHECKRUN(pTrans->Client_CreateParameterDataFile());
    
    //5 In-parameters
    CHECKRUN(pTrans->WriteMainBlockHeader(m_pvXBoxPointer, "IDirectMusicLoader8::LoadObjectFromFile", 4));  
    
    //PARAMETER 1: THIS!
    //1 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(LPVOID), &m_pvXBoxPointer));  

    //PARAMETER 2: rguidClassID
    //1 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(GUID), (void *)&rguidClassID));  

    //PARAMETER 3: iidInterfaceID
    //1 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(IID), (void *)&iidInterfaceID));  

    //PARAMETER 4: pwzFilePath
    //2 data chunks.
    CHECKRUN(pTrans->WriteParamBlockHeader(2));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue,         NULL, NULL, sizeof(CHAR *), &szFileName));  
//    CHECKRUN(pTrans->WriteDataBlock(PointerToMemory, 0, 0, (wcslen(pwzFilePath) + 1) * 2, (void *)pwzFilePath));  
    CHECKRUN(pTrans->WriteDataBlock(PointerToMemory, 0, 0, strlen(szFileName) + 1, (void *)szFileName));  

    //Close out the file.  This will cause it to be sent to the XBox.
    CHECKRUN(pTrans->CloseDataFile());

    //Now wait for the returned datafile.
    CHECKRUN(pTrans->Client_ReadReturnedDataFile());    

    //There should be 2 out-paramters - return value and the pointer.
    //  1) Return Value
    CHECKRUN(pTrans->ReadMainBlockHeader(NULL, NULL, &dwParameters));

    if (2!=dwParameters)
        Output("IDirectMusicLoader8::InitAudio returned %d paramters instead of 2", dwParameters);

    //Get a pointer to all the parameters.
    CHECKRUN(pTrans->ReadParameters(dwParameters, &ppDataBlocks));

    //Store the return code here.
    hrCall          = *(HRESULT *)ppDataBlocks         [0]    [0].pvData;
    pObject         = *(LPVOID *)ppDataBlocks          [1]    [0].pvData;

    CHECKRUN(pTrans->DeleteAllocatedParameterList(dwParameters, ppDataBlocks));

    CHECKRUN(pTrans->CloseDataFile());
    SAFEDELETE(pTrans);


    
    //Big old switch statment on IIDs to determine which object to wrap this in.
    //TODO: Put in the rest of the types, when they are created.
    if (SUCCEEDED(hrCall))
    {
        if (memcmp((void *)&iidInterfaceID, (void *)&xIID_IDirectMusicSegment8, sizeof(GUID))==0)
        {

            CtIDirectMusicSegment8 *pSegment8 = NULL;
            hr = CreateInstance(pObject, &pSegment8);
            *ppObject = (LPVOID)pSegment8;
        }
        else if (memcmp((void *)&iidInterfaceID, (void *)&xIID_IDirectMusicBand, sizeof(GUID))==0)
        {

        }
        else
        {
            Output("Error - unknown CLSID passed to LoadObjectFromFile!!");
        }
    }
    else
    {
        *ppObject = NULL;
    }
    
    
    return hrCall;

} // *** end CtIDirectMusicLoader8::InitAudio()



//===========================================================================
// CtIDirectMusicLoader8::InitAudio()
//
// Encapsulates calls to InitAudio
//
// History:
//  10/13/1999 - kcraven - created
//===========================================================================
HRESULT CtIDirectMusicLoader8::SetObject(LPDMUS_OBJECTDESC pDesc)
{
    DWORD dwParameters = 0;
    LPDATABLOCK *ppDataBlocks = NULL;
    HRESULT hr = S_OK;
    HRESULT hrCall = S_OK;
    void *pObject = NULL;

    //Copy the appropriate files over!!!  Es muy importante this is done prior to writing the actual data file.
    //BUGBUG: Only support relative paths right now!!!
    if (pDesc)
    {
        if (pDesc->dwValidData & DMUS_OBJ_FULLPATH)
        {
            CHECKRUN(CopyDMusicFileToXBox(pDesc->wszFileName));
        }
        else if (pDesc->dwValidData & DMUS_OBJ_FILENAME)
        {
            CHECKRUN(CopyDMusicFileToXBox(pDesc->wszFileName));
        }
        else
        {
            Output("CtIDirectMusicLoader8::SetObject couldn't find a filename to copy over, is this normal?");
        }

    }


    //Write to the file notifying it of this test class's creation.
    TRANS *pTrans = NULL;
    ALLOCATEANDCHECK(pTrans, TRANS);
    CHECKRUN(pTrans->Client_CreateParameterDataFile());
    
    //2 In-parameters
    CHECKRUN(pTrans->WriteMainBlockHeader(m_pvXBoxPointer, "IDirectMusicLoader8::SetObject", 2));  
    
    //PARAMETER 1: THIS!
    //1 data chunk.
    CHECKRUN(pTrans->WriteParamBlockHeader(1));  
    CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(LPVOID), &m_pvXBoxPointer));  

    //PARAMETER 2: pDesc
    //3 data chunks.
    CHECKRUN(pTrans->WriteParamBlockHeader(3));  
        //Data Chunk index 0: Value of the pointer
        CHECKRUN(pTrans->WriteDataBlock(ByValue, NULL, NULL, sizeof(GUID), (void *)&pDesc));  
        //Data Chunk index 1: Data in DMUS_OBJECTDESC, whose address will be located at offset 0 from chunk 1.
        CHECKRUN(pTrans->WriteDataBlock(PointerToMemory, 0, 0, sizeof(DMUS_OBJECTDESC), (void *)pDesc));  
        //Data Chunk index 2: The data pointed to by the pbMemData field of the DMUS_OBJECTDESC.  At offset 
        CHECKRUN(pTrans->WriteDataBlock(PointerToMemory, 1, DWORD(LPBYTE(&pDesc->pbMemData) - LPBYTE(pDesc)), pDesc->llMemLength, pDesc->pbMemData ));  

    //Close out the file.  This will cause it to be sent to the XBox.
    CHECKRUN(pTrans->CloseDataFile());

    //Now wait for the returned datafile.
    CHECKRUN(pTrans->Client_ReadReturnedDataFile());    

    //There should be 1 out-paramters - the return value
    //  1) Return Value
    CHECKRUN(pTrans->ReadMainBlockHeader(NULL, NULL, &dwParameters));

    if (1!=dwParameters)
        Output("IDirectMusicLoader8::SetObject returned %d paramters instead of 1", dwParameters);

    //Get a pointer to all the parameters.
    CHECKRUN(pTrans->ReadParameters(dwParameters, &ppDataBlocks));

    //Store the return code here.
    hrCall          = *(HRESULT *)ppDataBlocks         [0]    [0].pvData;

    CHECKRUN(pTrans->DeleteAllocatedParameterList(dwParameters, ppDataBlocks));
    CHECKRUN(pTrans->CloseDataFile());
    SAFEDELETE(pTrans);
    return hrCall;

} // *** end CtIDirectMusicLoader8::InitAudio()



