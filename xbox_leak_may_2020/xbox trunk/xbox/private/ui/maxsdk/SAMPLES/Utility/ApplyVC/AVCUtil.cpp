/**********************************************************************
 *<
	FILE:			AVCUtil.cpp
	DESCRIPTION:	Vertex Color Utility
	CREATED BY:		Christer Janson
	HISTORY:		Created Monday, June 02, 1997

 *>	Copyright (c) 1997 Kinetix, All Rights Reserved.
 **********************************************************************/

#include "ApplyVC.h"
#include "modstack.h"

static ApplyVCUtil theApplyVC;

#define NO_UPDATE -1

#define CFGFILE		_T("APPLYVC.CFG")
#define CFGVERSION	3

class ApplyVCClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return &theApplyVC;}
	const TCHAR *	ClassName() {return GetString(IDS_AVCU_CNAME);}
	SClass_ID		SuperClassID() {return UTILITY_CLASS_ID;}
	Class_ID		ClassID() {return APPLYVC_UTIL_CLASS_ID;}
	const TCHAR* 	Category() {return _T("");}
};

static ApplyVCClassDesc ApplyVCUtilDesc;
ClassDesc* GetApplyVCUtilDesc() {return &ApplyVCUtilDesc;}

static BOOL CALLBACK ApplyVCDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
			theApplyVC.Init(hWnd);
			break;

		case WM_DESTROY:
			theApplyVC.Destroy(hWnd);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_CLOSEBUTTON:
					theApplyVC.iu->CloseUtility();
					break;
				case IDC_VCUTIL_APPLY:
					theApplyVC.ApplySelected();
					break;
				case IDC_VCUTIL_UPDATEALL:
					theApplyVC.UpdateAll();
					break;
				case IDC_VCUTIL_SCENELIGHTS:
					EnableWindow(GetDlgItem(hWnd, IDC_CASTSHADOWS), TRUE);
					theApplyVC.lastLightModel = LOWORD(wParam);
					break;
				case IDC_VCUTIL_DIFFUSE:
					EnableWindow(GetDlgItem(hWnd, IDC_CASTSHADOWS), FALSE);
					theApplyVC.lastLightModel = LOWORD(wParam);
					break;
				case IDC_MIX:
					theApplyVC.lastMix = IsDlgButtonChecked(hWnd, IDC_MIX);
					break;
				case IDC_CASTSHADOWS:
					theApplyVC.lastShadow = IsDlgButtonChecked(hWnd, IDC_CASTSHADOWS);
					break;
				case IDC_USEMAPS:
					theApplyVC.lastUseMaps = IsDlgButtonChecked(hWnd, IDC_USEMAPS);
					break;
			}
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			theApplyVC.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return FALSE;
	}
	return TRUE;
}	

ApplyVCUtil::ApplyVCUtil()
{
	iu = NULL;
	ip = NULL;	
	hPanel = NULL;
	lastLightModel = IDC_VCUTIL_SCENELIGHTS;
	lastMix = BST_UNCHECKED;
	lastShadow = BST_UNCHECKED;
	lastUseMaps = BST_CHECKED;
}

ApplyVCUtil::~ApplyVCUtil()
{
}

void ApplyVCUtil::SaveOptions()
	{
	TSTR f;

	f = ip->GetDir(APP_PLUGCFG_DIR);
	f += _T("\\");
	f += CFGFILE;

	FILE* out = fopen(f, "w");
	if (out) {
		fputc(CFGVERSION, out);
		fprintf(out, "%d\n", lastLightModel);
		fprintf(out, "%d\n", lastMix);
		fprintf(out, "%d\n", lastShadow);
		fprintf(out, "%d\n", lastUseMaps);
		fclose(out);
		}
	}

void ApplyVCUtil::LoadOptions()
	{
	TSTR f;

	f = ip->GetDir(APP_PLUGCFG_DIR);
	f += _T("\\");
	f += CFGFILE;

	FILE* in = fopen(f, "r");
	if (in) {
		int version;
		version = fgetc(in);	// Version
		fscanf(in, "%d\n", &lastLightModel);
		fscanf(in, "%d\n", &lastMix);
		fscanf(in, "%d\n", &lastShadow);
		fscanf(in, "%d\n", &lastUseMaps);
		fclose(in);
		}
	}

void ApplyVCUtil::BeginEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = iu;
	this->ip = ip;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_VCUTIL_PANEL),
		ApplyVCDlgProc,
		GetString(IDS_AVCU_PANELTITLE),
		0);
	LoadOptions();
	CheckRadioButton(hPanel, IDC_VCUTIL_DIFFUSE, IDC_VCUTIL_SCENELIGHTS, lastLightModel);
	CheckDlgButton(hPanel, IDC_MIX, lastMix);
	CheckDlgButton(hPanel, IDC_CASTSHADOWS, lastShadow);
	CheckDlgButton(hPanel, IDC_USEMAPS, lastUseMaps);

	EnableWindow(GetDlgItem(hPanel, IDC_CASTSHADOWS), lastLightModel == IDC_VCUTIL_SCENELIGHTS);
}
	
void ApplyVCUtil::EndEditParams(Interface *ip,IUtil *iu) 
{
	SaveOptions();

	this->iu = NULL;
	this->ip = NULL;
	ip->DeleteRollupPage(hPanel);
	hPanel = NULL;
}

void ApplyVCUtil::Init(HWND hWnd)
{
}

void ApplyVCUtil::Destroy(HWND hWnd)
{
}

//***************************************************************************
//
// Apply / Update selected
//
//***************************************************************************

void ApplyVCUtil::ApplySelected()
{
	int		lightModel = LIGHT_SCENELIGHT;
	BOOL	mixColors = FALSE;
	BOOL	castShadows = FALSE;
	BOOL	useMaps = FALSE;

	if (IsDlgButtonChecked(hPanel, IDC_VCUTIL_DIFFUSE))
		lightModel = LIGHT_DIFFUSE;

	if (IsDlgButtonChecked(hPanel, IDC_MIX))
		mixColors = TRUE;

	if (IsDlgButtonChecked(hPanel, IDC_CASTSHADOWS))
		castShadows = TRUE;

	if (IsDlgButtonChecked(hPanel, IDC_USEMAPS))
		useMaps = TRUE;

	for (int i=0; i<ip->GetSelNodeCount(); i++)
	{
		if (!ApplyNode(ip->GetSelNode(i), lightModel, mixColors, castShadows, useMaps)) {
			break;
		}
	}

	ip->RedrawViews(ip->GetTime());
}

class ProgCallback : public EvalColProgressCallback {
public:
	ProgCallback(ApplyVCUtil* util) { u = util; }
	BOOL progress(float prog) {
		static int p=-1;
		if (p!=int(prog*100.f)) {
			// Eliminate ugly flashing of progress bar by only
			// calling update if the value has changed.
			((ApplyVCUtil*)u)->ip->ProgressUpdate(int(prog*100.0f));
			p = int(prog*100.0f);
		}
		return ((ApplyVCUtil*)u)->ip->GetCancel();
	}

private:
	UtilityObj* u;
};

DWORD WINAPI dummy(LPVOID arg) {
	return(0);
}

BOOL ApplyVCUtil::ApplyNode(INode* node, int lightModel, BOOL bMix, BOOL castShadows, BOOL useMaps)
{
	ProgCallback fn(this);

	if (!node)
		return TRUE;

	ObjectState os = node->EvalWorldState(ip->GetTime());
	if (!os.obj)
		return TRUE;

	if (!os.obj->CanConvertToType(triObjectClassID))
		return TRUE;

	FaceColorTab faceColors;
	ColorTab mixedVertexColors;

	if (lightModel != NO_UPDATE) {
		ip->ProgressStart(GetString(IDS_AVCU_PROGRESS), TRUE, dummy, NULL);

		if (bMix) {
			if (!calcMixedVertexColors(node, ip->GetTime(), lightModel, castShadows, useMaps, mixedVertexColors, &fn)) {
				ip->ProgressEnd();
				return FALSE;
			}
		}
		else {
			if (!calcFaceColors(node, ip->GetTime(), lightModel, castShadows, useMaps, faceColors, &fn)) {
				ip->ProgressEnd();
				return FALSE;
			}
		}

		ip->ProgressEnd();
	}


	ApplyVCMod* mod;
	
	if (!(mod = (ApplyVCMod*)GetModifier(node, APPLYVC_MOD_CLASS_ID))) {
		mod = (ApplyVCMod*)CreateInstance(OSM_CLASS_ID, APPLYVC_MOD_CLASS_ID);

		Object* obj = node->GetObjectRef();
		IDerivedObject* dobj = CreateDerivedObject(obj);
		dobj->AddModifier(mod);
		node->SetObjectRef(dobj);
	}

	if (lightModel != NO_UPDATE) {
		if (bMix) {
			mod->SetMixedColors(mixedVertexColors);
		}
		else {
			mod->SetColors(faceColors);
		}
	}

//	for (int i=0; i<vertexColors.Count(); i++) {
//		delete vertexColors[i];
//	}
	for (int i=0; i<faceColors.Count(); i++) {
		delete faceColors[i];
	}

	return TRUE;
}

//***************************************************************************
//
// Update all
//
//***************************************************************************

void ApplyVCUtil::UpdateAll()
{
	int		lightModel = LIGHT_SCENELIGHT;
	BOOL	mixColors = FALSE;
	BOOL	castShadows = FALSE;
	BOOL	useMaps = FALSE;

	if (IsDlgButtonChecked(hPanel, IDC_VCUTIL_DIFFUSE))
		lightModel = LIGHT_DIFFUSE;

	if (IsDlgButtonChecked(hPanel, IDC_MIX))
		mixColors = TRUE;

	if (IsDlgButtonChecked(hPanel, IDC_CASTSHADOWS))
		castShadows = TRUE;

	if (IsDlgButtonChecked(hPanel, IDC_USEMAPS))
		useMaps = TRUE;

	CheckForAndMakeUnique(lightModel, mixColors, castShadows, useMaps);
	
	for (int i=0; i<ip->GetRootNode()->NumberOfChildren(); i++)
	{
		if (!UpdateAllEnum(ip->GetRootNode()->GetChildNode(i), lightModel, mixColors, castShadows, useMaps)) {
			break;
		}
	}

	ip->RedrawViews(ip->GetTime());
}

BOOL ApplyVCUtil::UpdateAllEnum(INode* node, int lightModel, BOOL bMix, BOOL castShadows, BOOL useMaps)
{
	for (int i=0; i<node->NumberOfChildren(); i++) {
		if (!UpdateAllEnum(node->GetChildNode(i), lightModel, bMix, castShadows, useMaps)) {
			return FALSE;
		}
	}

	if (GetModifier(node, APPLYVC_MOD_CLASS_ID)) {
		ApplyNode(node, lightModel, bMix, castShadows, useMaps);
	}

	return TRUE;
}

//***************************************************************************
//
// Analyze scene for instanced modifiers and instanced nodes sharing the
// modifier
//
//***************************************************************************

void ApplyVCUtil::CheckForAndMakeUnique(int lightModel, BOOL bMix, BOOL castShadows, BOOL useMaps)
{
	ModInfoTab	modTab;
	
	CheckAllNodesEnum(ip->GetRootNode(), modTab);

	for (int i=0; i<modTab.Count(); i++) {
		if (modTab[i]->nodes.Count() > 1) {
			MakeUnique(modTab[i], lightModel, bMix, castShadows, useMaps);
		}
		delete modTab[i];
	}
	modTab.ZeroCount();
	modTab.Shrink();
}

void ApplyVCUtil::CheckAllNodesEnum(INode* node, ModInfoTab& modTab)
{
	int i;
	BOOL found = FALSE;
	Modifier* mod = GetModifier(node, APPLYVC_MOD_CLASS_ID);
	if (mod) {
		for (i=0; i<modTab.Count(); i++) {
			if (modTab[i]->mod == mod) {
				modTab[i]->nodes.Append(1, &node, 5);
				found = TRUE;
			}
		}
		if (!found) {
			ModInfo* mh = new ModInfo;
			mh->mod = mod;
			mh->nodes.Append(1, &node, 5);
			modTab.Append(1, &mh, 5);
		}
	}
	int numChildren = node->NumberOfChildren();
	for (i=0; i<numChildren; i++) {
		CheckAllNodesEnum(node->GetChildNode(i), modTab);
	}
}

class DerivedObjTab : public Tab<IDerivedObject*> {
public:
	BOOL InList(IDerivedObject *dob) {
		for (int i=0; i<Count(); i++) {
			if ((*this)[i]==dob) return TRUE;
		}
		return FALSE;
	}
};

BOOL ApplyVCUtil::MakeUnique(ModInfo* mh, int lightModel, BOOL bMix, BOOL castShadows, BOOL useMaps)
{
	int i;
	
	if (!mh) {
		return FALSE;
	}

	// Step through all the nodes sharing this modifier	
	// and delete the modifier.
	// The modifier will be applied later when all the nodes
	// are finally updated.

	int nodeCount = mh->nodes.Count();
	for (i=0; i<nodeCount; i++) {
	    Object* obj = mh->nodes[i]->GetObjectRef();

	    if (!obj)
	        return FALSE;

	    while (obj && (obj->SuperClassID() == GEN_DERIVOB_CLASS_ID)) {
	        IDerivedObject* dobj = (IDerivedObject*)obj;
	        int m;
	        int numMods = dobj->NumModifiers();

			// This is really only needed for the first node
			// since they all share the same DerivedObject
	        for (m=0; m<numMods; m++) {
	            Modifier* mod = dobj->GetModifier(m);
	            if (mod) {
	                dobj->DeleteModifier(m);
	            }
	        }

			if (dobj->NumModifiers() == 0 && !dobj->TestAFlag(A_DERIVEDOBJ_DONTDELETE)) {
				obj = dobj->GetObjRef();
				obj->TransferReferences(dobj);
				dobj->SetAFlag(A_LOCK_TARGET);
				dobj->NotifyDependents(FOREVER,0,REFMSG_SUBANIM_STRUCTURE_CHANGED);
				obj->NotifyDependents(FOREVER,0,REFMSG_SUBANIM_STRUCTURE_CHANGED);
				dobj->ClearAFlag(A_LOCK_TARGET);
				dobj->MaybeAutoDelete();
			}
			else {
				obj = dobj->GetObjRef();
			}
	    }

		ApplyNode(mh->nodes[i], NO_UPDATE, bMix, castShadows, useMaps);
	}

	return TRUE;
}


//***************************************************************************
//
// Utility methods
//
//***************************************************************************

// Traverse the pipeline for this node and return the first modifier with a
// specified ClassID

Modifier* ApplyVCUtil::GetModifier(INode* node, Class_ID modCID)
{
	Object* obj = node->GetObjectRef();

	if (!obj)
		return NULL;

	ObjectState os = node->EvalWorldState(0);
	if (os.obj && os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID) {
		return NULL;
	}

	// For all derived objects (can be > 1)
	while (obj && (obj->SuperClassID() == GEN_DERIVOB_CLASS_ID)) {
		IDerivedObject* dobj = (IDerivedObject*)obj;
		int m;
		int numMods = dobj->NumModifiers();
		// Step through all modififers and verify the class id
		for (m=0; m<numMods; m++) {
			Modifier* mod = dobj->GetModifier(m);
			if (mod) {
				if (mod->ClassID() == modCID) {
					// Match! Return it
					return mod;
				}
			}
		}
		obj = dobj->GetObjRef();
	}

	return NULL;
}
