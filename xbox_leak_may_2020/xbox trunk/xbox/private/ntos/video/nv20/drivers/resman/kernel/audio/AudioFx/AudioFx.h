// *************************************************************************
//
//	AudioFx.h		Support functions for DSP System memory operations
//
//	Written by Gints Klimanis, 2000
//
// *************************************************************************
//
#ifndef __AUDIOFX_H__
#define	__AUDIOFX_H__

#include "DSP_Util.h"
#include "AudioFxDef.h"

#ifndef AUDIOFX_STANDALONE
#include "AudioHw.h"
#endif

// ==============================================================
// Global Processor DSP Address Map 
// ==============================================================
// Relative to GPBase (24 bit words (3 bytes) in 32-bit containers (4 bytes))
//     0x0000 - 0x1FFC   GP DSP X-Memory 4K Words (24 lsbits of Dword)     
//     0x4000 - 0x5FFC   GP DSP Y-Memory 2K Words (24 lsbits of Dword) 
//     0x8000 - 0x8FFC   GP DSP P-Memory 4K Words (24 lsbits of Dword) 
//     0xFE00 - 0xFFF0   GP DSP Peripheral Registers
//     0xFFFC            GP DSP Reset
//
// Peripheral registers (Control,Status) mapped to upper 128 words
// of X-memory (0xFFFF80 to 0xFFFFFF) so they can be accessed with movep 
//
// GP DSP memory addresses 24-bit words
// System memory address    8-bit words 

#define kGPMemory_Base		 0x000000
#define kGPMemory_Base_XRAM	(kGPMemory_Base+0x000000)
#define kGPMemory_Base_YRAM	(kGPMemory_Base+0x001800)
#define kGPMemory_Base_PRAM	(kGPMemory_Base+0x002000)

#define kGPMemory_Size_XRAM	3*1024
#define kGPMemory_Size_YRAM	2*1024
#define kGPMemory_Size_PRAM	4*1024

// MixBuffers 0x1400 through 0x17ff  Two banks of 32 Mixbuffers of 32 words each
//	2 banks x 32 Buffers/bank x 32 words/buffer = 2048 ($1000) words
#define kMixBufferSize		kFrameLength
#define kMixBufferLength	kFrameLength

#define kGPMemory_Base_MixBuffers	0x001400	
#define kGPMemory_Base_TempBuffers	0x0	

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

#define kAudioFX_Pin0	0
#define kAudioFX_Pin1	1
#define kAudioFX_Pin2	2
#define kAudioFX_Pin3	3
#define kAudioFX_Pin4	4
#define kAudioFX_Pin5	5

#define kAudioFX_PinLeft	kAudioFX_Pin0
#define kAudioFX_PinRight	kAudioFX_Pin1

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

class CScratchDma;

class AUDIOFX {
public:
	AUDIOFX(long maxModules, CScratchDma *pDma);		
	~AUDIOFX(); 

// NV_AUDGP_ENABLE_EFFECT
//	To to disable an effect without deleting it:
//	1) Exclude from execution list
//	2) Disconnect inputs and outputs
	int	SetModuleExecutionList(long *moduleIDs, int count);

// ---- Module Functions
// When moduleID=kAudioFX_AllModules, apply to all modules
    // NV_AUDGP_ADD_EFFECT
    // RegisterModuleType() will return a unique typeID
	int AddModule	  (long  typeID);
	int AddModule	  (char *typeName);

    // NV_AUDGP_DELETE_EFFECT
	int DeleteModule (long moduleID);
	
// NV_AUDGP_SET_EFFECT_PARAMETERS
// The module will know the size of parameterData
// Will delay memory be allocated here?
	int SetModule    (long moduleID, void *parameterData);
	int GetModule    (long moduleID, void *parameterData);
	void *GetModuleParameterPtr(long moduleID);
	AFX_PARAMETERS *GetModuleAFXParametersPtr(long moduleID);
    /*
    NV_AUDGP_ROUTE_EFFECT
    shouldn't there be a pin # - like an outindex from the effect?
		Developers will know an effect's input and output count beforehand.
    buffer ID?  we need to make the RM do the resource management.. also is there a diff
    between the SW buffers, and HW buffers?
GK>> What kind of difference?  Right now, there isn't a difference.  
    */
//	int	SetModule_IOBuffers(long moduleID, int  inIndex, int  inBufferID, int outIndex, int outBufferID);
	int	SetModule_InBuffer (long moduleID, int  inIndex, int  inBufferID);
	int	SetModule_OutBuffer(long moduleID, int outIndex, int outBufferID);

	int SetInputFIFOFormat (int index, int channelFormat, int classDataSize);
	int SetOutputFIFOFormat(int index, int channelFormat, int classDataSize);

    // NV_AUDGP_REGISTER_EFFECT
    //the AUDIOFX_PLUGIN structure has to be shared between the driver and the RM
	int RegisterModuleType  (AUDIOFX_PLUGIN *data);

    // NV_AUDGP_DEREGISTER_EFFECT  
	int UnregisterModuleType(long typeID);

    /* NV_AUDGP_QUERY_EFFECT define query ID for the driver.*/
	int Command		 (long moduleID, int commandID, void *data);
	int QueryModule  (long moduleID, int queryID);

// Debug for now
	int  Update();
	void SetTopologyName(char *s);

	int DefaultModule(long moduleID);
	int ResetModule  (long moduleID);
	int UpdateModule (long moduleID);
	int PrepareModule(long moduleID);

private:

	char *audioFxModuleTypeNames[kAudioFxModule_MaxTypes];
	DSPCOMMANDNODE dspCommandNode;

// System Memory block pointers (address offsets from GPSADDR)
//	Int24 *dspCommandNode;
	Int24 *dspCodeBlock;		// DSP PRAM Code block
	Int24 *dspModuleBlock;		// DSP XRAM Execution List
	Int24 *dspStateBlock;		// DSP X/YRAM, includes command node
//	Int24 dspLargeStateBlock;   // System Memory state: delay lines, etc.

// Scratch memory address offsets
	Int24 smDspCommandNode;
	Int24 smDspCodeBlock;		
	Int24 smDspModuleBlock;		
	Int24 smDspStateBlock;		

// Allocated size, in Int24 words
//	long dspCommandNodeSize;
	long dspCodeBlockSize;		
	long dspModuleSize;
	long dspStateBlockSize;
//	long dspLargeStateBlockSize;

#define kAudioFX_MaxInputFIFOs	2
#define kAudioFX_MaxOutputFIFOs	4
	FIFO_INFO	inputFIFOs [kAudioFX_MaxInputFIFOs ];
	FIFO_INFO	outputFIFOs[kAudioFX_MaxOutputFIFOs];

// FX registry 
	AUDIOFX_PLUGIN plugIns[kAudioFxModule_MaxTypes];	// [0.. kAudioFxModule_MaxTypes]
	long	plugInCount;
	long	typeIDCounter;	// Incremented every time a new module is added

// Module lists
	AUDIOFX_MODULE	modules[kAudioFx_MaxModules];
	long	moduleCount;
	long	maxModules;

	long	moduleIDCounter;	// Incremented every time a new module is added
	long	executionList[kAudioFx_MaxModules];	// Array of module IDs
	long	executionListLength;
	char	*topologyName;

// DSP code & state data info
	Int24 *dspComputeRoutines    [kAudioFxModule_MaxTypes];	
	long   dspComputeRoutineSizes[kAudioFxModule_MaxTypes];	// in Int24 words
	Int24 *dspCommonRoutines;
	long   dspCommonRoutinesSize;

	void InitModule(AUDIOFX_MODULE *d);
	void InitPlugIn(AUDIOFX_PLUGIN *d);

	int  AssembleDSPCodeBlock  (char *filePath);
	int  AssembleDSPStateBlock (char *filePath);
	void TranslateModule_IOBufferIDs(AUDIOFX_MODULE *d);
	Int24 BufferIDToDSPAddress(Int24 id);

	AUDIOFX_MODULE *FindAudioFXModule(long moduleID);
	AUDIOFX_PLUGIN *FindAudioFXPlugInByID  (long typeID);
	AUDIOFX_PLUGIN *FindAudioFXPlugInByName(char *name );

	long WriteWordsToFile(char *filePath, Int24 *data, long count);
	char *FxTypeIDToName (long id);
	long  FxParameterSize(long typeID);
} ;



#endif  //	__AUDIOFX_H__
