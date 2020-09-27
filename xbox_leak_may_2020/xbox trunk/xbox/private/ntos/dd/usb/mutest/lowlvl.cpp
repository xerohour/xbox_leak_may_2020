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

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntdddisk.h>
#include <stdio.h>
#include <xtl.h>
extern "C" {
#include <ntos.h>
#include <xapip.h>  //private XAPI methods for low-level tests
}

#include <mutest.h>
#include <stdio.h>

//
// to test the double buffering code on the 8k fw restricted writes
// play with the two values below
// I tested (A=8k aligned, NA=non aligned)
// a) offset=A, length=N, 4k
// b) offset=NA,length=A, 8k
// c) offset=A, length=N, 12k
// d) offset=NA,length=N, 12k
// f) offset=NA,length=N, 4k
// g) offset=NA,length=A, 16k

#define WRITE_SIZE       (4096*256)   //1MB write size
#define TEST_BUFFER_SIZE (4096*32)    //128KB
#define TRANSFER_SIZE    4096
#define MU_MEDIA_OFFSET  4096*8

#define EXTRA_SIZE 4096
#define SECTOR_MASK 0xFFFFFF000

VOID
LowLevelTest(
          ULONG Port,
          ULONG Slot
          )
{
	
	ULONG					tryCount = 0;
    OCHAR                   name[64];
    HRESULT                 hr;
    HANDLE                  hFile;
    PUCHAR buffer,buffer2;
    UCHAR pattern[4] = {1,2,3,4};
    ULONG i;
    HANDLE hVolume;
    OBJECT_ATTRIBUTES oa;
    IO_STATUS_BLOCK statusBlock;
    DISK_GEOMETRY diskGeometry;
    OCHAR oszDeviceObjectBuffer[64];
    OBJECT_STRING objName;
    NTSTATUS status;
    ULONG   totalErrorCount = 0;
    ULONG   localErrorCount = 0;
    ULONGLONG bytesPerSecond = 0;
    

    //
    //  Create a device object for the MU
    //
    objName.Length = 
    objName.MaximumLength = sizeof(oszDeviceObjectBuffer)-1;
    objName.Buffer = oszDeviceObjectBuffer;
    status = MU_CreateDeviceObject(Port, Slot, &objName);
    if(NT_ERROR(status))
    {
        DebugPrint("Could not create device object for MU, status = 0x%0.8x");
        return;
    }

    buffer =  (PUCHAR) LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, TEST_BUFFER_SIZE+EXTRA_SIZE);
    buffer2 =  (PUCHAR) LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, TEST_BUFFER_SIZE+EXTRA_SIZE);

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

    for (i=0;i<TEST_BUFFER_SIZE/sizeof(ULONG);i++) {

        *((PULONG)buffer+i) = i;

    }

    DebugPrint("MUTEST: LowLevelTest: Test buffer 1 at %x, 2 at %x\n",buffer,buffer2);

    InitializeObjectAttributes(&oa, (POBJECT_STRING) &objName, OBJ_CASE_INSENSITIVE, NULL, NULL);

    DebugPrint("MUTEST: LowLevelTest: Opening volume \n");

    status = NtOpenFile(&hVolume,
                        SYNCHRONIZE | GENERIC_ALL,
                        &oa,
                        &statusBlock,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        FILE_NO_INTERMEDIATE_BUFFERING | FILE_SYNCHRONOUS_IO_ALERT);

    if (NT_SUCCESS(status)) {

        status = NtDeviceIoControlFile(hVolume,
                                       0,
                                       NULL,
                                       NULL,
                                       &statusBlock,
                                       IOCTL_DISK_GET_DRIVE_GEOMETRY,
                                       NULL,
                                       0,
                                       &diskGeometry,
                                       sizeof(diskGeometry));

        if (NT_SUCCESS(status)) {
            PARTITION_INFORMATION pi;
            ULONG ulSectorSize = diskGeometry.BytesPerSector;
            ULONG ulSectorsPerTrack = diskGeometry.SectorsPerTrack;
            ULONG ulHeads = diskGeometry.TracksPerCylinder;
            ULONG ulTotalSectorCount = (RtlExtendedIntegerMultiply(
                                        diskGeometry.Cylinders,
                                        diskGeometry.SectorsPerTrack *
                                            diskGeometry.TracksPerCylinder)).LowPart;

            DebugPrint("MUTEST: SectorSize %x, SectorCount %x\n",
                       ulSectorSize,
                       ulTotalSectorCount);
                                            
            status = NtDeviceIoControlFile(hVolume,
                                           0,
                                           NULL,
                                           NULL,
                                           &statusBlock,
                                           IOCTL_DISK_GET_PARTITION_INFO,
                                           NULL,
                                           0,
                                           &pi,
                                           sizeof(pi));
                                                      
            if ((NT_SUCCESS(status) && pi.RecognizedPartition) ||
                (status == STATUS_NOT_SUPPORTED)) {

                LARGE_INTEGER offset;
                LARGE_INTEGER start, end;
                LARGE_INTEGER freq;
                
                offset.QuadPart = MU_MEDIA_OFFSET;
                freq = KeQueryPerformanceFrequency();
                start = KeQueryPerformanceCounter();
                
                DebugPrint("MUTEST: Writing complete test pattern to offset %x, iterations %x, write length %x\n",
                           offset.LowPart,
                           WRITE_SIZE/TEST_BUFFER_SIZE,
                           TEST_BUFFER_SIZE);

                

                for (i=0;i<WRITE_SIZE/TEST_BUFFER_SIZE;i++) {

                    //DebugPrint("MUTEST: current offset %x\n",
                    //       offset.LowPart);

                    status = NtWriteFile(hVolume,
                                         0,
                                         NULL,
                                         NULL,
                                         &statusBlock,
                                         buffer,
                                         TEST_BUFFER_SIZE,
                                         &offset
                                         );
                    offset.QuadPart += TEST_BUFFER_SIZE;
                    if(NT_ERROR(status)) localErrorCount++;
                }

                end = KeQueryPerformanceCounter();
                end.QuadPart -= start.QuadPart;                

                if(localErrorCount)
                {
                    DebugPrint("MUTEST: %d write errors occured.\n", localErrorCount);
                    totalErrorCount += localErrorCount;
                }
                
                //
                //  Calculate bytes per second.
                //
                bytesPerSecond = (WRITE_SIZE * freq.QuadPart)/end.QuadPart;
                DebugPrint("MUTEST: data rate writing %d bytes was %d bytes per second.\n", WRITE_SIZE, (ULONG)bytesPerSecond);
                
                

                ///////////////////////////////////////////////////////////////
                //
                // READ perf test
                //
                ///////////////////////////////////////////////////////////////

                offset.QuadPart = MU_MEDIA_OFFSET;
                localErrorCount = 0;
                start = KeQueryPerformanceCounter();
                
                DebugPrint("MUTEST: Reading complete test pattern to offset %x, iterations %x, write length %x\n",
                           offset.LowPart,
                           WRITE_SIZE/TEST_BUFFER_SIZE,
                           TEST_BUFFER_SIZE);

                for (i=0;i<WRITE_SIZE/TEST_BUFFER_SIZE;i++) {

                    //DebugPrint("MUTEST: current offset %x\n",
                    //       offset.LowPart);

                    status = NtReadFile(hVolume,
                                         0,
                                         NULL,
                                         NULL,
                                         &statusBlock,
                                         buffer2,
                                         TEST_BUFFER_SIZE,
                                         &offset
                                         );
                    offset.QuadPart += TEST_BUFFER_SIZE;
                    if(NT_ERROR(status)) localErrorCount++;
                }

                end = KeQueryPerformanceCounter();
                end.QuadPart -= start.QuadPart;                

                if(localErrorCount)
                {
                    DebugPrint("MUTEST: %d read errors occured.\n", localErrorCount);
                    totalErrorCount += localErrorCount;
                }
                
                //
                //  Calculate bytes per second.
                //
                bytesPerSecond = (WRITE_SIZE * freq.QuadPart)/end.QuadPart;
                DebugPrint("MUTEST: data rate reading %d bytes was %d bytes per second.\n", WRITE_SIZE, (ULONG)bytesPerSecond);

                ///////////////////////////////////////////////////////////////
                //
                // check loop back for errors
                //
                ///////////////////////////////////////////////////////////////
                
                localErrorCount = TEST_BUFFER_SIZE - RtlCompareMemory(buffer, buffer2, TEST_BUFFER_SIZE);
                if(0==localErrorCount)
                {
                  DebugPrint("MUTEST: data read back matched what was written.\n");
                } else
                {
                    DebugPrint("MUTEST: There were %d errors in the read back.\n", localErrorCount);
                    totalErrorCount += localErrorCount;
                    DebugPrint("MUTEST: Offset: Written,  Read\n", i, buffer[i], buffer2[i]);
                    int errorCount = 0;
                    for(int i=0; i<TEST_BUFFER_SIZE; i++)
                    {
                        if(buffer[i]!=buffer2[i])
                        {
                            errorCount++;
                            if(errorCount < 5)
                            {
                                DebugPrint("MUTEST: 0x%0.4x:   0x%0.2x,  0x%0.2x\n", i, (ULONG)buffer[i], (ULONG)buffer2[i]);
                            } else
                            {
                                break;
                            }
                        }
                    }
                    _asm int 3;
                }


                ///////////////////////////////////////////////////////////////
                //
                // alignment fault check.
                //
                //  Test on 4k, 8k, 12k and 16k writes.  Induce alignment faults by
                //  repeating each write at three times changing the offset by
                //  4k each time.  
                //  
                //  Test by wiping the read back buffer, and reading the entire
                //  MU contents.  These should match the cached buffer with
                //  what ever changes were made to the pattern in the portion
                //  written.
                //
                ///////////////////////////////////////////////////////////////

                offset.QuadPart = MU_MEDIA_OFFSET;

                
                //
                //  Loop over write size
                //
                ULONG writeOffset = 0;
                for(ULONG writeLength = 0x1000; writeLength <= 0x4000; writeLength += 0x1000)
                {
                    //
                    //  repeat each length with three different offsets.
                    //  never reset the offset to 0, just keep incrementing by 4k
                    for(ULONG offsetRepeat = 0; offsetRepeat < 3; offsetRepeat++, writeOffset += 0x1000)
                    {
                        DebugPrint("MUTEST: Alignment Test: writeOffset = 0x%0.8x, writeLength = 0x%0.8x", writeOffset, writeLength);
                        //
                        //  fill write buffer with test pattern
                        //
                        
                        for(i=0; i < writeLength; i+=4)
                        {
                            *((PULONG)(buffer+i+writeOffset)) = writeLength-i;
                        }
                        
                        //
                        //  Setup offset, we bias the offset to try to avoid
                        //  corrupting a formatted MU.
                        //

                        offset.QuadPart = MU_MEDIA_OFFSET + writeOffset;

                        //
                        //  Execute write
                        //
                        status = NtWriteFile(hVolume,
                                         0,
                                         NULL,
                                         NULL,
                                         &statusBlock,
                                         buffer + writeOffset,
                                         writeLength,
                                         &offset
                                         );

                        if (!NT_SUCCESS(status)) {
                            DebugPrint("\nMUTEST: Write failed with status 0x%0.8x\n", status);
                            continue;
                        }

                        //
                        //  wipe read buffer
                        //
                        
                        RtlZeroMemory(buffer2, TEST_BUFFER_SIZE);

                        //
                        //  Reset offset to beginning of MU test area
                        //

                        offset.QuadPart = MU_MEDIA_OFFSET;

                        //
                        //  execute read on whole test area
                        //
                        status = NtReadFile(hVolume,
                                         0,
                                         NULL,
                                         NULL,
                                         &statusBlock,
                                         buffer2,
                                         TEST_BUFFER_SIZE,
                                         &offset
                                         );
                        if (!NT_SUCCESS(status)) {
                            DebugPrint("\nMUTEST: read failed with status 0x%0.8x\n", status);
                            continue;
                        }

                        //
                        //  Byte compare the buffers
                        //
                        localErrorCount = TEST_BUFFER_SIZE - RtlCompareMemory(buffer, buffer2, TEST_BUFFER_SIZE);
                        if(0==localErrorCount)
                        {
                            DebugPrint(": Success\n");
                        } else
                        {
                            DebugPrint(": Error\nMUTEST: There were %d errors, dumping first five errors.\n", localErrorCount);
                            totalErrorCount += localErrorCount;
                            DebugPrint("MUTEST: Offset: Written,  Read\n", i, buffer[i], buffer2[i]);
                            int errorCount = 0;
                            for(int i=0; i<TEST_BUFFER_SIZE; i++)
                            {
                                if(buffer[i]!=buffer2[i])
                                {
                                    errorCount++;
                                    if(errorCount < 5)
                                    {
                                        DebugPrint("MUTEST: 0x%0.4x:   0x%0.2x,  0x%0.2x\n", i, (ULONG)buffer[i], (ULONG)buffer2[i]);
                                    } else
                                    {
                                        break;
                                    }
                                }
                            }
                            _asm int 3;
                        }

                    }   //loop over write offset
                } //loop over write length
            } else  // if get geometry failed
            {
                DebugPrint("MUTEST: Couldn't get drive geometry, status 0x%0.8x\n", status);
                totalErrorCount++;
            }
        } else  // if get partition info failed
        {
                DebugPrint("MUTEST: Couldn't get partition info, status 0x%0.8x\n", status);
                totalErrorCount++;
        }
        //
        //  Close the volume
        //
        NtClose(hVolume);
    } else  //if NtOpenFile failed
    {
            DebugPrint("MUTEST: NtOpenFileFailed, status 0x%0.8x\n", status);
            totalErrorCount++;
    }
    if(totalErrorCount)
    {
        DebugPrint("MUTEST: FAILURE: Test completed with %d errors\n", totalErrorCount);
    } else
    {
        DebugPrint("MUTEST: SUCCESS: No Errors\n");
    }

    //
    //  Format the Mu
    //
    
    DebugPrint("MUTEST: Formatting the MU, so we can do the higher level tests\n");
    if (XapiFormatFATVolume(&objName))
    {
        DbgPrint("FormatFATVolume() succeeded\n");
    } else
    {
        DbgPrint("FormatFATVolume() failed\n");
    }

    //
    //  Close the device object that we created to test the mu.
    //

    MU_CloseDeviceObject(Port, Slot);
}

