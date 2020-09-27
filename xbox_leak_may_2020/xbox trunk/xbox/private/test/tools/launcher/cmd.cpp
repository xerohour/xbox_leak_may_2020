/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    cmd.cpp

Abstract:

    Extra commands. See dos.cpp for dos commands.

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    07-31-2000  Created

*/
#include "stdafx.h"
#include <xtestlib.h>
#include <heap.h>
#include <xbox.h>
#include <stdlib.h>
#include <math.h>

#include "launcherx.h"
#include "harnessapi.h"
#include "draw.h"

namespace Launcher {


DWORD BARTEST(SOCKET sock, char *nextToken)
    {
    SockPrint(sock, "Check the video output for the bars\r\n");

    drInit();
    drCls();

    drBox(  0,  20,  79, 480, 0xFFFFFFFF);
    drBox( 80,  20, 159, 480, 0xFF777777);
    drBox(160,  20, 239, 480, BUILDARGB(0xFF, 0xFF, 0, 0));
    drBox(240,  20, 319, 480, 0xFFFFFF00);
    drBox(320,  20, 399, 480, 0xFF00FFFF);
    drBox(400,  20, 479, 480, 0xFF00FF00);
    drBox(480,  20, 559, 480, 0xFF0000FF);
    drBox(560,  20, 639, 480, 0xFFFF00FF);
    drBox(  0, 320, 639, 400, 0x77000000);
    drBox(  0, 400, 639, 480, 0x77FF0000);

    drPrintf(5, 5, L"Graphic Bar Test");
    drPrintf(2, 340, L"Black");
    drPrintf(2, 360, L"Mix");
    drPrintf(2, 420, L"Red");
    drPrintf(2, 440, L"Mix");

    drShowScreen();

    drDeinit();

    return ERR_OK;
    }


DWORD DISKIO(SOCKET sock, char *nextToken)
    {
    // DISK "IN/OUT" Test for hardware guys
    bool exit = false;

    DWORD wSize, rSize, size;
    char *wbuffer = new char[MEGABYTE*3];
    if(!wbuffer)
        {
        SockPrint(sock, "Unable to allocate memory\r\n");
        return 0;
        }

    char filename[64];
    char drive;
    char op[10];

    if(!*nextToken)
        {
        drive = 'F';
        }
    else
        {
        sscanf(nextToken, "%s %c", op, &drive);
        }
    sprintf(filename, "%c:\\IOtest.txt", drive);

    HANDLE hFile = CreateFile(filename, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
        {
        SockPrint(sock, "Unable to open file: %s (ec: %ld)\r\n", filename, GetLastError());
        return 0;
        }

    SockPrint(sock, "Writing initial file: %s...\r\n", filename);
    memset(wbuffer, 0, MEGABYTE*2);
    unsigned count = 1;
    for(size=0; size<MEGABYTE*2; size+=64)
        {
        //wbuffer[size] = (char)size;
        (*(unsigned*)&wbuffer[size]) = count++;
        }
    wbuffer[MEGABYTE*2-1] = 0x0F;
    wbuffer[MEGABYTE*2-2] = 0x0F;
    wbuffer[MEGABYTE*2-3] = 0x0F;
    wbuffer[MEGABYTE*2-4] = 0x00;
    wbuffer[MEGABYTE*2-5] = 0x01;
    wbuffer[MEGABYTE*2-6] = 0x01;
    wbuffer[MEGABYTE*2-7] = 0x01;
    wbuffer[MEGABYTE*2-8] = 0x00;
    wbuffer[MEGABYTE*2-9] = 0x0F;
    wbuffer[MEGABYTE*2-10] = 0x0F;
    wbuffer[MEGABYTE*2-11] = 0x0F;

    if(! (WriteFile(hFile, wbuffer, size, &wSize, NULL) == TRUE && wSize == size))
        {
        SockPrint(sock, "Unable to write the file (bytes: %u) (ec: %ld)\r\n", wSize, GetLastError());
        CloseHandle(hFile);
        return 0;
        }

    SockPrint(sock, "Running DISKIO (%s) - press any key to stop.\r\n", op[0]=='O'||op[0]=='o'?"OUT":"IN");
    while(!exit)
        {
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

        if(op[0]=='O' || op[0]=='o') // OUT (write)
            {
            if(WriteFile(hFile, wbuffer, size, &wSize, NULL) != TRUE)
                SockPrint(sock, "Error: WriteFile[%d] (ec: %ld)\r\n", wSize, GetLastError());
            }
        else // IN (read)
            {
            if(ReadFile(hFile, wbuffer, size, &rSize, NULL) != TRUE)
                SockPrint(sock, "Error: ReadFile[%d] (ec: %ld)\r\n", rSize, GetLastError());
            }

        // check for user interaction
        if(NetHit(sock))
            {
            exit = true;
            }
        }

    CloseHandle(hFile);

    return ERR_OK;
    }


DWORD DUMP(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "DUMP");
        return ERR_SYNTAX;
        }

    DWORD len = 0;
    const unsigned chunkSize = 1024;
    unsigned char *buff = new unsigned char[chunkSize];
    if(!buff)
        {
        SockPrint(sock, "Error: Not enough memory.\r\n");
        return ERR_NOMEM;
        }

    HANDLE hFile = CreateFile(nextToken, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
        {
        SockPrint(sock, "Could not open file (ec: %lu)\r\n", GetLastError());
        delete[] buff;
        return ERR_FILE;
        }
    for(DWORD i=0; i<GetFileSize(hFile, NULL); i+=chunkSize)
        {
        ReadFile(hFile, buff, chunkSize, &len, NULL);
        HexDump(sock, buff, len, i);
        }

    CloseHandle(hFile);
    delete[] buff;

    return ERR_OK;
    }


DWORD FLUSH(SOCKET sock, char *nextToken)
    {
    FlushDiskCache();
    return ERR_OK;
    }


DWORD HELP(SOCKET sock, char *nextToken)
    {
    // individual help texts
    if(*nextToken)
        {
        for(unsigned i=0; i<commandLength; i++)
            {
            if(strcmp(commands[i].name, nextToken) == 0)
                {
                SockPrint(sock, "%s\r\n\r\n%s\r\n", commands[i].helpShort, commands[i].helpLong);
                return ERR_OK;
                }
            }
        for(i=0; i<hiddenCommandLength; i++)
            {
            if(strcmp(hiddenCommands[i].name, nextToken) == 0)
                {
                SockPrint(sock, "%s\r\n\r\n%s\r\n", hiddenCommands[i].helpShort, hiddenCommands[i].helpLong);
                return ERR_OK;
                }
            }
        }

    // print general help
    SockPrint(sock, "XBox Harness Remote Commands:\r\n");
    SockPrint(sock, "For more information on a specific command, type HELP command-name.\r\n\r\n");
    for(unsigned i=0; i<commandLength; i++)
        {
        SockPrint(sock, "%-8s %s\r\n", commands[i].name, commands[i].helpShort);
        }

    return ERR_OK;
    }


DWORD HOTPLUG(SOCKET sock, char *nextToken)
    {
    if(*nextToken)
        {
        if(_stricmp(nextToken, "1") == 0 || _stricmp(nextToken, "ON") == 0)
            usbmanager->hotplugs = true;
        else if(_stricmp(nextToken, "0") == 0 || _stricmp(nextToken, "OFF") == 0)
            usbmanager->hotplugs = false;

        SockPrint(sock, "Hot plugs are now %s\r\n", usbmanager->hotplugs?"ON":"OFF");
        return ERR_OK;
        }

    SockPrint(sock, "Hot plugs are currently %s\r\n", usbmanager->hotplugs?"ON":"OFF");

    return ERR_OK;
    }


DWORD MF(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "MF");
        return ERR_SYNTAX;
        }

    char *filename = new char[1024];
    char *pattern = new char[40];
    unsigned size;
    unsigned items;
    DWORD len;

    items = sscanf(nextToken, "%s %u %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x", filename, &size, &pattern[0], &pattern[1], &pattern[2], &pattern[3], &pattern[4], &pattern[5], &pattern[6], &pattern[7], &pattern[8], &pattern[9], &pattern[10], &pattern[11], &pattern[12], &pattern[13], &pattern[14], &pattern[15], &pattern[16], &pattern[17], &pattern[18], &pattern[19]);
    items -= 2;

    HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);

    if(hFile == INVALID_HANDLE_VALUE)
        {
        SockPrint(sock, "Could not open file (ec: %lu)\r\n", GetLastError());
        delete[] filename;
        delete[] pattern;
        return ERR_FILE;
        }

    for(unsigned i=0; i<size; i+=items)
        {
        if(i+items > size) items = size-i;
        if(WriteFile(hFile, pattern, items, &len, NULL) == FALSE)
            {
            SockPrint(sock, "          Write failed (ec: %lu)\r\n", GetLastError());
            break;
            }

        // check for user interaction
        if(NetHit(sock))
            {
            SockPrint(sock, "          Write failed - user interrupted.\r\n");
            break;
            }
        }

    if(i >= size)
        {
        SockPrint(sock, "          Created file: %s[%u]\r\n", filename, i);
        }

    CloseHandle(hFile);

    delete[] filename;
    delete[] pattern;

    return ERR_OK;
    }


#pragma data_seg( ".EXP$A" )
EXP_TABLE_DIRECTORY __export_table_directory_start[] = { NULL };
#pragma data_seg( ".EXP$Z" )
EXP_TABLE_DIRECTORY __export_table_directory_end[] = { NULL };
#pragma data_seg()
#pragma comment(linker, "/merge:.EXP=.export")

DWORD MODS(SOCKET sock, char *nextToken)
    {
    /*
    char *lpszModuleName_A;
    PRIVATE_HEAP_RECORD *pHeapRecords;
    DWORD dwHeap;

    // Allocate the memory for the private heap records
    pHeapRecords = new PRIVATE_HEAP_RECORD[MAX_HEAP_HANDLE_TRACKING];
    if(!pHeapRecords)
        {
        SockPrint(sock, "Error: Not enough memory.\r\n");
        return ERR_NOMEM;
        }

    // Get the private heap records
    if (FALSE == GetHeapTrackingDatabase(pHeapRecords, MAX_HEAP_HANDLE_TRACKING * sizeof(PRIVATE_HEAP_RECORD)))
        {
        SockPrint(sock, "Error: No modules found (ec: %lu).\r\n", GetLastError());
        delete[] pHeapRecords;
        return ERR_OK;
        }

    SockPrint(sock, "Available modules callable with RUN:\r\n");
    for(dwHeap = 0; dwHeap<MAX_HEAP_HANDLE_TRACKING; dwHeap++)
        {
        // Get the module name
        if(pHeapRecords[dwHeap].HeapHandle == NULL) continue;
        lpszModuleName_A = XeModuleNameFromModuleIndex(pHeapRecords[dwHeap].OwnerModuleIndex);

        // print it out if necessary
        _strlwr(lpszModuleName_A);
        if(strcmp(lpszModuleName_A, "launcher") == 0) continue;
        if(strcmp(lpszModuleName_A, "xlog") == 0) continue;
        SockPrint(sock, "   %s\r\n", lpszModuleName_A);
        }

    delete[] pHeapRecords;
    */

    PEXP_TABLE_DIRECTORY _ptbegin = __export_table_directory_start;
    PEXP_TABLE_DIRECTORY _ptend = __export_table_directory_end;

    SockPrint(sock, "Available modules callable with RUN:\r\n");
    for(; _ptbegin < _ptend; _ptbegin++)
        {
        if(!_ptbegin->ModuleName) continue;
        if(_stricmp(_ptbegin->ModuleName, "launcher") == 0) continue;

        SockPrint(sock, "   %s\r\n", _ptbegin->ModuleName);
        }

    return ERR_OK;
    }



DWORD MU(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "MU");
        return ERR_SYNTAX;
        }

    char* operation = strchr(nextToken, ' ');
    if(!operation)
        {
        HELP(sock, "MU");
        return ERR_SYNTAX;
        }
    ++operation; // skip the space

    char drive = nextToken[0];
    unsigned port = XMUPortFromDriveLetter(drive);
    unsigned slot = XMUSlotFromDriveLetter(drive);
    char deviceName[128];

    if(_stricmp(operation, "MOUNT") == 0)
        {
        slot = slot==XDEVICE_TOP_SLOT? 1 : 2;

        if(usbmanager->devices[port][slot] != NULL && usbmanager->devices[port][slot]->type != XDEVICE_TYPE_MEMORY_UNIT)
            {
            SockPrint(sock, "The port (%02X, %02X) must be empty or hold an MU!\r\n", port, slot);
            return ERR_OK;
            }

        if(usbmanager->devices[port][slot] != NULL && usbmanager->devices[port][slot]->type == XDEVICE_TYPE_MEMORY_UNIT)
            {
            SockPrint(sock, "Unmounting %c... ", drive);
            delete (DeviceMU*)(usbmanager->devices[port][slot]);
            usbmanager->devices[port][slot] = NULL;
            }

        SockPrint(sock, "Attempting to mount %c... ", drive);
        usbmanager->devices[port][slot] = new DeviceMU(port, slot);

        if(((DeviceMU*)(usbmanager->devices[port][slot]))->drive != drive)
            {
            delete (DeviceMU*)(usbmanager->devices[port][slot]);
            usbmanager->devices[port][slot] = NULL;
            SockPrint(sock, "Mount failed.\r\n");
            }
        else
            {
            SockPrint(sock, responseOk);
            }
        }
    else if(_stricmp(operation, "BADBLOCK") == 0)
        {
        #ifdef MU_DIAGNOSTIC_IOCTLS
            USHORT table[16+1];
            DWORD size = MU_VSC_BAD_BLOCK_TABLE_SIZE;
            SockPrint(sock, "MU Bad Block Map:\r\n");
            if(MUDiagnosticIoctl(sock, port, slot, MU_IOCTL_GET_BAD_BLOCK_TABLE, table, size))
                {
                for(unsigned i=0; i<16; i++)
                    SockPrint(sock, "  [%X] %04X\r\n", i, table[i]);
                }
        #else
            SockPrint(sock, "MU Diagnostics not enabled in this build - C_DEFINES MU_DIAGNOSTIC_IOCTLS\r\n");
        #endif
        }
    else if(_stricmp(operation, "MEMTEST") == 0)
        {
        #ifdef MU_DIAGNOSTIC_IOCTLS
            USHORT count;
            DWORD size = sizeof(USHORT);
            SockPrint(sock, "MU Memory Test (this can take up to 30 seconds):\r\n");
            if(MUDiagnosticIoctl(sock, port, slot, MU_IOCTL_MEMORY_TEST, &count, size))
                {
                SockPrint(sock, "   Bad Blocks: %04x\r\n", count);
                }
        #else
            SockPrint(sock, "MU Diagnostics not enabled in this build - C_DEFINES MU_DIAGNOSTIC_IOCTLS\r\n");
        #endif
        }
    else
        {
        HELP(sock, "MU");
        return ERR_SYNTAX;
        }
    return ERR_OK;
    }


DWORD RAWREAD(SOCKET sock, char *nextToken)
    {
    char *device;
    char *file;
    char *temp;
    DWORD size;

    if(!*nextToken)
        {
        HELP(sock, "RAWREAD");
        return ERR_SYNTAX;
        }

    device = nextToken;

    file = strchr(device, ' ');
    if(!file)
        {
        HELP(sock, "RAWREAD");
        return ERR_SYNTAX;
        }
    *file = L'\0';
    ++file;

    temp = strchr(file, ' ');
    if(!temp)
        {
        HELP(sock, "RAWREAD");
        return ERR_SYNTAX;
        }
    *temp = L'\0';
    ++temp;
    size = atol(temp);

    DWORD error = RawRead(device, file, 0, size);

    if(error == ERROR_SUCCESS)
        SockPrint(sock, responseOk);
    else
        SockPrint(sock, "   Error: %lu\r\n", error);

    return ERR_OK;
    }


DWORD RAWWRITE(SOCKET sock, char *nextToken)
    {
    char *device;
    char *file;
    char *temp;
    DWORD size;

    if(!*nextToken)
        {
        HELP(sock, "RAWREAD");
        return ERR_SYNTAX;
        }

    device = nextToken;

    file = strchr(device, ' ');
    if(!file)
        {
        HELP(sock, "RAWREAD");
        return ERR_SYNTAX;
        }
    *file = '\0';
    ++file;

    temp = strchr(file, ' ');
    if(!temp)
        {
        HELP(sock, "RAWREAD");
        return ERR_SYNTAX;
        }
    *temp = '\0';
    ++temp;
    size = atol(temp);

    DWORD error = RawWrite(device, file, 0, size);

    if(error == ERROR_SUCCESS)
        SockPrint(sock, responseOk);
    else
        SockPrint(sock, "   Error: %lu\r\n", error);

    return ERR_OK;
    }


DWORD REBOOT(SOCKET sock, char *nextToken)
    {
    SockPrint(sock, "Rebooting...\r\n");

    shutdown(sock, SD_BOTH);
    closesocket(sock);

    FlushDiskCache();

    if(!nextToken || !*nextToken)
        {
        XLaunchNewImage("D:\\default.xbe", NULL);
        }
    else
        {
        XLaunchNewImage(nextToken, NULL);
        }

    //HalReturnToFirmware(HalRebootRoutine);

    return ERR_EXIT;
    }


DWORD RUN(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "RUN");
        return ERR_SYNTAX;
        }

    // close off all the devices, so a test can use them
    delete usbmanager;

    // execute the library
    if(RunTestA(sock, nextToken) == FALSE)
        {
        SockPrint(sock, "Error executing the module, use the MODS command to verify the name.\r\n");
        }

    // re-open the attached devices
    usbmanager = new USBManager;

    return ERR_OK;
    }


DWORD SAVER(SOCKET sock, char *nextToken)
    {
    bool exit = false;

    SockPrint(sock, "Running screen saver - press any key to stop.\r\n");
    drInit();

    DWORD angle = rand() % 360;
    DWORD speed = 4;
    DWORD border = 3*speed;

    const DWORD width = 160;
    const DWORD height = 160;

    // center the image on start
    DWORD offsetX = 640/2 - width/2;
    DWORD offsetY = 480/2 - height/2;

    Bitmap logo("t:\\media\\xboxlogo.bmp");

    while(!exit)
        {
        drCls();

        if(offsetX < border)
            {
            if(offsetY < border) angle = rand() % 50 + 290;
            else if(offsetY > 480-height-border) angle = rand() % 50 + 20;
            else angle = rand() % 140 + 290;
            }
        else if(offsetX > 640-width-border)
            {
            if(offsetY < border) angle = rand() % 50 + 190;
            else if(offsetY > 480-height-border) angle = rand() % 50 + 110;
            else angle = rand() % 140 + 110;
            }
        else if(offsetY < border)
            {
            angle = rand() % 140 + 200;
            }
        else if(offsetY > 480-height-border)
            {
            angle = rand() % 140 + 20;
            }
        if(offsetX < border) offsetX = border;
        if(offsetX > 640-width-border) offsetX = 640-width-border;
        if(offsetY < border) offsetY = border;
        if(offsetY > 480-height-border) offsetY = 480-height-border;

        angle %= 360;

        // move it
        offsetX += (DWORD)(speed * sin(angle));
        offsetY += (DWORD)(speed * cos(angle));

        // draw the image
        logo.Draw(offsetX, offsetY);
        drShowScreen();

        // check for user interruption - also used to sleep
        if(NetHit(sock, 2500))
            {
            exit = true;
            }
        }

    drDeinit();

    return ERR_OK;
    }


DWORD STRESS(SOCKET sock, char *nextToken)
    {
    Shutdown(sock, atol(nextToken));
    return ERR_OK;
    }


DWORD STATS(SOCKET sock, char *nextToken)
    {
    const unsigned STATS_VERSION    = 0x01;
    const unsigned STATS_MEMORY     = 0x02;
    const unsigned STATS_PROCESS    = 0x04;
    const unsigned STATS_DISK       = 0x08;
    const unsigned STATS_DEVICE     = 0x10;
    const unsigned STATS_NETWORK    = 0x20;

    const DWORD pageSize = 4096;
    DWORD display = (DWORD)(~0);

    // make sure memory usage is not skewed by the cache
    FlushDiskCache();

    // specific pages of info
    if(*nextToken)
        {
        display = 0;
        if(strstr(nextToken, "VER") != NULL)   display |= STATS_VERSION;
        if(strstr(nextToken, "MEM") != NULL)   display |= STATS_MEMORY;
        if(strstr(nextToken, "PS") != NULL)    display |= STATS_PROCESS;
        if(strstr(nextToken, "DISK") != NULL)  display |= STATS_DISK;
        if(strstr(nextToken, "USB") != NULL)   display |= STATS_DEVICE;
        if(strstr(nextToken, "DEV") != NULL)   display |= STATS_DEVICE;
        if(strstr(nextToken, "DEVICE") != NULL)   display |= STATS_DEVICE;
        if(strstr(nextToken, "NET") != NULL)   display |= STATS_NETWORK;
        if(strstr(nextToken, "NETWORK") != NULL)   display |= STATS_NETWORK;
        if(strstr(nextToken, "ALL") != NULL)   display = (DWORD)(~0);
        }

    //
    // version
    //
    if(display & STATS_VERSION)
        {
        SockPrint(sock, "System Version---------------------------------------------------------------\r\n");

        if (0 != (XboxKrnlVersion->Qfe | 0x8000))
            SockPrint(sock, "   Kernel:      %12lu DEBUG\r\n", XboxKrnlVersion->Build);
        else
            SockPrint(sock, "   Kernel:      %12lu FREE\r\n", XboxKrnlVersion->Build);

        //
        // XapiBuildNumber[4] is major, [5] is minor, [6] is build, [7] is qfe
        //
        // The high bit of XapiBuildNumber[7] is set if it is a debug build
        //

        if (XeImageHeader()->XapiLibraryVersion->DebugBuild)
            SockPrint(sock, "   Xapi:        %12lu DEBUG\r\n", XeImageHeader()->XapiLibraryVersion->BuildVersion);
        else
            SockPrint(sock, "   Xapi:        %12lu FREE\r\n", XeImageHeader()->XapiLibraryVersion->BuildVersion);
        }

    //
    // memory
    //
    if(display & STATS_MEMORY)
        {
        MM_STATISTICS mmStats;
        memset(&mmStats, 0, sizeof(MM_STATISTICS));
        mmStats.Length = sizeof(MM_STATISTICS);
        MmQueryStatistics(&mmStats);

        SockPrint(sock, "Memory Status----------------------------------------------------------------\r\n");
        SockPrint(sock, "   Total:       %12lu B\r\n", mmStats.TotalPhysicalPages*pageSize);
        SockPrint(sock, "   Available:   %12lu B\r\n", mmStats.AvailablePages*pageSize);
        SockPrint(sock, "   Cache:       %12lu B\r\n", mmStats.CachePagesCommitted*pageSize);
        SockPrint(sock, "   Pool:        %12lu B\r\n", mmStats.PoolPagesCommitted*pageSize);
        SockPrint(sock, "   Stack:       %12lu B\r\n", mmStats.StackPagesCommitted*pageSize);
        SockPrint(sock, "   Image:       %12lu B\r\n", mmStats.ImagePagesCommitted*pageSize);
        }

    //
    // process
    //
    if(display & STATS_PROCESS)
        {
        PS_STATISTICS psStats;
        memset(&psStats, 0, sizeof(PS_STATISTICS));
        psStats.Length = sizeof(PS_STATISTICS);
        PsQueryStatistics(&psStats);

        SockPrint(sock, "Process Status---------------------------------------------------------------\r\n");
        SockPrint(sock, "   Handles:     %12lu\r\n", psStats.HandleCount);
        SockPrint(sock, "   Threads:     %12lu\r\n", psStats.ThreadCount);
        }

    //
    // network
    //
    if(display & STATS_NETWORK)
        {
        XNADDR  XnAddr;
        XNetGetTitleXnAddr(&XnAddr);

        SockPrint(sock, "Network Status---------------------------------------------------------------\r\n");
        SockPrint(sock, "   IP:          %u.%u.%u.%u\r\n", XnAddr.ina.S_un.S_un_b.s_b1, XnAddr.ina.S_un.S_un_b.s_b2, XnAddr.ina.S_un.S_un_b.s_b3, XnAddr.ina.S_un.S_un_b.s_b4);
        }

    //
    // disk drives
    //
    if(display & STATS_DISK)
        {
        unsigned numDrives;
        DWORD serialNum;
        DWORD maxFilename;
        DWORD flags;
        DWORD vnameSize = 128;
        DWORD fsnameSize = 128;
        ULARGE_INTEGER available;
        ULARGE_INTEGER total;
        ULARGE_INTEGER free;
        char drives[] = "A:\\\0B:\\\0C:\\\0D:\\\0E:\\\0F:\\\0G:\\\0H:\\\0I:\\\0J:\\\0K:\\\0L:\\\0M:\\\0"
                        "N:\\\0O:\\\0P:\\\0Q:\\\0R:\\\0S:\\\0T:\\\0U:\\\0V:\\\0W:\\\0X:\\\0Y:\\\0Z:\\";
        char *vnameBuffer = new char[128];
        char *fsnameBuffer = new char[128];

        numDrives = ARRAYSIZE(drives)/4;

        SockPrint(sock, "Disk Status------------------------------------------------------------------\r\n");
        for(unsigned i=0; i<numDrives; i++)
            {
            vnameBuffer[0] = fsnameBuffer[0] = '\0';
            serialNum = 0;
            total.QuadPart = available.QuadPart = free.QuadPart = 0;
            if (GetVolumeInformation(&drives[i*4], vnameBuffer, vnameSize, &serialNum, &maxFilename, &flags, fsnameBuffer, fsnameSize))
                {
                available.QuadPart = 0;
                total.QuadPart = 0;
                GetDiskFreeSpaceEx(&drives[i*4], &available, &total, &free);

                SockPrint(sock, "   %s, %04X-%04X, %s\r\n", &drives[i*4], HIWORD(serialNum), LOWORD(serialNum), fsnameBuffer);
                SockPrint(sock, "      Total:   %12I64u MB\r\n", total.QuadPart/MEGABYTE);
                SockPrint(sock, "      Free:    %12I64u MB\r\n", available.QuadPart/MEGABYTE);
                }
            }

        delete[] vnameBuffer;
        delete[] fsnameBuffer;
        }

    //
    // usb devices
    //
    if(display & STATS_DEVICE)
        {
        XINPUT_CAPABILITIES info;

        const char *xidSubTypes[] = {
            "",
            "GAMEPAD",
            "JOYSTICK",
            "WHEEL",
            "DANCE PAD",
            "FISHING ROD",
            "LIGHT GUN"
            };

        SockPrint(sock, "Device Status----------------------------------------------------------------\r\n");
        for(unsigned i=0; i<XGetPortCount(); i++)
            {
            for(unsigned j=0; j<SLOT_MAX; j++)
                {
                if(usbmanager->devices[i][j] != NULL)
                    {
                    SockPrint(sock, "   Port %u Slot %u is Active\r\n", i, j);
                    if(XDEVICE_TYPE_GAMEPAD == usbmanager->devices[i][j]->type)
                        {
                        XInputGetCapabilities(((DeviceDuke*)(usbmanager->devices[i][j]))->duke, &info);
                        SockPrint(sock, "      Type:    %12s\r\n", "XID");
                        SockPrint(sock, "      SubType: %12s\r\n", xidSubTypes[info.SubType]);
                        }
                    else if(XDEVICE_TYPE_MEMORY_UNIT == usbmanager->devices[i][j]->type)
                        {
                        SockPrint(sock, "      Type:   %13s\r\n", "XMU");
                        SockPrint(sock, "      Drive:  %13c\r\n", (char)((DeviceMU*)(usbmanager->devices[i][j]))->drive);
                        }
                    else if(XDEVICE_TYPE_VOICE_MICROPHONE == usbmanager->devices[i][j]->type)
                        {
                        SockPrint(sock, "      Type:   %13s\r\n", "XHAWK");
                        }
#ifdef DEBUG_KEYBOARD
                    else if(XDEVICE_TYPE_DEBUG_KEYBOARD == usbmanager->devices[i][j]->type)
                        {
                        SockPrint(sock, "      Type:   %13s\r\n", "KEYBOARD");
                        }
#endif // DEBUG_KEYBOARD
                    }
                }
            }
        }

    return ERR_OK;
    }


extern void drBitmap(float x, float y, DWORD dwWidth, DWORD dwHeight, DWORD *bitmap);

DWORD TEST(SOCKET sock, char *nextToken)
    {
    SockPrint(sock, "Test Command:\r\n");

    unsigned state = 0;

    if(*nextToken) state = atol(nextToken);

    switch(state)
        {
        case 0:
            {
            break;
            }
        case 1:
            {
            const unsigned width = 8;
            const unsigned height = 4;

            DWORD bitmap[width*height] = { 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000,
                0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000,
                0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFF00, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFF00, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

            drInit();
            drCls();
            drBitmap(50, 50, width, height, bitmap);
            drShowScreen();
            drDeinit();
            break;
            }
        case 2:
            {
            SockPrint(sock, "RawRead... ");
            RawRead("\\Device\\MemoryUnit01_01", "z:\\mu.dmp", 0, 8*MEGABYTE);
            SockPrint(sock, "Format... ");
            FORMAT(sock, "\\Device\\MemoryUnit01_01");
            SockPrint(sock, "RawWrite... ");
            RawWrite("z:\\mu.dmp", "\\Device\\MemoryUnit01_01", 0, 8*MEGABYTE);
            SockPrint(sock, "Done\r\n");
            }
        case 3:
            {
            }
        default:
            break;
        }

    return ERR_OK;
    }




DWORD USB(SOCKET sock, char *nextToken)
    {
    const unsigned defSec = 30; // default number of seconds in state spew
    const unsigned sleep = 8; // pause in ms between state queries
    unsigned numseconds = 0;

    if(*nextToken) numseconds = atol(nextToken);
    if(numseconds == 0) numseconds = defSec;

    unsigned time = numseconds * (1000/sleep);

    SockPrint(sock, "Displaying USB XID output for %u seconds...\r\n\r\n", numseconds);

    for(unsigned i=0; i<time; i++)
        {
        SleepEx(8, false);
        usbmanager->ProcessInput(sock);
        }

    return ERR_OK;
    }


DWORD WRITEINI(SOCKET sock, char *nextToken)
    {
    WCHAR *section;
    WCHAR *key;
    WCHAR *value;
    WCHAR *part2;

    nextToken = (char*)Ansi2UnicodeHack(nextToken);

    part2 = wcschr((WCHAR*)nextToken, ' ');
    value = wcschr((WCHAR*)nextToken, '=');

    if(!part2 || !value)
        {
        HELP(sock, "WRITEINI");
        return ERR_SYNTAX;
        }

    *part2 = '\0';
    part2 += 1;
    *value = '\0';
    value += 1;

    if(value > part2)
        {
        section = (WCHAR*)nextToken;
        key = part2;
        }
    else
        {
        section = part2;
        key = (WCHAR*)nextToken;
        }

    if(WriteProfileString(section, key, value) == FALSE)
        SockPrint(sock, "Error writing [%ws] %ws=%ws (ec: %lu)\r\n", section, key, value, GetLastError());
    else
        SockPrint(sock, responseOk);

    return ERR_OK;
    }


DWORD XMOUNT(SOCKET sock, char *nextToken)
    {
    if(!*nextToken)
        {
        HELP(sock, "XMOUNT");
        return ERR_SYNTAX;
        }

    char drive = nextToken[0];
    unsigned port = XMUPortFromDriveLetter(drive);
    unsigned slot = XMUSlotFromDriveLetter(drive);

    slot = slot==XDEVICE_TOP_SLOT? 1 : 2;

    if(usbmanager->devices[port][slot] != NULL && usbmanager->devices[port][slot]->type != XDEVICE_TYPE_MEMORY_UNIT)
        {
        SockPrint(sock, "The port (%02X, %02X) must be empty or hold an MU!\r\n", port, slot);
        return ERR_OK;
        }

    if(usbmanager->devices[port][slot] != NULL && usbmanager->devices[port][slot]->type == XDEVICE_TYPE_MEMORY_UNIT)
        {
        SockPrint(sock, "Unmounting %wc... ", drive);
        delete (DeviceMU*)(usbmanager->devices[port][slot]);
        usbmanager->devices[port][slot] = NULL;
        }

    SockPrint(sock, "Attempting to mount %wc... ", drive);
    usbmanager->devices[port][slot] = new DeviceMU(port, slot);

    if(((DeviceMU*)(usbmanager->devices[port][slot]))->drive != drive)
        {
        delete (DeviceMU*)(usbmanager->devices[port][slot]);
        usbmanager->devices[port][slot] = NULL;
        SockPrint(sock, "Mount failed.\r\n");
        }
    else
        {
        SockPrint(sock, responseOk);
        }

    return ERR_OK;
    }

} // namespace Launcher
