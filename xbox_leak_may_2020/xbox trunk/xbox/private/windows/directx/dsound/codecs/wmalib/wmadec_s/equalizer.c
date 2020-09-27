/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    MsAudioDec.cpp

Abstract:

    Implementation of top level functions of CAudioObjectDecoder.

Author:

    Wei-ge Chen (wchen) 14-July-1998

Revision History:


*************************************************************************/
#pragma code_seg("WMADEC")
#pragma data_seg("WMADEC_RW")
#pragma const_seg("WMADEC_RD")

#include "..\..\..\dsound\dsndver.h"

#ifdef ENABLE_EQUALIZER

#include "msaudiodec.h"
#include "macros.h"

WMARESULT audecSetEqualizer (CAudioObjectDecoder* paudec, Int iBand, Int iGainIndB)
{
    if (iBand >= 0 && iBand < MAX_NO_EQBAND && iGainIndB >= MIN_EQ && iGainIndB <= MAX_EQ)  {
        paudec->m_fNoEq = WMAB_FALSE;
        if (iGainIndB != 0) {   
            paudec->m_rgfltEqBandWeight [iBand] = (Float) pow (10, iGainIndB / 20.0);
        }
        else 
            paudec->m_rgfltEqBandWeight [iBand] = 1.0F;
        return WMA_OK;
    }
    else 
        return TraceResult(WMA_E_INVALIDARG);
}

WMARESULT audecResetEqualizer (CAudioObjectDecoder* paudec)
{
    Int i;
    paudec->m_fNoEq = WMAB_TRUE; 
    for (i = 0; i < MAX_NO_EQBAND; i++)
        paudec->m_rgfltEqBandWeight [i] = 1.0F;

    return WMA_OK;
}

WMARESULT prvEqualize (CAudioObjectDecoder* paudec, PerChannelInfo* ppcinfo)
{
    Int i = 0;
    Int iCoef;
    Float fltWeightC = 1.0F;
    Float fltWeightR = 0;
    Int   iWidthR = 0;

    Float fltBandDiffL;
    Float fltBandDiffR;
    Int iWidthOfTransition;

    if (paudec->m_fNoEq)
        return WMA_OK;

    // Calculate first equalizer weighting
    do
    {
        fltWeightR += paudec->m_rgfltEqBandWeight[i];
        i += 1;
    } while (i < paudec->m_cEqBand && paudec->m_rgwEqBandBoundary[i - 1] == paudec->m_rgwEqBandBoundary[i]);
    iWidthR = i;
    fltWeightR /= iWidthR; // Average all the EQ gains

    // Apply each equalizer band's weighting to the desired coefficients
    for (i = 0; i < paudec->m_cEqBand; ) {
        Float fltWeightL;
        Float fltWeight;
        Int   iWidthC;

        // Shift weightings and widths down from right to left
        fltWeightL = fltWeightC;
        fltWeightC = fltWeightR;
        iWidthC = iWidthR;
        assert(iWidthC > 0);

        // Calculate next band's weight and width (one or more EQ bands may live
        // in one coefficient bin if frame size is small)
        if (i + iWidthC >= paudec->m_cEqBand)
        {
            fltWeightR = 1.0F;
            iWidthR = 0;
        }
        else
        {
            Int j = i + iWidthC;
            fltWeightR = 0;
            do
            {
                fltWeightR += paudec->m_rgfltEqBandWeight[j];
                j += 1;
            } while (j < paudec->m_cEqBand && paudec->m_rgwEqBandBoundary[j - 1] == paudec->m_rgwEqBandBoundary[j]);
            iWidthR = j - (i + iWidthC);
            fltWeightR /= iWidthR; // Average all the EQ gains
        }


        // EQUALIZE
        fltBandDiffL = fltWeightC - fltWeightL;
        fltBandDiffR = fltWeightC - fltWeightR;
        iWidthOfTransition = min (5, (paudec->m_rgwEqBandBoundary [i + iWidthC] -
            paudec->m_rgwEqBandBoundary [i]) / 4);

        for (iCoef = paudec->m_rgwEqBandBoundary [i];
            iCoef < paudec->m_rgwEqBandBoundary [i + iWidthC]; iCoef++)
        {
            if (iCoef - paudec->m_rgwEqBandBoundary [i] < iWidthOfTransition)
            {
                fltWeight = fltWeightC - (iWidthOfTransition - 1 - (iCoef -
                    paudec->m_rgwEqBandBoundary [i])) * fltBandDiffL / iWidthOfTransition;
            }
            else if (paudec->m_rgwEqBandBoundary [i + iWidthC] - iCoef <= iWidthOfTransition)
            {
                fltWeight = fltWeightC - (iWidthOfTransition -
                    (paudec->m_rgwEqBandBoundary [i + iWidthC] - iCoef)) *
                    fltBandDiffR / iWidthOfTransition;
            }
            else
                fltWeight = fltWeightC;

            ppcinfo->m_rgfltCoefRecon [iCoef] *= fltWeight;
        }

        // Advance indices
        i += iWidthC;

    } // for

    return WMA_OK;
}

Void    prvComputeBandPower (CAudioObjectDecoder* paudec)
{
    Int i;
    Float fltPower = 0;
    Int   iWidthC;
    Int   j;
    Int   iCoef;
    Float fltPwrOffset;

    for (i = 0; i < paudec->m_cEqBand; )
    {
        fltPower = 0;

        // Figure out width of this EQ band (more than one EQ band
        // may map to a single coefficient if frame size is small)
        j = i + 1;
        while (j < paudec->m_cEqBand && paudec->m_rgwEqBandBoundary[j - 1] == paudec->m_rgwEqBandBoundary[j])
            j++;
        iWidthC = j - i;

        // Calculate power of all coefficients within this EQ band
        for (iCoef = paudec->m_rgwEqBandBoundary [i];
            iCoef < paudec->m_rgwEqBandBoundary [i + iWidthC]; iCoef++)
        {
            fltPower += paudec->pau->m_rgpcinfo[0].m_rgfltCoefRecon[iCoef] *
                        paudec->pau->m_rgpcinfo[0].m_rgfltCoefRecon[iCoef];

            if (paudec->pau->m_cChannel > 1)
            {
                assert (paudec->pau->m_cChannel == 2);
                fltPower += paudec->pau->m_rgpcinfo[1].m_rgfltCoefRecon[iCoef] *
                            paudec->pau->m_rgpcinfo[1].m_rgfltCoefRecon[iCoef];
            }
        } // for

        // Record the power in one or more EQ bands
        assert (fltPower >= 0);

        // Fwd transform used to scale coefficients by sqrt(2/paudec->pau->m_cSubband),
        // but now does not so we must explicitly scale
        fltPwrOffset = (Float) (10 * log10(2.0F / paudec->pau->m_cSubband));
        for (j = i; j < i + iWidthC; j++)
        {
            fltPower = fltPwrOffset + (Float)(10*log10(fltPower));
            paudec->m_rgbBandPower[j] = (I8) checkRange (fltPower, SCHAR_MIN, SCHAR_MAX);
        }

        // Advance indices
        i += iWidthC;

    } // for
} // computeBandPower

WMARESULT audecGetBandPower (CAudioObjectDecoder* paudec, I8* rgbBandPower)
{
    rgbBandPower [0] = paudec->m_cEqBand;
    memcpy (rgbBandPower + 1, paudec->m_rgbBandPower, sizeof (I8) * MAX_NO_EQBAND);
    return WMA_OK;
}

Void    audecStartComputeBandPower (CAudioObjectDecoder* paudec)
{
    paudec->m_fComputeBandPower = WMAB_TRUE;
}

Void    audecStopComputeBandPower (CAudioObjectDecoder* paudec)
{
    paudec->m_fComputeBandPower = WMAB_FALSE;
}

Void    prvAdaptEqToSubFrame(CAudioObjectDecoder* paudec)
{
    Int i;
    // Set up the equalizer, if it needs it
    if (WMAB_FALSE == paudec->m_fNoEq && paudec->m_iEqFrameSize != paudec->pau->m_cSubFrameSampleAdjusted)
    {
        const static U16 rgwEqBandFreq [MAX_NO_EQBAND + 1] = {0, 50, 100, 200, 400, 800,
            1600, 3200, 6400, 12800, 24100};   //make sure the last one is beyond the end

        const Float fltSamplingPeriod = 1.0F / paudec->pau->m_iSamplingRate;

        paudec->m_iEqFrameSize = paudec->pau->m_cSubFrameSampleAdjusted;
        paudec->m_rgwEqBandBoundary [0] = 0;
        for (i = 0; i < MAX_NO_EQBAND; i++) {
            paudec->m_rgwEqBandBoundary [i + 1] = (U16) (rgwEqBandFreq [i + 1] *
                paudec->pau->m_cSubFrameSampleAdjusted * fltSamplingPeriod + 0.5f); //open end

            // Note that is is possible for consecutive elements of
            // paudec->m_rgwEqBandBoundary to have the same value, especially
            // in the case of small frames!

            if (paudec->m_rgwEqBandBoundary [i + 1] > paudec->pau->m_cSubbandAdjusted)    {
                paudec->m_rgwEqBandBoundary [i + 1] = (U16) paudec->pau->m_cSubbandAdjusted;
                paudec->m_cEqBand = i + 1;
                break;
            }
        } // for
    } // if
}
#endif //ENABLE_EQUALIZER
