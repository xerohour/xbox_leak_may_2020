#ifndef __NV_UGP_H__
#define __NV_UGP_H__

// Gp interface methods for a MCP1 client
// The client will use NV1BA0_CHANNEL_DMA_PUT to put the functions in the commandQ.
// methods are 32 bits.  data is 32 bits


/*
for now... first pass debug!!!
forget RegisterEffect - directly use AddEffect(), passing hardcoded IDs defined in audiofx.h
*/


/*
METHOD: RegisterEffect - registers an effect with the RM.
DATA(32): pointer to structure (to be defined by Gints), which will define the effect completely.
Return(8): Effect ID in a notifier
*/

#define NV_AUDGP_REGISTER_EFFECT                        0x00000000
#define NV_AUDGP_REGISTER_EFFECT_DATA                   31:0

/*
METHOD: AddEffect - Adds a preregistered effect to the execultion list of the DSP.
DATA(32): Effect ID, of an already registered effect
Return(8): effect instance ID in a notifier
*/

#define NV_AUDGP_ADD_EFFECT                             0x00000001
#define NV_AUDGP_ADD_EFFECT_EFFECT_ID                   31:0

/*
METHOD: SetCurrentEffect - sets the effect for future calls (only used with NV_AUDGP_SET_CONTEXT_DMA_PARAMETER)
DATA(32): effect instance ID
*/

#define NV_AUDGP_SET_CURRENT_EFFECT                     0x00000002
#define NV_AUDGP_SET_CURRENT_EFFECT_INSTANCE_ID         31:0

/*
METHOD: RouteEffect
DATA(32):   effect instance id
            Bit identifying if the effect is the source, or destination,
            Effect Pin 
            Mixer buffer ID
            enable or disable data connection
*/

#define NV_AUDGP_ROUTE_EFFECT                           0x00000003
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

#define NV_AUDGP_ROUTE_TO_FIFO                      0x00000004
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
METHOD: QueryEffect - generic query effect to find out effect status, the result
is effect dependent and returned in a notifier
DATA(32):   effect instance id
            Query type
*/

#define NV_AUDGP_QUERY_EFFECT                           0x00000005
#define NV_AUDGP_QUERY_EFFECT_ID                        31:0
#define NV_AUDGP_QUERY_EFFECT_TYPE                      31:0

typedef union _queryEffect
{
        U032 effectID;
        U032 typeID;

    U032 uValue;
} STRUCT_QUERY_EFFECT;

/*
METHOD: UpdateEffectParameters - updates the parameters for the current effect
DATA(32): effect instance id
*/

#define NV_AUDGP_UPDATE_EFFECT_PARAMETERS               0x00000006
#define NV_AUDGP_UPDATE_EFFECT_PARAMETERS_EFFECT_ID     31:0

/*
METHOD: EnableEffect - starts the actual routing of data through the effect
DATA(32): effect instance id
*/

#define NV_AUDGP_ENABLE_EFFECT                          0x00000007
#define NV_AUDGP_ENABLE_EFFECT_ID                       31:0

/*
METHOD: DisableEffect - disables data routed to the effect, but the effect still remains
configured as it was, till DeleteEffect is called
DATA(32): effect instance id
*/

#define NV_AUDGP_DISABLE_EFFECT                         0x00000008
#define NV_AUDGP_DISABLE_EFFECT_ID                      31:0

/*
METHOD: DeleteEffect - removes the effect, releases the Instance ID.  
To reset an effect, call DeleteEffect, followed by AddEffect.
DATA(32): effect instance id
*/

#define NV_AUDGP_DELETE_EFFECT                          0x00000009
#define NV_AUDGP_DELETE_EFFECT_ID                       31:0

/*
METHOD: DeRegisterEffect - frees the effect.  Puts the module ID back into the pool
for reuse.
DATA(32): Effect ID
*/

#define NV_AUDGP_DEREGISTER_EFFECT                      0x0000000A
#define NV_AUDGP_DEREGISTER_EFFECT_ID                   31:0

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
METHOD: SetParameterContextDma - sets the context DMA for the parameter memory 
for parameter updates for the current effect
DATA(32) : Context DMA handle
*/

#define NV_AUDGP_SET_CONTEXT_DMA_PARAMETER              0x0000000E
#define NV_AUDGP_SET_CONTEXT_DMA_PARAMETER_HANDLE       31:0


/*
METHOD: SetExecutionList - sets the order of execution for the effects modules
DATA(32) : size of packed array containing effect IDs in order of execution
DATA(32)*size : packed array containing effect IDs in order of execution
*/

#define NV_AUDGP_SET_EXECUTION_LIST                     0x0000000F
#define NV_AUDGP_SET_EXECUTION_LIST_PARAMETER_SIZE      31:0
#define NV_AUDGP_SET_EXECUTION_LIST_PARAMETER_ARRAY     31:0

/*
METHOD: ConfigureOutputFIFO(i) - configure the output fifo - do this before you point an
effect to it.
DATA(32) : Data format
            If the data transfers are isochronous
            Data size and container size where
            NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_8_8 =  8-bit data in 8-bit container(always in blocks of 4)
            NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_16_16 = 16-bit data in 16-bit container (always in blocks of two)
            NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_24_32_MSB =  24 bit data in 32-bit container (msb justified)
            NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_24_32_LSB = 24-bit data in 32-bit container (lsb justified)
            NV_AUDGP_CONFIGURE_OUTPUT_FIFO_SIZE_32_32 = 32 bit data in 32-bit container
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

/*
METHOD: ConfigureInputFIFO(i) - configure the input fifo
*/

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



/*
METHOD: SetContextDmaDspCode - sets context DMA for memory holding the DSP
        ucode for current effect.
DATA(32) : Context DMA handle
*/

#define NV_AUDGP_SET_CONTEXT_DMA_DSP_CODE               0x00000016
#define NV_AUDGP_SET_CONTEXT_DMA_DSP_CODE_HANDLE        31:0


/*
METHOD: ALlocScratchMemory - Used to allocate scratch memory
DATA(32) : Number of bytes to allocate
Return(32) : Offset into scratch memory
*/

#define NV_AUDGP_ALLOC_SCRATCH_MEMORY                   0x00000017
#define NV_AUDGP_ALLOC_SCRATCH_MEMORY_BYTES             31:0


/*
METHOD: FreeScratchMemory - Used to free allocated scratch memory
DATA(32) : Offset into scratch memory
DATA(32) : Number of bytes to free
*/

#define NV_AUDGP_FREE_SCRATCH_MEMORY                    0x00000018
#define NV_AUDGP_FREE_SCRATCH_MEMORY_OFFSET             31:0
#define NV_AUDGP_FREE_SCRATCH_MEMORY_SIZE               31:0

/*
METHOD: SetScratchMemory - Used to set scratch memory to a value
DATA(32) : Offset into scratch memory
DATA(32) : Number of DWORDS to set
DATA(32) : Value to set memory
*/

#define NV_AUDGP_SET_SCRATCH_MEMORY                     0x00000019
#define NV_AUDGP_SET_SCRATCH_MEMORY_OFFSET              31:0
#define NV_AUDGP_SET_SCRATCH_MEMORY_LENGTH              31:0
#define NV_AUDGP_SET_SCRATCH_MEMORY_VALUE               31:0


#endif

