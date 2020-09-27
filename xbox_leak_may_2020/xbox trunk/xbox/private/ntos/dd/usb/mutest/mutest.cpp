/*++

Copyright (c) 1996-2000 Microsoft Corporation

Module Name:

    MUTEST.CPP

Abstract:

    This source file contains a simple test code for the MU

Environment:

    kernel mode

Revision History:

--*/

//*****************************************************************************
// I N C L U D E S
//*****************************************************************************

#include <xtl.h>
#include <xboxp.h>

#include <mutest.h>
#include <devioctl.h>
#include <ntdddisk.h>
#include <stdio.h>

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))

VOID
ReadWriteTest(
          CHAR DriveLetter
          );

VOID
ReadWriteScatterGatherTest(
          CHAR DriveLetter
          );

VOID
MUNameTest(
          CHAR DriveLetter
          );

VOID
DumpLogicalDriveList()
{
    CHAR szDriveStrings[] = "D:\\\0F:\\\0G:\\\0H:\\\0I:\\\0J:\\\0K:\\\0L:\\\0M:\\\0T:\\\0U:\\\0X:\\";
    LPSTR pszDrive;

    DebugPrint("Logical drive list:");
    pszDrive = szDriveStrings;
    while ('\0' != *pszDrive)
    {
        ULARGE_INTEGER ullFreeAvail, ullFree, ullTotal;
        if (GetDiskFreeSpaceEx(pszDrive, &ullFreeAvail, &ullTotal, &ullFree))
        {
            DebugPrint(" %s", pszDrive);
        }
        // Seek until next NULL:
        while (L'\0' != *pszDrive)
        {
            pszDrive++;
        }
        // Skip over the NULL:
        pszDrive++;
    }
    DebugPrint("\n");
}

VOID
WINAPI
GetDriveTypeTest()
{
    CHAR szDriveStrings[] = "D:\\\0F:\\\0G:\\\0H:\\\0I:\\\0J:\\\0K:\\\0L:\\\0M:\\\0T:\\\0U:\\\0X:\\";
    LPSTR pszDrive;

    // GetDriveType test
    DebugPrint("Logical drive list:\n");
    pszDrive = szDriveStrings;
    while (L'\0' != *pszDrive)
    {
        ULARGE_INTEGER ullFreeAvail, ullFree, ullTotal;
        if (GetDiskFreeSpaceEx(pszDrive, &ullFreeAvail, &ullTotal, &ullFree))
        {
            CHAR szLabel[MAX_PATH], szFileSystem[MAX_PATH];
            DWORD dwSerial, dwMaxNameLength, dwFSFlags;

            // Shift into a dword (right shift 20 and label as MB) to we can print it out..
            ullFreeAvail.LowPart = (ullFreeAvail.LowPart >> 20) | (ullFreeAvail.HighPart << 12);
            ullTotal.LowPart = (ullTotal.LowPart >> 20) | (ullTotal.HighPart << 12);
            ullFree.LowPart = (ullFree.LowPart >> 20) | (ullFree.HighPart << 12);

            if (GetVolumeInformation(pszDrive,
                                     szLabel,
                                     ARRAYSIZE(szLabel),
                                     &dwSerial,
                                     &dwMaxNameLength,
                                     &dwFSFlags,
                                     szFileSystem,
                                     ARRAYSIZE(szFileSystem)))
            {
                DebugPrint("Drive %s (label \"%s\", serial 0x%08x, maxname %lu, fsflags %lu, fs \"%s\")\n"
                           "          (total %lu MB, free %lu MB, avail %lu MB)\n",
                            pszDrive, szLabel, dwSerial, dwMaxNameLength, dwFSFlags, szFileSystem,
                            ullTotal.LowPart, ullFree.LowPart, ullFreeAvail.LowPart);
            }
            else
            {
                DWORD dwErr = GetLastError();
                DebugPrint("GetVolumeInformation() failed for %s: GetLastError()=0x%08x\n", pszDrive, dwErr);
            }
        }
        // Seek until next NULL:
        while (L'\0' != *pszDrive)
        {
            pszDrive++;
        }
        // Skip over the NULL:
        pszDrive++;
    }
    DebugPrint("\n");
}

VOID
WINAPI
FindFirstFindNextTest(LPCSTR pcszFilter)
{
    // FindFirstFile/FindNextFile/FindClose test
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile(pcszFilter, &wfd);
    if (INVALID_HANDLE_VALUE != hFind)
    {
        DebugPrint("Directory listing for %s\n", pcszFilter);
        do
        {
            DebugPrint("%s, size=0x%08x, attributes=\n", wfd.cFileName,
                wfd.nFileSizeLow, wfd.dwFileAttributes);
            
        } while (FindNextFile(hFind, &wfd));
        
        FindClose(hFind);
    }
    else
    {
        DWORD dwErr = GetLastError();
        DebugPrint("FindFirstFile() failed for %s: GetLastError()=0x%08x\n", pcszFilter, dwErr);
    }
    DebugPrint("\n");
}


void __cdecl main()
{
    DWORD err;
    ULONG DeviceTypes = 0xFFFF0000;//USB_DEVICE_TYPE_MU;
    CHAR szFilter[] = "C:\\*";
    DWORD dwInsertions, dwRemovals;
    int i;
    
    DebugPrint("MUTEST: Built on %s at %s\n", __DATE__,__TIME__);
    
    //Initialize core peripheral port support
    XInitDevices(0,NULL);
    
    DebugPrint("MUTEST: Polling for device insertions/removals..\n");

    while (TRUE) {

        if(XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &dwInsertions, &dwRemovals))
        {
        
            DebugPrint("MUTEST: Insertions 0x%08x, Removals 0x%08x\n", dwInsertions, dwRemovals);

            for(i=0; i < 32; i++)
            {
                if(dwRemovals&(1 << i))
                {
                    DWORD dwSlot;
                    DWORD dwPort;
                    if(i < 16)
                    {
                        dwPort = i;
                        dwSlot = XDEVICE_TOP_SLOT;
                    } else
                    {
                        dwPort = i-16;
                        dwSlot = XDEVICE_BOTTOM_SLOT;
                    }
                    XUnmountMU(dwPort, dwSlot);
                }
                //
                //  Handle Insertions
                //
                if(dwInsertions&(1 << i))
                {
                    CHAR chDrive;
                    DWORD dwSlot;
                    DWORD dwPort;
                    if(i < 16)
                    {
                        dwPort = i;
                        dwSlot = XDEVICE_TOP_SLOT;
                    } else
                    {
                        dwPort = i-16;
                        dwSlot = XDEVICE_BOTTOM_SLOT;
                    }
                    if(dwPort >= 4)
                    {
                        DebugPrint("ERROR: MU in illegal port, p=%d, s=%s\n", dwPort, (XDEVICE_TOP_SLOT==dwSlot) ? "XDEVICE_TOP_SLOT" : "XDEVICE_BOTTOM_SLOT" );
                    }

                    LowLevelTest(dwPort, dwSlot);


                    DWORD dwErr = XMountMU(dwPort, dwSlot, &chDrive);

                    if (ERROR_SUCCESS == dwErr)
                    {

                        DebugPrint("XMountMU() succeeded, dr=%lc, p=%d, s=%d\n",
                                   chDrive,
                                   XMUPortFromDriveLetter(chDrive),
                                   XMUSlotFromDriveLetter(chDrive));

                        GetDriveTypeTest();

                        szFilter[0] = chDrive;

                        FindFirstFindNextTest(szFilter);

                        ReadWriteTest(chDrive);

                        ReadWriteScatterGatherTest(chDrive);

                        FindFirstFindNextTest(szFilter);

                        MUNameTest(chDrive);
                    }
                    else
                    {
                        DebugPrint("XMountMU() failed, error = 0x%08x\n", dwErr);
                    }
                }
            } // for loop
            DumpLogicalDriveList();
        } // if

    } // while
}


#define TEST_BUFFER_SIZE 8192
#define EXTRA_SIZE 8192
#define SECTOR_MASK 0xFFFFE000

VOID
ReadWriteTest(
          CHAR DriveLetter
          )
{

	ULONG					tryCount = 0;
    CHAR                    name[64];
    HRESULT                 hr;
    HANDLE                  hFile;
    PUCHAR buffer,buffer2;
    UCHAR pattern[4] = {1,2,3,4};
    ULONG i;

    sprintf(name,"C:\\test.dat");

    name[0] = DriveLetter;

    hFile = CreateFile(name,
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_FLAG_WRITE_THROUGH,
                    NULL,
                    OPEN_ALWAYS,
                    0,
                    NULL);

    if(INVALID_HANDLE_VALUE == hFile) {

        hr = GetLastError();
        return;
    }

    buffer =  (PUCHAR)LocalAlloc(LPTR, TEST_BUFFER_SIZE+EXTRA_SIZE);
    buffer2 =  (PUCHAR)LocalAlloc(LPTR, TEST_BUFFER_SIZE+EXTRA_SIZE);

    //
    // sector align buffers
    //

    buffer = (PUCHAR)((ULONG) buffer & SECTOR_MASK);
    buffer += EXTRA_SIZE;

    buffer2 = (PUCHAR)((ULONG) buffer2 & SECTOR_MASK);
    buffer2 += EXTRA_SIZE;

    //
    // fill the buffer with a pattern
    //

    for (i=0;i<TEST_BUFFER_SIZE;i+=sizeof(pattern)) {

        RtlCopyMemory(buffer+i,pattern,sizeof(pattern));

    }

    DebugPrint("MUTEST: Test buffer 1 at %x, 2 at %x\n",buffer,buffer2);

    DebugPrint("MUTEST: Writing pattern to file\n");

    //
    // write the pattern to the file, close the file then read it
    //

    if (!WriteFile(hFile,
                  buffer,
                  TEST_BUFFER_SIZE,
                  &i,
                  NULL)) {

        hr = GetLastError();
        DebugPrint("MUTEST: Write Failed with %x\n",hr);

        LocalFree(buffer);
        LocalFree(buffer2);

    }

    DebugPrint("MUTEST: Closing file\n");

    //
    // close the file
    //

    CloseHandle(hFile);

    DebugPrint("MUTEST: Re-opening file\n");

    //
    // open the file and read from it
    //

    hFile = CreateFile(name,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_FLAG_NO_BUFFERING,
                NULL,
                OPEN_ALWAYS,
                0,
                NULL);

    if(INVALID_HANDLE_VALUE == hFile) {

        hr = GetLastError();
        return;
    }

    DebugPrint("MUTEST: Reading pattern to file\n");

    if (!ReadFile(hFile,
                  buffer2,
                  TEST_BUFFER_SIZE,
                  &i,
                  NULL)) {

        hr = GetLastError();
        DebugPrint("MUTEST: Read Failed with %x\n",hr);

        LocalFree(buffer);
        LocalFree(buffer2);

    }

    CloseHandle(hFile);

    DebugPrint("MUTEST: Success: Test buffer at %x\n",buffer);
}


#define PAGE_SIZE 4096
#define TEST_SG_BUFFER_SIZE (PAGE_SIZE * 5)

VOID
ReadWriteScatterGatherTest(
          CHAR DriveLetter
          )
{

	ULONG					tryCount = 0;
    CHAR                    name[64];
    HRESULT                 hr;
    HANDLE                  hFile;
    PUCHAR buffer,buffer2;
    FILE_SEGMENT_ELEMENT sg1[TEST_SG_BUFFER_SIZE / PAGE_SIZE + 1];
    FILE_SEGMENT_ELEMENT sg2[TEST_SG_BUFFER_SIZE / PAGE_SIZE + 1];
    UCHAR pattern[4] = {1,2,3,4};
    ULONG i;
    OVERLAPPED ov;

    sprintf(name,"C:\\testsg.dat");

    name[0] = DriveLetter;

    hFile = CreateFile(name,
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_FLAG_WRITE_THROUGH,
                    NULL,
                    OPEN_ALWAYS,
                    FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
                    NULL);

    if(INVALID_HANDLE_VALUE == hFile) {

        hr = GetLastError();
        return;
    }

    buffer =  (PUCHAR)VirtualAlloc(NULL,TEST_SG_BUFFER_SIZE,MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    buffer2 =  (PUCHAR)VirtualAlloc(NULL,TEST_SG_BUFFER_SIZE,MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    ZeroMemory(sg1, sizeof(sg1));
    ZeroMemory(sg2, sizeof(sg2));

    for (i = 0; i < TEST_SG_BUFFER_SIZE / PAGE_SIZE; i++) {
        sg1[i].Buffer = buffer + i * PAGE_SIZE;
        sg2[i].Buffer = buffer2 + i * PAGE_SIZE;
    }

    //
    // fill the buffer with a pattern
    //

    for (i=0;i<TEST_SG_BUFFER_SIZE;i+=sizeof(pattern)) {

        RtlCopyMemory(buffer+i,pattern,sizeof(pattern));

    }

    DebugPrint("MUTEST: Scatter/gather test\n");

    DebugPrint("MUTEST: Test S/G buffer 1 at %x, 2 at %x\n",buffer,buffer2);

    DebugPrint("MUTEST: Writing pattern to file\n");

    //
    // write the pattern to the file, close the file then read it
    //

    ZeroMemory(&ov, sizeof(ov));

    if (!WriteFileGather(hFile,
                  sg1,
                  TEST_SG_BUFFER_SIZE,
                  NULL,
                  &ov)) {

        hr = GetLastError();

        if (hr != ERROR_IO_PENDING ||
            !GetOverlappedResult(hFile, &ov, &i, TRUE)) {

            hr = GetLastError();

            DebugPrint("MUTEST: Write Failed with %x\n",hr);
        }
    }

    DebugPrint("MUTEST: Closing file\n");

    //
    // close the file
    //

    CloseHandle(hFile);

    DebugPrint("MUTEST: Re-opening file\n");

    //
    // open the file and read from it
    //

    hFile = CreateFile(name,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_FLAG_NO_BUFFERING,
                NULL,
                OPEN_ALWAYS,
                FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
                NULL);

    if(INVALID_HANDLE_VALUE == hFile) {

        hr = GetLastError();
        return;
    }

    DebugPrint("MUTEST: Reading pattern to file\n");

    ZeroMemory(&ov, sizeof(ov));

    if (!ReadFileScatter(hFile,
                  sg2,
                  TEST_SG_BUFFER_SIZE,
                  NULL,
                  &ov)) {

        hr = GetLastError();

        if (hr != ERROR_IO_PENDING ||
            !GetOverlappedResult(hFile, &ov, &i, TRUE)) {

            hr = GetLastError();

            DebugPrint("MUTEST: Read Failed with %x\n",hr);
        }
    }

    CloseHandle(hFile);

    DebugPrint("MUTEST: Success: Test S/G buffer at %x\n",buffer);
}

VOID
MUNameTest(
          CHAR DriveLetter
          )
{
    DWORD error;
    WCHAR MUName[MAX_MUNAME];

    RtlFillMemory(MUName, sizeof(MUName), 0xCC);

    error = XMUWriteNameToDriveLetter(DriveLetter, L"Test Name");

    if (error != ERROR_SUCCESS) {
        DebugPrint("MUTEST: Failed to write MU name with %x\n", error);
    }

    error = XMUNameFromDriveLetter(DriveLetter, MUName, MAX_MUNAME);

    if (error == ERROR_SUCCESS) {

        if (lstrcmpW(MUName, L"Test Name") != 0) {
            DebugPrint("MUTEST: Failed to read back correct MU name\n");
        }

    } else {
        DebugPrint("MUTEST: Failed to read MU name with %x\n", error);
    }
}
