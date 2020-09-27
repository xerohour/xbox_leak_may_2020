#ifndef _DDPRIV_
#define _DDPRIV_

/*==========================================================================;
 *
 *  Copyright (C) 1995, 1999 NVidia Corporation. All Rights Reserved.
 *
 *  File:       ddpriv.h
 *  Content:    header for ddpriv.cpp
 *
 ***************************************************************************/

typedef struct _MYDCICMD
{
	ULONG	dwCommand;
	ULONG	dwParam1;
	ULONG 	dwParam2;
	ULONG	dwVersion;
	ULONG	dwReserved;
} MYDCICMD;

typedef struct _MYDD32BITDRIVERDATA
{
    UCHAR   szName[260];            // 32-bit driver name
    UCHAR   szEntryPoint[64];       // entry point
    ULONG   dwContext;              // context to pass to entry point
} MYDD32BITDRIVERDATA;

#define DD_VERSION              0x00000200l
#define DDCREATEDRIVEROBJECT    10
#define DDGET32BITDRIVERNAME    11
#define DDNEWCALLBACKFNS        12
#define DDVERSIONINFO           13
#define DDDESTROYDRIVEROBJECT   99

#define MAX_ADAPTERS            9

#ifdef WINNT
#ifndef NV_ESC_DDPRIV_DMABLIT
// lpIn = pointer to input structure to nvMoComp
// lpOut = pointer to a DWORD return code
#define NV_ESC_DDPRIV_DMABLIT               0x6E88
#endif

#ifndef NV_ESC_DDPRIV_OVERLAYCONTROL
// lpIn = pointer to input structure to nvOverlayControl
// lpOut = pointer to a DWORD return code
#define NV_ESC_DDPRIV_OVERLAYCONTROL        0x6E89
#endif
#endif // WINNT

// --------------------------------------------------------------------------
// nvOverlayControl
//      Overlay control callback
//
// Command Structure
// -----------------
// NVOCCMD_GETCAPS
//      dwSize
//      dwDevice
//      returns caps
//          dwCaps1
//              NVOCCAPS1_BRIGHTNESS
//              NVOCCAPS1_CONTRAST
//              NVOCCAPS1_HUE
//              NVOCCAPS1_SATURATION
//              NVOCCAPS1_SHARPNESS
//              NVOCCAPS1_GAMMA
//          dwCaps2
//              NVOCCAPS2_HQVUPSCALE
//              NVOCCAPS2_TFILTER
//              NVOCCAPS2_DFILTER
//              NVOCCAPS2_FSMIRROR
//              NVOCCAPS2_OVLZOOM
//          dwCaps3
//              NVOCCAPS3_SUBPICTURE
//              NVOCCAPS3_LATEFLIPSYNC
//              NVOCCAPS3_SPLITVPP
//              NVOCCAPS3_BLTCAPS
//          dwCaps4
//              NVOCCAPS4_SMA
//              NVOCCAPS4_IMB
//              NVOCCAPS4_DEVICEHASOVL
//              NVOCCAPS4_MULTIHEADNUM (number of display heads on this device)
//              NVOCCAPS4_MULTIHEADMODE (0 = single, 1 = span, 2 = clone)
//              NVOCCAPS4_HEAD0_TYPE (0 = CRT, 1 = NTSC, 2 = PAL, 3 = DFP, -1 = inactive)
//              NVOCCAPS4_HEAD1_TYPE (0 = CRT, 1 = NTSC, 2 = PAL, 3 = DFP, -1 = inactive)
//          dwCaps5
//              NVOCCAPS5_BLT_YUV9_2_YUV422
//              NVOCCAPS5_BLT_YUV12_2_YUV422
//              NVOCCAPS5_BLT_YC12_2_YUV422
//              NVOCCAPS5_BLT_YUV422_2_YUV422
//              NVOCCAPS5_BLT_YUV422_2_RGB8
//              NVOCCAPS5_BLT_YUV422_2_RGB15
//              NVOCCAPS5_BLT_YUV422_2_RGB16
//              NVOCCAPS5_BLT_YUV422_2_RGB32
//              NVOCCAPS5_BLT_SYUV2VYUV
//              NVOCCAPS5_BLT_VYUV2VYUV
//              NVOCCAPS5_BLT_VYUV2SYUV
//              NVOCCAPS5_BLT_SYUV2VRGB
//              NVOCCAPS5_BLT_VYUV2VRGB
//              NVOCCAPS5_BLT_VYUV2SRGB
//              NVOCCAPS5_BLT_SRGB2VRGB
//              NVOCCAPS5_BLT_VRGB2VRGB
//              NVOCCAPS5_BLT_VRGB2SRGB
//              NVOCCAPS5_BLT_2RGBTEX
// NVOCCMD_SETCOLORCONTROL
//      dwSize
//      dwDevice
//      ccData
//          dwEnable (ignored for NV10 and above, always enabled)
//              0 = disable
//              1 = enable with vertical chroma subsampling
//              2 = enable with full chroma sampling (do not use)
//          dwCCFlags
//              NVOCCC_BRIGHTNESS
//                  lBrightness (range is -256 to +255)
//              NVOCCC_CONTRAST
//                  lContrast (range is 0 to 200)
//              NVOCCC_HUE
//                  lHue (range is 0 to 360)
//              NVOCCC_SATURATION
//                  lSaturation (range is 0 to 200)
// NVOCCMD_GETCOLORCONTROL
//      dwSize
//      dwDevice
//      returns ccData
//          returns dwEnable
//              0 = disable
//              1 = enable with vertical chroma subsampling
//              2 = enable with full chroma sampling (do not use)
//          returns dwCCFlags
//              NVOCCC_BRIGHTNESS
//                  returns lBrightness
//              NVOCCC_CONTRAST
//                  returns lContrast
//              NVOCCC_HUE
//                  returns lHue
//              NVOCCC_SATURATION
//                  returns lSaturation
// NVOCCMD_SETFEATURE (enables feature)
//      dwSize
//      dwDevice
//      dwFeature1
//          NVOCF1_HQVUPSCALE (always enable if enough memory (8M))
//          NVOCF1_TFILTER (only enable in BOB mode if enough memory (16M))
//              lCoefficient[0] (recommended value = 0xA4)
//              dwCmdFlags (optional)
//                  NVOCF_TF_PRECOPY
//          NVOCF1_DFILTER (only enable in BOB mode if enough memory (16M))
//              lCoefficient[1] (recommended value = 0xA4)
//              dwCmdFlags (optional)
//                  NVOCF_DF_PRECOPY
//          NVOCF1_FSMIRROR (only enable with dual head and large memory)
//              lCoefficient[4]:
//                  bits 0-3: head number
//                            0 = disable FSVM
//                            1 = display 1
//                            2 = display 2
//                            15 = autoselect display
//                  bits 4-7: aspect ratio
//                            0 = stretch to display full screen
//                            1 = preserve aspect of source (assume square pixels)
//                            3 = force anamorphic 16:9
//                            5 = track overlay aspect
//                            9 = force TV 4:3
//                  bits 8-11: zoom quadrant
//                             0 = disable zoom
//                             1 = top left
//                             2 = top right
//                             3 = bottom left
//                             4 = bottom right
//                             5 = center
//                  bits 12-19: zoom factor, 0 = 1x, 255 = 2x
//                  bit 20: 0 = use current display size
//                          1 = let driver pick the display size
//                  bit 21: 0 = independent FS and overlay zoom
//                          1 = FS zoom tracks overlay zoom controls
//                  bit 22: 0 = disable DVD output to TV
//                          1 = enable DVD output to TV
//          NVOCF1_OVLZOOM
//              lCoefficient[5]:
//                  bits 8-11: zoom quadrant
//                             0 = disable zoom
//                             1 = top left
//                             2 = top right
//                             3 = bottom left
//                             4 = bottom right
//                             5 = center
//                  bits 12-23: zoom factor, 0 = 1x, 255 = 2x
//          NVOCF1_DELIVERYCONTROL
//              lCoefficient[6]: low 32 bits of frame period (in ns)
//              lCoefficient[7]: high 32 bits of frame period (in ns)
//          NVOCF1_LATEFLIPSYNC
//      dwFeature2
//          NVOCF2_VPPMAXSURFACES
//              lCoefficient[2] = set to 0, 2, 3, 4, or 6
//              lCoefficient[3] = reserved
// NVOCCMD_RESETFEATURE (disables feature)
//      dwSize
//      dwDevice
//      dwFeature1
//          NVOCF1_HQVUPSCALE
//          NVOCF1_TFILTER
//          NVOCF1_DFILTER
//          NVOCF1_FSMIRROR
//          NVOCF1_OVLZOOM
//          NVOCF1_DELIVERYCONTROL
//          NVOCF1_LATEFLIPSYNC
// NVOCCMD_GETFEATURE
//      dwSize
//      dwDevice
//      returns dwFeature1
//          NVOCF1_HQVUPSCALE
//          NVOCF1_TFILTER
//              returns lCoefficient[0]
//          NVOCF1_DFILTER
//              returns lCoefficient[1]
//          NVOCF1_FSMIRROR
//              returns lCoefficient[4]
//          NVOCF1_OVLZOOM
//              returns lCoefficient[5]
//          NVOCF1_DELIVERYCONTROL
//              returns lCoefficient[6]
//                      lCoefficient[7]
//          NVOCF1_LATEFLIPSYNC
//      returns dwFeature2
//          NVOCF2_VPPMAXSURFACES
//              lCoefficient[2] = max VPP work surfaces
//              lCoefficient[3] = VPP work surfaces currently allocated
//

#define OVERLAYCONTROLCALLBACK "nvOverlayControl"

typedef enum { 
    NVOCERR_OK              = 0,
    NVOCERR_GENERIC_WARN    = 1,
    NVOCERR_GENERIC_FATAL   = 0x80000001,
    NVOCERR_BAD_COMMAND     = 0x80000002,
    NVOCERR_INVALID_PARMS   = 0x80000003,
    NVOCERR_ALREADY_ACTIVE  = 0x80000004,
    NVOCERR_NOT_SUPPORTED   = 5,
    NVOCERR_BAD_DEVICE      = 0x80000006
} NVOCERROR;

typedef enum {
    NVOCCMD_NOOP            = 0,
    NVOCCMD_GETCAPS         = 1,
    NVOCCMD_SETCOLORCONTROL = 2,
    NVOCCMD_GETCOLORCONTROL = 3,
    NVOCCMD_SETFEATURE      = 4,
    NVOCCMD_RESETFEATURE    = 5,
    NVOCCMD_GETFEATURE      = 6
} NVOCCOMMAND;

typedef struct {
    unsigned long   dwCCFlags;
    unsigned long   dwEnable;
    long            lBrightness;
    long            lContrast;
    long            lHue;
    long            lSaturation;
    long            lSharpness;
    long            lGamma;
} NVOCCCDATA, *LPNVOCCCDATA;

#define NVOCCC_BRIGHTNESS   0x1
#define NVOCCC_CONTRAST     0x2
#define NVOCCC_HUE          0x4
#define NVOCCC_SATURATION   0x8
#define NVOCCC_SHARPNESS    0x10
#define NVOCCC_GAMMA        0x20

typedef struct {
    unsigned long dwCaps1;
    unsigned long dwCaps2;
    unsigned long dwCaps3;
    unsigned long dwCaps4;
    unsigned long dwCaps5;
} NVOCCAPS, *LPNVOCCAPS;

#define NVOCCAPS1_BRIGHTNESS    0x1
#define NVOCCAPS1_CONTRAST      0x2
#define NVOCCAPS1_HUE           0x4
#define NVOCCAPS1_SATURATION    0x8
#define NVOCCAPS1_SHARPNESS     0x10
#define NVOCCAPS1_GAMMA         0x20

#define NVOCCAPS2_HQVUPSCALE    0x1
#define NVOCCAPS2_TFILTER       0x2
#define NVOCCAPS2_DFILTER       0x4
#define NVOCCAPS2_FSMIRROR      0x8
#define NVOCCAPS2_OVLZOOM       0x10

#define NVOCCAPS3_SUBPICTURE        0x1
#define NVOCCAPS3_LATEFLIPSYNC      0x2
#define NVOCCAPS3_SPLITVPP          0x4
#define NVOCCAPS3_DELIVERYCONTROL   0x8
#define NVOCCAPS3_BLTCAPS           0x80000000

#define NVOCCAPS4_IMB           0x1
#define NVOCCAPS4_SMA           0x2
#define NVOCCAPS4_DEVICEHASOVL  0x4
#define NVOCCAPS4_MULTIHEADNUM  0x000F0000
#define NVOCCAPS4_MULTIHEADMODE 0x00F00000
#define NVOCCAPS4_HEAD0_TYPE    0x0F000000
#define NVOCCAPS4_HEAD1_TYPE    0xF0000000

#define NVOCCAPS5_BLT_YUV9_2_YUV422     0x00000001
#define NVOCCAPS5_BLT_YUV12_2_YUV422    0x00000002
#define NVOCCAPS5_BLT_YC12_2_YUV422     0x00000004
#define NVOCCAPS5_BLT_YUV422_2_YUV422   0x00000008
#define NVOCCAPS5_BLT_YUV422_2_RGB8     0x00000100
#define NVOCCAPS5_BLT_YUV422_2_RGB15    0x00000200
#define NVOCCAPS5_BLT_YUV422_2_RGB16    0x00000400
#define NVOCCAPS5_BLT_YUV422_2_RGB32    0x00000800
#define NVOCCAPS5_BLT_SYUV2VYUV         0x00010000
#define NVOCCAPS5_BLT_VYUV2VYUV         0x00020000
#define NVOCCAPS5_BLT_VYUV2SYUV         0x00040000
#define NVOCCAPS5_BLT_SYUV2VRGB         0x00100000
#define NVOCCAPS5_BLT_VYUV2VRGB         0x00200000
#define NVOCCAPS5_BLT_VYUV2SRGB         0x00400000
#define NVOCCAPS5_BLT_SRGB2VRGB         0x01000000
#define NVOCCAPS5_BLT_VRGB2VRGB         0x02000000
#define NVOCCAPS5_BLT_VRGB2SRGB         0x04000000
#define NVOCCAPS5_BLT_2RGBTEX           0x80000000

typedef struct {
    unsigned long dwFeature1;
    unsigned long dwFeature2;
    unsigned long dwFeature3;
    unsigned long dwFeature4;
    long          lCoefficient[16];
} NVOCFEATUREDATA;

#define NVOCF1_HQVUPSCALE       0x1
#define NVOCF1_TFILTER          0x2
#define NVOCF1_LATEFLIPSYNC     0x4
#define NVOCF1_DFILTER          0x8
#define NVOCF1_FSMIRROR         0x10
#define NVOCF1_OVLZOOM          0x20
#define NVOCF1_DELIVERYCONTROL  0x40

#define NVOCF2_VPPMAXSURFACES   0x1

#define NVOCF_TF_PRECOPY   0x00000001
#define NVOCF_DF_PRECOPY   0x00000002

typedef struct {
    unsigned long dwSize;
    unsigned long dwDevice;

    // command
    NVOCCOMMAND cmd;
    unsigned long dwCmdFlags;

    // data
    union {
        NVOCCAPS        caps;
        NVOCCCDATA      ccData;
        NVOCFEATUREDATA featureData;
    };

    unsigned long dwReserved[40];
} NVOCDATATYPE, *LPNVOCDATATYPE;

typedef NVOCERROR (__stdcall *NVOVERLAYCONTROL)(LPNVOCDATATYPE data);


// --------------------------------------------------------------------------
// nvSetOverlayColorControl
//      Obsolete.  Only use with rel3 drivers
//      If lpDDCC is NULL, callback returns caps
//
//      dwDevice            always 1
//      lpDDCC->dwSize      sizeof(DDCOLORCONTROL)
//      lpDDCC->dwFlags     DDCOLOR_CONTRAST | DDCOLOR_BRIGHTNESS | DDCOLOR_HUE | DDCOLOR_SATURATION
//                          determines which members hold valid data
//      lpDDCC->lBrightness range is from -256 to +255
//      lpDDCC->lContrast   range is from 0 to 200
//      lpDDCC->lHue        range is from 0 to 360
//      lpDDCC->lSaturation range is from 0 to 200
//      lpDDCC->dwReserved1 0 = disable colour controls
//                          1 = enable with vertical chroma subsampling
//                          2 = enable with full chroma sampling
#define SETOVERLAYCOLORCONTROLCALLBACK "nvSetOverlayColorControl"

typedef DWORD (__stdcall *NVSETOVERLAYCOLORCONTROL)(DWORD dwDevice, LPDDCOLORCONTROL lpDDCC);


// --------------------------------------------------------------------------
// nvMoComp
//      Backdoor DMA blits and formatting for video
// 
// Command Structure
// -----------------
// NVS2VCMD_INIT
//      dwSize
//      dwDevice
//      dwIndex
//      dwHeight
//      dwSrcType
//          NVS2V_YUYV:
//              fpSrc
//          NVS2V_UYVY:
//              fpSrc
//          NVS2V_RGB16:
//              fpSrc
//          NVS2V_RGB32:
//              fpSrc
//          NVS2V_YC12
//              SrcYC12.fpY
//          NVS2V_YUV12
//              SrcYUV12.fpY (points to start of surface)
//      dwSrcPitch
//      dwDstFlags (optional)
//          NVS2VF_OFFSET
//      dwDstType (required only if using INDEXED type on BLIT)
//          NVS2V_YUYV
//              fpDst
//          NVS2V_UYVY
//              fpDst
//          NVS2V_DDSURF
//              lpDstDirectDrawSurface
// NVS2VCMD_RELEASE
//      dwSize
//      dwDevice
//      dwIndex
// NVS2VCMD_BLIT
//      dwSize
//      dwDevice
//      dwIndex
//      dwWidth
//      dwHeight
//      dwSrcFlags (optional)
//          NVS2VF_OFFSET (only use for V->S blits)
//          NVS2VF_LOCAL
//          NVS2VF_SYSTEM (default)
//      dwSrcType
//          NVS2V_YC12
//              SrcYC12.fpY
//              SrcYC12.fpC
//          NVS2V_YUV12
//              SrcYUV12.fpY
//              SrcYUV12.fpU
//              SrcYUV12.fpV
//          NVS2V_YUYV
//              fpSrc
//          NVS2V_UYVY
//              fpSrc
//          NVS2V_YV12
//              SrcYUV12.fpY
//              SrcYUV12.fpU
//              SrcYUV12.fpV
//              dwSrcYV12ChromaPitch
//      dwSrcPitch
//      dwDstFlags (optional)
//          NVS2VF_OFFSET (only use for S->V blits)
//          NVS2VF_LOCAL (default)
//          NVS2VF_SYSTEM
//      dwDstType
//          NVS2V_YUYV
//              fpDst
//          NVS2V_UYVY
//              fpDst
//          NVS2V_DDSURF
//              lpDstDirectDrawSurface
//          NVS2V_INDEXED (uses last YUY2 or UYVY state)
//              dwDstSize
//      dwDstPitch
//      bWaitForCompletion
//          dwTimeoutTime
// NVS2VCMD_QUERY_IDLE
//      dwSize
//      dwDevice
// NVS2VCMD_WAIT_FOR_IDLE
//      dwSize
//      dwDevice
//      dwTimeoutTime
// NVS2VCMD_SET_SUBPICTURE
//      dwSize
//      dwDevice
//      dwSPFlags (optional)
//          NVS2VF_OFFSET
//          NVS2VF_SP_SUBRECT
//              dwSubRectX
//              dwSubRectY
//              dwSubRectWidth
//              dwSubRectHeight
//      dwSPType
//      dwSPPitch
//      fpSP
// NVS2VCMD_VPP_START
//      dwSize
//      dwDevice
//      dwWidth
//      dwHeight
//      dwSrcType
//          NVS2V_YUYV
//              fpSrc
//          NVS2V_UYVY
//              fpSrc
//      dwSrcPitch
//      dwSrcFlags (optional)
//          NVS2VF_OFFSET
//          NVS2VF_VPP_ODD
//          NVS2VF_VPP_EVEN
//          NVS2VF_VPP_BOB
//          NVS2VF_VPP_INTERLEAVED
// NVS2VCMD_VPP_QUERY
//      dwSize
//      dwDevice
// NVS2VCMD_VPP_FLIP
//      dwSize
//      dwDevice
// NVS2VCMD_4CC_TO_VRGB
//      dwSize
//      dwIndex (0xFFFFFFFF means use video memory)
//      dwCmdFlags (optional)
//          NVS2VF_CRTC_SYNC
//              dwHead
//              dwScanline (blit will not begin until CRTC of specified head reaches this scanline)
//              dwTimeoutTime
//          NVS2VF_USE_DELIVERY_CTL
//              dwTimeoutTime
//      dwDevice
//      dwWidth
//      dwHeight
//      dwSrcFlags (optional)
//          NVS2VF_OFFSET
//          NVS2VF_LOCAL
//          NVS2VF_SYSTEM (default)
//      dwSrcType
//          NVS2V_YUYV
//              fpSrc (not required if in system memory)
//              dwSrcPixelBias (31:16 is the Y pixel bias in 12.4 fixed point, 15:0 is the X pixel bias in 12.4 fixed point)
//          NVS2V_UYVY
//              fpSrc (not required if in system memory)
//              dwSrcPixelBias (31:16 is the Y pixel bias in 12.4 fixed point, 15:0 is the X pixel bias in 12.4 fixed point)
//      dwSrcPitch
//      dwDstFlags (optional)
//          NVS2VF_OFFSET
//          NVS2VF_LOCAL (default)
//          NVS2VF_SYSTEM
//          NVS2VF_SWIZZLED
//      dwDstType
//          NVS2V_RGB16
//              DstRGB.fpRGB
//              DstRGB.dwDstWidth
//              DstRGB.dwDstHeight
//          NVS2V_RGB32
//              DstRGB.fpRGB
//              DstRGB.dwDstWidth
//              DstRGB.dwDstHeight
//      dwDstPitch
//      bWaitForCompletion
//          dwTimeoutTime
// NVS2VCMD_GET_TEXTURE_OFFSET
//      dwSize
//      dwDevice
//      fpTexture
//      returns fpTexture


#define MOCOMPCALLBACK "nvMoComp"

typedef enum {
    NVS2V_YC12      = 1,
    NVS2V_YUV12     = 2,
    NVS2V_YUYV      = 3,
    NVS2V_UYVY      = 4,
    NVS2V_DDSURF    = 5,
    NVS2V_INDEXED   = 6,
    NVS2V_YUVA16    = 7,
    NVS2V_YUVA32    = 8,
    NVS2V_YV12      = 9,
    NVS2V_RGB8      = 0x10,
    NVS2V_RGB16     = 0x11,
    NVS2V_RGB24     = 0x12,
    NVS2V_RGB32     = 0x13
} NVS2VSURFTYPE;

typedef enum {
    NVS2VCMD_NOOP               = 0,
    NVS2VCMD_INIT               = 1,
    NVS2VCMD_RELEASE            = 2,
    NVS2VCMD_BLIT               = 3,
    NVS2VCMD_QUERY_IDLE         = 4,
    NVS2VCMD_WAIT_FOR_IDLE      = 5,
    NVS2VCMD_SET_SUBPICTURE     = 6,
    NVS2VCMD_VPP_START          = 7,
    NVS2VCMD_VPP_QUERY          = 8,
    NVS2VCMD_VPP_FLIP           = 9,
    NVS2VCMD_4CC_TO_VRGB        = 0xA,
    NVS2VCMD_GET_TEXTURE_OFFSET = 0xB
} NVS2VCOMMAND;

typedef enum {
    NVS2VERR_OK             = 0,
    NVS2VERR_GENERIC_WARN   = 1,
    NVS2VERR_GENERIC_FATAL  = 0x80000001,
    NVS2VERR_BAD_COMMAND    = 0x80000002,
    NVS2VERR_INVALID_PARMS  = 0x80000003,
    NVS2VERR_INIT_FAILED    = 0x80000004,
    NVS2VERR_NO_EVENT       = 5,
    NVS2VERR_BUSY           = 6,
    NVS2VERR_TIMEOUT        = 7,
    NVS2VERR_BAD_DEVICE     = 0x80000008
} NVS2VERROR;

#define NVS2VF_OFFSET           0x00000001
#define NVS2VF_SP_SUBRECT       0x00000002
#define NVS2VF_SP_ODD           0x00000004
#define NVS2VF_SP_EVEN          0x00000008
#define NVS2VF_VPP_ODD          0x00000010
#define NVS2VF_VPP_EVEN         0x00000020
#define NVS2VF_VPP_BOB          0x00000040
#define NVS2VF_VPP_INTERLEAVED  0x00000080
#define NVS2VF_LOCAL            0x00000100
#define NVS2VF_SYSTEM           0x00000200
#define NVS2VF_SWIZZLED         0x00000400
#define NVS2VF_CRTC_SYNC        0x00000800
#define NVS2VF_USE_DELIVERY_CTL 0x00001000

typedef struct {
    unsigned long dwSize;               // sizeof(NVS2VDATATYPE)
    
    // command data
    NVS2VCOMMAND  dwCmd;
    unsigned long dwCmdFlags;
    BOOL          bWaitForCompletion;
    unsigned long dwTimeoutTime;        // in ms
    unsigned long dwIndex;
    unsigned long dwDevice;
    unsigned long dwHead;
    unsigned long dwScanline;
    unsigned long dwReserved1[55];

    // neutral data
    unsigned long dwWidth;              // in pixels
    unsigned long dwHeight;             // in lines

    // source data
    unsigned long dwSrcFlags;
    NVS2VSURFTYPE dwSrcType;
    unsigned long dwSrcPitch;           // in bytes
    union {
        void  *fpTexture;
        void  *fpSrc;                   // used with NVS2V_YUYV or NVS2V_UYVY or NVS2V_RGB32 or NVS2V_RGB16
        void  *lpSrcDirectDrawSurface;  // used with NVS2V_DDSURF (not implemented)
        unsigned long dwSrcSize;        // used with NVS2V_INDEXED (not implemented)
        struct {
            void  *fpY;
            void  *fpC;
        } SrcYC12;                      // used with NVS2V_YC12 (Y and C must be contiguous)
        struct {
            void  *fpY;
            void  *fpU;
            void  *fpV;
        } SrcYUV12;                     // used with NVS2V_YUV12 (not implemented)
    };

    // destination data
    unsigned long dwDstFlags;
    NVS2VSURFTYPE dwDstType;
    unsigned long dwDstPitch;           // in bytes
    union {
        void  *fpDst;                   // used with NVS2V_YUYV or NVS2V_UYVY or NVS2V_RGB32 or NVS2V_RGB16
        void  *lpDstDirectDrawSurface;  // used with NVS2V_DDSURF
        unsigned long dwDstSize;        // used with NVS2V_INDEXED
        struct {
            void *fpY;
            void *fpC;
        } DstYC12;                      // used with NVS2V_YC12 (not implemented)
        struct {
            void *fpY;
            void *fpU;
            void *fpV;
        } DstYUV12;                     // used with NVS2V_YUV12 (not implemented)
        struct {
            void *fpRGB;
            unsigned long dwDstWidth;
            unsigned long dwDstHeight;
        } DstRGB;                       // used with the RGB surface types
    };

    // subpicture data
    unsigned long dwSPFlags;
    NVS2VSURFTYPE dwSPType;
    unsigned long dwSPPitch;
    unsigned long fpSP;
    unsigned long dwSubRectX;
    unsigned long dwSubRectY;
    unsigned long dwSubRectWidth;
    unsigned long dwSubRectHeight;

    // more source data
    unsigned long dwSrcYV12ChromaPitch;
    unsigned long dwSrcPixelBias;

    // pad
    unsigned long dwReserved2[40];
} NVS2VDATATYPE, *LPNVS2VDATATYPE;

typedef NVS2VERROR (__stdcall *NVMOCOMP)(LPNVS2VDATATYPE data);

#endif