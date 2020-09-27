/*
** MEM.CPP
**
**	Created by :			Date :
**		waltcr				2/2/94
**
**	Description :
**		memory tests
*/

#include "stdafx.h"
#include "memcase.h"

/*
** start debugging and do some initialization.  
*/

///////////////////////////////////////////////////////////////////////////////
//	Test Cases
							
/* 
** The memory window address and format is set in StartDebugging.
** Execute to location after value of Global is known.  
** Verify the memory window correlty displays the bytes.
**
*/

BOOL CmemIDETest::MemExpectBytes(   )
{	
	bps.ClearAllBreakpoints();
	dbg.StepInto();
	uimem = UIDebug::ShowDockWindow(IDW_MEMORY_WIN);
	uimem.GoToAddress("Global");
	dbg.Restart();
	bps.SetBreakOnExpr("{foo}END","Global==3",COBP_TYPE_IF_EXP_TRUE);
	EXPECT_TRUE(dbg.Go("END",NULL, "foo"));
	uimem = UIDebug::ShowDockWindow(IDW_MEMORY_WIN);
	uimem.GoToAddress("Global");  
	CString Bytes = uimem.GetCurrentData(4);
	EXPECT_TRUE(Bytes == "03 00 00 00 ");

	// TODO: not fully implemented, always return true for now. 
	return TRUE;
}


#if 0
// TODO
BOOL CMemCases::ReadWriteMemory(   )
{

//	DESC("Read and Write Memory Window Test", "Memory");


	/* 
	** prepare and perform read memory test
	*/

	
	// set memory address ealier in the debug session, now confirm
	// first confirm at correct address

	
	/* 
	** prepare and perform write memory test
	*/

	// now write to the address and verify it takes

}
#endif


BOOL CmemIDETest::MemoryFormats(   )
{
	int m;
	BOOL fOkay = TRUE;

	UIMemory uimem = UIDebug::ShowDockWindow(IDW_MEMORY_WIN);
	for (m=1;m<=14;m++ )
		fOkay &= UIWB.SetMemoryFormat((MEM_FORMAT)m);

	if (fOkay)
		WriteLog(PASSED, "Memory Format Cycle");
	else
		WriteLog(FAILED, "Memory Format Cycle");

	return fOkay;  // REVIEW
}


