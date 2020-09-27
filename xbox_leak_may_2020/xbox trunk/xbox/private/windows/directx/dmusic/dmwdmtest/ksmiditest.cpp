#include <stdio.h>
#include "stdafx.h"
#include "AllocatorMXF.h"
#include "MXF.h"
#include "UnpackerMXF.h"
#include "PackerMXF.h"
#include "Ks.h"
#include "KsMedia.h"

void HexDump(FILE *fout, LPBYTE pbData, ULONG cbData);

// Some data in KSMUSICFORMAT
//
static BYTE abKSMusicData[] =
{
    // Delta time             Length                    Message
    0x00, 0x00, 0x00, 0x00,   0x02, 0x00, 0x00, 0x00,   0xC0, 0x02, 0x00, 0x00,     // C0 02 patch change
    
    0x01, 0x00, 0x00, 0x00,   0x01, 0x00, 0x00, 0x00,   0x05, 0x00, 0x00, 0x00,     // xx 05 patch change, running status

    0x01, 0x00, 0x00, 0x00,   0x04, 0x00, 0x00, 0x00,   0x90, 0xFE, 0x50, 0x7F,     // Note on, active sense

    0x01, 0x00, 0x00, 0x00,   0x03, 0x00, 0x00, 0x00,   0xFE, 0x53, 0x7F, 0x00,     // Note on, active sense, running status

    0x01, 0x00, 0x00, 0x00,   0x01, 0x00, 0x00, 0x00,   0x80, 0x00, 0x00, 0x00,     // Note off, split across packets
    0x01, 0x00, 0x00, 0x00,   0x02, 0x00, 0x00, 0x00,   0x50, 0x7F, 0x00, 0x00,

    0x01, 0x00, 0x00, 0x00,   0x07, 0x00, 0x00, 0x00,   0xF0, 0x00, 0x00, 0x41,     // Complete SysEx   
                                                        0x00, 0x5F, 0xF7, 0x00,

    
    0x01, 0x00, 0x00, 0x00,   0x04, 0x00, 0x00, 0x00,   0xF0, 0x00, 0x00, 0x41,     // SysEx across packets
    0x01, 0x00, 0x00, 0x00,   0x03, 0x00, 0x00, 0x00,   0x00, 0x5F, 0xF7, 0x00,

    0x01, 0x00, 0x00, 0x00,   0x04, 0x00, 0x00, 0x00,   0xF0, 0x00, 0x00, 0x41,     // Incomplete SysEx

    0x01, 0x00, 0x00, 0x00,   0x03, 0x00, 0x00, 0x00,   0x80, 0x53, 0x7F, 0x00,     // Note off


    0x01, 0x00, 0x00, 0x00,   0x08, 0x00, 0x00, 0x00,   0xF0, 0x00, 0x00, 0x41,     // Complete SysEx w/ active sense
                                                        0xFE, 0x00, 0x5F, 0xF7,
};


class CSinkMXF : public CMXF
{
public:
    CSinkMXF(CAllocatorMXF *allocatorMXF);
    ~CSinkMXF(void);

    HRESULT ConnectOutput(CMXF *sinkMXF);
    HRESULT DisconnectOutput(CMXF *sinkMXF);

    HRESULT PutMessage(PDMUS_KERNEL_EVENT pDMKEvt);

private:
    FILE *m_fout;
    PDMUS_KERNEL_EVENT m_expectedEvent;

};

CSinkMXF::CSinkMXF(CAllocatorMXF *allocatorMXF) 
    : CMXF(allocatorMXF)
{
    m_fout = fopen("c:\\ksmtest.log", "w");
    m_expectedEvent = NULL;
}

CSinkMXF::~CSinkMXF()
{
    if (m_expectedEvent)
    {
        fprintf(m_fout, "\nNever received last expected event\n");
    }

    fclose(m_fout);
}

HRESULT CSinkMXF::ConnectOutput(CMXF *)
{
    return E_FAIL;
}

HRESULT CSinkMXF::DisconnectOutput(CMXF *)
{
    return E_FAIL;
}

HRESULT CSinkMXF::PutMessage(PDMUS_KERNEL_EVENT pDMKevt)
{
    fprintf(m_fout, 
            "Sink: Event at 0x%08X%08X ",
            (DWORD)((pDMKevt->ullPresTime100Ns >> 32) & 0xFFFFFFFF),
            (DWORD)(pDMKevt->ullPresTime100Ns & 0xFFFFFFFF));

    BOOL fNextPiece = (BOOL)(pDMKevt == m_expectedEvent);
    if (fNextPiece)
    {
        m_expectedEvent = NULL;
    }
    fprintf(m_fout, "\n");

    fprintf(m_fout,"      Length %5u Channel Group %5u %c%c%c\n",
            (unsigned)pDMKevt->cbEvent, 
            pDMKevt->ulChannelGroup,
            fNextPiece ? 'N' : 'n',
            (INCOMPLETE_EVT(pDMKevt)) ? 'C' : 'c',
            pDMKevt->pNextEvt ? 'P' : 'p');
    fprintf(m_fout, "\n");

    if (INCOMPLETE_EVT(pDMKevt))
    {
        if (m_expectedEvent)
        {
            fprintf(m_fout, "NOTE: Received new expected event without completion of the last\n");
        }

        m_expectedEvent = pDMKevt->pNextEvt;
        if (m_expectedEvent == NULL)
        {
            fprintf(m_fout, "NOTE: Continued bit set but next pointer NULL\n");
        }
    }

    LPBYTE pbData = pDMKevt->cbEvent > sizeof(PBYTE) ? pDMKevt->uData.pbData : &pDMKevt->uData.abData[0];

    HexDump(m_fout, pbData, pDMKevt->cbEvent);

    return S_OK;
}

class CWyatt : public CIrp
{
public:
    CWyatt(ULONG ulBufferSize, void (*)(CWyatt*), FILE *);
    ~CWyatt();
    void Notify(void);
    void Complete(HRESULT hr);

private:
    void (*m_AppComplete)(CWyatt *);
    FILE *m_fout;
};

CWyatt::CWyatt(ULONG ulBufferSize, void (*AppComplete)(CWyatt*), FILE *fout)
{
    m_pbBuffer          = new BYTE[ulBufferSize];
    m_cbBuffer          = ulBufferSize;
    m_cbLeft            = ulBufferSize;
    m_ullPresTime100Ns  = 0;
    m_pNext             = NULL;
    m_AppComplete       = AppComplete;
    m_fout              = fout;
}

CWyatt::~CWyatt()
{
    if (m_pbBuffer)
    {
        delete[] m_pbBuffer;
    }
}

void CWyatt::Notify()
{
    fprintf(m_fout, "IRP %08X: Notify. cbBuffer %u; used %d\n", 
        (DWORD)this,
        m_cbBuffer,
        m_cbBuffer - m_cbLeft);
}

void CWyatt::Complete(HRESULT hr)
{
    fprintf(m_fout, "IRP %08p: Completed with hr %08x\n", this, hr);
    (*m_AppComplete)(this);
}

void TestUnpacker()
{
    CAllocatorMXF *allocatorMXF = new CAllocatorMXF;
    CKsUnpackerMXF *unpackerMXF = new CKsUnpackerMXF(allocatorMXF);
    CSinkMXF *sinkMXF = new CSinkMXF(allocatorMXF);

    HRESULT hr = unpackerMXF->ConnectOutput(sinkMXF);
   
    if (SUCCEEDED(hr))
    {
        unpackerMXF->SinkIRP(abKSMusicData, sizeof(abKSMusicData), 0);
            
        // Build a SysEx IRP that's big enought for a forced split
        //
        SYSTEM_INFO si;
        GetSystemInfo(&si);

        ULONG cbData = 2 * si.dwPageSize + 5;
        cbData = (cbData + 3) & ~3;

        PKSMUSICFORMAT pksmf = (PKSMUSICFORMAT)new BYTE[sizeof(KSMUSICFORMAT) + cbData];

        pksmf->TimeDeltaMs  = 0;
        pksmf->ByteCount    = cbData;

        LPBYTE pbData = (LPBYTE)(pksmf + 1);
        pbData[0] = 0xF0;
        pbData[1] = 0x00;
        pbData[2] = 0x00;
        pbData[3] = 0x41;
        pbData[cbData - 1] = 0xF7;

        BYTE bData = 0;
        for (ULONG idx = 4; idx < cbData - 1; idx++)
        {
            pbData[idx] = bData;
            bData = (BYTE)((bData + 1) & 0x7F);
        }

        unpackerMXF->SinkIRP((LPBYTE)pksmf, sizeof(KSMUSICFORMAT) + cbData, 0);

        delete[] ((LPBYTE)pksmf);

        unpackerMXF->DisconnectOutput(sinkMXF);
    }

    delete sinkMXF;
    delete unpackerMXF;
    delete allocatorMXF;
}

CIrp *pIrpCompleteHead;
CIrp *pIrpCompleteTail;

void TestPackerIrpComplete(CWyatt *pirp)
{
    if (pIrpCompleteHead)
    {
        pIrpCompleteTail->m_pNext = pirp;
        pIrpCompleteTail = pirp;
    }
    else
    {
        pIrpCompleteHead = pirp;
        pIrpCompleteTail = pirp;
    }

    pirp->m_pNext = NULL;
}

void TestPacker()
{
    FILE *fout = fopen("c:\\ksmtest.log", "w");

    CAllocatorMXF *allocatorMXF = new CAllocatorMXF;
    CKsUnpackerMXF *unpackerMXF = new CKsUnpackerMXF(allocatorMXF);
    CPackerMXF *packerMXF = new CDMusPackerMXF(allocatorMXF);

    HRESULT hr = unpackerMXF->ConnectOutput(packerMXF);

    for (int cirp = 0; cirp < 32; cirp++)
    {
        CWyatt *pirp = new CWyatt(64, TestPackerIrpComplete, fout);
        packerMXF->SubmitIrp(pirp);
    }

    pIrpCompleteHead = NULL;
    pIrpCompleteTail = NULL;
   
    if (SUCCEEDED(hr))
    {
        unpackerMXF->SinkIRP(abKSMusicData, sizeof(abKSMusicData), 0);
            
        // Build a SysEx IRP that's big enought for a forced split
        //
        SYSTEM_INFO si;
        GetSystemInfo(&si);

        ULONG cbData = 2 * si.dwPageSize + 5;
        cbData = (cbData + 3) & ~3;

        PKSMUSICFORMAT pksmf = (PKSMUSICFORMAT)new BYTE[sizeof(KSMUSICFORMAT) + cbData];

        pksmf->TimeDeltaMs  = 0;
        pksmf->ByteCount    = cbData;

        LPBYTE pbData = (LPBYTE)(pksmf + 1);
        pbData[0] = 0xF0;
        pbData[1] = 0x00;
        pbData[2] = 0x00;
        pbData[3] = 0x41;
        pbData[cbData - 1] = 0xF7;

        BYTE bData = 0;
        for (ULONG idx = 4; idx < cbData - 1; idx++)
        {
            pbData[idx] = bData;
            bData = (BYTE)((bData + 1) & 0x7F);
        }

        unpackerMXF->SinkIRP((LPBYTE)pksmf, sizeof(KSMUSICFORMAT) + cbData, 0);

        delete[] ((LPBYTE)pksmf);

        unpackerMXF->DisconnectOutput(packerMXF);
    }

    while (pIrpCompleteHead)
    {
        CIrp *pirp = pIrpCompleteHead;
        pIrpCompleteHead = pirp->m_pNext;

        fprintf(fout, "IRP: %08p  ", pirp);
        fprintf(fout, "%I64X\n",  (__int64)pirp->m_ullPresTime100Ns);
        HexDump(fout, pirp->m_pbBuffer, pirp->m_cbBuffer - pirp->m_cbLeft);
    }   

    delete packerMXF;
    delete unpackerMXF;
    delete allocatorMXF;

    fclose(fout);
}

void HexDump(FILE *fout, LPBYTE pbData, ULONG cbData)
{
    for (ULONG idxLine = 0; idxLine < cbData; idxLine += 16)
    {
        ULONG cbThisLine = cbData - idxLine;
        if (cbThisLine > 16) 
            cbThisLine = 16;

        fprintf(fout, "       ");
        for (ULONG idxByte = 0; idxByte < cbThisLine; idxByte++)
            fprintf(fout, "%02X ", *pbData++);
        fprintf(fout, "\n");
    }
    fprintf(fout, "\n");
}
