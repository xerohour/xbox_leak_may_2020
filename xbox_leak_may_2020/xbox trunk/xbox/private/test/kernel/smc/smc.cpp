
#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>

extern "C"
    {
    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalReadSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN ReadWordValue,
        OUT ULONG *DataValue
        );

    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalWriteSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN WriteWordValue,
        IN ULONG DataValue
        );
    }

DWORD WriteSMC(unsigned char addr, unsigned char value)
    {
    return HalWriteSMBusValue(0x20, addr, FALSE, value);
    }

DWORD ReadSMC(unsigned char addr)
    {
    DWORD value = 0xCCCCCCCC;
    DWORD status = HalReadSMBusValue(0x21, addr, FALSE, &value);
    return value;
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



void main(void)
    {
    DWORD dataType = 0xCCCCCCCC;
    LAUNCH_DATA launchInfo;

    memset(&launchInfo, 0xCC, sizeof(LAUNCH_DATA));
    XGetLaunchInfo(&dataType, &launchInfo);

    DWORD tValue = 0xCCCCCCCC;
    DWORD value;
    char ver[3];
    ver[0] = (char)ReadSMC(0x01);
    ver[1] = (char)ReadSMC(0x01);
    ver[2] = (char)ReadSMC(0x01);

    DebugPrint("SMC TEST APP.\n");
    //DebugPrint("    Launch Type: 0x%X\n", dataType);
    DebugPrint("    SMC Version: %c %c %c\n", ver[0], ver[1], ver[2]);

    WriteSMC(0x08, 0xF0); // set the led to red
    WriteSMC(0x07, 0x01); // set the led to red


    DebugPrint("Reading from SMC...\n");
    DebugPrint("  CPU Temp:  %d C\n", ReadSMC(0x09));
    DebugPrint("  Air Temp:  %d C\n", ReadSMC(0x0A));
    DebugPrint("  Fan Speed: %d\n",   ReadSMC(0x10));
    DebugPrint("  0xFF:      0x%X\n", ReadSMC(0xFF));
    DebugPrint("  0xFE:      0x%X\n", ReadSMC(0xFE));
    DebugPrint("  0x77:      0x%X\n", ReadSMC(0x77));
    DebugPrint("  0x21:      0x%X\n", ReadSMC(0x21));

    value = ReadSMC(0x03);
    DebugPrint("  DVD Tray:  %d %d %d\n", !!(value & 0x40), !!(value & 0x20), !!(value & 0x10));

    value = ReadSMC(0x04);
    DebugPrint("  AV Pack:   %d %d %d\n", !!(value & 0x04), !!(value & 0x02), !!(value & 0x01));

    //WriteSMC(0x02, 0x01); // reboot
    //WriteSMC(0x02, 0x80); // shutdown

    /*
    // Make sure Video mode is constant
    tValue = 0xCCCCCCCC;
    for(unsigned i=0; i<10000; i++)
        {
        value = ReadSMC(0x04);
        if(tValue != value)
            {
            DebugPrint("%d%d%d (0x%08X)\n", !!(value & 0x04), !!(value & 0x02), !!(value & 0x01), value);
            tValue = value;
            }
        Sleep(10); // without this we can starve out the SMC causing a shutdown ... :)
        }
        */
    

    /*
    // Read the DVD tray state
    DebugPrint("DVD Tray State:\n");
    tValue = 0xCCCCCCCC;
    while(1)
        {
        value = ReadSMC(0x03);
        if(tValue != value)
            {
            DebugPrint("%d%d%d %d (0x%08X)\n", !!(value & 0x40), !!(value & 0x20), !!(value & 0x10), value & 0x01, value);
            tValue = value;
            }
        Sleep(10); // without this we can starve out the SMC causing a shutdown ... :)
        }
    */

    DebugPrint("\nTest Done. Time to reboot\n");
    __asm int 3;
    }


