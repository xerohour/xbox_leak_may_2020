#define DEBUG_KEYBOARD

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
    { "0 XDEVICE_TYPE_DEBUG_KEYBOARD",
        1, { {XDEVICE_TYPE_DEBUG_KEYBOARD, 0}, {0, 0} } },
    { "1 XDEVICE_TYPE_DEBUG_KEYBOARD",
        1, { {XDEVICE_TYPE_DEBUG_KEYBOARD, 1}, {0, 0} } },
    { "2 XDEVICE_TYPE_DEBUG_KEYBOARDs",
        2, { {XDEVICE_TYPE_DEBUG_KEYBOARD, 1}, {XDEVICE_TYPE_DEBUG_KEYBOARD, 1} } },
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

