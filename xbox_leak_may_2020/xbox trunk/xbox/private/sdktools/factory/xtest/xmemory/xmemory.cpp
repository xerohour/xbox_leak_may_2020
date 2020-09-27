// (c) Copyright 2000-2001 Intel Corp. All rights reserved.
//
// Title: xmemory  
//
// History:
//
// 11/30/00 V1.00 SSS Release V1.00.
// 
//
// INTEL CONFIDENTIAL.  Copyright (c) 1997-2001 Intel Corp. All rights reserved.

// turn off the you may not inited with warning
#pragma warning( disable : 4701 )
#pragma warning( disable : 4800 )

// version number
#pragma comment(exestr,"version_number=1.00.0.010")

#include "..\stdafx.h"
#include "..\testobj.h"
#include "xmemory.h"
#include "stress.h"
#include "memutils.h"


extern "C" { 
	ULONG_PTR
		NTAPI
		MmGetPhysicalAddress(
		IN PVOID BaseAddress
		);
	
}

IMPLEMENT_MODULEUNLOCKED (CXModuleMemory);

bool CXModuleMemory::InitializeParameters ()
{
	// Call the base class.  This is REQUIRED
	//
	
	if (!CTestObj::InitializeParameters ())
		return false;
	GlobalSeed = 0;
	holdrand = 1;
	AbortMemPtr = NULL;
	
	
	// Mutex for memory tests to safely query and allocate memory when running in parallel.
	g_hMemoryMutex = CreateMutex(NULL,FALSE,"WinMTA*Memory*Mutex");
	
	
	// if non-zero, injects errors to test error messages.
	g_fFaultInject = ParameterSwitch(L"FaultInject");
	
	return true;
}


IMPLEMENT_TESTUNLOCKED (Memory, DisplayMemoryStatus, 1)
{
	TCHAR StrOutput[512];
	wchar_t TestName[] = L"DisplayMemoryStatus";
	
	//	#ifdef _DEBUG
	static int MemStat;
	MemStat++;
	ReportDebug(BIT0, _T(" DisplayMemoryStatus Test - Loop %d"), MemStat);
	//	#endif
	
	try 
	{
		_stprintf(StrOutput,_T("in code\n"));
		OutputDebugString(StrOutput);
		TestNamePtr = TestName;
		DisplayMemoryStatus();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
}
IMPLEMENT_TESTUNLOCKED (Memory, MemorySizeCheck, 2)
{
	//	#ifdef _DEBUG
	static int MemChk;
	MemChk++;
	ReportDebug(BIT0, _T(" MemorySizeCheck Test - Loop %d"), MemChk);
	//	#endif
	
	try 
	{
		MemorySizeCheck();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
}
IMPLEMENT_TESTUNLOCKED (Memory, MemorySlidingOnesAndZeros, 3)
{
	//	#ifdef _DEBUG
	static int Mem0and1;
	Mem0and1++;
	ReportDebug(BIT0, _T(" MemorySlidingOnesAndZeros Test - Loop %d"), Mem0and1);
	//	#endif
	
	try 
	{
		MemorySlidingOnesAndZeros();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
}
IMPLEMENT_TESTUNLOCKED (Memory, MemoryRandomData, 4)
{
	//	#ifdef _DEBUG
	static int MemRandom;
	MemRandom++;
	ReportDebug(BIT0, _T(" MemoryRandomData Test - Loop %d"), MemRandom);
	//	#endif
	
	try 
	{
		MemoryRandomData();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
}
IMPLEMENT_TESTUNLOCKED (Memory, MemoryAddressPatterns, 5)
{
	//	#ifdef _DEBUG
	static int MemAdPat;
	MemAdPat++;
	ReportDebug(BIT0, _T(" MemoryAddressPatterns Test - Loop %d"), MemAdPat);
	//	#endif
	
	try 
	{
		MemoryAddressPatterns();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
}

IMPLEMENT_TESTUNLOCKED (Memory, MemoryMixedOps, 6)
{
	//	#ifdef _DEBUG
	static int MemMixOp;
	MemMixOp++;
	ReportDebug(BIT0, _T(" MemoryMixedOps Test - Loop %d"), MemMixOp);
	//	#endif
	
	try 
	{
		MemoryMixedOps();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
}

IMPLEMENT_TESTUNLOCKED (Memory, MemoryRandomDwordOps, 7)
{
	//	#ifdef _DEBUG
	static int MemRandWordOp;
	MemRandWordOp++;
	ReportDebug(BIT0, _T(" MemoryRandomDwordOps Test - Loop %d"), MemRandWordOp);
	//	#endif
	
	try 
	{
		MemoryRandomDwordOps();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
}

IMPLEMENT_TESTUNLOCKED (Memory, CacheSlidingOnesAndZeros, 8 )
{
	//	#ifdef _DEBUG
	static int Cache0and1;
	Cache0and1++;
	ReportDebug(BIT0, _T(" CacheSlidingOnesAndZeros Test - Loop %d"), Cache0and1);
	//	#endif
	
	try 
	{
		CacheSlidingOnesAndZeros();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
}
IMPLEMENT_TESTUNLOCKED (Memory, CacheRandomData, 9)
{
	//	#ifdef _DEBUG
	static int CacheRandom;
	CacheRandom++;
	ReportDebug(BIT0, _T(" CacheRandomData Test - Loop %d"), CacheRandom);
	//	#endif
	
	try 
	{
		CacheRandomData();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
}
IMPLEMENT_TESTUNLOCKED (Memory, CacheAddressPatterns, 10)
{
	//	#ifdef _DEBUG
	static int CacheAdPat;
	CacheAdPat++;
	ReportDebug(BIT0, _T(" CacheAddressPatterns Test - Loop %d"), CacheAdPat);
	//	#endif
	
	try 
	{
		CacheAddressPatterns();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
}
IMPLEMENT_TESTUNLOCKED (Memory, CacheMixedOps, 11)
{
	//	#ifdef _DEBUG
	static int CacheMixOp;
	CacheMixOp++;
	ReportDebug(BIT0, _T(" CacheMixedOps Test - Loop %d"), CacheMixOp);
	//	#endif
	
	try 
	{
		CacheMixedOps();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
	
}
IMPLEMENT_TESTUNLOCKED (Memory, CacheRandomDwordOps, 12)
{
	//	#ifdef _DEBUG
	static int CacheRanDwOp;
	CacheRanDwOp++;
	ReportDebug(BIT0, _T(" CacheRandomDwordOps Test - Loop %d"), CacheRanDwOp);
	//	#endif
	
	try 
	{
		CacheRandomDwordOps();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
	
}
IMPLEMENT_TESTUNLOCKED (Memory, WindowedCacheRandomDwordOps, 13)
{
	//	#ifdef _DEBUG
	static int WCacheRanDwOp;
	WCacheRanDwOp++;
	ReportDebug(BIT0, _T(" WindowedCacheRandomDwordOps Test - Loop %d"), WCacheRanDwOp);
	//	#endif
	
	try 
	{
		WindowedCacheRandomDwordOps();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
	
}
IMPLEMENT_TESTUNLOCKED (Memory, WindowedCacheMixedOps, 14)
{
	//	#ifdef _DEBUG
	static int WCacheMixOp;
	WCacheMixOp++;
	ReportDebug(BIT0, _T(" WindowedCacheMixedOps Test - Loop %d"), WCacheMixOp);
	//	#endif
	
	try 
	{
		WindowedCacheMixedOps();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
	
}
IMPLEMENT_TESTUNLOCKED (Memory, WindowedCacheRandomData, 15)
{
	//	#ifdef _DEBUG
	static int WCacheRandom;
	WCacheRandom++;
	ReportDebug(BIT0, _T(" WindowedCacheRandomData Test - Loop %d"), WCacheRandom);
	//	#endif
	
	try 
	{
		WindowedCacheRandomData();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
	
}
IMPLEMENT_TESTUNLOCKED (Memory, WindowedCacheUniqueness, 16)
{
	//	#ifdef _DEBUG
	static int WCacheUniq;
	WCacheUniq++;
	ReportDebug(BIT0, _T(" WindowedCacheUniqueness Test - Loop %d"), WCacheUniq);
	//	#endif
	
	try 
	{
		WindowedCacheUniqueness();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
	
}
IMPLEMENT_TESTUNLOCKED (Memory, MemoryTransfer, 17)
{
	//	#ifdef _DEBUG
	static int Memxfer;
	Memxfer++;
	ReportDebug(BIT0, _T(" MemoryTransfer Test - Loop %d"), Memxfer);
	//	#endif
	
	try
	{
		MemoryTransfer();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
	
}
IMPLEMENT_TESTUNLOCKED (Memory, QBusNoise, 18)
{
	//	#ifdef _DEBUG
	static int QNoise;
	QNoise++;
	ReportDebug(BIT0, _T(" QBusNoise Test - Loop %d"), QNoise);
	//	#endif
	
	try 
	{
		QWORDBusNoise();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
	
}
IMPLEMENT_TESTUNLOCKED (Memory, CachePerformance, 19)
{
	//	#ifdef _DEBUG
	static int CachePerf;
	CachePerf++;
	ReportDebug(BIT0, _T(" CachePerformance Test - Loop %d"), CachePerf);
	//	#endif
	
	try 
	{
		CachePerformance();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
	
}
IMPLEMENT_TESTUNLOCKED (Memory, MemoryPerformance, 20)
{
	//	#ifdef _DEBUG
	static int MemPerf;
	MemPerf++;
	ReportDebug(BIT0, _T(" MemoryPerformance Test - Loop %d"), MemPerf);
	//	#endif
	
	try 
	{
		MemoryPerformance();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
	
}
IMPLEMENT_TESTUNLOCKED (Memory, DDR32KStride, 21)
{
	//	#ifdef _DEBUG
	static int DDR32KStr;
	DDR32KStr++;
	ReportDebug(BIT0, _T(" DDR32KStride Test - Loop %d"), DDR32KStr);
	//	#endif
	
	try 
	{
		DDR32KStride();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
	
}
IMPLEMENT_TESTUNLOCKED (Memory, DDRBusPowersof2, 22)
{
	//	#ifdef _DEBUG
	static int DDRPowof2;
	DDRPowof2++;
	ReportDebug(BIT0, _T(" DDRBusPowersof2 Test - Loop %d"), DDRPowof2);
	//	#endif
	
	try 
	{
		DDRBusPowersof2();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
	
}
IMPLEMENT_TESTUNLOCKED (Memory, DDRMemoryBanks, 23)
{
	//	#ifdef _DEBUG
	static int DDRMemBank;
	DDRMemBank++;
	ReportDebug(BIT0, _T(" DDRMemoryBanks Test - Loop %d"), DDRMemBank);
	//	#endif
	
	try 
	{
		DDRMemoryBanks();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknow exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
}
IMPLEMENT_TESTUNLOCKED (Memory, DDRDeviceStride, 24)
{
	//	#ifdef _DEBUG
	static int DDRDevStr;
	DDRDevStr++;
	ReportDebug(BIT0, _T(" DDRDeviceStride Test - Loop %d"), DDRDevStr);
	//	#endif
	
	try 
	{
		DDRDeviceStride();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknown exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
	
}
IMPLEMENT_TESTUNLOCKED (Memory, DDRBusNoise, 25)
{
	//	#ifdef _DEBUG
	static int DDRNoise;
	DDRNoise++;
	ReportDebug(BIT0, _T(" DDRBusNoise Test - Loop %d"), DDRNoise);
	//	#endif
	
	try 
	{
		DDRBusNoise();
	}
	catch (int except)
	{
		// just leaving
		if( except == 99) return;
		ReportDebug(0x999,L"Unknown exception 0x%x",except);
	}
	catch (CXmtaException &e)
	{
		ReportError (e.GetErrorCode (), e.GetMessage ());
	}
	
}

//******************************************************************************
// Function name	: the destructor!
// Description	    : Cleans up an allocations and handles left hanging
// Return type		: none
//******************************************************************************
CXModuleMemory::~CXModuleMemory()
{	
	if(!(g_hMemoryMutex == NULL))
		CloseHandle(g_hMemoryMutex);
}

//******************************************************************************
// Function name	: DisplayMemoryStatus
// Description	    : Display the values in the MEMORYSTATUS structure.
// Return type		: void
//******************************************************************************
void CXModuleMemory::DisplayMemoryStatus()
{
	MEMORYSTATUSEX MemoryStatus;
	VERIFY(SafeGlobalMemoryStatusEx(&MemoryStatus));
	
	ReportDebug(1,
		L"Memory Load: %lu%%\n"
		L"Total Physical: %I64Xh bytes\n"
		L"Available Physical: %I64Xh bytes\n"
		L"Total Page File %I64Xh bytes\n"
		L"Available Page File: %I64Xh bytes\n"
		L"Total Virtual: %I64Xh bytes\n"
		L"Available Virtual: %I64Xh bytes\n"
		L"Available Extended Virtual: %I64Xh bytes\n"
		L"Physical Memory Available to the OS: %I64uMB\n"
		L"Largest Contiguous Virtual Memory Block: %I64uMB",
		MemoryStatus.dwMemoryLoad,
		MemoryStatus.ullTotalPhys,
		MemoryStatus.ullAvailPhys,
		MemoryStatus.ullTotalPageFile,
		MemoryStatus.ullAvailPageFile,
		MemoryStatus.ullTotalVirtual,
		MemoryStatus.ullAvailVirtual,
		MemoryStatus.ullAvailExtendedVirtual,
		(QWORD)GetPhysicalMemorySize() >> 20,
		(QWORD)GetLargestVirtualBufferSize() >> 20);
}


//******************************************************************************
// Function name	: MemorySizeCheck
// Description	    : Verifies that the MemorySizeMB parameter matches the amount
//                    of memory in the system.
// Return type		: void
//******************************************************************************
void CXModuleMemory::MemorySizeCheck()
{
	DWORD dwExpectedMB,dwActualMB = (DWORD)(GetPhysicalMemorySize() >> 20);
	
	if((dwExpectedMB =GetCfgUint(L"MemorySizeMB",0)) == 0)
	{
		ReportError(0x10,L"MemorySizeMB missing");
		return;
	}
	if(dwExpectedMB != dwActualMB)
		errMemorySizeLL(dwExpectedMB,dwActualMB);
	
}


//******************************************************************************
// Function name	: MemorySlidingOnesAndZeros
// Description	    : Initializes the parameters for the SlidingOnesAndZeros
//                    test thread for a full memory test using one thread.
// Return type		: void
//******************************************************************************
void CXModuleMemory::MemorySlidingOnesAndZeros()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	bool           fForce,fNoCache;
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize        = ParameterMemorySize();
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	
	fForce               = ParameterSwitch(L"Force");
	fNoCache             = ParameterSwitch(L"NoCache");
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,fForce,fNoCache,1);
	
	SlidingOnesAndZeros(&ThreadParams);
	
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}


//******************************************************************************
// Function name	: CacheSlidingOnesAndZeros
// Description	    : Initializes the parameters for the SlidingOnesAndZeros
//                    test thread for a cache test using one thread per processor.
// Return type		: void
//******************************************************************************
void CXModuleMemory::CacheSlidingOnesAndZeros()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	bool           fForce,fNoCache;
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize = ParameterCacheSize();
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	fForce = ParameterSwitch(L"Force");
	fNoCache = ParameterSwitch(L"NoCache");
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,fForce,fNoCache,1);
	SlidingOnesAndZeros(&ThreadParams);
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
	
}


//******************************************************************************
// Function name	: MemoryRandomData
// Description	    : Initializes the parameters for the RandomData test thread
//                    for a full memory test using one thread.
// Return type		: void
//******************************************************************************
void CXModuleMemory::MemoryRandomData()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	bool           fForce,fNoCache;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize        = ParameterMemorySize();
	ThreadParams.ProcNumber      = -1;	// don't care;
	ThreadParams.pTestParameters = &parameters;
	
	fForce               = ParameterSwitch(L"Force");
	fNoCache             = ParameterSwitch(L"NoCache");
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,fForce,fNoCache,1);
	
	RandomData(&ThreadParams);
	
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}


//******************************************************************************
// Function name	: CacheRandomData
// Description	    : Initializes the parameters for the RandomData test thread
//                    for a cache test using one thread per processor.
// Return type		: void
//******************************************************************************
void CXModuleMemory::CacheRandomData()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	bool           fForce,fNoCache;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize = ParameterCacheSize();
	ThreadParams.pTestParameters = &parameters;
	fForce = ParameterSwitch(L"Force");
	fNoCache = ParameterSwitch(L"NoCache");
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,fForce,fNoCache,1);
	RandomData(&ThreadParams);
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}


//******************************************************************************
// Function name	: MemoryAddressPatterns
// Description	    : Initializes the parameters for the AddressPatterns test
//                    thread for a full memory test using one thread.
// Return type		: void
//******************************************************************************
void CXModuleMemory::MemoryAddressPatterns()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	bool           fForce,fNoCache;
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize        = ParameterMemorySize();
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	
	fForce               = ParameterSwitch(L"Force");
	fNoCache             = ParameterSwitch(L"NoCache");
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,fForce,fNoCache,1);
	
	AddressPatterns(&ThreadParams);
	
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}


//******************************************************************************
// Function name	: CacheAddressPatterns
// Description	    : Initializes the parameters for the AddressPatterns test
//                    thread for a cache test using one thread per processor.
// Return type		: void
//******************************************************************************
void CXModuleMemory::CacheAddressPatterns()
{
	//TBD update cache control
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	bool           fForce,fNoCache;
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize = ParameterCacheSize();
	ThreadParams.pTestParameters = &parameters;
	fForce               = ParameterSwitch(L"Force");
	fNoCache             = ParameterSwitch(L"NoCache");
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,fForce,fNoCache,1);
	AddressPatterns(&ThreadParams);
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}


//******************************************************************************
// Function name	: MemoryMixedOps
// Description	    : Initializes the parameters for the MixedOpsMixAlignRandomOrder
//                    test thread for a full memory test using one thread.   A
//                    list of non-overlapping byte, word, and dword accesses
//                    (both aligned and unaligned) is generated to completely
//                    access a 4KB block of memory. All of memory is written
//                    4KB at a time using that access list, then read back using
//                    a different 4KB access list.
// Return type		: void
//******************************************************************************
void CXModuleMemory::MemoryMixedOps()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	bool           fForce,fNoCache;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize        = ParameterMemorySize();
	parameters.BlockSize         = 0x1000;
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	
	fForce               = ParameterSwitch(L"Force");
	fNoCache             = ParameterSwitch(L"NoCache");
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,fForce,fNoCache,1);
	
	MixedOpsMixAlignRandomOrder(&ThreadParams);
	
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}


//******************************************************************************
// Function name	: MemoryRandomDwordOps
// Description	    : Initializes the parameters for the DwordRandomOrder
//                    test thread for a full memory test using one thread.   A
//                    list of non-overlapping dword accesses is generated to
//                    completely access a 32KB block of memory. All of memory is
//                    written 32KB at a time using that access list, then read
//                    back using a different 32KB access list.
// Return type		: void
//******************************************************************************
void CXModuleMemory::MemoryRandomDwordOps()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	bool           fForce,fNoCache;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize        = ParameterMemorySize();
	parameters.BlockSize         = 0x8000;
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	
	fForce               = ParameterSwitch(L"Force");
	fNoCache             = ParameterSwitch(L"NoCache");
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,fForce,fNoCache,1);
	
	DwordRandomOrder(&ThreadParams);
	
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}


//******************************************************************************
// Function name	: CacheMixedOps
// Description	    : Initializes the parameters for the MixedOpsMixAlignRandomOrder
//                    test thread for a cache test using one thread per processor.
//                    A list of non-overlapping byte, word, and dword accesses (both
//                    aligned and unaligned) is generated to completely access
//                    a 4KB block of memory.  A cache-sized block of memory is
//                    written 4KB at a time using that access list, then read
//                    back using a different 4KB access list.
// Return type		: void
//******************************************************************************
void CXModuleMemory::CacheMixedOps()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	bool           fForce,fNoCache;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize = ParameterCacheSize();
	parameters.BlockSize  = 0x1000;
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	fForce               = ParameterSwitch(L"Force");
	fNoCache             = ParameterSwitch(L"NoCache");
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,fForce,fNoCache,1);
	MixedOpsMixAlignRandomOrder(&ThreadParams);
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}


//******************************************************************************
// Function name	: CacheRandomDwordOps
// Description	    : Initializes the parameters for the DwordRandomOrder test
//                    thread for a cache test using one thread per processor.
//                    A list of non-overlapping dword accesses is generated to
//                    completely access a 32KB block of memory.  A cache-sized
//                    block of memory is written 32KB at a time using that access
//                    list, then read back using a different 32KB access list.
// Return type		: void
//******************************************************************************
void CXModuleMemory::CacheRandomDwordOps()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	bool           fForce,fNoCache;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize = ParameterCacheSize();
	parameters.BlockSize  = 0x8000;
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	fForce               = ParameterSwitch(L"Force");
	fNoCache             = ParameterSwitch(L"NoCache");
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,fForce,fNoCache,1);
	DwordRandomOrder(&ThreadParams);
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}


//******************************************************************************
// Function name	: WindowedCacheRandomData
// Description	    : Initializes the parameters for the RandomData test thread
//                    for a cache test using one thread per processor.  The test
//                    is "windowed" because it divides memory equally between
//                    processors and into cache-sized chunks and runs the cache
//                    test on each chunk.  This will test more tags in the cache.
// Return type		: void
//******************************************************************************
void CXModuleMemory::WindowedCacheRandomData()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	bool           fForce,fNoCache;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize  = ParameterMemorySize();
	parameters.WindowSize  = ParameterCacheSize();
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	fForce               = ParameterSwitch(L"Force");
	fNoCache             = ParameterSwitch(L"NoCache");
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,fForce,fNoCache,1);
	RandomData(&ThreadParams);
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}


//******************************************************************************
// Function name	: WindowedCacheMixedOps
// Description	    : Initializes the parameters for the MixedOpsMixAlignRandomOrder
//                    test thread for a cache test using one thread per processor.
//                    The test is "windowed" because it divides memory equally
//                    between processors and into cache-sized chunks and runs the
//                    equivalent of the CacheMixedOps test on each chunk.  This
//                    will test more tags in the cache.
// Return type		: void
//******************************************************************************
void CXModuleMemory::WindowedCacheMixedOps()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	bool           fForce,fNoCache;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize = ParameterMemorySize();
	parameters.BlockSize  = 0x1000;
	parameters.WindowSize = ParameterCacheSize();
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	fForce               = ParameterSwitch(L"Force");
	fNoCache             = ParameterSwitch(L"NoCache");
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,fForce,fNoCache,1);
	MixedOpsMixAlignRandomOrder(&ThreadParams);
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}


//******************************************************************************
// Function name	: WindowedCacheRandomDwordOps
// Description	    : Initializes the parameters for the DwordRandomOrder test
//                    thread for a cache test using one thread per processor.
//                    The test is "windowed" because it divides memory equally
//                    between processors and into cache-sized chunks and runs the
//                    equivalent of the CacheRandomDwordOps test on each chunk.
//                    This will test more tags in the cache.
// Return type		: void
//******************************************************************************
void CXModuleMemory::WindowedCacheRandomDwordOps()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	bool           fForce,fNoCache;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize = ParameterMemorySize();
	parameters.BlockSize  = 0x8000;
	parameters.WindowSize = ParameterCacheSize();
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	fForce               = ParameterSwitch(L"Force");
	fNoCache             = ParameterSwitch(L"NoCache");
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,fForce,fNoCache,1);
	DwordRandomOrder(&ThreadParams);
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
	
}

//******************************************************************************
// Function name	: WindowedCacheUniqueness
// Description	    : Initializes the parameters for the MixedOpsMixAlignRandomOrder
//                    test thread for a cache test using one thread per processor.
//                    The test is "windowed" because it divides memory equally
//                    between processors and into cache-sized chunks and runs the
//                    equivalent of the CacheMixedOps test on each chunk.  This
//                    will test more tags in the cache.  This test is the same
//                    as the WindowedCacheMixedOps test except it generates an
//                    access list for a cache-sized block of memory instead of
//                    a 4KB block of memory.
// Return type		: void
//******************************************************************************
void CXModuleMemory::WindowedCacheUniqueness()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	bool           fForce,fNoCache;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize = ParameterMemorySize();
	parameters.WindowSize = ParameterCacheSize();
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	fForce               = ParameterSwitch(L"Force");
	fNoCache             = ParameterSwitch(L"NoCache");
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,fForce,fNoCache,1);
	MixedOpsMixAlignRandomOrder(&ThreadParams);
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
	
}


//******************************************************************************
// Function name	: DDR32KStride
// Description	    : A special case of the powers of 2's test.  Most bang for the
//                  : for the test time buck.  Uses all caches and memory by creating a 
//                  : Write back cascade.
// Return type		: void
//******************************************************************************
void CXModuleMemory::DDR32KStride()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize        = ParameterMemorySize();
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,ParameterSwitch(L"Force"),FALSE,1);
	
	DPowersof2(&ThreadParams,0x8000, 0x8000);
	
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}
//******************************************************************************
// Function name	: DDRDeviceSuround
// Description	    : 
// Return type		: void
//******************************************************************************
void CXModuleMemory::DDRMemoryBanks()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize        = ParameterMemorySize();
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	
	// Runs with cache off to force writes at time of instruction.
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,ParameterSwitch(L"Force"),FALSE,1);
	
	DPowersof2(&ThreadParams,parameters.MemorySize/2,parameters.MemorySize/4 );
	
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}
//******************************************************************************
// Function name	: DDRDeviceStride
// Description	    : 
// Return type		: void
//******************************************************************************
void CXModuleMemory::DDRDeviceStride()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize        = ParameterMemorySize();
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	// Runs with cache off to force writes only to device targeted.
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,ParameterSwitch(L"Force"),FALSE,1);
	// the stride is because the device are 4 device 2 banks
	DPowersof2(&ThreadParams,16,16);
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}
//******************************************************************************
// Function name	: DDRBusNoise
// Description	    :
// Return type		: void
//******************************************************************************
void CXModuleMemory::DDRBusNoise()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize        = ParameterMemorySize();
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,ParameterSwitch(L"Force"),FALSE,1);
	
	DPowersof2(&ThreadParams,0x8000, 0x8000);
	
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}
//******************************************************************************
// Function name	: QWORDBusNoise
// Description	    : 
// Return type		: void
//******************************************************************************
void CXModuleMemory::QWORDBusNoise()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize        = ParameterMemorySize();
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	parameters.WindowSize = ParameterCacheSize()/2;
	
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,ParameterSwitch(L"Force"),FALSE,1);
	
	QWORDNoise(&ThreadParams);
	
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}
//******************************************************************************
// Function name	: DDRBusPowerof2
// Description	    : 
// Return type		: void
//******************************************************************************
void CXModuleMemory::DDRBusPowersof2()
{
	TESTCONDITIONS parameters;
	THREADPARAMS   ThreadParams;
	
	// Init the random number generator for this test.
	SetRandomSeed();
	
	ZeroMemory(&parameters,sizeof(parameters));
	parameters.MemorySize        = ParameterMemorySize();
	ThreadParams.ProcNumber      = -1;	// don't care
	ThreadParams.pTestParameters = &parameters;
	
	ThreadParams.pMemory = AllocateMemory(&parameters.MemorySize,ParameterSwitch(L"Force"),FALSE,1);
	
	DPowersof2(&ThreadParams,0x8, 0x10000);
	
	if(ThreadParams.pMemory)
		FreeMemory(ThreadParams.pMemory,parameters.MemorySize);
}

//******************************************************************************
// Function name	: SlidingOnesAndZeros
// Description	    : Shifts a single one and a single zero through every DWORD.
// Return type		: DWORD - Always 0.
// Argument         : LPTHREADPARAMS pThreadParams
//                    The processor number and parameters for the test.
//                    Only the MemorySizeMB parameter is used.
//******************************************************************************
DWORD CXModuleMemory::SlidingOnesAndZeros(LPTHREADPARAMS pThreadParams)
{
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	DWORD            pattern[2][64];
	DWORD_PTR        offset,MemorySize;
	DWORD            nProgCur,nProgMax;
	LPVOID           pMem = pThreadParams->pMemory;
	LPDWORD          pdwVirt;
	LPDWORD          pdwMax,pdwSrc;
	LPTESTCONDITIONS pParameters;
	int i,j;
	BYTE bAbortStatus;
	pParameters   = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize    = pParameters->MemorySize;
	fProc0        = (pThreadParams->ProcNumber <= 0);	// Thread running on proc zero?
	
	// Store the sliding ones and zeros pattern in a 64-DWORD array.
	//     pattern[0] pattern[1]
	//  [0] FFFFFFFE   00000001
	//  [1] FFFFFFFD   00000002
	//  [2] FFFFFFFB   00000004
	//  [3] FFFFFFF7   00000008
	//         :          :
	// [28] EFFFFFFF   10000000
	// [29] DFFFFFFF   20000000
	// [30] BFFFFFFF   40000000
	// [31] 7FFFFFFF   80000000
	// [32] FFFFFFFE   00000001
	// [33] FFFFFFFD   00000002
	// [34] FFFFFFFB   00000004
	// [35] FFFFFFF7   00000008
	//         :          :
	// [60] EFFFFFFF   10000000
	// [61] DFFFFFFF   20000000
	// [62] BFFFFFFF   40000000
	// [63] 7FFFFFFF   80000000
	
	for(i = 0; i < 64; i++)
	{
		pattern[1][i] = 1 << (i % 32);
		pattern[0][i] = ~pattern[1][i];
	}
	
	pdwMax = (LPDWORD)pMem + MemorySize / 4;	// end of allocated memory.
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = 64;
	nProgCur = 0;
	
	// Write every DWORD in the allocated memory with every pattern in the pattern array.
	// Writes pattern[0][0]  to pattern[0][31] over and over again, then
	// writes pattern[0][1]  to pattern[0][32] over and over again, then
	// writes pattern[0][2]  to pattern[0][33] over and over again, then
	// writes pattern[0][3]  to pattern[0][34] over and over again, then
	//    :       :          :    :             :   :    :    :      :
	// writes pattern[0][28] to pattern[0][59] over and over again, then
	// writes pattern[0][29] to pattern[0][60] over and over again, then
	// writes pattern[0][30] to pattern[0][61] over and over again, then
	// writes pattern[0][31] to pattern[0][62] over and over again, then
	// Writes pattern[1][0]  to pattern[1][31] over and over again, then
	// writes pattern[1][1]  to pattern[1][32] over and over again, then
	// writes pattern[1][2]  to pattern[1][33] over and over again, then
	// writes pattern[1][3]  to pattern[1][34] over and over again, then
	//    :       :          :    :             :   :    :    :      :
	// writes pattern[1][28] to pattern[1][59] over and over again, then
	// writes pattern[1][29] to pattern[1][60] over and over again, then
	// writes pattern[1][30] to pattern[1][61] over and over again, then
	// writes pattern[1][31] to pattern[1][62] over and over again.
	//
	// This shifts a single zero through every bit of every DWORD in the allocation,
	// then shifts a single one through every bit.  Adjacent DWORDs have adjacent
	// bits set.
	//
	for(i = 0; i < 2; i++)
	{
		for(j = 0; j < 32; j++)
		{
			// Compute start of test pattern.
			pdwSrc = pattern[i] + j;
			ReportDebug(BIT0,L"Slidingones progress %d",((i+1)*(j+1))/64);
			
			// check for abort
			
			bAbortStatus = CheckAbort(L"Error message report");
			if(bAbortStatus == TRUE)
				throw((int)99);
			
			// Step 32-DWORDs at a time through the memory allocation.
			for(LPDWORD pdw = (LPDWORD)pMem; pdw < pdwMax; pdw += 32)
				CopyMemory(pdw,pdwSrc,32 * 4);	// Copy 32 DWORDs from pattern array to memory.
			
			if(g_fFaultInject)
			{
				*(pdwMax - 10) = 0xDEADBEEF;
			}
			//check for abort status
			bAbortStatus = CheckAbort(L"Error message report");
			if(bAbortStatus == TRUE)
				throw((int)99);
			// Step 32-DWORDs at a time through the memory allocation.
			for(pdw = (LPDWORD)pMem; pdw < pdwMax; pdw += 32)
			{
				// Compare 32-DWORDS with the pattern buffer.
				if((pdwVirt = AsmDwordMemCmp(pdw,pdwSrc,32)) != NULL)
				{
					// non-zero pbVirt is the virtual address of a failure.
					offset = pdwVirt - pdw; // dword offset
					DecodeFailingLocation(1,pdwVirt, (QWORD) *(pdwSrc + offset), (QWORD) *pdwVirt, 4, NULL);
				}
			}
			
			if(fProc0)
				Progress(++nProgCur * 100 / nProgMax);
		}
	}
	
	return 0;
}


//******************************************************************************
// Function name	: RandomData
// Description	    : Writes random DWORDs and their complement to every DWORD
// Return type		: DWORD - Always 0.
// Argument         : LPTHREADPARAMS pThreadParams
//                    Uses the MemorySize and WindowSize parameters only.
//******************************************************************************
DWORD CXModuleMemory::RandomData(LPTHREADPARAMS pThreadParams)
{
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	DWORD            dwValue;
	DWORD_PTR        MemorySize,WindowSize,nProgCur,nProgMax;
	LPVOID           pMem = pThreadParams->pMemory;
	LPBYTE           pbMax;
	LPDWORD          pdwMax;
	LPTESTCONDITIONS pParameters;
	BYTE bAbortStatus;
	
	
	pParameters   = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize    = pParameters->MemorySize;
	WindowSize    = pParameters->WindowSize;
	fProc0        = (pThreadParams->ProcNumber <= 0);
	
	if(WindowSize == 0)
		WindowSize = MemorySize;
	
	// MemorySize must be a multiple of WindowSize
	MemorySize -= MemorySize % WindowSize;
	
	// Compute end test address.
	pbMax = (LPBYTE)pMem + MemorySize;
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = MemorySize / WindowSize * 4;
	if(nProgMax < 4)
		nProgMax = 4;
	nProgCur = 0;
	
	// Walk through memory a window at a time.
	for(LPBYTE pb = (LPBYTE)pMem; pb < pbMax; pb += WindowSize)
	{
		// Compute the end of the window.
		pdwMax = (LPDWORD)pb + WindowSize / 4;
		
		// Fill the window with random DWORDs
		ReSeed(pThreadParams->ProcNumber);
		for(LPDWORD pdw = (LPDWORD)pb; pdw < pdwMax; pdw++)
			*pdw = random();
		//check for abort status
		bAbortStatus = CheckAbort(L"Error message report");
		if(bAbortStatus == TRUE)
			throw((int)99);
		if(g_fFaultInject)
			*(pdwMax - 1) = 0xDEADBEEF;
		
		if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
			Progress((int)(++nProgCur * 100 / nProgMax));
		
		// Read the DWORDs back and check for errors.
		ReSeed(pThreadParams->ProcNumber);
		for(pdw = (LPDWORD)pb; pdw < pdwMax; pdw++)
		{
			dwValue = random();
			if(*pdw != dwValue)
			{
				DecodeFailingLocation(1,pdw, (QWORD) dwValue, (QWORD) *pdw, 4, NULL);
				
				/*				errRndDataMiscomparePQLLLI(
				real ? (LPVOID)real : pdw,
				(LPBYTE)pdw - pb,
				dwValue,
				*pdw,
				GetRandomSeed(),
				pThreadParams->ProcNumber);*/
			}
		}
		
		if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
			Progress((int)(++nProgCur * 100 / nProgMax));
		
		// Invert the memory.
		ReSeed(pThreadParams->ProcNumber);
		for( pdw = (LPDWORD)pb; pdw < pdwMax; pdw++)
			*pdw = ~random();
		
		if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
			Progress((int)(++nProgCur * 100 / nProgMax));
		
		if(g_fFaultInject)
			*(pdwMax - 1) = 0xDEADBEEF;
		//check for abort status
		bAbortStatus = CheckAbort(0);
		if(bAbortStatus == TRUE)
			throw((int)99);
		
		// Check for errors again.
		ReSeed(pThreadParams->ProcNumber);
		for(pdw = (LPDWORD)pb; pdw < pdwMax; pdw++)
		{
			dwValue = ~random();
			if(*pdw != dwValue)
			{
				DecodeFailingLocation(1,pdw, (QWORD) dwValue, (QWORD) *pdw, 4, NULL);
				
				/*	errRndDataMiscomparePQLLLI(
				real ? (LPVOID)real : pdw,
				(LPBYTE)pdw - pb,
				dwValue,
				*pdw,
				GetRandomSeed(),
				pThreadParams->ProcNumber);*/
			}
		}
		
		if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
			Progress((int)(++nProgCur * 100 / nProgMax));
	}
	
	return 0;
}

//******************************************************************************
// Function name	: QWORDNoise
// Description	    : Shifts a single a single zero through every nibble in QWORD
// Return type		: DWORD - Always 0.
// Argument         : LPTHREADPARAMS pThreadParams
//                    The processor number and parameters for the test.
//                    Only the MemorySizeMB parameter is used.
//******************************************************************************
DWORD CXModuleMemory::QWORDNoise(LPTHREADPARAMS pThreadParams)
{
	BOOL             fProc0;	// TRUE if thread sends wt_progress messages.
	DWORD            MemorySize,WindowSize,nProgCur,nProgMax;
	LPVOID           pMem = pThreadParams->pMemory;
	LPBYTE           pbMax;
	LPDWORD          pdwMax;
	LPTESTCONDITIONS pParameters;
	BYTE bAbortStatus;
	
	pParameters   = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize    = pParameters->MemorySize;
	WindowSize    = pParameters->MemorySize;
	
	fProc0        = (pThreadParams->ProcNumber <= 0);
	
	LPDWORD pdw;
	DWORD dwBlockSize;
	DWORD dwAddress;
	BYTE bPatterns;
	BYTE bStatus;
	QWORD qwLow ;
	QWORD qwHigh;
	QWORD qwData =  0x0001000100010001;
	DWORD dwBusWords =8;
	QWORD exp;
	QWORD rec;
	_int64 TransCount = 0;
	
	if(WindowSize == 0)
		WindowSize = MemorySize;
	
	// MemorySize must be a multiple of WindowSize
	MemorySize -= MemorySize % WindowSize;
	
	// Compute end test address.
	pbMax = (LPBYTE)pMem + MemorySize;
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = MemorySize / WindowSize * 64; // 8 patterns
	nProgCur = 0;
	
	// Walk through memory a window at a time.
	for(LPBYTE pb = (LPBYTE)pMem; pb < pbMax; pb += WindowSize)
	{
		// Compute the end of the window.
		pdwMax = (LPDWORD)pb + WindowSize / 4;
		
		pdw = (LPDWORD)pb;
		dwBlockSize = WindowSize / 2;  //bytes to words - the asm code counts words
		// the pattern is 4 shifts by even and odd
		for(bPatterns = 0; bPatterns<64; bPatterns++)
		{
			if (bPatterns < 32)
			{
				qwLow =  0xFFFFFFFFFFFFFFFF & ~(qwData << bPatterns%4);
				qwHigh = ~qwLow;
			}
			// 0111, 1011, 1101, 1110
			else
			{
				qwHigh =  0xFFFFFFFFFFFFFFFF & ~(qwData << bPatterns%4);
				qwLow = ~qwHigh;
				
			}
			
			TransCount = TransCount + WindowSize * 2; // write read 1 block
			ReportDebug(1,L"Writing\n");
			//check for abort status
			bAbortStatus = CheckAbort(0);
			if(bAbortStatus == TRUE)
				throw((int)99);
			
			__asm {
				emms
					push	eax
					push	ebx
					push	ecx
					push	edx
					push	edi
					push	esi
					mov		ebx, dwBlockSize
					mov		edx, dwBusWords
					movq	mm0,qwLow
					movq	mm1,qwHigh
					mov		edi, pb				// load the starting memory address
loop1:
				movq	[edi], mm0
					add		edi, edx
					movq	[edi], mm1
					add		edi, edx
					sub		ebx,8
					jnz		loop1
					
					pop		esi
					pop		edi
					pop		edx
					pop		ecx
					pop		ebx
					pop		eax
					emms
			} 
			
			
			if(g_fFaultInject)
				*(pdwMax - 1) = 0xDEADBEEF;
			// blow the data out to memory with a write back of the cache
			__asm{
				wbinvd
			}
			
			// pre load the cache with data
			__asm {
				emms
					push	eax
					push	ebx
					push	ecx
					push	edx
					push	edi
					push	esi
					mov		ebx, dwBlockSize // the size of the memory block to test in bytes
					mov		esi, 32			 // the stride of the loads by cache line- 32 bytes
					mov		edi, pb			 // load the starting memory address
loop3r:
				mov		eax, [edi]		// load the data in to ax
					add		edi, esi		// inc the memory pointer
					sub		ebx,32			// inc to next cache line
					jnz		loop3r
					
					pop		esi
					pop		edi
					pop		edx
					pop		ecx
					pop		ebx
					pop		eax
					emms
			}
			
			
			// do read from cache and compare
			__asm {
				emms
					push	eax
					push	ebx
					push	ecx
					push	edx
					push	edi
					push	esi
					mov		ebx, dwBlockSize
					mov		esi, dwBusWords
					movq	mm2,qwLow			// what to compare
					movq	mm3,qwHigh
					movq	mm4,qwLow			// make a copy for exp pat
					movq	mm5,qwHigh
					
					mov		edi, pb				// load the starting memory address
loop1r:
				movq	mm0, [edi]
					pcmpeqd mm2,mm0				// compare the data with the exp pat.
					psrlq	mm2,16				// shift so we can get both result in the low Dword
					movd	edx,mm2				// move the result to real reg where we can do real compare
					cmp		edx,0FFFFFFFFh
					jne		error1
					movq	mm2,mm4				// fresh data to cmp
					add		edi, esi
					
					movq	mm1, [edi]
					pcmpeqd mm3,mm1				// compare the data with the exp pat.
					psrlq	mm3,16				// shift so we can get both result in the low Dword
					movd	edx,mm3				// move the result to real reg where we can do real compare
					cmp		edx,0FFFFFFFFh
					jne		error2
					movq	mm3,mm5				// fresh data to cmp
					
					add		edi, esi
					sub		ebx,8
					jnz		loop1r
					
					mov		bStatus, 0			// passing status
					jmp		bye
error1:
				mov		bStatus, 1			// error in the low pat
					mov		dwAddress,edi
					movq	rec,mm0
					movq	exp,mm4
					jmp		bye
error2:
				mov		bStatus, 1			// error in the high pat
					mov		dwAddress,edi
					movq	rec,mm1
					movq	exp,mm5
bye:
				pop		esi
					pop		edi
					pop		edx
					pop		ecx
					pop		ebx
					pop		eax
					emms
			}
			ReportDebug(2,L"Finished Reading\n");
			
			if (bStatus == 1)
			{
				// This test returns a special error message to flag OQM
				DecodeFailingLocation(1,(LPDWORD)dwAddress, (QWORD) exp, (QWORD) rec, 8, NULL);
				/*		ReportError(0x88,L"Pattern miscompare at address %ph buffer index %I64uh\n"
				L"Expected value=%016I64Xh   Received value=%016I64Xh\n",
				
				  real ? (LPVOID)real : (LPVOID)dwAddress,
				  (LPBYTE)dwAddress - pb,
				  exp,
				  rec,
				pThreadParams->ProcNumber);*/
			}
			
			if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
				Progress(++nProgCur * 100 / nProgMax);
		} // Patterns
	} // windows
	return 0;
}

//******************************************************************************
// Function name	: RambusCell
// Description	    : Writes random DWORDs and their complement to every DWORD
// Return type		: DWORD - Always 0.
// Argument         : LPTHREADPARAMS pThreadParams
//                    Uses the MemorySize and WindowSize parameters only.
//******************************************************************************
DWORD CXModuleMemory::RambusCell(LPTHREADPARAMS pThreadParams)
{
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	DWORD            MemorySize,WindowSize,nProgCur,nProgMax;
	LPVOID           pMem = pThreadParams->pMemory;
	LPBYTE           pbMax;
	LPDWORD          pdwMax;
	LPTESTCONDITIONS pParameters;
	BYTE bAbortStatus;
	
	pParameters   = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize    = pParameters->MemorySize;
	WindowSize    = pParameters->WindowSize;
	fProc0        = (pThreadParams->ProcNumber <= 0);
	
	LPDWORD pdw;
	DWORD dwBlockSize;
	DWORD dwAddress;
	BYTE bPatterns;
	BYTE bDevices;
	BYTE bStatus;
	BYTE bBusWords = 4;
	_int64 pat0;
	_int64 pat1;
	_int64 zero	= 0;
	_int64 exp;
	_int64 rec;
	_int64 TransCount = 0;
	
	if(!MMXCheck())
	{
		errMMX();
		return 0;
	}
	
	if(WindowSize == 0)
		WindowSize = MemorySize;
	
	// MemorySize must be a multiple of WindowSize
	MemorySize -= MemorySize % WindowSize;
	
	// Compute end test address.
	pbMax = (LPBYTE)pMem + MemorySize;
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = MemorySize / WindowSize * 16;  // 2 bDevice loops, 4 bPattern loops, 2 Prog calls.
	nProgCur = 0;
	
	// Walk through memory a window at a time.
	for(LPBYTE pb = (LPBYTE)pMem; pb < pbMax; pb += WindowSize)
	{
		// Compute the end of the window.
		pdwMax = (LPDWORD)pb + WindowSize / 4;
		
		pdw = (LPDWORD)pb;
		dwBlockSize = WindowSize / 2;
		
		// seq  through all possible rambus device configurations
		for(bDevices = 4; bDevices < 8; bDevices = (BYTE)(bDevices + 2))
		{
			// 4 set of surround patterns
			for(bPatterns = 0; bPatterns<4; bPatterns++)
			{
				if(bPatterns == 0)
				{
					pat0=0xffff000000000000;
					pat1=0xAAAA000000000000;
				}
				if(bPatterns == 1)
				{
					pat0=0xffff000000000000;
					pat1=0x5555000000000000;
				}
				if(bPatterns == 2)
				{
					pat0=0xaaaa000000000000;
					pat1=0xFFFF000000000000;
				}
				if(bPatterns == 3)
				{
					pat0=0x5555000000000000;
					pat1=0xFFFF000000000000;
				}
				if(bPatterns == 4)
				{
					pat0=0x0000000000000000;
					pat1=0xAAAA000000000000;
				}
				if(bPatterns == 5)
				{
					pat0=0x0000000000000000;
					pat1=0x5555000000000000;
				}
				if(bPatterns == 6)
				{
					pat0=0xaaaa000000000000;
					pat1=0x0000000000000000;
				}
				if(bPatterns == 7)
				{
					pat0=0x5555000000000000;
					pat1=0x0000000000000000;
				}
				if(bPatterns == 4)
				{
					pat0=0xffff000000000000;
					pat1=0xAAAA000000000000;
				}
				
				TransCount = TransCount + WindowSize * 2; // write read 1 block
				
				_asm{
					emms
						push	eax
						push	ebx
						push	ecx
						push	edx
						push	edi
						movq	mm0,zero
						movq	mm1,pat0
						movq	mm2,pat1
						mov     al,bDevices
						mov		ebx, dwBlockSize
						mov		cl, bBusWords
						mov		ch,cl				// load the bus count
						mov		edi, pb				// load the starting memory address
loop0:				mov		ah, al				// reset the device count
loop1:				dec		ebx					// dec the size count
					jz		end					// check to see if all memory done
					psrlq	mm0,16				// move the pat1 into waveform
					por		mm0,mm1
					dec		ch					// dec the bus count
					jnz		nomove				// if bus count is 0 do the move
					movq	[edi], mm0
					mov		ch,cl				// reload the bus count
					add		edi,8				// inc the dest address
nomove:
					dec		ah					// dec the device count
						jnz		loop1
						
						mov		ah, al				//reset the device count
loop2:				dec		ebx
					jz		end
					psrlq	mm0,16				// move the pat1 into waveform
					por		mm0,mm2
					dec		ch					// dec the bus count
					jnz		nomove2				// if bus count is 0 do the move
					movq	[edi], mm0
					mov		ch,cl				// reload the bus count
					add		edi,8				// inc the dest address
nomove2:
					dec		ah					// dec the device count
						jnz		loop2
						jmp		loop0
end:				pop		edi
					pop		edx
					pop		ecx
					pop		ebx
					pop		eax
					emms
				}
				//check for abort status
				bAbortStatus = CheckAbort(0);
				if(bAbortStatus == TRUE)
					throw((int)99);
				if(g_fFaultInject)
					*(pdwMax - 3) = 0xDEADBEEF;
				
				if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
					Progress(++nProgCur * 100 / nProgMax);
				
				_asm{
					emms
						push	eax
						push	ebx
						push	ecx
						push	edx
						push	edi
						movq	mm0,zero
						movq	mm1,pat0
						movq	mm2,pat1
						mov     al,bDevices
						mov		ebx, dwBlockSize
						mov		cl, bBusWords
						mov		ch,cl				// load the bus count
						mov		edi, pb				// load the starting memory address
						movq	mm4, [edi]			// we pre fetch the data way before we need it
loop0r:	mov		ah, al				// reset the device count
loop1r:	dec		ebx					// dec the size count
		jz		endr				// check to see if all memory done
		psrlq	mm0,16				// move the pat1 into waveform
		por		mm0,mm1
		dec		ch					// dec the bus count
		jnz		nomover				// if bus count is 0 do the move
		movq	mm3,mm4				// make a copy of the data so that well will still have the original if there is an error
		mov		ch,cl				// reload the bus count. This is ordered here so in can be same clock
		pcmpeqd mm3,mm0				// compare the pefetched data with the exp pat.
		psrlq	mm3,16				// shift so we can get both result in the low Dword
		movd	edx,mm3				// move the result to real reg where we can do real compare
		cmp		edx,0FFFFFFFFh
		jne		error
		add		edi,8				// inc the dest address
		movq	mm4,[edi]			// we pre fetch the data way before we need it
nomover:
					dec		ah					// dec the device count
						jnz		loop1r
						
						mov		ah, al				//reset the device count
loop2r:				dec		ebx
					jz		endr
					psrlq	mm0,16				// move the pat1 into waveform
					por		mm0,mm2
					dec		ch					// dec the bus count
					jnz		nomove2r			// if bus count is 0 do the move
					movq	mm3,mm4				// make a copy of the data so that well will still have the original if there is an error
					mov		ch,cl				// reload the bus count. This is ordered here so in can be same clock
					pcmpeqd mm3,mm0				// compare the pefetched data with the exp pat.
					psrlq	mm3,16				// shift so we can get both result in the low Dword
					movd	edx,mm3				// move the result to real reg where we can do real compare
					cmp		edx,0FFFFFFFFh
					jne		error
					add		edi,8				// inc the dest address
					movq	mm4, [edi]			// we pre fetch the data way before we need it
nomove2r:
					dec		ah					// dec the device count
						jnz		loop2r
						jmp		loop0r
endr:				mov		bStatus, 0
					jmp		bye
error:				mov		bStatus, 1
					mov		dwAddress,edi
					movq	rec,mm4
					movq	exp,mm0
bye:				pop		edi
					pop		edx
					pop		ecx
					pop		ebx
					pop		eax
					emms
				}
				
				
				if (bStatus == 1)
				{
					DecodeFailingLocation(1,(LPDWORD)dwAddress, (QWORD) exp, (QWORD) rec, 8, NULL);
					/*				errDataMiscomparePQQQI(
					real ? (LPVOID)real : (LPVOID)dwAddress,
					(LPBYTE)dwAddress - pb,
					exp,
					rec,
					pThreadParams->ProcNumber);*/
				}
				
				if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
					Progress(++nProgCur * 100 / nProgMax);
			} // Patterns
		} //devices
	} // windows
	
	return 0;
}
//******************************************************************************
// Function name	: WriteOrder
// Description	    : Writes random DWORDs and their complement to every DWORD
// Return type		: DWORD - Always 0.
// Argument         : LPTHREADPARAMS pThreadParams
//                    Uses the MemorySize and WindowSize parameters only.
//******************************************************************************
DWORD CXModuleMemory::WriteOrder(LPTHREADPARAMS pThreadParams)
{
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	DWORD            MemorySize,WindowSize,nProgCur,nProgMax;
	LPVOID           pMem = pThreadParams->pMemory;
	LPBYTE           pbMax;
	LPDWORD          pdwMax;
	LPTESTCONDITIONS pParameters;
	BYTE bAbortStatus;
	
	pParameters   = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize    = pParameters->MemorySize;
	WindowSize    = pParameters->WindowSize;
	fProc0        = (pThreadParams->ProcNumber <= 0);
	
	LPDWORD			pdw;
	DWORD			dwBlockSize;
	DWORD			dwAddress;
	DWORD			dwDeviceStride,dwDeviceStride2;
	BYTE			bStatus;
	BYTE			bPatterns;
	WORD			pat0;
	WORD			pat1;
	WORD			exp;
	WORD			rec;
	QWORD			TransCount = 0;
	QWORD			qwWriteOrder0 = 0xFFFFFFFFFFFFFFFF;
	QWORD			qwWriteOrder1 = 0xFFFFFFFFFFFFFFFF;
	QWORD			qwWriteOrder2 = 0xFFFFFFFFFFFFFFFF;
	QWORD			qwWriteOrder3 = 0xFFFFFFFFFFFFFFFF;
	BYTE			bGood,bOrder[32],bRvalue = 0;
	DWORD			dwi,dwj,dwk;
	
	if(!MMXCheck())
	{
		errMMX();
		return 0;
	}
	if(WindowSize == 0)
		WindowSize = MemorySize;
	
	// MemorySize must be a multiple of WindowSize
	MemorySize -= MemorySize % WindowSize;
	
	// Compute end test address.
	pbMax = (LPBYTE)pMem + MemorySize;
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = MemorySize / WindowSize * 32; // 4 devices, 4 patterns, 2 calls.
	nProgCur = 0;
	
	ReSeed(pThreadParams->ProcNumber);
	
	// Walk through memory a window at a time.
	for(LPBYTE pb = (LPBYTE)pMem; pb < pbMax; pb += WindowSize)
	{
		// Compute the end of the window.
		pdwMax = (LPDWORD)pb + WindowSize / 4;
		pdw = (LPDWORD)pb;
		dwBlockSize = WindowSize / 2;  //bytes to words - the asm code counts words
		
		for(dwDeviceStride = 4; dwDeviceStride < 8; dwDeviceStride++) //it must be 32 not 33 because we are using 0xFF to trigger the end of the pattern at we need to leave room for the last 0xFF
		{
			dwBlockSize = (MemorySize / dwDeviceStride - 1) / 2;
			
			// 4 set of surround patterns
			for(bPatterns = 0; bPatterns < 4; bPatterns++)
			{
				// init the byte order array
				for(dwi = 0; dwi < 32; dwi++)
					bOrder[dwi] = 0xFF;
				
				// fill out the byte order array with uniq values are need for the
				// number of devices we are testing
				for(dwj = 0; dwj < dwDeviceStride; dwj++)
				{
					bGood = FALSE;
					while(!bGood)
					{
						bGood = TRUE;
						// Generate a random byte order write
						bRvalue = (BYTE)(random() % dwDeviceStride);
						// Compare this against every other byte order write
						// we have all ready generated
						for(dwk = 0; dwk < dwDeviceStride; dwk++)
						{
							// if is the same keep gen another one
							if(bOrder[dwk] == bRvalue)
							{
								bGood= FALSE;
								break;
							}
						}
					}
					
					bOrder[dwj] = bRvalue;
				}
				
				// convert the order indexes to words
				for(dwj=0; dwj<dwDeviceStride; dwj++)
				{
					if (bOrder[dwj] != 0xFF)
						bOrder[dwj] = (BYTE)(bOrder[dwj]*2);
				}
				
				// load the write write ordering bytes into QWORD for loading
				// into the MMX regs.  Also convert the indexes to words - *2
				for(dwj = 0; dwj < 8; dwj++)
				{
					qwWriteOrder0 = ((qwWriteOrder0 >> 8) & 0x00FFFFFFFFFFFFFF) | (((QWORD)bOrder[dwj]   ) << 56);
					qwWriteOrder1 = ((qwWriteOrder1 >> 8) & 0x00FFFFFFFFFFFFFF) | (((QWORD)bOrder[dwj+8] ) << 56);
					qwWriteOrder2 = ((qwWriteOrder2 >> 8) & 0x00FFFFFFFFFFFFFF) | (((QWORD)bOrder[dwj+16]) << 56);
					qwWriteOrder3 = ((qwWriteOrder3 >> 8) & 0x00FFFFFFFFFFFFFF) | (((QWORD)bOrder[dwj+24]) << 56);
				}
				
				if(bPatterns == 0)
				{
					pat0=0xFFFF;
					pat1=0x0000;
				}
				if(bPatterns == 1)
				{
					pat0=0x0000;
					pat1=0xFFFF;
				}
				if(bPatterns == 2)
				{
					pat0=0xAAAA;
					pat1=0x5555;
				}
				if(bPatterns == 3)
				{
					pat0=0x5555;
					pat1=0xAAAA;
				}
				if(bPatterns > 3)
				{
					pat0=(WORD)random();
					pat1=(WORD)~pat0;
				}
				
				dwDeviceStride2 = dwDeviceStride * 2;
				TransCount = TransCount + WindowSize * 2; // write read 1 block
				
				__asm {
					// mm4 - mm5 Write order bytes
					// mm0 - mm3 Copies write order bytes
					// eax - Data pattern pat0 high 16 bits pat1 low16 bits
					// ebx - max memory address
					// ecx - device order tmp copy
					// edx - Write order index counter
					// ecx - write order address offset
					// edi - Current address
					// esi -
					emms
						push	eax
						push	ebx
						push	ecx
						push	edx
						push	edi
						push	esi
						movq	mm4,qwWriteOrder0
						movq	mm5,qwWriteOrder1
						movq	mm6,qwWriteOrder2
						movq	mm7,qwWriteOrder3
						mov		ax, pat0
						shl		eax,16
						mov		ax, pat1
						mov		ebx, pbMax
						mov		edi, pb
						mov     esi,dwDeviceStride2
						sub		edi, esi			// correct for first loop
nextstr:
					
					// note this loop is unrolled for speed and to save regs
					// byte 0
					mov		ax, pat0
						shl		eax,16
						mov		ax, pat1
						
						movq	mm0,mm4				// fresh copy of write orders
						movq	mm1,mm5
						movq	mm2,mm6
						movq	mm3,mm7
						add		edi,esi				// inc by the stride
loop0:	//Byte 0
					rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		end
						mov		[ecx],ax
						
						// byte 1
						rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		end
						mov		[ecx],ax
						
						// byte 2
						rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		end
						mov		[ecx],ax
						
						// byte 3
						rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		end
						mov		[ecx],ax
						
						// byte 4
						rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		end
						mov		[ecx],ax
						
						// byte 7
						rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		end
						mov		[ecx],ax
						
						// byte 6
						rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		end
						mov		[ecx],ax
						
						// byte 7
						rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		end
						mov		[ecx],ax
						
						
						movq	mm0,mm1				// shift up the order count
						movq	mm1,mm2
						movq	mm2,mm3
						
						jmp		loop0
end:
						pop		esi
							pop		edi
							pop		edx
							pop		ecx
							pop		ebx
							pop		eax
							emms
				}
				//check for abort status
				bAbortStatus = CheckAbort(0);
				if(bAbortStatus == TRUE)
					throw((int)99);
				(0);
				if(g_fFaultInject)
					*(pdwMax - 1) = 0xDEADBEEF;
				
				if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
					Progress(++nProgCur * 100 / nProgMax);
				
				__asm {
					
					// mm4 - mm5 Write order bytes
					// mm0 - mm3 Copies write order bytes
					// eax - Data pattern pat0 high 16 bits pat1 low16 bits
					// ebx - max memory address
					// ecx - device order tmp copy
					// edx - Write order index counter
					// ecx - write order address offset
					// edi - Current address
					// esi -
					emms
						push	eax
						push	ebx
						push	ecx
						push	edx
						push	edi
						push	esi
						movq	mm4,qwWriteOrder0
						movq	mm5,qwWriteOrder1
						movq	mm6,qwWriteOrder2
						movq	mm7,qwWriteOrder3
						mov		ax, pat0
						shl		eax,16
						mov		ax, pat1
						mov		ebx, pbMax
						mov		edi, pb
						mov     esi,dwDeviceStride2
						sub		edi, esi			// correct for first loop
nextstrr:
					mov		ax, pat0
						shl		eax,16
						mov		ax, pat1
						
						// note this loop is unrolled for speed and to save regs
						movq	mm0,mm4				// fresh copy of write orders
						movq	mm1,mm5
						movq	mm2,mm6
						movq	mm3,mm7
						add		edi,esi				// inc by the stride
loop0r:	// byte 0
					rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstrr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		endr
						mov		dx,[ecx]
						cmp		ax,dx
						jne		error
						
						// byte 1
						rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstrr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		endr
						mov		dx,[ecx]
						cmp		dx,ax
						jne		error
						
						// byte 2
						rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstrr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		endr
						mov		dx,[ecx]
						cmp		dx,ax
						jne		error
						
						// byte 3
						rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstrr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		endr
						mov		dx,[ecx]
						cmp		dx,ax
						jne		error
						
						// byte 4
						rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstrr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		endr
						mov		dx,[ecx]
						cmp		dx,ax
						jne		error
						
						// byte 7
						rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstrr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		endr
						mov		dx,[ecx]
						cmp		dx,ax
						jne		error
						
						// byte 6
						rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstrr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		endr
						mov		dx,[ecx]
						cmp		dx,ax
						jne		error
						
						// byte 7
						rol		eax,16				// shift our data pattern
						movd	ecx,mm0				// get the write order from the mmx reg
						and		ecx, 0x000000FF
						cmp		ecx, 0x000000FF
						je		nextstrr
						add		ecx, edi			// generate the working address
						psrlq	mm0,8				// for next byte n - pilelining for speed
						cmp		ecx, ebx			// the max address?
						jge		endr
						mov		dx,[ecx]
						cmp		dx,ax
						jne		error
						
						movq	mm0,mm1				// shift up the order count
						movq	mm1,mm2
						movq	mm2,mm3
						
						jmp		loop0r
						
endr:
						mov		bStatus, 0			// passing status
							jmp		bye
error:
						mov		bStatus, 1			// error in the low pat
							mov		dwAddress,edi
							mov		rec,dx
							mov		exp,ax
							jmp		bye
							
bye:
						pop		esi
							pop		edi
							pop		edx
							pop		ecx
							pop		ebx
							pop		eax
							emms
				}
				
				if (bStatus == 1)
				{
					DecodeFailingLocation(1,(LPDWORD)dwAddress, (QWORD) exp, (QWORD) rec, 8, NULL);
					/*					errDataMiscomparePQWWQQQQLI(
					real ? (LPVOID)real : (LPVOID)dwAddress,
					(LPBYTE)dwAddress - pb,
					exp,
					rec,
					qwWriteOrder0,
					qwWriteOrder1,
					qwWriteOrder2,
					qwWriteOrder3,
					GetRandomSeed(),
					pThreadParams->ProcNumber);*/
				}
				
				if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
					Progress(++nProgCur * 100 / nProgMax);
				
			} // Patterns
		} //device stride
	} // windows
	
	return 0;
}

//******************************************************************************
// Function name	: AlternatingAddress
// Description	    : Writes random DWORDs and their complement to every DWORD
// Return type		: DWORD - Always 0.
// Argument         : LPTHREADPARAMS pThreadParams
//                    Uses the MemorySize and WindowSize parameters only.
//******************************************************************************
DWORD CXModuleMemory::AlternatingAddress(LPTHREADPARAMS pThreadParams)
{
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	DWORD            MemorySize,WindowSize,nProgCur,nProgMax;
	LPVOID           pMem = pThreadParams->pMemory;
	LPBYTE           pbMax;
	LPDWORD          pdwMax;
	LPTESTCONDITIONS pParameters;
	BYTE bAbortStatus;
	
	pParameters   = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize    = pParameters->MemorySize;
	WindowSize    = pParameters->WindowSize;
	fProc0        = (pThreadParams->ProcNumber <= 0);
	
	LPDWORD			pdw;
	DWORD			dwBlockSize;
	DWORD			dwAddress;
	DWORD			dwDeviceStride,dwDeviceStride2;
	BYTE			bStatus;
	BYTE			bPatterns;
	WORD			pat0=0;
	WORD			pat1=0;
	WORD			exp;
	WORD			rec;
	QWORD			TransCount = 0;
	
	
	if(WindowSize == 0)
		WindowSize = MemorySize;
	
	// MemorySize must be a multiple of WindowSize
	MemorySize -= MemorySize % WindowSize;
	
	// Compute end test address.
	pbMax = (LPBYTE)pMem + MemorySize;
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = MemorySize / WindowSize * 64; // 8 strides, 4 patterns, 2 calls.
	nProgCur = 0;
	
	ReSeed(pThreadParams->ProcNumber);
	
	// Walk through memory a window at a time.
	for(LPBYTE pb = (LPBYTE)pMem; pb < pbMax; pb += WindowSize)
	{
		// Compute the end of the window.
		pdwMax = (LPDWORD)pb + WindowSize / 4;
		pdw = (LPDWORD)pb;
		dwBlockSize = WindowSize/2;  //bytes to words - the asm code counts words
		
		for(dwDeviceStride = 1006; dwDeviceStride<1014; dwDeviceStride++) //it must be 32 not 33 because we are using 0xFF to trigger the end of the pattern at we need to leave room for the last 0xFF
		{
			dwBlockSize = (MemorySize/dwDeviceStride-1)/2;
			
			// 4 set of surround patterns
			for(bPatterns = 0; bPatterns<4; bPatterns++)
			{
				if(bPatterns == 0)
				{
					pat0=0xFFFF;
					pat1=0x0000;
				}
				if(bPatterns == 1)
				{
					pat0=0x0000;
					pat1=0xFFFF;
				}
				if(bPatterns == 2)
				{
					pat0=0xAAAA;
					pat1=0x5555;
				}
				if(bPatterns == 3)
				{
					pat0=0x5555;
					pat1=0xAAAA;
				}
				if(bPatterns > 3)
				{
					pat0=(WORD)random();
					pat1=(WORD)~pat0;
				}
				
				dwDeviceStride2 = dwDeviceStride*4;    // 2 word pre stride
				TransCount = TransCount+WindowSize*2; // write read 1 block
				
				_asm{
					
					// mm4 - mm5 Write order bytes
					// mm0 - mm3 Copies write order bytes
					// eax - Data pattern pat0 high 16 bits pat1 low16 bits
					// ebx - max memory address
					// ecx - device order tmp copy
					// edx - Write order index counter
					// ecx - write order address offset
					// edi - Current address
					// esi -
					
					emms
						push	eax
						push	ebx
						push	ecx
						push	edx
						push	edi
						push	esi
						mov		ax, pat0
						shl		eax,16
						mov		ax, pat1
						mov		ebx, pbMax
						mov		edi, pb
						mov     esi,dwDeviceStride2
						
						
						mov		ax, pat0			// load the high low data pattern into eax
						shl		eax,16
						mov		ax, pat1
						
loop0:				// the low address
					rol		eax,16				// shift our data pattern
						cmp		edi, ebx			// the max address?
						jge		end
						mov		[edi],ax
						
						
						// The high address
						rol		eax,16				// shift our data pattern
						mov		ecx, edi			// generate the working address
						add		ecx, esi
						add		ecx, 2
						cmp		ecx, ebx			// the max address?
						jge		end
						mov		[ecx],ax
						
						add		edi,4				// next address skip the high address written
						
						jmp		loop0
						
end:
					pop		esi
						pop		edi
						pop		edx
						pop		ecx
						pop		ebx
						pop		eax
						emms
				}
				//check for abort status
				bAbortStatus = CheckAbort(0);
				if(bAbortStatus == TRUE)
					throw((int)99);
				
				if(g_fFaultInject)
					*(pdwMax - 1) = 0xDEADBEEF;
				
				if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
					Progress(++nProgCur * 100 / nProgMax);
				
				_asm{
					// mm4 - mm5 Write order bytes
					// mm0 - mm3 Copies write order bytes
					// eax - Data pattern pat0 high 16 bits pat1 low16 bits
					// ebx - max memory address
					// ecx - device order tmp copy
					// edx - Write order index counter
					// ecx - write order address offset
					// edi - Current address
					// esi -
					
					emms
						push	eax
						push	ebx
						push	ecx
						push	edx
						push	edi
						push	esi
						mov		ax, pat0
						shl		eax,16
						mov		ax, pat1
						mov		ebx, pbMax
						mov		edi, pb
						mov     esi,dwDeviceStride2
						
						
						mov		ax, pat0			// load the high low data pattern into eax
						shl		eax,16
						mov		ax, pat1
						
loop0r:				// the low address
					rol		eax,16				// shift our data pattern
						cmp		edi, ebx			// the max address?
						jge		endr
						mov		dx,[edi]
						cmp		ax,dx
						jne		error
						
						// The high address
						rol		eax,16				// shift our data pattern
						mov		ecx, edi			// generate the working address
						add		ecx, esi
						add		ecx, 2
						cmp		ecx, ebx			// the max address?
						jge		endr
						mov		dx,[ecx]
						cmp		ax,dx
						jne		error
						
						add		edi,4				// next address skip the high address written
						
						jmp		loop0r
						
endr:				mov		bStatus, 0			// passing status
					jmp		bye
error:				mov		bStatus, 1			// error in the low pat
					mov		dwAddress,edi
					mov		rec,dx
					mov		exp,ax
					jmp		bye
					
bye:				pop		esi
					pop		edi
					pop		edx
					pop		ecx
					pop		ebx
					pop		eax
					emms
				}
				
				if (bStatus == 1)
				{
					DecodeFailingLocation(1,(LPDWORD)dwAddress, (QWORD) exp, (QWORD) rec, 8, NULL);
					/*				errDataMiscomparePQWWLI(
					real ? (LPVOID)real : (LPVOID)dwAddress,
					(LPBYTE)dwAddress - pb,
					exp,
					rec,
					GetRandomSeed(),
					pThreadParams->ProcNumber);*/
				}
				
				if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
					Progress(++nProgCur * 100 / nProgMax);
				
			} // Patterns
		} //device stride
	} // windows
	
	return 0;
}
//******************************************************************************
// Function name	: AlternatingPatterns
// Description	    : Writes random DWORDs and their complement to every DWORD
// Return type		: DWORD - Always 0.
// Argument         : LPTHREADPARAMS pThreadParams
//                    Uses the MemorySize and WindowSize parameters only.
//******************************************************************************
DWORD CXModuleMemory::AlternatingPatterns(LPTHREADPARAMS pThreadParams)
{
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	DWORD            MemorySize,WindowSize,nProgCur,nProgMax;
	LPVOID           pMem = pThreadParams->pMemory;
	LPBYTE           pbMax;
	LPDWORD          pdwMax;
	LPTESTCONDITIONS pParameters;
	LPDWORD          pdw;
	DWORD            dwBlockSize;
	DWORD            dwAddress;
	DWORD            dwStrideOffset,dwStrideOffset2;
	DWORD            dwDeviceStride,dwDeviceStride2;
	BYTE             bStatus;
	BYTE             bPatterns;
	BYTE             bWaveCount;
	BYTE             bWaveForm;
	BYTE             bShifter;
	QWORD            qwLow;
	QWORD            pat0 = 0;
	QWORD            pat1 = 0;
	QWORD            exp;
	QWORD            rec;
	QWORD            TransCount = 0;
	BYTE bAbortStatus;
	
	pParameters = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize  = pParameters->MemorySize;
	WindowSize  = pParameters->WindowSize;
	fProc0      = (pThreadParams->ProcNumber <= 0);
	
	if(!MMXCheck())
	{
		errMMX();
		return 0;
	}
	if(WindowSize == 0)
		WindowSize = MemorySize;
	
	// MemorySize must be a multiple of WindowSize
	MemorySize -= MemorySize % WindowSize;
	
	// Compute end test address.
	pbMax = (LPBYTE)pMem + MemorySize;
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = MemorySize / WindowSize * 180;	// 5 device, 4+5+6+7+8 strides, 6 patterns.
	nProgCur = 0;
	
	// Walk through memory a window at a time.
	for(LPBYTE pb = (LPBYTE)pMem; pb < pbMax; pb += WindowSize)
	{
		// Compute the end of the window.
		pdwMax = (LPDWORD)pb + WindowSize / 4;
		
		pdw = (LPDWORD)pb;
		dwBlockSize = WindowSize/2;  //bytes to words - the asm code counts words
		for(dwDeviceStride = 4; dwDeviceStride<9; dwDeviceStride++)
		{
			for(dwStrideOffset = 0; dwStrideOffset < dwDeviceStride; dwStrideOffset++)
			{
				dwBlockSize = ((MemorySize-dwStrideOffset)/dwDeviceStride-1)/2;
				// 4 set of surround patterns
				for(bPatterns = 0; bPatterns<6; bPatterns++)
				{
					for(bWaveCount = 0; bWaveCount < 2; bWaveCount++)
					{
						if(bWaveCount == 0)
							bWaveForm = 0xAA;
						else
							bWaveForm = 0x55;
						
						if(bPatterns == 0)
						{
							pat0=0xFFFF;
							pat1=0x0000;
						}
						
						if(bPatterns == 1)
						{
							pat0=0x5555;
							pat1=0xAAAA;
						}
						
						if(bPatterns == 3)
						{
							pat0=((_int64)random()) & 0x000000000000FFFF;
							pat1=~pat0 & 0x000000000000FFFF;
						}
						
						if(bPatterns == 4)
						{
							pat0=((_int64)random()) & 0x000000000000FFFF;
							pat1=~pat0 & 0x000000000000FFFF;
						}
						
						if(bPatterns == 5)
						{
							pat0=((_int64)random()) & 0x000000000000FFFF;
							pat1=~pat0 & 0x000000000000FFFF;
						}
						
						qwLow = 0;
						for(bShifter = 0; bShifter < 4; bShifter++)
						{
							// shift the low qword
							qwLow = qwLow << 16;
							
							// add new wave form data
							if (((bWaveForm << bShifter) & 0x80) == 0x80)
								qwLow = qwLow | pat0;
							else
								qwLow = qwLow | pat1;
						}
						
						dwStrideOffset2 = dwStrideOffset * 2;			//scale word vs. bytes;
						dwDeviceStride2 = dwDeviceStride * 2;
						TransCount      = TransCount+WindowSize * 2;	// write read 1 block
						
						_asm{
							emms
								push eax
								push ebx
								push ecx
								push edx
								push edi
								push esi
								movq mm0,qwLow
								mov  ebx,dwBlockSize
								mov  edi,pb              // load the starting memory address
								add  edi,dwStrideOffset2 // offset the pattern start to the selected devices.  We pick on up the missing bits on the wrap.
								mov  esi,dwDeviceStride2 // A 32 bit copy of the number of devices
								
loop0:
							movq [edi],mm0           // write
								add  edi,esi             // inc the dest address by the number of devices on the bus.  This will cause to hit the same devies on the next write
								dec  ebx                 // dec the size count
								jz   end
								jmp  loop0
								
end:
							pop  esi
								pop  edi
								pop  edx
								pop  ecx
								pop  ebx
								pop  eax
								emms
						}
						
						if(g_fFaultInject)
							*(pdwMax - 1) = 0xDEADBEEF;
						
						// keep alive
						//check for abort status
						bAbortStatus = CheckAbort(0);
						if(bAbortStatus == TRUE)
							throw((int)99);
						
						
						_asm{
							emms
								emms
								push    eax
								push    ebx
								push    ecx
								push    edx
								push    edi
								push    esi
								movq    mm2,qwLow
								movq    mm4,qwLow
								mov     ebx, dwBlockSize
								mov     edi, pb             // load the starting memory address
								add     edi,dwStrideOffset2 // offset the pattern start to the selected devices.  We pick on up the missing bits on the wrap.
								mov     esi,dwDeviceStride2 // A 32 bit copy of the number of devices
loop1r:
							movq    mm0, [edi]
								pcmpeqd mm2,mm0             // compare the data with the exp pat.
								psrlq   mm2,16              // shift so we can get both result in the low Dword
								movd    edx,mm2             // move the result to real reg where we can do real compare
								cmp     edx,0FFFFFFFFh
								jne     error1
								movq    mm2,mm4             // fresh data to cmp
								
								add     edi, esi            // Inc address and dec count
								dec     ebx
								jnz     loop1r
								
								mov     bStatus, 0          // passing status
								jmp     bye
error1:
							mov     bStatus, 1          // error in the low pat
								mov     dwAddress,edi
								movq    rec,mm0
								movq    exp,mm4
								jmp     bye
								
bye:
							pop     esi
								pop     edi
								pop     edx
								pop     ecx
								pop     ebx
								pop     eax
								emms
						}
						
						if(bStatus == 1)
						{
							DecodeFailingLocation(1,(LPDWORD)dwAddress, (QWORD) exp, (QWORD) rec, 8, NULL);
							/*						errDataMiscomparePQQQI(
							real ? (LPVOID)real : (LPVOID)dwAddress,
							(LPBYTE)dwAddress - pb,
							exp,
							rec,
							pThreadParams->ProcNumber);*/
						}
						// keep alive
						//check for abort status
						bAbortStatus = CheckAbort(0);
						if(bAbortStatus == TRUE)
							throw((int)99);
					} // Wave count
					
					if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
						Progress(++nProgCur * 100 / nProgMax);
				} // Patterns
			} //device offset
		} //device stride
	} // windows
	
	return 0;
}

void CXModuleMemory::TestPerformance(_int64 count)
{
	_int64 utime;
	HANDLE hThread;             // specifies the thread of interest
	FILETIME CreationTime;      // when the thread was created
	FILETIME ExitTime;          // when the thread was destroyed
	FILETIME KernelTime;        // time the thread has spent in kernel mode
	FILETIME UserTime;          // time the thread has spent in user mode);
	
	hThread = (HANDLE)GetCurrentThread();
	
	GetThreadTimes(
		hThread,
		&CreationTime,
		&ExitTime,
		&KernelTime,
		&UserTime
		);
	
	utime = (((_int64)UserTime.dwHighDateTime <<32) +(_int64)UserTime.dwLowDateTime);
	
	ReportDebug(1,L"thread run time %I64dmS, %I64d Megabytes transfered %I64d Megabytes/sec",
		utime/(_int64)10000,
		count/0x100000,
		(count/(utime/(_int64)10000)*1000)/0x100000);
}

#if !defined(WIN64)
//******************************************************************************
// Function name	: BusNoise
// Description	    : Writes random DWORDs and their complement to every DWORD
// Return type		: DWORD - Always 0.
// Argument         : LPTHREADPARAMS pThreadParams
//                    Uses the MemorySize and WindowSize parameters only.
//******************************************************************************
DWORD CXModuleMemory::BusNoise(LPTHREADPARAMS pThreadParams)
{
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	DWORD            MemorySize,WindowSize,nProgCur,nProgMax;
	LPVOID           pMem = pThreadParams->pMemory;
	LPBYTE           pbMax;
	LPDWORD          pdwMax;
	LPTESTCONDITIONS pParameters;
	BYTE bAbortStatus;
	
	pParameters   = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize    = pParameters->MemorySize;
	WindowSize    = pParameters->WindowSize;
	fProc0        = (pThreadParams->ProcNumber <= 0);
	
	LPDWORD pdw;
	DWORD dwBlockSize;
	DWORD dwAddress;
	BYTE bPatterns;
	BYTE bStatus;
	BYTE bShifter = 0;
	QWORD qwLow, qwHigh;
	BYTE bWaveForm;
	DWORD	dwBusWords =8;
	_int64 pat0 = 0;
	_int64 pat1 = 0;
	_int64 exp;
	_int64 rec;
	_int64 TransCount = 0;
	
	if(!MMXCheck())
	{
		errMMX();
		return 0;
	}
	
	if(WindowSize == 0)
		WindowSize = MemorySize;
	
	// MemorySize must be a multiple of WindowSize
	MemorySize -= MemorySize % WindowSize;
	
	// Compute end test address.
	pbMax = (LPBYTE)pMem + MemorySize;
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = MemorySize / WindowSize * 2*254; // 2*254 patterns * waveforms
	nProgCur = 0;
	
	// Walk through memory a window at a time.
	for(LPBYTE pb = (LPBYTE)pMem; pb < pbMax; pb += WindowSize)
	{
		// Compute the end of the window.
		pdwMax = (LPDWORD)pb + WindowSize / 4;
		
		pdw = (LPDWORD)pb;
		dwBlockSize = WindowSize / 2;  //bytes to words - the asm code counts words
		// the pattern is 8 words wides so skew by 8 to get all possible combo's
		for(bPatterns = 0; bPatterns<2; bPatterns++)
		{
			// Code in here to do a more than 2 waveform but for time
			// reasons this is set to 2 now.
			for(bWaveForm = 1; bWaveForm < 255; bWaveForm++)
			{
				if(bPatterns == 0)
				{
					pat0=0xFFFF;
					pat1=0x0000;
				}
				if(bPatterns == 1)
				{
					pat0=0x5555;
					pat1=0xAAAA;
				}
				if(bPatterns == 3)
				{
					pat0=((_int64)random()) & 0x000000000000FFFF;
					pat1=~pat0 & 0x000000000000FFFF;
				}
				if(bPatterns == 4)
				{
					pat0=((_int64)random()) & 0x000000000000FFFF;
					pat1=~pat0 & 0x000000000000FFFF;
				}
				if(bPatterns == 5)
				{
					pat0=((_int64)random()) & 0x000000000000FFFF;
					pat1=~pat0 & 0x000000000000FFFF;
				}
				qwHigh = 0;
				qwLow = 0;
				for(bShifter =0; bShifter <8; bShifter++)
				{
					//shift the high qword
					qwHigh = qwHigh << 16;
					
					// add the low qword carry to the high qword
					qwHigh = qwHigh | ((0xFFFF000000000000 & qwLow)>>48);
					
					// shift the low qword
					qwLow = qwLow << 16;
					
					// add new wave form data
					
					if (((bWaveForm << bShifter) & 0x80) == 0x80)
						qwLow = qwLow | pat0;
					else
						qwLow = qwLow | pat1;
				}
				
				TransCount = TransCount + WindowSize * 2; // write read 1 block
				
				__asm {
					emms
						push	eax
						push	ebx
						push	ecx
						push	edx
						push	edi
						push	esi
						mov		ebx, dwBlockSize
						mov		edx, dwBusWords
						movq	mm0,qwLow
						movq	mm1,qwHigh
						mov		edi, pb				// load the starting memory address
loop1:
					movq	[edi], mm0
						add		edi, edx
						movq	[edi], mm1
						add		edi, edx
						sub		ebx,8
						jnz		loop1
						
						pop		esi
						pop		edi
						pop		edx
						pop		ecx
						pop		ebx
						pop		eax
						emms
				}
				
				if(g_fFaultInject)
					*(pdwMax - 1) = 0xDEADBEEF;
				
				//check for abort status
				bAbortStatus = CheckAbort(0);
				if(bAbortStatus == TRUE)
					throw((int)99);
				
				__asm {
					emms
						push	eax
						push	ebx
						push	ecx
						push	edx
						push	edi
						push	esi
						mov		ebx, dwBlockSize
						mov		esi, dwBusWords
						movq	mm2,qwLow			// what to compare
						movq	mm3,qwHigh
						movq	mm4,qwLow			// make a copy for exp pat
						movq	mm5,qwHigh
						
						mov		edi, pb				// load the starting memory address
loop1r:
					movq	mm0, [edi]
						pcmpeqd mm2,mm0				// compare the data with the exp pat.
						psrlq	mm2,16				// shift so we can get both result in the low Dword
						movd	edx,mm2				// move the result to real reg where we can do real compare
						cmp		edx,0FFFFFFFFh
						jne		error1
						movq	mm2,mm4				// fresh data to cmp
						add		edi, esi
						
						movq	mm1, [edi]
						pcmpeqd mm3,mm1				// compare the data with the exp pat.
						psrlq	mm3,16				// shift so we can get both result in the low Dword
						movd	edx,mm3				// move the result to real reg where we can do real compare
						cmp		edx,0FFFFFFFFh
						jne		error2
						movq	mm3,mm5				// fresh data to cmp
						
						add		edi, esi
						sub		ebx,8
						jnz		loop1r
						
						mov		bStatus, 0			// passing status
						jmp		bye
error1:
					mov		bStatus, 1			// error in the low pat
						mov		dwAddress,edi
						movq	rec,mm0
						movq	exp,mm5
						jmp		bye
error2:
					mov		bStatus, 1			// error in the high pat
						mov		dwAddress,edi
						movq	rec,mm1
						movq	exp,mm6
bye:
					pop		esi
						pop		edi
						pop		edx
						pop		ecx
						pop		ebx
						pop		eax
						emms
				}
				
				if (bStatus == 1)
				{
					DecodeFailingLocation(1,(LPDWORD)dwAddress, (QWORD) exp, (QWORD) rec, 8, NULL);
					/*			errDataMiscomparePQQQI(
					real ? (LPVOID)real : (LPVOID)dwAddress,
					(LPBYTE)dwAddress - pb,
					exp,
					rec,
					pThreadParams->ProcNumber);*/
				}
				
				if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
					Progress(++nProgCur * 100 / nProgMax);
			} //waveforms
		} // Patterns
	} // windows
	
	TestPerformance(TransCount);
	
	return 0;
}

//******************************************************************************
// Function name	: Memory32K
// Description	    : Writes random DWORDs and their complement to every DWORD
// Return type		: DWORD - Always 0.
// Argument         : LPTHREADPARAMS pThreadParams
//                    Uses the MemorySize and WindowSize parameters only.
//******************************************************************************
DWORD CXModuleMemory::Memory32K(LPTHREADPARAMS pThreadParams)
{
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	DWORD            MemorySize,WindowSize,nProgCur,nProgMax;
	LPVOID           pMem = pThreadParams->pMemory;
	LPBYTE           pbMax;
	LPDWORD          pdwMax;
	LPTESTCONDITIONS pParameters;
	BYTE bAbortStatus;
	
	pParameters   = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize    = pParameters->MemorySize;
	WindowSize    = pParameters->WindowSize;
	fProc0        = (pThreadParams->ProcNumber <= 0);
	
	LPDWORD			pdw;
	LPDWORD			dwBlockMax;
	DWORD			dwAddress;
	DWORD			dwDeviceStride;
	BYTE			bStatus;
	BYTE			bPatterns;
	BYTE			bWaveCount;
	BYTE			bWaveForm;
	BYTE			bShifter;
	QWORD			qwLow;
	QWORD			pat0 = 0;
	QWORD			pat1 = 0;
	QWORD			exp;
	QWORD			rec;
	QWORD			TransCount = 0;
	
	if(!MMXCheck())
	{
		errMMX();
		return 0;
	}
	
	if(WindowSize == 0)
		WindowSize = MemorySize;
	
	// MemorySize must be a multiple of WindowSize
	MemorySize -= MemorySize % WindowSize;
	
	// Compute end test address.
	pbMax = (LPBYTE)pMem + MemorySize;
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = MemorySize / WindowSize * 12; // 2 waves, 6 patterns
	nProgCur = 0;
	
	ReSeed(pThreadParams->ProcNumber);
	
	// Walk through memory a window at a time.
	for(LPBYTE pb = (LPBYTE)pMem; pb < pbMax; pb += WindowSize)
	{
		// Compute the end of the window.
		pdwMax = (LPDWORD)pb + WindowSize / 4;
		
		pdw = (LPDWORD)pb;
		
		// 4 set of surround patterns
		for(bPatterns = 0; bPatterns<6; bPatterns++)
		{
			for(bWaveCount = 0; bWaveCount < 2; bWaveCount++)
			{
				if(bWaveCount == 0)
					bWaveForm = 0xAA;
				else
					bWaveForm = 0x55;
				
				if(bPatterns == 0)
				{
					pat0=0xFFFF;
					pat1=0x0000;
				}
				if(bPatterns == 1)
				{
					pat0=0x5555;
					pat1=0xAAAA;
				}
				if(bPatterns == 3)
				{
					pat0=((_int64)random()) & 0x000000000000FFFF;
					pat1=~pat0 & 0x000000000000FFFF;
				}
				if(bPatterns == 4)
				{
					pat0=((_int64)random()) & 0x000000000000FFFF;
					pat1=~pat0 & 0x000000000000FFFF;
				}
				if(bPatterns == 5)
				{
					pat0=((_int64)random()) & 0x000000000000FFFF;
					pat1=~pat0 & 0x000000000000FFFF;
				}
				qwLow = 0;
				for(bShifter =0; bShifter <4; bShifter++)
				{
					// shift the low qword
					qwLow = qwLow << 16;
					
					// add new wave form data
					
					if (((bWaveForm << bShifter) & 0x80) == 0x80)
						qwLow = qwLow | pat0;
					else
						qwLow = qwLow | pat1;
					
				}
				
				for(dwDeviceStride = 0x8000; dwDeviceStride<0x8001; dwDeviceStride++)
				{
					dwBlockMax = pdwMax - 8; // 8 byte writes
					TransCount = TransCount+WindowSize*2; // write read 1 block
					
					_asm{
						//      mm0 - the data pattern
						//		eax	- copy of the starting address pb
						//      ebx - the stopping address
						//		ecx - the offset count
						//		edx
						//		edi - the current address
						//		esi - the stride
						emms
							push	eax
							push	ebx
							push	ecx
							push	edx
							push	edi
							push	esi
							movq	mm0,qwLow
							mov		ebx,dwBlockMax
							mov		eax,pb				// load the starting memory address
							mov     esi,dwDeviceStride	// the access stride
							sub		ecx,ecx				// set starting offsetto 0
							
loop00:
						mov		edi, eax			// load the starting memory address
							add		edi, ecx			// offset it
loop0:
						movq	[edi], mm0			// write
							add		edi,esi				// inc the dest address by the  stride.
							cmp		edi, ebx			// compare the next address with the max address
							jl		loop0				// done if the max address in <= next address
							
							add		ecx,8				// inc the stride offset, 8 byte writes
							cmp		ecx,esi				// compare the stide and the current offset
							jl		loop00				// if the stride is <= to the next offset we are done.
							
							pop		esi
							pop		edi
							pop		edx
							pop		ecx
							pop		ebx
							pop		eax
							emms
					}
					
					if(g_fFaultInject)
						*(pdwMax - 9) = 0xDEADBEEF;
					
					//check for abort status
					bAbortStatus = CheckAbort(0);
					if(bAbortStatus == TRUE)
						throw((int)99);
					
					_asm{
						emms
							emms
							push	eax
							push	ebx
							push	ecx
							push	edx
							push	edi
							push	esi
							movq	mm2,qwLow
							movq	mm4,qwLow
							mov		ebx,dwBlockMax
							mov		eax,pb				// load the starting memory address
							mov     esi,dwDeviceStride	// the access stride
							sub		ecx,ecx				// set starting offsetto 0
loop00r:
						mov		edi, eax			// load the starting memory address
							add		edi, ecx			// offset it
							
loop1r:
						movq	mm0, [edi]
							pcmpeqd mm2,mm0				// compare the data with the exp pat.
							psrlq	mm2,16				// shift so we can get both result in the low Dword
							movd	edx,mm2				// move the result to real reg where we can do real compare
							cmp		edx,0FFFFFFFFh
							jne		error1
							movq	mm2,mm4				// fresh data to cmp
							
							add		edi,esi				// inc the dest address by the  stride.
							cmp		edi, ebx			// compare the next address with the max address
							jl		loop1r				// done if the max address in <= next address
							
							add		ecx,8				// inc the stride offset, 8 byte writes
							cmp		ecx,esi				// compare the stide and the current offset
							jl		loop00r				// if the stride is <= to the next offset we are done.
							
							mov		bStatus, 0			// passing status
							jmp		bye
error1:
						mov		bStatus, 1			// error in the low pat
							mov		dwAddress,edi
							movq	rec,mm0
							movq	exp,mm4
							jmp		bye
bye:
						pop		esi
							pop		edi
							pop		edx
							pop		ecx
							pop		ebx
							pop		eax
							emms
					}
					
					if (bStatus == 1)
					{
						DecodeFailingLocation(1,(LPDWORD)dwAddress, (QWORD) exp, (QWORD) rec, 8, NULL);
						/*						errDataMiscomparePQQQI(
						real ? (LPVOID)real : (LPVOID)dwAddress,
						(LPBYTE)dwAddress - pb,
						exp,
						rec,
						pThreadParams->ProcNumber);*/
					}
					
					//check for abort status
					bAbortStatus = CheckAbort(0);
					if(bAbortStatus == TRUE)
						throw((int)99);
				} // Patterns
				
				if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
					Progress(++nProgCur * 100 / nProgMax);
				
			} //device offset
		} //device stride
	} // windows
	
	return 0;
}
//******************************************************************************
// Function name	: Powersof2
// Description	    : Writes random DWORDs and their complement to every DWORD
// Return type		: DWORD - Always 0.
// Argument         : LPTHREADPARAMS pThreadParams
//                    Uses the MemorySize and WindowSize parameters only.
//******************************************************************************
DWORD CXModuleMemory::Powersof2(LPTHREADPARAMS pThreadParams)
{
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	DWORD            MemorySize,WindowSize,nProgCur,nProgMax;
	LPVOID           pMem = pThreadParams->pMemory;
	LPBYTE           pbMax;
	LPDWORD          pdwMax;
	LPTESTCONDITIONS pParameters;
	BYTE bAbortStatus;
	
	pParameters   = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize    = pParameters->MemorySize;
	WindowSize    = pParameters->WindowSize;
	fProc0        = (pThreadParams->ProcNumber <= 0);
	
	LPDWORD			pdw;
	LPDWORD			dwBlockMax;
	DWORD			dwAddress;
	DWORD			dwDeviceStride;
	BYTE			bStatus;
	BYTE			bPatterns;
	BYTE			bWaveCount;
	BYTE			bWaveForm;
	BYTE			bShifter;
	QWORD			qwLow;
	QWORD			pat0 = 0;
	QWORD			pat1 = 0;
	QWORD			exp;
	QWORD			rec;
	QWORD			TransCount = 0;
	
	if(!MMXCheck())
	{
		errMMX();
		return 0;
	}
	
	if(WindowSize == 0)
		WindowSize = MemorySize;
	
	// MemorySize must be a multiple of WindowSize
	MemorySize -= MemorySize % WindowSize;
	
	// Compute end test address.
	pbMax = (LPBYTE)pMem + MemorySize;
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = MemorySize / WindowSize * 88; // 11 strides, 2 patterns, 2 waves, 2 calls
	nProgCur = 0;
	
	ReSeed(pThreadParams->ProcNumber);
	
	// Walk through memory a window at a time.
	for(LPBYTE pb = (LPBYTE)pMem; pb < pbMax; pb += WindowSize)
	{
		// Compute the end of the window.
		pdwMax = (LPDWORD)pb + WindowSize / 4;
		
		pdw = (LPDWORD)pb;
		for(dwDeviceStride = 0x20; dwDeviceStride<0x10000; dwDeviceStride = dwDeviceStride*2)
		{
			// 2 set of surround patterns.  Code in here for more than that
			// but for time reasons we only running 2 for now
			for(bPatterns = 0; bPatterns<2; bPatterns++)
			{
				for(bWaveCount = 0; bWaveCount < 2; bWaveCount++)
				{
					if(bWaveCount == 0)
						bWaveForm = 0xAA;
					else
						bWaveForm = 0x55;
					
					if(bPatterns == 0)
					{
						pat0=0xFFFF;
						pat1=0x0000;
					}
					if(bPatterns == 1)
					{
						pat0=0x5555;
						pat1=0xAAAA;
					}
					if(bPatterns == 3)
					{
						pat0=((_int64)random()) & 0x000000000000FFFF;
						pat1=~pat0 & 0x000000000000FFFF;
					}
					if(bPatterns == 4)
					{
						pat0=((_int64)random()) & 0x000000000000FFFF;
						pat1=~pat0 & 0x000000000000FFFF;
					}
					if(bPatterns == 5)
					{
						pat0=((_int64)random()) & 0x000000000000FFFF;
						pat1=~pat0 & 0x000000000000FFFF;
					}
					qwLow = 0;
					for(bShifter =0; bShifter <4; bShifter++)
					{
						// shift the low qword
						qwLow = qwLow << 16;
						
						// add new wave form data
						
						if (((bWaveForm << bShifter) & 0x80) == 0x80)
							qwLow = qwLow | pat0;
						else
							qwLow = qwLow | pat1;
						
					}
					
					dwBlockMax = pdwMax - 8; // 8 byte writes
					TransCount = TransCount+WindowSize*2; // write read 1 block
					//check for abort status
					bAbortStatus = CheckAbort(0);
					if(bAbortStatus == TRUE)
						throw((int)99);
					
					_asm{
						//      mm0 - the data pattern
						//		eax	- copy of the starting address pb
						//      ebx - the stopping address
						//		ecx - the offset count
						//		edx
						//		edi - the current address
						//		esi - the stride
						emms
							push	eax
							push	ebx
							push	ecx
							push	edx
							push	edi
							push	esi
							movq	mm0,qwLow
							mov		ebx,dwBlockMax
							mov		eax,pb				// load the starting memory address
							mov     esi,dwDeviceStride	// the access stride
							sub		ecx,ecx				// set starting offsetto 0
							
loop00:
						mov		edi, eax			// load the starting memory address
							add		edi, ecx			// offset it
loop0:
						movq	[edi], mm0			// write
							add		edi,esi				// inc the dest address by the  stride.
							cmp		edi, ebx			// compare the next address with the max address
							jl		loop0				// done if the max address in <= next address
							
							add		ecx,8				// inc the stride offset, 8 byte writes
							cmp		ecx,esi				// compare the stide and the current offset
							jl		loop00				// if the stride is <= to the next offset we are done.
							
							pop		esi
							pop		edi
							pop		edx
							pop		ecx
							pop		ebx
							pop		eax
							emms
					}
					
					//check for abort status
					bAbortStatus = CheckAbort(0);
					if(bAbortStatus == TRUE)
						throw((int)99);
					if(g_fFaultInject)
						*(pdwMax - 9) = 0xDEADBEEF;
					
					if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
						Progress(++nProgCur * 100 / nProgMax);
					
					_asm{
						emms
							emms
							push	eax
							push	ebx
							push	ecx
							push	edx
							push	edi
							push	esi
							movq	mm2,qwLow
							movq	mm4,qwLow
							mov		ebx,dwBlockMax
							mov		eax,pb				// load the starting memory address
							mov     esi,dwDeviceStride	// the access stride
							sub		ecx,ecx				// set starting offsetto 0
loop00r:
						mov		edi, eax			// load the starting memory address
							add		edi, ecx			// offset it
							
loop1r:
						movq	mm0, [edi]
							pcmpeqd mm2,mm0				// compare the data with the exp pat.
							psrlq	mm2,16				// shift so we can get both result in the low Dword
							movd	edx,mm2				// move the result to real reg where we can do real compare
							cmp		edx,0FFFFFFFFh
							jne		error1
							movq	mm2,mm4				// fresh data to cmp
							
							add		edi,esi				// inc the dest address by the  stride.
							cmp		edi, ebx			// compare the next address with the max address
							jl		loop1r				// done if the max address in <= next address
							
							add		ecx,8				// inc the stride offset, 8 byte writes
							cmp		ecx,esi				// compare the stide and the current offset
							jl		loop00r				// if the stride is <= to the next offset we are done.
							
							mov		bStatus, 0			// passing status
							jmp		bye
error1:
						mov		bStatus, 1			// error in the low pat
							mov		dwAddress,edi
							movq	rec,mm0
							movq	exp,mm4
							jmp		bye
bye:
						pop		esi
							pop		edi
							pop		edx
							pop		ecx
							pop		ebx
							pop		eax
							emms
					}
					
					if (bStatus == 1)
					{
						DecodeFailingLocation(1,(LPDWORD)dwAddress, (QWORD) exp, (QWORD) rec, 8, NULL);
						/*				errDataMiscomparePQQQI(
						real ? (LPVOID)real : (LPVOID)dwAddress,
						(LPBYTE)dwAddress - pb,
						exp,
						rec,
						pThreadParams->ProcNumber);*/
					}
					
					if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
						Progress(++nProgCur * 100 / nProgMax);
					
				} // wave
			} // device pattern
		} // device stride
	} // windows
	
	return 0;
}

#endif	// WIN64

//******************************************************************************
// Function name	: AddressPatterns
// Description	    : Each DWORD is written with its address and its address's
//                    complement to check for uniqueness.
// Return type		: DWORD - Always returns 0.
// Argument         : LPTHREADPARAMS pThreadParams
//                    Only uses the MemorySize parameter.
//******************************************************************************
DWORD CXModuleMemory::AddressPatterns(LPTHREADPARAMS pThreadParams)
{
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	DWORD_PTR        MemorySize;
	DWORD            nProgCur,nProgMax;
	LPVOID           pMem = pThreadParams->pMemory;
	LPDWORD          pdwMax;
	LPTESTCONDITIONS pParameters;
	BYTE bAbortStatus;
	
	pParameters = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize  = pParameters->MemorySize;
	fProc0      = (pThreadParams->ProcNumber <= 0);
	
	// Compute end test address.
	pdwMax = (LPDWORD)pMem + MemorySize / 4;
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = 4;
	nProgCur = 0;
	
	// Write each DWORD with its address.
	for(LPDWORD pdw = (LPDWORD)pMem; pdw < pdwMax; pdw++)
		*pdw = (DWORD)(DWORD_PTR)pdw;
	
	
	if(g_fFaultInject)
		*(pdwMax - 1) = 0xDEADBEEF;
	
	if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
		Progress(++nProgCur * 100 / nProgMax);
	
	// Check for errors.
	for(pdw = (LPDWORD)pMem; pdw < pdwMax; pdw++)
	{
		if(*pdw != (DWORD)(DWORD_PTR)pdw)
		{
			DecodeFailingLocation(1,(LPDWORD)pdw, (QWORD) pdw, (QWORD) *pdw, 4, NULL);
			
			/*			errDataMiscomparePQLLI(
			real ? (LPVOID)real : pdw,
			(LPBYTE)pdw - (LPBYTE)pMem,
			(DWORD)(DWORD_PTR)pdw,
			*pdw,
			pThreadParams->ProcNumber);*/
		}
	}
	
	if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
		Progress(++nProgCur * 100 / nProgMax);
	
	// Write each DWORD with its address's complement.
	for(pdw = (LPDWORD)pMem; pdw < pdwMax; pdw++)
		*pdw = ~(DWORD)(DWORD_PTR)pdw;
	
	if(g_fFaultInject)
		*(pdwMax - 1) = 0xDEADBEEF;
	//check for abort status
	bAbortStatus = CheckAbort(0);
	if(bAbortStatus == TRUE)
		throw((int)99);
	
	if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
		Progress(++nProgCur * 100 / nProgMax);
	
	// Check for errors.
	for(pdw = (LPDWORD)pMem; pdw < pdwMax; pdw++)
	{
		if(*pdw != ~(DWORD)(DWORD_PTR)pdw)
		{
			DecodeFailingLocation(1,(LPDWORD)pdw, (QWORD) pdw, (QWORD) *pdw, 4, NULL);
			
			/*			errDataMiscomparePQLLI(
			real ? (LPVOID)real : pdw,
			(LPBYTE)pdw - (LPBYTE)pMem,
			(DWORD)(DWORD_PTR)pdw,
			*pdw,
			pThreadParams->ProcNumber);*/
		}
	}
	
	return 0;
}


//******************************************************************************
// Function name	: MixedOpsMixAlignRandomOrder
// Description	    : Performs random byte, word, and dword accesses to fill and
//                    compare memory a block at a time.  Accesses can be both
//                    aligned and unaligned.
// Return type		: DWORD - Always 0
// Argument         : LPTHREADPARAMS pThreadParams
//                    MemorySize - how much memory to test.
//                    WindowSize - write this much before reading back.
//                    BlockSize  - a non-overlapping list of addresses and sizes
//                                 is generated for this size of memory.  The
//                                 block is filled with random values and written
//                                 to all of memory using the access list.
//******************************************************************************
DWORD CXModuleMemory::MixedOpsMixAlignRandomOrder(LPTHREADPARAMS pThreadParams)
{
	LPTESTCONDITIONS pParameters;
	DWORD_PTR        MemorySize,BlockSize,WindowSize,offset,nProgCur,nProgMax;
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	LPBYTE           pbSrcBlock,pbWinMax,pbBlockMax,pbVirt;
	LPVOID           pMem = pThreadParams->pMemory;
	LPDWORD          pdwMax;
	BYTE bAbortStatus;
	
	pParameters = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize  = pParameters->MemorySize;
	BlockSize   = pParameters->BlockSize;
	WindowSize  = pParameters->WindowSize;
	fProc0      = (pThreadParams->ProcNumber <= 0);
	
	// Seed the thread.  The seed is a fixed difference
	// from the generated by SetRandomSeed().
	ReSeed(pThreadParams->ProcNumber);
	
	// If WindowSize or BlockSize are not specified, use these assumptions.
	if(WindowSize == 0)
		WindowSize = MemorySize;
	
	if(BlockSize == 0)
		BlockSize = WindowSize;
	
	// Make WindowSize an even multiple of BlockSize
	// Make MemorySize an even multiple of WindowSize
	WindowSize -= WindowSize % BlockSize;
	MemorySize -= MemorySize % WindowSize;
	
	// Compute end test address.
	pbWinMax = (LPBYTE)pMem + MemorySize;
	
	// Allocate a block of memory to fill with a random pattern.
	pbSrcBlock = new BYTE[BlockSize];
	
	if(pbSrcBlock == NULL)
	{
		errOutOfMemoryQ(BlockSize);
		// exit
		throw CXmtaException(99,NULL);
	}
	
	// Create the stress object (allocates buffers for access algorithm)
	// Read and write accesses must be data-aligned on IA-64, so stressobj
	// is built for a smaller memory size and multiplied by 4 later to get
	// DWORD-aligned addresses.
	
	stress stressobj(BlockSize,this);
	
	
	// Compute the end of the test block.
	pdwMax = (LPDWORD)(pbSrcBlock + BlockSize);
	
	// Generate random data in the source buffer.
	for(LPDWORD pdw = (LPDWORD)pbSrcBlock; pdw < pdwMax; pdw++)
		*pdw = random();
	
	// Build the list of write and read accesses.
	stressobj.build_write_log();
	stressobj.build_read_log();
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax  = MemorySize / BlockSize * 4;
	nProgCur  = 0;
	
	// Test a window of memory at a time.
	for(LPBYTE pbWin = (LPBYTE)pMem; pbWin < pbWinMax; pbWin += WindowSize)
	{
		// Compute the end of the test window.
		pbBlockMax = pbWin + WindowSize;
		
		// Use the write list to write the random data buffer a block
		// at a time to the window being tested.
		for(LPBYTE pbBlock = pbWin; pbBlock < pbBlockMax; pbBlock += BlockSize)
		{
			stressobj.StressWrite(pbSrcBlock,pbBlock);
			
			if(fProc0)
				Progress((int)(++nProgCur * 100 / nProgMax));
		}
		
		if(g_fFaultInject)
			*(LPDWORD)(pbBlockMax - 4) = 0xDEADBEEF;
		//check for abort status
		bAbortStatus = CheckAbort(0);
		if(bAbortStatus == TRUE)
			throw((int)99);
		
		
		// Use the read list to read and compare memory with the data
		// buffer a block at a time across the window being tested.
		for(pbBlock = pbWin; pbBlock < pbBlockMax; pbBlock += BlockSize)
		{
			pbVirt = stressobj.StressRead(pbSrcBlock,pbBlock);
			
			if(pbVirt)
			{
				offset = pbVirt - pbBlock;
				
				// Round down to nearest DWORD address if we are at the end of our buffer
				// so we don't walk off the end.  Recalc the offset if needed.
				if(BlockSize - offset < 4)
				{
					DWORD_PTR tmp = (DWORD_PTR)pbVirt & ~0x3;
					pbVirt        = (LPBYTE)tmp;
					offset        = pbVirt - pbBlock;
				}
				
				DecodeFailingLocation(1,(LPDWORD)pbVirt, (QWORD) *(pbSrcBlock + offset), (QWORD) *pbVirt, 4, NULL);
				
				/*				errRndDataMiscomparePQLLLI(
				real ? (LPVOID)real : pbVirt,
				offset,
				*(LPDWORD)(pbSrcBlock + offset),
				*(LPDWORD)pbVirt,
				GetRandomSeed(),
				pThreadParams->ProcNumber);*/
			}
			
			if(fProc0)
				Progress((int)(++nProgCur * 100 / nProgMax));
		}
	}
	
	// Complement the data in th source block so we test all the bits.
	for(pdw = (LPDWORD)pbSrcBlock; pdw < pdwMax; pdw++)
		*pdw = ~*pdw;
	
	// Build different read and write logs.
	stressobj.build_write_log();
	stressobj.build_read_log();
	
	// Test the memory again the same as above.
	for(pbWin = (LPBYTE)pMem; pbWin < pbWinMax; pbWin += WindowSize)
	{
		pbBlockMax = pbWin + WindowSize;
		
		for(LPBYTE pbBlock = pbWin; pbBlock < pbBlockMax; pbBlock += BlockSize)
		{
			stressobj.StressWrite(pbSrcBlock,pbBlock);
			
			if(fProc0)
				Progress((int)(++nProgCur * 100 / nProgMax));
		}
		
		if(g_fFaultInject)
			*(LPDWORD)(pbBlockMax - 4) = 0xDEADBEEF;
		//check for abort status
		bAbortStatus = CheckAbort(0);
		if(bAbortStatus == TRUE)
			throw((int)99);
		
		for(pbBlock = pbWin; pbBlock < pbBlockMax; pbBlock += BlockSize)
		{
			pbVirt = stressobj.StressRead(pbSrcBlock,pbBlock);
			
			if(pbVirt)
			{
				offset = (DWORD)(pbVirt - pbBlock);
				
				// Round down to nearest DWORD address if we are at the end of our buffer
				// so we don't walk off the end.  Recalc the offset if needed.
				if(BlockSize - offset < 4)
				{
					DWORD_PTR tmp = (DWORD_PTR)pbVirt & ~0x3;
					pbVirt        = (LPBYTE)tmp;
					offset        = pbVirt - pbBlock;
				}
				
				DecodeFailingLocation(1,(LPDWORD)pbVirt, (QWORD) *(pbSrcBlock + offset), (QWORD) *pbVirt, 4, NULL);
				
				/*	errRndDataMiscomparePQLLLI(
				real ? (LPVOID)real : pbVirt,
				offset,
				*(LPDWORD)(pbSrcBlock + offset),
				*(LPDWORD)pbVirt,
				GetRandomSeed(),
				pThreadParams->ProcNumber);*/
			}
			
			if(fProc0)
				Progress((int)(++nProgCur * 100 / nProgMax));
		}
	}
	
	delete [] pbSrcBlock;
	return 0;
}


//******************************************************************************
// Function name	: DwordRandomOrder
// Description	    : Performs random dword accesses to fill and compare memory
//                    a block at a time.  Accesses can be both aligned and unaligned.
// Return type		: DWORD - Always 0
// Argument         : LPTHREADPARAMS pThreadParams
//                    MemorySize - how much memory to test.
//                    WindowSize - write this much before reading back.
//                    BlockSize  - a non-overlapping list of addresses and sizes
//                                 is generated for this size of memory.  The
//                                 block is filled with random values and written
//                                 to all of memory using the access list.
//******************************************************************************
DWORD CXModuleMemory::DwordRandomOrder(LPTHREADPARAMS pThreadParams)
{
	LPTESTCONDITIONS pParameters;
	DWORD_PTR        MemorySize,BlockSize,WindowSize,offset;
	DWORD_PTR        nProgCur,nProgMax;
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	LPBYTE           pbSrcBlock,pbWinMax,pbBlockMax,pbVirt;
	LPVOID           pMem = pThreadParams->pMemory;
	LPDWORD          pdwMax;
	BYTE bAbortStatus;
	
	pParameters = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize  = pParameters->MemorySize;
	BlockSize   = pParameters->BlockSize;
	WindowSize  = pParameters->WindowSize;
	fProc0      = (pThreadParams->ProcNumber <= 0);
	
	// Seed the thread.  The seed is a fixed difference
	// from the generated by SetRandomSeed().
	ReSeed(pThreadParams->ProcNumber);
	
	// If WindowSize or BlockSize are not specified, use these assumptions.
	if(WindowSize == 0)
		WindowSize = MemorySize;
	
	if(BlockSize == 0)
		BlockSize = WindowSize;
	
	// Make WindowSize an even multiple of BlockSize
	// Make MemorySize an even multiple of WindowSize
	WindowSize -= WindowSize % BlockSize;
	MemorySize -= MemorySize % WindowSize;
	
	// Allocate a block of memory to fill with a random pattern.
	pbSrcBlock = new BYTE[BlockSize];
	
	if(pbSrcBlock == NULL)
	{
		errOutOfMemoryQ(BlockSize);
		// exit
		throw CXmtaException(99,NULL);
	}
	
	// Create the stress object (allocates buffers for access algorithm)
	// Builds a list for DWORD accesses by dividing size by four.
	stress stressobj(BlockSize / 4,this);
	
	// Compute the end of the test block.
	pdwMax = (LPDWORD)(pbSrcBlock + BlockSize);
	
	// Generate random data in the source buffer.
	for(LPDWORD pdw = (LPDWORD)pbSrcBlock; pdw < pdwMax; pdw++)
		*pdw = random();
	
	// Build the list of write and read accesses.
	stressobj.build_write_log();
	stressobj.build_read_log();
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax  = MemorySize / BlockSize * 4;
	nProgCur  = 0;
	
	// Compute end test address.
	pbWinMax = (LPBYTE)pMem + MemorySize;
	
	// Test a window of memory at a time.
	for(LPBYTE pbWin = (LPBYTE)pMem; pbWin < pbWinMax; pbWin += WindowSize)
	{
		// Compute the end of the test window.
		pbBlockMax = pbWin + WindowSize;
		
		// Use the write list to write the random data buffer a block
		// at a time to the window being tested.
		for(LPBYTE pbBlock = pbWin; pbBlock < pbBlockMax; pbBlock += BlockSize)
		{
			stressobj.StressWriteDword(pbSrcBlock,pbBlock);
			
			if(fProc0)
				Progress((int)(++nProgCur * 100 / nProgMax));
		}
		
		if(g_fFaultInject)
			*(LPDWORD)(pbBlockMax - 4) = 0xDEADBEEF;
		//check for abort status
		bAbortStatus = CheckAbort(0);
		if(bAbortStatus == TRUE)
			throw((int)99);
		
		// Use the read list to read and compare memory with the data
		// buffer a block at a time across the window being tested.
		for(pbBlock = pbWin; pbBlock < pbBlockMax; pbBlock += BlockSize)
		{
			pbVirt = stressobj.StressReadDword(pbSrcBlock,pbBlock);
			
			if(pbVirt)
			{
				offset = pbVirt - pbBlock;
				DecodeFailingLocation(1,(LPDWORD)pbVirt, (QWORD) *(pbSrcBlock + offset), (QWORD) *pbVirt, 4, NULL);
				
				/*				errRndDataMiscomparePQLLLI(
				real ? (LPVOID)real : pbVirt,
				offset,
				*(LPDWORD)(pbSrcBlock + offset),
				*(LPDWORD)pbVirt,
				GetRandomSeed(),
				pThreadParams->ProcNumber);*/
			}
			
			if(fProc0)
				Progress((int)(++nProgCur * 100 / nProgMax));
		}
	}
	
	// Complement the data in th source block so we test all the bits.
	for(pdw = (LPDWORD)pbSrcBlock; pdw < pdwMax; pdw++)
		*pdw = ~*pdw;
	
	// Build different read and write logs.
	stressobj.build_write_log();
	stressobj.build_read_log();
	
	// Test the memory again the same as above.
	for(pbWin = (LPBYTE)pMem; pbWin < pbWinMax; pbWin += WindowSize)
	{
		pbBlockMax = pbWin + WindowSize;
		
		for(LPBYTE pbBlock = pbWin; pbBlock < pbBlockMax; pbBlock += BlockSize)
		{
			stressobj.StressWriteDword(pbSrcBlock,pbBlock);
			
			if(fProc0)
				Progress((int)(++nProgCur * 100 / nProgMax));
		}
		
		if(g_fFaultInject)
			*(LPDWORD)(pbBlockMax - 4) = 0xDEADBEEF;
		//check for abort status
		bAbortStatus = CheckAbort(0);
		if(bAbortStatus == TRUE)
			throw((int)99);
		
		for(pbBlock = pbWin; pbBlock < pbBlockMax; pbBlock += BlockSize)
		{
			pbVirt = stressobj.StressReadDword(pbSrcBlock,pbBlock);
			
			if(pbVirt)
			{
				offset = (DWORD)(pbVirt - pbBlock);
				DecodeFailingLocation(1,(LPDWORD)pbVirt, (QWORD) *(pbSrcBlock + offset), (QWORD) *pbVirt, 4, NULL);
				
				/*				errRndDataMiscomparePQLLLI(
				real ? (LPVOID)real : pbVirt,
				offset,
				*(LPDWORD)(pbSrcBlock + offset),
				*(LPDWORD)pbVirt,
				GetRandomSeed(),
				pThreadParams->ProcNumber);*/
			}
			
			if(fProc0)
				Progress((int)(++nProgCur * 100 / nProgMax));
		}
	}
	
	delete [] pbSrcBlock;
	return 0;
}


//******************************************************************************
// Function name	: MemoryTransfer
// Description	    : For each 256KB of memory, fill it with a random pattern and
//                    write it do disk, then read it back and verify the pattern
//                    is unchanged.  This can catch failures in the disk controller's
//                    ability to access all locations in memory.
// Return type		: void
//******************************************************************************
void CXModuleMemory::MemoryTransfer()
{
/*
#ifdef _XBOX
char          FileName[204] = "mem.dat";
#else
char          FileName[204] = "c:\\tdata\\00000000\MEDIA\\INTEL\\mem.dat";

  #endif
	*/
	//	_TCHAR          TempFilePath[2000];
	//	LPCTSTR			 TempDirStrPtr;		
	DWORD_PTR        MemorySize,WindowSize,nProgCur,nProgMax;
	DWORD            dwValue,dummy;
	LPVOID           pMem;
	LPBYTE           pbMax;
	LPDWORD          pdwMax;
	HANDLE           hFile;
	BYTE bAbortStatus;
	//	size_t			 length;
	char			 FileString[1024];
	//	LPCSTR			 FileStrPtr;
	LPCTSTR			 FileTStrPtr;
	//	TCHAR StrOutput[512];
	// Init the random number generator for this test.
	SetRandomSeed();
	
	FileTStrPtr = GetCfgString(L"MemoryTransferFile",L"mem.dat");
	if(FileTStrPtr == NULL)
	{
		errMissingParameterS(L"MemoryTransferFile");
		return;
	}
	
	ReportDebug(2,L"Temporary filename = %s\n",FileTStrPtr);
	
	sprintf(FileString,"%S",FileTStrPtr);
	
	// Create temp file with no read/write buffering.
	hFile = CreateFile(
		FileString,
		GENERIC_WRITE | GENERIC_READ,
		0,				// no file sharing.
		NULL,			// default sec.
		CREATE_ALWAYS,
		//		FILE_FLAG_NO_BUFFERING | FILE_FLAG_DELETE_ON_CLOSE,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	
	if(INVALID_HANDLE_VALUE == hFile)
	{
		errUserTempFileS(GetLastError(),(char *)FileString);
		// exit
		return;
	}
	
	// Test available memory in 1MB increments.
	MemorySize = ParameterMemorySize();
	
	// Starting test address.
	pMem = AllocateMemory(&MemorySize,ParameterSwitch(L"Force"),ParameterSwitch(L"NoCache"),1);
	
	// reduced from 0x100000 to prevent problems with windows 2000 not being able to perform operation.
	WindowSize = 0x40000;
	
	// MemorySize must be a multiple of WindowSize
	MemorySize -= MemorySize % WindowSize;
	
	// Compute end test address.
	pbMax = (LPBYTE)pMem + MemorySize;
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = MemorySize / WindowSize * 2;
	if(nProgMax < 2)
		nProgMax = 2;
	nProgCur = 0; 
	
	// Test memory 256K at a time.
	for(LPBYTE pb = (LPBYTE)pMem; pb < pbMax; pb += WindowSize)
	{
		ReportDebug(3,L"overhead");
		// Compute the end of the 256kb window.
		pdwMax = (LPDWORD)pb + WindowSize / 4;
		
		// Fill the window with random data.
		ReSeed(0);
		for(LPDWORD pdw = (LPDWORD)pb; pdw < pdwMax; pdw++)
			*pdw = random();
		
		// Rewind and write 256KB of data to the temp file.
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		
		if(!WriteFile(hFile,pb,(DWORD)WindowSize,&dummy,NULL))
			errWriteFile(GetLastError());
		
		// Flush the data, rewind the file, and read the data back.
		FlushFileBuffers(hFile);
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		
		if(!ReadFile(hFile,pb,(DWORD)WindowSize,&dummy,NULL))
			errReadFile(GetLastError());
		ReportDebug(3,L"writing");
		
		
		if(g_fFaultInject)
			*(pdwMax - 1) = 0xDEADBEEF;
		
		//check for abort status
		bAbortStatus = CheckAbort(0);
		if(bAbortStatus == TRUE)
			throw((int)99);
		
		// Compare the data with the original random pattern.
		ReSeed(0);
		for(pdw = (LPDWORD)pb; pdw < pdwMax; pdw++)
		{
			dwValue = random();
			if(*pdw != dwValue)
			{
				DecodeFailingLocation(1,(LPDWORD)pdw, (QWORD) dwValue, (QWORD) *pdw, 4, NULL);
				
				/*		errRndDataMiscomparePQLLLI(
				real ? (LPVOID)real : pdw,
				(LPBYTE)pdw - pb,
				dwValue,
				*pdw,
				GetRandomSeed(),
				-1);*/
			}
		}
		
		Progress((int)(++nProgCur * 100 / nProgMax));
		
		// Complement the random pattern and test again.
		ReSeed(0);
		for(pdw = (LPDWORD)pb; pdw < pdwMax; pdw++)
			*pdw = ~random();
		
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		
		if(!WriteFile(hFile,pb,(DWORD)WindowSize,&dummy,NULL))
			errWriteFile(GetLastError());
		
		FlushFileBuffers(hFile);
		SetFilePointer(hFile,0,0,FILE_BEGIN);
		
		if(!ReadFile(hFile,pb,(DWORD)WindowSize,&dummy,NULL))
			errReadFile(GetLastError());
		
		if(g_fFaultInject)
			*(pdwMax - 1) = 0xDEADBEEF;
		
		ReportDebug(3,L"reading");
		ReSeed(0);
		for(pdw = (LPDWORD)pb; pdw < pdwMax; pdw++)
		{
			dwValue = ~random();
			if(*pdw != dwValue)
			{
				DecodeFailingLocation(1,(LPDWORD)pdw, (QWORD) dwValue, (QWORD) *pdw, 4, NULL);
				
				/*			errRndDataMiscomparePQLLLI(
				real ? (LPVOID)real : pdw,
				(LPBYTE)pdw - pb,
				dwValue,
				*pdw,
				GetRandomSeed(),
				-1);*/
			}
		}
		
		Progress((int)(++nProgCur * 100 / nProgMax));
	}
	CloseHandle(hFile); 
	FreeMemory(pMem,MemorySize);
	DeleteFile(FileString);
	
}
//******************************************************************************
// Function name	: CachePerformace
// Description	    : Benchmarks Cache performance to see if cache is operational
// Return type		: void
//******************************************************************************
void CXModuleMemory::CachePerformance()
{
	_int64 utime,utime1, utime2;
	HANDLE hThread;             // specifies the thread of interest
	FILETIME CreationTime;      // when the thread was created
	FILETIME ExitTime;          // when the thread was destroyed
	FILETIME KernelTime;        // time the thread has spent in kernel mode
	FILETIME UserTime;          // time the thread has spent in user mode);
	bool     fForce = 1,fNoCache = 0;
	DWORD	 MemorySize = 0x1b000;  //110K of cache will be used
	LPVOID	 pMemory;				//pointer to memory allocation for benchmarking
	int		 x;
	DWORD	 QwordCount = 0;
	QWORD	 count = 0;
	bool status;
	DWORD	dwRUCL,dwRLCL,dwWUCL,dwWLCL,MBSec;
	
	//Read Params
	dwRUCL = GetCfgUint(L"CacheReadPerformanceUCL",0);
	if(dwRUCL == 0)
		errMissingParameterS(L"CacheReadPerformaceUCL missing or 0");
	dwRLCL = GetCfgUint(L"CacheReadPerformanceLCL",0);
	if(dwRLCL == 0)
		errMissingParameterS(L"CacheReadPerformaceLCL missing or 0");
	
	//Write Params
	dwWUCL = GetCfgUint(L"CacheWritePerformanceUCL",0);
	if(dwWUCL == 0)
		errMissingParameterS(L"CacheWritePerformaceUCL missing or 0");
	dwWLCL = GetCfgUint(L"CacheWritePerformanceLCL",0);
	if(dwWLCL == 0)
		errMissingParameterS(L"CacheWritePerformaceLCL missing or 0");
	
	pMemory = AllocateMemory(&MemorySize,fForce,fNoCache,1);
	
	hThread = GetCurrentThread();
	status = GetThreadTimes(
		hThread,
		&CreationTime,
		&ExitTime,
		&KernelTime,
		&UserTime
		);
	
#ifdef _XBOX
	utime1 = (((_int64)KernelTime.dwHighDateTime <<32) +(_int64)KernelTime.dwLowDateTime);
#else 
	utime1 = (((_int64)UserTime.dwHighDateTime <<32) +(_int64)UserTime.dwLowDateTime);
#endif	
	QwordCount = (DWORD)(MemorySize/8); // convert to qword moves
	for(x=1; x<100000;x++)
	{
		//Write test
		_asm{
			EMMS
				PUSH	ECX
				PUSH	EDI
				MOV		ECX, QwordCount
				MOV		EDI, pMemory
loop1:
			MOVQ	[EDI],MM0
				ADD		SI,8
				DEC		ECX
				JNZ		loop1
				POP		EDI
				POP		ECX
				EMMS
				
		}
	}
	count = x*MemorySize;
	
	status = GetThreadTimes(
		hThread,
		&CreationTime,
		&ExitTime,
		&KernelTime,
		&UserTime
		);
#ifdef _XBOX
	utime2 = (((_int64)KernelTime.dwHighDateTime <<32) +(_int64)KernelTime.dwLowDateTime);
#else 
	utime2 = (((_int64)UserTime.dwHighDateTime <<32) +(_int64)UserTime.dwLowDateTime);
#endif		
	utime = utime2 - utime1;
	ASSERT(utime != 0);	
	MBSec = (DWORD)((count/(utime/(_int64)10000)*1000)/0x100000);	
	ReportDebug(1,L"Cache write performace: %I64dmS, %I64d Megabytes transfered %I64d Megabytes/sec",
		utime/(_int64)10000,
		count/0x100000,
		MBSec);
	ReportStatistic(L"CACHEWP",L"%f",(float)MBSec);
	if((MBSec < dwWLCL) || (MBSec > dwWUCL))
	{
		errPerProblemSIII(L"Cache Write", MBSec, dwWLCL, dwWUCL);
	}
	
	
	//Read Test
	
	status = GetThreadTimes(
		hThread,
		&CreationTime,
		&ExitTime,
		&KernelTime,
		&UserTime
		);
#ifdef _XBOX
	utime1 = (((_int64)KernelTime.dwHighDateTime <<32) +(_int64)KernelTime.dwLowDateTime);
#else 
	utime1 = (((_int64)UserTime.dwHighDateTime <<32) +(_int64)UserTime.dwLowDateTime);
#endif	
	QwordCount = (DWORD)(MemorySize/8); // convert to qword moves
	for(x=1; x<100000;x++)
	{
		//Write test
		_asm{
			EMMS
				PUSH	ECX
				PUSH	EDI
				MOV		ECX, QwordCount
				MOV		EDI, pMemory
loop2:
			MOVQ	MM0,[EDI]
				ADD		SI,8
				DEC		ECX
				JNZ		loop2
				POP		EDI
				POP		ECX
				EMMS
				
		}
	}
	count = x*MemorySize;
	
	status = GetThreadTimes(
		hThread,
		&CreationTime,
		&ExitTime,
		&KernelTime,
		&UserTime
		);
#ifdef _XBOX
	utime2 = (((_int64)KernelTime.dwHighDateTime <<32) +(_int64)KernelTime.dwLowDateTime);
#else 
	utime2 = (((_int64)UserTime.dwHighDateTime <<32) +(_int64)UserTime.dwLowDateTime);
#endif	
	utime = utime2 - utime1;
	ASSERT(utime != 0);	
	//MB/sec = bytes in Time /(Time in 100ns * 10000scale to ms *1000scale to sec)/) 1MB
	MBSec = (DWORD)((count/(utime/(_int64)10000)*1000)/0x100000);
	
	ReportDebug(1,L"Cache read performace: %I64dmS, %I64d Megabytes transfered %I64d Megabytes/sec",
		utime/(_int64)10000,
		count/0x100000,
		MBSec);
	ReportStatistic(L"CACHERP",L"%f",(float)MBSec);
	
	if((MBSec < dwRLCL) || (MBSec > dwRUCL))
	{
		errPerProblemSIII(L"Cache Read", MBSec, dwRLCL, dwRUCL);
	}
	FreeMemory(pMemory,MemorySize);
	
}
//******************************************************************************
// Function name	: MemoryPerformance
// Description	    : Benchmarks memory performance to see if memory is operational.
// Return type		: void
//******************************************************************************
void CXModuleMemory::MemoryPerformance()
{
	_int64 utime,utime1, utime2;
	HANDLE hThread;             // specifies the thread of interest
	FILETIME CreationTime;      // when the thread was created
	FILETIME ExitTime;          // when the thread was destroyed
	FILETIME KernelTime;        // time the thread has spent in kernel mode
	FILETIME UserTime;          // time the thread has spent in user mode);
	bool     fForce = 1,fNoCache = 0;
	DWORD	 MemorySize = 0x1400000;  // The amount of memory to benchmark - 20Meg
	LPVOID	 pMemory;				//pointer to memory allocation for benchmarking
	int		 x;
	DWORD	 QwordCount = 0;
	QWORD	 count = 0;
	bool status;
	DWORD	dwRUCL,dwRLCL,dwWUCL,dwWLCL,MBSec;
	
	//Read params
	dwRUCL = GetCfgUint(L"MemoryReadPerformanceUCL",0);
	if(dwRUCL == 0)
		errMissingParameterS(L"MemoryReadPerformanceUCL missing or 0");
	dwRLCL = GetCfgUint(L"MemoryReadPerformanceLCL",0);
	if(dwRLCL == 0)
		errMissingParameterS(L"MemoryReadPerformanceLCL missing or 0");
	
	//Write params
	dwWUCL = GetCfgUint(L"MemoryWritePerformanceUCL",0);
	if(dwWUCL == 0)
		errMissingParameterS(L"MemoryWritePerformanceUCL missing or 0");
	dwWLCL = GetCfgUint(L"MemoryWritePerformanceLCL",0);
	if(dwWLCL == 0)
		errMissingParameterS(L"MemoryWritePerformanceLCL missing or 0");
	
	pMemory = AllocateMemory(&MemorySize,fForce,fNoCache,1);
	
	hThread = GetCurrentThread();
	status = GetThreadTimes(
		hThread,
		&CreationTime,
		&ExitTime,
		&KernelTime,
		&UserTime
		);
#ifdef _XBOX
	utime1 = (((_int64)KernelTime.dwHighDateTime <<32) +(_int64)KernelTime.dwLowDateTime);
#else 
	utime1 = (((_int64)UserTime.dwHighDateTime <<32) +(_int64)UserTime.dwLowDateTime);
#endif
	QwordCount = (DWORD)(MemorySize/16); // convert to qword moves
	for(x=0; x<10;x++)
	{
		//Write test
		_asm{
			EMMS
				PUSH	ECX
				PUSH	EDI
				MOV		ECX, QwordCount
				MOV		EDI, pMemory
				
loop1:
			MOVAPS	[EDI],XMM0
				ADD		EDI,16
				DEC		ECX
				JNZ		loop1
				POP		EDI
				POP		ECX
				EMMS
				
		}
	}
	count = x*MemorySize;
	
	status = GetThreadTimes(
		hThread,
		&CreationTime,
		&ExitTime,
		&KernelTime,
		&UserTime
		);
#ifdef _XBOX
	utime2 = (((_int64)KernelTime.dwHighDateTime <<32) +(_int64)KernelTime.dwLowDateTime);
#else 
	utime2 = (((_int64)UserTime.dwHighDateTime <<32) +(_int64)UserTime.dwLowDateTime);
#endif
	
	utime = utime2 - utime1;
	ASSERT(utime != 0);
	MBSec = (DWORD)((count/(utime/(_int64)10000)*1000)/0x100000);
	ReportDebug(1,L"Memory write performace: %I64dmS, %I64d Megabytes transfered %I64d Megabytes/sec",
		utime/(_int64)10000,
		count/0x100000,
		MBSec);
	ReportStatistic(L"MEMORYWP",L"%f",(float)MBSec);
	
	if((MBSec < dwWLCL) || (MBSec > dwWUCL))
	{
		errPerProblemSIII(L"Memory Write", MBSec, dwWLCL, dwWUCL);
	}
	
	status = GetThreadTimes(
		hThread,
		&CreationTime,
		&ExitTime,
		&KernelTime,
		&UserTime
		);
	
	//Read Test
#ifdef _XBOX
	utime1 = (((_int64)KernelTime.dwHighDateTime <<32) +(_int64)KernelTime.dwLowDateTime);
#else 
	utime1 = (((_int64)UserTime.dwHighDateTime <<32) +(_int64)UserTime.dwLowDateTime);
#endif	
	QwordCount = (DWORD)(MemorySize/16); // convert to qword moves
	for(x=0; x<10;x++)
	{
		_asm{
			EMMS
				PUSH	ECX
				PUSH	EDI
				MOV		ECX, QwordCount
				MOV		EDI, pMemory
				
loop2:
			MOVAPS	XMM0,[EDI]
				ADD		EDI,16
				DEC		ECX
				JNZ		loop2
				POP		EDI
				POP		ECX
				EMMS
				
		}
	}
	count = x*MemorySize;
	
	status = GetThreadTimes(
		hThread,
		&CreationTime,
		&ExitTime,
		&KernelTime,
		&UserTime
		);
	
#ifdef _XBOX
	utime2 = (((_int64)KernelTime.dwHighDateTime <<32) +(_int64)KernelTime.dwLowDateTime);
#else 
	utime2 = (((_int64)UserTime.dwHighDateTime <<32) +(_int64)UserTime.dwLowDateTime);
#endif
	
	utime = utime2 - utime1;
	ASSERT(utime != 0);	
	MBSec = (DWORD)((count/(utime/(_int64)10000)*1000)/0x100000);
	ReportDebug(1,L"Memory read performace: %I64dmS, %I64d Megabytes transfered %I64d Megabytes/sec",
		utime/(_int64)10000,
		count/0x100000,
		MBSec);
	ReportStatistic(L"MEMORYRP",L"%f",(float)MBSec);
	
	if((MBSec < dwRLCL) || (MBSec > dwRUCL))
	{
		errPerProblemSIII(L"Memory Read", MBSec, dwWLCL, dwWUCL);
	}	FreeMemory(pMemory,MemorySize);
}
//******************************************************************************
// Function name	: DPowersof2
// Description	    : 
// Return type		: DWORD - currently not used
//******************************************************************************
DWORD CXModuleMemory::DPowersof2(LPTHREADPARAMS pThreadParams, DWORD StrideStart, DWORD StrideEnd)
{
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	DWORD            MemorySize,WindowSize,nProgCur,nProgMax;
	LPVOID           pMem = pThreadParams->pMemory;
	LPBYTE           pbMax;
	LPDWORD          pdwMax;
	LPBYTE			 pbFail;
	LPTESTCONDITIONS pParameters;
	BYTE bAbortStatus;
	
	pParameters   = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize    = pParameters->MemorySize;
	WindowSize    = pParameters->WindowSize;
	fProc0        = (pThreadParams->ProcNumber <= 0);
	
	LPDWORD			pdw;
	DWORD			dwDeviceStride;
	BYTE			bPatterns;
	QWORD			exp=0;
	QWORD			rec=0;
	QWORD			TransCount = 0;
	DWORD			Data;
	DWORD			Address=0,AddressOffset=0;
	
	if(!MMXCheck())
	{
		errMMX();
		return 0;
	}
	
	if(WindowSize == 0)
		WindowSize = MemorySize;
	
	// MemorySize must be a multiple of WindowSize
	MemorySize -= MemorySize % WindowSize;
	
	// Compute end test address.
	pbMax = (LPBYTE)pMem + MemorySize;
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = MemorySize / WindowSize * 88; // 11 strides, 2 patterns, 2 waves, 2 calls
	nProgCur = 0;
	
	ReSeed(pThreadParams->ProcNumber);
	
	// Walk through memory a window at a time.
	for(LPBYTE pb = (LPBYTE)pMem; pb < pbMax; pb += WindowSize)
	{
		// Compute the end of the window.
		pdwMax = (LPDWORD)pb + WindowSize / 4;
		
		pdw = (LPDWORD)pb;
		for(dwDeviceStride = StrideStart; dwDeviceStride<=StrideEnd; dwDeviceStride = dwDeviceStride*2)
		{
			//check for abort status
			bAbortStatus = CheckAbort(0);
			if(bAbortStatus == TRUE)
				throw((int)99);
			
			// 0 and 1 in every cell
			for(bPatterns = 0; bPatterns<2; bPatterns++)
			{
				TransCount = TransCount+MemorySize*2;
				if(bPatterns)
					Data =0xFFFFFFFF;
				else
					Data =0x0;
				// offset for the stride
				for(AddressOffset = 0; AddressOffset < dwDeviceStride; AddressOffset++)
				{
					// The stride
					for(Address = AddressOffset; Address<(DWORD)MemorySize/4; Address=(Address+dwDeviceStride*2))
					{
						pdw[Address] = Data;
						pdw[Address+dwDeviceStride] = ~Data;
					}
				}
				// offset for the stride
				for(AddressOffset = 0; AddressOffset < dwDeviceStride; AddressOffset++)
				{
					//check for abort status
					bAbortStatus = CheckAbort(0);
					if(bAbortStatus == TRUE)
						throw((int)99);
					
					// the stride
					for(Address = 0; Address<(DWORD)MemorySize/4; Address=(Address+dwDeviceStride*2))
					{
						if(pdw[Address] != Data)
						{
							rec = pdw[Address];
							exp = Data;
							pbFail = (LPBYTE)((DWORD)((LPBYTE)&pdw[Address])*4);
							break;
						}
						if(pdw[Address+dwDeviceStride] != ~Data)
						{
							rec = pdw[Address];
							exp = ~Data;
							pbFail = (LPBYTE)((DWORD)((LPBYTE)&pdw[Address])*4);
							break;
						}
					}
				}
				
				// check for error if we did not make to the end of the pattern
				if(Address<(DWORD)MemorySize/4)
				{
					DecodeFailingLocation(1,(LPDWORD)pbFail, (QWORD) exp, (QWORD) rec, 8, NULL);
					/*				errDataMiscomparePQQQI(
					real ? (LPVOID)real : (LPVOID)pbFail,
					Address*4,
					exp,
					rec,
					pThreadParams->ProcNumber); */
					
				}
			} // device pattern
		} // device stride
	} // windows
	
	
	return 0;
}
//******************************************************************************
// Function name	: DMemoryBanks
// Description	    : 
// Return type		: DWORD - currently not used
//******************************************************************************
DWORD CXModuleMemory::DMemoryBanks(LPTHREADPARAMS pThreadParams)
{
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	DWORD            MemorySize,WindowSize,nProgCur,nProgMax;
	LPVOID           pMem = pThreadParams->pMemory;
	LPBYTE           pbMax;
	LPDWORD          pdwMax;
	LPBYTE			 pbFail;
	LPTESTCONDITIONS pParameters;
	BYTE bAbortStatus;
	
	pParameters   = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize    = pParameters->MemorySize;
	WindowSize    = pParameters->WindowSize;
	fProc0        = (pThreadParams->ProcNumber <= 0);
	
	LPDWORD			pdw;
	DWORD			dwDeviceStride;
	BYTE			bPatterns;
	QWORD			exp;
	QWORD			rec;
	QWORD			TransCount = 0;
	DWORD			Data;
	DWORD			Address,AddressOffset;
	
	if(!MMXCheck())
	{
		errMMX();
		return 0;
	}
	
	if(WindowSize == 0)
		WindowSize = MemorySize;
	
	// MemorySize must be a multiple of WindowSize
	MemorySize -= MemorySize % WindowSize;
	
	// Compute end test address.
	pbMax = (LPBYTE)pMem + MemorySize;
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = MemorySize / WindowSize * 88; // 11 strides, 2 patterns, 2 waves, 2 calls
	nProgCur = 0;
	
	ReSeed(pThreadParams->ProcNumber);
	
	// Walk through memory a window at a time.
	for(LPBYTE pb = (LPBYTE)pMem; pb < pbMax; pb += WindowSize)
	{
		// Compute the end of the window.
		pdwMax = (LPDWORD)pb + WindowSize / 4;
		
		pdw = (LPDWORD)pb;
		for(dwDeviceStride = 0; dwDeviceStride<=0; dwDeviceStride = dwDeviceStride*2)
		{
			// 0 and 1 in every cell
			for(bPatterns = 0; bPatterns<2; bPatterns++)
			{
				TransCount = TransCount+MemorySize*2;
				if(bPatterns == 0)
					Data = 0xFFFFFFFF;
				if(bPatterns == 1)
					Data =0x0;
				// offset for the stride
				for(AddressOffset = 0; AddressOffset < dwDeviceStride; AddressOffset++)
				{
					//check for abort status
					bAbortStatus = CheckAbort(0);
					if(bAbortStatus == TRUE)
						throw((int)99);
					
					// The stride
					for(Address = AddressOffset; Address<(DWORD)MemorySize/4; Address=(Address+dwDeviceStride*2))
					{
						pdw[Address] = Data;
						pdw[Address+dwDeviceStride] = ~Data;
					}
				}
				// offset for the stride
				for(AddressOffset = 0; AddressOffset < dwDeviceStride; AddressOffset++)
				{
					//check for abort status
					bAbortStatus = CheckAbort(0);
					if(bAbortStatus == TRUE)
						throw((int)99);
					
					// the stride
					for(Address = 0; Address<(DWORD)MemorySize/4; Address=(Address+dwDeviceStride*2))
					{
						if(pdw[Address] != Data)
						{
							rec = pdw[Address];
							exp = Data;
							pbFail = (LPBYTE)((DWORD)((LPBYTE)&pdw[Address])*4);
							break;
						}
						if(pdw[Address+dwDeviceStride] != ~Data)
						{
							rec = pdw[Address];
							exp = ~Data;
							pbFail = (LPBYTE)((DWORD)((LPBYTE)&pdw[Address])*4);
							break;
						}
					}
				}
				// check for error if we did not make to the end of the pattern
				if(Address<(DWORD)MemorySize/4)
				{
					DecodeFailingLocation(1,(LPDWORD)pbFail, (QWORD) exp, (QWORD) rec, 8, NULL);
					/*			errDataMiscomparePQQQI(
					real ? (LPVOID)real : (LPVOID)pbFail,
					Address*4,
					exp,
					rec,
					pThreadParams->ProcNumber);*/
					
				}
			} // device pattern
		} // device stride
	} // windows
	TestPerformance(TransCount);
	
	return 0;
}

//******************************************************************************
// Function name	: DBusNoise
// Description	    : 
// Return type		: DWORD - currently not used
//******************************************************************************
DWORD CXModuleMemory::DBusNoise(LPTHREADPARAMS pThreadParams)
{
	bool             fProc0;	// TRUE if thread sends wt_progress messages.
	DWORD            MemorySize,WindowSize,nProgCur,nProgMax;
	LPVOID           pMem = pThreadParams->pMemory;
	LPBYTE           pbMax;
	LPDWORD          pdwMax;
	LPTESTCONDITIONS pParameters;
	BYTE bAbortStatus;
	
	pParameters   = (LPTESTCONDITIONS)pThreadParams->pTestParameters;
	MemorySize    = pParameters->MemorySize;
	WindowSize    = pParameters->WindowSize;
	fProc0        = (pThreadParams->ProcNumber <= 0);
	
	LPDWORD pdw;
	DWORD dwBlockSize;
	DWORD dwAddress;
	BYTE bPatterns;
	BYTE bStatus;
	QWORD qwLow ;
	QWORD qwHigh;
	QWORD qwData =  0x0001000100010001;
	DWORD dwBusWords =8;
	QWORD exp;
	QWORD rec;
	_int64 TransCount = 0;
	
	if(!MMXCheck())
	{
		errMMX();
		return 0;
	}
	
	if(WindowSize == 0)
		WindowSize = MemorySize;
	
	// MemorySize must be a multiple of WindowSize
	MemorySize -= MemorySize % WindowSize;
	
	// Compute end test address.
	pbMax = (LPBYTE)pMem + MemorySize;
	
	// Compute the number of times the Progress() function will be called
	// in the code below so a percent complete value can be computed.
	nProgMax = MemorySize / WindowSize * 8; // 8 patterns
	nProgCur = 0;
	
	// Walk through memory a window at a time.
	for(LPBYTE pb = (LPBYTE)pMem; pb < pbMax; pb += WindowSize)
	{
		// Compute the end of the window.
		pdwMax = (LPDWORD)pb + WindowSize / 4;
		
		pdw = (LPDWORD)pb;
		dwBlockSize = WindowSize / 2;  //bytes to words - the asm code counts words
		// the pattern is 4 shifts by even and odd
		for(bPatterns = 0; bPatterns<64; bPatterns++)
		{
			if (bPatterns < 32)
			{
				qwLow =  0xFFFFFFFFFFFFFFFF & ~(qwData << bPatterns%4);
				qwHigh = ~qwLow;
			}
			else
			{
				qwHigh =  0xFFFFFFFFFFFFFFFF & ~(qwData << bPatterns%4);
				qwLow = ~qwHigh;
				
			}
			
			TransCount = TransCount + WindowSize * 2; // write read 1 block
			
			__asm {
				emms
					push	eax
					push	ebx
					push	ecx
					push	edx
					push	edi
					push	esi
					mov		ebx, dwBlockSize
					mov		edx, dwBusWords
					movq	mm0,qwLow
					movq	mm1,qwHigh
					mov		edi, pb				// load the starting memory address
loop1:
				movq	[edi], mm0
					add		edi, edx
					movq	[edi], mm1
					add		edi, edx
					sub		ebx,8
					jnz		loop1
					
					pop		esi
					pop		edi
					pop		edx
					pop		ecx
					pop		ebx
					pop		eax
					emms
			}
			
			if(g_fFaultInject)
				*(pdwMax - 1) = 0xDEADBEEF;
			
			//check for abort status
			bAbortStatus = CheckAbort(0);
			if(bAbortStatus == TRUE)
				throw((int)99);
			
			__asm {
				emms
					push	eax
					push	ebx
					push	ecx
					push	edx
					push	edi
					push	esi
					mov		ebx, dwBlockSize
					mov		esi, dwBusWords
					movq	mm2,qwLow			// what to compare
					movq	mm3,qwHigh
					movq	mm4,qwLow			// make a copy for exp pat
					movq	mm5,qwHigh
					
					mov		edi, pb				// load the starting memory address
loop1r:
				movq	mm0, [edi]
					pcmpeqd mm2,mm0				// compare the data with the exp pat.
					psrlq	mm2,16				// shift so we can get both result in the low Dword
					movd	edx,mm2				// move the result to real reg where we can do real compare
					cmp		edx,0FFFFFFFFh
					jne		error1
					movq	mm2,mm4				// fresh data to cmp
					add		edi, esi
					
					movq	mm1, [edi]
					pcmpeqd mm3,mm1				// compare the data with the exp pat.
					psrlq	mm3,16				// shift so we can get both result in the low Dword
					movd	edx,mm3				// move the result to real reg where we can do real compare
					cmp		edx,0FFFFFFFFh
					jne		error2
					movq	mm3,mm5				// fresh data to cmp
					
					add		edi, esi
					sub		ebx,8
					jnz		loop1r
					
					mov		bStatus, 0			// passing status
					jmp		bye
error1:
				mov		bStatus, 1			// error in the low pat
					mov		dwAddress,edi
					movq	rec,mm0
					movq	exp,mm4
					jmp		bye
error2:
				mov		bStatus, 1			// error in the high pat
					mov		dwAddress,edi
					movq	rec,mm1
					movq	exp,mm5
bye:
				pop		esi
					pop		edi
					pop		edx
					pop		ecx
					pop		ebx
					pop		eax
					emms
			}
			
			if (bStatus == 1)
			{
				DecodeFailingLocation(1,(LPDWORD)dwAddress, (QWORD) exp, (QWORD) rec, 8, NULL);
				
				/*				ReportError(0x89,L"Pattern miscompare at address %ph buffer index %uh\n"
				L"Expected value=%016I64Xh   Received value=%016I64Xh\n"
				L"Test thread running on processor %d (-1 = not specified)\n"
				L"If MEMMAP.SYS is not installed or this is Win9X the address is VIRTUAL.",real ? (LPVOID)real : (LPVOID)dwAddress,
				(LPBYTE)dwAddress - pb,
				exp,
				rec,
				pThreadParams->ProcNumber);*/
			}
			
			if(fProc0)	// wt_progress messages only sent from the processor 0 thread.
				Progress(++nProgCur * 100 / nProgMax);
		} // Patterns
	} // windows
	return(0);
	
}

//******************************************************************************
// Function name	: GetPhysicalMemorySize
// Description	    : Returns the amount of physical memory installed in the system
//                    as detected by Windows NT.
// Return type		: DWORDLONG - Physical memory in bytes.
//******************************************************************************
DWORDLONG CXModuleMemory::GetPhysicalMemorySize()
{
	MEMORYSTATUSEX ms;
	DWORDLONG      nMem;
	
	SafeGlobalMemoryStatusEx(&ms);
	nMem = ms.ullTotalPhys & 0xFFFFFFFFFFF00000uI64;	// Truncate to an even MB.
	
	if(ms.ullTotalPhys & 0xFFFFF)				// Round up.
		nMem += 0x100000;
	
	return nMem;
}


//******************************************************************************
// Function name	: GetLargestVirtualBufferSize
// Description	    : Determines the largest block of contiguous virtual memory.
//                    Will not be accurate if available physical memory is
//                    less than the largest contiguous block, but we won't need
//                    more than that.
// Return type		: SIZE_T - The size of the block in bytes.
//******************************************************************************
SIZE_T CXModuleMemory::GetLargestVirtualBufferSize()
{
	MEMORYSTATUS ms;
	GlobalMemoryStatus(&ms);
	
	SIZE_T dwSize = ms.dwAvailVirtual & ~(SIZE_T)0xFFFFF;	// Round down to nearest MB.
	LPVOID pMem   = NULL;
	
	// On IA-64 systems this value is HUGE, and we really only need to check
	// if a contiguous virtual buffer the size of available physical memory
	// is present.
	if(dwSize > ms.dwAvailPhys)
		dwSize = ms.dwAvailPhys & ~(SIZE_T)0xFFFFF;	// Round down to nearest MB.
	
	// This routine fudges the number by 16MB in case.
	while(dwSize > 0x1100000 && (pMem = VirtualAlloc(NULL,dwSize,MEM_RESERVE,PAGE_READWRITE)) == NULL)
		dwSize -= 0x100000;
	
	if(pMem)
		VirtualFree(pMem,0,MEM_RELEASE);
	
	dwSize -= 0x1000000;
	
	return dwSize;
}



//******************************************************************************
// Function name	: EstimateAvailableMemory
// Description	    : Reads the MemorySize, Force and Reserved parameters
//                    to determine the amount of memory to be tested.
// Return type		: SIZE_T - Test memory size in bytes.
//******************************************************************************
SIZE_T CXModuleMemory::EstimateAvailableMemory(SIZE_T Request)
{
	MEMORYSTATUSEX ms;
	SIZE_T         Available,Virtual;
	DWORD          Reserved;
	
	ReportDebug(8,L"Requested Physical Memory = %I64Xh (%I64uMB)",(QWORD)Request,(QWORD)Request >> 20);
	
	SafeGlobalMemoryStatusEx(&ms);
	// Cast is OK because the largest contiguous virtual memory we can get will always fit a SIZE_T.
	Available = (SIZE_T)ms.ullAvailPhys;
	ReportDebug(8,L"Available Physical Memory = %I64Xh (%I64uMB)",(QWORD)Available,(QWORD)Available >> 20);
	
	Virtual = GetLargestVirtualBufferSize();
	ReportDebug(8,L"Contiguous Virtual Memory = %I64Xh (%I64uMB)",(QWORD)Virtual,(QWORD)Virtual >> 20);
	if((Reserved =GetCfgUint(L"Reserved",0)) == 0)
		Reserved = 0x200000;	// 2MB by default.
	ReportDebug(9,L"Reserved Physical Memory = %lXh (%luMB)",Reserved,Reserved >> 20);
	
	// Compute lesser of Request, Available less Reserved, and Virtual.
	// Cast is OK because the largest contiguous virtual memory we can get will always fit a SIZE_T.
	if(Available > Reserved)
		Available -= Reserved;
	
	if(Virtual < Available)
		Available = Virtual;
	
	if(Request < Available)
		Available = Request;
	
	if(Available < 0x100000)
		Available = 0x100000;		// Always at least 1MB.
	
	Available &= ~(SIZE_T)0xFFFFF;	// round down to nearest MB.
	
	ReportDebug(8,L"Estimated Test Memory = %I64Xh (%I64uMB)",(QWORD)Available,(QWORD)Available >> 20);
	
	return Available;
}


//******************************************************************************
// Function name : AllocateMemory
// Description   : Allocates the requested memory size.  If Windows NT, it then
//                 increases the minimum and maximum working set size of the
//                 test process by this amount and locks the buffer into memory.
// Return type   : LPVOID              - pointer to allocated memory.
// Argument      : PSIZE_T pMemorySize - [in]How much memory to allocate per thread.
//                                       [out]total memory allocated.
// Argument      : bool fForce         - TRUE if memory can't be downsized.
// Argument      : bool fNoCache       - TRUE if memory should be allocated
//                                       with the PAGE_NOCACHE attribute.
//******************************************************************************
LPVOID CXModuleMemory::AllocateMemory(PSIZE_T pMemorySize,bool fForce,bool fNoCache,DWORD nThreads)
{
	ASSERT(*pMemorySize <= (SIZE_T)-1 / nThreads);
	ASSERT(nThreads > 0);
	ASSERT(pMemorySize != NULL);
	
	SIZE_T OrgSize = *pMemorySize * nThreads;
	LPVOID pMem;
	SIZE_T AdjSize;
	BYTE bAbortStatus;
	
	MEMORYSTATUSEX ms;
	
	SafeGlobalMemoryStatusEx(&ms);
	ReportDebug(2,
		L"Memory Load: %ld%%\n"
		L"Total Physical: %I64Xh bytes\n"
		L"Available Physical: %I64Xh bytes\n"
		L"Total Page File %I64Xh bytes\n"
		L"Available Page File: %I64Xh bytes\n"
		L"Total Virtual: %I64Xh bytes\n"
		L"Available Virtual: %I64Xh bytes\n"
		L"Available Extended Virtual: %I64Xh bytes\n",
		ms.dwMemoryLoad,
		ms.ullTotalPhys,
		ms.ullAvailPhys,
		ms.ullTotalPageFile,
		ms.ullAvailPageFile,
		ms.ullTotalVirtual,
		ms.ullAvailVirtual,
		ms.ullAvailExtendedVirtual);
	
	// This function needs to query and allocate memory, so all
	// memory tests (and other modules that use a lot of memory)
	// need to use this mutex to lock the query/allocate process.
	while(::WaitForSingleObject(g_hMemoryMutex,5000) == WAIT_TIMEOUT)
	{
		//check for abort status
		bAbortStatus = CheckAbort(0);
		if(bAbortStatus == TRUE)
			throw((int)99);
	}
	
	if(fForce)
	{
		// Allocate the exact amount of memory needed.
		if((pMem = VirtualAlloc(NULL,OrgSize,MEM_COMMIT,fNoCache ? PAGE_READWRITE | PAGE_NOCACHE : PAGE_READWRITE)) == NULL)
		{
			errVirtualAllocQ(GetLastError(),OrgSize);
			goto error;
		}
		
		AdjSize = OrgSize;
	}
	else
	{
		// Adjust the memory amount requested based on available memory and virtual address space available.
		AdjSize = EstimateAvailableMemory(OrgSize);
		
		if((pMem = VirtualAlloc(NULL,AdjSize,MEM_COMMIT,fNoCache ? PAGE_READWRITE | PAGE_NOCACHE : PAGE_READWRITE)) == NULL)
		{
			errVirtualAllocQ(GetLastError(),AdjSize);
			goto error;
		}
		
	}
	
	VERIFY(ReleaseMutex(g_hMemoryMutex));
	
	
	ReportDebug(1,L"Test Memory Size = %I64Xh (%I64uMB)",(QWORD)AdjSize,(QWORD)AdjSize >> 20);
	
	*pMemorySize = AdjSize / nThreads;
	
	AbortMemPtr = pMem;
	return pMem;
	
error:
	VERIFY(ReleaseMutex(g_hMemoryMutex));
	// exit
	throw CXmtaException(99,NULL);
	//	return NULL; // not reached
}


//******************************************************************************
// Function name	: FreeMemory
// Description	    : Release the test memory buffer.  If Windows NT, reduce the
//                    process working set size by the MemorySize amount.
// Return type		: void
// Argument         : LPVOID pMem - The starting address of the buffer to release.
// Argument         : SIZE_T MemorySize - Needed only to report an error.
//******************************************************************************
void CXModuleMemory::FreeMemory(LPVOID pMem,SIZE_T MemorySize)
{
	if(!VirtualFree(pMem,0,MEM_RELEASE))
		errVirtualFreeQ(GetLastError(),MemorySize);
	AbortMemPtr = NULL;
}


//******************************************************************************
// Function names   : seed, random
// Description	    : Pair of functions to seed and generate random DWORDs.
//                    Seeding the generator a generating a list of numbers, then
//                    reseeding with the same value and generating the list again
//                    will produce the same list of numbers.  This function is
//                    thread-safe.
// Return type		: DWORD - a 32-bit random number.
// Argument         : seed  - a 32-bit starting point for the generator.
//******************************************************************************


void CXModuleMemory::seed(DWORD seed)
{
	holdrand = seed;
}

DWORD CXModuleMemory::random()
{
	DWORD ret;
	ret  = (holdrand = holdrand * 214013L + 2531011L) >> 16;
	ret |= (holdrand = holdrand * 214013L + 2531011L) & 0xFFFF0000;
	return ret;
}


//******************************************************************************
// Function names	: SetRandomSeed, GetRandomSeed, ReSeed
// Description	    : These functions manage the random seed for a particular
//                    test instance.  SetRandomSeed should be called once per
//                    test to initialize the generator and record the initial
//                    seed for the test.  A test can then call ReSeed as many
//                    times as needed to regenerate a list of random numbers.
//                    If a test fails, GetRandomSeed can report the seed to the
//                    user.  A parameter can be set to force a failing test to
//                    execute with the same seed with which it failed.
// Return type		: DWORD - The test seed.
//******************************************************************************

void CXModuleMemory::SetRandomSeed()
{
	ASSERT(GlobalSeed == 0);	// Call only once for process.
	
	// RandomSeed parameter can be used to force the same seed to be used
	// as reported by an error message.
	if((GlobalSeed = GetCfgUint(L"RandomSeed",0))== 0)
		GlobalSeed = (DWORD)time(NULL);
	
	seed(GlobalSeed);
	ReportDebug(1,L"RandomSeed = %Xh",GlobalSeed);
}

DWORD CXModuleMemory::GetRandomSeed()
{
	return GlobalSeed;
}

// offset is used to get a new seed from the original seed.
void CXModuleMemory::ReSeed(int offset)
{
	ASSERT(GlobalSeed != 0);	// Call SetRandomSeed() first.
	seed(GlobalSeed + (DWORD)offset);
}


//******************************************************************************
// Function name	: ParameterMemorySize
// Description	    : Reads the MemorySize parameter and returns its value
//                    rounded down to the nearest MB or 1MB, whichever is greater.
// Return type		: SIZE_T - greater of 0x100000 or MemorySizeMB in bytes.
//******************************************************************************
SIZE_T CXModuleMemory::ParameterMemorySize()
{
	DWORD     dwMemMB;
	DWORDLONG Mem;
	
	if((dwMemMB = GetCfgUint(L"MemorySizeMB",0)) == 0)
	{
		dwMemMB = (DWORD)(GetPhysicalMemorySize() >> 20);
		
		ReportWarning(
			L"MemorySizeMB parameter not found.\n"
			L"Defaulting to detected memory size of %luMB.",
			dwMemMB);
	}
	
	if(dwMemMB == 0)
		dwMemMB = 1;		// Always at least 1MB.
	
	Mem = (DWORDLONG)dwMemMB << 20;
	
	if(Mem > (SIZE_T)-1)
	{
		MEMORYSTATUS ms;
		GlobalMemoryStatus(&ms);
		Mem = GetLargestVirtualBufferSize();
		ReportWarning(
			L"MemorySizeMB parameter too large for this version of NT.\n"
			L"Defaulting to largest contiguous virtual buffer size of %I64uMB.",(QWORD)Mem >> 20);
	}
	
	ReportDebug(1,L"MemorySizeMB = %luMB (%I64Xh)",dwMemMB,Mem);
	return (SIZE_T)Mem;
}


//******************************************************************************
// Function name	: ParameterCacheSize
// Description	    : Returns the value of the CacheSize parameter rounded down
//                    to the nearest power of two.  If not found, it returns
//                    the L2 cache of the processor if it can be found, or 512KB
//                    if it can't be found.
// Return type		: DWORD - The cache size in bytes.
//******************************************************************************
DWORD CXModuleMemory::ParameterCacheSize()
{
	DWORD dwCache;
	
	// Read the CacheSize parameter.
	if((dwCache = GetCfgUint(L"CacheSize",0)) == 0)
	{
		// if no parameter, check the processor for cache (CPUID) or
		// use 512KB if the processor cache cannot be determined.
		if(!GetCpuCacheInfo(&dwCache))
			dwCache = 0x80000;
		
		ReportWarning(
			L"CacheSize parameter not found.\n"
			L"Defaulting to a cache size of %Xh (%uKB).",
			dwCache,dwCache >> 10);
	}
	else
	{
		// Round down to nearest power of two.
		if(dwCache > 0)
		{
			for(DWORD tmp = 0xFFFFFFFF; (~tmp & dwCache) == 0; tmp >>= 1)
				;
			dwCache &= ~tmp;
		}
	}
	
	ReportDebug(1,L"CacheSize = %Xh (%uKB)",dwCache,dwCache >> 10);
	return dwCache;
}


//******************************************************************************
// Function name	: ParameterSwitch
// Description	    : Reads the value of the named Boolean parameter.
// Return type		: bool - TRUE if the named parameter is present and non-zero.
// Argument         : LPCTSTR szName - name of the parameter.
//******************************************************************************
bool CXModuleMemory::ParameterSwitch(LPCTSTR szName)
{
	bool result;
	result = GetCfgBoolean(szName);
	ReportDebug(1,L"%s = %d",szName,result);
	return result;
}


//******************************************************************************
// Function name	: GetCpuCacheInfo
// Description	    : Calls CPUID.  If it supports reporting cache size info,
//                    look for an L2 cache and report its size.
//                    See the CPUID description in the Intel Instruction Set Reference.
//                    See the Processor Specification updates (Pentium III Xeon, etc.)
//                    for additional cache size tokens definitions (0x82,0x84,0x85 so far).
// Return type		: bool - TRUE if CPUID supports cache information reporting.
// Argument         : LPDWORD pdwSize - points to size of L2 cache if TRUE is returned.
//******************************************************************************
#define EFLAGS_ID (1<<21)

bool CXModuleMemory::GetCpuCacheInfo(LPDWORD pdwSize)
{
	DWORD regs[4];
	int   loops;
	
	*pdwSize = 0;
	
	GetCpuId(0,regs);	// Cmd zero returns highest command supported in EAX.
	if(regs[0] < 2)		// index 0=EAX 1=EBX, etc.  Must support 2 for cache types.
		return FALSE;
	
	loops = GetCpuId(2,regs);	// Cmd=2, get cache info.  AL (retval) is number of
	// times GetCpuId must be called to get all info.
	
	while(loops--)
	{
		DWORD type;
		
		// Individual bytes of EAX,EBX,ECX,EDX (regs[0] thru [4]) are cache descriptors.
		for(int i = 0; i < 4; i++)
		{
			// The reg is not valid if bit 31 is not a zero.
			if((regs[i] & (1<<31)) == 0)
			{
				// shift through the four bytes in each reg.
				for(int j = 0; j < 4; j++)
				{
					type = regs[i] & 0xFF;
					switch(type)
					{
					case 0x41:	// 4-way set associative, 128KB L2 cache
					case 0x81:	// not documented, assume 8-way 128KB
						*pdwSize = 0x020000;	// 128KB
						goto done;
					case 0x42:	// 4-way
					case 0x82:	// 8-way
						*pdwSize = 0x040000;	// 256KB
						goto done;
					case 0x43:	// 4-way
					case 0x83:	// not documented, assume 8-way 512KB
						*pdwSize = 0x080000;	// 512KB
						goto done;
					case 0x44:	// 4-way
					case 0x84:	// 8-way
						*pdwSize = 0x100000;	//   1MB
						goto done;
					case 0x45:	// 4-way
					case 0x85:	// 8-way
						*pdwSize = 0x200000;	//   2MB
						goto done;
					}
					
					regs[i] >>= 8;
				}
			}
		}
		
		GetCpuId(2,regs);
	}
	
done:
	return TRUE;
}

//******************************************************************************
// Function name	: GetCpuId
// Description	    : Executes the x86 CPUID instruction with EAX=cmd.
//                    Returns EAX,EBX,ECX,EDX in the regs array.
//                    See the CPUID description in the Intel Instruction Set Reference.
// Return type		: int - The value in AL after CPUID is executed.
//******************************************************************************
int CXModuleMemory::GetCpuId(DWORD cmd,DWORD regs[4])
{
	int retval = 0;
	
#ifdef _WIN64
	cmd;
	regs[0]=0;
#else // _WIN32
	__asm {
		mov eax,cmd
			cpuid
			push ebx
			mov ebx,regs
			mov 0[ebx],eax				// Save EAX,EBX,ECX,EDX in regs array.
			mov byte ptr [retval],al	// return the value in AL.
			pop eax
			mov 4[ebx],eax
			mov 8[ebx],ecx
			mov 12[ebx],edx
	}
#endif // _WIN64
	
	return retval;
}


//******************************************************************************
// Function name	: MMXCheck
// Description	    : Checks for the MMX feature being present
//
// Return type		: BOOL
//******************************************************************************
bool CXModuleMemory::MMXCheck()
{
	
	BYTE MMX;
	
	MMX=0;
	_asm
	{
		push eax
			push edx
			mov		eax,1
			cpuid
			test	edx, 0x800000
			jnz		yesmmx
			jmp		end
yesmmx:	mov		BYTE PTR MMX,1
end:	pop edx
		pop	eax
	}
	return MMX;
	
}


//******************************************************************************
// Function name	: InitAddressConversions
// Description	    : Loads the WinMTA helper DLL and retrieves the function
//                    to resolve virtual addresses into physical addresses.
// Return type		: BOOL
//******************************************************************************
bool CXModuleMemory::InitAddressConversions()
{
	static init = FALSE;
	
	if(init)
		return TRUE;
	
	
	return init;
}

/******************************************************************************
* Function: SafeGlobalMemoryStatusEx
* Purpose:  Safely handles a call to GlobalMemoryStatusEx() on Windows systems
*           that don't support it by filling out the structure on those systems
*           with information from GlobalMemoryStatus().
* Returns:  void
*             
* Argument: MEMORYSTATUSEX* p
*             [in]A pointer to a MEMORYSTATUSEX structure.
*             [out]A pointer to a filled out MEMORYSTATUSEX structure.
******************************************************************************/
bool CXModuleMemory::SafeGlobalMemoryStatusEx(LPMEMORYSTATUSEX p)
{
	
	MEMORYSTATUS ms;
	
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);
	
	p->dwLength = sizeof(MEMORYSTATUSEX);
	p->dwMemoryLoad = ms.dwMemoryLoad;
	p->ullTotalPhys = ms.dwTotalPhys;
	p->ullAvailPhys = ms.dwAvailPhys;
	p->ullTotalPageFile = ms.dwTotalPageFile;
	p->ullAvailPageFile = ms.dwAvailPageFile;
	p->ullTotalVirtual = ms.dwTotalVirtual;
	p->ullAvailVirtual = ms.dwAvailVirtual;
	p->ullAvailExtendedVirtual = 0;
	
	ASSERT(ms.dwTotalPhys != 0);
	
	return TRUE;
}



//******************************************************************************
// Function name    : DecodeFailingLocation
// Description      : Decodes the virtual address and failing data pattern to a 
//                    to a location on the board and reports the error
// Return type      : void
//******************************************************************************
void CXModuleMemory::DecodeFailingLocation(WORD ECode,LPDWORD dwMemoryAddress, QWORD qwExp, QWORD qwRec, BYTE Size, CHAR * MsgPtr)
{
	DWORD qwAddress;
	BYTE  bFirstBadBit; // first bad bit
	BYTE  bLastBadBit;  // last bad bit
	QWORD qwBadBits;
	BYTE  bBadBitCount;
	QWORD qwFirstBadAddress; //byte address of the first bad bit
	QWORD qwLastBadAddress; // byte address of the last bad bit
	BYTE  x;
	WCHAR DeviceStr[256];
	WCHAR DetailStr[256];
	DWORD Ecode;
	
	//Look up physical address
	qwAddress = MmGetPhysicalAddress((PVOID)dwMemoryAddress);
	
	//Mask for the size
	if(Size == 1)
	{
		qwRec = 0xFF & qwRec;
		qwExp = 0xFF & qwExp;
	}
	if(Size == 2)
	{
		qwRec = 0xFFFF & qwRec;
		qwExp = 0xFFFF & qwExp;
	}
	if(Size == 4)
	{
		qwRec = 0xFFFFFFFF & qwRec;
		qwExp = 0xFFFFFFFF & qwExp;
	}
	
	// Compare exp and rec. If the same soft read error
	// Generate a special error message for this.
	if(qwRec == qwExp)
	{
		if(Size == 1)
			ReportError(0x30,L"Address 0x%8.8x, Pattern 0x%2.2X, Failing pattern can not be determined. Error in NV2A or processor",qwAddress, (WORD)qwRec);
		if(Size == 2)
			ReportError(0x30,L"Address 0x%8.8x, Pattern 0x%4.4X, Failing pattern can not be determined. Error in NV2A or processor",qwAddress, (WORD)qwRec);
		if(Size == 4)
			ReportError(0x30,L"Address 0x%8.8x, Pattern 0x%8.8X, Failing pattern can not be determined. Error in NV2A or processor",qwAddress, (DWORD)qwRec);
		if(Size == 8)
			ReportError(0x30,L"Address 0x%8.8x, Pattern 0x%016I64X, Failing pattern can not be determined. Error in NV2A or processor",qwAddress, (QWORD)qwRec);
		return;
	}
	// generate a error message about the details
	if(Size == 1)
		wsprintf(DetailStr,L"Address 0x%8.8x, Rec 0x%2.2X Exp=0x%2.2X,",qwAddress, (WORD)qwRec, (WORD)qwExp);
	if(Size == 2)
		wsprintf(DetailStr,L"Address 0x%8.8x, Rec 0x%4.4X Exp=0x%4.4X,",qwAddress, (WORD)qwRec, (WORD)qwExp);
	if(Size == 4)
		wsprintf(DetailStr,L"Address 0x%8.8x, Rec 0x%8.8X Exp=0x%8.8X,",qwAddress, (DWORD)qwRec, (DWORD)qwExp);
	if(Size == 8)
		wsprintf(DetailStr,L"Address 0x%8.8x, Rec 0x%016I64X Exp=0x%016I64X,",qwAddress, (QWORD)qwRec, (QWORD)qwExp);
	// Now work on the failing device details
	// Compare exp and rec see how many bits are wrong.
	qwBadBits = qwRec ^ qwExp;
	// find the first bit that is bad
	for(x=0;x<64;x++)
	{
		if(((qwBadBits >>x) & 0x1) == 1)
		{
			// record the first bad bit and it's address
			bFirstBadBit = x;
			qwFirstBadAddress = x/8 + qwAddress;
			break;
		}
	}
	//find the last bad bit and the number of bad bits
	bBadBitCount = 0;
	for(x=0;x<64;x++)
	{
		if(((qwBadBits >>x) & 0x1) == 1)
		{	
			// count the bad bits
			bBadBitCount++;
			// record the last bad bit and it's address
			bLastBadBit = x;
			qwLastBadAddress = x/8 + qwAddress;
		}
	}
	ReportDebug(2,L"\nBit count %d \nFirst bit %d\nLast bit %d \nFirst address 0x%016I64X \nLast address 0x%016I64X\n",
		bBadBitCount,
		bFirstBadBit,
		bLastBadBit,
		qwFirstBadAddress,
		qwLastBadAddress);

	//Single bit error decode the device Multibit error bits on same device 
	if(((qwFirstBadAddress/8) - (qwLastBadAddress/8) == 0))
	{
		// error is in bank A Top, MSB 0
		
		switch (((qwAddress >> 4) & 0x3))
		{
		case 0:
			if(bBadBitCount == 1)
			{
				Ecode = 0x32;
				wcscpy(DeviceStr,L"Error is suspected on device U6N1");
			}
			else
			{
				Ecode = 0x33;
				wcscpy(DeviceStr,L"Error is suspected on device NV2A or U6N1");
			}
			break;
		case 1:
			if(bBadBitCount == 1)
			{
				Ecode = 0x34;
				wcscpy(DeviceStr,L"Error is suspected on device U6D1");
			}
			else
			{
				Ecode = 0x35;
				wcscpy(DeviceStr,L"Error is suspected on device NV2A or U6D1");
			}
			break;
		case 2:
			if(bBadBitCount == 1)
			{
				Ecode = 0x36;
				wcscpy(DeviceStr,L"Error is suspected on device U5F1");
			}
			else
			{
				Ecode = 0x37;
				wcscpy(DeviceStr,L"Error is suspected on device NV2A or U5F1");
			}
			break;
		case 3:
			if(bBadBitCount == 1)
			{
				Ecode = 0x38;
				wcscpy(DeviceStr,L"Error is suspected on device U3T1");
			}
			else
			{
				Ecode = 0x39;
				wcscpy(DeviceStr,L"Error is suspected on device NV2A or U3T1");
			}
			break;
		default:
			ReportError(31,L"Code error in memory test");
		}
	}
	//Multibit error across devices
	if((qwFirstBadAddress/8) - (qwLastBadAddress/8) != 0)
	{
		wcscpy(DeviceStr, L"Error is on multiple memory devices, Suspected part is NV2A");
	}
	ReportError(32,L"%s %s",DetailStr, DeviceStr);
	// check for abort
	BYTE bAbortStatus;
	bAbortStatus = CheckAbort(L"Error message report");
	if(bAbortStatus == TRUE)
	{
		if(AbortMemPtr != NULL)
		{
			FreeMemory(AbortMemPtr,0);
		}
		throw((int)99);
	}
	
}
