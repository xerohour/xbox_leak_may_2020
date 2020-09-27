// *************************************************************************
//
//	AudioFx:		DSP Execution Support routines
//
//	Written by Gints Klimanis
//
// *************************************************************************
#include "util.h"
#include <string.h>

#include "DSP_Util.h"
#include "AudioFx.h"

#include "DSP_Binaries.h"

// Built-in FX modules
#include "NullModule_Util.h"

// The First 32 buffers reference the MixBuffers
#define kAudioFX_BufferID_MixBuf00	 0	
#define kAudioFX_BufferID_MixBuf01	 1	
#define kAudioFX_BufferID_MixBuf02	 2	
#define kAudioFX_BufferID_MixBuf03	 3	
#define kAudioFX_BufferID_MixBuf04	 4	
#define kAudioFX_BufferID_MixBuf05	 5	
#define kAudioFX_BufferID_MixBuf06	 6	
#define kAudioFX_BufferID_MixBuf07	 7	
#define kAudioFX_BufferID_MixBuf08	 8	
#define kAudioFX_BufferID_MixBuf09	 9	
#define kAudioFX_BufferID_MixBuf10	10	
#define kAudioFX_BufferID_MixBuf11	11	
#define kAudioFX_BufferID_MixBuf12	12	
#define kAudioFX_BufferID_MixBuf13	13	
#define kAudioFX_BufferID_MixBuf14	14	
#define kAudioFX_BufferID_MixBuf15	15	

#define kAudioFX_BufferID_MixBuf16	16	
#define kAudioFX_BufferID_MixBuf17	17	
#define kAudioFX_BufferID_MixBuf18	18	
#define kAudioFX_BufferID_MixBuf19	19	
#define kAudioFX_BufferID_MixBuf20	20	
#define kAudioFX_BufferID_MixBuf21	21	
#define kAudioFX_BufferID_MixBuf22	22	
#define kAudioFX_BufferID_MixBuf23	23	
#define kAudioFX_BufferID_MixBuf24	24	
#define kAudioFX_BufferID_MixBuf25	25	
#define kAudioFX_BufferID_MixBuf26	26	
#define kAudioFX_BufferID_MixBuf27	27	
#define kAudioFX_BufferID_MixBuf28	28	
#define kAudioFX_BufferID_MixBuf29	29	
#define kAudioFX_BufferID_MixBuf30	30	
#define kAudioFX_BufferID_MixBuf31	31	

// GK FIXXX: check-in rush kludge.  Ideally, I would like these
// to be #define,
//char *audioFxModuleTypeName_DMA  = kAudioFxModuleTypeName_DMA;
//char *audioFxModuleTypeName_Null = kAudioFxModuleTypeName_Null;

//char *audioFxModuleTypeName_AmpMod     = kAudioFxModuleTypeName_AmpMod;
//char *audioFxModuleTypeName_Chorus     = kAudioFxModuleTypeName_Chorus;
//char *audioFxModuleTypeName_Compressor = kAudioFxModuleTypeName_Compressor;
//char *audioFxModuleTypeName_Delay      = kAudioFxModuleTypeName_Delay;
//char *audioFxModuleTypeName_Distortion = kAudioFxModuleTypeName_Distortion;
//char *audioFxModuleTypeName_Echo       = kAudioFxModuleTypeName_Echo;
//char *audioFxModuleTypeName_Equalizer  = kAudioFxModuleTypeName_Equalizer;
//char *audioFxModuleTypeName_Flange     = kAudioFxModuleTypeName_Flange;
//char *audioFxModuleTypeName_IIIR       = kAudioFxModuleTypeName_IIIR;
//char *audioFxModuleTypeName_IIR1       = kAudioFxModuleTypeName_IIR1;
//char *audioFxModuleTypeName_IIR2       = kAudioFxModuleTypeName_IIR2;
//char *audioFxModuleTypeName_Limiter    = kAudioFxModuleTypeName_Limiter;
//char *audioFxModuleTypeName_Mixer      = kAudioFxModuleTypeName_Mixer;
//char *audioFxModuleTypeName_Oscillator = kAudioFxModuleTypeName_Oscillator;
//char *audioFxModuleTypeName_Reverb     = kAudioFxModuleTypeName_Reverb;

// **********************************************************************************
// AUDIOFX:		Set up audio effects objects except the data-dependent
//						delay line stuff.  This structure will house all of
//						the ptrs to the Command Nodes, DSP Execution Lists, 
//						DSP Code Blocks, DSP Small State Blocks, DSP Large State 
//						blocks in system memory.  
//						
//		Return ptr to AUDIOFX structure
// ********************************************************************************** 
AUDIOFX::AUDIOFX(long uMaxModules, CScratchDma *pDma)
{
int i;
  
#ifndef AUDIOFX_STANDALONE  
afxScratchMemInitialize(pDma);
#endif

// Scratch Memory block pointers
dspCodeBlock   = (Int24 *) kAudioFX_SMAddress_CodeBlock;		// DSP PRAM Code block
dspModuleBlock = (Int24 *) kAudioFX_SMAddress_ModuleBlock;	// DSP XRAM Execution List
dspStateBlock  = (Int24 *) kAudioFX_SMAddress_StateBlock;		// DSP X/YRAM
//dspLargeStateBlock = NULL;  // System Memory state: delay lines, etc.

dspCodeBlockSize   = 0;		
dspModuleSize      = 0;
dspStateBlockSize  = 0;
//dspLargeStateBlockSize = 0;

topologyName = NULL;

// Setup individual modules
#ifndef AUDIOFX_STANDALONE  
if (uMaxModules > kAudioFx_MaxModules)
    {
    aosDbgPrintString(DEBUGLEVEL_ERRORS, "AUDIOFX::AUDIOFX - so many modules not supported!!\n");
    aosDbgBreakPoint();
    }
#endif
maxModules = uMaxModules;
for (i = 0; i < kAudioFxModule_MaxTypes; i++)
	{
	InitPlugIn(&plugIns[i]);

//	dspComputeRoutines    [i] = NullCompute_In1Out1;
//	dspComputeRoutineSizes[i] = NullCompute_In1Out1Length;
	}
dspModuleBlock = (Int24 *) afxSystemMemAlloc(maxModules*sizeof(DSPMODULE));

// Add Plug-In routines

// Other stuff
moduleIDCounter     = 0;
typeIDCounter       = kAudioFxModuleTypeID_Null+1;
moduleCount         = 0;
executionListLength = 0;

AUDIOFX_MODULE *p = modules;
for (i = 0; i < maxModules; i++)
	{
	p[i].moduleID = kAudioFxModule_EmptyID;
	executionList[i] = kAudioFxModuleTypeID_Null;
	}

for (i = 0; i < kAudioFX_MaxInputFIFOs; i++)
	{
	inputFIFOs[i].channelFormat = kFIFO_ChannelFormat_Stereo;
	inputFIFOs[i].classDataSize = kFIFO_ClassDataSize_16_16;
	}
for (i = 0; i < kAudioFX_MaxOutputFIFOs; i++)
	{
	outputFIFOs[i].channelFormat = kFIFO_ChannelFormat_Stereo;
	outputFIFOs[i].classDataSize = kFIFO_ClassDataSize_16_16;
	}

// FIXXX:  record to avoid this sort of string table
for (i = 0; i < kAudioFxModule_MaxTypes; i++)
	audioFxModuleTypeNames[i] = NULL;

audioFxModuleTypeNames[kAudioFxModuleTypeID_DMA       ] = kAudioFxModuleTypeName_DMA;
audioFxModuleTypeNames[kAudioFxModuleTypeID_Mixer     ] = kAudioFxModuleTypeName_Mixer;
audioFxModuleTypeNames[kAudioFxModuleTypeID_Null      ] = kAudioFxModuleTypeName_Null;

audioFxModuleTypeNames[kAudioFxModuleTypeID_AmpMod    ] = kAudioFxModuleTypeName_AmpMod;
audioFxModuleTypeNames[kAudioFxModuleTypeID_Chorus    ] = kAudioFxModuleTypeName_Chorus;
audioFxModuleTypeNames[kAudioFxModuleTypeID_Compressor] = kAudioFxModuleTypeName_Compressor;
audioFxModuleTypeNames[kAudioFxModuleTypeID_Delay     ] = kAudioFxModuleTypeName_Delay;
audioFxModuleTypeNames[kAudioFxModuleTypeID_Distortion] = kAudioFxModuleTypeName_Distortion;
audioFxModuleTypeNames[kAudioFxModuleTypeID_Echo      ] = kAudioFxModuleTypeName_Echo;
audioFxModuleTypeNames[kAudioFxModuleTypeID_Equalizer ] = kAudioFxModuleTypeName_Equalizer;
audioFxModuleTypeNames[kAudioFxModuleTypeID_Flange    ] = kAudioFxModuleTypeName_Flange;
audioFxModuleTypeNames[kAudioFxModuleTypeID_IIIR      ] = kAudioFxModuleTypeName_IIIR;
audioFxModuleTypeNames[kAudioFxModuleTypeID_IIR1      ] = kAudioFxModuleTypeName_IIR1;
audioFxModuleTypeNames[kAudioFxModuleTypeID_IIR2      ] = kAudioFxModuleTypeName_IIR2;
audioFxModuleTypeNames[kAudioFxModuleTypeID_Limiter   ] = kAudioFxModuleTypeName_Limiter;
audioFxModuleTypeNames[kAudioFxModuleTypeID_Mixer     ] = kAudioFxModuleTypeName_Mixer;
audioFxModuleTypeNames[kAudioFxModuleTypeID_Oscillator] = kAudioFxModuleTypeName_Oscillator;
audioFxModuleTypeNames[kAudioFxModuleTypeID_Reverb    ] = kAudioFxModuleTypeName_Reverb;

}	// ---- end AUDIOFX() ---- 

// ******************************************************************
// ~AUDIOFX:		Deallocate internal Audio FX data blocks
//	
//					Return Boolean success
// ****************************************************************** 
AUDIOFX::~AUDIOFX()
{
int	i;

// Deallocate modules
for (i = 0; i < kAudioFx_MaxModules; i++)
	DeleteModule(modules[i].moduleID);
    
#ifndef AUDIOFX_STANDALONE  
    afxScratchMemShutdown();
#endif
}	// ---- end ~AUDIOFX() ---- 

#ifdef SAFE
// ****************************************************************
// ConnectAudioFX:	Connect one module output to input of another module
//
//		Note:  Modules may be connected to themselves, but there will
//				be one frame of latency in the recirculation path.
//				To Disconnect a module, connect both outputs or
//				both inputs to the NULL module.  
//
//						Return Boolean success
// **************************************************************** 
	int	
AUDIOFX::ConnectModules(long sourceID, int outIndex, long destID, int inIndex)
{
AUDIOFX_MODULE *inModule, *outModule;

inModule  = FindAudioFXModule(sourceID);
outModule = FindAudioFXModule(destID  );

if (!inModule || !outModule)
	return (False);

if (kAudioFxModule_DMA == inModule->typeID || kAudioFxModule_DMA == outModule->typeID)
	return (False);

outModule->outBufferIDs[outIndex] = inModule->inBufferIDs[ inIndex];

return (True);
}	// ---- end ConnectAudioFX() ---- 
#endif

#ifdef SAFE
// ****************************************************************
// SetModule_IOBuffers:	Specify buffer IDs for module inputs and outputs
//
//		Note:  If a module has no inputs or outputs, the bufferIDS for
//				for those connections will be ignored.
//
//						Return Boolean success
// **************************************************************** 
	int	
AUDIOFX::SetModule_IOBuffers(long moduleID, int  inIndex, int  inBufferIFD, 
								int outIndex, int outBufferID)
{
AUDIOFX_MODULE *m = FindAudioFXModule(moduleID);

if (!m || kAudioFxModule_DMA == m->typeID)
	return (False);

m->inBufferIDs [ inIndex] =  inBufferID;
m->outBufferIDs[outIndex] = outBufferID;

return (True);
}	// ---- end SetModule_IOBuffers() ---- 
#endif

// ****************************************************************
// SetModule_InBuffer:	Specify buffer IDs for module inputs, only
//
//		Note:  If a module has no inputs, the bufferIDS for
//				for those connections will be ignored.
//
//						Return Boolean success
// **************************************************************** 
	int	
AUDIOFX::SetModule_InBuffer(long moduleID, int index, int bufferID)
{
AUDIOFX_MODULE *m = FindAudioFXModule(moduleID);

if (!m ) //|| kAudioFxModule_DMA == m->typeID)
	return (False);

m->inBufferIDs[index] = bufferID;

return (True);
}	// ---- end SetModule_InBuffer() ---- 

// ****************************************************************
// SetModule_OutBuffer:	Specify buffer IDs for module outputs, only
//
//		Note:  If a module has no outputs, the bufferIDS for
//				for those connections will be ignored.
//
//						Return Boolean success
// **************************************************************** 
	int	
AUDIOFX::SetModule_OutBuffer(long moduleID, int index, int bufferID)
{
AUDIOFX_MODULE *m = FindAudioFXModule(moduleID);

if (!m ) //|| kAudioFxModule_DMA == m->typeID)
	return (False);

m->outBufferIDs[index] = bufferID;

return (True);
}	// ---- end SetModule_OutBuffer() ---- 

// ****************************************************************
// TranslateModule_IOBufferIDs:	Convert input and output buffers IDs
//								to DSP addresses.
// **************************************************************** 
	void	
AUDIOFX::TranslateModule_IOBufferIDs(AUDIOFX_MODULE *d)
{
Int24 *ioAddresses = (Int24 *) d->data.smallState;

// Format {all inputs, all outputs}
for (int i = 0; i < d->ioCount; i++)
	{
	ioAddresses[i             ] = BufferIDToDSPAddress(d->inBufferIDs [i]);
	ioAddresses[i+d->ioCount/2] = BufferIDToDSPAddress(d->outBufferIDs[i]);
	}
}	// ---- end TranslateModule_IOBufferIDs() ----

// ****************************************************************
// BufferIDToDSPAddress:	Given buffers ID, return DSP addresses.
//		
//			In case of error, return (-1)
// **************************************************************** 
	Int24	
AUDIOFX::BufferIDToDSPAddress(Int24 id)
{
Int24 x = -1;

//  Deal with FX sources
// FIXXX: for now, using mix bins
switch (id)
	{
	case kAudioFX_BufferID_2DFxInputLeft:
		id = kGPMemory_MixBuffer00;
	break;
	case kAudioFX_BufferID_2DFxInputRight:
		id = kGPMemory_MixBuffer01;
	break;
	case kAudioFX_BufferID_2DFxOutputLeft:
		id = kGPMemory_MixBuffer00;
	break;
	case kAudioFX_BufferID_2DFxOutputRight:
		id = kGPMemory_MixBuffer01;
	break;

	case kAudioFX_BufferID_3DFxInputLeft:
		id = kGPMemory_MixBuffer02;
	break;
	case kAudioFX_BufferID_3DFxInputRight:
		id = kGPMemory_MixBuffer03;
	break;
	case kAudioFX_BufferID_3DFxOutputLeft:
	break;
	case kAudioFX_BufferID_3DFxOutputRight:
	break;

// FIXXXX: Only one global 2D->3D fx loopback 
	case kAudioFX_BufferID_2DLoopBackTo3D_Input0:
		id = kGPMemory_MixBuffer31;
	break;
	}

if (-1 != id)
	return (id);

// Next, temporary buffers
if (id >= kAudioFX_BufferID_Temp00 && id <= kAudioFX_BufferID_Temp07)
	x = kGPMemory_Base_TempBuffers + kFrameLength*(id-kAudioFX_BufferID_Temp00);

return (-1);
}	// ---- end BufferIDToDSPAddress() ---- 

// ****************************************************************
// SetModuleExecutionList:	Specify order of module execution.  If this
//						function is not called, the effects will be
//						executed in the order of their module IDs.
//
//			Return True if all modules were scheduled without error.
//
//						Return Boolean success (FIXXX: perhaps # of modules scheduled)
// **************************************************************** 
	int	
AUDIOFX::SetModuleExecutionList(long *moduleIDs, int count)
// moduleIDs		ptr to modules to be included in execution list
//					NULL value will construct an execution list of
//					all modules in increasing moduleID order.
{
if (!moduleIDs)
	{
// FIXXX: just add code.
	}

if (count > kAudioFx_MaxModules)
	return (False);

afxSystemMemCopy(executionList, moduleIDs, count*sizeof(long));
executionListLength = count;

return (True);
}	// ---- end SetModuleExecutionList() ---- 

// ****************************************************************
// AddModule:		Set up a new audio effect.  
//				Note: this function does not add this module to
//				the execution list.
//
//			Return module ID.  Return value of kAudioFxModule_EmptyID
//						indicates failure.
// **************************************************************** 
	int 
AUDIOFX::AddModule(char *typeName)
{
int		i;
AUDIOFX_MODULE *m      = NULL;
AUDIOFX_PLUGIN *plugIn = FindAudioFXPlugInByName(typeName);

// Search for plug-in as specified by typeID
if (!plugIn)
	{
	printf("AddModule: unable to find plug-in specified by '%s'.\n", typeName);
	return (-1);
	}

// Find an empty slot for the module.
for (i = 0; i < maxModules; i++)
	{
	m = &modules[i];
	if (kAudioFxModule_EmptyID == m->moduleID)
		break;
	}

// Uh oh . Unable to find a slot for the module.
if (i == maxModules)
	{
	printf("AddModule: out of module slots.\n");
	return (kAudioFxModule_EmptyID);
	}
InitModule(m);
m->parent = plugIn;

m->moduleID = moduleIDCounter++;
//printf("AddModule: added id=%d of type '%s'\n", m->moduleID, typeName);

return (m->moduleID);
}	// ---- end AddModule() ---- 

// ****************************************************************
// AddModule:		Set up a new audio effect.  
//				Note: this function does not add this module to
//				the execution list.
//
//			Return module ID.  Return value of kAudioFxModule_EmptyID
//						indicates failure.
// **************************************************************** 
	int 
AUDIOFX::AddModule(long typeID)
{
if (kAudioFxModuleTypeID_DMA <= typeID && typeID <= kAudioFxModuleTypeID_Reverb)
	return (AddModule(audioFxModuleTypeNames[typeID]));
else
	return (kAudioFxModuleTypeID_Invalid);
}	// ---- end AddModule() ---- 

#ifdef SAFE
// ****************************************************************
// Command:		Send command to one/all modules.
//
//			Return Boolean success.	
// **************************************************************** 
	int 
AUDIOFX::Command(long moduleID, int commandID, void *data)
{
switch (commandID)
	{
	case kDSPCommand_IgnoreMe:
	break;
	case kDSPCommand_StopDSPList:
	break;
	case kDSPCommand_StartDSPList:
	break;
	case kDSPCommand_LoadDSPModuleBlock:
	break;
	case kDSPCommand_LoadDSPCodeBlock:
	break;

	case kDSPCommand_ResetAllDSPModuleStates:
	break;
	case kDSPCommand_UpdateAllDSPModuleParameters:
	break;
	case kDSPCommand_PrepareAllDSPModules:
	break;

	case kDSPCommand_ResetOneDSPModuleState:
	break;
	case kDSPCommand_UpdateOneDSPModuleParameters:
	break;
	case kDSPCommand_PrepareOneDSPModule:
	break;
	default:
	return (False);
	}

return (True);
}	// ---- end Command() ---- 
#endif

// ****************************************************************
// QueryModule:		Inquire about module state
//
//						Return Boolean success
// **************************************************************** 
	int 
AUDIOFX::QueryModule(long moduleID, int queryID)
{
return (True);
}	// ---- end QueryModule() ---- 

// ****************************************************************
// SetInputFIFOFormat:	Use this call to register actual
//						FIFO settings set by driver.  This
//						call will not reconfigure the FIFO hardware
//
//						Return Boolean success
// **************************************************************** 
	int 
AUDIOFX::SetInputFIFOFormat(int index, int channelFormat, int classDataSize)
{
if (index >= kAudioFX_MaxInputFIFOs)
	return (False);

inputFIFOs[index].classDataSize = classDataSize;
inputFIFOs[index].channelFormat = channelFormat;

return (True);
}	// ---- end SetInputFIFOFormat() ---- 

// ****************************************************************
// SetOutputFIFOFormat:	Use this call to register actual
//						FIFO settings set by driver.  This
//						call will not reconfigure the FIFO hardware
//
//						Return Boolean success
// **************************************************************** 
	int 
AUDIOFX::SetOutputFIFOFormat(int index, int channelFormat, int classDataSize)
{
if (index >= kAudioFX_MaxOutputFIFOs)
	return (False);

outputFIFOs[index].classDataSize = classDataSize;
outputFIFOs[index].channelFormat = channelFormat;

return (True);
}	// ---- end SetOutputFIFOFormat() ---- 

// ****************************************************************
// SetTopologyName:	used to set file names for generated binary outputs
//
// **************************************************************** 
	void 
AUDIOFX::SetTopologyName(char *s)
{
topologyName = s;
}	// ---- end SetTopologyName() ---- 

// ****************************************************************
// RegisterModuleType:		Install audio fx plug-in from system.
//
//		Note:  An FX type may not be reregistered.  This
//				function does not instantiate a module.
//
//			Return type ID
// **************************************************************** 
	int 
AUDIOFX::RegisterModuleType(AUDIOFX_PLUGIN *d)
{
long i;

// Don't register if it already exists
if (FindAudioFXPlugInByName(d->name))
	{
	printf("RegisterModuleType: unable to find space for plugin '%s'\n", d->name);
	return (True);
	}

// Find an empty slot
for (i = 0; i < kAudioFxModule_MaxTypes; i++)
	{
	if (kAudioFxModuleTypeID_Null == plugIns[i].typeID)
		break;
	}
if (kAudioFxModule_MaxTypes == i)
	{
	printf("RegisterModuleType: unable to find space for plugin %d\n", d->typeID);
	return (False);
	}

// Copy plugin data
memcpy(&plugIns[i], d, sizeof(AUDIOFX_PLUGIN));
plugIns[i].typeID = typeIDCounter++;
//printf("RegisterModuleType: %d plugIn name='%s' typeID=%d\n", i, plugIns[i].name, plugIns[i].typeID);

return (plugIns[i].typeID);
}	// ---- end RegisterModuleType() ---- 

// ****************************************************************
// UnregisterModuleType:	Remove audio fx plug-in from system.
//
//			Return Boolean success.	
// **************************************************************** 
	int 
AUDIOFX::UnregisterModuleType(long typeID)
{
int	i;

// Don't unregister if it doesn't exist
AUDIOFX_PLUGIN *thisPlugin = FindAudioFXPlugInByID(typeID);
if (!thisPlugin)
	return (False);

// Remove all instances of this type from the module list
for (i = 0; i < maxModules; i++)
	{
	if (modules[i].parent == thisPlugin)
		InitModule(&modules[i]);
	}

// Clear plug-in entry
InitPlugIn(thisPlugin);

return (True);
}	// ---- end UnregisterModuleType() ---- 

// ****************************************************************
// DeleteModule:	Delete specified module instance
//
//			Return Boolean success.	
// **************************************************************** 
	int 
AUDIOFX::DeleteModule(long moduleID)
{
AUDIOFX_MODULE *m = FindAudioFXModule(moduleID);

if (!m)
	return (False);

m->moduleID = kAudioFxModule_EmptyID;

// Remove module from execution list before state memory is
// deallocated 


// Deallocate state block

// FIXXX: add signal to update DSP state and code blocks.

return (True);
}	// ---- end DeleteModule() ---- 

// ****************************************************************
// DefaultModule:		Set default parameters to specified effect
//
//			Return Boolean success.	
// **************************************************************** 
	int 
AUDIOFX::DefaultModule(long moduleID)
{
AUDIOFX_MODULE *m = FindAudioFXModule(moduleID);

if (!m)
	return (False);
(m->parent->functionIDList[kAudioFX_PlugIn_FunctionID_Default].function)(&m->data);

return (True);
}	// ---- end DefaultModule() ---- 

// ****************************************************************
// ResetModule:	Reset large and small parameter smallState
//
//			Return Boolean success.	
// **************************************************************** 
	int 
AUDIOFX::ResetModule(long moduleID)
{
AUDIOFX_MODULE *m = FindAudioFXModule(moduleID);

if (!m)
	return (False);
(m->parent->functionIDList[kAudioFX_PlugIn_FunctionID_Reset].function)(&m->data);

return (True);
}	// ---- end ResetModule() ---- 

// ****************************************************************
// UpdateModule:		Update parameter values
//					Note:  Update() does not reset the module
//								unless the parameters require 
//								a Reset.
//
//			Returns Boolean indication that a Reset() is needed.
// **************************************************************** 
	int 
AUDIOFX::UpdateModule(long moduleID)
{
AUDIOFX_MODULE *m = FindAudioFXModule(moduleID);
int resetNeeded = False;
if (!m)
	return (False);

// Convert buffer IDs to DSP memory IDs
TranslateModule_IOBufferIDs(m);

(m->parent->functionIDList[kAudioFX_PlugIn_FunctionID_Update].function)(&m->data);

return (resetNeeded);
}	// ---- end UpdateModule() ---- 

// ****************************************************************
// PrepareModule:		Update + Reset
//
//					Return Boolean success
// **************************************************************** 
	int 
AUDIOFX::PrepareModule(long moduleID)
{
AUDIOFX_MODULE *m = FindAudioFXModule(moduleID);

if (!m)
	return (False);

if (m->parent->functionIDList[kAudioFX_PlugIn_FunctionID_Prepare].function)
	(m->parent->functionIDList[kAudioFX_PlugIn_FunctionID_Prepare].function)(&m->data);
else
	{
	(m->parent->functionIDList[kAudioFX_PlugIn_FunctionID_Update].function)(&m->data);
	(m->parent->functionIDList[kAudioFX_PlugIn_FunctionID_Reset ].function)(&m->data);
	}

return (True);
}	// ---- end PrepareModule() ---- 

// ****************************************************************
// SetModule:		Set high level  (usually DirectX parameters), 
//					but do not process	parameters with Update() call.
//
//					Return Boolean success
// **************************************************************** 
	int 
AUDIOFX::SetModule(long moduleID, void *data)
// data		ptr to DX8 parameter structure
{
AUDIOFX_MODULE *m = FindAudioFXModule(moduleID);
if (!m)
	return (False);

// Copy parameter data
Int24 *localData = (Int24 *) m->data.moduleData;
afxSystemMemCopy(localData, data, m->parent->moduleStructSize);

return (True);
}	// ---- end SetModule() ---- 

// ****************************************************************
// GetModule:		Retrieve high level parameters
//
//					Return Boolean success
// **************************************************************** 
	int 
AUDIOFX::GetModule(long moduleID, void *data)
// parameterData		ptr to DX8 parameter structure
{
AUDIOFX_MODULE *m = FindAudioFXModule(moduleID);
if (!m)
	return (False);

// Copy parameter data
Int24 *localData = (Int24 *) m->data.moduleData;
afxSystemMemCopy(data, localData, m->parent->moduleStructSize);

return (True);
}	// ---- end GetModule() ---- 

// ****************************************************************
// GetModuleAFXParametersPtr:	Return ptr to AFX (high+low level) parameters
//
//					Return Boolean success
// **************************************************************** 
	AFX_PARAMETERS * 
AUDIOFX::GetModuleAFXParametersPtr(long moduleID)
{
AUDIOFX_MODULE *m = FindAudioFXModule(moduleID);
if (!m)
	return (False);

return (&m->data);
}	// ---- end GetModuleAFXParametersPtr() ---- 

// ****************************************************************
// GetModuleParameterPtr:		Return ptr high level parameters
//
// **************************************************************** 
	void * 
AUDIOFX::GetModuleParameterPtr(long moduleID)
{
AUDIOFX_MODULE *m = FindAudioFXModule(moduleID);
if (!m)
	return (False);
return (m->data.moduleData);
}	// ---- end GetModuleParameterPtr() ---- 

// ****************************************************************
// InitModule:		
// **************************************************************** 
	void
AUDIOFX::InitModule(AUDIOFX_MODULE *d)
{
int i;

for (i = 0; i < kAudioFX_MaxInputs; i++)
	d->inBufferIDs [i] = kAudioFX_BufferID_MixBuf00;
for (i = 0; i < kAudioFX_MaxOutputs; i++)
	d->outBufferIDs[i] = kAudioFX_BufferID_MixBuf00;

d->ioCount = 1;
d->parent  = NULL;

//d->data.parameters     = d->parameters;
//d->data.smallState     = d->smallState;
d->data.largeState     = 0;
d->data.largeStateSize = 0;
}	// ---- end InitModule() ---- 

// ****************************************************************
// InitPlugIn:		
// **************************************************************** 
	void
AUDIOFX::InitPlugIn(AUDIOFX_PLUGIN *d)
{
int i;

d->maxInputs  = 1;
d->maxOutputs = 1;
d->name       = NULL;
d->typeID     = kAudioFxModuleTypeID_Null;

d->moduleStructSize = 0;
d->smallStateSize   = 0;

for (i = 0; i < kAudioFX_PlugIn_Function_BaseCount; i++)
	{
	d->functionIDList[i].id       = kAudioFX_PlugIn_FunctionID_Null;
	d->functionIDList[i].function = 0;
	}
}	// ---- end InitPlugIn() ---- 

// ****************************************************************
// Update:	Assemble and download DSP code and state blocks
//
//					Return Boolean success
// **************************************************************** 
	int
AUDIOFX::Update()
{
char s[500];
if (!topologyName)
	topologyName = "out";

DefaultCommandNode(&dspCommandNode);

//sprintf(s, "%s.code", topologyName);
//if (!AssembleDSPCodeBlock(s))
//	{
//	printf("Update:  AssembleDSPCodeBlock() failed\n");
//	return (False);
//	}
dspCommandNode.dspModuleCodeBlock     = (Int24) dspCodeBlock;
dspCommandNode.dspModuleCodeBlockSize = dspCodeBlockSize;
// dspCommandNode.commandID |= kDSPCommandID_LoadDSPBlock_Code;

if (topologyName)
	{
	// All of this to get past lack of sprintf() in Resman.
	strcpy(s, "ms_");
	strcat(s, topologyName);
	strcat(s, ".dat");
//	sprintf(s, "ms_%s.dat", topologyName);
	}
else
	strcpy(s, "ms.dat");
if (!AssembleDSPStateBlock(s))
	{
	printf("Update:  AssembleDSPStateBlock() failed\n");
	return (False);
	}
dspCommandNode.moduleCount         = executionListLength;
dspCommandNode.dspModuleBlock      = (Int24) dspModuleBlock;
dspCommandNode.dspModuleBlockSize  = executionListLength*kDSPMODULE_Words;
dspCommandNode.commandID          |= kDSPCommandID_LoadDSPBlock_ModuleList;

dspCommandNode.dspModuleStateBlock     = (Int24) dspStateBlock;
dspCommandNode.dspModuleStateBlockSize = dspStateBlockSize;
dspCommandNode.commandID              |= kDSPCommandID_LoadDSPBlock_State;

// FIXXXX: add code to transfer blocks to System Memory
{
}

// Create DSP command node
{
dspCommandNode.dspModuleBlock     = 2;
dspCommandNode.dspModuleBlockSize = 2;


#ifdef AUDIOFX_STANDALONE
WriteWordsToFile("CommandNode.dat", (Int24 *) &dspCommandNode, sizeof(DSPCOMMANDNODE)/sizeof(Int24));
#else
#endif
}

return (True);
}	// ---- end Update() ---- 

// ****************************************************************
// AssembleDSPCodeBlock:	Concatenate DSP code blocks for
//							all modules included in the execution list.
//							Beginning of block is the list of common
//							routines.  Be sure to link any DSP code
//							functions.
//
//							This function will also fill in the
//							'state' and 'typeID' fields in the
//							DSPModuleBlock.
//
//					Return Boolean success
// **************************************************************** 
	int
AUDIOFX::AssembleDSPCodeBlock(char *filePath)
{
int i, j;
DSPMODULE *modulePtr  = (DSPMODULE *) dspModuleBlock;
Int24      blockIndex = 0;
long	   blockSize  = 0;
long	   computeRoutineAddress[kAudioFxModule_MaxTypes];	
AUDIOFX_MODULE *moduleList[kAudioFx_MaxModules];
AUDIOFX_MODULE *m;

// Setup 
#define kAudioFx_InvalidComputeRoutineAddress	(-1)
for (i = 0; i < kAudioFxModule_MaxTypes; i++)
	computeRoutineAddress[i] = kAudioFx_InvalidComputeRoutineAddress;

// First, calculate size of block.
blockSize = dspCommonRoutinesSize;
for (i = 0; i < executionListLength; i++)
	{
	m = FindAudioFXModule(executionList[i]);
// Failure !  The execution list specifies an invalid module.
	if (!m)
		return (False);
	moduleList[i] = m;
	blockSize += m->parent->dspComputeRoutineSize;
	}

// Allocate new block
if (dspCodeBlock)
        afxSystemMemFree(dspCodeBlock);
dspCodeBlock     = (Int24 *) afxSystemMemAlloc(sizeof(Int24)*blockSize);
dspCodeBlockSize = blockSize;

// Copy module info to new block
#ifdef AUDIOFX_STANDALONE
afxSystemMemCopy(dspCodeBlock, (void *) dspCommonRoutines, sizeof(Int24)*dspCommonRoutinesSize);
#else
afxScratchMemWrite((U032)dspCodeBlock, (void *) dspCommonRoutines, sizeof(Int24)*dspCommonRoutinesSize);
#endif

blockIndex += dspCommonRoutinesSize;
for (i = 0; i < executionListLength; i++)
	{
	m = moduleList[i];

// Add code to block if it doesn't yet exist
	if (kAudioFx_InvalidComputeRoutineAddress == computeRoutineAddress[0])
		{
#ifdef AUDIOFX_STANDALONE
		afxScratchMemCopy(dspCodeBlock+blockIndex, (void *) m->parent->dspComputeRoutine, sizeof(Int24)*m->parent->dspComputeRoutineSize);
#else
		afxScratchMemWrite((U032)(dspCodeBlock+blockIndex), (void *) m->parent->dspComputeRoutine, sizeof(Int24)*m->parent->dspComputeRoutineSize);
#endif
//		computeRoutineAddress[m->typeID] = blockIndex;

//		blockIndex += dspComputeRoutineSizes[m->typeID];
		}

// Assign FX pins (bufferIDs) to DSP addresses.  Process IO pairs
//			because that is the format expected by the DSP.
// Output address in format {all inputs, all outputs}
	for (j = 0; j < m->ioCount/2; j++)
		m->data.smallState[j] = BufferIDToDSPAddress(m->inBufferIDs[j]);

	for (     ; j < m->ioCount; j++)
		m->data.smallState[j] = BufferIDToDSPAddress(m->outBufferIDs[j]);

// Fill in 'compute' and 'typeID' fields in DSPModuleBlock.  Don't touch 'state' field.
//	modulePtr->compute = m->parent->dspComputeRoutine;
	modulePtr->typeID  = m->parent->typeID;
	modulePtr         += sizeof(DSPMODULE);
	}

WriteWordsToFile("DspCode.bin", dspStateBlock, blockIndex+1);

return (True);
}	// ---- end AssembleDSPCodeBlock() ---- 

// ****************************************************************
// AssembleDSPStateBlock:	Concatenate small state blocks for
//							all modules included in the execution list.
//							This function will allocate the correct 
//							block size.
//
//							This function will also fill in the
//							'state' and 'typeID' fields in the
//							DSPModuleBlock.
//
//		The DSP state block will reside in DSP X/YRAM
//
//					Return # words in state block, 0 on failure
// **************************************************************** 
	int
AUDIOFX::AssembleDSPStateBlock(char *filePath)
{
int i, j;
DSPMODULE *modulePtr  = (DSPMODULE *) dspModuleBlock;
Int24      blockIndex = 0;
long	   blockSize  = 0;
AUDIOFX_MODULE *moduleList[kAudioFx_MaxModules];
AUDIOFX_MODULE *m;
int byteSwap = True;

//printf("AssembleDSPStateBlock to '%s'\n", filePath);

// First, calculate size of block.
for (i = 0; i < executionListLength; i++)
	{
	m = FindAudioFXModule(executionList[i]);
// Failure !  The execution list specifies an invalid module.
	if (!m)
		{
		printf("AssembleDSPStateBlock: module %d not found\n", executionList[i]);
		return (0);
		}
	AUDIOFX_PLUGIN *plugIn = m->parent;

	moduleList[i] = m;
	blockSize += plugIn->smallStateSize;
	}

// Allocate new block
if (dspStateBlock)
      afxSystemMemFree(dspStateBlock);
dspStateBlock = (Int24 *) afxSystemMemAlloc(blockSize*sizeof(Int24));
if (!dspStateBlock)
	{
	printf("AssembleDSPStateBlock: unable to allocate new state block\n");
	return (0);
	}
dspStateBlockSize = blockSize;

//printf("AssembleDSPStateBlock: dspStateBlock=%d size=%d\n", dspStateBlock, dspStateBlockSize);

// Copy module info to new block
for (i = 0; i < executionListLength; i++)
	{
	m = moduleList[i];
	AUDIOFX_PLUGIN *plugIn = m->parent;
	if (plugIn)
		{
		Int24 *bPtr = dspStateBlock+blockIndex;
#ifdef AUDIOFX_STANDALONE
	    afxScratchMemCopy(dspStateBlock+blockIndex, (void *) m->data.smallState, sizeof(Int24)*plugIn->smallStateSize);
#else
		afxScratchMemWrite((U032)(dspStateBlock+blockIndex), (void *) m->data.smallState, sizeof(Int24)*plugIn->smallStateSize);
#endif

	// Fill connection information  (All inputs, followed by outputs)
		for (j = 0; j < plugIn->maxInputs; j++)
			*bPtr++ = BufferIDToDSPAddress(m->inBufferIDs[j]);	
		for (j = 0; j < plugIn->maxOutputs; j++)
			*bPtr++ = BufferIDToDSPAddress(m->outBufferIDs[j]);	
		*bPtr++ = kFrameLength;

	// Update DSP Module in execution list
	   blockIndex += plugIn->smallStateSize;
//printf("AssembleDSPStateBlock %2d: moduleID=%2d, typeID=%2d (%s), stateSize=%2d\n",
//	   i, m->moduleID, plugIn->typeID, plugIn->name, plugIn->smallStateSize);
	// Fill in 'state' and 'typeID' fields in DSPModuleBlock. 
		modulePtr->state  = blockIndex;
		modulePtr->typeID = plugIn->typeID;
		modulePtr        += sizeof(DSPMODULE);
		}
	}

// Blast to file
#ifdef AUDIOFX_STANDALONE
{
FILE *h = fopen(filePath, "w+b");
if (NULL == h)
	{
    printf("AssembleDSPStateBlock: unable to open file '%s'\n", filePath);
    return (0);
	}

// Shuffle bytes to write packed 24 bit words
unsigned char *a = (unsigned char *) dspStateBlock;
unsigned char *b = (unsigned char *) dspStateBlock;
if (byteSwap)
	{
	ByteSwapBuf32((long *)dspStateBlock, dspStateBlockSize);
	for (int i = 0; i < dspStateBlockSize; i++, a += 3, b += 4)
		{
		a[0] = b[1];
		a[1] = b[2];
		a[2] = b[3];
		}
	}
else
	{
	for (int i = 0; i < dspStateBlockSize; i++, a += 3, b += 4)
		{
		a[0] = b[0];
		a[1] = b[1];
		a[2] = b[2];
		}
	}

long bytesToWrite = (3*sizeof(Int24)*dspStateBlockSize)/4;
long bytesWritten = fwrite(dspStateBlock, sizeof(char), bytesToWrite, h);
//printf("AssembleDSPStateBlock: wrote %d words (%d bytes) to '%s'\n", 
//	   dspStateBlockSize, bytesWritten, filePath);
fclose(h);
}
#endif

return (dspStateBlockSize);
}	// ---- end AssembleDSPStateBlock() ---- 

// ****************************************************************
// FindAudioFXModule:	 Return ptr to module with specified ID	
// **************************************************************** 
	AUDIOFX_MODULE *
AUDIOFX::FindAudioFXModule(long moduleID)
{
// Scan array for particular module
for (int i = 0; i < maxModules; i++)
	{
	if (moduleID == modules[i].moduleID)
		return (&modules[i]);
	}

return (NULL);
}	// ---- end FindAudioFXModule() ---- 

// ****************************************************************
// FindAudioFXPlugInByID:	 Return ptr to plug-in with specified ID	
// **************************************************************** 
	AUDIOFX_PLUGIN *
AUDIOFX::FindAudioFXPlugInByID(long typeID)
{
// Scan array for particular module
for (int i = 0; i < kAudioFxModule_MaxTypes; i++)
	{
	if (typeID == plugIns[i].typeID)
		return (&plugIns[i]);
	}

return (NULL);
}	// ---- end FindAudioFXPlugInByID() ---- 

// ****************************************************************
// FindAudioFXPlugInByName:	 Return ptr to plug-in with specified name	
// **************************************************************** 
	AUDIOFX_PLUGIN *
AUDIOFX::FindAudioFXPlugInByName(char *name)
{
// Scan array for particular module
for (long i = 0; i < kAudioFxModule_MaxTypes; i++)
	{
	if (plugIns[i].name)
		{
		if(!strcmp(name, plugIns[i].name))
			return (&plugIns[i]);
		}
	}

return (NULL);
}	// ---- end FindAudioFXPlugInByName() ---- 

// **************************************************************** 
// WriteWordsToFile:	 Write array of Int24's to specified file
//
//					Return words written	
// **************************************************************** 
	long
AUDIOFX::WriteWordsToFile(char *filePath, Int24 *data, long count)
{
#ifdef AUDIOFX_STANDALONE
FILE *h = fopen(filePath, "w+b");
if (!h)
	{
	printf("WriteWordsToFile: failed to create '%s'\n", filePath);
	return (False);
	}

// Blast data (swap bytes!) to specified file
long wordsWritten = fwrite( data, sizeof(Int24), count, h );
if (wordsWritten != count)
	{
	printf("WriteWordsToFile: short write of %d/%d to '%s'\n",
				wordsWritten, count, filePath);
	fclose(h);
	return (False);
	}

//printf("WriteWordsToFile: wrote %d words to '%s'\n", wordsWritten, filePath);

fclose(h);
return (wordsWritten);
#else
return (0);
#endif
}	// ---- end WriteWordsToFile() ---- 

// **************************************************************** 
// FxTypeIDToName:	 Return ptr to effect name string
//
// **************************************************************** 
	char *
AUDIOFX::FxTypeIDToName(long typeID)
{
AUDIOFX_PLUGIN *plugIn = FindAudioFXPlugInByID(typeID);
if (!plugIn)
	{
	printf("FxTypeIDToName: unable to find plugIn type=%d\n", typeID);
	return (0);
	}

return (plugIn->name);
}	// ---- end FxTypeIDToName() ---- 

// **************************************************************** 
// FxParameterSize:	 Return size of high-level (often DirectX8) structure
//
// **************************************************************** 
	long
AUDIOFX::FxParameterSize(long typeID)
{
AUDIOFX_PLUGIN *plugIn = FindAudioFXPlugInByID(typeID);
if (!plugIn)
	{
	printf("FxParameterSize: unable to find plugIn type=%d\n", typeID);
	return (0);
	}

return (plugIn->moduleStructSize);

return (0);
}	// ---- end FxParameterSize() ---- 




