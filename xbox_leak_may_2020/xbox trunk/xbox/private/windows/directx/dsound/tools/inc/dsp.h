#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#define MAX_FX_INPUTS 50
#define MAX_FX_OUTPUTS 50

#define Int24 long


//
// max processing cycles in the GP, per 32 sample frame
//

#define DSP_MAXCYCLES_AVAILABLE 100000
#define DSP_BUFFER_FRAME_LENGTH	32

//
// each FX we download to the DSP will be pointed to a space in Xram
// That space will contain a fixed data structure followed by FX specific variables
// the data structure below has to match what is defined in the DSP asm includes

typedef struct DSP_STATE_FIXED {

	DWORD   dwScratchBase;
	DWORD   dwScratchLength;
	DWORD   dwYMemBase;
	DWORD   dwYMemLength;
	DWORD   dwFlags;
	DWORD	dwIoPtrs[2]; 

} DSP_STATE_FIXED, *PDSP_STATE_FIXED;

//
// state flags
//
#define BIT_FX_STATE_FLAG_INITIALIZED		0
#define VALUE_FX_STATE_FLAG_INITIALIZED     1

#define BIT_FX_STATE_FLAG_GLOBAL_FX         1
#define VALUE_FX_STATE_FLAG_GLOBAL_FX       2

//
// GP memory offsets for the mixbins specifed as sources/outputs
// BUGBUG part of XRAM inside the dsp is mapped to VP mixbins. Its ok to write to that for now..
//

#define DSP_VPMIXBIN_MEMOFFSET_BASE		0x001400	
#define DSP_GPMIXBIN_MEMOFFSET_BASE		0x000c00	

//
// defines that describe the scratch image file layout
//

typedef struct _SCRATCH_IMAGE_FX_DESCRIPTION {

	DWORD dwCodeOffset;
	DWORD dwCodeSize;
	DWORD dwStateOffset;
	DWORD dwStateSize;
	DWORD dwYMemOffset;
    DWORD dwYMemSize;
	DWORD dwScratchOffset;
	DWORD dwScratchSize;

} SCRATCH_IMAGE_FX_DESCRIPTION;

//
// immediately following the descriptor is an array of
// SCRATCH_IMAGE_FX_DESCRIPTION data structs.
// Each struct describes an effect parsed. The elements are ordered
// in the order they were parsed from the fx description INI file
// 

typedef struct _SCRATCH_IMAGE_DESCRIPTOR {

	DWORD dwNumFX;
    DWORD dwTotalScratchSize;

} SCRATCH_IMAGE_DESCRIPTOR;


typedef struct HOST_TO_DSP_COMMANDBLOCK {

    DWORD dwOffset;
	DWORD dwCodeLength;
	DWORD dwStateOffset;
	DWORD dwStateLength;
	DWORD dwCommandFlags;
	DWORD dwDspStatus;


} HOST_TO_DSP_COMMANDBLOCK, *PHOST_TO_DSP_COMMANDBLOCK;

//
// command block field defines
//

#define BIT_H2DCB_COMMAND_PRESENT			0x00
#define BIT_H2DCB_COMMAND_LOAD_CODE         0x01
#define BIT_H2DCB_COMMAND_LOAD_XRAM_STATE   0x02
#define BIT_H2DCB_COMMAND_UPDATE_XRAM_STATE 0x04


#define DSP_PMEMORY_SIZE	4096*sizeof(DWORD)
#define DSP_XMEMORY_SIZE	3072*sizeof(DWORD)
#define DSP_YMEMORY_SIZE	2048*sizeof(DWORD)

//
// the definition below is an estimate of the currnet size of the engine.
// 

#define DSP_EXECUTION_ENGINE_SIZE  0x600  // in bytes

//
// the following offsets can be moved around since we set up the command node
// with relative pointers
// The definitions blow must match dsound\dsp\fx\include\gputil.h
//

#define DSP_COMMANDBLOCK_SCRATCHOFFSET  	 2048

//
// base offset in scratch were we put the GP audio output + multipass
//

#define DSP_AUDIO_DATA_SCRATCHOFFSET	(((DSP_COMMANDBLOCK_SCRATCHOFFSET+sizeof(HOST_TO_DSP_COMMANDBLOCK)+\
                                           DSP_PMEMORY_SIZE+DSP_XMEMORY_SIZE - DSP_EXECUTION_ENGINE_SIZE + PAGE_SIZE-1)/PAGE_SIZE)* PAGE_SIZE)
	
//
// NOTE: keep in sync with dsound. 3 pages for ac3, 1 page for multipass

#define DSP_AUDIO_DATA_SIZE		(PAGE_SIZE*4)

#define DSP_FX_DELAY_DATA_SCRATCHOFFSET (DSP_AUDIO_DATA_SCRATCHOFFSET+DSP_AUDIO_DATA_SIZE)

// #############################################################
// NOTE: The definitions for Xmemory offsets and data structures
// MUST BE KEPT IN SYNC WITH THE DSP CODE!!!!
// The equivalent definitions for the dsp execution engine
// are at dsound\dsp\include\io.h
// #############################################################

//
// any variable in X-mem must be at this bas eoffset or higher
// This must be in synch with with dsp\fx\source\defaultengine.asm
// and the definition of dspStateBlockXRAM
//

#define DSP_FX_STATE_YMEMORY_BASE_OFFSET 0
#define DSP_FX_STATE_XMEMORY_BASE_OFFSET (128*sizeof(DWORD))

//
// XMEM offset of AC3 buffer variable. Set by the mcpx to tell the GP dma where to put the
// audio output for the EP to read..
//

#define DSP_AC3BUFFER_XMEMORY_OFFSET            (DSP_FX_STATE_XMEMORY_BASE_OFFSET-sizeof(DWORD))   
#define DSP_MULTIPASS_BUFFER_XMEMORY_OFFSET     (DSP_FX_STATE_XMEMORY_BASE_OFFSET-(sizeof(DWORD)*2))   

//
// both xram and scratch contain mcxp overheaed so they are not 
// completely available for FX 
//

#define DSP_FX_STATE_MAX_SIZE ((2048+1024-512)*sizeof(DWORD))

#define DSP_GPTEMPBIN_MEMOFFSET_BASE (DSP_FX_STATE_XMEMORY_BASE_OFFSET + DSP_FX_STATE_MAX_SIZE)	
#define DSP_MIN_FX_CODE_SIZE (sizeof(DWORD)*2)

//
// various opcodes we look for in the image builder
//

#define DSP_563000_OPCODE_RTS		    	0x0000000C
#define DSP_563000_OPCODE_MOVE_X_TO_R5_IMM	0x0065F400
