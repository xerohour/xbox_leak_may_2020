/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Helpers.h

Abstract:

	Helper functions for dmtest1

Author:

	Dan Haffner (danhaff) 

Revision History:

	13-Mar-2001 robheit
		Added input functions for joystick polling

--*/
#pragma once

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "globals.h"
//---------------------------------------------------------------------------

// minimum logging level
#ifndef MINLOGLEVEL
#define MINLOGLEVEL         0
#endif
// logging level for "test aborting" messages
#ifndef ABORTLOGLEVEL
#define ABORTLOGLEVEL       0
#endif
// logging level for "bug found" messages
#ifndef BUGLOGLEVEL
#define BUGLOGLEVEL         0
#endif
// logging level for "for your information" messages
#ifndef WARNLOGLEVEL
#define WARNLOGLEVEL        1
#endif
// logging level for "for your information" messages
#ifndef FYILOGLEVEL
#define FYILOGLEVEL         2
#endif
// logging level for parameter data
#ifndef PARAMLOGLEVEL
#define PARAMLOGLEVEL       5           
#endif
// logging level for structure contents
#ifndef STRUCTLOGLEVEL
#define STRUCTLOGLEVEL      6
#endif
// maximum logging level
#ifndef MAXLOGLEVEL
#define MAXLOGLEVEL         10
#endif
// max length of a logging string
#ifndef MAX_LOGSTRING
#define MAX_LOGSTRING       256
#endif
// max length of a string
#ifndef MAX_STRING
#define MAX_STRING          256
#endif

//MIDI helpers
#define MAKEPATCH(msb,lsb,patch) (((msb) << 16) | ((lsb) << 8) | (patch))
#define MAKEDRUMS(patch) ((patch) | 0x80000000)

// MIDI EVENT TYPE DEFINES
#define MIDI_NOTEOFF    0x80
#define MIDI_NOTEON     0x90
#define MIDI_PTOUCH     0xA0
#define MIDI_CCHANGE    0xB0
#define MIDI_PCHANGE    0xC0
#define MIDI_MTOUCH     0xD0
#define MIDI_PBEND      0xE0
#define MIDI_SYSX       0xF0
#define MIDI_MTC        0xF1
#define MIDI_SONGPP     0xF2
#define MIDI_SONGS      0xF3
#define MIDI_EOX        0xF7
#define MIDI_CLOCK      0xF8
#define MIDI_START      0xFA
#define MIDI_CONTINUE   0xFB
#define MIDI_STOP       0xFC
#define MIDI_SENSE      0xFE
#define CC_MODWHEEL		0x01
#define CC_VOLUME		0x07
#define CC_PAN			0x0A
#define CC_EXPRESSION	0x0B
#define CC_SUSTAIN		0x40


//Stuff for translating thread priority strings into their values.
struct THREADPRI
{
    LPCSTR sz;
    DWORD  dw;
};
extern THREADPRI g_ThreadPris[];
extern DWORD g_dwThreadPris;
HRESULT ThreadPriFromString(LPCSTR szString, DWORD *pdwPri);




/*
//===========================================================================
//===========================================================================
template <class T>
FLOAT CalculateErrorPercentage(T tExpected, T tActual)
{
FLOAT fError = 0.f;

    if (tExpected != 0.f)
        fError = (FLOAT)fabs( FLOAT(tExpected) - FLOAT(tActual) / FLOAT(tExpected)) * 100.f;
    else
        fError = (FLOAT)fabs( FLOAT(tExpected) - FLOAT(tActual) / 1.0f) * 100.f;

    return fError;
        
}
*/

double CalculateErrorPercentage(double tExpected, double tActual);

//===========================================================================
// dmthCreateTestWrappedObject
//
// template for making object creation easier
//
// Parameters:
//  REALOBJECT  *pObj   - pointer to the real object
//  TESTOBJECT  *pptObj - ptr to return newly created test object
//
// Returns: HRESULT
//===========================================================================
//lint -e10
template <class REALOBJECT, class TESTOBJECT>
HRESULT _stdcall dmthCreateTestWrappedObject(REALOBJECT *pObj, TESTOBJECT **pptObj) 
//lint +e10
{
    HRESULT hRes    = E_FAIL;

    // validate pObj
    if(!helpIsValidPtr((void*)pObj, sizeof(REALOBJECT), FALSE))
    {
        Log(MINLOGLEVEL, "Invalid ptr (Arg 1) passed to "
                "dmthCreateTestWrappedObject (%08Xh)",
                pObj);
        return E_POINTER;
    }

    // validate pptObj
    if(!helpIsValidPtr((void*)pptObj, sizeof(TESTOBJECT*), FALSE))
    {
        Log(MINLOGLEVEL, "Invalid ptr (Arg 2) passed to dmthCreateTestWrappedObject (%08Xh)", pptObj);
        return E_POINTER;
    }

    // create the test object
    *pptObj = new TESTOBJECT();
    if(NULL != *pptObj)
    {
        // initialize the test class
        hRes = (*pptObj)->InitTestClass(pObj);
        if(FAILED(hRes))
        {
            Log(MINLOGLEVEL, "**** Unable to initalize XXXXX test class (%s == %08Xh)",tdmXlatHRESULT(hRes), hRes);
            delete (*pptObj);
            *pptObj = NULL; 
        }
    }

    // done
    return hRes;

} 





//MACROS.
#define AMOUNT(a) (sizeof(a) / sizeof(a[0]))
//lint -e14
DEFINE_GUID(GUID_Random, 0x7040c11f, 0xd6da, 0x4c16, 0x99, 0x80, 0x71, 0xb, 0xe0, 0x1a, 0x69, 0x54);  // {7040C11F-D6DA-4c16-9980-710BE01A6954}
//lint +e14


//Creating and loading helpers.
HRESULT dmthCreateEmptySegment(CtIDirectMusicSegment **ppwSegment);
HRESULT dmthCreateToolGraph( CtIDirectMusicPerformance8* ptPerf8, IDirectMusicGraph** ppGraph );
HRESULT dmthCreateLoader(REFIID riid,CtIDirectMusicLoader **ppwLoader);
HRESULT dmthCreateLoader(REFIID riid, IDirectMusicLoader8 **pptLoader);
HRESULT dmthLoadSegment(LPCSTR pSegmentFile, CtIDirectMusicSegment8 **ppwSegment); //dx8
HRESULT dmthLoadSegment(LPCSTR pSegmentFile, IDirectMusicSegment8 **ppSegment); //dx8
HRESULT dmthCreatePerformance(REFIID riid, CtIDirectMusicPerformance8 **pwPerformance); //dx8
HRESULT dmthPlayMidiFile(CtIDirectMusicPerformance8 *pwPerformance8,REFIID riid,CtIDirectMusicSegment8 **ppwSegment8,CtIDirectMusicSegmentState **ppwSegmentState,CHAR *pMidiFile);

//Scriptin' stuff.
HRESULT dmthCreateScript( LPCSTR szScriptName, CtIDirectMusicLoader8* pLoader, CtIDirectMusicScript** ppScript );
HRESULT dmthInitScriptErrorInfo( DMUS_SCRIPT_ERRORINFO* pInfo );
void dmthVerifyScriptError( DMUS_SCRIPT_ERRORINFO* pInfo );


//Helpers to time playback.
HRESULT WaitForSegmentStart(CtIDirectMusicPerformance8 *ptPerf8, CtIDirectMusicSegment8 *ptSegment8, CtIDirectMusicSegmentState8 *ptSegmentState8, DWORD dwTimeout, BOOL enableBreak=FALSE);
HRESULT WaitForSegmentStop(CtIDirectMusicPerformance8 *ptPerf8, CtIDirectMusicSegment8 *ptSegment8, CtIDirectMusicSegmentState8 *ptSegmentState8, DWORD dwTimeout, BOOL enableBreak=FALSE);
HRESULT ExpectSegmentStart(CtIDirectMusicPerformance8 *ptPerf8, CtIDirectMusicSegment8 *ptSegment8, CtIDirectMusicSegmentState8 *ptSegmentState8, DWORD dwTimeout, LPCSTR szSegname, BOOL enableBreak = FALSE);
HRESULT ExpectSegmentStop(CtIDirectMusicPerformance8 *ptPerf8, CtIDirectMusicSegment8 *ptSegment8, CtIDirectMusicSegmentState8 *ptSegmentState8, DWORD dwTimeout, LPCSTR szSegname, BOOL enableBreak = FALSE);
HRESULT Wait(DWORD dwWait);
HRESULT CountDown(DWORD dwWait, DWORD dwInterval);
HRESULT GetTempo(CtIDirectMusicPerformance8 *ptPerf8, double *pdblTempo);

//Path parsing.
HRESULT ChopPath(const char *szFullString, LPSTR szPathOut, LPSTR szFileOut);
LPCSTR PathFindFileName(LPCSTR pPath);
HRESULT GUIDsFromFileName(LPSTR szFilename, GUID **ppCLSID, GUID **ppIID);


//Other
HRESULT WrapTool(IDirectMusicTool *pTool, CtIDirectMusicTool **pptTool);
LPVOID TDMGetBogusPointer(void);
void TDMFreeBogusPointer(LPVOID lpv);
BOOL FillBufferWithSineWave(
 LPVOID			    pBuffer, 
 DWORD				dwBufferBytes,
 DWORD              dwFrequency,
 LPWAVEFORMATEX		pWfx,
 DOUBLE				fFactor
 );
DWORD IgnorePChannel(DWORD dwStage);
void CleanUpIUnknown(HRESULT hr, LPUNKNOWN &pUnk);
HRESULT CreateAndInitPerformance(CtIDirectMusicPerformance8 **ppPerf8);

LPCSTR PathFindFileExtension(LPCSTR pPath);

//------------------------------------------------------------------------------
//	Input Helpers
//------------------------------------------------------------------------------
struct DMTHJoystick
{
	float	leftStickX;
	float	leftStickY;
	float	rightStickX;
	float	rightStickY;
	float	a;
	float	b;
	float	x;
	float	y;
	float	black;
	float	white;
	float	leftTrigger;
	float	rightTrigger;
	BOOL	back;
	BOOL	start;
	BOOL	dPadLeft;
	BOOL	dPadRight;
	BOOL	dPadUp;
	BOOL	dPadDown;
	BOOL	leftStick;
	BOOL	rightStick;
};
static const DWORD DMTH_JOYBUTTON_X			= 0x00000001;
static const DWORD DMTH_JOYBUTTON_Y			= 0x00000002;
static const DWORD DMTH_JOYBUTTON_WHITE		= 0x00000004;
static const DWORD DMTH_JOYBUTTON_A			= 0x00000008;
static const DWORD DMTH_JOYBUTTON_B			= 0x00000010;
static const DWORD DMTH_JOYBUTTON_BLACK		= 0x00000020;
static const DWORD DMTH_JOYBUTTON_LTRIG		= 0x00000040;
static const DWORD DMTH_JOYBUTTON_RTRIG		= 0x00000080;
static const DWORD DMTH_JOYBUTTON_LSTICK	= 0x00000100;
static const DWORD DMTH_JOYBUTTON_RSTICK	= 0x00000200;
static const DWORD DMTH_JOYBUTTON_UP		= 0x00000400;
static const DWORD DMTH_JOYBUTTON_DOWN		= 0x00000800;
static const DWORD DMTH_JOYBUTTON_LEFT		= 0x00001000;
static const DWORD DMTH_JOYBUTTON_RIGHT		= 0x00002000;
static const DWORD DMTH_JOYBUTTON_BACK		= 0x00004000;
static const DWORD DMTH_JOYBUTTON_START		= 0x00008000;
static const DWORD DMTH_JOYBUTTON_ALL		= 0xffffffff;

extern HANDLE		g_dmthInputHandles[4];
extern DMTHJoystick	g_dmthJoysticks[4];
enum ANSWER {EXPECT_YES, EXPECT_NO};


#define TESTNOTIFICATION_MEASUREANDBEAT     1
#define TESTNOTIFICATION_PERFORMANCE        2
#define TESTNOTIFICATION_SEGMENT            4
#define TESTNOTIFICATION_ALL                7            

#define CLOSEDOWN_BEFORESTOPPING 1
#define CLOSEDOWN_BEFOREUNLOADING 2
#define CLOSEDOWN_BEFORERELEASINGPATH 3
#define CLOSEDOWN_AFTEREVERYTHINGELSE 4
#define CLOSEDOWN_TWICE 5


void dmthInitInput(void);		// Will be called internally if not explicitly called
void dmthReleaseInput(UINT port);	
void dmthReleaseInput(void);		// Must be called before exit to clean up
void dmthGetJoystickStates(void);
BOOL dmthGetJoystickState(UINT port);
void dmthWaitForAllButtons(UINT port, DWORD buttonMask);
DWORD dmthWaitForAnyButton(DWORD buttonMask);
BOOL dmthIsAnyButtonDown(void);
BOOL dmthIsPortValid(UINT port);
void dmthPromptUserTestResults(HRESULT &hr, IN ANSWER   yesNoExpected,IN LPSTR	yesNoQuestion, ...);
void dmthWaitForAllButtonsUp(void);
BOOL FloatsAreEqual(const float& f1, const float& f2);
HRESULT DownloadScratch(IDirectSound *pDSound, PCHAR pszScratchFile);
HRESULT InitializeDSound(IDirectSound **ppDSound);
HRESULT ClearAllPMsgs(CtIDirectMusicPerformance8 *ptPerf8);
HRESULT HelpLoadWaveFileData(LPCSTR pszFile, LPCWAVEFORMATEX *ppwfxFormat, LPVOID *ppvAudioData, LPDWORD pdwAudioDataSize);
HRESULT StringToGuid(char* szString, LPGUID pguidResult);

extern LPCSTR g_szAppName;

void GetMemoryStats(LONG *pLongArray, DWORD dwMax);
void CompareMemoryStats(LONG *pLast, LONG *pCurrent, DWORD dwMax, LPSTR szTestName);
void EndTestRun(void);
BOOL IsValidAudioPath(DWORD dwAudioPath);


BOOL InitPerformanceEveryTest(void);
BOOL InitPerformanceOnce(void);
BOOL InitPerformanceNever(void);

HRESULT ReferenceToMusicTime(CtIDirectMusicPerformance8 *ptPerf8, REFERENCE_TIME rtTime, MUSIC_TIME* pmtTime);
HRESULT MusicToReferenceTime(CtIDirectMusicPerformance8 *ptPerf8, MUSIC_TIME mtTime, REFERENCE_TIME *prtTime);


//Copies one file.
HRESULT MediaCopyFile(LPCSTR szFromPath, LPCSTR szToPath, DWORD dwFlags);
HRESULT MediaCopyFile(LPCSTR szFromPath, LPCSTR szToPath);
HRESULT MediaCopyFile(LPCSTR szFromPath, DWORD dwFlags);
HRESULT MediaCopyFile(LPCSTR szFromPath);


//Takes the name of a file (or directory), strips off the file name, and copies everything in that directory to the "to" destination.
HRESULT MediaCopyDirectory(LPCSTR szFromPath, LPCSTR szToPath, DWORD dwFlags);
HRESULT MediaCopyDirectory(LPCSTR szFromPath, LPCSTR szToPath);
HRESULT MediaCopyDirectory(LPCSTR szFromPath, DWORD dwFlags);
HRESULT MediaCopyDirectory(LPCSTR szFromPath);

//Translates a path on the server to a corresponding local path.

//TODO: Translate all calls to the second one; delete the first one.
LPCSTR MediaServerToLocal(LPCSTR szFrom);
HRESULT MediaServerToLocal(LPCSTR szServer, LPSTR szLocal);


BOOL FilesInSameDirectory(LPSTR szFiles[], DWORD dwCount);
