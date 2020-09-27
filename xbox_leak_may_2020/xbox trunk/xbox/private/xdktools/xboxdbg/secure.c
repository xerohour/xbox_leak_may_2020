/*
 *
 * secure.c
 *
 * Connection security functions
 *
 */

#include "precomp.h"
#include <benaloh.h>

static UCHAR g_rgbOakleyGroup1Base[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02
};

static UCHAR g_rgbOakleyGroup1Mod[] =
{
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xc9, 0x0f, 0xda, 0xa2, 0x21, 0x68, 0xc2, 0x34,
    0xc4, 0xc6, 0x62, 0x8b, 0x80, 0xdc, 0x1c, 0xd1,
    0x29, 0x02, 0x4e, 0x08, 0x8a, 0x67, 0xcc, 0x74,
    0x02, 0x0b, 0xbe, 0xa6, 0x3b, 0x13, 0x9b, 0x22,
    0x51, 0x4a, 0x08, 0x79, 0x8e, 0x34, 0x04, 0xdd,
    0xef, 0x95, 0x19, 0xb3, 0xcd, 0x3a, 0x43, 0x1b,
    0x30, 0x2b, 0x0a, 0x6d, 0xf2, 0x5f, 0x14, 0x37,
    0x4f, 0xe1, 0x35, 0x6d, 0x6d, 0x51, 0xc2, 0x45,
    0xe4, 0x85, 0xb5, 0x76, 0x62, 0x5e, 0x7e, 0xc6,
    0xf4, 0x4c, 0x42, 0xe9, 0xa6, 0x3a, 0x36, 0x20,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

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

HRESULT HrSetupKeyExchange(PDM_CONNECTION pdcon, PULARGE_INTEGER pluKey)
{
    int i;
    HRESULT hr;
    DWORD cb;
    PULARGE_INTEGER plu;
    BYTE rgbDH[96];
    BYTE rgbX[96];
    BYTE rgbGY[96];
    char szCmd[256];

    // Send the KEYXCHG command to start DH
    hr = DmSendCommand(pdcon, "KEYXCHG", NULL, NULL);
    if(hr != XBDM_READYFORBIN) {
        if(SUCCEEDED(hr)) {
            TerminateConnection(pdcon);
            hr = E_UNEXPECTED;
        }
        return hr;
    }

    /* Perform Diffie-Hellman key exchange.  We make up a random number
     * X and send g^X to the Xbox.  We receive g^Y and use that to compute
     * g^XY.  The hash of that key is what we use as our key-exchange key.
     * The random number X is the current system time, repeatedly hashed into
     * the rdtsc value, xor'ed with whatever junk was in rgbX to start with */
    GetSystemTimeAsFileTime((PFILETIME)rgbX);
    for(i = 1; i < 96/8; ++i) {
        plu = (PULARGE_INTEGER)rgbX + i;
        _asm {
            mov ecx, plu
            rdtsc
            xor [ecx], eax
            xor [ecx+4], edx
        }
        XBCHashBlock(plu, plu - 1);
    }
    BenalohModExp((PULONG)rgbDH, (PULONG)g_rgbOakleyGroup1Base,
        (PULONG)rgbX, (PULONG)g_rgbOakleyGroup1Mod, 96/4);
    hr = DmSendBinary(pdcon, rgbDH, sizeof rgbDH);
    if(FAILED(hr)) {
Failure:
        TerminateConnection(pdcon);
        return hr;
    }
    cb = sizeof szCmd;
    hr = DmReceiveStatusResponse(pdcon, szCmd, &cb);
    if(hr != XBDM_BINRESPONSE) {
        hr = E_UNEXPECTED;
        goto Failure;
    }
    hr = DmReceiveBinary(pdcon, rgbGY, sizeof rgbGY, &cb);
    if(FAILED(hr))
        goto Failure;
    BenalohModExp((PULONG)rgbDH, (PULONG)rgbGY, (PULONG)rgbX,
        (PULONG)g_rgbOakleyGroup1Mod, 96/4);
    pluKey->QuadPart = 0;
    XBCHashData(pluKey, rgbDH, sizeof rgbDH);

    return XBDM_NOERR;
}

HRESULT HrAuthenticateUser(PDM_CONNECTION pdcon, PULARGE_INTEGER pluBoxId,
    PULARGE_INTEGER pluConnectNonce)
{
    int i;
    HRESULT hr;
    HKEY h;
    DWORD reg;
    DWORD cb;
    PULARGE_INTEGER plu;
    ULARGE_INTEGER luSeed;
    ULARGE_INTEGER luPasswd;
    ULARGE_INTEGER luResp;
    ULARGE_INTEGER luDH;
    struct sockaddr_in sin;
    char szName[MAX_COMPUTERNAME_LENGTH+1];
    char szCmd[MAX_COMPUTERNAME_LENGTH+64];

    pdcon->fAuthenticationAttempted = TRUE;

    /* Make sure we have a name */
    cb = sizeof szName;
    if(!GetComputerName(szName, &cb))
        return E_OUTOFMEMORY;

    /* See if we can find our passwd seed */
	if(ERROR_SUCCESS != RegCreateKeyEx(HKEY_LOCAL_MACHINE,
            "Software\\Microsoft\\XboxSDK", 0, NULL, 0, KEY_QUERY_VALUE |
            KEY_SET_VALUE, NULL, &h, NULL))
        return E_OUTOFMEMORY;
    cb = sizeof luSeed;
    if(ERROR_SUCCESS != RegQueryValueEx(h, "SecuritySeed", NULL, &reg,
        (PBYTE)&luSeed, &cb) || reg != REG_BINARY)
    {
        /* Either the data was missing or it wasn't what we want.  We'll make
         * up a new number */
        _asm {
            lea ecx, luSeed
            rdtsc
            mov [ecx], eax
            mov [ecx+4], edx
        }
        cb = sizeof sin;
        getsockname(pdcon->s, (struct sockaddr *)&sin, (int *)&cb);
        XBCHashData(&luSeed, (PBYTE)&sin, sizeof sin);
        if(ERROR_SUCCESS != RegSetValueEx(h, "SecuritySeed", 0, REG_BINARY,
            (PBYTE)&luSeed, sizeof luSeed))
        {
            RegCloseKey(h);
            return E_OUTOFMEMORY;
        }
    }

    /* Compute the password we want to use */
    XBCCross(&luSeed, pluBoxId, &luPasswd);

    /* Compute the response we're supposed to send */
    XBCCross(&luPasswd, pluConnectNonce, &luResp);

    /* First attempt: authenticate normally, using response */
    sprintf(szCmd, "AUTHUSER NAME=\"%s\" RESP=0q%08x%08x", szName,
        luResp.HighPart, luResp.LowPart);
    cb = sizeof szCmd;
    hr = DmSendCommand(pdcon, szCmd, szCmd, &cb);
    if(hr != XBDM_KEYXCHG)
        /* Either we've succeeded or failed at this point, so we can just
         * return */
        return hr;

    /* We're authenticatable, but haven't sent a password yet.  We DH key
     * exchange and then send the password over */
    hr = HrSetupKeyExchange(pdcon, &luDH);

    /* Now send the password */
    luPasswd.LowPart ^= luDH.LowPart;
    luPasswd.HighPart ^= luDH.HighPart;
    sprintf(szCmd, "AUTHUSER NAME=\"%s\" PASSWD=0q%08x%08x", szName,
        luPasswd.HighPart, luPasswd.LowPart);
    cb = sizeof szCmd;
    hr = DmSendCommand(pdcon, szCmd, szCmd, &cb);
    pdcon->fAuthenticated = SUCCEEDED(hr);

    /* That's all we can do */
    return hr;
}

DWORD DmplFromSz(LPCSTR sz)
{
    ULONG dmpl = 0;

    if(PchGetParam(sz, "read", FALSE, TRUE))
        dmpl |= DMPL_PRIV_READ;
    if(PchGetParam(sz, "write", FALSE, TRUE))
        dmpl |= DMPL_PRIV_WRITE;
    if(PchGetParam(sz, "control", FALSE, TRUE))
        dmpl |= DMPL_PRIV_CONTROL;
    if(PchGetParam(sz, "config", FALSE, TRUE))
        dmpl |= DMPL_PRIV_CONFIGURE;
    if(PchGetParam(sz, "manage", FALSE, TRUE))
        dmpl |= DMPL_PRIV_MANAGE;

    return dmpl;
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

HRESULT HrSetUserAccess(PDM_CONNECTION s, LPCSTR szUserName, DWORD dwAccess)
{
    char sz[1024];

    if(!szUserName)
        return E_INVALIDARG;

    sprintf(sz, "SETUSERPRIV NAME=\"%s\"", szUserName);
    FillAccessPrivSz(sz + strlen(sz), dwAccess);
    return HrDoOneLineCmd(s, sz);
}

HRESULT HrGetUserAccess(PDM_CONNECTION s, LPCSTR szUserName, LPDWORD lpdwAccess)
{
    char sz[1024];
    HRESULT hr;
    DWORD cch;
    LPCSTR psz;

    if(!lpdwAccess)
        return E_INVALIDARG;
    if(szUserName) {
        sprintf(sz, "GETUSERPRIV NAME=\"%s\"", szUserName);
        psz = sz;
    } else
        psz = "GETUSERPRIV ME";
    cch = sizeof sz;
    hr = DmSendCommand(s, psz, sz, &cch);
    if(SUCCEEDED(hr))
        *lpdwAccess = DmplFromSz(sz + 5);
    return hr;
}

HRESULT HrAddUser(PDM_CONNECTION s, LPCSTR szUserName, DWORD dwAccess)
{
    char sz[1024];
    HRESULT hr;

    if(!szUserName)
        return E_INVALIDARG;
    sprintf(sz, "USER NAME=\"%s\"", szUserName);
    FillAccessPrivSz(sz + strlen(sz), dwAccess);
    return HrDoOneLineCmd(s, sz);
}

HRESULT HrRemoveUser(PDM_CONNECTION s, LPCSTR szUserName)
{
    char sz[1024];
    HRESULT hr;

    if(!szUserName)
        return E_INVALIDARG;
    sprintf(sz, "USER NAME=\"%s\" REMOVE", szUserName);
    return HrDoOneLineCmd(s, sz);
}


HRESULT HrOpenUserList(SCI *psci, PDM_WALK_USERS *ppdmwu, LPDWORD lpdw)
{
    PDM_CONNECTION s;
    HRESULT hr;
    char sz[1024];
    struct _DM_WALKUSER *pwu, **ppwu;
    DWORD cch;
    DWORD c;

    if(!ppdmwu)
        return E_INVALIDARG;

    hr = HrDoOpenSharedConnection(psci, &s);
    if(FAILED(hr))
        return hr;
    hr = DmSendCommand(s, "USERLIST", NULL, 0);
    if(hr != XBDM_MULTIRESPONSE) {
        if(SUCCEEDED(hr)) {
            hr = E_UNEXPECTED;
fatal:
            TerminateConnection(s);
        }
        DoCloseSharedConnection(psci, s);
        return hr;
    }
    *ppdmwu = LocalAlloc(LMEM_FIXED, sizeof(struct _DM_WALK_USERS));
    if(!*ppdmwu) {
        hr = E_OUTOFMEMORY;
        goto fatal;
    }
    ppwu = &(*ppdmwu)->pwuFirst;
    for(c = 0; ; ++c) {
        cch = sizeof sz;
        hr = DmReceiveSocketLine(s, sz, &cch);
        if(FAILED(hr) || sz[0] == '.')
            break;
        if(sz[0] == '.')
            break;
        pwu = LocalAlloc(LMEM_FIXED, sizeof *pwu);
        if(!pwu) {
            hr = E_OUTOFMEMORY;
            break;
        }
        memset(pwu, 0, sizeof *pwu);
        FGetSzParam(sz, "name", pwu->dmu.UserName);
        pwu->dmu.AccessPrivileges = DmplFromSz(sz);
        *ppwu = pwu;
        ppwu = &pwu->pwuNext;
        ++c;
    }
    *ppwu = NULL;
    if(FAILED(hr))
        goto fatal;
    DoCloseSharedConnection(psci, s);
    if(lpdw)
        *lpdw = c;
    return XBDM_NOERR;
}

HRESULT HrWalkUserList(SCI *psci, PDM_WALK_USERS *ppdmwu, PDM_USER pdusr)
{
    HRESULT hr;
    PDM_CONNECTION s;
    struct _DM_WALKUSER *pwu, **ppwu;

    if(!ppdmwu || !pdusr)
        return E_INVALIDARG;

    if(!*ppdmwu) {
        hr = HrOpenUserList(psci, ppdmwu, NULL);
        if(FAILED(hr))
            return hr;
    }

    ppwu = &(*ppdmwu)->pwuFirst;
    pwu = *ppwu;
    if(!pwu)
        return XBDM_ENDOFLIST;
    *ppwu = pwu->pwuNext;
    memcpy(pdusr, &pwu->dmu, sizeof *pdusr);
    LocalFree(pwu);

    return XBDM_NOERR;
}

HRESULT DmCloseUserList(PDM_WALK_USERS pdmwu)
{
    struct _DM_WALKUSER *pwu;

    if(pdmwu) {
        pwu = pdmwu->pwuFirst;
        if(pwu) {
            pdmwu->pwuFirst = pwu->pwuNext;
            LocalFree(pwu);
        }
        LocalFree(pdmwu);
    }
    
    return XBDM_NOERR;
}

HRESULT HrSetAdminPassword(PDM_CONNECTION s, LPCSTR szPasswd)
{
    ULARGE_INTEGER luPasswd;
    ULARGE_INTEGER luKey;
    HRESULT hr;
    char sz[256];

    if(!szPasswd) {
        /* We're removing the password */
        return HrDoOneLineCmd(s, "ADMINPW NONE");
    }

    hr = HrSetupKeyExchange(s, &luKey);
    if(SUCCEEDED(hr)) {
        luPasswd.QuadPart = 0;
        XBCHashData(&luPasswd, szPasswd, strlen(szPasswd));
        luPasswd.LowPart ^= luKey.LowPart;
        luPasswd.HighPart ^= luKey.HighPart;
        sprintf(sz, "ADMINPW PASSWD=0q%08x%08x", luPasswd.HighPart,
            luPasswd.LowPart);
        hr = HrDoOneLineCmd(s, sz);
    }

    return hr;
}

HRESULT HrIsSecurityEnabled(PDM_CONNECTION pdcon, LPBOOL pfEnabled)
{
    HRESULT hr;

    if(!pfEnabled)
        return E_INVALIDARG;

    /* We ask the box for its ID.  We don't care what the box ID actually is,
     * just whether it has one.  That will tell us whether the box is locked
     * or not, or whether the box doesn't understand the boxid concept */
    hr = DmSendCommand(pdcon, "BOXID", NULL, 0);
    if(SUCCEEDED(hr))
        *pfEnabled = TRUE;
    else {
        switch(hr) {
        case XBDM_INVALIDCMD:
            /* The box doesn't understand the command, which doesn't necessarily
             * mean that the box isn't locked.  We check the state at the time
             * this connection was established */
            *pfEnabled = pdcon->fAuthenticationAttempted;
            hr = XBDM_NOERR;
            break;
        case XBDM_NOTLOCKED:
            *pfEnabled = FALSE;
            hr = XBDM_NOERR;
            break;
        }
    }

    return hr;
}

HRESULT HrEnableSecurity(PDM_CONNECTION s, BOOL fEnable)
{
    char sz[256];
    LPCSTR psz;
    ULARGE_INTEGER luBoxId;
    struct sockaddr_in rgsin[3];
    int cb;
    FILETIME ft;
    HRESULT hr;

    if(!fEnable)
        psz = "LOCKMODE UNLOCK";
    else {
        /* We prohibit enabling of security on pre-4100 build boxes.  We
         * do this by getting the current boxid.  If the boxid command is
         * unsupported, we assume this is a pre-4100 box and we return an
         * error */
        hr = DmSendCommand(s, "BOXID", NULL, 0);
        if(XBDM_INVALIDCMD == hr)
            return hr;

        /* We need to construct a unique box id to lock this box.  We'll hash
         * our IP address, the box's IP address, and the time we did the
         * lock */
        memset(rgsin, 0, sizeof rgsin);
        cb = sizeof rgsin[0];
        getsockname(s->s, (struct sockaddr *)&rgsin[0], &cb);
        cb = sizeof rgsin[1];
        getpeername(s->s, (struct sockaddr *)&rgsin[1], &cb);
        GetSystemTimeAsFileTime(&ft);
        memset(&rgsin[0], 0, sizeof rgsin);
        memcpy(&rgsin[2], &ft, sizeof ft);
        XBCHashData(&luBoxId, (PUCHAR)rgsin, sizeof rgsin);
        sprintf(sz, "LOCKMODE BOXID=0q%08x%08x", luBoxId.HighPart,
            luBoxId.LowPart);
        psz = sz;
    }

    return DmSendCommand(s, psz, NULL, 0);
}
