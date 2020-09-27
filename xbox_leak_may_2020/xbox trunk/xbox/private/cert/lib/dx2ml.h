/*
 *
 * dx2ml.h
 *
 * CDX2ML
 *
 */

#ifndef _DX2ML_H
#define _DX2ML_H

#include <filestm.h>

#define MAX_PLACEHOLDER_PER_LAYER 8

struct DXI
{
    LPSTR szRefName;
    LPSTR szNameOnDisk;
    LPSTR szNameOnTape;
    DWORD dwImageSize;
    union {
        struct {
            DWORD dwTapeId:1,
                fTapeFile:1,
                fUsedLyr0:1,
                fUsedLyr1:1,
                f2054:1,
                unused:27;
        };
        DWORD dwFlags;
    };

    bool operator==(const DXI &) const;
    bool operator!=(const DXI &dxi) const { return !(*this == dxi); }
};

struct DXR
{
    DWORD psnStart;
    DWORD cpsnSize;
    int idxiImage;
};

struct XBI
{
    DWORD lsnStart;
    DWORD cpsnSize;
	DWORD dwDummy;
};

// WARNING: the XBI and XPH have to be the same size

struct XPH
{
    DWORD lsnStart;
    enum {
        phNone,
        phMirror,
        phGroove,
        phVelocity,
        phMax
    };
    int ph;
    BOOL fDisallow;
};

struct DXL
{
    union {
        struct {
            DXR dxrLead;
            DXR dxrData;
            DXR dxrMiddle;
            DXR dxrXLead;
            DXR dxrXData;
            DXR dxrXMiddle;
        };
        DXR rgdxrAreas[6];
    };
    DWORD psnStart;
    DWORD ipsnControlStart;
    XPH rgxph[MAX_PLACEHOLDER_PER_LAYER];
    int cxph;
    int ixbiMac;
    int ixbiMax;
    XBI *pxbi;

    bool operator==(const DXL &) const;
    bool operator!=(const DXL &dxl) const { return !(*this == dxl); }
};

enum { idxrLead, idxrData, idxrMiddle, idxrXLead, idxrXData, idxrXMiddle,
    idxrMax };

enum { idxiXControl, idxiControl, idxiMax=6 };

struct DXML
{
    union {
        struct {
            DXI dxiXControl;
            DXI dxiControl;
        };
        DXI rgdxi[idxiMax];
    };
    DXL rgdxl[2];
    BOOL fLayoutTape;
    LPSTR szMasteringRef;
    DWORD iLayer;

    bool operator==(const DXML &) const;
    bool operator!=(const DXML &dxml) const { return !(*this == dxml); }
};

class CDX2MLFile
{
public:
#ifdef DVDEMU
    CDX2MLFile(void)
    {
        memset(&m_dxml, 0, sizeof m_dxml);
        m_pxml = &m_dxml;
    }
#else
    CDX2MLFile(void) : m_pxml(NULL), m_pxdoc(NULL) {}
#endif
    CDX2MLFile(const CDX2MLFile &xml);
    ~CDX2MLFile();

    BOOL FReadFile(LPCSTR sz, BOOL fRequireFull);
    BOOL FWriteFile(LPCSTR sz);
    BOOL FWriteFile(CFileStmSeq *pstm);

    BOOL FValidateForm(void) const;
    BOOL FValidateContent(void) const;
    BOOL FValidatePlaceholderLocations(BOOL fFixup, BOOL fAPOnly,
        BOOL fPrintResults);

    DXML *PdxmlGet(void) { return m_pxml; }
    void SetMasteringRef(LPCSTR sz);

    bool operator==(const CDX2MLFile &) const;
    bool operator!=(const CDX2MLFile &xml) const { return !(*this == xml); }

private:
    DXML *m_pxml;
#ifdef DVDEMU
    DXML m_dxml;
#else
    CComPtr<IXMLDOMDocument> m_pxdoc;
    void ReadInputStreams(int ilyr, IXMLDOMNode *pxnIn);
    void ReadDiscRegions(int ilyr, IXMLDOMNode *pxn);
#endif

    void WriteXData(CFileStmSeq *pstm, int ilyr);
    void WriteLeadIn(CFileStmSeq *pstm);
    void WriteXLeadOut(CFileStmSeq *pstm);
};

#endif // _DX2ML_H
