//////////////////////////////////////////////////////////////////////
//
//	Info.h
//
//////////////////////////////////////////////////////////////////////

#ifndef _INC_INFO
#define _INC_INFO

#include "riffstrm.h"
#include "UnknownChunk.h"

class CInfo
{
public:
	friend class CCollectionPropPg;
    friend class CInstrumentPropPg;
    friend class CWaveInfoPropPg;
	friend class CInstrument;
	friend class CStereoWave;
	friend class CMonoWave;

	void Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain);
    DWORD Save(IDMUSProdRIFFStream* pIRiffStream);

	bool Load(IStream* pIStream);
	bool Save(IStream* pIStream);
public:
	CString m_csName;

private:	
	void Alloc_and_Read(CString& csDest, IStream* pIStream, MMCKINFO* pck);
    CString m_csCopyright;
    CString m_csSoftware; 
    CString m_csSubject;
    CString m_csDate;
    CString m_csEngineer;
    CString m_csProduct;
    CString m_csComment;

    CUnknownChunk   m_UnknownChunk;
};

#endif // _INC_INFO