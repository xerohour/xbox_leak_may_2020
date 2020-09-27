// *************************************************************************
//
//	DSP_Util.h		Support functions for DSP System memory operations
//
//	Written by Gints Klimanis
//
// *************************************************************************
//
#ifndef __DSP_UTIL_H__
#define	__DSP_UTIL_H__

#include "util.h"

#define kAudioFX_SMAddress_CommandNode		 9000
#define kAudioFX_SMAddress_ModuleBlock		 9500
#define kAudioFX_SMAddress_StateBlock		10000
#define kAudioFX_SMAddress_CodeBlock		12000

// ****************************************************************
// Command/Status for driver <-> DSP messaging.   This is done
//							every few frames by DMAing the system 
//							memory command block
// ****************************************************************
typedef struct dspcommandnode {
	Int24	stateID;		// Values: kDSPState ID's
	Int24	commandID;		// Values: kDSPCommand ID's
	Int24	moduleIndex;	// {0..N-1}, used only for commands that operate on 
							// a single module

	Int24	moduleCount;	// {0 .. kMaxDSPModules-1}

	Int24	dspModuleBlock;
	Int24	dspModuleStateBlock;
	Int24	dspModuleCodeBlock;

	Int24	dspModuleBlockSize;
	Int24	dspModuleStateBlockSize;
	Int24	dspModuleCodeBlockSize;

	Int24	misc1;
	Int24	misc2;
	Int24	misc3;

} DSPCOMMANDNODE;

void DefaultCommandNode(DSPCOMMANDNODE *d);

// Bit 0
#define kDSPCommandID_IgnoreMe						0x00
#define kDSPCommandID_CheckForCommand				0x01
// Bit 1
#define kDSPCommandID_StopDSPList					0x00
#define kDSPCommandID_RunDSPList					0x02
// Bit 4:2
#define kDSPCommandID_LoadDSPBlock_ModuleList		0x04
#define kDSPCommandID_LoadDSPBlock_Code				0x08
#define kDSPCommandID_LoadDSPBlock_State			0x10


// The following commands access parts of the State block
// Reset and Prepare will be the same if the DSP relies on the
// driver to do all of the high->low level parameter conversions
#define kDSPCommandID_ResetAllDSPModuleStates		5
#define kDSPCommandID_UpdateAllDSPModuleParameters	6
#define kDSPCommandID_PrepareAllDSPModules			7	// Prepare = Update + Reset

// For "Single" commands, include module index in command node
#define kDSPCommandID_ResetOneDSPModuleState		8	
#define kDSPCommandID_UpdateOneDSPModuleParameters	9	
#define kDSPCommandID_PrepareOneDSPModule			10	

// Debug Stuff
// misc1= RAM Word Address (0-based, not mapped address)
// misc2= # words
// misc2= SM Byte Address
#define kDSPCommandID_DumpXRAM		10		
#define kDSPCommandID_DumpYRAM		11	
#define kDSPCommandID_DumpPRAM		12	

// dspStateWord Bit 0 is waiting processing bit
#define kDSPStateID_WaitingBitCommandMask		0x1	// 1=waiting, 0=processing
#define kDSPStateID_ProcessingCommand			0x0	// 1=waiting, 0=processing
#define kDSPStateID_WaitingForCommand			0x1	// 1=waiting, 0=processing

// dspStateWord Bit 1 is running bit
#define kDSPStateID_ModuleListRunningBitMask	0x2	// 1=running, 0=stopped
#define kDSPStateID_ModuleListRunning			0x2	// 1=running, 0=stopped
#define kDSPStateID_ModuleListStopped			0x0	// 1=running, 0=stopped


// ****************************************************************
//  DSP Module block:  concatenation of DSPModule structures
// ****************************************************************

// Size of low-level data structure used by DSP Code
#define kDSPState_DMATransfer_Size	3	// DATA_SIZE_DMATRANSFER
#define kDSPState_Mixer_Size		3	// DATA_SIZE_MIXER
#define kDSPState_NullModule_Size	3	// DATA_SIZE_
#define kDSPState_Limiter_Size		3	// DATA_SIZE_LIMITER

#define kDSPState_AmpMod_Size		3	// DATA_SIZE_AMPMOD
#define kDSPState_Chorus_Size		3	// DATA_SIZE_CHORUS
#define kDSPState_Compressor_Size	3	// DATA_SIZE_COMPRESSOR
#define kDSPState_Delay_Size		3	// DATA_SIZE_DELAY
#define kDSPState_Distortion_Size	3	// DATA_SIZE_DISTORTION
#define kDSPState_Echo_Size			3	// DATA_SIZE_ECHO
#define kDSPState_Equalizer_Size	3	// DATA_SIZE_EQUALIZER
#define kDSPState_Flange_Size		3	// DATA_SIZE_FLANGE
#define kDSPState_IIR1_Size			3	// DATA_SIZE_IIR1
#define kDSPState_IIR2_Size			3	// DATA_SIZE_IIR2
#define kDSPState_IIIR_Size			3	// DATA_SIZE_IIIR
#define kDSPState_Oscillator_Size	3	// DATA_SIZE_OSCILLATOR
#define kDSPState_Reverb_Size		3	// DATA_SIZE_REVERB

// Low level data accessed by DSP
typedef struct { Int24	d[kDSPState_DMATransfer_Size]; } DSP_DMATRANSFER_STATE;
typedef struct { Int24	d[kDSPState_Mixer_Size      ]; } DSP_MIXER_STATE;
typedef struct { Int24	d[kDSPState_NullModule_Size ]; } DSP_NULLMODULE_STATE;
typedef struct { Int24	d[kDSPState_Limiter_Size    ]; } DSP_LIMITER_STATE;

typedef struct { Int24	d[kDSPState_AmpMod_Size     ]; } DSP_AMPMOD_STATE;
typedef struct { Int24	d[kDSPState_Chorus_Size     ]; } DSP_CHORUS_STATE;
typedef struct { Int24	d[kDSPState_Compressor_Size ]; } DSP_COMPRESSOR_STATE;
typedef struct { Int24	d[kDSPState_Delay_Size      ]; } DSP_DELAY_STATE;
typedef struct { Int24	d[kDSPState_Distortion_Size ]; } DSP_DISTORTION_STATE;
typedef struct { Int24	d[kDSPState_Echo_Size       ]; } DSP_ECHO_STATE;
typedef struct { Int24	d[kDSPState_Equalizer_Size  ]; } DSP_EQUALIZER_STATE;
typedef struct { Int24	d[kDSPState_Flange_Size     ]; } DSP_FLANGE_STATE;
typedef struct { Int24	d[kDSPState_IIR1_Size       ]; } DSP_IIR1_STATE;
typedef struct { Int24	d[kDSPState_IIR2_Size       ]; } DSP_IIR2_STATE;
typedef struct { Int24	d[kDSPState_IIIR_Size       ]; } DSP_IIIR_STATE;
typedef struct { Int24	d[kDSPState_Oscillator_Size ]; } DSP_OSCILLATOR_STATE;
typedef struct { Int24	d[kDSPState_Reverb_Size     ]; } DSP_REVERB_STATE;


// The DSP execution list is contructed of these modules
typedef struct dspmodule {
// Note:  Don't reorder these fields because DSP execution kernel
// expects the values in specific places.
	Int24	state;		// Low-level data, I/O ptrs are stored in state, In1Out1, In2/Out2, etc.
	Int24	compute;	// Ptr to compute routine in PRAM
	Int24   typeID;		// Algorithm/I/O type
}  DSPMODULE;

#define kDSPMODULE_Words	(sizeof(DSPMODULE)/sizeof(Int24))
void DefaultDSPModule  (DSPMODULE      *d);

// ****************************************************************
//  DSP Module state block:  concatenation of state structures,
//							excluding delay lines.  Reset command will
//							DMA this stuff into DSP XRAM and zero out
//							delay line memory.
//
//			(initial state given latest parameter set)
// ****************************************************************
#define kState_MaxSize	20		// in Int24 words

// ****************************************************************
// DSP Module code block:		Linked DSP Module Compute subroutines
//							and library routines
//
//		160 MHz/48000 = 3333 cycles
// ****************************************************************
#define kSysMem_DSPModuleCodeBlockMaxLength		3000

// MixBuffers 0x1400 through 0x17ff  Two banks of 32 Mixbuffers of 32 words each
//	2 banks x 32 Buffers/bank x 32 words/buffer = 2048 ($1000) words
#define kMixBufferSize			kFrameLength
#define kMixBufferLength		kFrameLength

#define kGPMemory_MixBuffers		0x001400	
#define kGPMemory_MixBuffer00		(kGPMemory_MixBuffers)	
#define kGPMemory_MixBuffer01		(kGPMemory_MixBuffer00+kMixBufferLength)	
#define kGPMemory_MixBuffer02		(kGPMemory_MixBuffer01+kMixBufferLength)	
#define kGPMemory_MixBuffer03		(kGPMemory_MixBuffer02+kMixBufferLength)	
#define kGPMemory_MixBuffer04		(kGPMemory_MixBuffer03+kMixBufferLength)	
#define kGPMemory_MixBuffer05		(kGPMemory_MixBuffer04+kMixBufferLength)	
#define kGPMemory_MixBuffer06		(kGPMemory_MixBuffer05+kMixBufferLength)	
#define kGPMemory_MixBuffer07		(kGPMemory_MixBuffer06+kMixBufferLength)	
#define kGPMemory_MixBuffer08		(kGPMemory_MixBuffer07+kMixBufferLength)	
#define kGPMemory_MixBuffer09		(kGPMemory_MixBuffer08+kMixBufferLength)	
#define kGPMemory_MixBuffer10		(kGPMemory_MixBuffer09+kMixBufferLength)	
#define kGPMemory_MixBuffer11		(kGPMemory_MixBuffer10+kMixBufferLength)	
#define kGPMemory_MixBuffer12		(kGPMemory_MixBuffer11+kMixBufferLength)	
#define kGPMemory_MixBuffer13		(kGPMemory_MixBuffer12+kMixBufferLength)	
#define kGPMemory_MixBuffer14		(kGPMemory_MixBuffer13+kMixBufferLength)	
#define kGPMemory_MixBuffer15		(kGPMemory_MixBuffer14+kMixBufferLength)
	
#define kGPMemory_MixBuffer16		(kGPMemory_MixBuffer15+kMixBufferLength)	
#define kGPMemory_MixBuffer17		(kGPMemory_MixBuffer16+kMixBufferLength)	
#define kGPMemory_MixBuffer18		(kGPMemory_MixBuffer17+kMixBufferLength)	
#define kGPMemory_MixBuffer19		(kGPMemory_MixBuffer18+kMixBufferLength)	
#define kGPMemory_MixBuffer20		(kGPMemory_MixBuffer19+kMixBufferLength)	
#define kGPMemory_MixBuffer21		(kGPMemory_MixBuffer20+kMixBufferLength)	
#define kGPMemory_MixBuffer22		(kGPMemory_MixBuffer21+kMixBufferLength)	
#define kGPMemory_MixBuffer23		(kGPMemory_MixBuffer22+kMixBufferLength)	
#define kGPMemory_MixBuffer24		(kGPMemory_MixBuffer23+kMixBufferLength)	
#define kGPMemory_MixBuffer25		(kGPMemory_MixBuffer24+kMixBufferLength)	
#define kGPMemory_MixBuffer26		(kGPMemory_MixBuffer25+kMixBufferLength)	
#define kGPMemory_MixBuffer27		(kGPMemory_MixBuffer26+kMixBufferLength)	
#define kGPMemory_MixBuffer28		(kGPMemory_MixBuffer27+kMixBufferLength)	
#define kGPMemory_MixBuffer29		(kGPMemory_MixBuffer28+kMixBufferLength)	
#define kGPMemory_MixBuffer30		(kGPMemory_MixBuffer29+kMixBufferLength)	
#define kGPMemory_MixBuffer31		(kGPMemory_MixBuffer30+kMixBufferLength)	

#define kGPMemory_TmpBuffers		0x0	
#define kGPMemory_TmpBuffer00		(kGPMemory_TmpBuffers)	
#define kGPMemory_TmpBuffer01		(kGPMemory_TmpBuffer00+kMixBufferLength)	
#define kGPMemory_TmpBuffer02		(kGPMemory_TmpBuffer01+kMixBufferLength)	
#define kGPMemory_TmpBuffer03		(kGPMemory_TmpBuffer02+kMixBufferLength)	

#define kGPMemory_TmpBuffer0		kGPMemory_TmpBuffer00
#define kGPMemory_TmpBuffer1		kGPMemory_TmpBuffer01
#define kGPMemory_TmpBuffer2		kGPMemory_TmpBuffer02
#define kGPMemory_TmpBuffer3		kGPMemory_TmpBuffer03

#endif  //	__DSP_UTIL_H__
