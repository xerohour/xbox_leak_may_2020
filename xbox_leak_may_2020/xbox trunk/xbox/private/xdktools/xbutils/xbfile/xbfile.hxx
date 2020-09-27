BOOL FIsWildName(LPCTSTR sz);
BOOL FWildMatch(LPCTSTR szPattern, LPCTSTR szName);
BOOL FPatMatch(const char *pchPat, int cchPat, const char *pchName,
	int cchName, int *rgichStart=NULL, int *rgichLen=NULL, int cichWilds=0);
void Basename(LPCTSTR szPath, const TCHAR **ppchFile, const TCHAR **ppchExt);

struct FAT {
    DM_FILE_ATTRIBUTES dmfa;
    BOOL fValid;
};

struct FIL {
	TCHAR m_szDir[1024];
	TCHAR m_szName[256];
	char m_fXbox; // value is xbox drive letter
    char m_chPCDrive;   // Value is PC Drive letter

	FIL &operator =(LPCTSTR);
	bool operator ==(const FIL &);
	FIL(void) {}
	FIL(LPCSTR sz) { *this = sz; }

	void Descend(void);
	HRESULT HrLocalTempFile(void);
	HRESULT HrGetFileAttributes(PDM_FILE_ATTRIBUTES pdmfa) const;
	HRESULT HrSetFileAttributes(PDM_FILE_ATTRIBUTES pdmfa) const;
	BOOL FIsWildName(void) const { return ::FIsWildName(m_szName); }
	BOOL FWildMatch(LPCTSTR szPat) const
		{ return ::FWildMatch(szPat, m_szName); }
	//HRESULT HrMoveFile(const FIL &filTo, BOOL fSend) const;
	void FillFullName(LPTSTR, BOOL fOmitXboxDrive=FALSE) const;
	void FillDisplayName(LPTSTR) const;
	HRESULT HrMkdir(void) const;
	HRESULT HrRename(FIL &) const;
	HRESULT HrDelete(BOOL fIsDir=FALSE) const;
	HRESULT HrWalkDir(PDM_WALK_DIR *, LPCTSTR szPatMatch=NULL,
        FAT *pfat=NULL);
	HRESULT HrEnsureDir(void) const;
	//void ShortName(void);
	void EndWalkDir(PDM_WALK_DIR);
	void Fatal(HRESULT hr, DWORD ids) const;
	void Warning(HRESULT hr, DWORD ids) const;
};

void Fatal(HRESULT hr, DWORD ids, ...);
void Warning(HRESULT hr, DWORD ids, ...);
BOOL FFmtIds(DWORD ids, LPTSTR lpsz, ...);
HRESULT HrLastError(void);

enum { IDP_ERROR, IDP_WARNING };
void SetIdpIds(DWORD idp, DWORD ids);
