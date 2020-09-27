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
#ifndef _XBOX

VDASPIHandle __stdcall VD32_OpenASPI(void)
	{

	DP("ASPI++");
	if (ASPIPossible)
		{
		if (ASPICount == 0)
			{
			ASPIPossible = false;
			ASPIStructHandle = new VDASPIStruct;

			ASPIHandle = LoadLibrary("WNASPI32.DLL");

			if (ASPIHandle)
				{
				(FARPROC &) (ASPIStructHandle->xSendASPI32Command) = GetProcAddress(ASPIHandle, "SendASPI32Command");
				(FARPROC &) (ASPIStructHandle->xGetASPI32SupportInfo) = GetProcAddress(ASPIHandle, "GetASPI32SupportInfo");
				if (ASPIStructHandle->xSendASPI32Command && ASPIStructHandle->xGetASPI32SupportInfo)
					{
					ASPIStructHandle->event = CreateEvent(NULL, FALSE, FALSE, NULL);

					ASPIPossible = true;
					ASPICount++;

					DP("Got ASPI");
					return ASPIStructHandle;
					}
				}
			}
		else
			{
			ASPICount++;
			return ASPIStructHandle;
			}
		}
	else
		return NULL;

	if (ASPIStructHandle)
		delete ASPIStructHandle;

	return NULL;
	}


void __stdcall VD32_CloseASPI(VDASPIHandle h)
	{
	if (ASPICount == 0)
		{
		DP("VDSUP32 : Release but no handle open");
		return;
		}

	if ((--ASPICount) == 0)
		{
		CloseHandle(ASPIStructHandle->event);

		delete ASPIStructHandle;
		}

	}

DWORD __stdcall VD32_GetASPISupportInfo(VDASPIHandle h)
	{
	if (ASPIStructHandle)
		{
		return ASPIStructHandle->xGetASPI32SupportInfo();
		}
	else
		return 0;
	}

BOOL __stdcall VD32_SendASPICommand(VDASPIHandle h, SRB_ExecSCSICmd * cmd, void * buffer)
	{
	if (ASPIStructHandle)
		{
		::ResetEvent(ASPIStructHandle->event);

		cmd->SRB_PostProc = ASPIStructHandle->event;
		cmd->SRB_BufPointer = (BYTE *)buffer;

		ASPIStructHandle->xSendASPI32Command((void*)cmd);

		if (cmd->SRB_Status == SS_PENDING) ::WaitForSingleObject(ASPIStructHandle->event, INFINITE);

		return cmd->SRB_Status == SS_COMP;
		}
	else
		return FALSE;
	}

BOOL __stdcall VD32_GetASPIDiskInfo(VDASPIHandle h, SRB_GetDiskInfo * info)
	{
	if (ASPIStructHandle)
		{
		ASPIStructHandle->xSendASPI32Command((void*)info);

		do {} while (info->SRB_Status == SS_PENDING);

		return info->SRB_Status == SS_COMP;
		}
	else
		return FALSE;
	}

///////////////////////////////////////////////////////////////////////////////
// 32Bit Registry Support
///////////////////////////////////////////////////////////////////////////////

BOOL __stdcall VD32_GetPrivateProfileString(char * section, char * entry,
	                                              char * initBuff, char * buff,
	                                              int size, char * fileName)
	{
	LONG	status;
	DWORD	dummy;
	HKEY key;
	char ebuffer[200];
	strcpy(buff, initBuff);
	BOOL res;
	DWORD	type;
	DWORD isize;

	strcpy(ebuffer, "Software\\Quadrant International, Inc.\\");
	strcat(ebuffer, fileName);
	strcat(ebuffer, "\\");
	strcat(ebuffer, section);

	status = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
									ebuffer,
									NULL,
									__TEXT(""),
									REG_OPTION_NON_VOLATILE,
									KEY_READ | KEY_WRITE,
									NULL,
									&key,
									&dummy);
	if (status == ERROR_SUCCESS)
		{
		res = TRUE;

		isize = size;

		if (RegQueryValueEx(key, entry, NULL, &type, (BYTE *)buff, &isize) == ERROR_SUCCESS)
			{
			if (type != REG_SZ) res = FALSE;
			}
		else
			res = FALSE;

		RegCloseKey(key);

		return res;
		}
	else
		return FALSE;
	}

BOOL __stdcall VD32_WritePrivateProfileString(char * section, char * entry, char * value, char * fileName)
	{
	LONG	status;
	DWORD	dummy;
	HKEY key;
	char ebuffer[200];
	BOOL res;

	strcpy(ebuffer, "Software\\Quadrant International, Inc.\\");
	strcat(ebuffer, fileName);
	strcat(ebuffer, "\\");
	strcat(ebuffer, section);

	status = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
									ebuffer,
									NULL,
									__TEXT(""),
									REG_OPTION_NON_VOLATILE,
									KEY_READ | KEY_WRITE,
									NULL,
									&key,
									&dummy);
	if (status == ERROR_SUCCESS)
		{
		res = TRUE;

		if (RegSetValueEx(key, entry, NULL, REG_SZ, (BYTE *)(TCHAR *)value, strlen(value)) == ERROR_SUCCESS)
			{
			}
		else
			res = FALSE;

		RegCloseKey(key);

		return res;
		}
	else
		return FALSE;
	}

BOOL __stdcall VD32_GetPrivateProfileDWORD(char * section, char * entry, DWORD init, DWORD * buff,
	                                              char * fileName)
	{
	LONG	status;
	DWORD	dummy;
	HKEY key;
	char ebuffer[200];
	BOOL res;
	DWORD	type;
	DWORD isize;

	*buff = init;
	strcpy(ebuffer, "Software\\Quadrant International, Inc.\\");
	strcat(ebuffer, fileName);
	strcat(ebuffer, "\\");
	strcat(ebuffer, section);

	status = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
									ebuffer,
									NULL,
									__TEXT(""),
									REG_OPTION_NON_VOLATILE,
									KEY_READ | KEY_WRITE,
									NULL,
									&key,
									&dummy);
	if (status == ERROR_SUCCESS)
		{
		res = TRUE;

		isize = 4;

		if (RegQueryValueEx(key, entry, NULL, &type, (BYTE *)buff, &isize) == ERROR_SUCCESS)
			{
			if (type != REG_DWORD && type != REG_BINARY) res = FALSE;
			}

		RegCloseKey(key);

		return res;
		}
	else
		return FALSE;
	}

BOOL __stdcall VD32_WritePrivateProfileDWORD(char * section, char * entry, DWORD value, char * fileName)
	{
	LONG	status;
	DWORD	dummy;
	HKEY key;
	char ebuffer[200];
	BOOL res;

	strcpy(ebuffer, "Software\\Quadrant International, Inc.\\");
	strcat(ebuffer, fileName);
	strcat(ebuffer, "\\");
	strcat(ebuffer, section);

	status = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
									ebuffer,
									NULL,
									__TEXT(""),
									REG_OPTION_NON_VOLATILE,
									KEY_READ | KEY_WRITE,
									NULL,
									&key,
									&dummy);
	if (status == ERROR_SUCCESS)
		{
		res = TRUE;

		if (RegSetValueEx(key, entry, NULL, REG_DWORD, (BYTE *)&value, 4) != ERROR_SUCCESS)
			res = FALSE;

		RegCloseKey(key);

		return res;
		}
	else
		return FALSE;
	}
#endif
