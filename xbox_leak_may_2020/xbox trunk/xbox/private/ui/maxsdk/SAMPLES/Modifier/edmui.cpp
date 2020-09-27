/**********************************************************************
 *<
	FILE: edmui.cpp

	DESCRIPTION:  Edit Mesh OSM	UI code

	CREATED BY: Rolf Berteig

	HISTORY: created 1 September, 1995

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "mods.h"
#include "MeshDLib.h"
#include "editmesh.h"

static BOOL CALLBACK SelectDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL CALLBACK AffectRegionDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL CALLBACK SurfaceDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL CALLBACK GeomDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static float weldThresh = 0.1f;
static float planarFaceThresh = 45.0f;
static float explodeThresh  = 24.0f;
static float autoEdgeThresh  = 24.0f;
static float autoSmoothThresh  = 45.0f;
static int autoEdgeType = 0;	// set&clear
static float tessTens       = 25.0f;
static BOOL expObj          = TRUE;
static BOOL edgeTes         = TRUE;
static int sbmParams[2]     = {1,1};
static DWORD sbsParams[3]   = {1,1,0};
static float lastEdgeThresh = 30.0f;
static int selDeltaR		= 10;
static int selDeltaG		= 10;
static int selDeltaB		= 10;
static Point3 selByColor    = Point3(1,1,1);
static int cloneTo = IDC_EM_CLONE_ELEM;

void ResetEditMeshUI() {
	weldThresh     = 0.1f;
	planarFaceThresh = 45.0f;
	explodeThresh  = 24.0f;
	autoEdgeThresh = 24.0f;
	autoSmoothThresh = 45.0f;
	autoEdgeType = 0;
	tessTens       = 25.0f;
	expObj         = TRUE;
	edgeTes        = TRUE;
	sbmParams[0]   = 1;
	sbmParams[1]   = 1;
	sbsParams[0]   = 1;
	sbsParams[1]   = 1;
	sbsParams[2]   = 0;
	lastEdgeThresh = 30.0f;
	selDeltaR		= 10;
	selDeltaG		= 10;
	selDeltaB		= 10;
	selByColor    = Point3(1,1,1);
	cloneTo = IDC_EM_CLONE_ELEM;
}

// MeshSelImageHandler methods.
// Note: these are also used by meshsel.cpp (and declared in mods.h).
HIMAGELIST MeshSelImageHandler::LoadImages () {
	if (images ) return images;

	HBITMAP hBitmap, hMask;
	images = ImageList_Create(24, 23, ILC_COLOR|ILC_MASK, 10, 0);
	hBitmap = LoadBitmap (hInstance, MAKEINTRESOURCE(IDB_EM_SELTYPES));
	hMask = LoadBitmap (hInstance, MAKEINTRESOURCE(IDB_EM_SELMASK));
	ImageList_Add (images, hBitmap, hMask);
	DeleteObject(hBitmap);
	DeleteObject(hMask);
	return images;
}

// Local static instance.
static MeshSelImageHandler theMeshSelImageHandler;

static int SurfDlgs[] = { 0, IDD_EM_SURF_VERT, IDD_EM_SURF_EDGE,
	IDD_EM_SURF_FACE, IDD_EM_SURF_FACE, IDD_EM_SURF_FACE };

void EditMeshMod::UpdateSurfType () {
	if (!hGeom) return;
	if (!hSurf) {
		if (selLevel == SL_OBJECT) return;
		HWND hKeyFocus = GetFocus ();
		hSurf = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(SurfDlgs[selLevel]),
			SurfaceDlgProc, GetString(IDS_RB_EDITSURFACE), (LPARAM)this, rsSurf ? 0 : APPENDROLL_CLOSED);
		SetFocus (hKeyFocus);
		return;
	}
	bool wasFace = GetDlgItem (hSurf, IDC_SMOOTH_GRP1) ? TRUE : FALSE;
	bool wasEdge = GetDlgItem (hSurf, IDC_EM_EDGE_VIS) ? TRUE : FALSE;
	bool wasVert = GetDlgItem (hSurf, IDC_EM_VERT_SELCOLOR) ? TRUE : FALSE;
	if (wasVert && (selLevel == SL_VERTEX)) return;
	if (wasEdge && (selLevel == SL_EDGE)) return;
	if (wasFace && (selLevel >= SL_FACE)) return;

	rsSurf = IsRollupPanelOpen (hSurf);
	ip->DeleteRollupPage (hSurf);
	if (selLevel == SL_OBJECT) hSurf = NULL;
	else {
		HWND hKeyFocus = GetFocus ();
		hSurf = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(SurfDlgs[selLevel]),
			SurfaceDlgProc, GetString(IDS_RB_EDITSURFACE), (LPARAM)this, rsSurf ? 0 : APPENDROLL_CLOSED);
		SetFocus (hKeyFocus);
	}
}

static int butIDs[] = { 0, IDC_SELVERTEX, IDC_SELEDGE, IDC_SELFACE, IDC_SELPOLY, IDC_SELELEMENT };
void EditMeshMod::RefreshSelType () {
	ICustToolbar *iToolbar = GetICustToolbar(GetDlgItem(hSel,IDC_EM_SELTYPE));
	ICustButton *but;
	for (int i=1; i<6; i++) {
		but = iToolbar->GetICustButton (butIDs[i]);
		but->SetCheck (selLevel==i);
		ReleaseICustButton (but);
	}
	ReleaseICustToolbar(iToolbar);
	SetSelDlgEnables ();
	SetARDlgEnables ();
	SetGeomDlgEnables ();
	UpdateSurfType ();
}

static updateNumSel = TRUE;
void EditMeshMod::InvalidateNumberSelected () {
	if (!hSel) return;
	InvalidateRect (hSel, NULL, FALSE);
	updateNumSel = TRUE;
}

void EditMeshMod::SetNumSelLabel() {
	static TSTR buf;
	if (!hSel) return;
	if (!updateNumSel) {
		SetDlgItemText (hSel, IDC_EM_NUMBER_SEL, buf);
		return;
	}
	updateNumSel = FALSE;

	if (selLevel == SL_OBJECT) {
		buf.printf (GetString (IDS_EM_OBJECT_SEL));
		SetDlgItemText(hSel, IDC_EM_NUMBER_SEL, buf);
		return;
	}
	ModContextList mcList;	
	INodeTab nodes;	
	ip->GetModContexts(mcList,nodes);
	int i, num=0, where=0, thissel;
	for (i=0; i<mcList.Count(); i++) {
		EditMeshData *meshData = (EditMeshData*)mcList[i]->localData;
		if (!meshData) continue;

		switch (selLevel) {
		case SL_VERTEX:
			thissel = meshData->mdelta.vsel.NumberSet();
			break;
		case SL_FACE:
		case SL_POLY:
		case SL_ELEMENT:
			thissel = meshData->mdelta.fsel.NumberSet();
			break;
		case SL_EDGE:
			thissel= meshData->mdelta.esel.NumberSet();
			break;
		}
		if (thissel) {
			where = i;
			num += thissel;
		}
	}

	// If we have only one element selected, which one is it?
	int which;
	if (num==1) {
		EditMeshData *meshData = (EditMeshData*) mcList[where]->localData;
		switch (selLevel) {
		case SL_VERTEX:
			for (which=0; which<meshData->mdelta.vsel.GetSize(); which++) if (meshData->mdelta.vsel[which]) break;
			break;
		case SL_FACE:
		case SL_POLY:
		case SL_ELEMENT:
			for (which=0; which<meshData->mdelta.fsel.GetSize(); which++) if (meshData->mdelta.fsel[which]) break;
			break;
		case SL_EDGE:
			for (which=0; which<meshData->mdelta.esel.GetSize(); which++) if (meshData->mdelta.esel[which]) break;
			break;
		}
	}

	switch (selLevel) {
	case SL_VERTEX:			
		if (num==1) buf.printf (GetString(IDS_EM_WHICHVERTSEL), which+1);
		else buf.printf(GetString(IDS_RB_NUMVERTSELP),num);
		break;

	case SL_FACE:
	case SL_POLY:
	case SL_ELEMENT:
		if (num==1) buf.printf (GetString(IDS_EM_WHICHFACESEL), which+1);
		else buf.printf(GetString(IDS_RB_NUMFACESELP),num);
		break;

	case SL_EDGE:
		if (num==1) buf.printf (GetString(IDS_EM_WHICHEDGESEL), which+1);
		else buf.printf(GetString(IDS_RB_NUMEDGESELP),num);
		break;
	}

	SetDlgItemText(hSel, IDC_EM_NUMBER_SEL, buf);
}

float EditMeshMod::GetPolyFaceThresh() {
	return DegToRad(planarFaceThresh);
}

// --- Begin/End Edit Params ---------------------------------

const ShortcutTableId kEMeshShortcuts = EM_SHORTCUT_ID;
static BOOL oldShowEnd;
static EMeshShortcutCB *accel=NULL;

void EditMeshMod::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev ) {
	if (!GetSystemSetting(SYSSET_ENABLE_EDITMESHMOD)) return;
	this->ip = ip;
	CreateMeshDataTempData();
	UpdateSetNames ();

	if ( !hGeom ) {
		hSel = ip->AddRollupPage (hInstance, MAKEINTRESOURCE(IDD_EM_SELECT),
			SelectDlgProc, GetString (IDS_EM_SELECTION), (LPARAM)this, rsSel ? 0 : APPENDROLL_CLOSED);
		hAR = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_EM_AFFECTREGION),
			AffectRegionDlgProc, GetString(IDS_MS_AFFECTREGION), (LPARAM)this, rsAR ? 0 : APPENDROLL_CLOSED);
		hGeom = ip->AddRollupPage (hInstance, MAKEINTRESOURCE(IDD_EM_GEOM),
			GeomDlgProc, GetString(IDS_EM_EDIT_GEOM), (LPARAM)this, rsGeom ? 0 : APPENDROLL_CLOSED);
		if (selLevel == SL_OBJECT) {
			hSurf = NULL;
		} else {
			hSurf = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(SurfDlgs[selLevel]),
				SurfaceDlgProc, GetString(IDS_RB_EDITSURFACE), (LPARAM)this, rsSurf ? 0 : APPENDROLL_CLOSED);
		}
		InvalidateNumberSelected ();
		accel = new EMeshShortcutCB(this);
		ip->ActivateShortcutTable (accel, kEMeshShortcuts);
	} else {
		SetWindowLong( hGeom, GWL_USERDATA, (LONG)this );
		SetWindowLong (hSel, GWL_USERDATA, (LONG) this);
	}
	SetFlag (EM_EDITING);

	// Create sub object editing modes.
	moveMode       = new MoveModBoxCMode(this,ip);
	rotMode        = new RotateModBoxCMode(this,ip);
	uscaleMode     = new UScaleModBoxCMode(this,ip);
	nuscaleMode    = new NUScaleModBoxCMode(this,ip);
	squashMode     = new SquashModBoxCMode(this,ip);
	selectMode     = new SelectModBoxCMode(this,ip);
	weldVertMode   = new WeldVertCMode(this,ip);
	createVertMode = new CreateVertCMode(this,ip);
	createFaceMode  = new CreateFaceCMode(this,ip);
	divideEdgeMode = new DivideEdgeCMode (this, ip);
	turnEdgeMode   = new TurnEdgeCMode (this, ip);
	attachPickMode = new AttachPickMode(this,ip);
	extrudeMode    = new ExtrudeCMode(this,ip);
	bevelMode = new BevelCMode (this, ip);
	chamferMode = new ChamferCMode (this, ip);
	divideFaceMode = new DivideFaceCMode (this, ip);
	flipMode = new FlipNormCMode (this, ip);
	cutEdgeMode = new CutEdgeCMode (this, ip);

	// Add our sub object type
	TSTR type1( GetString(IDS_RB_VERTEX) );
	TSTR type2( GetString(IDS_RB_EDGE) );
	TSTR type3( GetString(IDS_RB_FACE) );
	TSTR type4(GetString (IDS_EM_POLY));
	TSTR type5 (GetString (IDS_EM_ELEMENT));
	const TCHAR *ptype[] = { type1, type2, type3, type4, type5 };
	ip->RegisterSubObjectTypes( ptype, 5 );

	// Restore the selection level.
	ip->SetSubObjectLevel(selLevel);

	// We want del key input if in sub object selection
	if (selLevel!=SL_OBJECT) {
		delEvent.SetEditMeshMod(this);
		ip->RegisterDeleteUser(&delEvent);
	}

	// Set show end result.
	oldShowEnd = ip->GetShowEndResult();
	ip->SetShowEndResult (GetFlag (EM_DISP_RESULT));

	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
	SetAFlag(A_MOD_BEING_EDITED);	
}

void EditMeshMod::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next ) {
	if (!GetSystemSetting(SYSSET_ENABLE_EDITMESHMOD)) return;
	// STEVE: are the following lines necessary?  Test.
	EndExtrude (ip->GetTime());
	EndBevel (ip->GetTime());

	if (flags&END_EDIT_REMOVEUI ) {
		if (hSurf) {
			rsSurf = IsRollupPanelOpen (hSurf);
			ip->DeleteRollupPage(hSurf);
			hSurf = NULL;
		}
		if (hGeom) {
			rsGeom = IsRollupPanelOpen (hGeom);
			ip->DeleteRollupPage (hGeom);
			hGeom = NULL;
		}
		if (hAR) {
			rsAR = IsRollupPanelOpen (hAR);
			ip->DeleteRollupPage (hAR);
			hAR = NULL;
		}
		if (hSel) {
			rsSel = IsRollupPanelOpen (hSel);
			ip->DeleteRollupPage (hSel);
			hSel = NULL;
		}
		if (accel) {
			ip->DeactivateShortcutTable (accel, kEMeshShortcuts);
			delete accel;
			accel = NULL;
		}
	} else {
		SetWindowLong( hGeom, GWL_USERDATA, 0 );
		SetWindowLong (hSel, GWL_USERDATA, 0);
	}

	// Unregister del key notification
	if (selLevel!=SL_OBJECT) ip->UnRegisterDeleteUser(&delEvent);

	// STEVE: are following lines needed?
	if ( ip->GetCommandMode()->ID() == CID_EXTRUDE ) ip->SetStdCommandMode( CID_OBJMOVE );
	if ( ip->GetCommandMode()->ID() == CID_BEVEL ) ip->SetStdCommandMode( CID_OBJMOVE );

	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_END_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF);
	ClearAFlag(A_MOD_BEING_EDITED);
	
	DeleteMeshDataTempData();

	ExitAllCommandModes ();
	if ( moveMode ) delete moveMode;
	moveMode = NULL;
	if ( rotMode ) delete rotMode;
	rotMode = NULL;
	if ( uscaleMode ) delete uscaleMode;
	uscaleMode = NULL;
	if ( nuscaleMode ) delete nuscaleMode;
	nuscaleMode = NULL;
	if ( squashMode ) delete squashMode;
	squashMode = NULL;
	if ( selectMode ) delete selectMode;
	selectMode = NULL;
	if (weldVertMode) delete weldVertMode;
	weldVertMode = NULL;
	if (createVertMode) delete createVertMode;
	createVertMode = NULL;
	if (createFaceMode) delete createFaceMode;
	createFaceMode = NULL;
	if (divideFaceMode) delete divideFaceMode;
	divideFaceMode = NULL;
	if (divideEdgeMode) delete divideEdgeMode;
	divideEdgeMode = NULL;
	if (turnEdgeMode) delete turnEdgeMode;
	turnEdgeMode = NULL;
	if (attachPickMode) delete attachPickMode;
	attachPickMode = NULL;
	if( extrudeMode ) delete extrudeMode;
	extrudeMode = NULL;
	if( bevelMode ) delete bevelMode;
	bevelMode = NULL;
	if (chamferMode) delete chamferMode;
	chamferMode = NULL;
	if (flipMode) delete flipMode;
	flipMode = NULL;
	if (cutEdgeMode) delete cutEdgeMode;
	cutEdgeMode = NULL;

	this->ip = NULL;
	ClearFlag (EM_EDITING);

	// Reset show end result
	SetFlag (EM_DISP_RESULT, ip->GetShowEndResult());
	ip->SetShowEndResult(oldShowEnd);
}

void EditMeshMod::ExitAllCommandModes (bool exSlice) {
	ip->DeleteMode (moveMode);
	ip->DeleteMode (rotMode);
	ip->DeleteMode (uscaleMode);
	ip->DeleteMode (nuscaleMode);
	ip->DeleteMode (squashMode);
	ip->DeleteMode (selectMode);
	ip->DeleteMode (weldVertMode);	
	ip->DeleteMode (createVertMode);	
	ip->DeleteMode (createFaceMode);
	ip->DeleteMode (divideFaceMode);
	ip->DeleteMode (divideEdgeMode);	
	ip->DeleteMode (turnEdgeMode);
	ip->DeleteMode (extrudeMode);
	ip->DeleteMode (bevelMode);
	ip->DeleteMode (chamferMode);
	ip->DeleteMode (flipMode);
	ip->DeleteMode (cutEdgeMode);
	ip->ClearPickMode();
	if (exSlice && sliceMode) ExitSliceMode ();
}


// -- Misc. Window procs ----------------------------------------

static int createCurveType   = IDC_EMCURVE_SMOOTH;
static int curveIgnoreHiddenEdges = TRUE;

static BOOL CALLBACK CurveNameDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static TSTR *name = NULL;

	switch (msg) {
	case WM_INITDIALOG:
		name = (TSTR*)lParam;
		SetWindowText (GetDlgItem(hWnd,IDC_EMCURVE_NAME), name->data());
		CenterWindow(hWnd,GetParent(hWnd));
		SendMessage(GetDlgItem(hWnd,IDC_EMCURVE_NAME), EM_SETSEL,0,-1);			
		CheckDlgButton(hWnd,createCurveType,TRUE);
		CheckDlgButton(hWnd,IDC_EMCURVE_IGNOREHIDDEN,curveIgnoreHiddenEdges);
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			name->Resize(GetWindowTextLength(GetDlgItem(hWnd,IDC_EMCURVE_NAME))+1);
			GetWindowText(GetDlgItem(hWnd,IDC_EMCURVE_NAME), name->data(), name->length()+1);
			if (IsDlgButtonChecked(hWnd,IDC_EMCURVE_SMOOTH)) createCurveType = IDC_EMCURVE_SMOOTH;
			else createCurveType = IDC_EMCURVE_LINEAR;
			curveIgnoreHiddenEdges = IsDlgButtonChecked(hWnd,IDC_EMCURVE_IGNOREHIDDEN);
			EndDialog(hWnd,1);
			break;
		
		case IDCANCEL:
			EndDialog(hWnd,0);
			break;
		}
		break;

	default:
		return 0;
	}
	return 1;
}

static BOOL detachToElem = FALSE;
static BOOL detachAsClone = FALSE;

static void SetDetachNameState(HWND hWnd) {
	if (detachToElem) {
		EnableWindow(GetDlgItem(hWnd,IDC_EM_DETACH_NAMELABEL),FALSE);
		EnableWindow(GetDlgItem(hWnd,IDC_EM_DETACH_NAME),FALSE);
	} else {
		EnableWindow(GetDlgItem(hWnd,IDC_EM_DETACH_NAMELABEL),TRUE);
		EnableWindow(GetDlgItem(hWnd,IDC_EM_DETACH_NAME),TRUE);
	}
}

static BOOL CALLBACK DetachDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static TSTR *name = NULL;

	switch (msg) {
	case WM_INITDIALOG:
		name = (TSTR*)lParam;
		SetWindowText(GetDlgItem(hWnd,IDC_EM_DETACH_NAME), name->data());
		CenterWindow(hWnd,GetParent(hWnd));
		SendMessage(GetDlgItem(hWnd,IDC_EM_DETACH_NAME), EM_SETSEL,0,-1);
		CheckDlgButton(hWnd,IDC_EM_DETACH_ELEM,detachToElem);
		CheckDlgButton (hWnd, IDC_EM_DETACH_CLONE, detachAsClone);
		if (detachToElem) SetFocus (GetDlgItem (hWnd, IDOK));
		else SetFocus (GetDlgItem (hWnd, IDC_EM_DETACH_NAME));
		SetDetachNameState(hWnd);
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			name->Resize (GetWindowTextLength(GetDlgItem(hWnd,IDC_EM_DETACH_NAME))+1);
			GetWindowText (GetDlgItem(hWnd,IDC_EM_DETACH_NAME),
				name->data(), name->length()+1);
			EndDialog(hWnd,1);
			break;

		case IDC_EM_DETACH_ELEM:
			detachToElem = IsDlgButtonChecked (hWnd, IDC_EM_DETACH_ELEM);
			SetDetachNameState(hWnd);
			break;

		case IDC_EM_DETACH_CLONE:
			detachAsClone = IsDlgButtonChecked (hWnd, IDC_EM_DETACH_CLONE);
			break;

		case IDCANCEL:
			EndDialog(hWnd,0);
			break;
		}
		break;

	default:
		return 0;
	}
	return 1;
}

BOOL GetDetachObjectName(Interface *ip,TSTR &name, BOOL &elem, BOOL &asClone) {
	HWND hCore = ip->GetMAXHWnd();
	name = GetString (IDS_EM_NEWOBJECTNAME);
	ip->MakeNameUnique (name);
	if (DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EM_DETACH),
			hCore, DetachDlgProc, (LPARAM)&name)) {
		elem = detachToElem;
		asClone = detachAsClone;
		return TRUE;
	} else {
		return FALSE;
	}
}

static void SetCloneNameState(HWND hWnd) {
	switch (cloneTo) {
	case IDC_EM_CLONE_ELEM:
		EnableWindow(GetDlgItem(hWnd,IDC_EM_CLONE_NAME),FALSE);
		break;
	case IDC_EM_CLONE_OBJ:
		EnableWindow(GetDlgItem(hWnd,IDC_EM_CLONE_NAME),TRUE);
		break;
	}
}

static BOOL CALLBACK CloneDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static TSTR *name = NULL;

	switch (msg) {
	case WM_INITDIALOG:
		name = (TSTR*)lParam;
		SetWindowText(GetDlgItem(hWnd,IDC_EM_CLONE_NAME), name->data());
		CenterWindow(hWnd, GetParent(hWnd));
		CheckRadioButton (hWnd, IDC_EM_CLONE_OBJ, IDC_EM_CLONE_ELEM, cloneTo);
		if (cloneTo == IDC_EM_CLONE_OBJ) {
			SetFocus(GetDlgItem(hWnd,IDC_EM_CLONE_NAME));
			SendMessage(GetDlgItem(hWnd,IDC_EM_CLONE_NAME), EM_SETSEL,0,-1);
		} else SetFocus (GetDlgItem (hWnd, IDOK));
		SetCloneNameState(hWnd);
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			name->Resize (GetWindowTextLength(GetDlgItem(hWnd,IDC_EM_CLONE_NAME))+1);
			GetWindowText (GetDlgItem(hWnd,IDC_EM_CLONE_NAME),
				name->data(), name->length()+1);
			EndDialog(hWnd,1);
			break;

		case IDCANCEL:
			EndDialog (hWnd, 0);
			break;

		case IDC_EM_CLONE_ELEM:
		case IDC_EM_CLONE_OBJ:
			cloneTo = LOWORD(wParam);
			SetCloneNameState(hWnd);
			break;
		}
		break;

	default:
		return 0;
	}
	return 1;
}

BOOL GetCloneObjectName (Interface *ip, TSTR &name) {
	HWND hCore = ip->GetMAXHWnd();
	name = GetString(IDS_EM_NEWOBJECTNAME);
	ip->MakeNameUnique (name);
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EM_CLONE), hCore,
		CloneDlgProc, (LPARAM)&name);
	return (cloneTo==IDC_EM_CLONE_OBJ);
}

static BOOL CALLBACK ExplodeDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static TSTR *name = NULL;

	switch (msg) {
	case WM_INITDIALOG:
		name = (TSTR*)lParam;
		SetWindowText(GetDlgItem(hWnd,IDC_EM_EXPLODE_NAME), name->data());
		CenterWindow(hWnd, GetParent(hWnd));
		SetFocus(GetDlgItem(hWnd,IDC_EM_EXPLODE_NAME));
		SendMessage(GetDlgItem(hWnd,IDC_EM_EXPLODE_NAME), EM_SETSEL,0,-1);
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			name->Resize (GetWindowTextLength(GetDlgItem(hWnd,IDC_EM_EXPLODE_NAME))+1);
			GetWindowText (GetDlgItem(hWnd,IDC_EM_EXPLODE_NAME),
				name->data(), name->length()+1);
			EndDialog(hWnd,1);
			break;

		case IDCANCEL:
			EndDialog(hWnd,0);
			break;
		}
		break;

	default:
		return 0;
	}
	return 1;
}

BOOL GetExplodeObjectName (HWND hCore, TSTR &name) {
	name = GetString(IDS_EM_NEWOBJECTNAME);
	if (DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EM_EXPLODE),
			hCore, ExplodeDlgProc, (LPARAM)&name)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void EditMeshMod::SetSelDlgEnables() {
	if (!hSel) return;
	BOOL fac = (selLevel >= SL_FACE);
	BOOL poly = (selLevel == SL_POLY);
	BOOL edg = (selLevel == SL_EDGE);
	BOOL obj = (selLevel == SL_OBJECT);
	BOOL vtx = (selLevel == SL_VERTEX);

	EnableWindow (GetDlgItem (hSel, IDC_EM_SEL_BYVERT), fac||edg);
	EnableWindow (GetDlgItem (hSel, IDC_EM_IGNORE_BACKFACES), !obj);
	EnableWindow (GetDlgItem (hSel,IDC_EM_IGNORE_VISEDGE), poly);
	EnableWindow (GetDlgItem (hSel, IDC_EM_SEL_PT_TEXT), poly);
	ISpinnerControl *spin = GetISpinner(GetDlgItem(hSel,IDC_EM_PLANARSPINNER));
	spin->Enable(poly);
	ReleaseISpinner(spin);
	ICustButton *but;
	but = GetICustButton (GetDlgItem (hSel, IDC_EM_HIDE));
	but->Enable (vtx||fac);
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hSel, IDC_EM_UNHIDEALL));
	but->Enable (vtx||fac);
	ReleaseICustButton (but);

	but = GetICustButton (GetDlgItem (hSel, IDC_EM_COPYNS));
	but->Enable (!obj);
	ReleaseICustButton(but);
	but = GetICustButton (GetDlgItem (hSel, IDC_EM_PASTENS));
	but->Enable (!obj && (GetMeshNamedSelClip (namedClipLevel[selLevel]) ? TRUE : FALSE));
	ReleaseICustButton(but);
}

static BOOL CALLBACK SelectDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	EditMeshMod *em = (EditMeshMod *) GetWindowLong (hWnd, GWL_USERDATA);
	ICustToolbar *iToolbar;
	ISpinnerControl *spin;

	switch (msg) {
	case WM_INITDIALOG:
		em = (EditMeshMod*) lParam;
		em->hSel = hWnd;
		SetWindowLong(hWnd,GWL_USERDATA,lParam);			

		spin = GetISpinner(GetDlgItem(hWnd,IDC_EM_PLANARSPINNER));
		spin->SetLimits(0, 180, FALSE);
		spin->SetScale(0.1f);
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_EM_PLANAR), EDITTYPE_FLOAT);
		spin->SetValue (planarFaceThresh, FALSE);
		ReleaseISpinner(spin);

		iToolbar = GetICustToolbar(GetDlgItem(hWnd,IDC_EM_SELTYPE));
		iToolbar->SetImage (theMeshSelImageHandler.LoadImages());
		iToolbar->AddTool (ToolButtonItem(CTB_CHECKBUTTON,0,5,0,5,24,23,24,23,IDC_SELVERTEX));
		iToolbar->AddTool (ToolButtonItem(CTB_CHECKBUTTON,1,6,1,6,24,23,24,23,IDC_SELEDGE));
		iToolbar->AddTool (ToolButtonItem(CTB_CHECKBUTTON,2,7,2,7,24,23,24,23,IDC_SELFACE));
		iToolbar->AddTool (ToolButtonItem(CTB_CHECKBUTTON,3,8,3,8,24,23,24,23,IDC_SELPOLY));
		iToolbar->AddTool (ToolButtonItem(CTB_CHECKBUTTON,4,9,4,9,24,23,24,23,IDC_SELELEMENT));
		ReleaseICustToolbar(iToolbar);
		em->RefreshSelType();

		CheckDlgButton (hWnd, IDC_EM_SEL_BYVERT, em->selByVert);
		CheckDlgButton (hWnd, IDC_EM_IGNORE_BACKFACES, em->ignoreBackfaces);
		CheckDlgButton (hWnd, IDC_EM_IGNORE_VISEDGE, em->ignoreVisEdge);
		break;

	case CC_SPINNER_CHANGE:
		spin = (ISpinnerControl*)lParam;
		switch (LOWORD(wParam)) {
		case IDC_EM_PLANARSPINNER:
			planarFaceThresh = spin->GetFVal();
			break;
		}
		break;

	case WM_COMMAND:
		if (HIWORD(wParam)==1) return FALSE;	// not handling keyboard accelerators here.

		switch (LOWORD(wParam)) {
		case IDC_SELVERTEX:
			if (em->selLevel == SL_VERTEX) em->ip->SetSubObjectLevel (SL_OBJECT);
			else em->ip->SetSubObjectLevel (SL_VERTEX);
			break;

		case IDC_SELEDGE:
			if (em->selLevel == SL_EDGE) em->ip->SetSubObjectLevel (SL_OBJECT);
			else em->ip->SetSubObjectLevel (SL_EDGE);
			break;

		case IDC_SELFACE:
			if (em->selLevel == SL_FACE) em->ip->SetSubObjectLevel (SL_OBJECT);
			else em->ip->SetSubObjectLevel (SL_FACE);
			break;

		case IDC_SELPOLY:
			if (em->selLevel == SL_POLY) em->ip->SetSubObjectLevel (SL_OBJECT);
			else em->ip->SetSubObjectLevel (SL_POLY);
			break;

		case IDC_SELELEMENT:
			if (em->selLevel == SL_ELEMENT) em->ip->SetSubObjectLevel (SL_OBJECT);
			else em->ip->SetSubObjectLevel (SL_ELEMENT);
			break;

		case IDC_EM_SEL_BYVERT:
			em->selByVert = IsDlgButtonChecked(hWnd,IDC_EM_SEL_BYVERT);
			break;

		case IDC_EM_IGNORE_BACKFACES:
			em->ignoreBackfaces = IsDlgButtonChecked(hWnd,IDC_EM_IGNORE_BACKFACES);
			break;

		case IDC_EM_IGNORE_VISEDGE:
			em->ignoreVisEdge = IsDlgButtonChecked(hWnd,IDC_EM_IGNORE_VISEDGE);
			break;

		case IDC_EM_HIDE: em->ButtonOp (MopHide); break;
		case IDC_EM_UNHIDEALL: em->ButtonOp (MopUnhideAll); break;
		case IDC_EM_COPYNS: em->NSCopy(); break;
		case IDC_EM_PASTENS: em->NSPaste(); break;
		}
		break;

	case WM_PAINT:
		if (updateNumSel) em->SetNumSelLabel ();
		return FALSE;

	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->code != TTN_NEEDTEXT) break;
		LPTOOLTIPTEXT lpttt;
		lpttt = (LPTOOLTIPTEXT)lParam;				
		switch (lpttt->hdr.idFrom) {
		case IDC_SELVERTEX:
			lpttt->lpszText = GetString (IDS_RB_VERTEX);
			break;
		case IDC_SELEDGE:
			lpttt->lpszText = GetString (IDS_RB_EDGE);
			break;
		case IDC_SELFACE:
			lpttt->lpszText = GetString(IDS_RB_FACE);
			break;
		case IDC_SELPOLY:
			lpttt->lpszText = GetString(IDS_EM_POLY);
			break;
		case IDC_SELELEMENT:
			lpttt->lpszText = GetString(IDS_EM_ELEMENT);
			break;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

#define GRAPHSTEPS 20

static void DrawCurve(HWND hWnd,HDC hdc, EditMeshMod *mod) {
	TSTR label = FormatUniverseValue(mod->falloff);
	TSTR zero = FormatUniverseValue(0.0f);
	SetWindowText (GetDlgItem (hWnd, IDC_FARLEFTLABEL), label);
	SetWindowText (GetDlgItem (hWnd, IDC_NEARLABEL), zero);
	SetWindowText (GetDlgItem (hWnd, IDC_FARRIGHTLABEL), label);

	Rect rect, orect;
	GetClientRectP(GetDlgItem(hWnd,IDC_AR_GRAPH),&rect);
	orect = rect;

	SelectObject(hdc,GetStockObject(NULL_PEN));
	SelectObject(hdc,GetStockObject(WHITE_BRUSH));
	Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);	
	SelectObject(hdc,GetStockObject(NULL_BRUSH));
	
	rect.left   += 3;
	rect.right  -= 3;
	rect.top    += 20;
	rect.bottom -= 20;
	
	SelectObject(hdc,CreatePen(PS_DOT,0,GetSysColor(COLOR_BTNFACE)));
	MoveToEx(hdc,orect.left,rect.top,NULL);
	LineTo(hdc,orect.right,rect.top);
	MoveToEx(hdc,orect.left,rect.bottom,NULL);
	LineTo(hdc,orect.right,rect.bottom);
	MoveToEx(hdc,(rect.left+rect.right)/2,orect.top,NULL);
	LineTo(hdc,(rect.left+rect.right)/2,orect.bottom);
	DeleteObject(SelectObject(hdc,GetStockObject(BLACK_PEN)));
	
	MoveToEx(hdc,rect.left,rect.bottom,NULL);
	for (int i=0; i<=GRAPHSTEPS; i++) {
		float dist = mod->falloff * float(abs(i-GRAPHSTEPS/2))/float(GRAPHSTEPS/2);		
		float y = AffectRegionFunction (dist, mod->falloff, mod->pinch, mod->bubble);
		int ix = rect.left + int(float(rect.w()-1) * float(i)/float(GRAPHSTEPS));
		int	iy = rect.bottom - int(y*float(rect.h()-2)) - 1;
		if (iy<orect.top) iy = orect.top;
		if (iy>orect.bottom-1) iy = orect.bottom-1;
		LineTo(hdc, ix, iy);
	}
	
	WhiteRect3D(hdc,orect,TRUE);
}

void EditMeshMod::SetARDlgEnables () {
	if (!hAR) return;
	ISpinnerControl *spin;
	EnableWindow (GetDlgItem (hAR, IDC_EM_AFFECT_REGION), selLevel);
	bool enable = (selLevel && affectRegion) ? TRUE : FALSE;
	EnableWindow (GetDlgItem (hAR, IDC_EM_E_DIST), enable);
	EnableWindow (GetDlgItem (hAR, IDC_EM_AR_BACK), enable);
	spin = GetISpinner (GetDlgItem (hAR, IDC_EM_E_ITER_SPIN));
	spin->Enable (enable && useEdgeDist);
	ReleaseISpinner (spin);
	spin = GetISpinner (GetDlgItem (hAR, IDC_FALLOFFSPIN));
	spin->Enable (enable);
	ReleaseISpinner (spin);
	spin = GetISpinner (GetDlgItem (hAR, IDC_PINCHSPIN));
	spin->Enable (enable);
	ReleaseISpinner (spin);
	spin = GetISpinner (GetDlgItem (hAR, IDC_BUBBLESPIN));
	spin->Enable (enable);
	ReleaseISpinner (spin);
	EnableWindow (GetDlgItem (hAR, IDC_FALLOFF_LABEL), enable);
	EnableWindow (GetDlgItem (hAR, IDC_PINCH_LABEL), enable);
	EnableWindow (GetDlgItem (hAR, IDC_BUBBLE_LABEL), enable);
}

BOOL CALLBACK AffectRegionDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	EditMeshMod *em = (EditMeshMod*)GetWindowLong(hWnd,GWL_USERDATA);
	ISpinnerControl *spin;
	Rect rect;

	switch (msg) {
	case WM_INITDIALOG:
		em = (EditMeshMod*)lParam;
		SetWindowLong(hWnd,GWL_USERDATA,lParam);

		// Set spinners:
		spin = GetISpinner(GetDlgItem(hWnd,IDC_FALLOFFSPIN));
		spin->SetLimits(0.001f,9999999.0f, FALSE);
		spin->SetAutoScale();
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_FALLOFF), EDITTYPE_POS_UNIVERSE);
		spin->SetValue(em->falloff,FALSE);
		spin->SetResetValue (20.0f);
		ReleaseISpinner(spin);

		spin = GetISpinner(GetDlgItem(hWnd,IDC_PINCHSPIN));
		spin->SetLimits(-10.0f,10.0f, FALSE);
		spin->SetScale(0.01f);
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_PINCH), EDITTYPE_FLOAT);
		spin->SetValue(em->pinch,FALSE);
		spin->SetResetValue (0.0f);
		ReleaseISpinner(spin);

		spin = GetISpinner(GetDlgItem(hWnd,IDC_BUBBLESPIN));
		spin->SetLimits(-10.0f,10.0f, FALSE);
		spin->SetScale(0.01f);
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_BUBBLE), EDITTYPE_FLOAT);
		spin->SetValue(em->bubble,FALSE);
		spin->SetResetValue (0.0f);
		ReleaseISpinner(spin);
					
		spin = GetISpinner(GetDlgItem(hWnd,IDC_EM_E_ITER_SPIN));
		spin->SetLimits(1,8, FALSE);
		spin->SetScale(0.01f);
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_EM_E_ITER), EDITTYPE_INT);
		spin->SetValue(em->edgeIts,FALSE);
		spin->SetResetValue (1);
		if (!em->affectRegion || !em->useEdgeDist) spin->Disable();
		ReleaseISpinner(spin);

		// Check Checkboxes:
		CheckDlgButton (hWnd, IDC_EM_AFFECT_REGION, em->affectRegion);
		CheckDlgButton(hWnd, IDC_EM_E_DIST, em->useEdgeDist);
		CheckDlgButton (hWnd, IDC_EM_AR_BACK, !em->arIgBack);

		ShowWindow(GetDlgItem(hWnd,IDC_AR_GRAPH),SW_HIDE);
		CenterWindow(hWnd,GetParent(hWnd));
		em->hAR = hWnd;
		em->SetARDlgEnables ();
		break;
		
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd,&ps);
		DrawCurve (hWnd, hdc, em);
		EndPaint(hWnd,&ps);
		break;
		}
		
	case CC_SPINNER_CHANGE:
		if (LOWORD(wParam) != IDC_EM_E_ITER_SPIN) {
			GetClientRectP(GetDlgItem(hWnd,IDC_AR_GRAPH),&rect);
			InvalidateRect(hWnd,&rect,FALSE);
		}
		spin = (ISpinnerControl*)lParam;
		switch (LOWORD(wParam)) {
		case IDC_FALLOFFSPIN:
			em->falloff = spin->GetFVal();
			em->InvalidateAffectRegion();
			break;
		case IDC_PINCHSPIN:
			em->pinch = spin->GetFVal();
			em->InvalidateAffectRegion();
			break;
		case IDC_BUBBLESPIN:
			em->bubble = spin->GetFVal();
			em->InvalidateAffectRegion();
			break;
		case IDC_EM_E_ITER_SPIN:
			em->edgeIts = spin->GetIVal ();
			em->InvalidateDistances ();
		}
		em->NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
		em->ip->RedrawViews (em->ip->GetTime(),REDRAW_INTERACTIVE);
		break;

	case WM_COMMAND:
		if (HIWORD(wParam)==1) return FALSE;	// not handling keyboard accelerators here.
		switch (LOWORD(wParam)) {
		case IDC_EM_AFFECT_REGION:
			em->affectRegion = IsDlgButtonChecked (hWnd,IDC_EM_AFFECT_REGION);
			em->SetARDlgEnables ();
			em->InvalidateDistances ();
			em->NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
			em->ip->RedrawViews(em->ip->GetTime());
			break;

		case IDC_EM_E_DIST:
			em->useEdgeDist = IsDlgButtonChecked (hWnd, IDC_EM_E_DIST);
			spin = GetISpinner (GetDlgItem (hWnd, IDC_EM_E_ITER_SPIN));
			spin->Enable (em->useEdgeDist && em->affectRegion);
			ReleaseISpinner(spin);
			em->InvalidateDistances ();
			em->NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
			em->ip->RedrawViews(em->ip->GetTime());
			break;

		case IDC_EM_AR_BACK:
			em->arIgBack = !IsDlgButtonChecked (hWnd, IDC_EM_AR_BACK);
			em->InvalidateAffectRegion ();
			em->NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
			em->ip->RedrawViews(em->ip->GetTime());
			break;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

void EditMeshMod::SetGeomDlgEnables () {
	if (!hGeom) return;
	BOOL edg = (selLevel == SL_EDGE);
	BOOL vtx = (selLevel == SL_VERTEX);
	BOOL fac = (selLevel >= SL_FACE);
	BOOL obj = (selLevel == SL_OBJECT);
	ISpinnerControl *spin;
	ICustButton *but;

	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_CREATE));
	but->Enable (vtx||fac);
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_DETACH));
	but->Enable (!edg);
	if (obj) but->SetText (GetString (IDS_EM_ATTACH_LIST));
	else but->SetText (GetString (IDS_EM_DETACH));
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_DIVIDE));
	but->Enable (!obj);
	if (edg||fac) {
		but->SetText (GetString (IDS_EM_DIVIDE));
		but->SetType(CBT_CHECK);
		but->SetHighlightColor(GREEN_WASH);
	} else {
		but->SetText (GetString (IDS_EM_BREAK));
		but->SetType(CBT_PUSH);
	}
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_TURN));
	but->Enable (edg);
	ReleaseICustButton (but);
	// Align buttons always active.
	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_MAKEPLANAR));
	but->Enable (!obj);
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_DELETE));
	but->Enable (!obj);
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_EXTRUDE));
	but->Enable (edg||fac);
	ReleaseICustButton (but);
	spin = GetISpinner (GetDlgItem (hGeom, IDC_EM_EXTRUDESPINNER));
	spin->Enable (edg||fac);
	ReleaseISpinner (spin);
	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_BEVEL));
	but->Enable (!obj);
	if (vtx||edg) but->SetText (GetString (IDS_EM_CHAMFER));
	else but->SetText (GetString (IDS_EM_BEVEL));
	ReleaseICustButton (but);
	spin = GetISpinner (GetDlgItem (hGeom, IDC_EM_OUTLINESPINNER));
	spin->Enable (!obj);
	if (vtx||edg) spin->SetLimits(0.0f, 9999999.0f, FALSE);
	else spin->SetLimits(-9999999.0f, 9999999.0f, FALSE);
	ReleaseISpinner (spin);
	EnableWindow (GetDlgItem (hGeom, IDC_EM_EXTYPE_A), fac||edg);
	EnableWindow (GetDlgItem (hGeom, IDC_EM_EXTYPE_B), fac||edg);
	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_COLLAPSE));
	but->Enable (!obj);
	ReleaseICustButton (but);
	// It would be nice if Slice Plane were always active, but we can't make it available
	// at the object level, since the transforms won't work.
	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_SLICEPLANE));
	but->Enable (!obj);
	ReleaseICustButton (but);
	EnableWindow (GetDlgItem (hGeom, IDC_EM_SPLIT), !obj);
	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_CUT));
	but->Enable (edg|fac);
	ReleaseICustButton (but);
	EnableWindow (GetDlgItem (hGeom, IDC_EM_REFINE), (edg||fac) && !sliceMode);

	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_TESSELLATE));
	but->Enable (fac);
	ReleaseICustButton (but);
	spin = GetISpinner (GetDlgItem (hGeom, IDC_EM_TENSIONSPINNER));
	spin->Enable (fac && edgeTes);
	ReleaseISpinner (spin);
	EnableWindow (GetDlgItem (hGeom, IDC_EM_TES_TEXT), fac);
	EnableWindow (GetDlgItem (hGeom, IDC_EM_TES_EDGE), fac);
	EnableWindow (GetDlgItem (hGeom, IDC_EM_TES_CENTER), fac);

	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_EXPLODE));
	but->Enable (fac || obj);
	ReleaseICustButton (but);
	spin = GetISpinner (GetDlgItem (hGeom, IDC_EM_ANGLETHRESHSPIN));
	spin->Enable (fac || obj);
	ReleaseISpinner (spin);
	EnableWindow (GetDlgItem (hGeom, IDC_EM_EXP_TEXT), fac||obj);
	EnableWindow (GetDlgItem (hGeom, IDC_EM_EXP_ELEMENTS), fac||obj);
	EnableWindow (GetDlgItem (hGeom, IDC_EM_EXP_OBJECTS), fac||obj);

	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_WELD));
	but->Enable (vtx);
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_WELDTOVERT));
	but->Enable (vtx);
	ReleaseICustButton (but);
	EnableWindow (GetDlgItem (hGeom, IDC_EM_PIXELS_TEXT), vtx);
	spin = GetISpinner (GetDlgItem (hGeom, IDC_EM_W_THR_SPIN));
	spin->Enable (vtx);
	ReleaseISpinner (spin);
	spin = GetISpinner (GetDlgItem (hGeom, IDC_EM_T_THR_SPIN));
	spin->Enable (vtx);
	ReleaseISpinner (spin);

	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_SELECT_OPEN));
	but->Enable (edg);
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hGeom, IDC_EM_CREATE_CURVE));
	but->Enable (edg);
	ReleaseICustButton (but);
}

static BOOL CALLBACK GeomDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	EditMeshMod *em = (EditMeshMod*)GetWindowLong(hWnd,GWL_USERDATA);
	ISpinnerControl *spin;
	ICustButton *but;
	TSTR name;

	switch (msg) {
	case WM_INITDIALOG:
		// Record the EM * in the window's long.
		em = (EditMeshMod*)lParam;
		em->hGeom = hWnd;
		SetWindowLong(hWnd,GWL_USERDATA,lParam);

		// Set up the "depressed" color for the command-mode buttons
		but = GetICustButton(GetDlgItem(hWnd,IDC_EM_CREATE));
		but->SetType(CBT_CHECK);
		but->SetHighlightColor(GREEN_WASH);
		ReleaseICustButton(but);

		but = GetICustButton(GetDlgItem(hWnd,IDC_EM_DIVIDE));
		but->SetType(CBT_CHECK);
		but->SetHighlightColor(GREEN_WASH);
		ReleaseICustButton(but);

		but = GetICustButton(GetDlgItem(hWnd,IDC_EM_TURN));
		but->SetType(CBT_CHECK);
		but->SetHighlightColor(GREEN_WASH);
		ReleaseICustButton(but);

		but = GetICustButton(GetDlgItem(hWnd,IDC_EM_EXTRUDE));
		but->SetType(CBT_CHECK);
		but->SetHighlightColor(GREEN_WASH);
		ReleaseICustButton(but);

		but = GetICustButton(GetDlgItem(hWnd,IDC_EM_BEVEL));
		but->SetType(CBT_CHECK);
		but->SetHighlightColor(GREEN_WASH);
		ReleaseICustButton(but);

		but = GetICustButton(GetDlgItem(hWnd,IDC_EM_OBJ_ATTACH));
		but->SetType(CBT_CHECK);
		but->SetHighlightColor(GREEN_WASH);
		ReleaseICustButton(but);					

		but = GetICustButton(GetDlgItem(hWnd,IDC_EM_SLICEPLANE));
		but->SetType(CBT_CHECK);
		but->SetHighlightColor(GREEN_WASH);
		but->SetCheck (em->sliceMode);
		ReleaseICustButton(but);

		but = GetICustButton(GetDlgItem(hWnd,IDC_EM_CUT));
		but->SetType(CBT_CHECK);
		but->SetHighlightColor(GREEN_WASH);
		ReleaseICustButton(but);

		but = GetICustButton(GetDlgItem(hWnd,IDC_EM_WELDTOVERT));
		but->SetType(CBT_CHECK);
		but->SetHighlightColor(GREEN_WASH);
		ReleaseICustButton(but);

		// Set up spinners
		spin = GetISpinner(GetDlgItem(hWnd,IDC_EM_EXTRUDESPINNER));
		spin->SetLimits(-9999999.0f, 9999999.0f, FALSE);
		spin->LinkToEdit (GetDlgItem (hWnd,IDC_EM_EXTRUDEAMOUNT), EDITTYPE_UNIVERSE);
		spin->SetScale (.1f);
		ReleaseISpinner (spin);

		spin = GetISpinner(GetDlgItem(hWnd,IDC_EM_OUTLINESPINNER));
		spin->SetLimits(-9999999.0f, 9999999.0f, FALSE);
		spin->LinkToEdit (GetDlgItem (hWnd,IDC_EM_OUTLINEAMOUNT), EDITTYPE_UNIVERSE);
		spin->SetScale (.1f);
		ReleaseISpinner (spin);

		spin = GetISpinner(GetDlgItem(hWnd,IDC_EM_TENSIONSPINNER));
		spin->SetLimits(-100,100, FALSE);
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_EM_TENSION), EDITTYPE_FLOAT);
		spin->SetValue(tessTens,FALSE);
		ReleaseISpinner(spin);

		spin = GetISpinner (GetDlgItem (hWnd,IDC_EM_ANGLETHRESHSPIN));
		spin->SetLimits(0, 180, FALSE);
		spin->SetScale(0.1f);
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_EM_ANGLETHRESH), EDITTYPE_FLOAT);
		spin->SetValue(explodeThresh,FALSE);
		ReleaseISpinner(spin);

		spin = GetISpinner (GetDlgItem(hWnd, IDC_EM_W_THR_SPIN));
		spin->SetLimits(0,9999999, FALSE);
		spin->SetAutoScale();
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_EM_W_THR), EDITTYPE_POS_UNIVERSE);
		spin->SetValue(weldThresh,FALSE);
		ReleaseISpinner(spin);

		spin = GetISpinner(GetDlgItem(hWnd,IDC_EM_T_THR_SPIN));
		spin->SetLimits(1,1000, FALSE);
		spin->SetScale(0.1f);
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_EM_T_THR), EDITTYPE_INT);
		spin->SetValue(em->weldBoxSize,FALSE);
		ReleaseISpinner(spin);

		// Take care of Explode and Tessellation radios:
		if (expObj) {
			CheckDlgButton(hWnd,IDC_EM_EXP_OBJECTS,TRUE);
			CheckDlgButton(hWnd,IDC_EM_EXP_ELEMENTS,FALSE);
		} else {
			CheckDlgButton(hWnd,IDC_EM_EXP_OBJECTS,FALSE);
			CheckDlgButton(hWnd,IDC_EM_EXP_ELEMENTS,TRUE);
		}

		if (edgeTes) {
			CheckDlgButton(hWnd,IDC_EM_TES_EDGE,TRUE);
			CheckDlgButton(hWnd,IDC_EM_TES_CENTER,FALSE);
		} else {
			CheckDlgButton(hWnd,IDC_EM_TES_EDGE,FALSE);
			CheckDlgButton(hWnd,IDC_EM_TES_CENTER,TRUE);				
		}

		if (em->extType == MESH_EXTRUDE_CLUSTER) {
			CheckRadioButton (hWnd, IDC_EM_EXTYPE_A, IDC_EM_EXTYPE_B, IDC_EM_EXTYPE_A);
		} else {
			CheckRadioButton (hWnd, IDC_EM_EXTYPE_A, IDC_EM_EXTYPE_B, IDC_EM_EXTYPE_B);
		}

		// Set Slice button to be grey if not in Slice Plane mode.
		but = GetICustButton (GetDlgItem (hWnd, IDC_EM_SLICE));
		but->Enable (em->sliceMode);
		ReleaseICustButton (but);

		// Check boxes as appropriate
		CheckDlgButton(hWnd, IDC_EM_SPLIT, em->sliceSplit);
		CheckDlgButton(hWnd, IDC_EM_REFINE, em->cutRefine);
		em->SetGeomDlgEnables();
		break;

	case CC_SPINNER_BUTTONDOWN:
		switch (LOWORD(wParam)) {
		case IDC_EM_EXTRUDESPINNER:
			em->BeginExtrude (em->ip->GetTime());
			break;

		case IDC_EM_OUTLINESPINNER:
			switch (em->selLevel) {
			case SL_VERTEX:
			case SL_EDGE:
				em->BeginChamfer (em->ip->GetTime ());
				break;
			case SL_FACE:
			case SL_POLY:
			case SL_ELEMENT:
				em->BeginBevel (em->ip->GetTime (), FALSE);
				break;
			}
			break;
		}
		break;

	case CC_SPINNER_BUTTONUP:
		switch (LOWORD(wParam)) {
		case IDC_EM_EXTRUDESPINNER:
			em->EndExtrude (em->ip->GetTime(), HIWORD(wParam));
			em->ip->RedrawViews (em->ip->GetTime(),REDRAW_END);
			break;

		case IDC_EM_OUTLINESPINNER:
			switch (em->selLevel) {
			case SL_VERTEX:
			case SL_EDGE:
				em->EndChamfer (em->ip->GetTime (), HIWORD(wParam));
				break;
			case SL_FACE:
			case SL_POLY:
			case SL_ELEMENT:
				em->EndBevel (em->ip->GetTime(), HIWORD(wParam));
				break;
			}
			em->ip->RedrawViews (em->ip->GetTime(),REDRAW_END);
			break;
		}
		break;

	case CC_SPINNER_CHANGE:
		spin = (ISpinnerControl*)lParam;

		switch (LOWORD(wParam)) {
		case IDC_EM_T_THR_SPIN: em->weldBoxSize = spin->GetIVal(); break;
		case IDC_EM_W_THR_SPIN: weldThresh = spin->GetFVal (); break;
		case IDC_EM_ANGLETHRESHSPIN: explodeThresh = spin->GetFVal (); break;
		case IDC_EM_TENSIONSPINNER: tessTens = spin->GetFVal (); break;

		case IDC_EM_EXTRUDESPINNER:
			bool enterKey;
			enterKey = FALSE;
			if (!HIWORD(wParam) && !em->inExtrude) {
				enterKey = TRUE;
				em->BeginExtrude(em->ip->GetTime());
			}
			em->Extrude (em->ip->GetTime(),spin->GetFVal());
			if (enterKey) {
				em->EndExtrude (em->ip->GetTime(),TRUE);
				em->ip->RedrawViews (em->ip->GetTime(), REDRAW_END);
			} else {
				em->ip->RedrawViews (em->ip->GetTime(),REDRAW_INTERACTIVE);
			}
			break;

		case IDC_EM_OUTLINESPINNER:
			enterKey = FALSE;
			switch (em->selLevel) {
			case SL_VERTEX:
			case SL_EDGE:
				if (!HIWORD(wParam) && !em->inChamfer) {
					enterKey = TRUE;
					em->BeginChamfer (em->ip->GetTime ());
				}
				em->Chamfer (em->ip->GetTime (), spin->GetFVal ());
				if (enterKey) em->EndChamfer (em->ip->GetTime (), TRUE);
				break;

			default:
				if (!HIWORD(wParam) && !em->inBevel) {
					enterKey = TRUE;
					em->BeginBevel (em->ip->GetTime ());
				}
				em->Bevel (em->ip->GetTime (), spin->GetFVal ());
				if (enterKey) em->EndBevel (em->ip->GetTime (), TRUE);
				break;
			}
			if (enterKey) em->ip->RedrawViews (em->ip->GetTime(), REDRAW_END);
			else em->ip->RedrawViews (em->ip->GetTime(),REDRAW_INTERACTIVE);
			break;
		}
		break;

	case WM_COMMAND:
		if (HIWORD(wParam)==1) return FALSE;	// not handling keyboard accelerators here.

		switch (LOWORD(wParam)) {
		case IDC_EM_CREATE: em->ToggleCommandMode (McmCreate); break;

		case IDC_EM_DETACH:
			if (em->selLevel == SL_OBJECT) {
				// Really an attach multiple button.
				MAttachHitByName proc(em);
				em->ip->DoHitByNameDialog(&proc);
			} else em->ButtonOp (MopDetach);
			break;

		case IDC_EM_DIVIDE:
			switch (em->selLevel) {
			case SL_OBJECT: break;
			case SL_VERTEX: em->ButtonOp (MopBreak); break;
			default: em->ToggleCommandMode (McmDivide); break;
			}
			break;

		case IDC_EM_TURN: em->ToggleCommandMode (McmTurnEdge); break;
		case IDC_EM_ALIGNVIEW: em->ButtonOp (MopViewAlign); break;
		case IDC_EM_ALIGNCONST: em->ButtonOp (MopGridAlign); break;
		case IDC_EM_MAKEPLANAR: em->ButtonOp (MopMakePlanar); break;
		case IDC_EM_DELETE: em->ButtonOp (MopDelete); break;
		case IDC_EM_EXTRUDE: em->ToggleCommandMode (McmExtrude); break;

		case IDC_EM_BEVEL:
			switch (em->selLevel) {
			case SL_VERTEX:
			case SL_EDGE:
				em->ToggleCommandMode (McmChamfer);
				break;
			case SL_FACE:
			case SL_POLY:
			case SL_ELEMENT:
				em->ToggleCommandMode (McmBevel);
				break;
			}
			break;

		case IDC_EM_EXTYPE_A:
			em->extType = MESH_EXTRUDE_CLUSTER;
			break;

		case IDC_EM_EXTYPE_B:
			em->extType = MESH_EXTRUDE_LOCAL;
			break;

		case IDC_EM_COLLAPSE: em->ButtonOp (MopCollapse); break;
		case IDC_EM_OBJ_ATTACH: em->ToggleCommandMode (McmAttach); break;
		case IDC_EM_SLICEPLANE: em->ToggleCommandMode (McmSlicePlane); break;
		case IDC_EM_CUT: em->ToggleCommandMode (McmCut); break;
		case IDC_EM_SLICE: em->ButtonOp (MopSlice); break;

		case IDC_EM_REFINE:
			em->cutRefine = IsDlgButtonChecked(hWnd,IDC_EM_REFINE) ? TRUE : FALSE;
			break;

		case IDC_EM_SPLIT:
			em->sliceSplit = IsDlgButtonChecked(hWnd,IDC_EM_SPLIT) ? TRUE : FALSE;
			break;

		case IDC_EM_TESSELLATE: em->ButtonOp (MopTessellate); break;

		case IDC_EM_TES_EDGE:
			edgeTes = TRUE;
			spin = GetISpinner (GetDlgItem (hWnd,IDC_EM_TENSIONSPINNER));
			spin->Enable();
			ReleaseISpinner(spin);
			break;

		case IDC_EM_TES_CENTER:
			edgeTes = FALSE;
			spin = GetISpinner (GetDlgItem (hWnd,IDC_EM_TENSIONSPINNER));
			spin->Disable();
			ReleaseISpinner(spin);
			break;

		case IDC_EM_EXPLODE: em->ButtonOp (MopExplode); break;

		case IDC_EM_EXP_OBJECTS:
			expObj = TRUE;
			break;

		case IDC_EM_EXP_ELEMENTS:
			expObj = FALSE;
			break;

		case IDC_EM_WELD: em->ButtonOp (MopWeld); break;
		case IDC_EM_WELDTOVERT: em->ToggleCommandMode (McmWeldTarget); break;
		case IDC_EM_REMOVE_ISO_VERTS: em->ButtonOp (MopRemoveIsolatedVerts); break;
		case IDC_EM_SELECT_OPEN: em->ButtonOp (MopSelectOpenEdges); break;
		case IDC_EM_CREATE_CURVE: em->ButtonOp (MopCreateShapeFromEdges); break;
		}
		break;

	default:
		return FALSE;
	}
	
	return TRUE;
}

static void SetSmoothButtonState(HWND hWnd,DWORD bits,DWORD invalid,DWORD unused=0) {
	for (int i=IDC_SMOOTH_GRP1; i<IDC_SMOOTH_GRP1+32; i++) {
		if ( (unused&(1<<(i-IDC_SMOOTH_GRP1))) ) {
			ShowWindow(GetDlgItem(hWnd,i),SW_HIDE);
			continue;
		}

		if ( (invalid&(1<<(i-IDC_SMOOTH_GRP1))) ) {
			SetWindowText(GetDlgItem(hWnd,i),NULL);
			SendMessage(GetDlgItem(hWnd,i),CC_COMMAND,CC_CMD_SET_STATE,FALSE);
		} else {
			TSTR buf;
			buf.printf(_T("%d"),i-IDC_SMOOTH_GRP1+1);
			SetWindowText(GetDlgItem(hWnd,i),buf);
			SendMessage(GetDlgItem(hWnd,i),CC_COMMAND,CC_CMD_SET_STATE,
				(bits&(1<<(i-IDC_SMOOTH_GRP1)))?TRUE:FALSE);
		}
		InvalidateRect(GetDlgItem(hWnd,i),NULL,TRUE);
	}
}

static BOOL CALLBACK SelectBySmoothDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static DWORD *param;
	int i;
	ICustButton *iBut;

	switch (msg) {
	case WM_INITDIALOG:
		param = (DWORD*)lParam;
		for (i=IDC_SMOOTH_GRP1; i<IDC_SMOOTH_GRP1+32; i++)
			SendMessage(GetDlgItem(hWnd,i),CC_COMMAND,CC_CMD_SET_TYPE,CBT_CHECK);
		SetSmoothButtonState(hWnd,param[0],0,param[2]);
		CheckDlgButton(hWnd,IDC_CLEARSELECTION,param[1]);
		CenterWindow(hWnd,GetParent(hWnd));
		break;

	case WM_COMMAND: 
		if (LOWORD(wParam)>=IDC_SMOOTH_GRP1 &&
			LOWORD(wParam)<=IDC_SMOOTH_GRP32) {
			iBut = GetICustButton(GetDlgItem(hWnd,LOWORD(wParam)));				
			int shift = LOWORD(wParam) - IDC_SMOOTH_GRP1;				
			if (iBut->IsChecked()) param[0] |= 1<<shift;
			else param[0] &= ~(1<<shift);
			ReleaseICustButton(iBut);
			break;
		}

		switch (LOWORD(wParam)) {
		case IDOK:
			param[1] = IsDlgButtonChecked(hWnd,IDC_CLEARSELECTION);
			EndDialog(hWnd,1);
			break;

		case IDCANCEL:
			EndDialog(hWnd,0);
			break;
		}
		break;			

	default: return FALSE;
	}
	return TRUE;
}

static BOOL CALLBACK SelectByMatDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static int *param;
	ISpinnerControl *spin;

	switch (msg) {
	case WM_INITDIALOG:
		param = (int*)lParam;
		SetupIntSpinner(hWnd,IDC_MAT_IDSPIN,IDC_MAT_ID,1,MAX_MATID,param[0]);			
		CheckDlgButton(hWnd,IDC_CLEARSELECTION,param[1]);
		CenterWindow(hWnd,GetParent(hWnd));
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			spin = GetISpinner(GetDlgItem(hWnd,IDC_MAT_IDSPIN));
			param[0] = spin->GetIVal();
			param[1] = IsDlgButtonChecked(hWnd,IDC_CLEARSELECTION);
			ReleaseISpinner(spin);
			EndDialog(hWnd,1);					
			break;

		case IDCANCEL:
			EndDialog(hWnd,0);
			break;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

void EditMeshMod::InvalidateSurfaceUI() {
	if (!hSurf) return;
	faceUIValid = FALSE;
	InvalidateRect(hSurf,NULL,FALSE);
}

class WeightSpinRestore : public RestoreObj {
public:
	EditMeshMod *eto;

	WeightSpinRestore () { eto = NULL; }
	WeightSpinRestore (EditMeshMod *eo) { eto = eo; }
	void Restore (int isUndo);
	void Redo () { Restore (TRUE); }
	int Size () { return sizeof(eto); }
	TSTR Description() { return _T("Weight Spin Restore"); }
};

void WeightSpinRestore::Restore (int isUndo) {
	if (!eto) return;
	if (!eto->hSurf) return;
	if (!GetDlgItem (eto->hSurf, IDC_EM_WEIGHT_LABEL)) return;
	eto->UpdateWeightDisplay (eto->ip ? eto->ip->GetTime() : 0, eto->hSurf);
}

static int autoEdgeTypeIDs [] = { IDC_EM_AE_SETCLEAR, IDC_EM_AE_SET, IDC_EM_AE_CLEAR };

static BOOL CALLBACK SurfaceDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	EditMeshMod *em = (EditMeshMod*)GetWindowLong(hWnd,GWL_USERDATA);
	ISpinnerControl *spin;
	ICustButton *but;
	IColorSwatch *iCol;
	COLORREF rgb;
	int i;
	bool isFace, isVert;

	switch (msg) {
	case WM_INITDIALOG:
		em = (EditMeshMod*)lParam;
		em->hSurf = hWnd;
		SetWindowLong(hWnd,GWL_USERDATA,lParam);

		if (GetDlgItem (hWnd, IDC_SMOOTH_GRP1)) {	// Face stuff:
			// NOTE: the following requires that the smoothing group ID's be sequential!
			isFace = TRUE;
			for (i=IDC_SMOOTH_GRP1; i<IDC_SMOOTH_GRP1+32; i++)
				SendMessage(GetDlgItem(hWnd,i),CC_COMMAND,CC_CMD_SET_TYPE,CBT_CHECK);
			SetupIntSpinner(hWnd,IDC_EM_MAT_IDSPIN,IDC_EM_MAT_ID,1,MAX_MATID,0);
			SetupFloatSpinner(hWnd, IDC_EM_SMOOTH_THRESHSPIN,
				IDC_EM_SMOOTH_THRESH, 0.0f, 180.0f, autoSmoothThresh, 0.1f);

			but = GetICustButton(GetDlgItem(hWnd,IDC_EM_NORMAL_FLIPMODE));
			but->SetType(CBT_CHECK);
			but->SetHighlightColor(GREEN_WASH);
			ReleaseICustButton(but);

			em->faceUIValid = FALSE;

			CheckDlgButton(hWnd,IDC_EM_NORMAL_SHOW,em->normScale>0.0f);
			if (em->normScale>0.0f) em->ShowNormals();
			SetupFloatSpinner(hWnd,IDC_EM_NORMAL_SCALESPIN,IDC_EM_NORMAL_SCALE,0.0f,999999999.0f,10.0f,0.1f);

			iCol = GetIColorSwatch (GetDlgItem (hWnd, IDC_EM_VERT_COLOR),
				em->GetFaceColor(), GetString (IDS_EM_VERTEXCOLOR));
			ReleaseIColorSwatch (iCol);
		} else {
			isFace = FALSE;
		}

		if (GetDlgItem (hWnd, IDC_EM_VERT_SELRSPIN)) {	// Vertex stuff:
			isVert = TRUE;
			spin = SetupFloatSpinner (hWnd, IDC_EM_WEIGHTSPIN, IDC_EM_WEIGHT, 0.0f, 9999999.0f, 1.0f, .1f);
			spin->SetAutoScale(TRUE);
			em->UpdateWeightDisplay (em->ip->GetTime(), hWnd);
			SetupIntSpinner (hWnd, IDC_EM_VERT_SELRSPIN, IDC_EM_VERT_SELR, 0, 255, selDeltaR);
			SetupIntSpinner (hWnd, IDC_EM_VERT_SELGSPIN, IDC_EM_VERT_SELG, 0, 255, selDeltaG);
			SetupIntSpinner (hWnd, IDC_EM_VERT_SELBSPIN, IDC_EM_VERT_SELB, 0, 255, selDeltaB);
			rgb = RGB(int(selByColor.x*255.0),int(selByColor.y*255.0),int(selByColor.z*255.0));
			iCol = GetIColorSwatch (GetDlgItem(hWnd,IDC_EM_VERT_SELCOLOR), rgb, GetString(IDS_EM_SELBYCOLOR));
			ReleaseIColorSwatch(iCol);
			iCol = GetIColorSwatch (GetDlgItem (hWnd, IDC_EM_VERT_COLOR),
				em->GetVertColor(), GetString (IDS_EM_VERTEXCOLOR));
			ReleaseIColorSwatch (iCol);
		} else {
			isVert = FALSE;
		}

		if (GetDlgItem (hWnd, IDC_EM_ANGLETHRESH)) {	// Edge stuff:
			SetupFloatSpinner (hWnd, IDC_EM_ANGLETHRESHSPIN, IDC_EM_ANGLETHRESH,
				0.f, 180.f, autoEdgeThresh, 0.1f);
			for (i=0; i<3; i++) CheckDlgButton (hWnd, autoEdgeTypeIDs[i], (autoEdgeType == i));
		}
		break;

	case WM_DESTROY:
		em->ShowNormals();
		break;

	case WM_PAINT:
		if (em->faceUIValid) return FALSE;
		if (GetDlgItem (hWnd, IDC_SMOOTH_GRP1)) {
			DWORD invalid, bits;
			bits = em->GetSelSmoothBits(invalid);
			invalid -= bits;
			DWORD mat;
			mat = em->GetMatIndex();
			SetSmoothButtonState(hWnd,bits,invalid);
			spin = GetISpinner(GetDlgItem(hWnd,IDC_EM_MAT_IDSPIN));
			if (mat==UNDEFINED) {
				spin->SetIndeterminate(TRUE);
			} else {
				spin->SetIndeterminate(FALSE);
				spin->SetValue (int(mat+1),FALSE);
			}
			ReleaseISpinner(spin);
		}

		if (iCol = GetIColorSwatch (GetDlgItem(hWnd,IDC_EM_VERT_COLOR),
			(em->selLevel==SL_VERTEX)?em->GetVertColor():em->GetFaceColor(),
			GetString(IDS_EM_VERTEXCOLOR))) {
			ReleaseIColorSwatch(iCol);
			em->UpdateWeightDisplay (em->ip->GetTime (), hWnd);
		}

		em->faceUIValid = TRUE;
		return FALSE;

	case CC_COLOR_BUTTONDOWN:
		theHold.Begin();
		break;

	case CC_COLOR_BUTTONUP:
		if (HIWORD(wParam)) theHold.Accept (GetString (IDS_EM_SETVERTCOLOR));
		else theHold.Cancel();
		break;

	case CC_COLOR_CHANGE:
		iCol = (IColorSwatch*)lParam;
		switch (LOWORD(wParam)) {
		case IDC_EM_VERT_COLOR:
			if (em->selLevel == SL_VERTEX) em->SetVertColor (iCol->GetColor());
			else em->SetFaceColor (iCol->GetColor());
			break;
		case IDC_EM_VERT_SELCOLOR:
			COLORREF rgb = iCol->GetColor();
			selByColor.x = float(GetRValue(rgb))/255.0f;
			selByColor.y = float(GetGValue(rgb))/255.0f;
			selByColor.z = float(GetBValue(rgb))/255.0f;
			break;
		}
		break;

	case CC_SPINNER_BUTTONDOWN:
		switch (LOWORD(wParam)) {
		case IDC_EM_MAT_IDSPIN:
			theHold.Begin();
			break;
		case IDC_EM_WEIGHTSPIN:
			theHold.Begin ();
			break;
		}
		break;

	case WM_CUSTEDIT_ENTER:
	case CC_SPINNER_BUTTONUP:
		switch (LOWORD(wParam)) {
		case IDC_EM_MAT_ID:
		case IDC_EM_MAT_IDSPIN:
			em->ip->RedrawViews(em->ip->GetTime(),REDRAW_END);
			if (HIWORD(wParam) || msg==WM_CUSTEDIT_ENTER) 
				 theHold.Accept(GetString(IDS_RB_ASSIGNMATID));
			else theHold.Cancel();
			break;

		case IDC_EM_NORMAL_SCALESPIN:
			if (!IsDlgButtonChecked(hWnd,IDC_EM_NORMAL_SHOW)) break;
			em->ip->RedrawViews(em->ip->GetTime(),REDRAW_END);
			break;

		case IDC_EM_WEIGHT:
		case IDC_EM_WEIGHTSPIN:
			em->ip->RedrawViews (em->ip->GetTime(),REDRAW_END);
			if (HIWORD(wParam) || msg==WM_CUSTEDIT_ENTER) 
				theHold.Accept (GetString(IDS_CHANGEWEIGHT));
			else theHold.Cancel();
			break;
		}
		break;

	case CC_SPINNER_CHANGE:
		spin = (ISpinnerControl*)lParam;
		switch (LOWORD(wParam)) {
		case IDC_EM_MAT_IDSPIN:
			if (!theHold.Holding()) theHold.Begin();
			em->SetMatIndex(spin->GetIVal()-1);
			break;

		case IDC_EM_NORMAL_SCALESPIN: 
			if (!IsDlgButtonChecked(hWnd,IDC_EM_NORMAL_SHOW)) break;
			em->normScale = spin->GetFVal();
			em->NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
			em->ip->RedrawViews (em->ip->GetTime(),REDRAW_INTERACTIVE);
			break;

		case IDC_EM_VERT_SELRSPIN: selDeltaR = spin->GetIVal(); break;
		case IDC_EM_VERT_SELGSPIN: selDeltaG = spin->GetIVal(); break;
		case IDC_EM_VERT_SELBSPIN: selDeltaB = spin->GetIVal(); break;
		case IDC_EM_ANGLETHRESHSPIN: autoEdgeThresh = spin->GetFVal(); break;
		case IDC_EM_SMOOTH_THRESHSPIN: autoSmoothThresh = spin->GetFVal(); break;

		case IDC_EM_WEIGHTSPIN:
			if (!theHold.Holding()) theHold.Begin();
			theHold.Put (new WeightSpinRestore (em));
			em->SetWeight (em->ip->GetTime(), spin->GetFVal());
			break;
		}
		break;

	case WM_COMMAND:
		if (HIWORD(wParam)==1) return FALSE;	// not handling keyboard accelerators here.
		if (LOWORD(wParam)>=IDC_SMOOTH_GRP1 &&
			LOWORD(wParam)<=IDC_SMOOTH_GRP32) {
			ICustButton *iBut = GetICustButton(GetDlgItem(hWnd,LOWORD(wParam)));
			int bit = iBut->IsChecked() ? 1 : 0;
			int shift = LOWORD(wParam) - IDC_SMOOTH_GRP1;
			em->SetSelSmoothBits(bit<<shift,1<<shift);
			ReleaseICustButton(iBut);
			break;
		}
		switch (LOWORD(wParam)) {
		case IDC_EM_EDGE_VIS: em->ButtonOp (MopVisibleEdge); break;
		case IDC_EM_EDGE_INVIS: em->ButtonOp (MopInvisibleEdge); break;
		case IDC_EM_EDGE_AUTO: em->ButtonOp (MopAutoEdge); break;

		case IDC_EM_AE_SETCLEAR:
			autoEdgeType = 0;
			break;

		case IDC_EM_AE_SET:
			autoEdgeType = 1;
			break;

		case IDC_EM_AE_CLEAR:
			autoEdgeType = 2;
			break;

		case IDC_EM_NORMAL_SHOW:
			spin = GetISpinner(GetDlgItem(hWnd,IDC_EM_NORMAL_SCALESPIN));
			if (IsDlgButtonChecked(hWnd,IDC_EM_NORMAL_SHOW)) {						
				em->normScale = spin->GetFVal();
				em->ShowNormals();
			} else {
				em->normScale = -1.0f;
				em->ShowNormals();
			}
			ReleaseISpinner(spin);
			break;

		case IDC_EM_NORMAL_FLIP: em->ButtonOp (MopFlipNormal); break;
		case IDC_EM_NORMAL_UNIFY: em->ButtonOp (MopUnifyNormal); break;
		case IDC_EM_NORMAL_FLIPMODE: em->ToggleCommandMode (McmFlipNormalMode); break;

		case IDC_EM_SELECT_BYID:
			if (DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EM_SELECTBYMAT),
						em->ip->GetMAXHWnd (), SelectByMatDlgProc, (LPARAM)sbmParams)) {
				if (em->ip) em->SelectByMat(sbmParams[0]-1,sbmParams[1]);
			}
			break;

		case IDC_EM_SELECTBYSMOOTH:
			sbsParams[2] = ~em->GetUsedSmoothBits();
			if (DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EM_SELECTBYSMOOTH),
						em->ip->GetMAXHWnd (), SelectBySmoothDlgProc, (LPARAM)sbsParams)) {
				if (em->ip) em->SelectBySmoothGroup(sbsParams[0],(BOOL)sbsParams[1]);
			}
			break;

		case IDC_EM_SMOOTH_CLEAR:
			em->SetSelSmoothBits(0,0xffffffff);
			break;

		case IDC_EM_SMOOTH_AUTO: em->ButtonOp (MopAutoSmooth); break;

		case IDC_EM_VERT_SELBYCOLOR:
			BOOL add, sub;
			add = GetKeyState(VK_CONTROL)<0;
			sub = GetKeyState(VK_MENU)<0;
			em->SelectVertByColor (selByColor,selDeltaR,selDeltaG,selDeltaB,add,sub);
			break;
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

// Following should be _identical_ to that in triedui.cpp; the whole class should be in some core location.
// -sca/1999.03.12
BOOL EMeshShortcutCB::KeyboardShortcut (int id) {
	if (!em) return FALSE;
	if (!em->Editing()) return FALSE;
	int val;

	switch (id) {
	case MDUID_EM_SELTYPE:
		int type;
		switch (em->GetEMeshSelLevel()) {
			case EM_SL_FACE:  type = EM_SL_POLYGON;    break;
			case EM_SL_POLYGON:   type = EM_SL_ELEMENT; break;
			case EM_SL_ELEMENT: type = EM_SL_OBJECT;  break;
			case EM_SL_OBJECT: type = EM_SL_VERTEX; break;
			case EM_SL_VERTEX: type = EM_SL_EDGE; break;
			case EM_SL_EDGE: type = EM_SL_FACE; break;
		}
		em->SetEMeshSelLevel (type);
		break;

	case MDUID_EM_SELTYPE_BACK:
		switch (em->GetEMeshSelLevel()) {
			case EM_SL_FACE:  type = EM_SL_EDGE;    break;
			case EM_SL_POLYGON:   type = EM_SL_FACE; break;
			case EM_SL_ELEMENT: type = EM_SL_POLYGON;  break;
			case EM_SL_VERTEX: type = EM_SL_OBJECT; break;
			case EM_SL_OBJECT: type = EM_SL_ELEMENT; break;
			case EM_SL_EDGE: type = EM_SL_VERTEX; break;
		}
		em->SetEMeshSelLevel (type);
		break;

	case MDUID_EM_SELTYPE_VERTEX:
		em->SetEMeshSelLevel (EM_SL_VERTEX);
		break;

	case MDUID_EM_SELTYPE_EDGE:
		em->SetEMeshSelLevel (EM_SL_EDGE);
		break;

	case MDUID_EM_SELTYPE_FACE:
		em->SetEMeshSelLevel (EM_SL_FACE);
		break;

	case MDUID_EM_SELTYPE_POLYGON:
		em->SetEMeshSelLevel (EM_SL_POLYGON);
		break;

	case MDUID_EM_SELTYPE_ELEMENT:
		em->SetEMeshSelLevel (EM_SL_ELEMENT);
		break;

	case MDUID_EM_SELTYPE_OBJ:
		em->SetEMeshSelLevel (EM_SL_OBJECT);
		break;

	case MDUID_EM_AUTOSMOOTH:
		em->ButtonOp (MopAutoSmooth);
		break;

	case MDUID_EM_ATTACH:
		em->ToggleCommandMode (McmAttach);
		break;

	case MDUID_EM_BREAK:
		em->ButtonOp (MopBreak);
		break;

	case MDUID_EM_IGBACK:
		em->GetUIParam (MuiIgBack, val);
		em->SetUIParam (MuiIgBack, !val);
		break;

	case MDUID_EM_BEVEL:
		if (em->GetEMeshSelLevel() >= EM_SL_FACE) em->ToggleCommandMode (McmBevel);
		else em->ToggleCommandMode (McmChamfer);
		break;

	case MDUID_EM_CREATE:
		em->ToggleCommandMode (McmCreate);
		break;

	case MDUID_EM_CUT:
		em->ToggleCommandMode (McmCut);
		break;

	case MDUID_EM_DIVIDE:
		em->ToggleCommandMode (McmDivide);
		break;

	case MDUID_EM_EXTRUDE:
		em->ToggleCommandMode (McmExtrude);
		break;

	case MDUID_EM_FLIPNORM:
		em->ButtonOp (MopFlipNormal);
		break;

	case MDUID_EM_SS_BACKFACE:
		em->GetUIParam (MuiSSBack, val);
		em->SetUIParam (MuiSSBack, !val);
		break;

	case MDUID_EM_UNIFY_NORMALS:
		em->ButtonOp (MopUnifyNormal);
		break;

	case MDUID_EM_HIDE:
		em->ButtonOp (MopHide);
		break;

	case MDUID_EM_EDGE_INVIS:
		em->ButtonOp (MopInvisibleEdge);
		break;

	case MDUID_EM_IGNORE_INVIS:
		em->GetUIParam (MuiIgnoreVis, val);
		em->SetUIParam (MuiIgnoreVis, !val);
		break;

	case MDUID_EM_COLLAPSE:
		em->ButtonOp (MopCollapse);
		break;

	case MDUID_EM_SHOWNORMAL:
		em->ButtonOp (MopShowNormal);
		break;

	case MDUID_EM_SELOPEN:
		em->ButtonOp (MopSelectOpenEdges);
		break;

	case MDUID_EM_REMOVE_ISO:
		em->ButtonOp (MopRemoveIsolatedVerts);
		break;

	case MDUID_EM_SLICEPLANE:
		em->ToggleCommandMode (McmSlicePlane);
		break;

	case MDUID_EM_SOFTSEL:
		em->GetUIParam (MuiSoftSel, val);
		em->SetUIParam (MuiSoftSel, !val);
		break;

	case MDUID_EM_SLICE:
		em->ButtonOp (MopSlice);
		break;

	case MDUID_EM_DETACH:
		em->ButtonOp (MopDetach);
		break;

	case MDUID_EM_TURNEDGE:
		em->ToggleCommandMode (McmTurnEdge);
		break;

	case MDUID_EM_UNHIDE:
		em->ButtonOp (MopUnhideAll);
		break;

	case MDUID_EM_EDGE_VIS:
		em->ButtonOp (MopVisibleEdge);
		break;

	case MDUID_EM_SELBYVERT:
		em->GetUIParam (MuiSelByVert, val);
		em->SetUIParam (MuiSelByVert, !val);
		break;

	case MDUID_EM_AUTOEDGE:
		em->ButtonOp (MopAutoEdge);
		break;

	case MDUID_EM_WELD:
		em->ButtonOp (MopWeld);
		break;

	case MDUID_EM_EXPLODE:
		em->ButtonOp (MopExplode);
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

//---------------------------------------------------------
//  MeshDeltaUser UI-related methods

void EditMeshMod::GetUIParam (meshUIParam uiCode, float & ret) {
	if (!ip) return;
	if (!Editing()) return;

	switch (uiCode) {
	case MuiPolyThresh:
		ret = planarFaceThresh;
		break;
	case MuiFalloff:
		ret = falloff;
		break;
	case MuiPinch:
		ret = pinch;
		break;
	case MuiBubble:
		ret = bubble;
		break;
	case MuiWeldDist:
		ret = weldThresh;
		break;
	}
}

void EditMeshMod::SetUIParam (meshUIParam uiCode, float val) {
	if (!ip) return;
	if (!Editing()) return;
	ISpinnerControl *spin;

	switch (uiCode) {
	case MuiPolyThresh:
		planarFaceThresh = val;
		if (hSel) {
			spin = GetISpinner (GetDlgItem (hSel, IDC_EM_PLANARSPINNER));
			spin->SetValue (val, FALSE);
			ReleaseISpinner (spin);
		}
		break;
	case MuiFalloff:
		falloff = val;
		InvalidateAffectRegion ();
		if (hAR) {
			spin = GetISpinner (GetDlgItem (hAR, IDC_FALLOFFSPIN));
			spin->SetValue (val, FALSE);
			ReleaseISpinner (spin);
		}
		break;
	case MuiPinch:
		pinch = val;
		InvalidateAffectRegion ();
		if (hAR) {
			spin = GetISpinner (GetDlgItem (hAR, IDC_PINCHSPIN));
			spin->SetValue (val, FALSE);
			ReleaseISpinner (spin);
		}
		break;
	case MuiBubble:
		bubble = val;
		InvalidateAffectRegion ();
		if (hAR) {
			spin = GetISpinner (GetDlgItem (hAR, IDC_BUBBLESPIN));
			spin->SetValue (val, FALSE);
			ReleaseISpinner (spin);
		}
		break;
	case MuiWeldDist:
		weldThresh = val;
		if (hGeom) {
			spin = GetISpinner (GetDlgItem (hGeom, IDC_EM_W_THR_SPIN));
			spin->SetValue (val, FALSE);
			ReleaseISpinner (spin);
		}
		break;
	}
}

void EditMeshMod::GetUIParam (meshUIParam uiCode, int & ret) {
	if (!ip) return;
	if (!Editing()) return;

	switch (uiCode) {
	case MuiSelByVert:
		ret = selByVert;
		break;
	case MuiIgBack:
		ret = ignoreBackfaces;
		break;
	case MuiIgnoreVis:
		ret = ignoreVisEdge;
		break;
	case MuiSoftSel:
		ret = affectRegion;
		break;
	case MuiSSUseEDist:
		ret = useEdgeDist;
		break;
	case MuiSSEDist:
		ret = edgeIts;
		break;
	case MuiSSBack:
		ret = arIgBack;
		break;
	case MuiWeldBoxSize:
		ret = weldBoxSize;
		break;
	case MuiExtrudeType:
		ret = extType;
		break;
	}
}

void EditMeshMod::SetUIParam (meshUIParam uiCode, int val) {
	if (!ip) return;
	if (!Editing()) return;
	ISpinnerControl *spin;

	switch (uiCode) {
	case MuiSelByVert:
		selByVert = val ? TRUE : FALSE;
		if (hSel) CheckDlgButton (hSel, IDC_EM_SEL_BYVERT, selByVert);
		break;
	case MuiIgBack:
		ignoreBackfaces = val ? TRUE : FALSE;
		if (hSel) CheckDlgButton (hSel, IDC_EM_IGNORE_BACKFACES, ignoreBackfaces);
		break;
	case MuiIgnoreVis:
		ignoreVisEdge = val ? TRUE : FALSE;
		if (hSel) CheckDlgButton (hSel, IDC_EM_IGNORE_VISEDGE, ignoreVisEdge);
		break;
	case MuiSoftSel:
		affectRegion = val ? TRUE : FALSE;
		if (hAR) {
			CheckDlgButton (hAR, IDC_EM_AFFECT_REGION, affectRegion);
			SetARDlgEnables ();
		}
		break;
	case MuiSSUseEDist:
		useEdgeDist = val ? TRUE : FALSE;
		if (hAR) {
			CheckDlgButton (hAR, IDC_EM_E_DIST, useEdgeDist);
			spin = GetISpinner (GetDlgItem (hAR, IDC_EM_E_ITER_SPIN));
			spin->Enable (useEdgeDist);
			ReleaseISpinner (spin);
		}
		break;
	case MuiSSEDist:
		edgeIts = val;
		if (hAR) {
			spin = GetISpinner (GetDlgItem (hAR, IDC_EM_E_ITER_SPIN));
			spin->SetValue (edgeIts, FALSE);
			ReleaseISpinner (spin);
		}
		break;
	case MuiSSBack:
		arIgBack = val ? TRUE : FALSE;
		if (hAR) CheckDlgButton (hAR, IDC_EM_AR_BACK, arIgBack);
		break;
	case MuiWeldBoxSize:
		weldBoxSize = val;
		if (hGeom) {
			spin = GetISpinner (GetDlgItem (hGeom, IDC_EM_T_THR_SPIN));
			spin->SetValue (val, FALSE);
			ReleaseISpinner (spin);
		}
		break;
	case MuiExtrudeType:
		extType = val;
		if (hGeom) {
			if (extType == MESH_EXTRUDE_CLUSTER) {
				CheckRadioButton (hGeom, IDC_EM_EXTYPE_A, IDC_EM_EXTYPE_B, IDC_EM_EXTYPE_A);
			} else {
				CheckRadioButton (hGeom, IDC_EM_EXTYPE_A, IDC_EM_EXTYPE_B, IDC_EM_EXTYPE_B);
			}
		}
		break;
	}
}

void EditMeshMod::ToggleCommandMode(meshCommandMode mode) {
	if (!ip) return;
	if (sliceMode && (mode != McmSlicePlane)) ExitSliceMode ();

	switch (mode) {
	case McmCreate:
		switch (selLevel) {
		case SL_EDGE:
		case SL_OBJECT:
			break;
		case SL_VERTEX:
			if (ip->GetCommandMode()==createVertMode)
				ip->SetStdCommandMode(CID_OBJMOVE);
			else ip->SetCommandMode(createVertMode);
			break;
		default:
			if (ip->GetCommandMode()==createFaceMode)
				ip->SetStdCommandMode(CID_OBJMOVE);
			else ip->SetCommandMode (createFaceMode);
			break;
		}
		break;

	case McmAttach:
		ip->SetPickMode (attachPickMode);
		break;

	case McmExtrude:
		if (selLevel < SL_EDGE) break;
		if (ip->GetCommandMode()==extrudeMode) ip->SetStdCommandMode(CID_OBJMOVE);
		else ip->SetCommandMode (extrudeMode);
		break;

	case McmBevel:
		if (selLevel < SL_FACE) break;
		if (ip->GetCommandMode()==bevelMode)
			ip->SetStdCommandMode (CID_OBJMOVE);
		else ip->SetCommandMode (bevelMode);
		break;

	case McmChamfer:
		if ((selLevel == SL_OBJECT) || (selLevel > SL_EDGE)) break;
		if (ip->GetCommandMode()==chamferMode)
			ip->SetStdCommandMode (CID_OBJMOVE);
		else ip->SetCommandMode (chamferMode);
		break;

	case McmSlicePlane:
		if (!selLevel) break;
		if (sliceMode) ExitSliceMode();
		else EnterSliceMode ();
		break;

	case McmCut:
		if (selLevel < SL_EDGE) break;
		if (ip->GetCommandMode()==cutEdgeMode)
			ip->SetStdCommandMode(CID_OBJMOVE);
		else ip->SetCommandMode(cutEdgeMode);
		break;

	case McmWeldTarget:
		if (selLevel != SL_VERTEX) break;
		if (ip->GetCommandMode()==weldVertMode)
			ip->SetStdCommandMode(CID_OBJMOVE);
		else ip->SetCommandMode(weldVertMode);
		break;

	case McmFlipNormalMode:
		if (selLevel < SL_FACE) break;
		if (ip->GetCommandMode()==flipMode)
			ip->SetStdCommandMode(CID_OBJMOVE);
		else ip->SetCommandMode(flipMode);
		break;

	case McmDivide:
		if (selLevel < SL_EDGE) break;
		if (selLevel == SL_EDGE) {
			if (ip->GetCommandMode()==divideEdgeMode)
				ip->SetStdCommandMode(CID_OBJMOVE);
			else ip->SetCommandMode (divideEdgeMode);
		} else {
			if (ip->GetCommandMode()==divideFaceMode)
				ip->SetStdCommandMode(CID_OBJMOVE);
			else ip->SetCommandMode (divideFaceMode);
		}
		break;

	case McmTurnEdge:
		if (selLevel != SL_EDGE) break;
		if (ip->GetCommandMode()==turnEdgeMode)
			ip->SetStdCommandMode(CID_OBJMOVE);
		else ip->SetCommandMode (turnEdgeMode);
		break;
	}
}

void EditMeshMod::ButtonOp (meshButtonOp opcode) {
	ISpinnerControl *spin;
	TSTR name;

	switch (opcode) {
	case MopHide:
		if (selLevel == SL_VERTEX) HideSelectedVerts ();
		else if (selLevel >= SL_FACE) HideSelectedFaces ();
		break;

	case MopUnhideAll:
		if (selLevel == SL_VERTEX) UnhideAllVerts();
		else if (selLevel >= SL_FACE) UnhideAllFaces ();
		break;

	case MopDelete:
		DeleteSelected ();
		break;

	case MopDetach:
		if (selLevel == SL_VERTEX) {
			BOOL elem, asClone;
			TSTR name;
			if (GetDetachObjectName (ip, name, elem, asClone)) {
				Detach (name, FALSE, !asClone, elem);
			}
		}
		if (selLevel >= SL_FACE) {
			BOOL elem, asClone;
			TSTR name;
			if (GetDetachObjectName (ip, name, elem, asClone)) {
				Detach (name, TRUE, !asClone, elem);
			}
		}
		break;

	case MopBreak:
		BreakVerts ();
		break;

	case MopViewAlign:
		AlignTo(ALIGN_VIEW);
		break;

	case MopGridAlign:
		AlignTo(ALIGN_CONST);
		break;

	case MopMakePlanar:
		if (selLevel > SL_OBJECT) MakePlanar ();
		break;

	case MopCollapse:
		if (selLevel > SL_OBJECT) Collapse ();
		break;

	case MopTessellate:
		if (selLevel > SL_EDGE) Tessellate (tessTens/400.0f, edgeTes);
		break;

	case MopExplode:
		if ((selLevel > SL_EDGE) || (selLevel == SL_OBJECT)) {
			if (expObj) {
				name = GetString(IDS_EM_NEWOBJECTNAME);
				if (GetExplodeObjectName (ip->GetMAXHWnd(), name)) {
					Explode (DegToRad(explodeThresh), TRUE, name);
				}
			} else {
				Explode (DegToRad (explodeThresh), FALSE, name);
			}
		}
		break;

	case MopSlice:
		if (sliceMode && selLevel) Slice ();
		break;

	case MopWeld:
		if (selLevel == SL_VERTEX) {
			if (!WeldVerts(weldThresh)) {
				TSTR buf1 = GetString(IDS_RB_NOVERTSTOWELD);
				TSTR buf2 = GetString(IDS_RB_WELDVERTS);
				MessageBox (ip->GetMAXHWnd(), buf1, buf2, MB_OK|MB_TASKMODAL);
			}
		}
		break;

	case MopRemoveIsolatedVerts:
		RemoveIsoVerts ();
		break;

	case MopSelectOpenEdges:
		if (selLevel == SL_EDGE) SelectOpenEdges ();
		break;

	case MopCreateShapeFromEdges:
		if (selLevel != SL_EDGE) break;
		name = GetString(IDS_EM_SHAPE);
		ip->MakeNameUnique (name);
		if (DialogBoxParam (hInstance, MAKEINTRESOURCE(IDD_EM_CREATECURVE),
			ip->GetMAXHWnd (), CurveNameDlgProc, (LPARAM)&name)) {
			ModContextList mcList;
			INodeTab nodes;
			ip->GetModContexts (mcList, nodes);
			ClearMeshDataFlag (mcList, EMD_BEENDONE);

			BOOL didSomething = FALSE;
			theHold.Begin();
			for (int nd=0; nd<mcList.Count(); nd++) {
				EditMeshData *emd = (EditMeshData *) mcList[nd]->localData;
				if (!emd || (emd->GetFlag (EMD_BEENDONE))) continue;
				Mesh *m = emd->GetMesh(ip->GetTime());
				AdjEdgeList *ae = emd->TempData(ip->GetTime())->AdjEList();
				if (CreateCurveFromMeshEdges (*m, nodes[nd], ip, ae, name,
					(createCurveType==IDC_EMCURVE_SMOOTH), curveIgnoreHiddenEdges)) didSomething = TRUE;
				emd->SetFlag (EMD_BEENDONE, TRUE);
			}
			theHold.Accept (GetString(IDS_EM_CREATECURVE));
			nodes.DisposeTemporary ();
			if (!didSomething) {
				TSTR buf1 = GetString(IDS_EM_CREATECURVE);
				TSTR buf2 = GetString(IDS_EM_NOEDGESSELECTED);
				MessageBox(ip->GetMAXHWnd(),buf2,buf1,MB_ICONEXCLAMATION|MB_OK);
				break;
			}
		}
		break;

	case MopShowNormal:
		if (!hSurf) break;
		spin = GetISpinner(GetDlgItem(hSurf,IDC_EM_NORMAL_SCALESPIN));
		if (!spin) break;
		if (IsDlgButtonChecked(hSurf, IDC_EM_NORMAL_SHOW)) {
			normScale = -1.0f;
			ShowNormals();
			CheckDlgButton (hSurf, IDC_EM_NORMAL_SHOW, FALSE);
		} else {
			normScale = spin->GetFVal();
			ShowNormals();
			CheckDlgButton (hSurf, IDC_EM_NORMAL_SHOW, TRUE);
		}
		ReleaseISpinner(spin);
		break;

	case MopFlipNormal:
		if (selLevel > SL_EDGE) FlipNormals ();
		break;

	case MopUnifyNormal:
		if (selLevel > SL_EDGE) UnifyNormals ();
		break;

	case MopAutoSmooth:
		if (selLevel > SL_EDGE) AutoSmooth (DegToRad (autoSmoothThresh));
		break;

	case MopVisibleEdge:
		if (selLevel == SL_EDGE) SetEdgeVis (TRUE);
		break;

	case MopInvisibleEdge:
		if (selLevel == SL_EDGE) SetEdgeVis (FALSE);
		break;

	case MopAutoEdge:
		if (selLevel == SL_EDGE) AutoEdge (DegToRad (autoEdgeThresh), autoEdgeType);
		break;
	}
}
