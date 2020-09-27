// (c) Copyright 1999-2000 Intel Corp. All rights reserved.
// Intel Confidential.
//
// cpu.h : main header file for the CPU application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

long THE_END;			// End of the used address space
int (__cdecl *lucas)();

/////////////////////////////////////////////////////////////////////////////
// CCpuApp:
// See cpu.cpp for the implementation of this class
//

class CCpuApp : public CWinApp
{
public:
	CCpuApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCpuApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCpuApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

void SelfSense();    
void SelfSenseCPU(char *path, char *name, int cpu = -1);    
void TestCleanup();    
void DisplayCPUFeatures();
void CPUFeatureCheck();               
void MMXCode();
void PrintError(unsigned int ecode, LPCTSTR format_string,...);
void FPUFeatures();
void MMXFeatures();
void SIMDFeatures();
extern "C" void xmmtest();
void FPUStress();
CString GetLastErrorText(DWORD dwLastError);
int CPUSpeed();
void CkCPUSpeed();
BOOL CPUDescription(char *desc, int desc_len, int Type, int Model, int Family);
BOOL CacheDescription(char *desc, int desc_len);
BOOL AppendData(char* data1, int data1_len, char* data2);
BOOL FindCacheMatch(char *desc, int desc_len, __int16 masked_data);
BOOL IsNTor2000();
void GetCPUID();
BOOL TrySIMD();
								   
union Type 
{
	DWORD reg;
	struct
	{
		DWORD Stepping:4;
		DWORD Model:4;
		DWORD Family:4;		// used to be 3, but spec says should be 4!
		DWORD ProcType:2;
	};
public:
};

union Cache
{
	DWORD reg;
	struct 
	{
		DWORD byte0:8;
		DWORD byte1:8;
		DWORD byte2:8;
		DWORD byte3:8;		// bit 7 of byte3 will be 0 if this register is valid
	};
	public:
};


union Features
{
	DWORD reg;
	struct
	{
		DWORD FPU:1;
		DWORD VME:1;
		DWORD DE:1;
		DWORD PSE:1;
		DWORD TSC:1;
		DWORD MSR:1;
		DWORD PAE:1;
		DWORD MCE:1;
		DWORD CXS:1;
		DWORD APIC:1;
		DWORD RES1:2;
		DWORD MTRR:1;
		DWORD PGE:1;
		DWORD MCA:1;
		DWORD CMOV:1;
		DWORD WHOKNOWS:7;
		DWORD MMX:1;
		DWORD FXSR:1;
		DWORD XMM:1;
		DWORD WNI:1;	//bit 26
	};
};

union struct_128 
{
	struct					// view data as 32 bit floating point numbers
	{
		float aa,bb,cc,dd;
	};
	struct
	{
		double low_float, high_float;
	};
	struct					// view data as 32 bit integers
	{
		DWORD a, b, c, d;
	};
	struct
	{
		DWORD low_int,high_int;	// view data as 64 bit integers
	};
public:
};
struct int_2x32{__int32 a, b;};

