#include <windows.h>


typedef void (APIENTRY *PDLLFUNC)(void);


struct FREQ_INFO	// struct returned by DLL function call- yes it's lame to define it here.
{
	unsigned long in_cycles;	// internal clock cycles during test	
	unsigned long ex_ticks;		// microseconds elapsed during test						
	unsigned long raw_freq;		// raw frequency of CPU in MHz
	unsigned long norm_freq;	// normalized frequency of CPU in MHz.
};


typedef FREQ_INFO (FAR *PCPUFUNC)(int BSFclocks);


int iGlobal = 0;
int giDelay = 0;


class CBase

{
public:

	int m_i;
	char m_ch;

	CBase(void)	// first line of CBase::CBase()
	{
		m_i = 0;
		m_ch = 0;
	}

} gCBaseObj;


int GetCPUSpeed(void)

{
	int iCPUSpeed = 0;

	// This function first uses a free distribution Intel DLL to determine CPU speed.  Note
	// that this only works for Intel CPUs!
	HINSTANCE hCpuDll = LoadLibrary("cpuinf32.dll");
	PCPUFUNC pCpuFunc = (PCPUFUNC)GetProcAddress(hCpuDll, "cpuspeed");
	FREQ_INFO CpuInfo = (*pCpuFunc)(0);
	iCPUSpeed = CpuInfo.norm_freq;
	FreeLibrary(hCpuDll);

	// If the DLL fails us, we might find the CPU speed in the registry.
	// WinNT puts it there, but Win95 doesn't.  Win98?  Dunno.
	if(iCPUSpeed <= 0)	// DLL function *should* return 0 if it fails
	
{
		HKEY keyOpen = NULL;
		LONG lRegStatus = ::RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
											"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
											0,
											KEY_ALL_ACCESS, 
											&keyOpen);
		if (lRegStatus == ERROR_SUCCESS)
		{
			DWORD dwBufferSize = sizeof(int);
			lRegStatus = ::RegQueryValueEx(	keyOpen,
											"~MHz",
											NULL,
											NULL,
											(LPBYTE)&iCPUSpeed,
											&dwBufferSize);
		}
	
		if (lRegStatus != ERROR_SUCCESS)
			iCPUSpeed = 0;	// paranoia
	
		::RegCloseKey(keyOpen);
	}

	// If registry lookup fails, the calling routine will have to punt.  Return will be 0.
	return iCPUSpeed;
}

void Func(void)

{						// first line of Func()
	int iLocal = 0;		// init iLocal in Func()

	while(0);			// line within Func()

	iLocal++;
	iGlobal = 1;

MultipleBreakpointsHitLabel:
	while(0);	// multiple breakpoints hit
}


void OverloadedFunc(void)
{							// first line of OverloadedFunc(void)
}

void OverloadedFunc(int i)
{							// first line of OverloadedFunc(int)
}


int FAR PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)

{	// first line of WinMain()
	int iLocal = 0;
	int i;

	Func();
	
	OverloadedFunc();	// line after call to Func()
	OverloadedFunc(1);
	
	gCBaseObj.m_ch = 'a';	
	while(0);	// gCBaseObj change

	HINSTANCE hDll = LoadLibrary("breakpoints pri2 dll.dll");
	PDLLFUNC pDllFunc = (PDLLFUNC)GetProcAddress(hDll, "DllFunc");	// dll loaded

	// We need to 'throttle' the number of iterations that the following for loops perform,
	// since too many iterations can cause Cafe to fail out at 5 min. on a slow machine, and
	// too few can cause both for loops to be executed in about the same span of time on a
	// fast machine.	mke 3/5/98
	int iCPUSpeed = GetCPUSpeed();
	int iLoopNum = 0;
	iCPUSpeed = (iCPUSpeed == 0) ? 133 : iCPUSpeed;	// assume 133 MHz (midrange) if speed test fails
	
	if(iCPUSpeed < 133)
		iLoopNum = 500;
	else if(iCPUSpeed < 200)
		iLoopNum = 3000;
	else
		iLoopNum = 10000;

	for(i = 0; i < iLoopNum; i++)
		pDllFunc();	// line within loop

	iLocal++;
	iGlobal++;

	for(i = 0; i < giDelay; i++)		   
		Sleep(1000);
	
	while(0);	// line within WinMain()

	iGlobal = 100;
	while(0);	// iGlobal = 100

	FreeLibrary(hDll);
	hDll = LoadLibrary("breakpoints pri2 dll.dll");
	pDllFunc = (PDLLFUNC)GetProcAddress(hDll, "DllFunc");
	pDllFunc();

	return 0;	
}