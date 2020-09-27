/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    readFile.cpp

Abstract:

    File System disk scanner.

Notes:

  
*****************************************************************************/

#include "readFile.h"
#include <xbeimage.h>
#define XeImageHeader() ((struct _XBEIMAGE_HEADER*)XBEIMAGE_STANDARD_BASE_ADDRESS)

HANDLE hDuke[XGetPortCount()] = { NULL, NULL, NULL, NULL };
HANDLE hSlot[XGetPortCount()*2] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
DWORD addDevice, removeDevice;

bool GetNumFiles(char *path, WIN32_FIND_DATA &data);
bool ReadFiles(char *path, WIN32_FIND_DATA &data);


//
// config settings
//
ConfigSettings globalSettings;

//
// stats
//
DWORD numFail = 0;
DWORD numPass = 0;
DWORD numBuffMisMatch = 0;
DWORD numTimesThrough = 1;
unsigned __int64 numFiles = 0;
unsigned __int64 numFilesRead = 0;
DWORD time = 0;
char model[64], serial[64], firmware[64];
WIN32_FIND_DATA findData;
double percentDone = 0; // read % of a single file
DWORD mediaType = MEDIA_UNKNOWN;

unsigned char *buff1;
unsigned char *buff2;
unsigned char *buff3;



void ParseTestConfig(char *variable, char *value, void *userParam)
    {
    ConfigSettings *settings = (ConfigSettings*)userParam;

    if(strstr(variable, "drive") != NULL) strcpy(settings->drive, value);
    else if(strstr(variable, "readsize") != NULL) settings->readSize = GetNumber(value);
    else if(strstr(variable, "comparebuffers") != NULL) settings->compareBuffers = atol(value);
    else if(strstr(variable, "pauseonexit") != NULL) settings->pauseOnExit = GetNumber(value);
    else if(strstr(variable, "numberofreads") != NULL) settings->numberOfReads = GetNumber(value);
    else if(strstr(variable, "pauseonmismatch") != NULL) settings->pauseOnMismatch = atol(value);
    else if(strstr(variable, "dir") != NULL) settings->dir = atol(value);
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
    bool exit = false;
    DBDATA results;

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
        FILE *file = fopen("D:\\readFile.ini", "rb");
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
    SetSaveName(globalSettings.saveName, "readFile.log");
    LogFlush();

    if(initialParams->titleID == HVSLAUNCHERID)
        LogPrint("Launched from HVS\n  Test %u of %u\n\n", initialParams->testNumber, initialParams->totalTests);
    else
        LogPrint("Running in stand a lone mode\n\n");

    LogPrint("Read File (Built on " __DATE__ " at " __TIME__ ")\n");
    LogPrint("Copyright (C) Microsoft Corporation.  All rights reserved.\n\n");


    memset(model, 0, 64);
    memset(serial, 0, 64);
    memset(firmware, 0, 64);

    char driveLetter = CLEARFLAG(globalSettings.drive[0], 0x20); // make it caps

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
    LogPrint("  ReadSize:         %u\n", globalSettings.readSize);
    LogPrint("  CompareBuffers:   %u\n", globalSettings.compareBuffers);
    LogPrint("  PauseOnExit:      %u\n", globalSettings.pauseOnExit);
    LogPrint("  PauseOnMismatch:  %u\n", globalSettings.pauseOnMismatch);
    LogPrint("  NumberOfReads:    %u\n", globalSettings.numberOfReads);
    LogPrint("  Dir:              %u\n", globalSettings.dir);
    LogPrint("\n");

    buff1 = new unsigned char[globalSettings.readSize];
    buff2 = new unsigned char[globalSettings.readSize];
    buff3 = new unsigned char[globalSettings.readSize];
    char *path = new char[MEGABYTE];
    if(!buff1 || !buff2 || !buff3 || !path)
        {
        LogPrint("Unable to allocate enough space (%u)\n", (globalSettings.readSize * 3) + MEGABYTE);
        DebugPrint("\n\n\nUnable to allocate enough space (%u)\n", (globalSettings.readSize * 3) + MEGABYTE);
        DebugPrint("Try lowering readSize\n");
        DebugPrint("Rebooting...\n\n\n");
        return ShutDown(launchInfo, 0, 0, "Unable to allocate enough space (%u).\\nTry lowering readSize", (globalSettings.readSize * 3) + MEGABYTE);
        }

    strcpy(path, globalSettings.drive);

    // Get the total number of files
    ParseFiles(path, GetNumFiles);
    LogPrint("Total Number of Files: %I64u\n", numFiles);
    if(numFiles == 0) exit = true;


    time = GetTickCount();
    // Run the test
    while(!exit)
        {
        XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDevice, &removeDevice);
        InputDukeInsertions(addDevice, removeDevice);
        XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &addDevice, &removeDevice);
        InputMUInsertions(addDevice, removeDevice);
        UpdateGraphics(DISPLAYMODE_RUN_TIME);

        exit = ParseFiles(path, ReadFiles);

        ++numTimesThrough;

        if(globalSettings.stopAfter.type == STOPAFTER_ITERATIONS)
            if(numTimesThrough > globalSettings.stopAfter.duration) exit = true;
        }

    //
    // dump the stats
    //
    DWORD finalTime = (GetTickCount() - time)/1000;
    DWORD totalErrors = numFail + numBuffMisMatch;

    LogPrint("****Program Stats:\n");
    LogPrint("*** %9u sec run time\n", finalTime);
    LogPrint("*** %9I64u Total files read\n", numFilesRead);
    LogPrint("****\n");
    if(globalSettings.compareBuffers == 1)
        LogPrint("*** %9u Buffer mismatches\n", numBuffMisMatch);
    LogPrint("*** %9u Failures\n", numFail);
    LogPrint("*** %9u Total Reported Errors\n", totalErrors);
    LogPrint("****\n");
    LogPrint("*** %9u errors / file\n", numFilesRead? totalErrors / numFilesRead : 0);
    LogPrint("*** %9u errors / sec\n", finalTime? totalErrors / finalTime : 0);

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
        results.testName = "Read File";

        char variation[256];
        char *v = variation;
        sprintf(variation, "Drive %s", globalSettings.drive);
        results.variation = variation;
        results.boxInformation = GetBoxInformation();
        results.configSettings = iniConfig;

        // passes
        results.status = 1;
        results.numOperations = numPass;
        results.notes = "Successful Reads";
        if(results.numOperations)
            PostResults(globalSettings.resultsServer, &results);

        // failures (read errors)
        results.status = 0;
        results.numOperations = numFail;
        results.notes = "Read Errors";
        if(results.numOperations)
            PostResults(globalSettings.resultsServer, &results);

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
    delete[] path;
    delete[] buff1;
    delete[] buff2;
    delete[] buff3;

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
    return ShutDown(launchInfo, numPass, totalErrors, "\0");
    }


bool GetNumFiles(char *path, WIN32_FIND_DATA &data)
    {
    if(globalSettings.dir) LogPrint("%8u %s\n", data.nFileSizeLow, path);
    ++numFiles;

    return false;
    }


bool ReadFiles(char *path, WIN32_FIND_DATA &data)
    {
    //LogPrint("%s\n", path);
    bool exit = false;

    FILE *file = fopen(path, "rb");
    if(!file)
        {
        LogPrint("Open Error %u: %s\n", GetLastError(), path);
        ++numFail;
        return false;
        }

    DWORD chunkSize = globalSettings.readSize;
    //DWORD fileLen = _filelength(_fileno(file));
    DWORD fileLen = data.nFileSizeLow;
    DWORD offset = 0;

    if(chunkSize > fileLen) chunkSize = fileLen;


    while(!exit)
        {
        percentDone = fileLen ? (double)offset / (double)fileLen * 100.00 : 100.00;
        XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDevice, &removeDevice);
        InputDukeInsertions(addDevice, removeDevice);
        XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &addDevice, &removeDevice);
        InputMUInsertions(addDevice, removeDevice);
        UpdateGraphics(DISPLAYMODE_RUN_TIME);

        //fseek(file, offset, SEEK_SET);
        if(fread(buff1, 1, chunkSize, file) != chunkSize)
            {
            LogPrint("Read Error %u: %s\n", GetLastError(), path);
            ++numFail;
            break;
            }

        if(globalSettings.numberOfReads == 2)
            {
            if(fseek(file, offset, SEEK_SET) != 0) LogPrint("Warning Seek Failed (ec %u) %s\n", GetLastError(), path);
            if(fread(buff2, 1, chunkSize, file) != chunkSize)
                {
                LogPrint("Read Error %u: %s\n", GetLastError(), path);
                ++numFail;
                break;
                }
            if(memcmp(buff1, buff2, chunkSize) != 0)
                {
                LogPrint("Read Error Buff Mismatch: %s\n", path);
                ++numBuffMisMatch;

                if(globalSettings.compareBuffers)
                    {
                    int retry = 3;
                    do {
                        // do another read for reference
                        if(fseek(file, offset, SEEK_SET) != 0) LogPrint("Warning Seek Failed (ec %u) %s\n", GetLastError(), path);
                        if(fread(buff3, 1, chunkSize, file) == chunkSize) break;
                        retry--;
                        } while(retry >= 0);
                    CompareBuff(buff1, buff2, buff3, chunkSize);
                    }

                break;
                }
            }

        offset += chunkSize;
        if(offset >= fileLen)
            {
            ++numPass;
            break; // done reading the file
            }

        if(offset + chunkSize >= fileLen)
            chunkSize = fileLen - offset;

        if(InputCheckDigitalButton(XINPUT_GAMEPAD_START)) GUI();
        if(InputCheckExitCombo())
            {
            exit = true;
            break;
            }
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

    percentDone = 0.0;
    ++numFilesRead;
    fclose(file);

    return exit;
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


