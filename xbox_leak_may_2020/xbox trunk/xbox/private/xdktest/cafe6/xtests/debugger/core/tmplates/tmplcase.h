///////////////////////////////////////////////////////////////////////////////
//	TMPLCASE.H
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CTemplatesIDETest Class
//

#ifndef __TMPLCASE_H__
#define __TMPLCASE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\dbgtestbase.h"
#include "tmplsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CTemplatesIDETest class

class CTemplatesIDETest : public CDbgTestBase
	
	{
	DECLARE_TEST(CTemplatesIDETest, CTemplatesSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);

	// Tests
	public:
		void StepIntoAndOutOfTemplateFunction(void);
		void StepOverTemplateFunction(void);
		void StepToCursorInTemplateFunction(void);
		void SetBreakpointOnLineInTemplateFunctionWhileDebugging(void);
		void SetBreakpointOnLineInTemplateFunctionWhileNotDebugging(void);
		void VerifyTemplateParameterOnStack(void);
		void GoToDisassemblyFromWithinTemplateFunction(void);
		void StepIntoSpecificTemplateFunction(void);

		void SetNextStatementInTemplateFunction(void);
		void StepToCursorInTemplateFunctionInStack(void);
		void EvaluateSTLObjectWithLongName(void);
		void StepToCursorInTemplateFunctionToStartDebugging(void);
		void HitBreakpointOnAmbiguousTemplateFunction(void);
		void HitBreakpointOnNonAmbiguousTemplateFunction(void);
		void DisableBreakpointInTemplateFunctionAndVerifyNotHit(void);
		void ToggleOffBreakpointInTemplateFunctionAndVerifyNotHit(void);
	};

#endif //__TMPLCASE_H__
