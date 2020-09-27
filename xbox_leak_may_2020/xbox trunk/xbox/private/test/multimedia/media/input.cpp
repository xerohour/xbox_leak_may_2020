/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	input.cpp

Abstract:

	Media stress - User input code

Author:

    Josh Poley (jpoley) 25-June-2001

Environment:

	Xbox only

Revision History:

	25-June-2001 jpoley
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "media.h"
#include "scene.h"
#include "music.h"
#include "input.h"
#include "helpers.h"

using namespace Media;

namespace Media {

extern CScene g_scene;
extern CMusic g_music;

#define STICK_DEAD_ZONE     7000
#define STICK_SCALE_FACTOR  16000.0f

HANDLE hDuke[XGetPortCount()];

static const DWORD XIDMasks[] = {
    XDEVICE_PORT0_MASK,
    XDEVICE_PORT1_MASK,
    XDEVICE_PORT2_MASK,
    XDEVICE_PORT3_MASK
    };

#define MAXPORTS NUMELEMS(XIDMasks)

/* uncomment if we add support for sub devices (such as mus)
static const DWORD XMUMasks[] = {
    XDEVICE_PORT0_TOP_MASK,
    XDEVICE_PORT1_TOP_MASK,
    XDEVICE_PORT2_TOP_MASK,
    XDEVICE_PORT3_TOP_MASK,
    XDEVICE_PORT0_BOTTOM_MASK,
    XDEVICE_PORT1_BOTTOM_MASK,
    XDEVICE_PORT2_BOTTOM_MASK,
    XDEVICE_PORT3_BOTTOM_MASK
    };
*/

//------------------------------------------------------------------------------
//	Local Functions:
//------------------------------------------------------------------------------
static void HandleInput(unsigned port, XINPUT_STATE *state);


#define HOLDTIME 300  //.3 seconds
#define REPEATTIME 30 //once per frame, at 30 FPS
#define UP         0
#define HOLDING    1
#define DOWN       2


void CheckUserInput(void)
    {
    // check for device insertions and removals
    DWORD addDuke, removeDuke;
    XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDuke, &removeDuke);
    InputDukeInsertions(addDuke, removeDuke);

    // check for user input
    XINPUT_STATE state;
    for(unsigned port=0; port<XGetPortCount(); port++)
        {
        if(hDuke[port] == NULL) continue;

        XInputGetState(hDuke[port], &state);
        HandleInput(port, &state);
        }
    }

void InputDukeInsertions(DWORD add, DWORD remove/*=0*/)
    {
    for(unsigned port=0; port<XGetPortCount(); port++)
        {
        if(remove & XIDMasks[port])
            {
            if(hDuke[port]) XInputClose(hDuke[port]);
            hDuke[port] = NULL;
            }
        if(add & XIDMasks[port])
            {
            hDuke[port] = XInputOpen(XDEVICE_TYPE_GAMEPAD, port, 0, NULL);
            }
        }
    }

//------------------------------------------------------------------------------
//	Local Functions:
//------------------------------------------------------------------------------
void HandleInput(unsigned port, XINPUT_STATE *state)
    {
    //State of D-Pad
    static DWORD dwRightState[MAXPORTS]={UP};
    static DWORD dwLeftState[MAXPORTS]={UP};
    static DWORD dwStartTimeRight[MAXPORTS] = {0};
    static DWORD dwStartTimeLeft[MAXPORTS] = {0}; 

    //Needed to keep track not only of positions, but what has changed.    
    static XINPUT_STATE stateOld[MAXPORTS];
    static bFirstIteration[MAXPORTS] = {TRUE};

    BOOL bChange = FALSE;
    DWORD dwTemp = 0;


    //If the first time, make it appear that no changes were made.
    if (bFirstIteration[port])
    {
        stateOld[port] = *state;
        bFirstIteration[port] = FALSE;
    }

    //Log 
    if (state->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP && !(stateOld[port].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP))
    {
        if (--g_TestParams.dwDMSelected == 0xFFFFFFFF)
            g_TestParams.dwDMSelected = NUM_SELECTIONS - 1;
    }

    if (state->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN && !(stateOld[port].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN))
    {
        if (++g_TestParams.dwDMSelected == NUM_SELECTIONS)
            g_TestParams.dwDMSelected = 0;
    }

	//
	// set text state based on button state and previous scene text state
	//
	if ( state->Gamepad.wButtons & XINPUT_GAMEPAD_START && ! ( stateOld[port].Gamepad.wButtons & XINPUT_GAMEPAD_START ) )
	{
		g_scene.SetDrawText( ! g_scene.DrawText() );
	}

    // ZOOM: trigger
    if(state->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 0x0A)
        {
        g_scene.m_camera.Zoom((float)state->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] / 255.0f);
        }
    if(state->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 0x0A)
        {
        g_scene.m_camera.Zoom((float)state->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] / -255.0f);
        }

    // Rotate X Y: left thumb stick
    if(!(state->Gamepad.sThumbLX > -STICK_DEAD_ZONE && state->Gamepad.sThumbLX < STICK_DEAD_ZONE))
        {
        g_scene.m_camera.RotateY(state->Gamepad.sThumbLX/STICK_SCALE_FACTOR);
        }
    if(!(state->Gamepad.sThumbLY > -STICK_DEAD_ZONE && state->Gamepad.sThumbLY < STICK_DEAD_ZONE))
        {
        g_scene.m_camera.RotateX(state->Gamepad.sThumbLY/STICK_SCALE_FACTOR);
        }


	// process DMUSIC input only if text is displayed    
    if ( g_scene.DrawText() )
    {
        //Handle right side of the D-Pad
        if (state->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
        {
            bChange = FALSE;
            if (UP == dwRightState[port])
            {
                bChange = TRUE;
                dwRightState[port] = HOLDING;
                dwStartTimeRight[port] = timeGetTime();
            }
            else if (HOLDING == dwRightState[port])
            {
                if (timeGetTime() - dwStartTimeRight[port] > HOLDTIME)
                {
                    dwStartTimeRight[port] = timeGetTime();
                    bChange = TRUE;
                    dwRightState[port] = DOWN;
                }
            }
            else if (DOWN == dwRightState[port])
            {
                if (timeGetTime() - dwStartTimeRight[port] > REPEATTIME)
                {
                    dwStartTimeRight[port] = timeGetTime();
                    bChange = TRUE;
                }
            }

            if (bChange)
            {
                switch (g_TestParams.dwDMSelected)
                {
			    case 0:
				    g_TestParams.dwNumBalls++;
				    if ( g_TestParams.dwNumBalls > g_TestParams.dwMaxBalls )
					    g_TestParams.dwNumBalls = g_TestParams.dwMaxBalls;

				    g_scene.AddBall();
				    break;

			    case 1:
				    g_TestParams.dwNumLights++;
				    if ( g_TestParams.dwNumLights > g_TestParams.dwMaxLights )
					    g_TestParams.dwNumLights = g_TestParams.dwMaxLights;

				    g_scene.AddLight();
				    break;

			    case 2:
				    g_TestParams.bDrawWireframe = !g_TestParams.bDrawWireframe;
				    break;

			    case 3:
				    g_TestParams.bRelativeSpeed = !g_TestParams.bRelativeSpeed;
				    break;

			    case 4:
				    g_TestParams.bSparksAreLights = !g_TestParams.bSparksAreLights;
				    break;

			    case 5:
				    g_TestParams.dwSparksPerCollision++;
				    break;

                case 6:
                    g_music.NextSegment();
                    break;
                case 7:
                    g_TestParams.lDMMasterVolume += 100;
                    if (g_TestParams.lDMMasterVolume > 0)
                        g_TestParams.lDMMasterVolume = 0;
                    g_music.SetMasterVolume(g_TestParams.lDMMasterVolume);
                    break;
                case 8: 
                    g_TestParams.fDMMasterTempo += 0.1f;
                    if (g_TestParams.fDMMasterTempo > 100.f)
                        g_TestParams.fDMMasterTempo = 100.f;
                    g_music.SetMasterTempo(g_TestParams.fDMMasterTempo);
                    break;
                case 9: 
                    if (g_TestParams.lDMMasterTranspose < 127)
                    {
                        g_TestParams.lDMMasterTranspose += 1;
                        g_music.SetMasterTranspose(g_TestParams.lDMMasterTranspose);
                    }
                    break;
                case 10: 
                    g_music.GetReverb(&dwTemp);
                    if (0xFFFFFFFF == dwTemp)
                        dwTemp = 127;                    
                    if (dwTemp < 127)
                    {
                        dwTemp++;
                        g_music.SetReverb((unsigned short)dwTemp);
                    }
                    break;
                case 11:
                    g_TestParams.lDSVolume += 100;

				    if ( g_TestParams.lDSVolume > 0 )
					    g_TestParams.lDSVolume = 0;

                    break;

                case 12:
                    g_TestParams.dwDSFreq += 100;

				    if ( g_TestParams.dwDSFreq > 96000 )
					    g_TestParams.dwDSFreq = 96000;

                    break;
                }
            }
        }
        else
        {
            dwRightState[port] = UP;
        }

        //Handle left side of the D-Pad.
        if (state->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
        {
            bChange = FALSE;
            if (UP == dwLeftState[port])
            {
                bChange = TRUE;
                dwLeftState[port] = HOLDING;
                dwStartTimeLeft[port] = timeGetTime();
            }
            else if (HOLDING == dwLeftState[port])
            {
                DWORD dwTemp = timeGetTime();

                if (timeGetTime() - dwStartTimeLeft[port] > HOLDTIME)
                {
                    dwStartTimeLeft[port] = timeGetTime();
                    bChange = TRUE;
                    dwLeftState[port] = DOWN;
                }
            }
            else if (DOWN == dwLeftState[port])
            {
                if (timeGetTime() - dwStartTimeLeft[port] > REPEATTIME)
                {
                    dwStartTimeLeft[port] = timeGetTime();
                    bChange = TRUE;
                }
            }
 
            if (bChange)
            {
                switch (g_TestParams.dwDMSelected)
                {
			case 0:
				g_TestParams.dwNumBalls--;
				if ( g_TestParams.dwNumBalls == 0xFFFFFFFF )
					g_TestParams.dwNumBalls = 0;

				g_scene.RemoveBall();
				break;

			case 1:
				g_TestParams.dwNumLights--;
				if ( g_TestParams.dwNumLights == 0xFFFFFFFF )
					g_TestParams.dwNumLights = 0;

				g_scene.RemoveLight();
				break;

			case 2:
				g_TestParams.bDrawWireframe = !g_TestParams.bDrawWireframe;
				break;

			case 3:
				g_TestParams.bRelativeSpeed = !g_TestParams.bRelativeSpeed;
				break;

			case 4:
				g_TestParams.bSparksAreLights = !g_TestParams.bSparksAreLights;
				break;

			case 5:
				g_TestParams.dwSparksPerCollision--;
				if ( g_TestParams.dwSparksPerCollision == 0xFFFFFFFF )
					g_TestParams.dwSparksPerCollision = 0;
				break;

            case 6:
                g_music.NextSegment();
                break;
            case 7:
                g_TestParams.lDMMasterVolume -= 100;
                if (g_TestParams.lDMMasterVolume < -10000)
                    g_TestParams.lDMMasterVolume = -10000;
                g_music.SetMasterVolume(g_TestParams.lDMMasterVolume);
                break;
            case 8: 
                g_TestParams.fDMMasterTempo -= 0.1f;
                if (g_TestParams.fDMMasterTempo < 0.f)
                    g_TestParams.fDMMasterTempo = 0.f;
                g_music.SetMasterTempo(g_TestParams.fDMMasterTempo);
                break;
            case 9: 
                if (g_TestParams.lDMMasterTranspose > -128)
                {
                    g_TestParams.lDMMasterTranspose -= 1;
                    g_music.SetMasterTranspose(g_TestParams.lDMMasterTranspose);
                }
                break;
            case 10: 
                g_music.GetReverb(&dwTemp);
                if (0xFFFFFFFF == dwTemp)
                    dwTemp = 127;                    
                if (dwTemp > 0)
                {
                    dwTemp--;
                    g_music.SetReverb((unsigned short)dwTemp);
                }
                break;

            case 11:
                g_TestParams.lDSVolume -= 100;

				if ( g_TestParams.lDSVolume < -10000 )
					g_TestParams.lDSVolume = -10000;

                break;

            case 12:
                g_TestParams.dwDSFreq -= 100;

				if ( g_TestParams.dwDSFreq < DSBFREQUENCY_MIN || g_TestParams.dwDSFreq > 96000 )
					g_TestParams.dwDSFreq = DSBFREQUENCY_MIN;

                break;
                }
            }
        }
        else
        {
            dwLeftState[port] = UP;
        }
    }


    //Store the last frame's joystick state.
    stateOld[port] = *state;
    }

}