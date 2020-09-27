//
// CMIXBINS.cpp
// 
// Copyright (c) 2001 Microsoft Corporation
//
// Created by danhaff 9/24/01
//
// Class representation of a DSMIXBINS struct.  Has lots of checks in 
// debug mode to prevent screw-ups.

#include "pchime.h"

/********************************************************************************
********************************************************************************/
CMIXBINS::CMIXBINS(void)
{
    m_bValid = FALSE;
    m_DSMixBins.dwMixBinCount = 0;        
    m_DSMixBins.lpMixBinVolumePairs = &m_VolumePair[0];
    ZeroMemory(&m_VolumePair, sizeof(DSMIXBINVOLUMEPAIR) * DSMIXBIN_ASSIGNMENT_MAX);
};


#ifdef _DEBUG
/********************************************************************************
********************************************************************************/
CMIXBINS::~CMIXBINS(void)
{
    //Trash our data.
    m_bValid = FALSE;
    memset(&m_DSMixBins,  0xFF, sizeof(m_DSMixBins));
    memset(&m_VolumePair, 0xFF, sizeof(DSMIXBINVOLUMEPAIR) * DSMIXBIN_ASSIGNMENT_MAX);
};
#endif

/********************************************************************************
DMusic doesn't care about volumes in some places, and simply stores its mixbin
values in a mask.  This sets the object based on that mask.
********************************************************************************/
void CMIXBINS::CreateFromMask(DWORD dwMask)
{
    DWORD dwChorusIndex = -1, dwReverbIndex = -1;

    //Make sure we're not blowing away an already-initialized object.
    ASSERT(!m_bValid);
    m_DSMixBins.dwMixBinCount = 0;
    for (DWORD i=0; i<32; i++)
    {
        if (dwMask & (1<<i))
        {

            if (i == DSMIXBIN_FXSEND_0) {
                dwReverbIndex = m_DSMixBins.dwMixBinCount;
            }

            if (i == DSMIXBIN_FXSEND_1) {
                dwChorusIndex = m_DSMixBins.dwMixBinCount;
            }

            m_VolumePair[m_DSMixBins.dwMixBinCount].dwMixBin = i;
            m_VolumePair[m_DSMixBins.dwMixBinCount].lVolume  = 0;
            m_DSMixBins.dwMixBinCount++;

        }
    }

    if (m_DSMixBins.dwMixBinCount <=6) {
        
        
        if (dwReverbIndex != -1) {
            
            //
            // for audiopaths that send data to chorus or reverb , specify two identical
            // mixbins for each fxsend. This way stereo waves will play properly and mono waves
            // will be at the same volume level
            //
            
            m_VolumePair[dwReverbIndex+1].dwMixBin = DSMIXBIN_FXSEND_0;
            m_VolumePair[dwReverbIndex+1].lVolume  = 0;
            m_DSMixBins.dwMixBinCount++;
            
        }
        
        if (dwChorusIndex != -1) {
            
            //
            // for audiopaths that send data to chorus or reverb , specify two identical
            // mixbins for each fxsend. This way stereo waves will play properly and mono waves
            // will be at the same volume level
            //
            
            for (i =1; i<3;i++) {
                m_VolumePair[dwChorusIndex+i].dwMixBin = DSMIXBIN_FXSEND_1;
                m_VolumePair[dwChorusIndex+i].lVolume  = 0;
            }

            m_DSMixBins.dwMixBinCount++;
            
        }
        
    }



    m_bValid = TRUE;
};

/********************************************************************************
Simply returns a pointer to the class's DSMIXBINS data.  Of course, the data
pointed to is only valid during the scope of the class.
********************************************************************************/
LPCDSMIXBINS CMIXBINS::GetMixBins(void)
{
    ASSERT(m_bValid);
    if (!m_DSMixBins.dwMixBinCount)
        return NULL;
    else
        return &m_DSMixBins;
};

/******************************************w**************************************
This function was built to interface with a DMusic kludge.  DSound's SetMixBinVolumes
once took a set of flags and an array of volumes.  Now DSound's mixbin identifiers have
changed from flags (1, 2, 4, 8) to indices (1, 2, 3, 4).  To minimize code changes,
I simply replaced all instances of (DSMIXBIN_*) with (1 << DSMIXBIN_*) to change
them back to flags so DMusic could continue to use a piddly DWORD instead of a 
DSMIXBINS struct to store the data, since DMusic was storing its volume info separately
anyway.  Eventually DMusic would call SetMixBinVolumes with its flags and volume array,
so this translates from that type of data into a real DSMIXBINS struct which
can be returned with GetMixBins().
********************************************************************************/
void CMIXBINS::PokeMixBinVolumesBasedOnMask(DWORD dwMixBinMask, long *alVolumes)
{
    DWORD dwCount = 0;
    DWORD i       = 0;
    
    ASSERT(m_bValid);
    ASSERT(alVolumes);        
    #ifdef _DEBUG
    DWORD dwBitCount = 0;
    for (i=0; i<32; i++)
    {
        if ((1<<i) & dwMixBinMask)
            dwBitCount++;
    }        
    ASSERT(dwBitCount <= DSMIXBIN_ASSIGNMENT_MAX);
    ASSERT(dwBitCount != 0);
    #endif

    //Scan through bits in the mask.
    for (i=0; i<32; i++)
    {
        //If a bit is set...
        if ((1<<i) & dwMixBinMask)
        {
            //...then find the mixbin represented by that bit and set its volume to the next element in the alVolumes array.
            BOOL bFound = FALSE;
            for (DWORD dwMixBinIndex=0; dwMixBinIndex<DSMIXBIN_ASSIGNMENT_MAX; dwMixBinIndex++)
            {
                if (m_DSMixBins.lpMixBinVolumePairs[dwMixBinIndex].dwMixBin == i)
                {
                    m_VolumePair[dwMixBinIndex].lVolume = alVolumes[dwCount];

                    if ((i == DSMIXBIN_FXSEND_0) || (i == DSMIXBIN_FXSEND_1)) 
                    {
                        //
                        // reverb send, this means the next volumepair is also for the reverb
                        //

                        m_VolumePair[dwMixBinIndex+1].lVolume = alVolumes[dwCount];
                    }

                    dwCount++;
                    break;
                }

            }
        }
    }

    #ifdef _DEBUG
    ASSERT(dwCount == dwBitCount);
    #endif
};


/********************************************************************************
********************************************************************************/
BOOL CMIXBINS::operator == (CMIXBINS &p_MixBins)
{
    ASSERT(m_bValid);
    ASSERT(p_MixBins.m_bValid);

    if (m_DSMixBins.dwMixBinCount == p_MixBins.m_DSMixBins.dwMixBinCount)
        if (memcmp(m_DSMixBins.lpMixBinVolumePairs, p_MixBins.m_DSMixBins.lpMixBinVolumePairs, sizeof(DSMIXBINVOLUMEPAIR) * p_MixBins.m_DSMixBins.dwMixBinCount))
            return TRUE;
    return FALSE;
}

/********************************************************************************
********************************************************************************/
BOOL CMIXBINS::operator != (CMIXBINS &p_MixBins)
{
    return (!(*this == p_MixBins));
}


/********************************************************************************
********************************************************************************/
/*
void CMIXBINS::SetMixBinCount(DWORD dwMixBinCount)
{
    ASSERT(dwMixBinCount < DSMIXBIN_ASSIGNMENT_MAX);
    m_DSMixBins.dwMixBinCount = dwMixBinCount;
};
*/


/********************************************************************************
********************************************************************************/
/*
void CMIXBINS::SetMixBinVolumePair(DWORD dwIndex, DSMIXBINVOLUMEPAIR &VolumePair)
{
    ASSERT(dwIndex < m_DSMixBins.dwMixBinCount);
    m_VolumePair[dwIndex] = VolumePair;
    m_bValid = TRUE;

};
*/
/********************************************************************************
********************************************************************************/
/*
void CMIXBINS::SetMixBinVolumePair(DWORD dwIndex, DWORD dwMixBin, long lVolume)
{
    ASSERT(dwIndex < m_DSMixBins.dwMixBinCount);
    m_VolumePair[dwIndex].dwMixBin = dwMixBin;
    m_VolumePair[dwIndex].lVolume  = lVolume;
    m_bValid = TRUE;
};
*/



/********************************************************************************
Actually COPIES the data into our object.
********************************************************************************/
/*
void CMIXBINS::SetMixBins(LPCDSMIXBINS pMixBins)
{
    ASSERT(pMixBins->dwMixBinCount < DSMIXBIN_ASSIGNMENT_MAX);
    ASSERT( !(pMixBins->dwMixBinCount && pMixBins->lpMixBinVolumePairs));
    ASSERT(pMixBins);

    m_DSMixBins.dwMixBinCount = pMixBins->dwMixBinCount;
    memcpy((void *)m_DSMixBins.lpMixBinVolumePairs, pMixBins->lpMixBinVolumePairs, sizeof(DSMIXBINVOLUMEPAIR) * m_DSMixBins.dwMixBinCount);
    m_bValid = TRUE;        
};
*/


/********************************************************************************
Not inline, because it contains more code than usual and will be called multiple
times from a function.
********************************************************************************/
/*
BOOL CMIXBINS::ContainsMixBin(DWORD dwMixBin)
{
    ASSERT(m_bValid);
    ASSERT(dwMixBin);

    for (DWORD i=0; i<DSMIXBIN_ASSIGNMENT_MAX; i++)
    {
        //Found it.
        if (m_DSMixBins.lpMixBinVolumePairs[i].dwMixBin == dwMixBin)
            return TRUE;
    }

    //Didn't find it.
    return FALSE;
}
*/


/********************************************************************************
********************************************************************************/
/*
BOOL DSMIXBINSAreEqual(LPCDSMIXBINS p1, LPCDSMIXBINS p2)
{

};
*/
