/*************************************************************************************************\
AccountInfo.cpp		: Implementation of the AccountInfo component.
Creation Date		: 1/23/2002 6:52:28 PM
Copyright Notice	: (C) 2000 Microsoft
Author				: Heidi Gaertner
//-------------------------------------------------------------------------------------------------
Notes				: 
\*************************************************************************************************/
#include "std.h"
#include "AccountData.h"
#include "dialog.h"

#define     TAG_COUNT 5

// keep as a pointer so this can be deallocated if necessary
Accounts*                Accounts::s_pAccounts = NULL;

#define ALTERNATE_NAME_COUNT 5



#define BIND(id,address)  { ##id, s_pAccounts->##address, countof( s_pAccounts->##address ) },


Accounts::ACT_BINDING_DATA Accounts::s_pBindData[] = 
{
    BIND( eACT_Idname,    m_NewAccountInfo.m_Data.wszNickname )
    BIND( eACT_Billing01, m_NewAccountInfo.m_Data.wszStreet1 )
    BIND( eACT_Billing02, m_NewAccountInfo.m_Data.wszPostalCode ) 
    BIND( eACT_Billing03, m_NewAccountInfo.m_Data.wszCity )
    BIND( eACT_Billing04, m_NewAccountInfo.m_Data.wszState )
    BIND( eACT_Billing05, m_NewAccountInfo.m_Data.wszBranchCode ) // they took away country code,,,,
    BIND( eACT_Idname, m_NewAccountInfo.m_Data.wszNickname )
    BIND( eACT_Idtag, m_NewAccountInfo.m_Data.wszKingdom )
    //BIND( eACT_Credit01, m_NewAccountInfo.m_Data.wszCardType )  // slamb: this is now bCardTypeId
    BIND( eACT_Credit02, m_NewAccountInfo.m_Data.wszCardHolder )
    BIND( eACT_Credit03, m_NewAccountInfo.m_Data.wszCardNumber )
    { eNullButtonId, 0, 0 } // need to terminate array
};


/*struct USER_ACCOUNT_INFO
{
    WCHAR   wszNickname[XONLINE_NAME_SIZE];
    WCHAR   wszKingdom[XONLINE_KINGDOM_SIZE];
    
    WCHAR   wszFirstName[MAX_FIRSTNAME_SIZE];
    WCHAR   wszLastName[MAX_LASTNAME_SIZE];
    WCHAR   wszStreet1[MAX_STREET_SIZE];
    WCHAR   wszStreet2[MAX_STREET_SIZE];
    WCHAR   wszStreet3[MAX_STREET_SIZE];
    WCHAR   wszCity[MAX_CITY_SIZE];
    WCHAR   wszState[MAX_STATE_SIZE];
    WCHAR   wszCountryCode[MAX_COUNTRYCODE_SIZE];
    WCHAR   wszPostalCode[MAX_POSTALCODE_SIZE];
    WCHAR   wszPhonePrefix[MAX_PHONE_PREFIX_SIZE];
    WCHAR   wszPhoneNumber[MAX_PHONE_NUMBER_SIZE];
    WCHAR   wszPhoneExtension[MAX_PHONE_EXTENSION_SIZE];
    
    WCHAR   wszCardHolder[MAX_CC_NAME_SIZE];
    WCHAR   wszCardType[MAX_CC_TYPE_SIZE];
    WCHAR   wszCardNumber[MAX_CC_NUMBER_SIZE];
    WCHAR   wszCardExpiration[MAX_CC_EXPIRATION_SIZE];
    
    WCHAR   wszEmail[MAX_EMAIL_SIZE];
    WCHAR   wszBankCode[MAX_BANK_CODE_SIZE];
    WCHAR   wszBranchCode[MAX_BRANCH_CODE_SIZE];
    WCHAR   wszCheckDigits[MAX_CHECK_DIGITS_SIZE];
}; */     



NewAccountInfo::NewAccountInfo()
{
    memset( &m_Data, 0, sizeof( m_Data ) );
    
    // Initalize Credit Card Expiration to 0 dates/times
    m_Data.ftCardExpiration.dwHighDateTime = 0;
    m_Data.ftCardExpiration.dwLowDateTime  = 0;
    m_ftDOB.dwHighDateTime = 0;
    m_ftDOB.dwLowDateTime  = 0;
}


//*************************************************************************************************
Accounts::Accounts()
{
    m_bHaveTask = 0;
    m_nCurTask = -1;
    m_CountryID = -1;

    // default to USA
    m_CountryID = 103;

    // need to fix up pointers...
    // grisly hack.
    int counter = 0; 
    while( s_pBindData[counter].buttonId != eNullButtonId )
    {
        long nonOffsetAddress = (long)s_pBindData[counter].pAddress;
        nonOffsetAddress += (long)this;
        s_pBindData[counter].pAddress = (TCHAR*)nonOffsetAddress;
        counter++;
    }
}

//*************************************************************************************************
Accounts::~Accounts()
{
}

//*************************************************************************************************
Accounts* Accounts::Instance()
{
    ASSERT( s_pAccounts );
    // don't allocate in here, because then we won't know who does the clean up.

    return s_pAccounts;
}

//*************************************************************************************************
HRESULT Accounts::Initialize()
{
    s_pAccounts = new Accounts();
    if ( !s_pAccounts )
    {
        return XBAPPERR_MEMORYERR;
    }

    return S_OK;
}

//*************************************************************************************************
HRESULT Accounts::CleanUp()
{
  if ( s_pAccounts )
      delete s_pAccounts;

  s_pAccounts = NULL;

  return S_OK;
}

//*************************************************************************************************
HRESULT  Accounts::UpdateAccountInfo()
{
    ASSERT( s_pAccounts );

    if ( s_pAccounts )
    {
        return s_pAccounts->FrameMove();
    }

    return -1; // truly hosed if this happens
}


//*************************************************************************************************
const TCHAR*    Accounts::GetValue(eButtonId eField)
{
    int index = 0;
    while( s_pBindData[index].buttonId != eNullButtonId )
    {
        if ( s_pBindData[index].buttonId == eField )
            return s_pBindData[index].pAddress;
        index++;
    }

    switch (eField)
    {
    case eACT_Credit04:
        {
            TCHAR *szExpiration = new TCHAR[20];
            swprintf( szExpiration, L"" );

            if ( 0 != m_NewAccountInfo.m_Data.ftCardExpiration.dwHighDateTime &&
                 0 != m_NewAccountInfo.m_Data.ftCardExpiration.dwLowDateTime )
            {
                // Credit Card Expiration 
                CDateObject tExpiration;
                tExpiration.setFileTime( m_NewAccountInfo.m_Data.ftCardExpiration );

                ASSERT( tExpiration.getMonth() >= 1 );
                ASSERT( tExpiration.getMonth() <= 12 );
                ASSERT( tExpiration.getYear()  >= 2000 );
                
                TCHAR *szYear = new TCHAR[6];
                _itow( tExpiration.getYear(), szYear, 10 );
           
                //BUGBUG: We should get this string from our locale class
                swprintf(szExpiration, L"%02d/%s", tExpiration.getMonth(), szYear + 2 );
            }
            return szExpiration;
        }
    case eACT_IdDOB:
        {
            TCHAR *szDOB = new TCHAR[20];
            swprintf( szDOB, L"" );

            if ( 0 != m_NewAccountInfo.m_ftDOB.dwHighDateTime &&
                 0 != m_NewAccountInfo.m_ftDOB.dwLowDateTime )
            {
                // Date of birth button
                CDateObject tDOB;
                tDOB.setFileTime( m_NewAccountInfo.m_ftDOB );

                ASSERT( tDOB.getDay()   >= 1 );
                ASSERT( tDOB.getDay()   <= 31 );
                ASSERT( tDOB.getMonth() >= 1 );
                ASSERT( tDOB.getMonth() <= 12 );
                
                TCHAR szYear[6];
                _itow( tDOB.getYear() , szYear, 10 );

                //BUGBUG: We should get this string from our locale class
                swprintf(szDOB, L"%02d/%02d/%s", tDOB.getMonth(), tDOB.getDay(), szYear + 2 );
            }
            return szDOB;
        }
    }

    return NULL;
}

//*************************************************************************************************
unsigned int Accounts::GetNumericValue(eButtonId eField)
{
    switch (eField)
    {
    case eGEN_Exp_TumblersMonth:
        {
            // Credit Card Expiration Month Tumbler
            CDateObject tExpiration; 
            tExpiration.setFileTime( m_NewAccountInfo.m_Data.ftCardExpiration );

            ASSERT( tExpiration.getMonth() >= 1 );
            ASSERT( tExpiration.getMonth() <= 12 );
            return tExpiration.getMonth();      
        }
    case eGEN_Exp_TumblersYear:
        {
            // Credit Card Expiration Year Tumbler
            CDateObject tExpiration; 

            tExpiration.setFileTime( m_NewAccountInfo.m_Data.ftCardExpiration );

            ASSERT( tExpiration.getYear() >= 2000 );
            return tExpiration.getYear();
        }
    case eGEN_Dob_TumblersMonth:
        {
            // Date of Birth Month Tumbler
            CDateObject tDOB; 
            tDOB.setFileTime( m_NewAccountInfo.m_ftDOB );
            return tDOB.getMonth();
        }
    case eGEN_Dob_TumblersDay:
        {
            // Date of Birth Day Tumbler
            CDateObject tDOB; 
            tDOB.setFileTime( m_NewAccountInfo.m_ftDOB );
            return tDOB.getDay();
        }
    case eGEN_Dob_TumblersYear:
        {
            // Date of Birth Year Tumbler
            CDateObject tDOB; 
            tDOB.setFileTime( m_NewAccountInfo.m_ftDOB );
            return tDOB.getYear();
        }
    }

    return NULL;
}

//*************************************************************************************************
// this will return XBMSG_NEED_WAITCURSOR, if there is
// a task pump involved, if so, pass UpdateAccountInfo 
// into the wait cursor dlg object
//*************************************************************************************************
HRESULT Accounts::Validate( eButtonId eField, const TCHAR* pNewValue )
{
    HRESULT hr = S_OK;


    // figure out which button needs to do what...
    switch (eField)
    {
        case eACT_Idname:
        {
             
        }
	    break;
        
        case eACT_Idtag:
        {
            // do we have tags?
            if ( !m_NewAccountInfo.m_TagList.size() )
            {
                hr = BeginTask( TASK_BEGIN_TAGS, pNewValue ); // start up this, so we're ready when the tags button is pressed
            }

        }
        break;
        
        case eACT_Idcontinue:
        {
            hr = BeginTask( TASK_ALT_NAMES, pNewValue );
        }
	    break;

        case eACT_Billing_Country01:
        case eACT_Billing_Country02:
        case eACT_Billing_Country03:
        {
            // Now that we have the country from the user, reinitalize local info
            hr = CTranslator::Instance()-> Initialize( m_CountryID );
        }
        break;

        case eACT_Cost_Confirm_Yes:
        {
            // User confirmed payment, create the account
            hr = BeginTask( TASK_CREATE_ACCOUNT, NULL );
        }
        break;
    }

    return hr;
}

HRESULT Accounts::Validate( eButtonId eField, const FILETIME* pNewValue )
{
    HRESULT hr = S_OK;


    switch (eField)
    {
    case eGEN_Exp_TumblersMonth:
    case eGEN_Exp_TumblersYear:
        {
            // Credit Card expiration date validation
        }
        break;
    case eGEN_Dob_TumblersMonth:
    case eGEN_Dob_TumblersDay:
    case eGEN_Dob_TumblersYear:
        {

        }
        break;
    case eACT_Credit04: // we do need to check that the date is before today.
        {

        }
        break;
	}


    return hr;
}




HRESULT Accounts::CopyString( TCHAR** pWhere, const TCHAR* pCopyMe )
{
    if ( *pWhere )
    {
        delete [] *pWhere;
        *pWhere = NULL;
    }

    if ( pCopyMe )
    {
        *pWhere = new TCHAR[_tcslen(pCopyMe)+1];
        if ( !*pWhere )
        {
            return XBAPPERR_MEMORYERR;
        }
        _tcscpy( *pWhere, pCopyMe );
    }

    return S_OK;
}

//*************************************************************************************************
// this will return XBMSG_NEED_WAITCURSOR, if there is
// a task pump involved, if so, pass UpdateAccountInfo 
// into the wait cursor dlg object
//*************************************************************************************************
HRESULT  Accounts::SetValue( eButtonId eField, const TCHAR* pNewValue )
{
    HRESULT hr = S_OK;

    int index = 0;
    while( s_pBindData[index].buttonId != eNullButtonId )
    {
        if ( s_pBindData[index].buttonId == eField )
        {
            wcsncpy( s_pBindData[index].pAddress, pNewValue, s_pBindData[index].maxChars );
            return hr;
        }
        index++;
    }

    return E_FAIL;
}

HRESULT  Accounts::SetValue(eButtonId eField, const FILETIME* pNewValue)
{
    HRESULT hr = S_OK;

    switch (eField)
    {
    case eGEN_Exp_TumblersMonth:
    case eGEN_Exp_TumblersYear:
        {
            // Credit Card Expiration Tumbler
            m_NewAccountInfo.m_Data.ftCardExpiration = *pNewValue;
            return S_OK;
        }
    case eGEN_Dob_TumblersMonth:
    case eGEN_Dob_TumblersDay:
    case eGEN_Dob_TumblersYear:
        {
            // Date of Birth Tumbler
            m_NewAccountInfo.m_ftDOB = *pNewValue;
            return S_OK;
        }
    }

    return E_FAIL;
}

//*************************************************************************************************
// this will return XBMSG_NEED_WAITCURSOR, if there is
// a task pump involved, if so, pass UpdateAccountInfo 
// into the wait cursor dlg object
//*************************************************************************************************
HRESULT         Accounts::EnableButton( eButtonId eID  )
{
    HRESULT hr = S_OK;

    // figure out which button needs to do what...
    switch (eID)
    {
        // Account ID Screen
        case eACT_Idname: // always enable
            {
                break;
            }

        case eACT_Idtag:  // disable until we have a name
            {
                if ( 0 == wcslen( m_NewAccountInfo.m_Data.wszNickname ) )
                    hr = E_FAIL;
                break;
            }
        case eACT_IdDOB: // disable until Tag is filled in
            {
                if ( 0 == wcslen( m_NewAccountInfo.m_Data.wszKingdom ) )
                    hr = E_FAIL;
                break;
            }
        case eACT_Idcontinue:  // disable until we have a birthday
            {
                // disable until we have a real birthday
                if ( 0 == m_NewAccountInfo.m_ftDOB.dwHighDateTime &&
                     0 == m_NewAccountInfo.m_ftDOB.dwLowDateTime )
                    hr = E_FAIL;
                break;
            }

        // Account Credit Card Info Screen
/*        case eACT_Credit01: // always enable
            {
                break;
            }
        case eACT_Credit02: // disable until a credit card is choosen
            {
                if ( 0 != m_NewAccountInfo.m_Data.bCardTypeId )
                    hr = E_FAIL;
                break;
            }
        case eACT_Credit03: // disable until the user gives us their name
            {
                if ( 0 == wcslen( m_NewAccountInfo.m_Data.wszCardHolder ) )
                    hr = E_FAIL;
                break;
            }
*/
    }

    return hr;
}


//*************************************************************************************************
// Helper function, just to set wether we have a task pump going
//*************************************************************************************************
HRESULT Accounts::BeginTask(DWORD nTaskID, const TCHAR* pNewValue )
{
   // need to make sure the handle isn't in use,
    // I can't imagine that it is, but...

    HRESULT hr = S_OK;
    ASSERT( !m_bHaveTask );

    if ( m_bHaveTask )
        return -1; 

    switch( nTaskID )
    {
        case TASK_ALT_NAMES:
            hr = _XOnlineReserveName( m_NewAccountInfo.m_Data.wszNickname,
                                      m_NewAccountInfo.m_Data.wszKingdom,
                                      ALTERNATE_NAME_COUNT,
                                      m_CountryID,
                                      NULL,
                                      &m_hTask );
            break;
        
        case TASK_CREATE_ACCOUNT:
            hr = _XOnlineCreateAccount( &m_NewAccountInfo.m_Data, NULL, &m_hTask);
            break;

        case TASK_BEGIN_TAGS:
            hr = BeginGetTags( pNewValue );
            break;


        default:
            ASSERT( !"Accounts::BeginTask couldn't resolve the task\n" );
            break;
    }



    if ( SUCCEEDED( hr ) || hr == XBMSG_NEED_WAITCURSOR )
    {
        m_bHaveTask = true;
        m_nCurTask = (char)nTaskID;
        hr = XBMSG_NEED_WAITCURSOR; // tell 'em to wait for an answer
    }
    else
    {
        DbgPrint( "Accounts::BeginTask Couldn't start up the work pump\n" );
    }   

    return hr;

}

//*************************************************************************************************
// Call after you're finished pumping
//*************************************************************************************************
HRESULT Accounts::EndTask(DWORD nTaskID )
{
    ASSERT( m_bHaveTask );

    m_bHaveTask = false;

    HRESULT hr = S_OK;

    switch( nTaskID )
    {

        case TASK_ALT_NAMES:
            {
                hr = EndAlternateNames();
            }
          break;
        
        case TASK_CREATE_ACCOUNT:
            {
                hr = _XOnlineGetCreateAccountResults(m_hTask, &m_NewAccountInfo.m_User);
                if (FAILED(hr))    
                {
                    DbgPrint( "Accounts::EndTask temp create account failed\n" );
                }
                else 
                    hr = S_OK;

                XOnlineTaskClose( m_hTask );
            }
            break;

        case TASK_BEGIN_TAGS: // needs to be done from setValue
            hr = EndGetTags(NULL ); // closes handle for us

          break;
    }

    return hr;
}


//*************************************************************************************************
// helper function to setup the tag task pump
//*************************************************************************************************
HRESULT Accounts::BeginGetTags( const TCHAR* pName )
{
    HRESULT  hr  = S_OK;

     // Get the info from the server
    // HACK, 103 = US for now
    hr = _XOnlineGetTags(m_CountryID, TAG_COUNT, NULL, NULL, &m_hTask);

    if (FAILED(hr))
    {
         ASSERT( "Accounts::GetTags Couldn't begin the XOnlineGetTags account task pump\n" );
         return hr;
    }

    return XBMSG_NEED_WAITCURSOR;
   
}

//*************************************************************************************************
// Call after you're finished pumping
//*************************************************************************************************
HRESULT Accounts::EndGetTags( const TCHAR* pName )
{

    HRESULT hr = S_OK;

    LPWSTR pTags[TAG_COUNT];
    for ( int i = 0; i < TAG_COUNT; i++ )
    {
        pTags[i] = NULL;

    }
    WORD    nTagCount = TAG_COUNT;

    hr = _XOnlineGetTagsResults( m_hTask, pTags, &nTagCount );


    // YEAH, it worked!
    if ( SUCCEEDED( hr ) )
    {
        for ( int i = 0; i < nTagCount; i++ )
        {
            if ( pTags[i] )
            {
                TCHAR* pChar = new TCHAR[_tcslen( pTags[i] ) +1];
                if ( !pChar )
                    return XBAPPERR_MEMORYERR;
        
                _tcscpy( pChar, pTags[i] );
        
                m_NewAccountInfo.m_TagList.push_back( pChar );
            }
        }

        // this should be removed after beta, for now we default to the one and only tag
        if ( m_NewAccountInfo.m_TagList.size() )
        {
            wcsncpy( m_NewAccountInfo.m_Data.wszKingdom, 
                m_NewAccountInfo.m_TagList.front(), XONLINE_KINGDOM_SIZE );
        }

       // close old task, start a new one
       XOnlineTaskClose(m_hTask);
    

    }
    else  if ( hr == XONLINE_S_ACCOUNTS_NAME_TAKEN || hr == XONLINE_E_ACCOUNTS_INVALID_USER )
    // get some other names to try
    {
        XOnlineTaskClose(m_hTask);
        hr = BeginTask( TASK_ALT_NAMES, pName );
        // nullify the name
        m_NewAccountInfo.m_Data.wszNickname[0] = 0;
    }

    return hr;
}

HRESULT Accounts::EndAlternateNames( )
{
    PUSER_XNAME altNames = NULL;
    DWORD wordCount = 0;

    HRESULT hr = _XOnlineGetReserveNameResults( m_hTask, &altNames, &wordCount );
    if (FAILED(hr))    
    {
        DbgPrint( "Accounts::EndAlternate names failed to get the names back from the server\n" );
    }
    else // store new names locally
    { 
        for ( DWORD i = 0; i < wordCount; i++ )
        {
            TCHAR* pChar = new TCHAR[ _tcslen( altNames[i].wszNickname ) + 1 ];
            if ( !pChar )
                return XBAPPERR_MEMORYERR;
        
            _tcscpy( pChar, altNames[i].wszNickname );
        
            m_NewAccountInfo.m_AltNameList.push_back( pChar );
        }
    }

    XOnlineTaskClose( m_hTask );

    // tell 'em to press the alternate names button
    return hr;
}



//*************************************************************************************************
HRESULT Accounts::FrameMove()
{
    HRESULT hr = XBMSG_NEED_WAITCURSOR;

    // if we have a work pump flowing, run with it
    if ( m_bHaveTask )
    {
        hr = XOnlineTaskContinue(m_hTask);

        if (FAILED(hr))
        {
             ASSERT( !"Accounts::FrameMove Couldn't resume the task pump\n" );
             EndTask( m_nCurTask ); // need to clean up anyway
        }

        else if ( (hr) != XONLINETASK_S_RUNNING )
        {
            hr = EndTask( m_nCurTask );
            // do something here
            hr = XBMSG_OK;
          
        }
    }
    else
        hr = S_OK;

    return hr;

}

//*************************************************************************************************
void    Accounts::CleanUpNewAccountData()
{
    for ( list<TCHAR*>::iterator iter = m_NewAccountInfo.m_AltNameList.begin();
        iter != m_NewAccountInfo.m_AltNameList.end(); iter++ )
        {
            delete *iter;
        }

    m_NewAccountInfo.m_AltNameList.clear();

     for ( list<TCHAR*>::iterator iter = m_NewAccountInfo.m_TagList.begin();
        iter != m_NewAccountInfo.m_TagList.end(); iter++ )
        {
            delete *iter;
        }

    m_NewAccountInfo.m_TagList.clear();

    memset( &m_NewAccountInfo.m_Data, 0, sizeof( m_NewAccountInfo.m_Data ) );

}

//*************************************************************************************************
//  Local function to help create TCHAR memory and copy a string
//*************************************************************************************************
TCHAR* CreateUnicodeStr(const TCHAR *szInputStr)
{
    TCHAR *szDestStr = new TCHAR[wcslen(szInputStr) + 1];
    wcscpy(szDestStr, szInputStr);
    return szDestStr;
}


//*************************************************************************************************
//  Call this function to fill your listbox with the appropriate items from the server
//*************************************************************************************************
HRESULT Accounts::SetListBoxItems(eSceneId eCurSceneId, TCHAR*** ppszListItems, TCHAR*** ppszListHelp, int* nNumItems )
{
    HRESULT hr = S_OK;  

    switch(eCurSceneId)
    {
    case eACT_CountryId:
        {
            // These are placeholder credit cards.
            *ppszListItems = new TCHAR*[4];
            *ppszListHelp = new TCHAR*[4];
            TCHAR** pArray      = *ppszListItems;
            TCHAR** pHelpArray  = *ppszListHelp;

            pArray[0]     = CreateUnicodeStr(L"American Express");
            pHelpArray[0] = CreateUnicodeStr(L"HELP_NULL");
            pArray[1]     = CreateUnicodeStr(L"MasterCard");
            pHelpArray[1] = CreateUnicodeStr(L"HELP_NULL");
            pArray[2]     = CreateUnicodeStr(L"Visa");
            pHelpArray[2] = CreateUnicodeStr(L"HELP_NULL");
            pArray[3]     = CreateUnicodeStr(L"Discover");
            pHelpArray[3] = CreateUnicodeStr(L"HELP_NULL");
            *nNumItems = 4;
        break;
        }
    case eACT_Billing_CountryId:
        {
            // These are placeholder countries.
            *ppszListItems = new TCHAR*[2];
            *ppszListHelp = new TCHAR*[2];
            TCHAR** pArray      = *ppszListItems;
            TCHAR** pHelpArray  = *ppszListHelp;

            pArray[0]     = CreateUnicodeStr(L"United States");
            pHelpArray[0] = CreateUnicodeStr(L"HELP_NULL");
            pArray[1]     = CreateUnicodeStr(L"Canada");
            pHelpArray[1] = CreateUnicodeStr(L"HELP_NULL");
            *nNumItems = 2;
        break;
        }
    default:

        break;
    }

    return hr;
}


HRESULT Accounts::SetListBoxValue( eSceneId eCurSceneId, TCHAR* pNewValue )
{
    HRESULT hr = S_OK;  

    switch(eCurSceneId)
    {
    case eACT_CountryId:
        {
            // Credit Card Name screen (Amex, Mastercard, etc.)
            //wcscpy( m_NewAccountInfo.m_Data.wszCardType, (pNewValue) );
            // this is now bCardTypeId
        break;
        }
    case eACT_Billing_CountryId:
        {
            // User's Billing Country

            // BUGBUG: need to load a table with countries & country codes
            // Currently defaulting to 103 - United States
            m_NewAccountInfo.m_Data.bCountryId = 0x67;
        break;
        }
    default:

        break;
    }

    return hr;
}


void Accounts::ClearNewAccountData( void )
{
    memset( &m_NewAccountInfo.m_Data, 0, sizeof( m_NewAccountInfo.m_Data ) );
    memset( &m_NewAccountInfo.m_User, 0, sizeof( m_NewAccountInfo.m_User ) );
    
    // Initalize Dates and Times to 0
    m_NewAccountInfo.m_Data.ftCardExpiration.dwHighDateTime = 0;
    m_NewAccountInfo.m_Data.ftCardExpiration.dwLowDateTime  = 0;
    m_NewAccountInfo.m_ftDOB.dwHighDateTime = 0;
    m_NewAccountInfo.m_ftDOB.dwLowDateTime  = 0;

    m_NewAccountInfo.m_TagList.clear();
    m_NewAccountInfo.m_AltNameList.clear();
}

//*************************************************************************************************
// end of file ( AccountInfo.cpp )
