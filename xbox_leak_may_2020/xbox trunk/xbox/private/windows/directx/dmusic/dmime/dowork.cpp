// Copyright (c) 1998-1999 Microsoft Corporation
// DoWork.cpp 

#include "pchime.h"

CBossMan g_BossMan;
static BOOL g_fUseThreads = TRUE;
static DWORD dwWorkerCount = 0;
static DWORD DoWorkThreadProc(void* pvhWndMain);
static void WaitForThreadEnd(HANDLE *phThread);
static HANDLE g_hDoWorkThread= NULL;
static HANDLE g_hDoWorkQuitEvent = NULL;
static HANDLE g_hDoWorkWakeupEvent = NULL;

void SetDoWorkThreadMode(DWORD dwOff)
{
    if(g_hDoWorkThread && dwOff) 
    {
        Trace(1,"Error: InitAudio() called with different thread flags from previous call. \n");
        dwOff = false;
    }
    g_fUseThreads = !dwOff;
}

/********************************************************************************
********************************************************************************/
static HRESULT StartDoWorkThreadProc(void)
{
    DWORD dwTemp = 0;
    g_hDoWorkQuitEvent = CreateEvent(NULL, TRUE, 0, 0);
    if (!g_hDoWorkQuitEvent)
    {
        return E_FAIL;        
    }
    g_hDoWorkWakeupEvent = CreateEvent(NULL, FALSE, 0, 0); // FALSE for second arg means automatically reset event after worker thread wakes up
    if (!g_hDoWorkWakeupEvent)
    {
        CloseHandle(g_hDoWorkQuitEvent);
        g_hDoWorkQuitEvent = NULL;
        return E_FAIL;        
    }
    g_hDoWorkThread =    CreateThread(NULL,20000,(LPTHREAD_START_ROUTINE)DoWorkThreadProc,(void*)dwTemp,0,NULL);
    if (!g_hDoWorkThread)
    {
        CloseHandle(g_hDoWorkQuitEvent);
        CloseHandle(g_hDoWorkWakeupEvent);
        g_hDoWorkQuitEvent = NULL;
        g_hDoWorkWakeupEvent = NULL;
        return E_FAIL;        
    }
    SetThreadPriority( g_hDoWorkThread, THREAD_PRIORITY_TIME_CRITICAL); 
    return S_OK;
};


/********************************************************************************
********************************************************************************/
static DWORD DoWorkThreadProc(void* pvhWndMain)
{
    DWORD dwQuantum = 30;
    HANDLE handles[2];
    handles[0] = g_hDoWorkQuitEvent;
    handles[1] = g_hDoWorkWakeupEvent;

    while (true){
        DWORD result = WaitForMultipleObjects(2, handles, FALSE, dwQuantum);
        if ( result == WAIT_OBJECT_0 ) {
            break; // asked to quit
        }
        else if ( result == WAIT_OBJECT_0 + 1 ) {
            // Asked to run quickly
            g_BossMan.DoWork(0);
        }
        else {
            // Normal quantum timeout
            g_BossMan.DoWork(dwQuantum);
        }
    }

	CloseHandle(g_hDoWorkQuitEvent);
	CloseHandle(g_hDoWorkWakeupEvent);

    g_hDoWorkThread= NULL;
    g_hDoWorkQuitEvent = NULL;
    g_hDoWorkWakeupEvent = NULL;

    return 0;
} 



/********************************************************************************
********************************************************************************/
static void WaitForThreadEnd(HANDLE *phThread)
{
    DWORD dwWaitResult = WAIT_TIMEOUT;

    if(*phThread)
    {
        dwWaitResult = WAIT_TIMEOUT;
        while(dwWaitResult != WAIT_OBJECT_0)
        {
            dwWaitResult = WaitForSingleObject(*phThread, 0);
            Sleep(100);
        }

        CloseHandle(*phThread);
        *phThread = NULL;
    }
}

CRITICAL_SECTION gDirectMusicDoWorkCriticalSection;
CRITICAL_SECTION gDirectMusicPinkSlipCriticalSection;

void InitializeDirectMusicDoWorkCriticalSection(){
    InitializeCriticalSection(&gDirectMusicDoWorkCriticalSection);
    InitializeCriticalSection(&gDirectMusicPinkSlipCriticalSection);
}

#ifdef DBG
void DirectMusicInitializedCheck();
#endif

void WINAPI DirectMusicDoWork(DWORD dwQuantum)

{
#ifdef DBG
    DirectMusicInitializedCheck();
#endif
    if (!g_fUseThreads)
    {
        g_BossMan.DoWork(dwQuantum);
    }
}

void CWorker::PinkSlip()
{
    ENTER_CRITICAL_SECTION(&gDirectMusicPinkSlipCriticalSection);
    m_bToldToDie = true;
    LEAVE_CRITICAL_SECTION(&gDirectMusicPinkSlipCriticalSection);
}

bool CWorker::HasBeenToldToDie()
{
    bool result;
    ENTER_CRITICAL_SECTION(&gDirectMusicPinkSlipCriticalSection);
    result = m_bToldToDie;
    LEAVE_CRITICAL_SECTION(&gDirectMusicPinkSlipCriticalSection);
    return result;
}

void CWorker::DoPinkSlip()
{
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    DMusicUnregisterPerformanceCounter(m_szName);
#endif
}

CBossMan::~CBossMan()
{
    if (m_pMasterClock)
    {
        m_pMasterClock->Release();
    }
}

void CBossMan::SetMasterClock(IReferenceClock *pClock)
{
    ENTER_CRITICAL_SECTION(&gDirectMusicDoWorkCriticalSection);
    if (m_pMasterClock)
    {
        m_pMasterClock->Release();
    }
    m_pMasterClock = pClock;
    if (m_pMasterClock)
    {
        m_pMasterClock->AddRef();
    }
    LEAVE_CRITICAL_SECTION(&gDirectMusicDoWorkCriticalSection);
}

CWorker * CBossMan::InstallWorker(PWORKER_ROUTINE pRoutine,void * pContext,DWORD dwOrder,char *pszName)
{
    ENTER_CRITICAL_SECTION(&gDirectMusicDoWorkCriticalSection);
    CWorker *pWorker = new CWorker;
    if (pWorker)
    {
        pWorker->m_bToldToDie = false;
        pWorker->m_pWorkRoutine = pRoutine;
        pWorker->m_pContext = pContext;
        pWorker->m_rtWakeUpTime = 0;
        pWorker->m_pBossMan = this;
        pWorker->m_dwOrder = dwOrder;
        CWorker *pScan = GetHead();
        for (;pScan;pScan = pScan->GetNext())
        {
            if (pScan->m_dwOrder > dwOrder)
            {
                break;
            }
        }
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
        strcpy (pWorker->m_szName,pszName);
        DMusicRegisterPerformanceCounter(pWorker->m_szName,
                                      DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                      &pWorker->m_dwElapsedTime );

#endif
        if (IsEmpty() && g_fUseThreads)
        {
            StartDoWorkThreadProc();
        }
        InsertBefore(pScan,pWorker);
    }
    LEAVE_CRITICAL_SECTION(&gDirectMusicDoWorkCriticalSection);
    return pWorker;
}

static void GetPentiumTime(REFERENCE_TIME *prtTime)

{
    _int64 cycNow;

    _asm
    {
        RDTSC                       // Get the time in EDX:EAX
        mov     dword ptr [cycNow], eax
        mov     dword ptr [cycNow+4], edx
    }

    cycNow *= 10;          // Gives greater clock granularity.
    cycNow /= 733; // m_dwDivisor;
    *prtTime = cycNow;
}

static REFERENCE_TIME rtWakeUp = 0;
static REFERENCE_TIME rtGoToSleep = 0;

void CBossMan::DoWork(DWORD dwQuantum)
{
//    GetPentiumTime(&rtWakeUp);
    ENTER_CRITICAL_SECTION(&gDirectMusicDoWorkCriticalSection);
    dwQuantum *= 10000;     // Convert from milliseconds to REFERENCE_TIME units.
    if (m_pMasterClock)
    {
        REFERENCE_TIME rtTime;
        m_pMasterClock->GetTime(&rtTime);   // Get current time.
        rtTime += (dwQuantum * 2);                // Find out when everything needs to be done by.
//        DWORD fAgain = TRUE;                 // This is used to track whether we need another pass through. 
//DbgPrint("%ld: ",(long) (rtTime / 10000));
        for (DWORD dwTries = 0; dwTries < 2; dwTries++)
        {
//            fAgain = FALSE;
            // Get rid of any workers who have been told to die
            {
                CWorker *pWorker = GetHead();
                while(pWorker)
                {
                    CWorker* pNext = pWorker->GetNext();
                    if(pWorker->HasBeenToldToDie())
                    {
                        pWorker->DoPinkSlip();
                        Remove(pWorker);

                        // If we're out of workers, we can shut down.
                        bool fStopThread = false;
                        if (IsEmpty() && g_fUseThreads)
                        {
                            fStopThread = true;
                        }
                        if (fStopThread)
                        {
                            SetEvent(g_hDoWorkQuitEvent);
                        }

                        delete pWorker;
                    }
                    pWorker = pNext;
                }
            }

            CWorker *pWorker = GetHead();
            for (;pWorker;pWorker = pWorker->GetNext())
            {
//                DbgPrint("%lx, %ld: \t",pWorker,(long)(pWorker->m_rtWakeUpTime / 10000));
//                if (pWorker->m_rtWakeUpTime <= rtTime)
                {
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
                    REFERENCE_TIME rtStart;
                    GetPentiumTime(&rtStart);
                    pWorker->m_pWorkRoutine(pWorker->m_pContext,&pWorker->m_rtWakeUpTime);
                    REFERENCE_TIME rtEnd;
                    GetPentiumTime(&rtEnd);
                    pWorker->m_rtElapsedTime += (rtEnd - rtStart);
#else
                    pWorker->m_pWorkRoutine(pWorker->m_pContext,&pWorker->m_rtWakeUpTime);
#endif
//                    fAgain = TRUE;
                }
            }
        }
    }
//    REFERENCE_TIME rtLastSleep = rtGoToSleep;
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    GetPentiumTime(&rtWakeUp);
    if ((rtWakeUp - rtGoToSleep) > 10000 * 1000)
    {
//        DbgPrint("Interval: %ld: ",(long) ((rtWakeUp - rtGoToSleep) / 10000));
        CWorker *pWorker = GetHead();
        for (;pWorker;pWorker = pWorker->GetNext())
        {
            pWorker->m_dwElapsedTime = (DWORD) (pWorker->m_rtElapsedTime / 10000);
//            DbgPrint("%ld: %ld \t",pWorker->m_dwOrder,(long)(pWorker->m_rtElapsedTime / 100));
            pWorker->m_rtElapsedTime = 0;
        }
//        DbgPrint("\n");
        rtGoToSleep = rtWakeUp;
    }
#endif
    LEAVE_CRITICAL_SECTION(&gDirectMusicDoWorkCriticalSection);
//    DbgPrint("Slept: %ld, Worked: %ld, Percent: %ld\n",
//        (long) (rtWakeUp - rtLastSleep), (long) (rtGoToSleep - rtWakeUp),
//        (long) (100 * (rtGoToSleep - rtWakeUp) / (rtGoToSleep - rtLastSleep)));

}



void CBossMan::WakeUp(CWorker* pWorker){
    if(g_fUseThreads){
        // We could deadlock if we actually tried to run worker
        pWorker->WakeUp(); // Run me right away
        SetEvent(g_hDoWorkWakeupEvent);
    }
    else
    {
        // No worker thread, just call worker now.
        REFERENCE_TIME rt;
        pWorker->m_pWorkRoutine(pWorker->m_pContext,&rt);
    }
}
