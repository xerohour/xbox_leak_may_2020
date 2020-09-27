#ifndef _VNVRMD_H_
#define _VNVRMD_H_
/********* Operating System Interface for NVidia Resource Manager **********\
*                                                                           *
* Module: NVRM.H                                                            *
*       Header file for NVRM defines and service exports.                   *
*                                                                           *
*---------------------------------------------------------------------------*
*                                                                           *
* History:                                                                  *
*        David Schmenk (dschmenk)  06/21/94 - wrote it                      *
*        Jeff Westerinen (jsw)     05/05/97 - Modified to build under MS    *
*                                                                           *
\*********************** Copyright 1994 NVidia, Inc. ***********************/

//
// Version numbers.
//

#define NVRM_Major_Version  1
#define NVRM_Minor_Version  0

//
// Device IDs.
//

#define NVRM_Device_ID      0x32DC

//
// Init order.
//

//#define NVRM_Init_Order     (VDD_Init_Order - 0x00100000)	// init before VDD  (jsw)
#define NVRM_Init_Order     (VDD_INIT_ORDER - 0x00100000)	// init before VDD

//
// Virtual device services.
//

#define NVRM_Get_Version        0
#define NVRM_Register           1

//
// Prototypes to call NVRM services.
//

typedef DWORD (CDECL *NVRMSYSAPI)(DWORD, DWORD, DWORD, DWORD);

/* (jsw)

DWORD nvrmGetVersion(VOID);
LONG  nvrmRegister(DWORD, NVRMSYSAPI);

#pragma aux nvrmGetVersion =                            \
            CALL_VXD(NVRM_Device_ID, NVRM_Get_Version)  \
            parm caller []                              \
            value no8087 [EAX]                          \
            modify [EAX EBX ECX EDX];

#pragma aux nvrmRegister =                          \
            CALL_VXD(NVRM_Device_ID, NVRM_Register) \
            parm caller []                          \
            value no8087 [EAX]                      \
            modify [EAX EBX ECX EDX];

*/

#endif // _VNVRMD_H_

