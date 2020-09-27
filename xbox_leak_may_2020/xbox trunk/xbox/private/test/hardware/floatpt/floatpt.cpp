/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    memsize.c

Abstract:

    floating point tests for the hardware team
    intel source->xbox port by JGould

Environment:

    Xbox

Revision History:

--*/


#include "precomp.h"
#include "xtestlib.h"

LONG THE_END;
BOOL BIOSmapped;
BYTE *pBuffer;
//Type CPUType;
//Cache CacheRegA, CacheRegB, CacheRegC, CacheRegD;
//Features CPUFeatures;

// assign values to variables declared in cpu.h
extern "C" unsigned long PARG=0;
extern "C" double MAXERR=0.0;
extern "C" double SUMINP=0.0;
extern "C" double SUMOUT=0.0;
extern "C" double ERRCHK=0.0;



BOOL FPUStress()
{
	static LPVOID memreserved = NULL;
	unsigned long fftlen;
	char buf[100];
	unsigned long k;
	unsigned int i;
	unsigned long p;
	unsigned int ll_iters;
	char failed;
//	int iTestProgress = 0;
	failed=FALSE;
	fftlen=65536;


	// Allocate memory for the assembly code
	memreserved = VirtualAlloc ((void*)DATA_ADDR,   /* compiler complains but it works */
		THE_END - DATA_ADDR,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE);

	if (memreserved == NULL) 
	{
		return FALSE;
	}

	// Determine the range from which we'll choose an exponent to test.
	i=128; 
	p=1344999;

	
	// Check for configuration parameter
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
		ERRCHK++;
		lucas64();
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
//			sprintf (buf, ERRMSG1B, SUMINP, SUMOUT);
//			sprintf(buf,"i=%u, k=%lu, ",i,k);
			failed = TRUE;
			break;
		}


		// Make sure round off error is tolerable
		if (MAXERR > 0.40) 
		{
//			sprintf (buf, ERRMSG1C, MAXERR);
//			sprintf(buf,"k=%lu, ",k);
			failed=TRUE;
//			PrintError(ERR_ROUNDING_S,error_list[ERR_ROUNDING_S],buf);
			break;
		}

	} // end for

	// Check for failure 
	if(failed) return FALSE;
	else return TRUE;

/*
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
*/		

}

VOID WINAPI FpTestStartTest(HANDLE LogHandle)
{
    xSetComponent( LogHandle, "Hardware", "floatpt" );
    xSetFunctionName( LogHandle, "FloatPt" );
    xStartVariation( LogHandle, "FloatPt" );

	if(FPUStress()) {
		xLog( LogHandle, XLL_PASS, "The floating point unit works!");
	} else {
		xLog( LogHandle, XLL_FAIL, "The floating point unit failed???");
	}

    xEndVariation( LogHandle );
}


VOID WINAPI FpTestEndTest(VOID)
{
}


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( floatpt )
#pragma data_seg()

BEGIN_EXPORT_TABLE( floatpt )
    EXPORT_TABLE_ENTRY( "StartTest", FpTestStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", FpTestEndTest )
END_EXPORT_TABLE( floatpt )
