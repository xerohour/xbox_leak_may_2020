#ifndef _HARDWARE_H
#define _HARDWARE_H

//
// This structure holds the Intel CPU Serial Number
//
typedef struct _IntelCPUID
{
    DWORD low;
    DWORD mid;
    DWORD high;
} IntelCPUID;


void GetIntelCPUID( IntelCPUID* pCPUID );
HRESULT ProcessInput( void );

// Handle the Joystick Input
void 
HandleInput( enum JOYSTICK joystick, int nThumbY, int nThumbX, bool bFirstYPress, bool bFirstXPress );

// Handles input (of the CONTROLS) for the current menu
void 
HandleInput( enum CONTROLS controlPressed, 
             bool bFirstPress );

// Handles input (of the BUTTONS) for the current menu
void 
HandleInput( enum BUTTONS buttonPressed, 
             bool bFirstPress );



#endif  _HARDWARE_H
