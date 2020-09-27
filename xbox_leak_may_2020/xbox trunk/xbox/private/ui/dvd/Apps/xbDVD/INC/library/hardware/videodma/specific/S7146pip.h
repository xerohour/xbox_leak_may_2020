
// FILE:      library\hardware\videodma\specific\s7146pip.h
// AUTHOR:    Dietmar Heidrich, S. Herr
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   03.10.96
//
// PURPOSE: The PIP-related classes for the SAA7146.
//
// HISTORY:

#ifndef SAA7146PIP_H
#define SAA7146PIP_H

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\lowlevel\memmpdio.h"
#include "library\lowlevel\dmabuffr.h"
#include "library\hardware\videodma\generic\gfxconfg.h"
#include "library\lowlevel\intrctrl.h"
#include "library\hardware\pcibrdge\specific\saa7146\saa7146.h"
#include "library\hardware\pcibrdge\specific\saa7146\rps7146.h"

#endif


#include "library\hardware\videodma\generic\dmapip.h"
#include "library\hardware\pcibrdge\specific\saa7146\saa7146r.h"



// Maximum values for destination rectangle size.

#define PIP_WIDTH_MAX   720
#define PIP_HEIGHT_MAX  576



// DMA configuration. Entries are measured in DWORDs.

class DMAConfig7146
	{
	public:
		int dataThreshold, dataBurst;   // video data channel
		int maskThreshold, maskBurst;   // mask data channel

		DMAConfig7146 (void) {dataThreshold = 32; dataBurst = 64; maskThreshold = 32; maskBurst = 64;}
	};



enum PIPDisplayMode
	{
	PDMD_AUTOMATIC
	};



// Error definitions.

#define GNR_DMA_PIP_NOT_POSSIBLE		MKERR(ERROR, PIP, GENERAL, 0x01)		// current screen mode does not allow DMA PIP
#define GNR_VIDEO_STD_NOT_SUPPORTED	MKERR(ERROR, PIP, GENERAL, 0x02)		// desired video standard (e.g. SECAM) not supported
#define GNR_PIP_NOT_RUNNING			MKERR(ERROR, PIP, GENERAL, 0x03)		// PIP is not running when trying to execute OptimizeBuffer()
#define GNR_NO_OPTIMAL_MINPIX			MKERR(ERROR, PIP, GENERAL, 0x04)		// no optimal minPix value could be found by OptimizeBuffer()
#define GNR_DEST_RECT_SIZE				MKERR(ERROR, PIP, GENERAL, 0x05)		// destination rectangle too small or too big
#define GNR_WRONG_GRAB_SIZE			MKERR(ERROR, PIP, GENERAL, 0x06)		// dimensions of frame to grab are too big
#define GNR_NO_VBLANK_IRQ				MKERR(ERROR, PIP, GENERAL, 0x07)		// expected VBlank IRQ did not occur
#define GNR_GRAB_TIMEOUT				MKERR(ERROR, PIP, GENERAL, 0x08)		// timeout occurred during frame grab
#define GNR_WRONG_GRAB_FORMAT			MKERR(ERROR, PIP, GENERAL, 0x09)		// unsupported capture format was requested
#define GNR_SAMPLEMODE_NOT_SUPPORTED	MKERR(ERROR, PIP, GENERAL, 0x0a)		// desired video sample mode not supported

#ifndef ONLY_EXTERNAL_VISIBLE



#define VBLANK_TIMEOUT 100				// milliseconds
#define GRAB_TIMEOUT	  1000			// milliseconds



class SAA7146PIPSetting
	{
	public:
		DWORD ctrl, vertScale, vertGain, preScale, fineScale, output, bcsCtrl;   // HPS registers
		DWORD baseOdd1, baseEven1, protAddr1, pitch1, basePage1, lineByte1;  // video DMA 1 registers
		DWORD baseOdd2, baseEven2, protAddr2, pitch2, basePage2, lineByte2;  // video DMA 2 registers
		BOOL switchOn;
		BOOL useMask;
		BOOL useMMU;   // target is mapped by MMU pages
		BOOL invertFieldPolarity;

		// Page table stuff (if "useMMU").
		SAA7146PageTable *pageTable;
		DWORD pageTablePhysical;

		SAA7146PIPSetting (void)
			{
			switchOn = FALSE;
			useMMU = FALSE;
			pageTable = NULL;
			pageTablePhysical = NULL;
			}

		~SAA7146PIPSetting (void)
			{
			delete pageTable;
			pageTable = NULL;
			pageTablePhysical = NULL;
			}

		void Free (void)
			{
			delete pageTable;
			pageTable = NULL;
			pageTablePhysical = NULL;
			}
	};



class PrepInfoPIP7146 : public PrepInfoPIP
	{
	protected:
		class SAA7146PIP *pip;
		SAA7146PIPSetting setting;
		PIPFormat pipFormat;
		WORD width, height;
		DWORD bytesPerLine;
		RPSProgram *rps;
	public:
		PrepInfoPIP7146 (class SAA7146PIP *pip, PIPFormat pipFormat, WORD width, WORD height, DWORD bytesPerLine);

		virtual Error Prepare (FPTR buffer, DWORD size);
		virtual Error Unprepare (void);

		Error GetPhysicalCodeAddress (DWORD __far &physical) {return rps->GetPhysicalCodeAddress (physical);}

		DWORD captureTime;
	};



class SAA7146PIPVBlankIntHandler;



// Physical SAA7146 Masked DMA PIP class.

class SAA7146PIP : public MaskDMAPIP
	{
	friend class VirtualSAA7146PIP;
	friend class SAA7146PIPVBlankIntHandler;
	friend class PrepInfoPIP7146;

	protected:
		SAA7146 *saa7146;
		MemoryMappedIO *io;					// SAA7146 registers
		Profile *profile;

		// Screen parameters.
		GraphicConfig *gfxConfig;			// info about current graphics board and screen mode
	   DWORD physicalBase;
		WORD bytesPerRow;
		PIPFormat pixelFormat;

		// Masking.
		ContinuousDMABuffer maskBuffer;
		FPTR maskDataPointer;
		DWORD maskPhysical;
		WORD maskRowDWords;									// DWORDs per mask row


		BOOL							grabMode;				// The PIP object is in grabbing mode, when this flag is TRUE.

		HardVideoParams 			ntscSQP, palSQP, ntscCCIR, palCCIR;
		const HardVideoParams* 	curVideoParams;		// Pointer to the settings of the current video standard.

		VideoStandard 				curVideoStd;			// Contains the current video standard.
		VideoSampleMode			sampleMode;
		int							hOffset, vOffset;
		DWORD							dispgrab;



		int srcLeft, srcTop;
		WORD srcWidth, srcHeight;			// source rectangle dimensions


		// 7146 specific parameters.
		HPSSource7146 dataSource;
		HPSSyncs7146 syncSource;
		BOOL useDither;						// for 16 and 15 bit modes
		BOOL maskEnabled;						// if FALSE, DMA is always on top

		DMAConfig7146 dmaConfig;


		WORD	ConvertGrabFormat(GrabFormat format);	// Converts destination format to the YUV2RGB register value. 
		WORD	ConvertPIPFormat(PIPFormat format);		// Same for PIP format. In both functions packed24 is set
																	// in respect to 24 or 32 bit mode.
		int	GetPixelSize (GrabFormat type);			// Get size of a pixel for a certain format.

		Error WaitVBlankIRQ(WORD count);					// Wait for the number of "count" VSYNC IRQs to occur.

		BOOL	adaptSourceSize;								// Tells, if the source rectangle is adapted to the video
																	// standard.
		SAA7146PIPVBlankIntHandler	* vBlankIntHandler;	// VBlank handler.
		InterruptServer 			* vBlankIntServer;	// Interrupt server.

		SAA7146PIPSetting				pipSetting, grabSetting;
		SAA7146PIPSetting	__far	*	currentPIPSetting;

		ContinuousDMABuffer	*	captureDMA;

		FPTR							captureDMABuffer;
		DWORD							captureDMADWordsPerRow;

		HPTR							captureBuffer;
		DWORD							captureDWordsPerRow;

		WORD							captureHeight;

		StreamCaptureHook		*	captureHook;
		YStreamCaptureHook	*	yCaptureHook;

	protected:
		int dmaSuspended;										// Internal DMA suspension flag.

		void 	VBlankInterrupt(void);						// VBlank (VSYNC) IRQ routine.
		DWORD volatile vBlankCount;						// Tells us, how many vertical blank IRQs happened.

		Error StopDMA(void);									// Stops DMA.
		Error StartDMA(void);								// Starts DMA.
		Error EnablePIP(BOOL enable);						// Enables PIP in general.

		Error Update(void);									// Updates all PIP parameters.

		virtual Error CalculateSettings (SAA7146PIPSetting __far &setting, FPTR dstLogical,
													DWORD dstPhysical, DWORD bytesPerLine, WORD lines,
													PIPFormat pixelFormat,
													BOOL targetIsDIB, BOOL useMask, WORD maskDWordsPerRow,
													BOOL singleShot,
													int left, int top, WORD width, WORD height);

		Error ProgramSettings (SAA7146PIPSetting __far &setting, BOOL startLater = FALSE);

		Error ProgramAll(void);								// Reprograms all DMA PIP/grab registers of the SAA7146.

		Error SetMask(RectBitMap * map);
		Error SetSourceRect(int left, int top, int width, int height);
		Error SetVideoStandard(VideoStandard std);
		Error SetSampleMode(VideoSampleMode sampleMode);
		Error SetHOffset(int offset);
		Error SetVOffset(int offset);
		Error SetAdaptSourceSize(BOOL adaptSourceSize);

		Error SetDataSource (HPSSource7146 dataSource);
		Error SetSyncSource (HPSSyncs7146 syncSource);
		Error SetUseDither (BOOL useDither);
		Error SetMaskEnable (BOOL enable);

		Error SetSourceCrop (int left, int top, int right, int bottom);

		Error GrabFrame (FPTR base, WORD width, WORD height, WORD stride, GrabFormat format);

		Error GrabExpandFrame (FPTR base, WORD width, WORD height, WORD stride, GrabFormat format,
		                       RPSProgram *playProgram, BOOL allowMultipleRun);


		virtual Error InitGrabStream (StreamCaptureHookHandle hook, DWORD milliSecPerFrame,
		                              WORD width, WORD height, WORD stride, GrabFormat format);
		virtual Error FinishGrabStream (void);
		virtual Error StartGrabStream (void);
		virtual Error StopGrabStream (void);
		virtual PrepInfoPIP *CreatePrepInfo (void);

		typedef struct CaptureParameters2
			{
			PIPFormat pipFormat;
			DWORD bytesPerLine;
			PrepInfoPIP7146 *captureBuffer;
			BOOL capturedInBuffer;
			BOOL singleField;
			LinkedQueue activePipe;
			int nextBuffer, lastBuffer;
			int nBuffers;   // active buffers
			DWORD irqCounter;
			};
		CaptureParameters2 capture2;
		RPSProgram *grabProgram;
		RPSProgram *screenPrg;


		FPTR	GetStreamGrabBuffer (void);

		Error StartGrabYStream(YStreamCaptureHookHandle hook,
		                       int left, int top,
		                       WORD width, WORD height);
		Error StopGrabYStream(void);
		Error SupplyStreamGrabBuffer(HPTR buffer);

		Error	StoreConfig (DMAConfig7146 *config);
		Error RetrieveConfig (DMAConfig7146 *config);

		int HorizontalAlign (void);		// alignment for SAA7146 PIP in pixels (DWORD aligned, e.g. 24 bit
													// modes need a different alignment than 16 or 15 bit)
		int VerticalAlign (void);

		int MaxDestWidth(void) {return srcWidth;}		// maximum horizontal size of the destination rectangle
		int MaxDestHeight(void) {return srcHeight;}	// maximum vertical size of the destination rectangle

		// For the SAA7146, we need a bitmap with first pixel represented by the MSB.
		RectBitMap *CreateDMAMap (int left, int top, int width, int height)
			{return new RectBitMap (left, top, width, height, 0L, RectBitMap::BITLAYOUT_FIRST_BIT7);}

	public:
		SAA7146PIP (HINSTANCE hinst, SAA7146 *saa7146, MemoryMappedIO *io, InterruptServer *vblankIntServer,
		            Profile *profile);
		~SAA7146PIP(void);

		VirtualUnit *CreateVirtual (void);

		Error ReadConfig (char *filename);				// Allows to read advanced parameters from a configuration file.
		Error Initialize(void);								// Initializes the SAA7146 PIP. Call after the object was constructed.

		Error SuspendDMA(void);								// Nested DMA disabe.
		Error ResumeDMA(void);								// Nested DMA enable.

#if NT_KERNEL || VXD_VERSION
		virtual Error SetPIPBases (DWORD physicalBase, WORD bytesPerRow, PIPFormat pixelFormat);
#elif VIONA_VERSION		
		Error Reconfigure(void);
		BOOL	ScreenModeChanged(void);
#endif
	};



// Virtual SAA7146 PIP.

class VirtualSAA7146PIP : public VirtualMaskDMAPIP 
	{
	friend class SAA7146PIP;

	private:
		SAA7146PIP *unit;

	protected:
		HPSSource7146 dataSource;
		HPSSyncs7146 syncSource;
		BOOL useDither;   // for 16 and 15 bit modes
		BOOL maskEnabled;

		Error Preempt (VirtualUnit *previous);

	public:
		VirtualSAA7146PIP (SAA7146PIP *physical);

		Error Configure (TAG __far *tags);

		Error GrabExpandFrame (FPTR base, WORD width, WORD height, WORD stride, GrabFormat fmt,
		                       RPSProgram *playProgram);
	};



#endif // ONLY_EXTERNAL_VISIBLE



// SAA7146 PIP specific tags.

MKTAG (PIP_DATA_SOURCE,		PIP_UNIT, 0x00000030, HPSSource7146)	// port A or B or combined
MKTAG (PIP_SYNC_SOURCE,		PIP_UNIT, 0x00000031, HPSSyncs7146) 	// port A or B
MKTAG (PIP_USE_DITHER,		PIP_UNIT, 0x00000032, BOOL)				// use dither for 15/16 bit ?
MKTAG (PIP_MASK_ENABLE,		PIP_UNIT, 0x00000033, BOOL)				// mask enable/disable switch
MKTAG (PIP_DISPLAY_MODE, 	PIP_UNIT, 0x00000034, PIPDisplayMode) 	// DispMode selection
MKTAG (PIP_EXT_FIELD_IND,  PIP_UNIT, 0x00000035, BOOL)				// ExtFI mode selection



#endif
