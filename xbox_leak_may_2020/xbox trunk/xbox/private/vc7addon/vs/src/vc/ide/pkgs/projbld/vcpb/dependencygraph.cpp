//
// CGrNode, CGraph, CDepGraph
//
// Graph node, dependency graph node, graph and dependency graph classes
//
// [matthewt]
//

#include "stdafx.h"				// our standard AFX include
#pragma hdrstop
#include "BuildEngine.h"
#include "DependencyGraph.h"	// our local header
#include <vccolls.h>
#include "msgboxes2.h"
#include "MRDependencies.h"

#pragma warning(disable:4244) // int to short conversion OK in this file ONLY !

//////////////////////////////////
// CGrNode - generic graph node	//
//////////////////////////////////


// granularity of allocations
#define C_ALLOC_PARENT	2
#define C_ALLOC_CHILD	12
#define C_ALLOC_ROOT	12
#define C_ALLOC_EDGES	C_ALLOC_CHILD

#ifdef _KIPDEBUG
	int CGrNode::nCount = 0;
#endif

// constructor+destructor
CGrNode::CGrNode(CGraph* pgraph, NID nid, USHORT cChildren /* = 0 */, USHORT iDepth /* = 0 */)
{
	// this graph node's identifier
	m_nid = nid;

	// this graph node's depth
#ifdef DEPGRAPH_VIEW
	m_iDepth = iDepth;
#endif // DEPGRAPH_VIEW

	// initialise
	m_rgChildren = m_rgParents = (CGrNode**)NULL;
	m_allocChildren = m_allocParents = 0;
	m_rgEdges = (EID *)NULL;

	// layout related
#ifdef DEPGRAPH_VIEW
	m_pgrnAlias = m_pgrnPrev = 	m_pgrnNext = (CGrNode *)NULL;
	m_iParent = 0;
	m_cReversed = 0;
	m_cost = 0xffff;
	m_iPos = 0;
	m_fFake = FALSE;
	m_fWorking = FALSE;
	m_fRecursive = FALSE;
#endif // DEPGRAPH_VIEW

	m_cParents = 0;
	m_cChildren = cChildren;
	m_fVisited = FALSE;

#ifdef DEPGRAPH_VIEW
	m_pgraph = pgraph;
#endif

	m_GrNfyRx = NULL;

	// pre-allocate some children?
	if (m_cChildren)
	{
		m_allocChildren += m_cChildren;

		m_rgChildren = new CGrNode *[m_allocChildren];
		m_rgEdges = new EID[m_allocChildren];
	}

#ifdef _KIPDEBUG
	nCount++;
#endif
}

CGrNode::~CGrNode()
{
	// inform that we are being deleted
	InformGrNfyRx(GrEvt_Destroy);
	 
	// delete our storage
	if (m_rgParents)	delete []m_rgParents;
	if (m_rgChildren)	delete []m_rgChildren;
	if (m_rgEdges)		delete []m_rgEdges;
}

// add or remove graph node event receivers
void CGrNode::AddGrNfyRx(CGrNfyRx* pgrnfyrx)
{	
	VSASSERT(m_GrNfyRx == NULL, "Can only call AddGrNfyRx once!");
	m_GrNfyRx = pgrnfyrx;
}

void CGrNode::RemoveGrNfyRx(CGrNfyRx* pgrnfyrx)
{
#ifdef _DEBUG
	if (m_GrNfyRx != NULL)
		VSASSERT(m_GrNfyRx == pgrnfyrx, "Trying to remove wrong GrNfyRx!");
#endif
	m_GrNfyRx = NULL;
}

void CGrNode::InformGrNfyRx(UINT grevt, void* pvhint)
{
	if (m_GrNfyRx == NULL)
		return;
	m_GrNfyRx->OnGrNfy(grevt, this, pvhint);
}

// add a new child to this node
void CGrNode::AddChild(CGrNode* pgrn, EID eid /* = (EID)0 */)
{
	if (pgrn == NULL)
		return;

	// allocate our new array of children?
	BOOL fRealloc = m_cChildren >= m_allocChildren;

	CGrNode** rgChildren = m_rgChildren; 
	EID* rgEdges = m_rgEdges;
	
	// re-alloc?
	if (fRealloc)
	{
		m_allocChildren += C_ALLOC_CHILD;
		rgChildren = new CGrNode *[m_allocChildren];
		rgEdges = new EID[m_allocChildren];
		if (rgChildren == NULL || rgEdges == NULL)
			return;

		// move current children
		if (m_cChildren)
		{
			memcpy(rgChildren, m_rgChildren, m_cChildren * sizeof(CGrNode *));
			memcpy(rgEdges, m_rgEdges, m_cChildren * sizeof(EID));
		}
	}
	 
	// insert child + edge into child + edge list
	rgChildren[m_cChildren] = pgrn;
	rgEdges[m_cChildren] = eid;

	// remember our new array?
	if (fRealloc)
	{

		if (m_rgChildren) 
			delete []m_rgChildren;
		if (m_rgEdges) 
			delete []m_rgEdges;

		m_rgChildren = rgChildren;
		m_rgEdges = rgEdges;
	}

	// increment our child count
	m_cChildren++;

	// inform
	InformGrNfyRx(GrEvt_AddChild, (void *)pgrn->Nid()); 
//	InformGrNfyRx(GrEvt_AddEdge, (void *)eid); 

	//
	// add ourselves as a parent of this node
	//

	// allocate our new array of parents for it?
	fRealloc = pgrn->m_cParents >= pgrn->m_allocParents;
 	CGrNode** rgParents = pgrn->m_rgParents;
	
	if (fRealloc)
	{
		pgrn->m_allocParents += C_ALLOC_PARENT;
		rgParents = new CGrNode *[pgrn->m_allocParents];

		// move current parents
		if(pgrn->m_cParents)
			memcpy(rgParents, pgrn->m_rgParents, pgrn->m_cParents * sizeof(CGrNode *));
	}

	// insert ourselves as a parent
	rgParents[pgrn->m_cParents] = this;

	// remember our new array?
	if (fRealloc)
	{
		if (pgrn->m_rgParents) delete []pgrn->m_rgParents;
		pgrn->m_rgParents = rgParents;
	}

	// increment their parent count
	pgrn->m_cParents++;

	// inform
	pgrn->InformGrNfyRx(GrEvt_AddParent, (void *)this->Nid());
}

// remove a child (referenced by index)
void CGrNode::DelChild(USHORT iChild)
{
	// ensure within our bounds
	VSASSERT(m_cChildren > 0 && iChild < m_cChildren, "Trying to delete out of bounds child!");

	// remember the child before removal
	CGrNode* pgrn = m_rgChildren[iChild];
	EID eid = m_rgEdges[iChild];

	// remove this child
	while (iChild + 1 < m_cChildren)
	{
		m_rgChildren[iChild] = m_rgChildren[iChild+1];
		m_rgEdges[iChild] = m_rgEdges[iChild+1];
		iChild++;
	}

	// decrement our child count
	m_cChildren--;

	// inform
	InformGrNfyRx(GrEvt_DelChild, (void *)pgrn->Nid());
//	InformGrNfyRx(GrEvt_DelEdge, (void *)eid); 

	// remove ourselves as a parent?
	if (pgrn->m_cParents)
	{
		BOOL fFound = FALSE;
		USHORT iParent = 0;
		while (iParent < pgrn->m_cParents)
		{
			// if found then remove parent
			if (fFound)
				pgrn->m_rgParents[iParent - 1] = pgrn->m_rgParents[iParent];

			// if ~found then search for parent
			else
				fFound = (pgrn->m_rgParents[iParent] == this);

			iParent++;
		}

		// make sure we found the parent
		VSASSERT(fFound, "Failed to find the node's parent!");

		// decrement their parent count
		pgrn->m_cParents--;

		// inform
		pgrn->InformGrNfyRx(GrEvt_DelParent, (void *)this);
	}
}

//////////////////////////////////
// CGrNode - generic graph node	//
//////////////////////////////////

CDepGrNode::CDepGrNode(CBldFileDepGraph* pgraph, CBldFileRegistry* pregistry, BldFileRegHandle frh)
	: CGrNode(pgraph, (NID)frh)
{
	// store the file registry
	m_pregistry = pregistry;

	// initialise
	m_state = DS_Unknown;
	m_fIgnoreDepcy = pgraph->m_mode != Graph_Stable; 

	if (!m_fIgnoreDepcy)
		(void) m_frsDepcy.CalcAttributes();

	// add ourselves as a Rx of file change events
	// of both the dependent and dependency
	m_pregistry->GetRegEntry(Frh())->AddNotifyRx(this, FALSE);

	if (!m_fIgnoreDepcy)
		m_frsDepcy.AddNotifyRx(this, FALSE);

	// dependency list at last build attempt is not dirty
	m_fDirtyDepLst = FALSE;

	// add a reference to this registry entry
	Frh()->AddFRHRef();

#ifdef _DEBUG_BLD
	wchar_t * pchDep;
	CBldFileRegEntry * preg = m_pregistry->GetRegEntry(Frh());

	if (preg->IsNodeType(CBldFileRegNotifyRx::nodetypeRegSet))
		pchDep = L"set";
	else
		pchDep = (wchar_t *)(const wchar_t *)*preg->GetFilePath();
#endif // _DEBUG_BLD
}

CDepGrNode::~CDepGrNode()
{
	// inform that we are being deleted
	InformGrNfyRx(GrEvt_Destroy);

#ifdef _DEBUG_BLD
	wchar_t * pchDep;
	CBldFileRegEntry* preg = m_pregistry->GetRegEntry(Frh());

	if (preg->IsNodeType(CBldFileRegNotifyRx::nodetypeRegSet))
		pchDep = L"set";
	else
		pchDep = (wchar_t *)(const wchar_t *)*preg->GetFilePath();
 #endif // _DEBUG_BLD

	// remove ourselves as a Rx of file change events of both the dependent and dependency
	m_pregistry->GetRegEntry(Frh())->RemoveNotifyRx(this);
	
	if (!m_fIgnoreDepcy)
		m_frsDepcy.RemoveNotifyRx(this);

	// assert that we don't have any dependencies
	VSASSERT(m_frsDepcy.IsEmpty(), "We shouldn't have dependencies at this point!");

	// remove a reference to this registry entry
	Frh()->ReleaseFRHRef();
}

// graph mode changed
void CDepGrNode::OnGraphMode(UINT newmode)
{
	// what would be our new state?
	BOOL fIgnoreDepcy = newmode != Graph_Stable; 

	// changed?
	if (m_fIgnoreDepcy != (USHORT)fIgnoreDepcy)
	{
		// remove ourselves as a dependency
		m_frsDepcy.RemoveNotifyRx(this);

		// empty our dependency set
		m_frsDepcy.EmptyContent();

		if (!fIgnoreDepcy)
		{
			// turn-off incr. calc. of attributes
			(void) m_frsDepcy.CalcAttributes(FALSE);

			// add new dependencies to our set
			for (USHORT iChild = CChildren(); iChild > 0; iChild--)
			{
				CDepGrNode* pgrnDep = (CDepGrNode *)Child(iChild - 1);
				m_frsDepcy.AddRegHandle(pgrnDep->Frh(), NULL);
			}

			// turn back on incr. calc. of attributes
			(void) m_frsDepcy.CalcAttributes();

			// update our dependency state
			FigureDepState();

			// add ourselves as a dependency now
			m_frsDepcy.AddNotifyRx(this, TRUE);
		}

		// dependency list at last build attempt is dirty   
		m_fDirtyDepLst = TRUE;

		m_fIgnoreDepcy = (USHORT)fIgnoreDepcy;
	}
}

// Rx for file change events
void CDepGrNode::OnNotify(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC, UINT idChange, void* pHint)
{
	// which type of file change event?
	switch (idChange)
	{
		// newest timestamp changes?
		case P_NewTStamp:

		// existence of dependencies?
		case P_ExistsOnDisk:

			// update our dependency state...
			FigureDepState();
			break;

		// File set changed.
		case FRI_ADD:
		case FRI_DESTROY:
			FigureDepState();
			break;

		default:
			break;
	}
}

// Rx for changes in dep. connections
void CDepGrNode::InformGrNfyRx(UINT grevt, void* pvhint)
{
	// which type of dep. connection event?
	switch (grevt)
	{
		// changing what is dependent on us?
		case DepGrEvt_AddDpnt:
		case DepGrEvt_DelDpnt:
			break;

		// changing what we depend on?
		case DepGrEvt_AddDpcy:
		case DepGrEvt_DelDpcy:
			
			// do we want to do this now?
			if (!m_fIgnoreDepcy)
			{
				if (grevt == DepGrEvt_AddDpcy)
					m_frsDepcy.AddRegHandle((BldFileRegHandle)pvhint, NULL);
				else
					m_frsDepcy.RemoveRegHandle((BldFileRegHandle)pvhint);

				// dependency list at last build attempt is dirty   
				m_fDirtyDepLst = TRUE;
			}
			break;

		default:
			break;
	}

	// send out the event (use the base-class to do this)
	CGrNode::InformGrNfyRx(grevt, pvhint);
}

// figure the dependency state
// and sent out an inform if it has changed
void CDepGrNode::FigureDepState()
{
	// cache old state
	UINT oldstate = (UINT)m_state;

	// new state 
	UINT newstate = DS_Unknown;

	// ignore if no dependencies
	if (!m_frsDepcy.IsEmpty())
	{
		BOOL bVal;

		// one of our deps. missing?
		if (m_frsDepcy.GetIntProp(P_ExistsOnDisk, bVal, FALSE) && !bVal)				
			newstate |= DS_DepMissing;

		// If we are forced out of date then ignore current/out of date stuff
		if ((m_state & DS_ForcedOutOfDate) == 0)
		{
			// compare deps. most recent timestamp to our timstamp
		 	CBldFileRegEntry* pentry = m_pregistry->GetRegEntry(Frh());
			if (pentry && pentry->GetIntProp(P_ExistsOnDisk, bVal, FALSE) && bVal)
			{
				FILETIME ftimeDep, ftimeOur;
				if (m_frsDepcy.GetTimeProp(P_NewTStamp, ftimeDep, FALSE) &&
					m_pregistry->GetRegEntry(Frh())->GetTimeProp(P_NewTStamp, ftimeOur, TRUE))
				{
					if (CBldFileRegistry::MyCompareFileTime(&ftimeDep, &ftimeOur) <= 0)
						newstate |= DS_Current;
					else
					{
						newstate |= DS_OutOfDate;
#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
						ATLTRACE("\tForcing dep node 0x%x out of date due inability to get timestamp\n", this);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS
					}
				}
			}
			else
			{
				newstate |= DS_OutOfDate;
#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
						ATLTRACE("\tForcing dep node 0x%x out of date due to missing dep\n", this);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS
			}
		}
	}

#ifdef _DEBUG	// this test moved above
	// If we are forced out of date then ignore current/out of date stuff
	if (m_state & DS_ForcedOutOfDate)
		VSASSERT((newstate & (DS_Current | DS_OutOfDate)) == 0, "Bad state!");
#endif

	// Actually alter our state now
	m_state &= ~(DS_Current | DS_OutOfDate | DS_DepMissing);
	m_state |= newstate;

	// changed?
	if (m_state != oldstate)
		OnDepStateChanged((UINT)m_state);
}

void CDepGrNode::OnDepStateChanged(UINT state)
{
    VSASSERT((m_state & (DS_Current | DS_OutOfDate)) != (DS_Current | DS_OutOfDate), "Bad state!");
    VSASSERT((m_state & (DS_Current | DS_ForcedOutOfDate)) != (DS_Current | DS_ForcedOutOfDate), "Bad state!");
#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
	ATLTRACE("\tChanging depstate for dep node 0x%x, depstate = %d\n",this, state);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS

    // If we are out of date then propagate changes to parents
    if (m_state & (DS_OutOfDate | DS_ForcedOutOfDate))
    {
        // Propagate changes to next layer down!!
        for (USHORT iParent = CParents(); iParent > 0; iParent--)
        {
            CDepGrNode* pgrnParent = (CDepGrNode *)Parent(iParent-1);
#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
	ATLTRACE("\tForcing parent 0x%x of dep node 0x%x out of date\n", pgrnParent, this);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS

            UINT depstate = pgrnParent->Depstate();

            // if not out of date, then force out of date
            if ((depstate & (DS_OutOfDate | DS_ForcedOutOfDate)) == 0)
                pgrnParent->SetDepstate((depstate & ~DS_Current) | DS_ForcedOutOfDate); 
        }
    }
}

///////////////////////////////////////
// CGraph - generic graph 'protocol' //
///////////////////////////////////////

CGraph::CGraph() :
	m_mapNodes(991)	// increase default hash table size to allow large graphs.
{
	// initialise
	m_rgRoots = (CGrNode**)NULL;
	m_allocRoots = 0;

	m_cRoots = 0;
	m_mode = Graph_Stable;
}

CGraph::~CGraph()
{
	// delete our roots (we shouldn't have any)
	VSASSERT(!m_cRoots, "All of our roots should be gone before CGraph destructor is called!");

	// delete our storage
	int cRoots = m_cRoots; // bug fix
	while (cRoots > 0)
		delete m_rgRoots[--cRoots];

	VSASSERT(m_cRoots == 0, "m_cRoots decremented inside d'tor!");
	if (m_rgRoots)	
		delete []m_rgRoots;
}

// CNidGraphDoc implementation
USHORT CGraph::CRootsInitWalk()
{
	m_iRoot = 0;
	return CRoots();
}

BOOL CGraph::FNextRoot(NID& nid)
{
	int n = CRoots();
	if( m_iRoot >= n )
		return FALSE;

	for( ;m_iRoot<n;m_iRoot++){
		CDepGrNode *pNode = (CDepGrNode*)Root(m_iRoot);
		CBldFileRegFile* pentry =(CBldFileRegFile*) pNode->Reg()->GetRegEntry(pNode->Frh());
		const CPathW* pFile = pentry->GetFilePath();
		CStringW strExt = pFile->GetExtension();
		// display it relative to current directory
		if(
		 	strExt == L".lib" ||
		 	strExt == L".ilk" ||
			strExt == L".idb" ||
			strExt == L".exp" ||
			strExt == L".pdb"
		  )
			continue;
		nid = (NID)pNode;
		m_iRoot++;
		return TRUE;
	}
	return FALSE;
}

USHORT CGraph::CChildrenInitWalk(NID nidNode)
{
	m_iChild = 0;
	m_pgrnChild = (CGrNode *)nidNode;

	return m_pgrnChild->CChildren();
}

BOOL CGraph::FNextNid(NID & nid, ET & et)
{
	if (m_iChild >= m_pgrnChild->CChildren())
		return FALSE;

	nid = (NID)m_pgrnChild->Child(m_iChild);
	et = m_pgrnChild->Edge(m_iChild++);
	return TRUE;

}

// insert and remove graph nodes
void CGraph::InsertNode(CGrNode* pgrn)
{
#ifdef _DEBUG
	void * pvDummy;
	VSASSERT(!m_mapNodes.Lookup((void *)pgrn->Nid(), pvDummy), "Found the node we're trying to insert already in the map!");
#endif

	// make sure we are informed of changes
	pgrn->AddGrNfyRx(this);

	// is this a root?
	if (pgrn->CParents() == 0)
		AddRoot(pgrn);
 	 
	// remember this
	m_mapNodes.SetAt((void *)pgrn->Nid(), pgrn);
}

void CGraph::RemoveNode(CGrNode* pgrn)
{
#ifdef _DEBUG
	void * pvDummy;
	VSASSERT(m_mapNodes.Lookup((void *)pgrn->Nid(), pvDummy), "Trying to remove a node that isn't there!");
#endif

	// is this a root?
	if (pgrn->CParents() == 0)
		DelRoot(pgrn);

	// make sure we are no longer informed
	pgrn->RemoveGrNfyRx(this);

	// don't remember this
	m_mapNodes.RemoveKey((void *)pgrn->Nid());
}

void CGraph::AddRoot(CGrNode* pgrn)
{
	VSASSERT(pgrn->CParents() == 0, "Root already has parents!");

	// allocate our new array of children?
	BOOL fRealloc = m_cRoots >= m_allocRoots;
	CGrNode** rgRoots = m_rgRoots;

	// re-alloc?
	if (fRealloc)
	{
		m_allocRoots += C_ALLOC_ROOT;

		rgRoots = new CGrNode *[m_allocRoots];

		// move our current roots
		if(m_cRoots)
			memcpy(rgRoots, m_rgRoots, m_cRoots * sizeof(CGrNode *));
	}

	// insert this root
	rgRoots[m_cRoots] = pgrn;

	// remember our new array?
	if (fRealloc)
	{
		if (m_rgRoots) 
			delete []m_rgRoots;
		m_rgRoots = rgRoots;
	} 

	// increment our root count
	m_cRoots++;
}

void CGraph::DelRoot(CGrNode* pgrn)
{
	BOOL fFound = FALSE;
	int iRoot = 0;
	while (iRoot < m_cRoots)
	{
		// if found then remove parent
		if (fFound)
			m_rgRoots[iRoot - 1] = m_rgRoots[iRoot];

		// if ~found then search for parent
		else
			fFound = (m_rgRoots[iRoot] == pgrn);

		iRoot++;
	}

	// make sure we found the root
	VSASSERT(fFound, "Failed to find the root!");

	// decrement the root count
	m_cRoots--;
}

// although originally writen to recursively clear flags, this routine was only
// used to clear the entire dep graph. It should only be used that way.
void CGraph::ClearVisited(void)
{
	void* p;
	void* pVal;
	CGrNode* pgrn;

	// iterate over all nodes in this graph. flipping the visited bit to false.
	VCPOSITION pos = m_mapNodes.GetStartPosition();
	while (pos != (VCPOSITION)NULL) 
	{
		m_mapNodes.GetNextAssoc(pos, p, pVal);
		pgrn = (CGrNode *)pVal;
		pgrn->m_fVisited = FALSE;
	}
}

// Rx for graph node change events
void CGraph::OnGrNfy(UINT grevt, CGrNode* pgrn, void* pvhint)
{
	switch (grevt)
	{
		case GrEvt_AddParent:
			// no longer a root? 
			if (pgrn->CParents() == 1)
				DelRoot(pgrn);
			break;

		case GrEvt_DelParent:
			// become a root?
			if (pgrn->CParents() == 0)
				AddRoot(pgrn);
 			break;

		case GrEvt_AddChild:
		case GrEvt_DelChild:
			break;

		case GrEvt_Destroy:
			RemoveNode(pgrn);
			break;

		default:
			break;
	}
}

// set graph mode
UINT CGraph::SetGraphMode(UINT mode)
{
	UINT oldmode = m_mode;
	m_mode = mode;

	// loop through and tell them of this change
	VCPOSITION pos = m_mapNodes.GetStartPosition();
	while (pos != (VCPOSITION)NULL)
	{
		void* key; 
		CGrNode* pgrn;
		m_mapNodes.GetNextAssoc(pos, key, (void * &)pgrn); 
		pgrn->OnGraphMode(m_mode);
	}

	return oldmode;	// return old mode
}

#if 0
BOOL CGraph::PerformOperationI(GraphOpFn pfn, DWORD& dw, UINT order, CGrNode* pgrn)
{
 	// visited
	pgrn->m_fVisited = TRUE;

	VSASSERT(pfn, "Bad input parameter");
	VSASSERT(pgrn != (CGrNode *)NULL, "Bad input parameter");

	BOOL fLookChildren = TRUE;	// enumerate kids?

	// perform a pre-op.?
	if (order == PrePerform)
		if (!(*pfn)(pgrn, dw, fLookChildren))
			return FALSE;	// failed to perform op.

	// enumerate kids
	if (fLookChildren)
	{
		for (USHORT iChild = 0; iChild < pgrn->CChildren(); iChild++)
		{
			CGrNode* pgrnChild = pgrn->Child(iChild);
			if (!pgrnChild->m_fVisited)
				if (!PerformOperationI(pfn, dw, order, pgrnChild))
					return FALSE;	// failed to perform op. for child
		}
	}

	// perform a post-op.
	if (order == PostPerform)
		if (!(*pfn)(pgrn, dw, fLookChildren))
			return FALSE;	// failed to perform op.

	return TRUE;	// ok
}
#endif

#if 0
BOOL CGraph::PerformOperation(GraphOpFn pfn, DWORD& dw, UINT order /* = PrePerform */, NID nid /* = nidNil */)
{
	VSASSERT(pfn, "Bad input parameter");

	// for each of these nodes construct a list of actions, don't visit nodes twice
    // which may happen given multiple starts
	ClearVisited();

	if (nid == nidNil)
	{
		// perform operation for roots
		for (USHORT iRoot = 0; iRoot < m_cRoots; iRoot++)
			if (!PerformOperationI(pfn, dw, order, m_rgRoots[iRoot]))
				return FALSE;	// failed to perform op. for root
	} 
	else
	{
		// perform op. for select node
		CGrNode* pgrn;
		if (!LookupNode(nid, pgrn) || !PerformOperationI(pfn, dw, order, pgrn))
			return FALSE;	// failed to find node or perform op.
	}

	return TRUE;	// ok
}
#endif

///////////////////////////////////////////
// Default graph ops
///////////////////////////////////////////

///////////////////////////////////////////
// CBldFileDepGraph - file dependency graph //
///////////////////////////////////////////

CBldFileDepGraph::CBldFileDepGraph(CBldFileRegistry* pregistry)
{
	// assign our registry
	m_pregistry = pregistry;

	// don't show dep. sets by default
	m_fShowDepSets = FALSE;
	m_bInitialized = FALSE;
}

CBldFileDepGraph::~CBldFileDepGraph()
{
}

BOOL CBldFileDepGraph::ModifyDep(BldFileRegHandle frhDep, CBldAction* pAction, UINT dep, BOOL fAdd,
	IVCBuildErrorContext* pEC, BOOL fWithAction /* = TRUE */)
{
	// what kind of dep.?
	switch (dep)
	{
		case DEP_Input:
		case DEP_Source:
		{
			// add this input or source dep. to each of the outputs
			int i = 0;
			BldFileRegHandle frhOut;
			CBldFileRegSet* pfrsOutput = pAction->GetOutput(pEC);
			CVCWordArray* paryOutputAttrib = pAction->GetOutputAttrib();

			pfrsOutput->InitFrhEnum();
			while ((frhOut = pfrsOutput->NextFrh()) != (BldFileRegHandle)NULL)
			{
				BOOL fWithAction = !!paryOutputAttrib->GetAt(i++);
				CBldAction* pEdgeAction = fWithAction ? pAction : (CBldAction *)NULL;

				if (fAdd)
				{
					if (!AddDepEdgeI(frhDep, frhOut, pEdgeAction))
					{
						frhOut->ReleaseFRHRef();
						return FALSE;
					}
				}
				else
				{
					if (!RemoveDepEdgeI(frhDep, frhOut, pEdgeAction, TRUE /* propagate */))
					{
						frhOut->ReleaseFRHRef();
						return FALSE;
					}
				}
				frhOut->ReleaseFRHRef();
			}
		
			break;
		}

		case DEP_Output:
		{
			// remove scanned/source/missing dependencies (en masse) to this output
			// this means all non-target tools, any 'custom' tools, plus the single target tool generating the primary output
			if (!fAdd && HandleDependencyInfo(pAction))
			{
				CBldFileRegSet* pfrsScanned = pAction->GetScannedDep();

				if (!RemoveDepEdgeI((BldFileRegHandle)pfrsScanned, frhDep, (CBldAction *)NULL, TRUE /* propagate */))
					return FALSE;

				// this set doesn't need to calc attributes!
				// OLYMPUS 13658: However we do need to calc attributes if this set is
				// used by some other action, i.e it has some parents after removing the edge above.
				// So we can only switch off calc of attributes when the set has 0 parents.
                CDepGrNode* pgrnDepcy;
				if (FindDep(pfrsScanned, pgrnDepcy) && pgrnDepcy->CParents() == 0)
					pfrsScanned->CalcAttributes(FALSE);

				CBldFileRegSet* pfrsSource = pAction->GetSourceDep();

				if (!RemoveDepEdgeI((BldFileRegHandle)pfrsSource, frhDep))
					return FALSE;

                // Remove source deps originally added as missing/scanned deps. 
				BldFileRegHandle frhSrcDep;
				pfrsSource->InitFrhEnum();
				while ((frhSrcDep = pfrsSource->NextFrh()) != (BldFileRegHandle)NULL)
				{
					if (pAction->GetScannedDep()->RegHandleExists(frhSrcDep))
					{
						if (!RemoveDepEdgeI(frhSrcDep, frhDep))
						{
							frhSrcDep->ReleaseFRHRef();
							return FALSE;
						}
					}

					frhSrcDep->ReleaseFRHRef();
				}
 			}

			// 
			// add each of the inputs to this output
			BldFileRegHandle frhIn;
			CBldFileRegSet* pfrsInput = pAction->GetInput(pEC);

			CBldAction* pEdgeAction = fWithAction ? pAction : (CBldAction *)NULL;
	
			pfrsInput->InitFrhEnum();

			// remove out input->output dep. first
			if (!fAdd)
			{
				while ((frhIn = pfrsInput->NextFrh()) != (BldFileRegHandle)NULL)
				{
					if (!RemoveDepEdgeI(frhIn, frhDep, pEdgeAction, TRUE /* propagate */))
					{
						frhIn->ReleaseFRHRef();
						return FALSE;
					}
					frhIn->ReleaseFRHRef();
				}
			}

			// can we find this output in a dep. set?
			CDepGrNode* pgrnDep;

			// find action for the dependents
			CVCPtrArray aryDeps;
			if (FindDep(frhDep, pgrnDep, &aryDeps, TRUE))
			{
				int i = aryDeps.GetSize();
				while (i > 0)
				{
					pgrnDep = (CDepGrNode *)aryDeps.GetAt(--i);
					if (pgrnDep->CParents())
					{
						// action is the 1st edge of our 1st parent
						pgrnDep = (CDepGrNode *)pgrnDep->Parent(0);

						CBldAction* pActionDep = (CBldAction *)pgrnDep->Edge(0);

						if (pActionDep != (CBldAction *)NULL)
						{
							// make this a source dep.
							if (fAdd)
							{
								if (!pActionDep->AddSourceDep(frhDep, pEC))
									return FALSE;
							}
							// can we find this output in the source dep. set?
							// remove as a source dep. if it originated from the
							// scanned or missing dep. otherwise leave it, we
							// didn't add it as a source dep. originally!
							else if( pActionDep->GetSourceDep()->RegHandleExists(frhDep) &&
								pActionDep->GetScannedDep()->RegHandleExists(frhDep))
							{
								if (!pActionDep->RemoveSourceDep(frhDep, pEC))
									return FALSE;
							}
						}
					}
				}
			}

			// add our input->output dep. last
	 		if (fAdd)
			{
				while ((frhIn = pfrsInput->NextFrh()) != (BldFileRegHandle)NULL)
				{
					if (!AddDepEdgeI(frhIn, frhDep, pEdgeAction))
					{
						frhIn->ReleaseFRHRef();
						return FALSE;
					}
					frhIn->ReleaseFRHRef();
				}
			}

			// add scanned/missing dependencies (en masse) to this output
			// (*only* for non-target tools, ie. ones that scan!)
			if (fAdd && HandleDependencyInfo(pAction))
			{
				CBldFileRegSet* pfrsScanned = pAction->GetScannedDep();

				if (!AddDepEdgeI((BldFileRegHandle)pfrsScanned, frhDep))
					return FALSE;

				// make sure we this set calc's attributes!
				(void)pfrsScanned->CalcAttributes();

				CBldFileRegSet* pfrsSource = pAction->GetSourceDep();

				if (!AddDepEdgeI((BldFileRegHandle)pfrsSource, frhDep))
					return FALSE;

                // Add scanned/missing deps that are really source deps
				BldFileRegHandle frhSrcDep;
				pfrsSource->InitFrhEnum();
				while ((frhSrcDep = pfrsSource->NextFrh()) != (BldFileRegHandle)NULL)
				{
					if (pAction->GetScannedDep()->RegHandleExists(frhSrcDep))
					{
						if (!AddDepEdgeI(frhSrcDep, frhDep))
						{
							frhSrcDep->ReleaseFRHRef();
							return FALSE;
						}
					}
					frhSrcDep->ReleaseFRHRef();
				}
			}

			break;
		}

		case DEP_Scanned:
			break;	// no nothing

		default:
			break;	// no nothing
	}

	return TRUE;	// done 
}

// add a dependency relationship
// o 'frhDpcy' is the file(s) we are dependent on
// o 'frhDpnt is the dependent file(s)
// o 'pActionSlob' is the action that should be performed to
//    make the dependent 'current' wrt the dependency
BOOL CBldFileDepGraph::AddDepEdgeI(BldFileRegHandle frhDpcy, BldFileRegHandle frhDpnt, 
	CBldAction * pAction /* = (CBldAction *)NULL*/)
{
	// ignore the joining the same files (this is a no-op)
	if (frhDpcy == frhDpnt)
		return TRUE;	// pretend that we've done it

	// get a node for the dependency and one for the dependent, else create one....
	CDepGrNode *pgrnDepcy, *pgrnDepnt;
	if (!FindDep(frhDpcy, pgrnDepcy))
	{
		// create and place in graph
		pgrnDepcy = new CDepGrNode(this, m_pregistry, frhDpcy);
		RETURN_ON_NULL2(pgrnDepcy, FALSE);
		InsertNode(pgrnDepcy);
#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
		{
			if (frhDpcy && frhDpcy->IsNodeType(CBldFileRegNotifyRx::nodetypeRegFile))
			{
				const CPathW* pPath = ((CBldFileRegFile*)frhDpcy)->GetFilePath();
				CStringA strName;
				if (pPath)
					strName = pPath->GetFullPath();
				ATLTRACE("\tCreating dep node 0x%x for file %s\n", pgrnDepcy, strName);
			}
			else
				ATLTRACE("\tCreating dep node 0x%x for reg handle 0x%x\n", pgrnDepcy, frhDpcy);
		}
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS

		// add to our list of sets?
		CBldFileRegEntry* preg = m_pregistry->GetRegEntry(frhDpcy);
		if (preg && preg->IsNodeType(CBldFileRegNotifyRx::nodetypeRegSet))
		{
			VSASSERT(m_lstDepSets.Find(preg) == (VCPOSITION)NULL, "Dependency set already there!");
			m_lstDepSets.AddTail(preg);
		}
	}

	if (!FindDep(frhDpnt, pgrnDepnt))
	{
		// create and place in graph
		pgrnDepnt = new CDepGrNode(this, m_pregistry, frhDpnt);
		RETURN_ON_NULL2(pgrnDepnt, FALSE);
		InsertNode(pgrnDepnt);
#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
		{
			if (frhDpnt && frhDpnt->IsNodeType(CBldFileRegNotifyRx::nodetypeRegFile))
			{
				const CPathW* pPath = ((CBldFileRegFile*)frhDpnt)->GetFilePath();
				CStringA strName;
				if (pPath)
					strName = pPath->GetFullPath();
				ATLTRACE("\tCreating dep node 0x%x for file %s\n", pgrnDepnt, strName);
			}
			else
				ATLTRACE("\tCreating dep node 0x%x for reg handle 0x%x\n", pgrnDepnt, frhDpnt);
		}
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS

		// add to our list of sets?
		CBldFileRegEntry* preg = m_pregistry->GetRegEntry(frhDpnt);
		if (preg && preg->IsNodeType(CBldFileRegNotifyRx::nodetypeRegSet))
		{
			VSASSERT(m_lstDepSets.Find(preg) == (VCPOSITION)NULL, "Dependency set already there!");
			m_lstDepSets.AddTail(preg);
		}
	}

	// connect dependency (child) <- dependant (parent) with the action (if one exists) as the edge

	// make sure we haven't already done this
	for (USHORT iChild = pgrnDepnt->CChildren(); iChild > 0; iChild--)
	{
		if (pgrnDepcy == pgrnDepnt->Child(iChild-1))
			return TRUE;	// already performed addition
	}

	pgrnDepnt->AddChild(pgrnDepcy, (EID)pAction);
#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
	ATLTRACE("\t\tDep node 0x%x is a child of dep node 0x%x\n", pgrnDepcy, pgrnDepnt);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS

    // Now need to propagate dep state to new node!!!
    if (pgrnDepcy->Depstate() & (DS_OutOfDate | DS_ForcedOutOfDate))
    {
        UINT depstate = pgrnDepnt->Depstate();
        if ((depstate & DS_ForcedOutOfDate) == 0)
            pgrnDepnt->SetDepstate((depstate & ~DS_Current) | DS_ForcedOutOfDate);
    }
    
	return TRUE;	// success
}

// remove a dependency relationship
BOOL CBldFileDepGraph::RemoveDepEdgeI(BldFileRegHandle frhDpcy, BldFileRegHandle frhDpnt,
	CBldAction * pAction /* = (CBldAction *)NULL*/, BOOL fAllDepnts /* = FALSE */)
{
	// get a node for the dependency and one for the dependent, and delete them
	// (automatic removal and disconnection from graph...)
	CDepGrNode *pgrnDepcy, *pgrnDepnt;
	if (!FindDep(frhDpcy, pgrnDepcy) || !FindDep(frhDpnt, pgrnDepnt))
		return TRUE;	// already removed....

		// ignore the joining the same files (this is a no-op)
	if (frhDpcy == frhDpnt)
	{
		// We may need to remove this node if this is joining the same files and the node
		// has no other children (but maybe some parents)
		if (pgrnDepnt->CChildren() == 0)
		{
			// Remove any parents
			for (USHORT iParent = pgrnDepcy->CParents(); iParent > 0; iParent--)
			{
				pgrnDepnt = (CDepGrNode *)pgrnDepcy->Parent(iParent - 1);
 				RemoveDepEdgeI(frhDpcy, pgrnDepnt->Frh());
 			}
		}

		return TRUE;	// pretend that we've done it
	}

	// disconnect dependency
	for (USHORT iChild = pgrnDepnt->CChildren(); iChild > 0; iChild--)
	{
		// search for this
		if (pgrnDepcy == pgrnDepnt->Child(iChild - 1))
		{
			// delete child
			pgrnDepnt->DelChild(iChild - 1);
			break;
		}
	}

	// delete dependency if no longer connected
	if (pgrnDepcy->CParents() == 0 && pgrnDepcy->CChildren() == 0)
	{
		// remove from our list of sets
		CBldFileRegEntry* preg = m_pregistry->GetRegEntry(frhDpcy);
		if (preg->IsNodeType(CBldFileRegNotifyRx::nodetypeRegSet))
		{
			VCPOSITION pos = m_lstDepSets.Find(preg);
			VSASSERT(pos != (VCPOSITION)NULL, "Dependency set not there!");
			m_lstDepSets.RemoveAt(pos);
		}

		delete pgrnDepcy;
	}

	// Remove if we don't have any parents or children OR if fAllDepnts is TRUE, remove from any 
	// parents we have that don't have an action that uses us and we don't have any children.
	if (fAllDepnts && pgrnDepnt->CParents() != 0 && pgrnDepnt->CChildren() == 0)
	{
		// For each parent, find the reference for pgrnDepnt, and check if the
		// associated edge is NULL.
		for (USHORT iParent = pgrnDepnt->CParents(); iParent > 0; iParent--)
		{
			CDepGrNode* pgrnParent = (CDepGrNode *)pgrnDepnt->Parent(iParent - 1);

			// Loop though children of our parent to find ourselves.
			CBldAction* pParentAction = NULL;
			for (USHORT iChild = pgrnParent->CChildren(); iChild > 0; iChild--)
			{
				if ((CDepGrNode *)pgrnParent->Child(iChild - 1) == pgrnDepnt )
				{
					pParentAction = (CBldAction*)pgrnParent->Edge(iChild - 1);
					break;
				}
			}
			if (NULL != pParentAction)
				continue;

			if (!RemoveDepEdgeI(frhDpnt, pgrnParent->Frh()))
				return FALSE;
		}
	}
	else
	{
	 	if (pgrnDepnt->CParents() == 0 && pgrnDepnt->CChildren() == 0)
		{
			// remove from our list of sets
			CBldFileRegEntry* preg = m_pregistry->GetRegEntry(frhDpnt);
			if (preg->IsNodeType(CBldFileRegNotifyRx::nodetypeRegSet))
			{
				VCPOSITION pos = m_lstDepSets.Find(preg);
				VSASSERT(pos != (VCPOSITION)NULL, "Dependency set not there!");
				m_lstDepSets.RemoveAt(pos);
			}

			delete pgrnDepnt;
		}
	}

	return TRUE;	// success
}

BOOL CBldFileDepGraph::FindDep(BldFileRegHandle frhDep, CDepGrNode*& pgrn, CVCPtrArray* parygrn,
	BOOL fSearchSets /* = FALSE */)
{
	BOOL fFound = FALSE;

	// can we find it as a 'single' node?
	if (LookupNode((NID)frhDep, (CGrNode*&)pgrn))
		return !fSearchSets;	// found... return whether found in a set

	// search our sets?
	if (fSearchSets)
	{
		VCPOSITION pos = m_lstDepSets.GetHeadPosition();
		while (pos != (VCPOSITION)NULL)
		{
			CBldFileRegSet * pfrsSet = (CBldFileRegSet *)m_lstDepSets.GetNext(pos);
			if (pfrsSet->RegHandleExists(frhDep) && FindDep((BldFileRegHandle)pfrsSet, pgrn, parygrn))
			{
				fFound = TRUE;

				// found one, make a note and .. carry on searching?
				if (parygrn == (CVCPtrArray *)NULL)
					break;	// no!

				parygrn->Add(pgrn);
			}
		}
	}

	return fFound;	// not found
}

void CBldFileDepGraph::StartPasses()
{
	// use a different action marker than the last time we did the passes
	CBldAction::m_wActionMarker++;

	// clear our 'cannot build' list
	m_lstCannotBuild.RemoveAll();
}

// what actions 'contribute' to the dependent?
// o 'plstActions' is a list of actions
// o 'frhStart' is the dependent to start from
// return CMD_ value
UINT CBldFileDepGraph::RetrieveOutputActions(CBldActionList* plstActions, IVCBuildErrorContext* pEC, BldFileRegHandle frhStart)
{
	VSASSERT(frhStart != (BldFileRegHandle)NULL, "Starting frh cannot be NULL!");

	// look for this in our graph
	CDepGrNode* pgrn;
	if (!FindDep(frhStart, pgrn))
		return CMD_Error;

	// remove existing ones
	plstActions->Clear();

	for (USHORT iChild = pgrn->CChildren(); iChild > 0; iChild--)
	{
		CBldAction * pAction = (CBldAction *)pgrn->Edge(iChild-1);
		if (pAction != (CBldAction *)NULL)
			plstActions->AddToEnd(pAction);
	}

	return CMD_Complete;
}

// what actions are performed on the dependencies with a state in 'stateFilter' (default is 'not current')?
// o 'frhStart' is the dependent to start from (default is 'do all out of date')
CMD CBldFileDepGraph::RetrieveBuildActions(CBldCfgRecord* pcrBuild, CBldActionList* plstActions, CVCPtrList& lstFrh,
 	IVCBuildErrorContext* pEC, CDynamicBuildEngine* pBldEngine, BldFileRegHandle frhStart /* = (BldFileRegHandle)NULL*/,
	UINT stateFilter /* = DS_OutOfDate */, UINT aor /* = AOR_Default */, BOOL bCheckTimeStamps /* = TRUE */)
{	
    // for each of these nodes construct a list of actions, don't visit nodes twice
    // which may happen given multiple starts
    ClearVisited();
	CBldFileRegFile::UpdateTimeStampTick();		// this makes it so we don't look up the time stamp on something multiple times

	CMD cmdRet = CMD_Complete;

	VSASSERT(pcrBuild != NULL, "CfgRecord is required parameter for determining build info");
	CStringW strIdb;
	if (pcrBuild != NULL)
	{
		VCConfiguration* pProjCfg = pcrBuild->GetVCConfiguration();
		CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = pProjCfg;
		VSASSERT(spProjCfgImpl != NULL, "CfgRecord improperly initialized");
		CComBSTR bstrIdb;
		if (spProjCfgImpl != NULL)
			spProjCfgImpl->get_MrePath(&bstrIdb);
		strIdb = bstrIdb;
	}

	// Minimal rebuild dependency interface.
	CBldMreDependenciesContainer mreDepend(strIdb);

	// nodes to start from?
	CVCPtrList lstStartNodes;
	if (frhStart == (BldFileRegHandle)NULL)
	{
		// any roots?
		USHORT iRoot = CRoots();
		if (iRoot == 0)
			return CMD_Complete;

		for (; iRoot > 0; iRoot--)
		{
			cmdRet = EnumerateBuildActionsI(plstActions, lstFrh, pEC, pBldEngine, (CDepGrNode *)Root(iRoot-1), stateFilter, aor,
				bCheckTimeStamps);
			if (cmdRet != CMD_Complete)
				goto ExitRetrieve;
		}
	}
	else
	{
		// find this node?
		CDepGrNode* pgrnDep;
		if (!FindDep(frhStart, pgrnDep))
		{
			// couldn't find the start node
			cmdRet = CMD_Error;
			goto ExitRetrieve;
		}

		cmdRet = EnumerateBuildActionsI(plstActions, lstFrh, pEC, pBldEngine, pgrnDep, stateFilter, aor, bCheckTimeStamps);
		if (cmdRet != CMD_Complete)
			goto ExitRetrieve;
	}

ExitRetrieve:
	// return command code
	return cmdRet;
}

BOOL CBldFileDepGraph::HandleDependencyInfo(CBldAction* pAction)
{
	return (!pAction->BuildToolWrapper()->IsTargetTool(pAction) || pAction->BuildToolWrapper()->HasPrimaryOutput() 
		|| pAction->BuildToolWrapper()->CustomToolType());
}

BOOL g_bBatchBuildInProgress;
CMD CBldFileDepGraph::EnumerateBuildActionsI(CBldActionList* plstActions, CVCPtrList& lstFrh, IVCBuildErrorContext* pEC, 
	CDynamicBuildEngine* pBldEngine, CDepGrNode* pgrn, UINT stateFilter, UINT& aor, BOOL bCheckTimeStamps)
{
	// make sure we have up-to-date timestamps prior to getting the dependency state
	if (!pgrn->m_fVisited && bCheckTimeStamps && m_pregistry)
	{
		BldFileRegHandle frh = m_pregistry->GetRegEntry(pgrn->Frh());
		if (frh)
			frh->UpdateAttributes();
	}

	if (pBldEngine && pBldEngine->m_bUserCanceled)
	{
		pBldEngine->InformUserCancel();
		return CMD_Canceled;
	}

	// visited
	pgrn->m_fVisited = TRUE;

	ULONG cChildren = pgrn->CChildren();
	if (cChildren == 0)
		return CMD_Complete;

	int cVisited = 0;

	// See if dependencies need to be updated.
    CBldAction* pAction = NULL;
	// the action associated with this node may not be in the first child, but it is always either first or third...
	for (ULONG idx = 0; idx < cChildren && idx < 3; idx++)	
	{
		if (pgrn->Edge(idx) != (EID)NULL)
		{
			pAction = (CBldAction *)pgrn->Edge(idx);
			break;
		}
	}

	if (NULL != pAction)
	{
		if (!pAction->IsDepInfoInitialized())
			pAction->UpdateDepInfo(pEC);
	}

	for (USHORT iChild = cChildren; iChild > 0; iChild--)
	{
		CDepGrNode* pgrnChild = (CDepGrNode *)pgrn->Child(iChild-1);

		// enumerate actions of our child?
		// o recursive and child not visited (depth first, post-order)
		if ((aor & AOR_Recurse) && !pgrnChild->m_fVisited)
		{
			CMD cmdRet = EnumerateBuildActionsI(plstActions, lstFrh, pEC, pBldEngine, pgrnChild, stateFilter, aor, bCheckTimeStamps);
			if (cmdRet != CMD_Complete)
				return cmdRet;

			cVisited++;
		}
	}


	// our dependency state check
	// o dependent state is a subset of our filter?
	UINT depState = pgrn->Depstate();

	if ((depState & DS_ForcedOutOfDate) != 0)
	{
		if ((stateFilter & DS_ForcedOutOfDate) != 0)
		{
			// REVIEW(kperry) just because we want find out what to build, we shouldn't
			//				  set it back in date !
			pgrn->SetDepstate(depState & ~DS_ForcedOutOfDate);
			pgrn->FigureDepState();
			depState = pgrn->Depstate() | DS_ForcedOutOfDate;
		}
		else if (pAction != NULL)
		{
			UINT oldState = depState;
			pgrn->SetDepstate(depState & ~DS_ForcedOutOfDate);
			pgrn->FigureDepState();
			depState = ((oldState & stateFilter) == 0) ? pgrn->Depstate() : oldState;
			pgrn->SetDepstate(oldState); // restore old state
		}
	}

	// do we have an action we might want to add?
	if (pAction == (CBldAction *)NULL)
		return CMD_Complete;
 
	BOOL fAddAction = ((depState & stateFilter) != 0);

	// have we already retrieved this?
	if (pAction->m_wMark == CBldAction::m_wActionMarker)
	{
		// be aggressive here, mark our parents as likely to be out of date
		if (fAddAction)
		{
			AddEnumeratedActionFile(pgrn, lstFrh);
#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
			ATLTRACE("\tAdding action for dep node 0x%x with dep state %d and filter %d\n", pgrn, depState, stateFilter);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS
		}
		return CMD_Complete;
	}

#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
	if (fAddAction)
		ATLTRACE("\t(1) Will be adding action for dep node 0x%x, depstate = %d, filter = %d\n", pgrn, depState, stateFilter);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS

	// not adding actions?
	// explain why?
	if (!fAddAction)
	{
		// dependency missing....and no actions that may generate them!
		if (pgrn->Depstate() & DS_DepMissing)
		{
			BOOL fIgnDepMissing = (pgrn->Depstate() & ~stateFilter) == DS_DepMissing;	// ignore missing dependency?

			for (USHORT iChild = pgrn->CChildren(); iChild > 0; iChild--)
			{
				CBldAction* pEdge = (CBldAction *)pgrn->Edge(iChild-1);
				if (pEdge != (CBldAction *)NULL)
				{
					CDepGrNode* pgrnChild = (CDepGrNode *)pgrn->Child(iChild-1);
					CBldFileRegEntry* pregChild = m_pregistry->GetRegEntry(pgrnChild->Frh());

					// not exist and an action?
					if (!pregChild->ExistsOnDisk(FALSE))
					{
						// don't ignore this missing dependency
						fIgnDepMissing = FALSE;
						break;
					}
				}
			}

			// ignore missing dependency in the case that we are *only* missing dependencies 
			// for non-action relationships, eg. scanned deps.
			fAddAction = fIgnDepMissing;
		}
	}

#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
	if (fAddAction)
		ATLTRACE("\t(2) Will be adding action for dep node 0x%x, depstate = %d, filter = %d\n", pgrn, depState, stateFilter);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS

	// refresh the command-line options
	
	// anything changed?
	BOOL fOptsChnged;
	BOOL fRefreshOk = pAction->RefreshCommandOptions(fOptsChnged, (aor & AOR_PreChk) != 0);
	BOOL bHaveMissingDeps = pAction->HaveMissingDeps();

	// o check for options changes and have they possibly changed?
	if ((aor & AOR_ChkOpts) && ((fRefreshOk && fOptsChnged) || bHaveMissingDeps))
	{
		if (!fAddAction)
		{
			// no longer bothering to prompt; we'll *always* rebuild if dirty
			fAddAction = TRUE;
#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
			ATLTRACE("\tAdding action for dep node 0x%x due to option change %d or missing deps %d\n", 
				pgrn, fOptsChnged, bHaveMissingDeps);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS

			// Delete the outputs to this action so that we will rebuild this if something goes wrong, 
			// as the change in build settings is lost after the first attempted build
			// Please note that only the *first* output is being deleted below.  This is enough
			// to get the action to rerun next time we build, but doesn't get rid of *all* dirty outputs.
			if (!bHaveMissingDeps && !pAction->BuildToolWrapper()->IsTargetTool(pAction) && !(aor & AOR_PreChk))
			{
				CBldFileRegSet* pOutput = pAction->GetOutput(pEC);
				CBldFileRegEntry* frh = (CBldFileRegEntry *)pOutput->GetFirstFrh();
				if (frh != NULL)
				{
					frh->DeleteFromDisk();
					frh->ReleaseFRHRef();
#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
					ATLTRACE("\tDeleting output for dep node 0x%x\n", pgrn);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS
				}
			}
		}

		// if the action we just marked out of date happens to have more than one output (such as if it was
		// a MIDL action or a custom build rule with multiple outputs), then we need to figure out what the
		// other parents of the out-of-date child are also out-of-date.
		for (USHORT iChild = pgrn->CChildren(); iChild > 0; iChild--)
		{
			CBldAction* pTmpAction = NULL;
			CDepGrNode* pgrnChild = (CDepGrNode *)pgrn->Child(iChild-1);

			// check the other parents of the children
			for (USHORT iParent = pgrnChild->CParents(); iParent > 0; iParent--)
			{
				CDepGrNode* pgrnParent = (CDepGrNode *)pgrnChild->Parent(iParent-1);
				if (pgrnParent == pgrn)	// don't recheck the original parent
					continue;

				for (ULONG idx = 0; idx < pgrnParent->CChildren(); idx++)	// the action associated with this node may not be in the first child..,
				{
					if (pgrnParent->Edge(idx) != (EID)NULL)
					{
						if (pAction == (CBldAction *)pgrnParent->Edge(idx))
						{
#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
							ATLTRACE("\tAdding action for dep node 0x%x due to out-of-date child 0x%x\n", 
									pgrnParent, pgrn);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS
							AddEnumeratedActionFile(pgrnParent, lstFrh);
							break;
						}
					}
				}
			}
		}
	}

	// add the action now?
	if (fAddAction)
	{
		// make sure we don't attempt to retrieve it again
		pAction->m_wMark = CBldAction::m_wActionMarker;
		AddEnumeratedActionFile(pgrn, lstFrh);
#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
		ATLTRACE("\tAdding action for dep node 0x%x, depstate = %d, filter = %d\n", pgrn, pgrn->Depstate(), stateFilter);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS
	}

	// not adding actions, yet attempted to build all we can?
	// explain why?
	if ((!(aor & AOR_Recurse) || (cVisited && !plstActions->NotEmpty())) && (aor & AOR_Verbose))
	{
		// dependency missing....and no actions that may generate them!
		if (pgrn->Depstate() & DS_DepMissing)
		{
			CStringW strMsg;	// message buffer
			for (USHORT iChild = pgrn->CChildren(); iChild > 0; iChild--)
			{
				CDepGrNode* pgrnChild = (CDepGrNode *)pgrn->Child(iChild-1);
				CBldFileRegEntry* pregChild = m_pregistry->GetRegEntry(pgrnChild->Frh());

				// exist?
				if (pregChild->ExistsOnDisk(FALSE))
					continue;
				// special -- Custom Build Events. These are not on disk
				if(pregChild->IsNodeType(CBldFileRegNotifyRx::nodetypeRegFile) && ((CBldFileRegFile*)pregChild)->m_bFileNotOnDisk)
					continue;

				// loop through deps. and warn about deps. that are missing
				if (pregChild->IsNodeType(CBldFileRegNotifyRx::nodetypeRegSet))
				{
					// report source deps. (headers) that are missing
					BldFileRegHandle frh;
					((CBldFileRegSet *)pregChild)->InitFrhEnum();
					while ((frh = ((CBldFileRegSet *)pregChild)->NextFrh()) != (BldFileRegHandle)NULL)
					{
						CBldFileRegFile* pregFile = (CBldFileRegFile *)m_pregistry->GetRegEntry(frh);
						if (!pregFile->ExistsOnDisk(FALSE))
						{
							// REVIEW:: this is O(n^3) but only called if build is going to fail anyway
							// Should change to a map.
							if (!m_lstCannotBuild.Find(pregFile))
							{
								::VCMsgTextW(strMsg, IDS_CANNOT_BUILD_DEP_PRJ0010, (const wchar_t *)*pregFile->GetFilePath());
								CVCProjectEngine::AddProjectError(pEC, strMsg, L"PRJ0010", pBldEngine->GetAssociatedCfgRecord());
								m_lstCannotBuild.AddTail(pregFile);
							}
						}
						frh->ReleaseFRHRef();
					}
				}
				else if (pgrnChild->CChildren() == 0)
				{
					// report file that we don't know how to or failed to build
					if (!m_lstCannotBuild.Find(pregChild))
					{
						::VCMsgTextW(strMsg, IDS_CANNOT_BUILD_DEP_PRJ0010, (const wchar_t *)*pregChild->GetFilePath());
						CVCProjectEngine::AddProjectError(pEC, strMsg, L"PRJ0010", pBldEngine->GetAssociatedCfgRecord());
						m_lstCannotBuild.AddTail(pregChild);
					}
				}
			}
		}
	}

	// construct our list of dependencies for when we were last attempted to be built?
	if (pgrn->m_fDirtyDepLst)
		pgrn->m_fDirtyDepLst = FALSE;	// dependencies at last build are now clean

	// transfer the actions to the 'master' list versions
	if (fAddAction)
		plstActions->AddToEnd(pAction);
		
	return CMD_Complete;	// ok
}

void CBldFileDepGraph::AddEnumeratedActionFile(CDepGrNode* pgrn, CVCPtrList& lstFrh)
{
	// add the file that caused the action
	lstFrh.AddTail(pgrn->Frh());

	// be aggressive here, mark our parents as likely to be out of date
	for (USHORT iParent = pgrn->CParents(); iParent > 0; iParent--)
	{
		CDepGrNode* pgrnParent = (CDepGrNode *)pgrn->Parent(iParent-1);
		
		UINT depState = pgrnParent->Depstate();

		// if current, then force out of date
		if ((depState & DS_OutOfDate) == 0 || (depState & DS_ForcedOutOfDate) == 0)
			pgrnParent->SetDepstate((depState & ~DS_Current) | DS_ForcedOutOfDate);
	}
}

CMD CBldFileDepGraph::RetrieveDeploymentDependencies(CBldCfgRecord* pcrBuild, IVCBuildStringCollection** ppDeployStrings)
{	
    // for each of these nodes construct a list of actions, don't visit nodes twice
    // which may happen given multiple starts
    ClearVisited();
	CBldFileRegFile::UpdateTimeStampTick();		// this makes it so we don't look up the time stamp on something multiple times

	CMD cmdRet = CMD_Complete;

	VSASSERT(pcrBuild != NULL, "CfgRecord is required parameter for determining build info");
	CStringW strIdb;
	if (pcrBuild != NULL)
	{
		VCConfiguration* pProjCfg = pcrBuild->GetVCConfiguration();
		CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = pProjCfg;
		VSASSERT(spProjCfgImpl != NULL, "CfgRecord improperly initialized");
		CComBSTR bstrIdb;
		if (spProjCfgImpl != NULL)
			spProjCfgImpl->get_MrePath(&bstrIdb);
		strIdb = bstrIdb;
	}

	// Minimal rebuild dependency interface.
	CBldMreDependenciesContainer mreDepend(strIdb);

	// any roots?
	USHORT iRoot = CRoots();
	if (iRoot == 0)
		return CMD_Complete;

	for (; iRoot > 0; iRoot--)
	{
		cmdRet = EnumerateDeploymentDependenciesI((CDepGrNode *)Root(iRoot-1), ppDeployStrings);
		if (cmdRet != CMD_Complete)
			return cmdRet;
	}

	return cmdRet;
}

CMD CBldFileDepGraph::EnumerateDeploymentDependenciesI(CDepGrNode* pgrn, IVCBuildStringCollection** ppDeployStrings)
{	
	// visited
	pgrn->m_fVisited = TRUE;

	ULONG cChildren = pgrn->CChildren();
	if (cChildren == 0)
		return CMD_Complete;

	int cVisited = 0;

	// See if dependencies need to be updated.
    CBldAction* pAction = NULL;
	// the action associated with this node may not be in the first child, but it is always either first or third...
	for (ULONG idx = 0; idx < cChildren && idx < 3; idx++)	
	{
		if (pgrn->Edge(idx) != (EID)NULL)
		{
			pAction = (CBldAction *)pgrn->Edge(idx);
			break;
		}
	}

	if (NULL != pAction)
	{
		if (!pAction->IsDepInfoInitialized())
			pAction->UpdateDepInfo(NULL);
	}

	for (USHORT iChild = cChildren; iChild > 0; iChild--)
	{
		CDepGrNode* pgrnChild = (CDepGrNode *)pgrn->Child(iChild-1);

		// enumerate actions of our child?
		// o recursive and child not visited (depth first, post-order)
		if (!pgrnChild->m_fVisited)
		{
			CMD cmdRet = EnumerateDeploymentDependenciesI(pgrnChild, ppDeployStrings);
			if (cmdRet != CMD_Complete)
				return cmdRet;

			cVisited++;
		}
	}

	RETURN_ON_NULL2(pAction, CMD_Complete);

	// have we already retrieved this?
	if (pAction->m_wMark != CBldAction::m_wActionMarker)
	{
		pAction->GetDeploymentDependencies(ppDeployStrings);
		pAction->m_wMark = CBldAction::m_wActionMarker;
	}
	
	return CMD_Complete;	// ok
}

// CNidGraphDoc implementation
USHORT CBldFileDepGraph::CChildrenInitWalk(NID nidNode)
{
	m_iChild = 0;
	m_pgrnChild = (CGrNode *)nidNode;

	// don't count dependency sets
	int iChild = 0, cChild = 0;
	for (; iChild < m_pgrnChild->CChildren(); iChild++)	
	{
		CDepGrNode* pgrn = (CDepGrNode *)m_pgrnChild->Child(iChild);
		CBldFileRegEntry* preg = g_FileRegistry.GetRegEntry(pgrn->Frh());
		if (!m_fShowDepSets && preg->IsNodeType(CBldFileRegNotifyRx::nodetypeRegSet))
			continue;

		cChild++;
	}

	return cChild;
}

BOOL CBldFileDepGraph::FNextNid(NID & nid, ET & et)
{
NextNidChild:

	if (m_iChild >= m_pgrnChild->CChildren())
		return FALSE;

	CDepGrNode* pgrn = (CDepGrNode *)m_pgrnChild->Child(m_iChild);
	CBldFileRegEntry* preg = g_FileRegistry.GetRegEntry(pgrn->Frh());
	if (!m_fShowDepSets && preg->IsNodeType(CBldFileRegNotifyRx::nodetypeRegSet))
	{
		m_iChild++;
		goto NextNidChild;
	}

	nid = (NID)m_pgrnChild->Child(m_iChild);
	et = m_pgrnChild->Edge(m_iChild++);
	return TRUE;
}
