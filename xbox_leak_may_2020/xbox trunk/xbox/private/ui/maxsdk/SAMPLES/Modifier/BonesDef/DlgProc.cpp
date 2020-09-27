/**********************************************************************
 
	FILE: DlgProc.cpp

	DESCRIPTION:  Main DLg proc to handle windows stuff mainly the list box and buttons

	CREATED BY: Peter Watje

	HISTORY: 8/5/98




 *>	Copyright (c) 1998, All Rights Reserved.
 **********************************************************************/
#include "mods.h"
#include "iparamm.h"
#include "shape.h"
#include "spline3d.h"
#include "splshape.h"
#include "linshape.h"

// This uses the linked-list class templates
#include "linklist.h"
#include "bonesdef.h"


static HIMAGELIST hParamImages = NULL;


void BonesDefMod::RegisterClasses()
	{
	if (!hParamImages) {
		HBITMAP hBitmap, hMask;	
		hParamImages = ImageList_Create(16, 15, TRUE, 4, 0);
		hBitmap = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_PARAMS));
		hMask   = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_PARAMS_MASK));
		ImageList_Add(hParamImages,hBitmap,hMask);
		DeleteObject(hBitmap);
		DeleteObject(hMask);
		}
	}

extern BonesRightMenu rMenu;

void SpinnerOff(HWND hWnd,int SpinNum,int Winnum)
{	
	ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hWnd,SpinNum));
	if (spin2 != NULL)
		{
		spin2->Enable(FALSE);
		EnableWindow(GetDlgItem(hWnd,Winnum),FALSE);
		ReleaseISpinner(spin2);
		}

};

void SpinnerOn(HWND hWnd,int SpinNum,int Winnum)
{
	ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hWnd,SpinNum));
	if (spin2 != NULL)
		{
		spin2->Enable(TRUE);
		EnableWindow(GetDlgItem(hWnd,Winnum),TRUE);
		ReleaseISpinner(spin2);
		}

};


BOOL MapDlgProc::DlgProc(
		TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
	int i;

	switch (msg) {
		case WM_INITDIALOG:
			{

			mod->hParam = hWnd;

			for (i=0;i<mod->BoneData.Count();i++)
				{
				TCHAR title[200];

				if (mod->BoneData[i].Node != NULL)
					{
					Class_ID bid(BONE_CLASS_ID,0);
					ObjectState os = mod->BoneData[i].Node->EvalWorldState(mod->RefFrame);

					if (( os.obj->ClassID() == bid) && (mod->BoneData[i].name.Length()) )
						{
						_tcscpy(title,mod->BoneData[i].name);
						}
					else _tcscpy(title,mod->BoneData[i].Node->GetName());

					SendMessage(GetDlgItem(hWnd,IDC_LIST1),
						LB_ADDSTRING,0,(LPARAM)(TCHAR*)title);



					}
				}

			mod->iCrossSectionButton = GetICustButton(GetDlgItem(hWnd, IDC_CREATE_CROSS_SECTION));
			mod->iCrossSectionButton->SetType(CBT_CHECK);
			mod->iCrossSectionButton->SetHighlightColor(GREEN_WASH);

			mod->iPaintButton = GetICustButton(GetDlgItem(hWnd, IDC_PAINT));
			mod->iPaintButton->SetType(CBT_CHECK);
			mod->iPaintButton->SetHighlightColor(GREEN_WASH);

			mod->iCrossSectionButton->Disable();
			mod->iPaintButton->Disable();
			EnableWindow(GetDlgItem(hWnd,IDC_CREATE_REMOVE_SECTION),FALSE);


			EnableWindow(GetDlgItem(hWnd,IDC_FILTER_VERTICES_CHECK),FALSE);
			EnableWindow(GetDlgItem(hWnd,IDC_FILTER_BONES_CHECK),FALSE);
			EnableWindow(GetDlgItem(hWnd,IDC_FILTER_ENVELOPES_CHECK),FALSE);
			EnableWindow(GetDlgItem(hWnd,IDC_DRAWALL_ENVELOPES_CHECK),FALSE);
			EnableWindow(GetDlgItem(hWnd,IDC_DRAW_VERTICES_CHECK),FALSE);


			SpinnerOff(hWnd,IDC_PAINT_STR_SPIN2,IDC_PAINT_STR2);
			SpinnerOff(hWnd,IDC_FEATHERSPIN,IDC_FEATHER);
			SpinnerOff(hWnd,IDC_SRADIUSSPIN,IDC_SRADIUS);
			SpinnerOff(hWnd,IDC_EFFECTSPIN,IDC_EFFECT);
			SpinnerOff(hWnd,IDC_ERADIUSSPIN,IDC_ERADIUS);

			SendMessage(GetDlgItem(mod->hParam,IDC_LIST1),
				LB_SETCURSEL ,mod->LastSelected,0);
			mod->ModeBoneIndex = mod->LastSelected;

  

				//set check
	
			rMenu.SetMod(mod);
			mod->ip->GetRightClickMenuManager()->Register(&rMenu);

			mod->RegisterClasses();
			mod->iParams = GetICustToolbar(GetDlgItem(hWnd,IDC_BONE_TOOLBAR));
			mod->iParams->SetBottomBorder(FALSE);	
			mod->iParams->SetImage(hParamImages);

			mod->iParams->AddTool(
					ToolButtonItem(CTB_CHECKBUTTON,
					13, 12, 13, 12, 16, 15, 23, 22, ID_ABSOLUTE));
			mod->iParams->AddTool(
					ToolButtonItem(CTB_CHECKBUTTON,
					14, 15, 14, 15, 16, 15, 23, 22, ID_DRAW_ENVELOPE));
			mod->iParams->AddTool(
					ToolButtonItem(CTB_PUSHBUTTON,
					0, 0, 1, 1, 16, 15, 23, 22, ID_FALLOFF));
			mod->iParams->AddTool(
					ToolButtonItem(CTB_PUSHBUTTON,
					16, 16, 16, 16, 16, 15, 23, 22, ID_COPY));
			mod->iParams->AddTool(
					ToolButtonItem(CTB_PUSHBUTTON,
					17, 17, 19, 19, 16, 15, 23, 22, ID_PASTE));

	
//			mod->iLock    = mod->iParams->GetICustButton(ID_LOCK);
			mod->iAbsolute= mod->iParams->GetICustButton(ID_ABSOLUTE);
			mod->iEnvelope= mod->iParams->GetICustButton(ID_DRAW_ENVELOPE);
			mod->iFalloff = mod->iParams->GetICustButton(ID_FALLOFF);
			mod->iCopy = mod->iParams->GetICustButton(ID_COPY);
			mod->iPaste = mod->iParams->GetICustButton(ID_PASTE);

//			mod->iLock->SetTooltip(TRUE,GetString(IDS_PW_LOCK));
			mod->iAbsolute->SetTooltip(TRUE,GetString(IDS_PW_ABSOLUTE));
			mod->iEnvelope->SetTooltip(TRUE,GetString(IDS_PW_ENVELOPE));
			mod->iFalloff->SetTooltip(TRUE,GetString(IDS_PW_FALLOFF));
			mod->iCopy->SetTooltip(TRUE,GetString(IDS_PW_COPY));
			mod->iPaste->SetTooltip(TRUE,GetString(IDS_PW_PASTE));

			if (mod->CopyBuffer.CList.Count() == 0) mod->iPaste->Enable(FALSE);

			FlyOffData fdata1[] = {
				{ 8,  8,  9,  9},
				{10, 10, 11, 11}
				};
			int lock=0;
//			mod->iLock->SetFlyOff(2,fdata1,mod->ip->GetFlyOffTime(),lock,FLY_DOWN);

//			FlyOffData fdata2[] = {
//				{13, 13, 13, 13},
//				{12, 12, 12, 12}
//				};
			int absolute=0;
//			mod->iAbsolute->SetFlyOff(2,fdata2,mod->ip->GetFlyOffTime(),absolute,FLY_DOWN);

			FlyOffData fdata3[] = {
				{14, 14, 14, 14},
				{15, 15, 15, 15}
				};
			int envelope=0;
//			mod->iEnvelope->SetFlyOff(2,fdata3,mod->ip->GetFlyOffTime(),envelope,FLY_DOWN);

			FlyOffData fdata4[] = {
				{ 0,  0,  1,  1},
				{ 2,  2,  3,  3},
				{ 4,  4,  5,  5},
				{ 6,  6,  7,  7},
				};
			int falloff=0;
			mod->iFalloff->SetFlyOff(4,fdata4,mod->ip->GetFlyOffTime(),falloff,FLY_DOWN);

			if (mod->ModeBoneIndex < (mod->BoneData.Count()-1))
				{
				mod->UpdatePropInterface();

				}
 // bug fix 206160 and 207093 9/8/99	watje
			if (mod->iAbsolute!=NULL)
				mod->iAbsolute->Disable();
			if (mod->iEnvelope!=NULL)
				mod->iEnvelope->Disable();
			if (mod->iFalloff!=NULL)
				mod->iFalloff->Disable();
			if (mod->iCopy!=NULL)
				mod->iCopy->Disable();
			if (mod->iPaste!=NULL)
				mod->iPaste->Disable();




			break;
			}
		case WM_CUSTEDIT_ENTER :
			switch (LOWORD(wParam)) {
			case IDC_ERADIUS:
				if (Animating())
					{
					Control *c = mod->pblock_param->GetController(PB_ERADIUS);
					IKeyControl *ikc = GetKeyControlInterface(c);
					float r;
					mod->pblock_param->GetValue(PB_ERADIUS,mod->ip->GetTime(),r,FOREVER);
					ikc->SetNumKeys(0);
					mod->pblock_param->SetValue(PB_ERADIUS,0,r);
					mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());
					}
				else
					{
					mod->updateP = TRUE;
					mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
					mod->ip->RedrawViews(mod->ip->GetTime());
					}
				break;
			case IDC_EFFECT:
				{
				ModContextList mcList;		
				INodeTab nodes;

				mod->ip->GetModContexts(mcList,nodes);
				int objects = mcList.Count();
				for ( int i = 0; i < objects; i++ ) 
					{
					BoneModData *bmd = (BoneModData*)mcList[i]->localData;
					bmd->effect = -1.0f;
					}
				mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
				mod->ip->RedrawViews(mod->ip->GetTime());


				}
				break;

			}

		case CC_SPINNER_BUTTONDOWN:
			switch (LOWORD(wParam)) {
			case IDC_EFFECTSPIN:
				mod->HoldWeights();
				break;
/*			case IDC_ERADIUSSPIN:
				theHold.SuperBegin();
				break;
*/

			}
			break;

		case CC_SPINNER_CHANGE:
			switch( LOWORD(wParam) ) {
				case IDC_ERADIUSSPIN:
					if (Animating())
						{
						Control *c = mod->pblock_param->GetController(PB_ERADIUS);
						IKeyControl *ikc = GetKeyControlInterface(c);
						float r;
						mod->pblock_param->GetValue(PB_ERADIUS,mod->ip->GetTime(),r,FOREVER);
						ikc->SetNumKeys(0);
						mod->pblock_param->SetValue(PB_ERADIUS,0,r);
//					mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
//					mod->ip->RedrawViews(mod->ip->GetTime());
						}
					break;
				}

			break;


		case CC_SPINNER_BUTTONUP:
			switch( LOWORD(wParam) ) {
				case IDC_EFFECTSPIN:
					mod->AcceptWeights(HIWORD(wParam));
					break;
				case IDC_ERADIUSSPIN:
					if (HIWORD(wParam))
						{
						if (Animating())
							{
							Control *c = mod->pblock_param->GetController(PB_ERADIUS);
							IKeyControl *ikc = GetKeyControlInterface(c);
							float r;
							mod->pblock_param->GetValue(PB_ERADIUS,mod->ip->GetTime(),r,FOREVER);
							ikc->SetNumKeys(0);
							mod->pblock_param->SetValue(PB_ERADIUS,0,r);
							mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
							mod->ip->RedrawViews(mod->ip->GetTime());
							}
						else
							{
							mod->updateP = TRUE;
							mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
							mod->ip->RedrawViews(mod->ip->GetTime());
							}

						}
					break;



				}
			break;

		case WM_COMMAND:

			switch (LOWORD(wParam)) 
				{
				case IDC_ALWAYSDEFORM_CHECK:
//				block_param->GetValue(PB_ALWAYS_DEFORM,t,AlwaysDeform,valid);
				mod->forceRecomuteBaseNode = TRUE;
				mod->UpdateEndPointDelta();
				break;

	
				case IDC_CREATE_CROSS_SECTION:
					{
					mod->StartCrossSectionMode(0);
					break;
					}
				case IDC_PAINT:
					{

					mod->StartPaintMode();
					break;
					}

				case IDC_BUILD_FALLOFF:
					{
//					mod->BuildFalloff();
					break;
					}
				case IDC_SELECT_FLEX:
					{
//					mod->SelectFlexibleVerts();
					break;
					}
				case IDC_CREATE_REMOVE_SECTION:
					{
					mod->RemoveCrossSection();
					break;
					}
				case IDC_UNLOCK_VERTS:
					{
					mod->unlockVerts = TRUE;
					mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
/*
					for (i=0;i<mod->bmd->VertexDataCount;i++)
						{
						if (mod->sel[i])
							mod->bmd->VertexData[i]->modified = FALSE;
						}
*/
					break;


					}
				case IDC_RESET_ALL:
					{
					mod->unlockBone = TRUE;
					mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
					break;
					}
				case IDC_ADD:
					{
					mod->ip->DoHitByNameDialog(new DumpHitDialog(mod));

					break;
					}

				case ID_FALLOFF:
					{
					if (mod->BoneData.Count() > 0)
						{

//watje 9-7-99  198721 
						mod->Reevaluate(TRUE);
						int foff = mod->iFalloff->GetCurFlyOff();

						if (foff == 0)
							mod->BoneData[mod->ModeBoneIndex].FalloffType = BONE_FALLOFF_X_FLAG;
						else if (foff == 1)
							mod->BoneData[mod->ModeBoneIndex].FalloffType = BONE_FALLOFF_SINE_FLAG;
						else if (foff == 2)
							mod->BoneData[mod->ModeBoneIndex].FalloffType = BONE_FALLOFF_3X_FLAG;
						else if (foff == 3)
							mod->BoneData[mod->ModeBoneIndex].FalloffType = BONE_FALLOFF_X3_FLAG;
						mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);

						}

					break;
					}

				case ID_COPY:
					{
					mod->CopyBone();
					mod->iPaste->Enable(TRUE);
					break;
					}
				case ID_PASTE:
					{
					if (mod->BoneData.Count() > 0)
						{
						theHold.Begin();
						theHold.Put(new PasteRestore(mod));

						mod->PasteBone();
						theHold.Accept(GetString(IDS_PW_PASTE));
						}
					break;
					}



				case ID_ABSOLUTE:
					{
					int abso = mod->iAbsolute->IsChecked();
						//mod->iAbsolute->GetCurFlyOff();
					if (mod->BoneData.Count() > 0)
						{
						if (abso==0)

							mod->BoneData[mod->ModeBoneIndex].flags |= BONE_ABSOLUTE_FLAG;

							else mod->BoneData[mod->ModeBoneIndex].flags &= ~BONE_ABSOLUTE_FLAG;
						mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);

						}
					break;
					}
				case ID_DRAW_ENVELOPE:
//				case IDC_DISPLAY_ENVELOPE_CHECK:
					{
					if (mod->BoneData.Count() > 0)
						{

						int disp = mod->iEnvelope->IsChecked();//GetCurFlyOff();
						if (disp)
							{
							mod->BoneData[mod->ModeBoneIndex].flags |= BONE_DRAW_ENVELOPE_FLAG;
							}
							else 
							{
							mod->BoneData[mod->ModeBoneIndex].flags &= ~BONE_DRAW_ENVELOPE_FLAG;
							}
						mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);


						}
					break;
					}
				case IDC_REMOVE:
					{
					mod->RemoveBone();
					GetSystemSetting(SYSSET_CLEAR_UNDO);
					SetSaveRequiredFlag(TRUE);

					break;
					}
				case IDC_FILTER_VERTICES_CHECK:
					{
//					mod->ClearVertexSelections();
					break;
					}
				case IDC_FILTER_BONES_CHECK:
					{
					mod->ClearBoneEndPointSelections();
					break;
					}
				case IDC_FILTER_ENVELOPES_CHECK:
					{
					mod->ClearEnvelopeSelections();
					break;
					}
				case IDC_LIST1:
					{
					if (HIWORD(wParam)==LBN_SELCHANGE) {
						int fsel;

//						if (theHold.Holding() ) 
						if (mod->ip)
							{
							ModContextList mcList;		
							INodeTab nodes;

							mod->ip->GetModContexts(mcList,nodes);
							int objects = mcList.Count();

							for ( int i = 0; i < objects; i++ ) 
								{
								BoneModData *bmd = (BoneModData*)mcList[i]->localData;

								theHold.Begin();
								theHold.Put(new SelectionRestore(mod,bmd));
								theHold.Accept(GetString(IDS_PW_SELECT));
								}
							}

						fsel = SendMessage(
							GetDlgItem(hWnd,IDC_LIST1),
							LB_GETCURSEL,0,0);	
						int sel = mod->ConvertSelectedListToBoneID(fsel);

						mod->cacheValid = TRUE;
						mod->ModeBoneIndex = sel;
						mod->ModeBoneEndPoint = -1;
						mod->ModeBoneEnvelopeIndex = -1;
						mod->ModeBoneEnvelopeSubType = -1;
						mod->NotifyDependents(FOREVER,PART_GEOM,REFMSG_CHANGE);
						mod->LastSelected = sel;

						mod->EnableRadius(FALSE);
						
						mod->UpdatePropInterface();


						}
					break;
					}



				}
			break;


		}
	return FALSE;
	}

