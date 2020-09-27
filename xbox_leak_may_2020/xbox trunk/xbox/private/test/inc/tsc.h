#ifndef __tsc_h__
#define __tsc_h__

#if XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif //def __cplusplus

#define _i __declspec(dllexport) __forceinline

	//*****************************************************************************
	//* Function:	TscInit
	//* Purpose:	Initialize the tsc routines, set the processor speed variables
	//* Params:		mhz --- the processor speed, in megahertz. 
	//*					If it is 0, 733 is used as the default
	//* Return:		TRUE
	//*****************************************************************************
BOOL TscInit(int mhz);

	//*****************************************************************************
	//* Function:	TscGetStamp
	//* Purpose:	return the current time-stamp count
	//* Params:		none
    //* Return:     an INT64 containing the number of clock cycles since reboot
	//* Note:		on a 733mhz, it will loop around in about 797 years...
	//*****************************************************************************
_i INT64 TscGetStamp();

	//*****************************************************************************
	//* Function:	TscGetDStamp
	//* Purpose:	return the current time-stamp count
	//* Params:		none
    //* Return:     an INT64 containing the number of clock cycles since reboot
	//*	Note:		This will loop every 5.8 seconds. For longer measurements, use
	//*					TscGetStamp.
	//*****************************************************************************
_i DWORD TscGetDStamp();

	//*****************************************************************************
    //****   INT64 functions                              ************************
	//**** (slower, but are accurate for 800 years or so)  ************************
	//*****************************************************************************


	//*****************************************************************************
	//* Function:	TscBegin
	//* Purpose:	start measuring time...
	//* Params:		none
    //* Return:     a INT64 that holds the clock time at start
	//* Note:		Pass the return value to TscEnd or TscCurrentTime
	//*****************************************************************************
_i INT64 TscBegin();

	//*****************************************************************************
	//* Function:	TscEnd
	//* Purpose:	stop counting time
	//* Params:		start is the time when you started measuring time
    //* Return:     a INT64 that holds the number of clock cycles since TscBegin
	//*****************************************************************************
_i INT64 TscEnd(INT64 start);

	//*****************************************************************************
	//* Function:	TscCurrentTime
	//* Purpose:	get the ellapsed time since the corresponding TscBegin
	//* Params:		start is the time when you started measuring time
    //* Return:     a INT64 that holds the number of clock cycles since TscBegin
	//*****************************************************************************
_i INT64 TscCurrentTime(INT64 start);


	//*****************************************************************************
	//****   DWORD functions                               ************************
	//**** (faster, but are only accurate for 5.8 seconds  ************************
	//*****************************************************************************


	//*****************************************************************************
	//* Function:	TscDBegin
	//* Purpose:	start measuring time...
	//* Params:		none
	//* Return:		a DWORD that holds the clock time at start
	//* Note:		Pass the return value to TscEnd or TscCurrentTime
	//*****************************************************************************
_i DWORD TscDBegin();

	//*****************************************************************************
	//* Function:	TscDEnd
	//* Purpose:	stop counting time
	//* Params:		start is the time when you started measuring time
	//* Return:		a DWORD that holds the number of clock cycles since TscBegin
	//*****************************************************************************
_i DWORD TscDEnd(DWORD start);

	//*****************************************************************************
	//* Function:	TscDCurrentTime
	//* Purpose:	get the ellapsed time since the corresponding TscBegin
	//* Params:		start is the time when you started measuring time
	//* Return:		a DWORD that holds the number of clock cycles since TscBegin
	//*****************************************************************************
_i DWORD TscDCurrentTime(DWORD start);

	//*****************************************************************************
	//* Function:   TscGetCpuidLen()
	//* Purpose:    If you're really nit-picky, this tells you how long it takes to
	//*               do a cpuid and a rdtsc, in clock-ticks. It shouldn't matter.
	//* Return:		a DWORD that holds the number of CCs it takes to cpuid & rdtsc
	//*****************************************************************************
_i DWORD TscGetCpuidLen();


	//*****************************************************************************
	//****   conversion functions                          ************************
	//****   clock-ticks (I64) to seconds, etc             ************************
	//*****************************************************************************
_i FLOAT TscTicksToFSeconds (INT64 ticks);
_i FLOAT TscTicksToFMSeconds(INT64 ticks);
_i FLOAT TscTicksToFUSeconds(INT64 ticks);

_i DWORD TscTicksToDSeconds (INT64 ticks);
_i DWORD TscTicksToDMSeconds(INT64 ticks);
_i DWORD TscTicksToDUSeconds(INT64 ticks);

_i INT64 TscTicksToSeconds (INT64 ticks);
_i INT64 TscTicksToMSeconds(INT64 ticks);
_i INT64 TscTicksToUSeconds(INT64 ticks);

_i INT64 TscFSecondsToTicks (FLOAT seconds );
_i INT64 TscFMSecondsToTicks(FLOAT mseconds);
_i INT64 TscFUSecondsToTicks(FLOAT useconds);

_i INT64 TscDSecondsToTicks (DWORD seconds );
_i INT64 TscDMSecondsToTicks(DWORD mseconds);
_i INT64 TscDUSecondsToTicks(DWORD useconds);


	//*****************************************************************************
	//****   conversion functions                          ************************
	//****   clock-ticks (DWORD) to seconds, etc           ************************
	//*****************************************************************************
_i FLOAT TscDTicksToFSeconds (DWORD ticks);
_i FLOAT TscDTicksToFMSeconds(DWORD ticks);
_i FLOAT TscDTicksToFUSeconds(DWORD ticks);

_i DWORD TscDTicksToDSeconds (DWORD ticks);
_i DWORD TscDTicksToDMSeconds(DWORD ticks);
_i DWORD TscDTicksToDUSeconds(DWORD ticks);

_i DWORD TscFSecondsToDTicks (FLOAT seconds );
_i DWORD TscFMSecondsToDTicks(FLOAT mseconds);
_i DWORD TscFUSecondsToDTicks(FLOAT useconds);

_i DWORD TscDSecondsToDTicks (DWORD seconds );
_i DWORD TscDMSecondsToDTicks(DWORD mseconds);
_i DWORD TscDUSecondsToDTicks(DWORD useconds);


#ifdef __cplusplus
}	//extern "C" {
#endif //def __cplusplus


#endif
