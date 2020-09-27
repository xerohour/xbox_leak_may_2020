//-----------------------------------------------------------------------------
// File: MatrixPaletteSkinning.cpp
//
// Desc: This sample demonstrates how to perform matrix palette skinning on
//       the Xbox, using vertex shaders
//
// Hist: 11.9.01 - New for December XDK
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <xgraphics.h>

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Rotate camera" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Zoom in/out" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Add/remove\nbones" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Next\nAnimation" },
};

#define NUM_HELP_CALLOUTS ( sizeof( g_HelpCallouts ) / sizeof( g_HelpCallouts[0] ) )



// The following header file is generated from "Resource.rdf" file using the
// Bundler tool. In addition to the header, the tool outputs a binary file
// (Resource.xpr) which contains compiled (i.e. bundled) resources and is
// loaded at runtime using the CXBPackedResource class.
#include "Resource.h"



//-----------------------------------------------------------------------------
// Map from ASCII names for the resources used by the app to offsets.
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "M0CM0.BMP", Resource_M0CM0_OFFSET },
    { "M1CM0.BMP", Resource_M1CM0_OFFSET },
    { "M2CM0.BMP", Resource_M2CM0_OFFSET },
    { NULL, 0 },
};



//-----------------------------------------------------------------------------
// Constants for how many bones and what constants register they use.
//-----------------------------------------------------------------------------
const int MIN_BONES = 5;
const int MAX_BONES = 30;
const int BONE_REGISTER_BASE = 5;

typedef enum
{
    ANIMATE_SINE = 0,
    ANIMATE_CIRCLE,
    ANIMATE_MAX,
} ANIMATE_MODE;



//-----------------------------------------------------------------------------
// Structure for per-vertex bone information.
//-----------------------------------------------------------------------------
#pragma pack(push,1)
struct BoneIndicesAndWeights
{
    short Indices[3];   // Index of the constant registers holding the bone matrix.
    float Weights[3];   // Weight for the corresponding bone matrix.
};
#pragma pack(pop)



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

    void    SetupBones( XBMESH_DATA& MeshData );        // Sets up the bones
    HRESULT CreateMeshWeights( XBMESH_DATA& MeshData ); // Sets up weights
    void    UpdateBoneTransforms();                     // Animates bones

    CXBFont     m_Font;             // Font object
    CXBHelp     m_Help;             // Help object
    BOOL        m_bDrawHelp;        // TRUE to draw help screen

    // Information for each bone.
    INT                     m_iNumberOfBones;
    float                   m_fBoneInfluenceRadius;
    D3DXVECTOR3             m_vBoneCenter[MAX_BONES];
    D3DXMATRIX              m_matBoneTransforms[MAX_BONES];

    CXBPackedResource       m_xprResource;      // Snake Textures
    CXBMesh                 m_Snake;            // Snake model
    DWORD                   m_dwDeformShader;   // Vertex Shader handle
    ANIMATE_MODE            m_Mode;             // Animation mode

    D3DXVECTOR3             m_vViewAngle;       // View angle
    D3DXVECTOR3             m_vCameraPos;       // Camera position

    LPDIRECT3DVERTEXBUFFER8 m_pBoneVB;          // Vertex buffer w/ bones
    LPDIRECT3DVERTEXBUFFER8 m_pBasisVB;         // Vertex buffer w/ basis vectors

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
    m_iNumberOfBones = 20;
}



//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      Resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the snake.
    if( FAILED( m_Snake.Create( m_pd3dDevice, "Models\\Snake.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create vertex shader.
    DWORD vdecl[] =
    {
        D3DVSD_STREAM(0),
        D3DVSD_REG(0, D3DVSDT_FLOAT3),  // v0 = XYZ
        D3DVSD_REG(1, D3DVSDT_FLOAT3),  // v1 = normals
        D3DVSD_REG(2, D3DVSDT_FLOAT2),  // v2 = textures

        D3DVSD_STREAM(1),
        D3DVSD_REG(3, D3DVSDT_SHORT3),  // Bone matrix indices
        D3DVSD_REG(4, D3DVSDT_FLOAT3),  // Bone weights
        
        D3DVSD_END()
    };

    if ( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\MatrixPaletteSkinning.xvu", vdecl, &m_dwDeformShader ) ) )
        return E_FAIL;

    // Setup bones.
    SetupBones( m_Snake.m_pMeshFrames[0].m_MeshData );

    // Create mesh weights.
    CreateMeshWeights( m_Snake.m_pMeshFrames[0].m_MeshData );

    m_Mode       = ANIMATE_SINE;

    m_vCameraPos = D3DXVECTOR3( 0.0f, 0.0f, 3.0f );
    m_vViewAngle = D3DXVECTOR3( -0.5f, 3.0f, 0.0f );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: SetupBones
// Desc: Define the positon and influence ofthe bones.
//-----------------------------------------------------------------------------
void CXBoxSample::SetupBones( XBMESH_DATA& MeshData )
{
    // Get the bounding box of the mesh.
    D3DXVECTOR3 vMin, vMax;
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_Snake.ComputeMeshBoundingBox( &MeshData, &matIdentity, &vMin, &vMax );

    // Space bones evenly along z, going through the middle of the snake
    float x = ( vMin.x + vMax.x ) * 0.5f;
    float y = ( vMin.y + vMax.y ) * 0.5f;
    float z_step = ( vMax.z - vMin.z ) / m_iNumberOfBones;
    float z = vMin.z + 0.5f * z_step;

    for( int i = 0; i < m_iNumberOfBones; i++ )
    {
        m_vBoneCenter[i].x = x;
        m_vBoneCenter[i].y = y;
        m_vBoneCenter[i].z = z;

        D3DXMatrixIdentity( &m_matBoneTransforms[i] );

        z += z_step;
    }

    // The z distance that the bone influences.
    m_fBoneInfluenceRadius = z_step * 1.0f;
}



//-----------------------------------------------------------------------------
// Name: CreateMeshWeights
// Desc: Create a vertex buffer to hold the bone matrix indices and the 
//       weights.  We'll walk the list of bones, and determine which vertices
//       each bone influences (we'll only keep the top 3)
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateMeshWeights( XBMESH_DATA& MeshData )
{
    // Create a vertex buffer to hold the indices and weights ..
    m_pd3dDevice->CreateVertexBuffer( MeshData.m_dwNumVertices * sizeof( BoneIndicesAndWeights ),
                                    0, 0, 0, &m_pBoneVB );

    // .. and lock it so we can fill it in.
    BoneIndicesAndWeights* pVertexIndicesAndWeights;
    m_pBoneVB->Lock( 0, 0, (BYTE**)&pVertexIndicesAndWeights, 0 );

    // Initialize indices and weights.
    for( DWORD v = 0; v < MeshData.m_dwNumVertices; v++ )
    {
        for ( int j = 0; j < 3; j++ )
        {
            pVertexIndicesAndWeights[v].Indices[j] = 0;
            pVertexIndicesAndWeights[v].Weights[j] = 0.0f;
        }
    }

    BYTE* pVertices;
    MeshData.m_VB.Lock( 0, 0, &pVertices, 0 );

    // Evenly space bones long z.
    for( int i = 0; i < m_iNumberOfBones; i++ )
    {
        float z = m_vBoneCenter[i].z;

        // Caclulate influence of bones on each vertex.
        for( DWORD v = 0; v < MeshData.m_dwNumVertices; v++ )
        {
            D3DXVECTOR3* pVert = (D3DXVECTOR3*)( pVertices + v * MeshData.m_dwVertexSize );

            float weight = 1.0f - fabsf( pVert->z - z ) / m_fBoneInfluenceRadius;
            
            // Weight = min( 1 - distance / radius, 0 ).
            if( weight > 0.0f )
            {
                // Keep only the nearest 3.
                float min_weight = weight;
                int min_j = -1;

                for( int j = 0; j < 3; j++ )
                {
                    if( pVertexIndicesAndWeights[v].Weights[j] < min_weight )
                    {
                        min_weight = pVertexIndicesAndWeights[v].Weights[j];
                        min_j = j;
                    }
                }

                if( min_j != -1 )
                {
                    pVertexIndicesAndWeights[v].Indices[min_j] = BONE_REGISTER_BASE + i * 3;
                    pVertexIndicesAndWeights[v].Weights[min_j] = weight;
                }
            }   
        }
    }

    MeshData.m_VB.Unlock();

    // Normalize weights.
    for( DWORD v = 0; v < MeshData.m_dwNumVertices; v++ )
    {
        float total_weight = 0.0f;

        for( int j = 0; j < 3; j++ )
        {
            total_weight += pVertexIndicesAndWeights[v].Weights[j];
        }

        if( total_weight > 0.0f )
        {
            float inv_total_weight = 1.0f / total_weight;

            for( int j = 0; j < 3; j++ )
            {
                pVertexIndicesAndWeights[v].Weights[j] *= inv_total_weight;
            }
        }
    }

    m_pBoneVB->Unlock();

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: UpdateBoneTransforms
// Desc: Offset the bones using a time based sine fuction.
//-----------------------------------------------------------------------------
void CXBoxSample::UpdateBoneTransforms()
{
    switch( m_Mode )
    {
    case ANIMATE_SINE:
        // Update transform for each bone.
        for( int i = 0; i < m_iNumberOfBones; i++ )
        {
            D3DXMATRIX mat;

            // x = f(z+t) = sin(z+t) * 0.2
            // tangent = f'(z+t) = cos(z+t) * 0.2

            float t = 10.0f * i / m_iNumberOfBones - m_fAppTime * 2.0f;
            float angle = atanf( cosf( t ) * 0.2f );
            
            // Translate the bone back to the origin
            D3DXMatrixTranslation( &m_matBoneTransforms[i], -m_vBoneCenter[i].x,
                                   -m_vBoneCenter[i].y, -m_vBoneCenter[i].z );

            // Rotate to the tangent angle
            D3DXMatrixRotationY( &mat, angle );
            D3DXMatrixMultiply( &m_matBoneTransforms[i], &m_matBoneTransforms[i], &mat );

            // Then translate back to bone position + sine wave
            D3DXMatrixTranslation( &mat, m_vBoneCenter[i].x + sinf( t ) * 0.2f, 
                                   m_vBoneCenter[i].y, m_vBoneCenter[i].z );
            D3DXMatrixMultiply( &m_matBoneTransforms[i], &m_matBoneTransforms[i], &mat );
        }
        break;

    case ANIMATE_CIRCLE:
        for( int i = 0; i < m_iNumberOfBones; i++ )
        {
            D3DXMATRIX mat;

            // x = cos( z + time )
            // z = -sin( z + time )
            // tan = z + time

            float t = -5.0f * i / m_iNumberOfBones + m_fAppTime;
            float angle = t;

            // Translate to origin
            D3DXMatrixTranslation( &m_matBoneTransforms[i], 
                                   -m_vBoneCenter[i].x,
                                   -m_vBoneCenter[i].y,
                                   -m_vBoneCenter[i].z );

            // Rotate bone
            D3DXMatrixRotationY( &mat, angle );
            D3DXMatrixMultiply( &m_matBoneTransforms[i], &m_matBoneTransforms[i], &mat );

            // Translate to position on unit circle
            D3DXMatrixTranslation( &mat, 
                                   cosf( t ),
                                   m_vBoneCenter[i].y,
                                   -sinf( t ) );
            D3DXMatrixMultiply( &m_matBoneTransforms[i], &m_matBoneTransforms[i], &mat );
        }
        break;
    }

    D3DXMATRIX mat;

    // Send bone transforms to the vertex shader.
    for( int i = 0; i < m_iNumberOfBones; i++ )
    {
        D3DXMatrixTranspose( &mat, &m_matBoneTransforms[i] );
        m_pd3dDevice->SetVertexShaderConstant( BONE_REGISTER_BASE + i * 3, &mat, 3 );
    }
}



//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
    {
        m_Mode = ANIMATE_MODE( ( m_Mode + 1 ) % ANIMATE_MAX );
    }

    // Set the matrices
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Update camera angle and position
    m_vViewAngle.y -= m_DefaultGamepad.fX1 * m_fElapsedTime;
    if( m_vViewAngle.y > 2 * D3DX_PI )
        m_vViewAngle.y -= 2 * D3DX_PI;
    else if( m_vViewAngle.y < 0.0f )
        m_vViewAngle.y += 2 * D3DX_PI;

    m_vViewAngle.x -= m_DefaultGamepad.fY1 * m_fElapsedTime;
    if( m_vViewAngle.x > 1.0f )
        m_vViewAngle.x = 1.0f;
    else if( m_vViewAngle.x < -1.0f )
        m_vViewAngle.x = -1.0f;

    m_vCameraPos.z -= m_DefaultGamepad.fY2 * m_fElapsedTime;
    if( m_vCameraPos.z < 0.2f )
        m_vCameraPos.z = 0.2f;

    // Calculate eye position based off view angle and camera zoom
    D3DXVECTOR3 vEyePosition;
    D3DXMATRIX m;
    D3DXMatrixRotationYawPitchRoll( &m, m_vViewAngle.y, m_vViewAngle.x, m_vViewAngle.z );
    D3DXVec3TransformCoord( &vEyePosition, &m_vCameraPos, &m );

    // Set up a view transform to look at the origin
    D3DXMATRIX  matView;
    D3DXVECTOR3 vLookAt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vEyePosition, &vLookAt, &vUp );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Projection transform
    D3DXMATRIX matProjection;
    D3DXMatrixPerspectiveFovLH( &matProjection, D3DX_PI/4, 4.0f/3.0f, 0.1f, 500.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjection );

    // World * View * Projection composite transformation
    D3DXMATRIX matComposite;
    D3DXMatrixMultiply( &matComposite, &matWorld, &matView );
    D3DXMatrixMultiply( &matComposite, &matComposite, &matProjection );
    D3DXMatrixTranspose( &matComposite, &matComposite );
    m_pd3dDevice->SetVertexShaderConstant( 0, &matComposite, 4 );

    // Adjust number of bones
    int iOldBones = m_iNumberOfBones;
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
    {
        if( ++m_iNumberOfBones > MAX_BONES )
            m_iNumberOfBones = MAX_BONES;
    }
    else if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
    {
        if( --m_iNumberOfBones < MIN_BONES )
            m_iNumberOfBones = MIN_BONES;
    }

    if( iOldBones != m_iNumberOfBones )
    {
        // Setup bones.
        SetupBones( m_Snake.m_pMeshFrames[0].m_MeshData );

        // Create mesh weights.
        CreateMeshWeights( m_Snake.m_pMeshFrames[0].m_MeshData );
    }

    // Animate bones based off current time
    UpdateBoneTransforms();

    // Set light direction
    D3DXVECTOR4 vLightDir( 0.7071067f, 0.7071067f, 0.0f, 0.0f );
    m_pd3dDevice->SetVertexShaderConstant( 4, &vLightDir, 1 );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    // Draw the snake.
    m_pd3dDevice->SetVertexShader( m_dwDeformShader );
    m_pd3dDevice->SetStreamSource( 1, m_pBoneVB, sizeof( BoneIndicesAndWeights ) );

    // Set up render and texture states
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

    m_Snake.Render( m_pd3dDevice, XBMESH_NOFVF | XBMESH_NOMATERIALS );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"MatrixPaletteSkinning" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        WCHAR str[100];
        swprintf( str, L"Number of bones: %d", m_iNumberOfBones );
        m_Font.DrawText( 64, 80, 0xffffff00, str );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

