#include "bldsetup.h"

//V7PLUS/Scrunch/wmaudio/integer/config/config.c#1 - add change 18676 (text)
#include "msaudio.h"
#include "config.h"

//
// Structures we use to store the samplingrate/bitrate mapping
//
#pragma pack(1)                 // byte-pack the structure
typedef struct WMARateEntry_tag
{
    U32 nBitrate;             // bit rate
    U8  nFramesLookAhead;     // no. of look ahead frames
    
    U8  fAllowSuperFrame;
    U8  nFramesPerPacket;
    U8  nMaxSubFrameDiv;
    
    WeightMode wtMode; //from msaudio.h, typedef enum {LPC_MODE=0,BARK_MODE} WeightMode;
    
} WMARateEntry;
#pragma pack()

typedef struct WMARateList_tag
{
    U32 nSamplingRate;
    U32 nChannels;
    U32 nEntries;
    const WMARateEntry *pRates;
} WMARateList;


#define SIZEOF_ARRAY(ar) (sizeof(ar)/sizeof((ar)[0]))

//
// Data for the samplingrate/bitrate mapping
//
// ---------------------------------------------------------------------------
// For Windows Media Audio V2

// mono
static const WMARateEntry gWma2RateList8000m[] = {
#if !defined (V7_CONFIG) // use buffer, rather than frame cbr
    { 8000, 32, 1, 10, 2, LPC_MODE }, // no-sub
    { 6000, 32, 1, 10, 2, LPC_MODE }, // sub
    { 5000, 32, 1, 10, 2, LPC_MODE }, // sub
#else
    { 8000, 0, 0, 1, 1, LPC_MODE }, // no-sub
    { 6000, 0, 0, 1, 1, LPC_MODE }, // sub
    { 5000, 0, 0, 1, 1, LPC_MODE }, // sub
#endif // !defined (V7_CONFIG) 
    {  128, 0, 0, 1, 1, LPC_MODE }  /* special silence mode: can't go any lower because nBlockAlign has to be >= 1 */
};

static const WMARateEntry gWma2RateList11025m[] = {
#if !defined (V7_CONFIG) // use buffer, rather than frame cbr
    { 10000, 32, 1, 10, 2, LPC_MODE },// sub
    {  8000, 32, 1, 10, 2, LPC_MODE } // sub
#else
    { 10000, 0, 0, 1, 1, LPC_MODE },// sub
    {  8000, 0, 0, 1, 1, LPC_MODE } // sub
#endif  // !defined (V7_CONFIG)
};

static const WMARateEntry gWma2RateList16000m[] = { // 448ms per packet, or 2.23 packets per second.
#if !defined (V7_CONFIG) // increase # packets per buffer
    { 16000, 32, 1, 10, 4, LPC_MODE },// In encoder, we produce max 2 subframes in this config.
    { 12000, 32, 1, 10, 2, LPC_MODE },// In encoder, we produce max 2 subframes in this config.
    { 10000, 32, 1, 10, 2, LPC_MODE },// In encoder, we produce max 2 subframes in this config.
#else
    { 16000, 32, 1, 14, 4, LPC_MODE },// sub
    { 12000, 32, 1, 14, 2, LPC_MODE },// sub
    { 10000, 32, 1, 14, 2, LPC_MODE },// sub
#endif
};

static const WMARateEntry gWma2RateList22050m[] = { // 325ms per packet, or 3.08 packets per second.
#if !defined (V7_CONFIG) // increase # packets per buffer
    { 20000, 32, 1, 8, 8, BARK_MODE },// sub
    { 16000, 32, 1, 8, 4,  BARK_MODE },// sub
#else
    { 20000, 32, 1, 14, 8, LPC_MODE },// sub
    { 16000, 32, 1, 14, 4, LPC_MODE },// sub
#endif
};

static const WMARateEntry gWma2RateList32000m[] = {
#if !defined (V7_CONFIG) // increase # packets per buffer
    { 20000, 32, 1, 8, 8, BARK_MODE } // sub // 640ms per packet, or 1.5625 packets per second. 
#else
    { 20000, 32, 1, 10, 8, LPC_MODE } // sub // 640ms per packet, or 1.5625 packets per second. 
#endif
};

static const WMARateEntry gWma2RateList44100m[] = { // 371.5ms per packet, or 2.69 packets per second
    { 48000, 32, 1, 8, 16, BARK_MODE },// no-sub
    { 32000, 32, 1, 8, 16, BARK_MODE },// no-sub
};

// stereo
static const WMARateEntry gWma2RateList8000s[]  = {
#if !defined (V7_CONFIG) // use buffer, rather than frame cbr
    { 12000, 32, 1, 10, 2, LPC_MODE }   // sub
#else
    { 12000, 0, 0, 1, 1, LPC_MODE }   // sub
#endif  // !defined (V7_CONFIG) 
};


static const WMARateEntry gWma2RateList16000s[] = { // 640ms per packet, or 1.5625 packets per second.
#if !defined (V7_CONFIG) // increase # packets per buffer
    { 20000, 32, 1, 10, 8, LPC_MODE }, // In encoder, we produce max 2 subframes in this config.
    { 16000, 32, 1, 10, 4, LPC_MODE }  // In encoder, we produce max 2 subframes in this config.
#else
    { 20000, 32, 1, 14, 8, LPC_MODE }, // sub
    { 16000, 32, 1, 14, 4, LPC_MODE }  // sub
#endif
};

static const WMARateEntry gWma2RateList22050s[] = { 
    { 32000, 32, 1, 8, 8, BARK_MODE }, // sub // 371.5ms per packet, or 2.69 packets per second. 
#if !defined (V7_CONFIG)  // increase # packets per buffer
    { 22000, 32, 1, 8, 4, BARK_MODE },// sub // 650ms per packet, or 1.538 packets per second. 
    { 20000, 32, 1, 8, 4, BARK_MODE } // sub // 650ms per packet, or 1.538 packets per second. 
#else
    { 22000, 32, 1, 14, 4, LPC_MODE },// sub // 650ms per packet, or 1.538 packets per second. 
    { 20000, 32, 1, 14, 4, LPC_MODE } // sub // 650ms per packet, or 1.538 packets per second. 
#endif
};

static const WMARateEntry gWma2RateList32000s[] = { // 384ms per packet, or 2.60 packets per second.
#if !defined (V7_CONFIG)
    { 48000, 32, 1, 6, 16, BARK_MODE}, // no-sub: addl subframes found useful 10/6/00
#else
    { 48000, 32, 1, 6, 8, BARK_MODE}, // sub 
#endif // !defined (V7_CONFIG)
    { 40000, 32, 1, 6, 8, BARK_MODE }, // sub
    { 32000, 32, 1, 6, 8, BARK_MODE }  // sub
};

static const WMARateEntry gWma2RateList44100s[] = { // 371.5ms per packet, or 2.69 packets per second
    { 192000, 32, 1, 8, 16, BARK_MODE }, // no-sub
    { 160000, 32, 1, 8, 16, BARK_MODE }, // no-sub
    { 128000, 32, 1, 8, 16, BARK_MODE }, // no-sub
    {  96000, 32, 1, 8, 16, BARK_MODE }, // no-sub
    {  80000, 32, 1, 8, 16, BARK_MODE }, // no-sub
    {  64000, 32, 1, 8, 16, BARK_MODE }, // no-sub
    {  48000, 64, 1, 8, 16, BARK_MODE }, // addl buffer to have maximum freedom
    {  32000, 64, 1, 8,  8, BARK_MODE },  // addl buffer to have maximum freedom
};

static const WMARateEntry gWma2RateList48000s[] = { // 341ms per packet, or 2.93 packets per second
    { 192000, 32, 1, 8, 16, BARK_MODE }, // no-sub
    { 160000, 32, 1, 8, 16, BARK_MODE }, // no-sub
    { 128000, 32, 1, 8, 16, BARK_MODE }  // no-sub
};

static const WMARateList gWma2RateList[] = {
    {    8000, 1, SIZEOF_ARRAY(gWma2RateList8000m),  &gWma2RateList8000m[0] }
    , {  8000, 2, SIZEOF_ARRAY(gWma2RateList8000s),  &gWma2RateList8000s[0] }
    , { 11025, 1, SIZEOF_ARRAY(gWma2RateList11025m), &gWma2RateList11025m[0] }
    , { 11025, 2, 0/*SIZEOF_ARRAY(gWma2RateList11025s)*/, NULL/*&gWma2RateList11025s[0]*/ }
    , { 16000, 1, SIZEOF_ARRAY(gWma2RateList16000m), &gWma2RateList16000m[0] }
    , { 16000, 2, SIZEOF_ARRAY(gWma2RateList16000s), &gWma2RateList16000s[0] }
    , { 22050, 1, SIZEOF_ARRAY(gWma2RateList22050m), &gWma2RateList22050m[0] }
    , { 22050, 2, SIZEOF_ARRAY(gWma2RateList22050s), &gWma2RateList22050s[0] }
    , { 32000, 1, SIZEOF_ARRAY(gWma2RateList32000m), &gWma2RateList32000m[0] }
    , { 32000, 2, SIZEOF_ARRAY(gWma2RateList32000s), &gWma2RateList32000s[0] }
    , { 44100, 1, SIZEOF_ARRAY(gWma2RateList44100m), &gWma2RateList44100m[0] }
    , { 44100, 2, SIZEOF_ARRAY(gWma2RateList44100s), &gWma2RateList44100s[0] }
    , { 48000, 1, 0/*SIZEOF_ARRAY(gMwa2RateList48000m)*/, NULL/*&gWma2RateList48000m[0]*/ }
    , { 48000, 2, SIZEOF_ARRAY(gWma2RateList48000s), &gWma2RateList48000s[0] }
};

////////////////////////////////////////////////////////////////////////
// Helper for finding the right PCM list in the above table
////////////////////////////////////////////////////////////////////////
const WMARateList *
findRateListByPCMParams(U32 nSamplingRate,
                        U32 nChannels)
{
   const WMARateList* pRateList = gWma2RateList;
   U32 nRateLists = SIZEOF_ARRAY(gWma2RateList);
   U32 i;

   for(i = 0; i < nRateLists; i++) {
      if(pRateList[i].nSamplingRate == nSamplingRate &&
         pRateList[i].nChannels == nChannels)
           return pRateList[i].pRates ? &pRateList[i] : NULL;
   }
   return NULL;
}


////////////////////////////////////////////////////////////////////////
// Logic to compupte recommended encoder settings
////////////////////////////////////////////////////////////////////////
static U16 prvEncOptionsBitMask (WeightMode wtMode,
                                 Bool bUseSuperFrame,
                                 Bool bUseSubFrame,
                                 U32 nMaxSubFrameDiv,
                                 Bool bLowBitRate) {
    U16 wEncodeOptions;
    
    if (wtMode == BARK_MODE) 
        wEncodeOptions = 1;
    else                  
    {
        //per discussion with serges, mingcl. we are ok to use 20 now.
#if !defined (V7_CONFIG)
        wEncodeOptions = 0x20;
#else
        wEncodeOptions = 0x00;
#endif
    }
    
    // for V2 and above?
    if(bUseSuperFrame)      // bit 1: fAllowSuperFrame
        wEncodeOptions |= 0x2;
    
    if(bUseSubFrame) {
        
        // bit 2: fAllowSubFrame, only valide when fAllowsuperFrame==TRUE
        wEncodeOptions |= 0x4;
        
        // bit 3-4: cSubFrames in a Frame, only valid when fAllowSubFrame==TRUE
        //   for >= 32 kbps / channel : 00 = 8, 01 = 16, 10 = 32, 11 = 64
        //                              (always default to 16 for now)
        //   for <  32 kbps / channel : 00 = 2, 01 = 4 , 10 = 8, 11 = 16
        //                              (not used for now since fAllowSubFrame
        //                               should always be false for this case)
        if (!bLowBitRate) {
            assert(nMaxSubFrameDiv >= 8 && nMaxSubFrameDiv <= 64);
            wEncodeOptions |= (LOG2(nMaxSubFrameDiv) - 3) << 3;
        }
        else {
            assert(nMaxSubFrameDiv >= 2 && nMaxSubFrameDiv <= 16);
            wEncodeOptions |= (LOG2(nMaxSubFrameDiv) - 1) << 3;
        }
    }
    
    return wEncodeOptions;
}

U16 prvEncodeOptions(U32 nSamplesPerSec,
                     U32 nChannels,
                     const WMARateEntry* pEntry) {
    return prvEncOptionsBitMask(pEntry->wtMode,
        pEntry->fAllowSuperFrame,
        pEntry->fAllowSuperFrame,
        pEntry->nMaxSubFrameDiv,
        pEntry->nBitrate / nChannels < 32000);
}

/////////////////////////
//                     //
//   Frame size math   //
//                     //
/////////////////////////

// returns a * b / c, rounding up
U32 MyMulDiv(U32 a, U32 b, U32 c) {
    // first cancel out some 2's to reduce the risk of overflows
    do {
        U32 tmpA = a & 1;
        U32 tmpB = b & 1;
        U32 tmpC = c & 1;
        if (tmpC) // c is no longer even - done
            break;
        if (!tmpA) // a is still even
            a >>= 1;
        else if (!tmpB) // b is still even
            b >>= 1;
        else // neither a nor b is even - done
            break;
        c >>= 1;
    } while (1);
    
    return (a * b + c - 1) / c;
}

//
// Define a common way of rounding things in various computations.
//

// returns the approximate average number of compressed bits for n samples
U32 SamplesToBits(U32 cSamples, U32 ulSamplingRate, U32 ulBitrate) {
   return MyMulDiv(ulBitrate, cSamples, ulSamplingRate);
}
U32 BitsToBytes(U32 cBits) {
    return (cBits + 7) / 8;
}

U32 prvSamplesPerPacket(U32 ulSamplingRate,
                        U32 ulChannels,
                        U32 ulBitrate,
                        U32 ulFramesPerPacket) {
   return ulFramesPerPacket * msaudioGetSamplePerFrame(ulSamplingRate, ulBitrate, ulChannels, 2);
}

//
// Picks a packet size that will give us approximately the desired bitrate
//
U32 prvBytesPerPacket(U32 ulSamplingRate,
                      U32 ulChannels,
                      U32 ulBitrate,
                      U32 ulFramesPerPacket) {
    U32 cSamplesPerPacket, cbPacket;
    if (ulBitrate == 128) return 1; // silence mode
    cSamplesPerPacket = prvSamplesPerPacket(ulSamplingRate, ulChannels, ulBitrate, ulFramesPerPacket);
    cbPacket = BitsToBytes(SamplesToBits(cSamplesPerPacket,
                                         ulSamplingRate,
                                         ulBitrate));
    
    // 32_44_2: bring nAvgBytesPerSec below 4000 to prevent ARM decoder crash
    if (ulSamplingRate == 44100 && ulChannels == 2 && ulBitrate == 32000) {
      // predict nAvgBytesPerSec by simulating the prvActualBitrate() logic
      while (BitsToBytes(MyMulDiv(8 * cbPacket,
                                  ulSamplingRate,
                                  cSamplesPerPacket)) >= 4000) {
        cbPacket--;
      }
    }
    return cbPacket;
}

//
// Returns the average bitrate according to the formula
//
//                 bits per packet  *  sampling rate
//    bitrate = -------------------------------------
//                       samples per packet
//
// The value returned by this function is accurate to within 1 bit per second
//
U32 prvActualBitrate(U32 ulSamplingRate,
                     U32 ulChannels,
                     U32 ulBitrate,
                     U32 ulFramesPerPacket) {
    return MyMulDiv(8 * prvBytesPerPacket(ulSamplingRate, ulChannels, ulBitrate, ulFramesPerPacket),
                    ulSamplingRate,
                    prvSamplesPerPacket(ulSamplingRate, ulChannels, ulBitrate, ulFramesPerPacket));
}

U32 prvBytesPerSec(U32 ulSamplingRate,
                   U32 ulChannels,
                   U32 ulBitrate,
                   U32 ulFramesPerPacket) {
   return BitsToBytes(prvActualBitrate(ulSamplingRate, ulChannels, ulBitrate, ulFramesPerPacket));
}

U32 prvPseudoSuperframeSamples(U32 ulSamplingRate,
                               U32 ulChannels,
                               U32 ulBitrate,
                               U32 ulFramesPerPacket,
                               U32 nBufLengthSamples) {
    
    // round down to the nearest multiple of whole packets
    U32 cSamplesPerPacket = prvSamplesPerPacket(ulSamplingRate, ulChannels, ulBitrate, ulFramesPerPacket);
    return nBufLengthSamples / cSamplesPerPacket * cSamplesPerPacket;
}

U32 prvPseudoSuperframeSize(U32 ulSamplingRate,
                            U32 ulChannels,
                            U32 ulBitrate,
                            U32 ulFramesPerPacket,
                            U32 nBufLengthSamples)
{
    // now convert from samples to compressed bytes
    return BitsToBytes(SamplesToBits(prvPseudoSuperframeSamples(ulSamplingRate, ulChannels, ulBitrate, ulFramesPerPacket, nBufLengthSamples),
                                     ulSamplingRate,
                                     prvActualBitrate(ulSamplingRate, ulChannels, ulBitrate, ulFramesPerPacket)));
}
/////////////////////////////
//                         //
//   End frame size math   //
//                         //
/////////////////////////////


//
// Proper ASF A/V interleaving requires at least a certain number
// of packets per second (about 3 for bitrates below 32kbps and
// about 5 for 32kbps and higher).  Our preferred packet sizes
// in the table above do not meet these requirements for sampling
// rates >= 16kHz.  However, we want to keep it that way for
// audio-only scenarios.
//
// The solution we use is to enumerate two separate format entries
// for any format whose default nFramesPerPacket does not meet the
// ASF AV interleave requirements.  One format (enumerated first)
// has the normal (preferred for audio-only) packet size.  The
// other, enumerated later, has a smaller packet size, suitable
// for ASF AV Interleave.
//
// These functions abstract the logic of choosing which formats
// need two entries,
//
U32 prvFramesPerPacketForASFAVInterleave(U32 ulSamplingRate, U32 ulChannels, const WMARateEntry* pEntry) {
    U32 ulRequiredPacketsPerSec = (pEntry->nBitrate >= 32000) ? 5 : 3;
    U32 ulSamplesPerFrame = msaudioGetSamplePerFrame(ulSamplingRate, pEntry->nBitrate, ulChannels, 2);
    if (ulSamplingRate / (pEntry->nFramesPerPacket * ulSamplesPerFrame) >= ulRequiredPacketsPerSec)
        return pEntry->nFramesPerPacket; // no need to have a special mode
    else // compute the special value
        return ulSamplingRate / (ulRequiredPacketsPerSec * ulSamplesPerFrame);
}

// If the default frames-per-packet from this entry does not work for
// AV interleave then we will have a second version of this format that does.
Bool prvIsSpecialForASFAVInterleave(U32 ulSamplingRate, U32 ulChannels, const WMARateEntry* pEntry) {
    if (pEntry->nFramesPerPacket !=
        prvFramesPerPacketForASFAVInterleave(ulSamplingRate,
                                          ulChannels,
                                          pEntry)) {
        return WMAB_TRUE;
    }
    else
        return WMAB_FALSE;
}


// Calls various functions and stuffs the results into a single struct
void prvInitializeFormat(WMAFormatInfo* pFormat,
                         U32 ulSamplingRate,
                         U32 ulChannels,
                         const WMARateEntry* pEntry,
                         Bool fSpecialASFAVInterleave) {
    U32 ulBitrate = pEntry->nBitrate;
    U32 nSamplesPerFrame = msaudioGetSamplePerFrame(ulSamplingRate, ulBitrate, ulChannels, 2);
    U32 nFramesPerPacket;
    if (fSpecialASFAVInterleave)
        nFramesPerPacket = prvFramesPerPacketForASFAVInterleave(ulSamplingRate, ulChannels, pEntry);
    else
        nFramesPerPacket = pEntry->nFramesPerPacket;

    // base WAVEFORMATEX
    pFormat->nSamplesPerSec  = ulSamplingRate;
    pFormat->nChannels       = ulChannels;
    pFormat->nAvgBytesPerSec = prvBytesPerSec(ulSamplingRate, ulChannels, ulBitrate, nFramesPerPacket);
    pFormat->nBlockAlign     = prvBytesPerPacket(ulSamplingRate, ulChannels, ulBitrate, nFramesPerPacket);
    
    pFormat->nLookaheadSamples    = pEntry->nFramesLookAhead * nSamplesPerFrame;

    // extended WAVEFORMATEX   
    pFormat->nSamplesPerBlock  = prvSamplesPerPacket(ulSamplingRate, ulChannels, ulBitrate, nFramesPerPacket);
    pFormat->dwSuperBlockAlign = prvPseudoSuperframeSize(ulSamplingRate, ulChannels, ulBitrate, nFramesPerPacket, pFormat->nLookaheadSamples);
    pFormat->wEncodeOptions    = prvEncodeOptions(ulSamplingRate, ulChannels, pEntry);

    // miscellaneous
    pFormat->nFramesPerPacket     = nFramesPerPacket;
    pFormat->nSamplesPerFrame     = nSamplesPerFrame;
    pFormat->nMaxSamplesPerPacket = MaxSamplesPerPacket(2, ulSamplingRate, ulChannels, ulBitrate);
    pFormat->nSuperFrameSamples   = prvPseudoSuperframeSamples(ulSamplingRate, ulChannels, ulBitrate, nFramesPerPacket, pFormat->nLookaheadSamples);
    pFormat->ulOfficialBitrate    = ulBitrate;
}

//
// "Special" formats are the versions modified adapted for ASF AV interleave.
// "Normal" formats are the ones listed in the table.
//
U32 prvNormalFormatCount() {
    static U32 cRet;
    static int fAlreadyComputed = 0;
    
    if (!fAlreadyComputed) {
        const WMARateList* pList = gWma2RateList;
        U32 cList = SIZEOF_ARRAY(gWma2RateList), nList;
        cRet = 0;
        for (nList = 0; nList < cList; nList++)
            cRet += pList[nList].nEntries;
        fAlreadyComputed = 1;
    }
    return cRet;
}
U32 prvSpecialFormatCount() {
    static U32 cRet;
    static int fAlreadyComputed = 0;
    
    if (!fAlreadyComputed) {
        const WMARateList* pList = gWma2RateList;
        U32 cList = SIZEOF_ARRAY(gWma2RateList), nList;
        cRet = 0;
        for (nList = 0; nList < cList; nList++) {
            U32 nEntry;
            for (nEntry = 0; nEntry < pList[nList].nEntries; nEntry++) {
                if (prvIsSpecialForASFAVInterleave(pList[nList].nSamplingRate,
                                                   pList[nList].nChannels,
                                                   &(pList[nList].pRates[nEntry]))) {
                    cRet++;
                }
            }
        }
        fAlreadyComputed = 1;
    }
    return cRet;
}
////////////////////////////////////////////////////////////////////////
//
// public entry points for enumerating and checking supported formats
//
////////////////////////////////////////////////////////////////////////

Bool GetPCMParamsByIndex(U32 ulIndex, U32* pulSamplingRate, U32* pulChannels) 
{
    const WMARateList* pList = gWma2RateList;
    U32 cList = SIZEOF_ARRAY(gWma2RateList);
    U32 cEntry = 0, cValid = 0;

   while (cEntry < cList) {
      if (pList[cEntry].nEntries) {
         if (cValid == ulIndex) {
            *pulSamplingRate = pList[cEntry].nSamplingRate;
            *pulChannels = pList[cEntry].nChannels;
            return WMAB_TRUE;
         }
         cValid++;
      }
      cEntry++;
   }
   return WMAB_FALSE;
}

Bool CheckPCMParams(U32 ulSamplingRate, U32 ulChannels) {
   if (findRateListByPCMParams(ulSamplingRate, ulChannels))
      return WMAB_TRUE;
   else
      return WMAB_FALSE;
}


U32 TotalFormatCount(Bool fIncludeASFAVInterleaveSpecial) {
    if (fIncludeASFAVInterleaveSpecial)
        return prvNormalFormatCount() + prvSpecialFormatCount();
    else
        return prvNormalFormatCount();
}

Bool GetFormatByIndex(U32 ulIndex, Bool fIncludeASFAVInterleaveSpecial, WMAFormatInfo *pFormat) {
    const WMARateList* pRateList = gWma2RateList;
    U32 nRateLists = SIZEOF_ARRAY(gWma2RateList);
    U32 iList;
    const WMARateEntry* pEntry = NULL;
    
   //
   // This translates an external index (where there are two positions for
   // every special format) into an internal one (where any every format
   // has just one position.  Also assigns the "special" flag based on the
   // external index.
   //
   // The way this mapping works is somewhat arbitrary.  We could have also
   // placse the peferred and special versions of each special format next
   // to each other.  I chose to put all the special versions at the end,
   // so that if a caller does not require the special version they may
   // never even get to the special section in the enumeration process.
   //
   Bool fSpecialASFAVInterleave = WMAB_FALSE;
   if (ulIndex >= prvNormalFormatCount() + prvSpecialFormatCount())
       return WMAB_FALSE;

   if (ulIndex >= prvNormalFormatCount()) {
       if (!fIncludeASFAVInterleaveSpecial)
           return WMAB_FALSE;
       fSpecialASFAVInterleave = WMAB_TRUE;
       ulIndex -= prvNormalFormatCount();
   }
   
   //
   // Now find the nth entry in our tables
   //
   for(iList = 0; iList < nRateLists; iList++) {
       if (fSpecialASFAVInterleave) {
           // go into the rate list and count the special entries
           U32 iRate;
           for (iRate = 0; iRate < pRateList[iList].nEntries; iRate++) {
               if (prvIsSpecialForASFAVInterleave(pRateList[iList].nSamplingRate,
                                                  pRateList[iList].nChannels,
                                                  &(pRateList[iList].pRates[iRate]))) {
                   if (ulIndex == 0) { // this is it
                       pRateList = &(pRateList[iList]);
                       pEntry = &(pRateList->pRates[iRate]);
                       goto found;
                   }
                   ulIndex--;
               }
           }
       }
       else { // skip whole lists based on their format count
           if(ulIndex < pRateList[iList].nEntries) {
               pRateList = &(pRateList[iList]);
               pEntry = &(pRateList->pRates[ulIndex]);
               goto found;
           }
           ulIndex -= pRateList[iList].nEntries;
       }
   }

found:
   if (!pEntry) {
       assert(!"bug in GetFormatByIndex or GetFormatCount");
       return WMAB_FALSE;
   }

   prvInitializeFormat(pFormat, pRateList->nSamplingRate, pRateList->nChannels, pEntry, fSpecialASFAVInterleave);
   return WMAB_TRUE;
}

Bool GetFormatByPCMParamsAndIndex(U32 ulIndex, U32 ulSamplingRate, U32 ulChannels, Bool fIncludeASFAVInterleaveSpecial, WMAFormatInfo* pFormat) {
   const WMARateList* pRateList = findRateListByPCMParams(ulSamplingRate, ulChannels);
   U32 nEntry, nSpecial;
   
   if (!pRateList)
      return WMAB_FALSE;
   
   if (ulIndex < pRateList->nEntries) { // normal
       prvInitializeFormat(pFormat, ulSamplingRate, ulChannels, &(pRateList->pRates[ulIndex]), WMAB_FALSE);
       return WMAB_TRUE;
   }

   if (!fIncludeASFAVInterleaveSpecial)
       return WMAB_FALSE;

   // ASFAVInterleave special - if it exists
   ulIndex -= pRateList->nEntries;
   // Find the nth special entry
   for (nEntry = 0, nSpecial = 0; nEntry < pRateList->nEntries; nEntry++) {
       if (prvIsSpecialForASFAVInterleave(ulSamplingRate, ulChannels, &(pRateList->pRates[ulIndex]))) {
           if (nSpecial == ulIndex) { // found the beast !
               prvInitializeFormat(pFormat, ulSamplingRate, ulChannels, &(pRateList->pRates[ulIndex]), WMAB_TRUE);
               return WMAB_TRUE;
           }
           nSpecial++;
       }
   }
   // not found
   return WMAB_FALSE;
}

const WMARateEntry* NearestRateEntry(U32 ulSamplingRate, U32 ulChannels, U32 ulBitrateWanted) {
   const WMARateList* pRateList = findRateListByPCMParams(ulSamplingRate, ulChannels);
   
   // Find the nearest bitrate entry.  Bitrates are always enumerated in descending order.
   U32 iBitrateIndex = 0;
   U32 uiMinDiff = 0xFFFFFFFF;
   I32 iBestMatch = -1;
   
   if (!pRateList)
      return WMAB_FALSE;
   
   for (iBitrateIndex = 0; iBitrateIndex < pRateList->nEntries; iBitrateIndex++) {
       U32 ulBitrate = pRateList->pRates[iBitrateIndex].nBitrate;
       U32 uiDiffTmp = abs ((I32)ulBitrateWanted - (I32)ulBitrate);
       if (uiDiffTmp == 0) {
           iBestMatch = iBitrateIndex;
           break;
       }
       if (uiMinDiff > uiDiffTmp) {
           uiMinDiff = uiDiffTmp;
           iBestMatch = iBitrateIndex;
       }
   }
   if (iBestMatch == -1) // nothing even close
       return NULL;
   return &(pRateList->pRates[iBestMatch]);
}
   
Bool GetFormatByPCMParamsAndBitrate(U32 ulSamplingRate, 
                                    U32 ulChannels, 
                                    U32 ulBitrateWanted, Bool fASFAVInterleave, WMAFormatInfo* pFormat) {
   const WMARateEntry* pEntry = NearestRateEntry(ulSamplingRate, ulChannels, ulBitrateWanted);
   if (!pEntry) return WMAB_FALSE;
   prvInitializeFormat(pFormat, ulSamplingRate, ulChannels, pEntry, fASFAVInterleave);
   return WMAB_TRUE;
}



//***************************************************************************
// Function: GetTranscodeDstFormatByIndex
//
// Purpose:
//   Enumerate the destination formats which are valid for the given
//   source format.
//
// Arguments:
//   WMAFormatInfo *pSrcFormat [in] - the source format whose destinations
//     we'd like to enumerate.
//   U32 ulDstIndex [in] - an index (0 is first) provided by the caller.
//     If the caller has not found what he wants he keeps advancing the
//     index until we give him what he wants or we return WMAB_FALSE,
//     indicating that there is no valid destination format for the given
//     index.
//   WMAFormatInfo *pDstFormat [out] - we return a valid destination format
//     for pSrcFormat here.
//
// Returns:
//   WMAB_TRUE if there exists a valid destination format for the given
//   pSrcFormat and ulDstIndex. WMAB_FALSE otherwise.
//***************************************************************************
Bool GetTranscodeDstFormatByIndex(const WMAFormatInfo* pSrcFormat,
                                  const U32 ulDstIndex,
                                  WMAFormatInfo* pDstFormat)
{
   const WMARateList* pSrcRateList =
       findRateListByPCMParams(pSrcFormat->nSamplesPerSec, pSrcFormat->nChannels);
   const I32 iSrcBitrate = pSrcFormat->nAvgBytesPerSec * 8;

   I32  iBestMatch;
   U32  iBitrateIndex;
   U32  uiMinDiff;
   Bool fDstFound;
   I32  iDstCount;

   // For now we have only tested 44kHz. As we test and add more, this function
   // will return more supported formats.
   if ((pSrcFormat->nSamplesPerSec != 44100) || (pSrcFormat->wEncodeOptions != 0x0F))
       return WMAB_FALSE;

   // Current rules for transcoder:
   // Source and dest must be same sample rate, number of channels and cSubband
   // Source and dest must be both bark band weighting mode, superframe mode.

   // Find position of source format in rate list
   uiMinDiff = 0xFFFFFFFF;
   iBestMatch = -1;
   for (iBitrateIndex = 0; iBitrateIndex < pSrcRateList->nEntries; iBitrateIndex++)
   {
       const U32 ulBitrate = pSrcRateList->pRates[iBitrateIndex].nBitrate;
       const U32 uiDiffTmp = abs (iSrcBitrate - (I32)ulBitrate);

       if (uiDiffTmp == 0)
       {
           iBestMatch = iBitrateIndex;
           break;
       }
       if (uiMinDiff > uiDiffTmp)
       {
           uiMinDiff = uiDiffTmp;
           iBestMatch = iBitrateIndex;
       }
   }

   if (-1 == iBestMatch)
       return WMAB_FALSE; // Couldn't find the source in our list

   // Now count the candidate output formats
   fDstFound = WMAB_FALSE;
   iBitrateIndex = iBestMatch + 1;  // Transcoder doesn't do same-to-same. Start with next one.
   iDstCount = ulDstIndex;
   while (iBitrateIndex < pSrcRateList->nEntries)
   {
       WMAFormatInfo rDstCandidate;

       prvInitializeFormat(&rDstCandidate, pSrcFormat->nSamplesPerSec,
           pSrcFormat->nChannels, &(pSrcRateList->pRates[iBitrateIndex]), WMAB_FALSE);
       if (0 == (rDstCandidate.wEncodeOptions & ENCOPT_BARK) ||
           0 == (rDstCandidate.wEncodeOptions & ENCOPT_SUPERFRAME))
       {
           // We've reached the end of the line, terminate the search
           break;
       }

       if (iDstCount <= 0)
       {
           // Found it!
           if (NULL != pDstFormat)
               *pDstFormat = rDstCandidate;

           fDstFound = WMAB_TRUE;
           break;
       }

       iDstCount -= 1;
       iBitrateIndex += 1;
   }

   return fDstFound;
}


