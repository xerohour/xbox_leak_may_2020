
// FILE:      library\hardware\videodma\specific\i20pip.h
// AUTHOR:    Dietmar Heidrich, S. Herr
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   08.04.95
//
// PURPOSE: The PIP-related classes.
//
// HISTORY:

#ifndef I20PIP_H
#define I20PIP_H

#ifndef ONLY_EXTERNAL_VISIBLE

#include "..\..\..\lowlevel\memmpdio.h"
#include "..\..\..\lowlevel\dmabuffr.h"      
#include "..\..\..\lowlevel\intrctrl.h"
#include "..\..\..\common\profiles.h"
#include "..\..\video\generic\videvice.h"

#if NT_KERNEL || VXD_VERSION
#include "..\..\..\lowlevel\kernlmem.h"
#else
#include "..\generic\gfxconfg.h"
#endif

#endif


#include "..\generic\dmapip.h"

//
// Maximum values for destination rectangle size.
//
#define PIP_WIDTH_MAX   768
#define PIP_HEIGHT_MAX  576

//
// Number of test cases for minPix detection.
//
#define NUM_TEST_CASES 3

//
// Structures for I20 PIP DMA configuration.
//
struct I20PIPConfig
	{ 
	int	minPixMasked[NUM_TEST_CASES],
			minPixNoMask[NUM_TEST_CASES],
			latencyMasked[NUM_TEST_CASES],
			latencyNoMask[NUM_TEST_CASES];
	};

struct I20PIPSingleConfig
	{ 
	int	minPixMasked,
			minPixNoMask,
			latencyMasked,
			latencyNoMask;
	};

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


#ifndef ONLY_EXTERNAL_VISIBLE

#define VBLANK_TIMEOUT 100				// Time in [ms] until a VSYNC IRQ must have occurred.
#define GRAB_TIMEOUT	  1000			// Time in [ms] until grabbing of one frame must be finished.

struct I20PIPSetting {
	DWORD			htiming;
	DWORD			vtiming;
	DWORD			vidconfig;
	DWORD	      vidtopbase;
	DWORD			vidbotbase;
	DWORD			dispgrab;
	DWORD			vidwin;
	DWORD			masktopbase;       
	DWORD			maskbotbase;
	DWORD			maskctrl;
	};
	
class I20PIPVBlankIntHandler;			// Forward declaration of IRQ handler class.

//
// Physical I20 Masked DMA PIP class.
//
class I20PIP : public MaskDMAPIP	
	{
	friend class VirtualI20PIP;
	friend class I20PIPVBlankIntHandler;

	protected:
		MemoryMappedIO			*	io;						// IO object for I20 PIP registers.
                                 
#if NT_KERNEL || VXD_VERSION
#else
		GraphicConfig			*	gfxConfig;				// Stores information about current graphics card and screen mode.
#endif
		ContinuousDMABuffer 		maskBuffer;				// mask DMA buffer
		DWORD							maskPhysical;

		I20PIPConfig			*	config;					// Stores configuration information (MinPix etc.)
		Profile					*	profile;

		PIPHFilterType				horizFilter;			// Current horizontal filter type for PIP or grabbing.
		PIPDisplayMode				dispMode;				// Current display mode (interlace / single field)
		BOOL							dupField;				// Source interlaced or both fields equal.
		PIPErrDiffusion			errDiffusion;			// not in config file !
		int 							minPix;					// Default FIFO threshold value (Minimum number of DWORDs).
		BOOL							minPixAuto;				// Tells if automatic MinPix adaption desired.

		BOOL							fieldToggle;			// Tells if interlaced should be simulated by toggling fields
		BOOL							fieldToggleRunning;

		BOOL 							maskEnabled;			// Indicates use of mask information. If false, the DMA window
																	// is always on top.
      BOOL							grabMode;				// The PIP object is in grabbing mode, when this flag is TRUE.
      
		const HardVideoParams* 	curVideoParams;		// Pointer to the settings of the current video standard.

		VideoStandard 				curVideoStd;			// Contains the current video standard.
		VideoSampleMode			sampleMode;
		int							hOffset, vOffset;
		WORD							activeLeft, activeTop, activeWidth, activeHeight;
		int							hTrigger;
		BIT							hsyncPol, vsyncPol;
		VideoFieldIndication		fieldIndication;
		BOOL							pixelClockEdge;
		      
		DWORD							dispgrab;		
		DWORD							lastStopTime;
		WORD	curMaskDwpr;									// Number of DWORDs per mask row.

		BOOL	extFieldInd;									// External field indication.
		BOOL	topField;										// Top field indication.
		
		int	srcLeft, srcTop;
		WORD  srcWidth, srcHeight;							// Dimensions of the source rectangle.

		WORD	ConvertGrabFormat(GrabFormat format);	// Converts destination format to the YUV2RGB register value. 
		WORD	ConvertPIPFormat(PIPFormat format);		// Same for PIP format. In both functions packed24 is set
																	// in respect to 24 or 32 bit mode.
		int	GetPixelSize (GrabFormat type);			// Get size of a pixel for a certain format.

		int	srcCropLeft, srcCropTop, srcCropRight, srcCropBottom;

		Error WaitVBlankIRQ(WORD count);					// Wait for the number of "count" VSYNC IRQs to occur.

		BOOL	adaptSourceSize;								// Tells, if the source rectangle is adapted to the video
																	// standard.
		I20PIPVBlankIntHandler	* vBlankIntHandler;	// VBlank handler.
		InterruptServer 			* vBlankIntServer;	// Interrupt server.

		I20PIPSetting				pipSetting, grabSetting;		
		I20PIPSetting	__far	*	currentPIPSetting;

		
		ContinuousDMABuffer	*	captureDMA;
		
		FPTR							captureDMABuffer;
		DWORD							captureDMADWordsPerRow;

		PrepInfoPIP					*captureBuffer;
		DWORD							captureDWordsPerRow;

		BOOL							capturedInBuffer;

		StreamCaptureHookHandle		captureHook;


#if NT_KERNEL || VXD_VERSION
		KernelMemory				yCaptureKHandle;
		HPTR							yCaptureKBuffer;
#endif
		DWORD							yCaptureDMADWordsPerRow;
//		DWORD							yCaptureDWordsPerRow;
		WORD							yCaptureHeight;
		FPTR							yCaptureBuffer;
		YStreamCaptureHookHandle	yCaptureHook;
		WORD							yCaptureCounter;
		
		BOOL							tritonFix;

	   DWORD							physicalBase;
		WORD							bytesPerRow;
		PIPFormat					pixelFormat;
      
#if VIONA_VERSION		      
		BOOL							offscreenOverride;
		DWORD							offscreenPhysicalBase, offscreenBytesPerRow;
		PIPFormat					offscreenPixelFormat;
		BOOL							offscreenBobbing;
				
		BOOL							forceSingleField;				
		WORD							maxOverlayBytesPerRow[PIPFORMAT_MAX + 1];
#endif
		FieldDisplayHookHandle	fieldDisplayHook;
	protected:
		int dmaSuspended;										// Internal DMA suspension flag.

		void 	VBlankInterrupt(void);						// VBlank (VSYNC) IRQ routine.
		DWORD volatile vBlankCount;						// Tells us, how many vertical blank IRQs happened.

		Error StopDMA(void);									// Stops DMA.
		Error StartDMA(void);								// Starts DMA.
		Error EnablePIP(BOOL enable);						// Enables PIP in general.

		Error Update(void);									// Updates all PIP parameters.
		
		virtual Error CalculateSettings(I20PIPSetting __far &setting, 
													DWORD dstBase, DWORD bytesPerRow,
													PIPFormat pixelFormat,
													BOOL mask,
													int left, int top,
													WORD width, WORD height,
													BOOL bobbing = FALSE);
		Error CalculatePIPSettings(void);													

		Error ProgramSettings(I20PIPSetting __far &setting);
		
		Error ProgramAll(void);								// Reprograms all DMA PIP/grab registers of the I20.

		Error SetMask(RectBitMap * map);

#if NT_KERNEL || VXD_VERSION
	public:
#endif
#if VIONA_VERSION
		Error SetOffscreenDMARegion(DWORD physical, DWORD stride, PIPFormat format,
		                            int left, int top, int width, int height);
#endif
		
	protected:
		Error SetSourceRect(int left, int top, int width, int height);
		Error SetVideoStandard(VideoStandard std);	
		Error SetSampleMode(VideoSampleMode sampleMode);
		Error SetHOffset(int offset);
		Error SetVOffset(int offset);
		Error SetHorizFilter(PIPHFilterType horizFilter);
		Error SetDispMode(PIPDisplayMode dispMode);
		Error	SetDupField(BOOL dupField);
		Error SetErrDiffusion(PIPErrDiffusion errDiffusion);
		Error	SetMinPix(int minPix);
		Error SetMaskEnable(BOOL enable);
		Error SetAdaptSourceSize(BOOL adaptSourceSize);
		Error SetExtFieldInd(BOOL enable);
		Error SetTopField(BOOL field);
		Error SetMinPixAuto(BOOL minPixAuto);
		Error SetFieldToggle(BOOL fieldToggle);
		Error SetFieldDisplayHook(FieldDisplayHookHandle hook);

		Error SetSourceCrop(int left, int top, int right, int bottom);

		Error GetPhysicalFromPointer(WORD selector, DWORD offset, DWORD __far& physical);
				
		Error	OptimizeBuffer(int __far & minPixVal);	// Calculates an optimal minpix value for the current
																	// PIP parameters. DMA must be on when calling this function.
		Error DetectMinPix(int __far & minPixVal);	// Helper subroutine for minpix detection.
		Error ExtendedOptimization(I20PIPSingleConfig & config);		// Extended Optimization routine.

		Error GrabFrame(FPTR base, WORD width,  WORD height, 		   // Grab a single frame from the running picture.
		                        	WORD stride, GrabFormat fmt);


		virtual Error InitGrabStream (StreamCaptureHookHandle hook, DWORD milliSecPerFrame,
		                              WORD width, WORD height, WORD stride, GrabFormat format);
		virtual Error FinishGrabStream (void);
		virtual Error StartGrabStream (void);
		virtual Error StopGrabStream (void);


		virtual Error StartGrabYStream (YStreamCaptureHookHandle hook, int left, int top, WORD width, WORD height);
		virtual Error StopGrabYStream (void);


		Error StartFieldToggle(void);
		Error StopFieldToggle(void);
		
		Error	StoreConfig(I20PIPConfig* config);		// Store configuration
		Error RetrieveConfig(I20PIPConfig* config);	// Retrieve configuration

		int   HorizontalAlign(void);						// Delivers the alignment for I20 PIP (destination is always
																	// DWORD aligned, so e.g. 24 bit modes need a different alignment
																	// than 16 or 15 bit).
		int   VerticalAlign(void);							// Same for vertical.

#if VIONA_VERSION
		int MaxOverlayDestWidth(void);
		int MaxInlayDestWidth(void);
		int MaxDestWidth(void) {return MaxInlayDestWidth();}
#else
		int MaxDestWidth(void) {return (activeWidth ? min(activeWidth, srcWidth) : srcWidth) - srcCropLeft - srcCropRight;}
#endif
		int MaxDestHeight(void);
		
		RectBitMap* CreateDMAMap(int left, int top, int width, int height)
			{return new InverseRectBitMap(left, top, width, height);}			// We need an inverse bitmap for the I20!
	public:
#if NT_KERNEL || VXD_VERSION
		I20PIP (MemoryMappedIO *io, InterruptServer * vblankIntServer,
				  Profile * profile, BOOL tritonFix,
				  DWORD physicalBase, WORD bytesPerRow, PIPFormat pixelFormat);
#else
		I20PIP (HINSTANCE hinst, MemoryMappedIO *io, InterruptServer * vblankIntServer,
				  Profile * profile, BOOL tritonFix);
#endif
		~I20PIP(void);

		Error Configure(TAG __far * tags);
		
		VirtualUnit * CreateVirtual(void);
		
		Error Initialize(void);								// Initializes the I20 PIP. Call after the object was constructed.
		
		Error SuspendDMA(void);								// Nested DMA disabe.
		Error ResumeDMA(void);								// Nested DMA enable.

#if NT_KERNEL || VXD_VERSION
		virtual Error SetPIPBases (DWORD physicalBase, WORD bytesPerRow, PIPFormat pixelFormat);
#elif VIONA_VERSION		
		Error Reconfigure(void);
		BOOL	ScreenModeChanged(void);
#endif

		HardVideoParams 			ntscSQP, palSQP, ntscCCIR, palCCIR; 
		BIT							vclkPolarity;									// VCLK polarity.
	};


//
// Virtual I20 PIP.
//
class VirtualI20PIP : public VirtualMaskDMAPIP 
	{
	friend class I20PIP;

	private:
		I20PIP* unit;

	protected:
		PIPHFilterType 	horizFilter;
		PIPDisplayMode 	dispMode;

		BOOL 					dupField;
		int 					minPix;
		PIPErrDiffusion	errDiffusion;
		BOOL					maskEnabled;
		BOOL					extFieldInd;
		BOOL					topField;
		BOOL					minPixAuto;
		BOOL					fieldToggle;
   
		Error Preempt (VirtualUnit* previous);
	public:
		VirtualI20PIP (I20PIP* physical);
		Error Configure (TAG __far * tags);
		
		Error OptimizeBuffer(int __far& minPixVal);	// Runs an optimization for the I20 FIFO buffer size.
																	// Returns an optimal value for the PIP_MIN_PIX tag parameter.

#if WDM_VERSION
		Error PassivateDone() {GNRAISE_OK;}
#endif
	};


#endif // ONLY_EXTERNAL_VISIBLE


// I20 PIP specific tags                     
MKTAG (PIP_H_FILTER,     	PIP_UNIT, 0x00000030, PIPHFilterType)	// Horizontal filter type selection.
MKTAG (PIP_DISPLAY_MODE, 	PIP_UNIT, 0x00000031, PIPDisplayMode) 	// DispMode selection.
MKTAG (PIP_DUP_FIELD,    	PIP_UNIT, 0x00000032, BOOL)				// DupField switch.
MKTAG (PIP_ERR_DIFFUSION,	PIP_UNIT, 0x00000034, PIPErrDiffusion)	// Error diffusion type selection.
MKTAG (PIP_MIN_PIX,      	PIP_UNIT, 0x00000035, int)					// MinPix value (FIFO threshold).
MKTAG (PIP_MASK_ENABLE,	 	PIP_UNIT, 0x00000036, BOOL)				// Mask enable/disable switch.
MKTAG (PIP_EXT_FIELD_IND,  PIP_UNIT, 0x00000037, BOOL)				// ExtFI mode selection.
MKTAG (PIP_TOP_FIELD,      PIP_UNIT, 0x00000038, BOOL)				// Top field interpretation selection.
MKTAG (PIP_MINPIX_AUTO,		PIP_UNIT, 0x00000039, BOOL)				// Automatic adaption of MinPix to picture size.
MKTAG (PIP_FIELDTOGGLE,		PIP_UNIT, 0x0000003a, BOOL)				// Simulate interlaced with single field by toggling fields


#endif
