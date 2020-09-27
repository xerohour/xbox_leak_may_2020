#ifndef __WMVDEC_MEMBER_H_
#define __WMVDEC_MEMBER_H_
#include "wmvdec_api.h"
#include "typedef.hpp"

#include "xplatform.h"
#include "stdio.h"
#include "Huffman_wmv.hpp"
#include "strmdec_wmv.hpp"
#include "motioncomp_wmv.h"
#include "bitfields.h"
#include "wmvdec_api.h"

//class CInputBitStream_WMV;
class ifstream;
class istream;
//class Huffman_WMV;

#ifdef _6BIT_COLORCONV_OPT_
typedef struct
{
    I8_WMV g_iYscale [256];
    I8_WMV g_iVtoR [256];
    I8_WMV g_iVtoG [256];
    I8_WMV g_iUtoG [256];
    I8_WMV g_iUtoB [256];
    
}
YUV2RGB_6BIT_TABLE;

#endif

#ifdef _EMB_WMV2_

/* the relative offsets of the fields of these classes are important for CE assembly optimizations. Affected files are:
    blkdec_ce_arm.s
*/

struct tagWMVDecInternalMember;

typedef struct _CDCTTableInfo_Dec {
	Huffman_WMV* hufDCTACDec; 
	U32_WMV  iTcoef_ESCAPE;
	U32_WMV  iStartIndxOfLastRunMinus1;
    U8_WMV* puiNotLastNumOfLevelAtRun; 
    U8_WMV* puiLastNumOfLevelAtRun; 
    U8_WMV* puiNotLastNumOfRunAtLevel; 
    U8_WMV* puiLastNumOfRunAtLevel;
    I8_WMV* pcLevelAtIndx; 
    U8_WMV* puiRunAtIndx; 
}CDCTTableInfo_Dec;

/* the relative offsets of the fields of these classes are important for CE assembly optimizations. Affected files are:
    blkdec_ce_arm.s
*/
typedef struct tagEMB_DecodeP_ShortCut
{

    struct tagWMVDecInternalMember * pVideoDecoderObject;

    I32_WMV * m_rgiCoefRecon;
    own CInputBitStream_WMV * m_pbitstrmIn;
    I32_WMV m_i2DoublePlusStepSize;
    I32_WMV m_iDoubleStepSize;
    I32_WMV m_iStepMinusStepIsEven;

    I32_WMV XFormMode;
    I32_WMV m_iDCTHorzFlags;

}EMB_DecodeP_ShortCut;

#else

typedef struct _CDCTTableInfo_Dec {
	Huffman_WMV* hufDCTACDec; 
	U32_WMV  iTcoef_ESCAPE;
	U32_WMV  iStartIndxOfLastRunMinus1;
    U8_WMV* puiNotLastNumOfLevelAtRun; 
    U8_WMV* puiLastNumOfLevelAtRun; 
    U8_WMV* puiNotLastNumOfRunAtLevel; 
    U8_WMV* puiLastNumOfRunAtLevel;
    I8_WMV* pcLevelAtIndx; 
    U8_WMV* puiRunAtIndx; 
}CDCTTableInfo_Dec;

#endif


enum THREADTYPE_DEC {REPEATPAD, DECODE, RENDER, LOOPFILTER, DEBLOCK, DEBLOCK_RENDER};
enum KEYS {FPPM, CPPM, PPM, RES, BITRATE};
enum SKIPBITCODINGMODE {Normal = 0, RowPredict, ColPredict};
//I32_WMV AccessRegistry(enum KEYS k, char rw, I32_WMV iData);

struct t_ContextWMV;
struct t_SpatialPredictor;
struct t_AltTablesDecoder;
// WMV2's new routines

#ifdef DYNAMIC_EDGEPAD
typedef struct
{
    I32_WMV m_iStartOfLastLine;
    I16_WMV m_iPMBMV_limit_x;  //m_iWidth-BLOCKSIZE_PLUS_2EDGES
    I16_WMV m_iPMBMV_limit_y;  //m_iHeight-BLOCKSIZE_PLUS_2EDGES
    I16_WMV m_iWidth;    
    I16_WMV m_iHeight;
}EdgePad_Y_UV_Data;

typedef struct
{
    U8_WMV * m_ppxliRef; //MB(Y) or Blk(UV) starting address for ref plane
    U8_WMV * m_ppxliCurr; //MB(Y) or Blk(UV) starting address for curr plane
    U8_WMV * m_ppxliRef0; //origins of ref for the YUV planes
         
    I32_WMV m_BlkInc;  //block incremental in the normal case
}
EdgePad_Blk_Data;

typedef struct _WMVEdgePad
{
    I16_WMV m_iPBMV_x;  //position of the (enlarged) block : block + surrounding edges
    I16_WMV m_iPBMV_y;  
    I16_WMV m_iWidthYMinusAllEdges;  
    I16_WMV m_iHeightYMinusAllEdges;  
    I16_WMV x_off_left;
    I16_WMV x_off_right;
    I32_WMV src_offset; 
    U8_WMV * ppxlcRefBufferOrig; 
    U8_WMV * ppxlcRef; 
    U8_WMV * ppxlcCurr;
    U32_WMV m_iWidthCombine;
    U32_WMV m_iround_ctrl;

    EdgePad_Y_UV_Data y_uv_data[2];

    EdgePad_Blk_Data blk_data[6];
    

    U8_WMV * buffer;

}
tWMVEdgePad;

#endif
// basic structs

typedef enum tagFrameType_WMV
{
//    IFRAME_WMV = 0, 
//    PFRAME_WMV
      IVOP = 0, 
      PVOP
} tFrameType_WMV;

typedef struct tagYUV420Frame_WMV
{
    U8_WMV* m_pucYPlane;
    U8_WMV* m_pucUPlane;
    U8_WMV* m_pucVPlane;

    U8_WMV* m_pucYPlane_Unaligned;
    U8_WMV* m_pucUPlane_Unaligned;
    U8_WMV* m_pucVPlane_Unaligned;

} tYUV420Frame_WMV;

typedef struct tagMotionVector_WMV
{
    I16_WMV m_iMVX;
    I16_WMV m_iMVY;
} tMotionVector_WMV;


// ===========================================================================
// private data members
// ===========================================================================
struct tagWMVDecInternalMember;
typedef struct tagWMVDecInternalMember 
{
    // Don't add any thing before this variable! It has been used by ARM assembly code
    #if defined(OUTPUT_ALL_FORMATS) || defined(OUTPUT_RGB12) || defined(OUTPUT_GRAY4)
        U8_WMV* m_rgiClapTab4BitDec;
        U32_WMV* m_rgiDithering;
        I32_WMV m_iDitheringIndex;
    #endif

    U32_WMV m_uiUserData;

    // sequence layer info
CInputBitStream_WMV     *m_pInputBitstream;
    own CInputBitStream_WMV* m_pbitstrmIn;
    U32_WMV m_uiFOURCCCompressed;
    I32_WMV m_iPostFilterLevel;
    Float_WMV m_fltFrameRate;
    Float_WMV m_fltBitRate;
    I32_WMV m_iFrameRate;
    I32_WMV m_iBitRate; // in Kbps

    //VOLMode m_volmd; // vol mode
    //VOPMode m_vopmd; // vop mode
    Bool_WMV m_fPrepared;
    Bool_WMV m_fDecodedI;

    U16_WMV m_uiNumFramesLeftForOutput;

    // size info
    I32_WMV m_iWidthSource; // source width, might not be MB aligned
    I32_WMV m_iHeightSource; // source height, might not be MB aligned
    I32_WMV m_iWidthInternal; // internal width, MB aligned
    I32_WMV m_iWidthInternalTimesMB;
    I32_WMV m_iHeightInternal;
    I32_WMV m_iWidthInternalUV;
    I32_WMV m_iWidthInternalUVTimesBlk;
    I32_WMV m_iHeightInternalUV;
    Bool_WMV m_bSizeMBAligned;
    U32_WMV m_uiNumMB, m_uiNumMBX, m_uiNumMBY;
U32_WMV m_uintNumMBX, m_uintNumMBY, m_uintNumMB, m_uiRightestMB;

    Bool_WMV m_bMBAligned;

    I32_WMV m_iFrmWidthSrc, m_iFrmHeightSrc; // original, non-MB-aligned size of input bitmap
    I32_WMV m_iWidthYRepeatPad, m_iWidthUVRepeatPad; // equal original for MP43, else = m_iWidthY
    I32_WMV m_iHeightYRepeatPad, m_iHeightUVRepeatPad; // equal original for MP43, else = m_iHeightY
    I32_WMV m_iWidthY, m_iWidthYPlusExp, m_iHeightY;
    I32_WMV m_iWidthUV, m_iWidthUVPlusExp, m_iHeightUV;
    I32_WMV m_iWidthPrevY, m_iWidthPrevUV;
    I32_WMV m_iHeightPrevY, m_iHeightPrevUV;
    I32_WMV m_iWidthPrevYXExpPlusExp, m_iWidthPrevUVXExpPlusExp;
	I32_WMV m_iMBSizeXWidthPrevY; // MB size (16) * width of Y-Plane, for accessing MB data
	I32_WMV m_iBlkSizeXWidthPrevUV;
    I32_WMV m_iWidthPrevYxBlkMinusBlk;
    I32_WMV m_iMVLeftBound, m_iMVRightBound;

    // tables
    const U8_WMV* m_rgiClapTabDec;


    // pictures, frame's data
    tYUV420Frame_WMV* m_pfrmCurrQ;
    tYUV420Frame_WMV* m_pfrmPrev;
    tYUV420Frame_WMV* m_pfrmPostQ;
 	U8_WMV* m_ppxliCurrQPlusExpY;
	U8_WMV* m_ppxliCurrQPlusExpU;
	U8_WMV* m_ppxliCurrQPlusExpV;
    U8_WMV* m_ppxliRef0Y;
    U8_WMV* m_ppxliRef0U;
    U8_WMV* m_ppxliRef0V;
    U8_WMV* m_ppxliRef0YPlusExp;
    U8_WMV* m_ppxliPostQY;
    U8_WMV* m_ppxliPostQU;
    U8_WMV* m_ppxliPostQV;
    U8_WMV* m_ppxliPostQPlusExpY;
    U8_WMV* m_ppxliPostQPlusExpU;
    U8_WMV* m_ppxliPostQPlusExpV;

    CVector* m_rgmv;                    // 2 MB row's MV
    CWMVMBMode*	m_rgmbmd;               // 2 MB row's MV
    CWMVMBMode*	m_pmbmdZeroCBPCY;       // Pointer to the ZeroCBPCY
    U8_WMV* m_rgchSkipPrevFrame;            // Prev frame's Skip bits

    // sequence layer info
    Bool_WMV m_bIFrameDecoded;
    Bool_WMV m_bXintra8Switch;
    Bool_WMV m_bXformSwitch;
    Bool_WMV m_bSKIPBIT_CODING_;
    Bool_WMV m_bNEW_PCBPCY_TABLE;
    Bool_WMV m_bMixedPel;
    Bool_WMV m_bLoopFilter;
    Bool_WMV m_bRndCtrlOn;
    I32_WMV m_iRndCtrl;
//    Bool_WMV m_iSliceCode;
    I32_WMV m_iSliceCode;
    U32_WMV m_uintNumMBYSlice; // slice size (number of MB's vertically)
    CodecVersion m_cvCodecVersion;
    Bool_WMV m_bMainProfileOn;
    Bool_WMV m_bMMXSupport;

    // picture layer info.
    tFrameType_WMV m_tFrmType;
    SKIPBITCODINGMODE m_SkipBitCodingMode;
    Bool_WMV m_bXintra8;
    Bool_WMV m_bFrmHybridMVOn;
    Bool_WMV m_bDCTTable_MB, m_bDCTTable_MB_ENABLED, m_bDCPred_IMBInPFrame;
    Bool_WMV m_bCODFlagOn;          // COD flag is on (coded) for a P frame
    I32_WMV m_iMvResolution;        // Indicate current frame's MV resolution

    I32_WMV m_iStepSize;

    I32_WMV m_iDCStepSize, m_iDCStepSizeC;
    I32_WMV m_i2DoublePlusStepSize, m_i2DoublePlusStepSizeNeg;
    I32_WMV m_iDoubleStepSize, m_iStepMinusStepIsEven;
    Bool_WMV m_bStepSizeIsEven;

    // MB layer info
    Bool_WMV m_bMBHybridMV;         // Indicates whether MB-based HybridMV is on
    Bool_WMV m_bMBXformSwitching;   // Indicates whether MB-based transform switching is chosen for the frame
    I32_WMV m_iMixedPelMV;          // 0 -> True MV, 1 -> Mixed
    I32_WMV m_iFrameXformMode;      // Transform mode (8x8, 8x4 or 4x8) for entire frame
    I32_WMV m_iOffsetToTopMB;       // m_uintNumMBX or -m_uintNumMBX to cycle 2 rows of MB data
       
    // Block layer buffers
    U8_WMV m_riReconBuf[64*4+32];   // IDCT coeff recon buffer. Allow enough space for 32-byte alignment
    U8_WMV m_riPixelError[64*4+32]; // Pixel error buffer. Allow enough space for 32-byte alignment
    PixelI32 __huge* m_rgiCoefReconPlus1;
    PixelI32 __huge* m_rgiCoefRecon;   
    Buffer __huge* m_ppxliErrorQ;
    Buffer __huge* m_rgiCoefReconBuf;

    // zigzag scan
    Bool_WMV m_bRotatedIdct;
    U8_WMV* m_pZigzagInv_I;
    U8_WMV* m_pHorizontalZigzagInv;
    U8_WMV* m_pVerticalZigzagInv;
    U8_WMV* m_pZigzagInvRotated_I;
    U8_WMV* m_pHorizontalZigzagInvRotated;
    U8_WMV* m_pVerticalZigzagInvRotated;
    U8_WMV* m_pZigzagInvRotated;
    U8_WMV* m_pZigzagInv;
    //zigzag of _XFORMSWITCHING_
    U8_WMV* m_pZigzagScanOrder;
    U8_WMV* m_p8x4ZigzagInv;
    U8_WMV* m_p4x8ZigzagInv;
    U8_WMV* m_p8x4ZigzagInvRotated;
    U8_WMV* m_p4x8ZigzagInvRotated;
    U8_WMV* m_p8x4ZigzagScanOrder;
    U8_WMV* m_p4x8ZigzagScanOrder;

    // for DCT AC pred. 
    I32_WMV m_iNumOfQuanDctCoefForACPredPerRow;
    I16_WMV* m_rgiQuanCoefACPred;
    I16_WMV** m_rgiQuanCoefACPredTable;
    I16_WMV* m_pAvgQuanDctCoefDec;
    I16_WMV* m_pAvgQuanDctCoefDecC;
    I16_WMV* m_pAvgQuanDctCoefDecLeft;
    I16_WMV* m_pAvgQuanDctCoefDecTop;

    // Misc
    U32_WMV m_iEscRunDiffV2V3; // Correct EscRunCoding error in MP42
    I32_WMV m_iDCPredCorrect;   // Correct DC Pred error in MP42
    I32_WMV m_iDCTHorzFlags;    // Sparse IDCT flag
    Bool_WMV m_bFirstEscCodeInFrame;                    //WMV1 vlc EscCoding
    I32_WMV m_iNUMBITS_ESC_LEVEL, m_iNUMBITS_ESC_RUN;   //WMV1 vlc EscCoding
    I32_WMV sm_iIDCTDecCount;    // idct resource counter
    U32_WMV m_uiNumProcessors;  // processor #

//    Bool_WMV* m_pbSkipMB;

    // for X8 : nonflat quantization
    I32_WMV  m_iNonflatQuant;
    t_SpatialPredictor  *m_pSp;
    t_ContextWMV        *m_pContext;
    t_AltTablesDecoder  *m_pAltTables;

    // output BMP info
    Bool_WMV m_bBMPInitialized;
    U32_WMV m_uiFOURCCOutput;
    U16_WMV m_uiBitsPerPixelOutput;
    Bool_WMV m_bRefreshBMP;

//I32_WMV m_iPostProcessMode;
    Bool_WMV m_bYUVDstBMP;
    U32_WMV m_uiRedscale, m_uiGreenscale;
    U32_WMV m_uiRedmask, m_uiGreenmask;
    U8_WMV m_rgDitherMap [4] [4] [3] [256];
    I32_WMV* m_piYscale;
    I32_WMV* m_piVtoR;
    I32_WMV* m_piUtoG;
    I32_WMV* m_piVtoG;
    I32_WMV* m_piUtoB;
    I32_WMV m_iWidthBMP;
    I32_WMV m_iBMPPointerStart; 
    I32_WMV m_iBMPMBIncrement, m_iBMPBlkIncrement;
    I32_WMV m_iBMPMBHeightIncrement, m_iBMPBlkHeightIncrement;
    // bitmap conversion
    U8_WMV* m_pBMPBits;
    I32_WMV m_iWidthPrevYTimes8Minus8, m_iWidthPrevUVTimes4Minus4; 


    // Huffman_WMV Tables
	U8_WMV* m_puXMvFromIndex;
	U8_WMV* m_puYMvFromIndex;
	Huffman_WMV* m_pHufMVDec;
    Huffman_WMV m_hufMVDec_Talking;
    Huffman_WMV m_hufMVDec_HghMt;

	Huffman_WMV* m_pHufMVDec_Set[2];
	U8_WMV* m_puMvFromIndex_Set[4];
    U32_WMV m_iMVTable;

//I32_WMV m_iPostProcessMode;

    // IntraDC
	//def _DCTDC
    Huffman_WMV m_hufDCTDCyDec_Talking;
    Huffman_WMV m_hufDCTDCcDec_Talking;
    Huffman_WMV m_hufDCTDCyDec_HghMt;
    Huffman_WMV m_hufDCTDCcDec_HghMt;

    U32_WMV m_iIntraDCTDCTable;
    Huffman_WMV* m_pHufDCTDCyDec;
    Huffman_WMV* m_pHufDCTDCcDec;
	Huffman_WMV* m_pHufDCTDCDec_Set[4];
    
    //NEW_PCBPCY_TABLE
    Huffman_WMV m_hufICBPCYDec, m_hufPCBPCYDec;
	Huffman_WMV* m_pHufNewPCBPCYDec;
	Huffman_WMV m_hufPCBPCYDec_HighRate; 
	Huffman_WMV m_hufPCBPCYDec_MidRate;
	Huffman_WMV m_hufPCBPCYDec_LowRate;
	//_DCTAC
	// For Table switching
    Huffman_WMV m_hufDCTACInterDec_HghMt;
    Huffman_WMV m_hufDCTACIntraDec_HghMt;
    Huffman_WMV m_hufDCTACInterDec_Talking;
    Huffman_WMV m_hufDCTACIntraDec_Talking;
    Huffman_WMV m_hufDCTACInterDec_MPEG4;
    Huffman_WMV m_hufDCTACIntraDec_MPEG4;

	CDCTTableInfo_Dec InterDCTTableInfo_Dec_HghMt;
	CDCTTableInfo_Dec IntraDCTTableInfo_Dec_HghMt;
	CDCTTableInfo_Dec InterDCTTableInfo_Dec_Talking;
	CDCTTableInfo_Dec IntraDCTTableInfo_Dec_Talking;
	CDCTTableInfo_Dec InterDCTTableInfo_Dec_MPEG4;
	CDCTTableInfo_Dec IntraDCTTableInfo_Dec_MPEG4;

	CDCTTableInfo_Dec* m_pInterDCTTableInfo_Dec;
	CDCTTableInfo_Dec* m_pIntraDCTTableInfo_Dec;
	CDCTTableInfo_Dec* m_pInterDCTTableInfo_Dec_Set[3];
	CDCTTableInfo_Dec* m_pIntraDCTTableInfo_Dec_Set[3];
    U32_WMV m_iDCTACInterTableIndx;
    U32_WMV m_iDCTACIntraTableIndx;

//I32_WMV m_iPostProcessMode;
    // function pointers
    Void_WMV (*m_pWMVideoDecUpdateDstMB) (
        struct tagWMVDecInternalMember *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidth
    );
    Void_WMV (*m_pWMVideoDecUpdateDstPartialMB) (
        struct tagWMVDecInternalMember *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iRgbWidth, I32_WMV iRgbHeight
    );

    Void_WMV (*m_pWMVideoDecUpdateDstBlk) (
        struct tagWMVDecInternalMember *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidth
    );

    tWMVDecodeStatus (*m_pDecodeI) (struct tagWMVDecInternalMember *pWMVDec);
    tWMVDecodeStatus (*m_pDecodeP) (struct tagWMVDecInternalMember *pWMVDec);
    tWMVDecodeStatus (*m_pDecodeIMBAcPred) (
        struct tagWMVDecInternalMember *pWMVDec,
        CWMVMBMode*            pmbmd, 
        U8_WMV*      ppxliTextureQMBY,
        U8_WMV*      ppxliTextureQMBU, 
        U8_WMV*      ppxliTextureQMBV, 
        I16_WMV*                piQuanCoefACPred, 
        I16_WMV**                piQuanCoefACPredTable, 
        Bool_WMV                bLeftMB, 
        Bool_WMV                bTopMB, 
        Bool_WMV                bLeftTopMB);
     tWMVDecodeStatus (*m_pDecodePMB) (
        struct tagWMVDecInternalMember *pWMVDec,
        CWMVMBMode*        pmbmd,
	    U8_WMV* ppxlcCurrQYMB,
	    U8_WMV* ppxlcCurrQUMB,
	    U8_WMV* ppxlcCurrQVMB,
	    CoordI xRefY, CoordI yRefY,
	    CoordI xRefUV, CoordI yRefUV
    );

    // Decoder MB Header
    tWMVDecodeStatus (* m_pDecodeMBOverheadOfIVOP) (struct tagWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);
    tWMVDecodeStatus (* m_pDecodeMBOverheadOfPVOP) (struct tagWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd);

    //block decoding fuctions
    tWMVDecodeStatus (*m_pDecodeInverseInterBlockQuantize) (struct tagWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV* pZigzag, I32_WMV iXformMode);

//I32_WMV m_iPostProcessMode;
    // IDCT routines
    Void_WMV (*m_pIntraIDCT_Dec) (U8_WMV* piDst, I32_WMV iOffsetToNextRowForDCT, const PixelI32 __huge* rgiCoefRecon);
    Void_WMV (*m_pInterIDCT_Dec) (Buffer __huge* piDst, Buffer __huge* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
    Void_WMV (*m_pInter8x4IDCT_Dec) (Buffer __huge* piDst, I32_WMV iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefRecon, I32_WMV iHalf);
    Void_WMV (*m_pInter4x8IDCT_Dec) (Buffer __huge* piDst, I32_WMV iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefRecon, I32_WMV iHalf);
    // Motion Comp
    Void_WMV (*m_pMotionCompAndAddError) (MOTION_COMP_ADD_ERROR_ARGS);
    Void_WMV (*m_pMotionComp) (MOTION_COMP_ARGS);
    Void_WMV (*m_pMotionCompAndAddErrorRndCtrlOn) (MOTION_COMP_ADD_ERROR_ARGS);
    Void_WMV (*m_pMotionCompRndCtrlOn) (MOTION_COMP_ARGS);
    Void_WMV (*m_pMotionCompAndAddErrorRndCtrlOff) (MOTION_COMP_ADD_ERROR_ARGS);
    Void_WMV (*m_pMotionCompRndCtrlOff) (MOTION_COMP_ARGS);
    Void_WMV (*m_pMotionCompZero) (MOTION_COMP_ZERO_ARGS);
    Void_WMV (*m_pMotionCompUV) (MOTION_COMP_ARGS);
    Void_WMV (*m_pMotionCompAndAddErrorUV) (MOTION_COMP_ADD_ERROR_ARGS);
    Void_WMV (*m_pMotionCompMixed) (MOTION_COMP_ARGS);
    Void_WMV (*m_pMotionCompMixedAndAddError) (MOTION_COMP_ADD_ERROR_ARGS);

//I32_WMV m_iPostProcessMode;

	// Deblocking filter
    Void_WMV (*m_pFilterHorizontalEdge)(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
    Void_WMV (*m_pFilterVerticalEdge)(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
    // IMBInPFrame DC Pred
    I32_WMV (*m_pBlkAvgX8_MMX) (const U8_WMV* ppxlcCurrRecnMB, I32_WMV iWidthPrev, I32_WMV iStepSize);

//I32_WMV m_iPostProcessMode;

// MPEG4-compliant variables
    U32_WMV uiFCode;
    I32_WMV iRange;
    I32_WMV iScaleFactor;
    I32_WMV m_iClockRate;

    Time m_t; // current time
    I32_WMV m_iVPMBnum;
    Time m_tModuloBaseDecd;
    Time m_tModuloBaseDisp;
    Time m_tOldModuloBaseDecd;
    Time m_tOldModuloBaseDisp;
    I32_WMV m_iNumBitsTimeIncr;
    Bool_WMV m_bResyncDisable;
    // For MPG4
#ifdef _MPG4_
    I32_WMV m_rgiDCRef [3]; // intra DC predictor
#endif

#ifndef _SLIM_C_
    // Multithread event for clearing frame
    // for multi-thread handles
    Bool_WMV m_bRenderFromPostBuffer;
    Bool_WMV m_bDisplay_AllMB;
    HANDLE hDecEvent0, hDecDone0;
    HANDLE hDecEvent1, hDecDone1;
    HANDLE threadDec0, threadDec1; //, threadDec2, threadDec3;
    Bool_WMV m_bShutdownThreads;
    THREADTYPE_DEC m_iMultiThreadType;
    I32_WMV m_iCurrQYOffset1, m_iCurrQUVOffset1;
    //Move to vo.hpp
    U32_WMV m_uiMBStart0, m_uiMBEnd0, m_uiMBEndX0; 
	CoordI m_iStartY0, m_iEndY0;
	CoordI m_iStartUV0, m_iEndUV0;
	I32_WMV m_iOffsetPrevY0, m_iOffsetPrevUV0;
    U32_WMV m_uiMBStart1, m_uiMBEnd1, m_uiMBEndX1; 
	CoordI m_iStartY1, m_iEndY1;
	CoordI m_iStartUV1, m_iEndUV1;
	I32_WMV m_iOffsetPrevY1, m_iOffsetPrevUV1;
#endif
    //Color Conv, Deblocking, Color Contrl, Possibly removable
//I32_WMV m_iPostProcessMode;

    Bool_WMV m_bDeblockOn;
    Bool_WMV m_bDeringOn; // deblock must be on for dering to be on
    I32_WMV m_bUseOldSetting;
    Bool_WMV m_bRefreshDisplay_AllMB_Enable;
    Bool_WMV m_bRefreshDisplay_AllMB;
    I32_WMV m_iRefreshDisplay_AllMB_Cnt;

    I32_WMV m_iRefreshDisplay_AllMB_Period;
    Bool_WMV m_bCopySkipMBToPostBuf;
    Bool_WMV m_bDefaultColorSetting;
    Bool_WMV m_bCPUQmoved;
    I32_WMV m_iPostProcessMode;

#ifdef _EMB_WMV2_

    U32_WMV m_iMotionCompRndCtrl;
    EMB_DecodeP_ShortCut m_EMBDecodePShortCut;
    tWMVDecodeStatus (*m_CoefDecoderAndIdctDecTable[4])(struct tagWMVDecInternalMember * pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec, I32_WMV iXFormMode);
    tWMVDecodeStatus (*m_pCoefDecoderAndIdctDec8x4or4x8Table[8])(struct tagWMVDecInternalMember * pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec);
    Void_WMV (*m_MotionCompAndAddErrorTable[16])(U8_WMV * ppxlcCurrQMB, const U8_WMV * ppxlcRef, I32_WMV iWidthFrm, I32_WMV * pErrorBuf);

#endif

#ifdef DYNAMIC_EDGEPAD
    tWMVEdgePad m_tEdgePad;
#endif

#ifdef _6BIT_COLORCONV_OPT_
    YUV2RGB_6BIT_TABLE *p_yuv2rgb_6bit_table;
    U8_WMV * m_rgiClapTab6BitDec;
#endif
} tWMVDecInternalMember;
#include "dectable_c.h"

/*
// basic structs
typedef enum tagFrameType_WMV 
{
    IFRAME_WMV = 0, 
    PFRAME_WMV
} tFrameType_WMV;

typedef struct tagYUV420Frame_WMV
{
    U8_WMV* m_pucYPlane;
    U8_WMV* m_pucUPlane;
    U8_WMV* m_pucVPlane;
} tYUV420Frame_WMV;

typedef struct tagMotionVector_WMV
{
    I16_WMV m_iMVX;
    I16_WMV m_iMVY;
} tMotionVector_WMV;

// ===========================================================================
// private data members
// ===========================================================================

typedef struct tagWMVDecInternalMember 
{
    tHuffmanDecoder_WMV* m_pHuffDec;
    tStreamParser_WMV* m_pStrmParse;
    U32_WMV m_uiFOURCCCompressed;
    Float_WMV m_fltFrameRate;
    Float_WMV m_fltBitRate;
    U16_WMV m_uiPostFilterLevel;

    // size info
    I32_WMV m_iWidthSource; // source width, might not be MB aligned
    I32_WMV m_iHeightSource; // source height, might not be MB aligned
    I32_WMV m_iWidthInternal; // internal width, MB aligned
    I32_WMV m_iWidthInternalTimesMB;
    I32_WMV m_iHeightInternal;
    I32_WMV m_iWidthInternalUV;
    I32_WMV m_iWidthInternalUVTimesBlk;
    I32_WMV m_iHeightInternalUV;
    Bool_WMV m_bSizeMBAligned;
    U32_WMV m_uiNumMB, m_uiNumMBX, m_uiNumMBY;

    // tables
    const U8_WMV* m_rgiClapTabDec;

    // pictures/frames data
    tYUV420Frame_WMV* m_pfrCurr;
    tYUV420Frame_WMV* m_pfrPrev;

    // sequence layer info
    Bool_WMV m_bIFrameDecoded;
    Bool_WMV m_bXformSwitch;
    Bool_WMV m_bXintra8Switch;
    Bool_WMV m_bRndCtrlOn;
    Bool_WMV m_iSliceCode;

    // picture layer info.
    tFrameType_WMV m_tFrmType;
    Bool_WMV m_bXintra8;
    Bool_WMV m_bMixedPel;
    Bool_WMV m_bLoopFilter;
    Bool_WMV m_bFrmHybridMVOn;
    Bool_WMV m_bDCTTable_MB_ENABLED;
    I32_WMV m_iStepSize;

    // MB layer info
    Bool_WMV* m_pbSkipMB;

    // output BMP info
    Bool_WMV m_bBMPInitialized;
    U32_WMV m_uiFOURCCOutput;
    U16_WMV m_uiBitsPerPixelOutput;
    Bool_WMV m_bRefreshBMP;

    Bool_WMV m_bYUVDstBMP;
    U32_WMV m_uiRedscale, m_uiGreenscale;
    U32_WMV m_uiRedmask, m_uiGreenmask;
    U8_WMV m_rgDitherMap [4] [4] [3] [256];
    I32_WMV* m_piYscale;
    I32_WMV* m_piVtoR;
    I32_WMV* m_piUtoG;
    I32_WMV* m_piVtoG;
    I32_WMV* m_piUtoB;
    I32_WMV m_iWidthBMP;
    I32_WMV m_iBMPPointerStart; 
    I32_WMV m_iBMPMBIncrement, m_iBMPBlkIncrement;
    I32_WMV m_iBMPMBHeightIncrement, m_iBMPBlkHeightIncrement;

    // function pointers
    Void_WMV_WMV (*m_pWMVideoDecUpdateDstMB) (
        Void_WMV_WMV *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidth
    );
    Void_WMV_WMV (*m_pWMVideoDecUpdateDstPartialMB) (
        Void_WMV_WMV *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iRgbWidth, I32_WMV iRgbHeight
    );

    Void_WMV_WMV (*m_pWMVideoDecUpdateDstBlk) (
        Void_WMV_WMV *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidth
    );


} tWMVDecInternalMember;
*/
#endif // __WMVDEC_MEMBER_H_
