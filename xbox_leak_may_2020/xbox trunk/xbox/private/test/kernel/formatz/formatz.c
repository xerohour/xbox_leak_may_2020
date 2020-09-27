/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    formatz.c

Abstract:

    test out the XFormatUtilityDrive() API

Author:

    John Daly (johndaly) 23-jan-2002

Environment:

    X-Box

Notes:

Revision History:

    23-jan-2002     johndaly
    created

Current Status:


--*/

//
// includes
//

#include "formatz.h"

void 
__cdecl 
main(
    void
    )
/*++

Routine Description:

    main
    
Arguments:

    None

Return Value:

    None

--*/
{
    
    //
    // call the tests
    //

    test_XFormatUtilityDriveNoFile();
    test_XFormatUtilityDriveOneFile();

    return;

}

VOID
WINAPI
test_XFormatUtilityDriveNoFile(
    void
    )
/*++

Routine Description:

    run iterations of XFormatUtilityDrive with no open files

Arguments:

    HANDLE LogHandle

Return Value:

    None

--*/
{
    DWORD x;

    for (x = 0; x < 10000; x++) {
        if(FALSE == XFormatUtilityDrive()) {
            OutputDebugStringA("ERROR: XFormatUtilityDrive failed - no files open scenario\r\n");
        } else {
            //OutputDebugStringA("XFormatUtilityDrive succeeded - no files open scenario\r\n");
        }
    }
    return;
}

VOID
WINAPI
test_XFormatUtilityDriveOneFile(
    void
    )
/*++

Routine Description:

    run iterations of XFormatUtilityDrive with an open file

Arguments:

    HANDLE LogHandle

Return Value:

    None

--*/
{
    DWORD x,y;
    char FileDataBuffer[] = {"abcdefghijklmnopqrstuvwxyz0123456789"};
    char dataname[MAX_PATH] = {0};
    DWORD FileCount = 1;
    HANDLE hFile;
    DWORD dwBytesWritten;

    for (y = 0; y < 10000; y++) {
        
        //
        // open a file(s)
        //

        for (x = 0; x < FileCount; x++) {

            sprintf(dataname, "Z:\\data%4.4x.dat", x);

            hFile = CreateFile(dataname,
                               GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ,
                               NULL,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);

            if (INVALID_HANDLE_VALUE == hFile) {
                OutputDebugStringA("ERROR: CreateFile failed while saving data (title)\r\n");
                continue;
            }

            //
            // write some easily understood pattern of data to the file
            //

            if (0 == WriteFile(hFile,
                               FileDataBuffer,
                               sizeof(FileDataBuffer),
                               &dwBytesWritten,
                               NULL)) {
                OutputDebugStringA("ERROR: WriteFile failed while saving data (title)\r\n");
                break;
            }
        }

        if (FALSE == XFormatUtilityDrive()) {
            OutputDebugStringA("ERROR: XFormatUtilityDrive failed - no files open scenario\r\n");
        } else {
            //OutputDebugStringA("XFormatUtilityDrive succeeded - no files open scenario\r\n");
        }
        
        CloseHandle(hFile);
    }

    return;

}

