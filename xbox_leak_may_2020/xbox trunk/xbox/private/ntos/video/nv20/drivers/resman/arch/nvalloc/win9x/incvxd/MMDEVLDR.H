/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/*
 *  MMDEVLDR.H - The main include file for the DevLoader
 *
 *  Version 4.00
 *
 */

#define Multimedia_OEM_ID   0x0440	//            ; MS Reserved OEM # 34
#define MMDEVLDR_DEVICE_ID  Multimedia_OEM_ID + 10 	//;MMDEVLDR's device ID

#ifdef Begin_Service_Table		// define only if vmm.h is included

#define	MMDEVLDR_Service	Declare_Service
#pragma warning (disable:4003)		// turn off not enough params warning

//MACROS
Begin_Service_Table(MMDEVLDR)

MMDEVLDR_Service	(MMDEVLDR_Register_Device_Driver, LOCAL)
MMDEVLDR_Service	(MMDEVLDR_SetDevicePresence)
MMDEVLDR_Service  (MMDEVLDR_SetEnvironmentString)
MMDEVLDR_Service  (MMDEVLDR_GetEnvironmentString)
MMDEVLDR_Service  (MMDEVLDR_RemoveEnvironmentString)
MMDEVLDR_Service  (MMDEVLDR_AddEnvironmentString)

End_Service_Table(MMDEVLDR)
//ENDMACROS       

#pragma warning (default:4003)		// turn on not enough params warning

#pragma warning (disable:4035)		// turn off no return code warning

VOID VXDINLINE MMDEVLDR_Register_Device_Driver
(
    DEVNODE         dnDevNode,
    DWORD           fnConfigHandler,
    DWORD           dwUserData
)
{
    _asm push ebx
    _asm mov eax,dnDevNode
    _asm mov ebx,fnConfigHandler
    _asm mov ecx,dwUserData
    VxDCall(MMDEVLDR_Register_Device_Driver);
    _asm pop ebx
}

VOID VXDINLINE MMDEVLDR_SetDevicePresence
(
    DEVNODE         dn,
    PCHAR           pszReg,
    BOOL            fPresent
)
{
    _asm push fPresent
    _asm push pszReg
    _asm push dn
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    VxDCall(MMDEVLDR_SetDevicePresence);
    _asm add  esp, 3*4
} ;

VOID VXDINLINE MMDEVLDR_SetEnvironmentString
(
    PCHAR           pszName,
    PCHAR           pszValue
)
{
    _asm push pszValue
    _asm push pszName
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    VxDCall(MMDEVLDR_SetEnvironmentString);
    _asm add  esp, 2*4
} ;

BOOL VXDINLINE MMDEVLDR_GetEnvironmentString
(
    PCHAR           pszName,
    PCHAR           pszValue,
    UINT            uBufSize
)
{
    _asm push uBufSize
    _asm push pszValue
    _asm push pszName
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    VxDCall(MMDEVLDR_GetEnvironmentString);
    _asm add  esp, 3*4
} ;

VOID VXDINLINE MMDEVLDR_RemoveEnvironmentString
(
    PCHAR           pszName
)
{
    _asm push pszName
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    VxDCall(MMDEVLDR_RemoveEnvironmentString);
    _asm add  esp, 4
} ;

VOID VXDINLINE MMDEVLDR_AddEnvironmentString
(
    PCHAR           pszName,
    PCHAR           pszValue
)
{
    _asm push pszValue
    _asm push pszName
    Touch_Register(eax)
    Touch_Register(ecx)
    Touch_Register(edx)
    VxDCall(MMDEVLDR_AddEnvironmentString);
    _asm add  esp, 2*4
} ;

#pragma warning (disable:4035)		// turn on no return code warning

#endif // Begin_Service_Table

