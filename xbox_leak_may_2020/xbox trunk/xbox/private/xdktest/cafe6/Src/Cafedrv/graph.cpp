/////////////////////////////////////////////////////////////////////////////
// graph.cpp
//
// email	date		change
// briancr	06/15/95	created
//
// copyright 1995 Microsoft

// Implementation of the CGraph class

#include "stdafx.h"
#include "graph.h"
#include "caferes.h"

#define new DEBUG_NEW

///////////////////////////////////////////////////////////////////////////////
// CGraph class

IMPLEMENT_DYNAMIC(CGraph, CObject);

CGraph::CGraph(LPCSTR pszName)
: m_strName(pszName)
{
	m_pGraph = new AdjacencyArray;
	m_pGraph->SetSize(0, 20);
}

CGraph::~CGraph(void)
{
	// delete each adjacency list and each item in the adjacency array
	for (int i = 0; i < m_pGraph->GetSize(); i++) {
		// delete each item in each adjacency list
		CAdjacencyList* pList = (*m_pGraph)[i]->m_pAdjacencyList;
		for (POSITION pos = pList->GetHeadPosition(); pos != NULL; ) {
			delete pList->GetNext(pos);
		}
		delete pList;
		delete (*m_pGraph)[i];
	}
	m_pGraph->RemoveAll();
	delete m_pGraph;
}

void CGraph::AddEdge(CEdge* pEdge, const CVertex& srcVertex, const CVertex& destVertex)
{
	int i;
	AdjacencyArrayItem* psrcAdjacencyArrayItem = NULL;
	AdjacencyArrayItem* pdestAdjacencyArrayItem = NULL;
	CAdjacencyList* pAdjacencyList;
	AdjacencyListItem* pAdjacencyListItem;

	// find the source vertex in the adjacency array
	// (or add an entry to the adjacency array if it doesn't exist)
	for (i = 0; i < m_pGraph->GetSize(); i++) {
		// does the vertex in this adjacency array item match the source vertex?
		if ((*m_pGraph)[i]->m_Vertex == srcVertex) {
			psrcAdjacencyArrayItem = (*m_pGraph)[i];
			break;
		}
	}
	// if the source vertex is not in the adjacency array, add it
	if (!psrcAdjacencyArrayItem) {
		pAdjacencyList = new CAdjacencyList;
		psrcAdjacencyArrayItem = new AdjacencyArrayItem(srcVertex, pAdjacencyList);
		m_pGraph->Add(psrcAdjacencyArrayItem);
	}

	// find the destination vertex in the adjacency array
	// (or add an entry to the adjacency array if it doesn't exist)
	for (i = 0; i < m_pGraph->GetSize(); i++) {
		// does the vertex in this adjacency array item match the destination vertex?
		if ((*m_pGraph)[i]->m_Vertex == destVertex) {
			pdestAdjacencyArrayItem = (*m_pGraph)[i];
			break;
		}
	}
	// if the destination vertex is not in the adjacency array, add it
	if (!pdestAdjacencyArrayItem) {
		pAdjacencyList = new CAdjacencyList;
		pdestAdjacencyArrayItem = new AdjacencyArrayItem(destVertex, pAdjacencyList);
		m_pGraph->Add(pdestAdjacencyArrayItem);
	}

	// add the edge from the source vertex to the destination vertex
	pAdjacencyListItem = new AdjacencyListItem(&(pdestAdjacencyArrayItem->m_Vertex), pEdge);
	psrcAdjacencyArrayItem->m_pAdjacencyList->AddTail(pAdjacencyListItem);
}

void CGraph::DeleteEdge(CEdge* pEdge)
{
	// NYI
	ASSERT(FALSE);
}

POSITION CGraph::GetFirstVertexPosition(void)
{
	// if the graph is empty, we can't iterate
	if (IsEmpty()) {
		return NULL;
	}
	return (POSITION)1;
}

CVertex* CGraph::GetNextVertex(POSITION& pos)
{
	// store the current vertex
	CVertex* pVertex = &((*m_pGraph)[(int)pos - 1]->m_Vertex);

	// increment and if the index is no longer in range, set pos to NULL
	pos = (POSITION)((int)pos + 1);
	if ((int)pos > m_pGraph->GetSize()) {
		pos = NULL;
	}
	return pVertex;
}

POSITION CGraph::GetFirstEdgePosition(CVertex* pSrcVertex)
{
	// find the source vertex
	for (int index = 0; index < m_pGraph->GetSize(); index++ ) {
		if ((*m_pGraph)[index]->m_Vertex == *pSrcVertex) {
			break;
		}
	}
	// did we find the source vertex?
	if (index >= m_pGraph->GetSize()) {
		return NULL;
	}

	// get the first position in the adjacency list
	return (*m_pGraph)[index]->m_pAdjacencyList->GetHeadPosition();
}

CEdge* CGraph::GetNextEdge(CVertex* pSrcVertex, POSITION& posEdge, CVertex** ppDestVertex)
{
	// find the source vertex
	for (int index = 0; index < m_pGraph->GetSize(); index++ ) {
		if ((*m_pGraph)[index]->m_Vertex == *pSrcVertex) {
			break;
		}
	}
	// did we find the source vertex?
	if (index >= m_pGraph->GetSize()) {
		return NULL;
	}

	// get the adjacency list item at the current position and iterate to the next position
	AdjacencyListItem* pItem = (*m_pGraph)[index]->m_pAdjacencyList->GetNext(posEdge);

	// store the destination vertex
	*ppDestVertex = pItem->m_pVertex;

	// return the edge to the destination
	return pItem->m_pEdge;
}

CAdjacencyList* CGraph::GetAdjacencyList(CVertex& vertex)
{
	// find the vertex
	for (int index = 0; index < m_pGraph->GetSize(); index++) {
		if ((*m_pGraph)[index]->m_Vertex == vertex) {
			// return the adjacency list
			return (*m_pGraph)[index]->m_pAdjacencyList;
		}
	}
	
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// CGraphForest class

IMPLEMENT_DYNAMIC(CGraphForest, CObject)

CGraphForest::CGraphForest(void)
{
	// allocate space for the graph array
	m_paGraphs = new GraphArray;
	m_paGraphs->SetSize(0, 20);

	// create all the graphs
	CString strName;
	CGraph* pGraph;
	for (int i = IDS_Graph_AccelEd; i <= IDS_Graph_VersionEd; i++) {
		// load the graph's name
		strName.LoadString(i);
		// create the graph
		pGraph = new CGraph(strName);
		// add it to the array of graphs
		m_paGraphs->Add(pGraph);
	}
}

CGraphForest::~CGraphForest()
{
	// delete each graph
	for (int i = 0; i < m_paGraphs->GetSize(); i++) {
		delete (*m_paGraphs)[i];
	}
	m_paGraphs->RemoveAll();
	delete m_paGraphs;
}

void CGraphForest::AddEdge(CEdge* pEdge, CVertex& srcVertex, CVertex& destVertex)
{
	// add the edge to the graph based on the source vertex's category
	(*m_paGraphs)[srcVertex.GetCategory()]->AddEdge(pEdge, srcVertex, destVertex);
}

void CGraphForest::DeleteEdge(CEdge* pEdge)
{
	// it may be much easier to implement if we require the source and dest
	// vertices as well as the edge when deleting an edge

	// NYI
	ASSERT(FALSE);
}

BOOL CGraphForest::IsEmpty(void)
{
	// the forest is empty if all graphs in it are empty
	for (int i = 0; i < m_paGraphs->GetSize(); i++) {
		if (!(*m_paGraphs)[i]->IsEmpty()) {
			return FALSE;
		}
	}
	return TRUE;
}

POSITION CGraphForest::GetFirstGraphPosition(void)
{
	// if the forest is empty, we can't iterate
	if (IsEmpty()) {
		return NULL;
	}
	return (POSITION)1;
}

CGraph* CGraphForest::GetNextGraph(POSITION& pos)
{
	// store the current graph
	CGraph* pGraph = (*m_paGraphs)[(int)pos - 1];

	// increment and if the index is no longer in range, set pos to NULL
	pos = (POSITION)((int)pos + 1);
	if ((int)pos > m_paGraphs->GetSize()) {
		pos = NULL;
	}
	return pGraph;
}

void CGraphForest::GetStateEdges(CState& state, CAdjacencyList* pAdjacencyList)
{
	// build the list of edges that are traversible from the given state
	// essentially this is an adjacency list that includes all graphs

	// iterate through each graph
	for (POSITION posGraph = GetFirstGraphPosition(); posGraph != NULL; ) {
		CGraph* pGraph = GetNextGraph(posGraph);
		// iterate through each vertex in the graph
		for (POSITION posVertex = pGraph->GetFirstVertexPosition(); posVertex != NULL; ) {
			CVertex* pVertex = pGraph->GetNextVertex(posVertex);
			// is this vertex in the state?
			if (state.Contains(*pVertex)) {
				// add this vertex's adjacency list to master list
				CAdjacencyList* pList = pGraph->GetAdjacencyList(*pVertex);
				pAdjacencyList->AddTail(pList);
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// CState class

IMPLEMENT_DYNAMIC(CState, CObject)
		
CState::CState(void)
{
	// allocate space for the vertex array
	m_paVertices = new VertexArray;
	m_paVertices->SetSize(0, 20);
	
	// create each vertex
	for (int i = 0; i < CVertex::NumCategories; i++) {
		// create the vertex
		CVertex* pVertex = new CVertex((CVertex::Category)i, 0);
		// add it to the array of vertices
		m_paVertices->Add(pVertex);
	}
}

CState::~CState()
{
	// delete each vertex in the vertex array
	for (int i = 0; i < m_paVertices->GetSize(); i++) {
		delete (*m_paVertices)[i];
	}
	m_paVertices->RemoveAll();
	delete m_paVertices;
}
	
void CState::Update(CVertex& vertex)
{
	// update the state to reflect the given vertex
	// that is, find the vertex of the same category
	// in the state and set it to the given vertex
	for (int i = 0; i < m_paVertices->GetSize(); i++) {
		if ((*m_paVertices)[i]->GetCategory() == vertex.GetCategory()) {
			*((*m_paVertices)[i]) = vertex;
			return;
		}
	}
	// we should always find a matching category
	ASSERT(0);
}

BOOL CState::Contains(CVertex& vertex)
{
	// determine if the current state contains the given vertex
	// by iterating through each vertex and checking equality
	for (int i = 0; i < m_paVertices->GetSize(); i++) {
		if (*((*m_paVertices)[i]) == vertex) {
			return TRUE;
		}
	}
	return FALSE;
}

void CState::Reset(void)
{
	// cannot call Reset when the size of the array
	// does not match the number of categories
	ASSERT(m_paVertices->GetSize() == CVertex::NumCategories);

	// reset each vertex
	for (int i = 0; i < CVertex::NumCategories; i++) {
		// create a temporary vertex
		CVertex* pVertex = new CVertex((CVertex::Category)i, 0);
		// assign the value of the temp vertex to the vertex in the array
		*((*m_paVertices)[i]) = *pVertex;
		// delete the temporary vertex
		delete pVertex;
	}

}

CString CState::GetName(void)
{
	CString strName;

	// iterate through each vertex and assemble the name
	for (int i = 0; i < m_paVertices->GetSize(); i++) {
		strName += CString((*m_paVertices)[i]->GetName()) + CString(", ");
	}
	strName = strName.Left(strName.GetLength()-2);

	return strName;
}
