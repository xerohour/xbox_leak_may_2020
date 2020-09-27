//-----------------------------------------------------------------------------
// File: DXUtil.h
//
// Desc: Helper functions and typing shortcuts for DirectX programming.
//
// Copyright (c) 1997-2000 Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
#ifndef DXUTIL_H
#define DXUTIL_H

//-----------------------------------------------------------------------------
// Helpers for handling errors.
//-----------------------------------------------------------------------------

void DisplayError(const WCHAR *szCall, HRESULT hr);

#define CheckHR(x) { HRESULT _hr = (x); if (FAILED(_hr)) { DisplayError(L#x, _hr); } }

//-----------------------------------------------------------------------------
// Miscellaneous helper functions
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

//-----------------------------------------------------------------------------
// Name: DXUtil_GetDXSDKMediaPath() and DXUtil_FindMediaFile() 
// Desc: Find a file in the media directory
//-----------------------------------------------------------------------------
void   DXUtil_FindMediaFile( CHAR* strPath, CHAR* strFilename );

//-----------------------------------------------------------------------------
// Name: DXUtil_Timer()
// Desc: Performs timer opertations. Must be reset before use.
//-----------------------------------------------------------------------------

class DXUtil_Timer
{
	static float GetTime() 
	{
        LARGE_INTEGER qwTime;

        QueryPerformanceCounter( &qwTime );
        return ((float)qwTime.QuadPart) * m_fSecsPerTick;
	}

	static FLOAT m_fSecsPerTick;
	static FLOAT m_fBaseTime;
	static FLOAT m_fStopTime;

public:

	static void Reset()
	{
        LARGE_INTEGER qwTicksPerSec;

        QueryPerformanceFrequency(&qwTicksPerSec);

        m_fSecsPerTick = 1.0f / (FLOAT)qwTicksPerSec.QuadPart;

        m_fBaseTime = GetTime();
	}

	static float GetAppTime()
	{
        return GetTime() - m_fBaseTime;
	}

	static float GetAbsoluteTime()
	{
        return GetTime();
	}

	static void Start()
	{
        m_fBaseTime += GetTime() - m_fStopTime;
	}

	static void Stop()
	{
        m_fStopTime = GetTime();
	}

	static void Advance()
	{
        m_fBaseTime += GetTime() - ( m_fStopTime + 0.1f );
	}
};

VOID DXUtil_ConvertWideStringToAnsi( CHAR* strDestination, const WCHAR* wstrSource, 
                                     int cchDestChar );
VOID DXUtil_ConvertGenericStringToAnsi( CHAR* strDestination, const TCHAR* tstrSource, 
                                        int cchDestChar );

#endif // DXUTIL_H
