class CCheckList;

class AvailableLangDlls
{
public:
	AvailableLangDlls() { m_nDefaultEntry = -1, m_nUSEntry = 0; m_bLastFilledManually = FALSE;}
	BOOL ScanForAvailableLanguages();
	BOOL InitLangDllList(CComboBox* pCombo);
	BOOL GetAvailableLanguages(CStringList& strlLangs);
	void SetSupportedLanguages(LPCTSTR szSupportedLangs, BOOL bParseTranslations = TRUE);
	
	inline BOOL ScanForAvailableLanguagesIfNecessary()
		{	if (m_astrAvailableLangs.GetSize() == 0)
				return ScanForAvailableLanguages();
			else
				return TRUE; }
	
	inline DWORD GetTranslationAt(int i)
		{ return m_adwTranslations[i]; }
		
	CString m_strDefaultDllName;
	DWORD m_dwDefaultTranslation;

protected:
	CStringArray m_astrAvailableLangs;
	CDWordArray m_adwTranslations;
	int m_nUSEntry;
	int m_nDefaultEntry;
	BOOL m_bLastFilledManually;
};

class LangDlls
{
public:
	LangDlls();
	~LangDlls() { FreeLibs(); }
	inline BOOL IsAnythingLoaded() { return ((m_ahDll.GetSize() > 0)
										&& m_ahDll[0] != NULL); }
	inline BOOL IsValidEntry(int i) { return !m_astrNameDll[i].IsEmpty(); }
	BOOL SetDefaultLang(LPCTSTR szLangSuffix);
	LPCTSTR GetDefaultLang();

	void FreeLibs();
	BOOL LoadLibs();
	BOOL LoadLibsByName(LPCTSTR szLibs = NULL);
	BOOL LoadLangSpecificDefaults();
	int GetSize();
	void Init();
	BOOL CheckPreviouslyCheckedLangs(CComboBox* pCombo);
	void WriteLangDlls(CComboBox* pCombo);
	HRSRC FindLangResource(LPCTSTR lpszResource, HINSTANCE& hinst);

	CStringArray m_astrNameDll;		// Name of resource-only .dll, containing templates for
									//  all localizable text resources for generated app.
	CTypedPtrArray<CPtrArray, HINSTANCE> m_ahDll; // Handles to the .dlls
	CStringArray m_astrNameLang;	// Name of languages the .dlls use.
	CDWordArray m_adwTranslation;	// LANGID/code page pair of corresponding language

	CStringArray m_astrFiles;		// The string "Files" translated to DLL's language
	CWordArray m_abFilesAfter;		// Indicates whether "Files" appears after doctype

	// Localized doc template strings
	CStringArray m_astrTitle;
	CStringArray m_astrDocFileNew;
	CStringArray m_astrDocFilter;
    CStringArray m_astrDocTag;      // mixed case tag
	CStringArray m_astrDocRegName;

	// Tracking flags for doc template strings tab of advanced dialog
	CWordArray m_abUpdateFilter, m_abUpdateFileNew, m_abUpdateRegName;

	AvailableLangDlls m_AvailableLangDlls;

protected:
	int m_nDefaultDll;			// DLL to try to load loc tpl from first
	void ComplainAndFreeAll(int nDll);
	CString m_strDefaultLang;
};

extern LangDlls langDlls;
