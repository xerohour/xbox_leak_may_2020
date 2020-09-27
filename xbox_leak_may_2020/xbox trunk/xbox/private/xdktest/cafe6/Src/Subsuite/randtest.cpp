/////////////////////////////////////////////////////////////////////////////
// randtest.cpp
//
// email	date		change
// briancr	06/15/95	created
//
// copyright 1995 Microsoft

// Implementation of the CRandTest classes

#include "stdafx.h"
#include "randtest.h"
#include "subsuite.h"

#define new DEBUG_NEW

///////////////////////////////////////////////////////////////////////////////
//	CTest class

IMPLEMENT_DYNAMIC(CRandomTest, CTest);

CRandomTest::CRandomTest(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename /*= NULL*/, CSet<CVertex> setPreCond /*= EmptySet*/, CSet<CVertex> setPostCond /*= EmptySet*/)
: CTest(pSubSuite, szName, nExpectedCompares, szListFilename),
  m_setPreCond(setPreCond),
  m_setPostCond(setPostCond)
{
	m_nType = TEST_TYPE_RANDOM;
}

CRandomTest::~CRandomTest()
{
}

///////////////////////////////////////////////////////////////////////////////
//	Operations

void CRandomTest::PreRun(void)
{
    CTest::PreRun();

	// dump pre-conditions to the log
	CString strCond;
	for (POSITION pos = m_setPreCond.GetStartPosition(); pos != NULL; ) {
		CVertex vertex = m_setPreCond.GetNext(pos);
		strCond += CString(vertex.GetName()) + CString(" | ");
	}
	strCond = strCond.Left(strCond.GetLength()-3);
	GetLog()->RecordInfo("Pre-conditions: %s.", (LPCSTR)strCond);
	// dump post-conditions to the log
	strCond.Empty();
	for (pos = m_setPostCond.GetStartPosition(); pos != NULL; ) {
		CVertex vertex = m_setPostCond.GetNext(pos);
		strCond += CString(vertex.GetName()) + CString(" & ");
	}
	strCond = strCond.Left(strCond.GetLength()-3);
	GetLog()->RecordInfo("Post-conditions: %s.", (LPCSTR)strCond);
}

BOOL CRandomTest::SetPreCond(CSet<CVertex>& setPreCond)
{
	m_setPreCond = setPreCond;

	return TRUE;
}

CSet<CVertex>& CRandomTest::GetPreCond(void)
{
	return m_setPreCond;
}

BOOL CRandomTest::SetPostCond(CSet<CVertex>& setPostCond)
{
	m_setPostCond = setPostCond;

	return TRUE;
}

CSet<CVertex>& CRandomTest::GetPostCond(void)
{
	return m_setPostCond;
}

CSet<CVertex> operator+(const CVertex& lhsVertex, const CVertex& rhsVertex)
{
	return CSet<CVertex>(lhsVertex) + CSet<CVertex>(rhsVertex);
}


///////////////////////////////////////////////////////////////////////////////
// Pre- and post-conditions

// accel ed vertices
extern const CVertex AccelBegin(CVertex::AccelEd, CVertex::V_AccelBegin);
extern const CVertex AccelOpen(CVertex::AccelEd, CVertex::V_AccelOpen);
// bitmap ed vertices
extern const CVertex BmpBegin(CVertex::BitmapEd, CVertex::V_BmpBegin);
extern const CVertex BmpOpen(CVertex::BitmapEd, CVertex::V_BmpOpen);
// cursor ed vertices
extern const CVertex CursorBegin(CVertex::CursorEd, CVertex::V_CursorBegin);
extern const CVertex CursorOpen(CVertex::CursorEd, CVertex::V_CursorOpen);
// debugger vertices
extern const CVertex DbgBegin(CVertex::Debugger, CVertex::V_DbgBegin);
extern const CVertex DbgDebug(CVertex::Debugger, CVertex::V_DbgDebug);
// dialog ed vertices
extern const CVertex DialogBegin(CVertex::DialogEd, CVertex::V_DialogBegin);
extern const CVertex DialogOpen(CVertex::DialogEd, CVertex::V_DialogOpen);
// icon ed vertices
extern const CVertex IconBegin(CVertex::IconEd, CVertex::V_IconBegin);
extern const CVertex IconOpen(CVertex::IconEd, CVertex::V_IconOpen);
// menu ed vertices
extern const CVertex MenuBegin(CVertex::MenuEd, CVertex::V_MenuBegin);
extern const CVertex MenuOpen(CVertex::MenuEd, CVertex::V_MenuOpen);
// project vertices
extern const CVertex ProjBegin(CVertex::Project, CVertex::V_ProjBegin);
extern const CVertex ProjOpen(CVertex::Project, CVertex::V_ProjOpen);
extern const CVertex ProjBuilt(CVertex::Project, CVertex::V_ProjBuilt);
// res script vertices
extern const CVertex ResBegin(CVertex::ResScript, CVertex::V_ResBegin);
extern const CVertex ResOpen(CVertex::ResScript, CVertex::V_ResOpen);
// source ed vertices
extern const CVertex SrcBegin(CVertex::SourceEd, CVertex::V_SrcBegin);
extern const CVertex SrcOpen(CVertex::SourceEd, CVertex::V_SrcOpen);
// string ed vertices
extern const CVertex StringBegin(CVertex::StringEd, CVertex::V_StringBegin);
extern const CVertex StringOpen(CVertex::StringEd, CVertex::V_StringOpen);
// toolbar ed vertices
extern const CVertex ToolbarBegin(CVertex::ToolbarEd, CVertex::V_ToolbarBegin);
extern const CVertex ToolbarOpen(CVertex::ToolbarEd, CVertex::V_ToolbarOpen);
// version ed vertices
extern const CVertex VersionBegin(CVertex::VersionEd, CVertex::V_VersionBegin);
extern const CVertex VersionOpen(CVertex::VersionEd, CVertex::V_VersionOpen);

// empty set
const CSet<CVertex> EmptySet;
