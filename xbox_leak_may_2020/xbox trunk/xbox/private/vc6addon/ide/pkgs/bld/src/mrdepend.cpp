//
// Implement minimal rebuild dependencies interface.
//
// [tomse]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "mrdepend.h"	// our local header file

#include <winver.h>
#include <mrengine.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

class EnumDepContext {
public:
	EnumDepContext() : 
		pstrList(NULL), 
		bRemovePchIncludes(FALSE), 
		bSourceFound(FALSE)
	{
	}
	CStringList* pstrList;
	BOOL bRemovePchIncludes;
	BOOL bSourceFound;
};

BOOL MRECALL
FEnumDepFile( PMREUtil pmre, EnumFile & ef, EnumType et )
{
	ASSERT(etDep==et);
	ASSERT(NULL!=ef.pvContext);
	EnumDepContext& context = *(EnumDepContext*)ef.pvContext;
	ASSERT(NULL!=context.pstrList);

	//
	// Don't add files that are included by precompiled header.
	//
	if (context.bRemovePchIncludes && ef.fiSrc.dwStatus & fsmInclByPch)
		return TRUE;

	//
	// Check for the slight possibility that ef.szFileSrc was not
	// updated.
	if (NULL!=ef.szFileSrc && ef.szFileSrc[0] != 0 )
		context.pstrList->AddTail(ef.szFileSrc);

	return TRUE;
}

BOOL MRECALL
FEnumSrcFile( PMREUtil pmre, EnumFile & ef, EnumType et )
{
	ASSERT(NULL!=ef.pvContext);
	EnumDepContext& context = *(EnumDepContext*)ef.pvContext;
	context.bSourceFound = TRUE;

	ASSERT(etSource==et);
	pmre->EnumDepFiles ( ef, FEnumDepFile );
	return TRUE;
}

typedef BOOL (MRECALL *FOpen_fnptr) (OUT PMREngine*	ppmre, SZC szPdb, EC& ec, 
	_TCHAR szErr[], BOOL fReproSig, BOOL fWrite );

class CMreDependencies : public IMreDependencies {
public:
	friend IMreDependencies* IMreDependencies::GetMreDependenciesIFace( LPCTSTR szIdbFile );
	virtual BOOL GetDependencies( LPCTSTR szPath, CStringList& strList, BOOL bUsePch );
	virtual BOOL Release();

private:
	CMreDependencies() : m_pMre(NULL), m_pMreUtil(NULL), m_cRef(0) {}
	unsigned m_cRef;
	PMREngine m_pMre;
	PMREUtil m_pMreUtil;
	static LPCTSTR szMspdbDll;
	static HINSTANCE s_hInstPdbDll;
	static FOpen_fnptr s_FOpen;

#if defined(_DEBUG)
public:
#endif
	// public for debug builds.
	static CMapStringToPtr m_OpenMreMap;
};

#if defined(_DEBUG)
// Make sure that CMreDependencies::m_OpenMreMap is empty when program is finished.
class CleanupMreDependencies {
public:
	~CleanupMreDependencies()
	{
		ASSERT(CMreDependencies::m_OpenMreMap.IsEmpty());
	}
};

CleanupMreDependencies tmpCleanup;
#endif

CMapStringToPtr CMreDependencies::m_OpenMreMap;
LPCTSTR CMreDependencies::szMspdbDll = _T("mspdb60.dll");
HINSTANCE CMreDependencies::s_hInstPdbDll = NULL;
FOpen_fnptr CMreDependencies::s_FOpen = NULL;

IMreDependencies* IMreDependencies::GetMreDependenciesIFace( LPCTSTR szIdbFile )
{
	CMreDependencies* pMreDepend = NULL;
	CString strKey = szIdbFile;
	strKey.MakeUpper();
	// Check map of open Mre files first.
	if (CMreDependencies::m_OpenMreMap.Lookup(strKey, (void*&)pMreDepend))
	{
		ASSERT(NULL!=pMreDepend);
		ASSERT(NULL!=pMreDepend->m_pMre);
		ASSERT(NULL!=pMreDepend->m_pMreUtil);
		ASSERT(0<pMreDepend->m_cRef);
		pMreDepend->m_cRef++;
	}
	else
	{
		pMreDepend = new CMreDependencies;
		PMREngine pMre;
		PMREUtil pMreUtil;
		TCHAR szErr[cbErrMax];
		EC ec;
		szErr[0] = 0;

		if (NULL!=pMreDepend)
		{
			if (CMreDependencies::m_OpenMreMap.IsEmpty())
			{
				ASSERT(NULL==CMreDependencies::s_hInstPdbDll);
				CMreDependencies::s_hInstPdbDll = LoadLibrary(CMreDependencies::szMspdbDll);
				ASSERT(NULL!=CMreDependencies::s_hInstPdbDll);

				if (NULL==CMreDependencies::s_hInstPdbDll)
					return NULL;

				ASSERT(NULL==CMreDependencies::s_FOpen);
				CMreDependencies::s_FOpen = (FOpen_fnptr)GetProcAddress(
					CMreDependencies::s_hInstPdbDll,
#if defined (_M_ALPHA)
					"?FOpen@MREngine@@SAHPAPAU1@PBDAAJQADHH@Z");
#else
					"?FOpen@MREngine@@SGHPAPAU1@PBDAAJQADHH@Z");
#endif

				ASSERT(NULL!=CMreDependencies::s_FOpen);

				if (NULL==CMreDependencies::s_FOpen)
				{
					VERIFY(FreeLibrary(CMreDependencies::s_hInstPdbDll));
					CMreDependencies::s_hInstPdbDll = NULL;
					return NULL;
				}
			}

			ASSERT(NULL!=CMreDependencies::s_hInstPdbDll);
			ASSERT(NULL!=CMreDependencies::s_FOpen);

            // n.b. Don't try to open .idb file unless it already exists
			if  ((_access(szIdbFile, 0) != -1) && ((*CMreDependencies::s_FOpen)( &pMre, szIdbFile, ec, szErr, FALSE, TRUE ) && pMre ))
			{
				pMre->QueryMreUtil(pMreUtil);
				ASSERT(NULL!=pMreUtil);
				pMreDepend->m_cRef = 1;
				pMreDepend->m_pMre = pMre;
				pMreDepend->m_pMreUtil = pMreUtil;
				CMreDependencies::m_OpenMreMap[strKey] = pMreDepend;
			}
			else
			{
				if (CMreDependencies::m_OpenMreMap.IsEmpty())
				{
					VERIFY(FreeLibrary(CMreDependencies::s_hInstPdbDll));
					CMreDependencies::s_hInstPdbDll = NULL;
					CMreDependencies::s_FOpen = NULL;
				}

				delete pMreDepend;
				pMreDepend = NULL;
			}
		}
	}

	return pMreDepend;
}

BOOL CMreDependencies::GetDependencies( LPCTSTR szPath, CStringList& strList, BOOL bUsePch )
{
	EnumDepContext context;
	context.bRemovePchIncludes = bUsePch;

	ASSERT(NULL!=m_pMre);
	ASSERT(NULL!=m_pMreUtil);
	strList.RemoveAll();

	context.pstrList = &strList;
	m_pMreUtil->EnumSrcFiles ( FEnumSrcFile, szPath, &context );

	return context.bSourceFound;
}

BOOL CMreDependencies::Release()
{
	BOOL bOk = 0 < m_cRef;

	ASSERT(NULL!=m_pMre);
	ASSERT(NULL!=m_pMreUtil);

	m_cRef--;
	if (0==m_cRef)
	{
		bOk = NULL!=m_pMreUtil && m_pMreUtil->FRelease() && bOk;
		bOk = NULL!=m_pMre && m_pMre->FClose (FALSE) && bOk;

		// Remove from map
		POSITION pos = m_OpenMreMap.GetStartPosition();
		ASSERT(pos!=NULL);
		while (NULL!=pos)
		{
			CString strFilename;
			CMreDependencies* pMreDepend;
			m_OpenMreMap.GetNextAssoc(pos,strFilename,(void*&)pMreDepend);
			ASSERT(NULL!=pMreDepend);
			if (this==pMreDepend)
			{
				VERIFY(m_OpenMreMap.RemoveKey(strFilename));
				if (m_OpenMreMap.IsEmpty())
				{
					ASSERT(NULL!=s_hInstPdbDll);
					if (NULL!=s_hInstPdbDll)
						VERIFY(FreeLibrary(s_hInstPdbDll));

					s_hInstPdbDll = NULL;
					s_FOpen = NULL;
				}

				break;
			}
		}
		delete this;
	}

	return bOk;
}
