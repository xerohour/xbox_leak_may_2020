/*****************************************************
*** configitems.cpp
***
*** CPP file for our config item classes.
*** This file will contain the definition for all
*** types of config items
*** 
*** by James N. Helm
*** November 27th, 2001
*** 
*****************************************************/

#include "stdafx.h"
#include "configitems.h"

/*****************************************************
/* CConfigItem( void )
/****************************************************/
CConfigItem::CConfigItem( void ) :
m_pwszFieldName( NULL ),
m_pwszHelpText( NULL ),
m_pwszType( NULL ),
m_bProcessed( FALSE )
{
}


CConfigItem::~CConfigItem( void )
{
    if( NULL != m_pwszFieldName )
    {
        delete[] m_pwszFieldName;
        m_pwszFieldName = NULL;
    }

    if( NULL != m_pwszHelpText )
    {
        delete[] m_pwszHelpText;
        m_pwszHelpText = NULL;
    }

    if( NULL != m_pwszType )
    {
        delete[] m_pwszType;
        m_pwszType = NULL;
    }
}

// Copy constructor
CConfigItem::CConfigItem( const CConfigItem& ConfigItem )
{
    *this = ConfigItem;
}


// Assignment Operator
CConfigItem& CConfigItem::operator=( const CConfigItem& ConfigItem )
{
    if( &ConfigItem == this )
        return *this;

    SetFieldName( ConfigItem.GetFieldName() );
    SetHelpText( ConfigItem.GetHelpText() );
    SetType( ConfigItem.GetType() );

    return *this;
}


BOOL CConfigItem::operator>( const CConfigItem& item )
{
    if( _wcsicmp( m_pwszFieldName, item.m_pwszFieldName ) > 0 )
        return TRUE;

    return FALSE;
}

BOOL CConfigItem::operator<( const CConfigItem& item )
{
    if( _wcsicmp( m_pwszFieldName, item.m_pwszFieldName ) < 0 )
        return TRUE;

    return FALSE;
}

BOOL CConfigItem::operator==( const CConfigItem& item )
{
    if( _wcsicmp( m_pwszFieldName, item.m_pwszFieldName ) == 0 )
        return TRUE;

    return FALSE;
}


// Set the Field Name
HRESULT CConfigItem::SetFieldName( WCHAR* pwszName )
{
    if( NULL == pwszName )
    {
        DebugPrint( "CConfigItem::SetFieldName():Invalid argument passed in!!\n" );

        return E_INVALIDARG;
    }

    if( NULL != m_pwszFieldName )
    {
        delete[] m_pwszFieldName;
        m_pwszFieldName = NULL;
    }

    m_pwszFieldName = new WCHAR[wcslen( pwszName ) + 1];
    if( NULL == m_pwszFieldName )
    {
        DebugPrint( "CConfigItem::SetFieldName():Unable to allocate memory!!\n" );

        return E_OUTOFMEMORY;
    }

    // Copy the user passed in value in to our string
    wcscpy( m_pwszFieldName, pwszName );

    return S_OK;
}


// Set the Help Text
HRESULT CConfigItem::SetHelpText( WCHAR* pwszText )
{
    if( NULL == pwszText )
    {
        DebugPrint( "CConfigItem::SetHelpText():Invalid argument passed in!!\n" );

        return E_INVALIDARG;
    }

    if( NULL != m_pwszHelpText )
    {
        delete[] m_pwszHelpText;
        m_pwszHelpText = NULL;
    }

    m_pwszHelpText = new WCHAR[wcslen( pwszText ) + 1];
    if( NULL == m_pwszHelpText )
    {
        DebugPrint( "CConfigItem::SetHelpText():Unable to allocate memory!!\n" );

        return E_OUTOFMEMORY;
    }

    // Copy the user passed in value in to our string
    wcscpy( m_pwszHelpText, pwszText );

    return S_OK;
}


// Type of item
HRESULT CConfigItem::SetType( WCHAR* pwszType )
{
    if( NULL == pwszType )
    {
        DebugPrint( "CConfigItem::SetType():Invalid argument passed in!!\n" );

        return E_INVALIDARG;
    }

    if( NULL != m_pwszType )
    {
        delete[] m_pwszType;
        m_pwszType = NULL;
    }

    m_pwszType = new WCHAR[wcslen( pwszType ) + 1];
    if( NULL == m_pwszType )
    {
        DebugPrint( "CConfigItem::SetType():Unable to allocate memory!!\n" );

        return E_OUTOFMEMORY;
    }

    // Copy the user passed in value in to our string
    wcscpy( m_pwszType, pwszType );

    return S_OK;
}


/*****************************************************
/* CConfigNumber( void )
/****************************************************/

// Constructor
CConfigNumber::CConfigNumber( void ) :
m_lDefault( 0l ),
m_lMinimum( 0l ),
m_lMaximum( 0l )
{
}


// Copy constructor
CConfigNumber::CConfigNumber( const CConfigNumber& ConfigNumber )
{
    *this = ConfigNumber;
}


// Destructor
CConfigNumber::~CConfigNumber( void )
{
}


// Assignment Operator
CConfigNumber& CConfigNumber::operator=( const CConfigNumber& ConfigNumber )
{
    if( &ConfigNumber == this )
        return *this;

    // Call the base class assignment operator
    *((CConfigItem*)this) = ConfigNumber;

    SetMinimum( ConfigNumber.GetMinimum() );
    SetMaximum( ConfigNumber.GetMaximum() );
    SetDefault( ConfigNumber.GetDefault() );

    return *this;
}


// Process a value that was passed to the item
HRESULT CConfigNumber::ProcessValue( char* pszValue )
{
    long lValue = m_lDefault;
    if( sscanf( pszValue, "%ld", &lValue ) != 1 )
    {
        DebugPrint( "CConfigNumber::ProcessValue():Failed to process the value!!\n" );

        return E_FAIL;
    }

    SetDefault( lValue );
    SetProcessed( TRUE );

    return S_OK;
}


// Set the Default value
void CConfigNumber::SetDefault( long lDefault )
{
    if( ( lDefault < GetMinimum() ) || ( lDefault > GetMaximum() ) )
    {
        DebugPrint( "CConfigNumber::SetDefault():Number was out of range!! Num - '%ld', Min - '%ld', Max - '%d'\n",
                            lDefault,
                            GetMinimum(),
                            GetMaximum() );

        return;
    }

    m_lDefault = lDefault;
};


// Allow the user to update all values at once
void CConfigNumber::UpdateItem( WCHAR* pwszName, WCHAR* pwszHelpText, WCHAR* pwszType, long lDefault, long lMinimum, long lMaximum )
{
    // Error checking will be done by the functions called below

    SetFieldName( pwszName );
    SetHelpText( pwszHelpText );
    SetType( pwszType );
    SetMinimum( lMinimum );
    SetMaximum( lMaximum );
    SetDefault( lDefault );
}


/*****************************************************
/* CConfigRange( void )
/****************************************************/

// Constructor
CConfigRange::CConfigRange( void ) :
m_lDefault1( 0l ),
m_lMinimum1( 0l ),
m_lMaximum1( 0l ),
m_lDefault2( 0l ),
m_lMinimum2( 0l ),
m_lMaximum2( 0l )
{
}


// Copy constructor
CConfigRange::CConfigRange( const CConfigRange& CConfigRange )
{
    *this = CConfigRange;
}


// Assignment Operator
CConfigRange& CConfigRange::operator=( const CConfigRange& ConfigRange )
{
    if( &ConfigRange == this )
        return *this;

    // Call the base class assignment operator
    *((CConfigItem*)this) = ConfigRange;

    SetMinimum1( ConfigRange.m_lMinimum1 );
    SetMaximum1( ConfigRange.m_lMaximum1 );
    SetDefault1( ConfigRange.m_lDefault1 );
    SetMinimum2( ConfigRange.m_lMinimum2 );
    SetMaximum2( ConfigRange.m_lMaximum2 );
    SetDefault2( ConfigRange.m_lDefault2 );
    
    return *this;
}


// Destructor
CConfigRange::~CConfigRange( void )
{
}


// Process a value that was passed to the item
HRESULT CConfigRange::ProcessValue( char* pszValue )
{
    long lDef1, lDef2;
    if( sscanf( pszValue, "%ld-%ld", &lDef1, &lDef2 ) != 2 )
    {
        DebugPrint( "CConfigRange::ProcessValue():Failed to process the value!!\n" );

        return E_FAIL;
    }

    SetDefault1( lDef1 );
    SetDefault2( lDef2 );
    SetProcessed( TRUE );
    
    return S_OK;
}


// Set the Default 1st value
void CConfigRange::SetDefault1( long lDefault )
{
    if( ( lDefault < GetMinimum1() ) || ( lDefault > GetMaximum1() ) )
    {
        DebugPrint( "CConfigRange::SetDefault1():Number was out of range!! Num - '%ld', Min - '%ld', Max - '%d'\n",
                            lDefault,
                            GetMinimum1(),
                            GetMaximum1() );

        return;
    }

    m_lDefault1 = lDefault;
};


// Set the Default 2nd value
void CConfigRange::SetDefault2( long lDefault )
{
    if( ( lDefault < GetMinimum2() ) || ( lDefault > GetMaximum2() ) )
    {
        DebugPrint( "CConfigRange::SetDefault2():Number was out of range!! Num - '%ld', Min - '%ld', Max - '%d'\n",
                            lDefault,
                            GetMinimum2(),
                            GetMaximum2() );

        return;
    }

    m_lDefault2 = lDefault;
};


// Allow the user to update all values at once
void CConfigRange::UpdateItem( WCHAR* pwszName, WCHAR* pwszHelpText, WCHAR* pwszType, long lDefault1, long lMinimum1, long lMaximum1, long lDefault2, long lMinimum2, long lMaximum2 )
{
    // Error checking will be done by the functions called below

    SetFieldName( pwszName );
    SetHelpText( pwszHelpText );
    SetType( pwszType );
    SetMinimum1( lMinimum1 );
    SetMaximum1( lMaximum1 );
    SetDefault1( lDefault1 );
    SetMinimum2( lMinimum2 );
    SetMaximum2( lMaximum2 );
    SetDefault2( lDefault2 );
}


/*****************************************************
/* CConfigString( void )
/****************************************************/
// Constructor
CConfigString::CConfigString( void )
{
    // Make sure, if we empty our list, that we delete our data item
    // to prevent a memory leak
    m_StringList.SetDeleteDataItem( TRUE );
}

// Destructor
CConfigString::~CConfigString( void )
{
    m_StringList.EmptyList();
}


// Copy constructor
CConfigString::CConfigString( const CConfigString& ConfigString )
{
    *this = ConfigString;
}


// Assignment Operator
CConfigString& CConfigString::operator=( const CConfigString& ConfigString )
{
    if( &ConfigString == this )
        return *this;

    // Call the base class assignment operator
    *((CConfigItem*)this) = ConfigString;

    for( unsigned int x = 0; x < ConfigString.GetNumStrings(); x++ )
        AddString( ConfigString.GetString( x ) );

    SetCurrentString( ConfigString.GetCurrentStringIndex() );

    return *this;
}


// Process a value that was passed to the item
HRESULT CConfigString::ProcessValue( char* pszValue )
{
    // Convert our value to a WCHAR string for comparison
    WCHAR* pwszNewValue = new WCHAR[strlen( pszValue ) + 1];
    if( NULL == pwszNewValue )
    {
        DebugPrint( "CConfigString::ProcessValue(): Unable to allocate memory!!\n" );

        return E_OUTOFMEMORY;
    }

    // Convert our string
    swprintf( pwszNewValue, L"%hs", pszValue );

    // Check to see if the string we read exists in our list
    BOOL bStringFound = FALSE;

    MoveFirstString();
    do
    {
        if( 0 == _wcsicmp( pwszNewValue, GetCurrentString() ) )
        {
            bStringFound = TRUE;

            break;
        }
    } while( MoveNextString() );

    // Clean up our memory
    delete[] pwszNewValue;
    pwszNewValue = NULL;

    if( !bStringFound )
    {
        DebugPrint( "CConfigString::ProcessValue():Did not find the string in our list!! - '%hs'\n", pszValue );

        MoveFirstString();

        return E_FAIL;
    }

    SetProcessed( TRUE );

    return S_OK;
}


// Add a string item to our list
HRESULT CConfigString::AddString( WCHAR* pwszString )
{
    if( NULL == pwszString )
    {
        DebugPrint( "CConfigString::AddString():Invalid arguments passed in!!\n" );

        return E_INVALIDARG;
    }

    WCHAR* pwszNewString = new WCHAR[wcslen( pwszString ) + 1];
    if( NULL == pwszNewString )
    {
        DebugPrint( "CConfigString::AddString():Unable to allocate memory!!\n" );

        return E_OUTOFMEMORY;
    }

    // Copy the user string to our new memory and store the pointer
    wcscpy( pwszNewString, pwszString );
    
    m_StringList.AddNode( pwszNewString );

    return S_OK;
}


// Update the informational part of the item
void CConfigString::UpdateItem( WCHAR* pwszName, WCHAR* pwszHelpText, WCHAR* pwszType )
{
    SetFieldName( pwszName );
    SetHelpText( pwszHelpText );
    SetType( pwszType );
}

// Move to the first string in the list
BOOL CConfigString::MoveFirstString( void )
{
    return m_StringList.MoveFirst();
};


// Move to the last string in the list
BOOL CConfigString::MoveLastString( void )
{
    return m_StringList.MoveLast();
}

// Move to the next string in the list
BOOL CConfigString::MoveNextString( void )
{ 
    return m_StringList.MoveNext();
}

// Move to the previous string in the list
BOOL CConfigString::MovePrevString( void )
{ 
    return m_StringList.MovePrev();
}


/*****************************************************
/* CConfigString2( void )
/****************************************************/
// Constructor
CConfigString2::CConfigString2( void ) :
m_pwszString( NULL )
{
}

// Destructor
CConfigString2::~CConfigString2( void )
{
    if( NULL != m_pwszString )
    {
        delete[] m_pwszString;
        m_pwszString = NULL;
    }
}

// Copy constructor
CConfigString2::CConfigString2( const CConfigString2& ConfigString2 )
{
    *this = ConfigString2;
}


// Assignment Operator
CConfigString2& CConfigString2::operator=( const CConfigString2& ConfigString2 )
{
    if( &ConfigString2 == this )
        return *this;

    // Call the base class assignment operator
    *((CConfigItem*)this) = ConfigString2;

    if( NULL != m_pwszString )
    {
        delete[] m_pwszString;
        m_pwszString = NULL;
    }

    SetString( ConfigString2.GetString() );

    return *this;
}


// Process a value that was passed to the item
HRESULT CConfigString2::ProcessValue( char* pszValue )
{
    if( NULL == pszValue )
    {
        SetString( L"" );
        
        return S_OK;
    }

    // Convert our value to a WCHAR string for comparison
    WCHAR* pwszNewValue = new WCHAR[strlen( pszValue ) + 1];
    if( NULL == pwszNewValue )
    {
        DebugPrint( "CConfigString::ProcessValue(): Unable to allocate memory!!\n" );

        return E_OUTOFMEMORY;
    }

    // Convert our string
    swprintf( pwszNewValue, L"%hs", pszValue );

    HRESULT hr = SetString( pwszNewValue );

    // Clean up our memory
    delete[] pwszNewValue;
    pwszNewValue = NULL;

    SetProcessed( TRUE );

    return hr;
}


// Add a string item to our list
HRESULT CConfigString2::SetString( WCHAR* pwszString )
{
    if( NULL == pwszString )
    {
        DebugPrint( "CConfigString2::AddString():Invalid arguments passed in!!\n" );

        return E_INVALIDARG;
    }

    // Free our memory
    if( NULL != m_pwszString )
    {
        delete[] m_pwszString;
        m_pwszString = NULL;
    }

    m_pwszString = new WCHAR[wcslen( pwszString ) + 1];
    if( NULL == m_pwszString )
    {
        DebugPrint( "CConfigString2::AddString():Unable to allocate memory!!\n" );

        return E_OUTOFMEMORY;
    }

    // Copy the user string to our new memory and store the pointer
    wcscpy( m_pwszString, pwszString );
    
    return S_OK;
}


// Update the informational part of the item
void CConfigString2::UpdateItem( WCHAR* pwszName, WCHAR* pwszHelpText, WCHAR* pwszType, WCHAR* pwszString )
{
    SetFieldName( pwszName );
    SetHelpText( pwszHelpText );
    SetType( pwszType );
    SetString( pwszString );
}


/*****************************************************
/* CConfigNumberString( void )
/****************************************************/
// Constructor
CConfigNumberString::CConfigNumberString( void ) :
m_lDefault( 0l ),
m_lMinimum( 0l ),
m_lMaximum( 0l )
{
    // Make sure, if we empty our list, that we delete our data item
    // to prevent a memory leak
    m_StringList.SetDeleteDataItem( TRUE );
}

// Destructor
CConfigNumberString::~CConfigNumberString( void )
{
    m_StringList.EmptyList();
}


// Copy constructor
CConfigNumberString::CConfigNumberString( const CConfigNumberString& ConfigNumberString )
{
    *this = ConfigNumberString;
}


// Assignment Operator
CConfigNumberString& CConfigNumberString::operator=( const CConfigNumberString& ConfigNumberString )
{
    if( &ConfigNumberString == this )
        return *this;

    // Call the base class assignment operator
    *((CConfigItem*)this) = ConfigNumberString;

    SetMinimum( ConfigNumberString.GetMinimum() );
    SetMaximum( ConfigNumberString.GetMaximum() );
    SetDefault( ConfigNumberString.GetDefault() );

    // Store all the string in the liked list
    for( unsigned int x = 0; x < ConfigNumberString.GetNumStrings(); x++ )
        AddString( ConfigNumberString.GetString( x ) );

    SetCurrentString( ConfigNumberString.GetCurrentStringIndex() );

    return *this;
}


// Set the Default value
void CConfigNumberString::SetDefault( long lDefault )
{
    if( ( lDefault < GetMinimum() ) || ( lDefault > GetMaximum() ) )
    {
        DebugPrint( "CConfigNumberString::SetDefault():Number was out of range!! Num - '%ld', Min - '%ld', Max - '%d'\n",
                            lDefault,
                            GetMinimum(),
                            GetMaximum() );

        return;
    }

    m_lDefault = lDefault;
};


// Process a value that was passed to the item
HRESULT CConfigNumberString::ProcessValue( char* pszValue )
{
    long lNumValue = 0l;
    WCHAR pwszStrValue[MAX_PATH+1];
    pwszStrValue[MAX_PATH] = L'\0';

    if( sscanf( pszValue, "%ld %ls", &lNumValue, pwszStrValue ) != 2 )
    {
        DebugPrint( "CConfigNumberString::ProcessValue():Failed to process the value!!\n" );

        return E_FAIL;
    }

    ///////////////////////////////////////
    // Set our string portion of the value
    ///////////////////////////////////////
    // Check to see if the string we read exists in our list
    BOOL bStringFound = FALSE;

    MoveFirstString();
    do
    {
        if( 0 == _wcsicmp( pwszStrValue, GetCurrentString() ) )
        {
            bStringFound = TRUE;

            break;
        }
    } while( MoveNextString() );

    if( !bStringFound )
    {
        DebugPrint( "CConfigNumberString::ProcessValue():Did not find the string in our list!! - '%ls'\n", pwszStrValue );

        MoveFirstString();

        return E_FAIL;
    }

    ///////////////////////////////////////
    // Set our number portion of the value
    ///////////////////////////////////////
    SetDefault( lNumValue );
    
    SetProcessed( TRUE );

    return S_OK;
}


// Add a string item to our list
HRESULT CConfigNumberString::AddString( WCHAR* pwszString )
{
    if( NULL == pwszString )
    {
        DebugPrint( "CConfigNumberString::AddString():Invalid arguments passed in!!\n" );

        return E_INVALIDARG;
    }

    WCHAR* pwszNewString = new WCHAR[wcslen( pwszString ) + 1];
    if( NULL == pwszNewString )
    {
        DebugPrint( "CConfigNumberString::AddString():Unable to allocate memory!!\n" );

        return E_OUTOFMEMORY;
    }

    // Copy the user string to our new memory and store the pointer
    wcscpy( pwszNewString, pwszString );
    
    m_StringList.AddNode( pwszNewString );

    return S_OK;
}


// Update the informational part of the item
void CConfigNumberString::UpdateItem( WCHAR* pwszName,
                                      WCHAR* pwszHelpText,
                                      WCHAR* pwszType,
                                      long lDefault,
                                      long lMinimum,
                                      long lMaximum )
{
    SetFieldName( pwszName );
    SetHelpText( pwszHelpText );
    SetType( pwszType );
    SetMinimum( lMinimum ); // Set the Minimum value
    SetMaximum( lMaximum ); // Set the Maximum value
    SetDefault( lDefault ); // Set the Default value
}

// Move to the first string in the list
BOOL CConfigNumberString::MoveFirstString( void )
{
    return m_StringList.MoveFirst();
};


// Move to the last string in the list
BOOL CConfigNumberString::MoveLastString( void )
{
    return m_StringList.MoveLast();
}

// Move to the next string in the list
BOOL CConfigNumberString::MoveNextString( void )
{ 
    return m_StringList.MoveNext();
}

// Move to the previous string in the list
BOOL CConfigNumberString::MovePrevString( void )
{ 
    return m_StringList.MovePrev();
}

