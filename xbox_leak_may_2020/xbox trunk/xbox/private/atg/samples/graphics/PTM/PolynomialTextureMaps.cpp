//-----------------------------------------------------------------------------
// File: PTM.cpp
//
// Desc: Example showing how to implement polynomial texture maps using the 
//       XBox GPU.
//
//       For a full description of polynomail texture maps see 
//       T. Malzbender, D. Gelb, and H. Wolters. Polynomial Texture Maps. In 
//       "Computer Graphics Proceedings", "Annual Conference Series", 2001, 
//       pages 519-528, August 2001.
//
// Hist: 01.21.02 - New for February XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <XBResource.h>
#include <XBUtil.h>
#include <assert.h>

// The following header file is generated from "Resource.rdf" file using the
// Bundler tool. In addition to the header, the tool outputs a binary file
// (Resource.xpr) which contains compiled (i.e. bundled) resources and is
// loaded at runtime using the CXBPackedResource class.
#include "Resource.h"




// By default we load the pre-computed polynomial textures from the resource
// file.  Setting this option to 1 will cause the textures to be computed
// from the sample textures.  However, in order to do so you will need to
// supply you own singular value decomposition routines.
#define COMPUTE_POLYNOMIAL_TEXTURE_MAPS 0




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Move Light" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
};

#define NUM_HELP_CALLOUTS (sizeof(g_HelpCallouts)/sizeof(g_HelpCallouts[0]))




//-----------------------------------------------------------------------------
// Data defining the quad the polynomial texure is applied to.
//-----------------------------------------------------------------------------
struct QUADVERT
{
    D3DVECTOR Pos;
    float U, V;
};


QUADVERT g_QuadVerts[] =
{
    {{ -1.0f,  1.0f, 0.0f, }, 0.0f, 0.0f },
    {{  1.0f,  1.0f, 0.0f, }, 1.0f, 0.0f },
    {{  1.0f, -1.0f, 0.0f, }, 1.0f, 1.0f },
    {{ -1.0f, -1.0f, 0.0f, }, 0.0f, 1.0f },
};


struct LINEVERT
{
    D3DVECTOR pos;
    D3DCOLOR color;
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource  m_xprResource;        // Packed resources for the app
    CXBFont            m_Font;               // Font class
    CXBHelp            m_Help;               // Help class
    BOOL               m_bDrawHelp;          // Whether to draw help

    IDirect3DTexture8* m_pCoeff1Texture;    // a0, a1, a2
    IDirect3DTexture8* m_pCoeff2Texture;    // a3, a4, a5
    IDirect3DTexture8* m_pColorTexture;     // r, g, b

    DWORD              m_dwPTMVertexShader;
    DWORD              m_dwPTMPixelShader;

    int                m_iBias[6];

    D3DXVECTOR3        m_vLight;
    D3DXVECTOR3        m_vEye;
    D3DXMATRIX         m_matWorld;
    D3DXMATRIX         m_matView;
    D3DXMATRIX         m_matProjection;

    HRESULT ComputePolynomialTextures();

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
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    // Allow unlimited frame rate
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    m_bDrawHelp     = false;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Load the resource, create the shaders, and intialize the view.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", Resource_NUM_RESOURCES, NULL ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the vertex shader.
    DWORD vdecl[] =
    {
        D3DVSD_STREAM(0),
        D3DVSD_REG(0, D3DVSDT_FLOAT3), // v0 = xyz
        D3DVSD_REG(1, D3DVSDT_FLOAT2), // v1 = uv
        D3DVSD_END()
    };

    if ( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\PTM.xvu", vdecl, &m_dwPTMVertexShader ) ) )
        return E_FAIL;

    // Create the pixel shader.
    if ( FAILED( XBUtil_CreatePixelShader( m_pd3dDevice, "Shaders\\PTM.xpu", &m_dwPTMPixelShader ) ) )
        return E_FAIL;

#if (COMPUTE_POLYNOMIAL_TEXTURE_MAPS)

    ComputePolynomialTextures();

#else

    // Get the textures.
    m_pCoeff1Texture = m_xprResource.GetTexture( Resource_MoonPoly1_OFFSET );
    m_pCoeff2Texture = m_xprResource.GetTexture( Resource_MoonPoly2_OFFSET );
    m_pColorTexture = m_xprResource.GetTexture( Resource_MoonColor_OFFSET );

    // Read the bias factors for the polynomial coefficients.
    FILE* fp = fopen( "D:\\Media\\MoonCoeffs.txt", "r" );
	if(!fp)
		return E_FAIL;

    for (int i = 0; i < 6; i++)
    {
        if(fscanf(fp, "%d", &m_iBias[i]) != 1)
			return E_FAIL;
    }

    fclose(fp);

#endif

    // Set the matrices
    D3DXMatrixIdentity( &m_matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

    m_vLight = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
    m_vEye = D3DXVECTOR3( 0.0f, 0.0f, -2.0f );
    D3DXVECTOR3 vAt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );

    D3DXMatrixLookAtLH( &m_matView, &m_vEye, &vAt, &vUp );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

    D3DXMatrixPerspectiveFovLH( &m_matProjection, D3DX_PI/3, 4.0f/3.0f, 0.1f, 40.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjection );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    D3DXMATRIX matView, matRotate;

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    D3DXVECTOR3 vNormal( 0.0f, 0.0f, -1.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );

    D3DXVECTOR3 vTemp;

    // Rotate light around up axis.
    D3DXMatrixRotationAxis( &matRotate, &vUp, -m_DefaultGamepad.fX1 * m_fElapsedTime );
    D3DXVec3TransformCoord( &vTemp, &m_vLight, &matRotate );

    // Place limits so we dont go around the back of the plane.
    FLOAT dot = D3DXVec3Dot( &vTemp, &vNormal );
    if( dot > 0.0f )
    {
        m_vLight = vTemp;
    }

    // Rotate eye points around side axis.
    D3DXVECTOR3 axis( 1.0f, 0.0f, 0.0f );
    D3DXMatrixRotationAxis( &matRotate, &axis, m_DefaultGamepad.fY1 * m_fElapsedTime );
    D3DXVec3TransformCoord( &vTemp, &m_vLight, &matRotate );

    dot = D3DXVec3Dot( &vTemp, &vNormal );
    if ( dot > 0.0f )
    {
        m_vLight = vTemp;
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the quad with
//       the polynomail texture map on it.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport, zbuffer, and stencil buffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0 );

    // Begin the scene
    m_pd3dDevice->BeginScene();

    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,   TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_CCW );
    
    // Draw a quad with the textures.
    m_pd3dDevice->SetVertexShader( m_dwPTMVertexShader );
    m_pd3dDevice->SetPixelShader( m_dwPTMPixelShader );

    D3DXMATRIX matComposite;
    D3DXMatrixMultiply( &matComposite, &m_matWorld, &m_matView );
    D3DXMatrixMultiply( &matComposite, &matComposite, &m_matProjection );
    D3DXMatrixTranspose( &matComposite, &matComposite );
    m_pd3dDevice->SetVertexShaderConstant( 0, &matComposite, 4 );

    D3DXVECTOR4 vLightDir = m_vLight;
    m_pd3dDevice->SetVertexShaderConstant( 5, &vLightDir, 1 );

    float fConstants[4] = { 0.5f, 0.5f, 1.0f, 0.0f };
    m_pd3dDevice->SetVertexShaderConstant( 6, fConstants, 1 );

    // Set bias for the polynomial coefficients.
    fConstants[0] = float(m_iBias[0]/255.0f);
    fConstants[1] = float(m_iBias[1]/255.0f);
    fConstants[2] = float(m_iBias[2]/255.0f);
    m_pd3dDevice->SetPixelShaderConstant( 0, fConstants, 1 );

    fConstants[0] = float(m_iBias[3]/255.0f);
    fConstants[1] = float(m_iBias[4]/255.0f);
    fConstants[2] = float(m_iBias[5]/255.0f);
    m_pd3dDevice->SetPixelShaderConstant( 1, fConstants, 1 );

    m_pd3dDevice->SetTexture( 0, m_pCoeff1Texture );
    m_pd3dDevice->SetTexture( 1, m_pCoeff2Texture );

    m_pd3dDevice->SetTexture( 2, m_pColorTexture );

    // Draw the quad.
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, g_QuadVerts, sizeof(QUADVERT) );

    // Show the vector to the light.
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZ | D3DFVF_DIFFUSE );
    m_pd3dDevice->SetPixelShader( 0 );

    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

    LINEVERT LineVerts[2];

    LineVerts[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    LineVerts[0].color = 0xff00ff00;

    LineVerts[1].pos = m_vLight * 0.75f;
    LineVerts[1].color = 0xff00ff00;

    m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, LineVerts, sizeof(LINEVERT) );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
    {
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    }
    else
    {
        // Show frame rate
        m_Font.DrawText(  64, 50, 0xffffffff, L"Polynomial Texture Maps" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
    }

    // End the scene.
    m_pd3dDevice->EndScene();

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




#if (COMPUTE_POLYNOMIAL_TEXTURE_MAPS)


// Turn off double->float conversion warning.
#pragma warning( disable : 4305 )


//-----------------------------------------------------------------------------
// Structure defining what images to use as input samples.
//-----------------------------------------------------------------------------
struct
{
    D3DVECTOR vLightPos;        // Direction of light.
    char *strFileName;          // File to load.
}
g_ImageList[] = 
{
    {{0.0000,-250.0000,0.0000},         "D:\\Media\\Textures\\TestImage0000.bmp"},
    {{223.6068,-111.8034,0.0000},       "D:\\Media\\Textures\\TestImage0001.bmp"},
    {{69.0983,-111.8034,212.6627},      "D:\\Media\\Textures\\TestImage0002.bmp"},
    {{-180.9017,-111.8034,131.4328},    "D:\\Media\\Textures\\TestImage0003.bmp"},
    {{-180.9017,-111.8034,-131.4328},   "D:\\Media\\Textures\\TestImage0004.bmp"},
    {{69.0983,-111.8034,-212.6627},     "D:\\Media\\Textures\\TestImage0005.bmp"},
    {{90.1823,-233.1676,0.0000},        "D:\\Media\\Textures\\TestImage0006.bmp"},
    {{168.2207,-184.9372,0.0000},       "D:\\Media\\Textures\\TestImage0007.bmp"},
    {{27.8679,-233.1676,85.7685},       "D:\\Media\\Textures\\TestImage0008.bmp"},
    {{51.9831,-184.9372,159.9874},      "D:\\Media\\Textures\\TestImage0009.bmp"},
    {{-72.9590,-233.1676,53.0078},      "D:\\Media\\Textures\\TestImage0010.bmp"},
    {{-136.0934,-184.9372,98.8777},     "D:\\Media\\Textures\\TestImage0011.bmp"},
    {{-72.9590,-233.1676,-53.0078},     "D:\\Media\\Textures\\TestImage0012.bmp"},
    {{-136.0934,-184.9372,-98.8777},    "D:\\Media\\Textures\\TestImage0013.bmp"},
    {{27.8679,-233.1676,-85.7684},      "D:\\Media\\Textures\\TestImage0014.bmp"},
    {{51.9831,-184.9372,-159.9874},     "D:\\Media\\Textures\\TestImage0015.bmp"},
    {{196.0886,-129.2015,85.7684},      "D:\\Media\\Textures\\TestImage0016.bmp"},
    {{142.1653,-129.2015,159.9874},     "D:\\Media\\Textures\\TestImage0017.bmp"},
    {{-20.9759,-129.2015,212.9952},     "D:\\Media\\Textures\\TestImage0018.bmp"},
    {{-108.2256,-129.2015,184.6461},    "D:\\Media\\Textures\\TestImage0019.bmp"},
    {{-209.0524,-129.2015,45.8698},     "D:\\Media\\Textures\\TestImage0020.bmp"},
    {{-209.0524,-129.2015,-45.8699},    "D:\\Media\\Textures\\TestImage0021.bmp"},
    {{-108.2256,-129.2015,-184.6461},   "D:\\Media\\Textures\\TestImage0022.bmp"},
    {{-20.9759,-129.2015,-212.9952},    "D:\\Media\\Textures\\TestImage0023.bmp"},
    {{142.1654,-129.2015,-159.9874},    "D:\\Media\\Textures\\TestImage0024.bmp"},
    {{196.0886,-129.2015,-85.7684},     "D:\\Media\\Textures\\TestImage0025.bmp"},
    {{241.1797,-39.0192,53.0078},       "D:\\Media\\Textures\\TestImage0026.bmp"},
    {{24.1152,-39.0192,245.7558},       "D:\\Media\\Textures\\TestImage0027.bmp"},
    {{-226.2757,-39.0192,98.8776},      "D:\\Media\\Textures\\TestImage0028.bmp"},
    {{-163.9613,-39.0192,-184.6461},    "D:\\Media\\Textures\\TestImage0029.bmp"},
    {{124.9421,-39.0192,-212.9952},     "D:\\Media\\Textures\\TestImage0030.bmp"},
    {{241.1798,-39.0192,-53.0078},      "D:\\Media\\Textures\\TestImage0031.bmp"},
    {{124.9421,-39.0192,212.9952},      "D:\\Media\\Textures\\TestImage0032.bmp"},
    {{-163.9613,-39.0192,184.6461},     "D:\\Media\\Textures\\TestImage0033.bmp"},
    {{-226.2757,-39.0192,-98.8777},     "D:\\Media\\Textures\\TestImage0034.bmp"},
    {{24.1152,-39.0192,-245.7559},      "D:\\Media\\Textures\\TestImage0035.bmp"},
    {{119.8765,-201.3555,87.0954},      "D:\\Media\\Textures\\TestImage0036.bmp"},
    {{-45.7887,-201.3555,140.9232},     "D:\\Media\\Textures\\TestImage0037.bmp"},
    {{-148.1754,-201.3555,-0.0000},     "D:\\Media\\Textures\\TestImage0038.bmp"},
    {{-45.7887,-201.3555,-140.9232},    "D:\\Media\\Textures\\TestImage0039.bmp"},
    {{119.8765,-201.3555,-87.0953},     "D:\\Media\\Textures\\TestImage0040.bmp"},
    {{199.3126,-42.4832,144.8091},      "D:\\Media\\Textures\\TestImage0041.bmp"},
    {{-76.1306,-42.4832,234.3059},      "D:\\Media\\Textures\\TestImage0042.bmp"},
    {{-246.3639,-42.4832,-0.0000},      "D:\\Media\\Textures\\TestImage0043.bmp"},
    {{-76.1306,-42.4833,-234.3060},     "D:\\Media\\Textures\\TestImage0044.bmp"},
    {{199.3126,-42.4833,-144.8090},     "D:\\Media\\Textures\\TestImage0045.bmp"},
};

const int g_NumSourceImages = sizeof(g_ImageList)/sizeof(g_ImageList[0]);




//-----------------------------------------------------------------------------
// .BMP header structs so we can save out the data.
//-----------------------------------------------------------------------------
#pragma pack(push,1)
typedef struct tagBITMAPFILEHEADER { 
  WORD    bfType; 
  DWORD   bfSize; 
  WORD    bfReserved1; 
  WORD    bfReserved2; 
  DWORD   bfOffBits; 
} BITMAPFILEHEADER, *PBITMAPFILEHEADER; 

typedef struct tagBITMAPINFOHEADER{
  DWORD  biSize; 
  LONG   biWidth; 
  LONG   biHeight; 
  WORD   biPlanes; 
  WORD   biBitCount; 
  DWORD  biCompression; 
  DWORD  biSizeImage; 
  LONG   biXPelsPerMeter; 
  LONG   biYPelsPerMeter; 
  DWORD  biClrUsed; 
  DWORD  biClrImportant; 
} BITMAPINFOHEADER, *PBITMAPINFOHEADER; 
#pragma pack(pop)



//-----------------------------------------------------------------------------
// Name: ComputePolynomialTextures()
// Desc: Compute the polynomial textures (and the color texture) from a set of 
//       sample textures.  Also saved out the textures, and the polynomail bias
//       factors.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::ComputePolynomialTextures()
{
    HRESULT hr;
    int width = 512, height = 512;

    LPDIRECT3DTEXTURE8 pSourceTextures[g_NumSourceImages];
    DWORD* pSrcBits[g_NumSourceImages];
    D3DXVECTOR3 vLightDir[g_NumSourceImages];

    // Read the image textures and lock them.
    for ( int i = 0; i < g_NumSourceImages; i++ )
    {
        vLightDir[i] = g_ImageList[i].vLightPos;
        vLightDir[i].z = -vLightDir[i].z;
        D3DXVec3Normalize( &vLightDir[i], &vLightDir[i] );

        hr = D3DXCreateTextureFromFileEx( m_pd3dDevice,
                                          g_ImageList[i].strFileName,
                                          D3DX_DEFAULT,
                                          D3DX_DEFAULT,
                                          1,
                                          0,
                                          D3DFMT_A8R8G8B8,
                                          0,
                                          D3DX_FILTER_NONE,
                                          D3DX_FILTER_NONE,
                                          0,
                                          NULL,
                                          NULL,
                                          pSourceTextures+i );

        D3DLOCKED_RECT lock;
        pSourceTextures[i]->LockRect( 0, &lock, 0, 0 );

        pSrcBits[i] = (DWORD*)lock.pBits;
    }

    assert( pSourceTextures[0] );

    D3DSURFACE_DESC Desc;
    pSourceTextures[0]->GetLevelDesc( 0, &Desc );

    width = Desc.Width;
    height = Desc.Height;

    // Create textures to hold the polynomial coefficients and the rgb values.
    hr = m_pd3dDevice->CreateTexture( width, height, 1, 0, D3DFMT_A8R8G8B8, 0, &m_pCoeff1Texture );
    hr = m_pd3dDevice->CreateTexture( width, height, 1, 0, D3DFMT_A8R8G8B8, 0, &m_pCoeff2Texture );
    hr = m_pd3dDevice->CreateTexture( width, height, 1, 0, D3DFMT_A8R8G8B8, 0, &m_pColorTexture );
    
    D3DSURFACE_DESC desc_color;
    m_pColorTexture->GetLevelDesc( 0, &desc_color );

    D3DLOCKED_RECT lock_color;
    m_pColorTexture->LockRect( 0, &lock_color, 0, 0 );

    DWORD* pBitsColor = (DWORD*)lock_color.pBits;

    // Allocate storage to hold polynomial coeff's.
    double* pCoeffs = new double[height*width*6];

    int m = g_NumSourceImages;
    int n = 6;

    double a[g_NumSourceImages][6];
    double u[g_NumSourceImages][6];
    double w[6];
    double vt[6][6];
    double b[g_NumSourceImages];
    double xx[6];

    // Keep track of minimum and maximum of each coeffient.
    double MinCoeff[6];
    double MaxCoeff[6];

    for ( int i = 0; i < 6; i++ )
    {
        MinCoeff[i] =  100000.0;
        MaxCoeff[i] = -100000.0;
    }

    // Solve a system of equations for each pixel.
    for ( int y = 0; y < height; y++ )
    {
        for ( int x = 0; x < width; x++ )
        {
            double luminance[g_NumSourceImages];
            double max_luminance = -1.0;

            for ( int i = 0; i < g_NumSourceImages; i++ )
            {
                double red = ((pSrcBits[i][y*width+x] >> 16) & 0xff) / 255.0;
                double green = ((pSrcBits[i][y*width+x] >> 8) & 0xff) / 255.0;
                double blue = (pSrcBits[i][y*width+x] & 0xff) / 255.0;

                luminance[i] = (0.299 * red + 0.587 * green + 0.144 * blue);

                if (luminance[i] > max_luminance)
                {
                    // Save the maximum luminance.
                    max_luminance = luminance[i];

                    // Use this color for the pixel.
                    int r1 = int(red * 255.0f);
                    int g1 = int(green * 255.0f);
                    int b1= int(blue * 255.0f);

                    if (r1 > 255) r1 = 255;
                    if (g1 > 255) g1 = 255;
                    if (b1 > 256) b1 = 255;

                    pBitsColor[y*width+x] = (r1 << 16) | (g1 << 8) | b1;
                }
            }

            // Form the system of equations ..
            for ( int i = 0; i < g_NumSourceImages; i++ )
            {
                a[i][0] = vLightDir[i].x * vLightDir[i].x;
                a[i][1] = vLightDir[i].z * vLightDir[i].z;
                a[i][2] = vLightDir[i].x * vLightDir[i].z;
                a[i][3] = vLightDir[i].x;
                a[i][4] = vLightDir[i].z;
                a[i][5] = 1.0f;

                // Compute luminance of pixel (normalized).
                if (max_luminance > 0.0)
                    b[i] = luminance[i] / max_luminance;
                else
                    b[i] = 0.0f;
            }

            // .. and solve it for best fit.
            #error "You need to supply your own SVD routines."
            //num_svd(&a[0][0], m, n, &u[0][0], w, &vt[0][0]);
            //num_svd_backsubst(m, n, &u[0][0], w, &vt[0][0], b, xx, 0.0);

            for ( int i = 0; i < 6; i++ )
            {
                pCoeffs[(y*width+x)*6+i] = xx[i];

                if (xx[i] < MinCoeff[i])
                    MinCoeff[i] = xx[i];

                if (xx[i] > MaxCoeff[i])
                    MaxCoeff[i] = xx[i];
            }
        }
    }

    // Unlock source textures.
    for ( int i = 0; i < g_NumSourceImages; i++ )
    {
        pSourceTextures[i]->UnlockRect( 0 );
    }

    // Determine scale and bias to use.
    double Scale[6];
    double Bias[6];

    for ( int i = 0; i < 6; i++ )
    {
        // Scale is fixed at 2.0 becuase that is what our vertex shader uses.
        Scale[i] = 2.0f;

        // Compute bias.
        Bias[i] = -MinCoeff[i] / Scale[i];

        assert( Bias[i] >= 0.0 && Bias[i] <= 1.0 );

        m_iBias[i] = (int)(Bias[i] * 255.0);
    }

    // Lock and fill the textures
    D3DSURFACE_DESC desc1;
    m_pCoeff1Texture->GetLevelDesc( 0, &desc1 );

    D3DLOCKED_RECT lock1;
    m_pCoeff1Texture->LockRect( 0, &lock1, 0, 0 );

    DWORD* pBits1 = (DWORD*)lock1.pBits;

    D3DSURFACE_DESC desc2;
    m_pCoeff2Texture->GetLevelDesc( 0, &desc2 );

    D3DLOCKED_RECT lock2;
    m_pCoeff2Texture->LockRect( 0, &lock2, 0, 0 );

    DWORD* pBits2 = (DWORD*)lock2.pBits;

    for ( int y = 0; y < height; y++ )
    {
        for ( int x = 0; x < width; x++ )
        {
            unsigned char buf[6];

            for ( int i = 0; i < 6; i++ )
            {
                double value = pCoeffs[(y*width+x)*6+i] / Scale[i] + Bias[i];

                assert( value >= 0.0 && value <= 1.0 );

                buf[i] = (unsigned char)(value * 255.0);
            }

            pBits1[y*width+x] = (buf[0] << 16) | (buf[1] << 8) | buf[2];
            pBits2[y*width+x] = (buf[3] << 16) | (buf[4] << 8) | buf[5];
        }
    }

    delete[] pCoeffs;


    // Save the computed textures.
    DWORD* pTempBits = new DWORD[width*height];

    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;

    bf.bfType = 'MB';
    bf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width * height * 4;
    bf.bfReserved1 = 0;
    bf.bfReserved2 = 0;
    bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = 0;
    bi.biSizeImage = width * height * 4;
    bi.biXPelsPerMeter = 2834; 
    bi.biYPelsPerMeter = 2834;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    FILE *fp;

    fp = fopen( "D:\\Media\\Textures\\MoonPoly1.BMP", "wb" );

    fwrite( &bf, sizeof(BITMAPFILEHEADER), 1, fp );
    fwrite( &bi, sizeof(BITMAPINFOHEADER), 1, fp );
    XGUnswizzleRect( pBits1, width, height, NULL, pTempBits, 0, NULL, sizeof(DWORD) );
    fwrite( pTempBits, width*height, 4, fp );

    fclose(fp);

    fp = fopen( "D:\\Media\\Textures\\MoonPoly2.BMP", "wb" );

    fwrite( &bf, sizeof(BITMAPFILEHEADER), 1, fp );
    fwrite( &bi, sizeof(BITMAPINFOHEADER), 1, fp );
    XGUnswizzleRect( pBits2, width, height, NULL, pTempBits, 0, NULL, sizeof(DWORD) );
    fwrite( pTempBits, width*height, 4, fp );

    fclose(fp);

    fp = fopen( "D:\\Media\\Textures\\MoonColor.BMP", "wb" );

    fwrite( &bf, sizeof(BITMAPFILEHEADER), 1, fp );
    fwrite( &bi, sizeof(BITMAPINFOHEADER), 1, fp );
    XGUnswizzleRect( pBitsColor, width, height, NULL, pTempBits, 0, NULL, sizeof(DWORD) );
    fwrite( pTempBits, width*height, 4, fp );

    fclose(fp);

    fp = fopen( "D:\\Media\\MoonCoeffs.txt", "w" );

    for ( int i = 0; i < 6; i++ )
    {
        fprintf(fp, "%d\n", m_iBias[i]);
    }

    fclose(fp);

    delete[] pTempBits;

    
    // Unlock the textures
    m_pCoeff1Texture->UnlockRect( 0 );
    m_pCoeff2Texture->UnlockRect( 0 );
    m_pColorTexture->UnlockRect( 0 );

    return S_OK;
}

#endif
