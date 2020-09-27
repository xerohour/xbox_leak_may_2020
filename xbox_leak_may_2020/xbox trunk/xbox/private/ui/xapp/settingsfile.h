#pragma once

class CSettingsFileValue
{
public:
	CSettingsFileValue();
	~CSettingsFileValue();

	CSettingsFileValue* m_pNextValue;
	TCHAR* m_szName;
	TCHAR* m_szValue;
};

class CSettingsFileSection
{
public:
	CSettingsFileSection();
	~CSettingsFileSection();

	CSettingsFileValue* FindValue(const TCHAR* szName);
	bool SetValue(const TCHAR* szName, const TCHAR* szValue);

	CSettingsFileSection* m_pNextSection;
	TCHAR* m_szName;
	CSettingsFileValue* m_pValueList;
};

class CSettingsFile
{
public:
	CSettingsFile();
	~CSettingsFile();

	bool OpenDir(const TCHAR* szDir);
	bool Open(const TCHAR* szFile);
	bool Close();
	void Cancel();
	bool Save();

	inline const TCHAR* GetFileName() const
	{
		return m_szFilePath;
	}

	bool GetValue(const TCHAR* szSection, const TCHAR* szName, TCHAR* szValueBuf, int cchValueBuf);
	void SetValue(const TCHAR* szSection, const TCHAR* szName, const TCHAR* szValue);

protected:
	CSettingsFileSection* FindSection(const TCHAR* szSection, bool bCreate = false);

	TCHAR* m_szFilePath;
	CSettingsFileSection* m_pSectionList;
	bool m_bDirty;

#ifdef _UNICODE
	bool m_bUnicode;
#endif
};
