/*
 *
 * recurse.cpp
 *
 * Implementation for recursive routines
 *
 */

#include "precomp.hxx"

BOOL fRecurse;
BOOL fIncludeHidden;
BOOL fSubdirs;

BOOL FParseFilrOption(char ch)
{
	switch(ch) {
	case 'h':
	case 'H':
		++fIncludeHidden;
		break;
	case 'r':
	case 'R':
		++fRecurse;
		break;
	case 's':
	case 'S':
		++fSubdirs;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

PDM_FILE_ATTRIBUTES FILR::PdmfaGet(void)
{
	if(!m_fat.fValid) {
		HRESULT hr = FIL::HrGetFileAttributes(&m_fat.dmfa);
		if(FAILED(hr)) {
			Warning(hr, 0);
			return NULL;
		}
		m_fat.fValid = TRUE;
	}
	return &m_fat.dmfa;
}

void FILR::DoOneFile(LPCSTR szPat)
{
	PDM_FILE_ATTRIBUTES pdmfa = PdmfaGet();

	if(!pdmfa)
		return;
	if(m_fMatches && !m_fDirsOnly)
		DoWork();
	if(pdmfa->Attributes & FILE_ATTRIBUTE_DIRECTORY) {
		/* Only do this directory if it's approved */
		if((fRecurse || fSubdirs) && FStartDir()) {
			DoSubTree(fRecurse && m_fMatches ? NULL : szPat, true);
		}
	}
}

void FILR::DoSubTree(LPCSTR szPattern, bool fDoEndDir)
{
	FILR *pfilr;
	PDM_WALK_DIR pdmwd;
	HRESULT hr;
	PDM_FILE_ATTRIBUTES pdmfa;
	FILR *pfilrList;
	FILR **ppfilr;

	pfilr = PfilrCopy();

	/* Walk the file tree and process each file that matches the pattern */
	pfilr->Descend();
	ppfilr = &pfilrList;
	pdmwd = NULL;
	while(SUCCEEDED(hr = pfilr->HrWalkDir(&pdmwd, NULL))) {
		pfilr->m_fMatches = szPattern ? pfilr->FWildMatch(szPattern) : TRUE;
        /* If this is no match and we're not going to recurse, then there's 
         * no reason to do an attributes fetch */
        if(!pfilr->m_fMatches && !fRecurse && !fSubdirs)
            continue;
		pdmfa = pfilr->PdmfaGet();
		if(!pdmfa)
			continue;
		if((pdmfa->Attributes & FILE_ATTRIBUTE_HIDDEN) && !fIncludeHidden)
			continue;
		if(m_fBreadthFirst && (fRecurse || fSubdirs) &&
			(pdmfa->Attributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			*ppfilr = pfilr->PfilrCopy();
			ppfilr = &(*ppfilr)->m_pfilrNext;
			if(pfilr->m_fMatches && !m_fDirsOnly)
				pfilr->DoWork();
		} else
			pfilr->DoOneFile(szPattern);
	}
	if(hr != XBDM_ENDOFLIST)
		Warning(hr, 0);
	pfilr->EndWalkDir(pdmwd);
	delete pfilr;

	if(fDoEndDir && m_fPreOrder)
		EndDir();

	/* Do any deferred work we have */
	*ppfilr = NULL;
	while(pfilrList) {
		pfilr = pfilrList;
		pfilrList = pfilr->m_pfilrNext;
		if(pfilr->FStartDir()) {
			pfilr->DoSubTree(fRecurse ? NULL : szPattern, true);
		}
		delete pfilr;
	}

	if(fDoEndDir && !m_fPreOrder)
		EndDir();
}

void FILR::DoTree(BOOL fIncludeRootDir)
{
	if(FIsWildName() || fSubdirs || fIncludeRootDir) {
		FILR *pfilr = PfilrCopy();
		pfilr->m_szName[0] = 0;
		if(!fIncludeRootDir || pfilr->FStartDir()) {
			pfilr->DoSubTree(m_szName, fIncludeRootDir ? true : false);
		}
		delete pfilr;
	} else {
		m_fMatches = TRUE;
		DoOneFile(NULL);
	}
}