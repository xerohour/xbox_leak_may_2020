
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
    XDEVICE_PREALLOC_TYPE deviceTypes[4];
    };


VariationDescription variations[] =
    {
    { "0 XDEVICE_TYPE_GAMEPADs; 0 XDEVICE_TYPE_MEMORY_UNITs",
        2, { {XDEVICE_TYPE_GAMEPAD, 0}, {XDEVICE_TYPE_MEMORY_UNIT, 0}, {0, 0}, {0, 0} } },
    { "1 XDEVICE_TYPE_GAMEPADs; 1 XDEVICE_TYPE_MEMORY_UNITs",
        2, { {XDEVICE_TYPE_GAMEPAD, 1}, {XDEVICE_TYPE_MEMORY_UNIT, 1}, {0, 0}, {0, 0} } },
    { "1 and 1 XDEVICE_TYPE_GAMEPADs; 1 and 1 XDEVICE_TYPE_MEMORY_UNITs",
        4, { {XDEVICE_TYPE_GAMEPAD, 1}, {XDEVICE_TYPE_GAMEPAD, 1}, {XDEVICE_TYPE_MEMORY_UNIT, 1}, {XDEVICE_TYPE_MEMORY_UNIT, 1} } },
    { "2 XDEVICE_TYPE_GAMEPADs; 2 XDEVICE_TYPE_MEMORY_UNITs",
        2, { {XDEVICE_TYPE_GAMEPAD, 2}, {XDEVICE_TYPE_MEMORY_UNIT, 2}, {0, 0}, {0, 0} } },
    { "3 XDEVICE_TYPE_GAMEPADs; 3 XDEVICE_TYPE_MEMORY_UNITs",
        2, { {XDEVICE_TYPE_GAMEPAD, 3}, {XDEVICE_TYPE_MEMORY_UNIT, 3}, {0, 0}, {0, 0} } },
    { "3 and 3 XDEVICE_TYPE_GAMEPADs; 3 and 3 XDEVICE_TYPE_MEMORY_UNITs",
        4, { {XDEVICE_TYPE_GAMEPAD, 3}, {XDEVICE_TYPE_GAMEPAD, 3}, {XDEVICE_TYPE_MEMORY_UNIT, 3}, {XDEVICE_TYPE_MEMORY_UNIT, 3} } },
    { "4 XDEVICE_TYPE_GAMEPADs; 4 XDEVICE_TYPE_MEMORY_UNITs",
        2, { {XDEVICE_TYPE_GAMEPAD, 4}, {XDEVICE_TYPE_MEMORY_UNIT, 4}, {0, 0}, {0, 0} } },
    { "5 XDEVICE_TYPE_GAMEPADs; 5 XDEVICE_TYPE_MEMORY_UNITs",
        2, { {XDEVICE_TYPE_GAMEPAD, 5}, {XDEVICE_TYPE_MEMORY_UNIT, 5}, {0, 0}, {0, 0} } },
    { "1024 XDEVICE_TYPE_GAMEPADs; 1024 XDEVICE_TYPE_MEMORY_UNITs",
        2, { {XDEVICE_TYPE_GAMEPAD, 1*KILOBYTE}, {XDEVICE_TYPE_MEMORY_UNIT, 1*KILOBYTE}, {0, 0}, {0, 0} } },
    { "1meg XDEVICE_TYPE_GAMEPADs; 1meg XDEVICE_TYPE_MEMORY_UNITs",
        2, { {XDEVICE_TYPE_GAMEPAD, 1*MEGABYTE}, {XDEVICE_TYPE_MEMORY_UNIT, 1*MEGABYTE}, {0, 0}, {0, 0} } },
    { "128meg XDEVICE_TYPE_GAMEPADs; 128meg XDEVICE_TYPE_MEMORY_UNITs",
        2, { {XDEVICE_TYPE_GAMEPAD, 128*MEGABYTE}, {XDEVICE_TYPE_MEMORY_UNIT, 128*MEGABYTE}, {0, 0}, {0, 0} } },
    { "4 XDEVICE_TYPE_GAMEPADs; 8 XDEVICE_TYPE_MEMORY_UNITs",
        2, { {XDEVICE_TYPE_GAMEPAD, 4}, {XDEVICE_TYPE_MEMORY_UNIT, 8}, {0, 0}, {0, 0} } },
    { "1 XDEVICE_TYPE_GAMEPADs; 8 XDEVICE_TYPE_MEMORY_UNITs",
        2, { {XDEVICE_TYPE_GAMEPAD, 1}, {XDEVICE_TYPE_MEMORY_UNIT, 8}, {0, 0}, {0, 0} } },
    { "4 XDEVICE_TYPE_GAMEPADs; 2 XDEVICE_TYPE_MEMORY_UNITs",
        2, { {XDEVICE_TYPE_GAMEPAD, 4}, {XDEVICE_TYPE_MEMORY_UNIT, 2}, {0, 0}, {0, 0} } },
    { "4 XDEVICE_TYPE_GAMEPADs; 2 XDEVICE_TYPE_MEMORY_UNITs",
        2, { {XDEVICE_TYPE_GAMEPAD, 4}, {XDEVICE_TYPE_MEMORY_UNIT, 2}, {0, 0}, {0, 0} } },
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
    else XLaunchNewImage("D:\\dukemu.xbe", &launchInfo);
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

