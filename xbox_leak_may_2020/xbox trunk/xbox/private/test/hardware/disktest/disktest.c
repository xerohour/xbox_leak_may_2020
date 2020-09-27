/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    disktest.c

Abstract:

    Test low-level IDE disk things for the hardware team

written by:

    John Daly (johndaly) 11-Sept-2000

Environment:

    XBox

Revision History:

--*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>

#include <xtl.h>
#include <stdio.h>
#include <xlog.h>
#include <xtestlib.h>
#include <ntdddisk.h>
#include <ntddscsi.h>

#include <pshpack1.h>
typedef struct _IDE_SMART_DATA {
    UCHAR VendorSpecific1[362];                 // bytes 0-361
    UCHAR OffLineCollectionStatus;              // byte 362
    UCHAR VendorSpecific2;                      // byte 363
    WORD  SecondsToComplete;                    // bytes 364, 365
    UCHAR VendorSpecific3;                      // byte 366
    UCHAR OffLineCollectionCapability;          // byte 367
    WORD  SMARTCapability;                      // byte 368, 369
    UCHAR Reserved[16];                         // bytes 370-385
    UCHAR VendorSpecific4[124];                 // bytes 386-510
    UCHAR Checksum;                             // byte 511
} IDE_SMART_DATA, *PIDE_SMART_DATA;
#include <poppack.h>

//
// Valid values for the bCommandReg member of IDEREGS.
//

#define SMART_CMD       0xB0            // Performs SMART cmd.
                                        // Requires valid bFeaturesReg,
                                        // bCylLowReg, and bCylHighReg

//
// Cylinder register defines for SMART command
//

#define SMART_CYL_LOW   0x4F
#define SMART_CYL_HI    0xC2

//
// bDriverError values
//

#define SMART_NO_ERROR          0       // No error
#define SMART_IDE_ERROR         1       // Error from IDE controller
#define SMART_INVALID_FLAG      2       // Invalid command flag
#define SMART_INVALID_COMMAND   3       // Invalid command byte
#define SMART_INVALID_BUFFER    4       // Bad buffer (null, invalid addr..)
#define SMART_INVALID_DRIVE     5       // Drive number not valid
#define SMART_INVALID_IOCTL     6       // Invalid IOCTL
#define SMART_ERROR_NO_MEM      7       // Could not lock user's buffer
#define SMART_INVALID_REGISTER  8       // Some IDE Register not valid
#define SMART_NOT_SUPPORTED     9       // Invalid cmd flag set
#define SMART_NO_IDE_DEVICE     10      // Cmd issued to device not present
                                        // although drive number is valid

//
// Feature register defines for SMART "sub commands"
//

#define READ_ATTRIBUTES         0xD0
#define READ_THRESHOLDS         0xD1
#define ENABLE_DISABLE_AUTOSAVE 0xD2
#define SAVE_ATTRIBUTE_VALUES   0xD3
#define EXECUTE_OFFLINE_DIAGS   0xD4
#define ENABLE_SMART            0xD8
#define DISABLE_SMART           0xD9
#define RETURN_SMART_STATUS     0xDA
#define ENABLE_DISABLE_AUTO_OFFLINE 0xDB


//
// SMART stuff
//

#define SMART_SetRegs(fea) \
    ZeroMemory(atapt, sizeof(ATA_PASS_THROUGH) + 512);  \
    atapt->IdeReg.bCommandReg   = SMART_CMD;            \
    atapt->DataBufferSize       = 512;                  \
    atapt->DataBuffer           = atapt + 1;            \
    atapt->IdeReg.bFeaturesReg  = fea;                  \
    atapt->IdeReg.bCylLowReg    = SMART_CYL_LOW;        \
    atapt->IdeReg.bCylHighReg   = SMART_CYL_HI ;        \
    atapt->IdeReg.bDriveHeadReg = 0;

#define SMART_DumpRegs()    \
            xLog(DiskTestLogHandle, XLL_FAIL, "SMART Error Detected! Investigate");                         \
            xLog(DiskTestLogHandle, XLL_FAIL, "Error Reg;         0x%X ", atapt->IdeReg.bFeaturesReg);      \
            xLog(DiskTestLogHandle, XLL_FAIL, "Sector Count Reg;  0x%X ", atapt->IdeReg.bSectorCountReg);   \
            xLog(DiskTestLogHandle, XLL_FAIL, "Sector Number Reg; 0x%X ", atapt->IdeReg.bSectorNumberReg);  \
            xLog(DiskTestLogHandle, XLL_FAIL, "Cyl Low Reg;       0x%X ", atapt->IdeReg.bCylLowReg);        \
            xLog(DiskTestLogHandle, XLL_FAIL, "Cyl High Reg;      0x%X ", atapt->IdeReg.bCylHighReg);       \
            xLog(DiskTestLogHandle, XLL_FAIL, "Drive Head Reg;    0x%X ", atapt->IdeReg.bDriveHeadReg);     \
            xLog(DiskTestLogHandle, XLL_FAIL, "Status Reg;        0x%X ", atapt->IdeReg.bCommandReg);
            
#define IDE_Status_ERR   0x01
#define IDE_Status_DRQ   0x08
#define IDE_Status_SERV  0x10
#define IDE_Status_DF    0x20
#define IDE_Status_DRDY  0x40
#define IDE_Status_BSY   0x80

//
// timer stuff
//

LARGE_INTEGER Frequency;
LARGE_INTEGER PerformanceCount;

//
// State of current device stuff
// yes, I know that these wonderful global variables will lead to spaghetti code
// so sue me...
//

WORD Current_Device_Heads;
WORD Current_Device_Cylinders;
WORD Current_Device_Sectors;
DWORD Current_Device_LBASectors;

//
// function prototypes
//

VOID
WINAPI
StartTimer(
    VOID
    );

DWORD
WINAPI
EndTimer(
    VOID
    );

VOID
WINAPI
test_identify(
    HANDLE fileHandle,
    PCHAR DeviceName,
    int index
         );

VOID
WINAPI
test_SMART(
    HANDLE fileHandle,
    PCHAR DeviceName,
    int index
    );

VOID
WINAPI
test_Seek(
    HANDLE fileHandle,
    PCHAR DeviceName,
    int index
    );

DWORD
WINAPI
RunSeekTest(
    WORD StartCylinder,
    WORD EndCylinder,
    WORD FullStrokes,
    HANDLE fileHandle
    );

DWORD
WINAPI
LBARunSeekTest(
    DWORD StartCylinder,
    DWORD EndCylinder,
    WORD FullStrokes,
    HANDLE fileHandle
    );

//
// Heap handle from HeapCreate
//

HANDLE DiskTestHeapHandle;
HANDLE DiskTestLogHandle;

VOID
WINAPI
DiskTestStartTest(
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

    DiskTestLogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

--*/ 
{
    DWORD accessMode, shareMode;
    HANDLE fileHandle;
    PIDEREGS pIdeReg;
    CHAR    string[100];
    char    stringa[100];
    ULONG   i, j;
    int DrivesFound = 0;
    IDE_IDENTIFY_DATA idedata = {0};
    DWORD x;

    DiskTestLogHandle = LogHandleX; // make this global - there is no threading intended for this
    
    //
    // set up the timer
    //

    if ( FALSE == QueryPerformanceFrequency(&Frequency) ) {
        ;// log and exit
    }

    for ( i=0; i<2; i++ ) {
        if ( i == 0 ) {
            sprintf (string, "\\\\.\\Device\\Harddisk0\\Partition0");
        } else {
            sprintf (string, "Cdrom0:");
        }

        shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;  // default
        accessMode = GENERIC_READ | GENERIC_WRITE;       // default

        fileHandle = CreateFile(string,
                                accessMode,
                                shareMode,
                                NULL,
                                OPEN_EXISTING,
                                0,
                                NULL);


        if ( fileHandle == INVALID_HANDLE_VALUE ) {
            continue;
        } else {
            ++DrivesFound;
        }

        //
        // run test sequences
        //

        test_identify(fileHandle, string, i);
        test_SMART(fileHandle, string, i);
        test_Seek(fileHandle, string, i);

        //
        // end test sequences
        //

        CloseHandle(fileHandle);
    }
    
    sprintf (stringa, "DrivesFound: %d", DrivesFound);
    xLog(DiskTestLogHandle, XLL_INFO, stringa);

    return;
}

VOID
WINAPI
test_Seek(
    HANDLE fileHandle,
    PCHAR DeviceName,
    int index
    )
/*++

Routine Description:

    measures the Full Stroke seek time (does not differentiate outer->inner, inner->outer)
    measures the track to track seek time from inner, middle, outer track

Arguments:

    HANDLE fileHandle - handle to the selected device
    PWCHAR DeviceName - name of device being tested
    int index - Id index, 0 or 1 (0 hd, 1 cd/dvd)

Return Value:

    None
    

--*/ 
{
    DWORD Time;

    xSetComponent( DiskTestLogHandle, "hwtest", "disktest" );
    xSetFunctionName(DiskTestLogHandle, "test_Seek" );
    xStartVariation( DiskTestLogHandle, "variation1" );

    //
    // for now, only do on disks
    //
    
    if ( index == 1 ) {

        return;
    }

    //
    // Experiment with LBA values - full stroke
    //
    
    Time = LBARunSeekTest(0, Current_Device_LBASectors, 1000, fileHandle);
    if (Time) {
        xLog(DiskTestLogHandle, 
             XLL_PASS, 
             "LBA full stroke, 1000 (inner->outer , outer->inner) cycles : Time: %10.10u ms",
             Time);
    } else {
        xLog(DiskTestLogHandle, 
             XLL_FAIL, 
             "LBA full stroke, 1000 (inner->outer , outer->inner) cycles : FAILED");
    }

    //
    // Full Stroke
    //
    
    Time = RunSeekTest(0, Current_Device_Cylinders, 1000, fileHandle);
    if (Time) {
        xLog(DiskTestLogHandle, 
             XLL_PASS, 
             "full stroke, 1000 (inner->outer , outer->inner) cycles : Time: %10.10u ms",
             Time);
    } else {
        xLog(DiskTestLogHandle, 
             XLL_FAIL, 
             "full stroke, 1000 (inner->outer , outer->inner) cycles : FAILED");
    }
    
    //
    // Track to track, inner
    //
    
    Time = RunSeekTest(0, 1, 1000, fileHandle);
    if (Time) {
    xLog(DiskTestLogHandle, 
         XLL_PASS, 
         "track to track, 1000 (inner->inner + 1, inner + 1 ->inner) cycles : Time: %10.10u ms",
         Time);
    } else {
        xLog(DiskTestLogHandle, 
             XLL_FAIL, 
             "track to track, 1000 (inner->inner + 1, inner + 1 ->inner) cycles : FAILED");
    }
    
    
    //
    // Track to track, middle
    //
    
    Time = RunSeekTest((Current_Device_Cylinders / 2), (Current_Device_Cylinders / 2) + 1, 1000, fileHandle);
    if (Time) {
    xLog(DiskTestLogHandle, 
         XLL_PASS, 
         "track to track, 1000 (middle->middle + 1, middle + 1 -> middle) cycles : Time: %10.10u ms",
         Time);
    } else {
        xLog(DiskTestLogHandle, 
             XLL_FAIL, 
             "track to track, 1000 (middle->middle + 1, middle + 1 -> middle) cycles : FAILED");
    }
    
    //
    // Track to track, outer
    //

    Time = RunSeekTest(Current_Device_Cylinders, Current_Device_Cylinders - 1, 1000, fileHandle);
    if (Time) {
    xLog(DiskTestLogHandle, 
         XLL_PASS, 
         "track to track, 1000 (outer->outer - 1, outer - 1 ->outer) cycles : Time: %10.10u ms",
         Time);
    } else {
        xLog(DiskTestLogHandle, 
             XLL_FAIL, 
             "track to track, 1000 (outer->outer - 1, outer - 1 ->outer) cycles : FAILED");
    }
    
    xEndVariation( DiskTestLogHandle );
}

DWORD
WINAPI
RunSeekTest(
    WORD StartCylinder,
    WORD EndCylinder,
    WORD FullStrokes,
    HANDLE fileHandle
    )
/*++

Routine Description:

    measures the Full Stroke seek time (does not differentiate outer->inner, inner->outer)

Arguments:

    WORD StartCylinder - cylinder to start on
    WORD EndCylinder - cylinder to stop on
    WORD FullStrokes - number of times to move the head
    HANDLE fileHandle - handle to device

Return Value:

    DWORD - elapsed time in Milliseconds
    

--*/ 
{
    BOOL status;
    PATA_PASS_THROUGH atapt;
    DWORD x;
    DWORD returned;

    atapt = HeapAlloc(DiskTestHeapHandle, HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, sizeof(ATA_PASS_THROUGH)); 

    if ( !atapt ) {
        xLog(DiskTestLogHandle, XLL_BLOCK, "memory allocation failure");
        return(0);
    }

    //
    // always start on the same track
    //

    atapt->IdeReg.bCommandReg      = 0x70;  // SEEK
    atapt->IdeReg.bSectorNumberReg = 0;
    atapt->IdeReg.bCylLowReg       = (BYTE)(StartCylinder & 0xFF) ;
    atapt->IdeReg.bCylHighReg      = (BYTE)((StartCylinder & 0xFF00) >> 8);
    atapt->IdeReg.bDriveHeadReg    = 0 | 0x1 /* head 1*/;
    atapt->IdeReg.bHostSendsData   = 0;

    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             NULL);
    if ( status != 0 ) {
        if ((atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||  
            (atapt->IdeReg.bCommandReg & IDE_Status_BSY) || 
            !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DF)) {
            SMART_DumpRegs()
        } else {                       
        //    xLog(DiskTestLogHandle, XLL_PASS, "SEEK:SUCCEEDED");
        }
    } else {
        xLog(DiskTestLogHandle, XLL_FAIL, "SEEK:FAILED");
    }
    
    //
    // start timer
    //

    StartTimer();

    for (x = 0; x < 1000; x++) {

        //
        // seek beginning
        //

        atapt->IdeReg.bCommandReg      = 0x70;  // SEEK
        atapt->IdeReg.bSectorNumberReg = 0;
        atapt->IdeReg.bCylLowReg       = (BYTE)(StartCylinder & 0xFF) ;
        atapt->IdeReg.bCylHighReg      = (BYTE)((StartCylinder & 0xFF00) >> 8);
        atapt->IdeReg.bDriveHeadReg    = 0 | 0x01 /* head 1*/;
        atapt->IdeReg.bHostSendsData   = 0;

        status = DeviceIoControl(fileHandle,
                                 IOCTL_IDE_PASS_THROUGH,
                                 atapt,
                                 sizeof(ATA_PASS_THROUGH),
                                 atapt,
                                 sizeof(ATA_PASS_THROUGH),
                                 &returned,
                                 NULL);
        if ( status != 0 ) {
            if ((atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||  
                (atapt->IdeReg.bCommandReg & IDE_Status_BSY) || 
                !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
                (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
                (atapt->IdeReg.bCommandReg & IDE_Status_DF)) {
                SMART_DumpRegs()
            } else {                       
            //    xLog(DiskTestLogHandle, XLL_PASS, "SEEK:SUCCEEDED");
            }
        } else {
            xLog(DiskTestLogHandle, XLL_FAIL, "SEEK:FAILED");
        }
        
        //
        // seek end
        // 

        atapt->IdeReg.bCommandReg      = 0x70;  // SEEK
        atapt->IdeReg.bSectorNumberReg = 0x0;
        atapt->IdeReg.bCylLowReg       = (BYTE)(EndCylinder & 0xFF) ;
        atapt->IdeReg.bCylHighReg      = (BYTE)((EndCylinder & 0xFF00) >> 8);
        atapt->IdeReg.bDriveHeadReg    = 0 | 0x01 /* head 1*/;
        atapt->IdeReg.bHostSendsData   = 0;

        status = DeviceIoControl(fileHandle,
                                 IOCTL_IDE_PASS_THROUGH,
                                 atapt,
                                 sizeof(ATA_PASS_THROUGH),
                                 atapt,
                                 sizeof(ATA_PASS_THROUGH),
                                 &returned,
                                 NULL);

        if ( status != 0 ) {
            if ((atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||  
                (atapt->IdeReg.bCommandReg & IDE_Status_BSY) || 
                !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
                (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
                (atapt->IdeReg.bCommandReg & IDE_Status_DF)) {
                SMART_DumpRegs()
            } else {                       
            //    xLog(DiskTestLogHandle, XLL_PASS, "SEEK:SUCCEEDED");
            }
        } else {
            xLog(DiskTestLogHandle, XLL_FAIL, "SEEK:FAILED");
        }

    }

    //
    // stop timer
    //

    HeapFree (DiskTestHeapHandle, HEAP_NO_SERIALIZE, atapt);
    return(EndTimer());
    
}
    
DWORD
WINAPI
LBARunSeekTest(
    DWORD StartCylinder,
    DWORD EndCylinder,
    WORD FullStrokes,
    HANDLE fileHandle
    )
/*++

Routine Description:

    measures the Full Stroke seek time (does not differentiate outer->inner, inner->outer)

Arguments:

    WORD StartCylinder - cylinder to start on
    WORD EndCylinder - cylinder to stop on
    WORD FullStrokes - number of times to move the head
    HANDLE fileHandle - handle to device

Return Value:

    DWORD - elapsed time in Milliseconds
    

--*/ 
{
    BOOL status;
    PATA_PASS_THROUGH atapt;
    DWORD x;
    DWORD returned;

    atapt = HeapAlloc(DiskTestHeapHandle, HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, sizeof(ATA_PASS_THROUGH)); 

    if ( !atapt ) {
        xLog(DiskTestLogHandle, XLL_BLOCK, "memory allocation failure");
        return(0);
    }
    
    //
    // always start on the same track
    //

    atapt->IdeReg.bCommandReg      = 0x70;  // SEEK
    atapt->IdeReg.bSectorNumberReg = (BYTE)(StartCylinder & 0xFF);
    atapt->IdeReg.bCylLowReg       = (BYTE)((StartCylinder & 0xFF00) >> 8);
    atapt->IdeReg.bCylHighReg      = (BYTE)((StartCylinder & 0xFF0000) >> 16);
    atapt->IdeReg.bDriveHeadReg    = (BYTE)((StartCylinder & 0x07000000) >> 24) | 0x20 /* LBA */;
    atapt->IdeReg.bHostSendsData   = 0;

    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             NULL);
    if ( status != 0 ) {
        if ((atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||  
            (atapt->IdeReg.bCommandReg & IDE_Status_BSY) || 
            !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DF)) {
            SMART_DumpRegs()
        } else {                       
        //    xLog(DiskTestLogHandle, XLL_PASS, "SEEK:SUCCEEDED");
        }
    } else {
        xLog(DiskTestLogHandle, XLL_FAIL, "SEEK:FAILED");
    }
    
    //
    // start timer
    //

    StartTimer();

    for (x = 0; x < 1000; x++) {

        //
        // seek beginning
        //

        atapt->IdeReg.bCommandReg      = 0x70;  // SEEK
        atapt->IdeReg.bSectorNumberReg = (BYTE)(StartCylinder & 0xFF);
        atapt->IdeReg.bCylLowReg       = (BYTE)((StartCylinder & 0xFF00) >> 8);
        atapt->IdeReg.bCylHighReg      = (BYTE)((StartCylinder & 0xFF0000) >> 16);
        atapt->IdeReg.bDriveHeadReg    = (BYTE)((StartCylinder & 0x07000000) >> 24) | 0x20 /* LBA */;
        atapt->IdeReg.bHostSendsData   = 0;

        status = DeviceIoControl(fileHandle,
                                 IOCTL_IDE_PASS_THROUGH,
                                 atapt,
                                 sizeof(ATA_PASS_THROUGH),
                                 atapt,
                                 sizeof(ATA_PASS_THROUGH),
                                 &returned,
                                 NULL);
        if ( status != 0 ) {
            if ((atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||  
                (atapt->IdeReg.bCommandReg & IDE_Status_BSY) || 
                !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
                (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
                (atapt->IdeReg.bCommandReg & IDE_Status_DF)) {
                SMART_DumpRegs()
            } else {                       
            //    xLog(DiskTestLogHandle, XLL_PASS, "SEEK:SUCCEEDED");
            }
        } else {
            xLog(DiskTestLogHandle, XLL_FAIL, "SEEK:FAILED");
        }
        
        //
        // seek end
        // 

        atapt->IdeReg.bCommandReg      = 0x70;  // SEEK
        atapt->IdeReg.bSectorNumberReg = (BYTE)(EndCylinder & 0xFF);
        atapt->IdeReg.bCylLowReg       = (BYTE)((EndCylinder & 0xFF00) >> 8);
        atapt->IdeReg.bCylHighReg      = (BYTE)((EndCylinder & 0xFF0000) >> 16);
        atapt->IdeReg.bDriveHeadReg    = (BYTE)((EndCylinder & 0x07000000) >> 24) | 0x20 /* LBA */;
        atapt->IdeReg.bHostSendsData   = 0;

        status = DeviceIoControl(fileHandle,
                                 IOCTL_IDE_PASS_THROUGH,
                                 atapt,
                                 sizeof(ATA_PASS_THROUGH),
                                 atapt,
                                 sizeof(ATA_PASS_THROUGH),
                                 &returned,
                                 NULL);

        if ( status != 0 ) {
            if ((atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||  
                (atapt->IdeReg.bCommandReg & IDE_Status_BSY) || 
                !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
                (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
                (atapt->IdeReg.bCommandReg & IDE_Status_DF)) {
                SMART_DumpRegs()
            } else {                       
            //    xLog(DiskTestLogHandle, XLL_PASS, "SEEK:SUCCEEDED");
            }
        } else {
            xLog(DiskTestLogHandle, XLL_FAIL, "SEEK:FAILED");
        }

    }

    //
    // stop timer
    //

    HeapFree (DiskTestHeapHandle, HEAP_NO_SERIALIZE, atapt);
    return(EndTimer());
    
}

VOID
WINAPI
test_identify(
    HANDLE fileHandle,
    PCHAR DeviceName,
    int index
    )
/*++

Routine Description:

    Identifies the device and logs capabilities (like idelog)

Arguments:

    HANDLE fileHandle - handle to the selected device
    PWCHAR DeviceName - name of device being tested
    int index - Id index, 0 or 1 (0 hd, 1 cd/dvd)

Return Value:

    None

--*/
{
#define temp_buff_len   100
    BOOL status;
    int i;
    UCHAR tempBuf[temp_buff_len];
    USHORT bitmap;
    ULONG mode;
    PATA_PASS_THROUGH atapt;
    ULONG length, errorCode, returned;
    PIDE_IDENTIFY_DATA IdData;

    xSetComponent( DiskTestLogHandle, "hwtest", "disktest" );
    xSetFunctionName(DiskTestLogHandle, "test_identify" );
    xStartVariation( DiskTestLogHandle, "variation1" );

    atapt = HeapAlloc(DiskTestHeapHandle, HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, sizeof(ATA_PASS_THROUGH) + 512);

    if ( !atapt ) {
        xLog(DiskTestLogHandle, XLL_BLOCK, "memory allocation failure");
        return;
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

    if ( index == 0 ) {

        atapt->IdeReg.bCommandReg      = 0xec;  // IDENTIFY DEVICE

    } else {

        atapt->IdeReg.bCommandReg      = 0xa1;  // IDENTIFY PACKET DEVICE
    }

    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             NULL);

    if ( status != 0 ) {
        IdData = (PIDE_IDENTIFY_DATA)atapt->DataBuffer;

        xLog(DiskTestLogHandle, XLL_PASS, "%s", DeviceName);

        if ( IdData->GeneralConfiguration & (1 << 15) ) {
            xLog(DiskTestLogHandle, XLL_PASS, "ATAPI device");
        } else {
            xLog(DiskTestLogHandle, XLL_PASS, "ATA device");
        }

        if ( IdData->GeneralConfiguration & (1 << 7) ) {
            xLog(DiskTestLogHandle, XLL_PASS, "removable media device");
        } else {
            xLog(DiskTestLogHandle, XLL_PASS, "non-removable media device");
        }

        if ( IdData->MediaStatusNotification & 1 ) {

            xLog(DiskTestLogHandle, XLL_PASS, "MSN supported");
        } else {

            xLog(DiskTestLogHandle, XLL_PASS, "MSN not supported");
        }

        xLog(DiskTestLogHandle, XLL_PASS, "ATA standard supported: ");
        bitmap = IdData->MajorRevision;
        mode = 0;
        if ( (bitmap != 0) && (bitmap != 0xffff) ) {

            bitmap >>= 1;
            while ( bitmap ) {

                if ( bitmap & 1 ) {
                    xLog(DiskTestLogHandle, XLL_PASS, "ATA-%d ", mode);
                }
                bitmap >>= 1;
                mode++;
            }
        } else {
            xLog(DiskTestLogHandle, XLL_PASS, "none");
        }

        for ( i=0; i<sizeof(IdData->ModelNumber); i+=2 ) {
            tempBuf[i + 0] = IdData->ModelNumber[i + 1];
            tempBuf[i + 1] = IdData->ModelNumber[i + 0];
        }
        tempBuf[i] = 0;
        xLog(DiskTestLogHandle, XLL_PASS, "Model Number: %s", tempBuf);

        for ( i=0; i<sizeof(IdData->SerialNumber); i+=2 ) {
            tempBuf[i + 0] = IdData->SerialNumber[i + 1];
            tempBuf[i + 1] = IdData->SerialNumber[i + 0];
        }
        tempBuf[i] = 0;
        xLog(DiskTestLogHandle, XLL_PASS, "Serial Number: %s", tempBuf);

        for ( i=0; i<sizeof(IdData->FirmwareRevision); i+=2 ) {
            tempBuf[i + 0] = IdData->FirmwareRevision[i + 1];
            tempBuf[i + 1] = IdData->FirmwareRevision[i + 0];
        }
        tempBuf[i] = 0;
        xLog(DiskTestLogHandle, XLL_PASS, "Firmware Revision: %s", tempBuf);

        xLog(DiskTestLogHandle, XLL_PASS, "default number of logical cylinder: 0x%x (%d)", IdData->NumberOfCylinders, IdData->NumberOfCylinders);
        xLog(DiskTestLogHandle, XLL_PASS, "default number of heads: 0x%x (%d)", IdData->NumberOfHeads, IdData->NumberOfHeads);
        xLog(DiskTestLogHandle, XLL_PASS, "default number of sectors per track: 0x%x (%d)", IdData->NumberOfSectorsPerTrack, IdData->NumberOfSectorsPerTrack);

        if ( IdData->MaximumBlockTransfer & 0xff ) {
            xLog(DiskTestLogHandle, XLL_PASS, "maximum number of sectors for READ/WRITE MULTIPLE: 0x%x (%d)", IdData->MaximumBlockTransfer, IdData->MaximumBlockTransfer & 0xf, IdData->MaximumBlockTransfer & 0xf);
        }
        if ( IdData->CurrentMultiSectorSetting & (1 << 8) ) {
            xLog(DiskTestLogHandle, XLL_PASS, "current number of sectors for READ/WRITE MULTIPLE: 0x%x (%d)", IdData->MaximumBlockTransfer, IdData->MaximumBlockTransfer & 0xf, IdData->CurrentMultiSectorSetting & 0xf);
        }

        if ( IdData->Capabilities & (1 << 10) ) {
            xLog(DiskTestLogHandle, XLL_PASS, "IORDY can be disabled");
        }
        if ( IdData->Capabilities & (1 << 11) ) {
            xLog(DiskTestLogHandle, XLL_PASS, "IORDY is supported");
        } else {
            xLog(DiskTestLogHandle, XLL_PASS, "IORDY may be supported");
        }

        xLog(DiskTestLogHandle, XLL_PASS, "Highest PIO mode supported: 0x%x (%d)", IdData->PioCycleTimingMode, IdData->PioCycleTimingMode);

        if ( IdData->TranslationFieldsValid & 1 << 0 ) {

            xLog(DiskTestLogHandle, XLL_PASS, "current number of logical cylinder: 0x%x (%d)", IdData->NumberOfCurrentCylinders, IdData->NumberOfCurrentCylinders);
            Current_Device_Cylinders = IdData->NumberOfCurrentCylinders;
            xLog(DiskTestLogHandle, XLL_PASS, "current number of heads: 0x%x (%d)", IdData->NumberOfCurrentHeads, IdData->NumberOfCurrentHeads);
            Current_Device_Heads = IdData->NumberOfCurrentHeads;
            xLog(DiskTestLogHandle, XLL_PASS, "current number of sectors per track: 0x%x (%d)", IdData->CurrentSectorsPerTrack, IdData->CurrentSectorsPerTrack);
            Current_Device_Sectors = IdData->CurrentSectorsPerTrack;
            xLog(DiskTestLogHandle, XLL_PASS, "current capacity in sectors: 0x%x (%d)", IdData->CurrentSectorCapacity, IdData->CurrentSectorCapacity);
        }

        xLog(DiskTestLogHandle, XLL_PASS, "total number of user addressable LBA sectors: 0x%x (%d)", IdData->UserAddressableSectors, IdData->UserAddressableSectors);
        Current_Device_LBASectors = IdData->UserAddressableSectors;

        if ( IdData->TranslationFieldsValid & (1 << 1) ) {

            xLog(DiskTestLogHandle, XLL_PASS, "advanced PIO supported: ");
            bitmap = IdData->AdvancedPIOModes;
            mode = 0;
            while ( bitmap ) {

                if ( bitmap & 1 ) {
                    xLog(DiskTestLogHandle, XLL_PASS, "mode %d ", mode + 3);
                }
                bitmap >>= 1;
                mode++;
            }
            if ( IdData->AdvancedPIOModes ) {
                ;
            } else {
                xLog(DiskTestLogHandle, XLL_PASS, "none");
            }

            xLog(DiskTestLogHandle, XLL_PASS, "sinlgeword DMA supported: ");
            bitmap = IdData->SingleWordDMASupport;
            mode = 0;
            while ( bitmap ) {

                if ( bitmap & 1 ) {
                    xLog(DiskTestLogHandle, XLL_PASS, "mode %d ", mode);
                }
                bitmap >>= 1;
                mode++;
            }
            if ( IdData->SingleWordDMASupport ) {
                ;
            } else {
                xLog(DiskTestLogHandle, XLL_PASS, "none");
            }

            xLog(DiskTestLogHandle, XLL_PASS, "sinlgeword DMA active: ");
            bitmap = IdData->SingleWordDMAActive;
            mode = 0;
            while ( bitmap ) {

                if ( bitmap & 1 ) {
                    xLog(DiskTestLogHandle, XLL_PASS, "mode %d ", mode);
                }
                bitmap >>= 1;
                mode++;
            }
            if ( IdData->SingleWordDMAActive ) {
                ;
            } else {
                xLog(DiskTestLogHandle, XLL_PASS, "none");
            }
        }

        xLog(DiskTestLogHandle, XLL_PASS, "multiword DMA supported: ");
        bitmap = IdData->MultiWordDMASupport;
        mode = 0;
        while ( bitmap ) {

            if ( bitmap & 1 ) {
                xLog(DiskTestLogHandle, XLL_PASS, "mode %d ", mode);
            }
            bitmap >>= 1;
            mode++;
        }
        if ( IdData->MultiWordDMASupport ) {
            ;
        } else {
            xLog(DiskTestLogHandle, XLL_PASS, "none");
        }

        xLog(DiskTestLogHandle, XLL_PASS, "multiword DMA active: ");
        bitmap = IdData->MultiWordDMAActive;
        mode = 0;
        while ( bitmap ) {

            if ( bitmap & 1 ) {
                xLog(DiskTestLogHandle, XLL_PASS, "mode %d ", mode);
            }
            bitmap >>= 1;
            mode++;
        }
        if ( IdData->MultiWordDMAActive ) {
            ;
        } else {
            xLog(DiskTestLogHandle, XLL_PASS, "none");
        }

        if ( IdData->TranslationFieldsValid & (1 << 1) ) {
            xLog(DiskTestLogHandle, XLL_PASS, "min multiword DMA transfer cycle time (ns): 0x%x (%d)", IdData->MinimumMWXferCycleTime, IdData->MinimumMWXferCycleTime);
            xLog(DiskTestLogHandle, XLL_PASS, "recommended multiword DMA transfer cycle time (ns): 0x%x (%d)", IdData->RecommendedMWXferCycleTime, IdData->RecommendedMWXferCycleTime);
            xLog(DiskTestLogHandle, XLL_PASS, "min PIO transfer cycle time (ns) w/o flow control: 0x%x (%d)", IdData->MinimumPIOCycleTime, IdData->MinimumPIOCycleTime);
            xLog(DiskTestLogHandle, XLL_PASS, "min PIO transfer cycle time (ns) w/ flow control: 0x%x (%d)", IdData->MinimumPIOCycleTimeIORDY, IdData->MinimumPIOCycleTimeIORDY);
        }

        if ( IdData->TranslationFieldsValid & (1 << 2) ) {

            xLog(DiskTestLogHandle, XLL_PASS, "ultra DMA supported: ");
            bitmap = IdData->UltraDMASupport;
            mode = 0;
            while ( bitmap ) {

                if ( bitmap & 1 ) {
                    xLog(DiskTestLogHandle, XLL_PASS, "mode %d ", mode);
                }
                bitmap >>= 1;
                mode++;
            }
            if ( IdData->UltraDMASupport ) {
                ;
            } else {
                xLog(DiskTestLogHandle, XLL_PASS, "none");
            }

            xLog(DiskTestLogHandle, XLL_PASS, "ultra DMA active: ");
            bitmap = IdData->UltraDMAActive;
            mode = 0;
            while ( bitmap ) {

                if ( bitmap & 1 ) {
                    xLog(DiskTestLogHandle, XLL_PASS, "mode %d ", mode);
                }
                bitmap >>= 1;
                mode++;
            }
            if ( IdData->UltraDMAActive ) {
                ;
            } else {
                xLog(DiskTestLogHandle, XLL_PASS, "none");
            }
        }

        xLog(DiskTestLogHandle, XLL_PASS, "number of unformatted bytes per track: 0x%x (%d)", IdData->Reserved2[0], IdData->Reserved2[0]);
        xLog(DiskTestLogHandle, XLL_PASS, "number of unformatted bytes per sector: 0x%x (%d)", IdData->Reserved2[1], IdData->Reserved2[1]);

    }

    HeapFree (DiskTestHeapHandle, HEAP_NO_SERIALIZE, atapt);
    xEndVariation( DiskTestLogHandle );
}

VOID
WINAPI
test_SMART(
    HANDLE fileHandle,
    PCHAR DeviceName,
    int index
    )
/*++

Routine Description:

    execute all the SMART commands against the selected device:
    SMART READ DATA
    SMART ENABLE/DISABLE ATTRIBUTE AUTOSAVE
    SMART SAVE ATTRIBUTE VALUES
    SMART EXECUTE OFF-LINE IMMEDIATE
    SMART ENABLE OPERATIONS
    SMART DISABLE OPERATIONS
    SMART RETURN STATUS

Arguments:

    HANDLE fileHandle - handle to the selected device
    PWCHAR DeviceName - name of device being tested
    int index - Id index, 0 or 1 (0 hd, 1 cd/dvd)

Return Value:

    None

--*/ 
{
    PATA_PASS_THROUGH atapt;
    BOOL status;
    ULONG returned;
    PIDE_SMART_DATA pSmart;
    PUCHAR pSmartChar;
    DWORD x;
    UCHAR checksum;

    xSetComponent( DiskTestLogHandle, "hwtest", "disktest" );
    xSetFunctionName(DiskTestLogHandle, "test_SMART" );
    xStartVariation( DiskTestLogHandle, "variation1" );

    //
    // SMART only works with disk drives, not CD-ROM devices, so punt
    //

    if ( index == 1 ) {

        return;
    }

    atapt = HeapAlloc(DiskTestHeapHandle, HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, sizeof(ATA_PASS_THROUGH) + 512); 

    if ( !atapt ) {
        xLog(DiskTestLogHandle, XLL_BLOCK, "memory allocation failure");
        return;
    }

    // 
    // SMART ENABLE OPERATIONS
    // run this one first to make sure SMART is turned on
    // 
    
    SMART_SetRegs (ENABLE_SMART)
    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             NULL);
    
    if ( status != 0 ) {

        //
        // check registers
        // note that we are abusing the struct here, but it should work and not ever change
        // (fameous last words...)
        //
        // atapt->IdeReg.bFeaturesReg;     // becomes Error after call 
        // atapt->IdeReg.bCommandReg;      // becomes Status after call
        //

        if ((atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||  
            (atapt->IdeReg.bCommandReg & IDE_Status_BSY) || 
            !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DF)) {
            SMART_DumpRegs()
        } else {                       
            xLog(DiskTestLogHandle, XLL_PASS, "SMART:ENABLE_SMART:SUCCEEDED");
        }
    } else {
        xLog(DiskTestLogHandle, XLL_FAIL, "SMART:ENABLE_SMART:FAILED");
    }
    
    //
    // SMART READ DATA
    //

    SMART_SetRegs (READ_ATTRIBUTES)
    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             NULL);

    if ( status != 0 ) {

        //
        // check / verify / log returned data
        //

        pSmart = (PIDE_SMART_DATA)atapt->DataBuffer;
        pSmartChar = (PUCHAR)pSmart;
        
        switch (pSmart->OffLineCollectionStatus) {
            case 0x00:
            case 0x80:
                xLog(DiskTestLogHandle,
                     XLL_PASS, 
                     "OffLineCollectionStatus = never started : 0x%2.2x", 
                     pSmart->OffLineCollectionStatus);
                break;

            case 0x02:
            case 0x82:
                xLog(DiskTestLogHandle, 
                     XLL_PASS, 
                     "OffLineCollectionStatus = completed, no errors : 0x%2.2x",
                     pSmart->OffLineCollectionStatus);
                break;

            case 0x04:
            case 0x84:
                xLog(DiskTestLogHandle, 
                     XLL_PASS, 
                     "OffLineCollectionStatus = suspended, interrupted by host : 0x%2.2x",
                     pSmart->OffLineCollectionStatus);
                break;

            case 0x05:
            case 0x85:
                xLog(DiskTestLogHandle, 
                     XLL_PASS, 
                     "OffLineCollectionStatus = aborted, interrupted by host : 0x%2.2x",
                     pSmart->OffLineCollectionStatus);
                break;

            case 0x06:
            case 0x86:
                xLog(DiskTestLogHandle, 
                     XLL_PASS, 
                     "OffLineCollectionStatus = aborted by device, fatal error : 0x%2.2x",
                     pSmart->OffLineCollectionStatus);
                break;
            
            default:
                xLog(DiskTestLogHandle, 
                     XLL_PASS, 
                     "OffLineCollectionStatus = vendor specific, reserved, or unknown, see IDE spec : 0x%2.2x",
                     pSmart->OffLineCollectionStatus);
                break;

        }
        
        xLog(DiskTestLogHandle,
             XLL_PASS, 
             "SecondsToComplete = : 0x%1.1x",
             pSmart->SecondsToComplete);
        
        xLog(DiskTestLogHandle, 
             XLL_PASS, 
             "OffLineCollectionCapability = 0x%1.1x",
             pSmart->OffLineCollectionCapability);
        
        if(pSmart->OffLineCollectionCapability && 0x01){
            xLog(DiskTestLogHandle, 
                 XLL_PASS, 
                 "OffLineCollectionCapability : EXECUTE OFF-LINE IMMEDIATE implemented");
        } else {
            xLog(DiskTestLogHandle, 
                 XLL_PASS, 
                 "OffLineCollectionCapability : EXECUTE OFF-LINE IMMEDIATE not implemented");
        }
        
        if (pSmart->OffLineCollectionCapability && 0x04) {
            xLog(DiskTestLogHandle, 
                 XLL_PASS, 
                 "OffLineCollectionCapability : EXECUTE OFF-LINE IMMEDIATE will abort with new command");
        } else {
            xLog(DiskTestLogHandle, 
                 XLL_PASS, 
                 "OffLineCollectionCapability : EXECUTE OFF-LINE IMMEDIATE will suspend then resume with new command");
        }

        xLog(DiskTestLogHandle, 
             XLL_PASS, 
             "SMARTCapability = : 0x%1.1x",
             pSmart->SMARTCapability);
        
        if(pSmart->SMARTCapability && 0x01){
            xLog(DiskTestLogHandle, 
                 XLL_PASS, 
                 "SMARTCapability : saves SMART data prior to entering power-saving mode");
        } else {
            xLog(DiskTestLogHandle, 
                 XLL_PASS, 
                 "SMARTCapability : does not save SMART data prior to entering power-saving mode");
        }
        
        if (pSmart->SMARTCapability && 0x02) {
            xLog(DiskTestLogHandle, 
                 XLL_PASS,
                 "SMARTCapability : device complies with data autosave after event");
        } else {
            xLog(DiskTestLogHandle, 
                 XLL_PASS, 
                 "SMARTCapability : device does not comply with data autosave after event");
        }

        checksum = 0;
        for (x = 0; x < 511; x++) {
            //
            // yes, this is supposed to overshoot
            //
            checksum += pSmartChar[x];
        }

        if (checksum != pSmart->Checksum){
            xLog(DiskTestLogHandle, 
             XLL_FAIL, 
            "Checksum : SMART data structure checksum does not match");
        } else {
            xLog(DiskTestLogHandle, 
             XLL_PASS, 
            "Checksum : SMART data structure checksum matches");
        }


    } else {
        xLog(DiskTestLogHandle, XLL_FAIL, "SMART:READ_ATTRIBUTES:FAILED");
    }

    // 
    // SMART ENABLE/DISABLE ATTRIBUTE AUTOSAVE
    // 
    
    //
    // disable
    //

    SMART_SetRegs (ENABLE_DISABLE_AUTOSAVE)
    atapt->IdeReg.bSectorCountReg = 0x00;   // disable

    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             NULL);
    if ( status != 0 ) {
        if ((atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||  
            (atapt->IdeReg.bCommandReg & IDE_Status_BSY) || 
            !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DF)) {
            SMART_DumpRegs()
        } else {                       
            xLog(DiskTestLogHandle, XLL_PASS, "SMART:ENABLE_DISABLE_AUTOSAVE (disable):SUCCEEDED");
        }
    } else {
        xLog(DiskTestLogHandle, XLL_FAIL, "SMART:ENABLE_DISABLE_AUTOSAVE (disable):FAILED");
    }
    

    //
    // enable
    //

    SMART_SetRegs (ENABLE_DISABLE_AUTOSAVE)
    atapt->IdeReg.bSectorCountReg = 0xF1;   // enable
    
    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             NULL);
    
    if ( status != 0 ) {
        if ((atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||  
            (atapt->IdeReg.bCommandReg & IDE_Status_BSY) || 
            !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DF)) {
            SMART_DumpRegs()
        } else {                       
            xLog(DiskTestLogHandle, XLL_PASS, "SMART:ENABLE_DISABLE_AUTOSAVE (enable):SUCCEEDED");
        }
    } else {
        xLog(DiskTestLogHandle, XLL_FAIL, "SMART:ENABLE_DISABLE_AUTOSAVE (enable):FAILED");
    }

    // 
    // SMART SAVE ATTRIBUTE VALUES
    // 
    
    SMART_SetRegs (SAVE_ATTRIBUTE_VALUES)
    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             NULL);
    
    if ( status != 0 ) {
        if ((atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||  
            (atapt->IdeReg.bCommandReg & IDE_Status_BSY) || 
            !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DF)) {
            SMART_DumpRegs()
        } else {                       
            xLog(DiskTestLogHandle, XLL_PASS, "SMART:SAVE_ATTRIBUTE_VALUES:SUCCEEDED");
        }
    } else {
        xLog(DiskTestLogHandle, XLL_FAIL, "SMART:SAVE_ATTRIBUTE_VALUES:FAILED");
    }
    
    // 
    // SMART EXECUTE_OFFLINE_DIAGS
    // then go back on line...
    // 
    
    SMART_SetRegs (EXECUTE_OFFLINE_DIAGS)
    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             NULL);
    
    if ( status != 0 ) {
        if ((atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||  
            (atapt->IdeReg.bCommandReg & IDE_Status_BSY) || 
            !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DF)) {
            SMART_DumpRegs()
        } else {                       
            xLog(DiskTestLogHandle, XLL_PASS, "SMART:EXECUTE_OFFLINE_DIAGS:SUCCEEDED");
        }
    } else {
        xLog(DiskTestLogHandle, XLL_FAIL, "SMART:EXECUTE_OFFLINE_DIAGS:FAILED");
    }
    
    // 
    // SMART DISABLE OPERATIONS
    // then turn them back on... after makining sure they are actually disabled
    // 
    
    SMART_SetRegs (DISABLE_SMART)
    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             NULL);
    
    if ( status != 0 ) {
        if ((atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||  
            (atapt->IdeReg.bCommandReg & IDE_Status_BSY) || 
            !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DF)) {
            SMART_DumpRegs()
        } else {                       
            xLog(DiskTestLogHandle, XLL_PASS, "SMART:DISABLE_SMART:SUCCEEDED");
        }
    } else {
        xLog(DiskTestLogHandle, XLL_FAIL, "SMART:DISABLE_SMART:FAILED");
    }

    //
    // do something simple that should fail...
    //

    SMART_SetRegs (READ_ATTRIBUTES)
    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             NULL);

    if ( status != 0 ) {
        if (!(atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_BSY) ||
            !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DF) ||
            !(atapt->IdeReg.bFeaturesReg & 0x04))    // ABRT in Error register
            {
            SMART_DumpRegs()
        } else {                       
            xLog(DiskTestLogHandle, XLL_PASS, "SMART:READ_ATTRIBUTES:FAILED CORRECTLY");
        }
    } else {
        xLog(DiskTestLogHandle, XLL_FAIL, "SMART:READ_ATTRIBUTES:FAILED");
    }

    //
    // now turn it back on...
    //
    
    SMART_SetRegs (ENABLE_SMART)
    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             NULL);
    
    if ( status != 0 ) {
        if ((atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||  
            (atapt->IdeReg.bCommandReg & IDE_Status_BSY) || 
            !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DF)) {
            SMART_DumpRegs()
        } else {                       
            xLog(DiskTestLogHandle, XLL_PASS, "SMART:ENABLE_SMART:SUCCEEDED");
        }
    } else {
        xLog(DiskTestLogHandle, XLL_FAIL, "SMART:ENABLE_SMART:FAILED");
    }
    
    // 
    // SMART RETURN STATUS
    //
    
    SMART_SetRegs (RETURN_SMART_STATUS)
    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             NULL);

    if ( status != 0 ) {
        if ((atapt->IdeReg.bCommandReg & IDE_Status_ERR) ||  
            (atapt->IdeReg.bCommandReg & IDE_Status_BSY) || 
            !(atapt->IdeReg.bCommandReg & IDE_Status_DRDY) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DRQ) ||
            (atapt->IdeReg.bCommandReg & IDE_Status_DF)) {
            SMART_DumpRegs()
        } else {                       
            xLog(DiskTestLogHandle, XLL_PASS, "SMART:RETURN_SMART_STATUS:SUCCEEDED");
        }
    } else {
        xLog(DiskTestLogHandle, XLL_FAIL, "SMART:RETURN_SMART_STATUS:FAILED");
    }
    
    HeapFree (DiskTestHeapHandle, HEAP_NO_SERIALIZE, atapt);
    xEndVariation( DiskTestLogHandle );

}

VOID
WINAPI
DiskTestEndTest(
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
    OutputDebugString( TEXT("IDELOG: EndTest is called\n") );
}

BOOL
WINAPI
DiskTestDllMain(
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

    None

--*/ 
{
    UNREFERENCED_PARAMETER( hInstance );
    UNREFERENCED_PARAMETER( lpReserved );

    if ( fdwReason == DLL_PROCESS_ATTACH ) {

        DiskTestHeapHandle = HeapCreate( 0, 0, 0 );

        if ( !DiskTestHeapHandle ) {
            OutputDebugString( TEXT("SAMPLEDLL: Unable to create heap\n") );
        }

    } else if ( fdwReason == DLL_PROCESS_DETACH ) {

        if ( DiskTestHeapHandle ) {
            HeapDestroy( DiskTestHeapHandle );
        }
    }

    return(TRUE);
}

DWORD
WINAPI
EndTimer(
        VOID
        )
/*++

Routine Description:

    Ends the test timer and returns the elapsed time in Milliseconds

Arguments:

    None

Return Value:

    elapsed time in Milliseconds

--*/ 
{
    LARGE_INTEGER PerformanceCountFinish;

    QueryPerformanceCounter(&PerformanceCountFinish);
    return(DWORD)((PerformanceCountFinish.QuadPart - PerformanceCount.QuadPart) / (Frequency.QuadPart / 1000));
}

VOID
WINAPI
StartTimer(
          VOID
          )
/*++

Routine Description:

    Start the test timer and returns 

Arguments:

    None

Return Value:

    None

Notes:

    this uses a global variable so the calling function does not have to 
    maintain state. this is to make this as convienient and easy as possible 
    to use and un-clutter the calling function

--*/ 
{
    QueryPerformanceCounter(&PerformanceCount);
}


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( disktest )
#pragma data_seg()

BEGIN_EXPORT_TABLE( disktest )
    EXPORT_TABLE_ENTRY( "StartTest", DiskTestStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DiskTestEndTest )
END_EXPORT_TABLE( disktest )
