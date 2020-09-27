
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
    { "0 XDEVICE_TYPE_VOICE_MICROPHONEs; 0 XDEVICE_TYPE_VOICE_HEADPHONEs",
        2, { {XDEVICE_TYPE_VOICE_MICROPHONE, 0}, {XDEVICE_TYPE_VOICE_HEADPHONE, 0}, {0, 0}, {0, 0} } },

    { "1 XDEVICE_TYPE_VOICE_MICROPHONEs; 1 XDEVICE_TYPE_VOICE_HEADPHONEs",
        2, { {XDEVICE_TYPE_VOICE_MICROPHONE, 1}, {XDEVICE_TYPE_VOICE_HEADPHONE, 1}, {0, 0}, {0, 0} } },

    { "1 and 1 XDEVICE_TYPE_VOICE_MICROPHONEs; 1 and 1 XDEVICE_TYPE_VOICE_HEADPHONEs",
        4, { {XDEVICE_TYPE_VOICE_MICROPHONE, 1}, {XDEVICE_TYPE_VOICE_MICROPHONE, 1}, {XDEVICE_TYPE_VOICE_HEADPHONE, 1}, {XDEVICE_TYPE_VOICE_HEADPHONE, 1} } },

    { "2 XDEVICE_TYPE_VOICE_MICROPHONEs; 2 XDEVICE_TYPE_VOICE_HEADPHONEs",
        2, { {XDEVICE_TYPE_VOICE_MICROPHONE, 2}, {XDEVICE_TYPE_VOICE_HEADPHONE, 2}, {0, 0}, {0, 0} } },

    { "3 XDEVICE_TYPE_VOICE_MICROPHONEs; 3 XDEVICE_TYPE_VOICE_HEADPHONEs",
        2, { {XDEVICE_TYPE_VOICE_MICROPHONE, 3}, {XDEVICE_TYPE_VOICE_HEADPHONE, 3}, {0, 0}, {0, 0} } },

    { "3 and 3 XDEVICE_TYPE_VOICE_MICROPHONEs; 3 and 3 XDEVICE_TYPE_VOICE_HEADPHONEs",
        4, { {XDEVICE_TYPE_VOICE_MICROPHONE, 3}, {XDEVICE_TYPE_VOICE_MICROPHONE, 3}, {XDEVICE_TYPE_VOICE_HEADPHONE, 3}, {XDEVICE_TYPE_VOICE_HEADPHONE, 3} } },

    { "4 XDEVICE_TYPE_VOICE_MICROPHONEs; 4 XDEVICE_TYPE_VOICE_HEADPHONEs",
        2, { {XDEVICE_TYPE_VOICE_MICROPHONE, 4}, {XDEVICE_TYPE_VOICE_HEADPHONE, 4}, {0, 0}, {0, 0} } },

    { "1 XDEVICE_TYPE_VOICE_MICROPHONEs; 4 XDEVICE_TYPE_VOICE_HEADPHONEs",
        2, { {XDEVICE_TYPE_VOICE_MICROPHONE, 1}, {XDEVICE_TYPE_VOICE_HEADPHONE, 4}, {0, 0}, {0, 0} } },

    { "4 XDEVICE_TYPE_VOICE_MICROPHONEs; 1 XDEVICE_TYPE_VOICE_HEADPHONEs",
        2, { {XDEVICE_TYPE_VOICE_MICROPHONE, 4}, {XDEVICE_TYPE_VOICE_HEADPHONE, 1}, {0, 0}, {0, 0} } },

    { "4 XDEVICE_TYPE_VOICE_MICROPHONEs; 2 XDEVICE_TYPE_VOICE_HEADPHONEs",
        2, { {XDEVICE_TYPE_VOICE_MICROPHONE, 4}, {XDEVICE_TYPE_VOICE_HEADPHONE, 2}, {0, 0}, {0, 0} } },

    { "2 XDEVICE_TYPE_VOICE_MICROPHONEs; 4 XDEVICE_TYPE_VOICE_HEADPHONEs",
        2, { {XDEVICE_TYPE_VOICE_MICROPHONE, 2}, {XDEVICE_TYPE_VOICE_HEADPHONE, 4}, {0, 0}, {0, 0} } },
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
    else XLaunchNewImage("D:\\hawk.xbe", &launchInfo);
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

