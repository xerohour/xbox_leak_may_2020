/////////////////////////////////////////////////////////////////////////////
// test.h
//
// email	date		change
// briancr	10/25/94	created
//
// copyright 1994 Microsoft

// Interface of the CTest classes

#ifndef __TEST_H__
#define __TEST_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "log.h"
#include "settings.h"
#include "target.h"
#include "idetarg.h"
#include "toolset.h"
#include "guiv1.h"
#include "platform.h"

///////////////////////////////////////////////////////////////////////////////
//useful macro for the compile time message very needed in CAFE;
//example of usage:
//#pragma message(__LOC__ "Working around C2XX codegen bug 3397 by...")
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)"): CAFE msg: "

///////////////////////////////////////////////////////////////////////////////
// CTest macros -- for use in derived classes

#define DECLARE_TEST(this_class, subsuite_class)	\
		DECLARE_DYNAMIC(this_class)								\
	public:														\
		this_class(subsuite_class* pSubSuite);					\
		subsuite_class* GetSubSuite(void);

#define DECLARE_LISTTEST(this_class, subsuite_class)	\
		DECLARE_DYNAMIC(this_class)								\
	public:														\
		this_class(subsuite_class* pSubSuite);					\
		subsuite_class* GetSubSuite(void);

#define IMPLEMENT_TEST(this_class, base_class, test_name, num_comparisons, subsuite_class)		\
	IMPLEMENT_DYNAMIC(this_class, base_class)													\
	this_class::this_class(subsuite_class* pSubSuite)											\
	: base_class(pSubSuite, test_name, num_comparisons)											\
	{																							\
	}																							\
	subsuite_class* this_class::GetSubSuite(void)												\
	{																							\
		ASSERT(m_pSubSuite->IsKindOf(RUNTIME_CLASS(subsuite_class)));							\
		return (subsuite_class*)m_pSubSuite;													\
	}

// A list test doesn't specify a number of comparisons because it can vary as the test is run.
#define IMPLEMENT_LISTTEST(this_class, base_class, test_name, test_list, subsuite_class)		\
	IMPLEMENT_DYNAMIC(this_class, base_class)													\
	this_class::this_class(subsuite_class* pSubSuite)											\
	: base_class(pSubSuite, test_name, -1, test_list)											\
	{																							\
	}																							\
	subsuite_class* this_class::GetSubSuite(void)												\
	{																							\
		ASSERT(m_pSubSuite->IsKindOf(RUNTIME_CLASS(subsuite_class)));							\
		return (subsuite_class*)m_pSubSuite;													\
	}

#define XSAFETY (((CIDESubSuite*)GetSubSuite())->GetIDE()->IsActive())

// forward declarations
class CRawListString;
class CSubSuite;

///////////////////////////////////////////////////////////////////////////////
//	CTest abstract base class

class CTest : public CObject
{
	DECLARE_DYNAMIC(CTest)

// data types
public:
	enum RunState { RS_NoRun = 0, RS_Run };

// constructor/destructor
public:
	CTest(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL);
	virtual ~CTest();

// Operations
public:
	virtual void PreRun(void);
  	virtual void PreRun(CRawListString &);
	virtual void Run(void);
    virtual void Run(CRawListString &);
	virtual void PostRun(void);
    virtual void PostRun(CRawListString &);

	virtual BOOL RunAsDependent(void);

  BOOL CanTestPlatform(CPlatform *pPlatform);

  void AddSupportedPlatform(const CPlatform::COS &rOS, const CPlatform::CLanguage &rLang);

	CLog* GetLog(void);

	CSubSuite* GetSubSuite(void);

	CString GetCWD(void);

	BOOL SetName(LPCSTR szName);
	CString GetName(void);

	BOOL SetRunState(RunState rs);
	RunState GetRunState(void);

    BOOL SetListFilename(LPCSTR szListFilename);
    CString GetListFilename(void);

	BOOL SetDependency(CTest* pTest);
	CTest* GetDependency(void);

	virtual int GetType(void);

	// WriteLog is provided for backward compatibility only
	// GetLog()->RecordCompare should be used instead
	enum LogType { PASSED = 0, FAILED };
	void WriteLog(int LT, LPCSTR szFmt, ... );

// Data
protected:
	// storage for the log
	CLog* m_pLog;

	// storage for the subsuite
	CSubSuite* m_pSubSuite;

	// test attributes: name, run/no run, pre- and post-conditions
	CString m_strName;
	RunState m_rs;

	// storage for the test's current working directory
	CString m_strCWD;

	// the name of the list filename, if applicable
    CString m_strListFilename;

	// the number of comparisons expected when this test runs
	int m_nExpectedCompares;

	// storage for the test's dependency
	CTest* m_pDependency;

    // list of supported platforms

    typedef CList<CPlatform *, CPlatform *> CPlatformList;

	CPlatformList *m_pPlatformList;

	// storage for the type of test
	int m_nType;

	// Store the start and end times for this test.
	CTime m_timeStart;
	CTime m_timeEnd;

// don't want anyone to call these
protected:
	CTest() { }
};

// types of tests
#define TEST_TYPE_STANDARD	0x01
#define TEST_TYPE_LIST		0x02

#endif //__TEST_H__
