/* Recursive file operation */

extern BOOL fRecurse;
extern BOOL fIncludeHidden;
extern BOOL fSubdirs;

struct FILR : public FIL
{
	union {
		struct {
			DWORD 
				m_fDirsOnly:1,
				m_fBreadthFirst:1,
				m_fMatches:1,
				m_fPreOrder:1,
				unused:28;
		};
		DWORD m_dwFlags;
	};
    FAT m_fat;
	FILR *m_pfilrNext;

	void Init(void)
	{
		m_dwFlags = 0;
        m_fat.fValid = FALSE;
		m_pfilrNext = NULL;
	}
	FILR(void) : FIL() { Init(); }
	FILR(const char *sz) : FIL(sz) { Init(); }
	FILR(const FIL &fil) : FIL(fil) { Init(); }

	/* Overloaded functions */
	HRESULT HrWalkDir(PDM_WALK_DIR *ppdmwd, LPCTSTR szPatMatch=NULL)
	{
		return FIL::HrWalkDir(ppdmwd, szPatMatch, &m_fat);
	}
    /* The attributes form of HrWalkDir is defined but unimplemented because
     * it's unsupported on FILRs */
	HRESULT HrWalkDir(PDM_WALK_DIR *ppdmwd, LPCTSTR szPatMatch,
        FAT *pfat);
	HRESULT HrSetFileAttributes(PDM_FILE_ATTRIBUTES pdmfa)
	{
		m_fat.fValid = FALSE;
		return FIL::HrSetFileAttributes(pdmfa);
	}
    void EndWalkDir(PDM_WALK_DIR pdmwd)
    {
		m_fat.fValid = FALSE;
        FIL::EndWalkDir(pdmwd);
    }

	/* New functions */
	PDM_FILE_ATTRIBUTES PdmfaGet(void);
	void DoSubTree(LPCSTR szPattern, bool fDoEndDir = false);
	void DoTree(BOOL fIncludeRootDir=FALSE);
	void DoOneFile(LPCSTR);

	/* Implementation functions */
	virtual void DoWork(void) =0;
	virtual BOOL FStartDir(void) { return m_fMatches || fSubdirs; }
	virtual void EndDir(void) {}
	virtual FILR *PfilrCopy(void) const =0;
};

BOOL FParseFilrOption(char ch);
