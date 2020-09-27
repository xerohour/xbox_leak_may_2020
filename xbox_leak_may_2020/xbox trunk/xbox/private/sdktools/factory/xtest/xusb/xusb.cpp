///////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000-2001 Intel Corp. All rights reserved.
//
// Title:  XUSB
//
// History:	
// 
// 10/05/00 V1.00 PHM Original release.
//
///////////////////////////////////////////////////////////////////////////////////


#include "..\stdafx.h"
#include "..\testobj.h"
#include "xusb.h"


IMPLEMENT_MODULEUNLOCKED (CXModuleUSB);

//////////////////////////////////////////////////////////////////////////////////
// Function name	: InitializeParameters
// Description	    : Tries to see if the configuration parameters were defined
//					  or not.
// Return type		: bool 
//////////////////////////////////////////////////////////////////////////////////
bool CXModuleUSB::InitializeParameters ()
{
//	TCHAR StrOut[512];
	int iPresent;

	if(!CTestObj::InitializeParameters ())
		return false;

	pucWriteBuf = NULL;
	pucReadBuf = NULL;

	//
	// Make sure that the "gamepads" parameter is there in the ini file
	//
	if((iPresent = GetCfgPresent(L"gamepads")) == 0)
		err_ParameterNotFound(L"GamePads");
	
	else
	{
		// Make sure that its a non-zero value
		iGamePads = GetCfgInt (L"gamepads", 0);

		// Make sure that its a non-zero value between 1 and 4 (there can be upto 4 gamepads
		
		if((iGamePads != 0) && ((iGamePads < 1) || (iGamePads > 4)))
			err_IncorrectValue(iGamePads, L"GamePads", 1, 4);
	
	} // end else

	// Make sure that the "memoryunits" parameter is there in the ini file
	if((iPresent = GetCfgPresent(L"memoryunits")) == 0)
		err_ParameterNotFound(L"MemoryUnits");

	else
	{
		// Make sure that its a non-zero value
		iMemoryUnits = GetCfgInt (L"memoryunits", 0);

		// Make sure that its a non-zero value between 1 and 8 (there can be upto 8 
		// memory units - 2 for each gamepad)
		if((iMemoryUnits != 0) && ((iMemoryUnits <1) || (iMemoryUnits > 8)))
			err_IncorrectValue(iMemoryUnits, L"MemoryUnits", 1, 8);

	} // end else

	// Get the "length" parameter
	uLength = GetCfgInt (L"length", 4096);

	// Get the number of reads to perform
	gdwTotalReadLoops = GetCfgUint (L"readloops", 1);

	return true;

} // end bool CXModuleUSB::InitializeParameters ()


///////////////////////////////////////////////////////////////////////////////////
// Actual XUSB tests.
///////////////////////////////////////////////////////////////////////////////////


// This test verifies that the number of Gamepads and Memory Units attached to the UUT
// (Unit Under Test) matches with what is specified by their respective configuration
// parameters.
IMPLEMENT_TESTUNLOCKED (USB, enumerationtest, 1)
{
	
	int iGamePadsFound = 0, iMemUnitsFound = 0;
	DWORD dwDeviceBitMapGamePad, dwDeviceBitMapMemUnit;
	int i, j;
	bool bFoundGamePad[4], bFoundMemUnit[8];
	
//	#ifdef _DEBUG
		static int Enum;
		Enum++;
		ReportDebug(BIT0,_T("Enumeration Test - Loop %d"), Enum);
//	#endif

	// debug
	/*static int x = 1;
	TCHAR StrOut[512];
	_stprintf(StrOut, _T("LOOPLIMIT for EnumerationTest = %d\n"), x);
	OutputDebugString(StrOut);
	x++;*/
	
	// initialize bFoundGamePad and bFoundMemUnit to false
	for(j = 0; j < 4; j++)
		bFoundGamePad[j] = false;

	for(j = 0; j < 8; j++)
		bFoundMemUnit[j] = false;


	// Trial of 5 loops for detecting gamepads and memory units that match the
	// configuration parameters.
	for(i = 0; i < 5; i++)
	{	
		// Get a bitmap for the gamepads to see how many are attached.
		dwDeviceBitMapGamePad = XGetDevices(XDEVICE_TYPE_GAMEPAD);
		
		// Get a bitmap for the memory units to see how many are attached.
		dwDeviceBitMapMemUnit = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);
	
		// Check for Gamepads on Ports 0 - 3. There could be max 4 gamepads.
		for(j = 0; j < 4; j++)
		{
			if(bFoundGamePad[j] == true)
				continue;

			else
			{
				if(dwDeviceBitMapGamePad & (1 << j))
				{
					bFoundGamePad[j] = true;
					iGamePadsFound++;

				} // end if(dwDeviceBitMapGamePad & (1 << i))

			} // end else

		} // end for(j = 0; j < 4; j++)

		// There can be max 8 memory units - 2 for each gamepad (top and bottom slot).
		// Top slots: bits 0 - 3
		// Bottom slots: bits 16 - 19
		for(j = 0; j < 8; j++)
		{
			if(bFoundMemUnit[j] == true)
				continue;
			else
			{
				if(j <=3)
				{
					if(dwDeviceBitMapMemUnit & (1 << j))
					{
						bFoundMemUnit[j] = true;
						iMemUnitsFound++;
			
					} // end if(dwDeviceBitMapMemUnit & (1 << j))
				}
				
				else
				{
					if(dwDeviceBitMapMemUnit & (1 << (j + 12)))
					{
						bFoundMemUnit[j] = true;
						iMemUnitsFound++;

					} // end if(dwDeviceBitMapMemUnit & (1 << j))
			
				}
			
			} // end else

		} // end for(j = 0; j < 8; j++)
		
		// After calling XInitDevices, it takes less than 30 seconds to detect a device.
		// So Wait for 200 ms during each trial to make sure that a device is detected.
		Sleep(200);

	} // end for(i = 0; i < 5; i++)

	// If completed the trial loop and didn't find gamepads that match the config
	// parameter, report error
	if((i == 5) && (iGamePadsFound != iGamePads))
		err_TimeOut(L"GamePads");

	// Found incorrect # of gamepads
	if(iGamePadsFound != iGamePads)
		err_MismatchedDevices(L"GamePads", iGamePads, iGamePadsFound);

	// If completed the trial loop and didn't find memory units that match the config
	// parameter, report error
	if((i == 5) && (iMemUnitsFound != iMemoryUnits))
		err_TimeOut(L"Memory Units");

	// Found incorrect # of memory units
	if(iMemUnitsFound != iMemoryUnits)
		err_MismatchedDevices(L"Memory Units", iMemoryUnits, iMemUnitsFound);
	
}	// IMPLEMENT_TESTUNLOCKED (USB, enumerationtest, 1)


// This test stresses all the memory units present on the gamepad(s). For each memory
// unit present, it creates a file on its drive, writes to the file, reads from the
// file and compares them to make sure that they're the same. This should generate 
// traffic on the USB bus.
IMPLEMENT_TESTUNLOCKED (USB, memoryunitstresstest, 2)
{
	DWORD dwBytesWritten, dwBytesRead, dwError;
	int j = 0, iMemUnitsFound = 0;
	bool bFoundMemUnit[8];
	char chDriveLetter[8];
	CHAR chDrive;
	HANDLE hFile;
	char szFileName[50];
	ULONG i;
	DWORD dwTotalErrors = 0, dwReadLoops = 0;
	UINT uiSeed, uiRandScratch;

	// debug
	//ReportDebug(BIT0,_T("In MUStressTest\n"));

//	#ifdef _DEBUG
		static int MemUnit;
		MemUnit++;
		ReportDebug(BIT0,_T("MemoryUnitStress Test - Loop %d"), MemUnit);
//	#endif

	
	// debug
	/*static int x = 1;
	TCHAR StrOut[512];	
	_stprintf(StrOut, _T("LOOPLIMIT for MemoryUnitStressTest = %d\n"), x);
	OutputDebugString(StrOut);
	//ReportDebug(BIT0,_T("LOOPLIMIT = %d\n"), x);
	x++;*/

	vNewSeed(&uiSeed);
	uiRandScratch = uiSeed;
	// Initialize the write buffer
	if(pucWriteBuf == NULL)
	{
		if ((pucWriteBuf = new UCHAR[uLength]) == NULL)
		{
			err_CouldNotAllocateBuffer((ULONG)(sizeof(UCHAR)*uLength));
			return;
		}
	}

	// Initialize the read buffer
	if(pucReadBuf == NULL)
	{
		if ((pucReadBuf = new UCHAR[uLength]) == NULL)
		{
			err_CouldNotAllocateBuffer((ULONG)(sizeof(UCHAR)*uLength));
			delete [] pucWriteBuf;
			pucWriteBuf = NULL;
			return;
		}
	}

	// debug
	//ReportDebug(BIT0,_T("after initializing buffers\n"));

	for(j = 0; j < 8; j++)
	{
		bFoundMemUnit[j] = false;
		chDriveLetter[j] = '\0';
	}

	// Try to detect a memory unit in top slot and/or bottom slot of each of the four
	// gamepads attached to the UUT. The gamepads are located on ports 1-4. Each gamepad
	// can have upto two memory units - one on the TOP SLOT and one on the BOTTOM SLOT.
	for(j = 0; j < 4; j++)
	{
		dwError = XMountMU(j, XDEVICE_TOP_SLOT, &chDrive);						
		
		// If found a memory unit, increment the count & store the drive letter for that unit.
		if(dwError == ERROR_SUCCESS)
		{
			iMemUnitsFound++;
			bFoundMemUnit[j] = true;
			chDriveLetter[j] = chDrive;
		}

		dwError = XMountMU(j, XDEVICE_BOTTOM_SLOT, &chDrive);						
		if(dwError == ERROR_SUCCESS)
		{
			iMemUnitsFound++;
			bFoundMemUnit[j + 4] = true;
			chDriveLetter[j + 4] = chDrive; 
		}

	} // end for(j = 0; j < 4; j++)

	// Verify that the memory units mounted match the number of memory units
	// specified in the iMemoryUnits configuration parameter.
	if(iMemUnitsFound != iMemoryUnits)
	{
		err_MismatchedDevices(L"Memory Units", iMemoryUnits, iMemUnitsFound);
	}
	else
	{
		// For each memory unit found create a file on the respective drive.
		// Drive letters F thru M are dedicated for the 8 memory units.
		for(j = 0; j <8; j++)
		{
			if(bFoundMemUnit[j] == true)
			{
				// Create a unique file name for each drive that is found.
				// For example, F:\\MemoryUnitStressTest0.txt
				sprintf(szFileName, "%c:\\MemoryUnitStressTest%d.txt", chDriveLetter[j], j);
			
				hFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, 
					    		   CREATE_ALWAYS, 0, NULL);

				if(hFile == INVALID_HANDLE_VALUE)
					err_InvalidHandle(GetLastError());
					
				// If a valid file is created, write to the file, read from the file
				// and compare the files.
				else
				{	
					// debug
					// Fill up write buffer with random values to be written to the 
					// files on the drives found on the Memory Units.
					for(i = 0; i < uLength; i++)
						pucWriteBuf[i] = (UCHAR)random(&uiRandScratch);
					
					// Rewind and write data to the file.
					if (SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
						err_SetFilePointer(GetLastError());
					else
					{
						if(!WriteFile(hFile, pucWriteBuf, uLength, &dwBytesWritten, NULL))
							err_WriteFile(GetLastError());
						else
						{
							for (dwReadLoops = 0; dwReadLoops < gdwTotalReadLoops; dwReadLoops++)
							{
								// Flush the data, rewind the file, and read the data back.
								FlushFileBuffers(hFile);
								if (SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
									err_SetFilePointer(GetLastError());
								else
								{
									memset(pucReadBuf, 0, uLength); // Clear the read buffer
									if(!ReadFile(hFile, pucReadBuf, uLength, &dwBytesRead, NULL))
										err_ReadFile(GetLastError());
									else
									{
										// Compare the data read with the data written
										for(i = 0; i < uLength; i++)
										{
											if(pucReadBuf[i] != pucWriteBuf[i])
											{
												err_WriteReadMismatch(pucReadBuf[i], pucWriteBuf[i]);
												if (CheckAbort(HERE))
													break;
												dwTotalErrors ++;
												if (dwTotalErrors > 100)
													break;
											}
										}
									}
								}
								if (CheckAbort(HERE))
									break;
							}
						}
					}
					if (!CloseHandle(hFile))
						err_CloseHandle(GetLastError());

				} // end else

			} // end if(bFoundMemUnit[j] == true)
			if (CheckAbort(HERE))
				break;
		} // end for(j = 0; j <8; j++)
	
	} // end else

	// debug
	//ReportDebug(BIT0,_T("Leaving MUStessTest\n"));
					
	for(j = 0; j < 4; j++) // Unmount all memory units
	{
		if (bFoundMemUnit[j])
		{
			if ((dwError = XUnmountMU(j, XDEVICE_TOP_SLOT)) != ERROR_SUCCESS)
				err_UnMountMU(j, L"Top", dwError); 
		}
		if (bFoundMemUnit[j+4])
		{
			if ((dwError = XUnmountMU(j, XDEVICE_BOTTOM_SLOT)) != ERROR_SUCCESS)
				err_UnMountMU(j, L"Bottom", dwError); 
		}
	}
	delete [] pucWriteBuf;
	pucWriteBuf = NULL;
	delete [] pucReadBuf;
	pucReadBuf = NULL;
} // IMPLEMENT_TESTUNLOCKED (USB, memoryunittest, 2)

void CXModuleUSB::vNewSeed(UINT *pSeed)
{
	SYSTEMTIME systimeSeed;
	FILETIME filetimeSeed;

	GetSystemTime(&systimeSeed);
	SystemTimeToFileTime(&systimeSeed, &filetimeSeed);
	*pSeed = (UINT)filetimeSeed.dwLowDateTime;

	return;
}

DWORD CXModuleUSB::random(UINT *pScratch)
{
	DWORD ret;
	ret  = (*pScratch = *pScratch * 214013L + 2531011L) >> 16;
	ret |= (*pScratch = *pScratch * 214013L + 2531011L) & 0xFFFF0000;
	return ret;
}
