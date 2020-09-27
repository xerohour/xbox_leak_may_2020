/*
**
** xbcp
**
** Copy files to and from the Xbox
**
*/

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <xboxdbg.h>
#include "xbfile.hxx"
#include "optn.hxx"
#include "xbfileres.h"
#include "resource.h"
#include <xboxverp.h>

enum { mdLockNone, mdLockUnlock, mdLockLock };
enum { mdPasswdNone, mdPasswdRemove, mdPasswdSet };
enum { mdPrivNone, mdPrivUnset, mdPrivSet };
int mdLock;
int mdPasswd;
BOOL fPrompt;
BOOL fList;
LPCSTR szHostName;
ULONG grbitPriv;
ULONG grbitPrivMask;

BOOL FSetPrivs(int mdPriv, DWORD bitPriv)
{
    switch(mdPriv) {
    case mdPrivNone:
        return FALSE;
    case mdPrivUnset:
        grbitPrivMask |= bitPriv;
        grbitPriv &= ~bitPriv;
        break;
    case mdPrivSet:
        grbitPrivMask |= bitPriv;
        grbitPriv |= bitPriv;
        break;
    }
    return TRUE;
}

void GetPasswd(LPCSTR szPrompt, LPSTR szPasswd, int cchMax)
{
    int ich;
    char ch;

    _cputs(szPrompt);
    for(ich = 0;;) {
        ch = (char)_getch();
        switch(ch) {
        case 10:
        case 13:
            _putch('\r');
            _putch('\n');
            szPasswd[ich >= cchMax - 1 ? cchMax : ich] = 0;
            return;
        case 8:
        case 127:
            if(ich > 0)
                --ich;
            break;
        default:
            if(ch >= 32 && ++ich < cchMax)
                szPasswd[ich - 1] = ch;
            break;
        }
    }
}

void NotLocked(void)
{
    Warning(0, IDS_NOTLOCKED);
    DmUseSharedConnection(FALSE);
    exit(-1);
}

void UserFatal(HRESULT hr, int ids, LPCSTR szUserName)
{
    switch(hr) {
    case XBDM_NOSUCHFILE:
        ids = IDS_NOUSER;
        break;
    case XBDM_CANNOTACCESS:
        ids = IDS_ACCESSDENIED;
        break;
    case XBDM_ALREADYEXISTS:
        ids = IDS_USEREXISTS;
        break;
    }
    Fatal(hr, ids, szUserName);
}

void FillAccessSz(LPSTR sz, DWORD dmpl)
{
    LPSTR szOrig = sz;

    if(dmpl & DMPL_PRIV_READ) {
        strcpy(sz, " read");
        sz += 5;
    }
    if(dmpl & DMPL_PRIV_WRITE) {
        strcpy(sz, " write");
        sz += 6;
    }
    if(dmpl & DMPL_PRIV_CONFIGURE) {
        strcpy(sz, " configure");
        sz += 10;
    }
    if(dmpl & DMPL_PRIV_CONTROL) {
        strcpy(sz, " control");
        sz += 8;
    }
    if(dmpl & DMPL_PRIV_MANAGE) {
        strcpy(sz, " manage");
        sz += 6;
    }
    if(sz == szOrig)
        strcpy(sz, " <none>");
}

void PrintBoxInfo(BOOL fSecure)
{
    char szName[1024];
    DWORD dw;
    HRESULT hr;

    dw = sizeof szName;
    hr = DmGetXboxName(szName, &dw);
    if(FAILED(hr))
        Fatal(hr, 0);

    printf(" Xbox target system %s\n Security is %s\n\n", szName,
        fSecure ? "enabled" : "disabled");
}

BOOL OPTH::FParseOption(char *&sz, char ch)
{
    int mdPriv;
    LPCSTR szOption = sz;

    /* We always parse options as complete words */
    sz += strlen(sz) - 1;

    switch(ch) {
    case '-':
        mdPriv = mdPrivUnset;
        break;
    case '+':
        mdPriv = mdPrivSet;
        break;
    default:
        mdPriv = mdPrivNone;
        break;
    }

    if(0 == _stricmp(szOption, "read"))
        return FSetPrivs(mdPriv, DMPL_PRIV_READ);
    else if(0 == _stricmp(szOption, "write"))
        return FSetPrivs(mdPriv, DMPL_PRIV_WRITE);
    else if(0 == _stricmp(szOption, "control"))
        return FSetPrivs(mdPriv, DMPL_PRIV_CONTROL);
    else if(0 == _stricmp(szOption, "configure"))
        return FSetPrivs(mdPriv, DMPL_PRIV_CONFIGURE);
    else if(0 == _stricmp(szOption, "manage"))
        return FSetPrivs(mdPriv, DMPL_PRIV_MANAGE);
    else if(0 == _stricmp(szOption, "all"))
        return FSetPrivs(mdPriv, -1);
    else if(0 == _stricmp(szOption, "lock")) {
        if(mdLock == mdLockUnlock)
            return FALSE;
        mdLock = mdLockLock;
    } else if(0 == _stricmp(szOption, "unlock")) {
        if(mdLock == mdLockLock)
            return FALSE;
        mdLock = mdLockUnlock;
    } else if(0 == _stricmp(szOption, "setpassword")) {
        if(mdPasswd == mdPasswdRemove)
            return FALSE;
        mdPasswd = mdPasswdSet;
    } else if(0 == _stricmp(szOption, "nopassword")) {
        if(mdPasswd == mdPasswdSet)
            return FALSE;
        mdPasswd = mdPasswdRemove;
    } else if(0 == _stricmp(szOption, "prompt"))
        ++fPrompt;
    else if(0 == _stricmp(szOption, "list")) {
        if(szHostName)
            return FALSE;
        ++fList;
    } else if(0 == _strnicmp(szOption, "host:", 5)) {
        if(szHostName || fList || !szOption[5])
            return FALSE;
        szHostName = szOption + 5;
    } else
        return FALSE;

    /* We won't take non-privilege options starting with '+', so even though
     * we've already processed them, we're still going to raise an error */
    return mdPriv != mdPrivSet;
}

void OPTH::UsageFailure(void)
{
    fprintf(stderr, "xbmanage version " VER_PRODUCTVERSION_STR "\n\n"
        "Manage access permissions of Xbox target system\n");
	fprintf(stderr, "\nusage: %s [/x xboxname] [options]\n",
		m_szExe);
	fprintf(stderr,
        "\t/lock        enable remote security\n"
		"\t/unlock      disable remote security\n"
        "\t/setpassword set remote administration password\n"
		"\t/nopassword  disable remote administration password\n"
		"\t/prompt      prompt for administration password\n"
		"\t/host:name   change permissions for specified host\n"
        "\t/list        list all permissions\n"
        "\n\t\tPermissions (precede with - or + to disable or enable):\n"
		"\t read        read files and directories\n"
        "\t write       write files\n"
        "\t control     debug or remotely control\n"
        "\t configure   change system configuration or options\n"
        "\t manage      remote access management\n"
        "\t all         all access permissions\n");
	exit(1);
}

int __cdecl main(int cArg, char **rgszArg)
{
	HRESULT hr;
    char szPasswd[256];
    char szPasswd2[256];
    char szLocalhost[MAX_COMPUTERNAME_LENGTH + 1];
    char szTemp[256];
    DWORD dw;
    BOOL fSecure;
    PDM_WALK_USERS pwu;
    DM_USER dmu;
    BOOL fDidWork = FALSE;
    int c;

	/* Process arguments */
	OPTH opth(cArg, rgszArg, "/-+");

    /* Get the current computer name; we may need it */
    dw = sizeof szLocalhost;
    if(!GetComputerName(szLocalhost, &dw))
        Fatal(E_UNEXPECTED, 0);

    /* We need to use a single connection, so set it up */
    DmUseSharedConnection(TRUE);

    /* If we need to enter the admin password, then fetch it and attempt to
     * connect with it */
    if(fPrompt) {
        GetPasswd("Enter admin password:", szPasswd, sizeof szPasswd);
        hr = DmOpenSecureConnection(NULL, szPasswd);
        if(FAILED(hr))
            UserFatal(hr, 0, NULL);
    }

    /* Get the current lock state */
    hr = DmIsSecurityEnabled(&fSecure);
    if(FAILED(hr))
        UserFatal(hr, 0, NULL);

    /* Lock or unlock as desired */
    switch(mdLock) {
    case mdLockUnlock:
        hr = DmEnableSecurity(FALSE);
        fDidWork = TRUE;
        fSecure = FALSE;
        break;
    case mdLockLock:
        /* If we're locking a previously unlocked box, we'll give ourselves
         * all permissions */
        hr = DmEnableSecurity(TRUE);
        if(FAILED(hr))
            UserFatal(hr, 0, NULL);
        if(!fSecure) {
            hr = DmAddUser(szLocalhost, -1);
            if(FAILED(hr))
                UserFatal(hr, 0, szTemp);
        }
        fDidWork = TRUE;
        fSecure = TRUE;
        break;
    default:
        hr = XBDM_NOERR;
        break;
    }

    /* Deal with the admin passwd as requested */
    if(mdPasswd != mdPasswdNone) {
        if(!fSecure)
            NotLocked();
        switch(mdPasswd) {
        default:
            hr = DmSetAdminPassword(NULL);
            fDidWork = TRUE;
            break;
        case mdPasswdSet:
            GetPasswd("Enter new admin password:", szPasswd, sizeof szPasswd);
            GetPasswd("Enter it again:", szPasswd2, sizeof szPasswd2);
            if(strcmp(szPasswd, szPasswd2))
                Fatal(0, IDS_PASSWD_MISMATCH);
            hr = DmSetAdminPassword(szPasswd);
            fDidWork = TRUE;
            break;
        }
        if(FAILED(hr))
            UserFatal(hr, 0, NULL);
    }

    /* List permissions if requested */
    if(fList) {
        if(!fSecure)
            NotLocked();
        PrintBoxInfo(fSecure);
        pwu = NULL;
        while(SUCCEEDED(hr = DmWalkUserList(&pwu, &dmu))) {
            FillAccessSz(szTemp, dmu.AccessPrivileges);
            printf("%s:%s\n", dmu.UserName, szTemp);
        }
        DmCloseUserList(pwu);
        if(hr != XBDM_ENDOFLIST)
            UserFatal(hr, 0, NULL);
        fDidWork = TRUE;
    }

    /* If we haven't done any work yet, we're going to be getting or setting
     * privileges.  If we don't have a hostname, we'll assume it's our
     * host */
    if(!fDidWork && !szHostName && fSecure)
        szHostName = szLocalhost;

    /* Set permissions if requested */
    if(szHostName) {
        if(!fSecure)
            NotLocked();

        /* Start by getting the specified user's privilege level.  If it's
         * query-only and no hostname was specified, we'll try a
         * self-access query */
        if(szHostName != szLocalhost || grbitPrivMask ||
                FAILED(hr = DmGetUserAccess(NULL, &dw)))
            hr = DmGetUserAccess(szHostName, &dw);

        /* If this is a query-only operation and we failed to get the
         * privileges, then report an error.  Otherwise, we'll assume zero
         * privilege */
        if(FAILED(hr)) {
            if(!grbitPrivMask)
                UserFatal(hr, 0, szHostName);
            dw = 0;
        }

        /* If this is query-only, then just display the user info */
        if(!grbitPrivMask) {
            PrintBoxInfo(fSecure);
            FillAccessSz(szTemp, dw);
            printf("%s:%s\n", szHostName, szTemp);
        } else {
            /* If we're removing manage permission, we'd better make sure
             * there's another user on the box with manage permission, or
             * we'll run into big trouble */
            if((dw & DMPL_PRIV_MANAGE) && (grbitPrivMask & DMPL_PRIV_MANAGE) &&
                ((grbitPriv & DMPL_PRIV_MANAGE) == 0))
            {
                pwu = NULL;
                c = 0;
                while(SUCCEEDED(hr = DmWalkUserList(&pwu, &dmu))) {
                    if(dmu.AccessPrivileges & DMPL_PRIV_MANAGE)
                        ++c;
                }
                DmCloseUserList(pwu);
                if(hr != XBDM_ENDOFLIST)
                    UserFatal(hr, 0, NULL);
                /* We need two users with manage permission, since we've
                 * counted this user as one */
                if(c < 2)
                    Fatal(0, IDS_NOMANAGE);
            }
            /* Change the appropriate bits */
            dw ^= (dw ^ grbitPriv) & grbitPrivMask;

            /* If the resultant permission is none, then simply remove the
             * user from the list.  Otherwise, we'll set in the new permission
             * bits.  If the previous attempt to get user privileges failed,
             * we'll try to add a new user with the requested bits */
            if(!dw)
                hr = DmRemoveUser(szHostName);
            else if(FAILED(hr))
                hr = DmAddUser(szHostName, dw);
            else
                hr = DmSetUserAccess(szHostName, dw);
            if(FAILED(hr))
                UserFatal(hr, 0, szHostName);
        }
        fDidWork = TRUE;
    }

    if(!fDidWork)
        /* We haven't done anything yet, so we'll print details on this box */
        PrintBoxInfo(fSecure);

    /* And that's it */
	DmUseSharedConnection(FALSE);

    return 0;
}
