//
// CMIXBINS.H
// 
// Copyright (c) 2001 Microsoft Corporation
//
// Created by danhaff 9/24/01
//
// Class representation of a DSMIXBINS struct.  Has lots of checks in 
// debug mode to prevent screw-ups.


#pragma once

#include <xtl.h>
#include <dsound.h>

class CMIXBINS
{
public:
    CMIXBINS(void);
    LPCDSMIXBINS GetMixBins(void);
    void         CreateFromMask(DWORD dwMask);
    void         PokeMixBinVolumesBasedOnMask(DWORD dwMixBinMask, long *alVolumes);
    BOOL         operator != (CMIXBINS &p_MixBins);
    BOOL         operator == (CMIXBINS &p_MixBins);

    #ifdef _DEBUG
    ~CMIXBINS(void);
    #endif

private:
    BOOL  m_bValid;
    DSMIXBINS m_DSMixBins;
    DSMIXBINVOLUMEPAIR m_VolumePair[DSMIXBIN_ASSIGNMENT_MAX];
};