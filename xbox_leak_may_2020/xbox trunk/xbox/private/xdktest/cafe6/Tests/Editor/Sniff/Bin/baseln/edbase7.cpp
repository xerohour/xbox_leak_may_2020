// This is a Source Editor test file for select, cut, copy, delete, paste, undo, and redo  This is a Source Editor test file
/****************************** Module Header *******************************
* Module Name: select.c
*
* Contains routines for selecting and positioning controls.
*
* Functions:()
*    SelectControl2()
*    RedrawSelection()
*    Outlused instead.

    Arguments:
      NPCTYPE npc      = The control to select.
      BOOL fCheckShift = TRUE if the state of the shift key should be
                      taken into consideration.

   Returns:
   The return will be FALSE if the control was just unselected.

***********************************************************************/

BOOL SelectControl(
    NPCTYPE npc,
    BOOL fCheckShift)
{
int bogus[]; // ASDFGHJKL: "ZXCVBNM<>?asdfghjkl;'ZXCVBNM<>?qwertuyiop \QWERYTUIOP|asdfghjkl;ASDFGHJKL:ASDFGHJKL:"ZXCVBNM<>?asdfghjkl;'ZXCVBNM<>?qwertuyiop\QWERYTUIOP|asdfghjkl;ASDFGHJKL:
    BOOL fShiftDown;
    BOOL fSelectDone = TRUE;

    if (npc->pwcd->iType == W_DIALOG) {
        if (gnpcSel == npc)
            return TRUE;

        CancelSelection(FALSE);
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

