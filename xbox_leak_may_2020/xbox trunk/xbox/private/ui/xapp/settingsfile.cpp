#include "std.h"
#include "XApp.h"
#include "SettingsFile.h"
#include "ActiveFile.h"
#include "Parser.h"

CSettingsFile::CSettingsFile()
{
	m_szFilePath = NULL;
	m_pSectionList = NULL;
	m_bDirty = false;

#ifdef _UNICODE
	m_bUnicode = false;
#endif
}

CSettingsFile::~CSettingsFile()
{
	Close();
}


bool CSettingsFile::OpenDir(const TCHAR* szDir)
{
	TCHAR szFile [MAX_PATH];
	_stprintf(szFile, _T("%s\\xbox.ini"), szDir);
	return Open(szFile);
}

bool CSettingsFile::Open(const TCHAR* szFile)
{
	CSettingsFileSection* pSection = NULL;

	ASSERT(m_szFilePath == NULL); // Already open?

	m_szFilePath = new TCHAR [_tcslen(szFile) + 1];
	_tcscpy(m_szFilePath, szFile);

	CActiveFile file;
	if (!file.Fetch(m_szFilePath, false, true))
		return false;

#ifdef _UNICODE
	m_bUnicode = file.IsUnicode();
	file.MakeUnicode();
#endif

	const TCHAR* pch = (const TCHAR*)file.GetContent();
	while (*pch != 0)
	{
		pch = SkipWhite(pch, none);

		if (*pch == '[')
		{
			pch += 1;

			TCHAR szSectionName [256];
			TCHAR* pchName = szSectionName;

			while (*pch != 0 && *pch != '\r' && *pch != '\n' && *pch != ']')
			{
				if (pchName >= szSectionName + countof(szSectionName) - 1)
				{
					TRACE(_T("\001Invalid XBX file (section name too long): %s\n"), m_szFilePath);
					Close();
					return false;
				}
				*pchName++ = *pch++;
			}
			*pchName = 0;

			if (*pch == ']')
			{
				pch += 1;
				pSection = FindSection(szSectionName, true);
			}
		}
		else
		{
			TCHAR szName [256];
			TCHAR* pchName = szName;
			TCHAR szValue [1024];
			TCHAR* pchValue = szValue;

			while (*pch != '\0' && *pch != '\r' && *pch != '\n' && *pch != '=')
			{
				if (pchName >= szName + countof(szName) - 1)
				{
					TRACE(_T("\001Invalid XBX file (name too long): %s\n"), m_szFilePath);
					Close();
					return false;
				}
				*pchName++ = *pch++;
			}
			*pchName = 0;

			if (*pch == '=')
			{
				pch += 1;

				while (*pch != '\0' && *pch != '\r' && *pch != '\n')
				{
					if (pchValue >= szValue + countof(szValue) - 1)
					{
						TRACE(_T("\001Invalid XBX file (value too long): %s\n"), m_szFilePath);
						Close();
						return false;
					}
					*pchValue++ = *pch++;
				}
				*pchValue = 0;

				if (pSection == NULL)
					pSection = FindSection(_T("default"), true);

				pSection->SetValue(szName, szValue);
			}
		}
	}

	return true;
}

void CSettingsFile::Cancel()
{
	m_bDirty = false;
	Close();
}

bool CSettingsFile::Save()
{
	if (!m_bDirty)
		return true;

#ifdef _UNICODE
	if (m_bUnicode)
	{
		// Write out Unicode file...

		FILE* pFile = _tfopen(m_szFilePath, _T("w"));
		if (pFile == NULL)
			return false;

		for (CSettingsFileSection* pSection = m_pSectionList; pSection != NULL; pSection = pSection->m_pNextSection)
		{
			_ftprintf(pFile, _T("[%s]\n"), pSection->m_szName);

			for (CSettingsFileValue* pValue = pSection->m_pValueList; pValue != NULL; pValue = pValue->m_pNextValue)
			{
				_ftprintf(pFile, _T("%s=%s\n"), pValue->m_szName, pValue->m_szValue);
			}
		}
	
		fclose(pFile);
	}
	else
	{
		// Convert our Unicode stuff to ANSI on the way out...

		HANDLE hFile = XAppCreateFile(m_szFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		DWORD dw;
		for (CSettingsFileSection* pSection = m_pSectionList; pSection != NULL; pSection = pSection->m_pNextSection)
		{
			char szBuf [1030];
			char* pch = szBuf;
			TCHAR* pwch = pSection->m_szName;
			*pch++ = '[';
			while (*pwch != 0)
				*pch++ = (char)*pwch++;
			*pch++ = ']';
			*pch++ = '\r';
			*pch++ = '\n';
			WriteFile(hFile, szBuf, (DWORD)(pch - szBuf), &dw, NULL);

			for (CSettingsFileValue* pValue = pSection->m_pValueList; pValue != NULL; pValue = pValue->m_pNextValue)
			{
				pch = szBuf;
				pwch = pValue->m_szName;
				while (*pwch != 0)
					*pch++ = (char)*pwch++;
				*pch++ = '=';
				WriteFile(hFile, szBuf, (DWORD)(pch - szBuf), &dw, NULL);

				pch = szBuf;
				pwch = pValue->m_szValue;
				while (*pwch != 0)
					*pch++ = (char)*pwch++;
				*pch++ = '\r';
				*pch++ = '\n';
				WriteFile(hFile, szBuf, (DWORD)(pch - szBuf), &dw, NULL);
			}
		}

		CloseHandle(hFile);
	}
#else
	// Just write out an ANSI file...

	FILE* pFile = fopen(m_szFilePath, "w");
	if (pFile == NULL)
		return false;

	for (CSettingsFileSection* pSection = m_pSectionList; pSection != NULL; pSection = pSection->m_pNextSection)
	{
		fprintf(pFile, "[%s]\n", pSection->m_szName);

		for (CSettingsFileValue* pValue = pSection->m_pValueList; pValue != NULL; pValue = pValue->m_pNextValue)
		{
			fprintf(pFile, "%s=%s\n", pValue->m_szName, pValue->m_szValue);
		}
	}

	fclose(pFile);
#endif

	m_bDirty = false;

	return true;
}

bool CSettingsFile::Close()
{
	if (!Save())
		return false;

	CSettingsFileSection* pNextSection;
	for (CSettingsFileSection* pSection = m_pSectionList; pSection != NULL; pSection = pNextSection)
	{
		pNextSection = pSection->m_pNextSection;
		delete pSection;
	}

	m_pSectionList = NULL;

	delete [] m_szFilePath;
	m_szFilePath = NULL;

	return true;
}

CSettingsFileValue* CSettingsFileSection::FindValue(const TCHAR* szName)
{
	for (CSettingsFileValue* pValue = m_pValueList; pValue != NULL; pValue = pValue->m_pNextValue)
	{
		if (_tcscmp(szName, pValue->m_szName) == 0)
			return pValue;
	}

	return NULL;
}

CSettingsFileSection* CSettingsFile::FindSection(const TCHAR* szSection, bool bCreate/*=false*/)
{
	for (CSettingsFileSection* pSection = m_pSectionList; pSection != NULL; pSection = pSection->m_pNextSection)
	{
		if (_tcscmp(szSection, pSection->m_szName) == 0)
			return pSection;
	}

	if (bCreate)
	{
		pSection = new CSettingsFileSection;
		pSection->m_szName = new TCHAR [_tcslen(szSection) + 1];
		_tcscpy(pSection->m_szName, szSection);
		pSection->m_pNextSection = m_pSectionList;
		m_pSectionList = pSection;
		return pSection;
	}

	return NULL;
}

bool CSettingsFile::GetValue(const TCHAR* szSection, const TCHAR* szName, TCHAR* szValueBuf, int cchValueBuf)
{
	szValueBuf[0] = 0;

	if (szSection == NULL)
		szSection = _T("default");

	CSettingsFileSection* pSection = FindSection(szSection);
	if (pSection == NULL)
	{
		// Also try default...
		pSection = FindSection(_T("default"));
		if (pSection == NULL)
			return false;
	}

	CSettingsFileValue* pValue = pSection->FindValue(szName);
	if (pValue == NULL)
		return false;

	_tcsncpy(szValueBuf, pValue->m_szValue, cchValueBuf);
	szValueBuf[cchValueBuf - 1] = 0;

	return true;
}


void CSettingsFile::SetValue(const TCHAR* szSection, const TCHAR* szName, const TCHAR* szValue)
{
	if (szSection == NULL)
		szSection = _T("default");

	CSettingsFileSection* pSection = FindSection(szSection, true);

	if (pSection->SetValue(szName, szValue))
	{
		m_bDirty = true;
		Save(); // REVIEW: every time???
	}
}

bool CSettingsFileSection::SetValue(const TCHAR* szName, const TCHAR* szValue)
{
	CSettingsFileValue* pValue = FindValue(szName);
	if (pValue == NULL)
	{
		pValue = new CSettingsFileValue;
		pValue->m_szName = new TCHAR [_tcslen(szName) + 1];
		_tcscpy(pValue->m_szName, szName);

		pValue->m_pNextValue = m_pValueList;
		m_pValueList = pValue;
	}

	if (pValue->m_szValue != NULL && _tcscmp(pValue->m_szValue, szValue) == 0)
		return false;

	delete [] pValue->m_szValue;
	pValue->m_szValue = new TCHAR [_tcslen(szValue) + 1];
	_tcscpy(pValue->m_szValue, szValue);

	return true;
}

CSettingsFileSection::CSettingsFileSection()
{
	m_szName = NULL;
	m_pValueList = NULL;
}

CSettingsFileSection::~CSettingsFileSection()
{
	CSettingsFileValue* pNextValue;
	for (CSettingsFileValue* pValue = m_pValueList; pValue != NULL; pValue = pNextValue)
	{
		pNextValue = pValue->m_pNextValue;
		delete pValue;
	}

	delete [] m_szName;
}

CSettingsFileValue::CSettingsFileValue()
{
	m_szName = NULL;
	m_szValue = NULL;
}

CSettingsFileValue::~CSettingsFileValue()
{
	delete [] m_szName;
	delete [] m_szValue;
}
