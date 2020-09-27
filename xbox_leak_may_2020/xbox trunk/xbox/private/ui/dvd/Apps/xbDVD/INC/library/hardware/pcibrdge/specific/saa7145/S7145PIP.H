
// FILE:      library\hardware\pcibridge\specific\saa7145\s7145pip.h
// AUTHOR:    Dietmar Heidrich, S. Herr
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   08.04.95
//
// PURPOSE: The PIP-related classes.
//
// HISTORY:

#ifndef SAA7145PIP_H
#define SAA7145PIP_H

#ifndef ONLY_EXTERNAL_VISIBLE

#include "..\lowlevel\memmpdio.h"
#include "..\lowlevel\dmabuffr.h"      
#include "..\videodma\gfxconfg.h"
#include "..\lowlevel\intrctrl.h"
#include "rps7145.h"

#endif


#include "..\videodma\dmapip.h"

//
// Maximum values for destination rectangle size.
//
#define PIP_WIDTH_MAX   768
#define PIP_HEIGHT_MAX  576


//
// PIP Display Mode (also valid for grabbing).
//
enum PIPDisplayMode
	{
	PDMD_MIN 						= 0,
	PDMD_EMULATED_INTERLACED	= 0,		// Interlaced display.
	PDMD_SINGLE_FIELD,						// Single field display.
	PDMD_AUTOMATIC,							// Automatic decision of display mode.
	PDMD_MAX = PDMD_AUTOMATIC
	};

//
// HFilter type (valid for PIP and grabbing).
//
enum PIPHFilterType
	{
	PIP_HF_CHOOSE_BEST 	= -1,				// Automatic selection of filter type.
	PIP_HF_L0C3  			= 0,				// The enum values correspond to the values of I20's HFilter register.
	PIP_HF_L3C3				= 1,
	PIP_HF_L4C4a 			= 2,
	PIP_HF_L5C4  			= 3,
	PIP_HF_L4C4b 			= 4,
	PIP_HFILTER_MIN 		= -1,
	PIP_HFILTER_MAX 		= 4
	};

enum PIPErrDiffusion
	{
	PIP_ED_ON 	= 0,
	PIP_ED_OFF 	= 1,
	PIP_ED_AUTOMATIC
	};


// Error definitions.
#define GNR_DMA_PIP_NOT_POSSIBLE		MKERR(ERROR, PIP, GENERAL, 0x01)		// Current screen mode does not allow DMA PIP
#define GNR_VIDEO_STD_NOT_SUPPORTED	MKERR(ERROR, PIP, GENERAL, 0x02)		// Desired video standard (e.g. SECAM) not supported.
#define GNR_PIP_NOT_RUNNING			MKERR(ERROR, PIP, GENERAL, 0x03)		// PIP is not running when trying to execute OptimizeBuffer()
#define GNR_NO_OPTIMAL_MINPIX			MKERR(ERROR, PIP, GENERAL, 0x04)		// No optimal minPix value could be found by OptimizeBuffer()
#define GNR_DEST_RECT_SIZE				MKERR(ERROR, PIP, GENERAL, 0x05)		// Destination rectangle too small or too big.
#define GNR_WRONG_GRAB_SIZE			MKERR(ERROR, PIP, GENERAL, 0x06)		// Dimensions of frame to grab are too big.
#define GNR_NO_VBLANK_IRQ				MKERR(ERROR, PIP, GENERAL, 0x07)		// An expected VBlank IRQ did not occur.
#define GNR_GRAB_TIMEOUT				MKERR(ERROR, PIP, GENERAL, 0x08)		// A timeout occurred during grabbing a frame.
#define GNR_WRONG_GRAB_FORMAT			MKERR(ERROR, PIP, GENERAL, 0x09)		// An unsupported capture format was requested
#define GNR_SAMPLEMODE_NOT_SUPPORTED	MKERR(ERROR, PIP, GENERAL, 0x0a)		// Desired video sample mode not supported

#ifndef ONLY_EXTERNAL_VISIBLE

#define VBLANK_TIMEOUT 100				// Time in [ms] until a VSYNC IRQ must have occurred.
#define GRAB_TIMEOUT	  1000			// Time in [ms] until grabbing of one frame must be finished.

struct SAA7145PIPSetting {
	DWORD			vdba;
	DWORD			vdpctrl1;
	DWORD			vdmac;
	DWORD			vpctl;
	DWORD			xwc;
	DWORD			ywc;
	DWORD			fsctl;
	DWORD			ypr;
	DWORD			cdba;
	DWORD			cdf;
	DWORD			cdmac;
	};

class SAA7145PIPVBlankIntHandler;			// Forward declaration of IRQ handler class.

//
// Physical SAA7145 Masked DMA PIP class.
//
class SAA7145PIP : public MaskDMAPIP
	{
	friend class VirtualSAA7145PIP;
	friend class SAA7145PIPVBlankIntHandler;

	protected:
		MemoryMappedIO			*	io;						// IO object for SAA7145 PIP registers.

		GraphicConfig			*	gfxConfig;				// Stores information about current graphics card and screen mode.
		ContinuousDMABuffer 		maskBuffer;				// mask DMA buffer
		DWORD							maskPhysical;

		Profile					*	profile;

		BOOL 							maskEnabled;			// Indicates use of mask information. If false, the DMA window
																	// is always on top.
		BOOL							grabMode;				// The PIP object is in grabbing mode, when this flag is TRUE.

		HardVideoParams 			ntsc, pal; 
		const HardVideoParams* 	curVideoParams;		// Pointer to the settings of the current video standard.

		VideoStandard 				curVideoStd;			// Contains the current video standard.
		VideoSampleMode			sampleMode;
		int							hOffset, vOffset;
		DWORD							dispgrab;

		WORD	curMaskDwpr;									// Number of DWORDs per mask row.

		BOOL	extFieldInd;									// External field indication.
		BOOL	topField;										// Top field indication.
		BIT	vclkPolarity;									// VCLK polarity.

		int	srcLeft, srcTop;
		WORD	srcWidth, srcHeight;	// Dimensions of the source rectangle.

		WORD	ConvertGrabFormat(GrabFormat format);	// Converts destination format to the YUV2RGB register value. 
		WORD	ConvertPIPFormat(PIPFormat format);		// Same for PIP format. In both functions packed24 is set
																	// in respect to 24 or 32 bit mode.
		int	GetPixelSize (GrabFormat type);			// Get size of a pixel for a certain format.

		Error WaitVBlankIRQ(WORD count);					// Wait for the number of "count" VSYNC IRQs to occur.

		BOOL	adaptSourceSize;								// Tells, if the source rectangle is adapted to the video
																	// standard.
		SAA7145PIPVBlankIntHandler	* vBlankIntHandler;	// VBlank handler.
		InterruptServer 			* vBlankIntServer;	// Interrupt server.

		SAA7145PIPSetting				pipSetting, grabSetting;
		SAA7145PIPSetting	__far	*	currentPIPSetting;

		ContinuousDMABuffer	*	captureDMA;

		FPTR							captureDMABuffer;
		DWORD							captureDMADWordsPerRow;

		HPTR							captureBuffer;
		DWORD							captureDWordsPerRow;

		WORD							captureHeight;

		StreamCaptureHook		*	captureHook;
		YStreamCaptureHook	*	yCaptureHook;

		RPSProgram *pipEnableRPS;

	protected:
		int dmaSuspended;										// Internal DMA suspension flag.

		void 	VBlankInterrupt(void);						// VBlank (VSYNC) IRQ routine.
		DWORD volatile vBlankCount;						// Tells us, how many vertical blank IRQs happened.

		Error StopDMA(void);									// Stops DMA.
		Error StartDMA(void);								// Starts DMA.
		Error EnablePIP(BOOL enable);						// Enables PIP in general.

		Error Update(void);									// Updates all PIP parameters.

		virtual Error CalculateSettings (SAA7145PIPSetting __far &setting,
													DWORD dstBase, DWORD bytesPerRow,
													PIPFormat pixelFormat,
													BOOL mask,
													int left, int top,
													WORD width, WORD height);

		Error ProgramSettings(SAA7145PIPSetting __far &setting);

		Error ProgramAll(void);								// Reprograms all DMA PIP/grab registers of the SAA7145.

		Error SetMask(RectBitMap * map);
		Error SetSourceRect(int left, int top, int width, int height);
		Error SetVideoStandard(VideoStandard std);
		Error SetSampleMode(VideoSampleMode sampleMode);
		Error SetHOffset(int offset);
		Error SetVOffset(int offset);
		Error SetMaskEnable(BOOL enable);
		Error SetAdaptSourceSize(BOOL adaptSourceSize);
		Error SetExtFieldInd(BOOL enable);
		Error SetTopField(BOOL field);

		Error GrabFrame(FPTR base, WORD width,  WORD height, 		   // Grab a single frame from the running picture.
							WORD stride, GrabFormat fmt);

		Error StartGrabStream(StreamCaptureHookHandle hook,
					             WORD width, WORD height, 
					             WORD stride, 
					             GrabFormat fmt);

		Error SupplyStreamGrabBuffer(HPTR buffer);

		HPTR	GetStreamGrabBuffer (void);

		Error StopGrabStream(void);

		Error StartGrabYStream(YStreamCaptureHookHandle hook,
		                       int left, int top,
		                       WORD width, WORD height);

		Error StopGrabYStream(void);

		int   HorizontalAlign(void);						// Delivers the alignment for SAA7145 PIP (destination is always
																	// DWORD aligned, so e.g. 24 bit modes need a different alignment
																	// than 16 or 15 bit).
		int   VerticalAlign(void);							// Same for vertical.

		int MaxDestWidth(void) {return srcWidth;}		// Returns maximum horizontal size of the destination rectangle.
		int MaxDestHeight(void) {return srcHeight;}	// Maximum vertical size of the destination rectangle.

		RectBitMap *CreateDMAMap(int left, int top, int width, int height)
			{return new InverseRectBitMap(left, top, width, height);}			// We need an inverse bitmap for the SAA7145!
	public:
		SAA7145PIP (HINSTANCE hinst, MemoryMappedIO *io, InterruptServer *vblankIntServer,
		            Profile *profile);
		~SAA7145PIP(void);

		VirtualUnit *CreateVirtual(void);

		Error ReadConfig (char *filename);				// Allows to read advanced parameters from a configuration file.
		Error Initialize(void);								// Initializes the SAA7145 PIP. Call after the object was constructed.

		Error SuspendDMA(void);								// Nested DMA disabe.
		Error ResumeDMA(void);								// Nested DMA enable.
	};


//
// Virtual SAA7145 PIP.
//
class VirtualSAA7145PIP : public VirtualMaskDMAPIP 
	{
	friend class SAA7145PIP;

	private:
		SAA7145PIP *unit;

	protected:
		BOOL					maskEnabled;
		BOOL					extFieldInd;
		BOOL					topField;

		Error Preempt (VirtualUnit *previous);
	public:
		VirtualSAA7145PIP (SAA7145PIP *physical);
		Error Configure (TAG __far *tags);
	};


#endif // ONLY_EXTERNAL_VISIBLE


// SAA7145 PIP specific tags                     
MKTAG (PIP_H_FILTER,     	PIP_UNIT, 0x00000030, PIPHFilterType)	// Horizontal filter type selection.
MKTAG (PIP_DISPLAY_MODE, 	PIP_UNIT, 0x00000031, PIPDisplayMode) 	// DispMode selection.
MKTAG (PIP_DUP_FIELD,    	PIP_UNIT, 0x00000032, BOOL)				// DupField switch.
MKTAG (PIP_ERR_DIFFUSION,	PIP_UNIT, 0x00000034, PIPErrDiffusion)	// Error diffusion type selection.
MKTAG (PIP_MIN_PIX,      	PIP_UNIT, 0x00000035, int)					// MinPix value (FIFO threshold).
MKTAG (PIP_MASK_ENABLE,	 	PIP_UNIT, 0x00000036, BOOL)				// Mask enable/disable switch.
MKTAG (PIP_EXT_FIELD_IND,  PIP_UNIT, 0x00000037, BOOL)				// ExtFI mode selection.
MKTAG (PIP_TOP_FIELD,      PIP_UNIT, 0x00000038, BOOL)				// Top field interpretation selection.
MKTAG (PIP_MINPIX_AUTO,		PIP_UNIT, 0x00000039, BOOL)				// Automatic adaption of MinPix to picture size.

#endif
