#ifndef _STDAFX_H_
#define _STDAFX_H_

// warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
// warning C4244: 'argument' : conversion from 'unsigned short' to 'unsigned char', possible loss of data
// warning C4242: '=' : conversion from 'unsigned int' to 'unsigned char', possible loss of data
// warning C4018: '<' : signed/unsigned mismatch
// warning C4146: unary minus operator applied to unsigned type, result still unsigned
#pragma warning( disable : 4800 4244 4242 4018 4146 )  

/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
#include <xtl.h>
#include <xdbg.h>
#include "videoconstants.h"
#include "xboxvideo.h"
#include "usbmanager.h"
#include "linkedlist.h"
#include "xdcitem.h"
#include "menuscreen.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "newstream.h"
#include "sprite.h"
#include "string1.h"
#include "bspf.h"
#include "Booster.h"
#include "Cart.h"
#include "Cart2K.h"
#include "Cart3F.h"
#include "Cart4K.h"
#include "CartAR.h"
#include "CartE0.h"
#include "CartE7.h"
#include "CartF4SC.h"
#include "CartF6.h"
#include "CartF6SC.h"
#include "CartF8.h"
#include "CartF8SC.h"
#include "CartFASC.h"
#include "CartFE.h"
#include "CartMC.h"
#include "Console.h"
#include "Control.h"
#include "D6502.h"
#include "DefProps.h"
#include "Device.h"
#include "Driving.h"
#include "Event.h"
#include "Joystick.h"
#include "Keyboard.h"
#include "M6502.h"
#include "M6502Low.h"
#include "M6502Hi.h"
#include "M6532.h"
#include "MD5.h"
#include "MediaSrc.h"
#include "NullDev.h"
#include "Paddles.h"
#include "Props.h"
#include "PropsSet.h"
#include "Random.h"
#include "Sound.h"
#include "Switches.h"
#include "System.h"
#include "TIA.h"
#include "SoundXBOX.h"

//Definitions
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ  | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#define BREAK_INTO_DEBUGGER     _asm { int 3 }
#define ARRAYSIZE(a)            (sizeof(a) / sizeof(a[0]))

#define DATA_DRIVE              L"t:"
#define DATA_DRIVE_A             "t:"
#define DATA_PATH               DATA_DRIVE   L"\\media"
#define DATA_PATH_A             DATA_DRIVE_A  "\\media"
#define GAME_PATH               DATA_DRIVE   L"\\games"
#define GAME_PATH_A             DATA_DRIVE_A  "\\games"
#define ROM_EXT                 L"bin"
#define ROM_EXT_A               "bin"

extern "C" {ULONG DebugPrint(PCHAR Format, ...);}         

#endif // _STDAFX_H_
