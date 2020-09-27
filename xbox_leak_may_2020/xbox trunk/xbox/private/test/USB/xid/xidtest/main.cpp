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
XINPUT_POLLING_PARAMETERS g_PollingParameters = {0};
//
//  Useful Macro
//
#define DECLARE_DO_TWICE int doTwice;
#define DO_TWICE doTwice=2; while(doTwice--)

void DrawTestCaseMessage(CDraw& draw, LPWSTR pMessage);

//------------------------------------------------------------------------------
//  Forward Declations
//------------------------------------------------------------------------------
void DrawTestCaseMessage(CDraw& draw, LPWSTR pMessage);

//------------------------------------------------------------------------------
// TestMain
//------------------------------------------------------------------------------

void __cdecl main()
{
    DECLARE_DO_TWICE;
    int iFrameCount=0;
    int iState = 0;
    BOOL fUseUpdateAndRedraw = TRUE;
    BOOL fCallPollDevice;
    DebugPrint("XInput Test Application.\nBuilt on %s at %s\n", __DATE__, __TIME__);
    CDraw draw;
    
    //Initialize core peripheral port support
    XDEVICE_PREALLOC_TYPE devices[]=
	{
		{XDEVICE_TYPE_GAMEPAD, 4}
        //{XDEVICE_TYPE_VOICE_HEADPHONE, 0}
	};

    XInitDevices(sizeof(devices)/sizeof(XDEVICE_PREALLOC_TYPE),devices);

    //For the memory unit driver to load
    XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);

    DO_TWICE
    {
        draw.FillRect(0, 0, 640, 480, BACKDROP_BLUE); 
        draw.DrawText(L"XInput Test Application", 300,  0, LABEL_WHITE);
        draw.Present();
    }
    CGameControllers *pGameControllers = NULL;
    do
    {
        fCallPollDevice = FALSE; //most of the case use autopoll
        iState = (iState+1)%5;
        //
        //  Decide how to open the game controllers this time
        //
        switch(iState)
        {
            case 0:
                DrawTestCaseMessage(draw, L"Default Polling Parameters");
                pGameControllers = new CGameControllers(NULL);
                break;
            case 1:
                DrawTestCaseMessage(draw, L"AutoPoll, Control-OUT, Input Interval 16 ms, Calling PollDevice");
                g_PollingParameters.fAutoPoll = TRUE;
                g_PollingParameters.fInterruptOut = FALSE;
                g_PollingParameters.bInputInterval = 16;
                g_PollingParameters.ReservedMBZ1 = 0;
                g_PollingParameters.ReservedMBZ2 = 0;
                pGameControllers = new CGameControllers(&g_PollingParameters);
                fCallPollDevice = TRUE; //make sure PollDevices does no harm.
                break;
            case 2:
                DrawTestCaseMessage(draw, L"AutoPoll, Control-OUT, Input Interval 4 ms");
                g_PollingParameters.fAutoPoll = TRUE;
                g_PollingParameters.fInterruptOut = FALSE;
                g_PollingParameters.bInputInterval = 4;
                g_PollingParameters.ReservedMBZ1 = 0;
                g_PollingParameters.ReservedMBZ2 = 0;
                pGameControllers = new CGameControllers(&g_PollingParameters);
                break;
            case 3:
                DrawTestCaseMessage(draw, L"ManualPoll, Control-OUT, Input Interval 2 ms");
                g_PollingParameters.fAutoPoll = FALSE;
                g_PollingParameters.fInterruptOut = FALSE;
                g_PollingParameters.bInputInterval = 2;
                g_PollingParameters.ReservedMBZ1 = 0;
                g_PollingParameters.ReservedMBZ2 = 0;
                pGameControllers = new CGameControllers(&g_PollingParameters);
                fCallPollDevice = TRUE; //make sure PollDevices does no harm.
                break;
            case 4:
                DrawTestCaseMessage(draw, L"ManualPoll, Control-OUT, Input Interval 2 ms, NOT POLLING");
                g_PollingParameters.fAutoPoll = FALSE;
                g_PollingParameters.fInterruptOut = FALSE;
                g_PollingParameters.bInputInterval = 2;
                g_PollingParameters.ReservedMBZ1 = 0;
                g_PollingParameters.ReservedMBZ2 = 0;
                pGameControllers = new CGameControllers(&g_PollingParameters);
                break;
        }
        //Poll for three hundred frames
        iFrameCount = 0;        
        while(300 > iFrameCount++)
        {
           pGameControllers->CheckForHotplugs();
           pGameControllers->UpdateState();
           DO_TWICE{
               pGameControllers->Draw(draw);
               draw.Present();
           }
           //
           //   Sleep for a video frame
           //
           if(fCallPollDevice)
           {
             Sleep(8);
             pGameControllers->PollDevices();
             Sleep(8);
           } else
           {
             Sleep(16);
           }
        }
        delete pGameControllers;
    }while(1);
}

void DrawTestCaseMessage(CDraw& draw, LPWSTR pMessage)
{
    DECLARE_DO_TWICE;
    DebugPrint("%ws\n", pMessage);
    DO_TWICE
    {
        draw.FillRect(0, 465, 640, 14, BACKDROP_BLUE); 
        draw.DrawText(L"TEST CASE:", 0, 465, LABEL_WHITE);
        draw.DrawText(pMessage, 60, 465, CONNECTED_YELLOW);
        draw.Present();
    }
}
