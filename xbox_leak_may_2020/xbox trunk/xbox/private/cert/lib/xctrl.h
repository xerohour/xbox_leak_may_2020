/*
 *
 * xctrl.h
 *
 * CXControl
 *
 */

#ifndef _XCTRL_H
#define _XCTRL_H

#include <dx2ml.h>

class CControlStruct
{
protected:
    struct XCXC {
        WORD ibData;
        WORD ibStruct;
        BYTE cbSize;
        BYTE fSwapBytes;
    };

    static BOOL FValidateZero(const BYTE *pb, const XCXC *rgxcz, int cxcz);
    void Import(const BYTE *pb, const XCXC *rgxcxc, int cxcxc);
    void Export(BYTE *pb, const XCXC *rgxcxc, int cxcxc) const;
};

class CXControl : public CControlStruct {
public:
    CXControl(void)
    {
        memset(this, 0, sizeof *this);
    }

    BOOL FInit(CFileStmSeq *pstm, DWORD dwBlkSize=2048);
    BOOL FInit(const BYTE *pb, DWORD dwBlkSize=2048);

    BOOL FWriteToStm(CFileStmSeq *pstm, DWORD dwBlkSize=2048);
    BOOL FWriteAPData(int ilyr, int ixph, CTapeDevice *ptapDst);

    void SetPsns(ULONG psnStart0, ULONG psnEnd0, ULONG psnEnd1)
    {
        m_psnStart0 = psnStart0;
        m_psnEnd0 = psnEnd0;
        m_psnEnd1 = psnEnd1;
        m_fSigned = FALSE;
    }

    void GetPsns(PULONG ppsnStart0, PULONG ppsnEnd0, PULONG ppsnEnd1)
    {
        *ppsnStart0 = m_psnStart0;
        *ppsnEnd0 = m_psnEnd0;
        *ppsnEnd1 = m_psnEnd1;
    }

    BOOL FValidate(DXML *pxml, CCryptContainer *pcc);
    BOOL FSetupAP(DXML *pdxml);
    BOOL FSign(CCryptContainer *pcc);

	BOOL FChallengeResponse(UCHAR id, DWORD dwChallenge, LPDWORD pdwResponse);

    bool operator==(const CXControl &xc)
    {
        return 0 == memcmp(&xc, this, sizeof *this);
    }

    ~CXControl()
    {
        if(m_pccSHA)
            delete m_pccSHA;
    }

private:
#pragma pack(push, 1)
    struct HCRE {
        BYTE bLevel;
        BYTE bID;
        BYTE rgbChallenge[4];
        BYTE bModifier;
        BYTE rgbResponse[4];
    };

    struct HCRT {
        BYTE bVer;
        BYTE chcre;
        HCRE rghcre[23];
    };

    struct DARE {
        BYTE bType;
        BYTE bID;
        BYTE bModifier;
        BYTE rgbUpper[3];
        BYTE rgbLower[3];
    };

    struct DART {
        BYTE bVer;
        BYTE cdare;
        DARE rgdare[23];
    };
#pragma pack(pop)

    CSHACrypt *m_pccSHA;
    BOOL m_fSigned;
    /* These fields don't in any way line up with the actual xcontrol data
     * block */
    DWORD m_dwDAC;
    DWORD m_psnStart0;
    DWORD m_psnEnd1;
    DWORD m_psnEnd0;
    BYTE m_fBCA;
    FILETIME m_ftContent;
    GUID m_guidContent;
    FILETIME m_ftAuth;
    BYTE m_bAuthSys;
    GUID m_guidAuth;
    BYTE m_rgbAuthHash[20];
    BYTE m_rgbAuthSig[256];
    HCRT m_hcrt;
    DART m_dart;
    BYTE m_rgbDartIndex[207];

    /* This is the representation of the AP data that we keep around to
     * fill in the placeholder regions */
    enum { aptPlaceholder, aptMirror, aptGroove, aptVelocity };
    struct APD {
        int apt;
        DWORD psnStart;
        BYTE rgbChallenge[4];
        BYTE rgbResponse[4];
    };
    APD m_rgapd[2][8];

    BOOL FAssemble(BYTE *pb, BOOL fIncludeDart, BOOL fIncludeHcrt);

    const static XCXC g_rgxcxc[];
    const static XCXC g_rgxcz[];
};

class CECMAControl : public CControlStruct
{
public:
    CECMAControl(void)
    {
        memset(this, 0, sizeof *this);
    }

    BOOL FInit(CFileStmSeq *pstm);
    BOOL FInit(const BYTE *pb);

    BOOL FValidate(DXML *pxml) const;
    BOOL FWriteToStm(CFileStmSeq *pstm);

    void SetPsns(ULONG psnStart0, ULONG psnEnd0, ULONG psnEnd1)
    {
        m_psnStart0 = psnStart0;
        m_psnEnd0 = psnEnd0;
        m_psnEnd1 = psnEnd1;
    }

    void GetPsns(PULONG ppsnStart0, PULONG ppsnEnd0, PULONG ppsnEnd1)
    {
        *ppsnStart0 = m_psnStart0;
        *ppsnEnd0 = m_psnEnd0;
        *ppsnEnd1 = m_psnEnd1;
    }

    bool operator==(const CXControl &xc)
    {
        return 0 == memcmp(&xc, this, sizeof *this);
    }

private:
    DWORD m_psnStart0;
    DWORD m_psnEnd1;
    DWORD m_psnEnd0;
    BYTE m_fBCA;

    const static XCXC g_rgxcxc[];
    const static XCXC g_rgxcz[];
};

#endif // _XCTRL_H
