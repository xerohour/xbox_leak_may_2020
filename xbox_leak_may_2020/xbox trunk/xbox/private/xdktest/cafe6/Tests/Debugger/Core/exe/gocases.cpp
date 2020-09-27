///////////////////////////////////////////////////////////////////////////////
//	GoCases.cpp
//
//	Created by :			Date :
//		YefimS			11/21/93
//
//	Description :
//		Implementation of Go cases
//

#include <process.h>
#include "stdafx.h"
#include "execase.h"

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

BOOL CexeIDETest::GoAndVerify( )
{
 	LogTestHeader( "GoAndVerify" );
	m_pLog->RecordInfo( " Run an app and verify it ran " );

	EXPECT_TRUE( dbg.Go(NULL, NULL, NULL, WAIT_FOR_TERMINATION) );
	return (TRUE);
}

BOOL CexeIDETest::RestartAndGo( )
{
 	LogTestHeader( "RestartAndGo" );
	m_pLog->RecordInfo( " Restart, verify stop and Go " );

	EXPECT_TRUE( dbg.Restart() );
	if( UIWB.GetPcodeSwitch() )
	{
		EXPECT_TRUE( dbg.CurrentLineIs( "int iGo = TestG();" ) );
	}
	else
	{
		EXPECT_TRUE( dbg.CurrentLineIs( "{" ) );
	}

	EXPECT_TRUE( dbg.Go(NULL, NULL, NULL, WAIT_FOR_TERMINATION) );

	return (TRUE);
}


BOOL CexeIDETest::RestartStopDebugging( )
{
 	LogTestHeader( "RestartStopDebugging" );
	m_pLog->RecordInfo( "Restart, StopDebugging and verify" );

    EXPECT_TRUE( dbg.Restart() );

	if( UIWB.GetPcodeSwitch() )
	{
	//dbg.SetSteppingMode(SRC);  //Temporary workaround: 
		EXPECT_TRUE( dbg.CurrentLineIs( "int iGo = TestG();" ) );
	}
	else
	{
		EXPECT_TRUE( dbg.CurrentLineIs( "{" ) );
	}
	EXPECT_TRUE( dbg.StopDebugging() );
	#if defined( _M_IX86 ) || defined( _MAC )  // WinslowF - for OEM compatibility
		EXPECT_TRUE( VerifyProjectRan(0) );
	#else
		EXPECT_TRUE( VerifyProjectRan(1) );
	#endif

	WriteLog(PASSED, "Stop Debugging was handled as expected" );
	return (TRUE);
}

