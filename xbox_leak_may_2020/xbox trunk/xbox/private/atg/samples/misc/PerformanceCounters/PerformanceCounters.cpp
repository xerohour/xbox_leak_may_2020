//-----------------------------------------------------------------------------
// File: PerformanceCounters.cpp
//
// Desc: Demonstrates how to create and query Xbox performance counters
//
// Hist: 3.20.01 - New for April XDK
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include <xbdm.h>


//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Next counter" },
};

#define NUM_HELP_CALLOUTS 2




const CHAR* g_strPerSec   = "Second";
const CHAR* g_strPerMSec  = "Millisecond";
const CHAR* g_strPerFrame = "Frame";
const CHAR* g_strPerTick  = "Tick";

LARGE_INTEGER g_liXButtonPresses = {0};
LARGE_INTEGER g_liYButtonValue   = {0};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    // Font and help
    CXBFont           m_Font;
    CXBHelp           m_Help;

    BOOL              m_bDrawHelp;

    DM_COUNTDATA      m_dmCounterData;   // Counter data
    DM_COUNTINFO      m_dmCounterInfo;   // Info on current counter
    PDM_WALK_COUNTERS m_pdmWalkCounters; // For walking counters
    HANDLE            m_hCounter;        // Handle to current counter
    HRESULT           m_hrQuery;         // HRESULT of query

    const CHAR* UnitFromEnum( DWORD dwType );

    // Function for synchronous app-defined counter
    static HRESULT __stdcall GetXButtonPresses( PLARGE_INTEGER pli, PLARGE_INTEGER pli2 );

public:
    CXBoxSample();

    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
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
    m_bDrawHelp       = FALSE;
    m_pdmWalkCounters = NULL;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Initialize the help system
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Enable GPU Performance counters so we can read them.
    DmEnableGPUCounter( TRUE );

    // Set us up for the first performance counter
    if( DmWalkPerformanceCounters( &m_pdmWalkCounters, &m_dmCounterInfo ) != XBDM_NOERR )
        return E_FAIL;
    if( FAILED( DmOpenPerformanceCounter( m_dmCounterInfo.Name, &m_hCounter ) ) )
        return E_FAIL;
    ZeroMemory( &m_dmCounterData, sizeof(DM_COUNTDATA) );
    
    DmRegisterPerformanceCounter( "X Button",
                                  DMCOUNT_EVENT | DMCOUNT_FREQ100MS | DMCOUNT_SYNC,
                                  GetXButtonPresses );
    DmRegisterPerformanceCounter( "Y Button",
                                  DMCOUNT_VALUE | DMCOUNT_ASYNC64,
                                  &g_liYButtonValue );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Handle X button
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
        ++g_liXButtonPresses.QuadPart;

    // Handle Y button
    g_liYButtonValue.QuadPart = m_DefaultGamepad.bAnalogButtons[ XINPUT_GAMEPAD_Y ];

    // Handle A button
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
    {
        if( DmWalkPerformanceCounters( &m_pdmWalkCounters, &m_dmCounterInfo ) == XBDM_ENDOFLIST )
        {
            // If we hit the end, restart from the beginning
            m_pdmWalkCounters = NULL;
            DmWalkPerformanceCounters( &m_pdmWalkCounters, &m_dmCounterInfo );
        }

        if( FAILED( DmOpenPerformanceCounter( m_dmCounterInfo.Name, &m_hCounter ) ) )
            return E_FAIL;
        ZeroMemory( &m_dmCounterData, sizeof( DM_COUNTDATA ) );
    }

    // Set up our query type
    DWORD dwQuery = m_dmCounterInfo.Type & DMCOUNT_COUNTTYPE;
    if( dwQuery == DMCOUNT_EVENT )
    {
        dwQuery |= ( m_dmCounterInfo.Type & DMCOUNT_COUNTSUBTYPE ) | DMCOUNT_PERSEC;
    }
    else if( dwQuery == DMCOUNT_VALUE )
    {
        dwQuery |= 0;
    }
    else if( dwQuery == DMCOUNT_PRATIO )
    {
        dwQuery |= 0;
    }

    m_hrQuery = DmQueryPerformanceCounterHandle( m_hCounter, dwQuery, &m_dmCounterData );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    WCHAR strTemp[512];

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
        m_Font.Begin();

        m_Font.DrawText(  64,  50, 0xffffffff, L"PerformanceCounters" );
        m_Font.DrawText( 450,  50, 0xffffff00, m_strFrameRate );
        
        m_Font.DrawText(  64, 100, 0xffffffff, L"Event counter:" );
        swprintf( strTemp,   L"\"%S\"", m_dmCounterInfo.Name );
        m_Font.DrawText( 100, 125, 0xffffff00, strTemp );

        // Build up a string to display, based on the 
        // counter type
        if( FAILED( m_hrQuery ) )
        {
            m_Font.DrawText(  64, 175, 0xffffffff, L"Query failed with:" );
            swprintf( strTemp, L"HRESULT 0x%x", m_hrQuery );
            m_Font.DrawText( 100, 200, 0xffffff00, strTemp );
        }
        else
        {
            switch( m_dmCounterInfo.Type & DMCOUNT_COUNTTYPE )
            {
                case DMCOUNT_EVENT:
                    if( m_dmCounterData.CountType & DMCOUNT_COUNTSUBTYPE )
                    {
                        m_Font.DrawText(  64, 175, 0xffffffff, L"Count:" );
                        swprintf( strTemp, L"%0.2f per %S", (float)m_dmCounterData.CountValue.QuadPart / (float)m_dmCounterData.RateValue.QuadPart,
                                           UnitFromEnum( m_dmCounterData.CountType ) );
                        m_Font.DrawText( 100, 200, 0xffffff00, strTemp );
                    }
                    else
                    {
                        m_Font.DrawText(  64, 175, 0xffffffff, L"Count:" );
                        swprintf( strTemp, L"%I64d", m_dmCounterData.CountValue.QuadPart );
                        m_Font.DrawText( 100, 200, 0xffffff00, strTemp );
                    }
                    break;

                case DMCOUNT_VALUE:
                    m_Font.DrawText(  64, 175, 0xffffffff, L"Value:" );
                    swprintf( strTemp, L"%I64d", m_dmCounterData.CountValue.QuadPart );
                    m_Font.DrawText( 100, 200, 0xffffff00, strTemp );
                    break;
            
                case DMCOUNT_PRATIO:
                    m_Font.DrawText(  64, 175, 0xffffffff, L"Ratio:" );
                    swprintf( strTemp, L"%0.2f%%", (float)m_dmCounterData.CountValue.LowPart / (float)m_dmCounterData.RateValue.LowPart * 100.0f );
                    m_Font.DrawText( 100, 200, 0xffffff00, strTemp );
                    break;

                default:
                    m_Font.DrawText(  64, 175, 0xffffffff, L"Could not query counter" );
                    break;
            }
        }

        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UnitFromEnum()
// Desc: Returns a string representing the unit in the counter type
//-----------------------------------------------------------------------------
const CHAR* CXBoxSample::UnitFromEnum( DWORD dwType )
{
    switch( dwType & DMCOUNT_COUNTSUBTYPE )
    {
        case DMCOUNT_PERSEC:
            return g_strPerSec;
        
        case DMCOUNT_PERMSEC:
            return g_strPerMSec;
        
        case DMCOUNT_PERFRAME:
            return g_strPerFrame;

        case DMCOUNT_PERTICK:
            return g_strPerTick;
    }

    return NULL;
}




//-----------------------------------------------------------------------------
// Name: GetXButtonPresses
// Desc: Returns the number of times the X Button has been pressed
//-----------------------------------------------------------------------------
HRESULT __stdcall CXBoxSample::GetXButtonPresses( LARGE_INTEGER* pli, 
                                                  LARGE_INTEGER* pli2 )
{
    (*pli) = g_liXButtonPresses;
    
    return S_OK;
}

