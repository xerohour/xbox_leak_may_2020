#ifndef __SLOB_H__
#define __SLOB_H__

#include "shlmenu.h"

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

class CSlobDragger;
class CSlobWnd;
class CSlobSequence;
class CPropBag;
class C3dPropertyPage;

class CPath;
class CFileRegEntry;
typedef CFileRegEntry * FileRegHandle;

// Slobs (Selection Objects) provide a basis for CEditor and CSelection
// that includes a standard property access mechanism with change
// notifications.
//
// Using this standard property mechanism, editors are isolated from
// the user interface of the property sheet.
//
// By making CSelection derive from CSlob and having it mirror the
// properties of all selected objects as well as broadcast any property
// changes it receives to all of the selected objects, handling setting
// properties on multiple selections becomes much easier.


// Special property identifiers
//
#define P_BEGIN_MAP		             0	// Pseudo-prop for marking the start of a map
#define P_END_MAP		             1   // Pseudo-prop for marking the end of a map
#define P_Container		             2   // Container property supported by all slobs
#define P_ID                         3
#define P_Value                      4
#define P_QuoteName                  5

#define P_Title     	             6
#define P_FileName                   7
#define P_FileDisplay                8

#define P_CurrentSlob	             9
#define P_PoppingProperties         10
#define P_CurrentSlobRect			11

// Properties for workspace window folder slobs
#define P_IsExpanded		        32
#define P_IsExpandable		        33
#define P_IsInvisibleNode           34
#define P_HasGlyphTipText	        35
#define P_BoldWorkspaceText	        36
#define P_ProjWinTitle		        37
#define P_ProxyContainer	        38
#define P_SccStatus			        39
#define P_PanePriorityIndex         40
#define P_PaneIsIndependent         41
#define P_SortPaneRootNodes         42
#define P_FileSaveCallBack          43
#define P_PaneHelpIdentifier        44
#define P_IPkgProject               45

// Standard Slob Notifications
#define SN_DESTROY		             0   // Dependancy has been destroyed
#define SN_ALL			             1   // All properties of dependancy (may) have changed
#define SN_CONTENT		            64	 // The CSlob content has changed
#define SN_ADD                      65   // A CSlob was added to the content list
#define SN_REMOVE                   66   // A CSlob was removed from the content list
#define SN_CONTENT_SCC_UPDATE       67	 // The CSlob content has changed

// Actions
#define A_PostSel                  100
#define A_PreSel                   101

// Commands and Verbs
#define C_COMMAND                  126
#define V_VERB                     127

#define P_MinPackage               128	// Minimum safe package property ID.

enum PROP_TYPE { null, action, integer, booln, longint, number, string, 
				slob, rect, point, verb, command };

// Property Descriptor
struct PRD
{
	UINT nProp;
	const void* pbOffset;
	// Note correspondence with CUndoSlob
	enum PROP_TYPE { null, action, integer, booln, longint, number, string, 
		slob, rect, point, verb, command } nType;
	LPCTSTR szName;
};


#define BEGIN_SLOBPROP_MAP(thisClass, parentClass) \
	PRD AFX_EXT_DATADEF thisClass::m_rgprd [] = { \
		{ P_BEGIN_MAP, &parentClass::m_rgprd[0], PRD::null, #thisClass },

#define END_SLOBPROP_MAP() \
	{ P_END_MAP, NULL, PRD::null, NULL } };

#define ACTION_PROP(name, memberFxn) \
	{ P_##name, &((theClass*)0)->memberFxn, PRD::action, #name },

#define BOOL_PROP(name) \
	{ P_##name, &((theClass*)0)->m_b##name, PRD::booln, #name },

#define FAKE_BOOL_PROP(name) \
	{ P_##name, NULL, PRD::booln, #name },

#define INT_PROP(name) \
	{ P_##name, &((theClass*)0)->m_n##name, PRD::integer, #name },

#define FAKE_INT_PROP(name) \
	{ P_##name, NULL, PRD::integer, #name },

#define LONG_PROP(name) \
	{ P_##name, &((theClass*)0)->m_l##name, PRD::longint, #name },

#define FAKE_LONG_PROP(name) \
	{ P_##name, NULL, PRD::longint, #name },

#define DBL_PROP(name) \
	{ P_##name, &((theClass*)0)->m_num##name, PRD::number, #name },

#define STR_PROP(name) \
	{ P_##name, &((theClass*)0)->m_str##name, PRD::string, #name },

#define FAKE_STR_PROP(name) \
	{ P_##name, NULL, PRD::string, #name },

#define RECT_PROP(name) \
	{ P_##name, &((theClass*)0)->m_rc##name, PRD::rect, #name },

#define POINT_PROP(name) \
	{ P_##name, &((theClass*)0)->m_pt##name, PRD::point, #name },

#define FAKE_POINT_PROP(name) \
	{ P_##name, NULL, PRD::point, #name },

#define SLOB_PROP(name) \
	{ P_##name, &((theClass*)0)->m_p##name, PRD::slob, #name },

#define VERB_PROP(verbID) \
	{ V_VERB, (const void*)(0x0000ffff & verbID), PRD::verb, _T("Verb") },

#define COMMAND_PROP(cmdID) \
	{ C_COMMAND, (const void*)(0x0000ffff & cmdID), PRD::command, _T("Command") },

#define DECLARE_SLOBPROP_MAP() \
	public: \
		static PRD AFX_DATA m_rgprd []; \
		virtual const PRD FAR* GetPropMap() const \
		{ return &m_rgprd[0]; }


// Get*Prop return type
//	In a multiple-selection, a lower code has precedence over a higher code.
//
enum GPT
{
	avoid = -1,		// Like invisible, but is overriding in multiple selections
	invalid = 0,	// Not a known property or disabled
	ambiguous,		// Multiple selection with different values
	valid,			// Value is correct
	invisible,		// Hide this property's UI on prop pages
};

//
//	Schema version for slob implemenetation.  Used in IMPLEMENT_SERIAL:
#define SLOB_SCHEMA_VERSION	1

//	Find a propery in a propery map:
const PRD* PrdFindProp(const PRD* propMap, UINT idProp);

enum SORT_TYPE { no_sort, alpha_sort, override_sort };

// act_insert_into_undo_slob: when the undo mechanism records the
// post-action selection it will only remember those slobs who return
// TRUE for this action type.

enum ACTION_TYPE { act_delete, act_cut, act_copy, act_paste, act_drag, act_drop, act_insert_into_undo_slob, act_rename};

#define S_ON_COMMAND(n, f) case (n): (f)(); return TRUE;
#define S_ON_UPDATE_COMMAND_UI(n, f) case (n): (f)(pCmdUI); return TRUE;

// Object of _many_ uses
//
class CSlob : public CObject
{
	DECLARE_SERIAL(CSlob)

public:
	CSlob();

	virtual ~CSlob();

	// Return a container-less, symbol-less, clone of 'this'
	virtual CSlob* Clone();

	// Copy properties only for 'this', doesn't copy properties
	// of contained slobs, both map and bag properties are copied
	virtual void CopyProps(CSlob *);

	// Clone an existing property bag into 'this' property bag
	virtual void ClonePropBag(CPropBag * pBag, int nBag = -1, BOOL fEmpty = TRUE);

	// Standard MFC serialization:
	virtual void Serialize(CArchive& ar);

	void SerializePropMap (CArchive& ar);
	virtual BOOL SerializePropMapFilter (int nIdProp) {return TRUE;}

	// Dependant Management
	void AddDependant(CSlob* newDependant);
	void RemoveDependant(CSlob* oldDepentant);
	virtual void InformDependants(UINT idChange, DWORD dwHint = 0);

	// Container Access
	inline CSlob* GetContainer() const
			{ return m_pContainer; }
	CSlob *GetRootContainer();

	// Move 'this' into pNewContainer.
	inline BOOL MoveInto(CSlob* pNewContainer)
			{ return SetSlobProp(P_Container, pNewContainer); }
	
	// Command enabling
	virtual BOOL CanAct(ACTION_TYPE action) { return TRUE; }
	// NYI: A final chance to refuse before the action
	virtual BOOL PreAct(ACTION_TYPE action) { return TRUE; }

#ifdef _DEBUG
	// Intentionally changed from BOOL to void returning functions to cause
	// compile errors in any code that still uses these.  You need to override
	// CanAct instead.

	virtual void CanDelete() {}
	virtual void CanCut() {}
	virtual void CanCopy() {}
	virtual void CanPaste() {}
	virtual void CanDrag() {}
#endif

	// Content Access (only valid for compound slobs!)
	virtual CObList* GetContentList();
	inline POSITION GetHeadPosition()
				{ CObList* p = GetContentList();
				  ASSERT(p != NULL);
				  return p->GetHeadPosition(); }
	inline CSlob* GetNext(POSITION& pos)
				{ CObList* p = GetContentList();
				  ASSERT(p != NULL);
				  return (CSlob*)p->GetNext(pos); }

	virtual void PrepareAddList(class CMultiSlob* pAddList, BOOL fPasting);

	virtual void Add(CSlob* pSlob);
	virtual void Remove(CSlob* pSlob);

	virtual BOOL CanAdd(CSlob* pAddSlob);
	virtual BOOL CanRemove(CSlob* pRemoveSlob);

#ifdef SLOB_REFERENCING
	// routines for creating a 'short-cut' to a slob
	// REVIEW: [patbr] the default 'reference' to a slob is just the slob itself
	virtual void CanReference() { return TRUE; }
	virtual CSlob *Reference()					// create a 'short-cut' to the slob
		{ return(this); }
	virtual void RefSerialize(CArchive& ar)		// serialize the 'reference'
		{ Serialize(ar); }
#endif

	// Notification Callback
	virtual void OnInform(CSlob* pChangedSlob, UINT idChange, DWORD dwHint);

	// Property Management
	virtual BOOL SetDoubleProp(UINT idProp, double val);
	virtual BOOL SetIntProp(UINT idProp, int val);
	virtual BOOL SetLongProp(UINT idProp, long val);
	virtual BOOL SetStrProp(UINT idProp, const CString& val);
	virtual BOOL SetSlobProp(UINT idProp, CSlob* val);
	virtual BOOL SetRectProp(UINT idProp, const CRect& val);
	virtual BOOL SetPointProp(UINT idProp, const CPoint& val);
	virtual GPT GetDoubleProp(UINT idProp, double& val);
	virtual	GPT GetIntProp(UINT idProp, int& val);
	virtual	GPT GetLongProp(UINT idProp, long& val);
	virtual GPT GetStrProp(UINT idProp, CString& val);
	virtual GPT GetSlobProp(UINT idProp, CSlob*& val);
	virtual GPT GetRectProp(UINT idProp, CRect& val);
	virtual GPT GetPointProp(UINT idProp, CPoint& val);

	// Property Bag Management
	virtual int GetPropBagCount() const;
	virtual CPropBag* GetPropBag(int nBag = -1);

	// Custom undo support hooks
	virtual void UndoAction(CSlobSequence* pSeq, UINT idAction);
	virtual void DeleteUndoAction(CSlobSequence* pSeq, UINT idAction);
	virtual void BeginUndo(UINT nID);
	virtual void EndUndo(BOOL bAbort = FALSE);

	// Return window viewing this slob if any
	virtual CSlobWnd* GetSlobWnd() const;

	// Property Window Interface...
	virtual BOOL SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption = TRUE);

	// Drag and Drop
	static  CSlob*	DropTargetFromPoint (CPoint screenPoint);
	virtual int GetCursorIndex();

#define DDC_SINGLE		0
#define DDC_COPY_SINGLE	1
#define DDC_MULTI		2
#define DDC_COPY_MULTI	3

	virtual HCURSOR GetCursor(int ddcCursorStyle);

	virtual void PrepareDrop(class CDragNDrop* pInfo);

	virtual CSlobDragger* CreateDragger(CSlob *pDragObject, 
		CPoint screenPoint);
	virtual void DoDrop(CSlob* pSlob, CSlobDragger* pDragger);
	virtual void DoDrag(CSlob* pSlob, CSlobDragger* pDragger, 
		CPoint screenPoint);

#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	virtual void GetSlobName(CString& strName) const;
#endif

	//  Get filename associated with this slob, if any:
	virtual const CPath *GetFilePath () const { return NULL; } 
	virtual FileRegHandle GetFileRegHandle () const { return NULL; }
	virtual int GetAssociatedFiles(CStringArray &);

protected:

	// CSlob data size is currently 16 bytes.

	// WARNING!!  Think twice before adding data to CSlob.  We use many,
	// many, many of these in the product.

	CSlob* m_pContainer;

	// NOTE:  Because of 4 byte alignment we have 3 unused bytes here,
	// that we might want to use in the future, perhaps we could add
	// a style word...

	// Dependencies optimized for the 99% case, 2 or fewer dependants.
	// We keep 2 dependencies on the CSlob, and if nDep gets bigger than 2,
	// then all dependencies are moved into a CObList, and assign it
	// to m_aDep[0].

	BYTE m_cDep;
	CObject* m_aDep[2];

public:		// properties

	DECLARE_SLOBPROP_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CProjSlob -- root project slob class 
// REVIEW(scotg)  if you move any functions from CSlob into this class,  you
// will need to verify that ALL classes that inherit from CProjSlob that provide
// and implementation of the function you just moved, call the correct base class
// member function.  Since a large number of classes already inherited from CSlob
// before we introduced CProjSlob, these classes call CSlob member functions when 
// looking for their base class implementation and not CProjSlob.  Only the member
// functions moved into CProjSlob have had their base class calls change to 
// call CProjSlob implementations and not CSlob implementations.

class CProjSlob : public CSlob
{
	DECLARE_DYNAMIC(CProjSlob)

public:
	CProjSlob();
	virtual ~CProjSlob();


	// allow slobs to override text displayed when mouse pauses on glyph.
	// only called if P_HasGlyphTipText property is set for the slob.
	virtual void GetGlyphTipText(CString &strTip);

	// routines related to visually sorting the slob's contents
	// REVIEW: [patbr] are default behaviors here appropriate?
	virtual SORT_TYPE GetSortType()				// get type of sorting slob desires on contents
		{ return(alpha_sort); }
	virtual int CompareSlob(CSlob *pCmpSlob);	// returns -1, 0, 1 for <, ==, >

	// Cmds that can be handled by this slob
	virtual void GetCmdIDs(CWordArray& aVerbs, CWordArray& aCmds);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo) { return FALSE; }	// Override to handle commands

	// Standard Display routine
	virtual BOOL DrawGlyph(CDC* pDC, LPCRECT lpRect);

	virtual HRESULT GetService(REFGUID guidService, REFIID riid, void ** ppvObj)
		{ *ppvObj = NULL; return E_NOTIMPL;}

};


/////////////////////////////////////////////////////////////////////////////
// CMultiSlob -- selection manager

class CMultiSlob : public CSlob
{
	DECLARE_DYNCREATE(CMultiSlob)

public:
	CMultiSlob();
	virtual	~CMultiSlob();

// Selection manipulation

	void Single(CSlob*, BOOL bSolo = FALSE);  // replace selection
	virtual void Clear(); // clear selection, remove all items
	virtual void Add(CSlob* pSlob);
	virtual void Remove(CSlob* pSlob);
	virtual void EnsureValidSelection();
	
// Select information

	BOOL IsSelected(CSlob* pSlob) const;
	BOOL IsOnlySelected(CSlob* pSlob) const;
	BOOL IsDominantSelected(CSlob* pSlob) const;
	CSlob* GetDominant() const;
	
// Slob Overrides
	
	virtual CObList* GetContentList();
	virtual CSlobWnd* GetSlobWnd() const;
	virtual int GetCursorIndex();
	virtual HCURSOR GetCursor(int ddcCursorStyle);
	virtual BOOL CanAdd(CSlob* pAddSlob);
	virtual BOOL CanAct(ACTION_TYPE action);
	virtual BOOL PreAct(ACTION_TYPE action);
	virtual BOOL Delete(BOOL bClip = FALSE, CSlob** ppNextSlob = NULL);
	virtual void UndoAction(CSlobSequence* pSeq, UINT idAction);
	virtual void OnInform(CSlob* pChangedSlob, UINT idChange, DWORD dwHint);

	virtual BOOL SetDoubleProp(UINT idProp, double val);
	virtual BOOL SetIntProp(UINT idProp, int val);
	virtual BOOL SetLongProp(UINT idProp, long val);
	virtual BOOL SetStrProp(UINT idProp, const CString& val);
	virtual BOOL SetSlobProp(UINT idProp, CSlob* val);
	virtual BOOL SetRectProp(UINT idProp, const CRect& val);
	virtual BOOL SetPointProp(UINT idProp, const CPoint& val);
	
	virtual GPT GetDoubleProp(UINT idProp, double& valReturn);
	virtual GPT GetIntProp(UINT idProp, int& valReturn);
	virtual GPT GetLongProp(UINT idProp, long& val);
	virtual GPT GetStrProp(UINT idProp, CString& valReturn);
	virtual GPT GetSlobProp(UINT idProp, CSlob*& val);
	virtual GPT GetRectProp(UINT idProp, CRect& val);
	virtual GPT GetPointProp(UINT idProp, CPoint& val);

	// Property Window Interface...
	virtual BOOL SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption = TRUE);

	CSlobWnd*	m_pSlobWnd;// the owner of this selection
	CObList m_objects;	// list of selected objects
	BOOL m_bSolo:1; // can the selected obj be part of a multiple selection?
	BOOL m_bAllowEmpty:1; // the this selection be empty?
	BOOL m_bAllowMultipleContainers:1; // can selection cross containers?
};


/////////////////////////////////////////////////////////////////////////////
// CSlobDragger -- base class for objects used to drag slobs around

class CSlobDragger : public CObject
{
public:
	virtual void Show(BOOL bShow = TRUE) = 0;
	void Hide()
		{ Show(FALSE); }
	virtual CRect GetRect() = 0;
	virtual void Move(const CRect& newRect, BOOL bForceShow = FALSE) = 0;
};

class CSlobDraggerEx : public CSlobDragger
{
public:
	virtual void AddItem(CSlob* pSlob) = 0;
};

// Initiate a drag of "dragSlob" with the mouse at "downPoint".  This will 
// use a default CDragNDrop object to handle the operation.  For a custom 
// handler, use the other version of this function (e.g. VRes must use a 
// CResDragNDrop object).
void BeginDragDrop(CPoint downPoint, CSlob* dragSlob);

// Initiate a drag using a custom CDragNDrop class.
void BeginDragDrop(CDragNDrop* pInfo);

// Cancel any pending drag operation.
void CancelDragDrop();

// Returns TRUE if a drag operation is in progress, FALSE otherwise.
BOOL IsDragDropInProgress();


////////////////////////////////////////////////////////////////////////////
// CDragNDrop

class CDragNDrop
{

public:
  	CDragNDrop(CPoint downPoint, CSlob *dragSlob);
	virtual	~CDragNDrop();

	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void AbortDrop();
	virtual void IdleUpdate();
	virtual void FixupSelections(CMultiSlob* pSource, CMultiSlob* pDest);
	virtual void ChangeCursor();
	virtual BOOL IsCopy();
	virtual void Drop(UINT nFlags, CPoint point);

	CSlob* m_dragSlob; // the slob being dragged
	CSlobDragger* m_dragInfo; // used to gather information about the drag and drop
	CSlob* m_target; // the last potential drop target
	WORD m_wLastKeyState; // last state of keys (DD_CTRL / DD_SHIFT)

	CPoint m_downPoint;
	CPoint m_lastPoint;

	UINT	m_flags;
	CPoint	m_point;

	// User's selection in right-button drop popup menu.  NotSet if this
	// hasn't been used.
	enum DragMoveCopy { dragNotSet, dragMove, dragCopy, dragCancel } m_MoveCopy;
};

// Flags for the m_wLastKeyState member of CDragNDrop
#define DD_CTRL		0x0001
#define DD_SHIFT	0x0002


// Initiate a drag of "dragSlob" with the mouse at "downPoint".  This will 
// use a default CDragNDrop object to handle the operation.  For a custom 
// handler, use the other version of this function (e.g. VRes must use a 
// CResDragNDrop object).
void BeginDragDrop(CPoint downPoint, CSlob* dragSlob);

// Initiate a drag using a custom CDragNDrop class.
void BeginDragDrop(CDragNDrop* pInfo);

// Cancel any pending drag operation.
void CancelDragDrop();

// Returns TRUE if a drag operation is in progress, FALSE otherwise.
BOOL IsDragDropInProgress();


/////////////////////////////////////////////////////////////////////////////
//	Properties

class CProp
{
public:
	static void DeleteCProp (CProp *pPRop);
	PROP_TYPE m_nType;
};

class CIntProp : public CProp
{
public:
	CIntProp(int val = 0)
		{ m_nVal = val; m_nType = integer; }

	int m_nVal;
};

class CBoolProp : public CProp
{
public:
	CBoolProp(int val = FALSE)
		{ m_bVal = val; m_nType = booln; }

	int m_bVal;
};

class CNumberProp : public CProp
{
public:
	CNumberProp(double val = 0.)
		{ m_numVal = val; m_nType = number; }

	double m_numVal;
};

class CStringProp : public CProp
{
public:
	CStringProp(const char* szVal = "")
		{ m_strVal = szVal; m_nType = string; }

	CString m_strVal;
};

inline void CProp::DeleteCProp (CProp *pPRop)
{
	switch (pPRop->m_nType)
	{
		case integer:
			delete ( (CIntProp *) pPRop );
			break;
		case booln:
			delete ( (CBoolProp *) pPRop );
			break;
		case number:
			delete ( (CNumberProp *) pPRop );
			break;
		case string:
			delete ( (CStringProp *) pPRop );
			break;
		default:
			ASSERT(FALSE);		 		
	}
}

class CPropBag
{
	friend class CSlob;

public:
	CPropBag () : m_props(11) {};		// Use small block size.
	CPropBag (UINT nHash) : m_props(11) {m_props.InitHashTable(nHash, FALSE);}
	~CPropBag () {Empty();}

	// Enumerate props -- same as CMapWordToPtr's methods
	__inline POSITION GetStartPosition() const
		{return m_props.GetStartPosition();}

	__inline void GetNextAssoc(POSITION & pos, UINT & nPropID, CProp * & pProp) const
		{nPropID = (UINT)0; m_props.GetNextAssoc(pos, (WORD &)nPropID, (void * &)pProp);}

	__inline int GetPropCount () const
		{return m_props.GetCount();}

	// Setting of prop values.
	void SetIntProp(CSlob* pSlob, UINT nPropID, int val);
	void SetStrProp(CSlob* pSlob, UINT nPropID, CString & str);

	// TODO: add SetProp functions for other types...
	__inline CProp * FindProp(UINT nPropID) const
		{CProp * pProp; // ASSERT(HIWORD(nPropID)==0); // PropIDs are 16 bits only!
			return m_props.Lookup((WORD)nPropID, (void*&)pProp) ? pProp : (CProp *)NULL;}
	__inline void AddProp(UINT nPropID, CProp* pProp)
		{// ASSERT(HIWORD(nPropID)==0); // PropIDs are 16 bits only!
			m_props[(WORD)nPropID] = pProp;}
	void RemoveProp(UINT nPropID);
	void RemovePropAndInform(UINT nPropID, CSlob * pSlob);

	void Empty();
	void Clone(CSlob * pSlob, CPropBag * pBag, BOOL fEmpty = TRUE);

	// Serialize the props in this bag.  If the slob pointer is provided
	// the slob's serialize SerializePropMapFilter filter is used:
	void Serialize(CArchive & ar, CSlob *pFilterSlob = NULL);

protected:
	CMapWordToPtr m_props;
};

__inline void CPropBag::RemovePropAndInform(UINT nPropID, CSlob * pSlob)
{
	CProp * pProp;
	// ASSERT(HIWORD(nPropID)==0); // PropIDs are 16 bits only!
	if (m_props.Lookup((WORD)nPropID, (void*&)pProp))
	{
		CProp::DeleteCProp(pProp);
		VERIFY(m_props.RemoveKey((WORD)nPropID));
		pSlob->InformDependants((WORD)nPropID);
	}
}

__inline void CPropBag::RemoveProp(UINT nPropID)
{
	// only do removal if we find it
	// N.B. we don't VERIFY this op. 'cos it is more efficient
	// for clients of this class to call
	//
	// 	pBag->RemoveProp(idProp);
	//
	// rather than
	//
	//  if (pBag->FindProp(idProp))
	// 		pBag->RemoveProp(idProp));
	//
	// , for the case where the client doesn't care about the prop
	// value, in order to prevent the VERIFY() firing
	CProp * pProp;
	// ASSERT(HIWORD(nPropID)==0); // PropIDs are 16 bits only!
	if (m_props.Lookup((WORD)nPropID, (void*&)pProp))
	{
		CProp::DeleteCProp(pProp);
		VERIFY(m_props.RemoveKey((WORD)nPropID));
	}
}


/////////////////////////////////////////////////////////////////////////////
// CClipboardSlob

class CClipboardSlob : public CSlob
{
  	DECLARE_DYNAMIC(CClipboardSlob)

public:
	CClipboardSlob();
	~CClipboardSlob();

	void Copy(CSlob *pCopySlob);
	void Paste(CSlob *pDestSlob, CMultiSlob* pSelection = NULL, BOOL bClearSelection = TRUE);
	void ClearContents();
	
	BOOL CanAct(ACTION_TYPE action);
	
	virtual CObList* GetContentList()
		{ return &m_objects; }

	CObList m_objects;
};

extern CClipboardSlob AFX_DATA theClipboardSlob;


/////////////////////////////////////////////////////////////////////////////
//  Undo slob
//		REVIEW:  This is much more than really needs to be exposed to the
//               package developer.  Using Find in Files, I found only
//               some 12 simple global functions necessary to support our
//               current usage.

class CUndoStack;           // A stack of undo/redo-able actions
// CByteArray
	class CSlobSequence;    // A sequence of changes for one action
class CUndoRecord;          // One change in a sequence
	class CIntUndoRecord;
	class CLongUndoRecord;
	class CDoubleUndoRecord;
	class CRectUndoRecord;
	class CPointUndoRecord;
	class CSlobUndoRecord;
// CSlob
	class CUndoSlob;        // The master undo/redo controller

#define DEFAULT_UNDO_LEVELS 100

#pragma pack(2)

class CUndoRecord
{
public:
	WORD m_op;
	WORD m_nPropID;
	CSlob* m_pSlob;
	CPropBag* m_pBag;
};

class CIntUndoRecord : public CUndoRecord
{
public:
	int m_nOldVal;
};

class CLongUndoRecord : public CUndoRecord
{
public:
	long m_nOldVal;
};

class CDoubleUndoRecord : public CUndoRecord
{
public:
	double m_numOldVal;
};

class CRectUndoRecord : public CUndoRecord
{
public:
	CRect m_rectOldVal;
};

class CPointUndoRecord : public CUndoRecord
{
public:
	CPoint m_ptOldVal;
};

class CSlobUndoRecord : public CUndoRecord
{
public:
	const CSlob* m_pOldVal;
};

#pragma pack()

///////////////////////////////////////////////////////////////////////////
//	CUndoStack
//		maintains a list of CSlobSequences, one for each action
//		that may be undone or redone.  Each CSlobWnd gets one of these.

class CUndoStack
{
public:
	CUndoStack();
	~CUndoStack();
	
private:
	void Truncate(); // chop seqs off at m_nMaxLevels in the undo part
	void Flush(); // delete seqs and objects
	void FlushRedo(); // delete redo seqs
	
	CObList m_seqs; // CSlobSequence's for actions
	CObList m_objects; // CSlob's that have been deleted
	int m_nRedoSeqs; // how many seqs at head are for redo?
	int m_nMaxLevels; // how many levels are allowed?
	
	friend class CUndoSlob;
	friend class CSlobWnd;
};

// A CSlobSequence holds the codes for one undo or redo operation.
//
class CSlobSequence : public CByteArray
{
public:
	CSlobSequence();
	~CSlobSequence();
	
	void Retrieve(BYTE* rgb, int cb);
	void RetrieveStr(CString& str);
	inline void RetrieveRecord(CUndoRecord& rec)
			{ Retrieve((BYTE*)&rec, sizeof (CUndoRecord)); }
	inline void RetrieveByte(BYTE& b)
			{ Retrieve(&b, 1); }
	inline void RetrieveWord(WORD& w)
			{ Retrieve((BYTE*)&w, 2); }
	inline void RetrieveInt(int& n)
			{ Retrieve((BYTE*)&n, sizeof (int)); }
	inline void RetrieveLong(long& n)
			{ Retrieve((BYTE*)&n, sizeof (long)); }
	inline void RetrieveNum(double& num)
			{ Retrieve((BYTE*)&num, sizeof (double)); }
	inline void RetrievePtr(CSlob*& ptr)
			{ Retrieve((BYTE*)&ptr, sizeof (CSlob*)); }
	inline void RetrievePtr(CPropBag*& ptr)
			{ Retrieve((BYTE*)&ptr, sizeof (CPropBag*)); }
	inline void RetrieveRect(CRect& rc)
			{ Retrieve((BYTE*)&rc, sizeof (rc)); }
	inline void RetrievePoint(CPoint& pt)
			{ Retrieve((BYTE*)&pt, sizeof (pt)); }
	
	void Cleanup();
	BOOL IsUseful(CSlob*&, int&);
	void Apply();
	
#ifdef _DEBUG
	void Dump();
#endif
	
	int m_nCursor;
	CString m_strDescription;
	long m_nSerial;
};


/////////////////////////////////////////////////////////////////////////////
//	CUndoSlob

class CUndoSlob : public CSlob
{
	DECLARE_DYNAMIC(CUndoSlob)

public:
	CUndoSlob();
	~CUndoSlob();

// User Interface control

	BOOL CanUndo() const;
	BOOL CanRedo() const;
			
	void GetUndoString(CString& strUndo, int nLevel = 0);
	void GetRedoString(CString& strRedo, int nLevel = 0);

	void DoUndo(CSlobWnd* pSlobWnd); // perform an undo command
	void DoRedo(CSlobWnd* pSlobWnd); // perform a redo command
	
	void SetMaxLevels(int nLevels);
	int GetMaxLevels() const;

	void Flush(); // empty the current undo and redo stacks
	void FlushLast(); // prevent action merging

	inline BOOL InUndoRedo() const
			{ return m_bPerformingUndoRedo; }
			
// Recording Control

	void BeginUndo(const char* szCmd, CSlobWnd* pSlobWnd = NULL,
		BOOL bResetCursor = TRUE);
	void BeginUndo(UINT nID, CSlobWnd* pSlobWnd = NULL,
		BOOL bResetCursor = TRUE);
	void EndUndo(BOOL bAbort = FALSE);

	inline BOOL IsRecording()
		{ return m_nRecording != 0 && m_nPauseLevel == 0; }

	inline void Pause()
		{ m_nPauseLevel += 1; }
		
	inline void Resume()
		{ ASSERT(m_nPauseLevel > 0); m_nPauseLevel -= 1; }
	
	// This is used to link undo operations in two separate stacks
	// together (only in drag&drop for now)...
	inline void Link()
			{ m_nSerial -= 1; }

// Property change recorders

	void OnSetIntProp(CSlob* pChangedSlob, UINT nPropID, 
		UINT nOldVal, CPropBag* pBag = NULL);
	void OnSetLongProp(CSlob* pChangedSlob, UINT nPropID, 
		long nOldVal, CPropBag* pBag = NULL);
	void OnSetDoubleProp(CSlob* pChangedSlob, UINT nPropID, 
		double numOldVal, CPropBag* pBag = NULL);
	void OnSetStrProp(CSlob* pChangedSlob, UINT nPropID,
		const CString* pStrOldVal, CPropBag* pBag = NULL);
	void OnSetSlobProp(CSlob* pChangedSlob, UINT nPropID,
		const CSlob* pOldSlob, CPropBag* pBag = NULL);
	void OnSetRectProp(CSlob* pChangedSlob, UINT nPropID,
		const CRect& rcOldVal, CPropBag* pBag = NULL);
	void OnSetPointProp(CSlob* pChangedSlob, UINT nPropID,
		const CPoint& rcOldVal, CPropBag* pBag = NULL);
	void OnAddProp(CSlob* pSlob, CPropBag* pBag, UINT nPropID);
	void OnRemoveIntProp(CSlob* pSlob, CPropBag* pBag, 
		UINT nPropID, int nOldVal);
	
// Selection change recorders...

	void OnMultiRemove(CMultiSlob* pMultiSlob, CSlob* pRemoveSlob);
	void InsertSel(UINT nAction, CMultiSlob* pSel, CObList* pItems = NULL);
	void OnPreSel(CMultiSlob* pSel);
	void OnPostSel(CMultiSlob* pSel);
	

// Really low-level recording stuff

	enum {
		// Note correspondence with PRD
		opStart,
		opEnd,
		opAction,
		opIntProp,
		opLongProp,
		opBoolProp,
		opDoubleProp,
		opStrProp,
		opSlobProp,
		opRectProp,
		opPointProp
	};

	UINT Insert(const void* rgb, int cb);
	UINT InsertStr(const char* sz);
	inline UINT InsertByte(BYTE b)
			{ return Insert(&b, 1); }
	inline UINT InsertInt(int n)
			{ return Insert((BYTE*)&n, sizeof (int)); }
	inline UINT InsertLong(long n)
			{ return Insert((BYTE*)&n, sizeof (long)); }
	inline UINT InsertNum(double num)
			{ return Insert((BYTE*)&num, sizeof (double)); }
	inline UINT InsertPtr(const void* ptr)
			{ if (ptr != NULL)
				{
					ASSERT(((CObject*)ptr)->IsKindOf(RUNTIME_CLASS(CSlob)));
					((CSlob*)ptr)->AddDependant(this);
				}
			return Insert((BYTE*)&ptr, sizeof (CSlob*)); }
	inline UINT InsertRect(const CRect& rc)
			{ return Insert((BYTE*)&rc, sizeof (CRect)); }
	inline UINT InsertPoint(const CPoint& pt)
			{ return Insert((BYTE*)&pt, sizeof (CPoint)); }
	inline UINT InsertRecord(const CUndoRecord* pRec)
			{ return Insert((BYTE*)pRec, sizeof (CUndoRecord)); }


// CSlob overrides
	CObList* GetContentList();
	
#ifdef _DEBUG
	void Dump();
#endif

private:
	void Truncate();
	BOOL DoUndoRedo(CSlobWnd* pSlobWnd);
	
	int m_nRecording; // BeginUndo() nesting count
	int m_nPauseLevel; // Pause() nesting count
	
	// These ?Last* variables are used to coalesce consecutive changes
	// to the same property...
	CSlob* m_pLastSlob;
	int m_nLastPropID;
	
	// Properties...
	int m_nMaxLevels;

	CSlobSequence* m_pCurSeq;
	CUndoStack* m_pStack;
	
	BOOL m_bPerformingUndoRedo;
	
	CMultiSlob* m_pSel;
	
	long m_nSerial;
	CSlob* m_pDragDropObject;
	
	BOOL m_bFlushAtEnd;
	
	friend class CSlobSequence;
	friend class CUndoStack;
};

extern CUndoSlob AFX_DATA theUndoSlob;

/////////////////////////////////////////////////////////////////////////////

#undef AFX_DATA
#define AFX_DATA NEAR

#endif // __SLOB_H__
