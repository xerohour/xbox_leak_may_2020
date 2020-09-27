// CBuildFile
//
// Build file reading and writing.
//
// [matthewt]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "bldrfile.h"	// local header
#include "version.h"    // for makefile versions
#include "project.h"	// g_pActiveProject

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define SECT_READ 0x0	// writing (supply buffer)
#define SECT_WRITE 0x1	// reading (process buffer)
#define SECT_END 0x2	// ended read or write (free buffer?)

//---------------------------------------------------
// EXTERNS



///////////////////////////////////////////////////////////////////////////////
// Convention: bump by 1 for OPT changes or simple makefile changes;
// Bump up to multiple of 10 for incompatible makefile format changes.

///////////////////////////////////////////////////////////////////////////////
// Makefile change history
// ALWAYS update this when bumping the OPT/Makefile version number
//
// Version number					Change							Made by
// 		30000						<reserved for release>			colint
// 		30001						Added targets					colint
//		30002						Fixed mirroring for targitems	colint
//		30003						Added file mapping				billjoy
//		30004						Removed old clw filename		colint
//      40000                       Break all old v3 makefiles      colint
//                                  (target name changes)
//      40001                       Fix INTDIR creation rule        colint
//                                  to use $(NULL), not nul


// Change these
static const UINT nMakeFileFormat = 00;	// internal makefile format number
const UINT nLastCompatibleMakefile = 40000;
const UINT nLastCompatibleOPTFile = 50009;

// Computed values do not alter
const UINT nVersionMajor = rmj;		// whole product major version
const UINT nVersionMinor = rmm;		// whole product minor version
const UINT nVersionNumber = (rmj * 10000) + (rmm * 100) + nMakeFileFormat;
const UINT nLastCompatibleMakefileMinor = ((nLastCompatibleMakefile % 10000) / 100);

#ifndef _SHIP
extern BOOL	g_bUseReleaseVersion;
#endif

// our instance of the build file reader and writer
CBuildFile  g_buildfile;

// builder file delimiters
static const TCHAR * pcPropertyPrefix = _TEXT("PROP ");
static const TCHAR * pcEndToken = _TEXT("End");

CBuildFile::CBuildFile()
{
}

CBuildFile::~CBuildFile()
{
}

// tool info. struct
typedef struct
{
	CBuildTool *	pTool;
	CBuildAction *	pAction;
	int				iAreas;
} S_ToolRec;

int TRcompare( const void *arg1, const void *arg2 )
{
	/* Compare all of both strings: */
	CBuildTool *pTool1 = (*( S_ToolRec ** )arg1)->pTool;
	CBuildTool *pTool2 = (*( S_ToolRec ** )arg2)->pTool;

	return _stricmp( (const TCHAR *)pTool1->m_strToolPrefix,(const TCHAR *)pTool2->m_strToolPrefix  );
}

// read in the VC++ builder file
BOOL CBuildFile::ReadBuildFile(const CProject * pBuilder)
{
	// remember this builder
	m_pBuilder = (CProject *)pBuilder;

	// no tool's used yet
	m_lstToolsUsed.RemoveAll();

	BOOL bRetval = TRUE;

	return bRetval;
}

// 'NMake syntax' section read
#define CCH_ALLOC_BLOCK		256
BOOL CBuildFile::ReadSectionForNMake(const TCHAR * pchname)
{
	// dynamic creation of sections?
	BOOL fAddDynaSection = FALSE;

	// get a section with this name?
	CBldrSection * pbldsect = g_BldSysIFace.FindSection(pchname);
	if (pbldsect == (CBldrSection *)NULL)
	{	
		HPROJECT hProj = (HPROJECT)g_pActiveProject; //dra
		if ((pbldsect = new CBldrSection(hProj)) == (CBldrSection *)NULL)
			return FALSE;

		// set the name, and mark as async.
		pbldsect->m_strName = pchname;
		pbldsect->m_pfn = NULL;

		fAddDynaSection = TRUE;
	}

	// init.
	pbldsect->m_pch = (TCHAR *)NULL;
	pbldsect->m_cch = 0;

 	// file exceptions can occur during builder file read
 	CFileException e;
	BOOL bRetval;

	// builder file object
	CObject * pObject = (CObject *)NULL;

	TRY
	{
		TCHAR * pch;
		size_t cch = 0;
		size_t cchAlloc = CCH_ALLOC_BLOCK;
		pch = (TCHAR *)calloc(cchAlloc + 1, sizeof(TCHAR));	// alloc. one for zero-term.
		if (pch == (TCHAR *)NULL)
			AfxThrowFileException(CFileException::generic);

		pbldsect->m_pch = pch;

		// break out when we reach the end of the section or EOF
		for (;;)
		{
			// read in section
			pObject = m_pmr->GetNextElement();
			VSASSERT(pObject != (CObject *)NULL, "NULL object in list!");

			// reached end of builder file prematurely?
			if (pObject->IsKindOf(RUNTIME_CLASS(CMakEndOfFile)))
				AfxThrowFileException(CFileException::generic);

			// read in our sections
			if (pObject->IsKindOf(RUNTIME_CLASS(CMakComment)))
			{
				TCHAR * pchComment = (TCHAR *)(const TCHAR *)((CMakComment *)pObject)->m_strText;

				// end of the section?
				if (!_tcsncmp(pchComment, pcEndToken, _tcslen(pcEndToken)))
				{
					*(pbldsect->m_pch + cch) = _T('\0'); cch++;	// zero. term

					// set out buffer size
					pbldsect->m_cch = cch;

					// provide the section data and get the return code?
					if (pbldsect->m_pfn)
						bRetval = (*pbldsect->m_pfn)(pbldsect, SECT_READ);
					else
						bRetval = TRUE;
					break;
				}

				// data length in section?
				size_t cchLen = _tcslen(pchComment);

			TryAgain:

				if ((cchLen + 1) > (cchAlloc - cch))	// incl zero. term
				{
					// need more memory so re-alloc
					cchAlloc += CCH_ALLOC_BLOCK;
					pch = (TCHAR *)realloc(pbldsect->m_pch, sizeof(TCHAR) * (cchAlloc + 1));		// alloc. one for zero-term.
					if (pch == (TCHAR *)NULL)
						AfxThrowFileException(CFileException::generic);

 					pbldsect->m_pch = pch;

					goto TryAgain;
				}

				// copy data into our buffer
				_tcscpy(pbldsect->m_pch + cch, pchComment);
				cch += cchLen;
				*(pbldsect->m_pch + cch) = _T('\0'); cch++;	// zero. term
			}

			// delete this object
			delete pObject; pObject = (CObject *)NULL;
		}
	}
	CATCH(CException, e)
	{	
		bRetval = FALSE;	// failure
	}
	END_CATCH

	// unget the last builder file element so that
	// our caller can process it
	if (pObject != (CObject *)NULL)
		m_pmr->UngetElement(pObject);

	// got a dyna-section and ok?
	if (fAddDynaSection)
	{
		// ok?
		if (bRetval)
		{
			// register this one
			g_BldSysIFace.RegisterSection((CBldrSection *)pbldsect);
		}
		else
		{
			// free up our data (we used sync. notify)
			if (pbldsect->m_pch != (TCHAR *)NULL)
				free(pbldsect->m_pch);

			// delete our section
			delete pbldsect;
		}
	}
	else
	{
		// free up our data (we used sync. notify)
		if (pbldsect->m_pch != (TCHAR *)NULL){
			free(pbldsect->m_pch);
			pbldsect->m_pch = NULL;
		}
	}

	return bRetval;
}
