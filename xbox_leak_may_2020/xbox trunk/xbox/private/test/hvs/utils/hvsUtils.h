/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    hvsUtils.h

Abstract:

    HVS Test Helper functions

Notes:

*****************************************************************************/

#ifndef _HVSUTILS_H_
#define _HVSUTILS_H_

#pragma warning(disable: 4200) // nonstandard extension used : zero-sized array in struct/union

// define filelength instead of including io.h because it will 
// collide with ntos\inc\io.h
extern "C"
    {
    long __cdecl _filelength(int);
    }

#ifndef KILOBYTE
#define KILOBYTE 1024
#define MEGABYTE (KILOBYTE * KILOBYTE)
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)        (sizeof(a) / sizeof(a[0]))
#endif

#define TICK2MIN(val)       ((val)/1000/60)
#define TICK2HOUR(val)      ((val)/1000/60/60)

#define HVSLAUNCHERID 0x48570000

struct ToTestData
    {
    DWORD titleID; // always == 48570000
    unsigned short testNumber; // test # out of totalTests
    unsigned short totalTests;
    char configSettings[0];
    };

struct ToLauncherData
    {
    DWORD titleID;
    unsigned __int64 numPass;
    unsigned __int64 numFail;
    char notes[0];
    };

struct DBDATA
    {
    char *deviceType;
    char *manufacturer;
    char *firmwareVersion;
    char *hardwareVersion;
    char *serialNumber;
    char *testName;
    char *variation;
    unsigned int status;
    unsigned __int64 numOperations;
    char *boxInformation;
    char *configSettings;
    char *notes;
    };

enum STOPAFTER_TYPES
    {
    STOPAFTER_MINUTES,
    STOPAFTER_HOURS,
    STOPAFTER_ITERATIONS,
    STOPAFTER_NEVER
    };

struct StopAfterSetting
    {
    unsigned duration;
    int type;           // STOPAFTER_TYPES

    // default values
    StopAfterSetting()
        {
        duration = 0;
        type = STOPAFTER_NEVER;
        }
    };

struct CoreConfigSettings
    {
    char resultsServer[128];
    char saveName[64];
    StopAfterSetting stopAfter;

    // default values
    CoreConfigSettings()
        {
        resultsServer[0] = '\0';
        strcpy(saveName, "Test Results");
        }
    };

// Save Game logging
void SetSaveName(char *saveName, char *fileName);
void LogPrint(char* format, ...);
void LogFlush(void);

// Sends the test entries to the web
DWORD PostResults(const char *url, const DBDATA *results);

// Retrieves the percentage of acceptable failres from the network
double GetAcceptanceCriteria(const char *url, const char* testName);

// Gathers various information about the system and software
char *GetBoxInformation(void);

// XLaunchNewImage wrapper (avoids restriction on matching title IDs)
void LaunchImage(char *xbe, LAUNCH_DATA *launchInfo);

// Misc Utilities
void DebugPrint(char* format, ...);
void DebugPrint(WCHAR* format, ...);
WCHAR* Ansi2UnicodeHack(char *str);
void HexDump(const unsigned char * buffer, DWORD length);
long GetNumber(char *string);
char* RightTrim(char *string);
WCHAR* RightTrim(WCHAR *string);

typedef void (*USERSETTING_FUNC)(char *variable, char *value, void *userParam);
void ParseConfigSettings(char *config, CoreConfigSettings *output, USERSETTING_FUNC LocalTestConfigs, void *userParam);

#endif // _HVSUTILS_H_