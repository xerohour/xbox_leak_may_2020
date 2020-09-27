// *************************************************************************
//
//	AudioFxDef.h		Support functions for DSP System memory operations
//
//	Written by Gints Klimanis, 2000
//
// *************************************************************************
//
#ifndef __AUDIOFXDEF_H__
#define	__AUDIOFXDEF_H__

// ID's for the DSP Module types 
#define kAudioFxModuleTypeID_Invalid	(-1)
#define kAudioFxModuleTypeID_Null		0
#define kAudioFxModuleTypeID_DMA       	1

#define kAudioFxModuleTypeID_AmpMod		10
#define kAudioFxModuleTypeID_Chorus		11
#define kAudioFxModuleTypeID_Compressor	12
#define kAudioFxModuleTypeID_Delay		13
#define kAudioFxModuleTypeID_Distortion	14
#define kAudioFxModuleTypeID_Echo		15
#define kAudioFxModuleTypeID_Equalizer	16
#define kAudioFxModuleTypeID_Flange 	17
#define kAudioFxModuleTypeID_IIIR 		18
#define kAudioFxModuleTypeID_IIR1		19
#define kAudioFxModuleTypeID_IIR2		20
#define kAudioFxModuleTypeID_Limiter	21
#define kAudioFxModuleTypeID_Mixer		22
#define kAudioFxModuleTypeID_Oscillator	23
#define kAudioFxModuleTypeID_Reverb		24
#define kAudioFxModule_MaxTypes			(1+kAudioFxModuleTypeID_Reverb)

#define kAudioFx_MaxModules		64
#define kAudioFxModule_EmptyID		(-1)	

// Name strings for the DSP Module types 
#define kAudioFxModuleTypeName_Invalid "Invalid"
#define kAudioFxModuleTypeName_DMA  "DMA"
#define kAudioFxModuleTypeName_Null "Null"

#define kAudioFxModuleTypeName_AmpMod     "AmpMod"
#define kAudioFxModuleTypeName_Chorus     "Chorus"
#define kAudioFxModuleTypeName_Compressor "Compressor"
#define kAudioFxModuleTypeName_Delay      "Delay"
#define kAudioFxModuleTypeName_Distortion "Distortion"
#define kAudioFxModuleTypeName_Echo       "Echo"
#define kAudioFxModuleTypeName_Equalizer  "Equalizer"
#define kAudioFxModuleTypeName_Flange     "Flange"
#define kAudioFxModuleTypeName_IIIR       "IIIR"
#define kAudioFxModuleTypeName_IIR1       "IIR1"
#define kAudioFxModuleTypeName_IIR2       "IIR2"
#define kAudioFxModuleTypeName_Limiter    "Limiter"
#define kAudioFxModuleTypeName_Mixer      "Mixer"
#define kAudioFxModuleTypeName_Oscillator "Oscillator"
#define kAudioFxModuleTypeName_Reverb     "Reverb"


// Buffer IDs used to connect effects modules
#define kAudioFX_BufferID_2DFxInputLeft		100		// left  channel from two channel FX input
#define kAudioFX_BufferID_2DFxInputRight	101		// right channel from two channel FX input
#define kAudioFX_BufferID_2DFxOutputLeft	102		// to left  front channel output
#define kAudioFX_BufferID_2DFxOutputRight	103		// to right front channel output

#define kAudioFX_BufferID_3DFxInputLeft		110		// left  channel from two channel I3DL2 input
#define kAudioFX_BufferID_3DFxInputRight	111		// right channel from two channel I3DL2 input
#define kAudioFX_BufferID_3DFxOutputLeft	112		// two front channels to front X-talk box
#define kAudioFX_BufferID_3DFxOutputRight	113		// two rear channels  to rear  X-talk box

#define kAudioFX_BufferID_Null		150				// Used to indicate an idle connection

#define kAudioFX_BufferID_Temp00	200		
#define kAudioFX_BufferID_Temp01	201	
#define kAudioFX_BufferID_Temp02	202	
#define kAudioFX_BufferID_Temp03	203	
#define kAudioFX_BufferID_Temp04	204	
#define kAudioFX_BufferID_Temp05	205	
#define kAudioFX_BufferID_Temp06	206	
#define kAudioFX_BufferID_Temp07	207	

#define kAudioFX_BufferID_FinalMix_Input	300		// Future use:   connect to all pins
#define kAudioFX_BufferID_FinalMix_Pin0		301
#define kAudioFX_BufferID_FinalMix_Pin1		302
#define kAudioFX_BufferID_FinalMix_Pin2		303
#define kAudioFX_BufferID_FinalMix_Pin3		304
#define kAudioFX_BufferID_FinalMix_Pin4		305
#define kAudioFX_BufferID_FinalMix_Pin5		306

// DO NOT change the values!!  it'll break NV_AUDGP_ROUTE_TO_FIFO - (CGpMethod.cpp)
#define kAudioFX_BufferID_FIFO0_Input		300		// Future use:   connect to all pins
#define kAudioFX_BufferID_FIFO0_Input_Pin0	301
#define kAudioFX_BufferID_FIFO0_Input_Pin1	302
#define kAudioFX_BufferID_FIFO0_Input_Pin2	303
#define kAudioFX_BufferID_FIFO0_Input_Pin3	304
#define kAudioFX_BufferID_FIFO0_Input_Pin4	305
#define kAudioFX_BufferID_FIFO0_Input_Pin5	306

#define kAudioFX_BufferID_FIFO1_Input		310	// Future use:   connect to all pins
#define kAudioFX_BufferID_FIFO1_Input_Pin0	311
#define kAudioFX_BufferID_FIFO1_Input_Pin1	312
#define kAudioFX_BufferID_FIFO1_Input_Pin2	313
#define kAudioFX_BufferID_FIFO1_Input_Pin3	314
#define kAudioFX_BufferID_FIFO1_Input_Pin4	315
#define kAudioFX_BufferID_FIFO1_Input_Pin5	316

#define kAudioFX_BufferID_FIFO0_Output		320	// Future use:   connect to all pins
#define kAudioFX_BufferID_FIFO0_Output_Pin0	321
#define kAudioFX_BufferID_FIFO0_Output_Pin1	322
#define kAudioFX_BufferID_FIFO0_Output_Pin2	323
#define kAudioFX_BufferID_FIFO0_Output_Pin3	324
#define kAudioFX_BufferID_FIFO0_Output_Pin4	325
#define kAudioFX_BufferID_FIFO0_Output_Pin5	326

#define kAudioFX_BufferID_FIFO1_Output		330	// Future use:   connect to all pins
#define kAudioFX_BufferID_FIFO1_Output_Pin0	331
#define kAudioFX_BufferID_FIFO1_Output_Pin1	332
#define kAudioFX_BufferID_FIFO1_Output_Pin2	333
#define kAudioFX_BufferID_FIFO1_Output_Pin3	334
#define kAudioFX_BufferID_FIFO1_Output_Pin4	335
#define kAudioFX_BufferID_FIFO1_Output_Pin5	336

#define kAudioFX_BufferID_FIFO2_Output		340	// Future use:   connect to all pins
#define kAudioFX_BufferID_FIFO2_Output_Pin0	341
#define kAudioFX_BufferID_FIFO2_Output_Pin1	342
#define kAudioFX_BufferID_FIFO2_Output_Pin2	343
#define kAudioFX_BufferID_FIFO2_Output_Pin3	344
#define kAudioFX_BufferID_FIFO2_Output_Pin4	345
#define kAudioFX_BufferID_FIFO2_Output_Pin5	346

#define kAudioFX_BufferID_FIFO3_Output		350	// Future use:   connect to all pins
#define kAudioFX_BufferID_FIFO3_Output_Pin0	351
#define kAudioFX_BufferID_FIFO3_Output_Pin1	352
#define kAudioFX_BufferID_FIFO3_Output_Pin2	353
#define kAudioFX_BufferID_FIFO3_Output_Pin3	354
#define kAudioFX_BufferID_FIFO3_Output_Pin4	355
#define kAudioFX_BufferID_FIFO3_Output_Pin5	356

#define kAudioFX_BufferID_2DLoopBackTo3D_Input0		120		// FIXXX: not done:Will probably be dynamic.
//#define kAudioFX_BufferID_2DLoopBackTo3D_Output0	121		

#define kAudioFX_MaxInputs	6
#define kAudioFX_MaxOutputs	6

#define kAudioFX_Pin0	0
#define kAudioFX_Pin1	1
#define kAudioFX_Pin2	2
#define kAudioFX_Pin3	3
#define kAudioFX_Pin4	4
#define kAudioFX_Pin5	5

#define kAudioFX_PinLeft	kAudioFX_Pin0
#define kAudioFX_PinRight	kAudioFX_Pin1

// ---- FIFO support
// These definitions are from FIFO Control Register definitions
// WARNING!!!  do not change these definitions!  match them with NV_AUDGP_CONFIGURE_OUTPUT_FIFO
#define kFIFO_ChannelFormat_Mono	0
#define kFIFO_ChannelFormat_Stereo	1
#define kFIFO_ChannelFormat_Quad	2
#define kFIFO_ChannelFormat_Hex		3

#define kFIFO_ClassDataSize_08_08		0	// 08-bit data in 8-bit container(always in blocks of 4)
#define kFIFO_ClassDataSize_16_16		1	// 16-bit data in 16-bit container (always in blocks of two)
#define kFIFO_ClassDataSize_24_32_MSB	2	// 24 bit data in 32-bit container (msb justified)
#define kFIFO_ClassDataSize_24_32_LSB	3	// 24-bit data in 32-bit container (lsb justified)
#define kFIFO_ClassDataSize_32_32		4	// 32 bit data in 32-bit container

typedef struct fifo_info {
	int channelFormat;	
	int classDataSize;  
}  FIFO_INFO;

// ****************************************************************
//  AudioFX PlugIn:  allows developers to add audio DSP modules
// ****************************************************************
typedef long (*AUDIOFX_PLUGIN_FUNCTION)(AFX_PARAMETERS *);

typedef struct audiofx_plugin_functionentry {
	long					id;
	AUDIOFX_PLUGIN_FUNCTION	function;
}  AUDIOFX_PLUGIN_FUNCTIONENTRY;

// All functions take two variables: FX module struct, FX module state 
#define kAudioFX_PlugIn_Function_BaseCount		5 // This constant will disappear
#define kAudioFX_PlugIn_FunctionID_Null			-1
#define kAudioFX_PlugIn_FunctionID_Default		0
#define kAudioFX_PlugIn_FunctionID_Prepare		1
#define kAudioFX_PlugIn_FunctionID_Reset		2
#define kAudioFX_PlugIn_FunctionID_Set			3	// FIXXX: Uh oh.  This breaks function typing
#define kAudioFX_PlugIn_FunctionID_Update		4

//#define kAudioFX_PlugIn_FunctionReturnValue		0	// ok
//#define kAudioFX_PlugIn_FunctionReturnValue		(-1)	// ok
//#define kAudioFX_PlugIn_FunctionReturnValue		(-2)	// ok

// Note:  if you change order of structure fields, 
//			adjust StandardPlugins[] in DSP_Binaries.cpp
typedef struct audiofx_plugin {
	int 	maxInputs;
	int 	maxOutputs;	
	long	typeID;		// ID from registered set of AudioFX module types

	long	moduleStructSize;	// High level Parameters, including Direct X parameter set
	Int24	smallStateSize;		// Low level info used by DSP
	Int24	*dspComputeRoutine;
	long	dspComputeRoutineSize;
	char	*name;

	AUDIOFX_PLUGIN_FUNCTIONENTRY functionIDList[kAudioFX_PlugIn_Function_BaseCount];	// FIXXX: allocate !
//	long	functionCount;
}  AUDIOFX_PLUGIN;

// ****************************************************************
//  AudioFX Module:  information used to construct DSP Module
// ****************************************************************
typedef struct audiofx_module {
	long	inBufferIDs [kAudioFX_MaxInputs ];
	long	outBufferIDs[kAudioFX_MaxOutputs];	
	long    ioCount;		// # Input and Output pairs.

//	char	 parameters[kAudioFX_MaxParameterSize ];	// Host form (high level)	
//	Int24	 smallState[kAudioFX_MaxSmallStateSize];	// DSP form
	AFX_PARAMETERS data;	// FIXXX: later on allocate parameters[] and							//			smallState[] 

	long	 moduleID;		// Unique identifier
	AUDIOFX_PLUGIN *parent;	// Ptr to parent plug-in definiction
}  AUDIOFX_MODULE;


// ****************************************************************
//  AudioFX:  the master system that manages audio fx modules and
//				assembles the memory blocks downloaded to the DSP
// ****************************************************************
// The first three memory block sizes are known before CreateAudioFX()
// The large state block size is calculated from parameter sets
#define kAudioFX_Memory_DspCodeBlock		0
#define kAudioFX_Memory_DspModuleBlock		1
#define kAudioFX_Memory_DspStateBlock		2
#define kAudioFX_Memory_DspLargeStateBlock	3		

#define kAudioFX_AllModules	(-1)





#endif  //	__AUDIOFXDEF_H__
