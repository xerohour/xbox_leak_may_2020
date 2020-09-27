////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef VDCOMMON_H
#define VDCOMMON_H

#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include "library\common\vddebug.h"
#include "library\support\aspi32\wnaspi32.h"

typedef struct VDASPIStruct
	{
	HANDLE		event;

	DWORD (__cdecl * xSendASPI32Command )   (LPSRB);
	DWORD (__cdecl * xGetASPI32SupportInfo) (VOID);

	}	*	VDASPIHandle;



#if !WINNT
#define DllEXPORT	__declspec( dllexport )
#else
#define DllEXPORT

#endif


extern "C"
	{
	DllEXPORT void		WINAPI VD32_VOID();
	DllEXPORT HANDLE	WINAPI VD32_OpenFile(const char * name, BOOL readWrite);
	DllEXPORT HANDLE	WINAPI VD32_CreateFile(const char * name);
	DllEXPORT void		WINAPI VD32_CloseFile(HANDLE file);
	DllEXPORT DWORD	WINAPI VD32_ReadFile(HANDLE file, void * data, DWORD size);
	DllEXPORT DWORD	WINAPI VD32_WriteFile(HANDLE file, void * data, DWORD size);
	DllEXPORT void		WINAPI VD32_SeekFile(HANDLE file, DWORD &low, DWORD &high);
	DllEXPORT void		WINAPI VD32_GetFileSize(HANDLE file, DWORD &low, DWORD &high);

	DllEXPORT BOOL		WINAPI VD32_GetPrivateProfileString(char * section, char * entry, char * initBuff, char * buff,
	                                      int size, char * fileName);
	DllEXPORT BOOL		WINAPI VD32_WritePrivateProfileString(char * section, char * entry, char * value, char * fileName);
	DllEXPORT BOOL		WINAPI VD32_GetPrivateProfileDWORD(char * section, char * entry, DWORD init, DWORD * buff,
	                                                 char * fileName);
	DllEXPORT BOOL		WINAPI VD32_WritePrivateProfileDWORD(char * section, char * entry, DWORD value, char * fileName);

#if !WINNT
	DllEXPORT void		WINAPI VD32_Sleep(DWORD millisecs);
	DllEXPORT VDASPIHandle		WINAPI VD32_OpenASPI(void);
	DllEXPORT void		WINAPI VD32_CloseASPI(VDASPIHandle handle);
	DllEXPORT DWORD		WINAPI VD32_GetASPISupportInfo(VDASPIHandle handle);
	DllEXPORT BOOL		WINAPI VD32_SendASPICommand(VDASPIHandle handle, SRB_ExecSCSICmd * cmd, void * buffer);
	DllEXPORT BOOL		WINAPI VD32_GetASPIDiskInfo(VDASPIHandle handle, SRB_GetDiskInfo * info);
#endif
	}


#endif // VDCOMMON_H
