/*
==============================================================================

    NVIDIA Corporation, (C) Copyright 
    1993-1999. All rights reserved.

    nvpecntx.h

    VPE kernel context definition. This is a shared file with Win9x minivdd

    Dec 17, 1999

==============================================================================
*/
#ifndef _NVPECNTX_H
#define _NVPECNTX_H

#define NVP_MAX_VPORTS				1
#define NVP_MAX_SURFACES    		5
#define NVP_MAX_TRANSFER_BUFFERS	10
#define NVP_MAX_EXTRA422            10
#define NVP_MAX_SYNC_EVENTS         3   // defined also in nvpesrvc.h and nvpkrnl.c !!!

//
// Surface definition (Video or VBI) and related data
//
typedef struct _NVP_SURFACE_INFO
{
    //HANDLE              hSurface;           // DirectDraw surface handle
    ULONG               dwCaps;             // capabilites of this DD surface
    ULONG               pVidMem;            // Flat pointer to the surface
    ULONG               dwOffset;           // Offset from the base of the frame buffer
    ULONG               dwPitch;
    ULONG               dwLength;               
    ULONG               dwHeight;               
    ULONG               dwWidth;            // Ignored for VBI
    ULONG               dwStartLine;        // Programs Y-crop for image, and startline for vbi
    ULONG               dwPreScaleSizeX;    // Ignored for VBI.. prescale size width
    ULONG               dwPreScaleSizeY;    // Ignored for VBI.. prescale size height
    ULONG               dwFieldType;        // what field was this surface setup to capture?
    //ULONG               bAutoFlip;          // do we turn around and auto program this surface?
	ULONG				bIsFirstFieldCaptured;

    LONG                nNextSurfaceIdx;     // the next surface idx in the chain
    LONG                nPrevSurfaceIdx;     // the previous surface, used for FS Mirror

} NVP_SURFACE_INFO, *PNVP_SURFACE_INFO;

// Overlay info
typedef struct _NVP_OVERLAY {
	ULONG 	dwOverlayFormat;
	ULONG 	dwOverlaySrcX;
	ULONG 	dwOverlaySrcY;
	ULONG 	dwOverlaySrcPitch;
	ULONG 	dwOverlaySrcSize;
	ULONG 	dwOverlaySrcWidth;
	ULONG 	dwOverlaySrcHeight;
	ULONG 	dwOverlayDstWidth;
	ULONG 	dwOverlayDstHeight;
	ULONG   dwOverlayDstX;
    ULONG   dwOverlayDstY;
	ULONG   dwOverlayDeltaX;
	ULONG   dwOverlayDeltaY;
	ULONG   dwOverlayColorKey;
	ULONG   dwOverlayMode;
    ULONG   aExtra422OverlayOffset[NVP_MAX_EXTRA422];	// extra memory space used for prescaling
    ULONG   dwExtra422Index;			// index of current extra surface
    ULONG   dwExtra422NumSurfaces;      // number of extra allocated surfaces
	ULONG   dwExtra422Pitch;            // extra surface pitch
	ULONG   pNvPioFlipOverlayNotifierFlat;
	ULONG   dwOverlayMaxDownScale;
	ULONG   dwOverlayBufferIndex;
	ULONG 	dwReserved1;
	ULONG 	dwReserved2;
} NVP_OVERLAY, *PNVP_OVERLAY;

// Transfer
typedef struct _NVP_TRANSFER_BUFFER
{
	ULONG	dwTransferId;
	ULONG	dwTransferStatus;
	ULONG	pSurfaceData;
	//LONG	nSurfaceIdx;	// Idx of surface we're transfering from
	LONG	lStartLine;
	LONG 	lEndLine;
	ULONG	dwTransferFlags;
	ULONG	pDestMDL;
	LONG	nNextTransferIdx;
	LONG	nPrevTransferIdx;
} NVP_TRANSFER_BUFFER, *PNVP_TRANSFER_BUFFER;

// transfer status 
#define NVP_TRANSFER_QUEUED			0x01
#define NVP_TRANSFER_IN_PROGRESS	0x02
#define NVP_TRANSFER_DONE			0x03
#define NVP_TRANSFER_FAILURE		0x04
#define NVP_TRANSFER_FREE			0xFFFFFFFF

//
// NVidia Video Port context
//
typedef struct _NVP_CONTEXT
{
    NVP_SURFACE_INFO    aVideoSurfaceInfo[NVP_MAX_SURFACES];
    NVP_SURFACE_INFO    aVBISurfaceInfo[NVP_MAX_SURFACES];
 
    // Full screen mirror context information
    ULONG               dwOverlayFSOvlHead;    
    int                 MultiMon;               // True/False: whether there are multiple monitors
    NVP_SURFACE_INFO    aFSMirrorSurfaceInfo[NVP_MAX_SURFACES];  //Fullscreen mirror surface pointers 
    int                 nNumFSSurf;			    // number of allocated fullscreen mirror surfaces 
    int                 nFSFlipSurfIdx;         // current fullscreen mirror surface index 
    int                 nFSMirrorIdx;           // current completed
    int                 nFSFlipIdx;             // simple flip modifier for CRTC
    ULONG               regOverlayMode;         // pass these down from pDriver Data and through to kernel
    ULONG               regOverlayMode2;        // might even need zoom modes , and aspect ratios.
    ULONG               regOverlayMode3;
    ULONG               dwOverlayFSHead;        // which head is the overlay on? not sure if we need this
    ULONG               dwFsMirrorOn;           // signifies that Fsmirror is currently on.
    ULONG               dwDesktopState;
    UCHAR               adacNotifiers[16*9];    // (DAC notifiers) NvNotification 
    UCHAR               as2mNotifiers[16*1];    // s2m notifiers NvNotification
    UCHAR               asrfNotifiers[16*1];    // cont surface 2d notifiers , we need NVP_MAX_SURFACES
    
    int                 nNumVidSurf;			// number of allocated video surfaces
    int                 nNumVBISurf;			// number of allocated vbi surfaces
    int                 nVidSurfIdx;    		// current video surface index
    int                 nVBISurfIdx;    		// current vbi surface index

	int 				nNextVideoSurfId;
	int 				nNextVBISurfId;

	int 				nLastFieldId;			// Id of last field captured
	ULONG				bLastFieldType;			// type of last field captured (TRUE if VBI, FALSE if VIDEO)

	ULONG				dwVPState;				// NVP_STATE_STOPPED, NVP_STATE_STARTED
	ULONG				dwVPFlags;
	ULONG				dwVPConFlags;			// connection flags

	ULONG				dwChipId;				// chip version Id
	ULONG				hClient;
	ULONG				dwDeviceId;				// NV_WIN_DEVICE
    ULONG               pFrameBuffer;  // frame buffer base address
    ULONG               dwFBLen;       // frame buffer length

	ULONG				dwOverlayObjectID;		// overlay object Id (shared object!)
	ULONG				dwScalerObjectID;		// scaler object Id
	ULONG				dwDVDSubPicObjectID;	// used for prescaling on nv4 and nv5 
	ULONG               dwDVDObjectCtxDma;      // context dma used for synchronization.
	int					nFreeCount;		// fifo free count

    // BUGBUG: hacking declerations to avoid including hardware dependent header files 
    //         in "nv.h"
    ULONG               pNVPChannel;            // NvChannel *pNVPChannel; // VPE pio channel
    UCHAR               avpNotifiers[16*9];     // (Media port notifiers) NvNotification avpNotifiers[NVP_CLASS04D_MAX_NOTIFIERS];
    UCHAR               aovNotifiers[16*3];     // (Overlay notifiers) NvNotification aovNotifiers[NVP_CLASS07A_MAX_NOTIFIERS];

	// overlay
	NVP_OVERLAY			nvpOverlay;				// information retrieved from UpdateOverlay32() in Ring3
	ULONG				pOverlayChannel;		// NvChannel *pOverlayChannel; // Overlay channel
	ULONG				dwOvrSubChannel;
    ULONG                          pOvrRing0FlipFlag;

	// vidmem to sysmem transfer (vbi and video capture !)
	NVP_TRANSFER_BUFFER aTransferBuffers[NVP_MAX_TRANSFER_BUFFERS];
	UCHAR				axfNotifiers[16*6];	// 
	int					nCurTransferIdx;	// current transfer buffer index
	int					nLastTransferIdx;
	int					nRecentTransferIdx;	// index of most recently transfered buffer
	int					nNumTransfer;		// current number of transfer buffer in queue
	ULONG				bxfContextCreated;	// TRUE if context already created
	ULONG				bxfContextFlushed;	// TRUE if context flushed
	ULONG				dwTransferCtxStart;	// old transfer context start
	ULONG				dwTransferCtxSize;	// transfer size

	PVOID				hNVPSyncEvent[NVP_MAX_SYNC_EVENTS];		// media port and overlay sync event handle // PKEVENT hVSyncEvent;

	// DMA push buffer
	PVOID				pDmaChannel;
	ULONG				*nvDmaFifo;
	ULONG				nvDmaCount;
	ULONG				DmaPushBufTotalSize;	// 32k DWORDs
	UCHAR				DmaPushBufErrNotifier[16*1];

	// WDM prescale stuff
	ULONG				ulVPECaptureFlag;			//hack for WDM ME prescaling when capturing to disk
	ULONG				ulVPECapturePrescaleYFactor;	//hack for WDM ME prescale. Y-scale factor
	ULONG				ulVPECapturePrescaleXFactor;	//hack for WDM ME prescale. X-scale factor
	ULONG				ulVPNotUseOverSurf;			//flag for VP using over surf.

	//m2m xfer stuff
	PVOID				pSysBuf;				//dst surf for dma xfer
	ULONG				ulCap;					//capture flag

	ULONG				dwDecoderHeight;			//need this to determine single or dual field capture

} NVP_CONTEXT, *PNVP_CONTEXT;

#endif // _NVPECNTX_H