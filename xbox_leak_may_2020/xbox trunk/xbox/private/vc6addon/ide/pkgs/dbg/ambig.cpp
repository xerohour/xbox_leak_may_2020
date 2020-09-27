/**********************************************************************

		RESOLVE AMBIGUITY DIALOG BOX AND PROCESSING

		Purpose : Resolve ambiguity in setting breakpoints on
				  overloaded functions

		origin	: ( BRPTDLG.C in CV401 project )

		implementation : 03 / 31 / 1992 J.M.

***********************************************************************/

// Visual C++ include files
#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define MAX_TMNAME	256

typedef struct _BPRSLV {
	int	fUse;
	int	itm;
} BPRSLV; // BreakPoint Resolve

static BPRSLV  far *rgbpr;
static int  cbpr;
static int  ibprSel ;
static PHTM rgtm;

static char * PASCAL near FormatTMName(char *szOut,int ibpr) ;
static void PASCAL near DlgInitList(HWND hwndList) ;

/*********************************************************************
		DlgInitList     : J.M. Mar 1992
        Purpose         : Initialize the ambiguous list
*********************************************************************/
// [CAVIAR #???? 11/15/92 v-natjm]
// Added an horizontal scrollbar to the ambig list

static void PASCAL near DlgInitList(HWND hwndList)
{
	int ibpr = 0 ;
	char szOut[_MAX_PATH] ;
	HDC hDC ;
	HFONT hFont ;
	TEXTMETRIC tm ;
	UINT wMaxExt = 0 ;
	UINT wExtent ;
	SIZE size;

	hDC=GetDC(hwndList);
	hFont=(HFONT)SendMessage(hwndList,WM_GETFONT,0,0L) ;

	if(hFont!= (HFONT)NULL)
		SelectObject(hDC,hFont) ;

	GetTextMetrics(hDC,&tm) ;

	for(ibpr=0;ibpr<cbpr;ibpr++) {
		if(FormatTMName(szOut,ibpr) != NULL) {
			// [CAVIAR #5877 11/24/92 v-natjm]
			SendMessage(
			 	hwndList,
			 	LB_ADDSTRING,
			 	0,
			 	(LONG)(LPSTR)szOut) ;
			// Compute size of string to reset
			GetTextExtentPoint(hDC,szOut,_ftcslen(szOut),&size);
			wExtent=size.cx+tm.tmAveCharWidth ;
			if(wExtent > wMaxExt) wMaxExt = wExtent ;
		}
	}
	// Adjust horizontal scrollbar
	SendMessage(hwndList, LB_SETHORIZONTALEXTENT, wMaxExt, 0L);
	ReleaseDC(hwndList, hDC);

	// Select the first one as the default...
	SendMessage(hwndList,LB_SETCURSEL,0,0L) ;
}


/*********************************************************************
		DlgResolveAmbig : J.M. Mar 1992
        Purpose         : Process the resolve ambiguity dialog box

        input			: Standard
        output			: None
        returns			: TRUE  means we set the breakpoints
        				  FALSE means we dismissed
*********************************************************************/
BOOL FAR PASCAL EXPORT DlgResolveAmbig(
	HWND hDlg,
	unsigned message,
	UINT wParam,
	LONG lParam)
{
	static HWND hwndList ;

	switch(message) {
		case WM_INITDIALOG : {
			// Initialize the TM list box
			hwndList = GetDlgItem(hDlg,ID_AMB_LIST) ;
			DlgInitList(hwndList) ;
			// We don't want to change the focus...
			return TRUE ;
		}
		case WM_CONTEXTMENU: {
			DoHelpContextMenu(CWnd::FromHandle(hDlg), IDD_RESOLVEAMBIGUITY, CPoint(LOWORD(lParam),HIWORD(lParam)));
			return TRUE;
		}
		case WM_HELP: {
			DoHelpInfo(CWnd::FromHandle(hDlg), IDD_RESOLVEAMBIGUITY, (HELPINFO *)lParam);
			return TRUE;
		}
		case WM_COMMAND : {
			switch(GET_WM_COMMAND_ID(wParam,lParam)) {
				case ID_AMB_LIST :
					if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_DBLCLK) {
						// [CAVIAR #5434 11/13/92 v-natjm]
						PostMessage(hDlg, WM_COMMAND,
							GET_WM_COMMAND_MPS(ID_AMB_OK, 0, 0));
						return TRUE ;
					}
					break ;

				case IDOK :
				case ID_AMB_OK : {
					long lRet ;

					// Check the selected TM...
					lRet = SendMessage(hwndList,LB_GETCURSEL,0,0L) ;
					if(lRet != LB_ERR) {
						ibprSel = (int)lRet ;
						rgbpr[ibprSel].fUse = TRUE ;
					}
					EndDialog(hDlg,TRUE) ;
					return TRUE ;
				}

				case IDCANCEL :
				case ID_AMB_CANCEL :
					EndDialog(hDlg,FALSE) ;
					return TRUE ;
			}
			break ;
		}
		default : {
			break ;
		}
	}

	// We didn't process the message
	return DefaultDialogHandler(hDlg, message, wParam, lParam);
}

// [CAVIAR #5432 11/15/92 v-natjm]
// We were not showing the context in case of an overloading
// with file-scope symbols ( static void foo(int) ; )
// Format is now:
// {func,module,exe} sym (type decl)
/***************************************************************************
		FormatTMName	: J.M. Mar 1992
		Purpose			: Format a string from a TM

		Input			: Pointer to szOut and BP index in our structure
		Output 			: Pointer to szOut ( not allocated... )
		returns			: Pointer to szOut ( filled string ) or NULL
***************************************************************************/
static char * PASCAL near FormatTMName(char *szOut,int ibpr)
{
	EEHSTR	hstr;
	EEHSTR	hszType;
	char far *lsz;
	char 		*szSave = szOut;
	int		cchMax = _MAX_PATH-1 ;

	if(EEGetNameFromTM(&rgtm[rgbpr[ibpr].itm], &hstr) == EENOERROR) {
		if(EEGetTypeFromTM(&rgtm[rgbpr[ibpr].itm], hstr, &hszType, 1L) == EENOERROR) {
			// skip the structure EEGetTypeFromTM places at the front
			// of the buffer.
			HCXTL hcxtl ;

			*szOut = '\0';

			if(!EEGetCXTLFromTM(&rgtm[rgbpr[ibpr].itm],&hcxtl)) {
				CXT    cxt;
				ADDR   addrT = { 0 };
				ushort usSegType = EEANYSEG;

            // Get the BP address from TM...
				BPADDRFromTM(&rgtm[rgbpr[ibpr].itm],&usSegType,&addrT) ;
				// And then the context from the addr...
				if(!SHSetCxt(&addrT,&cxt)) {
					// Build the cxt
					cxt = *(PCXT)BMLock(hcxtl) ;
					BMUnlock(hcxtl) ;
				}
				// Free the allocated space from the EE
				EEFreeCXTL(&hcxtl) ;
            // Format an understandable CXT...
				if(!EEFormatCXTFromPCXT((PCXT)&cxt,(PEEHSTR)&hstr)) {
				   // Get string from handle
					lsz = (char FAR *)BMLock(hstr) ;
					_ftcsncpy(szOut,lsz,cchMax) ;
					cchMax -= _ftcslen(szOut) ;
					BMUnlock(hstr) ;
				}
			}
			// Get the type string
			lsz = (char FAR *)BMLock(hszType) + sizeof(HDR_TYPE) ;
			// Concatenate it to the context
			_ftcsncat(szOut,lsz,cchMax) ;
			// Free all EE allocation
			BMUnlock(hszType) ;
			EEFreeStr(hszType) ;
			EEFreeStr(hstr) ;
			return szSave;
		}
	}
	return NULL;
}

// [CAVIAR #5459 11/27/92 v-natjm]
/*********************************************************************
		BPResolveDlg : J.M. Mar 1992
        Purpose      : This function is called from BRKPT1.C in CV401
        			   project to resolve ambiguous overloaded symbols
        			   in breakpoints.

 		Input  :   	TMList- A list of breakpoints to be resolved
	   				cTMListMax - The number of TMs in the list
	   				(some may be null).

 		Output :    TMList- Only the breakpoints chosen
  		Returns The number of breakpoints chosen
*********************************************************************/
int PASCAL BPResolveDlg( PHTM rgtmT, int ctmMax,BOOL fUnAmbig )
{
	int		itm;
	int		ctm = 0;
	int 	ibpr;
	PBREAKPOINTNODE pBpNode = (PBREAKPOINTNODE)NULL ;

	// Find out which one is our Breakpoint
	pBpNode = BHLastBPCreated() ;

	rgtm = rgtmT;

	rgbpr = (BPRSLV far *)_fcalloc(ctmMax,sizeof(BPRSLV)) ;
    ASSERT(rgbpr != (BPRSLV far *)NULL) ;

	for (itm = 0; itm < ctmMax; itm++) {
    	if (rgtm[itm] != (HTM)NULL) {
			rgbpr[ctm].itm = itm;
    		rgbpr[ctm].fUse = FALSE ; //!fUnAmbig
			ctm++;
		}
    }
	cbpr = ctm;

	rgtm = rgtmT ;

	if(pBpNode != (PBREAKPOINTNODE)NULL) {
		if((pbpnAmbigBP(pBpNode) == FALSE) &&
			(pbpnBPTMindex(pBpNode) > AMB_STS_UNRESOLVED) &&
			(pbpnBPTMindex(pBpNode) < cbpr)) {
			rgbpr[pbpnBPTMindex(pBpNode)].fUse = TRUE ;
		}
		else {
			if(StartDialog(IDD_RESOLVEAMBIGUITY, (DLGPROC)DlgResolveAmbig) == FALSE) {
				pbpnAmbigBP(pBpNode) = TRUE ;
				pbpnBPTMindex(pBpNode) = AMB_STS_UNRESOLVED ;
			}
			else {
				pbpnAmbigBP(pBpNode) = FALSE ;
				pbpnBPTMindex(pBpNode) = ibprSel ;
			}
		}
	}

	itm = 0;
	for (ibpr = 0; ibpr < cbpr; ibpr++) {
    	if (rgbpr[ibpr].fUse == TRUE) {
        	itm++;
		}
    	else {
        	EEFreeTM(&rgtm[rgbpr[ibpr].itm]);
        	rgtm[rgbpr[ibpr].itm] = (HTM)NULL;
		}
    }

	_ffree(rgbpr);

	return itm ;
}
