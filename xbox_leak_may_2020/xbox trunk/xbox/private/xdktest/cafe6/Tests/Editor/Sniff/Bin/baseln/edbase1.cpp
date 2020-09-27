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
	STATICFNe {
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

