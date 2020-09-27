// (c) Copyright 1999-2000 Intel Corp. All rights reserved.
// Intel Confidential.
//
// Title: cpu
//
// Abstract: This is the entry point into the CPU tests
//           
// History:
//
// 02/07/97 V1.00 SSS Release V1.00.
// 02/25/97 V1.10 SSS Testlib recompile.
// 03/17/97 V1.11
// 05/08/97 V2.00 MET Linked to testlib V2.00 DLL.
// 06/26/97 V2.01 JWW Added 'Stress subtest.
// 07/21/97 V2.02 JWW Added CPU.HLP file.
// 07/21/97 V2.03 JWW Added AUTOCONFIGURE.
// 01/06/98 V2.04 JWW Corrected bug in AUTOCONFIGURE.
// 				  Removed unnecessary status messages.
// 02/12/98 V2.05 JWW Modified OffSteppingsNotAllowed_X parameters
//                to OffSteppingsNotAllowed[X].	Modified help file
//				  to include Autoconfigure subtest.
// 03/12/98 V2.06 JWW Modified Autoconfigure and .snp file for V3.00.
// 08/25/98 V2.08 TRW Added SIMD features test, and cleaned up cpu clone problem in selfsense.
// 04/08/00 V2.09 SLK Corrected bug in SIMD test, added CPU speed test, cleaned up
//				  level 4 warnings, added cache and cpu text descriptions.  
//				  Renamed Katmai/MMX2 notations to SIMD.
//

#include "stdafx.h"
#include "wtlib.h"
#include "cpu.h"
#include "Prime95.h"
#include "windows.h"
#include "process.h"
#include "dmi16.h"
#include "string.h"

BOOL BIOSmapped;
BYTE *pBuffer;
Type CPUType;
Cache CacheRegA, CacheRegB, CacheRegC, CacheRegD;
Features CPUFeatures;

// assign values to variables declared in cpu.h
extern "C" unsigned long PARG=0;
extern "C" double MAXERR=0.0;
extern "C" double SUMINP=0.0;
extern "C" double SUMOUT=0.0;
extern "C" double ERRCHK=0.0;

char ERRMSG1A[]="ERROR: ILLEGAL SUMOUT";
char ERRMSG1B[]="ERROR: SUM(INPUTS) != SUM(OUTPUTS), %.10g != %.10g\n";
char ERRMSG1C[]="ERROR: ROUND OFF (%.10g) > 0.40\n";

// functions in xmm.asm, external because Microsoft C++ compiler doesn't recognize yet.
extern "C" void xmm128SHF(struct_128* fresultp);
extern "C" void xmm128CMP(void* dataf1, void* dataf2, struct_128* fresultp);
extern "C" void xmm128FLT(void* dataf1, void* dataf2, struct_128* fresultp);
extern "C" void xmm128INT(void* dataf1, struct_128* iresultp);
extern "C" void xmmtest(void);

#pragma warning (disable:4725)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
#endif

static unsigned char versionnumber[] = "version_number=3.0";	  /* compiler says never used-but is used in WinMTA */
int TestNumber;

#define NUM_TIMES 20
#define MBYTE 0x100000
#define ERR_INVALIDTESTCASE			0x0
#define ERR_CPUIDNOTSUPPORTED		0x1
#define ERR_CPUIDEXCEPTION			0x2
#define ERR_VALUENOTSAVED_S			0x3
#define ERR_ERRMSGNOTRETRIEVED		0x4
#define ERR_COULDNTREADOSVERS		0x5
#define ERR_SIMDEXCEPTION			0x6
#define ERR_PARAMETERNOTFOUND_S		0x7
#define ERR_WRONGFAMILY_D			0x8
#define ERR_WRONGMODEL_D			0x9
#define ERR_STEPPINGNOTALLOWED_X	0xA
#define ERR_MMXNOTPRESENT			0xB
#define ERR_MMXPRESENT_INVALID		0xC
#define ERR_SIMDNOTPRESENT			0xD
#define ERR_SIMDPRESENT_INVALID		0xE
#define ERR_FPUNOTPRESENT			0xF
#define ERR_FPUPRESENT_INVALID		0x10
#define ERR_MMX_MOVE				0x11
#define ERR_PENTIUMBUG_F			0x12
#define ERR_FREEINGMEMORY			0x13
#define ERR_ALLOCMEMORY				0x14
#define ERR_SIMD_NOTSUPPORT_OS		0x15	
#define ERR_COPYGROUP_S				0x16
#define ERR_SIMD_MOVE				0x17 
#define ERR_SIMD_FP_SP				0x18
#define ERR_SIMD_CMP_MAX_SUB		0x19
#define ERR_SIMD_MUL_DIV_CVT		0x1A
#define ERR_CHG_CLASS_PRIORITY		0x1B
#define	ERR_CHG_THREAD_PRIORITY		0x1C
#define	ERR_CREATE_MUTEX_D			0x1D
#define ERR_ACCESS_MUTEX_D			0x1E
#define ERR_RELEASE_MUTEX_D			0x1F
#define	ERR_NOORIGVALUE_S			0x20
#define	ERR_NOMATCH_ORIGANDNEW_DD	0x21
#define	ERR_BUSSPEED_OS				0x22	
#define ERR_LASTERRTXT_S			0x23
#define ERR_SQ_NUM_S				0x24
#define ERR_ROUNDING_S				0x25
#define ERR_FFT_FAILURE_S			0x26
#define ERR_UNEXPECTED_RESULT_S		0x27
#define ERR_INSUFF_SPACE			0x28
#define ERR_NULL_PTR				0x29
#define ERR_CACHE_DESC_KEY			0x2A
#define ERR_CPU_DESC				0x2B
#define ERR_CACHE_DESC				0x2C
#define ERR_RETRIEVING_VALUE_S		0x2D
#define ERR_DESC_SPACE				0x2E
#define ERR_WRONG_CPU_TYPE_DD		0x2F
#define ERR_LOC_PROCESSOR_INFO		0x30
#define ERR_RETRIEVING_BUS_SPEED	0x31
#define ERR_READING_SMBIOS			0x32
#define ERR_RETR_ERRMSG				0x33
#define ERR_MAPPING_BIOS			0x34
#define ERR_FINDING_PROC_STRUCTURE	0x35
#define ERR_2STEPPINGLISTS			0x36

char *error_list[]={
	"Test case number not valid.",												// 0
	"CPUID instruction not supported, unable to continue.",						// 1
	"Unexpected exception from CPUID instruction.",								// 2
	"Unable to save value %s.",													// 3
	"Unable to retrieve error message.",										// 4
	"Unable to read Operating System version.",									// 5
	"Unrecognized exception to Streaming SIMD instruction.",					// 6
	"The following parameter was not found: %s.",								// 7
	"The CPU is not of the correct family (value retrieved=%d).",				// 8
	"The CPU is not of the correct model (value retrieved=%d).",				// 9
	"Stepping is not allowed: 0x%x.",											// A
	"MMX support not present when it should be.",								// B
	"MMX support present when it should not be.",								// C
	"SIMD support not present when it should be.",								// D
	"SIMD support present when it should not be.",								// E
	"FPU unit not present when it should be.",									// F
	"FPU unit present when it should not be.",									// 10
	"MMX mov operations failed.",												// 11
	"Pentium FDIV bug detected\n Expected 4915835/3145727*3145727-4915835 = 0\nReceived %f.", // 12
	"Error Freeing Memory.",														// 13
	"Error Allocating Memory.",													// 14
	"Operating System does not support Streaming SIMD Extensions.\nWindows NT 4.0 requires SP5 or greater.\nIf you already have SP5 installed, please reinstall then retry the test.",			// 15
	"Unable to copy group %s.",													// 16
	"Streaming SIMD Register move operations failed.",							// 17
	"SIMD FP SP operations failed.",											// 18
	"SIMD CMP MAX AND SUB FP SP operations failed.",							// 19
	"SIMD MUL, DIV, CVT FP SP operations failed.",								// 1A
	"Unable to change class priority.",											// 1B
	"Unable to change thread priority.",										// 1C
	"Unable to create mutex, error code is %d.",								// 1D
	"Unable to access mutex, error code is %d.",								// 1E
	"Unable to release mutex, error code is %d.",								// 1F
	"Unable to retrieve original value of parameter %s.  Did auto-configure run?",	// 20
	"Original value (%d) did not match new value (%d).",						// 21
	"Bus Speed test must run on Windows NT or 2000 operating systems.",			// 22
	"Last error text=%s.",														// 23
	"Squaring: %s.",															// 24
	"Rounding: %s.",															// 25
	"FFT Failure: %s.",															// 26
	"Unexpected result: %s.",													// 27
	"Not enough space allocated for return value.",								// 28
	"Pointer cannot be NULL.",													// 29
	"Cannot access cache description key.",										// 2A
	"Unable to retrieve CPU description.",										// 2B
	"Unable to retrieve cache description",										// 2C
	"Unable to retrieve saved value %s.",										// 2D
	"Insufficient space to store all information.",								// 2E
	"The CPU (%d) is not of the correct type (%d).",							// 2F
	"Unable to locate Processor Information structure.",						// 30
	"Unable to retrieve bus speed.",											// 31
	"Unable to read SMBIOS table from memory.",									// 32
	"Unable to retrieve last error code and text.",								// 33
	"Unable to map BIOS.",														// 34
	"Unable to find processor structure in BIOS.",								// 35
	"Cannot have both 'Steppings Allowed' and 'Steppings Not Allowed' lists.",	// 36
	"",		// 37
	"",		// 38
	"",		// 39
	"",		// 3A
	"",		// 3B
	"",		// 3C
	"",		// 3D
	"",		// 3E
	""		// 3F
};

/////////////////////////////////////////////////////////////////////////////
// CCpuApp

BEGIN_MESSAGE_MAP(CCpuApp, CWinApp)
	//{{AFX_MSG_MAP(CCpuApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCpuApp construction

CCpuApp::CCpuApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCpuApp object

CCpuApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCpuApp initialization
#pragma optimize("g",off)//Necessary for use with in line assembly with Intel compiler

BOOL CCpuApp::InitInstance()
{
	if(!wt_initialize(&TestNumber))
		return FALSE;
	//get a test parameter common to all tests
	int status = wt_getint("loops",&status);
	// you would check status here if this was a required parameter
	status = wt_getint("faultinject",&status);
	status = wt_getint("devdebug",&status);

	if (TestNumber != 0) // ie, not autoconfig
	{
		int cpu;
		if (wt_getint ("cpu number", &cpu))
		{
			DWORD threadMask = 1;
			threadMask <<= cpu;
			SetThreadAffinityMask (GetCurrentThread (), threadMask);
			Sleep (0); // Wait for the next quantum to switch to the selected CPU
		} else {
			cpu=-1;		// unable to retrieve CPU number
		}
	}

	// Using the TestNumber passed to us we switch to the specific code for the given test.
	switch(TestNumber)
	{
		case 0:   SelfSense();			  break;
		case 1:   DisplayCPUFeatures();   break;
		case 2:   CPUFeatureCheck();      break;
		case 3:   FPUFeatures();          break;
		case 4:   MMXFeatures();          break;
		case 5:   FPUStress();            break;    
		case 6:   SIMDFeatures();         break;    
		case 7:	  CkCPUSpeed();			  break;


		default:  PrintError(ERR_INVALIDTESTCASE,error_list[ERR_INVALIDTESTCASE]);
	}

	wt_progress(100);
	wt_done();
	return FALSE;	  
}



//******************************************************************
// Title: SelfSense
// Abstract: Sets CPU configuation parameters.
//******************************************************************

void SelfSenseCPU (char *path, char *name, int cpu)
{
	int cpu_speed_val;
//	WORD bus_speed_val;			/* ADD BACK IN ONCE LIBRARY DAN OSAWA IS CREATING IS DONE */

	Sleep (1); // Wait for the next quantum to be schedualed on the selected CPU
	char PathStr[80] = "";
	
	GetCPUID();
	// Set configuration parameters
	  if(!wt_copy_group (path, name, PathStr, sizeof (PathStr))) {
		  PrintError(ERR_COPYGROUP_S, error_list[ERR_COPYGROUP_S], "CPU");
		  return;
	  }
	  if(!wt_putint(PathStr,"CPU NUMBER",cpu)) {
		  PrintError(ERR_VALUENOTSAVED_S, error_list[ERR_VALUENOTSAVED_S], "CPU NUMBER");
		  return;
	  }
	  if(!wt_putint(PathStr,"CPU TYPE",CPUType.ProcType)) {
		  PrintError(ERR_VALUENOTSAVED_S, error_list[ERR_VALUENOTSAVED_S], "CPU TYPE");
		  return;
	  }
	  if(!wt_putint(PathStr,"MODEL",CPUType.Model)) {
		  PrintError(ERR_VALUENOTSAVED_S, error_list[ERR_VALUENOTSAVED_S], "MODEL");
		  return;
	  }
	  if(!wt_putint(PathStr,"FAMILY",CPUType.Family)) {
		  PrintError(ERR_VALUENOTSAVED_S, error_list[ERR_VALUENOTSAVED_S], "FAMILY");
		  return;
	  }
	  if(!wt_putint(PathStr,"MMX",CPUFeatures.MMX)) {
		  PrintError(ERR_VALUENOTSAVED_S, error_list[ERR_VALUENOTSAVED_S], "MMX");
		  return;
	  }
	  if(!wt_putint(PathStr,"FPU",CPUFeatures.FPU)) {
		  PrintError(ERR_VALUENOTSAVED_S, error_list[ERR_VALUENOTSAVED_S], "FPU");
		  return;
	  }
	  if(!wt_putint(PathStr,"SIMD",CPUFeatures.XMM)) {
		  PrintError(ERR_VALUENOTSAVED_S, error_list[ERR_VALUENOTSAVED_S], "SIMD");
		  return;
	  }

	// ck processor speed
	cpu_speed_val=CPUSpeed();
	if(!wt_putint(PathStr, "CPU SPEED", cpu_speed_val)) {
		PrintError(ERR_VALUENOTSAVED_S, error_list[ERR_VALUENOTSAVED_S], "CPU SPEED");
		return;
	}
/*	ADD CODE BACK IN ONCE LIBRARY THAT DAN OSAWA IS CREATING IS COMPLETE
	// ck bus speed	ONLY IF OS =Windows NT/2000
	if(!IsNTor2000()) {
		wt_remove_subtest(PathStr, 0x09);
	} else { 
		if(!BusSpeed(&bus_speed_val, cpu)) {
			wt_remove_subtest(PathStr, 0x09);
			PrintError(0x31, error_list[0x31]);
			wt_done();
			return;
		}
		if(!wt_putint(PathStr, "BUS SPEED", bus_speed_val)) {
			PrintError(0x03, error_list[0x03]);
			return;
		}
	}
*/
	if(!CPUFeatures.FPU)
	{
		wt_remove_subtest(PathStr,0x03);
		wt_remove_subtest(PathStr,0x05);
	}
	if(!CPUFeatures.MMX)
		wt_remove_subtest(PathStr,0x04);

	if(!CPUFeatures.XMM) {
		wt_remove_subtest(PathStr,0x06);
	} else {		// processor reports SIMD support - does OS support also?
		if(!TrySIMD()) {	// operating system does NOT support
			wt_remove_subtest(PathStr, 0x06);
		}
	}
	wt_remove_subtest(PathStr, 0x08);
	return;
}

/***************************************************************************************\
* Title:	SelfSense
* Abstract:	
\***************************************************************************************/
void SelfSense ()
{
	char pathName[128] = "";
	char groupName [128], origGroupName[128];

	// set global variable BIOSMAPPED to FALSE to indicate that MapBios() fcn hasn't run
	BIOSmapped=FALSE;

	// First get the path 
	wt_getstring("$PATH",pathName,sizeof(pathName));
	char *name = strrchr (pathName, '.');
	strcpy (origGroupName, name? (name + 1):pathName);

/* ADD CODE BACK IN ONCE LIBRARY THAT DAN OSAWA IS CREATING IS DONE
	// BIOS should be mapped once for entire SelfSense, so map before calling SelfSenseCPU
	if(IsNTor2000()) {			// only works on Windows NT or Windows 2000 OS
		if(!MapBIOS()) {
			PrintError(0x34, error_list[0x34]);
		}
		BIOSmapped=TRUE;		// flag to stop from trying to map multiple times.
	}
*/

	// Get the process affinity mask to see how many processors are available
	DWORD processMask, systemMask, threadMask = 1;
	BOOL gotMask = GetProcessAffinityMask (GetCurrentProcess (), &processMask, &systemMask);
	if (gotMask && (processMask != systemMask))
		wt_advisory ("This process does not have access to all CPU's");

		for (int cpu = 0; cpu < 32; cpu++)
		{
			sprintf(groupName, "%s %d", origGroupName, cpu);
			if (processMask & 1) // if low bit set
			{
				if (SetThreadAffinityMask (GetCurrentThread (), threadMask))
					SelfSenseCPU (pathName, groupName, cpu);
				else
				{
					DWORD error = GetLastError();
					wt_advisory ("SetThreadAffinityMask failed for CPU %d\n%s", cpu, 
						GetLastErrorText(error));
				}
			}
			threadMask <<= 1;
			processMask >>= 1;
		}

		wt_remove_group (pathName);
}



//******************************************************************
// Title: CPUFeatureCheck
//
// Abstract: Checks the features on the CPU vs any parameters
//******************************************************************

void CPUFeatureCheck()
{
	char SteppingsParamName[256], cpu_desc[1024]="", cache_desc[1024]="";
	unsigned int SteppingsAllowed;
	unsigned int SteppingsNotAllowed;
	unsigned int Model, Family, Type;
	BOOL SteppingOK = TRUE;
	int x, locStepAllw=0, locStepNotAllw=0;
	unsigned int fpu,mmx, simd;

	GetCPUID();

	// check the family, type and model
	if (!wt_getuint("Family",&Family)){	
		PrintError(ERR_PARAMETERNOTFOUND_S, error_list[ERR_PARAMETERNOTFOUND_S], "Family");
		wt_done();
	}
	if(!wt_getuint("CPU Type", &Type)) {
		PrintError(ERR_PARAMETERNOTFOUND_S, error_list[ERR_PARAMETERNOTFOUND_S], "Type");
		wt_done();
	}
	if (!wt_getuint("Model",&Model)){	
		PrintError(ERR_PARAMETERNOTFOUND_S, error_list[ERR_PARAMETERNOTFOUND_S], "Model");
		wt_done();
	}
	// retrieve original CPU description
	if(!CPUDescription(cpu_desc, 1024, Type, Model, Family)) {
		PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
		wt_done();
	}

	if(Family != CPUType.Family){
		PrintError(ERR_WRONGFAMILY_D, error_list[ERR_WRONGFAMILY_D], Family);
	}
	if(Model != CPUType.Model) {
		PrintError(ERR_WRONGMODEL_D,error_list[ERR_WRONGMODEL_D],Model);
	}
	if(Type != CPUType.ProcType) {
		PrintError(ERR_WRONG_CPU_TYPE_DD, error_list[ERR_WRONG_CPU_TYPE_DD],Type);
	}
	wt_status("CPU description: %s", cpu_desc);

	// check for steppings not allowed
	for(x=0;x<99;x++)
	{
		sprintf(SteppingsParamName,"SteppingsNotAllowed[%d]",x);
		if (!wt_getuint(SteppingsParamName,&SteppingsNotAllowed)){
			break;
		}
		locStepNotAllw=TRUE;
		// if we found the featuresAllowed once from now on we must find a match
		if( SteppingsNotAllowed == (unsigned int)CPUType.Stepping){
			PrintError(ERR_STEPPINGNOTALLOWED_X, error_list[ERR_STEPPINGNOTALLOWED_X],CPUType.Stepping);
			wt_done();
		}
	}

	// check the steppings allowed
	for(x=0;x<99;x++)
	{
		sprintf(SteppingsParamName,"SteppingsAllowed[%d]",x);
		if (!wt_getuint(SteppingsParamName,&SteppingsAllowed)){
			break;
		}
		locStepAllw=TRUE;
		// if we found the featuresAllowed once from now on we must find a match
		SteppingOK = FALSE;
		if( SteppingsAllowed == (unsigned int)CPUType.Stepping){
			SteppingOK = TRUE;
			break;
		}
	}
	// can't have both a "Steppings Allowed" AND a "Steppings Not Allowed" list
	if (locStepAllw && locStepNotAllw) {
		PrintError(ERR_2STEPPINGLISTS, error_list[ERR_2STEPPINGLISTS]);
	}

	if (SteppingOK == FALSE) {
		PrintError(ERR_STEPPINGNOTALLOWED_X, error_list[ERR_STEPPINGNOTALLOWED_X], CPUType.Stepping);
	}
	
	// check for MMX
	if (wt_getuint("mmx",&mmx))
	{
		if(mmx != CPUFeatures.MMX)
		{
			if (CPUFeatures.MMX == 0)
			   PrintError(ERR_MMXNOTPRESENT,error_list[ERR_MMXNOTPRESENT]);
			else
			   PrintError(ERR_MMXPRESENT_INVALID,error_list[ERR_MMXPRESENT_INVALID]);
		}
	}

	// check for SIMD/mmx2
	if (wt_getuint("SIMD",&simd))
	{
		if(simd != CPUFeatures.XMM)
		{
			if (CPUFeatures.XMM == 0)
			   PrintError(ERR_SIMDNOTPRESENT,error_list[ERR_SIMDNOTPRESENT]);
			else
			   PrintError(ERR_SIMDPRESENT_INVALID,error_list[ERR_SIMDPRESENT_INVALID]);
		}
	}

	// check for FPU
	if (wt_getuint("fpu",&fpu))
	{
		if(fpu != CPUFeatures.FPU)
		{
			if (CPUFeatures.FPU == 0)
			   PrintError(ERR_FPUNOTPRESENT,error_list[ERR_FPUNOTPRESENT]);
			else
			   PrintError(ERR_FPUNOTPRESENT,error_list[ERR_FPUNOTPRESENT]);
		}
	}

	// retrieve the cache description
	if(!CacheDescription(cache_desc, 1024)) {
		PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
		wt_done();
	}
	wt_status("Cache=%s", cache_desc);
}


//******************************************************************
// Title:		MMXFeatures
// Abstract:	Test MMX commands
//******************************************************************
void mmxtest(void *TempDwordPtr, void *ResultPtr)
{
	_asm {
		; Move inst.	
		mov	eax,12345678h
		movd	mm0,eax
		movd	ebx,mm0
		movd	mm1,ebx
		mov	edi,TempDwordPtr
		movd	[EDI],mm1
		movd	mm2,[EDI]
		movq	mm3,mm2
		movd	[EDI],mm3
		movq	mm4,[EDI]
		movq	mm5,mm4
		movq	mm6,mm5
		movq	mm7,mm6
		mov	edi,ResultPtr
		movd	[edi],mm7
		emms		     ; empty MMX state
	}
	return;
}

void MMXFeatures()
{
	DWORD TempDword;
	DWORD * TempDwordPtr;
	DWORD Result;
	DWORD * ResultPtr;
	unsigned int x,y;
	int iTestProgress = 0;

	// check for an MMX unit
	GetCPUID();
	if(CPUFeatures.MMX == 0)
	{
		PrintError(ERR_MMXNOTPRESENT,error_list[ERR_MMXNOTPRESENT]);
		wt_done();
	}
	for(y=1;y<100;y++)
	{

    	// Record progress
		if(y%10 == 0)
		{
			iTestProgress+=10;
			wt_progress(iTestProgress);
		}

		
		for(x=0;x<1000;x++)
		{
			ResultPtr = &Result;
			TempDwordPtr = &TempDword;
			mmxtest(TempDwordPtr,ResultPtr);
			if(Result != 0x12345678)
			{
				PrintError(ERR_MMX_MOVE,error_list[ERR_MMX_MOVE]);
				break;
			}
		} //x
		Sleep(100);
	} //y
}
//******************************************************************
// Title: MMXDTC
// Abstract:
//******************************************************************
void MMXDTC()
{
}
//******************************************************************
// Title:	CPUfault
// Abstract:
//******************************************************************
void CPUfault()
{
}
//******************************************************************
// Title:	DisplayCPUFeatures
// Abstract:
//******************************************************************
void DisplayCPUFeatures()
{
	char cache_desc[1024]="", cpu_desc[1024]="";

	GetCPUID();
	wt_status("Type reg=0x%x, Feature reg = 0x%x",CPUType.reg,CPUFeatures.reg);
	wt_status("Stepping=0x%x\nModel=0x%x\nFamily=0x%x\nType=0x%x",CPUType.Stepping, CPUType.Model, CPUType.Family, CPUType.ProcType);
	if(!CacheDescription(cache_desc, 1024)) {
		PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
		return;
	}
	if(!CPUDescription(cpu_desc, 1024, CPUType.ProcType, CPUType.Model, CPUType.Family)) {
		PrintError(ERR_CPU_DESC, error_list[ERR_CPU_DESC]);
		return;
	}
	
	wt_status("CPU Description=%s\nCache Description=%s", cpu_desc, cache_desc);
	
	if(CPUFeatures.FPU)
		wt_status("Floating Point unit present");
	else
		wt_status("Floating Point unit not present");
	if(CPUFeatures.MMX)
		wt_status("MMX unit present");
	else
		wt_status("MMX unit not present");
	if(CPUFeatures.XMM)
		wt_status("SIMD unit present");
	else
		wt_status("SIMD unit not present");
}


//******************************************************************
// Title:	FPUFeatures
// Abstract:
//******************************************************************
void FPUFeatures()
{
	double num = 4195835;
	double den = 3145727;
	double result;
	unsigned int x,y;

	// check for an MMX unit
	GetCPUID();

	if(CPUFeatures.FPU == 0)
	{
		PrintError(ERR_FPUNOTPRESENT,error_list[ERR_FPUNOTPRESENT]);
		wt_done();
	}
	
	for(y=1;y<100;y++)
	{
	for(x=1;x<100000;x++)
	{
	// Pentium Bug Check

	result = num / den * den - num;

	/*
	 * Bad to test floating point for equality, allows some error.
	 * A real failure returns -256 anyway.
	 */
	if(result < -0.000000001 || result > 0.000000001)
	{
		PrintError(ERR_PENTIUMBUG_F, error_list[ERR_PENTIUMBUG_F],result);
		wt_done();
	}
	} //x
	Sleep(100);
	} //y
}
//******************************************************************
// Title: PrintError
// Abstract: Tacks the module number and testnumber on to our error messages
//******************************************************************

void PrintError(unsigned int ecode, LPCTSTR format_string,...)
{
	char ErrorMessage[4096];
	unsigned int mcode = 0x85;
	va_list argptr;

	mcode = (mcode << 20) + (TestNumber <<12) + ecode;
		
	va_start(argptr,format_string);
	vsprintf(ErrorMessage,format_string,argptr);
	wt_error(mcode,ErrorMessage);

}


//******************************************************************
// Title: FPUStress 
// Abstract: Computes Mersenne Prine Numbers
//           Uses Dr. Richard Crandall's algorithm
//******************************************************************
void FPUStress()
{
	static LPVOID memreserved = NULL;
	unsigned long fftlen;
	char buf[100];
	unsigned long k;
	unsigned int i;
	unsigned long p;
	unsigned int ll_iters;
	char failed;
	int iTestProgress = 0;
	failed=FALSE;
	fftlen=65536;

		
	// Load the appropriate EXE file and setup
	THE_END = THE_END_64;
	lucas = lucas64;
	
	// Now we know how much memory to reserve

	//freeing memory
	if (memreserved != NULL) 
	{
		if (!VirtualFree (memreserved,0, MEM_RELEASE))
		{		
			PrintError(ERR_FREEINGMEMORY,error_list[ERR_FREEINGMEMORY]);
			wt_done();				
		}
			
		memreserved = NULL;
	}


	// Allocate memory for the assembly code
	memreserved = VirtualAlloc ((void*)DATA_ADDR,   /* compiler complains but it works */
		THE_END - DATA_ADDR,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE);

	if (memreserved == NULL) 
	{
			PrintError(ERR_ALLOCMEMORY,error_list[ERR_ALLOCMEMORY]);
			wt_done();				
	}

	// Determine the range from which we'll choose an exponent to test.
	i=128; 
	p=1344999;

	
	// Check for configuration parameter
	if(!wt_getuint("ITERATIONS",&ll_iters))
			ll_iters = 400;

	// Now run Lucas setup
	PARG = p;
	setup64 ();

	// Init data area with a pre-determined value
	//clear the memory
	for (k=0; k<fftlen; k++) 
		set_fft_value(p, fftlen,k,0);
	set_fft_value (p, fftlen, 0, 4);


	// Do Lucas-Lehmer iterations
	for (k = 0; k <ll_iters; k++) 
	{
		
		// Record progress
		if(k%40 == 0)
		{
			iTestProgress+=10;
			wt_progress(iTestProgress);
		}
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
			sprintf(buf,"i=%u, k=%lu, ",i,k);
			failed=TRUE;
			break;
		}

		// Check that the sum of the input numbers squared is approximately
		// equal to the sum of unfft results.  Note that as the sum of the
		// inputs approaches zero the more error we must tolerate. 

		if (SUMOUT < SUMINP-128.0 || SUMOUT > SUMINP+128.0) 
		{
			sprintf (buf, ERRMSG1B, SUMINP, SUMOUT);
			sprintf(buf,"i=%u, k=%lu, ",i,k);
			PrintError(ERR_SQ_NUM_S,error_list[ERR_SQ_NUM_S],buf);
			failed=TRUE;
			break;
		}


		// Make sure round off error is tolerable
		if (MAXERR > 0.40) 
		{
			sprintf (buf, ERRMSG1C, MAXERR);
			sprintf(buf,"k=%lu, ",k);
			failed=TRUE;
			PrintError(ERR_ROUNDING_S,error_list[ERR_ROUNDING_S],buf);
			break;
		}

	} // end for

	// Check for failure 
	if (failed==TRUE) 
	{
		sprintf(buf,"FFT length=%lu --> ",fftlen);
		PrintError(ERR_FFT_FAILURE_S,error_list[ERR_FFT_FAILURE_S],buf);
		// FAILED!
	} 
	
	else 
	{
		sprintf(buf,"FFT length=%lu --> ",fftlen);
		wt_status("Passed FPU stress test!");
		// PASSED!	
	}
		

}

/***************************************************************************************\
* Title:	GetLastErrorText
* Abstract:	Retrieve error text matching code input as parameter to function.
\***************************************************************************************/
CString GetLastErrorText(DWORD dwLastError)
{
	DWORD dwRet;
	LPTSTR lpszTemp = NULL;
	CString msg;

	dwRet = ::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		 FORMAT_MESSAGE_FROM_SYSTEM    |
		 FORMAT_MESSAGE_ARGUMENT_ARRAY |
		 FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL,
		dwLastError,
		LANG_NEUTRAL,
		(LPTSTR)&lpszTemp,
		0,
		NULL);

	// supplied buffer is not long enough
	if(dwRet)
	{
		lpszTemp[lstrlen(lpszTemp) - 2] = TEXT('\0');  //remove cr+lf
		msg.Format(TEXT("%s (0x%x)"),lpszTemp,dwLastError);
		LocalFree((HLOCAL)lpszTemp);
	}

	return msg;
}



/***************************************************************************************\
* Title:	SIMDFeatures 
* Abstract:	Invoke SIMD commands, make sure appropriate values are returned.  If 
*			processor doesn't support SIMD, this function should have been disabled.
*			Error if this function is called and the processor does NOT support SIMD.
\***************************************************************************************/
void SIMDFeatures()
{
	char buf[100];

	unsigned int x,y;
	int iTestProgress = 0;
	
	// check for a SIMD unit
	GetCPUID();
	if(CPUFeatures.XMM == 0)
	{
		PrintError(ERR_SIMDNOTPRESENT,error_list[ERR_SIMDNOTPRESENT]);
		wt_done();
	}
	// Check for Streaming SIMD Extension OS Support
	if(!TrySIMD()) {
		wt_advisory(error_list[ERR_SIMD_NOTSUPPORT_OS]);
		wt_done();
	}
	for(y=1;y<10;y++)
	{
		iTestProgress+=10;
		wt_progress(iTestProgress);

		for (x=0;x<10;x++)
		{

			/* set up 128 bit INT data and INT results */			
			struct_128 data_int = {1,2,3,4};
			void* datai = &data_int;			/* compiler says never used - used in ASM */
			struct_128 iresult;
			struct_128* iresultp = &iresult;		/* compiler says never used - used in ASM */
			xmm128INT(datai, iresultp);
			if(memcmp(&data_int,&iresult,16) != 0)
			{
				PrintError(0x17,error_list[0x17]);
				sprintf(buf,"Expected=%08lx%08lx%08lx%08lx\nReceived=%08lx%08lx%08lx%08lx",data_int.d,data_int.c,data_int.b,data_int.a,
				iresult.d,iresult.c,iresult.b,iresult.a);
				PrintError(ERR_UNEXPECTED_RESULT_S, error_list[ERR_UNEXPECTED_RESULT_S], buf);
				break;
			}
			struct_128 data_float1 = {(float)1.2, (float)5.6, (float)9.7, (float)0.4};	/* compiler says never used - used in ASM */
			struct_128 data_float2 = {(float)5.3, (float)2.1, (float)10.2, (float)7.0};	/* compiler says never used - used in ASM */
			struct_128 exp_fresult = {(float)2.5495097637176514, (float)2.100000, (float)4.460942, (float)2.7202942371368408};
			struct_128 fresult;
			void* dataf1 = &data_float1;			/* compiler says never used - used in ASM */
			void* dataf2 = &data_float2;			/* compiler says never used - used in ASM */
			struct_128* fresultp = &fresult;			/* compiler says never used - used in ASM*/
			xmm128FLT(dataf1, dataf2, fresultp);
			if(memcmp(&exp_fresult,&fresult,16) != 0)
			{
				PrintError(0x18,error_list[0x18]);
				sprintf(buf,"Expected=%08.20f%08.20f%08.20f%08.20f\nReceived=%08.20f%08.20f%08.20f%08.20f",exp_fresult.aa,exp_fresult.bb,exp_fresult.cc,exp_fresult.dd,
				fresult.aa,fresult.bb,fresult.cc,fresult.dd);
				PrintError(ERR_UNEXPECTED_RESULT_S,error_list[ERR_UNEXPECTED_RESULT_S],buf);
				break;
			}
			xmm128CMP(dataf1, dataf2, fresultp);
			struct_128 check2 = {(float)-4.10000038146972660000, (float)-2.1, (float)-.5, (float)-6.6};
			if(memcmp(&check2,&fresult,16) != 0)
			{
				PrintError(0x19,error_list[0x19]);
				sprintf(buf,"Expected=%08.20f%08.20f%08.20f%08.20f\nReceived=%08.20f%08.20f%08.20f%08.20f",check2.aa,check2.bb,check2.cc,check2.dd,
				fresult.aa,fresult.bb,fresult.cc,fresult.dd);
				PrintError(ERR_UNEXPECTED_RESULT_S,error_list[ERR_UNEXPECTED_RESULT_S],buf);
				break;
			}  
			xmm128SHF(fresultp);
			struct_128 check3 = {20,42,42,40};
			if(memcmp(&check3,&fresult,16) != 0)
			{
				PrintError(0x1A,error_list[0x1A]);
				sprintf(buf,"Expected=%08.20f%08.20f%08.20f%08.20f\nReceived=%08.20f%08.20f%08.20f%08.20f",check3.aa,check3.bb,check3.cc,check3.dd,
				fresult.aa,fresult.bb,fresult.cc,fresult.dd);
				PrintError(ERR_UNEXPECTED_RESULT_S,error_list[ERR_UNEXPECTED_RESULT_S],buf);
				break;
			}  


		} //x
		Sleep(100);
	} //y
}


/*****************************************************************************************\
* Title:	CPUSpeed
* Abstract:	Set process and thread priority to realtime.  Run test several times,
*			choose median because first 4-6 times run slower than the correct speed.
*			***If speed is reported incorrectly, increase the NUM_TIMES value to get past these
*			first incorrect values.***  Speed is rounded to nearest normal value if it's
*			within a small range.  If it doesn't fit into that range, the actual value
*			is returned.
*			NOTE:  This test is not guaranteed to work as expected on laptop systems, due
*			to the CPU speed/power management policies used by laptops.
\*****************************************************************************************/
int CPUSpeed() {
	double b, c, d, results[NUM_TIMES], sorted_results[NUM_TIMES];
	int i, j, k, cpu_speed=0, origThreadPriority;
	DWORD e, wait_result;
	HANDLE hMutex;
	LARGE_INTEGER speed_v, speed_w, speed_x, speed_y, speed_z;

	origThreadPriority=GetThreadPriority(GetCurrentThread());

	if(!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) {
		PrintError(ERR_CHG_CLASS_PRIORITY,error_list[ERR_CHG_CLASS_PRIORITY]);
		return 0;
	}

	if(!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL)) {
		PrintError(ERR_CHG_THREAD_PRIORITY, error_list[ERR_CHG_THREAD_PRIORITY]);
		return 0;
	}
	Sleep(1);
    hMutex=CreateMutex(NULL, TRUE, "WINMTA_REALTIME_MUTEX");
	if (hMutex==NULL) {		// error creating mutex
		PrintError(ERR_CREATE_MUTEX_D, error_list[ERR_CREATE_MUTEX_D], GetLastError());
		return 0;
	}
	wait_result=WaitForSingleObject(hMutex, 60000);		// wait 1 min. for mutex to be released
	if (wait_result==WAIT_TIMEOUT || wait_result==WAIT_FAILED) {
		PrintError(ERR_ACCESS_MUTEX_D, error_list[ERR_ACCESS_MUTEX_D], wait_result);
		return 0;
	}
	QueryPerformanceFrequency(&speed_v);
	for(i=0; i < NUM_TIMES; ++i) {					// # of tests to make - pick median
		QueryPerformanceCounter(&speed_x);
		__asm rdtsc									// read TimeStampCounter
		__asm mov speed_y.HighPart, edx
	    __asm mov speed_y.LowPart, eax
	    Sleep(15);									// wait X milliseconds
	    QueryPerformanceCounter(&speed_w);
	    __asm rdtsc									// Read TimeStampCounter again
	    __asm mov speed_z.HighPart, edx
	    __asm mov speed_z.LowPart, eax
		results[i]=(double)(speed_w.QuadPart-speed_x.QuadPart)/speed_v.QuadPart;
		results[i]=(double)(speed_z.QuadPart-speed_y.QuadPart)/results[i];
	}
	// set priorities back to original
	if(!SetThreadPriority(GetCurrentThread(), origThreadPriority)) {
		PrintError(ERR_CHG_THREAD_PRIORITY, error_list[ERR_CHG_THREAD_PRIORITY]);
		return 0;
	}
	if(!SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS)) {
		PrintError(ERR_CREATE_MUTEX_D, error_list[ERR_CREATE_MUTEX_D]);
		return 0;
	}		
	  
	// Sort array - will pick median value
	for (i=0; i < NUM_TIMES; ++i) {		// find largest value	 
		for (j=0, b=0, k=0; j < NUM_TIMES; ++j) {
			if (results[j]>b) {
				b=results[j];
			    k=j;
			}
		}
	    // move into sorted_results[i]
	    sorted_results[i]=b;
	    results[k]=0;
	}

	// round speed value 
	c=sorted_results[NUM_TIMES/2];
	c=c/1000;
	e=(int)c/1000;
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
#pragma warning(disable:4244)  // compiler complained of double->int conversion.  It's OK here.
	cpu_speed=(int)(e*100)+d;
#pragma warning(default:4244)

	if(!ReleaseMutex(hMutex)) {		
		PrintError(ERR_RELEASE_MUTEX_D, error_list[ERR_RELEASE_MUTEX_D], GetLastError());
		return 0;
	}
	CloseHandle(hMutex);	

	return cpu_speed;
}


/*****************************************************************************************\
* Title:	CkCPUSpeed
* Abstract:	Retrieves CPU SPEED value (should have been input through autoconfig).
*			Runs CPUSpeed test again, and compares the values.  Outputs error if they
*			don't match, or if the original value wasn't input.	
\******************************************************************************************/
void CkCPUSpeed() {
	int orig_cpu_speed, new_cpu_speed;

	if(!wt_getint("CPU SPEED", &orig_cpu_speed)) {
		PrintError(ERR_NOORIGVALUE_S, error_list[ERR_NOORIGVALUE_S], "(CPU SPEED)");
		return;
	}

	// check current speed
	new_cpu_speed=CPUSpeed();

	// compare speeds
	if (orig_cpu_speed != new_cpu_speed) {
		PrintError(ERR_NOMATCH_ORIGANDNEW_DD, error_list[ERR_NOMATCH_ORIGANDNEW_DD], orig_cpu_speed, new_cpu_speed);
		return;
	}

	return;
}




/*******************************************************************************************\
* Title:	CPUDescription
* Abstract:	Pass in a string to contain the return value of the processor, length of
*			string, and Type, Model and Family parameters.  Function uses those
*			parameters to identify the name of the processor.  Returns FALSE if error
*			occurs, TRUE otherwise.
\*******************************************************************************************/
BOOL CPUDescription(char *desc, int desc_len, int Type, int Model, int Family) {
	char *NoMatch="New processor added, CPUDescription() test needs to be updated.";

	// make sure desc_len contains enough characters for the max option
	if (desc_len < 125) {
		PrintError(ERR_INSUFF_SPACE,error_list[ERR_INSUFF_SPACE]);
		return FALSE;
	}

	// desc can't be null
	if(desc==NULL) {
		PrintError(ERR_NULL_PTR, error_list[ERR_NULL_PTR]);
		return FALSE;
	}
	// Depending on the values, copy a string containing the name of the processor
	switch(Type) {
	case 0:
		switch(Family) {
		case 4:
			switch(Model) {
			case 0:
			case 1:
				if(!AppendData(desc, desc_len, "Intel 486 DX processor")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			case 2:
				if(!AppendData(desc, desc_len, "Intel 486 SX processor")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			case 3:
				if(!AppendData(desc, desc_len, "Intel 487 processor")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			case 4:
				if(!AppendData(desc, desc_len, "Intel 486 SL processor")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			case 5:
				if(!AppendData(desc, desc_len, "Intel SX2 processor")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			case 7:
				if(!AppendData(desc, desc_len, "Write-Back Enhanced Intel DX2 processor")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			case 8:
				if(!AppendData(desc, desc_len, "Intel DX4 processor")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			default:
				if(!AppendData(desc, desc_len, NoMatch)) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			}	
			break;		// end switch(Model) inside Type 00, Family 0100
		case 5:
			switch(Model) {
			case 1:
			case 2:
				if(!AppendData(desc, desc_len, "Pentium processor")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			case 4:
				if(!AppendData(desc, desc_len, "Pentium processor with MMX technology.")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			default:
				if(!AppendData(desc, desc_len, NoMatch)) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			} 
			break;		// end switch(Model) inside Type 00, Family 0101
		case 6:
			switch(Model) {
			case 1:
				if(!AppendData(desc, desc_len, "Pentium Pro processor.")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			case 3:
				if(!AppendData(desc, desc_len, "Pentium II processor, model 3.")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			case 5:
				if(!AppendData(desc, desc_len, "Pentium II processor, model 5, Pentium II Xeon processor, model 5, or Intel Celeron processor, model 5.")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			case 6:
				if(!AppendData(desc, desc_len, "Intel Celeron Processor, model 6.")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			case 7:
				if(!AppendData(desc, desc_len, "Pentium III processor, model 7, or Pentium III Xeon processor, model 7.")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			case 8:
				if(!AppendData(desc, desc_len, "Pentium III processor, model 8, Pentium III Xeon processor, model 8, or Intel Celeron processor, model 8.")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			default:
				if(!AppendData(desc, desc_len, NoMatch)) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			}
			break;
		default:
			if(!AppendData(desc, desc_len, NoMatch)) {
				PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
				return FALSE;
			}
			break;
		}					// end switch(Family) inside Type 00
		break;				// end Type 00
		case 1:
		switch(Family) {
		case 5:
			switch(Model) {
			case 1:
			case 2:
				if(!AppendData(desc, desc_len, "Pentium OverDrive processor for Pentium processor.")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			case 3:
				if(!AppendData(desc, desc_len, "Pentium OverDrive processors for Intel 486 processor-based systems.")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			case 4:
				if(!AppendData(desc, desc_len, "Pentium OverDrive processor with MMX technology for Pentium processor.")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;

			default:
				if(!AppendData(desc, desc_len, NoMatch)) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			} 
			break;		// end case Family 0101, Type 01
		case 6:
			switch(Model) {
			case 3:
				if(!AppendData(desc, desc_len, "Intel Pentium II OverDrive processor.")) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			default:
				if(!AppendData(desc, desc_len, NoMatch)) {
					PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
					return FALSE;
				}
				break;
			}
			break;
		}
		break;			// end switch(Family) inside Type 01
		default:		// no type match found
			if(!AppendData(desc, desc_len, NoMatch)) {
				PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
				return FALSE;
			}
			break;
	}
 	return TRUE;
}


/******************************************************************************************\
* Title:	CacheDescription
* Abstract:	Copy cache description into string passed into function.  Returns TRUE if passed,
*			FALSE if failed.
\******************************************************************************************/
void cacheinfo(DWORD ptrCacheRegA, DWORD ptrCacheRegB, DWORD ptrCacheRegC, DWORD ptrCacheRegD)
{
	_asm {
		mov		eax, 2		; ask for the cache information
		cpuid
		mov		edi, ptrCacheRegA
		mov		[edi], eax
		mov		edi, ptrCacheRegB
		mov		[edi], ebx
		mov		edi, ptrCacheRegC
		mov		[edi], ecx
		mov		edi, ptrCacheRegD
		mov		[edi], edx
	}
	return;
}

BOOL CacheDescription(char *desc, int desc_len) {
	int stop=0, count=0;

	
	if (desc==NULL) {
		PrintError(ERR_NULL_PTR,error_list[ERR_NULL_PTR]);
		return FALSE;
	}

	while (!stop) {
		++count;				 
		// get the cache information
		__try{		// necessary in case processor doesn't support CPUID call
			cacheinfo((DWORD)&(CacheRegA.reg), (DWORD)&(CacheRegB.reg), (DWORD)&(CacheRegC.reg), (DWORD)&(CacheRegD.reg));
		}				
		__except(EXCEPTION_EXECUTE_HANDLER) {
			if(_exception_code()==STATUS_ILLEGAL_INSTRUCTION) {
				wt_error(0x01,error_list[0x01]);
				return FALSE;
			}
			// if we get here, an unexpected error has occurred - what?
			PrintError(ERR_CPUIDEXCEPTION, error_list[ERR_CPUIDEXCEPTION]);
			return FALSE;
		}
		// check EAX bits 0-7.  Indicate how many times CPUID needs to be called with 2 in EAX
		if((unsigned)count >= CacheRegA.byte0) {
			stop=1;
		}

		if(!(CacheRegA.byte3 & 0x80)) {		// register values valid only if bit 31==0
			// no need to find match on byte0 - used to tell # calls/CPUID necessary
			if(!FindCacheMatch(desc, desc_len, CacheRegA.byte1)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;
			}
			if(!FindCacheMatch(desc, desc_len, CacheRegA.byte2)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;
			}
			if(!FindCacheMatch(desc, desc_len, CacheRegA.byte3)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;
			}
		}

		if(!(CacheRegB.byte3 & 0x80)) {		// register values valid only if bit 31==0
			if(!FindCacheMatch(desc, desc_len, CacheRegB.byte0)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;
			}
			if(!FindCacheMatch(desc, desc_len, CacheRegB.byte1)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;
			}
			if(!FindCacheMatch(desc, desc_len, CacheRegB.byte2)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;
			}
			if(!FindCacheMatch(desc, desc_len, CacheRegB.byte3)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;	
			}
		}

		if (!(CacheRegC.byte3 & 0x80)) {		// register values valid only if bit 31==0
			if(!FindCacheMatch(desc, desc_len, CacheRegC.byte0)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;
			}
			if(!FindCacheMatch(desc, desc_len, CacheRegC.byte1)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;
			}
			if(!FindCacheMatch(desc, desc_len, CacheRegC.byte2)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;
			}
			if(!FindCacheMatch(desc, desc_len, CacheRegC.byte3)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;
			}
		}

		if (!(CacheRegD.byte3 & 0x80)) {		// register values valid only if bit 31==0
			if(!FindCacheMatch(desc, desc_len, CacheRegD.byte0)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;
			}
			if(!FindCacheMatch(desc, desc_len, CacheRegD.byte1)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;
			}
			if(!FindCacheMatch(desc, desc_len, CacheRegD.byte2)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;
			}
			if(!FindCacheMatch(desc, desc_len, CacheRegD.byte3)) {
				PrintError(ERR_CACHE_DESC, error_list[ERR_CACHE_DESC]);
				return FALSE;
			}
		}

	} // end of while(!stop) loop
	return TRUE;
}


/**************************************************************************************\
* Title:	AppendData
* Abstract:	Append second data string to first if there's room in the first.  If not,
*			return FALSE.  Otherwise, return TRUE.
\***************************************************************************************/
BOOL AppendData(char* data1, int data1_len, char* data2) {
	int len1, len2;

	if(data1==NULL || data2==NULL) {
		PrintError(ERR_NULL_PTR, error_list[ERR_NULL_PTR]);
		return FALSE;
	}

	len1=strlen(data1);
	len2=strlen(data2);
	if(len1+len2+1 < data1_len) {
		if(len1 != 0) {
			strcat(data1, "\n");
		}
		strcat(data1, data2);
	} else {
		return FALSE;
	}
	return TRUE;
}



/***************************************************************************************\
* Title:	FindCacheMatch
* Abstract:	Search for match to "masked_data" criteria.  Return FALSE if error occurs,
*			TRUE otherwise.
\***************************************************************************************/
BOOL FindCacheMatch(char *desc, int desc_len, __int16 masked_data)
{
	switch(masked_data) {
	case 0x00:		// these first cases are for minor caches, don't bother to list them
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x06:
	case 0x08:
	case 0x0A:
	case 0x0C:
		break;
	case 0x40:
		if(!AppendData(desc, desc_len, "No L2 cache.")) {
			PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
			return FALSE;
		}
		break;
	case 0x41:
		if(!AppendData(desc, desc_len, "Unified cache, 32 byte cache line, 4-way set associative, 128K.")) {
			PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
			return FALSE;
		}
		break;
	case 0x42:
		if(!AppendData(desc, desc_len, "Unified cache, 32 byte cache line, 4-way set associative, 256K.")) {
			PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
			return FALSE;
		}
		break;
	case 0x43:
		if(!AppendData(desc, desc_len, "Unified cache, 32 byte cache line, 4-way set associative, 512K.")) {
			PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
			return FALSE;
		}
		break;
	case 0x44:
		if(!AppendData(desc, desc_len, "Unified cache, 32 byte cache line, 4-way set associative, 1M.")) {
			PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
			return FALSE;
		}
		break;
	case 0x45:
		if(!AppendData(desc, desc_len, "Unified cache, 32 byte cache line, 4-way set associative, 2M.")) {
			PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
			return FALSE;
		}
		break;
	case 0x82:
		if(!AppendData(desc, desc_len, "Unified cache, 32 byte cache line, 8 way set associative, 256K.")) {
			PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
			return FALSE;
		}
		break;
	default:
		if(!AppendData(desc, desc_len, "New cache description, CacheDescription() function needs to be updated.")) {
			PrintError(ERR_DESC_SPACE, error_list[ERR_DESC_SPACE]);
			return FALSE;
		}
		break;
	}
	return TRUE;
}


/***************************************************************************************\
* Title:	IsNTor2000
* Abstract:	Check version of current operating system.  Return TRUE is Windows NT or 
*			Windows 2000, returns FALSE if unable to retrieve error or if OS is NOT
*			Windows NT or 2000.
\***************************************************************************************/
BOOL IsNTor2000() {
	OSVERSIONINFO OS;

	// get version information
	OS.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(!GetVersionEx(&OS)) {
		return FALSE;					// unable to retrieve version info
	}
	if(OS.dwPlatformId!=VER_PLATFORM_WIN32_NT) {
		return FALSE;					// version info didn't meet requirements
	}
	return TRUE;
}


/***************************************************************************************\
* Title:	GetCPUID
* Abstract:	Try to call CPUID instruction, and store value in CPUType and CPUFeatures
*			structures.  If an exception occurs, and the exception is 
*			STATUS_ILLEGAL_INSTRUCTION, print an error message.  Otherwise, let the
*			exception filter up higher.
\***************************************************************************************/
void getcpuid(DWORD	ptrCPUTYPE, DWORD ptrCPUFEATURES)
{
	_asm {
		mov	eax, 1			; ask for the feature information
		cpuid
		mov	edi,ptrCPUTYPE
		mov	[edi], eax
		mov	edi,ptrCPUFEATURES
		mov	[edi], edx
	}
		return;
}

void GetCPUID() {
	__try{		// necessary in case processor doesn't support CPUID call
		getcpuid((DWORD)&(CPUType.reg), (DWORD)&(CPUFeatures.reg));
	  }
	__except(GetExceptionCode()==STATUS_ILLEGAL_INSTRUCTION) {
		wt_error(ERR_CPUIDNOTSUPPORTED,error_list[ERR_CPUIDNOTSUPPORTED]);
		return;
	}
}


/***************************************************************************************\
* Title:	TrySIMD
* Abstract:	Try a SIMD instruction to see if OS supports SIMD instructions.  Returns
*			TRUE if support exists, FALSE if support does not exist.
\***************************************************************************************/
BOOL TrySIMD() {
	  __try {
		  xmmtest();
	  }
	  __except(GetExceptionCode()==STATUS_ILLEGAL_INSTRUCTION) { // exception occurred
		  return FALSE;
	  }
	  return TRUE;
}
