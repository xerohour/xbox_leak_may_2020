#ifndef __NV_UGP_H__
#define __NV_UGP_H__

// Gp interface methods for a MCP1 client
// The client will use NV1BA0_CHANNEL_DMA_PUT to put the functions in the commandQ.
// methods are 32 bits.  data is 32 bits

// Address in GP DSP PMEM to place offset of shared EP/GP AC3 output buffer
#define NV_AUDGP_DSP_AC3_OFFSET_ADDR            0x28
#define NV_AUDGP_DSP_GPCOMMANDNODE_OFFSET_ADDR  0x29	// GP Command Node Base Address

// METHOD: SetContextDmaDspBlock - sets context DMA for memory holding the DSP data
// DATA(32) : Context DMA handle
//#define NV_AUDGP_SET_CONTEXT_DMA_DSP_BLOCK               0x0000000
//#define NV_AUDGP_SET_CONTEXT_DMA_DSP_BLOCK_HANDLE        31:0

// METHOD: ReadDSPBlock  read one block from scratch memory
// DATA(32): block ID  (code, module list, state block)
// DATA(32): block byte address
// DATA(32): block byte size
#define NV_AUDGP_READ_DSP_BLOCK                    0x00000001
#define NV_AUDGP_READ_DSP_BLOCK_ID					31:0
#define NV_AUDGP_READ_DSP_BLOCK_ADDRESS				31:0
#define NV_AUDGP_READ_DSP_BLOCK_SIZE				31:0

// METHOD: WriteDSPBlock  write one block to scratch memory
// DATA(32): block ID  (code, module list, state block)
// DATA(32): block byte address
// DATA(32): block byte size
#define NV_AUDGP_WRITE_DSP_BLOCK                    0x00000002
#define NV_AUDGP_WRITE_DSP_BLOCK_ID					31:0
#define NV_AUDGP_WRITE_DSP_BLOCK_ADDRESS			31:0
#define NV_AUDGP_WRITE_DSP_BLOCK_SIZE				31:0

#define NV_AUDGP_DSP_BLOCK_ID_COMMAND_NODE         0x0
#define NV_AUDGP_DSP_BLOCK_ID_MODULE_LIST          0x1
#define NV_AUDGP_DSP_BLOCK_ID_STATE_XRAM           0x2
#define NV_AUDGP_DSP_BLOCK_ID_STATE_YRAM           0x3
#define NV_AUDGP_DSP_BLOCK_ID_CODE                 0x4
#define NV_AUDGP_DSP_BLOCK_ID_STATE_LARGE          0x5

// Some DSP block byte addresses (set in command node)
#define NV_AUDGP_DSP_BLOCK_ID_COMMAND_NODE_OFFSET    19000	// $ 4A38
#define NV_AUDGP_DSP_BLOCK_ID_MODULE_LIST_OFFSET     19100	// $ 4A9C
#define NV_AUDGP_DSP_BLOCK_ID_STATE_XRAM_OFFSET      19500	// $ 4C2C
#define NV_AUDGP_DSP_BLOCK_ID_STATE_YRAM_OFFSET      31788	// $ 7C2C
#define NV_AUDGP_DSP_BLOCK_ID_CODE_OFFSET			 39980	// $ 9C2C
#define NV_AUDGP_DSP_BLOCK_ID_STATE_LARGE_OFFSET     56464	// $ DC90

/*
METHOD: RouteEffect
DATA(32):   effect instance id
            Bit identifying if the effect is the source, or destination,
            Effect Pin 
            Mixer buffer ID
            enable or disable data connection
*/
//#define NV_AUDGP_ROUTE_EFFECT                           0x00000003
#define NV_AUDGP_ROUTE_EFFECT_FILLER                    7:0	// was effectID
#define NV_AUDGP_ROUTE_EFFECT_ORIENTATION               8:8
#define NV_AUDGP_ROUTE_EFFECT_ORIENTATION_SOURCE        0x0
#define NV_AUDGP_ROUTE_EFFECT_ORIENTATION_DEST          0x1
#define NV_AUDGP_ROUTE_EFFECT_PIN                       12:9
#define NV_AUDGP_ROUTE_EFFECT_BUFFER_ID                 20:13
#define NV_AUDGP_ROUTE_EFFECT_CONNECTION                21:21
#define NV_AUDGP_ROUTE_EFFECT_CONNECTION_ENABLE         0x0
#define NV_AUDGP_ROUTE_EFFECT_CONNECTION_DISABLE        0x1

typedef union _routeEffect
{
    struct
    {
        U032 stuf           : 8;	// Filler material
        U032 orientation    : 1;
        U032 pin            : 4;
        U032 bufid          : 8;
        U032 connection     : 1;
        U032                : 10;
    };

	U032 effectID;
    U032 uValue;
} STRUCT_ROUTE_EFFECT;

/*
METHOD: RouteToFIFO - Connects either an effect or buffer to a FIFO 
DATA(32) :  source type (effect or buffer)
            source id (if effect then effect ID, else buffer ID)
            source pin (only used if source type = effect, in which case it's the effect pin)
            fifo type (input fifo, or output fifo) - configure with NV_AUDGP_CONFIGURE_XXX_FIFO
            fifo pin
            connection enable or disable
*/
//#define NV_AUDGP_ROUTE_TO_FIFO                      0x00000004
#define NV_AUDGP_ROUTE_TO_FIFO_SOURCE_TYPE          0:0
#define NV_AUDGP_ROUTE_TO_FIFO_SOURCE_TYPE_EFFECT   0x0
#define NV_AUDGP_ROUTE_TO_FIFO_SOURCE_TYPE_BUFFER   0x1
#define NV_AUDGP_ROUTE_TO_FIFO_SOURCE_ID            8:1
#define NV_AUDGP_ROUTE_TO_FIFO_SOURCE_PIN           12:9
#define NV_AUDGP_ROUTE_TO_FIFO_TYPE                 13:13
#define NV_AUDGP_ROUTE_TO_FIFO_TYPE_INPUT           0x0
#define NV_AUDGP_ROUTE_TO_FIFO_TYPE_OUTPUT          0x1
#define NV_AUDGP_ROUTE_TO_FIFO_NUMBER               15:14
#define NV_AUDGP_ROUTE_TO_FIFO_PIN                  19:16
#define NV_AUDGP_ROUTE_TO_FIFO_PIN_0                0x0
#define NV_AUDGP_ROUTE_TO_FIFO_PIN_1                0x1
#define NV_AUDGP_ROUTE_TO_FIFO_PIN_2                0x2
#define NV_AUDGP_ROUTE_TO_FIFO_PIN_3                0x3
#define NV_AUDGP_ROUTE_TO_FIFO_PIN_4                0x4
#define NV_AUDGP_ROUTE_TO_FIFO_PIN_5                0x5
#define NV_AUDGP_ROUTE_TO_FIFO_CONNECTION           20:20
#define NV_AUDGP_ROUTE_TO_FIFO_CONNECTION_ENABLE    0x0
#define NV_AUDGP_ROUTE_TO_FIFO_CONNECTION_DISABLE   0x1

typedef union _routeEffectToFifo
{
    struct
    {
        U032 sourceType : 1;
        U032 sourceID   : 8;
        U032 sourcePin  : 4;
        U032 fifoType   : 1;
        U032 fifoNum    : 2;
        U032 fifoPin    : 4; 
        U032 connection : 1;
        U032            : 11;
    };

    U032 uValue;
} STRUCT_ROUTE_TO_FIFO;

/* 
METHOD: GetMixerBuffer - Reserves a mixer buffer ID to used for connecting/routing effects.
This ID is used in RouteEffect calls, and is returned in a notifier as a 8-bit identifier.
DATA(32): Allocate HW mix bin, or a SW mix bin (mix bin in the DSP/scratch memory)
Return(8) : Mixer buffer ID
*/
#define NV_AUDGP_GET_MIX_BUFFER                         0x0000000B
#define NV_AUDGP_GET_MIX_BUFFER_TYPE                    1:0
#define NV_AUDGP_GET_MIX_BUFFER_TYPE_HW                 0x0
#define NV_AUDGP_GET_MIX_BUFFER_TYPE_SW                 0x1
#define NV_AUDGP_GET_MIX_BUFFER_TYPE_DONT_CARE          0x2

/* 
METHOD: FreeMixerBuffer - Frees an allocated mix buffer
DATA(32): ID of buffer to free
*/
#define NV_AUDGP_FREE_MIX_BUFFER                        0x0000000C
#define NV_AUDGP_FREE_MIX_BUFFER_ID                     7:0

/*
METHOD: SetContextDmaNotifier - Sets the context DMA for the GP notifier
DATA(32): Handle returned from AllocContextDma
*/
#define NV_AUDGP_SET_CONTEXT_DMA_NOTIFIER               0x0000000D
#define NV_AUDGP_SET_CONTEXT_DMA_NOTIFIER_VALUE         31:0

/*
METHOD: ConfigureOutputFIFO(i) - configure the output fifo - do this before you point an
effect to it.
DATA(32) : Data format
            If the data transfers are isochronous
            Data size and container size where
            NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_8_8       =  8-bit data in 8-bit container(always in blocks of 4)
            NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_16_16     = 16-bit data in 16-bit container (always in blocks of two)
            NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_24_32_MSB = 24 bit data in 32-bit container (msb justified)
            NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_24_32_LSB = 24-bit data in 32-bit container (lsb justified)
            NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_32_32     = 32 bit data in 32-bit container
*/
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO(i)               (0x00000010+(i))
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO_FORMAT           1:0
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO_FORMAT_MONO      0x0
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO_FORMAT_STEREO    0x1
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO_FORMAT_QUAD      0x2
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO_FORMAT_HEX       0x3
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO_TYPE             2:2
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO_TYPE_NONISO      0x0
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO_TYPE_ISO         0x1
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE             5:3
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_8_8         0x0
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_16_16       0x1
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_24_32_MSB   0x2
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_24_32_LSB   0x3
#define NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_32_32       0x4

// METHOD: ConfigureInputFIFO(i) - configure the input fifo
#define NV_AUDGP_CONFIGURE_INPUT_FIFO(i)                (0x00000014+(i))
#define NV_AUDGP_CONFIGURE_INPUT_FIFO_FORMAT            1:0
#define NV_AUDGP_CONFIGURE_INPUT_FIFO_FORMAT_MONO       0x0
#define NV_AUDGP_CONFIGURE_INPUT_FIFO_FORMAT_STEREO     0x1
#define NV_AUDGP_CONFIGURE_INPUT_FIFO_FORMAT_QUAD       0x2
#define NV_AUDGP_CONFIGURE_INPUT_FIFO_FORMAT_HEX        0x3
#define NV_AUDGP_CONFIGURE_INPUT_FIFO_TYPE              2:2
#define NV_AUDGP_CONFIGURE_INPUT_FIFO_TYPE_NONISO       0x0
#define NV_AUDGP_CONFIGURE_INPUT_FIFO_TYPE_ISO          0x1
#define NV_AUDGP_CONFIGURE_INPUT_FIFO_SIZE              5:3
#define NV_AUDGP_CONFIGURE_INPUT_FIFO_SIZE_8_8          0x0
#define NV_AUDGP_CONFIGURE_INPUT_FIFO_SIZE_16_16        0x1
#define NV_AUDGP_CONFIGURE_INPUT_FIFO_SIZE_24_32_MSB    0x2
#define NV_AUDGP_CONFIGURE_INPUT_FIFO_SIZE_24_32_LSB    0x3
#define NV_AUDGP_CONFIGURE_INPUT_FIFO_SIZE_32_32        0x4



typedef union _configureFifo
{
    struct
    {
        U032 format         : 2;
        U032 type           : 1;
        U032 size           : 3;
        U032                : 26;
    };

    U032 uValue;
} STRUCT_CONFIGURE_FIFO;


// METHOD: AllocScratchMemory - Used to allocate scratch memory
// DATA(32) : Number of bytes to allocate
// Return(32) : Offset into scratch memory
#define NV_AUDGP_ALLOC_SCRATCH_MEMORY                   0x00000017
#define NV_AUDGP_ALLOC_SCRATCH_MEMORY_BYTES             31:0

// METHOD: FreeScratchMemory - Used to free allocated scratch memory
// DATA(32) : Offset into scratch memory
// DATA(32) : Number of bytes to free
#define NV_AUDGP_FREE_SCRATCH_MEMORY                    0x00000018
#define NV_AUDGP_FREE_SCRATCH_MEMORY_OFFSET             31:0
#define NV_AUDGP_FREE_SCRATCH_MEMORY_SIZE               31:0

// METHOD: SetScratchMemory - Used to set scratch memory to a value
// DATA(32) : Offset into scratch memory
// DATA(32) : Number of DWORDS to set
// DATA(32) : Value to set memory
#define NV_AUDGP_SET_SCRATCH_MEMORY                     0x00000019
#define NV_AUDGP_SET_SCRATCH_MEMORY_OFFSET              31:0
#define NV_AUDGP_SET_SCRATCH_MEMORY_SIZE				31:0
#define NV_AUDGP_SET_SCRATCH_MEMORY_VALUE               31:0

// METHOD: ReadScratchMemory  read blocks to scratch memory
// DATA(32): block system memory byte address offset
// DATA(32): block byte size
#define NV_AUDGP_READ_SCRATCH_MEMORY                 0x0000001A
#define NV_AUDGP_READ_SCRATCH_MEMORY_OFFSET				31:0
#define NV_AUDGP_READ_SCRATCH_MEMORY_ADDRESS			31:0
#define NV_AUDGP_READ_SCRATCH_MEMORY_SIZE				31:0

// METHOD: WriteScratchMemory  write blocks to scratch memory
// DATA(32): block system memory byte address offset
// DATA(32): block byte size
#define NV_AUDGP_WRITE_SCRATCH_MEMORY                 0x0000001B
#define NV_AUDGP_WRITE_SCRATCH_MEMORY_OFFSET			31:0
#define NV_AUDGP_WRITE_SCRATCH_MEMORY_ADDRESS			31:0
#define NV_AUDGP_WRITE_SCRATCH_MEMORY_SIZE				31:0

/*
METHOD: SetCallbackTimer - sets the callback timer at which the callback registered
                           by the client in AUDIO_INIT_DEVICE.pISRFn is called
DATA(32) : callback control field
*/

#define NV_AUDGP_SET_CALLBACK_TIMER                     0x00000020
#define NV_AUDGP_SET_CALLBACK_TIMER_OPERATION           0:0
#define NV_AUDGP_SET_CALLBACK_TIMER_OPERATION_STOP      0x0
#define NV_AUDGP_SET_CALLBACK_TIMER_OPERATION_START     0x1
#define NV_AUDGP_SET_CALLBACK_TIMER_TYPE                1:1
#define NV_AUDGP_SET_CALLBACK_TIMER_TYPE_1_SHOT         0x0
#define NV_AUDGP_SET_CALLBACK_TIMER_TYPE_PERIODIC       0x1
#define NV_AUDGP_SET_CALLBACK_TIMER_OPERATION_INTERVAL  18:2        /* time interval in milliseconds */
                                                                    /* finer resolution sacrificed for performance */

typedef union _callbackTimer
{
    struct
    {
        U032 op             : 1;
        U032 type           : 1;
        U032 interval       : 18;
        U032                : 12;
    };
    
    U032 uValue;
} STRUCT_SET_TIMER;

/*
METHOD: SetCallbackTimerServiceContext - sets the service context for the above timer
DATA(32) : value
*/

/*
i'm holding this off till we get a good case to set service context out here
instead of in AUDIO_INIT_DEVICE.pServiceContext

#define NV_AUDGP_SET_CALLBACK_TIMER_SERVICE_CONTEXT         0x00000021 
#define NV_AUDGP_SET_CALLBACK_TIMER_SERVICE_CONTEXT_VALUE   31:0
*/

#endif

