
#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>



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

    DebugPrint("LOADER TEST APP: In the Game.\n");
    DebugPrint("    Launch Type: 0x%X\n", dataType);
    DebugPrint("    Sleeping forever...\n");

    while(1)
        {
        }
    }
