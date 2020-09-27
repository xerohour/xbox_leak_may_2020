#include <xtl.h>
#include "draw.h"
#include "devices.h"


// Hack! DbgPrint is much nicer than OutputDebugString so JonT exported it from xapi.lib
// (as DebugPrint) but the prototype isn't in the public headers...
extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}

//------------------------------------------------------------------------------
// TestMain
//------------------------------------------------------------------------------

void __cdecl main()
{
    CDraw draw;

    //
    //  Initialize core peripheral port support
    //
    XDEVICE_PREALLOC_TYPE devices[]=
	{
		{XDEVICE_TYPE_GAMEPAD, 4}
	};
    XInitDevices(sizeof(devices)/sizeof(XDEVICE_PREALLOC_TYPE),devices);

    //
    //  Initialize Display
    //
    draw.FillRect(0, 0, 640, 480, BACKGROUND_BLUE); 
    draw.DrawText(L"TEST FOR BAD DAKOTA UNITS:", 300,  10, LABEL_WHITE);
    draw.Present();
    draw.FillRect(0, 0, 640, 480, BACKGROUND_BLUE); 
    draw.DrawText(L"TEST FOR BAD DAKOTA UNITS:", 300,  10, LABEL_WHITE);

    CGameControllers *pGameControllers = new CGameControllers;
    
    do
    {
        pGameControllers->CheckForHotplugs();
        pGameControllers->Draw(draw);
        draw.Present();
        Sleep(8);  //Update display every 20 milliseconds
        
    }while(1);
}

