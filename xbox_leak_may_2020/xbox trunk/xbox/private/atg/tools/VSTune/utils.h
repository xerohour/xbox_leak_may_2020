//----------------------------------------------------------------------------
// FILE: UTILS.H
//
// Desc: utils header file
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define ARRAYSIZE( _a )     ( sizeof( _a ) / sizeof( ( _a )[0] ) )




//----------------------------------------------------------------------------
// Release helper function
//----------------------------------------------------------------------------
template <class T> inline void RELEASE( T& punk )
{
    if( punk )
    {
        punk->Release();
        punk = NULL;
    }
}




//----------------------------------------------------------------------------
// Class to help time things
//----------------------------------------------------------------------------
class CTimer
{
public:
    // constructor
    CTimer()
    {
        QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond);
        ticksPerSecond /= 1000;
    }

    ~CTimer() 
    {
    };

    void Start()
    { 
        frunning = true; 
        QueryPerformanceCounter((LARGE_INTEGER *)&ticksstart); 
    }

    void Stop()
    { 
        frunning = false; 
        QueryPerformanceCounter((LARGE_INTEGER *)&ticksend); 
    }

    float getTime()
    {
        if( frunning )
            QueryPerformanceCounter((LARGE_INTEGER *)&ticksend);
        return ( ( ticksend - ticksstart ) * 1.0f / ticksPerSecond );
    }

    bool frunning;
    _int64 ticksstart;
    _int64 ticksend;
    _int64 ticksPerSecond;
};




//----------------------------------------------------------------------------
// Screen Text helper class
//----------------------------------------------------------------------------
class CScrText
{
public:
    CScrText()
    { 
        cls(); 
    }

    ~CScrText() 
    {
    }

    void cls()
    {
        m_iLine = 0;
        for( int i = 0; i < NUM_LINES; i++ )
            m_rgText[i][0] = 0;
    }

    void _cdecl printf( LPCSTR lpFmt, ... );

    void drawtext( bool fDebug );

private:
    enum 
    { 
        LINE_WIDTH = 80, 
        NUM_LINES = 40 
    };

    char m_rgText[NUM_LINES][LINE_WIDTH];
    int m_iLine;
};




// redefine these guys to make them shorter and remove the misleading D3DMULTISAMPLE_ part
#define D3DMS_NONE                                         D3DMULTISAMPLE_NONE
#define D3DMS_2_SAMPLES_MULTISAMPLE_LINEAR                 D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR
#define D3DMS_2_SAMPLES_MULTISAMPLE_QUINCUNX               D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX
#define D3DMS_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR      D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR
#define D3DMS_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR        D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR
#define D3DMS_4_SAMPLES_MULTISAMPLE_LINEAR                 D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR
#define D3DMS_4_SAMPLES_MULTISAMPLE_GAUSSIAN               D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN
#define D3DMS_4_SAMPLES_SUPERSAMPLE_LINEAR                 D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR
#define D3DMS_4_SAMPLES_SUPERSAMPLE_GAUSSIAN               D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN
#define D3DMS_9_SAMPLES_MULTISAMPLE_GAUSSIAN               D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN
#define D3DMS_9_SAMPLES_SUPERSAMPLE_GAUSSIAN               D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN
#define D3DMS_PREFILTER_FORMAT_DEFAULT                     D3DMULTISAMPLE_PREFILTER_FORMAT_DEFAULT
#define D3DMS_PREFILTER_FORMAT_X1R5G5B5                    D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5
#define D3DMS_PREFILTER_FORMAT_R5G6B5                      D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5
#define D3DMS_PREFILTER_FORMAT_X8R8G8B8                    D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8
#define D3DMS_PREFILTER_FORMAT_A8R8G8B8                    D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8




//----------------------------------------------------------------------------
// utils.cpp
bool IsLinearFormat( DWORD Format );
DWORD BitsPerPixelOfD3DFORMAT( DWORD Format );
LPCSTR GetD3DCMPStr( D3DCMPFUNC d3dcmpfunc );
LPCSTR GetD3DFormatStr( D3DFORMAT d3dformat );
LPCSTR GetMultiSampleStr( DWORD mstype );
LPCSTR GetTextureModeStr( DWORD TextureMode );




//----------------------------------------------------------------------------
void DisplayError( const char *szCall, HRESULT hr );

#define CheckHR( x ) { HRESULT _hr = ( x ); if ( FAILED( _hr ) ) { DisplayError( #x, _hr ); } }

void _cdecl dprintf( LPCSTR lpFmt, ... );

//----------------------------------------------------------------------------

