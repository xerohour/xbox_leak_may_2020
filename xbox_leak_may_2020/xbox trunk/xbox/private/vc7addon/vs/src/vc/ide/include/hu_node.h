//---------------------------------------------------------------------------
// Microsoft Visual InterDev
//
// Microsoft Confidential
// Copyright 1994 - 1997 Microsoft Corporation. All Rights Reserved.
//
// File: hu_node.h
// Area: IHierarchy Utilities implementation
//
// Contents:
//	CHierNode definitions
//---------------------------------------------------------------------------
#ifndef __HU_NODE_H__
#define __HU_NODE_H__

class CHierContainer;
class CVsHierarchy;
interface IVsUserContext;
enum  ICON_TYPE;


// IsTypeOf types	(Please reserve < 500 for these utilities)
// we use these instead of the runtime type information (RTTI) (-GR)
enum {
	 Type_CHierNode = 1
	,Type_CHierContainer = 2
	,Type_CHierNestedContainer = 3
	,Type_CVCNode = 4
	,Type_CVCFile = 5
	,Type_CVCBaseFolderNode = 6
	,Type_CVCFileGroup = 7
	,Type_CVCProjectNode = 8
};

// Define type starts for each package
#define Type_VCPkgStart	500		// Start of web pkg node type defines
// (WINSLOWF) #define Type_DtgPkgStart	1000	// Start of dtg pkg node type defines

//-----------------------------------------------------------------------------
// Name: CHierNode
//
// Description:
//	Base node class for every object in a hierarchy. Implements the idea of
//	a node that has a parent, but no children.
//
//---------------------------------------------------------------------------
class CHierNode :
	public CComObjectRootEx<CComSingleThreadModel>		// explict
	,public IOleCommandTarget						// derives from IUnknown
{
protected:

public:
	CHierNode(void);
	virtual ~CHierNode(void);
	// IsKindOf checking
	virtual UINT GetKindOf(void) const;
	BOOL IsKindOf(UINT hKind) const
	{ return (hKind == GetKindOf()); }

	virtual IDispatch*  GetIDispatch(void) {return NULL;}

	virtual BOOL		IsContainer(void) const;	// return FALSE

	// Override in each derived class to test for that class and if failure
	// call the base class. For conveience, it is recommended that you define
	// types of the form: Type_className. ie. for CHierNode the type is 
	// Type_CHierNode.
	// 
	virtual BOOL	 IsTypeOf(int nodeType) const;

	// return a CHierNode typecasted to a VSITEMID or VISTEMID_ROOT
	// Override in each derived class that can also be a VSITEMID_ROOT
	virtual VSITEMID GetVsItemID(void) const;

	// return the itemid of the first child or VSITEMID_NIL
	virtual VSITEMID GetFirstChild(void);

	// traverses to root node via parents
	// the root node is expected to return the associated CVsHierarchy
	virtual CVsHierarchy* GetCVsHierarchy(void) const;

	virtual HRESULT GetNestedHierarchy(REFIID riid, 
										void **ppVsHierarchy, 
										VSITEMID *pitemidNested);

	// Property functions
	virtual HRESULT GetProperty(VSHPROPID propid, VARIANT* pvar);
	virtual HRESULT SetProperty(VSHPROPID propid, const VARIANT& var);
	virtual HRESULT GetGuidProperty(VSHPROPID propid, GUID* pGuid);
	virtual HRESULT ProcessUIMsg(const MSG *pMsg);
	// REVIEW: This is only for debugging purposes
	virtual HRESULT DisplayContextMenu(VARIANT *pvaIn);

	virtual HRESULT GetEditLabel(BSTR *ppEditLabel); // VSHPROPID_EditLabel
	virtual HRESULT SetEditLabel(BSTR   pEditLabel); // VSHPROPID_EditLabel

	// all nodes which neeed to handle these functions should over-ride them
	virtual const GUID* PGuidGetType(void) const;
	virtual UINT GetIconIndex(ICON_TYPE);
	virtual HRESULT GetCanonicalName(BSTR* ppszName);
	virtual HRESULT DoDefaultAction();

	virtual const CString& GetCaption(void) = 0;
	virtual const CString& GetFullPath(void) = 0;
	virtual void SetCaption(LPCSTR pszNewCaption) = 0;
	virtual void SetCaption(LPCWSTR pszNewCaption) = 0;
	virtual void SetCaption(const CString& strNewCaption) = 0;
	virtual	HRESULT			CleanUpBeforeDelete() = 0;

	virtual HRESULT GetUserContext(IVsUserContext **ppUserCtx);

	// CHierNode Properties
	CHierContainer* GetParent(void) const
	{
		return m_pNodeParent;
	}
	void SetParent(CHierContainer *pNode)
	{
		m_pNodeParent = pNode;
	}
	CHierNode* GetNext(void) const
	{
		return m_pNodeNext;
	}
	void SetNext(CHierNode *pHierNode)
	{
		m_pNodeNext = pHierNode;
	}
	CHierNode* GetPrev(void) const;
	CHierNode* GetRootNode(void) const;

	// is the given node an ancestor of this node
	BOOL HasAncestor(const CHierNode *pNode) const;

// interface functionality support
public:
	STDMETHOD(get_Caption)(BSTR *pCaption);
	STDMETHOD(put_Caption)(LPCWSTR Caption);
	BOOL			m_fTransient;
	BOOL			m_fUntitled;
	
protected:
	CHierContainer* m_pNodeParent;
	CHierNode*  	m_pNodeNext;	// this node is a part of a singly-linked list
};

//---------------------------------------------------------------------------
// _TMyClass - the class that is deriving from CHierImpl class
// _TBase	 - the class that _TMyClass would derive from instead of CHierImpl
//---------------------------------------------------------------------------
template <class _TMyClass, class _TBase, int _TType>
class CHierImpl : public _TBase
{
public:
	BOOL IsTypeOf(int nodeType) const
	{
		if (_TType == nodeType)
			return TRUE;
		return _TBase::IsTypeOf(nodeType);
	}
	UINT GetKindOf(void) const
	{
		return _TType;
	}
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <class _TMyClass, class _TBase, int _TType, int _TType2, int _TRet2>
class CHierImpl2 : public _TBase
{
public:
	BOOL IsTypeOf(int nodeType) const
	{
		if (_TType == nodeType)
			return TRUE;
		if (_TType2 == nodeType)
			return _TRet2;
		return _TBase::IsTypeOf(nodeType);
	}
	UINT GetKindOf(void) const
	{
		return _TType;
	}
};

#endif	// __HU_NODE_H__
