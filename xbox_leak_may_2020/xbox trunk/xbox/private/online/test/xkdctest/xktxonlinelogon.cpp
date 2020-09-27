#include "xkdctest.h"
#include <stdlib.h>
#include "xktxonlinelogon.h"
#include "accounts.h"
#include "ticketcache.h"


HRESULT XKTXOnlineLogon::VerifyUserHRs(
    IN Config &cfg, 
    IN char *szSectionName, 
    IN XONLINEP_USER *rgUsers)
{
    HRESULT hr = S_OK;

    if (cfg.getStr(szSectionName, "userhr"))
    {
        for (INT i = 0; i < XONLINE_MAX_LOGON_USERS; i++)
        {
            HRESULT hrExpected = E_INVALIDARG;
            char *szhrUser = cfg.getStrListItem(szSectionName, "userhr", i);
            if (!strcmp(szhrUser, "XONLINE_E_LOGON_USER_ACCOUNT_INVALID"))
            {
                hrExpected = XONLINE_E_LOGON_USER_ACCOUNT_INVALID;
            }
            else if (!strcmp(szhrUser, "S_OK"))
            {
                hrExpected = S_OK;
            }

            if (hrExpected == S_OK && rgUsers[i].hr == XONLINE_E_LOGON_USER_ACCOUNT_INVALID)
            {
                hr = XONLINE_E_LOGON_USER_ACCOUNT_INVALID;
            }
            else if (hrExpected == XONLINE_E_LOGON_USER_ACCOUNT_INVALID && rgUsers[i].hr == S_OK)
            {
                hr = E_FAIL;
            }
        }
    }
    else
    {
        for (INT i = 0; i < XONLINE_MAX_LOGON_USERS; i++)
        {
            if (rgUsers[i].hr != S_OK)
            {
                hr = XONLINE_E_LOGON_USER_ACCOUNT_INVALID;
            }
        }
    }

    return hr;
}


HRESULT XKTXOnlineLogon::VerifyServices(
    IN Config &cfg, 
    IN char *szSectionName, 
    IN XKERB_SERVICE_CONTEXT *pKerbServiceContext)
{
    HRESULT hr = E_FAIL;

    DWORD rgdwServices[XONLINE_MAX_NUMBER_SERVICE];
    HRESULT rghrServices[XONLINE_MAX_NUMBER_SERVICE];
    WORD rgwServicesPort[XONLINE_MAX_NUMBER_SERVICE];
    memcpy(rgdwServices, pKerbServiceContext->dwServiceID, sizeof(DWORD) * XONLINE_MAX_NUMBER_SERVICE);
    memcpy(rghrServices, pKerbServiceContext->hrService, sizeof(HRESULT) * XONLINE_MAX_NUMBER_SERVICE);
    memcpy(rgwServicesPort, pKerbServiceContext->wServicePort, sizeof(WORD) * XONLINE_MAX_NUMBER_SERVICE);

    DWORD rgdwServicesGranted[XONLINE_MAX_NUMBER_SERVICE];
    HRESULT rghrServicesGranted[XONLINE_MAX_NUMBER_SERVICE];
    DWORD rgdwServicesGrantedIp[XONLINE_MAX_NUMBER_SERVICE];
    WORD rgwServicesGrantedPort[XONLINE_MAX_NUMBER_SERVICE];
    memset(rgdwServicesGranted, 0, sizeof(DWORD) * XONLINE_MAX_NUMBER_SERVICE);
    memset(rghrServicesGranted, 0, sizeof(HRESULT) * XONLINE_MAX_NUMBER_SERVICE);
    memset(rgdwServicesGrantedIp, 0, sizeof(DWORD) * XONLINE_MAX_NUMBER_SERVICE);
    memset(rgwServicesGrantedPort, 0, sizeof(WORD) * XONLINE_MAX_NUMBER_SERVICE);

    char **rgszServicesRequested = NULL;
    INT cServicesRequested;
    BOOL fServicesRequestedFound = cfg.getStrList(szSectionName, "services", &rgszServicesRequested, cServicesRequested);
    Assert(fServicesRequestedFound && cServicesRequested >= 0 && cServicesRequested <= (INT)XONLINE_MAX_NUMBER_SERVICE);
    Assert((DWORD)cServicesRequested == pKerbServiceContext->dwNumServices);
    char **rgszhrServicesGranted = NULL;
    INT cServicesGranted;
    BOOL fServicesGrantedFound = cfg.getStrList(szSectionName, "granted", &rgszhrServicesGranted, cServicesGranted);
    Assert(cServicesGranted >= 0 && cServicesGranted <= (INT)XONLINE_MAX_NUMBER_SERVICE);
    Assert(cServicesRequested == cServicesGranted || !fServicesGrantedFound);

    for (INT i = 0; i < cServicesRequested; i++)
    {
        rgdwServicesGranted[i] = cfg.getIntListItem("services", rgszServicesRequested[i], 0);
        char *szSiteIndex = cfg.getStrListItem("services", rgszServicesRequested[i], 1);
        rgdwServicesGrantedIp[i] = cfg.getIP("sites", szSiteIndex);
        rgwServicesGrantedPort[i] = (WORD)cfg.getIntListItem("services", rgszServicesRequested[i], 2);

        if (fServicesGrantedFound)
        {
            if (!strcmp(rgszhrServicesGranted[i], "yes"))
            {
                rghrServicesGranted[i] = S_OK;
            }
            else if (!strcmp(rgszhrServicesGranted[i], "no"))
            {
                rghrServicesGranted[i] = XONLINE_E_LOGON_SERVICE_NOT_AUTHORIZED;
            }
            else if (!strcmp(rgszhrServicesGranted[i], "nonexistant"))
            {
                rghrServicesGranted[i] = XONLINE_E_INTERNAL_ERROR;
            }
            else
            {
                Assert(FALSE);
            }
        }
        else
        {
            rghrServicesGranted[i] = S_OK;
        }
    }

    hr = S_OK;
    BOOL fRealTicket = FALSE;

    for (i = 0; i < cServicesRequested; i++)
    {
        Assert(rgdwServices[i] == rgdwServicesGranted[i]);
        TA(rghrServices[i] == rghrServicesGranted[i]);
        if (rghrServices[i] == S_OK)
        {
            TA(rgdwServicesGrantedIp[i] == pKerbServiceContext->siteIPAddress.S_un.S_addr);
            TA(rgwServicesPort[i] == rgwServicesGrantedPort[i]);
            fRealTicket = TRUE;
        }
        else
        {
            TA(rgwServicesPort[i] == 0);
        }
    }

    //!! can we check the ip and other info for a 'fake' ticket?
    if (!fRealTicket)
    {
        TA(pKerbServiceContext->siteIPAddress.S_un.S_addr == 0);
    }

    delete rgszServicesRequested;

    delete rgszhrServicesGranted;

    return hr;
}


HRESULT XKTXOnlineLogon::SignInServices(
    IN CXoTest *pXo, 
    IN Config &cfg, 
    IN char *szSectionName,
    IN XONLINEP_USER *rgUsers,
    IN INT cUsers,
    IN XKERB_TGT_CONTEXT *ptkt)
{
    HRESULT hr = S_OK;

    DWORD dwXkdcIp = cfg.getIP("servers", "xkdc");
    USHORT usXkdcPort = cfg.getPort("servers", "xkdc");

    DWORD rgdwServices[XONLINE_MAX_NUMBER_SERVICE];
    memset(rgdwServices, 0, sizeof(DWORD) * XONLINE_MAX_NUMBER_SERVICE);

    char **rgszServiceIndicators = NULL;
    INT cServices;
    BOOL fServicesFound = cfg.getStrList(szSectionName, "services", &rgszServiceIndicators, cServices);

    TA(cServices >= 0 && cServices <= (INT)XONLINE_MAX_NUMBER_SERVICE);

    // If no services are requested then we can return success without doing any work.
    if (fServicesFound)
    {
        TestMsg(XKTVerbose_Process, "Signing in services...\n");

        char *szExpected = cfg.getStr(szSectionName, "expected");

        NTSTATUS nErrorValue = STATUS_SUCCESS;
        if (szExpected)
        {
            nErrorValue = KerbErrorCompare::GetErrorValue(szExpected);
        }

        char *szTimeSkew = cfg.getStr(szSectionName, "timeskew");
        INT nTimeSkew = 0;
        if (szTimeSkew)
        {
            nTimeSkew = atoi(szTimeSkew);
        }

        // The maximum allowed TimeSkew is 5 minutes to either side of correct
        BOOL fTimeSkewRepeat = nTimeSkew > 300;

        XKERB_SERVICE_CONTEXT KerbServiceContext;
        memset(&KerbServiceContext, 0, sizeof(XKERB_SERVICE_CONTEXT));

        KerbServiceContext.dwTitleID = 0xFFFE0000;

        for (INT i = 0; i < cUsers; i++)
        {
            KerbServiceContext.UserID[i] = rgUsers[i].xuid.qwUserID;
        }

        Assert(cServices <= XONLINE_MAX_NUMBER_SERVICE);
        KerbServiceContext.dwNumServices = cServices;

        for (i = 0; i < XONLINE_MAX_NUMBER_SERVICE; i++)
        {
            KerbServiceContext.dwServiceID[i] = XONLINE_INVALID_SERVICE;
            KerbServiceContext.hrService[i] = S_FALSE;
        }

        for (i = 0; i < cServices; i++)
        {
            KerbServiceContext.dwServiceID[i] = cfg.getIntListItem("services", rgszServiceIndicators[i], 0);
            if (!KerbServiceContext.dwServiceID[i])
            {
                TestMsg(XKTVerbose_All, "Service '%s' not found.\n", rgszServiceIndicators[i]);
                hr = E_INVALIDARG;
                break;
            }
        }

        if (SUCCEEDED(hr))
        {
            NTSTATUS Status = STATUS_SUCCESS;
            for (INT j = 0; j < 2; j++)
            {
                // Send an TGS-REQ to the XKDC to sign in
                hr = pXo->SendAndVerifyTGSRequest(dwXkdcIp, usXkdcPort, rgUsers, ptkt, &KerbServiceContext, Status);
                if (!fTimeSkewRepeat)
                {
                    break;
                }
                else
                {
                    if (SUCCEEDED(hr) || Status != STATUS_TIME_DIFFERENCE_AT_DC)
                    {
                        hr = E_FAIL;
                        break;
                    }
                    else
                    {
                        fTimeSkewRepeat = FALSE;
                    }
                }
            }

            if (nErrorValue != STATUS_SUCCESS)
            {
                if (hr != S_OK)
                {
                    if (Status == nErrorValue)
                    {
                        hr = S_OK;
                    }
                    else
                    {
                        TestMsg(XKTVerbose_All, "Incorrect return value from XKDC.\n");
                    }
                }
                else
                {
                    hr = E_FAIL;
                }
            }
            else
            {
                if (hr == S_OK)
                {
                    hr = VerifyUserHRs(cfg, szSectionName, rgUsers);
                    if (hr == S_OK)
                    {
                        hr = VerifyServices(cfg, szSectionName, &KerbServiceContext);
                    }
                    else if (hr == XONLINE_E_LOGON_USER_ACCOUNT_INVALID)
                    {
                        TestMsg(XKTVerbose_All, "Bad user account.\n");
                    }
                }
                else if (hr == S_FALSE)
                {
                    if (Status == XONLINE_E_LOGON_XBOX_ACCOUNT_INVALID)
                    {
                        TestMsg(XKTVerbose_All, "Bad machine account.\n");
                        hr = XONLINE_E_LOGON_XBOX_ACCOUNT_INVALID;
                    }
                    else
                    {
                        hr = E_FAIL;
                    }
                }
            }
        }
    }

    return hr;
}


HRESULT XKTXOnlineLogon::NewMachine(
    IN CXoTest *pXOn, 
    IN Config &cfg, 
    IN char *szSerial, 
    IN char *szPassword, 
    OUT XONLINEP_USER *pUser)
{
    HRESULT hr = E_FAIL;

    XONLINEP_USER MachineAccount;
    if (!szSerial || !szPassword)
    {
        TestMsg(XKTVerbose_All, "Machine account information is incorrectly formatted.\n");
    }
    else
    {
        if ((hr = MakeAccountWithPassword(&MachineAccount, 1, szSerial, "", "", "macs.xbox.com", szPassword)) != S_OK)
        {
            TestMsg(XKTVerbose_All, "Machine account information is incorrectly formatted.\n");
        }
        else
        {
            XKERB_TGT_CONTEXT tkt;
            RtlZeroMemory(&tkt, sizeof(XKERB_TGT_CONTEXT));

            NTSTATUS Status = 0;
            DWORD dwXmacsIp = cfg.getIP("servers", "xmacs");
            USHORT usXmacsPort = cfg.getPort("servers", "xmacs");

            if ((hr = pXOn->SignInAccount_AS(dwXmacsIp, usXmacsPort, &MachineAccount, FALSE, &tkt, Status)) == S_OK)
            {
                // Fill in the account structure with the newly created account
                if (tkt.pAccountCreationPreAuth)
                {
                    hr = MakeAccountWithKey(
                        pUser, 
                        tkt.pAccountCreationPreAuth->qwUserID, 
                        tkt.pAccountCreationPreAuth->name, 
                        tkt.pAccountCreationPreAuth->kingdom, 
                        tkt.pAccountCreationPreAuth->domain, 
                        tkt.pAccountCreationPreAuth->realm, 
                        tkt.pAccountCreationPreAuth->key);

                    pXOn->SysFree(tkt.pAccountCreationPreAuth);
                }
            }
        }
    }

    return hr;
}

/*
HRESULT XKTXOnlineLogon::NewUser(
    IN CXoTest *pXOn, 
    IN Config &cfg, 
    IN char *szName, 
    OUT XONLINEP_USER *pUser)
{
    HRESULT hr = E_FAIL;

    Assert(szName);

    BYTE bKey[XONLINE_KEY_LENGTH];
    memset(bKey, 0, XONLINE_KEY_LENGTH);
    if ((hr = MakeAccountWithKey(pUser, 0, szName, "Feb2002", "", "", bKey)) != S_OK)
    {
        TestMsg(XKTVerbose_All, "User account information is incorrectly formatted.\n");
    }
    else
    {
        pUser->name[strlen(pUser->name)] = '-';
        for (INT i = strlen(pUser->name); i < XONLINE_NAME_SIZE - 1; i++)
        {
            char chRand = (char)(((float)(rand() * ('z' - 'a'))) / RAND_MAX) + 'a';
            pUser->name[i] = chRand;
        }
        pUser->name[i] = 0;
        hr = pXOn->UacsCreate(cfg.getIP("servers", "uacs"), pUser);
    }

    return hr;
}
*/

//Simulate XOnlineLogon
HRESULT XKTXOnlineLogon::runTest(IN CXoTest *pXOn, IN char *szSectionName, IN Config &cfg)
{
    HRESULT hr = S_OK;

    BOOL fUseCache = !cfg.getBool(szSectionName, "nocache");
    DWORD dwXkdcIp = cfg.getIP("servers", "xkdc");
    USHORT usXkdcPort = cfg.getPort("servers", "xkdc");

    INT cRepeats = max(cfg.getInt(szSectionName, "repeat"), 1);
    for (INT nCurRepeat = 0; nCurRepeat < cRepeats && SUCCEEDED(hr); nCurRepeat++)
    {
        hr = E_FAIL;

        XONLINEP_USER rgUsers[1 + XONLINE_MAX_LOGON_USERS];
        memset(rgUsers, 0, sizeof(XONLINEP_USER) * (1 + XONLINE_MAX_LOGON_USERS));

        char *szUserSection = cfg.getStr("general", "users");

        // Load user accounts from file and put them into rgUsers

        char **rgszUserAccountIndicators = NULL;
        INT cUsers;
        BOOL fUsersFound = cfg.getStrList(szSectionName, "users", &rgszUserAccountIndicators, cUsers);

        hr = S_OK;

        if (cUsers < 0 || cUsers > XONLINE_MAX_LOGON_USERS)
        {
            hr = E_INVALIDARG;
        }
        else
        {
            for (INT i = 0; i < cUsers; i++)
            {
                char *szUserInfo = cfg.getStr(szUserSection, rgszUserAccountIndicators[i]);

                if (!szUserInfo)
                {
                    TestMsg(XKTVerbose_All, "Failed to find user info for '%s'.\n", rgszUserAccountIndicators[i]);
                    hr = E_FAIL;
                }
                else
                {
                    hr = MakeAccountFromString(szUserInfo, &rgUsers[i]);

                    if (hr != S_OK)
                    {
                        TestMsg(XKTVerbose_All, "Failed to interpret user account '%s' from config file.\n", rgszUserAccountIndicators[i]);
                        break;
                    }
                }
            }
        }

        // Load machine account from file and put it into rgUsers

        if (hr == S_OK)
        {
            char *szMachineAccountIndicator = cfg.getStr(szSectionName, "machine");
            if (!szMachineAccountIndicator)
            {
                TestErrMsg(XKTVerbose_Process, "Failed to find valid machine account indicator.\n");
                hr = E_INVALIDARG;
            }
            else
            {
                char *szUserInfo = cfg.getStr(szUserSection, szMachineAccountIndicator);
                if (!szUserInfo)
                {
                    TestErrMsg(XKTVerbose_Process, "Failed to find machine info for %s.\n", szMachineAccountIndicator);
                    hr = E_INVALIDARG;
                }
                else
                {
                    if (strstr(szUserInfo, "newmachine") == szUserInfo)
                    {
                        hr = TicketCache::FindAccount(szMachineAccountIndicator, &rgUsers[cUsers]);
                        if (hr == S_FALSE)
                        {
                            char *szSerial = cfg.getStrListItem(szUserSection, szMachineAccountIndicator, 1);
                            char *szPassword = cfg.getStrListItem(szUserSection, szMachineAccountIndicator, 2);
                            hr = NewMachine(pXOn, cfg, szSerial, szPassword, &rgUsers[cUsers]);

                            if (hr != S_OK || TicketCache::AddAccount(szMachineAccountIndicator, &rgUsers[cUsers]) != S_OK)
                            {
                                TestMsg(XKTVerbose_All, "Failed to create new machine account '%s'.\n", szMachineAccountIndicator);
                                break;
                            }
                        }
                    }
                    else
                    {
                        Assert(cUsers >= 0 && cUsers <= XONLINE_MAX_LOGON_USERS);

                        hr = MakeAccountFromString(szUserInfo, &rgUsers[cUsers]);

                        if (hr != S_OK)
                        {
                            TestMsg(XKTVerbose_All, "Failed to interpret user account '%s' from config file.\n", szMachineAccountIndicator);
                            break;
                        }
                    }
                }
            }
        }

        if (SUCCEEDED(hr))
        {
            XKERB_TGT_CONTEXT tkt;

            hr = TicketCache::FindTicket(rgUsers, &tkt);
            if (FAILED(hr))
            {
                TestErrMsg(XKTVerbose_Process, "Failed during ticket cache lookup.\n");
            }
            else
            {
                if (hr == S_FALSE || !fUseCache)
                {
                    // Get ticket from XKDC
                    memset(&tkt, 0, sizeof(XKERB_TGT_CONTEXT));

                    hr = S_OK;
                    for (INT i = 0; i < cUsers + 1; i++)
                    {
                        XKERB_TGT_CONTEXT tktPrev;
                        TestMsg(XKTVerbose_Process, "Requesting ticket for '%s.%s'.\n", rgUsers[i].name, rgUsers[i].kingdom);

                        NTSTATUS Status = 0;
                        // If i == cUsers then this account is the last account in the list and thus the machine account
                        if ((hr = pXOn->SignInAccount_AS(dwXkdcIp, usXkdcPort, &rgUsers[i], i == cUsers, &tkt, Status)) != S_OK)
                        {
                            TestMsg(XKTVerbose_Process, "Failed to sign in with account '%s.%s'.\n", rgUsers[i].name, rgUsers[i].kingdom);
                            hr = E_FAIL;
                            break;
                        }

                        if (i < cUsers)
                        {
                            memcpy(&tktPrev, &tkt, sizeof(XKERB_TGT_CONTEXT));
                            tktPrev.pInputTGTContext = NULL;
                            tkt.pInputTGTContext = &tktPrev;
                        }
                    }

                    if (hr == S_OK)
                    {
                        hr = TicketCache::AddTicket(rgUsers, &tkt);
                        if (FAILED(hr))
                        {
                            TestErrMsg(XKTVerbose_Process, "Failed to add ticket to cache.\n");
                        }
                    }
                }
            }

            if (SUCCEEDED(hr))
            {
                hr = SignInServices(pXOn, cfg, szSectionName, rgUsers, cUsers, &tkt);
            }
        }
    }

    return hr;
}
