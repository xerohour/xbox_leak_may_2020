/////////////////////////////////////////////////////////////////////////////
// vertex.cpp
//
// email	date		change
// briancr	06/15/95	created
//
// copyright 1995 Microsoft

// Implementation of the CVertex class

#include "stdafx.h"
#include "vertex.h"

#define new DEBUG_NEW

///////////////////////////////////////////////////////////////////////////////
// CVertex class

IMPLEMENT_DYNAMIC(CVertex, CObject);

LPCSTR CVertex::AccelEdVertexNames[]	= { "AccelEdBegin",		"AccelEdOpen",			NULL };
LPCSTR CVertex::BitmapEdVertexNames[]	= { "BitmapEdBegin",	"BitmapEdOpen",			NULL };
LPCSTR CVertex::CursorEdVertexNames[]	= { "CursorEdBegin",	"CursorEdOpen",			NULL };
LPCSTR CVertex::DebuggerVertexNames[]	= { "DebuggerBegin",	"DebuggerDebugging",	NULL };
LPCSTR CVertex::DialogEdVertexNames[]	= { "DialogEdBegin",	"DialogEdOpen",			NULL };
LPCSTR CVertex::IconEdVertexNames[]		= { "IconEdBegin",		"IconEdOpen",			NULL };
LPCSTR CVertex::MenuEdVertexNames[]		= { "MenuEdBegin",		"MenuEdOpen",			NULL };
LPCSTR CVertex::ProjectVertexNames[]	= { "ProjectBegin",		"ProjectOpen",			"ProjectBuilt" };
LPCSTR CVertex::ResScriptVertexNames[]	= { "ResScriptBegin",	"ResScriptOpen",		NULL };
LPCSTR CVertex::SourceEdVertexNames[]	= { "SourceEdBegin",	"SourceEdOpen",			NULL };
LPCSTR CVertex::StringEdVertexNames[]	= { "StringEdBegin",	"StringEdOpen",			NULL };
LPCSTR CVertex::ToolbarEdVertexNames[]	= { "ToolbarEdBegin",	"ToolbarEdOpen",		NULL };
LPCSTR CVertex::VersionEdVertexNames[]	= { "VersionEdBegin",	"VersionEdOpen",		NULL };
LPCSTR CVertex::NullVertexNames[]		= { "NullVertex",		NULL,					NULL };

LPCSTR* CVertex::VertexNames[] = {	CVertex::AccelEdVertexNames,
									CVertex::BitmapEdVertexNames,
									CVertex::CursorEdVertexNames,
									CVertex::DebuggerVertexNames,
									CVertex::DialogEdVertexNames,
									CVertex::IconEdVertexNames,
									CVertex::MenuEdVertexNames,
									CVertex::ProjectVertexNames,
									CVertex::ResScriptVertexNames,
									CVertex::SourceEdVertexNames,
									CVertex::StringEdVertexNames,
									CVertex::ToolbarEdVertexNames,
									CVertex::VersionEdVertexNames,
									CVertex::NullVertexNames
									};


CVertex::CVertex(void)
: m_Category(Null),
  m_Vertex(V_Null)
{
}

CVertex::CVertex(Category category, int vertex)
: m_Category(category),
  m_Vertex(vertex)
{
}

CVertex::CVertex(const CVertex& vertex)
: m_Category(vertex.m_Category),
  m_Vertex(vertex.m_Vertex)
{
}

CVertex::~CVertex()
{
}

int CVertex::GetVertex(void)
{
	return m_Vertex;
}

CVertex::Category CVertex::GetCategory(void)
{
	return m_Category;
}

CVertex& CVertex::operator=(const CVertex& vertex)
{
	m_Category = vertex.m_Category;
	m_Vertex = vertex.m_Vertex;

	return *this;
}

BOOL CVertex::operator==(const CVertex& vertex)
{
	// two vertices are equal if the vertex and category match
	return (m_Vertex == vertex.m_Vertex) && (m_Category == vertex.m_Category);
}

BOOL CVertex::operator!=(const CVertex& vertex)
{
	return !(operator==(vertex));
}

LPCSTR CVertex::GetName(void)
{
	return VertexNames[m_Category][m_Vertex];
}

///////////////////////////////////////////////////////////////////////////////
// CVertex constants

// null vertices
const CVertex NullVertex(CVertex::Null, CVertex::V_Null);
