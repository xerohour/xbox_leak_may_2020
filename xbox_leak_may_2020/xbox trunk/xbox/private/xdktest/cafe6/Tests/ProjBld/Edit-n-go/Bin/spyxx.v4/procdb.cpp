/*************************************************************
Module name: ProcDB.CPP
*************************************************************/

#include "stdafx.h"
#pragma hdrstop

#include "prftilib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////
TCHAR szKeyData[32] = "";

DWORD dwTIProcess = 230, dwTIThread = 232, dwTIProcAddrSpace = 786, dwTIImage = 740, dwTIThreadDet = 816;

/*
Process object instance names are process names, no parent
Thread object instance names are thread numbers, Parent is Process Object
Process Address Space Object instance names are Process Names, no parent
Image Object names are filenames of mapped images. Parent is Process Address Space Object
Thread Detail Object instances are thread numbers, Parent is Process Address Space Object
*/

#define SZ_DUMMY 10

int GetRegDataBound(HKEY hkey)
{
	TCHAR chDummy[SZ_DUMMY];
	DWORD cSubKeys;
	DWORD cbMaxSubkey;
	DWORD cbMaxClass;
	DWORD cValues;
	DWORD cbMaxValueName;

	DWORD cbMaxValueData;
	DWORD cbSecurityDescriptor;
	FILETIME ftLastWriteTime;

	DWORD lErr;
	DWORD cbClassSize = SZ_DUMMY;

	lErr = RegQueryInfoKey(hkey, chDummy, &cbClassSize, NULL, &cSubKeys, &cbMaxSubkey, &cbMaxClass,
		&cValues, &cbMaxValueName, &cbMaxValueData, &cbSecurityDescriptor, &ftLastWriteTime);

	if (lErr == ERROR_SUCCESS)
		return cbMaxValueData;
	else
		return -1;
}


BOOL GetRegDataString(WORD wQueryType)
{
	HKEY hKeyData;
	LONG lErr;
	LPTSTR lptstr;
	int   iSize;
	DWORD dwType;

	DWORD dwSize;
	int   iIndex;

	lErr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\Perflib\\009"),
		0, KEY_ALL_ACCESS, &hKeyData);
	ASSERT(lErr == ERROR_SUCCESS);

	iSize = GetRegDataBound(hKeyData);

	if (iSize == -1)
	    return(FALSE);

	iSize++;
	lptstr = (LPTSTR)malloc(iSize * sizeof(TCHAR));
	dwSize = iSize;

	lErr = RegQueryValueEx(hKeyData, TEXT("Counters"), 0, &dwType, (LPBYTE) lptstr, &dwSize);
	ASSERT(lErr == ERROR_SUCCESS);

	char szIndex[8];
//	BOOL fGotProcess = FALSE, fGotThread = FALSE;
	BOOL fRetVal = FALSE, fGotProcess = FALSE, fGotThread = FALSE, fGotProcAS = FALSE, fGotThrdDet = FALSE;

	while (*lptstr)
	{
		_stscanf(lptstr, TEXT(" %d"), &iIndex);
		lptstr += lstrlen(lptstr) + 1;
		if ((wQueryType | PROCDBQ_PROCESS) && (!_tcsicmp(lptstr, "Process")))
		{
			if (_tcslen(szKeyData))
				_tcscat(szKeyData, " ");
			_tcscat(szKeyData, itoa(iIndex, szIndex, 10));
			fGotProcess = TRUE;
		}
		else if ((wQueryType | PROCDBQ_PROCESSAS) && (!_tcsicmp(lptstr, "Process Address Space")))
		{
			if (_tcslen(szKeyData))
				_tcscat(szKeyData, " ");
			_tcscat(szKeyData, itoa(iIndex, szIndex, 10));
			fGotProcAS = TRUE;
		}
		else if ((wQueryType | PROCDBQ_THREAD) && (!_tcsicmp(lptstr, "Thread")))
		{
			if (_tcslen(szKeyData))
				_tcscat(szKeyData, " ");
			_tcscat(szKeyData, itoa(iIndex, szIndex, 10));
			fGotThread = TRUE;
		}
		else if ((wQueryType | PROCDBQ_THREADDET) && (!_tcsicmp(lptstr, "Thread Details")))
		{
			if (_tcslen(szKeyData))
				_tcscat(szKeyData, " ");
			_tcscat(szKeyData, itoa(iIndex, szIndex, 10));
			fGotThrdDet = TRUE;
		}
		lptstr += lstrlen(lptstr) + 1;
	}

	lErr = RegCloseKey(hKeyData);
	ASSERT(lErr == ERROR_SUCCESS);

	if (wQueryType | PROCDBQ_PROCESS)
		fRetVal |= fGotProcess;
	if (wQueryType | PROCDBQ_PROCESSAS)
		fRetVal |= fGotProcAS;
	if (wQueryType | PROCDBQ_THREAD)
		fRetVal |= fGotThread;
	if (wQueryType | PROCDBQ_THREADDET)
		fRetVal |= fGotThrdDet;

	return(fRetVal);
}


void CPROCDB::GetPrfData(WORD wQueryType)
{
	// Objects: Process(230), Thread(232), Process Address Space(786), Image(740), Thread Details(816)
	static HANDLE hHeap;
	static DWORD cbData	= 32 * 1024;

	if (m_lpbPrfData == NULL)
	{
		hHeap = GetProcessHeap();
		m_lpbPrfData = (LPBYTE) HeapAlloc(hHeap, 0, cbData);
	}

	cbData = HeapSize(hHeap, 0, m_lpbPrfData);

	if (!_tcslen(szKeyData))
	{
		if (!GetRegDataString(wQueryType))
//			_tcscpy(szKeyData, "230 232");
			_tcscpy(szKeyData, "230 232 786 816");
	}

	while (TRUE)
	{
		DWORD dwType, dwLen = cbData;
		long lRes;

		lRes = RegQueryValueEx(HKEY_PERFORMANCE_DATA, szKeyData, NULL, &dwType, m_lpbPrfData, &dwLen);
		if (lRes != ERROR_MORE_DATA) 
			break;
		m_lpbPrfData = (LPBYTE) HeapReAlloc(hHeap, 0, m_lpbPrfData, cbData *= 2);
	}

	theApp.m_bDidPerfQueries = TRUE;

	// We have the performance data.
	m_pPrfDB = new CPRFDB((PPERF_DATA_BLOCK) m_lpbPrfData);
}


//////////////////////////////////////////////////////////////


CPROCDB::CPROCDB(WORD wQueryType/*, PROCDBERR *lpPROCDBError*/)
{
	m_lpbPrfData = NULL;
	Refresh(wQueryType/*, lpPROCDBError*/);
}


//////////////////////////////////////////////////////////////


CPROCDB::~CPROCDB(void)
{
	if (m_lpbPrfData != NULL)
	{
		delete(m_pPrfDB);
		HeapFree(GetProcessHeap(), 0, m_lpbPrfData);
		m_lpbPrfData = NULL;
	}
}


//////////////////////////////////////////////////////////////


void CPROCDB::Refresh(WORD wQueryType/*, PROCDBERR *lpPROCDBError*/)
{
//	*lpPROCDBError = PROCDBE_NOERROR;
//	m_lpbPrfData = NULL;
	GetPrfData(wQueryType);
}
		

//////////////////////////////////////////////////////////////


int CPROCDB::GetNumProcesses(void) const
{
	int nNumProcesses = 0;
	// Find Process Object in performance data.
	PCPRFOBJTYPE pPrfObjType = m_pPrfDB->FindObjFromTitleIndex(dwTIProcess);
	if (pPrfObjType != NULL)
	{
		// If found, NumInstances indicates number 
		// of running processes
		nNumProcesses = pPrfObjType->NumInstances;		
	}
	return(nNumProcesses);
}


//////////////////////////////////////////////////////////////


int CPROCDB::GetProcIndexFromId(DWORD dwProcessId) const
{
	int nNumProcesses = GetNumProcesses();

	while (nNumProcesses--)
	{
		PRFCNTRPROC PrfCntrProc;
		GetPrfCntrProc(nNumProcesses, &PrfCntrProc);
		if (PrfCntrProc.dwProcessId == dwProcessId)
			return(nNumProcesses);
	}
	return(-1);
}


//////////////////////////////////////////////////////////////


int CPROCDB::GetProcASIndexFromId(DWORD dwProcessId) const
{
	int nNumProcesses = GetNumProcesses();

	while (nNumProcesses--)
	{
		PRFCNTRPROCAS PrfCntrProcAddrSpc;
		GetPrfCntrProcAS(nNumProcesses, &PrfCntrProcAddrSpc);
		if (PrfCntrProcAddrSpc.dwProcessId == dwProcessId)
			return(nNumProcesses);
	}
	return(-1);
}


//////////////////////////////////////////////////////////////


LPCWSTR CPROCDB::GetProcessName(int nIndexPrc) const
{
	LPCWSTR szProcessName = NULL;

	// Find Process Object in performance data.
	PCPRFOBJTYPE pPrfObjType = m_pPrfDB->FindObjFromTitleIndex(dwTIProcess);
	if (pPrfObjType != NULL)
	{
		// If found, Get the nIndexPrc'th instance
		PCPRFINSTDEF pPrfInstDef = m_pPrfDB->GetInst(pPrfObjType, nIndexPrc);
		if (pPrfInstDef != NULL)
			szProcessName = (LPCWSTR)m_pPrfDB->GetInstName(pPrfInstDef);
	}
	return(szProcessName);
}


//////////////////////////////////////////////////////////////


int CPROCDB::GetNumThreads(void) const
{
	int nNumThreads = 0;
	// Find Thread Object in performance data.
	PCPRFOBJTYPE pPrfObjType = m_pPrfDB->FindObjFromTitleIndex(dwTIThread);
	if (pPrfObjType != NULL)
	{
		// If found, NumInstances indicates number 
		// of running threads
		nNumThreads = pPrfObjType->NumInstances;		
	}
	return(nNumThreads);
}


//////////////////////////////////////////////////////////////
//
//
//int CPROCDB::GetNumProcessThreads (int nIndexPrc) const {
//	int nNumThreads = 0;
//	// Find Thread Object in performance data.
//	PCPRFOBJTYPE pPrfObjType = m_pPrfDB->FindObjFromTitleIndex(dwTIThread);
//	if (pPrfObjType != NULL) {
//		// If found, get first thread instance.
//		PCPRFINSTDEF pPrfInstDef = m_pPrfDB->GetFirstInst(pPrfObjType);
//		// Walk the list of instances and count the number of
//		// instances whose ParentObjectIndex equals nIndexPrc.
//		if (pPrfInstDef->ParentObjectInstance == (DWORD) nIndexPrc)
//			nNumThreads++;
//	}
//	return(nNumThreads);
//}


//////////////////////////////////////////////////////////////


int CPROCDB::GetNumProcessThreads(int nIndexPrc) const
{
	int nNumThreads = 0;
	// Find Thread Object in performance data.
	PCPRFOBJTYPE pPrfObjType = m_pPrfDB->FindObjFromTitleIndex(dwTIThread);
	if (pPrfObjType != NULL)
	{
		// If found, get first thread instance.
		PCPRFINSTDEF pPrfInstDef = m_pPrfDB->GetFirstInst(pPrfObjType);
		// Walk the list of instances and count the number of
		// instances whose ParentObjectIndex equals nIndexPrc.
		int nInstNum = 0, nNumInstances = m_pPrfDB->GetNumInstances(pPrfObjType);
		for (; nInstNum < nNumInstances; nInstNum++)
		{
			// ALERT: I am assumming that the Process names are 
			// enumerated in the same order for both the Process 
			// object instances and the Process Address Space 
			// Object instance
			if (pPrfInstDef->ParentObjectInstance == (DWORD) nIndexPrc)
				nNumThreads++;
			pPrfInstDef = m_pPrfDB->GetNextInst(pPrfInstDef);
		}
	}
	return(nNumThreads);
}


//////////////////////////////////////////////////////////////


int CPROCDB::GetThrdIndexFromId(DWORD dwThreadId) const
{
	int nNumThreads = GetNumThreads();

	while (nNumThreads--)
	{
		PRFCNTRTHRD PrfCntrThrd;
		GetPrfCntrThrd(nNumThreads, &PrfCntrThrd);
		if (PrfCntrThrd.dwThreadId == dwThreadId)
			return(nNumThreads);
	}
	return(-1);
}


//////////////////////////////////////////////////////////////


LPCWSTR CPROCDB::GetThreadName(int nIndexThd) const
{
	LPCWSTR szThreadName = NULL;

	// Find Thread Object in performance data.
	PCPRFOBJTYPE pPrfObjType = m_pPrfDB->FindObjFromTitleIndex(dwTIThread);
	if (pPrfObjType != NULL)
	{
		// If found, Get the nIndexPrc'th instance
		PCPRFINSTDEF pPrfInstDef = m_pPrfDB->GetInst(pPrfObjType, nIndexThd);
		if (pPrfInstDef != NULL)
			szThreadName = (LPCWSTR)GetProcessName(pPrfInstDef->ParentObjectInstance);
	}
	return(szThreadName);
}


//////////////////////////////////////////////////////////////


int CPROCDB::GetNumImages(void) const
{
	int nNumImages = 0;
	// Find Image Object in performance data.
	PCPRFOBJTYPE pPrfObjType = m_pPrfDB->FindObjFromTitleIndex(dwTIImage);
	if (pPrfObjType != NULL)
	{
		// If found, NumInstances indicates number 
		// of images
		nNumImages = pPrfObjType->NumInstances;		
	}
	return(nNumImages);
}


//////////////////////////////////////////////////////////////


int CPROCDB::GetNumProcessImages(int nIndexPrc) const
{
	int nNumImages = 0;
	// Find Image Object in performance data.
	PCPRFOBJTYPE pPrfObjType = m_pPrfDB->FindObjFromTitleIndex(dwTIImage);
	if (pPrfObjType != NULL)
	{
		// If found, get first image instance.
		PCPRFINSTDEF pPrfInstDef = m_pPrfDB->GetFirstInst(pPrfObjType);
		// Walk the list of instances and count the number of
		// instances whose ParentObjectIndex equals nIndexPrc.

		int nInstNum = 0, nNumInstances = m_pPrfDB->GetNumInstances(pPrfObjType);
		for (; nInstNum < nNumInstances; nInstNum++)
		{
			// ALERT: I am assumming that the Process names are 
			// enumerated in the same order for both the Process 
			// object instances and the Process Address Space 
			// Object instance
			if (pPrfInstDef->ParentObjectInstance == (DWORD) nIndexPrc)
				nNumImages++;
			pPrfInstDef = m_pPrfDB->GetNextInst(pPrfInstDef);
		}
	}
	return(nNumImages);
}


//////////////////////////////////////////////////////////////


LPCWSTR CPROCDB::GetImageName(int nIndexPrc, int nIndexImg) const
{
	LPCWSTR szImageName = NULL;
	int nNumImages = 0;

	// Find Image Object in performance data.
	PCPRFOBJTYPE pPrfObjType = m_pPrfDB->FindObjFromTitleIndex(dwTIImage);
	if (pPrfObjType != NULL) {
		// If found, get first image instance.
		PCPRFINSTDEF pPrfInstDef = m_pPrfDB->GetFirstInst(pPrfObjType);
		// Walk the list of instances and count the number of
		// instances whose ParentObjectIndex equals nIndexPrc.

		while ((pPrfInstDef != NULL) && (nNumImages <= nIndexImg))
		{
			// ALERT: I am assumming that the Process names are 
			// enumerated in the same order for both the Process 
			// object instances and the Process Address Space 
			// Object instance
			if (pPrfInstDef->ParentObjectInstance == (DWORD) nIndexPrc)
			{
				if (nNumImages++ == nIndexImg)
					szImageName = (LPCWSTR)m_pPrfDB->GetInstName(pPrfInstDef);
			}

			pPrfInstDef = m_pPrfDB->GetNextInst(pPrfInstDef);
		}
	}
	return(szImageName);
}


//////////////////////////////////////////////////////////////


BOOL CPROCDB::GetPrfCntrProc(int nIndexPrc, PRFCNTRPROC *lpPrfCntrProc) const
{
	// Find Process Object in performance data.
	PCPRFOBJTYPE pPrfObjType = m_pPrfDB->FindObjFromTitleIndex(dwTIProcess);
	if (pPrfObjType == NULL) 
		return(FALSE);
		
	// If found, get the nIndexPrc'th instance
	PCPRFINSTDEF pPrfInstDef = m_pPrfDB->GetInst(pPrfObjType, nIndexPrc);
	if (pPrfInstDef == NULL) 
		return(FALSE);
		
	int nCntr = 0, nNumCntrs = m_pPrfDB->GetNumCntrs(pPrfObjType);
	PCPRFCNTRDEF pPrfCntrDef = m_pPrfDB->GetFirstCntr(pPrfObjType);
	memcpy(&lpPrfCntrProc->PerfTime, &pPrfObjType->PerfTime, sizeof(pPrfObjType->PerfTime));
	memcpy(&lpPrfCntrProc->PerfFreq, &pPrfObjType->PerfFreq, sizeof(pPrfObjType->PerfFreq));

	for (; nCntr < nNumCntrs; nCntr++)
	{
		void * const pCntrData = m_pPrfDB->GetCntrData(pPrfInstDef, pPrfCntrDef);
		int cbDataLen = pPrfCntrDef->CounterSize;
		switch (pPrfCntrDef->CounterNameTitleIndex)
		{
			case PTI_PROCESS_PRIVILEGED_TIME:
				memcpy(&lpPrfCntrProc->liPctPrivTime, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_PROCESSOR_TIME:
				memcpy(&lpPrfCntrProc->liPctCPUTime, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_USER_TIME:
				memcpy(&lpPrfCntrProc->liPctUserTime, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_ELAPSED_TIME:
				memcpy(&lpPrfCntrProc->liElapsedTime, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_FILE_CONTROL_BYTES:
				memcpy(&lpPrfCntrProc->liFileCtrlBytesPerSec, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_FILE_CONTROL_OPERATIONS:
				memcpy(&lpPrfCntrProc->dwFileCtrlOpsPerSec, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_FILE_READ_BYTES:
				memcpy(&lpPrfCntrProc->liFileReadBytesPerSec, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_FILE_READ_OPERATIONS:
				memcpy(&lpPrfCntrProc->dwFileReadOpsPerSec, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_FILE_WRITE_BYTES:
				memcpy(&lpPrfCntrProc->liFileWriteBytesPerSec, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_FILE_WRITE_OPERATIONS:
				memcpy(&lpPrfCntrProc->dwFileWriteOpsPerSec, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_PROCESSID:
				memcpy(&lpPrfCntrProc->dwProcessId, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_FAULT_COUNT:
				memcpy(&lpPrfCntrProc->dwPageFltsPerSec, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_PAGEFILE_BYTES:
				memcpy(&lpPrfCntrProc->dwPageFilesBytes, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_PAGEFILE_BYTES_PEAK:
				memcpy(&lpPrfCntrProc->dwPageFilesBytesPeak, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_POOL_NONPAGED_BYTES:
				memcpy(&lpPrfCntrProc->dwPoolNonPagedBytes, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_POOL_PAGED_BYTES:
				memcpy(&lpPrfCntrProc->dwPoolPagedBytes, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_PRIORITY_BASE:
				memcpy(&lpPrfCntrProc->dwPriorityBase, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_PRIVATE_BYTES:
				memcpy(&lpPrfCntrProc->dwPrivateBytes, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_THREAD_COUNT:
				memcpy(&lpPrfCntrProc->dwThrdCnt, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_VIRTUAL_BYTES:
				memcpy(&lpPrfCntrProc->dwVirtBytes, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_VIRTUAL_BYTES_PEAK:
				memcpy(&lpPrfCntrProc->dwVirtBytesPeak, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_WORKING_SET:
				memcpy(&lpPrfCntrProc->dwWorkingSet, pCntrData, cbDataLen);
				break;
			case PTI_PROCESS_WORKING_SET_PEAK:
				memcpy(&lpPrfCntrProc->dwWorkingSetPeak, pCntrData, cbDataLen);
				break;
		}
		pPrfCntrDef = m_pPrfDB->GetNextCntr(pPrfCntrDef);
	}
	return(TRUE);
}


//////////////////////////////////////////////////////////////


BOOL CPROCDB::GetPrfCntrProcAS(int nIndexPrc, PRFCNTRPROCAS *lpPrfCntrProcAS) const
{
	// Find Process Object in performance data.
	PCPRFOBJTYPE pPrfObjType = m_pPrfDB->FindObjFromTitleIndex(dwTIProcAddrSpace);
	if (pPrfObjType == NULL) 
		return(FALSE);
		
	// If found, get the nIndexPrc'th instance
	PCPRFINSTDEF pPrfInstDef = m_pPrfDB->GetInst(pPrfObjType, nIndexPrc);
	if (pPrfInstDef == NULL) 
		return(FALSE);
		
	int nCntr = 0, nNumCntrs = m_pPrfDB->GetNumCntrs(pPrfObjType);
	PCPRFCNTRDEF pPrfCntrDef = m_pPrfDB->GetFirstCntr(pPrfObjType);
	memcpy(&lpPrfCntrProcAS->PerfTime, &pPrfObjType->PerfTime, sizeof(pPrfObjType->PerfTime));
	memcpy(&lpPrfCntrProcAS->PerfFreq, &pPrfObjType->PerfFreq, sizeof(pPrfObjType->PerfFreq));

	for (; nCntr < nNumCntrs; nCntr++)
	{
		void * const pCntrData = m_pPrfDB->GetCntrData(pPrfInstDef, pPrfCntrDef);
		int cbDataLen = pPrfCntrDef->CounterSize;
		switch (pPrfCntrDef->CounterNameTitleIndex) {
			case PTI_PROCADDR_BYTES_FREE:
				memcpy(&lpPrfCntrProcAS->dwBytesFree, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_BYTES_IMAGE_FREE:
				memcpy(&lpPrfCntrProcAS->dwBytesImgFree, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_BYTES_IMAGE_RESERVED:
				memcpy(&lpPrfCntrProcAS->dwBytesImgReserved, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_BYTES_RESERVED:
				memcpy(&lpPrfCntrProcAS->dwBytesReserved, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_PROCESSID:
				memcpy(&lpPrfCntrProcAS->dwProcessId, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_IMAGE_EXECREADONLY:
				memcpy(&lpPrfCntrProcAS->dwImgSpER, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_IMAGE_EXECREADWRITE:
				memcpy(&lpPrfCntrProcAS->dwImgSpERW, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_IMAGE_EXECWRITECOPY:
				memcpy(&lpPrfCntrProcAS->dwImgSpEWC, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_IMAGE_EXECUTABLE:
				memcpy(&lpPrfCntrProcAS->dwImgSpE, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_IMAGE_NOACCESS:
				memcpy(&lpPrfCntrProcAS->dwImgSpNA, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_IMAGE_READONLY:
				memcpy(&lpPrfCntrProcAS->dwImgSpR, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_IMAGE_READWRITE:
				memcpy(&lpPrfCntrProcAS->dwImgSpRW, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_IMAGE_WRITECOPY:
				memcpy(&lpPrfCntrProcAS->dwImgSpWC, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_MAPPED_EXECREADONLY:
				memcpy(&lpPrfCntrProcAS->dwMapSpER, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_MAPPED_EXECREADWRITE:
				memcpy(&lpPrfCntrProcAS->dwMapSpERW, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_MAPPED_EXECWRITECOPY:
				memcpy(&lpPrfCntrProcAS->dwMapSpEWC, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_MAPPED_EXECUTABLE:
				memcpy(&lpPrfCntrProcAS->dwMapSpE, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_MAPPED_NOACCESS:
				memcpy(&lpPrfCntrProcAS->dwMapSpNA, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_MAPPED_READONLY:
				memcpy(&lpPrfCntrProcAS->dwMapSpR, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_MAPPED_READWRITE:
				memcpy(&lpPrfCntrProcAS->dwMapSpRW, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_MAPPED_WRITECOPY:
				memcpy(&lpPrfCntrProcAS->dwMapSpWC, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_RESERVED_EXECREADONLY:
				memcpy(&lpPrfCntrProcAS->dwResSpER, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_RESERVED_EXECREADWRITE:
				memcpy(&lpPrfCntrProcAS->dwResSpERW, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_RESERVED_EXECWRITECOPY:
				memcpy(&lpPrfCntrProcAS->dwResSpEWC, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_RESERVED_EXECUTABLE:
				memcpy(&lpPrfCntrProcAS->dwResSpE, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_RESERVED_NOACCESS:
				memcpy(&lpPrfCntrProcAS->dwResSpNA, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_RESERVED_READONLY:
				memcpy(&lpPrfCntrProcAS->dwResSpR, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_RESERVED_READWRITE:
				memcpy(&lpPrfCntrProcAS->dwResSpRW, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_RESERVED_WRITECOPY:
				memcpy(&lpPrfCntrProcAS->dwResSpWC, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_UNASSIGNED_EXECREADONLY:
				memcpy(&lpPrfCntrProcAS->dwUnassSpER, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_UNASSIGNED_EXECREADWRITE:
				memcpy(&lpPrfCntrProcAS->dwUnassSpERW, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_UNASSIGNED_EXECWRITECOPY:
				memcpy(&lpPrfCntrProcAS->dwUnassSpEWC, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_UNASSIGNED_EXECUTABLE:
				memcpy(&lpPrfCntrProcAS->dwUnassSpE, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_UNASSIGNED_NOACCESS:
				memcpy(&lpPrfCntrProcAS->dwUnassSpNA, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_UNASSIGNED_READONLY:
				memcpy(&lpPrfCntrProcAS->dwUnassSpR, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_UNASSIGNED_READWRITE:
				memcpy(&lpPrfCntrProcAS->dwUnassSpRW, pCntrData, cbDataLen);
				break;
			case PTI_PROCADDR_UNASSIGNED_WRITECOPY:
				memcpy(&lpPrfCntrProcAS->dwUnassSpWC, pCntrData, cbDataLen);
				break;
		}
		pPrfCntrDef = m_pPrfDB->GetNextCntr(pPrfCntrDef);
	}
	return(TRUE);
}


//////////////////////////////////////////////////////////////


BOOL CPROCDB::GetPrfCntrThrd(int nIndexThd, PRFCNTRTHRD *lpPrfCntrThrd) const
{
	// Find Process Object in performance data.
	PCPRFOBJTYPE pPrfObjType = m_pPrfDB->FindObjFromTitleIndex(dwTIThread);
	if (pPrfObjType == NULL) 
		return(FALSE);
		
	// If found, get the nIndexThd'th instance
	PCPRFINSTDEF pPrfInstDef = m_pPrfDB->GetInst(pPrfObjType, nIndexThd);
	if (pPrfInstDef == NULL) 
		return(FALSE);
		
	int nCntr = 0, nNumCntrs = m_pPrfDB->GetNumCntrs(pPrfObjType);
	PCPRFCNTRDEF pPrfCntrDef = m_pPrfDB->GetFirstCntr(pPrfObjType);
	memcpy(&lpPrfCntrThrd->PerfTime, &pPrfObjType->PerfTime, sizeof(pPrfObjType->PerfTime));
	memcpy(&lpPrfCntrThrd->PerfFreq, &pPrfObjType->PerfFreq, sizeof(pPrfObjType->PerfFreq));

	for (; nCntr < nNumCntrs; nCntr++)
	{
		void * const pCntrData = m_pPrfDB->GetCntrData(pPrfInstDef, pPrfCntrDef);
		int cbDataLen = pPrfCntrDef->CounterSize;
		switch (pPrfCntrDef->CounterNameTitleIndex)
		{
			case PTI_THREAD_PRIVILEGED_TIME:
				memcpy(&lpPrfCntrThrd->liPctPrivTime, pCntrData, cbDataLen);
				break;
			case PTI_THREAD_PROCESSOR_TIME:
				memcpy(&lpPrfCntrThrd->liPctCPUTime, pCntrData, cbDataLen);
				break;
			case PTI_THREAD_USER_TIME:
				memcpy(&lpPrfCntrThrd->liPctUserTime, pCntrData, cbDataLen);
				break;
			case PTI_THREAD_CONTEXT_SWITCHES:
				memcpy(&lpPrfCntrThrd->dwCntxSwtchPerSec, pCntrData, cbDataLen);
				break;
			case PTI_THREAD_ELAPSED_TIME:
				memcpy(&lpPrfCntrThrd->liElapsedTime, pCntrData, cbDataLen);
				break;
			case PTI_THREAD_PROCESSID:
				memcpy(&lpPrfCntrThrd->dwProcessId, pCntrData, cbDataLen);
				break;
			case PTI_THREAD_THREADID:
				memcpy(&lpPrfCntrThrd->dwThreadId, pCntrData, cbDataLen);
				break;
			case PTI_THREAD_BASE_PRIORITY:
				memcpy(&lpPrfCntrThrd->dwPriorityBase, pCntrData, cbDataLen);
				break;
			case PTI_THREAD_CURRENT_PRIORITY:
				memcpy(&lpPrfCntrThrd->dwPriorityCrnt, pCntrData, cbDataLen);
				break;
			case PTI_THREAD_START_ADDRESS:
				memcpy(&lpPrfCntrThrd->dwStartAddr, pCntrData, cbDataLen);
				break;
			case PTI_THREAD_STATE:
				memcpy(&lpPrfCntrThrd->dwThreadState, pCntrData, cbDataLen);
				break;
			case PTI_THREAD_WAIT_REASON:
				memcpy(&lpPrfCntrThrd->dwThreadWaitReason, pCntrData, cbDataLen);
				break;
		}
		pPrfCntrDef = m_pPrfDB->GetNextCntr(pPrfCntrDef);
	}
	return(TRUE);
}


//////////////////////////////////////////////////////////////


BOOL CPROCDB::GetPrfCntrThrdDet(int nIndexThd, PRFCNTRTHRDDET *lpPrfCntrThrdDet) const
{
	// Find Process Object in performance data.
	PCPRFOBJTYPE pPrfObjType = m_pPrfDB->FindObjFromTitleIndex(dwTIThreadDet);
	if (pPrfObjType == NULL) 
		return(FALSE);
		
	// If found, get the nIndexThd'th instance
	PCPRFINSTDEF pPrfInstDef = m_pPrfDB->GetInst(pPrfObjType, nIndexThd);
	if (pPrfInstDef == NULL) 
		return(FALSE);
		
	int nCntr = 0, nNumCntrs = m_pPrfDB->GetNumCntrs(pPrfObjType);
	PCPRFCNTRDEF pPrfCntrDef = m_pPrfDB->GetFirstCntr(pPrfObjType);
	memcpy(&lpPrfCntrThrdDet->PerfTime, &pPrfObjType->PerfTime, sizeof(pPrfObjType->PerfTime));
	memcpy(&lpPrfCntrThrdDet->PerfFreq, &pPrfObjType->PerfFreq, sizeof(pPrfObjType->PerfFreq));

	for (; nCntr < nNumCntrs; nCntr++)
	{
		void * const pCntrData = m_pPrfDB->GetCntrData(pPrfInstDef, pPrfCntrDef);
		int cbDataLen = pPrfCntrDef->CounterSize;
		switch (pPrfCntrDef->CounterNameTitleIndex)
		{
			case PTI_THREAD_USER_PC:	// Thread Details object - User PC
				memcpy(&lpPrfCntrThrdDet->dwUserPC, pCntrData, cbDataLen);
				break;
		}
		pPrfCntrDef = m_pPrfDB->GetNextCntr(pPrfCntrDef);
	}
	return(TRUE);
}


//////////////////////////////////////////////////////////////


BOOL CPROCDB::GetPrfCntrImg(int nIndexPrc, int nIndexImg, PRFCNTRIMG *lpPrfCntrImg) const
{
	// Find Process Object in performance data.
	PCPRFOBJTYPE pPrfObjType = m_pPrfDB->FindObjFromTitleIndex(dwTIImage);
	if (pPrfObjType == NULL) 
		return(FALSE);
		
	// If found, get the nIndexPrc'th instance
	PCPRFINSTDEF pPrfInstDef = m_pPrfDB->GetInst(pPrfObjType, nIndexPrc);
	if (pPrfInstDef == NULL) 
		return(FALSE);
		
	int nCntr = 0, nNumCntrs = m_pPrfDB->GetNumCntrs(pPrfObjType);
	PCPRFCNTRDEF pPrfCntrDef = m_pPrfDB->GetFirstCntr(pPrfObjType);
	memcpy(&lpPrfCntrImg->PerfTime, &pPrfObjType->PerfTime, sizeof(pPrfObjType->PerfTime));
	memcpy(&lpPrfCntrImg->PerfFreq, &pPrfObjType->PerfFreq, sizeof(pPrfObjType->PerfFreq));

	for (; nCntr < nNumCntrs; nCntr++)
	{
		void * const pCntrData = m_pPrfDB->GetCntrData(pPrfInstDef, pPrfCntrDef);
		int cbDataLen = pPrfCntrDef->CounterSize;
		switch (pPrfCntrDef->CounterNameTitleIndex)
		{
			case PTI_IMAGE_EXECUTABLE:
				memcpy(&lpPrfCntrImg->dwE, pCntrData, cbDataLen);
				break;
			case PTI_IMAGE_EXE_READONLY:
				memcpy(&lpPrfCntrImg->dwER, pCntrData, cbDataLen);
				break;
			case PTI_IMAGE_EXE_READWRITE:
				memcpy(&lpPrfCntrImg->dwERW, pCntrData, cbDataLen);
				break;
			case PTI_IMAGE_EXE_WRITECOPY:
				memcpy(&lpPrfCntrImg->dwEWC, pCntrData, cbDataLen);
				break;
			case PTI_IMAGE_NOACCESS:
				memcpy(&lpPrfCntrImg->dwNA, pCntrData, cbDataLen);
				break;
			case PTI_IMAGE_READONLY:
				memcpy(&lpPrfCntrImg->dwR, pCntrData, cbDataLen);
				break;
			case PTI_IMAGE_READWRITE:
				memcpy(&lpPrfCntrImg->dwRW, pCntrData, cbDataLen);
				break;
			case PTI_IMAGE_WRITECOPY:
				memcpy(&lpPrfCntrImg->dwWC, pCntrData, cbDataLen);
				break;
		}
		pPrfCntrDef = m_pPrfDB->GetNextCntr(pPrfCntrDef);
	}
	return(TRUE);
}


//////////////////////// End Of File /////////////////////////
