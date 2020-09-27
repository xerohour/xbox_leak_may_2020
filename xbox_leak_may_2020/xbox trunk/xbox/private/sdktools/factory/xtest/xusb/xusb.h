#ifndef _XUSB_H
#define _XUSB_H

#include "..\parameter.h"

#pragma once
// C4512: unable to generate an assignment operator for the given class. 
#pragma warning (disable:4512)

//
// Naming of class test and module classes.
// 
// module class: CXModule<module>
// test class:   CXModule<module>Test<test>
//
// The names are important because the modules and executive are 
// all one project.  Since the classes are presented alphebetically, 
// using this naming convention makes the code easier to navigate.
//

#ifndef PASS
#define PASS 0
#endif
#ifndef	FAIL
#define FAIL 0xFFFFFFFF
#endif

#define XBOX_NOSLOT 0


// 
// XUSB Module Number for getting registered with the test executive
//
#define XUSB_MODNUM 4

class CXModuleUSB : public CTestObj//, public CXModuleUSB_Vars
{
public: // Get module number from numbers.h
	DECLARE_XMTAMODULE(CXModuleUSB, "usb", XUSB_MODNUM);	// module name and number

protected:
	//
	// Error messages can be declared here ...
	// (Note: 0x000 - 0x00f reserved for XMTA)
	//
	DWORD err_ParameterNotFound(LPCTSTR s1)
	{
		ReportError (0x010, L"Can't find Configuration Parameter \"%s\".\n", s1);
		return FAIL;
	}

	DWORD err_BadParameter(LPCTSTR s1, int i1, int i2)
	{
		ReportError (0x011, L"Configuration Parameter \"%s\" should have a non-zero value between %d and %d\n",
					 s1, i1, i2);
		return FAIL;
	}
	
	DWORD err_IncorrectValue(int i1, LPCTSTR s1, int i2, int i3)
	{
		ReportError(0x12, L"Incorrect Value of %d for Configuration Parameter \"%s\"."
					L" It should be between %d and %d\n", i1, s1, i2, i3);
		return FAIL;
	}

	DWORD err_Enumerate()
	{
		ReportError (0x013, L"Failed to enumerate the USB bus.\n");
		return FAIL;
	}
	
	DWORD err_ParameterRead(LPCTSTR string)
	{
		ReportError (0x014, L"Failed to read Parameter: %s\n", string);
		return FAIL;
	}
	
	DWORD err_ParameterWrite(LPCTSTR string)
	{
		ReportError (0x015, L"Failed to write Parameter: %s\n", string);
		return FAIL;
	}
	
	DWORD err_MismatchedDevices(LPCTSTR s1, int i1, int i2)
	{
		ReportError (0x016, L"Mismatched number of %s. Expected: %d,"
					 L" Detected: %d\n", s1, i1, i2);
		return FAIL;
	}
	
	DWORD err_InvalidHandle(DWORD d1)
	{
		ReportError (0x017, L"Unable to open file. GetLastError() returned %d\n", d1);
		return FAIL;
	}

	DWORD err_TimeOut(LPCTSTR s1)
	{
		ReportError(0x018, L"Timed out while trying to detect the %s.\n", s1);
		return FAIL;
	}

	DWORD err_WriteFile(DWORD d1)
	{
		ReportError(0x19, L"Error while writing to file. GetLastError() returned %d\n", d1);
		return FAIL;
	}

	DWORD err_ReadFile(DWORD d1)
	{
		ReportError(0x1A, L"Error while reading from file. GetLastError() returned %d\n", d1);
		return FAIL;
	}

	DWORD err_UnMountMU(int i1, LPCTSTR s1, DWORD d1)
	{
		ReportError (0x01B, L"Unable to UnMount Memory Unit on Port %d, %s Slot. Error code returned = %d\n", i1, s1, d1);
		return FAIL;
	}

	DWORD err_WriteReadMismatch(int i1, int i2)
	{
		ReportError(0x01C,(_T("Write/Read Mismatch. Wrote 0x%02X, Read 0x%02X\n")), i1, i2);
		return FAIL;
	}

	DWORD err_CouldNotAllocateBuffer(DWORD d1)
	{
		ReportError (0x01D, _T("Could not allocate a %lu byte data buffer\n"), d1); 
		return FAIL;
	}

	DWORD err_CloseHandle(DWORD d1)
	{
		ReportError (0x01E, L"Unable to close file handle. GetLastError() returned %d\n", d1);
		return FAIL;
	}

	DWORD err_SetFilePointer(DWORD d1)
	{
		ReportError(0x1F, L"Error while reseting the file pointer to the start of the file.  GetLastError() returned %d\n", d1);
		return FAIL;
	}

	// Parameters declared here ...
	int iGamePads;
	int iMemoryUnits;
	ULONG uLength;
	DWORD gdwTotalReadLoops;
	PUCHAR pucWriteBuf, pucReadBuf;

	virtual bool InitializeParameters();
	virtual ~CXModuleUSB() // Destructor
	{
		// delete the write buffer after its use
		if (pucWriteBuf != NULL)
		{
			delete [] pucWriteBuf;
			pucWriteBuf = NULL;
		}

		// delete the read buffer after its use
		if (pucReadBuf != NULL)
		{
			delete [] pucReadBuf;
			pucReadBuf = NULL;
		}
	} // end virtual ~CXModuleUSB()
	void vNewSeed(UINT *pSeed);
	DWORD random(UINT *pScratch);
};


#endif	// _XUSB_H














