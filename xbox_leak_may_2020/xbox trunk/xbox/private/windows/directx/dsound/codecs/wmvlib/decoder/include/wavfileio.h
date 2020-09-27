/* -------------------------------------------------------------------------
 *
 *  Microsoft Windows Media
 *
 *  Copyright (C) Microsoft Corporation, 1999 - 1999
 *
 *  File:       wavfileio.h
 *
 * ---------------------------------------------------------------------- */

#ifndef _WAV_FILE_IO_H_
#define _WAV_FILE_IO_H_

/* use this to turn off the use of mmio* calls */
#define WFIO_NO_WIN32 1

/* use this to control the endian-ness */
#define WFIO_LITTLE_ENDIAN 1

#if WFIO_NO_WIN32

#include <stdio.h>
#include "macros.h"

#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
#ifndef _Embedded_x86
#pragma COMPILER_PACKALIGN(1)
#endif
typedef struct tWAVEFORMATEX
{
    U16     wFormatTag;        /* format type */
    U16     nChannels;         /* number of channels (i.e. mono, stereo...) */
    U32     nSamplesPerSec;    /* sample rate */
    U32     nAvgBytesPerSec;   /* for buffer estimation */
    U16     nBlockAlign;       /* block size of data */
    U16     wBitsPerSample;    /* Number of bits per sample of mono data */
    U16     cbSize;            /* The count in bytes of the size of
                                    extra information (after cbSize) */

} WAVEFORMATEX, *PWAVEFORMATEX, *LPWAVEFORMATEX;
#ifndef _Embedded_x86
#pragma COMPILER_PACKALIGN_DEFAULT
#endif
#endif /* _WAVEFORMATEX_ */

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

#else  /* WFIO_NO_WIN32 */

#include <mmsystem.h>
#include <mmreg.h>

#endif /* WFIO_NO_WIN32 */

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

    WAVEFORMATEX *m_pFormat;      /* but allocated using BYTEs...  */
    U32   m_sizeFormat;

    U32   m_sizeData;
    U32   m_posDataStart;

} WavFileIO;


/* ---------------------------------------------------------------------------
 * member functions
 */

WavFileIO * wfioNew();
void wfioDelete(WavFileIO *pwfio);

/* pFormat and cbFormat are only for writing... */
I32 wfioOpen(WavFileIO *pwfio, const char *szFile, WAVEFORMATEX *pFormat, U32 cbFormat, I32 mode);
void wfioClose(WavFileIO *pwfio);

/* origin can be SEEK_SET, SEEK_CUR, or SEEK_END */
I32 wfioSeek(WavFileIO *pwfio, I32 position, I32 origin);

I32 wfioRead(WavFileIO *pwfio, U8 *pBuf, I32 cbBuf);
I32 wfioWrite(WavFileIO *pwfio, U8 *pBuf, I32 cbBuf);

WAVEFORMATEX * wfioGetFormat(WavFileIO *pwfio);
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


#endif /* _WAV_FILE_IO_H_ */
