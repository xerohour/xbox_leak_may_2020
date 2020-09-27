/*
 *
 * tape.cpp
 *
 * CTapeDevice
 *
 */

#include "precomp.h"

BOOL CTapeDevice::FMount(LPCSTR szName, BOOL fWrite)
{
    m_hdev = CreateFile(szName, GENERIC_READ | GENERIC_WRITE, 0, 0,
        OPEN_EXISTING, 0, NULL);
    if(m_hdev != INVALID_HANDLE_VALUE) {
        if(!FSetupTapeDevice()) {
            CloseHandle(m_hdev);
            m_hdev = NULL;
        }
    } else
        m_hdev = NULL;
    m_fWriteOnly = !!fWrite;
    m_ptod = NULL;
    return FRewind();
}

CTapeDevice::~CTapeDevice()
{
    FUnmount();
}

BOOL CTapeDevice::FSetupTapeDevice(void)
{
    DWORD err;
    DWORD cb;
    TAPE_GET_DRIVE_PARAMETERS dpGet;
    TAPE_SET_DRIVE_PARAMETERS dpSet;

    if(!m_hdev)
        return FALSE;
    err = PrepareTape(m_hdev, TAPE_LOAD, FALSE);
    if(err != NO_ERROR && err != ERROR_MEDIA_CHANGED)
        return FALSE;
    if(NO_ERROR != GetTapeStatus(m_hdev))
        return FALSE;
    cb = sizeof dpGet;
    if(NO_ERROR != GetTapeParameters(m_hdev, GET_TAPE_DRIVE_INFORMATION,
            &cb, &dpGet) || cb != sizeof dpGet)
        return FALSE;
    /* Keep parameters the same, except for Compression */
    dpSet.ECC = dpGet.ECC;
    dpSet.Compression = FALSE;
    dpSet.ReportSetmarks = dpGet.ReportSetmarks;
    dpSet.EOTWarningZoneSize = dpGet.EOTWarningZoneSize;
    return NO_ERROR == SetTapeParameters(m_hdev, SET_TAPE_DRIVE_INFORMATION,
        &dpSet);
}

BOOL CTapeDevice::FMount(HANDLE h, BOOL fWrite)
{
    m_hdev = h;
    m_ptod = new TOD;
    memset(m_ptod, 0, sizeof TOD);
    m_fWriteOnly = !!fWrite; 
    return FRewind();
}

BOOL CTapeDevice::FMountAsTapeOrDisk(LPCSTR sz, BOOL fWrite)
{
    if(0 == strncmp(sz, "\\\\.", 3))
        return FMount(sz, fWrite);
    if(fWrite)
        m_hdev = CreateFile(sz, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
    else
        m_hdev = CreateFile(sz, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, 0, NULL);
    if(m_hdev != INVALID_HANDLE_VALUE) {
        if(FMount(m_hdev, fWrite))
            return TRUE;
        CloseHandle(m_hdev);
    }
    m_hdev = NULL;
    return FALSE;
}

BOOL CTapeDevice::FUnmount(void)
{
    BOOL fRet = TRUE;

    if(!m_hdev)
        return FALSE;

    /* Close any open file */
    if(m_pbBufCur)
        fRet = FCloseFile();
    /* If writing, write the final tapemark */
    if(m_fWriteOnly)
        fRet = FWriteFilemark() && fRet;
    /* Unmount the tape */
    if(!m_ptod)
        fRet = (NO_ERROR == PrepareTape(m_hdev, TAPE_UNLOAD, TRUE)) && fRet;
    if(m_hdev)
        CloseHandle(m_hdev);
    m_hdev = NULL;
    m_dwFlags = 0;
    return fRet;
}

DWORD CTapeDevice::ReadFile(HANDLE h, LPVOID pv, DWORD cb, LPDWORD pcb,
    LPOVERLAPPED p)
{
    DWORD dwRet;

    if(m_ptod) {
        /* If we're on a filemark, we'll return a filemark failure and
         * prep for the next bit of data */
        if(m_ptod->cbToFilemark.QuadPart == 0) {
            *pcb = 0;
            SetAfterFilemark();
            SetLastError(ERROR_FILEMARK_DETECTED);
            return 0;
        }
        if(m_ptod->cbToFilemark.HighPart == 0 &&
                m_ptod->cbToFilemark.LowPart < cb)
            /* We'll run out of data prior to the next filemark, so cut the
             * read short */
            cb = m_ptod->cbToFilemark.LowPart;
        dwRet = ::ReadFile(h, pv, cb, pcb, p);
        m_ptod->cbToFilemark.QuadPart -= *pcb;
    } else
        dwRet = ::ReadFile(h, pv, cb, pcb, p);

    return dwRet;
}

DWORD CTapeDevice::WriteFile(HANDLE h, LPCVOID pv, DWORD cb, LPDWORD pcb,
    LPOVERLAPPED p)
{
    return ::WriteFile(h, pv, cb, pcb, p);
}

BOOL CTapeDevice::FSeekToFilemark(int c)
{
    if(m_fFatalError || m_fWriteOnly)
        return FALSE;

    if(m_ptod) {
        if(c < 0)
            return FALSE;
        while(c--) {
            /* Skip as many bytes as remain in this file */
            if(!SetFilePointerEx(m_hdev, m_ptod->cbToFilemark, NULL,
                FILE_CURRENT))
            {
                m_fFatalError = TRUE;
                return FALSE;
            }
            SetAfterFilemark();
        }
        return TRUE;
    } else
        return NO_ERROR == SetTapePosition(m_hdev, TAPE_SPACE_FILEMARKS, 0,
            c, c < 0 ? -1 : 0, FALSE);
}

BOOL CTapeDevice::FWriteFilemark(void)
{
    LARGE_INTEGER li;
    DWORD cb;

    if(m_fFatalError || !m_fWriteOnly)
        return FALSE;

    if(m_ptod) {
        /* First get the current pointer */
        li.QuadPart = 0;
        if(!SetFilePointerEx(m_hdev, li, &li, FILE_CURRENT)) {
fatal:
            m_fFatalError = TRUE;
            return FALSE;
        }
        m_ptod->cbToFilemark.QuadPart = li.QuadPart -
            m_ptod->liCurWriteMark.QuadPart - sizeof li;
        if(!SetFilePointerEx(m_hdev, m_ptod->liCurWriteMark, NULL, FILE_BEGIN))
            goto fatal;
        if(!::WriteFile(m_hdev, &m_ptod->cbToFilemark,
                sizeof m_ptod->cbToFilemark, &cb, NULL) || cb !=
                sizeof m_ptod->cbToFilemark)
            goto fatal;
        if(!SetFilePointerEx(m_hdev, li, NULL, FILE_BEGIN))
            goto fatal;
        m_ptod->liCurWriteMark = li;
        li.QuadPart = 0;
        if(!::WriteFile(m_hdev, &li, sizeof li, &cb, NULL) || cb != sizeof li)
            goto fatal;
        return TRUE;
    } else
        return NO_ERROR == WriteTapemark(m_hdev, TAPE_FILEMARKS, 1, FALSE);
}

void CTapeDevice::SetAfterFilemark(void)
{
    DWORD cb;

    if(m_ptod && !m_fFatalError && !m_fWriteOnly) {
        /* We should be sitting on a filemark, so read it */
        if(!::ReadFile(m_hdev, &m_ptod->cbToFilemark,
                sizeof m_ptod->cbToFilemark, &cb, NULL) || cb !=
                sizeof m_ptod->cbToFilemark)
            m_fFatalError = TRUE;
    }
}

BOOL CTapeDevice::FRewindNoHdrs(void)
{
    BOOL fRet;

    m_xsum.Reset();
    if(!m_hdev)
        return FALSE;

    /* Close any open file */
    if(m_pbBufCur)
        FCloseFile();
    m_fFatalError = FALSE;
    m_fAtEOT = FALSE;

    if(m_ptod) {
        if(INVALID_SET_FILE_POINTER == SetFilePointer(m_hdev, 0, NULL,
                FILE_BEGIN))
            fRet = FALSE;
        else {
            SetAfterFilemark();
            fRet = TRUE;
        }
    } else
        fRet = SetTapePosition(m_hdev, TAPE_REWIND, 0, 0, 0, TRUE) == NO_ERROR;
    if(!fRet)
        m_fFatalError = TRUE;
    return fRet;
}

BOOL CTapeDevice::FRewind(void)
{
    if(FRewindNoHdrs()) {
        if(m_fWriteOnly)
            WriteVolumeHeader();
        else
            ReadVolumeHeader();
    } else
        m_fFatalError = TRUE;
    return !m_fFatalError;
}

void CTapeDevice::ReadVolumeHeader(void)
{
    TAPE_SET_MEDIA_PARAMETERS mp;
    BYTE rgbVOL1[80];
    DWORD cb;

    /* Configure for block reads */
    mp.BlockSize = 80;
    if(!m_ptod && NO_ERROR != SetTapeParameters(m_hdev,
        SET_TAPE_MEDIA_INFORMATION, &mp))
    {
fatal:
        m_fFatalError = TRUE;
        return;
    }

    /* Read the VOL1 block */
    if(!ReadFile(m_hdev, rgbVOL1, sizeof rgbVOL1, &cb, NULL) ||
            cb != sizeof rgbVOL1 ||  *(DWORD *)rgbVOL1 != '1LOV')
        goto fatal;

    /* Don't really care what the contents are */
}

void CTapeDevice::WriteVolumeHeader(void)
{
    TAPE_SET_MEDIA_PARAMETERS mp;
    BYTE rgbVOL1[80];
    LARGE_INTEGER li;
    DWORD cb;
    int ich;

    if(m_ptod) {
        /* We should be ready to write a filemark */
        li.QuadPart = m_ptod->liCurWriteMark.QuadPart = 0;
        if(!::WriteFile(m_hdev, &li, sizeof li, &cb, NULL) || cb != sizeof li)
            goto fatal;
    } else {
        /* Configure for block writes */
        mp.BlockSize = 80;
        if(!m_ptod && NO_ERROR != SetTapeParameters(m_hdev,
            SET_TAPE_MEDIA_INFORMATION, &mp))
        {
fatal:
            m_fFatalError = TRUE;
            return;
        }
    }

    /* Construct the VOL1 block */
    memset(rgbVOL1, ' ', sizeof rgbVOL1);
    *(DWORD *)rgbVOL1 = '1LOV';
    rgbVOL1[79] = '4';

    /* Write it out */
    if(!WriteFile(m_hdev, rgbVOL1, sizeof rgbVOL1, &cb, NULL) ||
            cb != sizeof rgbVOL1)
        goto fatal;
}

DWORD CTapeDevice::CbRead(DWORD cb, PBYTE pbBuf)
{
    DWORD cbReq = cb;
    DWORD cbRet;
    LPBYTE pbBufT;
    DWORD cblk;
    const BYTE *pbSum = pbBuf;

    if(!m_hdev || !m_pbBufCur || m_fWriteOnly || m_fAtEOF)
        return 0;

    /* First fetch data from the current block if there is any */
    if(m_ibBlkCur) {
curblk:
        if(m_ibBlkCur + cb <= m_cbReadCur)
            cbRet = cb;
        else
            cbRet = m_cbReadCur - m_ibBlkCur;
        memcpy(pbBuf, m_pbBufCur + m_ibBlkCur, cbRet);
        m_ibBlkCur += cbRet;
        pbBuf += cbRet;
        cb -= cbRet;
        if(m_ibBlkCur == m_cbReadCur)
            m_ibBlkCur = 0;
    }

    /* Read all of the data that we can */
    while(cb && !m_fAtEOF) {
        /* If we know we have at least a full block waiting, then we can read
         * straight into the user buffer.  Otherwise, we need to read into our
         * buffer in preparation for a partial block read */
        if(cb >= m_cbBlkCur) {
            pbBufT = pbBuf;
            cblk = cb / m_cbBlkCur;
        } else {
            pbBufT = m_pbBufCur;
            cblk = 1;
        }
        if(!ReadFile(m_hdev, pbBufT, m_cbBlkCur * cblk, &cbRet, NULL) &&
            cbRet == 0)
        {
            /* We hit a filemark, or a fatal error, so mark EOF */
            if(GetLastError() != ERROR_FILEMARK_DETECTED)
                m_fFatalError = TRUE;
            m_fAtEOF = TRUE;
            break;
        }
        if(cbRet == m_cbBlkCur)
            m_cBlkCur += cblk;
        else
            ++m_cBlkCur;
        if(pbBufT == pbBuf) {
            pbBuf += cbRet;
            cb -= cbRet;
        } else {
            /* We read a partial block, so we need to copy the appropriate
             * amount of data into the user buffer.  We have that code above,
             * so we just jump there.  And since we may not have fulfilled
             * the read request due to a partial read of a full-sized variable
             * length block, we can fall through into the read loop again */
            m_cbReadCur = cbRet;
            goto curblk;
       }
    }

    if(cbReq - cb > 0)
        m_xsum.SumBytes(pbSum, cbReq - cb);
    return cbReq - cb;
}

DWORD CTapeDevice::CbWrite(DWORD cb, const BYTE *pbBuf)
{
    DWORD cbReq = cb;
    DWORD cbRet;

    if(!m_hdev || !m_pbBufCur || !m_fWriteOnly || m_fFatalError)
        return 0;

    m_xsum.SumBytes(pbBuf, cb);

    /* First fill data from the current block if there is a pending partial
     * write */
    if(m_ibBlkCur) {
        if(m_ibBlkCur + cb <= m_cbBlkCur)
            cbRet = cb;
        else
            cbRet = m_cbBlkCur - m_ibBlkCur;
        memcpy(m_pbBufCur + m_ibBlkCur, pbBuf, cbRet);
        m_ibBlkCur += cbRet;
        pbBuf += cbRet;
        cb -= cbRet;
        if(m_ibBlkCur == m_cbBlkCur) {
            /* Need to flush this block */
            if(!WriteFile(m_hdev, m_pbBufCur, m_cbBlkCur, &cbRet, NULL) ||
                cbRet != m_cbBlkCur)
            {
                m_fFatalError = TRUE;
                return 0;
            }
            ++m_cBlkCur;
            m_ibBlkCur = 0;
        }
    }

    /* Write all of the full blocks that we can */
    if(cb >= m_cbBlkCur) {
        DWORD cblk = cb / m_cbBlkCur;
        if(!WriteFile(m_hdev, pbBuf, m_cbBlkCur * cblk, &cbRet, NULL) ||
                cbRet != m_cbBlkCur * cblk)
            m_fFatalError = TRUE;
        else
            m_cBlkCur += cblk;
        pbBuf += cbRet;
        cb -= cbRet;
    }

    /* If there is still data to go and we know there's still room to write
     * file data, copy the remainder into a buffer to flush later */
    if(cb && !m_fAtEOF) {
        m_ibBlkCur = cb;
        memcpy(m_pbBufCur, pbBuf, cb);
        cb = 0;
    }

    return cbReq - cb;
}

BOOL CTapeDevice::FCloseFile(void)
{
    BOOL fRet;
    DWORD cb;

    if(!m_hdev || !m_pbBufCur || m_fFatalError)
        return FALSE;

    if(m_fWriteOnly) {
        /* If we have a pending write, flush it now */
        if(m_ibBlkCur && !m_fAtEOF) {
            DWORD cbRet;

            if(m_fVarBlks)
                cb = m_ibBlkCur;
            else {
                memset(m_pbBufCur + m_ibBlkCur, m_bWritePad,
                    m_cbBlkCur - m_ibBlkCur);
                m_xsum.SumBytes(m_pbBufCur + m_ibBlkCur,
                    m_cbBlkCur - m_ibBlkCur);
                cb = m_cbBlkCur;
            }
            if(!WriteFile(m_hdev, m_pbBufCur, cb, &cbRet, NULL) ||
                    cbRet != cb)
                m_fFatalError = TRUE;
            else
                ++m_cBlkCur;
        }
        fRet = !m_fFatalError;
        if(fRet)
            /* Write a filemark prior to the next file */
            fRet = FWriteFilemark();
        if(fRet)
            /* Now we need to write the EOF records */
            fRet = FWriteFileHeaders(TRUE);
        free(m_szFilenameCur);
    } else {
        if(!m_fAtEOF)
            fRet = FSeekToFilemark(1);
        else
            fRet = TRUE;
        if(fRet)
            /* Skip over the EOF records */
            fRet = FSeekToFilemark(1);
    }

    /* Mark as no file open */
    delete m_pbBufCur;
    m_pbBufCur = NULL;
    if(!fRet)
        m_fFatalError = TRUE;
    return fRet;
}

BOOL CTapeDevice::FReadFile(LPSTR szName, LPDWORD lpdwDataBlkSize,
    LPDWORD lpdwDataPerPhy)
{
    BYTE rgbHDR1[80];
    BYTE rgbHDR2[80];
    TAPE_SET_MEDIA_PARAMETERS mp;
    TAPE_GET_DRIVE_PARAMETERS dp;
    DWORD cb;
    BYTE *pch;
    char szT[8];
    DWORD dwBlkSize;
    DWORD dwDataPerPhy;

    if(!m_hdev || m_fFatalError || m_fWriteOnly || m_fAtEOT)
        return FALSE;

    if(m_pbBufCur) {
        /* File is open, so close it */
        FCloseFile();
    }

    /* Configure for block reads */
    mp.BlockSize = 80;
    if(!m_ptod && NO_ERROR != SetTapeParameters(m_hdev,
        SET_TAPE_MEDIA_INFORMATION, &mp))
    {
fatal:
        m_fFatalError = TRUE;
        return FALSE;
    }

    /* Read the HDR1 block */
    if(!ReadFile(m_hdev, rgbHDR1, sizeof rgbHDR1, &cb, NULL) ||
        cb != sizeof rgbHDR1 ||  *(DWORD *)rgbHDR1 != '1RDH')
    {
        /* Assume end of tape */
        m_fAtEOT = TRUE;
        return FALSE;
    }

    /* Read the HDR2 blck */
    if(!ReadFile(m_hdev, rgbHDR2, sizeof rgbHDR2, &cb, NULL) ||
            cb != sizeof rgbHDR2 ||  *(DWORD *)rgbHDR2 != '2RDH')
        goto fatal;

    /* Seek to the next filemark */
    if(!FSeekToFilemark(1))
        goto fatal;

    /* We only support fixed-record files */
    if(rgbHDR2[4] != 'F')
        goto fatal;

    /* Fetch the filename */
    pch = &rgbHDR1[21];
    *pch = 0;
    while(*--pch == ' ')
        *pch = 0;
    strcpy(szName, (LPSTR)&rgbHDR1[4]);

    /* Report the record and block sizes */
    memcpy(szT, rgbHDR2 + 5, 5);
    szT[5] = 0;
    if(!sscanf(szT, "%d", &m_cbBlkCur))
        goto fatal;
    memcpy(szT, rgbHDR2 + 10, 5);
    szT[5] = 0;
    if(!lpdwDataBlkSize)
        lpdwDataBlkSize = &dwBlkSize;
    if(!sscanf(szT, "%d", lpdwDataBlkSize))
        goto fatal;
    if(!lpdwDataPerPhy)
        lpdwDataPerPhy = &dwDataPerPhy;
    *lpdwDataPerPhy = m_cbBlkCur / *lpdwDataBlkSize;
    /* We'll refuse to deal with files whose physical blocks aren't multiples
     * of data blocks */
    if(*lpdwDataPerPhy * *lpdwDataBlkSize != m_cbBlkCur)
        goto fatal;

    /* Configure the block size */
    mp.BlockSize = m_cbBlkCur;
    if(!m_ptod && NO_ERROR != SetTapeParameters(m_hdev,
        SET_TAPE_MEDIA_INFORMATION, &mp))
    {
        /* The block size in the file is not one that the device likes, so
         * we'll switch to variable block mode and set our buffer to the
         * maximum block size that the device can handle */
        cb = sizeof dp;
        if(NO_ERROR != GetTapeParameters(m_hdev, GET_TAPE_DRIVE_INFORMATION,
                &cb, &dp) || cb != sizeof dp)
            goto fatal;
        m_cbBlkCur = dp.MaximumBlockSize;
        mp.BlockSize = 0;
        if(!m_ptod && NO_ERROR != SetTapeParameters(m_hdev,
                SET_TAPE_MEDIA_INFORMATION, &mp))
            goto fatal;
    }

    /* Allocate the read buffer and we're done */
    m_pbBufCur = new BYTE[m_cbBlkCur];
    if(!m_pbBufCur)
        goto fatal;
    m_ibBlkCur = 0;
    m_cBlkCur = 0;
    m_fAtEOF = FALSE;

    return TRUE;
}

BOOL CTapeDevice::FWriteFileHeaders(BOOL fEOF)
{
    BYTE rgbHDR1[80];
    BYTE rgbHDR2[80];
    TAPE_SET_MEDIA_PARAMETERS mp;
    DWORD cb;
    char szT[8];
    int ich;
    LPSTR szHDR = fEOF ? "EOF" : "HDR";

    if(m_fFatalError)
        return FALSE;

    /* Configure for block writes */
    mp.BlockSize = 80;
    if(!m_ptod && NO_ERROR != SetTapeParameters(m_hdev,
        SET_TAPE_MEDIA_INFORMATION, &mp))
    {
fatal:
        m_fFatalError = TRUE;
        return FALSE;
    }

    /* Contsruct the HDR1 block */
    strcpy(szT, "000000");
    sprintf((LPSTR)rgbHDR1, "%s1%-17.17s      0001%04d000101%s000000 %06d",
        szHDR, m_szFilenameCur, iFile, szT, m_cBlkCur);
    memset(rgbHDR1 + 60, ' ', 20);

    /* Write it out */
    if(!WriteFile(m_hdev, rgbHDR1, sizeof rgbHDR1, &cb, NULL) ||
            cb != sizeof rgbHDR1)
        goto fatal;

    /* Construct the HDR2 block */
    sprintf((LPSTR)rgbHDR2, "%s2F%05d%05d", szHDR, m_cbBlkCur, m_cbRecCur);
    memset(rgbHDR2 + 15, ' ', 65);

    /* Write it out */
    if(!WriteFile(m_hdev, rgbHDR2, sizeof rgbHDR2, &cb, NULL) ||
            cb != sizeof rgbHDR2)
        goto fatal;

    /* Write a filemark */
    if(!FWriteFilemark())
        goto fatal;

    return TRUE;
}

BOOL CTapeDevice::FWriteFile(LPCSTR szName, DWORD dwDataBlkSize,
    DWORD dwDataPerPhy)
{
    BYTE rgbHDR1[80];
    BYTE rgbHDR2[80];
    TAPE_SET_MEDIA_PARAMETERS mp;
    DWORD cb;
    char *pch;
    char szT[8];
    int ich;

    if(!m_hdev || m_fFatalError || !m_fWriteOnly)
        return FALSE;

    if(m_pbBufCur) {
        /* File is open, so close it */
        if(!FCloseFile())
            return FALSE;
    }

    m_cBlkCur = 0;
    m_cbBlkCur = dwDataBlkSize * dwDataPerPhy;
    m_cbRecCur = dwDataBlkSize;
    m_szFilenameCur = _strdup(szName);
    m_fVarBlks = FALSE;
    
    /* Need to upcase the filename */
    for(pch = m_szFilenameCur; *pch; ++pch) {
        if(islower(*pch))
            *pch = (char)toupper(*pch);
    }

    if(!FWriteFileHeaders(FALSE))
        return FALSE;

    /* Try to put the tape into our block size.  If it doesn't work, we'll
     * switch to a different size and write in variable mode */
    mp.BlockSize = m_cbBlkCur;
    if(!m_ptod && NO_ERROR != SetTapeParameters(m_hdev,
        SET_TAPE_MEDIA_INFORMATION, &mp))
    {
        /* This block size is not one the drive likes.  We'll put the drive
         * into variable block mode, but we'll still write out the data in
         * the user-requested block size -- except that the final block may be
         * incomplete if the user provides data which consists of a non-
         * integer number of blocks */
        mp.BlockSize = 0;
        if(!m_ptod && NO_ERROR != SetTapeParameters(m_hdev,
            SET_TAPE_MEDIA_INFORMATION, &mp))
        {
            m_fFatalError = TRUE;
            return FALSE;
        }
        m_fVarBlks = TRUE;
    }

    /* Allocate the write buffer and we're done */
    m_pbBufCur = new BYTE[m_cbBlkCur];
    m_ibBlkCur = 0;
    m_cBlkCur = 0;
    m_fAtEOF = FALSE;

    return TRUE;
}

BOOL CTapeDevice::FCopyEntire(CTapeDevice *ptap)
{
    BYTE rgb[8192];
    DWORD cb, cbT;
    int cMarks;
    TAPE_SET_MEDIA_PARAMETERS mp;

    if(!FRewindNoHdrs())
        return FALSE;
    if(!ptap->FRewindNoHdrs()) {
        FRewind();
        return FALSE;
    }

    /* Go into variable block mode on both devices */
    mp.BlockSize = 0;
    if(!m_ptod && NO_ERROR != SetTapeParameters(m_hdev,
            SET_TAPE_MEDIA_INFORMATION, &mp))
        goto error;
    if(!ptap->m_ptod && NO_ERROR != SetTapeParameters(ptap->m_hdev,
            SET_TAPE_MEDIA_INFORMATION, &mp))
        goto error;

    /* As long as we have data to copy, keep copying.  If we hit a filemark,
     * generate a filemark.  If we hit two filemarks in a row, we're done */

    cMarks = 0;
    while(cMarks < 2) {
        while(ReadFile(m_hdev, rgb, sizeof rgb, &cb, NULL) || cb) {
            cMarks = 0;
            if(!WriteFile(ptap->m_hdev, rgb, cb, &cbT, NULL) || cbT != cb) {
error:
                FRewind();
                ptap->FRewind();
                return FALSE;
            }
        }
        if(!ptap->FWriteFilemark())
            goto error;
        ++cMarks;
    }

    return TRUE;
}

BOOL CTapeDevice::FCopyToStm(CFileStmSeq *pstm, CCheckSum *pxsum)
{
    DWORD cb;

    if(!m_hdev || !m_pbBufCur || m_fWriteOnly || m_fAtEOF)
        return 0;

    /* If we have pending data in the read buffer, send it first */
    if(m_ibBlkCur) {
        cb = m_cbReadCur - m_ibBlkCur;
        if(pstm->CbWrite(cb, m_pbBufCur) != cb)
            return FALSE;
        m_xsum.SumBytes(m_pbBufCur, cb);
        if(pxsum)
            pxsum->SumBytes(m_pbBufCur, cb);
        m_ibBlkCur = 0;
    }

    /* We'll continue reading in one-block chunks until we're out of data */
    for(;;) {
        if(!ReadFile(m_hdev, m_pbBufCur, m_cbBlkCur, &cb, NULL) && cb == 0) {
            /* We hit a filemark, or a fatal error, so mark EOF */
            if(GetLastError() != ERROR_FILEMARK_DETECTED)
                m_fFatalError = TRUE;
            m_fAtEOF = TRUE;
            return !m_fFatalError;
        }
        m_xsum.SumBytes(m_pbBufCur, cb);
        if(pxsum)
            pxsum->SumBytes(m_pbBufCur, cb);
        if(pstm->CbWrite(cb, m_pbBufCur) != cb)
            return FALSE;
    }
}

BOOL CTapeDevice::FCopyFromStm(CFileStmSeq *pstm, CCheckSum *pxsum)
{
    DWORD cb;

    if(!m_hdev || !m_pbBufCur || !m_fWriteOnly || m_fAtEOF)
        return 0;

    /* We'll repeatedly read data into our write buffer; when we have a full
     * buffer, we'll do the write */
    for(;;) {
        while(m_ibBlkCur < m_cbBlkCur) {
            cb = pstm->CbRead(m_cbBlkCur - m_ibBlkCur, m_pbBufCur +
                m_ibBlkCur);
            if(cb == 0)
                /* There was no data to read, so we're done */
                return TRUE;
            m_xsum.SumBytes(m_pbBufCur + m_ibBlkCur, cb);
            if(pxsum)
                pxsum->SumBytes(m_pbBufCur + m_ibBlkCur, cb);
            m_ibBlkCur += cb;
        }
        /* We've filled up our block, so write it out */
        if(!WriteFile(m_hdev, m_pbBufCur, m_cbBlkCur, &cb, NULL) ||
            cb != m_cbBlkCur)
        {
            m_fFatalError = TRUE;
            return FALSE;
        }
        m_ibBlkCur = 0;
    }
}
