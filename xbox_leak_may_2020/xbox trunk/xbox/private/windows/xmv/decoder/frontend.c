/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       frontend.c
 *  Content:    parses the on-disk format to YUV macroblocks.
 *
 ****************************************************************************/

#include <xtl.h>
#include <xdbg.h>
#include <xmv.h>

#include "decoder.h"

/* 
 * Converts the unquantized AC coefficient array for a block into a block of
 * pixels.  
 */

// !!! andrewso: We can probably improve this a bit as we always know we
//   have SSE1 available.
//
void InverseDCT
(
    BYTE *pDestination, 
    DWORD Pitch,
    long *ReconstructedACCoefficients
)
{
  	long buf [72];
    long *rgblk = (long *)(((long)buf+32) & 0xFFFFFFE0);

    static const LONGLONG M1 = 0x023508E4023508E4;     //W7,W1-W7
    static const LONGLONG M2 = 0x0235F2B20235F2B2;     //W7,0-(W1+W7)
    static const LONGLONG M3 = 0x0968FCE10968FCE1;     //W3,0-(W3-W5)
    static const LONGLONG M4 = 0x0968F04F0968F04F;     //W3,0-(W3+W5)
    static const LONGLONG M5 = 0x0454F1380454F138;     //W6,0-(W2+W6)
    static const LONGLONG M6 = 0x0454062004540620;     //W6,W2-W6
    static const LONGLONG M7 = 0x0000008000000080;
    static const LONGLONG M8 = 0x00B55a8000B55a80;     //181,(181<<16)
    static const LONGLONG M9 = 0x0000023500000235;
    static const LONGLONG M10 = 0x000000B5000000B5;    //181,181
    static const LONGLONG low15mask = 0x00007fff00007fff;
    LONGLONG t64;

    _asm
    {
            mov         esi,ReconstructedACCoefficients
            mov         edi,rgblk
            mov         ecx,-4
            xor         ebx,ebx
HLoop:
            lea         eax,[esi+ecx*8]

            pcmpeqb     mm0,mm0
            psrld       mm0,16

            movq        mm4,[eax+32+16*2]
            movq        mm1,mm4
            movq        mm5,[eax+32+112*2]
            paddd       mm1,mm5
            pslld       mm1,16
            pand        mm4,mm0
            pand        mm5,mm0
            por         mm5,mm1
            por         mm4,mm1
            movq        mm1,M1
            pmaddwd     mm4,mm1
            movq        mm1,M2
            pmaddwd     mm5,mm1
            
            movq        mm6,[eax+32+80*2]
            movq        mm1,mm6
            movq        mm7,[eax+32+48*2]
            paddd       mm1,mm7
            pslld       mm1,16
            pand        mm6,mm0
            pand        mm7,mm0
            por         mm6,mm1
            por         mm7,mm1
            movq        mm1,M3
            pmaddwd     mm6,mm1
            movq        mm1,M4
            pmaddwd     mm7,mm1

            movq        mm3,[eax+32+32*2]
            movq        mm1,mm3
            movq        mm2,[eax+32+96*2]
            paddd       mm1,mm2
            pslld       mm1,16
            pand        mm3,mm0
            pand        mm2,mm0
            por         mm3,mm1
            por         mm2,mm1
            movq        mm1,M6
            pmaddwd     mm3,mm1
            movq        mm1,M5
            pmaddwd     mm2,mm1

            movq        mm0,mm6
            paddd       mm6,mm4
            psubd       mm4,mm0

            movq        mm0,mm7
            paddd       mm7,mm5
            psubd       mm5,mm0

            movq        mm0,mm5
            paddd       mm5,mm4
            psubd       mm4,mm0

            movq        mm0,mm5
            pand        mm0,low15mask
            movq        mm1,M10
            pmaddwd     mm0,mm1
            psrld       mm5,15
            pmaddwd     mm5,mm1
            pslld       mm5,15
            paddd       mm5,mm0
            movq        mm0,M7
            paddd       mm5,mm0
            psrad       mm5,8

            movq        mm0,mm4
            pand        mm0,low15mask
            pmaddwd     mm0,mm1
            psrld       mm4,15
            pmaddwd     mm4,mm1
            pslld       mm4,15
            paddd       mm4,mm0
            movq        mm1,M7
            paddd       mm4,mm1
            psrad       mm4,8

            movq        mm0,[esi+ecx*8+32]
            lea         eax,[esi+ecx*8]
            movq        [edi+4],mm4
            pslld       mm0,16
            psrad       mm0,5
            movq        mm1,M7
            paddd       mm0,mm1
            movq        mm1,[eax+32+64*2]
            pslld       mm1,16
            psrad       mm1,5
            movq        mm4,mm0
            psubd       mm0,mm1
            paddd       mm1,mm4

            movq        mm4,mm1
            psubd       mm1,mm3
            paddd       mm3,mm4

            movq        mm4,mm0
            psubd       mm0,mm2
            paddd       mm2,mm4

            movq        mm4,mm3
            psubd       mm3,mm6
            paddd       mm6,mm4
            psrad       mm3,8
            psrad       mm6,8

#if 0 // Old way of generating (181*(x4 + x5) + 128)>>8. Overflowed in certain cases
            
            movq        mm4,mm5
            pslld       mm4,17
            psrlw       mm4,1
            pslld       mm5,1
            psrld       mm5,16
            psllw       mm5,8
            por         mm5,mm4
            movq        mm4,M8
            pmaddwd     mm5,mm4
            movq        mm4,M7
            paddd       mm5,mm4
            psrad       mm5,8
#endif

            movq        mm4,mm2
            psubd       mm2,mm5
            paddd       mm5,mm4
            psrad       mm2,8
            psrad       mm5,8

            movq        mm4,mm6
            punpckldq   mm6,mm5
            punpckhdq   mm4,mm5
            packssdw    mm6,mm6
            
            movq        mm5,mm2
            punpckldq   mm2,mm3
            movd        [edi],mm6
            punpckhdq   mm5,mm3
            packssdw    mm5,mm5
            movd        [edi+12+16],mm5
            packssdw    mm2,mm4
            movq        mm4,[edi+4]
            movq        [edi+12],mm2

#if 0 // Old way of generating (181*(x4 + x5) + 128)>>8. Overflowed in certain cases

            movq        mm2,mm4
            pslld       mm4,17
            psrlw       mm4,1
            pslld       mm2,1
            psrld       mm2,16
            psllw       mm2,8
            por         mm2,mm4
            movq        mm4,M8
            pmaddwd     mm4,mm2
            movq        mm2,M7
            paddd       mm4,mm2
            psrad       mm4,8
#endif
            
            movq        mm2,mm0
            psubd       mm0,mm4
            paddd       mm4,mm2
            psrad       mm0,8
            psrad       mm4,8

            movq        mm2,mm1
            psubd       mm1,mm7
            paddd       mm7,mm2
            psrad       mm1,8
            psrad       mm7,8

            movq        mm2,mm4
            punpckldq   mm4,mm7
            punpckhdq   mm2,mm7

            movq        mm7,mm1
            punpckldq   mm1,mm0
            punpckhdq   mm7,mm0
            packssdw    mm4,mm1
            movq        [edi+4],mm4
            packssdw    mm2,mm7
            inc         ecx
            movq        [edi+4+16],mm2
            lea         edi,[edi+32]
            jl          HLoop

            xor         ecx,ecx
            mov         esi,rgblk
            mov         edi,pDestination
            mov         edx,Pitch
VLoop:

            pcmpeqb     mm1,mm1    //Generate constant 4
            psrld       mm1,31
            pslld       mm1,2

            movd        mm4,[esi+ecx*2+16]
            movd        mm5,[esi+ecx*2+112]
            movq        mm0,mm4
            paddw       mm0,mm5
            punpcklwd   mm4,mm0
            punpcklwd   mm5,mm0
            movq        mm0,M1
            pmaddwd     mm4,mm0
            paddd       mm4,mm1
            movq        mm0,M2
            pmaddwd     mm5,mm0
            paddd       mm5,mm1
            psrad       mm4,3
            psrad       mm5,3
        
            movd        mm6,[esi+ecx*2+80]
            movd        mm7,[esi+ecx*2+48]
            movq        mm0,mm6
            paddw       mm0,mm7
            punpcklwd   mm6,mm0
            punpcklwd   mm7,mm0
            movq        mm0,M3
            pmaddwd     mm6,mm0
            paddd       mm6,mm1
            movq        mm0,M4
            pmaddwd     mm7,mm0
            paddd       mm7,mm1
            psrad       mm6,3
            psrad       mm7,3

            movd        mm2,[esi+ecx*2+96]
            movd        mm3,[esi+ecx*2+32]
            movq        mm0,mm2
            paddw       mm0,mm3
            punpcklwd   mm2,mm0
            punpcklwd   mm3,mm0
            movq        mm0,M5
            pmaddwd     mm2,mm0
            paddd       mm2,mm1
            movq        mm0,M6
            pmaddwd     mm3,mm0
            paddd       mm3,mm1
            psrad       mm2,3
            psrad       mm3,3

            movq        mm0,mm4
            psubd       mm4,mm6
            paddd       mm6,mm0

            movq        mm0,mm5
            psubd       mm5,mm7
            paddd       mm7,mm0

            movq        mm0,mm4
            psubd       mm4,mm5
            paddd       mm5,mm0
 
            movq        mm0,mm5
            pand        mm0,low15mask
            movq        mm1,M10
            pmaddwd     mm0,mm1
            psrld       mm5,15
            pmaddwd     mm5,mm1
            pslld       mm5,15
            paddd       mm5,mm0
            movq        mm0,M7
            paddd       mm5,mm0
            psrad       mm5,8            

            movq        mm0,mm4
            pand        mm0,low15mask
            pmaddwd     mm0,mm1
            psrld       mm4,15
            pmaddwd     mm4,mm1
            pslld       mm4,15
            paddd       mm4,mm0
            movq        mm1,M7
            paddd       mm4,mm1
            psrad       mm4,8

#if 0 // Old way of generating (181*(x4 + x5) + 128)>>8. Overflowed in certain cases
            
            pslld       mm1,5  //Generate constant 128
            movq        mm0,mm5
            pslld       mm0,17
            psrlw       mm0,1
            pslld       mm5,1
            psrld       mm5,16
            psllw       mm5,8
            por         mm5,mm0
            movq        mm0,M8
            pmaddwd     mm5,mm0
            movq        mm0,mm4
            paddd       mm5,mm1
            psrad       mm5,8

            pslld       mm0,17
            psrlw       mm0,1
            pslld       mm4,1
            psrld       mm4,16
            psllw       mm4,8
            por         mm4,mm0
            movq        mm0,M8
            pmaddwd     mm4,mm0
            paddd       mm4,mm1
            psrad       mm4,8
#endif

            movq        t64,mm4

            pslld       mm1,6   //Generate 8192

            movd        mm0,[esi+ecx*2]
            punpcklwd   mm0,mm0
            pslld       mm0,16
            psrad       mm0,8
            paddd       mm0,mm1
            movd        mm4,[esi+ecx*2+64]
            punpcklwd   mm4,mm4
            pslld       mm4,16
            psrad       mm4,8
            movq        mm1,mm0
            psubd       mm0,mm4
            paddd       mm4,mm1

            movq        mm1,mm4
            psubd       mm4,mm3
            paddd       mm3,mm1

            movq        mm1,mm0
            psubd       mm0,mm2
            paddd       mm2,mm1

            movq        mm1,mm3
            psubd       mm3,mm6
            psrad       mm3,14
            paddd       mm6,mm1
            psrad       mm6,14

            packssdw    mm6,mm6
            packuswb    mm6,mm6
            movd        eax,mm6
            mov         [edi],ax

            lea         ebx,[edi+edx*8]
            sub         ebx,edx
            packssdw    mm3,mm3
            packuswb    mm3,mm3
            movd        eax,mm3
            mov         [ebx],ax

            movq        mm6,t64

            movq        mm1,mm4
            psubd       mm4,mm7
            psrad       mm4,14
            paddd       mm7,mm1
            psrad       mm7,14

            packssdw    mm4,mm4
            packuswb    mm4,mm4
            movd        eax,mm4
            mov         [edi+edx*4],ax
            lea         ebx,[edi+edx*2]
            packssdw    mm7,mm7
            packuswb    mm7,mm7
            movd        eax,mm7
            mov         [ebx+edx],ax

            movq        mm1,mm2
            psubd       mm2,mm5
            psrad       mm2,14
            paddd       mm5,mm1
            psrad       mm5,14

            lea         ebx,[ebx+edx*2]
            packssdw    mm2,mm2
            packuswb    mm2,mm2
            movd        eax,mm2
            mov         [ebx+edx*2],ax
            
            packssdw    mm5,mm5
            packuswb    mm5,mm5
            movd        eax,mm5
            mov         [edi+edx],ax
  
            movq        mm3,mm0
            psubd       mm0,mm6
            psrad       mm0,14
            paddd       mm6,mm3
            psrad       mm6,14
            packssdw    mm0,mm0
            packuswb    mm0,mm0
            movd        eax,mm0
            mov         [ebx+edx],ax
            packssdw    mm6,mm6
            packuswb    mm6,mm6
            movd        eax,mm6
            mov         [edi+edx*2],ax

            add         edi,2
            add         ecx,2
            cmp         ecx,8
            jnz         VLoop
            emms
        }    
}

/*
 * Decodes a block for a baseline I-frame.  The Top.... and Left... AC
 * Coefficient arrays will get overwritten with the values from this
 * 
 */

void DecodeBaselineIFrameBlock
(
    XMVDecoder *pDecoder,
    BYTE *pDestination,
    DWORD Pitch,
    DWORD DCStepSize,
    DWORD PictureQuantizerScale,
    BOOL  HasEncodedACCoefficientInformation,
    BOOL ACPredictionEnabled,
    short TopLeftQuantizedDCCoefficient,
    short *TopQuantizedACCoefficients,
    short *LeftQuantizedACCoefficients, 
    WORD *pDCDifferentialTable,
    DWORD DCDifferentialEscapeCode,
    XMVACCoefficientDecoderTable *pACDecoderTable,
    DWORD *pEscape3RunLength,
    DWORD *pEscape3LevelLength
)
{
    short DCPredictor;
    short DCDifferential;
    short QuantizedDCCoefficient;
    BOOL  IsPredictedLeft;
    DWORD DCMagnitude;
    long  ReconstructedACCoefficients[64];
    BYTE *pZigZag;
    long  QuantizedACCoefficient;
    DWORD DoubleQuantizerScale;
    DWORD QuantizerScaleOdd;

    DWORD CoefficientCounter;
    DWORD Index;
    BOOL  Done;
    DWORD Run;
    long  Level;
    DWORD Sign;
    DWORD CoefficientIndex;

    DWORD i, Bit;

    //
    // Calculate the DC predictor for this block.
    //

    if (abs(TopLeftQuantizedDCCoefficient - TopQuantizedACCoefficients[0]) <= abs(TopLeftQuantizedDCCoefficient - LeftQuantizedACCoefficients[0]))
    {
        DCPredictor = LeftQuantizedACCoefficients[0];
        IsPredictedLeft = TRUE;
    }
    else 
    {
        DCPredictor = TopQuantizedACCoefficients[0];
        IsPredictedLeft = FALSE;
    }

    //
    // Decode the DC differential.
    //

    // Read the table entry.
    DCMagnitude = HuffmanDecode(pDecoder, pDCDifferentialTable);

    // Escaped values are encoded directly as an 8-bit value.
    if (DCMagnitude == DCDifferentialEscapeCode)
    {
        DCMagnitude = ReadBits(pDecoder, 8);

        ASSERT(DCMagnitude != 0);
    }

    // Read the sign bit if necessary.
    if (DCMagnitude && ReadOneBit(pDecoder))
    {
        DCDifferential = - (short)DCMagnitude;
    }
    else
    {
        DCDifferential = (short)DCMagnitude;
    }

    //
    // Set the quantized and non-quantized DC coefficient.
    //

    QuantizedDCCoefficient = DCPredictor + DCDifferential;

    TopQuantizedACCoefficients[0]  = QuantizedDCCoefficient;
    LeftQuantizedACCoefficients[0] = QuantizedDCCoefficient;

    // Set up the AC Coefficient block.
    ReconstructedACCoefficients[0] = QuantizedDCCoefficient * DCStepSize;

    memset(ReconstructedACCoefficients + 1, 0, sizeof(ReconstructedACCoefficients) - sizeof(ReconstructedACCoefficients[0]));

    //
    // Initialize the arrays for the quantized AC coefficients.  We need to
    // keep these around as other blocks may want to use these values to do AC
    // coefficient prediction.
    //

    if (ACPredictionEnabled)
    {
        // The mmx routine we use to do the DCT inversion assumes that the
        // AC coefficients are rotated...this forces us to use the vertical
        // opposite predicted arrays...
        //
        if (IsPredictedLeft)
        {
            memset(TopQuantizedACCoefficients + 1, 0, sizeof(TopQuantizedACCoefficients[1]) * 7);
            pZigZag = g_VerticalZigzag;
        }
        else
        {
            memset(LeftQuantizedACCoefficients + 1,  0, sizeof(LeftQuantizedACCoefficients[1]) * 7);
            pZigZag = g_HorizontalZigzag;
        }
    }
    else
    {
        memset(TopQuantizedACCoefficients + 1,  0, sizeof(TopQuantizedACCoefficients[1]) * 7);
        memset(LeftQuantizedACCoefficients + 1, 0, sizeof(TopQuantizedACCoefficients[1]) * 7);

        pZigZag = g_NormalZigzag;
    }

    //
    // Read in the AC coefficients if this block has any.   This will inverse-
    // zigzag and invert-quantize the input at the same time except for the
    // ACs that are in either the first row or the first column.
    //

    DoubleQuantizerScale = PictureQuantizerScale * 2;
    QuantizerScaleOdd = (PictureQuantizerScale & 1) ? PictureQuantizerScale : PictureQuantizerScale - 1;

    if (HasEncodedACCoefficientInformation)
    {
        CoefficientCounter = 1;

        do
        {
            Index = HuffmanDecode(pDecoder, pACDecoderTable->pDTCACDecoderTable);
            ASSERT(Index <= pACDecoderTable->DCTACDecoderEscapeCode);

            // Normal case.
            if (Index != pACDecoderTable->DCTACDecoderEscapeCode)
            {
                Done = (Index >= pACDecoderTable->StartIndexOfLastRun);

                Run   = pACDecoderTable->RunTable[Index];
                Level = pACDecoderTable->LevelTable[Index];

                if (ReadOneBit(pDecoder))
                {
                    Level = -Level;
                }
            }

            // ESC + 1
            else if (ReadOneBit(pDecoder))
            {
                Index = HuffmanDecode(pDecoder, pACDecoderTable->pDTCACDecoderTable);

                Done = (Index >= pACDecoderTable->StartIndexOfLastRun);

                Run   = pACDecoderTable->RunTable[Index];
                Level = pACDecoderTable->LevelTable[Index];

                if (!Done)
                {
                    Level = Level + pACDecoderTable->NotLastDeltaLevelTable[Run];
                }
                else
                {
                    Level = Level + pACDecoderTable->LastDeltaLevelTable[Run];
                }

                if (ReadOneBit(pDecoder))
                {
                    Level = -Level;
                }
            }

            // ESC + 01
            else if (ReadOneBit(pDecoder))
            {
                Index = HuffmanDecode(pDecoder, pACDecoderTable->pDTCACDecoderTable);

                Done = (Index >= pACDecoderTable->StartIndexOfLastRun);

                Run   = pACDecoderTable->RunTable[Index];
                Level = pACDecoderTable->LevelTable[Index];

                if (!Done)
                {
                    Run = Run + pACDecoderTable->NotLastDeltaRunTable[Level] + 1;
                }
                else
                {
                    Run = Run + pACDecoderTable->LastDeltaRunTable[Level] + 1;
                }

                if (ReadOneBit(pDecoder))
                {
                    Level = -Level;
                }
            }

            // ESC + 00
            else
            {
                Done = ReadOneBit(pDecoder);

                if (!*pEscape3RunLength)
                {
                    if (PictureQuantizerScale >= 8)
                    {
                        i = 0;
                        Bit = 0;

                        while (i < 6 && !Bit)
                        {
                            Bit = ReadOneBit(pDecoder);
                            i++;
                        }

                        if (Bit)
                        {
                            *pEscape3LevelLength = i + 1;
                        }
                        else
                        {
                            *pEscape3LevelLength = 8;
                        }
                    }
                    else
                    {
                        *pEscape3LevelLength = ReadBits(pDecoder, 3);

                        if (!*pEscape3LevelLength)
                        {
                            *pEscape3LevelLength = 8 + ReadOneBit(pDecoder);
                        }
                    }

                    *pEscape3RunLength = 3 + ReadBits(pDecoder, 2);
                }

                Run   = ReadBits(pDecoder, *pEscape3RunLength);   
                Sign  = ReadOneBit(pDecoder);
                Level = ReadBits(pDecoder, *pEscape3LevelLength);

                if (Sign)
                {
                    Level = -Level;
                }
            }

            // Zip past the run of zeros.
            CoefficientCounter += Run;

            // Prevent a potential buffer overrun.
            if (CoefficientCounter > 63)
            {
                RIP("Buffer overrun while decoding file, bad file.");

                CoefficientCounter = 63;
            }

            // De-zigzag the coefficient we just decoded.
            CoefficientIndex = pZigZag[CoefficientCounter];

            // If this AC coefficient is in either the first row or column
            // then we need to store it in the Top/Left arrays.
            //
            // Top
            if (CoefficientIndex / 8 == 0)
            {
                LeftQuantizedACCoefficients[CoefficientIndex] += (short)Level;
            }

            // Left
            else if (CoefficientIndex % 8 == 0)
            {
                TopQuantizedACCoefficients[CoefficientIndex / 8] += (short)Level;
            }

            // Other
            else if (Level > 0)
            {
                ReconstructedACCoefficients[CoefficientIndex] = DoubleQuantizerScale * Level + QuantizerScaleOdd;
            }

            else // Level < 0
            {
                ReconstructedACCoefficients[CoefficientIndex] = DoubleQuantizerScale * Level - QuantizerScaleOdd;
            }

            CoefficientCounter++;
        }
        while(!Done);
    }

    // 
    // Inverse Quantize and propigate the first row and column AC coefficients
    // to the unquantized coefficient matrix.  The other coefficients are done
    // as they are read in.
    //

    for (i = 1; i < 8; i++)
    {
        QuantizedACCoefficient = LeftQuantizedACCoefficients[i];

        if (QuantizedACCoefficient == 0)
        {
            ReconstructedACCoefficients[i] = 0;
        }
        else if (QuantizedACCoefficient > 0)
        {
            ReconstructedACCoefficients[i] = DoubleQuantizerScale * QuantizedACCoefficient + QuantizerScaleOdd;
        }
        else // QuantizedACCoefficient < 0
        {
            ReconstructedACCoefficients[i] = DoubleQuantizerScale * QuantizedACCoefficient - QuantizerScaleOdd;
        }

        QuantizedACCoefficient = TopQuantizedACCoefficients[i];

        if (QuantizedACCoefficient == 0)
        {
            ReconstructedACCoefficients[i * 8] = 0;
        }
        else if (QuantizedACCoefficient > 0)
        {
            ReconstructedACCoefficients[i * 8] = DoubleQuantizerScale * QuantizedACCoefficient + QuantizerScaleOdd;
        }
        else // QuantizedACCoefficient < 0
        {
            ReconstructedACCoefficients[i * 8] = DoubleQuantizerScale * QuantizedACCoefficient - QuantizerScaleOdd;
        }
    }
    
    // 
    // Convert the AC coefficients into a bitmap.
    //

    InverseDCT(pDestination, Pitch, ReconstructedACCoefficients);
}

/* 
 * Decodes the guts of a baseline I-frame
 */
 
#if DBG

DWORD g_IFrameCount;

#endif DBG

void DecodeBaselineIFrame
(
    XMVDecoder *pDecoder,
    DWORD PictureQuantizerScale
)
{
    DWORD MBWidth;
    DWORD MBHeight;

    DWORD DCStepSize;
    short DefaultDCPredictor;

    BYTE *pY;
    BYTE *pU;
    BYTE *pV;
    BYTE *pBlock;

    DWORD YPitch, UVPitch;

    DWORD Escape3RunLength;
    DWORD Escape3LevelLength;

    // Frame header.
    BOOL  MBLevelDCTACCoding;

    XMVACCoefficientDecoderTable *pDCTACInterCodingSetTable;
    XMVACCoefficientDecoderTable *pDCTACIntraCodingSetTable;

    // CBPCY Decoding.
    XMVMacroblockCBPCY CBPCYTopLeft, CBPCYTop;
    XMVMacroblockCBPCY *pCBPCYLine;

    DWORD DecodedCBPCY, DecodedY1, DecodedY2, DecodedY3, DecodedY4;

    BOOL ACPredictionEnabled;

    // Block decoding tables.
    WORD *pYDCDifferentialTable;
    WORD *pUVDCDifferentialTable;

    DWORD MBACCodingSetIndex;

    short YLeftACQuantizedCoefficients[2][BLOCK_SIZE];
    short YDCTopLeft[2], YDCTop;
    short YDCTopLeftForNextMacroblock;
    short *pYACLine;

    short ULeftACQuantizedCoefficients[BLOCK_SIZE];
    short UDCTopLeft, UDCTop;
    short *pUACLine;

    short VLeftACQuantizedCoefficients[BLOCK_SIZE];
    short VDCTopLeft, VDCTop;
    short *pVACLine;

    DWORD x, y, block;

#if DBG

    g_IFrameCount++;

#endif DBG

    MBWidth  = pDecoder->MBWidth;
    MBHeight = pDecoder->MBHeight;

    pY = pDecoder->pYBuilding;
    pU = pDecoder->pUBuilding;
    pV = pDecoder->pVBuilding;

#if DBG
    memset(pY, 0xFF, MBWidth * MACROBLOCK_SIZE * MBHeight * MACROBLOCK_SIZE);
    memset(pU, 0xFF, MBWidth * BLOCK_SIZE * MBHeight * BLOCK_SIZE);
    memset(pV, 0xFF, MBWidth * BLOCK_SIZE * MBHeight * BLOCK_SIZE);
#endif

    YPitch  = MBWidth * MACROBLOCK_SIZE;
    UVPitch = MBWidth * BLOCK_SIZE;

    if (PictureQuantizerScale <= 4)
    {
        DCStepSize = 8;
    }
    else
    {
        DCStepSize = PictureQuantizerScale / 2 + 6;
    }

    Escape3RunLength = 0;
    Escape3LevelLength = 0;

    // Initialize our YACPrecition tables.
    memset(pDecoder->pYAC, 0, sizeof(short) * MACROBLOCK_SIZE * MBWidth);
    memset(pDecoder->pUAC, 0, sizeof(short) * BLOCK_SIZE * MBWidth);
    memset(pDecoder->pVAC, 0, sizeof(short) * BLOCK_SIZE * MBWidth);

    DefaultDCPredictor = (short)((1024 + (DCStepSize >> 1)) / DCStepSize);

    for (x = 0; x < MBWidth; x++)
    {
        *(pDecoder->pYAC + x * MACROBLOCK_SIZE) = DefaultDCPredictor;
        *(pDecoder->pYAC + x * MACROBLOCK_SIZE + BLOCK_SIZE) = DefaultDCPredictor;

        *(pDecoder->pUAC + x * BLOCK_SIZE) = DefaultDCPredictor;
        *(pDecoder->pVAC + x * BLOCK_SIZE) = DefaultDCPredictor;
    }

    // Use the preallocated CBPCY array, setting the extra one off to
    // the far left so things work for the beginning of a row.
    //
    pCBPCYLine = pDecoder->pCBPCY + 1;

    // Blank it out, along with the "Last" one, to set to the default values.
    ASSERT(sizeof(XMVMacroblockCBPCY) == 1);

    memset(pDecoder->pCBPCY + 1, 0, sizeof(XMVMacroblockCBPCY) * MBWidth);

    // Are the DCT AC codes done at a macroblock level?
    MBLevelDCTACCoding = ReadOneBit(pDecoder);

    // If not, read the code up-front.
    if (!MBLevelDCTACCoding)
    {
        // The picture-wide DCT AC inter coding set index.
        pDCTACInterCodingSetTable = &g_InterDecoderTables[ReadTriStateBits(pDecoder)];

        // The picture-wide DCT AC intra coding set index (table 2).
        pDCTACIntraCodingSetTable = &g_IntraDecoderTables[ReadTriStateBits(pDecoder)];
    }

    // Which intra table to use.
    if (!ReadOneBit(pDecoder))
    {
        // Talking
        pYDCDifferentialTable  = g_Huffman_DCTDCy_Talking;
        pUVDCDifferentialTable = g_Huffman_DCTDCc_Talking;
    }
    else
    {
        // High motion
        pYDCDifferentialTable  = g_Huffman_DCTDCy_HighMotion;
        pUVDCDifferentialTable = g_Huffman_DCTDCc_HighMotion;
    }

    // Iterate through all of the macroblocks.
    for (y = 0; y < MBHeight; y++)
    {
        // Reset the line arrays.
        pCBPCYLine = pDecoder->pCBPCY + 1;
        *(BYTE *)&CBPCYTopLeft = 0;

        pYACLine      = pDecoder->pYAC;
        YDCTopLeft[0] = DefaultDCPredictor;

        pUACLine   = pDecoder->pUAC;
        UDCTopLeft = DefaultDCPredictor;

        pVACLine   = pDecoder->pVAC;
        VDCTopLeft = DefaultDCPredictor;

        memset(YLeftACQuantizedCoefficients, 0, sizeof(YLeftACQuantizedCoefficients));
        memset(ULeftACQuantizedCoefficients, 0, sizeof(ULeftACQuantizedCoefficients));
        memset(VLeftACQuantizedCoefficients, 0, sizeof(VLeftACQuantizedCoefficients));

        YLeftACQuantizedCoefficients[0][0] = DefaultDCPredictor;
        YLeftACQuantizedCoefficients[1][0] = DefaultDCPredictor;
        ULeftACQuantizedCoefficients[0]    = DefaultDCPredictor;
        VLeftACQuantizedCoefficients[0]    = DefaultDCPredictor;

        for (x = 0; x < MBWidth; x++)
        {
#if DBG
            g_TotalBitsRead = 0;
#endif DBG

#if DBG && 0

if (g_IFrameCount == 4 && x < 8 && y == 18)
{
    _asm int 3;
}

#endif DBG

            // Read the decoded iCBPCY.
            DecodedCBPCY = HuffmanDecode(pDecoder, g_Huffman_ICBPCY);

            DecodedY1 = (DecodedCBPCY >> 5) & 1;
            DecodedY2 = (DecodedCBPCY >> 4) & 1;
            DecodedY3 = (DecodedCBPCY >> 3) & 1;
            DecodedY4 = (DecodedCBPCY >> 2) & 1;

            // Calculate the CBPCY
            //
            // Y1 = (LT4 == T3 ? L2 : T3) ^ DecodedY1
            // Y2 = (T3 == T4 ? Y1 : T4) ^ DecodedY2
            // Y3 = (L2 == Y1 ? L4 : Y1) ^ DecodedY3
            // Y4 = (Y1 == Y2 ? Y3 : Y2) ^ DecodedY4
            //  U = DecodedU
            //  V = DecodedV
            //
            // We store all of the CBPCY values from the previous line in the
            // pCBPCYLine array.  The new values for this line are constructed
            // directly in that array so that the array contains the values
            // for this row up until the column we're building, but then
            // contains the values for the previous row.
            //
            // The algorithm for processing the row is:
            //
            //   Set TopLeft = 0
            //   for each x in the macroblock row
            //     Set Top = Line[x]
            //     Calculate Line[x] using TopLeft, Top and Line[x - 1] as Left
            //     TopLeft = Top
            //
            *(BYTE *)&CBPCYTop = *(BYTE *)(pCBPCYLine);

            pCBPCYLine->Y1 = (BYTE)((CBPCYTopLeft.Y4 == CBPCYTop.Y3 ? (pCBPCYLine - 1)->Y2 : CBPCYTop.Y3) ^ DecodedY1);
            pCBPCYLine->Y2 = (BYTE)((CBPCYTop.Y3 == CBPCYTop.Y4 ? pCBPCYLine->Y1 : CBPCYTop.Y4) ^ DecodedY2);
            pCBPCYLine->Y3 = (BYTE)(((pCBPCYLine - 1)->Y2 == pCBPCYLine->Y1 ? (pCBPCYLine - 1)->Y4 : pCBPCYLine->Y1) ^ DecodedY3);
            pCBPCYLine->Y4 = (BYTE)((pCBPCYLine->Y1 == pCBPCYLine->Y2 ? pCBPCYLine->Y3 : pCBPCYLine->Y2) ^ DecodedY4);
            pCBPCYLine->U  = (BYTE)((DecodedCBPCY >> 1) & 1);
            pCBPCYLine->V  = (BYTE)((DecodedCBPCY >> 0) & 1);

            *(BYTE *)&CBPCYTopLeft = *(BYTE *)&CBPCYTop;

            // Read the AC prediction flag.
            ACPredictionEnabled = ReadOneBit(pDecoder);

            // Read the macroblock-level DTC AC 
            if (MBLevelDCTACCoding && *(BYTE *)pCBPCYLine)
            {
                MBACCodingSetIndex = ReadTriStateBits(pDecoder);

                pDCTACInterCodingSetTable = &g_InterDecoderTables[MBACCodingSetIndex];
                pDCTACIntraCodingSetTable = &g_IntraDecoderTables[MBACCodingSetIndex];
            }

            // We use the same mechanism for keeping track of the most recent
            // Quantized AC Coefficients and DC Coefficients so we can do AC
            // coefficient prediction.  We do the same as with the CBPCY array
            // in that we only keep one row of coefficients but with the added
            // complexity of having to keep track of multiple lefts due to the
            // TopLeft/TopRight/BottomLeft/BottomRight order of the blocks
            // inside of the macroblock.
            //
            YDCTopLeftForNextMacroblock = *(pYACLine + BLOCK_SIZE);
            YDCTopLeft[1] = YLeftACQuantizedCoefficients[0][0];

            // For each of the Y block...
            for (block = 0; block < 4; block++)
            {
                pBlock = pY + BLOCK_SIZE * (block & 1) + BLOCK_SIZE * YPitch * (block >> 1);

                YDCTop = (pYACLine + (block & 1) * BLOCK_SIZE)[0];

                // Decode the block.
                DecodeBaselineIFrameBlock(pDecoder,
                                          pBlock,
                                          YPitch,
                                          DCStepSize,
                                          PictureQuantizerScale,
                                          (*(BYTE *)pCBPCYLine) & (1 << block),
                                          ACPredictionEnabled,
                                          YDCTopLeft[block >> 1],
                                          pYACLine + (block & 1) * BLOCK_SIZE,
                                          YLeftACQuantizedCoefficients[block >> 1], 
                                          pYDCDifferentialTable,
                                          INTRADCYTCOEF_ESCAPE_CODE,
                                          pDCTACIntraCodingSetTable,
                                          &Escape3RunLength,
                                          &Escape3LevelLength);

                YDCTopLeft[block >> 1] = YDCTop;

#if DBG && 0

if (g_IFrameCount == 4 && x < 8 && y == 18)
{
    DWORD i, j;

    DbgPrint("----- Y block # %d for Macroblock %d-%d\n\n", block, x, y);

    for (j = 0; j < BLOCK_SIZE; j++)
    {
        for (i = 0; i < BLOCK_SIZE; i++)
        {
            DbgPrint("%02X ", *(pBlock + i + j * YPitch));
        }

        DbgPrint("\n");
    }

    DbgPrint("\n");
}

#endif DBG

            }

            // Remember the next top left.
            YDCTopLeft[0] = YDCTopLeftForNextMacroblock;

            // Move to the next pair.
            pYACLine += MACROBLOCK_SIZE;

            // Move to the next macroblock.
            pY += MACROBLOCK_SIZE;

            // The U Block.
            UDCTop = pUACLine[0];

            DecodeBaselineIFrameBlock(pDecoder,
                                      pU,
                                      UVPitch,
                                      DCStepSize,
                                      PictureQuantizerScale,
                                      pCBPCYLine->U,
                                      ACPredictionEnabled,
                                      UDCTopLeft,
                                      pUACLine,
                                      ULeftACQuantizedCoefficients, 
                                      pUVDCDifferentialTable,
                                      INTRADCUVTCOEF_ESCAPE_CODE,
                                      pDCTACInterCodingSetTable,
                                      &Escape3RunLength,
                                      &Escape3LevelLength);

            UDCTopLeft = UDCTop;
            pUACLine  += BLOCK_SIZE;

#if DBG && 0

if (g_IFrameCount == 4 && x < 8 && y == 18)
{
    DWORD i, j;

    DbgPrint("----- U block for Macroblock %d-%d\n\n", x, y);

    for (j = 0; j < BLOCK_SIZE; j++)
    {
        for (i = 0; i < BLOCK_SIZE; i++)
        {
            DbgPrint("%02X ", *(pU + i + j * UVPitch));
        }

        DbgPrint("\n");
    }

    DbgPrint("\n");
}

#endif DBG

            pU += BLOCK_SIZE;

            // The V Block.
            VDCTop = pVACLine[0];

            DecodeBaselineIFrameBlock(pDecoder,
                                      pV,
                                      UVPitch,
                                      DCStepSize,
                                      PictureQuantizerScale,
                                      pCBPCYLine->V,
                                      ACPredictionEnabled,
                                      VDCTopLeft,
                                      pVACLine,
                                      VLeftACQuantizedCoefficients, 
                                      pUVDCDifferentialTable,
                                      INTRADCUVTCOEF_ESCAPE_CODE,
                                      pDCTACInterCodingSetTable,
                                      &Escape3RunLength,
                                      &Escape3LevelLength);

            VDCTopLeft = VDCTop;
            pVACLine  += BLOCK_SIZE;

#if DBG && 0

if (g_IFrameCount == 4 && x < 8 && y == 18)
{
    DWORD i, j;

    DbgPrint("----- V block for Macroblock %d-%d\n\n", x, y);

    for (j = 0; j < BLOCK_SIZE; j++)
    {
        for (i = 0; i < BLOCK_SIZE; i++)
        {
            DbgPrint("%02X ", *(pV + i + j * UVPitch));
        }

        DbgPrint("\n");
    }

    DbgPrint("\n");
}

#endif DBG


            pV += BLOCK_SIZE;

            // Move to the next CBPCY macroblock descriptor.
            pCBPCYLine++;
        }

        // pY now points to the second row of a macroblock...move it down to 
        // point to the first row of the next row of macroblock.
        //
        pY += YPitch * (MACROBLOCK_SIZE - 1);

        // pU and pV points to the second of a block, move it down to point to
        // the first row of the next row of blocks.
        //
        pU += UVPitch * (BLOCK_SIZE - 1);
        pV += UVPitch * (BLOCK_SIZE - 1);
    }
}

/* 
 * Decodes an I-frame.
 */

void DecodeIFrame
(
    XMVDecoder *pDecoder
)
{
    DWORD XINTRA8Encoded = FALSE;
    DWORD PictureQuantizerScale;

    //
    // Decode the rest of the frame header.
    //

    // Ignore the buffer-fullness field.
    SkipBits(pDecoder, 7);

    // Read the picture quantizer field.
    PictureQuantizerScale = ReadBits(pDecoder, 5);

    // Get the XINTRA8 encoding mode but only if it's enabled.
    if (pDecoder->XIntra8IPictureCodingEnabled)
    {
        XINTRA8Encoded = ReadOneBit(pDecoder);
    }

    // Decode the proper frame.
    if (!XINTRA8Encoded)
    {
        DecodeBaselineIFrame(pDecoder, PictureQuantizerScale);
    }
    else
    {
        _asm int 3;
        //DecoderXINTRA8IFrame(pDecoder, &FrameData);
    }
}

/*
 * Decodes on frame into the next frame's YUV buffer.  This assumes that
 * the bit stream pointers have already been set up.
 */

void DecodeOneFrame
(
    XMVDecoder *pDecoder
)
{
    // I-Frame.
    if (!ReadOneBit(pDecoder))
    {
        DecodeIFrame(pDecoder);
    }

    // P-Frame
    else
    {
    }
}

