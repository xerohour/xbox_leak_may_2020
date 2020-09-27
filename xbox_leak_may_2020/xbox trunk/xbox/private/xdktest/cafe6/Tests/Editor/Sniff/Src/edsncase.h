///////////////////////////////////////////////////////////////////////////////
//	edsncase.h
//
//	Created by :			
//		GeorgeCh
//
//	Description :
//		Declaration of the Edsnif01TestSet Class
//

#include "resource.h"

#ifndef __EDSNCASE_H__
#define __EDSNCASE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "edsntype.h"
#include "emulcase.h"

#define ID_EDIT_COLUMN "^+{F8}"

///////////////////////////////////////////////////////////////////////////////
//	Sniff test classes

class SmartIndent : public CTest
{
	DECLARE_TEST(SmartIndent, EditorSniff)

public:
	virtual void Run(void);
	// Test cases --------------
	virtual	BOOL PoundCases();
	virtual BOOL Code1();
	virtual	BOOL Class1();
	virtual BOOL Code2();
	virtual BOOL Strings();
	virtual	BOOL MultiLineComments();
	virtual	BOOL CaseSwitch();

};


class EdsnifSelection : public CTest
{
	DECLARE_TEST(EdsnifSelection, EditorSniff)

public:
	virtual void Run(void);

// Test cases
	virtual BOOL CutCpyDelPst01( );
	virtual BOOL CutCpyDelPst02( );
	virtual BOOL CutCpyDelPst03( );
	virtual BOOL CutCpyDelPst04( );
};

class EdsnifTabIndent : public CTest
{
	DECLARE_TEST(EdsnifTabIndent, EditorSniff)

public:
	virtual void Run(void);

// Test cases

	virtual BOOL EnterTabs01( );
};

class EdsnifOvrInsert : public CTest
{
	DECLARE_TEST(EdsnifOvrInsert, EditorSniff)

public:
	virtual void Run(void);

// Test cases

	virtual BOOL OvrInsert01();	
};

class EdsnifToolsMenu : public CTest
{
	DECLARE_TEST(EdsnifToolsMenu, EditorSniff)

public:
	virtual void Run(void);
	// since this class is a base class for tests, it must have a ctor
	// that takes all test parameters for proper base class initialization

// Test cases

	virtual BOOL SaveOutputWindow(CString strOutputName);
	virtual BOOL AddTool(CString strToolPath,
							 CString strArguments,
							 CString strMenuText,
 							 int     OutputForm);
	virtual BOOL Verify(CString strTestName,
						CString strArguments,
						CString strMenuText,
						CString strShortCut,
						int     CompareMethod,
						int     OutputForm,
						BOOL	FILEOPEN);
	virtual BOOL General( );
};

BOOL PositionVerify(COSource testSource,
						int testCol1,
						int testLine1,
						int baseCol1,
						int baseLine1,
						LPCTSTR KeysToDo);

BOOL PositionSetup(COSource testSource,
						int testCol1,
						int testLine1,
						LPCTSTR KeysToDo);
BOOL LocationVerify(COSource testSource,
						int baseCol1,
						int baseLine1);


/*
class EdsnifEmulations : public CTest
{
	DECLARE_TEST(EdsnifEmulations, EditorSniff)

public:
	virtual void Run(void);
	// since this class is a base class for tests, it must have a ctor
	// that takes all test parameters for proper base class initialization
	virtual BOOL Emulations(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL);

// Test cases

//	virtual BOOL General( );
};
*/


class cVC2Position : public CTest
{
	DECLARE_TEST(cVC2Position, EditorSniff)

public:
	virtual void Run(void);

	virtual BOOL VC2Position( );
};

class cVC3Position : public CTest
{
	DECLARE_TEST(cVC3Position, EditorSniff)

public:
	
	virtual void Run(void);

	virtual BOOL VC3Position( );
};

class cBriefPosition : public CTest
{
	DECLARE_TEST(cBriefPosition, EditorSniff)

public:
	
	virtual void Run(void);

	virtual BOOL BriefPosition( );
};

class cEpsilonPosition : public CTest
{
	DECLARE_TEST(cEpsilonPosition, EditorSniff)

public:
	
	virtual void Run(void);

	virtual BOOL EpsilonPosition( );
};


#endif //__EDSNCASE_H__

