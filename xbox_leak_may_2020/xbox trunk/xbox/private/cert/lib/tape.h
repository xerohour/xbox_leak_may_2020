/*
 *
 * tape.h
 *
 * CTapeDevice
 *
 */

#ifndef _TAPE_H
#define _TAPE_H

#include <filestm.h>

class CTapeDevice : public CFileStmSeq {
public:
    CTapeDevice(void) : m_hdev(NULL), m_pbBufCur(NULL), m_dwFlags(0),
        m_bWritePad(0) {}
    virtual ~CTapeDevice();

    BOOL FMount(LPCSTR, BOOL fWrite);
    BOOL FMount(HANDLE, BOOL fWrite);
    BOOL FMountAsTapeOrDisk(LPCSTR, BOOL fWrite);
    BOOL FUnmount(void);
    BOOL FMounted(void) { return m_hdev != NULL; }
    BOOL FRewind(void);
    BOOL FReadFile(LPSTR szName, LPDWORD lpdwDataBlkSize,
        LPDWORD lpdwDataPerPhy);
    BOOL FWriteFile(LPCSTR szName, DWORD dwDataBlkSize=2048,
        DWORD dwDataPerPhy=16);
    BOOL FCloseFile(void);
    BOOL FAtEnd(void) { return !m_fFatalError && m_fAtEOT; }
    void SetWritePadByte(BYTE b) { m_bWritePad = b; }

    virtual BOOL FIsOpen(void) { return m_pbBufCur != NULL; }
    virtual DWORD CbRead(DWORD cb, PBYTE pbBuf);
    virtual DWORD CbWrite(DWORD cb, const BYTE *pbBuf);
    virtual BOOL FCopyToStm(CFileStmSeq *pstm, CCheckSum *pxsum=NULL);

    DWORD DwCheckSum(void) { return m_xsum.DwFinalSum(); }

    BOOL FCopyEntire(CTapeDevice *ptap);

protected:
    virtual BOOL FPreferredReceive(void) { return TRUE; }
    virtual BOOL FCopyFromStm(CFileStmSeq *pstm, CCheckSum *pxsum);

    struct TOD // tape on disk
    {
        LARGE_INTEGER liCurWriteMark;
        LARGE_INTEGER cbToFilemark;
    };

    HANDLE m_hdev;
    TOD *m_ptod;

private:
    DWORD m_cbBlkCur;
    DWORD m_ibBlkCur;
    DWORD m_cbReadCur;
    DWORD m_cBlkCur;
    BYTE *m_pbBufCur;
    LPSTR m_szFilenameCur;
    DWORD m_cbRecCur;
    BYTE m_bWritePad;
    int iFile;
    union {
        struct {
            BOOL m_fAtEOF:1,
                m_fWriteOnly:1,
                m_fFatalError:1,
                m_fAtEOT:1,
                m_fVarBlks:1,
                unused:27;
        };
        DWORD m_dwFlags;
    };
    DWORD m_cbToNextFilemark; // disk streams only
    CCheckSum m_xsum;

    BOOL FSetupTapeDevice(void);
    BOOL FRewindNoHdrs(void);
    void ReadVolumeHeader(void);
    void WriteVolumeHeader(void);
    DWORD ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
    DWORD WriteFile(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
    BOOL FSeekToFilemark(int);
    BOOL FWriteFilemark(void);
    void SetAfterFilemark(void);
    BOOL FWriteFileHeaders(BOOL fEOF);
};

#endif // _TAPE_H
