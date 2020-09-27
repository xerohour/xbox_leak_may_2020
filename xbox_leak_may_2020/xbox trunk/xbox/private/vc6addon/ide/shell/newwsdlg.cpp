// prjdlg.cpp : implementation file
//

#include "stdafx.h"
#include "dlgbase.h"
#include "utilbld_.h"
#include "resource.h"
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CProjTypeListItem::CProjTypeListItem(const TCHAR* szText, HICON hIcon, TYPE_ITEM_CLASS itemClass,
									 int index /* =0*/)
{
	m_strText = szText;
	m_class = itemClass;
	m_hIcon = hIcon;
	m_index = index;
}

CProjTypeListItem::~CProjTypeListItem()
{
}
	
void GetRegWorkspaceDir(CString& strLocation);

BOOL RunFileExistsDlg(const char* szFile)
{
	TCHAR strIn[80];
	TCHAR strOut[2048];
	// dont use AfxFormatString, other people need ERR_File_Exists too!
	AfxLoadString(ERR_File_Exists, strIn );
	wsprintf( strOut, strIn, szFile );
	return
		(AfxMessageBox(strOut, MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2) == IDYES);
}

static BOOL DoesFileExist(const char* szFile)
{
    struct _stat st;
    return (_stat(szFile, &st) == 0);
}

BOOL DestroyFile(const char* szFile)
{
	if (!DoesFileExist(szFile))
		return TRUE;
	CString strPrompt;
	AfxFormatString1(strPrompt, IDS_ERROR_CANT_DELETE_FILE, szFile);
	while (::remove(szFile) == -1)
	{
		if (AfxMessageBox(strPrompt, MB_RETRYCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
			return FALSE;
	}
	return TRUE;
}

// These guys make sure the user doesn't enter a "reserved" name as a workspace
// name.
static const char* rgszReservedWorkspace[] =
{
	"aux",
	"con",
	"ole2",
	"compobj",
	"mfcoleui",
	"ole2conv",
	"ole2disp",
	"ole2nls",
	"ole2prox",
	"storage",
	"typelib",
	"odbc",
	"comm",
	"lpt1",
	"lpt2",
	"lpt3",
	"lpt4",
	"com1",
	"com2",
	"com3",
	"com4",
	"nul",
	"gdi",
	"krnl386",
	"user",
	"win386",
    NULL
};

static BOOL IsReservedProjectName(const char* psz)
{
    for (const char **ppsz = rgszReservedWorkspace; *ppsz != NULL; ppsz++)
    {
        if (strcmp(psz, *ppsz) == 0)
            return TRUE;
    }
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Japanese-specific functions

// This tests whether we're on a Japanese system
BOOL IsJapaneseSystem(void)
{
	return (PRIMARYLANGID(GetSystemDefaultLangID()) == LANG_JAPANESE);
}

// This function assumes we're on a Japanese system
BOOL IsSBKatakana(unsigned char c)
{
	return c >= 0xa1 && c <= 0xdf;
}

#if 0
// Removed by martynl because never called
BOOL IsValidProjectName(const TCHAR* sz)
{
	if (sz[0] == _T('\0'))
		return FALSE;

	if (IsReservedProjectName(sz))
		return FALSE;

	int cchLeft = 8;                // 8 for file name
	DWORD dwMaxLen = 8;
	DWORD  dwDummy1;
	if (::GetVolumeInformation(NULL, NULL, 0, NULL, &dwMaxLen,
		&dwDummy1, NULL, 0))
	{
		// succesfully got info from file system -- use it.
		cchLeft = (int)dwMaxLen;
	}

	while (*sz != _T('\0'))
	{
		if (_istalnum(*sz) || *sz == '_' || *sz == ' ' || IsDBCSLeadByte(*sz)
			|| (IsJapaneseSystem() && IsSBKatakana(*sz)))
		{
			// count the character
			cchLeft -= _tclen(sz);
			if (cchLeft < 0)
				return FALSE;		// too long
		}
		else
		{
			return FALSE;
		}

		sz = _tcsinc(sz);
	}

	return TRUE;
}
#endif

BOOL IsAbsolutePath(const TCHAR* sz)
{
	return _istalpha(sz[0]) && sz[1] == _T(':') || 
		sz[0] == _T('\\') && sz[1] == _T('\\');
}

void EnsureSlashAtEnd(CString& str)
{
	const TCHAR* pch = str;
	const TCHAR* pchLast = NULL;
	while (*pch != _T('\0'))
	{
		pchLast = pch;
		pch = _tcsinc(pch);
	}

	if (pchLast == NULL || *pchLast != _T('\\'))
		str += _T('\\');
}

static const TCHAR szDirectories [] = _TEXT("Directories");
static const TCHAR szWorkspaceDir [] = _TEXT("WorkspaceDir");
static const TCHAR szInstallDirs [] = _TEXT("Install Dirs");
static const TCHAR szProjects [] = _TEXT("MyProjects");

void WriteRegWorkspaceDir(const CString& strLocation)
{
	WriteRegString(szDirectories, szWorkspaceDir, strLocation);
}

void GetRegWorkspaceDir(CString& strLocation)
{
	strLocation = GetRegString(szDirectories, szWorkspaceDir);

	if (strLocation.IsEmpty())
	{
		// get the msdev.exe directory
		TCHAR rgch[MAX_PATH];
		(void) GetModuleFileName(theApp.m_hInstance, rgch, MAX_PATH);

		CPath pathExe;
		CDir dirExe;
		if (pathExe.Create(rgch))
			dirExe.CreateFromPath((const CPath)pathExe);

		strLocation = dirExe;

		int ich = strLocation.ReverseFind(_T('\\'));
		if (ich != -1 && ich == (strLocation.GetLength() - 1))
		{
			// remove last backslash
			strLocation = strLocation.Left(strLocation.GetLength() - 1);
		}

		CString strTmp = strLocation.Right(19);
		if (strTmp.CompareNoCase(_TEXT("\\common\\msdev98\\bin")) == 0)
		{
			strLocation = strLocation.Left(strLocation.GetLength() - 19);
			strLocation += _TEXT("\\");
			strLocation += szProjects;
			return;
		}

		strTmp = strLocation.Right(20);
		if (strTmp.CompareNoCase(_TEXT("\\common\\msdev98\\bind")) == 0)
		{
			strLocation = strLocation.Left(strLocation.GetLength() - 20);
			strLocation += _TEXT("\\");
			strLocation += szProjects;
			return;
		}

		strTmp = strLocation.Right(4);
		if (strTmp.CompareNoCase(_TEXT("\\bin")) == 0)
		{
			strLocation = strLocation.Left(strLocation.GetLength() - 4);
			strLocation += _TEXT("\\");
			strLocation += szProjects;
			return;
		}

		strTmp = strLocation.Right(5);
		if (strTmp.CompareNoCase(_TEXT("\\bind")) == 0)
		{
			strLocation = strLocation.Left(strLocation.GetLength() - 5);
			strLocation += _TEXT("\\");
			strLocation += szProjects;
			return;
		}

		strLocation += _TEXT("\\");
		strLocation += szProjects;
	}
}

////////////////////////////////////////////////////////////////////////////
// CProjTypeIconList class

// THIS CLASS IS TEMPORARY!!!
BEGIN_MESSAGE_MAP(CProjTypeIconList, CIconList)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CProjTypeIconList::OnDestroy()
{
	int nItemCount = GetCount();
	for (int i = 0; i < nItemCount; i += 1)
		delete (CProjTypeListItem*)GetItemDataPtr(i);

	CIconList::OnDestroy();
}

void CProjTypeIconList::SetIcon(int nItem, HICON hIcon)
{
	ASSERT(FALSE); // Not Used!
}

HICON CProjTypeIconList::GetIcon(int nItem)
{
	CProjTypeListItem* pItem = (CProjTypeListItem*)GetItemDataPtr(nItem);
	return pItem->GetIcon();
}

int CProjTypeIconList::AddItem(CProjTypeListItem* pItem)
{
	int nItem = AddString(pItem->GetText());
	ASSERT(nItem != LB_ERR);
	SetItemDataPtr(nItem, pItem);
	return nItem;
}

void CProjTypeIconList::InsertItem(int nInsertAt, CProjTypeListItem* pItem)
{
	InsertString(nInsertAt, pItem->GetText());
	SetItemDataPtr(nInsertAt, pItem);
}

////////////////////////////////////////////////////////////////////////////
// CProjTypeList class

CProjTypeList::CProjTypeList()
{

}


CProjTypeList::~CProjTypeList()
{
	int nItemCount = GetCount();
	for (int i = 0; i < nItemCount; i++)
		delete (CProjTypeListItem*)GetItemDataPtr(i);
}

HICON CProjTypeList::GetIcon(int nItem)
{
	CProjTypeListItem* pItem = (CProjTypeListItem*)GetItemDataPtr(nItem);
	return pItem->GetIcon();
}

int CProjTypeList::AddItem(CProjTypeListItem* pItem)
{
	int nItem = AddString(pItem->GetText());
	ASSERT(nItem != LB_ERR);
	SetItemDataPtr(nItem, pItem);
	return nItem;
}

void CProjTypeList::InsertItem(int nInsertAt, CProjTypeListItem* pItem)
{
	InsertString(nInsertAt, pItem->GetText());
	SetItemDataPtr(nInsertAt, pItem);
}

int CProjTypeList::AddString(LPCTSTR str)
{
	int nIndex = m_dataPtrs.Add(NULL);
	VERIFY(nIndex == m_strings.Add(str));
	return nIndex;
}

int CProjTypeList::InsertString(int nInsertAt, LPCTSTR str)
{
	m_dataPtrs.InsertAt(nInsertAt, (void *)NULL);
	m_strings.InsertAt(nInsertAt, str);
	return nInsertAt;
}

void CProjTypeList::GetText(int nItem, CString &str)
{
	ASSERT(nItem < GetCount());
	str = m_strings[nItem];
	return;
}

void *CProjTypeList::GetItemDataPtr(int nItem)
{
	ASSERT(nItem < GetCount());
	return m_dataPtrs[nItem];
}

void CProjTypeList::SetItemDataPtr(int nItem, void *ptr)
{
	ASSERT(nItem < GetCount());
	m_dataPtrs[nItem] = ptr;
	return;
}

////////////////////////////////////////////////////////////////////////////
