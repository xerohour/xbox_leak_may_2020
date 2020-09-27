/*
** BREAK.CPP
**
**	Created by :			Date :
**		waltcr				8/14/94
**
**	Description :
**		breakpoint tests
*/


#include "stdafx.h"
#include "bpcase.h"	
#include "bpcase2.h"	

#if defined(_MIPS_) || defined(_ALPHA_)
#define _CHAIN_2 "chain_2"
#define _CHAIN_3 "chain_3"
#else
#define _CHAIN_2 "_chain_2"
#define _CHAIN_3 "_chain_3"
#endif

int UI2COBP( int x );


///////////////////////////////////////////////////////////////////////////////
//	Test Cases

/*
** Some tests lumped together.  Some involve different level of debug
** information or without debug information.  Also a test or two
** involving step/stepout and breakpoints.
** 
*/

BOOL CbpIDETest::BreakStepTrace(   )
{ 	
 	LogTestHeader( "BreakStepTrace" );

	bp *bp;
	BOOL fOkay = TRUE;
	
	bps.ClearAllBreakpoints();
	dbg.StopDebugging();
	bp=bps.SetBreakpoint("WndProc");
	dbg.Go();  								// execute to function WndProc 
	EXPECT_TRUE ( fOkay &= dbg.AtSymbol("WndProc") );	
	dbg.StepOver(); 						// Step past prolog
	bps.ClearAllBreakpoints();
	bp = bps.SetBreakpoint("TagCallChainMain"); 	// set breakpoint on label
	dbg.Go();  								// execute to bp at label
	EXPECT_TRUE ( fOkay &= dbg.AtSymbol("{WndProc}TagCallChainMain") );
	EXPECT_TRUE ( fOkay &= dbg.StepInto(1,"ChainMain") );
	EXPECT_TRUE ( fOkay &= bps.SetBreakpoint("chain_4") != NULL );
	EXPECT_TRUE ( fOkay &= bps.SetBreakpoint("chain_5") != NULL );
	dbg.Go();
	EXPECT_TRUE( fOkay &= dbg.AtSymbol("chain_4"));
	
	// attempt to step out but fire on breakpoint in nested function call
	// proper location is chain_5
	EXPECT_TRUE_COMMENT ( fOkay &= dbg.StepOut(1, NULL, NULL, "chain_5"), "Break in nested function" );

	dbg.StepInto();
	EXPECT_TRUE ( fOkay &= dbg.StepOut(1, NULL, NULL, "chain_4") );
	EXPECT_TRUE ( fOkay &= dbg.StepOut(1, NULL, NULL, "chain_3") );

	dbg.StopDebugging();
	return fOkay;
}									


/* 
** Execute a number of breakpoint tests defined in a table.
**
*/

BOOL CbpIDETest::BPTable(   )
{	
 	LogTestHeader( "BPTable" );

	BOOL fSuccess = TRUE;

	enum UIType	{ 
				UIBP_TYPE_LOC=1,
				UIBP_TYPE_LOC_IF_EXP_TRUE,
				UIBP_TYPE_LOC_IF_EXP_CHANGED,
				UIBP_TYPE_IF_EXP_TRUE,
				UIBP_TYPE_IF_EXP_CHANGED,
				UIBP_TYPE_WNDPROC_IF_MSG_RECVD 
				}; 

	struct CBreak
	{	
		BOOL fStopDbg;
		BOOL fRestart;
		char *szSymbol;
		char *szExpr;
		int Length;
		UIType Type;
		char *szVerifyWithOtherSymbol;
		char *szVerifyWithOtherExpr;
		BOOL fClear;
		//hmm, consider adding a verify current frame
		//hmmh, consider adding an int to execute n times.
		//hmmm, consider adding a pointer to func arg to call before setting
		//hmmm, consider adding a pointer to func to call after hitting bp.
		//hmmm, consider adding a description field
	} rgBreak[] = { // stop, restart, symbol, expr, length, type, verify_symbol, verify_expr, clear
/*pass*/			TRUE, FALSE, "WinMain", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, TRUE,
/*pass*/			TRUE, FALSE, "WndProc", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, TRUE,
/*pass*/			FALSE, FALSE, "chain_1", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, FALSE,
/*pass*/			FALSE, FALSE, _CHAIN_2, NULL, 0, UIBP_TYPE_LOC, NULL, NULL, FALSE,
/*pass*/			FALSE, FALSE, _CHAIN_3, NULL, 0, UIBP_TYPE_LOC, NULL, NULL, FALSE,
/*pass*/			FALSE, FALSE, "chain_4", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, TRUE,
/*pass*/			FALSE, FALSE, "chain_5", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, FALSE,
/*pass*/		 	TRUE, FALSE, "{BPWithLengthTest}BREAK", "gi[0]", 3, UIBP_TYPE_LOC_IF_EXP_CHANGED, NULL, "*(gi+2)==99", FALSE,
/*pass*/		 	FALSE, FALSE, "{foo2}START", "{foo2}paramA>2", 0, UIBP_TYPE_LOC_IF_EXP_TRUE, "{foo2}START", NULL, FALSE,
					// add additional breakpoints above this line
					FALSE, FALSE, NULL, NULL, 0, UIBP_TYPE_LOC, NULL, NULL, FALSE
					};


	CBreak *rgbp = rgBreak;
	bp *ThisBP;
	int iErrCount = 0;

	bps.ClearAllBreakpoints();

	while ((rgbp->szSymbol != NULL) || (rgbp->szExpr != NULL))
	{

		if (rgbp->fStopDbg)
			dbg.StopDebugging();

		switch (rgbp->Type)
		{	
				case UIBP_TYPE_LOC:
					ThisBP = bps.SetBreakpoint(rgbp->szSymbol);
					break;
				case UIBP_TYPE_LOC_IF_EXP_TRUE:
				case UIBP_TYPE_LOC_IF_EXP_CHANGED:
					if (rgbp->Length != 0)
						ThisBP = bps.SetBreakOnExpr(rgbp->szSymbol,	rgbp->szExpr, UI2COBP(rgbp->Type), rgbp->Length);
					else
						ThisBP = bps.SetBreakOnExpr(rgbp->szSymbol,	rgbp->szExpr, UI2COBP(rgbp->Type));
					break;
				case UIBP_TYPE_IF_EXP_TRUE:
				case UIBP_TYPE_IF_EXP_CHANGED:
					if (rgbp->Length != 0)
						ThisBP = bps.SetBreakOnExpr(rgbp->szExpr, UI2COBP(rgbp->Type), rgbp->Length);
					else
						ThisBP = bps.SetBreakOnExpr(rgbp->szExpr, UI2COBP(rgbp->Type)-4);
					break;
				case UIBP_TYPE_WNDPROC_IF_MSG_RECVD:
					ThisBP = bps.SetBreakOnMsg(
						rgbp->szSymbol, 
						rgbp->szExpr);
					break;
		}

		fSuccess = (ThisBP != NULL);
		if (fSuccess)
		{

			if (rgbp->fRestart)
				dbg.Restart();
	
			dbg.Go();

			// verification 
			// REVIEW: TODO: write better pass/fail messsages!


			fSuccess &= (dbg.GetDebugState() != NotDebugging);
			if (fSuccess && (rgbp->szVerifyWithOtherSymbol != NULL))
			{
				fSuccess &= dbg.AtSymbol(rgbp->szVerifyWithOtherSymbol);
				if (!fSuccess)
					m_pLog->RecordFailure("Not at location \'%s\'", rgbp->szVerifyWithOtherSymbol);
			}
			else if (fSuccess && (rgbp->szSymbol != NULL))
			{
				fSuccess &= dbg.AtSymbol(rgbp->szSymbol);
				if (!fSuccess)
					m_pLog->RecordFailure("Not at bp location \'%s\'", rgbp->szSymbol);
			}
			if ( fSuccess && (rgbp->Type != UIBP_TYPE_WNDPROC_IF_MSG_RECVD) && (rgbp->szVerifyWithOtherExpr != NULL) )
			{
				fSuccess &= cxx.ExpressionValueIs(rgbp->szVerifyWithOtherExpr,1);
			}
			else if ( fSuccess && (rgbp->Type != UIBP_TYPE_WNDPROC_IF_MSG_RECVD) && (rgbp->szExpr != NULL)  )
			{
				fSuccess &= cxx.ExpressionValueIs(rgbp->szExpr, 1);
			}

			// cleanup
			if (rgbp->fClear)
				bps.RemoveBreakpoint(ThisBP);
			else
				bps.DisableBreakpoint(ThisBP);
		}

		WriteLog( fSuccess ? PASSED : FAILED, "breakpoint: loc='%s', exp='%s' ... ", rgbp->szSymbol, rgbp->szExpr);
		if (!fSuccess)
			iErrCount++;

		rgbp++;
	}
	
	return (iErrCount==0);
}


BOOL CbpIDETest::MessageBP(   )
{
 	LogTestHeader( "MessageBP" );

	BOOL fSuccess = TRUE;
	int count;

	/* 
	** attempt Breakpoint when message received.
	*/

	bps.ClearAllBreakpoints();

	EXPECT_TRUE( fSuccess &= (bps.SetBreakOnMsg("WndProc","WM_CREATE") != NULL) );

	dbg.Restart();
	dbg.Go();
	EXPECT_TRUE(fSuccess &= dbg.AtSymbol("WndProc"));

	// try to make sure we broke on the correct message
	count=0;
	int countNumber = 25;
	do 
	{
		count++;
		dbg.StepOver();
	} while( (count < countNumber) && (!dbg.AtSymbol("TagCreateCase")));

	EXPECT_TRUE_COMMENT ( (fSuccess &= dbg.AtSymbol("TagCreateCase")),  "Break on WM_CREATE message")

	return fSuccess;
}

BOOL CbpIDETest::LocationBPWithExp(   )
{	
 	LogTestHeader( "LocationBPWithExp" );

	BOOL fSuccess = TRUE;

	bps.ClearAllBreakpoints();
	dbg.Restart();
	if (bps.SetBreakOnExpr(
		"{foo}END",
		"Global==3",
		COBP_TYPE_IF_EXP_TRUE) == FALSE)
	{
		fSuccess = FALSE;
		m_pLog->RecordFailure("Unable to set location breakpoint w/expression");
	}
	else if (dbg.Go("END",NULL, "foo") == FALSE)
	{
		fSuccess = FALSE;
		m_pLog->RecordFailure("Not at expected location");
	}

	return fSuccess;
}

int UI2COBP(int x)
{
	switch(x)
	{
		case UIBP_TYPE_LOC_IF_EXP_TRUE:
		case UIBP_TYPE_LOC_IF_EXP_CHANGED:
			return x-2;
			
		case UIBP_TYPE_IF_EXP_TRUE:
		case UIBP_TYPE_IF_EXP_CHANGED:
			return x-4;
	}
	return 0;
}


/*
** Break at DllMain() which is executed early i.e. at DLL_PROCESS_ATTACH
**
*/

BOOL Cbp2IDETest::BreakDLLInit(   )
{
 	LogTestHeader( "BreakDLLInit" );

	BOOL bOkay = TRUE;

	dbg.StopDebugging();
	bps.ClearAllBreakpoints();

	EXPECT_TRUE ( bOkay &= (bps.SetBreakpoint("{,,dec2.dll}DllEntryPoint") != NULL) );
	if (bOkay)
	{
		EXPECT_TRUE ( bOkay &= ( dbg.Go() && dbg.VerifyAtSymbol("{,,dec2.dll}DllEntryPoint") ) );
		bps.ClearAllBreakpoints();
	}

#if 0
	// at restart would be semi-interesting too
	FALSE, TRUE, "{,,dec2.dll}DllEntryPoint", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, TRUE,  // hit bp during restart
	TRUE, FALSE, "{,,incdll.dll}DllMain", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, FALSE,
	// how about a virtual dll?
#endif 

	return bOkay;	// WriteLog( bOkay, GetTestDesc() );
}


BOOL Cbp2IDETest::LoadDLLBreak(   )
{
 	LogTestHeader( "LoadDLLBreak" );
	BOOL bOkay = TRUE;
//	bp *ThisBP;

	bps.ClearAllBreakpoints();
	dbg.StopDebugging();
	dbg.StepInto(4);

	EXPECT_TRUE ( bOkay &= (prj.SetAdditionalDLLs("powdll.dll") != ERROR_SUCCESS) );
	if( bOkay )
		dbg.StepInto(4);
/*	if (prj.SetAdditionalDLLs("powdll.dll") != ERROR_SUCCESS)
	{
		m_pLog->RecordFailure("unable to set additional dlls");
		bOkay = FALSE;
	}
	else
	{
	// REVIEW (michma): cannot currently load dll symbols while debugging (menu limitation).
	// however, since prj.SetAdditionalDLLs isn't working yet either, we need
	// to start debugging to load dll symbols to get this test to finish.
	 dbg.StepInto(4);
	}
*/
	EXPECT_TRUE ( bOkay &= (prj.SetAdditionalDLLs("powdll.dll") != ERROR_SUCCESS) );
	if( bOkay )
	{
		EXPECT_TRUE ( bOkay &= (bps.SetBreakpoint("dPow") != NULL) );
		if( bOkay )
		{
			EXPECT_TRUE ( bOkay &= (dbg.Go() && dbg.VerifyAtSymbol("dPow")) );
			bps.ClearAllBreakpoints();
		}
	}
/*	if (prj.SetAdditionalDLLs("powdll.dll") != ERROR_SUCCESS)
	{
		m_pLog->RecordFailure("unable to set additional dlls");
		bOkay = FALSE;
	}
	else
	{
		ThisBP = bps.SetBreakpoint("dPow");
		bOkay &= (ThisBP != NULL);
		if (ThisBP == NULL)
		{
			m_pLog->RecordFailure("unable to set breakpoint after loading symbols");
		}
		else
		{
			dbg.Go();
			if (dbg.VerifyAtSymbol("dPow") == FALSE)
			{
				bOkay &= FALSE;
				m_pLog->RecordFailure("Not at symbol \'%s\'", "dPow" );
			}
			bps.RemoveBreakpoint(ThisBP);
		}
	}
*/
	return bOkay;
}


BOOL Cbp2IDETest::BPTableDLL(   )
{
 	LogTestHeader( "BPTableDLL" );
	BOOL fSuccess = TRUE;

	enum UIType	{ 
				UIBP_TYPE_LOC=1,
				UIBP_TYPE_LOC_IF_EXP_TRUE,
				UIBP_TYPE_LOC_IF_EXP_CHANGED,
				UIBP_TYPE_IF_EXP_TRUE,
				UIBP_TYPE_IF_EXP_CHANGED,
				UIBP_TYPE_WNDPROC_IF_MSG_RECVD 
				}; 

	struct CBreak
	{	
		BOOL fStopDbg;
		BOOL fRestart;
		char *szSymbol;
		char *szExpr;
		int Length;
		UIType Type;
		char *szVerifyWithOtherSymbol;
		char *szVerifyWithOtherExpr;
		BOOL fClear;
		//hmm, consider adding a verify current frame!
		//hmmh, consider adding an int to execute n times.
		//hmmm, consider adding a pointer to func arg to call before setting
		//hmmm, consider adding a pointer to func to call after hitting bp.
		//hmmm, consider adding a description field
	} rgBreak[] = 	{ 
					// stop, restart, symbol, expr, length, type, verify_symbol, verify-expr, clear
/* pass */			TRUE, FALSE, "WinMain", NULL, 0, UIBP_TYPE_LOC, NULL, NULL,  FALSE, 
/* pass */			FALSE, TRUE, "MainWndProc", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, TRUE,
/* pass */			FALSE, FALSE, "MainWndProc", "WM_CREATE", 0, UIBP_TYPE_WNDPROC_IF_MSG_RECVD,  NULL, NULL, FALSE,
/* pass */			TRUE, FALSE, "Dec", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, FALSE, 
/* pass */			FALSE, FALSE, "Inc", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, TRUE,
//					FALSE, TRUE, NULL, "{,,dllappx.exe}wValue", 0, UIBP_TYPE_IF_EXP_CHANGED, NULL, NULL, FALSE,  // TODO: need to be able to check between expressions
/* pass */			TRUE, TRUE, "InitApplication", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, FALSE, 
#if !defined(_ALPHA_) && !defined(_MIPS_)
/* pass bug */	//or#14945 is postponed:		FALSE, FALSE, NULL, "gi[0]", 3, UIBP_TYPE_IF_EXP_CHANGED, "{BPWithLengthTest}BREAK", "*(gi+2)==99", FALSE,
#endif
/* pass */			TRUE, TRUE, "InitInstance", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, TRUE,
/* pass */			FALSE, TRUE, "{MainWndProc,,dllappx.exe}TagEndPaint", "{,,dllappx.exe}hGlobalMem!=0", 0, UIBP_TYPE_LOC_IF_EXP_TRUE, NULL, NULL, FALSE,
/* part a */		TRUE, FALSE, "{Dec,dec2.c,dec2.dll}TagAfterGetProcAddr", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, FALSE,
/* part b */		FALSE, FALSE, "dPow", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, FALSE,
#if !defined(_MIPS_) && !defined(_ALPHA_)
/* pass a,b,c*/	//or#18749:
		// TODO(michma - 1/30/98) - this bp takes longer than 5 minutes to hit, but it hasn't before.
		//TRUE, FALSE, NULL, "{Dec,,dec2.dll}dValue==1000", 0, UIBP_TYPE_IF_EXP_TRUE, "TagAfterPow", "dValue==1000", FALSE,
#endif
/* pass */			TRUE, FALSE, "{,incdll.c,incdll.dll}DllMain", NULL, 0, UIBP_TYPE_LOC, NULL, NULL, FALSE,
//					TRUE, FALSE, NULL, "{DllEntryPoint,,dec2.dll}ulRbc==0", 0, UIBP_TYPE_IF_EXP_TRUE, NULL, "ulRbc==0", FALSE,
					// add additional breakpoints above this line
					FALSE, FALSE, NULL, NULL, 0, UIBP_TYPE_LOC, NULL, NULL, FALSE
					};


	CBreak *rgbp = rgBreak;
	bp *ThisBP;
	int iErrCount = 0;


	bps.ClearAllBreakpoints();

	/* 
	** for portability the following bp tests had to be isolated.
	** attempt Breakpoint when expression true.
	** attempt Breakpoint when expression changed.
	** attempt Breakpoint on global expression.
	** attempt Breakpoint on local expression.
	*/

	while ((rgbp->szSymbol != NULL) || (rgbp->szExpr != NULL))
	{

		if (rgbp->fStopDbg)
			dbg.StopDebugging();

		switch (rgbp->Type)
		{	
				case UIBP_TYPE_LOC:
					ThisBP = bps.SetBreakpoint(rgbp->szSymbol);
					break;
				case UIBP_TYPE_LOC_IF_EXP_TRUE:
				case UIBP_TYPE_LOC_IF_EXP_CHANGED:
					if (rgbp->Length != 0)
						ThisBP = bps.SetBreakOnExpr(rgbp->szSymbol,	rgbp->szExpr, UI2COBP(rgbp->Type), rgbp->Length);
					else
						ThisBP = bps.SetBreakOnExpr(rgbp->szSymbol,	rgbp->szExpr, UI2COBP(rgbp->Type));
					break;
				case UIBP_TYPE_IF_EXP_TRUE:
				case UIBP_TYPE_IF_EXP_CHANGED:
					if (rgbp->Length != 0)
						ThisBP = bps.SetBreakOnExpr(rgbp->szExpr, UI2COBP(rgbp->Type), rgbp->Length);
					else
						ThisBP = bps.SetBreakOnExpr(rgbp->szExpr, UI2COBP(rgbp->Type));
					break;
				case UIBP_TYPE_WNDPROC_IF_MSG_RECVD:
					ThisBP = bps.SetBreakOnMsg(
						rgbp->szSymbol, 
						rgbp->szExpr);
					break;
		}

		
		fSuccess = (ThisBP != NULL);
		if (fSuccess)
		{
			if (rgbp->fRestart)
				dbg.Restart();
	
			dbg.Go();

			// verification 
			fSuccess &= (dbg.GetDebugState() != NotDebugging);
			if (fSuccess && (rgbp->szVerifyWithOtherSymbol != NULL))
			{
				fSuccess &= dbg.AtSymbol(rgbp->szVerifyWithOtherSymbol);
				if (!fSuccess)
					m_pLog->RecordFailure("Not at symbol \'%s\'", rgbp->szVerifyWithOtherSymbol);
			}
			else if (fSuccess && (rgbp->szSymbol != NULL))
			{
				fSuccess &= dbg.AtSymbol(rgbp->szSymbol);
				if (!fSuccess)
					m_pLog->RecordFailure("Not at bp symbol \'%s\'", rgbp->szSymbol);
			}
			if ( fSuccess && (rgbp->Type != UIBP_TYPE_WNDPROC_IF_MSG_RECVD) && (rgbp->szVerifyWithOtherExpr != NULL) )
			{
				fSuccess &= cxx.ExpressionValueIs(rgbp->szVerifyWithOtherExpr,1);
			}
			else if ( fSuccess && (rgbp->Type != UIBP_TYPE_WNDPROC_IF_MSG_RECVD) && (rgbp->szExpr != NULL) )
			{
				fSuccess &= cxx.ExpressionValueIs(rgbp->szExpr, 1);
			}

			// cleanup
			if (rgbp->fClear)
				bps.RemoveBreakpoint(ThisBP);
			else
				bps.DisableBreakpoint(ThisBP);
		}
		WriteLog( fSuccess ? PASSED : FAILED, "breakpoint: loc='%s', exp='%s' ... ", rgbp->szSymbol, rgbp->szExpr);
		if (!fSuccess)
			iErrCount++;

		rgbp++;
	}
	
	
	return (iErrCount==0);	// WriteLog( iErrCount==0, GetTestDesc() );
}


// TODO?

/* 
** attempt to step over function but which fires on a message bp.
*/


/* 
** test Breakpoint at line location.
*/


/* 
** test Breakpoint at func location in primary module.
*/


/* 
** attempt Breakpoint at location when expression changed.
*/


/* 
** test Breakpoint on expression involving static.
*/


/* 
** test Breakpoint on expression involving both local and global.
*/


/* 
** test Breakpoint when expression true with length.
*/


/* 
** attempt Breakpoint when class of message received.
*/


#if 0
/*
** while executing child, set breakpoint which will fire.
*/
	dbg.Go(NOWAIT);
	bps.SetBreakpoint(some location );
#endif


#if 0


/*
** two step_out's, first hitting bp, second should return
*/
	bps.ClearAllBreakpoints();
	dbg.Restart();
	bps.SetBreakOnExpr("{MyPostMessage,,}TagPost","count>1",COBP_TYPE_IF_EXP_TRUE);
	dbg.Go();
	EXPECT_TRUE( dbg.AtSymbol("TagPost"));
#endif

	
/* 
** attempt Breakpoint at func location in secondary module.
*/


/* 
** attempt to step over function but which fires on a message bp.
*/
