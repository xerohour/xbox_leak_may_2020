///////////////////////////////////////////////////////////////////////////////
//	Dbg_stress_base.CPP
//											 
//	Created by :			
//		Xbox XDK Test		

#include "stdafx.h"
#include "Dbg_stress_base.h"
#include "PerfCounter.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

											   
IMPLEMENT_TEST(CDbgStressStopAndGoTest, CDbgTestBase, "Debugger Stress Stop & Go", -1, CDbgStressSubSuite)
IMPLEMENT_TEST(CDbgStressRestartAndGoTest, CDbgTestBase, "Debugger Stress Restart & Go", -1, CDbgStressSubSuite)
IMPLEMENT_TEST(CDbgStressBreakAndGoTest, CDbgTestBase, "Debugger Stress Break & Go", -1, CDbgStressSubSuite)
IMPLEMENT_TEST(CDbgStressConditionalBPNonCountingTest, CDbgTestBase, "Debugger Stress Conditional Breakpoint (non-counting type)", -1, CDbgStressSubSuite)
IMPLEMENT_TEST(CDbgStressConditionalBPCountingTest, CDbgTestBase, "Debugger Stress Conditional Breakpoint (counting type)", -1, CDbgStressSubSuite)
IMPLEMENT_TEST(CDbgStressDataBPTest, CDbgTestBase, "Debugger Stress Data Breakpoint", -1, CDbgStressSubSuite)

#define TIMESTORUN 250
#define TIMESTORUNSTR "250"

void CDbgStressStopAndGoTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CDbgStressStopAndGoTest::Run()
{	 
	XSAFETY;
	
	if( SetProject("stress01\\stress01", PROJECT_XBE))
	{	  
		bps.ClearAllBreakpoints();
		XSAFETY;
		StopAndGo();
		XSAFETY;
	}
	else
		m_pLog->RecordFailure("SetProject failed for \"stress01\"." );
}

void CDbgStressRestartAndGoTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CDbgStressRestartAndGoTest::Run()
{	 
	XSAFETY;
	
	if( SetProject("stress01\\stress01", PROJECT_XBE))
	{	  
		bps.ClearAllBreakpoints();
		XSAFETY;
		RestartAndGo();
		XSAFETY;
	}
	else
		m_pLog->RecordFailure("SetProject failed for \"stress01\"." );
}



void CDbgStressBreakAndGoTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CDbgStressBreakAndGoTest::Run()
{	 
	XSAFETY;
	
	if( SetProject("stress01\\stress01", PROJECT_XBE))
	{	  
		bps.ClearAllBreakpoints();
		XSAFETY;
		BreakAndGo();
		XSAFETY;
	}
	else
		m_pLog->RecordFailure("SetProject failed for \"stress01\"." );
}


void CDbgStressConditionalBPNonCountingTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CDbgStressConditionalBPNonCountingTest::Run()
{	 
	XSAFETY;
	
	if( SetProject("stress01\\stress01", PROJECT_XBE))
	{	  
		bps.ClearAllBreakpoints();
		XSAFETY;
		CondBPNonCounting();
		XSAFETY;
	}
	else
		m_pLog->RecordFailure("SetProject failed for \"stress01\"." );
}


void CDbgStressConditionalBPCountingTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CDbgStressConditionalBPCountingTest::Run()
{	 
	XSAFETY;
	
	if( SetProject("stress01\\stress01", PROJECT_XBE))
	{	  
		bps.ClearAllBreakpoints();
		XSAFETY;
		CondBPCounting();
		XSAFETY;
	}
	else
		m_pLog->RecordFailure("SetProject failed for \"stress01\"." );
}


void CDbgStressDataBPTest::PreRun(void)
{
	// call the base class
	CTest::PreRun();

}

void CDbgStressDataBPTest::Run()
{	 
	XSAFETY;
	
	if( SetProject("stress01\\stress01", PROJECT_XBE))
	{	  
		bps.ClearAllBreakpoints();
		XSAFETY;
		DataBP();
		XSAFETY;
	}
	else
		m_pLog->RecordFailure("SetProject failed for \"stress01\"." );
}


/*
1.  Rapid fire Go, Stop, Go, Stop x 10,000 (with memory leak checks)
2.  Rapid fire Go, Restart, Restart x 10,000 (with memory leak checks)
3.  Rapid fire Go, break, Go, break x 10,000 (with memory leak checks)
4.  Tight 500,000 loop with a conditional data breakpoint (counting type)
5.  Tight 500,000 loop with a conditional data breakpoint (evaluate data type)
*/
///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

BOOL CDbgStressStopAndGoTest::StopAndGo( )
{
	long		workingset = 0;
	PerfCounter	PC;
	char		buf[1024];

 	LogTestHeader( "StopAndGoStress" );
	m_pLog->RecordInfo( "Start and Stop Debugger " TIMESTORUNSTR " Times" );
	bps.ClearAllBreakpoints();

	//
	// Get Memory usage on xbox
	//
	Sleep(10000);
	PC.LogData();
	workingset = PC.GetCur();
	wsprintf(buf, "MSDEV Working Set: %uk", workingset / 1024);
	m_pLog->RecordInfo(buf);

	for (unsigned count = 0; count < TIMESTORUN; count++)
	{
		wsprintf(buf, "Run: %d", count+1);
		m_pLog->RecordInfo(buf);
		EXPECT_TRUE( dbg.Go(NULL, NULL, NULL, WAIT_FOR_RUN) );
//		Sleep(20000);
		EXPECT_TRUE( dbg.StopDebugging() );
		Sleep(500);
		PC.LogData();
	}
	//
	// Let MSDEV Settle down before capturing the last sample
	//
	Sleep(10000);
	PC.LogData();
	//
	// Get Xbox and Dev memory usage
	//
	wsprintf(buf, "MSDEV Working Set: %uk (diff = %ldk)", PC.GetCur() / 1024, (PC.GetCur() - workingset) / 1024);
	m_pLog->RecordInfo(buf);
	wsprintf(buf, "MSDEV Working Set: Min = %uk / Max = %uk / Avg = %uk", 
		PC.GetMin() / 1024,
		PC.GetMax() / 1024,
		PC.GetAvg() / 1024
		);
	m_pLog->RecordInfo(buf);

	return (TRUE);
}

BOOL CDbgStressBreakAndGoTest::BreakAndGo( )
{
	long		workingset = 0;
	PerfCounter	PC;
	char		buf[1024];

 	LogTestHeader( "BreakAndGoStress" );
	m_pLog->RecordInfo( "Go and Break Debugger " TIMESTORUNSTR " Times" );
	bps.ClearAllBreakpoints();


	//
	// Get Memory usage on xbox
	//
	Sleep(10000);
	PC.LogData();
	workingset = PC.GetCur();
	wsprintf(buf, "MSDEV Working Set: %uk", workingset / 1024);
	m_pLog->RecordInfo(buf);
	dbg.Restart();
	dbg.SetSteppingMode(SRC);
	for (unsigned count = 0; count < TIMESTORUN; count++)
	{
		wsprintf(buf, "Run: %d", count+1);
		m_pLog->RecordInfo(buf);
		
		EXPECT_TRUE( dbg.Go(NULL, NULL, NULL, WAIT_FOR_RUN) );
		Sleep(5000);
		EXPECT_TRUE( dbg.Break() );
		if (dbg.GetDebugState() == 2)
		{
			MST.DoKeys( "{esc}" );
		}

		Sleep(2000);
		PC.LogData();
	}
	EXPECT_TRUE( dbg.StopDebugging() );
	//
	// Let MSDEV Settle down before capturing the last sample
	//
	Sleep(10000);
	PC.LogData();
	//
	// Get Xbox and Dev memory usage
	//
	wsprintf(buf, "MSDEV Working Set: %uk (diff = %ldk)", PC.GetCur() / 1024, (PC.GetCur() - workingset) / 1024);
	m_pLog->RecordInfo(buf);
	wsprintf(buf, "MSDEV Working Set: Min = %uk / Max = %uk / Avg = %uk", 
		PC.GetMin() / 1024,
		PC.GetMax() / 1024,
		PC.GetAvg() / 1024
		);
	m_pLog->RecordInfo(buf);

	return (TRUE);
}


BOOL CDbgStressRestartAndGoTest::RestartAndGo( )
{
	long		workingset = 0;
	PerfCounter	PC;
	char		buf[1024];

 	LogTestHeader( "RestartAndGoStress" );
	m_pLog->RecordInfo( "Start and Restart Debugger " TIMESTORUNSTR " Times" );
	bps.ClearAllBreakpoints();


	//
	// Get Memory usage on xbox
	//
	Sleep(10000);
	PC.LogData();
	workingset = PC.GetCur();
	wsprintf(buf, "MSDEV Working Set: %uk", workingset / 1024);
	m_pLog->RecordInfo(buf);

	EXPECT_TRUE( dbg.Restart() );
	EXPECT_TRUE( dbg.ShowNextStatement() ); // insure we're in source
	EXPECT_TRUE( src.Find("Sleep(10);") );
	EXPECT_TRUE( bps.SetBreakpoint() );

	for (unsigned count = 0; count < TIMESTORUN; count++)
	{
		wsprintf(buf, "Run: %d", count+1);
		m_pLog->RecordInfo(buf);
		
		EXPECT_TRUE( dbg.Go(NULL, NULL, NULL, WAIT_FOR_BREAK) );
		EXPECT_TRUE( dbg.Restart() );
		Sleep(500);
		PC.LogData();
	}
	EXPECT_TRUE( dbg.StopDebugging() );
	//
	// Let MSDEV Settle down before capturing the last sample
	//
	Sleep(10000);
	PC.LogData();
	//
	// Get Xbox and Dev memory usage
	//
	wsprintf(buf, "MSDEV Working Set: %uk (diff = %ldk)", PC.GetCur() / 1024, (PC.GetCur() - workingset) / 1024);
	m_pLog->RecordInfo(buf);
	wsprintf(buf, "MSDEV Working Set: Min = %uk / Max = %uk / Avg = %uk", 
		PC.GetMin() / 1024,
		PC.GetMax() / 1024,
		PC.GetAvg() / 1024
		);
	m_pLog->RecordInfo(buf);

	return (TRUE);
}


BOOL CDbgStressConditionalBPNonCountingTest::CondBPNonCounting( )
{
	long		workingset = 0;
	PerfCounter	PC;
	char		buf[1024];

 	LogTestHeader( "Conditional BP" );
	m_pLog->RecordInfo( "Conditional BP at location " TIMESTORUNSTR " Times" );
	bps.ClearAllBreakpoints();

	//
	// Get Memory usage on xbox
	//
	Sleep(10000);
	PC.LogData();
	workingset = PC.GetCur();
	wsprintf(buf, "MSDEV Working Set: %uk", workingset / 1024);
	m_pLog->RecordInfo(buf);

	//
	// insert a conditional breakpoint
	//
	EXPECT_TRUE( dbg.Restart() );
	EXPECT_TRUE( dbg.ShowNextStatement() ); // insure we're in source
	EXPECT_VALIDBP(bps.SetBreakOnExpr("(global_count % 2) == 1", COBP_TYPE_IF_EXP_TRUE));

	// make sure TIMESTORUN is greater than or equal the times that
	// global_count is changed in the target source (LoopLevel1)
	for (unsigned count = 0; count < TIMESTORUN; count++)
	{
		wsprintf(buf, "Run: %d", count+1);
		m_pLog->RecordInfo(buf);
		
		EXPECT_TRUE( dbg.Go(NULL, NULL, NULL, WAIT_FOR_BREAK) );
		Sleep(500);
		PC.LogData();
	}
	EXPECT_TRUE( dbg.StopDebugging() );

	//
	// Let MSDEV Settle down before capturing the last sample
	//
	Sleep(10000);
	PC.LogData();
	//
	// Get Xbox and Dev memory usage
	//
	wsprintf(buf, "MSDEV Working Set: %uk (diff = %ldk)", PC.GetCur() / 1024, (PC.GetCur() - workingset) / 1024);
	m_pLog->RecordInfo(buf);
	wsprintf(buf, "MSDEV Working Set: Min = %uk / Max = %uk / Avg = %uk", 
		PC.GetMin() / 1024,
		PC.GetMax() / 1024,
		PC.GetAvg() / 1024
		);
	m_pLog->RecordInfo(buf);


	return (TRUE);
}


BOOL CDbgStressConditionalBPCountingTest::CondBPCounting( )
{
	long		workingset = 0;
	PerfCounter	PC;
	char		buf[1024];

 	LogTestHeader( "Conditional BP with counting" );
	m_pLog->RecordInfo( "Conditional BP with counting " TIMESTORUNSTR " Times" );
	bps.ClearAllBreakpoints();

	//
	// Get Memory usage on xbox
	//

	Sleep(10000);
	PC.LogData();
	workingset = PC.GetCur();
	wsprintf(buf, "MSDEV Working Set: %uk", workingset / 1024);
	m_pLog->RecordInfo(buf);

	//
	// insert a conditional breakpoint with a count
	//
	EXPECT_TRUE( dbg.Restart() );
	EXPECT_TRUE( dbg.ShowNextStatement() ); // insure we're in source
	EXPECT_VALIDBP(bps.SetBreakOnExpr("(global_count % 2) == 1", COBP_TYPE_IF_EXP_TRUE, 1, TIMESTORUN));

	// make sure TIMESTORUN is greater than or equal the times that
	// global_count is changed in the target source (LoopLevel1)
	for (unsigned count = 0; count < TIMESTORUN; count++)
	{
		wsprintf(buf, "Run: %d", count+1);
		m_pLog->RecordInfo(buf);
		
		EXPECT_TRUE( dbg.Go(NULL, NULL, NULL, WAIT_FOR_BREAK) );
		Sleep(500);
		PC.LogData();
	}
	EXPECT_TRUE( dbg.StopDebugging() );

	//
	// Let MSDEV Settle down before capturing the last sample
	//
	Sleep(10000);
	PC.LogData();
	//
	// Get Xbox and Dev memory usage
	//
	wsprintf(buf, "MSDEV Working Set: %uk (diff = %ldk)", PC.GetCur() / 1024, (PC.GetCur() - workingset) / 1024);
	m_pLog->RecordInfo(buf);
	wsprintf(buf, "MSDEV Working Set: Min = %uk / Max = %uk / Avg = %uk", 
		PC.GetMin() / 1024,
		PC.GetMax() / 1024,
		PC.GetAvg() / 1024
		);
	m_pLog->RecordInfo(buf);


	return (TRUE);
}


BOOL CDbgStressDataBPTest::DataBP( )
{
	long		workingset = 0;
	PerfCounter	PC;
	char		buf[1024];

 	LogTestHeader( "Conditional BP with counting" );
	m_pLog->RecordInfo( "Conditional BP with counting " TIMESTORUNSTR " Times" );
	bps.ClearAllBreakpoints();

	//
	// Get Memory usage on xbox
	//

	Sleep(10000);
	PC.LogData();
	workingset = PC.GetCur();
	wsprintf(buf, "MSDEV Working Set: %uk", workingset / 1024);
	m_pLog->RecordInfo(buf);

	//
	// insert a conditional breakpoint with a count
	//
	EXPECT_TRUE( dbg.Restart() );
	EXPECT_TRUE( dbg.ShowNextStatement() ); // insure we're in source
	EXPECT_VALIDBP(bps.SetBreakOnExpr("global_count", COBP_TYPE_IF_EXP_CHANGED, 1, TIMESTORUN));

	// make sure TIMESTORUN is greater than or equal the times that
	// global_count is changed in the target source (LoopLevel1)
	for (unsigned count = 0; count < TIMESTORUN; count++)
	{
		wsprintf(buf, "Run: %d", count+1);
		m_pLog->RecordInfo(buf);
		
		EXPECT_TRUE( dbg.Go(NULL, NULL, NULL, WAIT_FOR_BREAK) );
		Sleep(500);
		PC.LogData();
	}
	EXPECT_TRUE( dbg.StopDebugging() );

	//
	// Let MSDEV Settle down before capturing the last sample
	//
	Sleep(10000);
	PC.LogData();
	//
	// Get Xbox and Dev memory usage
	//
	wsprintf(buf, "MSDEV Working Set: %uk (diff = %ldk)", PC.GetCur() / 1024, (PC.GetCur() - workingset) / 1024);
	m_pLog->RecordInfo(buf);
	wsprintf(buf, "MSDEV Working Set: Min = %uk / Max = %uk / Avg = %uk", 
		PC.GetMin() / 1024,
		PC.GetMax() / 1024,
		PC.GetAvg() / 1024
		);
	m_pLog->RecordInfo(buf);


	return (TRUE);
}
