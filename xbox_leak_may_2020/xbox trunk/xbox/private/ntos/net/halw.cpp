// ---------------------------------------------------------------------------------------
// halw.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

#ifdef XNET_FEATURE_WINDOWS

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <rsa.h>
#include <rc4.h>
#include <sha.h>
#include <modes.h>
#include <des.h>
#include <tripldes.h>
#include <benaloh.h>
#include <shahmac.h>
#include <cryptkeys.h>

// ---------------------------------------------------------------------------------------
// Hal Dpc Support
// ---------------------------------------------------------------------------------------

struct CHalDpc
{
    LONG                _lInitLock;             // Spin lock to synchronize VLanInit/VLanTerm
    UINT                _cRefs;                 // Number of times HalDpcInit called
    HANDLE              _hEvent;                // Handle to signalling event for dispatch thread
    HANDLE              _hThread;               // Thread handle for dispatch thread
    BOOL                _fShutdown;             // TRUE to shutdown the dispatch thread
    DWORD               _dwThreadId;            // Thread Id of dispatch thread
    DWORD               _dwDpcThreadId;         // Thread Id of thread currently at dispatch level
    INT                 _iThreadPriority;       // Old thread priority of thread at dispatch level
    LIST_ENTRY          _dpcq;                  // Queued KDPCs
    LIST_ENTRY          _timerq;                // Queued KTIMERs
    CRITICAL_SECTION    _cs;                    // Synchronization
};

CHalDpc                 g_HalDpc;

DWORD WINAPI HalDpcThreadProc(void *)
{
    PKDPC       pkdpc;
    PKTIMER     pktimer;
    DWORD       dwNow;
    DWORD       dwWait;
    LONG        lWait;

    while (1)
    {
        EnterCriticalSection(&g_HalDpc._cs);

        if (g_HalDpc._fShutdown)
            break;

        Assert(g_HalDpc._dwThreadId == GetCurrentThreadId());
        Assert(g_HalDpc._dwDpcThreadId == 0);
        g_HalDpc._dwDpcThreadId = g_HalDpc._dwThreadId;
        Assert(g_HalDpc._dwDpcThreadId != 0);

        while (!IsListEmpty(&g_HalDpc._dpcq))
        {
            pkdpc = (PKDPC)RemoveHeadList(&g_HalDpc._dpcq);
            pkdpc->DpcListEntry.Flink = NULL;
            pkdpc->DeferredRoutine(pkdpc,
                                   pkdpc->DeferredContext,
                                   pkdpc->SystemArgument1,
                                   pkdpc->SystemArgument2);
        }

        dwWait = INFINITE;

        if (!IsListEmpty(&g_HalDpc._timerq))
        {
            while (1)
            {
                dwNow   = GetTickCount();
                dwWait  = INFINITE;
                pktimer = (PKTIMER)g_HalDpc._timerq.Flink;
                pkdpc   = NULL;

                while (pktimer != (PKTIMER)&g_HalDpc._timerq)
                {
                    lWait = (LONG)(pktimer->DueTime - dwNow);

                    if (lWait <= 0)
                    {
                        pktimer->DueTime = dwNow + pktimer->Period;
                        pkdpc = pktimer->Dpc;
                        break;
                    }

                    if (dwWait > (DWORD)lWait)
                        dwWait = (DWORD)lWait;

                    pktimer = (PKTIMER)pktimer->TimerListEntry.Flink;
                }

                if (pkdpc == NULL)
                    break;

                pkdpc->DeferredRoutine(pkdpc,
                                       pkdpc->DeferredContext,
                                       pkdpc->SystemArgument1,
                                       pkdpc->SystemArgument2);
            }
        }

        Assert(g_HalDpc._dwThreadId == GetCurrentThreadId());
        Assert(g_HalDpc._dwDpcThreadId == g_HalDpc._dwThreadId);
        g_HalDpc._dwDpcThreadId = 0;
        LeaveCriticalSection(&g_HalDpc._cs);
        WaitForSingleObject(g_HalDpc._hEvent, dwWait);
    }

    return(0);
}

void HalDpcCleanup()
{
    BOOL fResult = FALSE;

    if (g_HalDpc._hThread)
    {
        Assert(g_HalDpc._dwDpcThreadId == GetCurrentThreadId());
        g_HalDpc._fShutdown = TRUE;
        LeaveCriticalSection(&g_HalDpc._cs);
        SetEvent(g_HalDpc._hEvent);
        WaitForSingleObject(g_HalDpc._hThread, INFINITE);
        CloseHandle(g_HalDpc._hThread);
    }

    if (g_HalDpc._hEvent)
    {
        CloseHandle(g_HalDpc._hEvent);
    }

    DeleteCriticalSection(&g_HalDpc._cs);

    memset(&g_HalDpc, 0, sizeof(g_HalDpc));
}

BOOL HalDpcInit()
{
    BOOL fResult = FALSE;

    while (InterlockedCompareExchange(&g_HalDpc._lInitLock, 1, 0))
    {
        Sleep(10);
    }

    if (g_HalDpc._cRefs == 0)
    {
        InitializeCriticalSection(&g_HalDpc._cs);
        InitializeListHead(&g_HalDpc._dpcq);
        InitializeListHead(&g_HalDpc._timerq);

        g_HalDpc._hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

        if (g_HalDpc._hEvent == NULL)
        {
            TraceSz1(Warning, "HalDpcInit - CreateEvent failed (%d)", GetLastError());
            goto ret;
        }

        g_HalDpc._hThread = CreateThread(NULL, 0, HalDpcThreadProc, NULL, 0, &g_HalDpc._dwThreadId);

        if (g_HalDpc._hThread == NULL)
        {
            TraceSz1(Warning, "HalDpcInit - CreateThread failed (%d)", GetLastError());
            goto ret;
        }

        // Increase the priority of the DPC thread so that is preempts any user thread, 
        // and also preempts the VLan thread (which runs at THREAD_PRIORITY_ABOVE_NORMAL).

        SetThreadPriority(g_HalDpc._hThread, THREAD_PRIORITY_HIGHEST);
    }

    g_HalDpc._cRefs += 1;
    fResult = TRUE;

ret:

    if (!fResult)
    {
        HalDpcCleanup();
    }

    g_HalDpc._lInitLock = 0;

    return(fResult);
}

BOOL HalDpcTerm()
{
    BOOL fResult = FALSE;

    while (InterlockedCompareExchange(&g_HalDpc._lInitLock, 1, 0))
    {
        Sleep(10);
    }

    Assert(g_HalDpc._cRefs > 0);

    if (--g_HalDpc._cRefs == 0)
    {
        HalDpcCleanup();
        fResult = TRUE;
    }

    g_HalDpc._lInitLock = 0;

    return(fResult);
}

// ---------------------------------------------------------------------------------------
// CXnHal
// ---------------------------------------------------------------------------------------

NTSTATUS CXnHal::HalInit(XNetInitParams * pxnip)
{
    TCHECK(USER);

    if (!HalDpcInit())
    {
        return(NETERR_MEMORY);
    }

    strncpy(_achXbox, pxnip->pszXbox ? pxnip->pszXbox : "xb1@Lan", sizeof(_achXbox) - 1);

    SetInitFlag(INITF_HAL);

    return(NETERR_OK);
}

void CXnHal::HalTerm()
{
    TCHECK(UDPC);

    SetInitFlag(INITF_HAL_TERM);

    if (TestInitFlag(INITF_HAL))
    {
        if (HalDpcTerm())
        {
            SetInitFlag(INITF_HAL_STOP);
        }
    }
}

KIRQL CXnHal::KeGetCurrentIrql()
{
    ICHECK(HAL, USER|UDPC|SDPC);

    DWORD dwThreadId = GetCurrentThreadId();

    if (dwThreadId == g_HalDpc._dwDpcThreadId)
        return(DISPATCH_LEVEL);
    else
    {
        Assert(dwThreadId != g_HalDpc._dwThreadId);
        return(PASSIVE_LEVEL);
    }
}

KIRQL CXnHal::KeRaiseIrqlToDpcLevel()
{
    ICHECK(HAL, USER|UDPC|SDPC);

    DWORD dwThreadId = GetCurrentThreadId();

    EnterCriticalSection(&g_HalDpc._cs);

    Assert(dwThreadId != g_HalDpc._dwThreadId || dwThreadId == g_HalDpc._dwDpcThreadId);

    if (dwThreadId == g_HalDpc._dwDpcThreadId)
    {
        LeaveCriticalSection(&g_HalDpc._cs);
        Assert(DISPATCH_LEVEL != PASSIVE_LEVEL);
        return(DISPATCH_LEVEL);
    }

    g_HalDpc._dwDpcThreadId = dwThreadId;
    Assert(g_HalDpc._dwDpcThreadId != 0);
    Assert(dwThreadId != g_HalDpc._dwThreadId);

    if (dwThreadId != g_HalDpc._dwThreadId)
    {
        g_HalDpc._iThreadPriority = GetThreadPriority(GetCurrentThread());
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    }

    return(PASSIVE_LEVEL);
}

void CXnHal::KeLowerIrql(KIRQL kirql)
{
    TCHECK(USER|UDPC|SDPC);

    if (TestInitFlag(INITF_HAL_STOP))
    {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    }
    else if (kirql == PASSIVE_LEVEL)
    {
        DWORD dwThreadId = GetCurrentThreadId();
        Assert(dwThreadId != g_HalDpc._dwThreadId);
        Assert(dwThreadId == g_HalDpc._dwDpcThreadId);
        g_HalDpc._dwDpcThreadId = 0;

        if (dwThreadId != g_HalDpc._dwThreadId)
        {
            SetThreadPriority(GetCurrentThread(), g_HalDpc._iThreadPriority);
        }

        LeaveCriticalSection(&g_HalDpc._cs);
    }
}

void CXnHal::KeInitializeDpc(PRKDPC Dpc, PKDEFERRED_ROUTINE DeferredRoutine, PVOID DeferredContext)
{
    ICHECK(HAL, USER|UDPC|SDPC);

    Dpc->DeferredRoutine    = DeferredRoutine;
    Dpc->DeferredContext    = DeferredContext;
    Dpc->DpcListEntry.Flink = NULL;
}

BOOLEAN CXnHal::KeInsertQueueDpc(PRKDPC Dpc, PVOID SystemArgument1, PVOID SystemArgument2)
{
    ICHECK(HAL, USER|UDPC|SDPC);

    if (Dpc->DpcListEntry.Flink == NULL)
    {
        EnterCriticalSection(&g_HalDpc._cs);

        if (Dpc->DpcListEntry.Flink == NULL)
        {
            Dpc->SystemArgument1 = SystemArgument1;
            Dpc->SystemArgument2 = SystemArgument2;
            AssertList(&g_HalDpc._dpcq);
            InsertTailList(&g_HalDpc._dpcq, &Dpc->DpcListEntry);
        }

        LeaveCriticalSection(&g_HalDpc._cs);

        SetEvent(g_HalDpc._hEvent);
    }

    return(TRUE);
}

BOOLEAN CXnHal::KeRemoveQueueDpc(PRKDPC Dpc)
{
    ICHECK(HAL, USER|UDPC|SDPC);

    if (Dpc->DpcListEntry.Flink != NULL)
    {
        EnterCriticalSection(&g_HalDpc._cs);

        if (Dpc->DpcListEntry.Flink != NULL)
        {
            AssertListEntry(&g_HalDpc._dpcq, &Dpc->DpcListEntry);
            RemoveEntryList(&Dpc->DpcListEntry);
            Dpc->DpcListEntry.Flink = NULL;
        }

        LeaveCriticalSection(&g_HalDpc._cs);
    }

    return(TRUE);
}

void CXnHal::KeInitializeTimer(PKTIMER Timer)
{
    ICHECK(HAL, USER|UDPC|SDPC);

    Timer->TimerListEntry.Flink = NULL;
}

BOOLEAN CXnHal::KeSetTimerEx(PKTIMER Timer, LARGE_INTEGER DueTime, LONG Period, PKDPC Dpc)
{
    ICHECK(HAL, USER|UDPC|SDPC);

    EnterCriticalSection(&g_HalDpc._cs);

    if (Timer->TimerListEntry.Flink == NULL)
    {
        AssertList(&g_HalDpc._timerq);
        Timer->DueTime = GetTickCount() + (DWORD)(DueTime.QuadPart / -10000);
        Timer->Period  = Period;
        Timer->Dpc     = Dpc;
        InsertTailList(&g_HalDpc._timerq, &Timer->TimerListEntry);
    }

    LeaveCriticalSection(&g_HalDpc._cs);

    SetEvent(g_HalDpc._hEvent);

    return(TRUE);
}

BOOLEAN CXnHal::KeCancelTimer(PKTIMER Timer)
{
    ICHECK(HAL, USER|UDPC|SDPC);

    if (Timer->TimerListEntry.Flink != NULL)
    {
        EnterCriticalSection(&g_HalDpc._cs);

        if (Timer->TimerListEntry.Flink != NULL)
        {
            AssertListEntry(&g_HalDpc._timerq, &Timer->TimerListEntry);
            RemoveEntryList(&Timer->TimerListEntry);
            Timer->TimerListEntry.Flink = NULL;
        }

        LeaveCriticalSection(&g_HalDpc._cs);
    }

    return(TRUE);
}

CRaiseToDpc::CRaiseToDpc(CXnHal * pXnHal)
{
    _pXnHal = pXnHal;
    _irql = pXnHal->KeRaiseIrqlToDpcLevel();
    Assert(_irql == PASSIVE_LEVEL || _irql == DISPATCH_LEVEL);
    Assert(GetCurrentThreadId() != g_HalDpc._dwThreadId || _irql == DISPATCH_LEVEL);
}

CRaiseToDpc::~CRaiseToDpc()
{
    Assert(GetCurrentThreadId() != g_HalDpc._dwThreadId || _irql == DISPATCH_LEVEL);
    Assert(_irql == PASSIVE_LEVEL || _irql == DISPATCH_LEVEL);
    _pXnHal->KeLowerIrql((KIRQL)_irql);
}

#ifdef XNET_FEATURE_ASSERT

BOOL CXnHal::HalThreadCheck(UINT uiCheck)
{
    DWORD dwThreadId = GetCurrentThreadId();

    if (dwThreadId != g_HalDpc._dwDpcThreadId)
        return(uiCheck & USER);
    else if (dwThreadId == g_HalDpc._dwThreadId)
        return(uiCheck & SDPC);
    else
        return(uiCheck & UDPC);
}

#endif

void CXnHal::HalEnterDpc()
{
    EnterCriticalSection(&g_HalDpc._cs);
}

BOOL CXnHal::HalTryEnterDpc()
{
    return(TryEnterCriticalSection(&g_HalDpc._cs));
}

void CXnHal::HalLeaveDpc()
{
    LeaveCriticalSection(&g_HalDpc._cs);
}

// ---------------------------------------------------------------------------------------
// CXnHal::SetInitFlag
// ---------------------------------------------------------------------------------------

void CXnHal::SetInitFlag(DWORD dwFlag)
{
    DWORD dwFlagsOld, dwFlagsNew;

    while (1)
    {
        dwFlagsOld = _dwInitFlags;

        Assert((dwFlagsOld & dwFlag) == 0);

        dwFlagsNew = dwFlagsOld | dwFlag;

        if (InterlockedCompareExchange((LONG *)&_dwInitFlags, (LONG)dwFlagsNew, (LONG)dwFlagsOld) == (LONG)dwFlagsOld)
            break;

        TraceSz(Warning, "CXnHal::SetInitFlag: Thread/DPC contention.  Retrying.");
    }
}

// ---------------------------------------------------------------------------------------
// Hal System Memory
// ---------------------------------------------------------------------------------------

DefineTag(VMem, 0);
DefineTag(VMemFront, 0);

DWORD g_dwVMem = 0;

BOOL VMemIsEnabled()
{
#ifdef XNET_FEATURE_VMEM
    if (g_dwVMem == 0)
    {
        char * pszVMem = getenv("VMEM");

        g_dwVMem = VMEM_CHECKED;

        if (pszVMem)
        {
            g_dwVMem |= VMEM_ENABLED;

            if (lstrcmpiA(pszVMem, "frontside") != 0)
            {
                g_dwVMem |= VMEM_BACKSIDESTRICT;
            }
        }
        else if (Tag(VMem))
        {
            g_dwVMem |= VMEM_ENABLED;

            if (!Tag(VMemFront))
                g_dwVMem |= VMEM_BACKSIDESTRICT;

        }

        if (g_dwVMem & VMEM_ENABLED)
        {
            TraceSz1(VMem, "Enabled for %s byte faults", (g_dwVMem & VMEM_BACKSIDESTRICT) ? "next" : "prev");
        }
    }

    return(g_dwVMem & VMEM_ENABLED);
#else
    return(FALSE);
#endif
}

void * HalAlloc(size_t cb, ULONG tag)
{
#ifdef XNET_FEATURE_VMEM
    if (VMemIsEnabled())
    {
        return(VMemAlloc(cb));
    }
#endif

    return(LocalAlloc(LMEM_FIXED, cb));
}

void * HalAllocZ(size_t cb, ULONG tag)
{
#ifdef XNET_FEATURE_VMEM
    if (VMemIsEnabled())
    {
        void * pv = VMemAlloc(cb);

        if (pv)
        {
            memset(pv, 0, cb);
        }

        return(pv);
    }
#endif

    return(LocalAlloc(LPTR, cb));
}

void HalFree(void * pv)
{
#ifdef XNET_FEATURE_VMEM
    if (VMemIsEnabled())
    {
        VMemFree(pv);
        return;
    }
#endif

    if (pv)
    {
        LocalFree(pv);
    }
}

// ---------------------------------------------------------------------------------------
// Hal Randomness
// ---------------------------------------------------------------------------------------

UINT CXnHal::HalRandGather(BYTE * pb, UINT cb)
{
    memset(pb, 0xAA, cb);
    QueryPerformanceCounter((LARGE_INTEGER *)pb);
    GetSystemTimeAsFileTime((FILETIME *)(pb + sizeof(LARGE_INTEGER)));
    return(cb);
}

// ---------------------------------------------------------------------------------------
// Hal Crypto
// ---------------------------------------------------------------------------------------

void
XcSHAInit(
    IN PUCHAR pbSHAContext
    )
{
    A_SHAInit((A_SHA_CTX*)pbSHAContext);
}
   
void
XcSHAUpdate(
    IN PUCHAR pbSHAContext,
    IN PUCHAR pbInput,
    IN ULONG dwInputLength
    )
{
    A_SHAUpdate((A_SHA_CTX*)pbSHAContext, pbInput, dwInputLength);
}
   
void
XcSHAFinal(
    IN PUCHAR pbSHAContext,
    IN PUCHAR pbDigest
    )
{
    A_SHAFinal((A_SHA_CTX*)pbSHAContext, pbDigest);
}
   
void
XcRC4Key(
    IN PUCHAR pbKeyStruct,
    IN ULONG dwKeyLength,
    IN PUCHAR pbKey
    )
{
    rc4_key((RC4_KEYSTRUCT*)pbKeyStruct, dwKeyLength, pbKey);
}
   
void
XcRC4Crypt(
    IN PUCHAR pbKeyStruct,
    IN ULONG dwInputLength,
    IN PUCHAR pbInput
    )
{
    rc4((RC4_KEYSTRUCT*)pbKeyStruct, dwInputLength, pbInput);
}
   
void
XcHMAC(
    IN PBYTE pbKeyMaterial,
    IN ULONG cbKeyMaterial,
    IN PBYTE pbData,
    IN ULONG cbData,
    IN PBYTE pbData2,
    IN ULONG cbData2,
    OUT PBYTE HmacData // length must be A_SHA_DIGEST_LEN
    )
{
#define HMAC_K_PADSIZE              64
#define HMAC_KI_XOR_FACTOR            ((DWORD)0x36363636)
#define HMAC_KO_XOR_FACTOR            ((DWORD)0x5C5C5C5C)
    BYTE Kipad[HMAC_K_PADSIZE];
    BYTE Kopad[HMAC_K_PADSIZE];
    BYTE HMACTmp[HMAC_K_PADSIZE+A_SHA_DIGEST_LEN];
    ULONG dwBlock;
    A_SHA_CTX shaHash;

    // truncate
    if (cbKeyMaterial > HMAC_K_PADSIZE)
        cbKeyMaterial = HMAC_K_PADSIZE;

    RtlZeroMemory(Kipad, HMAC_K_PADSIZE);
    RtlCopyMemory(Kipad, pbKeyMaterial, cbKeyMaterial);

    RtlZeroMemory(Kopad, HMAC_K_PADSIZE);
    RtlCopyMemory(Kopad, pbKeyMaterial, cbKeyMaterial);

    //
    // Kipad, Kopad are padded sMacKey. Now XOR across...
    //
    for(dwBlock=0; dwBlock<HMAC_K_PADSIZE/sizeof(DWORD); dwBlock++)
    {
        ((DWORD*)Kipad)[dwBlock] ^= HMAC_KI_XOR_FACTOR;
        ((DWORD*)Kopad)[dwBlock] ^= HMAC_KO_XOR_FACTOR;
    }

    //
    // prepend Kipad to data, Hash to get H1
    //

    A_SHAInit(&shaHash);
    A_SHAUpdate(&shaHash, Kipad, HMAC_K_PADSIZE);
    if (cbData != 0)
    {
        A_SHAUpdate(&shaHash, pbData, cbData);
    }
    if (cbData2 != 0)
    {
        A_SHAUpdate(&shaHash, pbData2, cbData2);
    }

    // Finish off the hash
    A_SHAFinal(&shaHash,HMACTmp+HMAC_K_PADSIZE);

    // prepend Kopad to H1, hash to get HMAC
    RtlCopyMemory(HMACTmp, Kopad, HMAC_K_PADSIZE);

    // final hash: output value into passed-in buffer
    A_SHAInit(&shaHash);
    A_SHAUpdate(&shaHash,HMACTmp, sizeof(HMACTmp));
    A_SHAFinal(&shaHash,HmacData);
}

// compute A = B ^ C mod D, N = len of params in DWORDs
ULONG
XcModExp(
    IN LPDWORD pA,
    IN LPDWORD pB,
    IN LPDWORD pC,
    IN LPDWORD pD,
    IN ULONG dwN
    )
{
    //
    // compute A = B ^ C mod D
    //
    return BenalohModExp(pA, pB, pC, pD, dwN);
}
   
void
XcDESKeyParity(
    IN PUCHAR pbKey,
    IN ULONG dwKeyLength
    )
{
    desparityonkey(pbKey, dwKeyLength);
}
   
void
XcKeyTable(
    IN ULONG dwCipher,
    OUT PUCHAR pbKeyTable,
    IN PUCHAR pbKey
    )
{
    if (dwCipher == XC_SERVICE_DES_CIPHER)
    {
        deskey((DESTable*)pbKeyTable, pbKey);
    }
    else
    {
        // Assume XC_SERVICE_DES3_CIPHER:
        tripledes3key((PDES3TABLE)pbKeyTable, pbKey);
    }
}
   
void
XcBlockCrypt(
    IN ULONG dwCipher,
    IN PUCHAR pbOutput,
    IN PUCHAR pbInput,
    IN PUCHAR pbKeyTable,
    IN ULONG dwOp
    )
{
    void (RSA32API *pCipher)(BYTE *, BYTE *, void *, int) = (dwCipher == XC_SERVICE_DES_CIPHER) ? des : tripledes;
    (*pCipher)( pbOutput, pbInput, pbKeyTable, dwOp );
}
   
void
XcBlockCryptCBC(
    IN ULONG dwCipher,
    IN ULONG dwInputLength,
    IN PUCHAR pbOutput,
    IN PUCHAR pbInput,
    IN PUCHAR pbKeyTable,
    IN ULONG dwOp,
    IN PUCHAR pbFeedback
    )
{
    BYTE *pbInputEnd = pbInput + dwInputLength;
    void (RSA32API *pCipher)(BYTE *, BYTE *, void *, int) = (dwCipher == XC_SERVICE_DES_CIPHER) ? des : tripledes;
    while (pbInput < pbInputEnd)
    {
        CBC(pCipher, XC_SERVICE_DES_BLOCKLEN, pbOutput, pbInput, pbKeyTable, dwOp, pbFeedback);
        pbInput += XC_SERVICE_DES_BLOCKLEN;
        pbOutput += XC_SERVICE_DES_BLOCKLEN;
    }
}

DWORD
XcCalcKeyLen(
    IN  PBYTE  pbPublicKey
    )
{
    LPBSAFE_PUB_KEY pPubKey = (LPBSAFE_PUB_KEY)pbPublicKey;

    return pPubKey->keylen;
}

//
// Reverse ASN.1 Encodings of possible hash identifiers.  
//
static char * shaEncodings[] = {
            //      1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18
            "\x0f\x14\x04\x00\x05\x1a\x02\x03\x0e\x2b\x05\x06\x09\x30\x21\x30",
            "\x0d\x14\x04\x1a\x02\x03\x0e\x2b\x05\x06\x07\x30\x1f\x30",
            "\x00" };

BOOL XcVerifyPKCS1SigningFmt(
    IN  BSAFE_PUB_KEY* pKey,
    IN  BYTE* pbDigest,
    IN  BYTE* pbPKCS1Format
    )
{
    BYTE**    rgEncOptions;
    BYTE      rgbTmpHash[A_SHA_DIGEST_LEN + 16];
    DWORD     i;
    DWORD     cb;
    BYTE*     pbStart;
    DWORD     cbTmp;

    rgEncOptions = (BYTE **)shaEncodings;

    // 
    // reverse the hash to match the signature.
    //
    for (i = 0; i < A_SHA_DIGEST_LEN; i++) {
        rgbTmpHash[i] = pbDigest[A_SHA_DIGEST_LEN - (i + 1)];
    }

    // 
    // see if it matches.
    //
    if (memcmp(rgbTmpHash, pbPKCS1Format, A_SHA_DIGEST_LEN)) {
        return FALSE;
    }

    cb = A_SHA_DIGEST_LEN;

    //
    // check for any signature type identifiers
    //
    for (i = 0; 0 != *rgEncOptions[i]; i += 1) {
        pbStart = (LPBYTE)rgEncOptions[i];
        cbTmp = *pbStart++;
        if (0 == memcmp(&pbPKCS1Format[cb], pbStart, cbTmp)) {
            // adjust the end of the hash data. 
            cb += cbTmp;   
            break;
        }
    }

    // 
    // check to make sure the rest of the PKCS #1 padding is correct
    //
    if ((0x00 != pbPKCS1Format[cb]) || (0x00 != pbPKCS1Format[pKey->datalen]) ||
         (0x1 != pbPKCS1Format[pKey->datalen - 1])) {
        return FALSE;
    }

    for (i = cb + 1; i < (DWORD)pKey->datalen - 1; i++) {
        if (0xff != pbPKCS1Format[i]) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOLEAN
XcVerifyDigest(
    IN   PBYTE   pbSig,
    IN   PBYTE   pbPublicKey,
    IN   PBYTE   pbWorkspace,
    IN   PBYTE   pbCompareDigest
    )
{
    LPBSAFE_PUB_KEY pPubKey = (LPBSAFE_PUB_KEY)pbPublicKey;
    PBYTE           pbOutput;
    PBYTE           pbInput;
    DWORD           dwSigLen;

    dwSigLen = (pPubKey->bitlen + 7) / 8;

    pbOutput = pbWorkspace;
    pbInput = (PBYTE)((ULONG_PTR)pbWorkspace + pPubKey->keylen);

    memset(pbInput, 0, pPubKey->keylen);
    memcpy(pbInput, pbSig, dwSigLen);

    if (!BSafeEncPublic(pPubKey, pbInput, pbOutput)) {
        return FALSE;
    }


    if (!XcVerifyPKCS1SigningFmt(pPubKey, pbCompareDigest, pbOutput)) {
        return FALSE;
    }

    return TRUE;
}

ULONG
XcPKGetKeyLen(
    IN PUCHAR pbPubKey
    )
{
    return XcCalcKeyLen(pbPubKey);
}

BOOLEAN
XcVerifyPKCS1Signature(
    IN PUCHAR pbSig,
    IN PUCHAR pbPubKey,
    IN PUCHAR pbDigest
    )
{
    BYTE* pbWorkspace = (BYTE *)_alloca( 2 * XcCalcKeyLen(pbPubKey) );
    if ( pbWorkspace == NULL )
    {
        return FALSE;
    }
    return XcVerifyDigest(pbSig, pbPubKey, pbWorkspace, pbDigest);
}
   
VOID WINAPI XShaHmacInitialize(
            IN PBYTE                pbKey,
            IN DWORD                cbKey,
            IN OUT XSHAHMAC_CONTEXT Shactx
            )
{
    HRESULT        hr = S_OK;
    BYTE        rgbKipad[HMAC_K_PADSIZE];
    ULONG        dwBlock;

    // Shorten length if longer than our K padding
    if (cbKey > HMAC_K_PADSIZE)
        cbKey = HMAC_K_PADSIZE;

    // Build our Kipad
    memset(rgbKipad, 0, HMAC_K_PADSIZE);
    memcpy(rgbKipad, pbKey, cbKey);
    for (dwBlock = 0; 
         dwBlock < (HMAC_K_PADSIZE/sizeof(DWORD)); 
         dwBlock++)
    {
        ((DWORD*)rgbKipad)[dwBlock] ^= HMAC_KI_XOR_FACTOR;
    }

    // Initialize our SHA1 Hmac context
    XcSHAInit(Shactx);

    // Run our Kipad through this ...
    XcSHAUpdate(Shactx, rgbKipad, HMAC_K_PADSIZE);
}

//
// Function to update the Hmac
//
VOID WINAPI XShaHmacUpdate(
            IN XSHAHMAC_CONTEXT Shactx,
            IN PBYTE            pbData,
            IN DWORD            cbData
            )
{
    XcSHAUpdate(Shactx, pbData, cbData);
}
            
//
// Function to calculate the final Hmac
//
VOID WINAPI XShaHmacComputeFinal(
            IN XSHAHMAC_CONTEXT Shactx,
            IN PBYTE            pbKey,
            IN DWORD            cbKey,
            OUT PBYTE           pbHmac
            )
{
    HRESULT        hr = S_OK;
    BYTE        rgbKopad[HMAC_K_PADSIZE + A_SHA_DIGEST_LEN];
    ULONG        dwBlock;

    // Shorten length if longer than our K padding
    if (cbKey > HMAC_K_PADSIZE)
        cbKey = HMAC_K_PADSIZE;

    // Build our Kipad
    memset(rgbKopad, 0, HMAC_K_PADSIZE);
    memcpy(rgbKopad, pbKey, cbKey);
    for (dwBlock = 0; 
         dwBlock < (HMAC_K_PADSIZE/sizeof(DWORD)); 
         dwBlock++)
    {
        ((DWORD*)rgbKopad)[dwBlock] ^= HMAC_KO_XOR_FACTOR;
    }

    // Finish off the hash, and place the result right
    // after the Kopad data
    XcSHAFinal(Shactx, rgbKopad + HMAC_K_PADSIZE);

    // Do another hash, now with the Kopad data
    XcSHAInit(Shactx);
    XcSHAUpdate(Shactx, rgbKopad, sizeof(rgbKopad));
    XcSHAFinal(Shactx, pbHmac);
}            

// ---------------------------------------------------------------------------------------
// VMem
//
// VMem allocates memory using the operating system's low-level virtual allocator. It
// arranges for an allocation to start at the very beginning of a page, with a non-
// accessable page just before it, or for it to end at the very end of a page, with a
// non-accessible page just after it.  The idea is to catch memory overwrites quickly.
//
// The layout of an allocation is as follows:
//
//  +--- page VMEMINFO structure
//  |
//  |                 +--- pv if using front-side-strict memory allocations
//  |                 |
//  |                 |                   +--- filled with pattern to detect back-side overwrite
//  v                 v                   v
// +--------+--------+--------+--------+--------+--------+
// |VMEMINFO|   NO   |[User's memory area]XXXXXX|   NO   |
// |        | ACCESS |XXXXXX[User's memory area]| ACCESS |
// +--------+--------+--------+--------+--------+--------+
//                    ^     ^
//                    |     +--- pv if using back-side-strict memory allocations
//                    |
//                    +--- filled with pattern to detect front-side overwrite
//
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_VMEM

#define PAGE_SIZE       4096

DWORD
VMemQueryProtect(void * pv, DWORD cb)
{
    MEMORY_BASIC_INFORMATION mbi = { 0 };
    VirtualQuery(pv, &mbi, sizeof(mbi));
    return (mbi.Protect ? mbi.Protect : mbi.AllocationProtect);
}

VMEMINFO *
VMemIsValid(void * pv)
{
    VMEMINFO * pvmi;
    BYTE * pb;
    UINT cb;

    if (pv == NULL)
    {
        return NULL;
    }

    pvmi = (VMEMINFO *)(((DWORD_PTR)pv & ~(PAGE_SIZE - 1)) - PAGE_SIZE * 2);

    if (VMemQueryProtect(pvmi, PAGE_SIZE) != PAGE_READONLY)
    {
        Rip("VMemIsValid - VMEMINFO page is not marked READONLY");
        return NULL;
    }

    if (pv != pvmi->pv)
    {
        Rip("VMemIsValid - VMEMINFO doesn't point back to pv");
        return NULL;
    }

    if (VMemQueryProtect((BYTE *)pvmi + PAGE_SIZE, PAGE_SIZE) != PAGE_NOACCESS)
    {
        Rip("VMemIsValid - can't detect first no-access page");
        return NULL;
    }

    if (VMemQueryProtect((BYTE *)pvmi + PAGE_SIZE * 2, pvmi->cbFill1 + pvmi->cb + pvmi->cbFill2) != PAGE_READWRITE)
    {
        Rip("VMemIsValid - user memory block is not all writable");
        return NULL;
    }

    if (pvmi->cbFill1)
    {
        pb = (BYTE *)pvmi + PAGE_SIZE * 2;
        cb = pvmi->cbFill1;

        for (; cb > 0; --cb, ++pb)
        {
            if (*pb != 0x1A)
            {
                Rip("VMemIsValid - detected user memory pre-data overwrite");
                return NULL;
            }
        }
    }

    if (pvmi->cbFill2)
    {
        pb = (BYTE *)pvmi + PAGE_SIZE * 2 + pvmi->cbFill1 + pvmi->cb;
        cb = pvmi->cbFill2;

        for (; cb > 0; --cb, ++pb)
        {
            if (*pb != 0x3A)
            {
                Rip("VMemIsValid - detected user memory post-data overwrite");
                return NULL;
            }
        }
    }

    if (VMemQueryProtect((BYTE *)pvmi + PAGE_SIZE * 2 + pvmi->cbFill1 + pvmi->cb + pvmi->cbFill2, PAGE_SIZE) != PAGE_NOACCESS)
    {
        Rip("VMemIsValid - can't detect second no-access page");
        return NULL;
    }

    return(pvmi);
}

void *
VMemAlloc(size_t cb)
{
    DWORD dwFlags = g_dwVMem;
    void * pv1, * pv2, * pv3;
    size_t cbUser, cbPage;
    DWORD dwOldProtect;
    VMEMINFO * pvmi;

    if (cb == 0)
    {
        cb = 1;
    }

    if (    (dwFlags & VMEM_BACKSIDESTRICT)
        &&  (dwFlags & VMEM_BACKSIDEALIGN8))
        cbUser = (cb + 7) & ~7;
    else
        cbUser = cb;

    cbPage = (cbUser + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

	pv1 = VirtualAlloc(0, cbPage + PAGE_SIZE * 3, MEM_RESERVE, PAGE_NOACCESS);

    if (pv1 == NULL)
    {
        return(NULL);
    }

	pv2 = VirtualAlloc(pv1, PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE);

    if (pv2 == NULL)
    {
        return(NULL);
    }

    pvmi          = (VMEMINFO *)pv2;
    pvmi->cb      = cb;
    pvmi->dwFlags = dwFlags;

    pv3 = VirtualAlloc((BYTE *)pv1 + PAGE_SIZE * 2, cbPage, MEM_COMMIT, PAGE_READWRITE);

    if (pv3 == NULL)
    {
        return(NULL);
    }

    if (dwFlags & VMEM_BACKSIDESTRICT)
    {
        pvmi->cbFill1 = cbPage - cbUser;
        pvmi->cbFill2 = cbUser - cb;
    }
    else
    {
        pvmi->cbFill1 = 0;
        pvmi->cbFill2 = cbPage - cbUser;
    }

    Assert(pvmi->cbFill1 + cb + pvmi->cbFill2 == cbPage);

    if (pvmi->cbFill1)
    {
        memset((BYTE *)pv3, 0x1A, pvmi->cbFill1);
    }

    memset((BYTE *)pv3 + pvmi->cbFill1, 0x2A, cb);

    if (pvmi->cbFill2)
    {
        memset((BYTE *)pv3 + pvmi->cbFill1 + cb, 0x3A, pvmi->cbFill2);
    }

    pvmi->pv = (BYTE *)pv3 + pvmi->cbFill1;

    VirtualProtect(pv1, PAGE_SIZE, PAGE_READONLY, &dwOldProtect);

    Assert(VMemIsValid(pvmi->pv));

    return(pvmi->pv);
}

void
VMemFree(void * pv)
{
    VMEMINFO * pvmi = VMemIsValid(pv);

    if (pvmi)
    {
        if (!VirtualFree(pvmi, 0, MEM_RELEASE))
        {
            Rip("VmemFree - VirtualFree failed");
        }
    }
}

#endif // XNET_FEATURE_VMEM

// ---------------------------------------------------------------------------------------
// Assert support
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_ASSERT

struct MBOT { char * psz; int id; };

DWORD WINAPI DbgAssertPopOnThreadFn(MBOT * pmbot)
{
    pmbot->id = MessageBoxExA(NULL, pmbot->psz, "Assert",
                              MB_SYSTEMMODAL|MB_SETFOREGROUND|MB_OKCANCEL|MB_ICONWARNING|MB_DEFBUTTON2,
                              0);
    return(0);
}

int DbgAssertPop(const char * pszExpr, const char * pszFile, int iLine, const char * pszMsg)
{
    char ach[256];

    if (pszMsg)
        _snprintf(ach, sizeof(ach), "%s\n%s\n%s (%d)", pszExpr, pszMsg, pszFile, iLine);
    else
        _snprintf(ach, sizeof(ach), "%s\n%s (%d)", pszExpr, pszFile, iLine);

    MBOT  mbot = { ach, 0 };
    DWORD dwThread;
    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DbgAssertPopOnThreadFn,
                                  &mbot, 0, &dwThread);
    if (!hThread)
        DbgAssertPopOnThreadFn(&mbot);
    else
    {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }

    return(mbot.id == IDCANCEL);
}

#endif

// ---------------------------------------------------------------------------------------
// Trace support
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_TRACE

CTagInit::CTagInit(int * pi, const char * pszTag)
{
    int i = GetPrivateProfileIntA("tags", pszTag, -1, "\\xnettags.ini");

    if (i != -1)
    {
        *pi = i;
    }
}

#endif

// ---------------------------------------------------------------------------------------
// XBox File System Abstraction Layer
// ---------------------------------------------------------------------------------------

BOOL HalCreateDirectory(const char * pszDir, BOOL fHidden, BOOL * pfAlreadyExists)
{
    WIN32_FIND_DATA fd;
    HANDLE          hFind = FindFirstFile(pszDir, &fd);

    *pfAlreadyExists = FALSE;

    if (hFind == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() != ERROR_FILE_NOT_FOUND)
        {
            TraceSz2(Warning, "HalCreateDirectory: FindFirstFile '%s' failed (%08lX)", pszDir, GetLastError());
            return(FALSE);
        }

        if (!CreateDirectory(pszDir, NULL))
        {
            TraceSz2(Warning, "HalCreateDirectory: CreateDirectory '%s' failed (%08lX)", pszDir, GetLastError());
            return(FALSE);
        }

        if (fHidden && !SetFileAttributes(pszDir, FILE_ATTRIBUTE_HIDDEN))
        {
            TraceSz2(Warning, "HalCreateDirectory: SetFileAttributes '%s' failed (%08lX)", pszDir, GetLastError());
            return(FALSE);
        }
    }
    else
    {
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            TraceSz1(Warning, "HalCreateDirectory: Cannot create directory '%s' because file exists there", pszDir);
            return(FALSE);
        }

        FindClose(hFind);

        *pfAlreadyExists = TRUE;
    }

    return(TRUE);
}

BOOL HalGetPath(const char * pszXbox, const char * pszPathIn, char * pszPathOut)
{
    char achXbox[MAX_PATH];
    char achRoot[MAX_PATH];
    char achTemp[MAX_PATH];
    char * pch;
    BOOL fAlreadyExists;

    strncpy(achXbox, pszXbox, sizeof(achXbox));
    for (pch = achXbox; *pch && *pch != '@'; ++pch) ;
    *pch = 0;

    // Make sure directory c:\$xbox exists

    strcpy(achRoot, "c:\\$xbox");
    HalCreateDirectory(achRoot, TRUE, &fAlreadyExists);

    // Make sure directory c:\$xbox\<pszXbox> exists

    strcat(achRoot, "\\");
    strcat(achRoot, achXbox);
    HalCreateDirectory(achRoot, FALSE, &fAlreadyExists);

    if (!fAlreadyExists)
    {
        char achDir[MAX_PATH];

        strcpy(achDir, achRoot);
        strcat(achDir, "\\partition1");
        HalCreateDirectory(achDir, FALSE, &fAlreadyExists);

        strcpy(achDir, achRoot);
        strcat(achDir, "\\partition1\\TDATA");
        HalCreateDirectory(achDir, FALSE, &fAlreadyExists);

        strcpy(achDir, achRoot);
        strcat(achDir, "\\partition1\\CACHE");
        HalCreateDirectory(achDir, FALSE, &fAlreadyExists);

        strcpy(achDir, achRoot);
        strcat(achDir, "\\partition1\\devkit");
        HalCreateDirectory(achDir, FALSE, &fAlreadyExists);

        strcpy(achDir, achRoot);
        strcat(achDir, "\\partition2");
        HalCreateDirectory(achDir, FALSE, &fAlreadyExists);
    }

    // Handle special "eeprom" path

    if (lstrcmpiA(pszPathIn, "eeprom") == 0)
    {
        sprintf(pszPathOut, "%s\\eeprom.bin", achRoot);
        return(TRUE);
    }

    // Handle special "volume" path

    if (lstrcmpiA(pszPathIn, "\\Device\\Harddisk0\\partition0") == 0)
    {
        sprintf(pszPathOut, "%s\\config.bin", achRoot);

        // Make sure this file exists and has room for all the config sectors

        HANDLE hFile = CreateFile(pszPathOut, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hFile == INVALID_HANDLE_VALUE)
        {
            TraceSz2(Warning, "HalGetPath: CreateFile '%s' failed (%08lX)", pszPathOut, GetLastError());
            return(FALSE);
        }

        DWORD dwSize   = GetFileSize(hFile, NULL);
        DWORD dwExpect = (XBOX_CONFIG_SECTOR_INDEX + XBOX_NUM_CONFIG_SECTORS) * XBOX_HD_SECTOR_SIZE;

        if (dwSize != dwExpect)
        {
            if (dwSize != 0)
            {
                TraceSz3(Warning, "HalGetPath: '%s' has incorrect size (%d bytes); should be %d bytes.  Repairing.",
                         pszPathOut, dwSize, dwExpect);
            }

            BYTE ab[(XBOX_CONFIG_SECTOR_INDEX + XBOX_NUM_CONFIG_SECTORS) * XBOX_HD_SECTOR_SIZE];
            memset(ab, 0, sizeof(ab));

            if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0xFFFFFFFF)
            {
                TraceSz2(Warning, "HalGetPath: SetFilePointer '%s' failed (%08lX)", pszPathOut, GetLastError());
                CloseHandle(hFile);
                return(FALSE);
            }

            if (!WriteFile(hFile, ab, sizeof(ab), &dwExpect, NULL))
            {
                TraceSz2(Warning, "HalGetPath: WriteFile '%s' failed (%08lX)", pszPathOut, GetLastError());
                CloseHandle(hFile);
                return(FALSE);
            }

            Assert(dwExpect == sizeof(ab));
        }

        CloseHandle(hFile);
        return(TRUE);
    }

    // Handle Harddisk0 paths

    if (_strnicmp(pszPathIn, "\\Device\\Harddisk0\\", 18) == 0)
    {
        sprintf(pszPathOut, "%s\\%s", achRoot, pszPathIn + 18);
        return(TRUE);
    }

    TraceSz1(Warning, "HalGetPath: Don't understand path '%s'", pszPathIn);
    return(FALSE);
}

HANDLE HalCreateFile(char * pszXbox, LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    NTSTATUS    status = STATUS_UNSUCCESSFUL;
    char        achPath[MAX_PATH];

    if (!HalGetPath(pszXbox, lpFileName, achPath))
    {
        TraceSz2(Warning, "HalCreateFile: HalGetPath('%s','%s') failed", pszXbox, lpFileName);
        return(INVALID_HANDLE_VALUE);
    }
    
    return(CreateFile(achPath, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
                      dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile));
}

NTSTATUS HalNtOpenFile(char * pszXbox, PHANDLE FileHandle, ACCESS_MASK DesiredAccess,
                       POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock,
                       ULONG ShareAccess, ULONG OpenOptions)
{
    NTSTATUS    status = STATUS_UNSUCCESSFUL;
    char        achPath[MAX_PATH];

    if (!HalGetPath(pszXbox, (char *)ObjectAttributes->ObjectName->Buffer, achPath))
    {
        TraceSz2(Warning, "HalNtOpenFile: HalGetPath('%s','%s') failed",
                 pszXbox, (char *)ObjectAttributes->ObjectName->Buffer);
        *FileHandle = INVALID_HANDLE_VALUE;
    }
    else
    {
        *FileHandle = CreateFile(achPath, DesiredAccess & (GENERIC_READ|GENERIC_WRITE),
                                 ShareAccess & (FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE),
                                 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    if (*FileHandle != INVALID_HANDLE_VALUE)
    {
        status = STATUS_SUCCESS;
    }

    return(status);
}

NTSTATUS HalNtCreateFile(char * pszXbox, PHANDLE FileHandle, ACCESS_MASK DesiredAccess,
                         POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock,
                         PLARGE_INTEGER AllocationSize, ULONG FileAttributes,
                         ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions)
{
    NTSTATUS    status = STATUS_UNSUCCESSFUL;
    char        achPath[MAX_PATH];

    if (!HalGetPath(pszXbox, (char *)ObjectAttributes->ObjectName->Buffer, achPath))
    {
        TraceSz2(Warning, "HalNtCreateFile: HalGetPath('%s','%s') failed",
                 pszXbox, (char *)ObjectAttributes->ObjectName->Buffer);
        *FileHandle = INVALID_HANDLE_VALUE;
    }
    else
    {
        DWORD dwDisposition = CREATE_ALWAYS;

        switch (CreateDisposition)
        {
            case FILE_SUPERSEDE:    dwDisposition = TRUNCATE_EXISTING;  break;
            case FILE_OPEN:         dwDisposition = OPEN_ALWAYS;        break;
            case FILE_CREATE:       dwDisposition = CREATE_NEW;         break;
            case FILE_OPEN_IF:      dwDisposition = CREATE_ALWAYS;      break;
            case FILE_OVERWRITE:    dwDisposition = CREATE_ALWAYS;      break;
            case FILE_OVERWRITE_IF: dwDisposition = TRUNCATE_EXISTING;  break;
        }

        *FileHandle = CreateFile(achPath, DesiredAccess & (GENERIC_READ|GENERIC_WRITE),
                                 ShareAccess & (FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE),
                                 NULL, dwDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    if (*FileHandle != INVALID_HANDLE_VALUE)
    {
        status = STATUS_SUCCESS;
    }

    return(status);
}

NTSTATUS HalNtReadFile(char * pszXbox, HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine,
                       PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID Buffer, ULONG Length,
                       PLARGE_INTEGER ByteOffset)
{
    if (SetFilePointer(FileHandle, ByteOffset->LowPart, &ByteOffset->HighPart, FILE_BEGIN) == 0xFFFFFFFF)
    {
        TraceSz1(Warning, "HalNtReadFile: SetFilePointer failed (%08lX)", GetLastError());
        return(STATUS_UNSUCCESSFUL);
    }

    DWORD dwRead;

    if (!ReadFile(FileHandle, Buffer, Length, &dwRead, NULL))
    {
        TraceSz1(Warning, "HalNtReadFile: ReadFile failed (%08lX)", GetLastError());
        return(STATUS_UNSUCCESSFUL);
    }

    if (dwRead != Length)
    {
        TraceSz2(Warning, "HalNtReadFile: ReadFile returned only %d bytes (%d requested)", dwRead, Length);
        return(STATUS_UNSUCCESSFUL);
    }

    return(STATUS_SUCCESS);
}

NTSTATUS HalNtWriteFile(char * pszXbox, HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine,
                        PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID Buffer,
                        ULONG Length, PLARGE_INTEGER ByteOffset)
{
    if (SetFilePointer(FileHandle, ByteOffset->LowPart, &ByteOffset->HighPart, FILE_BEGIN) == 0xFFFFFFFF)
    {
        TraceSz1(Warning, "HalNtWriteFile: SetFilePointer failed (%08lX)", GetLastError());
        return(STATUS_UNSUCCESSFUL);
    }

    DWORD dwWrite;

    if (!WriteFile(FileHandle, Buffer, Length, &dwWrite, NULL))
    {
        TraceSz1(Warning, "HalNtWriteFile: WriteFile failed (%08lX)", GetLastError());
        return(STATUS_UNSUCCESSFUL);
    }

    if (dwWrite != Length)
    {
        TraceSz2(Warning, "HalNtWriteFile: WriteFile wrote only %d bytes (%d requested)", dwWrite, Length);
        return(STATUS_UNSUCCESSFUL);
    }

    return(STATUS_SUCCESS);
}

NTSTATUS HalNtClose(char * pszXbox, HANDLE Handle)
{
    if (Handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(Handle);
    }

    return(STATUS_SUCCESS);
}

// ---------------------------------------------------------------------------------------
// EEPROM
// ---------------------------------------------------------------------------------------

BOOL HalSaveEEPROM(char * pszXbox, EEPROM_LAYOUT * peeprom)
{
    HANDLE  hFile   = INVALID_HANDLE_VALUE;
    BOOL    fResult = FALSE;
    DWORD   dwSize;
    char    achPath[MAX_PATH];

    if (!HalGetPath(pszXbox, "eeprom", achPath))
    {
        TraceSz1(Warning, "HalSaveEEPROM: HalGetPath('%s','eeprom') failed", pszXbox);
        goto ret;
    }

    hFile = CreateFile(achPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,  NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        TraceSz2(Warning, "HalSaveEEPROM: CreateFile '%s' failed (%08lX)", achPath, GetLastError());
        goto ret;
    }

    if (!WriteFile(hFile, peeprom, sizeof(EEPROM_LAYOUT), &dwSize, NULL))
    {
        TraceSz2(Warning, "HalSaveEEPROM: WriteFile '%s' failed (%08lX)", achPath, GetLastError());
        goto ret;
    }

    fResult = TRUE;

ret:

    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
    }

    return(fResult);
}

BOOL HalLoadEEPROM(char * pszXbox, EEPROM_LAYOUT * peeprom)
{
    HANDLE  hFile   = INVALID_HANDLE_VALUE;
    BOOL    fResult = FALSE;
    DWORD   dwSize;
    char    achPath[MAX_PATH];

    if (!HalGetPath(pszXbox, "eeprom", achPath))
    {
        TraceSz1(Warning, "HalLoadEEPROM: HalGetPath('%s','eeprom') failed", pszXbox);
        goto ret;
    }

    hFile = CreateFile(achPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,  NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        TraceSz2(Warning, "HalLoadEEPROM: CreateFile '%s' failed (%08lX)", achPath, GetLastError());
        goto ret;
    }

    if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0xFFFFFFFF)
    {
        TraceSz2(Warning, "HalLoadEEPROM: SetFilePointer '%s' failed (%08lX)", achPath, GetLastError());
        goto ret;
    }

    dwSize = GetFileSize(hFile, NULL);

    if (dwSize == sizeof(EEPROM_LAYOUT))
    {
        if (!ReadFile(hFile, peeprom, sizeof(EEPROM_LAYOUT), &dwSize, NULL))
        {
            TraceSz2(Warning, "HalLoadEEPROM: ReadFile '%s' failed (%08lX)", achPath, GetLastError());
            goto ret;
        }
    }
    else
    {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;

        if (dwSize != 0)
        {
            TraceSz3(Warning, "HalLoadEEPROM: '%s' is incorrect size (%d bytes); should be %d bytes.  Repairing.",
                     achPath, dwSize, sizeof(EEPROM_LAYOUT));
        }

        memset(peeprom, 0, sizeof(EEPROM_LAYOUT));

        XBOX_ENCRYPTED_SETTINGS *   pes = (XBOX_ENCRYPTED_SETTINGS *)&peeprom->EncryptedSection;
        XBOX_FACTORY_SETTINGS *     pfs = (XBOX_FACTORY_SETTINGS *)&peeprom->FactorySection;

        memset(pfs->SerialNumber, '9', sizeof(pfs->SerialNumber));

        Assert(sizeof(pfs->OnlineKey) == DEVKIT_ONLINE_RAND_KEY_LEN);
        memcpy(pfs->OnlineKey, DEVKIT_ONLINE_RAND_KEY, sizeof(pfs->OnlineKey));

        BYTE abHdKey[16] = { 0 };

        MorphKeyByHardDriveKeyEx(abHdKey, pfs->OnlineKey, sizeof(pfs->OnlineKey));

        HalSaveEEPROM(pszXbox, peeprom);
    }

    fResult = TRUE;

ret:

    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
    }

    return(fResult);
}

NTSTATUS HalExQueryNonVolatileSetting(char * pszXbox, ULONG ValueIndex, ULONG * Type, VOID * Value, ULONG ValueLength, ULONG * ResultLength)
{
    EEPROM_LAYOUT   eeprom;
    NTSTATUS        status = STATUS_UNSUCCESSFUL;

    *Type = 0;
    *ResultLength = 0;

    if (HalLoadEEPROM(pszXbox, &eeprom))
    {
        XBOX_FACTORY_SETTINGS * pfs = (XBOX_FACTORY_SETTINGS *)&eeprom.FactorySection;

        switch (ValueIndex)
        {
            case XC_FACTORY_ETHERNET_ADDR:
            {
                if (ValueLength == sizeof(pfs->EthernetAddr))
                {
                    memcpy(Value, pfs->EthernetAddr, ValueLength);
                    *Type = REG_BINARY;
                    *ResultLength = ValueLength;
                    status = STATUS_SUCCESS;
                }
                break;
            }

            case XC_FACTORY_SERIAL_NUMBER:
            {
                if (ValueLength == sizeof(pfs->SerialNumber))
                {
                    memcpy(Value, pfs->SerialNumber, ValueLength);
                    *Type = REG_BINARY;
                    *ResultLength = ValueLength;
                    status = STATUS_SUCCESS;
                }
                break;
            }

            case XC_FACTORY_ONLINE_KEY:
            {
                if (ValueLength == sizeof(pfs->OnlineKey))
                {
                    memcpy(Value, pfs->OnlineKey, ValueLength);
                    *Type = REG_BINARY;
                    *ResultLength = ValueLength;
                    status = STATUS_SUCCESS;
                }
                break;
            }
        }

        AssertSz1(status == STATUS_SUCCESS, "HalExQueryNonVolatileSetting: Unrecognized ValueIndex (%d)", ValueIndex);
    }

    return(STATUS_SUCCESS);
}

NTSTATUS HalExSaveNonVolatileSetting(char * pszXbox, ULONG ValueIndex, ULONG Type, const VOID * Value, ULONG ValueLength)
{
    EEPROM_LAYOUT   eeprom;
    NTSTATUS        status = STATUS_UNSUCCESSFUL;

    if (HalLoadEEPROM(pszXbox, &eeprom))
    {
        XBOX_FACTORY_SETTINGS * pfs = (XBOX_FACTORY_SETTINGS *)&eeprom.FactorySection;

        switch (ValueIndex)
        {
            case XC_FACTORY_ETHERNET_ADDR:
            {
                if (Type == REG_BINARY && ValueLength == sizeof(pfs->EthernetAddr))
                {
                    memcpy(pfs->EthernetAddr, Value, ValueLength);
                    status = STATUS_SUCCESS;
                }
                break;
            }

            case XC_FACTORY_SERIAL_NUMBER:
            {
                if (Type == REG_BINARY && ValueLength == sizeof(pfs->SerialNumber))
                {
                    memcpy(pfs->SerialNumber, Value, ValueLength);
                    status = STATUS_SUCCESS;
                }
                break;
            }

            case XC_FACTORY_ONLINE_KEY:
            {
                if (Type == REG_BINARY && ValueLength == sizeof(pfs->OnlineKey))
                {
                    memcpy(pfs->OnlineKey, Value, ValueLength);
                    status = STATUS_SUCCESS;
                }
                break;
            }
        }

        AssertSz2(status == STATUS_SUCCESS, "HalExSaveNonVolatileSetting: Unrecognized ValueIndex (%d) Type (%d)", ValueIndex, Type);
    }

    if (status == STATUS_SUCCESS && !HalSaveEEPROM(pszXbox, &eeprom))
    {
        status = STATUS_UNSUCCESSFUL;
    }

    return(status);
}

// ---------------------------------------------------------------------------------------

#endif
