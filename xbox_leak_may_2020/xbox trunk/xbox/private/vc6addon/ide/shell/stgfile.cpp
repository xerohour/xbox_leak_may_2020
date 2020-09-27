// stgfile.cpp : implementation of the COLEStorage class
//

#include "stdafx.h"
#include "malloc.h"
#include "objbase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

// From olestrm.cpp: (MFC internal)
void FillFileExceptionFromScode(CFileException* pError, SCODE sc);
void ThrowFileExceptionFromScode(SCODE sc);


/////////////////////////////////////////////////////////////////////////////
// COLEStorage
#define AS_IStorage(pi) ((IStorage*) pi)

COLEStorage::COLEStorage(BOOL bReadOnly /*= TRUE*/)
	: m_pStorage(NULL), m_grfMode(bReadOnly ? STGM_READ : STGM_READWRITE)
{
}

COLEStorage::~COLEStorage()
{
	if (m_pStorage != NULL)
		AS_IStorage(m_pStorage)->Release();
}

LPCTSTR COLEStorage::GetName()
{
	static CString strName;

	ASSERT(m_pStorage != NULL);
	STATSTG stgStat;
	HRESULT hrc = AS_IStorage(m_pStorage)->Stat(&stgStat, STATFLAG_DEFAULT);
	if (FAILED(hrc))
	{
		// These are bugs
		ASSERT(hrc != STG_E_INVALIDFLAG);
		ASSERT(hrc != STG_E_INVALIDPOINTER);

		return NULL;
	}

	// Get the name
	strName = stgStat.pwcsName;

	// Free the name
	CoTaskMemFree(stgStat.pwcsName);

	return strName;
}

void COLEStorage::Open(LPCTSTR lpszPath)
{
	USES_CONVERSION;

	IStorage* pStorage = AS_IStorage(m_pStorage);
	HRESULT hrc = StgOpenStorage(A2CW(lpszPath), NULL, STGM_DIRECT | STGM_SHARE_DENY_WRITE | m_grfMode, NULL, 0, &pStorage);
	if (FAILED(hrc))
	{
		// These return values are bugs
		ASSERT(hrc != STG_E_INVALIDPOINTER);
		ASSERT(hrc != STG_E_INVALIDFLAG);
		ASSERT(hrc != STG_E_INVALIDFUNCTION);

		// Throw an exception
		ThrowFileExceptionFromScode(GetScode(hrc));
	}

	m_pStorage = pStorage;
}

void COLEStorage::Create(LPCTSTR lpszPath)
{
	USES_CONVERSION;

	IStorage* pStorage = AS_IStorage(m_pStorage);
	HRESULT hrc = StgCreateDocfile(A2CW(lpszPath), STGM_CREATE | STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, &pStorage);
	if (FAILED(hrc))
	{
		// These return values are bugs
		ASSERT(hrc != STG_E_INVALIDPOINTER);
		ASSERT(hrc != STG_E_INVALIDFLAG);

		// Throw an exception
		ThrowFileExceptionFromScode(GetScode(hrc));
	}

	m_pStorage = pStorage;
}

void COLEStorage::OpenStream(LPCTSTR lpszName, COleStreamFile* pStream)
{
	ASSERT_VALID(pStream);
	ASSERT(m_pStorage != NULL);

	CFileException e;
	if (pStream->OpenStream(AS_IStorage(m_pStorage), lpszName, CFile::modeRead | CFile::shareExclusive, &e))
		return;

	AfxThrowFileException(e.m_cause, e.m_lOsError);
}

void COLEStorage::CreateStream(LPCTSTR lpszName, COleStreamFile* pStream)
{
	ASSERT_VALID(pStream);
	ASSERT(m_pStorage != NULL);

	CFileException e;
	if (pStream->CreateStream(AS_IStorage(m_pStorage), lpszName, CFile::modeReadWrite | CFile::shareExclusive | CFile::modeCreate, &e))
		return;

	AfxThrowFileException(e.m_cause, e.m_lOsError);
}

void COLEStorage::OpenStorage(LPCTSTR lpszName, COLEStorage* pStorage)
{
	USES_CONVERSION;

	ASSERT(pStorage != NULL);
	ASSERT(m_pStorage != NULL);

	IStorage* pIStorage = AS_IStorage(pStorage->m_pStorage);
	HRESULT hrc = AS_IStorage(m_pStorage)->OpenStorage(A2CW(lpszName), NULL, STGM_DIRECT | STGM_SHARE_EXCLUSIVE | m_grfMode, NULL, 0, &pIStorage);
	if (FAILED(hrc))
	{
		// These return values are bugs
		ASSERT(hrc != STG_E_INVALIDPOINTER);
		ASSERT(hrc != STG_E_INVALIDFLAG);
		ASSERT(hrc != STG_E_INVALIDFUNCTION);
		ASSERT(hrc != STG_E_INVALIDNAME);

		// Throw an exception
		ThrowFileExceptionFromScode(GetScode(hrc));
	}

	pStorage->m_pStorage = pIStorage;
}

void COLEStorage::CreateStorage(LPCTSTR lpszName, COLEStorage* pStorage)
{
	USES_CONVERSION;

	ASSERT(pStorage != NULL);
	ASSERT(m_pStorage != NULL);

	IStorage* pIStorage;
	HRESULT hrc = AS_IStorage(m_pStorage)->CreateStorage(A2CW(lpszName), STGM_DIRECT | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, 0, &pIStorage);
	if (FAILED(hrc))
	{
		// These return values are bugs
		ASSERT(hrc != STG_E_INVALIDPOINTER);
		ASSERT(hrc != STG_E_INVALIDFLAG);
		ASSERT(hrc != STG_E_INVALIDPARAMETER);
		ASSERT(hrc != STG_E_INVALIDNAME);

		// Throw an exception
		ThrowFileExceptionFromScode(GetScode(hrc));
	}

	pStorage->m_pStorage = pIStorage;
}


/////////////////////////////////////////////////////////////////////////////
// Helpers
void FillFileExceptionFromScode(CFileException* pError, SCODE sc)
{
	ASSERT(pError != NULL);
	ASSERT(FAILED(sc));

	int cause;  // portable CFileException.m_cause

	// error codes 255 or less are DOS/Win32 error codes
	if (SCODE_SEVERITY(sc) == SEVERITY_ERROR &&
		SCODE_FACILITY(sc) == FACILITY_STORAGE &&
		SCODE_CODE(sc) < 0x100)
	{
		ASSERT(SCODE_CODE(sc) != 0);

		// throw an exception matching to the DOS error
		//  (NOTE: only the DOS error part of the SCODE becomes m_lOsError)
		cause = CFileException::OsErrorToException(SCODE_CODE(sc));
		sc = (SCODE)SCODE_CODE(sc);
	}
	else
	{
		// attempt some conversion of storage specific error codes to generic
		//  CFileException causes...
		switch (sc)
		{
		case STG_E_INUSE:
		case STG_E_SHAREREQUIRED:
			cause = CFileException::sharingViolation;
			break;

		case STG_E_NOTCURRENT:
		case STG_E_REVERTED:
		case STG_E_CANTSAVE:
		case STG_E_OLDFORMAT:
		case STG_E_OLDDLL:
			cause = CFileException::generic;
			break;

		default:
			cause = CFileException::generic;
			break;
		}
	}

	// fill in pError
	pError->m_cause = cause;
	pError->m_lOsError = (LONG)sc;
}

void ThrowFileExceptionFromScode(SCODE sc)
{
	CFileException e;
	FillFileExceptionFromScode(&e, sc);

	AfxThrowFileException(e.m_cause, e.m_lOsError);
}
