#include "std.h"
#include "xapp.h"
#include "FileUtil.h"
#include "ActiveFile.h"
#include "xip.h"

#ifdef _DEBUG
extern void DumpHex(const BYTE* pbData, int cbData, int cbMax = 0);
#endif

CActiveFile::CActiveFile()
{
	m_szURL = NULL;
	m_pbContent = NULL;
	m_cbContent = 0;
	ZeroMemory(&m_modifiedTime, sizeof (m_modifiedTime));
	m_dwReadPos = 0;
	m_bInXIP = false;

	m_updatePeriod = 5.0f;
	m_nextUpdateTime = 0.0f;
}

CActiveFile::~CActiveFile()
{
	Reset();
}

void CActiveFile::Reset()
{
	delete [] m_szURL;

	XAppFreeMemory(m_pbContent);

	m_szURL = NULL;
	m_pbContent = NULL;
	m_cbContent = 0;
	ZeroMemory(&m_modifiedTime, sizeof (m_modifiedTime));
}

BYTE* CActiveFile::DetachContent()
{
	BYTE* pbContent = m_pbContent;
	m_pbContent = NULL;
	return pbContent;
}

bool CActiveFile::Fetch(const TCHAR* szURL, bool bSearchAppDir/*=false*/, bool bTry/*=false*/)
{
	Reset();

	m_szURL = new TCHAR [_tcslen(szURL) + 1];
	_tcscpy(m_szURL, szURL);

	if (m_szURL[0] == '\0')
	{
		return true;
	}

	else if (_tcsnicmp(m_szURL, _T("string:"), 7) == 0)
	{
		szURL += 7; // skip 'string:'

		m_cbContent = _tcslen(szURL) * sizeof (TCHAR);
		m_pbContent = (BYTE*)XAppAllocMemory(m_cbContent + sizeof (TCHAR));
		_tcscpy((TCHAR*)m_pbContent, szURL);
	}
	else // file system
	{
		if (!((szURL[0] == '\\' && szURL[1] == '\\') || (szURL[0] != 0 && szURL[1] == ':')))
		{
			// Make it absolute...
			TCHAR szBuf [MAX_PATH];
			MakeAbsoluteURL(szBuf, szURL);

			if (FindInXIPAndDetach(szBuf, m_pbContent, m_cbContent))
			{
				m_bInXIP = true;
				return true;
			}

			if (bSearchAppDir && !DoesFileExist(szBuf))
			{
				TRACE(_T("File not found: %s\n"), szBuf);
				_tcscpy(szBuf, theApp.m_szAppDir);
				_tcscat(szBuf, szURL);
				TRACE(_T("\ttrying %s instead...\n"), szBuf);
			}

			delete [] m_szURL;
			m_szURL = new TCHAR [_tcslen(szBuf) + 1];
			_tcscpy(m_szURL, szBuf);
		}

		if (FindInXIPAndDetach(m_szURL, m_pbContent, m_cbContent))
		{
			m_bInXIP = true;
			return true;
		}
		
		if (!FetchFile(bTry))
			return false;
	}

	return true;
}

bool CActiveFile::FetchFile(bool bTry/*=false*/)
{
	BYTE* pbContent;
	DWORD cbContent;
    MEMORYSTATUS stat;

	HANDLE hFile;

	if ((hFile = XAppCreateFile(m_szURL, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL)) == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("\001CActiveFile::FetchFile (%s) failed %d\n"), m_szURL, GetLastError());
		return false;
	}

	cbContent = GetFileSize(hFile, NULL);

    if (bTry)
    {
        GlobalMemoryStatus(&stat);
        if (stat.dwAvailPhys < (cbContent + 1024*1024))
        {
            return false;
        }
    }

    pbContent = (BYTE*)XAppAllocMemory(cbContent + sizeof (TCHAR));
    ReadFile(hFile, pbContent, cbContent, &cbContent, NULL);

	pbContent[cbContent] = 0;
#ifdef _UNICODE
	pbContent[cbContent + 1] = 0;
#endif

	if (!GetFileTime(hFile, NULL, NULL, &m_modifiedTime))
    {
        CloseHandle(hFile);
        XAppFreeMemory(m_pbContent);
        return false;
    }

	CloseHandle(hFile);

	XAppFreeMemory(m_pbContent);

	m_pbContent = pbContent;
	m_cbContent = cbContent;
	m_bInXIP = false;

//#ifdef _DEBUG
//	TRACE(_T("File: %d bytes\n"), m_cbContent);
//	DumpHex(m_pbContent, m_cbContent);
//#endif

	return true;
}

bool CActiveFile::Update()
{
	if (m_bInXIP)
		return false;

	if (m_szURL == NULL || m_szURL[0] == '\0')
		return false;

	if (m_updatePeriod == 0.0f || XAppGetNow() < m_nextUpdateTime)
		return false;

	m_nextUpdateTime = XAppGetNow() + m_updatePeriod + rnd(1.0f);

	if (_tcsnicmp(m_szURL, _T("string:"), 7) == 0)
		return false;

	// BLOCK: Check files...
	{
		HANDLE hFile;
		if ((hFile = XAppCreateFile(m_szURL, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL)) != INVALID_HANDLE_VALUE)
		{
			FILETIME ft;
			VERIFY(GetFileTime(hFile, NULL, NULL, &ft));
			CloseHandle(hFile);

			if (ft.dwHighDateTime > m_modifiedTime.dwHighDateTime || ft.dwLowDateTime > m_modifiedTime.dwLowDateTime)
			{
				TRACE(_T("File has changed...\n"));
				m_modifiedTime = ft;
				return FetchFile();
			}
		}
		else
		{
			TRACE(_T("Check file time failed (%s)!\n"), m_szURL);
		}
	}

	return false;
}

#ifdef _UNICODE
bool CActiveFile::IsUnicode()
{
	return (m_pbContent != NULL && m_cbContent > 2 && m_pbContent[0] == 0xff && m_pbContent[1] == 0xfe);
}

void CActiveFile::MakeUnicode()
{
	if (IsUnicode())
	{
		// NOTE: Once this happens, IsUnicode will return false!
		MoveMemory(m_pbContent, m_pbContent + 2, m_cbContent);
		return;
	}

	TCHAR* wsz = (TCHAR*)XAppAllocMemory((m_cbContent + 1) * 2);
	Unicode(wsz, (const char*)m_pbContent, m_cbContent);
	wsz[m_cbContent] = 0;
	XAppFreeMemory(m_pbContent);
	m_pbContent = (BYTE*)wsz;
	m_cbContent = (m_cbContent + 1) * 2;
	m_bInXIP = false;
}
#endif
