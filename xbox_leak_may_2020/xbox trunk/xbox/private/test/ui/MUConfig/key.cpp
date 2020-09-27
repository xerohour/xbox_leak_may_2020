#include "stdafx.h"
#include "key.h"

// Key Class by Victor Blanco
CKey::CKey()
:
m_iXOrigin( 0 ),
m_iYOrigin( 0 ),
m_iWidth( 0 ),
m_iHeight( 0 ),		
m_dwSelectColor( 0x00000000 ),
m_fRender( TRUE ),
m_pwszResultChar( NULL )
{
}

CKey::~CKey()
{
    if( m_pwszResultChar )
    {
        delete[] m_pwszResultChar;
        m_pwszResultChar = NULL;
    }
}

void CKey::define( int iX, int iY, int iW, int iH, DWORD dwColor )
{
	m_iXOrigin = iX;
	m_iYOrigin = iY;
	m_iWidth = iW;
	m_iHeight = iH;		
	m_dwSelectColor = dwColor;	
}

void CKey::defineText( WCHAR* pwszResult )
{
    if( !pwszResult )
    {
        return;
    }

    // Make sure we don't cause a memory leak
    if( m_pwszResultChar )
    {
        delete[] m_pwszResultChar;
        m_pwszResultChar = NULL;
    }

    m_pwszResultChar = _wcsdup( pwszResult );

    /*
	int iLength = wcslen( pwszResult );
	m_pwszResultChar = new WCHAR[iLength + 1];
	wcscpy( m_pwszResultChar, pwszResult );
    */
}

void CKey::setRender( BOOL fValue )
{
	m_fRender = fValue;
}

BOOL CKey::getRender( void )
{
	return m_fRender;
}