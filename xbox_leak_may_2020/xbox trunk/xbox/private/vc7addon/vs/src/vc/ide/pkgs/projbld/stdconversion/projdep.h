///////////////////////////////////////////////////////////////////////////////
//
//	CProjectDependency
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class CProjectDependency : public CProjItem
{
	DECLARE_SERIAL (CProjectDependency)

public:
	CProjectDependency();
	virtual ~CProjectDependency(); 

	virtual GPT GetStrProp(UINT nIDProp, CString & val);

	// hooks to add as a dependent on the target we reference so we track changes in target name
	virtual BOOL PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);
	virtual BOOL PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);
	 	
	virtual BOOL UsesSourceTool() { return TRUE; }

	virtual void FlattenQuery(int fo, BOOL& bAddContent, BOOL& fAddItem);
 
	// .MAK reading
	virtual BOOL ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir);
	__inline CTargetItem * GetProjectDep() { return m_pTarget; }

//protected:
	
	// Return a container-less, symbol-less, clone of 'this'
	virtual CSlob* Clone();

private:
	// Makefile reading/writing support
	static const TCHAR *pcDepPrefix;

	BOOL IsDepComment(CObject*& pObject);

	CTargetItem*	m_pTarget;	// Target we depend upon
	CString m_strName;			// Name of target
	DECLARE_SLOBPROP_MAP()
};

class CProjDepInfo {
public:
	CProjectDependency *	pProjDep;
	CString					strProject;
	CString					strTarget;
};

extern CVCPtrList g_lstprojdeps;

CProjDepInfo * AddDep(CProjectDependency *pProjectDep);
BOOL RemoveDep(CProjectDependency *pProjDep);
BOOL RemoveAllReferences(CString strProj);
