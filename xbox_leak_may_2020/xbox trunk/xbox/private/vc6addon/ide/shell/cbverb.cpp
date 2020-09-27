/*****************************************************************
This file implements the OLE Verb menu for Command bars. Insert
a dialog into your application. Right click in the dialog. Select
Insert ActiveX Control... Insert a control. Right click on the control.
You should see a context menu for handling the control. 

  Some controls will put up a subpopup, others will not.

All of this code was borrowed from the standard OLE UI Dll. This code
was found on \\savik\cairo. The slm project is ole2ui32. The code in this
file borrows from the following files:
	ole2ui.h
	ole2ui.cpp
	olestd.cpp
	utility.cpp
	utility.h
	ole2ui.rc
	common.h

The main change to the code was to make it compatible with Command Bars.
	* Loading resource strings dyanmically instead of caching them.
	* Using MFC functions such as AfxLoadString

The really funny thing is that they took a bunch of functions out of MFC,
converted them to C++ and used them. I'm converting these funcitons back
to C++...


****************************************************************/

#include <stdafx.h>

#include <objbase.h>

#ifndef STRICT
#define STRICT
#endif

#include "resource.h"
#include <stdarg.h>

// From common.h

 // Maximum length of Object menu
#define OLEUI_OBJECTMENUMAX         256

// Maximim length of a path in BYTEs
#define MAX_PATH_SIZE               (MAX_PATH*sizeof(TCHAR))

////////////////////////////////////////////////////////
//
// From Utility.
//
//

/* OleStdFree
** ----------
**    free memory using the currently active IMalloc* allocator
*/
STDAPI_(void) OleStdFree(LPVOID pmem)
{
        LPMALLOC pmalloc;

        if (pmem == NULL)
                return;

        if (CoGetMalloc(MEMCTX_TASK, &pmalloc) != NOERROR)
        {
				ASSERT(0) ;
                return;
        }
        if (1 == pmalloc->DidAlloc(pmem))
        {
            pmalloc->Free(pmem);
        }
        pmalloc->Release();
}


/*
 * OleUIAddVerbMenu
 *
 * Purpose:
 *  Add the Verb menu for the specified object to the given menu.  If the
 *  object has one verb, we directly add the verb to the given menu.  If
 *  the object has multiple verbs we create a cascading sub-menu.
 *
 * Parameters:
 *  lpObj           LPOLEOBJECT pointing to the selected object.  If this
 *                  is NULL, then we create a default disabled menu item.
 *
 *  lpszShortType   LPTSTR with short type name (AuxName==2) corresponding
 *                  to the lpOleObj. if the string is NOT known, then NULL
 *                  may be passed. if NULL is passed, then
 *                  IOleObject::GetUserType will be called to retrieve it.
 *                  if the caller has the string handy, then it is faster
 *                  to pass it in.
 *
 *  hMenu           HMENU in which to make modifications.
 *
 *  uPos            Position of the menu item
 *
 *  uIDVerbMin      UINT ID value at which to start the verbs.
 *                      verb_0 = wIDMVerbMin + verb_0
 *                      verb_1 = wIDMVerbMin + verb_1
 *                      verb_2 = wIDMVerbMin + verb_2
 *                      etc.
 *  uIDVerbMax      UINT maximum ID value allowed for object verbs.
 *                     if uIDVerbMax==0 then any ID value is allowed
 *
 *  bAddConvert     BOOL specifying whether or not to add a "Convert" item
 *                  to the bottom of the menu (with a separator).
 *
 *  idConvert       UINT ID value to use for the Convert menu item, if
 *                  bAddConvert is TRUE.
 *
 * *****NOTE -- I do not return a menu pointer!*****
 *  lphMenu         HMENU FAR * of the cascading verb menu if it's created.
 *                  If there is only one verb, this will be filled with NULL.
 *
 *
 * Return Value:
 *  BOOL            TRUE if lpObj was valid and we added at least one verb
 *                  to the menu.  FALSE if lpObj was NULL and we created
 *                  a disabled default menu item
 */

STDAPI_(BOOL) CmdBarOleUIAddVerbMenu(LPOLEOBJECT lpOleObj,
        LPCTSTR lpszShortType,
        CBMenuPopup* pMenu, UINT uPos,
        UINT uIDVerbMin, UINT uIDVerbMax,
        BOOL bAddConvert, UINT idConvert)
        //CBMenuPopup** ppSubMenu)
{
        LPENUMOLEVERB       lpEnumOleVerb = NULL;
        OLEVERB             oleverb;
		CString strShortTypeName = lpszShortType ;
		CString				strVerbName;
        HRESULT             hrErr;
        BOOL                fStatus;
        BOOL                fIsLink = FALSE;
        BOOL                fResult = TRUE;
        BOOL                fAddConvertItem = FALSE;
        int                 cVerbs = 0;
        UINT                uFlags = MF_BYPOSITION;
        
		CString				strBuffer ;

        // Set fAddConvertItem flag
        if (bAddConvert & (idConvert != 0))
                fAddConvertItem = TRUE;

		// Create the Popup Menu here.
		CBMenuPopup* pSubMenu = new CBMenuPopup ;
		//@ pSubMenu->Create(pMenu, TRUE) ; // FIX
		pSubMenu->CreatePopupMenu(pMenu) ;

        // Delete whatever menu may happen to be here already.
        //DeleteMenu(hMenu, uPos, uFlags); 
		pMenu->DeleteMenu(uPos,uFlags) ;

        if ((!lpOleObj) || IsBadReadPtr(lpOleObj, sizeof (IOleObject)))
                goto AVMError;

        if (strShortTypeName.IsEmpty())
        {
                // get the Short form of the user type name for the menu
				TRACE0("IOleObject::GetUserType called\r\n");

                LPOLESTR wszShortTypeName = NULL;
                hrErr = lpOleObj->GetUserType(
                                USERCLASSTYPE_SHORT,
                                &wszShortTypeName);
				strShortTypeName = wszShortTypeName ;
                if (NULL != wszShortTypeName)
                {
                    OleStdFree(wszShortTypeName);
                }

                if (NOERROR != hrErr)
				{
                        TRACE("IOleObject::GetUserType returned %x.\r\n", hrErr);
						return hrErr ;
				}
        }

        // check if the object is a link
        IUnknown* pIOleLink;
        hrErr = lpOleObj->QueryInterface(IID_IOleLink,(void**)&pIOleLink);
        if (SUCCEEDED(hrErr))
        {
			pIOleLink->Release();
            fIsLink = TRUE;
        }
		else
		{
			fIsLink = FALSE;
		}


        // Get the verb enumerator from the OLE object
        TRACE0("IOleObject::EnumVerbs called\r\n");
        hrErr = lpOleObj->EnumVerbs(
                        (LPENUMOLEVERB FAR*)&lpEnumOleVerb
        );

        if (NOERROR != hrErr)
		{
                TRACE("IOleObject::EnumVerbs returned %x.\r\n", hrErr);
				return hrErr ;
		}

        // loop through all verbs
        while (lpEnumOleVerb != NULL)
        {
                hrErr = lpEnumOleVerb->Next(
                                1,
                                (LPOLEVERB)&oleverb,
                                NULL
                );
                if (NOERROR != hrErr)
                        break;              // DONE! no more verbs

                /* OLE2NOTE: negative verb numbers and verbs that do not
                **    indicate ONCONTAINERMENU should NOT be put on the verb menu
                */
                if (oleverb.lVerb < 0 ||
                                ! (oleverb.grfAttribs & OLEVERBATTRIB_ONCONTAINERMENU))
                {
                        /* OLE2NOTE: we must still free the verb name string */
                        if (oleverb.lpszVerbName)
                                OleStdFree(oleverb.lpszVerbName);
                        continue;
                }

				strVerbName = oleverb.lpszVerbName ;
                if (oleverb.lpszVerbName)
                   OleStdFree(oleverb.lpszVerbName);

                if ( 0 == uIDVerbMax ||
                        (uIDVerbMax >= uIDVerbMin+(UINT)oleverb.lVerb) )
                {
                        fStatus = pSubMenu->InsertMenu(
                                        (UINT)-1,
                                        MF_BYPOSITION | (UINT)oleverb.fuFlags,
                                        uIDVerbMin+(UINT)oleverb.lVerb,
                                        strVerbName);
                        if (! fStatus)
                                goto AVMError;

                        cVerbs++;
                }
        }

        // Add the separator and "Convert" menu item.
        if (fAddConvertItem)
        {
			static TCHAR szConvert[OLEUI_OBJECTMENUMAX];
			VERIFY(::AfxLoadString(IDS_OLE2UICONVERT,szConvert, OLEUI_OBJECTMENUMAX)) ;
            
                if (0 == cVerbs)
                {
                        uIDVerbMin = idConvert;
                        // if object has no verbs, then use "Convert" as the obj's verb
						strVerbName = szConvert ;
						LPTSTR lpsz = strVerbName.GetBuffer(strVerbName.GetLength()) ;

                        // remove "..." from "Convert..." string; it will be added later
                        if (lpsz)
                        {
                                while(*lpsz && *lpsz != '.')
                                        lpsz = CharNext(lpsz);
                                *lpsz = '\0';
                        }
						strVerbName.ReleaseBuffer() ;

                }

                if (cVerbs > 0)
                {
                        fStatus = pSubMenu->InsertMenu((UINT)-1,
                                                MF_BYPOSITION | MF_SEPARATOR,
                                                (UINT)0,
                                                (LPCTSTR)NULL);
                        if (! fStatus)
                                goto AVMError;
                }

                /* add convert menu */
                fStatus = pSubMenu->InsertMenu((UINT)-1,
                                        MF_BYPOSITION,
                                        idConvert,
                                        (LPCTSTR)szConvert);
                if (! fStatus)
                        goto AVMError;

                cVerbs++;
        }


        /*
         * Build the appropriate menu based on the number of verbs found
         *
         */
        if (cVerbs == 0)
        {
                // there are NO verbs (not even Convert...). set the menu to be
                // "<short type> &Object/Link" and gray it out.
      			AfxFormatString1(strBuffer,
                        (fIsLink ? IDS_OLE2UIEDITLINKCMD_NVERB : IDS_OLE2UIEDITOBJECTCMD_NVERB),
                        strShortTypeName /*May be empty...*/);

                uFlags |= MF_GRAYED;

                fResult = FALSE;
				
				// Single menu item. So delete the SubPopup
				delete pSubMenu ;
				pSubMenu = NULL ;

        }
        else if (cVerbs == 1)
        {
                //One verb without Convert, one item.
				UINT ids = (fIsLink ? IDS_OLE2UIEDITLINKCMD_1VERB : IDS_OLE2UIEDITOBJECTCMD_1VERB);

                // strip ampersands from lpszVerbName to ensure that
                // the right character is used as the menu key
                LPTSTR pchIn;
                LPTSTR pchOut;
                pchIn = pchOut = strVerbName.GetBuffer(strVerbName.GetLength());
                while (*pchIn)
                {
                    while (*pchIn && '&' == *pchIn)
                    {
                        pchIn++;
                    }
                    *pchOut = *pchIn;
                    pchOut++;
                    pchIn++;
                }
                *pchOut = 0;
				strVerbName.ReleaseBuffer() ;

                AfxFormatString2(strBuffer, ids, strVerbName, strShortTypeName);

                // if only "verb" is "Convert..." then append the ellipses
                if (fAddConvertItem)
				{

                    strBuffer += TEXT("...");
				}

				delete pSubMenu ;
				pSubMenu = NULL ;
        }
        else
        {

                //Multiple verbs or one verb with Convert, add the cascading menu
                AfxFormatString1(strBuffer, 
                        (fIsLink ? IDS_OLE2UIEDITLINKCMD_NVERB: IDS_OLE2UIEDITOBJECTCMD_NVERB),
						strShortTypeName); //lpszShortTypeName ? lpszShortTypeName : TEXT(""))

                uFlags |= MF_ENABLED | MF_POPUP;

				// We are going to add a subpopup so make the cmd id zero.
				uIDVerbMin = 0 ; 

        }

        if (!pMenu->InsertMenu(uPos, uFlags, uIDVerbMin, strBuffer,pSubMenu)) 
        {
AVMError:
				// No object command.				

				// Load the string.
				CString strNoObjectCmd ;
				strNoObjectCmd.LoadString(IDS_OLE2UIEDITNOOBJCMD);
				ASSERT(!strNoObjectCmd.IsEmpty());

				// Put it in the menu.
                pMenu->InsertMenu(uPos, MF_GRAYED | uFlags,	
                        uIDVerbMin, strNoObjectCmd);
                fResult = FALSE;

			// If there has been an error, we delete the submenu.
			// If there isn't an error, it is owned by pMenu and it is
			// pMenu's job to delete it.
			if (pSubMenu != NULL)
			{
				delete pSubMenu;
				pSubMenu = NULL ;
			}
        }

        if (lpEnumOleVerb)
                lpEnumOleVerb->Release();

		// Do not delete pSubMenu here.
        return fResult;
}

////////////////////////////////////////////////////////
//
//
//	Copied from MFC
//
//

/////////////////////////////////////////////////////////////////////////////////////////
// 
//  Support for adding OLE Verbs into CmdBars...
//

void CmdBarOleSetEditMenu(COleClientItem* pItem, CBMenuPopup* pMenu,
	UINT iMenuItem, UINT nIDVerbMin, UINT nIDVerbMax, UINT nIDConvert) 
{
	if (pItem != NULL)
		ASSERT_VALID(pItem);

	// Notice that my version of OleUIAddVerMenu is NOT the same as the
	// real version. This version does not return the sub popup menu.
	
	if (!CmdBarOleUIAddVerbMenu(pItem != NULL ? pItem->m_lpObject : NULL,
		NULL, pMenu, iMenuItem,
		nIDVerbMin, nIDVerbMax, nIDConvert != 0, nIDConvert))
	{
		CString szBuffer;
		pMenu->GetMenuString(iMenuItem, szBuffer, MF_BYPOSITION);
		pMenu->DeleteMenu(iMenuItem, MF_BYPOSITION);
		pMenu->InsertMenu(
			iMenuItem, MF_BYPOSITION|MF_STRING|MF_GRAYED|MF_DISABLED,
			nIDVerbMin, szBuffer);
	}
}

//
//
// Add OLE Object Verbs to a menu.
//
void CPartDoc::OnUpdateObjectVerbMenu(CCmdUI* pCmdUI)
{
	// Get a pointer to the menu. Cast to a CmdBar menu.
	CBMenuPopup* pCBMenu = (CBMenuPopup*)pCmdUI->m_pOther ;
	
	// Now check to see if its really a command bar menu.
	if (!CBMenuPopup::IsCBMenuCmdUI(pCmdUI) || 
		pCBMenu->IsSubPopup())
	{
		// This is either NOT a command bar, or its a SubPopup.
		// Therefore, let MFC do the work for us.
		COleDocument::OnUpdateObjectVerbMenu(pCmdUI) ;
		return ;
	}

	// We are updating a wonderous Command Bar. Therefore, we have
	// to do all the work ouselves.


	// All of this code was cut and pasted from MFC and then hacked up
	// until it worked.

	// check for single selection
	COleClientItem* pItem = GetPrimarySelectedItem(GetRoutingView());
	if (pItem == NULL || pItem->GetType() == OT_STATIC)
	{
		// no selection, or is 'static' item
		pCmdUI->Enable(FALSE);

		//return ; // This will put in the <OLE OBJECTS VERBS GO HERE>> Message.
	}

	// only include Convert... if there is a handler for ID_OLE_EDIT_CONVERT
	UINT nConvertID = ID_OLE_EDIT_CONVERT;
	AFX_CMDHANDLERINFO info;
	if (!OnCmdMsg(ID_OLE_EDIT_CONVERT, CN_COMMAND, NULL, &info))
	{
		nConvertID = 0;
	}

	// update the menu
	CmdBarOleSetEditMenu(GetPrimarySelectedItem(GetRoutingView()),
		pCBMenu, pCmdUI->m_nIndex,
		ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, nConvertID);
}

