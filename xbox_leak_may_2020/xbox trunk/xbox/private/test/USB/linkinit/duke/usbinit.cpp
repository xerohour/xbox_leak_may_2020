
#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>

extern void DebugPrint(char* format, ...);

#include "../commontest.h"

char *logVariationStr;

struct VariationDescription
    {
    char *description;
    unsigned size;
    XDEVICE_PREALLOC_TYPE deviceTypes[2];
    };

VariationDescription variations[] =
    {
    { "0 XDEVICE_TYPE_GAMEPADs",
        1, { {XDEVICE_TYPE_GAMEPAD, 0}, {0, 0} } },
    { "1 XDEVICE_TYPE_GAMEPADs",
        1, { {XDEVICE_TYPE_GAMEPAD, 1}, {0, 0} } },
    { "1 and 1 XDEVICE_TYPE_GAMEPADs",
        2, { {XDEVICE_TYPE_GAMEPAD, 1}, {XDEVICE_TYPE_GAMEPAD, 1} } },
    { "2 XDEVICE_TYPE_GAMEPADs",
        1, { {XDEVICE_TYPE_GAMEPAD, 2}, {0, 0} } },
    { "3 XDEVICE_TYPE_GAMEPADs",
        1, { {XDEVICE_TYPE_GAMEPAD, 3}, {0, 0} } },
    { "3 and 3 XDEVICE_TYPE_GAMEPADs",
        2, { {XDEVICE_TYPE_GAMEPAD, 3}, {XDEVICE_TYPE_GAMEPAD, 3} } },
    { "4 XDEVICE_TYPE_GAMEPADs",
        1, { {XDEVICE_TYPE_GAMEPAD, 4}, {0, 0} } },
    { "5 XDEVICE_TYPE_GAMEPADs",
        1, { {XDEVICE_TYPE_GAMEPAD, 5}, {0, 0} } },
    { "1024 XDEVICE_TYPE_GAMEPADs",
        1, { {XDEVICE_TYPE_GAMEPAD, 1*KILOBYTE}, {0, 0} } },
    { "1meg XDEVICE_TYPE_GAMEPADs",
        1, { {XDEVICE_TYPE_GAMEPAD, 1*MEGABYTE}, {0, 0} } },
    { "128meg XDEVICE_TYPE_GAMEPADs",
        1, { {XDEVICE_TYPE_GAMEPAD, 128*MEGABYTE}, {0, 0} } },
    };


void main(void)
    {
    HANDLE hLog;
    DWORD error;
    DWORD dataType = 0xCDCDCDCD;
    LAUNCH_DATA launchInfo;
    UsbInitInfo *info = (UsbInitInfo*)&launchInfo;

    error = XGetLaunchInfo(&dataType, &launchInfo);

    if(dataType == LDT_FROM_DASHBOARD || dataType == LDT_FROM_DEBUGGER_CMDLINE || dataType == 0xCDCDCDCD)
        {
        info->variation = 0;
        hLog = xCreateLog("t:\\initusb.log", XLO_REFRESH);
        }
    else
        {
        hLog = xCreateLog("t:\\initusb.log", XLO_DEFAULT);
        }

    if(hLog == NULL)
        {
        DebugPrint("LOG FILE IS NULL!!!!\n");
        }

    TESTCASE(variations[info->variation].description)
        {
        XInitDevices(variations[info->variation].size, variations[info->variation].deviceTypes);
        TESTPASS(hLog, "XInitDevices");
        Sleep(4000);
        DWORD devices;
        for(unsigned i=0; i<variations[info->variation].size; i++)
            {
            devices = XGetDevices(variations[info->variation].deviceTypes[i].DeviceType);
            if(devices != 0)
                TESTPASS(hLog, "  XGetDevices dev=0x%08X ret=0x%08X", variations[info->variation].deviceTypes[i].DeviceType, devices);
            else
                TESTFAIL(hLog, "  XGetDevices dev=0x%08X ret=0x%08X", variations[info->variation].deviceTypes[i].DeviceType, devices);
            }
        } ENDTESTCASE;

    ++info->variation;

    xCloseLog(hLog);

    if(info->variation == ARRAYSIZE(variations)) XLaunchNewImage(NULL, NULL);
    else XLaunchNewImage("D:\\duke.xbe", &launchInfo);
    }

void DebugPrint(char* format, ...)
    {
    va_list args;
    va_start(args, format);

    char szBuffer[1024];

    vsprintf(szBuffer, format, args);
    OutputDebugStringA(szBuffer);

    va_end(args);
    }

/*
char *configuration[] = 
    {
    "DUKE 0",
    "DUKE 1",
    "DUKE 2",
    "DUKE 3",
    "DUKE 4",
    "DUKE 5",
    "DUKE 6",
    "DUKE 7",
    "DUKE 8",
    "DUKE 9",
    "MU 0",
    "MU 1",
    "MU 2",
    "MU 3",
    "MU 4",
    "MU 5",
    "MU 6",
    "MU 7",
    "MU 8",
    "MU 9",
    "HAWK 0",
    "HAWK 1",
    "HAWK 2",
    "HAWK 3",
    "HAWK 4",
    "HAWK 5",
    "HAWK 6",
    "HAWK 7",
    "HAWK 8",
    "HAWK 9",
    "DVD 0",
    "DVD 1",
    "DVD 2",
    "DVD 3",
    "DVD 4",
    "DVD 5",
    "DVD 6",
    "DVD 7",
    "DVD 8",
    "DVD 9",
    "KEYBOARD 0",
    "KEYBOARD 1",
    "KEYBOARD 2",
    "KEYBOARD 3",
    "KEYBOARD 4",
    "KEYBOARD 5",
    "KEYBOARD 6",
    "KEYBOARD 7",
    "KEYBOARD 8",
    "KEYBOARD 9",
    "DUKE 1, MU 0",
    "DUKE 1, MU 1",
    "DUKE 1, MU 2",
    "DUKE 1, MU 8",
    "DUKE 1, HAWK 0",
    "DUKE 1, HAWK 1",
    "DUKE 1, HAWK 2",
    "DUKE 1, HAWK 8",
    "DUKE 1, KEYBOARD 0",
    "DUKE 1, KEYBOARD 1",
    "DUKE 1, KEYBOARD 2",
    "DUKE 1, KEYBOARD 8",
    "DUKE 1, DVD 0",
    "DUKE 1, DVD 1",
    "DUKE 1, DVD 2",
    "DUKE 1, DVD 8",
    "DUKE 1, MU 0, HAWK 0",
    "DUKE 1, MU 1, HAWK 1",
    "DUKE 1, MU 2, HAWK 2",
    "DUKE 1, MU 8, HAWK 8",
    "DUKE 1, MU 0, HAWK 0, KEYBOARD 1",
    "DUKE 1, MU 1, HAWK 1, DVD 1",
    "DUKE 1, MU 2, KEYBOARD 1",
    "DUKE 2, MU 4, HAWK 4",
    "DUKE 2, MU 6, HAWK 6",
    "DUKE 2, MU 8, HAWK 8",
    "DUKE 2, MU 0, HAWK 0, KEYBOARD 1",
    "DUKE 2, MU 1, HAWK 1, DVD 1",
    "DUKE 2, MU 2, KEYBOARD 1",
    "DUKE 3, MU 4, HAWK 4",
    "DUKE 3, MU 6, HAWK 6",
    "DUKE 3, MU 8, HAWK 8",
    "DUKE 3, MU 0, HAWK 0, KEYBOARD 1",
    "DUKE 3, MU 1, HAWK 1, DVD 1",
    "DUKE 3, MU 2, KEYBOARD 1",
    "DUKE 4, MU 4, HAWK 4",
    "DUKE 4, MU 6, HAWK 6",
    "DUKE 4, MU 8, HAWK 8",
    "DUKE 4, MU 0, HAWK 0, KEYBOARD 1",
    "DUKE 4, MU 1, HAWK 1, DVD 1",
    "DUKE 4, MU 2, KEYBOARD 1",
    "DUKE 4, MU 8, HAWK 8, DVD 4, KEYBOARD 4",
    };
*/

