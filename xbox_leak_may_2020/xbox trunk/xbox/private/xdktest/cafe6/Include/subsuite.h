/////////////////////////////////////////////////////////////////////////////
// subsuite.h
//
// email	date		change
// briancr	10/25/94	created
//
// copyright 1994 Microsoft

// Interface of the CSubSuite class

#ifndef __SUBSUITE_H__
#define __SUBSUITE_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifdef _DEBUG
	#pragma comment(lib, "subsuitd.lib")
#else
	#pragma comment(lib, "subsuite.lib")
#endif

#include "log.h"
#include "settings.h"

class CTest;	// Forward declaration.
class CToolset;
class CIDE;
	
///////////////////////////////////////////////////////////////////////////////
// TESTLIST macros
//
// To create a list of tests, call the TESTLIST macros as follows:
//
// BEGIN_TESTLIST(CThisClass'sName)
//   TEST(CTestClass, "Test name", [ Run | NoRun ], Pre-condition, Post-condition)
// END_TESTLIST()

#define DECLARE_TESTLIST()						\
	protected:									\
		virtual void InitializeTestList(void);

#define BEGIN_TESTLIST(this_class)												\
	void this_class::InitializeTestList(void)									\
	{																			\
		const char* const THIS_FUNCTION = #this_class "::InitializeTestList";	\
		CTest* pTest;															\
		CTest* pTestDependency;													\
		POSITION pos;															\
		m_listTests.RemoveAll();

#define TEST(test_class, test_run)				\
		pTest = new test_class(this);			\
		pTest->SetRunState(test_run);			\
		m_listTests.AddTail(pTest);

// in order for the chain of dependencies to work properly, we must store a
// pointer to the instantiation of the dependency (i.e., we can't instantiate
// another test because it won't have a dependency.
// to do this, we'll iterate through all the tests "registered" so far
// and use MFC's RTTI to find the test of the correct type
#define DEPENDENCY(test_dependency)				\
		for (pos = m_listTests.GetHeadPosition(); pos != NULL; ) {		\
			pTestDependency = m_listTests.GetNext(pos);							\
			if (pTestDependency->IsKindOf(RUNTIME_CLASS(test_dependency))) {	\
				pTest->SetDependency(pTestDependency);							\
				break;															\
			}																	\
		}																		\
		if (pos == NULL) {														\
			CString str;														\
			str.Format("%s: unable to find the dependency specified (%s) for the test: '%s'.", THIS_FUNCTION, #test_dependency, (LPCSTR)pTest->GetName()); \
			AfxMessageBox(str);													\
		}

#define END_TESTLIST()										\
		DBG_UNREFERENCED_LOCAL_VARIABLE(pTest);				\
		DBG_UNREFERENCED_LOCAL_VARIABLE(pos);				\
		DBG_UNREFERENCED_LOCAL_VARIABLE(pTestDependency);	\
	}

///////////////////////////////////////////////////////////////////////////////
// CSubSuite helper macros -- used by IMPLEMENT_SUBSUITE

#define IMPLEMENT_INSTANTIATESUBSUITE(this_class)									\
		this_class *suiteTable[20];													\
		int noAttached=0;

#define IMPLEMENT_CREATESUBSUITE(this_class)										\
	extern "C" __declspec(dllexport) CSubSuite* CreateSubSuite(LPCSTR SzCmdLine)	\
	{																				\
		ASSERT(noAttached<20);														\
		suiteTable[noAttached]= new this_class;										\
		TRACE1(#this_class " constructing the object@%x\n",suiteTable[noAttached]);	\
		suiteTable[noAttached]->m_CmdLine=SzCmdLine;								\
		return suiteTable[noAttached++];											\
	}

#define IMPLEMENT_DESTROYSUBSUITE(this_class)										\
	extern "C" __declspec(dllexport) void DestroySubSuite(CSubSuite *psuite)		\
	{	int i;																		\
		ASSERT(noAttached<=20);														\
		for(i=0;i<noAttached;i++)													\
			if(suiteTable[i]==psuite)												\
			{																		\
				TRACE1(#this_class " destructing the object@%x\n",suiteTable[i]);	\
				delete suiteTable[i];												\
				suiteTable[i]=NULL;													\
				break;																\
			}																		\
		if(i==noAttached-1)noAttached--;											\
	}

#define IMPLEMENT_ISDEBUGBUILD(debug_build)											\
	extern "C" __declspec(dllexport) BOOL IsDebugBuild(void)						\
	{																				\
		return debug_build;															\
	}

#define IMPLEMENT_DLLMAIN(this_class)												\
	static AFX_EXTENSION_MODULE extensionDLL = { NULL, NULL };						\
	extern "C" __declspec(dllexport) int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)	\
	{																				\
		if (dwReason == DLL_PROCESS_ATTACH) {										\
			TRACE0(#this_class " DLL Initializing!\n");								\
			AfxInitExtensionModule(extensionDLL, hInstance);						\
			new CDynLinkLibrary(extensionDLL);										\
		}																			\
		else if (dwReason == DLL_PROCESS_DETACH) {									\
			while(--noAttached>=0)													\
			{																		\
				if(suiteTable[noAttached]==NULL)									\
					continue;														\
				TRACE1(#this_class " destructing the object@%x\n",suiteTable[noAttached]);\
				delete suiteTable[noAttached];										\
				suiteTable[noAttached]=NULL;										\
			}																		\
			TRACE0(#this_class "DLL Terminating!\n");								\
			AfxTermExtensionModule(extensionDLL);									\
																					\
		}																			\
		return 1;																	\
		UNREFERENCED_PARAMETER(lpReserved);											\
	}

#ifdef _DEBUG
	#define DEBUGBUILD TRUE
#else
	#define DEBUGBUILD FALSE
#endif

///////////////////////////////////////////////////////////////////////////////
// CSubSuite macros -- for use in user derived classes

// includes DECLARE_DYNAMIC in the subsuite and provides a constructor
// use this macro in the definition of the subsuite class
#define DECLARE_SUBSUITE(this_class)			\
		DECLARE_DYNAMIC(this_class)				\
	public:										\
		this_class(void);

// includes IMPLEMENT_DYNAMIC in the subsuite and provides the constructor
// implementation, subsuite instantiation, and CreateSubSuite implementation
#define IMPLEMENT_SUBSUITE(this_class, base_class, subsuite_name, subsuite_owner)	\
	IMPLEMENT_DYNAMIC(this_class, base_class)										\
	this_class::this_class(void)													\
	: base_class(subsuite_name, subsuite_owner)										\
	{																				\
	}																				\
	IMPLEMENT_INSTANTIATESUBSUITE(this_class)										\
	IMPLEMENT_CREATESUBSUITE(this_class)											\
/*	IMPLEMENT_DESTROYSUBSUITE(this_class)	*/										\
	IMPLEMENT_ISDEBUGBUILD(DEBUGBUILD)												\
	IMPLEMENT_DLLMAIN(this_class)


// define default RUN and NORUN constants
#define RUN CTest::RS_Run
#define DONTRUN CTest::RS_NoRun

// preconditions and postconditions are not completely defined yet
// for now, we'll just define these constants that will be used in the TEST macro
#define NO_PRE_COND NULL
#define NO_POST_COND NULL

///////////////////////////////////////////////////////////////////////////////
//	CSubSuite class

class CSubSuite: public CObject
{
	DECLARE_DYNAMIC(CSubSuite)

// data types
public:
	// typedef for the test list
	typedef CTypedPtrList<CPtrList, CTest*> TestList;

// constructor/destructor
public:
	CSubSuite(LPCSTR pszName, LPCSTR pszOwner);
	virtual ~CSubSuite();

// Attributes
public:
	CString GetName(void);
	CString GetOwner(void);
	TestList* GetTestList(void);

	CToolset* GetToolset(void);
	CLog* GetLog(void);

	CString GetCWD(void);
	CSettings *m_SuiteParams;
	CString m_CmdLine;
// Operations
public:
	// if the user wants to do any set up before the subsuite is run,
	// he/she needs to override SetUp and call the base class's SetUp.
	virtual void SetUp(BOOL bCleanUp);
	// if the user wants to do any clean up after the subsuite is run,
	// he/she needs to override CleanUp and call the base class's CleanUp.
	virtual void CleanUp(void);

// supporting operations
public:
	// Load Initialization should never be overridden
	void LoadInitialization(HINSTANCE hDLL);
	// do not override Initialize and Finalize unless you have a good reason
	virtual BOOL Initialize(CToolset* pToolset);
	virtual BOOL Finalize(void);


// test list
protected:
	// use DECLARE_TESTLIST in your derived class to override InitializeTestList
	virtual void InitializeTestList(void) = 0;

// Data
protected:
	// store the subsuite's name and owner
	CString m_strName;
	CString m_strOwner;
	// store the list of tests in this subsuite
	TestList m_listTests;
	// store the toolset and the log
	CToolset* m_pToolset;
	CLog* m_pLog;
	// store the current working directory
	CString m_strCWD;
	// store this DLLs handle
	HINSTANCE m_hDLL;

	// Store the start and end times of this subsuite.
	CTime m_timeStart;
	CTime m_timeEnd;

// no one should call these
protected:
	CSubSuite();
};

///////////////////////////////////////////////////////////////////////////////
// CGUISubSuite

class CGUISubSuite: public CSubSuite
{
	DECLARE_DYNAMIC(CGUISubSuite)

// ctor/dtor
public:
	CGUISubSuite(LPCSTR pszName, LPCSTR pszOwner);
	~CGUISubSuite();

// supporting operations
	virtual BOOL Initialize(CToolset* pToolset);
	virtual BOOL Finalize(void);

// no one should call these
protected:
	CGUISubSuite();
};

///////////////////////////////////////////////////////////////////////////////
// CIDESubSuite

class CIDESubSuite: public CGUISubSuite
{
	DECLARE_DYNAMIC(CIDESubSuite)

// ctor/dtor
public:
	CIDESubSuite(LPCSTR pszName, LPCSTR pszOwner);
	~CIDESubSuite();

// operations
public:
	CIDE* GetIDE(void);
	// in case memory leaks must be set to !=0
	void ExpectedMemLeaks(int nExpectedLeaks);

// supporting operations
	virtual BOOL Initialize(CToolset* pToolset);
	virtual BOOL Finalize(void);

	virtual void CleanUp(void);

// data
protected:
	CIDE* m_pIDE;

// no one should call these
protected:
	CIDESubSuite();
};

#endif //__SUBSUITE_H__
