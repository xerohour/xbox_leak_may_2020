#pragma once

#undef UNICODE

#ifdef _XBOX
    #include <xtl.h>
    #include <xdbg.h>
    #include <xlog.h>
#endif

#ifdef WINDOWS
    #include <windows.h>
    #include <shellapi.h>
    #include <shlwapi.h>
    #include <xboxdbg.h>
#endif

#include "stdio.h"

enum DATATYPE {ByValue, PointerToMemory, PointerToFile};
enum SIDE     {SideClient, SideXBox};
enum DIRECTION {eNULL, eXBox_CreateReturnedDataFile, eXBox_ReadParameterDataFile, eClient_CreateParameterDataFile, eClient_ReadReturnedDataFile};

struct MAINBLOCKHEADER
{
    LPVOID pvXBoxThis;
    char   szMethodName[256];
    DWORD  dwParamBlockCount;
};

struct PARAMBLOCKHEADER
{
    DWORD dwDataBlockCount;
};

struct DATABLOCKHEADER
{
    DATATYPE eDataType;
    DWORD    dwParentDataBlock;
    DWORD    dwParentDataOffset;
    DWORD    dwDataSize;
};

typedef struct _tagDATABLOCK
{
    DATABLOCKHEADER Header;
    LPVOID   pvData;
} DATABLOCK, *LPDATABLOCK;



/********************************************************************************
Does the XBox side of the transactions.
********************************************************************************/
class TRANS
{


public:
    TRANS(void);
    ~TRANS(void);
    
    //File IO creation/destroy
    HRESULT XBox_Init(void);
    HRESULT XBox_CreateReturnedDataFile(void);
    HRESULT XBox_ReadParameterDataFile(void);
    HRESULT Client_CreateParameterDataFile(void);
    HRESULT Client_ReadReturnedDataFile(void);    
    HRESULT CloseDataFile(void);

    //Writing/Reading
    HRESULT WriteMainBlockHeader(LPVOID pvXBoxThis, char *szMethodName, DWORD dwParamCount);
    HRESULT WriteParamBlockHeader(DWORD dwDataBlockCount);
    HRESULT WriteDataBlock (DATATYPE datatype, DWORD ParentDataBlock, DWORD ParentDataOffset, DWORD dwDataSize, LPVOID pvData);
    
    HRESULT ReadMainBlockHeader(LPVOID *ppvThis, char *szMethodName, LPDWORD dwParamCount);
    HRESULT ReadMainBlockHeader(MAINBLOCKHEADER *pMainBlockHeader);

    HRESULT ReadParameters(DWORD dwParamCount, LPDATABLOCK **pppDataBlocks);

    HRESULT DeleteAllocatedParameterList(DWORD dwCount, LPDATABLOCK *ppDataBlock);

    //Waiting
    HRESULT ClientWaitForXBoxReturned(void);

    HRESULT Client_CopyFileToXBox(char *szSrcFile);
    HRESULT Client_CopyDMusicFileToXBox(char *szSrcFile);


private:
    DIRECTION m_Direction;
    FILE *m_file;
};


#define SIZE_METHODNAME 256
extern char *szDataOut;

void Output(LPSTR szFormat,...);

//Sends a file from the given location to xt:\trans.
HRESULT XBoxSendFile(char *szName, BOOL bWait);
