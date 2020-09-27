/*==========================================================================;
 *
 *  Copyright (C) 1997 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	ddkmmini.h
 *  Content:	Mini VDD support for DirectDraw
 *
 ***************************************************************************/

#ifndef __DDKMMINI_INCLUDED__
#define __DDKMMINI_INCLUDED__


/*============================================================================
 *
 * DDHAL table filled in by the Mini VDD and called by DirectDraw
 *
 *==========================================================================*/

typedef DWORD (* MINIPROC)(VOID);

typedef struct _DDMINIVDDTABLE {
    DWORD	dwMiniVDDContext;
    MINIPROC	vddGetIRQInfo;
    MINIPROC	vddIsOurIRQ;
    MINIPROC	vddEnableIRQ;
    MINIPROC	vddSkipNextField;
    MINIPROC	vddBobNextField;
    MINIPROC	vddSetState;
    MINIPROC	vddLock;
    MINIPROC	vddFlipOverlay;
    MINIPROC	vddFlipVideoPort;
    MINIPROC	vddGetPolarity;
    MINIPROC	vddReserved1;
    MINIPROC	vddGetCurrentAutoflip;
    MINIPROC	vddGetPreviousAutoflip;
    MINIPROC	vddTransfer;
    MINIPROC	vddGetTransferStatus;
} DDMINIVDDTABLE;
typedef DDMINIVDDTABLE *LPDDMINIVDDTABLE;


/*============================================================================
 *
 * MDL structure for handling pagelocked memory.  This is copied from WDM.H
 *
 *==========================================================================*/

typedef struct _MDL {
    struct _MDL *MdlNext;
    short MdlSize;
    short MdlFlags;
    struct _EPROCESS *Process;
    ULONG *lpMappedSystemVa;
    ULONG *lpStartVa;
    ULONG ByteCount;
    ULONG ByteOffset;
} MDL;
typedef MDL *PMDL;

#define MDL_MAPPED_TO_SYSTEM_VA     0x0001
#define MDL_PAGES_LOCKED            0x0002
#define MDL_SOURCE_IS_NONPAGED_POOL 0x0004
#define MDL_ALLOCATED_FIXED_SIZE    0x0008
#define MDL_PARTIAL                 0x0010
#define MDL_PARTIAL_HAS_BEEN_MAPPED 0x0020
#define MDL_IO_PAGE_READ            0x0040
#define MDL_WRITE_OPERATION         0x0080
#define MDL_PARENT_MAPPED_SYSTEM_VA 0x0100
#define MDL_LOCK_HELD               0x0200
#define MDL_SCATTER_GATHER_VA       0x0400
#define MDL_IO_SPACE                0x0800
#define MDL_NETWORK_HEADER          0x1000
#define MDL_MAPPING_CAN_FAIL        0x2000
#define MDL_ALLOCATED_MUST_SUCCEED  0x4000
#define MDL_64_BIT_VA               0x8000

#define MDL_MAPPING_FLAGS (MDL_MAPPED_TO_SYSTEM_VA     | \
                           MDL_PAGES_LOCKED            | \
                           MDL_SOURCE_IS_NONPAGED_POOL | \
                           MDL_PARTIAL_HAS_BEEN_MAPPED | \
                           MDL_PARENT_MAPPED_SYSTEM_VA | \
                           MDL_LOCK_HELD               | \
                           MDL_SYSTEM_VA               | \
                           MDL_IO_SPACE )

typedef DWORD *PKEVENT;

/*============================================================================
 *
 * Structures maintained by DirectDraw
 *
 *==========================================================================*/

//
// Data for every kernel mode surface
//
typedef struct _DDSURFACEDATA {
    DWORD	dwSize;			// Structure size
    DWORD	ddsCaps;		// Ring 3 creation caps
    DWORD	dwSurfaceOffset;	// Offset in frame buffer of surface
    DWORD	fpLockPtr;		// Surface lock ptr
    DWORD	dwWidth;		// Surface width
    DWORD	dwHeight;		// Surface height
    LONG	lPitch;			// Surface pitch
    DWORD	dwOverlayFlags;		// DDOVER_XX flags
    DWORD	dwOverlayOffset;	// Offset in frame buffer of overlay
    DWORD	dwOverlaySrcWidth;	// Src width of overlay
    DWORD	dwOverlaySrcHeight;	// Src height of overlay
    DWORD	dwOverlayDestWidth;	// Dest width of overlay
    DWORD	dwOverlayDestHeight;	// Dest height of overlay
    DWORD	dwVideoPortId; 		// ID of video port (-1 if not connected to a video port)
    ULONG	pInternal1;		// Private
    ULONG	pInternal2;		// Private
    ULONG	pInternal3;		// Private
    DWORD	dwFormatFlags;
    DWORD	dwFormatFourCC;
    DWORD	dwFormatBitCount;
    DWORD	dwRBitMask;
    DWORD	dwGBitMask;
    DWORD	dwBBitMask;
    DWORD	dwSurfInternalFlags;	// Private internal flags
    DWORD	dwIndex;		// Private
    DWORD	dwRefCnt;		// Private
    DWORD	dwDriverReserved1;	// Reserved for the HAL/Mini VDD
    DWORD	dwDriverReserved2;	// Reserved for the HAL/Mini VDD
    DWORD	dwDriverReserved3;	// Reserved for the HAL/Mini VDD
} DDSURFACEDATA;
typedef DDSURFACEDATA * LPDDSURFACEDATA;

//
// Data for every kernel mode video port
//
typedef struct DDVIDEOPORTDATA {
    DWORD	dwSize;			// Structure size
    DWORD	dwVideoPortId;		// ID of video port (0 - MaxVideoPorts-1)
    DWORD	dwVPFlags;		// Offset in frame buffer of surface
    DWORD	dwOriginOffset;		// Start address relative to surface
    DWORD	dwHeight;		// Height of total video region (per field)
    DWORD	dwVBIHeight;		// Height of VBI region (per field)
    DWORD	dwDriverReserved1;	// Reserved for the HAL/Mini VDD
    DWORD	dwDriverReserved2;	// Reserved for the HAL/Mini VDD
    DWORD	dwDriverReserved3;	// Reserved for the HAL/Mini VDD
} DDVIDEOPORTDATA;
typedef DDVIDEOPORTDATA *LPDDVIDEOPORTDATA;


/*============================================================================
 *
 * Structures used to communicate with the Mini VDD
 *
 *==========================================================================*/

// Output from vddGetIRQInfo
typedef struct _DDGETIRQINFO {
    DWORD	dwSize;
    DWORD	dwFlags;
    DWORD	dwIRQNum;
} DDGETIRQINFO;
#define IRQINFO_HANDLED                 0x01    // Mini VDD is managing IRQ
#define IRQINFO_NOTHANDLED              0x02    // Mini VDD wants VDD to manage the IRQ
#define IRQINFO_NODISABLEFORDOSBOX      0x04    // DDraw should  not tell mini VDD to disable IRQs when DOS boxes
                                                //  occur because they might still be able to operate in this mode

// Input to vddEnableIRQ
typedef struct _DDENABLEIRQINFO {
    DWORD dwSize;
    DWORD dwIRQSources;
    DWORD dwLine;
    DWORD IRQCallback;	// Mini VDD calls this when IRQ happens if they are managing the IRQ
    DWORD dwContext;	// Context to be specified in EBX when IRQCallback is called
} DDENABLEIRQINFO;

// Input to vddFlipVideoPort
typedef struct _DDFLIPVIDEOPORTINFO {
    DWORD dwSize;
    DWORD lpVideoPortData;
    DWORD lpCurrentSurface;
    DWORD lpTargetSurface;
    DWORD dwFlipVPFlags;
} DDFLIPVIDEOPORTINFO;

// Input to vddFlipOverlay
typedef struct _DDFLIPOVERLAYINFO {
    DWORD dwSize;
    DWORD lpCurrentSurface;
    DWORD lpTargetSurface;
    DWORD dwFlags;
} DDFLIPOVERLAYINFO;

// Input to vddSetState
typedef struct _DDSTATEININFO {
    DWORD dwSize;
    DWORD lpSurfaceData;
    DWORD lpVideoPortData;
} DDSTATEININFO;

// Output from vddSetState
typedef struct _DDSTATEOUTINFO {
    DWORD dwSize;
    DWORD dwSoftwareAutoflip;
    DWORD dwSurfaceIndex;
    DWORD dwVBISurfaceIndex;
} DDSTATEOUTINFO;

// Input to vddGetPolarity
typedef struct _DDPOLARITYININFO {
    DWORD dwSize;
    DWORD lpVideoPortData;
} DDPOLARITYININFO;

// Output from vddGetPolarity
typedef struct _DDPOLARITYOUTINFO {
    DWORD dwSize;
    DWORD bPolarity;
} DDPOLARITYOUTINFO;

// Input to vddLock
typedef struct _DDLOCKININFO {
    DWORD dwSize;
    DWORD lpSurfaceData;
} DDLOCKININFO;

// Output from vddLock
typedef struct _DDLOCKOUTINFO {
    DWORD dwSize;
    DWORD dwSurfacePtr;
} DDLOCKOUTINFO;

// Input to vddBobNextField
typedef struct _DDBOBINFO {
    DWORD dwSize;
    DWORD lpSurface;
} DDBOBINFO;

// Input to vddSkipNextField
typedef struct _DDSKIPINFO {
    DWORD dwSize;
    DWORD lpVideoPortData;
    DWORD dwSkipFlags;
} DDSKIPINFO;

// Input to vddSetSkipPattern
typedef struct _DDSETSKIPINFO {
    DWORD dwSize;
    DWORD lpVideoPortData;
    DWORD dwPattern;
    DWORD dwPatternSize;
} DDSETSKIPINFO;

// Input to vddGetCurrent/PreviousAutoflip
typedef struct _DDGETAUTOFLIPININFO {
    DWORD dwSize;
    DWORD lpVideoPortData;
} DDGETAUTOFLIPININFO;

// Output from vddGetCurrent/PreviousAutoflip
typedef struct _DDGETAUTOFLIPOUTINFO {
    DWORD dwSize;
    DWORD dwSurfaceIndex;
    DWORD dwVBISurfaceIndex;
} DDGETAUTOFLIPOUTINFO;

// Input to vddTransfer
typedef struct _DDTRANSFERININFO {
    DWORD dwSize;
    DWORD lpSurfaceData;
    DWORD dwStartLine;
    DWORD dwEndLine;
    DWORD dwTransferID;
    DWORD dwTransferFlags;
    PMDL  lpDestMDL;
} DDTRANSFERININFO;

// Input to vddTransfer
typedef struct _DDTRANSFEROUTINFO {
    DWORD dwSize;
    DWORD dwBufferPolarity;
} DDTRANSFEROUTINFO;

// Input to vddGetTransferStatus
typedef struct _DDGETTRANSFERSTATUSOUTINFO {
    DWORD dwSize;
    DWORD dwTransferID;
} DDGETTRANSFERSTATUSOUTINFO;


// IRQ source flags
#define DDIRQ_DISPLAY_VSYNC			0x00000001l
#define DDIRQ_BUSMASTER				0x00000002l
#define DDIRQ_VPORT0_VSYNC			0x00000004l
#define DDIRQ_VPORT0_LINE			0x00000008l
#define DDIRQ_VPORT1_VSYNC			0x00000010l
#define DDIRQ_VPORT1_LINE			0x00000020l
#define DDIRQ_VPORT2_VSYNC			0x00000040l
#define DDIRQ_VPORT2_LINE			0x00000080l
#define DDIRQ_VPORT3_VSYNC			0x00000100l
#define DDIRQ_VPORT3_LINE			0x00000200l
#define DDIRQ_VPORT4_VSYNC			0x00000400l
#define DDIRQ_VPORT4_LINE			0x00000800l
#define DDIRQ_VPORT5_VSYNC			0x00001000l
#define DDIRQ_VPORT5_LINE			0x00002000l
#define DDIRQ_VPORT6_VSYNC			0x00004000l
#define DDIRQ_VPORT6_LINE			0x00008000l
#define DDIRQ_VPORT7_VSYNC			0x00010000l
#define DDIRQ_VPORT7_LINE			0x00020000l
#define DDIRQ_VPORT8_VSYNC			0x00040000l
#define DDIRQ_VPORT8_LINE			0x00080000l
#define DDIRQ_VPORT9_VSYNC			0x00010000l
#define DDIRQ_VPORT9_LINE			0x00020000l
#define DDIRQ_MISCELLANOUS                      0x80000000l

// SkipNextField flags
#define DDSKIP_SKIPNEXT			1
#define DDSKIP_ENABLENEXT		2

// Flip VP flags
#define DDVPFLIP_VIDEO			0x00000001l
#define DDVPFLIP_VBI			0x00000002l

#define DDTRANSFER_SYSTEMMEMORY		0x00000001
#define DDTRANSFER_NONLOCALVIDMEM	0x00000002
#define DDTRANSFER_INVERT		0x00000004
#define DDTRANSFER_CANCEL		0x00000080
#define DDTRANSFER_HALFLINES		0x00000100



#endif

