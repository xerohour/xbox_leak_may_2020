/*
 *
 * emutape.cpp
 *
 * Emulate from tape files
 *
 */

#include "emutape.h"

CBasedFile *pstmXL0, *pstmXL1;
CBasedFile *pstmL0, *pstmL1;
PHYSICAL_GEOMETRY geomECMA, geomX2;
CDX2MLFile g_xml;
DXML *pdxml;
CXControl xct;
CECMAControl ect;
BYTE rgbControl[2048];
BYTE rgbXControl[2048];
DWORD dwDAC;
HANDLE hevtDone;
BOOL fX2Session;

CBasedFile::CBasedFile(CTapeDevice2 *ptap) : CDiskFile((HANDLE)NULL)
{
    LARGE_INTEGER li;

    if(ptap->m_ptod) {
        li.QuadPart = 0;
        if(SetFilePointerEx(ptap->m_hdev, li, &m_liBase, FILE_CURRENT))
            m_hFile = ptap->m_hdev;
    }
}

CBasedFile::CBasedFile(CDiskFile *pstm) : CDiskFile(*pstm)
{
    m_liBase.QuadPart = 0;
}

BOOL CBasedFile::FSeek(LONG l)
{
    LARGE_INTEGER li;

    li.QuadPart = l + m_liBase.QuadPart;
    return CDiskFile::FSeek(li);
}

BOOL CBasedFile::FSeek(LARGE_INTEGER li)
{
    li.QuadPart += m_liBase.QuadPart;
    return CDiskFile::FSeek(li);
}

CDiskFile *PstmCreateTemp(LPCSTR szName, DWORD dwFlags)
{
    CDiskFile *pstm = new CDiskFile(szName, dwFlags);
    if(!pstm->FIsOpen()) {
        delete pstm;
        pstm = NULL;
    } else
        pstm->SetDeleteOnClose(TRUE);
    return pstm;
}

BOOL FSeekToFile(CTapeDevice *ptap, LPCSTR szFileName, LPDWORD lpdwDataBlkSize,
	LPDWORD lpdwDataPerPhy)
{
	char szName[64];

	while(ptap->FReadFile(szName, lpdwDataBlkSize, lpdwDataPerPhy)) {
		if(0 == strcmp(szName, szFileName))
			return TRUE;
	}

	return FALSE;
}

CBasedFile *PstmBasedTapeFile(CTapeDevice2 *ptap, LPCSTR szName)
{
	if(!FSeekToFile(ptap, szName, NULL, NULL))
		return FALSE;
	return new CBasedFile(ptap);
}

BOOL FValidStm(CFileStmSeq *pstm)
{
	return pstm && pstm->FIsOpen();
}

BOOL FGetXMLData(CTapeDevice *ptap)
{
    CDiskFile *pstmXdr;
    CDiskFile *pstmXml = NULL;
    DWORD cbRes;
    HRSRC hres;
    PBYTE pb;
	BOOL fRet = FALSE;

    /* Get the XDR file handy in case we need it */
    pstmXdr = PstmCreateTemp("dx2ml.xdr", GENERIC_WRITE);
    if(!pstmXdr) {
noxdr:
        fprintf(stderr, "error: unable to create temporary XDR file\n");
        goto fatal;
    }
    hres = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(1), "FILE");
    if(!hres)
        goto noxdr;
    pb = (PBYTE)LockResource(LoadResource(GetModuleHandle(NULL), hres));
    if(!pb)
        goto noxdr;
    cbRes = SizeofResource(GetModuleHandle(NULL), hres);
    if(pstmXdr->CbWrite(cbRes, pb) != cbRes)
        goto noxdr;
    if(!pstmXdr->FReopen(GENERIC_READ))
        goto noxdr;

	/* Read the DX2ML data */
	pstmXml = PstmCreateTemp("dx2ml.xml", GENERIC_WRITE);
	if(!ptap->FCopyToStm(pstmXml)) {
badxml:
		fprintf(stderr, "error: processing DX2ML data\n");
		goto fatal;
	}
	if(!pstmXml->FReopen(GENERIC_READ))
		goto badxml;
    if(!g_xml.FReadFile(pstmXml->SzName(), FALSE))
        goto badxml;
	pdxml = g_xml.PdxmlGet();
	if(!pdxml)
		goto badxml;

	fRet = TRUE;

fatal:
	delete pstmXdr;
	if(pstmXml)
		delete pstmXml;
	return fRet;
}

BOOL FGetXctrl(CTapeDevice *ptap, ULONG dwBlkSize)
{
	BYTE rgb[2054];
	LPBYTE pb;
	int i;

	if(dwBlkSize == 2048)
		pb = rgbXControl;
	else if(dwBlkSize == 2054)
		pb = rgb;
	else
		return FALSE;

	/* Read the data from the tape */
	for(i = 0; i < 31; ++i) {
		if(ptap->CbRead(dwBlkSize, pb) != dwBlkSize)
			return FALSE;
	}

	/* Process the XControl data */
	if(!xct.FInit(pb, dwBlkSize))
		return FALSE;

	/* Retain the XControl data for the read structure */
	if(pb != rgbXControl)
		memcpy(rgbXControl, rgb + 6, 2048);

	return TRUE;
}

void FillGeomPsns(PHYSICAL_GEOMETRY *pgeom, DWORD dwStart0, DWORD dwEnd0,
	DWORD dwEnd1)
{
	pgeom->Layer0StartPSN = dwStart0;
	pgeom->Layer0SectorCount = dwEnd0 - dwStart0 + 1;
	pgeom->Layer1SectorCount = dwEnd1 - (dwEnd0 ^ 0xFFFFFF) + 1;
}

void SetupGeometry(BOOL fMultiSession)
{
	DWORD dwStart0;
	DWORD dwEnd0;
	DWORD dwEnd1;

	xct.GetPsns(&dwStart0, &dwEnd0, &dwEnd1);
	FillGeomPsns(&geomX2, dwStart0, dwEnd0, dwEnd1);
	geomX2.IsX2Disc = TRUE;
	if(fMultiSession) {
		geomX2.SupportsMultiplePartitions = TRUE;
		ect.GetPsns(&dwStart0, &dwEnd0, &dwEnd1);
		FillGeomPsns(&geomECMA, dwStart0, dwEnd0, dwEnd1);
		geomECMA.IsX2Disc = TRUE;
		geomECMA.SupportsMultiplePartitions = TRUE;
	} else {
		geomX2.SupportsMultiplePartitions = FALSE;
		geomECMA = geomX2;
	}
}

void CTapeEmulation::MountUnmount(BOOL fMount)
{
	fX2Session = !geomX2.SupportsMultiplePartitions;
	if(!fMount)
		SetEvent(hevtDone);
}

void CTapeEmulation::GetPhysicalGeometry(PHYSICAL_GEOMETRY *pgeom, BOOL fX2)
{
	memcpy(pgeom, fX2 ? &geomX2 : &geomECMA, sizeof *pgeom);
	if(fX2)
		fX2Session = TRUE;
}

int CTapeEmulation::NumberOfPlaceholders(int Layer)
{
	return pdxml->rgdxl[Layer].cxph;
}

void CTapeEmulation::GetPlaceholderInformation(int Layer,
	PLACEHOLDER *Placeholders)
{
}

ULONG CTapeEmulation::GetSectorData(ULONG lsn, ULONG cblk, PUCHAR rgbData)
{
	PHYSICAL_GEOMETRY *pgeomUse;
	CFileStmRand *pstm;
	int err;
	ULONG cblk0;
	BOOL fSuccess;
	LARGE_INTEGER li;

	pgeomUse = fX2Session ? &geomX2 : &geomECMA;
	if(lsn >= pgeomUse->Layer0SectorCount) {
		lsn -= pgeomUse->Layer0SectorCount;
		if(lsn >= pgeomUse->Layer1SectorCount || lsn + cblk >
				pgeomUse->Layer1SectorCount)
			return EMU_ERR_INVALID_LSN;
		pstm = fX2Session ? pstmXL1 : pstmL1;
	} else {
		if(lsn + cblk > pgeomUse->Layer0SectorCount) {
			/* This read spans the layer boundary.  Read the layer 1 data
			 * first. */
			cblk0 = pgeomUse->Layer0SectorCount - lsn;
			err = GetSectorData(lsn + cblk0, cblk - cblk0, rgbData + 2048 * cblk0);
			if(err)
				return err;
			cblk = cblk0;
		}
		pstm = fX2Session ? pstmXL0 : pstmL0;
	}

	li.QuadPart = lsn * 2048;
	fSuccess = pstm->FSeek(li) && pstm->CbRead(cblk * 2048, rgbData) ==
		cblk * 2048;
	err = fSuccess ? EMU_ERR_SUCCESS : EMU_ERR_UNEXPECTED_ERROR;

	return err;
}

BOOL CTapeEmulationDVDX2::GetControlData(BOOL fXControl, PUCHAR pb, ULONG cb,
	PULARGE_INTEGER Delay)
{
	PUCHAR pbSrc;
	ULONG cbMax;

	if(fXControl) {
		pbSrc = rgbXControl;
		cbMax = 1631;
	} else {
		pbSrc = rgbControl;
		cbMax = 2048;
	}

	if(!pbSrc[0])
		return FALSE;

	if(cb > cbMax) {
		memcpy(pb, pbSrc, cbMax);
		memset(pb + cbMax, 0, cb - cbMax);
	} else
		memcpy(pb, pbSrc, cb);
	return TRUE;
}

BOOL CTapeEmulationDVDX2::DiscChallengeResponse(UCHAR id,
	PULONG ChallengeValue, PULONG ResponseValue, PULARGE_INTEGER Delay)
{
	return FALSE;
}

int __cdecl main(int argc, char **argv)
{
    CTapeDevice2 tapSrc0, tapSrc1, *ptap;
	ULONG dwBlkSize;

	/* Mount the tape files.  If one tape file is present, we assume we're
	 * emulating a black tape.  If two are present, we're doing either
	 * red or green */

	if(argc < 2) {
		fprintf(stderr, "usage: emutape layer0.dat [layer1.dat]\n");
		return 1;
	}

	if(!tapSrc0.FMountAsTapeOrDisk(argv[1], FALSE)) {
notape0:
		fprintf(stderr, "error: reading %s\n", argv[1]);
		return 1;
	}

	if(argc == 2)
		ptap = &tapSrc0;
	else {
		if(!tapSrc1.FMountAsTapeOrDisk(argv[2], FALSE)) {
notape1:
			fprintf(stderr, "error: reading %s\n", argv[2]);
			return 1;
		}
		ptap = &tapSrc1;
	}

	/* Get the XML data */
	if(!FSeekToFile(&tapSrc0, "DX2ML.XML", NULL, NULL))
		goto notape0;
	if(!FGetXMLData(&tapSrc0))
		return 1;

	/* Load the control and xcontrol data */
	if(ptap != &tapSrc0) {
		if(!FSeekToFile(&tapSrc0, "CONTROL.DAT", NULL, NULL)) {
			fprintf(stderr, "error: could not find control data\n");
			return 1;
		}
		if(tapSrc0.CbRead(sizeof rgbControl, rgbControl) != sizeof rgbControl) {
badctrl:
			fprintf(stderr, "error: processing control data\n");
			return 1;
		}
		if(!ect.FInit(rgbControl))
			goto badctrl;
	}
	if(!FSeekToFile(ptap, "XCONTROL.DAT", &dwBlkSize, NULL)) {
		fprintf(stderr, "error: could not find xcontrol data\n");
		return 1;
	}
	if(!FGetXctrl(ptap, dwBlkSize))
		return 1;

	SetupGeometry(ptap != &tapSrc0);

	/* Get the layer files */
	if(ptap != &tapSrc0) {
		pstmL0 = PstmBasedTapeFile(&tapSrc0, "IMAGE0.DAT");
		pstmXL0 = PstmBasedTapeFile(&tapSrc0, "XIMAGE0.DAT");
		pstmXL1 = PstmBasedTapeFile(&tapSrc1, "XIMAGE1.DAT");
		pstmL1 = PstmBasedTapeFile(&tapSrc1, "IMAGE1.DAT");
		if(!FValidStm(pstmL0) || !FValidStm(pstmXL0))
			goto notape0;
		if(!FValidStm(pstmL1) || !FValidStm(pstmXL1))
			goto notape1;
	} else {
		pstmXL0 = PstmBasedTapeFile(&tapSrc0, "XIMAGE0.DAT");
		pstmXL1 = PstmBasedTapeFile(&tapSrc1, "XIMAGE1.DAT");
		if(!FValidStm(pstmXL0) || !FValidStm(pstmXL1))
			goto notape0;
	}

	/* Now begin emulation */
	if(FAILED(StartEmulating())) {
		fprintf(stderr, "error: starting DVD emulation session\n");
		return 1;
	}
	hevtDone = CreateEvent(NULL, FALSE, FALSE, NULL);
	EmulationOpenTray(TRUE);
	EmulationCloseTray(ptap == &tapSrc0 ? new CTapeEmulation :
		new CTapeEmulationDVDX2, TRUE);
	WaitForSingleObject(hevtDone, INFINITE);
	EmulationCloseTray(NULL, TRUE);
	StopEmulating();

	return 0;
}
