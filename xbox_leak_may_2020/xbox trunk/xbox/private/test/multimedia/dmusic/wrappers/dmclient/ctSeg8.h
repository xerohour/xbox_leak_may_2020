#pragma once

#include "dmusici.h"
#include "ctiunk.h"



//===========================================================================
// CtIDirectMusicPerformance8
//
// Wrapper class for IDirectMusicPerformance8
//
// Inherits from CtIDirectMusicPerformance
//===========================================================================
class CtIDirectMusicSegment8 : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicSegment8();
    //~CtIDirectMusicSegment8();
    // test class helpers
    //HRESULT InitTestClass(void);
    //HRESULT InitTestClass(LPVOID pvXBoxSegment);


    HRESULT Download(CtIUnknown *pAudioPath);
    HRESULT Unload(CtIUnknown *pAudioPath);





};
