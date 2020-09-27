/* -------------------------------------------------------------------------
 *
 *  Microsoft Windows Media
 *
 *  Copyright (C) Microsoft Corporation, 1999 - 1999
 *
 *  File:       wavfileio.c
 *
 * ---------------------------------------------------------------------- */

#include "bldsetup.h"

#include <stdlib.h>
#include <memory.h>
#include "wavfileio.h"

#if defined(UNDER_CE) && defined(WIN32_PLATFORM_PSPC)
	// Palm-size PC 2.11 needs this, H/PC Pro 2.11 does not
#   pragma warning( disable : 4273 )
	FILE *fopen( const char *filename, const char *mode );
	int fseek( FILE *stream, long offset, int origin );
	size_t fread( void *buffer, size_t size, size_t count, FILE *stream );
	size_t fwrite( const void *buffer, size_t size, size_t count, FILE *stream );
	int fclose( FILE *stream );
	int ferror( FILE *stream );
	int fflush( FILE *stream );
#endif // UNDER_CE

#ifndef mmioFOURCC
#  if WFIO_LITTLE_ENDIAN
#    define mmioFOURCC(ch0, ch1, ch2, ch3)  ((U32)(U8)(ch0)|((U32)(U8)(ch1)<<8)|((U32)(U8)(ch2)<<16)|((U32)(U8)(ch3)<<24))
#  else  /* WFIO_LITTLE_ENDIAN */
#    define mmioFOURCC(ch0, ch1, ch2, ch3)  ((U32)(U8)(ch3)|((U32)(U8)(ch2)<<8)|((U32)(U8)(ch1)<<16)|((U32)(U8)(ch0)<<24))
#  endif /* WFIO_LITTLE_ENDIAN */
#endif /* mmioFOURCC */

#define WFIO_FOURCC_RIFF    mmioFOURCC('R', 'I', 'F', 'F')
#define WFIO_FOURCC_WAVE    mmioFOURCC('W', 'A', 'V', 'E')
#define WFIO_FOURCC_fmt     mmioFOURCC('f', 'm', 't', ' ')
#define WFIO_FOURCC_data    mmioFOURCC('d', 'a', 't', 'a')
#define WFIO_FOURCC_fact    mmioFOURCC('f', 'a', 'c', 't')

/* forward declarations */
static I32 wfioReadHeaders(WavFileIO *pwfio);
static I32 wfioWriteHeaders(WavFileIO *pwfio, WAVEFORMATEX *pFormat, U32 cbFormat);
static I32 wfioCleanUpRead(WavFileIO *pwfio);
static I32 wfioCleanUpWrite(WavFileIO *pwfio);


/* utility functions */

U16
adjustByteOrder16(U16 val)
{
#if WFIO_LITTLE_ENDIAN
    return val;
#else  /* WFIO_LITTLE_ENDIAN */
    U8 *p = (U8 *)&val;
    return ((U16)p[0])|((U16)p[1]<<8);
#endif /* WFIO_LITTLE_ENDIAN */
}

U32
adjustByteOrder32(U32 val)
{
#if WFIO_LITTLE_ENDIAN
    return val;
#else  /* WFIO_LITTLE_ENDIAN */
    U8 *p = (U8 *)&val;
    return ((U32)p[0])|((U32)p[1]<<8)|((U32)p[2]<<16)|((U32)p[3]<<24);
#endif /* WFIO_LITTLE_ENDIAN */
}


WavFileIO *
wfioNew()
{
    WavFileIO *pwfio = (WavFileIO *)wmvalloc(sizeof(WavFileIO));
    if(pwfio == NULL)
        return NULL;

    pwfio->m_mode = wfioModeRead;
#if WFIO_NO_WIN32
    pwfio->m_fpWav = NULL;
    pwfio->m_posRiffLength = 0;
    pwfio->m_posDataLength = 0;
#else   /* WFIO_NO_WIN32 */
    pwfio->m_hmmio = NULL;
#endif  /* WFIO_NO_WIN32 */
    pwfio->m_pFormat = NULL;
    pwfio->m_sizeFormat = 0;
    pwfio->m_sizeData = 0;
    pwfio->m_posDataStart = 0;

#if !WFIO_NO_WIN32
    memset(&pwfio->m_mmioinfo, 0, sizeof(pwfio->m_mmioinfo));
    memset(&pwfio->m_ckWave, 0, sizeof(pwfio->m_ckWave));
    memset(&pwfio->m_ck, 0, sizeof(pwfio->m_ck));
#endif  /* WFIO_NO_WIN32 */

    return pwfio;
}


void
wfioDelete(WavFileIO *pwfio)
{
    wfioClose(pwfio);
    wmvfree(pwfio);
}


I32
wfioOpen(WavFileIO *pwfio,
         const char *szFile,
         WAVEFORMATEX *pFormat,
         U32 cbFormat,
         I32 mode)
{
    I32 lr;

    if(pwfio == NULL)
        return MMSYSERR_ERROR;

#if WFIO_NO_WIN32

    if(pwfio->m_fpWav)
        return MMSYSERR_ERROR;

    pwfio->m_mode = mode;

    if(pwfio->m_mode == wfioModeWrite)
    {
        /* open the file for writing */
        pwfio->m_fpWav = fopen(szFile, "wb");
        if(pwfio->m_fpWav == NULL)
        {
            lr = MMSYSERR_ERROR;
            goto lerror;
        }

        /* write the header info */
        lr = wfioWriteHeaders(pwfio, pFormat, cbFormat);
        if(lr != MMSYSERR_NOERROR)
            goto lerror;
    }
    else
    {
        /* open the file for reading */
        pwfio->m_fpWav = fopen(szFile, "rb");
        if(pwfio->m_fpWav == NULL)
        {
            lr = MMSYSERR_ERROR;
            goto lerror;
        }

        /* read the hearder info */
        lr = wfioReadHeaders(pwfio);
        if(lr != MMSYSERR_NOERROR)
            goto lerror;

        /* seek to the beginning */
        wfioSeek(pwfio, 0, SEEK_CUR);
    }

#else  /* WFIO_NO_WIN32 */

    if(pwfio->m_hmmio)
        return MMSYSERR_ERROR;

    pwfio->m_mode = mode;

    /* slightly different for READ and WRITE */
    if(pwfio->m_mode == wfioModeRead)
    {
        /* open the file for read */
        pwfio->m_hmmio = mmioOpen((LPSTR)szFile, NULL, MMIO_READ/*|MMIO_DENYWRITE*/);
        if(pwfio->m_hmmio == NULL)
        {
            lr = MMSYSERR_ERROR;
            goto lerror;
        }

        /* read the header info */
        lr = wfioReadHeaders(pwfio);
        if(lr != MMSYSERR_NOERROR)
            goto lerror;

        /* seek to the beginning */
        wfioSeek(pwfio, 0, SEEK_CUR);
  }
  else
  {
      /* open the file for write */
      pwfio->m_hmmio = mmioOpen((LPSTR)szFile, NULL, MMIO_READWRITE|MMIO_CREATE|MMIO_ALLOCBUF/*|MMIO_DENYWRITE*/);
      if(pwfio->m_hmmio == NULL)
      {
          lr = MMSYSERR_ERROR;
          goto lerror;
      }

      lr = wfioWriteHeaders(pwfio, pFormat, cbFormat);
      if(lr != MMSYSERR_NOERROR)
          goto lerror;
  }

#endif /* WFIO_NO_WIN32 */

  return MMSYSERR_NOERROR;

lerror:
  wfioClose(pwfio);
  return lr;
}


void
wfioClose(WavFileIO *pwfio)
{
    if(pwfio == NULL)
        return;

#if WFIO_NO_WIN32

    if(pwfio->m_fpWav)
    {
        if(pwfio->m_mode == wfioModeWrite)
            wfioCleanUpWrite(pwfio);
        else
            wfioCleanUpRead(pwfio);

        fclose(pwfio->m_fpWav);
        pwfio->m_fpWav = NULL;
    }

    if(pwfio->m_pFormat)
    {
        wmvfree(pwfio->m_pFormat);
        pwfio->m_pFormat = NULL;

        pwfio->m_sizeFormat = 0;
    }

    pwfio->m_sizeData = 0;
    pwfio->m_posDataStart = 0;

    /* just for the safety... */
    pwfio->m_posRiffLength = 0;
    pwfio->m_posDataLength = 0;

#else  /* WFIO_NO_WIN32 */

    if(pwfio->m_hmmio)
    {
        if(pwfio->m_mode == wfioModeWrite)
            wfioCleanUpWrite(pwfio);
        else
            wfioCleanUpRead(pwfio);

        mmioClose(pwfio->m_hmmio, 0);
        pwfio->m_hmmio = NULL;
    }

    if(pwfio->m_pFormat)
    {
        wmvfree(pwfio->m_pFormat);
        pwfio->m_pFormat = NULL;

        pwfio->m_sizeFormat = 0;
    }

    pwfio->m_sizeData = 0;
    pwfio->m_posDataStart = 0;

    /* just for safety... */
    memset(&pwfio->m_mmioinfo, 0, sizeof(pwfio->m_mmioinfo));
    memset(&pwfio->m_ckWave, 0, sizeof(pwfio->m_ckWave));
    memset(&pwfio->m_ck, 0, sizeof(pwfio->m_ck));

#endif /* WFIO_NO_WIN32 */
}


I32
wfioSeek(WavFileIO *pwfio,
         I32 position,
         I32 origin)
{
    if(pwfio == NULL)
        return -1;

    /* only for reading... */
    if(pwfio->m_mode != wfioModeRead)
        return -1;

#if WFIO_NO_WIN32

    switch(origin)
    {
    case SEEK_SET:
        return fseek(pwfio->m_fpWav, pwfio->m_posDataStart + position, SEEK_SET);

    case SEEK_CUR:
        return fseek(pwfio->m_fpWav, position, SEEK_CUR);

    case SEEK_END:
        return fseek(pwfio->m_fpWav, pwfio->m_posDataStart + pwfio->m_sizeData - position, SEEK_SET);
    }

#else  /* WFIO_NO_WIN32 */

    switch(origin)
    {
    case SEEK_SET:
        return mmioSeek(pwfio->m_hmmio, pwfio->m_posDataStart + position, SEEK_SET);

    case SEEK_CUR:
        return mmioSeek(pwfio->m_hmmio, position, SEEK_CUR);

    case SEEK_END:
        return mmioSeek(pwfio->m_hmmio, pwfio->m_posDataStart + pwfio->m_sizeData - position, SEEK_SET);
    }

#endif /* WFIO_NO_WIN32 */

  return -1;
}


I32
wfioRead(WavFileIO *pwfio,
         U8 *pBuf,
         I32 cbBuf)
{
#if WFIO_NO_WIN32

    size_t ret;

    if(pwfio == NULL)
        return (I32)-1;

    if(pwfio->m_fpWav == NULL
       || pwfio->m_mode != wfioModeRead
       || pBuf == NULL)
        return (I32)-1;

    /* in alignment units... */
    cbBuf = (cbBuf/pwfio->m_pFormat->nBlockAlign)*pwfio->m_pFormat->nBlockAlign;

#if !WFIO_LITTLE_ENDIAN

    /* have to swap bytes... */

    if(pwfio->m_pFormat->wBitsPerSample == 16)
    {
        I32 i;
        U16 *p = (U16 *)pBuf;
        for(i = cbBuf/2; i > 0; i--, p++)
            *p = adjustByteOrder16(*p);
    }

#endif  /* WFIO_LITTLE_ENDIAN */

    ret = fread(pBuf, 1, cbBuf, pwfio->m_fpWav);
    if(ferror(pwfio->m_fpWav))
        return (I32)-1;

    return (I32)ret;

#else  /* WFIO_NO_WIN32 */

    if(pwfio == NULL)
        return (I32)-1;

    return mmioRead(pwfio->m_hmmio, (HPSTR)pBuf, cbBuf);

#endif /* WFIO_NO_WIN32 */
}


I32
wfioWrite(WavFileIO *pwfio,
          U8 *pBuf,
          I32 cbBuf)
{
#if WFIO_NO_WIN32

    size_t ret;

    if(pwfio == NULL)
        return (I32)-1;

    if(pwfio->m_fpWav == NULL
       || pwfio->m_mode != wfioModeWrite
       || pBuf == NULL)
        return (I32)-1;

    /* in alignment units... */
    cbBuf = (cbBuf/pwfio->m_pFormat->nBlockAlign)*pwfio->m_pFormat->nBlockAlign;

#if !WFIO_LITTLE_ENDIAN

    /* have to swap bytes... */

    if(pwfio->m_pFormat->wBitsPerSample == 16)
    {
        I32 i;
        U16 *p = (U16 *)pBuf;
        for(i = cbBuf/2; i > 0; i--, p++)
            *p = adjustByteOrder16(*p);
    }

#endif  /* WFIO_LITTLE_ENDIAN */

    ret = fwrite(pBuf, 1, cbBuf, pwfio->m_fpWav);
    if(ferror(pwfio->m_fpWav))
        return (I32)-1;

    pwfio->m_sizeData += ret;

    return ret;

#else  /* WFIO_NO_WIN32 */

    /* buffered write (MMIO_ALLOCBUF used for opening) */

    I32 lr;
    I32 cbBlock;
    I32 cbWritten = 0;

    if(pwfio == NULL)
        return (I32)-1;

    if(pBuf == NULL)
        return 0;

    while(cbBuf > 0)
    {
        /* check if we need to advance to the next buffer */
        if(pwfio->m_mmioinfo.pchNext == pwfio->m_mmioinfo.pchEndWrite)
        {
            pwfio->m_mmioinfo.dwFlags |= MMIO_DIRTY;
            lr = mmioAdvance(pwfio->m_hmmio, &pwfio->m_mmioinfo, MMIO_WRITE);
            if(lr != MMSYSERR_NOERROR)
                goto lerror;
        }

        /* copy in blocks */
        cbBlock = pwfio->m_mmioinfo.pchEndWrite - pwfio->m_mmioinfo.pchNext;
        if(cbBlock > cbBuf)
            cbBlock = cbBuf;
        CopyMemory(pwfio->m_mmioinfo.pchNext, pBuf, cbBlock);

        /* update pointers and counters */
        pBuf                      += cbBlock;
        pwfio->m_mmioinfo.pchNext += cbBlock;
        cbWritten                 += cbBlock;
        cbBuf                     -= cbBlock;

        /* update global stat members */
        pwfio->m_sizeData += cbBlock;
    }

lerror:
    return cbWritten;

#endif /* WFIO_NO_WIN32 */
}


WAVEFORMATEX *
wfioGetFormat(WavFileIO *pwfio)
{
    if(pwfio == NULL)
        return NULL;

    return pwfio->m_pFormat;
}


U32
wfioGetFormatLength(WavFileIO *pwfio)
{
    if(pwfio == NULL || pwfio->m_pFormat == NULL)
        return 0;

    return sizeof(WAVEFORMATEX) + pwfio->m_pFormat->cbSize;
}


U32
wfioGetSampleSize(WavFileIO *pwfio)
{
    if(pwfio == NULL || pwfio->m_pFormat == NULL)
        return 0;

    return pwfio->m_pFormat->nBlockAlign;
}


U32
wfioGetDataLength(WavFileIO *pwfio)
{
    if(pwfio == NULL)
        return 0;

    return pwfio->m_sizeData;
}


I32
wfioGetMode(WavFileIO *pwfio)
{
    if(pwfio == NULL)
        return 0;

    return pwfio->m_mode;
}


I32
wfioIsOpened(WavFileIO *pwfio)
{
#if WFIO_NO_WIN32
    return pwfio && pwfio->m_fpWav != NULL;
#else   /* WFIO_NO_WIN32 */
    return pwfio && pwfio->m_hmmio != NULL;
#endif  /* WFIO_NO_WIN32 */
}


I32
wfioReadHeaders(WavFileIO *pwfio)
{
    I32 lr;
    Bool fFmtChunkFound  = WMAB_FALSE;  
    Bool fDataChunkFound = WMAB_FALSE;
    
#if WFIO_NO_WIN32
    
    size_t ret;
    size_t offset = 0;
    size_t offSeek;
    
    U32 dwTemp;
    U32 dwSize;
    U32 dwHead;
    
    if(pwfio == NULL)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    
    if(pwfio->m_fpWav == NULL)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    
    /* -----------------------------------------------------------------------
    * find the RIFF/WAVE chunk
    */
    
    /* read "RIFF" */
    
    ret = fread(&dwHead, 1, sizeof(U32), pwfio->m_fpWav);
    if(ret != sizeof(U32))
    {
        lr = MMSYSERR_READERROR;
        goto lerror;
    }
    offset += ret;
    
    if(dwHead != WFIO_FOURCC_RIFF)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    
    /* read "RIFF" length */
    
    pwfio->m_posRiffLength = offset;
    
    ret = fread(&dwTemp, 1, sizeof(U32), pwfio->m_fpWav);
    if(ret != sizeof(U32))
    {
        lr = MMSYSERR_READERROR;
        goto lerror;
    }
    offset += ret;
    dwSize = adjustByteOrder32(dwTemp); //Ignored
    
    /* read "WAVE" */
    
    ret = fread(&dwHead, 1, sizeof(U32), pwfio->m_fpWav);
    if(ret != sizeof(U32))
    {
        lr = MMSYSERR_READERROR;
        goto lerror;
    }
    offset += ret;
    
    if(dwHead != WFIO_FOURCC_WAVE)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    
    offSeek = offset;
    
    /* -----------------------------------------------------------------------
    * find the FMT chunk: skip over unknown chunks
    */
    
    /* read "fmt " */
    fFmtChunkFound = WMAB_FALSE;  
    do {
        // Read chunk header
        ret = fread(&dwHead, 1, sizeof(U32), pwfio->m_fpWav);
        if(ret != sizeof(U32))
        {
            lr = MMSYSERR_READERROR;
            goto lerror;
        }
        offset += ret;
        
        if(dwHead == WFIO_FOURCC_fmt)
            fFmtChunkFound = WMAB_TRUE;
        
        /* read chunk length */
        ret = fread(&dwTemp, 1, sizeof(U32), pwfio->m_fpWav);
        if(ret != sizeof(U32))
        {
            lr = MMSYSERR_READERROR;
            goto lerror;
        }
        offset += ret;
        
        dwSize = adjustByteOrder32(dwTemp);
        if (!fFmtChunkFound) {
            /* read-off dwSize bytes */
            offset += dwSize;
            ret = fseek(pwfio->m_fpWav, offset, SEEK_SET);
            if ( ret ) {
                lr = MMSYSERR_READERROR;
                goto lerror;
            } 
        }  
    } while (!fFmtChunkFound);
    
    /* "fmt " length was read earlier */
    /* prepare the format buffer */
    
    pwfio->m_sizeFormat = dwSize;
    if(pwfio->m_sizeFormat < sizeof(WAVEFORMATEX))
        pwfio->m_sizeFormat = sizeof(WAVEFORMATEX);
    
    pwfio->m_pFormat = (WAVEFORMATEX *)wmvalloc(pwfio->m_sizeFormat);
    if(pwfio->m_pFormat == NULL)
    {
        lr = MMSYSERR_NOMEM;
        goto lerror;
    }
    memset(pwfio->m_pFormat, 0, pwfio->m_sizeFormat);
    
    /* read the format chunk */
    
    ret = fread(pwfio->m_pFormat, 1, dwSize, pwfio->m_fpWav);
    if(ret != dwSize)
    {
        lr = MMSYSERR_READERROR;
        goto lerror;
    }
    offset += ret;
    
    /* HongCho: I can convert the fields into the correct order here,
    *          but since I don't know the layouts of all the possible
    *          formats, only the fields in WAVEFORMATEX will be
    *          converted.
    */
    
    pwfio->m_pFormat->wFormatTag      = adjustByteOrder16(pwfio->m_pFormat->wFormatTag);
    pwfio->m_pFormat->nChannels       = adjustByteOrder16(pwfio->m_pFormat->nChannels);
    pwfio->m_pFormat->nSamplesPerSec  = adjustByteOrder32(pwfio->m_pFormat->nSamplesPerSec);
    pwfio->m_pFormat->nAvgBytesPerSec = adjustByteOrder32(pwfio->m_pFormat->nAvgBytesPerSec);
    pwfio->m_pFormat->nBlockAlign     = adjustByteOrder16(pwfio->m_pFormat->nBlockAlign);
    pwfio->m_pFormat->wBitsPerSample  = adjustByteOrder16(pwfio->m_pFormat->wBitsPerSample);
    pwfio->m_pFormat->cbSize          = adjustByteOrder16(pwfio->m_pFormat->cbSize);
    
#if !WFIO_LITTLE_ENDIAN
    /* only PCM for big-endian platforms */
    if(pwfio->m_pFormat->wFormatTag != WAVE_FORMAT_PCM)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
#endif /* WFIO_LITTLE_ENDIAN */
    
    /* the format chunk for PCM on WAVE files are WAVEFORMAT */
    if(pwfio->m_pFormat->wFormatTag == WAVE_FORMAT_PCM)
        pwfio->m_pFormat->cbSize = 0;
    
        /* -----------------------------------------------------------------------
        * find the DATA chunk: skip over other unknown chunks
    */
    fDataChunkFound = WMAB_FALSE;
    
    do {  
        /* Read Chunk Header */
        ret = fread(&dwHead, 1, sizeof(U32), pwfio->m_fpWav);
        if(ret != sizeof(U32))
        {
            lr = MMSYSERR_READERROR;
            goto lerror;
        }
        offset += ret;
        
        fDataChunkFound = (dwHead == WFIO_FOURCC_data);
        
        if (!fDataChunkFound)
        {
            /* Read chunk length */
            ret = fread(&dwTemp, 1, sizeof(U32), pwfio->m_fpWav);
            if(ret != sizeof(U32))
            {
                lr = MMSYSERR_READERROR;
                goto lerror;
            }
            offset += ret;
            
            dwSize = adjustByteOrder32(dwTemp);
            offset += dwSize;
            
            // Unknown/ un-needed chunk. Skip over it.
            /* read-off dwSize bytes */
            ret = fseek(pwfio->m_fpWav, offset, SEEK_SET);
            if ( ret) {
                lr = MMSYSERR_READERROR;
                goto lerror;
            }
        }
    } while (!fDataChunkFound);
    
    /* read "data" length */
    
    pwfio->m_posDataLength = offset;
    
    ret = fread(&dwTemp, 1, sizeof(U32), pwfio->m_fpWav);
    if(ret != sizeof(U32))
    {
        lr = MMSYSERR_READERROR;
        goto lerror;
    }
    offset += ret;
    
    pwfio->m_sizeData   = adjustByteOrder32(dwTemp);
    
    pwfio->m_posDataStart = offset;
    
#else  /* WFIO_NO_WIN32 */
    
    MMCKINFO ck;
    
    U32 cksize;
    
    if(pwfio == NULL)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    
    if(pwfio->m_hmmio == NULL)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    
    /* find the wave info chunk */
    pwfio->m_ckWave.fccType = WFIO_FOURCC_WAVE;
    lr = mmioDescend(pwfio->m_hmmio, &pwfio->m_ckWave, NULL, MMIO_FINDRIFF);
    if(lr != MMSYSERR_NOERROR)
        goto lerror;
    
    /* find FMT chunk */
    ck.ckid = WFIO_FOURCC_fmt;
    lr = mmioDescend(pwfio->m_hmmio, &ck, &pwfio->m_ckWave, MMIO_FINDCHUNK);
    if(lr != MMSYSERR_NOERROR)
        goto lerror;
    
    cksize = ck.cksize;
    if(cksize < sizeof(WAVEFORMATEX))
        cksize = sizeof(WAVEFORMATEX);
    
    /* prepare the format buffer */
    pwfio->m_sizeFormat = cksize;
    pwfio->m_pFormat = (WAVEFORMATEX *)wmvalloc(pwfio->m_sizeFormat);
    if(pwfio->m_pFormat == NULL)
    {
        lr = MMSYSERR_NOMEM;
        goto lerror;
    }
    memset(pwfio->m_pFormat, 0, pwfio->m_sizeFormat);
    
    /* read the format chunk */
    lr = mmioRead(pwfio->m_hmmio, (HPSTR)pwfio->m_pFormat, ck.cksize);
    if(lr != (I32)ck.cksize)
    {
        lr = MMSYSERR_READERROR;
        goto lerror;
    }
    
    /* the format chunk for PCM on WAV files are WAVEFORMAT */
    if(pwfio->m_pFormat->wFormatTag == WAVE_FORMAT_PCM)
        pwfio->m_pFormat->cbSize = 0;
    
    /* ascend out of FMT */
    lr = mmioAscend(pwfio->m_hmmio, &ck, 0);
    if(lr != MMSYSERR_NOERROR)
        goto lerror;
    
    /* find DATA chunk */
    pwfio->m_ck.ckid = WFIO_FOURCC_data;
    lr = mmioDescend(pwfio->m_hmmio, &pwfio->m_ck, &pwfio->m_ckWave, MMIO_FINDCHUNK);
    if(lr != MMSYSERR_NOERROR)
        goto lerror;
    
    pwfio->m_sizeData     = pwfio->m_ck.cksize;
    
    pwfio->m_posDataStart = pwfio->m_ck.dwDataOffset;
    
    /* get info */
    lr = mmioGetInfo(pwfio->m_hmmio, &pwfio->m_mmioinfo, 0);
    if(lr != MMSYSERR_NOERROR)
        goto lerror;
    
#endif /* WFIO_NO_WIN32 */
    
    return MMSYSERR_NOERROR;
    
lerror:
    if(pwfio->m_pFormat)
    {
        wmvfree(pwfio->m_pFormat);
        pwfio->m_pFormat = NULL;
        
        pwfio->m_sizeFormat = 0;
    }
    return lr;
}


I32
wfioWriteHeaders(WavFileIO *pwfio,
                 WAVEFORMATEX *pFormat,
                 U32 cbFormat)
{
    I32 lr;

#if WFIO_NO_WIN32

    size_t ret;
    size_t offset = 0;

    U32 dwTemp;

    WAVEFORMATEX *pWavFormat = NULL;

    if(pwfio == NULL)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }

    if(pwfio->m_fpWav == NULL)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    if(pFormat == NULL)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    if(cbFormat < sizeof(WAVEFORMATEX))
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }

    /* copy the format locally */

    if(pwfio->m_pFormat)
    {
        wmvfree(pwfio->m_pFormat);
        pwfio->m_pFormat = NULL;

        pwfio->m_sizeFormat = 0;
    }
    pwfio->m_sizeFormat = cbFormat;
    pwfio->m_pFormat = (WAVEFORMATEX *)wmvalloc(pwfio->m_sizeFormat);
    if(pwfio->m_pFormat == NULL)
    {
        lr = MMSYSERR_NOMEM;
        goto lerror;
    }
    memcpy(pwfio->m_pFormat, pFormat, pwfio->m_sizeFormat);

    /* format in the right format... */
    pWavFormat = (WAVEFORMATEX *)wmvalloc(pwfio->m_sizeFormat);
    if(pWavFormat == NULL)
    {
        lr = MMSYSERR_NOMEM;
        goto lerror;
    }
    memcpy(pWavFormat, pFormat, pwfio->m_sizeFormat);

#if !WFIO_LITTLE_ENDIAN
    /* only PCM for big-endian platforms... */
    if(pFormat->wFormatTag != WAVE_FORMAT_PCM)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
#endif /* WFIO_LITTLE_ENDIAN */

    pWavFormat->wFormatTag      = adjustByteOrder16(pWavFormat->wFormatTag);
    pWavFormat->nChannels       = adjustByteOrder16(pWavFormat->nChannels);
    pWavFormat->nSamplesPerSec  = adjustByteOrder32(pWavFormat->nSamplesPerSec);
    pWavFormat->nAvgBytesPerSec = adjustByteOrder32(pWavFormat->nAvgBytesPerSec);
    pWavFormat->nBlockAlign     = adjustByteOrder16(pWavFormat->nBlockAlign);
    pWavFormat->wBitsPerSample  = adjustByteOrder16(pWavFormat->wBitsPerSample);
    pWavFormat->cbSize          = adjustByteOrder16(pWavFormat->cbSize);

    /* -----------------------------------------------------------------------
     * create RIFF/WAVE chunk
     */

    /* write "RIFF" */

    dwTemp = WFIO_FOURCC_RIFF;
    ret = fwrite(&dwTemp, 1, sizeof(U32), pwfio->m_fpWav);
    if(ret != sizeof(U32))
    {
        lr = MMSYSERR_WRITEERROR;
        goto lerror;
    }
    offset += ret;

    /* write "RIFF" length, temporarily */

    pwfio->m_posRiffLength = offset;

    dwTemp = 0;   /* temporary */
    ret = fwrite(&dwTemp, 1, sizeof(U32), pwfio->m_fpWav);
    if(ret != sizeof(U32))
    {
        lr = MMSYSERR_WRITEERROR;
        goto lerror;
    }
    offset += ret;

    /* write "WAVE" */

    dwTemp = WFIO_FOURCC_WAVE;
    ret = fwrite(&dwTemp, 1, sizeof(U32), pwfio->m_fpWav);
    if(ret != sizeof(U32))
    {
        lr = MMSYSERR_WRITEERROR;
        goto lerror;
    }
    offset += ret;

    /* -----------------------------------------------------------------------
     * create FMT chunk
     */

    /* write "fmt " */

    dwTemp = WFIO_FOURCC_fmt;
    ret = fwrite(&dwTemp, 1, sizeof(U32), pwfio->m_fpWav);
    if(ret != sizeof(U32))
    {
        lr = MMSYSERR_WRITEERROR;
        goto lerror;
    }
    offset += ret;

    /* write "fmt " length */

    dwTemp = adjustByteOrder32(cbFormat);
    ret = fwrite(&dwTemp, 1, sizeof(U32), pwfio->m_fpWav);
    if(ret != sizeof(U32))
    {
        lr = MMSYSERR_WRITEERROR;
        goto lerror;
    }
    offset += ret;

    /* write actual format */

    ret = fwrite(pWavFormat, 1, cbFormat, pwfio->m_fpWav);
    if(ret != cbFormat)
    {
        lr = MMSYSERR_WRITEERROR;
        goto lerror;
    }
    offset += ret;

    /* -----------------------------------------------------------------------
     * create DATA chunk
     */

    /* write "data" */

    dwTemp = WFIO_FOURCC_data;
    ret = fwrite(&dwTemp, 1, sizeof(U32), pwfio->m_fpWav);
    if(ret != sizeof(U32))
    {
        lr = MMSYSERR_WRITEERROR;
        goto lerror;
    }
    offset += ret;

    /* write "data" length */

    pwfio->m_posDataLength = offset;

    dwTemp = 0;
    ret = fwrite(&dwTemp, 1, sizeof(U32), pwfio->m_fpWav);
    if(ret != sizeof(U32))
    {
        lr = MMSYSERR_WRITEERROR;
        goto lerror;
    }
    offset += ret;

    pwfio->m_sizeData   = 0;

    pwfio->m_posDataStart = offset;

    if(pWavFormat)
    {
        wmvfree(pWavFormat);
        pWavFormat = NULL;
    }

    return MMSYSERR_NOERROR;

lerror:
    if(pwfio->m_pFormat)
    {
        wmvfree(pwfio->m_pFormat);
        pwfio->m_pFormat = NULL;

        pwfio->m_sizeFormat = 0;
    }

    if(pWavFormat)
    {
        wmvfree(pWavFormat);
        pWavFormat = NULL;
    }

    return lr;

#else  /* WFIO_NO_WIN32 */

    if(pwfio == NULL)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    if(pwfio->m_hmmio == NULL)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    if(pwfio->m_mode != wfioModeWrite)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    if(pFormat == NULL)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    if(cbFormat < sizeof(WAVEFORMATEX))
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }

    /* copy the format locally */
    if(pwfio->m_pFormat)
    {
        wmvfree(pwfio->m_pFormat);
        pwfio->m_pFormat = NULL;

        pwfio->m_sizeFormat = 0;
    }
    pwfio->m_sizeFormat = cbFormat;
    pwfio->m_pFormat = (WAVEFORMATEX *)wmvalloc(pwfio->m_sizeFormat);
    if(pwfio->m_pFormat == NULL)
    {
        lr = MMSYSERR_NOMEM;
        goto lerror;
    }
    CopyMemory(pwfio->m_pFormat, pFormat, pwfio->m_sizeFormat);

    /* create WAVE chunk */
    pwfio->m_ckWave.fccType = WFIO_FOURCC_WAVE;
    pwfio->m_ckWave.cksize  = 0;
    lr = mmioCreateChunk(pwfio->m_hmmio, &pwfio->m_ckWave, MMIO_CREATERIFF);
    if(lr != MMSYSERR_NOERROR)
        goto lerror;

    /* create FMT chunk */
    pwfio->m_ck.ckid   = WFIO_FOURCC_fmt;
    pwfio->m_ck.cksize = pwfio->m_sizeFormat;
    lr = mmioCreateChunk(pwfio->m_hmmio, &pwfio->m_ck, 0);
    if(lr != MMSYSERR_NOERROR)
        goto lerror;

    /* copy out the format */
    lr = mmioWrite(pwfio->m_hmmio, (HPSTR)pwfio->m_pFormat, pwfio->m_sizeFormat);
    if(lr != (I32)pwfio->m_sizeFormat)
    {
        lr = MMSYSERR_WRITEERROR;
        goto lerror;
    }

    /* ascend out of FMT */
    lr = mmioAscend(pwfio->m_hmmio, &pwfio->m_ck, 0);
    if(lr != MMSYSERR_NOERROR)
        goto lerror;

    /* create DATA chunk */

    pwfio->m_ck.ckid   = WFIO_FOURCC_data;
    pwfio->m_ck.cksize = 0;
    lr = mmioCreateChunk(pwfio->m_hmmio, &pwfio->m_ck, 0);
    if(lr != MMSYSERR_NOERROR)
        goto lerror;

    /* get info */
    lr = mmioGetInfo(pwfio->m_hmmio, &pwfio->m_mmioinfo, 0);
    if(lr != MMSYSERR_NOERROR)
        goto lerror;

    if(pwfio->m_pFormat)
    {
        wmvfree(pwfio->m_pFormat);
        pwfio->m_pFormat = NULL;

        pwfio->m_sizeFormat = 0;
    }

    return MMSYSERR_NOERROR;

lerror:
    if(pwfio->m_pFormat)
    {
        wmvfree(pwfio->m_pFormat);
        pwfio->m_pFormat = NULL;

        pwfio->m_sizeFormat = 0;
    }
    return lr;

#endif /* WFIO_NO_WIN32 */
}


I32
wfioCleanUpRead(WavFileIO *pwfio)
{
    if(pwfio == NULL)
        return MMSYSERR_ERROR;

    /* for a historic reason... */
    return MMSYSERR_NOERROR;
}


I32
wfioCleanUpWrite(WavFileIO *pwfio)
{
    I32 lr;

#if WFIO_NO_WIN32

    size_t ret;

    U32 dwTemp;

    if(pwfio == NULL)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    if(pwfio->m_fpWav == NULL)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    if(pwfio->m_mode != wfioModeWrite)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }

    /* flush it first */

    fflush(pwfio->m_fpWav);

    /* write back the data length */

    ret = fseek(pwfio->m_fpWav, pwfio->m_posDataLength, SEEK_SET);
    if(ret != 0)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }

    dwTemp = adjustByteOrder32(pwfio->m_sizeData);
    ret = fwrite(&dwTemp, 1, sizeof(U32), pwfio->m_fpWav);
    if(ret != sizeof(U32))
    {
        lr = MMSYSERR_WRITEERROR;
        goto lerror;
    }

    /* write back the riff length */

    ret = fseek(pwfio->m_fpWav, pwfio->m_posRiffLength, SEEK_SET);
    if(ret != 0)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }

    dwTemp = pwfio->m_sizeData + pwfio->m_posDataStart - 8; /* "RIFF" and its size (4 + 4 bytes) */
    dwTemp = adjustByteOrder32(dwTemp);
    ret = fwrite(&dwTemp, 1, sizeof(U32), pwfio->m_fpWav);
    if(ret != sizeof(U32))
    {
        lr = MMSYSERR_WRITEERROR;
        goto lerror;
    }

    /* flush again... */

    fflush(pwfio->m_fpWav);

#else   /* WFIO_NO_WIN32 */

    if(pwfio == NULL)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    if(pwfio->m_hmmio == NULL)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }
    if(pwfio->m_mode != wfioModeWrite)
    {
        lr = MMSYSERR_ERROR;
        goto lerror;
    }

    /* write back the info */
    pwfio->m_mmioinfo.dwFlags |= MMIO_DIRTY;
    lr = mmioSetInfo(pwfio->m_hmmio, &pwfio->m_mmioinfo, 0);
    if(lr != MMSYSERR_NOERROR)
        goto lerror;

    /* ascend out of DATA.  this will cause the write-out. */
    lr = mmioAscend(pwfio->m_hmmio, &pwfio->m_ck, 0);
    if(lr != MMSYSERR_NOERROR)
        goto lerror;

    /* ascend out of RIFF.  this will cause the file write-out, probably. */
    lr = mmioAscend(pwfio->m_hmmio, &pwfio->m_ckWave, 0);
    if(lr != MMSYSERR_NOERROR)
        goto lerror;

#endif  /* WFIO_NO_WIN32 */

    return MMSYSERR_NOERROR;

lerror:
    return lr;
}
