// (c) Copyright 2000-2001 Intel Corp. All rights reserved.
//
// xmta.cpp : Defines the entry point for the console application.
//

// (The disabled warning is informational only.)
#pragma warning (disable:4073)

// pragma init_seg(lib) forces global objects declared
// in this file to be initialized before any other objects
// in the application.  This is important because the
// test factory is declared in this file.
// 
#pragma init_seg(lib)

#include "stdafx.h"

#define XMTA_VERSION _T("2.18.1.1481")
LPCTSTR version = _T("version number ") XMTA_VERSION;
TCHAR g_tcXMTAVersionNumber[] = XMTA_VERSION;


__int64 gi64CPUSpeed = 733000000;

// The test factory must be initialize before any test
// object classes, because the classes register with the
// test factory in their constructors.
//
CTestFactory CTestObj::m_factory;

CHost g_host;
CError g_error;
CParameterTable g_parameters;
CDispatcher g_dispatch;
DWORD g_tlsTestObj;
CLoopManager g_main;

#if defined( _XBOX ) 
LPDIRECT3D8				g_pD3D = NULL;		// The D3D enumerator object
LPDIRECT3DDEVICE8		g_pd3dDevice = NULL;// The D3D rendering device
D3DPRESENT_PARAMETERS	g_d3dpp;			// Present parameters used for device description.
#endif

int CustomNewHandler (size_t)
{
	throw CXmtaException (0x01, _T("Memory allocation (new) Failed"));
}

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))

//int XboxMain()
#if defined( _XBOX ) 
int main (HINSTANCE hWnd, HINSTANCE, LPWSTR lpCmdLine, INT)
{
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(hWnd);
#else // Win32
int WINAPI wWinMain (HINSTANCE, HINSTANCE, LPWSTR lpCmdLine, INT)
{
#endif
	// _set_se_translator is used to create a wrapper
	// for "structured exceptions" and allows them
	// to be caught and recognized by C++ try/catch blocks
	//
	_set_se_translator (CustomExceptionTranslator);
    _set_new_handler (CustomNewHandler);

	g_tlsTestObj = TlsAlloc ();
	TlsSetValue (g_tlsTestObj, NULL);

	TCHAR szCommandLine[128];
	memset (szCommandLine, 0, sizeof(szCommandLine));

	TCHAR inifile[128];
	memset (inifile, 0, sizeof(inifile));

#ifndef _XBOX 
	// Copy the command line into a buffer, for XOS we can make
	// no assumptions about "lpCmdLine" and therefor avoid it completely.
	//
	if ((lpCmdLine != NULL) && (lpCmdLine[0] != NULL))
		_tcsncpy(szCommandLine, lpCmdLine, LENGTH(szCommandLine) - 1);
	else
#endif
		_tcsncpy(szCommandLine, _T("xmta.ini"), LENGTH(szCommandLine) - 1);

	// Read the ini file name from the command line buffer
	// into the inifile buffer
	//
	_tcscpy (inifile, _tcstok (szCommandLine, _T(" ")));

	// Now check for the host name host name and serial number
	//

	LPTSTR hostName = _tcstok (NULL, _T(" "));
	if (hostName)
	{
		static char asciiHostName[128];
		sprintf (asciiHostName, "%S", hostName);
		g_host.m_strHostName = asciiHostName;

		// Check for a serial number too
		g_host.m_strSerialNumber = _tcstok (NULL, _T(" "));
	}

#ifdef _XBOX
	XDEVICE_PREALLOC_TYPE deviceTypes[] =
	{
//		{XDEVICE_TYPE_VOICE_HEADPHONE, 1},
		{XDEVICE_TYPE_GAMEPAD, 4},
		{XDEVICE_TYPE_MEMORY_UNIT, 8}
	};

	// This function must be called before any peripherals (game controllers, 
	// memory cards, voice units, and so on) are used or enumerated
	XInitDevices(sizeof(deviceTypes) / sizeof(XDEVICE_PREALLOC_TYPE), deviceTypes);
	
	//*****This section is used to create the global DirectX graphics devices.*****
	// Set up the structure used to create the D3DDevice.  Each subtest will override these
	//settings with a struct of their own and reset the device.
	HRESULT hr;
	ZeroMemory( &g_d3dpp, sizeof(g_d3dpp) );

	// Set fullscreen 640x480x32 mode
	g_d3dpp.BackBufferWidth        = 640;
	g_d3dpp.BackBufferHeight       = 480;
	g_d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;

	// Create one backbuffer and a zbuffer
	g_d3dpp.BackBufferCount        = 1;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	// Set up how the backbuffer is "presented" to the frontbuffer each frame
	g_d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	
	// Create the Direct3D object
	g_pD3D = Direct3DCreate8( D3D_SDK_VERSION );
	if( g_pD3D == NULL )
	{	//This is a fatal error for any graphics related test.
		g_error.err_D3dCreateObjectFailed ();
	}	
	// Create the device
	if( FAILED( hr = g_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL,
											D3DCREATE_HARDWARE_VERTEXPROCESSING,
											&g_d3dpp, &g_pd3dDevice ) ) )
	{	//This is a fatal error for any graphics related tests.
		g_error.err_D3dCreateDeviceFailed ();
		g_pd3dDevice = NULL;
	}
	//*****End of DirectX graphics device init*****

	GetCPUSpeed(); // Compute the speed of the CPU
//	TCHAR speedstring[80];
//	_stprintf(speedstring, L"XMTA XPU speed %lu\n", (DWORD)gi64CPUSpeed);
//	OutputDebugString(speedstring);
#if defined( STAND_ALONE_MODE ) // Output to a file in XBOX STAND_ALONE_MODE
//	g_host.pF = _tfopen(_T("T:\\MEDIA\\INTEL\\INFO.OUT"), _T("wb"));
	g_host.pF = _tfopen(_T("D:\\INFO.OUT"), _T("wb"));
	if (g_host.pF == NULL)
		g_host.pF = stdout;
	_ftprintf (g_host.pF, _T("\xfeff")); // Identify output file as unicode text
#endif
#endif

#ifndef STAND_ALONE_MODE
#ifndef _XBOX
	// Okay, now that we have the server name and serial number
	// we can initialize the host communications
	g_host.Init (g_host.m_strHostName, g_host.m_strSerialNumber);
#endif

/*  Add this back in to require XMTA2.INI file on all but the first boot.
    This was removed due to the fact that sometimes a box can miss the first
	boot altogether (if restarted at just the right time by an operator) and the
	fact that UEM errors could render this count inaccurate.

	// And now that we have communications initialized we can
	// ask the host how many times we have booted to select
	// the appropriate INI file
	int boot_count = g_host.GetHostDword (MID_UUT_GET_BOOT_COUNT, 0);

	LPTSTR dot = _tcsstr (inifile, _T(".ini"));
	if ((dot != NULL) && (boot_count >= 2)) 
		_tcscpy (dot, _T("2.ini"));
*/
#endif

	LPTSTR str = g_host.OpenIniFile (inifile);
	if (!str)
		return 1;

	CCommandList commandList;

	try
	{
		CParseINI ini (str);
		ini.ParseSection (_T("parameters"), &g_parameters);
		ini.ParseSection (_T("testflow"), &commandList);

		g_host.CloseIniFile ();

		g_main.StartTesting (commandList.GetList());
	}
	catch (CXmtaException x) // this is a handled error 
	{
		if (x.GetMessage())
			g_dispatch.ErrorMessage (x);
	}
//#ifndef _DEBUG
	catch (CStructuredException e)
	{
		g_error.ReportStructuredExecption (
			e.GetExceptionString (),
			e.GetExceptionCode (),
			e.GetExceptionAddress ());
	}
	catch (...)
	{
		g_error.ReportUnknownException ();
	}
//#endif

	TlsFree (g_tlsTestObj);

	if (g_host.pF != NULL)
	{
		fclose(g_host.pF);
		g_host.pF = NULL;
	}

#if defined( _XBOX ) 
	g_host.DisconnectFromHost ();
	Sleep(INFINITE);
#endif
	Sleep(15000);

	return 0;
}

/* 
// LookupString - not needed yet
// 
// This is usefull algorithm for obtaining the index
// of a string in a list.
//
int LookupString (LPCTSTR target, LPCTSTR *list);
int LookupString (LPCTSTR target, LPCTSTR *list)
{
	for (int ii = 0; list [ii] != NULL; ii++)
	{
		if (_tcsicmp (list [ii], target) == 0)
			return ii;
	}

	return -1;
}
*/

CAutoString::CAutoString (LPCTSTR format, ...)
{
	va_list argptr;
	va_start (argptr, format);

	_TCHAR buffer [1024];
	int length = _vsntprintf (buffer, 1024, format, argptr);
	
	m_str = new _TCHAR [length + 1];

	_tcscpy (m_str, buffer);
}

void GetCPUSpeed()
{
#define RTC_PIT_TIMER_2 0x042
#define RTC_PIT_TIMER_CTRL 0x43
#define RTC_PIT_PORT_B_CTRL 0x61
	unsigned char nmi_temp_reg;
	DWORD pit_rolls = 0;
	DWORD tsc_ticks = 0;
	DWORD pit_start = 0;
	DWORD pit_end = 0;
	DWORD dwPitTime;
	LARGE_INTEGER liCPUSpeed, liPitTime, litsc_ticks;

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
		mov cl, 3
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

		// restore NMIs
		mov al, nmi_temp_reg
		out 70h, al
		// NMIs are now restored
		sti                 ; Start Interrupts
		in al, RTC_PIT_PORT_B_CTRL
		and al, 0feh
		out RTC_PIT_PORT_B_CTRL, al ;  disable Timer 2 gate (stop counting)

		popf
		pop esi
		pop edi
		pop edx
		pop ecx
		pop ebx
		pop eax
	}
	dwPitTime = (DWORD)(pit_rolls-1) * (DWORD)65536;
	dwPitTime = dwPitTime + (DWORD)pit_start + 1;
	dwPitTime = dwPitTime + (DWORD)(65536-pit_end);
	dwPitTime = dwPitTime - 1;
	liPitTime.QuadPart = dwPitTime;
	litsc_ticks.QuadPart = tsc_ticks;
	liCPUSpeed.QuadPart = litsc_ticks.QuadPart * 1125000;
	liCPUSpeed.QuadPart = liCPUSpeed.QuadPart/liPitTime.QuadPart;
	gi64CPUSpeed = liCPUSpeed.QuadPart;
	return;
}
