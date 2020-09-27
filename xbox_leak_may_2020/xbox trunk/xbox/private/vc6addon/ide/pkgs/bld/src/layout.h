//
// CLayout
//
// Self-laying out graph class
//
// [ricom] (modified by [matthewt]

#ifndef LAYOUT_H
#define LAYOUT_H

#ifndef _INCLUDE_DEPGRAPH_H
#error Expected depgraph.h to be included before layout.h.
#endif

#ifndef DEPGRAPH_VIEW
#error layout.h should only be used if DEPGRAPH_VIEW is defined.
#endif

#ifdef DEPGRAPH_VIEW

class CNidGraphDoc;
class CLayout
{
	friend BOOL CLayout__FCreate(CLayout * &, CNidGraphDoc *, USHORT);

public:
	CLayout() {}
	virtual ~CLayout();

private:
	CGrNode **		_rgRoots;		// array of root nodes for this graph
	USHORT			_cRoots;		// number of nodes in above array

	USHORT 			_iDepthConst;	// largest depth allowed by user
	USHORT 			_iDepthCur;		// current depth (used while assign depths)
	USHORT 			_iDepthMac;		// largest depth + 1
	USHORT 			_cNodesMac;		// total number of nodes
	USHORT			_iPosMac;		// largest row number
	CMapPtrToPtr 	_mapNodes;		// mapping from NID (ULONG cast to VOID *) to NODE *

	USHORT *		_rgcDepth;	  	// array of depth counts;
	CGrNode *** 	_rgrgNodeCol; 	// columns -- rgrgNodeCol[iDepth][iNode];

	USHORT *		_rgiDepth;	    // array of depth indices
	CNidGraphDoc *	_pngr;       	// graph document that is the source of nodes
	CGrNode **		_rgPnode;	    // main array of pointers to nodes
	CGrNode *		_pnodeHead;		// node at the head of the node list

	CGrNode *		WalkTree(NID);
	void			AdjustDepth(CGrNode *pnode, USHORT iDepth);
	void			InsertFakeNodes(CGrNode *pnode);
	void			CountDepth(CGrNode *pnode);
	void			GroupNodes(CGrNode *pnode);
	void			RegroupNodes(CGrNode *pnode);
	USHORT			Cost(CGrNode *pnode, USHORT iPos);
	void			AttachParents();
	BOOL			FImproveLayout();
	void			SortColumns();
	CGrNode *		PnodeNew(NID nid, USHORT cChildren, USHORT iDepth);
	CGrNode *		PnodeRemoveHead();
	void			ToHeadNode(CGrNode *);	

public:
	__inline USHORT 	IColMac()  				  	{return _iDepthMac;}
	__inline USHORT 	IRowMac()   				{return _iPosMac;}
	__inline USHORT 	CNodesMac() 				{return _cNodesMac;}
	__inline USHORT 	CNodesCol(USHORT col)		{return _rgcDepth[col];}
	__inline CGrNode *	PnodeAt(USHORT i, USHORT j)	{return _rgrgNodeCol[i][j];}
	__inline CGrNode *	PnodeAt(USHORT inode)		{return _rgPnode[inode];} 

	__inline CGrNode *	PnodeAtNid(NID nid)
		{CGrNode * pnode; return _mapNodes.Lookup((void *)nid, (void * &)pnode) ? pnode : (CGrNode *)NULL;}
											  	
	__inline BOOL 		FIncludesNid(NID nid)		{return PnodeAtNid(nid) != (CGrNode *)NULL;}

protected:
	BOOL		FInit(CNidGraphDoc *, USHORT usDepth);
};

#endif // DEPGRAPH_VIEW
#endif
