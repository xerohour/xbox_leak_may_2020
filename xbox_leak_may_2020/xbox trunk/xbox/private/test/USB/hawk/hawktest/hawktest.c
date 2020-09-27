#include "hawkpch.h"



VOID DrawMessage(int x, int y, LPCWSTR pcszText, int iProgPercent, int xProg, int yProg);
ULONG DebugPrint(PCHAR Format,...);



VOID
WaitForAnyButton()
{
    BOOL fButtonPressed = FALSE;
    static DWORD dwPads = 0;
    DWORD dwInsertions, dwRemovals;
    int i;

    while (!fButtonPressed)
    {
        if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals))
        {
            dwPads |= dwInsertions;
            dwPads &= ~dwRemovals;
        }

        for (i = 0; i < XGetPortCount(); i++)
        {
            if ((1 << i) & dwPads)
            {
                HANDLE hPad = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0 + i, XDEVICE_NO_SLOT, NULL);

                if (NULL != hPad)
                {
                    XINPUT_STATE State;

                    Sleep(10);

                    if (ERROR_SUCCESS == XInputGetState(hPad, &State))
                    {
                        if ((0 != State.Gamepad.wButtons) ||
                            (0 != State.Gamepad.bAnalogButtons[0]) ||
                            (0 != State.Gamepad.bAnalogButtons[1]) ||
                            (0 != State.Gamepad.bAnalogButtons[2]) ||
                            (0 != State.Gamepad.bAnalogButtons[3]) ||
                            (0 != State.Gamepad.bAnalogButtons[4]) ||
                            (0 != State.Gamepad.bAnalogButtons[5]) ||
                            (0 != State.Gamepad.bAnalogButtons[6]) ||
                            (0 != State.Gamepad.bAnalogButtons[7]))
                        {
                            fButtonPressed = TRUE;
                        }
                    }

                    XInputClose(hPad);
                }
            }

            if (fButtonPressed)
            {
                break;
            }
        }
    }
}

#define NOHAWK 0xFF

void
pHandleInitialHawks (
    IN OUT XMediaObject ** MicrophoneXmo,
    IN OUT XMediaObject ** SpeakerXmo,
    OUT int * MicrophonePort
    )
{

    DWORD dwConnectedMicrophones, dwConnectedHeadphones;
    int i;
    HRESULT result;

    //Sleep (3000);
    //
    //  Get the initial connected state of the devices
    //
    dwConnectedMicrophones = XGetDevices(XDEVICE_TYPE_VOICE_MICROPHONE);
    dwConnectedHeadphones = XGetDevices(XDEVICE_TYPE_VOICE_HEADPHONE);
    if( dwConnectedMicrophones != dwConnectedHeadphones)
    {
        DebugPrint("dwConnectedMicrophones != dwConnectedHeadphone\n");
        _asm int 3;
    }

    //
    // Create XMOs for any already connect units.
    //
    DebugPrint ("Mikes/Heads 0x%08x 0x%08x\n", dwConnectedMicrophones, dwConnectedHeadphones);
    for (i=0;i<4;i++) {

        if(dwConnectedMicrophones&(1<<i) && *MicrophonePort == NOHAWK) {

            DebugPrint ("Opening hawk in port %u.\n", i);
            result = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_MICROPHONE, i , 2, NULL, MicrophoneXmo);
            if (!SUCCEEDED(result)) {
                DebugPrint ("Failed to create Mike XMO. %s(%d): 0x%0.8x\n", __FILE__, __LINE__, result);
            }
            else {
                XDBGTRC ("HAWKTEST", "HawkTest: Created XMO for Microphone device in port %u", i);
                *MicrophonePort = i;
            }

            result = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_HEADPHONE, i , 2, NULL, SpeakerXmo);
            if (!SUCCEEDED(result)) {
                DebugPrint ("Failed to create Speakerx XMO. %s(%d): 0x%0.8x\n", __FILE__, __LINE__, result);
            }
            else {
                XDBGTRC ("HAWKTEST", "HawkTest: Created XMO for Speaker device in port %u", i);
            }

        }
    }
}

void
pHandleHotplugs (
    IN OUT XMediaObject ** MicrophoneXmo,
    IN OUT XMediaObject ** SpeakerXmo,
    OUT int * MicrophonePort
    )
{
    DWORD dwMicrophoneInsertions, dwHeadphoneInsertions, dwMicrophoneRemovals, dwHeadphoneRemovals;
    int i;
    HRESULT result;

    if(XGetDeviceChanges(XDEVICE_TYPE_VOICE_MICROPHONE, &dwMicrophoneInsertions, &dwMicrophoneRemovals))
    {
        if(!XGetDeviceChanges(XDEVICE_TYPE_VOICE_HEADPHONE, &dwHeadphoneInsertions, &dwHeadphoneRemovals))
        {
            DebugPrint("GetDeviceChanges does not match\n");
            _asm int 3;
        }
        if(
            (dwMicrophoneInsertions != dwHeadphoneInsertions) ||
            (dwMicrophoneRemovals != dwHeadphoneRemovals)
        ){
            DebugPrint("GetDeviceChanges does not match, different\n");
            _asm int 3;
        }

        //
        //  Loop over all the ports
        //

        DebugPrint ("Hotplug event occurred.\n");
        for(i=0; i<4; i++)
        {
            //
            //  Handle Removal, if there is one.
            //

            if(dwMicrophoneRemovals&(1<<i) && i == *MicrophonePort)
            {
                DebugPrint ("Closing hawk in port %u", *MicrophonePort);
                XMediaObject_Release ((*MicrophoneXmo));
                XMediaObject_Release ((*SpeakerXmo));
                *MicrophonePort = NOHAWK;

            }

            //
            //  Handle Insertion. Ignore if we already have an opened hawk
            //

            if(dwMicrophoneInsertions&(1<<i) && *MicrophonePort == NOHAWK)
            {
                DebugPrint ("Opening hawk in port %u.\n", i);
                result = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_MICROPHONE, i , 2, NULL, MicrophoneXmo);
                if (!SUCCEEDED(result)) {
                    DebugPrint ("Failed to create Mike XMO. %s(%d): 0x%0.8x\n", __FILE__, __LINE__, result);
                }
                else {
                    XDBGTRC ("HAWKTEST", "HawkTest: Created XMO for Microphone device in port %u", i);
                    *MicrophonePort = i;
                }

                result = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_HEADPHONE, i , 2, NULL, SpeakerXmo);
                if (!SUCCEEDED(result)) {
                    DebugPrint ("Failed to create Mike XMO. %s(%d): 0x%0.8x\n", __FILE__, __LINE__, result);
                }
                else {
                    XDBGTRC ("HAWKTEST", "HawkTest: Created XMO for Speaker device in port %u", i);
                }

            }
        }
    }
}

void
pRunTest (void) {

    XMediaObject * speakerXmo;
    XMediaObject * microphoneXmo;
    int microphonePort = NOHAWK;



    DrawMessage(100, 100, TEXT("Press any gamepad button"), -1, 0 , 0);
    WaitForAnyButton();


    //
    // Get initial hawk connectivity.
    //
    pHandleInitialHawks (&microphoneXmo, &speakerXmo, &microphonePort);



    //
    // Loop forever.
    //
    XDBGTRC ("HAWKTEST", "HawkTest: Entering Test Loop\n");
    while (1) {

        //
        // Check for hotplug events.
        //
        pHandleHotplugs (&microphoneXmo, &speakerXmo, &microphonePort);


        if (microphonePort != NOHAWK) {

            DrawMessage (150, 150, TEXT("We've got a hawk!"), -1, 0, 0);
        }
    }
}



int
WINAPI
wWinMain(
    HINSTANCE hInst,
    HINSTANCE hPrev,
    LPWSTR lpCmdLine,
    int nCmdShow)
{


    XDBGTRC("HAWKTEST", "HawkTest: Built on %s at %s", __DATE__,__TIME__);
    XDBGTRC("HAWKTEST", "HawkTest: Polling for device insertions/removals..");

    pRunTest ();


    return 0;
}

