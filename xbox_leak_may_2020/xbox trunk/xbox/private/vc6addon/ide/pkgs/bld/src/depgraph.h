//
// CGrNode, CGraph, CFileDepGraph
//
// Graph node, dependency graph node, graph and dependency graph classes
//
// [matthewt]
//

#ifndef _INCLUDE_DEPGRAPH_H
#define _INCLUDE_DEPGRAPH_H

#if defined(_DEBUG) && defined(new)
#define _DO_DEBUG_NEW
#undef new
#endif

#ifdef _DEBUG
// Define DEPGRAPH_VIEW to enable graphical view of dependency graph.
#define DEPGRAPH_VIEW
#endif

#include "pfilereg.h"	// file registry notification (CFileRegNotifyRx)

//////////////////////////////////
// CGrNode - generic graph node	//
//////////////////////////////////

// node instance identifier
typedef ULONG NID;
#define nidNil ((ULONG)-1)

// edge instance identifier
typedef ULONG EID;
#define ET EID

// CGrNfyRx events
#define GrEvt_Destroy		0x1

// Relationship manipulation
#define GrEvt_AddChild		0x2
#define GrEvt_AddParent		0x3
#define GrEvt_DelChild		0x4
#define GrEvt_DelParent		0x5

// Edge manipulation
#define GrEvt_AddEdge		0x6
#define GrEvt_DelEdge		0x7

// mode of graph
#define Graph_Creating		0x0
#define Graph_Destroying	0x1
#define Graph_Stable		0x2

// forward decls.
class CGrNode;
class CGraph;
class CFileDepGraph;
class CLayout;

class CGrNfyRx
{
public:
	CGrNfyRx() {}
	virtual ~CGrNfyRx() {}

	// Rx for graph node change events
	virtual void OnGrNfy(UINT grevt, CGrNode * grn, void * pvhint) = 0;
};

//////////////////////////////////
// CGrNode - generic graph node	//
//////////////////////////////////

class CGrNode
{
friend class CGraph;
friend class CLayout;

public:
	// constructor+destructor
	CGrNode(CGraph * pgraph, NID nid, USHORT cChildren = 0, USHORT iDepth = 0);
	virtual ~CGrNode();

	// handle our own allocations
	// we'll use our own private Win32 heap
	// and place de-alloc'ed blocks into a 
	// global list
// can't override operator new if the "new" is already #defined to be
// DEBUG_NEW
#ifdef USE_LOCAL_BUILD_HEAP
#if defined(_DEBUG) && defined (new)
#define REDEF_NEW
#undef new
#endif
	__inline void * operator new(size_t size) {return g_heapNodes.alloc(size);}
	__inline void * operator new(size_t size, LPCSTR, int) {return g_heapNodes.alloc(size);}
#ifdef REDEF_NEW
#undef REDEF_NEW
#define new DEBUG_NEW
#endif
	__inline void operator delete(void * p) {g_heapNodes.dealloc(p);}
#endif

	// retrieve the properties of this graph node
	__inline NID Nid()					{return m_nid;}
	__inline USHORT	CChildren()			{return m_cChildren;}
	__inline CGrNode * Child(USHORT i)	{return m_rgChildren[i];}
	__inline EID Edge(USHORT i)			{return m_rgEdges[i];}
	__inline USHORT CParents()			{return m_cParents;}
	__inline CGrNode * Parent(USHORT i)	{return m_rgParents[i];}

	// layout related
#ifdef DEPGRAPH_VIEW
	__inline USHORT	ICol()				{return m_iDepth;}
	__inline USHORT	IRow()				{return m_iPos;}
	__inline BOOL	FFake()				{return m_fFake;}
	__inline BOOL	CReversed()			{return m_cReversed;}
	__inline BOOL	FRecursive()		{return m_fRecursive;}
	__inline void	SetSize(CSize size)	{m_sizeView = size;}
	__inline CSize	GetSize()			{return m_sizeView;}
#endif // DEPGRAPH_VIEW

	// add or remove graph node event receivers
	void AddGrNfyRx(CGrNfyRx * pgrnfyrx);
	void RemoveGrNfyRx(CGrNfyRx * pgrnfyrx);

	// Rx for changes in node connections
	virtual void InformGrNfyRx(UINT grevt, void * pvhint = (void *)0);

	// graph mode changed
	virtual void OnGraphMode(UINT newmode) {/* do nothing */};

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext & dc) const;
	virtual void DumpNid(CDumpContext & dc) const;
#endif

	// add a new child to this node with specified edge
	void AddChild(CGrNode * pgrn, EID eid = (EID)NULL);

	// remove a child (refrenced by index)
	void DelChild(USHORT iChild);

#ifdef USE_LOCAL_BUILD_HEAP
	// our local graph node heap
	static CLclHeap g_heapNodes;
#endif

	BOOL		m_fVisited:1;		// visited?
	// BOOL        m_fCleared:1;       // cleared?

protected:
	NID			m_nid;				// node instance identifier
	USHORT		m_cParents;			// count of parents
	USHORT		m_cChildren;		// count of children

	CGrNode * *	m_rgChildren;		// array of children
	USHORT		m_allocChildren;	// size of child array alloc.
	USHORT		m_allocParents;		// size of parent array alloc.

	CGrNode * *	m_rgParents;		// array of parents

	EID *		m_rgEdges;			// array of edges

	// layout related.
#ifdef DEPGRAPH_VIEW
	BOOL		m_fWorking:1;		// this node is currently being examined
	BOOL		m_fFake:1;			// artifically created node -- invalid handle
	BOOL		m_fRecursive:1;		// mark item as recursive if it is
	USHORT		m_iDepth;			// depth of this item
	USHORT		m_iPos;				// position in column
	CGrNode *	m_pgrnAlias;		// pointer to equivalent fake node
	CGrNode *	m_pgrnNext;			// pointer to next node in queue
	CGrNode *	m_pgrnPrev;			// pointer to prev node in queue
	USHORT		m_iParent;			// next parent to insert
	USHORT		m_cSiblings;		// count of siblings
	USHORT		m_cReversed;		// count of reversed edges
	USHORT		m_cost;				// cost in current position
	CSize		m_sizeView;			// cache info to help views out
#endif // DEPGRAPH_VIEW

	// Rx
	CGrNfyRx	*m_GrNfyRx;
 
private:
#ifdef DEPGRAPH_VIEW
	// our assigned graph 
	CGraph *	m_pgraph;
#endif

#ifdef _KIPDEBUG
    static int nCount;
#endif
};

/*
/////////////////////////////////////
// CViewGrNode - visual graph node //
/////////////////////////////////////

class CViewGrNode : public CGrNode
{
public:
	CViewGrNode() {};
	~CViewGrNode() {};
};
*/

////////////////////////////////////////
// CDepGrNode - dependency graph node //
////////////////////////////////////////

// state of a dependency
typedef enum DepState
{
	DS_Unknown = 0x0,			// unknown state
	DS_DepMissing = 0x1,		// dependency missing
	DS_OutOfDate = 0x2,			// dependant out of date wrt dependency
	DS_Current = 0x4,			// dependant is current wrt to dependency
	DS_ForcedOutOfDate= 0x8	// dependant is forced out of date wrt dependency
};

//
// Relationship is as follows:
//
// Dependency (child) <- Dependant (parent)
//
#define DepGrEvt_AddDpnt	GrEvt_AddParent
#define DepGrEvt_DelDpnt	GrEvt_DelParent
#define DepGrEvt_AddDpcy	GrEvt_AddChild
#define DepGrEvt_DelDpcy	GrEvt_DelChild

// Manage a static length list of FileRegHandles
class FRHArray {
public:
	FRHArray( int n = 0 );
	~FRHArray( ){ delete []m_pDepcy; }

	void 		  SetAt( FileRegHandle frh, int i )	{ ASSERT(i<m_nFRH); m_pDepcy[i] = frh; }
	FileRegHandle GetAt( int i ) 					{ ASSERT(i<m_nFRH); return m_pDepcy[i]; }
	int 		  GetCount() 						{ return m_nFRH; }

	void Replace( int n );

private:		
	FileRegHandle  *m_pDepcy;		// the array of n+1 elements
	int				m_nFRH;			// the number of elements
};


class CDepGrNode : public CGrNode, CFileRegNotifyRx
{

public:
	CDepGrNode(CFileDepGraph * pgraph, CFileRegistry * pregistry, FileRegHandle frh);
	virtual ~CDepGrNode();

	// state query
	__inline UINT Depstate()				{return m_state;}
	__inline void SetDepstate(UINT state)	{
		USHORT oldState = m_state;
		m_state = (USHORT)state;
		if( m_state != oldState )
			OnDepStateChanged( m_state );
	}
	__inline FileRegHandle Frh()			{return (FileRegHandle)Nid();}
	__inline CFileRegSet * DepcySet()		{return &m_frsDepcy;}
//	__inline FRHArray * DepcyListAtCurr()	{return &m_lstDepcy;}

	// Rx for changes in dep. connections
	virtual void InformGrNfyRx(UINT grevt, void * pvhint = (void *)0);

	// Rx for file change events
	virtual void OnNotify(FileRegHandle hndFileReg, UINT idChange, DWORD dwHint);

	// figure the dependency state
	// and sent out an inform if it has changed
	void FigureDepState();

	// inform that dependency state has changed
	virtual void OnDepStateChanged(UINT state);

	// graph mode changed
	virtual void OnGraphMode(UINT newmode);

#ifdef _DEBUG
	virtual void DumpNid(CDumpContext & dc) const;
#endif
	// the depcy list is dirty
	USHORT 			m_fDirtyDepLst:1;	

private:
	// ignore dependency changes?
	USHORT			m_fIgnoreDepcy:1;

	// state of this node
	USHORT			m_state;

	// our assigned registry
	CFileRegistry * m_pregistry;

	// dependency file registry set
	CFileRegSet		m_frsDepcy;	// calc. attributes

	// dependency file set at last build attempt
//	FRHArray		m_lstDepcy;
};

/////////////////////////////////////////////////////////////////////////////////////
// CNidGraphDoc - protocol for applying graph structure to an underlying document //
////////////////////////////////////////////////////////////////////////////////////

class CGraphPainter;
class CNidGraphDoc
{
public:
	// constructor+destructor
	CNidGraphDoc() {}
	virtual ~CNidGraphDoc() {}

	//
	// what is the structure of this graph?
	// the 'protocol', ie. all these methods *must* be implemented in derived class
	//
	virtual USHORT	CRootsInitWalk() = 0;
	virtual BOOL	FNextRoot(NID &) = 0;

	virtual USHORT	CChildrenInitWalk(NID nidNode) = 0;
	virtual BOOL	FNextNid(NID &, ET &) = 0;

	//
	// how is this graph to be drawn?
	//
	virtual CGraphPainter * GetPainter() {ASSERT(FALSE); return NULL;}
	virtual USHORT	GetMaxDepth() {return 0;}
	virtual BOOL 	IsReversed() {return FALSE;}
};

////////////////////////////
// CGraph - generic graph //
////////////////////////////  

// order of performing ops. on graph
#define PrePerform	0x0
#define PostPerform	0x1

// graph operation funcation
typedef BOOL (*GraphOpFn)(CGrNode * pgrn, DWORD & dw, BOOL & fLookChildren);

class CGraph : public CObject, public CGrNfyRx, public CNidGraphDoc
{
	DECLARE_DYNCREATE(CGraph)

public:
	// constructor+destructor
	CGraph();
	virtual ~CGraph();

	// retrieve the properties of this graph
	__inline USHORT CRoots()			{return m_cRoots;}
	__inline CGrNode * Root(USHORT i)	{return m_rgRoots[i];}

	// add and delete root nodes
	void AddRoot(CGrNode * pgrn);
	void DelRoot(CGrNode * pgrn);

	// insert and remove graph nodes
	void InsertNode(CGrNode * pgrn);
	void RemoveNode(CGrNode * pgrn);

	// clear visited (default is all)?
	void ClearVisited( void );

	// CNidGraphDoc implementation for generic graphs
	USHORT	CRootsInitWalk();
	BOOL	FNextRoot(NID & nid);
	USHORT	CChildrenInitWalk(NID nidNode);
	BOOL	FNextNid(NID &, ET &);

	// graph node lookup
	__inline BOOL LookupNode(NID nid, CGrNode * & pgrn)
		{return m_mapNodes.Lookup((void *)nid, (void * &)pgrn);}

	// set graph mode
	UINT SetGraphMode(UINT mode);

	// Rx for graph node change events
	virtual void OnGrNfy(UINT grevt, CGrNode * pgrn, void * pvhint);

	// perform a graph operation 'pfn' over each node in the graph
	// passes the 'dwUser' to the operation
	// performs in the order specified by 'order'
	// starts with 'nid' or all roots if nid == nidNil
	BOOL PerformOperation(GraphOpFn pfn, DWORD & dwUser, UINT order = PrePerform, NID nid = nidNil);

#ifdef _DEBUG
	virtual void Dump(CGrNode * pgrn, int iDepth, CDumpContext & dc) const;
	virtual void Dump(CDumpContext & dc) const;
#endif

	// mode of the graph
	UINT		m_mode;

protected:
	BOOL PerformOperationI(GraphOpFn pfn, DWORD & dwUser, UINT order, CGrNode * pgrn);
   	// void ClearCleared(CGrNode * pgrn);

	CGrNode * * m_rgRoots;		// our array pf root nodes
	USHORT		m_allocRoots;	// size of root array alloc.

	USHORT		m_cRoots;		// count of roots
	USHORT		m_iRoot;		// state of root graph walk
	USHORT		m_iChild;		// state of children graph walk
	CGrNode *	m_pgrnChild;	// current node in walk

	CMapPtrToPtr m_mapNodes;	// our map of the nodes
};

//////////////////////////////////////
// CFileDepGraph - dependency graph //
//////////////////////////////////////

// attributes of dependency graph retrieval
#define AOR_None		0x0		// none
#define AOR_Recurse		0x1		// recursively check dependencies
#define AOR_ChkOpts		0x2		// check for action option changes
#define AOR_Verbose		0x4		// verbose mode, ie. use prompts etc.
#define AOR_PreChk		0x8		// pre-check dependencies, ie. don't reset state
#define AOR_ChkInp		0x10	// check for action input changes
#define AOR_Default		(AOR_Recurse | AOR_ChkOpts | AOR_Verbose)

// type of dependency
#define DEP_Scanned		0x1
#define DEP_Source		0x2
#define DEP_Input		0x3
#define DEP_Output		0x4
#define DEP_Dependency	0x5
#define DEP_Missing		0x6
#define DEP_UserDefined	0x7		// Never scanned or modified except by user.

class CActionSlob;
class CFileDepGraph : public CGraph
{
	DECLARE_DYNCREATE(CFileDepGraph)

public:
	CFileDepGraph(CFileRegistry * pregistry);
	virtual ~CFileDepGraph();

	//
	// Dependency graph modification
	//

	BOOL ModifyDep
	(
		FileRegHandle frhDep,
		CActionSlob * pAction,
		UINT dep,
		BOOL fAdd,
		BOOL fWithAction = TRUE
	);

	// find a dependency 'frhDep'
	//
	// 'pgrn' is the last found CGrNode (last in 'arggrn')
	// 'arygrn' is the accumulated list of CGrNode's that have the dep. (if NULL then find just one == 'pgrn')
	// 'fSearchSets' search in dependency sets?
	BOOL FindDep
	(
		FileRegHandle frhDep,
		CDepGrNode * & pgrn,
		CPtrArray * parygrn = (CPtrArray *)NULL,
		BOOL fSearchSets = FALSE
	);

	//
	// Dependency graph info. retrieval
	//

	void StartPasses();

	// what actions are performed on the dependencies with a state in 'stateFilter' (default is 'not current')?
	// o 'lstFrh' is a list of all files whose state 'match' caused the action addition
	// o 'frhStart' is the dependant to start from (default is 'do all out of date')
	// o 'aor' attributes of action retrieval (default is 'verbose, recurse and check for option changes')
	// returns CMD_ value
	UINT RetrieveBuildActions
	(
		CActionSlobList & lstActions,
		CPtrList & lstFrh,
 		CErrorContext & EC,
		FileRegHandle frhStart = (FileRegHandle)NULL,
		UINT stateFilter = DS_OutOfDate | DS_ForcedOutOfDate,
		UINT aor = AOR_Default
	);

	// what actions 'contribute' to the dependent?
	// o 'lstActions' is a list of actions
	// o 'frhStart' is the dependant to start from
	// return CMD_ value
	UINT RetrieveOutputActions
	(
		CActionSlobList & lstActions,
 		CErrorContext & EC,
		FileRegHandle frhStart
	);

	// CNidGraphDoc implementation for file dependency graphs
	virtual USHORT	CChildrenInitWalk(NID nidNode);
	virtual BOOL	FNextNid(NID &, ET &);

	// should we 'count' the dependency sets as part of our
	// graph document?
	BOOL m_fShowDepSets;

	//
	// File Graph Operations
	//

	// clean, dwUser is a error context (CErrorContext *)
	static BOOL Clean(CGrNode * pgrn, DWORD & dwUser, BOOL & fLookChildren);

	// get all output, dwUser is a an array to hold all output FRHs (CFileRegSet *)
	static BOOL GetAllOutput(CGrNode * pgrn, DWORD & dwUser, BOOL & fLookChildren);
	static BOOL GetAllOutputExceptCustom(CGrNode * pgrn, DWORD & dwUser, BOOL & fLookChildren);

	// get the target files
	static BOOL GetAllTarget(CGrNode * pgrn, DWORD & dwUser, BOOL & fLookChildren)
	{
		return GetTarget(FALSE, pgrn, dwUser, fLookChildren);
	}

	// get the primary files
	static BOOL GetPrimaryTarget(CGrNode * pgrn, DWORD & dwUser, BOOL & fLookChildren)
	{
		return GetTarget(TRUE, pgrn, dwUser, fLookChildren);
	}

	// get the primary files
	static BOOL GetPrimaryTargetNoCustom(CGrNode * pgrn, DWORD & dwUser, BOOL & fLookChildren)
	{
		return GetTarget(TRUE, pgrn, dwUser, fLookChildren, TRUE);
	}

	static BOOL GetCustomOutput(CGrNode * pgrn, DWORD & dwUser, BOOL & fLookChildren);

private:
	// helper
	static BOOL GetTarget(BOOL fPrimary, CGrNode * pgrn, DWORD & dwUser, BOOL & fLookChildren, BOOL bNoCustom = FALSE);
	 
	// add a dependency relationship
	// o 'frhDpcy' is the file(s) we are dependant on
	// o 'frhDpnt is the dependent file(s)
	// o 'pActionSlob' is the action that should be performed to
	//    make the dependant 'current' wrt the dependency
	BOOL AddDepEdgeI
	(
		FileRegHandle frhDpcy,
		FileRegHandle frhDpnt,
		CActionSlob * pActionSlob = (CActionSlob *)NULL
	);

	// remove a dependency relationship 
	BOOL RemoveDepEdgeI
	(
		FileRegHandle frhDpcy,
		FileRegHandle frhDpnt,
		CActionSlob * pActionSlob = (CActionSlob *)NULL,
		BOOL fAllDepnts = FALSE
	);

	// our list of dep. sets
	CPtrList m_lstDepSets;

	// our assigned registry
	CFileRegistry * m_pregistry;

	// helper for RetrieveBuildActions()
	// returns CMD_ value
	UINT EnumerateBuildActionsI
	(
		CActionSlobList & lstActions,
 		CPtrList & lstFrh,
		CErrorContext & EC,
		CDepGrNode * pgrn,
		UINT stateFilter,
		UINT & aor
	);

	// already prompted to build setting changes?
	BOOL m_fPromptedCheckOptions;
	
	// cannot build list
	CPtrList m_lstCannotBuild;

};

#ifdef _DO_DEBUG_NEW
#define new DEBUG_NEW
#endif

#endif // _INCLUDE_DEPGRAPH_H	    
