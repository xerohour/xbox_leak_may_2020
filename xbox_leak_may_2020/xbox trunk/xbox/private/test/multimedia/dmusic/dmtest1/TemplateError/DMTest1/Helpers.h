//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1996 - 1999
//
//  File:       helpers.h
//
//--------------------------------------------------------------------------
#pragma once


#include "globals.h"
#include "dmthcom.h"    // logging implementations of common COM functions
//---------------------------------------------------------------------------

// symbolic constants
#define SZAPPFRIENDLYNAME   "DMTest1"

// minimum logging level
#ifndef MINLOGLEVEL
#define MINLOGLEVEL         0
#endif
// logging level for "test aborting" messages
#ifndef ABORTLOGLEVEL
#define ABORTLOGLEVEL       0
#endif
// logging level for "bug found" messages
#ifndef BUGLOGLEVEL
#define BUGLOGLEVEL         0
#endif
// logging level for "for your information" messages
#ifndef WARNLOGLEVEL
#define WARNLOGLEVEL        1
#endif
// logging level for "for your information" messages
#ifndef FYILOGLEVEL
#define FYILOGLEVEL         2
#endif
// logging level for parameter data
#ifndef PARAMLOGLEVEL
#define PARAMLOGLEVEL       5           
#endif
// logging level for structure contents
#ifndef STRUCTLOGLEVEL
#define STRUCTLOGLEVEL      6
#endif
// maximum logging level
#ifndef MAXLOGLEVEL
#define MAXLOGLEVEL         10
#endif
// max length of a logging string
#ifndef MAX_LOGSTRING
#define MAX_LOGSTRING       256
#endif
// max length of a string
#ifndef MAX_STRING
#define MAX_STRING          256
#endif

//MIDI helpers
#define MAKEPATCH(msb,lsb,patch) (((msb) << 16) | ((lsb) << 8) | (patch))
#define MAKEDRUMS(patch) ((patch) | 0x80000000)

// MIDI EVENT TYPE DEFINES
#define MIDI_NOTEOFF    0x80
#define MIDI_NOTEON     0x90
#define MIDI_PTOUCH     0xA0
#define MIDI_CCHANGE    0xB0
#define MIDI_PCHANGE    0xC0
#define MIDI_MTOUCH     0xD0
#define MIDI_PBEND      0xE0
#define MIDI_SYSX       0xF0
#define MIDI_MTC        0xF1
#define MIDI_SONGPP     0xF2
#define MIDI_SONGS      0xF3
#define MIDI_EOX        0xF7
#define MIDI_CLOCK      0xF8
#define MIDI_START      0xFA
#define MIDI_CONTINUE   0xFB
#define MIDI_STOP       0xFC
#define MIDI_SENSE      0xFE
#define CC_MODWHEEL		0x01
#define CC_VOLUME		0x07
#define CC_PAN			0x0A
#define CC_EXPRESSION	0x0B
#define CC_SUSTAIN		0x40

// Macros to make better use of a dword parameter
#define DMTH_PACK_2WORD(word1,word2) ( (DWORD)( ( (word1) << 16 ) | (word2) ) )
#define DMTH_EXTR_WORD1(dword) ( (dword) >> 16 )
#define DMTH_EXTR_WORD2(dword) ( (dword) & 0x0000FFFF )

// Macros to help handling HRESULTs
#define HR_SUCCESS(hr, f) (SUCCEEDED(hr = f))
#define HR_FAIL(hr, f) (FAILED(hr = f))
#define HR_SOK(hr, f) ((hr = f) == S_OK)
#define HR_FALSE(hr, f) ((hr = f) == S_FALSE)
#define HR_NOTSOK(hr, f) !((hr = f) == S_OK)



HRESULT dmthLoadSegment(CHAR *pSegmentFile, CtIDirectMusicSegment8 **ppwSegment); //dx8
HRESULT dmthCreatePerformance(REFIID riid, CtIDirectMusicPerformance8 **pwPerformance); //dx8

//The following 3 functions are generic helper functions that do the grunt work
//of the Get/Set/Parse Descriptor tests.  They return FNS_<result> code
//For usage, see the Collection Unit tests
DWORD  dmthObject_GetDescriptorTest( CtIUnknown* pCtUnk );
DWORD  dmthObject_SetDescriptorTest( CtIUnknown* pCtUnk );
DWORD  dmthObject_ParseDescriptorTest(IStream* pIStream, CtIUnknown* pCtUnk);


LPVOID TDMGetBogusPointer(void);
void TDMFreeBogusPointer(LPVOID lpv);

DWORD tdmPackDWORD(WORD wHigh, WORD wLow);
BOOL tdmEnableBreakOnDMAssert(BOOL fBreak);

//Make sure you pass the REAL object ptr into these functions!
HRESULT dmthTestQI(IUnknown *pIU_Src, REFIID IID_Src, char *szSrcName, REFIID IID_Dst, char *szDstName); 
HRESULT dmthTestAddRefRelease(IUnknown *pIU_Src, REFIID IID_Src, char *szSrcName);  

BOOL FillBufferWithSineWave(
 LPVOID			    pBuffer, 
 DWORD				dwBufferBytes,
 DWORD              dwFrequency,
 LPWAVEFORMATEX		pWfx,
 DOUBLE				fFactor
 );

DWORD IgnorePChannel(DWORD dwStage);
void CleanUpIUnknown(HRESULT hr, LPUNKNOWN &pUnk);
LPCSTR PathFindFileName(LPCSTR pPath);
HRESULT dmthPlayMidiFile(CtIDirectMusicPerformance8 *pwPerformance8,
						 REFIID riid,
						 CtIDirectMusicSegment8 **ppwSegment8,
						 CtIDirectMusicSegmentState **ppwSegmentState,
                         CHAR *pMidiFile);
HRESULT CreateAndInitPerformance(CtIDirectMusicPerformance8 **ppPerf8);
HRESULT ChopPath(const char *szFullString, char *szPathOut, char *szFileOut);


// **********************************************************************
// **********************************************************************
template <class T>
T* SafeRelease (T* pT)
{
    if (pT)
    {
        pT->Release ();
    }

    return (NULL);
}


// **********************************************************************
//Added by danhaff 08-07-00
//
//PURPOSE:
//Given an array, finds the index of the value you pass.
// **********************************************************************
template <class T> 
DWORD FindIndex(T *pArray, DWORD dwCount, T Target)
{
DWORD i;

for (i=0; i<dwCount; i++)
{
    if (pArray[i] == Target)
    {
        return i;
    }
}
return 0xFFFFFFFF;
};


//===========================================================================
// dmthCreateTestWrappedObject
//
// template for making object creation easier
//
// Parameters:
//  REALOBJECT  *pObj   - pointer to the real object
//  TESTOBJECT  *pptObj - ptr to return newly created test object
//
// Returns: HRESULT
//===========================================================================
template <class REALOBJECT, class TESTOBJECT>
HRESULT _stdcall dmthCreateTestWrappedObject(REALOBJECT *pObj, TESTOBJECT **pptObj)
{
    HRESULT hRes    = E_FAIL;

    // validate pObj
    if(!helpIsValidPtr((void*)pObj, sizeof(REALOBJECT), FALSE))
    {
        fnsLog(MINLOGLEVEL, "Invalid ptr (Arg 1) passed to "
                "dmthCreateTestWrappedObject (%08Xh)",
                pObj);
        return E_POINTER;
    }

    // validate pptObj
    if(!helpIsValidPtr((void*)pptObj, sizeof(TESTOBJECT*), FALSE))
    {
        fnsLog(MINLOGLEVEL, "Invalid ptr (Arg 2) passed to dmthCreateTestWrappedObject (%08Xh)", pptObj);
        return E_POINTER;
    }

    // create the test object
    *pptObj = new TESTOBJECT();
    if(NULL != *pptObj)
    {
        // initialize the test class
        hRes = (*pptObj)->InitTestClass(pObj);
        if(FAILED(hRes))
        {
            fnsLog(MINLOGLEVEL, "**** Unable to initalize XXXXX test class (%s == %08Xh)",tdmXlatHRESULT(hRes), hRes);
            delete (*pptObj);
            *pptObj = NULL; 
        }
    }

    // done
    return hRes;

} //*** end dmthCreateTestWrappedObject()


#define AMOUNT(a) (sizeof(a) / sizeof(a[0]))



