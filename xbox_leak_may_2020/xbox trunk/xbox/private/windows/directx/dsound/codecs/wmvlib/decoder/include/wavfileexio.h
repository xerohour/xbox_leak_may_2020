/* -------------------------------------------------------------------------
 *
 *  Microsoft Windows Media
 *
 *  Copyright (C) Microsoft Corporation, 1999 - 2000
 *
 *  File:       wavfileexio.h
 *              This is a variation of wavfileio that works with 
 *              WAVEFORMATEXTENSIBLE, rather than with WAVEFORMATEX. 
 *              Deals with both WAVE_FORMAT_PCM and WAVE_FORMAT_EXTENSIBLE 
 *              format types.
 *              It is aware of multi-channel, and > 16 bits/sample.
 *              Updates from wavfileio.h were made by Thumpudi Naveen
 *              1/31/2000.
 *              
 * ---------------------------------------------------------------------- */

#ifndef _WAV_FILE_EX_IO_H_
#define _WAV_FILE_EX_IO_H_

/* use this to turn off the use of mmio* calls */
#define WFIO_NO_WIN32 1

/* use this to control the endian-ness */
#define WFIO_LITTLE_ENDIAN 1

#if WFIO_NO_WIN32

#include <stdio.h>
#include "macros.h"

#ifndef GUID_DEFINED
#define GUID_DEFINED

typedef struct {          // size is 16
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;

#endif // !GUID_DEFINED

#ifndef _WAVEFORMATEXTENSIBLE_
#define _WAVEFORMATEXTENSIBLE_
#ifndef _Embedded_x86
#pragma COMPILER_PACKALIGN(1)
#endif

#ifndef _XBOX
#if !defined (WINMMAPI)
#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
typedef struct 
{
    U16     wFormatTag;        /* format type */
    U16     nChannels;         /* number of channels (i.e. mono, stereo...) */
    U32     nSamplesPerSec;    /* sample rate */
    U32     nAvgBytesPerSec;   /* for buffer estimation */
    U16     nBlockAlign;       /* block size of data */
    U16     wBitsPerSample;    /* Number of bits per sample of mono data */
    U16     cbSize;            /* The count in bytes of the size of
                                * extra information (after cbSize) */
                               
} WAVEFORMATEX, *PWAVEFORMATEX, *LPWAVEFORMATEX;
#endif // _WAVEFORMATEX_
#endif // WINMMAPI
#endif

typedef struct 
{
    WAVEFORMATEX Format;       /* The traditional wave file header */
    union {
        unsigned short wValidBitsPerSample; /* bits of precision */
        unsigned short wSamplesPerBlock;    /* valid if wBitsPerSample==0 */
        unsigned short wReserved;           /* If neither applies, set to zero */
    } Samples;
    unsigned long    dwChannelMask;        /* which channels are present in stream */
    GUID SubFormat;           /* specialization */
} WAVEFORMATEXTENSIBLE, *PWAVEFORMATEXTENSIBLE;
#ifndef _Embedded_x86
#pragma COMPILER_PACKALIGN_DEFAULT
#endif
#endif /* _WAVEFORMATEXTENSIBLE_ */

#ifndef MMSYSERR_NOERROR
#define MMSYSERR_NOERROR     0
#define MMSYSERR_ERROR       1
#define MMSYSERR_NOMEM       7
#define MMSYSERR_READERROR   16
#define MMSYSERR_WRITEERROR  17
#endif /* MMSYSERR_NOERROR */

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM 1
#endif /* WAVE_FORMAT_PCM */

#ifndef WAVE_FORMAT_EXTENSIBLE
#define WAVE_FORMAT_EXTENSIBLE 65534
#endif /* WAVE_FORMAT_EXTENSIBLE */

#ifndef _SPEAKER_POSITIONS_
#define _SPEAKER_POSITIONS_
/* Speaker Positions for dwChannelMask in WAVEFORMATEXTENSIBLE: from ksmedia.h */
#define SPEAKER_FRONT_LEFT              0x1
#define SPEAKER_FRONT_RIGHT             0x2
#define SPEAKER_FRONT_CENTER            0x4
#define SPEAKER_LOW_FREQUENCY           0x8
#define SPEAKER_BACK_LEFT               0x10
#define SPEAKER_BACK_RIGHT              0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER    0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
#define SPEAKER_BACK_CENTER             0x100
#define SPEAKER_SIDE_LEFT               0x200
#define SPEAKER_SIDE_RIGHT              0x400
#define SPEAKER_TOP_CENTER              0x800
#define SPEAKER_TOP_FRONT_LEFT          0x1000
#define SPEAKER_TOP_FRONT_CENTER        0x2000
#define SPEAKER_TOP_FRONT_RIGHT         0x4000
#define SPEAKER_TOP_BACK_LEFT           0x8000
#define SPEAKER_TOP_BACK_CENTER         0x10000
#define SPEAKER_TOP_BACK_RIGHT          0x20000
#endif /* _SPEAKER_POSITIONS_ */

#else  /* WFIO_NO_WIN32 */
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>
#include "macros.h"

#endif /* WFIO_NO_WIN32 */

#define IS_KSDATAFORMAT_SUBTYPE_PCM(guid)\
    (((guid)->Data1       == 0x01) &&\
     ((guid)->Data2       == 0x00) &&\
     ((guid)->Data3       == 0x10) &&\
     ((guid)->Data4[0] == 0x80) &&\
     ((guid)->Data4[1] == 0x00) &&\
     ((guid)->Data4[2] == 0x00) &&\
     ((guid)->Data4[3] == 0xaa) &&\
     ((guid)->Data4[4] == 0x00) &&\
     ((guid)->Data4[5] == 0x38) &&\
     ((guid)->Data4[6] == 0x9b) &&\
     ((guid)->Data4[7] == 0x71))

#define COPY_KSDATAFORMAT_SUBTYPE_PCM(guid)\
     (guid)->Data1       = 0x01;\
     (guid)->Data2       = 0x00;\
     (guid)->Data3       = 0x10;\
     (guid)->Data4[0] = 0x80;\
     (guid)->Data4[1] = 0x00;\
     (guid)->Data4[2] = 0x00;\
     (guid)->Data4[3] = 0xaa;\
     (guid)->Data4[4] = 0x00;\
     (guid)->Data4[5] = 0x38;\
     (guid)->Data4[6] = 0x9b;\
     (guid)->Data4[7] = 0x71;


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
  
  /* flag for WavFileIO */
  enum
  {
    wfioModeRead = 0,
    wfioModeWrite
  };
  
  /* ===========================================================================
   * WavFileIO struct
   */
  typedef struct WavFileIO_tag
  {
    I32      m_mode;
    
#if WFIO_NO_WIN32
    FILE    *m_fpWav;
    
    size_t   m_posRiffLength;
    size_t   m_posDataLength;
#else  /* WFIO_NO_WIN32 */
    HMMIO    m_hmmio;
    MMIOINFO m_mmioinfo;
    
    MMCKINFO m_ckWave;
    MMCKINFO m_ck;
#endif /* WFIO_NO_WIN32 */
    
    WAVEFORMATEXTENSIBLE *m_pFormat;      /* but allocated using BYTEs...  */
    U32   m_sizeFormat;
    
    U32   m_sizeData;
    U32   m_posDataStart;
    
  } WavFileIO;
  
  
  /* ---------------------------------------------------------------------------
   * member functions
   */
  
  WavFileIO * wfioNew();
  void wfioDelete(WavFileIO *pwfio);
  
  /* pFormat and cbFormat are only for writing.
   * Upgrades the format to WAVE_FORMAT_EXTENSIBLE, if sources with
   * > 2 channels or samples with > 16 bits are to be saved. In such an upgrade
   * process, it "guesses" speaker positions.
   */
  I32 wfioOpen(WavFileIO *pwfio, const char *szFile, WAVEFORMATEXTENSIBLE *pFormat, U32 cbFormat, I32 mode);
  void wfioClose(WavFileIO *pwfio);
  
  /* origin can be SEEK_SET, SEEK_CUR, or SEEK_END */
  I32 wfioSeek(WavFileIO *pwfio, I32 position, I32 origin);
  
  I32 wfioRead(WavFileIO *pwfio, U8 *pBuf, I32 cbBuf);
  I32 wfioWrite(WavFileIO *pwfio, U8 *pBuf, I32 cbBuf);
  
  WAVEFORMATEXTENSIBLE * wfioGetFormat(WavFileIO *pwfio);
  U32 wfioGetFormatLength(WavFileIO *pwfio);
  
  U32 wfioGetSampleSize(WavFileIO *pwfio);
  U32 wfioGetDataLength(WavFileIO *pwfio);
  
  I32 wfioGetMode(WavFileIO *pwfio);
  I32 wfioIsOpened(WavFileIO *pwfio);
  
  /* utility functions */
  U16 adjustByteOrder16(U16 val);
  U32 adjustByteOrder32(U32 val);
  
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _WAV_FILE_EX_IO_H_ */
