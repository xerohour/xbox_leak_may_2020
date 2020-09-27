#include "tsc.h"

//#define rdtsc __asm __emit 0fh __asm __emit 031h
//#define cpuid __asm __emit 0fh __asm __emit 0a2h

/*****************Global Varaibles********************************************/

//if you really want to get accurate, you could remove the few cc's it takes to 
//call cpuid, which is really only used to keep code aligned, and to make sure
//rdtsc doesn't get executed before the other stuff does...
DWORD gCpuidLen = 0;		

//processor speed, in megahertz
INT gMhz = 733;

//processor speed, in kilohertz
INT gKhz = 733000;

//processor speed, in hertz
INT gHz  = 733000000;

//processor speed, nanoseconds per clock cycle
FLOAT giMhz = (1 / (FLOAT)gMhz);

//processor speed, milliseconds per clock cycle
FLOAT giKhz = (1 / (FLOAT)gKhz);

//processor speed, seconds per clock cycle
FLOAT giHz  = (1 / (FLOAT)gHz );



/*****************Functions***************************************************/

//*****************************************************************************
//* Function:	TscInit
//* Purpose:	Initialize the tsc routines, set the processor speed variables
//* Params:		mhz --- the processor speed, in megahertz. 
//*					If it is 0, 733 is used as the default
//* Return:		TRUE
//*****************************************************************************

BOOL TscInit(int mhz)
{
	if(mhz == 0) mhz = 733;

	_asm{
		cpuid
		rdtsc
		cpuid
		rdtsc
		mov ebx, eax
		cpuid
		rdtsc
		sub eax, ebx
		mov dword ptr [gCpuidLen], eax
	}
	gMhz = mhz;
	gKhz = mhz * 1000;
	gHz  = mhz * 1000000;
	giMhz = (1 / (FLOAT)gMhz);
	giKhz = (1 / (FLOAT)gKhz);
	giHz  = (1 / (FLOAT)gHz );
	return TRUE;
}

//*****************************************************************************
//* Function:	TscGetStamp
//* Purpose:	return the current time-stamp count
//* Params:		none
//* Return:     an INT64 containing the number of clock cycles since reboot
//* Note:		on a 733mhz, it will loop around in about 797 years...
//*****************************************************************************
_i INT64 TscGetStamp()
{
    INT64 i;
	_asm{
		cpuid
		rdtsc
		mov dword ptr [i], eax
		mov dword ptr [i+4], edx
	}
	return i;
}


//*****************************************************************************
//* Function:	TscGetDStamp
//* Purpose:	return the current time-stamp count
//* Params:		none
//* Return:     an INT64 containing the number of clock cycles since reboot
//*	Note:		This will loop every 5.8 seconds. For longer measurements, use
//*					TscGetStamp.
//*****************************************************************************
_i DWORD TscGetDStamp()
{
	DWORD i;
	_asm{
		cpuid
		rdtsc
		mov dword ptr [i], eax
	}
	return i;
}

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
_i INT64 TscBegin() { return TscGetStamp(); }
//*****************************************************************************
//* Function:	TscEnd
//* Purpose:	stop counting time
//* Params:		start is the time when you started measuring time
//* Return:     a INT64 that holds the number of clock cycles since TscBegin
//*****************************************************************************
_i INT64 TscEnd(INT64 start) { return TscGetStamp() - start; }
//*****************************************************************************
//* Function:	TscCurrentTime
//* Purpose:	get the ellapsed time since the corresponding TscBegin
//* Params:		start is the time when you started measuring time
//* Return:     a INT64 that holds the number of clock cycles since TscBegin
//*****************************************************************************
_i INT64 TscCurrentTime(INT64 start) { return TscGetStamp() - start; }



//*****************************************************************************
//****   DWORD functions                               ************************
//**** (faster, but are only accurate for 5.8 seconds  ************************
//*****************************************************************************


//*****************************************************************************
//* Function:	TscBegin
//* Purpose:	start measuring time...
//* Params:		none
//* Return:		a DWORD that holds the clock time at start
//* Note:		Pass the return value to TscEnd or TscCurrentTime
//*****************************************************************************
_i DWORD TscDBegin() { return TscGetDStamp(); }
//*****************************************************************************
//* Function:	TscDEnd
//* Purpose:	stop counting time
//* Params:		start is the time when you started measuring time
//* Return:		a DWORD that holds the number of clock cycles since TscBegin
//*****************************************************************************
_i DWORD TscDEnd(DWORD start) { return TscGetDStamp() - start; }
//*****************************************************************************
//* Function:	TscDCurrentTime
//* Purpose:	get the ellapsed time since the corresponding TscBegin
//* Params:		start is the time when you started measuring time
//* Return:		a DWORD that holds the number of clock cycles since TscBegin
//*****************************************************************************
_i DWORD TscDCurrentTime(DWORD start) { return TscGetDStamp() - start; }

//*****************************************************************************
//* Function:   TscGetCpuidLen()
//* Purpose:    If you're really nit-picky, this tells you how long it takes to
//*               do a cpuid and a rdtsc, in clock-ticks. It shouldn't matter.
//* Return:		a DWORD that holds the number of CCs it takes to cpuid & rdtsc
//*****************************************************************************
_i DWORD TscGetCpuidLen() { return gCpuidLen; }


//*****************************************************************************
//****   conversion functions                          ************************
//****   clock-ticks (I64) to seconds, etc             ************************
//*****************************************************************************
_i FLOAT TscTicksToFSeconds (INT64 ticks) {return (FLOAT)ticks * giHz; }
_i FLOAT TscTicksToFMSeconds(INT64 ticks) {return (FLOAT)ticks * giKhz;}
_i FLOAT TscTicksToFUSeconds(INT64 ticks) {return (FLOAT)ticks * giMhz;}

_i DWORD TscTicksToDSeconds (INT64 ticks) {return (DWORD)(ticks / gHz );}
_i DWORD TscTicksToDMSeconds(INT64 ticks) {return (DWORD)(ticks / gKhz);}
_i DWORD TscTicksToDUSeconds(INT64 ticks) {return (DWORD)(ticks / gMhz);}

_i INT64 TscTicksToSeconds (INT64 ticks) {return (INT64)(ticks / gHz );}
_i INT64 TscTicksToMSeconds(INT64 ticks) {return (INT64)(ticks / gKhz);}
_i INT64 TscTicksToUSeconds(INT64 ticks) {return (INT64)(ticks / gMhz);}


_i INT64 TscFSecondsToTicks (FLOAT seconds ) {return (INT64)(seconds  * gHz); }
_i INT64 TscFMSecondsToTicks(FLOAT mseconds) {return (INT64)(mseconds * gKhz);}
_i INT64 TscFUSecondsToTicks(FLOAT useconds) {return (INT64)(useconds * gMhz);}

_i INT64 TscDSecondsToTicks (DWORD seconds ) {return (INT64)seconds  * gHz; }
_i INT64 TscDMSecondsToTicks(DWORD mseconds) {return (INT64)mseconds * gKhz;}
_i INT64 TscDUSecondsToTicks(DWORD useconds) {return (INT64)useconds * gMhz;}


//*****************************************************************************
//****   conversion functions                          ************************
//****   clock-ticks (DWORD) to seconds, etc           ************************
//*****************************************************************************
_i FLOAT TscDTicksToFSeconds (DWORD ticks) {return (FLOAT)ticks * giHz; }
_i FLOAT TscDTicksToFMSeconds(DWORD ticks) {return (FLOAT)ticks * giKhz;}
_i FLOAT TscDTicksToFUSeconds(DWORD ticks) {return (FLOAT)ticks * giMhz;}

_i DWORD TscDTicksToDSeconds (DWORD ticks) {return ticks / gHz; }
_i DWORD TscDTicksToDMSeconds(DWORD ticks) {return ticks / gKhz;}
_i DWORD TscDTicksToDUSeconds(DWORD ticks) {return ticks / gMhz;}

_i DWORD TscFSecondsToDTicks (FLOAT seconds ) {return (DWORD)(seconds  * gHz); }
_i DWORD TscFMSecondsToDTicks(FLOAT mseconds) {return (DWORD)(mseconds * gKhz);}
_i DWORD TscFUSecondsToDTicks(FLOAT useconds) {return (DWORD)(useconds * gMhz);}

_i DWORD TscDSecondsToDTicks (DWORD seconds ) {return (DWORD)seconds  * gHz; }
_i DWORD TscDMSecondsToDTicks(DWORD mseconds) {return (DWORD)mseconds * gKhz;}
_i DWORD TscDUSecondsToDTicks(DWORD useconds) {return (DWORD)useconds * gMhz;}

