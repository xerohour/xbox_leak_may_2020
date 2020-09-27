/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    sampleTest.cpp

Abstract:

    Sample code for integrating with the HVS Launcher

Notes:
    
    * Custom ini file settings are stored in the ConfigSettings struct
    * Ini file settings are parsed out in the ParseTestConfig function


*****************************************************************************/

#include <xtl.h>
#include <stdio.h>
#include <xbeimage.h>
#define XeImageHeader() ((struct _XBEIMAGE_HEADER*)XBEIMAGE_STANDARD_BASE_ADDRESS)

#include "../utils/hvsUtils.h"

HANDLE hDuke[XGetPortCount()] = { NULL, NULL, NULL, NULL };
HANDLE hSlot[XGetPortCount()*2] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

static void Initialize(void);

struct ConfigSettings : public CoreConfigSettings
    {
    DWORD customParam1;
    char customParam2[64];

    ConfigSettings()
        {
        customParam1 = 123;
        customParam2[0] = '\0';
        }
    } globalSettings;


void ParseTestConfig(char *variable, char *value, void *userParam)
    {
    ConfigSettings *settings = (ConfigSettings*)userParam;

    if(strstr(variable, "customparam1") != NULL) settings->customParam1 = GetNumber(value);
    else if(strstr(variable, "customparam2") != NULL) strcpy(settings->customParam2, value);
    }

void _cdecl main(void)
    {
    //
    // Get the launch data (if any)
    //
    DWORD dataType = 0xCDCDCDCD;
    LAUNCH_DATA launchInfo;
    XGetLaunchInfo(&dataType, &launchInfo);
    ToTestData* initialParams = (ToTestData*)&launchInfo;
    char *iniConfig = NULL;

    Initialize();

    //
    // Grab the startup parameters
    //
    if(initialParams->titleID == HVSLAUNCHERID)
        {
        // parameters are coming from HVS
        iniConfig = new char[strlen(initialParams->configSettings)+1];
        strcpy(iniConfig, initialParams->configSettings);
        ParseConfigSettings(initialParams->configSettings, &globalSettings, ParseTestConfig, &globalSettings);
        }
    else
        {
        // not launched from HVS, run in standAlone mode
        FILE *file = fopen("D:\\sampleTest.ini", "rb");
        if(file)
            {
            DWORD filesize = _filelength(_fileno(file));
            iniConfig = new char[filesize+1];
            unsigned bytesRead = fread(iniConfig, 1, filesize, file);
            iniConfig[bytesRead] = '\0';

            ParseConfigSettings(iniConfig, &globalSettings, ParseTestConfig, &globalSettings);

            // read the file again, cause ParseConfigSettings will modify the buffer
            if(fseek(file, 0, SEEK_SET) == 0)
                {
                bytesRead = fread(iniConfig, 1, filesize, file);
                iniConfig[bytesRead] = '\0';
                }
            fclose(file);
            }
        }

    // Call SetSaveName BEFORE calling LogPrint or LogFlush
    SetSaveName(globalSettings.saveName, "sampleTest.log");
    LogFlush();

    if(initialParams->titleID == HVSLAUNCHERID)
        LogPrint("Launched from HVS\n  Test %u of %u\n\n", initialParams->testNumber, initialParams->totalTests);
    else
        LogPrint("Running in stand a lone mode\n\n");


    // Log out important INI file settings for reference
    LogPrint("Config Settings:\n");
    LogPrint("  ResultsServer: %s\n", globalSettings.resultsServer);
    LogPrint("  SaveName:      %s\n", globalSettings.saveName);
    LogPrint("  StopAfter:     %u %s\n", globalSettings.stopAfter.duration, globalSettings.stopAfter.type==STOPAFTER_MINUTES?"min":(globalSettings.stopAfter.type==STOPAFTER_HOURS?"hours":"iterations"));

    LogPrint("  CustomParam1:  %u\n", globalSettings.customParam1);
    LogPrint("  CustomParam2:  %s\n", globalSettings.customParam2);



    //
    // TODO run the test here
    //


    //
    // Sends test results + hardware information to the database
    //
    if(strncmp(globalSettings.resultsServer, "http://", 7) == 0)
        {
        DBDATA data;
        data.deviceType = "deviceType";
        data.manufacturer = "manufacturer";
        data.firmwareVersion = "firmwareVersion";
        data.hardwareVersion = "hardwareVersion";
        data.serialNumber = "serialNumber";
        data.testName = "Sample Test";
        data.variation = "Test 1";
        data.status = 1; // 1=pass, 0=fail
        data.numOperations = 100;
        data.boxInformation = GetBoxInformation();
        data.configSettings = iniConfig;
        data.notes = "notes";

        PostResults(globalSettings.resultsServer, &data);

        // if we had any failures, we would call PostResults again 
        // with data.status=0, and numOperations = to the number of
        // failures

        LogPrint("\nBox Information:\n");
        LogPrint("%s\n\n", data.boxInformation);
        delete[] data.boxInformation;
        }
    delete[] iniConfig;


    //
    // Get the percentage of acceptable failres for this test
    //
    if(strncmp(globalSettings.resultsServer, "http://", 7) == 0)
        {
        double criteria = GetAcceptanceCriteria(globalSettings.resultsServer, "Sample Test");
        LogPrint("Criteria for 'test' = %lf\n", criteria);
        }


    //
    // test done, reboot
    //
    if(initialParams->titleID == HVSLAUNCHERID)
        {
        ToLauncherData* outParams = (ToLauncherData*)&launchInfo;
        outParams->titleID = XeImageHeader()->Certificate->TitleID;
        outParams->numPass = 100;
        outParams->numFail = 0;
        outParams->notes[0] = '\0';

        LaunchImage("D:\\default.xbe", &launchInfo);
        }
    else
        XLaunchNewImage(NULL, NULL);
    }


static void Initialize(void)
    {
    DWORD error;

    //
    // USB
    //
    XInitDevices(0, NULL);

    //
    // Network
    //
    WSADATA wsaData;
    unsigned short version = MAKEWORD(2, 2);
    XNetStartupParams xnsp = { sizeof(XNetStartupParams), XNET_STARTUP_BYPASS_SECURITY };
    error = XNetStartup(&xnsp);
    if(error != NO_ERROR)
        {
        }
    Sleep(5000); // needed cause XNetStartup isnt fully started when call exits
    WSAStartup(version, &wsaData);

    //
    //
    //
    //InitGraphics();
    }

