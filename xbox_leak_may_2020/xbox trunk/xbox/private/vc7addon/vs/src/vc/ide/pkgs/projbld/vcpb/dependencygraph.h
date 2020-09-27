//
// CGrNode, CGraph, CBldFileDepGraph
//
// Graph node, dependency graph node, graph and dependency graph classes
//
// [matthewt]
//

#ifndef __DEPENDENCYGRAPH_H__
#define __DEPENDENCYGRAPH_H__

#pragma once

#ifdef _DEBUG
// Define DEPGRAPH_VIEW to enable graphical view of dependency graph.
#define DEPGRAPH_VIEW
#endif

#include "FileRegistry.h"	// file registry notification (CBldFileRegNotifyRx)

// needed from buildengine.h
class CDynamicBuildEngine;

//////////////////////////////////
// CGrNode - generic graph node	//
//////////////////////////////////

// node instance identifier
typedef DWORD_PTR NID;
#define nidNil ((DWORD_PTR)-1)

// edge instance identifier
typedef CBldAction* EID;
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
class CBldFileDepGraph;
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

	// retrieve the properties of this graph node
	__inline NID Nid()					{ return m_nid; }
	__inline USHORT	CChildren()			{ return m_cChildren; }
	__inline CGrNode* Child(USHORT i)	{ return m_rgChildren[i]; }
	__inline EID Edge(USHORT i)			{ return m_rgEdges[i]; }
	__inline USHORT CParents()			{ return m_cParents; }
	__inline CGrNode* Parent(USHORT i)	{ return m_rgParents[i]; }

	// layout related
#ifdef DEPGRAPH_VIEW
	__inline USHORT	ICol()				{ return m_iDepth; }
	__inline USHORT	IRow()				{ return m_iPos; }
	__inline BOOL	FFake()				{ return m_fFake; }
	__inline BOOL	CReversed()			{ return m_cReversed; }
	__inline BOOL	FRecursive()		{ return m_fRecursive; }
#if DIANEME_TODO	// CGrNode::SetSize, CGrNode::GetSize
	__inline void	SetSize(CSize size)	{ m_sizeView = size; }
	__inline CSize	GetSize()			{ return m_sizeView; }
#endif	// DIANEME_TODO -- CGrNode::SetSize, CGrNode::GetSize
#endif // DEPGRAPH_VIEW

	// add or remove graph node event receivers
	void AddGrNfyRx(CGrNfyRx* pgrnfyrx);
	void RemoveGrNfyRx(CGrNfyRx* pgrnfyrx);

	// Rx for changes in node connections
	virtual void InformGrNfyRx(UINT grevt, void* pvhint = (void *)0);

	// graph mode changed
	virtual void OnGraphMode(UINT newmode) { /* do nothing */ };

	// add a new child to this node with specified edge
	void AddChild(CGrNode* pgrn, EID eid = (EID)NULL);

	// remove a child (referenced by index)
	void DelChild(USHORT iChild);

	BOOL		m_fVisited:1;		// visited?

protected:
	NID			m_nid;				// node instance identifier
	USHORT		m_cParents;			// count of parents
	USHORT		m_cChildren;		// count of children

	CGrNode**	m_rgChildren;		// array of children
	USHORT		m_allocChildren;	// size of child array alloc.
	USHORT		m_allocParents;		// size of parent array alloc.

	CGrNode**	m_rgParents;		// array of parents

	EID *		m_rgEdges;			// array of edges

	// layout related.
#ifdef DEPGRAPH_VIEW
	BOOL		m_fWorking:1;		// this node is currently being examined
	BOOL		m_fFake:1;			// artifically created node -- invalid handle
	BOOL		m_fRecursive:1;		// mark item as recursive if it is
	USHORT		m_iDepth;			// depth of this item
	USHORT		m_iPos;				// position in column
	CGrNode*	m_pgrnAlias;		// pointer to equivalent fake node
	CGrNode*	m_pgrnNext;			// pointer to next node in queue
	CGrNode*	m_pgrnPrev;			// pointer to prev node in queue
	USHORT		m_iParent;			// next parent to insert
	USHORT		m_cSiblings;		// count of siblings
	USHORT		m_cReversed;		// count of reversed edges
	USHORT		m_cost;				// cost in current position
#if DIANEME_TODO	// CGrNode::m_sizeView
	CSize		m_sizeView;			// cache info to help views out
#endif	// DIANEME_TODO	-- CGrNode::m_sizeView
#endif // DEPGRAPH_VIEW

	// Rx
	CGrNfyRx*	m_GrNfyRx;
 
private:
#ifdef DEPGRAPH_VIEW
	// our assigned graph 
	CGraph*	m_pgraph;
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

class CDepGrNode : public CGrNode, CBldFileRegNotifyRx
{
public:
	CDepGrNode(CBldFileDepGraph* pgraph, CBldFileRegistry* pregistry, BldFileRegHandle frh);
	virtual ~CDepGrNode();

	// state query
	__inline UINT Depstate()				{return m_state;}
	__inline void SetDepstate(UINT state)	
		{
			USHORT oldState = m_state;
			m_state = (USHORT)state;
			if( m_state != oldState )
				OnDepStateChanged(m_state);
		}
	__inline CBldFileRegistry*Reg()			{return m_pregistry;}
	__inline BldFileRegHandle Frh()			{return (BldFileRegHandle)Nid();}

	// Rx for changes in dep. connections
	virtual void InformGrNfyRx(UINT grevt, void* pvhint = (void *)0);

	// Rx for file change events
	virtual void OnNotify(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC, UINT idChange, void* pHint);

	// figure the dependency state
	// and sent out an inform if it has changed
	void FigureDepState();

	// inform that dependency state has changed
	virtual void OnDepStateChanged(UINT state);

	// graph mode changed
	virtual void OnGraphMode(UINT newmode);

	USHORT 			m_fDirtyDepLst:1;	// the depcy list is dirty

private:
	USHORT			m_fIgnoreDepcy:1;	// ignore dependency changes?
	USHORT			m_state;			// state of this node
	CBldFileRegistry*	m_pregistry;		// our assigned registry
	CBldFileRegSet		m_frsDepcy;			// dependency file registry set; calc. attributes
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

	// what is the structure of this graph?
	// the 'protocol', ie. all these methods *must* be implemented in derived class
	virtual USHORT	CRootsInitWalk() = 0;
	virtual BOOL	FNextRoot(NID &) = 0;

	virtual USHORT	CChildrenInitWalk(NID nidNode) = 0;
	virtual BOOL	FNextNid(NID &, ET &) = 0;

	// how is this graph to be drawn?
	virtual CGraphPainter* GetPainter()	{ VSASSERT(FALSE, "GetPainter requires override"); return NULL; }
	virtual USHORT	GetMaxDepth()		{ return 0; }
	virtual BOOL 	IsReversed()		{ return FALSE; }
};

////////////////////////////
// CGraph - generic graph //
////////////////////////////  

// order of performing ops. on graph
#define PrePerform	0x0
#define PostPerform	0x1

// graph operation funcation
typedef BOOL (*GraphOpFn)(CGrNode* pgrn, DWORD& dw, BOOL& fLookChildren);

class CGraph : public CGrNfyRx, public CNidGraphDoc
{
public:
	// constructor+destructor
	CGraph();
	virtual ~CGraph();

	// retrieve the properties of this graph
	__inline USHORT CRoots()			{ return m_cRoots; }
	__inline CGrNode* Root(USHORT i)	{ return m_rgRoots[i]; }

	// add and delete root nodes
	void AddRoot(CGrNode* pgrn);
	void DelRoot(CGrNode* pgrn);

	// insert and remove graph nodes
	void InsertNode(CGrNode* pgrn);
	void RemoveNode(CGrNode* pgrn);

	// clear visited
	void ClearVisited(void);

	// CNidGraphDoc implementation for generic graphs
	USHORT	CRootsInitWalk();
	BOOL	FNextRoot(NID& nid);
	USHORT	CChildrenInitWalk(NID nidNode);
	BOOL	FNextNid(NID&, ET&);

	// graph node lookup
	__inline BOOL LookupNode(NID nid, CGrNode*& pgrn)
		{ return m_mapNodes.Lookup((void *)nid, (void * &)pgrn); }

	// set graph mode
	UINT SetGraphMode(UINT mode);

	// Rx for graph node change events
	virtual void OnGrNfy(UINT grevt, CGrNode* pgrn, void* pvhint);

	// perform a graph operation 'pfn' over each node in the graph
	// passes the 'dwUser' to the operation
	// performs in the order specified by 'order'
	// starts with 'nid' or all roots if nid == nidNil
//	BOOL PerformOperation(GraphOpFn pfn, DWORD& dwUser, UINT order = PrePerform, NID nid = nidNil);

	UINT		m_mode;			// mode of the graph

protected:
//	BOOL PerformOperationI(GraphOpFn pfn, DWORD& dwUser, UINT order, CGrNode* pgrn);

	CGrNode**   m_rgRoots;		// our array pf root nodes
	USHORT		m_allocRoots;	// size of root array alloc.

	USHORT		m_cRoots;		// count of roots
	USHORT		m_iRoot;		// state of root graph walk
	USHORT		m_iChild;		// state of children graph walk
	CGrNode*	m_pgrnChild;	// current node in walk

	CVCMapPtrToPtr m_mapNodes;	// our map of the nodes
};

//////////////////////////////////////
// CBldFileDepGraph - dependency graph //
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

class CBldAction;
class CBldFileDepGraph : public CGraph
{
public:
	CBldFileDepGraph(CBldFileRegistry * pregistry);
	virtual ~CBldFileDepGraph();
	
	BOOL IsInitialized()		{ return m_bInitialized; }
	void SetGraphInitialized()	{ m_bInitialized = TRUE; }

	// Dependency graph modification
	BOOL ModifyDep(BldFileRegHandle frhDep, CBldAction* pAction, UINT dep, BOOL fAdd, IVCBuildErrorContext* pEC,
		BOOL fWithAction = TRUE);

	// find a dependency 'frhDep'
	//
	// 'pgrn' is the last found CGrNode (last in 'arggrn')
	// 'arygrn' is the accumulated list of CGrNode's that have the dep. (if NULL then find just one == 'pgrn')
	// 'fSearchSets' search in dependency sets?
	BOOL FindDep(BldFileRegHandle frhDep, CDepGrNode*& pgrn, CVCPtrArray* parygrn = (CVCPtrArray *)NULL,
		BOOL fSearchSets = FALSE);

	// Dependency graph info. retrieval
	void StartPasses();

	// what actions are performed on the dependencies with a state in 'stateFilter' (default is 'not current')?
	// o 'lstFrh' is a list of all files whose state 'match' caused the action addition
	// o 'frhStart' is the dependent to start from (default is 'do all out of date')
	// o 'aor' attributes of action retrieval (default is 'verbose, recurse and check for option changes')
	// returns CMD_ value
	CMD RetrieveBuildActions(CBldCfgRecord* pcrBuild, CBldActionList* plstActions, CVCPtrList& lstFrh, 
		IVCBuildErrorContext* pEC, CDynamicBuildEngine* pBldEngine, BldFileRegHandle frhStart = (BldFileRegHandle)NULL, 
		UINT stateFilter = DS_OutOfDate | DS_ForcedOutOfDate, UINT aor = AOR_Default, BOOL bCheckTimeStamps = TRUE);

	// what actions 'contribute' to the dependent?
	// o 'plstActions' is a list of actions
	// o 'frhStart' is the dependent to start from
	// return CMD_ value
	UINT RetrieveOutputActions(CBldActionList* plstActions, IVCBuildErrorContext* pEC, BldFileRegHandle frhStart);
	
	// what deployment dependencies do we have?
	CMD RetrieveDeploymentDependencies(CBldCfgRecord* pcrBuild, IVCBuildStringCollection** ppDeployStrings);

	// CNidGraphDoc implementation for file dependency graphs
	virtual USHORT	CChildrenInitWalk(NID nidNode);
	virtual BOOL	FNextNid(NID &, ET &);

	// should we 'count' the dependency sets as part of our graph document?
	BOOL m_fShowDepSets;

private:
	// add a dependency relationship
	// o 'frhDpcy' is the file(s) we are dependent on
	// o 'frhDpnt is the dependent file(s)
	// o 'pActionSlob' is the action that should be performed to
	//    make the dependant 'current' wrt the dependency
	BOOL AddDepEdgeI(BldFileRegHandle frhDpcy, BldFileRegHandle frhDpnt, CBldAction* pActionSlob = (CBldAction *)NULL);

	// remove a dependency relationship 
	BOOL RemoveDepEdgeI(BldFileRegHandle frhDpcy, BldFileRegHandle frhDpnt, 
		CBldAction* pActionSlob = (CBldAction *)NULL, BOOL fAllDepnts = FALSE);

	// helper for RetrieveBuildActions(); returns CMD_ value
	CMD EnumerateBuildActionsI(CBldActionList* plstActions, CVCPtrList& lstFrh, IVCBuildErrorContext* pEC,
		CDynamicBuildEngine* pBldEngine, CDepGrNode* pgrn, UINT stateFilter, UINT& aor, BOOL bCheckTimeStamps);
	void AddEnumeratedActionFile(CDepGrNode* pgrn, CVCPtrList& lstFrh);
	BOOL HandleDependencyInfo(CBldAction* pAction);
	CMD EnumerateDeploymentDependenciesI(CDepGrNode* pgrn, IVCBuildStringCollection** ppDeployStrings);

	CVCPtrList 		m_lstDepSets;			// our list of dep. sets
	CBldFileRegistry*  m_pregistry;			// our assigned registry
	CVCPtrList		m_lstCannotBuild;		// cannot build list
	BOOL 			m_bInitialized;			// have we OnInformed yet ?
};

#endif // __DEPENDENCYGRAPH_H__	    
