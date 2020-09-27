/////////////////////////////////////////////////////////////////////////////
// vertex.h
//
// email	date		change
// briancr	06/15/95	created
//
// copyright 1994 Microsoft

// Interface of the CVertex class

#ifndef __VERTEX_H__
#define __VERTEX_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CVertex class

class CVertex: public CObject
{
	DECLARE_DYNAMIC(CVertex)

// data types
public:
	enum Category { AccelEd = 0, BitmapEd, CursorEd, Debugger, DialogEd, IconEd, MenuEd, Project, ResScript, SourceEd, StringEd, ToolbarEd, VersionEd, NumCategories, Null };

	enum NullVertices		{ V_Null = 0 };
	enum AccelEdVertices	{ V_AccelBegin = 0,		V_AccelOpen };
	enum BitmapEdVertices	{ V_BmpBegin = 0,		V_BmpOpen };
	enum CursorEdVertices	{ V_CursorBegin = 0,	V_CursorOpen };
	enum DebuggerVertices	{ V_DbgBegin = 0,		V_DbgDebug };
	enum DialogEdVertices	{ V_DialogBegin = 0,	V_DialogOpen };
	enum IconEdVertices		{ V_IconBegin = 0,		V_IconOpen };
	enum MenuEdVertices		{ V_MenuBegin = 0,		V_MenuOpen };
	enum ProjectVertices	{ V_ProjBegin = 0,		V_ProjOpen,		V_ProjBuilt };
	enum ResScriptVertices	{ V_ResBegin = 0,		V_ResOpen };
	enum SourceEdVertices	{ V_SrcBegin = 0,		V_SrcOpen };
	enum StringEdVertices	{ V_StringBegin = 0,	V_StringOpen };
	enum ToolbarEdVertices	{ V_ToolbarBegin = 0,	V_ToolbarOpen };
	enum VersionEdVertices	{ V_VersionBegin = 0,	V_VersionOpen };

// constructor/destructor
public:
	CVertex(void);
	CVertex(Category category, int vertex);
	CVertex(const CVertex& vertex);
	virtual ~CVertex();

// operations
public:
	int GetVertex(void);
	Category GetCategory(void);

	CVertex& operator=(const CVertex& vertex);
	BOOL operator==(const CVertex& vertex);
	BOOL operator!=(const CVertex& vertex);

	LPCSTR GetName(void);

// data:
protected:
	int m_Vertex;
	Category m_Category;

// data
private:
	static LPCSTR NullVertexNames[];
	static LPCSTR AccelEdVertexNames[];
	static LPCSTR BitmapEdVertexNames[];
	static LPCSTR CursorEdVertexNames[];
	static LPCSTR DebuggerVertexNames[];
	static LPCSTR DialogEdVertexNames[];
	static LPCSTR IconEdVertexNames[];
	static LPCSTR MenuEdVertexNames[];
	static LPCSTR ProjectVertexNames[];
	static LPCSTR ResScriptVertexNames[];
	static LPCSTR SourceEdVertexNames[];
	static LPCSTR StringEdVertexNames[];
	static LPCSTR ToolbarEdVertexNames[];
	static LPCSTR VersionEdVertexNames[];

	static LPCSTR* VertexNames[];
};

// null vertices
extern const CVertex NullVertex;

#endif //__VERTEX_H__
