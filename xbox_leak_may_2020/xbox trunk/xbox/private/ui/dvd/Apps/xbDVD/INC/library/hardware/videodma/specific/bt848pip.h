// FILE:      videodma (BT848)/ VIDMABTR.H
// AUTHOR:    P.C.Straasheijm
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   09.10.96
//
// PURPOSE: The PIP-related classes.
//
// HISTORY:

#ifndef BT848PIP_H
#define BT848PIP_H

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\lowlevel\memmpdio.h"
#include "library\lowlevel\dmabuffr.h"      
#include "library\lowlevel\intrctrl.h"
#include "library\common\profiles.h"    
#include "library\support\gfxdebug\gfxdebug.h"
#include "library\general\lists.h" 

#if !NT_KERNEL
#include "library\hardware\videodma\generic\gfxconfg.h"
#else
#include "library\lowlevel\kernlmem.h"
#endif

#include "library\hardware\videodma\generic\dmapip.h"
#include "library\hardware\pcibrdge\specific\bt848\bt848reg.h" 
#include "library\hardware\pcibrdge\specific\bt848\framlist.h"

#endif


//
// Maximum values for destination rectangle size.
//
#define PIP_WIDTH_MAX   768
#define PIP_HEIGHT_MAX  576  


#ifndef ONLY_EXTERNAL_VISIBLE

//
//number of riscinstructionbytes-prefetch of Brooktree
//

#define PREFETCH 0x20;

//
// Number of test cases for minPix detection.
//
#define NUM_TEST_CASES 3

//
// Structures for BT848 PIP DMA configuration.
//
struct BT848PIPConfig
	{
	WORD threshold;
	WORD burstlength;
	};
          

struct BT848PIPSingleConfig
	{
	WORD threshold;
	WORD burstlength;
	};

// for grabstream routines ;

#endif	 

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

enum PIPErrDiffusion
	{
	PIP_ED_ON 	= 0,
	PIP_ED_OFF 	= 1,
	PIP_ED_AUTOMATIC
	};
          
          
enum PIPHFilterType          
	{
	AUTO_HFILT 	=0,
	CIF			=1,
	QCIF			=2,
	ICON			=3
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

struct BT848PIPSetting {
	BYTE			hdelay_O;
	BYTE			vdelay_O;
	BYTE			hactive_O;
	BYTE	      vactive_O;   
	BYTE			msbcrobb_O;
	BYTE			hhscaling_O;
	BYTE			lhscaling_O;
	BYTE			hvscaling_O;
	BYTE			lvscaling_O;
	BYTE			color_fm;   
	BYTE			capturecontrol;
   BYTE			vert_filter;
	WORD			GPIODMAControl;
   DWORD			DMABase;
	};



class BT848PrepInfoPIP : public PrepInfoPIP
	{
	friend class BT848PIP;
	protected:
		WORD width, height;
		DWORD bytesPerLine;
		ContinuousDMABuffer *riscbuffer;
		ScatteredDMABuffer *ram;
		DWORD riscphysical;
		DWORD __huge *risclogicjmp;
		BT848PIP		*ptr848;
	public:
		BT848PrepInfoPIP (BT848PIP *ptr848, WORD width, WORD height, DWORD bytesPerLine);

		virtual Error Prepare (FPTR buffer, DWORD size);
		virtual Error Unprepare (void);
	};


	
class BT848PIPVBlankIntHandler;			// Forward declaration of IRQ handler class.

//
// Physical BT848 Masked DMA PIP class.
//
class BT848PIP : public DMAPIP	
	{
	friend class VirtualBT848PIP;
	friend class BT848PIPVBlankIntHandler;

	protected:
		MemoryMappedIO			*	io;						// IO object for I20 PIP registers.
                                 
#if NT_KERNEL
#else
		GraphicConfig			*	gfxConfig;				// Stores information about current graphics card and screen mode.
#endif
		FrameList				* framelist;
		DWORD						riscPhysical;

		BT848PIPConfig			* config;					// Stores configuration information (MinPix etc.)
		Profile					* profile;

		PIPDisplayMode			dispMode;					// Current display mode (interlace / single field)
		BOOL						dupField;					// Source interlaced or both fields equal.
		PIPErrDiffusion		errDiffusion;				// not in config file !

		VirtualUnit 			* videoBus;
		
		BOOL						fieldToggle;				// Tells if interlaced should be simulated by toggling fields
		BOOL						fieldToggleRunning;
      
		BOOL						maskEnabled;				// Indicates use of mask information. If false, the DMA window
																	// is always on top.
      BOOL						grabMode;					// The PIP object is in grabbing mode, when this flag is TRUE.
      BOOL						autoOverride;
      
		HardVideoParams	* 	curVideoParams;		// Pointer to the settings of the current video standard.

		VideoStandard 			curVideoStd;			// Contains the current video standard.
		VideoSampleMode		sampleMode;
		int						hOffset, vOffset;
      
		PIPHFilterType			horizFilter;
		
		BOOL						minPixAuto;
		int 						minPix;
		
		BOOL	extFieldInd;								// External field indication.
		BOOL	topField;									// Top field indication.
		
		int	srcLeft, srcTop;
		WORD  srcWidth, srcHeight;							// Dimensions of the source rectangle.

		WORD	ConvertGrabFormat(GrabFormat format);		// Converts destination format to the YUV2RGB register value. 
		WORD	ConvertPIPFormat(PIPFormat format);			// Same for PIP format. In both functions packed24 is set
															// in respect to 24 or 32 bit mode.
		int	GetPixelSize (GrabFormat type);					// Get size of a pixel for a certain format.

		Error WaitVBlankIRQ(WORD count);					// Wait for the number of "count" VSYNC IRQs to occur.

		BOOL	adaptSourceSize;							// Tells, if the source rectangle is adapted to the video
															// standard.
		BT848PIPVBlankIntHandler	* vBlankIntHandler;			// VBlank handler.
		InterruptServer 				* vBlankIntServer;			// Interrupt server.

		BT848PIPSetting				pipSetting, grabSetting;		
		BT848PIPSetting	__far	*	currentPIPSetting;
		
		PIPFormat					overrPixelFormat;
		
		ContinuousDMABuffer	*	captureDMA;
		FPTR							captureDMABuffer;
		DWORD							captureDMADWordsPerRow;
		
		WORD							captureHeight;
				
		DWORD						__huge  *riscjumpadd;
			
		DWORD							captureDWordsPerRow;

		BOOL							capturedInBuffer;

#if NT_KERNEL || VXD_VERSION
		KernelMemory				captureKHandle;
		HPTR							captureKBuffer;
#endif

		StreamCaptureHookHandle			captureHook;
		YStreamCaptureHookHandle		yCaptureHook;
		WORD							yCaptureCounter;
		
	   ContinuousDMABuffer		*riscBuffer;
	   
	   DWORD							overrPhysicalBase;
	   WORD							overrBytesPerRow;
	   DWORD							physicalBase;
		WORD							bytesPerRow;
		PIPFormat					pixelFormat;
#if VIONA_VERSION		      
		BOOL							offscreenOverride;
		DWORD							offscreenPhysicalBase, offscreenBytesPerRow;
		PIPFormat					offscreenPixelFormat;
				
		WORD							maxOverlayBytesPerRow[PIPFORMAT_MAX + 1];
#endif      

	protected:
		int dmaSuspended;									// Internal DMA suspension flag.

		void  VBlankInterrupt(void);						// VBlank (VSYNC) IRQ routine.
		DWORD volatile vBlankCount;							// Tells us, how many vertical blank IRQs happened.

		Error StopDMA(void);								// Stops DMA.
		Error StartDMA(void);								// Starts DMA.
		Error EnablePIP(BOOL enable);						// Enables PIP in general.

		Error Update(void);									// Updates all PIP parameters.
		
		virtual Error CalculateSettings(BT848PIPSetting __far &setting, 
													DWORD dstBase, DWORD bytesPerRow,
													PIPFormat pixelFormat,
													BOOL mask,
													int left, int top,
													WORD width, WORD height);

		Error CalculatePIPSettings(void);
		Error ProgramSettings(BT848PIPSetting __far &setting);
		Error SetMask(FrameList * map);                               
#if VIONA_VERSION		
		Error SetOffscreenDMARegion(DWORD physical, DWORD stride, PIPFormat format,
		                            int left, int top, int width, int height);
#endif
		Error SetSourceRect(int left, int top, int width, int height); 
		Error SetVideoStandard(VideoStandard std);	 			
		Error SetSampleMode(VideoSampleMode sampleMode);
		Error SetHOffset(int offset);
		Error SetVOffset(int offset);
		Error SetHorizFilter(PIPHFilterType horizFilter);
		Error SetDispMode(PIPDisplayMode dispMode);
		Error SetDupField(BOOL dupField);
		Error SetErrDiffusion(PIPErrDiffusion errDiffusion);
		Error SetMinPix(int minPix);
//		Error SetMaskEnable(BOOL enable);
		Error SetAdaptSourceSize(BOOL adaptSourceSize);
		Error SetExtFieldInd(BOOL enable);
		Error SetTopField(BOOL field);
		Error SetMinPixAuto(BOOL minPixAuto);
		Error SetFieldToggle(BOOL fieldToggle);

		Error SetAutoOverride(BOOL autoOverride);
		Error SetPhysical(void __far * physicalBase);
		Error SetPhysicalLinear(DWORD physicalBaseLinear);
		Error SetBytesPerRow(WORD bytesPerRow);
		Error SetPixelFormat(PIPFormat pixelFormat);

		Error GetPhysicalFromPointer(WORD selector, DWORD offset, DWORD __far& physical);
				
		Error OptimizeBuffer(int __far & minPixVal);	// Calculates an optimal minpix value for the current
																	// PIP parameters. DMA must be on when calling this function.
		Error DetectMinPix(int __far & minPixVal) {GNRAISE_OK;}	// Helper subroutine for minpix detection.
		Error ExtendedOptimization(BT848PIPSingleConfig & config){GNRAISE_OK;}		// Extended Optimization routine.
		Error GrabFrame(FPTR base, WORD width,  WORD height, 		   // Grab a single frame from the running picture.
		                        	WORD stride, GrabFormat fmt); 


		virtual Error InitGrabStream (StreamCaptureHookHandle hook, DWORD milliSecPerFrame,
		                              WORD width, WORD height, WORD stride, GrabFormat format);
		virtual Error FinishGrabStream (void);
		virtual Error StartGrabStream (void);
		virtual Error StopGrabStream (void);
		virtual PrepInfoPIP *CreatePrepInfo (void);

		typedef struct CaptureParameters2
			{
			DWORD bytesPerLine;
			BT848PrepInfoPIP *captureBuffer;
			BOOL capturedInBuffer;
			DWORD next;
			BT848PrepInfoPIP *captureBufferNext;
			BT848PrepInfoPIP *last;
			BOOL NewBuffer;
			DWORD captureTime;
			};
		CaptureParameters2 capture2;

      HPTR  captureBuffer;	
		Error SupplyStreamGrabBuffer(HPTR buffer);
		
		FPTR	GetStreamGrabBuffer(void);
							             
		Error StartGrabYStream(YStreamCaptureHookHandle hook,
		                       int left, int top,
		                       WORD width, WORD height);

		Error StopGrabYStream(void);		
		Error StartFieldToggle(void){GNRAISE_OK;}
		Error StopFieldToggle(void){GNRAISE_OK;}
		
		Error	StoreConfig(BT848PIPConfig* config);		// Store configuration
		Error RetrieveConfig(BT848PIPConfig* config);	// Retrieve configuration

 		int   HorizontalAlign(void);						// Delivers the alignment for BT848PIP (destination is always
																	// DWORD aligned, so e.g. 24 bit modes need a different alignment
																	// than 16 or 15 bit).
		int   VerticalAlign(void);							// Same for vertical.

		int 	MaxDestWidth(void) {return srcWidth;}		// Returns maximum horizontal size of the destination rectangle.
		int 	MaxDestHeight(void) {return srcHeight;}		// Maximum vertical size of the destination rectangle.
		
		Error InitDMARegion(int left, int top, int width, int height);
		Error AddDMAClipRect(int left, int top, int width, int height);
		Error RemDMAClipRect(int left, int top, int width, int height);
		Error CompleteDMARegion(DWORD __huge* &riscjumpadd);
		Error CompleteDMARegion(void) {return CompleteDMARegion(riscjumpadd);}
		int 	HorizontalPixelsPerMaskBit (void);
		int 	VerticalPixelsPerMaskBit (void);		
		
			
	public:
#if NT_KERNEL
		BT848PIP (MemoryMappedIO *io, InterruptServer * vblankIntServer,
				  Profile * profile, 
				  DWORD physicalBase, WORD bytesPerRow, PIPFormat pixelFormat);
#else
		BT848PIP (HINSTANCE hinst, MemoryMappedIO *io, InterruptServer * vblankIntServer,
				  Profile * profile);
#endif
		~BT848PIP(void);

		Error CompleteDMARAM(FPTR base, WORD bytesPerRow, WORD height, DWORD buffSize, ContinuousDMABuffer *riscbuffer, 
   							   ScatteredDMABuffer *ram, DWORD __far &riscphysical, DWORD __huge* __far &risclogicjmp);

		
		VirtualUnit * CreateVirtual(void);
		
		Error Initialize(void);								// Initializes the BT848 PIP. Call after the object was constructed.
		
		Error SuspendDMA(void);								// Nested DMA disabe.
		Error ResumeDMA(void);								// Nested DMA enable.

		HardVideoParams 			ntscSQP, palSQP, ntscCCIR, palCCIR; 
		BIT							vclkPolarity;									// VCLK polarity.
	};


//
// Virtual BT848 PIP.
//
class VirtualBT848PIP : public VirtualDMAPIP 
	{
	friend class BT848PIP;

	private:
		BT848PIP* unit;

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
		BOOL					autoOverride;
   
		void __far *		physicalBase;
		DWORD					physicalBaseLinear;
		WORD					bytesPerRow;
		PIPFormat			pixelFormat;

   
		Error Preempt (VirtualUnit* previous);
	public:
		VirtualBT848PIP (BT848PIP* physical);
		Error Configure (TAG __far * tags);
		
		Error OptimizeBuffer(int __far& minPixVal);	// Runs an optimization for the BT848 FIFO buffer size.
																	// Returns an optimal value for the PIP_MIN_PIX tag parameter.
	};


#endif // ONLY_EXTERNAL_VISIBLE


// BT848 PIP specific tags                     
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

// Make sure to set physical, bytesPerRow and pixelFormat at the same time when enabling this:
MKTAG (PIP_AUTO_OVERRIDE,	PIP_UNIT, 0x0000003b, BOOL)
MKTAG (PIP_PHYSICAL_BASE,	PIP_UNIT, 0x0000003c, void __far *)
MKTAG (PIP_PHYSICAL_BASE_LINEAR,	PIP_UNIT, 0x0000003d, DWORD)		// Use a 32bit flat pointer
MKTAG (PIP_BYTES_PER_ROW,	PIP_UNIT, 0x0000003e, WORD)
MKTAG (PIP_PIXEL_FORMAT,	PIP_UNIT, 0x0000003f, PIPFormat)

#endif
