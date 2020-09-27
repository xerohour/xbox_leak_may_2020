///////////////////////////////////////////////////////////////////////////////
//	ME1CASES.CPP
//
//	Created by :			Date :
//		YefimS			11/21/93
//
//	Description :
//		Implementation of the CmemIDETest class
//

#include <process.h>
#include "stdafx.h"
#include "memcase.h"

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
///////////////////////////////////////////////////////////////////////////////

BOOL CmemIDETest::GoAfterScrollMemoryWnd( )
{
//	DESC( "Restart, Scroll MemoryWnd and Go", "GoAfterScrollMemoryWnd" );

	BOOL retval = TRUE;
	COSource		src;	// WinslowF - added to call GoToLine. It's not in UIWB any more.

	CODebug codebug;
	EXPECT( codebug.Restart() );

	int i;
	UIMemory umw = UIDebug::ShowDockWindow( IDW_MEMORY_WIN, TRUE );
	EXPECT( umw.IsActive() );

	EXPECT ( umw.GoToAddress("0x16") == ERROR_SUCCESS);

	EXPECT ( umw.GoToAddress("0x7f0000") == ERROR_SUCCESS);
	
	for ( i = 0; i < 200; i++)
	{
		MST.DoKeys ("+{pgdn}");
	}

   if ( !UIWB.VerifySubstringAtLine( "  ?? ??" ) )
	{
		// REVIEW(michma - 3/16/98): VerifySubstringAtLine causes focus to go to source window due to new DoKeyshWnd call.
	    umw = UIDebug::ShowDockWindow( IDW_MEMORY_WIN, TRUE );
		EXPECT ( umw.GoToAddress("0x2000000") == ERROR_SUCCESS);
	}
		
   if ( !UIWB.VerifySubstringAtLine( "  ?? ??" ) )
	{
		// REVIEW(michma - 3/16/98): VerifySubstringAtLine causes focus to go to source window due to new DoKeyshWnd call.
	    umw = UIDebug::ShowDockWindow( IDW_MEMORY_WIN, TRUE );
		EXPECT ( umw.GoToAddress("0xffffffff") == ERROR_SUCCESS);
	}
		
   if ( !UIWB.VerifySubstringAtLine( "  ?? ??" ) )
	{
   	m_pLog->RecordInfo("0x02000000 : The app is too big: test case should be revised" );
		retval = FALSE;
	}

   EXPECT( codebug.Go(NULL,NULL,NULL,WAIT_FOR_TERMINATION) );

   WriteLog(PASSED, "Scrolling Memory Window didn't corrupt Debugging" );

//codebug.StopDebugging();
	return retval;
}


