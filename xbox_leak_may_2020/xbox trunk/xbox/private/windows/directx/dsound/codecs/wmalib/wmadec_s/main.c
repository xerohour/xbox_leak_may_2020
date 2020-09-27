/* test.c */

#pragma code_seg("WMADEC")
#pragma data_seg("WMADEC_RW")
#pragma const_seg("WMADEC_RD")

#pragma comment(linker, "/merge:WMADEC_RW=WMADEC")
#pragma comment(linker, "/merge:WMADEC_RD=WMADEC")
#pragma comment(linker, "/section:WMADEC,ERW")

#pragma warning ( disable : 4103 )
#include <malloc.h> // for heapcheck
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <tchar.h>
#include "wmaudio.h"
#include "assert.h"
#include "AutoProfile.h"
#include "..\..\..\dsound\dsndver.h"

// Define MEASURE_CLOCK for top level timing
//#define MEASURE_CLOCK

// AUTOPC_STYLE has a very large buffer and calls our routines to repeatedly fill it up
//#define AUTOPC_STYLE

// Define DISCARD_OUTPUT for profiling.
#if defined(PROFILE) || defined(MEASURE_CLOCK)
// this must be commented out if you really want output
//#define DISCARD_OUTPUT
#endif

// comment out DUMP_WAV define below to output a raw .pcm file rather than a .wav file.
#define DUMP_WAV
#ifdef DUMP_WAV
#include "wavfileio.h"
#endif  /* DUMP_WAV */

// Obnoxious Tests
// un-comment out the define below to enable some modestly obnoxious tests for the interface (output should still be OK)
//#define OBNOXIOUS
// un-comment out the next define to enable interface tests which will not generate the correct output
//#define REAL_OBNOXIOUS

//#define _MARKERDEMO_
//#define TEST_SPEED

//#define TEST_MARKER

// WMA2CMP - define in project settings to produce a .cmp output file from the .wma suitable for use with decapp
#ifdef WMA2CMP
#   ifndef WMAAPI_NO_DRM
#       define WMAAPI_NO_DRM
#   endif
#   include "wmaudio_type.h"
#   include "msaudio.h"
    FILE    *pfWma2Cmp = NULL;       // file (and flag) to output .cmp file
    // typically you would rename the execuable of this WMA2CMP build as Wma2Cmp.exe when moving it into your path
#endif


// HEAP_DEBUG_CHECK is the same as the defines in msaudio.h 
// Although it is normally bad form to copy something out a .h instead of 
// including the .h, in this test program, we want to make sure we do not 
// inadvertently use anything from msaudio.h 
#if defined(HEAP_DEBUG_TEST) && defined(_DEBUG)
void HeapDebugCheck();
#define HEAP_DEBUG_CHECK HeapDebugCheck()
#else
#define HEAP_DEBUG_CHECK
#endif

//#define PRINT_FROM_SAMPLE 67966
//#define PRINT_TO_SAMPLE   68228
//#define PRINT_FROM_SAMPLE 66000
//#define PRINT_TO_SAMPLE   68228
#if defined(PRINT_FROM_SAMPLE) && defined(PRINT_TO_SAMPLE)
    // One of the DCT print defines must be defined in fft.c to get this
    extern int bPrintDctAtFrame;	// used to print coefs before and after DCT
#endif


/* global */

tWMAFileHdrState g_hdrstate;
tHWMAFileState g_state;
tWMAFileHeader g_hdr;
tWMAFileLicParams g_lic;

#ifndef TEST_SPEED
FILE *g_fp = NULL;
FILE *g_fpLic = NULL;
#endif /* TEST_SPEED */

#ifdef TEST_SPEED
unsigned char *g_pBuffer = NULL;
#else  /* TEST_SPEED */
const int MAX_BUFSIZE = WMA_MAX_DATA_REQUESTED;
unsigned char g_pBuffer[WMA_MAX_DATA_REQUESTED];
#endif /* TEST_SPEED */
tWMA_U32 g_cbBuffer = 0;

#ifdef TEST_SPEED
#else  /* TEST_SPEED */
unsigned char g_pBufLic[WMA_MAX_DATA_REQUESTED];
#endif /* TEST_SPEED */
tWMA_U32 g_cbBufLic = 0;

unsigned long g_ulStartFirstSec, g_ulEndSec;
unsigned long g_ulFullSec = 0;
unsigned long g_ulOutputSamples = 0;
unsigned int  g_SampleRate;

/* Portable Media ID
 *
 * This is different for each portable medium (PM).  Should be
 * provided by the application by reading an identification from
 * the PD
 */


unsigned char g_pmid[20] =
{
    // Jonas
//    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x4D, 0x20,
//    0x33, 0x54, 0x31, 0x30, 0x31, 0x33, 0x37, 0x32, 0x37, 0x33
// Robert
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x69, 0x20,
  0x32, 0x30, 0x35, 0x30, 0x30, 0x32, 0x30, 0x31, 0x32, 0x31,
//    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
//    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x02, 0x03, 0x04,
};

/*
//train 
unsigned char g_pmid[20] =
{
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x4d, 0x20,
    0x33, 0x54, 0x31, 0x30, 0x30, 0x35, 0x30, 0x30, 0x39, 0x35
};
*/

/*
unsigned char g_pmid[20] =
{ 
    0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0x55, 0xef, 
    0xee, 0xee, 0xef, 0xef, 0xef, 0xef, 0xef, 0xdf, 0xdf, 0xd9
};
*/

// This should work either with or without TEST_INTERLEAVED_DATA
//   TEST_INTERLEAVED_DATA should be more efficient for all platforms except perhaps ARM.
//   but some applications want separate right and left channel data returned.
// One should check-in with TEST_INTERLEAVED_DATA defined 
//   but run appropriate tests both with and without TEST_INTERLEAVED_DATA


#if defined(AUTOPC_STYLE)
// This style uses a large buffer and repeatedly calls our interface to fill it up
#define MAX_SAMPLES 132000
#define TEST_INTERLEAVED_DATA

#else
// This style is like our sample programs
// should work with any buffer size.  No point in it being larger than 2048.
//#define MAX_SAMPLES 256
#define MAX_SAMPLES 2048
#define TEST_INTERLEAVED_DATA

#endif

#ifdef TEST_INTERLEAVED_DATA
    short g_pLeft [MAX_SAMPLES * 2];
    short *g_pRight = NULL;
#else
    short g_pLeft [MAX_SAMPLES];
    short g_pRight [MAX_SAMPLES];
#   ifdef DUMP_WAV
        // this combo not supported below
#       undef DUMP_WAV
#   endif
#endif

#define STRING_SIZE 128
unsigned char g_szTitle[STRING_SIZE];
unsigned char g_szAuthor[STRING_SIZE];
unsigned char g_szCopyright[STRING_SIZE];
unsigned char g_szDescription[STRING_SIZE];
unsigned char g_szRating[STRING_SIZE];

#if 0
extern "C" void
SerialSendString(char *pcString)
{
    fprintf(stderr, pcString);
}

extern "C" void
SerialPrintf(const char *format, ...)
{
    va_list a;
    char s[256];
    va_start( a, format );
    vsprintf(s, format, a);
    va_end(a);
    SerialSendString(s);
}
#endif /* 0 */

/**************************************************/

void
WStrToStrN(unsigned char *pStr,
           unsigned char *pWStr,
           int n)
{
    if(pStr == NULL || pWStr == NULL)
    {
        return;
    }

    while(n-- > 0)
    {
        *pStr++ = *pWStr;
        pWStr += 2;
    }
    *pStr = 0;
}


//
// georgioc - The callback below is not used. Instead  we redefine it in codecs\wma\wmaxmo.cpp
// since the WmaXMO is the proper interface for reading wma files
//


#if 0

/* WMAFileCBGetData */

tWMA_U32 WMAFileCBGetData (
    tHWMAFileState *state,
    tWMA_U32 offset,
    tWMA_U32 num_bytes,
    unsigned char **ppData)
{
    tWMA_U32 ret;

#ifdef TEST_SPEED

    if(offset >= g_cbBuffer)
    {
        *ppData = g_pBuffer + g_cbBuffer;
        ret = 0;
    }
    else
    {
        *ppData = g_pBuffer + offset;

        if(offset + num_bytes > g_cbBuffer)
        {
            ret = g_cbBuffer - offset;
        }
        else
        {
            ret = num_bytes;
        }
    }

#else  /* TEST_SPEED */

    tWMA_U32 nWanted = num_bytes <= (tWMA_U32) MAX_BUFSIZE ? num_bytes : (tWMA_U32) MAX_BUFSIZE;
    if(num_bytes != nWanted)
    {
        fprintf(stderr, "** WMAFileCBGetData: Requested too much (%lu).\n",
                num_bytes);
    }

    fseek (g_fp, offset, SEEK_SET);
    ret = fread (g_pBuffer, 1, (size_t)nWanted, g_fp);

    g_cbBuffer = ret;

    *ppData = g_pBuffer;

#endif /* TEST_SPEED */

//    fprintf(stderr, "++ WMAFileCBGetData: %lu bytes from %lu.\n", ret, offset);

    return ret;
}

#endif // 0 disable WMAFileCBGetData


// need to provide a platform independent fprintf to stderr for wmaudio.c and wmaudio_parse.c
void WMADebugMessage(const char* pszFmt, ... )
{
    va_list vargs;
    va_start(vargs, pszFmt);
    vfprintf(stderr, pszFmt, vargs );
    va_end(vargs);
}

#if defined(AUTOPC_STYLE)

// AutoPc reads into large 132000 byte buffers using the following code (much simplified)

#define LONG long
#define BYTE unsigned char
//
// llPos   - Starting position to read.
// lLength - Number of bytes to read.
// pBuffer - Ptr to buffer to populate with data.
WMARESULT
AutoPCReadNext(
    LONG lLength,
    short* pBuffer,
    LONG* lWritten)
{

    tWMAFileStatus rc;

    // calculate sample length
    tWMA_U32 sampleLength = (tWMA_U32)lLength / (g_hdr.num_channels << 1);

    short* pLeft = (short*)pBuffer;

    short* pTempLeft = pLeft;

    tWMA_U32 num_samples = 0;

    *lWritten = 0;

    //Keep looping around until no samples are left.
    while(sampleLength)
    {
        do
        {
            num_samples = WMAFileGetPCM(g_state, pTempLeft, NULL, sampleLength);

            *lWritten += num_samples*(g_hdr.num_channels << 1);
            sampleLength -= num_samples;
            pTempLeft += (num_samples * g_hdr.num_channels);
        } while(num_samples);

        if ( sampleLength == 0 )
            break;

        rc = WMAFileDecodeData(g_state);
        if( rc == cWMA_NoMoreFrames )
        {
            return WMA_S_NO_MORE_SRCDATA;
        } else if( rc != cWMA_NoErr )
        {
            return rc;
        }
    }

    return cWMA_NoErr;
}

#endif  // defined(AUTOPC_STYLE)


//
// georgioc - We dont need main since we use this strictly as a LIB
//

#if 0

/* main */

int __cdecl main (int argc, char *argv[])
{
 
    tWMA_U32 msSeekTo;

#ifdef TEST_MARKER
    int k; 
    MarkerEntry pEntry[5];
#endif

    tWMAFileStatus rc;
    tWMA_U32 num_samples;
    int cCountDecodeData = 0;
    int cCountGetPCM = 0;
//    const char *strOut = "d:\\test\\output.pcm";
//    const char *strLic = "drmv1pm.lic";
    const char *strLic = "c:\\ti_test\\files\\drmv1pm.new";
    tWMAFileContDesc desc;
    tWMAExtendedContentDesc *pECDesc;
    unsigned char szTemp [STRING_SIZE];
    int iRV = 1;    // assume error exit return value
#ifndef DISCARD_OUTPUT
#ifdef DUMP_WAV
    WavFileIO *pwfioOut = wfioNew ();
    WAVEFORMATEX wfx;
#else   /* DUMP_WAV */
    FILE *pfOutPCM = NULL;
#endif /* DUMP_WAV */
#endif // !DISCARD_OUTPUT
#ifdef REAL_OBNOXIOUS
    unsigned long ulObnoxiousLoopCount = 0;
#endif
#ifdef MEASURE_CLOCK
    PERFTIMERINFO  *pPerfTimerInfo;
#endif  // MEASURE_CLOCK
    FUNCTION_PROFILE(fpDecode);

    if (argc < 3) {
        fprintf(stderr, "** Too few arguments.\n");
        goto lexit;
    }

    g_fp = fopen (argv [1], "rb");
    if (g_fp == NULL) {
        fprintf(stderr, "** Cannot open %s.\n", argv [1]);
        goto lexit;
    }

    /* init struct */
    memset ((void *)&g_hdrstate, 0, sizeof(g_hdrstate));
    memset ((void *)&g_state, 0, sizeof(g_state));
    memset ((void *)&g_hdr, 0, sizeof(g_hdr));

#ifdef OBNOXIOUS
    // try to close interface which is not yet open
    WMAFileDecodeClose (&g_state);
    // try to get PCM data now
    num_samples = WMAFileGetPCM (g_state, g_pLeft, g_pRight, MAX_SAMPLES);
    if ( num_samples > 0 )
    {
        fprintf( stderr, "** Obnoxious WMAFileGetPCM returns %d samples.\n", num_samples);
        assert( num_samples <= 0 );
    }
    // try to decode data
    rc = WMAFileDecodeData (g_state);
    if ( rc == cWMA_NoErr )
    {
        fprintf( stderr, "** Obnoxious WMAFileDecodeData returns NoErr.\n" );
        assert( rc != cWMA_NoErr );
    }
    // try some other stuff as well
    rc = WMAFileDecodeInfo (g_state, &g_hdr);
    if ( rc == cWMA_NoErr )
    {
        fprintf( stderr, "** Obnoxious WMAFileDecodeInfo returns NoErr.\n" );
        assert( rc != cWMA_NoErr );
    }
    rc = WMAFileContentDesc (g_state, &desc);
    if ( rc == cWMA_NoErr )
    {
        fprintf( stderr, "** Obnoxious WMAFileContentDesc returns NoErr.\n" );
        assert( rc != cWMA_NoErr );
    }
#endif

    /* test the checking API */

    rc = WMAFileIsWMA (&g_hdrstate);
    if(rc != cWMA_NoErr)
    {
        fprintf(stderr, "** The file is not a WMA file.\n");
        goto lexit;
    }

//test Marker 
#ifdef TEST_MARKER
	for ( k = 0 ; k < 5 && k < WMAGetNumberOfMarkers(&g_hdrstate); k++)
	{
		 WMAGetMarker(&g_hdrstate, k, &pEntry[k]); 
	}
#endif 

    /* init the decoder */

    rc = WMAFileDecodeCreate (&g_state);
    if(rc != cWMA_NoErr)
    {
        fprintf(stderr, "** Cannot create the WMA decoder.\n");
        goto lexit;
    }
    rc = WMAFileDecodeInit (g_state);
    if(rc != cWMA_NoErr)
    {
        fprintf(stderr, "** Cannot initialize the WMA decoder.\n");
        goto lexit;
    }

#ifdef OBNOXIOUS
    // Init twice for good measure
    rc = WMAFileDecodeCreate (&g_state);
    if(rc != cWMA_NoErr)
    {
        fprintf(stderr, "** Cannot create the WMA decoder.\n");
        goto lexit;
    }
    rc = WMAFileDecodeInit (g_state);
    if(rc != cWMA_NoErr)
    {
        fprintf(stderr, "** Cannot initialize the WMA decoder.\n");
        goto lexit;
    }
#endif
    
    /* get header information */

    rc = WMAFileDecodeInfo (g_state, &g_hdr);
    if(rc != cWMA_NoErr)
    {
        fprintf(stderr, "** Failed to retrieve information.\n");
        goto lexit;
    }

    /* set up the content description struct */

    memset((void *)&desc, 0, sizeof(desc));
    desc.title_len = STRING_SIZE;
    desc.pTitle    = (unsigned char *)g_szTitle;
    desc.author_len = STRING_SIZE;
    desc.pAuthor    = (unsigned char *)g_szAuthor;
    desc.copyright_len = STRING_SIZE;
    desc.pCopyright    = (unsigned char *)g_szCopyright;
    desc.description_len = STRING_SIZE;
    desc.pDescription    = (unsigned char *)g_szDescription;
    desc.rating_len = STRING_SIZE;
    desc.pRating    = (unsigned char *)g_szRating;

    /* get content description */

    rc = WMAFileContentDesc (g_state, &desc);
    if(rc != cWMA_NoErr)
    {
        fprintf(stderr, "** Failed to retrieve content description.\n");
        goto lexit;
    }

    /* display information */

    WStrToStrN(szTemp, desc.pTitle, desc.title_len);
    printf("++            Song title: %s\n", szTemp);
    WStrToStrN(szTemp, desc.pAuthor, desc.author_len);
    printf("++                Author: %s\n", szTemp);
    WStrToStrN(szTemp, desc.pCopyright, desc.copyright_len);
    printf("++             Copyright: %s\n", szTemp);
    WStrToStrN(szTemp, desc.pDescription, desc.description_len);
    printf("++           Description: %s\n", szTemp);
    WStrToStrN(szTemp, desc.pRating, desc.rating_len);
    printf("++                Rating: %s\n", szTemp);

    rc = WMAFileExtendedContentDesc (g_state, &pECDesc);

    if(rc != cWMA_NoErr)
    {
        fprintf(stderr, "** Failed to retrieve extended content description.\n")
;
        goto lexit;
    }

    printf("++ WMA bitstream version: %d\n", g_hdr.version);
    printf("++         sampling rate: ");
    switch(g_hdr.sample_rate)
    {
    case cWMA_SR_08kHz:
        printf("8000 Hz\n");
        g_SampleRate = 8000;
        break;
    case cWMA_SR_11_025kHz:
        printf("11025 Hz\n");
        g_SampleRate = 11025;
        break;
    case cWMA_SR_16kHz:
        printf("16000 Hz\n");
        g_SampleRate = 16000;
        break;
    case cWMA_SR_22_05kHz:
        printf("22050 Hz\n");
        g_SampleRate = 22050;
        break;
    case cWMA_SR_32kHz:
        printf("32000 Hz\n");
        g_SampleRate = 32000;
        break;
    case cWMA_SR_44_1kHz:
        printf("44100 Hz\n");
        g_SampleRate = 44100;
        break;
    case cWMA_SR_48kHz:
        printf("48000 Hz\n");
        g_SampleRate = 48000;
        break;
    default:
        printf("Unknown??? [%d]\n", g_hdr.sample_rate);
        g_SampleRate = g_hdr.sample_rate;
        break;
    }
    printf("++         # of channels: %d\n", g_hdr.num_channels);
    printf("++              bit-rate: %ld bps\n", g_hdr.bitrate);
    printf("++              duration: %ld ms\n", g_hdr.duration);
    printf("++           DRM content: %s\n", g_hdr.has_DRM ? "Yes" : "No");

    /* if DRM, init with the license file */

    if(g_hdr.has_DRM)
    {
        g_lic.pPMID = (unsigned char *)&g_pmid;
        g_lic.cbPMID = sizeof(g_pmid);

        rc = WMAFileLicenseInit (g_state, &g_lic, CHECK_ALL_LICENSE);
        if(rc != cWMA_NoErr)
        {
            fprintf(stderr, "** WMALicenseInit failed (%u).\n", rc);
            goto lexit;
        }
    }

#ifndef DISCARD_OUTPUT
#ifdef DUMP_WAV
    wfx.wFormatTag      = (USHORT)WAVE_FORMAT_PCM;
    wfx.nSamplesPerSec  = g_SampleRate;
    wfx.nChannels       = (USHORT)g_hdr.num_channels;
    wfx.wBitsPerSample  = 16;
    wfx.nBlockAlign     = ((wfx.wBitsPerSample + 7) / 8) * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
    wfx.cbSize          = 0;

    if(wfioOpen (pwfioOut, argv [2], &wfx, sizeof(wfx), wfioModeWrite) != 0) {
        fprintf(stderr, "Can't create file\n");
        exit(1);
    }
#else   /* DUMP_WAV */
    pfOutPCM = fopen (argv [2], "wb");
    if (pfOutPCM == NULL) {
        fprintf(stderr, "** Cannot open output file %s.\n", argv [2]);
        goto lexit;
    }
#endif // DUMP_WAV
#endif // !DISCARD_OUTPUT

#if defined(WMA2CMP) && defined(DUMP_WAV) && !defined(DISCARD_OUTPUT)
    if (argc > 3) 
    {
	    //setup the output bitstream file in private format
        WAVEFORMATEX  wfxCMP; 
        tWMAFileHdrStateInternal *pInt = (tWMAFileHdrStateInternal *)(&g_hdrstate);

	    if ((pfWma2Cmp = fopen (argv [3], "wb")) == NULL) {
		    fprintf (stderr, "Can't create file %s\n", argv [3]);
		    exit (1);
	    }

        wfxCMP = wfx;
        wfxCMP.nBlockAlign = (U16)(pInt->nBlockAlign);

        if (pInt->nVersion==1)         {
	        wfxCMP.cbSize = sizeof(MSAUDIO1WAVEFORMAT) - sizeof(WAVEFORMATEX);
            fwrite (&wfxCMP, sizeof (U8), sizeof (WAVEFORMATEX), pfWma2Cmp);
            {
                U16 dwStupidNumber = (U16)MaxSamplesPerPacket(1, pInt->nSamplesPerSec, pInt->nChannels, pInt->nAvgBytesPerSec*8);
                fwrite (&dwStupidNumber , sizeof (U16), 1, pfWma2Cmp);    
            }
        } else {
            // not V1 so must be V2 
	        wfxCMP.cbSize = sizeof(WMAUDIO2WAVEFORMAT) - sizeof(WAVEFORMATEX);
            fwrite (&wfxCMP, sizeof (U8), sizeof (WAVEFORMATEX), pfWma2Cmp);
            //should be this; but to be compatible with V4RTM...
            //fwrite (&dwSamplesPerFrame, sizeof (U32), 1, pfWma2Cmp);
            {
                //this is sent but only used by the decoder for some unnecessary
                //computation. We need to fudge this number so that the V4 decoder 
                //can work without any change. The following is the smallest
                //number that can safely fool the old decoder.
                U32 dwStupidNumber = MaxSamplesPerPacket(2, pInt->nSamplesPerSec, pInt->nChannels, pInt->nAvgBytesPerSec*8);
                fwrite (&dwStupidNumber , sizeof (U32), 1, pfWma2Cmp);    
            }
        }

        {
            I16 nEncodeOpt = pInt->nEncodeOpt;
            fwrite (&nEncodeOpt, sizeof (U16), 1, pfWma2Cmp);
        }
        {
            U32 nAvgBytesPerSec = pInt->nAvgBytesPerSec;
            fwrite (&nAvgBytesPerSec, sizeof (U32), 1, pfWma2Cmp);
        }
    }
#endif // WMA2CMP

#if 0
    /* testing the seek */

    {
        tWMA_U32 msSeekTo = 3000;
        tWMA_U32 msReturned = WMAFileSeek(g_state, msSeekTo);
        fprintf(stderr, "++ Seek to %d and actually gotten to %d\n",
                msSeekTo, msReturned);
    }
#endif /* 0 */

    HEAP_DEBUG_CHECK;

    //iMarkerNum = WMAGetMarkers(&g_hdrstate, &pEntry);

    /* decoding loop */
#ifdef MEASURE_CLOCK
    pPerfTimerInfo = PerfTimerNew( g_SampleRate * g_hdr.num_channels );
    if (NULL == pPerfTimerInfo)
    {
        fprintf(stderr, "Could not allocate perf timer structure\n");
        exit(1);
    }
#endif
#ifdef PROFILE
    Profiler_init(_T("profile.txt"));
#endif
#ifdef MEASURE_CLOCK
    PerfTimerStart(pPerfTimerInfo);
#endif  // MEASURE_CLOCK
#ifdef PROFILE
    FUNCTION_PROFILE_START(&fpDecode,MSAC_DECODE_PROFILE);
#endif  // PROFILE
    g_ulStartFirstSec = time(NULL); 


    msSeekTo = 0;

#if defined(AUTOPC_STYLE)
    do
    {
        rc = AutoPCReadNext( MAX_SAMPLES, g_pLeft, &num_samples );
        if ( rc == WMA_S_NO_MORE_SRCDATA )
        {
            iRV = 0;
            break;
        }        
        if ( rc != cWMA_NoErr )
        {   // an error occured 
            iRV = 2;        // error decoding data
            break;
        }
        
#ifndef DISCARD_OUTPUT

#       ifdef DUMP_WAV
            wfioWrite (pwfioOut, (U8*) g_pLeft, num_samples );
#       else   /* DUMP_WAV */
            fwrite (g_pLeft, sizeof (short), num_samples / sizeof(short), pfOutPCM);
#       endif // DUMP_WAV

#endif // !DISCARD_OUTPUT
    } while (1);

#else  // so not AUTOPC_STYLE

    do
    {
        int cGetLoopCount = 0;
#       ifdef OBNOXIOUS
            // try to get PCM data now
            num_samples = WMAFileGetPCM (g_state, g_pLeft, g_pRight, MAX_SAMPLES);
            if ( num_samples > 0 )
            {
                fprintf( stderr, "** Obnoxious WMAFileGetPCM returns %d samples.\n", num_samples);
                assert( num_samples <= 0 );
            }
#       endif

        cCountDecodeData++;
        if ( cCountDecodeData == 74 )
            cGetLoopCount = 0;  // really a place for a breakpint
        rc = WMAFileDecodeData (g_state);

#ifdef _MARKERDEMO_
        if (msSeekTo==0) 
            msSeekTo ++;
        if (msSeekTo == 1) 
        {
            tWMA_U32 msReturned;
            msSeekTo = 2;
            msSeekTo = pEntry[3].m_dwSendTime;
//            msSeekTo = pEntry[1].m_qtime.dwLo/10000;
            msReturned = WMAFileSeek(g_state, msSeekTo);
        }
#endif //_MARKERDEMO_

        if(rc != cWMA_NoErr)
        {
            g_ulEndSec = time(NULL);
            if ( rc == cWMA_NoMoreFrames || rc == cWMA_Failed )
                iRV = 0;        // normal exit
            else
                iRV = 2;        // error decoding data
            break;
        }

        do
        {
            short *pL = g_pLeft;
            short *pR = g_pRight;

#ifdef REAL_OBNOXIOUS
            if ( (++ulObnoxiousLoopCount % 101) == 0 || (ulObnoxiousLoopCount%1009)==0 )
            {
                // skip getting these PCM samples and see if interface recovers
                break;
            } 
#endif
            HEAP_DEBUG_CHECK;
            cCountGetPCM++;
            num_samples = WMAFileGetPCM (g_state, g_pLeft, g_pRight, MAX_SAMPLES);
            if (num_samples == 0)
            {
                /* no more, so on with the decoding... */
                break;
            }
            if ( cGetLoopCount > 0 )
            {
                cGetLoopCount++;  // actually a place for a breakpoint
            }
            cGetLoopCount++;

#ifndef DISCARD_OUTPUT

#   ifdef MEASURE_CLOCK
            PerfTimerStop(pPerfTimerInfo, num_samples * g_hdr.num_channels );
#   endif  // MEASURE_CLOCK
            FUNCTION_PROFILE_STOP(&fpDecode);

#   ifdef TEST_INTERLEAVED_DATA
#       ifdef DUMP_WAV
            wfioWrite (pwfioOut, (U8*) g_pLeft, num_samples * g_hdr.num_channels * sizeof (short));
#       else   /* DUMP_WAV */
            fwrite (g_pLeft, sizeof (short), num_samples * g_hdr.num_channels, pfOutPCM);
#       endif // DUMP_WAV
#   else    // TEST_INTERLEAVED_DATA
            {
                unsigned int i;
                for( i = 0; i < num_samples; i++ ) {
#                   ifdef DUMP_WAV
                        wfioWrite (pwfioOut, (U8*)(g_pLeft+i), sizeof (short));
                        if ( g_hdr.num_channels == 2 )
                            wfioWrite (pwfioOut, (U8*)(g_pRight+i), sizeof (short));
#                   else   /* DUMP_WAV */
                        fwrite (g_pLeft+i, sizeof (short), 1, pfOutPCM);
                        if ( g_hdr.num_channels == 2 )
                            fwrite (g_pRight+i, sizeof (short), 1, pfOutPCM);
#                   endif // DUMP_WAV
                }
            }
#   endif   // TEST_INTERLEAVED_DATA

            FUNCTION_PROFILE_START(&fpDecode,MSAC_DECODE_PROFILE);
#   ifdef MEASURE_CLOCK
            PerfTimerStart(pPerfTimerInfo);
#   endif  // MEASURE_CLOCK

#endif // !DISCARD_OUTPUT

// #define BREAK_AT_SAMPLE 146431 /* tough_16m_16.wma overflow of noise_subst_level in next frame */
//#define BREAK_AT_SAMPLE 50680  /* dire_200k_wmaV1_16k16kHzM.wma unstable frame after this */
#define BREAK_AT_SAMPLE 68000 
#ifdef BREAK_AT_SAMPLE
            if ( g_ulOutputSamples <= BREAK_AT_SAMPLE && BREAK_AT_SAMPLE < (g_ulOutputSamples+num_samples) )
            { int j; j = (int)g_ulOutputSamples; }
#endif //BREAK_AT_SAMPLE

#if defined(PRINT_FROM_SAMPLE) && defined(PRINT_TO_SAMPLE)
            if ( g_ulOutputSamples <= PRINT_FROM_SAMPLE && PRINT_FROM_SAMPLE < (g_ulOutputSamples+num_samples) )
            { bPrintDctAtFrame = 0xff0; }	// flag wma_api.cpp to print coefs
            else if ( g_ulOutputSamples <= PRINT_TO_SAMPLE && PRINT_TO_SAMPLE < (g_ulOutputSamples+num_samples) )
            { bPrintDctAtFrame = 0; }
#endif

            g_ulOutputSamples += num_samples;

        } while (1);

    } while (1);
#endif // not AUTOPC_STYLE

    g_ulFullSec   = g_ulEndSec - g_ulStartFirstSec;
    FUNCTION_PROFILE_STOP(&fpDecode);
#ifdef MEASURE_CLOCK
    PerfTimerStopElapsed(pPerfTimerInfo);
#   ifndef DISCARD_OUTPUT
        PerfTimerStop(pPerfTimerInfo, 0 );
#   else
        PerfTimerStop(pPerfTimerInfo, g_ulOutputSamples * g_hdr.num_channels );
#   endif
#endif  // MEASURE_CLOCK

    HEAP_DEBUG_CHECK;

    fprintf(stderr,"Full Read and Decode took %d s.\r\nPlay Time would be %d s.\r\n",
        g_ulFullSec, g_ulOutputSamples / ( g_SampleRate ) );

#ifdef PROFILE
    {
        char szProfileMsg[400];
        DWORD dwDecodeTime = g_ulFullSec*1000;
#       ifdef MEASURE_CLOCK
            dwDecodeTime = (DWORD)(fltPerfTimerDecodeTime(pPerfTimerInfo)*1000); 
#       endif // MEASURE_CLOCK
        sprintf( szProfileMsg, "%d bps\r\n%d Hz,%d chans\r\n%d ms\r\n%s\r\n%s\r\n%s\r\nTrack = %d\r\nClock = %d\r\nIdle  = %d\r\n\r\n", 
            g_hdr.bitrate, g_SampleRate, g_hdr.num_channels, g_hdr.duration,
            desc.pTitle, desc.pAuthor, desc.pDescription, 
            0, dwDecodeTime, 0
            );
        Profiler_closeEX((unsigned int)(g_hdr.duration),dwDecodeTime,szProfileMsg);
    }
#endif  // PROFILE

#ifdef MEASURE_CLOCK
    PerfTimerReport(pPerfTimerInfo);
    PerfTimerFree(pPerfTimerInfo);
#endif  // MEASURE_CLOCK

#ifdef OBNOXIOUS
    // try to get PCM data now
    num_samples = WMAFileGetPCM (g_state, g_pLeft, g_pRight, MAX_SAMPLES);
    if ( num_samples > 0 )
        fprintf( stderr, "** Obnoxious WMAFileGetPCM returns %d samples.\n", num_samples);
    // try to decode data
    rc = WMAFileDecodeData (g_state);
    if ( rc == cWMA_NoErr )
        fprintf( stderr, "** Obnoxious WMAFileDecodeData returns NoErr.\n" );
#endif

lexit:

    /* clean up */

    HEAP_DEBUG_CHECK;

    if (g_fpLic) {
        fclose (g_fpLic);
        g_fpLic = NULL;
    }

#ifndef DISCARD_OUTPUT
#ifdef DUMP_WAV
    if (pwfioOut)
        wfioDelete (pwfioOut);
#else   /* DUMP_WAV */
    if (pfOutPCM) {
        fclose (pfOutPCM);
        pfOutPCM = NULL;
    }
#endif // DUMP_WAV
#endif // !DISCARD_OUTPUT

    if (g_fp) {
        fclose (g_fp);
        g_fp = NULL;
    }

#ifdef OBNOXIOUS
    // try to get PCM data now
    num_samples = WMAFileGetPCM (g_state, g_pLeft, g_pRight, MAX_SAMPLES);
    if ( num_samples > 0 )
    {
        fprintf( stderr, "** Obnoxious WMAFileGetPCM returns %d samples.\n", num_samples);
        assert( num_samples <= 0 );
    }
#endif

    WMAFileDecodeClose (&g_state);

    HEAP_DEBUG_CHECK;

#ifdef OBNOXIOUS
    // try to close interface a second time
    WMAFileDecodeClose (&g_state);
    // try to get PCM data now
    num_samples = WMAFileGetPCM (g_state, g_pLeft, g_pRight, MAX_SAMPLES);
    if ( num_samples > 0 )
    {
        fprintf( stderr, "** Obnoxious WMAFileGetPCM returns %d samples.\n", num_samples);
        assert( num_samples <= 0 );
    }
    // try to decode data
    rc = WMAFileDecodeData (g_state);
    if ( rc == cWMA_NoErr )
    {
        fprintf( stderr, "** Obnoxious WMAFileDecodeData returns NoErr.\n" );
        assert( rc != cWMA_NoErr );
    }
    // try to close a third time
    WMAFileDecodeClose (&g_state);
#endif

    HEAP_DEBUG_CHECK;
    
    return(iRV);
}


#endif // disable main

// Some compile time warning messages
#ifndef PLATFORM_SPECIFIC_COMPILER_MESSAGE
#   define COMPILER_MESSAGE(x)         message(x)
#endif
#ifdef MEASURE_CLOCK
#   pragma COMPILER_MESSAGE(__FILE__ "(927) : Warning - MEASURE_CLOCK Enabled.")
#endif
#ifdef PROFILE
#   pragma COMPILER_MESSAGE(__FILE__ "(930) : Warning - PROFILE Enabled.")
#endif
#ifdef DISCARD_OUTPUT
#   pragma COMPILER_MESSAGE(__FILE__ "(933) : Warning - DISCARD_OUTPUT Enabled.")
#endif
#ifdef OBNOXIOUS
#   pragma COMPILER_MESSAGE(__FILE__ "(936) : Warning - OBNOXIOUS tests Enabled.")
#endif
#ifdef REAL_OBNOXIOUS
#   pragma COMPILER_MESSAGE(__FILE__ "(939) : Warning - REAL OBNOXIOUS tests Enabled - output will be incorrect.")
#endif
