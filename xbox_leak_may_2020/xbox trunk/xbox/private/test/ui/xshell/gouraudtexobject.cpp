#include "stdafx.h"
#include "gouraudtexobject.h"

// Constructor
GouraudTexObject::GouraudTexObject( void ) :
m_wNumVertices( 0 ),
m_pVertex( NULL ),
m_wNumFaces( 0 ),
m_pFace( NULL ),
m_pVB( NULL ),
m_pTexture( NULL ),
m_wNumTexVertices( 0 ),
m_pTextureVertex( NULL )
{
    ZeroMemory( m_pszTextureName, 30 );
}

// Desctructor
GouraudTexObject::~GouraudTexObject( void )
{
    // Clean up our vertices
    if( m_pVertex )
    {
        delete[] m_pVertex;
        m_pVertex = NULL;
    }

    // Clean up our faces
    if( m_pFace )
    {
        delete[] m_pFace;
        m_pFace = NULL;
    }
}


// Load our Texture from a file
void GouraudTexObject::loadTexture( char pszTexture[30], IDirect3DDevice8* pD3DDevice )
{
    if( ( NULL == pszTexture ) || ( NULL == pD3DDevice ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "GouraudTexObject::loadTexture():Invalid parameter passed in!!" );

        return;
    }

    _snprintf( m_pszTextureName, 29, "%s", pszTexture );
    
    XDBGTRC( APP_TITLE_NAME_A, "GouraudTexObject::loadTexture():pszTexture       - '%hs'", pszTexture );
    XDBGTRC( APP_TITLE_NAME_A, "GouraudTexObject::loadTexture():m_pszTextureName - '%hs'", m_pszTextureName );

	//
	// Load the appropriate texture
	//
    if( m_pTexture )
    {
        m_pTexture->Release();
        m_pTexture = NULL;
    }

	if( FAILED( D3DXCreateTextureFromFile( pD3DDevice, m_pszTextureName, &m_pTexture ) ) )
	{
        XDBGWRN( APP_TITLE_NAME_A, "GouraudTexObject::loadTexture():Unable to load the texture!!" );
	}
}

void GouraudTexObject::render( IDirect3DDevice8* pD3DDevice )
{
	
	D3DXMatrixTranslation( &m_HoldMatrix, m_TranslateVector.x, m_TranslateVector.y, m_TranslateVector.z );
	
//	D3DXMatrixMultiply( &m_HoldMatrix, &m_ObjectMatrix, &m_HoldMatrix );
//	D3DXMatrixMultiply( &m_WorkMatrix, &m_WorkMatrix, &m_HoldMatrix );
	D3DXMatrixMultiply( &m_WorkMatrix, &m_WorkMatrix, &m_ObjectMatrix);
//	pD3DDevice->SetTransform( D3DTS_WORLD, &m_WorkMatrix );
	pD3DDevice->SetTransform( D3DTS_WORLD, &m_ObjectMatrix );


	pD3DDevice->SetStreamSource( 0, m_pVB, sizeof(TEXVERTEX) );
	pD3DDevice->SetVertexShader( D3DFVF_TEXVERTEX );

    pD3DDevice->DrawPrimitive( D3DPT_TRIANGLELIST , 0, m_wNumFaces );
}

void GouraudTexObject::loadMDL( char* pszFilename, IDirect3DDevice8* pD3DDevice )
{
	FILE* pfInput;
	DWORD dwTotalObjects;

	pfInput = fopen( pszFilename, "rb" );

    if( !pfInput )
    {
        XDBGERR( APP_TITLE_NAME_A, "GouraudTexObject::loadMDL( char* ):Failed to open the file - '%s'!!", pszFilename );

        return;
    }

	fread( &dwTotalObjects, sizeof(DWORD), 1, pfInput );

	loadMDL( pfInput, pD3DDevice );
	fclose( pfInput );
}

void GouraudTexObject::loadMDL( FILE* pfInput, IDirect3DDevice8* pD3DDevice )
{
	//
	// Read vertices, normals, and diffuse for object
	//
	fread( &m_wNumVertices, sizeof(WORD), 1, pfInput );

    // Clean up our vertices
    if( m_pVertex )
    {
        delete[] m_pVertex;
        m_pVertex = NULL;
    }
	
    m_pVertex = new TEXVERTEX[m_wNumVertices];

	for( WORD i = 0; i < m_wNumVertices; ++i )
	{
		fread( &m_pVertex[i].p.x, sizeof(float), 1, pfInput );
		fread( &m_pVertex[i].p.y, sizeof(float), 1, pfInput );
		fread( &m_pVertex[i].p.z, sizeof(float), 1, pfInput );

		fread( &m_pVertex[i].n.x, sizeof(float), 1, pfInput );
		fread( &m_pVertex[i].n.y, sizeof(float), 1, pfInput );
		fread( &m_pVertex[i].n.z, sizeof(float), 1, pfInput );

		fread( &m_pVertex[i].diffuse, sizeof(DWORD), 1, pfInput );

        /*
		fread( &m_pVertex[i].tu, sizeof(float), 1, pfInput );
		fread( &m_pVertex[i].tv, sizeof(float), 1, pfInput );

        m_pVertex[i].tv += 1.0f;
        
        m_pVertex[i].tu *= MENUBOX_MAINAREA_X2;
        m_pVertex[i].tv *= MENUBOX_MAINAREA_Y2;
        */
	}

	//
	// Read in a list of the vertex coordinates
	//
	fread( &m_wNumTexVertices, sizeof(WORD), 1, pfInput );

    if( m_pTextureVertex )
    {
        delete[] m_pTextureVertex;
        m_pTextureVertex = NULL;
    }

	m_pTextureVertex  = new TextureVert[m_wNumTexVertices];

	for( i = 0; i < m_wNumTexVertices; i++ )
	{
		fread( &m_pTextureVertex[i].m_tu, sizeof(float), 1, pfInput );
		fread( &m_pTextureVertex[i].m_tv, sizeof(float), 1, pfInput );
    }

    //
	// Read face list for object
	//
	fread( &m_wNumFaces, sizeof(WORD), 1, pfInput );

    // Clean up our faces
    if( m_pFace )
    {
        delete[] m_pFace;
        m_pFace = NULL;
    }

	m_pFace = new TexFace[m_wNumFaces];

	for( i = 0; i < m_wNumFaces; ++i )
	{
		fread( &m_pFace[i].point[0], sizeof(WORD), 1, pfInput );
		fread( &m_pFace[i].point[1], sizeof(WORD), 1, pfInput );
		fread( &m_pFace[i].point[2], sizeof(WORD), 1, pfInput );

		fread( &m_pFace[i].m_wTexPoint[0], sizeof(WORD), 1, pfInput );
		fread( &m_pFace[i].m_wTexPoint[1], sizeof(WORD), 1, pfInput );
		fread( &m_pFace[i].m_wTexPoint[2], sizeof(WORD), 1, pfInput );
	}

	D3DXMatrixIdentity( &m_ObjectMatrix );
	m_TranslateVector.x = 0.0f;
	m_TranslateVector.y = 0.0f;
	m_TranslateVector.z = 0.0f;
	m_TranslateVector.w = 1.0f;

	//
	// Create a vertex buffer for the object
	//
    if( m_pVB )
    {
        m_pVB->Release();
        m_pVB = NULL;
    }

	if( FAILED( pD3DDevice->CreateVertexBuffer( m_wNumFaces * 3 * sizeof(TEXVERTEX),
                                                0,  /* Usage */
                                                D3DFVF_TEXVERTEX,
                                                D3DPOOL_DEFAULT, 
                                                &m_pVB ) ) )
	{
        XDBGWRN( APP_TITLE_NAME_A, "GouraudTexObject::loadMDL(FILE*):Failed to create a Vertex Buffer!!" );
	}

	//
	// Fill the vertex buffer
	//
	TEXVERTEX* pVertices;
	if( FAILED( m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
        XDBGWRN( APP_TITLE_NAME_A, "GouraudTexObject::loadMDL(FILE*):Failed to lock the Vertex Buffer!!" );
	}
	
    //memcpy( pVertices, m_pVertex, sizeof(m_pVertex) );
	int counter = 0;
    for( i = 0; i < m_wNumFaces; ++i )
	{
		m_pVertex[m_pFace[i].point[0]].tu = m_pTextureVertex[m_pFace[i].m_wTexPoint[0]].m_tu;
		m_pVertex[m_pFace[i].point[0]].tv = m_pTextureVertex[m_pFace[i].m_wTexPoint[0]].m_tv;
		pVertices[counter++] = m_pVertex[m_pFace[i].point[0]];

		m_pVertex[m_pFace[i].point[1]].tu = m_pTextureVertex[m_pFace[i].m_wTexPoint[1]].m_tu;
		m_pVertex[m_pFace[i].point[1]].tv = m_pTextureVertex[m_pFace[i].m_wTexPoint[1]].m_tv;
		pVertices[counter++] = m_pVertex[m_pFace[i].point[1]];

		m_pVertex[m_pFace[i].point[2]].tu = m_pTextureVertex[m_pFace[i].m_wTexPoint[2]].m_tu;
		m_pVertex[m_pFace[i].point[2]].tv = m_pTextureVertex[m_pFace[i].m_wTexPoint[2]].m_tv;
		pVertices[counter++] = m_pVertex[m_pFace[i].point[2]];
	}

    m_pVB->Unlock();
}