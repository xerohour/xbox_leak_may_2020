#ifndef NVWIN32_INCLUDED
#define NVWIN32_INCLUDED
 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NV32_INCLUDED
#include "nv32.h"                       /* for Nv3/4/10 Channel typedef */
#endif

#define NV_SLEEP     Sleep(0)
#define NV_SLEEP1    Sleep(1)
#define NV_SLEEP2    Sleep(2)
#define NV_SLEEP3    Sleep(3)
#define NV_SLEEP4    Sleep(4)
#define NV_SLEEP5    Sleep(5)
#define NV_SLEEPFLIP Sleep(1)

/***************************************************************************
 *                   NVIDIA-supplied routines                              *
 ***************************************************************************/

/***** Hardware access *****/

/*
 * NvOpen() opens a 64KB channel (shared memory region) to the chip.
 * Pass NULL as the parameter for the default NV1 chip in the computer.
 */
extern NvChannel * __stdcall NvOpen(char *);

#ifdef NOTHERENOTNOW

// this is commented out because somebody defined it else where to get their stuff working even
// though the SDK has it defined here...  ah well so much for standardization!
/*
 * NvOpenDma() opens a 64KB channel (shared memory region) to the chip.
 * Pass NULL as the parameter for the default NV1 chip in the computer.
 */
extern NvChannel * __stdcall NvOpenDma(char *, int, int, int);

#endif //0

/*
 * NvClose() closes the channel to the chip and attempts to
 * clean up all objects created in that channel.
 */
extern int                  __stdcall NvClose(NvChannel *);

/***** Operating Environment routines *****/
/*
 * NvWait() waits to be "awakened" after requesting notification with
 * the NV_OS_NOTIFY_WILL_SLEEP style.
 * Note that in Windows 3.1 any Windows message will wake up the
 * application.
 */
extern int  __stdcall NvWait( NvChannel *);

/*
 * NvNotifyEvent() specifies a Windows event for NV to send to the
 * application after requesting notifications with the
 * NV_OS_NOTIFY_EVENT style.
 *    first parameter is the NvChannel
 *    second is the Ring0 Win32 event handle
 */
extern int  __stdcall NvNotifyEvent( NvChannel *, int);

/*
 * NvNotifyMsg() specifies a Windows message for NV1 to send to the
 * application after requesting notifications with the
 * NV_OS_NOTIFY_MESSAGE style.
 *    first parameter is the NvChannel
 *    second is the HWND window handle
 *    third is the Windows message that the Resource Manager should send
 */
extern int  __stdcall NvNotifyMsg(NvChannel *, int, int);

/*
 * NvAllocEvent() specifies a Windows event or callback for NV to send to the
 * application after requesting notifications with the
 * NV_OS_WRITE_THEN_AWAKEN style.
 *    first parameter is the NvChannel
 *    second is the object name
 *    third is the notify index
 *    fourth is the notify event type (NV_OS_EVENT_*)
 *    fifth is the lower 32bits of the 64bit event data
 *    sixth is the upper 32bits of the 64bit event data
 */
extern int  __stdcall NvAllocEvent(NvChannel *, int, int, int, int, int);

/*
 * NvErrorMsg() specifies a Windows message for NV to send to the
 * application upon detecting an error.
 *    first parameter is the NvChannel
 *    second is the HWND window handle
 *    third is the Windows message that the Resource Manager should send
 */
extern int  __stdcall NvErrorMsg(NvChannel *, int, int);

/*
 * Although Win32 applications have a flat address space, on the X86 
 * architecture they still have a single selector that you must supply as
 *    contextDmaToMemory.SetDmaSpecifier.address[1].
 */
extern int  __stdcall NvGetFlatDataSelector(void);
extern int  __stdcall NvGetFlatCodeSelector(void);

/*
 * NvConfigVersion() returns an NV version number indicating the NV version.
 * The format of this is 0x00010206, where
 *  - 0x0001 is the chip architecture
 *  -   0x02 is the software release
 *  -   0x06 is the minor revision
 */
extern int  __stdcall NvConfigVersion(void);

#define NV_CHIP_ARCH(ver)   ((ver) >> 16)
#define NV_SW_RELEASE(ver)  (((ver) >> 8) & 0xFF)
#define NV_MINOR_REV(ver)   ((ver) & 0xFF)



/***************************************************************************
 *                          Constants                                      *
 ***************************************************************************/

/***** OS-dependent constants *****/

/* This posts a new architecture event */
#define NV_OS_WRITE_THEN_AWAKEN	   0x00000001

/* This puts the process to sleep awaiting notification */
#define NV_OS_NOTIFY_WILL_SLEEP	   0x00000003

/* This puts a message in the client's message queue upon notification */
#define NV_OS_NOTIFY_MESSAGE	   0x00000002

/* This posts a Win32 event to the client upon notification */
#define NV_OS_NOTIFY_EVENT  	   0x00000004

/* This posts a Ring0 event to the client upon notification */
#define NV_OS_NOTIFY_VM_EVENT  	   0x00000005

/* Event Types */
#define NV_OS_EVENT_VXD_CALLBACK   0x00000000
#define NV_OS_EVENT_WIN32_EVENT    0x00000001
#define NV_OS_EVENT_VM_EVENT       0x00000002
#define NV_OS_EVENT_WIN32_MSG      0x00000003
#define NV_OS_EVENT_WIN16          0x00000004
#define NV_OS_EVENT_INVALID        0xFFFFFFFF


/***************************************************************************
 *                     OS Resource Strings                                 *
 ***************************************************************************/

/* Maximum length of an OS string (conservative, actual is about 24) */
#define NV_OS_MAX_NAME_LENGTH      32

/***** OS resource names for sources and sinks of samples *****/

/* for NV_VIDEO_SINK */
#define NV_OS_VIDEO_MONITOR        "CON:"

/* for NV_IMAGE_TO_VIDEO. The HWND window handle should replace %u */
#define NV_OS_CANVAS_WINDOW        "WND%u:"

/* for NV_AUDIO_SINK */
#define NV_OS_LINE_OUT_LEFT        "LIN:\\LEFT"
#define NV_OS_LINE_OUT_RIGHT       "LIN:\\RIGHT"

/* for NV_AUDIO_SOURCE */
#define NV_OS_LINE_IN_LEFT         "LIN:\\LEFT"
#define NV_OS_LINE_IN_RIGHT        "LIN:\\RIGHT"
#define NV_OS_MIC_IN_LEFT          "MIC:\\LEFT"
#define NV_OS_MIC_IN_RIGHT         "MIC:\\RIGHT"


/***** Many devices can provide samples for NV_GAME_PORT_SOURCE *****/

/* Raw potentiometer data on analog connector from NV_GAME_PORT_SOURCE. */
/* One of the 8 channels (0-7) replaces %d */
/* NOTE: The NV_OS_POT O/S string changed to "POTA%d:". The 2.0 and higher
 * Resource Manager will generate NV_ERROR1_NO_SUCH_OBJECT if there is no 
 * joystick connected. This string means nothing to the 1.20 Resource Manager.
 * For backwards compatibility, use NV_OS_GAME_PORT ("POT%d:"): both the 2.0 
 * and 1.20 Resource Manager will not generate an error if no joystick is
 * connected.
 */
#define NV_OS_POT                      "POTA%d:"
/* XXX Old name preserved for compatibility, use NV_OS_POT */
#define NV_OS_GAME_PORT                "POT%d:"


/* Calibrated joystick data on analog connector from NV_GAME_PORT_SOURCE. */
/* One of the 2 channels (0-1) replaces %d */
/* XXX Not yet implemented */
#define NV_OS_JOYSTICK_BUTTONS         "JOY%d:\\BUTTONS"

#define NV_OS_JOYSTICK_BUTTON_TRIGGER  0x0001
#define NV_OS_JOYSTICK_BUTTON_SELECT   0x0002
#define NV_OS_JOYSTICK_BUTTON_A        0x0004
#define NV_OS_JOYSTICK_BUTTON_B        0x0008

#define NV_OS_JOYSTICK_X_AXIS          "JOY%d:\\X_AXIS"
#define NV_OS_JOYSTICK_Y_AXIS          "JOY%d:\\Y_AXIS"


/* Pedal data on analog connector from NV_GAME_PORT_SOURCE. */
/* One of the 2 channels (0-1) replaces %d */
/* XXX Not yet implemented */
#define NV_OS_PEDALS                   "PEDALS%d:"


/* Throttle data on analog connector from NV_GAME_PORT_SOURCE. */
/* One of the 2 channels (0-1) replaces %d */
/* XXX Not yet implemented */
#define NV_OS_THROTTLE                 "THROTTLE%d:"


/* 8-button game pad data on Sega Saturn connector from NV_GAME_PORT_SOURCE. */
/* One of the 6 channels (0-5) replaces %d */
#define NV_OS_PAD_BUTTONS              "PAD%d:"
/* XXX Old name preserved for compatibility, use NV_OS_PAD_BUTTONS */
#define NV_OS_PAD                      "PAD%d:"

#define NV_OS_PAD_BUTTON_START         0x0001
#define NV_OS_PAD_BUTTON_RIGHT         0x0002
#define NV_OS_PAD_BUTTON_LEFT          0x0004
#define NV_OS_PAD_BUTTON_UP            0x0008
#define NV_OS_PAD_BUTTON_DOWN          0x0010
#define NV_OS_PAD_BUTTON_A             0x0020
#define NV_OS_PAD_BUTTON_B             0x0040
#define NV_OS_PAD_BUTTON_C             0x0080
#define NV_OS_PAD_BUTTON_X             0x0100
#define NV_OS_PAD_BUTTON_Y             0x0200
#define NV_OS_PAD_BUTTON_Z             0x0400
#define NV_OS_PAD_BUTTON_TOP_RIGHT     0x0800
#define NV_OS_PAD_BUTTON_TOP_LEFT      0x1000


/* 3-axis yoke data on Sega Saturn connector from NV_GAME_PORT_SOURCE. */
/* One of the 6 channels (0-5) replaces %d */
#define NV_OS_YOKE_BUTTONS             "YOKE%d:\\BUTTONS"

#define NV_OS_YOKE_BUTTON_START        0x0001
#define NV_OS_YOKE_BUTTON_RIGHT        0x0002
#define NV_OS_YOKE_BUTTON_LEFT         0x0004
#define NV_OS_YOKE_BUTTON_UP           0x0008
#define NV_OS_YOKE_BUTTON_DOWN         0x0010
#define NV_OS_YOKE_BUTTON_A            0x0020
#define NV_OS_YOKE_BUTTON_B            0x0040
#define NV_OS_YOKE_BUTTON_C            0x0080
#define NV_OS_YOKE_BUTTON_X            0x0100
#define NV_OS_YOKE_BUTTON_Y            0x0200
#define NV_OS_YOKE_BUTTON_Z            0x0400
#define NV_OS_YOKE_BUTTON_TOP_RIGHT    0x0800
#define NV_OS_YOKE_BUTTON_TOP_LEFT     0x1000

#define NV_OS_YOKE_X_AXIS              "YOKE%d:\\X_AXIS"
#define NV_OS_YOKE_Y_AXIS              "YOKE%d:\\Y_AXIS"
#define NV_OS_YOKE_Z_AXIS              "YOKE%d:\\Z_AXIS"

/* 2-axis gun position data on Sega Saturn connector from NV_GAME_PORT_SOURCE. */
/* One of the 2 channels (0-1) replaces %d */
/* The HWND window handle should replace %u */
#define NV_OS_GUN_BUTTONS              "GUN%dWND%u:\\BUTTONS"

#define NV_OS_GUN_BUTTON_START         0x0001
#define NV_OS_GUN_BUTTON_TRIGGER	   0x0002

#define NV_OS_GUN_X_POSITION		   "GUN%dWND%u:\\X_POS"
#define NV_OS_GUN_Y_POSITION		   "GUN%dWND%u:\\Y_POS"

/* Value returned if gun is still determing X/Y coordinate */
#define NV_OS_GUN_POSITION_COUNTING    0xFFFE
/* Value returned if gun is off screen or outside canvas) */
#define NV_OS_GUN_POSITION_OFF_SCREEN  0xFFFF

/***** WATCOM special handling *****/
/* WATCOM 10.0 compiler needs special declaration of these functions */
#ifdef __WATCOMC__
#pragma aux (__stdcall) NvOpen "*";
#pragma aux (__stdcall) NvClose "*";
#pragma aux (__stdcall) NvWait "*";
#pragma aux (__stdcall) NvNotifyMsg "*";
#pragma aux (__stdcall) NvErrorMsg "*";
#pragma aux (__stdcall) NvGetFlatDataSelector "*";
#pragma aux (__stdcall) NvGetFlatCodeSelector "*";
#ifdef _WIN32  // don't do this for the DOS compile!!
#pragma aux (__stdcall) NvConfigVersion "*";
#endif
#endif /* __WATCOMC__ */

#ifdef __cplusplus
};
#endif
#endif /* NVWIN32_INCLUDED */

