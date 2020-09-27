///////////////////////////////////////////////////////////////////////////////
//  COSTACK.CPP
//
//  Created by :            Date :
//      WayneBr                 1/14/94
//
//  Description :
//      Implementation of the COStack class
//

#include "stdafx.h"
#include "costack.h"
#include "testxcpt.h"
#include "mstwrap.h"
#include "..\sym\qcqp.h"
#include "guiv1.h"
#include "..\shl\uwbframe.h"
#include "codebug.h"
#include "uidebug.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

COStack::COStack( CODebug* owner ) {
	m_pOwner = NULL;
	if( owner ) {
		// Setup cooperation with owning CODebug instance.
		owner->SetOwnedStack( this );
	}
}


// BEGIN_HELP_COMMENT
// Function: BOOL COStack::RunToFrame(CString strFrame, int ambiguity /* 0 */)
// Description: Run to the given call stack frame.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: strFrame A CString that contains the name of the call stack frame to run to.
// Param: ambiguity An integer that contains the 1-based index of the list of entries to resolve an ambiguity. (Default value is -1.)
// END_HELP_COMMENT
BOOL COStack::RunToFrame(CString strFrame, int ambiguity /* 0 */)
{
	return RunToFrame(Index(strFrame), ambiguity);
}


// BEGIN_HELP_COMMENT
// Function: int COStack::Index( CString str )
// Description: Get the index of the given call stack frame in the current call stack.
// Return: An integer that contains the index of the given call stack frame.
// Param: str A CString that contains the name of the call stack frame.
// END_HELP_COMMENT
int COStack::Index( CString str ) {
	CString strStack = GetAllFunctions();
	FrameList list;
	list.CreateFromText( strStack );
	return list.Index( str );
}


// BEGIN_HELP_COMMENT
// Function: BOOL COStack::CurrentFunctionIs(LPCSTR str)
// Description: Determine if the current function matches the given function.
// Return: A Boolean value that indicates whether the current function matches the given function (TRUE) or not.
// Param: str A pointer to a string that contains the name of the expected function.
// END_HELP_COMMENT
BOOL COStack::CurrentFunctionIs(LPCSTR str) {
		return FunctionIs( str, 0 );
}

// BEGIN_HELP_COMMENT
// Function: BOOL COStack::VerifyCurrentFunction(LPCSTR str)
// Description: Determine if the current function matches the given function.
// Return: A Boolean value that indicates whether the current function matches the given function (TRUE) or not.
// Param: str A pointer to a string that contains the name of the expected function.
// END_HELP_COMMENT
BOOL COStack::VerifyCurrentFunction(LPCSTR str) {
		return FunctionIs( str, 0 );
}

// BEGIN_HELP_COMMENT
// Function: CString COStack::GetFunction(int level /* 0 */, BOOL bNameOnly /* FALSE */)
// Description: Get the call stack description string at the given call stack level (index).
// Return: A CString that contains the call stack description string from the given call stack level.
// Param: level An integer that contains the 0-based index into the call stack. (Default value is 0.)
// Param: bNameOnly A boolean specifying whether or not to return the name of the function only (rather than the whole line)..
// END_HELP_COMMENT
CString COStack::GetFunction(int level /* 0 */, BOOL bNameOnly /* FALSE */)	{
	HWND hFocus = ::WGetFocus();

	CString current;

#if defined(V4LOW)	// V4Low (or standard) does not have a call-stack window.
	EXPECT(uivar = UIDebug::ShowDockWindow(IDW_LOCALS_WIN));
	current = uivar.GetFunction(level);
#else
	EXPECT(uistack = UIDebug::ShowDockWindow(IDW_CALLS_WIN));
	// if callstack line in question displays same string
	// over 500 ms, we'll assume it is done painting.
	// we'll only wait 5 seconds though.
	CString last = uistack.GetFunction(level);

	for(int i = 0; i < 10; i++)
		
		{
		Sleep(500);
		current = uistack.GetFunction(level);

		if((current == last) && (current != ""))
			break;
		
		last = current;
		}

	// REVIEW (michma): we don't want the stack to be closed so it is visible during the sniff (more
	// coverage). however, what we probably should do later is close it	only if it wasn't open previously.
	// the memory and ee component classes already support such behavior.
	//EXPECT(uistack.Close());	
#endif

	if( hFocus ) {
		::QueSetFocus( hFocus );
		::QueFlush( FALSE );
	}
	else {
		LOG->RecordInfo( "COStack::GetFunction(): focus was NULL" );
	}

	// check if we are returning the function name only.
	if(bNameOnly)
	{
		// find the opening parentheses marking the end of the function name.
		int iParen = current.Find("(");
		// function name must be at least 1 character long.
		if(iParen > 0)
			// parse the function name.
			current = current.Left(current.GetLength() - iParen);
	}

	return current;
}

// BEGIN_HELP_COMMENT
// Function: CString COStack::GetAllFunctions(void)
// Description: Get all the call stack frame descriptions for all frames on the call stack.
// Return: A CString that contains a newline-delimited list of call stack frames.
// END_HELP_COMMENT
CString COStack::GetAllFunctions(void) {
	if( IsDirty() ) {
		HWND hFocus = ::WGetFocus();

		#if defined(V4LOW)
			EXPECT(uivar = UIDebug::ShowDockWindow(IDW_LOCALS_WIN));
			m_LastStack = uivar.GetAllFunctions();
		#else
			EXPECT(uistack = UIDebug::ShowDockWindow(IDW_CALLS_WIN));
			m_LastStack = uistack.GetAllFunctions();
			// REVIEW (michma): we don't want the stack to be closed so it is visible during the sniff (more
			// coverage). however, what we probably should do later is close it	only if it wasn't open previously.
			// the memory and ee component classes already support such behavior.
			// EXPECT(uistack.Close());
		#endif

		if( hFocus ) {
			::QueSetFocus( hFocus );
			::QueFlush( FALSE );
		}
		else {
			LOG->RecordInfo( "COStack::GetAllFunctions(): focus was NULL" );
		}
		SetDirty( FALSE );
	}

	return  m_LastStack;
}

// BEGIN_HELP_COMMENT
// Function: int COStack::NavigateStack( CString str )
// Description: Navigate to the given call stack frame & checks the line number navigated to.
// Return: 0 if not successful; -1 if no source avail; line no otherwise.
// Param: str A CString that contains the name of the call stack frame to navigate to.
// END_HELP_COMMENT
int COStack::NavigateStack( CString str ) {
	return NavigateStack( Index( str ) );
}


// BEGIN_HELP_COMMENT
// Function: int COStack::NavigateStack(int level /* 0 */)
// Description: Navigate to the given call stack frame & checks the line number navigated to.
// Return: 0 if not successful; -1 if no source avail; line no otherwise.
// Param: level An integer that contains the 0-based index into the call stack. This parameter specifies which call stack to navigate to. (Default value is 0.)
// END_HELP_COMMENT
int COStack::NavigateStack(int level /* 0 */) {
#if defined(V4LOW)
	EXPECT(uivar = UIDebug::ShowDockWindow(IDW_LOCALS_WIN));
	uivar.NavigateStack(level);
	return TRUE;	// Assume success!
#else
	EXPECT(uistack = UIDebug::ShowDockWindow(IDW_CALLS_WIN));
	return uistack.NavigateStack(level);
	// REVIEW (michma): we don't want the stack to be closed so it is visible during the sniff (more
	// coverage). however, what we probably should do later is close it	only if it wasn't open previously.
	// the memory and ee component classes already support such behavior.
	//uistack.Close();
#endif
}


// BEGIN_HELP_COMMENT
// Function: BOOL COStack::RunToFrame(int level, int ambiguity /* 0 */)
// Description: Run to the given call stack frame.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: level An integer that contains the 0-based index into the call stack. This parameter specifies which call stack to run to.
// Param: ambiguity An integer that contains the 1-based index of the list of entries to resolve an ambiguity. (Default value is -1.)
// END_HELP_COMMENT
BOOL COStack::RunToFrame(int level, int ambiguity /* 0 */) 

{
	EXPECT(uistack = UIDebug::ShowDockWindow(IDW_CALLS_WIN));
	uistack.GoToFrame(level);
//REVIEW(chriskoz) this is the F7 command from uistack context, cannot be executed from UIWB context (dbg.StepToCursor)
	MST.DoKeyshWnd(uistack, "{F7}" /*KEY_STEPTOCURSOR*/);

	CODebug dbg;
	if(ambiguity > 0)
		dbg.ResolveSourceLineAmbiguity(ambiguity);
	dbg.Wait(WAIT_FOR_BREAK);
	
	// return focus from stack window.
	MST.DoKeys("{ESC}");
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COStack::FunctionIs(LPCSTR str, int level /* 0 */)
// Description: Determine if the name of the function at the specified call stack level matches the given function name.
// Return: A Boolean value that indicates whether the function name at the specified call stack level matches the given function name.
// Param: str A pointer to a string that specifies the expected name of the function.
// Param: level An integer that contains the 0-based index into the call stack. (Default value is 0.)
// END_HELP_COMMENT
BOOL COStack::FunctionIs(LPCSTR str, int level /* 0 */) {
	
	
	CString temp = GetFunction(level);

	if (  ((CString)temp).Find((CString)str) != (-1)  )
		return TRUE;
	else
	{
		// log warning
		LOG->RecordInfo( (LPCSTR) "Current Function is %s, expected %s",temp.GetBuffer(temp.GetLength()), str);
		return FALSE;
	}
}

// BEGIN_HELP_COMMENT
// Function: BOOL COStack::Compare( char const * const * stk, FrameList::Filter filter, int top, int bottom )
// Description: Compare a portion of the call stack to a given portion of the call stack.
// Return: A Boolean value that indicates whether the specified portion of the call stack matches the given call stack (TRUE) or not.
// Param: stk A pointer to a portion of the call stack. This is commonly a pointer to part of the buffer retrieved by GetAllFunctions(void).
// Param: filter A value that specifies the type of filtering to do when comparing call stacks: FrameList::NoFilter, FrameList::NoAbsoluteAddresses. (Default value is FrameList::NoFilter.)
// Param: top An integer that specifies the 0-based index into the call stack to start comparison. (Default value is 0.)
// Param: bottom An integer that specifies the 0-based index into the call stack to end comparison. -1 means the bottom of the stack. (Default value is -1.)
// END_HELP_COMMENT
BOOL COStack::Compare( char const * const * stk, FrameList::Filter filter, int top, int bottom ) {
	CString strStack = GetAllFunctions();
	FrameList list;
	list.CreateFromText( strStack );
	return list.Compare( stk, filter, top, bottom );
}

//
// class FrameList
//
FrameList::FrameList() {
}

FrameList::FrameList(char const* const* ppchar)   {
	while( *ppchar ) {
		m_StringArray.Add( *ppchar );
		ppchar++;
	}
}

BOOL FrameList::CreateFromText( CString str ) {
	const CString CrLf( "\r\n" );
	const int cCrLf = CrLf.GetLength();

	// Clear string array
	m_StringArray.RemoveAll();

	while( str.GetLength() ) {
		CString frame;

		int count = str.Find( CrLf );

		if( -1 != count ) {
			frame = str.Left( count );
			str = str.Mid( count + cCrLf );
		}
		else {
			frame = str;
			str = "";
		}
		m_StringArray.Add( frame );
	}

	return TRUE;
}

int FrameList::Index( CString frame0 ) const {
	int index = 0;

	for( index = 0; index < m_StringArray.GetSize(); index++) {
		Frame frame1( m_StringArray[index] );

		if( frame1.Test( frame0 ) )
			return index;
	}
	LOG->RecordInfo( "FrameList::Index - '%s' not found", (LPCTSTR)frame0 );
	return -1;
}

BOOL FrameList::Compare( char const * const * stk, Filter filter, int top, int bottom ) const {
	BOOL bResult = TRUE;

	// Do general integrity check for warnings.  Ignore return code
	GeneralIntegrity();

	//
	//	Do a frame by frame compare
	//
	if( bottom == -1 ) {
		bottom = m_StringArray.GetUpperBound();
	}

	// Check limits.
	bResult = top >= 0 && bottom >= top && bottom <= m_StringArray.GetUpperBound();
	if( !bResult ) {
		// Write warnings.
		LOG->RecordInfo( "FrameList::Compare - Invalid param: top=%d, bottom=%d, UpperBound=%d",
			top, bottom, m_StringArray.GetUpperBound() );

	}

	// Setting bSkip to TRUE when '*' is encountered in stk allows skipping
	// frames until next match is found.  Useful for skipping system dependent frames
	// when using system callbacks
	BOOL bSkip = FALSE;
	for( int index = top; bResult && index <= bottom; index++ ) {
		Frame frame( m_StringArray[index] );

		// Special "*" in stk.
		if( CString( *stk ) == CString( "*" ) ) {
			stk++;
			ASSERT( *stk );
			ASSERT( **stk );
			ASSERT( CString( *stk ) != CString( "*" ) );
			bSkip = TRUE;
		}

		// Apply filter.
		if( filter == NoAbsoluteAddresses ) {
			CString tmp;
			if( frame.GetAbsoluteAddress( tmp ) )
				continue;
		}

		if( *stk ) {
			bResult = frame.Test( *stk );

			if( bResult ) {
				bSkip = FALSE;
				stk++;
			}
			else if ( !bSkip ) {
				LOG->RecordInfo( "FrameList::Compare - Mismatch on Frame[%d] - Expected '%s', Got '%s'",
					index, *stk, (LPCTSTR)frame );
				stk++;
			}
			else {
				// Continue loop
				bResult = TRUE;
			}

		}
		else {
			bResult = FALSE;
			LOG->RecordInfo( "FrameList::Compare - %d is not enough baseline frames",
				index-top );
		}
	}

	// Make sure that we used the entire baseline.
	if( *stk ) {
		bResult = FALSE;
		LOG->RecordInfo( "FrameList::Compare - Never checked %s in baseline", *stk );
	}

	// Consistency check.  This should only happen if *stk != NULL
	if( bSkip ) {
		bResult = FALSE;
		LOG->RecordInfo( "FrameList::Compare - Never reset 'bSkip'" );
	}
	return bResult;
}

BOOL FrameList::GeneralIntegrity() const {
	BOOL bResult = TRUE;

	// Checks
	// 	consecutive duplicate frames
	//	blank frame
	//	
	CString lastFrame;
	for( int index = 0; bResult && index <= m_StringArray.GetUpperBound(); index++ ) {
		Frame frame( m_StringArray[index] );

		if( lastFrame == m_StringArray[index] ) {
			LOG->RecordInfo( "FrameList::GeneralIntegrity - '%s' duplicated", (LPCTSTR)lastFrame );
			bResult = FALSE;
		}

		CString tmp;
		// Absolute address should always be include with image name.
		if( frame.GetImageName( tmp ) ) {
			if( !frame.GetAbsoluteAddress( tmp ) ) {
				LOG->RecordInfo( "FrameList::GeneralIntegrity - '%s' image name without absolute address", 
					(LPCTSTR)m_StringArray[index] );
				bResult = FALSE;
			}
		}

		// Frame without absolute address should contain function name
		if( !frame.GetAbsoluteAddress( tmp ) ) {
			if( !frame.GetFunctionName( tmp ) ) {
				LOG->RecordInfo( "FrameList::GeneralIntegrity - '%s' No function or absolute address", 
					(LPCTSTR)m_StringArray[index] );
				bResult = FALSE;
			}
		}

		lastFrame = m_StringArray[index];
	}

	return bResult;
}

//
// class Frame
//
BOOL Frame::GetAbsoluteAddress( CString& str ) const {
	//
	// Assume that absolute address has the form XXXXXXXX().
	//
	CString frame = m_strFrame;

	BOOL found = FALSE;
	str.Empty();
	int index = frame.Find( "(" );

	found = -1 != index;
	if( found ) {
		frame = frame.Left( index );
		frame.MakeReverse();
		frame = frame.SpanExcluding( " " );
		frame.MakeReverse();

		// Must have exactly 8.  I don't want to mismatch on short names like abc();
		if( frame.GetLength() != 8 ) {
			found = FALSE;
		}
		else {
			// Allow only lower case hexadecimal.
			str = frame.SpanIncluding( "0123456789abcdef" );
			if( str != frame ) {
				str.Empty();
				found = FALSE;
			}
		}
	}

	return found;
}

BOOL Frame::GetFunctionName( CString& str ) const {
	//
	// Assume that function name and absolute address or image name are mutally exclusive.
	// Assume that the function name if it exists starts at beginning of string
	//
	CString frame = m_strFrame;
	CString tmp;

	BOOL found = FALSE;
	str.Empty();
	if( !GetAbsoluteAddress( tmp ) ) {
		if( GetImageName( tmp ) )
		{
			frame = frame.Mid( tmp.GetLength() + 1); //skip image name + '!'
			frame.TrimLeft(); //get rid of leading spaces
		}
		int index = frame.Find( "(" );

		if( -1 == index )
			index = frame.Find( " " );

		if( -1 != index )
			frame = frame.Left( index );

		if( !frame.IsEmpty() ) {

			// Make sure first character of function is not a digit
			TCHAR ch = frame[0];

			if( IsCharAlphaNumeric( ch ) && !IsCharAlpha( ch ) ) {
				found = FALSE;
			}
			else {
				str = frame;
				found = TRUE;
			}
		}
	}
	return found;
}

BOOL Frame::GetImageName( CString& str ) const {
	//
	// Assume that image name if it exists is at the beginning of the line, is appended with '!'
	// and has no spaces.
	//
	CString frame = m_strFrame;

	BOOL found = FALSE;
	str.Empty();
	int index = frame.Find( "!" );
	found = -1 != index;
	if( found ) {
		frame = frame.Left( index );
		if( frame.IsEmpty() || frame.Find( ' ' ) != -1 ) {
			found = FALSE;
		}
		else {
			str = frame;
		}
	}

	return found;
}

BOOL Frame::GetLineNumber( CString& str ) const {
	CString frame = m_strFrame;

	BOOL found = FALSE;
	str.Empty();
	const CString prefix( "line " );
	int index = frame.Find( prefix );
	if( -1 != index ) {
		frame = frame.Mid( index + prefix.GetLength() ).SpanIncluding( "1234567890" );
		if( !frame.IsEmpty() ) {
			str = frame;
			found = TRUE;
		}
	}
	return found;
}

BOOL Frame::GetByteOffset( CString& str ) const {
	CString frame = m_strFrame;

	BOOL found = FALSE;
	str.Empty();
	const CString postfix( " byte" );
	int index = frame.Find( postfix );
	if( -1 != index ) {
		frame = frame.Left( index );
		frame.MakeReverse();
		frame = frame.SpanIncluding( "1234567890" );
		frame.MakeReverse();
		if( !frame.IsEmpty() ) {
			str = frame;
			found = TRUE;
		}
	}
	return found;
}

BOOL Frame::GetParameters( CString& str ) const {
	BOOL found = FALSE;
	str.Empty();
	int first = m_strFrame.Find( '(' );
	int last = m_strFrame.ReverseFind( ')' );
	if( first != -1 && last > first ) {
		found = TRUE;
		// Include stuff between but excluding parens
		str = m_strFrame.Mid( first+1, last - first - 1 );
	}
	return found;
}

BOOL Frame::GetParameterType( CString& str, int index ) const
{
	BOOL found = FALSE;
	int space_pos = GetParam(str, index);
	if( -1 != space_pos )
	{
		str = str.Left( space_pos );
		found = TRUE;
	} //else NOTE: space_pos can be -1 & str not empty if not Type & Value are present
	return found;
}

BOOL Frame::GetParameterValue( CString& str, int index ) const 
{
	BOOL found = FALSE;
	int space_pos = GetParam(str, index);
	if( -1 != space_pos )
	{
		str = str.Mid( space_pos+1 );
		found = TRUE;
	} //else NOTE: space_pos can be -1 & str not empty if not Type & Value are present
	return found;
}


//Finds the space before the ADDRESS
//RETURN: -1 if not found or ADDRESS is the leftmost substring of params
int FindAddress(const CString& param)
{
	int start,ndigit;
	if((start=param.Find(" 0x")) >=0) //ADDRESS must be prefixed by space
	{
		for(ndigit=0;ndigit<8;ndigit++)
		{
			if (!isxdigit(param[start+3+ndigit]))
			{
				start = -1; //this is not the address
				break;
			}
		}
	} //else NOTE: returns -1 when ADDRESS is the leftmost substr, which is expected
	return start;
}  //FindAddress


// Gets the given parameter and returns the position of the space 
// separating its type & its value in m_strFrame.
// Function assumes both type and value are present in m_strFrame
// Param: index - the 0-indexed parameter number
//RETURN: str - retrieved parameter string
//RETURN: the position of the space separating type & value (-1 if not found)
int Frame::GetParam( CString& str, int index ) const 
{
	int space_pos=-1;
	str.Empty();
	CString params;
	if( GetParameters( params ) ) 
	{

		// Only handle very simple types without spaces.

		// Get start of nth parameter.
		int count = index;
		int start = 0;
		while( count ) {
			start = params.Find( ',' );
			if( -1 == start ) {
				break;
			}
			else {
				params = params.Mid( start + 1 );
				count--;
			}
		}

		if( -1 != start ) 
		{
			int last = params.Find( ',' );
			if( -1 != last ) {
				params = params.Left( last );
			}
			
			// Get rid of tabs & remove leading & trailing white chars
			params.Replace('\t',' ');
			params.TrimLeft();
			params.TrimRight();

			if( (space_pos=FindAddress(params)) <0)
				space_pos = params.ReverseFind( ' ' );
			str = params;
		}
	}
	return space_pos;
} //GetParam



BOOL Frame::Test( CString str ) const {
	//
	// Only test elements that exist in str
	//
	BOOL result = TRUE;
	Frame testFrame( str );
	CString str0, str1;

	if( result && testFrame.GetAbsoluteAddress( str0 ) ) {
		result = GetAbsoluteAddress( str1 ) && str0 == str1;
	}

	if( result && testFrame.GetImageName( str0 ) ) {
		result = GetImageName( str1 ) && str0 == str1;
	}

	if( result && testFrame.GetFunctionName( str0 ) ) {
		result = GetFunctionName( str1 ) && str0 == str1;
	}

	if( result && testFrame.GetLineNumber( str0 ) ) {
		result = GetLineNumber( str1 ) && str0 == str1;
	}

	if( result && testFrame.GetByteOffset( str0 ) ) {
		result = GetByteOffset( str1 ) && str0 == str1;
	}

	if( result && testFrame.GetParameters( str0 ) ) {

		// Parameters must agree in number and type.  Value check is optional.
		result = GetParameters( str1 );
		int i = 0;
		while( result && testFrame.GetParameterType( str0, i ) ) {
			result = GetParameterType( str1, i ) && str0 == str1;

			// Optional value check
			if( result && testFrame.GetParameterValue( str0, i ) ) {
				result = GetParameterValue( str1, i );
				// Compare on if not special case for '%' which doesn't check actual value
				if( result && str0[0] != '%' ) {
					result = str0 == str1;
				}
			}
			i++;
		}

		// Make sure that Frame doesn't have more parameters than testFrame.
		result = result && !GetParameterType( str1, i );
	}

#if 0
	// This might be printed in a special mode.
	if( !result ) {
		LOG->RecordInfo( "Frame::Test - Expected '%s', Got '%s'", (LPCTSTR)str0, (LPCTSTR)str1 );
	}
#endif

	return result;
}

BOOL COStack::IsDirty() const {

	// Always dirty if no CODebug owner
	BOOL dirty = TRUE;

	if( m_pOwner ) {
		dirty = m_bDirty;
	}
	return dirty;
}


// BEGIN_HELP_COMMENT
// Function: BOOL COStack::ToggleBreakpoint(int level /* 0 */)
// Description: Run to the given call stack frame.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: level An integer that contains the 0-based index into the call stack. This parameter specifies which frame to toggle the breakpoint on.
// END_HELP_COMMENT
BOOL COStack::ToggleBreakpoint(int level /* 0 */) 
{
	EXPECT(uistack = UIDebug::ShowDockWindow(IDW_CALLS_WIN));
	uistack.GoToFrame(level);
	MST.DoKeyshWnd(uistack, KEY_TOGGLE_BREAKPOINT);
	return TRUE;
}

