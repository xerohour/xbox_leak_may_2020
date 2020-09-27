#include <xtl.h>
#include <tchar.h>
#include <xgraphics.h>

// model types
#define MT_QUAD     0
#define MT_CYLINDER 1
#define MT_MAX      2

// rendering types
#define RT_TEXTURE  0
#define RT_LIT      1
#define RT_SHADER1  2
#define RT_SHADER2  3
#define RT_MAX      4


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D8             g_pD3D       = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE8       g_pd3dDevice = NULL; // Our rendering device

UINT                    g_SelectedModel = MT_CYLINDER;  // displayed model
BOOL                    g_bRSelectUp = TRUE; // model select button release
UINT                    g_SelectedRenderer = RT_SHADER1;  // render type
BOOL                    g_bLSelectUp = TRUE; // renderer select button release
BOOL                    g_bDumpImage = FALSE; // dump an image to disk
BOOL                    g_bDumpImageUp = TRUE; // button is released
BOOL                    g_bDoParameterTest = FALSE; // do bad parameter test

LPDIRECT3DVERTEXBUFFER8 g_pCylinderVB = NULL; // Buffer for cylinder vertices
LPDIRECT3DVERTEXBUFFER8 g_pQuadVB = NULL;     // Buffer for quad vertices
LPDIRECT3DINDEXBUFFER8  g_pIndicesIB = NULL;  // index buffer for quad
LPDIRECT3DTEXTURE8      g_pPShaderTexture0 = NULL;
LPDIRECT3DTEXTURE8      g_pPShaderTexture1 = NULL;
LPDIRECT3DTEXTURE8      g_pPShaderTexture2 = NULL;

DWORD                   g_PShader1 = 0; // handle for pixel shader 1
DWORD                   g_PShader2 = 0; // handle for pixel shader 2

DWORD                   g_VertexShader = 0; // handle for vertex shader

struct CUSTOMVERTEX
{
    D3DXVECTOR3 position; // The position
    D3DXVECTOR3 normal;   // The vertex normals
    D3DCOLOR    color;    // The color
    FLOAT       tu0, tv0;   // Texture 0 coordinates
    FLOAT       tu1, tv1;   // Texture 1 coordinates
    FLOAT       tu2, tv2;   // Texture 2 coordinates
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEX2|D3DFVF_TEX3)

HANDLE  g_hDevice = 0;      // handle of the gamepad
D3DXMATRIX matObject;       // transform matrix for the object
D3DXMATRIX matLight;        // transform matrix for the light
BOOL bWireFrame = FALSE;    // wireframe flag
FLOAT EyeScale = 1.0;       // scale of viewing distance

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D()
{
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice.
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.BackBufferWidth        = 640;
    d3dpp.BackBufferHeight       = 480;
    d3dpp.BackBufferFormat       = D3DFMT_A8R8G8B8;
    d3dpp.BackBufferCount        = 1;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    d3dpp.MultiSampleType        = D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN;

    // Create the Direct3D device.
    if( FAILED( g_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
        return E_FAIL;

    // Turn off culling
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn off D3D lighting
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    
    // Turn on the zbuffer
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    // initialize globals
    D3DXMatrixIdentity(&matObject);
    D3DXMatrixIdentity(&matLight);
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Creates the scene geometry
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
    // Create pixel shader 1

/*
    D3DPIXELSHADERDEF psd;
    ZeroMemory( &psd, sizeof(psd) );
 
    psd.PSConstant0[0] = 0xffff00ff;
    psd.PSFinalCombinerConstant0 = 0xffff00ff;
    psd.PSCombinerCount = PS_COMBINERCOUNT(
        2, 
        PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
    psd.PSTextureModes = PS_TEXTUREMODES(
        PS_TEXTUREMODES_PROJECT2D,
        PS_TEXTUREMODES_PROJECT2D,
        PS_TEXTUREMODES_PROJECT2D,
        PS_TEXTUREMODES_NONE);
    
    psd.PSRGBInputs[0] = PS_COMBINERINPUTS(
        PS_REGISTER_ZERO | PS_CHANNEL_RGB,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB,
        PS_REGISTER_ZERO,
        PS_REGISTER_ZERO);
    psd.PSAlphaInputs[0] = PS_COMBINERINPUTS(
        PS_REGISTER_T2 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_BLUE,
        PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_INVERT,
        PS_REGISTER_ZERO,
        PS_REGISTER_ZERO);
    psd.PSRGBOutputs[0] = PS_COMBINEROUTPUTS(
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY);
    psd.PSAlphaOutputs[0] = PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY);
    
    psd.PSRGBInputs[1] = PS_COMBINERINPUTS(
        PS_REGISTER_T0 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,
        PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_INVERT | PS_CHANNEL_RGB,
        PS_REGISTER_T1 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,
        PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_INVERT | PS_CHANNEL_RGB);
    psd.PSAlphaInputs[1] = PS_COMBINERINPUTS(
        PS_REGISTER_ZERO,
        PS_REGISTER_ZERO,
        PS_REGISTER_ZERO,
        PS_REGISTER_ZERO);
    psd.PSRGBOutputs[1] = PS_COMBINEROUTPUTS(
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_REGISTER_R0 | PS_CHANNEL_RGB,
        PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_MUX);
    psd.PSAlphaOutputs[1] = PS_COMBINEROUTPUTS(
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY);
    psd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(
        PS_REGISTER_ZERO,
        PS_REGISTER_R0 | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_R0 | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO);
    psd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(
        PS_REGISTER_ZERO,
        PS_REGISTER_ZERO,
        PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_INVERT,    // opaque output
        0);
*/

D3DPIXELSHADERDEF psd;
    ZeroMemory( &psd, sizeof(psd) );
psd.PSCombinerCount=PS_COMBINERCOUNT(
    2,
    PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
psd.PSTextureModes=PS_TEXTUREMODES(
    PS_TEXTUREMODES_PROJECT2D,
    PS_TEXTUREMODES_PROJECT2D,
    PS_TEXTUREMODES_PROJECT2D,
    PS_TEXTUREMODES_NONE);
psd.PSInputTexture=PS_INPUTTEXTURE(0,0,0,0);
psd.PSDotMapping=PS_DOTMAPPING(
    0,
    PS_DOTMAPPING_ZERO_TO_ONE,
    PS_DOTMAPPING_ZERO_TO_ONE,
    PS_DOTMAPPING_ZERO_TO_ONE);
psd.PSCompareMode=PS_COMPAREMODE(
    PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
    PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
    PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
    PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT);

//------------- Stage 0 -------------
psd.PSRGBInputs[0]=PS_COMBINERINPUTS(
    PS_REGISTER_T2 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_EXPAND_NEGATE,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
psd.PSAlphaInputs[0]=PS_COMBINERINPUTS(
    PS_REGISTER_T2 | PS_CHANNEL_BLUE | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_EXPAND_NEGATE,
    PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
psd.PSRGBOutputs[0]=PS_COMBINEROUTPUTS(
    PS_REGISTER_R0,
    PS_REGISTER_DISCARD,
    PS_REGISTER_DISCARD,
    PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
psd.PSAlphaOutputs[0]=PS_COMBINEROUTPUTS(
    PS_REGISTER_R0,
    PS_REGISTER_DISCARD,
    PS_REGISTER_DISCARD,
    PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
psd.PSConstant0[0]=00000000;
psd.PSConstant1[0]=00000000;

//------------- Stage 1 -------------
psd.PSRGBInputs[1]=PS_COMBINERINPUTS(
    PS_REGISTER_T0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_EXPAND_NEGATE,
    PS_REGISTER_T1 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_EXPAND_NEGATE);
psd.PSAlphaInputs[1]=PS_COMBINERINPUTS(
    PS_REGISTER_T0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_EXPAND_NEGATE,
    PS_REGISTER_T1 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_EXPAND_NEGATE);
psd.PSRGBOutputs[1]=PS_COMBINEROUTPUTS(
    PS_REGISTER_DISCARD,
    PS_REGISTER_DISCARD,
    PS_REGISTER_R0,
    PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_MUX);
psd.PSAlphaOutputs[1]=PS_COMBINEROUTPUTS(
    PS_REGISTER_DISCARD,
    PS_REGISTER_DISCARD,
    PS_REGISTER_R0,
    PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_MUX);
psd.PSConstant0[1]=00000000;
psd.PSConstant1[1]=00000000;

//------------- FinalCombiner -------------
psd.PSFinalCombinerInputsABCD=PS_COMBINERINPUTS(
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_R0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_R0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
psd.PSFinalCombinerInputsEFG=PS_COMBINERINPUTS(
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
    PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
    0 | 0 | 0);

psd.PSC0Mapping = 00000000;
psd.PSC1Mapping = 00000000;

    if( FAILED( g_pd3dDevice->CreatePixelShader(&psd, &g_PShader1)))
    {
        OutputDebugString( L"Could not create pixel shader 1\n");
        return E_FAIL;
    }

    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD BytesRead;

    hFile = CreateFile("D:\\media\\pshader.xpu",
                       GENERIC_READ, 
                       0, 
                       NULL, 
                       OPEN_EXISTING, 
                       0, 
                       NULL);
    
    if(INVALID_HANDLE_VALUE == hFile)
    {
        WCHAR buf2[256];
        swprintf(buf2, L"Unable to open file:  error %x\n", GetLastError());
        OutputDebugString(buf2);
        return E_FAIL;
    }
    
    D3DPIXELSHADERDEF_FILE psdf;
    ReadFile(hFile, &psdf, sizeof(D3DPIXELSHADERDEF_FILE), &BytesRead, NULL);
    CloseHandle(hFile); // finished reading shader

    if(BytesRead != sizeof(D3DPIXELSHADERDEF_FILE) || psdf.FileID != D3DPIXELSHADERDEF_FILE_ID)
    {
        OutputDebugString( L"Invalid pixel shader file\n");
        return E_FAIL;
    }

    if( FAILED( g_pd3dDevice->CreatePixelShader(&(psdf.Psd), &g_PShader2)))
    {
        OutputDebugString( L"Could not create pixel shader 2\n");
        return E_FAIL;
    }


    hFile = CreateFile("D:\\media\\pshader.xvu",
                       GENERIC_READ, 
                       0, 
                       NULL, 
                       OPEN_EXISTING, 
                       0, 
                       NULL);
    
    if(INVALID_HANDLE_VALUE == hFile)
    {
        WCHAR buf2[256];
        swprintf(buf2, L"Unable to open file:  error %x\n", GetLastError());
        OutputDebugString(buf2);
        return E_FAIL;
    }
    
    // Allocate memory to read the vertex shader file
    DWORD dwSize = GetFileSize( hFile, NULL );
    BYTE* pData  = new BYTE[dwSize+4];
    if( NULL == pData )
        return E_FAIL;
    ZeroMemory( pData, dwSize+4 );

    // Read the pre-compiled vertex shader microcode
    ReadFile( hFile, pData, dwSize, &BytesRead, NULL );
    CloseHandle(hFile); // finished reading shader

    DWORD dwVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),                // This data comes from stream 0
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),   // v0 = Position of first mesh
        D3DVSD_REG( 1, D3DVSDT_FLOAT3 ),   // v1 = Normal
        D3DVSD_REG( 2, D3DVSDT_D3DCOLOR ), // v2 = Diffuse color
        D3DVSD_REG( 3, D3DVSDT_FLOAT2 ),   // v3 = Tex coord 0
        D3DVSD_REG( 4, D3DVSDT_FLOAT2 ),   // v4 = Tex coord 1
        D3DVSD_REG( 5, D3DVSDT_FLOAT2 ),   // v5 = Tex coord 2
        D3DVSD_END()
    };

    // Create the vertex shader
    if(FAILED(g_pd3dDevice->CreateVertexShader( dwVertexDecl, (const DWORD*)pData,
                                                &g_VertexShader, 0 )))
    {
        OutputDebugString( L"Could not create vertex shader\n");
        return E_FAIL;
    }

    
    
    // Use D3DX to create a texture from a file
    if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, "D:\\MEDIA\\pshader0.bmp",
                                           &g_pPShaderTexture0 ) ) )
    {
        OutputDebugString( L"Could not load texture pshader0.bmp\n" );
        return E_FAIL;
    }
    if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, "D:\\MEDIA\\pshader1.bmp",
                                           &g_pPShaderTexture1 ) ) )
    {
        OutputDebugString( L"Could not load texture pshader1.bmp\n" );
        return E_FAIL;
    }
    if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, "D:\\MEDIA\\pshader2.bmp",
                                           &g_pPShaderTexture2 ) ) )
    {
        OutputDebugString( L"Could not load texture pshader2.bmp\n" );
        return E_FAIL;
    }

    
    /* Initialize the Quad */

    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 5*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &g_pQuadVB ) ) )
        return E_FAIL;

    CUSTOMVERTEX *pVertices;

    if( FAILED( g_pQuadVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
        return E_FAIL;

    pVertices[0].position = D3DXVECTOR3(  0.0f, 0.0f, 0.0f );
    pVertices[0].color    = 0xffffff00;
    pVertices[0].tu0      = 0.5;
    pVertices[0].tv0      = 0.5;
    pVertices[0].tu1      = 0.5;
    pVertices[0].tv1      = 0.5;
    pVertices[0].tu2      = 0.5;
    pVertices[0].tv2      = 0.5;
    pVertices[0].normal   = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );

    pVertices[4].position = D3DXVECTOR3(  1.0f, -1.0f, 0.0f );
    pVertices[4].color    = 0xff00ff00;
    pVertices[4].tu0      = 1.0;
    pVertices[4].tv0      = 1.0;
    pVertices[4].tu1      = 1.0;
    pVertices[4].tv1      = 1.0;
    pVertices[4].tu2      = 1.0;
    pVertices[4].tv2      = 1.0;
    pVertices[4].normal   = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );

    pVertices[1].position = D3DXVECTOR3(  1.0f,  1.0f, 0.0f );
    pVertices[1].color    = 0xff0000ff;
    pVertices[1].tu0      = 1.0;
    pVertices[1].tv0      = 0.0;
    pVertices[1].tu1      = 1.0;
    pVertices[1].tv1      = 0.0;
    pVertices[1].tu2      = 1.0;
    pVertices[1].tv2      = 0.0;
    pVertices[1].normal   = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );

    pVertices[3].position = D3DXVECTOR3( -1.0f, -1.0f, 0.0f );
    pVertices[3].color    = 0xffff0000;
    pVertices[3].tu0      = 0.0;
    pVertices[3].tv0      = 1.0;
    pVertices[3].tu1      = 0.0;
    pVertices[3].tv1      = 1.0;
    pVertices[3].tu2      = 0.0;
    pVertices[3].tv2      = 1.0;
    pVertices[3].normal   = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );

    pVertices[2].position = D3DXVECTOR3( -1.0f,  1.0f, 0.0f );
    pVertices[2].color    = 0xffffffff;
    pVertices[2].tu0      = 0.0;
    pVertices[2].tv0      = 0.0;
    pVertices[2].tu1      = 0.0;
    pVertices[2].tv1      = 0.0;
    pVertices[2].tu2      = 0.0;
    pVertices[2].tv2      = 0.0;
    pVertices[2].normal   = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
    
    g_pQuadVB->Unlock();


    // Create the index buffer and fill it
    g_pd3dDevice->CreateIndexBuffer( 12*sizeof(WORD),
                                     D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
                                     D3DPOOL_MANAGED, &g_pIndicesIB );
    WORD* pIndices;
    g_pIndicesIB->Lock( 0, 0, (BYTE**)&pIndices, 0 );
    pIndices[ 0] = 0;
    pIndices[ 1] = 1;
    pIndices[ 2] = 2;
    pIndices[ 3] = 0;
    pIndices[ 4] = 2;
    pIndices[ 5] = 3;
    pIndices[ 6] = 0;
    pIndices[ 7] = 3;
    pIndices[ 8] = 4;
    pIndices[ 9] = 0;
    pIndices[10] = 4;
    pIndices[11] = 1;
    g_pIndicesIB->Unlock();

    /* Initialize the Cylinder */
    // Create the vertex buffer.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 50*2*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &g_pCylinderVB ) ) )
        return E_FAIL;

    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    if( FAILED( g_pCylinderVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
        return E_FAIL;
    for( DWORD i=0; i<50; i++ )
    {
        FLOAT theta = (2*D3DX_PI*i)/(50-1);

        pVertices[2*i+0].position = D3DXVECTOR3( sinf(theta),-1.2f, cosf(theta) );
        pVertices[2*i+0].color    = 0xffffffff;
        pVertices[2*i+0].tu0      = ((FLOAT)i*2.0f)/(50-1);
        pVertices[2*i+0].tv0      = 1.0f;
        pVertices[2*i+0].tu1      = ((FLOAT)i*2.0f)/(50-1);
        pVertices[2*i+0].tv1      = 1.0f;
        pVertices[2*i+0].tu2      = ((FLOAT)i*2.0f)/(50-1);
        pVertices[2*i+0].tv2      = 1.0f;
        pVertices[2*i+0].normal   = D3DXVECTOR3( sinf(theta), 0.0f, cosf(theta) );

        pVertices[2*i+1].position = D3DXVECTOR3( sinf(theta), 1.2f, cosf(theta) );
        pVertices[2*i+1].color    = 0xff808080;
        pVertices[2*i+1].tu0      = ((FLOAT)i*2.0f)/(50-1);
        pVertices[2*i+1].tv0      = 0.0f;
        pVertices[2*i+1].tu1      = ((FLOAT)i*2.0f)/(50-1);
        pVertices[2*i+1].tv1      = 0.0f;
        pVertices[2*i+1].tu2      = ((FLOAT)i*2.0f)/(50-1);
        pVertices[2*i+1].tv2      = 0.0f;
        pVertices[2*i+1].normal   = D3DXVECTOR3( sinf(theta), 0.0f, cosf(theta) );
    }
    g_pCylinderVB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices(BOOL bIdent)
{
    CONST D3DXMATRIX *pmatWorld;
    // set object transform
    if(bIdent)
    {
        D3DXMATRIX matIdentity;
        D3DXMatrixIdentity(&matIdentity);
        pmatWorld = &matIdentity;
    }
    else
        pmatWorld = &matObject;
        
    g_pd3dDevice->SetTransform( D3DTS_WORLD, pmatWorld );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f*EyeScale, 3.0f*EyeScale,-5.0f*EyeScale ),
                                  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set the transform matrices
    D3DXMATRIX matW, matWV, matWVP;
    D3DXMatrixMultiply( &matWV,  pmatWorld, &matView );
    D3DXMatrixMultiply( &matWVP, &matWV, &matProj );
    D3DXMatrixTranspose( &matWVP, &matWVP );
    g_pd3dDevice->SetVertexShaderConstant( 10, &matWVP, 4 );
    
    float cnst1[4] = {1.0, 1.0, 1.0, 1.0};
    g_pd3dDevice->SetVertexShaderConstant( 1, cnst1, 1);
    float cnst2[4] = {2.0, 2.0, 2.0, 2.0};
    g_pd3dDevice->SetVertexShaderConstant( 2, cnst2, 1);
}




//-----------------------------------------------------------------------------
// Name: SetupLights()
// Desc: Sets up the lights for the scene.
//-----------------------------------------------------------------------------
VOID SetupLights()
{
    // Set up a material. The material here just has the diffuse and ambient
    // colors set to yellow. Note that only one material can be used at a time.
    D3DMATERIAL8 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    g_pd3dDevice->SetMaterial( &mtrl );

    // Direction vector for the light
	D3DXVECTOR3 vDirection( 0.7f, 0.14f, 0.7f );
    D3DXVec3TransformNormal( &vDirection, &vDirection, &matLight );

	// The light structure
    D3DLIGHT8 light;
    ZeroMemory( &light, sizeof(D3DLIGHT8) );
    light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r  = 1.0f;
    light.Diffuse.g  = 1.0f;
    light.Diffuse.b  = 1.0f;
	light.Direction  = vDirection;
    light.Range      = 1000.0f;
    g_pd3dDevice->SetLight( 0, &light );
    g_pd3dDevice->LightEnable( 0, TRUE );
    
	// When using lights, we must turn lighting on
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Finally, turn on some ambient light.
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00303030 );
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    // Clear the backbuffer and the zbuffer
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB(20,20,30), 1.0f, 0 );

    // Begin the scene
    g_pd3dDevice->BeginScene();

    // set default state
    // disable any previous textures
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
    // disable lighting
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    // remove any pixel shaders
    g_pd3dDevice->SetPixelShader( NULL );
    
    // Setup the world, view, and projection matrices
    SetupMatrices(FALSE);

    if(g_bDoParameterTest)
    {
        // set pixel shader renderstates with random values and make 
        // sure that the chip doesn't choke
        OutputDebugString(L"Beginning parameter test\n");
        g_pd3dDevice->SetPixelShader( g_PShader1 );
        while(1)
        {
            static DWORD mode=0;
            static DWORD offset=0;
            static DWORD value=1;
            switch(mode)
            {
            case 2:
                offset = rand() % 57; // 57 dwords in pixel shader definition
                value = ((rand() >> 8) & 0xff) |
                        (((rand() >> 8) & 0xff) <<  8) |
                        (((rand() >> 8) & 0xff) << 16) |
                        (((rand() >> 8) & 0xff) << 24);
                    break;

            }
            WCHAR buf[256];
            swprintf(buf, L"RS %d: %08x\n", offset, value);
            OutputDebugString(buf);
            g_pd3dDevice->SetRenderState((D3DRENDERSTATETYPE)(D3DRS_PSALPHAINPUTS0+offset), value);

            switch(mode)
            {
            case 0:
                offset++;
                if(offset == 57)
                {
                    offset = 0;
                    if(value == 0)
                    {
                        mode++;
                        value = 1;
                    }
                    else
                        value = value << 1;
                }
                break;
            case 1:
                offset++;
                if(offset == 57)
                {
                    offset = 0;
                    if(value == 0x0)
                    {
                        mode++;
                        value = 1;
                    }
                    else
                    {
                        if(value & 0x80000000)
                            value = (value << 1);
                        else
                            value = (value << 1) | 0x1;
                    }
                }
                break;
            }
        }
    }
    static FLOAT fXoff = 0.0;
    static FLOAT fYoff = 0.0;
    D3DXMATRIX matTex;
    
    switch(g_SelectedRenderer)
    {
    case RT_TEXTURE:
        // Setup our texture. Using textures introduces the texture stage states,
        // which govern how textures get blended together (in the case of multiple
        // textures) and lighting information. In this case, we are modulating
        // (blending) our texture with the diffuse color of the vertices.
        g_pd3dDevice->SetTexture( 0, g_pPShaderTexture0 );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
        
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
        break;
    
    case RT_LIT:
        // Setup the lights
        SetupLights();
        break;

    case RT_SHADER1:
        g_pd3dDevice->SetTexture( 0, g_pPShaderTexture0 );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetTexture( 1, g_pPShaderTexture1 );
        g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetTexture( 2, g_pPShaderTexture2 );
        g_pd3dDevice->SetTextureStageState( 2, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetTextureStageState( 2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
        D3DXMatrixRotationYawPitchRoll(&matTex, fXoff, fYoff, 0.0);
        g_pd3dDevice->SetTransform( D3DTS_TEXTURE2, &matTex );
        fXoff += 0.01f;
        fYoff += 0.005f;

        // install pixel shader
        g_pd3dDevice->SetPixelShader( g_PShader1 );
        break;

    case RT_SHADER2:
        static FLOAT color[] = {0.0, 0.0, 0.0, 1.0};
        g_pd3dDevice->SetTexture( 0, g_pPShaderTexture0 );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetTexture( 1, g_pPShaderTexture1 );
        g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetTexture( 2, g_pPShaderTexture2 );
        g_pd3dDevice->SetTextureStageState( 2, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetTextureStageState( 2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
        D3DXMatrixRotationYawPitchRoll(&matTex, fXoff, fYoff, 0.0);
        g_pd3dDevice->SetTransform( D3DTS_TEXTURE2, &matTex );
        fXoff += 0.01f;
        fYoff += 0.005f;

        // install pixel shader
        g_pd3dDevice->SetPixelShader( g_PShader2 );
        //g_pd3dDevice->SetPixelShaderConstant(0, color, 1);
        color[0] += 0.01F;
        color[1] += 0.037F;
        color[2] += 0.045F;
        if(color[0] > 1.0) color[0] -= 1.0;
        if(color[1] > 1.0) color[1] -= 1.0;
        if(color[2] > 1.0) color[2] -= 1.0;
        break;

    }
    
    switch(g_SelectedModel)
    {
    case MT_CYLINDER:
        // Render the cylinder
        g_pd3dDevice->SetStreamSource( 0, g_pCylinderVB, sizeof(CUSTOMVERTEX) );
//        g_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
        g_pd3dDevice->SetVertexShader( g_VertexShader );
        g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*50-2 );
        break;

    case MT_QUAD:
        // Render the quad
        g_pd3dDevice->SetStreamSource( 0, g_pQuadVB, sizeof(CUSTOMVERTEX) );
        g_pd3dDevice->SetIndices( g_pIndicesIB, 0 );
//        g_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
        g_pd3dDevice->SetVertexShader( g_VertexShader );
        g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 5, 0, 4 );
        break;
    }

    // End the scene
    g_pd3dDevice->EndScene();

    if(g_bDumpImage)
    {
        static char filename[]="D:\\media\\Image000.bmp";
        IDirect3DSurface8 *pBackBuffer;
        g_pd3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
        XGWriteSurfaceToFile(pBackBuffer, filename);
        pBackBuffer->Release();
        // increment filename
        if(filename[16] == '9')
        {
            filename[16] = '0';
            if(filename[15] == '9')
            {
                filename[15] = '0';
                if(filename[14] == '9')
                {
                    filename[14] = '0';
                }
                else
                    filename[14]++;
            }
            else
                filename[15]++;
        }
        else
            filename[16]++;

        g_bDumpImage = FALSE;   // only a single write per button push
    }

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}



void GetInput(void)
{
    DWORD dwInsertions, dwRemovals;
    
    //
    //  Check to see if a port 0 gamepad has come or gone.
    //
    if(XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals))
    {
        if(dwRemovals&XDEVICE_PORT0_MASK)
        {
            if(g_hDevice)
            {
                XInputClose(g_hDevice);
                g_hDevice = NULL;
            }
        }
        if(dwInsertions&XDEVICE_PORT0_MASK)
        {
            g_hDevice = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL);
        }
    }

    // If we have a device, get its state.
    if (g_hDevice)
    {
        XINPUT_STATE InputState;

        XInputGetState(g_hDevice, &InputState);

        FLOAT dxRot, dyRot, dzRot;
        D3DXMATRIX matTmp;
		
        // calculate object transform
        static FLOAT objXRot=0;
        static FLOAT objYRot=0;
        static FLOAT objZRot=0;
        
        if(InputState.Gamepad.wButtons & XINPUT_GAMEPAD_START)
        {
            // reset object position
            objXRot = objYRot = objZRot = 0.0;
        }
        else
        {
            if(fabs((FLOAT)InputState.Gamepad.sThumbLY) > 10000.0)
                dxRot = (FLOAT)InputState.Gamepad.sThumbLY/(float)32768.0;
            else
                dxRot = 0.0;
            if(InputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
                dyRot =  0.75;
            else if(InputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
                dyRot = -0.75;
            else
                dyRot = 0.0;
            if(fabs((FLOAT)InputState.Gamepad.sThumbLX) > 10000.0)
                dzRot = (FLOAT)InputState.Gamepad.sThumbLX/(float)32768.0;
            else
                dzRot = 0.0;
            objXRot += dxRot*3.1415926F/75.0F;  // these scaling factors can be changed 
            objYRot += dyRot*3.1415926F/75.0F;  // these scaling factors can be changed 
            objZRot -= dzRot*3.1415926F/75.0F;  // these scaling factors can be changed 
        }

        D3DXMatrixRotationX(&matObject, objXRot);   // start with X rotation
        D3DXMatrixRotationY(&matTmp, objYRot);
        D3DXMatrixMultiply(&matObject, &matTmp, &matObject);    // include Y rotation
        D3DXMatrixRotationZ(&matTmp, objZRot);
        D3DXMatrixMultiply(&matObject, &matTmp, &matObject);    // include Z rotation
    
        // calculate light transform
        static FLOAT lightXRot=0;
        static FLOAT lightYRot=0;
        static FLOAT lightZRot=0;
        
        if(InputState.Gamepad.wButtons & XINPUT_GAMEPAD_START)
        {
            // reset light position
            lightXRot = lightYRot = lightZRot = 0.0;
        }
        else
        {
            dxRot = (FLOAT)InputState.Gamepad.sThumbRY/(float)32768.0;
            dyRot = 0.0;
            dzRot = (FLOAT)InputState.Gamepad.sThumbRX/(float)32768.0;
            lightXRot += dxRot*3.1415926F/75.0F;  // these scaling factors can be changed 
            lightYRot += dyRot*3.1415926F/75.0F;  // these scaling factors can be changed 
            lightZRot -= dzRot*3.1415926F/75.0F;  // these scaling factors can be changed 
        }

        D3DXMatrixRotationX(&matLight, lightXRot);   // start with X rotation
        D3DXMatrixRotationY(&matTmp, lightYRot);
        D3DXMatrixMultiply(&matLight, &matTmp, &matLight);    // include Y rotation
        D3DXMatrixRotationZ(&matTmp, lightZRot);
        D3DXMatrixMultiply(&matLight, &matTmp, &matLight);    // include Z rotation

        // check for eye vector scale
        if(InputState.Gamepad.wButtons & XINPUT_GAMEPAD_START)
            EyeScale = 1.0f;
        else if(InputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
            EyeScale = EyeScale/1.01f;
        else if(InputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
            EyeScale *= 1.01f;

        // check for wireframe toggle
        if(InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] != 0)
            g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
        else
            g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

        // check to see if we need to change models next frame
        if(InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] != 0)
        {
            if(g_bRSelectUp) // button was previously up
            {
                g_bRSelectUp = FALSE;
                g_SelectedModel++;
                if(g_SelectedModel == MT_MAX)
                    g_SelectedModel = 0;
            }
        }
        else
            g_bRSelectUp = TRUE;

        // check to see if we need to change rendering type next frame
        if(InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] != 0)
        {
            if(g_bLSelectUp) // button was previously up
            {
                g_bLSelectUp = FALSE;
                g_SelectedRenderer++;
                if(g_SelectedRenderer == RT_MAX)
                    g_SelectedRenderer = 0;
            }
        }
        else
            g_bLSelectUp = TRUE;

        // check to see if we need to dump an image
        if((InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] != 0) && g_bDumpImageUp)
        {
            g_bDumpImage = TRUE;
            g_bDumpImageUp = FALSE;
        }
        else if(InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] == 0)
            g_bDumpImageUp = TRUE;

        // check to see if we need to go into parameter test mode
        if(InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] != 0)
            g_bDoParameterTest = TRUE;

    }
}

//-----------------------------------------------------------------------------
// Name: main()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
void __cdecl main()
{
    OutputDebugString( L"### Beginning PShader.xbe ###\n" );
    

/*
    _asm nop;
    _asm int 3;
    _asm nop;
*/
    //Initialize core peripheral port support
    XInitDevices(0,NULL);

    // Initialize Direct3D
    if( FAILED( InitD3D() ) )
		return;

    // Initialize the gamepad
    if(XDEVICE_PORT0_MASK & XGetDevices(XDEVICE_TYPE_GAMEPAD))
    {
        g_hDevice = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL);
    }

    // Create the geometry
    if( FAILED( InitGeometry() ) )
		return;

	// Enter the render loop
    while( TRUE )
    {
        GetInput();
        Render();
        Sleep(10);
    }
}



