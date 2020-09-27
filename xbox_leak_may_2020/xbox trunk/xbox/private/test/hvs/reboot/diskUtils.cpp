/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    diskUtils.cpp

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

#include "reboot.h"

/*
APIs in this file:

    void GetDriveID(HANDLE hDevice, bool cdrom, char* model, char* serial, char* firmware);
    bool CDTestReady(HANDLE hDevice);
    DWORD GetDiskType(HANDLE hDevice);

*/







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
