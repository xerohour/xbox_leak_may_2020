// ipcundo.h : interface of the COleUndoManager and
// CEnumOleUndoUnit classes.
//

#ifndef __IPCUNDO_H__
#define __IPCUNDO_H__

enum UNDOSTATE
{
	usBaseState,
	usUndoState,
	usRedoState
};

enum UNDOACTIONTYPE
{
	atUndoAction,
	atRedoAction,
};

/////////////////////////////////////////////////////////////////////////////
// COleUndoManager
// Implementation of the IOleUndoManager class, which manages
// Undo actions on behalf of in-place components.
//
class COleUndoManager : public IOleUndoManager
{
public:
	COleUndoManager();
	~COleUndoManager();

	//=================================================
	// IUnknown/CCom methods
	//=================================================
	STDMETHOD(QueryInterface)(REFIID riid, void ** ppvObj);
	STDMETHOD_(ULONG,AddRef)(void);
	STDMETHOD_(ULONG,Release)(void);

	//=================================================
	// IOleUndoManager methods
	//=================================================

	STDMETHOD(Open) (IOleParentUndoUnit *pCUA);
	STDMETHOD(Close) (IOleParentUndoUnit *pCUA, BOOL fCommit);
	STDMETHOD(Add) (IOleUndoUnit *pUA);
	STDMETHOD(DiscardFrom)(IOleUndoUnit *pUA);
	STDMETHOD(UndoTo)(IOleUndoUnit *pUA);
	STDMETHOD(RedoTo)(IOleUndoUnit *pUA);
	STDMETHOD(EnumUndoable)(IEnumOleUndoUnits **ppEnum);
	STDMETHOD(EnumRedoable)(IEnumOleUndoUnits **ppEnum);
	STDMETHOD(GetLastUndoDescription) (BSTR *pbstr);
	STDMETHOD(GetLastRedoDescription) (BSTR *pbstr);
	STDMETHOD(Enable)(BOOL fEnable);
	STDMETHOD(GetOpenParentState) (DWORD *pdwState);

	// Non-OLE methods specific to this implementation
	BOOL IsUndoEmpty() {return m_fUndoStackIsEmpty;};
	BOOL IsRedoEmpty() {return m_fRedoStackIsEmpty;};;

protected:
	void OnStackChange();

	DWORD m_cRef;
	DWORD m_cRollbackCtr;
	//$CONSIDER -- these booleans could be combined into bit flags.
	BOOL m_fEnable;
	BOOL m_fUndoStackIsEmpty;
	BOOL m_fRedoStackIsEmpty;
	UNDOSTATE m_usManagerState;
	IOleParentUndoUnit *m_pOpenUndoAction;
	IOleParentUndoUnit *m_pOpenRedoAction;
	CTypedPtrList<CPtrList, IOleUndoUnit *> m_UndoStack, m_RedoStack;
};

/////////////////////////////////////////////////////////////////////////////
// CEnumOleUndoUnit
//
// Description: Implementation of the IEnumOleUndoUnits Manager
// interface that goes with the implement of IOleUndoManager
// above.
//
class CEnumOleUndoUnit : public IEnumOleUndoUnits
{
public:
	CEnumOleUndoUnit(IOleUndoManager * pUndoMgr,
					   CTypedPtrList<CPtrList, IOleUndoUnit *> *pActionList);
	CEnumOleUndoUnit(IOleUndoManager * pUndoMgr,
					   CTypedPtrList<CPtrList, IOleUndoUnit *> *pActionList,
					   ULONG cPos);

	//=================================================
	// IUnknown/CCom methods
	//=================================================
	STDMETHOD(QueryInterface)(REFIID riid, void ** ppvObj);
	STDMETHOD_(ULONG,AddRef)(void);
	STDMETHOD_(ULONG,Release)(void);

	//=================================================
	// IEnumOleUndoUnits
	//=================================================

	STDMETHOD(Next)(ULONG celt, IOleUndoUnit **rgelt, ULONG *pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumOleUndoUnits** ppenum);

private:
	DWORD m_cRef;
	DWORD m_cPos;
	COleRef<IOleUndoManager> m_srpUndoMgr;
	CTypedPtrList<CPtrList, IOleUndoUnit *>  *m_pActionList;
};


/*-----------------------------------------------------------------------------
Class: COleUndoUnit

Description: Implements the Action Manager Interface
-----------------------------------------------------------------------------*/
class COleUndoUnit : public IOleUndoUnit
{
public:
	COleUndoUnit(LPOLESTR strDescription,
				   BOOL fUndoFail=FALSE,
				   BOOL fRedoFail=FALSE);

	//=================================================
	// IUnknown/CCom methods
	//=================================================
	STDMETHOD(QueryInterface)(REFIID riid, void ** ppvObj);
	STDMETHOD_(ULONG,AddRef)(void);
	STDMETHOD_(ULONG,Release)(void);

	//=================================================
	// end of inherited methods
	//=================================================

	STDMETHOD(Do) (IOleUndoManager *pUndoActionManager);
	STDMETHOD(GetDescription) (BSTR *pbstr);
	STDMETHOD(GetUnitType)(CLSID *pclsid, LONG *pnID);
	STDMETHOD(OnNextAdd) (void);
private:
	DWORD m_cRef;
	CString m_strDesc;
	UNDOACTIONTYPE m_atActionType;
	BOOL m_fUndoFail;
	BOOL m_fRedoFail;
};

/*-----------------------------------------------------------------------------
Class: COleParentUndoUnit

Description: Implements the IOleParentUndoUnit Interface
-----------------------------------------------------------------------------*/
class COleParentUndoUnit : public IOleParentUndoUnit
{
public:
	COleParentUndoUnit(UNDOACTIONTYPE atActionType, LPOLESTR strDescription);

	//=================================================
	// IUnknown/CCom methods
	//=================================================
	STDMETHOD(QueryInterface)(REFIID riid, void ** ppvObj);
	STDMETHOD_(ULONG,AddRef)(void);
	STDMETHOD_(ULONG,Release)(void);

	//=================================================
	// IOleUndoUnit methods
	//=================================================
	STDMETHOD(Do) (IOleUndoManager *pUndoActionManager);
	STDMETHOD(GetDescription) (BSTR *pbstr);
	STDMETHOD(GetUnitType)(CLSID *pclsid, LONG *pnID);
	STDMETHOD(OnNextAdd) (void);

	//=================================================
	// end of inherited methods
	//=================================================
	STDMETHOD(Open) (IOleParentUndoUnit *pCUA);
	STDMETHOD(Close) (IOleParentUndoUnit *pCUA, BOOL fCommit);
	STDMETHOD(Add) (IOleUndoUnit *pUA);
	STDMETHOD(FindUnit) (IOleUndoUnit *pUA);
	STDMETHOD(GetParentState) (DWORD *pdwState);

	//=================================================
	// Non OLE methods
	//=================================================
	virtual BOOL IsUndoActionListEmpty();
	virtual void SetUndoDesc(LPCTSTR szDesc);

private:
	DWORD m_cRef;
	BOOL m_fActionBlocked;
	UNDOACTIONTYPE m_atActionType;
	CString m_strDesc;
	IOleParentUndoUnit *m_pOpenAction;
	CTypedPtrList<CPtrList, IOleUndoUnit *> m_culCompoundActionList;
};

#endif	// __IPCUNDO_H__
