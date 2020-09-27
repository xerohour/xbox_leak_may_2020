///////////////////////////////////////////////////////////////////////////////
//	RetVcase.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CRetValIDETest Class
//

#ifndef __RetVcase_H__
#define __RetVcase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\dbgset.h"
#include "retvsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CRetValIDETest class

class CRetValIDETest : public CDebugTestSet
{
		DECLARE_TEST(CRetValIDETest, CRetValSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);

	// Test cases
	private:
		void StepOverFunctionsReturningNativeTypes(void);
		void StepOverFunctionReturningClassObject(void);
		void StepOverFunctionReturningPointerToClassObject(void);
		void StepOverFunctionThrowingExceptionToParent(void);
		void StepOutOfWinMain(void);
		void StepOverDllFunction(void);
		void StepOverConstructorAndVirtualMemberFunction(void);
		void StepOverFunctionWithOnlyLineNumberDebugInfo(void);
		void StepOverFunctionWithNoDebugInfo(void);
		void StepOverStaticallyLinkedNonDebugCrtFunction(void);
		void StepOverWin32ApiFunction(void);
		void StepOutOfFunctionWithF8(void);
		void StepOutOfFunctionWithF10(void);
		void StepOutOfFunctionWithShiftF7(void);
		void StepOverLineWithNoFunctionCall(void);
		void VerifyNoReturnValueDisplayedAfterGo(void);
		void VerifyReturnValueDisappearsWhenAssigningValue(void);
		void VerifyReturnValueDisappearsWhenNotAssigningValue(void);
		void VerifyReturnValueStaysAfterResettingNextStatement(void);
		void VerifyReturnValueStaysAfterStackNavigation(void);
		void VerifyReturnValueForFastcallCallingConvention(void);
		void StepOutOfMultipleLevelsOfRecursion(void);
		void StepOutOfCallbackFunction(void);
		void VerifyReturnValueFieldCannotBeEdited(void);
		void VerifyNoReturnValueDisplayedAfterRestart(void);
		void VerifyNoReturnValueDisplayedAfterStopAndRestart(void); 
		void ToggleReturnValueOffAndOn(void);

	// Data
	private:
		EXPR_INFO expr_info_actual[4], expr_info_expected[4];
	};

#endif //__RetVcase_H__
