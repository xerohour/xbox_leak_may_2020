//-----------------------------------------------------------------------------
// File: Gamepad.cpp
//
// Desc: Tool to experiment with all things related to an Xbox gamepad
//       controller
//
// Hist: 04.06.01 - Created
//       10.03.01 - Ensure use of only one gamepad
//       10.03.01 - Added count to quantization page
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBMesh.h>
#include <XBUtil.h>
#include <XBResource.h>
#include <xgraphics.h>
#include "Resource.h"


// Subsets for within the gamepad mesh. This are specific to the gamepad mesh.
// For instance, subset 7 represents an internal XBMESH_SUBSET structure for
// rendering the geometry for the gamepad's x button.
#define CONTROL_LEFTTHUMBSTICK  0
#define CONTROL_RIGHTTHUMBSTICK 1
#define CONTROL_BODY            2
#define CONTROL_BACKBUTTON      3
#define CONTROL_STARTBUTTON     4
#define CONTROL_ABUTTON         5
#define CONTROL_BBUTTON         6
#define CONTROL_XBUTTON         7
#define CONTROL_YBUTTON         8
#define CONTROL_WHITEBUTTON     9
#define CONTROL_BLACKBUTTON    10
#define CONTROL_CORDSTUB       11
#define CONTROL_GASKETS        12
#define CONTROL_MEMCARDSLOT    13
#define CONTROL_LEFTTRIGGER    14
#define CONTROL_RIGHTTRIGGER   15
#define CONTROL_DPAD           16
#define CONTROL_JEWEL          17
#define NUM_CONTROLS           18


// Offsets for building matrices which are used to animate the gamepad controls.
D3DXVECTOR3 g_vLeftTriggerAxis    = D3DXVECTOR3( 1.0f, -0.13f, 0.0f );
D3DXVECTOR3 g_vLeftTriggerOffset  = D3DXVECTOR3( 0.0f, 12.0f, 38.0f );
D3DXVECTOR3 g_vRightTriggerAxis   = D3DXVECTOR3( 1.0f, 0.13f, 0.0f );
D3DXVECTOR3 g_vRightTriggerOffset = D3DXVECTOR3( 0.0f, 12.0f, 38.0f );
D3DXVECTOR3 g_vDPadOffset         = D3DXVECTOR3( -36.70f, -26.27f, 12.0f );
D3DXVECTOR3 g_vLeftStickOffset    = D3DXVECTOR3( -48.54f,   8.72f, 18.0f );
D3DXVECTOR3 g_vRightStickOffset   = D3DXVECTOR3(  36.70f, -26.27f, 18.0f );


// Structures for animating, highlighting, and textureing the gamepad controls.
BOOL               g_ControlActive[NUM_CONTROLS];
D3DXMATRIX         g_ControlMatrix[NUM_CONTROLS];
LPDIRECT3DTEXTURE8 g_ControlTexture[NUM_CONTROLS];


// Global world matrix, so the app can orient the gamepad
D3DXMATRIX g_matWorld( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );




//-----------------------------------------------------------------------------
// Name: class CGamepadMesh
// Desc: The gamepad mesh. This is overrided from the base class so that we can
//       provide a custom RenderCallback() function.
//-----------------------------------------------------------------------------
class CGamepadMesh : public CXBMesh
{
public:
    BOOL    RenderCallback( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwSubset,
                            XBMESH_SUBSET* pSubset, DWORD dwFlags );
};




//-----------------------------------------------------------------------------
// Name: RenderCallback()
// Desc: Overrided from the base class so that we can animate and highlight
//       individual mesh subsets before rendering them.
//-----------------------------------------------------------------------------
BOOL CGamepadMesh::RenderCallback( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwSubset,
                                   XBMESH_SUBSET* pSubset, DWORD dwFlags )
{
    // Set matrix
    D3DXMATRIX mat = g_matWorld;
    D3DXMatrixMultiply( &mat, &g_ControlMatrix[dwSubset], &mat );
    pd3dDevice->SetTransform( D3DTS_WORLD, &mat );

    // Set material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f, 1.0f );
    pd3dDevice->SetMaterial( &mtrl );

    // Set texture
    pd3dDevice->SetTexture( 0, g_ControlTexture[dwSubset] );

    return TRUE;
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    // Valid app states
    enum APPSTATE 
    { 
        APPSTATE_CONTROLTEST=0, 
        APPSTATE_VIBRATIONTEST,
        APPSTATE_DEADZONECALIBRATION,
        APPSTATE_BUTTONQUANTIZATION,
        APPSTATE_MEMORYUNITDETECTION,
        APPSTATE_MAX,
    };

    // Valid gamepad types
    enum GAMEPADTYPE
    { 
        GAMEPADTYPE_NONE=0, 
        GAMEPADTYPE_US, 
        GAMEPADTYPE_JAPAN,
    };

    // General application members
    APPSTATE           m_AppState;         // State of the app
    CXBPackedResource  m_xprResource;      // Packed resources for the app
    CXBFont            m_Font16;           // 16-point font class
    CXBFont            m_Font12;           // 12-point font class

    // Active gamepad
    DWORD              m_dwNumInsertedGamepads;
    XBGAMEPAD*         m_pGamepad;

    // Geometry
    CGamepadMesh       m_GamepadMesh;      // Geometry for the US gamepad
    CGamepadMesh       m_GamepadSMesh;     // Geometry for the Japan gamepad
    CGamepadMesh*      m_pGamepadMesh;     // Geometry for the active gamepad
    GAMEPADTYPE        m_GamepadType;      // Type of gamepad used

    // Options
    DWORD              m_dwTextureSelection;
    BOOL               m_bAlphaEnabled;

    // Vibration motor values
    FLOAT              m_fLeftMotorSpeed;
    FLOAT              m_fRightMotorSpeed;

    // Deadzone calibration page
    FLOAT              m_fDeadZone;

    // Control quantization page
    BYTE*              m_pQuantizedThumbStickValues;
    BYTE*              m_pQuantizedButtonValues;

    // Internal members
    VOID    ShowTexture( LPDIRECT3DTEXTURE8 pTexture );
    HRESULT DrawBox( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2,
                     DWORD dwFillColor, DWORD dwOutlineColor );
    HRESULT RenderInsertRemoveControllerPage();
    HRESULT RenderControlTestPage();
    HRESULT RenderVibrationTestPage();
    HRESULT RenderDeadZoneCalibrationPage();
    HRESULT RenderButtonQuantizationPage();

protected:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();

public:
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
    // Override base class variable to enable fullscreen-antialiasing
    m_d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR |
                              D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8;

    m_AppState              = APPSTATE_CONTROLTEST;
    m_dwNumInsertedGamepads = 0L;
    m_pGamepad              = &m_DefaultGamepad;
    m_GamepadType           = GAMEPADTYPE_NONE;
    m_pGamepadMesh          = NULL;
    m_dwTextureSelection    = 0L;
    m_bAlphaEnabled         = FALSE;
    m_fDeadZone             = 0.24f;  // Set default deadzone to 24%
    m_fLeftMotorSpeed       = 0.0f;
    m_fRightMotorSpeed      = 0.0f;

    // Quantized control values
    m_pQuantizedThumbStickValues = new BYTE[256];
    ZeroMemory( m_pQuantizedThumbStickValues, 256 );

    m_pQuantizedButtonValues = new BYTE[256];
    ZeroMemory( m_pQuantizedButtonValues, 256 );
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: This creates all device-dependent display objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the fonts
    if( FAILED( m_Font16.Create( m_pd3dDevice, "Font16.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;
    if( FAILED( m_Font12.Create( m_pd3dDevice, "Font12.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the gamepad objects
    if( FAILED( m_GamepadMesh.Create( m_pd3dDevice, "Models\\Gamepad.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( m_GamepadSMesh.Create( m_pd3dDevice, "Models\\GamepadS.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Initialize the control highlight states, matrices, and textures
    for( DWORD i=0; i< NUM_CONTROLS; i++ )
    {
        g_ControlActive[i] = FALSE;
        D3DXMatrixIdentity( &g_ControlMatrix[i] );
        g_ControlTexture[i] = NULL;
    }

    g_ControlTexture[CONTROL_BODY]            = m_xprResource.GetTexture( resource_MatteBlack_OFFSET );
    g_ControlTexture[CONTROL_GASKETS]         = m_xprResource.GetTexture( resource_MatteBlack_OFFSET );
    g_ControlTexture[CONTROL_LEFTTHUMBSTICK]  = m_xprResource.GetTexture( resource_MatteBlack_OFFSET );
    g_ControlTexture[CONTROL_RIGHTTHUMBSTICK] = m_xprResource.GetTexture( resource_MatteBlack_OFFSET );
    g_ControlTexture[CONTROL_JEWEL]           = m_xprResource.GetTexture( resource_GreenBezel_OFFSET );
    g_ControlTexture[CONTROL_CORDSTUB]        = m_xprResource.GetTexture( resource_Cord_OFFSET );

    g_ControlTexture[CONTROL_MEMCARDSLOT]  = m_xprResource.GetTexture( resource_ShinyBlack_OFFSET );
    g_ControlTexture[CONTROL_BACKBUTTON]   = m_xprResource.GetTexture( resource_ShinyBlack_OFFSET );
    g_ControlTexture[CONTROL_STARTBUTTON]  = m_xprResource.GetTexture( resource_ShinyBlack_OFFSET );
    g_ControlTexture[CONTROL_LEFTTRIGGER]  = m_xprResource.GetTexture( resource_ShinyBlack_OFFSET );
    g_ControlTexture[CONTROL_RIGHTTRIGGER] = m_xprResource.GetTexture( resource_ShinyBlack_OFFSET );
    g_ControlTexture[CONTROL_DPAD]         = m_xprResource.GetTexture( resource_ShinyBlack_OFFSET );
    g_ControlTexture[CONTROL_ABUTTON]      = m_xprResource.GetTexture( resource_GreenGlass_OFFSET );
    g_ControlTexture[CONTROL_BBUTTON]      = m_xprResource.GetTexture( resource_RedGlass_OFFSET );
    g_ControlTexture[CONTROL_XBUTTON]      = m_xprResource.GetTexture( resource_BlueGlass_OFFSET );
    g_ControlTexture[CONTROL_YBUTTON]      = m_xprResource.GetTexture( resource_YellowGlass_OFFSET );
    g_ControlTexture[CONTROL_WHITEBUTTON]  = m_xprResource.GetTexture( resource_WhiteGlass_OFFSET );
    g_ControlTexture[CONTROL_BLACKBUTTON]  = m_xprResource.GetTexture( resource_ShinyBlack_OFFSET );

    // Misc render states
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0xffffffff );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,       D3DCULL_NONE );

    // Set up world matrix
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set up view matrix
    D3DXMATRIX  matView;
    D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f,-250.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,   0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,   0.0f );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set up proj matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 1000.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Setup a base material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Detect the number and type of gamepads that are inserted. By default,
    // use the US gamepad (aka "Xbox Controller"), unless any gamepad slot has
    // a Japan gamepad (aka "Xbox Controller S") plugged into it. For this
    // sample, we do this every frame just in case someone puts in a different
    // controller.
    m_dwNumInsertedGamepads = 0;
    m_GamepadType           = GAMEPADTYPE_NONE;
    for( DWORD i=0; i<4; i++ )
    {
        if( m_Gamepad[i].hDevice )
        {
            if( m_Gamepad[i].caps.SubType == XINPUT_DEVSUBTYPE_GC_GAMEPAD_ALT )
            {
                // Use the Japan controller
                m_pGamepad    = &m_Gamepad[i];
                m_GamepadType = GAMEPADTYPE_JAPAN;
                m_dwNumInsertedGamepads++;
            }
            else
            {
                // For all other controller types, use the US controller
                m_pGamepad    = &m_Gamepad[i];
                m_GamepadType = GAMEPADTYPE_US;
                m_dwNumInsertedGamepads++;
            }
        }
    }
    if( GAMEPADTYPE_US == m_GamepadType )
    {
        m_pGamepadMesh        = &m_GamepadMesh;
        g_vLeftTriggerAxis    = D3DXVECTOR3(  1.00f, -0.13f,  0.0f );
        g_vLeftTriggerOffset  = D3DXVECTOR3(  0.00f, 12.00f, 38.0f );
        g_vRightTriggerAxis   = D3DXVECTOR3(  1.00f,  0.13f,  0.0f );
        g_vRightTriggerOffset = D3DXVECTOR3(  0.00f, 12.00f, 38.0f );
        g_vDPadOffset         = D3DXVECTOR3(-36.70f,-26.27f, 12.0f );
        g_vLeftStickOffset    = D3DXVECTOR3(-48.54f,  8.72f, 18.0f );
        g_vRightStickOffset   = D3DXVECTOR3( 36.70f,-26.27f, 18.0f );
    }
    else if( GAMEPADTYPE_JAPAN == m_GamepadType )
    {
        m_pGamepadMesh        = &m_GamepadSMesh;
        g_vLeftTriggerAxis    = D3DXVECTOR3(  1.00f, -0.13f,  0.00f );
        g_vRightTriggerAxis   = D3DXVECTOR3(  1.00f,  0.13f,  0.00f );
        g_vLeftTriggerOffset  = D3DXVECTOR3(-41.40f, 60.86f,  8.43f );
        g_vRightTriggerOffset = D3DXVECTOR3( 41.40f, 60.86f,  8.43f );
        g_vDPadOffset         = D3DXVECTOR3(-22.01f, 21.02f,-11.31f );
        g_vLeftStickOffset    = D3DXVECTOR3(-42.01f, 45.60f, -7.70f );
        g_vRightStickOffset   = D3DXVECTOR3( 20.45f, 20.96f, -7.70f );
    }
    else 
        m_pGamepadMesh = NULL;

    // Move to next app state when user presses BACK and START together
    BOOL bStartAndBackButtonsPushed = FALSE;

    if( ( m_pGamepad->wButtons & XINPUT_GAMEPAD_START ) &&
        ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_BACK ) )
        bStartAndBackButtonsPushed = TRUE;

    if( ( m_pGamepad->wButtons & XINPUT_GAMEPAD_BACK ) &&
        ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_START ) )
        bStartAndBackButtonsPushed = TRUE;

    if( bStartAndBackButtonsPushed )
    {
        switch( m_AppState )
        {
            case APPSTATE_CONTROLTEST:         m_AppState = APPSTATE_VIBRATIONTEST; break;
            case APPSTATE_VIBRATIONTEST:       m_AppState = APPSTATE_DEADZONECALIBRATION; break;
            case APPSTATE_DEADZONECALIBRATION: m_AppState = APPSTATE_BUTTONQUANTIZATION; break;
            case APPSTATE_BUTTONQUANTIZATION:  m_AppState = APPSTATE_CONTROLTEST; break;
        }

        m_fLeftMotorSpeed    = 0.0f;
        m_fRightMotorSpeed   = 0.0f;
    }

    // Handle the control test page
    if( m_AppState == APPSTATE_CONTROLTEST )
    {
        // Select options
        if( m_pGamepad->wButtons & XINPUT_GAMEPAD_START )
        {
            if( m_pGamepad->bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
            {
                static dwTexture = 0;
                dwTexture = (dwTexture+1)%resource_NUM_RESOURCES;
                g_ControlTexture[CONTROL_BODY] = m_xprResource.GetTexture( 20L*dwTexture );
            }

            if( m_pGamepad->bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
            {
                static dwTexture = 0;
                dwTexture = (dwTexture+1)%resource_NUM_RESOURCES;
                g_ControlTexture[ CONTROL_LEFTTHUMBSTICK ]  = m_xprResource.GetTexture( 20L*dwTexture );
                g_ControlTexture[ CONTROL_RIGHTTHUMBSTICK ] = m_xprResource.GetTexture( 20L*dwTexture );
            }

            if( m_pGamepad->bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
                m_bAlphaEnabled = !m_bAlphaEnabled;
        }

        // Perform object rotation
        D3DXMATRIX matRotate;
        FLOAT fXRotate = m_pGamepad->fX1*m_fElapsedTime*D3DX_PI*0.5f;
        FLOAT fYRotate = m_pGamepad->fY1*m_fElapsedTime*D3DX_PI*0.5f;
        D3DXMatrixRotationYawPitchRoll( &matRotate, -fXRotate, -fYRotate, 0.0f );
        D3DXMatrixMultiply( &g_matWorld, &g_matWorld, &matRotate );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matWorld );

        // Record which controls are active
        g_ControlActive[CONTROL_LEFTTHUMBSTICK]  = ( m_pGamepad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB ||
                                                     m_pGamepad->fX1 || m_pGamepad->fY1 ) ? TRUE : FALSE;
        g_ControlActive[CONTROL_RIGHTTHUMBSTICK] = ( m_pGamepad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ||
                                                     m_pGamepad->fX2 || m_pGamepad->fY2 ) ? TRUE : FALSE;
        g_ControlActive[CONTROL_BACKBUTTON]  = ( m_pGamepad->wButtons & XINPUT_GAMEPAD_BACK ) ? TRUE : FALSE;
        g_ControlActive[CONTROL_STARTBUTTON] = ( m_pGamepad->wButtons & XINPUT_GAMEPAD_START ) ? TRUE : FALSE;
        g_ControlActive[CONTROL_DPAD]        = ( m_pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP || 
                                                 m_pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN || 
                                                 m_pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT || 
                                                 m_pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) ? TRUE : FALSE;
        g_ControlActive[CONTROL_ABUTTON]      = m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_A];
        g_ControlActive[CONTROL_BBUTTON]      = m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_B];
        g_ControlActive[CONTROL_XBUTTON]      = m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_X];
        g_ControlActive[CONTROL_YBUTTON]      = m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_Y];
        g_ControlActive[CONTROL_BLACKBUTTON]  = m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_BLACK];
        g_ControlActive[CONTROL_WHITEBUTTON]  = m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_WHITE];
        g_ControlActive[CONTROL_LEFTTRIGGER]  = m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER];
        g_ControlActive[CONTROL_RIGHTTRIGGER] = m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER];


        // Animate buttons
        {
            D3DXMatrixTranslation( &g_ControlMatrix[CONTROL_LEFTTHUMBSTICK],  0.0f, 0.0f, m_pGamepad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB ? 1.0f : 0.0f );
            D3DXMatrixTranslation( &g_ControlMatrix[CONTROL_RIGHTTHUMBSTICK], 0.0f, 0.0f, m_pGamepad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ? 1.0f : 0.0f );
            D3DXMatrixTranslation( &g_ControlMatrix[CONTROL_BACKBUTTON],     0.0f, 0.0f, m_pGamepad->wButtons & XINPUT_GAMEPAD_BACK  ? 1.5f : 0.0f );
            D3DXMatrixTranslation( &g_ControlMatrix[CONTROL_STARTBUTTON],    0.0f, 0.0f, m_pGamepad->wButtons & XINPUT_GAMEPAD_START ? 1.5f : 0.0f );
            D3DXMatrixTranslation( &g_ControlMatrix[CONTROL_ABUTTON],     0.0f, 0.0f, 2.0f * m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_A] / 255.0f );
            D3DXMatrixTranslation( &g_ControlMatrix[CONTROL_BBUTTON],     0.0f, 0.0f, 2.0f * m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_B] / 255.0f );
            D3DXMatrixTranslation( &g_ControlMatrix[CONTROL_XBUTTON],     0.0f, 0.0f, 2.0f * m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_X] / 255.0f );
            D3DXMatrixTranslation( &g_ControlMatrix[CONTROL_YBUTTON],     0.0f, 0.0f, 2.0f * m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_Y] / 255.0f );
            D3DXMatrixTranslation( &g_ControlMatrix[CONTROL_BLACKBUTTON], 0.0f, 0.0f, 2.0f * m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_BLACK] / 255.0f );
            D3DXMatrixTranslation( &g_ControlMatrix[CONTROL_WHITEBUTTON], 0.0f, 0.0f, 2.0f * m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_WHITE] / 255.0f );
        }

        // Animate left trigger
        { 
            D3DXMATRIX matTrans1, matRotate, matTrans2, matAll;
            D3DXMatrixTranslation( &matTrans1, -g_vLeftTriggerOffset.x, -g_vLeftTriggerOffset.y, -g_vLeftTriggerOffset.z );
            D3DXMatrixRotationAxis( &matRotate, &g_vLeftTriggerAxis, (D3DX_PI/6) * m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] / 255.0f );
            D3DXMatrixTranslation( &matTrans2, g_vLeftTriggerOffset.x, g_vLeftTriggerOffset.y, g_vLeftTriggerOffset.z );
            D3DXMatrixMultiply( &matAll, &matTrans1, &matRotate );
            D3DXMatrixMultiply( &matAll, &matAll, &matTrans2 );
            g_ControlMatrix[CONTROL_LEFTTRIGGER] = matAll;
        }

        // Animate right trigger
        { 
            D3DXMATRIX matTrans1, matRotate, matTrans2, matAll;
            D3DXMatrixTranslation( &matTrans1, -g_vRightTriggerOffset.x, -g_vRightTriggerOffset.y, -g_vRightTriggerOffset.z );
            D3DXMatrixRotationAxis( &matRotate, &g_vRightTriggerAxis, (D3DX_PI/6) * m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] / 255.0f );
            D3DXMatrixTranslation( &matTrans2, g_vRightTriggerOffset.x, g_vRightTriggerOffset.y, g_vRightTriggerOffset.z );
            D3DXMatrixMultiply( &matAll, &matTrans1, &matRotate );
            D3DXMatrixMultiply( &matAll, &matAll, &matTrans2 );
            g_ControlMatrix[CONTROL_RIGHTTRIGGER] = matAll;
        }

        // Animate DPAD
        { 
            D3DXVECTOR3 vAxis(0,0,0);

            if( m_pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP )    vAxis.x = +1.0f;
            if( m_pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN )  vAxis.x = -1.0f;
            if( m_pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT )  vAxis.y = +1.0f;
            if( m_pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) vAxis.y = -1.0f;

            if( vAxis.x || vAxis.y )
            {
                D3DXMATRIX matTrans1, matRotate, matTrans2, matAll;
                D3DXMatrixTranslation( &matTrans1, -g_vDPadOffset.x, -g_vDPadOffset.y, -g_vDPadOffset.z );
                D3DXMatrixRotationAxis( &matRotate, &vAxis, D3DX_PI/20 );
                D3DXMatrixTranslation( &matTrans2, g_vDPadOffset.x, g_vDPadOffset.y, g_vDPadOffset.z );
                D3DXMatrixMultiply( &matAll, &matTrans1, &matRotate );
                D3DXMatrixMultiply( &matAll, &matAll, &matTrans2 );
                g_ControlMatrix[CONTROL_DPAD] = matAll;
            }
            else
                D3DXMatrixIdentity( &g_ControlMatrix[CONTROL_DPAD] );
        }

        // Animate left thumbstick
        { 
            D3DXVECTOR3 vAxis(0,0,0);

            vAxis.x = +m_pGamepad->fY1;
            vAxis.y = -m_pGamepad->fX1;

            FLOAT fStickTrans = m_pGamepad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB ? 1.0f : 0.0f;

            if( vAxis.x || vAxis.y )
            {
                D3DXMATRIX matTrans1, matRotate, matTrans2, matAll;
                D3DXMatrixTranslation( &matTrans1, -g_vLeftStickOffset.x, -g_vLeftStickOffset.y, -g_vLeftStickOffset.z );
                D3DXMatrixRotationAxis( &matRotate, &vAxis, D3DXVec3Length( &vAxis)/3 );
                D3DXMatrixTranslation( &matTrans2, g_vLeftStickOffset.x, g_vLeftStickOffset.y, g_vLeftStickOffset.z+fStickTrans );
                D3DXMatrixMultiply( &matAll, &matTrans1, &matRotate );
                D3DXMatrixMultiply( &matAll, &matAll, &matTrans2 );
                g_ControlMatrix[CONTROL_LEFTTHUMBSTICK] = matAll;
            }
            else
                D3DXMatrixTranslation( &g_ControlMatrix[CONTROL_LEFTTHUMBSTICK], 0.0f, 0.0f, fStickTrans );
        }

        // Animate right thumbstick
        { 
            D3DXVECTOR3 vAxis(0,0,0);

            vAxis.x = +m_pGamepad->fY2;
            vAxis.y = -m_pGamepad->fX2;

            FLOAT fStickTrans = m_pGamepad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ? 1.0f : 0.0f;

            if( vAxis.x || vAxis.y )
            {
                D3DXMATRIX matTrans1, matRotate, matTrans2, matAll;
                D3DXMatrixTranslation( &matTrans1, -g_vRightStickOffset.x, -g_vRightStickOffset.y, -g_vRightStickOffset.z );
                D3DXMatrixRotationAxis( &matRotate, &vAxis, D3DXVec3Length( &vAxis)/3 );
                D3DXMatrixTranslation( &matTrans2, g_vRightStickOffset.x, g_vRightStickOffset.y, g_vRightStickOffset.z+fStickTrans );
                D3DXMatrixMultiply( &matAll, &matTrans1, &matRotate );
                D3DXMatrixMultiply( &matAll, &matAll, &matTrans2 );
                g_ControlMatrix[CONTROL_RIGHTTHUMBSTICK] = matAll;
            }
            else
                D3DXMatrixTranslation( &g_ControlMatrix[CONTROL_RIGHTTHUMBSTICK], 0.0f, 0.0f, fStickTrans );
        }
    }

    // Handle the vibration test page
    if( m_AppState == APPSTATE_VIBRATIONTEST )
    {
        m_fLeftMotorSpeed  = m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] / 255.0f;
        m_fRightMotorSpeed = m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] / 255.0f;
    }

    // Handle the deadzone calibration page
    if( m_AppState == APPSTATE_DEADZONECALIBRATION )
    {
        // Adjust the deadzone
        if( m_pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP )
            m_fDeadZone += 1.0f/256.0f;
        if( m_pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN )
            m_fDeadZone -= 1.0f/256.0f;
        m_fDeadZone = min( 1.0f, max( 0.0f, m_fDeadZone ) );
    }

    // Handle the button quantization page
    if( m_AppState == APPSTATE_BUTTONQUANTIZATION )
    {
        // Reset the recorded values
        if( m_pGamepad->wButtons & XINPUT_GAMEPAD_START )
        {
            ZeroMemory( m_pQuantizedThumbStickValues, 256 );
            ZeroMemory( m_pQuantizedButtonValues, 256 );
        }

        // Record quantized thumbstick values
        m_pQuantizedThumbStickValues[ (32768 + m_pGamepad->sThumbLX) >> 8 ] |= 0x01;
        m_pQuantizedThumbStickValues[ (32768 + m_pGamepad->sThumbLY) >> 8 ] |= 0x02;
        m_pQuantizedThumbStickValues[ (32768 + m_pGamepad->sThumbRX) >> 8 ] |= 0x04;
        m_pQuantizedThumbStickValues[ (32768 + m_pGamepad->sThumbRY) >> 8 ] |= 0x08;

        // Record quantized button values
        m_pQuantizedButtonValues[ m_pGamepad->bAnalogButtons[0] ] |= 0x01;
        m_pQuantizedButtonValues[ m_pGamepad->bAnalogButtons[1] ] |= 0x02;
        m_pQuantizedButtonValues[ m_pGamepad->bAnalogButtons[2] ] |= 0x04;
        m_pQuantizedButtonValues[ m_pGamepad->bAnalogButtons[3] ] |= 0x08;
        m_pQuantizedButtonValues[ m_pGamepad->bAnalogButtons[4] ] |= 0x10;
        m_pQuantizedButtonValues[ m_pGamepad->bAnalogButtons[5] ] |= 0x20;
        m_pQuantizedButtonValues[ m_pGamepad->bAnalogButtons[6] ] |= 0x40;
        m_pQuantizedButtonValues[ m_pGamepad->bAnalogButtons[7] ] |= 0x80;
    }

    // Set the vibration motors
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
// Name: DrawBox()
// Desc: Graphics helper function
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawBox( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2,
                              DWORD dwFillColor, DWORD dwOutlineColor )
{
    D3DXVECTOR4 v[4];
    v[0] = D3DXVECTOR4( x1-0.5f, y1-0.5f, 0, 0 );
    v[1] = D3DXVECTOR4( x2-0.5f, y1-0.5f, 0, 0 );
    v[2] = D3DXVECTOR4( x2-0.5f, y2-0.5f, 0, 0 );
    v[3] = D3DXVECTOR4( x1-0.5f, y2-0.5f, 0, 0 );

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW );

    // Render the box
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwFillColor );
    m_pd3dDevice->DrawVerticesUP( D3DPT_QUADLIST, 4, v, sizeof(v[0]) );

    // Render the lines
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwOutlineColor );
    m_pd3dDevice->DrawVerticesUP( D3DPT_LINELOOP, 4, v, sizeof(v[0]) );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderInsertRemoveControllerPage()
// Desc: Inform the user to please insert or remove a controller
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderInsertRemoveControllerPage()
{
    // Draw a gradient filled background
    RenderGradientBackground( 0xff0000ff, 0xff000000 );

    // Set some default state
    m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,          0x00ffffff );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         TRUE );

    // Draw header
    DrawBox( 64, 50, 576, 93, 0x40000000, 0xff000000 );
    m_Font16.DrawText(  69,  50, 0xffffffff, L"Xbox Controller Tool", XBFONT_LEFT );

    // Display a message requesting the user to insert a gamepad controller. Since
    // the Xbox input API take a second or so to detect a controller, let's delay
    // this message a tiny bit
    if( m_fAppTime > 2.0f )
    {
        DrawBox( 64, 103, 576, 430, 0x40000000, 0xff000000 );

        if( m_dwNumInsertedGamepads == 0 )
            m_Font16.DrawText( 320, 250, 0xffffffff, L"Please insert a gamepad controller", XBFONT_CENTER_X );
        else // if( m_dwNumInsertedGamepads > 1 )
            m_Font16.DrawText( 320, 250, 0xffffffff, L"Please remove all but one gamepad controllers", XBFONT_CENTER_X );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderControlTestPage()
// Desc: Renders the page for testing the controls
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderControlTestPage()
{
    // Draw a gradient filled background
    RenderGradientBackground( 0xff0000ff, 0xff000000 );

    // Set some default state
    m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,          0x00ffffff );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         TRUE );

    // Since xrays go through objects, it only looks convincing if we turn on
    // alpha-blending and render both sides of the object 
    if( m_bAlphaEnabled )
    {
        // Lighting is not needed, since we don't use the diffuse color
        m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,     FALSE );

        // Setup to render both sides of the object
        m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,     D3DCULL_NONE );

        // Turn on alpha blending
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,     D3DBLEND_SRCCOLOR );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,    D3DBLEND_ONE );

        // Set the transparency
        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0x80808080 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
    }

    // Generate spheremap texture coords from the camera space normal. This has
    // two steps. First, tell D3D to use the vertex normal (in camera space) as
    // texture coordinates. Then, we setup a texture matrix to transform these
    // texcoords from (-1,+1) view space to (0,1) texture space. This way,
    // the normal can be used to look up a texel in the spheremap.
    D3DXMATRIX mat;
    mat._11 = 0.5f; mat._12 = 0.0f;
    mat._21 = 0.0f; mat._22 =-0.5f;
    mat._31 = 0.0f; mat._32 = 0.0f;
    mat._41 = 0.5f; mat._42 = 0.5f;
    m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );

    // Finally, draw the object
    m_pGamepadMesh->Render( m_pd3dDevice, XBMESH_NOTEXTURES|XBMESH_NOMATERIALS );

    // Restore render states
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,     TRUE );

    // Draw back side to fill holes
    if( FALSE == m_bAlphaEnabled )
    {
        m_pd3dDevice->SetTexture( 0, NULL );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xff000000 );
        m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
        m_pGamepadMesh->Render( m_pd3dDevice, XBMESH_NOTEXTURES|XBMESH_NOMATERIALS );
    }

    // Restore render states
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE );


    // Draw header
    DrawBox( 64, 50, 576, 93, 0x40000000, 0xff000000 );
    m_Font16.DrawText(  69,  50, 0xffffffff, L"Xbox Controller Tool", XBFONT_LEFT );
    m_Font16.DrawText( 571,  50, 0xffffffff, L"Control Test Page", XBFONT_RIGHT );
    m_Font12.DrawText( 571,  75, 0xff808080, L"Press START and BACK for next page", XBFONT_RIGHT );

    // Draw options
    m_Font12.Begin();


    WCHAR strBuffer[40];

    swprintf( strBuffer, L"LeftTrigger = %d", m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] );
    m_Font12.DrawText( 140,  95, m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] ? 0xffffff00: 0x80ffffff, strBuffer );
    
    swprintf( strBuffer, L"RightTrigger = %d", m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] );
    m_Font12.DrawText( 350,  95, m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] ? 0xffffff00: 0x80ffffff, strBuffer );

    swprintf( strBuffer, L"LeftStick.x = %d", m_pGamepad->sThumbLX );
    m_Font12.DrawText(  64, 116, m_pGamepad->fX1!=0.0f ? 0xffffff00: 0x80ffffff, strBuffer );
    swprintf( strBuffer, L"LeftStick.y = %d", m_pGamepad->sThumbLY );
    m_Font12.DrawText(  64, 134, m_pGamepad->fY1!=0.0f ? 0xffffff00: 0x80ffffff, strBuffer );
    swprintf( strBuffer, L"LeftThumb" );
    m_Font12.DrawText(  64, 152, m_pGamepad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB ? 0xffffff00: 0x80ffffff, strBuffer );

    m_Font12.DrawText( 110, 362, m_pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP    ? 0xffffff00: 0x80ffffff, L"Up",    XBFONT_CENTER_X );
    m_Font12.DrawText(  85, 380, m_pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT  ? 0xffffff00: 0x80ffffff, L"Left",  XBFONT_CENTER_X );
    m_Font12.DrawText( 135, 380, m_pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 0xffffff00: 0x80ffffff, L"Right", XBFONT_CENTER_X );
    m_Font12.DrawText( 110, 398, m_pGamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN  ? 0xffffff00: 0x80ffffff, L"Down",  XBFONT_CENTER_X );

    m_Font12.DrawText( 320, 380, g_ControlActive[CONTROL_BACKBUTTON] ? 0xffffff00: 0x80ffffff, L"Back", XBFONT_RIGHT );
    m_Font12.DrawText( 330, 380, g_ControlActive[CONTROL_STARTBUTTON] ? 0xffffff00: 0x80ffffff, L"Start", XBFONT_LEFT );
    
    swprintf( strBuffer, L"White = %d", m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_WHITE] );
    m_Font12.DrawText( 390, 116, g_ControlActive[CONTROL_WHITEBUTTON] ? 0xffffff00: 0x80ffffff, strBuffer );
    swprintf( strBuffer, L"Y = %d", m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_Y] );
    m_Font12.DrawText( 390, 134, g_ControlActive[CONTROL_YBUTTON] ? 0xffffff00: 0x80ffffff, strBuffer );
    swprintf( strBuffer, L"X = %d", m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_X] );
    m_Font12.DrawText( 390, 152, g_ControlActive[CONTROL_XBUTTON] ? 0xffffff00: 0x80ffffff, strBuffer );

    swprintf( strBuffer, L"Black = %d", m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_BLACK] );
    m_Font12.DrawText( 490, 116, g_ControlActive[CONTROL_BLACKBUTTON] ? 0xffffff00: 0x80ffffff, strBuffer );
    swprintf( strBuffer, L"B = %d", m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_B] );
    m_Font12.DrawText( 490, 134, g_ControlActive[CONTROL_BBUTTON] ? 0xffffff00: 0x80ffffff, strBuffer );
    swprintf( strBuffer, L"A = %d", m_pGamepad->bAnalogButtons[XINPUT_GAMEPAD_A] );
    m_Font12.DrawText( 490, 152, g_ControlActive[CONTROL_ABUTTON] ? 0xffffff00: 0x80ffffff, strBuffer );

    swprintf( strBuffer, L"RightStick.x = %d", m_pGamepad->sThumbRX );
    m_Font12.DrawText( 400, 362, m_pGamepad->fX2!=0.0f ? 0xffffff00: 0x80ffffff, strBuffer );
    swprintf( strBuffer, L"RightStick.y = %d", m_pGamepad->sThumbRY );
    m_Font12.DrawText( 400, 380, m_pGamepad->fY2!=0.0f ? 0xffffff00: 0x80ffffff, strBuffer );
    swprintf( strBuffer, L"RightThumb" );
    m_Font12.DrawText( 400, 398, m_pGamepad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ? 0xffffff00: 0x80ffffff, strBuffer );

    m_Font12.End();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderVibrationTestPage()
// Desc: Renders the page for testing the gamepad vibration motors.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderVibrationTestPage()
{
    // Draw a gradient filled background
    RenderGradientBackground( 0xff0000ff, 0xff000000 );

    // Draw header
    DrawBox( 64, 50, 576, 93, 0x40000000, 0xff000000 );
    m_Font16.DrawText(  69,  50, 0xffffffff, L"Xbox Controller Tool", XBFONT_LEFT );
    m_Font16.DrawText( 571,  50, 0xffffffff, L"Vibration Test Page", XBFONT_RIGHT );
    m_Font12.DrawText( 571,  75, 0xff808080, L"Press START and BACK for next page", XBFONT_RIGHT );

    // Draw instructions
    DrawBox( 376, 100, 576, 430, 0x40000000, 0xff000000 );
    m_Font12.DrawText( 381, 100, 0xff808080, L"Use the left and right\n"
                                             L"triggers to test the\n"
                                             L"vibration function of\n"
                                             L"gamepad motors.\n" );

    // Draw outside box
    DrawBox( 64, 100, 366, 430, 0x00000000, 0xff000000 );

    // Draw the analog gauges
    m_pd3dDevice->SetTexture( 0, m_xprResource.GetTexture( resource_AnalogGauge_OFFSET ) );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    struct VERTEX { D3DXVECTOR4 p; FLOAT tu, tv; };
    VERTEX v[4];
    v[0].p = D3DXVECTOR4( 215-100     - 0.5f, 120     - 0.5f, 0, 0 );  v[0].tu =   0; v[0].tv =   0;
    v[1].p = D3DXVECTOR4( 215-100+256 - 0.5f, 120     - 0.5f, 0, 0 );  v[1].tu = 256; v[1].tv =   0;
    v[2].p = D3DXVECTOR4( 215-100+256 - 0.5f, 120+128 - 0.5f, 0, 0 );  v[2].tu = 256; v[2].tv = 128;
    v[3].p = D3DXVECTOR4( 215-100     - 0.5f, 120+128 - 0.5f, 0, 0 );  v[3].tu =   0; v[3].tv = 128;
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, v, sizeof(v[0]) );
    v[0].p = D3DXVECTOR4( 215-100     - 0.5f, 260     - 0.5f, 0, 0 );  v[0].tu =   0; v[0].tv =   0;
    v[1].p = D3DXVECTOR4( 215-100+256 - 0.5f, 260     - 0.5f, 0, 0 );  v[1].tu = 256; v[1].tv =   0;
    v[2].p = D3DXVECTOR4( 215-100+256 - 0.5f, 260+128 - 0.5f, 0, 0 );  v[2].tu = 256; v[2].tv = 128;
    v[3].p = D3DXVECTOR4( 215-100     - 0.5f, 260+128 - 0.5f, 0, 0 );  v[3].tu =   0; v[3].tv = 128;
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, v, sizeof(v[0]) );

    // Draw the gauge needles
    FLOAT fLeftAngle  = 0.85f * (2*m_fLeftMotorSpeed-1);
    FLOAT fRightAngle = 0.85f * (2*m_fRightMotorSpeed-1);
    v[0].p.x = 215 + 92*sinf(fLeftAngle);   v[0].p.y = 120 + 111 - 92*cosf(fLeftAngle); 
    v[1].p.x = 215 + 48*sinf(fLeftAngle);   v[1].p.y = 120 + 111 - 48*cosf(fLeftAngle); 
    v[2].p.x = 215 + 92*sinf(fRightAngle);  v[2].p.y = 260 + 111 - 92*cosf(fRightAngle); 
    v[3].p.x = 215 + 48*sinf(fRightAngle);  v[3].p.y = 260 + 111 - 48*cosf(fRightAngle); 
    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 2, v, sizeof(v[0]) );

    // Draw the motor text
    WCHAR strBuffer[80];
    swprintf( strBuffer, L"%d%%", (WORD)(100*m_fLeftMotorSpeed) );
    m_Font16.DrawText( 215+40, 120+100, 0xffffffff, L"Left Motor: ", XBFONT_RIGHT );
    m_Font16.DrawText( 215+40, 120+100, 0xffffff00, strBuffer, XBFONT_LEFT );

    swprintf( strBuffer, L"%d%%", (WORD)(100*m_fRightMotorSpeed) );
    m_Font16.DrawText( 215+40, 260+100, 0xffffffff, L"Right Motor: ", XBFONT_RIGHT );
    m_Font16.DrawText( 215+40, 260+100, 0xffffff00, strBuffer, XBFONT_LEFT );
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderDeadZoneCalibrationPage()
// Desc: Renders the page for calibrating the thumbsticks' deadzone.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderDeadZoneCalibrationPage()
{
    // Draw a gradient filled background
    RenderGradientBackground( 0xff0000ff, 0xff000000 );

    // Draw header
    DrawBox( 64, 50, 576, 93, 0x40000000, 0xff000000 );
    m_Font16.DrawText(  69,  50, 0xffffffff, L"Xbox Controller Tool", XBFONT_LEFT );
    m_Font16.DrawText( 571,  50, 0xffffffff, L"Deadzone Calibration Page", XBFONT_RIGHT );
    m_Font12.DrawText( 571,  75, 0xff808080, L"Press START and BACK for next page", XBFONT_RIGHT );

    FLOAT fBoxCenterX = 204.0f;
    FLOAT fBoxCenterY = 240.0f;
    FLOAT fBoxRadius  = 140.0f;
    FLOAT fDeadZone   = m_fDeadZone * fBoxRadius;

    // Draw outside box
    DrawBox( fBoxCenterX - fBoxRadius, fBoxCenterY - fBoxRadius, 
             fBoxCenterX + fBoxRadius, fBoxCenterY + fBoxRadius, 
             0x00000000, 0xff000000 );

    // Draw inside box
    DrawBox( fBoxCenterX - fDeadZone, fBoxCenterY - fDeadZone, 
             fBoxCenterX + fDeadZone, fBoxCenterY + fDeadZone, 
             0x00000000, 0xffffffff );

    // Draw left thumb stick in red
    FLOAT fThumbLX = fBoxCenterX + fBoxRadius*(m_pGamepad->sThumbLX+0.5f)/32767.5f;
    FLOAT fThumbLY = fBoxCenterY - fBoxRadius*(m_pGamepad->sThumbLY+0.5f)/32767.5f;
    DrawBox( fThumbLX-1, fThumbLY-8, fThumbLX+1, fThumbLY+8, 0xffff0000, 0xffff0000 );
    DrawBox( fThumbLX-8, fThumbLY-1, fThumbLX+8, fThumbLY+1, 0xffff0000, 0xffff0000 );

    // Draw right thumb stick in green
    FLOAT fThumbRX = fBoxCenterX + fBoxRadius*(m_pGamepad->sThumbRX+0.5f)/32767.5f;
    FLOAT fThumbRY = fBoxCenterY - fBoxRadius*(m_pGamepad->sThumbRY+0.5f)/32767.5f;
    DrawBox( fThumbRX-1, fThumbRY-8, fThumbRX+1, fThumbRY+8, 0xff00ff00, 0xff00ff00 );
    DrawBox( fThumbRX-8, fThumbRY-1, fThumbRX+8, fThumbRY+1, 0xff00ff00, 0xff00ff00 );

    // Draw instructions
    DrawBox( 354, 100, 576, 380, 0x40000000, 0xff000000 );
    m_Font12.DrawText( 359, 100, 0xff808080, L"Move thumbsticks to test\n"
                                             L"sensitivty around the\n"
                                             L"the deadzone (defined by\n"
                                             L"the white box). Let them\n"
                                             L"snap back and notice how\n"
                                             L"the controls adapt over\n"
                                             L"time allowing for a smaller\n"
                                             L"deadzone.\n\n"
                                             L"Use DPad to control the\n"
                                             L"deadzone size.");

    WCHAR strBuffer[80];
    swprintf( strBuffer, L"Deadzone: %ld (%ld%%)", (LONG)(32768*m_fDeadZone), (LONG)(100*m_fDeadZone) );
    m_Font12.DrawText( 359, 360, 0xffffffff, strBuffer );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderButtonQuantizationPage()
// Desc: Renders the page for testing the quantization of the controls
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderButtonQuantizationPage()
{
    // Draw a gradient filled background
    RenderGradientBackground( 0xff0000ff, 0xff000000 );

    // Draw header
    DrawBox( 64, 50, 576, 93, 0x40000000, 0xff000000 );
    m_Font16.DrawText(  69,  50, 0xffffffff, L"Xbox Controller Tool", XBFONT_LEFT );
    m_Font16.DrawText( 571,  50, 0xffffffff, L"Control Quantization Page", XBFONT_RIGHT );
    m_Font12.DrawText( 571,  75, 0xff808080, L"Press START and BACK for next page", XBFONT_RIGHT );

    // Draw instructions
    DrawBox( 374, 100, 576, 400, 0x40000000, 0xff000000 );
    m_Font12.DrawText( 379, 100, 0xff808080, L"Analog controls have 8\n"
                                             L"bits of precision. Press\n"
                                             L"the analog buttons and\n"
                                             L"move the thumbsticks to\n"
                                             L"see what values are\n"
                                             L"quantized. Hit the START\n"
                                             L"button to reset the values.");

    // Draw outside box
    DrawBox( 64, 100, 364, 400, 0x00000000, 0xff000000 );

    // Colors for rendering the quantized values
    DWORD adwColors[8] =
    {
        0xff80ff80, // Light green
        0xffff8080, // Light red
        0xff8080ff, // Light blue
        0xffffff80, // Light yellow
        0xff000000, // Black
        0xffffffff, // White
        0xff000000, // Black
        0xff000000, // Black
    };

    // Count of unique quantized values for each of the 8 analog controls and 
    // the 4 thumbstick axes.
    DWORD adwQuantizationCount[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    // Display quantized button values
    m_Font12.DrawText( 150, 100, 0xffffffff, L"Buttons", XBFONT_CENTER_X );

    for( DWORD x=0; x<8; x++ )
    {
        // Draw gray bar
        DrawBox( 20.0f*x+78.0f, 120.0f, 20.0f*x+82.0f, 376.0f, 0xff404040, 0xff404040 );

        for( DWORD y=0; y<256; y++ )
        {
            // Highlight recorded values
            if( m_pQuantizedButtonValues[y] & (1<<x) )
            {
                DrawBox( 20.0f*x+76.0f, 376.0f-y, 20.0f*x+84.0f, 376.0f-y, adwColors[x], adwColors[x] );
                adwQuantizationCount[x]++;
            }

            // Highlight the current value
            DrawBox( 20.0f*x+74.0f, 375.0f-m_pGamepad->bAnalogButtons[x], 
                     20.0f*x+86.0f, 377.0f-m_pGamepad->bAnalogButtons[x], 
                     adwColors[x], adwColors[x] );
        }

        // Draw label
        m_Font12.DrawText( 20.0f*x+80.0f, 376.0f, 0xffffffff, L"A\0B\0X\0Y\0B\0W\0L\0R\0"+2*x, XBFONT_CENTER_X );
    }

    // Display quantized thumbstick values
    m_Font12.DrawText( 305.0f, 100.0f, 0xffffffff, L"Thumbsticks", XBFONT_CENTER_X );

    for( x=0; x<4; x++ )
    {
        // Draw gray bar
        DrawBox( 30.0f*x+258.0f, 120.0f, 30.0f*x+262.0f, 376.0f, 0xff404040, 0xff404040 );

        for( DWORD y=0; y<256; y++ )
        {
            // Highlight recorded values
            if( m_pQuantizedThumbStickValues[y] & (1<<x) )
            {
                DrawBox( 30.0f*x+256.0f, 376.0f-y, 30.0f*x+264, 376.0f-y, 0xffffffff, 0xffffffff );
                adwQuantizationCount[8+x]++;
            }

            // Highlight the current value
            BYTE val = 0;
            if( x == 0 ) val = (32768 + m_pGamepad->sThumbLX) >> 8;
            if( x == 1 ) val = (32768 + m_pGamepad->sThumbLY) >> 8;
            if( x == 2 ) val = (32768 + m_pGamepad->sThumbRX) >> 8;
            if( x == 3 ) val = (32768 + m_pGamepad->sThumbRY) >> 8;
            DrawBox( 30.0f*x+254.0f, 375.0f-val, 30.0f*x+266.0f, 377.0f-val, 0xffff0000, 0xffff0000 );
        }

        // Draw label
        m_Font12.DrawText( 30.0f*x+260.0f, 376.0f, 0xffffffff, L"LX\0LY\0RX\0RY\0"+3*x, XBFONT_CENTER_X );
    }

    // Show the number of unique values each analog control has hit
    WCHAR strBuffer[80];
    m_Font12.DrawText( 379, 260, 0xff808080, L"Count of unique values:" );

    m_Font12.DrawText( 399, 280, 0xff808080, L"A\nB\nX\nY\nB\nW\n" );
    swprintf( strBuffer, L"%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n",
                         adwQuantizationCount[0], adwQuantizationCount[1], 
                         adwQuantizationCount[2], adwQuantizationCount[3], 
                         adwQuantizationCount[4], adwQuantizationCount[5] );
    m_Font12.DrawText( 419, 280, 0xffffffff, strBuffer );

    m_Font12.DrawText( 485, 280, 0xff808080, L"LT\nRT\nLX\nLY\nRX\nRY\n" );
    swprintf( strBuffer, L"%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n",
                         adwQuantizationCount[6], adwQuantizationCount[7], 
                         adwQuantizationCount[8], adwQuantizationCount[9], 
                         adwQuantizationCount[10], adwQuantizationCount[11] );
    m_Font12.DrawText( 515, 280, 0xffffffff, strBuffer );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    if( m_dwNumInsertedGamepads != 1 )
    {
        RenderInsertRemoveControllerPage();
    }
    else
    {
        switch( m_AppState )
        {
            case APPSTATE_CONTROLTEST:
                RenderControlTestPage();
                break;

            case APPSTATE_VIBRATIONTEST:
                RenderVibrationTestPage();
                break;
        
            case APPSTATE_DEADZONECALIBRATION:
                RenderDeadZoneCalibrationPage();
                break;
        
            case APPSTATE_BUTTONQUANTIZATION:
                RenderButtonQuantizationPage();
                break;
        }
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

