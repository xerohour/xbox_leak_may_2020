/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    dvdread.cpp

Abstract:

    Low level disk test scanner.

Notes:
    dvd player code:
        "\xboxsrc\private\ui\dvd\library\hardware\drives\specific\xboxdvddrive.cpp"
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

    Hard Drive Partition Sizes
        \Device\Harddisk0\Partition0 19541088
        \Device\Harddisk0\Partition1  9997568
        \Device\Harddisk0\Partition2  1023840
        \Device\Harddisk0\Partition3  1535776
        \Device\Harddisk0\Partition4  1535776
        \Device\Harddisk0\Partition5  1535776

*****************************************************************************/

#include "dvdread.h"

HANDLE hDuke[XGetPortCount()] = { NULL, NULL, NULL, NULL };
HANDLE hSlot[XGetPortCount()*2] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

//
// config settings
//
DWORD avgTime;
DISK_GEOMETRY gDiskGeometry;
DWORD successfulReadSectors;
int enableRetries = 1;
DWORD spindleSpeed = 2;
DWORD speedDelay0to1 = 0;
DWORD speedDelay1to2 = 0;
DWORD speedDelay2to1 = 0;
DWORD speedDelay1to0 = 0;
DWORD speedDelay0to2 = 0;
DWORD speedDelay2to0 = 0;
DWORD delayOn062E = 300;
DWORD seekOn062E = 0;
DWORD summaryInternalErrs = 1;
ErrorDistrubition *errors = NULL;
DWORD lastReadTime;
DWORD avgTimeCount;
DWORD chunkSizeMin = 32;
DWORD chunkSizeMax = 32;
char deviceName[128];
DWORD readSize = 32;
DWORD startLBA=0, endLBA=0;
DWORD compareBuffers = 0;
DWORD sequentialReads = 1;
DWORD pauseOnMismatch = 0;
DWORD readMode = READMODE_DETECT;
DWORD stopAfter = 0;
DWORD numberOfReads = 0;
ExcludeList *exclude = NULL;
DWORD requestSense = 0;
DWORD launchGUI = 0;
DWORD pauseOnExit = 0;
DWORD avgSpan = 256;
DWORD defaultSpeed = 2;
DWORD seed = 1;
DWORD mediaType = MEDIA_UNKNOWN;

//
// stats
//
DWORD numFailThenFail = 0;
DWORD numFailThenFailSame = 0;
DWORD numFailThenPass = 0;
DWORD numPassThenFail = 0;
DWORD numBuffMisMatch = 0;
DWORD numTimesThrough = 1;
unsigned __int64 numBlocksRead = 0;
DWORD time = 0;
char model[64], serial[64], firmware[64];


void _cdecl main(void)
    {
    Initialize();

    //
    // cleanup old files
    //
    DeleteFile("T:\\dvdexit.txt");


    DebugPrint("DVD Read (Built on " __DATE__ " at " __TIME__ ")\n");
    DebugPrint("Copyright (C) Microsoft Corporation.  All rights reserved.\n\n");

    DWORD addDevice, removeDevice;
    DWORD dumpData = 0;
    DWORD status;
    READ_FUNC ReadBlocks;
    DWORD geometryIOCTL;
    DWORD geoSize;
    bool exit = false;
    DWORD rando = 0;
    ULONGLONG bigResult = 0;
    DWORD block = 0;
    SCSIError err1, err2, err3;
    DWORD numReads = 0;
    DWORD avgStartLBA = 0;
    unsigned numCommandErrors = 0;
    DWORD extraSpace = 0;

    memset(&gDiskGeometry, 0, sizeof(gDiskGeometry));
    strcpy(deviceName, "\\Device\\cdrom0");

    //
    // parse out any user settings
    //
    FILE *f = fopen("D:\\config.txt", "r");
    if(f)
        {
        char line[1024];
        char value[128];
        char variable[64];
        while(fgets(line, 1024, f))
            {
            if(sscanf(line, "%[^=]= %s", variable, value) == 2)
                {
                char *c = strchr(variable, ';');
                if(c) *c = '\0';
                c = strchr(variable, '[');
                if(c) *c = '\0';
                _strlwr(variable);
                if(strstr(variable, "device") != NULL) strcpy(deviceName, value);
                else if(strstr(variable, "startlba") != NULL) startLBA = GetNumber(value);
                else if(strstr(variable, "endlba") != NULL) endLBA = GetNumber(value);
                else if(strstr(variable, "defaultspeed") != NULL) defaultSpeed = atol(value);
                else if(strstr(variable, "enableretries") != NULL) enableRetries = atol(value);
                else if(strstr(variable, "readsizemin") != NULL) chunkSizeMin = GetNumber(value);
                else if(strstr(variable, "readsizemax") != NULL) chunkSizeMax = GetNumber(value);
                else if(strstr(variable, "readsize") != NULL) chunkSizeMax = chunkSizeMin = GetNumber(value);
                else if(strstr(variable, "comparebuffers") != NULL) compareBuffers = atol(value);
                else if(strstr(variable, "sequentialreads") != NULL) sequentialReads = atol(value);
                else if(strstr(variable, "stopafter") != NULL) stopAfter = atol(value);
                else if(strstr(variable, "readmode") != NULL) readMode = GetNumber(value);
                else if(strstr(variable, "dumpdata") != NULL) dumpData = atol(value);
                else if(strstr(variable, "avgspan") != NULL) avgSpan = atol(value);
                else if(strstr(variable, "requestsense") != NULL) requestSense = atol(value);
                else if(strstr(variable, "summaryinternalerrs") != NULL) summaryInternalErrs = atol(value);
                else if(strstr(variable, "pauseonexit") != NULL) pauseOnExit = GetNumber(value);
                else if(strstr(variable, "launchgui") != NULL) launchGUI = GetNumber(value);
                else if(strstr(variable, "speeddelay0to1") != NULL) speedDelay0to1 = atol(value);
                else if(strstr(variable, "speeddelay1to2") != NULL) speedDelay1to2 = atol(value);
                else if(strstr(variable, "speeddelay2to1") != NULL) speedDelay2to1 = atol(value);
                else if(strstr(variable, "speeddelay1to0") != NULL) speedDelay1to0 = atol(value);
                else if(strstr(variable, "speeddelay0to2") != NULL) speedDelay0to2 = atol(value);
                else if(strstr(variable, "speeddelay2to0") != NULL) speedDelay2to0 = atol(value);
                else if(strstr(variable, "delayon062e") != NULL) delayOn062E = atol(value);
                else if(strstr(variable, "seekon062e") != NULL) seekOn062E = atol(value);
                else if(strstr(variable, "numberofreads") != NULL) numberOfReads = GetNumber(value);
                else if(strstr(variable, "pauseonmismatch") != NULL) pauseOnMismatch = atol(value);
                else if(strstr(variable, "seed") != NULL) seed = GetNumber(value);
                else if(strstr(variable, "exclude") != NULL)
                    {
                    if(!exclude) exclude = new ExcludeList(value);
                    else exclude->Add(value);
                    }
                }
            }
        
        fclose(f);
        }

    //
    // seed random number generator
    //
    srand(seed);


    //
    // open the DVD / Hard drive
    //
    HANDLE hDevice;
    OBJECT_ATTRIBUTES ObjA;
    OBJECT_STRING VolumeString;
    IO_STATUS_BLOCK IoStatusBlock;
    RtlInitObjectString(&VolumeString, deviceName);
    InitializeObjectAttributes(&ObjA, &VolumeString, OBJ_CASE_INSENSITIVE, NULL, NULL);
    status = NtCreateFile(&hDevice, SYNCHRONIZE|GENERIC_READ, &ObjA, &IoStatusBlock, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT | FILE_NO_INTERMEDIATE_BUFFERING);
    if(!NT_SUCCESS(status))
        {
        DebugPrint("\n\n\nUnable to open the device '%s' (ec: %u)\n", deviceName, RtlNtStatusToDosError(status));
        DebugPrint("Rebooting...\n\n\n");
        Sleep(1000);
        XLaunchNewImage(NULL, NULL);
        }

    _strlwr(deviceName);

    if(strstr(deviceName, "cdrom") != NULL) // target == DVD or CD
        {
        mediaType = GetDiskType(hDevice);
        if(readMode == READMODE_DETECT)
            {
            if(mediaType == MEDIA_CDDA) readMode = READMODE_CD;
            else readMode = READMODE_DVD;
            }

        if(readMode == READMODE_DVD)
            {
            ReadBlocks = ReadDVDBlocks;
            ChangeSpeed(hDevice, defaultSpeed);
            }
        else if(readMode == READMODE_CD)
            {
            ReadBlocks = ReadCDBlocks2;
            extraSpace = 304; // we read in chunks of an audio frame size not clustor size
            }
        else if(readMode == READMODE_HD)
            {
            ReadBlocks = ReadHDBlocks;
            }

        geometryIOCTL = IOCTL_CDROM_GET_DRIVE_GEOMETRY;
	    DeviceIoControl(hDevice, geometryIOCTL, NULL, 0, &gDiskGeometry, sizeof(gDiskGeometry), &geoSize, NULL);
        }
    else // target == hard disk
        {
        mediaType = MEDIA_HD;
        readMode = READMODE_HD;
        ReadBlocks = ReadHDBlocks;
        geometryIOCTL = IOCTL_DISK_GET_DRIVE_GEOMETRY;
	    DeviceIoControl(hDevice, geometryIOCTL, NULL, 0, &gDiskGeometry, sizeof(gDiskGeometry), &geoSize, NULL);

        // gDiskGeometry.Cylinders.QuadPart contains the size of the entire 
        // disk but we only want the size of the current partition so we 
        // need to query the size of the volume and adjust the value
        FILE_FS_SIZE_INFORMATION NormalSizeInfo;
        memset(&NormalSizeInfo, 0, sizeof(NormalSizeInfo));
        NtQueryVolumeInformationFile(
                    hDevice,
                    &IoStatusBlock,
                    &NormalSizeInfo,
                    sizeof(NormalSizeInfo),
                    FileFsSizeInformation
                    );
        gDiskGeometry.Cylinders.QuadPart = NormalSizeInfo.TotalAllocationUnits.QuadPart * NormalSizeInfo.SectorsPerAllocationUnit;
        }
    if(endLBA == 0) endLBA = (DWORD)gDiskGeometry.Cylinders.QuadPart;


    if(launchGUI)
        {
        GUI(true);
        }

    //
    // print out the drive info, disk geometry, and config settings
    //
    memset(model, 0, 64);
    memset(serial, 0, 64);
    memset(firmware, 0, 64);
    GetDriveID(hDevice, geometryIOCTL == IOCTL_CDROM_GET_DRIVE_GEOMETRY, model, serial, firmware);
	DebugPrint("DRIVE: Model:             %s\n", model);
	DebugPrint("DRIVE: Serial:            %s\n", serial);
	DebugPrint("DRIVE: Firmware:          %s\n", firmware);
	DebugPrint("MEDIA: Type:              %s\n", mediaTypeNames[mediaType]);
	DebugPrint("MEDIA: Cylinders:         %u\n", gDiskGeometry.Cylinders.QuadPart);
	DebugPrint("MEDIA: TracksPerCylinder: %u\n", gDiskGeometry.TracksPerCylinder);
	DebugPrint("MEDIA: SectorsPerTrack:   %u\n", gDiskGeometry.SectorsPerTrack);
	DebugPrint("MEDIA: BytesPerSector:    %u\n", gDiskGeometry.BytesPerSector);
	DebugPrint("\n");
    DebugPrint("CONFIG: Device:           %s\n", deviceName);
    DebugPrint("CONFIG: ReadSizeMin:      %u\n", chunkSizeMin);
    DebugPrint("CONFIG: ReadSizeMax:      %u\n", chunkSizeMax);
    DebugPrint("CONFIG: StartLBA:         %u\n", startLBA);
    DebugPrint("CONFIG: EndLBA:           %u\n", endLBA);
    DebugPrint("CONFIG: ReadMode:         %u\n", readMode);
    DebugPrint("CONFIG: NumberOfReads:    %u\n", numberOfReads);
    DebugPrint("CONFIG: SequentialReads:  %u\n", sequentialReads);
    DebugPrint("CONFIG: SummaryInternalErrs: %u\n", summaryInternalErrs);
    DebugPrint("CONFIG: CompareBuffers:   %u\n", compareBuffers);
    DebugPrint("CONFIG: PauseOnMismatch:  %u\n", pauseOnMismatch);
	DebugPrint("\n");
    DebugPrint("CONFIG: RequestSense:     %u\n", requestSense);
    DebugPrint("CONFIG: EnableRetries:    %u\n", enableRetries);
    DebugPrint("CONFIG: DefaultSpeed:     %u\n", defaultSpeed);
    DebugPrint("CONFIG: SpeedDelay0to1:   %u\n", speedDelay0to1);
    DebugPrint("CONFIG: SpeedDelay1to2:   %u\n", speedDelay1to2);
    DebugPrint("CONFIG: SpeedDelay2to1:   %u\n", speedDelay2to1);
    DebugPrint("CONFIG: SpeedDelay1to0:   %u\n", speedDelay1to0);
    DebugPrint("CONFIG: SpeedDelay0to2:   %u\n", speedDelay0to2);
    DebugPrint("CONFIG: SpeedDelay2to0:   %u\n", speedDelay2to0);
    DebugPrint("CONFIG: DelayOn062E:      %u\n", delayOn062E);
    DebugPrint("CONFIG: SeekOn062E :      %u\n", seekOn062E );
    DebugPrint("\n");


    //
    // allocate buffer space
    //
    unsigned char *buff1 = new unsigned char[chunkSizeMax * (gDiskGeometry.BytesPerSector+extraSpace)];
    unsigned char *buff2 = new unsigned char[chunkSizeMax * (gDiskGeometry.BytesPerSector+extraSpace)];
    unsigned char *buff3 = new unsigned char[chunkSizeMax * (gDiskGeometry.BytesPerSector+extraSpace)];
    if(!buff1 || !buff2 || !buff3)
        {
        DebugPrint("\n\n\nUnable to allocate enough space (%u)\n", chunkSizeMax * gDiskGeometry.BytesPerSector * 3);
        DebugPrint("Lower the 'ReadSize=' value in the config.txt file\n");
        DebugPrint("Rebooting...\n\n\n");
        Sleep(1000);
        XLaunchNewImage(NULL, NULL);
        return;
        }

    time = GetTickCount();

    //
    // run the test
    //
    avgStartLBA = block = startLBA;
    while(!exit)
        {
        XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDevice, &removeDevice);
        InputDukeInsertions(addDevice, removeDevice);
        XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &addDevice, &removeDevice);
        InputMUInsertions(addDevice, removeDevice);
        UpdateGraphics(DISPLAYMODE_RUN_TIME);

        memset(buff1, 0, chunkSizeMax * gDiskGeometry.BytesPerSector);
        memset(buff2, 0, chunkSizeMax * gDiskGeometry.BytesPerSector);
        memset(buff3, 0, chunkSizeMax * gDiskGeometry.BytesPerSector);

        //
        // calculate the size of the next read
        //
        rando = rand();
        bigResult = (ULONGLONG)(rando*(ULONGLONG)(chunkSizeMax-chunkSizeMin));
        readSize = (DWORD)((bigResult/RAND_MAX) + chunkSizeMin);

        ++numReads;

        //
        // read from the disk
        //
        err1 = ReadBlocks(hDevice, block, readSize, buff1);
        numBlocksRead += readSize;
        if(requestSense == 2 && err1.win32 == 0 && geometryIOCTL == IOCTL_CDROM_GET_DRIVE_GEOMETRY && readMode == READMODE_DVD) RequestSense(hDevice, err1);
        if(dumpData)
            {
            DebugPrint("Block %u: ", block);
            err1.Print();
            DebugPrint("\n");
            HexDump(buff1, readSize * gDiskGeometry.BytesPerSector);
            goto EOErrorCheck;
            }
        if(numberOfReads == 1) goto EOErrorCheck;

        err2 = ReadBlocks(hDevice, block, readSize, buff2);
        numBlocksRead += readSize;
        if(requestSense == 2 && err2.win32 == 0 && geometryIOCTL == IOCTL_CDROM_GET_DRIVE_GEOMETRY && readMode == READMODE_DVD) RequestSense(hDevice, err2);

        //
        // check for errors
        //
        if(err1.win32 != 0 && err2.win32 == 0)
            {
            numCommandErrors = 0;
            if(!errors) errors = new ErrorDistrubition(err1);
            else errors->AddError(err1);
            ++numFailThenPass;
            DebugPrint("Block %u: Read1 failed ", block);
            err1.Print();
            DebugPrint(" Read2 succeeded ");
            err2.Print();
            DebugPrint("\n");
            }
        else if(err1.win32 == 0 && err2.win32 != 0)
            {
            numCommandErrors = 0;
            if(!errors) errors = new ErrorDistrubition(err2);
            else errors->AddError(err2);
            ++numPassThenFail;
            DebugPrint("Block %u: Read1 succeeded ", block);
            err1.Print();
            DebugPrint(" Read2 failed ");
            err2.Print();
            DebugPrint("\n");
            }
        else if((err1.sense.ErrorCode                    != err2.sense.ErrorCode)           ||
                (err1.sense.Valid                        != err2.sense.Valid)               ||
                (err1.sense.SenseKey                     != err2.sense.SenseKey)            ||
                (err1.sense.Reserved                     != err2.sense.Reserved)            ||
                (err1.sense.IncorrectLength              != err2.sense.IncorrectLength)     ||
                (err1.sense.EndOfMedia                   != err2.sense.EndOfMedia)          ||
                (err1.sense.FileMark                     != err2.sense.FileMark)            ||
                (err1.sense.AdditionalSenseCode          != err2.sense.AdditionalSenseCode) ||
                (err1.sense.AdditionalSenseCodeQualifier != err2.sense.AdditionalSenseCodeQualifier))
            {
            numCommandErrors = 0;
            if(!errors) errors = new ErrorDistrubition(err1);
            else errors->AddError(err1);
            if(!errors) errors = new ErrorDistrubition(err2);
            else errors->AddError(err2);
            ++numFailThenFail;
            DebugPrint("Block %u: Err1", block);
            err1.Print();
            DebugPrint(" != Err2");
            err2.Print();
            DebugPrint("\n");
            }

        // errors are the same at this point
        else if(err1.win32!=0 || err2.win32!=0)
            {
            if(!errors) errors = new ErrorDistrubition(err1);
            else errors->AddError(err1);
            errors->AddError(err2);
            ++numFailThenFailSame;
            DebugPrint("Block %u: Same Error Err1/2", block);
            err1.Print();
            DebugPrint("\n");
            if(err1.sense.SenseKey == 0x05 && err1.sense.AdditionalSenseCode == 0x81) // command error
                {
                numCommandErrors += readSize;
                if(numCommandErrors >= 4096)
                    {
                    numCommandErrors = 0;
                    numFailThenFailSame -= (4096/readSize);
                    errors->RemoveError(err1, (4096/readSize) * 2);
                    DebugPrint("End of security section.\n");
                    }
                }
            }
        else if(err1.win32==0 && err2.win32==0 && (memcmp(buff1, buff2, readSize*gDiskGeometry.BytesPerSector) != 0))
            {
            numCommandErrors = 0;
            if(requestSense == 1 && readMode == READMODE_DVD) RequestSense(hDevice, err2);
            if(pauseOnMismatch) Pause();
            int retry = 3;
            do {
                // do another read for reference
                err3 = ReadBlocks(hDevice, block, readSize, buff3);
                retry--;
                } while(err3.win32 != 0 && retry >= 0);

            numBlocksRead += readSize;

            ++numBuffMisMatch;
            DebugPrint("Block %u: buff1 != buff2, Err1", block);
            err1.Print();
            DebugPrint(", Err2");
            err2.Print();
            DebugPrint("\n");
            if(compareBuffers) CompareBuff(buff1, buff2, buff3, readSize*gDiskGeometry.BytesPerSector);
            }

        EOErrorCheck:

        if(numberOfReads == 1)
            {
            if(err1.win32!=0)
                {
                if(!errors) errors = new ErrorDistrubition(err1);
                else errors->AddError(err1);
                ++numFailThenFailSame;
                DebugPrint("Block %u: Single Error ", block);
                err1.Print();
                DebugPrint("\n");
                if(err1.sense.SenseKey == 0x05 && err1.sense.AdditionalSenseCode == 0x81) // command error
                    {
                    numCommandErrors += readSize;
                    if(numCommandErrors >= 4096)
                        {
                        numCommandErrors = 0;
                        numFailThenFailSame -= (4096/readSize);
                        errors->RemoveError(err1, (4096/readSize) * 2);
                        DebugPrint("End of security section.\n");
                        }
                    }
                }
            }


        //
        // adjust the read pointer (skipping any exclude areas)
        //
        do {
            if(sequentialReads)
                {
                block += readSize;
                if(block >= endLBA)
                    {
                    block = startLBA;
                    ++numTimesThrough;
                    if(stopAfter && numTimesThrough >= stopAfter) 
                        {
                        exit = true;
                        break;
                        }
                    }
                }
            else
                {
                rando = rand();
                bigResult = (ULONGLONG)(rando*(ULONGLONG)(endLBA-startLBA));
                block = (DWORD)((bigResult/RAND_MAX) + startLBA);
                }
            } while((exclude && exclude->InList(block, chunkSizeMax)) || (block+chunkSizeMax >= endLBA));


        //
        // avg read time
        //
        if(avgSpan && (numReads % avgSpan == 0))
            {
            DWORD avg = 0;
            if(avgTimeCount != 0)
                avg = avgTime/avgTimeCount;

            if(sequentialReads)
                DebugPrint("Blocks %u-%u: avg Read Time == %u ms\n", avgStartLBA, block, avg);
            else
                DebugPrint("Read %u-%u: avg Read Time == %u ms\n", numReads-avgSpan, numReads, avg);

            avgTimeCount = 0;
            avgTime = 0;
            avgStartLBA = block;
            }

        if(InputCheckDigitalButton(XINPUT_GAMEPAD_START)) GUI(false);
        if(InputCheckExitCombo()) exit = true;
        } // while


    //
    // dump the stats
    //
    DWORD finalTime = (GetTickCount() - time)/1000;
    DWORD totalErrors = numFailThenFailSame*2 + numFailThenFail*2 + numFailThenPass + numPassThenFail;

    DebugPrint("****Program Stats:\n");
    DebugPrint("*** %9u sec run time\n", finalTime);
    DebugPrint("*** %9I64u Total blocks read\n", numBlocksRead);
    DebugPrint("****\n");
    if(numberOfReads == 1)
        DebugPrint("*** %9u Single Failures\n", numFailThenFailSame);
    else
        {
        DebugPrint("*** %9u Buffer mismatches (no failure reported)\n", numBuffMisMatch);
        DebugPrint("*** %9u Failures then different failures\n", numFailThenFail);
        DebugPrint("*** %9u Failures then same failures\n", numFailThenFailSame);
        DebugPrint("*** %9u Failures then successes\n", numFailThenPass);
        DebugPrint("*** %9u Successes then failures\n", numPassThenFail);
        }
    DebugPrint("*** %9u Total Reported Errors\n", totalErrors);
    DebugPrint("****\n");
    DebugPrint("*** %9u errors / reads\n", numReads? totalErrors / numReads : 0);
    DebugPrint("*** %9u errors / sec\n", finalTime? totalErrors / finalTime : 0);
    if(errors)
        {
        DebugPrint("****Error Distribution:\n");
        errors->PrintErrors();
        }

    if(pauseOnExit) Pause();

    XLaunchNewImage(NULL, NULL);
    }



void Initialize(void)
    {
    DWORD error;

    //
    // Misc
    //
    DWORD dataType = 0xCDCDCDCD;
    LAUNCH_DATA launchInfo;
    error = XGetLaunchInfo(&dataType, &launchInfo);

    //
    // USB
    //
    XInitDevices(0, NULL);

    //
    // Network
    //
    /*
    XNetStartupParams xnsp = { sizeof(XNetStartupParams), XNET_STARTUP_BYPASS_SECURITY };
    error = XNetStartup(&xnsp);
    if(error != NO_ERROR)
        {
        DebugPrint("XNetStartup Error (ec: %ld)\n", error);
        }
    Sleep(5000); // needed cause XNetStartup isnt fully started when call exits
    */

    //
    //
    //
    InitGraphics();
    }


