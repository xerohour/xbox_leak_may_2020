// findspec.h

// FindSpecialHandler
//
// hDlg		Dialog handle
// idEdit	ID of combo box control to modify
// cmd		Menu command ID from the IDR_FIND_SPECIAL menu
// dwSel	Packed selection range (use return val from CB_GETEDITSEL)
// bWrap	TRUE: Wrap range/group/tag items.
//			FALSE: Just replace selection
//
// Use OnCommandRange ID_FIND_ANYCHARACTER to ID_FIND_STRING
// for Find What text.
//
// Use OnCommandRange ID_REPLACE_FINDWHATTEXT to ID_REPLACE_TAG_9
// for Replace With text:
//
// Example use in a DlgProc:
//
//	case WM_COMMAND:
//	{
//		UINT CommandId;
//
//		CommandId = GET_WM_COMMAND_ID(wParam, lParam);
//		if (ID_FIND_ANYCHARACTER <= CommandId && CommandId <= ID_FIND_STRING)
//		{
//			// add appropriate item to Find What
//			return FindSpecialHandler(hDlg,ID_FIND_WHAT,CommandId);
//		}
//	...

BOOL FindSpecialHandler( CButtonEdit &ComboEdit, UINT cmd, BOOL bWrap = FALSE );

/////////////////////////////////////////////////////////////
// ReplaceSel - Replace the selection in a combobox.
//
//   ComboEdit		Edit control to modify
//
//   szRep		Replacement string
//
//   selpos		How to select the result:
//		spStart			Ip at start of replaced selection
//	 	spEnd			Ip at end of replaced selection
//	 	spSel			Select the replacement
//		spInsideStart	Ip inside start of wrap
//		spInsideEnd		Ip inside end of wrap
//
//   bWrap		Wrap the replacement around the selection?
//
//   nWrapBreak	Position in szRep to break when wrapping.
//   
enum NewSelPos { spStart, spEnd, spSel, spInsideStart, spInsideEnd };
void ReplaceSel( 
	CButtonEdit & ComboEdit,
	LPCTSTR szRep, 
	NewSelPos selpos = spEnd, 
	BOOL bWrap = FALSE, 
	int nWrapBreak = 0
	);
