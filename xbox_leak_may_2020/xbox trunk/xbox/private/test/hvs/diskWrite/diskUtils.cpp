/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    diskUtils.cpp

Abstract:

    Functions that talk to the DVD / Hard drive

Notes:

*****************************************************************************/

#include "diskWrite.h"

/*
APIs in this file:

    void GetDriveID(HANDLE hDevice, bool cdrom, char* model, char* serial, char* firmware);
    DWORD GetDiskType(HANDLE hDevice);

*/

DWORD ReadHDBlocks(HANDLE hDevice, DWORD block, DWORD num, BYTE *buffer)
    {
    DWORD accessTime = GetTickCount();
    ++avgTimeCount;

    IO_STATUS_BLOCK ioStatusBlock;
    DWORD status;
    LARGE_INTEGER Offset;

    // Calculate Offset
    Offset.QuadPart = (unsigned __int64)block * (unsigned __int64)globalSettings.gDiskGeometry.BytesPerSector;

    // Read bytes at offset
    status = NtReadFile(hDevice, NULL, NULL, NULL, &ioStatusBlock, buffer, num * globalSettings.gDiskGeometry.BytesPerSector, &Offset);

    lastReadTime = GetTickCount()-accessTime;
    avgTime += lastReadTime;
    if(lastReadTime > 1000)
        LogPrint("Block %u: Read took %ums\n", block, lastReadTime);

    return status;
    }

DWORD WriteHDBlocks(HANDLE hDevice, DWORD block, DWORD num, BYTE *buffer)
    {
    DWORD accessTime = GetTickCount();
    ++avgTimeCount;

    IO_STATUS_BLOCK ioStatusBlock;
    DWORD status;
    LARGE_INTEGER Offset;

    // Calculate Offset
    Offset.QuadPart = (unsigned __int64)block * (unsigned __int64)globalSettings.gDiskGeometry.BytesPerSector;

    // Read bytes at offset
    status = NtWriteFile(hDevice, NULL, NULL, NULL, &ioStatusBlock, buffer, num * globalSettings.gDiskGeometry.BytesPerSector, &Offset);

    lastReadTime = GetTickCount()-accessTime;
    avgTime += lastReadTime;
    if(lastReadTime > 1000)
        LogPrint("Block %u: Write took %ums\n", block, lastReadTime);

    return status;
    }


void GetDriveID(HANDLE hDevice, bool cdrom, char* model, char* serial, char* firmware)
    {
    unsigned i;
    DWORD returned;
    char buffer[sizeof(ATA_PASS_THROUGH) + 512];
    PATA_PASS_THROUGH atapt = (PATA_PASS_THROUGH)buffer;
    bool closeHandle = false;

    if(hDevice == NULL)
        {
        OBJECT_ATTRIBUTES ObjA;
        OBJECT_STRING VolumeString;
        IO_STATUS_BLOCK IoStatusBlock;
        if(cdrom)
            RtlInitObjectString(&VolumeString, "\\Device\\cdrom0");
        else
            RtlInitObjectString(&VolumeString, "\\Device\\Harddisk0\\Partition0");
        InitializeObjectAttributes(&ObjA, &VolumeString, OBJ_CASE_INSENSITIVE, NULL, NULL);
        NtCreateFile(&hDevice, SYNCHRONIZE|GENERIC_READ, &ObjA, &IoStatusBlock, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT | FILE_NO_INTERMEDIATE_BUFFERING);
        closeHandle = true;
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

    if(closeHandle) NtClose(hDevice);
    }


DWORD GetDiskType(HANDLE hDevice)
    {
    DVD_READ_STRUCTURE readStruct;
    UCHAR readStructureOutput[ALIGN_UP(sizeof(DVD_DESCRIPTOR_HEADER) + sizeof(DVD_LAYER_DESCRIPTOR), ULONG)];

    bool closeHandle = false;

    if(hDevice == NULL)
        {
        OBJECT_ATTRIBUTES ObjA;
        OBJECT_STRING VolumeString;
        IO_STATUS_BLOCK IoStatusBlock;
        RtlInitObjectString(&VolumeString, "\\Device\\cdrom0");
        InitializeObjectAttributes(&ObjA, &VolumeString, OBJ_CASE_INSENSITIVE, NULL, NULL);
        NtCreateFile(&hDevice, SYNCHRONIZE|GENERIC_READ, &ObjA, &IoStatusBlock, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT | FILE_NO_INTERMEDIATE_BUFFERING);
        closeHandle = true;
        }

    memset(&readStruct, 0, sizeof(DVD_READ_STRUCTURE));
    memset(readStructureOutput, 0, sizeof(readStructureOutput));

    // try DVD Read Structure
    readStruct.Format = DvdPhysicalDescriptor;

    DWORD returned = 0;
    BOOL result = DeviceIoControl(hDevice, IOCTL_DVD_READ_STRUCTURE, &readStruct, sizeof(DVD_READ_STRUCTURE), readStructureOutput, sizeof(readStructureOutput), &returned, NULL);
    if(result)
        {
        if(closeHandle) NtClose(hDevice);
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
        if(closeHandle) NtClose(hDevice);
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
