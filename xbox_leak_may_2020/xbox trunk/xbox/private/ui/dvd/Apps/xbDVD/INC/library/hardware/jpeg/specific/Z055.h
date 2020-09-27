
// FILE:      library\hardware\jpeg\generic\z055.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   02.02.96
//
// PURPOSE: The ZR36055 motion JPEG controller class.
//
// HISTORY:

#ifndef Z055_H
#define Z055_H

#include "library\lowlevel\hardwrio.h"
#include "library\lowlevel\dmabuffr.h"
#include "library\lowlevel\intrctrl.h"
#include "library\hardware\video\generic\vidtypes.h"

#include "library\support\gfxdebug\gfxdebug.h"



#define H55_N_BUFFERS  4



// 055 registers

#define H55REG_CODMEM      0x0   // WORD
#define H55REG_050_ADDRESS 0x2   // WORD
#define H55REG_050_DATA    0x4

#define H55REG_STATUS0    0x5
#define H55IDX_FNV         5
#define H55REG_FNV         H55REG_STATUS0, H55IDX_FNV
#define H55IDX_STRP_AVAL   4
#define H55REG_STRP_AVAL   H55REG_STATUS0, H55IDX_STRP_AVAL
#define H55IDX_RDY         3
#define H55REG_RDY         H55REG_STATUS0, H55IDX_RDY
#define H55IDX_LASTPAGE    0, 2
#define H55REG_LASTPAGE    H55REG_STATUS0, H55IDX_LASTPAGE

#define H55REG_LEN0       0x6
#define H55REG_LEN1       0x7

#define H55REG_CMD0       0x9
#define H55IDX_FAST        7
#define H55REG_FAST        H55REG_CMD0, H55IDX_FAST
#define H55IDX_FFI         6
#define H55REG_FFI         H55REG_CMD0, H55IDX_FFI
#define H55IDX_GRESET      5
#define H55REG_GRESET      H55REG_CMD0, H55IDX_GRESET
#define H55IDX_VID422      4
#define H55REG_VID422      H55REG_CMD0, H55IDX_VID422
#define H55IDX_EXP         3
#define H55REG_EXP         H55REG_CMD0, H55IDX_EXP
#define H55IDX_STILL       2
#define H55REG_STILL       H55REG_CMD0, H55IDX_STILL
#define H55IDX_PAGE        0, 2
#define H55REG_PAGE        H55REG_CMD0, H55IDX_PAGE

#define H55REG_CMD1       0xA
#define H55IDX_VID_FORMAT  7
#define H55REG_VID_FORMAT  H55REG_CMD1, H55IDX_VID_FORMAT
#define H55IDX_INT_EN      6
#define H55REG_INT_EN      H55REG_CMD1, H55IDX_INT_EN
#define H55IDX_CLK_EN      5
#define H55REG_CLK_EN      H55REG_CMD1, H55IDX_CLK_EN
#define H55IDX_SNC_SRC     4
#define H55REG_SNC_SRC     H55REG_CMD1, H55IDX_SNC_SRC
#define H55IDX_HOR_DEC     3
#define H55REG_HOR_DEC     H55REG_CMD1, H55IDX_HOR_DEC
#define H55IDX_VER_DEC     2
#define H55REG_VER_DEC     H55REG_CMD1, H55IDX_VER_DEC
#define H55IDX_CBUF_SIZE   1
#define H55REG_CBUF_SIZE   H55REG_CMD1, H55IDX_CBUF_SIZE
#define H55IDX_ACTIVE_N    0
#define H55REG_ACTIVE_N    H55REG_CMD1, H55IDX_ACTIVE_N

#define H55REG_CMD2       0x8
#define H55IDX_STRP_SIZE   7
#define H55REG_STRP_SIZE   H55REG_CMD2, H55IDX_STRP_SIZE
#define H55IDX_VID_FILT    6
#define H55REG_VID_FILT    H55REG_CMD2, H55IDX_VID_FILT
#define H55IDX_ID          0, 6
#define H55REG_ID          H55REG_CMD2, H55IDX_ID

#define H55REG_FCNT       0xB
#define H55REG_STRIPBUFF  0xC   // WORD
#define H55REG_IMAGE_SIZE 0xE   // WORD



class Z055
	{
	private:
		IndexedInOutPort *io;
		BOOL fastClock, codeBuf256K, stripBuf32K;   // hardware restrictions
		BYTE defCmd0, defCmd1, defCmd2;

		DWORD cmd0, cmd1, cmd2;   // caching the hardware registers
		BYTE fieldCount, status;
		WORD length;

		BOOL stillMode;

		void SetImageDimension (WORD mask, WORD dimension);

	public:
		Z055 (IndexedInOutPort *io, BOOL fastClock, BOOL codeBuf256K, BOOL stripBuf32K);

		BOOL IsPresent (void);

		void Reset (void);
		void StartActivity (void);
		void StopActivity (void);

		void ReadStatus (void);
		WORD GetFieldLength (void) {return length;}
		BYTE GetFieldCount (void) {return fieldCount;}
		BYTE GetPageNum (void) {return (BYTE)XTBF(H55IDX_LASTPAGE, status);}

		void SetPageNum (BYTE num);

		void SetBlocksInStrip (WORD blocks, BOOL lastStrip);

		BOOL NewStatusReady (void)
			{ io->InByte (H55REG_STATUS0, status); return XTBF(H55IDX_RDY, status) != 0; }

		BOOL StripReady (void)
			{ io->InByte (H55REG_STATUS0, status); return XTBF(H55IDX_STRP_AVAL, status) != 0; }

		BOOL InitOperation (BOOL compress, BOOL stillMode, VideoStandard vStd,
		                    VideoFormat vFormat, VideoField fieldOrder,
		                    BOOL filtering, WORD width, WORD height);
	};



#endif
