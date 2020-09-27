///////////////////////////////////////////////////////////////////////////////
//  UIBROWSE.CPP
//
//  Created by :            Date :
//      WayneBr                 1/21/94
//
//  Description :
//      Implementation of the UIBrowse class
//

#include "stdafx.h"
#include "uibrowse.h"
#include "testxcpt.h"
#include "..\..\testutil.h"
#include "mstwrap.h"
#include "..\sym\cmdids.h"
#include "..\sym\vcpp32.h"
#include "guiv1.h"
#include "..\shl\uwbframe.h"
#include "..\shl\wbutil.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

static const LPCTSTR szPushPinName = "PUSHPIN";
/*
static const LPCTSTR szClassesFilterName = "&Classes";
static const LPCTSTR szFunctionsFilterName = "F&unctions";
static const LPCTSTR szDataFilterName = "D&ata";
static const LPCTSTR szMacrosFilterName = "&Macros";
static const LPCTSTR szTypesFilterName = "T&ypes";

// REVIEW (michma): need to use accelerators to get at filter buttons instead of clicking
// because we can't guarantee that the buttons will be completely visible and
// ms-test may not be able to click buttons that aren't completely visible.  it
// seems to try to click in the center of the button.
static const LPCTSTR szClassesFilterAccel = "%c";
static const LPCTSTR szFunctionsFilterAccel = "%u";
static const LPCTSTR szDataFilterAccel = "%a";
static const LPCTSTR szMacrosFilterAccel = "%m";
static const LPCTSTR szTypesFilterAccel = "%y";
*/

// Utility for filling CStringArray from clipboard.
// BEGIN_HELP_COMMENT
// Function: BOOL FillStringArrayFromClipBrd( CStringArray& lines )
// Description: Fill the given CStringArray with lines from the clipboard.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: lines A reference to a CStringArray that will contain the lines of text from the clipboard.
// END_HELP_COMMENT
static BOOL FillStringArrayFromClipBrd( CStringArray& lines ) {

	const CString CrLf( "\r\n" );
	const int cCrLf = CrLf.GetLength();

	lines.RemoveAll();

	CString strClip;

	if( GetClipText(strClip) && !strClip.IsEmpty() ) {
		while( strClip.GetLength() ) {
			CString line;

			int count = strClip.Find( CrLf );

			if( -1 != count ) {
				line = strClip.Left( count );
				strClip = strClip.Mid( count + cCrLf );
			}
			else {
				line = strClip;
				strClip = "";
			}
			lines.Add( line );
		}
	}

	return !!lines.GetSize();
}

void UIBrowse::OnUpdate(void) {
	if( HWnd() ) {
		// Look for "PUSHPIN" Button in browser window
		HWND hWndPushButton = CMSTest::WFndWndC( szPushPinName, "Button", FW_DEFAULT );
		if( hWndPushButton ) {
			if( IsChild( HWnd(), hWndPushButton ) ) {
				m_hWndPushPin = hWndPushButton;
				return;
			}
		}
	}
	m_hWndPushPin = NULL;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::IsValid(void) const
// Description: Determine if the browser window is valid by verifying the push pin control exists.
// Return: A Boolean value that indicates whether the browser window is valid. TRUE if it's valid; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::IsValid(void) const {
	if( UIWindow::IsValid() ) {
#if 0
		// Test for unique "PUSHPIN" Button in browser window
		HWND hWndPushButton = CMSTest::WFndWndC( szPushPinName, "Button", FW_DEFAULT );
		if( hWndPushButton ) {
			return IsChild( HWnd(), hWndPushButton );
		}
#else
		if( m_hWndPushPin )
			return TRUE;
#endif
		LOG->RecordInfo( "UIBrowse::IsValid - Couldn't find 'PUSHPIN'" );
	}

	return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::Find(void)
// Description: Find the browser window and attach to it.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::Find(void) {

	// Find unique "PUSHPIN" Button in browser window
	HWND hWndPushButton = CMSTest::WFndWndC( szPushPinName, "Button", FW_DEFAULT );
	if( hWndPushButton ) {
		return Attach( GetParent(GetParent(hWndPushButton)) );
	}
	return FALSE;
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::IsActive(void)
// Description: Determine if the browser window is active.
// Return: A Boolean value that indicates whether the browser is active. TRUE if it's active; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::IsActive(void) {
    HWND hwnd = MST.WGetFocus();
    return (hwnd == HWnd()) || IsChild(HWnd(), hwnd);
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::Query(QueryType query, LPCSTR name /*NULL*/, int filters /*NONE*/)
// Description: Perform a query via the Browse dialog that results in the browser window being active.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: query A QueryType value that indicates the type of query to perform: DefRef, FileOutline, BaseClass, DerivedClass, CallGraph, CallerGraph.
// Param: name A pointer to a string that contains the name of the symbol to query on. (Default value is NULL.)
// Param: filter A bit field that specifies the type of filter to apply to the query: NONE, CLASSES, FUNCTIONS, DATA, MACROS, TYPES. (Default value is NONE.)
// END_HELP_COMMENT
BOOL UIBrowse::Query(QueryType query, LPCSTR name /*NULL*/, int filters /*NONE*/, int case_sensitive /* TRUE */) {

	// Most recent information
	m_type = query;
	m_filters = filters;
	m_name = name;

	EXPECT( UIWB.DoCommand( IDM_BROWSE_QUERY, DC_MNEMONIC ) );

	// look for error dialog
	UIDialog dlg( GetLocString(IDSS_WORKBENCH_TITLEBAR) );
	if( dlg.AttachActive() )
	{
		LOG->RecordInfo( "UIBrowse::Query - Browse operation resulted in error, e.g., File not found, etc." );
		dlg.Cancel();

		// assume file not found
		m_status = FileNotFound;
		return FALSE;
	}

    // wait for query dlg
	HWND hWndBrowseDialog;
	
	if((hWndBrowseDialog = MST.WFndWndWait(GetLocString(IDSS_TB_BROWSE), FW_NOCASE | FW_DIALOG | FW_FOCUS, 3)) == NULL)
		{
		LOG->RecordInfo( "UIBrowse::Query - query dlg did not appear within 3 seconds" );
		return FALSE;
		}

	UIWindow uiBrowseDialog;
	uiBrowseDialog.Attach( hWndBrowseDialog );

    // Enter Name (use defualt of NULL)
    if (name)
    {
        EXPECT(MST.WEditEnabled(GetLabel(VCPP32_IDC_QY_NAME)));
        MST.WEditSetText(GetLabel(VCPP32_IDC_QY_NAME), name);

    }
	else {
		LOG->RecordInfo( "UIBrowse::Query - search name is blank" );
		m_status = SymbolNotFound;
		return FALSE;
	}

    //Query
    // assumes querytypes  maps exactly to listbox !!!
    EXPECT(MST.WListEnabled(GetLabel(VCPP32_IDC_QUERY)));
    MST.WListItemClk(GetLabel(VCPP32_IDC_QUERY), (int) query);

	// set case-sensitivty.
	if(case_sensitive)
		ToggleCaseSensitivity(TOGGLE_ON);
	else
		ToggleCaseSensitivity(TOGGLE_OFF);

    //OK
    EXPECT(MST.WButtonEnabled(GetLabel(IDOK)));
    MST.WButtonClick(GetLabel(IDOK));

	// Test that browse dialog is gone
	EXPECT( uiBrowseDialog.WaitUntilGone(5000) );

	// Check for ambiguity dialog
	UIAmbiguiousSymbolDialog dlgAmb;
	if( dlgAmb.AttachActive() )
	{
		m_status = AmbiguityDialog;
		return FALSE;
	}

	// Check for not found
	if( dlg.AttachActive() )
	{
		LOG->RecordInfo( "UIBrowse::Query - Browse query resulted in error, e.g., Symbol not found, etc." );
		dlg.Cancel();

		// assume symbol not found
		m_status = SymbolNotFound;
		return FALSE;
	}

    // Wait for browse window
    // review really define UIBROWSE_CLASS
    //        #define UIBROWSE_CLASS ""
    //        m_hwndbrowser=MST.WFndWndWaitC(NULL, UIBROWSE_CLASS, FW_FOCUS, 3);
	WaitAttachActive(1000);
	EXPECT( IsValid() );
	m_status = Active;

	// assume unpinned state.
	MST.WCheckCheck( szPushPinName );

    // Set Filters
    SetFilters(query, filters);

    // set pane data
    m_pane=SYMBOL_PANE;         // assumes symbol pane is always the default focus.

    return TRUE;
}

//static void SetFilterButton(UIBrowse* pBrowse, LPCTSTR szButtonAccel, BOOL fButtonDown) 
static void SetFilterButton(UIBrowse* pBrowse, UINT nButtonID, BOOL fButtonDown) 
{
	CStringArray matches;

	CString szButtonAccel = "%";
	szButtonAccel += ExtractHotKey(GetLabel(nButtonID));

	// get the count of matches before the filter button is toggled.
	EXPECT(pBrowse->GetMatches(matches));
	int match_count_before_toggle = matches.GetSize();

	// toggle the filter button.
	MST.DoKeys(szButtonAccel);
	// wait for the browser window to be updated.
	Sleep(1000);

	// get the count of matches after the filter button is toggled.
	EXPECT(pBrowse->GetMatches(matches));
	int match_count_after_toggle = matches.GetSize();

	// if the match count before and after are equal, we can leave the filter button as is.
	// otherwise, the following table illustrates how the second expression works:
	
	// AFTER < BEFORE ?		BUTTON DOWN ?		ACTION ON BUTTON
	// TRUE					TRUE				toggle back
	// TRUE					FALSE				leave as is
	// FALSE				TRUE				leave as is
	// FALSE				FALSE				toggle back

	if((match_count_before_toggle != match_count_after_toggle) &&
	   ((match_count_after_toggle < match_count_before_toggle) == fButtonDown))
		{
		// toggle the filter button again.
		MST.DoKeys(szButtonAccel);
		// wait for the browser window to be updated.
		Sleep(1000);
		}

	return;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::SetFilters(QueryType type, int filters)
// Description: Set the filters for the given query
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: type A QueryType value that indicates the type of query to set the filters for: DefRef, FileOutline, BaseClass, DerivedClass, CallGraph, CallerGraph.
// Param: filters A bit field that specifies the type of filter to set: NONE, CLASSES, FUNCTIONS, DATA, MACROS, TYPES. (Default value is NONE.)
// END_HELP_COMMENT
BOOL UIBrowse::SetFilters(QueryType type, int filters) {  		// set filters based on current type

	if( m_status != Active ) {
		LOG->RecordInfo( "UIBrowse::SetFilters -  m_status != Active" );
		return FALSE;
	}

    EXPECT( Activate() );

    int filt = 0;   // used for temp.
    switch (type)
    {
        case FileOutline:
			if( filters == NONE )
				filters = CLASSES|FUNCTIONS|DATA|MACROS|TYPES;
			// new implementation

			SetFilterButton (this, IDM_CTL_BROWSE_TYPE, filters / TYPES);
//			SetFilterButton (this, szTypesFilterAccel, filters / TYPES);
			filt = filters % TYPES;

            SetFilterButton (this, IDM_CTL_BROWSE_MACRO, filt / MACROS);
//            SetFilterButton (this, szMacrosFilterAccel, filt / MACROS);
            filt = filt    % MACROS;

            SetFilterButton (this, IDM_CTL_BROWSE_DATA, filt / DATA);
//            SetFilterButton (this, szDataFilterAccel, filt / DATA);
            filt = filt    % DATA;

            SetFilterButton (this, IDM_CTL_BROWSE_FUNCS, filt / FUNCTIONS);
//            SetFilterButton (this, szFunctionsFilterAccel, filt / FUNCTIONS);
            filt = filt   % FUNCTIONS;

            SetFilterButton (this, IDM_CTL_BROWSE_CLASS, filt / CLASSES);
//            SetFilterButton (this, szClassesFilterAccel, filt / CLASSES);

			m_filters = filters;
            return TRUE;
            break;

        case BaseClass:
        case DerivedClass:
			if( filters == NONE )
				filters = AllFNCs|AllData;

			int i;
			CString szButtonAccel = "%";
			szButtonAccel += ExtractHotKey(GetLabel(IDM_CTL_BROWSE_DATAMEM));
			
			szButtonAccel += "{DOWN}{HOME}";

			MST.DoKeys( szButtonAccel.GetBuffer(szButtonAccel.GetLength()));
			for( i = 1; i < filters%AllData; i++ )
				MST.DoKeys( "{DOWN}" );

			szButtonAccel = "{TAB}+{TAB}%";
			szButtonAccel += ExtractHotKey(GetLabel(IDM_CTL_BROWSE_FUNCMEM));

			szButtonAccel += "{DOWN}{HOME}";

			MST.DoKeys(szButtonAccel.GetBuffer(szButtonAccel.GetLength()));
			for( i = 1; i < filters/AllData; i++ )
				MST.DoKeys( "{DOWN}" );

			// restore cursor in original pane
			MST.DoKeys( "{TAB}+{TAB}" );

			m_filters = filters;
            return TRUE;
            break;
    }
    return FALSE;
};


// BEGIN_HELP_COMMENT
// Function: int  UIBrowse::GetFilters(QueryType type)
// Description: Get the filters for the current query (of the type given).
// Return: A bit field that specifies the type of filter: NONE, CLASSES, FUNCTIONS, DATA, MACROS, TYPES.
// Param: type A QueryType value that indicates the type of query: DefRef, FileOutline, BaseClass, DerivedClass, CallGraph, CallerGraph.
// END_HELP_COMMENT
int  UIBrowse::GetFilters(QueryType type) {         		// get filters based on current type
	if( m_status != Active ) {
		LOG->RecordInfo( "UIBrowse::SetFilters -  m_status != Active" );
		return FALSE;
	}

	
    EXPECT( Activate() );

    switch (type)
    {
        case FileOutline:
            return ( (CLASSES   * IsButtonDown(GetLabel(IDM_CTL_BROWSE_CLASS)))  +  \
                     (FUNCTIONS * IsButtonDown(GetLabel(IDM_CTL_BROWSE_FUNCMEM)))+  \
                     (DATA      * IsButtonDown(GetLabel(IDM_CTL_BROWSE_DATAMEM)))     +  \
                     (MACROS    * IsButtonDown(GetLabel(IDM_CTL_BROWSE_MACRO)))   +  \
                     (TYPES     * IsButtonDown(GetLabel(IDM_CTL_BROWSE_TYPE)))      );
            break;
        case BaseClass:
        case DerivedClass:
            // {AllFNCs=1,Virtual, Static,NonVirtual, NonStatic,NSNV,NoFCNs, AllData=101,StaticData,NonStaticData,NoData};
            // assume enum exactly matches combo boxes !!!!!
            return ( MST.WComboIndex(GetLabel(VCPP32_IDC_COMBO_FUNS)) + (100*MST.WComboIndex(GetLabel(VCPP32_IDC_COMBO_DATA))));
            // attributeFilterType{NONE=0, Static = 1, NonStatic = 2, Virtual = 4, NonVirtual = 8 };
            return TRUE;
            break;
    }
    return 0;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::PopContext(void)
// Description: Perform the Pop Context browser operation.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::PopContext(void) {
    CString Keys = "^{NUMPAD*}";
    MST.DoKeys(Keys);
    return TRUE;
};


// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::FirstDefinition(void)
// Description: Perform the Go To Definition browser operation.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::FirstDefinition(void) {
    CString Keys = "{F11}";
    MST.DoKeys(Keys);
    return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::NextDefinition(void)
// Description: Perform the Go To Next Definition browser operation.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::NextDefinition(void) {
    CString Keys = "^{NUMPAD+}";
    MST.DoKeys(Keys);
    return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::PrevDefinition(void)
// Description: Perform the Go To Previous Definition browser operation.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::PrevDefinition(void) {
    CString Keys = "^{NUMPAD-}";
    MST.DoKeys(Keys);
    return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::FirstReference(void)
// Description: Perform the Go To Reference browser operation.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::FirstReference(void) {
    CString Keys = "+{F11}";
    MST.DoKeys(Keys);
    return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::NextReference(void)
// Description: Perform the Go To Next Reference browser operation.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::NextReference(void) {
    CString Keys = "^{NUMPAD+}";
    MST.DoKeys(Keys);
    return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::PrevReference(void)
// Description: Perform the Go To Previous Reference browser operation.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::PrevReference(void) {
    CString Keys = "^{NUMPAD-}";
    MST.DoKeys(Keys);
    return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::GotoPane(PaneType pane, QueryType type)
// Description: Set focus to the specified pane in the browser window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: pane A PaneType value that specifies which pane to set focus to: SYMBOL_PANE, MEMBER_PANE, DEFREF_PANE.
// Param: type A QueryType value that indicates the type of query: DefRef, FileOutline, BaseClass, DerivedClass, CallGraph, CallerGraph.
// END_HELP_COMMENT
BOOL UIBrowse::GotoPane(PaneType pane, QueryType type) {
    Activate();
    if ((type == BaseClass) || (type == DerivedClass))  // three panes
    {
        char buffer[10];
        CString count;
        //assumes panes SYMBOL_PANE MEMBER_PANE DEFREF_PANE numbered 1-3 respctively
        if (pane >= m_pane)
            count=itoa((pane-m_pane), buffer, 10);
        else
            count=itoa(((pane+3)-m_pane), buffer, 10);
        CString Keys = "{TAB "+count+"}";
        MST.DoKeys(Keys);
    }
    else                                                // two panes
    {
      if (m_pane != pane ) MST.DoKeys("{TAB}");
    }
  return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::GotoDefinitionsHeading(void)
// Description: Go to the definitions heading in definitions/references pane in the browser window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::GotoDefinitionsHeading(void) {
    Activate();
    CString Heading=GetLocString(IDSS_BRS_DEFINITIONS);
    CString Keys = "{ENTER}";
    MST.DoKeys(Keys);
    while (GetCurrentItem()!=Heading)
          SelectNthItem(DefRef,1);
    return (GetCurrentItem()==Heading);
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::GotoReferencesHeading(void)
// Description: Go to the references heading in the definitions/references pane in the browser window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::GotoReferencesHeading(void) {
    Activate();
    CString Heading=GetLocString(IDSS_BRS_REFERENCES);
    CString Keys = "{ENTER}";
    MST.DoKeys(Keys);
    while (GetCurrentItem()!=Heading)
          SelectNthItem(DefRef,1);
    return (GetCurrentItem()==Heading);
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::ExpandCurrentItem(void)
// Description: Expand the selected item in the browser window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::ExpandCurrentItem(void) {
    CString Keys = "{ENTER}";
    MST.DoKeys(Keys);
    return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: CString UIBrowse::GetCurrentItem(void)
// Description: Get a string that describes the currently selected item in the browser window. This function is NYI.
// Return: A CString that contains the description of the currently selected item.
// END_HELP_COMMENT
CString UIBrowse::GetCurrentItem(void) {
    Activate();
    //todo call something to get the current item !!!!!!!!!
    return "";
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::Navigate(void)
// Description: Navigate from the currently selected item in the browser window to its location in the source code.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::Navigate(void) {
    CString Keys = "{ENTER}";
    MST.DoKeys(Keys);
    return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::SelectNthItem(QueryType type, int item)
// Description: Select the n-th item in the current pane in the browser window. Expandable nodes will be expanded by this function.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: type A QueryType value that indicates the type of query: DefRef, FileOutline, BaseClass, DerivedClass, CallGraph, CallerGraph.
// Param: item An integer that contains the 0-based index of the item to select.
// END_HELP_COMMENT
BOOL UIBrowse::SelectNthItem(QueryType type, int item) {
    Activate();
    for (int temp = 0 ; temp < item; temp++)
    {
         // only symbol pand expandable, also defref, fileoutline not expandable (goto's instaed!)
         if ((m_pane==SYMBOL_PANE) && !((type==DefRef) || (type==FileOutline)))
               ExpandCurrentItem();              //expand as we go
         CString Keys = "{DOWN}";                //move down one
         MST.DoKeys(Keys);
    }
    return TRUE;
};


// Cracks notation returned from GetNthDefinition/reference
// ie "f:\cafe\cafe\wb\cobrowse.h(33)"
// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::CrackFileLine(LPCSTR file_line,CHAR *file,int *line)
// Description: Parse the notation returned from GetNthDefinition/Reference (i.e. f:\cafe\cafe\wb\cobrowse.h(33)).
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: file_line A pointer to a string that contains the file+line string to crack.
// Param: file A pointer to a character buffer that will contain the file part of the file_line string.
// Param: line A pointer to an integer buffer that will contain the line part of the file_line string.
// END_HELP_COMMENT
BOOL UIBrowse::CrackFileLine(LPCSTR file_line,CHAR *file,int *line) {
    CString fileline=file_line;
    int start_num = fileline.ReverseFind('(');
    int end_num =   fileline.ReverseFind(')');
    
    EXPECT( start_num != -1 );
    EXPECT( end_num != -1 );

    //
    //  Number starts after '('
    //
    start_num++;
    
    EXPECT( end_num > start_num );
    
    CString szline= fileline.Mid(start_num, end_num - start_num );
    CString strfile=fileline.Left(fileline.ReverseFind('('));

    *line=atoi(szline);
    strcpy(file, strfile.GetBuffer(strfile.GetLength()));
    return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: void UIBrowse::ButtonDown(const char *button)
// Description: Set the specified button to its down state.
// Return: none
// Param: button A pointer to a string that contains the name of the button.
// END_HELP_COMMENT
void UIBrowse::ButtonDown(const char *button) {
    if (!IsButtonDown(button))
        MST.WButtonClick(button);
};

// BEGIN_HELP_COMMENT
// Function: void UIBrowse::ButtonUp(const char *button)
// Description: Set the specified button to its up state.
// Return: none
// Param: button A pointer to a string that contains the name of the button.
// END_HELP_COMMENT
void UIBrowse::ButtonUp(const char *button) {
    if (IsButtonDown(button))
        MST.WButtonClick(button);
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::IsButtonDown(const char *button)
// Description: Determine if the specified button is in its down state.
// Return: A Boolean value that indicates whether the button is in its down state (TRUE) or not.
// Param: button A pointer to a string that contains the name of the button.
// END_HELP_COMMENT
BOOL UIBrowse::IsButtonDown(const char *button) {

    EXPECT(MST.WButtonEnabled(button));
    //todo determine if down
    return MST.WOptionState(szPushPinName) == 1;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::CloseBrowser(void)
// Description: Close the browser window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::CloseBrowser(void) {
	Activate();
	// assume that button is down.
	MST.WCheckClick( szPushPinName );
	MST.DoKeys("{ESC}");
	if( !WaitUntilGone( 1000 ) ) {
		// try again
		Activate();
		MST.WButtonClick( szPushPinName );
		MST.DoKeys("{ESC}");
	}
    return TRUE;
};

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::GetMatches( CStringArray& matches )
// Description: Get the list of symbols that match the symbol name specified in the last query from the symbols pane (the left pane) in the browser window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: matches A CStringArray reference that will contain the list of symbols.
// END_HELP_COMMENT
BOOL UIBrowse::GetMatches( CStringArray& matches ) {
	// Assume that left pane has focus.  True if window just opened.

	matches.RemoveAll();

	if( m_status != Active ) {
		LOG->RecordInfo( "UIBrowse::GetMatches -  m_status != Active" );
		return FALSE;
	}
	EXPECT( Activate() );

	if( m_type == FileOutline || m_type == DefRef ) {
		MST.DoKeys("+(^c)");
	}
	else {
		// Only select top item for all other queries.
		MST.DoKeys("^{HOME}");
		MST.DoKeys("^c");
	}

	return FillStringArrayFromClipBrd( matches );
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::SelectMatch( CString name )
// Description: Select the specified match by name in the symbols pane (the left pane) in the browser window. (See SelectMatch(int) to select a symbol by index.)
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: name A CString that contains the name of the match to select.
// END_HELP_COMMENT
BOOL UIBrowse::SelectMatch(CString name)

	{
	CString dokeys;
	char str[8];

	int i = FindMatch(name);

	if(i > -1)
		{
		dokeys = (CString)"{HOME}{DOWN " + _itoa(i, str, 10) + "}";
		MST.DoKeys(dokeys);
		return TRUE;
		}

	LOG->RecordInfo( "UIBrowse::SelectMatch - could not find %s", name);
	return FALSE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::SelectMatch( int index )
// Description: Select the specified match by index in the symbols pane (the left pane) in the browser window. (See SelectMatch(CString) to select a symbol by name.) This function is NYI.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: index An integer that contains the 1-based index into the list of symbols.
// END_HELP_COMMENT
BOOL UIBrowse::SelectMatch( int index ) {
	LOG->RecordInfo( "UIBrowse::SelectMatch - Not Implemented" );
	return FALSE;
}


// BEGIN_HELP_COMMENT
// Function: int FindMatch(CStringArray& matches, CString name)
// Description: returns the index of the specified match in the list of matches in the symbol pane of the browser wnd.
// Return: the zero-based index of the match if successful, -1 if not successful.
// Param: name A CString that contains the name of the match to search for.
// END_HELP_COMMENT
int UIBrowse::FindMatch(CString name)

	{
	CStringArray matches;

	if(!GetMatches(matches))
		{
		LOG->RecordInfo( "UIBrowse::FindMatch - could not get matches");
		return FALSE;
		}

	for(int i = matches.GetSize() - 1; i > -1; i--)
		{
		if(matches[i] == name)
			break;
		}

	return i;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::GetMembers( CStringArray& members )
// Description: Get the list of members for the last query performed in the browser window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: members A CStringArray reference that will contain the list of members.
// END_HELP_COMMENT
BOOL UIBrowse::GetMembers( CStringArray& members ) {

	// Assume that left pane has focus.  True if window just opened.
	members.RemoveAll();

	if( m_status != Active ) {
		LOG->RecordInfo( "UIBrowse::GetMembers -  m_status != Active" );
		return FALSE;
	}
	EXPECT( Activate() );

	if( m_type == BaseClass || m_type == DerivedClass ) {
		MST.DoKeys("{TAB}");
		MST.DoKeys("+(^c)");
		return FillStringArrayFromClipBrd( members );
	}
	else {
		LOG->RecordInfo( "UIBrowse::GetMembers - Query is not class tree" );
		return FALSE;
	}
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::SelectMember( CString member )
// Description: Select the specified member by name in the member pane (the upper right pane) in the browser window. (See SelectMember(int) to select a member by index.) This function is NYI.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: member A CString that contains the name of the member to select.
// END_HELP_COMMENT
BOOL UIBrowse::SelectMember( CString member ) {
	LOG->RecordInfo( "UIBrowse::SelectMember - Not Implemented" );
	return FALSE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::SelectMember( int index )
// Description: Select the specified member by index in the member pane (the upper right pane) in the browser window. (See SelectMember(CString) to select a member by name.) This function is NYI.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: index An integer that contains the 1-based index of the member to select.
// END_HELP_COMMENT
BOOL UIBrowse::SelectMember( int index ) {
	LOG->RecordInfo( "UIBrowse::SelectMember - Not Implemented" );
	return FALSE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::GetLocations( CStringArray& locations )
// Description: Get the list of definitions and references for the selected symbol in last query performed in the browser window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: locations A CStringArray reference that will contain the list of defs/refs.
// END_HELP_COMMENT
BOOL UIBrowse::GetLocations( CStringArray& locations ) {

	// Assume that left pane has focus. True if window just opened.
	locations.RemoveAll();

	if( m_status != Active ) {
		LOG->RecordInfo( "UIBrowse::GetLocations -  m_status != Active" );
		return FALSE;
	}
	EXPECT( Activate() );

	if( m_type == BaseClass || m_type == DerivedClass ) {
		MST.DoKeys("{TAB 2}");
		MST.DoKeys("+(^c)");
	}
	else {
		MST.DoKeys("{TAB}");
		MST.DoKeys("+(^c)");
	}

	MST.DoKeys("{TAB}");
	return FillStringArrayFromClipBrd( locations );
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::GetFirstLevelBranches( CStringArray& branches )
// Description: Get the list of first level branches for the last query performed in the browser window.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: branches A CStringArray reference that will contain the list of first level branches.
// END_HELP_COMMENT
BOOL UIBrowse::GetFirstLevelBranches( CStringArray& branches ) {

	// Assume that left pane has focus. True if window just opened.
	branches.RemoveAll();

	if( m_status != Active ) {
		LOG->RecordInfo( "UIBrowse::GetFirstLevelBranches -  m_status != Active" );
		return FALSE;
	}
	EXPECT( Activate() );

	if( m_type == FileOutline || m_type == DefRef ) {
		LOG->RecordInfo( "UIBrowse::GetFirstLevelBranches -  Not for file outline or def & ref queries" );
		return FALSE;
	}
	else {
		// toggle top item and get branches, if none, toggle again and get branches
		//		This makes sure that only one level of branches appear
		MST.DoKeys("^{HOME}{ENTER}");
		MST.DoKeys("+(^c)");
		if( FillStringArrayFromClipBrd( branches ) ) {
			if( branches.GetSize() == 1 ) {
				// toggle
				MST.DoKeys("{ENTER}");
				MST.DoKeys("+(^c)");
				// worked once, expect it to work again
				EXPECT( FillStringArrayFromClipBrd( branches ) );
			}
			if( branches.GetSize() > 0 ) {
				// remove parent, then tabs in front of children
				branches.RemoveAt( 0 );
				for( int index=0; index < branches.GetSize(); index++ ) {
					EXPECT( branches[index][0] == '\t' );
					branches[index] = branches[index].Mid( 1 );
				}
				return TRUE;
			}
		}
		return FALSE;
	}

	return FillStringArrayFromClipBrd( branches );
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::SelectDefinition( CString definition )
// Description: Select the specified definition by name in the definitions/references pane (the lower right pane) in the browser window. (See SelectDefinition(int) to select a definition by index.) This function is NYI.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: definition A CString that contains the name of the definition to select.
// END_HELP_COMMENT
BOOL UIBrowse::SelectDefinition( CString definition ) {
	LOG->RecordInfo( "UIBrowse::SelectDefinition - Not Implemented" );
	return FALSE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::SelectDefinition( int index )
// Description: Select the specified definition by index in the definitions/references pane (the lower right pane) in the browser window. (See SelectDefinition(CString) to select a definition by name.) This function is NYI.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: index An integer that contains the 1-based index of the definition to select.
// END_HELP_COMMENT
BOOL UIBrowse::SelectDefinition( int index ) {
	LOG->RecordInfo( "UIBrowse::SelectDefinition - Not Implemented" );
	return FALSE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::SelectReference( CString reference )
// Description: Select the specified reference by name in the definitions/references pane (the lower right pane) in the browser window. (See SelectReference(int) to select a reference by index.) This function is NYI.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: reference A CString that contains the name of the reference to select.
// END_HELP_COMMENT
BOOL UIBrowse::SelectReference( CString reference ) {
	LOG->RecordInfo( "UIBrowse::SelectReference - Not Implemented" );
	return FALSE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::SelectReference( int index )
// Description: Select the specified reference by index in the definitions/references pane (the lower right pane) in the browser window. (See SelectReference(CString) to select a reference by name.) This function is NYI.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: index An integer that contains the 1-based index of the reference to select.
// END_HELP_COMMENT
BOOL UIBrowse::SelectReference( int index ) {
	LOG->RecordInfo( "UIBrowse::SelectReference - Not Implemented" );
	return FALSE;
}


//
// UIBrowse interface to ambiguity dialog
//
// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::GetAmbiguities( CStringArray& ambiguities )
// Description: Get the list of ambiguities from the Resolve Ambiguities dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: ambiguities A reference to a CStringArray that will contain the list of ambiguities.
// END_HELP_COMMENT
BOOL UIBrowse::GetAmbiguities( CStringArray& ambiguities ) {
	if( m_status != AmbiguityDialog ) {
		LOG->RecordInfo( "UIBrowse::GetAmbiguities -  m_status != AmbiguityDialog" );
		return FALSE;
	}

	UIAmbiguiousSymbolDialog dlgAmb;
	EXPECT( dlgAmb.AttachActive() );
	return dlgAmb.GetAmbiguities( ambiguities );
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::GetCurrentAmbiguity( CString& symbol )
// Description: Get the currently selected ambiguity from the Resolve Ambiguities dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: symbol A reference to a CString that will contain the name of the currently selected ambiguity.
// END_HELP_COMMENT
BOOL UIBrowse::GetCurrentAmbiguity( CString& symbol ) {
	if( m_status != AmbiguityDialog ) {
		LOG->RecordInfo( "UIBrowse::GetCurrentAmbiguity -  m_status != AmbiguityDialog" );
		return FALSE;
	}

	UIAmbiguiousSymbolDialog dlgAmb;
	EXPECT( dlgAmb.AttachActive() );
	return dlgAmb.GetCurrentAmbiguity( symbol );
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::SelectAmbiguity( CString symbol )
// Description: Select the specified ambiguity by name in the Resolve Ambiguities dialog. (See SelectAmbiguity(int) to select an ambiguity by index.)
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: symbol A CString that contains the name of the ambiguity to select.
// END_HELP_COMMENT
BOOL UIBrowse::SelectAmbiguity( CString symbol ) {
	if( m_status != AmbiguityDialog ) {
		LOG->RecordInfo( "UIBrowse::SelectAmbiguity -  m_status != AmbiguityDialog" );
		m_status = NotActive;
		return FALSE;
	}

	UIAmbiguiousSymbolDialog dlgAmb;
	EXPECT( dlgAmb.AttachActive() );

	if( dlgAmb.SelectAmbiguity( symbol ) ) {
		AttachActive();
		m_status = Active;
		return TRUE;
	}
	else {
		dlgAmb.Cancel();
		m_status = NotActive;
		return FALSE;
	}
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::SelectAmbiguity( int index )
// Description: Select the specified ambiguity by index in the Resolve Ambiguities dialog. (See SelectAmbiguity(CString) to select an ambiguity by name.)
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: index An integer that contains the 1-based index of the ambiguity to select.
// END_HELP_COMMENT
BOOL UIBrowse::SelectAmbiguity( int index ) {
	if( m_status != AmbiguityDialog ) {
		LOG->RecordInfo( "UIBrowse::SelectAmbiguity -  m_status != AmbiguityDialog" );
		return FALSE;
	}

	UIAmbiguiousSymbolDialog dlgAmb;
	EXPECT( dlgAmb.AttachActive() );

	if( dlgAmb.SelectAmbiguity( index ) ) {
		AttachActive();
		m_status = Active;
		return TRUE;
	}
	else {
		dlgAmb.Cancel();
		m_status = NotActive;
		return FALSE;
	}
}

// BEGIN_HELP_COMMENT
// Function: BOOL UIBrowse::CancelAmbiguity()
// Description: Cancel the query by cancelling the Resolve Ambiguities dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::CancelAmbiguity() {
	if( m_status != AmbiguityDialog ) {
		LOG->RecordInfo( "UIBrowse::CancelAmbiguity -  m_status != AmbiguityDialog" );
		return FALSE;
	}

	UIAmbiguiousSymbolDialog dlgAmb;
	EXPECT( dlgAmb.AttachActive() );

	return dlgAmb.Cancel() == NULL;
}

UIAmbiguiousSymbolDialog::UIAmbiguiousSymbolDialog() : 
	UIDialog( GetLocString( IDSS_RESOLVE_AMBIGUITY_TITLE ) ) {
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIAmbiguiousSymbolDialog::GetAmbiguities( CStringArray& ambiguities )
// Description: Get the list of ambiguities from the Resolve Ambiguities dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: ambiguities A reference to a CStringArray that will contain the list of ambiguities.
// END_HELP_COMMENT
BOOL UIAmbiguiousSymbolDialog::GetAmbiguities( CStringArray& ambiguities ) {

	// Clear CStringArray
	ambiguities.RemoveAll();

	EXPECT( IsValid() );

	// Make sure dialog exists and symbol listbox has focus.
	int cListCount = MST.WListCount( GetLabel( VCPP32_ID_AMB_LIST ) );

	if( cListCount < 0 ) { // Should check for -1 but any negative number is bad
		LOG->RecordInfo( "UIAmbiguiousSymbolDialog::GetAmbiguities - Couldn't find listbox" );
		return FALSE;
	}
	else if( cListCount < 2 ) {
		LOG->RecordInfo( "UIAmbiguiousSymbolDialog::GetAmbiguities - Count == %d doesn't make sense", cListCount );
	}

	for ( int i = 0; i < cListCount; i++ ) {
		CString item;
		CMSTest::WListItemText( GetLabel( VCPP32_ID_AMB_LIST ), i+1, item );
		ambiguities.Add( item );
	}

	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIAmbiguiousSymbolDialog::GetCurrentAmbiguity( CString& symbol )
// Description: Get the currently selected ambiguity from the Resolve Ambiguities dialog.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: symbol A reference to a CString that will contain the name of the currently selected ambiguity.
// END_HELP_COMMENT
BOOL UIAmbiguiousSymbolDialog::GetCurrentAmbiguity( CString& symbol ) {
	EXPECT( IsValid() );
	CMSTest::WListText( GetLabel( VCPP32_ID_AMB_LIST ), symbol );
	return TRUE;
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIAmbiguiousSymbolDialog::SelectAmbiguity( CString symbol )
// Description: Select the specified ambiguity by name in the Resolve Ambiguities dialog. (See SelectAmbiguity(int) to select an ambiguity by index.)
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: symbol A CString that contains the name of the ambiguity to select.
// END_HELP_COMMENT
BOOL UIAmbiguiousSymbolDialog::SelectAmbiguity( CString symbol ) {
	EXPECT( IsValid() );
	CMSTest::WListItemDblClk( GetLabel( VCPP32_ID_AMB_LIST ), symbol );
	return WaitUntilGone( 3000 );
}


// BEGIN_HELP_COMMENT
// Function: BOOL UIAmbiguiousSymbolDialog::SelectAmbiguity( int index )
// Description: Select the specified ambiguity by index in the Resolve Ambiguities dialog. (See SelectAmbiguity(CString) to select an ambiguity by name.)
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: index An integer that contains the 1-based index of the ambiguity to select.
// END_HELP_COMMENT
BOOL UIAmbiguiousSymbolDialog::SelectAmbiguity( int index ) {
	EXPECT( IsValid() );
	CMSTest::WListItemDblClk( GetLabel( VCPP32_ID_AMB_LIST ), index+1 );
	return WaitUntilGone( 3000 );
}



// BEGIN_HELP_COMMENT
// Function: HWND UIAmbiguiousSymbolDialog::Cancel(void)
// Description: Close the Resolve Ambiguities dialog by clicking the Cancel button.
// Return: NULL if successful; the HWND of any remaining messsage or dialog box otherwise.
// END_HELP_COMMENT
HWND UIAmbiguiousSymbolDialog::Cancel(void)
{
	ExpectValid();

	MST.WButtonClick(GetLabel(VCPP32_ID_AMB_CANCEL));
	if( WaitUntilGone(1000) )
		return NULL;		// NULL means all is well
	else
		return MST.WGetActWnd(0);	// an HWND presumes some alert came up
}


// BEGIN_HELP_COMMENT
// Function: BOOL ToggleCaseSensitivity(TOGGLE_TYPE t)
// Description: Toggle the case-sensitivity of browser queries.
// Param: t an enum type with value TOGGLE_OFF or TOGGLE_ON.
// Return: TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL UIBrowse::ToggleCaseSensitivity(TOGGLE_TYPE t)
	
	{
	if(t)
		MST.WCheckCheck("@1");
	else
		MST.WCheckUnCheck("@1");

	return TRUE;
	}