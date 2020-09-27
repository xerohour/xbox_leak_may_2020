
// FILE:      library\hardware\jpeg\generic\z055.cpp
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   02.02.96
//
// PURPOSE: The ZR36055 motion JPEG controller class.
//
// HISTORY:

#include "z055.h"
#include "library\lowlevel\timer.h"
#include "library\lowlevel\timer.h"





#define H55_IDENTIFIER  0x15

#define H55_DIMENSION_MASK  0x07FF

#define H55_HIMG_STRT       0x0000
#define H55_HIMG_END        0x1000
#define H55_HIMG_TOTAL      0x2000
#define H55_VIMG_STRT       0x8000
#define H55_VIMG_END        0x9000
#define H55_VIMG_TOTAL      0xA000
#define H55_HSYNC_STRT      0x3000
#define H55_HSYNC_END       0x4000
#define H55_ABLANK          0x5000
#define H55_MCUS_IN_LINE    0xC000
#define H55_BLOCKS_IN_STRIP 0xD000
#define H55_LAST_STRIP      0xD800
#define H55_BACK_COLOUR_Y   0xE000
#define H55_BACK_COLOUR_UV  0xE800





// These arrays define the video window processing parameters for the 055 as
// the sync master. They are indexed by the video standard (NTSC, PAL, SECAM
// in that order).

WORD FULL_HEIGHT[VIDEOSTANDARD_MAX+1] = { 480, 576, 576 };
WORD HALF_HEIGHT[VIDEOSTANDARD_MAX+1] = { 240, 288, 288 };
WORD FULL_WIDTH[VIDEOSTANDARD_MAX+1]  = { 640, 768, 768 };
WORD HALF_WIDTH[VIDEOSTANDARD_MAX+1]  = { 320, 384, 384 };
WORD VTOTAL[VIDEOSTANDARD_MAX+1]      = { 525, 625, 625 };
WORD HTOTAL[VIDEOSTANDARD_MAX+1]      = { 780, 944, 944 };
WORD BLANK[VIDEOSTANDARD_MAX+1]       = { 140, 176, 176 };
WORD SKIP[VIDEOSTANDARD_MAX+1]        = {  14,  18,  18 };
WORD HSYNC_START[VIDEOSTANDARD_MAX+1] = { 659, 795, 795 };
WORD HSYNC_END[VIDEOSTANDARD_MAX+1]   = { 723, 859, 859 };
WORD ABLANK[VIDEOSTANDARD_MAX+1]      = { 640, 768, 768 };





Z055::Z055 (IndexedInOutPort *io, BOOL fastClock, BOOL codeBuf256K, BOOL stripBuf32K)
	{
	this->io = io;
	this->fastClock   = fastClock;
	this->codeBuf256K = codeBuf256K;
	this->stripBuf32K = stripBuf32K;
	defCmd0 = (BYTE) MKBF(H55IDX_FAST, fastClock);
	defCmd1 = (BYTE) MKBF(H55IDX_CBUF_SIZE, codeBuf256K);
	defCmd2 = (BYTE) MKBF(H55IDX_STRP_SIZE, stripBuf32K);
	}





BOOL Z055::IsPresent (void)
	{
	BYTE id;
	io->InByte (H55REG_CMD2, id);
	return XTBF(H55IDX_ID, id) == H55_IDENTIFIER;
	}





void Z055::Reset (void)
	{
	cmd1 = WRBF(cmd1, H55IDX_CLK_EN, HIGH);
	io->OutByte (H55REG_CMD1, (BYTE)cmd1);
	cmd0 = WRBF(cmd0, H55IDX_GRESET, LOW);
	io->OutByte (H55REG_CMD0, (BYTE)cmd0);
	Timer.WaitMicroSecs (1);   // 050 needs reset for at least four CLK_INs
	cmd0 = WRBF(cmd0, H55IDX_GRESET, HIGH);
	io->OutByte (H55REG_CMD0, (BYTE)cmd0);
	}





void Z055::StartActivity (void)
	{
	cmd1 = WRBF(cmd1, H55IDX_INT_EN, (stillMode ? LOW : HIGH));
	cmd1 = WRBF(cmd1, H55IDX_ACTIVE_N, LOW);
	io->OutByte (H55REG_CMD1, (BYTE)cmd1);
	}





void Z055::StopActivity (void)
	{
	// Disable interrupts.
	cmd1 = WRBF(cmd1, H55IDX_INT_EN, LOW);
	io->OutByte (H55REG_CMD1, (BYTE)cmd1);
	// Switch to compression mode to disable video bus drivers.
	cmd0 = WRBF(cmd0, H55IDX_EXP, LOW);
	io->OutByte (H55REG_CMD0, (BYTE)cmd0);
	}





void Z055::ReadStatus (void)
	{
	BYTE len0, len1;

	// Note that the read order has the 055 generate the next interrupt.
	// STATUS0 must be read first, LEN1 must be read last.
	io->InByte (H55REG_STATUS0, status);
	io->InByte (H55REG_FCNT, fieldCount);
	io->InByte (H55REG_LEN0, len0);
	io->InByte (H55REG_LEN1, len1);
	*((BYTE *)(&length))   = len0;
	*((BYTE *)(&length)+1) = len1;
	if (XTBF(H55IDX_FNV, status) != 0)
		length = 0;   // invalid field: set length to zero
	}





void Z055::SetPageNum (BYTE num)
	{
	cmd0 = WRBF(cmd0, H55IDX_PAGE, num);
	io->OutByte (H55REG_CMD0, (BYTE)cmd0);
	}





void Z055::SetBlocksInStrip (WORD blocks, BOOL lastStrip)
	{
	SetImageDimension (H55_BLOCKS_IN_STRIP | (lastStrip ? H55_LAST_STRIP : 0), blocks);
	}





	inline void Z055::SetImageDimension (WORD mask, WORD dimension)
		{
		WORD w = dimension;
		w &= H55_DIMENSION_MASK;
		w |= mask;
		io->OutWord (H55REG_IMAGE_SIZE, w);
		}





// Initialize for an operation. Return success.

BOOL Z055::InitOperation (BOOL compress, BOOL stillMode, VideoStandard vStd,
                          VideoFormat vFormat, VideoField fieldOrder,
                          BOOL filtering, WORD width, WORD height)
	{
	WORD activePages, activeWidth, activeHeight, activeHTotal, activeVTotal;
	WORD activeBlank, activeSkip, activeSyncStart, activeSyncEnd, activeMCUSize;


	this->stillMode = stillMode;
	cmd0 = defCmd0;
	cmd1 = defCmd1;
	cmd2 = defCmd2;

	// For full width, we need the fast 055 version.
	if (width > HALF_WIDTH[vStd]  &&  ! fastClock)
		return FALSE;

	// Set reset, be not active and disable coder clock.
	cmd0 = WRBF(cmd0, H55IDX_GRESET, LOW);
	io->OutByte (H55REG_CMD0, (BYTE)cmd0);
	cmd1 = WRBF(cmd1, H55IDX_ACTIVE_N, HIGH);
	cmd1 = WRBF(cmd1, H55IDX_CLK_EN, LOW);
	io->OutByte (H55REG_CMD1, (BYTE)cmd1);

	cmd0 = WRBF(cmd0, H55IDX_VID422, (vFormat == VFMT_YUV_422));
	cmd1 = WRBF(cmd1, H55IDX_VID_FORMAT, (vStd == VSTD_NTSC));
	if (width > HALF_WIDTH[vStd])
		{
		cmd1 = WRBF(cmd1, H55IDX_HOR_DEC, LOW);
		activeWidth = width;
		}
	else
		{
		cmd1 = WRBF(cmd1, H55IDX_HOR_DEC, HIGH);
		activeWidth = width * 2;
		}
	cmd2 = WRBF(cmd2, H55IDX_VID_FILT, filtering);
	if (fieldOrder != VFLD_SINGLE)
		height /= 2;

	if (compress)
		{
		// Compression.
		cmd0 = WRBF(cmd0, H55IDX_EXP, LOW);
		activePages = H55_N_BUFFERS;
		cmd0 = WRBF(cmd0, H55IDX_PAGE, H55_N_BUFFERS-1);   // select page 3
		cmd1 = WRBF(cmd1, H55IDX_VER_DEC, (fieldOrder != VFLD_SINGLE));
		}
	else
		{
		// Expansion.
		cmd0 = WRBF(cmd0, H55IDX_EXP, HIGH);
		activePages = 2;
		cmd0 = WRBF(cmd0, H55IDX_PAGE, 0);   // select page 0
		switch (fieldOrder)
			{
			case VFLD_EVEN:
				cmd1 = WRBF(cmd1, H55IDX_VER_DEC, LOW);
				cmd0 = WRBF(cmd0, H55IDX_FFI, HIGH);
				break;
			case VFLD_ODD:
				cmd1 = WRBF(cmd1, H55IDX_VER_DEC, LOW);
				cmd0 = WRBF(cmd0, H55IDX_FFI, LOW);
				break;
			case VFLD_SINGLE:
			default:
				cmd1 = WRBF(cmd1, H55IDX_VER_DEC, HIGH);
				break;
			}
		cmd1 = WRBF(cmd1, H55IDX_SNC_SRC, LOW);
		}

	activeMCUSize = (vFormat == VFMT_YUV_422) ? 16 : 8;
	cmd0 = WRBF(cmd0, H55IDX_STILL, stillMode);
	cmd1 = WRBF(cmd1, H55IDX_INT_EN, LOW);

	io->OutByte (H55REG_CMD0, (BYTE)cmd0);
	io->OutByte (H55REG_CMD1, (BYTE)cmd1);
	io->OutByte (H55REG_CMD2, (BYTE)cmd2);

	activeHeight = height;
	activeVTotal = VTOTAL[vStd];
	activeHTotal = HTOTAL[vStd];
	activeBlank = BLANK[vStd];
	activeSkip = SKIP[vStd];
	activeSyncStart = HSYNC_START[vStd];
	activeSyncEnd = HSYNC_END[vStd];

	WORD hCenter = FULL_WIDTH[vStd] / 2;
	WORD hStart = hCenter - (activeWidth / 2);
	WORD hEnd = hCenter + (activeWidth / 2);

	SetImageDimension (H55_HIMG_STRT, hStart);
	SetImageDimension (H55_HIMG_END, (hEnd - 1));
	SetImageDimension (H55_HIMG_TOTAL, activeHTotal);

	WORD vCenter = (HALF_HEIGHT[vStd] / 2) + activeSkip;
	WORD vStart = vCenter - (activeHeight / 2);
	WORD vEnd = vCenter + (activeHeight / 2);

	SetImageDimension (H55_VIMG_STRT, vStart);
	SetImageDimension (H55_VIMG_END, vEnd - 1);
	SetImageDimension (H55_VIMG_TOTAL, activeVTotal);

	SetImageDimension (H55_HSYNC_STRT, activeSyncStart);
	SetImageDimension (H55_HSYNC_END, activeSyncEnd);

	SetImageDimension (H55_MCUS_IN_LINE, ((hEnd - hStart) / activeMCUSize) - 1);
	SetImageDimension (H55_ABLANK, ABLANK[vStd]);

	cmd1 = WRBF(cmd1, H55IDX_CLK_EN, HIGH);
	io->OutByte (H55REG_CMD1, (BYTE)cmd1);

	// Delay (at least) 5000 coder clocks to allow PLL to stabilize.
	Timer.WaitMicroSecs (250);

	cmd0 = WRBF(cmd0, H55IDX_GRESET, HIGH);
	io->OutByte (H55REG_CMD0, (BYTE)cmd0);

	// Note that background colour cannot be changed during reset.
	SetImageDimension (H55_BACK_COLOUR_Y,  0);   // black background
	SetImageDimension (H55_BACK_COLOUR_UV, 128);

	return TRUE;
	}
