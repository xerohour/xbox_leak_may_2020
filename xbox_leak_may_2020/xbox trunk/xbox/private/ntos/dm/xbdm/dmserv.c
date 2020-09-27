/*
 *
 * dmserv.c
 *
 * The debug monitor server
 *
 */

#include "dmp.h"
#include "xboxp.h"
#include "xconfig.h"
#include "av.h"
#include "xlaunch.h"
#undef DeleteFile
#include "d3d8.h"

void ShowIPAddress(ULONG);
DWORD DedicatedServThread(LPVOID lpv);

RTL_CRITICAL_SECTION csEch;
RTL_CRITICAL_SECTION csAccept;
CST rgcst[MAX_CONNECTIONS];
const char rgchHex[] = "0123456789ABCDEF";
char rgchDbgName[256];
ULONG ulIpAddr;
BOOL g_fServShutdown;
BOOL g_fDebugging;
BOOL g_fLockLevel;
BOOL g_fDebugSecureMode;// = TRUE;
ULARGE_INTEGER g_luBoxId;
KEVENT kevtServ;
PETHREAD pthrServ;
extern CHH rgchh[];
extern int cchh;

INITIALIZED_OBJECT_STRING_RDATA(ostDash, "\\Device\\Harddisk0\\Partition1\\DASHBOARD.XBX");
CHAR XdkLauncherPathname[] = "\\Device\\Harddisk0\\Partition2\\xshell.xbe";

/* Extension stuff */
ECH *rgpech[MAX_ECH];

NTSTATUS
WINAPI
XWriteTitleInfoNoReboot(
    PCOSTR pszLaunchPath,
    PCOSTR pszDDrivePath,
    DWORD dwLaunchDataType,
    DWORD dwTitleId,
    PLAUNCH_DATA pLaunchData
    );

BOOL
WINAPI
XapiFormatFATVolume(
    POBJECT_STRING VolumePath
    );

void InitServ(void)
{
    InitializeCriticalSection(&csEch);
    InitializeCriticalSection(&csAccept);
    KeInitializeEvent(&kevtServ, NotificationEvent, FALSE);
}

void StopServ(void)
{
    int icst;
    PETHREAD pthr;

    /* Loop through the socket list and close each one down */
    EnterCriticalSection(&csAccept);
    g_fServShutdown = TRUE;
    for(icst = 0; icst < MAX_CONNECTIONS; ++icst) {
        if(rgcst[icst].s != INVALID_SOCKET) {
            /* If this socket isn't running on the current thread, it may be
             * inside of a select.  In that case, we take it off the thread,
             * stop the select, and give the thread a little bit of time to
             * exit the select */
            pthr = rgcst[icst].pthrDedicated;
            rgcst[icst].pthrDedicated = NULL;
            if(pthr != PsGetCurrentThread()) {
                KeAlertThread(&pthr->Tcb, KernelMode);
                Sleep(20);
            }
            shutdown(rgcst[icst].s, SD_SEND);
        }
    }
    LeaveCriticalSection(&csAccept);

    /* Wait for the sockets to close */
    Sleep(200);
}

int SgnCompareRgch(const char *sz1, const char *sz2, int cch)
{
    while(cch-- && *sz1) {
        char ch1 = *sz1++;
        char ch2 = *sz2++;
        if(ch1 >= 'a' && ch2 <= 'z')
            ch1 -= 32;
        if(ch2 >= 'a' && ch2 <= 'z')
            ch2 -= 32;
        if(ch1 != ch2)
            return ch1 - ch2;
    }
    if(*sz1)
        return *sz1;
    return cch < 0 ? 0 : -*sz2;
}

BOOL FEqualRgch(const char *sz1, const char *sz2, int cch)
{
    return SgnCompareRgch(sz1, sz2, cch) == 0;
}

DWORD DwHexFromSz(LPCSTR sz, LPCSTR *szOut)
{
    DWORD dw = 0;

    for(;;) {
        if(*sz >= '0' && *sz <= '9')
            dw = dw * 16 + (*sz - '0');
        else if(*sz >= 'A' && *sz <= 'F')
            dw = dw * 16 + (*sz - 'A' + 10);
        else if(*sz >= 'a' && *sz <= 'f')
            dw = dw * 16 + (*sz - 'a' + 10);
        else
            break;
        ++sz;
    }
    if(szOut)
        *szOut = sz;
    return dw;
}

DWORD DwFromSz(LPCSTR sz, int *pcchUsed)
{
    DWORD dw = 0;
    LPCSTR szStart = sz;

    /* Process decimal, octal, or hex */
    if(*sz == '0') {
        ++sz;
        if(*sz == 'x')
            dw = DwHexFromSz(++sz, &sz);
        else
            while(*sz >= '0' && *sz <= '7')
                dw = dw * 8 + (*sz++ - '0');
    } else
        while(*sz >= '0' && *sz <= '9')
            dw = dw * 10 + (*sz++ - '0');
    if(pcchUsed)
        *pcchUsed = sz - szStart;
    return dw;
}

void CloseConn(int icst)
{
    KIRQL irql;

    ASSERT(rgcst[icst].s != INVALID_SOCKET);
    closesocket(rgcst[icst].s);

    /* The server thread will attempt to read data from this socket once
     * pechDedicated is cleared, and may attempt to reuse this
     * connection state as soon as the socket ID is set to INVALID.  We
     * need to do all of this as one atomic operation */
    irql = KeRaiseIrqlToDpcLevel();
    rgcst[icst].dwStatus = 0;
    rgcst[icst].pechDedicated = NULL;
    rgcst[icst].pthrDedicated = NULL;
    rgcst[icst].s = INVALID_SOCKET;
    KeLowerIrql(irql);
}

void InitDmcc(int icst)
{
    rgcst[icst].dmcc.BytesRemaining = -1;
    rgcst[icst].dmcc.Buffer = NULL;
    rgcst[icst].dmcc.CustomData = &rgcst[icst].ccs;
    rgcst[icst].dmcc.HandlingFunction = NULL;
    rgcst[icst].dwStatus &= ~(CONN_BINARY | CONN_BINSEND | CONN_MULTILINE);
}

BOOL PrintSockLine(SOCKET s, LPCSTR sz)
{
    char szT[1024];
    int ich;

    for(ich = 0; sz[ich] && ich < sizeof szT - 2; ++ich)
        szT[ich] = sz[ich];

    if(sz[ich]) {
        /* Didn't get the whole string, so we have to send two packets */
        if(send(s, sz, strlen(sz), 0) <= 0)
            return FALSE;
        if(send(s, "\015\012", 2, 0) <= 0)
            return FALSE;
    } else {
        szT[ich++] = '\015';
        szT[ich++] = '\012';
        if(send(s, szT, ich, 0) <= 0)
            return FALSE;
    }
    return TRUE;
}

BOOL ReadSockLine(SOCKET s, char *sz, int cch)
{
    int cb;
    int ich = 0;

    --cch; /* save room for \0 */
    while(ich < cch) {
        cb = recv(s, &sz[ich], 1, 0);
        if(cb <= 0) {
            if(ich == 0)
                return FALSE;
            sz[ich] = 0;
            return TRUE;
        }
        if(sz[ich] == '\012') {
            sz[ich] = 0;
            return TRUE;
        }
        if(sz[ich] != '\015')
            ++ich;
    }
    sz[ich] = 0;
    return TRUE;
}

__inline BOOL FIsSpace(char ch)
{
    return ch == ' ' || ch == '\015' || ch == 0;
}

const char *PchGetParam(LPCSTR szCmd, LPCSTR szKey, BOOL fNeedValue)
{
    const char *pchTok;
    int cchTok;
    BOOL fQuote = FALSE;

    /* Skip the command */
    for(pchTok = szCmd; !FIsSpace(*pchTok); ++pchTok);

    while(*pchTok) {
        /* Skip leading spaces */
        while(*pchTok && FIsSpace(*pchTok))
            ++pchTok;
        if(!*pchTok)
            return NULL;
        for(cchTok = 0; !FIsSpace(pchTok[cchTok]); ++cchTok) {
            if(pchTok[cchTok] == '=') {
                if(FEqualRgch(szKey, pchTok, cchTok))
                    return pchTok + cchTok + 1; /* Skip the '=' */
                break;
            }
        }
        /* If we didn't see the '=' we need to check anyway */
        if(!fNeedValue && pchTok[cchTok] != '=' && FEqualRgch(szKey, pchTok,
                cchTok))
            return pchTok + cchTok;
        /* No match, so we need to skip past the value */
        pchTok += cchTok;
        while(*pchTok && (!FIsSpace(*pchTok) || fQuote))
            if(*pchTok++ == '"')
                fQuote = !fQuote;
    }
    return NULL;
}

int CchOfWord(LPCSTR sz)
{
    int cch;
    for(cch = 0; !FIsSpace(sz[cch]); ++cch);
    return cch;
}

void GetParam(LPCSTR szLine, LPSTR szBuf, int cchBuf)
{
    int cch = 0;
    BOOL fQuote = FALSE;

    while(cch < cchBuf-1 && *szLine && (!FIsSpace(*szLine) || fQuote)) {
        if(*szLine == '"') {
            if(fQuote && szLine[1] == '"') {
                /* Double quote inside a string gets copied as a single
                 * quote */
                szBuf[cch++] = '"';
                szLine += 2;
            } else {
                fQuote = !fQuote;
                ++szLine;
            }
        } else
            szBuf[cch++] = *szLine++;
    }
    szBuf[cch] = 0;
}

BOOL FGetSzParam(LPCSTR szLine, LPCSTR szKey, LPSTR szBuf, int cchBuf)
{
    LPCSTR pch = PchGetParam(szLine, szKey, TRUE);
    if(!pch)
        return FALSE;
    GetParam(pch, szBuf, cchBuf);
    return TRUE;
}

BOOL FGetDwParam(LPCSTR szLine, LPCSTR szKey, DWORD *pdw)
{
    int cch;
    char sz[32];
    LPCSTR pch = PchGetParam(szLine, szKey, TRUE);
    if(!pch)
        return FALSE;
    GetParam(pch, sz, sizeof sz);
    *pdw = DwFromSz(sz, &cch);
    return FIsSpace(sz[cch]);
}

BOOL FGetQwordParam(LPCSTR szLine, LPCSTR szKey, ULARGE_INTEGER *plu)
{
    int cch;
    char sz[32];
    LPCSTR pch;

    pch = PchGetParam(szLine, szKey, TRUE);
    if(!pch)
        return FALSE;
    GetParam(pch, sz, sizeof sz - 1);
    sz[sizeof sz - 1] = 0;

    /* Verify the 0q prefix */
    if(sz[0] != '0' || sz[1] != 'q')
        return FALSE;
    /* Make sure we have a bunch of hex characters */
    for(cch = 2; cch < sizeof sz && !FIsSpace(sz[cch]); ++cch) {
        if(!(sz[cch] >= '0' && sz[cch] <= '9' ||
                sz[cch] >= 'A' && sz[cch] <= 'F' ||
                sz[cch] >= 'a' && sz[cch] <= 'f'))
            return FALSE;
    }
    cch -= 2;
    if(cch <= 0)
        return FALSE;

    /* Move the text out to the end of the string and fill the preceding
     * characters with zeroes */
    memmove(&sz[sizeof sz - 1 - cch], &sz[2], cch);
    memset(sz, '0', sizeof sz - 1 - cch);

    /* Now parse out the two dwords */
    plu->LowPart = DwHexFromSz(&sz[sizeof sz - 9], NULL);
    sz[sizeof sz - 9] = 0;
    plu->HighPart = DwHexFromSz(&sz[sizeof sz - 17], NULL);
    return TRUE;
}

BOOL FGetNamedDwParam(LPCSTR szLine, LPCSTR szKey, DWORD *pdw, LPSTR szResp)
{
    if(!FGetDwParam(szLine, szKey, pdw)) {
        sprintf(szResp, "missing %s", szKey);
        return FALSE;
    }
    return TRUE;
}

ULONG UlAddrFromSz(LPCSTR sz)
{
    ULONG ul;
    int ib;
    int ich;

    for(ib = 0; ib < 4; ++ib) {
        BYTE b = 0;

        for(ich = 0; ich < 3; ++ich) {
            if(sz[ich] < '0' || sz[ich] > '9')
                break;
            b = 10 * b + (sz[ich] - '0');
        }
        if(ich == 0 || sz[ich] != (ib == 3 ? 0 : '.'))
            return 0;
        sz += ich + 1;
        ((BYTE *)&ul)[ib ^ 3] = b;
    }
    return ul;
}

LPCSTR SzStdResponse(HRESULT hr)
{
    LPCSTR pszResp;

    switch(hr) {
    case XBDM_NOSUCHFILE:
        pszResp = "file not found";
        break;
    case XBDM_NOMODULE:
        pszResp = "no such module";
        break;
    case XBDM_MEMUNMAPPED:
        pszResp = "memory not mapped";
        break;
    case XBDM_NOTHREAD:
        pszResp = "no such thread";
        break;
    case XBDM_INVALIDCMD:
        pszResp = "unknown command";
        break;
    case XBDM_NOTSTOPPED:
        pszResp = "not stopped";
        break;
    case XBDM_MUSTCOPY:
        pszResp = "file must be copied";
        break;
    case XBDM_ALREADYEXISTS:
        pszResp = "file already exists";
        break;
    case XBDM_DIRNOTEMPTY:
        pszResp = "directory not empty";
        break;
    case XBDM_BADFILENAME:
        pszResp = "filename is invalid";
        break;
    case XBDM_CANNOTCREATE:
        pszResp = "file cannot be created";
        break;
    case XBDM_DEVICEFULL:
        pszResp = "no room on device";
        break;
    case XBDM_MULTIRESPONSE:
        pszResp = "multiline response follows";
        break;
    case XBDM_BINRESPONSE:
        pszResp = "binary response follows";
        break;
    case XBDM_READYFORBIN:
        pszResp = "send binary data";
        break;
    case XBDM_CANNOTACCESS:
        pszResp = "access denied";
        break;
    case XBDM_NOTDEBUGGABLE:
        pszResp = "not debuggable";
        break;
    case XBDM_BADCOUNTTYPE:
        pszResp = "type invalid";
        break;
    case XBDM_COUNTUNAVAILABLE:
        pszResp = "data not available";
        break;
    case XBDM_NOTLOCKED:
        pszResp = "box is not locked";
        break;
    case XBDM_KEYXCHG:
        pszResp = "key exchange required";
        break;
    case XBDM_MUSTBEDEDICATED:
        pszResp = "dedicated connection required";
        break;
    case E_OUTOFMEMORY:
        pszResp = "out of memory";
        break;
    case E_UNEXPECTED:
        pszResp = "unexpected error";
        break;
    case E_INVALIDARG:
        pszResp = "bad parameter";
        break;
    case XBDM_NOERR:
        pszResp = "OK";
        break;
    default:
        pszResp = "";
        break;
    }
    return pszResp;
}

HRESULT HrFromStatus(NTSTATUS st, HRESULT hrDefault)
{
    switch(st) {
    case STATUS_DIRECTORY_NOT_EMPTY:
        return XBDM_DIRNOTEMPTY;
    case STATUS_OBJECT_NAME_COLLISION:
        return XBDM_ALREADYEXISTS;
    case STATUS_OBJECT_PATH_NOT_FOUND:
    case STATUS_OBJECT_NAME_NOT_FOUND:
        return XBDM_NOSUCHFILE;
    case STATUS_OBJECT_PATH_INVALID:
    case STATUS_OBJECT_NAME_INVALID:
        return XBDM_BADFILENAME;
    case STATUS_ACCESS_DENIED:
        return XBDM_CANNOTACCESS;
    case STATUS_DISK_FULL:
        return XBDM_DEVICEFULL;
    case STATUS_INSUFFICIENT_RESOURCES:
        return E_OUTOFMEMORY;
    case STATUS_INVALID_HANDLE:
        return E_INVALIDARG;
    }
    return hrDefault;
}

ULONG DmplOfConnection(PDM_CMDCONT pdmcc)
{
    if(!pdmcc)
        return -1;
    return rgcst[(PCST)pdmcc - rgcst].dmplCur;
}

BOOL FDedicateConnection(int icst, PECH pech)
{
    PETHREAD pthrExpect;
    HANDLE hthrNew;
    PVOID pfnCreateThread;

    pthrExpect = rgcst[icst].pthrDedicated;

    /* We should only be rededicating a connection running on the current
     * thread */
    if(PsGetCurrentThread() != pthrExpect) {
        DbgPrint("dm: rededicating a connection on the wrong thread!\n");
        return FALSE;
    }

    /* Create the new thread */
    if(pech) {
        pfnCreateThread = pech->pfnCreateThread;
        if(!pfnCreateThread)
            pfnCreateThread = CreateThread;
        hthrNew = (*(HANDLE (*)())pfnCreateThread)(NULL, 0,
            DedicatedServThread, (LPVOID)icst, CREATE_SUSPENDED, NULL);
        if(!hthrNew)
            return FALSE;
    } else
        pfnCreateThread = NULL;

    /* Send the success message */
    PrintSockLine(rgcst[icst].s, "205- connection dedicated");

    /* Mark the connection state for the new thread.  We mark the thread ID
     * as NULL; dedicated threads will fill it in when they start */
    rgcst[icst].pthrDedicated = NULL;
    rgcst[icst].pechDedicated = pech;

    /* Wake up the new thread */
    if(pfnCreateThread) {
        NtResumeThread(hthrNew, NULL);
        CloseHandle(hthrNew);
    } else {
        rgcst[icst].pthrDedicated = pthrServ;
        KeAlertThread(&pthrServ->Tcb, KernelMode);
    }

    return TRUE;
}

void SendHrSzResp(SOCKET s, HRESULT hr, LPCSTR szResp, LPSTR szBuf)
{
    /* Make sure we have an error code we like */
    if(((hr >> 16) & 0x7fff) != FACILITY_XBDM) {
        hr = SUCCEEDED(hr) ? XBDM_NOERR : XBDM_UNDEFINED;
        if(!szResp)
            szResp = SzStdResponse(E_UNEXPECTED);
    } else if((hr & 0xffff) > 0xff)
        hr = XBDM_UNDEFINED;

    if(FAILED(hr))
        szBuf[0] = '4';
    else
        szBuf[0] = '2';
    szBuf[1] = (char) ('0' + (hr & 0xffff) / 10);   // overflow?
    szBuf[2] = (char) ('0' + (hr & 0xffff) % 10);
    szBuf[3] = '-';
    szBuf[4] = ' ';
    if(szResp != szBuf) {
        if(szResp)
            strcpy(szBuf + 5, szResp);
        else
            szBuf[5] = 0;
    }
    PrintSockLine(s, szBuf);
}

HRESULT HrHandleBuiltinCommand(DWORD dmplCur, LPCSTR sz, int cchCmd,
    LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    HRESULT hr;
    int ichhMin, ichhMac, ichhMid;
    int sgn;
    BOOL fPermitted;

    ichhMin = 0;
    ichhMac = cchh;

    while(ichhMin < ichhMac) {
        ichhMid = (ichhMin + ichhMac) / 2;
        sgn = SgnCompareRgch(rgchh[ichhMid].szCommand, sz, cchCmd);
        if(sgn == 0)
            break;
        else if(sgn < 0)
            ichhMin = ichhMid + 1;
        else
            ichhMac = ichhMid;
    }

    if(ichhMin < ichhMac) {
        /* Check for appropriate permission */
        if(rgchh[ichhMid].dmplCombined)
            fPermitted = (dmplCur & rgchh[ichhMid].dmplCombined) ==
                rgchh[ichhMid].dmplCombined;
        else if(rgchh[ichhMid].dmplMask)
            fPermitted = dmplCur & rgchh[ichhMid].dmplMask;
        else
            fPermitted = TRUE;
        if(fPermitted) {
            /* Make sure we're running on the right thread */
            if(!pthrServ || PsGetCurrentThread() == pthrServ ||
                (rgchh[ichhMid].dwFlags & CHH_ANYTHREAD))
            {
                hr = (*rgchh[ichhMid].pfnHandler)(sz, szResp, cchResp, pdmcc);
            } else
                hr = XBDM_MUSTBEDEDICATED;
        } else
            hr = XBDM_CANNOTACCESS;
    } else
        hr = XBDM_INVALIDCMD;
    return hr;
}

void DoSendReceive(int icst, BOOL fSend)
{
    HRESULT hr;
    SOCKET s = rgcst[icst].s;
    char szBuf[256];
    char szResp[256];

    if(!rgcst[icst].dmcc.Buffer) {
        *szBuf = 0;
        rgcst[icst].dmcc.Buffer = szBuf;
        rgcst[icst].dmcc.BufferSize = sizeof szBuf;
    }
    szResp[0] = 0;
    if(!fSend) {
        int cb;
        /* We need to receive data.  First, we attempt to take it from our
         * command receive buffer */
        cb = rgcst[icst].dwStatus & CONN_RECEIVED;
        if(cb) {
            int ib;
            int ibT;

            ib = cb;
            if((DWORD)cb > rgcst[icst].dmcc.BytesRemaining)
                ib = rgcst[icst].dmcc.BytesRemaining;
            if((DWORD)cb > rgcst[icst].dmcc.BufferSize)
                ib = rgcst[icst].dmcc.BufferSize;
            if(ib) {
                rgcst[icst].dmcc.DataSize = ib;
                memcpy(rgcst[icst].dmcc.Buffer, rgcst[icst].szBuf, ib);
                for(ibT = 0; ib < cb; )
                    rgcst[icst].szBuf[ibT++] = rgcst[icst].szBuf[ib++];
                rgcst[icst].dwStatus = (rgcst[icst].dwStatus &
                    ~CONN_RECEIVED) | ibT;
            } else
                rgcst[icst].dmcc.DataSize = 0;
        } else {
            /* Receive as much data as we can */
            cb = rgcst[icst].dmcc.BufferSize;
            if((DWORD)cb > rgcst[icst].dmcc.BytesRemaining)
                cb = rgcst[icst].dmcc.BytesRemaining;
            if(cb) {
                cb = recv(s, rgcst[icst].dmcc.Buffer, cb, 0);
                if(cb <= 0) {
                    /* Socket is gone, so clean up and flag an error */
                    cb  = 0;
                    CloseConn(icst);
                }
            }
            rgcst[icst].dmcc.DataSize = cb;
        }
    }
    hr = rgcst[icst].dmcc.HandlingFunction(&rgcst[icst].dmcc, szResp,
        sizeof szResp);
    if(fSend) {
        if(SUCCEEDED(hr)) {
            if(rgcst[icst].dmcc.DataSize == -1) {
                /* We have a string to send, so send it */
                PrintSockLine(s, rgcst[icst].dmcc.Buffer);
            } else if(rgcst[icst].dmcc.DataSize) {
                /* We have data to send, so send it */
                int cbSend = rgcst[icst].dmcc.DataSize;
                while(cbSend) {
                    int cbSent = send(s, (PBYTE)rgcst[icst].dmcc.Buffer +
                        rgcst[icst].dmcc.DataSize - cbSend, cbSend, 0);
                    if(cbSent <= 0) {
                        /* A send error occurred, so close the connection and
                         * tell the handling function we can accept no more
                         * data */
                        rgcst[icst].dmcc.BytesRemaining = 0;
                        rgcst[icst].dmcc.HandlingFunction(&rgcst[icst].dmcc,
                            szResp, sizeof szResp);
                        CloseConn(icst);
                        break;
                    } else
                        cbSend -= cbSent;
                }
            }
        } else if(hr == XBDM_ENDOFLIST) {
            /* We're out of data, so send end-of-data if appropriate and go
             * back to text mode */
            if(rgcst[icst].dwStatus & CONN_MULTILINE)
                PrintSockLine(s, ".");
            InitDmcc(icst);
        } else {
            /* An unexpected error has occurred.  Forcibly terminate the
             * connection */
            CloseConn(icst);
        }
    } else if(rgcst[icst].dmcc.BytesRemaining == 0) {
        /* If we've reached the end of our data stream, we can give the
         * appropriate response */
        LPCSTR pszResp;

        /* We may have an extended response, in which case we need to
         * prepare for it */
        rgcst[icst].dwStatus &= ~CONN_BINARY;
        rgcst[icst].dmcc.BytesRemaining = -1;
        switch(hr) {
        case XBDM_MULTIRESPONSE:
            rgcst[icst].dwStatus |= CONN_MULTILINE;
            rgcst[icst].dmcc.DataSize = -1;
            break;
        case XBDM_BINRESPONSE:
            rgcst[icst].dwStatus |= CONN_BINSEND;
            break;
        default:
            InitDmcc(icst);
            break;
        }
        if(rgcst[icst].dwStatus & (CONN_MULTILINE | CONN_BINSEND | CONN_BINARY) &&
            !rgcst[icst].dmcc.HandlingFunction)
        {
            /* We gotta fail this */
            hr = E_UNEXPECTED;
            rgcst[icst].dwStatus &= ~(CONN_MULTILINE | CONN_BINSEND | CONN_BINARY);
            szResp[0] = 0;
        }

        pszResp = szResp[0] ? szResp : SzStdResponse(hr);
        SendHrSzResp(s, hr, pszResp, szBuf);
    }

    if(rgcst[icst].dmcc.Buffer == szBuf)
        rgcst[icst].dmcc.Buffer = NULL;
}

PECH PechFindHandler(LPCSTR sz, int cch)
{
    int iech;

    /* Look up the command processor */
    for(iech = 0; iech < MAX_ECH; ++iech) {
        if(rgpech[iech] && FEqualRgch(rgpech[iech]->szExtName, sz, cch))
            return rgpech[iech];
    }
    return NULL;
}

void HandleCommand(int icst, LPCSTR sz)
{
    int cchCmd = CchOfWord(sz);
    HRESULT hr = 0;
    int ich;
    char szResp[512];
    LPCSTR pszResp;
    PDM_CMDCONT pdmcc;
    DWORD dmplCur;
    PECH pech = NULL;

    /* Make sure we're prepared to call this thing */
    if(icst >= 0) {
        pdmcc = &rgcst[icst].dmcc;
        InitDmcc(icst);
        dmplCur = rgcst[icst].dmplCur;
    } else {
        pdmcc = NULL;
        dmplCur = -1;
    }
    szResp[5] = 0;
    /* If this command belongs to an external command processor, we send it
     * there */
    for(ich = 0; ich < cchCmd; ++ich) {
        if(sz[ich] == '!') {
            BOOL fMatchingConnection;

            /* Look up the command processor */
            pech = PechFindHandler(sz, ich);
            if(pech) {
                /* All external commands require control permission */
                if(dmplCur & DMPL_PRIV_CONTROL) {
                    /* Make sure this handler was appropriately dedicated, if
                     * necessary */
                    if(pech == rgcst[icst].pechDedicated)
                        fMatchingConnection = TRUE;
                    else if(!pech->pfnCreateThread && !rgcst[icst].pechDedicated)
                        fMatchingConnection = TRUE;
                    else
                        fMatchingConnection = FALSE;
                    if(fMatchingConnection) {
                        hr = pech->pfnHandler(sz, szResp + 5,
                            sizeof szResp - 5, pdmcc);
                        if(!hr)
                            hr = XBDM_NOERR;
                    } else
                        hr = XBDM_MUSTBEDEDICATED;
                } else
                    hr = XBDM_CANNOTACCESS;
            } else {
                /* No command handler, can't do anything */
                hr = XBDM_INVALIDCMD;
                *szResp = 0;
            }
            break;
        } else if(FIsSpace(sz[ich]))
            break;
    }

    if(!hr) {
        /* Nobody has handled it yet, so we use our own internal handler.
         * Internal commands must be run on the server thread to avoid
         * possible deadlock if xapi is stopped */
        hr = HrHandleBuiltinCommand(dmplCur, sz, cchCmd, szResp + 5,
            sizeof szResp - 5, pdmcc);
    }

    if(hr && icst >= 0) {
        /* We have some sort of response to process.  First convert to a known
         * error code */
        if(szResp[5])
            pszResp = szResp;
        else
            pszResp = SzStdResponse(hr);

        /* We may have to indicate a special mode */
        switch(hr) {
        case XBDM_MULTIRESPONSE:
            rgcst[icst].dwStatus |= CONN_MULTILINE;
            rgcst[icst].dmcc.DataSize = -1;
            break;
        case XBDM_BINRESPONSE:
            rgcst[icst].dwStatus |= CONN_BINSEND;
            break;
        case XBDM_READYFORBIN:
            rgcst[icst].dwStatus |= CONN_BINARY;
            break;
        }
        if(rgcst[icst].dwStatus & (CONN_MULTILINE | CONN_BINSEND | CONN_BINARY) &&
            !rgcst[icst].dmcc.HandlingFunction)
        {
            /* We gotta fail this */
            hr = E_UNEXPECTED;
            rgcst[icst].dwStatus &= ~(CONN_MULTILINE | CONN_BINSEND | CONN_BINARY);
        }

        if(hr == XBDM_DEDICATED) {
            /* We need to spin off a thread to handle this connection */
            if(FDedicateConnection(icst, pech))
                /* The new thread will send the dedicated message */
                hr = 0;
            else
                hr = E_FAIL;
        }
    }

    if(hr && icst >= 0) {
        /* Now send our response */
        SendHrSzResp(rgcst[icst].s, hr, pszResp, szResp);

        if(hr == XBDM_READYFORBIN && pdmcc->BytesRemaining == 0)
            /* Zero-length receive, so fake up the receive now to end it */
            DoSendReceive(icst, FALSE);
    }
}

void AnswerName(SOCKET s)
{
    struct {
        BYTE bRequest;
        BYTE cchName;
        char szName[256];
    } nm;
    struct sockaddr_in sin;
    int cbAddr = sizeof sin;
    int cbPkt;

    cbPkt = recvfrom(s, (PVOID)&nm, sizeof nm, 0, (struct sockaddr *)&sin,
        &cbAddr);
    switch(nm.bRequest) {
    case 1:
    case 3:
        /* Name request.  1 is to match the name, 3 is to match anything */
        if(nm.bRequest == 1) {
            nm.bRequest = 0;
            /* Need to ensure more than two bytes received before we can
             * look at nm.cchName */
            if(cbPkt < 2 || cbPkt != 2 + nm.cchName)
                /* Ill-formed packet, do nothing */
                break;
            nm.szName[nm.cchName] = 0;
            /* Is it us? */
            if(!nm.cchName || !FEqualRgch(nm.szName, rgchDbgName, nm.cchName)
                    || rgchDbgName[nm.cchName] != 0)
                /* Not for us */
                break;
        }
        /* Answer the request */
        nm.bRequest = 2;
        for(nm.cchName = 0; rgchDbgName[nm.cchName]; ++nm.cchName)
            nm.szName[nm.cchName] = rgchDbgName[nm.cchName];
        break;
    default:
        /* We won't answer a request we don't recognize */
        nm.bRequest = 0;
        break;
    }
    if(nm.bRequest)
        sendto(s, (PVOID)&nm, nm.cchName + 2, 0, (struct sockaddr *)&sin,
            cbAddr);
}

void HandleSocketRead(int icst)
{
    if(rgcst[icst].dwStatus & CONN_BINARY)
        DoSendReceive(icst, FALSE);
    else {
        /* Text data coming in; read it */
        int cbRcv;
        int cbBuf;
        char *pchBuf;
        char rgbBufT[64];
        int ich;

        if(rgcst[icst].dwStatus & CONN_OVERFLOW) {
            /* Overflowed the buffer, ignore the data */
            pchBuf = rgbBufT;
            cbBuf = MAX_CONN_BUF - sizeof rgbBufT;
        } else {
            cbBuf = rgcst[icst].dwStatus & CONN_RECEIVED;
            pchBuf = &rgcst[icst].szBuf[cbBuf];
        }
        cbRcv = recv(rgcst[icst].s, pchBuf, MAX_CONN_BUF -
            cbBuf, 0);
        if(cbRcv <= 0) {
            /* Connection is bad, give up */
            CloseConn(icst);
            return;
        }
        if(!(rgcst[icst].dwStatus & CONN_OVERFLOW)) {
            rgcst[icst].dwStatus += cbRcv;
            if((rgcst[icst].dwStatus & CONN_RECEIVED) ==
                    MAX_CONN_BUF)
                rgcst[icst].dwStatus |= CONN_OVERFLOW;
        }
        if(pchBuf != rgcst[icst].szBuf) {
            /* We're receiving additional data, so we may need to look
             * back one for the CR */
            --pchBuf;
            ++cbRcv;
        }
        /* Let's see if we got our end-of-line, which must be of the
         * form CR-LF or CR-0 */
        for(ich = 0; ich < cbRcv; ++ich)
            if(pchBuf[ich] == '\015')
                break;
        if(ich < cbRcv - 1) {
            /* Got EOL, so let's do our requisite processing */
            if(rgcst[icst].dwStatus & CONN_OVERFLOW) {
                PrintSockLine(rgcst[icst].s, "406- line too long");
                rgcst[icst].dwStatus &= ~(CONN_OVERFLOW | CONN_RECEIVED);
            } else {
                pchBuf[ich++] = 0;
                HandleCommand(icst, rgcst[icst].szBuf);
                if(rgcst[icst].s != INVALID_SOCKET) {
                    int ichNew = 0;
                    while(++ich < cbRcv)
                        rgcst[icst].szBuf[ichNew++] = pchBuf[ich];
                    rgcst[icst].dwStatus &= ~CONN_RECEIVED;
                    rgcst[icst].dwStatus |= ichNew;
                }
            }
        }
    }
}

DWORD DedicatedServThread(LPVOID lpv)
{
    int icst = (int)lpv;

    DmGetCurrentDmtd()->DebugFlags |= DMFLAG_DEBUGTHREAD;
    rgcst[icst].pthrDedicated = PsGetCurrentThread();

    while(rgcst[icst].s != INVALID_SOCKET && rgcst[icst].pthrDedicated ==
        PsGetCurrentThread())
    {
        fd_set fds;
        BOOL fSend;

        FD_ZERO(&fds);
        FD_SET(rgcst[icst].s, &fds);
        fSend = rgcst[icst].dwStatus & (CONN_BINSEND | CONN_MULTILINE);

        while(select(0, fSend ? NULL : &fds, fSend ? &fds : NULL, NULL, 0) == 0);
        if(!FD_ISSET(rgcst[icst].s, &fds))
            /* How did this happen? */
            CloseConn(icst);
        else if(fSend)
            DoSendReceive(icst, TRUE);
        else
            HandleSocketRead(icst);
    }
    return 0;
}

DWORD ServerThread(LPVOID pv)
{
    SOCKET sockListen;
    SOCKET sockName;
    int icst;
    struct sockaddr_in sin;
    fd_set fds;
    fd_set fdsSend;
    NTSTATUS st;
    char sz[128];
    PULARGE_INTEGER plu;

    //Boost the base priority of this thread so that the dm is response, especially
    //important xbcp.
    KeSetBasePriorityThread(KeGetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    DmGetCurrentDmtd()->DebugFlags |= DMFLAG_DEBUGTHREAD;
    pthrServ = PsGetCurrentThread();

    for(icst = 0; icst < MAX_CONNECTIONS; ++icst)
        rgcst[icst].s = INVALID_SOCKET;
    sockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockListen == INVALID_SOCKET)
        return 0;
    st = TRUE;
    if(0 != setsockopt(sockListen, SOL_SOCKET, SO_REUSEADDR, (PVOID)&st,
        sizeof st))
    {
        _asm int 3
        closesocket(sockListen);
        return 0;
    }
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(DEBUGGER_PORT);
    if(0 != bind(sockListen, (struct sockaddr *)&sin, sizeof sin)) {
        _asm int 3
        closesocket(sockListen);
        return 0;
    }
    if(0 != listen(sockListen, MAX_CONNECTIONS)) {
        _asm int 3
        closesocket(sockListen);
        return 0;
    }

    /* Set up our name answering socket */
    sockName = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockName != INVALID_SOCKET) {
        BOOL fReuse = TRUE;
        if(0 != setsockopt(sockName, SOL_SOCKET, SO_REUSEADDR, (PVOID)&fReuse,
            sizeof fReuse))
        {
NameErr:
            closesocket(sockName);
            sockName = INVALID_SOCKET;
        } else {
            sin.sin_family = AF_INET;
            sin.sin_addr.s_addr = 0;
            sin.sin_port = htons(DEBUGGER_PORT);
            if(0 != bind(sockName, (struct sockaddr *)&sin, sizeof sin))
                goto NameErr;
        }
    }

    // DbgPrint("dm: listening\n");
    KeSetEvent(&kevtServ, EVENT_INCREMENT, FALSE);

    /* Now we wait for connections and answer what's there */
    for(;;) {
        do {
            FD_ZERO(&fds);
            FD_ZERO(&fdsSend);
            FD_SET(sockListen, &fds);
            if(sockName != INVALID_SOCKET)
                FD_SET(sockName, &fds);
            for(icst = 0; icst < MAX_CONNECTIONS; ++icst) {
                if(rgcst[icst].s != INVALID_SOCKET &&
                    rgcst[icst].pthrDedicated == PsGetCurrentThread())
                {
                    FD_SET(rgcst[icst].s, rgcst[icst].dwStatus &
                        (CONN_BINSEND | CONN_MULTILINE) ? &fdsSend : &fds);
                }
            }
        } while(select(0, &fds, &fdsSend, NULL, NULL) <= 0);

        /* Accept new connections */
        if(FD_ISSET(sockListen, &fds)) {
            SOCKET sockNew;
            int cb = sizeof sin;
            sockNew = accept(sockListen, (struct sockaddr *)&sin, &cb);
            if(sockNew == INVALID_SOCKET)
                continue;
            EnterCriticalSection(&csAccept);
            if(g_fServShutdown)
                icst = MAX_CONNECTIONS;
            else {
                for(icst = 0; icst < MAX_CONNECTIONS; ++icst) {
                    if(rgcst[icst].s == INVALID_SOCKET)
                        break;
                }
            }
            if(icst < MAX_CONNECTIONS) {
                rgcst[icst].s = sockNew;
                rgcst[icst].pthrDedicated = pthrServ;
            }
            LeaveCriticalSection(&csAccept);
            if(icst == MAX_CONNECTIONS) {
                PrintSockLine(sockNew, "401- max number of connections exceeded");
                fAllowKd = TRUE; // we're going to need to debug if this happens
                closesocket(sockNew);
            } else {
                if(g_fLockLevel) {
                    rgcst[icst].dmplCur = 0;
                    plu = &rgcst[icst].luConnectNonce;
                    KeQuerySystemTime((PLARGE_INTEGER)plu);
                    _asm {
                        mov ecx, plu
                        rdtsc
                        xor [ecx], eax
                        xor [ecx+4], edx
                    }
                    sprintf(sz, "201- boxid=0q%08x%08x nonce=0q%08x%08x",
                        g_luBoxId.HighPart, g_luBoxId.LowPart, plu->HighPart,
                        plu->LowPart);
                    PrintSockLine(sockNew, sz);
                } else {
                    rgcst[icst].dmplCur = -1;
                    PrintSockLine(sockNew, "201- connected");
                }
                rgcst[icst].dwStatus = 0;
                InitDmcc(icst);
            }
        } else if(FD_ISSET(sockName, &fds)) {
            /* Answer name requests */
            AnswerName(sockName);
        } else for(icst = 0; icst < MAX_CONNECTIONS; ++icst) {
            if(rgcst[icst].s == INVALID_SOCKET)
                continue;
            if(FD_ISSET(rgcst[icst].s, &fdsSend)) {
                DoSendReceive(icst, TRUE);
                continue;
            }
            if(FD_ISSET(rgcst[icst].s, &fds))
                HandleSocketRead(icst);
        }
    }
}

BOOL FStartDebugService(void)
{
    NTSTATUS st;
    HANDLE hthr;

    st = PsCreateSystemThread(&hthr, NULL, ServerThread, NULL, TRUE);
    return NT_SUCCESS(st);
}

HRESULT DmSetXboxName(LPCSTR szName)
{
    if(szName) {
        strncpy(rgchDbgName, szName, sizeof rgchDbgName);
        rgchDbgName[sizeof rgchDbgName - 1] = 0;
    } else
        rgchDbgName[0] = 0;
    WriteIniFile();
    return XBDM_NOERR;
}

HRESULT DmGetXboxName(LPSTR szName, LPDWORD pcch)
{
    DWORD ich;

    if (!szName || !pcch)
        return E_INVALIDARG;

    for(ich = 0; rgchDbgName[ich] && ich < *pcch - 1; ++ich)
        *szName++ = rgchDbgName[ich];
    *szName = 0;
    return rgchDbgName[ich] ? XBDM_BUFFER_TOO_SMALL : XBDM_NOERR;
}

HRESULT DmRegisterCommandProcessor(LPCSTR sz, PDM_CMDPROC pfn)
{
    return DmRegisterCommandProcessorEx(sz, pfn, NULL);
}

HRESULT DmRegisterCommandProcessorEx(LPCSTR sz, PDM_CMDPROC pfn,
    PVOID pfnCreateThread)
{
    int ich;
    int iech;
    ECH *pech;
    HRESULT hr;

    if (!sz)
        return E_INVALIDARG;

    RtlEnterCriticalSection(&csEch);
    if(!pfn) {
        /* We're going to unregister, so find the matching handler and pull
         * it */
        for(ich = 0; sz[ich]; ++ich);
        for(iech = 0; iech < MAX_ECH; ++iech) {
            if(rgpech[iech] && FEqualRgch(rgpech[iech]->szExtName, sz, ich)) {
                DmFreePool(rgpech[iech]);
                rgpech[iech] = NULL;
            }
        }
        hr = XBDM_NOERR;
    } else {
        /* Let's find somewhere to put it */
        for(iech = 0; iech < MAX_ECH; ++iech)
            if(!rgpech[iech])
                break;
        if(iech < MAX_ECH) {
            pech = DmAllocatePoolWithTag(sizeof *pech, 'hcmd');
        } else
            pech = NULL;
        if(pech) {
            for(ich = 0; sz[ich] && ich < sizeof pech->szExtName - 1; ++ich)
                pech->szExtName[ich] = sz[ich];
            if(sz[ich]) {
                DmFreePool(pech);
                hr = E_INVALIDARG;
            } else {
                pech->szExtName[ich] = 0;
                pech->pfnHandler = pfn;
                pech->pfnCreateThread = pfnCreateThread;
                rgpech[iech] = pech;
                hr = XBDM_NOERR;
            }
        } else
            hr = E_OUTOFMEMORY;
    }
    RtlLeaveCriticalSection(&csEch);
    return hr;
}

HRESULT HrDedicateConnection(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    char szHandler[256];
    int icst;
    PECH pech;

    if(!pdmcc)
        return E_INVALIDARG;

    if(PchGetParam(sz, "global", FALSE))
        pech = NULL;
    else if(!FGetSzParam(sz, "handler", szHandler, sizeof szHandler)) {
        strcpy(szResp, "missing handler");
        return E_FAIL;
    } else {
        pech = PechFindHandler(szHandler, strlen(szHandler));
        if(!pech)
            return XBDM_NOSUCHFILE;
    }

    icst = (PCST)pdmcc - rgcst;
    /* Rededicate the connection.  If this is successful, the success
     * message will be sent in the new thread */
    return FDedicateConnection(icst, pech) ? 0 : E_FAIL;
}

HRESULT DmThreadUserData(DWORD tid, PDWORD *ppdwRet)
{
    PETHREAD pthr;
    NTSTATUS st;
    HRESULT hr = XBDM_NOTHREAD;

    if (!ppdwRet)
        return E_INVALIDARG;

    if(tid == DM_CURRENT_THREAD) {
        pthr = PsGetCurrentThread();
        ObfReferenceObject(pthr);
        st = STATUS_SUCCESS;
    } else
        st = PsLookupThreadByThreadId((HANDLE)tid, &pthr);
    if(NT_SUCCESS(st)) {
        DMTD *pdmtd = DmGetCurrentDmtd();
        if(pdmtd) {
            *ppdwRet = &pdmtd->UserData;
            hr = XBDM_NOERR;
        }
        ObDereferenceObject(pthr);
    }
    return hr;
}

HRESULT HrEndConversation(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    int icst;

    if(!pdmcc)
        return E_INVALIDARG;
    icst = (PCST)pdmcc - rgcst;
    PrintSockLine(rgcst[icst].s, "200- bye");
    CloseConn(icst);
    return 0;
}

HRESULT HrSetDbgName(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    char szNewName[sizeof rgchDbgName];
    HRESULT hr;
    KIRQL irqlSav;

    if(!FGetSzParam(sz, "name", szNewName, sizeof szNewName)) {
        /* We're querying for the current name */
        strcpy(szResp, rgchDbgName);
        hr = XBDM_NOERR;
    } else if(!FConnectionPermission(pdmcc, DMPL_PRIV_CONFIGURE))
        hr = XBDM_CANNOTACCESS;
    else {
        irqlSav = KeRaiseIrqlToDpcLevel();
        memcpy(rgchDbgName, szNewName, sizeof rgchDbgName);
        hr = XBDM_NOERR;
        KeLowerIrql(irqlSav);
        WriteIniFile();
    }
    return hr;
}

HRESULT HrToggleGPUCounters(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    HRESULT hr;

    if(PchGetParam(sz, "enable", FALSE))
        hr = DmEnableGPUCounter(TRUE);
    else if(PchGetParam(sz, "disable", FALSE))
        hr = DmEnableGPUCounter(FALSE);
    return hr;
}

HRESULT HrToggleKDState(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    HRESULT hr = XBDM_NOERR;

    if(PchGetParam(sz, "enable", FALSE)) {
        fAllowKd = TRUE;
        strcpy(szResp, "kd enabled");
    } else if(PchGetParam(sz, "disable", FALSE)) {
        fAllowKd = FALSE;
        strcpy(szResp, "kd disabled");
    } else if(PchGetParam(sz, "except", FALSE)) {
        fAllowKd = TRUE;
        ExceptionsToKd();
    } else if(PchGetParam(sz, "exceptif", FALSE)) {
        /* Forward exceptions to KD only if KD is connected */
        if(*KdDebuggerEnabled) {
            fAllowKd = TRUE;
            ExceptionsToKd();
        }
    } else {
        strcpy(szResp, "improper command");
        hr = E_FAIL;
    }

    return hr;
}

HRESULT HrSetupNotifyAt(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    HRESULT hr;
    int icst;

    if(!pdmcc)
        return E_FAIL;
    icst = (PCST)pdmcc - rgcst;
    hr = FNotifyAtCmd(rgcst[icst].s, sz, FALSE) ? XBDM_NOERR :
        E_FAIL;
    /* Keep the ini file up-to-date */
    if(SUCCEEDED(hr))
        WriteIniFile();
    return hr;
}

HRESULT HrSetupNotify(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    int icst;
    HRESULT hr;

    if(!pdmcc)
        return E_FAIL;
    icst = (PCST)pdmcc - rgcst;

    if(FConvertToNotifySock(rgcst[icst].s)) {
        /* This socket is no longer our responsibility */
        rgcst[icst].s = INVALID_SOCKET;
        hr = 0;
    } else {
        strcpy(szResp, "notification limit exceeded");
        hr = E_FAIL;
    }
    return hr;
}

HRESULT HrDoStopon(BOOL fStop, LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    DWORD dw = 0;

    if(PchGetParam(sz, "all", FALSE)) {
        dw = -1;
        strcpy(szResp, fStop ? "stop on all" : "stop on none");
    } else {
        if(PchGetParam(sz, "fce", FALSE))
            dw |= DMSTOP_FCE;
        if(PchGetParam(sz, "debugstr", FALSE))
            dw |= DMSTOP_DEBUGSTR;
        if(PchGetParam(sz, "createthread", FALSE))
            dw |= DMSTOP_CREATETHREAD;
    }
    return DmStopOn(dw, fStop);
}

HRESULT HrStopon(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    return HrDoStopon(TRUE, sz, szResp, cchResp, pdmcc);
}

HRESULT HrNostopon(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    return HrDoStopon(FALSE, sz, szResp, cchResp, pdmcc);
}

HRESULT HrGo(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    if(dwExecState == DMN_EXEC_START) {
        sprintf(szResp, "not stopped");
        return E_FAIL;
    }
    DmGo();
    return XBDM_NOERR;
}

HRESULT HrStop(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    if(dwExecState != DMN_EXEC_START) {
        sprintf(szResp, "already stopped");
        return E_FAIL;
    }
    DmStop();
    return XBDM_NOERR;
}

BOOL FTitleExists(LPCSTR szDir, LPCSTR szTitle, LPCSTR szCmdLine, BOOL fMapIt)
{
    HANDLE h;
    OBJECT_ATTRIBUTES oa;
    OBJECT_STRING objectName;
    IO_STATUS_BLOCK iosb;
    BOOL fRet;
    NTSTATUS st;
    OCHAR oszPath[MAX_OBJ_PATH+1];
    OCHAR *pochTitle;
    OCHAR *pochMax;
    int ich;

    if(!szDir || !szDir[0]) {
		/* No dir name, so we expect szTitle to have the complete path name
		 * to the title */
		if(!szTitle || !FFileNameToObName(szTitle, oszPath, sizeof oszPath /
			sizeof(OCHAR)))
		{
			return FALSE;
		}
		pochTitle = NULL;
	} else {
		if(!FFileNameToObName(szDir, oszPath, sizeof oszPath / sizeof(OCHAR)))
			return FALSE;
		pochTitle = oszPath;
	}
    if(!szTitle)
        szTitle = "default.xbe";

	if(pochTitle) {
		/* Construct a complete path for the title and remember where both
		 * parts go */
		pochMax = oszPath + sizeof oszPath / sizeof(OCHAR);
		while(*pochTitle)
			++pochTitle;
		/* If we're in a root dir, don't add the extra '\' */
		if(pochTitle > oszPath && pochTitle[-1] != '\\')
			*pochTitle++ = '\\';
		for(ich = 0; &pochTitle[ich] < pochMax && szTitle[ich]; ++ich)
			pochTitle[ich] = szTitle[ich];
		if(&pochTitle[ich] >= pochMax - 1)
			return FALSE;
		pochTitle[ich] = 0;
	}

    /* Verify the file exists */
    RtlInitObjectString(&objectName, oszPath);
    InitializeObjectAttributes(&oa, &objectName, OBJ_CASE_INSENSITIVE, NULL,
        NULL);
    st = NtOpenFile(&h, SYNCHRONIZE, &oa, &iosb, FILE_SHARE_READ,
        FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);
    fRet = NT_SUCCESS(st);
    if(fRet) {
        NtClose(h);
        if(fMapIt) {
			if(pochTitle) {
				pochTitle[-1] = 0;
				pochMax = oszPath;
			} else {
				pochMax = NULL;
				pochTitle = oszPath;
			}
            /* We require that the command line passed in is the size of the
             * launch data, so we don't need to do any copying here */
            fRet = NT_SUCCESS(XWriteTitleInfoNoReboot(pochTitle, pochMax,
                szCmdLine ? LDT_FROM_DEBUGGER_CMDLINE : LDT_NONE, 0,
                (PLAUNCH_DATA) szCmdLine));
        }
    }
    return fRet;
}

HRESULT HrReceiveFileData(PDM_CMDCONT pdmcc, LPSTR szResponse,
    DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;
    FILE_DISPOSITION_INFORMATION fdi;
    IO_STATUS_BLOCK iosb;

    if(pdmcc->DataSize) {
        if(pccs->rgbBuf)
            /* Digest the data in the buffer */
            pccs->ib += pdmcc->DataSize;
        else
            pccs->ib = DISK_BUF_SIZE;
        pdmcc->BytesRemaining -= pdmcc->DataSize;
    } else {
        /* If there's no data, then either we had a zero-length file, or
         * we've lost our socket.  In the latter case, we need to delete
         * the file */
        if(pdmcc->BytesRemaining) {
            pdmcc->BytesRemaining = 0;
            fdi.DeleteFile = TRUE;
            NtSetInformationFile(pccs->hFile, &iosb, &fdi, sizeof(fdi),
                FileDispositionInformation);
        }
    }

    /* See if we're ready for a disk write */
    if(!pdmcc->BytesRemaining || pccs->ib == DISK_BUF_SIZE) {
        NTSTATUS st;

        if(pccs->hFile) {
            st = NtWriteFile(pccs->hFile, NULL, NULL, NULL, &iosb,
                pccs->rgbBuf, pccs->ib, NULL);
            /* Close the file if we're out of data */
            if(!NT_SUCCESS(st)) {
                /* If an error, try to delete the file and signal an error
                 * condition */
                fdi.DeleteFile = TRUE;
                NtSetInformationFile(pccs->hFile, &iosb, &fdi, sizeof(fdi),
                    FileDispositionInformation);
                NtClose(pccs->hFile);
                pccs->hFile = NULL;
                pccs->fError = TRUE;
            } else if(!pdmcc->BytesRemaining) {
                NtClose(pccs->hFile);
                pccs->hFile = NULL;
            }
        }
        pccs->ib = 0;
        if(!pdmcc->BytesRemaining && pccs->rgbBuf)
            /* Clean up our buffer */
            DmFreePool(pccs->rgbBuf);
    }

    /* Make sure the buffer pointer is set up */
    if(pccs->rgbBuf) {
        pdmcc->Buffer = pccs->rgbBuf + pccs->ib;
        pdmcc->BufferSize = DISK_BUF_SIZE - pccs->ib;
    }

    return pccs->fError ? E_UNEXPECTED : XBDM_NOERR;
}

HRESULT HrReceiveFile(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    LARGE_INTEGER liSize;
    NTSTATUS st;
    char szName[MAX_OBJ_PATH+1];
    CCS *pccs;
    IO_STATUS_BLOCK iosb;
    FILE_END_OF_FILE_INFORMATION feof;
    FILE_DISPOSITION_INFORMATION fdi;
    HRESULT hr;

    if(!pdmcc)
        return E_FAIL;
    if(!FGetSzParam(sz, "name", szName, sizeof szName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }
    if(!FGetNamedDwParam(sz, "length", &pdmcc->BytesRemaining, szResp))
        return E_FAIL;

    pccs = (CCS *)pdmcc->CustomData;
    liSize.HighPart = 0;
    liSize.LowPart = pdmcc->BytesRemaining;

    st = FCreateFile(&pccs->hFile, GENERIC_WRITE | DELETE | SYNCHRONIZE,
        szName, &liSize, 0, 0, FILE_OVERWRITE_IF,
        FILE_SYNCHRONOUS_IO_NONALERT);
    if(NT_SUCCESS(st)) {
        /* Set the size of the file now */
        feof.EndOfFile = liSize;
        st = NtSetInformationFile(pccs->hFile, &iosb, &feof, sizeof(feof),
            FileEndOfFileInformation);
        if(NT_SUCCESS(st)) {
            /* Make sure we'll have a buffer for received data */
            pccs->ib = 0;
            pccs->rgbBuf = DmAllocatePoolWithTag(DISK_BUF_SIZE, 'bdbd');
            /* Even if the alloc fails, we'll still have a small static buffer */
            pdmcc->Buffer = pccs->rgbBuf;
            pdmcc->BufferSize = DISK_BUF_SIZE;
            pccs->fError = FALSE;
            pdmcc->HandlingFunction = HrReceiveFileData;
            return XBDM_READYFORBIN;
        } else {
            /* Mark the file for deletion and close the file */
            fdi.DeleteFile = TRUE;
            NtSetInformationFile(pccs->hFile, &iosb, &fdi, sizeof(fdi),
                FileDispositionInformation);
            NtClose(pccs->hFile);
            pccs->hFile = NULL;
        }
    }
    return HrFromStatus(st, XBDM_CANNOTCREATE);
}

HRESULT HrSendFileData(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;

    if(pdmcc->BytesRemaining == 0) {
        /* End of file */
        NtClose(pccs->hFile);
        if(pccs->rgbBuf)
            DmFreePool(pccs->rgbBuf);
        return XBDM_ENDOFLIST;
    }
    if(!pccs->fDidSize) {
        /* First we need to send the length */
        pdmcc->BytesRemaining = pccs->ib;
        memcpy(pdmcc->Buffer, &pccs->ib, sizeof(DWORD));
        pdmcc->DataSize = sizeof(DWORD);
        pccs->fDidSize = TRUE;
    } else {
        NTSTATUS st;
        IO_STATUS_BLOCK iosb;
        DWORD cb;

        cb = pdmcc->BufferSize;
        if(cb > pdmcc->BytesRemaining)
            cb = pdmcc->BytesRemaining;
        st = NtReadFile(pccs->h, NULL, NULL, NULL, &iosb, pdmcc->Buffer, cb,
            NULL);
        if(!NT_SUCCESS(st))
            // BUG BUG -- need to signal read failure
            RtlZeroMemory(pdmcc->Buffer, cb);
        pdmcc->BytesRemaining -= cb;
        pdmcc->DataSize = cb;
    }
    return XBDM_NOERR;
}

HRESULT HrSendFile(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    NTSTATUS st;
    IO_STATUS_BLOCK iosb;
    FILE_NETWORK_OPEN_INFORMATION ni;
    char szName[MAX_OBJ_PATH+1];
    CCS *pccs;
    HRESULT hr;
    int icst;

    if(!pdmcc)
        return E_FAIL;
    if(!FGetSzParam(sz, "name", szName, sizeof szName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }

    pccs = (CCS *)pdmcc->CustomData;
    st = FCreateFile(&pccs->hFile, GENERIC_READ | SYNCHRONIZE, szName,
                    NULL, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT);
    if(NT_SUCCESS(st)) {
        st = NtQueryInformationFile(pccs->hFile, &iosb, &ni, sizeof ni,
            FileNetworkOpenInformation);
        if(NT_SUCCESS(st)) {
            if(ni.EndOfFile.HighPart) {
                /* File is too big to transfer */
                sprintf(szResp, "file is too big");
                hr = E_FAIL;
            } else {
                /* Get a buffer for sending if we can */
                pdmcc->BufferSize = 1024;
                pccs->rgbBuf = pdmcc->Buffer = DmAllocatePoolWithTag(
                    pdmcc->BufferSize, 'bdbd');
                pccs->ib = ni.EndOfFile.LowPart;
                pccs->fDidSize = FALSE;
                pdmcc->BytesRemaining = 1;
                pdmcc->HandlingFunction = HrSendFileData;
                return XBDM_BINRESPONSE;
            }
        } else
            hr = E_UNEXPECTED;
        NtClose(pccs->h);
    } else
        hr = HrFromStatus(st, XBDM_CANNOTACCESS);
    return hr;
}

void GetFileAttrSz(LPSTR sz, ULONG dwAttributes, PLARGE_INTEGER pliChange,
    PLARGE_INTEGER pliCreate, PLARGE_INTEGER pliSize)
{
    sprintf(sz, "sizehi=0x%x sizelo=0x%x createhi=0x%08x createlo=0x%08x "
        "changehi=0x%08x changelo=0x%08x%s%s%s", pliSize->HighPart,
        pliSize->LowPart, pliCreate->HighPart, pliCreate->LowPart,
        pliChange->HighPart, pliChange->LowPart,
        dwAttributes & FILE_ATTRIBUTE_DIRECTORY ? " directory" : "",
        dwAttributes & FILE_ATTRIBUTE_READONLY ? " readonly" : "",
        dwAttributes & FILE_ATTRIBUTE_HIDDEN ? " hidden" : "");
}

HRESULT HrReportFileAttributes(PDM_CMDCONT pdmcc, LPSTR szResponse,
    DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;

    for(;;) {
        switch(pdmcc->BytesRemaining++) {
        case 1:
            GetFileAttrSz(pdmcc->Buffer, pccs->fba.FileAttributes,
                &pccs->fba.LastWriteTime, &pccs->fba.CreationTime,
                &pccs->fba.EndOfFile);
            return XBDM_NOERR;
        default:
            return XBDM_ENDOFLIST;
        }
    }
}

HRESULT HrGetFileAttributes(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    OCHAR oszFilename[MAX_OBJ_PATH+1];
    NTSTATUS st;
    OBJECT_STRING objFilename;
    OBJECT_ATTRIBUTES oa;
    char szName[MAX_OBJ_PATH+1];
    CCS *pccs;

    if(!pdmcc)
        return E_FAIL;
    if(!FGetSzParam(sz, "name", szName, sizeof szName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }

    /* Turn the filename into an object manager name*/
    if(!FFileNameToObName(szName, oszFilename, sizeof oszFilename /
            sizeof(OCHAR)))
        return XBDM_NOSUCHFILE;
    RtlInitObjectString(&objFilename, oszFilename);
    InitializeObjectAttributes(&oa, &objFilename, OBJ_CASE_INSENSITIVE, NULL,
        NULL);

    pccs = (CCS *)pdmcc->CustomData;
    pdmcc->HandlingFunction = HrReportFileAttributes;
    pdmcc->BytesRemaining = 1;
    st = NtQueryFullAttributesFile(&oa, &pccs->fba);
    return NT_SUCCESS(st) ? XBDM_MULTIRESPONSE : HrFromStatus(st,
        XBDM_CANNOTACCESS);
}

HRESULT HrSetFileAttributes(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    NTSTATUS st;
    HANDLE h;
    IO_STATUS_BLOCK iosb;
    char szName[MAX_OBJ_PATH+1];
    HRESULT hr;

    if(!FGetSzParam(sz, "name", szName, sizeof szName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }

    st = FCreateFile(&h, SYNCHRONIZE, szName, NULL, 0,
        FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN,
        FILE_SYNCHRONOUS_IO_NONALERT);
    if(NT_SUCCESS(st)) {
        FILE_NETWORK_OPEN_INFORMATION fna;

        st = NtQueryInformationFile(h, &iosb, &fna, sizeof fna,
            FileNetworkOpenInformation);
        if(NT_SUCCESS(st)) {
            FILE_BASIC_INFORMATION fba;
            DWORD dwAttrMask = 0;
            DWORD dwAttrNew = 0;
            DWORD dwT;

            RtlZeroMemory(&fba, sizeof fba);
            if(FGetDwParam(sz, "readonly", &dwT)) {
                dwAttrMask |= FILE_ATTRIBUTE_READONLY;
                if(dwT)
                    dwAttrNew |= FILE_ATTRIBUTE_READONLY;
            }
            if(FGetDwParam(sz, "hidden", &dwT)) {
                dwAttrMask |= FILE_ATTRIBUTE_HIDDEN;
                if(dwT)
                    dwAttrNew |= FILE_ATTRIBUTE_HIDDEN;
            }
            FGetDwParam(sz, "createhi", &fba.CreationTime.HighPart);
            FGetDwParam(sz, "createlo", &fba.CreationTime.LowPart);
            FGetDwParam(sz, "changehi", &fba.LastWriteTime.HighPart);
            FGetDwParam(sz, "changelo", &fba.LastWriteTime.LowPart);
            fba.FileAttributes = fna.FileAttributes ^ ((fna.FileAttributes ^
                dwAttrNew) & dwAttrMask);
            if(!fba.FileAttributes)
                fba.FileAttributes = FILE_ATTRIBUTE_NORMAL;
            st = NtSetInformationFile(h, &iosb, &fba, sizeof fba,
                FileBasicInformation);
            hr = NT_SUCCESS(st) ? XBDM_NOERR : HrFromStatus(st, E_FAIL);
        }
        NtClose(h);
    } else
        hr = HrFromStatus(st, XBDM_CANNOTACCESS);
    return hr;
}

HRESULT HrRenameFile(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    NTSTATUS st;
    HANDLE h;
    IO_STATUS_BLOCK iosb;
    char  szName[MAX_OBJ_PATH+1];
    OCHAR oszNewName[MAX_OBJ_PATH+1];
    FILE_RENAME_INFORMATION fri;
    HRESULT hr;

    if(!FGetSzParam(sz, "newname", szName, sizeof szName)) {
        strcpy(szResp, "missing new name");
        return E_FAIL;
    }
    if(!FFileNameToObName(szName, oszNewName, MAX_OBJ_PATH+1)) {
        strcpy(szResp, "new name does not exist");
        return XBDM_NOSUCHFILE;
    }

    if(!FGetSzParam(sz, "name", szName, sizeof szName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }

    /* Get a handle to the original file */
    st = FCreateFile(&h, GENERIC_WRITE | SYNCHRONIZE, szName, NULL, 0, 0,
                    FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT);
    if(NT_SUCCESS(st)) {
        RtlZeroMemory(&fri, sizeof fri);
        RtlInitObjectString(&fri.FileName, oszNewName);
        st = NtSetInformationFile(h, &iosb, &fri, sizeof fri,
             FileRenameInformation);
        NtClose(h);
        switch(st) {
        case STATUS_NOT_SAME_DEVICE:
            hr = XBDM_MUSTCOPY;
            break;
        default:
            if(NT_SUCCESS(st))
                hr = XBDM_NOERR;
            else
                hr = HrFromStatus(st, XBDM_CANNOTCREATE);
            break;
        }
    } else
        hr = XBDM_CANNOTACCESS;

    return hr;
}

HRESULT HrDeleteFile(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    NTSTATUS st;
    HANDLE h;
    IO_STATUS_BLOCK iosb;
    FILE_DISPOSITION_INFORMATION fdi;
    char szName[MAX_OBJ_PATH+1];
    HRESULT hr;
    BOOL fIsDirectory;
    char *pch;

    if(!FGetSzParam(sz, "name", szName, sizeof szName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }
    fIsDirectory = PchGetParam(sz, "dir", FALSE) != NULL;

    /* We're going to prohibit the deletion of any drives */
    pch = szName;
    if(*pch == 'x' || *pch == 'X')
        ++pch;
    if(pch[1] == ':' && pch[2] == '\\' && pch[3] == 0)
        /* Looks like a drive name */
        return XBDM_CANNOTACCESS;

    st = FCreateFile(&h, DELETE | SYNCHRONIZE, szName, NULL, 0, 0,
        FILE_OPEN, fIsDirectory ?
        FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE :
        FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE);
    if(NT_SUCCESS(st)) {
        RtlZeroMemory(&fdi, sizeof fdi);
        fdi.DeleteFile = TRUE;
        st = NtSetInformationFile(h, &iosb, &fdi, sizeof fdi,
            FileDispositionInformation);
        if(NT_SUCCESS(st))
            hr = XBDM_NOERR;
        else
            hr = HrFromStatus(st, XBDM_CANNOTACCESS);
        NtClose(h);
    } else {
        switch(st) {
        case STATUS_NOT_A_DIRECTORY :
            sprintf(szResp, "not a directory");
            hr = XBDM_CANNOTACCESS;
            break;
        case STATUS_FILE_IS_A_DIRECTORY:
            sprintf(szResp, "is a directory");
            hr = XBDM_CANNOTACCESS;
            break;
        default:
            hr = HrFromStatus(st, XBDM_CANNOTACCESS);
            break;
        }
    }

    return hr;
}

HRESULT HrReportDirList(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    NTSTATUS st;
    IO_STATUS_BLOCK iosb;
    struct
    {
        FILE_DIRECTORY_INFORMATION fna;
        OCHAR ozName[MAX_OBJ_PATH+1];
    } fna;
    CCS *pccs = (CCS *)pdmcc->CustomData;
    HRESULT hr;
    LPSTR szResp;

    st = NtQueryDirectoryFile(pccs->h, NULL, NULL, NULL, &iosb,
        &fna.fna, sizeof fna, FileDirectoryInformation,
        NULL, (BOOLEAN)(pccs->i == 0));
    if(pdmcc->BytesRemaining && NT_SUCCESS(st))
    {
        ULONG ich;

        ++pccs->i;

        fna.fna.FileNameLength /= sizeof(OCHAR); // from bytes to cwch
        if(fna.fna.FileNameLength >= pdmcc->BufferSize)
            fna.fna.FileNameLength = pdmcc->BufferSize - 1;
        strcpy(pdmcc->Buffer, "name=\"");
        for(szResp = pdmcc->Buffer; *szResp; ++szResp);
        for(ich = 0; ich < fna.fna.FileNameLength; ++ich)
            szResp[ich] = (char)fna.fna.FileName[ich];
        szResp[ich++] = '"';
        szResp[ich++] = ' ';
        GetFileAttrSz(szResp + ich, fna.fna.FileAttributes,
            &fna.fna.LastWriteTime, &fna.fna.CreationTime, &fna.fna.EndOfFile);
        hr = XBDM_NOERR;
    } else {
        NtClose(pccs->h);
        DmFreePool(pdmcc->Buffer);
        hr = XBDM_ENDOFLIST;
    }
    return hr;
}

HRESULT HrGetDirList(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    NTSTATUS st;
    char szName[MAX_OBJ_PATH+1];
    CCS *pccs;

    if(!pdmcc)
        return E_FAIL;
    if(!FGetSzParam(sz, "name", szName, sizeof szName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }

    pccs = (CCS *)pdmcc->CustomData;
    pccs->i = 0;
    pdmcc->BufferSize = 512;
    pdmcc->Buffer = DmAllocatePool(pdmcc->BufferSize);
    if(!pdmcc->Buffer)
        /* We can't fall back on the default 256-byte buffer in this case */
        return E_OUTOFMEMORY;
    pdmcc->HandlingFunction = HrReportDirList;
    pdmcc->DataSize = -1;

    st = FCreateFile(&pccs->h, FILE_LIST_DIRECTORY | SYNCHRONIZE, szName, NULL, 0,
                FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN,
                FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);

    return NT_SUCCESS(st) ? XBDM_MULTIRESPONSE : HrFromStatus(st,
        XBDM_CANNOTACCESS);
}

HRESULT HrMkdir(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    NTSTATUS st;
    HANDLE h;
    char szName[MAX_OBJ_PATH+1];

    if(!FGetSzParam(sz, "name", szName, sizeof szName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }

    st = FCreateFile(&h, FILE_LIST_DIRECTORY | SYNCHRONIZE, szName,
        NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_CREATE, FILE_DIRECTORY_FILE |
        FILE_SYNCHRONOUS_IO_NONALERT);
    if(NT_SUCCESS(st)) {
        NtClose(h);
        return XBDM_NOERR;
    } else if(st == STATUS_OBJECT_NAME_COLLISION)
        return XBDM_ALREADYEXISTS;
    else
        return HrFromStatus(st, XBDM_CANNOTCREATE);
}

HRESULT HrFormatFAT(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    DWORD dwPartition;
    OCHAR szPartition[MAX_OBJ_PATH + 1];
    OBJECT_STRING ost;

    if(dwExecState != DMN_EXEC_PENDING) {
        strcpy(szResp, "not stopped");
        return E_FAIL;
    }
    if(!FGetNamedDwParam(sz, "partition", &dwPartition, szResp))
        return E_FAIL;
    if(dwPartition < 1 || dwPartition > 5)
        return XBDM_NOSUCHFILE;

    sprintf(szPartition, "\\Device\\Harddisk0\\Partition%d", dwPartition);
    RtlInitObjectString(&ost, szPartition);
    if(XapiFormatFATVolume(&ost))
        return XBDM_NOERR;

    strcpy(szResp, "unable to format");
    return XBDM_CANNOTACCESS;
}

HRESULT DmSetTitle(LPCSTR szDir, LPCSTR szTitle, LPCSTR pchCmdLine)
{
    if (!szTitle)
        return E_INVALIDARG;

    if(pchCmdLine) {
        KIRQL irql = KeRaiseIrqlToDpcLevel();
        if(pszCmdLine)
            DmFreePool(pszCmdLine);
        pszCmdLine = DmAllocatePool(sizeof(LD_FROM_DEBUGGER_CMDLINE));
        if(pszCmdLine)
            strcpy(pszCmdLine, pchCmdLine);
        KeLowerIrql(irql);
        if(!pszCmdLine)
            return E_OUTOFMEMORY;
    }

    if(!FTitleExists(szDir, szTitle, NULL, FALSE))
        return XBDM_CANNOTACCESS;
	if(szDir)
		strcpy(rgchTitleDir, szDir);
	else
		rgchTitleDir[0] = 0;
    strcpy(rgchTitleName, szTitle);
    return XBDM_NOERR;
}

HRESULT HrSetTitle(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    char szNewDir[sizeof rgchTitleDir];
    char szNewName[sizeof rgchTitleName];
	char *pszDir;

    pszDir = FGetSzParam(sz, "dir", szNewDir, sizeof szNewDir) ?
		szNewDir : NULL;
    if(!FGetSzParam(sz, "name", szNewName, sizeof szNewName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }
    return DmSetTitle(pszDir, szNewName, PchGetParam(sz, "cmdline", TRUE));
}

HRESULT HrSetDefaultTitle(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    char szLaunchData[MAX_LAUNCH_PATH + 1];
    char szLaunchDir[MAX_PATH + 1];
    NTSTATUS st;
    HANDLE h;
    IO_STATUS_BLOCK iosb;
    HRESULT hr;
    char *pch;
    OBJECT_STRING ost;
    OBJECT_ATTRIBUTES oa;
    FILE_DISPOSITION_INFORMATION fdi;

    // Make sure dashboard.xbx file isn't read-only
    InitializeObjectAttributes(&oa, &ostDash, OBJ_CASE_INSENSITIVE, NULL, NULL);
    st = NtOpenFile(&h, FILE_WRITE_ATTRIBUTES|SYNCHRONIZE, &oa, &iosb, 0,
            FILE_SYNCHRONOUS_IO_NONALERT);

    if (NT_SUCCESS(st)) {
        FILE_BASIC_INFORMATION BasicInfo;
        RtlZeroMemory(&BasicInfo,sizeof(BasicInfo));
        BasicInfo.FileAttributes = FILE_ATTRIBUTE_NORMAL;
        NtSetInformationFile(h, &iosb, &BasicInfo, sizeof(BasicInfo), FileBasicInformation);
        NtClose(h);
    }

    if(PchGetParam(sz, "none", FALSE)) {
        // Set the default title to be the dashboard
        InitializeObjectAttributes(&oa, &ostDash, OBJ_CASE_INSENSITIVE, NULL, NULL);
        st = NtOpenFile(&h, DELETE | SYNCHRONIZE, &oa, &iosb, 0,
            FILE_SYNCHRONOUS_IO_NONALERT);
        if(NT_SUCCESS(st)) {
            RtlZeroMemory(&fdi, sizeof fdi);
            fdi.DeleteFile = TRUE;
            st = NtSetInformationFile(h, &iosb, &fdi, sizeof fdi,
                FileDispositionInformation);
            hr = NT_SUCCESS(st) ? XBDM_NOERR : E_UNEXPECTED;
            NtClose(h);
        } else
            hr = XBDM_NOERR;
        return hr;
    }

    RtlZeroMemory(szLaunchData, sizeof szLaunchData);

    if (PchGetParam(sz, "launcher", FALSE)) {
        // Set the default title to be the xdk launcher
        strcpy(szLaunchData, XdkLauncherPathname);
        pch = strrchr(szLaunchData, '\\');
        ASSERT(pch != NULL);
        pch += 1;
    } else {
        // Set the default title to be the specified XBE
        if(!FGetSzParam(sz, "dir", szLaunchDir, sizeof szLaunchDir - 1)) {
            strcpy(szResp, "missing dir");
            return E_FAIL;
        }
        if(!FFileNameToObName(szLaunchDir, szLaunchData, sizeof szLaunchData - 1))
            return XBDM_NOSUCHFILE;
        for(pch = szLaunchData; *pch; ++pch);
        if(pch[-1] != '\\')
            *pch++ = '\\';
        if(!FGetSzParam(sz, "name", pch, szLaunchData + (sizeof szLaunchData - 1) - pch))
        {
            strcpy(szResp, "missing name");
            return E_FAIL;
        }
    }

    /* Verify the file exists */
    RtlInitObjectString(&ost, szLaunchData);
    InitializeObjectAttributes(&oa, &ost, OBJ_CASE_INSENSITIVE, NULL, NULL);
    st = NtOpenFile(&h, SYNCHRONIZE, &oa, &iosb, FILE_SHARE_READ,
        FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);
    if(!NT_SUCCESS(st))
        return HrFromStatus(st, XBDM_CANNOTACCESS);
    NtClose(h);

    /* We've found the file, so now write out the name */
    pch[-1] = TITLE_PATH_DELIMITER;
    InitializeObjectAttributes(&oa, &ostDash, OBJ_CASE_INSENSITIVE, NULL, NULL);
    st = NtCreateFile(&h, GENERIC_WRITE | DELETE | SYNCHRONIZE,
        &oa, &iosb, NULL, 0, 0, FILE_OVERWRITE_IF,
        FILE_SYNCHRONOUS_IO_NONALERT);
    if(NT_SUCCESS(st)) {
        st = NtWriteFile(h, NULL, NULL, NULL, &iosb, szLaunchData, sizeof
            szLaunchData - 1, NULL);
        if(!NT_SUCCESS(st)) {
            /* Can't write, so try to delete */
            hr = HrFromStatus(st, E_UNEXPECTED);
            RtlZeroMemory(&fdi, sizeof fdi);
            fdi.DeleteFile = TRUE;
            st = NtSetInformationFile(h, &iosb, &fdi, sizeof fdi,
                FileDispositionInformation);
            hr = NT_SUCCESS(st) ? XBDM_NOERR : E_UNEXPECTED;
        } else
            hr = XBDM_NOERR;
        NtClose(h);
    } else
        hr = E_FAIL;
    return hr;
}

HRESULT HrReportMemory(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;
    /* We print 32 bytes per line, which takes 64 characters + 2 newline chars
     * (66 chars) to constitute a line */
    DWORD cb;
    DWORD dwPageBase;
    BYTE b;
    DWORD ibLim = 32;
    DWORD ib;
    LPSTR sz = pdmcc->Buffer;
    BOOL fPageValid;

    if(!pdmcc->BytesRemaining) {
        /* No more data */
        if(pdmcc->Buffer && pdmcc->BufferSize == 1056) {
            DmFreePool(pdmcc->Buffer);
            pdmcc->Buffer = NULL;
        }
        return XBDM_ENDOFLIST;
    }

    cb = pdmcc->BufferSize / 66;
    if(cb > pdmcc->BytesRemaining)
        cb = pdmcc->BytesRemaining;
    dwPageBase = (DWORD)pccs->pb + 0x1000;
    pdmcc->BytesRemaining -= cb;

    while(cb) {
        ibLim = 32;

        if(cb < ibLim)
            ibLim = cb;
        for(ib = 0; ib < ibLim; ++ib, ++pccs->pb) {
            if(((DWORD)pccs->pb ^ dwPageBase) & 0xfffff000) {
                dwPageBase = (DWORD)pccs->pb & 0xfffff000;
                fPageValid = MmIsAddressValid(pccs->pb);
            }
            if(fPageValid)
                /* If this address has an associated breakpoint, then the
                 * memory we want isn't the int 3, but the breakpoint data */
                fPageValid = FGetMemory(pccs->pb, &b);
            if(fPageValid) {
                sz[2*ib] = rgchHex[(b >> 4) & 0xf];
                sz[2*ib+1] = rgchHex[b & 0xf];
            } else
                sz[2*ib] = sz[2*ib+1] = '?';
        }
        sz[2*ib] = '\015';
        sz[2*ib+1] = '\012';
        sz += 2*ib + 2;
        cb -= ibLim;
    }

    pdmcc->DataSize = sz - (LPSTR)pdmcc->Buffer;
    return XBDM_NOERR;
}

HRESULT HrGetMemory(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    CCS *pccs;

    if(!pdmcc)
        return E_FAIL;
    pccs = (CCS *)pdmcc->CustomData;
    if(!FGetNamedDwParam(sz, "addr", (DWORD *)&pccs->pb, szResp))
        return E_FAIL;
    if(!FGetNamedDwParam(sz, "length", &pdmcc->BytesRemaining, szResp))
        return E_FAIL;

    strcpy(szResp, "memory data follows");
    pdmcc->HandlingFunction = HrReportMemory;
    /* We'd like a ~1k buffer for sending data, but if we can't find the
     * memory, we'll just get the default buffer */
    pdmcc->BufferSize = 1056;
    pdmcc->Buffer = DmAllocatePoolWithTag(pdmcc->BufferSize, 'mdbX');
    return XBDM_MULTIRESPONSE;
}

HRESULT HrSetMemory(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    int i;
    char rgch[5];
    BYTE b;
    DWORD dwPageBase;
    BOOL fPageValid;
    int cb = 0;
    LPCSTR szData;
    BYTE *pb;

    if(!FGetNamedDwParam(sz, "addr", (DWORD *)&pb, szResp))
        return E_FAIL;
    szData = PchGetParam(sz, "data", FALSE);
    dwPageBase = (DWORD)pb + 0x1000;

    rgch[0] = '0';
    rgch[1] = 'x';
    rgch[4] = 0;

    if(szData) {
        for(;;++cb, ++pb) {
            if(FIsSpace(*szData))
                break;
            for(i = 0; i < 2; ++i) {
                if(!(*szData >= '0' && *szData <= '9' ||
                        *szData >= 'a' && *szData <= 'f' ||
                        *szData >= 'A' && *szData <= 'F')) {
                    strcpy(szResp, "400- syntax error in data");
                    return E_FAIL;
                }
                rgch[2+i] = *szData++;
            }
            b = (BYTE)DwFromSz(rgch, NULL);
            if(((DWORD)pb ^ dwPageBase) & 0xfffff000) {
                dwPageBase = (DWORD)pb & 0xfffff000;
                fPageValid = MmIsAddressValid(pb);
            }
            if(fPageValid)
                /* If there's a breakpoint at this address, we need to set
                 * the breakpoint data, not change the int 3 */
                fPageValid = FSetMemory(pb, b);
            if(!fPageValid) {
                sprintf(szResp, "set %d bytes", cb);
                return XBDM_MEMUNMAPPED;
            }
        }
    }
    sprintf(szResp, "set %d bytes", cb);
    return XBDM_NOERR;
}

HRESULT HrReportThreads(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;
    int ich;

    if(!pdmcc->BytesRemaining || !pccs->i) {
        DmFreePool(pccs->h);
        return XBDM_ENDOFLIST;
    }

    /* Pack as many thread ids into the buffer as we can */
    for(ich = 0; (DWORD)ich < pdmcc->BufferSize - 10 && pccs->i; ) {
        sprintf((LPSTR)pdmcc->Buffer + ich, "%d\015\012",
            ((LPDWORD)pccs->h)[--pccs->i]);
        while(((LPSTR)pdmcc->Buffer)[ich])
            ++ich;
    }

    pdmcc->DataSize = ich;
    return XBDM_NOERR;
}

HRESULT HrListThreads(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    CCS *pccs;
    HRESULT hr;

    if(!pdmcc)
        return E_FAIL;
    /* Make a list that we think will be big enough */
    pccs = (CCS *)pdmcc->CustomData;
    pccs->h = DmAllocatePoolWithTag(256 * sizeof(DWORD), 'mdbX');
    if(!pccs->h)
        return E_OUTOFMEMORY;
    pccs->i = 256;
    hr = DmGetThreadList((LPDWORD)pccs->h, (LPDWORD)&pccs->i);
    if(FAILED(hr)) {
        if(hr == XBDM_BUFFER_TOO_SMALL)
            hr = E_UNEXPECTED;
        DmFreePool(pccs->h);
    } else {
        hr = XBDM_MULTIRESPONSE;
        strcpy(szResp, "thread list follows");
    }
    pdmcc->HandlingFunction = HrReportThreads;
    return hr;
}

HRESULT HrHaltThread(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    DWORD tid;

    if(!FGetDwParam(sz, "thread", &tid))
        tid = 0;
    return DmHaltThread(tid);
}

HRESULT HrContinueThread(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    DWORD tid;

    if(!FGetNamedDwParam(sz, "thread", &tid, szResp))
        return E_FAIL;
    return DmContinueThread(tid, (BOOL)PchGetParam(sz, "exception", FALSE));
}

HRESULT HrSuspendThread(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    DWORD tid;

    if(!FGetNamedDwParam(sz, "thread", &tid, szResp))
        return E_FAIL;
    return DmSuspendThread(tid);
}

HRESULT HrResumeThread(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    DWORD tid;

    if(!FGetNamedDwParam(sz, "thread", &tid, szResp))
        return E_FAIL;
    return DmResumeThread(tid);
}

HRESULT HrFunctionCall(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    DWORD tid;

    if(!FGetNamedDwParam(sz, "thread", &tid, szResp))
        return E_FAIL;
    return DmSetupFunctionCall(tid);
}

typedef struct _CREG {
    char *szName;
    DWORD dwOffset;
    DWORD cb;
} CREG;

#define CtxOffset(field) ((DWORD)(&((PCONTEXT)0)->field))
#define Creg(reg) { #reg, CtxOffset(reg), sizeof ((PCONTEXT)0)->reg }
#define FpCreg(reg) { #reg, CtxOffset(FloatSave.reg), \
    sizeof ((PCONTEXT)0)->FloatSave.reg }

static CREG rgcregCtrl[] = {
    Creg(Ebp), Creg(Esp), Creg(Eip), Creg(EFlags)
};

static CREG rgcregInt[] = {
    Creg(Eax), Creg(Ebx), Creg(Ecx), Creg(Edx), Creg(Edi), Creg(Esi)
};

static CREG rgcregFPCtrl[] = {
    FpCreg(Cr0NpxState)
};

typedef struct _RCC {
    CONTEXT cr;
    union {
        struct {
            int irgcreg;
            CREG *pcreg;
            int icreg;
            CREG *pcregNext;
        };
        struct {
            DWORD tid;
            BOOL fSend;
            int iSend;
            int ib;
        };
    };
} RCC;

CREG *PcregNext(RCC *prcc)
{
    while(!prcc->icreg) {
        /* Time to switch to the next register set */
        switch(prcc->irgcreg) {
        case 0:
            if((prcc->cr.ContextFlags & CONTEXT_CONTROL) == CONTEXT_CONTROL) {
                prcc->pcreg = rgcregCtrl;
                prcc->icreg = sizeof rgcregCtrl / sizeof(CREG);
            } else
                prcc->icreg = 0;
            ++prcc->irgcreg;
            break;
        case 1:
            if((prcc->cr.ContextFlags & CONTEXT_INTEGER) == CONTEXT_INTEGER) {
                prcc->pcreg = rgcregInt;
                prcc->icreg = sizeof rgcregInt / sizeof(CREG);
            } else
                prcc->icreg = 0;
            ++prcc->irgcreg;
            break;
        case 2:
            if((prcc->cr.ContextFlags & CONTEXT_FLOATING_POINT) ==
                CONTEXT_FLOATING_POINT)
            {
                prcc->pcreg = rgcregFPCtrl;
                prcc->icreg = sizeof rgcregFPCtrl / sizeof(CREG);
            } else
                prcc->icreg = 0;
            ++prcc->irgcreg;
            break;
        default:
            /* Nothing left */
            return prcc->pcregNext = NULL;
        }
    }
    --prcc->icreg;
    return prcc->pcregNext = prcc->pcreg++;
}

#define SIZE_OF_EXT_REGISTERS 0x200

HRESULT HrDoExtContext(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    RCC *prcc = (RCC *)pdmcc->CustomData;
    HRESULT hr = XBDM_NOERR;
    BOOL fDone = FALSE;

    if(prcc->fSend) {
        switch(prcc->iSend++) {
        case 0:
            pdmcc->DataSize = sizeof(DWORD);
            memcpy(pdmcc->Buffer, &pdmcc->BytesRemaining, pdmcc->DataSize);
            break;
        case 1:
            pdmcc->Buffer = &prcc->cr.FloatSave;
            pdmcc->DataSize = pdmcc->BytesRemaining;
            break;
        default:
            pdmcc->BytesRemaining = 0;
            break;
        }
        if(!pdmcc->BytesRemaining) {
            fDone = TRUE;
            hr = XBDM_ENDOFLIST;
        }
    } else if(pdmcc->DataSize != 0) {
        DWORD cbCopy = SIZE_OF_EXT_REGISTERS - prcc->ib;
        if(cbCopy > pdmcc->DataSize)
            cbCopy = pdmcc->DataSize;
        if(cbCopy)
            memcpy((BYTE *)&prcc->cr.FloatSave + prcc->ib, pdmcc->Buffer,
                cbCopy);
        pdmcc->BytesRemaining -= pdmcc->DataSize;
        prcc->ib += cbCopy;
        if(!pdmcc->BytesRemaining) {
            fDone = TRUE;
            prcc->cr.ContextFlags = CONTEXT_EXTENDED_REGISTERS;
            hr = DmSetThreadContext(prcc->tid, &prcc->cr);
        } else
            hr = XBDM_NOERR;
    } else
        fDone = TRUE;

    if(fDone)
        DmFreePool(prcc);
    return hr;
}

HRESULT HrGetExtContext(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    RCC *prcc;
    CONTEXT cr;
    HRESULT hr;
    DWORD tid;

    if(!pdmcc)
        return E_FAIL;
    if(!FGetNamedDwParam(sz, "thread", &tid, szResp))
        return E_FAIL;
    cr.ContextFlags = CONTEXT_EXTENDED_REGISTERS;
    hr = DmGetThreadContext(tid, &cr);
    if(SUCCEEDED(hr)) {
        if((cr.ContextFlags & CONTEXT_EXTENDED_REGISTERS) ==
            CONTEXT_EXTENDED_REGISTERS)
        {
            prcc = DmAllocatePoolWithTag(sizeof *prcc, 'rcmd');
            if(!prcc)
                return E_OUTOFMEMORY;
            memcpy(&prcc->cr, &cr, sizeof cr);
            prcc->iSend = 0;
            prcc->fSend = TRUE;
            pdmcc->CustomData = prcc;
            pdmcc->HandlingFunction = HrDoExtContext;
            ASSERT(SIZE_OF_EXT_REGISTERS <= sizeof prcc->cr.FloatSave);
            pdmcc->BytesRemaining = SIZE_OF_EXT_REGISTERS;
            hr = XBDM_BINRESPONSE;
        } else
            hr = XBDM_NOTSTOPPED;
    }
    return hr;
}

HRESULT HrReportContext(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    RCC *prcc = (RCC *)pdmcc->CustomData;
    DWORD ich = 0;
    DWORD ichLim = pdmcc->BufferSize - 22;
    CREG *pcreg;

    if(!pdmcc->BytesRemaining || !(pcreg = prcc->pcregNext)) {
        DmFreePool(prcc);
        return XBDM_ENDOFLIST;
    }

    /* Fill as much of the buffer as we can */
    while(pcreg && ich < ichLim) {
        if(pcreg->cb == 4)
            sprintf((char *)pdmcc->Buffer + ich, "%s=0x%08x\015\012",
                pcreg->szName, *(DWORD *)((BYTE *)&prcc->cr + pcreg->dwOffset));
        else
            sprintf((char *)pdmcc->Buffer + ich, "%s=0x%04x\015\012",
                pcreg->szName, *(USHORT *)((BYTE *)&prcc->cr + pcreg->dwOffset));
        while(((char *)pdmcc->Buffer)[ich])
            ++ich;
        pcreg = PcregNext(prcc);
    }

    pdmcc->DataSize = ich;
    return XBDM_NOERR;
}

HRESULT HrGetContext(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    RCC *prcc;
    CONTEXT cr;
    HRESULT hr;
    DWORD tid;

    if(!pdmcc)
        return E_FAIL;
    if(!FGetNamedDwParam(sz, "thread", &tid, szResp))
        return E_FAIL;
    cr.ContextFlags = 0;
    if(PchGetParam(sz, "control", FALSE))
        cr.ContextFlags |= CONTEXT_CONTROL;
    if(PchGetParam(sz, "int", FALSE))
        cr.ContextFlags |= CONTEXT_INTEGER;
    if(PchGetParam(sz, "full", FALSE))
        cr.ContextFlags |= CONTEXT_FULL;
    if(PchGetParam(sz, "fp", FALSE))
        cr.ContextFlags |= CONTEXT_FLOATING_POINT;
    hr = DmGetThreadContext(tid, &cr);
    if(SUCCEEDED(hr)) {
        prcc = DmAllocatePoolWithTag(sizeof *prcc, 'rcmd');
        if(!prcc)
            return E_OUTOFMEMORY;
        memcpy(&prcc->cr, &cr, sizeof cr);
        prcc->icreg = 0;
        prcc->irgcreg = 0;
        PcregNext(prcc);
        pdmcc->CustomData = prcc;
        pdmcc->HandlingFunction = HrReportContext;
        hr = XBDM_MULTIRESPONSE;
    }
    return hr;
}

HRESULT HrSetContext(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    CONTEXT cr;
    HRESULT hr;
    DWORD dw;
    DWORD tid;
    RCC *prcc;
    DWORD cbExtRegs;

    if(!FGetNamedDwParam(sz, "thread", &tid, szResp))
        return E_FAIL;

    /* If we're going to set the extended context, make sure we'll be able to
     * do so */
    if(FGetDwParam(sz, "ext", &cbExtRegs)) {
        if(!pdmcc)
            return E_FAIL;
        pdmcc->BytesRemaining = cbExtRegs;
        prcc = DmAllocatePoolWithTag(sizeof *prcc, 'rcmd');
        if(!prcc)
            return E_OUTOFMEMORY;
    } else
        prcc = NULL;

    /* We need the current context and then we'll modify the appropriate
     * fields */
    cr.ContextFlags = CONTEXT_FULL | CONTEXT_FLOATING_POINT;
    hr = DmGetThreadContext(tid, &cr);
    if(SUCCEEDED(hr)) {
        int icreg;
        int irgcreg;
        CREG *pcreg;

        for(irgcreg = 0; irgcreg < 2; ++irgcreg) {
            switch(irgcreg) {
            case 0:
                pcreg = rgcregCtrl;
                icreg = sizeof rgcregCtrl / sizeof(CREG);
                break;
            case 1:
                pcreg = rgcregInt;
                icreg = sizeof rgcregInt / sizeof(CREG);
                break;
            case 2:
                pcreg = rgcregFPCtrl;
                icreg = sizeof rgcregFPCtrl / sizeof(CREG);
                break;
            }

            for(; icreg--; ++pcreg) {
                if(FGetDwParam(sz, pcreg->szName, &dw)) {
                    if(pcreg->cb == 4)
                        *(DWORD *)((BYTE *)&cr + pcreg->dwOffset) = dw;
                    else
                        *(USHORT *)((BYTE *)&cr + pcreg->dwOffset) = (USHORT)dw;
                }
            }
        }
        hr = DmSetThreadContext(tid, &cr);
    }
    /* Now see if we want to capture the extended context */
    if(prcc) {
        if(SUCCEEDED(hr)) {
            /* We're not going to be getting the cr0 state when the data
             * comes in, so we'll put it in now */
            prcc->cr.FloatSave.Cr0NpxState = cr.FloatSave.Cr0NpxState;
            pdmcc->CustomData = prcc;
            pdmcc->Buffer = NULL;
            prcc->fSend = FALSE;
            prcc->ib = 0;
            prcc->tid = tid;
            pdmcc->HandlingFunction = HrDoExtContext;
            hr = XBDM_READYFORBIN;
        } else
            DmFreePool(prcc);
    }

    return hr;
}

void FillSzFromWz(char *sz, int cch, WCHAR *wz, int cwch)
{
    if(cwch < 0) {
        while(*wz && cch > 1) {
            *sz++ = (char)*wz++;
            --cch;
        }
    } else {
        while(cwch && cch > 1) {
            *sz++ = (char)*wz++;
            --cch;
            --cwch;
        }
    }
    *sz = 0;
}

HRESULT HrReportCounters(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;
    HRESULT hr;
    DWORD cch;
    DM_COUNTINFO dmci;

    if(!pdmcc->BytesRemaining) {
        DmCloseCounters(pccs->pdmwc);
        return XBDM_ENDOFLIST;
    }

    cch = pdmcc->BufferSize;
    hr = DmWalkPerformanceCounters(&pccs->pdmwc, &dmci);
    if(SUCCEEDED(hr))
        sprintf(pdmcc->Buffer, "name=\"%s\" type=0x%08x", dmci.Name,
            dmci.Type);
    else {
        DmCloseCounters(pccs->pdmwc);
        pdmcc->BytesRemaining = 0;
    }
    return hr;
}

HRESULT HrListCounters(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    CCS *pccs;
    if(!pdmcc)
        return E_FAIL;
    pccs = (CCS *)pdmcc->CustomData;
    pccs->pdmwc = NULL;
    pdmcc->HandlingFunction = HrReportCounters;
    return XBDM_MULTIRESPONSE;
}

HRESULT HrReportCounterData(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;

    switch(pdmcc->BytesRemaining++) {
    case 1:
        sprintf(pdmcc->Buffer, "type=0x%08x vallo=0x%08x "
            "valhi=0x%08x ratelo=0x%08x ratehi=0x%08x", pccs->dmcd.CountType,
            pccs->dmcd.CountValue.LowPart, pccs->dmcd.CountValue.HighPart,
            pccs->dmcd.RateValue.LowPart, pccs->dmcd.RateValue.HighPart);
        break;
    default:
        return XBDM_ENDOFLIST;
    }
    return XBDM_NOERR;
}

HRESULT HrQueryPerformanceCounter(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    CCS *pccs;
    HRESULT hr;
    char szName[256];
    HANDLE h;
    DWORD dwType;

    if(!pdmcc)
        return E_FAIL;

    if(!FGetSzParam(sz, "name", szName, sizeof szName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }

    if(!FGetNamedDwParam(sz, "type", &dwType, szResp))
        return E_FAIL;

    hr = DmOpenPerformanceCounter(szName, &h);
    if(SUCCEEDED(hr)) {
        pccs = (CCS *)pdmcc->CustomData;
        hr = DmQueryPerformanceCounterHandle(h, dwType, &pccs->dmcd);
        NtClose(h);
    }
    if(SUCCEEDED(hr)) {
        pdmcc->HandlingFunction = HrReportCounterData;
        pdmcc->BytesRemaining = 1;
        hr = XBDM_MULTIRESPONSE;
        strcpy(szResp, "counter data follows");
    }
    return hr;
}

HRESULT HrReportModules(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;
    HRESULT hr;
    DMN_MODLOAD dmml;

    if(!pdmcc->BytesRemaining) {
        DmCloseLoadedModules(pccs->pdmwm);
        return XBDM_ENDOFLIST;
    }

    hr = DmWalkLoadedModules(&pccs->pdmwm, &dmml);
    if(SUCCEEDED(hr))
        GetModLoadSz(pdmcc->Buffer, &dmml);
    else {
        DmCloseLoadedModules(pccs->pdmwm);
        pdmcc->BytesRemaining = 0;
    }
    return hr;
}

HRESULT HrListModules(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    CCS *pccs;
    if(!pdmcc)
        return E_FAIL;
    pccs = (CCS *)pdmcc->CustomData;
    pccs->pdmwm = NULL;
    pdmcc->HandlingFunction = HrReportModules;
    return XBDM_MULTIRESPONSE;
}

HRESULT HrReportModuleSections(PDM_CMDCONT pdmcc, LPSTR szResponse,
    DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;
    DMN_SECTIONLOAD dmsl;
    HRESULT hr;

    if(!pdmcc->BytesRemaining) {
        DmCloseModuleSections(pccs->pdmws);
        return XBDM_ENDOFLIST;
    }

    hr = DmWalkModuleSections(&pccs->pdmws, NULL, &dmsl);
    if(SUCCEEDED(hr))
        GetSectLoadSz(pdmcc->Buffer, &dmsl);
    else {
        DmCloseModuleSections(pccs->pdmws);
        pdmcc->BytesRemaining = 0;
    }
    return hr;
}

HRESULT HrListModuleSections(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    CCS *pccs;
    char szName[MAX_OBJ_PATH+1];
    HRESULT hr;
    DMN_SECTIONLOAD dmsl;

    if(!pdmcc)
        return E_FAIL;
    if(!FGetSzParam(sz, "name", szName, sizeof szName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }
    pccs = (CCS *)pdmcc->CustomData;
    pccs->pdmws = NULL;
    hr = DmWalkModuleSections(&pccs->pdmws, szName, &dmsl);
    if(SUCCEEDED(hr) || hr == XBDM_ENDOFLIST) {
        pdmcc->HandlingFunction = HrReportModuleSections;
        RewindDmws(pccs->pdmws);
        hr = XBDM_MULTIRESPONSE;
    }
    return hr;
}

HRESULT HrDoBreak(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    PVOID pvAddr;
    HRESULT hr;
    BOOL fClear;

#if 0
    if(PchGetParam(sz, "list", FALSE)) {
        DoListBreakpoints(isock);
        return;
    }
#endif
    if(PchGetParam(sz, "now", FALSE)) {
        Sleep(250);
        _asm int 3
        return XBDM_NOERR;
    }
    if(PchGetParam(sz, "start", FALSE)) {
        hr = DmSetInitialBreakpoint();
        if(FAILED(hr))
            strcpy(szResp, "execution not pending");
        return hr;
    }
    if(PchGetParam(sz, "clearall", FALSE)) {
        RemoveAllBreakpoints();
        return XBDM_NOERR;
    }
    fClear = PchGetParam(sz, "clear", FALSE) != 0;
    if(FGetNamedDwParam(sz, "addr", (DWORD *)&pvAddr, szResp)) {
        if(fClear)
            /* We're only removing this breakpoint */
            hr = DmRemoveBreakpoint(pvAddr);
        else
            hr = DmSetBreakpoint(pvAddr);
    } else {
        /* See if we're doing a hw breakpoint */
        DWORD dwType = DMBREAK_NONE;
        DWORD dwSize = 0;

        if(FGetDwParam(sz, "read", (DWORD *)&pvAddr))
            dwType = DMBREAK_READWRITE;
        else if(FGetDwParam(sz, "write", (DWORD *)&pvAddr))
            dwType = DMBREAK_WRITE;
        else if(FGetDwParam(sz, "execute", (DWORD *)&pvAddr))
            dwType = DMBREAK_EXECUTE;
        if(dwType == DMBREAK_NONE)
            /* Never saw a valid command */
            hr = E_FAIL;
        else if(fClear || FGetNamedDwParam(sz, "size", &dwSize, szResp)) {
            szResp[0] = 0;
            hr = DmSetDataBreakpoint(pvAddr, fClear ? DMBREAK_NONE : dwType,
                dwSize);
        } else
            hr = E_FAIL;
    }
    return hr;
}

HRESULT HrIsBreak(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    PVOID pvAddr;
    DWORD dwType;
    HRESULT hr;

    if(!FGetNamedDwParam(sz, "addr", (DWORD *)&pvAddr, szResp))
        return E_FAIL;

    hr = DmIsBreakpoint(pvAddr, &dwType);
    if(SUCCEEDED(hr))
        sprintf(szResp, "type=%d", dwType);
    return hr;
}

HRESULT HrSetConfig(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    ULONG ulIndex;
    DWORD dwValue;

    if(!FGetNamedDwParam(sz, "index", &ulIndex, szResp))
        return E_FAIL;

    if(!FGetNamedDwParam(sz, "value", &dwValue, szResp))
        return E_FAIL;

    if (!NT_SUCCESS(ExSaveNonVolatileSetting(ulIndex, REG_DWORD, &dwValue, sizeof(DWORD))))
    {
        strcpy(szResp, "setvalue failed");
        return E_FAIL;
    }

    return XBDM_NOERR;
}

#if 0
void DoListBreakpoints(int isock)
{
    SOCKET s = rgsockConnect[isock];
    BYTE *pb = (BYTE *)-1;
    char sz[64];

    PrintSockLine(s, "202- breakpoint list follows");
    while(FGetNextBreakpoint(&pb)) {
        sprintf(sz, "0x%08x", pb);
        PrintSockLine(s, sz);
    }
    PrintSockLine(s, ".");
}
#endif

HRESULT HrIsStopped(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    HRESULT hr;
    DM_THREADSTOP dmts;
    DWORD tid;

    if(!FGetNamedDwParam(sz, "thread", &tid, szResp))
        return E_FAIL;
    hr = DmIsThreadStopped(tid, &dmts);
    if(SUCCEEDED(hr)) {
        switch(dmts.NotifiedReason) {
        case DM_DEBUGSTR:
            sprintf(szResp, "debugstr thread=%d stop",
                dmts.u.DebugStr.ThreadId);
            break;
        case DM_ASSERT:
            sprintf(szResp, "assert prompt thread=%d stop",
                dmts.u.DebugStr.ThreadId);
            break;
        default:
            if(!FGetNotifySz(dmts.NotifiedReason, (DWORD)&dmts.u, szResp))
                strcpy(szResp, "stopped");
            break;
        }
    }
    return hr;
}

HRESULT HrReportThreadInfo(PDM_CMDCONT pdmcc, LPSTR szResponse,
    DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;

    switch(pdmcc->BytesRemaining++) {
    case 1:
        sprintf(pdmcc->Buffer, "suspend=%d priority=%d tlsbase=0x%08x",
            pccs->dmti.SuspendCount, pccs->dmti.Priority, pccs->dmti.TlsBase);
        break;
    default:
        return XBDM_ENDOFLIST;
    }
    return XBDM_NOERR;
}

HRESULT HrThreadInfo(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    HRESULT hr;
    DWORD tid;
    CCS *pccs;

    if(!pdmcc)
        return E_FAIL;
    if(!FGetNamedDwParam(sz, "thread", &tid, szResp))
        return E_FAIL;
    pccs = (CCS *)pdmcc->CustomData;
    hr = DmGetThreadInfo(tid, &pccs->dmti);
    if(SUCCEEDED(hr)) {
        pdmcc->HandlingFunction = HrReportThreadInfo;
        pdmcc->BytesRemaining = 1;
        hr = XBDM_MULTIRESPONSE;
        strcpy(szResp, "thread info follows");
    }
    return hr;
}

HRESULT HrReportXapiInfo(PDM_CMDCONT pdmcc, LPSTR szResponse,
    DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;

    switch(pdmcc->BytesRemaining++) {
    case 1:
        sprintf(pdmcc->Buffer, "lasterr=0x%08x", pccs->dmxd.LastErrorOffset);
        break;
    default:
        return XBDM_ENDOFLIST;
    }
    return XBDM_NOERR;
}

HRESULT HrXapiInfo(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    HRESULT hr;
    DWORD tid;
    CCS *pccs;

    if(!pdmcc)
        return E_FAIL;
    pccs = (CCS *)pdmcc->CustomData;
    hr = DmGetXtlData(&pccs->dmxd);
    if(SUCCEEDED(hr)) {
        pdmcc->HandlingFunction = HrReportXapiInfo;
        pdmcc->BytesRemaining = 1;
        hr = XBDM_MULTIRESPONSE;
        strcpy(szResp, "xtl info follows");
    }
    return hr;
}

HRESULT HrDoLongName(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    HRESULT hr;
    char szModule[MAX_OBJ_PATH+1];

    if(!FGetSzParam(sz, "name", szModule, sizeof szModule)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }

    hr = DmGetModuleLongName(szModule, szResp, &cchResp);
    if(hr == E_FAIL)
        strcpy(szResp, "no long name available");
    return hr;
}

HRESULT HrReportXbeInfo(PDM_CMDCONT pdmcc, LPSTR szResponse,
    DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;

    switch(pccs->iiXbe++) {
    case 0:
        sprintf(pdmcc->Buffer, "timestamp=0x%08x", pccs->pxbe->TimeStamp);
        break;
    case 1:
        sprintf(pdmcc->Buffer, "checksum=0x%08x", pccs->pxbe->CheckSum);
        break;
    case 2:
        /* We're a little sneaky with the name -- we know the buffer we have
         * for the name is big enough, so we slide it over, overwriting the
         * timestamp and checksum values, and insert the "name=" in front */
        memmove(&pccs->pxbe->LaunchPath[5], pccs->pxbe->LaunchPath,
            MAX_PATH + 1);
        memcpy(pccs->pxbe->LaunchPath, "name=", 5);
        pdmcc->Buffer = pccs->pxbe->LaunchPath;
        break;
    default:
        pdmcc->BytesRemaining = 0;
        break;
    }

    if(!pdmcc->BytesRemaining) {
        DmFreePool(pccs->pxbe);
        return XBDM_ENDOFLIST;
    }
    return XBDM_NOERR;
}

HRESULT HrXbeInfo(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    char szName[MAX_OBJ_PATH+1];
    char *pszName;
    CCS *pccs;
    HRESULT hr;

    if(!pdmcc)
        return E_FAIL;
    if(PchGetParam(sz, "running", FALSE))
        pszName = NULL;
    else if(!FGetSzParam(sz, "name", szName, sizeof szName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    } else
        pszName = szName;
    pccs = (CCS *)pdmcc->CustomData;
    pccs->pxbe = DmAllocatePoolWithTag(sizeof(DM_XBE), 'dmbX');
    if(!pccs->pxbe)
        return E_OUTOFMEMORY;
    hr = DmGetXbeInfo(pszName, pccs->pxbe);
    if(FAILED(hr))
        DmFreePool(pccs->pxbe);
    else {
        pdmcc->HandlingFunction = HrReportXbeInfo;
        pdmcc->BytesRemaining = 1;
        pccs->iiXbe = 0;
        hr = XBDM_MULTIRESPONSE;
    }
    return hr;
}

BOOL DmIsDebuggerPresent(void)
{
    return g_fDebugging;
}

HRESULT HrConnectDebugger(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    if(g_dmi.Flags & DMIFLAG_RUNSHELL)
        return XBDM_NOTDEBUGGABLE;
    if(PchGetParam(sz, "connect", FALSE)) {
        g_fDebugging = TRUE;
        fAllowKd = FALSE;
    } else if(PchGetParam(sz, "disconnect", FALSE))
        g_fDebugging = FALSE;
    else {
        sprintf(szResp, "no command");
        return E_FAIL;
    }
    return XBDM_NOERR;
}

HRESULT HrSystemTime(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    LARGE_INTEGER li;

    KeQuerySystemTime(&li);
    if(li.HighPart > 0x20000000)
        return XBDM_CLOCKNOTSET;
    sprintf(szResp, "high=0x%x low=0x%x", li.HighPart, li.LowPart);
    return XBDM_NOERR;
}

HRESULT HrGetAltAddr(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    XNADDR xna;
    int err;

    err = XNetGetTitleXnAddr(&xna);
    if(err == XNET_GET_XNADDR_NONE || err == XNET_GET_XNADDR_PENDING)
        return E_FAIL;
    sprintf(szResp, "addr=0x%08x", ntohl(xna.ina.s_addr));
    return XBDM_NOERR;
}

HRESULT HrSetSystemTime(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    LARGE_INTEGER li;
    NTSTATUS st;
    DWORD tzflag;

    if(!FGetDwParam(sz, "clockhi", &li.HighPart) || !FGetDwParam(sz, "clocklo", &li.LowPart))
        return XBDM_CLOCKNOTSET;
    st = NtSetSystemTime(&li, NULL);

    // Set default timezone and language if necessary
    if (FGetDwParam(sz, "tz", &tzflag)) {
        XBOX_USER_SETTINGS userSettings;
        ULONG type, length;
        st = ExQueryNonVolatileSetting(
                XC_MAX_OS,
                &type,
                &userSettings,
                sizeof(userSettings),
                &length);

        if (NT_SUCCESS(st)) {
            BOOL isInvalid = FALSE;
            // Default to Pacific time zone if not set
            if (XBOX_INVALID_TIMEZONE_SETTING(userSettings)) {
                userSettings.TimeZoneBias = 480;
                strncpy(userSettings.TimeZoneStdName, "PST", XC_TZNAMELEN);
                strncpy(userSettings.TimeZoneDltName, "PDT", XC_TZNAMELEN);
                userSettings.TimeZoneStdBias = 0;
                userSettings.TimeZoneDltBias = -60;
                userSettings.TimeZoneStdDate.Month = 10;
                userSettings.TimeZoneStdDate.Day = 5;
                userSettings.TimeZoneStdDate.DayOfWeek = 0;
                userSettings.TimeZoneStdDate.Hour = 0;
                userSettings.TimeZoneDltDate.Month = 4;
                userSettings.TimeZoneDltDate.Day = 1;
                userSettings.TimeZoneDltDate.DayOfWeek = 0;
                userSettings.TimeZoneDltDate.Hour = 0;
                isInvalid = TRUE;
            }

            // Default language to English if not set
            if (userSettings.Language == XC_LANGUAGE_UNKNOWN) {
                userSettings.Language = XC_LANGUAGE_ENGLISH;
                isInvalid = TRUE;
            }

            if (isInvalid) {
                st = ExSaveNonVolatileSetting(
                        XC_MAX_OS,
                        REG_BINARY,
                        &userSettings,
                        sizeof(userSettings));
            }
        }
    }

    return NT_SUCCESS(st) ? XBDM_NOERR : XBDM_CLOCKNOTSET;
}

HRESULT HrReboot(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    int icst;
    DWORD dw = 0;

    if(!pdmcc)
        return FALSE;
    icst = (PCST)pdmcc - rgcst;
    PrintSockLine(rgcst[icst].s, "200- OK");

    if(PchGetParam(sz, "stop", FALSE))
        dw |= DMBOOT_STOP;
    if(PchGetParam(sz, "wait", FALSE))
        dw |= DMBOOT_WAIT;
    if(PchGetParam(sz, "warm", FALSE))
        dw |= DMBOOT_WARM;
    if(PchGetParam(sz, "nodebug", FALSE))
        dw |= DMBOOT_NODEBUG;
    DmReboot(dw);
    return 0;
}

HRESULT HrMagicReboot(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    int icst;
    HANDLE h;
    OBJECT_ATTRIBUTES oa;
    OBJECT_STRING objectName;
    IO_STATUS_BLOCK iosb;
    NTSTATUS st;
    OCHAR oszTitle[MAX_OBJ_PATH+1];
    OCHAR *pochTitle;
    OCHAR *poch;
    char szTitle[MAX_OBJ_PATH+1];
    DWORD dwFlags = DMBOOT_NODEBUG | DMBOOT_WARM;

    if(!FGetSzParam(sz, "title", szTitle, sizeof szTitle)) {
        strcpy(szResp, "missing title");
        return E_FAIL;
    }

    if(PchGetParam(sz, "debug", FALSE))
        dwFlags &= ~DMBOOT_NODEBUG;

    if(!FFileNameToObName(szTitle, oszTitle, sizeof oszTitle / sizeof(OCHAR)))
        return XBDM_NOSUCHFILE;

    for(poch = oszTitle; *poch; ++poch)
        if(*poch == '\\')
            pochTitle = poch;

    DmStop();
    /* Verify the file exists */
    RtlInitObjectString(&objectName, oszTitle);
    InitializeObjectAttributes(&oa, &objectName, OBJ_CASE_INSENSITIVE, NULL,
        NULL);
    st = NtOpenFile(&h, SYNCHRONIZE, &oa, &iosb, FILE_SHARE_READ,
        FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);
    if(NT_SUCCESS(st)) {
        NtClose(h);
        *pochTitle++ = 0;
        st = NT_SUCCESS(XWriteTitleInfoNoReboot(pochTitle, oszTitle, LDT_NONE,
            0, NULL));
        if(NT_SUCCESS(st)) {
            icst = (PCST)pdmcc - rgcst;
            PrintSockLine(rgcst[icst].s, "200- OK");
            DmReboot(dwFlags);
        }
    }
    return HrFromStatus(st, E_FAIL);
}

#include "xprofp.h"
#define XBOX_CAP_FILENAME "E:\\xboxcap.dat"

XProfpFileHeader* CAPGetFileHeader()
{
    XProfpFileHeader* fileheader = (XProfpFileHeader*)
        MmAllocateSystemMemory(XPROF_FILE_HEADER_SIZE, PAGE_READWRITE);

    if (fileheader) {
        PLIST_ENTRY list, head;
        ULONG count;

        memset(fileheader, 0, XPROF_FILE_HEADER_SIZE);
        fileheader->version = XPROF_FILE_VERSION;

        // save information about currently loaded modules
        head = g_dmi.LoadedModuleList;
        list = head->Flink;
        for (count=0; list != head; list=list->Flink) {
            PLDR_DATA_TABLE_ENTRY entry;
            WCHAR* wstr;
            CHAR* str;
            ULONG len;

            entry = CONTAINING_RECORD(list, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
            fileheader->modules[count].loadaddr = (ULONG) entry->DllBase;
            fileheader->modules[count].size = entry->SizeOfImage;

            len = entry->BaseDllName.Length / sizeof(WCHAR);
            if (len >= XPROF_MAX_MODULE_NAMELEN-1) {
                // The base DLL name is too long
                // Just ignore this module.
                continue;
            }

            // Truncate unicode name to ASCII
            wstr = entry->BaseDllName.Buffer;
            str = fileheader->modules[count].name;
            while (len--)
                *str++ = (CHAR) *wstr++;

            // Null-terminator
            *str = 0;
            count++;
        }

        fileheader->module_count = count;
    }

    return fileheader;
}

NTSTATUS CAPWriteFile(XProfpGlobals* xprofData)
{
    BYTE* bufstart = (BYTE*) xprofData->bufstart;
    BYTE* bufend = (BYTE*) xprofData->bufnext_stopped;
    HANDLE file = NULL;
    XProfpFileHeader* fileheader = NULL;
    ULONG count;
    NTSTATUS status;
    IO_STATUS_BLOCK iostatusBlock;
    FILE_END_OF_FILE_INFORMATION endOfFile;
    FILE_ALLOCATION_INFORMATION allocation;

    // Sanity check
    if (!bufstart || !bufend || bufend <= bufstart) {
        return STATUS_UNSUCCESSFUL;
    }

    // Create the file for writing
    //  we're doing sequential writes only and
    //  we don't need intermediate buffering

    count = bufend - bufstart;
    ASSERT(((ULONG_PTR) bufstart % PAGE_SIZE) == 0);

    status = FCreateFile(
                &file,
                GENERIC_WRITE | SYNCHRONIZE,
                XBOX_CAP_FILENAME,
                NULL,
                0,
                0,
                FILE_OVERWRITE_IF,
                FILE_SYNCHRONOUS_IO_NONALERT |
                    FILE_SEQUENTIAL_ONLY |
                    FILE_NO_INTERMEDIATE_BUFFERING);

    if (!NT_SUCCESS(status)) goto exit;

    // Generate the profile data file header
    fileheader = CAPGetFileHeader();
    if (!fileheader) {
        status = STATUS_NO_MEMORY;
        goto exit;
    }

    ASSERT(XPROF_FILE_HEADER_SIZE % PAGE_SIZE == 0);
    status = NtWriteFile(
                file,
                NULL,
                NULL,
                NULL,
                &iostatusBlock,
                fileheader,
                XPROF_FILE_HEADER_SIZE,
                NULL);

    if (!NT_SUCCESS(status)) goto exit;

    // Write the profile data out to disk
    //  size is rounded up to a multiple of 512
    status = NtWriteFile(
                file,
                NULL,
                NULL,
                NULL,
                &iostatusBlock,
                bufstart,
                (count + 511) & ~511,
                NULL);

    if (!NT_SUCCESS(status)) goto exit;

    // Set file size
    count += XPROF_FILE_HEADER_SIZE;
    endOfFile.EndOfFile.QuadPart = count;
    status = NtSetInformationFile(
                file,
                &iostatusBlock,
                &endOfFile,
                sizeof(endOfFile),
                FileEndOfFileInformation);

    if (!NT_SUCCESS(status)) goto exit;

    allocation.AllocationSize.QuadPart = count;
    status = NtSetInformationFile(
                file,
                &iostatusBlock,
                &allocation,
                sizeof(allocation),
                FileAllocationInformation);

exit:
    if (file) { NtClose(file); }
    if (fileheader) { MmFreeSystemMemory(fileheader, XPROF_FILE_HEADER_SIZE); }
    return status;
}

HRESULT HrCAPControl(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    NTSTATUS status = STATUS_NOT_IMPLEMENTED;

    if (PchGetParam(sz, "start", FALSE)) {
        // Start profile data collection
        status = DmCapControl(XPROF_START, 0);
    } else {
        // Stop profile data collection
        status = DmCapControl(XPROF_STOP, 0);

        // Save the profile data to disk
        if (NT_SUCCESS(status)) {
            XProfpGlobals* xprofData = DmProfileData;
            LONG lock;

            // Prevent other threads from freeing the profile data buffer
            // while we're trying to write it to disk
            lock = InterlockedCompareExchange(&xprofData->lock, 1, 0);

            if (lock != 0) {
                status = STATUS_DEVICE_BUSY;
            } else {
                CAPWriteFile(xprofData);

                // Unlock the profile data buffer
                xprofData->lock = 0;
            }
        }
    }

    if (NT_SUCCESS(status)) return XBDM_NOERR;

    // Note: should we map error status to HRESULT?
    sprintf(szResp, "error status 0x%08x", status);
    return XBDM_UNDEFINED;
}

HRESULT HrIrtSweep(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    NTSTATUS status = STATUS_NOT_IMPLEMENTED;
    HANDLE file;

    #ifdef _XBOX_ENABLE_PROFILING
    #define IRTSWEEP_FILENAME "E:\\irtsweep.dat"

    status = FCreateFile(
                &file,
                GENERIC_WRITE | SYNCHRONIZE,
                IRTSWEEP_FILENAME,
                NULL,
                0,
                0,
                FILE_OVERWRITE_IF,
                FILE_SYNCHRONOUS_IO_NONALERT |
                    FILE_SEQUENTIAL_ONLY |
                    FILE_NO_INTERMEDIATE_BUFFERING);

    if (NT_SUCCESS(status)) {
        status = IrtSweep(file);
        NtClose(file);
    }

    #endif

    if (NT_SUCCESS(status)) return XBDM_NOERR;
    sprintf(szResp, "error status 0x%08x", status);
    return XBDM_UNDEFINED;
}

HRESULT HrReceiveKernelImage(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    HRESULT hr = XBDM_NOERR;
    DWORD crc;
    CCS *pccs;

    pccs = pdmcc->CustomData;

    if (pdmcc->DataSize) {
        if ( pccs->KernelImage.BytesReceived + pdmcc->DataSize <= \
             pccs->KernelImage.ImageSize) {
            CopyMemory(pccs->KernelImage.ImageBuffer + pccs->KernelImage.BytesReceived, \
                pdmcc->Buffer, pdmcc->DataSize);
        }
        pdmcc->BytesRemaining -= pdmcc->DataSize;
        pccs->KernelImage.BytesReceived += pdmcc->DataSize;
    } else if (pdmcc->BytesRemaining) {
        DmFreePool(pccs->KernelImage.ImageBuffer);
        DmFreePool(pdmcc->Buffer);
        return E_UNEXPECTED;
    }

    if (pdmcc->BytesRemaining == 0) {
        crc = Crc32(0xFFFFFFFF, pccs->KernelImage.ImageBuffer, pccs->KernelImage.ImageSize);
        if (pccs->KernelImage.Crc != crc) {
            _snprintf(szResponse, cchResponse, "bad data detected in kernel image");
            hr = E_UNEXPECTED;
        } else {
            _snprintf(szResponse, cchResponse, "flashing...");
            Sleep(250);
            hr = FlashKernelImage(pccs->KernelImage.ImageBuffer,
                    pccs->KernelImage.ImageSize, szResponse, cchResponse,
                    pccs->KernelImage.IgnoreVersionChecking);
        }
        DmFreePool(pccs->KernelImage.ImageBuffer);
        DmFreePool(pdmcc->Buffer);
    }

    return hr;
}

HRESULT HrFlashKernelImage(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    CCS *pccs;
    SIZE_T ImageSize;
    BOOL IgnoreVersionChecking = FALSE;

    if (!pdmcc) {
        return E_FAIL;
    }

    pccs = (CCS *)pdmcc->CustomData;

    if (!FGetNamedDwParam(sz, "length", &ImageSize, szResp) || \
        !FGetNamedDwParam(sz, "crc", &pccs->KernelImage.Crc, szResp)) {
        _snprintf(szResp, cchResp, "Invalid parameter(s)");
        return E_FAIL;
    }

    FGetNamedDwParam(sz, "ignoreversionchecking", &IgnoreVersionChecking, szResp);
    pccs->KernelImage.IgnoreVersionChecking = IgnoreVersionChecking;

    pccs->KernelImage.ImageSize = ImageSize;
    pdmcc->BytesRemaining = ImageSize;

    if (ImageSize < 0x80000 || (ImageSize & (ImageSize - 1)) != 0) {
        _snprintf(szResp, cchResp, "Invalid kernel image size");
        return E_FAIL;
    }

    pccs->KernelImage.ImageBuffer = DmAllocatePoolWithTag(ImageSize, 'mdbX');

    if (!pccs->KernelImage.ImageBuffer) {
        _snprintf(szResp, cchResp, "Not enough memory");
        return HrFromStatus(STATUS_NO_MEMORY, XBDM_UNDEFINED);
    }

    pdmcc->Buffer = DmAllocatePoolWithTag(0x2000, 'mdbX');

    if (!pdmcc->Buffer) {
        DmFreePool(pccs->KernelImage.ImageBuffer);
        _snprintf(szResp, cchResp, "Not enough memory");
        return HrFromStatus(STATUS_NO_MEMORY, XBDM_UNDEFINED);
    }

    pdmcc->BufferSize = 0x2000;
    pdmcc->HandlingFunction = HrReceiveKernelImage;
    pccs->KernelImage.BytesReceived = 0;

    return XBDM_READYFORBIN;
}

typedef struct _DRIVENAME_CONVERSION_ENTRY
{
    PCOSTR  ObjectName;
    ULONG   ObjectNameLength; //Without a terminating NULL
    OCHAR       DriveName;
    BOOL    Visible;
} DRIVENAME_TABLE_ENTRY, *PDRIVENAME_TABLE_ENTRY;
extern DRIVENAME_TABLE_ENTRY DriveNameConversionTable[];

HRESULT HrGetDriveList(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    char szName[8];
    OBJECT_STRING objectName;
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK iosb;
    NTSTATUS status;
    HANDLE h;
    DRIVENAME_TABLE_ENTRY *pdte;
    struct {
        OBJECT_DIRECTORY_INFORMATION odi;
        OCHAR sz[256];
    } odi;

    RtlInitObjectString(&objectName, "");
    InitializeObjectAttributes(&objectAttributes, &objectName,
        OBJ_CASE_INSENSITIVE, ObDosDevicesDirectory(), NULL);

    status = NtOpenDirectoryObject(&h, &objectAttributes);
    if(NT_SUCCESS(status)) {
        int i = 0;

        // Query the directory object to get the list of actual drives
        for(;;) {
            status = NtQueryDirectoryObject(h, &odi, sizeof odi, (i == 0), &i,
                NULL);
            if (!NT_SUCCESS(status))
                break;

            /* Reject this drive if it's not one character plus a colon */
            if(odi.odi.Name.Length != 2 || odi.odi.Name.Buffer[1] != ':')
                continue;

            /* Compare each drive in the drivename conversion table to the
             * current enumerated drive.  If there is a match, then that drive
             * has been remapped - let the table handling code below deal with
             * it */
            for(pdte = DriveNameConversionTable; pdte->ObjectName; ++pdte) {
                BYTE bMask;
                if(pdte->DriveName < 'A' || (pdte->DriveName & 0xDF) > 'Z')
                    bMask = 0xFF;
                else
                    bMask = 0xDF;
                /* Compare case-insensitive */
                if (!((odi.odi.Name.Buffer[0] ^ pdte->DriveName) & bMask))
                {
                    /* Found a matching entry - ignore the entry; it'll get
                     * handled in the table handling code below */
                    break;
                }
            }

            if (!pdte->ObjectName)
            {
                HANDLE hT;
                /* The current enumerated drive does not exist in the table.
                 * We'll need to handle it here.  Is the drive mounted? */
                odi.odi.Name.Buffer[2] = '\\';
                odi.odi.Name.Length = 3;
                InitializeObjectAttributes(&objectAttributes, &odi.odi.Name,
                    OBJ_CASE_INSENSITIVE, ObDosDevicesDirectory(), NULL);
                status = NtOpenFile(&hT, SYNCHRONIZE,&objectAttributes, &iosb,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);
                if (NT_SUCCESS(status))
                {
                    *szResp++ = odi.odi.Name.Buffer[0];
                    NtClose(hT);
                }
            }
        }
        NtClose(h);
    }

    // Enumerate through the conversion table
    szName[1] = ':';
    szName[2] = '\\';
    szName[3] = 0;
    for(pdte = DriveNameConversionTable; pdte->ObjectName; ++pdte) {
        if (pdte->Visible)
        {
            // Is the drive mounted?
            szName[0] = pdte->DriveName;
            status = FCreateFile(&h, SYNCHRONIZE, szName, NULL, 0,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                FILE_OPEN, FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);
            if (NT_SUCCESS(status))
            {
                *szResp++ = pdte->DriveName;
                NtClose(h);
            }
        }
    }

    // Close off the 'list' string value
    *szResp = 0;

    // Mark that we've been through here once so that next time through we know we're done
    return XBDM_NOERR;
}

HRESULT HrReportDriveFreeSpace(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;
    FILE_FS_SIZE_INFORMATION NormalSizeInfo;
    CCS *pccs = (CCS *)pdmcc->CustomData;

    ULARGE_INTEGER FreeBytesAvailableToCaller;
    ULARGE_INTEGER TotalNumberOfBytes;
    ULARGE_INTEGER BytesPerAllocationUnit;

    if (pdmcc->BytesRemaining-- == 0)
    {
        // Already sent the data, so close the file, free up our buffer, and return "we're done!"
        NtClose(pccs->h);
        DmFreePool(pdmcc->Buffer);
        pdmcc->Buffer = NULL;
        return XBDM_ENDOFLIST;
    }

    // Determine the size parameters of the volume.
    Status = NtQueryVolumeInformationFile(
                pccs->h,
                &IoStatusBlock,
                &NormalSizeInfo,
                sizeof(NormalSizeInfo),
                FileFsSizeInformation
                );

    if ( !NT_SUCCESS(Status) ) {
        sprintf(pdmcc->Buffer, "unknown error (%d)", Status);
        return E_FAIL;
    }

    BytesPerAllocationUnit.QuadPart     = NormalSizeInfo.BytesPerSector   * NormalSizeInfo.SectorsPerAllocationUnit;
    FreeBytesAvailableToCaller.QuadPart = BytesPerAllocationUnit.QuadPart * NormalSizeInfo.AvailableAllocationUnits.QuadPart;
    TotalNumberOfBytes.QuadPart         = BytesPerAllocationUnit.QuadPart * NormalSizeInfo.TotalAllocationUnits.QuadPart;

    // Add the results to the result string
    sprintf(pdmcc->Buffer, "freetocallerlo=0x%08x freetocallerhi=0x%08x totalbyteslo=0x%08x totalbyteshi=0x%08x totalfreebyteslo=0x%08x totalfreebyteshi=0x%08x",
            FreeBytesAvailableToCaller.LowPart, FreeBytesAvailableToCaller.HighPart,
            TotalNumberOfBytes.LowPart, TotalNumberOfBytes.HighPart,
            FreeBytesAvailableToCaller.LowPart, FreeBytesAvailableToCaller.HighPart);

    return XBDM_NOERR;
}

HRESULT HrGetDriveFreeSpace(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    NTSTATUS st;
    char szName[MAX_OBJ_PATH+1];
    CCS *pccs = (CCS *)pdmcc->CustomData;

    if(!pdmcc)
        return E_FAIL;

    if(!FGetSzParam(sz, "name", szName, sizeof szName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }

    pdmcc->BufferSize = 512;
    pdmcc->Buffer = DmAllocatePool(pdmcc->BufferSize);
    pdmcc->HandlingFunction = HrReportDriveFreeSpace;
    pdmcc->BytesRemaining = 1;  // Internal counter
    st = FCreateFile(&pccs->h, FILE_LIST_DIRECTORY | SYNCHRONIZE, szName, NULL, 0,
                FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN,
                FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);

    return NT_SUCCESS(st) ? XBDM_MULTIRESPONSE : HrFromStatus(st,
        XBDM_CANNOTACCESS);
}

HRESULT HrSendFrameData(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;

    // end of data?
    if (pdmcc->BytesRemaining <= 0) {
        DmFreePool(pdmcc->Buffer);
        pdmcc->Buffer = NULL;

        // We've finished sending the data, so start the threads going again
        DmGo();

        return XBDM_ENDOFLIST;
    }

    if(pccs->Screenshot.fFirstTime == FALSE) {
        /* First we need to send the various parameters (pitch, format, buffer size, etc) */
        sprintf(pdmcc->Buffer, "pitch=0x%08x width=0x%08x height=0x%08x format=0x%08x, framebuffersize=0x%08x",
                            pccs->Screenshot.Pitch, pccs->Screenshot.Width, pccs->Screenshot.Height, pccs->Screenshot.Format, pdmcc->BytesRemaining);
        pdmcc->DataSize = -1;
        pccs->Screenshot.fFirstTime = TRUE;
    } else {
        // if (buffersize%8k != 0) need to send less data
        DWORD cBytesToSend = min (pdmcc->BytesRemaining, pdmcc->BufferSize);
        // send 8k of data
        memcpy(pdmcc->Buffer, pccs->Screenshot.pbyFrameBuffer, cBytesToSend);
        pdmcc->DataSize = cBytesToSend;
        pdmcc->BytesRemaining -= cBytesToSend;

        // point at next 8k of data
        pccs->Screenshot.pbyFrameBuffer += cBytesToSend;
    }

    return XBDM_NOERR;
}

HRESULT HrScreenshot(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;
    D3DSurface d3dsurface;
    int width, height, pitch, bitdepth, format;
    DWORD start;

    if(!g_dmgd.Surface) {
        strcpy(szResp, "can't find screen buffer");
        return E_FAIL;
    }

    //pdmgd = DmGetCurrentValue((ULONG)((BYTE*)&dm.D3DDriverData - (BYTE*)&dm));
    d3dsurface = *(g_dmgd.Surface);

    // Stop all threads so that no more Presents are added to the queue.
    DmStop();

    // Busy-wait until all previously queues Presents are processed by the GPU
    start = NtGetTickCount();
    while( *(g_dmgd.FlipCounter) != *(g_dmgd.FrameCounter))
    {
        if ( (NtGetTickCount() - start) >= 2000 ) {
            DmGo();
            strcpy(szResp, "can't lock GPU");
            return E_FAIL;
        }
    }

    // Extract size information
    width = (d3dsurface.Size & D3DSIZE_WIDTH_MASK) + 1;
    height = ((d3dsurface.Size & D3DSIZE_HEIGHT_MASK) >> D3DSIZE_HEIGHT_SHIFT) + 1;
    pitch = (((d3dsurface.Size & D3DSIZE_PITCH_MASK) >> D3DSIZE_PITCH_SHIFT) + 1) * D3DTEXTURE_PITCH_ALIGNMENT;
    format = (d3dsurface.Format & D3DFORMAT_FORMAT_MASK) >> D3DFORMAT_FORMAT_SHIFT;

    switch(format) {
    case D3DFMT_LIN_A8R8G8B8:
        bitdepth = 4;
        break;

    case D3DFMT_LIN_X8R8G8B8:
        bitdepth = 4;
        break;

    case D3DFMT_LIN_R5G6B5:
        bitdepth = 2;
        break;

    case D3DFMT_LIN_X1R5G5B5:
        bitdepth = 2;
        break;

    default:
        DmGo();
        strcpy(szResp, "Unexpected framebuffer format");
        return E_FAIL;
    }

    pccs->Screenshot.pbyFrameBuffer = (BYTE *) (d3dsurface.Data | 0xF0000000);

    pccs->Screenshot.fFirstTime = FALSE;
    pccs->Screenshot.Width = width;
    pccs->Screenshot.Height = height;
    pccs->Screenshot.Pitch = pitch;
    pccs->Screenshot.Format = format;

    // Set up the transfer buffer
    pdmcc->BufferSize = 8192;
    pdmcc->Buffer = DmAllocatePool(pdmcc->BufferSize);
    pdmcc->HandlingFunction = HrSendFrameData;
    pdmcc->BytesRemaining = pitch * height;
    return XBDM_BINRESPONSE;
}

HRESULT HrSendPSSnapshotData(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;

    // end of data?
    if (pdmcc->BytesRemaining <= 0)
    {
        pdmcc->Buffer = NULL;
        *(g_dmgd.pdwOpcode) = PSSNAP_DONE;      // signal D3D to release buffer
        return XBDM_ENDOFLIST;
    }

    pdmcc->DataSize = 32768;
    pdmcc->BytesRemaining = 0;

    return XBDM_NOERR;
}

HRESULT HrPSSnap(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;
    DWORD dwX, dwY, dwOpcode;
    struct
    {
        DWORD dwX;
        DWORD dwY;
        DWORD dwFlags;
        DWORD dwMarker;
    } args;
    DWORD dwStart;

    if(!FGetDwParam(sz, "x", &args.dwX))
    {
        strcpy(szResp, "Missing X parameter");
        return E_FAIL;
    }
    if(!FGetDwParam(sz, "y", &args.dwY))
    {
        strcpy(szResp, "Missing Y parameter");
        return E_FAIL;
    }

    if(!FGetDwParam(sz, "flags", &args.dwFlags))
        args.dwFlags = 0;

    if(!FGetDwParam(sz, "marker", &args.dwMarker))
        args.dwMarker = 0;

    if(*(g_dmgd.pdwOpcode))
    {
        sprintf(szResp, "D3D snapshot client in invalid state (%08x)", *(g_dmgd.pdwOpcode));
        return E_FAIL;
    }

    // build opcode for D3D pixel shader snapshot
    *(g_dmgd.ppSnapshotBuffer) = (BYTE *)(&args);
    *(g_dmgd.pdwOpcode) = PSSNAP_REQUEST;

    // wait for the opcode to change
    dwStart = NtGetTickCount();
    while(1)
    {
        dwOpcode = *(g_dmgd.pdwOpcode) & 0xff000000;
        if((dwOpcode == PSSNAP_ACK) ||
           (dwOpcode == PSSNAP_ERROR))
            break;

        Sleep(50);
        if ( (NtGetTickCount() - dwStart) >= 2000 ) {
            strcpy(szResp, "Opcode timeout:  Failed to receive acknowledge");
            *(g_dmgd.pdwOpcode) = PSSNAP_IDLE;
            return E_FAIL;
        }
    }

    if(dwOpcode == PSSNAP_ERROR)
    {
        switch(*(g_dmgd.pdwOpcode) & 0x00ffffff)
        {
        case 1:
            sprintf(szResp, "The Application is using an unsupported framebuffer format.  Only A8R8G8B8 is supported at this time.");
            break;

        case 2:
            sprintf(szResp, "Debugger doesn't support an Xbox app using a Pure Device - Remove D3DCREATE_PUREDEVICE");
            break;

        case 3:
            sprintf(szResp, "Debugger wasn't able to allocate memory.");
            break;

        default:
            sprintf(szResp, "D3D snapshot client returned error %d",  *(g_dmgd.pdwOpcode) & 0x00ffffff);
            break;
        }

        *(g_dmgd.pdwOpcode) = PSSNAP_IDLE;
        return E_FAIL;
    }

    // Set up the transfer buffer
    pdmcc->BufferSize = 32768;
    pdmcc->Buffer = *(g_dmgd.ppSnapshotBuffer);
    pdmcc->HandlingFunction = HrSendPSSnapshotData;
    pdmcc->BytesRemaining = 32768;

    return XBDM_BINRESPONSE;
}

HRESULT HrSendVSSnapshotData(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;

    // end of data?
    if (pdmcc->BytesRemaining <= 0)
    {
        pdmcc->Buffer = NULL;
        *(g_dmgd.pdwOpcode) = VSSNAP_DONE;      // signal D3D to release buffer
        return XBDM_ENDOFLIST;
    }

    pdmcc->DataSize = 32768;
    pdmcc->BytesRemaining = 0;

    return XBDM_NOERR;
}

HRESULT HrVSSnap(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;
    DWORD dwOpcode;
    DWORD dwStart;
    struct
    {
        DWORD dwFirst;
        DWORD dwLast;
        DWORD dwFlags;
        DWORD dwMarker;
    } args;

    if(*(g_dmgd.pdwOpcode))
    {
        sprintf(szResp, "D3D snapshot client in invalid state (%08x)", *(g_dmgd.pdwOpcode));
        return E_FAIL;
    }

    if(!FGetDwParam(sz, "first", &args.dwFirst))
    {
        strcpy(szResp, sz);
        return E_FAIL;
    }
    if(!FGetDwParam(sz, "last", &args.dwLast))
    {
        strcpy(szResp, "Missing Last parameter");
        return E_FAIL;
    }
    if(!FGetDwParam(sz, "flags", &args.dwFlags))
        args.dwFlags = 0;

    if(!FGetDwParam(sz, "marker", &args.dwMarker))
        args.dwMarker = 0;

    if(args.dwFlags & 0x08000000)
    {
        // xray sequence instead
        *(g_dmgd.pdwOpcode) = XRAY_BEGINREQ;
        *(g_dmgd.ppSnapshotBuffer) = (BYTE *)(&args);

        // wait for the opcode to change
        dwStart = NtGetTickCount();
        while(1)
        {
            dwOpcode = *(g_dmgd.pdwOpcode) & 0xff000000;
            if((dwOpcode == XRAY_BEGINACK) ||
               (dwOpcode == XRAY_ERROR))
                break;

            Sleep(50);
            if ( (NtGetTickCount() - dwStart) >= 2000 ) {
                sprintf(szResp, "Opcode timeout:  Failed to receive acknowledge: %08x", dwOpcode);
                *(g_dmgd.pdwOpcode) = XRAY_IDLE;
                return E_FAIL;
            }
        }
        if(dwOpcode == XRAY_ERROR)
        {
            switch(*(g_dmgd.pdwOpcode) & 0x00ffffff)
            {
            case 2:
                sprintf(szResp, "Debugger doesn't support an Xbox app using a Pure Device - Remove D3DCREATE_PUREDEVICE");
                break;

            default:
                sprintf(szResp, "D3D snapshot client returned error %d",  *(g_dmgd.pdwOpcode) & 0x00ffffff);
                break;
            }
            *(g_dmgd.pdwOpcode) = XRAY_IDLE;
            return E_FAIL;
        }
        *(g_dmgd.pdwOpcode) = XRAY_IDLE;
        return XBDM_NOERR;
    }
    else
    {
        if(args.dwFlags & 0x04000000)
        {
            // end xray sequence instead
            *(g_dmgd.pdwOpcode) = XRAY_ENDREQ;

            // wait for the opcode to change
            dwStart = NtGetTickCount();
            while(1)
            {
                dwOpcode = *(g_dmgd.pdwOpcode) & 0xff000000;
                if((dwOpcode == XRAY_ENDACK) ||
                   (dwOpcode == XRAY_ERROR))
                    break;

                Sleep(50);
                if ( (NtGetTickCount() - dwStart) >= 2000 ) {
                    strcpy(szResp, "Opcode timeout:  Failed to receive acknowledge");
                    *(g_dmgd.pdwOpcode) = VSSNAP_IDLE;
                    return E_FAIL;
                }
            }

            if(dwOpcode == XRAY_ERROR)
            {
                switch(*(g_dmgd.pdwOpcode) & 0x00ffffff)
                {
                case 2:
                    sprintf(szResp, "Debugger doesn't support an Xbox app using a Pure Device - Remove D3DCREATE_PUREDEVICE");
                    break;

                default:
                    sprintf(szResp, "D3D snapshot client returned error %d",  *(g_dmgd.pdwOpcode) & 0x00ffffff);
                    break;
                }
                *(g_dmgd.pdwOpcode) = XRAY_IDLE;
                return E_FAIL;
            }
            *(g_dmgd.pdwOpcode) = XRAY_IDLE;
            return XBDM_NOERR;
        }
    }

    // build opcode for D3D vertex shader snapshot
    dwOpcode = VSSNAP_REQUEST;

    *(g_dmgd.ppSnapshotBuffer) = (BYTE *)(&args);
    *(g_dmgd.pdwOpcode) = dwOpcode;

    // wait for the opcode to change
    dwStart = NtGetTickCount();
    while(1)
    {
        dwOpcode = *(g_dmgd.pdwOpcode) & 0xff000000;
        if((dwOpcode == VSSNAP_ACK) ||
           (dwOpcode == VSSNAP_ERROR))
            break;

        Sleep(50);
        if ( (NtGetTickCount() - dwStart) >= 2000 ) {
            strcpy(szResp, "Opcode timeout:  Failed to receive acknowledge");
            *(g_dmgd.pdwOpcode) = VSSNAP_IDLE;
            return E_FAIL;
        }
    }

    if(dwOpcode == VSSNAP_ERROR)
    {
        switch(*(g_dmgd.pdwOpcode) & 0x00ffffff)
        {
        case 2:
            sprintf(szResp, "Debugger doesn't support an Xbox app using a Pure Device - Remove D3DCREATE_PUREDEVICE");
            break;

        case 3:
            sprintf(szResp, "Debugger wasn't able to allocate memory.");
            break;

        default:
            sprintf(szResp, "D3D snapshot client returned error %d",  *(g_dmgd.pdwOpcode) & 0x00ffffff);
            break;
        }
        *(g_dmgd.pdwOpcode) = VSSNAP_IDLE;
        return E_FAIL;
    }

    // Set up the transfer buffer
    pdmcc->BufferSize = 32768;
    pdmcc->Buffer = *(g_dmgd.ppSnapshotBuffer);
    pdmcc->HandlingFunction = HrSendVSSnapshotData;
    pdmcc->BytesRemaining = 32768;

    return XBDM_BINRESPONSE;
}

HRESULT HrSetLockMode(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    HRESULT hr;

    if(PchGetParam(sz, "unlock", FALSE)) {
        g_fLockLevel = FALSE;
        hr = XBDM_NOERR;
    } else if(FGetQwordParam(sz, "boxid", &g_luBoxId)) {
        g_fLockLevel = TRUE;
        hr = XBDM_NOERR;
    } else {
        /* Potential failure */
        hr = E_FAIL;
        strcpy(szResp, "missing parameters");
    }
    if(PchGetParam(sz, "encrypt", FALSE)) {
        if(g_fLockLevel) {
            g_fLockLevel = 2;
            hr = XBDM_NOERR;
        } else
            /* If unlock was specified, we'll return success as set above;
             * if not, we'll return failure.  In either case, hr is already
             * set correctly, so we'll just make sure the error message
             * is sufficient */
            strcpy(szResp, "box not locked; file encryption not enabled");
    }

    RemoveAllUsers();
    WriteIniFile();
    return hr;
}

HRESULT HrSetDebugSecureMode(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    int icst;

    if(!g_fDebugSecureMode)
        return XBDM_INVALIDCMD;
    if(!pdmcc)
        return E_FAIL;
    icst = (PCST)pdmcc - rgcst;
    rgcst[icst].dmplCur = -1;
    return XBDM_NOERR;
}

HRESULT HrGetBoxId(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
    if(!g_fLockLevel)
        return XBDM_NOTLOCKED;
    sprintf(szResp, "boxid=0q%08x%08x", g_luBoxId.HighPart, g_luBoxId.LowPart);
    return XBDM_NOERR;
}

HRESULT HrAuthenticateUser(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    char szName[256];
    ULARGE_INTEGER luResponse;
    int icst;
    BOOL fKeyXchg;
    LPCSTR pszName;

    if(!pdmcc)
        return E_FAIL;
    icst = (PCST)pdmcc - rgcst;
    if((rgcst[icst].dwStatus & CONN_DHKEY) && FGetQwordParam(sz, "passwd",
        &luResponse))
    {
        luResponse.LowPart ^= rgcst[icst].luDHKey.LowPart;
        luResponse.HighPart ^= rgcst[icst].luDHKey.HighPart;
        fKeyXchg = TRUE;
    } else if(!FGetQwordParam(sz, "resp", &luResponse)) {
noresp:
        strcpy(szResp, "missing response");
        return E_FAIL;
    } else
        fKeyXchg = FALSE;

    if(PchGetParam(sz, "admin", FALSE)) {
        /* Trying to authenticate using the admin passwd.  This requires a
         * valid response */
        if(fKeyXchg)
            goto noresp;
        pszName = NULL;
    } else if(!FGetSzParam(sz, "name", szName, sizeof szName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    } else
        pszName = szName;

    rgcst[icst].dmplCur = DmplAuthenticateUser(pszName,
        &rgcst[icst].luConnectNonce, &luResponse, &fKeyXchg);
    if(!rgcst[icst].dmplCur)
        return fKeyXchg ? XBDM_KEYXCHG : XBDM_CANNOTACCESS;
    return XBDM_NOERR;
}

HRESULT HrSetAdminPasswd(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    int icst;
    ULARGE_INTEGER luPasswd;
    ULARGE_INTEGER luT;
    PULARGE_INTEGER plu;

    if(PchGetParam(sz, "none", FALSE)) {
        g_fAdminPasswd = FALSE;
        WriteIniFile();
        return XBDM_NOERR;
    }

    /* We need to establish a key-exchange key.  But if we're running from the
     * ini file, we'll just use a key-exchange key of 0 to load in the
     * plaintext password */
    if(!pdmcc) {
        luT.QuadPart = 0;
        plu = &luT;
    } else {
        icst = (PCST)pdmcc - rgcst;
        if(!(rgcst[icst].dwStatus & CONN_DHKEY))
            return XBDM_KEYXCHG;
        plu = &rgcst[icst].luDHKey;
    }

    if(!FGetQwordParam(sz, "passwd", &luPasswd)) {
        strcpy(szResp, "missing passwd");
        return E_FAIL;
    }
    g_luAdminPasswd.LowPart = luPasswd.LowPart ^ plu->LowPart;
    g_luAdminPasswd.HighPart = luPasswd.HighPart ^ plu->HighPart;
    g_fAdminPasswd = TRUE;
    WriteIniFile();
    return XBDM_NOERR;
}

HRESULT HrGetUserPriv(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    char szUserName[128];
    DWORD dwPrivileges;
    HRESULT hr;
    int icst;

    dwPrivileges = DmplOfConnection(pdmcc);
    if(PchGetParam(sz, "me", FALSE)) {
        /* We're interested in the privileges for this connection */
        hr = pdmcc ? XBDM_NOERR : E_FAIL;
    } else if(!(dwPrivileges & DMPL_PRIV_MANAGE))
        hr = XBDM_CANNOTACCESS;
    else if(!FGetSzParam(sz, "name", szUserName, sizeof szUserName)) {
        strcpy(szResp, "missing name");
        hr = E_FAIL;
    } else
        hr = DmGetUserAccess(szUserName, &dwPrivileges);

    if(SUCCEEDED(hr)) {
        if(dwPrivileges)
            FillAccessPrivSz(szResp-1, dwPrivileges);
        else
            strcpy(szResp, "none");
    }

    return hr;
}

HRESULT HrSetUserPriv(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    char szUserName[128];
    DWORD dwPrivileges;
    HRESULT hr;

    if(!FGetSzParam(sz, "name", szUserName, sizeof szUserName)) {
        strcpy(szResp, "missing name");
        return E_FAIL;
    }

    dwPrivileges = DmplFromSz(sz);
    return DmSetUserAccess(szUserName, dwPrivileges);
}

HRESULT HrReportUsers(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;
    HRESULT hr;
    DM_USER dusr;

    hr = DmWalkUserList(&pccs->pdmwu, &dusr);
    if(!pdmcc->BytesRemaining)
        hr = XBDM_ENDOFLIST;
    else if(SUCCEEDED(hr))
        FillUserInfoSz(pdmcc->Buffer, dusr.UserName, dusr.AccessPrivileges,
            NULL);
    else {
        DmCloseUserList(pccs->pdmwu);
        hr = XBDM_ENDOFLIST;
    }

    return hr;
}

HRESULT HrListUsers(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    CCS *pccs;
    if(!g_fLockLevel)
        return XBDM_NOTLOCKED;
    if(!pdmcc)
        return E_FAIL;

    pccs = (CCS *)pdmcc->CustomData;
    pdmcc->HandlingFunction = HrReportUsers;
    pccs->pdmwu = NULL;
    return XBDM_MULTIRESPONSE;
}

HRESULT HrPerformDHExchange(PDM_CMDCONT pdmcc, LPSTR szResponse,
    DWORD cchResponse)
{
    extern BYTE g_abOakleyGroup1Base[];
    extern BYTE g_abOakleyGroup1Mod[];
    BYTE rgbDHKey[96];
    BYTE rgbY[96];
    int icst;
    HRESULT hr;

    if(pdmcc->CustomData) {
        /* We're sending, so send all of the data */
        if(pdmcc->BytesRemaining) {
            pdmcc->DataSize = 96;
            pdmcc->BytesRemaining = 0;
            hr = XBDM_NOERR;
        } else {
            DmFreePool(pdmcc->Buffer);
            hr = XBDM_ENDOFLIST;
        }
    } else if(pdmcc->DataSize) {
        /* Grab the data that's here */
        memcpy((PUCHAR)pdmcc->Buffer + 2 * 96 - pdmcc->BytesRemaining,
            pdmcc->Buffer, pdmcc->DataSize);
        pdmcc->BytesRemaining -= pdmcc->DataSize;
        if(!pdmcc->BytesRemaining) {
            /* OK, now we've got g^x at buffer+96.  We need to compute y,
             * g^xy, and g^y.  First generate y */
            XNetRandom(rgbY, 96);
            /* Now compute g^xy */
            XcModExp((PULONG)rgbDHKey, (PULONG)((PUCHAR)pdmcc->Buffer + 96),
                (PULONG)rgbY, (PULONG)g_abOakleyGroup1Mod,
                96 / 4);
            /* And prepare g^y to be sent */
            XcModExp((PULONG)pdmcc->Buffer, (PULONG)g_abOakleyGroup1Base,
                (PULONG)rgbY, (PULONG)g_abOakleyGroup1Mod,
                96 / 4);
            pdmcc->CustomData = pdmcc->Buffer;
            /* Now that we have g^xy, we can hash it and mark the key
             * exchange complete */
            icst = (PCST)pdmcc - rgcst;
            rgcst[icst].luDHKey.QuadPart = 0;
            XBCHashData(&rgcst[icst].luDHKey, rgbDHKey, sizeof rgbDHKey);
            rgcst[icst].dwStatus |= CONN_DHKEY;
            /* Now get ready to send g^y */
            hr = XBDM_BINRESPONSE;
        } else
            hr = XBDM_NOERR;
    } else {
        /* Connection died on receive */
        DmFreePool(pdmcc->Buffer);
        pdmcc->BytesRemaining = 0;
        hr = E_UNEXPECTED;
    }

    return hr;
}

HRESULT HrKeyExchange(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    if(!pdmcc)
        return E_FAIL;

    /* We need to allocate a buffer to build up the remote constant --
     * we can't let it trickle in via the standard buffer */
    pdmcc->BufferSize = 2 * 96;
    pdmcc->Buffer = DmAllocatePoolWithTag(pdmcc->BufferSize, 'HDmd');
    if(!pdmcc->Buffer)
        return E_OUTOFMEMORY;

    /* Set up to receive the remote constant */
    pdmcc->BytesRemaining = 96;
    pdmcc->HandlingFunction = HrPerformDHExchange;
    pdmcc->CustomData = NULL;
    return XBDM_READYFORBIN;
}

HRESULT HrReportMmGlobal(PDM_CMDCONT pdmcc, LPSTR szResponse,
    DWORD cchResponse)
{
    extern PMMGLOBALDATA MmGlobalData;
    PMMGLOBALDATA p = MmGlobalData;
    ULONG MmHighestPhysicalPage;
    PCI_SLOT_NUMBER PCISlotNumber;
    MM_STATISTICS mmstat;
    UCHAR MemoryTop;
    PMMPFN MmPfnDatabase;
    HRESULT hr;
    int n;

    switch (pdmcc->BytesRemaining++) {

    case 1:

        //
        // Access the host bridge's configuration space.
        //

        PCISlotNumber.u.AsULONG = 0;
        PCISlotNumber.u.bits.DeviceNumber = XPCICFG_HOSTBRIDGE_DEVICE_ID;
        PCISlotNumber.u.bits.FunctionNumber = XPCICFG_HOSTBRIDGE_FUNCTION_ID;
        HalReadPCISpace(0, PCISlotNumber.u.AsULONG, CR_CPU_MEMTOP_LIMIT,
            &MemoryTop, sizeof(UCHAR));

        MmHighestPhysicalPage = (((ULONG)MemoryTop + 1) * 4096) - 1;

        MmPfnDatabase = (PMMPFN)(MM_SYSTEM_PHYSICAL_MAP +
            (MiGetPdeAddress(MM_SYSTEM_PHYSICAL_MAP)->Hard.PageFrameNumber << PAGE_SHIFT));

        n = _snprintf(pdmcc->Buffer, pdmcc->BufferSize,
                "MmHighestPhysicalPage=0x%x RetailPfnRegion=0x%x SystemPteRange=0x%x "
                "AvailablePages=0x%x AllocatedPagesByUsage=0x%x PfnDatabase=0x%x",
                MmHighestPhysicalPage, p->RetailPfnRegion, p->SystemPteRange,
                p->AvailablePages, p->AllocatedPagesByUsage, MmPfnDatabase);
        hr = n < 0 ? XBDM_BUFFER_TOO_SMALL : XBDM_NOERR;
        break;

    case 2:
        mmstat.Length = sizeof(mmstat);
        MmQueryStatistics(&mmstat);
        n = _snprintf(pdmcc->Buffer, pdmcc->BufferSize,
                "AddressSpaceLock=0x%x VadRoot=0x%x VadHint=0x%x VadFreeHint=0x%x "
                "MmNumberOfPhysicalPages=0x%x MmAvailablePages=0x%x",
                p->AddressSpaceLock, p->VadRoot, p->VadHint, p->VadFreeHint,
                mmstat.TotalPhysicalPages, mmstat.AvailablePages);

        hr = n < 0 ? XBDM_BUFFER_TOO_SMALL : XBDM_NOERR;
        break;

    default:
        hr = XBDM_ENDOFLIST;
    }

    return hr;
}

HRESULT HrGetMmGlobal(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    CCS *pccs = (CCS *)pdmcc->CustomData;
    pdmcc->HandlingFunction = HrReportMmGlobal;
    pdmcc->BytesRemaining = 1;
    return XBDM_MULTIRESPONSE;
}

HRESULT HrReturnBuffer(PDM_CMDCONT pdmcc, LPSTR szResponse,
    DWORD cchResponse)
{
    if(!pdmcc->BytesRemaining) {
        DmFreePool(pdmcc->Buffer);
        return XBDM_ENDOFLIST;
    }
    pdmcc->DataSize = pdmcc->BytesRemaining;
    pdmcc->BytesRemaining = 0;
    return XBDM_NOERR;
}

HRESULT HrGetDvdBlk(LPCSTR sz, LPSTR szResp, DWORD cchResp,
    PDM_CMDCONT pdmcc)
{
    DWORD dwBlk;
    CCS *pccs;
    HANDLE h;
    IO_STATUS_BLOCK iosb;
    OBJECT_ATTRIBUTES oa;
    OBJECT_STRING ost;
    NTSTATUS st;
    LARGE_INTEGER li;
    PDEVICE_OBJECT pdev;

    if(!pdmcc)
        return E_FAIL;

    if(!FGetNamedDwParam(sz, "block", &dwBlk, szResp))
        return E_FAIL;

    RtlInitObjectString(&ost, "\\Device\\Cdrom0");
    st = ObReferenceObjectByName(&ost, 0, IoDeviceObjectType, NULL, &pdev);
    if(!NT_SUCCESS(st))
        return E_FAIL;
    pdmcc->Buffer = DmAllocatePool(pdmcc->BufferSize = 2048);
    if(!pdmcc->Buffer) {
        ObDereferenceObject(pdev);
        return E_FAIL;
    }
    li.QuadPart = dwBlk;
    li.QuadPart *= 2048;
    st = IoSynchronousFsdRequest(IRP_MJ_READ, pdev, pdmcc->Buffer,
        2048, &li);
    ObDereferenceObject(pdev);
    if(!NT_SUCCESS(st)) {
        ExFreePool(pdmcc->Buffer);
        return E_FAIL;
    }
    pdmcc->HandlingFunction = HrReturnBuffer;
    pdmcc->BytesRemaining = 2048;
    return XBDM_BINRESPONSE;
}

/* This list must be sorted */
CHH rgchh[] = {
    { "adminpw", DMPL_PRIV_MANAGE, HrSetAdminPasswd },
    { "altaddr", DMPL_PRIV_CONTROL, HrGetAltAddr },
    { "authuser", 0, HrAuthenticateUser },
    { "boxid", 0, HrGetBoxId },
    { "break", DMPL_PRIV_CONTROL, HrDoBreak },
    { "bye", 0, HrEndConversation, 0, CHH_ANYTHREAD },
    { "capcontrol", DMPL_PRIV_CONTROL, HrCAPControl },
    { "continue", DMPL_PRIV_CONTROL, HrContinueThread },
    { "dbgname", 0, HrSetDbgName },
    { "debugger", DMPL_PRIV_CONTROL, HrConnectDebugger },
    { "debugmode", 0, HrSetDebugSecureMode },
    { "dedicate", 0, HrDedicateConnection, 0, CHH_ANYTHREAD },
    { "deftitle", DMPL_PRIV_CONTROL, HrSetDefaultTitle },
    { "delete", 0, HrDeleteFile, DMPL_PRIV_READ | DMPL_PRIV_WRITE },
    { "dirlist", DMPL_PRIV_READ, HrGetDirList },
    { "drivefreespace", DMPL_PRIV_READ, HrGetDriveFreeSpace },
    { "drivelist", DMPL_PRIV_READ, HrGetDriveList },
    { "dvdblk", DMPL_PRIV_READ, HrGetDvdBlk },
    { "flash", DMPL_PRIV_CONFIGURE, HrFlashKernelImage },
    { "fmtfat", DMPL_PRIV_CONFIGURE, HrFormatFAT },
    { "funccall", DMPL_PRIV_CONTROL, HrFunctionCall },
    { "getcontext", DMPL_PRIV_CONTROL, HrGetContext },
    { "getextcontext", DMPL_PRIV_CONTROL, HrGetExtContext },
    { "getfile", DMPL_PRIV_READ, HrSendFile },
    { "getfileattributes", DMPL_PRIV_READ | DMPL_PRIV_WRITE, HrGetFileAttributes },
    { "getmem", DMPL_PRIV_CONTROL, HrGetMemory },
    { "getuserpriv", 0, HrGetUserPriv },
    { "go", DMPL_PRIV_CONTROL, HrGo },
    { "gpucount", DMPL_PRIV_CONTROL, HrToggleGPUCounters },
    { "halt", DMPL_PRIV_CONTROL, HrHaltThread },
    { "irtsweep", DMPL_PRIV_CONTROL, HrIrtSweep },
    { "isbreak", DMPL_PRIV_CONTROL, HrIsBreak },
    { "isstopped", DMPL_PRIV_CONTROL, HrIsStopped },
    { "kd", DMPL_PRIV_CONTROL, HrToggleKDState },
    { "keyxchg", 0, HrKeyExchange },
    { "lockmode", DMPL_PRIV_MANAGE, HrSetLockMode },
    { "magicboot", DMPL_PRIV_CONTROL, HrMagicReboot },
    { "mkdir", DMPL_PRIV_WRITE, HrMkdir },
    { "mmglobal", DMPL_PRIV_CONTROL, HrGetMmGlobal },
    { "modlong", DMPL_PRIV_CONTROL, HrDoLongName },
    { "modsections", DMPL_PRIV_CONTROL, HrListModuleSections },
    { "modules", DMPL_PRIV_CONTROL, HrListModules },
    { "nostopon", DMPL_PRIV_CONTROL, HrNostopon },
    { "notify", DMPL_PRIV_CONTROL, HrSetupNotify },
    { "notifyat", DMPL_PRIV_CONTROL, HrSetupNotifyAt },
    { "pclist", DMPL_PRIV_CONTROL, HrListCounters },
    { "pssnap", DMPL_PRIV_CONTROL, HrPSSnap },
    { "querypc", DMPL_PRIV_CONTROL, HrQueryPerformanceCounter },
    { "reboot", DMPL_PRIV_CONTROL, HrReboot },
    { "rename", 0, HrRenameFile, DMPL_PRIV_READ | DMPL_PRIV_WRITE },
    { "resume", DMPL_PRIV_CONTROL, HrResumeThread },
    { "screenshot", DMPL_PRIV_CONTROL, HrScreenshot },
    { "sendfile", DMPL_PRIV_WRITE, HrReceiveFile },
    { "setconfig", DMPL_PRIV_CONFIGURE, HrSetConfig },
    { "setcontext", DMPL_PRIV_CONTROL, HrSetContext },
    { "setfileattributes", DMPL_PRIV_WRITE, HrSetFileAttributes },
    { "setmem", DMPL_PRIV_CONTROL, HrSetMemory },
    { "setsystime", DMPL_PRIV_CONFIGURE, HrSetSystemTime },
    { "setuserpriv", DMPL_PRIV_MANAGE, HrSetUserPriv },
    { "stop", DMPL_PRIV_CONTROL, HrStop },
    { "stopon", DMPL_PRIV_CONTROL, HrStopon },
    { "suspend", DMPL_PRIV_CONTROL, HrSuspendThread },
    { "sysfileupd", DMPL_PRIV_CONTROL, HrUpdateSystemFile },
    { "systime", 0, HrSystemTime },
    { "threadinfo", DMPL_PRIV_CONTROL, HrThreadInfo },
    { "threads", DMPL_PRIV_CONTROL, HrListThreads },
    { "title", DMPL_PRIV_CONTROL, HrSetTitle },
    { "user", DMPL_PRIV_MANAGE, HrAddUserCommand },
    { "userlist", DMPL_PRIV_MANAGE, HrListUsers },
    { "vssnap", DMPL_PRIV_CONTROL, HrVSSnap },
    { "xbeinfo", DMPL_PRIV_CONTROL, HrXbeInfo },
    { "xtlinfo", DMPL_PRIV_CONTROL, HrXapiInfo },
};

int cchh = sizeof rgchh / sizeof rgchh[0];
