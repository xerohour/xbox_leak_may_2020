/*++

Copyright (c) 1996,2000  Microsoft Corporation

Module Name:

    TIMER.C

Abstract:

	Handle adjusting timer resolution for throttling and do thread pool

Author:

	Aaron Ogus (aarono)

Environment:

	Win32

Revision History:

	Date   	Author  Description
   ======  	======  ============================================================
   6/04/98 	aarono  Original
   10/13/99	a-evsch	Adding support for standard long-timers
   8/16/00  RichGr  Removed InitCount global as InitTimerWorkaround() is now guaranteed
                    to be called only once, triggered by DllMain.
   8/20/00  aarono  More intelligent multiproc timer management for dnet
   9/30/00  RichGr  Bug #46064: Track timer thread handles in an array for clean shutdown. 

--*/

/*
**		Quick Insert Optimizers
**
**		In a very high user system there are going to be many timers being set and cancelled.  Timer
**	cancels and timer fires are already optimized,  but as the timer list grows the SetTimer operations
**	become higher and higher overhead as we walk through a longer and longer chain for our insertion-sort.
**
**		Front First for Short Timers
**
**		When very short timers are being set we can assume that they will insert towards the front of the
**	timer list.  So it would be smarter to walk the list front-to-back instead of the back-to-front default
**	behavior which correctly assumes that most new timers will be firing after timers already set.  If the
**	the Timeout value of a new timer is near the current timer resolution then we will try the front-first
**	insertion-sort instead.  This will hopefully reduce short timer sets to fairly quick operations
**
**		Standard Long Timers
**
**		Standard means that they will all have the same duration.  If we keep a seperate chain
**	for all these timers with a constant duration they can be trivally inserted at the end of the chain.  This
**	will be used for the periodic background checks run on each endpoint every couple of seconds.
**
**		Quick Set Timer Array
**
**		The really big optimization is an array of timeout lists, with a current pointer.  Periodic timeout
**	will walk the array a number of slots corresponding to the interval since it was last run.  All events
**	on those lists will be scheduled.  This turns all SetTimer ops into constant time operations
**	no matter how many timers are running in the system.  This can be used for all timers within the
**	range of the array (resolution X number of slots) which may be 4ms * 256 slots or a 1K ms range.  We expect
**	most link timers to fall into this range, although it can be doubled or quadrupled quite trivially.
**
**	I plan to run QST algorithm on any server platform,  which will replace Front First Short Timers for
**	obvious reasons.  Client or Peer servers will use FFS instead.  Both configs will benefit from StdLTs
**	unless the range of the QST array grows to encompass the standard length timeout.
*/

#include "dnproti.h"


#define DEFAULT_TIME_RESOLUTION 20	/* ms */
#ifndef DPLAY_DOWORK
#define MIN_TIMER_THREADS		1
#define MAX_TIMER_THREADS_PER_PROCESSOR 8
#endif

VOID QueueTimeout(PMYTIMER pTimer);
#ifndef DPLAY_DOWORK
DWORD WINAPI TimerWorkerThread(LPVOID foo);
#endif


// Timer Resolution adjustments;
DWORD dwOldPeriod=DEFAULT_TIME_RESOLUTION;
DWORD dwCurrentPeriod=DEFAULT_TIME_RESOLUTION;
DWORD dwPeriodInUse=DEFAULT_TIME_RESOLUTION;

//	EJS - There are now two timer lists.  The original random-access timer list (MyTimerList) and the
//  new StdTimerList.  On each PeriodicTimeout both lists will now be checked for timer expirations.

CBilink MyTimerList;			// Random Timer List
CBilink StdTimerList;			// Standard Length Timer List
CRITICAL_SECTION MyTimerListLock;							// One lock will guard both lists

LPFPOOL pTimerPool=NULL;
#ifndef DPLAY_DOWORK
DWORD uWorkaroundTimerID;
#endif

LONG g_Running = 0;		// is the timer system up and running

DWORD Unique=0;


CRITICAL_SECTION ThreadListLock;		// locks ALL this stuff.

CBilink ThreadList;	// ThreadPool grabs work from here.

DWORD nThreads=0;		// number of running threads.
DWORD ActiveReq=0;		// number of requests being processed.
DWORD PeakReqs=0;
DWORD bShutDown=FALSE;
#ifndef DPLAY_DOWORK
DWORD KillCount=0;
#endif
DWORD ExtraSignals=0;
#ifdef	DEBUG
DWORD MaxPeakReqs = 0;
#endif

#ifndef DPLAY_DOWORK
HANDLE hWorkToDoSem;

#endif

PVOID uAdjustResTimer=0;
UINT AdjustResUnique=0;

PVOID uAdjustThreadsTimer=0;
UINT AdjustThreadsUnique=0;

#ifndef DPLAY_DOWORK
DWORD   MAX_TIMER_THREADS = MAX_TIMER_THREADS_PER_PROCESSOR;

//  9/30/00(RichGr) - Bug #46064: Track thread handles in an array so we have an accurate count of how many need
//     to be closed at shutdown.
HANDLE *g_phTimerThreadHandles = NULL;

#endif

/***
*
*	QUICK-START TIMER SUPPORT
*
***/

#define	QST_SLOTCOUNT		2048					// 2048 seperate timer queues
#define	QST_GRANULARITY		4						// 4 ms clock granularity * 2048 slots == 8192 ms max timeout value
#define	QST_MAX_TIMEOUT		(QST_SLOTCOUNT * QST_GRANULARITY)
#define	QST_MOD_MASK		(QST_SLOTCOUNT - 1)		// Calculate a quick modulo operation for wrapping around the array

#if	( (QST_GRANULARITY - 1) & QST_GRANULARITY )
This Will Not Compile -- ASSERT that QST_GRANULARITY is power of 2!
#endif
#if	( (QST_SLOTCOUNT - 1) & QST_SLOTCOUNT )
This Will Not Compile -- ASSERT that QST_SLOTCOUNT is power of 2!
#endif

#define	QST

#ifdef	QST
CBilink	*rgQSTimerArray = NULL;

UINT	QST_CurrentIndex;				// Last array slot that was executed
DWORD	QST_LastRunTime;				// Tick count when QSTs last ran
#endif

/*
*	END OF QST SUPPORT
*/


#undef	Lock
#undef	Unlock
#define	Lock	EnterCriticalSection
#define	Unlock	LeaveCriticalSection

/*
**		Periodic Timer
**
**		This runs every RESOLUTION millisecs and checks for expired timers. It must check two lists
**	for expired timers,  plus a variable number of slots in the QST array.
*/
#undef DPF_MODNAME
#define DPF_MODNAME "PeriodicTimer"

#ifndef DPLAY_DOWORK
void CALLBACK PeriodicTimer (UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
#else
void PeriodicTimer (void)
#endif
{
	DWORD  		time;
	PMYTIMER  	pTimerWalker;
	CBilink 		*pBilink;
	DWORD 		dwReleaseCount=0;
#ifdef	QST
	INT 		interval;
	DWORD		slot_count;
#endif

	if(g_Running == FALSE)
		return;
		
	Lock(&MyTimerListLock);
	Lock(&ThreadListLock);

	if(g_Running == FALSE){
		Unlock(&ThreadListLock);
		Unlock(&MyTimerListLock);
		return;
	}
		
	time=timeGetTime()+(dwCurrentPeriod/2);

	//   service QST lists:  Calculate how many array slots have expired and
	// service any timers in those slots.

#ifdef	QST

	interval = (INT) (time - QST_LastRunTime);

	if( (interval) > 0){

		slot_count = ((DWORD) interval) / QST_GRANULARITY;
		slot_count = MIN(slot_count, QST_SLOTCOUNT);

		if(slot_count < QST_SLOTCOUNT){
			QST_LastRunTime += (slot_count * QST_GRANULARITY);
		}
		else{
			// If there was a LONG delay in scheduling this, (longer then the range of the whole array)
			// then we must complete everything that is on the array and then re-synchronize the times

			slot_count = QST_SLOTCOUNT;
			QST_LastRunTime = time;
		}

		while(slot_count--){

			while( (pBilink = rgQSTimerArray[QST_CurrentIndex].GetNext()) != &rgQSTimerArray[QST_CurrentIndex] ){
				pTimerWalker=CONTAINING_RECORD(pBilink, MYTIMER, Bilink);
				pBilink->RemoveFromList();

				pTimerWalker->Bilink.InsertBefore( &ThreadList);
				pTimerWalker->TimerState=QueuedForThread;
				dwReleaseCount++;
			}
			QST_CurrentIndex = (QST_CurrentIndex + 1) & QST_MOD_MASK;
		}

	}
#endif

	//	Walk the sorted timer list.  Expired timers will all be at the front of the
	// list so we can stop checking as soon as we find any un-expired timer.

	pBilink=MyTimerList.GetNext();

	while(pBilink!=&MyTimerList){

		pTimerWalker=CONTAINING_RECORD(pBilink, MYTIMER, Bilink);
		pBilink=pBilink->GetNext();

		if(((INT)(time-pTimerWalker->TimeOut) > 0)){
			pTimerWalker->Bilink.RemoveFromList();
			pTimerWalker->Bilink.InsertBefore( &ThreadList);
			pTimerWalker->TimerState=QueuedForThread;
			dwReleaseCount++;
		} else {
			break;
		}

	}

	//  Next walk the Standard Length list.   Same rules apply

	pBilink=StdTimerList.GetNext();
	while(pBilink != &StdTimerList){
		pTimerWalker=CONTAINING_RECORD(pBilink, MYTIMER, Bilink);
		pBilink=pBilink->GetNext();

		if(((INT)(time-pTimerWalker->TimeOut) > 0)){
			pTimerWalker->Bilink.RemoveFromList();
			pTimerWalker->Bilink.InsertBefore( &ThreadList);
			pTimerWalker->TimerState=QueuedForThread;
			dwReleaseCount++;
		} else {
			break;
		}
	}

	ActiveReq += dwReleaseCount;
	if(ActiveReq > PeakReqs){
		PeakReqs=ActiveReq;
	}

#ifndef DPLAY_DOWORK
	ReleaseSemaphore(hWorkToDoSem,dwReleaseCount,NULL);
#endif

	Unlock(&ThreadListLock);
	Unlock(&MyTimerListLock);

}

#define min(a,b)            (((a) < (b)) ? (a) : (b))

#undef DPF_MODNAME
#define DPF_MODNAME "AdjustTimerResolution"

VOID CALLBACK AdjustTimerResolution(PVOID uID, UINT uMsg, PVOID dwUser)
{
	DWORD dwWantPeriod;

	if (!g_Running)
		return;

	Lock(&MyTimerListLock);

	if (!g_Running){
		Unlock(&MyTimerListLock);
		return;
	}

	dwWantPeriod=min(dwCurrentPeriod,dwOldPeriod);
	dwOldPeriod=dwCurrentPeriod;
	dwCurrentPeriod=DEFAULT_TIME_RESOLUTION;

	if(dwPeriodInUse != dwWantPeriod){
		dwPeriodInUse=dwWantPeriod;
#ifndef DPLAY_DOWORK
		timeKillEvent(uWorkaroundTimerID);
		LOGPF(9, "PERIODIC TIMER ADJUSTED TO %dms", dwPeriodInUse);
		uWorkaroundTimerID=timeSetEvent(dwPeriodInUse, dwPeriodInUse, PeriodicTimer, 0, TIME_PERIODIC);
#endif
	}

	Unlock(&MyTimerListLock);

	SetMyTimer(1000,100,AdjustTimerResolution,0,&uAdjustResTimer,&AdjustResUnique);
}

#undef DPF_MODNAME
#define DPF_MODNAME "AdjustThreads"

VOID CALLBACK AdjustThreads(PVOID uID, UINT uMsg, PVOID dwUser)
{
	Lock(&ThreadListLock);
#ifndef DPLAY_DOWORK
	if((PeakReqs < nThreads) && nThreads){
		KillCount=nThreads-PeakReqs;
		ReleaseSemaphore(hWorkToDoSem, KillCount, NULL);
	}
#endif
#ifdef	DEBUG
	if(PeakReqs > MaxPeakReqs){
		MaxPeakReqs = PeakReqs;
	}
#endif
	PeakReqs=0;
	Unlock(&ThreadListLock);

	SetMyTimer(60000,500,AdjustThreads,0,&uAdjustThreadsTimer,&AdjustThreadsUnique);
}

#undef DPF_MODNAME
#define DPF_MODNAME "SetTimerResolution"

VOID SetTimerResolution(UINT msResolution)
{
	if (!g_Running)
		return;

	Lock(&MyTimerListLock);

	if (!g_Running){
		Unlock(&MyTimerListLock);
		return;
	}

	if(msResolution < dwPeriodInUse){
		dwCurrentPeriod = dwPeriodInUse=msResolution;
#ifndef DPLAY_DOWORK
		timeKillEvent(uWorkaroundTimerID);
		LOGPF(9, "SETTING TIMER RESOLUTION TO %dms", dwPeriodInUse);
		uWorkaroundTimerID=timeSetEvent(dwPeriodInUse, dwPeriodInUse, PeriodicTimer, 0, TIME_PERIODIC);
#endif
	}
	else if(msResolution < dwCurrentPeriod){
		dwCurrentPeriod=msResolution;
	}

	Unlock(&MyTimerListLock);
}

#undef DPF_MODNAME
#define DPF_MODNAME "ScheduleTimerThread"

VOID ScheduleTimerThread(MYTIMERCALLBACK TimerCallBack, PVOID UserContext, PVOID *pHandle, PUINT pUnique)
{
	PMYTIMER pTimer;

	if (!g_Running){
		*pHandle = 0;
		*pUnique = 0;
		return;
	}

	Lock(&MyTimerListLock);
	Lock(&ThreadListLock);

	if( (!g_Running) || ((pTimer = static_cast<PMYTIMER>( pTimerPool->Get(pTimerPool) ))== NULL)){
		Unlock(&ThreadListLock);
		Unlock(&MyTimerListLock);
		*pHandle = 0;
		*pUnique = 0;
		return;
	}

	pTimer->CallBack = TimerCallBack;
	pTimer->Context = UserContext;

	*pUnique = ++Unique;
	if(Unique == 0){
		*pUnique = ++Unique;
	}
	
	*pHandle = pTimer;
	pTimer->Unique = *pUnique;

	
	pTimer->Bilink.InsertBefore( &ThreadList);
	pTimer->TimerState = QueuedForThread;

	ActiveReq++;
	if(ActiveReq > PeakReqs){
		PeakReqs = ActiveReq;
	}
	
#ifndef DPLAY_DOWORK	
	ReleaseSemaphore(hWorkToDoSem,1,NULL);
#endif

	Unlock(&ThreadListLock);
	Unlock(&MyTimerListLock);
}

#undef DPF_MODNAME
#define DPF_MODNAME "SetMyTimer"

VOID SetMyTimer(DWORD dwTimeOut, DWORD TimerRes, MYTIMERCALLBACK TimerCallBack, PVOID UserContext, PVOID *pHandle, PUINT pUnique)
{
	CBilink *pBilink;
	PMYTIMER pMyTimerWalker,pTimer;
	DWORD time;
	BOOL bInserted=FALSE;
#ifdef	QST
	UINT	Offset;
	UINT	Index;
#endif

	if( (g_Running == FALSE) || ((pTimer = static_cast<PMYTIMER>( pTimerPool->Get(pTimerPool) ))== NULL)){
		*pHandle = 0;
		*pUnique = 0;
		return;
	}

	pTimer->CallBack=TimerCallBack;
	pTimer->Context=UserContext;

	if(TimerRes){
		SetTimerResolution(TimerRes);
	}

	Lock(&MyTimerListLock);

		++Unique;
		if(Unique==0){
			++Unique;
		}
		*pUnique=Unique;

		pTimer->Unique=Unique;
		*pHandle = (PVOID) pTimer;

		time=GETTIMESTAMP();
		pTimer->TimeOut=time+dwTimeOut;
		pTimer->TimerState=WaitingForTimeout;

#ifdef	QST
		if(dwTimeOut < QST_MAX_TIMEOUT){
			Offset = (dwTimeOut + (QST_GRANULARITY/2)) / QST_GRANULARITY;	// Round nearest and convert time to slot offset
			Index = (Offset + QST_CurrentIndex) & QST_MOD_MASK;				// Our index will be Current + Offset MOD TableSize
			pTimer->Bilink.InsertBefore( &rgQSTimerArray[Index]);			// Its called Quick-Start for a reason.
			// 7/28/2000(a-JiTay): IA64: Use %p format specifier for 32/64-bit pointers, addresses, and handles.
			LOGPF(9, "SET TIMER QST handle=%p; Bucket=%d; CurrentBucket=%d",pTimer, Index, QST_CurrentIndex);
		}
		else
#endif	QST

		// OPTIMIZE FOR STANDARD TIMER
		//
		// Rather then calling a special API for StandardLongTimers as described above,  we can just pull out
		// any timer with the correct Timeout value and stick it on the end of the StandardTimerList.  I believe
		// this is the most straightforward way to do it.  Now really,  we could put anything with a TO +/- resolution
		// on the standard list too,  but that might not be all that useful...

		if(dwTimeOut == STANDARD_LONG_TIMEOUT_VALUE){
			// This is a STANDARD TIMEOUT so add it to the end of the standard list.

			pTimer->Bilink.InsertBefore( &StdTimerList);
		}

		// OPTIMIZE FOR SHORT TIMERS  !! DONT NEED TO DO THIS IF USING Quick Start Timers !!
		//
		// If the timer has a very small Timeout value (~20ms) lets insert from the head of the list
		// instead of from the tail.
#ifndef	QST		
		else if (dwTimeOut < 25){
			pBilink=MyTimerList.GetNext();
			while(pBilink != &MyTimerList){
				pMyTimerWalker=CONTAINING_RECORD(pBilink, MYTIMER, Bilink);
				pBilink=pBilink->GetNext();

				if((int)(pMyTimerWalker->TimeOut-pTimer->TimeOut) > 0 ){
					pTimer->Bilink.InsertBefore( &pMyTimerWalker->Bilink);
					bInserted=TRUE;
					break;
				}
			}

			if(!bInserted){
				pTimer->Bilink.InsertBefore( &MyTimerList);
			}
		}
#endif	// ! QST
		else
		{

		//	DEFAULT - Assume new timers will likely sort to the end of the list.
		//
		// Insert this guy in the sorted list by timeout time,  walking from the tail forward.

			pBilink=MyTimerList.GetPrev();
			while(pBilink != &MyTimerList){
				pMyTimerWalker=CONTAINING_RECORD(pBilink, MYTIMER, Bilink);
				pBilink=pBilink->GetPrev();

				if((int)(pTimer->TimeOut-pMyTimerWalker->TimeOut) > 0 ){
					pTimer->Bilink.InsertAfter( &pMyTimerWalker->Bilink);
					bInserted=TRUE;
					break;
				}
			}

			if(!bInserted){
				pTimer->Bilink.InsertAfter( &MyTimerList);
			}
		}	
	Unlock(&MyTimerListLock);

	return;
}


#undef DPF_MODNAME
#define DPF_MODNAME "CancelMyTimer"

HRESULT CancelMyTimer(PVOID dwTimer, DWORD Unique)
{
	PMYTIMER pTimer=(PMYTIMER)dwTimer;
	HRESULT hr=DPNERR_GENERIC;

	if(pTimer == 0){
		return DPN_OK;
	}

	Lock(&MyTimerListLock);
	Lock(&ThreadListLock);

	if(pTimer->Unique == Unique){
		switch(pTimer->TimerState){
			case WaitingForTimeout:
				pTimer->Bilink.RemoveFromList();
				pTimer->TimerState=End;
				pTimer->Unique=0;
				pTimerPool->Release(pTimerPool, pTimer);
				hr=DPN_OK;
				break;

			case QueuedForThread:
				pTimer->Bilink.RemoveFromList();
				pTimer->TimerState=End;
				pTimer->Unique=0;
				pTimerPool->Release(pTimerPool, pTimer);
				if(ActiveReq)ActiveReq--;
				ExtraSignals++;
				hr=DPN_OK;
				break;

			default:
				break;
		}
	}

	Unlock(&ThreadListLock);
	Unlock(&MyTimerListLock);
	return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "InitQST"

HRESULT InitQST(void)
{
#ifdef	QST
	int		i;

	if( SALLOC(rgQSTimerArray, CBilink, QST_SLOTCOUNT) == NULL ){	// Allocate the timer table
		return DPNERR_OUTOFMEMORY;
	}

	for(i=0; i<QST_SLOTCOUNT; i++){
		rgQSTimerArray[i].Initialize();
	}

	QST_CurrentIndex = 0;
	QST_LastRunTime = GETTIMESTAMP();
#endif
	return DPN_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "InitTimerWorkaround"

HRESULT InitTimerWorkaround()
{
	DWORD   dwJunk;


    pTimerPool = NULL;
    nThreads = 0;		// number of running threads.
    ActiveReq = 0;		// number of requests being processed.
    PeakReqs = 0;
    bShutDown = FALSE;
#ifndef DPLAY_DOWORK
    KillCount = 0;
#endif
	ExtraSignals = 0;

#ifndef DPLAY_DOWORK
	MAX_TIMER_THREADS = MAX_TIMER_THREADS_PER_PROCESSOR;

    //  9/30/00(RichGr) - Bug #46064: Track thread handles in an array so we have an accurate count of how many need
    //     to be closed at shutdown.
    if ( (g_phTimerThreadHandles = new HANDLE[MAX_TIMER_THREADS]) == NULL)
		return DPNERR_OUTOFMEMORY;

    memset(g_phTimerThreadHandles, 0, sizeof HANDLE * MAX_TIMER_THREADS);
#endif

	MyTimerList.Initialize();
	StdTimerList.Initialize();
	ThreadList.Initialize();

	if(InitQST() != DPN_OK){
		return DPNERR_OUTOFMEMORY;
	}

#ifndef DPLAY_DOWORK
	hWorkToDoSem = CreateSemaphoreA(NULL, 0, 65535, NULL);
#endif

	InitializeCriticalSection(&MyTimerListLock);
	InitializeCriticalSection(&ThreadListLock);

	pTimerPool=FPM_Create(sizeof(MYTIMER),NULL,NULL,NULL,NULL);


#ifndef DPLAY_DOWORK
	if(!hWorkToDoSem || !pTimerPool){
		FiniTimerWorkaround();
		return DPNERR_OUTOFMEMORY;
	}

	uWorkaroundTimerID=timeSetEvent(DEFAULT_TIME_RESOLUTION, DEFAULT_TIME_RESOLUTION, PeriodicTimer, 0, TIME_PERIODIC);

	if(!uWorkaroundTimerID){
#else
	if(!pTimerPool){
#endif
		FiniTimerWorkaround();
		return DPNERR_OUTOFMEMORY;
	}

    Lock(&ThreadListLock);
	nThreads = 1;
#ifndef DPLAY_DOWORK
    //  9/30/00(RichGr) - Bug #46064: Pass a pointer to the destination for the thread handle to TimerWorkerThread
    //     so the handle will be available to be closed on thread exit.
	g_phTimerThreadHandles[0] = CreateThread(NULL, 4096, TimerWorkerThread, &g_phTimerThreadHandles[0], 0, &dwJunk);
#endif
    Unlock(&ThreadListLock);

#ifndef DPLAY_DOWORK
	if( !g_phTimerThreadHandles[0])
    {
		nThreads = 0;
		FiniTimerWorkaround();
		return DPNERR_OUTOFMEMORY;
	}
#endif

	InterlockedIncrement(&g_Running);

	SetMyTimer(1000,500,AdjustTimerResolution,0,&uAdjustResTimer,&AdjustResUnique);
	SetMyTimer(60000,500,AdjustThreads,0,&uAdjustThreadsTimer,&AdjustThreadsUnique);


	return DPN_OK;

}

#undef DPF_MODNAME
#define DPF_MODNAME "PurgeTimerList"

VOID PurgeTimerList(CBilink *pList)
{
	PMYTIMER	pTimer;

	while(!pList->IsEmpty()){
		pTimer=CONTAINING_RECORD(pList->GetNext(), MYTIMER, Bilink);
		pTimer->Unique=0;
		pTimer->TimerState=End;
		pTimer->Bilink.RemoveFromList();
		pTimerPool->Release(pTimerPool, pTimer);
	}
}

#undef DPF_MODNAME
#define DPF_MODNAME "FiniQST"

void FiniQST(void)
{
#ifdef	QST
	UINT	index;
	CBilink	*pLink;

	if(rgQSTimerArray){

		pLink = rgQSTimerArray;
		for(index=0; index<QST_SLOTCOUNT; index++){
			PurgeTimerList(pLink++);
		}

		FREE(rgQSTimerArray);
		rgQSTimerArray = NULL;
	}
#endif
}

#undef DPF_MODNAME
#define DPF_MODNAME "FiniTimerWorkaround"

VOID FiniTimerWorkaround()
{
    DWORD   nThreadHandleCount;
    int     i;
    int     slowcount;
    DWORD   dwSuspendCount;


	// Set our running state to false
	EnterCriticalSection(&MyTimerListLock);
	g_Running = FALSE;
	LeaveCriticalSection(&MyTimerListLock);

	// At this point:
	// 1) AdjustTimerResolution will not reschedule the winmm timer
	// 2) SetTimerResolution will not reschedule the winmm timer
	// 3) PeriodicTimer will not do anything if it fires
	// 4) ScheduleTimerThread will not do anything if it fires
	// 5) Any thread waiting in PeriodicTimer will see Running == FALSE and won't do anything
	// 6) Any thread waiting in ScheduleTimerThread will see Running == FALSE and won't do anything

#ifndef DPLAY_DOWORK
	// Kill the timer so it never fires again
	if(uWorkaroundTimerID){
		timeKillEvent(uWorkaroundTimerID);
	}	
#endif
	
	// At this point:
	// 1) The winmm timer will not fire again and therefore PeriodicTimer will not be called again

	// Cancel the timers we started
	CancelMyTimer(uAdjustResTimer, AdjustResUnique);
	CancelMyTimer(uAdjustThreadsTimer, AdjustThreadsUnique);

	// Tell all remaining timer threads to shutdown
	Lock(&ThreadListLock);
	bShutDown = TRUE;
#ifndef DPLAY_DOWORK
	ReleaseSemaphore(hWorkToDoSem, 60000, NULL);
#endif
	Unlock(&ThreadListLock);
    Sleep(0);

	// At this point:
	// 1) No threads should be waiting in TimerWorkerThread and no new ones will be scheduled

#ifndef DPLAY_DOWORK
    if (nThreads > 0)
    {
        //  9/30/00(RichGr) - Bug #46064: Count the remaining threads and reset nThreads accordingly.
        nThreadHandleCount = 0;
    
        Lock(&ThreadListLock);

        for (i = 0; i < (int)MAX_TIMER_THREADS; i++)
        {
            if (g_phTimerThreadHandles[i])
            {
                // Use SuspendThread() to tell us whether the thread is still alive,
                // and if it's accessible then resume it.
                dwSuspendCount = SuspendThread(g_phTimerThreadHandles[i]);

                if (dwSuspendCount == (DWORD)-1)
                {
                    g_phTimerThreadHandles[i] = 0;
                }
                else
                {
                    dwSuspendCount = ResumeThread(g_phTimerThreadHandles[i]);
                    nThreadHandleCount++;
                }
            }
        }

        if (nThreadHandleCount < nThreads)  // We only care if we found less threads.
        {
            LOGPF(9, "Remaining thread count at cleanup is being reset from %d to %d", nThreads, nThreadHandleCount);
            nThreads = nThreadHandleCount;
        }

        Unlock(&ThreadListLock);

	    // Wait for the threads to finish
	    slowcount = 0;

	    while (nThreads && slowcount < (60000/100))	// don't wait more than 60 seconds.
        {
		    slowcount++;
		    Sleep(100);
	    }
    }

	CloseHandle(hWorkToDoSem);
#endif
	DeleteCriticalSection(&MyTimerListLock);
	DeleteCriticalSection(&ThreadListLock);

	PurgeTimerList(&MyTimerList);
	PurgeTimerList(&StdTimerList);
	PurgeTimerList(&ThreadList);

	FiniQST();

	if(pTimerPool){
		pTimerPool->Fini(pTimerPool);
	}

#ifndef DPLAY_DOWORK
    delete g_phTimerThreadHandles;
#endif
}


#undef DPF_MODNAME
#define DPF_MODNAME "TimerWorkerThread"

#ifndef DPLAY_DOWORK
DWORD WINAPI TimerWorkerThread(LPVOID lpv)  // lpv points to g_phTimerThreadHandles[i].
#else
DWORD TimerWorkerThread(VOID)
#endif
{
#ifndef DPLAY_DOWORK
    HANDLE     *phCurrentThreadHandle = (HANDLE*)lpv;
#endif
	CBilink    *pBilink;
	PMYTIMER    pTimer;
#ifndef DPLAY_DOWORK
	DWORD       dwJunk;
    int         i;


	while (1){

		WaitForSingleObject(hWorkToDoSem, INFINITE);

#endif
		Lock(&ThreadListLock);

			if(bShutDown){
				Unlock(&ThreadListLock);
#ifndef DPLAY_DOWORK
				break;	
			}

			if(KillCount && nThreads > 1){
				KillCount--;
				Unlock(&ThreadListLock);
				Sleep(1000); // stay around a sec to let I/O complete.
				break;	
#else
				return 1;	
#endif
			}

			if(ExtraSignals){
				ExtraSignals--;
				Unlock(&ThreadListLock);
#ifndef DPLAY_DOWORK
				continue;
			}

			if(KillCount){
				KillCount--; //keeps us from trying to continually evaluate killing last thread.
				Unlock(&ThreadListLock);
				continue;
			}

			if (ActiveReq > nThreads && nThreads < MAX_TIMER_THREADS)
            {
                //  9/30/00(RichGr) - Bug #46064: Find the first empty slot.
                for (i = 0; i < (int)MAX_TIMER_THREADS; i++)
                {
                    if (g_phTimerThreadHandles[i] == 0)
                        break;
                }

                // In case of an illogical error, just use the last slot anyway.  The cleanup code that
                // uses the array is failsafe.
                if (i >= (int)MAX_TIMER_THREADS)
                {
    			    DNASSERT("NO SLOTS LEFT IN TIMER THREAD HANDLE TABLE!");
                    i = MAX_TIMER_THREADS - 1;
                }

                // Pass a pointer to the destination for the thread handle to TimerWorkerThread
                // so the handle will be available to be closed on thread exit.
				g_phTimerThreadHandles[i] = CreateThread(NULL, 4096, TimerWorkerThread, &g_phTimerThreadHandles[i], 0, &dwJunk);

				if (g_phTimerThreadHandles[i])
                {
					nThreads++;
				}
#else
				return 0;
#endif
			}

			pBilink=ThreadList.GetNext();

			if(pBilink == &ThreadList) {
				Unlock(&ThreadListLock);
#ifndef DPLAY_DOWORK
				continue;
#else
				return 1;
#endif
			};

			pBilink->RemoveFromList();	// pull off the list.

			pTimer=CONTAINING_RECORD(pBilink, MYTIMER, Bilink);

			// Call a callback

			pTimer->TimerState=InCallBack;

		Unlock(&ThreadListLock);

		(pTimer->CallBack)(pTimer, (UINT) pTimer->Unique, pTimer->Context);

		pTimer->Unique=0;
		pTimer->TimerState=End;
		pTimerPool->Release(pTimerPool, pTimer);

		if (!g_Running)
#ifndef DPLAY_DOWORK
			break;
#else
			return 1;
#endif

		Lock(&ThreadListLock);

		if(ActiveReq)ActiveReq--;

		Unlock(&ThreadListLock);
#ifndef DPLAY_DOWORK
	}	


    //  9/30/00(RichGr) - Bug #46064: We certainly should have the handle.  Close it and set it to 0, which will
    //     update the thread handle array.
	Lock(&ThreadListLock);

    if (*phCurrentThreadHandle)
    {
		CloseHandle(*phCurrentThreadHandle);
        *phCurrentThreadHandle = 0;
    }

    // This used to be decremented earlier in this function, but that can give a misleading count
    // if we are cleaning up, so decrement it here.
	nThreads--;
	Unlock(&ThreadListLock);

    // Thread is terminating.
#endif
	return 0;
}



