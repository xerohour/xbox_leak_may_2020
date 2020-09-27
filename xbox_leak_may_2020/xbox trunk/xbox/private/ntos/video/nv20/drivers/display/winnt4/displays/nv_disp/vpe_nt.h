


// this will bring over alot of types which are useful

#include <dx95type.h>

#ifndef DEBUG
	#ifndef DPF
	    #define DPF         1 ? (void)0 : (void)
	#endif
#else
/*
 * Msg
 * Defined in ddsamp.c
 */
extern void __cdecl DPF( LPSTR szFormat, ... );
#endif

/* Debug stuff */
#ifdef DEBUG
	#define ASSERT(ParamAssert) 	(ParamAssert)? \
			(void)0:DPF("Assertion failed in %s at line %i\n", __FILE__, __LINE__)

	#define strASSERT( Condition, string )	(Condition)?\
			(void)0:DPF("Assertion failed in %s at line %i, %s", __FILE__, __LINE__, strAssert)		

#else
	#define Assert( ParamAssert )
	#define ASSERT( ParamAssert )
	#define strASSERT( Condition, string )
#endif
	
/* Prototypes for VPE_NT.c */
                                   
DWORD PASCAL CanCreateVideoPort32 (LPDDHAL_CANCREATEVPORTDATA lpInput);
DWORD __stdcall CreateVideoPort32 (LPDDHAL_CREATEVPORTDATA lpInput);
DWORD __stdcall FlipVideoPort32 (LPDDHAL_FLIPVPORTDATA lpInput);
DWORD __stdcall GetVideoPortBandwidth32 (LPDDHAL_GETVPORTBANDWIDTHDATA lpInput);
DWORD __stdcall GetVideoPortInputFormat32 (LPDDHAL_GETVPORTINPUTFORMATDATA lpInput);
DWORD __stdcall GetVideoPortOutputFormat32 (LPDDHAL_GETVPORTOUTPUTFORMATDATA lpInput);
DWORD __stdcall GetVideoPortField32 (LPDDHAL_GETVPORTFIELDDATA lpInput);
DWORD __stdcall GetVideoPortLine32 (LPDDHAL_GETVPORTLINEDATA lpInput);
DWORD __stdcall GetVideoPortConnectInfo (LPDDHAL_GETVPORTCONNECTDATA lpInput);
DWORD __stdcall DestroyVideoPort32 (LPDDHAL_DESTROYVPORTDATA lpInput);
DWORD __stdcall GetVideoPortFlipStatus32 (LPDDHAL_GETVPORTFLIPSTATUSDATA lpInput);
DWORD __stdcall UpdateVideoPort32 (LPDDHAL_UPDATEVPORTDATA lpInput);
DWORD __stdcall WaitForVideoPortSync32 (LPDDHAL_WAITFORVPORTSYNCDATA lpInput);
DWORD __stdcall GetVideoSignalStatus32 ( LPDDHAL_GETVPORTSIGNALDATA lpInput);	


// This structure will be used to store all of the OS specific stuff which might 
// otherwise be part of the vpSurfaces  structure

typedef struct _OSSpecificGlobals {
	long	dwVideoPortsAvailable;		// the number of video ports available.. inited as one usually
												// and decremented per useage
												
												
} OSSpecificGlobals;


