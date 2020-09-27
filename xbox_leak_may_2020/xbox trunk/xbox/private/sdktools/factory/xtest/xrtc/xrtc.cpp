///////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000-2001 Intel Corp. All rights reserved.
//
// Title:  XRTC
//
// History:	
// 
// 11/07/00 V1.00 PHM Original release 
//
///////////////////////////////////////////////////////////////////////////////////


#include "..\stdafx.h"
#include "..\testobj.h"
#include "databuffer.h"
#include "xrtc.h"
#include "rtcioctl.h"

IMPLEMENT_MODULELOCKED (CXModuleRTC);

//const UCHAR CXModuleRTC::DateTimeRegs[] = 
//{RTC_SECONDS,RTC_MINUTES,RTC_HOURS,RTC_DAY,RTC_DATE,RTC_MONTH,RTC_YEAR};

//////////////////////////////////////////////////////////////////////////////////
// Function name	: InitializeParameters
// Description	    : Tries to see if the configuration parameters were defined
//					  or not.
// Return type		: bool 
//////////////////////////////////////////////////////////////////////////////////
bool CXModuleRTC::InitializeParameters ()
{
	if (!CTestObj::InitializeParameters ())
		return false;

//	Initialize();

	if ((iStart = GetCfgInt(_T("start"), -1)) == -1)
		err_BadParameter(L"Start");
		
	if ((iLength = GetCfgInt(_T("length"), -1)) == -1)
		err_BadParameter(L"Length");
		
	if ((iNumPatterns = GetCfgInt(_T("numpatterns"), 0)) == 0)
		err_BadParameter(L"NumPatterns");

	if ((iPpm = GetCfgInt(_T("ppm"), -1)) == -1)
		err_BadParameter(L"Ppm");

	if ((iSeconds = GetCfgInt(_T("seconds"), -1)) == -1)
		err_BadParameter(L"Seconds");

	// debug
//	ReportDebug(BIT0,_T("End of InitialzieParameters.\n"));

	return true;

} // end bool CXModuleRTC::InitializeParameters ()


//////////////////////////////////////////////////////////////////////////////////
// Function name	: ReadRam
// Description	    : Reads the contents of the RTC RAM
// Input			: uStart - start of the RAM. Usually this is 0x0E (14) as the
//					           first 14 bytes of RTC RAM contain the registers 
//							   for date, time and control registers
//					: uLength - Length of the RTC RAM. This could be 128 or 256
//							    bytes
//					: pData - buffer that will read the data from the RTC RAM
// Return type		: bool 
//////////////////////////////////////////////////////////////////////////////////
bool CXModuleRTC::ReadRam(ULONG uStart, ULONG uLength, PUCHAR pData)
{

	ULONG uIndex;
	PUCHAR pVerifyData = new UCHAR[iLength];
	TCHAR StrOutput[512];
	//
	// Verify for a valid range
	//
	if((uLength > RTC_RAM_LEN) || (uStart < RTC_RAM_START)|| (uStart > RTC_RAM_END)
		|| ((uStart + uLength) > (RTC_RAM_END + 1)))
	{
		err_InvalidRange();
		return false;
	}
	
	// debug first 14 bytes (0-13) in CMOS give info about time, date and Status Regs A-D
	/*uStart = 0;
	for(uIndex = 0; uIndex < 14; uIndex++)
	{
		ReportDebug(BIT0,_T("uStart+uIndex = %d\n"), uStart+uIndex);
		pData[uIndex] = RtcHwrReadReg((UCHAR)(uStart + uIndex));
	}
*/
	
	for(uIndex = 0; uIndex < uLength; uIndex++)
		pData[uIndex] = RtcHwrReadReg((UCHAR)(uStart + uIndex));

	// debug
	for(uIndex = 0; uIndex < uLength; uIndex++)
	{
		pVerifyData[uIndex] = RtcHwrReadReg((UCHAR)(uStart + uIndex));
		if(pVerifyData[uIndex] != pData[uIndex])
		{
			_stprintf(StrOutput, _T("error generated so there'll be halt on error.\n"));
			OutputDebugString(StrOutput);
			err_MismatchedRead(uIndex+uStart, pVerifyData[uIndex], pData[uIndex]);
			return false;
		}
	}


	// debug
/*	for(uIndex = 0; uIndex < 14; uIndex++)	
		ReportDebug(BIT0,_T("pData[%0X] = %d\n"), uIndex, pData[uIndex]);
*/
	return true;

} // end void CXModuleRTC::ReadRam(ULONG uStart, ULONG uLength, PUCHAR pData)



//////////////////////////////////////////////////////////////////////////////////
// Function name	: AtomicWriteReadRam
// Description	    : Reads and writes the contents of the RTC RAM
// Input			: uStart - start of the RAM. Usually this is 0x0E (14) as the
//					           first 14 bytes of RTC RAM contain the registers 
//							   for date, time and control registers
//					: uLength - Length of the RTC RAM. This could be 128 or 256
//							    bytes
//					: pWriteData - buffer that contains the data to be written to
//								  the RAM
//					: pReadData - buffer that will read the data from the RTC RAM
// Return type		: void 
//////////////////////////////////////////////////////////////////////////////////
void CXModuleRTC::AtomicWriteReadRam(ULONG uStart, ULONG uLength, PUCHAR pWriteData, PUCHAR pReadData)
{
	//
	// Verify for a valid range
	//
	if((uLength > RTC_RAM_LEN) || (uStart < RTC_RAM_START)|| (uStart > RTC_RAM_END)
		|| ((uStart + uLength) > (RTC_RAM_END + 1)))
	{
		err_InvalidRange();
		return;
	}

	RtcHwrAtomicWriteReadReg(pWriteData, pReadData, uStart, uLength);

} // end void CXModuleRTC::AtomicWriteReadRam(ULONG uStart, ...)



//////////////////////////////////////////////////////////////////////////////////
// Function name	: WriteRam
// Description	    : writes the contents to the RTC RAM
// Input			: uStart - start of the RAM. Usually this is 0x0E (14) as the
//					           first 14 bytes of RTC RAM contain the registers 
//							   for date, time and control registers
//					: uLength - Length of the RTC RAM. This could be 128 or 256
//							    bytes
//					: pData - buffer that contains the data to be written to
//							  the RAM
// Return type		: bool 
//////////////////////////////////////////////////////////////////////////////////
bool CXModuleRTC::WriteRam(ULONG uStart, ULONG uLength, PUCHAR pData)
{
	ULONG uIndex;
	PUCHAR pVerifyData = new UCHAR[iLength];
	TCHAR StrOutput[512];
	
	//
	// Verify for a valid range
	//
	if((uLength > RTC_RAM_LEN) || (uStart < RTC_RAM_START)|| (uStart > RTC_RAM_END)
		|| ((uStart + uLength) > (RTC_RAM_END + 1)))
	{
		err_InvalidRange();
		return false;
	}

	for(uIndex = 0; uIndex < uLength; uIndex++)
		RtcHwrWriteReg((UCHAR)(uStart + uIndex), pData[uIndex]);

	// debug
	for(uIndex = 0; uIndex < uLength; uIndex++)
	{
		pVerifyData[uIndex] = RtcHwrReadReg((UCHAR)(uStart + uIndex));
		if(pVerifyData[uIndex] != pData[uIndex])
		{
			_stprintf(StrOutput, _T("error generated so there'll be halt on error.\n"));
			OutputDebugString(StrOutput);
			err_MismatchedWrite(uIndex+uStart, pVerifyData[uIndex], pData[uIndex]);
			return false;
		}
	}

	return true;
} // end void CXModuleRTC::(ULONG uStart, ULONG uLength, PUCHAR pData)



//////////////////////////////////////////////////////////////////////////////////
// Function name	: RtcHwrReadReg
// Description	    : Reads the contents of the RTC RAM
// Return type		: uchar 
//////////////////////////////////////////////////////////////////////////////////
UCHAR CXModuleRTC::RtcHwrReadReg(IN UCHAR Address)
{
	__asm
	{
		;Stop interrupts to avoid any potential conflict
		cli
		
		;Point to the address
		mov	al,Address
		out	RTC_ADDR_PORT,al

		;Get the data & leave returned value at al
		in	al,RTC_DATA_PORT

		;Safe to allow interrupts now
		sti
	}

} // end UCHAR CXModuleRTC::RtcHwrReadReg(UCHAR Address)



//////////////////////////////////////////////////////////////////////////////////
// Function name	: RtcHwrAtomicWriteReadReg
// Description	    : Reads and writes the contents from/to the RTC RAM
// Return type		: void 
//////////////////////////////////////////////////////////////////////////////////
void CXModuleRTC::RtcHwrAtomicWriteReadReg(PUCHAR pWriteData, PUCHAR pReadData, ULONG uStart, ULONG uLength)
{
		__asm
	{
		;Stop interrupts to make operation atomic
		cli

		;Init write counter & pointers
		mov ecx,uLength
		mov ebx,uStart
		mov edx,pWriteData

WriteLoop:
		;Set-up address
		mov	al,bl
		out	RTC_ADDR_PORT,al

		;Set-up data
		mov	al,[edx]
		out	RTC_DATA_PORT,al

		;Point to next
		inc ebx
		inc edx
		loop WriteLoop

		;Init read counter & pointers
		mov ecx,uLength
		mov ebx,uStart
		mov edx,pReadData

ReadLoop:
		;Set-up address
		mov	al,bl
		out	RTC_ADDR_PORT,al

		;Get data
		in  al,RTC_DATA_PORT
		mov	[edx],al

		;Point to next
		inc ebx
		inc edx
		loop ReadLoop

		;Safe to allow interrupts now
		sti
	}

} // end void CXModuleRTC::RtcHwrAtomicWriteReadReg(PUCHAR pWriteData, ...)




//////////////////////////////////////////////////////////////////////////////////
// Function name	: RtcHwrWriteReg
// Description	    : Writes the contents to the RTC RAM
// Return type		: void 
//////////////////////////////////////////////////////////////////////////////////
void CXModuleRTC::RtcHwrWriteReg(UCHAR Address, UCHAR Data)
{
	__asm
	{
		;Stop interrupts to avoid any potential conflict
		cli
		
		;Point to the address
		mov	al,Address
		out	RTC_ADDR_PORT,al

		;Set the data
		mov	al,Data
		out	RTC_DATA_PORT,al

		;Safe to allow interrupts now
		sti
	}

} // end void CXModuleRTC::RtcHwrWriteReg(UCHAR Address, UCHAR Data)




//////////////////////////////////////////////////////////////////////////////////
// Function name	: CheckInterrupt
// Description	    : Gets the code that indicates the status after generating
//					  interrupt.
// Return type		: ulong
//////////////////////////////////////////////////////////////////////////////////

ULONG CXModuleRTC::CheckInterrupt()
{
	ULONG uCode;

	uCode = RtcHwrCheckInterrupt();

	return uCode;

} // end ULONG CXModuleRTC::CheckInterrupt() 



//////////////////////////////////////////////////////////////////////////////////
// Function name	: RtcHwrCheckInterrupt
// Description	    : Verifies the operation of the RTC Periodic Interrupt
// Return type		: ulong
//////////////////////////////////////////////////////////////////////////////////
ULONG CXModuleRTC::RtcHwrCheckInterrupt()
{
	__asm
	{
		;Stop interrupts for a couple of micro-seconds 
		;it shouldn't cause any performance hit
		cli

		;Preserve Control Register B at bl
		mov		al,RTC_CTRL_REG_B
		out		RTC_ADDR_PORT,al
		in		al,RTC_DATA_PORT
		mov		bl,al

		;Preserve Control Register A at bh
		mov		al,RTC_CTRL_REG_A
		out		RTC_ADDR_PORT,al
		in		al,RTC_DATA_PORT
		mov		bh,al

		;Disable all RTC interrupt sources
		mov		al,RTC_CTRL_REG_B
		out		RTC_ADDR_PORT,al
		mov		al,bl
		and		al,8Fh
		out		RTC_DATA_PORT,al

		;Disable the periodic interrupt
		mov		al,RTC_CTRL_REG_A
		out		RTC_ADDR_PORT,al
		mov		al,bh
		mov		al,20h
		out		RTC_DATA_PORT,al

		;Clear any old pending interrupt
		;Do 3 attempts
		mov		ecx,3
ClearInterrupt1:
		mov		al,RTC_CTRL_REG_C
		out		RTC_ADDR_PORT,al
		in		al,RTC_DATA_PORT

		;Pre-set next error code
		mov		ah,RTC_STATUS_NO_CLEAR

		;Verify that IRQF & PF bits are clear
		mov		al,RTC_CTRL_REG_C
		out		RTC_ADDR_PORT,al
		in		al,RTC_DATA_PORT
		test	al,0C0h
		loopnz	ClearInterrupt1
		jnz		Done

		;Pre-set next error code
		mov		ah,RTC_STATUS_UNEXP_INTR

		;Verify IRQ8 inactive by reading Slave PIC IRR
		mov		al,0Ah
		out		SPIC_CMD_PORT,al
		in		al,SPIC_IRR_PORT
		test	al,01h
		jnz		Done

		;Set-up the periodic rate to 1.953125 ms
		mov		al,RTC_CTRL_REG_A
		out		RTC_ADDR_PORT,al
		mov		al,27h
		out		RTC_DATA_PORT,al

		;Enable the periodic rate interrupt
		mov		al,RTC_CTRL_REG_B
		out		RTC_ADDR_PORT,al
		mov		al,bl
		or		al,40h
		out		RTC_DATA_PORT,al

		;Pre-set next error code
		mov		ah,RTC_STATUS_NO_INTR

		;Wait for IRQ8
		mov		ecx,RTC_INTR_TIMEOUT
WaitInterrupt:
		in		al,SPIC_IRR_PORT
		test	al,01h
		loopz	WaitInterrupt
		jz		Done

		;Pre-set next error code
		mov		ah,RTC_STATUS_NO_SET

		;Verify that both IRQF & PF bits are set
		mov		al,RTC_CTRL_REG_C
		out		RTC_ADDR_PORT,al
		in		al,RTC_DATA_PORT
		and		al,0C0h
		cmp		al,0C0h
		jne		Done

		;Disable all RTC interrupt sources
		mov		al,RTC_CTRL_REG_B
		out		RTC_ADDR_PORT,al
		mov		al,bl
		and		al,8Fh
		out		RTC_DATA_PORT,al

		;Clear interrupt status
		;Do 3 attempts
		mov		ecx,3
ClearInterrupt2:
		mov		al,RTC_CTRL_REG_C
		out		RTC_ADDR_PORT,al
		in		al,RTC_DATA_PORT

		;Pre-set next error code
		mov		ah,RTC_STATUS_NO_CLEAR

		;Verify that IRQF & PF bits are now clear
		in		al,RTC_DATA_PORT
		test	al,0C0h
		loopnz	ClearInterrupt2
		jnz		Done

		;Pre-set next error code
		mov		ah,RTC_STATUS_UNEXP_INTR

		;Verify IRQ8 inactive by reading Slave PIC IRR
		; This may not work right because interrupts are disabled, so the IRR was never cleared
		; from the interrupt that we generated.
;		mov		al,0Ah
;		out		SPIC_CMD_PORT,al
;		in		al,SPIC_IRR_PORT
;		test	al,01h
;		jnz		Done

		;Successful
		mov		ah,RTC_STATUS_SUCCESS

Done:
		;Restore Control Register A from bh
		mov		al,RTC_CTRL_REG_A
		out		RTC_ADDR_PORT,al
		mov		al,bh
		out		RTC_DATA_PORT,al

		;Restore Control Register B from bl
		mov		al,RTC_CTRL_REG_B
		out		RTC_ADDR_PORT,al
		mov		al,bl
		out		RTC_DATA_PORT,al

		;Safe to allow interrupts now
		sti

		;Leave the final result at eax
		movzx	eax,ah
	}

} // end void CXModuleRTC::RtcHwrCheckInterrupt()



//////////////////////////////////////////////////////////////////////////////////
// Function name	: GetCpuSpeed
// Description	    : Get an accurate measurement of the CPU speed.  This should
//					  be the only routine that needs to use the PIT timer within
//					  the RTC module.  All other routines can use the Time Stamp 
//					  Counter (which is much easier to access) after calling this 
//					  routine to determine the CPU speed.
// Return type		: double
//////////////////////////////////////////////////////////////////////////////////
double CXModuleRTC::GetCPUSpeed()
{
	unsigned char nmi_temp_reg;
	DWORD pit_rolls = 0;
	DWORD tsc_ticks = 0;
	DWORD pit_start = 0;
	DWORD pit_end = 0;
	double dblCPUSpeed, dblPitTime;

	__asm
	{
		push eax
		push ebx
		push ecx
		push edx
		push edi
		push esi
		pushf

		cli                 ;  stop interrupts
		// Disable NMIs
		in al, 70h
		mov nmi_temp_reg, al
		or al, 80h
		out 70h, al
		// NMIs are now disabled

		in  al, RTC_PIT_PORT_B_CTRL ; Get Port B control register byte
		and al, 0FCh        ;  mask for Speaker and Timer 2 gate bits
		out RTC_PIT_PORT_B_CTRL, al ;  disable Speaker and Timer 2 gate
		mov al, 0B4h        ; Initialize Timer 2
		out RTC_PIT_TIMER_CTRL, al  ;
		mov al, 0           ; Clear Timer 2 count
		out RTC_PIT_TIMER_2, al     ;
		nop
		out RTC_PIT_TIMER_2, al     ;

		in  al, RTC_PIT_PORT_B_CTRL ; Get Port B control register byte
		or  al, 1           ;  mask for Timer 2 gate enable bit
	 	out RTC_PIT_PORT_B_CTRL, al ;  enable Timer 2 gate (start counting)
		; The timer is now running
		; Do the loop below to ensure we don't read 0000 as our first value
get_tsc_loop0:
		mov AL, 80h
		out RTC_PIT_TIMER_CTRL, al  ; Latch the count
		in  al, RTC_PIT_TIMER_2     ; Get Timer 2 count low byte
		mov ah, al          ;
		in  al, RTC_PIT_TIMER_2     ; Get Timer 2 count high byte
		xchg al, ah         ; Put low and high byte in correct order
		cmp ax, 8000h
		jbe get_tsc_loop0   ; if the new value is below or equal to 0x8000, continue
		; Now we know the timer has at least rolled over from 0000 to FFFF.
		mov al, 80h
		out RTC_PIT_TIMER_CTRL, al  ; Latch the count
		rdtsc
		mov edi, eax        ; Preserve the Least Significant 32 bits
		mov esi, edx        ; Preserve the Most Significant 32 bits
		mov eax, 0
		in  al, RTC_PIT_TIMER_2     ; Get Timer 2 count low byte
		mov ah, al          ;
		in  al, RTC_PIT_TIMER_2     ; Get Timer 2 count high byte
		xchg al, ah         ; Put low and high byte in correct order
		mov pit_start, eax  ; Record the initial PIT value
		; Now wait around for the counter to roll over several times
		mov cl, 19
get_tsc_loop1:
		mov bx, ax
		mov al, 80h
		out RTC_PIT_TIMER_CTRL, al  ; Latch the count
		in  al, RTC_PIT_TIMER_2     ; Get Timer 2 count low byte
		mov ah, al          ;
		in  al, RTC_PIT_TIMER_2     ; Get Timer 2 count high byte
		xchg al, ah         ; Put low and high byte in correct order
		cmp ax, bx
		jbe get_tsc_loop1   ; if the new value is below or equal to the old value, continue
		inc DWORD PTR pit_rolls  ; Continue doing this until the timer rolls over the correct number of times
		dec cl
		jne get_tsc_loop1
		; Now read the final PIT and TSC values
		mov al, 80h
		out RTC_PIT_TIMER_CTRL, al  ; Latch the count
		rdtsc
		sub edx, esi        ; Calculate the change in high DWORD
		sub eax, edi        ; Calculate the change in low DWORD
		jnc GTD1
		dec edx
GTD1:
		mov tsc_ticks, eax        ; Preserve the low DWORD of CPU counts
		mov eax, 0
		in  al, RTC_PIT_TIMER_2     ; Get Timer 2 count low byte
		mov ah, al          ;
		in  al, RTC_PIT_TIMER_2     ; Get Timer 2 count high byte
		xchg al, ah         ; Put low and high byte in correct order
		mov pit_end, eax ; Record the final PIT value

		in al, RTC_PIT_PORT_B_CTRL
		and al, 0feh
		out RTC_PIT_PORT_B_CTRL, al ;  disable Timer 2 gate (stop counting)

		// restore NMIs
		mov al, nmi_temp_reg
		out 70h, al
		// NMIs are now restored
		sti                 ; Start Interrupts

		popf
		pop esi
		pop edi
		pop edx
		pop ecx
		pop ebx
		pop eax
	}
	dblPitTime = (double)(pit_rolls-1) * (double)65536;
	dblPitTime = dblPitTime + (double)pit_start + (double)1;
	dblPitTime = dblPitTime + (double)(65536-pit_end);
	dblPitTime = dblPitTime - (double)1;
	dblPitTime = dblPitTime/(double)1125000.0;
	dblCPUSpeed = tsc_ticks/dblPitTime;

	return (dblCPUSpeed);

} // end double CXModuleRTC::GetCPUSpeed()


//////////////////////////////////////////////////////////////////////////////////
// Function name	: bWaitUIPandGetTimesSetTimes
// Description	    : This routine waits for an Update In Progress indication to 
//					  pass and then reads in the Time Stamp Counter and RTC time 
//				      values. If pucRTCWriteValues is not NULL, these values are 
//					  written into the RTC time bytes.
// Input			: pTSCVal - Time stamp counter value
//					: pucRTCReadValues - date and time value read
//					: pucRTCWriteValues - date and time value to be written
// Return type		: bool
//////////////////////////////////////////////////////////////////////////////////
bool CXModuleRTC::bWaitUIPandGetTimesSetTimes(LARGE_INTEGER *pTSCVal, unsigned char *pucRTCReadValues, unsigned char *pucRTCWriteValues)
{
	bool bSuccess = true;
	char cSuccessASM = 0;
	DWORD dwTSCLow, dwTSCHigh;
	unsigned char nmi_temp_reg;

	__asm
	{
		push eax
		push ebx
		push ecx
		push edx
		push edi
		push esi
		pushf

		cli                 ;  stop interrupts
		// Disable NMIs
		in al, 70h
		mov nmi_temp_reg, al
		or al, 80h
		out 70h, al
		// NMIs are now disabled

		mov al, 8Ah
		out RTC_ADDR_PORT, al
		mov edx, 15100000; This is about a 10 second timeout in the XBox Dev system
wuip1:  // Wait for the UIP bit to go high
		in al, RTC_DATA_PORT
		and al, RTC_UIP
		jne wuip2
		dec edx
		jne wuip1
		// Timed out waiting for UIP to go high
		mov cSuccessASM, 1
		jmp wuip_end1
wuip2:  // Wait for the UIP bit to go low
		in al, RTC_DATA_PORT
		and al, RTC_UIP
		je wuip2a
		dec edx
		jne wuip2
		// Timed out waiting for UIP to go high
		mov cSuccessASM, 1
		jmp wuip_end1
wuip2a:
		// The UIP just went low, so read the TSC
		rdtsc
		mov dwTSCLow, eax        ; Preserve the Least Significant 32 bits
		mov dwTSCHigh, edx       ; Preserve the Most Significant 32 bits
		// Now read the 10 time values from the RTC
		mov edx, pucRTCReadValues
		mov ecx, 10
		mov ebx, 80h ; start with RTC address 0 with NMIs disabled
wuip3:
		mov eax, ebx
		out RTC_ADDR_PORT, al
		in al, RTC_DATA_PORT
		mov BYTE PTR [edx], al
		inc ebx
		inc edx
		dec ecx
		jne wuip3
		// Now handle the century byte
;		mov eax, 0B2h
		mov eax, 0FFh
		out RTC_ADDR_PORT, al
		in al, RTC_DATA_PORT
		mov BYTE PTR [edx], al

		// Update the RTC to new time values, if applicable
		cmp pucRTCWriteValues, 0 // Don't update the RTC if no values are specified to write
		je wuip_end1
		mov edx, pucRTCWriteValues
		mov ecx, 10
		mov ebx, 80h ; start with RTC address 0 with NMIs disabled
wuip4:
		mov eax, ebx
		out RTC_ADDR_PORT, al
		mov al, BYTE PTR [edx]
		out RTC_DATA_PORT, al
		inc ebx
		inc edx
		dec ecx
		jne wuip4
		// Now handle the century byte
;		mov eax, 0B2h
		mov eax, 0FFh
		out RTC_ADDR_PORT, al
		mov al, BYTE PTR [edx]
		out RTC_DATA_PORT, al

wuip_end1:
		// restore NMIs
		mov al, nmi_temp_reg
		out 70h, al
		// NMIs are now restored
		sti                 ; Start Interrupts

		popf
		pop esi
		pop edi
		pop edx
		pop ecx
		pop ebx
		pop eax
  }
	if (cSuccessASM != 0) // UIP didn't happen in time
		bSuccess = false;
	pTSCVal->LowPart = dwTSCLow;
	pTSCVal->HighPart = dwTSCHigh;

	return bSuccess;

} // end bool CXModuleRTC::bWaitUIPandGetTimesSetTimes(LARGE_INTEGER *pTSCVal...)




//////////////////////////////////////////////////////////////////////////////////
// Function name	: vFillDateString
// Description	    : This routine displays the day of the week, date and time
// Return type		: bool
//////////////////////////////////////////////////////////////////////////////////
void CXModuleRTC::vFillDateString(LPTSTR pBuffer, unsigned char *pucRTCValues, unsigned short second)
{
	LPCTSTR DayList[] = {_T("Sunday"), _T("Monday"), _T("Tuesday"), _T("Wednesday"),
						      _T("Thursday"), _T("Friday"), _T("Saturday")};
	TCHAR sDay[80];

	if((pucRTCValues[6] >= 0x01)&&(pucRTCValues[6] <= 0x07))
		_tcscpy(sDay,DayList[pucRTCValues[6] - 1]);
	
	else
		_stprintf(sDay, _T("(Invalid Day: %02X)"),pucRTCValues[6]);
	
	_stprintf((LPTSTR)pBuffer, _T("%s, %02X/%02X/%02X%02X, %02X:%02X:%02X"), sDay, (unsigned short)pucRTCValues[8], 
		(unsigned short)pucRTCValues[7], (unsigned short)pucRTCValues[10], (unsigned short)pucRTCValues[9], (unsigned short)pucRTCValues[4], (unsigned short)pucRTCValues[2], (unsigned short)second);
}


/////////////////////////////////////////////////////////////////////////////////
// ACTUAL TESTS
//////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// Ram Test
//////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_TESTLOCKED (RTC, ramtest, 1)
{
//	#ifdef _DEBUG
		static int Ram;
		Ram++;
		ReportDebug(BIT0,_T("Ram Test - Loop %d"), Ram);
//	#endif

	UINT uiSeed = 0, uiRandScratch = 0;
	if (iStart+iLength > 0x7D) // Avoid CMOS RAM locations 0x7D, 0x7E, and 0x7F
		iLength = 0x7D - iStart;
	// debug
	/*static int x = 1;
	TCHAR StrOutput[512];
	_stprintf(StrOutput, _T("ramtest loopcount = %d\n"), x);
	OutputDebugString(StrOutput);
	x++;*/

	// Init the random number generator
	vNewSeed(&uiSeed);
	if((m_nSeed = GetCfgInt(_T("Seed"), -1)) != -1)
		uiSeed = (UINT)m_nSeed;
	uiRandScratch = uiSeed;
	//bool flag;

	PUCHAR SavedRam = new UCHAR[iLength];

	ReadRam(iStart, iLength, SavedRam);

	// debug
	/*for(int index = 0; index < 10; index++)
	{
		flag = ReadRam(iStart, iLength, SavedRam);
		if(flag == false)
			ReportDebug(BIT0,_T("ReadRam failed\n"));
	}*/
	
	CDataBuffer Pats(iNumPatterns > iLength? iNumPatterns : iLength);
	Pats.FillRandom(&uiRandScratch);

	// debug
	//ReportDebug(BIT0,_T("iNumPatterns = %d, iLength = %d\n"), iNumPatterns, iLength);
	//
	// Create the read buffer
	//
	PUCHAR Read = new UCHAR[iLength];

	//
	// Write/Read/Verify patterns. During each iteration a
	// unique pattern is used for each address.
	//
	for(int i = 0; i < iNumPatterns; i++)
	{
		//
		// Do the block write & read as an atomic operation to avoid
		// any potential conflict with the OS (added on V2.02)
		//
		AtomicWriteReadRam(iStart, iLength, Pats, Read);

		for(int j = 0; j < iLength; j++)
		{
			if (iStart + j < 0x7D) // Look at all 8 bits of the RAM byte
			{
				if(Read[j] != Pats[j])
					err_Ram(iStart + j, Pats[j], Read[j], uiSeed);
			}
/*          Don't even test the last 3 bytes becuase the behavior is strange
			else  // Look at only the lower 6 bits of the RAM byte
			{
				if((Read[j] & 0x3F) != (Pats[j] & 0x3F))
					err_Ram(iStart + j, Pats[j] & 0x3F, Read[j] & 0x3F, uiSeed);
			}
*/
		}

		Pats.RotateUp();
	}

	//
	// Release the read buffer
	//
	delete Read;

	//
	// Restore original RAM contents
	//
	WriteRam(iStart, iLength, SavedRam);

	// debug
	/*for(index = 0; index < 10; index++)
	{
		flag = WriteRam(iStart, iLength, SavedRam);
		if(flag == false)
			ReportDebug(BIT0,_T("WriteRam failed\n"));
	}*/
	delete SavedRam;

} // end IMPLEMENT_TESTLOCKED (RTC, ramtest, 1)



IMPLEMENT_TESTLOCKED (RTC, datetimeaccuracytest, 2)
{
//    #ifdef _DEBUG
		static int DTA;
		DTA++;
		ReportDebug(BIT0,_T("DateTimeAccuracy Test - Loop %d"), DTA);
//	#endif

	// debug
	/*static int x = 1;
	TCHAR StrOutput[512];
	_stprintf(StrOutput, _T("datetimeaccuracytest loopcount = %d\n"), x);
	OutputDebugString(StrOutput);
	x++;*/
	
	unsigned char end_of_year[] = {0x59, 0x00, 0x59, 0x00, 0x23, 0x00, 0x06, 0x31, 0x12, 0x99, 0x19};
    unsigned char start_of_year[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x01, 0x01, 0x00, 0x20};
	unsigned char pcInitialTime[11], pcEndTime[11];
	double dblCPUSpeed;
	double dblElapsedTime;
	double dblOneSecondTime, dblPPM;
	LARGE_INTEGER liStartTSC, liEndTSC;
	int i;
	int iElapsedTime;
	bool bCorrectEndTime = true;

	// Get an accurate calculation of the CPU speed
	dblCPUSpeed = GetCPUSpeed();
//ReportDebug(BIT0,L"CPU Speed = %lf\n", dblCPUSpeed);
	// Avoid midnight rollover (makes calculations messy)
	i = 1000; // Ten seconds worth of waiting
	while (i > 0)
	{
		if ((RtcHwrReadReg(4) == 0x23) && (RtcHwrReadReg(2) == 0x59)  && (RtcHwrReadReg(0) > 0x53))
		{
			Sleep(100);
			i--;
		}
		else
			break;
	}

	// Report an error because we timed out waiting for an UIP
	if (!bWaitUIPandGetTimesSetTimes(&liStartTSC, pcInitialTime, end_of_year))
		err_UipTimeout();
	
	Sleep(2300); // Sleep 2 seconds to allow the year to roll over
	//ReportDebug(BIT0,_T("End time = %02lx:%02lx:%02lx\n"), (ULONG)pcInitialTime[4], (ULONG)pcInitialTime[2], (ULONG)pcInitialTime[0]);
	
	// Update the initial time to compensate for the time (normally 3 seconds)
	// that elapses between the read above and the write below.  This could be
	// longer than 3 seconds in some cases, and the RTC clock will get a little off
	pcInitialTime[0] = (unsigned char)(pcInitialTime[0] + 3);
	if ((pcInitialTime[0] & 0xF) > 0x9) // Rolled over the lower seconds digit
	{
		pcInitialTime[0] = (unsigned char)(pcInitialTime[0] + 0x10);
		pcInitialTime[0] = (unsigned char)(pcInitialTime[0] - 0x0a);
	}
	if (pcInitialTime[0] > 0x59) // Rolled over the minute
	{
		pcInitialTime[0] = (unsigned char)(pcInitialTime[0] - 0x60);
		pcInitialTime[2] = (unsigned char)(pcInitialTime[2] + 1);
		if ((pcInitialTime[2] & 0xF) > 0x9) // Rolled over the lower minutes digit
		{
			pcInitialTime[2] = (unsigned char)(pcInitialTime[2] + 0x10);
			pcInitialTime[2] = (unsigned char)(pcInitialTime[2] - 0x0a);
		}
		if (pcInitialTime[2] > 0x59) // Rolled over the hour
		{
			pcInitialTime[2] = (unsigned char)(pcInitialTime[2] - 0x60);
			pcInitialTime[4] = (unsigned char)(pcInitialTime[4] + 1);
			if ((pcInitialTime[4] & 0xF) > 0x9) // Rolled over the lower hours digit
			{
				pcInitialTime[4] = (unsigned char)(pcInitialTime[4] + 0x10);
				pcInitialTime[4] = (unsigned char)(pcInitialTime[4] - 0x0a);
			}
		}
	}
	//ReportDebug(BIT0,_T("End time = %02lx:%02lx:%02lx\n"), (ULONG)pcInitialTime[4], (ULONG)pcInitialTime[2], (ULONG)pcInitialTime[0]);
	// Report an error because we timed out waiting for an UIP
	if (!bWaitUIPandGetTimesSetTimes(&liEndTSC, pcEndTime, pcInitialTime))
		err_UipTimeout();

	// As mentioned above, normally 3 seconds elapses, but sometimes more.
	// Determine how many seconds really elapsed from the TSC readings
	dblElapsedTime = ((double)liEndTSC.QuadPart - (double)liStartTSC.QuadPart)/dblCPUSpeed;
	iElapsedTime = (int)(dblElapsedTime + 0.5); // Round to the nearest second

	//ReportDebug(BIT0,_T("Elapsed time = %lf\n"), dblElapsedTime);

	// We started the seconds RTC count at 59 seconds, so decrement iElapsed Time
	// to compensate
	iElapsedTime = iElapsedTime - 1;
	// Check to make sure the end time matches the expected end time
	for (i = 0; i < sizeof(start_of_year); i++)
	{
		if (i == 0) // Seconds entry
		{
			if (pcEndTime[i] != iElapsedTime)
				bCorrectEndTime = false;
		}
		else // All other entries
		{
			if (pcEndTime[i] != start_of_year[i])
				bCorrectEndTime = false;
		}
	}

	if (!bCorrectEndTime)
	{
		// Report an error because the end time was not right
		TCHAR ExpectedString[80];
		TCHAR ActualString[80];

		vFillDateString(ExpectedString, start_of_year, (unsigned short)iElapsedTime);
		vFillDateString(ActualString, pcEndTime, (unsigned short)pcEndTime[0]);
		err_DateTime(ExpectedString, ActualString);
	}

	// Now compare the RTC time to the TSC.  iElapsedTime will contain the RTC time
	iElapsedTime = (int)(dblElapsedTime + 0.5); // Round to the nearest second

	dblOneSecondTime = dblElapsedTime/(double)iElapsedTime;
	dblPPM = (double)((double)1.0 - dblOneSecondTime) * (double)1000000;
	if (dblPPM < 0)
		dblPPM = 0 - dblPPM;

	ReportStatistic(_T("RTC PPM"), _T("%.3lf"), dblPPM);
	if (dblPPM > (double)iPpm)
		err_Accuracy((double)iPpm, dblPPM);

	 ReportDebug(BIT1, _T("PPM 1 = %lf\n"), dblPPM);
	// ReportDebug(_T("Elapsed Time = %lf\n"), dblElapsedTime);

} // end IMPLEMENT_TESTLOCKED (RTC, dateandtimetest, 2)




IMPLEMENT_TESTLOCKED (RTC, interrupttest, 3)
{
//	#ifdef _DEBUG
		static int Intr;
		Intr++;
		ReportDebug(BIT0,_T("Interrupt Test - Loop %d"), Intr);
//	#endif

	// debug
	/*static int x = 1;
	TCHAR StrOutput[512];
	_stprintf(StrOutput, _T("interrupttest loopcount = %d\n"), x);
	OutputDebugString(StrOutput);
	x++;*/

	// Call CheckInterrupt and report appropriate error messages depending
	// on the RTC status after generating interrupt.
	switch(CheckInterrupt())
	{
	case RTC_STATUS_NO_CLEAR:
		err_StatusNoClear();
		break;
	case RTC_STATUS_NO_SET:
		err_StatusNoSet();
		break;
	case RTC_STATUS_UNEXP_INTR:
		err_UnexpIntr();
		break;
	case RTC_STATUS_NO_INTR:
		err_NoIntr();
		break;
	}

} // end IMPLEMENT_TESTLOCKED (RTC, interrupttest, 3)



// This is a debugging tool. This test will dump the contents of 128 or 256
// bytes of the RTC RAM.
IMPLEMENT_TESTLOCKED (RTC, datetimedumputility, 4)
{
//	#ifdef _DEBUG
		static int RTCDump;
		RTCDump++;
		ReportDebug(BIT0,_T("DateTimeDumpUtility Test - Loop %d"), RTCDump);
//	#endif

	// debug
	/*static int x = 1;
	TCHAR StrOut[512];
	_stprintf(StrOut, _T("rtcdumputility loopcount = %d\n"), x);
	OutputDebugString(StrOut);
	x++;*/

    UCHAR index; // offset into the RTC address range 
    int size;
	//TCHAR StrOutput[512];

    /* initialize parameters */
    index = 0;
   // Read 128 bytes of the RTC data
	// 0x80 = 128
	// 0x100 = 256
    size = 0x80; 

   	ReportDebug(BIT0, _T("DAY OF WEEK: %02X, DATE(mm:dd:yy): %02X:%02X:%02X\n"), RtcHwrReadReg(6), 
				  RtcHwrReadReg(7), RtcHwrReadReg(8), RtcHwrReadReg(9));

	ReportDebug(BIT0, _T("TIME(hh:mm:ss): %02X:%02X:%02X\n"), RtcHwrReadReg(4), RtcHwrReadReg(2), 
				  RtcHwrReadReg(0));
	   
} // end IMPLEMENT_TESTLOCKED (RTC, datetimedumputility, 4)

void CXModuleRTC::vNewSeed(UINT *pSeed)
{
	SYSTEMTIME systimeSeed;
	FILETIME filetimeSeed;

	GetSystemTime(&systimeSeed);
	SystemTimeToFileTime(&systimeSeed, &filetimeSeed);
	*pSeed = (UINT)filetimeSeed.dwLowDateTime;

	return;
}

DWORD CXModuleRTC::random(UINT *pScratch)
{
	DWORD ret;
	ret  = (*pScratch = *pScratch * 214013L + 2531011L) >> 16;
	ret |= (*pScratch = *pScratch * 214013L + 2531011L) & 0xFFFF0000;
	return ret;
}
