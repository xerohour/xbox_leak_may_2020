/*****************************************************
*** xbeinfo.cpp
***
*** CPP file for our XBE info class.
*** This file will contain the implementation an XBE
*** info object.
*** 
*** by James N. Helm
*** November 27th, 2001
*** 
*****************************************************/

#include "stdafx.h"
#include "xbeinfo.h"

CXBEInfo::CXBEInfo( void ) :
m_bLoaded( FALSE )
{
    ZeroMemory( m_pszDirectory, MAX_PATH + 1 );
    ZeroMemory( m_pszFilename, MAX_PATH + 1 );
    ZeroMemory( m_pszFileExtension, 4 );
    ZeroMemory( m_pwszTitleName, sizeof( WCHAR ) * ( XBEIMAGE_TITLE_NAME_LENGTH + 1 ) );

    m_TestConfigurations.SetDeleteDataItem( TRUE );
}

CXBEInfo::~CXBEInfo( void )
{
    m_TestConfigurations.EmptyList();
}


// Operators
BOOL CXBEInfo::operator>( const CXBEInfo& item )
{
    if( _wcsicmp( m_pwszTitleName, item.m_pwszTitleName ) > 0 )
        return TRUE;

    return FALSE;
}

BOOL CXBEInfo::operator<( const CXBEInfo& item )
{
    if( _wcsicmp( m_pwszTitleName, item.m_pwszTitleName ) < 0 )
        return TRUE;

    return FALSE;
}

BOOL CXBEInfo::operator==( const CXBEInfo& item )
{
    if( _wcsicmp( m_pwszTitleName, item.m_pwszTitleName ) == 0 )
        return TRUE;

    return FALSE;
}


// Add a test configuration based on the Default Config
void CXBEInfo::AddNewTestConfig( void )
{
    CTestConfig* pTestConfig = new CTestConfig;
    if( NULL == pTestConfig )
    {
        DebugPrint( "CXBEInfo::AddNewTestConfig():Could not allocate memory!!\n" );

        return;
    }

    *pTestConfig = *GetDefaultTestConfig();

    // Add the configuration to our list
    AddTestConfig( pTestConfig );

    // Set the current config to the last configuration (our newest)
    MoveLastTestConfig();
}


// Returns the number of test configs associated with this XBE
unsigned int CXBEInfo::GetNumTestConfigs( void )
{
    return m_TestConfigurations.GetNumItems();
}


// Move to the specified test configuration
void CXBEInfo::MoveToTestConfig( unsigned int uiIndex )
{
    m_TestConfigurations.MoveTo( uiIndex );
}


// Move to the next test config in the list
BOOL CXBEInfo::MoveNextTestConfig( void )
{
    return m_TestConfigurations.MoveNext();
}


// Move to the prev test config in the list
BOOL CXBEInfo::MovePrevTestConfig( void )
{
    return m_TestConfigurations.MovePrev();
}


// Move to the first test config in the list
BOOL CXBEInfo::MoveFirstTestConfig( void )
{
    return m_TestConfigurations.MoveFirst();
}


// Move to the last test config in the list
BOOL CXBEInfo::MoveLastTestConfig( void )
{
    return m_TestConfigurations.MoveLast();
}


// Get the default test config for our XBE
CTestConfig* CXBEInfo::GetDefaultTestConfig( void )
{
    return &m_DefaultTestConfig;
}


// Get the test config that is currently being pointed to
CTestConfig* CXBEInfo::GetCurrentTestConfig( void )
{
    return m_TestConfigurations.GetCurrentNode();
}


// Get the test config at the specified index
CTestConfig* CXBEInfo::GetTestConfig( unsigned int uiIndex )
{
    return m_TestConfigurations.GetNode( uiIndex );
}


// Delete the test config at the specified index
HRESULT CXBEInfo::DeleteTestConfig( unsigned int uiIndex )
{
    return m_TestConfigurations.DelNode( uiIndex );
}


// Delete the test config at current index
HRESULT CXBEInfo::DeleteCurrentTestConfig( void )
{
    HRESULT hr = E_ABORT;

    if( GetNumTestConfigs() > 1 )
        hr = m_TestConfigurations.DelCurrentNode();
    
    return hr;
}


// Set the directory of the XBE
void CXBEInfo::SetDirectory( char* pszDirectory )
{
    if( NULL == pszDirectory )
        return;

    strncpy( m_pszDirectory, pszDirectory, MAX_PATH );
}


// Set the filename of the XBE
void CXBEInfo::SetFilename( char* pszFilename )
{
    if( NULL == pszFilename )
        return;

    strncpy( m_pszFilename, pszFilename, MAX_PATH );
}


// Set the file extension of the XBE
void CXBEInfo::SetFileExtension( char* pszFileExtension )
{
    if( NULL == pszFileExtension )
        return;

    strncpy( m_pszFileExtension, pszFileExtension, 3 );
}


// Set the Title Name of the XBE
void CXBEInfo::SetTitleName( WCHAR* pwszTitleName )
{
    if( NULL == pwszTitleName )
        return;

    wcsncpy( m_pwszTitleName, pwszTitleName, XBEIMAGE_TITLE_NAME_LENGTH );
}
