// memory.h - Memory test module

#pragma once
#define QWORD _int64

// Disable the compiler warning for a non threatening condition.
// C4512: unable to generate an assignment operator for the given class. 
#pragma warning (disable:4512)
#include "..\parameter.h"

// Naming of class test and module classes.
// 
// module class: CXModule<module>
// test class:   CXModule<module>Test<test>
//
// The names are important because the modules and exective are 
// all one project.  Since the classes are presented alphebetically, 
// using this naming convention makes the code easier to navigate.
//
#define MEMORY_MODNUM   10
class CXModuleMemory : public CTestObj
{
public: 
	// Get module number from numbers.h
	// If this is a new module, add the number to the file.
	//
	DECLARE_XMTAMODULE (CXModuleMemory, "memory", MEMORY_MODNUM);

	virtual bool InitializeParameters ();
	virtual ~CXModuleMemory();
	// for access by stress object
	DWORD		random();
	DWORD holdrand;
	friend class stress;
	protected:

	void DecodeFailingLocation(WORD ECode,LPDWORD qwAddress, QWORD qwExp, QWORD qwRec, BYTE Size, CHAR * MsgPtr);
	// Critical section to make the DevStat function thread-safe.
	CRITICAL_SECTION g_critDevStat;

	// Example Error messages ...
	// (Note: 0x000 - 0x00f reserved for XMTA)
	//
	void ReportDeviceNotFoundAt (UINT addr)
	{
		ReportError (0x010, _T("Device Not found at address 0x%x\n"), addr);
	}
	void ThrowDeviceNotResponding (LPCTSTR string)
	{
		throw CXmtaException (0x011, _T("Device not responding: %s\n"), string);
	}
	void ReportRequiredParameter (LPCTSTR string)
	{
		ReportError (0x012, _T("Required parameter \"%s\" not found.\n"), string);
	}

	// Example parameters ...
	//
	int m_cfgCount;
	UINT m_cfgAddress;
	LPCTSTR m_cfgDevice;
	int m_cfgLevel;
	LPCTSTR m_cfgMessage;

protected:
	
	/////////////////////////////////////////////////////////////////////////////
	
	
	BOOL	g_fFaultInject;
	LPDWORD g_pMemmap;
	HANDLE	   g_hMemoryMutex;
	DWORD GlobalSeed;

	wchar_t * TestNamePtr;
	typedef struct tagTHREADPARAMS
	{
		int    ProcNumber;			// The processor running this thread.
		LPVOID pTestParameters;		// The parameters for this thread.
		LPVOID pMemory;				// The preallocated and locked memory buffer to test.
	} THREADPARAMS,*LPTHREADPARAMS;
	
	typedef struct tagTESTCONDITIONS
	{
		DWORD_PTR MemorySize;	// Amount of memory to test.
		DWORD_PTR BlockSize;	// Some tests set up a block of memory and fill the test window with it.
		DWORD_PTR WindowSize;	// This much memory is written before being read back.
	} TESTCONDITIONS,*LPTESTCONDITIONS;
	
#ifdef _XBOX
typedef struct _MEMORYSTATUSEX {
    DWORD dwLength;
    DWORD dwMemoryLoad;
    DWORDLONG ullTotalPhys;
    DWORDLONG ullAvailPhys;
    DWORDLONG ullTotalPageFile;
    DWORDLONG ullAvailPageFile;
    DWORDLONG ullTotalVirtual;
    DWORDLONG ullAvailVirtual;
    DWORDLONG ullAvailExtendedVirtual;
} MEMORYSTATUSEX,  *LPMEMORYSTATUSEX;
#endif
	
	void   DisplayMemoryStatus();
	void   MemorySizeCheck();
	void   MemorySlidingOnesAndZeros();
	void   MemoryRandomData();
	void   MemoryAddressPatterns();
	void   MemoryMixedOps();
	void   MemoryRandomDwordOps();
	void   MemoryTransfer();
	void   CacheSlidingOnesAndZeros();
	void   CacheRandomData();
	void   CacheAddressPatterns();
	void   CacheMixedOps();
	void   CacheRandomDwordOps();
	void   WindowedCacheRandomDwordOps();
	void   WindowedCacheMixedOps();
	void   WindowedCacheRandomData();
	void   WindowedCacheUniqueness();
	void   RambusDeviceSuround();
	void   RambusDeviceStride();
	void   RambusBusNoise();
	void   RambusBus32KStride();
	void   RambusBusPowersof2();
	void   RambusBusWriteOrder();
	void   RambusBusAlternatingAddress();
	void   QWORDBusNoise();
	void   CachePerformance();
	void   MemoryPerformance();
	void   DDR32KStride();
	void   DDRBusPowersof2();
	void   DDRMemoryBanks();
	void   DDRDeviceStride();
	void   DDRBusNoise();
	
	
	DWORD	RambusCell(LPTHREADPARAMS pThreadParams);
	DWORD	AlternatingPatterns(LPTHREADPARAMS pThreadParams);
	DWORD	BusNoise(LPTHREADPARAMS pThreadParams);
	DWORD	Memory32K(LPTHREADPARAMS pThreadParams);
	DWORD	Powersof2(LPTHREADPARAMS pThreadParams);
	DWORD	WriteOrder(LPTHREADPARAMS pThreadParams);
	DWORD	AlternatingAddress(LPTHREADPARAMS pThreadParams);
	DWORD	QWORDNoise(LPTHREADPARAMS pThreadParams);
	DWORD	DPowersof2(LPTHREADPARAMS pThreadParams, DWORD dwStrideStart,DWORD dwStrideEnd);
	DWORD	DMemoryBanks(LPTHREADPARAMS pThreadParams);
	DWORD	DDeviceStride(LPTHREADPARAMS pThreadParams);
	DWORD	DBusNoise(LPTHREADPARAMS pThreadParams);
	
	DWORD  SlidingOnesAndZeros        (LPTHREADPARAMS pThreadParams);
	DWORD  RandomData                 (LPTHREADPARAMS pThreadParams);
	DWORD  AddressPatterns            (LPTHREADPARAMS pThreadParams);
	DWORD  MixedOpsMixAlignRandomOrder(LPTHREADPARAMS pThreadParams);
	DWORD  DwordRandomOrder           (LPTHREADPARAMS pThreadParams);
	
	LPVOID AllocateMemory(PSIZE_T MemorySize,bool fForce,bool fNoCache,DWORD nThreads);
	LPVOID AllocateMemoryAWE(PSIZE_T pMemorySize,bool fForce,bool fNoCache,DWORD nThreads);
	LPVOID AllocateMemoryOld(PSIZE_T pMemorySize,bool fForce,bool fNoCache,DWORD nThreads);
	void   FreeMemory(LPVOID pMem,SIZE_T MemorySize);
	
	SIZE_T    EstimateAvailableMemory(SIZE_T Request);
	DWORDLONG GetPhysicalMemorySize();
	SIZE_T    GetLargestVirtualBufferSize();
	
	void      seed(DWORD seed);
	void      SetRandomSeed();
	DWORD     GetRandomSeed();
	void      ReSeed(int offset);
	SIZE_T    ParameterMemorySize();
	DWORD     ParameterCacheSize();
	bool      ParameterSwitch(LPCTSTR szName);
	int       GetCpuId(DWORD cmd,DWORD regs[4]);
	bool      GetCpuCacheInfo(LPDWORD pdwSize);
	bool      MMXCheck();
	bool      SafeGlobalMemoryStatusEx(LPMEMORYSTATUSEX p);
	PVOID	  AbortMemPtr; 

	void TestPerformance(_int64 count);

	//
	// Error Definitions messages
	//
	void errInvalidTestNumber()
	{
		ReportError(0x00,L"Invalid test number.");
	}
	void errNotSupported()
	{
		ReportError(0x01,L"Background tests have been retired and are no longer supported.\n"
			L"Please remove them from the package file.");
	}
	void errTempPath(DWORD dw1)  
	{
		ReportError(0x02,L"A temporary directory is not defined for this system.  Set the\n"
			L"TMP and TEMP environment variables or the TempDrive test parameter.  Last Error =%X", dw1);
		
	}
	void errTempFile(DWORD dw1)
	{
		ReportError(0x03,L"Error trying to create a temporary file.  Last Error =%X",dw1);
	}
	void errUserTempFileS(DWORD dw1,char * lpcStr1)
	{
		ReportError(0x04,L"Error creating temp file %s. Last Error =%x"
			,lpcStr1,dw1);
		
	}
	void errMemorySizeLL(DWORD dw1,DWORD dw2)
	{
		ReportError(0x05,L"Memory size miscompare.\n"
			L"Expected: %luMB   Received: %luMB"
			,dw1,dw2);
	}
	void errOutOfMemoryQ(QWORD qw1)
	{
		ReportError(0x06,L"Could not allocate %I64u bytes for a buffer.  Out of memory or allocation too large.\n"
			L"Each memory test removes \"MemorySizeMB\" megabytes of memory from the memory pool.\n"
			L"If memory tests are running in parallel their \"MemorySizeMB\", when added, cannot\n"
			L"exceed the total physical memory available to the operating system.\n"
			L"Also try running fewer tests at the same time or increasing the page file size.\n"
			L"If looping on tests in WIN98 SE a small, permanent memory leak in the OS occurs\n"
			L"each time a test is executed and will eventually cause this error."
			,qw1);
		
	}

	void errWriteFile(DWORD dw1)
	{
		ReportError(0x09,L"WriteFile() failed. Last Error = %X",dw1);
	}
	void errReadFile(DWORD dw1)
	{
		ReportError(0x0A,L"ReadFile() failed. Last Error = %X", dw1);
	}
	void errTestThreadTimeout()
	{
		ReportError(0x0D,L"The wait for a test thread failed.");
	}
	void errLogOverflowQ(QWORD qw1)
	{
		ReportError(0x00E,L"Testing log overflow at index %I64Xh (software bug)."
			,qw1);
	}
	void errGetPwsSize(DWORD dw1)
	{
		ReportError(0x0F,L"Failed to determine the process working set size. Last error =%X",dw1);
	}
	void errSetPwsSizeHigherQ(DWORD dw1,QWORD qw1)
	{
		ReportError(0x10,L"Failed to increase the minimum and maximum working set size for this process by %I64Xh bytes.\n"
			L"The system is out of memory.  If memory tests are running in parallel make sure their combined\n"
			L"\"MemorySizeMB\" parameters do not exceed the physical memory available to the operating system.\n"
			L"Last Error = %X"
			,qw1,dw1);
		// 0x011 NOT USED
	}
	void errVirtualAllocQ(DWORD dw1,QWORD qw1)
	{
		ReportError(0x12,L"Failed to allocate %I64Xh bytes of virtual memory. Last Error = %X",
			qw1,dw1);
	}
	void errVirtualLockQ(DWORD dw1, QWORD qw1)
	{
		ReportError(0x13,L"Failed to lock %I64Xh bytes of virtual memory into physical memory. Last Error =%X"
			,qw1,dw1);
	}
	void errVirtualFreeQ(DWORD dw1,QWORD qw1)
	{
		ReportError(0x014,L"Failed to free %I64Xh bytes of virtual memory. Last error = %x."
			,qw1, dw1);
	}
	void errOutOfMemory()
	{
		ReportError(0x15,L"All memory is reserved in the system.  Each memory test removes \"MemorySizeMB\" megabytes of memory\n"
			L"from the memory pool.  If memory tests are running in parallel their \"MemorySizeMB\", when added,\n"
			L"cannot exceed the total physical memory available to the operating system.  Also try running fewer\n"
			L"tests at the same time or increasing the page file size. If looping on tests in WIN98 SE a small,\n"
			L"permanent memory leak in the OS occurs each time a test is executed and will eventually cause this error.");
	}
	
	void errRamDiskWriteQL(QWORD qw1,DWORD dw1)
	{
		ReportError(0x17,L"An error occurred writing address %09I64Xh (%ld bytes)."
			,qw1,dw1);
	}
	void errRamDiskReadQL(QWORD qw1,DWORD dw1)
	{
		ReportError(0x18,L"An error occurred reading address %09I64Xh (%ld bytes)."
			,qw1,dw1);
	}
	void errRamDiskMiscompareQLL(QWORD qw1,DWORD dw1,DWORD dw2)
	{
		ReportError(0x19,L"Data miscompare reading address %09I64Xh.\n"
			L"Expected: %08lXh   Received: %08lXh"
			,qw1,dw1,dw2);
	}
	void errRamDiskMiscompareRndQLLL(QWORD qw1,DWORD dw1,DWORD dw2,DWORD dw3)
	{
		ReportError(0x1A,L"Data miscompare reading address %09I64Xh.\n"
			L"Expected: %08lXh   Received: %08lXh   Random Seed=%08lXh"
			,qw1,dw1,dw2,dw3);
	}
	void errVlmMemorySizeLL(DWORD dw1,DWORD dw2)
	{
		ReportError(0x1B,L"Non-NT accessible memory size miscompare.\n"
			L"Expected: %luMB   Received: %luMB"
			,dw1,dw2);
	}
	void errMMX()
	{
		ReportError(0x1F,L"This test requires MMX. MMX is not present!");
	}
	// 0x20 NOT USED
	void errMissingParameterS(LPCTSTR lpStr)
	{
		ReportError(0x21,L"Required parameter \"%s\" was not found.",lpStr);
	}
	void errPerProblemSIII(LPCTSTR lpStr,DWORD MBSec,DWORD LCL,DWORD UCL)
	{
		ReportError(0x22,L"%s performance out of range, Measured = %dMB/Sec LCL = %d UCL = %d",lpStr, MBSec, LCL, UCL);
	}

	//******************************************************************************
	// This block of stuff allows us to dynamically load the WMTAFUNC helper DLL
	// and retrieve the virtual-physical address converter.
	//******************************************************************************
	bool      InitAddressConversions();
	DWORD_PTR GetPhysicalAddress(PVOID virtaddr);
	
	#define WMTAFUNC_DWORD_VERSION  0x300
	
	typedef LARGE_INTEGER PHYSICAL_ADDRESS,*LPPHYSICAL_ADDRESS;
	
	typedef struct tagWMINFO
	{
		DWORD nLength;
		DWORD dwDllVersion;
		DWORD dwDriverVersion;
	} WMINFO,*PWMINFO;
	
	typedef bool (APIENTRY *LPGETWMTAFUNCINFO)(PWMINFO pInfo);
	typedef bool (APIENTRY *LPGETPHYSICALADDRESS)(PVOID VirtAddr,LPPHYSICAL_ADDRESS pPhysAddr);
	
	LPGETWMTAFUNCINFO    wmGetWmtafuncInfo;
	LPGETPHYSICALADDRESS wmGetPhysicalAddress;
	
	//******************************************************************************
	// Function name	: Progress
	// Description	    : Sends a progress message only if different from the last one.
	// Return type		: inline void 
	// Argument         : int nPercent
	//******************************************************************************
	inline void Progress(int nPercent)
	{
		static nProgLast = 0;
		
		if(nProgLast != nPercent)
		{
			//fol		wt_progress(nPercent);
			nProgLast = nPercent;
		}
	}
};

