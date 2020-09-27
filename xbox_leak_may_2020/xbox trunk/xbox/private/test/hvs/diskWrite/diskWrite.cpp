/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    diskWrite.cpp

Abstract:

    Disk scanner

Notes:
    Hard Drive Partition Sizes
        \Device\Harddisk0\Partition0 19541088
        \Device\Harddisk0\Partition1  9997568   C:
        \Device\Harddisk0\Partition2  1023840   Y:
        \Device\Harddisk0\Partition3  1535776   Z:
        \Device\Harddisk0\Partition4  1535776   Z:
        \Device\Harddisk0\Partition5  1535776   Z:

Tests:
    Read Write Read (low level)                     (test 0)
    Create Many Files (file system level)           (test 1)
    Massive Write then verify  (file system level)  (test 2)

*****************************************************************************/

#include "diskWrite.h"
#include <xbeimage.h>
#define XeImageHeader() ((struct _XBEIMAGE_HEADER*)XBEIMAGE_STANDARD_BASE_ADDRESS)

HANDLE hDuke[XGetPortCount()] = { NULL, NULL, NULL, NULL };
HANDLE hSlot[XGetPortCount()*2] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
DWORD addDevice, removeDevice;


//
// config settings
//
ConfigSettings globalSettings;

//
// stats
//
DWORD numFail = 0;
DWORD numPass = 0;
DWORD numErrors = 0;
DWORD numBuffMismatch = 0;
DWORD numTimesThrough = 1;
DWORD time = 0;
unsigned __int64 bytesWritten = 0;
unsigned __int64 numWrites = 0;
unsigned __int64 numBlocksRead = 0;
unsigned __int64 numBlocksWritten = 0;
char model[64], serial[64], firmware[64];
DWORD mediaType = MEDIA_UNKNOWN;
bool cleanup = false;
DWORD avgTimeCount;
DWORD avgTime;
DWORD lastReadTime;
LAUNCH_DATA launchInfo;

void TestReadWriteRead(void);
void TestCreateFile(void);
void TestWriteFile(void);

void ParseTestConfig(char *variable, char *value, void *userParam)
    {
    ConfigSettings *settings = (ConfigSettings*)userParam;

    if(strstr(variable, "drive") != NULL) strcpy(settings->drive, value);
    else if(strstr(variable, "test") != NULL) settings->test = GetNumber(value);
    else if(strstr(variable, "seed") != NULL) settings->seed = GetNumber(value);
    else if(strstr(variable, "writesize") != NULL) settings->writeSize = GetNumber(value);
    else if(strstr(variable, "pauseonexit") != NULL) settings->pauseOnExit = GetNumber(value);
    else if(strstr(variable, "startlba") != NULL) settings->startLBA = GetNumber(value);
    else if(strstr(variable, "endlba") != NULL) settings->endLBA = GetNumber(value);
    else if(strstr(variable, "sequentialreads") != NULL) settings->sequentialReads = GetNumber(value);
    else if(strstr(variable, "partition") != NULL) settings->partition = GetNumber(value);
    else if(strstr(variable, "pauseonmismatch") != NULL) settings->pauseOnMismatch = GetNumber(value);
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
    DBDATA results;

    //
    // Get the launch data (if any)
    //
    DWORD dataType = 0xCDCDCDCD;
    XGetLaunchInfo(&dataType, &launchInfo);
    ToTestData* initialParams = (ToTestData*)&launchInfo;

    Initialize();

    //
    // cleanup old files
    //
    DeleteFile("T:\\testexit.txt");

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
        FILE *file = fopen("D:\\diskWrite.ini", "rb");
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
    SetSaveName(globalSettings.saveName, "diskWrite.log");
    LogFlush();

    if(initialParams->titleID == HVSLAUNCHERID)
        LogPrint("Launched from HVS\n  Test %u of %u\n\n", initialParams->testNumber, initialParams->totalTests);
    else
        LogPrint("Running in stand a lone mode\n\n");

    LogPrint("Disk Write (Built on " __DATE__ " at " __TIME__ ")\n");
    LogPrint("Copyright (C) Microsoft Corporation.  All rights reserved.\n\n");


    memset(model, 0, 64);
    memset(serial, 0, 64);
    memset(firmware, 0, 64);

    char driveLetter = CLEARFLAG(globalSettings.drive[0], 0x20); // make it caps

    if(globalSettings.test == TEST_READ_WRITE_READ) driveLetter = 'C'; // drive letter isnt used on this test...

    if(driveLetter == 'D') // can be either DVD or HD
        {
        ULARGE_INTEGER avail, total, free;
        GetDiskFreeSpaceEx("D:\\", &avail, &total, &free);
        if(free.QuadPart != 0) driveLetter = 'C'; // D points to hard disk
        }
    else if(driveLetter == 'A') // DVD
        driveLetter = 'D';

    if(driveLetter >= 'F' && driveLetter <= 'M') // MU
        {
        Sleep(2000); // let the mus get mounted

        strcpy(model, "MU");
        results.deviceType = "MU";
        mediaType = MEDIA_MU;
        }
    else if(driveLetter == 'D') // CD/DVD
        {
        results.deviceType = "DVD";
        GetDriveID(NULL, true, model, serial, firmware);
        mediaType = GetDiskType(NULL);
        // TODO error, cant write to a DVD / CD
        }

    // hard disk
    else if(driveLetter == 'C')
        {
        results.deviceType = "Hard Drive";
        GetDriveID(NULL, false, model, serial, firmware);
        mediaType = MEDIA_HD;
        MapDrive('C', "\\Device\\Harddisk0\\Partition1");
        }
    else if(driveLetter == 'Y')
        {
        results.deviceType = "Hard Drive";
        GetDriveID(NULL, false, model, serial, firmware);
        mediaType = MEDIA_HD;
        MapDrive('Y', "\\Device\\Harddisk0\\Partition2");
        }
    else
        {
        results.deviceType = "Hard Drive";
        GetDriveID(NULL, false, model, serial, firmware);
        mediaType = MEDIA_HD;
        }


    //
    // print out the drive info and config settings
    //
	LogPrint("DRIVE: Model:             %s\n", model);
	LogPrint("DRIVE: Serial:            %s\n", serial);
	LogPrint("DRIVE: Firmware:          %s\n", firmware);
    LogPrint("MEDIA: Type:              %s\n", mediaTypeNames[mediaType]);

    LogPrint("Config Settings:\n");
    LogPrint("  ResultsServer:    %s\n", globalSettings.resultsServer);
    LogPrint("  SaveName:         %s\n", globalSettings.saveName);
    LogPrint("  StopAfter:        %u %s\n", globalSettings.stopAfter.duration, globalSettings.stopAfter.type==STOPAFTER_MINUTES?"min":(globalSettings.stopAfter.type==STOPAFTER_HOURS?"hours":"iterations"));
	LogPrint("\n");
    LogPrint("  Drive:            %s\n", globalSettings.drive);
    LogPrint("  Test:             %u\n", globalSettings.test);
    LogPrint("  WriteSize:        %u\n", globalSettings.writeSize);
    LogPrint("\n");
    // TODO logPrint the rest of the settings

    srand(globalSettings.seed);

    // run test
    if(globalSettings.test == TEST_READ_WRITE_READ)
        {
        TestReadWriteRead();
        }
    else if(globalSettings.test == TEST_CREATE_FILES)
        {
        TestCreateFile();
        }
    else if(globalSettings.test == TEST_WRITE_FILE)
        {
        TestWriteFile();
        }


    //
    // dump the stats
    //
    DWORD finalTime = (GetTickCount() - time)/1000;

    LogPrint("****Program Stats:\n");
    LogPrint("*** %9u sec run time\n", finalTime);
    LogPrint("*** %9I64u Total bytes written\n", bytesWritten);
    LogPrint("****\n");
    LogPrint("*** %9u Failures\n", numFail);
    LogPrint("*** %9u Successes\n", numPass);
    LogPrint("****\n");
    LogPrint("*** %9u errors / attempts\n", (numFail+numPass)? numFail / (numFail+numPass) : 0);
    LogPrint("*** %9u errors / sec\n", finalTime? numFail / finalTime : 0);

    //
    // Post results if necessary
    //
    if(strncmp(globalSettings.resultsServer, "http://", 7) == 0)
        {
        WSADATA wsaData;
        unsigned short version = MAKEWORD(2, 2);
        WSAStartup(version, &wsaData);

        results.manufacturer = RightTrim(model);
        results.firmwareVersion = RightTrim(firmware);
        results.hardwareVersion = "";
        results.serialNumber = RightTrim(serial);
        results.testName = "Disk Write";

        char variation[256];
        results.boxInformation = GetBoxInformation();
        results.configSettings = iniConfig;
        results.variation = variation;

        if(globalSettings.test == TEST_READ_WRITE_READ)
            {
            sprintf(variation, "Test %s, Partition %u", testNames[globalSettings.test], globalSettings.partition);

            // failures (read/write errors)
            results.status = 0;
            results.numOperations = numErrors;
            results.notes = "Read/Write Errors";
            if(results.numOperations)
                PostResults(globalSettings.resultsServer, &results);

            // failures (buffer mismatches)
            results.status = 0;
            results.numOperations = numBuffMismatch;
            results.notes = "Buffer Mismatches";
            if(results.numOperations)
                PostResults(globalSettings.resultsServer, &results);
            }
        else if(globalSettings.test == TEST_CREATE_FILES)
            {
            sprintf(variation, "Test %s, Drive %s", testNames[globalSettings.test], globalSettings.drive);

            // failures (read errors)
            results.status = 0;
            results.numOperations = numFail;
            results.notes = "Read Errors";
            if(results.numOperations)
                PostResults(globalSettings.resultsServer, &results);
            }
        else if(globalSettings.test == TEST_WRITE_FILE)
            {
            sprintf(variation, "Test %s, Drive %s", testNames[globalSettings.test], globalSettings.drive);

            // TODO
            }

        // passes
        results.status = 1;
        results.numOperations = numPass;
        results.notes = "Successful Writes";
        if(results.numOperations)
            PostResults(globalSettings.resultsServer, &results);

        // cleanup
        delete[] results.boxInformation;
        }

    delete[] iniConfig;

    if(driveLetter == 'C')
        UnMapDrive('C');
    else if(driveLetter == 'Y')
        UnMapDrive('Y');

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
    return ShutDown(launchInfo, numPass, numFail, "\0");
    }


void TestReadWriteRead(void)
    {
    DWORD rando = 0;
    ULONGLONG bigResult = 0;
    DWORD geoSize;
    DWORD status;
    DWORD err1=0, err2=0, err3=0;
    DWORD block = globalSettings.startLBA;

    char deviceName[48];
    sprintf(deviceName, "\\Device\\Harddisk0\\Partition%u", globalSettings.partition);

    //
    // open the Hard drive
    //
    HANDLE hDevice;
    OBJECT_ATTRIBUTES ObjA;
    OBJECT_STRING VolumeString;
    IO_STATUS_BLOCK IoStatusBlock;
    RtlInitObjectString(&VolumeString, deviceName);
    InitializeObjectAttributes(&ObjA, &VolumeString, OBJ_CASE_INSENSITIVE, NULL, NULL);
    status = NtCreateFile(&hDevice, SYNCHRONIZE|GENERIC_READ|GENERIC_WRITE, &ObjA, &IoStatusBlock, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ|FILE_SHARE_WRITE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT | FILE_NO_INTERMEDIATE_BUFFERING | FILE_FLAG_WRITE_THROUGH);
    if(!NT_SUCCESS(status))
        {
        LogPrint("Unable to open the device '%s' (ec: %u)\n", deviceName, RtlNtStatusToDosError(status));
        DebugPrint("Rebooting...\n\n\n");
        ShutDown(launchInfo, 0, 0, "Unable to open the device:\\n '%s' (ec: %u)", deviceName, RtlNtStatusToDosError(status));
        return;
        }

	DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &globalSettings.gDiskGeometry, sizeof(globalSettings.gDiskGeometry), &geoSize, NULL);
    if(globalSettings.gDiskGeometry.Cylinders.QuadPart == 0)
        {
        NtClose(hDevice);
        LogPrint("No Media Detected\n");
        DebugPrint("Rebooting...\n\n\n");
        ShutDown(launchInfo, 0, 0, "No Media Detected");
        return;
        }

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
    if(globalSettings.endLBA == 0) globalSettings.endLBA = (DWORD)globalSettings.gDiskGeometry.Cylinders.QuadPart;

    //
    // allocate buffer memory
    //
    unsigned char *buff1 = new unsigned char[globalSettings.writeSize * globalSettings.gDiskGeometry.BytesPerSector];
    unsigned char *buff2 = new unsigned char[globalSettings.writeSize * globalSettings.gDiskGeometry.BytesPerSector];
    unsigned char *buff3 = new unsigned char[globalSettings.writeSize * globalSettings.gDiskGeometry.BytesPerSector];
    if(!buff1 || !buff2 || !buff3)
        {
        LogPrint("Unable to allocate enough space (%u)\n", (globalSettings.writeSize * globalSettings.gDiskGeometry.BytesPerSector) * 3);
        LogPrint("Try lowering writeSize\n");
        DebugPrint("Rebooting...\n\n\n");
        NtClose(hDevice);
        ShutDown(launchInfo, 0, 0, "Unable to allocate enough space (%u)", (globalSettings.writeSize * globalSettings.gDiskGeometry.BytesPerSector) * 3);
        return;
        }

    time = GetTickCount();

    bool exit = false;
    while(!exit)
        {
        XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDevice, &removeDevice);
        InputDukeInsertions(addDevice, removeDevice);
        XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &addDevice, &removeDevice);
        InputMUInsertions(addDevice, removeDevice);
        UpdateGraphics(DISPLAYMODE_RUN_TIME);

        err1 = err2 = err3 = 0;

        memset(buff1, 0, globalSettings.writeSize * globalSettings.gDiskGeometry.BytesPerSector);
        memset(buff2, 0, globalSettings.writeSize * globalSettings.gDiskGeometry.BytesPerSector);
        memset(buff3, 0, globalSettings.writeSize * globalSettings.gDiskGeometry.BytesPerSector);

        ++numWrites;

        err1 = ReadHDBlocks(hDevice, block, globalSettings.writeSize, buff1);
        numBlocksRead += globalSettings.writeSize;
        if(err1 == 0)
            {
            // if the read failed dont even try to write it
            err2 = WriteHDBlocks(hDevice, block, globalSettings.writeSize, buff1);
            numBlocksWritten += globalSettings.writeSize;
            err3 = ReadHDBlocks(hDevice, block, globalSettings.writeSize, buff2);
            numBlocksRead += globalSettings.writeSize;
            }

        if((err1 != err2) || (err1 != err3))
            {
            LogPrint("Block %u: Error1(%u, %08X) != Error2(%u, %08X) != Error3(%u, %08X)\n", block, RtlNtStatusToDosError(err1), err1, RtlNtStatusToDosError(err2), err2, RtlNtStatusToDosError(err3), err3);
            ++numErrors;
            // TODO if(err2) try rewriting buff1 becuase we may have just screwed up the disk
            }
        else if(memcmp(buff1, buff2, globalSettings.writeSize*globalSettings.gDiskGeometry.BytesPerSector) != 0)
            {
            if(globalSettings.pauseOnMismatch) Pause();

            LogPrint("Block %u: Buff1 != Buff2\n", block);
            ++numBuffMismatch;

            /*
            int retry = 3;
            do {
                // do another read for reference
                err4 = ReadBlocks(hDevice, block, globalSettings.writeSize, buff3);
                retry--;
                } while(err4 != 0 && retry >= 0);

            numBlocksRead += globalSettings.readSize;
            if(globalSettings.compareBuffers) CompareBuff(buff1, buff2, buff3, globalSettings.readSize*globalSettings.gDiskGeometry.BytesPerSector);
            */
            // TODO try rewriting buff1 becuase we may have just screwed up the disk
            }
        else
            {
            ++numPass;
            }

        //
        // adjust the read/write pointer
        //
        do {
            if(globalSettings.sequentialReads)
                {
                block += globalSettings.writeSize;
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
            } while(block+globalSettings.writeSize >= globalSettings.endLBA);

        if(InputCheckDigitalButton(XINPUT_GAMEPAD_START)) GUI();
        if(InputCheckExitCombo()) exit = true;
        if(globalSettings.stopAfter.type == STOPAFTER_ITERATIONS && numTimesThrough > globalSettings.stopAfter.duration) exit = true;
        if(globalSettings.stopAfter.duration)
            {
            if((globalSettings.stopAfter.type == STOPAFTER_MINUTES && TICK2MIN(GetTickCount() - time) >= globalSettings.stopAfter.duration) ||
                (globalSettings.stopAfter.type == STOPAFTER_HOURS && TICK2HOUR(GetTickCount() - time) >= globalSettings.stopAfter.duration) )
                {
                exit = true;
                }
            }
        }

    delete[] buff1;
    delete[] buff2;
    delete[] buff3;
    NtClose(hDevice);

    numFail = numErrors + numBuffMismatch;
    }



void TestCreateFile(void)
    {
    char path[MAX_PATH];
    char *filename;
    DWORD wSize, write;
    HANDLE hFile;
    filename = path + sprintf(path, "%s\\diskWriteDir", globalSettings.drive);

    unsigned char *buff1 = new unsigned char[globalSettings.writeSize];
    if(!buff1)
        {
        LogPrint("Unable to allocate enough space (%u)\n", globalSettings.writeSize);
        LogPrint("Try lowering writeSize\n");
        DebugPrint("Rebooting...\n\n\n");
        ShutDown(launchInfo, 0, 0, "Unable to allocate enough space (%u)", globalSettings.writeSize);
        return;
        }
    for(unsigned buffFiller=0; buffFiller<globalSettings.writeSize; buffFiller++) buff1[buffFiller] = (unsigned char)buffFiller;

    time = GetTickCount();

    if(!CreateDirectory(path, NULL))
        {
        LogPrint("Create directory '%s' failed (ec: %u)\n", path, GetLastError());
        delete[] buff1;
        return;
        }

    bool exit = false;
    while(!exit)
        {
        XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDevice, &removeDevice);
        InputDukeInsertions(addDevice, removeDevice);
        XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &addDevice, &removeDevice);
        InputMUInsertions(addDevice, removeDevice);
        UpdateGraphics(DISPLAYMODE_RUN_TIME);

        sprintf(filename, "\\diskWrite%04X.tmp", numTimesThrough);
        hFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN|FILE_FLAG_WRITE_THROUGH, NULL);

        if(hFile == INVALID_HANDLE_VALUE)
            {
            LogPrint("CreateFile Failure: (ec: %u) on %s\n", GetLastError(), path);
            ++numFail;
            }
        else
            {
            write = rand() % globalSettings.writeSize;
            if(!WriteFile(hFile, buff1, write, &wSize, NULL))
                {
                LogPrint("Write Failure: (ec: %u) on %s\n", GetLastError(), path);
                ++numFail;
                }
            else if(write != wSize)
                {
                LogPrint("Write Failure: unable to write correct number of bytes (wrote %u, from %u) (ec: %u) on %s\n", wSize, write, GetLastError(), path);
                ++numFail;
                }
            else
                {
                bytesWritten += write;
                ++numPass;
                }
            CloseHandle(hFile);
            }

        ++numTimesThrough;

        if(InputCheckDigitalButton(XINPUT_GAMEPAD_START)) GUI();
        if(InputCheckExitCombo())
            {
            exit = true;
            break;
            }
        if(globalSettings.stopAfter.type == STOPAFTER_ITERATIONS)
            if(numTimesThrough > globalSettings.stopAfter.duration) exit = true;
        if(globalSettings.stopAfter.duration)
            {
            if((globalSettings.stopAfter.type == STOPAFTER_MINUTES && TICK2MIN(GetTickCount() - time) >= globalSettings.stopAfter.duration) ||
                (globalSettings.stopAfter.type == STOPAFTER_HOURS && TICK2HOUR(GetTickCount() - time) >= globalSettings.stopAfter.duration) )
                {
                exit = true;
                break;
                }
            }
        }

    cleanup = true;

    for(DWORD i=1; i<=numTimesThrough; i++)
        {
        sprintf(filename, "\\diskWrite%04X.tmp", i);
        if(!DeleteFile(path))
            {
            LogPrint("Delete Failure: %u on %s\n", GetLastError(), path);
            }
        }

    filename[0] = '\0';
    RemoveDirectory(path);
    delete[] buff1;
    }


void TestWriteFile(void)
    {
    time = GetTickCount();

    // Run the test
    bool exit = false;
    while(!exit)
        {
        XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDevice, &removeDevice);
        InputDukeInsertions(addDevice, removeDevice);
        XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &addDevice, &removeDevice);
        InputMUInsertions(addDevice, removeDevice);
        UpdateGraphics(DISPLAYMODE_RUN_TIME);

        //TODO run test

        if(InputCheckDigitalButton(XINPUT_GAMEPAD_START)) GUI();
        if(InputCheckExitCombo())
            {
            exit = true;
            break;
            }
        if(globalSettings.stopAfter.type == STOPAFTER_ITERATIONS)
            if(numTimesThrough > globalSettings.stopAfter.duration) exit = true;
        if(globalSettings.stopAfter.duration)
            {
            if((globalSettings.stopAfter.type == STOPAFTER_MINUTES && TICK2MIN(GetTickCount() - time) >= globalSettings.stopAfter.duration) ||
                (globalSettings.stopAfter.type == STOPAFTER_HOURS && TICK2HOUR(GetTickCount() - time) >= globalSettings.stopAfter.duration) )
                {
                exit = true;
                break;
                }
            }
        }

    // TODO DeleteFile(globalSettings.fileName);
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


