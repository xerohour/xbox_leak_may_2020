#ifndef _NVKELVINAA_H
#define _NVKELVINAA_H

/*
 * Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvKelvinAA.h                                                      *
*     prototypes for aa routines                                            *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Scott Cutler                     11Oct2000   created                *
*                                                                           *
\***************************************************************************/

#if (NVARCH >= 0x020)
typedef union _DWORD_float {
    DWORD d;
    float f;
} DF;

typedef unsigned __int64 uint64;
uint64 nvHash64(unsigned char *, unsigned int);

#define KELVIN_AA_NUM_FLIP_CHAIN_SLOTS 16

class CKelvinAAState {
public:
    // Public enums
    typedef enum BufferType { 
        BUFFER_SRT       = 0,          // Super render target
        BUFFER_RT        = 1,          // Normal render target
        BUFFER_SZB       = 2,          // Super Z buffer
        BUFFER_ZB        = 3,          // Normal Z buffer
        BUFFER_SCRATCH   = 4,          // Stratch Z buffer
        BUFFER_MAX       = 5,
        BUFFER_INVALID   = 0xffffffff  // invalid
    };
    
    typedef enum AccessType { 
        ACCESS_READ          = 0, // Read access (write semantics are undefined)
        ACCESS_WRITE         = 1, // Full read/write access
        ACCESS_WRITE_DISCARD = 2, // Write access, except that a) no read access, and b) you must overwrite the entire buffer
        ACCESS_MAX           = 3
    };

    typedef enum AppCompatibilityFlags {
        // Usually, we assume that apps will completely overwrite the contents of the color buffer.
        // This allows the optimization that we don't have to upsample the normal buffer after a
        // flip.  Some apps, particularly ones where there are cursor drawing routines that use
        // read/modify/write blits, break when using this optimization, and as such we can turn it 
        // off.
        APPFLAGS_MAGNIFY_AFTER_FLIP        = 0x00000001,

        // Apps that perform large numbers of upsample/downsample operations often show dithering 
        // artifacts, because of a hardware property where the dither is applied even for colors
        // truncated to 16 bits.  Force dithering off for these apps (we want it on by default
        // because of banding issues).
        APPFLAGS_DISABLE_DITHER            = 0x00000002,

        // Multisampling has the peculiar property where a texture sample can be taken outside 
        // of the border of a triangle.  In most cases this is ok, as triangles that are joined
        // together usually have their textures join in a similar way.  Unfortunately, sometimes
        // apps will store several uncorrelated textures in a single map, and we sometimes sample
        // from these adjacent textures, leading to color artifacts on the edges of some triangles.
        APPFLAGS_FORCE_SUPERSAMPLE         = 0x00000004,

        // Some apps are just fundamentally broken with AA.  Usually this includes apps that perform
        // nasty hacks with the Z buffer.
        APPFLAGS_DISABLE_AA                = 0x00000008,

        // Win2k will often stomp all over our buffer when we're not in a BeginScene/EndScene.
        // For apps where this is a problem, we give the application write access to the normal
        // buffer when it sends us an EndScene.
        APPFLAGS_WRITE_ACCESS_ON_ENDSCENE  = 0x00000010,

        // Normally, we will blit to either the super or normal buffer, depending on
        // which one is valid.  Some apps don't like it when we blit to the superbuffer.
        // In these cases, always downscale the buffer before blitting.
        APPFLAGS_DOWNSAMPLE_ON_BLIT        = 0x00000020,

        // Z compression doesn't always work right when we're upscaling/downscaling
        // the Z buffer.  Force compression off if it's causing problems.
        APPFLAGS_FORCE_UNCOMPRESSED_Z      = 0x00000040
    };
    
    typedef enum NotificationType {
        NOTIFY_FULL_SCREEN_COLOR_CLEAR,
    };

    static const char * BufferTypeNames[];
    static const char * AccessTypeNames[];
    
    typedef enum BufferFlags {
        FLAGS_VALID  = 0x00000001,  // Contains valid data
    };

    // Public members
    CKelvinAAState();

    void SetSampleMask();

    void TestCreation(PNVD3DCONTEXT);
    void Create(PNVD3DCONTEXT);
    void Destroy();
    void Suspend();
    void Resume();
    void Refresh();
    void Flip();
    void Notify(NotificationType);
    void ReCreateZBuffer();
    void TagBadSemantics() { _badSemantics = true; }
    void EndScene();
    bool IsValid(BufferType buffer) { return (_bufferInfo[buffer].flags & FLAGS_VALID); }

    void Save();
    void Restore();
    void SetChanged() { _bAAMethodChanged = true; }

    void UpdateZBuffer();
    void UpdateRenderTarget();

    float GetXOffset() { return _appCompatibility.fXOffset; }
    float GetYOffset() { return _appCompatibility.fYOffset; }

    void TagSeen3D() { _seen3D = 1; }

    CSimpleSurface * GetSRT() { return _bufferInfo[BUFFER_SRT].pSurface; }
    CSimpleSurface * GetSZB() { return _bufferInfo[BUFFER_SZB].pSurface; }

    CSimpleSurface * GetCurrentRT(PNVD3DCONTEXT);
    CSimpleSurface * GetCurrentZB(PNVD3DCONTEXT);

    DWORD GetAAFormat();
    DWORD GetWidthScale() { return BufferBltInfo[MethodInfo[_method].type].dwSuperBufferScaleX; }
    DWORD GetHeightScale() { return BufferBltInfo[MethodInfo[_method].type].dwSuperBufferScaleY; }
    
    int IsEnabled();
    int IsCreated();
    int IsMultisampling();
    int IsSupersampling();

    bool CompatibilityMode(AppCompatibilityFlags flags) { return ((_appCompatibility.dwFlags & flags) ? true : false); }
    bool IsMixedModeAA() { return _MixedModeAA; }
    static bool CheckBufferMemoryConfig(PNVD3DCONTEXT);

    void GrantAccess(BufferType, AccessType, bool = false, bool = false);

    static PNVD3DCONTEXT GetContext(CSimpleSurface *);

private:
    // Private enums
    typedef enum MethodType {
        METHOD_NONE                         = 0,
        METHOD_SUPERSAMPLE_2X_H             = 1,
        METHOD_SUPERSAMPLE_2X_V             = 2,
        METHOD_MULTISAMPLE_2X               = 3,
        METHOD_MULTISAMPLE_2X_QUINCUNX      = 4,
        METHOD_SUPERSAMPLE_4X_BIAS          = 5,
        METHOD_SUPERSAMPLE_4X               = 6,
        METHOD_SUPERSAMPLE_4X_GAUSSIAN      = 7,
        METHOD_MULTISAMPLE_4X               = 8,
        METHOD_MULTISAMPLE_4X_GAUSSIAN      = 9,
        METHOD_MULTISAMPLE_4X_ROTATED_4TAP  = 10,   
        METHOD_MULTISAMPLE_4X_ROTATED_8TAP  = 11,
        METHOD_SUPERSAMPLE_9X_BIAS          = 12,   // Not used yet
        METHOD_SUPERSAMPLE_9X               = 13,   // Not used yet
        METHOD_SUPERSAMPLE_16X_BIAS         = 14,   // Not used yet
        METHOD_SUPERSAMPLE_16X              = 15,   // Not used yet
        METHOD_MAX                          = 16
    };

    typedef enum SimpleMethodType {
        SIMPLEMETHOD_1X1 = 0,
        SIMPLEMETHOD_1X2 = 1,
        SIMPLEMETHOD_2X1 = 2,
        SIMPLEMETHOD_2X2 = 3,
        SIMPLEMETHOD_3X3 = 4,
        SIMPLEMETHOD_4X4 = 5,
        SIMPLEMETHOD_MAX = 6,
    };

    // Private structs
    struct BufferInfo {
        DWORD flags;
        BufferType other;
        CSimpleSurface *pSurface;
    } _bufferInfo[BUFFER_MAX];

    struct MethodInfoStruct {
        bool  bMultisample;
        float fBias;
        float fLinePointSize;
        SimpleMethodType type;
        DWORD AASurfaceType;
        DWORD dwKernel;
        DWORD dwBoxFilteredAlternative;
        DWORD dwSuperSampledAlternative;
    };

    struct BufferBltInfoStruct {
        DWORD   dwSuperBufferScaleX;
        DWORD   dwSuperBufferScaleY;

        DWORD   dwScratchBufferScaleX;
        DWORD   dwScratchBufferScaleY;

        double  fDownscaleOffsetX1;
        double  fDownscaleOffsetY1;
        double  fDownscaleOffsetX2;
        double  fDownscaleOffsetY2;

        double  fUpscaleOffsetX;
        double  fUpscaleOffsetY;
    };


    // break filter into strips
    struct  stripinfo {
        DF x0, y0;
        DF _x0;
        DWORD remainder;
        DWORD num_strips;
        DWORD stripsize;
        DF srcyinc;
        DF srcyrem;
        DF dstyinc;
        DF dstyrem;
        DF srcWidth;
        DF srcWidth_x0;
        DF dstWidth;
        DWORD swath;
    } _strip;

    // Private methods
    void ScaledBlt(BufferType, BufferType, bool, bool);
    void ScaledBlt(CSimpleSurface *, DWORD, DWORD, DWORD, DWORD, CSimpleSurface *, double, double, DWORD, DWORD);

    void ImageModeTexBlt(CSimpleSurface *, CSimpleSurface *, DWORD, bool, bool, bool);

    bool InGrantAccess() { return _bInGrantAccess; }
    void EnterGrantAccess() { _bInGrantAccess = true; }
    void ExitGrantAccess() { _bInGrantAccess = false; }

    void InitAppCompatibility();

    // Private members
    PNVD3DCONTEXT _pContext;
    bool _enabled;
    bool _seen3D;
    bool _MixedModeAA;
    bool _invalidRT;
    bool _badSemantics;
    bool _didUpsample;
    bool _lastDownsampleUsedConvolution;
    bool _bSubdivideAA;
    bool _bSubdivideAASave;
    bool _bAAMethodChanged;
    bool _bInitializedState;
    bool _bInGrantAccess;
    DWORD _sampleMask;
    DWORD _method;
    DWORD _methodSave;
    DWORD _numUpsamplings;
    DWORD _UpsampleFlip;
    DWORD _numZLocks;
    KELVIN_MICRO_PROGRAM _program;

    struct AppCompatibilityStruct {
        uint64 qwHashVal;
        DWORD dwFlags;
        float fXOffset, fYOffset;
    } _appCompatibility;

    CSimpleSurface *_flipChain[KELVIN_AA_NUM_FLIP_CHAIN_SLOTS]; // potentially they could have a huge flip chain; in reality it'll be 2 or 3


    static const MethodInfoStruct MethodInfo[METHOD_MAX];
    static const BufferBltInfoStruct BufferBltInfo[SIMPLEMETHOD_MAX];
    static const MethodType RegQualityToAATypeMapping[10];

    void SetMethod(MethodType);
};

#endif // NVARCH >= 0x020

#endif