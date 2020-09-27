///////////////////////////////////////////////////////////////////////////////
//	USYMDLG.CPP
//
//	Created by :			Date :
//		DavidGa					9/20/93
//
//	Description :
//		Implementation of the UISymbolBrowser class
//

#include "stdafx.h"
#include "usymdlg.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\vres.h"

// this dialog is in sushi\vres's RC file (VRES.RC)

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// BEGIN_HELP_COMMENT
// Function: int UISymbolBrowser::CountSymbols(BOOL bReadOnly)
// Description: Get the number of symbols displayed in the symbol browser dialog.
// Return: An integer that contains the number symbols displayed in the symbol browser dialog.
// Param: bReadOnly A Boolean value that indicates whether to include read only symbols (TRUE) in the count or not.
// END_HELP_COMMENT
int UISymbolBrowser::CountSymbols(BOOL bReadOnly)
{
	if( bReadOnly )
		MST.WCheckCheck(GetLabel(VRES_ID_SYM_SHOWREADONLY));
	else
		MST.WCheckUnCheck(GetLabel(VRES_ID_SYM_SHOWREADONLY));

	return MST.WListCount(GetLabel(VRES_IDC_SYM_LIST));
}
