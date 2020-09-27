#include "trans.h"

/*
#define SW_HIDE             0
#define SW_SHOWNORMAL       1
#define SW_NORMAL           1
#define SW_SHOWMINIMIZED    2
#define SW_SHOWMAXIMIZED    3
#define SW_MAXIMIZE         3
#define SW_SHOWNOACTIVATE   4
#define SW_SHOW             5
#define SW_MINIMIZE         6
#define SW_SHOWMINNOACTIVE  7
#define SW_SHOWNA           8
#define SW_RESTORE          9
#define SW_SHOWDEFAULT      10
#define SW_FORCEMINIMIZE    11
#define SW_MAX              11


STDAPI_(HINSTANCE) ShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd);
STDAPI_(HINSTANCE) ShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd);
#ifdef UNICODE
#define ShellExecute  ShellExecuteW
#else
#define ShellExecute  ShellExecuteA
#endif // !UNICODE
*/

//Must use full pathnames.
char *szXBoxReturnNotify   =   "XBRETURN.TXT";
char *szXBoxParameters     =   "DATAIN.BIN";
char *szXBoxReturnValues   =   "DATAOUT.BIN";
char *szXBoxPath           =   "T:\\TRANS\\";
char *szAll                =   "*.*";


HRESULT SystemCall(char *szString)
{
HINSTANCE hInst = NULL;
/*
HINSTANCE ShellExecute(
    HWND hwnd, 
    LPCTSTR lpOperation,
    LPCTSTR lpFile, 
    LPCTSTR lpParameters, 
    LPCTSTR lpDirectory,
    INT nShowCmd
);	
*/

#ifdef WINDOWS
//    hInst = ShellExecute(NULL, NULL, szString, NULL, NULL, SW_HIDE);
    WinExec(szString, SW_HIDE);
#endif

    return S_OK;
};


/****************************************************************
Sends a file from wherever it is to xt:\trans.
****************************************************************/
HRESULT XBoxSendFile(char *szName, BOOL bWait)
{
char szString[512] = {0};
HRESULT hrSend = S_OK;
HRESULT hr = S_OK;
DWORD dwIterations = 0;
char *szNakedName = NULL;

    //Copy the file over to the xbox.
    #ifdef WINDOWS        
    szNakedName = PathFindFileName(szName);
    sprintf(szString, "t:\\trans\\%s", szNakedName);        
    do
    {
        hrSend = DmSendFile(szName, szString);
        if (XBDM_NOERR == hrSend)
            break;
        if (!bWait)
            break;
        dwIterations++;
        Sleep(100);
    }
    while (dwIterations < 100);
    if (XBDM_NOERR != hrSend)
    {
        Output("XBoxSendFile: could not send %s to XBox %s", szName, hrSend == XBDM_NOSUCHFILE ? "Because it did not exist" : "");
        if (hrSend == XBDM_NOSUCHFILE && !bWait)
            hr = S_OK;
        else
            hr = E_FAIL;
    }

    #endif

    return hr;

}

/****************************************************************
****************************************************************/
HRESULT XBoxReceiveFile(char *szName)
{
char szString[512] = {0};
HRESULT hrSend = S_OK;
HRESULT hr = S_OK;
DWORD dwIterations = 0;
char *szNakedName = NULL;

    //Copy the file from the xbox.
    #ifdef WINDOWS        
    szNakedName = PathFindFileName(szName);
    sprintf(szString, "t:\\trans\\%s", szName);        
    do
    {
        hrSend = DmReceiveFile(szNakedName, szString);
        if (XBDM_NOERR == hrSend)
            break;
        dwIterations++;
        Sleep(100);
    }
    while (/*dwIterations < 100*/ 1);
    if (XBDM_NOERR != hrSend)
    {
        Output("XBoxReceiveFile: could not receive %s from XBox", szString);
        hr = E_FAIL;
    }

    #endif

    return hr;

};

HRESULT XBoxMkdir(LPCSTR szName)
{
char szString[512] = {0};
HRESULT hrSend = S_OK;
HRESULT hr = S_OK;
DWORD dwIterations = 0;

    //Copy the file from the xbox.
    #ifdef WINDOWS        
    sprintf(szString, "t:\\trans\\%s", szName);        
    do
    {
        hrSend = DmMkdir(szString);
        if (XBDM_NOERR == hrSend)
            break;
        if (XBDM_ALREADYEXISTS == hrSend)
            break;
        dwIterations++;
        Sleep(100);
    }
    while (dwIterations < 100);
    if (XBDM_NOERR != hrSend)
    {
//        Output("XBoxMkDir: could not make %s on XBox %s", szString, XBDM_ALREADYEXISTS == hrSend ? "because it already exists" : "");
        //Okay if the directory exists.
        if (XBDM_ALREADYEXISTS == hrSend)
            hr = S_OK;
        else
            hr = E_FAIL;
    }

    #endif

    return hr;


};


HRESULT XBoxDeleteFile(LPCSTR szName, BOOL fIsDirectory, BOOL bWait)
{
char szString[512] = {0};
HRESULT hrSend = S_OK;
HRESULT hr = S_OK;
DWORD dwIterations = 0;

    //Delete the file from the xbox.
    #ifdef WINDOWS        
    sprintf(szString, "t:\\trans\\%s", szName);        
    do
    {
        hrSend = DmDeleteFile(szString, fIsDirectory);
        if (XBDM_NOERR == hrSend)
            break;

        if (!bWait)
            break;
        dwIterations++;
        Sleep(100);
    }
    while (dwIterations < 100);
    if (XBDM_NOERR != hrSend)
    {
        //Output("XBoxDeleteFile: could not delete %s from XBox %s", szString, hrSend == XBDM_NOSUCHFILE ? "Because it did not exist" : "");
        //It's only okay to fail the deletion if we didn't want to wait (file might not exist) and the file was verified not to exist.
        if (hrSend == XBDM_NOSUCHFILE && !bWait)
            hr = S_OK;
        else
            hr = E_FAIL;
    }

    #endif

    return hr;



};



/****************************************************************
****************************************************************/
HRESULT CheckFileDump(char *szFilename, char *szString, BOOL *pbFound)
{
FILE *fyle = NULL;
char szFileString[256];
BOOL bFound = FALSE;


    //Open up the file as a text file.
    fyle = fopen(szFilename, "rt");
    if (fyle == NULL)
    {
       Output("CheckFileDump: Couldn't open %s\n", szFilename);
       *pbFound = bFound;
       return E_FAIL;
    }
        

    //See if we find our string.
    while (fgets(szFileString, 255, fyle))
    {
        _strupr(szFileString);
        if (strstr(szFileString, szString) != NULL)
        {
            *pbFound = TRUE;
            break;
        }
    }

    fclose(fyle);
    *pbFound = bFound;
    return S_OK;
};


/****************************************************************
****************************************************************/
TRANS::TRANS(void)
{
    m_file = NULL;
};

/****************************************************************
****************************************************************/
TRANS::~TRANS(void)
{
    CloseDataFile();
};


/****************************************************************
Must be called at the beginning of our host app.
****************************************************************/
HRESULT TRANS::XBox_Init(void)
{
FILE *tempfile = NULL;
char szString[512] = {0};
HANDLE hSearch = NULL;
HRESULT hr = S_OK;
WIN32_FIND_DATA Data = {0};


    //Create the trans directory, if it doesn't already exist.
    CreateDirectory("T:\\Trans\\", NULL);

//Delete the parameters and the return values files in the XBox directory.
/*
    sprintf(szString, "%s%s", szXBoxPath, szXBoxParameters);
    DeleteFile(szString);

    sprintf(szString, "%s%s", szXBoxPath, szXBoxReturnValues);
    DeleteFile(szString);
*/

    //Delete ALL files in the XBox directory.
    sprintf(szString, "%s%s", szXBoxPath, szAll);
    hSearch = FindFirstFile(szString, &Data);
    if (hSearch == INVALID_HANDLE_VALUE)
        hr = E_FAIL;
    do
    {
        sprintf(szString, "%s%s", szXBoxPath, Data.cFileName);
        DeleteFile(szString);
        
    } 
    while (FindNextFile(hSearch, &Data));

    FindClose(hSearch);
    hSearch = NULL;

    //Create the szXBoxReturnNotify file so the client knows that it
    //  can proceed to make calls.
    sprintf(szString, "%s%s", szXBoxPath, szXBoxReturnNotify);
    tempfile = fopen(szString, "w");
    fclose(tempfile);   

    return S_OK;
}


/****************************************************************
****************************************************************/
HRESULT TRANS::ClientWaitForXBoxReturned(void)
{

HRESULT hr = S_OK;

    BOOL bFound = FALSE;
    DWORD dwIterations = 0;
    FILE *file = NULL;
    char szBuffer[1000] = {0};
    int iRet = 0;
    BOOL bRet = FALSE;
//    bFound = FALSE;
  /*  do
    {
*/
        //Try to copy over the file and open it.
        hr = XBoxReceiveFile(szXBoxReturnNotify);
        if (FAILED(hr))
        {
            Output("TRANS::ClientWaitForXBoxReturned failed to obtain %s", szXBoxReturnNotify);
            return hr;
        }


/*
        sprintf(szBuffer, "xbcp -y xt:\\trans\\%s .", szXBoxReturnNotify);
        do
        {
            SystemCall(szBuffer);
            file = fopen(szXBoxReturnNotify, "r");
            if (file)
                break;
            Sleep(300);
        }
        while (1);

        //Immediately close and delete this file.
        do
        {
            iRet = fclose(file);
            if (0 == iRet)
                break;
            Sleep(100);
        }
        while (1);
*/
        //Immediately delete the file locally.

        do
        {
            bRet = DeleteFile(szXBoxReturnNotify);
            if (bRet)
                break;
            Sleep(100);
        }
        while (1);

/*
        //Store the contents of the XBox directory in filedump.txt
        SystemCall("xbdir xt:\\trans\\ > filedump.txt");

        //See if our file is there.
        hr = CheckFileDump("filedump.txt", szXBoxReturnNotify, &bFound);
        if (FAILED(hr))
        {
            OutputDebugString("TRANS::ClientWaitForXBoxReturned: couldn't create filedump.txt");
            return E_FAIL;
        }

        //Try again.
        dwIterations ++;
        Sleep(50);

    }
    while (!bFound);

    if (!bFound)
    {
        OutputDebugString("Error: waited more than 5s for call to complete on XBox");
        return E_FAIL;
    }
*/

return hr;
};


/****************************************************************
****************************************************************/
HRESULT TRANS::XBox_CreateReturnedDataFile()
{
    HRESULT hr = S_OK;
    char szString[512] = {0};
    DWORD dwIterations = 0;


    //Open up the file to send. 
    sprintf(szString, "%s%s", szXBoxPath, szXBoxReturnValues);

    do 
    {
        m_file = fopen(szString, "wb");
        if (m_file)
            break;
        dwIterations++;
        Sleep(100);
    }
    while (dwIterations < 100);  //10 seconds
    
    
    if (NULL == m_file)
    {
        Output("TRANS::XBox_CreateReturnedDataFile(): Could not open %s%s for 10 seconds.", szXBoxPath, szXBoxReturnValues);
        return E_FAIL;
    }

    m_Direction = eXBox_CreateReturnedDataFile;

    return hr;

};

/****************************************************************
****************************************************************/
HRESULT TRANS::XBox_ReadParameterDataFile()
{

    HRESULT hr = S_OK;
    FILE *tempfile = NULL;
    DWORD dwIterations = 0;
    char szString[512] = {0};

    //Wait for the file to appear and then open it.
    do
    {

        sprintf(szString, "%s%s", szXBoxPath, szXBoxParameters);
        m_file = fopen(szString, "rb");
        Sleep(100);
        if (dwIterations % 100 == 0)
            Output("Polling for %s to appear\n", szXBoxParameters);
        dwIterations++;
    } while (!m_file);

    m_Direction = eXBox_ReadParameterDataFile;

    return hr;
};


/****************************************************************
****************************************************************/
HRESULT TRANS::Client_CreateParameterDataFile()
{
    HRESULT hr = S_OK;
    char szString[512] = {0};
    BOOL bRet = FALSE;

    //Delete the ready file
//    sprintf(szString, "xbdel xt:\\trans\\%s", szXBoxReturnNotify);
//    SystemCall(szString);
    hr = XBoxDeleteFile(szXBoxReturnNotify, FALSE, FALSE);
    if (FAILED(hr))
    {
        Output("TRANS::Client_CreateParameterDataFile() failed to delete %s on the XBox", szXBoxReturnNotify);
        return hr;
    }


    // 2) delete the local parameter file.    
    bRet = DeleteFile(szXBoxParameters);
    if (!bRet)
    {
        Output("TRANS::Client_CreateParameterDataFile(): DeleteFile %s locally failed, must not have existed.  Oh well.", szXBoxParameters);
    }

    //Open up the file to send. 
    m_file = fopen(szXBoxParameters, "wb");
    if (NULL == m_file)
    {
        Output("TRANS::Client_CreateParameterDataFile(): Could not open %s locally", szXBoxParameters);
        hr = E_FAIL;
    }
    else
    {
        m_Direction = eClient_CreateParameterDataFile;
    }

    return hr;

};


/****************************************************************
This function must always be called, even if there's not returned
data!!
****************************************************************/
HRESULT TRANS::Client_ReadReturnedDataFile()
{

    HRESULT hr = S_OK;
    char szString[512] = {0};
    DWORD dwIterations = 0;

    //Wait for szXBoxReturned to show up so we can read the data.
    ClientWaitForXBoxReturned();

    //Copy the file locally, then delete the original!
    //sprintf(szString, "xbcp -y xt:\\trans\\%s .", szXBoxReturnValues);
    //SystemCall(szString);
    hr = XBoxReceiveFile(szXBoxReturnValues);
    if (FAILED(hr))
    {
        Output("TRANS::Client_ReadReturnedDataFile(): Could not retrieve %s", szXBoxReturnValues);
        return hr;
    }

    //sprintf(szString, "xbdel xt:\\trans\\%s", szXBoxReturnValues);
    //SystemCall(szString);
    hr = XBoxDeleteFile(szXBoxReturnValues, FALSE, TRUE);
    if (FAILED(hr))
    {
        Output("TRANS::Client_ReadReturnedDataFile(): Could not delete %s on XBox", szXBoxReturnValues);
        return hr;
    }

    //Open up the file to read.  It may not be done copying yet so loop for it.

    do 
    {
        m_file = fopen(szXBoxReturnValues, "rb");
        if (m_file)
            break;
        dwIterations++;
        Sleep(100);
    }
    while (dwIterations < 100);  //10 seconds


    if (NULL == m_file)
    {
        Output("TRANS::Client_ReadReturnedDataFile(): Could not open %s%s for 10 seconds.", szXBoxReturnValues);
        return E_FAIL;
    }

    m_Direction = eClient_ReadReturnedDataFile;
    return S_OK;
};


/****************************************************************
****************************************************************/
HRESULT TRANS::CloseDataFile()
{
HRESULT hr = S_OK;
DWORD dwIterations = 0;
char szString[512];

    //Close this data file.
    fclose(m_file);

    //If we're sending results, then
    // 1) xbcp the file over to the XBox.  
    // 2) delete the local file.    
    if (m_Direction  == eClient_CreateParameterDataFile)
    {

        // 1) Make sure the directory exists
        hr = XBoxMkdir("");
        if (FAILED(hr))
        {
            Output("TRANS::CloseDataFile(), eClient_CreateParameterDataFile: Could not make trans directory on XBox, must already exist.");
        }

        //SystemCall("xbmkdir xt:\\trans\\");

        //Send the file over.
        hr = XBoxSendFile(szXBoxParameters, TRUE);
        if (FAILED(hr))
        {
            Output("TRANS::CloseDataFile(), eClient_CreateParameterDataFile: Could not send %s to XBox", szXBoxParameters);
            return hr;
        }
        //sprintf(szString, "xbcp -y %s xt:\\trans\\", szXBoxParameters);        
        //SystemCall(szString);
        m_Direction = eNULL;
    }

    //If the client is receiving results (and we're done of course) then delete:
    //  The notification file on the XBox
    //  The data file on the XBox
    //  The notification file here
    //  The data file here.
    if (m_Direction  == eClient_ReadReturnedDataFile)
    {        

        
        //sprintf(szString, "xbdel xt:\\trans\\%s", szXBoxReturnNotify);
        //SystemCall(szString);
        hr = XBoxDeleteFile(szXBoxReturnNotify, FALSE, TRUE);
        if (FAILED(hr))
        {
            Output("TRANS::CloseDataFile(), eClient_ReadReturnedDataFile: Could not delete %s from XBox", szXBoxReturnNotify);
            return hr;
        }

        //sprintf(szString, "xbdel xt:\\trans\\%s", szXBoxReturnValues);
        //SystemCall(szString);
/*
        hr = XBoxDeleteFile(szXBoxReturnValues, FALSE, TRUE);
        if (FAILED(hr))
        {
            Output("TRANS::CloseDataFile(), eClient_ReadReturnedDataFile: Could not delete %s from XBox", szXBoxReturnValues);
            return hr;
        }
*/

//        DMSendFile(

        DeleteFile(szXBoxReturnNotify);
        DeleteFile(szXBoxReturnValues);
        m_Direction = eNULL;
    }

    //If we're sending results FROM the XBox, then delete the parameters and 
    //  create the file that notifies the client that the XBox's
    //  return values are ready.
    if (m_Direction  == eXBox_CreateReturnedDataFile)
    {
        
        //Delete old parameters.
        sprintf(szString, "%s%s", szXBoxPath, szXBoxParameters);
        DeleteFile(szString);
        
        //Create the file that notifies the client that the XBox's
        //  return values are ready.
        sprintf(szString, "%s%s", szXBoxPath, szXBoxReturnNotify);
        m_file = fopen(szString, "wb");
        fclose(m_file);
        m_Direction = eNULL;
    }


    return hr;


};



/****************************************************************
****************************************************************/
HRESULT TRANS::WriteMainBlockHeader(LPVOID pvXBoxThis, char *szMethodName, DWORD dwParamBlockCount)
{
    HRESULT hr = S_OK;
    MAINBLOCKHEADER MainBlockHeader = {0};

    MainBlockHeader.pvXBoxThis = pvXBoxThis;
    ZeroMemory((void *)MainBlockHeader.szMethodName, SIZE_METHODNAME);
    sprintf(MainBlockHeader.szMethodName, szMethodName);
    MainBlockHeader.dwParamBlockCount = dwParamBlockCount;

    //Write this structure.
    fwrite((void *)&MainBlockHeader, sizeof(MainBlockHeader),   1, m_file);
    return hr;

};


/****************************************************************
****************************************************************/
HRESULT TRANS::WriteParamBlockHeader(DWORD dwDataBlockCount)
{
    HRESULT hr = S_OK;
    PARAMBLOCKHEADER ParamBlockHeader = {0};
    ParamBlockHeader.dwDataBlockCount = dwDataBlockCount;
    fwrite((void *)&ParamBlockHeader, sizeof(ParamBlockHeader), 1, m_file);
    return hr;

};


/****************************************************************
****************************************************************/
HRESULT TRANS::WriteDataBlock (DATATYPE eDataType, DWORD dwParentDataBlock, DWORD dwParentDataOffset, DWORD dwDataSize, LPVOID pvData)
{

   HRESULT hr = S_OK;
   DATABLOCKHEADER DataBlockHeader;
   ZeroMemory(&DataBlockHeader, sizeof(DataBlockHeader));

    //Write the header
   DataBlockHeader.eDataType = eDataType;
   DataBlockHeader.dwParentDataBlock = dwParentDataBlock;
   DataBlockHeader.dwParentDataOffset = dwParentDataOffset;
   DataBlockHeader.dwDataSize = dwDataSize;
   fwrite((void *)&DataBlockHeader, sizeof(DataBlockHeader), 1, m_file);

   //Write the data
   fwrite(pvData, dwDataSize, 1, m_file);

   return hr;

};



/****************************************************************
****************************************************************/
HRESULT TRANS::ReadMainBlockHeader(LPVOID *ppvXBoxThis, char *szMethodNameOut, LPDWORD pdwParamBlockCount)
{
    HRESULT hr = S_OK;
    MAINBLOCKHEADER MainBlockHeader = {0};

    fread((void *)&MainBlockHeader,      sizeof(MainBlockHeader),   1, m_file);
    
    //You can pass NULL for these.
    if (ppvXBoxThis)
        *ppvXBoxThis = MainBlockHeader.pvXBoxThis;        
    if (szMethodNameOut)    
        memcpy(szMethodNameOut, MainBlockHeader.szMethodName, SIZE_METHODNAME);
    if (pdwParamBlockCount)
        *pdwParamBlockCount = MainBlockHeader.dwParamBlockCount;

    return hr;

};

/****************************************************************
****************************************************************/
HRESULT TRANS::ReadMainBlockHeader(MAINBLOCKHEADER *pMainBlockHeader)
{
    HRESULT hr = S_OK;
    fread((void *)pMainBlockHeader,      sizeof(MAINBLOCKHEADER),   1, m_file);
    return hr;

};



/****************************************************************
This returns an array of pointers, one for each parameter.  Each must be deleted.
The array must also be deleted after the individual pointers are deleted.

for (i=0; i<*pdwCount; i++)
    delete (ppv[i])[];
delete ppv[];

Make sure the file is at the paramblock.
****************************************************************/
HRESULT TRANS::ReadParameters(DWORD dwParamCount, LPDATABLOCK **pppDataBlocks)
{
    DWORD dwDataBlockCount = 0;
    LPDATABLOCK * ppDataBlocks = NULL;
    DWORD i=0, j=0;
    HRESULT hr = S_OK;

    //Allocate an array of LPDATABLOCKs - one for each in-parameter.  These will in turn point to 
    //  each parameter's array of datablocks.  NULL them out.
    ppDataBlocks = new LPDATABLOCK[dwParamCount];
    ZeroMemory((void *)ppDataBlocks, sizeof(LPDATABLOCK) * dwParamCount);

    //For each parameter block
    for (i=0; i<dwParamCount; i++)
    {
        //How many data blocks are there?
        fread((void *)&dwDataBlockCount, sizeof(DWORD), 1, m_file);

        //Allocate that many datablocks for this parameter.
        ppDataBlocks[i] = new DATABLOCK[dwDataBlockCount];

        for (j=0; j<dwDataBlockCount; j++)
        {
            //Read in the header for the datablock.
            fread((void *)&ppDataBlocks[i][j].Header, sizeof(DATABLOCKHEADER), 1, m_file);

            //Allocate room for and read in the data.
            ppDataBlocks[i][j].pvData = (LPVOID *)new BYTE[ppDataBlocks[i][j].Header.dwDataSize];
            fread(ppDataBlocks[i][j].pvData, ppDataBlocks[i][j].Header.dwDataSize, 1, m_file);

            //If this is by value, do a sanity check - make sure these unused values are both NULL.
            if (ByValue == ppDataBlocks[i][j].Header.eDataType)
            {
               if (ppDataBlocks[i][j].Header.dwParentDataBlock || ppDataBlocks[i][j].Header.dwParentDataOffset)
                  OutputDebugString("Error - dwParentDataBlock or dwParentDataOffset are not NULL!");

            }

            //If it's not by value, insert this pointer into the correct block *IF THE PARENT POINTER IS NOT NULL*
            else if (ppDataBlocks[i][j].Header.eDataType == PointerToMemory)
            {
                LPDATABLOCK pParentDataBlock;
                LPVOID *ppvDestination = NULL;

                //Index the array of data blocks for this parameter (i), then index that array with the appropriate parent block.
                //  This will denote the entire block, so take the address of that.                               
                pParentDataBlock = &ppDataBlocks[i][ppDataBlocks[i][j].Header.dwParentDataBlock];

                //Now cast that pointer to a byte, add the offset to it, and cast that back to a void pointer!
                ppvDestination = (LPVOID *)  &LPBYTE(pParentDataBlock->pvData)[ppDataBlocks[i][j].Header.dwParentDataOffset];
                                
                //If the original pointer was NULL, then don't point it to this data - instead, just delete the data.
                if (NULL != *ppvDestination)
                    *ppvDestination = ppDataBlocks[i][j].pvData;
                else
                {
                    delete []ppDataBlocks[i][j].pvData;
                    ppDataBlocks[i][j].pvData = NULL;
                }
            }
        }
    }

    *pppDataBlocks = ppDataBlocks;
    return hr;

};



/****************************************************************
Deletes an array of pointers allocated by ReadParameters.
****************************************************************/
HRESULT TRANS::DeleteAllocatedParameterList(DWORD dwCount, LPDATABLOCK *ppDataBlock)
{
DWORD i;
HRESULT hr = S_OK;

    //Go through the datablock array for all parameters.
    for (i=0; i<dwCount; i++)
    {
        //Delete the datablock array 
        if (ppDataBlock[i])
        {
            //BUGBUG: We'll leak memory for any void *'s that are allocated!!!!  Need to keep track of # of
            //  datablocks we allocate!!  Right now we can only tell that we allocated one, even if we
            //  may have allocated more.
            if (ppDataBlock[i][0].pvData)
            {
                delete []ppDataBlock[i][0].pvData;
                ppDataBlock[i][0].pvData = NULL;
            }

            //Delete the array of datablocks for this parameter.
            delete []ppDataBlock[i];
            ppDataBlock[i] = NULL;

        }
    }
    
    //Delete the entire array.
    delete []ppDataBlock;

    return hr;

};




void Output(LPSTR szFormat,...)
{
    va_list va;
    static char szBuffer[1000];

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);

    //A carriage return goes here when we've decided not to output to VC anymore!!
    OutputDebugString(szBuffer);

};






