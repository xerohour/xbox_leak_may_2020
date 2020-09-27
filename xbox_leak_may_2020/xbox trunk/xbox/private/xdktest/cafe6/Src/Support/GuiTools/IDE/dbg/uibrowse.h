///////////////////////////////////////////////////////////////////////////////
//  UIBROWSE.H
//
//  Created by :            Date :
//      WayneBr             1/21/94
//
//  Description :
//      Declaration of the UIBrowse class
//

#ifndef __UIBROWSE_H__
#define __UIBROWSE_H__

#include "..\..\udialog.h"
#include "..\..\testutil.h"

#include "dbgxprt.h"

///////////////////////////////////////////////////////////////////////////////
//  UIBrowse symbols

//filters for FILTER BIT MASK
#define NONE 0
enum bmfFilterType {CLASSES=1,FUNCTIONS=2,DATA=4,MACROS=8,TYPES=16};
enum attributeFilterType {AllFNCs=1,Virtual, Static,NonVirtual, NonStatic,NSNV,NoFCNs, AllData=16,StaticData=32,NonStaticData=48,NoData=64};

// Query types
enum QueryType {DefRef=1, FileOutline, BaseClass, DerivedClass, CallGraph, CallerGraph};

// Pane types
enum PaneType {SYMBOL_PANE=1, MEMBER_PANE, DEFREF_PANE};

///////////////////////////////////////////////////////////////////////////////
//  UIBrowse class : public UIWindow

// BEGIN_CLASS_HELP
// ClassName: UIBrowse
// BaseClass: UIWindow
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UIBrowse : public UIWindow {
public:
	UIWND_COPY_CTOR(UIBrowse, UIWindow);

	virtual BOOL Find(void);

	// Query status
	enum QueryStatus {
		NotActive = 0,
		AmbiguityDialog,
		FileNotFound,
		SymbolNotFound,
		Active,
	};

private:
	// Data
	PaneType m_pane;
	HWND m_hWndPushPin;
	QueryType m_type;
	QueryStatus m_status;
	int m_filters;
	CString m_name;


// UIWindow overrides
public:
	virtual BOOL IsValid(void) const;
	virtual BOOL IsActive(void);    // derived classes must provide their own Activate (usually a DoCommand)


// Utilities
public:
	// Note: There may be a not found or resolve ambiguity dialog if Query returns FALSE
	BOOL Query(QueryType query, LPCSTR name=NULL, int filters=NONE, int case_sensitive = TRUE);
	BOOL SetFilters(QueryType type, int filters);          // set filters based on current type
	int  GetFilters(QueryType type);                       // get filters based on current type

	QueryStatus GetStatus() const { return m_status; }

	BOOL GetMatches( CStringArray& matches );
	BOOL SelectMatch( CString name );
	BOOL SelectMatch( int index );
	int FindMatch(CString name);

	// only applies to class queries ( usually located in upper-right pane )
	BOOL GetMembers( CStringArray& members );
	BOOL SelectMember( CString member );
	BOOL SelectMember( int index );

	// definitions for current match ( usually located in right or lower-right pane )
	BOOL GetLocations( CStringArray& locations );
	BOOL SelectDefinition( CString definition );
	BOOL SelectDefinition( int index );

	// references for current match ( usually located directly below definitions in same pane )
	BOOL SelectReference( CString reference );
	BOOL SelectReference( int index );

	// get first level of descendents or ancestors in tree.
	BOOL GetFirstLevelBranches( CStringArray& branches );

	BOOL PopContext(void);

	BOOL FirstDefinition(void);
	BOOL NextDefinition(void);
	BOOL PrevDefinition(void);

	BOOL FirstReference(void);
	BOOL NextReference(void);
	BOOL PrevReference(void);


	BOOL GotoPane(PaneType pane, QueryType type);
	BOOL GotoDefinitionsHeading(void);
	BOOL GotoReferencesHeading(void);
	BOOL SelectNthItem(QueryType type, int item);
	BOOL Navigate(void);
	BOOL ExpandCurrentItem(void);
	CString GetCurrentItem(void);

	BOOL CrackFileLine(LPCSTR file_line,CHAR *file,int *line);
	void ButtonDown(const char * button);
	void ButtonUp(const char * button);
	BOOL IsButtonDown(const char * button);
	BOOL CloseBrowser(void);

	// interface for ambigious dialog ( only valid when m_status == AmbiguityDialog )
	BOOL GetAmbiguities( CStringArray& ambiguities );
	BOOL GetCurrentAmbiguity( CString& symbol );
	BOOL SelectAmbiguity( CString symbol );
	BOOL SelectAmbiguity( int index );  // only needed if 
	BOOL CancelAmbiguity();

	BOOL ToggleCaseSensitivity(TOGGLE_TYPE t);

protected:
	virtual void OnUpdate(void);
};

// class UIAmbiguiousSymbolDialog 

// BEGIN_CLASS_HELP
// ClassName: UIAmbiguiousSymbolDialog
// BaseClass: UIDialog
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UIAmbiguiousSymbolDialog : public UIDialog {
public:
    virtual HWND Cancel(void);

	UIAmbiguiousSymbolDialog();
	UIAmbiguiousSymbolDialog(HWND hwnd) : UIDialog(hwnd) {
		UIAmbiguiousSymbolDialog::OnUpdate();
	}
	HWND operator=(HWND hwnd) {
		UIDialog::operator=(hwnd);
		/*UIAmbiguiousSymbolDialog::OnUpdate();*/ 
		return hwnd;
	}

	BOOL GetAmbiguities( CStringArray& ambiguities );
	BOOL GetCurrentAmbiguity( CString& symbol );
	BOOL SelectAmbiguity( CString symbol );
	BOOL SelectAmbiguity( int index );  // only needed if 
};

#endif
