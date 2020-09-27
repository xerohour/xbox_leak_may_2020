
// FILE:      library\hardware\jpeg\generic\jcor22.cpp
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   15.01.96
//
// PURPOSE: The virtualized JPEG core implemented as combination of 022/016 chips.
//
// HISTORY:

#include "jcor22.h"

#include "library\common\prelude.h"
#include "library\lowlevel\memmpdio.h"
#include "library\lowlevel\timer.h"
#include "library\general\parse.h"
#include "library\lowlevel\kernlmem.h"
#include "library\hardware\pcibrdge\specific\i22\i22regs.h"
#include "library\hardware\pcibrdge\specific\i22\fragtab.h"
#include "library\hardware\videodma\specific\i20pip.h"
#include "library\hardware\jpeg\specific\z016.h"

#if NT_KERNEL
#include "msgfile.h"
#endif

#define NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS	1

#if NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
#define KMEM(x)	(((KernelMemory *)(x))->KernelSpace())
#else
#define KMEM(x)	x
#endif

#include "library\support\gfxdebug\gfxdebug.h"
#include "library\common\vddebug.h"

//#include <mmsystem.h>



#define N_DLL_BUFFERS  4   // number of buffers possible in the DLL pipeline, must be power of 2

#define N_FRAGTABLES  4   // Fragment tables per STATCOM table

#define MAX_FIELDSIZE  200000



// Compile switches

#define PIP_EXPAND  1   // use PIP for still expansion ?

#define COMPRESS_POLL_EIGHTH_LINE  0   // poll every eighth line completely for still compress ?

#define EXPAND_ONE_FRAGMENT  1   // use one fragment for decompression (I22 chip bug) ?

#define PLAYBACK_ONE_FRAGMENT  1   // enable one fragment for playback (I22 chip bug) ?

#define DO_TIMING  1   // print time needed for operation ?

#define DUMMY_CAPBUFFER  1

#define STARTCAP_DROPFRAMES  0L

#define NT_DEBUGOUTPUT  (NT_KERNEL && 1)

#define DEEP_PIPELINE  0   // make pipeline deeper than N_DLL_BUFFERS ?
#if DEEP_PIPELINE
#undef N_DLL_BUFFERS
#define N_DLL_BUFFERS  8
#endif

#if VIONA_VERSION
#define DO_THUMBNAIL  0

#define SYNCSEL_DECODER  0
#define SYNCSEL_NONE     1
#define SYNCSEL_016      2
#define SYNCSEL_I22      3
#endif





static BOOL InPlayInterrupt;

#if PLAYBACK_ONE_FRAGMENT
static PoolContDMABuffer *PlayBuffers[N_DLL_BUFFERS];
static FragmentTableClass *PlayBufferFrags[N_DLL_BUFFERS];
static FPTR PlayBufferPtr[N_DLL_BUFFERS];
static DWORD PlaySize;
#endif





// This is the bit field that represents the video formats we support.

static DWORD VideoFormatBitField = (1 << VFMT_YUV_411) | (1 << VFMT_YUV_422);










//************************************************************************
// A class that serves as the info structure for prepared video buffers.

class PrepareInfo
	{
	protected:
		FPTR userSpace, kernelSpace;

	public:
		KernelMemory *kernelMem;
		FragmentTableClass *frag;

		PrepareInfo (void) { kernelMem = NULL; frag = NULL; userSpace = kernelSpace = NULL; }
		~PrepareInfo (void) { FreeMemory (); }

		Error PrepareMemory (FPTR buffer, DWORD size);
		Error AllocMemory (DWORD size, FPTR __far &data);
		Error FreeMemory (void);

		FPTR KernelSpace(void) {return kernelSpace;}
	};


Error PrepareInfo::PrepareMemory (FPTR buffer, DWORD size)
	{
	kernelMem = new KernelMemory;
	frag = new FragmentTableClass;
	if (kernelMem == NULL  ||  frag == NULL)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	GNREASSERT( kernelMem->MapMemory (buffer, size, kernelSpace) );
	GNREASSERT( frag->CreateFragmentBuffer (kernelSpace, size) );
	GNRAISE_OK;
	}


Error PrepareInfo::AllocMemory (DWORD size, FPTR __far &data)
	{
	FreeMemory ();
	kernelMem = new KernelMemory;
	frag = new FragmentTableClass;
	if (kernelMem == NULL  ||  frag == NULL)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	GNREASSERT( kernelMem->AllocMemory (size, userSpace, kernelSpace) );
	data = userSpace;
	GNREASSERT( frag->CreateFragmentBuffer (kernelSpace, size) );
	GNRAISE_OK;
	}


Error PrepareInfo::FreeMemory (void)
	{
	delete frag;
	frag = NULL;
	delete kernelMem;
	kernelMem = NULL;
	userSpace = kernelSpace = NULL;
	GNRAISE_OK;
	}





//************************************************************************
// The interrupt handler for JPEG code interrupt.

class JPEGCodeIntHandler : public InterruptHandler
	{
	private:
		JPEGManager022 *jpegManager;

		void Interrupt (void) {jpegManager->JPEGCodeInterrupt();}

	public:
		JPEGCodeIntHandler (JPEGManager022 *jpegManager)
			{
			this->jpegManager = jpegManager;
			}
	};










//************************************************************************
// The JPEG core manager, i.e. the PhysicalUnit.
//************************************************************************


JPEGManager022::JPEGManager022 (MemoryMappedIO *io,
                                InterruptServer *jpegCodeIntServer,
                                Profile *profile,
                                Z016Class *z016,
                                BitOutputPort *pxoe016Bit,
                                BitOutputPort *resetJPEGBit,
                                BitOutputPort *clken050Bit,
#if VIONA_VERSION
                                ByteOutputPort *syncSelection,
                                BitOutputPort *decoderSyncEnable,
                                BitOutputPort *frontEndEnable,
                                BitOutputPort *videoBusDecode,
                                VirtualUnit *blankUnit,
                                BOOL fixPlaybackLongBursts,
                                KernelLogger *log,
                                BOOL ccirMode,
                                AllowedJPEGVStandard allowedStandard)
#else
                                BitOutputPort *syncMUX,
                                BOOL fixPlaybackLongBursts)
#endif
	{
	Error err;
	int i;


	this->io = io;
	this->jpegCodeIntServer = jpegCodeIntServer;
	this->profile = profile;
	this->z016 = z016;
	this->pxoe016Bit  = pxoe016Bit;
	this->resetJPEGBit = resetJPEGBit;
	this->clken050Bit = clken050Bit;
#if VIONA_VERSION
	this->syncSelection = syncSelection;
	this->decoderSyncEnable = decoderSyncEnable;
	this->frontEndEnable = frontEndEnable;
	this->videoBusDecode = videoBusDecode;
	this->blankUnit = blankUnit;
	this->log = log;
	this->ccirMode = ccirMode;
	this->allowedStandard = allowedStandard;
#else
	this->syncMUX = syncMUX;
#endif

	this->fixPlaybackLongBursts = fixPlaybackLongBursts;

	jpegCodeIntHandler = new JPEGCodeIntHandler (this);
	handlerEnabled = FALSE;

	jpegCoreRunning = FALSE;

	// Read code thresholds.
#if VIONA_VERSION
	profile->Read (__TEXT("Q22DRV"), __TEXT("PlaybackCodeThreshold"), playbackCodeThreshold, 10,  20);
	profile->Read (__TEXT("Q22DRV"), __TEXT("CaptureCodeThreshold"),  captureCodeThreshold,  10,  80);
	profile->Read (__TEXT("Q22DRV"), __TEXT("ExpandCodeThreshold"),   expandCodeThreshold,   10, 120);
	profile->Read (__TEXT("Q22DRV"), __TEXT("CompressCodeThreshold"), compressCodeThreshold, 10,  80);
#else
	profile->Read (__TEXT("H22DRV"), __TEXT("PlaybackCodeThreshold"), playbackCodeThreshold, 10,  20);
	profile->Read (__TEXT("H22DRV"), __TEXT("CaptureCodeThreshold"),  captureCodeThreshold,  10,  80);
	profile->Read (__TEXT("H22DRV"), __TEXT("ExpandCodeThreshold"),   expandCodeThreshold,   10, 120);
	profile->Read (__TEXT("H22DRV"), __TEXT("CompressCodeThreshold"), compressCodeThreshold, 10,  80);
#endif

#if PLAYBACK_ONE_FRAGMENT
	for (i = 0;  i < N_DLL_BUFFERS;  i++)
		{
		PlayBuffers[i] = NULL;
		PlayBufferFrags[i] = NULL;
		}
#if NT_KERNEL
	err = profile->Read (__TEXT("Q22DRV"), __TEXT("PlaybackLongBurstSize"), i, MAX_FIELDSIZE);
	PlaySize = i;
#else
	err = profile->Read (__TEXT("Q22DRV"), __TEXT("PlaybackLongBurstSize"), PlaySize, (int)10, (DWORD)MAX_FIELDSIZE);
#endif
	if (IS_ERROR(err))
		{
#if NT_DEBUGOUTPUT
		DbgPrint("Registry read err %08lx\n", err);
#endif
		PlaySize = MAX_FIELDSIZE;
		}
#endif

	Reset ();
	pxoe016Bit->OutBit (LOW);   // disable PXOUT bus
	io->ClearBit (I22REG_SYNCMSTR);
	syncSelection->OutByte (SYNCSEL_DECODER);
	decoderSyncEnable->OutBit (HIGH);
	frontEndEnable->OutBit (HIGH);
	videoBusDecode->OutBit (LOW);
	}





JPEGManager022::~JPEGManager022 (void)
	{
	delete jpegCodeIntHandler;
	jpegCodeIntHandler = NULL;

#if PLAYBACK_ONE_FRAGMENT
	for (int i = 0;  i < N_DLL_BUFFERS;  i++)
		{
		delete PlayBufferFrags[i];
		delete PlayBuffers[i];
		}
#endif
	}





VirtualUnit *JPEGManager022::CreateVirtual (void)
	{
	return NULL;   // NOT USED YET
	}





// JPEG code interrupt.

void JPEGManager022::JPEGCodeInterrupt (void)
	{
	GetCurrent()->JPEGCodeInterrupt();
	}





// Start or stop the JPEG core operation.

void JPEGManager022::Start (void)
	{
	VirtualJPEGUnit022 *jpeg = GetCurrent ();


	if (! jpeg->stillExpansion)
		io->SetBit (I22REG_P_RESET);   // release JPEG process reset

	z016->Start();

	if (jpeg->compress)
		jpeg->coder->Start ();

	io->SetBit (I22REG_COD_TRNS_EN);

	if (! jpeg->stillExpansion)
		io->SetBit (I22REG_ACTIVE);

	// Don't start the 050. The 050 will be started by the I22.

	jpegCoreRunning = TRUE;
	}



void JPEGManager022::Stop (void)
	{
	io->ClearBit (I22REG_P_RESET);   // reset JPEG process
	z016->Stop ();
	pxoe016Bit->OutBit (LOW);   // disable PXOUT bus
#if VIONA_VERSION
	// Switch to capture so that video input can reach the I22.
	if (! GetCurrent()->stillMode)
		syncSelection->OutByte (SYNCSEL_DECODER);
	decoderSyncEnable->OutBit (HIGH);
	frontEndEnable->OutBit (HIGH);
	videoBusDecode->OutBit (LOW);
#else
	syncMUX->OutBit (0);
#endif
	io->ClearBit (I22REG_ACTIVE);
	io->ClearBit (I22REG_COD_TRNS_EN);

	io->ClearBit (I22REG_SYNCMSTR);

	jpegCoreRunning = FALSE;
	}





void JPEGManager022::Reset (void)
	{
	// Reset the JPEG core (050 & 016).
	clken050Bit->OutBit (LOW);
	clken050Bit->OutBit (HIGH);   // enable 050 clock, otherwise no reset possible
	Timer.WaitMicroSecs (2500);
	resetJPEGBit->OutBit (HIGH);
	Timer.WaitMicroSecs (10);   // 050 needs reset for at least four CLK_INs
	resetJPEGBit->OutBit (LOW);
	}





// Note that "statCom" parameter is the physical address of the STATCOM table.

Error JPEGManager022::InitOperation (DWORD statCom)
	{
	VirtualJPEGUnit022 *jpeg = GetCurrent ();
	Z016_CONFIG *c16;
	DWORD reg;
	int oddEven, codeThreshold;
	Error err;
	int coreTries;


	// Configure the hardware due to compression or changes.
	if (jpeg->changed  ||  jpeg->compress)
		{
//GDP("Programming JPEG core");

		jpeg->CalcVideoParameters ();

		
		// Stop all other JPEG operations.
		Stop ();


		err = GNR_OK;
		coreTries = 5;

		do {
			Reset ();

			// Configure the Z016.
GDP("op width is %ld", (long)jpeg->opWidth);
			c16 = new Z016_CONFIG;
			if (c16 == NULL)
				GNRAISE(GNR_NOT_ENOUGH_MEMORY);
			c16->compress    = jpeg->compress;
			c16->stillMode   = FALSE; //jpeg->stillMode;
			c16->thumbnail   = FALSE;
			c16->colFiltersOn= FALSE; //jpeg->filtersOn;
			c16->horFilterOn = TRUE; //jpeg->filtersOn;
			c16->vDecimation = FALSE;
			c16->coderFormat = jpeg->vFormat;
			c16->pxoutFormat = VFMT_YUV_422;

			c16->hDecimation = jpeg->horDecimation;
			c16->width  = jpeg->opWidth;
			c16->height = jpeg->opHeight;
			c16->left   = jpeg->nax;
			c16->top    = jpeg->nay - 1;   // note that I22 starts one line later than 016
//GDP("016 %d,%d size %d,%d", c16->left, c16->top, c16->width, c16->height);

			if (jpeg->compress)
				{
				// Compression.
				if (jpeg->stillMode)
					{
					c16->vbusFormat = VFMT_RGB_444;   // still compression
					c16->ccirLevels = jpeg->ccirLevels;
					}
				else
					c16->vbusFormat = VFMT_YUV_422;   // motion compression
				// c16->pxoutFormat does not matter since PXOUT bus disabled.
				pxoe016Bit->OutBit (LOW);   // disable PXOUT bus
				}
			else
				{
				// Expansion. c16->vbusFormat does not matter.
				if (jpeg->jpgMode == I22_STILL_DECOMPRESS)
					{
					c16->pxoutFormat = VFMT_RGB_444;
					c16->thumbnail = jpeg->thumbnail;   // still expansion
					jpeg->nax += jpeg->horDecimation ? 24 : 20;
					}
				pxoe016Bit->OutBit (HIGH);   // enable PXOUT bus
				}

//GDP("016 %d,%d size %d,%d", c16->left, c16->top, c16->width, c16->height);
			// c16.doubleClock, c16.syen are always zero.
			if (! z016->Configure (c16) )
				{
#if NT_DEBUGOUTPUT
				DbgPrint ("InitOp 016 fails\n");
#endif
				err = GNR_RANGE_VIOLATION;
				}
			delete c16;


			if (! IS_ERROR(err))
				{
#if VIONA_VERSION
				BOOL playback = (jpeg->jpgMode == I22_MOTION_DECOMPRESS  &&  !jpeg->stillExpansion);
				if (! jpeg->stillMode)
					syncSelection->OutByte (playback ? SYNCSEL_016 : SYNCSEL_DECODER);
				decoderSyncEnable->OutBit (jpeg->jpgMode == I22_MOTION_COMPRESS);
				frontEndEnable->OutBit (jpeg->jpgMode == I22_MOTION_COMPRESS  ||  playback);
				videoBusDecode->OutBit (playback);
#else
				syncMUX->OutBit (jpeg->jpgMode == I22_MOTION_DECOMPRESS);
#endif


				// Configure the Z050. To switch in a (possibly new) clock, the 050 needs
				// at least 5000 clocks to stabilize PLLs.
				clken050Bit->OutBit (LOW);  // disable 050 clock
				Timer.WaitMicroSecs (20);
				clken050Bit->OutBit (HIGH);  // enable 050 clock
				Timer.WaitMicroSecs (300);
				err = jpeg->coder->InitOperation ();
				if (IS_ERROR(err))
					{
#if NT_DEBUGOUTPUT
					DbgPrint ("InitOp 050 fails %08lx\n", err);
#endif
					GDP("050 time out");
					}
				}
			} while (IS_ERROR(err)  &&  --coreTries > 0);
		if (IS_ERROR(err))
			return err;


		// Configure the I22.
		io->ClearBit (I22REG_P_RESET);   // reset JPEG process
		io->SetBit (I20REG_CODFLUSH);   // flush CFIFO

		oddEven = HIGH;
		switch (jpeg->jpgMode)
			{
			case I22_MOTION_COMPRESS:
				if (ccirMode)
					{
					if (jpeg->vStandard == VSTD_NTSC)
						oddEven = (jpeg->fieldOrder != VFLD_EVEN);
					else
						oddEven = (jpeg->fieldOrder != VFLD_EVEN);
					}
				else
					{
					if (jpeg->vStandard == VSTD_NTSC)
						oddEven = (jpeg->fieldOrder != VFLD_EVEN);
					else
						oddEven = (jpeg->fieldOrder != VFLD_ODD);
					}
				break;
			case I22_STILL_COMPRESS:
				oddEven = HIGH;
				break;
			case I22_STILL_DECOMPRESS:
				oddEven = HIGH;
				break;
			case I22_MOTION_DECOMPRESS:
				switch (jpeg->fieldOrder)
					{
					case VFLD_SINGLE:  oddEven = !jpeg->stillExpansion;  break;
					case VFLD_ODD:     oddEven = LOW;   break;
					case VFLD_EVEN:    oddEven = HIGH;  break;
					}
				break;
			}

		// Set code threshold depending on compression/expansion.
		if (jpeg->compress)
			codeThreshold = jpeg->stillMode ? compressCodeThreshold : captureCodeThreshold;
		else
			codeThreshold = jpeg->stillExpansion ? expandCodeThreshold : playbackCodeThreshold;
		io->WriteBits (I22REG_COD_THRESHOLD, codeThreshold);
//GDP("codeThreshold %d", codeThreshold);

		reg =	MKBF (I22IDX_JPG,           TRUE   ) |
				MKBF (I22IDX_JPGMODE,       jpeg->jpgMode) |
#if COMPRESS_POLL_EIGHTH_LINE
				MKBF (I22IDX_RTBSY_FB,      jpeg->stillMode) |
#else
				MKBF (I22IDX_RTBSY_FB,      jpeg->stillExpansion) |
#endif
				MKBF (I22IDX_GO_ENABLE,     LOW    ) |   // will be set after 050 is inited
				MKBF (I22IDX_SYNCMSTR,      jpeg->syncMaster) |
				MKBF (I22IDX_FLD_PER_BUFF,  (jpeg->fieldOrder == VFLD_SINGLE)) |
#if PIP_EXPAND
				MKBF (I22IDX_VFIFO_FB,      jpeg->stillExpansion) |   // only for "still like motion expansion"
#else
				MKBF (I22IDX_VFIFO_FB,      LOW    ) |
#endif
				MKBF (I22IDX_CFIFO_FB,      LOW    ) |
				MKBF (I22IDX_STILL_LENDIAN, TRUE   );
		io->WriteDWord (I22REG_JMODE, reg);
//GDP("JMODE %8lx", reg);

		reg =	MKBF (I22IDX_VSYNCSIZE, jpeg->vSyncSize) |
				MKBF (I22IDX_FRMTOT,    jpeg->frameTotal);
		io->WriteDWord (I22REG_VERTSYNC, reg);
//GDP("VERTSYNC %8lx", reg);

		reg =	MKBF (I22IDX_HSYNCSTART, jpeg->hSyncStart) |
				MKBF (I22IDX_LINETOT,    jpeg->lineTotal);
		io->WriteDWord (I22REG_HORZSYNC, reg);
//GDP("HORZSYNC %8lx", reg);

		reg =	MKBF (I22IDX_NAX, jpeg->nax) |
				MKBF (I22IDX_PAX, jpeg->pax);
		io->WriteDWord (I22REG_HORZACTIVE, reg);
//GDP("HORZACTIVE %8lx", reg);

		reg =	MKBF (I22IDX_NAY, jpeg->nay) |
				MKBF (I22IDX_PAY, jpeg->pay);
		io->WriteDWord (I22REG_VERTACTIVE, reg);
//GDP("VERTACTIVE %8lx", reg);

		reg = MKBF (I22IDX_ODD_EVEN, oddEven);
		io->WriteDWord (I22REG_FIELDPARAS, reg);
//GDP("ODDEVEN %8lx", reg);

		io->WriteDWord (I22REG_I_STAT_COM, statCom);

		io->SetBit (I22REG_GO_ENABLE);   // to enable JPEG GO cycle
		io->ClearBit (I20REG_CODFLUSH);   // ensure that CFIFO is not constantly flushed
		}


	jpeg->changed = FALSE;

	GNRAISE_OK;
	}





Error JPEGManager022::StartIRQs (void)
	{
	if (! handlerEnabled)
		{
		GNREASSERT( jpegCodeIntServer->InstallHandler (jpegCodeIntHandler) );
		GNREASSERT( jpegCodeIntHandler->Enable () );
		handlerEnabled = TRUE;
		}
	GNRAISE_OK;
	}



Error JPEGManager022::StopIRQs (void)
	{
	if (handlerEnabled)
		{
		handlerEnabled = FALSE;
		GNREASSERT( jpegCodeIntHandler->Disable () );
		}
	return jpegCodeIntServer->RemoveHandler (jpegCodeIntHandler);
	}





	static void WriteStillLinePolled (FPTR stillReg, FPTR src, WORD width, WORD totalWidth)
		{
#if NT_KERNEL || VXD_VERSION
		__asm {
				push	esi

				mov	ebx, stillReg
				mov	esi, src
				mov	cx, width
				sub	cx, 1
				jz		lastPixel
		loop1:
				// Write a pixel to the I22.
				mov	eax, [esi]
				mov	[ebx], eax
				add	esi, 3
		loop2:
				// Poll the pixel.
				mov	eax, [ebx]
				or		eax, eax
				js		loop2

				sub	cx, 1
				jnz	loop1

		lastPixel:
				// The last pixel must be read as 3 bytes, not as four (otherwise
				// selector protection can cause GPF).
				// Write the last pixel to the I22.
				xor	eax, eax
				mov	al, [esi+2]
				shl	eax, 16
				mov	ax, [esi]
				mov	[ebx], eax
		loop3:
				// Poll the pixel.
				mov	edx, [ebx]
				or		edx, edx
				js		loop3
				}
#if VIONA_VERSION
		__asm {
				// Repeat the last pixel until width is a multiple of 16.
				mov	dx, totalWidth
				mov	cx, 16
				and	dx, 15
				jz		noFill
				sub	cx, dx
		loop4:
				mov	[ebx], eax
		loop5:
				// Poll the pixel.
				mov	edx, [ebx]
				or		edx, edx
				js		loop5

				sub	cx, 1
				jnz	loop4
		noFill:
				}
#endif
		__asm {
				pop	esi
				}
#else
		__asm {
				push	ds
				_emit	0x66
				push	si   // push esi

				les	bx, stillReg
				_emit	0x66
				xor	si, si   // xor esi,esi
				lds	si, src
				mov	cx, width
				sub	cx, 1
				jz		lastPixel
		loop1:
				// Write a pixel to the I22.
				_emit	0x67
				_emit	0x66
				_emit	0x8b
				_emit	0x06   // mov eax, [esi]
				_emit	0x66
				mov	es:[bx], ax   // mov es:[bx],eax
				_emit	0x66
				add	si, 3   // add esi,3
		loop2:
				// Poll the pixel.
				_emit	0x66
				mov	ax, es:[bx]   // mov eax,es:[bx]
				_emit	0x66
				or		ax, ax   // or eax,eax
				js		loop2

				sub	cx, 1
				jnz	loop1

		lastPixel:
				// The last pixel must be read as 3 bytes, not as four (otherwise
				// selector protection can cause GPF).
				// Write the last pixel to the I22.
				_emit	0x66
				xor	ax, ax   // xor eax,eax
				_emit	0x67
				_emit	0x8A
				_emit	0x46
				_emit	0x02   // mov al,[esi+2]
				_emit	0x66
				shl	ax, 16   // shl eax,16
				_emit	0x67
				_emit	0x8B
				_emit	0x06   // mov ax, [esi]
				_emit	0x66
				mov	es:[bx], ax   // mov es:[bx],eax
		loop3:
				// Poll the pixel.
				_emit	0x66
				mov	dx, es:[bx]   // mov edx,es:[bx]
				_emit	0x66
				or		dx, dx   // or edx,edx
				js		loop3
				}
#if VIONA_VERSION
		__asm {
				// Repeat the last pixel until width is a multiple of 16.
				mov	dx, totalWidth
				mov	cx, 16
				and	dx, 15
				jz		noFill
				sub	cx, dx
		loop4:
				_emit	0x66
				mov	es:[bx], ax   // mov es:[bx],eax
		loop5:
				// Poll the pixel.
				_emit	0x66
				mov	dx, es:[bx]   // mov edx,es:[bx]
				_emit	0x66
				or		dx, dx   // or edx,edx
				js		loop5

				sub	cx, 1
				jnz	loop4
		noFill:
				}
#endif
		__asm {
				_emit	0x66
				pop	si   // pop esi
				pop	ds
				}
#endif
		}


	static void WriteStillLineNonPolled (FPTR stillReg, FPTR src, WORD width, WORD totalWidth)
		{
#if NT_KERNEL || VXD_VERSION
		__asm {
				push	esi

				mov	ebx, stillReg
				mov	esi, src
				mov	cx, width
				sub	cx, 1
				jz		lastPixel
		loop1:
				// Write a pixel to the I22.
				mov	eax, [esi]
				add	esi, 3
				sub	cx, 1
				mov	[ebx], eax   // (does not change flags)
				jnz	loop1

		lastPixel:
				// The last pixel must be read as 3 bytes, not as four (otherwise
				// selector protection can cause GPF).
				// Write the last pixel to the I22.
				xor	eax, eax
				mov	al, [esi+2]
				shl	eax, 16
				mov	ax, [esi]
				mov	[ebx], eax
				}
#if VIONA_VERSION
		__asm {
				// Repeat the last pixel until width is a multiple of 16.
				mov	dx, totalWidth
				mov	cx, 16
				and	dx, 15
				jz		noFill
				sub	cx, dx
		loop2:
				sub	cx, 1
				mov	[ebx], eax   // (does not change flags)
				jnz	loop2
		noFill:
				}
#endif
		__asm {
				pop	esi
				}
#else
		__asm {
				push	ds
				_emit	0x66
				push	si   // push esi

				les	bx, stillReg
				_emit	0x66
				xor	si, si   // xor esi,esi
				lds	si, src
				mov	cx, width
				sub	cx, 1
				jz		lastPixel
		loop1:
				// Write a pixel to the I22.
				_emit	0x67
				_emit	0x66
				_emit	0x8b
				_emit	0x06   // mov eax, [esi]
				_emit	0x66
				add	si, 3   // add esi,3
				sub	cx, 1
				_emit	0x66
				mov	es:[bx], ax   // mov es:[bx],eax (does not change flags)
				jnz	loop1

		lastPixel:
				// The last pixel must be read as 3 bytes, not as four (otherwise
				// selector protection can cause GPF).
				// Write the last pixel to the I22.
				_emit	0x66
				xor	ax, ax   // xor eax,eax
				_emit	0x67
				_emit	0x8A
				_emit	0x46
				_emit	0x02   // mov al,[esi+2]
				_emit	0x66
				shl	ax, 16   // shl eax,16
				_emit	0x67
				_emit	0x8B
				_emit	0x06   // mov ax, [esi]
				_emit	0x66
				mov	es:[bx], ax   // mov es:[bx],eax
				}
#if VIONA_VERSION
		__asm {
				// Repeat the last pixel until width is a multiple of 16.
				mov	dx, totalWidth
				mov	cx, 16
				and	dx, 15
				jz		noFill
				sub	cx, dx
		loop2:
				sub	cx, 1
				_emit	0x66
				mov	es:[bx], ax   // mov es:[bx],eax (does not change flags)
				jnz	loop2
		noFill:
				}
#endif
		__asm {
				_emit	0x66
				pop	si   // pop esi
				pop	ds
				}
#endif
		}





Error JPEGManager022::WriteStillPixels24 (FPTR src, WORD width, WORD height,
                                          VideoField fieldOrder)
	{
	VirtualJPEGUnit022 *jpeg = GetCurrent ();
	DWORD timeout;
	DWORD reg, old, stride;
	BYTE __huge *data;
	WORD fields, field, lines, line, y;
	BOOL busy;
	Error err;
	volatile DWORD __far * stillReg;


	// Configure WaitState parameter to 4.
	io->ReadDWord (I20REG_SYSCTRL, old);
	reg = WRBF (old, I20IDX_WAITSTATE, 4);
	io->WriteDWord (I20REG_SYSCTRL, reg);

	// Note: We are working with little endian here, thus assuming fixed
	// organization of the still transfer register.

	// Note that bitmaps have the lines reversed.
	if (fieldOrder == VFLD_SINGLE)
		{
		fields = 1;
		lines = height;
		stride = (3L * width + 3) & ~3;
		data = (BYTE __huge *)src + (stride * (lines-1));
		}
	else // two fields
		{
		fields = 2;
		lines = height / 2;
		stride = (2L * 3L * width + 3) & ~3;
		data = (BYTE __huge *)src + stride * lines - ((3L * width + 3) & ~3);
		}

	io->GetDWordPtr (I22REG_STILL_TRANSFER, stillReg);

	// Write the first pixel and assure it was transferred.
	io->SetBit (I22REG_P_RESET);   // release JPEG process reset
	z016->Start ();
	jpeg->coder->Start ();
	io->SetBit (I22REG_COD_TRNS_EN);
	io->WriteDWord (I22REG_STILL_TRANSFER, *((DWORD __far *)data));
	io->SetBit (I22REG_ACTIVE);
	do { io->ReadBit (I22REG_STILL_BSY, busy); } while (busy);
#if DO_TIMING
	DWORD start = Timer.GetMilliTicks ();
#endif

	// Write the rest of the first line in one run.
	WriteStillLineNonPolled ((FPTR)stillReg, data+3, width-1, width);
	data -= stride;
	y = 6;

	// Do each field.
	for (field = 0;  field < fields;  field++)
		{
		// Use wait state writing.
		for (line = 0;  line < lines;  )
			{
			line += 8;
#if VIONA_VERSION
			WORD fillLines;
			if (line > lines)
				{
				fillLines = line - lines;
				y -= fillLines - 1;
				GDP("compress repeat %ld lines", (long)fillLines);
				}
			else
				fillLines = 0;
#endif
			// The next lines can be written in one run, polling the first pixel.
			while (y > 0)
				{
				io->WriteDWord (I22REG_STILL_TRANSFER, *((DWORD __far *)data));
				do { io->ReadBit (I22REG_STILL_BSY, busy); } while (busy);
				WriteStillLineNonPolled ((FPTR)stillReg, data+3, width-1, width);
				data -= stride;
				y--;
				}
#if VIONA_VERSION
			if (fillLines > 0)
				{
				// Repeat the last line until height is a multiple of 8.
				data += stride;   // read the last line again
				y = fillLines - 1;
				while (y > 0)
					{
					io->WriteDWord (I22REG_STILL_TRANSFER, *((DWORD __far *)data));
					do { io->ReadBit (I22REG_STILL_BSY, busy); } while (busy);
					WriteStillLineNonPolled ((FPTR)stillReg, data+3, width-1, width);
					y--;
					}
				}
#endif
			y = 7;
#if COMPRESS_POLL_EIGHTH_LINE
			// Poll all pixels in every eighth line.
			WriteStillLinePolled ((FPTR)stillReg, data, width, width);
#else
			// Poll only the first pixel.
			io->WriteDWord (I22REG_STILL_TRANSFER, *((DWORD __far *)data));
			do { io->ReadBit (I22REG_STILL_BSY, busy); } while (busy);
			WriteStillLineNonPolled ((FPTR)stillReg, data+3, width-1, width);
#endif
			data -= stride;
			}
		// Prepare data pointer to next field.
		data = (BYTE __huge *)src + (stride * (lines-1));
		}

	// Write a dummy pixel to end the last field.
	io->WriteDWord (I22REG_STILL_TRANSFER, 0L);

	// Wait for end of operation.
	err = GNR_OK;
	timeout = Timer.GetMilliTicks () + 20;
	while (0 == (jpeg->statCom[0] & 0x01))
		{
		if (timeout < Timer.GetMilliTicks ())
			{
GDP("compress TIMEOUT !");
			err = GNR_JPEG_TIMEOUT;
			break;
			}
		}
#if DO_TIMING
	GDP("compression time %ld", Timer.GetMilliTicks() - start);
#endif

	Stop ();

	io->WriteDWord (I20REG_SYSCTRL, old);
	if (! IS_ERROR(err))
		{
		((VirtualJCoderRated *)(jpeg->coder))->GetLastScaleFactor ();
		}
	return err;
	}





#if VIONA_VERSION


	static void WriteStillLinePolled32 (FPTR stillReg, FPTR src, WORD width, WORD totalWidth)
		{
#if NT_KERNEL || VXD_VERSION
		__asm {
				push	esi

				mov	ebx, stillReg
				mov	esi, src
				mov	cx, width
		loop1:
				// Write a pixel to the I22.
				mov	eax, [esi]
				mov	[ebx], eax
				add	esi, 4
		loop2:
				// Poll the pixel.
				mov	edx, [ebx]
				or		edx, edx
				js		loop2

				sub	cx, 1
				jnz	loop1

				// Repeat the last pixel until width is a multiple of 16.
				mov	dx, totalWidth
				mov	cx, 16
				and	dx, 15
				jz		noFill
				sub	cx, dx
		loop4:
				mov	[ebx], eax
		loop5:
				// Poll the pixel.
				mov	edx, [ebx]
				or		edx, edx
				js		loop5

				sub	cx, 1
				jnz	loop4
		noFill:
				pop	esi
				}
#else
		__asm {
				push	ds
				_emit	0x66
				push	si   // push esi

				les	bx, stillReg
				_emit	0x66
				xor	si, si   // xor esi,esi
				lds	si, src
				mov	cx, width
		loop1:
				// Write a pixel to the I22.
				_emit	0x67
				_emit	0x66
				_emit	0x8b
				_emit	0x06   // mov eax, [esi]
				_emit	0x66
				mov	es:[bx], ax   // mov es:[bx],eax
				_emit	0x66
				add	si, 4   // add esi,4
		loop2:
				// Poll the pixel.
				_emit	0x66
				mov	dx, es:[bx]   // mov edx,es:[bx]
				_emit	0x66
				or		dx, dx   // or edx,edx
				js		loop2

				sub	cx, 1
				jnz	loop1

				// Repeat the last pixel until width is a multiple of 16.
				mov	dx, totalWidth
				mov	cx, 16
				and	dx, 15
				jz		noFill
				sub	cx, dx
		loop4:
				_emit	0x66
				mov	es:[bx], ax   // mov es:[bx],eax
		loop5:
				// Poll the pixel.
				_emit	0x66
				mov	dx, es:[bx]   // mov edx,es:[bx]
				_emit	0x66
				or		dx, dx   // or edx,edx
				js		loop5

				sub	cx, 1
				jnz	loop4
		noFill:
				_emit	0x66
				pop	si   // pop esi
				pop	ds
				}
#endif
		}


	static void WriteStillLineNonPolled32 (FPTR stillReg, FPTR src, WORD width, WORD totalWidth)
		{
#if NT_KERNEL || VXD_VERSION
		__asm {
				push	esi

				mov	ebx, stillReg
				mov	esi, src
				mov	cx, width
		loop1:
				// Write a pixel to the I22.
				mov	eax, [esi]
				add	esi, 4
				sub	cx, 1
				mov	[ebx], eax   // (does not change flags)
				jnz	loop1

				// Repeat the last pixel until width is a multiple of 16.
				mov	dx, totalWidth
				mov	cx, 16
				and	dx, 15
				jz		noFill
				sub	cx, dx
		loop2:
				sub	cx, 1
				mov	[ebx], eax   // (does not change flags)
				jnz	loop2
		noFill:
				pop	esi
				}
#else
		__asm {
				push	ds
				_emit	0x66
				push	si   // push esi

				les	bx, stillReg
				_emit	0x66
				xor	si, si   // xor esi,esi
				lds	si, src
				mov	cx, width
		loop1:
				// Write a pixel to the I22.
				_emit	0x67
				_emit	0x66
				_emit	0x8b
				_emit	0x06   // mov eax, [esi]
				_emit	0x66
				add	si, 4   // add esi,4
				sub	cx, 1
				_emit	0x66
				mov	es:[bx], ax   // mov es:[bx],eax (does not change flags)
				jnz	loop1

				// Repeat the last pixel until width is a multiple of 16.
				mov	dx, totalWidth
				mov	cx, 16
				and	dx, 15
				jz		noFill
				sub	cx, dx
		loop2:
				sub	cx, 1
				_emit	0x66
				mov	es:[bx], ax   // mov es:[bx],eax (does not change flags)
				jnz	loop2
		noFill:
				_emit	0x66
				pop	si   // pop esi
				pop	ds
				}
#endif
		}



Error JPEGManager022::WriteStillPixels32 (FPTR src, WORD width, WORD height,
                                          VideoField fieldOrder)
	{
	VirtualJPEGUnit022 *jpeg = GetCurrent ();
	DWORD timeout;
	DWORD reg, old, stride;
	BYTE __huge *data;
	WORD fields, field, lines, line, y;
	BOOL busy;
	Error err;
	volatile DWORD __far * stillReg;


	// Configure WaitState parameter to 4.
	io->ReadDWord (I20REG_SYSCTRL, old);
	reg = WRBF (old, I20IDX_WAITSTATE, 4);
	io->WriteDWord (I20REG_SYSCTRL, reg);

	// Note: We are working with little endian here, thus assuming fixed
	// organization of the still transfer register.

	// Note that bitmaps have the lines reversed.
	if (fieldOrder == VFLD_SINGLE)
		{
		fields = 1;
		lines = height;
		stride = 4L * width;
		data = (BYTE __huge *)src + (stride * (lines-1));
		}
	else // two fields
		{
		fields = 2;
		lines = height / 2;
		stride = 2L * 4L * width;
		data = (BYTE __huge *)src + stride * lines - 4L * width;
		}

	io->GetDWordPtr (I22REG_STILL_TRANSFER, stillReg);

	// Write the first pixel and assure it was transferred.
	io->SetBit (I22REG_P_RESET);   // release JPEG process reset
	z016->Start ();
	jpeg->coder->Start ();
	io->SetBit (I22REG_COD_TRNS_EN);
	io->WriteDWord (I22REG_STILL_TRANSFER, *((DWORD __far *)data));
	io->SetBit (I22REG_ACTIVE);
	do { io->ReadBit (I22REG_STILL_BSY, busy); } while (busy);
#if DO_TIMING
	DWORD start = Timer.GetMilliTicks ();
#endif

	// Write the rest of the first line in one run.
	WriteStillLineNonPolled32 ((FPTR)stillReg, data+4, width-1, width);
	data -= stride;
	y = 6;

	// Do each field.
	for (field = 0;  field < fields;  field++)
		{
		// Use wait state writing.
		for (line = 0;  line < lines;  )
			{
			line += 8;
#if VIONA_VERSION
			WORD fillLines;
			if (line > lines)
				{
				fillLines = line - lines;
				y -= fillLines - 1;
				GDP("compress repeat %ld lines", (long)fillLines);
				}
			else
				fillLines = 0;
#endif
			// The next lines can be written in one run, polling the first pixel.
			while (y > 0)
				{
				io->WriteDWord (I22REG_STILL_TRANSFER, *((DWORD __far *)data));
				do { io->ReadBit (I22REG_STILL_BSY, busy); } while (busy);
				WriteStillLineNonPolled32 ((FPTR)stillReg, data+4, width-1, width);
				data -= stride;
				y--;
				}
#if VIONA_VERSION
			if (fillLines > 0)
				{
				// Repeat the last line until height is a multiple of 8.
				data += stride;   // read the last line again
				y = fillLines - 1;
				while (y > 0)
					{
					io->WriteDWord (I22REG_STILL_TRANSFER, *((DWORD __far *)data));
					do { io->ReadBit (I22REG_STILL_BSY, busy); } while (busy);
					WriteStillLineNonPolled32 ((FPTR)stillReg, data+4, width-1, width);
					y--;
					}
				}
#endif
			y = 7;
#if COMPRESS_POLL_EIGHTH_LINE
			// Poll all pixels in every eighth line.
			WriteStillLinePolled32 ((FPTR)stillReg, data, width, width);
#else
			// Poll only the first pixel.
			io->WriteDWord (I22REG_STILL_TRANSFER, *((DWORD __far *)data));
			do { io->ReadBit (I22REG_STILL_BSY, busy); } while (busy);
			WriteStillLineNonPolled32 ((FPTR)stillReg, data+4, width-1, width);
#endif
			data -= stride;
			}
		// Prepare data pointer to next field.
		data = (BYTE __huge *)src + (stride * (lines-1));
		}

	// Write a dummy pixel to end the last field.
	io->WriteDWord (I22REG_STILL_TRANSFER, 0L);

	// Wait for end of operation.
	err = GNR_OK;
	timeout = Timer.GetMilliTicks () + 20;
	while (0 == (jpeg->statCom[0] & 0x01))
		{
		if (timeout < Timer.GetMilliTicks ())
			{
GDP("compress TIMEOUT !");
			err = GNR_JPEG_TIMEOUT;
			break;
			}
		}
#if DO_TIMING
	GDP("compression time %ld", Timer.GetMilliTicks() - start);
#endif

	Stop ();

	io->WriteDWord (I20REG_SYSCTRL, old);
	if (! IS_ERROR(err))
		{
		((VirtualJCoderRated *)(jpeg->coder))->GetLastScaleFactor ();
		}
	return err;
	}



	static void ReadStillLinePolled (FPTR stillReg, FPTR dst, WORD width)
		{
#if NT_KERNEL || VXD_VERSION
		__asm {
				push	esi

				mov	ebx, stillReg
				mov	esi, dst
				mov	cx, width
				sub	cx, 1
				jz		lastPixel
		loop1:
				// Poll the pixel from the I22.
				mov	eax, [ebx]
				or		eax, eax
				js		loop1

				// Store the pixel.
				mov	[esi], eax
				add	esi, 3
				sub	cx, 1
				jnz	loop1

		lastPixel:
				// The last pixel must be written as 3 bytes, not as four (otherwise
				// selector protection can cause GPF).
				// Poll the pixel from the I22.
				mov	eax, [ebx]
				or		eax, eax
				js		lastPixel

				// Store the pixel.
				mov	[esi], ax
				shr	eax, 16
				mov	[esi+2], al

				pop	esi
				}
#else
		__asm {
				push	ds
				_emit	0x66
				push	si   // push esi

				les	bx, stillReg
				_emit	0x66
				xor	si, si   // xor esi,esi
				lds	si, dst
				mov	cx, width
				sub	cx, 1
				jz		lastPixel
		loop1:
				// Poll the pixel from the I22.
				_emit	0x66
				mov	ax, es:[bx]   // mov eax,es:[bx]
				_emit	0x66
				or		ax, ax   // or eax,eax
				js		loop1

				// Store the pixel.
				_emit	0x67
				_emit	0x66
				_emit	0x89
				_emit	0x06   // mov [esi],eax
				_emit	0x66
				add	si, 3   // add esi,3
				sub	cx, 1
				jnz	loop1

		lastPixel:
				// The last pixel must be written as 3 bytes, not as four (otherwise
				// selector protection can cause GPF).
				// Poll the pixel from the I22.
				_emit	0x66
				mov	ax, es:[bx]   // mov eax,es:[bx]
				_emit	0x66
				or		ax, ax   // or eax,eax
				js		lastPixel

				// Store the pixel.
				_emit	0x67
				_emit	0x89
				_emit	0x06   // mov [esi],ax
				_emit	0x66
				shr	ax, 16   // shr eax,16
				_emit	0x67
				_emit	0x88
				_emit	0x46
				_emit	0x02   // mov [esi+2],al

				_emit	0x66
				pop	si   // pop esi
				pop	ds
				}
#endif
		}


	static void ReadStillLineNonPolled (FPTR stillReg, FPTR dst, WORD width)
		{
#if NT_KERNEL || VXD_VERSION
		__asm {
				push	esi

				mov	ebx, stillReg
				mov	esi, dst
				mov	cx, width
				sub	cx, 1
				jz		lastPixel
		loop1:
				// Read a pixel from the I22.
				mov	eax, [ebx]
				mov	[esi], eax
				add	esi, 3
				sub	cx, 1
				jnz	loop1

		lastPixel:
				// The last pixel must be written as 3 bytes, not as four (otherwise
				// selector protection can cause GPF).
				// Read the last pixel from the I22.
				mov	eax, [ebx]
				mov	[esi], ax
				shr	eax, 16
				mov	[esi+2], al

				pop	esi
				}
#else
		__asm {
				push	ds
				_emit	0x66
				push	si   // push esi

				les	bx, stillReg
				_emit	0x66
				xor	si, si   // xor esi,esi
				lds	si, dst
				mov	cx, width
				sub	cx, 1
				jz		lastPixel
		loop1:
				// Read a pixel from the I22.
				_emit	0x66
				mov	ax, es:[bx]   // mov eax,es:[bx]
				_emit	0x67
				_emit	0x66
				_emit	0x89
				_emit	0x06   // mov [esi],eax
				_emit	0x66
				add	si, 3   // add esi,3
				sub	cx, 1
				jnz	loop1

		lastPixel:
				// The last pixel must be written as 3 bytes, not as four (otherwise
				// selector protection can cause GPF).
				// Read the last pixel from the I22.
				_emit	0x66
				mov	ax, es:[bx]   // mov eax,es:[bx]
				_emit	0x67
				_emit	0x89
				_emit	0x06   // mov [esi],ax
				_emit	0x66
				shr	ax, 16   // shr eax,16
				_emit	0x67
				_emit	0x88
				_emit	0x46
				_emit	0x02   // mov [esi+2],al

				_emit	0x66
				pop	si   // pop esi
				pop	ds
				}
#endif
		}



Error JPEGManager022::ReadStillPixels24 (FPTR dst, WORD width, WORD height,
                                         DWORD stride, VideoField fieldOrder)
	{
	VirtualJPEGUnit022 *jpeg = GetCurrent ();
	DWORD reg, old;
	BYTE __huge *data;
	WORD fields, field, lines, line, y;
	volatile DWORD __far * stillReg;


GDP("Read still pixels");
	// Configure WaitState parameter to 6.
	io->ReadDWord (I20REG_SYSCTRL, old);
	reg = WRBF (old, I20IDX_WAITSTATE, 6);
	io->WriteDWord (I20REG_SYSCTRL, reg);

	// Note: We are working with little endian here, thus assuming fixed
	// organization of the still transfer register.

	// Note that bitmaps have the lines reversed.
	if (fieldOrder == VFLD_SINGLE)
		{
		fields = 1;
		lines = height;
		data = (BYTE __huge *)dst + (stride * (lines-1));
		}
	else // two fields
		{
		fields = 2;
		lines = height / 2;
		stride *= 2L;
		data = (BYTE __huge *)dst + stride * lines - 3L * width;
		}

	io->GetDWordPtr (I22REG_STILL_TRANSFER, stillReg);

	// Start.
	io->SetBit (I22REG_P_RESET);   // release JPEG process reset
	z016->Start ();
	io->SetBit (I22REG_COD_TRNS_EN);
	io->SetBit (I22REG_ACTIVE);
#if DO_TIMING
	DWORD start = Timer.GetMilliTicks ();
#endif


	// Do each field.
	for (field = 0;  field < fields;  field++)
		{
		for (line = 0;  line < lines;  line += 8)
			{
			// The first line is polled completely.
			ReadStillLinePolled ((FPTR)stillReg, data, width);
			data -= stride;
			if (line == 0)
				io->ClearBit (I22REG_GO_ENABLE);   // 016 bug fix
			// The next seven are not polled.
			for (y = 7;  y > 0;  y--)
				{
				ReadStillLineNonPolled ((FPTR)stillReg, data, width);
				data -= stride;
				}
			}
		// Prepare data pointer to next field.
		data = (BYTE __huge *)dst + (stride * (lines-1));
		io->SetBit (I22REG_GO_ENABLE);
		}
#if DO_TIMING
	GDP("expansion time %ld", Timer.GetMilliTicks() - start);
#endif

	Stop ();

	io->WriteDWord (I20REG_SYSCTRL, old);
	GNRAISE_OK;
	}



#endif // VIONA_VERSION










//************************************************************************
// The JPEG unit class.
//************************************************************************



// Constructor.

VirtualJPEGUnit022::VirtualJPEGUnit022 (JPEGManager022 *physical, VirtualJCoder *coder,
                                        VirtualI22PIP *pip)
	: VirtualJPEGUnit (physical, coder)
	{
	FPTR fptr;
	DWORD dummySize;


	manager = physical;
	this->pip = pip;

	statComTable.AllocateBuffer (N_FRAGTABLES * sizeof(DWORD));
	statComTable.GetBuffer (fptr, dummySize);
	statCom = (DWORD __far *)fptr;
	statComTable.GetBufferFragment (0, statComPhysical, dummySize);

	dummyCaptureData = NULL;
	dummyCaptureInfo = NULL;

	playBackupData = NULL;
	playBackupInfo = NULL;

	running = FALSE;
	playHook = NULL;
	nextBuffer = 0;
	lastBuffer = 0;
	nBuffers   = 0;

	preemptHook = NULL;
	stillExpansion = FALSE;
	singleFrame    = FALSE;
	syncMaster     = FALSE;
	horDecimation  = FALSE;
	jpgMode        = I22_MOTION_COMPRESS;

	// Set defaults.
	vStandard  = VSTD_NTSC;
	vFormat    = VFMT_YUV_422;
	fieldOrder = VFLD_SINGLE;
	thumbnail  = FALSE;
	filtersOn  = TRUE;
	}



// Destructor.

VirtualJPEGUnit022::~VirtualJPEGUnit022 (void)
	{
	FlushPlayback (0);
	delete pip;
	pip = NULL;
	statComTable.FreeBuffer ();
	statComPhysical = NULL;
	statCom = NULL;
	}





// The function responsible for the tag processing.

Error VirtualJPEGUnit022::Configure (TAG __far *tags)
	{
	GNREASSERT( VirtualJPEGUnit::Configure (tags) );
	GNREASSERT( coder->Configure (tags) );

	{
	PARSE_TAGS_START(tags)
		GETSETC (JPG_VIDEO_STANDARD, vStandard);
	PARSE_TAGS_END
	}

	// Calculate the video offsets according to the current operation mode.
	CalcVideoParameters ();

	PARSE_TAGS_START(tags)
		GETSETC (JPG_VIDEO_FORMAT,   vFormat);
		GETSETC (JPG_FIELD_ORDER,    fieldOrder);
		GETSETC (JPG_THUMBNAIL,      thumbnail);
		GETSETC (JPG_MSEC_PER_FRAME, milliSecPerFrame);
		GETSETC (JPG_SINGLEFRAME,    singleFrame);
		GETSETC (JPG_TOTALCODEVOL,   totalCodeVolume);
		GETSETC (JPG_FILTERS_ON,     filtersOn);
		GETSET  (JPG_PREEMPT_HOOK,   preemptHook);
		GETONLY (JPG_HAS_CODEMEMORY, FALSE);
		GETONLY (JPG_HOFFSET,        hOffset);
		GETONLY (JPG_VOFFSET,        vOffset);
		GETONLY (JPG_WANTS_EXT_FI,   ! syncMaster);
		GETONLY (JPG_ALLOWED_VSTD,   manager->allowedStandard);
		GETONLY (JPG_MAX_FIELDSIZE,  MAX_FIELDSIZE);
		GETONLY (JPG_VBUS_WIDTH,     opWidth);
		GETONLY (JPG_VBUS_HEIGHT,    2*opHeight);
		GETINQUIRE (JPG_VIDEO_FMT_LIST, data &= VideoFormatBitField;);
	PARSE_TAGS_END

	GNRAISE_OK;
	}





Error VirtualJPEGUnit022::PrepareBuffer (FPTR buffer, DWORD size, FPTR __far &info)
	{
	PrepareInfo *prep = new PrepareInfo;
	if (prep == NULL)
		{
		GDP("PrepareBuffer: not enough memory");
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
	GNREASSERT( prep->PrepareMemory (buffer, size) );
	info = prep;
	GNRAISE_OK;
	}





Error VirtualJPEGUnit022::UnprepareBuffer (FPTR info)
	{
	PrepareInfo *prep = (PrepareInfo *)info;
	if (prep == NULL)
		{
		GDP("UnprepareBuffer: NULL ptr");
		GNRAISE(GNR_MEM_NOT_ALLOCATED);
		}
	delete prep;
	GNRAISE_OK;
	}





Error VirtualJPEGUnit022::AllocateVideoBuffer (DWORD size, FPTR __far &info, FPTR __far &data)
	{
	size = (size + 3) & ~3;

	PrepareInfo *prep = new PrepareInfo;
	if (prep == NULL)
		{
		GDP("AllocateVideoBuffer: not enough memory");
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
	GNREASSERT( prep->AllocMemory (size, data) );
	info = prep;
	GNRAISE_OK;
	}





Error VirtualJPEGUnit022::FreeVideoBuffer (FPTR info)
	{
	PrepareInfo *prep = (PrepareInfo *)info;
	if (prep == NULL)
		{
		GDP("FreeVideoBuffer: NULL ptr");
		GNRAISE(GNR_MEM_NOT_ALLOCATED);
		}
	delete prep;
	GNRAISE_OK;
	}





#if VIONA_VERSION


// Is a scaling valid ?  We need srcWidth <= 768 (720 for CCIR) and an even number of MCUs
// (i.e. srcWidth must be multiple of 16). We also cannot scale up.

BOOL VirtualJPEGUnit022::IsValidExpansionScaling (WORD srcWidth, WORD srcHeight,
                                                  WORD dstWidth, WORD dstHeight,
                                                  GrabFormat bitFormat)
	{
	// We support 32, 24 and 16 bit formats.
	if (bitFormat != VGRB_RGB_888x  &&  bitFormat != VGRB_RGB_888  &&  bitFormat != VGRB_RGB_565
	    &&  bitFormat != VGRB_YUV_422)
		return FALSE;

	// No scaling is valid.
	if (dstWidth == srcWidth  &&  dstHeight == srcHeight)
		return TRUE;

	// Check restricted scaling capability.
	if (srcWidth > 768  ||  (manager->ccirMode  &&  srcWidth > 720)  ||  (srcWidth & 0x0f) != 0)
		return FALSE;
	if (dstWidth > srcWidth  ||  dstHeight > srcHeight)
		return FALSE;
	return TRUE;
	}



Error VirtualJPEGUnit022::ExpandToRGB (FPTR src, DWORD size, FPTR dst, WORD dstWidth, WORD dstHeight, GrabFormat bitFormat)
	{
	FragmentTableClass *frag = new FragmentTableClass;
	DWORD physical, dummySize, oldSize;
	Error err;
	int i;
	int retries;
	WORD jpegWidth, jpegHeight;
#if EXPAND_ONE_FRAGMENT
	PoolContDMABuffer *codeBuffer;
	FPTR fptr;
#endif


#if DO_TIMING
	DWORD start = Timer.GetMilliTicks ();
#endif


	// Check unit state.
	if (GetState() != current)
		GNRAISE(GNR_OBJECT_NOT_CURRENT);
	if (compress  ||  ! stillMode)
		GNRAISE(GNR_INVALID_CONFIGURE_STATE);

	jpegWidth  = width;
	jpegHeight = height;
#if 1
	// Cope with irregular sizes.
	width = (width + 15) & ~15;
	i = (fieldOrder == VFLD_SINGLE ? 7 : 15);
	height = (height + i) & ~i;
#else
	// Width must be a multiple of 16 and height a multiple of 8.
	if (width & 0x0f  ||  height & 0x07)
		GNRAISE(GNR_RANGE_VIOLATION);
#endif

	oldSize = size;

	stillExpansion = TRUE;
	size = (size + 3) & ~3;   // round up to DWORD boundary

	// Allocate JPEG source buffer.
#if EXPAND_ONE_FRAGMENT
	// If the I22 FIFO is completely full and a new fragment is started, the I22
	// chokes on the code. Fix this bug by either lowering the CodeThreshold for
	// expansion such that a burst filling the FIFO completely is highly unlikely
	// or by using only one code fragment. We choose the latter here.
	codeBuffer = new PoolContDMABuffer;
	if (codeBuffer == NULL)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	err = codeBuffer->AllocateBuffer (size);
	if (IS_ERROR(err))
		{
		delete codeBuffer;
		GNRAISE(err);
		}
	err = codeBuffer->GetBuffer (fptr, dummySize);
	if (IS_ERROR(err))
		{
		codeBuffer->FreeBuffer ();
		delete codeBuffer;
		GNRAISE(err);
		}
#if NT_KERNEL || VXD_VERSION
	memcpy (fptr, src, oldSize);
#else
	hmemcpy (fptr, src, oldSize);
#endif
	src = fptr;
#else
	Check if CreateFragmentBuffer works with byte-precise source segment.
#endif

	// For expanding a single frame, we only need one fragment table.
	if (frag == NULL)
		err = GNR_NOT_ENOUGH_MEMORY;
	else
		{
		err = frag->CreateFragmentBuffer (src, size);
		if (! IS_ERROR(err))
			{
			// Get physical address of the fragment and write it to the STATCOM table.
			if (! frag->GetBufferFragment (0, physical, dummySize))
				err = GNR_OBJECT_NOT_FOUND;   // will never occur
			else
				{
				// Try several times until timeout.
				err = GNR_JPEG_TIMEOUT;
				for (retries = 5;  retries > 0 && (err == GNR_JPEG_TIMEOUT || err == GNR_GRAB_TIMEOUT);  retries--)
					{
					statCom[0] = physical;
					// The other fragment entries are declared "status" so only one
					// fragment is expanded.
					for (i = 1;  i < N_FRAGTABLES;  i++)
						statCom[i] = 0x01;

#if DO_THUMBNAIL
					thumbnail = TRUE;
//					width /= 8;
					height /= 8;

					// Initialize the operation.
					changed = TRUE;   // reprogram chip parameters
					err = manager->InitOperation (statComPhysical);
					if (! IS_ERROR(err))
						{
						err = manager->ReadStillPixels24 (dst, width/8, height, 3L*dstWidth, fieldOrder);
						}
#else
					// Initialize the operation.
					changed = TRUE;   // reprogram chip parameters
					err = manager->InitOperation (statComPhysical);
					if (! IS_ERROR(err))
						{
#if ! PIP_EXPAND
						err = manager->ReadStillPixels24 (dst, dstWidth, dstHeight, 3L*dstWidth, fieldOrder);
#else
						err = pip->ConfigureTags (SET_PIP_VIDEOSTANDARD(vStandard),
						                          SET_PIP_SAMPLEMODE(VSAMOD_SQP),
						                          SET_PIP_HOFFSET(hOffset),
						                          SET_PIP_VOFFSET(vOffset),
						                          SET_PIP_EXT_FIELD_IND(! syncMaster),
						                          SET_PIP_TOP_FIELD(HIGH),
						                          SET_PIP_SOURCE_LEFT(0),
						                          SET_PIP_SOURCE_TOP(0),
						                          SET_PIP_SOURCE_WIDTH(opWidth),
						                          SET_PIP_SOURCE_HEIGHT(jpegHeight),
						                          SET_PIP_DEST_LEFT(0),
						                          SET_PIP_DEST_TOP(0),
						                          SET_PIP_DEST_WIDTH(width),
						                          SET_PIP_DEST_HEIGHT(height),
						                          SET_PIP_ADAPT_CLIENT_SIZE(FALSE),
						                          SET_PIP_ADAPT_SOURCE_SIZE(FALSE),
						                          TAGDONE);
						if (! IS_ERROR(err))
							{
							err = pip->Activate ();
							if (! IS_ERROR(err))
								{
								if (opWidth != width)
									jpegWidth *= 2;
								if (opHeight == height)
									jpegHeight *= 2;
								Start ();
								err = pip->GrabExpandFrame (dst, jpegWidth, jpegHeight, dstWidth, dstHeight, 0, bitFormat);
//								err = pip->GrabExpandFrame (dst, opWidth, 2 * opHeight, dstWidth, dstHeight, 0, bitFormat);
								Stop ();
								}
							}
#endif
						}
#endif // DO_THUMBNAIL
					}
				}
			frag->DeleteFragmentBuffer ();
			}
		}

	pip->Passivate ();
	delete frag;
#if EXPAND_ONE_FRAGMENT
	codeBuffer->FreeBuffer ();
	delete codeBuffer;
#endif

	if (IS_ERROR(err))
		{
		GDP("expand failed %lx", err);
		}
	stillExpansion = FALSE;
#if DO_TIMING
	GDP("total expansion time %ld", Timer.GetMilliTicks()-start);
#endif
	return err;
	}



#else // not VIONA_VERSION



// Expand a compressed frame into packed RGB data.

Error VirtualJPEGUnit022::ExpandToRGB (FPTR src, DWORD size, FPTR dst)
	{
	FragmentTableClass *frag = new FragmentTableClass;
	DWORD physical, dummySize, oldSize;
	Error err;
	int i;
	int retries;
#if EXPAND_ONE_FRAGMENT
	ContinuousDMABuffer *codeBuffer;
	FPTR fptr;
#else
	HGLOBAL handle;
	char __far *ptr;
#endif


#if DO_TIMING
	DWORD start = Timer.GetMilliTicks ();
#endif


_asm int 3
	// Check unit state.
	if (GetState() != current)
		GNRAISE(GNR_OBJECT_NOT_CURRENT);
	if (compress  ||  ! stillMode)
		GNRAISE(GNR_INVALID_CONFIGURE_STATE);

	// Width must be a multiple of 16 and height a multiple of 8.
	if (width & 0x0f  ||  height & 0x07)
		GNRAISE(GNR_RANGE_VIOLATION);

	oldSize = size;

	stillExpansion = TRUE;
	size = (size + 3) & ~3;   // round up to DWORD boundary

	// Allocate JPEG source buffer.
#if EXPAND_ONE_FRAGMENT
	// If the I22 FIFO is completely full and a new fragment is started, the I22
	// chokes on the code. Fix this bug by either lowering the CodeThreshold for
	// expansion such that a burst filling the FIFO completely is highly unlikely
	// or by using only one code fragment. We choose the latter here.
	codeBuffer = new ContinuousDMABuffer;
	if (codeBuffer == NULL)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	err = codeBuffer->AllocateBuffer (size);
	if (IS_ERROR(err))
		{
		delete codeBuffer;
		GNRAISE(err);
		}
	err = codeBuffer->GetBuffer (fptr, dummySize);
	if (IS_ERROR(err))
		{
		codeBuffer->FreeBuffer ();
		delete codeBuffer;
		GNRAISE(err);
		}
	hmemcpy (fptr, src, oldSize);
	src = fptr;
#else
	handle = GlobalAlloc (GMEM_FIXED, size);
	if (handle == NULL)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	if (0 == GlobalPageLock (handle))
		{
		GlobalFree (handle);
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
	ptr = GlobalLock (handle);
	if (ptr == NULL)
		{
		GlobalPageUnlock (handle);
		GlobalFree (handle);
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
	hmemcpy (ptr, src, oldSize);
	src = ptr;
#endif

	// For expanding a single frame, we only need one fragment table.
	if (frag == NULL)
		err = GNR_NOT_ENOUGH_MEMORY;
	else
		{
		err = frag->CreateFragmentBuffer (src, size);
		if (! IS_ERROR(err))
			{
			// Get physical address of the fragment and write it to the STATCOM table.
			if (! frag->GetBufferFragment (0, physical, dummySize))
				err = GNR_OBJECT_NOT_FOUND;   // will never occur
			else
				{
				// Try several times until timeout.
				err = GNR_JPEG_TIMEOUT;
				for (retries = 5;  retries > 0 && (err == GNR_JPEG_TIMEOUT || err == GNR_GRAB_TIMEOUT);  retries--)
					{
					statCom[0] = physical;
					// The other fragment entries are declared "status" so only one
					// fragment is expanded.
					for (i = 1;  i < N_FRAGTABLES;  i++)
						statCom[i] = 0x01;

					// Initialize the operation.
					changed = TRUE;   // reprogram chip parameters
					err = manager->InitOperation (statComPhysical);
					if (! IS_ERROR(err))
						{
						err = pip->ConfigureTags (SET_PIP_VIDEOSTANDARD(vStandard),
						                          SET_PIP_SAMPLEMODE(VSAMOD_SQP),
						                          SET_PIP_HOFFSET(hOffset),
						                          SET_PIP_VOFFSET(vOffset),
						                          SET_PIP_EXT_FIELD_IND(! syncMaster),
						                          SET_PIP_TOP_FIELD(HIGH),
						                          SET_PIP_SOURCE_LEFT(0),
						                          SET_PIP_SOURCE_TOP(0),
						                          SET_PIP_SOURCE_WIDTH(opWidth),
						                          SET_PIP_SOURCE_HEIGHT(height),
						                          SET_PIP_DEST_LEFT(0),
						                          SET_PIP_DEST_TOP(0),
						                          SET_PIP_DEST_WIDTH(width),
						                          SET_PIP_DEST_HEIGHT(height),
						                          SET_PIP_ADAPT_CLIENT_SIZE(FALSE),
						                          SET_PIP_ADAPT_SOURCE_SIZE(FALSE),
						                          TAGDONE);
						if (! IS_ERROR(err))
							{
							err = pip->Activate ();
							if (! IS_ERROR(err))
								{
								Start ();
								err = pip->GrabExpandFrame (dst, opWidth, 2 * opHeight, width, height, 0, VGRB_RGB_888);
								Stop ();
								}
							}
						}
					}
				}
			frag->DeleteFragmentBuffer ();
			}
		}

	pip->Passivate ();
	delete frag;
#if EXPAND_ONE_FRAGMENT
	codeBuffer->FreeBuffer ();
	delete codeBuffer;
#else
	GlobalUnlock (handle);
	GlobalPageUnlock (handle);
	GlobalFree (handle);
#endif

	if (IS_ERROR(err))
		{
		GDP("expand failed %lx", err);
		}
	stillExpansion = FALSE;
#if DO_TIMING
	GDP("total expansion time %ld", Timer.GetMilliTicks()-start);
#endif
	return err;
	}



#endif // VIONA_VERSION





// Compress RGB data into a frame.

#if VIONA_VERSION

BOOL VirtualJPEGUnit022::IsValidCompression (WORD width, WORD height, BOOL bits32)
	{
	if (height > 288  &&  (height & 1))
		return FALSE;
	return (width >= 16  &&  height >= 8);
	}

Error VirtualJPEGUnit022::CompressFromRGB (FPTR src, FPTR dst, DWORD maxSize, DWORD __far &size, BOOL bits32)
#else
Error VirtualJPEGUnit022::CompressFromRGB (FPTR src, FPTR dst, DWORD maxSize, DWORD __far &size)
#endif
	{
	FragmentTableClass *frag;
	DWORD physical, dummySize;
	Error err;
	int i;
	int retries;
#if !NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
	HGLOBAL handle;
#endif
	char __far *ptr;
#if DO_TIMING
	DWORD start = Timer.GetMilliTicks ();
#endif


	// Check unit state.
	if (GetState() != current)
		GNRAISE(GNR_OBJECT_NOT_CURRENT);
	if (! compress  ||  ! stillMode)
		GNRAISE(GNR_INVALID_CONFIGURE_STATE);

#if VIONA_VERSION
	// Size must be valid. For two fields, height must be even.
	if (! IsValidCompression (width, height, bits32))
		GNRAISE(GNR_RANGE_VIOLATION);
	if (fieldOrder != VFLD_SINGLE  &&  (height & 1))
		GNRAISE(GNR_RANGE_VIOLATION);

	// Cope with irregular sizes.
	WORD bitmapWidth  = width;
	WORD bitmapHeight = height;
	width = (width + 15) & ~15;
	i = (fieldOrder == VFLD_SINGLE ? 7 : 15);
	height = (height + i) & ~i;

	// We could directly create the fragment table from "dst", but timing shows
	// that this is a bit slower than using a temporary buffer (about 204/186).
#else
	// Width must be a multiple of 16 and height a multiple of 8.
	if (width & 0x0f  ||  height & 0x07)
		GNRAISE(GNR_RANGE_VIOLATION);
#endif
	// Allocate JPEG code target buffer.
#if NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
#if WIN32
	ptr = new __huge (PagedPool) char[maxSize];
#else
	ptr = (char __far *) _halloc (maxSize, sizeof(char));   // MSVC bug in "new __huge"
#endif
	if (ptr == NULL)
		{
		GDP("temp buffer not allocated");
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
#else
	handle = GlobalAlloc (GMEM_FIXED, maxSize);
	if (handle == NULL)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	if (0 == GlobalPageLock (handle))
		{
		GlobalFree (handle);
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
	ptr = GlobalLock (handle);
	if (ptr == NULL)
		{
		GlobalPageUnlock (handle);
		GlobalFree (handle);
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
#endif

	// For compressing a single frame, we only need one fragment table.
	frag = new FragmentTableClass;
	err = frag->CreateFragmentBuffer (ptr, maxSize);
	if (! IS_ERROR(err))
		{
		// Get physical address of the fragment and write it to the STATCOM table.
		if (! frag->GetBufferFragment (0, physical, dummySize))
			err = GNR_OBJECT_NOT_FOUND;   // will never occur
		else
			{
			// Try several times until timeout.
			err = GNR_JPEG_TIMEOUT;
			for (retries = 5;  retries > 0 && err == GNR_JPEG_TIMEOUT;  retries--)
				{
				statCom[0] = physical;
				// The other fragment entries are declared "status" so only one
				// fragment is compressed.
				for (i = 1;  i < N_FRAGTABLES;  i++)
					statCom[i] = 0x01;

				// Initialize the operation.
#if VIONA_VERSION
				err = manager->InitOperation (statComPhysical);
				if (! IS_ERROR(err))
					{
					if (bits32)
						err = manager->WriteStillPixels32 (src, bitmapWidth, bitmapHeight, fieldOrder);
					else
						err = manager->WriteStillPixels24 (src, bitmapWidth, bitmapHeight, fieldOrder);
					if (! IS_ERROR(err))
						{
						size = XTBF (1, 22, statCom[0]);
#if NT_KERNEL || VXD_VERSION
						memcpy (dst, ptr, size);
#else
						hmemcpy (dst, ptr, size);
#endif

						// Mark the first field correctly.
						coder->MarkFirstField (dst, size, (fieldOrder == VFLD_SINGLE ? VFLD_SINGLE : VFLD_ODD));
						}
					}
#else
				err = manager->InitOperation (statComPhysical);
				if (! IS_ERROR(err))
					{
					err = manager->WriteStillPixels24 (src, width, height, fieldOrder);
					if (! IS_ERROR(err))
						{
						size = XTBF (1, 22, statCom[0]);
						hmemcpy (dst, ptr, size);
						}
					}
#endif
				}
			}
		frag->DeleteFragmentBuffer ();
		}
	delete frag;
#if NO_MORE_GLOBAL_ALLOC_IN_LOW_LEVEL_DRIVERS
#if WIN32
	delete[] ptr;
#else
	_hfree (ptr);
#endif
#else
	GlobalUnlock (handle);
	GlobalPageUnlock (handle);
	GlobalFree (handle);
#endif

#if DO_TIMING
	GDP("total compression time %ld", Timer.GetMilliTicks()-start);
#endif
	return err;
	}





Error VirtualJPEGUnit022::InitCapture (JPEGCaptureHook __far *hook)
	{
	int i;
	Error err;


	// Mark all fragments as "status".
	for (i = 0;  i < N_FRAGTABLES;  i++)
		statCom[i] = 0x01;
	for (i = 0;  i < N_DLL_BUFFERS;  i++)
		{
		buffers[i].info     = NULL;
		buffers[i].userData = NULL;
		}
	nextBuffer = 0;
	lastBuffer = 0;
	nBuffers   = 0;
	nStatBuffers = 0;
	frameNum   = 0;
	lastFieldCnt = 255;

	nextFrameTime = 0;
	stoppingCapture = 0;
	playAddAllowed = TRUE;

	// Check unit state.
	if (GetState() != current)
		GNRAISE(GNR_OBJECT_NOT_CURRENT);
	if (! compress  ||  stillMode)
		GNRAISE(GNR_INVALID_CONFIGURE_STATE);

	// Width must be a multiple of 16 and height a multiple of 8.
	if (width & 0x0f  ||  height & 0x07)
		{
#if NT_DEBUGOUTPUT
		DbgPrint ("InitCapture size %ld,%ld\n", (long)width, (long)height);
#endif
		GNRAISE(GNR_RANGE_VIOLATION);
		}

	if (fieldOrder == VFLD_SINGLE)
		{
		dummyCapSize = 2L * totalCodeVolume;
		GDP("InitCap: allocating %ld", dummyCapSize);
		GNREASSERT( AllocateVideoBuffer (dummyCapSize, dummyCaptureInfo, dummyCaptureData) );
		}

	// Initialize the operation.
	err = manager->InitOperation (statComPhysical);
	if (! IS_ERROR(err))
		{
		capHook = hook;
#if NT_DEBUGOUTPUT
		DbgPrint ("InitCapture OK\n");
#endif
		err = manager->StartIRQs ();
		if (! IS_ERROR(err))
			{
			GNRAISE_OK;
			}		
		}
	if (dummyCaptureInfo != NULL)
		{
		FreeVideoBuffer (dummyCaptureInfo);
		dummyCaptureInfo = NULL;
		}
#if NT_DEBUGOUTPUT
	DbgPrint ("InitCapture error %08lx\n", err);
#endif
	GNRAISE(err);
	}





Error VirtualJPEGUnit022::FinishCapture (void)
	{
	Error err;


#if NT_KERNEL
	irqMutex.Enter();
	playAddAllowed = FALSE;
	capHook = NULL;
	irqMutex.Leave();
#else
	playAddAllowed = FALSE;
	capHook = NULL;
#endif
	err = manager->StopIRQs ();

	if (dummyCaptureInfo != NULL)
		{
		FreeVideoBuffer (dummyCaptureInfo);
		dummyCaptureInfo = NULL;
		}

	GNRAISE(err);
	}





void VirtualJPEGUnit022::JPEGCodeInterrupt (void)
	{
	DWORD status;
	WORD fieldCnt;
	JPEGCaptureMsg msg;
	JPEGPlaybackMsg pmsg;


#if NT_KERNEL
	irqMutex.EnterAtIRQLevel();
#endif


	if (compress)
		{
		PrepareInfo *prep;
		DWORD nextFrameNum, timeStream;

		// Process all filled capture buffers.
		while (nBuffers > 0  &&  ((status = statCom[nextBuffer]) & 0x01))
			{
			nBuffers--;
			msg.info     = buffers[nextBuffer].info;
			msg.userData = buffers[nextBuffer].userData;
			msg.byteSize = XTBF(1, 22, status);

			// If there is no further entry in the STATCOM table, this buffer will
			// be the last I22 interrupt processed. If the high level driver does
			// not supply another buffer, we must repeat this one to have a buffer
			// in the low level DLL that triggers interrupts. If this is not done,
			// capture process can stop completely under conditions where the
			// capture application cannot supply unfilled buffers fast enough.
			// Note that for successful single frame capture, this is acceptable.
			if (nBuffers == 0  &&  ! singleFrame)
				{
				// We MUST fill this buffer again.
//				GDP("MUST refill buffer");
				buffers[nextBuffer].info = NULL;   // buffer available for adding another buffer
				InternalAddVideoBuffer (msg.info, msg.userData, buffers[nextBuffer].byteSize);
				msg.info = NULL;
				msg.userData = NULL;
				msg.byteSize = 0;
				}
			else
				{
				// We can step to the next buffer.
				fieldCnt = (WORD)(XTBF(24, 8, status));
				if (lastFieldCnt >= fieldCnt)
					frameNum += (256 + fieldCnt) - lastFieldCnt;
				else
					frameNum += fieldCnt - lastFieldCnt;
//GDP("stat %ld:  %ld %ld %ld %ld", (long)fieldCnt, statCom[0] & 1, statCom[1] & 1, statCom[2] & 1, statCom[3] & 1);
#if NT_DEBUGOUTPUT
	if (frameNum <= 2  ||  frameNum == 20  ||  frameNum == 60  ||  frameNum == 100)
		DbgPrint ("irq22\n");
#endif
GDP("Cap %ld, size %ld, cnt %ld", frameNum, (long)msg.byteSize, (long)fieldCnt);
				lastFieldCnt = fieldCnt;
				msg.frameNum = frameNum;

				if (! singleFrame  &&  fieldOrder == VFLD_SINGLE)
					{
#if DUMMY_CAPBUFFER
					if (buffers[nextBuffer].userData == NULL)
						{
						// Drop second field in single field mode.
//						GDP("Dropping second field");
//						if ((fieldCnt & 1) != 1)
//							GDP("got even field cnt");
						buffers[nextBuffer].info = NULL;   // buffer available for adding another buffer
						msg.byteSize = 0;
						}
#else
					if ((fieldCnt & 1) == 1)
						{
						// Drop second field in single field mode.
//						GDP("Dropping second field");
						buffers[nextBuffer].info = NULL;   // buffer available for adding another buffer
						msg.byteSize = 0;
						}
#endif
					msg.frameNum >>= 1;
					}

				// Drop the very first two frames of a sequence to let the bit rate control
				// process stabilize.
				if (msg.byteSize > 0  &&  (msg.frameNum >= STARTCAP_DROPFRAMES  ||  singleFrame))
					{
					// Mark the first field correctly.
					VideoField order;
					switch (fieldOrder)
						{
						case VFLD_SINGLE:  order = VFLD_SINGLE;  break;
						case VFLD_ODD:     order = VFLD_EVEN;    break;
						case VFLD_EVEN:    order = VFLD_ODD;     break;
						}
					prep = (PrepareInfo *)buffers[nextBuffer].info;
					coder->MarkFirstField (prep->KernelSpace(), msg.byteSize, order);

					buffers[nextBuffer].info = NULL;   // buffer available for adding another buffer

					if (singleFrame)
						nextFrameNum = msg.frameNum;
					else
						nextFrameNum = msg.frameNum - STARTCAP_DROPFRAMES;

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
						msg.captureTime = timeStream;
						}
					else
						{
						GDP("not yet time\n");
						msg.byteSize = 0;
						}
					}
				else
					msg.byteSize = 0;
				}
			// Do a callback to give the high level driver opportunity to pass down buffers.
#if DUMMY_CAPBUFFER
			if (fieldOrder != VFLD_SINGLE  ||  buffers[nextBuffer].userData != NULL)
#endif
				{
				if (capHook)
					capHook.Call (msg);
				else
					{
					GDP("No capHook");
					}
				if (stoppingCapture == 1)
					stoppingCapture++;
				}
			nextBuffer = (nextBuffer+1) & (N_DLL_BUFFERS-1);
			}
		}


	if (! compress)
		{
		InPlayInterrupt = TRUE;
		// Playback interrupt.
//GDP("stat:  %ld %ld %ld %ld", statCom[0] & 1, statCom[1] & 1, statCom[2] & 1, statCom[3] & 1);
		BOOL called = FALSE;

		while (nBuffers > 0  &&  ((status = statCom[nextBuffer]) & 0x01))
			{
			nBuffers--;
//GDP("finish %ld, stat: %ld %ld %ld %ld", (long)nextBuffer, statCom[0] & 1, statCom[1] & 1, statCom[2] & 1, statCom[3] & 1);
			buffers[nextBuffer].info = NULL;   // buffer available for adding another buffer

			if (frameNum <= 0)
				{
				GDP("irq base time");
				nextFrameTime = Timer.GetMilliTicks ();   // set base start time
				}
			pmsg.nowTime = (fieldOrder == VFLD_SINGLE) ? frameNum/2 : frameNum;
			if (VSTD_NTSC == vStandard)
				// nextFrameNum * 780 * 525 * 1000 / 12 272 727.
				pmsg.nowTime = ScaleDWord (pmsg.nowTime, 4090909L, 136500000);
			else
				pmsg.nowTime = 40 * pmsg.nowTime;
if (pmsg.nowTime == 0)
	GDP("irq nowtime zero, nft %ld", nextFrameTime);
			pmsg.nowTime += nextFrameTime;
			pmsg.baseTime = nextFrameTime;

			if (playHook)
				{
				pmsg.userData = buffers[nextBuffer].userData;
//				pmsg.nowTime = Timer.GetMilliTicks ();
				called = TRUE;
				playHook.Call (pmsg);
				}

#if 1
			nextBuffer = (nextBuffer+1) & (N_DLL_BUFFERS-1);
#else
			if (buffers[nextBuffer].info != NULL)
				nextBuffer = (nextBuffer+1) & (N_DLL_BUFFERS-1);
			else
				{
				nBuffers++;
				GDP("Buffer skipped");
				frameNum++;
				break;
				}
#endif
			frameNum++;
			}
		if (! called  &&  playHook)
			{
			if (frameNum <= 0)
				{
				GDP("irq base time");
				nextFrameTime = Timer.GetMilliTicks ();   // set base start time
				}
			pmsg.nowTime = (fieldOrder == VFLD_SINGLE) ? frameNum/2 : frameNum;
			if (VSTD_NTSC == vStandard)
				// nextFrameNum * 780 * 525 * 1000 / 12 272 727.
				pmsg.nowTime = ScaleDWord (pmsg.nowTime, 4090909L, 136500000);
			else
				pmsg.nowTime = 40 * pmsg.nowTime;
if (pmsg.nowTime == 0)
	GDP("irq nowtime zero, nft %ld", nextFrameTime);
			pmsg.nowTime += nextFrameTime;
			pmsg.baseTime = nextFrameTime;

			GDP("extra call, nbufs %ld", (long)nBuffers);
			pmsg.userData = NULL;
//			pmsg.nowTime = Timer.GetMilliTicks ();
			playHook.Call (pmsg);
			frameNum++;
			}

/*if (frameNum > 10)
	{
	int k, m;
	for (m = 0, k = 0;  k < 4;  k++)
		{
		if (statCom[k] & 1)
			m++;
		}
	if (m > 2)
		{
		DP("stat:  %ld %ld %ld %ld\n", statCom[0] & 1, statCom[1] & 1, statCom[2] & 1, statCom[3] & 1);
		}
	if (nBuffers <= 2)
		{
		DP("am empty\n");
		}
	}
frameNum++;*/
		InPlayInterrupt = FALSE;
		}


#if NT_KERNEL
	irqMutex.LeaveAtIRQLevel();
#endif
	}





Error VirtualJPEGUnit022::AddVideoBuffer (FPTR info, FPTR userData, DWORD size)
	{
	Error err;

#if NT_KERNEL
	irqMutex.Enter();
	if (playAddAllowed)
		err = InternalAddVideoBuffer (info, userData, size);
	else
		err = GNR_JPEG_PIPE_FULL;
	irqMutex.Leave();
#else
	err = InternalAddVideoBuffer (info, userData, size);
#endif
	GNRAISE(err);
	}





Error VirtualJPEGUnit022::InternalAddVideoBuffer (FPTR info, FPTR userData, DWORD size)
	{
	PrepareInfo *prep = (PrepareInfo *)info;
	FragmentTableClass *frag;
	DWORD physical, dummySize;
	DWORD status;


#if NT_KERNEL
	if (info == NULL)
		{
		_asm int 3
		}
#endif
	frag = prep->frag;

	// Finish previous buffers if we're not called from interrupt.
#if NT_KERNEL
	if (! InPlayInterrupt  &&  ! compress  &&  ! manager->handlerEnabled)
#else
	if (! InPlayInterrupt  &&  ! compress)
#endif
		{
		while (nBuffers > 0  &&  ((status = statCom[nextBuffer]) & 0x01))
			{
			nBuffers--;
//GDP("finish b %ld, stat: %ld %ld %ld %ld", (long)nextBuffer, statCom[0] & 1, statCom[1] & 1, statCom[2] & 1, statCom[3] & 1);
			buffers[nextBuffer].info = NULL;   // buffer available for adding another buffer
			nextBuffer = (nextBuffer+1) & (N_DLL_BUFFERS-1);
			}
		}

	// Is the STATCOM table entry available ?
//	if (0 == (statCom[lastBuffer] & 0x01))
	if (nBuffers >= N_DLL_BUFFERS)
		GNRAISE(GNR_JPEG_PIPE_FULL);

#if PLAYBACK_ONE_FRAGMENT
	if (manager->fixPlaybackLongBursts  &&  ! compress  &&  ! stillMode)
		{
		FPTR fptr;
		DWORD dummy;
		if (size > PlaySize)
			GNRAISE(GNR_RANGE_VIOLATION);
		GNREASSERT( frag->GetBufferPointer (fptr, dummy) );
//		GDP("using one fragment");
#if NT_DEBUGOUTPUT
//		DbgPrint ("using one fragment\n");
#endif
#if NT_KERNEL || VXD_VERSION
		memcpy (PlayBufferPtr[lastBuffer], fptr, size);
#else
		hmemcpy (PlayBufferPtr[lastBuffer], fptr, size);
#endif
		frag = PlayBufferFrags[lastBuffer];
		info = frag;
		}
#endif

#if DUMMY_CAPBUFFER
	if (fieldOrder == VFLD_SINGLE  &&  compress)
		{
		if (1 == (lastBuffer & 1))
			{
			prep = (PrepareInfo *)dummyCaptureInfo;
			frag = prep->frag;
			if (! frag->GetBufferFragment (0, physical, dummySize))
				{
				GDP("AddVideoBuffer: single physical not obtained");
				GNRAISE(GNR_OBJECT_NOT_FOUND);   // will never occur
				}
//GDP("filling %ld", (long)lastBuffer);
			statCom[lastBuffer] = physical;   // note that bit zero (STAT_BIT) is cleared !

			buffers[lastBuffer].info     = info;
			buffers[lastBuffer].userData = NULL;
			buffers[lastBuffer].byteSize = dummyCapSize;
			lastBuffer = (lastBuffer+1) & (N_DLL_BUFFERS-1);
			nBuffers++;

			// Is the STATCOM table entry available ?
//			if (0 == (statCom[lastBuffer] & 0x01))
			if (nBuffers >= N_DLL_BUFFERS)
				GNRAISE(GNR_JPEG_PIPE_FULL);

			prep = (PrepareInfo *)info;
			frag = prep->frag;
			}
		}
#endif

	// Get physical address of the fragment and write it to the STATCOM table.
	if (! frag->GetBufferFragment (0, physical, dummySize))
		{
		GDP("AddVideoBuffer: physical not obtained");
		GNRAISE(GNR_OBJECT_NOT_FOUND);   // will never occur
		}
//GDP("filling %ld", (long)lastBuffer);
#if NT_DEBUGOUTPUT
//	DbgPrint ("filling %ld\n", (long)lastBuffer);
#endif

	statCom[lastBuffer] = physical;   // note that bit zero (STAT_BIT) is cleared !

	buffers[lastBuffer].info     = info;
	buffers[lastBuffer].userData = userData;
	buffers[lastBuffer].byteSize = size;
	lastBuffer = (lastBuffer+1) & (N_DLL_BUFFERS-1);
	nBuffers++;

	GNRAISE_OK;
	}





Error VirtualJPEGUnit022::InitPlayback (JPEGPlaybackHook __far *hook)
	{
	int i;
	Error err = GNR_OK;


	// Create stable object state.
	stillExpansion = FALSE;
	InPlayInterrupt = FALSE;
	frameNum = 0;
	nextFrameTime = 0;
	playAddAllowed = TRUE;

	if (changed)
		{
		// Mark all fragments as "status".
		for (i = 0;  i < N_FRAGTABLES;  i++)
			statCom[i] = 0x01;
		for (i = 0;  i < N_DLL_BUFFERS;  i++)
			{
			buffers[i].info     = NULL;
			buffers[i].userData = NULL;
			}
		nextBuffer = 0;
		lastBuffer = 0;
		nBuffers   = 0;
		lastFieldCnt = 255;
#if DEEP_PIPELINE
		statNextBuffer = 0;
		statLastBuffer = 0;
		nStatBuffers = 0;
#endif
		}

	// Check unit state.
	if (GetState() != current)
		GNRAISE(GNR_OBJECT_NOT_CURRENT);
	if (compress  ||  stillMode)
		GNRAISE(GNR_INVALID_CONFIGURE_STATE);

#if VIONA_VERSION
	// Cope with irregular sizes.
	width = (width + 15) & ~15;
	i = (fieldOrder == VFLD_SINGLE ? 7 : 15);
	height = (height + i) & ~i;
#else
	// Width must be a multiple of 16 and height a multiple of 8.
	if (width & 0x0f  ||  height & 0x07)
		GNRAISE(GNR_RANGE_VIOLATION);
#endif

	// Check size restriction.
	if (width > 768  ||  (manager->ccirMode  &&  width > 720))
		GNRAISE(GNR_INVALID_CONFIGURE_STATE);
GDP("playing size %ld,%ld", (long)width, (long)height);

#if PLAYBACK_ONE_FRAGMENT
	if (manager->fixPlaybackLongBursts)
		{
		// If the I22 FIFO is completely full and a new fragment is started, the I22
		// chokes on the code. Fix this bug by either lowering the CodeThreshold for
		// expansion such that a burst filling the FIFO completely is highly unlikely
		// or by using only one code fragment. We choose the latter here.
		DWORD dummy;
		if (PlayBuffers[0] == NULL)
			{
			dummy = PoolContDMABuffer::LargestBlocksize ();
			if (dummy > 0)
				PlaySize = dummy / N_DLL_BUFFERS;
			GDP("Allocating one-fragment-buffers size %ld", PlaySize);
#if NT_DEBUGOUTPUT
//			DbgPrint("Allocating one-fragment-buffers size %ld\n", PlaySize);
#endif
			for (int i = 0;  i < N_DLL_BUFFERS;  i++)
				{
				PlayBuffers[i] = new PoolContDMABuffer;
				if (PlayBuffers[i] == NULL)
					err = GNR_NOT_ENOUGH_MEMORY;
				else
					{
					err = PlayBuffers[i]->AllocateBuffer (PlaySize);
					if (! IS_ERROR(err))
						{
						PlayBufferFrags[i] = new FragmentTableClass;
						if (PlayBufferFrags[i] == NULL)
							err = GNR_NOT_ENOUGH_MEMORY;
						else
							{
							err = PlayBuffers[i]->GetBuffer (PlayBufferPtr[i], dummy);
							if (! IS_ERROR(err))
								err = PlayBufferFrags[i]->CreateFragmentBuffer (PlayBufferPtr[i], PlaySize);
							}
						}
					}
				if (IS_ERROR(err))
					{
					GDP("  FAILED");
#if NT_DEBUGOUTPUT
					DbgPrint("  FAILED\n");
#endif
					for (i = 0;  i < N_DLL_BUFFERS;  i++)
						{
						delete PlayBufferFrags[i];
						delete PlayBuffers[i];
						PlayBuffers[i] = NULL;
						PlayBufferFrags[i] = NULL;
						}
					GNRAISE(err);
					}
				}
			}
		}
#endif

	// Initialize the operation.
	GNREASSERT( manager->InitOperation (statComPhysical) );
	playHook = hook;
	GNRAISE_OK;
	}





Error VirtualJPEGUnit022::FinishPlayback (void)
	{
#if 1
	int buffer;
	DWORD timeout;

	if (playHook)
		{
		playAddAllowed = FALSE;
		manager->StopIRQs ();
		playHook = NULL;
		}
	if (manager->jpegCoreRunning  &&  running)
		{
		// Wait until the last passed buffer is playing.
		buffer = (lastBuffer+N_DLL_BUFFERS-2) & (N_DLL_BUFFERS-1);
		timeout = Timer.GetMilliTicks () + 400;
		while ((statCom[buffer] & 1) == 0  &&  Timer.GetMilliTicks() < timeout)
			Timer.WaitMilliSecs (5);
		// If there's a timeout, the JPEG core has probably crashed.
		}
	while (nBuffers > 1)
		{
		nBuffers--;
		buffers[nextBuffer].info = NULL;   // buffer available for adding another buffer
		nextBuffer = (nextBuffer+1) & (N_DLL_BUFFERS-1);
		}
#else
	if (playHook)
		{
#if NT_KERNEL
		irqMutex.Enter();
#endif
		int wait = (nBuffers > N_FRAGTABLES) ? N_FRAGTABLES : nBuffers;
		playHook = NULL;
#if NT_KERNEL
		irqMutex.Leave();
#endif
		Timer.WaitMilliSecs (wait*40);   // wait until the last passed buffer has been played
		return manager->StopIRQs ();
		}
#endif
	GNRAISE_OK;
	}





Error VirtualJPEGUnit022::FlushPlayback (DWORD bufferSize)
	{
	PrepareInfo *prep;
	DWORD dummy;
	DWORD copySize;
	FPTR src, dst;
	int buffer, buf2;


	if (bufferSize == 0)
		{
		if (playBackupInfo)
			{
//_asm int 3
			FreeVideoBuffer (playBackupInfo);
			}
		playBackupInfo = NULL;
		playBackupData = NULL;
		playBackupSize = 0;
		GNRAISE_OK;
		}

	if (GetState() != current)
		GDP("FLUSH NOT CURRENT");

	if (nBuffers <= 0)
		GNRAISE_OK;

	GNREASSERT( FinishPlayback () );

	buffer = (lastBuffer+N_DLL_BUFFERS-1) & (N_DLL_BUFFERS-1);
//GDP("flush a %ld, stat: %ld %ld %ld %ld", (long)buffer, statCom[0] & 1, statCom[1] & 1, statCom[2] & 1, statCom[3] & 1);
//GDP("  %lx, %lx", playBackupInfo, buffers[buffer].info);
	if (playBackupInfo == buffers[buffer].info)
		{
		GDP("flush save buffer");
		}
	else
		{
		copySize = buffers[buffer].byteSize;
		if (copySize > bufferSize)
			bufferSize = copySize;
		if (playBackupInfo != NULL  &&  bufferSize > playBackupSize)
			{
//_asm int 3
			FreeVideoBuffer (playBackupInfo);
			playBackupInfo = NULL;
			playBackupSize = 0;
			}
		if (playBackupInfo == NULL)
			{
			GNREASSERT( AllocateVideoBuffer (bufferSize, playBackupInfo, playBackupData) );
			playBackupSize = bufferSize;
			}
		// A DMA board will display one of the buffers. This buffer must be saved
		// to be able to free the playback pipeline.
		if (manager->fixPlaybackLongBursts  &&  ! compress  &&  ! stillMode)
			{
			GNREASSERT( ((FragmentTableClass *)buffers[buffer].info)->GetBufferPointer (src, dummy) );
			}
		else
			{
			prep = (PrepareInfo *)buffers[buffer].info;
			GNREASSERT( prep->frag->GetBufferPointer (src, dummy) );
			}
		prep = (PrepareInfo *)playBackupInfo;
		GNREASSERT( prep->frag->GetBufferPointer (dst, dummy) );
#if NT_KERNEL || VXD_VERSION
		memcpy (dst, src, copySize);
#else
		hmemcpy (dst, src, copySize);
#endif
//GDP("flush b %lx, stat: %ld %ld %ld %ld", playBackupInfo, statCom[0] & 1, statCom[1] & 1, statCom[2] & 1, statCom[3] & 1);
		GNREASSERT( InternalAddVideoBuffer (playBackupInfo, NULL, playBackupSize) );
//GDP("flush c %lx, stat: %ld %ld %ld %ld", playBackupInfo, statCom[0] & 1, statCom[1] & 1, statCom[2] & 1, statCom[3] & 1);
		}

	// Finish previous buffers.
#if 1
	GNREASSERT( FinishPlayback () );
#else
	if (playHook)
		GNREASSERT( FinishPlayback () );
	else
		{
		GDP("wait");
		while ((statCom[buffer] & 1) == 0)   // wait until the last passed buffer is playing
			Timer.WaitMilliSecs (5);
		}
	while (nBuffers > 1)
		{
		nBuffers--;
		buffers[nextBuffer].info = NULL;   // buffer available for adding another buffer
		nextBuffer = (nextBuffer+1) & (N_DLL_BUFFERS-1);
		}
#endif
//GDP("flush d %lx, stat: %ld %ld %ld %ld", playBackupInfo, statCom[0] & 1, statCom[1] & 1, statCom[2] & 1, statCom[3] & 1);

	GNRAISE_OK;
	}





Error VirtualJPEGUnit022::PlayBuffer (FPTR info, DWORD size)
	{
	GNRAISE_OK;
	}





Error VirtualJPEGUnit022::Start (void)
	{
	if (GetState() != current)
		GNRAISE(GNR_OBJECT_NOT_CURRENT);
	else
		{
		if (compress  ||  playHook)
			GNREASSERT( manager->StartIRQs () );
		manager->Start ();
		running = TRUE;
		GNRAISE_OK;
		}
	}





Error VirtualJPEGUnit022::Stop (void)
	{
	if (GetState() != current)
		GNRAISE(GNR_OBJECT_NOT_CURRENT);
	else
		{
		if (jpgMode == I22_MOTION_COMPRESS  &&  ! singleFrame  &&  stoppingCapture == 0)
			{
			// Wait for the next successful frame and then stop immediately.
			stoppingCapture++;
			DWORD timeout = Timer.GetMilliTicks() + 1000;
			while (stoppingCapture < 2  &&  Timer.GetMilliTicks() < timeout)
				;
			}
		manager->Stop ();
		FlushPlayback (0);
		running = FALSE;
#if PLAYBACK_ONE_FRAGMENT
		if (PlayBuffers[0] != NULL  &&  ! compress  &&  ! stillMode)
			{
			GDP("Freeing one-fragment buffers");
			for (int i = 0;  i < N_DLL_BUFFERS;  i++)
				{
				delete PlayBufferFrags[i];
				delete PlayBuffers[i];
				PlayBuffers[i] = NULL;
				PlayBufferFrags[i] = NULL;
				}
			}
#endif
		GNRAISE_OK;
		}
	}





// Preempt the other virtual unit and program our state into the chips.
// We must be the current unit before this call.

Error VirtualJPEGUnit022::Preempt (VirtualUnit *previous)
	{
	VirtualJPEGUnit022 *prev = (VirtualJPEGUnit022 *)previous;
	JPEGMsg msg;


	if (this != previous  &&  GetState() == current)
		{
		GNREASSERT( coder->Activate () );
		manager->io->ClearBit (I22REG_P_RESET);   // reset JPEG process
		manager->z016->Stop ();
		manager->pxoe016Bit->OutBit (LOW);   // disable PXOUT bus
		manager->io->ClearBit (I22REG_ACTIVE);
		manager->io->ClearBit (I22REG_COD_TRNS_EN);
#if VIONA_VERSION
		if (prev != NULL)
			{
			// Switching from playback to still operation blanks the video output.
			if (stillMode  &&  ! prev->stillMode  &&  ! prev->compress  &&  manager->blankUnit != NULL)
				manager->blankUnit->Activate ();
			// Inform the preempted user that his operation is suspended.
			if (prev->preemptHook != NULL)
				{
				msg.userData = NULL;
				prev->preemptHook.Call (msg);
				}
/*			manager->syncSelection->OutByte (SYNCSEL_DECODER);
			manager->decoderSyncEnable->OutBit (HIGH);
			manager->frontEndEnable->OutBit (HIGH);
			manager->videoBusDecode->OutBit (LOW);
*/			}
#else
		manager->syncMUX->OutBit (0);
		if (prev != NULL  &&  prev->preemptHook != NULL)
			{
			// Inform the preempted user that his operation is suspended.
			msg.userData = NULL;
			prev->preemptHook.Call (msg);
			}
#endif
		}
	GNRAISE_OK;
	}





// Calculate the video parameters according to the current state. The horizontal
// and vertical offsets are calculated in difference to the 7110 programming.

void VirtualJPEGUnit022::CalcVideoParameters (void)
	{
	// These arrays define the video window processing parameters for the 7110 as
	// the external sync master, indexed by the video standard (NTSC, PAL, SECAM
	// in that order).

	static int VLineStart [VIDEOSTANDARD_MAX+1] = { 12, 16, 16 };
	static int HPixelStart[VIDEOSTANDARD_MAX+1] = { 47, 83, 83 };

	static int VLineStartCCIR [VIDEOSTANDARD_MAX+1] = { 11, 16, 16 };
	static int HPixelStartCCIR[VIDEOSTANDARD_MAX+1] = { 61, 72, 72 };

	static int VSyncSize [VIDEOSTANDARD_MAX+1] = { 6, 6, 6 };
	static int HSyncStart[VIDEOSTANDARD_MAX+1] = { 640, 768, 768 };
	static int HSyncStartCCIR[VIDEOSTANDARD_MAX+1] = { 720, 720, 720 };
	static int HSyncStartCCIRPlay[VIDEOSTANDARD_MAX+1] = { 733, 730, 730 };

	// For I22 as internal sync master.
	static int VLineStart16 [VIDEOSTANDARD_MAX+1] = { 6, 6, 6 };
	static int HPixelStart16[VIDEOSTANDARD_MAX+1] = { 0, 0, 0 };

	const struct HardVideoParams *hard;


	// Select the JPEG mode. Note that I22_STILL_DECOMPRESS denotes the mode
	// using the still transfer register for expansion.
	if (compress)
		jpgMode = stillMode ? I22_STILL_COMPRESS : I22_MOTION_COMPRESS;
	else
#if PIP_EXPAND
		jpgMode = I22_MOTION_DECOMPRESS;
#else
		jpgMode = stillMode ? I22_STILL_DECOMPRESS : I22_MOTION_DECOMPRESS;
#endif


	if (manager->ccirMode  &&  (compress || ! stillMode))
		hard = (vStandard == VSTD_NTSC) ? &NTSCParams : &PALParams;
	else
		hard = (vStandard == VSTD_NTSC) ? &NTSCSquareParams : &PALSquareParams;


	// Which operation width do we have ?
	if (width > (WORD)hard->activeWidth / 2  ||  jpgMode == I22_STILL_COMPRESS  ||  jpgMode == I22_STILL_DECOMPRESS)
		{
		horDecimation = FALSE;
		opWidth = width;
		}
	else
		{
		horDecimation = TRUE;
		opWidth = width * 2;
		}


	// Which operation height do we have ?
	if (height > (WORD)hard->activeHeight / 2)
		opHeight = height / 2;   // two fields
	else
		opHeight = height;   // one field


	switch (jpgMode)
		{
		case I22_STILL_DECOMPRESS:
#if 0
			// These work, only the horizontal starting point is not correct.
			syncMaster = TRUE;
			vSyncSize  = 6;
			frameTotal = 2 * opHeight + 19;
			hSyncStart = opWidth + 8;
			lineTotal  = opWidth + 32;
			nax        = 1;
			pax        = opWidth;
			nay        = 1;
			pay        = opHeight;
			hOffset = nax - hard->hStart;
			vOffset = nay - hard->vStart - 1;
#else
			syncMaster = TRUE;
			vSyncSize  = 6 + 10;
			frameTotal = 2 * opHeight + 19 + 10;
			hSyncStart = opWidth;
			lineTotal  = opWidth + 32;
			nax        = 1;
			pax        = opWidth;
			nay        = 1;
			pay        = opHeight;
			hOffset = nax - hard->hStart;
			vOffset = nay - hard->vStart - 1;
#endif
			break;

		case I22_STILL_COMPRESS:
			syncMaster = TRUE;
			vSyncSize  = 6;
			frameTotal = 2 * opHeight + 19;
			hSyncStart = opWidth + 8;
			lineTotal  = opWidth + 16;
			nax        = 1;
			pax        = opWidth;
			nay        = 1;
			pay        = opHeight;
			hOffset = nax - hard->hStart;
			vOffset = nay - hard->vStart - 1;
			break;

		case I22_MOTION_DECOMPRESS:
			syncMaster = TRUE;
			if (stillExpansion)
				{
				vSyncSize  = 16;
				frameTotal = hard->totalHeight;
				hSyncStart = manager->ccirMode ? HSyncStart[vStandard] : HSyncStart[vStandard];
				lineTotal  = hard->totalWidth;
				nax        = HPixelStart16[vStandard];
				pax        = opWidth;
				nay        = VLineStart16[vStandard] + 1;
				pay        = opHeight;
				hOffset = nax - hard->hStart;
				vOffset = nay - hard->vStart - 1;
				hOffset += horDecimation ? 24 : 20;
				vOffset += 8;
				}
			else
				{
				vSyncSize  = (vStandard == VSTD_NTSC) ? 17 : 22;
				frameTotal = hard->totalHeight;
				hSyncStart = manager->ccirMode ? HSyncStartCCIRPlay[vStandard] : HSyncStart[vStandard];
				lineTotal  = hard->totalWidth;
#if VIONA_VERSION
				nax        = HPixelStart16[vStandard] + (hard->activeWidth - opWidth) / 2;
				pax        = hard->activeWidth;
				nay        = 1 + (hard->activeHeight/2 - opHeight) / 2;   // + VLineStart16[vStandard];
				pay        = hard->activeHeight / 2;
				hOffset = nax - hard->hStart;
				vOffset = 2*(nay-1) - hard->vStart;
				hOffset += horDecimation ? 24 : 20;
				vOffset += 2;
				if (manager->ccirMode)
					{
					hOffset += 0;
					vOffset += 0;
					}
				if ((hOffset & 1) == 0)
					{
					hOffset--;   // for correct PIP offset, hOffset must be odd
					}
#else
				nax        = HPixelStart16[vStandard];
				pax        = hard->activeWidth;
				nay        = VLineStart16[vStandard] + 1;
				pay        = hard->activeHeight / 2;
				hOffset = nax - hard->hStart;
				vOffset = nay - hard->vStart - 1;
				hOffset += horDecimation ? 24 : 20;
				vOffset += 8;
#endif
				}
			break;

		case I22_MOTION_COMPRESS:
		default:
			syncMaster = FALSE;
#if VIONA_VERSION
			nax        = manager->ccirMode ? HPixelStartCCIR[vStandard] : HPixelStart[vStandard];
			nay        = manager->ccirMode ? VLineStartCCIR[vStandard] : VLineStart[vStandard];
			nax       += (hard->activeWidth - opWidth) / 2;
			nay       += (hard->activeHeight/2 - opHeight) / 2 + 1;
#else
			nax        = HPixelStart[vStandard];
			nay        = VLineStart[vStandard] + 1;
#endif
			if (manager->ccirMode)
				{
				if (vStandard == VSTD_NTSC)
					{
					hOffset = -62;
					vOffset = 4;
					}
				else
					{
					hOffset = -61;
					vOffset = -12;
					}
				}
			else
				{
				hOffset = 0;   // 7110 drives video bus
				vOffset = 0;
				}
			// unimportant:
			vSyncSize  = 6;
			frameTotal = hard->totalHeight;
			hSyncStart = manager->ccirMode ? HSyncStartCCIR[vStandard] : HSyncStart[vStandard];
			lineTotal  = hard->totalWidth;
			pax        = hard->activeWidth;
			pay        = hard->activeHeight / 2;
			break;
		}
	}
