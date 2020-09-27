/////////////////////////////////
// Header file for NVSTAT.VxD
// FileName: NVSTAT.h - created by VxDWriter

#ifndef _NVSTAT_H
#define _NVSTAT_H

#define  NVSTAT_MAJOR_VER      1	// Major version No. of VxD
#define  NVSTAT_MINOR_VER      7    // Minor version No. of VxD
#define  NVSTAT_DEVICE_ID      0x3FD5	// Device ID for this VxD
#define  NVSTAT_INIT_ORDER     UNDEFINED_INIT_ORDER	// Initialization order

enum NVSTATIOCTLS {
// WIN95 device IO Control's are +1
#ifdef _VXDLIB_H
		GETVERSION = 0,
#else
		GETVERSION = 1,
#endif
		STARTCAPTURE,
		ENDCAPTURE,
		SETCONFIGURATION,
		GETCONFIGURATION,
		SETRING3CALLBACK,		// callback when pageflip/blt happens.
		CLEARSTATISTICS,
		INITUSERMEMORY			// has to be done in context of application
};

#ifndef NVSTATNT_IOCTL
#define NVSTATNT_IOCTL(_index_) \
    CTL_CODE (FILE_DEVICE_NVSTATNT, _index_, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif

#define IOCTL_NVSTATNT_GETVERSION					NVSTATNT_IOCTL (GETVERSION)
#define IOCTL_NVSTATNT_STARTCAPTURE					NVSTATNT_IOCTL (STARTCAPTURE)
#define IOCTL_NVSTATNT_ENDCAPTURE					NVSTATNT_IOCTL (ENDCAPTURE)
#define IOCTL_NVSTATNT_SETCONFIGURATION				NVSTATNT_IOCTL (SETCONFIGURATION)
#define IOCTL_NVSTATNT_GETCONFIGURATION				NVSTATNT_IOCTL (GETCONFIGURATION)
#define IOCTL_NVSTATNT_SETRING3CALLBACK				NVSTATNT_IOCTL (SETRING3CALLBACK)
#define IOCTL_NVSTATNT_CLEARSTATISTICS				NVSTATNT_IOCTL (CLEARSTATISTICS)
#define IOCTL_NVSTATNT_INITUSERMEMORY				NVSTATNT_IOCTL (INITUSERMEMORY)

#define DRIVER_DEVICE_NAME L"NVSTAT"
#define FILE_DEVICE_NVSTATNT  0x00008400


#define	CTRLHOTKEY	(1 << 16)
#define ALTHOTKEY	(2 << 16)
#define SHIFTHOTKEY (4 << 16)

typedef enum FRAMEENDTYPE
{
	FEFLIP = 0,
	FEBEGINEND,
	FEBLIT
} FRAMEENDTYPE;

typedef enum VERTFLAGS
{
	VERTSBMASK = 1,
	VERTSNORMALMASK = 2,
	VERTSUSERUV1 = 4,
	VERTSUSERUV0 = 8,
	VERTSSPECULAR = 16,
	VERTSDIFFUSE = 32,
	VERTSXYZ = 64,
	VERTSRHW = 128
} VERTFLAGS;

#pragma pack(push,1)
typedef struct NVSTATCNF
{
	DWORD captureSize;			// capture size in bytes/ this is static
	void *captureMem;
	DWORD logSize;				// how big the memory log size is (4k chunks)
	void *logMem;				// pointer to log memory
	DWORD logHead;				// header of log buffer for wrap mode
	DWORD logTail;				// tail of log buffer for wrap mode
	DWORD logFormat;			// 0 = off, 1 = minimal, 2 = maximum (everything)
	DWORD  logFull;				// flag when circular buffer is full
	DWORD logMissed;			// number of dwords missed while log is full
	DWORD hotKey;				// LOWWORD = scan code/ HIWORD(CTRLHOTKEY | ALTHOTKEY | SHIFTHOTKEY)
	FRAMEENDTYPE fEndType;		// how to capture/ flips/blts - Begin Scene/End Scene pair
	DWORD startFrame;			// starting Frame to capture
	DWORD endFrame;				// ending frame to capture
	DWORD frameRangeEnable;
} NVSTATCNF;
#pragma pack(pop)

#include "statdef.h"

#ifndef NVSTATDRIVER
#include "basetyps.h"
#include "d3d8types.h"
#define D3D_RENDERSTATE_MAX     D3DRS_BLENDOP
#define D3D_TEXSTAGESTATE_MAX   D3DTSS_RESULTARG
#else
// include this earlier, so we can redefine dbgTracePush
#include "nvDbg.h"
#include "d3dconst.h"
#endif

#define D3D_NUM_RENDERSTATES	(D3D_RENDERSTATE_MAX+1)
#define D3D_NUM_TEXSTAGESTATES	(D3D_TEXSTAGESTATE_MAX+1)

#define DXNUMFUNCTIONS (STAT_NUM_FUNCTIONS+D3D_NUM_RENDERSTATES+D3D_NUM_TEXSTAGESTATES)

#define START_RENDER_STATE (STAT_NUM_FUNCTIONS)
#define END_RENDER_STATE (STAT_NUM_FUNCTIONS+(D3D_NUM_RENDERSTATES))

#define D3D_LOG_PARAMETERS	0x8000					// bit to set for parameters

enum {
	D3D_LOG_ERROR = DXNUMFUNCTIONS | D3D_LOG_PARAMETERS,
	D3D_LOG_PERFORMANCE,
	D3D_LOG_DX6_PRIMITIVES,
	D3D_LOG_DX5_PRIMITIVES,
	D3D_LOG_DX7_PRIMITIVES,
	D3D_LOG_DX8_PRIMITIVES,
	D3D_LOG_FRAME_END,
	D3D_PRIM_COUNT_PARMS,
	D3D_LOG_LOCK_INDEX
};

typedef enum LOGFORMATBITS
{
	LOG_NONE_BIT = 0,							// no bits set
	LOG_MINIMUM_BIT = 1,
	LOG_MEDIUM_BIT = 2,
	LOG_MAXIMUM_BIT = 4,
	LOG_PERFORMANCE_BIT = 8
} LOGFORMATBITS;

#pragma pack(push,1)
//
// ---- structure for static clock tick counters
//
typedef struct NVDXTICK
{
	DWORD captureOn;							// 0 = No Capture, >= 1 if lock count
	FRAMEENDTYPE fEndType;						// how to capture/ flips/blts - Begin Scene/End Scene pair
	DWORD frameRangeEnable;						// whether to capture between frames
	DWORD startFrame;							// which frame to turn on the capture
	DWORD endFrame;								// when to turn off the capture
	DWORD FRCount;								// incremented hit count for range of frames
	DWORD tickCount;							// Timer tick count for this frame
	DWORD curFrame;								// current frame count for direct draw
	DWORD logSize;								// how big the memory log size is (4k chunks)
	DWORD logHead;								// header of log buffer for wrap mode
	DWORD logTail;								// tail of log buffer for wrap mode
	DWORD logFormat;
	void *logMem;								// pointer to log memory, can change so this variable is duplicated
	void *userLogMem;							// user memory in case different from kernel memory
	DWORD  logFull;								// flag when circular buffer is full
	DWORD logMissed;							    // number of dwords missed while log is full
	__int64 DXAdditiveTick[DXNUMFUNCTIONS];	// all the ticks for the capture
	DWORD DXHits[DXNUMFUNCTIONS];				// number of hits for this function
} NVDXTICK;
#pragma pack(pop)

#define SCAN_SYSREQ     0x54	// default scan-code
#define VK_SYSREQ		0x2c	// windows NT uses VK Codes.
#define SCAN_BREAK		0		// ?????

// this was added so we can include this file for Ring 3 applications.
#ifdef _VXDLIB_H
// Prototype declaration(s) for Win32 device ioctl APIs(s)
W32API_PROC GetVersion;		// Declare additional prototypes here.
W32API_PROC	StartCapture;
W32API_PROC	EndCapture;
W32API_PROC	SetConfiguration;
W32API_PROC	GetConfiguration;
W32API_PROC SetRing3Callback;
W32API_PROC ClearStatistics;

#define NVSHIFTSTATE(mask, compare) ( (~((mask) | SS_Toggle_mask)) << 16 | compare)

#endif

#ifdef NVSTATDRIVER

extern NVDXTICK *DXStatTickPtr;
void __cdecl nvStatLookupIndex(char *fmt, ...);

typedef struct STATINDEX
{
	DWORD	index;
	DWORD	nParms;
	__int64 startCount;
	__int64 outsideCount;
	STATINDEX *lastStatIndex;
} STATINDEX;

#define INVALID_INDEX 0xffffffff

#undef dbgTracePush
#define dbgTracePush																						\
	static STATINDEX StatIndex = { INVALID_INDEX, 0 };	/* Create the static class */ 						\
	CStatEnd StatEnd(&StatIndex);						/* when this routine ends call ~CStatEnd */			\
	if (gNVStatIndex != &StatIndex)						/* check for recursion */							\
	{																										\
		StatIndex.lastStatIndex = gNVStatIndex;																\
		StatIndex.outsideCount = 0;						/* reset outside count to zero */					\
	}																										\
	gNVStatIndex = &StatIndex;							/* Setup global for our lookup routine */			\
	if (DXStatTickPtr && DXStatTickPtr->captureOn >= 1)	/* if we are capturing data	*/						\
	{																										\
		DXStatTickPtr->captureOn++;						/* bump up reference count */						\
		if ((StatIndex.index != INVALID_INDEX) && (StatIndex.index < STAT_NUM_FUNCTIONS)) 					\
		{												/* if not first time through */						\
			nvStatWriteLogOpCode(1, StatIndex.index);	/* call to write opcode to log memory */			\
			if (DXStatTickPtr->logFormat & LOG_MEDIUM_BIT) /* if medium logging is enabled */				\
			{																								\
				NVStatLogMedium(StatIndex.index, 0);														\
			}																								\
			else if (DXStatTickPtr->logFormat & LOG_MAXIMUM_BIT) /* call maximum logging function */		\
			{																								\
				NVStatLogMaximum(StatIndex.index, 0);														\
			}																								\
		}																									\
		{																									\
		__asm push	eax																						\
		__asm push	ebx																						\
		__asm push	edx									/* save some register */							\
		__asm rdtsc										/* read time stamp count */							\
		__asm mov	ebx, OFFSET (StatIndex.startCount)	/* save off count into static StatIndex structure */\
		__asm mov	[ebx], eax																				\
		__asm mov	[ebx+4], edx																			\
		__asm pop	edx																						\
		__asm pop	ebx																						\
		__asm pop	eax																						\
		}																									\
	}												/* this next code replaces dbgPushTrace	with lookup */	\
	nvStatLookupIndex

extern STATINDEX *gNVStatIndex;

class CStatEnd {
public:
	STATINDEX *curSI;

	CStatEnd(STATINDEX *localSI)
	{
		curSI = localSI;
	}

	~CStatEnd()
	{
		__int64 endCount;

		// if capture is hooked up and it it is on and startCount has been initialized
		if (DXStatTickPtr && DXStatTickPtr->captureOn > 1)
		{
			__asm {
				push eax
				push ebx
				push edx
				rdtsc
				mov	dword ptr endCount, eax			/* save off end count */
				mov	dword ptr endCount+4,edx
				pop	edx
				pop	ebx
				pop	eax
			};

			if (curSI->index != INVALID_INDEX)
			{
				// turn into delta count
				endCount -= curSI->startCount;
				if (curSI->lastStatIndex != NULL)
				{
					// add our accumlated deltas to subtract out of parents
					curSI->lastStatIndex->outsideCount += endCount;
				}

				// subtract out calls to other functions we called
				endCount -= curSI->outsideCount;

				DXStatTickPtr->DXAdditiveTick[curSI->index] += endCount;
				DXStatTickPtr->DXHits[curSI->index]++;
			}

			DXStatTickPtr->captureOn--;
		}

		gNVStatIndex = curSI->lastStatIndex;
	}
};



typedef enum
{
	NVSTAT_LOCK_UNKNOWN = 0,
	NVSTAT_LOCK_PALETTE = 1,
	NVSTAT_LOCK_OPT_DEST = 2,
	NVSTAT_LOCK_VB = 3,
	NVSTAT_LOCK_AUTO_MIP = 4,
	NVSTAT_LOCK_VB_FLUSH = 5,
	NVSTAT_LOCK_UPDATE_LINEAR = 6,
	NVSTAT_LOCK_L2L_COPY = 7,
	NVSTAT_LOCK_L2O_COPY = 8,
	NVSTAT_LOCK_O2L_COPY = 9,
	NVSTAT_LOCK_O2O_COPY = 10,
	NVSTAT_LOCK_FILL_OPT = 11,
	NVSTAT_LOCK_STR_BLT = 12,
	NVSTAT_LOCK_TEX_NORMAL = 13,
	NVSTAT_LOCK_TEXMAN_EVICT = 14,
	NVSTAT_LOCK_TEXMAN_EVICT_ALL = 15,
	NVSTAT_LOCK_EXECUTE_BUFFER = 16,
	NVSTAT_LOCK_VID_TEX = 17,
	NVSTAT_LOCK_COPY_TEX = 18,
	NVSTAT_LOCK_UPDATE_OPTIMIZED = 19
} NVSTATLOCKINDEX;

extern NVSTATLOCKINDEX nvStatLockIndex;

struct _def_nv_d3d_context;
typedef _def_nv_d3d_context *PNVD3DCONTEXT;

void NVStatLogMedium(DWORD DXFunction, DWORD value);
void NVStatLogMaximum(DWORD DXFunction, DWORD value);
void nvStatWriteLogOpCode(DWORD nDWords, DWORD opCode);
void nvStatWriteLogData(DWORD nDWords, DWORD *theData);
BOOL nvStatCheckLogSpace(DWORD nDWords);
void __cdecl nvStatLogDwordData(DWORD parmEnum, int count, ...);
void nvStatLogVertexData(BOOL dx6, DWORD *pVertex);
void __stdcall nvCelsiusDispatchPrintVertex (PNVD3DCONTEXT pContext,DWORD dwPutAddress);
void __stdcall nvKelvinDispatchPrintVertex (PNVD3DCONTEXT pContext,DWORD dwPutAddress);

#define DDLOCKINDEX(LockIndex)				\
	nvStatLockIndex = LockIndex;

#define DDSTARTTICK(DXFunction)
#define DDENDTICK(DXFunction)


// FOffset come in one based, so decrement it.
#define DDTICKOFFSET(DXFunction, value)				\
{														\
	if (DXStatTickPtr != NULL)							\
	{													\
		if (DXFunction < (D3D_NUM_RENDERSTATES+D3D_NUM_TEXSTAGESTATES))			\
		{												\
			StatIndex.index = (DXFunction)+STAT_NUM_FUNCTIONS;	\
			if (DXStatTickPtr->captureOn >= 1)				\
			{												\
				nvStatWriteLogOpCode(1, StatIndex.index);	\
				if (DXStatTickPtr->logFormat & LOG_MEDIUM_BIT)	\
				{											\
					NVStatLogMedium(StatIndex.index, (value));	\
				}											\
				else if (DXStatTickPtr->logFormat & LOG_MAXIMUM_BIT)	\
				{											\
					NVStatLogMaximum(StatIndex.index, (value)); \
				}											\
			}												\
		}													\
	}														\
}	

#define DDFLIPTICKS(flipType)				\
	if (DXStatTickPtr != NULL)				\
	{										\
		if (DXStatTickPtr->fEndType == flipType)	\
		{									\
			DWORD frameTickCount;			\
											\
			frameTickCount = DXStatTickPtr->tickCount;		\
			DXStatTickPtr->tickCount = NvGetTickCount();	\
			frameTickCount = DXStatTickPtr->tickCount - frameTickCount;	\
			if (DXStatTickPtr->captureOn >= 1) \
			{								\
				nvStatWriteLogOpCode(3, D3D_LOG_FRAME_END);	\
				nvStatWriteLogData(1, &frameTickCount); \
				nvStatWriteLogData(1, &(DXStatTickPtr->curFrame)); \
			}								\
			DXStatTickPtr->curFrame++;		\
		}									\
		if (DXStatTickPtr->frameRangeEnable)	\
		{									\
			if (DXStatTickPtr->captureOn == 0)	\
			{								\
				if (DXStatTickPtr->curFrame == DXStatTickPtr->startFrame)	\
					DXStatTickPtr->captureOn = 1; \
			}								\
			else							\
			{								\
				if (DXStatTickPtr->curFrame == DXStatTickPtr->endFrame)	\
				{							\
					DXStatTickPtr->captureOn = 0;	\
					DXStatTickPtr->FRCount++; \
				}							\
			}								\
		}									\
	}

#define DDFLIPINIT()						\
	if (DXStatTickPtr != NULL)				\
	{										\
		DXStatTickPtr->curFrame = 0;		\
		DXStatTickPtr->logTail = 0;			 /* reset logging */ \
	}				

#define DDSTATLOGWORDS(x, y, z)				\
	if ((DXStatTickPtr != NULL) &&			\
		((DXStatTickPtr->logFormat & (LOG_MAXIMUM_BIT | LOG_MEDIUM_BIT)) != 0))	\
	{										\
		DWORD temp;							\
		temp = (z);							\
		nvStatWriteLogOpCode(2, x);		\
		nvStatWriteLogData(1, (DWORD *)(&temp)); \
	}										\

#else

#define DDSTARTTICK(DXFunction)
#define DDENDTICK(DXFunction)
#define DDSTARTTICKO(pContext, DXFunction, FOffset)
#define DDENDTICKO(DXFunction, FOffset)
#define DDFLIPTICKS(flipType)
#define DDFLIPINIT()
#define DDSTATLOGWORDS(x, y, z)
#define DDLOCKINDEX(LockIndex)
#define DDTICKOFFSET(DXFunction, value)

#endif

#endif
