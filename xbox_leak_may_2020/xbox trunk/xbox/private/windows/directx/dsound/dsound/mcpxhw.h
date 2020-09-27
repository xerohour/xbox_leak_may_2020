/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mcpxhw.h
 *  Content:    MCP-X hardware definitions.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  01/09/01    dereks  Created.
 *
 ****************************************************************************/

#ifndef __MCPXHW_H__
#define __MCPXHW_H__

#include "nv_papu.h"
#include "nv_uap.h"

#define MCPX_HW_PCI_VENDOR_ID 0x10DE
#define MCPX_HW_PCI_DEVICE_ID 0x01B0

//
// DONT CHANGE SCRATCH SIZES WITHOUT CHANGING XGPIMAGE\DSP.h and DSP\INCLUDE\IO.H!!!!!
// dsp code depends on the value for the EP input dma
//

#define MCPX_GLOBAL_PROC_SCRATCH_SIZE              DSP_AUDIO_DATA_SCRATCHOFFSET  

//
// FX can potentially use up to a megabyte of scratch space
//

#define MCPX_HW_MAX_FX_SCRATCH_PAGES               512

//
// DSP memory spaces are addressable from within the mcpx register space
// the offsets fo GP p, x and y mem are below
//

#define MCPX_HW_GPXMEM_REG_OFFSET (XPCICFG_APU_MEMORY_REGISTER_BASE_0+0x30000)
#define MCPX_HW_GPYMEM_REG_OFFSET (XPCICFG_APU_MEMORY_REGISTER_BASE_0+0x36000)
#define MCPX_HW_GPPMEM_REG_OFFSET (XPCICFG_APU_MEMORY_REGISTER_BASE_0+0x3A000)

//
// internal DSP offsets, in words
//
#define MCPX_HW_GPMEMOFSET_MIXBIN00                 0x1400      // VP mixbins are mapped to offset 0x1400 of GP X mem.

#define MCPX_HW_MAX_NOTIFIERS (MCPX_HW_MAX_VOICES * 4 + 2)
#define MCPX_HW_NOTIFIERS_PER_VOICE 4

#define MCPX_SW_DEFAULT_HRTF_HEADROOM 4
#define MCPX_HW_DEFAULT_TRACKING 0xFFF

//
// encode proc output buffer defines
//

#define MCPX_HW_EP_STEP_SIZE    256
#define AC3_BUFFER_COUNT        2      // so we can ping-pong
#define AC3_FRAME_SIZE          MCPX_HW_EP_STEP_SIZE
#define AC3_CONTAINER_SIZE      4
#define AC3_NUM_CHANNELS        6
#define MCPX_HW_AC3_NUM_INPUT_PAGES     (((AC3_BUFFER_COUNT * AC3_FRAME_SIZE * AC3_CONTAINER_SIZE * AC3_NUM_CHANNELS) + PAGE_SIZE-1)  / PAGE_SIZE)

//
// multipass buffer pages
//

#define MCPX_HW_MULTIPASS_NUM_PAGES ((((DSMIXBIN_FXSEND_LAST-DSMIXBIN_FXSEND_FIRST+1)*128)+PAGE_SIZE-1)/PAGE_SIZE)

//
// Misc MCP-X values
//

#define MCPX_MAX_VOICE_CHANNELS             3
                                                
#define MCPX_ALIGN_16K                      0x4000
                                            
#define MCPX_MAX_VALID_ADDRESS              0x8000000
                                            
#define MCPX_BASEFREQ                       48000
#define MCPX_BASEFREQF                      48000.0f
#define MCPX_ONEOVERBASEFREQ                2.0833333333333E-5f
                                            
#define MCPX_HW_MAX_VOICES                  256
#define MCPX_HW_MAX_3D_VOICES               64
#define MCPX_HW_MAX_2D_VOICES               (MCPX_HW_MAX_VOICES - MCPX_HW_MAX_3D_VOICES)

#define MCPX_HW_FIRST_3D_VOICE              0
#define MCPX_HW_FIRST_2D_VOICE              MCPX_HW_MAX_3D_VOICES
                                            
#define MCPX_HW_MAX_PRD_ENTRIES_PER_SSL     16
#define MCPX_HW_SSLS_PER_VOICE              2
#define MCPX_HW_MAX_PRD_ENTRIES_PER_VOICE   (MCPX_HW_MAX_PRD_ENTRIES_PER_SSL * MCPX_HW_SSLS_PER_VOICE)
#define MCPX_HW_MAX_SSL_PRDS                (MCPX_HW_MAX_VOICES * MCPX_HW_MAX_PRD_ENTRIES_PER_VOICE)

#define MCPX_HW_3DFILTERS_PER_VOICE         2

#define MCPX_HW_MAX_SUBMIX_BINS             32

#define MCPX_HW_MAX_BUFFER_PRDS             2048

#define MCPX_HW_FRAME_SIZE_SAMPLES          32
#define MCPX_HW_FRAME_LENGTH_US             667

#define MCPX_HW_MAX_VOICE_MIXBINS           8

///////////////////////////////////////////////////////////////////////////////
//
// list of methods that the RM will write to the
// output Q in the scratch memory, for communication
// with the GP
//
///////////////////////////////////////////////////////////////////////////////

/*
NV_OGP_SET_EFFECT - Method to set an effect in the GP

NV_OGP_SET_EFFECT_INDEX - Index of the effect that has changed.  The GP
whould use this index to access the Effect_Control in the scratch memory
NV_OGP_SET_EFFECT_OPERATION_RESET - A new effect has been requested by the
client.  The GP should pull this effect into the p-mem.  All information
reguired by the GP will be present in the Effect_Control[Index].
NV_OGP_SET_EFFECT_OPERATION_ENABLE - An already loaded effect has been
enabled
NV_OGP_SET_EFFECT_OPERATION_DISABLE - An already loaded effect has been
disabled
NV_OGP_SET_EFFECT_OFFSET - Offset in scratch memory, of the effect control.
This offset need only be accessed on NV_OGP_SET_EFFECT_OPERATION_RESET.
*/

#define NV_OGP_SET_EFFECT                           0x00000001
#define NV_OGP_SET_EFFECT_INDEX                     15:0
#define NV_OGP_SET_EFFECT_OPERATION                 31:16
#define NV_OGP_SET_EFFECT_OPERATION_RESET           0x0
#define NV_OGP_SET_EFFECT_OPERATION_ENABLE          0x1
#define NV_OGP_SET_EFFECT_OPERATION_DISABLE         0x2
#define NV_OGP_SET_EFFECT_OFFSET                    63:32

/*
NV_OGP_SET_AC3_OUTPUT_BUFFER - method to set the AC3 output buffer offset

NV_OGP_SET_AC3_OUTPUT_BUFFER_OPER_ENABLE - the buffer described by the size
and offset is valid.  The GP should start putting de-interleaved data in it
NV_OGP_SET_AC3_OUTPUT_BUFFER_OPER_DISABLE - a previously configured AC3 output
buffer is not valid, the GP should stop putting data in it
NV_OGP_SET_AC3_OUTPUT_BUFFER_SIZE - buffer size in bytes
NV_OGP_SET_AC3_OUTPUT_BUFFER_OFFSET - offset of the buffer in GP scratch
*/

#define NV_OGP_SET_AC3_OUTPUT_BUFFER                0x00000002
#define NV_OGP_SET_AC3_OUTPUT_BUFFER_OPER           0:0
#define NV_OGP_SET_AC3_OUTPUT_BUFFER_OPER_ENABLE    0x1
#define NV_OGP_SET_AC3_OUTPUT_BUFFER_OPER_DISABLE   0x0
#define NV_OGP_SET_AC3_OUTPUT_BUFFER_SIZE           31:16
#define NV_OGP_SET_AC3_OUTPUT_BUFFER_OFFSET         63:32

///////////////////////////////////////////////////////////////////////////////
//
// data structures
//
///////////////////////////////////////////////////////////////////////////////

#pragma pack (push, _store_)
#pragma pack (1)

// interrupt registers
typedef union
{
	struct
	{	
		ULONG General		: 1;
		ULONG DeltaWarning : 1;
		ULONG RetriggerEvent : 1;		/* not in INTERRUPT_ENABLE  */
		ULONG DeltaPanic	: 1;
		ULONG FETrap		: 1;
		ULONG FENotify		: 1;
		ULONG FEVoice		: 1;
		ULONG FEMethodOverFlow : 1;
		ULONG GPMailbox	: 1;
		ULONG GPNotify		: 1;
		ULONG EPMailbox	: 1;
		ULONG EPNotify		: 1;
		ULONG				: 21;
	};
	
	ULONG uValue;
} R_INTR;

// front end
typedef union
{
	struct
	{	
		ULONG NotifyISO	: 1;
		ULONG ReadISO		: 1;
		ULONG WriteISO		: 1;
		ULONG TrapOnNotifier : 1;
		ULONG Lock			: 1;
		ULONG Mode			: 3;
		ULONG TrapReason	: 4;
        ULONG PIOClass     : 1;
        ULONG EnableLock   : 1;
		ULONG MethodOrigin : 1;
		ULONG ValidSESSL   : 1;
		ULONG ValidSESGE   : 1;
		ULONG ValidGPSGE   : 1;
		ULONG				: 14;
	};
	
	ULONG uValue;
} R_FE_CONTROL;

// setup engine
typedef union
{
	struct
	{	
		ULONG ReadISO		: 1;
		ULONG WriteISO		: 1;
		ULONG SampleReadISO : 1;
		ULONG GSCUpdate	: 2;
		ULONG DeltaWarn	: 1;
		ULONG Retriggered	: 1;
		ULONG DeltaPanic	: 1;
		ULONG				: 24;
	};
	
	ULONG uValue;
} R_SE_CONTROL;

typedef union
{
	struct
	{	
		ULONG Global   : 1;
		ULONG DSP      : 1;
		ULONG NMI      : 1;
		ULONG Abort   	: 1;
		ULONG          : 28;
	};
	
	ULONG uValue;
} R_GP_RESET;

typedef union
{
	struct
	{	
		ULONG Start		: 1;
		ULONG Ready		: 1;
		ULONG ReqIOC       : 1;
		ULONG ReqEOL   	: 1;
		ULONG ReqErr	    : 1;
		ULONG				: 27;
	};
	
	ULONG uValue;
} R_GPDMA_CONFIG;

typedef union
{
	struct
	{	
		ULONG Idle         : 1;
		ULONG Stopped      : 1;
		ULONG EnableStop   : 1;
		ULONG IntrNotify   : 1;
		ULONG				: 28;
	};
	
	ULONG uValue;
} R_GP_CONTROL;

typedef struct
{
    ULONG  uAddr;

    union
    {
        struct
        {
            ULONG Length   : 16;
            ULONG ContSize : 2;
            ULONG Samples  : 5;
            ULONG Stereo   : 1;
            ULONG Owner    : 1;
            ULONG          : 6;
            ULONG Intr     : 1;
        };

        ULONG uValue;
    }Control;

} MCP1_PRD;

// word format for DSP commands

typedef union
{
    struct
    {
        ULONG nextCommand    : 14;
        ULONG EOL            : 1;
        ULONG                : 17;
    } field;

    ULONG uValue;
} WORD0;

typedef union
{
    struct
    {
        ULONG interleave     : 1;
        ULONG dspToSys       : 1;
        ULONG IOC            : 2;
        ULONG smOffWrBack    : 1;
        ULONG smBufId        : 4;
        ULONG iso            : 1;
        ULONG smDataFormat   : 3;
        ULONG                : 1;
        ULONG increment      : 11;
        ULONG                : 7;
    } field;

    ULONG uValue;
} WORD1;

typedef union
{
    struct
    {
        ULONG count0     : 4;
        ULONG count1     : 10;
        ULONG            : 18;
    } field;

    ULONG uValue;
} WORD2;

typedef union
{
    struct
    {
        ULONG dspStart   : 14;
        ULONG nul        : 1;
        ULONG            : 17;
    } field;

    ULONG uValue;
} WORD3;

typedef union
{
    struct
    {
        ULONG smOffset   : 24;
        ULONG            : 8;
    } field;

    ULONG uValue;
} WORD4;

typedef union
{
    struct
    {
        ULONG smBaseAddr : 24;
        ULONG            : 8;
    } field;

    ULONG uValue;
} WORD5;

typedef union
{
    struct
    {
        ULONG smSizeMinus1   : 24;
        ULONG                : 8;
    } field;

    ULONG uValue;
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

typedef volatile struct _MCPX_HW_NOTIFICATION
{
	ULONG	GSCNT;
	ULONG	CurrentOffset;
	ULONG	Zero;
	UCHAR	Res0;
	UCHAR	SamplesAvailable;       // not in sync
	UCHAR	EnvelopeActive;         // not in sync
	UCHAR	Status;
} MCPX_HW_NOTIFICATION, *PMCPX_HW_NOTIFICATION;

#define NV1BA0_NOTIFICATION_STATUS_DONE_SUCCESS       0x01
#define NV1BA0_NOTIFICATION_STATUS_IN_PROGRESS        0x80

#define LOW16(x)								(USHORT)(0x0000ffff & (ULONG)(x))
#define LOW08(x)								(UCHAR)(0x000000ff & (ULONG)(x))

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

// maximum PIOFree value (formatted for method count -- real max value is 0x80)
#define PIO_METHOD_QUEUE_CLEAR                  0x20

#pragma pack (pop, _store_)

//
// voice processor register defines
//

#define VPPIOFREE DWORD volatile
#define VPPIOINFO DWORD volatile
#define VPDMAPUT DWORD volatile
#define VPDMAGET DWORD volatile
#define VPNOOPERATION DWORD volatile
#define VPSYNCHRONIZE DWORD volatile
#define VPSETTIME DWORD volatile
#define VPSETPROCESSORMODE DWORD volatile
#define VPSETANTECEDENTVOICE DWORD volatile
#define VPVOICEON DWORD volatile
#define VPVOICEOFF DWORD volatile
#define VPVOICERELEASE DWORD volatile
#define VPVOICEPAUSE DWORD volatile
#define VPSETCONTEXTDMANOTIFY DWORD volatile
#define VPGETVOICEPOSITION DWORD volatile
#define VPSETSUBMIXHEADROOM DWORD volatile
#define VPSETHRTFHEADROOM DWORD volatile
#define VPSETHRTFSUBMIX DWORD volatile
#define VPSETHRTFSUBMIXES DWORD volatile
#define VPSETVOLUMETRACKING DWORD volatile
#define VPSETPITCHTRACKING DWORD volatile
#define VPSETHRTFTRACKING DWORD volatile
#define VPSETITDTRACKING DWORD volatile
#define VPSETFILTERTRACKING DWORD volatile
#define VPSETCURRENTVOICE DWORD volatile
#define VPVOICELOCK DWORD volatile
#define VPSETVOICECFGVBIN DWORD volatile
#define VPSETVOICECFGFMT DWORD volatile
#define VPSETVOICECFGENV0 DWORD volatile
#define VPSETVOICECFGENVA DWORD volatile
#define VPSETVOICECFGENV1 DWORD volatile
#define VPSETVOICECFGENVF DWORD volatile
#define VPSETVOICECFGMISC DWORD volatile
#define VPSETVOICETARHRTF DWORD volatile
#define VPSETVOICECFGLFODLY DWORD volatile
#define VPSETVOICESSLA DWORD volatile
#define VPSETVOICESSLB DWORD volatile
#define VPSETVOICECFGBUFBASE DWORD volatile
#define VPSETVOICECFGBUFLBO DWORD volatile
#define VPSETVOICEBUFCBOFRAC DWORD volatile
#define VPSETVOICEBUFCBO DWORD volatile
#define VPSETVOICECFGBUFEBO DWORD volatile
#define VPSETVOICETARVOLA DWORD volatile
#define VPSETVOICETARVOLB DWORD volatile
#define VPSETVOICETARVOLC DWORD volatile
#define VPSETVOICELFOENV DWORD volatile
#define VPSETVOICELFOMOD DWORD volatile
#define VPSETVOICETARFCA DWORD volatile
#define VPSETVOICETARFCB DWORD volatile
#define VPSETVOICETARPITCH DWORD volatile
#define VPSETCURRENTHRTFENTRY DWORD volatile
#define VPSETHRIR DWORD volatile
#define VPSETHRIRX DWORD volatile
#define VPSETCURRENTSSLCONTEXTDMA DWORD volatile
#define VPSETCURRENTSSL DWORD volatile
#define VPSETSSLSEGMENTOFFSET DWORD volatile
#define VPSETSSKSEGMENTLENGTH DWORD volatile
#define VPSETCURRENTINBUFSGECONTEXTDMA DWORD volatile
#define VPSETCURRENTINBUFSGE DWORD volatile
#define VPSETCURRENTINBUFSGEOFFSET DWORD volatile
#define VPSETOUTBUFLEN DWORD volatile
#define VPSETOUTBUFBA DWORD volatile
#define VPSETCURRENTOUTBUFSGE DWORD volatile
#define VPSETCURRENTOUTBUFSGECONTEXTDMA DWORD volatile
#define VPSETOUTBUFSGEOFFSET DWORD volatile

#define VPSETSGEVOICENUMHACK DWORD volatile
#define VPSETSGEFULLLENHACK DWORD volatile
#define VPSETSGEADDRHACK DWORD volatile

#define VPSTARTRMDMA DWORD volatile

BEGIN_DEFINE_STRUCT()

    DWORD Reserved0000[4];                     // 0x0000 - 0x000C
    VPPIOFREE PIOFree;                         // 0x0010
    VPPIOINFO PIOInfo;                         // 0x0014
    DWORD Reserved2[10];                       // 0x0018 - 0x003C
    VPDMAPUT DMAPut;                           // 0x0040
    VPDMAGET DMAGet;                           // 0x0044
    DWORD Reserved0048[46];                    // 0x0048 - 0x00FC
    VPNOOPERATION NoOperation;                 // 0x0100
    VPSYNCHRONIZE Synchronize;                 // 0x0104
    VPSETTIME SetTime;                         // 0x0108
    VPSETPROCESSORMODE SetProcessorMode;       // 0x010C
    DWORD Reserved0110[4];                     // 0x0110 - 0x011C
    VPSETANTECEDENTVOICE SetAntecedentVoice;   // 0x0120
    VPVOICEON VoiceOn;                         // 0x0124
    VPVOICEOFF VoiceOff;                       // 0x0128
    VPVOICERELEASE VoiceRelease;               // 0x012C
    VPGETVOICEPOSITION GetVoicePosition;       // 0x0130
    DWORD Reserved0134[3];                     // 0x0134 - 0x13f
    VPVOICEPAUSE VoicePause;                   // 0x0140
    DWORD Reserved0144[7];                     // 0x0144 - 0x15c
    VPSETCURRENTHRTFENTRY SetCurrentHRTFEntry; // 0x0160
    DWORD Reserved0164[7];                     // 0x0164 - 0x017f
    VPSETCONTEXTDMANOTIFY SetContextDMANotify; // 0x0180
    DWORD Reserved0184[2];                     // 0x0184 - 0x018B
    VPSETCURRENTSSLCONTEXTDMA SetCurrentSSLContextDMA; // 0x018C
    VPSETCURRENTSSL SetCurrentSSL;             // 0x0190
    DWORD Reserved0194[27];                    // 0x0194 - 0x01fc
    VPSETSUBMIXHEADROOM SetSubMixHeadroom[32]; // 0x0200 - 0x027C
    VPSETHRTFHEADROOM SetHRTFHeadroom;         // 0x0280
    DWORD Reserved0284[3];                     // 0x0284 - 0x028C
    VPSETHRTFSUBMIX SetHRTFSubmix[4];          // 0x0290 - 0x029C
    VPSETVOLUMETRACKING SetVolumeTracking;     // 0x02A0
    VPSETPITCHTRACKING SetPitchTracking;       // 0x02A4
    VPSETHRTFTRACKING SetHRTFTracking;         // 0x02A8
    VPSETITDTRACKING SetITDTracking;           // 0x02AC
    VPSETFILTERTRACKING SetFilterTracking;     // 0x02B0
    DWORD Reserved02B4[3];                     // 0x02B4 - 0x02BC
    VPSETHRTFSUBMIXES SetHRTFSubmixes;         // 0x02C0
    DWORD Reserved02C4[13];                    // 0x02C4 - 0x02F4
    VPSETCURRENTVOICE SetCurrentVoice;         // 0x02F8
    VPVOICELOCK VoiceLock;                     // 0x02FC
    VPSETVOICECFGVBIN SetVoiceCfgVBIN;         // 0x0300
    VPSETVOICECFGFMT SetVoiceCfgFMT;           // 0x0304
    VPSETVOICECFGENV0 SetVoiceCfgENV0;         // 0x0308
    VPSETVOICECFGENVA SetVoiceCfgENVA;         // 0x030C
    VPSETVOICECFGENV1 SetVoiceCfgENV1;         // 0x0310
    VPSETVOICECFGENVF SetVoiceCfgENVF;         // 0x0314
    VPSETVOICECFGMISC SetVoiceCfgMISC;         // 0x0318
    VPSETVOICETARHRTF SetVoiceTarHRTF;         // 0x031C
    VPSETVOICESSLA SetVoiceSSLA;               // 0x0320
    DWORD Reserved0324[11];                    // 0x0324 - 0x034C
    VPSETVOICECFGLFODLY SetVoiceCfgLFODLY;     // 0x0350
    DWORD Reserved0354[2];                     // 0x0354 - 0x0358
    VPSETVOICESSLB SetVoiceSSLB;               // 0x035C
    VPSETVOICETARVOLA SetVoiceTarVOLA;         // 0x0360
    VPSETVOICETARVOLB SetVoiceTarVOLB;         // 0x0364
    VPSETVOICETARVOLC SetVoiceTarVOLC;         // 0x0368
    VPSETVOICELFOENV SetVoiceLFOENV;           // 0x036C
    VPSETVOICELFOMOD SetVoiceLFOMOD;           // 0x0370
    VPSETVOICETARFCA SetVoiceTarFCA;           // 0x0374
    VPSETVOICETARFCB SetVoiceTarFCB;           // 0x0378
    VPSETVOICETARPITCH SetVoiceTarPitch;       // 0x037C
    DWORD Reserved0360[8];                     // 0x0380 - 0x039C
    VPSETVOICECFGBUFBASE SetVoiceCfgBufBase;   // 0x03A0
    VPSETVOICECFGBUFLBO SetVoiceCfgBufLBO;     // 0x03A4
    DWORD Reserved03A8[11];                    // 0x03A8 - 0x03D0
    VPSETVOICEBUFCBOFRAC SetVoiceBufCBOFrac;   // 0x03D4
    VPSETVOICEBUFCBO SetVoiceBufCBO;           // 0x03D8
    VPSETVOICECFGBUFEBO SetVoiceCfgBufEBO;     // 0x03DC
    DWORD Reserved03E0[8];                     // 0x03E0 - 0x03FC
    VPSETHRIR SetHRIR[15];                     // 0x0400 - 0x0438
    VPSETHRIRX SetHRIRX;                       // 0x043C
    DWORD Reserved0440[112];                   // 0x0440 - 0x05FC
    struct {
        VPSETSSLSEGMENTOFFSET Offset;          // 0x0600 - 0x07F8
        VPSETSSKSEGMENTLENGTH Length;          // 0x0604 - 0x07FC
    } SetSSLSegment[64];                       // 0x0600 - 0x07FC
    VPSETCURRENTINBUFSGECONTEXTDMA SetCurrentInBufSGEContextDMA; // 0x0800
    VPSETCURRENTINBUFSGE SetCurrentInBufSGE;   // 0x0804
    VPSETCURRENTINBUFSGEOFFSET SetCurrentInBufSGEOffset; // 0x0808
    DWORD Reserved080C[509];                   // 0x080C - 0x0FFC
    struct {
        VPSETOUTBUFBA  BA;                     // 0x1000 - 0x1018
        VPSETOUTBUFLEN Len;                    // 0x1004 - 0x101C
    } SetOutBuf[4];                            // 0x1000 - 0x101C
    DWORD Reserved1020[504];                   // 0x1020 - 0x17FC
    VPSETCURRENTOUTBUFSGE SetCurrentOutBufSGE; // 0x1800
    VPSETCURRENTOUTBUFSGECONTEXTDMA SetCurrentOutBufSGEContextDMA; // 0x1804
    VPSETOUTBUFSGEOFFSET SetOutBufSGEOffset;   // 0x1808

    VPSETSGEVOICENUMHACK  SetSGEVoiceNumHack;  // 0x180c  --- Not a real function (trace use only)
    VPSETSGEFULLLENHACK  SetSGEFullLenHack;    // 0x1810  --- Not a real function (trace use only)
    VPSETSGEADDRHACK  SetSGEAddrHack;          // 0x1814  --- Not a real function (trace use only)

    DWORD Reserved1818[2554];                  // 0x1818 - 0x3fff

END_DEFINE_STRUCT(MCPXVPREGSET);

#endif // __MCPXHW_H__
