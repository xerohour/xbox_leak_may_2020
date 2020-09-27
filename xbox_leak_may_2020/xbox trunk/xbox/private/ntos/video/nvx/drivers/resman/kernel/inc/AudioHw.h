#if !defined _AUDIODEF_H_
#define _AUDIODEF_H_

#pragma pack (push, _store_)
#pragma pack (1)

#include <nvtypes.h>

#define VOLU32		U032 volatile
#define VOLU16		U016 volatile
#define VOLU8		U08 volatile

// interrupt registers
typedef union
{
	struct
	{	
		VOLU32 General		: 1;
		VOLU32 DeltaWarning : 1;
		VOLU32 RetriggerEvent : 1;		/* not in INTERRUPT_ENABLE  */
		VOLU32 DeltaPanic	: 1;
		VOLU32 FETrap		: 1;
		VOLU32 FENotify		: 1;
		VOLU32 FEVoice		: 1;
		VOLU32 FEMethodOverFlow : 1;
		VOLU32 GPMailbox	: 1;
		VOLU32 GPNotify		: 1;
		VOLU32 EPMailbox	: 1;
		VOLU32 EPNotify		: 1;
		VOLU32				: 21;
	};
	
	VOLU32 uValue;
} R_INTR;

// front end
typedef union
{
	struct
	{	
		VOLU32 NotifyISO	: 1;
		VOLU32 ReadISO		: 1;
		VOLU32 WriteISO		: 1;
		VOLU32 TrapOnNotifier : 1;
		VOLU32 Lock			: 1;
		VOLU32 Mode			: 3;
		VOLU32 TrapReason	: 4;
        VOLU32 PIOClass     : 1;
        VOLU32 EnableLock   : 1;
		VOLU32 MethodOrigin : 1;
		VOLU32 ValidSESSL   : 1;
		VOLU32 ValidSESGE   : 1;
		VOLU32 ValidGPSGE   : 1;
		VOLU32				: 14;
	};
	
	VOLU32 uValue;
} R_FE_CONTROL;

// setup engine
typedef union
{
	struct
	{	
		VOLU32 ReadISO		: 1;
		VOLU32 WriteISO		: 1;
		VOLU32 SampleReadISO : 1;
		VOLU32 GSCUpdate	: 2;
		VOLU32 DeltaWarn	: 1;
		VOLU32 Retriggered	: 1;
		VOLU32 DeltaPanic	: 1;
		VOLU32				: 24;
	};
	
	VOLU32 uValue;
} R_SE_CONTROL;

typedef union
{
	struct
	{	
		VOLU32 Global   : 1;
		VOLU32 DSP      : 1;
		VOLU32 NMI      : 1;
		VOLU32 Abort   	: 1;
		VOLU32          : 28;
	};
	
	VOLU32 uValue;
} R_GP_RESET;

typedef union
{
	struct
	{	
		VOLU32 Start		: 1;
		VOLU32 Ready		: 1;
		VOLU32 ReqIOC       : 1;
		VOLU32 ReqEOL   	: 1;
		VOLU32 ReqErr	    : 1;
		VOLU32				: 27;
	};
	
	VOLU32 uValue;
} R_GPDMA_CONFIG;

typedef union
{
	struct
	{	
		VOLU32 Idle         : 1;
		VOLU32 Stopped      : 1;
		VOLU32 EnableStop   : 1;
		VOLU32 IntrNotify   : 1;
		VOLU32				: 28;
	};
	
	VOLU32 uValue;
} R_GP_CONTROL;

typedef struct
{
    VOLU32  uAddr;

    union
    {
        struct
        {
            VOLU32 Length   : 16;
            VOLU32 ContSize : 2;
            VOLU32 Samples  : 5;
            VOLU32 Stereo   : 1;
            VOLU32 Owner    : 1;
            VOLU32          : 6;
            VOLU32 Intr     : 1;
        };

        VOLU32 uValue;
    }Control;

} MCP1_PRD;

// word format for DSP commands

typedef union
{
    struct
    {
        U032 nextCommand    : 14;
        U032 EOL            : 1;
        U032                : 17;
    } field;

    U032 uValue;
} WORD0;

typedef union
{
    struct
    {
        U032 interleave     : 1;
        U032 dspToSys       : 1;
        U032 IOC            : 2;
        U032 smOffWrBack    : 1;
        U032 smBufId        : 4;
        U032 iso            : 1;
        U032 smDataFormat   : 3;
        U032                : 1;
        U032 increment      : 11;
        U032                : 7;
    } field;

    U032 uValue;
} WORD1;

typedef union
{
    struct
    {
        U032 count0     : 4;
        U032 count1     : 10;
        U032            : 18;
    } field;

    U032 uValue;
} WORD2;

typedef union
{
    struct
    {
        U032 dspStart   : 14;
        U032 nul        : 1;
        U032            : 17;
    } field;

    U032 uValue;
} WORD3;

typedef union
{
    struct
    {
        U032 smOffset   : 24;
        U032            : 8;
    } field;

    U032 uValue;
} WORD4;

typedef union
{
    struct
    {
        U032 smBaseAddr : 24;
        U032            : 8;
    } field;

    U032 uValue;
} WORD5;

typedef union
{
    struct
    {
        U032 smSizeMinus1   : 24;
        U032                : 8;
    } field;

    U032 uValue;
} WORD6;

typedef struct
{
    WORD0 w0;
    WORD1 w1;
    WORD2 w2;
    WORD3 w3;
} DSP_CONTROL;

#define INCR(x,limit)		(((x) < ((limit)-1))?((x)++):0)
#define DECR(x,limit)		((x)?((x)--):((limit)-1))

#define APU_INVALID_METHOD		0xFFFF

typedef struct _memdesc
{
	void	*pLinAddr;
	void	*pPhysAddr;
	U032	Size;
} MEM_DESC, *PMEM_DESC;


typedef struct 
{
	U032	GSCNT;
	U032	CurrentOffset;
	U032	Zero;
	U008	Res0;
	U008	SamplesAvailable;       // not in sync
	U008	EnvelopeActive;         // not in sync
	U008	Status;
} NV1BA0NOTIFICATION, *PNV1BA0NOTIFICATION;

#define LOW16(x)								(U016)(0x0000ffff & (U032)(x))
#define LOW08(x)								(U008)(0x000000ff & (U032)(x))

#define MIN(x,y)								(((x)<(y))?(x):(y))
#define MAX(x,y)								(((x)>(y))?(x):(y))

// codec defines
#define ACI_OFFSET                              0x100
#define MIXER_OFFSET                            0x000

// revision IDs of the hardware
#define REV_MCP1								0xA1

// sizes of various HW structures (in bytes)
// #define SIZEOF_PRD                              8
// #define SIZEOF_VOICE                            128
#define SIZEOF_TARGET_HRTF                      64
#define SIZEOF_CURRENT_HRTF                     96

// all EP methods are in NV_PAPU_NV1BA0 + PIO_EP_OFFSET
#define PIO_EP_OFFSET                           0x8000

// all debug methods are in NV_PAPU_NV1BA0 + PIO_DEBUG_OFFSET
#define PIO_DEBUG_OFFSET                        0x4000

#pragma pack (pop, _store_)

#endif
