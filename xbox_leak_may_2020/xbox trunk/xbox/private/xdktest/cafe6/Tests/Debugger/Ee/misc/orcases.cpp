///////////////////////////////////////////////////////////////////////////////
//  ORCASES.CPP
//
//  Created by :            Date :
//      MichMa                  1/25/94
//
//  Description :
//      Implementation of the COrCases class
//

#include "stdafx.h"
#include "orcases.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(COrCases, CDebugTestSet, "Or", 162, CMiscSubSuite)


void COrCases::PreRun(void)
	{
	// call the base class
	CTest::PreRun();
	}


void COrCases::Run(void)

	{
	InitProject("or");
	bps.SetBreakpoint("break_here");
	dbg.Go();
	cxx.Enable();

	WriteLog(cxx.ExpressionValueIs("Ic[x_0] | Pc[x[x_0]]", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pc[x[x_0]] | Ic[x_0]", 7) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ic[x_0] | Psc[x[x_1]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Psc[x[x_1]] | Ic[x_0]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ic[x_0] | Ps[x[x_2]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ps[x[x_2]] | Ic[x_0]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ic[x_0] | Pi[x[x_3]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pi[x[x_3]] | Ic[x_0]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ic[x_0] | Puc[x[x_4]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Puc[x[x_4]] | Ic[x_0]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ic[x_0] | Pus[x[x_5]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pus[x[x_5]] | Ic[x_0]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ic[x_0] | Pui[x[x_6]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pui[x[x_6]] | Ic[x_0]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ic[x_0] | Pl[x[x_7]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pl[x[x_7]] | Ic[x_0]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ic[x_0] | Pul[x[x_8]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pul[x[x_8]] | Ic[x_0]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Isc[x_1] | Pc[x[x_0]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pc[x[x_0]] | Isc[x_1]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Isc[x_1] | Psc[x[x_1]]", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Psc[x[x_1]] | Isc[x_1]", 8) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Isc[x_1] | Ps[x[x_2]]", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ps[x[x_2]] | Isc[x_1]", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Isc[x_1] | Pi[x[x_3]]", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pi[x[x_3]] | Isc[x_1]", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Isc[x_1] | Puc[x[x_4]]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Puc[x[x_4]] | Isc[x_1]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Isc[x_1] | Pus[x[x_5]]", 12) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pus[x[x_5]] | Isc[x_1]", 12) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Isc[x_1] | Pui[x[x_6]]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pui[x[x_6]] | Isc[x_1]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Isc[x_1] | Pl[x[x_7]]", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pl[x[x_7]] | Isc[x_1]", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Isc[x_1] | Pul[x[x_8]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pul[x[x_8]] | Isc[x_1]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Is[x_2] | Pc[x[x_0]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pc[x[x_0]] | Is[x_2]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Is[x_2] | Psc[x[x_1]]", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Psc[x[x_1]] | Is[x_2]", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Is[x_2] | Ps[x[x_2]]", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ps[x[x_2]] | Is[x_2]", 9) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Is[x_2] | Pi[x[x_3]]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pi[x[x_3]] | Is[x_2]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Is[x_2] | Puc[x[x_4]]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Puc[x[x_4]] | Is[x_2]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Is[x_2] | Pus[x[x_5]]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pus[x[x_5]] | Is[x_2]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Is[x_2] | Pui[x[x_6]]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pui[x[x_6]] | Is[x_2]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Is[x_2] | Pl[x[x_7]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pl[x[x_7]] | Is[x_2]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Is[x_2] | Pul[x[x_8]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pul[x[x_8]] | Is[x_2]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ii[x_3] | Pc[x[x_0]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pc[x[x_0]] | Ii[x_3]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ii[x_3] | Psc[x[x_1]]", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Psc[x[x_1]] | Ii[x_3]", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ii[x_3] | Ps[x[x_2]]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ps[x[x_2]] | Ii[x_3]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ii[x_3] | Pi[x[x_3]]", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pi[x[x_3]] | Ii[x_3]", 10) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ii[x_3] | Puc[x[x_4]]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Puc[x[x_4]] | Ii[x_3]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ii[x_3] | Pus[x[x_5]]", 14) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pus[x[x_5]] | Ii[x_3]", 14) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ii[x_3] | Pui[x[x_6]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pui[x[x_6]] | Ii[x_3]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ii[x_3] | Pl[x[x_7]]", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pl[x[x_7]] | Ii[x_3]", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ii[x_3] | Pul[x[x_8]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pul[x[x_8]] | Ii[x_3]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iuc[x_4] | Pc[x[x_0]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pc[x[x_0]] | Iuc[x_4]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iuc[x_4] | Psc[x[x_1]]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Psc[x[x_1]] | Iuc[x_4]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iuc[x_4] | Ps[x[x_2]]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ps[x[x_2]] | Iuc[x_4]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iuc[x_4] | Pi[x[x_3]]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pi[x[x_3]] | Iuc[x_4]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iuc[x_4] | Puc[x[x_4]]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Puc[x[x_4]] | Iuc[x_4]", 11) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iuc[x_4] | Pus[x[x_5]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pus[x[x_5]] | Iuc[x_4]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iuc[x_4] | Pui[x[x_6]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pui[x[x_6]] | Iuc[x_4]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iuc[x_4] | Pl[x[x_7]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pl[x[x_7]] | Iuc[x_4]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iuc[x_4] | Pul[x[x_8]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pul[x[x_8]] | Iuc[x_4]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ius[x_5] | Pc[x[x_0]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pc[x[x_0]] | Ius[x_5]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ius[x_5] | Psc[x[x_1]]", 12) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Psc[x[x_1]] | Ius[x_5]", 12) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ius[x_5] | Ps[x[x_2]]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ps[x[x_2]] | Ius[x_5]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ius[x_5] | Pi[x[x_3]]", 14) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pi[x[x_3]] | Ius[x_5]", 14) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ius[x_5] | Puc[x[x_4]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Puc[x[x_4]] | Ius[x_5]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ius[x_5] | Pus[x[x_5]]", 12) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pus[x[x_5]] | Ius[x_5]", 12) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ius[x_5] | Pui[x[x_6]]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pui[x[x_6]] | Ius[x_5]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ius[x_5] | Pl[x[x_7]]", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pl[x[x_7]] | Ius[x_5]", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ius[x_5] | Pul[x[x_8]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pul[x[x_8]] | Ius[x_5]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iui[x_6] | Pc[x[x_0]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pc[x[x_0]] | Iui[x_6]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iui[x_6] | Psc[x[x_1]]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Psc[x[x_1]] | Iui[x_6]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iui[x_6] | Ps[x[x_2]]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ps[x[x_2]] | Iui[x_6]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iui[x_6] | Pi[x[x_3]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pi[x[x_3]] | Iui[x_6]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iui[x_6] | Puc[x[x_4]]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Puc[x[x_4]] | Iui[x_6]", 15) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iui[x_6] | Pus[x[x_5]]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pus[x[x_5]] | Iui[x_6]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iui[x_6] | Pui[x[x_6]]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pui[x[x_6]] | Iui[x_6]", 13) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iui[x_6] | Pl[x[x_7]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pl[x[x_7]] | Iui[x_6]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iui[x_6] | Pul[x[x_8]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pul[x[x_8]] | Iui[x_6]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Il[x_7] | Pc[x[x_0]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pc[x[x_0]] | Il[x_7]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Il[x_7] | Psc[x[x_1]]", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Psc[x[x_1]] | Il[x_7]", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Il[x_7] | Ps[x[x_2]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ps[x[x_2]] | Il[x_7]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Il[x_7] | Pi[x[x_3]]", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pi[x[x_3]] | Il[x_7]", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Il[x_7] | Puc[x[x_4]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Puc[x[x_4]] | Il[x_7]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Il[x_7] | Pus[x[x_5]]", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pus[x[x_5]] | Il[x_7]", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Il[x_7] | Pui[x[x_6]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pui[x[x_6]] | Il[x_7]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Il[x_7] | Pl[x[x_7]]", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pl[x[x_7]] | Il[x_7]", 14L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Il[x_7] | Pul[x[x_8]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pul[x[x_8]] | Il[x_7]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iul[x_8] | Pc[x[x_0]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pc[x[x_0]] | Iul[x_8]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iul[x_8] | Psc[x[x_1]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Psc[x[x_1]] | Iul[x_8]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iul[x_8] | Ps[x[x_2]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Ps[x[x_2]] | Iul[x_8]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iul[x_8] | Pi[x[x_3]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pi[x[x_3]] | Iul[x_8]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iul[x_8] | Puc[x[x_4]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Puc[x[x_4]] | Iul[x_8]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iul[x_8] | Pus[x[x_5]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pus[x[x_5]] | Iul[x_8]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iul[x_8] | Pui[x[x_6]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pui[x[x_6]] | Iul[x_8]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iul[x_8] | Pl[x[x_7]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pl[x[x_7]] | Iul[x_8]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Iul[x_8] | Pul[x[x_8]]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);
	WriteLog(cxx.ExpressionValueIs("Pul[x[x_8]] | Iul[x_8]", 15L) ? PASSED : FAILED, "line = %d\n", __LINE__);

	cxx.Disable();
    }
