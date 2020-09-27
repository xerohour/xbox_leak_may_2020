/*****************************************************
*** testconfig.cpp
***
*** CPP file for our Test Config info class.
*** This class will hold information on a
*** configuration for a test.
*** 
*** by James N. Helm
*** December 5th, 2001
*** 
*****************************************************/

#include "stdafx.h"
#include "testconfig.h"

// Constructors
CTestConfig::CTestConfig( void ) :
m_lConfigVersion( 0l )
{
    m_ConfigItems.SetDeleteDataItem( TRUE );
}

// Copy Constructor
CTestConfig::CTestConfig( const CTestConfig& TestConfig )
{
    *this = TestConfig;
}

// Destructor
CTestConfig::~CTestConfig( void )
{
    ClearConfigItems();
}

// Assignment Operator
CTestConfig& CTestConfig::operator=( const CTestConfig& TestConfig )
{
    if( &TestConfig == this )
        return *this;

    // Clear out any data that was already here
    ClearConfigItems();

    SetConfigVersion( TestConfig.GetConfigVersion() );

    // Copy all of the config items
    for( unsigned int x = 0; x < TestConfig.GetNumConfigItems(); x++ )
    {
        CConfigItem* pConfigItem = TestConfig.GetConfigItem( x );
        CConfigItem* pNewConfigItem = NULL;

        // Determine which type of config item it is
        if( 0 == _wcsicmp( pConfigItem->GetType(), L"number" ) )
        {
            pNewConfigItem = (CConfigItem*)new CConfigNumber;
            if( NULL == pNewConfigItem )
            {
                DebugPrint( "CTestConfig::operator=():Unable to allocate memory!!\n" );

                break;
            }

            *((CConfigNumber*)pNewConfigItem) = *((CConfigNumber*)pConfigItem);
        }
        else if( 0 == _wcsicmp( pConfigItem->GetType(), L"range" ) )
        {
            pNewConfigItem = (CConfigItem*)new CConfigRange;
            if( NULL == pNewConfigItem )
            {
                DebugPrint( "CTestConfig::operator=():Unable to allocate memory!!\n" );

                break;
            }
            
            *((CConfigRange*)pNewConfigItem) = *((CConfigRange*)pConfigItem);
        }
        else if( 0 == _wcsicmp( pConfigItem->GetType(), L"string" ) )
        {
            pNewConfigItem = (CConfigItem*)new CConfigString;
            if( NULL == pNewConfigItem )
            {
                DebugPrint( "CTestConfig::operator=():Unable to allocate memory!!\n" );

                break;
            }
            
            *((CConfigString*)pNewConfigItem) = *((CConfigString*)pConfigItem);
        }
        else if( 0 == _wcsicmp( pConfigItem->GetType(), L"string2" ) )
        {
            pNewConfigItem = (CConfigItem*)new CConfigString2;
            if( NULL == pNewConfigItem )
            {
                DebugPrint( "CTestConfig::operator=():Unable to allocate memory!!\n" );

                break;
            }
            
            *((CConfigString2*)pNewConfigItem) = *((CConfigString2*)pConfigItem);
        }
        else if( 0 == _wcsicmp( pConfigItem->GetType(), L"numberstring" ) )
        {
            pNewConfigItem = (CConfigItem*)new CConfigNumberString;
            if( NULL == pNewConfigItem )
            {
                DebugPrint( "CTestConfig::operator=():Unable to allocate memory!!\n" );

                break;
            }
            
            *((CConfigNumberString*)pNewConfigItem) = *((CConfigNumberString*)pConfigItem);
        }

        AddConfigItem( pNewConfigItem );
    }

    return *this;
}

// Returns the number of config items this configuration contains
unsigned int CTestConfig::GetNumConfigItems( void ) const
{
    return m_ConfigItems.GetNumItems();
}


// Move the next config item in the list
BOOL CTestConfig::MoveNextConfigItem( void )
{
    return m_ConfigItems.MoveNext();
}


// Move the prev config item in the list
BOOL CTestConfig::MovePrevConfigItem( void )
{
    return m_ConfigItems.MovePrev();
}


// Move the first config item in the list
BOOL CTestConfig::MoveFirstConfigItem( void )
{
    return m_ConfigItems.MoveFirst();
}


// Move the last config item in the list
BOOL CTestConfig::MoveLastConfigItem( void )
{
    return m_ConfigItems.MoveLast();
}


// Get the config item that is currently being pointed to
CConfigItem* CTestConfig::GetCurrentConfigItem( void ) const
{
    return m_ConfigItems.GetCurrentNode();
}


// Get the config item at the specified index
CConfigItem* CTestConfig::GetConfigItem( unsigned int uiIndex ) const
{
    return m_ConfigItems.GetNode( uiIndex );
}

// Operators
BOOL CTestConfig::operator>( const CTestConfig& item )
{
    return FALSE;
}

BOOL CTestConfig::operator<( const CTestConfig& item )
{
    return FALSE;
}

BOOL CTestConfig::operator==( const CTestConfig& item )
{
    return FALSE;
}
