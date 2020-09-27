/*
**		Direct Network Protocol
**
**		This file contains internal prototypes and global definitions.
*/


#ifndef	_DNINTERNAL_INCLUDED_
#define	_DNINTERNAL_INCLUDED_

//	Global Constants
#define		DNET_VERSION_NUMBER				0x00010000

#define		DELAYED_ACK_TIMEOUT				100			// Delay before sending dedicated ACK packet
#define		SHORT_DELAYED_ACK_TIMEOUT		20			// Delay before sending dedicated NACK packet
#define		DELAYED_SEND_TIMEOUT			40			// Delay before sending dedicated SEND_INFO packet

#define		CONNECT_DEFAULT_TIMEOUT		(200)		// At .1 we saw too many retries, users can set in SetCaps
#define		CONNECT_DEFAULT_RETRIES			8			// Lowering this from 10 to accomodate timeout change, users can set in SetCaps

#define		STANDARD_LONG_TIMEOUT_VALUE		30000
#define		DEFAULT_KEEPALIVE_INTERVAL		60000
#define		ENDPOINT_BACKGROUND_INTERVAL	STANDARD_LONG_TIMEOUT_VALUE		// this is really what its for...

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_PROTOCOL

typedef	void CALLBACK LPCB(UINT, UINT, DWORD, DWORD, DWORD);

//	Global Variable definitions

extern LPFPOOL	ChkPtPool;
extern LPFPOOL	EPDPool;
extern LPFPOOL	MSDPool;
extern LPFPOOL	FMDPool;
extern LPFPOOL	RCDPool;
extern LPFPOOL	BufPool;
extern LPFPOOL	MedBufPool;
extern LPFPOOL	BigBufPool;

//	Internal function prototypes

VOID			AbortSendsOnConnection(PEPD);
ULONG WINAPI 	BackgroundThread(PVOID);
BOOL			Buf_Allocate(PVOID);
VOID			Buf_Get(PVOID);
VOID			Buf_GetMed(PVOID);
VOID			Buf_GetBig(PVOID);
HRESULT			DoCancel(PMSD, HRESULT);
VOID 			CompleteConnect(PMSD, PSPD, PEPD, HRESULT);
VOID 			CompleteDatagramSend(PSPD, PMSD, HRESULT);
VOID			CompleteReliableSend(PSPD, PMSD, HRESULT);
VOID			CompleteSPConnect(PMSD, PSPD, HRESULT);
VOID CALLBACK	ConnectRetryTimeout(PVOID, UINT, PVOID);
VOID CALLBACK	DelayedAckTimeout(PVOID, UINT, PVOID);
VOID			DisconnectConnection(PEPD);
VOID			DropLink(PEPD);
VOID			ReleaseEndPoint(PEPD, BOOL);
PMSD			BuildDisconnectFrame(PEPD);
VOID			EndPointDroppedFrame(PEPD, DWORD);
VOID CALLBACK	EndPointBackgroundProcess(PVOID, UINT, PVOID);
VOID			EnqueueMessage(PMSD, PEPD);
BOOL			EPD_Allocate(PVOID);
VOID			EPD_Free(PVOID);
VOID			EPD_Get(PVOID);
VOID			EPD_Release(PVOID);
VOID 			FlushCheckPoints(PEPD);
BOOL			FMD_Allocate(PVOID);
VOID			FMD_Free(PVOID);
VOID			FMD_Get(PVOID);
VOID			FMD_Release(PVOID);
VOID 			InitLinkParameters(PEPD, UINT, UINT, UINT);
VOID			KillConnection(PEPD);
PCHKPT			LookupCheckPoint(PEPD, BYTE);
BOOL			MSD_Allocate(PVOID);
VOID			MSD_Free(PVOID);
VOID			MSD_Get(PVOID);
VOID			MSD_Release(PVOID);
PEPD			NewEndPoint(PSPD, HANDLE);
VOID			PerformCheckpoint(PEPD);
BOOL			RCD_Allocate(PVOID);
VOID			RCD_Free(PVOID);
VOID			RCD_Get(PVOID);
VOID			RCD_Release(PVOID);
VOID			ReceiveComplete(PEPD);
VOID			RejectConnection(PEPD);
VOID CALLBACK	RetryTimeout(PVOID, UINT, PVOID);
VOID CALLBACK	ScheduledSend(PVOID, UINT, PVOID);
VOID			SendAckFrame(PEPD, BOOL);
HRESULT			SendCommandFrame(PEPD, BYTE, BYTE);
VOID			SendDisconnectedFrame(PEPD);
ULONG WINAPI 	SendThread(PVOID);
VOID			ServiceCmdTraffic(PSPD);
VOID			ServiceEPD(PSPD, PEPD);
VOID CALLBACK	TimeoutConnect(PVOID, UINT, PVOID);
VOID CALLBACK	TimeoutSend(PVOID, UINT, PVOID);
VOID 			UpdateEndPoint(PEPD, UINT, UINT, UINT, DWORD);
VOID			UpdateXmitState(PEPD, BYTE, PSACKFRAME, ULONG, ULONG, DWORD);

#ifdef	DEBUG
VOID			KeepConnectStats(PEPD);
#endif

//	Internal Macro definitions

#define	MALLOC			DNMalloc
#define	FREE			DNFree
#define MEMCPY			memcpy
#define	MEMSET			memset
#define	GETTIMESTAMP()	timeGetTime()

#define	INITCRITSEC(P)		DNInitializeCriticalSection(P);DebugSetCriticalSectionRecursionCount(P,0)
#define	DELETECRITSEC(P)	DNDeleteCriticalSection(P)

#define	Lock(P)			DNEnterCriticalSection(P)
#define	Unlock(P)		DNLeaveCriticalSection(P)

#define	ASSERT_SPD(PTR)	ASSERT((PTR) != NULL); ASSERT((PTR)->Sign == SPD_SIGN)
#define	ASSERT_EPD(PTR)	ASSERT((PTR) != NULL); ASSERT((PTR)->Sign == EPD_SIGN)
#define	ASSERT_MSD(PTR)	ASSERT((PTR) != NULL); ASSERT((PTR)->Sign == MSD_SIGN)
#define	ASSERT_FMD(PTR)	ASSERT((PTR) != NULL); ASSERT((PTR)->Sign == FMD_SIGN)
#define	ASSERT_RCD(PTR)	ASSERT((PTR) != NULL); ASSERT((PTR)->Sign == RCD_SIGN)

#define	INTER_INC(PTR)	(InterlockedIncrement(reinterpret_cast<LONG*>( &((PTR)->uiRefCnt) )))
#define	INTER_DEC(PTR)	(InterlockedDecrement(reinterpret_cast<LONG*>( &((PTR)->uiRefCnt) )))

#ifdef DEBUG

BOOL	LockEPD(PEPD, PCHAR);
VOID	ReleaseEPD(PEPD, PCHAR);
VOID	ReleaseEPDLocked(PEPD, PCHAR);
VOID	LockMSD(PMSD, PCHAR);
VOID	ReleaseMSD(PMSD, PCHAR);
VOID	DecrementMSD(PMSD, PCHAR);

#define	LOCK_EPD(a, b)				LockEPD(a, b)
#define	RELEASE_EPD(a, b)			ReleaseEPD(a, b)
#define	RELEASE_EPD_LOCKED(a, b)	ReleaseEPDLocked(a, b)
#define	LOCK_MSD(a, b)				LockMSD(a, b)
#define RELEASE_MSD(a, b)			ReleaseMSD(a, b)
#define DECREMENT_MSD(a, b)			DecrementMSD(a, b)

#else

BOOL	LockEPD(PEPD);
VOID	ReleaseEPD(PEPD);
VOID	ReleaseEPDLocked(PEPD);
VOID	LockMSD(PMSD);
VOID	ReleaseMSD(PMSD);
VOID	DecrementMSD(PMSD);

#define	LOCK_EPD(a, b)				LockEPD(a)
#define	RELEASE_EPD(a, b)			ReleaseEPD(a)
#define	RELEASE_EPD_LOCKED(a, b)	ReleaseEPDLocked(a)
#define	LOCK_MSD(a, b)				LockMSD(a)
#define RELEASE_MSD(a, b)			ReleaseMSD(a)
#define DECREMENT_MSD(a, b)			DecrementMSD(a)

#endif

#define	LOCK_FMD(PTR)		(INTER_INC(PTR))
#define	RELEASE_FMD(PTR)	ASSERT((PTR)->uiRefCnt > 0); if( INTER_DEC(PTR) == 0) { FMDPool->Release(FMDPool, (PTR)); }

#define	LOCK_RCD(PTR)		(INTER_INC(PTR))
#define	RELEASE_RCD(PTR)	ASSERT((PTR)->uiRefCnt > 0); if( INTER_DEC(PTR) == 0) { RCDPool->Release(RCDPool, (PTR)); }

#define		RELEASE_SP_BUFFER(PTR)	if(PTR != NULL){ (PTR)->pNext = pRcvBuff; pRcvBuff = (PTR); }

#ifndef	CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field) ((type *)( \
                                                  (PCHAR)(address) - \
                                                  (UINT_PTR)(&((type *)0)->field)))
#endif

#define	RIGHT_SHIFT_64(HIGH_MASK, LOW_MASK) { ((LOW_MASK) >>= 1); if((HIGH_MASK) & 1){ (LOW_MASK) |= 0x80000000; } ((HIGH_MASK) >>= 1); }

#define	MAX(A, B)		( (A) > (B) ?  (A) : (B) )
#define	MIN(A, B)		( (A) < (B) ?  (A) : (B) )
#define	ABS(A)			( (A) < 0 ? -(A) : (A) )

//	CONVERT TO AND FROM 16.16 FIXED POINT REPRESENTATION

#define	TO_FP(X)		((X) << 16)
#define	FP_INT(X)		((X) >> 16)


#define	SALLOC(PTR, TYPE, NUMBER)	((PTR) = (TYPE *) MALLOC(sizeof(TYPE) * (NUMBER)))


#endif
