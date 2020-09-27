//
// CLayout
// 
// simple minded layout code
//
// given an object from which we can get the "shape" of the tree
// we lay out each of the nodes in generic row column positions.
// The coordinates are generic, we only assume that any given column
// is of constant width and any given row of constant height (just
// like in a spreadsheet for instance)
// 
// the output is a row-column assignment for each node in the graph
// plus some dummy nodes which are introduce to make the graph look nicer
//
// [ricom] (modified by [matthewt]

#include "stdafx.h"
#pragma hdrstop

#ifdef DEPGRAPH_VIEW

#include "layout.h"	// local header file

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BOOL CLayout__FCreate(CLayout * & playout, CNidGraphDoc * pngr, USHORT usDepth)
// create a CLayout object, defer initialization to FInit method
//
{
	if ((playout = new CLayout) == (CLayout *)NULL)
		return FALSE;

	if (playout->FInit(pngr, usDepth))
		return TRUE;

	delete playout;
	playout = (CLayout *)NULL;

	return FALSE;
}

BOOL CLayout::FInit(CNidGraphDoc * pngr, USHORT usDepthConst)
//
// create a tree structure from the given CNidGraphDoc and NID values
// -- setup items which are constant over the whole tree walk
// -- then do the walk itself
// -- group items into columns
// -- regroup items within their columns using parent information
// -- exchange items within columns to minimize the cost
//
{
	_pngr        = pngr;
	_iDepthConst = usDepthConst;
	_iDepthCur   = 0;
	_iDepthMac   = 0;
	_cNodesMac   = 0;
	_iPosMac     = 0;
	_rgcDepth    = NULL;
	_rgiDepth    = NULL;
	_rgrgNodeCol = NULL;
	_rgRoots     = NULL;
	_rgPnode     = NULL;
	_pnodeHead	 = NULL;

	BOOL fRet = FALSE;

	for (;;)	// this loop will not actually repeat
	{
		// FUTURE: put a CWaitCursor cursor CTOR here
		//

		if ((_cRoots	= _pngr->CRootsInitWalk()) == 0)
		{
			fRet = TRUE;
			break;
		}

#ifdef USE_LOCAL_BUILD_HEAP
		if ((_rgRoots = (CGrNode **)CGrNode::g_heapNodes.alloc(sizeof(CGrNode *) * _cRoots)) == (CGrNode **)NULL)
#else
		if ((_rgRoots = (CGrNode **)malloc(sizeof(CGrNode *) * _cRoots)) == (CGrNode **)NULL)
#endif
			break;

		NID nid = 0;
		USHORT iRoot = 0;

		for (iRoot = 0; _pngr->FNextRoot(nid); iRoot++)
			_rgRoots[iRoot] = (CGrNode *)nid;

		ASSERT(iRoot == _cRoots);

		USHORT cRoots = _cRoots;	// save this away because new (fake) roots
									// might be added during the walk

		for (iRoot = 0; iRoot < cRoots; iRoot++)
		{
			CGrNode *pRoot = WalkTree((NID)_rgRoots[iRoot]);
			_rgRoots[iRoot] = pRoot;
		}

		for (iRoot = 0; iRoot < _cRoots; iRoot++)
			InsertFakeNodes(_rgRoots[iRoot]);

#ifdef USE_LOCAL_BUILD_HEAP
		if ((_rgcDepth = (USHORT *)CGrNode::g_heapNodes.alloc(sizeof(USHORT) * _iDepthMac)) == (USHORT *)NULL)
			break;

		if ((_rgiDepth = (USHORT *)CGrNode::g_heapNodes.alloc(sizeof(USHORT) * _iDepthMac)) == (USHORT *)NULL)
			break;

		if ((_rgrgNodeCol = (CGrNode ***)CGrNode::g_heapNodes.alloc(sizeof(CGrNode **) * _iDepthMac)) == (CGrNode ***)NULL)
			break;

		if ((_rgPnode = (CGrNode **)CGrNode::g_heapNodes.alloc(sizeof(CGrNode *) * _cNodesMac)) == (CGrNode **)NULL)
			break;
#else
		if ((_rgcDepth = (USHORT *)malloc(sizeof(USHORT) * _iDepthMac)) == (USHORT *)NULL)
			break;

		if ((_rgiDepth = (USHORT *)malloc(sizeof(USHORT) * _iDepthMac)) == (USHORT *)NULL)
			break;

		if ((_rgrgNodeCol = (CGrNode ***)malloc(sizeof(CGrNode **) * _iDepthMac)) == (CGrNode ***)NULL)
			break;

		if ((_rgPnode = (CGrNode **)malloc(sizeof(CGrNode *) * _cNodesMac)) == (CGrNode **)NULL)
			break;
#endif

		UINT i;

		for (i=0; i<_iDepthMac; i++)
			_rgcDepth[i] = _rgiDepth[i] = 0;

		for (iRoot = 0; iRoot < _cRoots; iRoot++)
			CountDepth(_rgRoots[iRoot]);

		USHORT iNodeNext = 0;

		for (i=0; i<_iDepthMac; i++)
		{
			_rgrgNodeCol[i] = &_rgPnode[iNodeNext];
			iNodeNext += _rgcDepth[i];
		}

		for (iRoot = 0; iRoot < _cRoots; iRoot++)
			GroupNodes(_rgRoots[iRoot]);

#ifdef _DEBUG
		for (i = 0; i < _cNodesMac; i++)
			ASSERT(_rgPnode[i] != NULL);
#endif

		AttachParents();

		for (i=0; i<_iDepthMac; i++)
			_rgiDepth[i] = 0;

		for (iRoot = 0; iRoot < _cRoots; iRoot++)
			RegroupNodes(_rgRoots[iRoot]);

		for (i=0; i<_iDepthMac; i++)
			if (_rgiDepth[i] >= _iPosMac)
				_iPosMac = _rgiDepth[i]+1;

		// if there are too many nodes then we don't try to do any
		// smart formatting because it would take too long.

		if (CNodesMac() < 500)
			for (int cImp = 0; cImp < 10 && FImproveLayout(); cImp++);
		else
			SortColumns();	// minimally sort the columns by row

		fRet = TRUE;
		break;
	}

	return fRet;
}

CGrNode * CLayout::WalkTree(NID nidRoot)
// use the CNidGraphDoc implementation to return the nodes of the tree
{
	USHORT cChildren;

	if (_iDepthConst == 0 || _iDepthCur < _iDepthConst)
		cChildren = _pngr->CChildrenInitWalk(nidRoot);
	else
		cChildren = 0;

	CGrNode *pnode = PnodeNew(nidRoot, cChildren, _iDepthCur);
	if (pnode == (CGrNode *)NULL)
	{
		ASSERT(FALSE);
		return pnode;
	}
	
	// add node to NDX of known nodes
	_mapNodes.SetAt((void *)nidRoot, (void *)pnode);

	pnode->m_fVisited = TRUE;

	_iDepthCur++;

	ASSERT(pnode->m_cChildren == cChildren);

	// Here comes a slimey trick: to avoid a ton of memory fragmentation
	// we store the Nid's in place of the pointers until they've been
	// processed.  This also allows us to get all the children for a given
	// node at once which in turn gives us much better locality of
	// reference in the underlying graph structure

	USHORT ichild = 0;
	NID nid = 0;
	ET  et  = 0;

	while (ichild < cChildren)	// don't walk any children that might be added
	{
		VERIFY(_pngr->FNextNid(nid, et));

		if (nid != nidRoot)
		{
			pnode->m_rgEdges[ichild] = et;
			pnode->m_rgChildren[ichild++] = (CGrNode *)nid;
		}
		else
		{
			cChildren--;
			pnode->m_cChildren--;
			pnode->m_fRecursive = TRUE;
		}
	}

	ASSERT(pnode->m_cChildren == cChildren);

	// now do the actual walk by looking at the nids we got from the pointers
	// and converting them to real pointers as we go

	for (ichild = 0; ichild < cChildren; ichild++)
	{
		NID nidChild = (NID)pnode->m_rgChildren[ichild];

		// check if bob is already in tree
		if (FIncludesNid(nidChild))
		{
			CGrNode * pChild = PnodeAtNid(nidChild);

			// check for cycle
			if (pChild->m_fVisited)
			{
				// we've found a cycle	-- reverse this edge
				// but make a note of the reversal so we can fix
				// it when we draw the graph

				pnode->DelChild(ichild);
				pChild->AddChild(pnode);
				pChild->m_cReversed++;
				ichild--;
				cChildren--;
			}
			else
			{
				// if no cycle, connect to existing node and adjust depths
				//
				pnode->m_rgChildren[ichild] = pChild;
				pnode->m_fWorking = TRUE;
				AdjustDepth(pnode->m_rgChildren[ichild], _iDepthCur);
				pnode->m_fWorking = FALSE;
			}
		}
		else
			pnode->m_rgChildren[ichild] = WalkTree(nidChild);
	}

	_iDepthCur--;

	pnode->m_fVisited = FALSE;;

	return pnode;
}

void CLayout::AdjustDepth(CGrNode *pnode, USHORT iDepth)
// check to make sure the given node is at least the given depth and if not
// adjust it and its children
//
{
	if (pnode->m_iDepth >= iDepth || pnode->m_fWorking)
		return;

	pnode->m_fWorking = TRUE;
	pnode->m_iDepth = iDepth;

	if (iDepth >= _iDepthMac) _iDepthMac = iDepth+1;

	for (USHORT iChild = 0; iChild < pnode->m_cChildren; iChild++)
		AdjustDepth(pnode->m_rgChildren[iChild], iDepth+1);

	pnode->m_fWorking = FALSE;
}

void CLayout::InsertFakeNodes(CGrNode *pnode)
// check that all nodes have children that are at exactly one level
// deeper than their parents.  If this is not the case then insert
// fake nodes to make it so
//
{
	USHORT cChildren   = pnode->m_cChildren;
	USHORT iDepth      = pnode->m_iDepth;
	CGrNode ** rgChildren  = pnode->m_rgChildren;

	CGrNode * pnodeChild;

	USHORT iChild = 0;

	for (; iChild <cChildren; iChild++)
	{
		pnodeChild = rgChildren[iChild];

		if (pnodeChild->m_iDepth > iDepth+1)
		{
			// the child is more than one level away from us...
			// we need to introduce some fake nodes to span the gap from
			// the parent to the child

			// at this point we could use one fake node for all children
			// which are more than one level away from this parent

			// we can also use one fake node for all parents which are
			// more than one level away from the same child

			// search for an already created fake node that we can reuse

			CGrNode * pnodeNew = pnodeChild->m_pgrnAlias;
			BOOL fReverseEdge =	(iChild + pnode->CReversed() >= cChildren);
			ET   et = pnode->m_rgEdges[iChild];

			while (pnodeNew != (CGrNode *)NULL)
			{
				USHORT cRev;

				cRev  = pnodeNew->CReversed();

				ASSERT(pnodeNew->FFake());
				ASSERT(pnodeNew->CChildren() == 1);
				ASSERT(cRev == 0 || cRev == 1);

				if (pnodeNew->m_iDepth == iDepth+1 &&
					fReverseEdge == !!cRev &&
					pnodeNew->m_rgEdges[0] == et
				   )
				   break;

				pnodeNew = pnodeNew->m_pgrnAlias;
			}

			if (pnodeNew == (CGrNode *)NULL)
			{
				// must create a new fake node

				pnodeNew = PnodeNew(nidNil, 1, iDepth+1);
				pnodeNew->m_rgChildren[0] = pnodeChild;
				pnodeNew->m_rgEdges[0] = pnode->m_rgEdges[iChild];
				pnodeNew->m_pgrnAlias = pnodeChild->m_pgrnAlias;
				pnodeNew->m_fFake = TRUE;
				pnodeChild->m_pgrnAlias = pnodeNew;

				if (fReverseEdge)
					pnodeNew->m_cReversed = 1;
			}
				
			pnodeChild = rgChildren[iChild] = pnodeNew;
		}

		// no further work is required on this node, check the children

		if (pnodeChild->m_iDepth == iDepth+1 && pnodeChild->m_cChildren)
			InsertFakeNodes(pnodeChild);
	}
}

CGrNode * CLayout::PnodeNew(NID nid, USHORT cChildren, USHORT iDepth)
// Create a new node, keep track of the total number of nodes
// allocated and the maximum depth reached so far.
// Use the CGrNode constructor to initialize the node
//
{
	CGrNode * pnode = new CGrNode((CGraph *)NULL, nid, cChildren, iDepth);
	ASSERT(pnode != (CGrNode *)NULL);

	if (iDepth >= _iDepthMac)
		_iDepthMac = iDepth+1;

	_cNodesMac++;

	return pnode;
}

void CLayout::CountDepth(CGrNode *pnode)
// count the number of nodes at each level
// as each node is counted, its fWorking bit is set to TRUE
//
{
	if (pnode->m_fWorking) return;

	pnode->m_fWorking = TRUE;

	ASSERT(pnode->m_iDepth < _iDepthMac);

	_rgcDepth[pnode->m_iDepth]++;

	int iMac = pnode->m_cChildren;

	for (int i=0; i < iMac; i++)
		CountDepth(pnode->m_rgChildren[i]);
}

void CLayout::GroupNodes(CGrNode *pnode)
// put this node into its appropriate group
// as each node is placed in its column, its fWorking bit is reset to FALSE
//
// this initial grouping will allow us to conveniently walk all the nodes
// by just marching through the _rgPnode[] array.  A better grouping will
// be done by RegroupNodes
// 
{
	if (!pnode->m_fWorking) return;

	pnode->m_fWorking = FALSE;
	pnode->m_fVisited  = FALSE;

	USHORT depth = pnode->m_iDepth;
	pnode->m_iPos = 0xffff;

	_rgrgNodeCol[depth][_rgiDepth[depth]] = pnode;
	_rgiDepth[depth]++;

	int iMac = pnode->m_cChildren;

	for (int i=0; i < iMac; i++)
		GroupNodes(pnode->m_rgChildren[i]);
}

void CLayout::RegroupNodes(CGrNode *pnode)
// put this node into its appropriate group
// as each node is placed in its column, its fWorking bit is set to TRUE
//
// this grouping differs from the simple GroupNodes grouping in that:
//
// 		a) we traverse both child and parent links in order to get
//		   common parents of a child together as well as common children
//	       of a parent together
//
//      b) we assign a sensible position which is to allow for the
//	       actual amount of space that will probably be required
//		   by a node plus it's children
//
{
	while (pnode)
	{
		if (!pnode->m_fWorking)
		{
			int i = 0;

			pnode->m_fWorking = TRUE;

			USHORT depth = pnode->m_iDepth;
			pnode->m_iPos = _rgiDepth[depth];

			_rgiDepth[depth]++;

			// determine the number of "New" children that this node will
			// have and assign it a position which will center it amongst
			// these new children

			int iMac = pnode->m_cChildren;

			if (iMac && depth < _iDepthMac-1)
			{
				USHORT iNew = 0;

				for (i=0; i < iMac; i++)
					if (!pnode->m_rgChildren[i]->m_fWorking) iNew++;

				if (iNew)
				{
					if (_rgiDepth[depth+1] + iNew/2 < pnode->m_iPos)
					{
						_rgiDepth[depth+1] = pnode->m_iPos - iNew/2;
					}
					else if (pnode->m_iPos < _rgiDepth[depth+1] + (iNew-1)/2)
					{
						pnode->m_iPos = _rgiDepth[depth+1] + (iNew-1)/2;

						ASSERT(_rgiDepth[depth] < pnode->m_iPos+1);
						_rgiDepth[depth] = pnode->m_iPos+1;
					}
				}
			}

			// group any ungrouped parents of this node

			iMac = pnode->m_cParents;

			for (i=iMac; --i >= 0;)
				if (!pnode->m_rgParents[i]->m_fWorking)
					ToHeadNode(pnode->m_rgParents[i]);

			// group any ungrouped children of this node

			iMac = pnode->m_cChildren;

			for (i=iMac; --i >= 0;)
				if (!pnode->m_rgChildren[i]->m_fWorking)
					ToHeadNode(pnode->m_rgChildren[i]);
			}

		pnode = PnodeRemoveHead();
	}
}

USHORT CLayout::Cost(CGrNode *pnode, USHORT iPos)
// compute the cost of putting a given node at a certain position
//
{
	USHORT cMetric = 0, iDepthReqd, i;
	USHORT 	cParents  = pnode->m_cParents;
	CGrNode ** rgParents = pnode->m_rgParents;

	iDepthReqd = pnode->m_iDepth-1;

	for (i=0; i<cParents; i++)
	{
		CGrNode * pnodeParent = rgParents[i];

		// don't consider nodes at the wrong depth
		if (pnodeParent->m_iDepth != iDepthReqd) continue;

		// compute distance to the child

		int d = pnodeParent->m_iPos - iPos;
		if (d <0 ) d = -d;

		cMetric += d;
	}

	USHORT 	cChildren  = pnode->m_cChildren;
	CGrNode **	rgChildren = pnode->m_rgChildren;
	iDepthReqd = pnode->m_iDepth+1;

	for (i=0; i<cChildren; i++)
	{
		CGrNode *  pnodeChild = rgChildren[i];

		// don't consider nodes at the wrong depth
		if (pnodeChild->m_iDepth != iDepthReqd) continue;

		// don't consider leaf nodes with only one parent -
		//		-- they're easy to move

		// if (pnodeChild->m_cChildren == 0 && pnodeChild->m_cParents == 1)
		//	continue;

		// compute distance to the child

		int d = pnodeChild->m_iPos - iPos;
		if (d<0) d = -d;

		cMetric += d;
	}

	return cMetric;
}

BOOL CLayout::FImproveLayout()
// try to minimize the total "Cost()" of the graph
//
{
	CGrNode *	pnode, * pnode2;
	CGrNode **	mpPosNode;
	USHORT 	c1, c1T, c2, c2T, p1, p2, iNode, iDepth;

	BOOL fDidWork = FALSE;

#ifdef USE_LOCAL_BUILD_HEAP
	if ((mpPosNode = (CGrNode **)CGrNode::g_heapNodes.alloc(sizeof(CGrNode *) * _iPosMac)) == (CGrNode **)NULL)
#else
	if ((mpPosNode = (CGrNode **)malloc(sizeof(CGrNode *) * _iPosMac)) == (CGrNode **)NULL)
#endif
		return FALSE;

	for (iDepth=0; iDepth < _iDepthMac; iDepth++)
	{
		USHORT cNodes = _rgcDepth[iDepth];
		CGrNode ** rgNodes = _rgrgNodeCol[iDepth];

		for (iNode = 0; iNode < _iPosMac; iNode++)
			mpPosNode[iNode] = (CGrNode *)NULL;

		for (iNode=0; iNode<cNodes; iNode++)
		{
			pnode = rgNodes[iNode];
			mpPosNode[pnode->m_iPos] = pnode;
			pnode->m_cost = Cost(pnode, pnode->m_iPos);
		}

		USHORT cLimit = 10;
		BOOL fOptimal;
		do
		{
			fOptimal = TRUE;

			for (iNode=0; iNode < cNodes; iNode++)
			{
				pnode   = rgNodes[iNode];
				p1  	= pnode->m_iPos;
				c1  	= pnode->m_cost;

				if (c1 == 0) continue;

				ASSERT(mpPosNode[p1] == pnode);

				for (p2 = pnode->m_iPos + 1; p2 <_iPosMac; p2++)
				{
					c1T = Cost(pnode, p2);

					if (c1T == c1) goto endsearch;
					if (c1T > c1) break;

					pnode2 = mpPosNode[p2];

					if (pnode2 != NULL)
					{
						ASSERT(pnode2->m_iPos == p2);

						c2   = pnode2->m_cost;
						c2T  = Cost(pnode2, p1);

						int effect = c1T + c2T - c1 - c2;

						if (effect > 0) continue;

						if (effect == 0 &&
							pnode->m_cSiblings <= pnode2->m_cSiblings
						   )
						   continue;
					}

					// lower cost -- reposition node

					ASSERT(p1 != p2);

					mpPosNode[p2] = pnode;
					mpPosNode[p1] = pnode2;

					pnode->m_iPos = p2;
					pnode->m_cost = c1T;

					fOptimal = FALSE;
					fDidWork = TRUE;
			
					if (pnode2)
					{
						pnode2->m_iPos = p1; 
						pnode2->m_cost = c2T;
					}

					c1 = c1T;
					p1 = p2;

					if (c1 == 0) break;
				}

				for (p2 = pnode->m_iPos - 1; p2 < _iPosMac; p2--)
				{
					c1T = Cost(pnode, p2);

					if (c1T == c1) goto endsearch;
					if (c1T > c1) break;

					pnode2 = mpPosNode[p2];

					if (pnode2 != NULL)
					{
						ASSERT(pnode2->m_iPos == p2);

						c2   = pnode2->m_cost;
						c2T  = Cost(pnode2, p1);

						int effect = c1T + c2T - c1 - c2;

						if (effect > 0) continue;
						if (effect == 0 &&
							pnode->m_cSiblings <= pnode2->m_cSiblings
						   )
							continue;
					}

					// lower cost -- reposition node

					ASSERT(p1 != p2);

					mpPosNode[p2] = pnode;
					mpPosNode[p1] = pnode2;

					pnode->m_iPos = p2;
					pnode->m_cost = c1T;

					fOptimal = FALSE;
					fDidWork = TRUE;
			
					if (pnode2)
					{
						pnode2->m_iPos = p1; 
						pnode2->m_cost = c2T;
					}

					c1  = c1T;
					p1  = p2;
					if (c1 == 0) break;
				}

				endsearch:; 
			}

		} while (!fOptimal && --cLimit > 0);

		p1 = 0;

		for (iNode = 0; iNode < cNodes && p1 < _iPosMac; p1++)
		{
			pnode = mpPosNode[p1];
			if (pnode != (CGrNode *)NULL)
				rgNodes[iNode++] = pnode;
		}

		ASSERT(iNode == cNodes);
	}

#ifdef USE_LOCAL_BUILD_HEAP
	CGrNode::g_heapNodes.dealloc(mpPosNode);
#else
	free(mpPosNode);
#endif
	return fDidWork;
}

void CLayout::AttachParents()
// count every nodes parents and then allocate and fill in the parent
// array in each node.  We do this now so that artifically introduced
// nodes will be counted.
//
// at this time we also compute the number of "siblings" for each node
//
{
	USHORT iNode, iChild, cChildren;
	CGrNode *pnode, *pChild, **rgChildren;
	
	// this isn't pretty we have to:
	//	a) count the parents
	//	b) allocate memory for all the parents
	//	c) fill in the pointer values
	//  d) compute the number of "siblings"
	//
	// this requires four separate passes over all the nodes (eeek!)
	//
	// oh well...	-Rico

	// first we count all the parents
	for (iNode = 0; iNode < _cNodesMac; iNode++)
	{
		pnode = _rgPnode[iNode];

		cChildren  = pnode->m_cChildren;
		rgChildren = pnode->m_rgChildren;

		for (iChild = 0; iChild < cChildren; iChild++)
		{
			pChild = rgChildren[iChild];
			pChild->m_cParents++;
		}
	}

	// now allocate the memory
	for (iNode = 0; iNode < _cNodesMac; iNode++)
	{
		pnode = _rgPnode[iNode];

		if (pnode->m_cParents)
		{
#ifdef USE_LOCAL_BUILD_HEAP
			pnode->m_rgParents = (CGrNode **)CGrNode::g_heapNodes.alloc(sizeof(CGrNode *) * pnode->m_cParents);
#else
			pnode->m_rgParents = (CGrNode **)malloc(sizeof(CGrNode *) * pnode->m_cParents);
#endif
			ASSERT(pnode->m_rgParents != (CGrNode **)NULL);
		}
	}

	// finally, fill in the parent values
	for (iNode = 0; iNode < _cNodesMac; iNode++)
	{
		pnode = _rgPnode[iNode];

		cChildren  = pnode->m_cChildren;
		rgChildren = pnode->m_rgChildren;

		for (iChild = 0; iChild < cChildren; iChild++)
		{
			pChild = rgChildren[iChild];
			pChild->m_rgParents[pChild->m_iParent++] = pnode;
		}
	}

	// now compute the number of siblings for each node
	for (iNode = 0; iNode < _cNodesMac; iNode++)
	{
		USHORT cSibs = 0;
		pnode = _rgPnode[iNode];

		cChildren  = pnode->m_cChildren;
		rgChildren = pnode->m_rgChildren;

		for (iChild = 0; iChild < cChildren; iChild++)
			cSibs += rgChildren[iChild]->m_cParents;

		cChildren = pnode->m_cParents;
		rgChildren = pnode->m_rgParents;

		for (iChild = 0; iChild < cChildren; iChild++)
			cSibs += rgChildren[iChild]->m_cParents;

		pnode->m_cSiblings = cSibs;
	}
}

static int CmpNodes(void *n1, void *n2)
// compare two nodes -- use their row as the key
//
{
	USHORT p1 = (*(CGrNode **)n1)->IRow();
	USHORT p2 = (*(CGrNode **)n2)->IRow();

	if (p1 < p2)
		return -1;
	if (p1 == p2)
		return 0;
	else
		return 1;
}

void CLayout::SortColumns()
// sort the nodes in each column so that they are in increasing order
// of display position.  This helps us to do intelligent painting
//
{
	for (UINT iDepth = 0; iDepth < _iDepthMac; iDepth++)
	{
		USHORT cNodes    = _rgcDepth[iDepth];
		CGrNode **rgNodes = _rgrgNodeCol[iDepth];

		if (cNodes > 1)
			qsort(rgNodes, cNodes, sizeof(CGrNode *),
				(int (*)(const void *, const void *)) (int (*)(void *, void *))
				CmpNodes);
	}
}

CLayout::~CLayout()
// do a full cleanup
//
{
#ifdef USE_LOCAL_BUILD_HEAP
	if (_rgcDepth)    CGrNode::g_heapNodes.dealloc(_rgcDepth);
	if (_rgiDepth)    CGrNode::g_heapNodes.dealloc(_rgiDepth);
	if (_rgrgNodeCol) CGrNode::g_heapNodes.dealloc(_rgrgNodeCol);
	if (_rgRoots)     CGrNode::g_heapNodes.dealloc(_rgRoots);
#else
	if (_rgcDepth)    free(_rgcDepth);
	if (_rgiDepth)    free(_rgiDepth);
	if (_rgrgNodeCol) free(_rgrgNodeCol);
	if (_rgRoots)     free(_rgRoots);
#endif

	if (_rgPnode)
	{
		// free all the nodes

		for (USHORT i = 0; i<_cNodesMac; i++)
		{
#ifdef USE_LOCAL_BUILD_HEAP
			CGrNode::g_heapNodes.dealloc(_rgPnode[i]);
#else
			free(_rgPnode[i]);
#endif
			_rgPnode[i] = (CGrNode *)NULL;
		}

#ifdef USE_LOCAL_BUILD_HEAP
		CGrNode::g_heapNodes.dealloc(_rgPnode);
#else
		free(_rgPnode);
#endif
	}
}

void CLayout::ToHeadNode(CGrNode *pnode)
// if pnode is not in the queue, add it at the head 
//
// if pnode is elsewhere in the queue currently then move it to the head
// 
{
	if (pnode->m_fVisited)
	{
		// node is already in the queue

		if (pnode->m_pgrnPrev == (CGrNode *)NULL)
		{
			ASSERT(_pnodeHead == pnode);
			return;		// this is already at the head -- no work
		}

		pnode->m_pgrnPrev->m_pgrnNext = pnode->m_pgrnNext;

		if (pnode->m_pgrnNext)
			pnode->m_pgrnNext->m_pgrnPrev = pnode->m_pgrnPrev;
	}
	else
		pnode->m_fVisited = TRUE;

	pnode->m_pgrnPrev = (CGrNode *)NULL;
	pnode->m_pgrnNext = _pnodeHead;

	if (_pnodeHead)
	{
		ASSERT(_pnodeHead->m_pgrnPrev == (CGrNode *)NULL);
		_pnodeHead->m_pgrnPrev = pnode;
	}

	_pnodeHead = pnode;
}

CGrNode * CLayout::PnodeRemoveHead()
// remove the first node from the queue
// return NULL if no such node is available
//
{
	CGrNode *pnode;

	if (_pnodeHead == (CGrNode *)NULL)
		return  (CGrNode *)NULL;

	pnode = _pnodeHead;
	_pnodeHead = pnode->m_pgrnNext;
	pnode->m_pgrnNext = NULL;

	if (_pnodeHead)
	{
		ASSERT(_pnodeHead->m_pgrnPrev == pnode);
		_pnodeHead->m_pgrnPrev = (CGrNode *)NULL;
	}

	return pnode;
}

#endif // DEPGRAPH_VIEW
