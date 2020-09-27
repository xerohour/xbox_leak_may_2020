/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    reboot.cpp

Abstract:

    File System disk scanner.

Notes:

  xbsetcfg /D xc:\reboot.xbe
*****************************************************************************/

#include "reboot.h"
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
Stats stats;
char model[64], serial[64], firmware[64];
DWORD mediaType = MEDIA_UNKNOWN;

#define SCREEN_WAIT_TIME 2000

void ParseTestConfig(char *variable, char *value, void *userParam)
    {
    ConfigSettings *settings = (ConfigSettings*)userParam;

    if(strstr(variable, "seed") != NULL) settings->seed = GetNumber(value);
    else if(strstr(variable, "reboottype") != NULL) settings->rebootType = GetNumber(value);
    }

extern "C" POBJECT_STRING XeImageFileName;

void _cdecl main(void)
    {
    bool firstRun = false;
    bool exit = false;
    DBDATA results;
    DWORD totalReboots;

    //
    // Get the launch data (if any)
    //
    DWORD dataType = 0xCDCDCDCD;
    LAUNCH_DATA launchInfo;
    XGetLaunchInfo(&dataType, &launchInfo);
    ToTestData* initialParams = (ToTestData*)&launchInfo;

    Initialize();


    // mount c:
    MapDrive('C', "\\Device\\Harddisk0\\Partition1");
    if(!FileExists("c:\\dashboard_backup.xbx"))
        {
        firstRun = true;
        }

    if(firstRun) 
        {
        // save the original D path so we know where the launcher is located
        GetMapDrive('D', stats.driveD);
        //DebugPrint("************Drive D: %s\n", stats.driveD);
        }
    else
        {
        // when run via the dashboard.xbx file, D is not mapped to the 
        // location of the xbe, so we map it ourselves
        UnMapDrive('D');
        MapDrive('D', "\\Device\\Harddisk0\\Partition1");
        }

    //
    // cleanup old files
    //
    DeleteFile("T:\\testexit.txt");

    //
    // Grab the startup parameters
    //
    char *iniConfig = NULL;
    if(initialParams->titleID == HVSLAUNCHERID)
        {
        FILE *f = fopen("C:\\reboot.ini", "w+");
        if(f)
            {
            fprintf(f, "%s", initialParams->configSettings);
            fclose(f);
            }
        else
            {
            LogPrint("Unable to write to C:\\reboot.ini (ec: %u), test will run with default parameters.\n", GetLastError());
            }
        iniConfig = new char[strlen(initialParams->configSettings)+1];
        strcpy(iniConfig, initialParams->configSettings);
        ParseConfigSettings(initialParams->configSettings, &globalSettings, ParseTestConfig, &globalSettings);
        }
    else
        {
        FILE *file = fopen("D:\\reboot.ini", "rb");
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

    //
    // seed random number generator
    //
    srand(globalSettings.seed);

    // if random mode, set the reboot mode to an actual setting
    if(globalSettings.rebootType == REBOOT_RANDOM)
        globalSettings.rebootType = rand() % REBOOT_RANDOM;


    SetSaveName(globalSettings.saveName, "reboot.log");

    //
    // configure our test to run on reboots
    //
    if(firstRun)
        {
        char rebootLoc[128];

        // Call SetSaveName BEFORE calling LogPrint or LogFlush
        LogFlush();

        DeleteFile("T:\\stats.dat");

        // figure out where this xbe is located so we can copy it
        // eg: \Device\Harddisk0\Partition1\devkit\hvs\tests\reboot.xbe
        strncpy(rebootLoc, XeImageFileName->Buffer, XeImageFileName->Length);
        rebootLoc[XeImageFileName->Length] = '\0';
        char *temp = strrchr(rebootLoc, '\\');
        if(temp) *temp = '\0';
        MapDrive('Q', rebootLoc);
        if(CopyFile("Q:\\reboot.xbe", "C:\\reboot.xbe", FALSE) == 0) // just in case reboot.xbe is on a DVD
            {
            UnMapDrive('Q');
            LogPrint("Unable to copy %s\\reboot.xbe (ec: %u)\n", rebootLoc, GetLastError());
            goto systemCleanup;
            }
        SetFileAttributes("Q:\\reboot.xbe", FILE_ATTRIBUTE_NORMAL);
        UnMapDrive('Q');
        if(CopyFile("C:\\dashboard.xbx", "C:\\dashboard_backup.xbx", FALSE) == 0)
            {
            LogPrint("Unable to backup C:\\dashboard.xbx (ec: %u)\n", GetLastError());
            goto systemCleanup;
            }

        // copy ini file (or create ini) to c:
        if(iniConfig)
            {
            CopyFile("D:\\reboot.ini", "C:\\reboot.ini", FALSE);
            }

        // edit dashboard.xbx to load reboot.xbe on bootup
        FILE *f = fopen("C:\\dashboard.xbx", "wb+");
        if(f)
            {
            char metaImage[360];
            memset(metaImage, 0, 360);
            strcpy(metaImage, "\\Device\\Harddisk0\\Partition1;reboot.xbe");
            fwrite(metaImage, sizeof(metaImage), 1, f);
            fclose(f);
            }
        else
            {
            LogPrint("Unable to write to C:\\dashboard.xbx (ec: %u)\n", GetLastError());
            }
        }




    memset(model, 0, 64);
    memset(serial, 0, 64);
    memset(firmware, 0, 64);

    results.deviceType = "DVD";
    GetDriveID(NULL, true, model, serial, firmware);
    mediaType = GetDiskType(NULL);

    ULARGE_INTEGER FreeBytesAvailable;
    ULARGE_INTEGER TotalNumberOfBytes;
    ULARGE_INTEGER TotalNumberOfFreeBytes;
    GetDiskFreeSpaceEx("A:\\", &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes);

    if(firstRun)
        {
        LogPrint("Reboot (Built on " __DATE__ " at " __TIME__ ")\n");
        LogPrint("Copyright (C) Microsoft Corporation.  All rights reserved.\n\n");

        if(initialParams->titleID == HVSLAUNCHERID)
            LogPrint("Launched from HVS\n  Test %u of %u\n\n", initialParams->testNumber, initialParams->totalTests);
        else
            LogPrint("Running in stand a lone mode\n\n");

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
        LogPrint("  RebootType:       %u\n", globalSettings.rebootType);
        LogPrint("  Seed:             %u\n", globalSettings.seed);
        LogPrint("\n");
        }

    // Open and read the stats file
    FILE *f = fopen("T:\\stats.dat", "rb+");
    if(!f) f = fopen("T:\\stats.dat", "wb+");
    if(!f)
        {
        LogPrint("Error: unable to open the stats file (ec: %u)\n", GetLastError());
        }
    if(!firstRun) 
        {
        if(fread(&stats, sizeof(stats), 1, f) != 1)
            {
            LogPrint("Error: unable to read the stats file (ec: %u)\n", GetLastError());
            }
        }

    // initialize stats if necessary
    if(firstRun)
        {
        if(initialParams->titleID == HVSLAUNCHERID) stats.launchedFromHVS = true;
        }
    if(stats.mediaType == MEDIA_UNKNOWN) stats.mediaType = mediaType;
    if(stats.mediaSize == 0) stats.mediaSize = TotalNumberOfBytes.QuadPart;

    totalReboots = stats.numSoftReboots + stats.numHardReboots + stats.numQuickReboots + stats.numSMCReboots + stats.failures + 1;

    // update stats
    if(mediaType == MEDIA_UNKNOWN)
        {
        ++stats.failures;
        LogPrint("Error on reboot %u: no media detected\n", totalReboots);
        }
    else if(mediaType != stats.mediaType)
        {
        ++stats.failures;
        LogPrint("Error on reboot %u: Current media (%s) does not match previous media (%s)\n", totalReboots, mediaTypeNames[mediaType], mediaTypeNames[stats.mediaType]);
        }
    else if(stats.mediaSize < TotalNumberOfBytes.QuadPart)
        {
        stats.mediaSize = TotalNumberOfBytes.QuadPart;
        ++stats.failures;
        LogPrint("Error on reboot %u: Detected wrong media size\n", totalReboots);
        }
    else if(stats.mediaSize < TotalNumberOfBytes.QuadPart)
        {
        ++stats.failures;
        LogPrint("Error on reboot %u: Detected wrong media size\n", totalReboots);
        }
    else // media detected successfully
        {
        if(globalSettings.rebootType == REBOOT_SOFT) ++stats.numSoftReboots;
        else if(globalSettings.rebootType == REBOOT_HARD) ++stats.numHardReboots;
        else if(globalSettings.rebootType == REBOOT_QUICK) ++stats.numQuickReboots;
        else if(globalSettings.rebootType == REBOOT_SMC) ++stats.numSMCReboots;
        }
    stats.runTime += GetTickCount() + SCREEN_WAIT_TIME;

    // write the stats
    if(fseek(f, 0, SEEK_SET) == 0)
        {
        fwrite(&stats, sizeof(stats), 1, f);
        }
    fclose(f);


    UpdateGraphics(DISPLAYMODE_RUN_TIME);

    // give USB time to enum devices, and the user time to see the screen
    for(unsigned time=0; time<SCREEN_WAIT_TIME; time+=200)
        {
        XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDevice, &removeDevice);
        InputDukeInsertions(addDevice, removeDevice);

        // check stopAfter
        if(globalSettings.stopAfter.type == STOPAFTER_ITERATIONS && totalReboots >= globalSettings.stopAfter.duration) exit = true;
        else if(globalSettings.stopAfter.type == STOPAFTER_MINUTES && TICK2MIN(stats.runTime) >= globalSettings.stopAfter.duration) exit = true;
        else if(globalSettings.stopAfter.type == STOPAFTER_HOURS && TICK2HOUR(stats.runTime) >= globalSettings.stopAfter.duration) exit = true;

        // check for usb exit
        if(InputCheckExitCombo()) exit = true;

        if(exit) break;

        Sleep(200);
        }


    if(!exit)
        {
        UnMapDrive('C');

        // persist the display across the reboot
        g_pDevice->PersistDisplay();

        LogPrint("Rebooting (#%u) (type=%u)...\n", totalReboots+1, globalSettings.rebootType);

        // let IO complete - may be unneccessary, better safe than blowing a test run
        Sleep(200);

        //
        // reboot
        //
        if(globalSettings.rebootType == REBOOT_SOFT)
            HalReturnToFirmware(HalRebootRoutine);
        else if(globalSettings.rebootType == REBOOT_HARD)
            HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_RESET, SMC_RESET_ASSERT_POWERCYCLE);
        else if(globalSettings.rebootType == REBOOT_QUICK)
            XLaunchNewImage("D:\\reboot.xbe", NULL);
        else if(globalSettings.rebootType == REBOOT_SMC)
            HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_RESET, SMC_RESET_ASSERT_RESET);

        // If the reboot is successful, we will never get here

        // sleep for 10 sec
        Sleep(10000);

        // if reboot hasnt happened yet, end the run and cleanup
        LogPrint("Reboot didnt happen after 10 sec. Ending Run.\n");
        LogPrint("   You might not have a new SMC.\n");
        MapDrive('C', "\\Device\\Harddisk0\\Partition1"); // gotta remount C
        }



    /////////////////////////////////////////////////////////////////////////
    //
    // test is done if we get here, cleanup and reboot back to the real world
    //
    /////////////////////////////////////////////////////////////////////////


systemCleanup:

    //
    // system cleanup
    //
    CopyFile("C:\\dashboard_backup.xbx", "C:\\dashboard.xbx", FALSE);
    DeleteFile("C:\\dashboard_backup.xbx");
    DeleteFile("C:\\reboot.xbe");
    DeleteFile("C:\\reboot.ini");
    DeleteFile("T:\\stats.dat");


    //
    // dump the stats
    //
    DWORD finalTime = stats.runTime/1000;
    DWORD totalErrors = stats.failures;

    LogPrint("****Program Stats:\n");
    LogPrint("*** %9u sec run time\n", finalTime);
    LogPrint("*** %9u Total Reboots\n", totalReboots);
    LogPrint("****\n");
    LogPrint("*** %9u Total Reported Errors\n", totalErrors);
    LogPrint("****\n");
    LogPrint("*** %9u errors / reboot\n", totalReboots? totalErrors / totalReboots : 0);
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
        results.testName = "Reboot";

        char variation[256];
        char *v = variation;
        sprintf(variation, "Reboot Mode: %u", globalSettings.rebootType);
        results.variation = variation;
        results.boxInformation = GetBoxInformation();
        results.configSettings = iniConfig;

        // passes
        results.status = 1;
        results.numOperations = stats.numSoftReboots;
        results.notes = "Successful Soft Reboots";
        if(results.numOperations)
            PostResults(globalSettings.resultsServer, &results);

        // passes
        results.status = 1;
        results.numOperations = stats.numHardReboots;
        results.notes = "Successful Hard Reboots";
        if(results.numOperations)
            PostResults(globalSettings.resultsServer, &results);

        // passes
        results.status = 1;
        results.numOperations = stats.numQuickReboots;
        results.notes = "Successful Quick Reboots";
        if(results.numOperations)
            PostResults(globalSettings.resultsServer, &results);

        // passes
        results.status = 1;
        results.numOperations = stats.numSMCReboots;
        results.notes = "Successful SMC Reboots";
        if(results.numOperations)
            PostResults(globalSettings.resultsServer, &results);

        // failures
        results.status = 0;
        results.numOperations = stats.failures;
        results.notes = "Media Detection Failures";
        if(results.numOperations)
            PostResults(globalSettings.resultsServer, &results);

        // cleanup
        delete[] results.boxInformation;
        }

    delete[] iniConfig;
    UnMapDrive('C');
    if(!firstRun) UnMapDrive('D');

    // persist the display across the reboot
    g_pDevice->PersistDisplay();

    //
    // test done, reboot back to the real world
    //
    if(stats.launchedFromHVS)
        {
        UnMapDrive('D');
        MapDrive('D', stats.driveD);

        ToLauncherData* outParams = (ToLauncherData*)&launchInfo;
        outParams->titleID = XeImageHeader()->Certificate->TitleID;
        outParams->numPass = totalReboots - totalErrors;
        outParams->numFail = totalErrors;
        outParams->notes[0] = '\0';

        LaunchImage("D:\\default.xbe", &launchInfo);
        }
    else
        XLaunchNewImage(NULL, NULL);
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
    // D3D
    //
    InitGraphics();
    }


