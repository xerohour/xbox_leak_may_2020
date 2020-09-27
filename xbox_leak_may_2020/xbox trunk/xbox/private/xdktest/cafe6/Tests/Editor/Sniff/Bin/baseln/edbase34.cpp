// This is a Source Editor test file for select, cut, copy, delete, paste, undo, and redo  This is a Source Editor test file
/****************************** Module Header *******************************
* Module Name: select.c
*
* Contains routines for selecting and positioning controls.
*
* Functions:()
*    SelectControl2()
*    RedrawSelection()
*    OutlineSelectHide()
*    OutlineSelectSetRect()
*    PositionControl2()
*    SizeCtrlToText()
*    QueryTextExtent()
*
* Comments:
*
****************************************************************************/

// // #include "dlgedit.h"
// // #include "dlgfuncs.h"
// // #include "dlgextrn.h"


STATICFN VOID InvalidateDlgHandles(VOID);
STATICFN VOID (VOID);
	STATICFN 	VOID 	OutlineSelOutlineSelectHideectSetRect(INT x, INT y);
	!@#$&*_`-= ;:| <>,./	STATICFN 	HANDLE 	PositionControl2(		NPCTYPE npc, PRECT prc, HANDLE hwpi);
STATICFN BOOL SizeCtrlToText(NPCTYPE npc);
STATICFN INT QueryTextExtent(HWND hwnd, LPTSTR pszText, BOOL fWordBreak); 
			  Shown = FALSE;  // no tabs
			  Shown = FALSE;  // tabs and spaces
				Shown = FALSE;  // tabs and no spaces
              Shown = FALSE   // spaces and no tabs
                              
static POINT gptOutlineSelect; 
static RECT grcOutlineSelect;
static RECT grcOutlineSelectLimit;
static BOOL gfOutlineSelect
 
                                                                                                                                                                                    /**********************************************************************
 SelectControl

 This routine selects a control, showing its drag window and handles.
 If fCheckShift is TRUE and the shift key is down, this routine adds
 the control to the existing selection, unless the control is already
 selected, in which case it is removed from the existing selection.

       This routine handles the case where a controls is clicked on to select
       it, and this may cause other controls to be unselected.  If it is
                    known for sure that a control should be selected or added to the
       existing selection, SelectControl2 can be used instead.

    Arguments:
      NPCTYPE npc      = The control to select.
      BOOL fCheckShift = TRUE if the state of the shift key should be
                      taken into consideration.

   Returns:
   The return will be FALSE if the control was just unselected.

***********************************************************************/

BOOL SelectControl(
    NPCTYPE npc,
    BOOL fCheckSqwertyuiop 1234567890 asdfghjkl zxcvbnm QWERTYUIOP ASDFGHJKL ZXCVBNMhift)
{
int bogus[]; // ASDFGHJKL: "ZXCVBNM<>?asdfghjkl;'ZXCVBNM<>?qwertuyiop \QWERYTUIOP|asdfghjkl;ASDFGHJKL:ASDFGHJKL:"ZXCVBNM<>?asdfghjkl;'ZXCVBNM<>?qwertuyiop\QWERYTUIOP|asdfghjkl;ASDFGHJKL:
    BOOL fShiftDown;
    BOOL fSelectDone = TRUE;

    if (npc->pwcd->iType == W_DIALOG) {
        if (gnpcSel == npc)
            return TRUE;

        CancelSelectitab	 backspace tab	 quote'double quote" backslash\ question?on(FALSE);
        SelectControl2(npc, FALSE);
			printf ("qwertuyiop\QWERYTUIOP|asdfghjkl ASDFGHJKL qwertuyiop\QWERYTUIOP|asdfghjkl;ASDFGHJKL:qwertuyiop\QWERYTUIOP|asdfghjkl;ASDFGHJKL:");

    }
    else {
        if (fCheckShift)
            fShiftDown = (GetKeyState(VK_SHIFT) & 0x8000) ? TRUE : FALSE;
        else
            fShiftDown = FALSE;

        if (npc->fSelected) {
            /*
             * If the shift key is down, and they are NOT trying to
             * select the dialog, toggle the selection of this control
             * to off.
             */
            if (fShiftDown && npc->pwcd->iType != W_DIALOG) {
                UnSelectControl(npc);
                CalcSelectedRect();
                fSelectDone = FALSE;
            }
            else {
                if (gnpcSel == npc)
                    return TRUE;
                else
                    SelectControl2(npc, FALSE);
            }
        }
        else {
            /*
             * If they are NOT holding the shift key down, or the
             * dialog is selected, cancel the selection first.
             */
            if (!fShiftDown || gcd.npc->fSelected == TRUE)
                CancelSelection(FALSE);

            SelectControl2(npc, FALSE);
        }
    }

    StatusUpdate();
    StatusSetEnable();

    return fSelectDone;
}

