//
//	CTargetItem
//
//	This is a projitem that represents a target, that is something
//	which is buildable, e.g. an exe, dll, or lib.
//
//	[colint]
//				   

#ifndef _TARGETITEM_H										
#define _TARGETITEM_H

///////////////////////////////////////////////////////////////////////////////
class CTargetItem : public CProjContainer 
{
//
//	Represents a target. It is the same as a CProjContainer
//	except that conceptually it can have operations applied
//	to it that are normally only applied to the CProject,
//	e.g. build.
//
	DECLARE_SERIAL (CTargetItem)

protected:

public:
	CTargetItem ();
	virtual ~CTargetItem ();

	// The CanAdd method tells us what it makes sense for
	// a user to be able to add to this slob container.
	// So for a CTarget a user can add CProjGroup's, and CFileItem's but
	// nothing else. NOTE: This is different to our
	// CanContain method, which tells us what we can
	// add from an internal (within VPROJ code) viewpoint.
	virtual BOOL CanAdd ( CSlob * pSlob );	

	// Can we do clipboard operations?
	// FUTURE (colint): These are disabled for the
	// moment
	virtual BOOL CanAct(ACTION_TYPE action);

	virtual	GPT GetIntProp(UINT idProp, int& val);
	virtual GPT GetStrProp(UINT idProp, CString &str);
	virtual BOOL SetStrProp(UINT idProp,  const CString &str);

	virtual BOOL SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption = TRUE);

	// allowing/disallowing global registry access to this target registry
	virtual BOOL PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);

	// .MAK reading and writing
	virtual BOOL WriteToMakeFile(CMakFileWriter& pw, CMakFileWriter& mw, const CDir &BaseDir, CNameMunger& nm);
	virtual BOOL ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir);

	// FUTURE: remove these
	//  Get list of FileRegHandles that for targets file of this item:
	virtual CFileRegSet * GetTargetPaths();

	//  Get list of FileRegHandles that for output file of this item:
	//  Identical to the target list for source items, but proj/schmooze items
	// can have 'extra' outputs that aren't targets (.map, .ilk files, etc.)
	virtual CFileRegSet * GetOutputPaths();
	// FUTURE: remove these

	// External target HACK for writing makefiles
	BOOL WriteExternalTargetBuildRule(CMakFileWriter& mw, const CDir &BaseDir);

	//	Our internal CanAdd method - tells us what WE can add
	//	rather than what the user can add
	//	So for CTarget we can add both CProjGroup's and CFileItems and
	//	CDependencyContainer's. However this is different
	//	to what CanAdd says we can add - CanAdd only allows
	//	CProjGroup's and CFileItems, since a user can never explicitly add
	//	a dependency container (we always do it when it is
	//	necessary!).
	virtual BOOL CanContain(CSlob* pSlob);

	virtual void FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem);
	
 	// Get the file registry for this project.
	virtual CFileRegistry* GetRegistry() { return &m_TargetRegistry; }

	// Override of default CSlob function to test for duplicate filenames,
	// multiple .rc/.def files, etc. when adding/pasting to a group
	virtual void PrepareAddList(CMultiSlob* pAddList, BOOL bPasting);

    // Handle notifications for addition/deletion of files from us    
    void AddToNotifyList(FileRegHandle frh, BOOL bRemove, BOOL fDepFile = FALSE);

	// Send out notifications about addition/deletion of files
	void EmptyNotifyLists();

	// UNDONE : colint, Not yet - maybe in the future
	// Drop and Drag support
	//virtual void PrepareDrop(CDragNDrop *pInfo);
	//virtual CSlobDragger* CreateDragger(CSlob *pDragObject, CPoint screenPoint);
	//virtual void	DoDrop(CSlob *pSlob, CSlobDragger *pDragger);
	//virtual void	DoDrag(CSlob *pSlob, CSlobDragger *pDragger, CPoint screenPoint);

	__inline CString GetTargDir() { return m_strTargDir; }
	__inline void SetTargDir(CString &strTargDir) { m_strTargDir = strTargDir; }
	__inline const CString & GetTargetName() { return m_strTargetName; }
	__inline void SetTargetName(const CString & str) { m_strTargetName = str; }

    BOOL IsTargetReachable(CTargetItem * pTarget);
	BOOL ContainsTarget(CTargetItem * pTarget);
	BOOL IsProjectFile(LPCTSTR pszFile);

	BOOL IsFileInTarget( FileRegHandle frh, BOOL bAsFileItem = FALSE );

private:
	// Makefile reading/writing support
	static const TCHAR *pcNamePrefix;

	BOOL IsNameComment(CObject*& pObject);
	void WriteTargetNames(CMakFileWriter& mw);


	// Properties
    CString m_strProjClsWzdName;            // The classwizard file name
	CFileRegistry m_TargetRegistry;			// The target registry

	CString m_strTargetName;
	CString m_strTargDir;                   // save per target directory

	// Add/Delete file notification lists
    NOTIFYINFO  m_AddNotifyInfo;
    NOTIFYINFO  m_DelNotifyInfo;

	DECLARE_SLOBPROP_MAP()
};

#endif // _TARGETITEM_H
