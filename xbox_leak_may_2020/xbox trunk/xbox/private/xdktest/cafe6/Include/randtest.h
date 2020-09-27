/////////////////////////////////////////////////////////////////////////////
// randtest.h
//
// email	date		change
// briancr	06/15/95	created
//
// copyright 1994 Microsoft

// Interface of the CRandomTest class

#ifndef __RANDTEST_H__
#define __RANDTEST_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "test.h"
#include "vertex.h"
#include "set.h"

///////////////////////////////////////////////////////////////////////////////
// CRandomTest macros -- for use in derived classes

#define DECLARE_RANDOM_TEST(this_class, subsuite_class)			\
		DECLARE_DYNAMIC(this_class)								\
	public:														\
		this_class(subsuite_class* pSubSuite);					\
		subsuite_class* GetSubSuite(void);

#define IMPLEMENT_RANDOM_TEST(this_class, base_class, test_name, num_comparisons, subsuite_class, pre_cond, post_cond)	\
	IMPLEMENT_DYNAMIC(this_class, base_class)													\
	this_class::this_class(subsuite_class* pSubSuite)											\
	: base_class(pSubSuite, test_name, num_comparisons, NULL, pre_cond, post_cond)				\
	{																							\
	}																							\
	subsuite_class* this_class::GetSubSuite(void)												\
	{																							\
		ASSERT(m_pSubSuite->IsKindOf(RUNTIME_CLASS(subsuite_class)));							\
		return (subsuite_class*)m_pSubSuite;													\
	}

// empty set
extern const CSet<CVertex> EmptySet;

// vertex addition
CSet<CVertex> operator+(const CVertex& lhsVertex, const CVertex& rhsVertex);

///////////////////////////////////////////////////////////////////////////////
//	CTest abstract base class

class CRandomTest : public CTest
{
	DECLARE_DYNAMIC(CRandomTest)

// constructor/destructor
public:
	CRandomTest(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL, CSet<CVertex> setPreCond = EmptySet, CSet<CVertex> setPostCond = EmptySet);
	virtual ~CRandomTest();

// Operations
public:
	virtual void PreRun(void);

	BOOL SetPreCond(CSet<CVertex>& setPreCond);
	CSet<CVertex>& GetPreCond(void);

	BOOL SetPostCond(CSet<CVertex>& setPostCond);
	CSet<CVertex>& GetPostCond(void);

// Data
protected:
	// test attributes: pre- and post-conditions
	CSet<CVertex> m_setPreCond;
	CSet<CVertex> m_setPostCond;

// don't want anyone to call these
protected:
	CRandomTest() { }
};

// test types
#define TEST_TYPE_RANDOM	0x03

// accel ed vertices
extern const CVertex AccelBegin;
extern const CVertex AccelOpen;
// bitmap ed vertices
extern const CVertex BmpBegin;
extern const CVertex BmpOpen;
// cursor ed vertices
extern const CVertex CursorBegin;
extern const CVertex CursorOpen;
// debugger vertices
extern const CVertex DbgBegin;
extern const CVertex DbgDebug;
// dialog ed vertices
extern const CVertex DialogBegin;
extern const CVertex DialogOpen;
// icon ed vertices
extern const CVertex IconBegin;
extern const CVertex IconOpen;
// menu ed vertices
extern const CVertex MenuBegin;
extern const CVertex MenuOpen;
// project vertices
extern const CVertex ProjBegin;
extern const CVertex ProjOpen;
extern const CVertex ProjBuilt;
// res script vertices
extern const CVertex ResBegin;
extern const CVertex ResOpen;
// source ed vertices
extern const CVertex SrcBegin;
extern const CVertex SrcOpen;
// string ed vertices
extern const CVertex StringBegin;
extern const CVertex StringOpen;
// toolbar ed vertices
extern const CVertex ToolbarBegin;
extern const CVertex ToolbarOpen;
// version ed vertices
extern const CVertex VersionBegin;
extern const CVertex VersionOpen;

#endif //__RANDTEST_H__
