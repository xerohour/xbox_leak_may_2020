/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    dvdUtils.cpp

Abstract:

    Functions that talk to the DVD drive

Notes:
    dvd player code:
        "\xboxsrc\private\ui\dvd\library\hardware\drives\specific\xboxdvddrive.cpp"
    filesystem retry code:
        "\xboxsrc\private\ntos\idex\cdrom.c" (IdexCdRomFinishRequestSense)
    sense data format:
        "\xboxsrc\public\ddk\inc\scsi.h"


    DVD-X2 Supported Error Sense Codes
        Sense   Sense   Sense   Description
        Key	    Code    Code Qu
        ----------------------------------------------------------------------
        0x00	0x00	0x00	No Additional Sense information (= no error)
        0x01	0x80	0x00	Cache fill error, speed reduction requested
        0x02	0x04	0x00	Logical Unit not ready, cause not reported
        0x02	0x04	0x01	Logical Unit is in process of becoming ready
        0x02	0x30	0x00	Incompatible medium installed
        0x02	0x3A	0x00	Medium not present
        0x03	0x57	0x00	Unable to recover Table of Contents
        0x03	0x80	0x00	Read Error
        0x04	0x00	0x00	Hardware error
        0x04	0x08	0x03	Logical Unit Communication CRC Error - UDMA33
        0x05	0x21	0x00	Logical Block Address Out of Range
        0x05	0x55	0x00	System Resource Failure
        0x05	0x64	0x00	Illegal mode for this track
        0x05	0x6F	0x00	Authentication Failure - CSS
        0x05	0x80	0x00	Authentication Failure - Xbox
        0x05	0x81	0x00	Command error
        0x06	0x29	0x00	Power On, Reset or Bus Device Reset occurred
        0x06	0x2A	0x01	Mode Parameters changed
        0x06	0x2E	0x00	Insufficient time for operation

    Win32 Error code mapping:
        1/80:   ERROR_CRC                   23
        2/30:   ERROR_UNRECOGNIZED_MEDIA    1785
        2/other ERROR_NOT_READY             21
        3/30:   ERROR_UNRECOGNIZED_MEDIA    1785
        3/other ERROR_CRC                   23
        5/21:   ERROR_SECTOR_NOT_FOUND      27
        5/30:   ERROR_UNRECOGNIZED_MEDIA    1785
        5/6F:   ERROR_TOO_MANY_SECRETS      1381
        5/80:   ERROR_TOO_MANY_SECRETS      1381
        other:  ERROR_IO_DEVICE             1117

    IOCTL_CDROM_SET_SPINDLE_SPEED
        speed 2     ~3000rpm      2x -    5x
        speed 1     ~2000rpm    4/3x - 10/3x
        speed 0     ~1000rpm    2/3x -  5/3x

*****************************************************************************/

#include "dvdread.h"

/*
APIs in this file:

    SCSIError ReadDVDBlocks(HANDLE hDevice, DWORD block, DWORD num, BYTE *buffer);
    SCSIError ReadCDBlocks(HANDLE hDevice, DWORD block, DWORD num, BYTE *buffer);
    SCSIError ReadHDBlocks(HANDLE hDevice, DWORD block, DWORD num, BYTE *buffer);
    void SeekLBA(HANDLE hDevice, DWORD LBA);
    void GetDriveID(HANDLE hDevice, bool cdrom, char* model, char* serial, char* firmware);
    void ChangeSpeed(HANDLE hDevice, DWORD newSpeed);
    bool CDTestReady(HANDLE hDevice);
    void RequestSense(HANDLE hDevice, SCSIError &sense);

*/





/*
    Retry Schema for the DeviceIoControl mode:

                                                action1                     action2                         action3                     action4
    Retry for 01/80/00:                         change speed wait retry.... change speed, wait, retry....   fail
    Retry for 03/80/00, 04/00/00 and 05/55/00:  retry...                    change speed, wait, retry....   change speed, wait, retry   fail
    Retry for 06/2E/00:                         delay 2 sec, retry...       change speed, wait, retry       change speed, wait, retry   fail
    Retry for 06/29/00:                         wait untill drive is ready
    Retry for 05/81/00:                         retry...                    fail
*/
SCSIError ReadDVDBlocks(HANDLE hDevice, DWORD block, DWORD num, BYTE *buffer)
    {
    DWORD time = GetTickCount();
    ++avgTimeCount;

    SCSIError err;
    bool retry = false;
	PCDB Cdb;
    SCSI_PASS_THROUGH_DIRECT PassThrough;
    DWORD cbReturned;
    SENSE_DATA sense;
    SENSE_DATA previousSense;
    memset(&previousSense, 0, sizeof(SENSE_DATA));

    ZeroMemory(&PassThrough, sizeof(PassThrough));
    PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
    PassThrough.DataBuffer = buffer;
    PassThrough.DataTransferLength = num*gDiskGeometry.BytesPerSector;

	Cdb = (PCDB)&PassThrough.Cdb;
    Cdb->CDB10.OperationCode = SCSIOP_READ;
	Cdb->CDB10.ForceUnitAccess = 1;
	Cdb->CDB10.LogicalBlock = RtlUlongByteSwap(block);
	Cdb->CDB10.TransferBlocks = RtlUshortByteSwap((unsigned short)num);

    DWORD actionState = 1;
    DWORD retryCount = 0;

    do {
        if(retry) ++retryCount;
        SetLastError(0);
        memset(&err, 0, sizeof(SCSIError));
        memset(&sense, 0, sizeof(SENSE_DATA));
        retry = false;

        if(!DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT, &PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT), &sense, sizeof(SENSE_DATA), &cbReturned, NULL))
            {
            err.win32 = GetLastError();
            memcpy(&err.sense, &sense, sizeof(SENSE_DATA));
            successfulReadSectors = 0;
            }
        else if(sense.ErrorCode)
            {
            err.win32 = 99999;
            memcpy(&err.sense, &sense, sizeof(SENSE_DATA));

            if(enableRetries && sense.ErrorCode == 0x70)
                {
                if(actionState == 2 && memcmp(&previousSense, &sense, sizeof(SENSE_DATA)) != 0)
                    {
                    if(retryCount == 3)
                        {
                        retry = false;
                        actionState = 99;
                        }
                    else
                        {
                        actionState = 1;
                        }
                    }

                if(actionState == 1)
                    {
                    if((sense.SenseKey == 0x01 && sense.AdditionalSenseCode == 0x80))
                        {
                        if(spindleSpeed == 0) retry = false;
                        else { PrintPreRetryError(block, err); ChangeSpeed(hDevice, spindleSpeed-1); retry = true; }
                        }
                    else if((sense.SenseKey == 0x06 && sense.AdditionalSenseCode == 0x2E))
                        {
                        PrintPreRetryError(block, err);
                        if(seekOn062E)
                            {
                            SeekLBA(hDevice, 0);
                            }
                        Sleep(delayOn062E);

                        retry = true;
                        }
                    else if((sense.SenseKey == 0x06 && sense.AdditionalSenseCode == 0x29))
                        {
                        PrintPreRetryError(block, err);
                        while(CDTestReady(hDevice) == false)
                            {
                            // do nothing until drive is ready
                            }
                        retry = true;
                        }
                    else
                        {
                        PrintPreRetryError(block, err); 
                        retry = true;
                        }
                    }
                else if(actionState == 2)
                    {
                    if((sense.SenseKey == 0x01 && sense.AdditionalSenseCode == 0x80) ||
                       (sense.SenseKey == 0x03 && sense.AdditionalSenseCode == 0x80) ||
                       (sense.SenseKey == 0x04 && sense.AdditionalSenseCode == 0x00) ||
                       (sense.SenseKey == 0x05 && sense.AdditionalSenseCode == 0x55) ||
                       (sense.SenseKey == 0x06 && sense.AdditionalSenseCode == 0x2E) )
                        {
                        if(spindleSpeed == 0) retry = false;
                        else { PrintPreRetryError(block, err); ChangeSpeed(hDevice, spindleSpeed-1); retry = true; }
                        }
                    else
                        {
                        retry = false;
                        }
                    }
                else if(actionState == 3)
                    {
                    if((sense.SenseKey == 0x03 && sense.AdditionalSenseCode == 0x80) ||
                       (sense.SenseKey == 0x04 && sense.AdditionalSenseCode == 0x00) ||
                       (sense.SenseKey == 0x05 && sense.AdditionalSenseCode == 0x55) ||
                       (sense.SenseKey == 0x06 && sense.AdditionalSenseCode == 0x2E) )
                        {
                        if(spindleSpeed == 0) retry = false;
                        else { PrintPreRetryError(block, err); ChangeSpeed(hDevice, spindleSpeed-1); retry = true; }
                        }
                    else
                        {
                        retry = false;
                        }
                    }
                else
                    {
                    retry = false;
                    }
                }

            memcpy(&previousSense, &sense, sizeof(SENSE_DATA));
            successfulReadSectors = 0;
            }
        else
            {
            successfulReadSectors += num;
            }

        if(retry && summaryInternalErrs)
            {
            if(!errors) errors = new ErrorDistrubition(err);
            else errors->AddError(err);
            }

        ++actionState;
        } while(retry);

    if(enableRetries == 1 && spindleSpeed == 0 && successfulReadSectors >= 4096)
        {
        ChangeSpeed(hDevice, spindleSpeed+1);
        successfulReadSectors = 0;
        }
    else if(enableRetries == 1 && spindleSpeed == 1 && successfulReadSectors >= 16384)
        {
        ChangeSpeed(hDevice, spindleSpeed+1);
        successfulReadSectors = 0;
        }

    lastReadTime = GetTickCount()-time;
    avgTime += lastReadTime;
    if(lastReadTime > 1000)
        DebugPrint("Block %u: Read took %ums\n", block, lastReadTime);
    return err;
    }

SCSIError ReadCDBlocks2(HANDLE hDevice, DWORD block, DWORD num, BYTE *buffer)
    {
    // "E:\xboxsrc\private\ntos\idex\cdrom.c" (1519):IdexCdRomStartRawRead(
    DWORD time = GetTickCount();
    ++avgTimeCount;

	PCDB Cdb;
    SCSI_PASS_THROUGH_DIRECT PassThrough;
    DWORD cbReturned;
    SENSE_DATA sense;
    memset(&sense, 0, sizeof(SENSE_DATA));

    SCSIError err;

    // NT drivers seem to want the disk offset to be (sector - 150) * 2048,
    // regardless of what the actual disk lead-in amount is.
    RAW_READ_INFO rri;
    rri.DiskOffset.QuadPart = (unsigned __int64)(block - CDAUDIO_SECTOR_OFFSET) * (unsigned __int64)gDiskGeometry.BytesPerSector;
    rri.SectorCount = num;
    rri.TrackMode = CDDA;


    ZeroMemory(&PassThrough, sizeof(PassThrough));
    PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
    PassThrough.DataBuffer = buffer;
    PassThrough.DataTransferLength = rri.SectorCount * IDE_ATAPI_RAW_CD_SECTOR_SIZE;

	Cdb = (PCDB)&PassThrough.Cdb;
    Cdb->READ_CD.OperationCode = SCSIOP_READ_CD;
    *((PULONG)Cdb->READ_CD.StartingLBA) = RtlUlongByteSwap((ULONG)(rri.DiskOffset.QuadPart >>IDE_ATAPI_CD_SECTOR_SHIFT));
    Cdb->READ_CD.TransferBlocks[2] = (UCHAR)(rri.SectorCount);
    Cdb->READ_CD.TransferBlocks[1] = (UCHAR)(rri.SectorCount >> 8);
    Cdb->READ_CD.TransferBlocks[0] = (UCHAR)(rri.SectorCount >> 16);

    // TrackMode == CDDA
    Cdb->READ_CD.ExpectedSectorType = 1;
    Cdb->READ_CD.IncludeUserData = 1;
    Cdb->READ_CD.HeaderCode = 3;
    Cdb->READ_CD.IncludeSyncData = 1;

/*
    // TrackMode == YellowMode2
    Cdb.READ_CD.ExpectedSectorType = 3;
    Cdb.READ_CD.IncludeUserData = 1;
    Cdb.READ_CD.HeaderCode = 1;
    Cdb.READ_CD.IncludeSyncData = 1;

    // TrackMode == XAForm2
    Cdb.READ_CD.ExpectedSectorType = 5;
    Cdb.READ_CD.IncludeUserData = 1;
    Cdb.READ_CD.HeaderCode = 3;
    Cdb.READ_CD.IncludeSyncData = 1;
*/

    if(!DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT, &PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT), &sense, sizeof(SENSE_DATA), &cbReturned, NULL))
        {
        err.win32 = GetLastError();
        memcpy(&err.sense, &sense, sizeof(SENSE_DATA));
        }
    //status = DeviceIoControl(hDevice, IOCTL_CDROM_RAW_READ, &rri, sizeof(RAW_READ_INFO), buffer, CDAUDIO_BYTES_PER_FRAME * num, &cbReturned, NULL);
    //status = DeviceIoControl(hDevice, IOCTL_CDROM_RAW_READ, &rri, sizeof(RAW_READ_INFO), buffer, gDiskGeometry.BytesPerSector * num, &cbReturned, NULL);


    lastReadTime = GetTickCount()-time;
    avgTime += lastReadTime;
    if(lastReadTime > 1000)
        DebugPrint("Block %u: Read took %ums\n", block, lastReadTime);
    return err;
    }


SCSIError ReadCDBlocks(HANDLE hDevice, DWORD block, DWORD num, BYTE *buffer)
    {
    DWORD time = GetTickCount();
    ++avgTimeCount;

    SCSIError err;
    NTSTATUS status;
    DWORD cbReturned;
    RAW_READ_INFO rri;

    // NT drivers seem to want the disk offset to be (sector - 150) * 2048,
    // regardless of what the actual disk lead-in amount is.
    rri.DiskOffset.QuadPart = (unsigned __int64)(block - CDAUDIO_SECTOR_OFFSET) * (unsigned __int64)gDiskGeometry.BytesPerSector;
    rri.SectorCount = num;
    rri.TrackMode = CDDA;

    status = DeviceIoControl(hDevice, IOCTL_CDROM_RAW_READ, &rri, sizeof(RAW_READ_INFO), buffer, CDAUDIO_BYTES_PER_FRAME * num, &cbReturned, NULL);
    //status = DeviceIoControl(hDevice, IOCTL_CDROM_RAW_READ, &rri, sizeof(RAW_READ_INFO), buffer, gDiskGeometry.BytesPerSector * num, &cbReturned, NULL);

    if(!NT_SUCCESS(status))
        {
        err.win32 = RtlNtStatusToDosError(status);
        }

    lastReadTime = GetTickCount()-time;
    avgTime += lastReadTime;
    if(lastReadTime > 1000)
        DebugPrint("Block %u: Read took %ums\n", block, lastReadTime);
    return err;
    }

SCSIError ReadHDBlocks(HANDLE hDevice, DWORD block, DWORD num, BYTE *buffer)
    {
    DWORD time = GetTickCount();
    ++avgTimeCount;

    SCSIError err;
    IO_STATUS_BLOCK ioStatusBlock;
    NTSTATUS status;
    LARGE_INTEGER Offset;

    // Calculate Offset
    Offset.QuadPart = (unsigned __int64)block * (unsigned __int64)gDiskGeometry.BytesPerSector;

    // Read bytes at offset
    status = NtReadFile(hDevice, NULL, NULL, NULL, &ioStatusBlock, buffer, num * gDiskGeometry.BytesPerSector, &Offset);

    if(!NT_SUCCESS(status))
        {
        err.win32 = RtlNtStatusToDosError(status);
        }

    lastReadTime = GetTickCount()-time;
    avgTime += lastReadTime;
    if(lastReadTime > 1000)
        DebugPrint("Block %u: Read took %ums\n", block, lastReadTime);
    return err;
    }


void GetDriveID(HANDLE hDevice, bool cdrom, char* model, char* serial, char* firmware)
    {
    unsigned i;
    DWORD returned;
    char buffer[sizeof(ATA_PASS_THROUGH) + 512];
    PATA_PASS_THROUGH atapt = (PATA_PASS_THROUGH)buffer;

    atapt->DataBufferSize = 512;
    atapt->DataBuffer = atapt + 1;

    atapt->IdeReg.bFeaturesReg     = 0;
    atapt->IdeReg.bSectorCountReg  = 0;
    atapt->IdeReg.bSectorNumberReg = 0;
    atapt->IdeReg.bCylLowReg       = 0;
    atapt->IdeReg.bCylHighReg      = 0;
    atapt->IdeReg.bDriveHeadReg    = 0;
    atapt->IdeReg.bHostSendsData   = 0;

    if(cdrom) atapt->IdeReg.bCommandReg = 0xA1;
    else atapt->IdeReg.bCommandReg = 0xEC;

    DeviceIoControl(hDevice, IOCTL_IDE_PASS_THROUGH, atapt, sizeof(ATA_PASS_THROUGH), atapt, sizeof(ATA_PASS_THROUGH), &returned, FALSE);
    PIDE_IDENTIFY_DATA IdData = (PIDE_IDENTIFY_DATA)atapt->DataBuffer;

    for(i=0; i<sizeof(IdData->ModelNumber); i+=2)
        {
        model[i + 0] = IdData->ModelNumber[i + 1];
        model[i + 1] = IdData->ModelNumber[i + 0];
        }
    model[i] = 0;

    for(i=0; i<sizeof(IdData->SerialNumber); i+=2)
        {
        serial[i + 0] = IdData->SerialNumber[i + 1];
        serial[i + 1] = IdData->SerialNumber[i + 0];
        }
    serial[i] = 0;

    for(i=0; i<sizeof(IdData->FirmwareRevision); i+=2)
        {
        firmware[i + 0] = IdData->FirmwareRevision[i + 1];
        firmware[i + 1] = IdData->FirmwareRevision[i + 0];
        }
    firmware[i] = 0;
    }


void ChangeSpeed(HANDLE hDevice, DWORD newSpeed)
    {
    DWORD cbReturned;

    if(spindleSpeed == newSpeed) return;

    DeviceIoControl(hDevice, IOCTL_CDROM_SET_SPINDLE_SPEED, &newSpeed, sizeof(newSpeed), NULL, 0, &cbReturned, NULL);
    DebugPrint("Speed changed to %u (ec: %u)\n", newSpeed, GetLastError());

    if(spindleSpeed == 0 && newSpeed == 1) Sleep(speedDelay0to1);
    else if(spindleSpeed == 1 && newSpeed == 2) Sleep(speedDelay1to2);
    else if(spindleSpeed == 2 && newSpeed == 1) Sleep(speedDelay2to1);
    else if(spindleSpeed == 1 && newSpeed == 0) Sleep(speedDelay1to0);
    else if(spindleSpeed == 0 && newSpeed == 2) Sleep(speedDelay0to2);
    else if(spindleSpeed == 2 && newSpeed == 0) Sleep(speedDelay2to0);

    spindleSpeed = newSpeed;
    }


void SeekLBA(HANDLE hDevice, DWORD LBA)
    {
    SCSI_PASS_THROUGH_DIRECT PassThrough;
    SENSE_DATA SenseData;
    DWORD cbBytesReturned;
    PCDB Cdb;

    RtlZeroMemory(&PassThrough, sizeof(PassThrough));
    PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);

    Cdb = (PCDB)&PassThrough.Cdb;
    Cdb->SEEK.OperationCode = SCSIOP_SEEK;
    *((PULONG)Cdb->SEEK.LogicalBlockAddress) = RtlUlongByteSwap(LBA);

    DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
								&PassThrough, sizeof(PassThrough), 
                                &SenseData, sizeof(SenseData),
                                &cbBytesReturned, NULL);
    
    
    DebugPrint("Seek to %u (ec: %u)\n", LBA, GetLastError());
    }


bool CDTestReady(HANDLE hDevice)
    {
    DWORD cbReturned;

    BOOL fReturn = DeviceIoControl(hDevice, IOCTL_CDROM_CHECK_VERIFY, NULL, 0, NULL, 0, &cbReturned, NULL);

    if(fReturn || (GetLastError() == ERROR_UNRECOGNIZED_MEDIA))
        {
        return false;
        }
    
    return true;
    }


void RequestSense(HANDLE hDevice, SCSIError &err)
    {
	PCDB Cdb;
    SCSI_PASS_THROUGH_DIRECT PassThrough;
    DWORD cbReturned;
    SENSE_DATA sense;

    ZeroMemory(&PassThrough, sizeof(PassThrough));
    PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
    PassThrough.DataBuffer = &sense;
    PassThrough.DataTransferLength = sizeof(SENSE_DATA);

	Cdb = (PCDB)&PassThrough.Cdb;
    Cdb->CDB6INQUIRY.OperationCode = SCSIOP_REQUEST_SENSE;
	Cdb->CDB6INQUIRY.AllocationLength = sizeof(SENSE_DATA);

    SetLastError(0);
    memset(&err, 0, sizeof(SCSIError));
    memset(&sense, 0, sizeof(SENSE_DATA));

    DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT, &PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT), &sense, sizeof(SENSE_DATA), &cbReturned, NULL);

    err.win32 = GetLastError();
    memcpy(&err.sense, &sense, sizeof(SENSE_DATA));
    }





DWORD GetDiskType(HANDLE hDevice)
    {
    DVD_READ_STRUCTURE readStruct;
    UCHAR readStructureOutput[ALIGN_UP(sizeof(DVD_DESCRIPTOR_HEADER) + sizeof(DVD_LAYER_DESCRIPTOR), ULONG)];

    memset(&readStruct, 0, sizeof(DVD_READ_STRUCTURE));
    memset(&readStructureOutput, 0, sizeof(readStructureOutput));

    // try DVD Read Structure
    readStruct.Format = DvdPhysicalDescriptor;

    DWORD returned = 0;
    BOOL result = DeviceIoControl(hDevice, IOCTL_DVD_READ_STRUCTURE, &readStruct, sizeof(DVD_READ_STRUCTURE), &readStructureOutput, sizeof(readStructureOutput), &returned, NULL);
    if(result)
        {
        DVD_LAYER_DESCRIPTOR *layerDescr = (DVD_LAYER_DESCRIPTOR *)(&readStructureOutput[sizeof(DVD_DESCRIPTOR_HEADER)]);
        BYTE layerType = (layerDescr->BookType) | (layerDescr->BookVersion << 4);
        if(layerDescr->LayerType == 1)
            {
            if(layerDescr->NumberOfLayers == 0)
                return MEDIA_DVD_5_RO;
            else return MEDIA_DVD_9_RO;
            }
        else
            {
            if(layerDescr->NumberOfLayers == 0)
                return MEDIA_DVD_5_RW;
            else return MEDIA_DVD_9_RW;
            }
        }

    // not a DVD disk...
    else
        {
	    CDROM_TOC toc;
        memset(&toc, 0, sizeof(CDROM_TOC));
		result = DeviceIoControl( hDevice, IOCTL_CDROM_READ_TOC, &toc, sizeof(CDROM_TOC), &toc, sizeof(CDROM_TOC), &returned, NULL);
        if(result)
            {
            if(toc.FirstTrack == toc.LastTrack) return MEDIA_CDROM;
            return MEDIA_CDDA;
            }
        else
            {
            return MEDIA_CDROM;
            }
        }

    return MEDIA_UNKNOWN;
    }
