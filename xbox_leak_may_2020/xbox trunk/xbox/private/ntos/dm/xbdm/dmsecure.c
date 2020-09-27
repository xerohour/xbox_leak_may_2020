/*
 *
 * dmsecure.c
 *
 * Connection security functions
 *
 */

#include "dmp.h"

static ULONG rgN[8] = {
    0x283C481D,
    0x9AD82AA1,
    0x85A5E1F9,
    0x1B23963C,
    0xF70B4975,
    0xDFDC02C7,
    0xF29176FC,
    0x6B04BD38
};

static HANDLE hUserList;
RTL_CRITICAL_SECTION csUserList;
ULARGE_INTEGER g_luAdminPasswd;
BOOL g_fAdminPasswd;

typedef struct _DMUSER {
    ULARGE_INTEGER luPasswd;
    DWORD dwPrivileges;
    BOOL fHasPasswd;
} DMUSER, *PDMUSER;

static OBJECT_TYPE obtUser = {
    DmAllocatePoolWithTag,
    DmFreePool,
    NULL,
    NULL,
    NULL,
    &kevtNull,
    'SUmd'
};

void XBCEncryptCore(PULARGE_INTEGER pluKey, PULARGE_INTEGER pluBlock)
{
    int i;

    for(i = 0; i < 8; ++i) { 
        pluBlock->LowPart ^= (pluBlock->HighPart >> 5) + rgN[i] +
            (~pluBlock->HighPart << 6) + (pluBlock->HighPart ^ pluKey->LowPart);
        pluBlock->HighPart ^= (~pluBlock->LowPart >> 5) + rgN[7-i] +
            (pluBlock->LowPart << 6) + (pluBlock->LowPart ^ pluKey->HighPart);
    }
}

void XBCDecryptCore(PULARGE_INTEGER pluKey, PULARGE_INTEGER pluBlock)
{
    int i;

    for(i = 0; i < 8; ++i) { 
        pluBlock->HighPart ^= (~pluBlock->LowPart >> 5) + rgN[i] +
            (pluBlock->LowPart << 6) + (pluBlock->LowPart ^ pluKey->HighPart);
        pluBlock->LowPart ^= (pluBlock->HighPart >> 5) + rgN[7-i] +
            (~pluBlock->HighPart << 6) + (pluBlock->HighPart ^ pluKey->LowPart);
    }
}

void XBCHashBlock(PULARGE_INTEGER pluHash, PULARGE_INTEGER pluData)
{
    /* We don't use Davies-Meyer directly, we use an analogous formula:
     *   H(i) = E(H(i-1),M(i)) X M(i)
     */
    ULARGE_INTEGER luTemp;

    luTemp = *pluData;
    XBCEncryptCore(pluHash, &luTemp);
    pluHash->LowPart = pluData->LowPart ^ luTemp.LowPart;
    pluHash->HighPart = pluData->HighPart ^ luTemp.HighPart;
}

void XBCHashData(PULARGE_INTEGER pluHash, const BYTE *pb, ULONG cb)
{
    ULARGE_INTEGER lu;

    /* Process the 8-byte chunks first */
    while(cb >= 8) {
        memcpy(&lu, pb, 8);
        pb += 8;
        cb -= 8;
        XBCHashBlock(pluHash, &lu);
    }
    if(cb) {
        memset(&lu, 0, sizeof lu);
        memcpy(&lu, pb, cb);
        XBCHashBlock(pluHash, &lu);
    }
}

void XBCCross(PULARGE_INTEGER pluKey, PULARGE_INTEGER pluData,
    PULARGE_INTEGER pluResult)
{
    ULARGE_INTEGER luTemp;

    /* Compute H(K, H(K, M)) where K is the key and M is the data */
    pluResult->QuadPart = 0x2718281831415926;
    XBCHashBlock(pluResult, pluKey);
    luTemp = *pluResult;
    XBCHashBlock(&luTemp, pluData);
    XBCHashBlock(pluResult, &luTemp);
}

void InitSecurity(void)
{
    NTSTATUS st;

    /* We use the object manager to store the user list, so we need to create
     * a directory object to contain the list */
    st = NtCreateDirectoryObject(&hUserList, NULL);
    if(!NT_SUCCESS(st))
        hUserList = NULL;
    InitializeCriticalSection(&csUserList);
}

PDMUSER PusrLookupUser(LPCSTR szUserName)
{
    PDMUSER pusr;
    OBJECT_ATTRIBUTES oa;
    NTSTATUS st;
    OBJECT_STRING ost;
    HANDLE h;

    /* First look up the handle -- we need to do this because this object
     * isn't rooted in the object namespace.  We don't synchronize access
     * to the list since the object manager does that for us */
    RtlInitObjectString(&ost, szUserName);
    InitializeObjectAttributes(&oa, &ost, 0, hUserList, NULL);
    st = ObOpenObjectByName(&oa, &obtUser, NULL, &h);
    if(!NT_SUCCESS(st))
        return NULL;
    /* Now get the underlying object */
    st = ObReferenceObjectByHandle(h, NULL, &pusr);
    NtClose(h);
    return NT_SUCCESS(st) ? pusr : NULL;
}

DWORD DmplAuthenticateUser(LPCSTR szUserName, PULARGE_INTEGER pluNonce,
    PULARGE_INTEGER pluResponse, BOOL *pfKeyXchg)
{
    PDMUSER pusr;
    ULARGE_INTEGER luHandshake;
    ULONG dmpl;

    if(!szUserName) {
        /* We're doing an admin password check */
        if(*pfKeyXchg || !g_fAdminPasswd)
            return 0;
        XBCCross(&g_luAdminPasswd, pluNonce, &luHandshake);
        return luHandshake.QuadPart == pluResponse->QuadPart ? -1 : 0;
    }

    /* Look up this user */
    pusr = PusrLookupUser(szUserName);
    if(!pusr)
        return 0;
    
    /* Verify the password */
    if(pusr->fHasPasswd) {
        if(!*pfKeyXchg) {
            XBCCross(&pusr->luPasswd, pluNonce, &luHandshake);
            dmpl = luHandshake.QuadPart == pluResponse->QuadPart ?
                pusr->dwPrivileges : 0;
        } else
            dmpl = 0;
        *pfKeyXchg = FALSE;
    } else {
        /* The user hasn't set a password yet.  If we're getting the password
         * now, then all is well.  If not, we'll deny access and demand a
         * password */
        if(*pfKeyXchg) {
            pusr->luPasswd.QuadPart = pluResponse->QuadPart;
            pusr->fHasPasswd = TRUE;
            dmpl = pusr->dwPrivileges;
            *pfKeyXchg = FALSE;
            /* We've changed the userlist, so we need to rewrite it */
            WriteIniFile();
        } else {
            dmpl = 0;
            *pfKeyXchg = TRUE;
        }
    }

    /* Clean up */
    ObDereferenceObject(pusr);
    return dmpl;
}

HRESULT DmEnableSecurity(BOOL fEnable)
{
    g_fLockLevel = !!fEnable;
    RemoveAllUsers();
    return XBDM_NOERR;
}

HRESULT DmIsSecurityEnabled(LPBOOL pfEnabled)
{
    if(!pfEnabled)
        return E_INVALIDARG;

    *pfEnabled = g_fLockLevel != 0;
    return XBDM_NOERR;
}

HRESULT HrDoAddUser(LPCSTR szUserName, PDMUSER pusrTemplate)
{
    PDMUSER pusr;
    HRESULT hr;
    OBJECT_STRING ost;
    OBJECT_ATTRIBUTES oa;
    NTSTATUS st;
    HANDLE h;

    /* No users allowed if the box isn't locked */
    if(!g_fLockLevel)
        return XBDM_NOTLOCKED;

    /* Synchronize access */
    EnterCriticalSection(&csUserList);

    /* See if this user already exists */
    pusr = PusrLookupUser(szUserName);
    if(pusr) {
        LeaveCriticalSection(&csUserList);
        return XBDM_ALREADYEXISTS;
    }

    /* Make a new user */
    RtlInitObjectString(&ost, szUserName);
    InitializeObjectAttributes(&oa, &ost, OBJ_PERMANENT, hUserList, NULL);
    st = ObCreateObject(&obtUser, &oa, sizeof *pusr, &pusr);
    if(!NT_SUCCESS(st)) {
        LeaveCriticalSection(&csUserList);
        return E_OUTOFMEMORY;
    }

    /* Now add this user to the handle table so it can be found later.  Mark a
     * pointer bias of 1 to keep the object persistent */
    st = ObInsertObject(pusr, &oa, 1, &h);

    /* We no longer need exclusive access to the user list */
    LeaveCriticalSection(&csUserList);

    /* We don't need our base reference to this object anymore.  If we added it
     * to the handle table, the pointer will still be valid and if not, we want
     * to kill off the reference anyway */
    ObDereferenceObject(pusr);
    
    if(NT_SUCCESS(st)) {
        memcpy(pusr, pusrTemplate, sizeof *pusr);
        NtClose(h);
        hr = XBDM_NOERR;
    } else
        hr = E_OUTOFMEMORY;

    return hr;
}

HRESULT DmAddUser(LPCSTR szUserName, DWORD dmpl)
{
    DMUSER usr;

    memset(&usr, 0, sizeof usr);
    usr.dwPrivileges = dmpl;
    return HrDoAddUser(szUserName, &usr);
}

HRESULT DmRemoveUser(LPCSTR szUserName)
{
    PDMUSER pusr;
    HRESULT hr;

    /* Synchronize access */
    EnterCriticalSection(&csUserList);

    /* Find the user we want to delete */
    pusr = PusrLookupUser(szUserName);

    /* Delete it if we found it */
    if(pusr) {
        /* Mark the object as temporary so it will be removed from the
         * directory */
        ObMakeTemporaryObject(pusr);
        /* Deref the object twice -- once for our reference and once for the
         * base reference -- to actually delete the object */
        ObDereferenceObject(pusr);
        ObDereferenceObject(pusr);
        hr = XBDM_NOERR;
    } else
        hr = XBDM_NOSUCHFILE;

    LeaveCriticalSection(&csUserList);

    return hr;
}

HRESULT HrAddUserCommand(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    DMUSER usr;
    char szUserName[64];
    HRESULT hr;
    BOOL fRemove;

    /* Get the user name */
    if(!FGetSzParam(sz, "name", szUserName, sizeof szUserName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }

    /* If we're removing, then remove */
    if(PchGetParam(sz, "remove", FALSE))
        return DmRemoveUser(szUserName);

    /* Set the password, if available.  We only accept the password if this
     * command is coming from the ini file */
    if(!pdmcc && FGetQwordParam(sz, "passwd", &usr.luPasswd))
        usr.fHasPasswd = TRUE;
    else
        usr.fHasPasswd = FALSE;

    /* Set the privileges */
    usr.dwPrivileges = DmplFromSz(sz);

    /* Do the work */
    return HrDoAddUser(szUserName, &usr);
}

void FillAccessPrivSz(LPSTR sz, DWORD dwPriv)
{
    LPSTR szOrig = sz;

    if(dwPriv & DMPL_PRIV_READ) {
        strcpy(sz, " read");
        sz += 5;
    }
    if(dwPriv & DMPL_PRIV_WRITE) {
        strcpy(sz, " write");
        sz += 6;
    }
    if(dwPriv & DMPL_PRIV_CONTROL) {
        strcpy(sz, " control");
        sz += 8;
    }
    if(dwPriv & DMPL_PRIV_CONFIGURE) {
        strcpy(sz, " config");
        sz += 7;
    }
    if(dwPriv & DMPL_PRIV_MANAGE) {
        strcpy(sz, " manage");
        sz += 7;
    }
    *sz = 0;
}

DWORD DmplFromSz(LPCSTR sz)
{
    ULONG dmpl = 0;

    if(PchGetParam(sz, "read", FALSE))
        dmpl |= DMPL_PRIV_READ;
    if(PchGetParam(sz, "write", FALSE))
        dmpl |= DMPL_PRIV_WRITE;
    if(PchGetParam(sz, "control", FALSE))
        dmpl |= DMPL_PRIV_CONTROL;
    if(PchGetParam(sz, "config", FALSE))
        dmpl |= DMPL_PRIV_CONFIGURE;
    if(PchGetParam(sz, "manage", FALSE))
        dmpl |= DMPL_PRIV_MANAGE;

    return dmpl;
}

void FillUserInfoSz(LPSTR sz, LPCSTR szName, DWORD dwPrivileges,
    PULARGE_INTEGER pluPasswd)
{
    char szPasswd[32];

    if(pluPasswd)
        sprintf(szPasswd, " passwd=0q%08x%08x", pluPasswd->HighPart,
            pluPasswd->LowPart);
    else
        szPasswd[0] = 0;

    sprintf(sz, "name=\"%s\"%s", szName, szPasswd);
    FillAccessPrivSz(sz + strlen(sz), dwPrivileges);
}

void WriteIniSecurity(INF *pinf)
{
    struct {
        OBJECT_DIRECTORY_INFORMATION odi;
        OCHAR sz[64];
    } odi;
    HRESULT hr;
    HANDLE h;
    int i;
    PDMUSER pusr;
    char sz[256];

    EnterCriticalSection(&csUserList);

    if(g_fLockLevel) {
        sprintf(sz, "lockmode boxid=0q%08x%08x%s", g_luBoxId.HighPart,
            g_luBoxId.LowPart, g_fLockLevel==2 ? " encrypt" : "");
        WriteIniSz(pinf, sz);
    }

    if(g_fAdminPasswd) {
        sprintf(sz, "adminpw passwd=0q%08x%08x", g_luAdminPasswd.HighPart,
            g_luAdminPasswd.LowPart);
        WriteIniSz(pinf, sz);
    }

    if(g_fLockLevel && hUserList) {
        strcpy(sz, "user ");
        i = 0;
        while(NT_SUCCESS(NtQueryDirectoryObject(hUserList, &odi, sizeof odi,
            i == 0, &i, NULL)))
        {
            ASSERT(odi.odi.Name.Buffer == odi.sz);
            odi.sz[odi.odi.Name.Length] = 0;
            pusr = PusrLookupUser(odi.odi.Name.Buffer);
            if(pusr) {
                FillUserInfoSz(sz + 5, odi.sz, pusr->dwPrivileges,
                    pusr->fHasPasswd ? &pusr->luPasswd : NULL);
                WriteIniSz(pinf, sz);
                ObDereferenceObject(pusr);
            }
        }
    }

    LeaveCriticalSection(&csUserList);
}

void RemoveAllUsers(void)
{
    struct {
        OBJECT_DIRECTORY_INFORMATION odi;
        OCHAR sz[64];
    } odi;
    HRESULT hr;
    HANDLE h;
    int i;
    PDMUSER pusr;

    EnterCriticalSection(&csUserList);

    /* We actually only empty the user list if the lock level is zero */
    if(!g_fLockLevel && hUserList) {
        i = 0;
        while(NT_SUCCESS(NtQueryDirectoryObject(hUserList, &odi, sizeof odi,
            i == 0, &i, NULL)))
        {
            ASSERT(odi.odi.Name.Buffer == odi.sz);
            odi.sz[odi.odi.Name.Length] = 0;
            pusr = PusrLookupUser(odi.odi.Name.Buffer);
            if(pusr) {
                ObMakeTemporaryObject(pusr);
                ObDereferenceObject(pusr);
                ObDereferenceObject(pusr);
                i = 0;
            }
        }
    }

    LeaveCriticalSection(&csUserList);
}

DMHRAPI DmSetUserAccess(LPCSTR szUserName, DWORD dwAccess)
{
    PDMUSER pusr;

    pusr = PusrLookupUser(szUserName);
    if(!pusr)
        return XBDM_NOSUCHFILE;
    pusr->dwPrivileges = dwAccess;
    ObDereferenceObject(pusr);
    return XBDM_NOERR;
}

DMHRAPI DmGetUserAccess(LPCSTR szUserName, LPDWORD lpdwAccess)
{
    PDMUSER pusr;

    pusr = PusrLookupUser(szUserName);
    if(!pusr)
        return XBDM_NOSUCHFILE;
    *lpdwAccess = pusr->dwPrivileges;
    ObDereferenceObject(pusr);
    return XBDM_NOERR;
}

DMHRAPI DmWalkUserList(PDM_WALK_USERS *ppdmwu, PDM_USER pdusr)
{
    struct {
        OBJECT_DIRECTORY_INFORMATION odi;
        OCHAR sz[64];
    } odi;
    HRESULT hr;
    HANDLE h;
    int i;
    PDMUSER pusr;
    NTSTATUS st;

    if(!ppdmwu)
        return E_INVALIDARG;
    if(!g_fLockLevel || !hUserList)
        return XBDM_ENDOFLIST;

    st = NtQueryDirectoryObject(hUserList, &odi, sizeof odi,
        *(PULONG)ppdmwu == 0, (PULONG)ppdmwu, NULL);
    if(st == STATUS_NO_MORE_ENTRIES)
        return XBDM_ENDOFLIST;
    else if(!NT_SUCCESS(st))
        return HrFromStatus(st, E_FAIL);
    
    memset(pdusr, 0, sizeof *pdusr);
    memcpy(pdusr->UserName, odi.odi.Name.Buffer, odi.odi.Name.Length);
    pusr = PusrLookupUser(pdusr->UserName);
    if(!pusr)
        return E_UNEXPECTED;
    pdusr->AccessPrivileges = pusr->dwPrivileges;
    return XBDM_NOERR;
}

DMHRAPI DmCloseUserList(PDM_WALK_USERS pdmwu)
{
    return XBDM_NOERR;
}
