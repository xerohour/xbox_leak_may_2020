/*

Copyright (c) 2000  Microsoft Corporation

Module Name:

    utils.cpp

Abstract:


Author:

    jpoley

Environment:

    XBox

Revision History:

*/


#include "stdafx.h"
#include <xbox.h>
#include <ntdddisk.h>

#include "launcherx.h"


namespace Launcher {

/*****************************************************************************

Routine Description:

    Ansi2UnicodeHack

    In-place Pseudo Ansi to Unicode (char to wide char) conversion.

Arguments:

    IN char* str - char string to convert to wide char string

Return Value:

    WCHAR* - pointer to Unicode string

Note:
    
    Because a bunch of Unicode functions expect a WCHAR string to be on an
    even boundry, the returned string may be moved 1 character over.

*****************************************************************************/
WCHAR* Ansi2UnicodeHack(char *str)
    {
    if(!str) return NULL;

    int align = 0;
    int len = strlen(str)+1;

    // put string on an even boundry because some freak put a bunch of ASSERTs
    // that check for even boundries in Unicode functions like 
    // RtlEqualUnicodeString()
    if(((ULONG_PTR)str & 1) != 0)
        {
        align = 1;
        }

    for(; len>=0; len--)
        {
        str[len*2+align] = str[len];
        str[len*2+align+1] = '\0';
        }

    str += align;

    return (WCHAR*)str;
    }


/*****************************************************************************

Routine Description:

    Unicode2AnsiHack

    In-place Pseudo Unicode to Ansi (wide char to char) conversion.

Arguments:

    IN unsigned short* str - wide char string to convert to char string

Return Value:

    char* - pointer to ANSI string

*****************************************************************************/
char* Unicode2AnsiHack(unsigned short *str)
    {
    if(!str) return NULL;
    char *str2 = (char*)str;

    size_t len = wcslen(str)+1;
    for(size_t i=0; i<len; i++)
        {
        str2[i] = str2[i*2];
        }

    return (char*)str;
    }


/*****************************************************************************

Routine Description:

    HexDump

    Writes the contents of a chunk of memory to the socket as a hex dump.

Arguments:

    SOCKET sock     - [in] socket to send the data through
    uchar* buffer   - [in] the memory address
    DWORD length    - [in] the number of bytes to write
    DWORD offset    - [in optional] starting offset for the displayed address

Return Value:

    void

*****************************************************************************/
void HexDump(SOCKET sock, const unsigned char * buffer, DWORD length, DWORD offset/*=0*/)
    {
    DWORD i, count, index;
    CHAR digits[]="0123456789ABCDEF";
    CHAR line[100];
    char cbLine;

    for(index = 0; length; length -= count, buffer += count, index += count)
        {
        count = (length > 16) ? 16:length;

        sprintf(line, "%04X  ", index+offset);
        cbLine = 6;

        for(i=0; i<count; i++)
            {
            line[cbLine++] = digits[buffer[i] >> 4];
            line[cbLine++] = digits[buffer[i] & 0x0f];
            if(i == 7)
                {
                line[cbLine++] = ' ';
                }
            else
                {
                line[cbLine++] = ' ';
                }
            }
        for(; i<16; i++)
            {
            line[cbLine++] = ' ';
            line[cbLine++] = ' ';
            line[cbLine++] = ' ';
            }

        line[cbLine++] = ' ';

        for(i=0; i<count; i++)
            {
            if(buffer[i] < 32 || buffer[i] > 126)
                {
                line[cbLine++] = '.';
                }
            else
                {
                line[cbLine++] = buffer[i];
                }
            }

        line[cbLine++] = '\r';
        line[cbLine++] = '\n';
        line[cbLine++] = 0;
        if(sock == INVALID_SOCKET) DebugPrint(line);
        else send(sock, line, strlen(line), 0);        
        }
    }


/*****************************************************************************

Routine Description:

    This routine will flush the disk cache

Arguments:

    None

Return Value:

    No return value

*****************************************************************************/
void FlushDiskCache(void)
    {
    ULONG Volume, Disk;
    HANDLE Handle;
    NTSTATUS Status;
    char VolumeBuffer[MAX_PATH];
    OBJECT_STRING VolumeString;
    OBJECT_ATTRIBUTES ObjA;
    IO_STATUS_BLOCK IoStatusBlock;

    for(Disk=0; Disk<2; Disk++)
        {
        for(Volume=1; Volume<4; Volume++)
            {
            sprintf(VolumeBuffer, "\\Device\\Harddisk%d\\Partition%d", Disk, Volume);

            RtlInitObjectString(&VolumeString, VolumeBuffer);

            InitializeObjectAttributes(&ObjA,&VolumeString,OBJ_CASE_INSENSITIVE,NULL,NULL);

            Status = NtCreateFile(&Handle, SYNCHRONIZE|GENERIC_READ, &ObjA, &IoStatusBlock, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT);

            if(!NT_SUCCESS(Status)) continue;

            Status = NtFlushBuffersFile(Handle, &IoStatusBlock);
            NtClose(Handle);
            }
        }
    }


/*****************************************************************************

Routine Description:

    xPathFindFileName
    Returns a pointer to the start of the filename in a path string.

Arguments:

    WCHAR *name - [in] string to search for the filename

Return Value:

    WCHAR*

*****************************************************************************/
char* xPathFindFileName(char *name)
    {
    DWORD attribs;
    char *sep;

    size_t len = strlen(name);

    attribs = GetFileAttributes(name);
    if(attribs != -1 && attribs&FILE_ATTRIBUTE_DIRECTORY)
        {
        return name + len;
        }

    sep = strrchr(name, '\\');

    if(!sep)
        {
        return NULL;
        }

    return sep + 1;
    }


/*****************************************************************************

Routine Description:

    NetHit
    Looks for incoming data on the socket.

Arguments:

    SOCKET sock     [in] connected socket
    unsigned usec   [in] microsec time to wait for input (0 is default)

Return Value:

    true - socket closed or input
    false - no input

*****************************************************************************/
bool NetHit(SOCKET sock, unsigned usec/*=0*/)
    {
    // check for user interaction
    TIMEVAL timeout = { 0, usec };
    FD_SET bucket;
    bucket.fd_count = 1;
    bucket.fd_array[0] = sock;
    int err = select(0, &bucket, NULL, NULL, &timeout);
    if(err == SOCKET_ERROR || err != 0)
        {
        char eat[10];
        recv(sock, eat, 10, 0);
        return true;
        }

    return false;
    }



/*****************************************************************************

Routine Description:

    OpenMU
    Creates a device object name usable in calls to NtCreateFile etc.

Arguments:

    unsigned port   [in] 0 based index of the usb port number
    unsigned slot   [in] XDEVICE_TOP_SLOT or XDEVICE_BOTTOM_SLOT

Return Value:

    true - happy
    false - failure

*****************************************************************************/
bool OpenMU(unsigned port, unsigned slot, char *device)
    {
    OBJECT_STRING deviceName;
    deviceName.Buffer = device;
    deviceName.Length = 0;
    deviceName.MaximumLength = 63;
    if(! NT_SUCCESS(MU_CreateDeviceObject(port, slot, &deviceName)))
        return false;
    return true;
    }


/*****************************************************************************

Routine Description:

    CloseMU
    Destroys a device object

Arguments:

    unsigned port   [in] 0 based index of the usb port number
    unsigned slot   [in] XDEVICE_TOP_SLOT or XDEVICE_BOTTOM_SLOT

Return Value:

    true

*****************************************************************************/
bool CloseMU(unsigned port, unsigned slot)
    {
    MU_CloseDeviceObject(port, slot);
    return true;
    }

bool ReadMUCapacity(HANDLE handle)
    {
    NTSTATUS status;
    IO_STATUS_BLOCK statusBlock;
    DISK_GEOMETRY diskGeometry;
    PARTITION_INFORMATION pi;

    status = NtDeviceIoControlFile(handle,
                                   0,
                                   NULL,
                                   NULL,
                                   &statusBlock,
                                   IOCTL_DISK_GET_DRIVE_GEOMETRY,
                                   NULL,
                                   0,
                                   &diskGeometry,
                                   sizeof(diskGeometry));
    DebugPrint("IOCTL_DISK_GET_DRIVE_GEOMETRY: %x\n", status);
/*
    status = NtDeviceIoControlFile(handle,
                                   0,
                                   NULL,
                                   NULL,
                                   &statusBlock,
                                   IOCTL_DISK_GET_PARTITION_INFO,
                                   NULL,
                                   0,
                                   &pi,
                                   sizeof(pi));
    DebugPrint("IOCTL_DISK_GET_PARTITION_INFO: %x\n", status);
*/
    /*
    ULONG ulSectorSize = diskGeometry.BytesPerSector;
    ULONG ulSectorsPerTrack = diskGeometry.SectorsPerTrack;
    ULONG ulHeads = diskGeometry.TracksPerCylinder;
    ULONG ulTotalSectorCount = (RtlExtendedIntegerMultiply(
                                diskGeometry.Cylinders,
                                diskGeometry.SectorsPerTrack *
                                    diskGeometry.TracksPerCylinder)).LowPart;
                                    */

    return true;
    }

/*****************************************************************************

Routine Description:

    MUDiagnosticIoctl
    Calls in the the MU's Diagnostic Ioctl.

Arguments:

    SOCKET sock     [in] socket for output
    unsigned port   [in] 0 based index of the usb port number
    unsigned slot   [in] XDEVICE_TOP_SLOT or XDEVICE_BOTTOM_SLOT
    DWORD ioctl     [in] MU_DIAGNOSTIC_IOCTLS identifier
    void *output    [in out] buffer to be filled with the return data from the IOCTL
    DWORD &size     [in out] size of the buffer

Return Value:

    true - happy
    false - failure

Notes:
    
    This library AND the MU driver must be compiled with 
    C_DEFINES=-DMU_DIAGNOSTIC_IOCTLS

    The MU must NOT be mounted prior to calling this method

*****************************************************************************/
bool MUDiagnosticIoctl(SOCKET sock, unsigned port, unsigned slot, DWORD ioctl, void *output, DWORD &size)
    {
    HANDLE handle;
    NTSTATUS status;
    char deviceName[128];
    OBJECT_STRING deviceString;
    OBJECT_ATTRIBUTES objAttrib;
    IO_STATUS_BLOCK ioStatusBlock;

    if(!OpenMU(port, slot, deviceName))
        {
        SockPrint(sock, "   MU_CreateDeviceObject failed\r\n");
        return false;
        }
    RtlInitObjectString(&deviceString, deviceName);
    InitializeObjectAttributes(&objAttrib, &deviceString, OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = NtOpenFile(&handle, SYNCHRONIZE|GENERIC_READ, &objAttrib, &ioStatusBlock, FILE_SHARE_READ, FILE_NO_INTERMEDIATE_BUFFERING|FILE_SYNCHRONOUS_IO_NONALERT);
    if(!NT_SUCCESS(status)) 
        {
        SockPrint(sock, "   CreateFile Error %s (ec: %ld)\r\n", deviceName, RtlNtStatusToDosError(status));
        CloseMU(port, slot);
        return false;
        }

    if(DeviceIoControl(handle, ioctl, NULL, 0, output, size, &size, NULL) == FALSE)
        {
        SockPrint(sock, "   DeviceIoControl Error (ec: %ld)\r\n", GetLastError());
        CloseMU(port, slot);
        NtClose(handle);
        return false;
        }

    CloseMU(port, slot);
    NtClose(handle);
    return true;
    }


//#define MU_MEDIA_OFFSET     4096*8
//#define MU_MEDIA_OFFSET     0


/*****************************************************************************

Routine Description:

    RawRead
    Reads from a device and writes the data to a file.

Arguments:

    WCHAR *device   [in] device name (eg "\\Device\\MemoryUnit01_01")
    WCHAR *file     [in] destination filename (eg "z:\\mu.dmp")
    DWORD offset    [in] starting offset to read from on the device
    DWORD size      [in] number of bytes to read

Return Value:

    DWORD - win32 error code

*****************************************************************************/
DWORD RawRead(char *device, char *file, DWORD offset, DWORD size)
    {
    const DWORD EXTRA_SIZE = 4096;
    const unsigned __int64 SECTOR_MASK = 0xFFFFFF000;
    DWORD error = ERROR_SUCCESS;
    OBJECT_STRING uniName;
    NTSTATUS status;
    char muName[128];
    PUCHAR mem = (PUCHAR)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, size + 2*EXTRA_SIZE);
    if(!mem) return ERROR_NOT_ENOUGH_MEMORY;
    
    bool mu = false;
    DWORD port, slot;

    if(sscanf(device, "\\Device\\MemoryUnit%02x_%02x", &port, &slot) == 2)
        {
        if(usbmanager->devices[port][slot] != NULL && usbmanager->devices[port][slot]->type == XDEVICE_TYPE_MEMORY_UNIT)
            {
            delete (DeviceMU*)(usbmanager->devices[port][slot]);
            usbmanager->devices[port][slot] = NULL;
            }
        mu = true;
        }

    // sector align buffers
    PUCHAR buffer = (PUCHAR)((ULONG) mem & SECTOR_MASK);
    buffer += EXTRA_SIZE;

    // initialize src name
    RtlInitObjectString(&uniName, device);
    if(mu) OpenMU(port-1, slot==1?XDEVICE_TOP_SLOT:XDEVICE_BOTTOM_SLOT, muName);

    do {

    // open the source
    HANDLE hSrc;
    OBJECT_ATTRIBUTES oa;
    IO_STATUS_BLOCK statusBlock;
    InitializeObjectAttributes(&oa, &uniName, OBJ_CASE_INSENSITIVE, NULL, NULL);
    status = NtOpenFile(&hSrc, SYNCHRONIZE | GENERIC_ALL, &oa, &statusBlock, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_NO_INTERMEDIATE_BUFFERING | FILE_SYNCHRONOUS_IO_ALERT);
    if(NT_SUCCESS(status))
        {
        if(mu) ReadMUCapacity(hSrc);
        // read data
        LARGE_INTEGER startLoc;
        startLoc.QuadPart = offset;
        //__asm int 3;
        status = NtReadFile(hSrc, 0, NULL, NULL, &statusBlock, buffer, size, &startLoc);
        NtClose(hSrc);

        if(!NT_SUCCESS(status))
            {
            //SockPrint(sock, "RawRead: Read from Source failed (ec: 0x%X)\r\n", status);
            error = RtlNtStatusToDosError(status);
            break;
            }
        }
    else
        {
        //SockPrint(sock, "RawRead: Open Source failed (ec: 0x%X)\r\n", status);
        error = RtlNtStatusToDosError(status);
        break;
        }

    // open the destination
    HANDLE hDest = CreateFile(file, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hDest != INVALID_HANDLE_VALUE)
        {
        // write the data
        DWORD wSize=0;
        if(!(WriteFile(hDest, buffer, size, &wSize, NULL) == TRUE && wSize == size))
            {
            CloseHandle(hDest);
            //SockPrint(sock, "RawRead: Write failed[%lu]\r\n", wSize);
            error = GetLastError();
            break;
            }
        CloseHandle(hDest);
        }
    else
        {
        //SockPrint(sock, "RawRead: Open Destination failed (ec: %lu)\r\n", GetLastError());
        error = GetLastError();
        break;
        }

    } while(0);

    if(mu) CloseMU(port-1, slot==1?XDEVICE_TOP_SLOT:XDEVICE_BOTTOM_SLOT);
    LocalFree(mem);
    return error;
    }


/*****************************************************************************

Routine Description:

    RawWrite
    Writes to a device from the data in a file.

Arguments:

    WCHAR *device   [in] source filename (eg "z:\\mu.dmp")
    WCHAR *file     [in] device name (eg "\\Device\\MemoryUnit01_01")
    DWORD offset    [in] starting offset to write on the device
    DWORD size      [in] number of bytes to write

Return Value:

    DWORD - win32 error code

*****************************************************************************/
DWORD RawWrite(char *device, char *file, DWORD offset, DWORD size)
    {
    const DWORD EXTRA_SIZE = 4096;
    const unsigned __int64 SECTOR_MASK = 0xFFFFFF000;
    DWORD error = ERROR_SUCCESS;
    OBJECT_STRING uniName;
    char muName[128];
    NTSTATUS status;
    PUCHAR mem = (PUCHAR)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, size + 2*EXTRA_SIZE);
    if(!mem) return ERROR_NOT_ENOUGH_MEMORY;
    
    bool mu = false;
    DWORD port, slot;

    if(sscanf(device, "\\Device\\MemoryUnit%02x_%02x", &port, &slot) == 2)
        {
        mu = true;
        if(usbmanager->devices[port][slot] != NULL && usbmanager->devices[port][slot]->type == XDEVICE_TYPE_MEMORY_UNIT)
            {
            delete (DeviceMU*)(usbmanager->devices[port][slot]);
            usbmanager->devices[port][slot] = NULL;
            }
        }

    // sector align buffers
    PUCHAR buffer = (PUCHAR)((ULONG) mem & SECTOR_MASK);
    buffer += EXTRA_SIZE;

    // initialize src name
    RtlInitObjectString(&uniName, device);
    if(mu) OpenMU(port-1, slot==1?XDEVICE_TOP_SLOT:XDEVICE_BOTTOM_SLOT, muName);

    do {

    // open the source
    HANDLE hSrc = CreateFile(file, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hSrc != INVALID_HANDLE_VALUE)
        {
        // read the data
        DWORD rSize=0;
        if(!(ReadFile(hSrc, buffer, size, &rSize, NULL) == TRUE && rSize == size))
            {
            CloseHandle(hSrc);
            //SockPrint(sock, "RawWrite: Read failed[%lu]\r\n", rSize);
            error = GetLastError();
            break;
            }
        CloseHandle(hSrc);
        }
    else
        {
        //SockPrint(sock, "RawRead: Open Source failed (ec: %lu)\r\n", GetLastError());
        error = GetLastError();
        break;
        }

    // open the destination
    HANDLE hDest;
    OBJECT_ATTRIBUTES oa;
    IO_STATUS_BLOCK statusBlock;
    InitializeObjectAttributes(&oa, &uniName, OBJ_CASE_INSENSITIVE, NULL, NULL);
    status = NtOpenFile(&hDest, SYNCHRONIZE | GENERIC_ALL, &oa, &statusBlock, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_NO_INTERMEDIATE_BUFFERING | FILE_SYNCHRONOUS_IO_ALERT);
    if(NT_SUCCESS(status))
        {
        if(mu) ReadMUCapacity(hDest);
        // write data
        LARGE_INTEGER startLoc;
        startLoc.QuadPart = offset;
        status = NtWriteFile(hDest, 0, NULL, NULL, &statusBlock, buffer, size, &startLoc);
        NtClose(hDest);

        if(!NT_SUCCESS(status))
            {
            //SockPrint(sock, "RawWrite: Write to Destination failed (ec: 0x%X)\r\n", status);
            error = RtlNtStatusToDosError(status);
            break;
            }
        }
    else
        {
        //SockPrint(sock, "RawWrite: Open Destination failed (ec: 0x%X)\r\n", status);
        error = RtlNtStatusToDosError(status);
        break;
        }

    } while(0);

    if(mu) CloseMU(port-1, slot==1?XDEVICE_TOP_SLOT:XDEVICE_BOTTOM_SLOT);
    LocalFree(mem);
    return error;
    }

} // namespace Launcher
