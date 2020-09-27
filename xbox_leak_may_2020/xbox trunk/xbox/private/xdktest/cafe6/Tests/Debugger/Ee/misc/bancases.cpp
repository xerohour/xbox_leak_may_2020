///////////////////////////////////////////////////////////////////////////////
//  BANCASES.CPP
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Implementation of the CBitAndCases class
//

#include "stdafx.h"
#include "bancases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CBitAndCases, CDebugTestSet, "BitAnd", 162, CMiscSubSuite)


void CBitAndCases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void CBitAndCases::Run(void)

	{
	InitProject("band");
	bps.SetBreakpoint("break_here");
	dbg.Go();
	cxx.Enable();

	WriteLog(cxx.ExpressionValueIs("pG->c & *Nc()", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nc() & pG->c", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->c & *Nsc()", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nsc() & pG->c", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->c & *Ns()", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ns() & pG->c", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->c & *Ni()", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ni() & pG->c", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->c & *Nuc()", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nuc() & pG->c", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->c & *Nus()", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nus() & pG->c", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->c & *Nui()", 5) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nui() & pG->c", 5) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->c & *Nl()", 6L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nl() & pG->c", 6L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->c & *Nul()", 7L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nul() & pG->c", 7L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->sc & *Nc()", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nc() & pG->sc", 0) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->sc & *Nsc()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nsc() & pG->sc", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->sc & *Ns()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ns() & pG->sc", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->sc & *Ni()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ni() & pG->sc", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->sc & *Nuc()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nuc() & pG->sc", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->sc & *Nus()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nus() & pG->sc", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->sc & *Nui()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nui() & pG->sc", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->sc & *Nl()", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nl() & pG->sc", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->sc & *Nul()", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nul() & pG->sc", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->s & *Nc()", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nc() & pG->s", 1) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->s & *Nsc()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nsc() & pG->s", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->s & *Ns()", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ns() & pG->s", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->s & *Ni()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ni() & pG->s", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->s & *Nuc()", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nuc() & pG->s", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->s & *Nus()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nus() & pG->s", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->s & *Nui()", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nui() & pG->s", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->s & *Nl()", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nl() & pG->s", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->s & *Nul()", 9L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nul() & pG->s", 9L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->i & *Nc()", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nc() & pG->i", 2) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->i & *Nsc()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nsc() & pG->i", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->i & *Ns()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ns() & pG->i", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->i & *Ni()", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ni() & pG->i", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->i & *Nuc()", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nuc() & pG->i", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->i & *Nus()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nus() & pG->i", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->i & *Nui()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nui() & pG->i", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->i & *Nl()", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nl() & pG->i", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->i & *Nul()", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nul() & pG->i", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->uc & *Nc()", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nc() & pG->uc", 3) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->uc & *Nsc()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nsc() & pG->uc", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->uc & *Ns()", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ns() & pG->uc", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->uc & *Ni()", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ni() & pG->uc", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->uc & *Nuc()", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nuc() & pG->uc", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->uc & *Nus()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nus() & pG->uc", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->uc & *Nui()", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nui() & pG->uc", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->uc & *Nl()", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nl() & pG->uc", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->uc & *Nul()", 11L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nul() & pG->uc", 11L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->us & *Nc()", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nc() & pG->us", 4) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->us & *Nsc()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nsc() & pG->us", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->us & *Ns()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ns() & pG->us", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->us & *Ni()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ni() & pG->us", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->us & *Nuc()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nuc() & pG->us", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->us & *Nus()", 12) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nus() & pG->us", 12) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->us & *Nui()", 12) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nui() & pG->us", 12) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->us & *Nl()", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nl() & pG->us", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->us & *Nul()", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nul() & pG->us", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ui & *Nc()", 5) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nc() & pG->ui", 5) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ui & *Nsc()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nsc() & pG->ui", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ui & *Ns()", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ns() & pG->ui", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ui & *Ni()", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ni() & pG->ui", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ui & *Nuc()", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nuc() & pG->ui", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ui & *Nus()", 12) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nus() & pG->ui", 12) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ui & *Nui()", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nui() & pG->ui", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ui & *Nl()", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nl() & pG->ui", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ui & *Nul()", 13L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nul() & pG->ui", 13L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->l & *Nc()", 6L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nc() & pG->l", 6L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->l & *Nsc()", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nsc() & pG->l", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->l & *Ns()", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ns() & pG->l", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->l & *Ni()", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ni() & pG->l", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->l & *Nuc()", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nuc() & pG->l", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->l & *Nus()", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nus() & pG->l", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->l & *Nui()", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nui() & pG->l", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->l & *Nl()", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nl() & pG->l", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->l & *Nul()", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nul() & pG->l", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ul & *Nc()", 7L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nc() & pG->ul", 7L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ul & *Nsc()", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nsc() & pG->ul", 8L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ul & *Ns()", 9L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ns() & pG->ul", 9L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ul & *Ni()", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Ni() & pG->ul", 10L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ul & *Nuc()", 11L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nuc() & pG->ul", 11L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ul & *Nus()", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nus() & pG->ul", 12L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ul & *Nui()", 13L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nui() & pG->ul", 13L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ul & *Nl()", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nl() & pG->ul", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("pG->ul & *Nul()", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("*Nul() & pG->ul", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
    }
