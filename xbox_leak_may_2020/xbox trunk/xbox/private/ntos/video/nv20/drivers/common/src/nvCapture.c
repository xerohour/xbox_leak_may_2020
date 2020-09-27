 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/

#include "nvCapture.h"
#include <time.h>

#include "targa.c"

#define CAPTURE_MAX_FILESIZE  0x80000000          // 2 GB
#define CAPTURE_MAX_TEMP      (65536 * 10)
#define CAPTURE_MAX_INCS      16                  // maximum number of times file can be inc'ed
                                                  // before temp buffer is flushed
#define CAPTURE_STRING_SIZE   64

#define CAPTURE_FILE_FORMAT_RECORD "%s\\%s%05d.acf"
#define CAPTURE_FILE_FORMAT_PLAY   "%s\\%s%06d.tga"

// globals
HANDLE hCaptureFile                        = NULL;
BYTE   abTempSpace[CAPTURE_MAX_TEMP];
DWORD  adwTempOffset[CAPTURE_MAX_INCS];
DWORD  adwTempSerial[CAPTURE_MAX_INCS];               // the serial number of the file for which this temp data is eventually destined
DWORD  dwTempOffsetCount                   = 0;
DWORD  dwTempOffsetCurrent                 = 0;
DWORD  dwSerialNumber                      = 0;
DWORD  dwPlaySerialNumber                  = 0;
DWORD  dwFileSize                          = 0;
char   szPath[CAPTURE_STRING_SIZE]         = "|||||||";
char   szFilename[CAPTURE_STRING_SIZE]     = "|||||||";
char   szPlayPath[CAPTURE_STRING_SIZE]     = "|||||||";
char   szPlayFilename[CAPTURE_STRING_SIZE] = "|||||||";

// ---------------------------- RECORDING -----------------------------------

//
// captureFilePurge
// remove all old files beginning with serial number dwSerial
//
int captureFilePurge
(
    DWORD dwSerial
)
{
    char  filename[128];
    BOOL  bSuccess;

    do {

        // generate filename
        sprintf (filename, CAPTURE_FILE_FORMAT_RECORD, szPath, szFilename, dwSerial);
        // delete
        bSuccess = DeleteFile (filename);
        // increment
        dwSerial++;

    } while (bSuccess);

    return 1;
}

//
// captureFileOpen
// opens a new capture file with the current serial number
//
int captureFileOpen
(
    DWORD dwSerial
)
{
    char filename[128];

    if (hCaptureFile)
    {
        // shouldn't re-open a file
        __asm int 3
    }

    // generate filename
    sprintf (filename, CAPTURE_FILE_FORMAT_RECORD, szPath, szFilename, dwSerial);

    // open file
    hCaptureFile = CreateFile (filename, GENERIC_WRITE, 0,0, OPEN_ALWAYS, 0,0);
    if (hCaptureFile == INVALID_HANDLE_VALUE)
    {
        hCaptureFile = NULL;
        return 0;
    }

    // set the file pointer to the end. this appears to be
    // the only way to append to a file. how retarded.
    SetFilePointer(hCaptureFile, 0,0, FILE_END);

    return 1;
}

//
// captureFileClose
// closes the current capture file, if any
//
int captureFileClose
(
    void
)
{
    if (hCaptureFile)
    {
        BOOL bSuccess = CloseHandle (hCaptureFile);
        if (!bSuccess)
        {
            __asm int 3
        }
        hCaptureFile = NULL;
    }

    return 1;
}

//
// captureFileInc
// close the current file and open the next in the series
//
int captureFileInc
(
    void
)
{
    dwSerialNumber++;
    dwFileSize = 0;
    return 1;
}

//
// captureLog
// write stuff into the log file (or into a temporary buffer for lack of the former)
//
int captureLog
(
    void     *pBuffer,
    unsigned  uCount
)
{
    DWORD dw;
    DWORD dwCount;

    // do we have to save off buffered data first?
    if (dwTempOffsetCount)
    {
        for (dwCount=0; dwCount<dwTempOffsetCount; dwCount++)
        {
            DWORD dwStart = dwCount ? adwTempOffset[dwCount-1] : 0;
            DWORD dwEnd   = adwTempOffset[dwCount];
            if ((!captureFileOpen(adwTempSerial[dwCount]))
                ||
                (!WriteFile (hCaptureFile, &(abTempSpace[dwStart]), dwEnd-dwStart, &dw, NULL)))
            {
                captureFileClose();
                goto noFileAccess;
            }
            captureFileClose();
        }
        // reset, now that it is empty
        dwTempOffsetCount = 0;
    }

    // write out new data
    if (captureFileOpen(dwSerialNumber))
    {
        if (!WriteFile (hCaptureFile,pBuffer,uCount,&dw,NULL))
        {
            captureFileClose();
            goto noFileAccess;
        }
        captureFileClose();
        dwFileSize += uCount;
        if (dwFileSize > CAPTURE_MAX_FILESIZE)
        {
            // don't let files grow too large
            captureFileInc();
        }
        return 1;
    }

noFileAccess:

    if ((dwTempOffsetCount == 0) ||
        (adwTempSerial[dwTempOffsetCount-1] != dwSerialNumber))
    {
        if (dwTempOffsetCount < CAPTURE_MAX_INCS)
        {
            // move to the next file and save a pointer to this location in the buffer
            adwTempOffset[dwTempOffsetCount] = dwTempOffsetCurrent;
            adwTempSerial[dwTempOffsetCount] = dwSerialNumber;
            dwTempOffsetCount++;
        }
        else
        {
            // we're out of array space
            __asm int 3;
            return 0;
        }
    }

    if (adwTempOffset[dwTempOffsetCount-1] + uCount > CAPTURE_MAX_TEMP)
    {
        DWORD err = GetLastError();
        __asm mov eax,[err]
        __asm int 3; // out of temp space
        return 0;
    }

    // save off temporarily
    memcpy (abTempSpace + adwTempOffset[dwTempOffsetCount-1], pBuffer, uCount);

    dwTempOffsetCurrent += uCount;
    adwTempOffset[dwTempOffsetCount-1] = dwTempOffsetCurrent;

    return 1;
}

//
// captureSetParams
// set name, path, and base serial number of recorded files
//
int captureSetParams
(
    char  *szBasePath,      // directory in which to dump captures (NULL is legal)
    char  *szBaseFilename,  // base name of files to write
    DWORD  dwBaseFilenum    // the serial number of the first file recorded
)
{
    DWORD dwStrEnd;

    // get rid of terminating '\' (if it exists) since
    // we'll tack one on later and having two is ugly
    dwStrEnd = strlen(szBasePath) - 1;
    if (szBasePath[dwStrEnd] == '\\') {
        szBasePath[dwStrEnd] = '\0';
    }

    // if things have changed, update and reset
    if ((strcmp (szBasePath, szPath)) ||
        (strcmp (szBaseFilename, szFilename)))
    {
        if (szBasePath) {
            strncpy (szPath, szBasePath, CAPTURE_STRING_SIZE);
        }
        if (szBaseFilename) {
            strncpy (szFilename, szBaseFilename, CAPTURE_STRING_SIZE);
        }
        dwSerialNumber = dwBaseFilenum;
        dwFileSize     = 0;
        captureFilePurge (dwBaseFilenum);
    }

    return 1;
}

//
// captureStartup
// should be called only once to initialize root capture file
//
int captureStartup
(
    char  *szBasePath,      // directory in which to dump captures (NULL is legal)
    char  *szBaseFilename,  // base name of files to write
    DWORD  dwBaseFilenum    // the serial number of the first file recorded
)
{
    CAPTURE_INFORMATION  info;
    struct tm           *newtime;
    time_t               aclock;
    static BOOL          bCalled = FALSE;

    // defer multiple creations
    if (bCalled) {
        // this shouldn't get called more than once
        __asm int 3
        return 0;
    }
    else {
        bCalled = TRUE;
    }

    captureSetParams (szBasePath, szBaseFilename, dwBaseFilenum);

    // reset temporary file buffer stuff
    dwTempOffsetCount = 0;

    // write header
    memset (&info,0,sizeof(info));
    info.dwJmpCommand         = CAPTURE_JMP_COMMAND;
    info.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
    info.dwExtensionID        = CAPTURE_XID_INFORMATION;
    info.dwSchema             = CAPTURE_CURRENT_SCHEMA;
    strcpy (info.szInformation,"Capture V1.00::");
#if defined(NVDD32)
    strcat (info.szInformation,"Direct3D::");
#elif defined(__GL_ICD)
    strcat (info.szInformation,"OpenGL::");
#else
    strcat (info.szInformation,"Unknown::");
#endif
    time (&aclock);
    newtime = localtime(&aclock);
    strcat (info.szInformation,asctime(newtime));
    captureLog (&info,sizeof(info));

    // done
    return 1;
}

//
// captureShutdown
// called once when all capturing is done
//
int captureShutdown
(
    void
)
{
    // close the file, if any
    captureFileClose();

    return 1;
}

// ------------------------------ PLAYBACK ----------------------------------

//
// capturePlayFileInc
// increment the playback file counter
//
int capturePlayFileInc
(
    void
)
{
    dwPlaySerialNumber++;
    return 1;
}

//
// capturePlay
// copy the contents of a file into the render target
//
int capturePlay
(
    void *pRenderTarget,
    DWORD dwPitch,
    DWORD dwWidth,
    DWORD dwHeight
)
{
    char  filename[128];
    BYTE *pBuffer;
    int   iTGAWidth, iTGAHeight;
    DWORD i;

    // clear the frame buffer, so we know we're in playback mode
    // even if the file copy fails for some reason
    for (i=0; i<dwHeight; i++)
    {
        memset ((BYTE *)pRenderTarget+i*dwPitch, 0xff, dwWidth);
    }

    // generate filename
    sprintf (filename, CAPTURE_FILE_FORMAT_PLAY, szPlayPath, szPlayFilename, dwPlaySerialNumber);

    // copy contents to render target
    read_targa_file (filename, &pBuffer, &iTGAWidth, &iTGAHeight);

    if (!pBuffer)
    {
        return 0;
    }

    // sanity checks
    if ((4*iTGAWidth != (int)dwWidth) || (iTGAHeight != (int)dwHeight))
    {
        // sizes must match
        __asm int 3
        return 0;
    }

    // copy into the render target
    for (i=0; i<dwHeight; i++)
    {
        memcpy ((BYTE *)pRenderTarget+i*dwPitch, pBuffer+i*dwWidth, dwWidth);
    }

    // free the buffer
    GlobalFree (pBuffer);

    return 1;
}

//
// capturePlaySetParams
// change the name / path / base serial number of the playback files
//
int capturePlaySetParams
(
    char  *szBasePath,      // directory from which to read bitmaps
    char  *szBaseFilename,  // base name of files to read
    DWORD  dwBaseFilenum    // the serial number of the first file played back
)
{
    DWORD dwStrEnd;

    // get rid of terminating '\' (if it exists) since
    // we'll tack one on later and having two is ugly
    dwStrEnd = strlen(szBasePath) - 1;
    if (szBasePath[dwStrEnd] == '\\') {
        szBasePath[dwStrEnd] = '\0';
    }

    // if things have changed, update and reset
    if ((strcmp (szBasePath, szPlayPath)) ||
        (strcmp (szBaseFilename, szPlayFilename)))
    {
        if (szBasePath) {
            strncpy (szPlayPath, szBasePath, CAPTURE_STRING_SIZE);
        }
        if (szBaseFilename) {
            strncpy (szPlayFilename, szBaseFilename, CAPTURE_STRING_SIZE);
        }
        dwPlaySerialNumber = dwBaseFilenum;
    }

    return 1;
}

//
// capturePlayStartup
// should be called only once to initialize root capture file
//
int capturePlayStartup
(
    char  *szBasePath,      // directory from which to read bitmaps
    char  *szBaseFilename,  // base name of files to read
    DWORD  dwBaseFilenum    // the serial number of the first file played back
)
{
    capturePlaySetParams (szBasePath, szBaseFilename, dwBaseFilenum);

    // done
    return 1;
}

//
// capturePlayShutdown
// called once when all playback is done
//
int capturePlayShutdown
(
    void
)
{
    return 1;
}

