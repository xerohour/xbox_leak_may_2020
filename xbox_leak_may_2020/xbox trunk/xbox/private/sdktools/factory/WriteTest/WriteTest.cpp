#include <xtl.h>
//#include <xboxp.h>
#include <xdbg.h>
#include "PerBoxData.h"

#if DBG
extern "C" ULONG XDebugOutLevel;
#endif

//======================== The main function
void __cdecl main()
{
    BYTE buffer[8000];
    HANDLE hf = INVALID_HANDLE_VALUE;
    HRESULT hr;
    DWORD dwDataLen;
    BOOL bResult;
    
#if DBG
    XDebugOutLevel = XDBG_EXIT;
#endif

    // Open Key File
    hf = CreateFile("T:\\perBoxImage.dat",
                  GENERIC_READ,
                  0,
                  NULL,
                  OPEN_EXISTING,
                  FILE_ATTRIBUTE_NORMAL,
                  NULL);
    if (hf == INVALID_HANDLE_VALUE)
    {
        hr = GetLastError();
        goto ErrorExit;
    }

    // Get file length
    dwDataLen = GetFileSize(hf, NULL);
    if (dwDataLen == 0xFFFFFFFF)
    {
        hr = GetLastError();
        goto ErrorExit;
    }

    ASSERT( dwDataLen <= sizeof(buffer) );
    
    // Read encoded key data
    bResult = ReadFile(hf,
                (LPVOID)buffer,
                dwDataLen,
                &dwDataLen,
                NULL);
    if (!bResult)
    {
        hr = GetLastError();
        goto ErrorExit;
    }

    hr = WritePerBoxData( buffer, dwDataLen );
    if ( FAILED( hr ) )
    {
        XDBGTRC("factory", "WritePerBoxData returned %x",hr);
        goto ErrorExit;        
    }

    hr = VerifyPerBoxData();
    if ( FAILED( hr ) )
    {
        XDBGTRC("factory", "VerifyPerBoxData returned %x",hr);
        goto ErrorExit;        
    }

ErrorExit:

    if ( hf != INVALID_HANDLE_VALUE )
    {
        CloseHandle(hf);
    }
    
    return;
}
