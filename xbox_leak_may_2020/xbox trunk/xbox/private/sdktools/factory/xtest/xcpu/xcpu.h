// xCPU.h - CPU testing module

#if !defined(_XCPU_H_)
#define _XCPU_H_

// supress futile warnings about assignment operators
#pragma warning (disable:4512)

#include "..\parameter.h"


#ifndef PASS
#define PASS 0
#endif
#ifndef FAIL
#define FAIL 0xFFFFFFFF
#endif

#define XCPU_MODNUM   3
// Naming of class test and module classes.
// 
// module class: CX<module>
// test class:   CX<module>Test<test>
//
// The names are important because the modules and exective are 
// all one project.  Since the classes are presented alphebetically, 
// using this naming convention makes the code easier to navigate.
//
long THE_END;			// End of the used address space
int (__cdecl *lucas)();

class CXModuleCPU : public CTestObj
{
public:
	DECLARE_XMTAMODULE (CXModuleCPU, "cpu", XCPU_MODNUM);  //(module name, module number)

protected:
	// Error messages declared here ...
	//(Note: 0x000 - 0x00f reserved for XMTA)
	//
	DWORD err_BADPARAMETER (LPCTSTR s1)
	{
		ReportError (0x010, L"Can't find configuration parameter \"%s\"\nThis configuration parameter doesn't exist or is invalid", s1); return FAIL;
	}
	DWORD err_WRONGFAMILY_D (int i1)
	{
		ReportError (0x011, L"The CPU is not of the correct family (value retrieved=%d).", i1); return FAIL;
	}
	DWORD err_WRONGMODEL_D (int i1)
	{
		ReportError (0x012, L"The CPU is not of the correct model (value retrieved=%d).", i1); return FAIL;
	}
	DWORD err_STEPPINGNOTALLOWED_X (int i1)
	{
		ReportError (0x013, L"Stepping is not allowed: 0x%x.", i1); return FAIL;
	}
	DWORD err_MMXNOTPRESENT ()
	{
		ReportError (0x014, L"MMX support not present when it should be."); return FAIL;
	}
	DWORD err_SIMDNOTPRESENT ()
	{
		ReportError (0x015, L"SIMD support not present when it should be."); return FAIL;
	}
	DWORD err_FPUNOTPRESENT ()
	{
		ReportError (0x016, L"FPU unit not present when it should be."); return FAIL;
	}
	DWORD err_NOMATCH_CACHE_DD (DWORD dw1, DWORD dw2)
	{
		ReportError (0x017, L"Actual cache descriptor value (0x%8.8lx) does not match expected value (0x%8.8lx).", dw1, dw2); return FAIL;
	}
	DWORD err_MMX_MOVE ()
	{
		ReportError (0x018, L"MMX mov operations failed."); return FAIL;
	}
	DWORD err_FREEINGMEMORY ()
	{
		ReportError (0x019, L"Error Freeing Memory."); return FAIL;
	}
	DWORD err_ALLOCMEMORY ()
	{
		ReportError (0x01A, L"Error Allocating Memory."); return FAIL;
	}
	DWORD err_SIMD_MOVE (DWORD dw1, DWORD dw2, DWORD dw3, DWORD dw4, DWORD dw5, DWORD dw6, DWORD dw7, DWORD dw8)
	{
		ReportError (0x01B, L"Streaming SIMD Register move operations failed with an unexpected result.\nExpected=%08lx%08lx%08lx%08lx\nReceived=%08lx%08lx%08lx%08lx", dw1, dw2, dw3, dw4, dw5, dw6, dw7, dw8); return FAIL;
	}
	DWORD err_SIMD_FP_SP (float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8)
	{
		ReportError (0x01C, L"SIMD FP SP operations failed with an unexpected result.\nExpected=%08.10f %08.10f %08.10f %08.10f\nReceived=%08.10f %08.10f %08.10f %08.10f", f1, f2, f3, f4, f5, f6, f7, f8); return FAIL;
	}
	DWORD err_SIMD_CMP_MAX_SUB (float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8)
	{
		ReportError (0x01D, L"SIMD CMP MAX AND SUB FP SP operations failed with an unexpected result.\nExpected=%08.10f %08.10f %08.10f %08.10f\nReceived=%08.10f %08.10f %08.10f %08.10f", f1, f2, f3, f4, f5, f6, f7, f8); return FAIL;
	}
	DWORD err_SIMD_MUL_DIV_CVT (float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8)
	{
		ReportError (0x01E, L"SIMD MUL, DIV, CVT FP SP operations failed with an unexpected result.\nExpected=%08.10f %08.10f %08.10f %08.10f\nReceived=%08.10f %08.10f %08.10f %08.10f", f1, f2, f3, f4, f5, f6, f7, f8); return FAIL;
	}
	DWORD err_NOMATCH_ORIGANDNEW_DD (int i1, int i2)
	{
		ReportError (0x01F, L"Expected value (%d) did not match actual value (%d).", i1, i2); return FAIL;
	}
	DWORD err_SQ_NUM_S (LPCTSTR s1)
	{
		ReportError (0x020, L"Squaring: %s.", s1); return FAIL;
	}
	DWORD err_ROUNDING_S (LPCTSTR s1)
	{
		ReportError (0x021, L"Rounding: %s.", s1); return FAIL;
	}
	DWORD err_FFT_FAILURE_S (LPCTSTR s1)
	{
		ReportError (0x022, L"FFT Failure: %s.", s1); return FAIL;
	}
	DWORD err_WRONG_CPU_TYPE_DD (int i1, int i2)
	{
		ReportError (0x023, L"The CPU (%d) is not of the correct type (%d).", i1, i2); return FAIL;
	}
	DWORD err_2STEPPINGLISTS ()
	{
		ReportError (0x024, L"Cannot have both 'Steppings Allowed' and 'Steppings Not Allowed' lists."); return FAIL;
	}
	DWORD err_NOMATCH_ORIGANDNEW_DDD (int i1, int i2, int i3)
	{
		ReportError (0x025, L"Actual speed MHz value (%d) did not fall in the expected range (%d - %d).", i1, i2, i3); return FAIL;
	}
	DWORD err_XXX ()
	{
		ReportError (0x026, L""); return FAIL;
	}


	// Parameters declared here ...
	//
	int iFamily;
	int iModel;
	int iCPUType;
	DWORD dwCacheDescriptor;
	int iCPUInternalSpeed;
	int iCPUTolerance;
	int iFPUStressLoops;
//	int m_cfgInt;
//	UINT m_cfgUint;
//	LPCTSTR m_cfgString;
//	int m_cfgChoice;

virtual bool InitializeParameters ();

public:
	// Common functions go here.  Put any functions that need 
	// to be global to the tests in the module class
	//
	void HelperFunction (); // delete this
};

#pragma warning (disable:4201)
union CPUType 
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
#pragma warning (default:4201)

#endif // _XCPU_H_
