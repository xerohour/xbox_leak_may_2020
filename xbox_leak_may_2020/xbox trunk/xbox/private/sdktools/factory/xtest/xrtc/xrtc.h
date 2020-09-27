#ifndef _XRTC_H
#define _XRTC_H

#include "..\parameter.h"
//#include "DateAndTime.h"
#pragma once

#ifndef PASS
#define PASS 0
#endif
#ifndef FAIL
#define FAIL 0xFFFFFFFF
#endif

// C4512: unable to generate an assignment operator for the given class. 
#pragma warning (disable:4512)

// Naming of class test and module classes.
// 
// module class: CXModule<module>
// test class:   CXModule<module>Test<test>
//
// The names are important because the modules and exective are 
// all one project.  Since the classes are presented alphebetically, 
// using this naming convention makes the code easier to navigate.
//


// 
// XUSB Module Number for getting registered with the test executive
//
#define XRTC_MODNUM 5

// Constants needed for accessing the RTC
#define RTC_PIT_TIMER_2 0x042
#define RTC_PIT_TIMER_CTRL 0x43
#define RTC_PIT_PORT_B_CTRL 0x61
#define RTC_UIP 0x80


class CXModuleRTC : public CTestObj
{
public: // Get module number from numbers.h
	DECLARE_XMTAMODULE(CXModuleEval, "rtc", XRTC_MODNUM);

protected:
	// Error messages can be declared here ...
	// (Note: 0x000 - 0x00f reserved for XMTA)
	//
	DWORD err_BadParameter(LPCTSTR s1)
	{
		ReportError(0x010, (_T("Can't find configuration parameter \"%s\"\nThis configuration ")
					 _T("parameter doesn't exist or is invalid")), s1); 
		return FAIL;
	}

	DWORD err_SystemError(DWORD i)
	{
		ReportError(0x011, (_T("System Error. GetLastError() returned %d\n")), i); 
		return FAIL;
	}

	DWORD err_DateTime(LPCTSTR s1, LPCTSTR s2)
	{
		ReportError(0x012, (_T("Incorrect Date/Time: Expected %s, Read %s\n")), s1, s2); 
		return FAIL;
	}
	
	DWORD err_Ram(int i1, int i2, int i3, int i4)
	{
		ReportError(0x013,(_T("Mismatch at Offset 0x%02X, Wrote 0x%02X, Read 0x%02X, Seed %u\n")), i1, i2, i3, i4);
		return FAIL;
	}

	DWORD err_Accuracy(double f1, double f2)
	{
		ReportError(0x014, (_T("Accuracy error: required +\\-%f ppm, measured %f ppm\n")), f1, f2);
		return FAIL;
	}

	DWORD err_StatusNoClear()
	{
		ReportError(0x015, _T("Status flag IRQF not cleared.\n"));
		return FAIL;
	}
	
	DWORD err_StatusNoSet()
	{
		ReportError(0x016, _T("Status flag IRQF not set.\n"));
		return FAIL;
	}
	
	DWORD err_UnexpIntr()
	{
		ReportError(0x017, _T("Signal IRQ8 unexpectedly activated.\n"));
		return FAIL;
	}

	DWORD err_NoIntr()
	{
		ReportError(0x018, _T("Signal IRQ8 not activated.\n"));
		return FAIL;
	}

	DWORD err_InvalidRange()
	{
		ReportError(0x019, _T("Invalid range for Length or Start Parameter(s).\n"));
		return FAIL;
	}

	DWORD err_InvalidParameter()
	{
		ReportError(0x01A, _T("Invalid range for Date or Time Parameter(s).\n"));
		return FAIL;
	}

	//debug
	DWORD err_MismatchedWrite(ULONG u1, int p1, int p2)
	{
		ReportError(0x01B, _T("Mismatched Write: Index = %d, Read = %d, Wrote = %d\n"), u1, p1, p2);
	 	return FAIL;
	}

	DWORD err_MismatchedRead(ULONG u1, int p1, int p2)
	{
		ReportError(0x01C, _T("Mismatched Read: Index = %d, Actual = %d, Read = %d\n"), u1, p1, p2);
	 	return FAIL;
	}

	DWORD err_UipTimeout()
	{
		ReportError(0x01D, _T("Timed out waiting for UIP\n"));
		return FAIL;
	}


	int iStart;
	int iLength;
	int iNumPatterns;
	int iPpm;
	int iSeconds;
	int m_nSeed;
	HANDLE m_hDriver;

	//
// RTC defs
//
#define RTC_ADDR_PORT		0x70
#define RTC_DATA_PORT		0x71
#define RTC_EX_ADDR_PORT	0x72
#define RTC_EX_DATA_PORT	0x73
#define RTC_RAM_LEN			114
#define RTC_RAM_START		14
#define RTC_RAM_END			(RTC_RAM_START + RTC_RAM_LEN - 1)
#define RTC_SECONDS			0x00
#define RTC_MINUTES			0x02
#define RTC_HOURS			0x04
#define RTC_DAY				0x06
#define RTC_DATE			0x07
#define RTC_MONTH			0x08
#define RTC_YEAR			0x09
#define RTC_CTRL_REG_A		0x0A
#define RTC_CTRL_REG_B		0x0B
#define RTC_CTRL_REG_C		0x0C
#define RTC_CTRL_REG_D		0x0D
#define RTC_UIP_MASK		0x80
#define RTC_UIP_TIMEOUT		0x0C000000
#define RTC_INTR_TIMEOUT	0x00C00000
	//
// Slave PIC defs
//
#define SPIC_CMD_PORT		0xA0
#define SPIC_IRR_PORT		0xA0
#define MPIC_CMD_PORT		0x20

//
// An array to conveniently access RTC Date-Time info
//
static const UCHAR DateTimeRegs[];// = {RTC_SECONDS,RTC_MINUTES,RTC_HOURS,RTC_DAY,RTC_DATE,RTC_MONTH,RTC_YEAR};


	virtual bool InitializeParameters ();
//	~CXModuleRTC();

public:
	//
	// Common functions go here.  Put any functions that need 
	// to be global to the tests in the module class
	//
	void InitSeed();
	ULONG CheckInterrupt();
	
	bool ReadRam(ULONG uStart, ULONG uLength, PUCHAR pData);
	void AtomicWriteReadRam(ULONG uStart, ULONG uLength, PUCHAR pWriteData, PUCHAR pReadData);	
	bool WriteRam(ULONG uStart, ULONG uLength, PUCHAR pData);
	
	UCHAR RtcHwrReadReg(UCHAR Address);
	void RtcHwrAtomicWriteReadReg(PUCHAR pWriteData, PUCHAR pReadData, ULONG uStart, ULONG uLength);
	void RtcHwrWriteReg(UCHAR Address, UCHAR Data);

	double GetCPUSpeed();
	bool bWaitUIPandGetTimesSetTimes(LARGE_INTEGER *pTSCVal, unsigned char *pucRTCReadValues, unsigned char *pucRTCWriteValues);
	void vFillDateString(LPTSTR pBuffer, unsigned char *pucRTCValues, unsigned short second);

	ULONG RtcHwrCheckInterrupt();
	void vNewSeed(UINT *pSeed);
	DWORD random(UINT *pScratch);

};


#endif	// _XRTC_H
