///////////////////////////////////////////////////////////////////////////////
//	EECASEPP.H
//
//	Created by:			
//		dverma
//
//	Description :
//		VC6 Processor Pack testcases.
//

#ifndef __EECASEPP_H__
#define __EECASEPP_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\DbgTestBase.h"
#include "eesub.h"

///////////////////////////////////////////////////////////////////////////////
//	CEEPP class

class CEEPP : public CDbgTestBase
	
	{
	DECLARE_TEST(CEEPP, CEESubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);

	// testcases.
	protected:
		void testRegVal(COExpEval& cxx, char *value, char *message);
		void testRegVal(int regid, COExpEval& cxx, char *value, char *message);
		void testRegVal(char *varname, char *vartype, COExpEval& cxx, char *value, char *message);
		void testRegVal(COExpEval& cxx, char *value1, char *value2, char *value3, char *value4, char *message);
		void testRegVal(int reg_id, COExpEval& cxx, char *value1, char *value2, char *value3, char *value4, char *message);
		void test_MMX(COExpEval& cxx, CODebug& dbg, COSource& src);
		void test_3dNow(COExpEval& cxx, CODebug& dbg, COSource& src);
		void test_3dNowEnhanced(COExpEval& cxx, CODebug& dbg, COSource& src);
		void test_Katmai(COExpEval& cxx, CODebug& dbg, COSource& src);

	// Attributes
	public:
		// the base name of the localized directories and files we will use.
		CString m_strProjBase;
		// the location of the unlocalized sources, under which the projects we will use will be located.
		CString m_strSrcDir;
		// the locations of the dll and exe projects, and their localized sources, that we will use for this test.
		CString m_strExeDir;
};

#endif //__EECASEPP_H__
