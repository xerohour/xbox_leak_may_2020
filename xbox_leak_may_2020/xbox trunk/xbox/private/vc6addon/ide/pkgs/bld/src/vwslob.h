//
// CBuildViewSlob
//
// Build View Slob
//
// [colint]
//

#ifndef _INCLUDE_BLDVWSLOB_H
#define _INCLUDE_BLDVWSLOB_H

/////////////////////////////////////////////////////////////////////////////
// CBuildViewSlob
//
// This class is used to represent the top level target nodes in the project
// window

class CBuildViewSlob : public CProjSlob
{
	DECLARE_SERIAL(CBuildViewSlob)

public:
	CBuildViewSlob();
	CBuildViewSlob(HBLDTARGET hTarget, HBUILDER hBld);
	~CBuildViewSlob();

	// Sets the current target represented by this slob
	void SetFilterTarget(HBLDTARGET hTarget);

	// Paste/remove support
	virtual BOOL CanAdd ( CSlob *pAddSlob );
 	virtual BOOL CanRemove(CSlob* pRemoveSlob) { ASSERT(m_pTargetSlob); return m_pTargetSlob->CanRemove(pRemoveSlob); }
	virtual BOOL CanAct(ACTION_TYPE action);

	// Paste support - filter list of things to paste here
	virtual void PrepareAddList(CMultiSlob* pAddList, BOOL fPasting);

	// We fake our content list to be that of the corresponding CBuildSlob
	virtual CObList * GetContentList() { ASSERT(m_pTargetSlob != NULL); return m_pTargetSlob->GetContentList(); }

	// Property Window Interface
	virtual BOOL SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption = TRUE);

	// Standard Display routine
	virtual BOOL DrawGlyph(CDC *pDC, LPCRECT lpRect) { return (m_pTargetSlob != NULL) ? m_pTargetSlob->DrawGlyph(pDC, lpRect) : FALSE; }
	virtual void GetGlyphTipText(CString & str);

	// Get the target we represent
	HBLDTARGET GetTarget() { return m_hTarget; }
	HBUILDER GetBuilder() { return m_hBld; }

	// Notification handling
	virtual void OnInform(CSlob* pChangedSlob, UINT idChange, DWORD dwHint);

	// Get our corresponding CBuildSlob
	CBuildSlob * GetBuildSlob() { return m_pTargetSlob; }

	//  Get filename associated with this slob, if any:
	virtual const CPath *GetFilePath () const;
	virtual FileRegHandle GetFileRegHandle () const;

	// Property support
    virtual BOOL SetIntProp(UINT idProp, int val);
    virtual BOOL SetStrProp(UINT idProp, const CString & val);
    virtual GPT GetIntProp(UINT idProp, int & val);
    virtual GPT GetStrProp(UINT idProp, CString & val);

	// Override SetSlobProp to handle disconnecting notification
	BOOL SetSlobProp(UINT idProp, CSlob * val);

    // Serialization of the target nodes
	virtual void Serialize(CArchive & ar);

	// Drag and drop support
	virtual void PrepareDrop(CDragNDrop *pInfo);
	virtual CSlobDragger* CreateDragger(CSlob *pDragObject, CPoint screenPoint);
	virtual void DoDrop(CSlob *pSlob, CSlobDragger *pDragger);
	virtual void DoDrag(CSlob *pSlob, CSlobDragger *pDragger, CPoint screenPoint);

	// Get the commands and verbs supported by this top level target node
    virtual void GetCmdIDs(CWordArray& aVerbs, CWordArray& aCmds); 
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);

// jayk
// in the past, the files associated with a project were just the sources
// now, working on source control, I'm making the .bld file associated too.
// This could very conceivably break other things, but it is the right approach
// for what I am trying to do -- which is for source control to treat .bld files
// better -- be more inclined to preselect them for check in and add to scc.
//
// The GetAssociatedFiles that we inherit from CProjSlob by default returns
// the associated files of the contained slobs, but it doesn't consider that
// non-leaf nodes in a slob tree might have associated files other than
// their children's files. Perhaps this idea should be pushed up into CProjSlob,
// but for now I'll just put it here.
	virtual int GetAssociatedFiles(CStringArray& files);


protected:
	//{{AFX_MSG(CBuildViewSlob)
	afx_msg void OnBuild();
	afx_msg void OnBuildNoDeps();
  	afx_msg void OnClean();
  	afx_msg void OnAddGroup();
	afx_msg void OnUpdateAddGroup(CCmdUI * pCmdUI);
	afx_msg void OnSetDefault();
	afx_msg void OnUnloadProject();
	afx_msg void OnPopupFilesIntoProject();
	afx_msg void OnUpdateFilesIntoProject(CCmdUI *pCmdUI);
#ifdef VB_MAKEFILES
  	afx_msg void OnLaunchVB();
#endif
	//}}AFX_MSG

public:
	UINT GetExpandedState(BOOL fIgnoreDeps = FALSE);
private:
	CString m_strTitle;			// Our title
	CString m_strProjWinTitle;	// ?????????
	CString m_strConfigName;	// Config name property. This is used to record renames of config names.

	BOOL m_bIsInvisibleNode;	// invisible node?
	BOOL m_bIsExpanded;			// are we expanded?

	// property containing IPkgProject interface pointer (if appropriate)
	LONG m_lIPkgProject;

	HBLDTARGET	m_hTarget;			// The target we represent
	HBUILDER	m_hBld;				// The project we represent
	CString m_strTarget;
	CBuildSlob * m_pTargetSlob;	// Our corresponding CBuildSlob

	DECLARE_SLOBPROP_MAP();
};

#endif // _INCLUDE_BLDVWSLOB_H
