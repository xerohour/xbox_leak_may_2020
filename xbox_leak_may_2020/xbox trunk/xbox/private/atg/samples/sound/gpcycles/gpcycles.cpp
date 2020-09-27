#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>


extern HRESULT PlaySineWave(double Frequency, DWORD dwMixBins,LPDIRECTSOUNDBUFFER     *pBuffer );

extern HRESULT
PlayLoopingBuffer
(
    LPCSTR pszFile,
    LPDIRECTSOUNDBUFFER     *pBuffer
);

extern HRESULT
DownloadEffectsImage(PCHAR pszScratchFile);

#define MAX_GP_IDLE_CYCLES  106720 // 32 samples at 160Mhz (or 667us*160Mhz)

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_BLACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" }    
};

#define NUM_HELP_CALLOUTS 2

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

    FLOAT       m_fGPAvgCycles;
    DWORD       m_dwGPMinCycles;
    DWORD       m_dwGPMaxCycles;

    // Draw help?
    BOOL        m_bDrawHelp;

    HRESULT     m_hOpenResult;
    LPDIRECTSOUNDBUFFER m_pDSBuffer;
    LPDIRECTSOUNDBUFFER m_pOscillatorBuffer0;
    LPDIRECTSOUNDBUFFER m_pOscillatorBuffer1;
    LPDIRECTSOUNDBUFFER m_pOscillatorBuffer2;

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
    m_fGPAvgCycles = 0.0f;
    m_dwGPMinCycles = -1;
    m_dwGPMaxCycles = 0;
}

//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    HRESULT                 hr  = DS_OK;
        
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    //
    // create oscillators that drive FX
    //

    hr = PlaySineWave(3,DSMIXBIN_FXSEND_2,&m_pOscillatorBuffer0);
    if(SUCCEEDED(hr)) hr = PlaySineWave(1,DSMIXBIN_FXSEND_3,&m_pOscillatorBuffer1);
    if(SUCCEEDED(hr)) hr = PlaySineWave(1005,DSMIXBIN_FXSEND_4,&m_pOscillatorBuffer2);

    //
    // create audio buffer
    //

    if(SUCCEEDED(hr)) hr = PlayLoopingBuffer("d:\\media\\sounds\\CleanGuitarArpeggios.wav", &m_pDSBuffer);

    //
    // download an effects image
    //
    
    if(SUCCEEDED(hr)) hr = DownloadEffectsImage("d:\\media\\scratchimg.bin");

    return S_OK;
}

#define ELAPSED_TIME_BASE    (0x30000+512-4*sizeof(DWORD))

#define USAGE_SCALE 20.0f
// CPU_MAXTIME is 100% CPU utilization in seconds
#define CPU_MAXTIME 0.016667f * 6
// GPU_MAXTIME is 100% GPU utilization in seconds
#define GPU_MAXTIME 0.016667f * 6
//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FrameMove()
{
    LARGE_INTEGER liFreq;
    FLOAT         fSecondsPerTick;
    LARGE_INTEGER liStart;
    LARGE_INTEGER liCurrent;

    DWORD dwValue;

    //
    // get some DSP usage statistics straight from a variable inside the DSP X-ram and print them out
    //

    dwValue = MAX_GP_IDLE_CYCLES - *(PDWORD)(0xFE800000+ELAPSED_TIME_BASE);
    m_fGPAvgCycles = (FLOAT) dwValue;

    if (dwValue>m_dwGPMaxCycles) {
        m_dwGPMaxCycles=dwValue;
    }

    if (dwValue<m_dwGPMinCycles) {
        m_dwGPMinCycles=dwValue;
    }

    if (m_dwGPMinCycles <= 1000) {
        m_dwGPMinCycles = m_dwGPMaxCycles;
    }

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
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
    WCHAR szString[256];
    DWORD dwValue=0;

    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    DirectSoundDoWork();

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    dwValue = (DWORD)m_fGPAvgCycles;
    swprintf (szString,L"Average DSP Idle cycles per Frame: %d",dwValue);
    m_Font.DrawText( 50, 150, 0xFFFF0000, szString);

    swprintf (szString,L"Min.(worst case) DSP Idle cycles per Frame: %d",m_dwGPMinCycles);
    m_Font.DrawText( 50, 250, 0xFF0000FF, szString);

    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );

    // Draw the GP Avg cycles Usage bar
    {
        struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
        BACKGROUNDVERTEX v[4];
        FLOAT x1 = 100, x2 = x1 + (340*m_fGPAvgCycles)/MAX_GP_IDLE_CYCLES;
        FLOAT y1 = 180, y2 = y1 + 20;
        v[0].p = D3DXVECTOR4( x1-0.5f, y1-0.5f, 1.0f, 1.0f );  v[0].color = 0xffffffff;
        v[1].p = D3DXVECTOR4( x2-0.5f, y1-0.5f, 1.0f, 1.0f );  v[1].color = 0xffffffff;
        v[2].p = D3DXVECTOR4( x1-0.5f, y2-0.5f, 1.0f, 1.0f );  v[2].color = 0xffff0000;
        v[3].p = D3DXVECTOR4( x2-0.5f, y2-0.5f, 1.0f, 1.0f );  v[3].color = 0xffff0000;

        m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
    }

    // Draw the minimum cycles bar
    {
        struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
        BACKGROUNDVERTEX v[4];
        FLOAT x1 = 100, x2 = x1 + (340*(FLOAT)m_dwGPMinCycles)/MAX_GP_IDLE_CYCLES;
        FLOAT y1 = 280, y2 = y1 + 20;
        v[0].p = D3DXVECTOR4( x1-0.5f, y1-0.5f, 1.0f, 1.0f );  v[0].color = 0xffffffff;
        v[1].p = D3DXVECTOR4( x2-0.5f, y1-0.5f, 1.0f, 1.0f );  v[1].color = 0xffffffff;
        v[2].p = D3DXVECTOR4( x1-0.5f, y2-0.5f, 1.0f, 1.0f );  v[2].color = 0xff0000ff;
        v[3].p = D3DXVECTOR4( x2-0.5f, y2-0.5f, 1.0f, 1.0f );  v[3].color = 0xff0000ff;

        m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
		m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"GPCycles" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
		m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

