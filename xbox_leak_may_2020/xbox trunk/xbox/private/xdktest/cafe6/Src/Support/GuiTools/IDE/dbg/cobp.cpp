///////////////////////////////////////////////////////////////////////////////
//  COBP.CPP
//
//  Created by :            Date :
//      MichMa                  1/13/94
//
//  Description :
//      Implementation of the COBreakpoints class
//
		   
#include "stdafx.h"
#include "cobp.h"
#include "codebug.h"
#include "..\shl\uwbframe.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: bp* COBreakpoints::SetBreakpoint(int line, LPCSTR source, int passcount /* 0 */)
// Description: Set a breakpoint on the given line in the specified source file with the given pass count.
// Return: A pointer to a bp object that is used as a handle to the breakpoint. Store this pointer to reference the breakpoint later.
// Param: line An integer that contains the line to set the breakpoint on.
// Param: source A pointer to a string that contains the name of the source file to set the breakpoint in. NULL means use the current file.
// Param: passcount An integer that contains the number of times to pass this breakpoint before breaking. (Default value is 0.)
// END_HELP_COMMENT
bp *COBreakpoints::SetBreakpoint(int line, LPCSTR source, int passcount /* 0 */)
	{
	char string[12];
	CString cstr;
	if (source)
		cstr = cstr + "{," + source + ",}";
	cstr = cstr + "." + _itoa(line, string, 10);
	return SetBreakpoint(cstr, passcount);
	}


// BEGIN_HELP_COMMENT
// Function: bp* COBreakpoints::SetBreakpoint(LPCSTR function, LPCSTR source, int passcount /* 0 */)
// Description: Set a breakpoint on the given function in the specified source file with the given pass count.
// Return: A pointer to a bp object that is used as a handle to the breakpoint. Store this pointer to reference the breakpoint later.
// Param: function A pointer to a string that contains the name of the function to set the breakpoint on.
// Param: source A pointer to a string that contains the name of the source file to set the breakpoint in. NULL means use the current file.
// Param: passcount An integer that contains the number of times to pass this breakpoint before breaking. (Default value is 0.)
// END_HELP_COMMENT
bp *COBreakpoints::SetBreakpoint(LPCSTR function, LPCSTR source, int passcount /* 0 */)
	{
	CString cstr;
	if (source)
		cstr = cstr + "{," + source + ",}";
	cstr = cstr + function;
	return SetBreakpoint(cstr, passcount);
	}


// BEGIN_HELP_COMMENT
// Function: bp* COBreakpoints::SetBreakpoint(int line, LPCSTR expression, int option, int length, int passcount /* 0 */)
// Description: Set an expression breakpoint on the given line with the given pass count.
// Return: A pointer to a bp object that is used as a handle to the breakpoint. Store this pointer to reference the breakpoint later.
// Param: line An integer that contains the line to set the breakpoint on.
// Param: expression A pointer to a string that contains the expression to use in the breakpoint. Expression is specified by the following form: {function name, source filename, EXE filename} expression.
// Param: option A value that specifies the type of expression breakpoint to set: COBP_TYPE_IF_EXP_TRUE, COBP_TYPE_IF_EXP_CHANGED.
// Param: length An integer that contains the number of elements to watch.
// Param: passcount An integer that contains the number of times to pass this breakpoint before breaking. (Default value is 0.)
// END_HELP_COMMENT
bp *COBreakpoints::SetBreakpoint(int line, LPCSTR expression, int option, int length, int passcount /* 0 */)
	{
	char string[12];
	CString cstr;
	cstr = cstr + "." + _itoa(line, string, 10);
	return SetBreakOnExpr(cstr, expression, option, length, passcount);
	}


// BEGIN_HELP_COMMENT
// Function: bp* COBreakpoints::SetBreakpoint(int line, LPCSTR source, LPCSTR expression, int option, int length, int passcount /* 0 */)
// Description: Set an expression breakpoint on the given line in the specified source file with the given pass count.
// Return: A pointer to a bp object that is used as a handle to the breakpoint. Store this pointer to reference the breakpoint later.
// Param: line An integer that contains the line to set the breakpoint on.
// Param: source A pointer to a string that contains the name of the source file to set the breakpoint in. NULL means use the current file.
// Param: expression A pointer to a string that contains the expression to use in the breakpoint. Expression is specified by the following form: {function name, source filename, EXE filename} expression.
// Param: option A value that specifies the type of expression breakpoint to set: COBP_TYPE_IF_EXP_TRUE, COBP_TYPE_IF_EXP_CHANGED.
// Param: length An integer that contains the number of elements to watch.
// Param: passcount An integer that contains the number of times to pass this breakpoint before breaking. (Default value is 0.)
// END_HELP_COMMENT
bp *COBreakpoints::SetBreakpoint(int line, LPCSTR source, LPCSTR expression, int option, int length, int passcount /* 0 */)
	{
	char string[12];								
	CString cstr;
	if (source)
		cstr = cstr + "{," + source + ",}";
	cstr = cstr + _itoa(line, string, 10);
	return SetBreakOnExpr(cstr, expression, option, length, passcount);
	}


// BEGIN_HELP_COMMENT
// Function: bp* COBreakpoints::SetBreakpoint(LPCSTR function, LPCSTR expression, int option, int length, int passcount /* 0 */)
// Description: Set an expression breakpoint on the given function with the given pass count.
// Return: A pointer to a bp object that is used as a handle to the breakpoint. Store this pointer to reference the breakpoint later.
// Param: function A pointer to a string that contains the name of the function to set the breakpoint on.
// Param: expression A pointer to a string that contains the expression to use in the breakpoint. Expression is specified by the following form: {function name, source filename, EXE filename} expression.
// Param: option A value that specifies the type of expression breakpoint to set: COBP_TYPE_IF_EXP_TRUE, COBP_TYPE_IF_EXP_CHANGED.
// Param: length An integer that contains the number of elements to watch.
// Param: passcount An integer that contains the number of times to pass this breakpoint before breaking. (Default value is 0.)
// END_HELP_COMMENT
bp *COBreakpoints::SetBreakpoint(LPCSTR function, LPCSTR expression, int option, int length, int passcount /* 0 */)
	{
	return SetBreakOnExpr(function, expression, option, length, passcount);
	}


// BEGIN_HELP_COMMENT
// Function: bp* COBreakpoints::SetBreakpoint(LPCSTR function, LPCSTR source, LPCSTR expression, int option, int length, int passcount /* 0 */)
// Description: Set an expression breakpoint on the given function in the specified source file with the given pass count.
// Return: A pointer to a bp object that is used as a handle to the breakpoint. Store this pointer to reference the breakpoint later.
// Param: function A pointer to a string that contains the name of the function to set the breakpoint on.
// Param: source A pointer to a string that contains the name of the source file to set the breakpoint in. NULL means use the current file.
// Param: expression A pointer to a string that contains the expression to use in the breakpoint. Expression is specified by the following form: {function name, source filename, EXE filename} expression.
// Param: option A value that specifies the type of expression breakpoint to set: COBP_TYPE_IF_EXP_TRUE, COBP_TYPE_IF_EXP_CHANGED.
// Param: length An integer that contains the number of elements to watch.
// Param: passcount An integer that contains the number of times to pass this breakpoint before breaking. (Default value is 0.)
// END_HELP_COMMENT
bp *COBreakpoints::SetBreakpoint(LPCSTR function, LPCSTR source, LPCSTR expression, int option, int length, int passcount /* 0 */)
	{
	CString cstr;
	if (source)
		cstr = cstr + "{," + source + ",}";
	cstr = cstr + function;
	return SetBreakOnExpr(cstr, expression, option, length, passcount);
	}


// BEGIN_HELP_COMMENT
// Function: bp* COBreakpoints::SetBreakpoint(LPCSTR expression, int option, int length, int passcount /* 0 */)
// Description: Set an expression breakpoint with the given pass count.
// Return: A pointer to a bp object that is used as a handle to the breakpoint. Store this pointer to reference the breakpoint later.
// Param: expression A pointer to a string that contains the expression to use in the breakpoint. Expression is specified by the following form: {function name, source filename, EXE filename} expression.
// Param: option A value that specifies the type of expression breakpoint to set: COBP_TYPE_IF_EXP_TRUE, COBP_TYPE_IF_EXP_CHANGED.
// Param: length An integer that contains the number of elements to watch.
// Param: passcount An integer that contains the number of times to pass this breakpoint before breaking. (Default value is 0.)
// END_HELP_COMMENT
bp *COBreakpoints::SetBreakpoint(LPCSTR expression, int option, int length, int passcount /* 0 */)
	{
	return SetBreakOnExpr(expression, option, length, passcount);
	}

// BEGIN_HELP_COMMENT
// Function: bp* COBreakpoints::SetBreakpoint(LPCSTR location /* NULL */, int passcount /* 0 */)
// Description: Set breakpoint at the given location with the given pass count.
// Return: A pointer to a bp object that is used as a handle to the breakpoint. Store this pointer to reference the breakpoint later.
// Param: location A pointer to a string that specifies the location of the breakpoint. Location is specified by the following form: {function name, source file name, EXE filename} .line. NULL means set the breakpoint at the current source location. (Default value is NULL.)
// Param: passcount An integer that contains the number of times to pass this breakpoint before breaking. (Default value is 0.)
// END_HELP_COMMENT
bp *COBreakpoints::SetBreakpoint(LPCSTR location /* NULL */, int passcount /* 0 */)
	{
	char string[12];
	CString cstr;
//	int line;

	if (!location)										 // F9 if line=0
	{
		if (!UIWB.ToggleBreakpoint()) return NULL;  // REVIEW what if we toggle off
		return (AddBPToList());
	}
//	below doesn't work msvc won't let it add the bp!
//	if (!location)                                       //use current line if line=0
//		line = UIWB.GetEditorCurPos(GECP_LINE);
//		cstr = "." + _itoa(line, string, 10);

// REVIEW(briancr): this is obsolete code
//	if(!(uibp = UIWB.Breakpoints())) return NULL;			
	if (!uibp.Activate()) {
		return NULL;
	}

//  below doesn't work default location somehow cleared!!!!
//	if (!locaion)                                       //use defaults if line=0
//		return AddBPandClose();
	cstr = location;
//	if(!uibp.SetType(UIBP_TYPE_LOC)) return FailAndClose();
	if(!uibp.SetLocation(cstr)) return FailAndClose();
	if(passcount) {	
		if(!uibp.SetPassCount(_itoa(passcount, string, 10))) return FailAndClose();
	}
	return AddBPandClose();
	}


// BEGIN_HELP_COMMENT
// Function: bp* COBreakpoints::SetAmbiguousBreakpoint(LPCSTR location, int ambiguity)
// Description: Set breakpoint at the given ambiguous location, resolving the ambiguity as specified.
// Return: A pointer to a bp object that is used as a handle to the breakpoint. Store this pointer to reference the breakpoint later.
// Param: location A pointer to a string that specifies the location of the breakpoint (ex. a function, or a context string).
// Param: ambiguity An integer that specifies which function to select from the ambiguity list
// END_HELP_COMMENT
bp *COBreakpoints::SetAmbiguousBreakpoint(LPCSTR location, int ambiguity)

{
	if (!uibp.Activate())
		return NULL;

	uibp.SetLocation(location); 
	MST.WButtonClick(GetLabel(UIBP_OK_BTN));

	// wait for the ra dlg to be displayed
	if(!MST.WFndWndWait(GetLocString(IDSS_RESOLVE_AMBIGUITY_TITLE), FW_DEFAULT, 5))
	{
		LOG->RecordInfo("ERROR: Resolve Ambiguity dlg not found");
		return FailAndClose();
	}

	// make sure the dlg has enough entries in it.
	if(MST.WListCount("") < ambiguity)
	{
		LOG->RecordInfo("ERROR: Number of entries in RA dlg is less than %d", ambiguity);
		return FailAndClose();
	}
	
	// choose the source line entry from the list box in the rsla dlg
	for(int i = 1; i < ambiguity; i++)
		MST.DoKeys("{DOWN}");
	
	// ok the dlg
	MST.DoKeys("{ENTER}");
	
	// wait for the ra dlg to be displayed
	if(!MST.WFndWndWait(GetLocString(IDSS_RESOLVE_AMBIGUITY_TITLE), FW_DEFAULT | FW_NOEXIST, 5))
	{
		LOG->RecordInfo("ERROR: Resolve Ambiguity dlg still active");
		return FailAndClose();
	}

	return AddBPToList();
}


// BEGIN_HELP_COMMENT
// Function: bp* COBreakpoints::SetBreakOnExpr(LPCSTR location, LPCSTR expression, int expr_type, int length /* 1 */, int passcount /* 0 */)
// Description: Set an expression breakpoint at the given location with the given pass count.
// Return: A pointer to a bp object that is used as a handle to the breakpoint. Store this pointer to reference the breakpoint later.
// Param: location A pointer to a string that specifies the location of the breakpoint. Location is specified by the following form: {function name, source filename, EXE filename} .line.
// Param: expression A pointer to a string that contains the expression to use in the breakpoint. Expression is specified by the following form: {function name, source filename, EXE filename} expression.
// Param: expr_type A value that specifies the type of expression breakpoint to set: COBP_TYPE_IF_EXP_TRUE, COBP_TYPE_IF_EXP_CHANGED.
// Param: length An integer that contains the number of elements to watch. (Default value is 1.)
// Param: passcount An integer that contains the number of times to pass this breakpoint before breaking. (Default value is 0.)
// END_HELP_COMMENT
bp *COBreakpoints::SetBreakOnExpr(LPCSTR location, LPCSTR expression, int expr_type, int length /* 1 */, int passcount /* 0 */)
	{
	char string[12];						   	
// REVIEW(briancr): this is obsolete code
//	if(!(uibp = UIWB.Breakpoints())) return NULL;
	if (!uibp.Activate()) {
		return NULL;
	}

	if(!uibp.SetLocation(location)) return FailAndClose();	
	if(passcount) {	
		if(!uibp.SetPassCount(_itoa(passcount, string, 10))) return FailAndClose();
	}
	switch(expr_type)
		{
		case COBP_TYPE_IF_EXP_TRUE:
			{
			if(!uibp.SetLocationExpression(expression, UIBP_TYPE_IF_EXP_TRUE)) return FailAndClose();
			break;
			}
		case COBP_TYPE_IF_EXP_CHANGED:
			{
			if(!uibp.SetLocationExpression(expression, UIBP_TYPE_IF_EXP_CHANGED)) return FailAndClose();
			if(!uibp.SetLocationExprLength(_itoa(length, string, 10))) return FailAndClose();
			}
		}

	return AddBPandClose();
	}
	
	
// BEGIN_HELP_COMMENT
// Function: bp* COBreakpoints::SetBreakOnMsg(LPCSTR location, LPCSTR message)
// Description: Set a message breakpoint at the given location with the given pass count.
// Return: A pointer to a bp object that is used as a handle to the breakpoint. Store this pointer to reference the breakpoint later.
// Param: location A pointer to a string that specifies the location of the breakpoint. Location is specified by the following form: {function name, source filename, EXE filename} .line.
// Param: message A pointer to a string that contains the name of the message to break on.
// END_HELP_COMMENT
bp *COBreakpoints::SetBreakOnMsg(LPCSTR location, LPCSTR message)
	{
//	char string[12];						   	
// REVIEW(briancr): this is obsolete code
//	if(!(uibp = UIWB.Breakpoints())) return NULL;
	if (!uibp.Activate()) {
		return NULL;
	}
///	if(!uibp.SetType(UIBP_TYPE_WNDPROC_IF_MSG_RECVD)) return FailAndClose();
	if(!uibp.SetWndProc(location)) return FailAndClose();	
	if(!uibp.SetMessage(message, 0)) return FailAndClose();
//	if(!uibp.SetPassCount(_itoa(passcount, string, 10))) return FailAndClose();
	return AddBPandClose();
	}
	

// BEGIN_HELP_COMMENT
// Function: bp* COBreakpoints::SetBreakOnExpr(LPCSTR expression, int expr_type, int length /* 1 */, int passcount /* 0 */)
// Description: Set an expression breakpoint with the given pass count.
// Return: A pointer to a bp object that is used as a handle to the breakpoint. Store this pointer to reference the breakpoint later.
// Param: expression A pointer to a string that contains the expression to use in the breakpoint. Expression is specified by the following form: {function name, source filename, EXE filename} expression.
// Param: expr_type A value that specifies the type of expression breakpoint to set: COBP_TYPE_IF_EXP_TRUE, COBP_TYPE_IF_EXP_CHANGED.
// Param: length An integer that contains the number of elements to watch. (Default value is 1.)
// Param: passcount An integer that contains the number of times to pass this breakpoint before breaking. (Default value is 0.)
// END_HELP_COMMENT
bp *COBreakpoints::SetBreakOnExpr(LPCSTR expression, int expr_type, int length /* 1 */, int passcount /* 0 */)
	{
	char string[12];
// REVIEW(briancr): this is obsolete code
//	if(!(uibp = UIWB.Breakpoints())) return NULL;
	if (!uibp.Activate()) {
		return NULL;
	}

	switch(expr_type)
		{
		case COBP_TYPE_IF_EXP_TRUE:
			{
			if(!uibp.SetDataExpression(expression, UIBP_TYPE_IF_EXP_TRUE)) return FailAndClose();
			break;
			}
		case COBP_TYPE_IF_EXP_CHANGED:
			{
			if(!uibp.SetDataExpression(expression, UIBP_TYPE_IF_EXP_CHANGED)) return FailAndClose();
			if(!uibp.SetExprLength(_itoa(length, string, 10))) return FailAndClose();
			}
		}

	return AddBPandClose();
	}



// BEGIN_HELP_COMMENT
// Function: BOOL COBreakpoints::EnableBreakpoint(bp* pbp)
// Description: Enable the breakpoint specified by the bp object given.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: pbp A pointer to a bp object that specifies the breakpoint to enable.
// END_HELP_COMMENT
BOOL COBreakpoints::EnableBreakpoint(bp *pbp)
	{
	BOOL success=TRUE;
	int bpindex = FindBPIndex(bp_list.Find(pbp));
	ASSERT(bpindex != -1);
// REVIEW(briancr): this is obsolete code
//	if(!(uibp = UIWB.Breakpoints())) return FALSE;
	if (!uibp.Activate()) {
		return FALSE;
	}
	success = uibp.Enable(bpindex);//can't return here without closing dialog!
	success = (uibp.Close() == NULL) && success;
	return (success);  		
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COBreakpoints::EnableAllBreakpoints(void)
// Description: Enable all breakpoints.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COBreakpoints::EnableAllBreakpoints(void)
	{
	BOOL success=TRUE;
// REVIEW(briancr): this is obsolete code
//	if(!(uibp = UIWB.Breakpoints())) return FALSE;
	if (!uibp.Activate()) {
		return FALSE;
	}
	
	int index=0;
	for (POSITION bp_pos = bp_list.GetHeadPosition(); ((bp_pos != NULL) && (success));)
	{
		bp_list.GetNext(bp_pos);
		success=uibp.Enable(index++);//can't return here without closing dialog!
	}
	if (bp_pos!=NULL) return success=FALSE;	
	
	success = (uibp.Close() == NULL) && success;
	return (success);  		
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COBreakpoints::DisableBreakpoint(bp* pbp)
// Description: Disable the breakpoint specified by the bp object given.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: pbp A pointer to a bp object that specifies the breakpoint to disable.
// END_HELP_COMMENT
BOOL COBreakpoints::DisableBreakpoint(bp *pbp)
	{
	BOOL success=TRUE;
	int bpindex = FindBPIndex(bp_list.Find(pbp));
	ASSERT(bpindex != -1);
// REVIEW(briancr): this is obsolete code
//	if(!(uibp = UIWB.Breakpoints())) return FALSE;
	if (!uibp.Activate()) {
		return FALSE;
	}
	success = uibp.Disable(bpindex);//can't return here without closing dialog!
	success = (uibp.Close() == NULL) && success;
	return (success);  		
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COBreakpoints::DisableAllBreakpoints(void)
// Description: Disable all breakpoints.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COBreakpoints::DisableAllBreakpoints(void)
	{
	BOOL success=TRUE;
// REVIEW(briancr): this is obsolete code
//	if(!(uibp = UIWB.Breakpoints())) return FALSE;
	if (!uibp.Activate()) {
		return FALSE;
	}
	
//	int index=1;
	int index=0;
	for (POSITION bp_pos= bp_list.GetHeadPosition(); (bp_pos != NULL) && (success);)
	{
		bp_list.GetNext(bp_pos);
		success=uibp.Disable(index++);//can't return here without closing dialog!
	}
	if (bp_pos!=NULL) return success=FALSE;	
	
	success = (uibp.Close() == NULL) && success;
	return (success);  		
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COBreakpoints::RemoveBreakpoint(bp* pbp)
// Description: Remove the breakpoint specified by the bp object given.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: pbp A pointer to a bp object that specifies the breakpoint to remove.
// END_HELP_COMMENT
BOOL COBreakpoints::RemoveBreakpoint(bp *pbp)
	{
	BOOL success=TRUE;
	int bpindex = FindBPIndex(bp_list.Find(pbp));
	ASSERT(bpindex != -1);
// REVIEW(briancr): this is obsolete code
//	if(!(uibp = UIWB.Breakpoints())) return FALSE;
	if (!uibp.Activate()) {
		return FALSE;
	}
	success = uibp.Remove(bpindex);//can't return here without closing dialog!
	success = (uibp.Close() == NULL) && success;
	if(!success)  return FALSE;
	
	delete bp_list.GetAt(bp_list.Find(pbp));
	bp_list.RemoveAt(bp_list.Find(pbp));
	
	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COBreakpoints::ClearAllBreakpoints(void)
// Description: Remove all breakpoints.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COBreakpoints::ClearAllBreakpoints(void)
	{
	BOOL success=TRUE;
// REVIEW(briancr): this is obsolete code
//	if(!(uibp = UIWB.Breakpoints())) return FALSE;
	if (!uibp.Activate()) {
		return FALSE;
	}
	success = uibp.ClearAll();	//can't return here without closing dialog!
	success = (uibp.Close() == NULL) && success;
	if(!success)  return FALSE;  		

	while(! bp_list.IsEmpty())
		delete bp_list.RemoveTail();

	return TRUE;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL VerifyBreakpointHit(void)
// Description: Verify that a breakpoint was hit. This function is NYI.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL VerifyBreakpointHit(void)
	{
	//todo chech for bp dialog/message
	return TRUE;
	}

bp *COBreakpoints::FailAndClose(void) //close dialog then fail Set Bp operation
	{
	uibp.Close();
	return NULL;
	};

bp *COBreakpoints::AddBPandClose(void)
	{
	BOOL success = TRUE;
///	success = uibp.Add();  //can't return here without closing dialog!
	success = (uibp.Close() == NULL) && success;
	if(!success)  return FALSE;  		
	return AddBPToList();
	};

bp *COBreakpoints::AddBPToList(void)
	{
	bp *pbp = new bp;
	bp_list.AddTail( pbp );
	return pbp;
	}

int COBreakpoints::FindBPIndex(POSITION pos)
	{
	int index=0;	 //YS: changed to 0 to keep consistent 0-based indexing
	for (POSITION bp_pos = bp_list.GetHeadPosition(); ((bp_pos != NULL) && (bp_pos!=pos));)
	{
		bp_list.GetNext(bp_pos);
		index++;
	}
	if ((pos==NULL) || (bp_pos==NULL)) return -1;	
	return index;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COBreakpoints::EditCode(bp *pbp, LPCSTR szFileDir /* NULL */)
// Description: Displays the source file (and cursor) where the specified bp is located,  
// Param: pbp A pointer to a bp object that specifies the breakpoint in question.
// Param: szFileDir An optional string specifying the file path if it needs to be supplied for the debugger to find the file.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COBreakpoints::EditCode(bp *pbp, LPCSTR szFileDir /* NULL */)

{
	// figure out which position in the bp list this bp should be at.
	int bpindex = FindBPIndex(bp_list.Find(pbp));
	
	// make sure we got a valid position.
	if(bpindex < 0)
	{
		LOG->RecordInfo("ERROR in COBreakpoints::EditCode() - can't find bp in list");
		return FALSE;
	}

	// active the bp dlg.
	if(!uibp.Activate())
	{
		LOG->RecordInfo("ERROR in COBreakpoints::EditCode() - can't activate bp dlg");
		return FALSE;
	}

	// select the specified bp.
	if(!uibp.SetCurrentBreakpoint(bpindex))
	{
		LOG->RecordInfo("ERROR in COBreakpoints::EditCode() - can't select bp %d", bpindex);
		return FALSE;
	}

	// if a file path was specified, we are expecting the find source dlg & want to handle it
	if(szFileDir)
		UIWB.m_pTarget->SetHandleMsgs(FALSE);
	// select the edit code option in the bp dlg.
	if(!uibp.EditCode())
	{
		LOG->RecordInfo("ERROR in COBreakpoints::EditCode() - can't edit code from bp dlg");
		UIWB.m_pTarget->SetHandleMsgs(TRUE);
		return FALSE;
	}
	
	// wait around 10 seconds max for the bp dlg to go away.
	int iSleep = 0;
	while(uibp.IsActive())
	{
		Sleep(1000);
		if(++iSleep == 10)
		{
			LOG->RecordInfo("ERROR in COBreakpoints::EditCode() - bp dlg still active after 10 seconds");
			UIWB.m_pTarget->SetHandleMsgs(TRUE);
			return FALSE;
		}
	}

	// if a file path was specified, we are expecting the find source dlg.
	if(szFileDir)
	
	{
		// set up the object for the Find Source dlg.
		UIDialog dlgFindSource(GetLocString(IDSS_FS_TITLE));

		// wait up to 3 seconds for Find Source dlg to come up.
		if(dlgFindSource.WaitAttachActive(3000))
		{
			// verify that it is the Find Source dlg.
			if(dlgFindSource.VerifyTitle() )
			{
				// enter the specified directory and close the dlg.
				MST.DoKeys(szFileDir);
				dlgFindSource.OK();
			}
			else
			{
				LOG->RecordInfo("ERROR in COBreakpoints::EditCode() - dlg other than Find Source came up");
				UIWB.m_pTarget->SetHandleMsgs(TRUE);
				return FALSE;
			}
		}
		else
		{
			LOG->RecordInfo("ERROR in COBreakpoints::EditCode() - no dlg came up (expected Find Source)");
			UIWB.m_pTarget->SetHandleMsgs(TRUE);
			return FALSE;
		}
	}
	UIWB.m_pTarget->SetHandleMsgs(TRUE);
	return TRUE;  		
}


// BEGIN_HELP_COMMENT
// Function: BOOL COBreakpoints::RemainingPassCountIs(bp *pbp, int iPassCount)
// Description: Compares teh passcount of a specified bp with a specified value.  
// Param: pbp A pointer to a bp object that specifies the breakpoint in question.
// Param: iPassCount An integer specifying the expected pass count.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COBreakpoints::RemainingPassCountIs(bp *pbp, int iPassCount)

{
	// figure out which position in the bp list this bp should be at.
	int bpindex = FindBPIndex(bp_list.Find(pbp));
	
	// make sure we got a valid position.
	if(bpindex < 0)
	{
		LOG->RecordInfo("ERROR in COBreakpoints::RemainingPassCountIs() - can't find bp in list");
		return FALSE;
	}

	// active the bp dlg.
	if(!uibp.Activate())
	{
		LOG->RecordInfo("ERROR in COBreakpoints::RemainingPassCountIs() - can't activate bp dlg");
		return FALSE;
	}

	// get the bp string.
	CString strBp;
	MST.WListItemText("@1", bpindex + 1, strBp);
	
	// close the bp dlg.
	if(uibp.Close())
	{
		LOG->RecordInfo("ERROR in COBreakpoints::RemainingPassCountIs() - bp dlg still active after 10 seconds");
		return FALSE;
	}

	// see if the passcount is as expected.
	char szPassCount[12];
	if(strBp.Find((CString)_itoa(iPassCount, szPassCount, 10) + " remaining") == -1)
	{
		LOG->RecordInfo("ERROR in COBreakpoints::RemainingPassCountIs() - "
						"couldn't find \"%s remaining\" in \"%s\"", szPassCount, strBp);
		return FALSE;
	}

	return TRUE;  		
}
