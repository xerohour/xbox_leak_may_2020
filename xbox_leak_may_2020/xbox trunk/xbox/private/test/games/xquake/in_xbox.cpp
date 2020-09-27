/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// in_xbox.c -- xbox game controller code

#include <xtl.h>
#include <xbdm.h>
#include "quakedef.h"
#include "xquake.h"
#include "xgc.h"

CGameControllers *g_pGameControllers;
XINPUT_STATE      g_GameControllerState;
DWORD             g_dwGcCmdUp;
DWORD             g_dwGcCmdDown;
BOOL              g_fInitDone = FALSE;

extern "C"
{
cvar_t	gcforwardthreshold = {"gcforwardthreshold", "0.15"};
cvar_t	gcsidethreshold = {"gcsidethreshold", "0.15"};
cvar_t	gcpitchthreshold = {"gcpitchthreshold", "0.15"};
cvar_t	gcyawthreshold = {"gcyawthreshold", "0.15"};
cvar_t	gcbuttonthreshold = {"gcbuttonthreshold", "50"};
cvar_t	gcforwardsensitivity = {"gcforwardsensitivity", "1.0"};
cvar_t	gcsidesensitivity = {"gcsidesensitivity", "1.0"};
cvar_t	gcpitchsensitivity = {"gcpitchsensitivity", "1.0"};
cvar_t	gcyawsensitivity = {"gcyawsensitivity", "-1.0"};
cvar_t	gcswaphorizontalthumbs = {"gcswaphorizontalthumbs", "0"};
cvar_t	gcswapverticalthumbs = {"gcswapverticalthumbs", "0"};
}

/*
===========
IN_Init
===========
*/
void IN_Init (void)
{
    //Initialize core peripheral port support
    XInitDevices(0,NULL);

#ifdef DEBUG_KEYBOARD
    XINPUT_DEBUG_KEYQUEUE_PARAMETERS parameters;
    memset(&parameters,0,sizeof(parameters));
    parameters.dwFlags = XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN | XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT | XINPUT_DEBUG_KEYQUEUE_FLAG_KEYUP;
    parameters.dwQueueSize = 32;
    parameters.dwRepeatDelay = 400; // ms
    parameters.dwRepeatInterval = 150; // ms
    XInputDebugInitKeyboardQueue(&parameters);
#endif // DEBUG_KEYBOARD

    Cvar_RegisterVariable (&gcforwardthreshold);
	Cvar_RegisterVariable (&gcsidethreshold);
	Cvar_RegisterVariable (&gcpitchthreshold);
	Cvar_RegisterVariable (&gcyawthreshold);
    Cvar_RegisterVariable (&gcbuttonthreshold);
	Cvar_RegisterVariable (&gcforwardsensitivity);
	Cvar_RegisterVariable (&gcsidesensitivity);
	Cvar_RegisterVariable (&gcpitchsensitivity);
	Cvar_RegisterVariable (&gcyawsensitivity);
    Cvar_RegisterVariable (&gcswaphorizontalthumbs);
    Cvar_RegisterVariable (&gcswapverticalthumbs);
    g_dwGcCmdUp = 0;
    g_dwGcCmdDown = 0;
    memset(&g_GameControllerState, 0, sizeof(XINPUT_STATE));
    g_pGameControllers = new CGameControllers;

    g_fInitDone = TRUE;
}


/*
===========
IN_Shutdown
===========
*/
void IN_Shutdown (void)
{
    delete g_pGameControllers;
}

#ifdef DEBUG_KEYBOARD
byte        xvkToQuake[256] = {
//  0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F
    0,      0,      0,      0,      0,      0,      0,      0,      K_BACKSPACE, 0, 0,      0,      0,      13,     0,      0,   // 0    
    K_SHIFT,K_CTRL, K_ALT,  K_PAUSE,0,      0,      0,      0,      0,      0,      0,      27,     0,      0,      0,      0,   // 1    
    ' ',    K_PGDN, K_PGUP, K_END,  K_HOME, K_LEFTARROW,K_UPARROW,K_RIGHTARROW,K_DOWNARROW,0,0,0,   0,      K_INS,  K_DEL,  0,   // 2    
    '0',    '1',    '2',    '3',    '4',    '5',    '6',    '7',    '8',    '9',    0,      0,      0,      0,      0,      0,   // 3    
    0,      'a',    'b',    'c',    'd',    'e',    'f',    'g',    'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o', // 4    
    'p',    'q',    'r',    's',    't',    'u',    'v',    'w',    'x',    'y',    'z',    0,      0,      0,      0,      0,   // 5    
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,   // 6    
    K_F1,   K_F2,   K_F3,   K_F4,   K_F5,   K_F6,   K_F7,   K_F8,   K_F9,   K_F10,  K_F11,  K_F12,  0,      0,      0,      0,   // 7    
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,   // 8    
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,   // 9    
    K_SHIFT,K_SHIFT,0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,   // a    
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      ';',    '=',    ',',    '-',    '.',    '/', // b    
    '`',    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,   // c    
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      '[',    '\\',   ']',    '\'',   0,   // d    
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,   // e    
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0    // f  
};

/*
=======
MapKey

Map from Xbox to quake keynums
=======
*/
int MapKey (int key)
{
	if (key < 0 || key > 255)
		return 0;
	if (xvkToQuake[key] == 0)
		Con_DPrintf("key 0x%02x has no translation\n", key);
	return xvkToQuake[key];
}

#endif

/*
===========
IN_Move
===========
*/
void IN_Move (usercmd_t *cmd)
{
    XINPUT_STATE newState;

    //
    //  Read the game controller packet
    //  and make sure that it was received.
    //
    if(!g_fInitDone)
    {
        return;
    }

#ifdef DEBUG_KEYBOARD
    {
        XINPUT_DEBUG_KEYSTROKE keystroke;
        memset(&keystroke,0,sizeof(keystroke));
        XInputDebugGetKeystroke(&keystroke);
        if(keystroke.VirtualKey){
            Con_Printf ("Got keystroke virtualKey: 0x%08x flags: 0x%08x ascii: 0x%08x\n", keystroke.VirtualKey, keystroke.Flags, keystroke.Ascii);
            bool keyDown = ( keystroke.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_KEYUP ) != 0;
            Key_Event(MapKey(keystroke.VirtualKey), keyDown);
        }
    }
#endif // DEBUG_KEYBOARD

    if(FALSE == g_pGameControllers->GetState(&newState))
    {
        return;
    }

    //
    //  First process buttons up and buttons down.
    //  We would send these until IN_Commands
    //  but we will preprocess them here.
    //
    if(newState.dwPacketNumber != g_GameControllerState.dwPacketNumber)
    {
        //
        //  Mark keyup and keydown events for the first eight
        //  buttons (digital)
        //

        g_dwGcCmdDown = (newState.Gamepad.wButtons & ~g_GameControllerState.Gamepad.wButtons) & 0xFF;
        g_dwGcCmdUp = (~newState.Gamepad.wButtons & g_GameControllerState.Gamepad.wButtons) & 0xFF;

        for(int i=0; i<8; i++)
        {
           if(
               (newState.Gamepad.bAnalogButtons[i] > gcbuttonthreshold.value) &&
               (g_GameControllerState.Gamepad.bAnalogButtons[i] <= gcbuttonthreshold.value)
           ) {
                g_dwGcCmdDown |= ((0x100) << i);
           } else if (
               (newState.Gamepad.bAnalogButtons[i] <= gcbuttonthreshold.value) &&
               (g_GameControllerState.Gamepad.bAnalogButtons[i] > gcbuttonthreshold.value)
               )
           {
                g_dwGcCmdUp |= ((0x100) << i);
           }
        }

        //
        //  Now copy the new state over the existing one
        //
        memcpy(&g_GameControllerState, &newState, sizeof(XINPUT_STATE));
    }


    //
    //  Handle the axes now.
    //

    float rAxisValue, speed, aspeed;
    if (in_speed.state & 1)
		speed = cl_movespeedkey.value;
	else
		speed = 1;
	aspeed = speed * host_frametime;

    //
    //  Right Thumb X (is turn) - unless swap = 1
    //
    if(1.0==gcswaphorizontalthumbs.value)
    {
        rAxisValue = ((float)newState.Gamepad.sThumbLX) / 32768.0;
    } else
    {
        rAxisValue = ((float)newState.Gamepad.sThumbRX) / 32768.0;
    }
    if (fabs(rAxisValue) > gcyawthreshold.value)
	{
       cl.viewangles[YAW] += (rAxisValue * gcyawsensitivity.value) * aspeed * cl_yawspeed.value;
    }

    //
    //  Right Thumb Y (is look) - unless swap = 1
    //
    if(1.0==gcswapverticalthumbs.value)
    {
        rAxisValue = ((float)newState.Gamepad.sThumbLY) / 32768.0;
    } else
    {
        rAxisValue = ((float)newState.Gamepad.sThumbRY) / 32768.0;
    }
    if (fabs(rAxisValue) > gcpitchthreshold.value)
	{
	    cl.viewangles[PITCH] += (rAxisValue * gcpitchsensitivity.value) * aspeed * cl_pitchspeed.value;
	    V_StopPitchDrift();
    } else
    {
	    if(lookspring.value == 0.0) V_StopPitchDrift();
	}

    // bounds check pitch
	if (cl.viewangles[PITCH] > 80.0)
		cl.viewangles[PITCH] = 80.0;
	if (cl.viewangles[PITCH] < -70.0)
		cl.viewangles[PITCH] = -70.0;

    //
    //  Left Thumb X (is side-to-side - strafe) - unless swap = 1
    //
    if(1.0==gcswaphorizontalthumbs.value)
    {
        rAxisValue = ((float)newState.Gamepad.sThumbRX) / 32768.0;
    } else
    {
        rAxisValue = ((float)newState.Gamepad.sThumbLX) / 32768.0;
    }
    if (fabs(rAxisValue) > gcsidethreshold.value)
    {
        cmd->sidemove += (rAxisValue * gcsidesensitivity.value) * speed * cl_sidespeed.value;
    }

    //
    //  Right Thumb Y (is forward-back) - unless swap = 1
    //
    if(1.0==gcswapverticalthumbs.value)
    {
        rAxisValue = ((float)newState.Gamepad.sThumbRY) / 32768.0;
    } else
    {
        rAxisValue = ((float)newState.Gamepad.sThumbLY) / 32768.0;
    }
    if (fabs(rAxisValue) > gcforwardthreshold.value)
    {
        cmd->forwardmove += (rAxisValue * gcforwardsensitivity.value) * speed * cl_forwardspeed.value;
    }
}


/*
===================
IN_ClearStates
===================
*/
void IN_ClearStates (void)
{
    memset(&g_GameControllerState, 0, sizeof(XINPUT_STATE));
    g_dwGcCmdUp = 0;
    g_dwGcCmdDown = 0;
}

#ifdef _XBOX
bool RXCmdHandleRCmds();
#endif

/*
===========
IN_Commands
===========
*/
void IN_Commands (void)
{
	if(!g_fInitDone)
    {
        return;
    }
    //
    //  Send key events for all of the buttons
    //
    for(int i=0; i < 16; i++)
    {
        if(g_dwGcCmdUp & (1 << i))
        {
            Key_Event (K_GC_BASE+i, false);
        } else if(g_dwGcCmdDown & (1 << i))
        {
            Key_Event (K_GC_BASE+i, true);
        }
    }

    RXCmdHandleRCmds();
}

#ifdef _XBOX

// our critical section
bool g_fcsRxcmdInited = false;
CRITICAL_SECTION g_csRxcmd;

// command prefix
static const char g_szCmd[] = "x";

// processorproc buffer
#define CCH_MAXCMDLEN       256
static char g_szRemoteBuf[CCH_MAXCMDLEN];

//=========================================================================
// Name: RXCmdPRocessorProc
// Desc: Cmd notification proc
//
//  This is what is called by the Xbox debug monitor to have us process
//  a command.
//=========================================================================
HRESULT __stdcall RXCmdProcessorProc(LPCSTR szCommand, LPSTR szResp,
    DWORD cchResp, PDM_CMDCONT pdmcc)
{
    // skip over prefix
    char *szCmd = (char *)szCommand + lstrlenA(g_szCmd) + 1;

    // g_szRemoteBuf needs to be protected by the critical section
    EnterCriticalSection(&g_csRxcmd);

    if(g_szRemoteBuf[0])
        Q_strncpy(szResp, "Cannot execute - previous command still pending", cchResp);
    else
    {
        Q_strncpy(szResp, "Ack.", cchResp);
        Q_strncpy(g_szRemoteBuf, szCmd, sizeof(g_szRemoteBuf) / sizeof(g_szRemoteBuf[0]));
    }

    LeaveCriticalSection(&g_csRxcmd);
    return XBDM_NOERR;
}

//=========================================================================
// Name: RXCmdHandleRCmds
// Desc: Poll routine called by app
//
//  The application should call this function periodically, usually every
//  frame.  This will check to see if there is a command waiting to be
//  executed, and if so, execute it.
//=========================================================================
bool RXCmdHandleRCmds()
{
    static BOOL fRegistered = false;
    char        cmd[CCH_MAXCMDLEN];

    // Initialize ourselves when we're first called.
    if(!fRegistered)
    {
        // init our critical section
        if(!g_fcsRxcmdInited)
        {
            InitializeCriticalSection(&g_csRxcmd);
            g_fcsRxcmdInited = true;
        }

        // register our command handler with the debug monitor
        HRESULT hr = DmRegisterCommandProcessor(g_szCmd, RXCmdProcessorProc);
        if(FAILED(hr))
            return false;

        fRegistered = true;
    }

    // If there's nothing waiting, return.
    if(!g_szRemoteBuf[0])
        return false;

    //
    // Grab a copy of the command to free up
    // the critical section
    //
    EnterCriticalSection(&g_csRxcmd);

    lstrcpyA(cmd, g_szRemoteBuf);
    g_szRemoteBuf[0] = 0;

    LeaveCriticalSection(&g_csRxcmd);

    // go through and handle all backspace characters (0x8)
    int ichcur = 0;
    for(int ich = 0; cmd[ich]; ich++)
    {
        if(cmd[ich] == 0x8)
        {
            if(ichcur)
                ichcur--;
        }
        else
        {
            cmd[ichcur++] = cmd[ich];
        }
    }
    cmd[ichcur] = 0;

    if(cmd[0] == 0x3)
    {
        // if prefixed by the ctrl+c character then send as key events
        for(int ich = 1; cmd[ich]; ich++)
        {
            Key_Event(cmd[ich], true);
            Key_Event(cmd[ich], false);
        }
    }
    else
    {
        Con_Printf("%s\n", cmd);
        Cbuf_AddText(cmd);
    }
    return true;
}

#endif

