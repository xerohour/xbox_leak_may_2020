/*************************************************************
Module name: ProcDB.H
*************************************************************/

#ifndef _PROCDB_H_
#define _PROCDB_H_

class CPROCDB
{
	#define PROCDBQ_PROCESS		0x0001
	#define PROCDBQ_PROCESSAS	0x0002
	#define PROCDBQ_THREAD		0x0004
	#define PROCDBQ_THREADDET	0x0008
	#define PROCDBQ_IMAGE		0x0010

	public:
//		enum PROCDBERR {PROCDBE_NOERROR,};

	private:
		public:
		CPRFDB *m_pPrfDB;
		private:
		LPBYTE m_lpbPrfData;

		void GetPrfData(WORD wQueryType);

	public:
		CPROCDB (WORD wQueryType/*, PROCDBERR *lpPROCDBError*/);
		~CPROCDB (void);
		// Copy constructor goes here.

		void Refresh (WORD wQueryType/*, PROCDBERR *lpPROCDBError*/);

		struct PRFCNTRPROC
		{
			LARGE_INTEGER PerfTime;
			LARGE_INTEGER PerfFreq;			
			LARGE_INTEGER liPctPrivTime;			// PERF_100NSEC_TIMER
			LARGE_INTEGER liPctCPUTime;				// PERF_100NSEC_TIMER
			LARGE_INTEGER liPctUserTime;			// PERF_100NSEC_TIMER
			DWORD dwCntxSwtchPerSec;				// PERF_COUNTER_COUNTER
			LARGE_INTEGER liElapsedTime;			// PERF_ELAPSED_TIMER

			LARGE_INTEGER liFileCtrlBytesPerSec;	// PERF_COUNTER_BULK_COUNT
			DWORD dwFileCtrlOpsPerSec;				// PERF_COUNTER_COUNTER

			LARGE_INTEGER liFileReadBytesPerSec;	// PERF_COUNTER_BULK_COUNT
			DWORD dwFileReadOpsPerSec;				// PERF_COUNTER_COUNTER

			LARGE_INTEGER liFileWriteBytesPerSec;	// PERF_COUNTER_BULK_COUNT
			DWORD dwFileWriteOpsPerSec;				// PERF_COUNTER_COUNTER

			DWORD dwProcessId;						// PERF_COUNTER_RAWCOUNT

			DWORD dwPageFltsPerSec;					// PERF_COUNTER_COUNTER
			DWORD dwPageFilesBytes;					// PERF_COUNTER_RAWCOUNT
			DWORD dwPageFilesBytesPeak;				// PERF_COUNTER_RAWCOUNT

			DWORD dwPoolNonPagedBytes;				// PERF_COUNTER_RAWCOUNT
			DWORD dwPoolPagedBytes;					// PERF_COUNTER_RAWCOUNT

			DWORD dwPriorityBase;					// PERF_COUNTER_RAWCOUNT
			DWORD dwPrivateBytes;					// PERF_COUNTER_RAWCOUNT
			DWORD dwThrdCnt;						// PERF_COUNTER_RAWCOUNT

			DWORD dwVirtBytes;						// PERF_COUNTER_RAWCOUNT
			DWORD dwVirtBytesPeak;					// PERF_COUNTER_RAWCOUNT

			DWORD dwWorkingSet;						// PERF_COUNTER_RAWCOUNT
			DWORD dwWorkingSetPeak;					// PERF_COUNTER_RAWCOUNT
		};
		BOOL GetPrfCntrProc (int nIndexPrc, PRFCNTRPROC *lpPrfCntrProc) const;
		int GetNumProcesses (void) const;
		int GetProcIndexFromId (DWORD dwProcessId) const;
		LPCWSTR GetProcessName (int nIndexPrc) const;

		int GetNumProcessThreads (int nIndexPrc) const;
		int GetNumProcessImages (int nIndexPrc) const;

		struct PRFCNTRPROCAS
		{
			LARGE_INTEGER PerfTime;
			LARGE_INTEGER PerfFreq;			
			DWORD dwBytesFree;						// PERF_COUNTER_RAWCOUNT
			DWORD dwBytesImgFree;					// PERF_COUNTER_RAWCOUNT
			DWORD dwBytesImgReserved;				// PERF_COUNTER_RAWCOUNT
			DWORD dwBytesReserved;					// PERF_COUNTER_RAWCOUNT

			DWORD dwProcessId;						// PERF_COUNTER_RAWCOUNT

			DWORD dwImgSpER;						// PERF_COUNTER_RAWCOUNT
			DWORD dwImgSpERW;						// PERF_COUNTER_RAWCOUNT
			DWORD dwImgSpEWC;						// PERF_COUNTER_RAWCOUNT
			DWORD dwImgSpE;							// PERF_COUNTER_RAWCOUNT
			DWORD dwImgSpNA;						// PERF_COUNTER_RAWCOUNT
			DWORD dwImgSpR;							// PERF_COUNTER_RAWCOUNT
			DWORD dwImgSpRW;						// PERF_COUNTER_RAWCOUNT
			DWORD dwImgSpWC;						// PERF_COUNTER_RAWCOUNT

			DWORD dwMapSpER;						// PERF_COUNTER_RAWCOUNT
			DWORD dwMapSpERW;						// PERF_COUNTER_RAWCOUNT
			DWORD dwMapSpEWC;						// PERF_COUNTER_RAWCOUNT
			DWORD dwMapSpE;							// PERF_COUNTER_RAWCOUNT
			DWORD dwMapSpNA;						// PERF_COUNTER_RAWCOUNT
			DWORD dwMapSpR;							// PERF_COUNTER_RAWCOUNT
			DWORD dwMapSpRW;						// PERF_COUNTER_RAWCOUNT
			DWORD dwMapSpWC;						// PERF_COUNTER_RAWCOUNT

			DWORD dwResSpER;						// PERF_COUNTER_RAWCOUNT
			DWORD dwResSpERW;						// PERF_COUNTER_RAWCOUNT
			DWORD dwResSpEWC;						// PERF_COUNTER_RAWCOUNT
			DWORD dwResSpE;							// PERF_COUNTER_RAWCOUNT
			DWORD dwResSpNA;						// PERF_COUNTER_RAWCOUNT
			DWORD dwResSpR;							// PERF_COUNTER_RAWCOUNT
			DWORD dwResSpRW;						// PERF_COUNTER_RAWCOUNT
			DWORD dwResSpWC;						// PERF_COUNTER_RAWCOUNT

			DWORD dwUnassSpER;						// PERF_COUNTER_RAWCOUNT
			DWORD dwUnassSpERW;						// PERF_COUNTER_RAWCOUNT
			DWORD dwUnassSpEWC;						// PERF_COUNTER_RAWCOUNT
			DWORD dwUnassSpE;	 					// PERF_COUNTER_RAWCOUNT
			DWORD dwUnassSpNA;						// PERF_COUNTER_RAWCOUNT
			DWORD dwUnassSpR;						// PERF_COUNTER_RAWCOUNT
			DWORD dwUnassSpRW;						// PERF_COUNTER_RAWCOUNT
			DWORD dwUnassSpWC;						// PERF_COUNTER_RAWCOUNT
		};
		BOOL GetPrfCntrProcAS (int nIndexPrc, PRFCNTRPROCAS *lpPrfCntrProcAS) const;
		int GetProcASIndexFromId (DWORD dwProcessId) const;

		struct PRFCNTRTHRD
		{
			LARGE_INTEGER PerfTime;
			LARGE_INTEGER PerfFreq;			
			LARGE_INTEGER liPctPrivTime;			// PERF_100NSEC_TIMER
			LARGE_INTEGER liPctCPUTime;				// PERF_100NSEC_TIMER
			LARGE_INTEGER liPctUserTime;			// PERF_100NSEC_TIMER
			DWORD dwCntxSwtchPerSec;				// PERF_COUNTER_COUNTER
			LARGE_INTEGER liElapsedTime;			// PERF_ELAPSED_TIMER
			DWORD dwProcessId;						// PERF_COUNTER_RAWCOUNT
			DWORD dwThreadId;						// PERF_COUNTER_RAWCOUNT
			DWORD dwPriorityBase;					// PERF_COUNTER_RAWCOUNT
			DWORD dwPriorityCrnt;					// PERF_COUNTER_RAWCOUNT
			DWORD dwStartAddr;						// PERF_COUNTER_RAWCOUNT
			DWORD dwThreadState;					// PERF_COUNTER_RAWCOUNT
			DWORD dwThreadWaitReason;				// PERF_COUNTER_RAWCOUNT
		};
		BOOL GetPrfCntrThrd (int nIndexThd, PRFCNTRTHRD *lpPrfCntrThrd) const;
		int GetNumThreads (void) const;
		int GetThrdIndexFromId (DWORD dwThreadId) const;
		LPCWSTR GetThreadName (int nIndexThd) const;

		struct PRFCNTRIMG
		{
			LARGE_INTEGER PerfTime;
			LARGE_INTEGER PerfFreq;			
			DWORD dwE;								// PERF_COUNTER_RAWCOUNT
			DWORD dwER;								// PERF_COUNTER_RAWCOUNT
			DWORD dwERW;							// PERF_COUNTER_RAWCOUNT
			DWORD dwEWC;							// PERF_COUNTER_RAWCOUNT
			DWORD dwNA;								// PERF_COUNTER_RAWCOUNT
			DWORD dwR;								// PERF_COUNTER_RAWCOUNT
			DWORD dwRW;								// PERF_COUNTER_RAWCOUNT
			DWORD dwWC;								// PERF_COUNTER_RAWCOUNT
		};
		BOOL GetPrfCntrImg (int nIndexPrc, int nIndexImg, PRFCNTRIMG *lpPrfCntrImg) const;
		int GetNumImages (void) const;
		LPCWSTR GetImageName (int nIndexPrc, int nIndexImg) const;

		struct PRFCNTRTHRDDET
		{
			LARGE_INTEGER PerfTime;
			LARGE_INTEGER PerfFreq;			
			DWORD dwUserPC;							// PERF_COUNTER_RAWCOUNT
		};
		BOOL GetPrfCntrThrdDet (int nIndexThd, PRFCNTRTHRDDET *lpPrfCntrThrdDet) const;
};

#endif	//_PROCDB_H_

//////////////////////// End Of File /////////////////////////
