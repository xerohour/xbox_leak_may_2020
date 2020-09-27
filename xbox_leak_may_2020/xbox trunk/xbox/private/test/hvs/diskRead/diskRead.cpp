/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    diskRead.cpp

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
        \Device\Harddisk0\Partition1  9997568   C:
        \Device\Harddisk0\Partition2  1023840   Y:
        \Device\Harddisk0\Partition3  1535776   Z:
        \Device\Harddisk0\Partition4  1535776   Z:
        \Device\Harddisk0\Partition5  1535776   Z:

*****************************************************************************/

#include "diskRead.h"
#include <xbeimage.h>
#define XeImageHeader() ((struct _XBEIMAGE_HEADER*)XBEIMAGE_STANDARD_BASE_ADDRESS)

HANDLE hDuke[XGetPortCount()] = { NULL, NULL, NULL, NULL };
HANDLE hSlot[XGetPortCount()*2] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

//
// config settings
//
ConfigSettings globalSettings;

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

ErrorDistrubition *errors = NULL;
DWORD avgTimeCount;
DWORD avgTime;
DWORD lastReadTime;
DWORD spindleSpeed;
DWORD successfulReadSectors;

void ParseTestConfig(char *variable, char *value, void *userParam)
    {
    ConfigSettings *settings = (ConfigSettings*)userParam;

    if(strstr(variable, "device") != NULL) strcpy(settings->deviceName, value);
    else if(strstr(variable, "startlba") != NULL) settings->startLBA = GetNumber(value);
    else if(strstr(variable, "endlba") != NULL) settings->endLBA = GetNumber(value);
    else if(strstr(variable, "defaultspeed") != NULL) settings->defaultSpeed = atol(value);
    else if(strstr(variable, "enableretries") != NULL) settings->enableRetries = atol(value);
    else if(strstr(variable, "readsizemin") != NULL) settings->chunkSizeMin = GetNumber(value);
    else if(strstr(variable, "readsizemax") != NULL) settings->chunkSizeMax = GetNumber(value);
    else if(strstr(variable, "readsize") != NULL) settings->chunkSizeMax = settings->chunkSizeMin = GetNumber(value);
    else if(strstr(variable, "comparebuffers") != NULL) settings->compareBuffers = atol(value);
    else if(strstr(variable, "sequentialreads") != NULL) settings->sequentialReads = atol(value);
    else if(strstr(variable, "readmode") != NULL) settings->readMode = GetNumber(value);
    else if(strstr(variable, "dumpdata") != NULL) settings->dumpData = atol(value);
    else if(strstr(variable, "avgspan") != NULL) settings->avgSpan = atol(value);
    else if(strstr(variable, "requestsense") != NULL) settings->requestSense = atol(value);
    else if(strstr(variable, "summaryinternalerrs") != NULL) settings->summaryInternalErrs = atol(value);
    else if(strstr(variable, "pauseonexit") != NULL) settings->pauseOnExit = GetNumber(value);
    else if(strstr(variable, "speeddelay0to1") != NULL) settings->speedDelay0to1 = atol(value);
    else if(strstr(variable, "speeddelay1to2") != NULL) settings->speedDelay1to2 = atol(value);
    else if(strstr(variable, "speeddelay2to1") != NULL) settings->speedDelay2to1 = atol(value);
    else if(strstr(variable, "speeddelay1to0") != NULL) settings->speedDelay1to0 = atol(value);
    else if(strstr(variable, "speeddelay0to2") != NULL) settings->speedDelay0to2 = atol(value);
    else if(strstr(variable, "speeddelay2to0") != NULL) settings->speedDelay2to0 = atol(value);
    else if(strstr(variable, "delayon062e") != NULL) settings->delayOn062E = atol(value);
    else if(strstr(variable, "seekon062e") != NULL) settings->seekOn062E = atol(value);
    else if(strstr(variable, "numberofreads") != NULL) settings->numberOfReads = GetNumber(value);
    else if(strstr(variable, "pauseonmismatch") != NULL) settings->pauseOnMismatch = atol(value);
    else if(strstr(variable, "pauseonerror") != NULL) settings->pauseOnError = atol(value);
    else if(strstr(variable, "seed") != NULL) settings->seed = GetNumber(value);
    else if(strstr(variable, "exclude") != NULL)
        {
        if(!settings->exclude) settings->exclude = new ExcludeList(value);
        else settings->exclude->Add(value);
        }
    }

#pragma warning(disable : 4326) // return type of 'main' should be 'int or void' instead of 'ShutDown'
struct ShutDown
    {
    ShutDown(LAUNCH_DATA &launchInfo, unsigned __int64 pass, unsigned __int64 fail, char *format, ...)
        {
        ToLauncherData* outParams = (ToLauncherData*)&launchInfo;
        ToTestData* initialParams = (ToTestData*)&launchInfo;

        if(initialParams->titleID == HVSLAUNCHERID)
            {
            outParams->titleID = XeImageHeader()->Certificate->TitleID;
            outParams->numPass = pass;
            outParams->numFail = fail;
            va_list args;
            va_start(args, format);
            vsprintf(outParams->notes, format, args);
            va_end(args);

            LaunchImage("D:\\default.xbe", &launchInfo);
            }
        else
            XLaunchNewImage(NULL, NULL);
        }
    };

ShutDown _cdecl main(void)
    {
    //
    // Get the launch data (if any)
    //
    DWORD dataType = 0xCDCDCDCD;
    LAUNCH_DATA launchInfo;
    XGetLaunchInfo(&dataType, &launchInfo);
    ToTestData* initialParams = (ToTestData*)&launchInfo;

    Initialize();

    //
    // cleanup old files
    //
    DeleteFile("T:\\testexit.txt");

    DWORD addDevice, removeDevice;
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
    char *iniConfig = NULL;



    //
    // Grab the startup parameters
    //
    if(initialParams->titleID == HVSLAUNCHERID)
        {
        iniConfig = new char[strlen(initialParams->configSettings)+1];
        strcpy(iniConfig, initialParams->configSettings);
        ParseConfigSettings(initialParams->configSettings, &globalSettings, ParseTestConfig, &globalSettings);
        }
    else
        {
        FILE *file = fopen("D:\\diskRead.ini", "rb");
        if(file)
            {
            DWORD filesize = _filelength(_fileno(file));
            iniConfig = new char[filesize+1];
            unsigned bytesRead = fread(iniConfig, 1, filesize, file);
            iniConfig[bytesRead] = '\0';

            ParseConfigSettings(iniConfig, &globalSettings, ParseTestConfig, &globalSettings);
            if(fseek(file, 0, SEEK_SET) == 0)
                {
                bytesRead = fread(iniConfig, 1, filesize, file);
                iniConfig[bytesRead] = '\0';
                }
            fclose(file);
            }
        }


    // Call SetSaveName BEFORE calling LogPrint or LogFlush
    SetSaveName(globalSettings.saveName, "diskRead.log");
    LogFlush();

    if(initialParams->titleID == HVSLAUNCHERID)
        LogPrint("Launched from HVS\n  Test %u of %u\n\n", initialParams->testNumber, initialParams->totalTests);
    else
        LogPrint("Running in stand a lone mode\n\n");

    LogPrint("DISK Read (Built on " __DATE__ " at " __TIME__ ")\n");
    LogPrint("Copyright (C) Microsoft Corporation.  All rights reserved.\n\n");


    //
    // seed random number generator
    //
    srand(globalSettings.seed);


    //
    // open the DVD / Hard drive
    //
    HANDLE hDevice;
    OBJECT_ATTRIBUTES ObjA;
    OBJECT_STRING VolumeString;
    IO_STATUS_BLOCK IoStatusBlock;
    RtlInitObjectString(&VolumeString, globalSettings.deviceName);
    InitializeObjectAttributes(&ObjA, &VolumeString, OBJ_CASE_INSENSITIVE, NULL, NULL);
    status = NtCreateFile(&hDevice, SYNCHRONIZE|GENERIC_READ, &ObjA, &IoStatusBlock, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT | FILE_NO_INTERMEDIATE_BUFFERING);
    if(!NT_SUCCESS(status))
        {
        LogPrint("Unable to open the device '%s' (ec: %u)\n", globalSettings.deviceName, RtlNtStatusToDosError(status));
        DebugPrint("Rebooting...\n\n\n");
        return ShutDown(launchInfo, 0, 0, "Unable to open the device:\\n '%s' (ec: %u)", globalSettings.deviceName, RtlNtStatusToDosError(status));
        }

    _strlwr(globalSettings.deviceName);

    if(strstr(globalSettings.deviceName, "cdrom") != NULL) // target == DVD or CD
        {
        globalSettings.mediaType = GetDiskType(hDevice);
        if(globalSettings.readMode == READMODE_DETECT)
            {
            if(globalSettings.mediaType == MEDIA_CDDA) globalSettings.readMode = READMODE_CD;
            else globalSettings.readMode = READMODE_DVD;
            }

        if(globalSettings.readMode == READMODE_DVD)
            {
            ReadBlocks = ReadDVDBlocks;
            ChangeSpeed(hDevice, globalSettings.defaultSpeed);
            }
        else if(globalSettings.readMode == READMODE_CD)
            {
            ReadBlocks = ReadCDBlocks2;
            extraSpace = 304; // we read in chunks of an audio frame size not clustor size
            }
        else if(globalSettings.readMode == READMODE_HD)
            {
            ReadBlocks = ReadHDBlocks;
            }

        geometryIOCTL = IOCTL_CDROM_GET_DRIVE_GEOMETRY;
	    DeviceIoControl(hDevice, geometryIOCTL, NULL, 0, &globalSettings.gDiskGeometry, sizeof(globalSettings.gDiskGeometry), &geoSize, NULL);
        }
    else // target == hard disk
        {
        globalSettings.mediaType = MEDIA_HD;
        globalSettings.readMode = READMODE_HD;
        ReadBlocks = ReadHDBlocks;
        geometryIOCTL = IOCTL_DISK_GET_DRIVE_GEOMETRY;
	    DeviceIoControl(hDevice, geometryIOCTL, NULL, 0, &globalSettings.gDiskGeometry, sizeof(globalSettings.gDiskGeometry), &geoSize, NULL);

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
        globalSettings.gDiskGeometry.Cylinders.QuadPart = NormalSizeInfo.TotalAllocationUnits.QuadPart * NormalSizeInfo.SectorsPerAllocationUnit;
        }
    if(globalSettings.gDiskGeometry.Cylinders.QuadPart == 0)
        {
        NtClose(hDevice);
        LogPrint("No Media Detected\n");
        DebugPrint("Rebooting...\n\n\n");
        return ShutDown(launchInfo, 0, 0, "No Media Detected");
        }
    if(globalSettings.endLBA == 0) globalSettings.endLBA = (DWORD)globalSettings.gDiskGeometry.Cylinders.QuadPart;
    spindleSpeed = globalSettings.defaultSpeed;


    //
    // print out the drive info, disk geometry, and config settings
    //
    memset(model, 0, 64);
    memset(serial, 0, 64);
    memset(firmware, 0, 64);
    GetDriveID(hDevice, geometryIOCTL == IOCTL_CDROM_GET_DRIVE_GEOMETRY, model, serial, firmware);

	LogPrint("DRIVE: Model:             %s\n", model);
	LogPrint("DRIVE: Serial:            %s\n", serial);
	LogPrint("DRIVE: Firmware:          %s\n", firmware);
	LogPrint("MEDIA: Type:              %s\n", mediaTypeNames[globalSettings.mediaType]);
	LogPrint("MEDIA: Cylinders:         %u\n", globalSettings.gDiskGeometry.Cylinders.QuadPart);
	LogPrint("MEDIA: TracksPerCylinder: %u\n", globalSettings.gDiskGeometry.TracksPerCylinder);
	LogPrint("MEDIA: SectorsPerTrack:   %u\n", globalSettings.gDiskGeometry.SectorsPerTrack);
	LogPrint("MEDIA: BytesPerSector:    %u\n", globalSettings.gDiskGeometry.BytesPerSector);

    LogPrint("Config Settings:\n");
    LogPrint("  ResultsServer: %s\n", globalSettings.resultsServer);
    LogPrint("  SaveName:      %s\n", globalSettings.saveName);
    LogPrint("  StopAfter:     %u %s\n", globalSettings.stopAfter.duration, globalSettings.stopAfter.type==STOPAFTER_MINUTES?"min":(globalSettings.stopAfter.type==STOPAFTER_HOURS?"hours":"iterations"));
	LogPrint("\n");
    LogPrint("  Device:              %s\n", globalSettings.deviceName);
    LogPrint("  ReadSizeMin:         %u\n", globalSettings.chunkSizeMin);
    LogPrint("  ReadSizeMax:         %u\n", globalSettings.chunkSizeMax);
    LogPrint("  StartLBA:            %u\n", globalSettings.startLBA);
    LogPrint("  EndLBA:              %u\n", globalSettings.endLBA);
    LogPrint("  ReadMode:            %u\n", globalSettings.readMode);
    LogPrint("  NumberOfReads:       %u\n", globalSettings.numberOfReads);
    LogPrint("  SequentialReads:     %u\n", globalSettings.sequentialReads);
    LogPrint("  SummaryInternalErrs: %u\n", globalSettings.summaryInternalErrs);
    LogPrint("  CompareBuffers:      %u\n", globalSettings.compareBuffers);
    LogPrint("  PauseOnMismatch:     %u\n", globalSettings.pauseOnMismatch);
	LogPrint("\n");
    LogPrint("  RequestSense:        %u\n", globalSettings.requestSense);
    LogPrint("  EnableRetries:       %u\n", globalSettings.enableRetries);
    LogPrint("  DefaultSpeed:        %u\n", globalSettings.defaultSpeed);
    LogPrint("  SpeedDelay0to1:      %u\n", globalSettings.speedDelay0to1);
    LogPrint("  SpeedDelay1to2:      %u\n", globalSettings.speedDelay1to2);
    LogPrint("  SpeedDelay2to1:      %u\n", globalSettings.speedDelay2to1);
    LogPrint("  SpeedDelay1to0:      %u\n", globalSettings.speedDelay1to0);
    LogPrint("  SpeedDelay0to2:      %u\n", globalSettings.speedDelay0to2);
    LogPrint("  SpeedDelay2to0:      %u\n", globalSettings.speedDelay2to0);
    LogPrint("  DelayOn062E:         %u\n", globalSettings.delayOn062E);
    LogPrint("  SeekOn062E :         %u\n", globalSettings.seekOn062E );
    LogPrint("\n");

    //
    // allocate buffer space
    //
    unsigned char *buff1 = new unsigned char[globalSettings.chunkSizeMax * (globalSettings.gDiskGeometry.BytesPerSector+extraSpace)];
    unsigned char *buff2 = new unsigned char[globalSettings.chunkSizeMax * (globalSettings.gDiskGeometry.BytesPerSector+extraSpace)];
    unsigned char *buff3 = new unsigned char[globalSettings.chunkSizeMax * (globalSettings.gDiskGeometry.BytesPerSector+extraSpace)];
    if(!buff1 || !buff2 || !buff3)
        {
        LogPrint("Unable to allocate enough space (%u)\n", (globalSettings.chunkSizeMax * globalSettings.gDiskGeometry.BytesPerSector+extraSpace) * 3);
        LogPrint("Try lowering chunkSizeMin and chunkSizeMax\n");
        DebugPrint("Rebooting...\n\n\n");
        return ShutDown(launchInfo, 0, 0, "Unable to allocate enough space (%u).\\nTry lowering chunkSizeMin and chunkSizeMax", globalSettings.chunkSizeMax * globalSettings.gDiskGeometry.BytesPerSector * 3);
        }

    time = GetTickCount();

    //
    // run the test
    //
    avgStartLBA = block = globalSettings.startLBA;
    while(!exit)
        {
        XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDevice, &removeDevice);
        InputDukeInsertions(addDevice, removeDevice);
        XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &addDevice, &removeDevice);
        InputMUInsertions(addDevice, removeDevice);
        UpdateGraphics(DISPLAYMODE_RUN_TIME);

        memset(buff1, 0, globalSettings.chunkSizeMax * globalSettings.gDiskGeometry.BytesPerSector);
        memset(buff2, 0, globalSettings.chunkSizeMax * globalSettings.gDiskGeometry.BytesPerSector);
        memset(buff3, 0, globalSettings.chunkSizeMax * globalSettings.gDiskGeometry.BytesPerSector);

        //
        // calculate the size of the next read
        //
        rando = rand();
        bigResult = (ULONGLONG)(rando*(ULONGLONG)(globalSettings.chunkSizeMax-globalSettings.chunkSizeMin));
        globalSettings.readSize = (DWORD)((bigResult/RAND_MAX) + globalSettings.chunkSizeMin);

        ++numReads;

        //
        // read from the disk
        //
        err1 = ReadBlocks(hDevice, block, globalSettings.readSize, buff1);
        numBlocksRead += globalSettings.readSize;
        if(globalSettings.pauseOnError && err1.win32 != 0) Pause();
        if(globalSettings.requestSense == 2 && err1.win32 == 0 && geometryIOCTL == IOCTL_CDROM_GET_DRIVE_GEOMETRY && globalSettings.readMode == READMODE_DVD) RequestSense(hDevice, err1);
        if(globalSettings.dumpData)
            {
            LogPrint("Block %u: ", block);
            err1.Print();
            LogPrint("\n");
            HexDump(buff1,globalSettings. readSize * globalSettings.gDiskGeometry.BytesPerSector);
            goto EOErrorCheck;
            }
        if(globalSettings.numberOfReads == 1) goto EOErrorCheck;

        err2 = ReadBlocks(hDevice, block, globalSettings.readSize, buff2);
        numBlocksRead += globalSettings.readSize;
        if(globalSettings.pauseOnError && err1.win32 != 0) Pause();
        if(globalSettings.requestSense == 2 && err2.win32 == 0 && geometryIOCTL == IOCTL_CDROM_GET_DRIVE_GEOMETRY && globalSettings.readMode == READMODE_DVD) RequestSense(hDevice, err2);

        //
        // check for errors
        //
        if(err1.win32 != 0 && err2.win32 == 0)
            {
            numCommandErrors = 0;
            if(!errors) errors = new ErrorDistrubition(err1);
            else errors->AddError(err1);
            ++numFailThenPass;
            LogPrint("Block %u: Read1 failed ", block);
            err1.Print();
            LogPrint(" Read2 succeeded ");
            err2.Print();
            LogPrint("\n");
            }
        else if(err1.win32 == 0 && err2.win32 != 0)
            {
            numCommandErrors = 0;
            if(!errors) errors = new ErrorDistrubition(err2);
            else errors->AddError(err2);
            ++numPassThenFail;
            LogPrint("Block %u: Read1 succeeded ", block);
            err1.Print();
            LogPrint(" Read2 failed ");
            err2.Print();
            LogPrint("\n");
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
            LogPrint("Block %u: Err1", block);
            err1.Print();
            LogPrint(" != Err2");
            err2.Print();
            LogPrint("\n");
            }

        // errors are the same at this point
        else if(err1.win32!=0 || err2.win32!=0)
            {
            if(!errors) errors = new ErrorDistrubition(err1);
            else errors->AddError(err1);
            errors->AddError(err2);
            ++numFailThenFailSame;
            LogPrint("Block %u: Same Error Err1/2", block);
            err1.Print();
            LogPrint("\n");
            if(err1.sense.SenseKey == 0x05 && err1.sense.AdditionalSenseCode == 0x81) // command error
                {
                numCommandErrors += globalSettings.readSize;
                if(numCommandErrors >= 4096)
                    {
                    numCommandErrors = 0;
                    numFailThenFailSame -= (4096/globalSettings.readSize);
                    errors->RemoveError(err1, (4096/globalSettings.readSize) * 2);
                    LogPrint("End of security section.\n");
                    }
                }
            }
        else if(err1.win32==0 && err2.win32==0 && (memcmp(buff1, buff2, globalSettings.readSize*globalSettings.gDiskGeometry.BytesPerSector) != 0))
            {
            numCommandErrors = 0;
            if(globalSettings.requestSense == 1 && globalSettings.readMode == READMODE_DVD) RequestSense(hDevice, err2);
            if(globalSettings.pauseOnMismatch) Pause();
            int retry = 3;
            do {
                // do another read for reference
                err3 = ReadBlocks(hDevice, block, globalSettings.readSize, buff3);
                retry--;
                } while(err3.win32 != 0 && retry >= 0);

            numBlocksRead += globalSettings.readSize;

            ++numBuffMisMatch;
            LogPrint("Block %u: buff1 != buff2, Err1", block);
            err1.Print();
            LogPrint(", Err2");
            err2.Print();
            LogPrint("\n");
            if(globalSettings.compareBuffers) CompareBuff(buff1, buff2, buff3, globalSettings.readSize*globalSettings.gDiskGeometry.BytesPerSector);
            }

        EOErrorCheck:

        if(globalSettings.numberOfReads == 1)
            {
            if(err1.win32!=0)
                {
                if(!errors) errors = new ErrorDistrubition(err1);
                else errors->AddError(err1);
                ++numFailThenFailSame;
                LogPrint("Block %u: Single Error ", block);
                err1.Print();
                LogPrint("\n");
                if(err1.sense.SenseKey == 0x05 && err1.sense.AdditionalSenseCode == 0x81) // command error
                    {
                    numCommandErrors += globalSettings.readSize;
                    if(numCommandErrors >= 4096)
                        {
                        numCommandErrors = 0;
                        numFailThenFailSame -= (4096/globalSettings.readSize);
                        errors->RemoveError(err1, (4096/globalSettings.readSize) * 2);
                        LogPrint("End of security section.\n");
                        }
                    }
                }
            }


        //
        // adjust the read pointer (skipping any exclude areas)
        //
        do {
            if(globalSettings.sequentialReads)
                {
                block += globalSettings.readSize;
                if(block >= globalSettings.endLBA)
                    {
                    block = globalSettings.startLBA;
                    ++numTimesThrough;
                    }
                }
            else
                {
                rando = rand();
                bigResult = (ULONGLONG)(rando*(ULONGLONG)(globalSettings.endLBA-globalSettings.startLBA));
                block = (DWORD)((bigResult/RAND_MAX) + globalSettings.startLBA);
                }

            if(globalSettings.stopAfter.duration)
                {
                if((globalSettings.stopAfter.type == STOPAFTER_ITERATIONS && numTimesThrough > globalSettings.stopAfter.duration) ||
                    (globalSettings.stopAfter.type == STOPAFTER_MINUTES && TICK2MIN(GetTickCount() - time) >= globalSettings.stopAfter.duration) ||
                    (globalSettings.stopAfter.type == STOPAFTER_HOURS && TICK2HOUR(GetTickCount() - time) >= globalSettings.stopAfter.duration) )
                    {
                    exit = true;
                    break;
                    }
                }
            } while((globalSettings.exclude && globalSettings.exclude->InList(block, globalSettings.chunkSizeMax)) || (block+globalSettings.chunkSizeMax >= globalSettings.endLBA));


        //
        // avg read time
        //
        if(globalSettings.avgSpan && (numReads % globalSettings.avgSpan == 0))
            {
            DWORD avg = 0;
            if(avgTimeCount != 0)
                avg = avgTime/avgTimeCount;

            if(globalSettings.sequentialReads)
                LogPrint("Blocks %u-%u: avg Read Time == %u ms\n", avgStartLBA, block, avg);
            else
                LogPrint("Read %u-%u: avg Read Time == %u ms\n", numReads-globalSettings.avgSpan, numReads, avg);

            avgTimeCount = 0;
            avgTime = 0;
            avgStartLBA = block;
            }

        if(InputCheckDigitalButton(XINPUT_GAMEPAD_START)) GUI();
        if(InputCheckExitCombo()) exit = true;
        } // while


    //
    // dump the stats
    //
    DWORD finalTime = (GetTickCount() - time)/1000;
    DWORD totalErrors = numFailThenFailSame*2 + numFailThenFail*2 + numFailThenPass + numPassThenFail;

    LogPrint("****Program Stats:\n");
    LogPrint("*** %9u sec run time\n", finalTime);
    LogPrint("*** %9I64u Total blocks read\n", numBlocksRead);
    LogPrint("****\n");
    if(globalSettings.numberOfReads == 1)
        LogPrint("*** %9u Single Failures\n", numFailThenFailSame);
    else
        {
        LogPrint("*** %9u Buffer mismatches (no failure reported)\n", numBuffMisMatch);
        LogPrint("*** %9u Failures then different failures\n", numFailThenFail);
        LogPrint("*** %9u Failures then same failures\n", numFailThenFailSame);
        LogPrint("*** %9u Failures then successes\n", numFailThenPass);
        LogPrint("*** %9u Successes then failures\n", numPassThenFail);
        }
    LogPrint("*** %9u Total Reported Errors\n", totalErrors);
    LogPrint("****\n");
    LogPrint("*** %9u errors / reads\n", numReads? totalErrors / numReads : 0);
    LogPrint("*** %9u errors / sec\n", finalTime? totalErrors / finalTime : 0);
    if(errors)
        {
        LogPrint("****Error Distribution:\n");
        errors->PrintErrors();
        }


    //
    // Post results if necessary
    //
    if(strncmp(globalSettings.resultsServer, "http://", 7) == 0)
        {
        WSADATA wsaData;
        unsigned short version = MAKEWORD(2, 2);
        WSAStartup(version, &wsaData);

        DBDATA results;
        if(geometryIOCTL == IOCTL_CDROM_GET_DRIVE_GEOMETRY)
            results.deviceType = "DVD";
        else
            results.deviceType = "Hard Drive";

        results.manufacturer = RightTrim(model);
        results.firmwareVersion = RightTrim(firmware);
        results.hardwareVersion = "";
        results.serialNumber = RightTrim(serial);
        results.testName = "Disk Read";

        char *modes[] = 
            {
            "Detect",
            "Raw DVD",
            "Raw CDDA",
            "Volume"
            };
        char variation[256];
        char *v = variation;
        v += sprintf(v, "%s reads of %u-%u blocks, ", globalSettings.sequentialReads?"Sequential":"Random", globalSettings.chunkSizeMin, globalSettings.chunkSizeMax);
        v += sprintf(v, "From LBA %u to %u, ", globalSettings.startLBA, globalSettings.endLBA);
        v += sprintf(v, "Read Mode: %s", modes[globalSettings.readMode]);
        results.variation = variation;
        results.boxInformation = GetBoxInformation();
        //DebugPrint("%s", results.boxInformation);
        results.configSettings = iniConfig;

        // passes
        results.status = 1;
        results.numOperations = numBlocksRead-(totalErrors+numBuffMisMatch);
        results.notes = "Successful Reads";
        if(results.numOperations)
            PostResults(globalSettings.resultsServer, &results);

        // failures (read errors)
        results.status = 0;
        results.numOperations = totalErrors;
        if(results.numOperations)
            {
            results.notes = new char[4*1024];
            if(results.notes)
                {
                char *temp = results.notes;
                temp += sprintf(temp, "Read Errors:\n");
                errors->SPrintErrors(temp);
                PostResults(globalSettings.resultsServer, &results);
                delete[] results.notes;
                }
            else
                {
                results.notes = "Read Errors";
                PostResults(globalSettings.resultsServer, &results);
                }
            }

        // failures (buffer mismatches)
        results.status = 0;
        results.numOperations = numBuffMisMatch;
        results.notes = "Buffer Mismatches";
        if(results.numOperations)
            PostResults(globalSettings.resultsServer, &results);

        // cleanup
        delete[] results.boxInformation;
        }

    delete[] iniConfig;
    delete[] buff1;
    delete[] buff2;
    delete[] buff3;
    NtClose(hDevice);

    if(globalSettings.pauseOnExit)
        {
        Pause();
        }
    else
        {
        // persist the display across the reboot
        g_pDevice->PersistDisplay();
        }

    //
    // test done, reboot
    //
    return ShutDown(launchInfo, numBlocksRead, totalErrors+numBuffMisMatch, "\0");
    }



void Initialize(void)
    {
    //
    // USB
    //
    XInitDevices(0, NULL);

    //
    // Network
    //
    XNetStartupParams xnsp = { sizeof(XNetStartupParams), XNET_STARTUP_BYPASS_SECURITY };
    XNetStartup(&xnsp);

    //
    //
    //
    InitGraphics();
    }


