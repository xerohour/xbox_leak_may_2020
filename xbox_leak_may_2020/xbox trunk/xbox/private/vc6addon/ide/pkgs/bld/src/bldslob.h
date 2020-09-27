//
// CBldSlob
//
// Build Slob
//
// [colint]
//

#ifndef _INCLUDE_BLDSLOB_H
#define _INCLUDE_BLDSLOB_H

class CBuildSlob : public CProjSlob
{
	DECLARE_SERIAL(CBuildSlob)
	typedef CProjSlob Inherited;
public:
	CBuildSlob();
	CBuildSlob(CProjItem * pItem, ConfigurationRecord * pcr);
	~CBuildSlob();

	// Content list support
	virtual CObList* GetContentList() { return (&m_Contents); }

	// Window viewing this slob
	virtual CSlobWnd* GetSlobWnd() const;

	// Paste/remove support
	virtual BOOL CanAct(ACTION_TYPE action) { return((m_pItem == NULL) ? FALSE : m_pItem->CanAct(action)); }
	virtual BOOL CanAdd ( CSlob *pAddSlob );
	virtual BOOL CanRemove(CSlob* pRemoveSlob)
		{ return m_pItem->CanRemove(((CBuildSlob *)pRemoveSlob)->GetProjItem()); }
	virtual BOOL PreAct(ACTION_TYPE action)	{ return m_pItem->PreAct(action); }

	// Property support
	virtual BOOL SetIntProp(UINT idProp, int val);
	virtual BOOL SetStrProp(UINT idProp, const CString & val);
	virtual BOOL SetLongProp(UINT idProp, long val) { return SetIntProp(idProp, val); }
	virtual	GPT GetIntProp(UINT idProp, int& val);
	virtual GPT GetStrProp(UINT idProp, CString & val);
	virtual GPT GetLongProp(UINT idProp, long& val) { int val1 = val; GPT retval = GetIntProp(idProp, val1); val = val1; return retval; }

	// Property Window Interface...
	virtual BOOL SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption = TRUE);

	// Standard Display routine
	virtual BOOL DrawGlyph(CDC *pDC, LPCRECT lpRect);

	//  Get filename associated with this slob, if any:
	virtual const CPath *GetFilePath () const;
	virtual FileRegHandle GetFileRegHandle () const;
	virtual int GetAssociatedFiles(CStringArray &);

    // Verb and command support
    virtual void GetCmdIDs(CWordArray& aVerbs, CWordArray& aCmds);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);

	virtual void OnInform(CSlob* pChangedSlob, UINT idChange, DWORD dwHint);
	CProjItem * GetProjItem() { return m_pItem; }

	// Override SetSlobProp to handle disconnecting notification
	BOOL SetSlobProp(UINT idProp, CSlob * val);

	// Our incredible move item overrides
	virtual BOOL PreMoveItem(CSlob * & pContainer, BOOL fFromBldSlob, BOOL fToBldSlob, BOOL fPrjChanged);
 	virtual BOOL MoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromBldSlob, BOOL fToBldSlob, BOOL fPrjChanged);
 	virtual BOOL PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromBldSlob, BOOL fToBldSlob, BOOL fPrjChanged);

	// Set the container of this slob, without telling anyone
	CBuildSlob * SetContainer(CSlob * pSlob);

	// Tree sorting
	virtual SORT_TYPE GetSortType() { return override_sort; }
	virtual int CompareSlob(CSlob *pCmpSlob);

	// Fixup the active config on the project, so we can do prop stuff
	// correctly
	void FixupItem();
	void UnFixupItem();

	// Get the configuration name for this item (i.e. the config of the projitem that
	// it represents)
	CString & GetConfigurationName() const { return m_pcr->GetConfigurationName(); }

	void SetMatchingConfig(LPCTSTR pszConfig, BOOL bContents = TRUE);

		// HACK ALERT, HACK ALERT, HACK ALERT
	// I couldn't come up with a clean way to do this, so here we have some state
	// variables.

	// Used to switch thunking of CBuildSlob moves through to the CProjItems. E.g.
	// if you move a CBuildSlob, and thunking is on, then the corresponding CProjItem
	// will also be moved. 
	void ThunkMoves() { m_bThunkMoves = TRUE; }
	void UnThunkMoves() { m_bThunkMoves = FALSE; }

	// Used to ignore informs. This is necessary when we are in the process of undoing
	// an action, and have moved a CProjItem back into the project, which causes a
	// bunch of notifications to be sent to CProjItems. However we want to ignore
	// some of these notifications, namely the ones sent to the CBuildSlob representing
	// the container that the CBuildSlob on the undo stack will be moved into. That is
	// we have a CProjItem, and a CBuildSlob on the undo stack, when we move the CProjItem
	// into the project, we would normally create two wrapping CBuildSlobs by the
	// notification mechanism. BUT we now only have to create one of these wrappers,
	// because the other one is on the undo stack and will be moved back into our CBuildSlob
	// hierachy shortly.
	void IgnoreInforms() { m_bIgnoreInforms = TRUE; }
	void UnIgnoreInforms() { m_bIgnoreInforms = FALSE; }
	BOOL IgnoringInforms() { return m_bIgnoreInforms; }

	// Pass informs onto a select list of CBuildSlobs. At the moment this list
	// contains CBuildSlobs that care about configuration name changes
	void InformBldSlobs(CSlob * pChangedSlob, UINT idChange, DWORD dwHint);

	// Clipboard support. Clone must make a clone of the wrapper (i.e. this), and
	// also a clone of the corresponding CProjItem. PrepareAddList does filtering
	// on the list of things to paste into this CBuildSlob, based on the contents
	// of this CBuildSlob already (i.e remove dupes, remove items already in the
	// container being pasted into).
	virtual CSlob * Clone();
	virtual void PrepareAddList(CMultiSlob* pAddList, BOOL fPasting);

	virtual void GetGlyphTipText(CString & str);

	// Drop/Drag cursor support
	virtual HCURSOR GetCursor(int ddcCursorStyle);

	virtual void	PrepareDrop(CDragNDrop *pInfo);
	virtual CSlobDragger* CreateDragger(CSlob *pDragObject, CPoint screenPoint);
	virtual void	DoDrop(CSlob *pSlob, CSlobDragger *pDragger);
	virtual void	DoDrag(CSlob *pSlob, CSlobDragger *pDragger, CPoint screenPoint);

	// Verb Support

	// Context sensitive build support
	// This has to be public, since the CBuildViewSlob will thunk to it
	afx_msg void OnClean();
	afx_msg void OnUpdateClean(CCmdUI * pCmdUI);
	afx_msg void OnBuild();
	afx_msg void OnBuildNoDeps();
	afx_msg void OnUpdateBuild(CCmdUI * pCmdUI);
	afx_msg void OnAddGroup();
	afx_msg void OnUpdateAddGroup(CCmdUI * pCmdUI);
	afx_msg void OnPopupFilesIntoProject();
	afx_msg void OnUpdateFilesIntoProject(CCmdUI *pCmdUI);

	void FlattenContents(CObList & ol, BOOL bItemsOnly = TRUE);
protected:
    //{{AFX_MSG(CBuildSlob)
    afx_msg void OnOpenItem();
	afx_msg void OnCompileItem();
    afx_msg void OnUpdateOpenItem(CCmdUI * pCmdUI);
	afx_msg void OnUpdateCompileItem(CCmdUI * pCmdUI);
    //}}AFX_MSG

	// Set the item we are wrapping
	void SetItem(CProjItem * pItem) { m_pItem = pItem; }

	// Set the config we are wrapping
	void SetConfig(ConfigurationRecord * pcr) { m_pcr = pcr; }
	ConfigurationRecord * GetConfig(void) { return m_pcr; }

private:
	CObList m_Contents;			// Our contents
	CString m_strTitle;			// Our title
	CString m_strProjWinTitle;	// ???????????

	// to make slobs aware of tree-control connections
	BOOL m_bIsExpanded;
	BOOL m_bIsExpandable;

	// property containing IPkgProject interface pointer (if appropriate)
	LONG m_lIPkgProject;

	// HACK ALERT
	// State variables
	BOOL m_bIgnoreInforms;		// Ignore informs
	static BOOL m_bThunkMoves;	// Thunk moves

	// The item and config record this slob represents
	CProjItem * m_pItem;
	ConfigurationRecord * m_pcr;

	// Used to implement Fixup/UnFixup Item
	CProjTempConfigChange *m_pTempConfigChange;
	DWORD	m_dwFixupRefCount;

	// Our list of CBuildSlobs that wish to be informed about certain prop changes
	static CObList m_lstInform;

	CPtrList m_DeathRow;	// list of Slobs to be delete during a Drop
	CSlob *m_pProxyContainer;

	DECLARE_SLOBPROP_MAP();
};

class CBuildIt {
public:
	CBuildIt(BOOL bFireEvents = TRUE);
	~CBuildIt();
	static CMapStringToPtr 	m_mapConfigurationsBuilt;
	static CMapStringToPtr 	m_mapMissingEnvVars;
	static int 				m_actualErrs;
	static int 				m_warns;
	static int 				m_depth;
	static BOOL				m_bFireEvents;
};


#endif // _INCLUDE_BLDSLOB_H
