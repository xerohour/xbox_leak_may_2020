#include "xkdctest.h"
#include "xktxmacscreate.h"
#include "accounts.h"


HRESULT XKTXmacsCreate::VerifyCreate(IN CXoTest *xo, Config &cfg, XONLINEP_USER &MachineAccount, XKERB_TGT_CONTEXT &tgt)
{
    HRESULT hr = S_OK;

    // Verify that the account was created correctly
    if (tgt.pAccountCreationPreAuth)
    {
        TA(tgt.pAccountCreationPreAuth->qwUserID > 0);
        //!!Verify tgt.pAccountCreationPreAuth->name (base 64)
        if (!strcmp(MachineAccount.name, "999999999999"))
        {
            TA(!strcmp(tgt.pAccountCreationPreAuth->kingdom, "_devkit_"));
        }
        else
        {
            TA(strcmp(tgt.pAccountCreationPreAuth->kingdom, "machines"));
        }
        TA(!strcmp(tgt.pAccountCreationPreAuth->domain, cfg.getStr("general", "XmacsDomain")));
        TA(!strcmp(tgt.pAccountCreationPreAuth->realm, "passport.net"));
        char szClientName[] = "2\0" "_|_|_|_|_|_|\0" "macs.xbox.com";
        memcpy(szClientName + 2, &MachineAccount.name, 12);
        TA(!memcmp(tgt.ClientName, szClientName, 29));
        TA(!strcmp(tgt.ClientRealm, "macs.xbox.com"));
        TA(tgt.ExpirationTime.QuadPart > tgt.StartTime.QuadPart);

        TAHR(MakeAccountWithKey(
            &MachineAccount, 
            tgt.pAccountCreationPreAuth->qwUserID, 
            tgt.pAccountCreationPreAuth->name, 
            tgt.pAccountCreationPreAuth->kingdom, 
            tgt.pAccountCreationPreAuth->domain, 
            tgt.pAccountCreationPreAuth->realm, 
            tgt.pAccountCreationPreAuth->key));

        TestMsg(XKTVerbose_Process, "Verifying created account by signing into XKDC...\n");

        DWORD dwXkdcIp = cfg.getIP("servers", "xkdc");
        USHORT usXkdcPort = cfg.getPort("servers", "xkdc");

        XKERB_TGT_CONTEXT tgt2;
        memset(&tgt2, 0, sizeof(XKERB_TGT_CONTEXT));
        NTSTATUS Status = 0;
        TAHR(xo->SignInAccount_AS(dwXkdcIp, usXkdcPort, &MachineAccount, TRUE, &tgt2, Status));

        char *szT = tgt2.ClientName;
        TA(!strcmp(szT, "2"));
        szT += strlen(szT) + 1;
        TA(strstr(szT, tgt.pAccountCreationPreAuth->name) == szT);
        TA((INT)strlen(tgt.pAccountCreationPreAuth->name) == (strstr(szT, tgt.pAccountCreationPreAuth->kingdom) - szT - 1));
        szT += strlen(szT) + 1;
        TA(!strcmp(szT, "passport.net"));

        TA(!strcmp(tgt2.ClientRealm, "passport.net"));
        TA(tgt2.ctNumNonces == 1);

        xo->SysFree(tgt.pAccountCreationPreAuth);
    }
    else
    {
        hr = E_FAIL;
    }

    return hr;
}


char *XKTXmacsCreate::getStrX(Config &cfg, char *szSectionName, char *szItem, INT nIteration, INT nCreations)
{
    char *sz;
    if (nCreations == 0)
    {
        sz = cfg.getStr(szSectionName, szItem);
    }
    else
    {
        char szT[20];
        strcpy(szT, szItem);
        _itoa(nIteration, szT + strlen(szT), 10);
        sz = cfg.getStr(szSectionName, szT);
    }

    return sz;
}


HRESULT XKTXmacsCreate::MakeCreationAccount(IN char *szSectionName, IN Config &cfg, IN INT nIteration, IN INT nCreations, OUT XONLINEP_USER *pAccount)
{
    HRESULT hr = E_FAIL;

    char *szMachineAccountIndicator = getStrX(cfg, szSectionName, "machine", nIteration, nCreations);

    char *szSerial;
    char *szPassword;
    char *szRealm;

    if (szMachineAccountIndicator)
    {
        szSerial = cfg.getStrListItem("machines", szMachineAccountIndicator, 1);
        szPassword = cfg.getStrListItem("machines", szMachineAccountIndicator, 2);
        szRealm = cfg.getStrListItem("machines", szMachineAccountIndicator, 2);
    }
    else
    {
        szSerial = cfg.getStr(szSectionName, "serial");
        szPassword = cfg.getStr(szSectionName, "password");
        szRealm = cfg.getStr(szSectionName, "realm");
    }

    char *szSerialT = "999999999999";
    char *szPasswordT = "PASSWORD";
    char *szRealmT = "macs.xbox.com";

    if(!szSerial)
    {
        szSerial = szSerialT;
    }    
    if(!szPassword)
    {
        szPassword = szPasswordT;
    }
    if(!szRealm)
    {
        szRealm = szRealmT;
    }

    if (szPassword[0] == '0' && (szPassword[1] == 'x' || szPassword[1] == 'X'))
    {
        hr = MakeAccountWithKey(pAccount, 1, szSerial, "", "", szRealm, (BYTE*)(szPassword + 2));
    }
    else
    {
        hr = MakeAccountWithPassword(pAccount, 1, szSerial, "", "", szRealm, szPassword);
    }

    return hr;
}


HRESULT XKTXmacsCreate::runTest(IN CXoTest *xo, IN char *szSectionName, IN Config &cfg)
{
    HRESULT hr = E_FAIL;

    DWORD dwXmacsIp = cfg.getIP("servers", "xmacs");
    USHORT usXmacsPort = cfg.getPort("servers", "xmacs");

    int nCreations = cfg.getInt(szSectionName, "numcreations");

    INT i = 0;
    while (i < nCreations || (nCreations == 0 && i == 0))
    {
        HRESULT hrT = E_FAIL;
        
        char *szExpected = getStrX(cfg, szSectionName, "expected", i, nCreations);

        NTSTATUS nErrorValue = STATUS_SUCCESS;
        if (szExpected)
        {
            nErrorValue = KerbErrorCompare::GetErrorValue(szExpected);
        }

        char *szTimeSkew = getStrX(cfg, szSectionName, "timeskew", i, nCreations);
        INT nTimeSkew = 0;
        if (szTimeSkew)
        {
            nTimeSkew = atoi(szTimeSkew);
        }
        // The maximum allowed TimeSkew is 5 minutes to either side of correct
        BOOL fTimeSkewRepeat = nTimeSkew > 300;

        XONLINEP_USER MachineAccount;
        if ((hr = MakeCreationAccount(szSectionName, cfg, i, nCreations, &MachineAccount)) == S_OK)
        {
            TestMsg(XKTVerbose_Process, "Sending machine account creation request with principal \"%s@%s\"...\n", MachineAccount.name, MachineAccount.realm);

            XKERB_TGT_CONTEXT tgt;
            RtlZeroMemory(&tgt, sizeof(tgt));

            NTSTATUS Status = 0;
            for (INT j = 0; j < 2; j++)
            {
                // Send an AS to the XMACS to create the account
                hr = xo->SignInAccount_AS(dwXmacsIp, usXmacsPort, &MachineAccount, FALSE, &tgt, Status);
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
                        TestMsg(XKTVerbose_All, "Incorrect return value from XMACS.\n");
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
                    hr = VerifyCreate(xo, cfg, MachineAccount, tgt);
                }
            }
        }

        i++;
    }
    
    return hr;
}

