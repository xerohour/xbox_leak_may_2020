#include "stdafx.h"
#include "gouraudobject.h"


// Constructor
GouraudObject::GouraudObject( void ) :
m_pVB( NULL ),
m_pIB( NULL ),
m_wNumVertices( 0 ),
m_pVertex( NULL ),
m_wNumFaces( 0 ),
m_pFace( NULL ),
m_dwPrimitiveCount( 0 ),
m_dwNumVertices( 0 ),
m_dwStrippedIndexCount( 0 ),
m_pwStrippedIndices( NULL )
{

}

// Destructor
GouraudObject::~GouraudObject( void ) 
{
    // Clean up our Vertex Buffer
    if( m_pVB )
    {
        m_pVB->Release();
        m_pVB = NULL;
    }

    // Clean up our Index Buffer
    if( m_pIB )
    {
        m_pIB->Release();
        m_pIB = NULL;
    }

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

    // Clean up our Stripped Indices
    if( m_pwStrippedIndices )
    {
        delete[] m_pwStrippedIndices;
        m_pwStrippedIndices = NULL;
    }
}


void GouraudObject::render( IDirect3DDevice8 *pD3DDevice )
{
	pD3DDevice->SetTransform( D3DTS_WORLD, &m_ObjectMatrix );

	pD3DDevice->SetStreamSource( 0, m_pVB, sizeof(XRAYVERTEX) );
	pD3DDevice->SetIndices( m_pIB, 0 );

	pD3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
		                              0,
									  m_wNumVertices,
									  0,
									  m_wNumFaces );
}

void GouraudObject::renderStrip( IDirect3DDevice8* pD3DDevice )
{
	pD3DDevice->SetTransform( D3DTS_WORLD, &m_ObjectMatrix );

	pD3DDevice->SetStreamSource( 0, m_pVB, sizeof(XRAYVERTEX));
	pD3DDevice->SetIndices( m_pIB, 0 );

	pD3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP , 
		                              0, 
									  m_dwNumVertices, 
									  0, 
									  m_dwPrimitiveCount );
}

void GouraudObject::loadMDL( char* pszFilename, IDirect3DDevice8* pD3DDevice )
{
	FILE* pfInput;
	int   iTotalObjects;

	pfInput = fopen( pszFilename, "rb" );

    if( !pfInput )
    {
        XDBGERR( APP_TITLE_NAME_A, "GouraudObject::loadMDL( char* ):Failed to open the file - '%s'!!", pszFilename );

        return;
    }

	fread( &iTotalObjects, sizeof(DWORD), 1, pfInput );
	loadMDL( pfInput, pD3DDevice );
	fclose( pfInput );
}

void GouraudObject::loadMDL( FILE* pfInput, IDirect3DDevice8* pD3DDevice )
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

	m_pVertex = new XRAYVERTEX[m_wNumVertices];
	
    for( WORD i = 0; i < m_wNumVertices; ++i )
	{
		fread( &m_pVertex[i].p.x, sizeof(float), 1, pfInput );
		fread( &m_pVertex[i].p.y, sizeof(float), 1, pfInput );
		fread( &m_pVertex[i].p.z, sizeof(float), 1, pfInput );

		fread( &m_pVertex[i].n.x, sizeof(float), 1, pfInput );
		fread( &m_pVertex[i].n.y, sizeof(float), 1, pfInput );
		fread( &m_pVertex[i].n.z, sizeof(float), 1, pfInput );

		fread( &m_pVertex[i].diffuse, sizeof(DWORD), 1, pfInput );
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

	m_pFace = new Face[m_wNumFaces];

	for( i = 0; i < m_wNumFaces; ++i )
	{
		fread( &m_pFace[i].point[0], sizeof(WORD), 1, pfInput );
		fread( &m_pFace[i].point[1], sizeof(WORD), 1, pfInput );
		fread( &m_pFace[i].point[2], sizeof(WORD), 1, pfInput );
	}

	D3DXMatrixIdentity( &m_ObjectMatrix );
	m_TranslateVector.x = 0.0f;
	m_TranslateVector.y = 0.0f;
	m_TranslateVector.z = 0.0f;
	m_TranslateVector.w = 1.0f;

	//
	// Create a vertex buffer for the object
	//

    // Make sure this is cleared before we create another one
    if( m_pVB )
    {
        m_pVB->Release();
        m_pVB = NULL;
    }

    if( FAILED( pD3DDevice->CreateVertexBuffer( m_wNumVertices * sizeof(XRAYVERTEX),
                                                0,
                                                D3DFVF_D3DVERTEX,
                                                D3DPOOL_DEFAULT, 
                                                &m_pVB ) ) )
	{
        XDBGWRN( APP_TITLE_NAME_A, "GouraudObject::loadMDL( FILE* ):Failed to create a Vertex Buffer!!" );
	}

	//
	// Fill the vertex buffer
	//
	XRAYVERTEX* pVertices;
	if( FAILED( m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
        XDBGWRN( APP_TITLE_NAME_A, "GouraudObject::loadMDL( FILE* ):Failed to lock the Vertex Buffer!!" );
	}

	for( i = 0; i < m_wNumVertices; ++i )
	{
		pVertices[i] = m_pVertex[i];
	}
	
    m_pVB->Unlock();


	//
	// Create an index buffer for the object
	//

    // Make sure this is cleared before we create another one
    if( m_pIB )
    {
        m_pIB->Release();
        m_pIB = NULL;
    }

	if( FAILED( pD3DDevice->CreateIndexBuffer( m_wNumFaces * 3 * sizeof(WORD),
                                               D3DUSAGE_WRITEONLY,
                                               D3DFMT_INDEX16,
                                               D3DPOOL_DEFAULT,
                                               &m_pIB ) ) )
	{
        XDBGWRN( APP_TITLE_NAME_A, "GouraudObject::loadMDL( FILE* ):Failed to create an Index Buffer!!" );
	}

	WORD* pIndices;
	if( FAILED( m_pIB->Lock( 0, 0, (BYTE**)&pIndices, 0 ) ) )  
	{
        XDBGWRN( APP_TITLE_NAME_A, "GouraudObject::loadMDL( FILE* ):Failed to lock the Index Buffer!!" );
	}

	int counter = 0;
	for( i = 0; i < m_wNumFaces; ++i )
	{
		pIndices[counter++] = m_pFace[i].point[0];
		pIndices[counter++] = m_pFace[i].point[1];
		pIndices[counter++] = m_pFace[i].point[2];
	}
	
	m_pIB->Unlock();

}

void GouraudObject::loadStrippedMDL( FILE* pfInput, IDirect3DDevice8 *pD3DDevice )
{
	//
	// Read vertices, normals, and diffuse for object
	//
	fread( &m_dwNumVertices, sizeof(DWORD), 1, pfInput );

    // Clean up our vertices
    if( m_pVertex )
    {
        delete[] m_pVertex;
        m_pVertex = NULL;
    }

	m_pVertex = new XRAYVERTEX[m_dwNumVertices];

	for( DWORD i = 0; i < m_dwNumVertices; ++i )
	{
		fread( &m_pVertex[i].p.x, sizeof(float), 1, pfInput );
		fread( &m_pVertex[i].p.y, sizeof(float), 1, pfInput );
		fread( &m_pVertex[i].p.z, sizeof(float), 1, pfInput );

		fread( &m_pVertex[i].n.x, sizeof(float), 1, pfInput );
		fread( &m_pVertex[i].n.y, sizeof(float), 1, pfInput );
		fread( &m_pVertex[i].n.z, sizeof(float), 1, pfInput );

		fread( &m_pVertex[i].diffuse, sizeof(DWORD), 1, pfInput );
	}

	//
	// Read face list for object
	//
	fread( &m_dwStrippedIndexCount, sizeof(DWORD), 1, pfInput );

    // Clean up our Stripped Indices
    if( m_pwStrippedIndices )
    {
        delete[] m_pwStrippedIndices;
        m_pwStrippedIndices = NULL;
    }

	m_pwStrippedIndices = new WORD[m_dwStrippedIndexCount];

	for( i = 0; i < m_dwStrippedIndexCount; ++i )
	{
		fread( &m_pwStrippedIndices[i], sizeof(WORD), 1, pfInput );
	}

	m_dwPrimitiveCount = m_dwStrippedIndexCount - 2;

	D3DXMatrixIdentity( &m_ObjectMatrix );
	m_TranslateVector.x = 0.0f;
	m_TranslateVector.y = 0.0f;
	m_TranslateVector.z = 0.0f;
	m_TranslateVector.w = 1.0f;

	//
	// Create and fill the vertex buffer
	//
    
    // Make sure this is cleared before we create another one
    if( m_pVB )
    {
        m_pVB->Release();
        m_pVB = NULL;
    }
 	
    if( FAILED( pD3DDevice->CreateVertexBuffer( m_dwNumVertices * sizeof(XRAYVERTEX),
                                                0, //D3DUSAGE_WRITEONLY,
                                                D3DFVF_D3DVERTEX,
                                                D3DPOOL_DEFAULT, 
                                                &m_pVB ) ) )
	{
        XDBGWRN( APP_TITLE_NAME_A, "GouraudObject::loadStrippedMDL( FILE* ):Failed to create a Vertex Buffer!!" );
	}

	XRAYVERTEX* pVertices;
	if( FAILED( m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
	{
        XDBGWRN( APP_TITLE_NAME_A, "GouraudObject::loadStrippedMDL( FILE* ):Failed to lock the Vertex Buffer!!" );
	}
	
    memcpy( pVertices, m_pVertex, m_dwNumVertices * sizeof(XRAYVERTEX) );

	m_pVB->Unlock();

	//
	// Create an index buffer for the object
	//

    // Make sure this is cleared before we create another one
    if( m_pIB )
    {
        m_pIB->Release();
        m_pIB = NULL;
    }
	if( FAILED( pD3DDevice->CreateIndexBuffer( m_dwStrippedIndexCount * sizeof(WORD),
                                               D3DUSAGE_WRITEONLY,
                                               D3DFMT_INDEX16,
                                               D3DPOOL_DEFAULT,
                                               &m_pIB ) ) )
	{
        XDBGWRN( APP_TITLE_NAME_A, "GouraudObject::loadStrippedMDL( FILE* ):Failed to create an Index Buffer!!" );
	}

    WORD* pIndices;
	if( FAILED( m_pIB->Lock( 0, 0, (BYTE**)&pIndices, 0 ) ) )  
	{
        XDBGWRN( APP_TITLE_NAME_A, "GouraudObject::loadStrippedMDL( FILE* ):Failed to lock the Index Buffer!!" );
	}
	
    memcpy( pIndices, m_pwStrippedIndices, m_dwStrippedIndexCount * sizeof(WORD) );
	
	m_pIB->Unlock();
}