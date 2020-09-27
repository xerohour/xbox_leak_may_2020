#include <XOnlineAuthUtil.h>
#include <CXLiveUserManager.h>
#include <CXLiveConnectionManager.h>
#include <OnlineAccounts.h>

using namespace std;

using XLive::CXLiveConnectionManager;

namespace XLive
{



CXLiveUserManager::~CXLiveUserManager()
{
}



//
//////////// Private functions /////////////
//


void CXLiveUserManager::ComputeAccounts(void)
{
    // Compute and store the location of the first account.
    m_pFirstAccount = find_if(m_pStart, m_pEnd, XLive::IsAccount());

    // Compute and store the location of the last account.
    if (m_pFirstAccount == m_pEnd)
    {
        m_pLastAccount = m_pFirstAccount = NULL;
        return;
    }

    XONLINE_USER *pCurrent = m_pFirstAccount;
    for (; pCurrent != m_pEnd; ++pCurrent)
    {
        if (IsAccount(pCurrent))
        {
            m_pLastAccount = pCurrent;
        }
    }
}

void CXLiveUserManager::ComputeMUAccounts(void)
{
    // Compute and store the location of the first MU account.
    m_pFirstMUAccount = find_if(m_pStart, m_pEnd, XLive::IsMUAccount());

    // Compute and store the location of the last MU account.
    if (m_pFirstMUAccount == m_pEnd)
    {
        m_pLastMUAccount = m_pFirstMUAccount = NULL;
        return;
    }

    XONLINE_USER *pCurrent = m_pFirstMUAccount;
    for (; pCurrent != m_pEnd; ++pCurrent)
    {
        if (IsMUAccount(pCurrent))
        {
            m_pLastMUAccount = pCurrent;
        }
    }
}

void CXLiveUserManager::ComputeHDAccounts(void)
{
    // Compute and store the location of the first HD account.
    m_pFirstHDAccount = find_if(m_pStart, m_pEnd, XLive::IsHDAccount());

    // Compute and store the location of the last HD account.
    if (m_pFirstHDAccount == m_pEnd)
    {
        m_pLastHDAccount = m_pFirstHDAccount = NULL;
        return;
    }

    XONLINE_USER *pCurrent = m_pFirstHDAccount;
    for (; pCurrent != m_pEnd; ++pCurrent)
    {
        if (IsHDAccount(pCurrent))
        {
            m_pLastHDAccount = pCurrent;
        }
    }
}

bool CXLiveUserManager::VerifyNoDuplicates() const
{
    const_AccountIterator it = Account_begin();
    const_AccountIterator end1 = Account_end();
    --end1;
    for (; it != end1; ++it)
    {
        const_AccountIterator it2 = it;
        for (++it2; it2 != Account_end(); ++it2)
        {
            if (*it == *it2)
            {
                // Found a duplicate.
                return false;
            }
        }
    }
    return true;
}

HRESULT CXLiveUserManager::PopulateMUs(DWORD *pCount)
{
    ASSERT(NULL != pCount);

    HRESULT hr = S_OK;
    static char chDrives[ 8 ] = {
        'F', 'G', 'H', 'I',
        'J', 'K', 'L', 'M'
    };
    *pCount = 0;    // Don't depend on the caller to init.
    XONLINEP_USER user;
    XONLINEP_USER aIniUsers[XONLINE_MAX_STORED_ONLINE_USERS];
    
    static DWORD dwServices[] = {
            XONLINE_USER_ACCOUNT_SERVICE
    };
    DWORD cServices = sizeof(dwServices) / sizeof(dwServices[0]);
    
    // zero out the users
    ZeroMemory( aIniUsers, sizeof( aIniUsers ) );
    
    // Note: The test harness has already mounted the MUs.
    // This means that we cannot mount them here.

    // Make sure that there is at least one MU.
    DWORD dwMUs = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);
    Sleep(1000);       // Wait for all MUs to enumerate.
    dwMUs = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);

    if (dwMUs > 0)
    {
        // Logon with 0 users just to create a connection.
        CHECKRUN(CXLiveConnectionManager::Instance().SelectServices(dwServices, cServices));
        CHECKRUN(CXLiveConnectionManager::Instance().LogonAndWaitForConnection((PXONLINE_USER) aIniUsers));

        // Try to put an account in every mounted MU.
        for (size_t i = 0; i < sizeof(chDrives) / sizeof(char) && SUCCEEDED(hr); ++i)
        {
            // Cause a random name, kingdom, etc. to be generated.
            ZeroMemory(&user, sizeof(user));
            
            // Add a user to this MU.
            // This call will fail if this MU does not exist.  We accept this
            // type of failure.
            hr = COnlineAccounts::Instance()->PopulateUserAccountsMU(chDrives[ i ], &user);
            if (SUCCEEDED(hr))
            {
                // We count the number of accounts successfully created.
                *pCount += 1;
            }
            else
            {
                // This MU does not exist.  That's OK.
                hr = S_OK;
            }
        }
        CHECKRUN(CXLiveConnectionManager::Instance().Logoff());
    }

    hr = (SUCCEEDED(hr) && *pCount > 0) ? S_OK : E_FAIL;

    return hr;
}

HRESULT CXLiveUserManager::CreateAndSelectGuestAccount(const UserAccountIterator it, int guestNumber)
{
    if (m_numberOfSelectedAccounts >= XONLINE_MAX_LOGON_USERS)
    {
        return E_INVALIDARG;
    }

    // Copy the master account to the selected account array.
    m_selectedAccounts[ m_numberOfSelectedAccounts ] = *it;

    // Set the guest flag.  
    m_selectedAccounts[ m_numberOfSelectedAccounts ].xuid.dwUserFlags |= 
                        (guestNumber & XONLINE_USER_GUEST_MASK);

    ++m_numberOfSelectedAccounts;

    return S_OK;
}

HRESULT CXLiveUserManager::VerifyLogon(XONLINE_USER const *pLoggedUsers) const
{
    HRESULT hr = S_OK;

    if (NULL == pLoggedUsers)
    {
        hr = E_FAIL;
    }

    for (size_t i = 0; i < m_numberOfSelectedAccounts && SUCCEEDED(hr); ++i)
    {
        if (pLoggedUsers[i].xuid == 0)
        {
            // Skip empty slots.
            continue;
        }

        // Verify the HRESULT for this account.
        hr = pLoggedUsers[i].hr;

        // Verify the XUIDs.
        if (SUCCEEDED(hr))
        {
            hr = (pLoggedUsers[i].xuid == m_selectedAccounts[i].xuid) ? S_OK : E_FAIL;
        }

        // Verify the index.
        if (SUCCEEDED(hr))
        {
            int index = pLoggedUsers[i].index;
            hr = (index == i) ? S_OK : E_FAIL;
        }
    }

    return hr;
}

HRESULT CXLiveUserManager::GenerateAndSelectRandomAccounts( int hdMasterAccounts,
                                                            int hdGuestAccounts,
                                                            int muMasterAccounts,
                                                            int muGuestAccounts)
{
    HRESULT             hr              = S_OK;
    DWORD				dwUsers		= 0;
    DWORD               dwMUs = 0;

    // Make sure they didn't ask for too many accounts to be selected.
    if (SUCCEEDED(hr))
    {
        if (hdMasterAccounts + hdGuestAccounts + muMasterAccounts + muGuestAccounts > XONLINE_MAX_LOGON_USERS)
        {
            hr = E_INVALIDARG;
        }
    }

    // Make sure there is a master for every guest account.
    if (SUCCEEDED(hr))
    {
        if (hdGuestAccounts > 0 && hdMasterAccounts < 1 ||
           (muGuestAccounts > 0 && muMasterAccounts < 1))
        {
            hr = E_INVALIDARG;
        }
    }

    if (SUCCEEDED(hr))
    {
        // Delete any existing user accounts on the HD before populating.
        hr = COnlineAccounts::Instance()->PopulateUserAccountsHD( NULL, XONLINE_MAX_HD_ONLINE_USERS, true );
    }
    
    if (SUCCEEDED(hr))
    {
        // Create an account in every MU.
        hr = PopulateMUs(&dwMUs);
        hr = (SUCCEEDED(hr) && dwMUs > 0) ? S_OK : E_FAIL;
    }
        
    // Get the users.
    CHECKRUN(XOnlineGetUsers(m_users, &dwUsers));
    CHECKRUN((XONLINE_MAX_HD_ONLINE_USERS + dwMUs == dwUsers && SUCCEEDED(hr)) ? hr : E_FAIL);

    // Generate the indexes.  This is required for the iterators to work.
    ComputeAccounts();
    ComputeHDAccounts();
    ComputeMUAccounts();
    
    // Verify that there are the correct number of users.
    CHECKRUN((CountMUAccounts() + CountHDAccounts() == dwUsers && SUCCEEDED(hr)) ? hr : E_FAIL);
    CHECKRUN((CountAccounts() == dwUsers) ? hr : E_FAIL);

    // Verify that there are enough MUs.
    CHECKRUN(((int)dwMUs >= muMasterAccounts + muGuestAccounts) ? S_OK : E_INVALIDARG);

    // Verify that there are no duplicates.
    CHECKRUN((VerifyNoDuplicates()) ? S_OK : E_FAIL);
    
    // Clear account selections.
    CHECKRUN(ClearAccountSelections());

    // Select some HD master accounts to logon.
    HDAccountIterator hdIt = HDAccount_begin();
    if (SUCCEEDED(hr))
    {
        for (int i = 0; hdIt != HDAccount_end() && (i < hdMasterAccounts) && SUCCEEDED(hr); ++i)
        {
            hr = SelectAccount(hdIt++);
        }
    }
    
    // Select some HD guest accounts to logon.
    if (SUCCEEDED(hr))
    {
        for (int i = 0; (i < hdGuestAccounts) && SUCCEEDED(hr); ++i)
        {
            CHECKRUN(CreateAndSelectGuestAccount(hdIt, i));
        }
    }
    
    // Select some MU master accounts to logon.
    MUAccountIterator muIt = MUAccount_begin();
    if (SUCCEEDED(hr))
    {
        for (int i = 0; muIt != MUAccount_end() && (i < muMasterAccounts) && SUCCEEDED(hr); ++i)
        {
            hr = SelectAccount(muIt++);
        }
    }
    
    // Select some MU guest accounts to logon.
    if (SUCCEEDED(hr))
    {
        for (int i = 0; (i < muGuestAccounts) && SUCCEEDED(hr); ++i)
        {
            CHECKRUN(CreateAndSelectGuestAccount(muIt, i));
        }
    }
    
    return hr;
}


} // namespace XLive

