// DIRMGR.CPP
// ----------
// Implementation of CDirMgr and CToolset class.
//
// History
// =======
// 28-Aug-93	mattg		Created
// 10-Jan-94	colint		Added CToolset class
//
////////////////////////////////////////////////////////////
// Include files

#include "stdafx.h"
#pragma hdrstop

#include <bldapi.h>
#include <bldguid.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////
// Global variables

CDirMgr		g_theDirMgr;	// The one and only CDirMgr

CDirMgr * GetDirMgr() {return &g_theDirMgr;}

////////////////////////////////////////////////////////////
// Helper routines

static VOID DestroyListContents
(
	CObList *	pList
)
{
	POSITION	pos = pList->GetHeadPosition();

	while (pos != NULL)
		delete pList->GetNext(pos);

	pList->RemoveAll();
}

////////////////////////////////////////////////////////////
// CToolset Class

////////////////////////////////////////////////////////////
// Constructors, destructors

CToolset::CToolset
(
)
{
	INT			type;

	for (type=0; type<C_DIRLIST_TYPES ; ++type)
		m_Dirs[type] = new CObList;
}


CToolset::~CToolset
(
)
{
	INT 		type;
	CObList *	pList;

	for (type=0; type<C_DIRLIST_TYPES ; ++type)
	{
		if ((pList = m_Dirs[(DIRLIST_TYPE)type]) != NULL)
		{
			DestroyListContents(pList);
			delete pList;
		}	
	}
}

////////////////////////////////////////////////////////////
// CToolset::GetDirList

CObList * CToolset::GetDirList
(
	DIRLIST_TYPE	type
)
{
	ASSERT((type >= (DIRLIST_TYPE)0) && (type < C_DIRLIST_TYPES));

	return (m_Dirs[type]);
}

/////////////////////////////////////////////////////////////
// CToolset::GetDirListString

VOID CToolset::GetDirListString
(
	CString&		str,
	DIRLIST_TYPE	type
)
{
	ASSERT((type >= (DIRLIST_TYPE)0) && (type < C_DIRLIST_TYPES));

	str = m_DirString[type];
}

/////////////////////////////////////////////////////////////
// CToolset::RefreshAllCachedStrings

VOID CToolset::RefreshAllCachedStrings
(
)
{
	INT type;

	for ( type = 0; type < C_DIRLIST_TYPES ; ++type )
	{
		CString &str = m_DirString[type];
		CObList *pList = m_Dirs[type];

		str.Empty();

		if ( pList != NULL )
		{
			POSITION pos = pList->GetHeadPosition();
			while ( pos != NULL )
			{
				if (!str.IsEmpty())
					str += _T(';');
				
				str += (const TCHAR *)*(CDir *)pList->GetNext(pos);
			}
		}
	}
}

////////////////////////////////////////////////////////////
// CToolset::SetDirList

VOID CToolset::SetDirList
(
	DIRLIST_TYPE	type,
	CObList *		pListNew
)
{
	ASSERT((type >= (DIRLIST_TYPE)0) && (type < C_DIRLIST_TYPES));
	ASSERT(pListNew != NULL);

	if (m_Dirs[type] != NULL)
	{
		DestroyListContents(m_Dirs[type]);
		delete m_Dirs[type];
	}

	m_Dirs[type] = pListNew;
	RefreshAllCachedStrings( );
}

////////////////////////////////////////////////////////////
// CDirMgr Class

////////////////////////////////////////////////////////////
// Constructors, destructors

CDirMgr::CDirMgr
(
)
{
	m_nCurrentToolset = 0;
}

CDirMgr::~CDirMgr
(
)
{
 	POSITION		pos;
	CToolset *		pToolset;

	pos = m_Toolsets.GetHeadPosition();
	while (pos != NULL)
	{
		pToolset = (CToolset *)m_Toolsets.GetNext(pos);
		delete pToolset;
	}
	m_Toolsets.RemoveAll();
}


////////////////////////////////////////////////////////////
// CDirMgr::GetCurrentToolset
//
// This returns the current toolset for the project, if
// one exists. If we don't have a project then we will
// return whatever m_nCurrentToolset was last set to.

INT CDirMgr::GetCurrentToolset
(
)
{
	INT	nToolset = m_nCurrentToolset;

	LPBUILDSYSTEM pInterface;
	theApp.FindInterface(IID_IBuildSystem, (LPVOID FAR *)&pInterface);
	if (pInterface != NULL)
	{
		// GetBuilderToolset only changes nToolset if valid project
		pInterface->GetBuilderToolset(&nToolset, ACTIVE_BUILDER);
		pInterface->Release();
	}

	return nToolset;
}
 
////////////////////////////////////////////////////////////
// CDirMgr::SetCurrentToolset
VOID CDirMgr::SetCurrentToolset
(
	INT		nToolset
)
{ 
	if (nToolset >= 0 && nToolset < m_Toolsets.GetCount())
		m_nCurrentToolset = nToolset; 
}

////////////////////////////////////////////////////////////
// CDirMgr::GetNumberOfToolsets

INT CDirMgr::GetNumberOfToolsets
(
)
{
	return m_Toolsets.GetCount();
}

////////////////////////////////////////////////////////////
// CDirMgr::AddToolset

INT CDirMgr::AddToolset
(
	const CString & strTargetPlatform
)
{
	INT				nToolset;
	CToolset *		pToolset;

	nToolset = m_Toolsets.GetCount();
	pToolset = new CToolset;
	m_Toolsets.AddTail(pToolset);
	m_ToolsetNames.AddTail(strTargetPlatform);

	return nToolset;
}

////////////////////////////////////////////////////////////
// CDirMgr::DeleteToolset

VOID CDirMgr::DeleteToolset
(
	INT nToolset
)
{
	POSITION 	pos;
	CToolset *	pToolset;

	if (nToolset >= 0 && nToolset < m_Toolsets.GetCount())
	{
		pos = m_Toolsets.FindIndex(nToolset);
		pToolset = (CToolset *)m_Toolsets.GetAt(pos);
		delete pToolset;
		m_Toolsets.RemoveAt(pos);
		m_ToolsetNames.RemoveAt(pos);
	}
}

////////////////////////////////////////////////////////////
// CDirMgr::GetToolset

CToolset * CDirMgr::GetToolset
(
	INT 	nToolset
)
{
	POSITION pos = m_Toolsets.FindIndex(nToolset);
	if (pos != NULL)
		return (CToolset *) (m_Toolsets.GetAt(pos));
	else
		return NULL;

}

////////////////////////////////////////////////////////////
// CDirMgr::GetDirList

const CObList * CDirMgr::GetDirList
(
	DIRLIST_TYPE	type,
	INT		nToolset	/* = -1 */
)
{
	if (nToolset == -1)
		nToolset = GetCurrentToolset();

	ASSERT((type >= (DIRLIST_TYPE)0) && (type < C_DIRLIST_TYPES));
	CToolset * pToolset = GetToolset(nToolset);
	if (pToolset != NULL)
		return(pToolset->GetDirList(type));
	else
		return NULL;
}

////////////////////////////////////////////////////////////
// CDirMgr::GetDirListString

VOID CDirMgr::GetDirListString
(
	CString &		strRet,
	DIRLIST_TYPE	type,
	INT				nToolset	/* = -1 */
)
{
	if (nToolset == -1)
		nToolset = GetCurrentToolset();
	
	ASSERT((type >= (DIRLIST_TYPE)0) && (type < C_DIRLIST_TYPES));
	CToolset * pToolset = GetToolset(nToolset);
	if (pToolset != NULL)
		pToolset->GetDirListString(strRet, type);
	else
		strRet.Empty();
}

////////////////////////////////////////////////////////////
// CDirMgr::CloneDirList

CObList * CDirMgr::CloneDirList
(
	DIRLIST_TYPE	type,
	INT				nToolset
)
{
	POSITION	pos;
	CObList *	pListSrc;
	CObList *	pListDst;
	CDir *		pDirSrc;
	CDir *		pDirDst;

	ASSERT((type >= (DIRLIST_TYPE)0) && (type < C_DIRLIST_TYPES));

	pListSrc = GetToolset(nToolset)->GetDirList(type);
	pListDst = new CObList;

	pos = pListSrc->GetHeadPosition();

	while (pos != NULL)
	{
		pDirSrc = (CDir *)pListSrc->GetNext(pos);
		ASSERT(pDirSrc->IsKindOf(RUNTIME_CLASS(CDir)));
		pDirDst = new CDir(*pDirSrc);

		pListDst->AddTail(pDirDst);
	}

	return(pListDst);
}

////////////////////////////////////////////////////////////
// CDirMgr::SetDirList

VOID CDirMgr::SetDirList
(
	DIRLIST_TYPE	type,
	INT				nToolset,
	CObList *		pListNew
)
{
	CObList *		pList;
 
	ASSERT(pListNew != NULL);
	pList = GetToolset(nToolset)->GetDirList(type);
		
	GetToolset(nToolset)->SetDirList(type, pListNew);
}

////////////////////////////////////////////////////////////
// CDirMgr::SetDirListFromString

VOID CDirMgr::SetDirListFromString
(
	DIRLIST_TYPE	type,
	INT				nToolset,
	const TCHAR *	sz,
	BOOL			fMustExist /* FALSE */
)
{
	TCHAR *		pchCopy;
	TCHAR *		pchDir;
	CString		str = sz;
	CObList *	pList;
	CDir *		pDir;

	pList = GetToolset(nToolset)->GetDirList(type);

	if (pList == NULL)
		pList = new CObList;
	else
		DestroyListContents(pList);

	pchCopy = str.GetBuffer(1);
	pchDir = _tcstok(pchCopy, _T(";"));

	while (pchDir != NULL)
	{
		pDir = new CDir;
		if (!pDir->CreateFromString(pchDir))
		{
			ASSERT(FALSE);
			delete pDir;
			pchDir = _tcstok(NULL, _T(";"));

			continue;
		}

		// Check that the directory is not already
		// in the list. We do not add duplicates.
		BOOL fAddDir = TRUE;

 		POSITION pos = pList->GetHeadPosition();
 		while (pos != NULL)
		{
			CDir * pTempDir = (CDir *)pList->GetNext(pos);
			if (*pTempDir == *pDir)
			{
				fAddDir = FALSE;
				break;	// found, break-out
			}
		}

		// If we are to add this and it must exist, make
		// sure it does, otherwise don't add it
		if (fMustExist && fAddDir)
			fAddDir = pDir->ExistsOnDisk();

		// If the directory is not a duplicate then add it,
		// else de-allocate
		if (fAddDir)			
		 	pList->AddTail(pDir);
		else
			delete pDir;

		pchDir = _tcstok(NULL, _T(";"));
	}

	GetToolset(nToolset)->RefreshAllCachedStrings();

	str.ReleaseBuffer();
}

////////////////////////////////////////////////////////////
// CDirMgr::GetPlatformToolset

INT	CDirMgr::GetPlatformToolset
(
	const CString & strPlatform
)
{
	POSITION	pos;
	CString		strToolsetName;
	INT			nToolset = 0;

	pos = m_ToolsetNames.GetHeadPosition();
	while (pos != NULL)
	{
		strToolsetName = m_ToolsetNames.GetNext(pos);
		if (strToolsetName == strPlatform)
			return nToolset;
		nToolset++;
	}

	return -1;
}

////////////////////////////////////////////////////////////
// CDirMgr::GetToolsetName

CString	& CDirMgr::GetToolsetName
(
	INT nToolset
)
{
	POSITION pos = m_ToolsetNames.FindIndex(nToolset);
	ASSERT (pos != NULL);
	return m_ToolsetNames.GetAt(pos);
}

BOOL IsFileThere(const CString& strDir, const CString& strFName, CString& strFullPath)
{
	ASSERT(!strFName.IsEmpty());

	const TCHAR *pch = strFName;
	if ((strFName[0] != _T('\\')) &&
	    ((strFName.GetLength() < 2) || (*_tcsinc(pch) != _T(':'))))
	{
		if (strDir.IsEmpty())
			return FALSE;

		strFullPath = strDir;

		// Add a backslash between path and fname if needed
		// chauv - crystal 1529
		// can't use CString.Right(1).Compare("\\")) since this won't work with trailing backslash char in MBCS
		// if (strFullPath.Right(1).Compare("\\"))
		if ( _tcsrchr(strFullPath, '\\') != strFullPath.Right(1) )
			strFullPath += "\\";
	}

	strFullPath += strFName;

	if (_access(strFullPath, 04) == 0)	// check for read privs
		return TRUE;

	return FALSE;
}

BOOL FindFileOnPath(const CString& strName, CString& strFullPath,
	DIRLIST_TYPE dt /*=DIRLIST_INC*/, INT nToolset /*=-1*/)
{
	ASSERT(!strName.IsEmpty());
	CString strDir;

	// Check in project directory, if any
	LPBUILDSYSTEM pBldSysIFace;
	theApp.FindInterface(IID_IBuildSystem, (LPVOID FAR *)&pBldSysIFace);
	if (pBldSysIFace)
	{
		if (pBldSysIFace->IsActiveBuilderValid() == S_OK)
		{
			CDir dir;
			LPCSTR pszPath;
			pBldSysIFace->GetBuilderFile(ACTIVE_BUILDER, &pszPath);
			if (dir.CreateFromPath(pszPath))
			{
				strDir = dir;
				ASSERT(!strDir.IsEmpty());
				if (IsFileThere(strDir, strName, strFullPath))
				{
					pBldSysIFace->Release();
					return TRUE;
				}
			}
		}
		pBldSysIFace->Release();
	}

	// Check in all the directories on the INCLUDE path
	CDirMgr* pDirMgr = GetDirMgr();
	ASSERT(pDirMgr);

	CDir* pDir;
	const CObList* pDirList = pDirMgr->GetDirList(dt, nToolset);
	POSITION pos = pDirList->GetHeadPosition();
	while (pos)
	{
		pDir = (CDir*) pDirList->GetNext(pos);
		strDir = *pDir;
		if (IsFileThere(strDir, strName, strFullPath))
			return TRUE;
	}

	// Check in current directory
	GetCurrentDirectory(_MAX_PATH, strDir.GetBufferSetLength(_MAX_PATH));
	strDir.ReleaseBuffer();
	if (IsFileThere(strDir, strName, strFullPath))
		return TRUE;

	return FALSE;
}
