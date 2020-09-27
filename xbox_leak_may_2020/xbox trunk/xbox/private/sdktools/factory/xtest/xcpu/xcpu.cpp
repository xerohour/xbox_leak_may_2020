// (c) Copyright 2000-2001 Intel Corp. All rights reserved.
//
// Title: xCPU
//
// History:
//
// 09/25/00 V1.00 BLI Release V1.00.
// 10/03/00 V1.01 BLI Modified FPU Stress test so that the data buffer
//          is now relocateable.  Also changed module to use new XMTA Macro module
//          and test declarations.
// 10/05/00 X1.02 BLI Changed module and test declarations to work with XMTA 1.03
// 10/05/00 X1.03 BLI Changed module and test declarations to work with XMTA 1.04 and
//          added CheckAbort() calls appropriately

#include "..\stdafx.h"
#include "..\testobj.h"
#include "xcpu.h"
#include "prime95.h"

IMPLEMENT_MODULEUNLOCKED (CXModuleCPU);

bool CXModuleCPU::InitializeParameters ()
{
	if (!CTestObj::InitializeParameters ())
		return false;
//	m_cfgString = GetCfgString (_T("string"), NULL);
	if ((iFamily = GetCfgInt (L"family", -1)) == -1)
	{
		err_BADPARAMETER(L"FAMILY");
		if (CheckAbort (HERE)) return true;
	}
	if ((iModel = GetCfgInt (L"model", -1)) == -1)
	{
		err_BADPARAMETER(L"MODEL");
		if (CheckAbort (HERE)) return true;
	}
	if ((iCPUType = GetCfgInt (L"cpu_type", -1)) == -1)
	{
		err_BADPARAMETER(L"CPU_TYPE");
		if (CheckAbort (HERE)) return true;
	}
	dwCacheDescriptor = GetCfgUint (L"cache_descriptor", 0xFFFFFFFF);
	if ((iCPUInternalSpeed = GetCfgInt (L"cpu_internal_speed", -1)) == -1)
	{
		err_BADPARAMETER(L"CPU_INTERNAL_SPEED");
		if (CheckAbort (HERE)) return true;
	}
	iCPUTolerance = GetCfgInt (L"cpu_speed_tolerance", 5);
	iFPUStressLoops = GetCfgInt (L"fpu_stress_loops", 20);
//	m_cfgUint = GetCfgUint (_T("unsigned"), 0);
//	m_cfgChoice = GetCfgChoice (_T("choice"), 0, _T("none"), _T("one"), _T("many"));
	return true;
}

extern "C" unsigned long PARG=0;
extern "C" double MAXERR=0;
extern "C" double SUMINP=0;
extern "C" double SUMOUT=0;
extern "C" DWORD ERRCHK=0;
extern "C" unsigned long inputaddrqq=0;
extern "C" unsigned long FPSHALF=0x3F000000;
extern "C" unsigned long FPSMINUS2=0xC0000000;
extern "C" unsigned long FPSBIGVAL=0;
extern "C" unsigned long FPSscaled_numbig=0;
extern "C" unsigned long FPSscaled_numlit=0;
extern "C" unsigned long FPSp=0;
extern "C" unsigned long FPSnumlit=0;
extern "C" unsigned long FPSnumbig=0;

//******************************************************************
// Title: CXCPUTestFeatureCheck
//
// Abstract: Checks the features on the CPU vs any parameters
//
// Uses config parameters:
//           family
//           model
//           cpu_type
//           cache_descriptor
//           steppings_allowed[XX]
//           steppings_not_allowed[XX]
//******************************************************************
IMPLEMENT_TESTUNLOCKED (CPU, feature_check, 1)
{
//	#ifdef _DEBUG
		static int FeatureChk;
		FeatureChk++;
		ReportDebug(BIT0, _T("FeatureCheck Test - Loop %d"), FeatureChk);
//	#endif

	int x, locStepAllw, locStepNotAllw;
	int SteppingsAllowed;
	int SteppingsNotAllowed;
	BOOL SteppingOK;
	DWORD dwEAX, dwEBX, dwECX, dwEDX;
	wchar_t SteppingsParamName[64];
	CPUType ActualCPUType;
	Features ActualCPUFeatures;

//ReportWarning (L"This is my warning message");
//ReportDebug (BIT1, L"This is my bit 1 debug message");
//ReportDebug (BIT4, L"This is my bit 4 debug message");
//ReportStatistic (L"my_stat_key", L"This is my stat message");

	locStepAllw = FALSE;
	locStepNotAllw = FALSE;
	_asm // Get CPUID
	{
		push eax
		push ebx
		push ecx
		push edx
		mov	eax, 1
		cpuid
		mov	ActualCPUType.reg, eax
		mov	ActualCPUFeatures.reg, edx
		pop edx
		pop ecx
		pop ebx
		pop eax
	}

	// Check the falmily, model, and cpu type
	if(iFamily != (int)ActualCPUType.Family)
	{
		err_WRONGFAMILY_D((int)ActualCPUType.Family);
		if (CheckAbort (HERE)) return;
	}
	if(iModel != (int)ActualCPUType.Model)
	{
		err_WRONGMODEL_D((int)ActualCPUType.Model);
		if (CheckAbort (HERE)) return;
	}
	if(iCPUType != (int)ActualCPUType.ProcType)
	{
		if (CheckAbort (HERE)) return;
		err_WRONG_CPU_TYPE_DD((int)ActualCPUType.ProcType, iCPUType);
	}

	// check for steppings not allowed
	// Normally configuration parameter capturing happens in the InitializeParameters()
	// routine, but these are done here to simplify the code.
	for(x=0;x<99;x++)
	{
		swprintf(SteppingsParamName,L"steppings_not_allowed[%d]",x);
		if ((SteppingsNotAllowed = GetCfgInt (SteppingsParamName, -1)) == -1)
			break;
		locStepNotAllw=TRUE;
		// Now see if this matches the actual stepping
		if( SteppingsNotAllowed == (int)ActualCPUType.Stepping)
		{
			err_STEPPINGNOTALLOWED_X((int)ActualCPUType.Stepping);
			if (CheckAbort (HERE)) return;
		}
	}

	// check the steppings allowed
	SteppingOK = TRUE;
	for(x=0;x<99;x++)
	{
		swprintf(SteppingsParamName,L"steppings_allowed[%d]",x);
		if ((SteppingsAllowed = GetCfgInt (SteppingsParamName, -1)) == -1)
			break;
		locStepAllw=TRUE;
		// Now see if this matches the actual stepping
		SteppingOK = FALSE;
		if( SteppingsAllowed == (int)ActualCPUType.Stepping)
		{
			SteppingOK = TRUE;
			break;
		}
	}
	// can't have both a "Steppings Allowed" AND a "Steppings Not Allowed" list
	if (locStepAllw && locStepNotAllw)
	{
		err_2STEPPINGLISTS();
		if (CheckAbort (HERE)) return;
	}

	if (SteppingOK == FALSE)
	{
		err_STEPPINGNOTALLOWED_X(ActualCPUType.Stepping);
		if (CheckAbort (HERE)) return;
	}

		
	// check for MMX
	if(ActualCPUFeatures.MMX != 1)
	{
		err_MMXNOTPRESENT();
		if (CheckAbort (HERE)) return;
	}

	// check for SIMD/mmx2
	if(ActualCPUFeatures.XMM != 1)
	{
		err_SIMDNOTPRESENT();
		if (CheckAbort (HERE)) return;
	}

	// check for FPU
	if(ActualCPUFeatures.FPU != 1)
	{
		err_FPUNOTPRESENT();
		if (CheckAbort (HERE)) return;
	}

	// retrieve the cache description.  This is a shortcut instead of walking the
	// list of descriptors.
	_asm // Get the CPUID descriptor information
	{
		push eax
		push ebx
		push ecx
		push edx
		mov	eax, 2
		cpuid
		mov dwEAX, eax
		mov dwEBX, ebx
		mov dwECX, ecx
		mov dwEDX, edx
		pop edx
		pop ecx
		pop ebx
		pop eax
	}

	// check for correct cache Features
	if (dwCacheDescriptor != 0xFFFFFFFF) // Only check this if the config parameter was present
	{
		if(dwEDX != dwCacheDescriptor)
		{
			err_NOMATCH_CACHE_DD(dwEDX, dwCacheDescriptor);
			if (CheckAbort (HERE)) return;
		}
	}
}


//******************************************************************
// Title: CXCPUTestMultimediaExtensions
//
// Abstract: Executes MMX instructions to confirm proper operation
//
// Uses config parameters:
//
//******************************************************************
IMPLEMENT_TESTUNLOCKED (CPU, multimedia_extensions, 2)
{
//	#ifdef _DEBUG
		static int MMEx;
		MMEx++;
		ReportDebug(BIT0, _T("MultimediaExtensions Test - Loop %d"), MMEx);
//	#endif

	DWORD dwTempDword;
	DWORD dwResult;
	Features ActualCPUFeatures;

	// check for an MMX unit
	_asm // Get CPUID
	{
		push eax
		push ebx
		push ecx
		push edx
		mov	eax, 1
		cpuid
		mov	ActualCPUFeatures.reg, edx
		pop edx
		pop ecx
		pop ebx
		pop eax
	}
	if(ActualCPUFeatures.MMX == 0)
	{
		err_MMXNOTPRESENT();
		if (CheckAbort (HERE)) return;
	}
	else // MMX feature is present
	{
		_asm
		{
			push eax
			push ebx

			mov	eax,12345678h
			movd	mm0,eax
			movd	ebx,mm0
			movd	mm1,ebx
			movd	dwTempDword,mm1
			movd	mm2,dwTempDword
			movq	mm3,mm2
			movd	dwTempDword,mm3
			movq	mm4,dwTempDword
			movq	mm5,mm4
			movq	mm6,mm5
			movq	mm7,mm6
			movd	dwResult,mm7
			emms		     ; empty MMX state

			pop ebx
			pop eax
		}
		if(dwResult != 0x12345678)
		{
			err_MMX_MOVE();
			if (CheckAbort (HERE)) return;
		}
	}
}

//******************************************************************
// Title: CXCPUTestFPUStress
//
// Abstract: Executes a stressful loop of floating point instructions
//           Computes Mersenne Prime Numbers
//           Uses Dr. Richard Crandall's algorithm
//
// Uses config parameters:
//
//******************************************************************
IMPLEMENT_TESTUNLOCKED (CPU, fpu_stress, 3)
{

//	#ifdef _DEBUG
		static int FpuStress;
		FpuStress++;
		ReportDebug(BIT0, _T("FPUStress Test - Loop %d"), FpuStress);
//	#endif

	//	static LPVOID memreserved;
	static char *memreserved;
	unsigned long fftlen;
	wchar_t buf[100];
	unsigned long k, l;
	unsigned int i;
	unsigned long p;
	unsigned int ll_iters;
	char failed;

	memreserved = NULL;
	failed=FALSE;
	fftlen=65536;
	ll_iters = iFPUStressLoops;
	PARG = 0;
	MAXERR = 0.0;
	SUMINP = 0.0;
	SUMOUT = 0.0;
	ERRCHK = 0;

	// Load the appropriate EXE file and setup
	THE_END = THE_END_64;
	lucas = lucas64;
	
	// Now we know how much memory to reserve

	//freeing memory
	if (memreserved != NULL) 
	{
		delete [] memreserved;
		memreserved = NULL;
	}
//	err_FREEINGMEMORY();

	// Allocate memory for the assembly code
	if ((memreserved = new char[THE_END]) == NULL)
	{
		err_ALLOCMEMORY(); // Don't need to CheckAbort here because we return anyway
		return;
	}
	inputaddrqq = (unsigned long)memreserved;



	// Determine the range from which we'll choose an exponent to test.
	i=128; 
	p=1344999;


	// Now run Lucas setup
	PARG = p;
	setup64 ();

	// Do Lucas-Lehmer iterations
	for (k = 0; k <ll_iters; k++) 
	{
		if (CheckAbort (HERE)) break;  // Do this CheckAbort because this loop could be long
/*
		if (CheckAbort (HERE))
		{
			printf("Loop = %ld\n", k);
			break;  // Do this CheckAbort because this loop could be long
		}
*/
		// Don't need to CheckAbort anywhere else in this loop because all errors cause the
		// loop to break and then quickly exit the test.
		// Init data area with a pre-determined value
		//clear the memory
		for (l=0; l<fftlen; l++) 
			set_fft_value(p, fftlen,l,0);
		set_fft_value (p, fftlen, 0, 4);

		// One Lucas-Lehmer test with error checking			
		
		ERRCHK++;
		(*lucas)();
		ERRCHK--;
		
		// If the sum of the output values is an error (such as infinity)
		// then raise an error.  For some reason these bad values are treated
		// as zero by the C compiler.  There is probably a better way to
		// check for this error condition.

		if (SUMOUT <= 0.0) 
		{
			failed=TRUE;
			break;
		}

		// Check that the sum of the input numbers squared is approximately
		// equal to the sum of unfft results.  Note that as the sum of the
		// inputs approaches zero the more error we must tolerate. 

//printf("insum = %lf, outsum = %lf\n", SUMINP, SUMOUT);
		if (SUMOUT < SUMINP-0.1 || SUMOUT > SUMINP+0.1) 
		{
//			sprintf (buf, ERRMSG1B, SUMINP, SUMOUT);
			wsprintf(buf,L"i=%u, k=%lu, ",i,k);
			err_SQ_NUM_S(buf);
			failed=TRUE;
			break;
		}


		// Make sure round off error is tolerable
		if (MAXERR > 0.40) 
		{
//			sprintf (buf, ERRMSG1C, MAXERR);
			wsprintf(buf,L"k=%lu, ",k);
			failed=TRUE;
			err_ROUNDING_S(buf);
			break;
		}

	} // end for

	// Check for failure 
	if (failed==TRUE) 
	{
		wsprintf(buf,L"FFT length=%lu --> ",fftlen);
		err_FFT_FAILURE_S(buf);
		// FAILED!
	}
	if (memreserved != NULL) 
	{
		delete [] memreserved;
		memreserved = NULL;
	}
}

//******************************************************************
// Title: CXCPUTestSIMDNewInstructions
//
// Abstract: Executes SIMD instructions to confirm proper operation
//
// Uses config parameters:
//
//******************************************************************
IMPLEMENT_TESTUNLOCKED (CPU, simd_new_instructions, 4)
{
//	#ifdef _DEBUG
		static int SimdInstr;
		SimdInstr++;
		ReportDebug(BIT0, _T("SimdNewInstructions Test - Loop %d"), SimdInstr);
//	#endif


	Features ActualCPUFeatures;
	// set up 128 bit INT data and INT results
	struct_128 data_int, iresult, data_float1, data_float2, exp_fresult, fresult, check2, check3;
	void *datai, *dataf1, *dataf2;
	struct_128 *iresultp, *fresultp;

	data_int.a = 1;  // Initialize the DWORD structure
	data_int.b = 2;
	data_int.c = 3;
	data_int.d = 4;
	data_float1.aa = (float)1.2;
	data_float1.bb = (float)5.6;
	data_float1.cc = (float)9.7;
	data_float1.dd = (float)0.4;
	data_float2.aa = (float)5.3;
	data_float2.bb = (float)2.1;
	data_float2.cc = (float)10.2;
	data_float2.dd = (float)7.0;
	exp_fresult.aa = (float)2.5495097637176514;
	exp_fresult.bb = (float)2.100000;
	exp_fresult.cc = (float)4.460942;
	exp_fresult.dd = (float)2.7202942371368408;
	check2.aa = (float)-4.10000038146972660000;
	check2.bb = (float)-2.1;
	check2.cc = (float)-.5;
	check2.dd = (float)-6.6;
	check3.aa = (float)20;
	check3.bb = (float)42;
	check3.cc = (float)42;
	check3.dd = (float)40;

	// check for an SIMD support in CPU
	_asm // Get CPUID
	{
		push eax
		push ebx
		push ecx
		push edx
		mov	eax, 1
		cpuid
		mov	ActualCPUFeatures.reg, edx
		pop edx
		pop ecx
		pop ebx
		pop eax
	}
	if(ActualCPUFeatures.XMM == 0)
	{
		err_SIMDNOTPRESENT();
		if (CheckAbort (HERE)) return;
	}
	else // SIMD is supported
	{
		datai = &data_int;
		iresultp = &iresult;
		_asm
		{
			push eax
			push edi
			mov	eax,datai
			movups	xmm0,[eax]
			movaps	xmm1, xmm0
			movaps	xmm2, xmm1
			movaps	xmm3, xmm2
			movaps	xmm4, xmm3
			movaps	xmm5, xmm4
			movaps	xmm6, xmm5
			movaps	xmm7, xmm6
			shufps	xmm7, xmm7, 1bh	;shuffle a3,a2,a1,a0 to a0,a1,a2,a3
			shufps	xmm7, xmm7, 1bh	;shuffle a3,a2,a1,a0 to a0,a1,a2,a3
			mov	edi,iresultp
			movups	[edi],xmm7
			pop edi
			pop eax
			emms
		}
		if(memcmp(&data_int,&iresult,16) != 0)
		{
			err_SIMD_MOVE(data_int.d,data_int.c,data_int.b,data_int.a,iresult.d,iresult.c,iresult.b,iresult.a);
			if (CheckAbort (HERE)) return;
		}
	
		dataf1 = &data_float1;
		dataf2 = &data_float2;
		fresultp = &fresult;
		_asm
		{
			push eax
			push edi
			mov	eax, dataf1		;load 2 registers with 128 bit floats
			movups	xmm0, [eax]
			mov	eax, dataf2
			movups	xmm1, [eax]		
			addps	xmm0, xmm1		;add xmm0 + xmm1 store in xmm0
			sqrtps	xmm2, xmm0		;sqrt xmm0 store in xmm2
			minps	xmm1, xmm2		;take the min of xmm1 and xmm2, store in xmm1
			mov	edi,  fresultp		;point to result structure
			movups	[edi],xmm1		;xfr result to structure
			pop edi
			pop eax
			emms
		}
		if(memcmp(&exp_fresult,&fresult,16) != 0)
		{
			err_SIMD_FP_SP(exp_fresult.aa,exp_fresult.bb,exp_fresult.cc,exp_fresult.dd, fresult.aa,fresult.bb,fresult.cc,fresult.dd);
			if (CheckAbort (HERE)) return;
		}
		_asm
		{
			push eax
			push edi
			mov	eax, dataf1
			movups	xmm0, [eax]
			mov	eax, dataf2
			movups	xmm1, [eax]
			maxps	xmm1, xmm0	;find max and store in xmm1
			movups	xmm2, [eax]
			cmpeqps	xmm1, xmm2	;which value changed?  should be second value
			andps	xmm0, xmm1	;lose the second value in xmm0
			subps	xmm0, xmm2	;
			mov	edi,  fresultp	;point to result structure
			movups	[edi],xmm0	;xfr result to structure				
			pop edi
			pop eax
			emms
		}
		if(memcmp(&check2,&fresult,16) != 0)
		{
			err_SIMD_CMP_MAX_SUB(check2.aa,check2.bb,check2.cc,check2.dd,fresult.aa,fresult.bb,fresult.cc,fresult.dd);
			if (CheckAbort (HERE)) return;
		}  
		_asm
		{
			push eax
			push edi
			mov		eax, 5
			cvtsi2ss	xmm0, eax		;load 5 into lsd
			shufps		xmm0, xmm0, 93h		;rotate by 32 bits 5 in 2 of 4
			mov		eax, 6
			cvtsi2ss	xmm0, eax		;load 6 into lsd
			shufps		xmm0, xmm0, 93h		;rotate by 32 bits 5 in 3 of 4
			mov		eax, 7
			cvtsi2ss	xmm0, eax		;load 7	
			shufps		xmm0, xmm0, 93h		;rotate by 32 bits 5 in 4 of 4
			mov		eax, 8
			cvtsi2ss	xmm0, eax		;load 8   xmm0 = 5678
			movaps		xmm1, xmm0
			shufps		xmm1, xmm1, 1bh		;rotate	  xmm1 = 8765
			mulps		xmm1, xmm0		;ans  40,42,42,40
			mov		eax, 2
			cvtsi2ss	xmm0, eax		;load 2
			divss		xmm1, xmm0		;divide by 2 scalar

			mov		edi,  fresultp		;point to result structure
			movups		[edi],xmm1		;xfr result to structure
			pop edi
			pop eax
			emms
		}
		if(memcmp(&check3,&fresult,16) != 0)
		{
			err_SIMD_MUL_DIV_CVT(check3.aa,check3.bb,check3.cc,check3.dd,fresult.aa,fresult.bb,fresult.cc,fresult.dd);
			if (CheckAbort (HERE)) return;
		}  
	}
}

//******************************************************************
// Title: CXCPUTestCPUInternalSpeed
//
// Abstract: Computes the internal speed of the CPU by comparing the
// rate of the known system clock to the rate of the CPU Time Stamp Counter
//
// Uses config parameters:
//
//******************************************************************
IMPLEMENT_TESTUNLOCKED (CPU, cpu_internal_speed, 5)
{
	int iCPUUpper, iCPULower;

	iCPUUpper = iCPUInternalSpeed + (int)((float)iCPUInternalSpeed * ((float)iCPUTolerance/100.0));
	iCPULower = iCPUInternalSpeed - (int)((float)iCPUInternalSpeed * ((float)iCPUTolerance/100.0));

//	#ifdef _DEBUG
		static int CpuIntSpd;
		CpuIntSpd++;
		ReportDebug(BIT0, _T("CPUInternalSpeed Test - Loop %d"), CpuIntSpd);
//	#endif

	#define CPU_NUM_TIMES 20
	double b, c, results[CPU_NUM_TIMES], sorted_results[CPU_NUM_TIMES];
	int i, j, k, cpu_speed;
	DWORD e;
	LARGE_INTEGER speed_v, speed_w, speed_x, speed_y, speed_z;

	cpu_speed = 0;
	QueryPerformanceFrequency(&speed_v);
	for(i=0; i < CPU_NUM_TIMES; i++)					// # of tests to make - pick median
	{
// CLI and STI are priveledged instructions in Windows 2000, so we should probably put a pre-compiler directive
// in here to only include the instructions when building an XBOX application.
//		__asm cli  // This should be enough to get full CPU time for this thread in a single-process OS
		QueryPerformanceCounter(&speed_x);
		__asm rdtsc									// read TimeStampCounter
		__asm mov speed_y.HighPart, edx
	    __asm mov speed_y.LowPart, eax
	    Sleep(15);									// wait X milliseconds
	    QueryPerformanceCounter(&speed_w);
	    __asm rdtsc									// Read TimeStampCounter again
	    __asm mov speed_z.HighPart, edx
	    __asm mov speed_z.LowPart, eax
//		__asm sti
		results[i]=(double)(speed_w.QuadPart-speed_x.QuadPart)/speed_v.QuadPart;
		results[i]=(double)(speed_z.QuadPart-speed_y.QuadPart)/results[i];
	}
	// Sort array - will pick median value
	for (i=0; i < CPU_NUM_TIMES; i++)		// find largest value	 
	{
		for (j=0, b=0, k=0; j < CPU_NUM_TIMES; j++)
		{
			if (results[j]>b)
			{
				b=results[j];
			    k=j;
			}
		}
	    // move into sorted_results[i]
	    sorted_results[i]=b;
	    results[k]=0;
	}

	// round speed value 
	c=sorted_results[CPU_NUM_TIMES/2];
	c=c/1000;
	e=(int)c/1000;

/*	
	d=e % 100;			// retrieve last two digits
	e=e/100;			// retrieve first digit
	if (d >= 0 && d <=2) 
		d=0;
	else if (d >= 22 && d <= 27)
		d=25;
	else if (d>=30 && d <= 35)
		d=33;
	else if (d >= 46 && d <= 52)
		d=50;
	else if (d >= 63 && d <= 69)
		d=66;
	else if (d >= 71 && d <= 77)
		d=75;
	else if (d >= 96 && d <= 100)
		d=100;
	// otherwise, just leave d the way it is
	cpu_speed=(int)((e*100)+d);

	// compare speeds
	if (iCPUInternalSpeed != cpu_speed)
	{
		err_NOMATCH_ORIGANDNEW_DD(iCPUInternalSpeed, cpu_speed);
		if (CheckAbort (HERE)) return;
	}
*/
	cpu_speed = e;
	if ((cpu_speed > iCPUUpper) || (cpu_speed < iCPULower))
	{
		err_NOMATCH_ORIGANDNEW_DDD(cpu_speed, iCPULower, iCPUUpper);
		if (CheckAbort (HERE)) return;
	}
}
