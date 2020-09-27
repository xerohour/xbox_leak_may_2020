
// FILE:      library\hardware\jpeg\generic\jcoram55.cpp
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   02.02.96
//
// PURPOSE: The virtualized JPEG core implemented as combination of S5933/055/050 chips.
//
// HISTORY:

#include "jcoram55.h"
#include "library\hardware\jpeg\generic\jcodrate.h"

#include "library\common\prelude.h"
#include "library\lowlevel\memmpdio.h"
#include "library\lowlevel\dmachanl.h"
#include "library\lowlevel\timer.h"
#include "library\general\parse.h"
#include "library\lowlevel\kernlmem.h"
#include "library\hardware\jpeg\generic\yuv2rgb.h"
#include "library\hardware\jpeg\generic\rgb2yuv.h"
#include "library\hardware\jpeg\specific\z055.h"
#include "library\common\vddebug.h"

#if NT_KERNEL
#include "msgfile.h"
#endif

#define NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS	1

#if NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
#define KMEM(x)	(((KernelMemory *)(x))->KernelSpace())
#else
#define KMEM(x)	x
#endif

// #include <mmsystem.h>

#include "library\support\gfxdebug\gfxdebug.h"





#define MAX_BLOCKS_IN_STRIP  128

#define MAX_COMPRESS_TRIES   5

#define MAX_FIELDSIZE  60000   // as we have an 055

#define DO_TIMING  1





// These arrays define the video window processing parameters for the 055 as
// the sync master. They are indexed by the video standard (NTSC, PAL, SECAM
// in that order).

static int VLineStart [VIDEOSTANDARD_MAX+1] = { 10, 22, 22 };
static int HPixelStart[VIDEOSTANDARD_MAX+1] = { 122, 132, 132 };

static int VSyncSize [VIDEOSTANDARD_MAX+1] = { 6, 6, 6 };
static int HSyncStart[VIDEOSTANDARD_MAX+1] = { 640, 768, 768 };





// This is the bit field that represents the video formats we support.

static DWORD VideoFormatBitField = (1 << VFMT_YUV_411) | (1 << VFMT_YUV_422);





static DWORD FirstLen, FirstNum;










//************************************************************************
// The interrupt handler for JPEG end interrupt.

class JPEGEndIntHandler : public InterruptHandler
	{
	private:
		JPEGManager055 *jpegManager;

		void Interrupt (void) {jpegManager->JPEGEndInterrupt();}

	public:
		JPEGEndIntHandler (JPEGManager055 *jpegManager)
			{this->jpegManager = jpegManager;}
	};










//************************************************************************
// DMA request for our transfer purposes.

class JPEGTransferRequest : public ContinuousDMARequest
	{
	protected:
		VirtualJPEGUnit055 *unit;

	public:
		JPEGTransferRequest (ContinuousDMABuffer *buffer);
		virtual ~JPEGTransferRequest (void);

		void SetUnit (VirtualJPEGUnit055 *unit) {this->unit = unit;}

		virtual void RequestCompleted (void);
	};



JPEGTransferRequest::JPEGTransferRequest (ContinuousDMABuffer *buffer)
	: ContinuousDMARequest (buffer)
	{
	start = 0;
	unit = NULL;
	}



JPEGTransferRequest::~JPEGTransferRequest (void)
	{
	// We got the buffer passed down, so it should not be destroyed by us.
	buffer = NULL;
	}



void JPEGTransferRequest::RequestCompleted (void)
	{
	unit->DMATransferDone ();
	}










//************************************************************************
// The JPEG core manager, i.e. the PhysicalUnit.
//************************************************************************


JPEGManager055::JPEGManager055 (Z055 *z055, VirtualUnit *blankUnit, InterruptServer *jpegIRQ,
                                DataInOutPort *codePort, DataInOutPort *stripPort,
                                DMAChannel *readChannel, DMAChannel *writeChannel,
										  KernelLogger *log)
	{
	this->z055 = z055;
	this->blankUnit = blankUnit;
	this->jpegIRQ     = jpegIRQ;
	this->codePort  = codePort;
	this->stripPort = stripPort;
	this->readChannel  = readChannel;
	this->writeChannel = writeChannel;
	this->log = log;

	jpegEndIntHandler = new JPEGEndIntHandler (this);
	handlerEnabled = FALSE;
	dmaBuffer = NULL;
	dmaRequest = NULL;

#if PCI_VERSION
	if (readChannel)  readChannel->StartRequest ();
	if (writeChannel) writeChannel->StartRequest ();
#endif
	}





JPEGManager055::~JPEGManager055 (void)
	{
	delete jpegEndIntHandler;
	jpegEndIntHandler = NULL;
	delete dmaBuffer;
	delete dmaRequest;
	}





VirtualUnit *JPEGManager055::CreateVirtual (void)
	{
	return NULL;   // NOT USED YET
	}





// JPEG end interrupt.

void JPEGManager055::JPEGEndInterrupt (void)
	{
	GetCurrent()->JPEGEndInterrupt();
	}





// Start or stop the JPEG core operation.

void JPEGManager055::Start (void)
	{
	GetCurrent()->irqSecondField = TRUE;
	z055->StartActivity ();
	// Don't start the 050. The 050 will be started by the 055.
	}


void JPEGManager055::Stop (void)
	{
	z055->StopActivity ();
	}





void JPEGManager055::Reset (void)
	{
	z055->Reset ();
	}





Error JPEGManager055::InitOperation (void)
	{
	VirtualJPEGUnit055 *jpeg = GetCurrent ();
	int i;


#if PCI_VERSION
	// Allocate DMA buffer if needed.
	if ((readChannel || writeChannel)  &&  dmaBuffer == NULL)
		{
		dmaBuffer = new ContinuousDMABuffer;
		if (!IS_ERROR(dmaBuffer->AllocateBuffer (128*1024L) ) &&
			 !IS_ERROR(dmaBuffer->GetBuffer (dmaDataPtr, dmaSize) ))
			dmaRequest = new JPEGTransferRequest (dmaBuffer);
		else
			{
#if NT_KERNEL | WIN32
			if (log) log->LogEvent (LOG_MSG_NO_DMA_BUFFER);
#endif

			readChannel = NULL;
			writeChannel = NULL;
			delete dmaBuffer;
			dmaBuffer = NULL;
			}			
		}
	if (dmaRequest)
		dmaRequest->SetUnit (jpeg);
#endif

	// Configure the 055 and 050 due to compression or changes.
	if (jpeg->changed  ||  jpeg->compress)
		{
		// Stop all other JPEG operations.
		Stop ();
		Reset ();

		// Reconfigure.
		if (! z055->InitOperation (jpeg->compress, jpeg->stillMode, jpeg->vStandard,
		                           jpeg->vFormat, jpeg->fieldOrder, TRUE,
		                           jpeg->width, jpeg->height))
			{
			GDP("055 init operation failed\n");
			GNRAISE(GNR_RANGE_VIOLATION);
			}

		GNREASSERT( jpeg->coder->InitOperation () );
		z055->ReadStatus ();   // to clear RDY resulting from 050's table preload end

		currentPage = 0;
		for (i = 0;  i < H55_N_BUFFERS;  i++)
			pageAvailable[i] = TRUE;
		}

	jpeg->changed = FALSE;

	GNRAISE_OK;
	}





Error JPEGManager055::StartIRQs (void)
	{
	if (! handlerEnabled)
		{
		jpegEndIntServer = jpegIRQ;
		GNREASSERT( jpegEndIntServer->InstallHandler (jpegEndIntHandler) );
		GNREASSERT( jpegEndIntHandler->Enable () );
		handlerEnabled = TRUE;
		}
	GNRAISE_OK;
	}



Error JPEGManager055::StopIRQs (void)
	{
	if (handlerEnabled)
		{
		handlerEnabled = FALSE;
		GNREASSERT( jpegEndIntHandler->Disable () );
		GNREASSERT( jpegEndIntServer->RemoveHandler (jpegEndIntHandler) );
		}
	GNRAISE_OK;
	}





Error JPEGManager055::ExpandIntoOverlayFrame (FPTR frame, DWORD size)
	{
	DWORD timeOut;


	if (size == 0)
		GNRAISE(GNR_RANGE_VIOLATION);

	if (size >= 128*1024L)
		size = 128*1024L - 4;

	// Wait until the expected page becomes available.
	while (! pageAvailable[currentPage])
		{
		timeOut = Timer.GetMilliTicks () + 200;
		while (! z055->NewStatusReady ())
			{
			if (timeOut < Timer.GetMilliTicks())
				GNRAISE(GNR_JPEG_TIMEOUT);
			}
		z055->ReadStatus();   // clear ready bit
		pageAvailable[currentPage] = TRUE;
		}

	codePort->WriteData (frame, size);
	ExpandStop ();
	GNRAISE_OK;
	}



Error JPEGManager055::ExpandStart (DWORD size)
	{
	DWORD timeOut;


	if (size == 0)
		GNRAISE(GNR_RANGE_VIOLATION);

	if (size >= 128*1024L)
		size = 128*1024L - 4;

	// Wait until the expected page becomes available.
	while (! pageAvailable[currentPage])
		{
		timeOut = Timer.GetMilliTicks () + 200;
		while (! z055->NewStatusReady ())
			{
			if (timeOut < Timer.GetMilliTicks())
				GNRAISE(GNR_JPEG_TIMEOUT);
			}
		z055->ReadStatus();   // clear ready bit
		pageAvailable[currentPage] = TRUE;
		}

	dmaRequest->size = size;
	writeChannel->PostRequest (dmaRequest);

	GNRAISE_OK;
	}



void JPEGManager055::ExpandStop (void)
	{
	pageAvailable[currentPage] = FALSE;
	currentPage++;
	currentPage %= H55_N_BUFFERS / 2;
	z055->SetPageNum (currentPage);

	// Status ready indicates (in most cases) that the 055 started to expand the
	// last page once again. To prevent the host from writing to the page which
	// is being expanded, clear the RDY bit.
	z055->ReadStatus();
	}





Error JPEGManager055::ReadCompressedField (FPTR field, BYTE pageNum, DWORD size)
	{
	// Prevent reads of lengthes ending with 511 or 512 - chip bug.
	DWORD paddedLen = size;
	if ((paddedLen & 0xff) == 0xfe  ||  (paddedLen & 0xff) == 0xfd)
		paddedLen += 8;

	z055->SetPageNum (pageNum);

	// For erroneous sizes - quit without reading !
	if (size == 0)
		GNRAISE(GNR_RANGE_VIOLATION);

#if PCI_VERSION
	paddedLen = (paddedLen+3) & ~3;   // round up to DWORD boundary
	if ((paddedLen & 0xff) == 0xfe  ||  (paddedLen & 0xff) == 0xfd)
		paddedLen += 8;
#endif
	codePort->ReadData (field, paddedLen);

	// Check correctness of compressed field.
	BYTE __huge *fieldHPtr = (BYTE __huge *)field;
	if ((unsigned char)fieldHPtr[0] != (unsigned char)0xff  ||
	    (unsigned char)fieldHPtr[1] != (unsigned char)0xd8)
		GNRAISE(GNR_JPEG_ERROR);

	if ((unsigned char)fieldHPtr[size-2] != (unsigned char)0xff  ||
	    (unsigned char)fieldHPtr[size-1] != (unsigned char)0xd9)
		GNRAISE(GNR_JPEG_ERROR);

	GNRAISE_OK;
	}



Error JPEGManager055::CompressStart (BYTE pageNum, DWORD size)
	{
	// Prevent reads of lengthes ending with 511 or 512 - chip bug.
	DWORD paddedLen = size;
	if ((paddedLen & 0xff) == 0xfe  ||  (paddedLen & 0xff) == 0xfd)
		paddedLen += 8;

	z055->SetPageNum (pageNum);

	// For erroneous sizes - quit without reading !
	if (size == 0)
		GNRAISE(GNR_RANGE_VIOLATION);

#if PCI_VERSION
	paddedLen = (paddedLen+3) & ~3;   // round up to DWORD boundary
	if ((paddedLen & 0xff) == 0xfe  ||  (paddedLen & 0xff) == 0xfd)
		paddedLen += 8;
#endif

	dmaRequest->size = paddedLen;
	readChannel->PostRequest (dmaRequest);

	GNRAISE_OK;
	}



Error JPEGManager055::CompressStop (DWORD size)
	{
	// Check correctness of compressed field.
	BYTE __huge *fieldHPtr = (BYTE __huge *)dmaDataPtr;
	if ((unsigned char)fieldHPtr[0] != (unsigned char)0xff  ||
	    (unsigned char)fieldHPtr[1] != (unsigned char)0xd8)
		GNRAISE(GNR_JPEG_ERROR);

	if ((unsigned char)fieldHPtr[size-2] != (unsigned char)0xff  ||
	    (unsigned char)fieldHPtr[size-1] != (unsigned char)0xd9)
		GNRAISE(GNR_JPEG_ERROR);

	GNRAISE_OK;
	}










//************************************************************************
// The JPEG unit class.
//************************************************************************



// Constructor.

VirtualJPEGUnit055::VirtualJPEGUnit055 (JPEGManager055 *physical, VirtualJCoder *coder)
	: VirtualJPEGUnit (physical, coder)
	{
	manager = physical;

	preemptHook = NULL;

	// Set defaults.
	vStandard  = VSTD_NTSC;
	vFormat    = VFMT_YUV_422;
	fieldOrder = VFLD_SINGLE;
	thumbnail  = FALSE;
	filtersOn  = TRUE;
	}



// Destructor.

VirtualJPEGUnit055::~VirtualJPEGUnit055 (void)
	{
	}





// The function responsible for the tag processing.

Error VirtualJPEGUnit055::Configure (TAG __far *tags)
	{
	GNREASSERT( VirtualJPEGUnit::Configure (tags) );
	GNREASSERT( coder->Configure (tags) );

	// Width must be a multiple of 16 and height a multiple of 8.
	if (width & 0x0f  ||  height & 0x07)
		GNRAISE(GNR_RANGE_VIOLATION);

	PARSE_TAGS_START(tags)
		GETSETC (JPG_VIDEO_STANDARD, vStandard);
		GETSETC (JPG_VIDEO_FORMAT,   vFormat);
		GETSETC (JPG_FIELD_ORDER,    fieldOrder);
		GETSETC (JPG_THUMBNAIL,      thumbnail);
		GETSETC (JPG_MSEC_PER_FRAME, milliSecPerFrame);
		GETSETC (JPG_SINGLEFRAME,    singleFrame);
		GETSETC (JPG_FILTERS_ON,     filtersOn);
		GETSETC (JPG_TOTALCODEVOL,   totalCodeVolume);
		GETSET  (JPG_PREEMPT_HOOK,   preemptHook);
		GETONLY (JPG_HAS_CODEMEMORY, TRUE);
		GETONLY (JPG_HOFFSET,        0);
		GETONLY (JPG_VOFFSET,        0);
		GETONLY (JPG_WANTS_EXT_FI,   TRUE);
		GETONLY (JPG_MAX_FIELDSIZE,  MAX_FIELDSIZE);
		GETONLY (JPG_VBUS_WIDTH,     width);
		GETONLY (JPG_VBUS_HEIGHT,    height);
		GETINQUIRE (JPG_VIDEO_FMT_LIST, data &= VideoFormatBitField;);
	PARSE_TAGS_END

	// We want the coder to use the second field marker for both field.
	// The first field is marked correctly after the grab.
	VideoField order;
	if (vStandard == VSTD_NTSC)
		order = fieldOrder;
	else
		{
		switch (fieldOrder)
			{
			case VFLD_SINGLE:  order = VFLD_SINGLE;  break;
			case VFLD_ODD:     order = VFLD_EVEN;    break;
			case VFLD_EVEN:    order = VFLD_ODD;     break;
			}
		}
	GNREASSERT( coder->ConfigureTags (SET_JPG_FIELD_ORDER(order), TAGDONE) );

	GNRAISE_OK;
	}





Error VirtualJPEGUnit055::PrepareBuffer (FPTR buffer, DWORD size, FPTR __far &info)
	{
	// Note that the buffers are used during interrupt.
#if NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
	FPTR dummy;
	KernelMemory * kernelMem = new KernelMemory();

	kernelMem->MapMemory(buffer, size, dummy);
	info = kernelMem;
#else
	GlobalPageLock (FP_SEG(buffer));
	info = buffer;
#endif

	GNRAISE_OK;
	}





Error VirtualJPEGUnit055::UnprepareBuffer (FPTR info)
	{
#if NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
	delete (KernelMemory *)info;
#else
	GlobalPageUnlock (FP_SEG(info));
#endif
	GNRAISE_OK;
	}





#if ! VIONA_VERSION
	// The next line will produce a compiler error.
	The Q-Motion board must have VIONA_VERSION=1.
#endif





// Is a scaling valid ?  We need 24 or 32 bits per pixel, srcWidth <= 768,
// the dst sizes must be <= half the src sizes, and we need an even
// number of MCUs (i.e. srcWidth must be multiple of 16).

BOOL VirtualJPEGUnit055::IsValidExpansionScaling (WORD srcWidth, WORD srcHeight,
                                                  WORD dstWidth, WORD dstHeight,
                                                  GrabFormat bitFormat)
	{
	// We support 32, 24 and 16 bit formats.
	if (bitFormat != VGRB_RGB_888x  &&  bitFormat != VGRB_RGB_888  &&  bitFormat != VGRB_RGB_565)
		return FALSE;

	// No scaling is valid.
	if (dstWidth == srcWidth  &&  dstHeight == srcHeight)
		return TRUE;

	// Check restricted scaling capability.
	if ((bitFormat != VGRB_RGB_888x  &&  bitFormat != VGRB_RGB_888)  ||  srcWidth > 768)
		return FALSE;
	if (dstWidth * 2 > srcWidth  ||  dstHeight * 2 > srcHeight)
		return FALSE;
	if ((srcWidth & 0x0f) != 0)
		return FALSE;
	return TRUE;
	}





// Expand a compressed frame into packed RGB data.

Error VirtualJPEGUnit055::ExpandToRGB (FPTR src, DWORD size, FPTR dst, WORD dstWidth, WORD dstHeight, GrabFormat bitFormat)
	{
	char __huge *blocksBuff;
	WORD hmcuSize, vmcuSize;
	WORD blocksInStrip, blocksInFrame, blocksInField, remainBlocks, fldCnt;
#if !NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
	HANDLE hBlk;
#endif
	DWORD timeOut;
	DWORD imageSizeInBytes;
	BOOL expansionOK = TRUE;
	char __huge *tmpBlocksBuff;
	WORD width  = this->width;
	WORD height = this->height;
	BOOL scaling;
	Error err;
#if DO_TIMING
	DWORD start = Timer.GetMilliTicks ();
#endif


	// Check unit state.
	if (GetState() != current)
		GNRAISE(GNR_OBJECT_NOT_CURRENT);
	if (compress  ||  ! stillMode)
		GNRAISE(GNR_INVALID_CONFIGURE_STATE);

	// Is scaling valid ?
	if (! IsValidExpansionScaling (width, height, dstWidth, dstHeight, bitFormat))
		GNRAISE(GNR_RANGE_VIOLATION);
	scaling = (dstWidth != width  ||  dstHeight != height);

	changed = TRUE;   // reprogram chip parameters
	GNREASSERT( manager->InitOperation () );

	// Round up source sizes to MCU block size boundaries.
	switch (vFormat)
		{
		case VFMT_YUV_422:  hmcuSize = 16;  vmcuSize =  8;  break;
		case VFMT_YUV_411:  hmcuSize = 32;  vmcuSize =  8;  break;
		case VFMT_YUV_420:  hmcuSize = 16;  vmcuSize = 16;  break;
		default:
			GNRAISE(GNR_RANGE_VIOLATION);
		}
	width += hmcuSize - 1;      // we modify a local copy of the member variable !
	width -= width % hmcuSize;  // we modify a local copy of the member variable !
	height += vmcuSize - 1;
	height -= height % vmcuSize;
	blocksInFrame = (width / 8) * (height / 8) * 2;   // 2 bytes in YUV format

	// Allocate temporary buffer.
	imageSizeInBytes = (DWORD)blocksInFrame * 64L;

#if NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
#if WIN32
	blocksBuff = new __huge (PagedPool) char[imageSizeInBytes];
#else
	blocksBuff = (char __huge *) _halloc (imageSizeInBytes, sizeof(char));   // MSVC bug in "new __huge"
#endif
	if (! blocksBuff)
		{
		GDP("temp buffer not allocated");
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
#else
	hBlk = GlobalAlloc (GMEM_MOVEABLE, imageSizeInBytes);
	if (! hBlk)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	blocksBuff = (char __huge *) GlobalLock (hBlk);
	if (! blocksBuff)
		{
		GDP("temp buffer not allocated");
		GlobalFree (hBlk);
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
#endif

	tmpBlocksBuff = blocksBuff;

	manager->z055->SetBlocksInStrip (MAX_BLOCKS_IN_STRIP - 1, FALSE);

	// Start expansion.
	GNREASSERT( Start () );
	err = manager->ExpandIntoOverlayFrame (src, size);
	if (IS_ERROR(err))
		{
		Stop ();
#if NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
#if WIN32
		delete[] blocksBuff;
#else
		_hfree (blocksBuff);
#endif
#else
		GlobalUnlock (hBlk);
		GlobalFree (hBlk);
#endif
		return err;
		}
	manager->z055->ReadStatus ();   // to clear RDY bit
	if (fieldOrder != VFLD_SINGLE)
		{
		blocksInField = blocksInFrame / 2;
		if (scaling)
			fldCnt = 1;   // transfer only one field, ignore second
		else
			fldCnt = 2;
		}
	else
		{
		blocksInField = blocksInFrame;
		fldCnt = 1;
		}

	// Get the strip buffer contents.
	while (fldCnt != 0)
		{
		remainBlocks = blocksInField;
		while (remainBlocks != 0)
			{
			timeOut = Timer.GetMilliTicks () + 200;
			while (! manager->z055->StripReady ())
				{
				if (timeOut < Timer.GetMilliTicks())
					{
					Stop ();
#if NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
#if WIN32
					delete[] blocksBuff;
#else
					_hfree (blocksBuff);
#endif
#else
					GlobalUnlock (hBlk);
					GlobalFree (hBlk);
#endif
					GNRAISE(GNR_JPEG_TIMEOUT);
					}
				}
			if (remainBlocks > MAX_BLOCKS_IN_STRIP)
				{
				blocksInStrip = MAX_BLOCKS_IN_STRIP;
				// if (remainBlocks > (MAX_BLOCKS_IN_STRIP * 2))
				if (manager->z055->NewStatusReady ())
					expansionOK = FALSE;
				}
			else
				blocksInStrip = remainBlocks;
			remainBlocks -= blocksInStrip;
			manager->stripPort->ReadData (tmpBlocksBuff, blocksInStrip * 64);
			tmpBlocksBuff += blocksInStrip * 64;
			}
		fldCnt--;
		}

	// Convert from blocks to raster and from YUV to RGB.
#if DO_TIMING
	DWORD yuvStart = Timer.GetMilliTicks ();
#endif
	if (scaling)
		{
		if (fieldOrder != VFLD_SINGLE)
			height /= 2;
		switch (bitFormat)
			{
			case VGRB_RGB_888x:  mapYUV422BlockstoBGR32Scale (blocksBuff, dst, width, height, dstWidth, dstHeight);  break;
#if !NT_KERNEL
			case VGRB_RGB_888:   mapYUV422BlockstoBGR24Scale (blocksBuff, dst, width, height, dstWidth, dstHeight);  break;
#endif
			default:
				GNRAISE(GNR_RANGE_VIOLATION);
			}
		}
	else
		{
		switch (bitFormat)
			{
			case VGRB_RGB_888x:  mapYUV422BlockstoBGR32 (blocksBuff, dst, width, height, fieldOrder);  break;
			case VGRB_RGB_888:   mapYUV422BlockstoBGR24 (blocksBuff, dst, width, height, fieldOrder);  break;
			case VGRB_RGB_565:   mapYUV422BlockstoRGB16 (blocksBuff, dst, width, height, fieldOrder);  break;
			default:
				GNRAISE(GNR_RANGE_VIOLATION);
			}
		}
#if DO_TIMING
	GDP("yuv time %ld", Timer.GetMilliTicks() - yuvStart);
#endif

	// Deallocate temporary buffer.
	Stop ();
#if NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
#if WIN32
	delete[] blocksBuff;
#else
	_hfree (blocksBuff);
#endif
#else
	GlobalUnlock (hBlk);
	GlobalFree (hBlk);
#endif

#if DO_TIMING
	GDP("total expansion time %ld", Timer.GetMilliTicks() - start);
#endif
	GNRAISE_OK;
	}





// Compress RGB data into a frame.

Error VirtualJPEGUnit055::CompressFromRGB (FPTR src, FPTR dst, DWORD maxSize, DWORD __far &size, BOOL bits32)
	{
	DWORD imageSizeInBytes;
	WORD numberOfBlocks;
#if !NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
	HANDLE hBlk;
#endif
	char __huge *blocksBuff;
	FPTR oddField, oddBlocks;
	DWORD evenSize, oddSize;
	Error err;
#if DO_TIMING
	DWORD start = Timer.GetMilliTicks ();
#endif


	// Check unit state.
	if (GetState() != current)
		GNRAISE(GNR_OBJECT_NOT_CURRENT);
	if (! compress  ||  ! stillMode)
		GNRAISE(GNR_INVALID_CONFIGURE_STATE);

	GNREASSERT( manager->InitOperation () );

	imageSizeInBytes = (DWORD)width * (DWORD)height * 3L;
	if (fieldOrder != VFLD_SINGLE)
		maxSize /= 2;

	// Allocate temporary buffer.
#if NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
#if WIN32
	blocksBuff = new __huge (PagedPool) char[imageSizeInBytes];
#else
	blocksBuff = (char __huge *) _halloc (imageSizeInBytes, sizeof(char));   // MSVC bug in "new __huge"
#endif
	if (! blocksBuff)
		{
		GDP("temp buffer not allocated");
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
#else
	hBlk = GlobalAlloc (GMEM_MOVEABLE, imageSizeInBytes);
	if (! hBlk)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	blocksBuff = (FPTR) GlobalLock (hBlk);
	if (! blocksBuff)
		{
		GDP("temp buffer not allocated");
		GlobalFree (hBlk);
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
#endif

#if DO_TIMING
	DWORD yuvStart = Timer.GetMilliTicks ();
#endif
	if (bits32)
		{
		GDP("using 32 bits");
		numberOfBlocks = mapRGB32toYUV422Blocks (src, blocksBuff, width, height, (fieldOrder == VFLD_SINGLE));
		// for an interlaced image, numberOfBlocks is the number of blocks
		// in EACH of the fields !!!
		}
	else
		{
		GDP("using 24 bits");
		numberOfBlocks = mapRGB24toYUV422Blocks (src, blocksBuff, width, height, (fieldOrder == VFLD_SINGLE));
		// for an interlaced image, numberOfBlocks is the number of blocks
		// in EACH of the fields !!!
		}
#if DO_TIMING
	GDP("yuv time %ld", Timer.GetMilliTicks() - yuvStart);
#endif

	// Compress the first field.
	GNREASSERT( Start () );
	err = CompressField (blocksBuff, numberOfBlocks, dst, maxSize, evenSize);
	if (! IS_ERROR(err))
		{
		size = evenSize;
		if (fieldOrder != VFLD_SINGLE)
			{
			// Compress the second field.
			oddField  = (FPTR)((char __huge *)dst + evenSize);
			oddBlocks = (FPTR)((char __huge *)blocksBuff + (DWORD)numberOfBlocks * 64L);

			err = CompressField (oddBlocks, numberOfBlocks, oddField, maxSize, oddSize);
			size += oddSize;
			}
		}

	Stop ();
	if (! IS_ERROR(err))
		{
		((VirtualJCoderRated *)(coder))->GetLastScaleFactor ();

		// Mark the first field correctly.
		VideoField order;
		if (vStandard == VSTD_NTSC)
			{
			switch (fieldOrder)
				{
				case VFLD_SINGLE:  order = VFLD_SINGLE;  break;
				case VFLD_ODD:     order = VFLD_EVEN;    break;
				case VFLD_EVEN:    order = VFLD_ODD;     break;
				}
			}
		else
			order = fieldOrder;
		coder->MarkFirstField (dst, size, order);
		}
#if NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
#if WIN32
	delete[] blocksBuff;
#else
	_hfree (blocksBuff);
#endif
#else
	GlobalUnlock (hBlk);
	GlobalFree (hBlk);
#endif

#if DO_TIMING
	GDP("total compression time %ld", Timer.GetMilliTicks() - start);
#endif
	return err;
	}





	Error VirtualJPEGUnit055::CompressField (FPTR src, WORD numberOfBlocks, FPTR dst, DWORD maxSize, DWORD __far &size)
		{
		BYTE __huge *blocksPtr;
		BYTE pageNum;
		WORD blocksInStrip;
		DWORD timeOut;
		DWORD minSize = totalCodeVolume * 5 / 8;
		DWORD compressedSize = 0;
		WORD numOfBlocks = numberOfBlocks;
		WORD tries = 0;
		BOOL accept = FALSE;
		BOOL remainBlocksDivided, lastStrip;


		while (! accept)
			{
			remainBlocksDivided = FALSE;   // to ident split of last strip
			numberOfBlocks = numOfBlocks;
			blocksPtr = (BYTE __huge *)src;   // update in case of fail to compress
			while (numberOfBlocks != 0)
				{
				timeOut = Timer.GetMilliTicks () + 200;
				while (! manager->z055->StripReady ())
					{
					if (timeOut < Timer.GetMilliTicks())
						{
						GDP("timeout waiting for StripReady");
						GNRAISE(GNR_JPEG_TIMEOUT);
						}
					}

				if (numberOfBlocks > MAX_BLOCKS_IN_STRIP)
					{
					blocksInStrip = MAX_BLOCKS_IN_STRIP;
					lastStrip = FALSE;
					}
				else if (! remainBlocksDivided)
					{
					remainBlocksDivided = TRUE;
					blocksInStrip = numberOfBlocks / 2;
					lastStrip = FALSE;
					}
				else
					{
					blocksInStrip = numberOfBlocks;
					lastStrip = TRUE;
					}
				manager->z055->SetBlocksInStrip (blocksInStrip - 1, lastStrip);
				manager->stripPort->WriteData (blocksPtr, blocksInStrip * 64);
				numberOfBlocks -= blocksInStrip;
				blocksPtr += blocksInStrip * 64;   // due to 64 bytes per block
				}

			timeOut = Timer.GetMilliTicks () + 200;
			while (! manager->z055->NewStatusReady ())
				{
				if (timeOut < Timer.GetMilliTicks())
					{
					GDP("timeout waiting for NewStatusReady");
					GNRAISE(GNR_JPEG_TIMEOUT);
					}
				}

			manager->z055->ReadStatus ();
			pageNum = manager->z055->GetPageNum ();
			compressedSize = manager->z055->GetFieldLength ();

			if (compressedSize <= maxSize  &&  compressedSize >= minSize)
				accept = TRUE;
			else
				{
				GDP("one more: %ld in range %ld - %ld", compressedSize, minSize, maxSize);
				if (++tries >= MAX_COMPRESS_TRIES)
					{
					if (compressedSize < minSize)
						accept = TRUE;   // we cannot get more code, so accept this compression
					else
						{
						GDP("too many tries");
						GNRAISE(GNR_JPEG_TIMEOUT);
						}
					}
				}
			} // while

		GNREASSERT( manager->ReadCompressedField (dst, pageNum, compressedSize) );

		size = compressedSize;
		GNRAISE_OK;
		}





Error VirtualJPEGUnit055::InitCapture (JPEGCaptureHook __far *hook)
	{
	int i;


	// Check unit state.
	if (GetState() != current)
		GNRAISE(GNR_OBJECT_NOT_CURRENT);
	if (! compress  ||  stillMode)
		GNRAISE(GNR_INVALID_CONFIGURE_STATE);

	for (i = 0;  i < N_DLL_BUFFERS;  i++)
		{
		buffers[i].info     = NULL;
		buffers[i].userData = NULL;
		}
	nextBuffer = 0;
	lastBuffer = 0;
	nBuffers   = 0;
	fieldNum   = 0;
	lastFieldCnt = 255;

	nextFrameTime = 0;

	// Prevent starting on second field for single field capture.
	FirstLen = 100;   // must not be zero
	transferRunning = FALSE;
	capState = CAP_NONE;

	// Initialize the operation.
	GNREASSERT( manager->InitOperation () );
	capHook = hook;
	GNRAISE( manager->StartIRQs () );
	}





Error VirtualJPEGUnit055::FinishCapture (void)
	{
#if NT_KERNEL
	irqMutex.Enter();
	capHook = NULL;
	irqMutex.Leave();
#else
	capHook = NULL;
#endif
	return manager->StopIRQs ();
	}





void VirtualJPEGUnit055::PassUpBuffer (void)
	{
	DWORD nextFrameNum, timeStream;
	Error err;


	buffers[nextBuffer].info = NULL;   // buffer available for adding another buffer
	nBuffers--;
	capMsg.repeatIfEmpty = FALSE; //(nBuffers == 0);

	if (! singleFrame  &&  fieldOrder == VFLD_SINGLE)
		{
		if ((capMsg.frameNum & 1) == 1)
			{
			// Drop second field in single field mode.
			GDP("Dropping second field");
			buffers[nextBuffer].info = NULL;   // buffer available for adding another buffer
			capMsg.byteSize = 0;
			}
		capMsg.frameNum >>= 1;
		}

	// Drop the very first two frames of a sequence to let the bit rate control
	// process stabilize.
	if (capMsg.byteSize > 0  &&  (capMsg.frameNum >= 2L  ||  singleFrame))
		{
		// Mark the first field correctly.
		VideoField order;
		if (vStandard == VSTD_NTSC)
			{
			switch (fieldOrder)
				{
				case VFLD_SINGLE:  order = VFLD_SINGLE;  break;
				case VFLD_ODD:     order = VFLD_EVEN;    break;
				case VFLD_EVEN:    order = VFLD_ODD;     break;
				}
			}
		else
			order = fieldOrder;
		err = coder->MarkFirstField (KMEM(capMsg.info), capMsg.byteSize, order);
		if (IS_ERROR(err))
			{
			GDP("marking first field %08lx", err);
			}

		if (singleFrame)
			nextFrameNum = capMsg.frameNum;
		else
			nextFrameNum = capMsg.frameNum - 2L;

		// Compute the time of the frame.
		if (VSTD_NTSC == vStandard)
			// nextFrameNum * 780 * 525 * 1000 / 12 272 727.
			timeStream = ScaleDWord (nextFrameNum, 4090909L, 136500000);
		else
			timeStream = 40 * nextFrameNum;

		// If it's time for a new frame, process it.
		if (timeStream >= nextFrameTime)
			{
			// Capture OK.
			nextFrameTime += milliSecPerFrame;
			capMsg.captureTime = timeStream;
			}
		else
			{
			GDP("not yet time\n");
			capMsg.byteSize = 0;
			}
		}
	else
		capMsg.byteSize = 0;

	capHook.Call (capMsg);
	nextBuffer = (nextBuffer+1) & (N_DLL_BUFFERS-1);
	}





void VirtualJPEGUnit055::DMATransferDone (void)
	{
#if PCI_VERSION
#if NT_KERNEL
	irqMutex.EnterAtIRQLevel();
#endif

	if (compress  &&  transferRunning)
		{
		transferRunning = FALSE;
		manager->CompressStop (capMsg.byteSize);

		BOOL doCallback = FALSE;

		switch (capState)
			{
			case CAP_NONE:
				break;

			case CAP_SINGLE_FIRST_BAD:
			case CAP_SINGLE_FIRST:
				_hmemcpy (KMEM(capMsg.info), manager->dmaDataPtr, capMsg.byteSize);
				doCallback = TRUE;
				break;

			case CAP_FIRST:
				_hmemcpy (KMEM(capMsg.info), manager->dmaDataPtr, capMsg.byteSize);
				break;

			case CAP_FIRST_BAD:
				_hmemcpy (KMEM(capMsg.info), manager->dmaDataPtr, capMsg.byteSize);
				GDP("Duplicate second");
				_hmemcpy ((char __huge *)KMEM(capMsg.info) + capMsg.byteSize, KMEM(capMsg.info), capMsg.byteSize);
				capMsg.byteSize *= 2;
				doCallback = TRUE;
				break;

			case CAP_SECOND:
				char __huge *p = (char __huge *)KMEM(capMsg.info) + FirstLen;
				_hmemcpy (p, manager->dmaDataPtr, capMsg.byteSize);
				capMsg.byteSize += FirstLen;
				doCallback = TRUE;
				break;
			}

		if (doCallback)
			{
GDP("finDMA %d", nextBuffer);
			PassUpBuffer ();
			}
		}


	if (! compress  &&  transferRunning)
		{
		transferRunning = FALSE;
		manager->ExpandStop ();

GDP("finish %ld", (long)nextBuffer);
		nBuffers--;
		buffers[nextBuffer].info = NULL;   // buffer available for adding another buffer
		nextBuffer = (nextBuffer+1) & (N_DLL_BUFFERS-1);
		}

#if NT_KERNEL
	irqMutex.LeaveAtIRQLevel();
#endif
#endif
	}





void VirtualJPEGUnit055::JPEGEndInterrupt (void)
	{
	class Z055 *z055 = manager->z055;
	DWORD fieldCnt;
	BYTE page;
	DWORD firstFieldBit;
	Error err;

	JPEGPlaybackMsg pmsg;
	FPTR src;
	DWORD size;

#if NT_KERNEL
	irqMutex.EnterAtIRQLevel();
#endif

	// Motion compression.
	if (compress)
		{
		if (transferRunning)
			{
			GDP("transfer running");
#if NT_KERNEL
			irqMutex.LeaveAtIRQLevel();
#endif
			return;
			}

		if (capHook == NULL)
			{
			GDP("No capHook");
			z055->ReadStatus ();
#if NT_KERNEL
			irqMutex.LeaveAtIRQLevel();
#endif
			return;
			}

		if (! manager->readChannel)
			transferRunning = TRUE;
		do {
			z055->ReadStatus ();

			if (buffers[nextBuffer].info == NULL)
				{
				// No buffer available.
				GDP("no buffer available");
				// Call the capture driver so it can pass down new buffers.
				page = z055->GetPageNum ();
				manager->ReadCompressedField (NULL, page, 0);   // dummy read
				capMsg.info     = NULL;
				capMsg.userData = NULL;
				capMsg.byteSize = 0;
				capHook.Call (capMsg);
				}
			else
				{
				// Get field parameters from the 055.
				page = z055->GetPageNum ();
				fieldCnt = (DWORD) z055->GetFieldCount ();

				if ((fieldNum & 0xff) > fieldCnt)
					fieldNum += 256;
				fieldNum = (fieldNum & 0xffffff00) | fieldCnt;
				capMsg.info     = buffers[nextBuffer].info;
				capMsg.userData = buffers[nextBuffer].userData;
				capMsg.frameNum = fieldNum;
				capMsg.byteSize = z055->GetFieldLength ();

#if 0
					{
					static int lastPageNum;
					static DWORD lastFieldNum;

					if (page != (lastPageNum + 1) % 4)
						DP("Page num wrong %d -> %d", page, lastPageNum);
					if (fieldNum != lastFieldNum + 1)
						DP("Field num wrong %ld -> %ld", fieldNum, lastFieldNum);

					lastFieldNum = fieldNum;
					lastPageNum = page;
					}
#endif

				capState = CAP_NONE;

				if (fieldOrder == VFLD_SINGLE)
					{
					// Single field capture.
					if (1 == (fieldNum & 1))
						{
						if (FirstLen != 0)
							// First field was captured. Drop second field.
							manager->ReadCompressedField (KMEM(capMsg.info), page, 0);   // dummy read
						else
							{
							// First field was bad. We can save the frame from being dropped by
							// using the second field.
							if (capMsg.byteSize > buffers[nextBuffer].byteSize)
								{
								GDP("Single second field too large");
								manager->ReadCompressedField (KMEM(capMsg.info), page, 0);   // dummy read
								}
							else
								{
								// Get the second field data.
								if (manager->readChannel)
									{
									GDP("single first bad");
									err = manager->CompressStart (page, capMsg.byteSize);   // use DMA
									}
								else
									err = manager->ReadCompressedField (KMEM(capMsg.info), page, capMsg.byteSize);

								if (IS_ERROR(err))
									GDP("Single second field bad");
								else
									{
									GDP("Using second");
									capMsg.frameNum = FirstNum;
									capState = CAP_SINGLE_FIRST_BAD;   // pass data to capture driver
									if (manager->readChannel)
										{
										transferRunning = TRUE;
#if NT_KERNEL
										irqMutex.LeaveAtIRQLevel();
#endif
										return;
										}
									}
								}
							}
						}
					else
						{
						// Get the data of the first field. Take care of maximum buffer size.
GDP("cnt %ld ", fieldNum);
						FirstNum = fieldNum;
						FirstLen = 0;
						if (capMsg.byteSize > buffers[nextBuffer].byteSize)
							{
							GDP("Single field too large");
							manager->ReadCompressedField (KMEM(capMsg.info), page, 0);   // dummy read
							}
						else
							{
							if (manager->readChannel)
								{
								GDP("single first");
								err = manager->CompressStart (page, capMsg.byteSize);   // use DMA
								}
							else
								err = manager->ReadCompressedField (KMEM(capMsg.info), page, capMsg.byteSize);

							if (IS_ERROR(err))
								{
								GDP("Bad single field");
								}
							else
								{
								FirstLen = capMsg.byteSize;   // first field OK
								capState = CAP_SINGLE_FIRST;   // pass data to capture driver
								if (manager->readChannel)
									{
									transferRunning = TRUE;
#if NT_KERNEL
									irqMutex.LeaveAtIRQLevel();
#endif
									return;
									}
								}
							}
						}
					}
				else
					{
					// Two field capture. Join first and second field.
					if (vStandard == VSTD_NTSC)
						firstFieldBit = (fieldOrder != VFLD_EVEN ? 1UL : 0UL);
					else
						firstFieldBit = (fieldOrder != VFLD_ODD ? 1UL : 0UL);
					if (firstFieldBit == (fieldNum & 1))
						{
						// First field. Remember its parameters and get it. Take care of maximum buffer size.
						FirstNum = fieldNum;
						FirstLen = 0;
						if (capMsg.byteSize > buffers[nextBuffer].byteSize)
							{
							GDP("First field too large");
							manager->ReadCompressedField (KMEM(capMsg.info), page, 0);   // dummy read
							}
						else
							{
							if (manager->readChannel)
								err = manager->CompressStart (page, capMsg.byteSize);   // use DMA
							else
								err = manager->ReadCompressedField (KMEM(capMsg.info), page, capMsg.byteSize);

							if (IS_ERROR(err))
								{
								GDP("Bad first field");
								}
							else
								{
								FirstLen = capMsg.byteSize;   // first field OK
								capState = CAP_FIRST;
								if (manager->readChannel)
									{
									transferRunning = TRUE;
#if NT_KERNEL
									irqMutex.LeaveAtIRQLevel();
#endif
									return;
									}
								}
							}
						}
					else
						{
						// Second field.
						capMsg.frameNum = FirstNum >> 1;
						if (FirstLen == 0)
							{
							// First field was bad. We can save the frame from being dropped by duplicating
							// the second field.
							if (2 * capMsg.byteSize > buffers[nextBuffer].byteSize)
								{
								GDP("First bad, second too large");
								manager->ReadCompressedField (KMEM(capMsg.info), page, 0);   // dummy read
								}
							else
								{
								if (manager->readChannel)
									err = manager->CompressStart (page, capMsg.byteSize);   // use DMA
								else
									err = manager->ReadCompressedField (KMEM(capMsg.info), page, capMsg.byteSize);

								if (IS_ERROR(err))
									{
									GDP("First and second bad");
									}
								else
									{
									capState = CAP_FIRST_BAD;   // pass data to capture driver
									if (manager->readChannel)
										{
										transferRunning = TRUE;
#if NT_KERNEL
										irqMutex.LeaveAtIRQLevel();
#endif
										return;
										}
									// Duplicate second field.
									GDP("Duplicate second");
									_hmemcpy ((char __huge *)KMEM(capMsg.info) + capMsg.byteSize, KMEM(capMsg.info), capMsg.byteSize);
									capMsg.byteSize *= 2;
									}
								}
							}
						else if (FirstNum + 1 != fieldNum)
							{
							// Fields don't belong together.
							GDP("Field %ld and %ld not joined", FirstNum, fieldNum);
							manager->ReadCompressedField (KMEM(capMsg.info), page, 0);   // dummy read
							}
						else
							{
							// Read the second field data behind the first field data. Take care of maximum buffer size.
GDP("cnt %ld ", fieldNum);
							char __huge *p = (char __huge *)KMEM(capMsg.info) + FirstLen;
							if (capMsg.byteSize+FirstLen > buffers[nextBuffer].byteSize)
								{
								GDP("Second field too large");
								manager->ReadCompressedField (KMEM(capMsg.info), page, 0);   // dummy read
								}
							else
								{
								if (manager->readChannel)
									err = manager->CompressStart (page, capMsg.byteSize);   // use DMA
								else
									err = manager->ReadCompressedField ((FPTR)p, page, capMsg.byteSize);

								if (IS_ERROR(err))
									{
									GDP("Bad second field");
									}
								else
									{
									capState = CAP_SECOND;   // pass data to capture driver
									if (manager->readChannel)
										{
										transferRunning = TRUE;
#if NT_KERNEL
										irqMutex.LeaveAtIRQLevel();
#endif
										return;
										}
									capMsg.byteSize += FirstLen;
									}
								}
							// If the second field is bad or too large, duplicate the first.
							if (capState != CAP_SECOND)
								{
								if (2 * FirstLen > buffers[nextBuffer].byteSize)
									{
									GDP("First not duplicated, too large");
									}
								else
									{
									GDP("Duplicate first");
									_hmemcpy ((FPTR)p, KMEM(capMsg.info), FirstLen);
									capMsg.byteSize = 2 * FirstLen;
									capState = CAP_SECOND;   // pass data to capture driver
									}
								}
							}
						}
					}

				if (capState != CAP_NONE  &&  capState != CAP_FIRST)
					{
GDP("finish %d", nextBuffer);
					PassUpBuffer ();
					}
				}

			} while (buffers[nextBuffer].info != NULL  &&  z055->NewStatusReady ());
		if (! manager->readChannel)
			transferRunning = FALSE;
		}


	// Motion playback.
	if (! compress)
		{
		if (transferRunning)
			{
			// We are still copying data via port, so lose this frame.
			GDP("transfer overlap");
#if NT_KERNEL
			irqMutex.LeaveAtIRQLevel();
#endif
			return;
			}

		if (fieldOrder == VFLD_SINGLE  &&  irqSecondField)
			{
			// Interrupt for the second field.
			irqSecondField = ! irqSecondField;
			manager->z055->ReadStatus ();
#if NT_KERNEL
			irqMutex.LeaveAtIRQLevel();
#endif
			return;
			}

		// Interrupt for the first field.
		irqSecondField = ! irqSecondField;

		pmsg.nowTime = Timer.GetMilliTicks ();
		if (playHook != NULL)
			{
			playHook.Call (pmsg);
			}
#if 0
			{
			static int x;
			static DWORD now;
			static DWORD pred;

			now = Timer.GetMilliTicks();

			DP("<%ld>", now-pred);
			if (x++ == 32)
				{
				x = 0;
				DP("\n");
				}

			pred = now;
			}
#endif

		if (nBuffers <= 1)
			{
			// No new buffer was supplied.
			manager->z055->ReadStatus ();
			}
		else
			{
			src  = KMEM(buffers[nextBuffer].info);
			size = buffers[nextBuffer].byteSize;
			if (size != 0  &&  nBuffers > 1)
				{
				if (manager->writeChannel)
					{
					// Write the data via DMA.
					_hmemcpy (manager->dmaDataPtr, src, size);
					if (IS_ERROR( manager->ExpandStart (size) ))
						{
						GDP("ExpandStart failed");
						manager->z055->ReadStatus ();
						}
					else
						{
						transferRunning = TRUE;
						}
#if NT_KERNEL
					irqMutex.LeaveAtIRQLevel();
#endif
					return;
					}
				else
					{
					// No DMA used.
					transferRunning = TRUE;
					if (IS_ERROR( manager->ExpandIntoOverlayFrame (src, size) ))
						manager->z055->ReadStatus ();
					transferRunning = FALSE;
					}
				}
			else
				{
				GDP("size is zero");
				manager->z055->ReadStatus ();
				}
			// If we had overlapping interrupts and the intermediate
			// interrupt terminated itself, ExpandIntoOverlayFrame()
			// or the code above will have called ReadStatus() to
			// maintain interrupts.
//GDP("finish %ld", (long)nextBuffer);
			if (nBuffers > 1)
				{
				nBuffers--;
				buffers[nextBuffer].info = NULL;   // buffer available for adding another buffer
				nextBuffer = (nextBuffer+1) & (N_DLL_BUFFERS-1);
				}
			}
		}

#if NT_KERNEL
	irqMutex.LeaveAtIRQLevel();
#endif
	}





Error VirtualJPEGUnit055::AddVideoBuffer (FPTR info, FPTR userData, DWORD size)
	{
	if (buffers[lastBuffer].info != NULL)
		{
//		GDP("Pipe full");
		GNRAISE(GNR_JPEG_PIPE_FULL);
		}

//GDP("filling %d", lastBuffer);
	buffers[lastBuffer].info     = info;
	buffers[lastBuffer].userData = userData;
	buffers[lastBuffer].byteSize = size;
	lastBuffer = (lastBuffer+1) & (N_DLL_BUFFERS-1);
	nBuffers++;
	GNRAISE_OK;
	}





Error VirtualJPEGUnit055::AllocateVideoBuffer (DWORD size, FPTR __far &info, FPTR __far &data)
	{
	size = (size + 3) & ~3;

#if NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
	FPTR userSpace, kernelSpace;

	KernelMemory * kernelMem = new KernelMemory();
	kernelMem->AllocMemory(size, userSpace, kernelSpace);

	info = kernelMem;
	data = userSpace;

	GNRAISE_OK;
#else
	HGLOBAL handle;
	char __far *ptr;

	handle = GlobalAlloc (GMEM_FIXED, size);
	if (handle == NULL)
		{
		GDP("AllocateVideoBuffer: no alloc size %ld", size);
		}
	else
		{
		if (0 == GlobalPageLock (handle))
			{
			GDP("AllocateVideoBuffer: can't lock handle %ld", (DWORD)handle);
			}
		else
			{
			ptr = GlobalLock (handle);
			if (ptr == NULL)
				{
				GDP("AllocateVideoBuffer: no ptr for handle %ld", (DWORD)handle);
				}
			else
				{
				data = ptr;
				return PrepareBuffer (data, size, info);
				}
			GlobalPageUnlock (handle);
			}
		GlobalFree (handle);
		}
	GNRAISE(GNR_NOT_ENOUGH_MEMORY);
#endif
	}





Error VirtualJPEGUnit055::FreeVideoBuffer (FPTR info)
	{
#if NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
	if (info)
		{
		delete (KernelMemory *)info;
		}
	else
		GNRAISE(GNR_MEM_NOT_ALLOCATED);

	GNRAISE_OK;
#else
	HGLOBAL handle;
	Error err;

	if (info == NULL)
		{
		GDP("FreeVideoBuffer: NULL frag ptr");
		GNRAISE(GNR_MEM_NOT_ALLOCATED);
		}
	err = UnprepareBuffer (info);
	if (IS_ERROR(err))
		{
		GDP("FreeVideoBuffer: can't unprepare, error %08lx", err);
		}
	handle = FP_SEG(info);
	GlobalUnlock (handle);
	GlobalPageUnlock (handle);
	GlobalFree (handle);
#endif
	GNRAISE_OK;
	}





Error VirtualJPEGUnit055::InitPlayback (JPEGPlaybackHook __far *hook)
	{
	int i;


	// Check unit state.
	if (GetState() != current)
		GNRAISE(GNR_OBJECT_NOT_CURRENT);
	if (compress  ||  stillMode)
		GNRAISE(GNR_INVALID_CONFIGURE_STATE);

	if (changed)
		{
		for (i = 0;  i < N_DLL_BUFFERS;  i++)
			{
			buffers[i].info     = NULL;
			buffers[i].userData = NULL;
			}
		nextBuffer = 0;
		lastBuffer = 0;
		nBuffers   = 0;
		fieldNum   = 0;
		lastFieldCnt = 255;
		}

	// Initialize the operation.
	GNREASSERT( manager->InitOperation () );
	transferRunning = FALSE;
	playHook = hook;
	GNRAISE_OK;
	}





Error VirtualJPEGUnit055::FinishPlayback (void)
	{
	if (playHook)
		{
#if NT_KERNEL
		irqMutex.Enter();
#endif
		int wait = (nBuffers > N_DLL_BUFFERS) ? N_DLL_BUFFERS : nBuffers;
		playHook = NULL;
#if NT_KERNEL
		irqMutex.Leave();
#endif
		Timer.WaitMilliSecs (wait*40);   // wait until the last passed buffer has been played
		return manager->StopIRQs ();
		}
	GNRAISE_OK;
	}





Error VirtualJPEGUnit055::PlayBuffer (FPTR info, DWORD size)
	{
	Error err;

	if (manager->writeChannel == NULL  ||  playHook == NULL)
		{
		// No DMA used.
		if (nBuffers > 0) //  &&  buffers[nextBuffer].info == info)
			{
			GDP("removing %ld", (long)nextBuffer);
			nBuffers--;
			buffers[nextBuffer].info = NULL;   // buffer available for adding another buffer
			nextBuffer = (nextBuffer+1) & (N_DLL_BUFFERS-1);
			}
		transferRunning = TRUE;
		err = manager->ExpandIntoOverlayFrame (info, size);
		transferRunning = FALSE;
		}
	else
		{
		// Write the data via DMA.
		_hmemcpy (manager->dmaDataPtr, info, size);
		err = manager->ExpandStart (size);
		if (IS_ERROR(err))
			{
			GDP("ExpandStart failed");
			manager->z055->ReadStatus ();
			}
		else
			{
			transferRunning = TRUE;
			}
		}
	return err;
	}





Error VirtualJPEGUnit055::Start (void)
	{
	if (GetState() != current)
		GNRAISE(GNR_OBJECT_NOT_CURRENT);
	else
		{
		if (compress  ||  playHook)
			GNREASSERT( manager->StartIRQs () );
		manager->Start ();
		GNRAISE_OK;
		}
	}





// Preempt the other virtual unit and program our state into the chips.
// We must be the current unit before this call.

Error VirtualJPEGUnit055::Preempt (VirtualUnit *previous)
	{
	VirtualJPEGUnit055 *prev = (VirtualJPEGUnit055 *)previous;
	JPEGMsg msg;


	if (this != previous  &&  GetState() == current)
		{
		GNREASSERT( coder->Activate () );
		if (prev != NULL)
			{
			// Inform the preempted user that his operation is suspended. He also
			// must switch his video units to pattern mode.
			if (prev->preemptHook)
				{
				msg.userData = NULL;
				prev->preemptHook.Call (msg);
				}
			else
				{
				// Switching from playback to still operation blanks the video output.
				if (stillMode  &&  ! prev->stillMode  &&  ! prev->compress  &&  manager->blankUnit != NULL)
					manager->blankUnit->Activate ();
				}
			}
		}
	GNRAISE_OK;
	}
