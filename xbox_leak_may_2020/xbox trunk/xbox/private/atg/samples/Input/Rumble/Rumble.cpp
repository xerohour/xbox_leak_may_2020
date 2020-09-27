//-----------------------------------------------------------------------------
// File: Rumble.cpp
//
// Desc: This sample is a demonstration of the rumble capabilities of the Xbox
//       controller.
//
// Hist: 05.30.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xgraphics.h>
#include <stdio.h>
#include <XBApp.h>
#include <XBInput.h>
#include <XBFont.h>
#include <XBUtil.h>




//-----------------------------------------------------------------------------
// Structures and Macros
//-----------------------------------------------------------------------------
struct SCREENVERTEX
{
    D3DXVECTOR4 pos;   // The transformed position for the vertex point.
    DWORD       color; // The vertex color. 
};
#define D3DFVF_SCREENVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

enum State
{
   TEST,
   DEMO,
};

// Some colors
#define SEMITRANS_BLACK     0x40000000
#define COLOUR_BLACK        0xff000000
#define COLOUR_WHITE        0xffffffff
#define COLOUR_YELLOW       0xffffff00
#define COLOUR_RED          0xffff0000
#define COLOUR_DARK_RED     0xff500000
#define COLOUR_GREY         0xff404040
#define COLOUR_LIGHT_BLUE   0xff6060ff


#define NUM_SAMPLES 400
#define CHART_LEFT   ((640.0f-NUM_SAMPLES)/2)
#define CHART_RIGHT  (640.0f-CHART_LEFT)
#define CHART_TOP1     130.0f
#define CHART_BOTTOM1  270.0f
#define CHART_TOP2     290.0f
#define CHART_BOTTOM2  430.0f



//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    // Fonts
    CXBFont    m_Font16;
    CXBFont    m_Font12;

    // Current display states
    enum State m_eCurrentState;


    // Rumble variables
    FLOAT      m_fLeftMotorSpeed;
    FLOAT      m_fRightMotorSpeed;
    BOOL       m_bPlayWave;
    WORD       m_wLeftPattern;
    WORD       m_wRightPattern;
    INT        m_iRightMotor[NUM_SAMPLES];
    INT        m_iLeftMotor[NUM_SAMPLES];
    WORD       m_wCurrentPos;

    // Member variables
    VOID    DrawTest();
    VOID    DrawDemo();
    VOID    DrawChart();
    VOID    DrawMotorGraph();
    VOID    SetMotorGraph();
    VOID    DrawLine( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, 
                            DWORD dwColor );
    VOID    DrawRect( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, 
                            DWORD dwStartColor, DWORD dwEndColor );
    VOID    DrawRectOutline( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, 
                                   DWORD dwColor );
    INT     RandomNum( INT low, INT high );

public:
    HRESULT Initialize();
    HRESULT Render();
    HRESULT FrameMove();

    CXBoxSample();
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
// Name: CXBoxSample()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    m_eCurrentState  = TEST;

    m_fLeftMotorSpeed  = 0.0f;
    m_fRightMotorSpeed = 0.0f;
    m_bPlayWave        = FALSE;
    m_wCurrentPos      = 0;
    m_wLeftPattern     = 1;
    m_wRightPattern    = 1;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create fonts
    if( FAILED( m_Font16.Create( g_pd3dDevice, "Font16.xpr" ) ) )
        return E_FAIL;
    if( FAILED( m_Font12.Create( g_pd3dDevice, "Font12.xpr" ) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START )
    {
        if( m_eCurrentState == TEST )
            m_eCurrentState = DEMO;
        else
            m_eCurrentState = TEST;

        m_fLeftMotorSpeed  = 0.0f;
        m_fRightMotorSpeed = 0.0f;
        m_bPlayWave        = FALSE;
        m_wCurrentPos      = 0;
        SetMotorGraph();
    }

    switch( m_eCurrentState )
    {
        case TEST:
            // Set rumble motors based on left and right trigger controls
            m_fLeftMotorSpeed  = m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]/255.0f;
            m_fRightMotorSpeed = m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]/255.0f;

            break;

        case DEMO:
            if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
            {
                m_bPlayWave = !m_bPlayWave;
                
                m_fLeftMotorSpeed  = 0.0f;
                m_fRightMotorSpeed = 0.0f;
                m_wCurrentPos      = 0;
            }

            if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
            {
                m_bPlayWave        = FALSE;
                m_fLeftMotorSpeed  = 0.0f;
                m_fRightMotorSpeed = 0.0f;
                m_wCurrentPos      = 0;
                if( ++m_wLeftPattern > 7 )
                    m_wLeftPattern = 1;

                SetMotorGraph();
            }

            if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
            {
                m_bPlayWave        = FALSE;
                m_fLeftMotorSpeed  = 0.0f;
                m_fRightMotorSpeed = 0.0f;
                m_wCurrentPos      = 0;
                if( ++m_wRightPattern > 7 )
                    m_wRightPattern = 1;

                SetMotorGraph();
            }

            if( m_bPlayWave )
            {
                m_fLeftMotorSpeed  = m_iLeftMotor[m_wCurrentPos]/100.0f;
                m_fRightMotorSpeed = m_iRightMotor[m_wCurrentPos]/100.0f;
                if( ++m_wCurrentPos >= NUM_SAMPLES )
                    m_wCurrentPos = 0;
            }

            break;
    }

    // Set the rumble motors.
    for( DWORD i=0; i<4; i++ )
    {
        if( m_Gamepad[i].hDevice )
        {
            // Check to see if we are still transferring current motor value
            if( m_Gamepad[i].Feedback.Header.dwStatus != ERROR_IO_PENDING )
            {
                // We can alter the motor values
                m_Gamepad[i].Feedback.Rumble.wLeftMotorSpeed  = WORD( m_fLeftMotorSpeed  * 65535.0f );
                m_Gamepad[i].Feedback.Rumble.wRightMotorSpeed = WORD( m_fRightMotorSpeed * 65535.0f );
                XInputSetState( m_Gamepad[i].hDevice, &m_Gamepad[i].Feedback );
            }
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Draw a gradient filled background
    RenderGradientBackground( 0xff0000ff, 0xff000000 );

    // Depending on the app state, render the scene
    switch( m_eCurrentState )
    {
        case TEST:
            DrawTest();
            break;

        case DEMO:
            DrawDemo();
            break;
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawTest()
// Desc: Overall rendering of the test screen
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawTest()
{
    WCHAR strBuffer[20];

    // Draw a rectangle and a semitrans poly to put the text in
    DrawRect( 64, 50, 576, 112, SEMITRANS_BLACK, SEMITRANS_BLACK );
    DrawRectOutline( 64, 50, 576, 112, COLOUR_BLACK );

    // Draw the left motor value
    DrawRect( 220.0f-20.0f, 350.0f - 2*(WORD)(100*m_fLeftMotorSpeed), 
              220.0f+20.0f, 350.0f, COLOUR_RED, COLOUR_DARK_RED );
    DrawRectOutline( 220.0f-20.0f, 150.0f, 
                     220.0f+20.0f, 350.0f, COLOUR_YELLOW );

    // Draw the right motor value
    DrawRect( 420.0f-20.0f, 350.0f - 2*(WORD)(100*m_fRightMotorSpeed),
              420.0f+20.0f, 350.0f, COLOUR_RED, COLOUR_DARK_RED );
    DrawRectOutline( 420.0f-20.0f, 150.0f, 
                     420.0f+20.0f, 350.0f, COLOUR_YELLOW );

    // Draw the text labels
    m_Font16.DrawText(  69,  50, 0xffffff00, L"Rumble" );
    m_Font16.DrawText( 571,  50, 0xffffffff, L"Motor Test Page", XBFONT_RIGHT );
    m_Font12.DrawText(  69,  75, 0xffffffff, L"Use left and right triggers to alter the rumble motors' speeds.\n"
                                             L"Press START to go to the Waveform Playback Page." );
    swprintf( strBuffer, L"Left : %d%%", (WORD)(100*m_fLeftMotorSpeed) );
    m_Font16.DrawText( 220.0f, 360.0f, 0xffffff00, strBuffer, XBFONT_CENTER_X );
    swprintf( strBuffer, L"Right : %d%%", (WORD)(100*m_fRightMotorSpeed) );
    m_Font16.DrawText( 420.0f, 360.0f, 0xffffff00, strBuffer, XBFONT_CENTER_X );
}




//-----------------------------------------------------------------------------
// Name: DrawDemo()
// Desc: Overall rendering of the demo screen
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawDemo()
{
    // Draw a rectangle and a semitrans poly to put the text in
    DrawRect( 64, 50, 576, 112, SEMITRANS_BLACK, SEMITRANS_BLACK );
    DrawRectOutline( 64, 50, 576, 112, COLOUR_BLACK );

    // Draw the text labels
    m_Font16.DrawText(  69,  50, 0xffffff00, L"Rumble" );
    m_Font16.DrawText( 571,  50, 0xffffff00, L"Waveform Playback Page", XBFONT_RIGHT );
    m_Font12.DrawText(  69,  75, 0xffffffff, L"Press X to start playback, and press A/B to select new waveforms.\n"
                                             L"Press START to go to the Motor Test Page." );

    DrawMotorGraph();
    DrawChart();
}




//-----------------------------------------------------------------------------
// Name: DrawMotorGraph()
// Desc: Displays the waveform and the playback position line
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawMotorGraph()
{
    for( int x = 0; x < NUM_SAMPLES; x++ )
    {
        DrawLine( CHART_LEFT + x*1.0f, CHART_BOTTOM1,
                  CHART_LEFT + x*1.0f, CHART_BOTTOM1 - 1.5f * m_iLeftMotor[x],
                  COLOUR_LIGHT_BLUE );

        DrawLine( CHART_LEFT + x*1.0f, CHART_BOTTOM2,
                  CHART_LEFT + x*1.0f, CHART_BOTTOM2 - 1.5f * m_iRightMotor[x],
                  COLOUR_LIGHT_BLUE);
    }

    // Playback line
    DrawLine( CHART_LEFT + m_wCurrentPos, CHART_TOP1, 
              CHART_LEFT + m_wCurrentPos, CHART_BOTTOM2, 
              D3DCOLOR_RGBA( 0xff, 0xff, 0x00, 0xff ) );
}




//-----------------------------------------------------------------------------
// Name: DrawMotorGraph()
// Desc: Displays text and static graphics for the test screen
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawChart()
{
    // Top and bottom chart
    DrawLine( CHART_LEFT-1, CHART_TOP1,    CHART_LEFT-1,  CHART_BOTTOM1, COLOUR_WHITE );
    DrawLine( CHART_LEFT-1, CHART_BOTTOM1, CHART_RIGHT+1, CHART_BOTTOM1, COLOUR_WHITE );
    DrawLine( CHART_LEFT-1, CHART_TOP2,    CHART_LEFT-1,  CHART_BOTTOM2, COLOUR_WHITE );
    DrawLine( CHART_LEFT-1, CHART_BOTTOM2, CHART_RIGHT+1, CHART_BOTTOM2, COLOUR_WHITE );

    m_Font16.DrawText( CHART_LEFT-5, CHART_TOP1,       0xffffff00, L"100%", XBFONT_RIGHT );
    m_Font16.DrawText( CHART_LEFT-5, CHART_BOTTOM1-25, 0xffffff00,   L"0%", XBFONT_RIGHT );
    m_Font16.DrawText( CHART_LEFT-5, CHART_TOP2,       0xffffff00, L"100%", XBFONT_RIGHT );
    m_Font16.DrawText( CHART_LEFT-5, CHART_BOTTOM2-25, 0xffffff00,   L"0%", XBFONT_RIGHT );
}




//-----------------------------------------------------------------------------
// Name: SetMotorGraph()
// Desc: Switches between MAX_PATTERNS types of waveform for the test screen
//-----------------------------------------------------------------------------
VOID CXBoxSample::SetMotorGraph()
{
    int   ax = 0;
    int   ay = 0;

    for( int x = 0; x < NUM_SAMPLES; x++ )
    {
        FLOAT radians   = (x)/(2*D3DX_PI);
        int   sinewave1 = (INT)(50*(sinf(0.5f*radians)+1));
        int   sinewave2 = (INT)(50*(sinf(0.7f*radians)+1));
        int   sinewave3 = (INT)(sinf(radians) * 50);

        switch( m_wLeftPattern )
        {
            case 1:
                m_iLeftMotor[x]  = ((4*x)%101);
                break;
            case 2:
                m_iLeftMotor[x]  = sinewave1;
                break;
            case 3:
                if( sinewave2 < 30 )
                    m_iLeftMotor[x]   = 0;
                else
                    m_iLeftMotor[x]   = 100;
                break;
            case 4:
                m_iLeftMotor[x] = x/5;
                break;
            case 5:
                m_iLeftMotor[x] = ( (rand() % ( 100 - 0 ) ) + 0 );
                break;
            case 6:
                m_iLeftMotor[x]  = 100-((4*x)%101);
                break;
            case 7:
                m_iLeftMotor[x] = 0;
                break;
        }
        switch( m_wRightPattern )
        {
            case 1:
                m_iRightMotor[x] = ((5*x)%101);
                break;
            case 2:
                m_iRightMotor[x] = sinewave1;
                break;
            case 3:
                if( sinewave2 < 30 )
                    m_iRightMotor[x]  = 0;
                else
                    m_iRightMotor[x]  = 80;
                break;
            case 4:
                m_iRightMotor[x] = x/5;
                break;
            case 5:
                m_iRightMotor[x] = ( (rand() % ( 100 - 0 ) ) + 0 );
                break;
            case 6:
                if( x < NUM_SAMPLES/2 )
                    m_iRightMotor[x] = 0;
                else
                    m_iRightMotor[x] = (ay++)/3;
                break;
            case 7:
                m_iRightMotor[x] = 0;
                break;
        }
    }
}




//-----------------------------------------------------------------------------
// Name: RandomNum()
// Desc: 
//-----------------------------------------------------------------------------
INT CXBoxSample::RandomNum( INT low, INT high )
{
    return( (rand() % ( high - low ) ) + low );
}




//-----------------------------------------------------------------------------
// Name: DrawLine()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawLine( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, 
                            DWORD dwColor )
{
    SCREENVERTEX v[2];
    v[0].pos = D3DXVECTOR4( x1, y1, 0.5f, 1.0f );   v[0].color = dwColor;
    v[1].pos = D3DXVECTOR4( x2, y2, 0.5f, 1.0f );   v[1].color = dwColor;
    
    // Render the line
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    m_pd3dDevice->SetVertexShader( D3DFVF_SCREENVERTEX) ;
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, v, sizeof(SCREENVERTEX) );
}




//-----------------------------------------------------------------------------
// Name: DrawRectOutline()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawRectOutline( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, 
                                   DWORD dwColor )
{
    DrawLine( x1, y1, x1, y2, dwColor );
    DrawLine( x1, y1, x2, y1, dwColor );
    DrawLine( x2, y1, x2, y2, dwColor );
    DrawLine( x1, y2, x2, y2, dwColor );
}




//-----------------------------------------------------------------------------
// Name: DrawRect()
// Desc: 
//-----------------------------------------------------------------------------
VOID CXBoxSample::DrawRect( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, 
                            DWORD dwStartColor, DWORD dwEndColor )
{
    SCREENVERTEX v[4];
    v[0].pos = D3DXVECTOR4( x1-0.5f, y1-0.5f, 1.0f, 1.0f );  v[0].color = dwStartColor;
    v[1].pos = D3DXVECTOR4( x2-0.5f, y1-0.5f, 1.0f, 1.0f );  v[1].color = dwStartColor;
    v[2].pos = D3DXVECTOR4( x1-0.5f, y2-0.5f, 1.0f, 1.0f );  v[2].color = dwEndColor;
    v[3].pos = D3DXVECTOR4( x2-0.5f, y2-0.5f, 1.0f, 1.0f );  v[3].color = dwEndColor;
    
    // Render the rectangle
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    m_pd3dDevice->SetVertexShader( D3DFVF_SCREENVERTEX );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(SCREENVERTEX) );
}




