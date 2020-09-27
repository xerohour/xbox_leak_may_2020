//
// Implement minimal rebuild dependencies interface.
//
// [tomse]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "mrdependencies.h"	// our local header file

#include <winver.h>

#ifdef UNICODE
#undef _TCHAR
#define  _TCHAR char
#endif
#include <mrengine.h>


#include <utf.h>
#include "scanner.h"

CTestableSection g_sectionMreMap;

class BldEnumDepContext
{
public:
	BldEnumDepContext() : m_pstrList(NULL), m_bRemovePchIncludes(FALSE), m_bSourceFound(FALSE) {}
	CVCBuildStringCollection* m_pstrList;
	BOOL m_bRemovePchIncludes;
	BOOL m_bSourceFound;
};

BOOL MRECALL FEnumDepFile(PMREUtil pmre, EnumFile& ef, EnumType et)
{
	VSASSERT(etDep == et, "Trying to determine dependencies with bad et value");
	VSASSERT(NULL != ef.pvContext, "No context to determine dependency info for");
	BldEnumDepContext& context = *(BldEnumDepContext*)ef.pvContext;
	VSASSERT(NULL != context.m_pstrList, "No list to return dependency info in");

	// Don't add files that are included by precompiled header.
	if (context.m_bRemovePchIncludes && (ef.fiSrc.dwStatus & fsmInclByPch))
		return TRUE;

	// Check for the slight possibility that ef.szFileSrc was not updated.
	if (NULL != ef.szFileSrc && ef.szFileSrc[0] != 0)
	{
		// Filter out system headers and write into list.
		CStringW str;
		PWSTR pStr = str.GetBuffer(_MAX_PATH);
		UTF8ToUnicode(ef.szFileSrc, NULL_TERMINATED_MODE, pStr, _MAX_PATH);
		str.ReleaseBuffer();
		if (str.IsEmpty())
			return TRUE;
		if (!g_SysInclReg.IsSysInclude(str, FALSE))
			context.m_pstrList->m_strStrings.Add(str);
	}

	return TRUE;
}

BOOL MRECALL FEnumDepFileForDeployment(PMREUtil pmre, EnumFile& ef, EnumType et)
{
	VSASSERT(etDep == et, "Trying to determine dependencies with bad et value");
	VSASSERT(NULL != ef.pvContext, "No context to determine dependency info for");
	BldEnumDepContext& context = *(BldEnumDepContext*)ef.pvContext;
	VSASSERT(NULL != context.m_pstrList, "No list to return dependency info in");

	if ((IncludeType)(ef.fiSrc.ftInclude) == itText)
		return TRUE;

	// Check for the slight possibility that ef.szFileSrc was not updated.
	if (NULL != ef.szFileSrc && ef.szFileSrc[0] != 0)
	{
		// Filter out system-provided stuff and write into list.
		CStringW str;
		PWSTR pStr = str.GetBuffer(_MAX_PATH);
		UTF8ToUnicode(ef.szFileSrc, NULL_TERMINATED_MODE, pStr, _MAX_PATH);
		str.ReleaseBuffer();
		if (str.IsEmpty())
			return TRUE;
		if (!g_SysInclReg.IsSysInclude(str, FALSE))
			context.m_pstrList->m_strStrings.Add(str);
	}

	return TRUE;
}

BOOL MRECALL FEnumSrcFile(PMREUtil pmre, EnumFile& ef, EnumType et)
{
	VSASSERT(NULL != ef.pvContext, "No context to determine dependency info for");
	BldEnumDepContext& context = *(BldEnumDepContext*)ef.pvContext;
	context.m_bSourceFound = TRUE;

	VSASSERT(etSource == et, "Invalid et value");
	pmre->EnumDepFiles(ef, FEnumDepFile);
	return TRUE;
}

BOOL MRECALL FEnumSrcFileForDeployment(PMREUtil pmre, EnumFile& ef, EnumType et)
{
	VSASSERT(NULL != ef.pvContext, "No context to determine dependency info for");
	BldEnumDepContext& context = *(BldEnumDepContext*)ef.pvContext;
	context.m_bSourceFound = TRUE;

	VSASSERT(etSource == et, "Invalid et value");
	pmre->EnumDepFiles(ef, FEnumDepFileForDeployment);
	return TRUE;
}

typedef BOOL (MRECALL *FOpen_fnptr)(OUT PMREngine *ppmre, SZC szPdb, EC& ec, char szErr[], BOOL fReproSig, BOOL fWrite);

class CBldMreDependencies : public IBldMreDependencies
{
public:
	friend IBldMreDependencies* IBldMreDependencies::GetMreDependenciesIFace(LPCOLESTR bstrIdbFile);
	friend void CBldMreDependenciesContainer::UnloadMSPDB();
	virtual BOOL GetDependencies(LPCOLESTR szPath, CVCBuildStringCollection* pCollection, BOOL bUsePch);
	virtual BOOL GetDeploymentDependencies(LPCOLESTR szPath, CVCBuildStringCollection* pCollection);
	virtual BOOL Release();

private:
	CBldMreDependencies() : m_pMre(NULL), m_pMreUtil(NULL), m_cRef(0) {}
	unsigned m_cRef;
	PMREngine m_pMre;
	PMREUtil m_pMreUtil;
	static LPCOLESTR m_szMspdbDll;
	static HINSTANCE s_hInstPdbDll;
	static FOpen_fnptr s_FOpen;

#if defined(_DEBUG)
public:
#endif
	// public for debug builds.
	static CVCMapStringWToPtr s_OpenMreMap;
};

#if defined(_DEBUG)
// Make sure that CBldMreDependencies::s_OpenMreMap is empty when program is finished.
class BldCleanupMreDependencies
{
public:
	~BldCleanupMreDependencies()
	{
		VSASSERT(CBldMreDependencies::s_OpenMreMap.IsEmpty(), "Failed to clean up MreDependencies before destruction");
	}
};

BldCleanupMreDependencies tmpCleanup;
#endif

CVCMapStringWToPtr CBldMreDependencies::s_OpenMreMap;
LPCOLESTR CBldMreDependencies::m_szMspdbDll = L"mspdb70.dll";
HINSTANCE CBldMreDependencies::s_hInstPdbDll = NULL;
FOpen_fnptr CBldMreDependencies::s_FOpen = NULL;

IBldMreDependencies* IBldMreDependencies::GetMreDependenciesIFace(LPCOLESTR bstrIdbFile)
{
	CBldMreDependencies* pMreDepend = NULL;
	CStringW strKey = bstrIdbFile;
	strKey.MakeLower();

	CritSectionT cs(g_sectionMreMap);
	// Check map of open Mre files first.
	if (CBldMreDependencies::s_OpenMreMap.Lookup(strKey, (void*&)pMreDepend))
	{
		VSASSERT(NULL!=pMreDepend, "Bad MreMap key/pointer pair");
		VSASSERT(NULL!=pMreDepend->m_pMre, "Bad PMREngine pointer in MreDepend");
		VSASSERT(NULL!=pMreDepend->m_pMreUtil, "Bad PMREUtil pointer in MreDepend");
		VSASSERT(0<pMreDepend->m_cRef, "Bad refcount on MreDepend");
		pMreDepend->m_cRef++;
	}
	else
	{
		pMreDepend = new CBldMreDependencies;
		PMREngine pMre;
		PMREUtil pMreUtil;
		EC ec;

		if (NULL != pMreDepend)
		{
			if (CBldMreDependencies::s_OpenMreMap.IsEmpty())
			{
				if (CBldMreDependencies::s_hInstPdbDll == NULL || CBldMreDependencies::s_FOpen == NULL)
				{
					if (CBldMreDependencies::s_hInstPdbDll == NULL)
					{
						CBldMreDependencies::s_hInstPdbDll = LoadLibraryW(CBldMreDependencies::m_szMspdbDll);
						VSASSERT(NULL != CBldMreDependencies::s_hInstPdbDll, "Failed to load PDB DLL");
						if (NULL == CBldMreDependencies::s_hInstPdbDll)
							return FALSE;
					}

					VSASSERT(NULL==CBldMreDependencies::s_FOpen, "MREngine already open!");
					CBldMreDependencies::s_FOpen = (FOpen_fnptr)GetProcAddress(
						CBldMreDependencies::s_hInstPdbDll,
#if defined (_M_ALPHA)
						"?FOpen@MREngine@@SAHPAPAU1@PBDAAJQADHH@Z");
#else
#if defined (_WIN64)
						"?FOpen@MREngine@@SAHPEAPEAU1@PEBDAEAJQEADHH@Z");
#else
						"?FOpen@MREngine@@SGHPAPAU1@PBDAAJQADHH@Z");
#endif	// _WIN64
#endif	// _M_ALPHA

					VSASSERT(NULL != CBldMreDependencies::s_FOpen, "Failed to open MREngine");

					if (NULL == CBldMreDependencies::s_FOpen)
					{
						BOOL bOK = FreeLibrary(CBldMreDependencies::s_hInstPdbDll);
						VSASSERT(bOK, "Failed to free CBldMreDependencies::s_hInstPdbDll library!");
						CBldMreDependencies::s_hInstPdbDll = NULL;
						return NULL;
					}
				}
			}

			VSASSERT(NULL != CBldMreDependencies::s_hInstPdbDll, "Failed to load PDB DLL");
			VSASSERT(NULL != CBldMreDependencies::s_FOpen, "Failed to open MREngine");

			// n.b. Don't try to open .idb file unless it already exists
			char szErr[cbErrMax];
			szErr[0] = 0;
			CStringA strIdbFileA = bstrIdbFile;	// yes, ANSI -- 'cause CBldMreDependencies needs it this way
			if  ((_access(strIdbFileA, 0) != -1) && 
				((*CBldMreDependencies::s_FOpen)(&pMre, strIdbFileA, ec, szErr, FALSE, FALSE) && pMre))
			{
				pMre->QueryMreUtil(pMreUtil);
				VSASSERT(NULL!=pMreUtil, "Bad PMREUtil pointer for .idb file");
				pMreDepend->m_cRef = 1;
				pMreDepend->m_pMre = pMre;
				pMreDepend->m_pMreUtil = pMreUtil;
				CBldMreDependencies::s_OpenMreMap[strKey] = pMreDepend;
			}
			else
			{
				delete pMreDepend;
				pMreDepend = NULL;
			}
		}
	}

	return pMreDepend;
}

void CBldMreDependenciesContainer::UnloadMSPDB()
{
	if (CBldMreDependencies::s_hInstPdbDll == NULL)
		return;		// nothing to do

	BOOL bOK = FreeLibrary(CBldMreDependencies::s_hInstPdbDll);
	VSASSERT(bOK, "Failed to free CBldMreDependencies::s_hInstPdbDll library!");
	CBldMreDependencies::s_hInstPdbDll = NULL;
	CBldMreDependencies::s_FOpen = NULL;
}

BOOL CBldMreDependencies::GetDependencies(LPCOLESTR szPath, CVCBuildStringCollection* pCollection, BOOL bUsePch)
{
	BldEnumDepContext context;
	context.m_bRemovePchIncludes = bUsePch;

	VSASSERT(NULL != m_pMre, "Invalid MreDependencies object.  Bad initialization.");
	VSASSERT(NULL != m_pMreUtil, "Invalid MreDependencies object.  Bad initialization.");

	pCollection->Clear();
	context.m_pstrList = pCollection;

	USES_CONVERSION;
	m_pMreUtil->EnumSrcFiles(FEnumSrcFile, W2A(szPath), &context);	// yes, ANSI

	return context.m_bSourceFound;
}

BOOL CBldMreDependencies::GetDeploymentDependencies(LPCOLESTR szPath, CVCBuildStringCollection* pCollection)
{
	BldEnumDepContext context;
	context.m_bRemovePchIncludes = FALSE;

	VSASSERT(NULL != m_pMre, "Invalid MreDependencies object.  Bad initialization.");
	VSASSERT(NULL != m_pMreUtil, "Invalid MreDependencies object.  Bad initialization.");

	pCollection->Clear();
	context.m_pstrList = pCollection;

	USES_CONVERSION;
	m_pMreUtil->EnumSrcFiles(FEnumSrcFileForDeployment, W2A(szPath), &context);  // yes, ANSI

	return context.m_bSourceFound;
}

BOOL CBldMreDependencies::Release()
{
	BOOL bOk = 0 < m_cRef;

	VSASSERT(NULL != m_pMre, "Invalid MreDependencies object.  Bad initialization.");
	VSASSERT(NULL != m_pMreUtil, "Invalid MreDependencies object.  Bad initialization.");

	m_cRef--;
	if (0 == m_cRef)
	{
		bOk = NULL != m_pMreUtil && m_pMreUtil->FRelease() && bOk;
		bOk = NULL != m_pMre && m_pMre->FClose(FALSE) && bOk;

		// Remove from map
		CritSectionT cs(g_sectionMreMap);
		VCPOSITION pos = s_OpenMreMap.GetStartPosition();
		VSASSERT(pos != NULL, "Ref count on MreMap says we have something; but we didn't find anything there");
		while (NULL != pos)
		{
			CStringW strFilename;
			CBldMreDependencies* pMreDepend;
			s_OpenMreMap.GetNextAssoc(pos, strFilename, (void*&)pMreDepend);
			VSASSERT(NULL != pMreDepend, "Bad file name/MreDependencies pointer pair");
			if (this == pMreDepend)
			{
				s_OpenMreMap.RemoveKey(strFilename);
				break;
			}
		}
		delete this;
	}

	return bOk;
}
