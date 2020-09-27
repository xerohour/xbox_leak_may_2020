/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       device.hpp
 *  Content:    Contains all the device associated state.
 *
 ***************************************************************************/

//------------------------------------------------------------------------------
// Externally visible globals (see globals.cpp)

extern "C" 
{ 
    extern DWORD D3D__DeadlockTimeOutVal; 

    extern BOOL D3D__SingleStepPusher;

    extern DWORD D3D__DeadlockTimeOutVal;

    extern BOOL D3D__Parser; 

    extern volatile DWORD *D3D__GpuReg; 

    extern DWORD D3D__AvInfo;
}

//------------------------------------------------------------------------------
// Internally-only visible globals (see globals.cpp)

extern PPUSH g_StartPut;

extern DWORD g_NullHardwareGetPut[2];

// Enforce 16-byte alignment of SSE structure types:

#define D3DALIGN __declspec(align(16))

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{
 
class CDevice;

// This points to our active device structure.  In Xbox, there's only ever
// one...

extern CDevice* g_pDevice;

extern CDevice g_Device;

// Debug-only array for tracking loaded vertex shader programs:

extern BYTE g_LoadedVertexShaderSlot[];

// Debug-only variable for tracking if stencil was ever enabled:

extern BOOL g_StencilEverEnabled;

// Debug-only variable for tracking if caller was ever warned about Z-clears
// being faster if Stencil is cleared at the same time:

extern BOOL g_WarnedAboutStencil;

// Debug-only variable for tracking if caller was ever warned about 
// rops / clipping rects with swizzled textures.

extern BOOL g_WarnAboutCopyRectStateSwizzle;

// Handy prototypes:

BYTE* GetVisibilityAddress(DWORD Index);

VOID FastCopyToWC(PPUSH pPush, DWORD* pSource, DWORD dwCount);

// Make enough room in the push-buffer to hold more space:

PPUSH MakeSpace();

// Handy macros:

#define ARRAYSIZE(_a) (sizeof(_a) / sizeof((_a)[0]))

// The following macro reduces code by telling the compiler that the 'default'
// case in a switch statement should never be reached:

#if DBG
    #define NODEFAULT(Message) DXGRIP(Message)
#else 
    #define NODEFAULT(Message) __assume(0)
#endif

//------------------------------------------------------------------------------
// HWREG
//
// Handy struct declaration for direct access to hardware registers.

typedef union _HWREG
{
    volatile unsigned long Reg032[1];

} HWREG;

//------------------------------------------------------------------------------
// Canonical 'SetVertexDataArrayOffset' ordering, for the NV2A fixed-function
// pipeline:

#define SLOT_POSITION       0
#define SLOT_WEIGHT         1
#define SLOT_NORMAL         2
#define SLOT_DIFFUSE        3
#define SLOT_SPECULAR       4
#define SLOT_FOG            5
#define SLOT_POINT_SIZE     6
#define SLOT_BACK_DIFFUSE   7
#define SLOT_BACK_SPECULAR  8
#define SLOT_TEXTURE0       9
#define SLOT_TEXTURE1       10
#define SLOT_TEXTURE2       11
#define SLOT_TEXTURE3       12
#define SLOT_RESERVED1      13
#define SLOT_RESERVED2      14
#define SLOT_RESERVED3      15

//------------------------------------------------------------------------------
// Short time waster to help us avoid constant querying of the hardware when
// spinning.

VOID BusyLoop();

//------------------------------------------------------------------------------
// Maximum number of swaps that are permitted to be enqueued in the
// push-buffer at once (must be a power of 2):

#define SWAP_THROTTLE_QUEUE_SIZE 2

//------------------------------------------------------------------------------
// Stream

struct Stream
{
    // Stream stride, as specified by SetStreamSource:

    DWORD Stride;

    // Stream offset, as specified by SetStreamOffset:

    DWORD Offset;

    // The associated vertex buffer:

    D3DVertexBuffer* pVertexBuffer;
};

// g_Stream keeps track of all the current selected streams for the device.

extern Stream g_Stream[16];

//------------------------------------------------------------------------------
// VertexShaderSlot
//
// DX8 and the NV2A have 16 register 'slots' usable by the vertex shader
// program.  This structure describes where to find the associated data
// and how to interpret it.

struct VertexShaderSlot
{
    // Our data comes from the following stream:

    DWORD StreamIndex;

    // Byte offset from the start of the stream.  Note that the stream
    // has the actual memory start and stride.

    DWORD Offset;

    // Type and Size, pre-encoded for the SetVertexDataArrayFormat 
    // register, telling us the slot's dimensionality, type (e.g., 
    // 'FLOAT'), and whether it's enabled:

    DWORD SizeAndType;

    // Type of autocalc : 0=none, 1=normal, 2=autotex

    BYTE Flags;

    // Source register for a normal/uv autocalc

    BYTE Source;
};

//------------------------------------------------------------------------------
// VERTEXSHADER flags

// The program can write to the constant registers (has to be '1' so that
// we can just copy to the hardware register after doing an AND):

#define VERTEXSHADER_WRITE 0x1 // Must be 1

// The vertex shader is an XYZRHW passthrough FVF (meaning no hardware T&L
// is used), which we do using a special vertex shader program:

#define VERTEXSHADER_PASSTHROUGH 0x2 

// This is a vertex state shader:

#define VERTEXSHADER_STATE 0x8

// The vertex shader has a program that we have to load:

#define VERTEXSHADER_PROGRAM 0x10 // Must be 16

// The following flags indicate when the vertex shader has an attribute
// of the specified type (either persistent or from the data stream).
//
// These flags must match the values for the corresponding D3DUSAGE_ defines.

#define VERTEXSHADER_HASDIFFUSE 0x400 

#define VERTEXSHADER_HASSPECULAR 0x800 

#define VERTEXSHADER_HASBACKDIFFUSE 0x1000

#define VERTEXSHADER_HASBACKSPECULAR 0x2000

// On Kelvin, a disabled vertex slot has to have a type of 'float' and
// size of '0':

#define SIZEANDTYPE_DISABLED 0x2

// Total number of slots that can be assigned in a vertex shader declaration:

#define VERTEXSHADER_SLOTS 16
                   
//------------------------------------------------------------------------------
// VertexShader
//
// Encapsulates all the data needed to describe a vertex shader program.
                   
struct VertexShader
{
#if DBG

    // This contains the dword value 'Vshd' if the object is valid.

    DWORD Signature;

#endif

    // Reference count:

    DWORD RefCount;

    // VERTEXSHADER_ flags:

    DWORD Flags;

    // Size, in slots, of a programmed vertex shader program (unused for
    // pass-through or FVF shaders):

    DWORD ProgramSize;

    // Size, in dwords, of the push-buffer snippet that loads the vertex 
    // shader function (constants and program data):

    DWORD ProgramAndConstantsDwords;

    // Dimensionality of every texture coordinate specified (applies
    // only to the fixed-function pipeline).  Every byte contains the
    // corresponding dimension (zero if there is no associated texture
    // coordinate):

    DWORD Dimensionality;

    // How to find and interpret the data for each of the vertex shader
    // slots.

    VertexShaderSlot Slot[VERTEXSHADER_SLOTS];

    // Push-buffer snippet containing the push-buffer commands for
    // loading the vertex shader function (constant and program data):

    DWORD ProgramAndConstants[1];
};

// Vertex shader storage used whenever an FVF is specified as the current
// vertex shader program:

extern VertexShader g_FvfVertexShader;

// Vertex shader storage used whenever a struct is used to specify the 
// vertex attribute format:

extern VertexShader g_DirectVertexShader;

//------------------------------------------------------------------------------
// g_UnitsOfElement
//
// Converts from a Kelvin SetVertexDataArrayFormat.Size value to a count
// of units:

D3DCONST BYTE g_UnitsOfElement[] = { 0, 1, 2, 3, 4, 0, 0, 3 };

//------------------------------------------------------------------------------
// g_BytesPerUnit
//
// Converts from a Kelvin SetVertexDataArrayFormat.Type value to a count
// of bytes-per-unit:

D3DCONST BYTE g_BytesPerUnit[] = { 1, 2, 4, 0, 1, 2, 4 };

//------------------------------------------------------------------------------
// IsVertexProgram
//
// Returns TRUE if the vertex shader handle is for a vertex program; FALSE
// if the handle is actually an FVF.

FORCEINLINE BOOL IsVertexProgram(
    DWORD vertexShaderHandle)
{
    return vertexShaderHandle & D3DFVF_RESERVED0;
}

//------------------------------------------------------------------------------
// CheckVertexAttributeFormatStruct prototype
//
// Checks to make sure that valid formats have been provided in the 
// VertexAttributeFormat Struct and also that no formats overlap in a stream.

#if DBG
void CheckVertexAttributeFormatStruct(D3DVERTEXATTRIBUTEFORMAT *pVAF);
#endif

//------------------------------------------------------------------------------
// RestoreVertexShaders prototype

VOID RestoreVertexShaders(
    CONST DWORD* pProgram,
    DWORD Dwords);

//------------------------------------------------------------------------------
// ST
//
// Handy little macro for initializing the SetVertexDataArrayFormat field.

#define ST(Size, Type) \
    (((NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_##Size) << 4) | \
      (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_##Type))
                   
//------------------------------------------------------------------------------
// g_MinFilter

D3DCONST DWORD g_MinFilter[2][3] =
{
    // Min == POINT:

    DRF_DEF(097, _SET_TEXTURE_FILTER, _MIN, _BOX_LOD0),        // Mip == NONE
    DRF_DEF(097, _SET_TEXTURE_FILTER, _MIN, _BOX_NEARESTLOD),  // Mip == POINT
    DRF_DEF(097, _SET_TEXTURE_FILTER, _MIN, _BOX_TENT_LOD),    // Mip == LINEAR

    // Min == LINEAR:

    DRF_DEF(097, _SET_TEXTURE_FILTER, _MIN, _TENT_LOD0),       // Mip == NONE
    DRF_DEF(097, _SET_TEXTURE_FILTER, _MIN, _TENT_NEARESTLOD), // Mip == POINT
    DRF_DEF(097, _SET_TEXTURE_FILTER, _MIN, _TENT_TENT_LOD),   // Mip == LINEAR
};

//------------------------------------------------------------------------------
// MinFilter 

FORCEINLINE DWORD MinFilter(
    DWORD MinFilter,
    DWORD MipFilter)
{
    ASSERT((MinFilter <= D3DTEXF_LINEAR) && (MipFilter <= D3DTEXF_LINEAR));
    ASSERT(MinFilter > D3DTEXF_NONE);

    return g_MinFilter[MinFilter - 1][MipFilter];
}

//------------------------------------------------------------------------------
// PixelShader
//
// Encapsulates all the data needed to describe a pixel shader program.
                   
struct PixelShader
{
#if DBG

    // This contains the dword value 'Pshd' if the object is valid.

    DWORD Signature;

#endif

    // Reference count:

    DWORD RefCount;

    // Flag indicating whether D3D owns the pPSDef memory
    DWORD D3DOwned;

    // A copy of the pixel shader definition

    D3DPIXELSHADERDEF *pPSDef;
};

//------------------------------------------------------------------------------
// Light flags

// The light at this index position has been 'Set' via SetLight at any point
// in time:

#define LIGHT_SET 1

// This light was set since the last BeginStateBlock call:

#define LIGHT_STATEBLOCK_SET 2

// This light was enabled or disabled since the last BeginStateBlock call:

#define LIGHT_STATEBLOCK_ENABLED 4

//------------------------------------------------------------------------------
// Light structure

struct Light
{
    // Light parameters as specified to the API (unmodified);

    D3DLIGHT8 Light8;

    // LIGHT_* flags:

    DWORD Flags;

    // Direction, negated and normalized as for OpenGL:

    D3DVECTOR Direction;        

    // Spotlight falloff parameters:

    FLOAT Falloff_L;
    FLOAT Falloff_M;
    FLOAT Falloff_N;

    // Spotlight scale factor:

    FLOAT Scale;

    // Spotlight direction W:

    FLOAT W;

    // Next light on the active list, from newest to oldest:

    Light* pNext;
};

//------------------------------------------------------------------------------
// InlineAttributeData
//
// Structure describing each inline vertex attribute, for use by UP 
// DrawPrim calls.

struct InlineAttributeData
{
    // The count of dwords for this attribute:

    DWORD UP_Count;

    // The byte stride from the end of this attribute to the start
    // of the next, in the source vertex:

    DWORD UP_Delta;
};

//------------------------------------------------------------------------------
// D3D__DirtyFlags

// The following 'DIRECT' flags indicate that the direct (as opposed
// to the lazily evaluated) versions of these APIs were most recently
// invoked:

#define D3DDIRTYFLAG_DIRECT_MODELVIEW   0x80000000
#define D3DDIRTYFLAG_DIRECT_INPUT       0x40000000

#define D3DDIRTYFLAG_REALLY_FILTHY      (D3DDIRTYFLAG_POINTPARAMS          | \
                                         D3DDIRTYFLAG_TRANSFORM            | \
                                         D3DDIRTYFLAG_TEXTURE_TRANSFORM    | \
                                         D3DDIRTYFLAG_COMBINERS            | \
                                         D3DDIRTYFLAG_TEXTURE_STATE        | \
                                         D3DDIRTYFLAG_LIGHTS               | \
                                         D3DDIRTYFLAG_SPECFOG_COMBINER     | \
                                         D3DDIRTYFLAG_VERTEXFORMAT_VB      | \
                                         D3DDIRTYFLAG_VERTEXFORMAT_UP      | \
                                         D3DDIRTYFLAG_VERTEXFORMAT_OFFSETS | \
                                         D3DDIRTYFLAG_SHADER_STAGE_PROGRAM)

//------------------------------------------------------------------------------
// m_StateFlags

// The hardware currently has DXT noise enabled:

#define STATE_DXTNOISEENABLE 0x1 // Must be one

// Set if the current projection transform dictates that the fog source is
// Z (as opposed to W):

#define STATE_FOGSOURCEZ 0x2

// We're currently recording a push-buffer via BeginPushBuffer:

#define STATE_RECORDPUSHBUFFER 0x4

// The game has defined the D3DCREATE_PUREDEVICE compile-time flag.  In 
// this case, we don't need to remember any state for the purposes of 
// GetRenderState/GetTextureStageState or state blocks.

#define STATE_PUREDEVICE 0x10 // Same as D3DCREATE_PUREDEVICE

// We're currently recording a state block via BeginStateBlock:

#define STATE_RECORDBLOCK 0x20

// Combiner needs specular:

#define STATE_COMBINERNEEDSSPECULAR 0x40

// PushBufferGetOffset was called on the currently recording push-buffer,
// and was already padded with NOPS:

#define STATE_RECORDPUSHBUFFERPADDED 0x80

// The next time a SetRenderTarget is done back to the active back-buffer,
// we'll have to do a FLIPSTALL:

#define STATE_FLIPSTALLPENDING 0x100

// Don't bother setting the magic -38 and -37 registers for vertex shaders
// (they promise that all their vertex shaders will be #pragma screenspace):

#define STATE_NORESERVEDCONSTANTS 0x200

// Set whenever RunPushBuffer is called, on debug builds.  Use only for
// SetDmaRange.

#define STATE_RUNPUSHBUFFERWASCALLED 0x400

// We're in a GPU Begin/End bracket and sending vertices to the GPU:

#define STATE_BEGINENDBRACKET 0x800

// A segment fence should be written as soon as we get out of the Begin/End
// bracket:

#define STATE_SEGMENTFENCEPENDING 0x1000

// We've done all the initialization required to emulate inifinitely fast
// hardware:

#define STATE_NULLHARDWARE 0x2000

// Do the Swap using a copy-blt, either because they specified
// D3DSWAPEFFECT_COPY or because they're in an antialiased mode:

#define STATE_COPYSWAP 0x4000

// The hardware is currently in a multisampling mode:

#define STATE_MULTISAMPLING 0x8000

//------------------------------------------------------------------------------
// Flags for decoding the D3DRS_MULTISAMPLETYPE renderstate, which represents
// our current full-screen antialiasing control

#define ANTIALIAS_SAMPLE_MASK     0x03000
#define ANTIALIAS_MULTISAMPLE     0x01000       // Flag
#define ANTIALIAS_SUPERSAMPLE     0x02000       // Flag

#define ANTIALIAS_LINEAR          0x00000
#define ANTIALIAS_QUINCUNX        0x00100       // Flag
#define ANTIALIAS_GAUSSIAN        0x00200       // Flag

#define ANTIALIAS_XSCALE_MASK     0x00f0
#define ANTIALIAS_XSCALE_SHIFT    4

#define ANTIALIAS_YSCALE_MASK     0x000f
#define ANTIALIAS_YSCALE_SHIFT    0

#define ANTIALIAS_SCALE_MASK      0x000ff
#define ANTIALIAS_TYPE_MASK       0x0ffff

#define ANTIALIAS_FORMAT_MASK     0xf0000

// If any bit in 'ANTIALIAS_ACTIVE' is set, then we're doing antialiasing:

#define ANTIALIAS_ACTIVE ANTIALIAS_SAMPLE_MASK

// Default value when aliasing (sets stretch factors to 1):

#define ANTIALIAS_NONE 0x00011

// Decode the stretch factor:

#define ANTIALIAS_XSCALE(antiAliasType) \
    ((antiAliasType & ANTIALIAS_XSCALE_MASK) >> ANTIALIAS_XSCALE_SHIFT)
    
#define ANTIALIAS_YSCALE(antiAliasType) \
    ((antiAliasType & ANTIALIAS_YSCALE_MASK) >> ANTIALIAS_YSCALE_SHIFT)

//------------------------------------------------------------------------------
// Visibility test reporting stuff

// The memory manager gives us pages with no extra hidden fields, so we can
// use the whole page:

#define REPORTS_ALLOCATION_SIZE 4096

// The hardware writes 16 bytes for every report:

#define REPORTS_SIZE 16

// The hardware writes 16 bytes for every report, so a single page can
// hold 256 reports:

#define REPORTS_PER_ALLOCATION (REPORTS_ALLOCATION_SIZE / REPORTS_SIZE)

// We always allocate a page at a time, so this is the number of allocations
// we'd maximally have to do:

#define REPORTS_ALLOCATIONS_MAX (D3DVISIBILITY_TEST_MAX / REPORTS_PER_ALLOCATION)

// Reserved value which we use to indicate that the visibility 
// test is still incomplete:

#define REPORTS_INCOMPLETE_MARKER 0xffffffff

// Reserved value which we use on debug to tell if this index
// location didn't actually have an EndVisibilityTest done on it:

#define REPORTS_UNINITIALIZED_MARKER 0xfffffffe

// The Offset field of NV097_GET_REPORT is restricted to 24 bits.  For
// simplicity we use our standard DMA mapping for reports and so restrict
// the allocation area to the first 16 MBs:

#define REPORTS_MAXIMUM_ADDRESS (1 << 24)

//------------------------------------------------------------------------------
// CDevice                                                                
//
// Base object for the D3DDevice object

class CDevice : public D3DDevice
{
public:
    // Embedded push buffer structure.  This _must_ be the first variable in 
    // the device or the internal version of StartPush/EndPush will have to do
    // an extra add.
    // 
    // This structure currently only holds the members that need to be exposed
    // outside of the driver or need to be used directly in StartPush/EndPush.

    XMETAL_PushBuffer m_Pusher;

    //--------------------------------------------------------------------------
    // State stuff

    // State flags (STATE_*) that reflect current state: 

    DWORD m_StateFlags;

    // Contains the NV097_SET_TEXTURE_FORMAT style format for the currently
    // selected texture - but only the dimensionality and cubemap-enable 
    // fields are actually valid.  And this will be ~0 if the texture unit
    // is currently disabled.

    DWORD m_TextureCubemapAndDimension[D3DTSS_MAXSTAGES];

    // Current base vertex index as set by SetIndices:

    DWORD m_IndexBase;

    // Index base value for which we last set the vertex format offset 
    // registers (compare and contrast with 'm_IndexBase'):

    DWORD m_CachedIndexBase;

    //--------------------------------------------------------------------------
    // Pusher stuff

    // The size of the push buffer, in bytes. 

    static DWORD m_PushBufferSize;

    // The size of a push buffer segment, in bytes.

    static DWORD m_PushSegmentSize;

    // Points to the start of memory for the push buffer:

    PPUSH m_pPushBase;

    // Points to the first dword following the end of the push buffer:

    PPUSH m_pPushLimit;

    // This is the last 'm_pPut' value written to the hardware via
    // KickOff:

    PPUSH m_pKickOff;

    // The time which any data currently being written into the push
    // buffer will have been processed and it no longer needed by
    // the GPU.  This will get pushed into the segment after it ends.

    DWORD m_CpuTime;

    // Points to the push-buffer 'Time' at which the GPU is currently 
    // processing push-buffer data.  All resources with a time equal to or 
    // less than this value are no longer being used by the GPU.

    volatile DWORD* m_pGpuTime;

    // Index into m_PusherSegment array where the most recent segment's 
    // data was placed:

    DWORD m_PusherLastSegment;

    // AND this value with m_PusherLastSegment to find the segment's
    // index into the m_PusherSegment array:

    DWORD m_PusherSegmentMask;

    // Total number of bytes of RunPushBuffer calls ever inserted into
    // the main push-buffer (does not include any push-buffers called
    // from within a push-buffer). This is the total size at the time 
    // that we put the Run instruction into the push-buffer; 
    // m_Miniport.m_PusherGetRunTotal is the total size at the time that 
    // the Run instruction is actually executed by the GPU.

    DWORD m_PusherPutRunTotal;

    // The completed size of the push-buffer when we last had to wrap back
    // to the beginning:

    DWORD m_PusherLastSize;

    // Time at which the last RunPushBuffer was done, so that we know if
    // any RunPushBuffer commands are pending by the GPU or not:

    DWORD m_LastRunPushBufferTime;

    // Pointer to an array containing the regular-interval segment fences:

    Fence* m_PusherSegment;

    // An array containing the most recent fences, including both segment 
    // fences and inserted fences:

    Fence m_PusherFence[PUSHER_FENCE_COUNT];

    //--------------------------------------------------------------------------
    // Push-buffer record stuff:

    // The push-buffer object into which we're recording:

    D3DPushBuffer* m_pPushBufferRecordResource;

    // When the buffer is too small, we still want to be able to tell the
    // caller the size of the buffer that would have been needed.  This
    // value accounts for any 'wraps' of the push-buffer (where the size
    // needed is this plus the current pointer minus the start pointer).
    //
    // A non-zero value means that the buffer was too small (and so the
    // contents are invalid):

    DWORD m_PushBufferRecordWrapSize;

    // When recording a push-buffer, we hijack the 'm_pThreshold' pointer
    // and hold the old value here:

    PPUSH m_pPushBufferRecordSavedThreshold;

    // When recording a push-buffer, we hijack the 'm_pPut' pointer
    // and hold the old value here.  So this is the 'm_pPut' value at the
    // time that BeginPushBuffer was called:

    PPUSH m_pPushBufferRecordSavedPut;

    //--------------------------------------------------------------------------
    // Miscellaneous stuff

    // Hardware encoded register value that enables this texture stage:

    DWORD m_TextureControl0Enabled[D3DTSS_MAXSTAGES];

    // Pointer to the current pixel shader program (may be NULL):

    PixelShader* m_pPixelShader;

    // Indicates whether the currently installed pixel shader
    // uses the final combiner.  Non-zero if it does.  Only valid 
    // if a pixel shader is set.

    DWORD m_ShaderUsesSpecFog;

    // Indicates whether the currently installed pixel shader
    // wants the texture modes adjusted according to the textures
    // set in each stage.

    DWORD m_ShaderAdjustsTexMode;

    // Used to shadow the actual shader program that we send to the 
    // hardware.  This may be different than the shader program 
    // specified by the app because we have to change it based on 
    // the texture types currently set.

    DWORD m_PSShaderStageProgram;

    // Pointer to the current vertex shader program (will point to
    // g_FvfVertexShader if a FVF vertex type is current).  This
    // value is never NULL:

    VertexShader* m_pVertexShader;

    // Vertex shader handle, as passed in to SetVertexShader:

    DWORD m_VertexShaderHandle;

    // Vertex shader start address valid only if a vertex shader
    // is active.

    DWORD m_VertexShaderStart;

    // Current index buffer, as set by SetIndices:

    D3DIndexBuffer* m_pIndexBuffer;

    // Pointer to the allocated lights array:

    Light* m_pLights;

    // Count of elements in 'm_pLights' array:

    DWORD m_LightCount;

    // Linked list of active lights, in order of most recently activated
    // to least recently:

    Light* m_pActiveLights;

    // Array of pointers to our individual report allocations:

    VOID* m_ReportAllocations[REPORTS_ALLOCATIONS_MAX];

    // The next four member variables are used for pixel shader capture
    // The opcode is used to handshake between D3D and DM

    DWORD m_dwOpcode;
    
    // Indicates whether D3D is in capture mode and what the mode is

    DWORD m_dwSnapshot;
    
    // A pointer to a buffer to hold the capture data for a frame

    DWORD *m_pShaderCaptureBuffer;
    
    // The current write pointer into the capture buffer

    DWORD *m_pShaderCapturePtr;

    // Cache for D3D constants specified via SetPixelShaderConstant.
    // This storage isn't needed if we are a pure device

    DWORD m_pPixelShaderConstants[16];

    // pixelshader struct for use with user pixel shaders

    PixelShader m_UserPixelShader;

    // Holds the current debug marker set by the user

    DWORD m_dwDebugMarker;

    // Pointer to the base of the hardware registers.

    HWREG* m_NvBase;

    // Reference count for this object.

    DWORD m_cRefs;

    //--------------------------------------------------------------------------
    // Cached state

    // Near W, as computed from the perspective transform:

    FLOAT m_WNear;

    // Far W, as computed from the perspective transform:

    FLOAT m_WFar;

    // 1.0f / m_WFar:

    FLOAT m_InverseWFar;

    // Z scale, as based on the Z buffer depth, either 2^16 - 1 or
    // 2^24 - 1:

    FLOAT m_ZScale;

    // A bit array for every stage that indicates whether that stage has
    // a tex-gen mode set that requires the inverse modelview transform:

    DWORD m_TexGenInverseNeeded;

    // Supersampling 'amplifying' factors.  Note that we can have different 
    // scaling factors in X and in Y, so we also have to have a separate
    // scalar factor.
    //
    // Any of these values may be fractional.
    //
    // NOTE: These values are valid for the currently selected render target 
    //       only.

    FLOAT m_SuperSampleScaleX;
    FLOAT m_SuperSampleScaleY;
    FLOAT m_SuperSampleScale;
    FLOAT m_SuperSampleLODBias;

    // Antialiasing 'amplifying' factors for the back-buffer.  Whenever the
    // back-buffer is the current render target, m_SuperSampleScale* values
    // will be set to these:

    FLOAT m_AntiAliasScaleX;
    FLOAT m_AntiAliasScaleY;

    // Pre-computed composite transformation which is the combination of 
    // the projection transform, the viewport, the antialiasing state, and
    // the w-buffering state:

    D3DALIGN D3DMATRIX m_ProjectionViewportTransform;

    // The model-view transforms as set by Set[VertexBlend]ModelView:

    D3DMATRIX m_ModelViewTransform[4];
    D3DMATRIX m_ProjectionViewport;

    // The vertex shader handle as specified to SetVertexShaderInput:

    DWORD m_VertexShaderInputHandle;

    // The number of streams specified to SetVertexShaderInput:

    DWORD m_VertexShaderInputCount;

    // The stream data as specified to SetVertexShaderInput:

    D3DSTREAM_INPUT m_VertexShaderInputStream[16];

    //--------------------------------------------------------------------------
    // Inline vertex cached state
    //
    // Note that the following inline fields, for UP rendering, are cached
    // values valid only when the D3DDIRTYFLAG_VERTEXFORMAT_UP is not set.
    
    // This is the total count of dwords each vertex represents:

    DWORD m_InlineVertexDwords;

    // Start offset, in bytes, from the start of the vertex to the first
    // attribute that has to be sent via the UP inline rendering code:

    DWORD m_InlineStartOffset;

    // Offset, in bytes, from the end of the last attribute to the
    // start of the first attribute:

    DWORD m_InlineDelta;

    // This array describes each attribute of the current vertex shader
    // in a way that can best be chewed up by the UP inline rendering
    // functions:

    InlineAttributeData m_InlineAttributeData[16];

    // Number of attributes for the current vertex shader.  Note that 
    // attributes can be appended together, so this isn't a *true*
    // count of attributes, but is rather the count of elements in the
    // m_InlineAttributeData array.

    DWORD m_InlineAttributeCount;

    //--------------------------------------------------------------------------
    // API state we have to keep

    // Straight copy of whatever matrix was given to us at SetTransform
    // time, indexed using a D3DTS_ value:

    D3DALIGN D3DMATRIX m_Transform[D3DTS_MAX];

    // Current viewport, as set by SetViewport:

    D3DVIEWPORT8 m_Viewport;

    // Current screen space offset, as set by SetScreenSpaceOffset:

    FLOAT m_ScreenSpaceOffsetX;
    FLOAT m_ScreenSpaceOffsetY;

    // Current material as set by SetMaterial:

    D3DMATERIAL8 m_Material;

    // Current material as set by SetBackMaterial:

    D3DMATERIAL8 m_BackMaterial;

    // Current textures, as set by SetTexture:

    D3DBaseTexture *m_Textures[D3DTSS_MAXSTAGES];

    // Corresponding palettes for the textures.

    D3DPalette *m_Palettes[D3DTSS_MAXSTAGES];

    // Current pixel shader constants, as set by SetPixelShaderConstant:

    FLOAT m_PixelShaderConstants[D3DPS_CONSTREG_MAX_DX8][4];

    // Current vertex shader constants, as set by SetVertexShaderConstant:

    FLOAT m_VertexShaderConstants[D3DVS_CONSTREG_COUNT_XBOX][4];

    // Currently loaded vertex shader programs, as set by LoadVertexShader:

    DWORD m_VertexShaderProgramSlots[VSHADER_PROGRAM_SLOTS][4];

    // Constant mode as set by SetShaderConstantMode:

    D3DSHADERCONSTANTMODE m_ConstantMode;

    // Current color format set by SetCopyRectsState and used by CopyRects

    D3DCOPYRECTSTATE m_CopyRectState;
    D3DCOPYRECTROPSTATE m_CopyRectRopState;

    // Original MultiSampleType as passed in at creation/Reset:

    DWORD m_MultiSampleType;

    // Original SwapEffect as passed in at creation/Reset.  
    //
    // NOTE: 0 and D3DSWAPEFFECT_FLIP (2) should be treated equivalently.

    DWORD m_SwapEffect;

    // Swap type:

    DWORD m_PresentationInterval;

    // Keeps track of the times of the last swaps, for swap throttling
    // purposes:

    DWORD m_SwapTime[SWAP_THROTTLE_QUEUE_SIZE];

    //--------------------------------------------------------------------------
    // Screen information

    // Current render target.  This is never NULL (except during 
    // initialization).

    D3DSurface* m_pRenderTarget;

    // Current Z buffer surface.  This may be NULL.

    D3DSurface* m_pZBuffer;

    // The total number of frame buffers, including the front buffer (i.e., 
    // 2 if double buffering, 3 if triple buffering):

    DWORD m_FrameBufferCount;

    // Array of surfaces that point to each frame buffer.  The buffer that
    // is currently visible (that the DAC is scanning from) is at index 1, and 
    // the buffer currently being rendering to is at index 0.
    //
    // Note that with antialiasing, these buffers are not necessarily all the
    // same dimension or format.

    D3DSurface* m_pFrameBuffer[3];

    // Points to the auto-supplied Z buffer.  NULL if none was created or
    // supplied:

    D3DSurface* m_pAutoDepthBuffer;

    // Storage space for the D3D-allocated frame buffer structures, or a
    // copy if the title specified their own via the D3DPRESENT_PARAMETERS
    // BufferSurfaces[] field.

    D3DSurface m_BufferSurfaces[3];

    // Storage space for the D3D-allocate Z buffer structure, or a copy
    // if the title specified their own via the D3DPRESENT_PARAMETERS
    // DepthStencilSurface field.

    D3DSurface m_DepthStencilSurface;    
    
    // Start address for the D3D-allocated frame buffer (virtual address, used 
    // for freeing, NULL if title supplied its own):

    VOID* m_pFrameBufferBase;

    // Start address for the D3D-allocated antialias buffers (virtual address, 
    // used for freeing, NULL if title supplied its own):

    VOID* m_pAntiAliasBufferBase;

    // Start address for the D3D-allocated depth/stencil buffer (virtual address,
    // used for freeing, NULL if title supplied its own):

    VOID* m_pAutoDepthBufferBase;

    // Storage area for saving the original surfaces at the time of a Swap
    // call:

    D3DSurface* m_pSaveRenderTarget;
    D3DSurface* m_pSaveZBuffer;
    D3DBaseTexture* m_pSaveTexture;
    D3DVIEWPORT8 m_SaveViewport;

    // Maintains the status of all currently used tile regions:

    D3DTILE m_Tile[D3DTILE_MAXTILES];

    // The current scissors state:

    D3DRECT m_ScissorsRects[8];

    DWORD m_ScissorsCount;

    BOOL m_ScissorsExclusive;

    //--------------------------------------------------------------------------
    // Miniport stuff

    // Pointer to the hardware push buffer registers:

    Nv20ControlDma* m_pControlDma;

    // Miniport state

    CMiniport m_Miniport;

    // Number of swaps done:

    ULONG m_SwapCount;

    // Context DMA instances, used for modifying DMA contexts on-the-fly
    // for debug-build checking:

    DWORD m_ColorContextDmaInstance;
    DWORD m_ZetaContextDmaInstance;
    DWORD m_CopyContextDmaInstance;

    // Base for all of the cached contiguous memory allocated when the
    // device is initialized, not counting the frame buffers.

    BYTE* m_pCachedContiguousMemoryBase;

    // Notifiers for the memory copy class.
    
    volatile NvNotification* m_pMemCopyNotifiers;

    // Kelvin notifiers.

    volatile NvNotification* m_pKelvinNotifiers;

    //--------------------------------------------------------------------------
    // Methods

    // CDevice initializer:

    HRESULT Init(D3DPRESENT_PARAMETERS* pPresentationParameters);

    // Cleans up:

    VOID UnInit();

    // Creates the frame and back buffers.

    HRESULT InitializeFrameBuffers(D3DPRESENT_PARAMETERS* pPresentationParameters);

    // Frees the frame and back buffers.

    VOID FreeFrameBuffers();

    // Reset any dirty hardware state, for use by UP DrawPrim calls:

    VOID SetStateUP();

    // Reset any dirty hardware state, for use by vertex-buffer DrawPrim calls:

    VOID SetStateVB(DWORD IndexBase);

    //--------------------------------------------------------------------------
    // Pusher stuff

    // Use 'StartPush' if you're not sure at call time of the exact number
    // of dwords you'll be sticking into the buffer.  Give 'StartPush' an
    // upper bound of the count of dwords, and then call 'EndPush' with
    // the final pointer when you're done.
    //
    // Use StartPush() with no arguments to default to ensuring there
    // are at least PUSHER_THRESHOLD_SIZE = 128 dwords.
    //
    // Don't forget to call 'EndPush' when done.

    __forceinline PPUSH StartPush() 
    { 
        // We won't normally need to call 'MakeSpace':

        PPUSH pStart = m_Pusher.m_pPut;

        if (pStart >= m_Pusher.m_pThreshold)
            pStart = MakeSpace();

        DbgRecordPushStart(PUSHER_THRESHOLD_SIZE);

        return pStart;
    }

    __forceinline PPUSH StartPush(DWORD count) 
    { 
        return XMETAL_StartPushCount(&m_Pusher, count); 
    } 

    // 'EndPush' is the companion to 'StartPush', and its parameter is
    // the pointer one past the last dword added:
    
    __forceinline VOID EndPush(PPUSH pPush) 
    { 
        XMETAL_EndPush(&m_Pusher, pPush); 
    }

    // Suppress inserting any fences while we're in a GPU Begin/End bracket
    // (because the hardware falls over if we try):

    __forceinline VOID StartBracket()
    {
        DBG_CHECK(g_StartPut = m_Pusher.m_pPut);
        ASSERT(!(m_StateFlags & STATE_BEGINENDBRACKET));

        m_StateFlags |= STATE_BEGINENDBRACKET;
    }

    // Stop suppressing any fences, and insert an interval fence if one was
    // pending:

    __forceinline VOID EndBracket()
    {
    #if DBG
        DWORD distance = (DWORD) m_Pusher.m_pPut - (DWORD) g_StartPut;
        if ((INT) distance < 0)
            distance += m_PusherLastSize;
        if (distance > m_PushBufferSize / 2)
        {
            WARNING("Vertex or index data for this call exceeded half of the push-buffer size.\n"
                    "Reduce the call size or increase the push-buffer size with SetPushBufferSize.\n");
        }
    #endif
    
        ASSERT(m_StateFlags & STATE_BEGINENDBRACKET);
        if (m_StateFlags & STATE_SEGMENTFENCEPENDING)
        {
            SetFence(SETFENCE_SEGMENT);
        }
        m_StateFlags &= ~(STATE_SEGMENTFENCEPENDING | STATE_BEGINENDBRACKET);
    }

    // Update the hardware's 'put' address.  This lets the GPU start
    // processing whatever's been written into the push-buffer:

    VOID KickOff();

    // Set up the push buffer.

    HRESULT InitializePushBuffer();
    VOID UninitializePushBuffer();

    // Let the hardware know the latest 'put' address.
    
    VOID HwPut(PPUSH pPut) 
    { 
        WRITE_REGISTER(&m_pControlDma->Put, GetGPUAddress((void*) pPut)); 
    }

    // Read the hardware's current 'Get' pointer and shadow the value.
    //
    // NOTE: This register read is actually quite expensive - it affects the
    //       chip's internal pipeline!  
    //
    // NOTE: This returns a weird address if the GPU is currently executing
    //       a static push-buffer!

    PPUSH HwGet() 
    { 
        // The GetWriteCombinedAddress translates the physical memory
        // offset returned by m_pControlDma->Get into a write-
        // combined mapped pointer into the push buffer.  

        return (PPUSH)
            GetWriteCombinedAddress(READ_REGISTER(&m_pControlDma->Get));
    }

    // Returns the current GPU location in the main push-buffer.  
    //
    // NOTE: This returns a value that is always in the main push-buffer, 
    //       even if the GPU is currently executing a static push-buffer.

    PPUSH GpuGet()
    {
        PPUSH pGet = HwGet();
    
        // If the GPU is not currently pulling from the primary push-buffer,
        // then it must currently be executing a push-buffer called via
        // RunPushBuffer.  In that case, get the return address:
    
        if ((pGet < m_pPushBase) || (pGet >= m_pPushLimit))
        {
            // PUSHER_CALL is used every time a push-buffer resource is
            // called from the main push-buffer, so we can simply read
            // the following register to determine our location in the
            // main push-buffer.  (Note that nested push-buffers do not
            // use PUSHER_CALL, so we are guaranteed this address will
            // always be in the main push-buffer):

            pGet = (PPUSH) GetWriteCombinedAddress(
                REG_RD32(m_NvBase, NV_PFIFO_CACHE1_DMA_SUBROUTINE) 
                    & ~NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE_ACTIVE);

            ASSERT((pGet >= m_pPushBase) && (pGet <= m_pPushLimit));
        }
    
        return pGet;
    }

    // Returns the current GPU time: 

    __forceinline DWORD GpuTime()
    {
        return *m_pGpuTime;
    }

    // Because of potential DWORD time overflow, we can't simply 
    // compare two time values (e.g., 0x6 is 'newer' than 0x3, but 
    // 0xfffffffd may be 'older' than 0x3).  Fortunately, it's safe 
    // to compare 'ages', which are computed relative to the current 
    // time.
    //
    // 'Age' may also be thought of as how many fences ago the 
    // corresponding fence was written (if you shift the result down
    // by PUSHER_TIME_SHIFT).

    DWORD Age(DWORD Time)
    {
        return m_CpuTime - Time;
    }

    // Figure out if a time represents data still in the push buffer.
    // If 'true' is returned, then the GPU isn't yet finished with the
    // object.

    __forceinline BOOL IsTimePending(DWORD Time)
    {   
        ASSERT(Time != 0);

        return Age(Time) < Age(GpuTime());
    }

    // Record where an object is pushed into the push buffer so we know when
    // it can be locked.

    __forceinline VOID RecordResourceReadPush(IDirect3DResource8 *pResource)
    {
        ASSERT(!IsChildSurface(pResource));
        pResource->Lock = m_CpuTime;
    }

    // Record where an object is pushed into the push buffer so we know when
    // it can be locked.

    __forceinline VOID RecordSurfaceWritePush(IDirect3DResource8 *pResource)
    {
        ASSERT(PixelJar::IsSurface(pResource));

        D3DResource *pParent = ((D3DSurface*) pResource)->Parent;

        if (pParent)
        {
            pResource = pParent;
        }

        pResource->Lock = m_CpuTime;
    }
};

} // end namespace
