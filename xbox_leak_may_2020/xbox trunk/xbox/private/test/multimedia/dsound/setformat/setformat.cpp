/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	setformat.cpp

Abstract:

	SetFormat api test

Author:

	Robert Heitkamp (robheit) 26-Oct-2001

Environment:

	Xbox only

Revision History:

	26-Oct-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "setformat.h"
#include "dsutil.h"

//------------------------------------------------------------------------------
//	The Global Logging Handle
//------------------------------------------------------------------------------
extern HANDLE g_hLog;

//------------------------------------------------------------------------------
//	Constants
//------------------------------------------------------------------------------
static const double cPi	= 3.1415926535;

//------------------------------------------------------------------------------
//	Static Functions:
//------------------------------------------------------------------------------
void* MakeSineWave(double, WORD, DWORD, WORD, DWORD&);

//------------------------------------------------------------------------------
//	setformat_BasicTest
//------------------------------------------------------------------------------
HRESULT
setformat_BasicTest(void)
/*++

Routine Description:

    Basic Test for setformat

Arguments:

    None

Return Value:

    S_OK on success
    E_XX on failure

--*/
{
    HRESULT				hr	= S_OK;
	LPDIRECTSOUNDBUFFER	pBuffer;
	LPDIRECTSOUND8		pDSound;
	int					i;
	WAVEFORMATEX		wfx;
	WORD				b;
	DWORD				s;
	int					index;
	void*				pData[22];
	DWORD				size[22];
	LPDIRECTSOUNDBUFFER	pControlBuffer[22];

	// Create the DSound object
	hr = DirectSoundCreate(NULL, &pDSound, NULL);
	if(FAILED(hr))
		return hr;

	// Create the data buffers
	for(index=0, s = 8000, i=0; i<11; ++i, s+=4000)
	{
		for(b=8; b<=16; b+=8, ++index)
		{
			pData[index] = MakeSineWave(440, 1, s, b, size[index]); 
			ASSERT(pData[index]);
			pControlBuffer[index] = DSUtilMakeSineWave(pDSound, NULL, 440, 1, s, b);
			ASSERT(pControlBuffer[index]);
		}
	}

	// Create a sound buffer
	pBuffer = DSUtilMakeSineWave(pDSound, NULL, 440, 1, 48000, 16);
	ASSERT(pBuffer);

	// Loop through the different buffers, set the data and format
	for(index=0, s = 8000, i=0; i<11; ++i, s+=4000)
	{
		for(b=8; b<=16; b+=8, ++index)
		{
			// Create a new format	
			ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
			wfx.cbSize			= 0;
			wfx.wFormatTag		= WAVE_FORMAT_PCM;
			wfx.nChannels		= 1;
			wfx.nSamplesPerSec	= s;
			wfx.wBitsPerSample	= b;
			wfx.nBlockAlign		= (b / 8);
			wfx.nAvgBytesPerSec	= wfx.nSamplesPerSec * wfx.nBlockAlign;

			// Change the format and data
			ASSERT(pBuffer->SetBufferData(NULL, 0) == S_OK);
			ASSERT(pBuffer->SetFormat(&wfx) == S_OK);
			ASSERT(pBuffer->SetBufferData(pData[index], size[index]) == S_OK);

			// Play the control buffer
			ASSERT(pControlBuffer[index]->Play(0, 0, DSBPLAY_LOOPING) == S_OK);
			Sleep(1000);
			ASSERT(pControlBuffer[index]->Stop() == S_OK);

			// Start it playing again
			ASSERT(pBuffer->Play(0, 0, DSBPLAY_LOOPING) == S_OK);
			Sleep(1000);
			ASSERT(pBuffer->Stop() == S_OK);
		}
	}

	// Release
	for(i=0; i<22; ++i)
		pControlBuffer[i]->Release();

	pBuffer->Release();
	pDSound->Release();

	for(i=0; i<22; ++i)
		XPhysicalFree(pData[i]);

    return S_OK;
}

//------------------------------------------------------------------------------
//	setformat_StartTest
//------------------------------------------------------------------------------
VOID WINAPI 
setformat_StartTest( 
					IN HANDLE	LogHandle 
					)
/*++

Routine Description:

    The harness entry into the setformat tests

Arguments:

    LogHandle - a handle to a logging object

Return Value:

    None

--*/
{
    HRESULT             hr              = S_OK;

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

#ifdef CODE_COVERAGE

#endif // CODE_COVERAGE

    //
    // Test setformat in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "robheit", "MCPX", "setformat", "Basic" );
    EXECUTE( setformat_BasicTest() );

}

//------------------------------------------------------------------------------
//	setformat_EndTest
//------------------------------------------------------------------------------
VOID WINAPI 
setformat_EndTest(VOID)
/*++

Routine Description:

    The exit function for the test harness

Arguments:

    None

Return Value:

    None

--*/
{
}

//------------------------------------------------------------------------------
//	main
//------------------------------------------------------------------------------
#ifdef NOLOGGING
void 
__cdecl 
main(void)
/*++

Routine Description:

    the exe entry point

Arguments:

    None

Return Value:

    None

--*/
{
    setformat_StartTest( NULL );
    setformat_EndTest();
	DSUtilReboot();
}
#endif // NOLOGGING

//------------------------------------------------------------------------------
//	DSUtilMakeSineWave
//------------------------------------------------------------------------------
void*
MakeSineWave(
		     IN double	frequency, 
		     IN WORD	numChannels,
		     IN DWORD	nSamplesPerSec,
		     IN WORD	wBitsPerSample,
			 OUT DWORD&	size		
			 )
/*++

Routine Description:

    Creates a sine wave buffer that can be used in a call to SetBufferData

Arguments:

	IN frequency -		Frequency of sine wave in hertz, 
	IN numChannels -	Number of channels: 1, 2, 4 or 6
	IN nSamplesPerSec -	Samples per sec (22000, 41000, 48000, etc.)
	IN wBitsPerSample -	Bits per sample (8 or 16)
	OUT size -			Size of buffer (in bytes)

Return Value:

	If the buffer is created successfully, a pointer to the Buffer 
	otherwise NULL.

--*/
{
	DWORD				i;
	DWORD				index;
	double				dF;
	double				dInc;
	void*				pData		= NULL;
	DWORD				dwBytes;
	long				value;
	DWORD				c;
	LPDIRECTSOUNDBUFFER	pBuffer;
	
	// Basic error checking
	if((numChannels != 1) && (numChannels != 2) && (numChannels != 4) &&
	   (numChannels != 6))
	   return NULL;
	if((frequency > DSBFREQUENCY_MAX) || (frequency < DSBFREQUENCY_MIN))
		return NULL;

	// Setup the dsound buffer description with the waveformatex
	dwBytes	= (DWORD)((double)(nSamplesPerSec * (numChannels * wBitsPerSample / 8)) / frequency);
	dwBytes	+= (dwBytes % (numChannels * wBitsPerSample / 8));

	// Create the buffer
	pData = XPhysicalAlloc(dwBytes, MAXULONG_PTR, 0, PAGE_READWRITE);

	size	= (dwBytes / (numChannels * wBitsPerSample / 8)) / numChannels;
	dInc	= (2.0 * cPi) / (double)size;
	for(dF=0.0, index=0, i=0;  i<size; ++i, dF+=dInc)
	{
		switch(wBitsPerSample)
		{
		case 8:
			value = (long)((sin(dF) + 1.0) / 2.0 * 255.0);
			for(c=0; c<numChannels; ++c)
				((unsigned char*)pData)[index++] = (unsigned char)value;
			break;

		case 16:
			value = (long)(sin(dF) * 32767.0);
			for(c=0; c<numChannels; ++c)
				((short*)pData)[index++] = (short)value;
			break;

		}

	}

	size = dwBytes;
	return pData;
}

//------------------------------------------------------------------------------
//	Export Function Pointers for StartTest and EndTest
//------------------------------------------------------------------------------
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( setformat )
#pragma data_seg()

BEGIN_EXPORT_TABLE( setformat )
    EXPORT_TABLE_ENTRY( "StartTest", setformat_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", setformat_EndTest )
END_EXPORT_TABLE( setformat )
