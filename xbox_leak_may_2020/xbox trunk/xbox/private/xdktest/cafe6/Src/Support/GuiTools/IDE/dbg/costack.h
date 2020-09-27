///////////////////////////////////////////////////////////////////////////////
//  COStack.H
//
//  Created by :            Date :
//      WayneBr             1/14/94
//
//  Description :
//      Declaration of the COStack class
//

#ifndef __COSTACK_H__
#define __COSTACK_H__

#include "uistack.h"
#include "uivar.h"

#include "dbgxprt.h"

///////////////////////////////////////////////////////////////////////////////
//  COStack class

//
// class Frame - utility class for operating on frame from callstack window
//
// BEGIN_CLASS_HELP
// ClassName: Frame
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS Frame {
public:
	Frame(){}
	Frame( LPCTSTR frame ) { m_strFrame = frame; };
	operator LPCTSTR () const { return LPCTSTR( m_strFrame ); };

	//	Functions for extracting parts of Frame strings.
	BOOL GetAbsoluteAddress( CString& str ) const;
	BOOL GetFunctionName( CString& str ) const;
	BOOL GetImageName( CString& str ) const;
	BOOL GetLineNumber( CString& str ) const;
	BOOL GetByteOffset( CString& str ) const;
	BOOL GetParameters( CString& str ) const;
	BOOL GetParameterType( CString& str, int index = 0 ) const;
	BOOL GetParameterValue( CString& str, int index = 0 ) const;

	BOOL Test( CString str ) const;

private:
	CString m_strFrame;
	int GetParam( CString& str, int index) const;
};


//
// class FrameList
//
// BEGIN_CLASS_HELP
// ClassName: FrameList
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS FrameList {
public:
	// CreateFromText from clipboard.
	BOOL CreateFromText( CString str );
	FrameList();
	FrameList(char const* const* ppchar);

	// General utilities
	int Index( CString str ) const;
	CString operator[] ( int index ) const { return m_StringArray[index]; }
	int GetSize() const { return m_StringArray.GetSize(); }

	enum Filter {
		NoFilter = 0,
		NoAbsoluteAddresses,
	};

	// Testing operations
	BOOL Compare( char const * const * stk, Filter filter, int top, int bottom ) const;

	// General common sense checks on callstack
	BOOL GeneralIntegrity() const;

private:
	CStringArray m_StringArray;
};


//
//	class COStack.
//
class CODebug; // Forward declaration

// BEGIN_CLASS_HELP
// ClassName: COStack
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS COStack {
	friend CODebug;

// Data
private:
    UIStack uistack;
	UIVariables uivar;
	CODebug* m_pOwner;
	BOOL m_bDirty;
	CString m_LastStack;

// Call Stack Utilities
public:
	COStack( CODebug* owner = NULL );
    BOOL CurrentFunctionIs(LPCSTR str);
    BOOL VerifyCurrentFunction(LPCSTR str);  // logs warning if not =
    CString GetFunction(int level = 0, BOOL bNameOnly = FALSE);
    CString GetAllFunctions(void);
    int  NavigateStack(int level = 0);
    int  NavigateStack( CString str );
	BOOL RunToFrame(CString str,  int ambiguity = 0);
	BOOL RunToFrame(int level, int ambiguity = 0); 
	BOOL FunctionIs(LPCSTR str, int level = 0 );
	BOOL ToggleBreakpoint(int level = 0);

    int Index( CString str );
	BOOL Compare( char const * const * stk, FrameList::Filter filter = FrameList::NoFilter, int top = 0, int bottom = -1 );

	void SetDirty( BOOL dirty ) { m_bDirty = dirty; }
	BOOL IsDirty() const;

protected:
};


#endif // __COSTACK_H__
