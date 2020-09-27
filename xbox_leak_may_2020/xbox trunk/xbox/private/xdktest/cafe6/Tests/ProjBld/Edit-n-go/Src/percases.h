///////////////////////////////////////////////////////////////////////////////
//	PERCASES.H
//
//	Created by :			Date :
//		WayneBr				10/3/94
//
//	Description :
//		Declaration of the CSysTestCases class
//

#ifndef __PERCASES_H__
#define __PERCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

typedef struct Project_Info {
		CString	Project;
		CString SrcProjDir;
		CString ProjDir;
		CString Makefile;
		CString	Dep_Make;
		CString Target;
		CString	AddFcn;
		CString AddFcn_File;
		DWORD	AddFcn_Line;
		CString AddDef;
		CString	AddDef_File;
		DWORD	AddDef_Line;
		CString AddRef;
		CString	AddRef_File;
		DWORD	AddRef_Line;
		CString ModCode;
		DWORD	Time_Start_IDE;
		DWORD	Time_Start_Debug;
		DWORD	Time_Restart_Debug;
		DWORD	Time_Full_Build;
		DWORD	Time_Major_Build;
		DWORD	Time_Minor_Build;
		_int64 	Disk_Before;
		_int64 	Disk_After;

		} Project_Info;


#define MAX_SRC		3
#define DBG_START	1
#define DBG_RESTART	2

///////////////////////////////////////////////////////////////////////////////
//	BOOL CPerformanceCases:: class

class CPerformanceCases : public CTest
{
	DECLARE_TEST(CPerformanceCases, CSniffDriver)

// ctor because this class is a base class for tests
	CPerformanceCases(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL);

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(Project_Info* pProjInfo);

// Data
protected:
	CString FCN;
	CString MOD;

	CODebug m_dbg;
	COBrowse m_brz;
	COProject m_prj;
	COProject m_prj_dep;
	COResScript m_res;
	COBreakpoints m_bp;
	COExpEval m_cxx;
	COSource m_src[MAX_SRC];
	
// Test Cases		    
protected:
	BOOL Setup(Project_Info* );
	BOOL Shutdown(Project_Info* , BOOL);
	BOOL CleanBuild(Project_Info *);
	BOOL MajorChange(Project_Info *);
	BOOL MinorChange(Project_Info *);
	BOOL LogData(Project_Info *);


// Utilities
protected:
	void RemoveTree(  CString strRootDir );
	void MeasureTree( CString strRootDir, _int64 *pDirSize );

	
	COSource *OpenSource(LPCSTR filename);
	void CloseALLSources();

	CString ConvertTime(unsigned long Time);
	DWORD   GetBuildTime();
	DWORD   GetDBGTime(int DBG_Type = DBG_START);

	inline BOOL MyWaitForBreak(HWND hwnd);
};

#endif // PERCASES_H__
