///////////////////////////////////////////////////////////////////////////////
//
//	CProjectDependency
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __PROJECTDEP__
#define __PROJECTDEP__

class BLD_IFACE CProjectDependency : public CProjItem
{
	DECLARE_SERIAL (CProjectDependency)

protected:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);

	//	File item commands
	//{{AFX_MSG(CFileItem)
	afx_msg void OnOpen();
	//}}AFX_MSG(CFileItem)

public:
	CProjectDependency();
	virtual ~CProjectDependency(); 

	virtual BOOL SetStrProp (UINT idProp, const CString &str);
	virtual GPT GetStrProp(UINT nIDProp, CString & val);

	// hooks to add as a dependent on the target we reference so we track changes in target name
	virtual BOOL PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);
	virtual BOOL PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);
	 	
	virtual BOOL UsesSourceTool () {return TRUE;}

	virtual void FlattenQuery(int fo, BOOL& bAddContent, BOOL& fAddItem);
 
	// .MAK reading and writing
	virtual BOOL WriteToMakeFile(CMakFileWriter& pw, CMakFileWriter& mw, const CDir &BaseDir, CNameMunger& nm);
	virtual BOOL ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir);
	void CProjectDependency::WriteRule( BOOL bClean, CMakFileWriter& mw, const CDir &BaseDir );


 	virtual BOOL CanAct(ACTION_TYPE action);
	virtual BOOL PreAct(ACTION_TYPE action);

	__inline void SetProjectDep(CTargetItem * pTarget, LPCTSTR pszTarget = NULL) { m_pTarget = pTarget; if (pszTarget) m_strName = pszTarget; }
	__inline CTargetItem * GetProjectDep() { return m_pTarget; }
	ConfigurationRecord* GetTargetConfig();
	ConfigurationRecord* GetCompatibleTargetConfig(ConfigurationRecord *);

//protected:
	
	// Return a container-less, symbol-less, clone of 'this'
	virtual CSlob* Clone();

private:
	// Makefile reading/writing support
	static const TCHAR *pcDepPrefix;

	BOOL IsDepComment(CObject*& pObject);

	CTargetItem	*	m_pTarget;	// Target we depend upon
	CString m_strName;			// Name of target
	DECLARE_SLOBPROP_MAP()
};

class CProjDepInfo {
public:
	CProjectDependency *	pProjDep;
	CString					strProject;
	CString					strTarget;
};

extern CPtrList g_lstprojdeps;

CProjDepInfo * AddDep( CString strTop, CString strSub, CProjectDependency *pProjectDep );
CProjDepInfo * AddDep( CProjectDependency *pProjectDep );
BOOL RemoveDep( CString strTop, CString strSub );
BOOL RemoveDep(  CProjectDependency *pProjDep  );
void GetSubProjects( CString &strTop, CStringList &listSub );
BOOL IsReachable(CString &strTop, CString &strSeek);
void InitWorkspaceDeps( void );
BOOL RemoveAllReferences( CString strProj );


#endif	// __PROJECTDEP__
