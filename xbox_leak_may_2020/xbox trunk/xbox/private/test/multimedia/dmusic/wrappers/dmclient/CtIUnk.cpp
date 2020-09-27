#include "globals.h"
#include "DMClient.h"

CtIUnknown::CtIUnknown(void)
{
      m_pvXBoxPointer = NULL;
      m_dwRefCount = 1;
      ZeroMemory(m_szClassName, MAX_PATH);
};



HRESULT CtIUnknown::InitTestClass(LPVOID pObject)
{
    //Get our name.
//    strcpy(m_szClassName, typeid(this).name());
    m_pvXBoxPointer = pObject;

/*
    //Write to the file notifying it of this test class's creation.
    TRANS *pTrans = NULL;
    ALLOCATEANDCHECK(pTrans, TRANS);
    CHECKRUN(pTrans->Client_CreateParameterDataFile());
    
    //No in parameters, one "out" parameter. No THIS ptr
    sprintf(szString, "Create_%s", m_szClassName);
    CHECKRUN(pTrans->WriteMainBlockHeader(NULL, szString, 0));  
    
    //No data chunks, since we're just creating.
    CHECKRUN(pTrans->WriteParamBlockHeader(0));  

    //Close out the file.  This will cause it to be sent to the XBox.
    CHECKRUN(pTrans->CloseDataFile());

    //Now wait for the returned datafile.
    CHECKRUN(pTrans->Client_ReadReturnedDataFile());    

    //Get the number of parameters
    CHECKRUN(pTrans->ReadMainBlockHeader(NULL, NULL, &dwParameters));

    if (2!=dwParameters)
        Output("%s creation returned %d instead of 2 paramters", m_szClassName, dwParameters);

    //Get a pointer to all the parameters.
    CHECKRUN(pTrans->ReadParameters(dwParameters, &ppDataBlocks));

    //Store the XBox pointer and return code here.
    //Parameter         (TYPE)                         [blk]
    //--------------------------------------------------------------------
    hrCall          = *(HRESULT *)ppDataBlocks         [0]    [0].pvData;
    m_pvXBoxPointer = *(LPVOID *) ppDataBlocks         [1]    [0].pvData;

    CHECKRUN(pTrans->DeleteAllocatedParameterList(dwParameters, ppDataBlocks));
*/
    return S_OK;
};






//===========================================================================
// CtIUnknown::InitTestClass
//===========================================================================
HRESULT CtIUnknown::InitTestClass(void)
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
    
    //No in parameters, one "out" parameter. No THIS ptr
    sprintf(szString, "Create_%s", m_szClassName);
    CHECKRUN(pTrans->WriteMainBlockHeader(NULL, szString, 0));  
    
    //No data chunks, since we're just creating.
    CHECKRUN(pTrans->WriteParamBlockHeader(0));  

    //Close out the file.  This will cause it to be sent to the XBox.
    CHECKRUN(pTrans->CloseDataFile());

    //Now wait for the returned datafile.
    CHECKRUN(pTrans->Client_ReadReturnedDataFile());    

    //Get the number of parameters
    CHECKRUN(pTrans->ReadMainBlockHeader(NULL, NULL, &dwParameters));

    if (2!=dwParameters)
        Output("%s creation returned %d instead of 2 paramters", m_szClassName, dwParameters);

    //Get a pointer to all the parameters.
    CHECKRUN(pTrans->ReadParameters(dwParameters, &ppDataBlocks));

    //Store the XBox pointer and return code here.
    //Parameter         (TYPE)                         [blk]
    //--------------------------------------------------------------------
    hrCall          = *(HRESULT *)ppDataBlocks         [0]    [0].pvData;
    m_pvXBoxPointer = *(LPVOID *) ppDataBlocks         [1]    [0].pvData;

    CHECKRUN(pTrans->DeleteAllocatedParameterList(dwParameters, ppDataBlocks));

    CHECKRUN(pTrans->CloseDataFile());
    SAFEDELETE(pTrans);

    return hrCall;


} // *** end CtIDirectMusicPerformance8::InitTestClass()




HRESULT CtIUnknown::GetRealObjPtr(LPVOID *ppVoid)
{
    *ppVoid = m_pvXBoxPointer;
    return S_OK;
};


DWORD CtIUnknown::AddRef()
{

    DWORD dwParameters = 0;
    LPVOID *ppv = NULL;
    HRESULT hr = S_OK;
    LPDATABLOCK *ppDataBlocks = NULL;
    char szString[MAX_PATH];
    DWORD dwRet = 0;


    //Write to the file notifying it of this test class's creation.
    TRANS *pTrans = NULL;
    ALLOCATEANDCHECK(pTrans, TRANS);
    CHECKRUN(pTrans->Client_CreateParameterDataFile());

    //One parameter - the this pointer.
    sprintf(szString, "IUnknown::AddRef", m_szClassName);
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
        Output("IUnknown::AddRef returned %d parameters instead of 1", dwParameters);

    //Get a pointer to all the parameters.
    CHECKRUN(pTrans->ReadParameters(dwParameters, &ppDataBlocks));

    //Store the XBox pointer and return code here.
    //Parameter         (TYPE)                         [blk]
    //--------------------------------------------------------------------
    dwRet           = *(DWORD *)  ppDataBlocks         [0]    [0].pvData;

    CHECKRUN(pTrans->DeleteAllocatedParameterList(dwParameters, ppDataBlocks));

    CHECKRUN(pTrans->CloseDataFile());
    SAFEDELETE(pTrans);

    return ++m_dwRefCount;
};





DWORD CtIUnknown::Release()
{
    DWORD dwParameters = 0;
    LPVOID *ppv = NULL;
    HRESULT hr = S_OK;
    LPDATABLOCK *ppDataBlocks = NULL;
    char szString[MAX_PATH];
    DWORD dwRet = 0;


    //Write to the file notifying it of this test class's creation.
    TRANS *pTrans = NULL;
    ALLOCATEANDCHECK(pTrans, TRANS);
    CHECKRUN(pTrans->Client_CreateParameterDataFile());
    
    //One parameter - the this pointer.
    sprintf(szString, "IUnknown::Release", m_szClassName);
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
        Output("IUnknown::Release returned %d parameters instead of 1", dwParameters);

    //Get a pointer to all the parameters.
    CHECKRUN(pTrans->ReadParameters(dwParameters, &ppDataBlocks));

    //Store the XBox pointer and return code here.
    //Parameter         (TYPE)                         [blk]
    //--------------------------------------------------------------------
    dwRet           = *(DWORD *)  ppDataBlocks         [0]    [0].pvData;
    CHECKRUN(pTrans->DeleteAllocatedParameterList(dwParameters, ppDataBlocks));

    CHECKRUN(pTrans->CloseDataFile());
    SAFEDELETE(pTrans);



m_dwRefCount--;
if (0==m_dwRefCount)
    {
        delete this;
        return 0;
    }

return m_dwRefCount;

}
