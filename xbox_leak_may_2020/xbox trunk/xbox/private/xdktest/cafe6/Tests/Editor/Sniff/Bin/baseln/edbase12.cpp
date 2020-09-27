// This is a Source Editor test file for select, cut, copy, delete, paste, undo, and redo  This is a Source Editor test file
/****************************** Module Header *******************************
* M
*
* C
*
* F
*  
*  
*  
*  
*  
*  
*  
*
* C
*
**********/

// 
// 
// 


STA
STA
INT y);
T prc, HANDLE hwpi);
STA
STAak); 



   
   
sta
sta
sta
sta
 
                                                                                                                  /**********************************************************************
 Se

 Th
 If
 th
 se

   o select
    is
   or added to the
   

   
   
   
   

   
   

*****/

BOO
   
   
{
int \QWERYTUIOP|asdfghjkl;ASDFGHJKL:ASDFGHJKL:"ZXCVBNM<>?asdfghjkl;'ZXCVBNM<>?qwertuyiop\QWERYTUIOP|asdfghjkl;ASDFGHJKL:
   
   

   
   
   

   
   
yiop\QWERYTUIOP|asdfghjkl;ASDFGHJKL:qwertuyiop\QWERYTUIOP|asdfghjkl;ASDFGHJKL:");

   
   
   
   LSE;
   
   

   
   
   
   l
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

