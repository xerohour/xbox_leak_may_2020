/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    idelog.c

Abstract:

    This is a port form the idelog utility from the NT group

ported by:

    John Daly (johndaly) 25-April-2000

Environment:

    XBox

Revision History:

--*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntexapi.h>

#include <ntos.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntddcdvd.h>
#include <windows.h>

#include <xtl.h>
#include <stdio.h>
#include <xlog.h>
#include <xtestlib.h>
#include <ntdddisk.h>
#include <ntddscsi.h>

VOID
PrintData(
    PIDE_IDENTIFY_DATA IdData, 
    POCHAR DeviceName
    );

VOID
WINAPI
CheckBCArea(
    VOID
    );

//
// Heap handle from HeapCreate
//
HANDLE IdeLogHeapHandle;
HANDLE LogHandle;

VOID
WINAPI
IdeLogStartTest(
         HANDLE LogHandleX
         )
/*++

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    LogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

--*/
{
    BOOL status;
    DWORD accessMode, shareMode;
    HANDLE fileHandle;
    PATA_PASS_THROUGH atapt;
    ULONG length,errorCode,returned;
    PIDEREGS pIdeReg;
    OCHAR    string[100];
    char    stringa[100];
    ULONG   i, j;
    int DrivesFound = 0;
    IDE_IDENTIFY_DATA idedata = {0};
    DWORD x;
    NTSTATUS NTStatus;
    OBJECT_ATTRIBUTES ObjA;
    OBJECT_STRING VolumeString;
    IO_STATUS_BLOCK IoStatusBlock;

    LogHandle = LogHandleX; // make this global - there is no threading intended for this

    xSetComponent( LogHandle, "kernel", "idelog" );
    xSetFunctionName(LogHandle, "StartTest" );
    xStartVariation( LogHandle, "variation1" );

    atapt = HeapAlloc(IdeLogHeapHandle, HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, sizeof(ATA_PASS_THROUGH) + 512);

    if ( !atapt ) {
        xLog(LogHandle, XLL_BLOCK, "memory allocation failure");
        return;
    }

    for ( i=0; i<2; i++ ) {
        if ( i == 0 ) {
            soprintf (string, OTEXT("\\Device\\Harddisk0\\Partition0"));
        } else {
            soprintf (string, OTEXT("\\Device\\CdRom0"));
        }

        shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;  // default
        accessMode = GENERIC_READ | GENERIC_WRITE;       // default

        //
        //fileHandle = CreateFile(string,
        //                        accessMode,
        //                        shareMode,
        //                        NULL,
        //                        OPEN_EXISTING,
        //                        0,
        //                        NULL);
        //

        RtlInitObjectString(&VolumeString, string);
        InitializeObjectAttributes(&ObjA,&VolumeString,OBJ_CASE_INSENSITIVE,NULL,NULL);

        NTStatus = NtCreateFile(&fileHandle,
                                SYNCHRONIZE|GENERIC_READ,
                                &ObjA,
                                &IoStatusBlock,
                                0,
                                FILE_ATTRIBUTE_NORMAL,
                                FILE_SHARE_READ, FILE_OPEN,
                                FILE_SYNCHRONOUS_IO_NONALERT);

        if ( !NT_SUCCESS(NTStatus)) {
            continue;
        } else {
            ++DrivesFound;
        }

        atapt->DataBufferSize = 512;
        atapt->DataBuffer = atapt + 1;

        atapt->IdeReg.bFeaturesReg     = 0;
        atapt->IdeReg.bSectorCountReg  = 0;
        atapt->IdeReg.bSectorNumberReg = 0;
        atapt->IdeReg.bCylLowReg       = 0;
        atapt->IdeReg.bCylHighReg      = 0;
        atapt->IdeReg.bDriveHeadReg    = 0;
        atapt->IdeReg.bHostSendsData   = 0;

        if ( i == 0 ) {

            atapt->IdeReg.bCommandReg      = 0xec;

        } else {

            atapt->IdeReg.bCommandReg      = 0xa1;
        }

        status = DeviceIoControl(fileHandle,
                                 IOCTL_IDE_PASS_THROUGH,
                                 atapt,
                                 sizeof(ATA_PASS_THROUGH),
                                 atapt,
                                 sizeof(ATA_PASS_THROUGH),
                                 &returned,
                                 FALSE);
        //
        // CloseHandle(fileHandle);
        //

        NtClose(fileHandle);

        if ( status != 0 ) {
            PrintData((PIDE_IDENTIFY_DATA)atapt->DataBuffer, string);
            memset(&idedata, sizeof(IDE_IDENTIFY_DATA), 0);
        }

        //
        // Tracy's BCA test
        //
        
        if (i == 1) {
            CheckBCArea();
        }
    }

    sprintf (stringa, "DrivesFound: %d", DrivesFound);
    xLog(LogHandle, XLL_INFO, stringa);

    HeapFree (IdeLogHeapHandle, HEAP_NO_SERIALIZE, atapt);

    xEndVariation( LogHandle );

    return;
}

VOID
PrintData(
    PIDE_IDENTIFY_DATA IdData, 
    POCHAR DeviceName
    )
/*++

Routine Description:

    prints out the data colledted from an IDE device

Arguments:

    PIDENTIFY_DATA IdData : pointer to a structure containing IDE identity data
    PCHAR DeviceName : name of the device

Return Value:

    None

--*/ 
{
#define temp_buff_len   100
    UCHAR tempBuf[temp_buff_len];
    ULONG i;
    USHORT bitmap;
    ULONG mode;

    xLog(LogHandle, XLL_PASS, "%s", DeviceName);

    if ( IdData->GeneralConfiguration & (1 << 15) ) {
        xLog(LogHandle, XLL_PASS, "ATAPI device");
    } else {
        xLog(LogHandle, XLL_PASS, "ATA device");
    }

    if ( IdData->GeneralConfiguration & (1 << 7) ) {
        xLog(LogHandle, XLL_PASS, "removable media device");
    } else {
        xLog(LogHandle, XLL_PASS, "non-removable media device");
    }

    if ( IdData->MediaStatusNotification & 1 ) {

        xLog(LogHandle, XLL_PASS, "MSN supported");
    } else {

        xLog(LogHandle, XLL_PASS, "MSN not supported");
    }

    xLog(LogHandle, XLL_PASS, "ATA standard supported: ");
    bitmap = IdData->MajorRevision;
    mode = 0;
    if ( (bitmap != 0) && (bitmap != 0xffff) ) {

        bitmap >>= 1;
        while ( bitmap ) {

            if ( bitmap & 1 ) {
                xLog(LogHandle, XLL_PASS, "ATA-%d ", mode);
            }
            bitmap >>= 1;
            mode++;
        }
    } else {
        xLog(LogHandle, XLL_PASS, "none");
    }

    for ( i=0; i<sizeof(IdData->ModelNumber); i+=2 ) {
        tempBuf[i + 0] = IdData->ModelNumber[i + 1];
        tempBuf[i + 1] = IdData->ModelNumber[i + 0];
    }
    tempBuf[i] = 0;
    xLog(LogHandle, XLL_PASS, "Model Number: %s", tempBuf);

    for ( i=0; i<sizeof(IdData->SerialNumber); i+=2 ) {
        tempBuf[i + 0] = IdData->SerialNumber[i + 1];
        tempBuf[i + 1] = IdData->SerialNumber[i + 0];
    }
    tempBuf[i] = 0;
    xLog(LogHandle, XLL_PASS, "Serial Number: %s", tempBuf);

    for ( i=0; i<sizeof(IdData->FirmwareRevision); i+=2 ) {
        tempBuf[i + 0] = IdData->FirmwareRevision[i + 1];
        tempBuf[i + 1] = IdData->FirmwareRevision[i + 0];
    }
    tempBuf[i] = 0;
    xLog(LogHandle, XLL_PASS, "Firmware Revision: %s", tempBuf);

    xLog(LogHandle, XLL_PASS, "default number of logical cylinder: 0x%x (%d)", IdData->NumberOfCylinders, IdData->NumberOfCylinders);
    xLog(LogHandle, XLL_PASS, "default number of heads: 0x%x (%d)", IdData->NumberOfHeads, IdData->NumberOfHeads);
    xLog(LogHandle, XLL_PASS, "default number of sectors per track: 0x%x (%d)", IdData->NumberOfSectorsPerTrack, IdData->NumberOfSectorsPerTrack);

    if ( IdData->MaximumBlockTransfer & 0xff ) {
        xLog(LogHandle, XLL_PASS, "maximum number of sectors for READ/WRITE MULTIPLE: 0x%x (%d)", IdData->MaximumBlockTransfer, IdData->MaximumBlockTransfer & 0xf, IdData->MaximumBlockTransfer & 0xf);
    }
    if ( IdData->CurrentMultiSectorSetting & (1 << 8) ) {
        xLog(LogHandle, XLL_PASS, "current number of sectors for READ/WRITE MULTIPLE: 0x%x (%d)", IdData->MaximumBlockTransfer, IdData->MaximumBlockTransfer & 0xf, IdData->CurrentMultiSectorSetting & 0xf);
    }

    if ( IdData->Capabilities & (1 << 10) ) {
        xLog(LogHandle, XLL_PASS, "IORDY can be disabled");
    }
    if ( IdData->Capabilities & (1 << 11) ) {
        xLog(LogHandle, XLL_PASS, "IORDY is supported");
    } else {
        xLog(LogHandle, XLL_PASS, "IORDY may be supported");
    }

    xLog(LogHandle, XLL_PASS, "Highest PIO mode supported: 0x%x (%d)", IdData->PioCycleTimingMode, IdData->PioCycleTimingMode);

    if ( IdData->TranslationFieldsValid & 1 << 0 ) {

        xLog(LogHandle, XLL_PASS, "current number of logical cylinder: 0x%x (%d)", IdData->NumberOfCurrentCylinders, IdData->NumberOfCurrentCylinders);
        xLog(LogHandle, XLL_PASS, "current number of heads: 0x%x (%d)", IdData->NumberOfCurrentHeads, IdData->NumberOfCurrentHeads);
        xLog(LogHandle, XLL_PASS, "current number of sectors per track: 0x%x (%d)", IdData->CurrentSectorsPerTrack, IdData->CurrentSectorsPerTrack);
        xLog(LogHandle, XLL_PASS, "current capacity in sectors: 0x%x (%d)", IdData->CurrentSectorCapacity, IdData->CurrentSectorCapacity);
    }

    xLog(LogHandle, XLL_PASS, "total number of user addressable LBA sectors: 0x%x (%d)", IdData->UserAddressableSectors, IdData->UserAddressableSectors);

    if ( IdData->TranslationFieldsValid & (1 << 1) ) {

        xLog(LogHandle, XLL_PASS, "advanced PIO supported: ");
        bitmap = IdData->AdvancedPIOModes;
        mode = 0;
        while ( bitmap ) {

            if ( bitmap & 1 ) {
                xLog(LogHandle, XLL_PASS, "mode %d ", mode + 3);
            }
            bitmap >>= 1;
            mode++;
        }
        if ( IdData->AdvancedPIOModes ) {
            ;
        } else {
            xLog(LogHandle, XLL_PASS, "none");
        }

        xLog(LogHandle, XLL_PASS, "singleword DMA supported: ");
        bitmap = IdData->SingleWordDMASupport;
        mode = 0;
        while ( bitmap ) {

            if ( bitmap & 1 ) {
                xLog(LogHandle, XLL_PASS, "mode %d ", mode);
            }
            bitmap >>= 1;
            mode++;
        }
        if ( IdData->SingleWordDMASupport ) {
            ;
        } else {
            xLog(LogHandle, XLL_PASS, "none");
        }

        xLog(LogHandle, XLL_PASS, "singleword DMA active: ");
        bitmap = IdData->SingleWordDMAActive;
        mode = 0;
        while ( bitmap ) {

            if ( bitmap & 1 ) {
                xLog(LogHandle, XLL_PASS, "mode %d ", mode);
            }
            bitmap >>= 1;
            mode++;
        }
        if ( IdData->SingleWordDMAActive ) {
            ;
        } else {
            xLog(LogHandle, XLL_PASS, "none");
        }
    }

    xLog(LogHandle, XLL_PASS, "multiword DMA supported: ");
    bitmap = IdData->MultiWordDMASupport;
    mode = 0;
    while ( bitmap ) {

        if ( bitmap & 1 ) {
            xLog(LogHandle, XLL_PASS, "mode %d ", mode);
        }
        bitmap >>= 1;
        mode++;
    }
    if ( IdData->MultiWordDMASupport ) {
        ;
    } else {
        xLog(LogHandle, XLL_PASS, "none");
    }

    xLog(LogHandle, XLL_PASS, "multiword DMA active: ");
    bitmap = IdData->MultiWordDMAActive;
    mode = 0;
    while ( bitmap ) {

        if ( bitmap & 1 ) {
            xLog(LogHandle, XLL_PASS, "mode %d ", mode);
        }
        bitmap >>= 1;
        mode++;
    }
    if ( IdData->MultiWordDMAActive ) {
        ;
    } else {
        xLog(LogHandle, XLL_PASS, "none");
    }

    if ( IdData->TranslationFieldsValid & (1 << 1) ) {
        xLog(LogHandle, XLL_PASS, "min multiword DMA transfer cycle time (ns): 0x%x (%d)", IdData->MinimumMWXferCycleTime, IdData->MinimumMWXferCycleTime);
        xLog(LogHandle, XLL_PASS, "recommended multiword DMA transfer cycle time (ns): 0x%x (%d)", IdData->RecommendedMWXferCycleTime, IdData->RecommendedMWXferCycleTime);
        xLog(LogHandle, XLL_PASS, "min PIO transfer cycle time (ns) w/o flow control: 0x%x (%d)", IdData->MinimumPIOCycleTime, IdData->MinimumPIOCycleTime);
        xLog(LogHandle, XLL_PASS, "min PIO transfer cycle time (ns) w/ flow control: 0x%x (%d)", IdData->MinimumPIOCycleTimeIORDY, IdData->MinimumPIOCycleTimeIORDY);
    }


    if ( IdData->TranslationFieldsValid & (1 << 2) ) {

        xLog(LogHandle, XLL_PASS, "ultra DMA supported: ");
        bitmap = IdData->UltraDMASupport;
        mode = 0;
        while ( bitmap ) {

            if ( bitmap & 1 ) {
                xLog(LogHandle, XLL_PASS, "mode %d ", mode);
            }
            bitmap >>= 1;
            mode++;
        }
        if ( IdData->UltraDMASupport ) {
            ;
        } else {
            xLog(LogHandle, XLL_PASS, "none");
        }

        xLog(LogHandle, XLL_PASS, "ultra DMA active: ");
        bitmap = IdData->UltraDMAActive;
        mode = 0;
        while ( bitmap ) {

            if ( bitmap & 1 ) {
                xLog(LogHandle, XLL_PASS, "mode %d ", mode);
            }
            bitmap >>= 1;
            mode++;
        }
        if ( IdData->UltraDMAActive ) {
            ;
        } else {
            xLog(LogHandle, XLL_PASS, "none");
        }
    }

    xLog(LogHandle, XLL_PASS, "number of unformatted bytes per track: 0x%x (%d)", IdData->Reserved2[0], IdData->Reserved2[0]);
    xLog(LogHandle, XLL_PASS, "number of unformatted bytes per sector: 0x%x (%d)", IdData->Reserved2[1], IdData->Reserved2[1]);

}

VOID
WINAPI
IdeLogEndTest(
       VOID
       )
/*++

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

--*/ 
{
    //OutputDebugString( TEXT("IDELOG: EndTest is called\n") );
}

BOOL
WINAPI
IdeLogDllMain(
       HINSTANCE   hInstance,
       DWORD       fdwReason,
       LPVOID      lpReserved
       )
/*++

Routine Description:

    This function gets called whenever a dll is loaded, unloaded, 
    a thread created or thread exited

Arguments:

    HINSTANCE   hInstance : instance handle (The value is the base address of the DLL)
    fwdReason : DLL_PROCESS_ATTACH, DLL_THREAD_ATTACH, DLL_THREAD_DETACH,DLL_PROCESS_DETACH
    LPVOID      lpReserved : who knows!

Return Value:

    TRUE if nothing bad happened

--*/ 
{
    UNREFERENCED_PARAMETER( lpReserved );

    if ( fdwReason == DLL_PROCESS_ATTACH ) {

        IdeLogHeapHandle = HeapCreate( 0, 0, 0 );

        if ( !IdeLogHeapHandle ) {
            OutputDebugString( TEXT("IDELOG: Unable to create heap\n") );
            return(FALSE);
        }

    } else if ( fdwReason == DLL_PROCESS_DETACH ) {

        if ( IdeLogHeapHandle ) {
            HeapDestroy( IdeLogHeapHandle );
        }
    }

    return(TRUE);
}

VOID
WINAPI
CheckBCArea(
    VOID
    )
{
    HANDLE hDevice;
    DVD_READ_STRUCTURE ReadStructure;
    UCHAR BCABuffer[64];
    BOOL fResult;
    DWORD cbReturned;
    ULONG i;

    hDevice = CreateFile("cdrom0:", 
                         GENERIC_READ, 
                         FILE_SHARE_READ, 
                         NULL,
                         OPEN_EXISTING, 
                         0, 
                         NULL);

    if (hDevice != INVALID_HANDLE_VALUE) {

        ZeroMemory(&ReadStructure, sizeof(ReadStructure));

        ReadStructure.Format = DvdBCADescriptor;
        ReadStructure.LayerNumber = 0xFE;

        fResult = DeviceIoControl(hDevice, 
                                  IOCTL_DVD_READ_STRUCTURE,
                                  &ReadStructure, 
                                  sizeof(ReadStructure), 
                                  BCABuffer, 
                                  sizeof(BCABuffer),
                                  &cbReturned, NULL);

        if (fResult) {

            xLog(LogHandle, XLL_PASS, "READ BCA succeeded: cbReturned=%d\n", cbReturned);

            for (i = 0; i < sizeof(BCABuffer); i+= 8) {
                xLog(LogHandle, XLL_INFO, "%02x: %02x %02x %02x %02x %02x %02x %02x %02x\n", i,
                    BCABuffer[i + 0], BCABuffer[i + 1], BCABuffer[i + 2],
                    BCABuffer[i + 3], BCABuffer[i + 4], BCABuffer[i + 5],
                    BCABuffer[i + 6], BCABuffer[i + 7]);
            }

        } else {
            xLog(LogHandle, XLL_FAIL, "READ BCA failed: %d\n", GetLastError());
        }

        CloseHandle(hDevice);
    } else {
        xLog(LogHandle, XLL_FAIL, "CreateFile(""cdrom0:"",... Failed : %d\n", GetLastError());
    }
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( idelog )
#pragma data_seg()

BEGIN_EXPORT_TABLE( idelog )
    EXPORT_TABLE_ENTRY( "StartTest", IdeLogStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", IdeLogEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", IdeLogDllMain )
END_EXPORT_TABLE( idelog )
