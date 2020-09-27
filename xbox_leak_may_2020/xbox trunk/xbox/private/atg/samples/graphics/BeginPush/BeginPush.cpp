//-----------------------------------------------------------------------------
// File: BeginPush.cpp
//
// Desc: Example code showing how to use the BeginPush() API to write
//       dynamic vertices to the pushbuffer. 
//
//       The CPU time to write the vertices is timed, along with other draw
//       methods (DrawPrimitiveUP() and Begin()) for performance comparison.
//
// Hist: 11.27.01 - New for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBUtil.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include "Resource.h"  // Constants for bundled resources




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Switch draw\nmethods" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display\nhelp" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
};

#define NUM_HELP_CALLOUTS 3




//-----------------------------------------------------------------------------
// Structure and data for Streaks
//-----------------------------------------------------------------------------
#define NUM_STREAKS         13
#define TRAIL_SIZE        100

struct StreakObject
{
    D3DXVECTOR3 vLastPositions[TRAIL_SIZE];

    D3DXVECTOR3 vPos;
    D3DXVECTOR3 vDir;       // Current direction
    D3DXVECTOR3 vDeltaPos;  // Change in position from flock centering
    D3DXVECTOR3 vDeltaDir;  // Change in direction
    WORD        wDeltaCnt;  // Number of streaks that influence this vDeltaDir
    FLOAT       fSpeed;
    FLOAT       fYaw, fPitch, fRoll, fDYaw;
    D3DCOLOR    Color;
    FLOAT       afDist[NUM_STREAKS]; // Array of distances to other streaks
};

StreakObject g_pStreaks[NUM_STREAKS];  // Array of streaks
D3DXVECTOR3  g_vGoal;                  // Goal for each streak to head for
const FLOAT cfInfluenceRadius        = 20.0f;
const FLOAT cfInfluenceRadiusSquared = cfInfluenceRadius * cfInfluenceRadius;
const FLOAT cfCollisionFraction      = 0.8f;
const FLOAT cfInvCollisionFraction   = 1.0f/(1.0f-cfCollisionFraction);
const FLOAT cfParticleSize           = 2.0f;
const FLOAT cfNormalSpeed            = 0.2f;
const FLOAT cfAngleTweak             = 0.02f;
const FLOAT cfPitchToSpeedRatio      = 0.002f;

// Function to update the flock of streaks
VOID UpdateFlock();

// Helper macro
#define rnd()  (((FLOAT)rand() ) / RAND_MAX) 



//-----------------------------------------------------------------------------
// Vertical blank callback to regulate the updating of the flock
//-----------------------------------------------------------------------------
BOOL  g_bFlockNeedsUpdating = FALSE;
BOOL  g_bTimerNeedsUpdating = FALSE;
DWORD g_dwVBlankCount       = 0;

VOID _cdecl VerticalBlankCB( D3DVBLANKDATA* )
{
    // Trigger an update for the flock 60 times per sec.
    g_bFlockNeedsUpdating = TRUE;

    // Trigger an update for the perf-timing code once per sec (60 vblanks = 1 sec)
    if( ++g_dwVBlankCount >= 60 )
    {
        g_dwVBlankCount = 0;
        g_bTimerNeedsUpdating = TRUE;
    }
}




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    CXBPackedResource  m_xprResource;        // Packed resources for the app
    CXBFont            m_Font;               // Font class
    CXBHelp            m_Help;             // Help class
    BOOL               m_bDrawHelp;        // Whether to draw help

    LPDIRECT3DTEXTURE8 m_pParticleTexture;   // Texture for the streak particles

    enum
    {
        USEDRAWPRIMUP,
        USEBEGINPUSH,
        USEBEGIN,
    } m_DrawMethod;                          // Various draw methods 

    LARGE_INTEGER      m_qwElapsedTime;      // For timing the draw method
    int                m_dwRenderCount;
    FLOAT              m_fElapsedTime;

public:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();

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
    // Allow unlimited frame rate
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Don't draw help initially
    m_bDrawHelp = FALSE;

    // Seed the random number generator
    srand((DWORD)m_fTime);

    // Init the streaks
    for( WORD i=0; i<NUM_STREAKS; i++ )
    {
        g_pStreaks[i].vPos   = D3DXVECTOR3(100.0f*(rnd()-rnd()), 10.0f*rnd(), 100.0f*(rnd()-rnd()));
        D3DXVec3Normalize( &g_pStreaks[i].vDir, &D3DXVECTOR3(rnd()-rnd(), rnd()-rnd(), rnd()-rnd()));
        g_pStreaks[i].fYaw   = 0.0f;
        g_pStreaks[i].fPitch = 0.0f;
        g_pStreaks[i].fRoll  = 0.0f;
        g_pStreaks[i].fDYaw  = 0.0f;
        g_pStreaks[i].fSpeed = cfNormalSpeed;

        D3DXVECTOR3 rgb( rnd(), rnd(), rnd() );
        D3DXVec3Normalize( &rgb, &rgb );
        g_pStreaks[i].Color = ((DWORD)(255*rgb.x)<<16)|((DWORD)(255*rgb.y)<<8)|((DWORD)(255*rgb.z)<<0);
    }

    // Init the streaks' goal
    g_vGoal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

    // Set a draw method
    m_DrawMethod = USEBEGINPUSH;

    // Initialize the timer variables
    m_qwElapsedTime.QuadPart = 0;
    m_dwRenderCount          = 0;
    m_fElapsedTime           = 0;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: This creates all device-dependant display objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create some textures
    m_pParticleTexture = m_xprResource.GetTexture( resource_Particle_OFFSET );

    // Set the transform matrices
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, 0.75f, 4.0f/3.0f, 1.0f, 1000.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set a vertical blank callback to regulate the updating of the flock
    m_pd3dDevice->SetVerticalBlankCallback( VerticalBlankCB );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    // Switch the draw method if the user presses the A button
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] ) 
    {
        switch( m_DrawMethod )
        {
            case USEDRAWPRIMUP: m_DrawMethod = USEBEGINPUSH; break;
            case USEBEGINPUSH:  m_DrawMethod = USEBEGIN; break;
            case USEBEGIN:      m_DrawMethod = USEDRAWPRIMUP; break;
        }

        // Reset the timer variables
        m_qwElapsedTime.QuadPart = 0;
        m_dwRenderCount          = 0;
    }
    
    // Update the flock at 60Hz, regardless of framerate. The trigger variable
    // is set during a VBlank callback to ensure 60Hz.
    if( g_bFlockNeedsUpdating )
    {
        // Update the flock
        if( FALSE == m_bPaused )
            UpdateFlock();

        // Reset the trigger variable
        g_bFlockNeedsUpdating = FALSE;
    }

    // Set the view and projection matrices. Note: these are static since
    // the view changes each frame
    D3DXMATRIX matWorld;
    static D3DXVECTOR3 vEyePt( 0.0f, 30.0f, 100.0f );
    static D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 50.0f );
    static D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

    // Compute the lookat point (look at the center of the flock)
    vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    for( WORD i=0; i<NUM_STREAKS; i++)
        vLookatPt += g_pStreaks[i].vPos;
    vLookatPt /= NUM_STREAKS;

    // Move eyepoint in ellipse about the lookat point
    vEyePt.x = vLookatPt.x + ( 30.0f*sinf(m_fAppTime*0.223f) );
    vEyePt.y = vLookatPt.y + ( 21.0f + 20.0f*sinf(m_fAppTime*0.33f) );
    vEyePt.z = vLookatPt.z + ( 30.0f*cosf(m_fAppTime*0.31f) );

    // Move goal in an ellipse
    g_vGoal.x = 105.0f * sinf(m_fAppTime*0.1f);
    g_vGoal.y = 10.0f;
    g_vGoal.z = 105.0f * cosf(m_fAppTime*0.1f);

    // Set the view matrices
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x00000000, 1.0f, 0L );

    // Set the render states for using point sprites. Note that pointsprites
    // use texture stage 3 only.
    m_pd3dDevice->SetTexture( 3, m_pParticleTexture );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE,         FtoDW(cfParticleSize) );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,      FtoDW(0.0f) );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,      FtoDW(0.0f) );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,      FtoDW(1.0f) );

    // Turn on alphablending and disable z-writes
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_ONE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,     FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         FALSE );

    // Set the vertex shader
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZ|D3DFVF_DIFFUSE );

    // Capture the start time, so we can calculate the CPU-time it takes to
    // process the dynamic vertices by the various rendering methods.
    LARGE_INTEGER qwStartTime;
    QueryPerformanceCounter( &qwStartTime );

    // Draw the streaks
    switch( m_DrawMethod )
    {
        case USEDRAWPRIMUP: // Draw the streaks with the DrawPrimitiveUP() API
        {
            struct STREAKVERTEX
            {
                D3DXVECTOR3 vPos;
                D3DCOLOR    color;
            } vertices[TRAIL_SIZE];

            for( WORD i=0; i<NUM_STREAKS; i++)
            {
                StreakObject* pStreak = &g_pStreaks[i];

                for( WORD j=0; j<TRAIL_SIZE; j++ )
                {
                    // Fade the alpha for the streak trail particles
                    DWORD dwAlpha = j*255/(TRAIL_SIZE-1);

                    vertices[j].vPos  = pStreak->vLastPositions[j];
                    vertices[j].color = (dwAlpha<<24) | pStreak->Color;
                }

                // Draw the particles
                m_pd3dDevice->DrawPrimitiveUP( D3DPT_POINTLIST, TRAIL_SIZE, vertices, sizeof(STREAKVERTEX) );
            }
        }
        break;

        case USEBEGINPUSH: // Draw the streaks with the BeginPush() API
        {
            DWORD dwVertexSize       = sizeof(D3DXVECTOR3) + sizeof(D3DCOLOR);
            DWORD dwNumVertices      = TRAIL_SIZE;
            DWORD dwPushSizeInBytes  = dwVertexSize * dwNumVertices;
            DWORD dwPushSizeInDWORDs = dwPushSizeInBytes / sizeof(DWORD);

            for( WORD i=0; i<NUM_STREAKS; i++)
            {
                StreakObject* pStreak = &g_pStreaks[i];

                // Gain direct access to the pushbuffer. Note the "+5", which is
                // to reserve overhead for the encoding parameters.
                DWORD* pPush;
                m_pd3dDevice->BeginPush( dwPushSizeInDWORDs + 5, &pPush );

                // Push the macro that start things off
                *pPush++ = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
                
                // Specify the primitive type of the vertices that follow
                *pPush++ = D3DPT_POINTLIST;

                // Specify that an array of vertices comes next. Note that a max
                // of 2047 dwords can be specified in a INLINE_ARRAY section.
                // For handling more vertex data than that, simply split the data
                // into multiple INLINE_ARRAY sections.
                *pPush++ = D3DPUSH_ENCODE( D3DPUSH_NOINCREMENT_FLAG|D3DPUSH_INLINE_ARRAY, dwPushSizeInDWORDs );

                // Write the dynamic vertices
                for( WORD j=0; j<TRAIL_SIZE; j++ )
                {
                    // Fade the alpha for the streak trail particles
                    DWORD dwAlpha = j*255/(TRAIL_SIZE-1);

                    // Write the position
                    *((D3DXVECTOR3*)pPush) = pStreak->vLastPositions[j];
                    pPush += 3;

                    // Write the diffuse color
                    *((D3DCOLOR*)pPush) = (dwAlpha<<24) | pStreak->Color;
                    pPush += 1;
                }

                // Push the macros that finish things off
                *pPush++ = D3DPUSH_ENCODE( D3DPUSH_SET_BEGIN_END, 1 );
                *pPush++ = 0;

                m_pd3dDevice->EndPush( pPush );
            }
        }
        break;

        case USEBEGIN: // Draw the streaks with the Begin() API
        {
            for( WORD i=0; i<NUM_STREAKS; i++)
            {
                StreakObject* pStreak = &g_pStreaks[i];

                // Begin sending vertex component data
                m_pd3dDevice->Begin( D3DPT_POINTLIST );

                // Write the dynamic vertices
                for( WORD j=0; j<TRAIL_SIZE; j++ )
                {
                    // Fade the alpha for the streak trail particles
                    DWORD dwAlpha = j*255/(TRAIL_SIZE-1);

                    // Write the diffuse color
                    m_pd3dDevice->SetVertexDataColor( D3DVSDE_DIFFUSE, (dwAlpha<<24) | pStreak->Color );

                    // Write the position
                    D3DXVECTOR3* vPos = &pStreak->vLastPositions[j];
                    m_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, vPos->x, vPos->y, vPos->z, 1.0f );
                }

                // End sending vertex component data
                m_pd3dDevice->End();
            }
        }
        break;
    }

    // Every few seconds, we will poll the timer variables and report the results
    if( g_bTimerNeedsUpdating )
    {
        // Get the frequency of the timer
        LARGE_INTEGER qwTicksPerSec;
        QueryPerformanceFrequency( &qwTicksPerSec );
        FLOAT fSecsPerTick = 1.0f / (FLOAT)qwTicksPerSec.QuadPart;

        m_fElapsedTime = fSecsPerTick * ((FLOAT)(m_qwElapsedTime.QuadPart)) / m_dwRenderCount;
    
        // Reset the timer variables
        m_qwElapsedTime.QuadPart = 0L;
        m_dwRenderCount          = 0;
        g_bTimerNeedsUpdating    = FALSE;
    }

    // Capture the stop time, and calculate the elpased CPU-time it took to
    // process the dynamic vertices by the various rendering methods above.
    LARGE_INTEGER qwStopTime;
    QueryPerformanceCounter( &qwStopTime );
    m_qwElapsedTime.QuadPart += qwStopTime.QuadPart - qwStartTime.QuadPart;
    m_dwRenderCount++;

    // Reset render states
    m_pd3dDevice->SetTexture( 3, NULL );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,      TRUE );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.DrawText(  64, 50, 0xffffff00, L"BeginPush" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        // Show the perf-timing results
        WCHAR str[80];
        m_Font.DrawText( 64, 80, 0xffffff00, L"Using draw method:" );
        switch( m_DrawMethod )
        {
            case USEDRAWPRIMUP: m_Font.DrawText( 270, 80, 0xffffffff, L"DrawPrimitiveUP()" ); break;
            case USEBEGINPUSH:  m_Font.DrawText( 270, 80, 0xffffffff, L"BeginPush()" ); break;
            case USEBEGIN:      m_Font.DrawText( 270, 80, 0xffffffff, L"Begin()" ); break;
        }
        swprintf( str, L"%.3f ms", 1000*m_fElapsedTime );
        m_Font.DrawText(  64, 105, 0xffffff00, L"Draw method took:" );
        m_Font.DrawText( 270, 105, 0xffffffff, str );
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateFlock()
// Desc: Update posiiton of each streak in flock
//-----------------------------------------------------------------------------
VOID UpdateFlock()
{
    FLOAT fDist;

    // First update the dist array 0.0..1.0 with 0.0 being furthest away
    for( WORD i=0; i<NUM_STREAKS; i++ )
    {
        for( WORD j=i+1; j<NUM_STREAKS; j++ )
        {
            fDist = D3DXVec3LengthSq( &(g_pStreaks[i].vPos-g_pStreaks[j].vPos) );
            fDist = cfInfluenceRadiusSquared - fDist;
            if( fDist < 0.0f )
                fDist = 0.0f;
            else
                fDist /= cfInfluenceRadiusSquared;

            g_pStreaks[i].afDist[j] = g_pStreaks[j].afDist[i] = fDist;
        }
        g_pStreaks[i].afDist[i] = 0.0f;
        g_pStreaks[i].vDeltaDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
        g_pStreaks[i].vDeltaPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
        g_pStreaks[i].wDeltaCnt = 0;
    }

    for( i=0; i<NUM_STREAKS; i++ )
    {
        for( WORD j=i+1; j<NUM_STREAKS; j++ )
        {
            // If i is near j have them influence each other
            if( g_pStreaks[i].afDist[j] > 0.0f )
            {
                D3DXVECTOR3 vDiff;
                D3DXVECTOR3 vDelta;
                FLOAT       fCollWeight = 0.0f;     // Collision weighting

                D3DXVec3Normalize( &vDiff, &(g_pStreaks[i].vPos-g_pStreaks[j].vPos) );

                // Only do collision testing against the nearest ones
                if( g_pStreaks[i].afDist[j] - cfCollisionFraction > 0.0f )
                    fCollWeight = (g_pStreaks[i].afDist[j] - cfCollisionFraction) *
                                  cfInvCollisionFraction;

                // Add in a little flock centering
                if( g_pStreaks[i].afDist[j] - (1.0f-cfCollisionFraction) > 0.0f )
                    fCollWeight -= g_pStreaks[i].afDist[j] * (1.0f-fCollWeight);

                vDelta = fCollWeight * vDiff;

                // Add in the collision avoidance
                g_pStreaks[i].vDeltaPos += vDelta;
                g_pStreaks[j].vDeltaPos -= vDelta;

                // Add in the velocity influences
                g_pStreaks[i].vDeltaDir += g_pStreaks[j].vDir * g_pStreaks[i].afDist[j];
                g_pStreaks[j].vDeltaDir += g_pStreaks[i].vDir * g_pStreaks[i].afDist[j];
                g_pStreaks[i].wDeltaCnt++;
                g_pStreaks[j].wDeltaCnt++;
            }
        }
    }

    // Update the streaks
    for( i=0; i<NUM_STREAKS; i++ )
    {
        if( g_pStreaks[i].wDeltaCnt )
        {
            g_pStreaks[i].vDeltaDir /= (FLOAT)g_pStreaks[i].wDeltaCnt;
            g_pStreaks[i].vDeltaDir -= g_pStreaks[i].vDir;
            g_pStreaks[i].vDeltaDir *= 1.5f;
        }
        D3DXVECTOR3 vDelta = g_pStreaks[i].vDeltaDir + g_pStreaks[i].vDeltaPos;
        D3DXVECTOR3 vOffset;

        // Add in the influence of the global goal
        D3DXVECTOR3 vGoal;
        D3DXVec3Normalize( &vGoal, &(g_vGoal-g_pStreaks[i].vPos) );
        vDelta += 0.5f * vGoal;

        // First deal with pitch changes
        if( vDelta.y > 0.01f )
        {   // We're too low
            g_pStreaks[i].fPitch += cfAngleTweak;
            if( g_pStreaks[i].fPitch > 0.8f )
                g_pStreaks[i].fPitch = 0.8f;
        }
        else if( vDelta.y < -0.01f )
        {   // We're too high
            g_pStreaks[i].fPitch -= cfAngleTweak;
            if( g_pStreaks[i].fPitch < -0.8f )
                g_pStreaks[i].fPitch = -0.8f;
        } 
        else
        {
            // Add damping
            g_pStreaks[i].fPitch *= 0.98f;
        }

        // Speed up or slow down depending on angle of attack
        g_pStreaks[i].fSpeed -= g_pStreaks[i].fPitch * cfPitchToSpeedRatio;
        // Damp back to normal
        g_pStreaks[i].fSpeed = (g_pStreaks[i].fSpeed-cfNormalSpeed)*0.99f + cfNormalSpeed;

        if( g_pStreaks[i].fSpeed < cfNormalSpeed/2 )
            g_pStreaks[i].fSpeed = cfNormalSpeed/2;
        if( g_pStreaks[i].fSpeed > cfNormalSpeed*5 )
            g_pStreaks[i].fSpeed = cfNormalSpeed*5;

        // Now figure out yaw changes
        vOffset    = vDelta;
        vOffset.y  = 0.0f;
        vDelta     = g_pStreaks[i].vDir;
        D3DXVec3Normalize( &vOffset, &vOffset );
        FLOAT fDot = D3DXVec3Dot( &vOffset, &vDelta );
        
        // Speed up slightly if not turning much
        if( fDot > 0.7f )
        {
            fDot -= 0.7f;
            g_pStreaks[i].fSpeed += fDot * 0.005f;
        }
        D3DXVec3Cross( &vOffset, &vOffset, &vDelta );
        fDot = (1.0f-fDot)/2.0f * 0.07f;
        if( vOffset.y > 0.05f )
            g_pStreaks[i].fDYaw = (g_pStreaks[i].fDYaw*19.0f + fDot) * 0.05f;
        else if( vOffset.y < -0.05f )
            g_pStreaks[i].fDYaw = (g_pStreaks[i].fDYaw*19.0f - fDot) * 0.05f;
        else
            g_pStreaks[i].fDYaw *= 0.98f; // damp it

        g_pStreaks[i].fYaw += g_pStreaks[i].fDYaw;
        g_pStreaks[i].fRoll = -g_pStreaks[i].fDYaw * 20.0f;
    }

    // Update the streaks' direction and position
    for( WORD i=0; i<NUM_STREAKS; i++)
    {
        g_pStreaks[i].vDir.x =  cosf( -g_pStreaks[i].fPitch ) * sinf( -g_pStreaks[i].fYaw );
        g_pStreaks[i].vDir.y = -sinf( -g_pStreaks[i].fPitch );
        g_pStreaks[i].vDir.z =  cosf( -g_pStreaks[i].fPitch ) * cosf( -g_pStreaks[i].fYaw );

        g_pStreaks[i].vPos += g_pStreaks[i].vDir * g_pStreaks[i].fSpeed;
    }

    // Update the streaks tails
    for( WORD i=0; i<NUM_STREAKS; i++)
    {
        for( DWORD j=0; j<TRAIL_SIZE-1; j++ )
            g_pStreaks[i].vLastPositions[j] = g_pStreaks[i].vLastPositions[j+1];

        g_pStreaks[i].vLastPositions[j] = g_pStreaks[i].vPos;
    }
}




