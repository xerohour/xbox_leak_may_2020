//
// CGrNode, CGraph, CDepGraph
//
// Graph node, dependency graph node, graph and dependency graph classes
//
// [matthewt]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "depgraph.h"	// our local header
#include "engine.h"		// CActionSlob
#include "mrdepend.h"
#include "toolsdlg.h"

IMPLEMENT_DYNAMIC(CGraph, CObject)
IMPLEMENT_DYNAMIC(CFileDepGraph, CGraph)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// FRHArray thin array

FRHArray::FRHArray( int n ){
	m_pDepcy = new FileRegHandle[n]; 
	m_nFRH = n;
}

void FRHArray::Replace( int n ){ 
	delete []m_pDepcy;
	m_pDepcy = new FileRegHandle[n]; 
	m_nFRH = n;
}


//////////////////////////////////
// CGrNode - generic graph node	//
//////////////////////////////////

#ifdef USE_LOCAL_BUILD_HEAP
// graph node local heap
CLclHeap CGrNode::g_heapNodes(50);
#endif

// granularity of allocations
#define C_ALLOC_PARENT	2
#define C_ALLOC_CHILD	12
#define C_ALLOC_ROOT	12
#define C_ALLOC_EDGES	C_ALLOC_CHILD

#ifdef _KIPDEBUG
	int CGrNode::nCount = 0;
#endif

// constructor+destructor
CGrNode::CGrNode(CGraph * pgraph, NID nid, USHORT cChildren /* = 0 */, USHORT iDepth /* = 0 */)
{
	// this graph node's identifier
	m_nid = nid;

	// this graph node's depth
#ifdef DEPGRAPH_VIEW
	m_iDepth = iDepth;
#endif // DEPGRAPH_VIEW

	// initialise
	m_rgChildren = m_rgParents = (CGrNode * *)NULL;
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
    	// m_fCleared = FALSE;

#ifdef DEPGRAPH_VIEW
	m_pgraph = pgraph;
#endif

	m_GrNfyRx = NULL;

	// pre-allocate some children?
	if (m_cChildren)
	{
		m_allocChildren += m_cChildren;

#ifdef USE_LOCAL_BUILD_HEAP
		m_rgChildren = (CGrNode * *)g_heapNodes.alloc(sizeof(CGrNode *) * m_allocChildren);
		m_rgEdges = (EID *)g_heapNodes.alloc(sizeof(EID) * m_allocChildren);
#else
		m_rgChildren = new CGrNode *[m_allocChildren];
		m_rgEdges = new EID[m_allocChildren];
#endif
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
#ifdef USE_LOCAL_BUILD_HEAP
	if (m_rgParents)	g_heapNodes.dealloc(m_rgParents);
	if (m_rgChildren)	g_heapNodes.dealloc(m_rgChildren);
	if (m_rgEdges)		g_heapNodes.dealloc(m_rgEdges);
#else
	if (m_rgParents)	delete []m_rgParents;
	if (m_rgChildren)	delete []m_rgChildren;
	if (m_rgEdges)		delete []m_rgEdges;
#endif

}

#ifdef _DEBUG
void CGrNode::AssertValid() const
{
	// validate our storage boundaries
	ASSERT(m_cParents >= 0 && m_cChildren >= 0);

	// validate our storage
	ASSERT(m_rgParents == (CGrNode * *)NULL ||
		   AfxIsValidAddress((const void *)m_rgParents, m_cParents * sizeof(CGrNode *)));

	ASSERT(m_rgChildren == (CGrNode * *)NULL ||
		   AfxIsValidAddress((const void *)m_rgChildren, m_cChildren * sizeof(CGrNode *)));
}

void CGrNode::Dump(CDumpContext & dc) const
{
	// dump node info.
	DumpNid(dc);

	// dump linkage info.
	dc << "Count children : " << m_cChildren << " : Count parents : " << m_cParents << "\r\n";
}						    

void CGrNode::DumpNid(CDumpContext & dc) const
{
	// dump basic node identifier info.
	dc << "NID : " << m_nid << "\r\n";
}
#endif

// add or remove graph node event receivers
void CGrNode::AddGrNfyRx(CGrNfyRx * pgrnfyrx)
{	
	ASSERT( m_GrNfyRx == NULL );
	m_GrNfyRx = pgrnfyrx;
}

void CGrNode::RemoveGrNfyRx(CGrNfyRx * pgrnfyrx)
{
#ifdef _DEBUG
	if( m_GrNfyRx != NULL ){
		ASSERT( m_GrNfyRx == pgrnfyrx );
	}
#endif
	m_GrNfyRx = NULL;
}

void CGrNode::InformGrNfyRx(UINT grevt, void * pvhint)
{
	if( m_GrNfyRx == NULL ) return;
	m_GrNfyRx->OnGrNfy(grevt, this, pvhint);
}

void CGrNode::AddChild(CGrNode * pgrn, EID eid /* = (EID)0 */)
{
	//
	// add a new child to this node
	//

	// allocate our new array of children?
	BOOL fRealloc = m_cChildren >= m_allocChildren;

	CGrNode * * rgChildren = m_rgChildren; EID * rgEdges = m_rgEdges;
	
	// re-alloc?
	if (fRealloc)
	{
		m_allocChildren += C_ALLOC_CHILD;

#ifdef USE_LOCAL_BUILD_HEAP
		rgChildren = (CGrNode * *)g_heapNodes.alloc(sizeof(CGrNode *) * m_allocChildren);
		rgEdges = (EID *)g_heapNodes.alloc(sizeof(EID) * m_allocChildren);
#else
		rgChildren = new CGrNode *[m_allocChildren];
		rgEdges = new EID[m_allocChildren];
#endif

		// move current children
		memcpy(rgChildren, m_rgChildren, m_cChildren * sizeof(CGrNode *));
		memcpy(rgEdges, m_rgEdges, m_cChildren * sizeof(EID));
	}
	 
	// insert child + edge into child + edge list
	rgChildren[m_cChildren] = pgrn;
	rgEdges[m_cChildren] = eid;

	// remember our new array?
	if (fRealloc)
	{

#ifdef USE_LOCAL_BUILD_HEAP
		if (m_rgChildren) g_heapNodes.dealloc(m_rgChildren);
		if (m_rgEdges) g_heapNodes.dealloc(m_rgEdges);
#else
		if (m_rgChildren) delete []m_rgChildren;
		if (m_rgEdges) delete []m_rgEdges;
#endif

		m_rgChildren = rgChildren;
		m_rgEdges = rgEdges;
	}

	// increment our child count
	m_cChildren++;

	// inform
	InformGrNfyRx(GrEvt_AddChild, (void *)pgrn->Nid()); 
	InformGrNfyRx(GrEvt_AddEdge, (void *)eid); 

	//
	// add ourselves as a parent of this node
	//

	// allocate our new array of parents for it?
	fRealloc = pgrn->m_cParents >= pgrn->m_allocParents;
 	CGrNode * * rgParents = pgrn->m_rgParents;
	
	if (fRealloc)
	{
		pgrn->m_allocParents += C_ALLOC_PARENT;

#ifdef USE_LOCAL_BUILD_HEAP
		rgParents = (CGrNode * *)g_heapNodes.alloc(sizeof(CGrNode *) * pgrn->m_allocParents);
#else
		rgParents = new CGrNode *[pgrn->m_allocParents];
#endif

		// move current parents
		memcpy(rgParents, pgrn->m_rgParents, pgrn->m_cParents * sizeof(CGrNode *));
	}

	// insert ourselves as a parent
	rgParents[pgrn->m_cParents] = this;

	// remember our new array?
	if (fRealloc)
	{
#ifdef USE_LOCAL_BUILD_HEAP
		if (pgrn->m_rgParents) g_heapNodes.dealloc(pgrn->m_rgParents);
#else
		if (pgrn->m_rgParents) delete []pgrn->m_rgParents;
#endif

		pgrn->m_rgParents = rgParents;
	}

	// increment their parent count
	pgrn->m_cParents++;

	// inform
	pgrn->InformGrNfyRx(GrEvt_AddParent, (void *)this->Nid());
}

void CGrNode::DelChild(USHORT iChild)
{
	//
	// remove a child (refrenced by index)
	//

	// ensure within our bounds
	ASSERT(m_cChildren > 0 && iChild < m_cChildren);

	// remember the child before removal
	CGrNode * pgrn = m_rgChildren[iChild];
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
	InformGrNfyRx(GrEvt_DelEdge, (void *)eid); 

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
		ASSERT(fFound);

		// decrement their parent count
		pgrn->m_cParents--;

		// inform
		pgrn->InformGrNfyRx(GrEvt_DelParent, (void *)this);
	}
}

//////////////////////////////////
// CGrNode - generic graph node	//
//////////////////////////////////

CDepGrNode::CDepGrNode(CFileDepGraph * pgraph, CFileRegistry * pregistry, FileRegHandle frh)
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
	m_pregistry->GetRegEntry(Frh())->AddNotifyRx(this);

	if (!m_fIgnoreDepcy)
		m_frsDepcy.AddNotifyRx(this);

	// dependency list at last build attempt is not dirty
	m_fDirtyDepLst = FALSE;

	// add a reference to this registry entry
#ifndef REFCOUNT_WORK
	g_FileRegistry.AddRegRef(Frh());
#else
	Frh()->AddFRHRef();
#endif

#ifdef _DEBUG_BLD
	TCHAR * pchDep;
	CFileRegEntry * preg = m_pregistry->GetRegEntry(Frh());

	if (preg->IsNodeType(CFileRegNotifyRx::nodetypeRegSet))
		pchDep = "set";
	else
		pchDep = (TCHAR *)(const TCHAR *)*preg->GetFilePath();

	TRACE("\r\nCreate <%s> (%lx) in %lx", pchDep, this, m_pregistry);
#endif // _DEBUG_BLD
}

CDepGrNode::~CDepGrNode()
{
	// inform that we are being deleted
	InformGrNfyRx(GrEvt_Destroy);

#ifdef _DEBUG_BLD
	TCHAR * pchDep;
	CFileRegEntry * preg = m_pregistry->GetRegEntry(Frh());

	if (preg->IsNodeType(CFileRegNotifyRx::nodetypeRegSet))
		pchDep = "set";
	else
		pchDep = (TCHAR *)(const TCHAR *)*preg->GetFilePath();

	TRACE("\r\nDelete <%s> (%lx) in %lx", pchDep, this, m_pregistry);
 #endif // _DEBUG_BLD

	// remove ourselves as a Rx of file change events
	// of both the dependent and dependency
	m_pregistry->GetRegEntry(Frh())->RemoveNotifyRx(this);
	
	if (!m_fIgnoreDepcy)
		m_frsDepcy.RemoveNotifyRx(this);

	// assert that we don't have any dependencies
#ifndef REFCOUNT_WORK
	ASSERT(m_frsDepcy.GetContent()->IsEmpty());
#else
	ASSERT(m_frsDepcy.IsEmpty());
#endif

	// remove a reference to this registry entry
#ifndef REFCOUNT_WORK
	g_FileRegistry.ReleaseRegRef(Frh());
#else
	Frh()->ReleaseFRHRef();
#endif
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
				CDepGrNode * pgrnDep = (CDepGrNode *)Child(iChild - 1);
				m_frsDepcy.AddRegHandle(pgrnDep->Frh());
			}

			// turn back on incr. calc. of attributes
			(void) m_frsDepcy.CalcAttributes();

			// update our dependency state
			FigureDepState();

			// add ourselves as a dependency now
			m_frsDepcy.AddNotifyRx(this);
		}

		// dependency list at last build attempt is dirty   
		m_fDirtyDepLst = TRUE;

		m_fIgnoreDepcy = (USHORT)fIgnoreDepcy;
	}
}

// Rx for file change events
void CDepGrNode::OnNotify(FileRegHandle hndFileReg, UINT idChange, DWORD dwHint)
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

#ifdef _DEBUG

#ifndef REFCOUNT_WORK
		// local destroy?
		case FRN_LCL_DESTROY:
			if ((CFileRegistry *)dwHint != m_pregistry)
				break;

			// destroying ourselves?
			ASSERT(hndFileReg != Frh());
			break;

		// global destroy
		case FRN_DESTROY:

			// destroying ourselves?
			ASSERT(hndFileReg != Frh());
			break;
#endif

#endif

		default:
			break;
	}
}

// Rx for changes in dep. connections
void CDepGrNode::InformGrNfyRx(UINT grevt, void * pvhint)
{
	// which type of dep. connection event?
	switch (grevt)
	{
		// changing what are dependant on us?
		case DepGrEvt_AddDpnt:
		case DepGrEvt_DelDpnt:
		{
			break;
		}

		// changing what we depend on?
		case DepGrEvt_AddDpcy:
		case DepGrEvt_DelDpcy:
			
			// do we want to do this now?
			if (!m_fIgnoreDepcy)
			{
				if (grevt == DepGrEvt_AddDpcy)
					m_frsDepcy.AddRegHandle((FileRegHandle)pvhint);
				else
					m_frsDepcy.RemoveRegHandle((FileRegHandle)pvhint);

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
#ifndef REFCOUNT_WORK
	if (!m_frsDepcy.GetContent()->IsEmpty())
#else
	if (!m_frsDepcy.IsEmpty())
#endif
	{
		BOOL bVal;

		//
		// one of our deps. missing?
		//
		if (m_frsDepcy.GetIntProp(P_ExistsOnDisk, bVal) && !bVal)				
			newstate |= DS_DepMissing;

		// If we are forced out of date then ignore current/out of date stuff
		if ((m_state & DS_ForcedOutOfDate)==0)
		{
			//
			// compare deps. most recent timestamp to our timstamp
			//
		 	CFileRegEntry * pentry = m_pregistry->GetRegEntry(Frh());
			if (pentry->GetIntProp(P_ExistsOnDisk, bVal) && bVal)
			{
				FILETIME ftimeDep, ftimeOur;
				if (m_frsDepcy.GetTimeProp(P_NewTStamp, ftimeDep) &&
					m_pregistry->GetRegEntry(Frh())->GetTimeProp(P_NewTStamp, ftimeOur))
				{
#ifdef FUZZY95
					if (CFileRegistry::MyFuzzyCompareFileTime(&ftimeDep, &ftimeOur) <= 0)
#else
					if (CFileRegistry::MyCompareFileTime(&ftimeDep, &ftimeOur) <= 0)
#endif
						newstate |= DS_Current;
					else
						newstate |= DS_OutOfDate;
				}
			}
			else
			{
				newstate |= DS_OutOfDate;
			}
		}
	}

#ifdef _DEBUG	// this test moved above
	// If we are forced out of date then ignore current/out of date stuff
	if (m_state & DS_ForcedOutOfDate)
	{
		ASSERT((newstate & (DS_Current | DS_OutOfDate))==0);
	}
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
#ifdef _DEBUG
	if (g_buildengine.m_dwDebugging & DBG_BS_GRP)
	{
		// our state has changed, dump
		DumpNid(afxDump);
	}
#endif

#ifdef BUILD_SYSTEM
	TRACE("\r\n>> Dep State Changed for %s << now ", Frh()->GetFilePath()->GetFullPath() );
	if( m_state & DS_Current )
		TRACE(" %s ", "DS_Current" );
	if( m_state & DS_OutOfDate )
		TRACE(" %s ", "DS_OutOfDate" );
	if( m_state & DS_ForcedOutOfDate )
		TRACE(" %s ", "DS_ForcedOutOfDate" );
	if( m_state & DS_DepMissing )
		TRACE(" %s ", "DS_DepMissing" );
#endif

    ASSERT((m_state & (DS_Current | DS_OutOfDate)) != (DS_Current | DS_OutOfDate));
    ASSERT((m_state & (DS_Current | DS_ForcedOutOfDate)) != (DS_Current | DS_ForcedOutOfDate));

    // If we are out of date then propagate changes to parents
    if (m_state & (DS_OutOfDate | DS_ForcedOutOfDate))
    {
        // Propagate changes to next layer down!!
        for (USHORT iParent = CParents(); iParent > 0; iParent--)
        {
            CDepGrNode * pgrnParent = (CDepGrNode *)Parent(iParent-1);

            UINT depstate = pgrnParent->Depstate();

            // if not out of date, then force out of date
            if ((depstate & ( DS_OutOfDate | DS_ForcedOutOfDate ) ) == 0)
            {
                // Set dep state and recurse on parent
                pgrnParent->SetDepstate((depstate & ~DS_Current) | DS_ForcedOutOfDate); 
                // pgrnParent->OnDepStateChanged((depstate & ~DS_Current) | DS_ForcedOutOfDate);
            }
        }
    }
}

#ifdef _DEBUG
void CDepGrNode::DumpNid(CDumpContext & dc) const
{
	// dump filereghandle info.
	CFileRegEntry * pentry = m_pregistry->GetRegEntry(((CDepGrNode *)this)->Frh());
	ASSERT(pentry != (CFileRegEntry *)NULL);
	pentry->Dump(dc);

	// dump state
	dc << "Dep. State : ";
	UINT state = m_state;
	if (state == DS_Unknown)
	{
		dc << "unknown";
	}
	else
	{
		BOOL fNeedAnd = FALSE;
		while (state != 0)
		{
			if (fNeedAnd)
				dc << " and ";

			if (state & DS_OutOfDate)
			{
				dc << "out of date";
				state &= ~DS_OutOfDate; 
			}
			else
			if (state & DS_DepMissing)
			{
				dc << "dependency missing";
				state &= ~DS_DepMissing; 
			}
			else
			if (state & DS_Current)
			{
				dc << "current";
				state &= ~DS_Current; 
			}
			else
			if (state & DS_ForcedOutOfDate)
			{
				dc << "forced-out";
				state &= ~DS_ForcedOutOfDate; 
			}

			fNeedAnd = TRUE;
		}
	}
	dc << "\r\n";
}
#endif
 
///////////////////////////////////////
// CGraph - generic graph 'protocol' //
///////////////////////////////////////

CGraph::CGraph() :
	m_mapNodes(991)	// increase default hash table size to allow large graphs.
{
	// initialise
	m_rgRoots = (CGrNode * *)NULL;
	m_allocRoots = 0;

	m_cRoots = 0;
	m_mode = Graph_Stable;
}

CGraph::~CGraph()
{
	// delete our roots (we shouldn't have any)
	ASSERT(!m_cRoots);

#ifdef _DEBUG
	if (m_cRoots != 0)
		Dump(afxDump);
#endif

	// delete our storage
#ifdef USE_LOCAL_BUILD_HEAP
	while (m_cRoots > 0)
		CGrNode::g_heapNodes.dealloc(m_rgRoots[--m_cRoots]);
	if (m_rgRoots)	CGrNode::g_heapNodes.dealloc(m_rgRoots);
#else
	int cRoots = m_cRoots; // bug fix
	while (cRoots > 0)
	{
		delete m_rgRoots[--cRoots];
	}
	ASSERT(m_cRoots == 0); // m_cRoots decremented inside d'tor!
	if (m_rgRoots)	delete []m_rgRoots;
#endif
}

#ifdef _DEBUG
void CGraph::Dump(CGrNode * pgrn, int iDepth, CDumpContext & dc) const
{
	// dump node + children
	pgrn->Dump(dc);
	for (USHORT iChild = 0; iChild < pgrn->CChildren(); iChild++)
	{
		// dump depth
		dc << "Depth : " << iDepth;
		Dump(pgrn->Child(iChild), iDepth + 1, dc);
		dc << "\r\n";
	}
}

void CGraph::Dump(CDumpContext & dc) const
{
	for (USHORT iRoot = 0; iRoot < m_cRoots; iRoot++)
	{
		// dump roots
		dc << "Root " << iRoot << " : ";
		Dump(m_rgRoots[iRoot], 0, dc);
		dc << "\r\n\r\n";
	}
} 
#endif

// CNidGraphDoc implementation
USHORT CGraph::CRootsInitWalk()
{
	m_iRoot = 0;
	return CRoots();
}

BOOL CGraph::FNextRoot(NID & nid)
{
	if (m_iRoot >= CRoots())
		return FALSE;

	nid = (NID)Root(m_iRoot++);
	return TRUE;
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
void CGraph::InsertNode(CGrNode * pgrn)
{
#ifdef _DEBUG
	void * pvDummy;
#endif
	ASSERT(!m_mapNodes.Lookup((void *)pgrn->Nid(), pvDummy));

	// make sure we are informed of changes
	pgrn->AddGrNfyRx(this);

	// is this a root?
	if (pgrn->CParents() == 0)
		AddRoot(pgrn);
 	 
	// remember this
	m_mapNodes.SetAt((void *)pgrn->Nid(), pgrn);

#ifdef _DEBUG
	if (g_buildengine.m_dwDebugging & DBG_BS_GRP)
	{
		TRACE("\r\n>> Insert node <<"); pgrn->Dump(afxDump);
	}
#endif
}

void CGraph::RemoveNode(CGrNode * pgrn)
{
#ifdef _DEBUG
	void * pvDummy;
#endif
	ASSERT(m_mapNodes.Lookup((void *)pgrn->Nid(), pvDummy));

	// is this a root?
	if (pgrn->CParents() == 0)
		DelRoot(pgrn);

	// make sure we are no longer informed
	pgrn->RemoveGrNfyRx(this);

	// don't remember this
	m_mapNodes.RemoveKey((void *)pgrn->Nid());

#ifdef _DEBUG
	if (g_buildengine.m_dwDebugging & DBG_BS_GRP)
	{
		TRACE("\r\n>> Remove node <<"); pgrn->Dump(afxDump);
	}
#endif
}

void CGraph::AddRoot(CGrNode * pgrn)
{
	ASSERT(pgrn->CParents() == 0);

	// allocate our new array of children?
	BOOL fRealloc = m_cRoots >= m_allocRoots;
	CGrNode * * rgRoots = m_rgRoots;

	// re-alloc?
	if (fRealloc)
	{
		m_allocRoots += C_ALLOC_ROOT;

#ifdef USE_LOCAL_BUILD_HEAP
		rgRoots = (CGrNode * *)CGrNode::g_heapNodes.alloc(sizeof(CGrNode *) * m_allocRoots);
#else
		rgRoots = new CGrNode *[m_allocRoots];
#endif

		// move our current roots
		memcpy(rgRoots, m_rgRoots, m_cRoots * sizeof(CGrNode *));
	}

	// insert this root
	rgRoots[m_cRoots] = pgrn;

	// remember our new array?
	if (fRealloc)
	{
#ifdef USE_LOCAL_BUILD_HEAP
		if (m_rgRoots) CGrNode::g_heapNodes.dealloc(m_rgRoots);
#else
		if (m_rgRoots) delete []m_rgRoots;
#endif

		m_rgRoots = rgRoots;
	} 

	// increment our root count
	m_cRoots++;

#ifdef _DEBUG
	if (g_buildengine.m_dwDebugging & DBG_BS_GRP)
	{
		TRACE("\r\n>> Add root <<"); pgrn->Dump(afxDump);
	}
#endif
}

void CGraph::DelRoot(CGrNode * pgrn)
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
	ASSERT(fFound);

	// decrement the root count
	m_cRoots--;

#ifdef _DEBUG
	if (g_buildengine.m_dwDebugging & DBG_BS_GRP)
	{
		TRACE("\r\n>> Delete root <<"); pgrn->Dump(afxDump);
	}
#endif
}

#if 0
void CGraph::ClearCleared(CGrNode * pgrn)
{
    if (pgrn->m_fCleared)
    {
        pgrn->m_fCleared = FALSE;
		for (USHORT iChild = pgrn->CChildren(); iChild > 0; iChild--)
			ClearCleared(pgrn->Child(iChild-1));
    }
}

void CGraph::ClearVisited(CGrNode * pgrn /* = (CGrNode *)NULL */)
{
	if (pgrn == (CGrNode *) NULL)
	{
		for (USHORT iRoot = CRoots(); iRoot > 0; iRoot--)
			ClearVisited(Root(iRoot-1));

		for (iRoot = CRoots(); iRoot > 0; iRoot--)
			ClearCleared(Root(iRoot-1));
	}
	else
	{
        if (pgrn->m_fCleared)
            return;

		pgrn->m_fVisited = FALSE;
        pgrn->m_fCleared = TRUE;
		for (USHORT iChild = pgrn->CChildren(); iChild > 0; iChild--)
			ClearVisited(pgrn->Child(iChild-1));
	}
} 
#else
// although originally writen to recursivly clear flags, this routine was only
// used to clear the enbtire dep graph. It should only be used that way.
void CGraph::ClearVisited( void )
{
	void *p;
	void *pVal;
	CGrNode * pgrn;

	// iterate over all nodes in this graph. flipping the visited bit to false.
	POSITION pos = m_mapNodes.GetStartPosition();
	while( pos != (POSITION)NULL ) {
		m_mapNodes.GetNextAssoc( pos, p, pVal );
		pgrn = (CGrNode *)pVal;
		pgrn->m_fVisited = FALSE;
	}
}
#endif


// Rx for graph node change events
void CGraph::OnGrNfy(UINT grevt, CGrNode * pgrn, void * pvhint)
{
	switch (grevt)
	{
		case GrEvt_AddParent:
			// no longer a root? 
			if (pgrn->CParents() == 1)
				DelRoot(pgrn);
#ifdef _DEBUG
			if (g_buildengine.m_dwDebugging & DBG_BS_GRP)
			{
				TRACE("\r\n>> Add parent to <<"); pgrn->Dump(afxDump);
				TRACE("\r\n>>            of <<"); ((CGrNode *)pvhint)->Dump(afxDump);
			}
#endif
			break;

		case GrEvt_DelParent:
			// become a root?
			if (pgrn->CParents() == 0)
				AddRoot(pgrn);

#ifdef _DEBUG
			if (g_buildengine.m_dwDebugging & DBG_BS_GRP)
			{
				TRACE("\r\n>> Delete parent from <<"); pgrn->Dump(afxDump);
				TRACE("\r\n>>                 of <<"); ((CGrNode *)pvhint)->Dump(afxDump);
			}
#endif
 			break;

		case GrEvt_AddChild:
#ifdef _DEBUG
			if (g_buildengine.m_dwDebugging & DBG_BS_GRP)
			{
				TRACE("\r\n>> Add child to <<"); pgrn->Dump(afxDump);
				TRACE("\r\n>>           of <<"); ((CGrNode *)pvhint)->Dump(afxDump);
			}
#endif
			break;

		case GrEvt_DelChild:

#ifdef _DEBUG
			if (g_buildengine.m_dwDebugging & DBG_BS_GRP)
			{
				TRACE("\r\n>> Delete child from <<"); pgrn->Dump(afxDump);
				TRACE("\r\n>>                of <<"); ((CGrNode *)pvhint)->Dump(afxDump);
			}
#endif
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
	POSITION pos = m_mapNodes.GetStartPosition();
	while (pos != (POSITION)NULL)
	{
		void * key; CGrNode * pgrn;
		m_mapNodes.GetNextAssoc(pos, key, (void * &)pgrn); 
		pgrn->OnGraphMode(m_mode);
	}

	return oldmode;	// return old mode
}

BOOL CGraph::PerformOperationI
(
	GraphOpFn pfn,
	DWORD & dw,
	UINT order,
	CGrNode * pgrn
)
{
 	// visited
	pgrn->m_fVisited = TRUE;

	ASSERT(pfn);
	ASSERT(pgrn != (CGrNode *)NULL);

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
			CGrNode * pgrnChild = pgrn->Child(iChild);
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

BOOL CGraph::PerformOperation
(
	GraphOpFn pfn,
	DWORD & dw,
	UINT order /* = PrePerform */,
	NID nid /* = nidNil */
)
{
	ASSERT(pfn);

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
		CGrNode * pgrn;
		if (!LookupNode(nid, pgrn) ||
			!PerformOperationI(pfn, dw, order, pgrn))
			return FALSE;	// failed to find node or perform op.
	}

	return TRUE;	// ok
}

///////////////////////////////////////////
// Default graph ops
///////////////////////////////////////////

///////////////////////////////////////////
// CFileDepGraph - file dependency graph //
///////////////////////////////////////////

CFileDepGraph::CFileDepGraph(CFileRegistry * pregistry)
{
	// assign our registry
	m_pregistry = pregistry;

	// don't show dep. sets by default
	m_fShowDepSets = FALSE;
}

CFileDepGraph::~CFileDepGraph()
{
}

BOOL CFileDepGraph::ModifyDep
(
	FileRegHandle frhDep,
	CActionSlob * pAction,
	UINT dep,
	BOOL fAdd,
	BOOL fWithAction /* = TRUE */
)
{
	// what kind of dep.?
	switch (dep)
	{
		case DEP_Input:
		case DEP_Source:
		{
			// 
			// add this input or source dep. to each of the outputs
			//
			FileRegHandle frhOut;
			CFileRegSet * pfrsOutput = pAction->GetOutput();
			CWordArray * paryOutputAttrib = pAction->GetOutputAttrib();
			int i = 0;

			pfrsOutput->InitFrhEnum();
			while ((frhOut = pfrsOutput->NextFrh()) != (FileRegHandle)NULL)
			{
				BOOL fWithAction = !!paryOutputAttrib->GetAt(i++);
				CActionSlob * pEdgeAction = fWithAction ? pAction : (CActionSlob *)NULL;

				if (fAdd)
				{
					if (!AddDepEdgeI(frhDep, frhOut, pEdgeAction))
#ifndef REFCOUNT_WORK
						return FALSE;
#else
					{
						frhOut->ReleaseFRHRef();
						return FALSE;
					}
#endif
				}
				else
				{
					if (!RemoveDepEdgeI(frhDep, frhOut, pEdgeAction, TRUE /* propagate */))
#ifndef REFCOUNT_WORK
						return FALSE;
#else
					{
						frhOut->ReleaseFRHRef();
						return FALSE;
					}
#endif
				}
#ifdef REFCOUNT_WORK
				frhOut->ReleaseFRHRef();
#endif
			}
		
			break;
		}

		case DEP_Output:
		{
			//
			// remove scanned/source/missing dependencies (en masse) to this output
			//
			// (*only* for non-target tools, ie. ones that scan!)

			if (!fAdd && !pAction->m_pTool->m_fTargetTool)
			{
				CFileRegSet * pfrsScanned = pAction->GetScannedDep();

				if (!RemoveDepEdgeI((FileRegHandle)pfrsScanned, frhDep, (CActionSlob *)NULL, TRUE /* propagate */))
					return FALSE;

				// this set doesn't need to calc attributes!
				// OLYMPUS 13658: However we do need to calc attributes if this set is
				// used by some other action, i.e it has some parents after removing the edge above.
				// So we can only switch off calc of attributes when the set has 0 parents.
                CDepGrNode * pgrnDepcy;
				if (FindDep(pfrsScanned, pgrnDepcy))
				{
					if (pgrnDepcy->CParents() == 0)
						pfrsScanned->CalcAttributes(FALSE);

				}

				CFileRegSet * pfrsSource = pAction->GetSourceDep();

				if (!RemoveDepEdgeI((FileRegHandle)pfrsSource, frhDep))
					return FALSE;

				CFileRegSet * pfrsMissing = pAction->GetMissingDep();

				if (!RemoveDepEdgeI((FileRegHandle)pfrsMissing, frhDep, (CActionSlob *)NULL, TRUE /* propagate */))
					return FALSE;

                // Remove source deps originally added as missing/scanned
                // deps. 
				FileRegHandle frhSrcDep;
				pfrsSource->InitFrhEnum();
				while ((frhSrcDep = pfrsSource->NextFrh()) != (FileRegHandle)NULL)
#ifndef REFCOUNT_WORK
					if (pAction->GetMissingDep()->RegHandleExists(frhSrcDep) ||
						pAction->GetScannedDep()->RegHandleExists(frhSrcDep))
						if (!RemoveDepEdgeI(frhSrcDep, frhDep))
							return FALSE;
#else
				{
					if (pAction->GetMissingDep()->RegHandleExists(frhSrcDep) ||
						pAction->GetScannedDep()->RegHandleExists(frhSrcDep))
					{
						if (!RemoveDepEdgeI(frhSrcDep, frhDep))
						{
							frhSrcDep->ReleaseFRHRef();
							return FALSE;
						}
					}

					frhSrcDep->ReleaseFRHRef();
				}
#endif
 			}

			//
			// 
			// add each of the inputs to this output
			//
			FileRegHandle frhIn;
			CFileRegSet * pfrsInput = pAction->GetInput();

			CActionSlob * pEdgeAction = fWithAction ? pAction : (CActionSlob *)NULL;
	
			pfrsInput->InitFrhEnum();

			//
			// remove out input->output dep. first
			//
			if (!fAdd)
			{
				while ((frhIn = pfrsInput->NextFrh()) != (FileRegHandle)NULL)
#ifndef REFCOUNT_WORK
					if (!RemoveDepEdgeI(frhIn, frhDep, pEdgeAction, TRUE /* propagate */))
						return FALSE;
#else
				{
					if (!RemoveDepEdgeI(frhIn, frhDep, pEdgeAction, TRUE /* propagate */))
					{
						frhIn->ReleaseFRHRef();
						return FALSE;
					}
					frhIn->ReleaseFRHRef();
				}
#endif
			}

			//
			// can we find this output in a dep. set?
			//
			CDepGrNode * pgrnDep;

			// find action for the dependents
			CPtrArray aryDeps;
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

						CActionSlob * pActionDep = (CActionSlob *)pgrnDep->Edge(0);
						// ASSERT(pActionDep != (CActionSlob *)NULL);

						if (pActionDep != (CActionSlob *)NULL)
						{

							//
							// make this a source dep.
							//
							if (fAdd)
							{
								if (!pActionDep->AddSourceDep(frhDep))
									return FALSE;
							}
							//
							// can we find this output in the source dep. set?
							// remove as a source dep. if it originated from the
							// scanned or missing dep. otherwise leave it, we
							// didn't add it as a source dep. originally!
							//
							else
							if (pActionDep->GetSourceDep()->RegHandleExists(frhDep) &&
								(pActionDep->GetMissingDep()->RegHandleExists(frhDep) ||
								pActionDep->GetScannedDep()->RegHandleExists(frhDep))
								)
							{
								if (!pActionDep->RemoveSourceDep(frhDep))
									return FALSE;
							}
						}
					}
				}
			}

			//
			// add our input->output dep. last
			//
	 		if (fAdd)
			{
				while ((frhIn = pfrsInput->NextFrh()) != (FileRegHandle)NULL)
#ifndef REFCOUNT_WORK
					if (!AddDepEdgeI(frhIn, frhDep, pEdgeAction))
						return FALSE;
#else
				{
					if (!AddDepEdgeI(frhIn, frhDep, pEdgeAction))
					{
						frhIn->ReleaseFRHRef();
						return FALSE;
					}
					frhIn->ReleaseFRHRef();
				}
#endif
			}

			//
			// add scanned/missing dependencies (en masse) to this output
			//
			// (*only* for non-target tools, ie. ones that scan!)
			//
			if (fAdd && !pAction->m_pTool->m_fTargetTool)
			{
				CFileRegSet * pfrsScanned = pAction->GetScannedDep();

				if (!AddDepEdgeI((FileRegHandle)pfrsScanned, frhDep))
					return FALSE;

				// make sure we this set calc's attributes!
				(void)pfrsScanned->CalcAttributes();

				CFileRegSet * pfrsSource = pAction->GetSourceDep();

				if (!AddDepEdgeI((FileRegHandle)pfrsSource, frhDep))
					return FALSE;

				CFileRegSet * pfrsMissing = pAction->GetMissingDep();

				if (!AddDepEdgeI((FileRegHandle)pfrsMissing, frhDep))
					return FALSE;

                // Add scanned/missing deps that are really source deps
				FileRegHandle frhSrcDep;
				pfrsSource->InitFrhEnum();
				while ((frhSrcDep = pfrsSource->NextFrh()) != (FileRegHandle)NULL)
#ifndef REFCOUNT_WORK
					if (pAction->GetMissingDep()->RegHandleExists(frhSrcDep) ||
						pAction->GetScannedDep()->RegHandleExists(frhSrcDep))
						if (!AddDepEdgeI(frhSrcDep, frhDep))
							return FALSE;
#else
				{
					if (pAction->GetMissingDep()->RegHandleExists(frhSrcDep) ||
						pAction->GetScannedDep()->RegHandleExists(frhSrcDep))
					{
						if (!AddDepEdgeI(frhSrcDep, frhDep))
						{
							frhSrcDep->ReleaseFRHRef();
							return FALSE;
						}
					}
					frhSrcDep->ReleaseFRHRef();
				}
#endif
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
// o 'frhDpcy' is the file(s) we are dependant on
// o 'frhDpnt is the dependent file(s)
// o 'pActionSlob' is the action that should be performed to
//    make the dependant 'current' wrt the dependency
BOOL CFileDepGraph::AddDepEdgeI
(
	FileRegHandle frhDpcy,
	FileRegHandle frhDpnt,
	CActionSlob * pAction /* = (CActionSlob *)NULL*/
)
{
	// ignore the joining the same files (this is a no-op)
	if (frhDpcy == frhDpnt)
		return TRUE;	// pretend that we've done it

#if 0
#pragma message( "Check for library as input" )
	if (frhDpcy->IsNodeType(CFileRegNotifyRx::nodetypeRegFile))
		ASSERT( 0 != _tcsicmp(frhDpcy->GetFilePath()->GetExtension(), ".lib" ) );
#endif

	// get a node for the dependency and one for the dependent,
	// else create one....
	CDepGrNode * pgrnDepcy, * pgrnDepnt;
	if (!FindDep(frhDpcy, pgrnDepcy))
	{
		// create and place in graph
		pgrnDepcy = new CDepGrNode(this, m_pregistry, frhDpcy);
		InsertNode(pgrnDepcy);

		// add to our list of sets?
		CFileRegEntry * preg = m_pregistry->GetRegEntry(frhDpcy);
		if (preg->IsNodeType(CFileRegNotifyRx::nodetypeRegSet))
		{
			ASSERT(m_lstDepSets.Find(preg) == (POSITION)NULL);
			m_lstDepSets.AddTail(preg);
		}
	}

	if (!FindDep(frhDpnt, pgrnDepnt))
	{
		// create and place in graph
		pgrnDepnt = new CDepGrNode(this, m_pregistry, frhDpnt);
		InsertNode(pgrnDepnt);

		// add to our list of sets?
		CFileRegEntry * preg = m_pregistry->GetRegEntry(frhDpnt);
		if (preg->IsNodeType(CFileRegNotifyRx::nodetypeRegSet))
		{
			ASSERT(m_lstDepSets.Find(preg) == (POSITION)NULL);
			m_lstDepSets.AddTail(preg);
		}
	}

	// connect dependency (child) <- dependant (parent)
	// with the action (if one exists) as the edge

	// make sure we haven't already done this
	for (USHORT iChild = pgrnDepnt->CChildren(); iChild > 0; iChild--)
	{
		if (pgrnDepcy == pgrnDepnt->Child(iChild-1))
			return TRUE;	// already performed addition
	}

#ifdef _DEBUG_BLD
	// trace this
	TCHAR * pchDepcy, * pchDepnt;
	CFileRegEntry * preg;
	
	preg = m_pregistry->GetRegEntry(frhDpcy);
	if (preg->IsNodeType(CFileRegNotifyRx::nodetypeRegSet))
		pchDepcy = "set";
	else
		pchDepcy = (TCHAR *)(const TCHAR *)*preg->GetFilePath();

 	preg = m_pregistry->GetRegEntry(frhDpnt);
	if (preg->IsNodeType(CFileRegNotifyRx::nodetypeRegSet))
		pchDepnt = "set";
	else
		pchDepnt = (TCHAR *)(const TCHAR *)*preg->GetFilePath();

	TRACE("\r\nAdd <%s>:%lx (%lx) to <%s>:%lx (%lx) in %lx", pchDepcy, frhDpcy, pgrnDepcy, pchDepnt, frhDpnt, pgrnDepnt, m_pregistry);
#endif // _DEBUG_BLD

	pgrnDepnt->AddChild(pgrnDepcy, (EID)pAction);

    // Now need to propagate dep state to new node!!!
    if (pgrnDepcy->Depstate() & (DS_OutOfDate | DS_ForcedOutOfDate))
    {
        UINT depstate = pgrnDepnt->Depstate();

        if ((depstate & DS_ForcedOutOfDate) == 0)
        {
            pgrnDepnt->SetDepstate((depstate & ~DS_Current) | DS_ForcedOutOfDate);
            // pgrnDepnt->OnDepStateChanged((depstate & ~DS_Current) | DS_ForcedOutOfDate); 
        }
    }
    
	return TRUE;	// success
}

// remove a dependency relationship
BOOL CFileDepGraph::RemoveDepEdgeI
(
	FileRegHandle frhDpcy,
	FileRegHandle frhDpnt,
	CActionSlob * pAction /* = (CActionSlob *)NULL*/,
	BOOL fAllDepnts /* = FALSE */
)
{
	// get a node for the dependency and one for the dependent,
	// and delete them
	// (automatic removal and disconnection from graph...)
	CDepGrNode * pgrnDepcy, * pgrnDepnt;
	if (!FindDep(frhDpcy, pgrnDepcy) ||
		!FindDep(frhDpnt, pgrnDepnt))
	{
		// already removed....
		return TRUE;
	}

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

#ifdef _DEBUG_BLD
	// trace this
	TCHAR * pchDepcy, * pchDepnt;
	CFileRegEntry * preg;

	preg = m_pregistry->GetRegEntry(frhDpcy);
	if (preg->IsNodeType(CFileRegNotifyRx::nodetypeRegSet))
		pchDepcy = "set";
	else
		pchDepcy = (TCHAR *)(const TCHAR *)*preg->GetFilePath();

 	preg = m_pregistry->GetRegEntry(frhDpnt);
	if (preg->IsNodeType(CFileRegNotifyRx::nodetypeRegSet))
		pchDepnt = "set";
	else
		pchDepnt = (TCHAR *)(const TCHAR *)*preg->GetFilePath();

	TRACE("\r\nRemove <%s>:%lx (%lx) from <%s>:%lx (%lx) in %lx", pchDepcy, frhDpcy, pgrnDepcy, pchDepnt, frhDpnt, pgrnDepnt, m_pregistry);
 #endif // _DEBUG_BLD

	// delete dependency if no longer connected
	if (pgrnDepcy->CParents() == 0 && pgrnDepcy->CChildren() == 0)
	{
		// remove from our list of sets
		CFileRegEntry * preg = m_pregistry->GetRegEntry(frhDpcy);
		if (preg->IsNodeType(CFileRegNotifyRx::nodetypeRegSet))
		{
			POSITION pos = m_lstDepSets.Find(preg);
			ASSERT(pos != (POSITION)NULL);
			m_lstDepSets.RemoveAt(pos);
		}

		delete pgrnDepcy;
	}

	//
	// Remove if we don't have any parents or children OR
	// If fAllDepnts is TRUE, remove from any parents we have that don't have
	// an action that uses us and we don't have any children.
	//
	if (fAllDepnts && pgrnDepnt->CParents() != 0 && pgrnDepnt->CChildren() == 0)
	{
		//
		// For each parent, find the reference for pgrnDepnt, and check if the
		// associated edge is NULL.
		//
		for (USHORT iParent = pgrnDepnt->CParents(); iParent > 0; iParent--)
		{
			CDepGrNode* pgrnParent = (CDepGrNode *)pgrnDepnt->Parent(iParent - 1);

			//
			// Loop though children of our parent to find ourselves.
			//
			CActionSlob* pParentAction = NULL;
			for (USHORT iChild = pgrnParent->CChildren(); iChild > 0; iChild--)
			{
				if ((CDepGrNode *)pgrnParent->Child(iChild - 1) == pgrnDepnt )
				{
					pParentAction = (CActionSlob*)pgrnParent->Edge(iChild - 1);
					break;
				}
			}
			if (NULL!=pParentAction)
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
			CFileRegEntry * preg = m_pregistry->GetRegEntry(frhDpnt);
			if (preg->IsNodeType(CFileRegNotifyRx::nodetypeRegSet))
			{
				POSITION pos = m_lstDepSets.Find(preg);
				ASSERT(pos != (POSITION)NULL);
				m_lstDepSets.RemoveAt(pos);
			}

			delete pgrnDepnt;
		}
	}

	return TRUE;	// success
}

BOOL CFileDepGraph::FindDep
(
	FileRegHandle frhDep,
	CDepGrNode * & pgrn,	// a CDepGrNode
	CPtrArray * parygrn,	// accumulator of CDepGrNode's
	BOOL fSearchSets		/* = FALSE */
)
{
	BOOL fFound = FALSE;

	// can we find it as a 'single' node?
	if (LookupNode((NID)frhDep, (CGrNode * &)pgrn))
		return !fSearchSets;	// found... return whether found in a set

	// search our sets?
	if (fSearchSets)
	{
		POSITION pos = m_lstDepSets.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CFileRegSet * pfrsSet = (CFileRegSet *)m_lstDepSets.GetNext(pos);
			if (pfrsSet->RegHandleExists(frhDep) &&
				FindDep((FileRegHandle)pfrsSet, pgrn, parygrn)
			   )
			{
				fFound = TRUE;

				// found one, make a note and .. carry on searching?
				if (parygrn == (CPtrArray *)NULL)
					break;	// no!

				parygrn->Add(pgrn);
			}
		}
	}

	return fFound;	// not found
}

void CFileDepGraph::StartPasses()
{
	// use a different action marker than the last time we did the passes
	CActionSlob::m_wActionMarker++;

	// clear our 'cannot build' list
	m_lstCannotBuild.RemoveAll();
}

// what actions 'contribute' to the dependent?
// o 'lstActions' is a list of actions
// o 'frhStart' is the dependant to start from
// return CMD_ value
UINT CFileDepGraph::RetrieveOutputActions
(
	CActionSlobList & lstActions,
 	CErrorContext & EC,
	FileRegHandle frhStart
)
{
	ASSERT(frhStart != (FileRegHandle)NULL);

	// look for this in our graph
	CDepGrNode * pgrn;
	if (!FindDep(frhStart, pgrn))
		return CMD_Error;

	// remove existing ones
	lstActions.RemoveAll();

	for (USHORT iChild = pgrn->CChildren(); iChild > 0; iChild--)
	{
		CActionSlob * pAction = (CActionSlob *)pgrn->Edge(iChild-1);
		if (pAction != (CActionSlob *)NULL)
			lstActions.AddTail(pAction);
	}

	return CMD_Complete;
}

// what actions are performed on the dependencies with a state in 'stateFilter' (default is 'not current')?
// o 'frhStart' is the dependant to start from (default is 'do all out of date')
UINT CFileDepGraph::RetrieveBuildActions
(
	CActionSlobList & lstActions,
	CPtrList & lstFrh,
 	CErrorContext & EC,
	FileRegHandle frhStart /* = (FileRegHandle)NULL*/,
	UINT stateFilter /* = DS_OutOfDate */,
	UINT aor /* = AOR_Default */
)
{	
	// initialise
	m_fPromptedCheckOptions = !(aor & AOR_Verbose);

    // for each of these nodes construct a list of actions, don't visit nodes twice
    // which may happen given multiple starts
    ClearVisited();

	UINT cmdRet = CMD_Complete;

	ASSERT(NULL != g_pActiveProject);
	CPath pathIdb =  
		g_pActiveProject->GetMrePath(g_pActiveProject->GetActiveConfig());

	// Minimal rebuild dependency interface.
	IMreDependencies* pMreDepend = 
		IMreDependencies::GetMreDependenciesIFace((LPCTSTR)pathIdb);

	// nodes to start from?
	CPtrList lstStartNodes;
	if (frhStart == (FileRegHandle)NULL)
	{
		// any roots?
		USHORT iRoot = CRoots();
		if (iRoot == 0)
		{
			if (NULL!=pMreDepend)
				pMreDepend->Release();

			return CMD_Complete;
		}

		for (; iRoot > 0; iRoot--)
		{
			CDepGrNode* pNode = (CDepGrNode *)Root(iRoot-1);
			//
			// Filter out deferred tools.
			//
			if (NULL!=pNode && NULL!=pNode->Edge(0))
			{
				CActionSlob * pAction = (CActionSlob *)pNode->Edge(0);
				DAB dab = pAction->m_pTool->DeferredAction(pAction);
				if (dab == dabDeferred || dab == dabNeverBuild)
				{
					// don't use this one, don't replace
					continue;
				}
			}
			cmdRet = EnumerateBuildActionsI(lstActions, lstFrh, EC, (CDepGrNode *)Root(iRoot-1), stateFilter, aor);
			if (cmdRet != CMD_Complete)
				goto ExitRetrieve;
		}
	}
	else
	{
		// find this node?
		CDepGrNode * pgrnDep;
		if (!FindDep(frhStart, pgrnDep))
		{
			// couldn't find the start node
			cmdRet = CMD_Error;
			goto ExitRetrieve;
		}

		cmdRet = EnumerateBuildActionsI(lstActions, lstFrh, EC, pgrnDep, stateFilter, aor);
		if (cmdRet != CMD_Complete)
			goto ExitRetrieve;
	}

ExitRetrieve:

	if (NULL!=pMreDepend)
		pMreDepend->Release();

	// return command code
	return cmdRet;
}

extern BOOL g_bBatchBuildInProgress;
UINT CFileDepGraph::EnumerateBuildActionsI
(
	CActionSlobList & lstActions,
	CPtrList & lstFrh,
 	CErrorContext & EC,
	CDepGrNode * pgrn,
	UINT stateFilter,
	UINT & aor
)
{	
	// make sure we have up-to-date
	// timestamps prior to getting the
	// dependency state
	if (!pgrn->m_fVisited)
		(void) m_pregistry->GetRegEntry(pgrn->Frh())->UpdateAttributes();	// does nothing if being watched

	// visited
	pgrn->m_fVisited = TRUE;

	if (pgrn->CChildren() == 0)
		return CMD_Complete;

	int cVisited = 0;

	//
	// See if dependencies need to be updated.
	//
    CActionSlob * pAction = (CActionSlob *)pgrn->Edge(0);
	if ( NULL != pAction )
	{
		if ( !pAction->IsDepInfoInitialized() )
		{
			pAction->UpdateDepInfo();
		}
	}

	for (USHORT iChild = pgrn->CChildren(); iChild > 0; iChild--)
	{
		CDepGrNode * pgrnChild = (CDepGrNode *)pgrn->Child(iChild-1);

		// enumerate actions of our child?
		// o recursive and child not visited
		// (depth first, post-order)
		if ((aor & AOR_Recurse) && !pgrnChild->m_fVisited)
		{
			UINT cmdRet = EnumerateBuildActionsI(lstActions, lstFrh, EC, pgrnChild, stateFilter, aor);
			if (cmdRet != CMD_Complete)
				return cmdRet;

			cVisited++;
		}
	}


	// our dependency state check
	// o dependant state is a subset of our filter?
	// pgrn->FigureDepState( );
	UINT depState = pgrn->Depstate();

	if ((depState & DS_ForcedOutOfDate) != 0)
	{
		if ((stateFilter & DS_ForcedOutOfDate) != 0)
		{
			// REVIEW(kperry) just because we want find out what t build, we shouldn't
			//				  set it back in date !
			pgrn->SetDepstate(depState & ~DS_ForcedOutOfDate);
			pgrn->FigureDepState( );
			depState = pgrn->Depstate() | DS_ForcedOutOfDate;
		}
		else if (pAction != NULL)
		{
			UINT oldState = depState;
			pgrn->SetDepstate(depState & ~DS_ForcedOutOfDate);
			pgrn->FigureDepState( );
			depState = ((oldState & stateFilter) == 0) ? pgrn->Depstate() : oldState;
			pgrn->SetDepstate(oldState); // restore old state
		}
	}

	// do we have an action we might want to add?
	if (pAction == (CActionSlob *)NULL)
		return CMD_Complete;
 
	BOOL fAddAction = ((depState & stateFilter) != 0);

	// have we already retrieved this?
	if ( pAction->m_wMark == CActionSlob::m_wActionMarker)
	{
		// be aggressive here, mark our parents as likely to be out of date
		if (fAddAction)
			for (USHORT iParent = pgrn->CParents(); iParent > 0; iParent--)
			{
				CDepGrNode * pgrnParent = (CDepGrNode *)pgrn->Parent(iParent-1);
				
				UINT depState = pgrnParent->Depstate();

				// if current, then force out of date
				if ((depState & DS_OutOfDate) == 0)
					pgrnParent->SetDepstate((depState & ~DS_Current) | DS_OutOfDate);
			}
		return CMD_Complete;
	}


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
				CActionSlob * pEdge = (CActionSlob *)pgrn->Edge(iChild-1);
				if ( pEdge != (CActionSlob *)NULL  )
				{
					CDepGrNode * pgrnChild = (CDepGrNode *)pgrn->Child(iChild-1);
					CFileRegEntry * pregChild = m_pregistry->GetRegEntry(pgrnChild->Frh());

					// not exist and an action?
					if ( !pregChild->ExistsOnDisk() )
					{
						// don't ignore this missing dependency
						fIgnDepMissing = FALSE;
						break;
					}
				}
			}

			// ignore missing dependency in the case that we
			// we *only* missing dependencies for non-action
			// relationships, eg. scanned deps.
			fAddAction = fIgnDepMissing;
		}
	}


	// refresh the command-line options
	
	// anything changed?
	BOOL fOptsChnged;
	BOOL fRefreshOk = pAction->RefreshCommandOptions(fOptsChnged, (aor & AOR_PreChk) != 0);

	// o check for options changes and have they possibly changed?
	if ((aor & AOR_ChkOpts) && fRefreshOk && fOptsChnged && !fAddAction)
	{
		// do we want to prompt?
		if (!m_fPromptedCheckOptions)
		{
			m_fPromptedCheckOptions = TRUE;
			int iReply;
			if( g_bBatchBuildInProgress )
				iReply = IDYES;
			else
				iReply = MsgBox(Question, IDS_PROJ_SUGGEST_REBUILD);

			if (iReply == IDNO)
			{
				// don't check for option changes...
				aor &= ~AOR_ChkOpts;
			}
			else if (iReply == IDCANCEL)
			{
				// user cancelled the build
				return CMD_Canceled;
			}
			else
			{
				// user replied yes, add this
				fAddAction = TRUE;

				// Delete the outputs to this action so that we will
				// rebuild this if something goes wrong, as the change
				// in build settings is lost after the first attempted
				// build. Don't delete target outputs, since thats rude.
				if( !pAction->BuildTool()->m_fTargetTool && !(aor & AOR_PreChk) ){
					CFileRegSet * pOutput = pAction->GetOutput();
					CFileRegEntry *frh = (CFileRegEntry *)pOutput->GetFirstFrh();
					if( frh != NULL ){
						frh->DeleteFromDisk();
						frh->ReleaseFRHRef();
					}
				}
			}
		}
		else
		{
			// already prompted and the answer
			// must have been yes, add this
			fAddAction = TRUE;

            // Delete the outputs to this action so that we will
            // rebuild this if something goes wrong, as the change
            // in build settings is lost after the first attempted
            // build
			if( !pAction->BuildTool()->m_fTargetTool && !(aor & AOR_PreChk) ){
				CFileRegSet * pOutput = pAction->GetOutput();
				CFileRegEntry *frh = (CFileRegEntry *)pOutput->GetFirstFrh();
				if( frh != NULL ){
					frh->DeleteFromDisk();
					frh->ReleaseFRHRef();
				}
			}
		}
	}
/*	
	// o check for action input changes?
	if (!fAddAction &&							// not already adding action?
		(aor & AOR_ChkInp) &&					// want to check intputs?
		(pgrn->Depstate() & DS_Current)	&&		// dep-state current?
		pgrn->m_fDirtyDepLst &&					// possible dep. list change?
		pgrn->DepcyListAtCurr()->GetCount()
	   )
	{
#ifndef REFCOUNT_WORK
		const CPtrList * plstDepcy = pgrn->DepcySet()->GetContent();
		FRHArray *plstOldDepcy = pgrn->DepcyListAtCurr();

		// anything changed?
		if (plstDepcy->GetCount() != plstOldDepcy->GetCount())
#else
		FRHArray *plstOldDepcy = pgrn->DepcyListAtCurr();

		// anything changed?
		if (pgrn->DepcySet()->GetCount() != plstOldDepcy->GetCount())
#endif
		{
			// for sure ...., so add action
			fAddAction = TRUE;
		}
		else
		{
			// compare content to be sure ....
			int n = plstOldDepcy->GetCount();
			int i = 0;
			CFileRegSet * pregset = pgrn->DepcySet();
			ASSERT( NULL != pregset );
			while( i < n ){
			    FileRegHandle frh = plstOldDepcy->GetAt(i);
			    if( pregset->RegHandleExists( frh ) == FALSE ){
					fAddAction = TRUE;
					break;
			    }
			    i++;
			}
		}
	}

  */

	// add the action now?
	if (fAddAction)
	{
		// make sure we don't attempt to retrieve it again
		pAction->m_wMark = CActionSlob::m_wActionMarker;

		// add the file that caused the action
		ASSERT(pgrn->Frh()->IsKindOf(RUNTIME_CLASS(CFileRegFile)));
		lstFrh.AddTail(pgrn->Frh());

		// be aggressive here, mark our parents as likely to be out of date
		for (USHORT iParent = pgrn->CParents(); iParent > 0; iParent--)
		{
			CDepGrNode * pgrnParent = (CDepGrNode *)pgrn->Parent(iParent-1);
			
			UINT depState = pgrnParent->Depstate();

			// if current, then force out of date
			if ((depState & DS_OutOfDate) == 0)
				pgrnParent->SetDepstate((depState & ~DS_Current) | DS_OutOfDate);
		}
	}

	// not adding actions, yet attempted to build all we can?
	// explain why?
	if ( (!(aor & AOR_Recurse) || (cVisited && lstActions.GetCount()== 0)) && (aor & AOR_Verbose) )
	// if ( (!(aor & AOR_Recurse) || (cVisited)) && (aor & AOR_Verbose) )
	{
		// dependency missing....and no actions that may generate them!
		if (pgrn->Depstate() & DS_DepMissing)
		{
			CString strMsg;	// message buffer
			for (USHORT iChild = pgrn->CChildren(); iChild > 0; iChild--)
			{
				CDepGrNode * pgrnChild = (CDepGrNode *)pgrn->Child(iChild-1);
				CFileRegEntry * pregChild = m_pregistry->GetRegEntry(pgrnChild->Frh());

				// exist?
				if (pregChild->ExistsOnDisk())
					continue;

				// loop through deps. and warn about deps. that are missing
				if (pregChild->IsNodeType(CFileRegNotifyRx::nodetypeRegSet))
				{
					// report source deps. (headers) that are missing
					FileRegHandle frh;
					((CFileRegSet *)pregChild)->InitFrhEnum();
					while ((frh = ((CFileRegSet *)pregChild)->NextFrh()) != (FileRegHandle)NULL)
					{
						CFileRegFile * pregFile = (CFileRegFile *)m_pregistry->GetRegEntry(frh);
						if (!pregFile->ExistsOnDisk())
						{
							// REVIEW:: this is O(n^3) but only called if build is going to fail anyway
							// Should change to a map.
							if (!m_lstCannotBuild.Find(pregFile))
							{
								EC.AddString(MsgText(strMsg, IDS_CANNOT_BUILD_DEP, (const TCHAR *)*pregFile->GetFilePath()));
								m_lstCannotBuild.AddTail(pregFile);
								g_Spawner.m_dwProjWarning++;
							}
						}
#ifdef REFCOUNT_WORK
						frh->ReleaseFRHRef();
#endif
					}
				}
				else if (pgrnChild->CChildren() == 0)
				{
					// report file that we don't know how to or failed to build
					if (!m_lstCannotBuild.Find(pregChild))
					{
						EC.AddString(MsgText(strMsg, IDS_CANNOT_BUILD_DEP, (const TCHAR *)*pregChild->GetFilePath()));
						m_lstCannotBuild.AddTail(pregChild);
						g_Spawner.m_dwProjWarning++;
					}
				}
			}
		}
	}

	// construct our list of dependencies for when we were
	// last attempted to be built?
	if (pgrn->m_fDirtyDepLst)
	{
//#ifndef REFCOUNT_WORK
#if 1
		//
		// review(tomse): pgrn->DepcySet contains both CFileRegFile's and
		// CFileRegSet's.  Since this algorithm is only interested in top level
		// m_Contents items, it can't use InitFrhEnum/NextFrh.  This should
		// be reviewed.
		//
#ifndef REFCOUNT_WORK
		const CPtrList * plstDepcy = pgrn->DepcySet()->GetContent();
#else
//		const CPtrList * plstDepcy = &pgrn->DepcySet()->m_Contents;
#endif
//		FRHArray * plstOldDepcy = pgrn->DepcyListAtCurr();
//		plstOldDepcy->Replace( plstDepcy->GetCount() );
		
//		int i;		
//		POSITION pos;
//		for( i = 0, pos = plstDepcy->GetHeadPosition(); pos != (POSITION)NULL; i++ ) {
//			plstOldDepcy->SetAt( (FileRegHandle)plstDepcy->GetNext(pos), i);
#else
		CFileRegSet* pregset = pgrn->DepcySet();
		FRHArray * plstOldDepcy = pgrn->DepcyListAtCurr();
		plstOldDepcy->Replace( pregset->GetCount() );
		
		int i;
		FileRegHandle frhTemp = NULL;
		pregset->InitFrhEnum();
		i = 0;
		while ( NULL!=(frhTemp = pregset->NextFrh()) )
		{
			// No file sets expected.
			ASSERT( frhTemp->IsNodeType(CFileRegNotifyRx::nodetypeRegFile));
			plstOldDepcy->SetAt( frhTemp, i);
			frhTemp->ReleaseFRHRef();
			i++;
#endif
		// }

		// dependencies at last build are now clean
		pgrn->m_fDirtyDepLst = FALSE;
	}

	// transfer the actions to the 'master' list
	// versions
	if (fAddAction)
		lstActions.AddTail( pAction );
		
	return CMD_Complete;	// ok
}

// CNidGraphDoc implementation
USHORT CFileDepGraph::CChildrenInitWalk(NID nidNode)
{
	m_iChild = 0;
	m_pgrnChild = (CGrNode *)nidNode;

	// don't count dependency sets
	int iChild = 0, cChild = 0;
	for (;iChild < m_pgrnChild->CChildren(); iChild++)	
	{
		CDepGrNode * pgrn = (CDepGrNode *)m_pgrnChild->Child(iChild);
		CFileRegEntry * preg = g_FileRegistry.GetRegEntry(pgrn->Frh());
		if (!m_fShowDepSets && preg->IsNodeType(CFileRegNotifyRx::nodetypeRegSet))
			continue;

		cChild++;
	}

	return cChild;
}

BOOL CFileDepGraph::FNextNid(NID & nid, ET & et)
{
	NextNidChild:

	if (m_iChild >= m_pgrnChild->CChildren())
		return FALSE;

	CDepGrNode * pgrn = (CDepGrNode *)m_pgrnChild->Child(m_iChild);
	CFileRegEntry * preg = g_FileRegistry.GetRegEntry(pgrn->Frh());
	if (!m_fShowDepSets && preg->IsNodeType(CFileRegNotifyRx::nodetypeRegSet))
	{
		m_iChild++;
		goto NextNidChild;
	}

	nid = (NID)m_pgrnChild->Child(m_iChild);
	et = m_pgrnChild->Edge(m_iChild++);
	return TRUE;
}

///////////////////////////////////////////
// Default file graph ops
///////////////////////////////////////////

BOOL CFileDepGraph::Clean(CGrNode * pgrn, DWORD & dwUser, BOOL & fLookChildren)
{
	// only clean intermediate files
	if (pgrn->CParents() == 0 || pgrn->CChildren() == 0)
		return TRUE;	// ignore

	// file registry handle
	FileRegHandle frh = ((CDepGrNode *)pgrn)->Frh();
	CFileRegEntry * preg = g_FileRegistry.GetRegEntry(frh);
	ASSERT(preg != (CFileRegEntry *)NULL);

	// only files!
	if (!preg->IsNodeType(CFileRegNotifyRx::nodetypeRegFile))
		return TRUE;	// ignore

	// attempt to delete the file
	(void)g_buildengine.DeleteFile(frh, *(CErrorContext *)dwUser);
	return TRUE;
}

BOOL CFileDepGraph::GetAllOutput(CGrNode * pgrn, DWORD & dwUser, BOOL & fLookChildren)
{
	// only get output files
	if (pgrn->CChildren() == 0)
		return TRUE;	// ignore

	// file registry handle
	FileRegHandle frh = ((CDepGrNode *)pgrn)->Frh();

	// add to set
	((CFileRegSet *)dwUser)->AddRegHandle(frh);
	return TRUE;
}

BOOL CFileDepGraph::GetAllOutputExceptCustom(CGrNode * pgrn, DWORD & dwUser, BOOL & fLookChildren)
{
	// only get output files
	if (pgrn->CChildren() == 0)
		return TRUE;	// ignore

	// get our actions that contribute to target
	for (USHORT iChild = pgrn->CChildren(); iChild > 0; iChild--)
	{
		CActionSlob * pAction = (CActionSlob *)pgrn->Edge(iChild-1);
		if ( pAction == (CActionSlob *)NULL )
			continue;
		if ( pAction->BuildTool()->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)) )
			return TRUE;

	}

	// file registry handle
	FileRegHandle frh = ((CDepGrNode *)pgrn)->Frh();

	// add to set
	((CFileRegSet *)dwUser)->AddRegHandle(frh);
	return TRUE;
}

BOOL CFileDepGraph::GetTarget(BOOL fPrimary, CGrNode * pgrn, DWORD & dwUser, BOOL & fLookChildren, BOOL bNoCustom )
{
	// only look at children if we think there might be more targets!
	fLookChildren = FALSE;

	// file registry handle
	FileRegHandle frh = ((CDepGrNode *)pgrn)->Frh();

	// get our actions that contribute to target
	for (USHORT iChild = pgrn->CChildren(); iChild > 0; iChild--)
	{
		CActionSlob * pAction = (CActionSlob *)pgrn->Edge(iChild-1);

		// want this?
		
		// primary target's *must* have an action!
		if (fPrimary && pAction == (CActionSlob *)NULL)
			continue;

		// must be target-item or no-parents
		BOOL fActionOnTargetItem = pAction == (CActionSlob *)NULL ||
								   pAction->Item()->IsKindOf(RUNTIME_CLASS(CTargetItem)) ||
								   pAction->Item()->IsKindOf(RUNTIME_CLASS(CProject)); 
		
		if (!(pgrn->CParents() == 0 || fActionOnTargetItem))
			continue;

		// front or back?
		BOOL fFront = TRUE;
		
		if (pAction != (CActionSlob *)NULL)
		{
			// *all* deferred tools go at back
			if (pAction->BuildTool()->m_fDeferredTool)
				fFront = FALSE;

			// *all* non-target 'targets' go at back
			else if (!pAction->BuildTool()->m_fTargetTool)
				fFront = FALSE;

			// look in children if we're a non-target or deferred-tool
			// action on a target-item
			if (!fFront && fActionOnTargetItem)
				fLookChildren = TRUE;

			if ( bNoCustom && pAction->BuildTool()->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)) )
				continue;
		}
		else
		{
			fFront = FALSE;
		}

		((CFileRegSet *)dwUser)->AddRegHandle(frh, TRUE, fFront);
		break;	// done!
	}

	return TRUE;
}


BOOL CFileDepGraph::GetCustomOutput( CGrNode * pgrn, DWORD & dwUser, BOOL & fLookChildren)
{
	BOOL fPrimary= TRUE;
	// only look at children if we think there might be more targets!
	fLookChildren = FALSE;

	// file registry handle
	FileRegHandle frh = ((CDepGrNode *)pgrn)->Frh();

	// get our actions that contribute to target
	for (USHORT iChild = pgrn->CChildren(); iChild > 0; iChild--)
	{
		CActionSlob * pAction = (CActionSlob *)pgrn->Edge(iChild-1);

		// want this?
		
		// primary target's *must* have an action!
		if (fPrimary && pAction == (CActionSlob *)NULL)
			continue;

		// must be target-item or no-parents
		BOOL fActionOnTargetItem = pAction == (CActionSlob *)NULL ||
								   pAction->Item()->IsKindOf(RUNTIME_CLASS(CTargetItem)) ||
								   pAction->Item()->IsKindOf(RUNTIME_CLASS(CProject)); 
		
		if (!(pgrn->CParents() == 0 || fActionOnTargetItem))
			continue;

		// front or back?
		BOOL fFront = TRUE;
		
		if (pAction != (CActionSlob *)NULL)
		{
			// *all* deferred tools go at back
			if ( !pAction->BuildTool()->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)) )
				continue;

			// *all* non-target 'targets' go at back
			else if (!pAction->BuildTool()->m_fTargetTool)
				fFront = FALSE;

			// look in children if we're a non-target or deferred-tool
			// action on a target-item
			if (!fFront && fActionOnTargetItem)
				fLookChildren = TRUE;
		}
		else
		{
			fFront = FALSE;
		}
		
	    CString strOutput;
		CCustomBuildTool *pTool = (CCustomBuildTool *)pAction->BuildTool();
		pTool->GetCustomBuildStrProp(pAction->Item(), P_CustomOutputSpec, strOutput);
		ExpandMacros(&strOutput,strOutput,pAction,TRUE);
		COptionList lstOutSpec(_T('\t'), FALSE);
		lstOutSpec.SetString((const TCHAR *)strOutput);
		POSITION pos = lstOutSpec.GetHeadPosition();

		while (pos != (POSITION)NULL)
		{
			CString strOutSpec= lstOutSpec.GetNext(pos);
			((CMapStringToPtr *)dwUser)->SetAt(strOutSpec,(void *)fActionOnTargetItem);
		}
		break;	// done!
	}

	return TRUE;
}
