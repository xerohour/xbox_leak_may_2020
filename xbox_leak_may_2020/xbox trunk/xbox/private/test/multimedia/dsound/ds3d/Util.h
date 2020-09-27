#pragma once

//CONSOLE's intent is to display stuff on the XBox screen.
#define CONSOLE xDebugStringA
void xDebugStringA(LPSTR szFormat, ...);

//Logs to both the debugger and the log file.
void LogBoth(DWORD wPassOrFail, LPSTR szFormat, ...);


/********************************************************************************
BUGBUG: These are crap functions cuz the D3DOVERLOADS don't work.
BUG 2371 Overloaded functions unusable on _D3DVECTOR due to inclusion of less functional _D3DVECTOR class in D3D8TYPES.H
//TODO: blow away when D3DOVERLOADS work.
********************************************************************************/
_D3DVECTOR operator - (const _D3DVECTOR& v, const _D3DVECTOR& w);
_D3DVECTOR operator -= (_D3DVECTOR& v, const _D3DVECTOR& w);
BOOL operator == (_D3DVECTOR& v, const _D3DVECTOR& w);
D3DVECTOR x_D3DVECTOR(FLOAT _x, FLOAT _y, FLOAT _z);


HRESULT LoadWaveFile(LPDSBUFFERDESC *ppdsbd, LPVOID *ppvSoundData, LPDIRECTSOUNDBUFFER *ppBuffer, char *szFileName);
HRESULT FreeDSBD(LPDSBUFFERDESC &pdsbd);

//Sets the x, y, or z component of a vector where x, y, z are indexed by values 0-2.
void SetComponent(D3DVECTOR *pVector, DWORD dwComponent, FLOAT fValue);

HRESULT Help_DirectSoundCreate(DWORD dwDeviceId, LPDIRECTSOUND *ppDirectSound, LPUNKNOWN pUnkOuter);
HRESULT Help_DirectSoundCreateBuffer(DWORD dwDeviceId, LPCDSBUFFERDESC pdsbd, LPDIRECTSOUNDBUFFER *ppBuffer, LPUNKNOWN pUnkOuter);
//HRESULT DownloadScratch(IDirectSound *pDSound, PCHAR pszScratchFile);
HRESULT DownloadLinkedDSPImage(IDirectSound *pDSound);


// Constants
#define PI 3.141592653589793238
#define PI_TIMES_TWO 6.28318530718
#define PI_OVER_TWO 1.570796326795
#define THREE_PI_OVER_TWO 4.712388980385
#define NEG_PI_OVER_TWO -1.570796326795
#define C180_OVER_PI 57.29577951308
#define PI_OVER_360 0.008726646259972
#define TWO_OVER_PI 0.6366197723676
#define SPEEDOFSOUND 359660.0
#define LOGE_2_INV 1.44269504088896
#define LOG10_2 0.30102999566398


char *String(TestApplies eTestApply);
void SleepEx(DWORD dwWait);

extern char *WAVEFILES;
extern char *WAVEFILESDIR;


//Set this to false to disable waiting.
extern BOOL g_bWait;
