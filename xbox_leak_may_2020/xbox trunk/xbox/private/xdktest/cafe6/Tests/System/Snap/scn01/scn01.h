///////////////////////////////////////////////////////////////////////////////
//	scn01.H
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Declaration of the Cscn01Test class

#ifndef __scn01_H__
#define __scn01_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\sysset.h"
#include "scn01sub.h"

///////////////////////////////////////////////////////////////////////////////
//	CCreateAppWizAppTest class

class CCreateAppWizAppTest : public CSystemTestSet
{
	DECLARE_TEST(CCreateAppWizAppTest, CCreateBuildDebugAppWizAppSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:

// Test Cases
protected:
	BOOL CreateAppWizApp();
//	BOOL AddMoreOptions();

// Utilities

};

///////////////////////////////////////////////////////////////////////////////
//	CAddDialogTest class

class CAddDialogTest : public CSystemTestSet
{
	DECLARE_TEST(CAddDialogTest, CCreateBuildDebugAppWizAppSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:

// Test Cases
protected:
	BOOL AddDialog( LPCSTR szDialogCaption, LPCSTR szDialogID, LPCSTR szDefaultDialogID = "IDD_DIALOG1");
	BOOL AddMenu( LPCSTR szMenuName, LPCSTR szMenuItem );
	BOOL CreateMenuItemMsgHandler(LPCSTR szMenuName, LPCSTR szMenuItem);
	BOOL CreateTestDialogClass(LPCSTR szDialogName);
	BOOL ModifyCodeForDialog(LPCSTR szDialogName);
	BOOL Build( void );

// Utilities

};

#endif //__scn01_H__
