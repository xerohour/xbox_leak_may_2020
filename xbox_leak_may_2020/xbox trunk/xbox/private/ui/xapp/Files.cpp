#include "std.h"
#include "xapp.h"
#include "FileUtil.h"
#include "Node.h"
#include "Date.h"
#include "Runner.h"

class CFile : public CNode
{
	DECLARE_NODE(CFile, CNode)
public:
	CFile();
	~CFile();

	TCHAR* m_name;
	TCHAR* m_type;
	TCHAR* m_path;
	int m_length;
	CDateObject* m_date;

	CStrObject* readText();

	DECLARE_NODE_PROPS()
	DECLARE_NODE_FUNCTIONS()
};


class CFolder : public CNode
{
	DECLARE_NODE(CFolder, CNode)
public:
	CFolder();
	~CFolder();

	TCHAR* m_path;
	TCHAR* m_name;
	CNodeArray m_files;
	CNodeArray m_subFolders;

	bool OnSetProperty(const PRD* pprd, const void* pvValue);

	void Refresh(const TCHAR* szPath);

	void sortByName();
	void sortByType();
	void sortByDate();
	void sortByLength();

	DECLARE_NODE_PROPS()
	DECLARE_NODE_FUNCTIONS()
};


IMPLEMENT_NODE("File", CFile, CNode)

START_NODE_PROPS(CFile, CNode)
	NODE_PROP(pt_string, CFile, name)
	NODE_PROP(pt_string, CFile, type)
	NODE_PROP(pt_string, CFile, path)
	NODE_PROP(pt_integer, CFile, length)
	NODE_PROP(pt_node, CFile, date)
END_NODE_PROPS()

START_NODE_FUN(CFile, CNode)
	NODE_FUN_SV(readText)
END_NODE_FUN()


IMPLEMENT_NODE("Folder", CFolder, CNode)

START_NODE_PROPS(CFolder, CNode)
	NODE_PROP(pt_string, CFolder, path)
	NODE_PROP(pt_string, CFolder, name)
	NODE_PROP(pt_nodearray, CFolder, files)
	NODE_PROP(pt_nodearray, CFolder, subFolders)
END_NODE_PROPS()

START_NODE_FUN(CFolder, CNode)
	NODE_FUN_VV(sortByName)
	NODE_FUN_VV(sortByType)
	NODE_FUN_VV(sortByDate)
	NODE_FUN_VV(sortByLength)
END_NODE_FUN()


CFile::CFile() :
	m_name(NULL),
	m_type(NULL),
	m_path(NULL),
	m_length(-1),
	m_date(NULL)
{
}

CFile::~CFile()
{
	delete [] m_name;
	delete [] m_type;
	delete [] m_path;
	delete m_date;
}

CStrObject* CFile::readText()
{
	CStrObject* pStr = new CStrObject;

	if (m_path != NULL)
	{
		CActiveFile file;
		if (file.Fetch(m_path))
		{
#ifdef _UNICODE
			file.MakeUnicode();
#endif

			pStr->Append((TCHAR*)file.GetContent());
		}
	}

	return pStr;
}


CFolder::CFolder() :
	m_path(NULL),
	m_name(NULL)
{
}

CFolder::~CFolder()
{
	delete [] m_path;
	delete [] m_name;
}

bool CFolder::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	if ((int)pprd->pbOffset == offsetof(m_path))
	{
		const TCHAR* szPath = *((const TCHAR**)pvValue);
		Refresh(szPath);
	}

	return true;
}

void CFolder::Refresh(const TCHAR* szPath)
{
	TCHAR szWild [MAX_PATH];
	WIN32_FIND_DATA fd;
	HANDLE h;

	lstrcpyn(szWild, szPath, MAX_PATH - sizeof("\\*.*"));
	_tcscat(szWild, _T("\\*.*"));

	char szWildA [MAX_PATH];
	Ansi(szWildA, szWild, MAX_PATH);
	h = FindFirstFile(szWildA, &fd);

    if (h == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("\001Cannot read directory: \"%s\" (%d)\n"), szPath, GetLastError());
		return;
	}

	// BLOCK: Update name based on path...
	{
		const TCHAR* pchName = _tcsrchr(szPath, '\\');
		if (pchName == NULL)
			pchName = _T("");
		else
			pchName += 1;

		int cchName = _tcslen(pchName);
		delete [] m_name;

		m_name = new TCHAR [cchName + 1];
		_tcscpy(m_name, pchName);
	}


	do
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
			continue;

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
			continue;

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)
			continue;

		TCHAR szFileName [MAX_PATH];
		Unicode(szFileName, fd.cFileName, countof(szFileName));

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			CFolder* pFolder = new CFolder;
			m_subFolders.AddNode(pFolder);

			pFolder->m_path = new TCHAR [_tcslen(szPath) + 1 + _tcslen(szFileName) + 1];
			_stprintf(pFolder->m_path, _T("%s\\%s"), szPath, szFileName);

			pFolder->m_name = new TCHAR [_tcslen(szFileName) + 1];
			_tcscpy(pFolder->m_name, szFileName);
		}
		else
		{
			CFile* pFile = new CFile;
			m_files.AddNode(pFile);

			pFile->m_date = new CDateObject;
			pFile->m_date->m_time = fd.ftLastWriteTime;

			const TCHAR* pch = _tcsrchr(szFileName, '.');

			int cchName = _tcslen(szFileName);
			if (pch != NULL)
				cchName -= _tcslen(pch);
			pFile->m_name = new TCHAR [cchName + 1];
			CopyChars(pFile->m_name, szFileName, cchName);
			pFile->m_name[cchName] = 0;

			if (pch != NULL)
				pch += 1;
			else
				pch = _T("");
			pFile->m_type = new TCHAR [_tcslen(pch) + 1];
			_tcscpy(pFile->m_type, pch);
			_tcslwr(pFile->m_type);

			pFile->m_path = new TCHAR [_tcslen(szPath) + 1 + _tcslen(szFileName) + 1];
			_stprintf(pFile->m_path, _T("%s\\%s"), szPath, szFileName);

			pFile->m_length = fd.nFileSizeLow;
		}
	}
	while (FindNextFile(h, &fd));

	FindClose(h);
}

static int __cdecl CmpName(const void* p1, const void* p2)
{
	const CFile* pFile1 = *(CFile**)p1;
	const CFile* pFile2 = *(CFile**)p2;

	return _tcsicmp(pFile1->m_name, pFile2->m_name);
}

void CFolder::sortByName()
{
	m_files.Sort(&CmpName);
}

static int __cdecl CmpType(const void* p1, const void* p2)
{
	const CFile* pFile1 = *(CFile**)p1;
	const CFile* pFile2 = *(CFile**)p2;

	return _tcsicmp(pFile1->m_type, pFile2->m_type);
}

void CFolder::sortByType()
{
	m_files.Sort(CmpType);
}

static int __cdecl CmpDate(const void* p1, const void* p2)
{
	const CFile* pFile1 = *(CFile**)p1;
	const CFile* pFile2 = *(CFile**)p2;

	return CompareFileTime(&pFile1->m_date->m_time, &pFile2->m_date->m_time);
}

void CFolder::sortByDate()
{
	m_files.Sort(CmpDate);
}

static int __cdecl CmpLength(const void* p1, const void* p2)
{
	const CFile* pFile1 = *(CFile**)p1;
	const CFile* pFile2 = *(CFile**)p2;

	return pFile1->m_length - pFile2->m_length;
}

void CFolder::sortByLength()
{
	m_files.Sort(CmpLength);
}

