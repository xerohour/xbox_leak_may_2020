//-----------------------------------------------------------------------------
// File: FPConv.cpp
//
// Desc: Code to time floating point conversion routines
//
// Hist: 4.23.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
};

#define NUM_HELP_CALLOUTS 1

#define NUM_ENTRIES 100000L

typedef FLOAT (*CONVERSIONPROC)(FLOAT * pf, LONG * pl, LONG n);

typedef struct
{
    WCHAR * szDescription;
    CONVERSIONPROC pfn;
} CONV;

FLOAT TimeFToL( FLOAT * pf, LONG * pl, LONG n );
FLOAT TimeSSE1( FLOAT * pf, LONG * pl, LONG n );
FLOAT TimeSSE2( FLOAT * pf, LONG * pl, LONG n );
FLOAT Timefistp1( FLOAT * pf, LONG * pl, LONG n );
FLOAT Timefistp2( FLOAT * pf, LONG * pl, LONG n );
FLOAT TimeNothing( FLOAT * pf, LONG * pl, LONG n );
CONV g_ac[] = 
{
    { L"ftol()",            TimeFToL },
    { L"cvttss2si",         TimeSSE1 },
    { L"fstp; cvttss2si",   TimeSSE2 },
    { L"fistp result",      Timefistp1 },
    { L"fistp esp",         Timefistp2 },
    { L"Nothing",           TimeNothing },
};
#define NUM_METHODS 6

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    CXBoxSample();

    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();

    // Font and help
    CXBFont     m_Font;
    CXBHelp     m_Help;

    // Data to process
    FLOAT *     m_pfFloats;
    LONG *      m_plLongs;

    // Timing 
    LARGE_INTEGER m_liFreq;
    FLOAT       m_fTimes[NUM_METHODS];

    BOOL        m_bDrawHelp;
};



//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}



//-----------------------------------------------------------------------------
// Name: CXBoxSample (constructor)
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
            :CXBApplication()
{
    m_bDrawHelp = FALSE;
}



//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;    

    m_pfFloats = new FLOAT[ NUM_ENTRIES ];
    m_plLongs  = new LONG[ NUM_ENTRIES ];
    QueryPerformanceFrequency( &m_liFreq );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    for( int i = 0; i < NUM_METHODS; i++ )
    {
        for( int j = 0; j < NUM_ENTRIES; j++ )
        {
            m_pfFloats[ j ] = float(rand()) / float(RAND_MAX);
        }
        ZeroMemory( m_plLongs, NUM_ENTRIES * sizeof( LONG ) );

        m_fTimes[ i ] = g_ac[ i ].pfn( m_pfFloats, m_plLongs, NUM_ENTRIES );
        // m_fTimes[ i ] = i;
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        WCHAR sz[100];
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"FPConv" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        swprintf( sz, L"%d entries", NUM_ENTRIES );
        m_Font.DrawText( 64, 75, 0xffffff00, sz );

        for( int i = 0; i < NUM_METHODS; i++ )
        {
            swprintf( sz, L"%s: %0.8f", g_ac[ i ].szDescription, m_fTimes[ i ] );
            m_Font.DrawText( 64, FLOAT(75 + ( i + 1 ) * 25), 0xffffffff, sz );
        }

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

#define BEGIN_TIMING(XX) \
int i = n - 1;\
LARGE_INTEGER liFreq, liBegin, liEnd;\
QueryPerformanceFrequency( &liFreq );\
QueryPerformanceCounter( &liBegin );\
for( ; i >= 0; i-- ) \
{


#define END_TIMING(XX) \
} \
QueryPerformanceCounter( &liEnd );\
return float( liEnd.QuadPart - liBegin.QuadPart ) / float(liFreq.QuadPart);

__forceinline LONG FTOL( FLOAT f )
{
    return (LONG)f;
}

FLOAT TimeFToL( FLOAT * pf, LONG * pl, LONG n )
{
    BEGIN_TIMING(FTOL);
    pl[i] = FTOL( pf[i] );
    END_TIMING(FTOL);
}

__forceinline LONG sse1( FLOAT f )
{
    _asm cvttss2si eax, f
}

FLOAT TimeSSE1( FLOAT * pf, LONG * pl, LONG n )
{
    BEGIN_TIMING(SSE1);
    pl[i] = sse1( pf[i] );
    END_TIMING(SSE1);
}

__forceinline LONG sse2( FLOAT f )
{
    _asm {
        cvttss2si eax, f;
    }
}

FLOAT TimeSSE2( FLOAT * pf, LONG * pl, LONG n )
{
    BEGIN_TIMING(SSE2);
    pl[i] = sse2( pf[i] );
    END_TIMING(SSE2);
}

__forceinline LONG fistp1( FLOAT f )
{
    _asm {
        fld f
        fwait
        sub esp, 8;
        fistp [esp];
        mov eax, [esp];
        mov edx, [esp+4];
        add esp, 8;
    }
}

FLOAT Timefistp1( FLOAT * pf, LONG * pl, LONG n )
{
    WORD oldcw, newcw;

    _asm {
        fstcw   [oldcw];
        fwait;
        mov     ax, [oldcw]             ; round mode saved
        or      ah, 0ch                 ; set chop rounding mode
        mov     [newcw], ax             ; back to memory
        fldcw   [newcw]                 ; reset rounding
    }

    BEGIN_TIMING(FISTP1);
    pl[i] = fistp1( pf[i] );
    END_TIMING(FISTP1);
    _asm {
        fldcw [oldcw];
    }
}

__forceinline LONG fistp2( FLOAT f )
{
    DWORD result[2];

    _asm
    {
        fld f
        wait;
        fistp qword ptr [result];
    }

    return result[0];
}

FLOAT Timefistp2( FLOAT * pf, LONG * pl, LONG n )
{
    WORD oldcw, newcw;

    _asm {
        fstcw   [oldcw];
        fwait;
        mov     ax, [oldcw]             ; round mode saved
        or      ah, 0ch                 ; set chop rounding mode
        mov     [newcw], ax             ; back to memory
        fldcw   [newcw]                 ; reset rounding
    }

    BEGIN_TIMING(FISTP1);
    pl[i] = fistp2( pf[i] );
    END_TIMING(FISTP1);
    _asm {
        fldcw [oldcw];
    }
}

FLOAT TimeNothing( FLOAT * pf, LONG * pl, LONG n )
{
    BEGIN_TIMING(NOTHING);
    _asm nop
    END_TIMING(NOTHING);
}
