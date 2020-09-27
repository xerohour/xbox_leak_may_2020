/*===========================================================================*\
 | 
 |  FILE:	wM3_material_pick.cpp
 |			Weighted Morpher for MAX R3
 |			Pickmode for the morph material
 | 
 |  AUTH:   Harry Denholm
 |			Copyright(c) Kinetix 1998
 |			All Rights Reserved.
 |
 |  HIST:	Started 23-12-98
 | 
\*===========================================================================*/

#include "wM3.h"



/*===========================================================================*\
 |
 | The mini-mod stack window handler, for picking a morph modifier
 |
\*===========================================================================*/


BOOL CALLBACK BindProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	M3Mat *mp = (M3Mat*)GetWindowLong(hWnd,GWL_USERDATA);
	if (!mp && msg!=WM_INITDIALOG) return FALSE;

	int id = LOWORD(wParam);
	int notify = HIWORD(wParam);


	switch (msg) {
		case WM_INITDIALOG:{
			mp = (M3Mat*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,(LONG)mp);

			HWND modList = GetDlgItem(hWnd,IDC_MODLIST);

			SendMessage(modList,LB_RESETCONTENT,0,0);

			POINT lpPt; GetCursorPos(&lpPt);
			SetWindowPos(hWnd, NULL, lpPt.x, lpPt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

			Object *pObj = mp->Wnode->GetObjectRef();
			IDerivedObject *pDerObj = NULL;
			Modifier *pMod = NULL;

			if( pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID) 
			{
				pDerObj = (IDerivedObject *) pObj;

				for(int i = 0 ; i < pDerObj->NumModifiers() ; i++ )
				{
					pMod = pDerObj->GetModifier(i);	
					SendMessage(modList,LB_ADDSTRING,0,(LPARAM) (LPCTSTR) pMod->GetName());
				}
			}

			SendMessage(modList,LB_SETCURSEL ,(WPARAM)-1,0);

			break;}


		case WM_COMMAND:

			if (notify==LBN_SELCHANGE){
				if(id==IDC_MODLIST){
					int mkSel = SendMessage(GetDlgItem(hWnd, IDC_MODLIST), LB_GETCURSEL, 0, 0);
					if(mkSel>=0){

						Object *pObj = mp->Wnode->GetObjectRef();
						IDerivedObject *pDerObj = NULL;
						Modifier *pMod = NULL;

						if( pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID) 
						{
							pDerObj = (IDerivedObject *) pObj;
							pMod = pDerObj->GetModifier(mkSel);	
							if(pMod->ClassID() == MR3_CLASS_ID) EnableWindow(GetDlgItem(hWnd,IDOK),TRUE);
							else EnableWindow(GetDlgItem(hWnd,IDOK),FALSE);
						}


					}
				}
			}

			switch (id) {
				case IDOK:
				{
					int mkSel = SendMessage(GetDlgItem(hWnd, IDC_MODLIST), LB_GETCURSEL, 0, 0);
					if(mkSel>=0){

						Object *pObj = mp->Wnode->GetObjectRef();
						IDerivedObject *pDerObj = NULL;
						Modifier *pMod = NULL;

						if( pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID) 
						{
							pDerObj = (IDerivedObject *) pObj;
							pMod = pDerObj->GetModifier(mkSel);	

							mp->ReplaceReference(102,(MorphR3*)pMod);
							mp->obName = mp->Wnode->GetName();

							mp->dlg->UpdateMorphInfo(UD_LINK);
							mp->dlg->ReloadDialog();
						}


					}
				}
				case IDCANCEL:
					EndDialog(hWnd,1);
				break;
				}
			break;
		
		

		default:
			return FALSE;
		}
	return TRUE;
	}



/*===========================================================================*\
 |
 | Pickmode support
 |
\*===========================================================================*/


BOOL  GetMorphMod::Filter(INode *node)
{
	Interval valid; 
	
	ObjectState os = node->GetObjectRef()->Eval(mp->dlg->ip->GetTime());

	if( os.obj->IsDeformable() == FALSE ) return FALSE;

	return TRUE;
}


BOOL  GetMorphMod::Pick(INode *node)
	{
	if (node) {

		mp->Wnode = node;

		int res = DialogBoxParam( 
			hInstance, 
			MAKEINTRESOURCE( IDD_BINDMORPH ),
			GetDlgItem(mp->dlg->hPanel,IDC_BIND),
			(DLGPROC)BindProc,
			(LPARAM)(M3Mat*)mp
			);

	}
	
	return TRUE;
}


void  GetMorphMod::EnterMode()
{
	isPicking=TRUE;
	if (mp->dlg->pickBut) mp->dlg->pickBut->SetCheck(TRUE);
}

void  GetMorphMod::ExitMode()
{
	isPicking=FALSE;
	if (mp->dlg->pickBut) mp->dlg->pickBut->SetCheck(FALSE);
}