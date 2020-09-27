 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/****************************** DMA Manager ********************************\
*                                                                           *
* Module: CMixerBuffer.h													*
*   Class to resource manage the mixer buffers							    *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#if !defined _CMIXERBUFFERH_
#define _CMIXERBUFFERH_

// max 256
#define MIXER_BUFFERS_HW            32
#define MIXER_BUFFERS_SW            32


class CMixerBuffer
{
public:

    enum MIXER_TYPE
    {
        HW,
        SW
    };

    CMixerBuffer()
    {
        U008 uCnt;

        for (uCnt = 0; uCnt < MIXER_BUFFERS_HW; uCnt++)
            m_bMixHw[uCnt] = FALSE;

        for (uCnt = 0; uCnt < MIXER_BUFFERS_SW; uCnt++)
            m_bMixSw[uCnt] = FALSE;
    }

    RM_STATUS Allocate(MIXER_TYPE eType, U008 *pId)
    {
        U008 uCnt;

        if (eType == HW)
        {
            for (uCnt = 0; uCnt < MIXER_BUFFERS_HW; uCnt++)
            {
                if (m_bMixHw[uCnt] == FALSE)
                {
                    *pId = uCnt;
                    m_bMixHw[uCnt] = TRUE;
                    return RM_OK;

                }
            }
        }
        else
        {
            for (uCnt = 0; uCnt < MIXER_BUFFERS_SW; uCnt++)
            {
                if (m_bMixSw[uCnt] == FALSE)
                {
                    *pId = MIXER_BUFFERS_HW + uCnt;
                    m_bMixSw[uCnt] = TRUE;
                    return RM_OK;

                }
            }
        }

        return RM_ERROR;
    }
    
    VOID Free(U008 uId)
    {
        if (uId < MIXER_BUFFERS_HW)
            m_bMixHw[uId] = FALSE;
        else
            m_bMixSw[uId - MIXER_BUFFERS_HW] = FALSE;
    }
    
private:
    BOOL m_bMixHw[MIXER_BUFFERS_HW];
    BOOL m_bMixSw[MIXER_BUFFERS_SW];
};


#endif