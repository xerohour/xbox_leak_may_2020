///////////////////////////////////////////////////////////////////////////////
//  REMCASES.CPP
//
//  Created by :            Date :
//      MichMa                  1/22/94
//
//  Description :
//      Implementation of the CRemainCases class
//

#include "stdafx.h"
#include "remcases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CRemainCases, CDebugTestSet, "Remain", 162, CMiscSubSuite)


void CRemainCases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CRemainCases::Run(void)

	{
	InitProject("remain");
	bps.SetBreakpoint("break_here");
	dbg.Go();
	cxx.Enable();

    WriteLog(cxx.ExpressionValueIs("pE1->c % Mc()", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mc() % pE1->c", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE1->c % Msc()", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Msc() % pE1->c", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE1->c % Ms()", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ms() % pE1->c", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE1->c % Mi()", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mi() % pE1->c", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE1->c % Muc()", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Muc() % pE1->c", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE1->c % Mus()", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mus() % pE1->c", 5) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE1->c % Mui()", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mui() % pE1->c", 6) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE1->c % Ml()", 7L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ml() % pE1->c", 0L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE1->c % Mul()", 7L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mul() % pE1->c", 1L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE2->sc % Mc()", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mc() % pE2->sc", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE2->sc % Msc()", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Msc() % pE2->sc", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE2->sc % Ms()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ms() % pE2->sc", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE2->sc % Mi()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mi() % pE2->sc", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE2->sc % Muc()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Muc() % pE2->sc", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE2->sc % Mus()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mus() % pE2->sc", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE2->sc % Mui()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mui() % pE2->sc", 5) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE2->sc % Ml()", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ml() % pE2->sc", 6L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE2->sc % Mul()", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mul() % pE2->sc", 7L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE3->s % Mc()", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mc() % pE3->s", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE3->s % Msc()", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Msc() % pE3->s", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE3->s % Ms()", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ms() % pE3->s", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE3->s % Mi()", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mi() % pE3->s", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE3->s % Muc()", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Muc() % pE3->s", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE3->s % Mus()", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mus() % pE3->s", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE3->s % Mui()", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mui() % pE3->s", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE3->s % Ml()", 9L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ml() % pE3->s", 5L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE3->s % Mul()", 9L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mul() % pE3->s", 6L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE4->i % Mc()", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mc() % pE4->i", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE4->i % Msc()", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Msc() % pE4->i", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE4->i % Ms()", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ms() % pE4->i", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE4->i % Mi()", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mi() % pE4->i", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE4->i % Muc()", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Muc() % pE4->i", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE4->i % Mus()", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mus() % pE4->i", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE4->i % Mui()", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mui() % pE4->i", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE4->i % Ml()", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ml() % pE4->i", 4L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE4->i % Mul()", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mul() % pE4->i", 5L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE5->uc % Mc()", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mc() % pE5->uc", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE5->uc % Msc()", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Msc() % pE5->uc", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE5->uc % Ms()", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ms() % pE5->uc", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE5->uc % Mi()", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mi() % pE5->uc", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE5->uc % Muc()", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Muc() % pE5->uc", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE5->uc % Mus()", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mus() % pE5->uc", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE5->uc % Mui()", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mui() % pE5->uc", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE5->uc % Ml()", 11L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ml() % pE5->uc", 3L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE5->uc % Mul()", 11L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mul() % pE5->uc", 4L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE6->us % Mc()", 5) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mc() % pE6->us", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE6->us % Msc()", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Msc() % pE6->us", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE6->us % Ms()", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ms() % pE6->us", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE6->us % Mi()", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mi() % pE6->us", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE6->us % Muc()", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Muc() % pE6->us", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE6->us % Mus()", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mus() % pE6->us", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE6->us % Mui()", 12) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mui() % pE6->us", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE6->us % Ml()", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ml() % pE6->us", 2L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE6->us % Mul()", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mul() % pE6->us", 3L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE7->ui % Mc()", 6) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mc() % pE7->ui", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE7->ui % Msc()", 5) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Msc() % pE7->ui", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE7->ui % Ms()", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ms() % pE7->ui", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE7->ui % Mi()", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mi() % pE7->ui", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE7->ui % Muc()", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Muc() % pE7->ui", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE7->ui % Mus()", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mus() % pE7->ui", 12) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE7->ui % Mui()", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mui() % pE7->ui", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE7->ui % Ml()", 13L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ml() % pE7->ui", 1L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE7->ui % Mul()", 13L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mul() % pE7->ui", 2L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE8->l % Mc()", 0L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mc() % pE8->l", 7L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE8->l % Msc()", 6L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Msc() % pE8->l", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE8->l % Ms()", 5L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ms() % pE8->l", 9L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE8->l % Mi()", 4L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mi() % pE8->l", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE8->l % Muc()", 3L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Muc() % pE8->l", 11L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE8->l % Mus()", 2L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mus() % pE8->l", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE8->l % Mui()", 1L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mui() % pE8->l", 13L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE8->l % Ml()", 0L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ml() % pE8->l", 0L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE8->l % Mul()", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mul() % pE8->l", 1L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE9->ul % Mc()", 1L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mc() % pE9->ul", 7L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE9->ul % Msc()", 7L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Msc() % pE9->ul", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE9->ul % Ms()", 6L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ms() % pE9->ul", 9L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE9->ul % Mi()", 5L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mi() % pE9->ul", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE9->ul % Muc()", 4L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Muc() % pE9->ul", 11L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE9->ul % Mus()", 3L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mus() % pE9->ul", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE9->ul % Mui()", 2L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mui() % pE9->ul", 13L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE9->ul % Ml()", 1L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Ml() % pE9->ul", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("pE9->ul % Mul()", 0L) ? PASSED : FAILED, "line = %d\n", __LINE__);
    WriteLog(cxx.ExpressionValueIs("Mul() % pE9->ul", 0L) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
    }
