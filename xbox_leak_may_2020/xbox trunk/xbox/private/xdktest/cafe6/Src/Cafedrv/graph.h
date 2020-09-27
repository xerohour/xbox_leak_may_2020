/////////////////////////////////////////////////////////////////////////////
// graph.h
//
// email	date		change
// briancr	06/15/95	created
//
// copyright 1994 Microsoft

// Interface of the CGraph class

#ifndef __GRAPH_H__
#define __GRAPH_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "vertex.h"
#include "edge.h"

///////////////////////////////////////////////////////////////////////////////
// CAdjacencyList class

// the type for each item in the adjacency list
struct AdjacencyListItem
{
	AdjacencyListItem(CVertex* pvertex, CEdge* pedge) : m_pVertex(pvertex), m_pEdge(pedge) { }
	CVertex* m_pVertex;
	CEdge* m_pEdge;
};

// the adjacency list type
typedef CList<AdjacencyListItem*, AdjacencyListItem*> CAdjacencyList;

///////////////////////////////////////////////////////////////////////////////
// CState class

class CState: public CObject
{
	DECLARE_DYNAMIC(CState)
		
// constructor/destructor
public:
	CState(void);
	~CState();
	
// data types
protected:
	typedef CArray<CVertex*, CVertex*> VertexArray;
	
// operations
public:
	void Update(CVertex& vertex);
	BOOL Contains(CVertex& vertex);
	void Reset(void);
	CString GetName(void);

// data
protected:
	// the order of the vertices in this array *must* match the order of
	// the enumeration CVertex::Category
	VertexArray* m_paVertices;
};

///////////////////////////////////////////////////////////////////////////////
// CGraph class

class CGraph: public CObject
{
	DECLARE_DYNAMIC(CGraph)

// ctor/dtor
public:
	CGraph(LPCSTR pszName);
	virtual ~CGraph(void);

// operations
public:
	void AddEdge(CEdge* pEdge, const CVertex& srcVertex, const CVertex& destVertex);
	void DeleteEdge(CEdge* pEdge);

	void SetName(LPCSTR pszName)	{ m_strName = pszName; }
	CString GetName(void)			{ return m_strName; }

	BOOL IsEmpty(void)				{ return (m_pGraph->GetSize() == 0); }

	// for iterating through the vertices in this graph
	POSITION GetFirstVertexPosition(void);
	CVertex* GetNextVertex(POSITION& pos);

	// for iterating through the edges for a vertex at the given position
	POSITION GetFirstEdgePosition(CVertex* pSrcVertex);
	CEdge* GetNextEdge(CVertex* pSrcVertex, POSITION& posVertex, CVertex** ppDestVertex);

	CAdjacencyList* GetAdjacencyList(CVertex& vertex);
	
// data types
protected:

	// the type for each item in the adjacency array
	struct AdjacencyArrayItem
	{
		AdjacencyArrayItem(const CVertex& vertex, CAdjacencyList* plist) : m_Vertex(vertex), m_pAdjacencyList(plist) { }
		CVertex m_Vertex;
		CAdjacencyList* m_pAdjacencyList;
	};

	// the type for the array of adjacency lists
	typedef CArray<AdjacencyArrayItem*, AdjacencyArrayItem*> AdjacencyArray;

// data
protected:
	// the graph is an adjacency array
	AdjacencyArray* m_pGraph;
	CString m_strName;

// don't want anyone to call these
protected:
	CGraph(void) { }
};

///////////////////////////////////////////////////////////////////////////////
// CGraphForest class

class CGraphForest: public CObject
{
	DECLARE_DYNAMIC(CGraphForest)

// constructor/destructor
public:
	CGraphForest(void);
	virtual ~CGraphForest();

// data types
public:
	typedef CArray<CGraph*, CGraph*> GraphArray;

// operations
public:
	void AddEdge(CEdge* pEdge, CVertex& srcVertex, CVertex& destVertex);
	void DeleteEdge(CEdge* pEdge);

	BOOL IsEmpty(void);

	// for iterating through the graphs in this forest
	POSITION GetFirstGraphPosition(void);
	CGraph* GetNextGraph(POSITION& pos);

	void GetStateEdges(CState& state, CAdjacencyList* pList);
	
// data
protected:
	// the order of the graphs in this array *must* match the order of
	// the enumeration CVertex::Category
	GraphArray* m_paGraphs;
};

#endif //__GRAPH_H__
