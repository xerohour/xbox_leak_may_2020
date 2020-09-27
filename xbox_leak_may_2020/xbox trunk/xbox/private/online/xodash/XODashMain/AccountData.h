#ifndef ACCOUNTDATA_H
#define ACCOUNTDATA_H
/*************************************************************************************************\
AccountData.h		: Interface for all account and billing info
Creation Date		: 1/23/2002 6:31:00 PM
Library				: .lib
Copyright Notice	: (C) 2000 Microsoft
Author				: Heidi Gaertner
//-------------------------------------------------------------------------------------------------
Notes				: 
\*************************************************************************************************/

//*************************************************************************************************
#include "xonline.h"
#include <list>
#include "XOConst.h"
#include "SmartObject.h"
#include "Date.h"
#include "Translator.h"

using namespace std;

#define TASK_ALT_NAMES 1
#define TASK_CREATE_ACCOUNT 2
#define TASK_BEGIN_TAGS 3

#define MAX_ACCOUNT_STRLEN 64
/**************************************************************************************************
CLASS DESCRIPTION
AccountInfo:  This structure holds the basic for accounts
**************************************************************************************************/

struct AccountInfo // the basics
{
    XONLINE_USER		m_User;   // might have no XName if this is an owner account
 	char				m_cLocation; //  -1 == in process of creation...0 == HD, 1-8 = MU slots .... could be more if ever a splitter.

};

/**************************************************************************************************
CLASS DESCRIPTION
UserAccountInfo:  This is the class for user accounts, these have their own xname and stuff, but 
                there might be multiple of these tied to a single credit card
**************************************************************************************************/
struct UserAccountInfo : public AccountInfo// user Accounts
{
    DWORD               m_dwPurchasingPower; // might as well be bit field
    DWORD               m_dwParentalControls;
    TCHAR*              m_szPassport;
    BYTE                m_bPassportCanBeChanged; // could look for XBox.com in the passport itself

};

/**************************************************************************************************
CLASS DESCRIPTION
CreditCardInfo:  Everything you need to define a credit card
**************************************************************************************************/
struct CreditCardInfo
{
    TCHAR*				m_szType;
    TCHAR*				m_szCardholder;
    TCHAR*              m_szNumber;// only last 4 digits
    TCHAR*              m_szBillingAddress;
    TCHAR*              m_szCity;
    TCHAR*              m_szState;
    TCHAR*              m_szZip;
    SYSTEMTIME          m_ExpirationDate;
    TCHAR*              m_szCountryDisplayName;

};

/**************************************************************************************************
CLASS DESCRIPTION
OwnerAccountInfo:  This is the account that gets stuck paying for all the various user accounts,
                    if the account is already created, you probably don't get ALL the credit card
                    info
**************************************************************************************************/
struct OwnerAccountInfo : public AccountInfo // owner accounts
{
    WORD                m_localeID;
    CreditCardInfo      m_CardInfo;
    TCHAR*              m_pPassport;
};

struct NewAccountInfo
{
    NewAccountInfo();
    USER_ACCOUNT_INFO               m_Data;
    FILETIME                        m_ftDOB;
    list< TCHAR* >                  m_TagList;
    list< TCHAR* >                  m_AltNameList;
    XONLINE_USER                    m_User;

};

/**************************************************************************************************
CLASS DESCRIPTION
NewAccountInfo:  The info you need to create a new account
**************************************************************************************************/
class Accounts
{
public:

    // call this BEFORE you do anything else
    static HRESULT Initialize();
    static HRESULT CleanUp();

    static Accounts* Instance();
    static HRESULT  UpdateAccountInfo();

    const TCHAR*    GetValue(eButtonId eField);
    unsigned int    GetNumericValue(eButtonId eField);

    // this will return XBMSG_NEED_WAITCURSOR, if there is
    // a task pump involved, if so, pass UpdateAccountInfo 
    // into the wait cursor dlg object
    HRESULT         Validate( eButtonId eField, const TCHAR*    pNewValue );
    HRESULT         Validate( eButtonId eField, const FILETIME* pNewValue );

    // make sure you call Validate before SetValue
    HRESULT         SetValue(eButtonId eField, const TCHAR*    pNewValue);
    HRESULT         SetValue(eButtonId eField, const FILETIME* pNewValue);

    HRESULT         FrameMove();     // update task pumps etc, called from UpdateAccountInfo

    HRESULT         SetListBoxItems( eSceneId eCurSceneId, TCHAR*** ppszListItems, TCHAR*** ppszListHelp, int* nNumItems );
    HRESULT         SetListBoxValue( eSceneId eCurSceneId, TCHAR* pNewValue );

    HRESULT         EnableButton( eButtonId eID  );

    void            ClearNewAccountData( void );

  
private:

    Accounts();
    ~Accounts();

    struct ACT_BINDING_DATA
    {
        eButtonId   buttonId;
        TCHAR*      pAddress;
        long        maxChars;
    };

    static ACT_BINDING_DATA   s_pBindData[];


    HRESULT     BeginTask(DWORD nTaskID, const TCHAR* pValue );
    HRESULT     EndTask(DWORD  nTaskID);

    void        CleanUpNewAccountData();
    static HRESULT     CopyString( TCHAR** pWhere, const TCHAR* pCopyMe );


    static Accounts*               s_pAccounts;

    
    NewAccountInfo                 m_NewAccountInfo;
    WORD                           m_CountryID;

    
    // for task pumping purposes
    XONLINETASK_HANDLE             m_hTask;
    bool                           m_bHaveTask;
    char                           m_nCurTask;
    
    // HELPERS
    HRESULT     BeginGetTags( const TCHAR* pName);
    HRESULT     EndGetTags( const TCHAR* pName );
    HRESULT     EndAlternateNames( );
};
 



//*************************************************************************************************
#endif  // end of file ( AccountData.h )
