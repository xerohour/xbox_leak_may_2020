#include "stdafx.h"
#include "gouraudgroup.h"


// Constructor	
GouraudGroup::GouraudGroup(void) :
m_dwTotalObjects( 0 ),
m_pObjects( NULL )
{
}


// Destructor
GouraudGroup::~GouraudGroup(void) 
{
    // Clean up our objects
    if( m_pObjects )
    {
        delete[] m_pObjects;
        m_pObjects = NULL;
    }
}


// Render our Group
void GouraudGroup::render( IDirect3DDevice8* pD3DDevice )
{
	for( DWORD i = 0; i < m_dwTotalObjects; ++i )
	{
		m_pObjects[i].render( pD3DDevice );
	}
}


// Render our Group
void GouraudGroup::renderStrip( IDirect3DDevice8* pD3DDevice )
{
	for( DWORD i = 0; i < m_dwTotalObjects; ++i )
	{
		m_pObjects[i].renderStrip( pD3DDevice );
	}
}


// Load the models for our group
void GouraudGroup::loadMDL( char* pszFilename, IDirect3DDevice8* pD3DDevice )
{
	FILE* pfInput;

    pfInput = fopen( pszFilename, "rb" );

    if( !pfInput )
    {
        XDBGERR( APP_TITLE_NAME_A, "GouraudGroup::loadMDL( char* ):Failed to open the file - '%s'!!", pszFilename );

        return;
    }
    
	fread( &m_dwTotalObjects, sizeof(DWORD), 1, pfInput );

    // Clean up our objects
    if( m_pObjects )
    {
        delete[] m_pObjects;
        m_pObjects = NULL;
    }

    m_pObjects = new GouraudObject[m_dwTotalObjects];
	
    for( DWORD i = 0; i < m_dwTotalObjects; ++i )
	{	
		m_pObjects[i].loadMDL( pfInput, pD3DDevice );
	}
	
    fclose( pfInput );
}


// Load the models for our group
void GouraudGroup::loadStrippedMDL( char* pszFilename, IDirect3DDevice8* pD3DDevice )
{
	FILE* pfInput;

	pfInput = fopen( pszFilename, "rb" );

    if( !pfInput )
    {
        XDBGERR( APP_TITLE_NAME_A, "GouraudGroup::loadStrippedMDL( char* ):Failed to open the file - '%s'!!", pszFilename );

        return;
    }

	fread( &m_dwTotalObjects, sizeof(DWORD), 1, pfInput );

    // Clean up our objects
    if( m_pObjects )
    {
        delete[] m_pObjects;
        m_pObjects = NULL;
    }
	
    m_pObjects = new GouraudObject[m_dwTotalObjects];
	
    for( DWORD i = 0; i < m_dwTotalObjects; ++i )
	{	
		m_pObjects[i].loadStrippedMDL( pfInput, pD3DDevice );

        XDBGTRC( APP_TITLE_NAME_A, "GouraudGroup::loadStrippedMDL( char* ):Loaded object %d of %d to file", i + 1, m_dwTotalObjects );
	}

    fclose( pfInput );
}