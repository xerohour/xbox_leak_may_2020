/*===========================================================================*\
 |    File: CUITest.cpp
 |
 | Purpose: To test the CUI Toolbar APIs.  This program adds a toolbar
 |          to MAX when BeginEditParams() is called and hides it when 
 |          EndEditParams() is called.
 |
 |          Important Notes:
 |          o This file requires four image files be placed in the MAX EXE\UI 
 |            directory.  These are:
 |             CUITest_16a.bmp
 |             CUITest_16i.bmp
 |             CUITest_24a.bmp
 |             CUITest_24i.bmp
 |
 |          o It also requires CUITest.mcr to be put into EXE\UI as well.
 |
 |			NOTE: The above files can be found in the CONTENT directory, off the
 |					root directory of this project.
 |
 |
 | History: Mark Meier, 03/05/99, Began.
 |          MM, 03/14/99, Last Change.
 |			Harry Denholm, 03/28/99, Prep'd for SDK inclusion
 |
\*===========================================================================*/

// Please see the following header file for class information
#include "CUITest.h"



/*===========================================================================*\
 | Class Descriptor for the CUITest plugin
\*===========================================================================*/

class CUITestClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return &theCUITest;}
	const TCHAR *	ClassName() {return MY_CLASSNAME;}
	SClass_ID		SuperClassID() {return UTILITY_CLASS_ID;}
	Class_ID		ClassID() {return MY_CLASS_ID;}
	const TCHAR* 	Category() {return MY_CATEGORY;}
};
static CUITestClassDesc CUITestDesc;
ClassDesc* GetCUITestDesc() {return &CUITestDesc;}



/*===========================================================================*\
 | This class is the custom message handler installed by the method
 | ICUIFrame::InstallMsgHandler(tbMsgHandler).  It has one method,
 | ProcessMessage() which, uh, processes the messages.
\*===========================================================================*/

class TBMsgHandler : public CUIFrameMsgHandler {
	CUITest *ct;
  public:
	TBMsgHandler(CUITest *ctst)	{ this->ct = ctst; }
	int ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
};	
static TBMsgHandler *tbMsgHandler;


int TBMsgHandler::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_TB_0: // Dock or Float
			if (ct->iFrame->IsFloating()) {
				GetCUIFrameMgr()->DockCUIWindow(ct->hWnd, CUI_TOP_DOCK, NULL);
				GetCUIFrameMgr()->RecalcLayout(TRUE);
			}
			else {
				GetCUIFrameMgr()->FloatCUIWindow(ct->hWnd, NULL);
				GetCUIFrameMgr()->RecalcLayout(TRUE);
			}
			return TRUE;
		case ID_TB_1: // This is the check button ... ignore it.
			return TRUE;
		case ID_TB_2:
			MessageBox(NULL, _T("Pressed 2"), _T("ID_TB_2"), MB_OK);
			return TRUE;
		default: // ID not recognized -- use default CUI processing
			return FALSE;
		}
	}
	return FALSE;
}




/*===========================================================================*\
 | The Begin/EndEditParams calls, which create and destroy the toolbar
\*===========================================================================*/

void CUITest::BeginEditParams(Interface *ip,IUtil *iu) {
	this->iu = iu;
	this->ip = ip;

	if (iFrame) {
		// We have the toolbar already, just show it...(EndEditParams() hid it)
		iFrame->Hide(FALSE);
		// If the frame is floating there is no reason to 
		// recalc the layout, but if it's docked we need to do so
		if (!iFrame->IsFloating()) {
			GetCUIFrameMgr()->RecalcLayout(TRUE);
		}
	}
	else {
		// Create a simple toolbar
		// -- First create the frame
		HWND hParent = ip->GetMAXHWnd();
		hWnd = CreateCUIFrameWindow(hParent, _T("CUI Test Toolbar"), 0, 0, 250, 100);
		// iFrame->SetName();
		iFrame = GetICUIFrame(hWnd);
		iFrame->SetContentType(CUI_TOOLBAR);
		iFrame->SetPosType(CUI_HORIZ_DOCK | CUI_VERT_DOCK | CUI_FLOATABLE | CUI_SM_HANDLES);

		// -- Now create the toolbar window
		HWND hToolbar = CreateWindow(
				CUSTTOOLBARWINDOWCLASS,
				NULL,
				WS_CHILD | WS_VISIBLE,
				0, 0, 250, 100,
				hWnd,
				NULL,
				hInstance,
				NULL);
		// -- Now link the toolbar to the CUI frame
		ICustToolbar *iToolBar = GetICustToolbar(hToolbar);
		iToolBar->LinkToCUIFrame(hWnd, NULL);
		iToolBar->SetBottomBorder(FALSE);
		iToolBar->SetTopBorder(FALSE);

		// Install the message handler to process the controls we'll add...
		tbMsgHandler = new TBMsgHandler(this);
		iFrame->InstallMsgHandler(tbMsgHandler);

		// -- Toss in a few controls of various sorts...
		iToolBar->SetImage(GetCUIFrameMgr()->GetDefaultImageList());
		// Get the index into the entire image list of the CUITest icons.
		int iconIndex = GetCUIFrameMgr()->GetGroupStartIndex(_T("CUITest"));
		// Add a push button
		// This one docks and undocks the toolbar if clicked...
		iToolBar->AddTool(TBITEM(CTB_PUSHBUTTON, iconIndex, ID_TB_0));

		// Add a separator
		iToolBar->AddTool(ToolSeparatorItem(8));

		// Add a check button
		// This one doesn't do anything except change to GREEN when on...
		iToolBar->AddTool(TBITEM(CTB_CHECKBUTTON, iconIndex+1, ID_TB_1));
		ICustButton* iBtn;
		(iBtn = iToolBar->GetICustButton(ID_TB_1))->SetHighlightColor(GREEN_WASH);
		ReleaseICustButton(iBtn);

		// Add a separator
		iToolBar->AddTool(ToolSeparatorItem(8));

		// Add a keyboard macro command.  In this case the Time Configuration dialog
		// is presented as an example.  In order to get the proper command ID to use
		// we must search through the names in the MAX UI shortcut table and find
		// a match.  The list of names can be reviewed in the MAX UI when setting up
		// a keyboard shortcut.
		TSTR findName = _T("Time Configuration");
		for (int i = 0; i < ip->GetShortcutCount(kShortcutMainUI); i++) {
			if (_tcscmp(findName.data(), ip->GetShortcutCommandName(kShortcutMainUI, i)) == 0) {
				int cmdID = ip->GetShortcutCommandId(kShortcutMainUI, i);
				MacroButtonData md1(kShortcutMainUI, NULL, cmdID, _T("KBD Cmd"), _T("Key Macro Tooltip"));
				iToolBar->AddTool(TBMACRO(&md1));
				break;
			}
		}

		// Add a separator
		iToolBar->AddTool(ToolSeparatorItem(8));

		// Add a macro script button to the toolbar.  This is a custom macro script
		// which is loaded, then located, then added.
		TSTR ui = ip->GetDir(APP_UI_DIR);
		TSTR path = ui + _T("\\CUITEST.MCR");
		GetMacroScriptDir().LoadMacroScripts(path.data(), FALSE);
		// Find it based on the category and name defined in the CUITest.mcr file.
		MacroEntry *me = GetMacroScriptDir().FindMacro(_T("CUITestCategory"), _T("CUITestName"));
		if (me) {
			MacroID mID = me->GetID();
			MacroButtonData md2(mID, _T("Macro Scr"), _T("Macro Script Tooltip"));
			iToolBar->AddTool(TBMACRO(&md2));
		}

		// This macro button is processed by our message handler (ID_TB_2)
		MacroButtonData md3(0, NULL, ID_TB_2, "", _T("MacroButtonData Tooltip"), 2, _T("CUITest"));
		iToolBar->AddTool(TBMACRO(&md3));
		
		// -- Set the initial floating position
		SIZE sz; RECT rect;
		iToolBar->GetFloatingCUIFrameSize(&sz);
		rect.top = 200; rect.left = 200;
		rect.right = rect.left+sz.cx; rect.bottom = rect.top+sz.cy;
		GetCUIFrameMgr()->FloatCUIWindow(hWnd, &rect);
		MoveWindow(hWnd, rect.left, rect.right, sz.cx, sz.cy, TRUE);

		// We are done, release the toolbar and frame handles
		ReleaseICustToolbar(iToolBar);
		ReleaseICUIFrame(iFrame);
	}
}
	
void CUITest::EndEditParams(Interface *ip,IUtil *iu) {
	this->iu = NULL;
	this->ip = NULL;
	// Hide the toolbar so the user can't execute anything 
	// when we're not active.
	iFrame->Hide(TRUE);
	// If the toolbar was docked we need to recalc the layout
	// so the toolbar will be removed.
	if (!iFrame->IsFloating()) {
		GetCUIFrameMgr()->RecalcLayout(TRUE);
	}
}

