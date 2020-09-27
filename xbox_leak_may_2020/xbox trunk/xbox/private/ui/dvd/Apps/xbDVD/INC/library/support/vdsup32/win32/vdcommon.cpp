////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "vdcommon.h"



HINSTANCE		ASPIHandle;
bool				ASPIPossible		= TRUE;
VDASPIStruct *	ASPIStructHandle;
int				ASPICount			= 0;



void __stdcall VD32_VOID()
	{
	}

void  __stdcall VD32_Sleep(DWORD millisecs)
	{
	Sleep(millisecs);
	}

///////////////////////////////////////////////////////////////////////////////
// 32 Bit File Access
///////////////////////////////////////////////////////////////////////////////

HANDLE __stdcall VD32_OpenFile(const char * name, BOOL readWrite)
	{
	DP("Open File");
	if (readWrite)
		return CreateFile(name,
		                  GENERIC_READ | GENERIC_WRITE,
								0,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
								NULL);
	else
		return CreateFile(name,
		                  GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
								NULL);
	}

HANDLE __stdcall VD32_CreateFile(const char * name)
	{
	return CreateFile(name,
		               GENERIC_READ | GENERIC_WRITE,
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
							NULL);
	}

void  __stdcall VD32_CloseFile(HANDLE file)
	{
	CloseHandle(file);
	}

DWORD __stdcall VD32_ReadFile(HANDLE file, void * data, DWORD size)
	{
	ReadFile(file, data, size, &size, NULL);

	return size;
	}

DWORD __stdcall VD32_WriteFile(HANDLE file, void * data, DWORD size)
	{
	WriteFile(file, data, size, &size, NULL);

	return size;
	}

void __stdcall VD32_SeekFile(HANDLE file, DWORD &low, DWORD &high)
	{
	low = SetFilePointer(file, low, &(LONG &)high, FILE_BEGIN);
	}

void __stdcall VD32_GetFileSize(HANDLE file, DWORD &low, DWORD &high)
	{
	low = GetFileSize(file, &high);
	}


///////////////////////////////////////////////////////////////////////////////
// 32Bit ASPI Support
///////////////////////////////////////////////////////////////////////////////
