//
// CBldNode
//
// Build Node
//
// [colint]
//

#ifndef _INCLUDE_BLDNODE_H
#define _INCLUDE_BLDNODE_H

/////////////////////////////////////////////////////////////////////////////
// CBuildNode
//
// This class is the main interface to the project window system

class CBuildNode
{
public:
	CBuildNode();
	~CBuildNode();

	// Basic project window interface
	virtual const TCHAR *GetNodeName();
	virtual CSlob *CreateNode();
	virtual void SetNodeFilter();

	// Process notifications about target deletions/additions here
	void TargetNotify(BOOL bDelete, HBLDTARGET hTarget);
	void ConfigChange();

	// Make the build pane visible
	void ShowBuildPane(BOOL bSetFocus = FALSE);

    // Hold/Enable visual updates to the workspace window
    void HoldUpdates();
    void EnableUpdates();

	// Target Node manager apis, for adding/removing target nodes
	// from our list of target nodes that we maintain
	void AddNode(CSlob * pViewSlob);
	void RemoveNode(CSlob * pViewSlob);

	// Creates a new target
	CSlob * CreateNode(HBLDTARGET hTarget, HBUILDER hBld);

	// Create new wrapper slobs for a given item
	CBuildSlob * CreateSlobs(CProjItem * pItem, ConfigurationRecord * pcrBase, CBuildSlob * pParent, BOOL bClone = FALSE);

	// Get the CBuildSlob corresponding to a given target. This function
	// will create CBuildSlobs if no appropriate ones already exist
	CBuildSlob * GetTargetSlob(HBLDTARGET hTarget);

	// Remove a CBuildSlob corresponding to a given target.
	void RemoveTargetSlob(HBLDTARGET hTarget);
	
	// Removes a Buildslob from our map
	void RemoveBuildSlob(CBuildSlob * pBuildSlob);

	// Removes all nodes in the build pane belonging to project hBld
	void RemoveNodes(HBUILDER hBld);

	// Removes all nodes in the build pane
	void RemoveAllNodes();

	// Are we currently in the process of removing all the nodes in the build pane
	BOOL RemovingAllNodes() { return m_bRemovingAllNodes; }

	// Tell anyone in the build pane about changes in config names
	void InformBldSlobs(CSlob * pChangedSlob, UINT idChange, DWORD dwHint);

	void AddDefaultNodes();

    // Gets a pointer to the first target slob if one exists
    CSlob * GetFirstTarget();

	void InitBldNodeEnum() { m_posTarget = m_lstNodes.GetHeadPosition(); }
	BOOL NextBldNodeEnum(void ** pNode)
	{
		if (m_posTarget == (POSITION)NULL)
			return FALSE;
		*pNode = m_lstNodes.GetNext(m_posTarget);
		return TRUE;
	}

	HBLDTARGET GetActiveTarget() { return m_hActiveTarget; }
	
protected:
	// Create new wrappers
	CBuildSlob * CreateTargetSlob(HBLDTARGET hTarget);

private:
	CMapPtrToPtr	m_mapTargets;	// Map from hTarget to CBuildSlob
	CPtrList		m_lstNodes;		// List of CBuildViewSlobs
	POSITION		m_posTarget;

	HBLDTARGET			m_hActiveTarget;	// Active target
	
	BOOL	m_bRemovingAllNodes;	// Are we removing all the nodes
};


/////////////////////////////////////////////////////////////////////////////
// CDefBuildNode
//
// This class provides the default build pane and nodes

class CDefBuildNode : public CDefProvidedNode
{
public:
	CDefBuildNode();
	~CDefBuildNode();

	void Add(CSlob * pNewNode);

	// We have our own pane
 	virtual BOOL HasOwnPane() { return TRUE; }

	// ...a string to use as the name for the pane, and...
	const TCHAR* GetPaneName()
	{
		if (m_strPaneName.IsEmpty())
			m_strPaneName.LoadString(IDS_PANE_NAME);
		return m_strPaneName;
	}

	// ...a function which can be called to create the node,
	// which returns a pointer to a slob which will then be
	// inserted into the pane (this can be a CMultiSlob if the
	// node provider wants to insert more than one root node).
	virtual CSlob *CreateNode();

	virtual int GetPriorityIndex()
		{ return 500; }

	virtual UINT GetHelpID()
		{ return (UINT)HID_WND_FILEVIEW; }

	virtual HBITMAP GetPaneGlyph()
	{
		HRSRC hRes = ::FindResource(GetResourceHandle(), MAKEINTRESOURCE(IDB_PANEGLYPH_BITMAP), RT_BITMAP);
		return(::AfxLoadSysColorBitmap(GetResourceHandle(), hRes));
	}

private:
    CMultiSlob * m_pTargets;
	CString m_strPaneName;
};

/////////////////////////////////////////////////////////////////////////////
// CDefAddBuildNode
//
// This class provides is used to add a new node to the build pane

class CDefAddBuildNode : public CDefProvidedNode
{
public:
	CDefAddBuildNode(HBLDTARGET hTarget, HBUILDER hBld);
	~CDefAddBuildNode();

	// We have our own pane
 	virtual BOOL HasOwnPane() { return TRUE; }

	// ...a string to use as the name for the pane, and...
	const TCHAR* GetPaneName()
	{
		if (m_strPaneName.IsEmpty())
			m_strPaneName.LoadString(IDS_PANE_NAME);
		return m_strPaneName;
	}

	// ...a function which can be called to create the node,
	// which returns a pointer to a slob which will then be
	// inserted into the pane (this can be a CMultiSlob if the
	// node provider wants to insert more than one root node).
	virtual CSlob *CreateNode();

	virtual int GetPriorityIndex()
		{ return 500; }

	virtual UINT GetHelpID()
		{ return (UINT)HID_WND_FILEVIEW; }

	virtual HBITMAP GetPaneGlyph()
	{
		HRSRC hRes = ::FindResource(GetResourceHandle(), MAKEINTRESOURCE(IDB_PANEGLYPH_BITMAP), RT_BITMAP);
		return(::AfxLoadSysColorBitmap(GetResourceHandle(), hRes));
	}

protected:
	HBLDTARGET	m_hTarget;
	HBUILDER	m_hBld;
	CString		m_strPaneName;
};

#ifdef CUSTOM_BLDNODES
// Only do this if the user can add and delete bld nodes as visual 
// representations of targets

/////////////////////////////////////////////////////////////////////////////
// CBuildGeneralPage
//
// This is the class for our Target filter page.

class CBuildGeneralPage : public CSlobPage
{
	virtual void InitializePage();
	virtual BOOL Validate();

	DECLARE_DYNAMIC(CBuildGeneralPage)
	DECLARE_IDE_CONTROL_MAP()
};

extern CBuildGeneralPage g_BuildGeneralPage;

#endif

/////////////////////////////////////////////////////////////////////////////
// Helper function for getting a pointer to the one and only build node
CBuildNode* GetBuildNode();
extern CBuildNode* g_pBuildNode;

#endif // _INCLUDE_BLDNODE_H
