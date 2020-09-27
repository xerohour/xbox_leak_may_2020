/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	regsuite.h

Abstract:

	Exported header file for regression suite library (regsuite.lib)

Author:

	Jeff Sullivan (jeffsul) 11-Feb-2002

[Notes:]

	Known Issues

		RSBeginTest() returns XLL_FAIL at this point for error.  It should return XLL_ABORT or XLL_SKIP when those values are defined.

		If g_D3D_PresentFlag is not set to one of the following values, the variable will not be set correctly:
		D3DPRESENTFLAG_WIDESCREEN
		D3DPRESENTFLAG_INTERLACED            
		D3DPRESENTFLAG_PROGRESSIVE
		D3DPRESENTFLAG_FIELD

		If g_D3D_MultiSampleType is not set to one of the following values, the variable will not be set correctly:
		D3DMULTISAMPLE_NONE                                    
		D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR            
		D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX          
		D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR 
		D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR   
		D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR            
		D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN          
		D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR            
		D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN          
		D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN          
		D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN


Revision History:

	11-Feb-2002 jeffsul
		Initial Version

--*/

#ifndef _REGRESSIONSUITE_H_
#define _REGRESSIONSUITE_H_

#include <xtl.h>
#include "..\..\hvs\utils\hvsUtils.h"
#include <xlog.h>
//#include <medialoader.h>
#include <macros.h>
#include <xtestlib.h>

#define RS_DONTALLOCMEMORY	0x00000001L
#define RS_DONTALLOCVOICES	0x00000002L
#define RS_DONTSETMLDEBUG	0x00000004L

typedef VOID (_stdcall *RSINIT_FUNC) ( LPCSTR szKey, LPCSTR szValue, LPVOID lpUserParam );

typedef struct
{
	// D3D
	UINT                D3D_uiBackBufferWidth;
    UINT                D3D_uiBackBufferHeight;
    D3DFORMAT           D3D_BackBufferFormat;
    UINT                D3D_uiBackBufferCount;
    D3DMULTISAMPLE_TYPE D3D_MultiSampleType;
    D3DSWAPEFFECT       D3D_SwapEffect;
	BOOL                D3D_bEnableAutoDepthStencil;
    D3DFORMAT           D3D_AutoDepthStencilFormat;
    DWORD               D3D_dwFlags;								// Display mode flag, Widescreen mode flag, 1:1.1 aspect ratio flag
    UINT                D3D_uiFullScreen_RefreshRateInHz; 
    UINT                D3D_uiFullScreen_PresentationInterval;
	BOOL				D3D_bSingleStepPusher;
	BOOL				D3D_bD3dParser;
	BOOL				D3D_bNullHardware;

	// DMusic
	BOOL				DM_bDoWorkLocally;							// “Test will call DirectMusicDoWork itself”, number, 0, 0, 1
	DWORD				DM_dwDoWorkFrequency;						// “Times per second test will call DirectMusicDoWork”, number, 60, 0, 100000 
	DWORD				DM_dwDebugLevel;							// “DirectMusic debug level”, number, 1, 0, 10
	DWORD				DM_dwRIPLevel;								// “DirectMusic RIP level”, number, 0, 0, 10
	DWORD				DM_dwTestDebugLevel;						// “DirectMusic tests’ debug level”, number, 2, 0, 10
	DWORD				DM_dwVoiceCount;							// “Max # of HW 2D voices DMusic can use”, number, 180, 0, 192
	CHAR				DM_szDMHeap[MAX_PATH];						// “Type of heap to use for DirectMusic”, string, (DMDDEFAULT), DMDEFAULT, DMFIXED, USER, USERFIXED
	DWORD				DM_dwHeapSizeNorm;							// “Sized of DM’s normal heap (if fixed size)”, number, 16777216, 0, 134217728
	DWORD				DM_dwHeapSizePhys;							// “Sized of DM’s physical heap (if fixed size)”, number, 16777216, 0, 134217728

	// DSound
	BOOL				DS_bEnableHeadphones;						// “Use EnableHeadphones()”, 0, 0, 1
	CHAR				DS_szFXImage[MAX_PATH];						// “Path of DSP code on server”, string
	CHAR				DS_szHRTF[MAX_PATH];						// “HRTF to use”,  string, (FULL), FULL, LIGHT, PAN3D

	// other 
	CHAR				RS_szTestRun[MAX_PATH];						// “An optional unique name to help identify your test run”, string2 
	CHAR				RS_szHttpServer[MAX_PATH];					// =string2 (172.26.172.13)
	CHAR				RS_szConfigPage[MAX_PATH];					// =string2 (/dxregression/submit/sysConfig.asp)
	CHAR				RS_szStatePage[MAX_PATH];					// =string2, optional
	CHAR				RS_szLogPage[MAX_PATH];						// =string2 (/dxregression/submit/log.asp)
	CHAR				RS_szPostFileName[MAX_PATH];				// =string2 (t:\webpost.log)
	BOOL				RS_bLogResults;								// “Store test results in a database”, number, 0, 0, 1 
	DWORD				RS_dwDecreaseMemory;						// “Memory to preallocate before tests are run (bytes)”, number, 0, 0, 134217728
	UINT				RS_dwDSUsedVoices2D;						// “2DVoices to burn before tests run”, number, 0, 0, 192
	UINT				RS_dwDSUsedVoices3D;						// “3DVoices to burn before tests run”, number, 0, 0, 64
	DWORD				RS_dwMLErrorLevel;							// “Media Loader debug level”, number, dwJeffsCall, 0, 10

	// test specific data passed directly through callback

} RSTESTPARAMS;

/*++

Routine Description:

	Parses launch data for test initialization.  
	If the test is launched from HVS Launcher, the data comes from the launch data.  Otherwise (test is stand alone), 
		the data comes from <XBENAME>.ini file in the same directory as the .xbe.
	The routine generates a .repro file (in the same directory as the .xbe) that will only be removed if RSEndTest() 
		is called with 0 passed as uiFail.
	The routine also allocates memory and voices as specified by launch parameters.  To override this, use the flags parameter.
	The routine will set medialoader debug level based on input.

Arguments:

	IN DWORD			dwFlags			-	0 for default behaviour, use RS_DONTALLOCXXX to avoid unwanted allocations
	OUT RSTESTPARAMS*	pTestParams		-	pointer to a struct to be filled in with launch data
	IN RSINIT_FUNC		LocalTestInit	-	pointer to function to parse remaining test-specific init values
	IN LPVOID			lpUserParam		-	pointer to a user defined variable that is passed to LocalTestInit when called

Return Value:

	XLL_PASS	-	if success
	XLL_FAIL	-	never returned
	XLL_INFO	-	?
	XLL_ABORT	-	if function fails
	XLL_SKIP	-	if current configuration is not supported

--*/
extern DWORD _stdcall
RSBeginTest( 
	IN DWORD			dwFlags,
	OUT RSTESTPARAMS*	pTestParams,
	IN RSINIT_FUNC		LocalTestInit,
	IN LPVOID			lpUserParam
);


/*++

Routine Description:

	Called at the end of a test app to give control back to the HVS launcher or reboot the box.
	If uiFail is 0, this routine deletes the .repro file created by RSInitializeTest().
	Also cleans up any memory/voices allocated by RSBeginTest().

Arguments:

	UINT64		uiPass		-	the number of tests passed
	UINT64		uiFail		-	the number of tests failed
	LPCSTR		lpszNotes	-	optional string of notes to pass back to the launcher

Return Value:

	None

--*/
extern VOID _stdcall
RSEndTest( 
	UINT64				uiPass,
	UINT64				uiFail,
	LPCSTR				lpszNotes 
);


/*++

Routine Description:

	gets a constant or enumerated value from its string equivalent

Arguments:

	IN CONST CHAR*	szStr	-	constant's name
	IN INT			cchStr	-	length of szStr
	OUT PDWORD		pval	-	pointer to DWORd to store value

Return Value:

	returns TRUE if the constant is found, FALSE otherwise

--*/
extern BOOL _stdcall
GetConst(
	IN CONST CHAR*	szStr, 
	IN INT			cchStr, 
	OUT PDWORD		pval
);

#endif // #ifndef _REGRESSIONSUITE_H_