
/**********************************************************************
 *<
	FILE: editpat.cpp

	DESCRIPTION:  Edit Patch OSM

	CREATED BY: Tom Hudson, Dan Silva & Rolf Berteig

	HISTORY: created 23 June, 1995

	IMPORTANT USAGE NOTE:

		When you do an operation in edit patch which will change the topology, the form
		of the code should look like this code, taken from the vertex deletion:

		-----

			ip->GetModContexts(mcList,nodes);
			ClearPatchDataFlag(mcList,EPD_BEENDONE);

			theHold.Begin();
		-->	RecordTopologyTags();
			for ( int i = 0; i < mcList.Count(); i++ ) {
				int altered = 0;
				EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
				if ( !patchData ) continue;
				if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

				// If the mesh isn't yet cache, this will cause it to get cached.
				PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
				if(!patch)
					continue;
		-->		patchData->RecordTopologyTags(patch);

				// If this is the first edit, then the delta arrays will be allocated
				patchData->BeginEdit(t);

				// If any bits are set in the selection set, let's DO IT!!
				if(patch->vertSel.NumberSet()) {
					altered = holdNeeded = 1;
					if ( theHold.Holding() )
						theHold.Put(new PatchRestore(patchData,this,patch,"DoVertDelete"));
					// Call the vertex delete function
					DeleteSelVerts(patch);
		-->			patchData->UpdateChanges(patch);
					patchData->TempData(this)->Invalidate(PART_TOPO);
					}
				patchData->SetFlag(EPD_BEENDONE,TRUE);
				}
			
			if(holdNeeded) {
		-->		ResolveTopoChanges();
				theHold.Accept(GetString(IDS_TH_VERTDELETE));
				}
			else {
				ip->DisplayTempPrompt(GetString(IDS_TH_NOVERTSSEL),PROMPT_TIME);
				theHold.End();
				}
			
			nodes.DisposeTemporary();
			ClearPatchDataFlag(mcList,EPD_BEENDONE);

		-----

		The key elements in the "changed topology" case are the calls noted by arrows.
		These record special tags inside the object so that after the topology is changed
		by the modifier code, the UpdateChanges code can make a new mapping from the old
		object topology to the new.

		If the operation doesn't change the topology, then the three topology tag calls
		aren't needed and the UpdateChanges call becomes:

			patchData->UpdateChanges(patch, FALSE);

		This tells UpdateChanges not to bother remapping the topology.

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/
#include "mods.h"
#include "editpat.h"
#include "decomp.h"

#define DBGWELD_DUMPx
#define DBGWELD_ACTIONx

#define DBG_NAMEDSELSx

// Uncomment this for vert mapper debugging
//#define VMAP_DEBUG 1

// Forward references
BOOL CALLBACK PatchSelectDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK PatchOpsDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK PatchObjSurfDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK PatchSurfDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );

// A handy zero point
static Point3 zeroPoint(0,0,0);

// Our temporary prompts last 2 seconds:
#define PROMPT_TIME 2000

// in mods.cpp
extern HINSTANCE hInstance;

// Select by material parameters
static int sbmParams[2]     = {1,1};

// Select by smooth parameters
static DWORD sbsParams[3]   = {1,1,0};

HWND				EditPatchMod::hSelectPanel    = NULL;
HWND				EditPatchMod::hOpsPanel       = NULL;
HWND				EditPatchMod::hSurfPanel      = NULL;
BOOL				EditPatchMod::rsSel           = TRUE;
BOOL				EditPatchMod::rsOps           = TRUE;
BOOL				EditPatchMod::rsSurf          = TRUE;
IObjParam*          EditPatchMod::ip              = NULL;
MoveModBoxCMode*    EditPatchMod::moveMode        = NULL;
RotateModBoxCMode*  EditPatchMod::rotMode 	      = NULL;
UScaleModBoxCMode*  EditPatchMod::uscaleMode      = NULL;
NUScaleModBoxCMode* EditPatchMod::nuscaleMode     = NULL;
SquashModBoxCMode *	EditPatchMod::squashMode      = NULL;
SelectModBoxCMode*  EditPatchMod::selectMode      = NULL;
ISpinnerControl*	EditPatchMod::weldSpin        = NULL;
ISpinnerControl*	EditPatchMod::stepsSpin       = NULL;
//3-18-99 to suport render steps and removal of the mental tesselator
ISpinnerControl*	EditPatchMod::stepsRenderSpin       = NULL;

BOOL				EditPatchMod::settingViewportTess = FALSE;
BOOL				EditPatchMod::settingDisp     = FALSE;
ISpinnerControl*	EditPatchMod::uSpin           = NULL;
ISpinnerControl*	EditPatchMod::vSpin           = NULL;
ISpinnerControl*	EditPatchMod::edgeSpin        = NULL;
ISpinnerControl*	EditPatchMod::distSpin        = NULL;
ISpinnerControl*	EditPatchMod::angSpin         = NULL;
ISpinnerControl*	EditPatchMod::mergeSpin       = NULL;
ISpinnerControl*	EditPatchMod::matSpin         = NULL;
BOOL				EditPatchMod::patchUIValid    = TRUE;
PickPatchAttach		EditPatchMod::pickCB;
int					EditPatchMod::condenseMat     = FALSE;
int					EditPatchMod::attachMat       = ATTACHMAT_IDTOMAT;

static float weldThreshold = 0.1f;

// Checkbox items for rollup pages
static int lockedHandles = 0;
static int patchDetachCopy = 0;
static int patchDetachReorient = 0;
static int attachReorient = 0;
static BOOL filterVerts = TRUE;
static BOOL filterVecs = TRUE;


EPM_BindCMode*			EditPatchMod::bindMode   = NULL;
EPM_ExtrudeCMode*		EditPatchMod::extrudeMode   = NULL;
EPM_BevelCMode*			EditPatchMod::bevelMode   = NULL;


static void SetVertFilter() {
	patchHitLevel[EP_VERTEX] = (filterVerts ? SUBHIT_PATCH_VERTS : 0) | (filterVecs ? SUBHIT_PATCH_VECS : 0);
	}

// This is a special override value which allows us to hit-test on
// any sub-part of a patch

int patchHitOverride = 0;	// If zero, no override is done

void SetPatchHitOverride(int value) {
	patchHitOverride = value;
	}

void ClearPatchHitOverride() {
	patchHitOverride = 0;
	}

/*-------------------------------------------------------------------*/

static HIMAGELIST hFaceImages = NULL;
static void LoadImages() {
	if (hFaceImages) return;

	HBITMAP hBitmap, hMask;
	hFaceImages = ImageList_Create(24, 23, ILC_COLOR|ILC_MASK, 6, 0);
	hBitmap     = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_PATCHSELTYPES));
	hMask       = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_PATCHSELMASK));
	ImageList_Add(hFaceImages,hBitmap,hMask);
	DeleteObject(hBitmap);
	DeleteObject(hMask);
}

class EPImageListDestroyer {
	~EPImageListDestroyer() {
		if(hFaceImages)
			ImageList_Destroy(hFaceImages);
		}
	};

/*-------------------------------------------------------------------*/

class PatchDeleteUser : public EventUser {
	private:
		EditPatchMod *ep;
	public:
		void Notify() { ep->DoDeleteSelected(); }
		void SetMod(EditPatchMod *ep) { this->ep = ep; }
	};

static PatchDeleteUser pDel;

class PatchRightMenu : public RightClickMenu {
	private:
		EditPatchMod *ep;
	public:
		void Init(RightClickMenuManager* manager, HWND hWnd, IPoint2 m);
		void Selected(UINT id);
		void SetMod(EditPatchMod *ep) { this->ep = ep; }
	};

void PatchRightMenu::Init(RightClickMenuManager* manager, HWND hWnd, IPoint2 m) {
	switch(ep->GetSubobjectLevel()) {
		case EP_VERTEX:
			if(ep->RememberVertThere(hWnd, m)) {
				int oldType = -1;
				int flags1, flags2;
				flags1 = flags2 = MF_STRING;
				switch(ep->rememberedData) {
					case PVERT_COPLANAR:
						flags1 |= MF_CHECKED;
						break;
					case 0:
						flags2 |= MF_CHECKED;
						break;
					}
				manager->AddMenu(this, MF_SEPARATOR, 0, NULL);
				manager->AddMenu(this, flags1, PVERT_COPLANAR, GetString(IDS_TH_COPLANAR));
				manager->AddMenu(this, flags2, 0, GetString(IDS_TH_CORNER));
				}
			break;
		case EP_PATCH:
			if(ep->RememberPatchThere(hWnd, m)) {
				int oldType = -1;
				int flags1, flags2;
				flags1 = flags2 = MF_STRING;
				switch(ep->rememberedData) {
					case PATCH_AUTO:
						flags1 |= MF_CHECKED;
						break;
					case 0:
						flags2 |= MF_CHECKED;
						break;
					}
				manager->AddMenu(this, MF_SEPARATOR, 0, NULL);
				manager->AddMenu(this, flags1, PATCH_AUTO, GetString(IDS_TH_AUTOINTERIOR));
				manager->AddMenu(this, flags2, 0, GetString(IDS_TH_MANUALINTERIOR));
				}
			break;
		}
	}

void PatchRightMenu::Selected(UINT id) {
	switch(ep->GetSubobjectLevel()) {
		case EP_VERTEX:
			ep->SetRememberedVertType((int)id);
			break;
		case EP_PATCH:
			ep->SetRememberedPatchType((int)id);
			break;
		}
	}

PatchRightMenu pMenu;

/*-------------------------------------------------------------------*/

static
BOOL IsCompatible(BitArray &a, BitArray &b) {
	return (a.GetSize() == b.GetSize()) ? TRUE : FALSE;
	}


//--- Named Selection Set Methods ------------------------------------

// Used by EditPatchMod destructor to free pointers
void EditPatchMod::ClearSetNames()
	{
	for (int i=0; i<3; i++) {
		for (int j=0; j<namedSel[i].Count(); j++) {
			delete namedSel[i][j];
			namedSel[i][j] = NULL;
			}
		}
	}

int EditPatchMod::FindSet(TSTR &setName,int level)
	{	
	assert(level>0 && level<4);
	for (int i=0; i<namedSel[level-1].Count(); i++) {
		if (setName == *namedSel[level-1][i]) {
			return i;			
			}
		}
	return -1;
	}

void EditPatchMod::AddSet(TSTR &setName,int level)
	{
	assert(level>0 && level<4);
	TSTR *name = new TSTR(setName);
	namedSel[level-1].Append(1,&name);
	}

void EditPatchMod::RemoveSet(TSTR &setName,int level)
	{
	MaybeFixupNamedSels();
	assert(level>0 && level<4);
	int i = FindSet(setName,level);
	if (i>=0) {
		delete namedSel[level-1][i];
		namedSel[level-1].Delete(i,1);
		}
	}

static void AssignSetMatchSize(BitArray &dst, BitArray &src)
	{
	int size = dst.GetSize();
	dst = src;
	if (dst.GetSize() != size) {
		dst.SetSize(size,TRUE);
		}
	}

void EditPatchMod::ActivateSubSelSet(TSTR &setName)
	{
	MaybeFixupNamedSels();
	ModContextList mcList;
	INodeTab nodes;
	int index = FindSet(setName,selLevel);
	if (index<0 || !ip) return;	

	ip->GetModContexts(mcList,nodes);

	theHold.Begin();
	for (int i = 0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;
		patchData->BeginEdit(ip->GetTime());
		// If that set exists in this context, deal with it
		GenericNamedSelSetList &sel = patchData->GetSelSet(this);
		BitArray *set = sel.GetSet(setName);
		if(set) {
			if (theHold.Holding())
				theHold.Put(new PatchSelRestore(patchData,this,patch));
			BitArray *psel = GetLevelSelectionSet(patch);	// Get the appropriate selection set
			AssignSetMatchSize(*psel, *set);				
			PatchSelChanged();
			}

		patchData->UpdateChanges(patch, FALSE);
		if (patchData->tempData)
			patchData->TempData(this)->Invalidate(PART_SELECT);
		}
	
	theHold.Accept(GetString(IDS_DS_SELECT));
	nodes.DisposeTemporary();	
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
	}

void EditPatchMod::NewSetFromCurSel(TSTR &setName)
	{
	MaybeFixupNamedSels();

	ModContextList mcList;
	INodeTab nodes;	
	if (!ip) return;
	
	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;
		GenericNamedSelSetList &sel = patchData->GetSelSet(this);
		BitArray *exist = sel.GetSet(setName);	
		switch (selLevel) {
			case EP_VERTEX:	
				if (exist) {
					*exist = patch->vertSel;
				} else {
					patchData->vselSet.AppendSet(patch->vertSel, 0, setName);
					}
				break;

			case EP_PATCH:
				if (exist) {
					*exist = patch->patchSel;
				} else {
					patchData->pselSet.AppendSet(patch->patchSel, 0, setName);
					}
				break;

			case EP_EDGE:
				if (exist) {
					*exist = patch->edgeSel;
				} else {
					patchData->eselSet.AppendSet(patch->edgeSel, 0, setName);
					}
				break;
			}
		}	
	
	int index = FindSet(setName,selLevel);
	if (index<0)
		AddSet(setName,selLevel);		
	nodes.DisposeTemporary();
	}

void EditPatchMod::RemoveSubSelSet(TSTR &setName)
	{
	MaybeFixupNamedSels();

	ModContextList mcList;
	INodeTab nodes;

	if (!ip) return;	
	
	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData) continue;		
		patchData->BeginEdit(ip->GetTime());
		GenericNamedSelSetList &sel = patchData->GetSelSet(this);
		sel.RemoveSet(setName);
		}
	// Remove the modifier's entry
	RemoveSet(setName,selLevel);
	ip->ClearCurNamedSelSet();
	SetupNamedSelDropDown();
	nodes.DisposeTemporary();
	}

void EditPatchMod::SetupNamedSelDropDown()
	{
	// Setup named selection sets	
	if (selLevel == EP_OBJECT)
		return;
	ip->ClearSubObjectNamedSelSets();
	for (int i=0; i<namedSel[selLevel-1].Count(); i++)
		ip->AppendSubObjectNamedSelSet(*namedSel[selLevel-1][i]);
	}

int EditPatchMod::NumNamedSelSets() {
	if(GetSubobjectLevel() == PO_OBJECT)
		return 0;
	return namedSel[selLevel-1].Count();
	}

TSTR EditPatchMod::GetNamedSelSetName(int i) {
	return *namedSel[selLevel-1][i];
	}

class EPSelSetNameRestore : public RestoreObj {
	public:
		TSTR undo, redo;
		TSTR *target;
		EditPatchMod *mod;
		EPSelSetNameRestore(EditPatchMod *m, TSTR *t, TSTR &newName) {
			mod = m;
			undo = *t;
			target = t;
			}
		void Restore(int isUndo) {			
			if(isUndo)
				redo = *target;
			*target = undo;
			if (mod->ip)
				mod->ip->NamedSelSetListChanged();
			}
		void Redo() {
			*target = redo;
			if (mod->ip)
				mod->ip->NamedSelSetListChanged();
			}
				
		TSTR Description() {return TSTR(_T("Sel Set Name"));}
	};

void EditPatchMod::SetNamedSelSetName(int index, TSTR &newName) {
	if(!ip) return;
	MaybeFixupNamedSels();

	// First do the master name list
	if (theHold.Holding())
		theHold.Put(new EPSelSetNameRestore(this, namedSel[selLevel-1][index], newName));

	// Save the old name so we can change those in the EditPatchData
	TSTR oldName = *namedSel[selLevel-1][index];
	*namedSel[selLevel-1][index] = newName;

	ModContextList mcList;
	INodeTab nodes;
	
	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (patchData)
			patchData->GetSelSet(this).RenameSet(oldName, newName);
		}
	nodes.DisposeTemporary();
	}

void EditPatchMod::NewSetByOperator(TSTR &newName,Tab<int> &sets,int op) {
	MaybeFixupNamedSels();

	// First do it in the master name list
	AddSet(newName,selLevel);		
// TO DO: Undo?
	ModContextList mcList;
	INodeTab nodes;	

	if (!ip) return;
	
	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData ) continue;		
		GenericNamedSelSetList &set = patchData->GetSelSet(this);
		BitArray bits = *set.GetSetByIndex(sets[0]);
		for (i=1; i<sets.Count(); i++) {
			BitArray *bit2 = set.GetSetByIndex(sets[i]);
			switch (op) {
				case NEWSET_MERGE:
					bits |= *bit2;
					break;

				case NEWSET_INTERSECTION:
					bits &= *bit2;
					break;

				case NEWSET_SUBTRACT:
					bits &= ~(*bit2);
					break;
				}
			}
		set.AppendSet(bits,0,newName);
		}	
	
	nodes.DisposeTemporary();
	}

// Named selection set copy/paste methods follow...

static BOOL CALLBACK PickSetNameDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	static TSTR *name;

	switch (msg) {
		case WM_INITDIALOG: {
			name = (TSTR*)lParam;
			ICustEdit *edit =GetICustEdit(GetDlgItem(hWnd,IDC_SET_NAME));
			edit->SetText(*name);
			ReleaseICustEdit(edit);
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: {
					ICustEdit *edit =GetICustEdit(GetDlgItem(hWnd,IDC_SET_NAME));
					TCHAR buf[256];
					edit->GetText(buf,256);
					*name = TSTR(buf);
					ReleaseICustEdit(edit);
					EndDialog(hWnd,1);
					break;
					}

				case IDCANCEL:
					EndDialog(hWnd,0);
					break;
				}
			break;

		default:
			return FALSE;
		};
	return TRUE;
	}

BOOL EditPatchMod::GetUniqueSetName(TSTR &name) {
	while (1) {		
		if(FindSet(name, selLevel) < 0)
			break;

		if (!DialogBoxParam(
			hInstance, 
			MAKEINTRESOURCE(IDD_PASTE_NAMEDSET),
			ip->GetMAXHWnd(), 
			PickSetNameDlgProc,
			(LPARAM)&name)) return FALSE;		
		}
	return TRUE;
	}

static BOOL CALLBACK PickSetDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{	
	switch (msg) {
		case WM_INITDIALOG:	{
			Tab<TSTR*> &names = *((Tab<TSTR*>*)lParam);
			for (int i=0; i<names.Count(); i++) {
				int pos  = SendDlgItemMessage(hWnd,IDC_NS_LIST,LB_ADDSTRING,0,
					(LPARAM)(TCHAR*)*names[i]);
				SendDlgItemMessage(hWnd,IDC_NS_LIST,LB_SETITEMDATA,pos,i);
				}
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_NS_LIST:
					if (HIWORD(wParam)!=LBN_DBLCLK) break;
					// fall through
				case IDOK: {
					int sel = SendDlgItemMessage(hWnd,IDC_NS_LIST,LB_GETCURSEL,0,0);
					if (sel!=LB_ERR) {
						int res =SendDlgItemMessage(hWnd,IDC_NS_LIST,LB_GETITEMDATA,sel,0);
						EndDialog(hWnd,res);
						break;
						}
					// fall through
					}

				case IDCANCEL:
					EndDialog(hWnd,-1);
					break;
				}
			break;

		default:
			return FALSE;
		};
	return TRUE;
	}

int EditPatchMod::SelectNamedSet() {
	Tab<TSTR*> names = namedSel[selLevel-1];
	return DialogBoxParam(
		hInstance, 
		MAKEINTRESOURCE(IDD_SEL_NAMEDSET),
		ip->GetMAXHWnd(), 
		PickSetDlgProc,
		(LPARAM)&names);
	}

void EditPatchMod::NSCopy() {
	MaybeFixupNamedSels();
	if (selLevel == EP_OBJECT) return;
	int index = SelectNamedSet();
	if(index < 0) return;
	if(!ip) return;
	// Get the name for that index
	int nsl = namedSetLevel[selLevel];
	TSTR setName = *namedSel[nsl][index];
	PatchNamedSelClip *clip = new PatchNamedSelClip(setName);

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData) continue;

		GenericNamedSelSetList &setList = patchData->GetSelSet(this);
		BitArray *set = setList.GetSet(setName);
		if(set) {
			BitArray *bits = new BitArray(*set);
			clip->sets.Append(1,&bits);
			}
		}
	SetPatchNamedSelClip(clip, namedClipLevel[selLevel]);

	// Enable the paste button
	ICustButton *but = GetICustButton(GetDlgItem(hSelectPanel,IDC_NS_PASTE));
	but->Enable();
	ReleaseICustButton(but);
	}

void EditPatchMod::NSPaste() {
	MaybeFixupNamedSels();
	if (selLevel == EP_OBJECT) return;
	int nsl = namedSetLevel[selLevel];
	PatchNamedSelClip *clip = GetPatchNamedSelClip(namedClipLevel[selLevel]);
	if (!clip) return;
	TSTR name = clip->name;
	if (!GetUniqueSetName(name)) return;
	if(!ip) return;

	ModContextList mcList;
	INodeTab nodes;

	AddSet(name, selLevel);

	ip->GetModContexts(mcList,nodes);
	for (int i = 0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData) continue;

		GenericNamedSelSetList & setList = patchData->GetSelSet(this);

		if (i>=clip->sets.Count()) {
			BitArray bits;
			setList.AppendSet(bits, 0, name);
			}
		else
			setList.AppendSet(*clip->sets[i], 0, name);
		}	
	
	ActivateSubSelSet(name);
	ip->SetCurNamedSelSet(name);
	SetupNamedSelDropDown();
	}

// Old MAX files (pre-r3) have EditPatchData named selections without names assigned.  This
// assigns them their proper names for r3 and later code.  If no fixup is required, this does nothing.
void EditPatchMod::MaybeFixupNamedSels() {
	int i;
	if(!ip) return;

	// Go thru the modifier contexts, and stuff the named selection names into the EditPatchData
	ModContextList mcList;
	INodeTab nodes;
	
	ip->GetModContexts(mcList,nodes);

#ifdef DBG_NAMEDSELS
	DebugPrint("Context/named sels:\n");
	for (i=0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData) continue;		

		// Go thru each subobject level
		for(int j = 0; j < 3; ++j) {
			GenericNamedSelSetList &pdSel = patchData->GetSelSet(j);
			for(int k = 0; k < pdSel.Count(); ++k)
				DebugPrint("Context %d, level %d, set %d: [%s]\n", i, j, k, *pdSel.names[k]);
			}
		}	
#endif //DBG_NAMEDSELS

	if(!namedSelNeedsFixup) {
#ifdef DBG_NAMEDSELS
		DebugPrint("!!! NO FIXUP REQUIRED !!!\n");
#endif //DBG_NAMEDSELS
		return;
		}

#ifdef DBG_NAMEDSELS
	DebugPrint("*** Fixing up named sels ***\n");
#endif //DBG_NAMEDSELS

	for (i=0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData) continue;		

		// Go thru each subobject level
		for(int j = 0; j < 3; ++j) {
			Tab<TSTR*> &mSel = namedSel[j];
			GenericNamedSelSetList &pdSel = patchData->GetSelSet(j);
			// Some old files may have improper counts in the EditPatchData.  Limit the counter
			int mc = mSel.Count();
			int pdc = pdSel.Count();
			int limit = (mc < pdc) ? mc : pdc;
#ifdef DBG_NAMEDSELS
			if(mc != pdc)
				DebugPrint("****** mSel.Count=%d, pdSel.Count=%d ******\n", mc, pdc);
#endif //DBG_NAMEDSELS
			for(int k = 0; k < limit; ++k)
				*pdSel.names[k] = *mSel[k];
			}
		}	
	
	nodes.DisposeTemporary();
	namedSelNeedsFixup = FALSE;
	}

void EditPatchMod::RemoveAllSets()
	{
	ModContextList mcList;
	INodeTab nodes;
	
	if (!ip) return;	
	
	ip->GetModContexts(mcList,nodes);

	for (int i=0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData) continue;		
		
		int j;
		for (j=patchData->vselSet.Count()-1; j>=0; j--) {
			patchData->vselSet.DeleteSet(j);
			}
		for (j=patchData->pselSet.Count()-1; j>=0; j--) {
			patchData->pselSet.DeleteSet(j);
			}
		for (j=patchData->eselSet.Count()-1; j>=0; j--) {
			patchData->eselSet.DeleteSet(j);
			}		
		}	
	
	for (int j=0; j<3; j++) {
		for (int i=0; i<namedSel[j].Count(); i++) {
			delete namedSel[j][i];		
			}
		namedSel[j].Resize(0);
		}

	ip->ClearCurNamedSelSet();
	ip->ClearSubObjectNamedSelSets();
	nodes.DisposeTemporary();
	}


/*-------------------------------------------------------------------*/

// Deletes any vertices tagged, also any patches tagged.  Automatically deletes the vectors that
// are deleted as a result of the patch deletion and sweeps any vertices floating in space.

static void DeletePatchParts(PatchMesh *patch, BitArray &delVerts, BitArray &delPatches) {
	int patches = patch->getNumPatches();
	int verts = patch->getNumVerts();
	int vecs = patch->getNumVecs();
	int dest;

	// We treat vectors specially in order to clean up after welds.  First, we tag 'em all,
	// then untag only those on unselected patches so that any dangling vectors will be deleted.
	BitArray delVectors(vecs);
	delVectors.SetAll();

	// Untag vectors that are on nondeleted patches
	for(int i = 0; i < patches; ++i) {
		if(!delPatches[i]) {
			Patch& p = patch->patches[i];
			for(int j = 0; j < (p.type * 2); ++j) {
				delVectors.Clear(p.vec[j]);
				}
			for(j = 0; j < p.type; ++j)
				delVectors.Clear(p.interior[j]);
			}
		}

	// Make a table of vertices that are still in use -- Used to
	// delete those vertices which are floating, unused, in space.
	BitArray usedVerts(verts);
	usedVerts.ClearAll();
	for(i = 0; i < patches; ++i) {
		if(!delPatches[i]) {
			Patch& p = patch->patches[i];
			for(int j = 0; j < p.type; ++j) {
				usedVerts.Set(p.v[j]);
				}
			}
		}
	for(i = 0; i < verts; ++i) {
		if(!usedVerts[i])
			delVerts.Set(i);
		}

	// If we have texture vertices, handle them, too
	for(int chan = 0; chan < patch->getNumMaps(); ++chan) {
		int tverts = patch->numTVerts[chan];
		if(tverts && patch->tvPatches[chan]) {
			BitArray delTVerts(tverts);
			delTVerts.SetAll();
			for(i = 0; i < patches; ++i) {
				if(!delPatches[i]) {
					Patch& p = patch->patches[i];
					TVPatch& tp = patch->tvPatches[chan][i];
					for(int j = 0; j < p.type; ++j)
						delTVerts.Clear(tp.tv[j]);
					}
				}
			// Got the list of tverts to delete -- now delete 'em
			// Build a table of redirected texture vertex indices
			int newTVerts = tverts - delTVerts.NumberSet();
			IntTab tVertIndex;
			tVertIndex.SetCount(tverts);
			UVVert *newTVertArray = new UVVert[newTVerts];
			dest = 0;
			for(i = 0; i < tverts; ++i) {
				if(!delTVerts[i]) {
					newTVertArray[dest] = patch->tVerts[chan][i];
					tVertIndex[i] = dest++;
					}
				}
			delete[] patch->tVerts[chan];
			patch->tVerts[chan] = newTVertArray;
			patch->numTVerts[chan] = newTVerts;
			// Now, copy the untagged texture patches to a new array
			// While you're at it, redirect the vertex indices
			int newTVPatches = patches - delPatches.NumberSet();
			TVPatch *newArray = new TVPatch[newTVPatches];
			dest = 0;
			for(i = 0; i < patches; ++i) {
				if(!delPatches[i]) {
					Patch& p = patch->patches[i];
					TVPatch& tp = newArray[dest++];
					tp = patch->tvPatches[chan][i];
					for(int j = 0; j < p.type; ++j)
						tp.tv[j] = tVertIndex[tp.tv[j]];
					}
				}
			delete[] patch->tvPatches[chan];
			patch->tvPatches[chan] = newArray;;
			}
		}

	// Build a table of redirected vector indices
	IntTab vecIndex;
	vecIndex.SetCount(vecs);
	int newVectors = vecs - delVectors.NumberSet();
	PatchVec *newVecArray = new PatchVec[newVectors];
	dest = 0;
	for(i = 0; i < vecs; ++i) {
		if(!delVectors[i]) {
			newVecArray[dest] = patch->vecs[i];
			vecIndex[i] = dest++;
			}
		else
			vecIndex[i] = -1;
		}
	delete[] patch->vecs;
	patch->vecs = newVecArray;
	patch->numVecs = newVectors;

	// Build a table of redirected vertex indices
	int newVerts = verts - delVerts.NumberSet();
	IntTab vertIndex;
	vertIndex.SetCount(verts);
	PatchVert *newVertArray = new PatchVert[newVerts];
	BitArray newVertSel(newVerts);
	newVertSel.ClearAll();
	dest = 0;
	for(i = 0; i < verts; ++i) {
		if(!delVerts[i]) {
			newVertArray[dest] = patch->verts[i];
			newVertSel.Set(dest, patch->vertSel[i]);
			// redirect & adjust attached vector list
			PatchVert& v = newVertArray[dest];
			for(int j = 0; j < v.vectors.Count(); ++j) {
				v.vectors[j] = vecIndex[v.vectors[j]];
				if(v.vectors[j] < 0) {
					v.vectors.Delete(j, 1);
					j--;	// realign index
					}
				}
			vertIndex[i] = dest++;
			}
		}
	delete[] patch->verts;
	patch->verts = newVertArray;
	patch->numVerts = newVerts;
	patch->vertSel = newVertSel;

	// Now, copy the untagged patches to a new array
	// While you're at it, redirect the vertex and vector indices
	int newPatches = patches - delPatches.NumberSet();
	Patch *newArray = new Patch[newPatches];
	BitArray newPatchSel(newPatches);
	newPatchSel.ClearAll();
	dest = 0;
	for(i = 0; i < patches; ++i) {
		if(!delPatches[i]) {
			newArray[dest] = patch->patches[i];
			Patch& p = newArray[dest];
			for(int j = 0; j < p.type; ++j)
				p.v[j] = vertIndex[p.v[j]];
			for(j = 0; j < (p.type * 2); ++j)
				p.vec[j] = vecIndex[p.vec[j]];
			for(j = 0; j < p.type; ++j)
				p.interior[j] = vecIndex[p.interior[j]];
			newPatchSel.Set(dest++, patch->patchSel[i]);
			}
		}
	delete[] patch->patches;
	patch->patches = newArray;;
	patch->numPatches = newPatches;
	patch->patchSel.SetSize(newPatches,TRUE);
	patch->patchSel = newPatchSel;
	patch->buildLinkages();
	}

/*-------------------------------------------------------------------*/

// This function checks the current command mode and resets it to CID_OBJMOVE if
// it's one of our command modes

static
void CancelEditPatchModes(IObjParam *ip) {
	switch(ip->GetCommandMode()->ID()) {
		case CID_STDPICK:
			ip->SetStdCommandMode( CID_OBJMOVE );
			break;
		}
	}

// This gets rid of two-step modes, like booleans.  This is necessary because
// the first step, which activates the mode button, validates the selection set.
// If the selection set changes, the mode must be turned off because the new
// selection set may not be valid for the mode.
static
void Cancel2StepPatchModes(IObjParam *ip) {
//	switch(ip->GetCommandMode()->ID()) {
//		case CID_BOOLEAN:
//			ip->SetStdCommandMode( CID_OBJMOVE );
//			break;
//		}
	}

/*-------------------------------------------------------------------*/

static
TSTR detachName;

static
BOOL CALLBACK DetachDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	TCHAR tempName[256];
	switch(message) {
		case WM_INITDIALOG:
			SetDlgItemText(hDlg, IDC_DETACH_NAME, detachName);
			SetFocus(GetDlgItem(hDlg, IDC_DETACH_NAME));
			return FALSE;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDOK:
					GetDlgItemText(hDlg, IDC_DETACH_NAME, tempName, 255);
					detachName = TSTR(tempName);
					EndDialog(hDlg, 1);
					return TRUE;
				case IDCANCEL:
					EndDialog(hDlg, 0);
					return TRUE;
			}
		}
	return FALSE;
	}

static
int GetDetachOptions(IObjParam *ip, TSTR& newName) {
	detachName = newName;
	ip->MakeNameUnique(detachName);	
	if(DialogBox(hInstance, MAKEINTRESOURCE(IDD_DETACH), ip->GetMAXHWnd(), (DLGPROC)DetachDialogProc)==1) {
		newName = detachName;
		return 1;
		}
	return 0;
	}

/*-------------------------------------------------------------------*/

static EditPatchClassDesc editPatchDesc;
extern ClassDesc* GetEditPatchModDesc() { return &editPatchDesc; }

void EditPatchClassDesc::ResetClassParams(BOOL fileReset)
	{
	sbmParams[0]   = 1;
	sbmParams[1]   = 1;
	EditPatchMod::condenseMat = FALSE;
	EditPatchMod::attachMat = ATTACHMAT_IDTOMAT;
	}

/*-------------------------------------------------------------------*/

PatchPointTab::PatchPointTab() {
	}

PatchPointTab::~PatchPointTab() {
	}

void PatchPointTab::Empty() {
	ptab.Delete(0, ptab.Count());
	vtab.Delete(0, vtab.Count());
	pttab.Delete(0, pttab.Count());
	}

void PatchPointTab::Zero() {
//DebugPrint("Zeroing\n");
	int points = ptab.Count();
	int vectors = vtab.Count();
	Point3 zero(0, 0, 0);

	for(int i = 0; i < points; ++i) {
		ptab[i] = zero;
		pttab[i] = 0;
		}
	for(i = 0; i < vectors; ++i)
		vtab[i] = zero;
	}

void PatchPointTab::MakeCompatible(PatchMesh& patch,int clear) {
	int izero = 0;
	if(clear) {
		ptab.Delete(0, ptab.Count());
		pttab.Delete(0, pttab.Count());
		vtab.Delete(0, vtab.Count());
		}
	// First, the verts
	int size = patch.numVerts;
	if(ptab.Count() > size) {
		int diff = ptab.Count() - size;
		ptab.Delete( ptab.Count() - diff, diff );
		pttab.Delete( pttab.Count() - diff, diff );
		}
	if(ptab.Count() < size) {
		int diff = size - ptab.Count();
		ptab.Resize( size );
		pttab.Resize( size );
		for( int j = 0; j < diff; j++ ) {
			ptab.Append(1,&zeroPoint);
			pttab.Append(1,&izero);
			}
		}
	// Now, the vectors
	size = patch.numVecs;
	if(vtab.Count() > size) {
		int diff = vtab.Count() - size;
		vtab.Delete( vtab.Count() - diff, diff );
		}
	if(vtab.Count() < size) {
		int diff = size - vtab.Count();
		vtab.Resize( size );
		for( int j = 0; j < diff; j++ )
			vtab.Append(1,&zeroPoint);
		}
	}

PatchPointTab& PatchPointTab::operator=(PatchPointTab& from) {
	ptab = from.ptab;
	vtab = from.vtab;
	pttab = from.pttab;
	return *this;
	}

BOOL PatchPointTab::IsCompatible(PatchMesh &patch) {
	if(ptab.Count() != patch.numVerts)
		return FALSE;
	if(pttab.Count() != patch.numVerts)
		return FALSE;
	if(vtab.Count() != patch.numVecs)
		return FALSE;
	return TRUE;
	}

void PatchPointTab::RescaleWorldUnits(float f) {
	Matrix3 stm = ScaleMatrix(Point3(f, f, f));
	int points = ptab.Count();
	int vectors = vtab.Count();

	for(int i = 0; i < points; ++i)
		ptab[i] = ptab[i] * stm;
	for(i = 0; i < vectors; ++i)
		vtab[i] = vtab[i] * stm;
	}

#define PPT_VERT_CHUNK		0x1000
#define PPT_VEC_CHUNK		0x1010
#define PPT_VERTTYPE_CHUNK	0x1020

IOResult PatchPointTab::Save(ISave *isave) {	
	int i;
	ULONG nb;
	isave->BeginChunk(PPT_VERT_CHUNK);
	int count = ptab.Count();
	isave->Write(&count,sizeof(int),&nb);
	for(i = 0; i < count; ++i)
		isave->Write(&ptab[i],sizeof(Point3),&nb);
	isave->EndChunk();
	isave->BeginChunk(PPT_VERTTYPE_CHUNK);
	count = pttab.Count();
	isave->Write(&count,sizeof(int),&nb);
	for(i = 0; i < count; ++i)
		isave->Write(&pttab[i],sizeof(int),&nb);
	isave->EndChunk();
	isave->BeginChunk(PPT_VEC_CHUNK);
	count = vtab.Count();
	isave->Write(&count,sizeof(int),&nb);
	for(i = 0; i < count; ++i)
		isave->Write(&vtab[i],sizeof(Point3),&nb);
	isave->EndChunk();
	return IO_OK;
	}

IOResult PatchPointTab::Load(ILoad *iload) {	
	int i, count;
	Point3 workpt;
	int workint;
	IOResult res;
	ULONG nb;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case PPT_VERT_CHUNK:
				ptab.Delete(0,ptab.Count());
				iload->Read(&count,sizeof(int),&nb);
				for(i = 0; i < count; ++i) {
					iload->Read(&workpt,sizeof(Point3),&nb);
					ptab.Append(1,&workpt);
					}
				break;
			case PPT_VERTTYPE_CHUNK:
				pttab.Delete(0,pttab.Count());
				iload->Read(&count,sizeof(int),&nb);
				for(i = 0; i < count; ++i) {
					iload->Read(&workint,sizeof(int),&nb);
					pttab.Append(1,&workint);
					}
				break;
			case PPT_VEC_CHUNK:
				vtab.Delete(0,vtab.Count());
				iload->Read(&count,sizeof(int),&nb);
				for(i = 0; i < count; ++i) {
					iload->Read(&workpt,sizeof(Point3),&nb);
					vtab.Append(1,&workpt);
					}
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/

void PatchVertexDelta::SetSize(PatchMesh& patch, BOOL load)
	{
	dtab.MakeCompatible(patch, FALSE);
	
	// Load it if necessary
	if(load) {
		int verts = patch.numVerts;
		int vecs = patch.numVecs;
		for(int i = 0; i < verts; ++i) {
			dtab.ptab[i] = patch.verts[i].p;
			dtab.pttab[i] = patch.verts[i].flags & PVERT_COPLANAR;
			}
		for(i = 0; i < vecs; ++i)
			dtab.vtab[i] = patch.vecs[i].p;
		}
	}

void PatchVertexDelta::Apply(PatchMesh &patch)
	{
//DebugPrint(_T("PVD:Applying\n"));
	// This does nothing if the number of verts hasn't changed in the mesh.
	SetSize(patch, FALSE);

	// Apply the deltas
	int verts = patch.numVerts;
	int vecs = patch.numVecs;
	for(int i = 0; i < verts; ++i) {
		patch.verts[i].p += dtab.ptab[i];
		patch.verts[i].flags ^= dtab.pttab[i];
		}
	for(i = 0; i < vecs; ++i) {
		patch.vecs[i].p += dtab.vtab[i];
		}
	patch.computeInteriors();
	}

void PatchVertexDelta::UnApply(PatchMesh &patch)
	{
//DebugPrint(_T("PVD:UnApplying\n"));
	// This does nothing if the number of verts hasn't changed in the mesh.
	SetSize(patch, FALSE);

	// Apply the deltas
	int verts = patch.numVerts;
	int vecs = patch.numVecs;
	for(int i = 0; i < verts; ++i) {
		patch.verts[i].p -= dtab.ptab[i];
		patch.verts[i].flags ^= dtab.pttab[i];
		}
	for(i = 0; i < vecs; ++i) {
		patch.vecs[i].p -= dtab.vtab[i];
		}
	patch.computeInteriors();
	}

// This function applies the current changes to slave handles and their knots, and zeroes everything else
void PatchVertexDelta::ApplyHandlesAndZero(PatchMesh &patch, int handleVert) {
//DebugPrint(_T("PVD:ApplyAndZero\n"));
	// This does nothing if the number of verts hasn't changed in the mesh.
	SetSize(patch, FALSE);

	Point3 zeroPt(0.0f, 0.0f, 0.0f);

	// Apply the deltas	to just the slave handles
	int verts = patch.numVerts;
	int vecs = patch.numVecs;
	Point3Tab& delta = dtab.vtab;
	IntTab& kdelta = dtab.pttab;
	for(int i = 0; i < vecs; ++i) {
		if(!(delta[i] == zeroPt)) {
			if(i != handleVert)
				patch.vecs[i].p += delta[i];
			else
				delta[i] = zeroPt;
			}
		}

	for(i = 0; i < verts; ++i) {
		if(kdelta[i])
			patch.verts[i].flags ^= kdelta[i];
		}
	}


#define PVD_POINTTAB_CHUNK		0x1000

IOResult PatchVertexDelta::Save(ISave *isave) {
	isave->BeginChunk(PVD_POINTTAB_CHUNK);
	dtab.Save(isave);
	isave->	EndChunk();
	return IO_OK;
	}

IOResult PatchVertexDelta::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case PVD_POINTTAB_CHUNK:
				res = dtab.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/

EPVertMapper::~EPVertMapper() {
	if(vertMap) {
		delete [] vertMap;
		vertMap = NULL;
		}
	if(vecMap) {
		delete [] vecMap;
		vecMap = NULL;
		}
	}

void EPVertMapper::Build(PatchMesh &patch) {
	verts = patch.numVerts;
	if(vertMap)
		delete [] vertMap;
	vertMap = new EPMapVert[verts];
	vecs = patch.numVecs;
	if(vecMap)
		delete [] vecMap;
	vecMap = new EPMapVert[vecs];
	for(int i = 0; i < verts; ++i)
		vertMap[i] = EPMapVert(i, patch.verts[i].p, zeroPoint);
	for(i = 0; i < vecs; ++i)
		vecMap[i] = EPMapVert(i, patch.vecs[i].p, zeroPoint);
	}

void EPVertMapper::RecordTopologyTags(PatchMesh &patch) {
	for(int i = 0; i < verts; ++i) {
		// If it's still mapped, record it!
		if(vertMap[i].vert >= 0)
			patch.verts[vertMap[i].vert].aux1 = i;
		}
	for(i = 0; i < vecs; ++i) {
		// If it's still mapped, record it!
		if(vecMap[i].vert >= 0)
			patch.vecs[vecMap[i].vert].aux1 = i;
		}
	}

void EPVertMapper::UpdateMapping(PatchMesh &patch) {
	// Flush existing mapping
	for(int i = 0; i < verts; ++i)
		vertMap[i].vert = -1;
	for(i = 0; i < vecs; ++i)
		vecMap[i].vert = -1;
	// Build the new mapping
	int wverts = patch.numVerts;
	for(int wvert = 0; wvert < wverts; ++wvert) {
		int aux = patch.verts[wvert].aux1;
		if(aux != 0xffffffff) {
			if(aux >=0 && aux < verts)
				vertMap[aux].vert = wvert;
			}
		}
	int wvecs = patch.numVecs;
	for(int wvec = 0; wvec < wvecs; ++wvec) {
		int aux = patch.vecs[wvec].aux1;
		if(aux != 0xffffffff) {
			if(aux >= 0 && aux < vecs)
				vecMap[aux].vert = wvec;
			}
		}
	}

void EPVertMapper::RecomputeDeltas(PatchMesh &patch) {
	for(int i = 0; i < verts; ++i) {
		EPMapVert &map = vertMap[i];
		if(map.vert >= 0 && map.originalStored) {
			Point3 pnew = patch.verts[map.vert].p;
#ifdef VMAP_DEBUG
			Point3 oldDelta = map.delta;
#endif
			map.delta = pnew - map.original;
#ifdef VMAP_DEBUG
			if(map.delta != oldDelta)
				DebugPrint("Vert %d delta changed from %.2f %.2f %.2f to %.2 %.2f %.2f\n",i,oldDelta.x,oldDelta.y,oldDelta.z,map.delta.x,map.delta.y,map.delta.z);
#endif
			}
		}
	for(i = 0; i < vecs; ++i) {
		EPMapVert &map = vecMap[i];
		if(map.vert >= 0 && map.originalStored) {
			Point3 pnew = patch.vecs[map.vert].p;
#ifdef VMAP_DEBUG
			Point3 oldDelta = map.delta;
#endif
			map.delta = pnew - map.original;
#ifdef VMAP_DEBUG
			if(map.delta != oldDelta)
				DebugPrint("Vec %d delta changed from %.2f %.2f %.2f to %.2 %.2f %.2f\n",i,oldDelta.x,oldDelta.y,oldDelta.z,map.delta.x,map.delta.y,map.delta.z);
#endif
			}
		}
	}

void EPVertMapper::UpdateAndApplyDeltas(PatchMesh &inPatch, PatchMesh &outPatch) {

	//watje 4-27-99 here to handle 0 patch situations
	if (inPatch.numPatches == 0)
		{
//		outPatch.setNumVerts(0,TRUE); 
		return;
		}
	

	// Update the original point locations
	for(int i = 0; i < verts; ++i) {
		// If this table has more in it than we need, forget the rest
		// This can happen if the input object changes to fewer verts
		if(i >= inPatch.numVerts)
			break;
		// If it's still mapped, update it!
		if(vertMap[i].vert >= 0) {
			vertMap[i].original = inPatch.verts[i].p;
			vertMap[i].originalStored = TRUE;
			}
		}
	for(i = 0; i < vecs; ++i) {
		// If this table has more in it than we need, forget the rest
		// This can happen if the input object changes to fewer vecs
		if(i >= inPatch.numVecs)
			break;
		// If it's still mapped, update it!
		if(vecMap[i].vert >= 0) {
			vecMap[i].original = inPatch.vecs[i].p;
			vecMap[i].originalStored = TRUE;
			}
		}
	// Now apply to output
	for(i = 0; i < verts; ++i) {
		EPMapVert &pv = vertMap[i];
		if(pv.vert >= 0 && pv.originalStored) {
//assert(pv.vert >= 0 && pv.vert < outPatch.numVerts);
//watje 4-27-99 instead just throwing an assert it pops a message box up and troes to recover
if (!(pv.vert >= 0 && pv.vert < outPatch.numVerts))
	{
	outPatch.setNumVerts(pv.vert+1,TRUE); 
	TSTR title = GetString(IDS_TH_EDITPATCH_CLASS),
	warning = GetString(IDS_PW_SURFACEERROR);

	MessageBox(GetCOREInterface()->GetMAXHWnd(),
		warning, title, MB_OK|MB_APPLMODAL );
	}

			if(i >= inPatch.numVerts) 
				outPatch.verts[pv.vert].p = zeroPoint;
			else
				outPatch.verts[pv.vert].p = pv.original + pv.delta;
#ifdef VMAP_DEBUG
			if(pv.delta != zeroPoint)
				DebugPrint("Vert %d applied delta of %.2f %.2f %.2f\n",i,pv.delta.x,pv.delta.y,pv.delta.z);
#endif
			}
		}
	for(i = 0; i < vecs; ++i) {
		EPMapVert &pv = vecMap[i];
		if(pv.vert >= 0 && pv.originalStored) {
//assert(pv.vert >= 0 && pv.vert < outPatch.numVecs);
//watje 4-27-99 instead just throwing an assert it pops a message box up and troes to recover
if (!(pv.vert >= 0 && pv.vert < outPatch.numVecs))
	{
	outPatch.setNumVecs(pv.vert+1,TRUE); 

	TSTR title = GetString(IDS_TH_EDITPATCH_CLASS),
	warning = GetString(IDS_PW_SURFACEERROR);

	MessageBox(GetCOREInterface()->GetMAXHWnd(),
		warning, title, MB_OK|MB_APPLMODAL );
	}

			if(i >= inPatch.numVecs) 
				outPatch.vecs[pv.vert].p = zeroPoint;
			else
				outPatch.vecs[pv.vert].p = pv.original + pv.delta;
#ifdef VMAP_DEBUG
			if(pv.delta != zeroPoint)
				DebugPrint("Vec %d applied delta of %.2f %.2f %.2f\n",i,pv.delta.x,pv.delta.y,pv.delta.z);
#endif
			}
		}
	}

EPVertMapper& EPVertMapper::operator=(EPVertMapper &from) {
	if(vertMap)
		delete [] vertMap;
	verts = from.verts;
	vertMap = new EPMapVert[verts];
	for(int i = 0; i < verts; ++i)
		vertMap[i] = from.vertMap[i];
	if(vecMap)
		delete [] vecMap;
	vecs = from.vecs;
	vecMap = new EPMapVert[vecs];
	for(i = 0; i < vecs; ++i)
		vecMap[i] = from.vecMap[i];
	return *this;
	}

void EPVertMapper::RescaleWorldUnits(float f) {
	for(int i = 0; i < verts; ++i) {
		vertMap[i].delta *= f;
		if(vertMap[i].originalStored)
			vertMap[i].original *= f;
		}
	for(i = 0; i < vecs; ++i) {
		vecMap[i].delta *= f;
		if(vecMap[i].originalStored)
			vecMap[i].original *= f;
		}
	}

#define EPVM_DATA_CHUNK 0x1000

IOResult EPVertMapper::Save(ISave *isave) {
	ULONG nb;
	isave->BeginChunk(EPVM_DATA_CHUNK);
	isave->Write(&verts,sizeof(int),&nb);
	isave->Write(vertMap,sizeof(EPMapVert) * verts, &nb);
	isave->Write(&vecs,sizeof(int),&nb);
	isave->Write(vecMap,sizeof(EPMapVert) * vecs, &nb);
	isave->EndChunk();
	return IO_OK;
	}

IOResult EPVertMapper::Load(ILoad *iload) {
	IOResult res;
	ULONG nb;
	int index = 0;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case EPVM_DATA_CHUNK:
				res = iload->Read(&verts,sizeof(int),&nb);
				if(vertMap)
					delete [] vertMap;
				vertMap = new EPMapVert[verts];
				res = iload->Read(vertMap,sizeof(EPMapVert) * verts,&nb);
				res = iload->Read(&vecs,sizeof(int),&nb);
				if(vecMap)
					delete [] vecMap;
				vecMap = new EPMapVert[vecs];
				res = iload->Read(vecMap,sizeof(EPMapVert) * vecs,&nb);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/

EditPatchData::EditPatchData(EditPatchMod *mod)
	{
	meshSteps = mod->meshSteps;
//3-18-99 to suport render steps and removal of the mental tesselator
	meshStepsRender = mod->meshStepsRender;
	showInterior = mod->showInterior;
	

//	meshAdaptive = mod->meshAdaptive;	// Future use (Not used now)
	viewTess = mod->viewTess;
	prodTess = mod->prodTess;
	dispTess = mod->dispTess;
	mViewTessNormals = mod->mViewTessNormals;
	mProdTessNormals = mod->mProdTessNormals;
	mViewTessWeld = mod->mViewTessWeld;
	mProdTessWeld = mod->mProdTessWeld;
	displayLattice = mod->displayLattice;
	displaySurface = mod->displaySurface;
	flags = 0;
	tempData = NULL;
	}

EditPatchData::EditPatchData(EditPatchData& emc)
	{
	meshSteps = emc.meshSteps;
//3-18-99 to suport render steps and removal of the mental tesselator
	meshStepsRender = emc.meshStepsRender;
	showInterior = emc.showInterior;

//	meshAdaptive = emc.meshAdaptive;	// Future use (Not used now)
	viewTess = emc.viewTess;
	prodTess = emc.prodTess;
	dispTess = emc.dispTess;
	mViewTessNormals = emc.mViewTessNormals;
	mProdTessNormals = emc.mProdTessNormals;
	mViewTessWeld = emc.mViewTessWeld;
	mProdTessWeld = emc.mProdTessWeld;
	displayLattice = emc.displayLattice;
	displaySurface = emc.displaySurface;
	flags = emc.flags;
	tempData = NULL;
	vertMap = emc.vertMap;
	finalPatch = emc.finalPatch;
	}

void EditPatchData::Apply(TimeValue t,PatchObject *patchOb,int selLevel)
	{
	// Either just copy it from the existing cache or rebuild from previous level!
	if ( !GetFlag(EPD_UPDATING_CACHE) && tempData 
	      && tempData->PatchCached(t) ) {
		patchOb->patch.DeepCopy( 
			tempData->GetPatch(t),
			PART_GEOM|SELECT_CHANNEL|PART_SUBSEL_TYPE|
			PART_DISPLAY|PART_TOPO|TEXMAP_CHANNEL );		
		patchOb->PointsWereChanged();
		}	
	else if ( GetFlag(EPD_HASDATA) ) {
		// For old files, which contain exhaustive data to reconstruct the editing process
		// of patches, we'll have data in the 'changes' table.  If it's there, go ahead and
		// replay the edits, then store the alterations in our new delta format and discard
		// the change table!
		int count = changes.Count();
		if(count) {
//DebugPrint("*** Applying old style (%d) ***\n", count);
			// Store the topology for future reference
			vertMap.Build(patchOb->patch);
			finalPatch = patchOb->patch;
			for(int i = 0; i < count; ++i) {
				PModRecord *rec = changes[i];
				// Record the topo flags
				RecordTopologyTags(&patchOb->patch);
				BOOL result = rec->Redo(&patchOb->patch,0);
				UpdateChanges(&patchOb->patch);
				// If we hit one that didn't play back OK, we need to flush the remainder
				if(!result) {
					for(int j = i; j < count; ++j)
						delete changes[j];
					changes.Delete(i, count - i);
					break;
					}
				}
			// Nuke the changes table
			count = changes.Count();
			for(int k = 0; k < count; ++k)
				delete changes[k];
			changes.Delete(0, count);
			changes.Shrink();
			count = 0;
			}
		else {
			// Apply deltas to incoming shape, placing into finalPatch
			vertMap.UpdateAndApplyDeltas(patchOb->patch, finalPatch);
			patchOb->patch = finalPatch;
			}
		patchOb->PointsWereChanged();
		// Kind of a waste when there's no animation...		
		patchOb->UpdateValidity(GEOM_CHAN_NUM,FOREVER);
		patchOb->UpdateValidity(TOPO_CHAN_NUM,FOREVER);
		patchOb->UpdateValidity(SELECT_CHAN_NUM,FOREVER);
		patchOb->UpdateValidity(SUBSEL_TYPE_CHAN_NUM,FOREVER);
		patchOb->UpdateValidity(DISP_ATTRIB_CHAN_NUM,FOREVER);		
		}
	else {	// No data yet -- Store initial required data
//DebugPrint("<<<Storing Initial Data>>>\n");
		vertMap.Build(patchOb->patch);
		finalPatch = patchOb->patch;
		}

	// Hand it its mesh interpolation info
	patchOb->SetMeshSteps(meshSteps);
//3-18-99 to suport render steps and removal of the mental tesselator
	patchOb->SetMeshStepsRender(meshStepsRender);
	patchOb->SetShowInterior(showInterior);

//	patchOb->SetAdaptive(meshAdaptive);	// Future use (Not used now)
	patchOb->SetViewTess(viewTess);
	patchOb->SetProdTess(prodTess);
	patchOb->SetDispTess(dispTess);
	patchOb->SetViewTessNormals(mViewTessNormals);
	patchOb->SetProdTessNormals(mProdTessNormals);
	patchOb->SetViewTessWeld(mViewTessWeld);
	patchOb->SetProdTessWeld(mProdTessWeld);

	patchOb->showMesh = displaySurface;
	patchOb->SetShowLattice(displayLattice);
	patchOb->patch.dispFlags = 0;	// TH 3/3/99
	switch ( selLevel ) {
		case EP_PATCH:
			patchOb->patch.SetDispFlag(DISP_SELPATCHES);
			break;
		case EP_EDGE:
			patchOb->patch.SetDispFlag(DISP_SELEDGES);
			break;
		case EP_VERTEX:
			patchOb->patch.SetDispFlag(DISP_VERTTICKS|DISP_SELVERTS|DISP_VERTS);
			break;
		}
	patchOb->patch.selLevel = patchLevel[selLevel];
	
	if ( GetFlag(EPD_UPDATING_CACHE) ) {
		assert(tempData);
		tempData->UpdateCache(patchOb);
		SetFlag(EPD_UPDATING_CACHE,FALSE);
		}		
	}

void EditPatchData::Invalidate(PartID part,BOOL patchValid)
	{
	if ( tempData ) {
		tempData->Invalidate(part,patchValid);
		}
	}

void EditPatchData::BeginEdit(TimeValue t)
	{
	assert(tempData);
	if ( !GetFlag(EPD_HASDATA) )
		SetFlag(EPD_HASDATA,TRUE);
	}

EPTempData *EditPatchData::TempData(EditPatchMod *mod)
	{
	if ( !tempData ) {
		assert(mod->ip);
		tempData = new EPTempData(mod,this);
		}
	return tempData;
	}

void EditPatchData::RescaleWorldUnits(float f) {
	// Scale the deltas inside the vertex map
	vertMap.RescaleWorldUnits(f);
	// Now rescale stuff inside our data structures
	Matrix3 stm = ScaleMatrix(Point3(f, f, f));
	finalPatch.Transform(stm);
	}

void EditPatchData::RecordTopologyTags(PatchMesh *patch) {
	// First, stuff all -1's into aux fields
	for(int i = 0; i < patch->numVerts; ++i)
		patch->verts[i].aux1 = 0xffffffff;
	for(i = 0; i < patch->numVecs; ++i)
		patch->vecs[i].aux1 = 0xffffffff;
	// Now put in our tags
	vertMap.RecordTopologyTags(*patch);
	}

GenericNamedSelSetList &EditPatchData::GetSelSet(EditPatchMod *mod) {
	switch(mod->GetSubobjectLevel()) {
		case EP_VERTEX:
			return vselSet;
		case EP_EDGE:
			return eselSet;
		case EP_PATCH:
		default:
			return pselSet;
		}
	}

GenericNamedSelSetList &EditPatchData::GetSelSet(int level) {
	switch(level+EP_VERTEX) {
		case EP_VERTEX:
			return vselSet;
		case EP_EDGE:
			return eselSet;
		case EP_PATCH:
		default:
			return pselSet;
		}
	}

// --------------------------------------------------------------------------------------

class EPVertMapRestore : public RestoreObj {
public:
	BOOL gotRedo;
	EPVertMapper undo;
	EPVertMapper redo;
	EditPatchData *epd;
	
	EPVertMapRestore(EditPatchData *d) {
		undo = d->vertMap;
		epd = d;
		gotRedo = FALSE;
		}

	void Restore(int isUndo) {
		if(!gotRedo) {
			gotRedo = TRUE;
			redo = epd->vertMap;
			}
		epd->vertMap = undo;
		}

	void Redo() {
		epd->vertMap = redo;
		}

	int Size() { return 1; }
	void EndHold() { }
	TSTR Description() { return TSTR(_T("EPVertMapRestore")); }
};

// --------------------------------------------------------------------------------------

class FinalPatchRestore : public RestoreObj {
public:
	BOOL gotRedo;
	PatchMesh undo;
	PatchMesh redo;
	PatchMesh *patch;
	
	FinalPatchRestore(PatchMesh *s) {
		undo = *s;
		patch = s;
		gotRedo = FALSE;
		}

	void Restore(int isUndo) {
		if(!gotRedo) {
			gotRedo = TRUE;
			redo = *patch;
			}
		*patch = undo;
		}

	void Redo() {
		*patch = redo;
		}

	int Size() { return 1; }
	void EndHold() { }
	TSTR Description() { return TSTR(_T("FinalPatchRestore")); }
};

// --------------------------------------------------------------------------------------

void EditPatchData::UpdateChanges(PatchMesh *patch, BOOL checkTopology) {
	if(theHold.Holding()) {
		theHold.Put(new EPVertMapRestore(this));
		theHold.Put(new FinalPatchRestore(&finalPatch));
		}
	// Update the mapper's indices
	if(checkTopology)
		vertMap.UpdateMapping(*patch);
	// Update mapper's XYZ deltas
	vertMap.RecomputeDeltas(*patch);
	// Store the final shape
	finalPatch = *patch;
	}

#define EPD_GENERAL_CHUNK		0x1000	// Obsolete as of 11/12/98 (r3)
#define CHANGE_CHUNK			0x1010 	// Obsolete as of 11/12/98 (r3)
#define EPD_R3_GENERAL_CHUNK	0x1015
#define MESH_ATTRIB_CHUNK		0x1020
#define DISP_PARTS_CHUNK		0x1030
#define VTESS_ATTRIB_CHUNK		0x1070
#define PTESS_ATTRIB_CHUNK		0x1080
#define DTESS_ATTRIB_CHUNK		0x1090
#define NORMAL_TESS_ATTRIB_CHUNK	0x1110
#define WELD_TESS_ATTRIB_CHUNK	0x1120
#define VERTMAP_CHUNK			0x1130
#define FINALPATCH_CHUNK		0x1140
#define RENDERSTEPS_CHUNK		0x1150
#define SHOWINTERIOR_CHUNK		0x1160

// Named sel set chunks
#define VSELSET_CHUNK		0x1040
#define ESELSET_CHUNK		0x1050
#define PSELSET_CHUNK		0x1060


IOResult EditPatchData::Save(ISave *isave) {
	ULONG nb;
	isave->BeginChunk(EPD_R3_GENERAL_CHUNK);
	isave->Write(&flags,sizeof(DWORD),&nb);
	isave->EndChunk();
	isave->BeginChunk(MESH_ATTRIB_CHUNK);
	isave->Write(&meshSteps,sizeof(int),&nb);
// Future use (Not used now)
	BOOL fakeAdaptive = FALSE;	
	isave->Write(&fakeAdaptive,sizeof(BOOL),&nb);
//	isave->Write(&meshAdaptive,sizeof(BOOL),&nb);	// Future use (Not used now)
	isave->EndChunk();

//3-18-99 to suport render steps and removal of the mental tesselator
	isave->BeginChunk(RENDERSTEPS_CHUNK);
	if ( (meshStepsRender < 0) || (meshStepsRender > 100))
		{
		meshStepsRender = 5;
		DbgAssert(0);
		}
	isave->Write(&meshStepsRender,sizeof(int),&nb);
	isave->EndChunk();
	isave->BeginChunk(SHOWINTERIOR_CHUNK);
	isave->Write(&showInterior,sizeof(BOOL),&nb);
	isave->EndChunk();


	isave->BeginChunk(VTESS_ATTRIB_CHUNK);
	viewTess.Save(isave);
	isave->EndChunk();
	isave->BeginChunk(PTESS_ATTRIB_CHUNK);
	prodTess.Save(isave);
	isave->EndChunk();
	isave->BeginChunk(DTESS_ATTRIB_CHUNK);
	dispTess.Save(isave);
	isave->EndChunk();
	isave->BeginChunk(DISP_PARTS_CHUNK);
	isave->Write(&displaySurface,sizeof(BOOL),&nb);
	isave->Write(&displayLattice,sizeof(BOOL),&nb);
	isave->EndChunk();

	isave->BeginChunk(NORMAL_TESS_ATTRIB_CHUNK);
	isave->Write(&mViewTessNormals,sizeof(BOOL),&nb);
	isave->Write(&mProdTessNormals,sizeof(BOOL),&nb);
	isave->EndChunk();

	isave->BeginChunk(WELD_TESS_ATTRIB_CHUNK);
	isave->Write(&mViewTessWeld,sizeof(BOOL),&nb);
	isave->Write(&mProdTessWeld,sizeof(BOOL),&nb);
	isave->EndChunk();

	
	// Save named sel sets
	if (vselSet.Count()) {
		isave->BeginChunk(VSELSET_CHUNK);
		vselSet.Save(isave);
		isave->EndChunk();
		}
	if (eselSet.Count()) {
		isave->BeginChunk(ESELSET_CHUNK);
		eselSet.Save(isave);
		isave->EndChunk();
		}
	if (pselSet.Count()) {
		isave->BeginChunk(PSELSET_CHUNK);
		pselSet.Save(isave);
		isave->EndChunk();
		}

	isave->BeginChunk(VERTMAP_CHUNK);
	vertMap.Save(isave);
	isave->EndChunk();
	isave->BeginChunk(FINALPATCH_CHUNK);
	finalPatch.Save(isave);
	isave->EndChunk();

	return IO_OK;
	}

IOResult EditPatchData::Load(ILoad *iload) {
	IOResult res;
	ULONG nb;
	PModRecord *theChange;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			// The following code is here to load pre-release 3 files.
			case EPD_GENERAL_CHUNK:
				iload->SetObsolete();
				iload->Read(&flags,sizeof(DWORD),&nb);
				break;
			case CLEARVERTSELRECORD_CHUNK:
				theChange = new ClearPVertSelRecord;
				goto load_change;
			case SETVERTSELRECORD_CHUNK:
				theChange = new SetPVertSelRecord;
				goto load_change;
			case INVERTVERTSELRECORD_CHUNK:
				theChange = new InvertPVertSelRecord;
				goto load_change;
			case CLEAREDGESELRECORD_CHUNK:
				theChange = new ClearPEdgeSelRecord;
				goto load_change;
			case SETEDGESELRECORD_CHUNK:
				theChange = new SetPEdgeSelRecord;
				goto load_change;
			case INVERTEDGESELRECORD_CHUNK:
				theChange = new InvertPEdgeSelRecord;
				goto load_change;
			case CLEARPATCHSELRECORD_CHUNK:
				theChange = new ClearPatchSelRecord;
				goto load_change;
			case SETPATCHSELRECORD_CHUNK:
				theChange = new SetPatchSelRecord;
				goto load_change;
			case INVERTPATCHSELRECORD_CHUNK:
				theChange = new InvertPatchSelRecord;
				goto load_change;
			case VERTSELRECORD_CHUNK:
				theChange = new PVertSelRecord;
				goto load_change;
			case EDGESELRECORD_CHUNK:
				theChange = new PEdgeSelRecord;
				goto load_change;
			case PATCHSELRECORD_CHUNK:
				theChange = new PatchSelRecord;
				goto load_change;
			case PATCHDELETERECORD_CHUNK:
				theChange = new PatchDeleteRecord;
				goto load_change;
			case VERTMOVERECORD_CHUNK:
				theChange = new PVertMoveRecord;
				goto load_change;
			case PATCHCHANGERECORD_CHUNK:
				theChange = new PatchChangeRecord;
				goto load_change;
			case VERTCHANGERECORD_CHUNK:
				theChange = new PVertChangeRecord;
				goto load_change;
			case PATCHADDRECORD_CHUNK:
				theChange = new PatchAddRecord;
				goto load_change;
			case EDGESUBDIVIDERECORD_CHUNK:
				theChange = new EdgeSubdivideRecord;
				goto load_change;
			case PATCHSUBDIVIDERECORD_CHUNK:
				theChange = new PatchSubdivideRecord;
				goto load_change;
			case PATTACHRECORD_CHUNK:
				theChange = new PAttachRecord;
				goto load_change;
			case PATCHDETACHRECORD_CHUNK:
				theChange = new PatchDetachRecord;
				goto load_change;
			case PATCHMTLRECORD_CHUNK:
				theChange = new PatchMtlRecord;
				goto load_change;
			case VERTWELDRECORD_CHUNK:
				theChange = new PVertWeldRecord;
				goto load_change;
			case VERTDELETERECORD_CHUNK:
				theChange = new PVertDeleteRecord;
				// Intentional fall-thru!
				load_change:
				changes.Append(1,&theChange);
				changes[changes.Count()-1]->Load(iload);
				break;
			//
			// The following code is used for post-release 3 files
			//
			case EPD_R3_GENERAL_CHUNK:
				res = iload->Read(&flags,sizeof(DWORD),&nb);
				break;
			case VERTMAP_CHUNK:
				res = vertMap.Load(iload);
				break;
			case FINALPATCH_CHUNK:
				res = finalPatch.Load(iload);
				break;
			//
			// The following code is common to all versions' files
			//
			case MESH_ATTRIB_CHUNK:
				iload->Read(&meshSteps,sizeof(int),&nb);
				res = iload->Read(&meshAdaptive,sizeof(BOOL),&nb);	// Future use (Not used now)
				break;
//3-18-99 to suport render steps and removal of the mental tesselator
			case RENDERSTEPS_CHUNK:
				iload->Read(&meshStepsRender,sizeof(int),&nb);
				if ( (meshStepsRender < 0) || (meshStepsRender > 100))
					{
					meshStepsRender = 5;
					DbgAssert(0);
					}

				break;
			case SHOWINTERIOR_CHUNK:
				iload->Read(&showInterior,sizeof(BOOL),&nb);
				break;

			case VTESS_ATTRIB_CHUNK:
				viewTess.Load(iload);
				break;
			case PTESS_ATTRIB_CHUNK:
				prodTess.Load(iload);
				break;
			case DTESS_ATTRIB_CHUNK:
				dispTess.Load(iload);
				break;
			case NORMAL_TESS_ATTRIB_CHUNK:
				iload->Read(&mViewTessNormals,sizeof(BOOL),&nb);
				res = iload->Read(&mProdTessNormals,sizeof(BOOL),&nb);
				break;
			case WELD_TESS_ATTRIB_CHUNK:
				iload->Read(&mViewTessWeld,sizeof(BOOL),&nb);
				res = iload->Read(&mProdTessWeld,sizeof(BOOL),&nb);
				break;
			case DISP_PARTS_CHUNK:
				iload->Read(&displaySurface,sizeof(BOOL),&nb);
				res = iload->Read(&displayLattice,sizeof(BOOL),&nb);
				break;
			// Load named selection sets
			case VSELSET_CHUNK:
				res = vselSet.Load(iload);
				break;
			case PSELSET_CHUNK:
				res = pselSet.Load(iload);
				break;
			case ESELSET_CHUNK:
				res = eselSet.Load(iload);
				break;

			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL ClearPVertSelRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord)
		sel = patch->vertSel;
	patch->vertSel.ClearAll();
	return TRUE;
	}

#define CVSR_SEL_CHUNK 0x1000

IOResult ClearPVertSelRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case CVSR_SEL_CHUNK:
				res = sel.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL SetPVertSelRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord)
		sel = patch->vertSel;
	patch->vertSel.SetAll();
	return TRUE;
	}

#define SVSR_SEL_CHUNK 0x1000

IOResult SetPVertSelRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case SVSR_SEL_CHUNK:
				res = sel.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL InvertPVertSelRecord::Redo(PatchMesh *patch,int reRecord) {
	patch->vertSel = ~patch->vertSel;
	return TRUE;
	}

IOResult InvertPVertSelRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
//		switch(iload->CurChunkID())  {
//			default:
//				break;
//			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL ClearPEdgeSelRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord)
		sel = patch->edgeSel;
	patch->edgeSel.ClearAll();
	return TRUE;
	}

#define CESR_SEL_CHUNK 0x1000

IOResult ClearPEdgeSelRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case CESR_SEL_CHUNK:
				res = sel.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL SetPEdgeSelRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord)
		sel = patch->edgeSel;
	patch->edgeSel.SetAll();
	return TRUE;
	}

#define SESR_SEL_CHUNK 0x1000

IOResult SetPEdgeSelRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case SESR_SEL_CHUNK:
				res = sel.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL InvertPEdgeSelRecord::Redo(PatchMesh *patch,int reRecord) {
	patch->edgeSel = ~patch->edgeSel;
	return TRUE;
	}

IOResult InvertPEdgeSelRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
//		switch(iload->CurChunkID())  {
//			default:
//				break;
//			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL ClearPatchSelRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord)
		sel = patch->patchSel;
	patch->patchSel.ClearAll();
	return TRUE;
	}

#define CPSR_SEL_CHUNK 0x1000

IOResult ClearPatchSelRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case CPSR_SEL_CHUNK:
				res = sel.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL SetPatchSelRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord)
		sel = patch->patchSel;
	patch->patchSel.SetAll();
	return TRUE;
	}

#define SPSR_SEL_CHUNK 0x1000

IOResult SetPatchSelRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case SPSR_SEL_CHUNK:
				res = sel.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL InvertPatchSelRecord::Redo(PatchMesh *patch,int reRecord) {
	patch->patchSel = ~patch->patchSel;
	return TRUE;
	}

IOResult InvertPatchSelRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
//		switch(iload->CurChunkID())  {
//			default:
//				break;
//			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL PVertSelRecord::Redo(PatchMesh *patch,int reRecord) {
	if(!IsCompatible(patch->vertSel, newSel))
		return FALSE;
	patch->vertSel = newSel;
	return TRUE;
	}

#define VSR_OLDSEL_CHUNK 0x1000
#define VSR_NEWSEL_CHUNK 0x1010

IOResult PVertSelRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case VSR_OLDSEL_CHUNK:
				res = oldSel.Load(iload);
				break;
			case VSR_NEWSEL_CHUNK:
				res = newSel.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL PEdgeSelRecord::Redo(PatchMesh *patch,int reRecord) {
	if(!IsCompatible(patch->edgeSel, newSel))
		return FALSE;
	patch->edgeSel = newSel;
	return TRUE;
	}

#define ESR_OLDSEL_CHUNK 0x1000
#define ESR_NEWSEL_CHUNK 0x1010

IOResult PEdgeSelRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case ESR_OLDSEL_CHUNK:
				res = oldSel.Load(iload);
				break;
			case ESR_NEWSEL_CHUNK:
				res = newSel.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL PatchSelRecord::Redo(PatchMesh *patch,int reRecord) {
	if(!IsCompatible(patch->patchSel, newSel))
		return FALSE;
	patch->patchSel = newSel;
	return TRUE;
	}

#define PSR_OLDSEL_CHUNK 0x1000
#define PSR_NEWSEL_CHUNK 0x1010

IOResult PatchSelRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case PSR_OLDSEL_CHUNK:
				res = oldSel.Load(iload);
				break;
			case PSR_NEWSEL_CHUNK:
				res = newSel.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

static void DeleteSelPatches(PatchMesh *patch) {
	if(!patch->patchSel.NumberSet())
		return;		// Nothing to do!

	int patches = patch->getNumPatches();
	int verts = patch->getNumVerts();

	// Tag the patches that are selected
	BitArray delPatches(patches);
	delPatches = patch->patchSel;

	BitArray delVerts(verts);
	delVerts.ClearAll();

	DeletePatchParts(patch, delVerts, delPatches);
	patch->computeInteriors();
	}

BOOL PatchDeleteRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord)
		oldPatch = *patch;
	DeleteSelPatches(patch);
	return TRUE;
	}

#define PDELR_PATCH_CHUNK		0x1060

IOResult PatchDeleteRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
//		switch(iload->CurChunkID())  {
//			case PDELR_PATCH_CHUNK:
//				res = oldPatch.Load(iload);
//				break;
//			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL PVertMoveRecord::Redo(PatchMesh *patch,int reRecord) {
	if(!delta.IsCompatible(*patch))
		return FALSE;
	delta.Apply(*patch);
	return TRUE;
	}

#define VMR_DELTA_CHUNK		0x1000

IOResult PVertMoveRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case VMR_DELTA_CHUNK:
				res = delta.Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

static void DeleteSelVerts(PatchMesh *patch) {
	if(!patch->vertSel.NumberSet())
		return;		// Nothing to do!

	int patches = patch->getNumPatches();
	int verts = patch->getNumVerts();

	// Tag the patches that use selected vertices
	BitArray delPatches(patches);
	delPatches.ClearAll();
	for(int i = 0; i < patches; ++i) {
		Patch& p = patch->patches[i];
		for(int j = 0; j < p.type; ++j) {
			if(patch->vertSel[p.v[j]]) {
				delPatches.Set(i);
				goto next_patch;
				}
			}
		next_patch:;
		}

	BitArray delVerts(verts);
	delVerts = patch->vertSel;
	DeletePatchParts(patch, delVerts, delPatches);
	patch->computeInteriors();
	}

BOOL PVertDeleteRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord)
		oldPatch = *patch;
	DeleteSelVerts(patch);
	return TRUE;
	}

#define VDELR_PATCH_CHUNK		0x1060

IOResult PVertDeleteRecord::Load(ILoad *iload) {
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
//		switch(iload->CurChunkID())  {
//			case VDELR_PATCH_CHUNK:
//				res = oldPatch.Load(iload);
//				break;
//			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL PVertChangeRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord)
		oldPatch = *patch;
	patch->ChangeVertType(index, type);
	return TRUE;
	}

#define VCHG_GENERAL_CHUNK		0x1001
#define VCHG_PATCH_CHUNK		0x1010

IOResult PVertChangeRecord::Load(ILoad *iload) {
	IOResult res;
	ULONG nb;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case VCHG_GENERAL_CHUNK:
				res = iload->Read(&index,sizeof(int),&nb);
				res = iload->Read(&type,sizeof(int),&nb);
				break;
//			case VCHG_PATCH_CHUNK:
//				res = oldPatch.Load(iload);
//				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL PAttachRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord)
		oldPatchCount = patch->numPatches;
	patch->Attach(&attPatch, mtlOffset);
	return TRUE;
	}

#define ATTR_GENERAL_CHUNK		0x1001
#define ATTR_ATTPATCH_CHUNK		0x1010
#define ATTR_MTLOFFSET_CHUNK	0x1020

IOResult PAttachRecord::Load(ILoad *iload) {
	IOResult res;
	ULONG nb;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case ATTR_GENERAL_CHUNK:
				res = iload->Read(&oldPatchCount,sizeof(int),&nb);
				break;
			case ATTR_ATTPATCH_CHUNK:
				res = attPatch.Load(iload);
				break;
			case ATTR_MTLOFFSET_CHUNK:
				res = iload->Read(&mtlOffset,sizeof(int),&nb);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL PatchDetachRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord && !copy)
		oldPatch = *patch;
	if(!copy) {
		BitArray vdel(patch->numVerts);
		vdel.ClearAll();
		BitArray pdel = patch->patchSel;
		DeletePatchParts(patch, vdel, pdel);
		}
	return TRUE;
	}

#define PDETR_GENERAL_CHUNK		0x1000
#define PDETR_PATCH_CHUNK		0x1030

IOResult PatchDetachRecord::Load(ILoad *iload) {
	IOResult res;
	ULONG nb;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case PDETR_GENERAL_CHUNK:
				res = iload->Read(&copy,sizeof(int),&nb);
				break;
//			case PDETR_PATCH_CHUNK:
//				res = oldPatch.Load(iload);
//				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL PatchMtlRecord::Redo(PatchMesh *patch,int reRecord) {
	for(int i = 0; i < patch->numPatches; ++i) {
		if(patch->patchSel[i])
			patch->patches[i].setMatID(index);
		}
	return TRUE;
	}

#define PMTLR_GENERAL_CHUNK		0x1000
#define PMTLR_INDEX_CHUNK		0x1020

IOResult PatchMtlRecord::Load(ILoad *iload) {
	IOResult res;
	ULONG nb;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case PMTLR_INDEX_CHUNK:
				res = iload->Read(&index,sizeof(MtlID),&nb);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

static void ChangePatchType(PatchMesh *patch, int index, int type) {
	// If positive vertex number, do it to just one vertex
	if(index >= 0) {
		patch->patches[index].flags = type;
		patch->computeInteriors();
		return;
		}

	// Otherwise, do it to all selected vertices!
	int patches = patch->numPatches;
	BitArray &psel = patch->patchSel;
	for(int i = 0; i < patches; ++i) {
		if(psel[i])
			patch->patches[i].flags = type;
		}
	patch->computeInteriors();
	}

//watje
static void FixUpVerts(PatchMesh *patch) {
	int patches = patch->numPatches;
	for(int i = 0; i < patches; i++) {

		if (!(patch->patches[i].IsHidden()))
			{
			int ct = 4;
			if (patch->patches[i].type==PATCH_TRI)
				ct = 3;
			for (int k = 0; k < ct; k++)
				{
				int a = patch->patches[i].v[k];
				patch->verts[a].SetHidden(FALSE);
				}

			}
		}

	}

//watje unhide all
static void UnHidePatches(PatchMesh *patch) {
	// If positive vertex number, do it to just one vertex
	int patches = patch->numPatches;
	for(int i = 0; i < patches; i++) {
		if (patch->patches[i].IsHidden())
			patch->patches[i].SetHidden(FALSE);
		}
	int verts = patch->numVerts;
	for(i = 0; i < verts; i++) {
		if (patch->verts[i].IsHidden())
			patch->verts[i].SetHidden(FALSE);
		}
	}

//watje hide patch
static void HidePatches(PatchMesh *patch) {
	// If positive vertex number, do it to just one vertex
	int patches = patch->numPatches;
	BitArray &psel = patch->patchSel;
	for(int i = 0; i < patches; i++) {
		if(psel[i])
			{
			patch->patches[i].SetHidden(TRUE);
//hide all 
			int ct = 4;
			if (patch->patches[i].type==PATCH_TRI)
				ct = 3;
			for (int k = 0; k < ct; k++)
				{
				int a = patch->patches[i].v[k];
				patch->verts[a].SetHidden(TRUE);
				}
			}
		}
	FixUpVerts(patch);
	}
//watje hide patches by verts
static void HideVerts(PatchMesh *patch) {
	// If positive vertex number, do it to just one vertex
	int patches = patch->numPatches;
	BitArray &vsel = patch->vertSel;
	for(int i = 0; i < patches; i++) {
		int ct = 4;
		if (patch->patches[i].type==PATCH_TRI)
			ct = 3;
		for (int k = 0; k < ct; k++)
			{
			int a = patch->patches[i].v[k];

			if(vsel[a])
				{
				patch->patches[i].SetHidden(TRUE);
				}
			}
		}
	for(i = 0; i < patches; i++) {
		if(patch->patches[i].IsHidden())
			{
//hide all 
			int ct = 4;
			if (patch->patches[i].type==PATCH_TRI)
				ct = 3;
			for (int k = 0; k < ct; k++)
				{
				int a = patch->patches[i].v[k];
				patch->verts[a].SetHidden(TRUE);
				}
			}
		}

	FixUpVerts(patch);
	}
//watje hide patches by verts
static void HideEdges(PatchMesh *patch) {
	// If positive vertex number, do it to just one vertex
	int edges = patch->numEdges;
	BitArray &esel = patch->edgeSel;
	for(int i = 0; i < edges; i++) {
		if (esel[i])
			{
			int a = patch->edges[i].patch1;
			int b = patch->edges[i].patch2;
			if (a>0)
				patch->patches[a].SetHidden(TRUE);
			if (b>0)
				patch->patches[b].SetHidden(TRUE);
			}
		}
	int patches = patch->numPatches;
	for(i = 0; i < patches; i++) {
		if(patch->patches[i].IsHidden())
			{
//hide all 
			int ct = 4;
			if (patch->patches[i].type==PATCH_TRI)
				ct = 3;
			for (int k = 0; k < ct; k++)
				{
				int a = patch->patches[i].v[k];
				patch->verts[a].SetHidden(TRUE);
				}
			}
		}
	FixUpVerts(patch);
	}



BOOL PatchChangeRecord::Redo(PatchMesh *patch,int reRecord) {
	if(index >= 0 && index >= patch->numPatches)
		return FALSE;
	if(reRecord)
		oldPatch = *patch;
	ChangePatchType(patch, index, type);
	return TRUE;
	}

#define PCHG_GENERAL_CHUNK		0x1001
#define PCHG_PATCH_CHUNK		0x1010

IOResult PatchChangeRecord::Load(ILoad *iload) {
	IOResult res;
	ULONG nb;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case PCHG_GENERAL_CHUNK:
				res = iload->Read(&index,sizeof(int),&nb);
				res = iload->Read(&type,sizeof(int),&nb);
				break;
//			case PCHG_PATCH_CHUNK:
//				res = oldPatch.Load(iload);
//				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

static void AddPatches(int type, PatchMesh *patch, BOOL postWeld) {
	if(!patch->edgeSel.NumberSet())
		return;		// Nothing to do!

	int lastVert = patch->getNumVerts();
	int edges = patch->getNumEdges();

	// Add a patch of the desired type to each selected edge that doesn't have two patches atatched!
	for(int i = 0; i < edges; ++i) {
		if(patch->edgeSel[i]) {
			PatchEdge &edge = patch->edges[i];
			if(edge.patch2 < 0) {
				int verts = patch->getNumVerts();
				int vecs = patch->getNumVecs();
				int patches = patch->getNumPatches();
				patch->setNumPatches(patches+1, TRUE);			// Add a patch
				patch->patches[patches].SetType(type);			// Make it the type we want
				patch->setNumVerts(verts + type - 2, TRUE);		// Add the appropriate number of verts
				patch->setNumVecs(vecs + (type-1) * 2 + type, TRUE);	// And the appropriate vector count
				Point3 p1 = patch->verts[edge.v1].p;
				Point3 p2 = patch->verts[edge.v2].p;
				Point3 v12 = patch->vecs[edge.vec12].p;
				Point3 v21 = patch->vecs[edge.vec21].p;
				Point3 edgeCenter = (p1 + p2) / 2.0f;
				// Load up the patch with the correct vert/vector indices
				Patch &spatch = patch->patches[edge.patch1];
				Patch &dpatch = patch->patches[patches];
				switch(type) {
					case PATCH_TRI:
						dpatch.setVerts(edge.v2, edge.v1, verts);
						dpatch.setVecs(edge.vec21, edge.vec12, vecs, vecs+1, vecs+2, vecs+3);
						dpatch.setInteriors(vecs+4, vecs+5, vecs+6);
						switch(spatch.type) {
							case PATCH_TRI: {		// Tri from Tri
								// Find the opposite vertex in the source triangle
								int opposite, o2a, o1a;
								if(spatch.edge[0] == i) {
									opposite = 2;
									o1a = 5;
									o2a = 2;
									}
								else
								if(spatch.edge[1] == i) {
									opposite = 0;
									o1a = 1;
									o2a = 4;
									}
								else {
									opposite = 1;
									o1a = 3;
									o2a = 0;
									}
								// Compute the new vert position
								Point3 oppVec = edgeCenter - patch->verts[spatch.v[opposite]].p;
								float oppLen = Length(oppVec);
								if(oppLen == 0.0f) {
									oppVec = Point3(0,0,1);
									oppLen = 1.0f;
									}
								Point3 v1a = patch->vecs[spatch.vec[o1a]].p - p1;
								Point3 v2a = patch->vecs[spatch.vec[o2a]].p - p2;
								Point3 n1a, n2a;
								if(Length(v1a) > 0.0f)
									n1a = Normalize(v1a);
								else
									n1a = Normalize(oppVec);
								if(Length(v2a) > 0.0f)
									n2a = Normalize(v2a);
								else
									n2a = Normalize(oppVec);
								
								// Build a composite vector based on the two edge vectors
								Point3 compVec = Normalize((n1a + n2a) / 2.0f);
								
								// Create the new vertex
								Point3 newPos = edgeCenter - compVec * oppLen;
								patch->verts[verts].p = newPos;

								// Compute the vectors
								patch->vecs[vecs].p = p1 - v1a;
								patch->vecs[vecs+1].p = newPos - (newPos - p1) / 3.0f;
								patch->vecs[vecs+2].p = newPos - (newPos - p2) / 3.0f;
								patch->vecs[vecs+3].p = p2 - v2a;
								}
								break;
							case PATCH_QUAD: {	// Tri from Quad
								// Find the opposite edge verts in the source quad
								int opposite1, opposite2, o1a, o2a;
								if(spatch.edge[0] == i) {
									opposite1 = 2;
									opposite2 = 3;
									o1a = 7;
									o2a = 2;
									}
								else
								if(spatch.edge[1] == i) {
									opposite1 = 3;
									opposite2 = 0;
									o1a = 1;
									o2a = 4;
									}
								else
								if(spatch.edge[2] == i) {
									opposite1 = 0;
									opposite2 = 1;
									o1a = 3;
									o2a = 6;
									}
								else {
									opposite1 = 1;
									opposite2 = 2;
									o1a = 5;
									o2a = 0;
									}
								// Compute the new vert position
								Point3 otherCenter = (patch->verts[spatch.v[opposite1]].p + patch->verts[spatch.v[opposite2]].p) / 2.0f;
								Point3 oppVec = edgeCenter - otherCenter;
								float oppLen = Length(oppVec);
								if(oppLen == 0.0f) {
									oppVec = Point3(0,0,1);
									oppLen = 1.0f;
									}
								Point3 v1a = patch->vecs[spatch.vec[o1a]].p - p1;
								Point3 v2a = patch->vecs[spatch.vec[o2a]].p - p2;
								Point3 n1a, n2a;
								if(Length(v1a) > 0.0f)
									n1a = Normalize(v1a);
								else
									n1a = Normalize(oppVec);
								if(Length(v2a) > 0.0f)
									n2a = Normalize(v2a);
								else
									n2a = Normalize(oppVec);
								
								// Build a composite vector based on the two edge vectors
								Point3 compVec = Normalize((n1a + n2a) / 2.0f);
								
								// Create the new vertex
								Point3 newPos = edgeCenter - compVec * oppLen;
								patch->verts[verts].p = newPos;

								// Compute the vectors
								patch->vecs[vecs].p = p1 - v1a;
								patch->vecs[vecs+1].p = newPos - (newPos - p1) / 3.0f;
								patch->vecs[vecs+2].p = newPos - (newPos - p2) / 3.0f;
								patch->vecs[vecs+3].p = p2 - v2a;
								}
								break;
							}
						break;
					case PATCH_QUAD:
						dpatch.setVerts(edge.v2, edge.v1, verts, verts+1);
						dpatch.setVecs(edge.vec21, edge.vec12, vecs, vecs+1, vecs+2, vecs+3, vecs+4, vecs+5);
						dpatch.setInteriors(vecs+6, vecs+7, vecs+8, vecs+9);
						switch(spatch.type) {
							case PATCH_TRI: {		// Quad from Tri
								// Find the opposite vertex in the source triangle
								int opposite, o2a, o1a;
								if(spatch.edge[0] == i) {
									opposite = 2;
									o1a = 5;
									o2a = 2;
									}
								else
								if(spatch.edge[1] == i) {
									opposite = 0;
									o1a = 1;
									o2a = 4;
									}
								else {
									opposite = 1;
									o1a = 3;
									o2a = 0;
									}

								Point3 oppVec = edgeCenter - patch->verts[spatch.v[opposite]].p;
								float oppLen = Length(oppVec);
								if(oppLen == 0.0f) {
									oppVec = Point3(0,0,1);
									oppLen = 1.0f;
									}
								Point3 v1a = patch->vecs[spatch.vec[o1a]].p - p1;
								Point3 v2a = patch->vecs[spatch.vec[o2a]].p - p2;
								Point3 n1a, n2a;
								if(Length(v1a) > 0.0f)
									n1a = Normalize(v1a);
								else
									n1a = Normalize(oppVec);
								if(Length(v2a) > 0.0f)
									n2a = Normalize(v2a);
								else
									n2a = Normalize(oppVec);

								// Compute the new vert positions
								Point3 newPos1 = p1 - n1a * oppLen;
								Point3 newPos2 = p2 - n2a * oppLen;
								patch->verts[verts].p = newPos1;
								patch->verts[verts+1].p = newPos2;
								// Compute the vectors
								patch->vecs[vecs].p = p1 - v1a;
								patch->vecs[vecs+1].p = newPos1 - (newPos1 - p1) / 3.0f;
								patch->vecs[vecs+2].p = newPos1 + (v12 - p1);
								patch->vecs[vecs+3].p = newPos2 + (v21 - p2);
								patch->vecs[vecs+4].p = newPos2 + (p2 - newPos2) / 3.0f;
								patch->vecs[vecs+5].p = p2 - v2a;
								}
								break;
							case PATCH_QUAD: {	// Quad from Quad
								// Find the opposite edge verts in the source quad
								int opposite1, opposite2, o1a, o2a;
								if(spatch.edge[0] == i) {
									opposite1 = 2;
									opposite2 = 3;
									o1a = 7;
									o2a = 2;
									}
								else
								if(spatch.edge[1] == i) {
									opposite1 = 3;
									opposite2 = 0;
									o1a = 1;
									o2a = 4;
									}
								else
								if(spatch.edge[2] == i) {
									opposite1 = 0;
									opposite2 = 1;
									o1a = 3;
									o2a = 6;
									}
								else {
									opposite1 = 1;
									opposite2 = 2;
									o1a = 5;
									o2a = 0;
									}

								Point3 otherCenter = (patch->verts[spatch.v[opposite1]].p + patch->verts[spatch.v[opposite2]].p) / 2.0f;
								Point3 oppVec = edgeCenter - otherCenter;
								float oppLen = Length(oppVec);
								if(oppLen == 0.0f) {
									oppVec = Point3(0,0,1);
									oppLen = 1.0f;
									}
								Point3 v1a = patch->vecs[spatch.vec[o1a]].p - p1;
								Point3 v2a = patch->vecs[spatch.vec[o2a]].p - p2;
								Point3 n1a, n2a;
								if(Length(v1a) > 0.0f)
									n1a = Normalize(v1a);
								else
									n1a = Normalize(oppVec);
								if(Length(v2a) > 0.0f)
									n2a = Normalize(v2a);
								else
									n2a = Normalize(oppVec);

								// Compute the new vert position
								Point3 newPos1 = p1 - n1a * oppLen;
								Point3 newPos2 = p2 - n2a * oppLen;
								patch->verts[verts].p = newPos1;
								patch->verts[verts+1].p = newPos2;

								// Compute the vectors
								patch->vecs[vecs].p = p1 - v1a;
								patch->vecs[vecs+1].p = newPos1 - (newPos1 - p1) / 3.0f;
								patch->vecs[vecs+2].p = newPos1 + (v12 - p1);
								patch->vecs[vecs+3].p = newPos2 + (v21 - p2);
								patch->vecs[vecs+4].p = newPos2 + (p2 - newPos2) / 3.0f;
								patch->vecs[vecs+5].p = p2 - v2a;
								}
								break;
							}
						break;
					}
				}
			}
		}
	patch->computeInteriors();
	patch->buildLinkages();
	// This step welds all new identical verts
	if(postWeld && (patch->getNumVerts() != lastVert))
		patch->Weld(0.0f, TRUE, lastVert);
	}

BOOL PatchAddRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord)
		oldPatch = *patch;
	AddPatches(type, patch, postWeld);
	return TRUE;
	}

#define PADDR_TYPE_CHUNK		0x1000
#define PADDR_PATCH_CHUNK		0x1010
#define PADDR_POSTWELD_CHUNK	0x1020

IOResult PatchAddRecord::Load(ILoad *iload) {
	IOResult res;
	ULONG nb;
	postWeld = FALSE;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case PADDR_TYPE_CHUNK:
				res = iload->Read(&type,sizeof(int),&nb);
				break;
//			case PADDR_PATCH_CHUNK:
//				res = oldPatch.Load(iload);
//				break;
			// If the following chunk is present, it's a MAX 2.0 file and a post-addition
			// weld is to be performed
			case PADDR_POSTWELD_CHUNK:
				postWeld = TRUE;
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

class NewEdge {
	public:
		int oldEdge;
		int v1;
		int vec12;
		int vec21;
		int v2;
		int vec23;
		int vec32;
		int v3;
		NewEdge() { oldEdge = v1 = v2 = v3 = vec12 = vec21 = vec23 = vec32 = -1; }
	};

class PatchDivInfo {
	public:
		BOOL div02;
		BOOL div13;
		PatchDivInfo() { div02 = div13 = FALSE; }
	};

// Compute midpoint division for patch vectors -- Provide patchmesh, patch number, 4 bez points
// returns 2 new vectors

static Point3 InterpCenter(PatchMesh *patch, int index, int e1, int i1, int i2, int e2, Point3 *v1=NULL, Point3 *v2=NULL, Point3 *v3=NULL, Point3 *v4=NULL) {
	PatchVec *v = patch->vecs;
	Patch &p = patch->patches[index];
	Point3 e1i1 = (v[p.vec[e1]].p + v[p.interior[i1]].p) / 2.0f;
	Point3 i1i2 = (v[p.interior[i1]].p + v[p.interior[i2]].p) / 2.0f;
	Point3 i2e2 = (v[p.interior[i2]].p + v[p.vec[e2]].p) / 2.0f;
	Point3 a = (e1i1 + i1i2) / 2.0f;
	Point3 b = (i1i2 + i2e2) / 2.0f;
	if(v1) *v1 = e1i1;
	if(v2) *v2 = a;
	if(v3) *v3 = b;
	if(v4) *v4 = i2e2;
	return (a + b) / 2.0f;
	}

static Point3 InterpCenter(PatchMesh *patch, int index, int e1, int i1, int e2, Point3 *v1=NULL, Point3 *v2=NULL) {
	PatchVec *v = patch->vecs;
	Patch &p = patch->patches[index];
	Point3 a = (p.aux[e1] + v[p.interior[i1]].p) / 2.0f;
	Point3 b = (v[p.interior[i1]].p + p.aux[e2]) / 2.0f;
	if(v1) *v1 = a;
	if(v2) *v2 = b;
	return (a + b) / 2.0f;
	}

static Point3 InterpCenter(Point3 e1, Point3 i1, Point3 i2, Point3 e2, Point3 *v1=NULL, Point3 *v2=NULL, Point3 *v3=NULL, Point3 *v4=NULL ) {
	Point3 e1i1 = (e1 + i1) / 2.0f;
	Point3 i1i2 = (i1 + i2) / 2.0f;
	Point3 i2e2 = (i2 + e2) / 2.0f;
	Point3 a = (e1i1 + i1i2) / 2.0f;
	Point3 b = (i1i2 + i2e2) / 2.0f;
	if(v1) *v1 = e1i1;
	if(v2) *v2 = a;
	if(v3) *v3 = b;
	if(v4) *v4 = i2e2;
	return (a + b) / 2.0f;
	}

static Point3 InterpCenter(Point3 e1, Point3 i1, Point3 e2, Point3 *v1=NULL, Point3 *v2=NULL) {
	Point3 a = (e1 + i1) / 2.0f;
	Point3 b = (i1 + e2) / 2.0f;
	if(v1) *v1 = a;
	if(v2) *v2 = b;
	return (a + b) / 2.0f;
	}

static Point3 InterpEdge(PatchMesh *patch, int index, float pct, int c1, int e1, int e2, int c2, Point3 *v1=NULL, Point3 *v2=NULL, Point3 *v3=NULL, Point3 *v4=NULL) {
	PatchVert *vert = patch->verts;
	PatchVec *v = patch->vecs;
	Patch &p = patch->patches[index];
	Point3 pv1 = vert[p.v[c1]].p;
	Point3 pv2 = vert[p.v[c2]].p;
	Point3 pe1 = v[p.vec[e1]].p;
	Point3 pe2 = v[p.vec[e2]].p;
	Point3 v1e1 = pv1 + (pe1 - pv1) * pct;
	Point3 e1e2 = pe1 + (pe2 - pe1) * pct;
	Point3 e2v2 = pe2 + (pv2 - pe2) * pct;
	Point3 a = v1e1 + (e1e2 - v1e1) * pct;
	Point3 b = e1e2 + (e2v2 - e1e2) * pct;
	if(v1) *v1 = v1e1;
	if(v2) *v2 = a;
	if(v3) *v3 = b;
	if(v4) *v4 = e2v2;
	return a + (b - a) * pct;
	}

static Point3 InterpPoint(PatchMesh *patch, int index, float pct, int e1, int i1, int i2, int e2, Point3 *v1=NULL, Point3 *v2=NULL, Point3 *v3=NULL, Point3 *v4=NULL) {
	PatchVec *v = patch->vecs;
	Patch &p = patch->patches[index];
	Point3 pe1 = v[p.vec[e1]].p;
	Point3 pe2 = v[p.vec[e2]].p;
	Point3 pi1 = v[p.interior[i1]].p;
	Point3 pi2 = v[p.interior[i2]].p;
	Point3 e1i1 = pe1 + (pi1 - pe1) * pct;
	Point3 i1i2 = pi1 + (pi2 - pi1) * pct;
	Point3 i2e2 = pi2 + (pe2 - pi2) * pct;
	Point3 a = e1i1 + (i1i2 - e1i1) * pct;
	Point3 b = i1i2 + (i2e2 - i1i2) * pct;
	if(v1) *v1 = e1i1;
	if(v2) *v2 = a;
	if(v3) *v3 = b;
	if(v4) *v4 = i2e2;
	return a + (b - a) * pct;
	}

static Point3 InterpPoint(float pct, Point3 e1, Point3 i1, Point3 i2, Point3 e2, Point3 *v1=NULL, Point3 *v2=NULL, Point3 *v3=NULL, Point3 *v4=NULL ) {
	Point3 e1i1 = e1 + (i1 - e1) * pct;
	Point3 i1i2 = i1 + (i2 - i1) * pct;
	Point3 i2e2 = i2 + (e2 - i2) * pct;
	Point3 a = e1i1 + (i1i2 - e1i1) * pct;
	Point3 b = i1i2 + (i2e2 - i1i2) * pct;
	if(v1) *v1 = e1i1;
	if(v2) *v2 = a;
	if(v3) *v3 = b;
	if(v4) *v4 = i2e2;
	return a + (b - a) * pct;
	}

static Point3 InterpLinear(Point3 a, Point3 b, float interp) {
	return a + (a - b) * interp;
	}

static Point3 InterpDegree2(Point3 a, Point3 b, Point3 c, float interp) {
	Point3 ab = a + (b - a) * interp;
	Point3 bc = b + (c - b) * interp;
	return ab + (bc - ab) * interp;
	}

static Point3 InterpDegree3(Point3 a, Point3 b, Point3 c, Point3 d, float interp) {
	Point3 ab = a + (b - a) * interp;
	Point3 bc = b + (c - b) * interp;
	Point3 cd = c + (d - c) * interp;
	Point3 abbc = ab + (bc - ab) * interp;
	Point3 bccd = bc + (cd - bc) * interp;
	return abbc + (bccd - abbc) * interp;
	}

// Handy fractional constants
#define _1_16 0.0625f
#define _1_8 0.125f
#define _3_16 0.1875f
#define _1_4 0.25f

static Point3 GetOuterInside(Point3 a, Point3 b, Point3 c, Point3 d, Point3 e, Point3 f) {
	return a * _1_8 + b * _1_8 + c * _1_4 + d * _1_8 + e * _1_4 + f * _1_8;
	}

static Point3 GetNewEdgeVec(Point3 a, Point3 b, Point3 c, Point3 d, Point3 e, Point3 f, Point3 g, Point3 h) {
	return a * _1_16 + b * _1_16 + c * _3_16 + d * _3_16 + e * _1_16 + f * _1_16 + g * _3_16 + h * _3_16;
	}

static Point3 GetCentralInterior(Point3 a, Point3 b, Point3 c, Point3 d, Point3 e, Point3 f, Point3 g, Point3 h, Point3 i, Point3 j) {
	return a * _1_16 + b * _1_8 + c * _1_16 + d * _1_16 + e * _1_16 + f * _1_16 + g * _1_16 + h * _3_16 + i * _3_16 + j * _1_8;
	}

static Point3 GetNewEdgeCenter(Point3 a, Point3 b, Point3 c, Point3 d, Point3 e, Point3 f, Point3 g, Point3 h, Point3 i) {
	return a * _1_16 + b * _1_8 + c * _1_16 + d * _1_8 + e * _1_16 + f * _1_16 + g * _1_4 + h * _1_8 + i * _1_8;
	}

static Point3 GetOuterOutside(Point3 a, Point3 b, Point3 c, Point3 d) {
	return a * _1_4 + b * _1_4 + c * _1_4 + d * _1_4;
	}

static void FindNewTriEdge(PatchMesh *patch, Patch &p, int vert, Point3 &e1, Point3 &e2, Point3 &e3) {
	int a = vert;
	int b = vert * 3;
	int c = b + 1;
	int d = (b + 8) % 9;
	int e = (b + 7) % 9;
	int f = (b + 4) % 9;
	int g = vert;
	int h = (g + 1) % 3;
	int i = (g + 2) % 3;
	int j = (b + 6) % 9;
	int k = (b + 5) % 9;
	int l = b + 2;
	int m = (b + 3) % 9;
	Point3 pa = patch->verts[p.v[a]].p;
	Point3 pb = p.aux[b];
	Point3 pc = p.aux[c];
	Point3 pd = p.aux[d];
	Point3 pe = p.aux[e];
	Point3 pf = p.aux[f];
	Point3 pg = patch->vecs[p.interior[g]].p;
	Point3 ph = patch->vecs[p.interior[h]].p;
	Point3 pi = patch->vecs[p.interior[i]].p;
	Point3 pj = p.aux[j];
	Point3 pk = p.aux[k];
	Point3 pl = p.aux[l];
	Point3 pm = p.aux[m];
	e1 = GetNewEdgeVec(pa,pb,pd,pe,pj,pk,pg,pi);
	e2 = GetNewEdgeCenter(pa,pb,pc,pd,pe,pf,pg,ph,pi);
	e3 = GetNewEdgeVec(pa,pd,pb,pc,pl,pm,pg,ph);
	}

static void FindNewOuterTriInteriors(PatchMesh *patch, Patch &p, int vert, Point3 &i1, Point3 &i2, Point3 &i3) {
	int a = vert;
	int b = vert * 3;
	int c = (b + 8) % 9;
	int d = (b + 7) % 9;
	int e = vert;
	int f = (e + 2) % 3;
	int g = b + 1;
	int h = (e + 1) % 3;
	Point3 pa = patch->verts[p.v[a]].p;
	Point3 pb = p.aux[b];
	Point3 pc = p.aux[c];
	Point3 pd = p.aux[d];
	Point3 pe = patch->vecs[p.interior[e]].p;
	Point3 pf = patch->vecs[p.interior[f]].p;
	Point3 pg = p.aux[g];
	Point3 ph = patch->vecs[p.interior[h]].p;
	i1 = GetOuterOutside(pa,pb,pc,pe);
	i2 = GetOuterInside(pa,pc,pb,pg,pe,ph);
	i3 = GetOuterInside(pa,pb,pc,pd,pe,pf);
	}

static void FindNewInnerTriInteriors(PatchMesh *patch, Patch &p, Point3 &i1, Point3 &i2, Point3 &i3) {
	Point3 pa = p.aux[0];
	Point3 pb = p.aux[1];
	Point3 pc = p.aux[2];
	Point3 pd = p.aux[3];
	Point3 pe = p.aux[4];
	Point3 pf = p.aux[5];
	Point3 pg = p.aux[6];
	Point3 ph = p.aux[7];
	Point3 pi = p.aux[8];
	Point3 pj = patch->vecs[p.interior[0]].p;
	Point3 pk = patch->vecs[p.interior[1]].p;
	Point3 pl = patch->vecs[p.interior[2]].p;
	i1 = GetCentralInterior(pa,pb,pc,pi,ph,pe,pd,pj,pk,pl);
	i2 = GetCentralInterior(pd,pe,pf,pc,pb,ph,pg,pk,pl,pj);
	i3 = GetCentralInterior(pg,ph,pi,pf,pe,pb,pa,pl,pj,pk);
	}

// This is a first shot at a degree reducer which turns a degree-4 curve into a degree-3 curve,
// it probably won't give very good results unless the curve was converted from degree 3 to degree 4
// returns just the vector points
static void CubicFromQuartic(Point3 q1, Point3 q2, Point3 q3, Point3 q4, Point3 q5, Point3 &c2, Point3 &c3) {
	c2 = q1 + (q2 - q1) * 1.33333f;
	c3 = q5 + (q4 - q5) * 1.33333f;
	}

#define SUBDIV_EDGES 0
#define SUBDIV_PATCHES 1

static void SubdividePatch(int type, BOOL propagate, PatchMesh *patch) {
	int i;

	int verts = patch->getNumVerts();
	int vecs = patch->getNumVecs();
	int edges = patch->getNumEdges();
	int patches = patch->getNumPatches();

	// Make an edge flags array to note which edges must be processed
	BitArray eDiv(edges);
	// Make a patch flags array to note which patches must be processed
	BitArray pDiv(patches);
	// Make an edge flags array to note which edges have been done
	BitArray eDone(edges);
	eDone.ClearAll();
	// Make a patch flags array to note which patches have been done
	BitArray pDone(patches);
	pDone.ClearAll();

	switch(type) {
		case SUBDIV_EDGES:
			if(!patch->edgeSel.NumberSet())
				return;		// Nothing to do!
			eDiv = patch->edgeSel;
			pDiv.ClearAll();
			break;
		case SUBDIV_PATCHES:
			if(!patch->patchSel.NumberSet())
				return;		// Nothing to do!
			eDiv.ClearAll();
			pDiv = patch->patchSel;
			for(i = 0; i < patches; ++i) {
				if(pDiv[i]) {
					Patch &p = patch->patches[i];
					// Mark all edges for division
					eDiv.Set(p.edge[0]);
					eDiv.Set(p.edge[1]);
					eDiv.Set(p.edge[2]);
					if(p.type == PATCH_QUAD)
						eDiv.Set(p.edge[3]);
					}
				}
			// If not propagating, mark the edges as done
			if(!propagate)
				eDone = eDiv;
			break;
		}

	BOOL more = TRUE;
	while(more) {
		BOOL altered = FALSE;
		for(i = 0; i < edges; ++i) {
			if(eDiv[i] && !eDone[i]) {
				PatchEdge &e = patch->edges[i];
				pDiv.Set(e.patch1);
				if(e.patch2 >= 0)
					pDiv.Set(e.patch2);
				eDone.Set(i);
				altered = TRUE;
				}
			}
		if(altered && propagate) {
			for(i = 0; i < patches; ++i) {
				if(pDiv[i] && !pDone[i]) {
					Patch &p = patch->patches[i];
					if(p.type == PATCH_TRI) {	// Triangle -- tag all edges for division
						eDiv.Set(p.edge[0]);
						eDiv.Set(p.edge[1]);
						eDiv.Set(p.edge[2]);
						}
					else {		// Quad -- Tag edges opposite tagged edges
						if(eDiv[p.edge[0]])
							eDiv.Set(p.edge[2]);
						if(eDiv[p.edge[1]])
							eDiv.Set(p.edge[3]);
						if(eDiv[p.edge[2]])
							eDiv.Set(p.edge[0]);
						if(eDiv[p.edge[3]])
							eDiv.Set(p.edge[1]);
						}
					pDone.Set(i);
					}
				}
			}
		else
			more = FALSE;
		}

	// Keep a count of the new interior vectors
	int newInteriors = 0;

	// Also keep a count of the new vertices inside double-divided quads
	int newCenters = 0;

	// And a count of new texture vertices
	Tab<int> newTVerts;
	newTVerts.SetCount (patch->getNumMaps());
	for(int chan = 0; chan < patch->getNumMaps(); ++chan)
		newTVerts[chan] = 0;

	// And a count of new patches
	int newPatches = 0;

	int divPatches = pDiv.NumberSet();
	PatchDivInfo *pInfo = new PatchDivInfo [divPatches];
	int pDivIx;

	// Tag the edges that are on tagged patches but aren't tagged (only happens in propagate=0)
	// And set up a table with useful division info
	for(i = 0, pDivIx = 0; i < patches; ++i) {
		if(pDiv[i]) {
			PatchDivInfo &pi = pInfo[pDivIx];
			Patch &p = patch->patches[i];
			if(p.type == PATCH_TRI) {	// Triangle -- tag all edges for division
				eDiv.Set(p.edge[0]);
				eDiv.Set(p.edge[1]);
				eDiv.Set(p.edge[2]);
				newInteriors += (6 + 12);
				newPatches += 4;
				for(chan = 0; chan < patch->getNumMaps(); ++chan) {
					if(patch->tvPatches[chan])
						newTVerts[chan] += 3;
					}
				}
			else {		// Quad -- Tag edges opposite tagged edges
				int divs = 0;
				pi.div02 = pi.div13 = FALSE;
				if(eDiv[p.edge[0]]) {
					eDiv.Set(p.edge[2]);
					divs++;
					pi.div02 = TRUE;
					}
				else
				if(eDiv[p.edge[2]]) {
					eDiv.Set(p.edge[0]);
					divs++;
					pi.div02 = TRUE;
					}
				if(eDiv[p.edge[1]]) {
					eDiv.Set(p.edge[3]);
					divs++;
					pi.div13 = TRUE;
					}
				else
				if(eDiv[p.edge[3]]) {
					eDiv.Set(p.edge[1]);
					divs++;
					pi.div13 = TRUE;
					}
				newPatches += (divs==1) ? 2 : 4;
				newInteriors += (divs==1) ? (2 + 8) : (8 + 16);
				for(chan = 0; chan < patch->getNumMaps(); ++chan) {
					if(patch->tvPatches[chan]) {
						if(divs == 2)
							newTVerts[chan] += 5;
						else
							newTVerts[chan] += 2;
						}
					}
				if(divs==2)
					newCenters++;
				}
			pDivIx++;
			}
		}

	// Figure out how many new verts and vecs we'll need...
	int divEdges = eDiv.NumberSet();
	int newVerts = divEdges + newCenters;		// 1 new vert per edge
	int newVecs = divEdges * 4 + newInteriors;	// 4 new vectors per edge + new interior verts

	int vert = verts;
	Tab<int> tvert;
	tvert.SetCount (patch->getNumMaps());
	Tab<int> tverts;
	tverts.SetCount (patch->getNumMaps());
	Tab<int> tpat;
	tpat.SetCount (patch->getNumMaps());
	for(chan = 0; chan < patch->getNumMaps(); ++chan) {
		tverts[chan] = tvert[chan] = patch->getNumMapVerts (chan);
		tpat[chan] = patches;
	}
	int vec = vecs;
	int pat = patches;

	// Add the new vertices
	patch->setNumVerts(verts + newVerts, TRUE);

	// Add the new texture vertices
	for(chan = 0; chan < patch->getNumMaps(); ++chan)
		patch->setNumMapVerts (chan, tverts[chan] + newTVerts[chan], TRUE);

	// Add the new vectors
	patch->setNumVecs(vecs + newVecs, TRUE);

	// Add the new patches
	patch->setNumPatches(patches + newPatches, TRUE);

	// Create a new edge map
	NewEdge *eMap = new NewEdge [edges];
	for(i = 0; i < edges; ++i) {
		if(eDiv[i]) {
			PatchEdge &edge = patch->edges[i];
			NewEdge &map = eMap[i];
			map.oldEdge = i;
			map.v1 = edge.v1;
			map.vec12 = vec++;
			map.vec21 = vec++;
			map.v2 = vert++;
			map.vec23 = vec++;
			map.vec32 = vec++;
			map.v3 = edge.v2;
			
			// Compute the new edge vertex and vectors
			Point3 v00 = patch->verts[edge.v1].p;
			Point3 v10 = patch->vecs[edge.vec12].p;
			Point3 v20 = patch->vecs[edge.vec21].p;
			Point3 v30 = patch->verts[edge.v2].p;
			Point3 v01 = (v10 + v00) / 2.0f;
			Point3 v21 = (v30 + v20) / 2.0f;
			Point3 v11 = (v20 + v10) / 2.0f;
			Point3 v02 = (v11 + v01) / 2.0f;
			Point3 v12 = (v21 + v11) / 2.0f;
			Point3 v03 = (v12 + v02) / 2.0f;

			patch->verts[map.v2].p = v03;
			patch->vecs[map.vec12].p = v01;
			patch->vecs[map.vec21].p = v02;
			patch->vecs[map.vec23].p = v12;
			patch->vecs[map.vec32].p = v21;
			}
		}

#ifdef DUMPING
// Dump edge map
DebugPrint("Edge map:\n");
for(i = 0; i < edges; ++i) {
	NewEdge &e = eMap[i];
	DebugPrint("Old edge: %d  New edge: %d (%d %d) %d (%d %d) %d\n",e.oldEdge,e.v1,e.vec12,e.vec21,e.v2,e.vec23,e.vec32,e.v3);
	}
#endif

	// Now go and subdivide them!

	for(i = 0, pDivIx = 0; i < patches; ++i) {
		if(pDiv[i]) {
			PatchDivInfo &pi = pInfo[pDivIx];
			Patch &p = patch->patches[i];
			if(p.type == PATCH_TRI) {
				// Need to create four new patches
				int newev1 = vec++;	// edge 0 -> edge 1
				int newev2 = vec++;	// edge 1 -> edge 0
				int newev3 = vec++;	// edge 1 -> edge 2
				int newev4 = vec++;	// edge 2 -> edge 1
				int newev5 = vec++;	// edge 2 -> edge 0
				int newev6 = vec++;	// edge 0 -> edge 2

				// Get pointers to new edges
				NewEdge &e0 = eMap[p.edge[0]];
				NewEdge &e1 = eMap[p.edge[1]];
				NewEdge &e2 = eMap[p.edge[2]];

				// See if edges need to be flopped
				BOOL flop0 = (e0.v1 == p.v[0]) ? FALSE : TRUE;
				BOOL flop1 = (e1.v1 == p.v[1]) ? FALSE : TRUE;
				BOOL flop2 = (e2.v1 == p.v[2]) ? FALSE : TRUE;

				// Create the four new patches
				Patch &p1 = patch->patches[pat++];
				Patch &p2 = patch->patches[pat++];
				Patch &p3 = patch->patches[pat++];
				Patch &p4 = patch->patches[pat++];

				p1.SetType(PATCH_TRI);
				p1.v[0] = e0.v2;
				p1.v[1] = flop1 ? e1.v3 : e1.v1;
				p1.v[2] = e1.v2;
				p1.vec[0] = flop0 ? e0.vec21 : e0.vec23;
				p1.vec[1] = flop0 ? e0.vec12 : e0.vec32;
				p1.vec[2] = flop1 ? e1.vec32 : e1.vec12;
				p1.vec[3] = flop1 ? e1.vec23 : e1.vec21;
				p1.vec[4] = newev2;
				p1.vec[5] = newev1;
				p1.interior[0] = vec++;
				p1.interior[1] = vec++;
				p1.interior[2] = vec++;

				p2.SetType(PATCH_TRI);
				p2.v[0] = e1.v2;
				p2.v[1] = flop2 ? e2.v3 : e2.v1;
				p2.v[2] = e2.v2;
				p2.vec[0] = flop1 ? e1.vec21 : e1.vec23;
				p2.vec[1] = flop1 ? e1.vec12 : e1.vec32;
				p2.vec[2] = flop2 ? e2.vec32 : e2.vec12;
				p2.vec[3] = flop2 ? e2.vec23 : e2.vec21;
				p2.vec[4] = newev4;
				p2.vec[5] = newev3;
				p2.interior[0] = vec++;
				p2.interior[1] = vec++;
				p2.interior[2] = vec++;

				p3.SetType(PATCH_TRI);
				p3.v[0] = e0.v2;
				p3.v[1] = e1.v2;
				p3.v[2] = e2.v2;
				p3.vec[0] = newev1;
				p3.vec[1] = newev2;
				p3.vec[2] = newev3;
				p3.vec[3] = newev4;
				p3.vec[4] = newev5;
				p3.vec[5] = newev6;
				p3.interior[0] = vec++;
				p3.interior[1] = vec++;
				p3.interior[2] = vec++;

				p4.SetType(PATCH_TRI);
				p4.v[0] = flop0 ? e0.v3 : e0.v1;
				p4.v[1] = e0.v2;
				p4.v[2] = e2.v2;
				p4.vec[0] = flop0 ? e0.vec32 : e0.vec12;
				p4.vec[1] = flop0 ? e0.vec23 : e0.vec21;
				p4.vec[2] = newev6;
				p4.vec[3] = newev5;
				p4.vec[4] = flop2 ? e2.vec21 : e2.vec23;
				p4.vec[5] = flop2 ? e2.vec12 : e2.vec32;
				p4.interior[0] = vec++;
				p4.interior[1] = vec++;
				p4.interior[2] = vec++;

				// If this patch is textured, create three new texture verts for it
				for(chan = 0; chan < patch->getNumMaps(); ++chan) {
					if(patch->tvPatches[chan]) {
						int tva = tvert[chan]++;
						int tvb = tvert[chan]++;
						int tvc = tvert[chan]++;
						TVPatch &tp = patch->tvPatches[chan][i];
						TVPatch &tp1 = patch->tvPatches[chan][tpat[chan]++];
						TVPatch &tp2 = patch->tvPatches[chan][tpat[chan]++];
						TVPatch &tp3 = patch->tvPatches[chan][tpat[chan]++];
						TVPatch &tp4 = patch->tvPatches[chan][tpat[chan]++];
						tp1.tv[0] = tva;
						tp1.tv[1] = tp.tv[1];
						tp1.tv[2] = tvb;
						tp2.tv[0] = tvb;
						tp2.tv[1] = tp.tv[2];
						tp2.tv[2] = tvc;
						tp3.tv[0] = tva;
						tp3.tv[1] = tvb;
						tp3.tv[2] = tvc;
						tp4.tv[0] = tp.tv[0];
						tp4.tv[1] = tva;
						tp4.tv[2] = tvc;
						patch->tVerts[chan][tva] = (patch->tVerts[chan][tp.tv[0]] + patch->tVerts[chan][tp.tv[1]]) / 2.0f;
						patch->tVerts[chan][tvb] = (patch->tVerts[chan][tp.tv[1]] + patch->tVerts[chan][tp.tv[2]]) / 2.0f;
						patch->tVerts[chan][tvc] = (patch->tVerts[chan][tp.tv[2]] + patch->tVerts[chan][tp.tv[0]]) / 2.0f;
						}
					}

				// Now we'll compute the vectors for the three new edges being created inside this patch
				// These come back as degree 4's, and we need to reduce them to degree 3 for use in our
				// edges -- This is a bit risky because we aren't guaranteed a perfect fit.
				Point3 i1, i2, i3, i4, i5, i6, i7, i8, i9;
				FindNewTriEdge(patch, p, 0, i1, i2, i3);
				FindNewTriEdge(patch, p, 1, i4, i5, i6);
				FindNewTriEdge(patch, p, 2, i7, i8, i9);
				Point3 v1, v2, v3, v4, v5, v6;
				CubicFromQuartic(patch->verts[e2.v2].p, i1, i2, i3, patch->verts[e0.v2].p, v1, v2);
				CubicFromQuartic(patch->verts[e0.v2].p, i4, i5, i6, patch->verts[e1.v2].p, v3, v4);
				CubicFromQuartic(patch->verts[e1.v2].p, i7, i8, i9, patch->verts[e2.v2].p, v5, v6);
				patch->vecs[newev1].p = v3;
				patch->vecs[newev2].p = v4;
				patch->vecs[newev3].p = v5;
				patch->vecs[newev4].p = v6;
				patch->vecs[newev5].p = v1;
				patch->vecs[newev6].p = v2;
				// Now compute the interior vectors for the new patches if the one we're dividing isn't automatic
				// Must compute vectors for this patch's divided edges
				if(!(p.flags & PATCH_AUTO)) {
					p1.flags &= ~PATCH_AUTO;
					p2.flags &= ~PATCH_AUTO;
					p3.flags &= ~PATCH_AUTO;
					p4.flags &= ~PATCH_AUTO;

					FindNewOuterTriInteriors(patch, p, 1, patch->vecs[p1.interior[1]].p, patch->vecs[p1.interior[2]].p, patch->vecs[p1.interior[0]].p);
					FindNewOuterTriInteriors(patch, p, 2, patch->vecs[p2.interior[1]].p, patch->vecs[p2.interior[2]].p, patch->vecs[p2.interior[0]].p);
					FindNewInnerTriInteriors(patch, p, patch->vecs[p3.interior[0]].p, patch->vecs[p3.interior[1]].p, patch->vecs[p3.interior[2]].p);
					FindNewOuterTriInteriors(patch, p, 0, patch->vecs[p4.interior[0]].p, patch->vecs[p4.interior[1]].p, patch->vecs[p4.interior[2]].p);
					}
				}
			else {		// Quad patch
				// Check division flags to see how many patches we'll need
				if(pi.div02 && pi.div13) {		// Divide both ways
					// Need a new central vertex
					Point3 newc = p.interp(patch, 0.5f, 0.5f);
					patch->verts[vert].p = newc;
					int center = vert++;

					// Need to create four new patches
					int newev1 = vec++;	// edge 0 -> center
					int newev2 = vec++;	// center -> edge 0
					int newev3 = vec++;	// edge 1 -> center
					int newev4 = vec++;	// center -> edge 1
					int newev5 = vec++;	// edge 2 -> center
					int newev6 = vec++;	// center -> edge 2
					int newev7 = vec++;	// edge 3 -> center
					int newev8 = vec++;	// center -> edge 3

					// Get pointers to new edges
					NewEdge &e0 = eMap[p.edge[0]];
					NewEdge &e1 = eMap[p.edge[1]];
					NewEdge &e2 = eMap[p.edge[2]];
					NewEdge &e3 = eMap[p.edge[3]];

					// See if edges need to be flopped
					BOOL flop0 = (e0.v1 == p.v[0]) ? FALSE : TRUE;
					BOOL flop1 = (e1.v1 == p.v[1]) ? FALSE : TRUE;
					BOOL flop2 = (e2.v1 == p.v[2]) ? FALSE : TRUE;
					BOOL flop3 = (e3.v1 == p.v[3]) ? FALSE : TRUE;

					// Compute the new vectors for the dividing line
					Point3 w1,w2,w3,w4;
					w1 = InterpCenter(patch, i, 7, 0, 1, 2);
					w2 = InterpCenter(patch, i, 6, 3, 2, 3);
					w3 = InterpCenter(patch, i, 1, 1, 2, 4);
					w4 = InterpCenter(patch, i, 0, 0, 3, 5);
					Point3 new0 = patch->verts[e0.v2].p;
					Point3 new1 = patch->verts[e1.v2].p;
					Point3 new2 = patch->verts[e2.v2].p;
					Point3 new3 = patch->verts[e3.v2].p;
					InterpCenter(new0, w1, w2, new2, &patch->vecs[newev1].p, &patch->vecs[newev2].p, &patch->vecs[newev6].p, &patch->vecs[newev5].p);
					InterpCenter(new1, w3, w4, new3, &patch->vecs[newev3].p, &patch->vecs[newev4].p, &patch->vecs[newev8].p, &patch->vecs[newev7].p);

					// Create the four new patches
					Patch &p1 = patch->patches[pat++];
					Patch &p2 = patch->patches[pat++];
					Patch &p3 = patch->patches[pat++];
					Patch &p4 = patch->patches[pat++];

					p1.SetType(PATCH_QUAD);
					p1.v[0] = p.v[0];
					p1.v[1] = e0.v2;
					p1.v[2] = center;
					p1.v[3] = e3.v2;
					p1.vec[0] = flop0 ? e0.vec32 : e0.vec12;
					p1.vec[1] = flop0 ? e0.vec23 : e0.vec21;
					p1.vec[2] = newev1;
					p1.vec[3] = newev2;
					p1.vec[4] = newev8;
					p1.vec[5] = newev7;
					p1.vec[6] = flop3 ? e3.vec21 : e3.vec23;
					p1.vec[7] = flop3 ? e3.vec12 : e3.vec32;
					p1.interior[0] = vec++;
					p1.interior[1] = vec++;
					p1.interior[2] = vec++;
					p1.interior[3] = vec++;

					p2.SetType(PATCH_QUAD);
					p2.v[0] = p.v[1];
					p2.v[1] = e1.v2;
					p2.v[2] = center;
					p2.v[3] = e0.v2;
					p2.vec[0] = flop1 ? e1.vec32 : e1.vec12;
					p2.vec[1] = flop1 ? e1.vec23 : e1.vec21;
					p2.vec[2] = newev3;
					p2.vec[3] = newev4;
					p2.vec[4] = newev2;
					p2.vec[5] = newev1;
					p2.vec[6] = flop0 ? e0.vec21 : e0.vec23;
					p2.vec[7] = flop0 ? e0.vec12 : e0.vec32;
					p2.interior[0] = vec++;
					p2.interior[1] = vec++;
					p2.interior[2] = vec++;
					p2.interior[3] = vec++;

					p3.SetType(PATCH_QUAD);
					p3.v[0] = p.v[2];
					p3.v[1] = e2.v2;
					p3.v[2] = center;
					p3.v[3] = e1.v2;
					p3.vec[0] = flop2 ? e2.vec32 : e2.vec12;
					p3.vec[1] = flop2 ? e2.vec23 : e2.vec21;
					p3.vec[2] = newev5;
					p3.vec[3] = newev6;
					p3.vec[4] = newev4;
					p3.vec[5] = newev3;
					p3.vec[6] = flop1 ? e1.vec21 : e1.vec23;
					p3.vec[7] = flop1 ? e1.vec12 : e1.vec32;
					p3.interior[0] = vec++;
					p3.interior[1] = vec++;
					p3.interior[2] = vec++;
					p3.interior[3] = vec++;

					p4.SetType(PATCH_QUAD);
					p4.v[0] = p.v[3];
					p4.v[1] = e3.v2;
					p4.v[2] = center;
					p4.v[3] = e2.v2;
					p4.vec[0] = flop3 ? e3.vec32 : e3.vec12;
					p4.vec[1] = flop3 ? e3.vec23 : e3.vec21;
					p4.vec[2] = newev7;
					p4.vec[3] = newev8;
					p4.vec[4] = newev6;
					p4.vec[5] = newev5;
					p4.vec[6] = flop2 ? e2.vec21 : e2.vec23;
					p4.vec[7] = flop2 ? e2.vec12 : e2.vec32;
					p4.interior[0] = vec++;
					p4.interior[1] = vec++;
					p4.interior[2] = vec++;
					p4.interior[3] = vec++;

					// If this patch is textured, create five new texture verts for it
					for(chan = 0; chan < patch->getNumMaps(); ++chan) {
						if(patch->tvPatches[chan]) {
							int tva = tvert[chan]++;
							int tvb = tvert[chan]++;
							int tvc = tvert[chan]++;
							int tvd = tvert[chan]++;
							int tve = tvert[chan]++;
							TVPatch &tp = patch->tvPatches[chan][i];
							TVPatch &tp1 = patch->tvPatches[chan][tpat[chan]++];
							TVPatch &tp2 = patch->tvPatches[chan][tpat[chan]++];
							TVPatch &tp3 = patch->tvPatches[chan][tpat[chan]++];
							TVPatch &tp4 = patch->tvPatches[chan][tpat[chan]++];
							tp1.tv[0] = tp.tv[0];
							tp1.tv[1] = tva;
							tp1.tv[2] = tve;
							tp1.tv[3] = tvd;
							tp2.tv[0] = tp.tv[1];
							tp2.tv[1] = tvb;
							tp2.tv[2] = tve;
							tp2.tv[3] = tva;
							tp3.tv[0] = tp.tv[2];
							tp3.tv[1] = tvc;
							tp3.tv[2] = tve;
							tp3.tv[3] = tvb;
							tp4.tv[0] = tp.tv[3];
							tp4.tv[1] = tvd;
							tp4.tv[2] = tve;
							tp4.tv[3] = tvc;
							patch->tVerts[chan][tva] = (patch->tVerts[chan][tp.tv[0]] + patch->tVerts[chan][tp.tv[1]]) / 2.0f;
							patch->tVerts[chan][tvb] = (patch->tVerts[chan][tp.tv[1]] + patch->tVerts[chan][tp.tv[2]]) / 2.0f;
							patch->tVerts[chan][tvc] = (patch->tVerts[chan][tp.tv[2]] + patch->tVerts[chan][tp.tv[3]]) / 2.0f;
							patch->tVerts[chan][tvd] = (patch->tVerts[chan][tp.tv[3]] + patch->tVerts[chan][tp.tv[0]]) / 2.0f;
							patch->tVerts[chan][tve] = (patch->tVerts[chan][tp.tv[0]] + patch->tVerts[chan][tp.tv[1]] + patch->tVerts[chan][tp.tv[2]] + patch->tVerts[chan][tp.tv[3]]) / 4.0f;
							}
						}

					// If it's not an auto patch, compute the new interior points
					if(!(p.flags & PATCH_AUTO)) {
						p1.flags &= ~PATCH_AUTO;
						p2.flags &= ~PATCH_AUTO;
						p3.flags &= ~PATCH_AUTO;
						p4.flags &= ~PATCH_AUTO;

						Point3 a,b,c,d;
						Point3 a1,b1,c1,d1;
						Point3 a2,b2,c2,d2;
						Point3 a3,b3,c3,d3;
						Point3 a4,b4,c4,d4;
						InterpEdge(patch, i, 0.5f, 0, 0, 1, 1, &a1, &b1, &c1, &d1);
						InterpCenter(patch, i, 7, 0, 1, 2, &a2, &b2, &c2, &d2);
						InterpCenter(patch, i, 6, 3, 2, 3, &a3, &b3, &c3, &d3);
						InterpEdge(patch, i, 0.5f, 3, 5, 4, 2, &a4, &b4, &c4, &d4);

						InterpCenter(a1, a2, a3, a4, &a, &b, &c, &d);
						patch->vecs[p1.interior[0]].p = a;
						patch->vecs[p1.interior[3]].p = b;
						patch->vecs[p4.interior[1]].p = c;
						patch->vecs[p4.interior[0]].p = d;
						InterpCenter(b1, b2, b3, b4, &a, &b, &c, &d);
						patch->vecs[p1.interior[1]].p = a;
						patch->vecs[p1.interior[2]].p = b;
						patch->vecs[p4.interior[2]].p = c;
						patch->vecs[p4.interior[3]].p = d;
						InterpCenter(c1, c2, c3, c4, &a, &b, &c, &d);
						patch->vecs[p2.interior[3]].p = a;
						patch->vecs[p2.interior[2]].p = b;
						patch->vecs[p3.interior[2]].p = c;
						patch->vecs[p3.interior[1]].p = d;
						InterpCenter(d1, d2, d3, d4, &a, &b, &c, &d);
						patch->vecs[p2.interior[0]].p = a;
						patch->vecs[p2.interior[1]].p = b;
						patch->vecs[p3.interior[3]].p = c;
						patch->vecs[p3.interior[0]].p = d;
						}
					}
				else
				if(pi.div02) {					// Divide edges 0 & 2
					// Need to create two new patches
					// Compute new edge vectors between new edge verts
					int newev1 = vec++;	// edge 0 -> edge 2
					int newev2 = vec++;	// edge 2 -> edge 0

					// Get pointers to new edges
					NewEdge &e0 = eMap[p.edge[0]];
					NewEdge &e2 = eMap[p.edge[2]];

					// See if edges need to be flopped
					BOOL flop0 = (e0.v1 == p.v[0]) ? FALSE : TRUE;
					BOOL flop2 = (e2.v1 == p.v[2]) ? FALSE : TRUE;

					// Compute the new vectors for the dividing line
					
					patch->vecs[newev1].p = InterpCenter(patch, i, 7, 0, 1, 2);
					patch->vecs[newev2].p = InterpCenter(patch, i, 6, 3, 2, 3);

					// Create the two new patches
					Patch &p1 = patch->patches[pat++];
					Patch &p2 = patch->patches[pat++];

					p1.SetType(PATCH_QUAD);
					p1.v[0] = flop0 ? e0.v3 : e0.v1;
					p1.v[1] = e0.v2;
					p1.v[2] = e2.v2;
					p1.v[3] = flop2 ? e2.v1 : e2.v3;
					p1.vec[0] = flop0 ? e0.vec32 : e0.vec12;
					p1.vec[1] = flop0 ? e0.vec23 : e0.vec21;
					p1.vec[2] = newev1;
					p1.vec[3] = newev2;
					p1.vec[4] = flop2 ? e2.vec21 : e2.vec23;
					p1.vec[5] = flop2 ? e2.vec12 : e2.vec32;
					p1.vec[6] = p.vec[6];
					p1.vec[7] = p.vec[7];
					p1.interior[0] = vec++;
					p1.interior[1] = vec++;
					p1.interior[2] = vec++;
					p1.interior[3] = vec++;

					p2.SetType(PATCH_QUAD);
					p2.v[0] = e0.v2;
					p2.v[1] = flop0 ? e0.v1 : e0.v3;
					p2.v[2] = flop2 ? e2.v3 : e2.v1;
					p2.v[3] = e2.v2;
					p2.vec[0] = flop0 ? e0.vec21 : e0.vec23;
					p2.vec[1] = flop0 ? e0.vec12 : e0.vec32;
					p2.vec[2] = p.vec[2];
					p2.vec[3] = p.vec[3];
					p2.vec[4] = flop2 ? e2.vec32 : e2.vec12;
					p2.vec[5] = flop2 ? e2.vec23 : e2.vec21;
					p2.vec[6] = newev2;
					p2.vec[7] = newev1;
					p2.interior[0] = vec++;
					p2.interior[1] = vec++;
					p2.interior[2] = vec++;
					p2.interior[3] = vec++;

					// If this patch is textured, create two new texture verts for it
					for(chan = 0; chan < patch->getNumMaps(); ++chan) {
						if(patch->tvPatches[chan]) {
							int tva = tvert[chan]++;
							int tvb = tvert[chan]++;
							TVPatch &tp = patch->tvPatches[chan][i];
							TVPatch &tp1 = patch->tvPatches[chan][tpat[chan]++];
							TVPatch &tp2 = patch->tvPatches[chan][tpat[chan]++];
							tp1.tv[0] = tp.tv[0];
							tp1.tv[1] = tva;
							tp1.tv[2] = tvb;
							tp1.tv[3] = tp.tv[3];
							tp2.tv[0] = tva;
							tp2.tv[1] = tp.tv[1];
							tp2.tv[2] = tp.tv[2];
							tp2.tv[3] = tvb;
							patch->tVerts[chan][tva] = (patch->tVerts[chan][tp.tv[0]] + patch->tVerts[chan][tp.tv[1]]) / 2.0f;
							patch->tVerts[chan][tvb] = (patch->tVerts[chan][tp.tv[2]] + patch->tVerts[chan][tp.tv[3]]) / 2.0f;
							}
						}

					// If it's not an auto patch, compute the new interior points
					if(!(p.flags & PATCH_AUTO)) {
						p1.flags &= ~PATCH_AUTO;
						p2.flags &= ~PATCH_AUTO;

						Point3 a,b,c,d;
						InterpCenter(patch, i, 7, 0, 1, 2, &a, &b, &c, &d);
						patch->vecs[p1.interior[0]].p = a;
						patch->vecs[p1.interior[1]].p = b;
						patch->vecs[p2.interior[0]].p = c;
						patch->vecs[p2.interior[1]].p = d;
						InterpCenter(patch, i, 6, 3, 2, 3, &a, &b, &c, &d);
						patch->vecs[p1.interior[3]].p = a;
						patch->vecs[p1.interior[2]].p = b;
						patch->vecs[p2.interior[3]].p = c;
						patch->vecs[p2.interior[2]].p = d;
						}
					}
				else {							// Divide edges 1 & 3
					// Need to create two new patches
					// Compute new edge vectors between new edge verts
					int newev1 = vec++;	// edge 1 -> edge 3
					int newev2 = vec++;	// edge 3 -> edge 1

					// Get pointers to new edges
					NewEdge &e1 = eMap[p.edge[1]];
					NewEdge &e3 = eMap[p.edge[3]];

					// See if edges need to be flopped
					BOOL flop1 = (e1.v1 == p.v[1]) ? FALSE : TRUE;
					BOOL flop3 = (e3.v1 == p.v[3]) ? FALSE : TRUE;

					// Compute the new vectors for the dividing line
					patch->vecs[newev1].p = InterpCenter(patch, i, 1, 1, 2, 4);
					patch->vecs[newev2].p = InterpCenter(patch, i, 0, 0, 3, 5);

					// Create the two new patches
					Patch &p1 = patch->patches[pat++];
					Patch &p2 = patch->patches[pat++];

					p1.SetType(PATCH_QUAD);
					p1.v[0] = p.v[1];
					p1.v[1] = e1.v2;
					p1.v[2] = e3.v2;
					p1.v[3] = p.v[0];
					p1.vec[0] = flop1 ? e1.vec32 : e1.vec12;
					p1.vec[1] = flop1 ? e1.vec23 : e1.vec21;
					p1.vec[2] = newev1;
					p1.vec[3] = newev2;
					p1.vec[4] = flop3 ? e3.vec21 : e3.vec23;
					p1.vec[5] = flop3 ? e3.vec12 : e3.vec32;
					p1.vec[6] = p.vec[0];
					p1.vec[7] = p.vec[1];
					p1.interior[0] = vec++;
					p1.interior[1] = vec++;
					p1.interior[2] = vec++;
					p1.interior[3] = vec++;

					p2.SetType(PATCH_QUAD);
					p2.v[0] = e1.v2;
					p2.v[1] = p.v[2];
					p2.v[2] = p.v[3];
					p2.v[3] = e3.v2;
					p2.vec[0] = flop1 ? e1.vec21 : e1.vec23;
					p2.vec[1] = flop1 ? e1.vec12 : e1.vec32;
					p2.vec[2] = p.vec[4];
					p2.vec[3] = p.vec[5];
					p2.vec[4] = flop3 ? e3.vec32 : e3.vec12;
					p2.vec[5] = flop3 ? e3.vec23 : e3.vec21;
					p2.vec[6] = newev2;
					p2.vec[7] = newev1;
					p2.interior[0] = vec++;
					p2.interior[1] = vec++;
					p2.interior[2] = vec++;
					p2.interior[3] = vec++;

					// If this patch is textured, create two new texture verts for it
					for(chan = 0; chan < patch->getNumMaps(); ++chan) {
						if(patch->tvPatches[chan]) {
							int tva = tvert[chan]++;
							int tvb = tvert[chan]++;
							TVPatch &tp = patch->tvPatches[chan][i];
							TVPatch &tp1 = patch->tvPatches[chan][tpat[chan]++];
							TVPatch &tp2 = patch->tvPatches[chan][tpat[chan]++];
							tp1.tv[0] = tp.tv[1];
							tp1.tv[1] = tva;
							tp1.tv[2] = tvb;
							tp1.tv[3] = tp.tv[0];
							tp2.tv[0] = tva;
							tp2.tv[1] = tp.tv[2];
							tp2.tv[2] = tp.tv[3];
							tp2.tv[3] = tvb;
							patch->tVerts[chan][tva] = (patch->tVerts[chan][tp.tv[1]] + patch->tVerts[chan][tp.tv[2]]) / 2.0f;
							patch->tVerts[chan][tvb] = (patch->tVerts[chan][tp.tv[0]] + patch->tVerts[chan][tp.tv[3]]) / 2.0f;
							}
						}

					// If it's not an auto patch, compute the new interior points
					if(!(p.flags & PATCH_AUTO)) {
						p1.flags &= ~PATCH_AUTO;
						p2.flags &= ~PATCH_AUTO;

						Point3 a,b,c,d;
						InterpCenter(patch, i, 1, 1, 2, 4, &a, &b, &c, &d);
						patch->vecs[p1.interior[0]].p = a;
						patch->vecs[p1.interior[1]].p = b;
						patch->vecs[p2.interior[0]].p = c;
						patch->vecs[p2.interior[1]].p = d;
						InterpCenter(patch, i, 0, 0, 3, 5, &a, &b, &c, &d);
						patch->vecs[p1.interior[3]].p = a;
						patch->vecs[p1.interior[2]].p = b;
						patch->vecs[p2.interior[3]].p = c;
						patch->vecs[p2.interior[2]].p = d;
						}
					}	
				}
			pDivIx++;
			}
		}

	delete [] pInfo;
	delete [] eMap;

	// Now call the DeletePatchParts function to clean it all up
	BitArray dumVerts(patch->getNumVerts());
	dumVerts.ClearAll();
	BitArray dumPatches(patch->getNumPatches());
	dumPatches.ClearAll();
	// Mark the subdivided patches as deleted
	for(i = 0; i < patches; ++i)
		dumPatches.Set(i, pDiv[i]);

#ifdef DUMPING
DebugPrint("Before:\n");
patch->Dump();
#endif

	DeletePatchParts(patch, dumVerts, dumPatches);

#ifdef DUMPING
DebugPrint("After:\n");
patch->Dump();
#endif

	patch->computeInteriors();
	patch->buildLinkages();
	}

BOOL EdgeSubdivideRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord)
		oldPatch = *patch;
	SubdividePatch(SUBDIV_EDGES, propagate, patch);
	return TRUE;
	}

#define ESUBR_PROPAGATE_CHUNK		0x1000
#define ESUBR_PATCH_CHUNK			0x1010

IOResult EdgeSubdivideRecord::Load(ILoad *iload) {
	IOResult res;
	propagate = FALSE;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case ESUBR_PROPAGATE_CHUNK:
				propagate = TRUE;
				break;
//			case ESUBR_PATCH_CHUNK:
//				res = oldPatch.Load(iload);
//				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/		

BOOL PatchSubdivideRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord)
		oldPatch = *patch;
	SubdividePatch(SUBDIV_PATCHES, propagate, patch);
	return TRUE;
	}

#define PSUBR_PROPAGATE_CHUNK		0x1000
#define PSUBR_PATCH_CHUNK			0x1010

IOResult PatchSubdivideRecord::Load(ILoad *iload) {
	IOResult res;
	propagate = FALSE;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case PSUBR_PROPAGATE_CHUNK:
				propagate = TRUE;
				break;
//			case PSUBR_PATCH_CHUNK:
//				res = oldPatch.Load(iload);
//				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

BOOL PVertWeldRecord::Redo(PatchMesh *patch,int reRecord) {
	if(reRecord)
		oldPatch = *patch;
	patch->Weld(thresh);
	return TRUE;
	}

#define WELDR_THRESH_CHUNK			0x1010
#define WELDR_PATCH_CHUNK			0x1000

IOResult PVertWeldRecord::Load(ILoad *iload) {
	IOResult res;
	ULONG nb;
	propagate = FALSE;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case WELDR_THRESH_CHUNK:
				res = iload->Read(&thresh,sizeof(float),&nb);
				break;
//			case WELDR_PATCH_CHUNK:
//				res = oldPatch.Load(iload);
//				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

/*-------------------------------------------------------------------*/

PatchRestore::PatchRestore(EditPatchData* pd, EditPatchMod* mod, PatchMesh *patch, TCHAR *id)
	{
	gotRedo = FALSE;
	epd = pd;
	this->mod = mod;
	oldPatch = *patch;
	t = mod->ip->GetTime();
	where = TSTR(id);
	}

void PatchRestore::Restore(int isUndo)
	{
	if ( epd->tempData && epd->TempData(mod)->PatchCached(t) ) {
		PatchMesh *patch = epd->TempData(mod)->GetPatch(t);
		if(patch) {
			if(isUndo && !gotRedo) {
				newPatch = *patch;
				gotRedo = TRUE;
				}
			}
		DWORD selLevel = patch->selLevel;	// Grab this...
		DWORD dispFlags = patch->dispFlags;	// Grab this...
		*patch = oldPatch;
		patch->selLevel = selLevel;	// ...and put it back in
		patch->dispFlags = dispFlags;	// ...and put it back in
		patch->InvalidateGeomCache();
		epd->TempData(mod)->Invalidate(PART_GEOM | PART_TOPO | PART_SELECT);
		}
	else
	if ( epd->tempData ) {
		epd->TempData(mod)->Invalidate(PART_GEOM | PART_TOPO | PART_SELECT, FALSE);
		}
	if(mod->ip)
		Cancel2StepPatchModes(mod->ip);
	mod->InvalidateSurfaceUI();
	mod->SelectionChanged();
	mod->NotifyDependents(FOREVER, PART_GEOM | PART_TOPO | PART_SELECT, REFMSG_CHANGE);
	}

void PatchRestore::Redo()
	{
	if ( epd->tempData && epd->TempData(mod)->PatchCached(t) ) {
		PatchMesh *patch = epd->TempData(mod)->GetPatch(t);
		if(patch) {
			DWORD selLevel = patch->selLevel;	// Grab this...
			DWORD dispFlags = patch->dispFlags;	// Grab this...
			*patch = newPatch;
			patch->selLevel = selLevel;	// ...and put it back in
			patch->dispFlags = dispFlags;	// ...and put it back in
			patch->InvalidateGeomCache();
			}
		epd->TempData(mod)->Invalidate(PART_GEOM | PART_TOPO | PART_SELECT);
		}
	else
	if ( epd->tempData ) {
		epd->TempData(mod)->Invalidate(PART_GEOM | PART_TOPO | PART_SELECT,FALSE);
		}
	if(mod->ip)
		Cancel2StepPatchModes(mod->ip);
	mod->InvalidateSurfaceUI();
	mod->SelectionChanged();
	mod->NotifyDependents(FOREVER, PART_GEOM | PART_TOPO | PART_SELECT, REFMSG_CHANGE);
	}

/*-------------------------------------------------------------------*/

PatchSelRestore::PatchSelRestore(EditPatchData* pd, EditPatchMod* mod, PatchMesh *patch)
	{
	gotRedo = FALSE;
	epd = pd;
	this->mod = mod;
	oldVSel = patch->vertSel;
	oldESel = patch->edgeSel;
	oldPSel = patch->patchSel;
	t = mod->ip->GetTime();
	}

void PatchSelRestore::Restore(int isUndo)
	{
	if ( epd->tempData && epd->TempData(mod)->PatchCached(t) ) {
		PatchMesh *patch = epd->TempData(mod)->GetPatch(t);
		if(patch) {
			if(isUndo && !gotRedo) {
				newVSel = patch->vertSel;
				newESel = patch->edgeSel;
				newPSel = patch->patchSel;
				gotRedo = TRUE;
				}
			}
		patch->vertSel = oldVSel;
		patch->edgeSel = oldESel;
		patch->patchSel = oldPSel;
		epd->TempData(mod)->Invalidate(PART_GEOM | PART_TOPO | PART_SELECT);
		}
	else
	if ( epd->tempData ) {
		epd->TempData(mod)->Invalidate(PART_GEOM | PART_TOPO | PART_SELECT, FALSE);
		}
	if(mod->ip)
		Cancel2StepPatchModes(mod->ip);
	mod->InvalidateSurfaceUI();
//	mod->PatchSelChanged();
//	mod->UpdateSelectDisplay();
	mod->NotifyDependents(FOREVER, PART_GEOM | PART_TOPO | PART_SELECT, REFMSG_CHANGE);
	}

void PatchSelRestore::Redo()
	{
	if ( epd->tempData && epd->TempData(mod)->PatchCached(t) ) {
		PatchMesh *patch = epd->TempData(mod)->GetPatch(t);
		if(patch) {
			patch->vertSel = newVSel;
			patch->edgeSel = newESel;
			patch->patchSel = newPSel;
			}
		epd->TempData(mod)->Invalidate(PART_GEOM | PART_TOPO | PART_SELECT);
		}
	else
	if ( epd->tempData ) {
		epd->TempData(mod)->Invalidate(PART_GEOM | PART_TOPO | PART_SELECT,FALSE);
		}
	if(mod->ip)
		Cancel2StepPatchModes(mod->ip);
	mod->InvalidateSurfaceUI();
//	mod->PatchSelChanged();
//	mod->UpdateSelectDisplay();
	mod->NotifyDependents(FOREVER, PART_GEOM | PART_TOPO | PART_SELECT, REFMSG_CHANGE);
	}

/*-------------------------------------------------------------------*/

BOOL PickPatchAttach::Filter(INode *node)
	{
	ModContextList mcList;		
	INodeTab nodes;
	if (node) {
		// Make sure the node does not depend on us
		node->BeginDependencyTest();
		ep->NotifyDependents(FOREVER,0,REFMSG_TEST_DEPENDENCY);
		if (node->EndDependencyTest()) return FALSE;

		ObjectState os = node->GetObjectRef()->Eval(ep->ip->GetTime());
		GeomObject *object = (GeomObject *)os.obj;
		// Make sure it isn't one of the nodes we're editing, for heaven's sake!
		ep->ip->GetModContexts(mcList,nodes);
		int numNodes = nodes.Count();
		for(int i = 0; i < numNodes; ++i) {
			if(nodes[i] == node) {
				nodes.DisposeTemporary();
				return FALSE;
				}
			}
		if(object->CanConvertToType(patchObjectClassID)) {
			nodes.DisposeTemporary();
			return TRUE;
			}
		}
	nodes.DisposeTemporary();
	return FALSE;
	}

BOOL PickPatchAttach::HitTest(
		IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)
	{	
	INode *node = ip->PickNode(hWnd,m,this);
	ModContextList mcList;		
	INodeTab nodes;
	
	if (node) {
		ObjectState os = node->GetObjectRef()->Eval(ip->GetTime());
		GeomObject *object = (GeomObject *)os.obj;
		// Make sure it isn't one of the nodes we're editing, for heaven's sake!
		ep->ip->GetModContexts(mcList,nodes);
		int numNodes = nodes.Count();
		for(int i = 0; i < numNodes; ++i) {
			if(nodes[i] == node) {
				nodes.DisposeTemporary();
				return FALSE;
				}
			}
		if(object->CanConvertToType(patchObjectClassID)) {
			nodes.DisposeTemporary();
			return TRUE;
			}
		}

	nodes.DisposeTemporary();
	return FALSE;
	}

BOOL PickPatchAttach::Pick(IObjParam *ip,ViewExp *vpt)
	{
	INode *node = vpt->GetClosestHit();
	assert(node);
	GeomObject *object = (GeomObject *)node->GetObjectRef()->Eval(ip->GetTime()).obj;
	if(object->CanConvertToType(patchObjectClassID)) {
		PatchObject *attPatch = (PatchObject *)object->ConvertToType(ip->GetTime(),patchObjectClassID);
		if(attPatch) {
			PatchMesh patch = attPatch->patch;
			ModContextList mcList;
			INodeTab nodes;
			ip->GetModContexts(mcList,nodes);
			BOOL res = TRUE;
			if (nodes[0]->GetMtl() && node->GetMtl() && (nodes[0]->GetMtl()!=node->GetMtl()))
				res = DoAttachMatOptionDialog(ep->ip, ep);
			if(res) {
				bool canUndo = TRUE;
				ep->DoAttach(node, &patch, canUndo);
				if (!canUndo)
					GetSystemSetting (SYSSET_CLEAR_UNDO);
				}
			nodes.DisposeTemporary();
			// Discard the copy it made, if it isn't the same as the object itself
			if(attPatch != (PatchObject *)object)
				delete attPatch;
			}
		}
	return FALSE;
	}


void PickPatchAttach::EnterMode(IObjParam *ip)
	{
	if ( ep->hOpsPanel ) {
		ICustButton *but = GetICustButton(GetDlgItem(ep->hOpsPanel,IDC_ATTACH));
		but->SetCheck(TRUE);
		ReleaseICustButton(but);
		}
	}

void PickPatchAttach::ExitMode(IObjParam *ip)
	{
	if ( ep->hOpsPanel ) {
		ICustButton *but = GetICustButton(GetDlgItem(ep->hOpsPanel,IDC_ATTACH));
		but->SetCheck(FALSE);
		ReleaseICustButton(but);
		}
	}

HCURSOR PickPatchAttach::GetHitCursor(IObjParam *ip) {
	return LoadCursor(hInstance, MAKEINTRESOURCE(IDC_ATTACHCUR));
	}

int EditPatchMod::DoAttach(INode *node, PatchMesh *attPatch, bool & canUndo) {
	ModContextList mcList;	
	INodeTab nodes;	

	if ( !ip ) return 0;

	ip->GetModContexts(mcList,nodes);

	if(mcList.Count() != 1) {
		nodes.DisposeTemporary();
		return 0;
		}

	EditPatchData *patchData = (EditPatchData*)mcList[0]->localData;
	if ( !patchData ) {
		nodes.DisposeTemporary();
		return 0;
		}
	patchData->BeginEdit(ip->GetTime());

	// If the mesh isn't yet cached, this will cause it to get cached.
	PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
	if(!patch) {
		nodes.DisposeTemporary();
		return 0;
		}
	patchData->RecordTopologyTags(patch);
	RecordTopologyTags();

	// Transform the shape for attachment:
	// If reorienting, just translate to align pivots
	// Otherwise, transform to match our transform
	Matrix3 attMat(1);
	if(attachReorient) {
		Matrix3 thisTM = nodes[0]->GetNodeTM(ip->GetTime());
		Matrix3 thisOTMBWSM = nodes[0]->GetObjTMBeforeWSM(ip->GetTime());
		Matrix3 thisPivTM = thisTM * Inverse(thisOTMBWSM);
		Matrix3 otherTM = node->GetNodeTM(ip->GetTime());
		Matrix3 otherOTMBWSM = node->GetObjTMBeforeWSM(ip->GetTime());
		Matrix3 otherPivTM = otherTM * Inverse(otherOTMBWSM);
		Point3 otherObjOffset = node->GetObjOffsetPos();
		attMat = Inverse(otherPivTM) * thisPivTM;
		}
	else {
		attMat = node->GetObjectTM(ip->GetTime()) *
			Inverse(nodes[0]->GetObjectTM(ip->GetTime()));
		}
		
	// RB 3-17-96 : Check for mirroring
	AffineParts parts;
	decomp_affine(attMat,&parts);
	if (parts.f<0.0f) {
		int v[8], ct, ct2, j;
		Point3 p[9];
//watje 10-21-99  212991 since there is topochange need to record and resolve topo 
//		even though it is a temporary object  to update any bind
		attPatch->RecordTopologyTags();

		for (int i=0; i<attPatch->numPatches; i++) {
			
			// Re-order vertices
			ct = attPatch->patches[i].type==PATCH_QUAD ? 4 : 3;
			for (j=0; j<ct; j++) {
				v[j] = attPatch->patches[i].v[j];
				}
			for (j=0; j<ct; j++) {
				attPatch->patches[i].v[j] = v[ct-j-1];
				}

			// Re-order vecs
			ct  = attPatch->patches[i].type==PATCH_QUAD ? 8 : 6;
			ct2 = attPatch->patches[i].type==PATCH_QUAD ? 5 : 3;
			for (j=0; j<ct; j++) {
				v[j] = attPatch->patches[i].vec[j];
				}
			for (j=0; j<ct; j++,ct2--) {
				if (ct2<0) ct2 = ct-1;
				attPatch->patches[i].vec[j] = v[ct2];
				}

			// Re-order enteriors
			if (attPatch->patches[i].type==PATCH_QUAD) {
				ct = 4;
				for (j=0; j<ct; j++) {
					v[j] = attPatch->patches[i].interior[j];
					}
				for (j=0; j<ct; j++) {
					attPatch->patches[i].interior[j] = v[ct-j-1];
					}
				}

			// Re-order aux
			if (attPatch->patches[i].type==PATCH_TRI) {
				ct = 9;
				for (j=0; j<ct; j++) {
					p[j] = attPatch->patches[i].aux[j];
					}
				for (j=0; j<ct; j++) {
					attPatch->patches[i].aux[j] = p[ct-j-1];
					}
				}

			// Re-order TV faces if present
			for(int chan = 0; chan < patch->getNumMaps(); ++chan) {
				if (attPatch->tvPatches[chan]) {
					ct = attPatch->patches[i].type==PATCH_QUAD ? 4 : 3;
					for (j=0; j<ct; j++) {
						v[j] = attPatch->tvPatches[chan][i].tv[j];
						}
					for (j=0; j<ct; j++) {
						attPatch->tvPatches[chan][i].tv[j] = v[ct-j-1];
						}
					}
				}
			}
//watje 10-21-99  212991 since there is topochange need to record and resolve topo 
//		even though it is a temporary object  to update any bind
		attPatch->buildLinkages();
		attPatch->HookFixTopology();
		}

	for(int i = 0; i < attPatch->numVerts; ++i)
		attPatch->verts[i].p = attPatch->verts[i].p * attMat;
	for(i = 0; i < attPatch->numVecs; ++i)
		attPatch->vecs[i].p = attPatch->vecs[i].p * attMat;
	attPatch->computeInteriors();

	theHold.Begin();

	// Combine the materials of the two nodes.
	int mat2Offset=0;
	Mtl *m1 = nodes[0]->GetMtl();
	Mtl *m2 = node->GetMtl();
	bool condenseMe = FALSE;
	if (m1 && m2 && (m1 != m2)) {
		if (attachMat==ATTACHMAT_IDTOMAT) {
			int ct=1;
			if (m1->IsMultiMtl())
				ct = m1->NumSubMtls();
			for(int i = 0; i < patch->numPatches; ++i) {
				int mtid = patch->getPatchMtlIndex(i);
				if(mtid >= ct)
					patch->setPatchMtlIndex(i, mtid % ct);
				}
			FitPatchIDsToMaterial (*attPatch, m2);
			if (condenseMat) condenseMe = TRUE;
			}
		// the theHold calls here were a vain attempt to make this all undoable.
		// This should be revisited in the future so we don't have to use the SYSSET_CLEAR_UNDO.
		theHold.Suspend ();
		if (attachMat==ATTACHMAT_MATTOID) {
			m1 = FitMaterialToPatchIDs (*patch, m1);
			m2 = FitMaterialToPatchIDs (*attPatch, m2);
			}

		Mtl *multi = CombineMaterials (m1, m2, mat2Offset);
		if (attachMat == ATTACHMAT_NEITHER) mat2Offset = 0;
		theHold.Resume ();
		// We can't be in face subobject mode, else we screw up the materials:
		DWORD oldSL = patch->selLevel;
		patch->selLevel = PATCH_OBJECT;
		nodes[0]->SetMtl(multi);
		patch->selLevel = oldSL;
		m1 = multi;
		canUndo = FALSE;	// Absolutely cannot undo material combinations.
		}
	if (!m1 && m2) {
		// We can't be in face subobject mode, else we screw up the materials:
		DWORD oldSL = patch->selLevel;
		patch->selLevel = PATCH_OBJECT;
		nodes[0]->SetMtl(m2);
		patch->selLevel = oldSL;
		m1 = m2;
		}

	// Start a restore object...
	if ( theHold.Holding() )
		theHold.Put(new PatchRestore(patchData,this,patch,"DoAttach"));

	// Do the attach
	patch->Attach(attPatch, mat2Offset);
	patchData->UpdateChanges(patch);
	patchData->TempData(this)->Invalidate(PART_TOPO|PART_GEOM);

	// Get rid of the original node
	ip->DeleteNode(node);

	ResolveTopoChanges();
	theHold.Accept(GetString(IDS_TH_ATTACH));

	if (m1 && condenseMe) {
		// Following clears undo stack.
		patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		m1 = CondenseMatAssignments (*patch, m1);
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO|PART_GEOM, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	return 1;
	}

/*-------------------------------------------------------------------*/

EditPatchMod::EditPatchMod()
	{
	selLevel = EP_OBJECT;
	displayLattice = TRUE;
	displaySurface = TRUE;
	propagate = TRUE;
	meshSteps = 5;
//3-18-99 to suport render steps and removal of the mental tesselator
	meshStepsRender = 5;
	showInterior = TRUE;

	namedSelNeedsFixup = FALSE;
//	meshAdaptive = FALSE;	// Future use (Not used now)
	}

EditPatchMod::~EditPatchMod()
	{
	ClearSetNames();
	}

Interval EditPatchMod::LocalValidity(TimeValue t)
	{
	// Force a cache if being edited.
	if (TestAFlag(A_MOD_BEING_EDITED))
		return NEVER;  			   
	return FOREVER;
	}

RefTargetHandle EditPatchMod::Clone(RemapDir& remap) {
	EditPatchMod* newmod = new EditPatchMod();	
	newmod->selLevel = selLevel;
	newmod->displaySurface = displaySurface;
	newmod->displayLattice = displayLattice;
	newmod->meshSteps = meshSteps;
//3-18-99 to suport render steps and removal of the mental tesselator
	newmod->meshStepsRender = meshStepsRender;
	newmod->showInterior = showInterior;

//	newmod->meshAdaptive = meshAdaptive;	// Future use (Not used now)
	newmod->viewTess = viewTess;
	newmod->prodTess = prodTess;
	newmod->dispTess = dispTess;
	newmod->mViewTessNormals = mViewTessNormals;
	newmod->mProdTessNormals = mProdTessNormals;
	newmod->mViewTessWeld = mViewTessWeld;
	newmod->mProdTessWeld = mProdTessWeld;
	newmod->propagate = propagate;
	return(newmod);
	}

void EditPatchMod::ClearPatchDataFlag(ModContextList& mcList,DWORD f)
	{
	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		patchData->SetFlag(f,FALSE);
		}
	}

void EditPatchMod::XFormHandles(XFormProc *xproc, TimeValue t, Matrix3& partm, Matrix3& tmAxis,	int object, int handleIndex)
	{	
	ModContextList mcList;		
	INodeTab nodes;
	Matrix3 mat,imat,theMatrix;
	Interval valid;
	int numAxis;
	Point3 oldpt,newpt,oldin,oldout,rel;
	BOOL shiftPressed = FALSE;
	static BOOL wasBroken;
	Point3 theKnot;
	Point3 oldVector;
	Point3 newVector;
	float oldLen;
	float newLen;
//DebugPrint("XFormHandles\n");
	shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) ? TRUE : FALSE;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	numAxis = ip->GetNumAxis();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	EditPatchData *patchData = (EditPatchData*)mcList[object]->localData;
	if ( !patchData ) {
		nodes.DisposeTemporary();
		return;
		}
	
	// If the mesh isn't yet cache, this will cause it to get cached.
	PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
	if(!patch) {
		nodes.DisposeTemporary();
		return;
		}
			
	// If this is the first edit, then the delta arrays will be allocated
	patchData->BeginEdit(t);

	// Create a change record for this object and store a pointer to its delta info in this EditPatchData
	if(!TestAFlag(A_HELD)) {
		patchData->vdelta.SetSize(*patch,FALSE);
		if ( theHold.Holding() ) {
			theHold.Put(new PatchRestore(patchData,this,patch,"XFormHandles"));
			}
		patchData->vdelta.Zero();		// Reset all deltas
		patchData->ClearHandleFlag();
		wasBroken = FALSE;
		}
	else {
		if(wasBroken && !shiftPressed)
			wasBroken = FALSE;
		if(patchData->DoingHandles())
			patchData->ApplyHandlesAndZero(*patch);		// Reapply the slave handle deltas
		else
			patchData->vdelta.Zero();
		}

	patchData->SetHandleFlag(handleIndex);
	int primaryKnot = patch->vecs[handleIndex].vert;
	Point3Tab &pDeltas = patchData->vdelta.dtab.vtab;

	tmAxis = ip->GetTransformAxis(nodes[object],primaryKnot);
	mat    = nodes[object]->GetObjectTM(t,&valid) * Inverse(tmAxis);
	imat   = Inverse(mat);
	xproc->SetMat(mat);
				
	// XForm the cache vertices
	oldpt = patch->vecs[handleIndex].p;
	newpt = xproc->proc(oldpt,mat,imat);

	// Update the vector being moved
	patch->vecs[handleIndex].p = newpt;

	// Move the delta's vertices.
	patchData->vdelta.SetVec(handleIndex,newpt - oldpt);

	if(primaryKnot >= 0) {
		PatchVert &vert = patch->verts[primaryKnot];
		theKnot = vert.p;
		// If locked handles, turn the movement into a transformation matrix
		// and transform all the handles attached to the owner vertex
		if(lockedHandles) {
			if(!wasBroken && shiftPressed)
				wasBroken = TRUE;
			goto locked_handles;
			}
		else {
			if(shiftPressed) {
				wasBroken = TRUE;
				vert.flags &= ~PVERT_COPLANAR;
				// Need to record this for undo!
				patchData->vdelta.SetVertType(primaryKnot,PVERT_COPLANAR);
				}
			// If a coplanar knot, do the other vectors!
			// If at the same point as knot, do nothing!
			if((vert.flags & PVERT_COPLANAR) && (vert.vectors.Count() > 2) && !(newpt == theKnot)) {
locked_handles:
				oldVector = oldpt - theKnot;
				newVector = newpt - theKnot;
				oldLen = Length(oldVector);
				newLen = Length(newVector);
				Point3 oldNorm = Normalize(oldVector);
				Point3 newNorm = Normalize(newVector);
				theMatrix.IdentityMatrix();
				Point3 axis;
				float angle = 0.0f;
				int owner = patch->vecs[handleIndex].vert;
				if(owner >= 0) {
					PatchVert &vert = patch->verts[owner];
					int vectors = vert.vectors.Count();
					// Watch out for cases where the vectors are exactly opposite -- This
					// results in an invalid axis for transformation!
					// In this case, we look for a vector to one of the other handles that
					// will give us a useful vector for the rotational axis
					if(newNorm == -oldNorm) {
						for(int v = 0; v < vectors; ++v) {
							int theVec = vert.vectors[v];
							// Ignore the vector being moved!
							if(theVec != handleIndex) {
								Point3 testVec = patch->vecs[theVec].p - pDeltas[theVec] - theKnot;
								if(testVec != zeroPoint) {
									Point3 testNorm = Normalize(testVec);
									if(!(testNorm == newNorm) && !(testNorm == oldNorm)) {
										// Cross product gives us the normal of the rotational axis
										axis = Normalize(testNorm ^ newNorm);
										// The angle is 180 degrees
										angle = PI;
										goto build_matrix;
										}
									}
								}
							}
						}
					else {
						// Get a matrix that will transform the old point to the new one
						// Cross product gives us the normal of the rotational axis
						axis = Normalize(oldNorm ^ newNorm);
						// Dot product gives us the angle
						float dot = DotProd(oldNorm, newNorm);
						if(dot >= -1.0f && dot <= 1.0f)
							angle = (float)-acos(dot);
						}
build_matrix:
					if(angle != 0.0f) {
						// Now let's build a matrix that'll do this for us!
						Quat quat = QFromAngAxis(angle, axis);
						quat.MakeMatrix(theMatrix);
						if(lockedHandles) {
							// If need to break the vector, 
							if(shiftPressed && vert.flags & PVERT_COPLANAR) {
								vert.flags &= ~PVERT_COPLANAR;
								patchData->vdelta.SetVertType(primaryKnot,PVERT_COPLANAR);
								}
							}
						}
					// Process all other handles through the matrix
					for(int v = 0; v < vectors; ++v) {
						int theVec = vert.vectors[v];
						// Ignore the vector being moved!
						if(theVec != handleIndex) {
							Point3 oldpt2 = patch->vecs[theVec].p - pDeltas[theVec];
							Point3 newpt2 = (oldpt2 - theKnot) * theMatrix + theKnot;
							patch->vecs[theVec].p = newpt2;
							// Move the delta's vertices.
							patchData->vdelta.SetVec(theVec,newpt2 - oldpt2);
							}
						}
					}
				}
			}
		}

	// Really only need to do this if neighbor knots are non-bezier
	patch->computeInteriors();

	patchData->UpdateChanges(patch);					
	patchData->TempData(this)->Invalidate(PART_GEOM);
	patchData->SetFlag(EPD_BEENDONE,TRUE);
	
	// Mark all objects in selection set
	SetAFlag(A_HELD);
	
	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_GEOM, REFMSG_CHANGE);
	}

void EditPatchMod::XFormVerts( 
		XFormProc *xproc, 
		TimeValue t, 
		Matrix3& partm, 
		Matrix3& tmAxis  ) 
	{	
	ModContextList mcList;		
	INodeTab nodes;
	Matrix3 mat,imat;	
	Interval valid;
	int numAxis;
	Point3 oldpt,newpt,rel,delta;
	int shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) ? 1 : 0;
	static BOOL wasBroken;
	static BOOL handleEdit = FALSE;
	static int handleObject;
	static int handleIndex;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	numAxis = ip->GetNumAxis();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	if(!TestAFlag(A_HELD)) {
		handleEdit = FALSE;
//DebugPrint("Handle edit cleared\n");
		// Check all patches to see if they are altering a bezier vector handle...
		if(selLevel == EP_VERTEX) {
			for ( int i = 0; i < mcList.Count(); i++ ) {
				EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
				if ( !patchData ) continue;
				if ( patchData->GetFlag(EPD_BEENDONE) ) continue;
		
				// If the mesh isn't yet cache, this will cause it to get cached.
				PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
				if(!patch)
					continue;

				if(!ip->SelectionFrozen() && patch->bezVecVert >= 0) {
					// Editing a bezier handle -- Go do it!
					handleEdit = TRUE;
					handleObject = i;
					handleIndex = patch->bezVecVert;
					goto edit_handles;
					}
	 			patchData->SetFlag(EPD_BEENDONE,TRUE);
				}
			}
		}
	
	// If editing the handles, cut to the chase!
	if(handleEdit) {
		edit_handles:
		XFormHandles(xproc, t, partm, tmAxis, handleObject, handleIndex);
		nodes.DisposeTemporary();
		return;
		}

	// Not doing handles, just plain ol' verts
	ClearPatchDataFlag(mcList,EPD_BEENDONE);	// Clear these out again
	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;
		
		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
					
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// Create a change record for this object and store a pointer to its delta info in this EditPatchData
		if(!TestAFlag(A_HELD)) {
			patchData->vdelta.SetSize(*patch,FALSE);
			if ( theHold.Holding() ) {
				theHold.Put(new PatchRestore(patchData,this,patch,"XFormVerts"));
				}
			patchData->vdelta.Zero();		// Reset all deltas
			patchData->ClearHandleFlag();
			wasBroken = FALSE;
			}
		else {
			if(wasBroken)
				shiftPressed = TRUE;
			if(patchData->DoingHandles())
				patchData->ApplyHandlesAndZero(*patch);		// Reapply the slave handle deltas
			else
				patchData->vdelta.Zero();
			}

		// Compute the transforms
		if (numAxis==NUMAXIS_INDIVIDUAL) {
			switch(selLevel) {
				case EP_VERTEX: {
					// Selected vertices - either directly or indirectly through selected faces or edges.
					BitArray sel = patch->VertexTempSel();
					int verts = patch->numVerts;
					for( int vert = 0; vert < verts; vert++ ) {
						if ( sel[vert] ) {
							tmAxis = ip->GetTransformAxis(nodes[i],vert);
							mat    = nodes[i]->GetObjectTM(t,&valid) * Inverse(tmAxis);
							imat   = Inverse(mat);
							xproc->SetMat(mat);
			
							// XForm the cache vertices
							oldpt = patch->verts[vert].p;
							newpt = xproc->proc(oldpt,mat,imat);
							patch->verts[vert].p = newpt;
							delta = newpt - oldpt;

							// Move the delta's vertices.
							patchData->vdelta.MoveVert(vert,delta);

							// Also affect its vectors
							int vecs = patch->verts[vert].vectors.Count();
							for(int vec = 0; vec < vecs; ++vec) {
								int index = patch->verts[vert].vectors[vec];
								// XForm the cache vertices
								oldpt = patch->vecs[index].p;
								newpt = xproc->proc(oldpt,mat,imat);
								patch->vecs[index].p = newpt;
								delta = newpt - oldpt;

								// Move the delta's vertices.
								patchData->vdelta.MoveVec(index,delta);
								}
							}
						}
					patch->computeInteriors();	// Kind of broad-spectrum -- only need to recompute affected patches
					}
					break;
				case EP_EDGE:
				case EP_PATCH: {
					// Selected vertices - either directly or indirectly through selected faces or edges.
					BitArray sel = patch->VertexTempSel();
					int verts = patch->numVerts;
					for( int vert = 0; vert < verts; vert++ ) {
						if ( sel[vert] ) {
							tmAxis = ip->GetTransformAxis(nodes[i],vert);
							mat    = nodes[i]->GetObjectTM(t,&valid) * Inverse(tmAxis);
							imat   = Inverse(mat);
							xproc->SetMat(mat);
			
							// XForm the cache vertices
							oldpt = patch->verts[vert].p;
							newpt = xproc->proc(oldpt,mat,imat);
							patch->verts[vert].p = newpt;
							delta = newpt - oldpt;

							// Move the delta's vertices.
							patchData->vdelta.MoveVert(vert,delta);

							// Also affect its vectors
							int vecs = patch->verts[vert].vectors.Count();
							for(int vec = 0; vec < vecs; ++vec) {
								int index = patch->verts[vert].vectors[vec];
								// XForm the cache vertices
								oldpt = patch->vecs[index].p;
								newpt = xproc->proc(oldpt,mat,imat);
								patch->vecs[index].p = newpt;
								delta = newpt - oldpt;

								// Move the delta's vertices.
								patchData->vdelta.MoveVec(index,delta);
								}
							}
						}
					patch->computeInteriors();
					}
					break;
				}			
			}
		else {
			mat = nodes[i]->GetObjectTM(t,&valid) * Inverse(tmAxis);
			imat = Inverse(mat);
			xproc->SetMat(mat);

			// Selected vertices - either directly or indirectly through selected faces or edges.
			BitArray sel = patch->VertexTempSel();
			int verts = patch->numVerts;
			for( int vert = 0; vert < verts; vert++ ) {
				if ( sel[vert] ) {
					// XForm the cache vertices
					oldpt = patch->verts[vert].p;
					newpt = xproc->proc(oldpt,mat,imat);
					patch->verts[vert].p = newpt;
					delta = newpt - oldpt;

					// Move the delta's vertices.
					patchData->vdelta.MoveVert(vert,delta);

					// Also affect its vectors
					int vecs = patch->verts[vert].vectors.Count();
					for(int vec = 0; vec < vecs; ++vec) {
						int index = patch->verts[vert].vectors[vec];
						// XForm the cache vertices
						oldpt = patch->vecs[index].p;
						newpt = xproc->proc(oldpt,mat,imat);
						patch->vecs[index].p = newpt;
						delta = newpt - oldpt;

						// Move the delta's vertices.
						patchData->vdelta.MoveVec(index,delta);
						}
					}
				}
			patch->computeInteriors();
			}
		patchData->UpdateChanges(patch);					
		patchData->TempData(this)->Invalidate(PART_GEOM);
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	// Mark all objects in selection set
	SetAFlag(A_HELD);
	
	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_GEOM, REFMSG_CHANGE);
	}

void EditPatchMod::Move( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin )
	{
	MoveXForm proc(val);
	XFormVerts(&proc,t,partm,tmAxis); 	
	}

void EditPatchMod::Rotate( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Quat& val, BOOL localOrigin )
	{
	RotateXForm proc(val);
	XFormVerts(&proc,t,partm,tmAxis); 	
	}

void EditPatchMod::Scale( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin )
	{
	ScaleXForm proc(val);
	XFormVerts(&proc,t,partm,tmAxis); 	
	}

void EditPatchMod::TransformStart(TimeValue t)
	{
	if (ip) ip->LockAxisTripods(TRUE);
	}

void EditPatchMod::TransformFinish(TimeValue t)
	{
	if (ip) ip->LockAxisTripods(FALSE);
	UpdateSelectDisplay();

	if ( !ip ) return;	
	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;
		patchData->UpdateChanges(patch, FALSE);
		}
	}

void EditPatchMod::TransformCancel(TimeValue t)
	{
	if (ip) ip->LockAxisTripods(FALSE);
	}

void EditPatchMod::ModifyObject(TimeValue t, ModContext &mc, ObjectState * os, INode *node) 
	{		
//Alert(_T("in ModifyObject"));
	assert( os->obj->ClassID() == Class_ID(PATCHOBJ_CLASS_ID,0) );
//Alert(_T("ModifyObject class ID is OK"));
	
	PatchObject *patchOb = (PatchObject *)os->obj;
	EditPatchData *patchData;

	if ( !mc.localData ) {
		mc.localData = new EditPatchData(this);
		patchData = (EditPatchData*)mc.localData;
		meshSteps = patchData->meshSteps = patchOb->GetMeshSteps();
//3-18-99 to suport render steps and removal of the mental tesselator
		meshStepsRender = patchData->meshStepsRender = patchOb->GetMeshStepsRender();
		showInterior = patchData->showInterior = patchOb->GetShowInterior();

//		meshAdaptive = patchData->meshAdaptive = patchOb->GetAdaptive();	// Future use (Not used now)
		viewTess = patchData->viewTess = patchOb->GetViewTess();
		prodTess = patchData->prodTess = patchOb->GetProdTess();
		dispTess = patchData->dispTess = patchOb->GetDispTess();
		mViewTessNormals = patchData->mViewTessNormals = patchOb->GetViewTessNormals();
		mProdTessNormals = patchData->mProdTessNormals = patchOb->GetProdTessNormals();
		mViewTessWeld = patchData->mViewTessWeld = patchOb->GetViewTessWeld();
		mProdTessWeld = patchData->mProdTessWeld = patchOb->GetProdTessWeld();
		displayLattice = patchData->displayLattice = patchOb->ShowLattice();
		displaySurface = patchData->displaySurface = patchOb->showMesh;
	} else {
		patchData = (EditPatchData*)mc.localData;
		}

	PatchMesh &pmesh = patchOb->patch;	
	assert(pmesh.numVerts == pmesh.vertSel.GetSize());
	assert(pmesh.getNumEdges() == pmesh.edgeSel.GetSize());
	assert(pmesh.numPatches == pmesh.patchSel.GetSize());

	patchData->Apply(t,patchOb,selLevel);
	}

void EditPatchMod::NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc)
	{
	if ( mc->localData ) {
		EditPatchData *patchData = (EditPatchData*)mc->localData;
		if ( patchData ) {
			// The FALSE parameter indicates the the mesh cache itself is
			// invalid in addition to any other caches that depend on the
			// mesh cache.
			patchData->Invalidate(partID,FALSE);
			}
		}
	}

// Select a subcomponent within our object(s).  WARNING! Because the HitRecord list can
// indicate any of the objects contained within the group of patches being edited, we need
// to watch for control breaks in the patchData pointer within the HitRecord!

void EditPatchMod::SelectSubComponent( HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert )
	{
	// Don't do anything if at vertex level with verts turned off
	if(selLevel == EP_VERTEX && !filterVerts)
		return;

	if ( !ip ) return; 
	TimeValue t = ip->GetTime();

	ip->ClearCurNamedSelSet();

	// Keep processing hit records as long as we have them!
	while(hitRec) {	
		EditPatchData *patchData = (EditPatchData*)hitRec->modContext->localData;
	
		if ( !patchData )
			return;

		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			return;

		patchData->BeginEdit(t);
		if ( theHold.Holding() ) 
			theHold.Put(new PatchRestore(patchData,this,patch,"SelectSubComponent"));

		switch ( selLevel ) {
			case EP_VERTEX: {
				if ( all ) {				
					if ( invert ) {
						while( hitRec ) {
							// If the object changes, we're done!
							if(patchData != (EditPatchData*)hitRec->modContext->localData)
								goto vert_done;
							int index = ((PatchHitData *)(hitRec->hitData))->index;
							if(((PatchHitData *)(hitRec->hitData))->type == PATCH_HIT_VERTEX) {
								if(patch->vertSel[index])
									patch->vertSel.Clear(index);
								else
									patch->vertSel.Set(index);
								}
							hitRec = hitRec->Next();
							}
						}
					else
					if ( selected ) {
						while( hitRec ) {
							// If the object changes, we're done!
							if(patchData != (EditPatchData*)hitRec->modContext->localData)
								goto vert_done;
							PatchHitData *hit = (PatchHitData *)(hitRec->hitData);
							if(hit->type == PATCH_HIT_VERTEX)
								patch->vertSel.Set(hit->index);
							hitRec = hitRec->Next();
							}
						}
					else {
						while( hitRec ) {
							// If the object changes, we're done!
							if(patchData != (EditPatchData*)hitRec->modContext->localData)
								goto vert_done;
							PatchHitData *hit = (PatchHitData *)(hitRec->hitData);
							if(hit->type == PATCH_HIT_VERTEX)
								patch->vertSel.Clear(hit->index);
							hitRec = hitRec->Next();
							}
						}
					}
				else {
					int index = ((PatchHitData *)(hitRec->hitData))->index;
					if(((PatchHitData *)(hitRec->hitData))->type == PATCH_HIT_VERTEX) {
						if( invert ) {
							if(patch->vertSel[index])
								patch->vertSel.Clear(index);
							else
								patch->vertSel.Set(index);
							}
						else
						if ( selected )
							patch->vertSel.Set(index);
						else
							patch->vertSel.Clear(index);
						}
					hitRec = NULL;	// Reset it so we can exit	
					}
				vert_done:
				break;
				}
			case EP_EDGE: {
				if ( all ) {				
					if ( invert ) {
						while( hitRec ) {
							// If the object changes, we're done!
							if(patchData != (EditPatchData*)hitRec->modContext->localData)
								goto edge_done;
							int index = ((PatchHitData *)(hitRec->hitData))->index;
							if(patch->edgeSel[index])
								patch->edgeSel.Clear(index);
							else
								patch->edgeSel.Set(index);
							hitRec = hitRec->Next();
							}
						}
					else
					if ( selected ) {
						while( hitRec ) {
							// If the object changes, we're done!
							if(patchData != (EditPatchData*)hitRec->modContext->localData)
								goto edge_done;
							patch->edgeSel.Set(((PatchHitData *)(hitRec->hitData))->index);
							hitRec = hitRec->Next();
							}
						}
					else {
						while( hitRec ) {
							// If the object changes, we're done!
							if(patchData != (EditPatchData*)hitRec->modContext->localData)
								goto edge_done;
							patch->edgeSel.Clear(((PatchHitData *)(hitRec->hitData))->index);
							hitRec = hitRec->Next();
							}
						}
					}
				else {
					int index = ((PatchHitData *)(hitRec->hitData))->index;
					if( invert ) {
						if(patch->edgeSel[index])
							patch->edgeSel.Clear(index);
						else
							patch->edgeSel.Set(index);
						}
					else
					if ( selected ) {
						patch->edgeSel.Set(index);
						}
					else {
						patch->edgeSel.Clear(index);
						}
					hitRec = NULL;	// Reset it so we can exit	
					}
				edge_done:
				break;
				}
			case EP_PATCH: {
				if ( all ) {				
					if ( invert ) {
						while( hitRec ) {
							// If the object changes, we're done!
							if(patchData != (EditPatchData*)hitRec->modContext->localData)
								goto patch_done;
							int index = ((PatchHitData *)(hitRec->hitData))->index;
							if(patch->patchSel[index])
								patch->patchSel.Clear(index);
							else
								patch->patchSel.Set(index);
							hitRec = hitRec->Next();
							}
						}
					else
					if ( selected ) {
						while( hitRec ) {
							// If the object changes, we're done!
							if(patchData != (EditPatchData*)hitRec->modContext->localData)
								goto patch_done;
							patch->patchSel.Set(((PatchHitData *)(hitRec->hitData))->index);
							hitRec = hitRec->Next();
							}
						}
					else {
						while( hitRec ) {
							// If the object changes, we're done!
							if(patchData != (EditPatchData*)hitRec->modContext->localData)
								goto patch_done;
							patch->patchSel.Clear(((PatchHitData *)(hitRec->hitData))->index);
							hitRec = hitRec->Next();
							}
						}
					}
				else {
					int index = ((PatchHitData *)(hitRec->hitData))->index;
					if( invert ) {
						if(patch->patchSel[index])
							patch->patchSel.Clear(index);
						else
							patch->patchSel.Set(index);
						}
					else
					if ( selected ) {
						patch->patchSel.Set(index);
						}
					else {
						patch->patchSel.Clear(index);
						}
					hitRec = NULL;	// Reset it so we can exit	
					}
				patch_done:
				break;
				}
			case EP_OBJECT:
			default:
				return;
			}
		patchData->UpdateChanges(patch, FALSE);
		if ( patchData->tempData ) {
			patchData->tempData->Invalidate(PART_SELECT);
			}
		PatchSelChanged();
		}

	UpdateSelectDisplay();
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	}

void EditPatchMod::ClearSelection(int selLevel) 
	{
	// Don't do anything if at vertex level with verts turned off
	if(selLevel == EP_VERTEX && !filterVerts)
		return;
	if(selLevel == EP_OBJECT)
		return;

	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	ip->ClearCurNamedSelSet();
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;

		patchData->BeginEdit(ip->GetTime());
		if ( theHold.Holding() ) {
			theHold.Put(new PatchRestore(patchData,this,patch,"ClearSelection"));
			}

		switch ( selLevel ) {
			case EP_VERTEX: {
				patch->vertSel.ClearAll();
				break;
				}
			case EP_EDGE: {
				patch->edgeSel.ClearAll();
				break;
				}
			case EP_PATCH: {
				patch->patchSel.ClearAll();
				break;
				}
			}
		patchData->UpdateChanges(patch, FALSE);
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_SELECT);
			}
		PatchSelChanged();
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	}


void UnselectHiddenPatches(int level, PatchMesh *patch)
{
switch ( level ) {
	case EP_VERTEX: 
		{
		for (int i = 0; i < patch->numVerts; i++)
			{
			if (patch->getVert(i).IsHidden())
				patch->vertSel.Set(i,FALSE);
			}
		break;
		}
	case EP_EDGE: 
		{
		for (int i = 0; i < patch->numEdges; i++)
			{
			int a,b;
			a = patch->edges[i].v1;
			b = patch->edges[i].v2;
			if (patch->getVert(a).IsHidden() && patch->getVert(b).IsHidden())
				patch->edgeSel.Set(i,FALSE);
			}
		break;
		}
	case EP_PATCH: 
		{
		for (int i = 0; i < patch->numPatches; i++)
			{
			if (patch->patches[i].IsHidden())
				patch->patchSel.Set(i,FALSE);
			}
		break;
		}

	}


}

void EditPatchMod::SelectAll(int selLevel) 
	{
	// Don't do anything if at vertex level with verts turned off
	if(selLevel == EP_VERTEX && !filterVerts)
		return;
	if(selLevel == EP_OBJECT)
		return;

	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);
	ip->ClearCurNamedSelSet();

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;

		patchData->BeginEdit(ip->GetTime());
		if ( theHold.Holding() ) {
			theHold.Put(new PatchRestore(patchData,this,patch,"SelectAll"));
			}

		switch ( selLevel ) {
			case EP_VERTEX: {
				patch->vertSel.SetAll();
				break;
				}
			case EP_EDGE: {
				patch->edgeSel.SetAll();
				break;
				}
			case EP_PATCH: {
				patch->patchSel.SetAll();
				break;
				}
			}
		UnselectHiddenPatches(selLevel, patch);
		patchData->UpdateChanges(patch, FALSE);
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_SELECT);
			}
		PatchSelChanged();
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	}

void EditPatchMod::InvertSelection(int selLevel) 
	{
	// Don't do anything if at vertex level with verts turned off
	if(selLevel == EP_VERTEX && !filterVerts)
		return;
	if(selLevel == EP_OBJECT)
		return;

	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);
	ip->ClearCurNamedSelSet();

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;

		patchData->BeginEdit(ip->GetTime());
		if ( theHold.Holding() )
			theHold.Put(new PatchRestore(patchData,this,patch,"InvertSelection"));

		switch ( selLevel ) {
			case EP_VERTEX: {
				patch->vertSel = ~patch->vertSel;
				break;
				}
			case EP_EDGE: {
				patch->edgeSel = ~patch->edgeSel;
				break;
				}
			case EP_PATCH: {
				patch->patchSel = ~patch->patchSel;
				break;
				}
			}
		UnselectHiddenPatches(selLevel, patch);
		patchData->UpdateChanges(patch, FALSE);
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_SELECT);
			}
		PatchSelChanged();
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	}

void EditPatchMod::SetDisplaySurface(BOOL sw) {
	sw = TRUE;
	displaySurface = sw;
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		patchData->displaySurface = sw;

		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_DISPLAY);
			}
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

void EditPatchMod::SetDisplayLattice(BOOL sw) {
	displayLattice = sw;
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		patchData->displayLattice = sw;

		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;
		if(sw)
			patch->SetDispFlag(DISP_LATTICE);
		else
			patch->ClearDispFlag(DISP_LATTICE);
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_DISPLAY);
			}
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

void EditPatchMod::SetPropagate(BOOL sw) {
	propagate = sw;
	}

void EditPatchMod::SetMeshSteps(int steps) {
	meshSteps = steps;
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;

		patch->SetMeshSteps(steps);
		patchData->meshSteps = steps;
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_DISPLAY);
			}
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

//3-18-99 to suport render steps and removal of the mental tesselator
void EditPatchMod::SetMeshStepsRender(int steps) {
	meshStepsRender = steps;
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;

		patch->SetMeshStepsRender(steps);
		patchData->meshStepsRender = steps;
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_DISPLAY);
			}
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

void EditPatchMod::SetShowInterior(BOOL si) {
	showInterior = si;
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;

		patch->SetShowInterior(si);
		patchData->showInterior = si;
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_DISPLAY);
			}
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}


/*
// Future use (Not used now)
void EditPatchMod::SetMeshAdaptive(BOOL sw) {
	meshAdaptive = sw;
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		patch->SetAdaptive(sw);
		patchData->meshAdaptive = sw;
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_DISPLAY);
			}
		}
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}
*/



void EditPatchMod::SetViewTess(TessApprox &tess) {
	viewTess = tess;
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;

		patch->SetViewTess(tess);
		patchData->viewTess = tess;
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_DISPLAY);
			}
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

void EditPatchMod::SetProdTess(TessApprox &tess) {
	prodTess = tess;
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;

		patch->SetProdTess(tess);
		patchData->prodTess = tess;
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_DISPLAY);
			}
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

void EditPatchMod::SetDispTess(TessApprox &tess) {
	dispTess = tess;
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;

		patch->SetDispTess(tess);
		patchData->dispTess = tess;
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_DISPLAY);
			}
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

void EditPatchMod::SetViewTessNormals(BOOL use) {
	mViewTessNormals = use;
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;

		patch->SetViewTessNormals(use);
		patchData->mViewTessNormals = use;
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_DISPLAY);
			}
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

void EditPatchMod::SetProdTessNormals(BOOL use) {
	mProdTessNormals = use;
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;

		patch->SetProdTessNormals(use);
		patchData->mProdTessNormals = use;
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_DISPLAY);
			}
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

void EditPatchMod::SetViewTessWeld(BOOL weld) {
	mViewTessWeld = weld;
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;

		patch->SetViewTessWeld(weld);
		patchData->mViewTessWeld = weld;
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_DISPLAY);
			}
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

void EditPatchMod::SetProdTessWeld(BOOL weld) {
	mProdTessWeld = weld;
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;	
	
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;

		patch->SetProdTessWeld(weld);
		patchData->mProdTessWeld = weld;
		if ( patchData->tempData ) {
			patchData->TempData(this)->Invalidate(PART_DISPLAY);
			}
		}
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}


void EditPatchMod::ActivateSubobjSel(int level, XFormModes& modes )
	{	
	ModContextList mcList;
	INodeTab nodes;
	int old = selLevel;

	if ( !ip ) return;
	ip->GetModContexts(mcList,nodes);

	selLevel = level;
//3-10-99 watje
	if (level != EP_PATCH)
		{
		if (ip->GetCommandMode()==bevelMode)
			ip->SetStdCommandMode(CID_OBJMOVE);
		if (ip->GetCommandMode()==extrudeMode)
			ip->SetStdCommandMode(CID_OBJMOVE);
		if (inBevel)
			{
			ISpinnerControl *spin;
			spin = GetISpinner(GetDlgItem(hOpsPanel,IDC_EP_OUTLINESPINNER));
			if (spin) {
				HWND hWnd = spin->GetHwnd();
				SendMessage(hWnd,WM_LBUTTONUP,0,0);
				ReleaseISpinner(spin);
				}

			}
		if (inExtrude)
			{
			ISpinnerControl *spin;
			spin = GetISpinner(GetDlgItem(hOpsPanel,IDC_EP_EXTRUDESPINNER));
			if (spin) {
				HWND hWnd = spin->GetHwnd();
				SendMessage(hWnd,WM_LBUTTONUP,0,0);
				ReleaseISpinner(spin);
				}
			}
		}	
	if (level != EP_VERTEX)
		{
		if (ip->GetCommandMode()==bindMode)
			ip->SetStdCommandMode(CID_OBJMOVE);
		}


	switch ( level ) {
		case EP_OBJECT:
			// Not imp.
			break;

		case EP_PATCH:
			modes = XFormModes(moveMode,rotMode,nuscaleMode,uscaleMode,squashMode,selectMode);
			break;

		case EP_EDGE:
			modes = XFormModes(moveMode,rotMode,nuscaleMode,uscaleMode,squashMode,selectMode);
			break;

		case EP_VERTEX:

			modes = XFormModes(moveMode,rotMode,nuscaleMode,uscaleMode,squashMode,selectMode);
			break;
		}

	if ( selLevel != old ) {
		SetSubobjectLevel(level);

		// Modify the caches to reflect the new sel level.
		for ( int i = 0; i < mcList.Count(); i++ ) {
			EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
			if ( !patchData ) continue;		
		
			if ( patchData->tempData && patchData->TempData(this)->PatchCached(ip->GetTime()) ) {
				
				PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
				if(patch) {
					if(selLevel == EP_VERTEX)
						patch->dispFlags = DISP_VERTS;
					else
						patch->dispFlags = 0;
					if(displayLattice)
						patch->SetDispFlag(DISP_LATTICE);
					patch->SetDispFlag(patchLevelDispFlags[selLevel]);
					patch->selLevel = patchLevel[selLevel];
					}
				}
			}		

		NotifyDependents(FOREVER, PART_SUBSEL_TYPE|PART_DISPLAY,REFMSG_CHANGE);
		ip->PipeSelLevelChanged();
		// Update selection UI display, named sel
		SelectionChanged();
		}
	
	nodes.DisposeTemporary();
	}


int EditPatchMod::SubObjectIndex(HitRecord *hitRec)
	{	
	EditPatchData *patchData = (EditPatchData*)hitRec->modContext->localData;
	if ( !patchData ) return 0;
	if ( !ip ) return 0;
	TimeValue t = ip->GetTime();
	PatchHitData *hit = (PatchHitData *)(hitRec->hitData);
	switch ( selLevel ) {
		case EP_VERTEX: {
			if(hit->type != PATCH_HIT_VERTEX)
				return 0;
			int hitIndex = hit->index;
			return hitIndex;
			}
		case EP_EDGE: {
			int hitIndex = hit->index;
			return hitIndex;
			}
		case EP_PATCH: {
			int hitIndex = hit->index;
			return hitIndex;
			}
		default:
			return 0;
		}
	}

void EditPatchMod::GetSubObjectTMs(
		SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc)
	{
	Interval valid;
	if ( mc->localData ) {
		EditPatchData *patchData = (EditPatchData*)mc->localData;
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		// Watch out -- The system can call us even if we didn't get a valid patch object
		if(!patch)
			return;

		switch ( selLevel ) {
			case EP_VERTEX: {
				Matrix3 otm = node->GetObjectTM(t,&valid);
				Matrix3 tm = node->GetNodeTM(t,&valid);
				BitArray sel = patch->VertexTempSel();
				int count = sel.GetSize();
				for(int i = 0; i < count; ++i) {
					if(sel[i]) {
						tm.SetTrans(patch->verts[i].p * otm);
						cb->TM(tm, i);
						}
					}
				break;
				}
			case EP_EDGE:
			case EP_PATCH: {
 				Matrix3 otm = node->GetObjectTM(t,&valid);
				Matrix3 tm = node->GetNodeTM(t,&valid);
				Box3 box;
				BitArray sel = patch->VertexTempSel();
				int count = sel.GetSize();
				for ( int i = 0; i < count; i++ ) {
					if ( sel[i] )
						box += patch->verts[i].p;
					}
				tm.SetTrans(otm * box.Center());
				cb->TM(tm, 0);
				break;
				}
			}
		}
	}

void EditPatchMod::GetSubObjectCenters(
		SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc)
	{
	Interval valid;
	Matrix3 tm = node->GetObjectTM(t,&valid);	
	
	assert(ip);
	if ( mc->localData ) {	
		EditPatchData *patchData = (EditPatchData*)mc->localData;		
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		// Watch out -- The system can call us even if we didn't get a valid patch object
		if(!patch)
			return;

		switch ( selLevel ) {
			case EP_VERTEX: {
				Box3 box;
				BitArray sel = patch->VertexTempSel();
				int verts = patch->numVerts;
				for ( int i = 0; i < verts; i++ ) {
					if ( sel[i] )
						cb->Center(patch->verts[i].p * tm, i);
					}
				break;
				}
			case EP_EDGE:
			case EP_PATCH: {
				Box3 box;
				BOOL bHasSel = FALSE;
				BitArray sel = patch->VertexTempSel();
				int verts = patch->numVerts;
				for ( int i = 0; i < verts; i++ ) {
					if ( sel[i] ) {
						box += patch->verts[i].p * tm;
						bHasSel = TRUE;
						}
					}
				if (bHasSel)
					cb->Center(box.Center(), 0);
				break;
				}
			default:
				cb->Center(tm.GetTrans(), 0);
				break;
			}		
		}
	}

BOOL EditPatchMod::DependOnTopology(ModContext &mc)
	{
	EditPatchData *patchData = (EditPatchData*)mc.localData;
	if (patchData) {
		if (patchData->GetFlag(EPD_HASDATA)) {
			return TRUE;
			}
		}
	return FALSE;
	}

void EditPatchMod::DeletePatchDataTempData()
	{
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;		
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;				
		if ( patchData->tempData ) {
			delete patchData->tempData;
			}
		patchData->tempData = NULL;
		}
	nodes.DisposeTemporary();
	}


void EditPatchMod::CreatePatchDataTempData()
	{
	ModContextList mcList;
	INodeTab nodes;

	if ( !ip ) return;		
	ip->GetModContexts(mcList,nodes);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;				
		if ( !patchData->tempData ) {
			patchData->tempData = new EPTempData(this,patchData);
			}		
		}
	nodes.DisposeTemporary();
	}

//--------------------------------------------------------------

int EditPatchMod::RememberPatchThere(HWND hWnd, IPoint2 m) {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();

	// Initialize so there isn't any remembered patch
	rememberedPatch = NULL;

	if ( !ip ) return 0;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	// See if we're over a patch
	ViewExp *vpt = ip->GetViewport(hWnd);
	GraphicsWindow *gw = vpt->getGW();
	HitRegion hr;
	MakeHitRegion(hr, HITTYPE_POINT, 1, 4, &m);
	gw->setHitRegion(&hr);
	SubPatchHitList hitList;

	int result = 0;

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
		INode *inode = nodes[i];
		Matrix3 mat = inode->GetObjectTM(t);
		gw->setTransform(mat);	
		patch->SubObjectHitTest(gw, gw->getMaterial(), &hr, SUBHIT_PATCH_PATCHES/* | HIT_ABORTONHIT*/, hitList );
		PatchSubHitRec *hit = hitList.First();
		if(hit) {
			result = 1;
			// Go thru the list and see if we have one that's selected
			// If more than one selected and they're different types, set unknown type
			hit = hitList.First();
			while(hit) {
				if(patch->patchSel[hit->index]) {
					if(patch->SelPatchesSameType()) {
						rememberedPatch = NULL;
						rememberedData = patch->patches[hit->index].flags & (~PATCH_INTERIOR_MASK);
						goto finish;
						}
					// Selected patches not all the same type!
					rememberedPatch = NULL;
					rememberedData = -1;	// Not all the same!
					goto finish;
					}
				hit = hit->Next();
				}
			if(ip->SelectionFrozen())
				goto finish;
			// Select just this patch
			hit = hitList.First();
			theHold.Begin();
			if ( theHold.Holding() )
				theHold.Put(new PatchSelRestore(patchData,this,patch));
			patch->patchSel.ClearAll();
			patch->patchSel.Set(hit->index);
			patchData->UpdateChanges(patch, FALSE);
			theHold.Accept(GetString(IDS_DS_SELECT));
			NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
			ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
			PatchSelChanged();

			rememberedPatch = patch;
			rememberedIndex = hit->index;
			rememberedData = patch->patches[rememberedIndex].flags & (~PATCH_INTERIOR_MASK);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
finish:
	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	if ( vpt ) ip->ReleaseViewport(vpt);
	return result;
	}

void EditPatchMod::ChangeRememberedPatch(int type) {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
		if(patch == rememberedPatch) {
			// If this is the first edit, then the delta arrays will be allocated
			patchData->BeginEdit(t);

			theHold.Begin();
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch,"ChangeRememberedPatch"));
			// Call the patch type change function
			ChangePatchType(patch, rememberedIndex, type);
			patchData->UpdateChanges(patch, FALSE);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			theHold.Accept(GetString(IDS_TH_PATCHCHANGE));
			ClearPatchDataFlag(mcList,EPD_BEENDONE);
			NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
			ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
			nodes.DisposeTemporary();
			return;
 			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	}

void EditPatchMod::ChangeSelPatches(int type) {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	BOOL holdNeeded = FALSE;
	BOOL hadSelected = FALSE;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		BOOL altered = FALSE;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;		
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->patchSel.NumberSet()) {
			altered = holdNeeded = TRUE;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch,"ChangeSelPatches"));
			// Call the vertex type change function
			ChangePatchType(patch, -1, type);
			patchData->UpdateChanges(patch, FALSE);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded)
		theHold.Accept(GetString(IDS_TH_PATCHCHANGE));
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}


void EditPatchMod::SetRememberedPatchType(int type) {
	if(rememberedPatch)
		ChangeRememberedPatch(type);
	else
		ChangeSelPatches(type);
	}

//--------------------------------------------------------------

int EditPatchMod::RememberVertThere(HWND hWnd, IPoint2 m) {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();

	// Initialize so there isn't any remembered patch
	rememberedPatch = NULL;

	if ( !ip ) return 0;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	// See if we're over a vertex
	ViewExp *vpt = ip->GetViewport(hWnd);
	GraphicsWindow *gw = vpt->getGW();
	HitRegion hr;
	MakeHitRegion(hr, HITTYPE_POINT, 1, 4, &m);
	gw->setHitRegion(&hr);
	SubPatchHitList hitList;

	int result = 0;
	
	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
		INode *inode = nodes[i];
		Matrix3 mat = inode->GetObjectTM(t);
		gw->setTransform(mat);	
		patch->SubObjectHitTest(gw, gw->getMaterial(), &hr, SUBHIT_PATCH_VERTS/* | HIT_ABORTONHIT*/, hitList );
		PatchSubHitRec *hit = hitList.First();
		if(hit) {
			result = 1;
			// Go thru the list and see if we have one that's selected
			// If more than one selected and they're different types, set unknown type
			hit = hitList.First();
			while(hit) {
				if(patch->vertSel[hit->index]) {
					if(patch->SelVertsSameType()) {
						rememberedPatch = NULL;
						rememberedData = patch->verts[hit->index].flags & (~PVERT_TYPE_MASK);
						goto finish;
						}
					// Selected verts not all the same type!
					rememberedPatch = NULL;
					rememberedData = -1;	// Not all the same!
					goto finish;
					}
				hit = hit->Next();
				}
			if(ip->SelectionFrozen())
				goto finish;
			// Select just this vertex
			hit = hitList.First();
			theHold.Begin();
			if ( theHold.Holding() )
				theHold.Put(new PatchSelRestore(patchData,this,patch));
			patch->vertSel.ClearAll();
			patch->vertSel.Set(hit->index);
			patchData->UpdateChanges(patch, FALSE);
			theHold.Accept(GetString(IDS_DS_SELECT));
			NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
			ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
			PatchSelChanged();

			rememberedPatch = patch;
			rememberedIndex = hit->index;
			rememberedData = patch->verts[rememberedIndex].flags & (~PVERT_TYPE_MASK);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
finish:
	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	if ( vpt ) ip->ReleaseViewport(vpt);
	return result;
	}

void EditPatchMod::ChangeRememberedVert(int type) {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
		if(patch == rememberedPatch) {
			// If this is the first edit, then the delta arrays will be allocated
			patchData->BeginEdit(t);

			theHold.Begin();
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch,"ChangeRememberedVert"));
			// Call the vertex type change function
			patch->ChangeVertType(rememberedIndex, type);
			patchData->UpdateChanges(patch, FALSE);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			theHold.Accept(GetString(IDS_TH_VERTCHANGE));
			ClearPatchDataFlag(mcList,EPD_BEENDONE);
			NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
			ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
			nodes.DisposeTemporary();
			return;
 			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	}

void EditPatchMod::ChangeSelVerts(int type) {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	BOOL holdNeeded = FALSE;
	BOOL hadSelected = FALSE;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		BOOL altered = FALSE;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
					
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->vertSel.NumberSet()) {
			altered = holdNeeded = TRUE;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch,"ChangeSelVerts"));
			// Call the vertex type change function
			patch->ChangeVertType(-1, type);
			patchData->UpdateChanges(patch, FALSE);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded)
		theHold.Accept(GetString(IDS_TH_VERTCHANGE));
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOVERTSSEL),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}


void EditPatchMod::SetRememberedVertType(int type) {
	if(rememberedPatch)
		ChangeRememberedVert(type);
	else
		ChangeSelVerts(type);
	}

//--------------------------------------------------------------
int EditPatchMod::HitTest(TimeValue t, INode* inode, int type, int crossing, 
		int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc) 
	{
	Interval valid;
	int savedLimits,res = 0;
	GraphicsWindow *gw = vpt->getGW();
	HitRegion hr;
	MakeHitRegion(hr,type, crossing,4,p);
	gw->setHitRegion(&hr);
	Matrix3 mat = inode->GetObjectTM(t);
	gw->setTransform(mat);	
	gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);
	gw->clearHitCode();
	
	if ( mc->localData ) {		
		EditPatchData *patchData = (EditPatchData*)mc->localData;
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			return FALSE;

		SubPatchHitList hitList;
		PatchSubHitRec *rec;
		res = patch->SubObjectHitTest( gw, gw->getMaterial(), &hr,
			flags|((patchHitOverride) ? patchHitLevel[patchHitOverride] : patchHitLevel[selLevel]), hitList );
	
		rec = hitList.First();
		while( rec ) {
			vpt->LogHit(inode,mc,rec->dist,123456,new PatchHitData(rec->patch, rec->index, rec->type));
			rec = rec->Next();
			}
		}

	gw->setRndLimits(savedLimits);	
	return res;
	}

int EditPatchMod::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags, ModContext *mc) {	
	return 0;	
	}

void EditPatchMod::GetWorldBoundBox(TimeValue t,INode* inode, ViewExp *vpt, Box3& box, ModContext *mc) {
	box.Init();
	}



//---------------------------------------------------------------------
// UI stuff

void EditPatchMod::RecordTopologyTags() {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;
		
		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
		patch->RecordTopologyTags();
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	}

class ChangeNamedSetRestore : public RestoreObj {
	public:
		BitArray oldset,newset;
		int index;
		GenericNamedSelSetList *setList;

		ChangeNamedSetRestore(GenericNamedSelSetList *sl,int ix,BitArray *o) {
			setList = sl; index = ix; oldset = *o;
			}   		
		void Restore(int isUndo) {
			newset = *(setList->sets[index]);
			*(setList->sets[index]) = oldset;
			}
		void Redo() {
			*(setList->sets[index]) = newset;
			}
				
		TSTR Description() {return TSTR(_T("Change Named Sel Set"));}
	};

// Selection set, misc fixup utility function
// This depends on PatchMesh::RecordTopologyTags being called prior to the topo changes
void EditPatchMod::ResolveTopoChanges() {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	for ( int i = 0; i < mcList.Count(); i++ ) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;
		
		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
		// First, the vertex selections
		for(int set = 0; set < patchData->vselSet.Count(); ++set) {
			BitArray *oldVS = &patchData->vselSet[set];
			BitArray newVS;
			newVS.SetSize(patch->numVerts);
			for(int vert = 0; vert < patch->numVerts; ++vert) {
				// Get the knot's previous location, then copy that selection into the new set
				int tag = patch->verts[vert].aux1;
				if(tag >= 0)
					newVS.Set(vert, (*oldVS)[tag]);
				else
					newVS.Clear(vert);
				}
			if(theHold.Holding())
				theHold.Put(new ChangeNamedSetRestore(&patchData->vselSet, set, oldVS));
			patchData->vselSet[set] = newVS;
			}
		// Now the edge selections
		for(set = 0; set < patchData->eselSet.Count(); ++set) {
			BitArray *oldES = &patchData->eselSet[set];
			BitArray newES;
			newES.SetSize(patch->numEdges);
			for(int edge = 0; edge < patch->numEdges; ++edge) {
				// Get the knot's previous location, then copy that selection into the new set
				int tag = patch->edges[edge].aux1;
				if(tag >= 0)
					newES.Set(edge, (*oldES)[tag]);
				else
					newES.Clear(edge);
				}
			if(theHold.Holding())
				theHold.Put(new ChangeNamedSetRestore(&patchData->eselSet, set, oldES));
			patchData->eselSet[set] = newES;
			}
		// Now the patch selections
		for(set = 0; set < patchData->pselSet.Count(); ++set) {
			BitArray *oldPS = &patchData->pselSet[set];
			BitArray newPS;
			newPS.SetSize(patch->numPatches);
			for(int p = 0; p < patch->numPatches; ++p) {
				// Get the knot's previous location, then copy that selection into the new set
				int tag = patch->patches[p].aux1;
				if(tag >= 0)
					newPS.Set(p, (*oldPS)[tag]);
				else
					newPS.Clear(p);
				}
			if(theHold.Holding())
				theHold.Put(new ChangeNamedSetRestore(&patchData->pselSet, set, oldPS));
			patchData->pselSet[set] = newPS;
			}

//watje 4-16-99
		patch->HookFixTopology();
//watje 8-23-99 fiexs collapse crash
		patchData->finalPatch.hooks = patch->hooks;

		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	}

class EPModContextEnumProc : public ModContextEnumProc {
	float f;
	public:
		EPModContextEnumProc(float f) { this->f = f; }
		BOOL proc(ModContext *mc);  // Return FALSE to stop, TRUE to continue.
	};

BOOL EPModContextEnumProc::proc(ModContext *mc) {
	EditPatchData *patchData = (EditPatchData*)mc->localData;
	if ( patchData )		
		patchData->RescaleWorldUnits(f);
	return TRUE;
	}

// World scaling
void EditPatchMod::RescaleWorldUnits(float f) {
	if (TestAFlag(A_WORK1))
		return;
	SetAFlag(A_WORK1);
	
	// rescale all our references
	for (int i=0; i<NumRefs(); i++) {
		ReferenceMaker *srm = GetReference(i);
		if (srm) 
			srm->RescaleWorldUnits(f);
		}
	
	// Now rescale stuff inside our data structures
	EPModContextEnumProc proc(f);
	EnumModContexts(&proc);
	NotifyDependents(FOREVER, PART_GEOM, REFMSG_CHANGE);
	}

int EditPatchMod::GetSubobjectLevel()
	{
	return selLevel;
	}

void EditPatchMod::SetSubobjectLevel(int level)
	{
	selLevel = level;
	if(hSelectPanel)
		RefreshSelType();
	// Setup named selection sets	
	SetupNamedSelDropDown();
	}

static int butIDs[] = { 0, EP_VERTEX, EP_EDGE, EP_PATCH };

void EditPatchMod::RefreshSelType () {
	if(!hSelectPanel)
		return;
	if(hOpsPanel) {
		// Set up or remove the surface properties rollup if needed
		if(hSurfPanel) {
			rsSurf = IsRollupPanelOpen (hSurfPanel);
			ip->DeleteRollupPage(hSurfPanel);
			hSurfPanel = NULL;
			}
/* watje 3-18-99
		if(selLevel == EP_OBJECT) {
			hSurfPanel = ip->AddRollupPage (hInstance, MAKEINTRESOURCE(IDD_EDPATCH_SURF_OBJ),
				PatchObjSurfDlgProc, GetString (IDS_TH_SURFACEPROPERTIES), (LPARAM) this, rsSurf ? 0 : APPENDROLL_CLOSED);
			}
		else
*/
		if(selLevel == EP_PATCH) {
			hSurfPanel = ip->AddRollupPage (hInstance, MAKEINTRESOURCE(IDD_EDPATCH_SURF),
				PatchSurfDlgProc, GetString (IDS_TH_SURFACEPROPERTIES), (LPARAM) this, rsSurf ? 0 : APPENDROLL_CLOSED);
			}
		SetSurfDlgEnables();
		}

	ICustToolbar *iToolbar = GetICustToolbar(GetDlgItem(hSelectPanel,IDC_SELTYPE));
	ICustButton *but;
	for (int i=1; i<4; i++) {
		but = iToolbar->GetICustButton (butIDs[i]);
		but->SetCheck (GetSubobjectLevel()==i);
		ReleaseICustButton (but);
	}
	ReleaseICustToolbar(iToolbar);
	SetSelDlgEnables();
	SetOpsDlgEnables();
	UpdateSelectDisplay();
}

void EditPatchMod::SelectionChanged() {
	if (hSelectPanel) {
		UpdateSelectDisplay();
		InvalidateRect(hSelectPanel,NULL,FALSE);
		}
	// Now see if the selection set matches one of the named selections!
	if(ip && (selLevel != EP_OBJECT)) {
		ModContextList mcList;		
		INodeTab nodes;
		TimeValue t = ip->GetTime();
		ip->GetModContexts(mcList,nodes);
		int sublevel = selLevel - 1;
		int dataSet;
		for(int set = 0; set < namedSel[sublevel].Count(); ++set) {
			ClearPatchDataFlag(mcList,EPD_BEENDONE);
			BOOL gotMatch = FALSE;
			for ( int i = 0; i < mcList.Count(); i++ ) {
				EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
				if ( !patchData ) continue;
				if ( patchData->GetFlag(EPD_BEENDONE) ) continue;
				PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
				if(!patch) continue;
				// See if this patch has the named selection set
				switch(selLevel) {
					case EP_VERTEX: 
						for(dataSet = 0; dataSet < patchData->vselSet.Count(); ++dataSet) {
							if(*(patchData->vselSet.names[dataSet]) == *namedSel[sublevel][set]) {
								if(!(*patchData->vselSet.sets[set] == patch->vertSel))
									goto next_set;
								gotMatch = TRUE;
								break;
								}
							}
						break;
					case EP_EDGE:
						for(dataSet = 0; dataSet < patchData->eselSet.Count(); ++dataSet) {
							if(*(patchData->eselSet.names[dataSet]) == *namedSel[sublevel][set]) {
								if(!(*patchData->eselSet.sets[set] == patch->edgeSel))
									goto next_set;
								gotMatch = TRUE;
								break;
								}
							}
						break;
					case EP_PATCH:
						for(dataSet = 0; dataSet < patchData->pselSet.Count(); ++dataSet) {
							if(*(patchData->pselSet.names[dataSet]) == *namedSel[sublevel][set]) {
								if(!(*patchData->pselSet.sets[set] == patch->patchSel))
									goto next_set;
								gotMatch = TRUE;
								break;
								}
							}
						break;
					}
				patchData->SetFlag(EPD_BEENDONE,TRUE);
				}
			// If we reach here, we might have a set that matches
			if(gotMatch) {
				ip->SetCurNamedSelSet(*namedSel[sublevel][set]);
				goto namedSelUpdated;
				}
next_set:;
			}
		// No set matches, clear the named selection
		ip->ClearCurNamedSelSet();
				

namedSelUpdated:
		nodes.DisposeTemporary();
		ClearPatchDataFlag(mcList,EPD_BEENDONE);
		}
	}

void EditPatchMod::InvalidateSurfaceUI() {
	if(hSurfPanel && selLevel == EP_PATCH) {
		InvalidateRect (hSurfPanel, NULL, FALSE);
		patchUIValid = FALSE;
		}
	}

BitArray *EditPatchMod::GetLevelSelectionSet(PatchMesh *patch) {
	switch(selLevel) {
		case EP_VERTEX:
			return &patch->vertSel;

		case EP_PATCH:
			return &patch->patchSel;

		case EP_EDGE:
			return &patch->edgeSel;
		}
	assert(0);
	return NULL;
	}

void EditPatchMod::UpdateSelectDisplay() {	
	TSTR buf;
	int num, j;

	if (!hSelectPanel) return;

	ModContextList mcList;
	INodeTab nodes;
	if ( !ip )
		return;
	ip->GetModContexts(mcList,nodes);

	switch (GetSubobjectLevel()) {
		case EP_OBJECT:
			buf.printf (GetString (IDS_TH_OBJECT_SEL));
			break;

		case EP_VERTEX: {
			num = 0;
			PatchMesh *thePatch = NULL;
			for ( int i = 0; i < mcList.Count(); i++ ) {
				EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
				if ( !patchData ) continue;		
			
				if ( patchData->tempData && patchData->TempData(this)->PatchCached(ip->GetTime()) ) {
					PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
					if(!patch) continue;
					int thisNum = patch->vertSel.NumberSet();
					if(thisNum) {
						num += thisNum;
						thePatch = patch;
						}
					}
				}
			if (num==1) {
				for (j=0; j<thePatch->vertSel.GetSize(); j++)
					if (thePatch->vertSel[j]) break;
				buf.printf (GetString(IDS_TH_NUMVERTSEL), j+1);
				}
			else
				buf.printf (GetString(IDS_TH_NUMVERTSELP), num);
			}
			break;

		case EP_PATCH: {
			num = 0;
			PatchMesh *thePatch = NULL;
			for ( int i = 0; i < mcList.Count(); i++ ) {
				EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
				if ( !patchData ) continue;		
			
				if ( patchData->tempData && patchData->TempData(this)->PatchCached(ip->GetTime()) ) {
					PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
					if(!patch) continue;
					int thisNum = patch->patchSel.NumberSet();
					if(thisNum) {
						num += thisNum;
						thePatch = patch;
						}
					}
				}
			if (num==1) {
				for (j=0; j<thePatch->patchSel.GetSize(); j++)
					if (thePatch->patchSel[j]) break;
				buf.printf (GetString(IDS_TH_NUMPATCHSEL), j+1);
				}
			else
				buf.printf(GetString(IDS_TH_NUMPATCHSELP),num);
			}
			break;

		case EP_EDGE: {
			num = 0;
			PatchMesh *thePatch = NULL;
			for ( int i = 0; i < mcList.Count(); i++ ) {
				EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
				if ( !patchData ) continue;		
			
				if ( patchData->tempData && patchData->TempData(this)->PatchCached(ip->GetTime()) ) {
					PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
					if(!patch) continue;
					int thisNum = patch->edgeSel.NumberSet();
					if(thisNum) {
						num += thisNum;
						thePatch = patch;
						}
					}
				}
			if (num==1) {
				for (j=0; j<thePatch->edgeSel.GetSize(); j++)
					if (thePatch->edgeSel[j]) break;
				buf.printf (GetString(IDS_TH_NUMEDGESEL), j+1);
				}
			else
				buf.printf(GetString(IDS_TH_NUMEDGESELP),num);
			}
			break;
		}

	nodes.DisposeTemporary();
	SetDlgItemText(hSelectPanel, IDC_NUMSEL_LABEL, buf);
	}

void EditPatchMod::BeginEditParams( IObjParam *ip, ULONG flags, Animatable *prev )
	{
	this->ip = ip;

	patchUIValid = FALSE;
	CreatePatchDataTempData();

	hSelectPanel = ip->AddRollupPage (hInstance, MAKEINTRESOURCE(IDD_EDPATCH_SELECT),
		PatchSelectDlgProc, GetString(IDS_TH_SELECTION), (LPARAM)this, rsSel ? 0 : APPENDROLL_CLOSED);
	hOpsPanel = ip->AddRollupPage (hInstance, MAKEINTRESOURCE(IDD_EDPATCH_OPS),
		PatchOpsDlgProc, GetString (IDS_TH_GEOMETRY), (LPARAM) this, rsOps ? 0 : APPENDROLL_CLOSED);
/* watje 3-18-99
	if(selLevel == EP_OBJECT) {
		hSurfPanel = ip->AddRollupPage (hInstance, MAKEINTRESOURCE(IDD_EDPATCH_SURF_OBJ),
			PatchObjSurfDlgProc, GetString (IDS_TH_SURFACEPROPERTIES), (LPARAM) this, rsSurf ? 0 : APPENDROLL_CLOSED);
		}
	else
*/
	if(selLevel == EP_PATCH) {
		hSurfPanel = ip->AddRollupPage (hInstance, MAKEINTRESOURCE(IDD_EDPATCH_SURF),
			PatchSurfDlgProc, GetString (IDS_TH_SURFACEPROPERTIES), (LPARAM) this, rsSurf ? 0 : APPENDROLL_CLOSED);
		}
	else
		hSurfPanel = NULL;

	// Create sub object editing modes.
	moveMode        = new MoveModBoxCMode(this,ip);
	rotMode         = new RotateModBoxCMode(this,ip);
	uscaleMode      = new UScaleModBoxCMode(this,ip);
	nuscaleMode     = new NUScaleModBoxCMode(this,ip);
	squashMode      = new SquashModBoxCMode(this,ip);
	selectMode      = new SelectModBoxCMode(this,ip);
	extrudeMode    = new EPM_ExtrudeCMode(this,ip);
	bevelMode    = new EPM_BevelCMode(this,ip);
	bindMode    = new EPM_BindCMode(this,ip);


	// Add our sub object type
	TSTR type1( GetString(IDS_TH_VERTEX) );
	TSTR type2( GetString(IDS_TH_EDGE) );
	TSTR type3( GetString(IDS_TH_PATCH) );
	const TCHAR *ptype[] = { type1, type2, type3 };
	ip->RegisterSubObjectTypes( ptype, 3 );

	// Restore the selection level.
	ip->SetSubObjectLevel(selLevel);
	
	// Disable show end result.
	ip->EnableShowEndResult(FALSE);

	// Setup named selection sets	
	SetupNamedSelDropDown();

	// Update selection UI display
	SelectionChanged();

	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
	SetAFlag(A_MOD_BEING_EDITED);	
	}
		
void EditPatchMod::EndEditParams( IObjParam *ip, ULONG flags, Animatable *next )
	{
	if (hSelectPanel) {
		rsSel = IsRollupPanelOpen (hSelectPanel);
		ip->DeleteRollupPage(hSelectPanel);
		hSelectPanel = NULL;
		}
	if (hOpsPanel) {
		rsOps = IsRollupPanelOpen (hOpsPanel);
		ip->DeleteRollupPage(hOpsPanel);
		hOpsPanel = NULL;
		}
	if (hSurfPanel) {
		rsSurf = IsRollupPanelOpen (hSurfPanel);
		ip->DeleteRollupPage(hSurfPanel);
		hSurfPanel = NULL;
		}

	// Enable show end result
	ip->EnableShowEndResult(TRUE);

	CancelEditPatchModes(ip);

	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_END_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF);
	ClearAFlag(A_MOD_BEING_EDITED);
	
	DeletePatchDataTempData();
	this->ip = NULL;

//	if ( ip->GetCommandMode()->ID() == CID_EP_EXTRUDE ) ip->SetStdCommandMode( CID_OBJMOVE );
//	if ( ip->GetCommandMode()->ID() == CID_EP_BEVEL ) ip->SetStdCommandMode( CID_OBJMOVE );

	
	ip->DeleteMode(moveMode);
	ip->DeleteMode(rotMode);
	ip->DeleteMode(uscaleMode);
	ip->DeleteMode(nuscaleMode);
	ip->DeleteMode(squashMode);
	ip->DeleteMode(selectMode);
	ip->DeleteMode(extrudeMode);
	ip->DeleteMode(bevelMode);
	ip->DeleteMode(bindMode);

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

	if( extrudeMode ) delete extrudeMode;
	extrudeMode = NULL;

	if( bevelMode ) delete bevelMode;
	bevelMode = NULL;
	if( bindMode ) delete bindMode;
	bindMode = NULL;
	}

void EditPatchMod::DoDeleteSelected() {
	switch(GetSubobjectLevel()) {
		case EP_VERTEX:
			DoVertDelete();
			break;
		case EP_EDGE:
			DoEdgeDelete();
			break;
		case EP_PATCH:
			DoPatchDelete();
			break;
		}
	}

// Vertex Delete modifier method
void EditPatchMod::DoVertDelete() {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	int holdNeeded = 0;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		int altered = 0;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
		patchData->RecordTopologyTags(patch);

		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->vertSel.NumberSet()) {
			altered = holdNeeded = 1;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch,"DoVertDelete"));
			// Call the vertex delete function
			DeleteSelVerts(patch);
			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_VERTDELETE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOVERTSSEL),PROMPT_TIME);
		theHold.End();
		}
	
	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

// Edger Delete modifier method
void EditPatchMod::DoEdgeDelete() {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	int holdNeeded = 0;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		int altered = 0;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
		patchData->RecordTopologyTags(patch);

		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->edgeSel.NumberSet()) {
			altered = holdNeeded = 1;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch,"DoEdgeDelete"));
			int edges = patch->getNumEdges();
			int patches = patch->getNumPatches();
			int verts = patch->getNumVerts();

			// Tag the patches that are attached to selected edges
			BitArray delPatches(patches);
			delPatches.ClearAll();

			for(int i = 0; i < edges; ++i) {
				if(patch->edgeSel[i]) {
					if(patch->edges[i].patch1 >= 0)
						delPatches.Set(patch->edges[i].patch1);
					if(patch->edges[i].patch2 >= 0)
						delPatches.Set(patch->edges[i].patch2);
					}
				}

			BitArray delVerts(verts);
			delVerts.ClearAll();

			DeletePatchParts(patch, delVerts, delPatches);
			patch->computeInteriors();

			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_EDGEDELETE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOEDGESSEL),PROMPT_TIME);
		theHold.End();
		}
	
	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

void EditPatchMod::DoPatchAdd(int type) {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	int holdNeeded = 0;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		int altered = 0;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
		patchData->RecordTopologyTags(patch);

		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->edgeSel.NumberSet()) {
			altered = holdNeeded = 1;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch,"DoPatchAdd"));
			// Call the patch add function
			AddPatches(type, patch, TRUE);
			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_PATCHADD));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOVALIDEDGESSEL),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

void EditPatchMod::DoPatchDelete() {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	int holdNeeded = 0;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		int altered = 0;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
		patchData->RecordTopologyTags(patch);
					
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->patchSel.NumberSet()) {
			altered = holdNeeded = 1;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch,"DoPatchDelete"));
			// Call the patch delete function
			DeleteSelPatches(patch);
			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_PATCHDELETE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

//watje 12-10-98
void EditPatchMod::DoUnHide() {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	BOOL holdNeeded = FALSE;
	BOOL hadSelected = FALSE;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		BOOL altered = FALSE;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;		
		patchData->RecordTopologyTags(patch);
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
//		if(patch->patchSel.NumberSet()) {
		if(1) {
			altered = holdNeeded = TRUE;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch));
			// Call the vertex type change function
			UnHidePatches(patch);
			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_PATCHCHANGE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

void EditPatchMod::DoHide(int type) {
	switch(type) {
		case EP_VERTEX:
			DoVertHide();
			break;
		case EP_EDGE:
			DoEdgeHide();
			break;
		case EP_PATCH:
			DoPatchHide();
			break;
		}
	}

void EditPatchMod::DoPatchHide() 
	{
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	BOOL holdNeeded = FALSE;
	BOOL hadSelected = FALSE;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		BOOL altered = FALSE;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;		
		patchData->RecordTopologyTags(patch);
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->patchSel.NumberSet()) {
			altered = holdNeeded = TRUE;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch));
			// Call the vertex type change function
			HidePatches(patch);
			patch->patchSel.ClearAll();
			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_PATCHCHANGE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);

	}

void EditPatchMod::DoVertHide() 
	{
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	BOOL holdNeeded = FALSE;
	BOOL hadSelected = FALSE;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		BOOL altered = FALSE;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;		
		patchData->RecordTopologyTags(patch);
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->vertSel.NumberSet()) {
			altered = holdNeeded = TRUE;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch));
			// Call the vertex type change function
			HideVerts(patch);
			patch->vertSel.ClearAll();
			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_PATCHCHANGE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);

	}

void EditPatchMod::DoEdgeHide() 
	{
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	BOOL holdNeeded = FALSE;
	BOOL hadSelected = FALSE;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		BOOL altered = FALSE;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;		
		patchData->RecordTopologyTags(patch);
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->edgeSel.NumberSet()) {
			altered = holdNeeded = TRUE;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch));
			// Call the vertex type change function
			HideEdges(patch);
			patch->edgeSel.ClearAll();
			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_PATCHCHANGE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);

	}


void EditPatchMod::DoAddHook(PatchMesh *pMesh, int vert, int seg) {


	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	BOOL holdNeeded = FALSE;
	BOOL hadSelected = FALSE;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);


	if (mcList.Count() != 1) return;

	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
//	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		BOOL altered = FALSE;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if ((!patch) || (patch != pMesh))
			continue;		
//		patchData->RecordTopologyTags(patch);
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
//		if(patch->vertSel.NumberSet()) {

		altered = holdNeeded = TRUE;
		if ( theHold.Holding() )
			theHold.Put(new PatchRestore(patchData,this,patch));
			// Call the vertex type change function
		patch->AddHook(vert,seg);
//		patch->UpdateHooks();
//			InvalidateMesh();

		patchData->UpdateChanges(patch);
		patchData->TempData(this)->Invalidate(PART_TOPO);
//			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
//		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_PATCHCHANGE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);

/*	// If any bits are set in the selection set, let's DO IT!!
	if ( !ip ) return;
	theHold.Begin();
	POPatchGenRecord *rec = new POPatchGenRecord(this);
	if ( theHold.Holding() )
		theHold.Put(new PatchObjectRestore(this,rec));
		// Call the patch type change function

	patch.AddHook();
	patch.InvalidateGeomCache();
	InvalidateMesh();
	theHold.Accept(GetResString(IDS_TH_PATCHCHANGE));

	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
*/
	}

void EditPatchMod::DoRemoveHook() 
{

	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	BOOL holdNeeded = FALSE;
	BOOL hadSelected = FALSE;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		BOOL altered = FALSE;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;		
		patchData->RecordTopologyTags(patch);
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->vertSel.NumberSet()) {

			altered = holdNeeded = TRUE;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch));
			// Call the vertex type change function
			patch->RemoveHook();
//			patch->InvalidateGeomCache();
//			InvalidateMesh();
 


			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);

//watje 10-14-99 bug 212886 some spurious topo flags need to be cleared
			patch->hookTopoMarkers.ZeroCount();
			patch->hookTopoMarkersA.ZeroCount();
			patch->hookTopoMarkersB.ZeroCount();

			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_PATCHCHANGE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}


void EditPatchMod::DoExtrude()
{


	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	BOOL holdNeeded = FALSE;
	BOOL hadSelected = FALSE;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		BOOL altered = FALSE;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;		
		patchData->RecordTopologyTags(patch);
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->patchSel.NumberSet()) {

			altered = holdNeeded = TRUE;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch));
			// Call the vertex type change function
			patch->CreateExtrusion();
//			patch->InvalidateGeomCache();
//			InvalidateMesh();

			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_PATCHCHANGE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}


	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);

/*
	theHold.Begin();
	patch.RecordTopologyTags();
	POPatchGenRecord *rec = new POPatchGenRecord(this);
	if ( theHold.Holding() )
		theHold.Put(new PatchObjectRestore(this,rec));

	patch.CreateExtrusion();
	
	ResolveTopoChanges();
	theHold.Accept(GetResString(IDS_TH_PATCHADD));

	patch.InvalidateGeomCache();
	InvalidateMesh();

	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
*/
}


void EditPatchMod::BeginExtrude(TimeValue t) {	
	if (inExtrude) return;
	inExtrude = TRUE;
	theHold.SuperBegin();
	DoExtrude();
//	PlugControllersSel(t,sel);
	theHold.Begin();
}

void EditPatchMod::EndExtrude (TimeValue t, BOOL accept) {		
	if (!ip) return;

	if (!inExtrude) return;

	ISpinnerControl *spin;
	inExtrude = FALSE;
	spin = GetISpinner(GetDlgItem(hOpsPanel,IDC_EP_EXTRUDESPINNER));
	if (spin) {
		spin->SetValue(0,FALSE);
		ReleaseISpinner(spin);
		}
//	TempData()->freeBevelInfo();

	theHold.Accept(GetString(IDS_RB_EXTRUDE));
	if (accept) theHold.SuperAccept(GetString(IDS_RB_EXTRUDE));
	else theHold.SuperCancel();

}



void EditPatchMod::Extrude( TimeValue t, float amount, BOOL useLocalNorms ) {
	if (!inExtrude) return;


	ModContextList mcList;		
	INodeTab nodes;
	BOOL holdNeeded = FALSE;
	BOOL hadSelected = FALSE;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

//	theHold.Begin();
	RecordTopologyTags();

	for ( int i = 0; i < mcList.Count(); i++ ) {
		BOOL altered = FALSE;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;		
		patchData->RecordTopologyTags(patch);
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->patchSel.NumberSet()) {

			altered = holdNeeded = TRUE;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch));
			// Call the vertex type change function
			patch->MoveNormal(amount,useLocalNorms);
//			patch->InvalidateGeomCache();
//			InvalidateMesh();

			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_PATCHCHANGE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}


	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);

/*	theHold.Restore();
	patch.MoveNormal(amount,useLocalNorms);

	patch.InvalidateGeomCache();
	InvalidateMesh();

	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
*/


}


void EditPatchMod::DoBevel()
{

	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	BOOL holdNeeded = FALSE;
	BOOL hadSelected = FALSE;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		BOOL altered = FALSE;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;		
		patchData->RecordTopologyTags(patch);
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->patchSel.NumberSet()) {

			altered = holdNeeded = TRUE;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch));
			// Call the vertex type change function
			patch->CreateBevel();
//			patch->CreateExtrusion();
//			patch->InvalidateGeomCache();
//			InvalidateMesh();

			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_PATCHCHANGE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}


	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);

	/*
	theHold.Begin();
	patch.RecordTopologyTags();
	POPatchGenRecord *rec = new POPatchGenRecord(this);
	if ( theHold.Holding() )
		theHold.Put(new PatchObjectRestore(this,rec));

	patch.CreateBevel();
	
	ResolveTopoChanges();
	theHold.Accept(GetResString(IDS_TH_PATCHADD));

	patch.InvalidateGeomCache();
	InvalidateMesh();

	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
*/
}


void EditPatchMod::BeginBevel(TimeValue t) {	
	if (inBevel) return;
	inBevel = TRUE;
	theHold.SuperBegin();
	DoBevel();
//	PlugControllersSel(t,sel);
	theHold.Begin();
}

void EditPatchMod::EndBevel (TimeValue t, BOOL accept) {		
	if (!ip) return;
	if (!inBevel) return;
	inBevel = FALSE;
//	TempData()->freeBevelInfo();
	ISpinnerControl *spin;

	spin = GetISpinner(GetDlgItem(hOpsPanel,IDC_EP_OUTLINESPINNER));
	if (spin) {
		spin->SetValue(0,FALSE);
		ReleaseISpinner(spin);
		}


	theHold.Accept(GetString(IDS_EM_BEVEL));
	if (accept) theHold.SuperAccept(GetString(IDS_EM_BEVEL));
	else theHold.SuperCancel();

}



void EditPatchMod::Bevel( TimeValue t, float amount, BOOL smoothStart, BOOL smoothEnd ) {
	if (!inBevel) return;


	ModContextList mcList;		
	INodeTab nodes;
	BOOL holdNeeded = FALSE;
	BOOL hadSelected = FALSE;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

//	theHold.Begin();

	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		BOOL altered = FALSE;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;		
		patchData->RecordTopologyTags(patch);
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->patchSel.NumberSet()) {

			altered = holdNeeded = TRUE;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch));
			// Call the vertex type change function
			patch->Bevel(amount,smoothStart, smoothEnd);
//			patch->MoveNormal(amount,useLocalNorms);
//			patch->InvalidateGeomCache();
//			InvalidateMesh();

			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_PATCHCHANGE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}


	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
/*
	theHold.Restore();
	patch.Bevel(amount,smoothStart, smoothEnd);

	patch.InvalidateGeomCache();
	InvalidateMesh();

	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
*/
}









void EditPatchMod::DoSubdivide(int type) {
	switch(type) {
		case EP_EDGE:
			DoEdgeSubdivide();
			break;
		case EP_PATCH:
			DoPatchSubdivide();
			break;
		}
	}

void EditPatchMod::DoEdgeSubdivide() {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	int holdNeeded = 0;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		int altered = 0;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
		patchData->RecordTopologyTags(patch);
					
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->edgeSel.NumberSet()) {
			altered = holdNeeded = 1;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch,"DoEdgeSubdivide"));
			// Call the patch add function
			SubdividePatch(SUBDIV_EDGES, propagate, patch);
			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_EDGESUBDIVIDE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOVALIDEDGESSEL),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

void EditPatchMod::DoPatchSubdivide() {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	int holdNeeded = 0;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		int altered = 0;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
		patchData->RecordTopologyTags(patch);
					
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->patchSel.NumberSet()) {
			altered = holdNeeded = 1;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch,"DoPatchSubdivide"));
			// Call the patch add function
			SubdividePatch(SUBDIV_PATCHES, propagate, patch);
			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_PATCHSUBDIVIDE));
		}
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

void EditPatchMod::DoVertWeld() {
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	int holdNeeded = 0;
	BOOL hadSel = FALSE;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();
	for ( int i = 0; i < mcList.Count(); i++ ) {
		BOOL altered = FALSE;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
		patchData->RecordTopologyTags(patch);
		
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any bits are set in the selection set, let's DO IT!!
		if(patch->vertSel.NumberSet() > 1) {
			hadSel = TRUE;
			if ( theHold.Holding() )
				theHold.Put(new PatchRestore(patchData,this,patch,"DoVertWeld"));
			// Call the patch weld function
			if(patch->Weld(weldThreshold)) {
				altered = holdNeeded = TRUE;
				patchData->UpdateChanges(patch);
				patchData->TempData(this)->Invalidate(PART_TOPO);
				}
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_VERTWELD));
		}
	else {
		if(!hadSel)
			ip->DisplayTempPrompt(GetString(IDS_TH_NOVERTSSEL),PROMPT_TIME);
		else
			ip->DisplayTempPrompt(GetString(IDS_TH_NOWELDPERFORMED),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);
	}

static void MakeDummyMapPatches(int channel, PatchMesh *patch) {
	patch->setNumMapVerts (channel, 1);
	patch->tVerts[channel][0] = UVVert(0,0,0);
	patch->setNumMapPatches (channel, patch->numPatches);
	for(int i = 0; i < patch->numPatches; ++i) {
		Patch &p = patch->patches[i];
		TVPatch &tp = patch->tvPatches[channel][i];
		tp.Init();	// Sets all indices to zero
		}
	}

// Detach all selected patches
void EditPatchMod::DoPatchDetach(int copy, int reorient) {
	int dialoged = 0;
	TSTR newName(GetString(IDS_TH_PATCH));
	ModContextList mcList;		
	INodeTab nodes;
	TimeValue t = ip->GetTime();
	int holdNeeded = 0;

	if ( !ip ) return;

	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	theHold.Begin();
	RecordTopologyTags();

	// Create a patch mesh object
	PatchObject *patchOb = new PatchObject;
	PatchMesh &pmesh = patchOb->patch;
	int verts = 0;
	int vecs = 0;
	int patches = 0;

	int multipleObjects = (mcList.Count() > 1) ? 1 : 0;

//watje 10-4-99  184681 
	Tab<HookPoint> whooks;

	for ( int i = 0; i < mcList.Count(); i++ ) {
		int altered = 0;
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if ( !patchData ) continue;
		if ( patchData->GetFlag(EPD_BEENDONE) ) continue;

		// If the mesh isn't yet cache, this will cause it to get cached.
		PatchMesh *patch = patchData->TempData(this)->GetPatch(t);
		if(!patch)
			continue;
		patchData->RecordTopologyTags(patch);
		
		// If this is the first edit, then the delta arrays will be allocated
		patchData->BeginEdit(t);

		// If any patches selected, we'll need to process this one
		if(patch->patchSel.NumberSet()) {
			if(!dialoged) {
				dialoged = 1;
				if(!GetDetachOptions(ip, newName))
					goto bail_out;
				}
			// Save the unmodified info.
			if ( theHold.Holding() ) {
				theHold.Put(new PatchRestore(patchData,this,patch,"DoPatchDetach"));
				}
			PatchMesh wpatch = *patch;
//watje 10-4-99 184681
			wpatch.hooks = patch->hooks;
			wpatch.hookTopoMarkers = patch->hookTopoMarkers;
			wpatch.hookTopoMarkersA = patch->hookTopoMarkersA;
			wpatch.hookTopoMarkersB = patch->hookTopoMarkersB;

			BitArray vdel(wpatch.numVerts);
			vdel.ClearAll();
			BitArray pdel = wpatch.patchSel;	// Get inverse selection set
			// If not copying, delete the patches from this object
			if(!copy)
				DeletePatchParts(patch, vdel, pdel);
			pdel = ~wpatch.patchSel;	// Get inverse selection set
			if(pdel.NumberSet()) {
				vdel.ClearAll();
				DeletePatchParts(&wpatch, vdel, pdel);
				}
//watje 10-4-99 184681
			wpatch.HookFixTopology();
//watje 10-4-99 184681
			int oldEdges = pmesh.numEdges;


			// We've deleted everything that wasn't selected -- Now add this to the patch object accumulator
			int oldVerts = pmesh.numVerts;
			int oldVecs = pmesh.numVecs;
			int oldPatches = pmesh.numPatches;

//watje 10-4-99 184681
			for (int hi = 0; hi < wpatch.hooks.Count(); hi++)
				{
				HookPoint tempHook = wpatch.hooks[hi];
				tempHook.upperPoint += oldVerts;
				tempHook.lowerPoint += oldVerts;
				tempHook.hookPoint += oldVerts;
				tempHook.upperVec += oldVecs;
				tempHook.lowerVec += oldVecs;
				tempHook.upperHookVec += oldVecs;
				tempHook.lowerHookVec += oldVecs;
				tempHook.upperPatch += oldPatches; 
				tempHook.lowerPatch += oldPatches;
				tempHook.hookPatch += oldPatches;
				tempHook.hookEdge += oldEdges;
				tempHook.upperEdge += oldEdges;
				tempHook.lowerEdge += oldEdges;

				whooks.Append(1,&tempHook,1);

				}

			int newVerts = oldVerts + wpatch.numVerts;
			int newVecs = oldVecs + wpatch.numVecs;
			int newPatches = oldPatches + wpatch.numPatches;
			pmesh.setNumVerts(newVerts, TRUE);
			pmesh.setNumVecs(newVecs, TRUE);
			pmesh.setNumPatches(newPatches, TRUE);
			altered = holdNeeded = 1;
			Matrix3 tm(1);
			if(multipleObjects && !reorient)
				tm = nodes[i]->GetObjectTM(t);
			for(int i = 0, i2 = oldVerts; i < wpatch.numVerts; ++i, ++i2) {
				pmesh.verts[i2] = wpatch.verts[i];
				pmesh.verts[i2].p = pmesh.verts[i2].p * tm;
				}
			for(i = 0, i2 = oldVecs; i < wpatch.numVecs; ++i, ++i2) {
				pmesh.vecs[i2] = wpatch.vecs[i];
				pmesh.vecs[i2].p = pmesh.vecs[i2].p * tm;
				}
			for(i = 0, i2 = oldPatches; i < wpatch.numPatches; ++i, ++i2) {
				Patch &p = wpatch.patches[i];
				Patch &p2 = pmesh.patches[i2];
				p2 = p;
				for(int j = 0; j < p2.type; ++j) {	// Adjust vertices and interior vectors
					p2.v[j] += oldVerts;
					p2.interior[j] += oldVecs;
					}
				for(j = 0; j < (p2.type * 2); ++j)	// Adjust edge vectors
					p2.vec[j] += oldVecs;
				}
			// Now copy over mapping information
			int dmaps = pmesh.getNumMaps();
			int smaps = wpatch.getNumMaps();
			int maxMaps = dmaps > smaps ? dmaps : smaps;
			if(maxMaps != dmaps)
				pmesh.setNumMaps (maxMaps, TRUE);
			if(maxMaps != smaps)
				wpatch.setNumMaps(maxMaps, TRUE);
			// Then make sure any active maps are active in both:
			for(int chan = 0; chan < maxMaps; ++chan) {
				if(pmesh.tvPatches[chan] || wpatch.tvPatches[chan]) {
					if(!pmesh.tvPatches[chan]) MakeDummyMapPatches(chan, &pmesh);
					if(!wpatch.tvPatches[chan]) MakeDummyMapPatches(chan, &wpatch);
					}
				}
			for(chan = 0; chan < pmesh.getNumMaps(); ++chan) {
				if(chan < wpatch.getNumMaps()) {
					int oldTVerts = pmesh.numTVerts[chan];
					int newTVerts = oldTVerts + wpatch.numTVerts[chan];
					pmesh.setNumMapVerts (chan, newTVerts, TRUE);
					for(i = 0, i2 = oldTVerts; i < wpatch.numTVerts[chan]; ++i, ++i2)
						pmesh.tVerts[chan][i2] = wpatch.tVerts[chan][i];
					if(pmesh.tvPatches[chan]) {
						for(i = 0, i2 = oldPatches; i < wpatch.numPatches; ++i, ++i2) {
							Patch &p = wpatch.patches[i];
							TVPatch &tp = wpatch.tvPatches[chan][i];
							TVPatch &tp2 = pmesh.tvPatches[chan][i2];
							tp2 = tp;
							for(int j = 0; j < p.type; ++j)	// Adjust vertices
								tp2.tv[j] += oldTVerts;
							}
						}
					}
				}
			patchData->UpdateChanges(patch);
			patchData->TempData(this)->Invalidate(PART_TOPO);
//watje 10-4-99 184681
			pmesh.buildLinkages();
			}

		bail_out:
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
	
	if(holdNeeded) {
		pmesh.computeInteriors();
		pmesh.buildLinkages();
//watje 10-4-99 184681
		pmesh.hooks = whooks;

		INode *newNode = ip->CreateObjectNode(patchOb);
		newNode->SetMtl(nodes[0]->GetMtl());
		newNode->SetName(newName.data());
		patchOb->patch.InvalidateGeomCache();
		if(!multipleObjects) {	// Single input object?
			if(!reorient) {
				Matrix3 tm = nodes[0]->GetObjectTM(t);
				newNode->SetNodeTM(t, tm);	// Use this object's TM.
				}
			}
		else {
			if(!reorient) {
				Matrix3 matrix;
				matrix.IdentityMatrix();
				newNode->SetNodeTM(t, matrix);	// Use identity TM
				}
			}
		newNode->FlagForeground(t);		// WORKAROUND!
		ResolveTopoChanges();
		theHold.Accept(GetString(IDS_TH_DETACHPATCH));
		}
	else {
		delete patchOb;	// Didn't need it after all!
		if(!dialoged)
			ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(t,REDRAW_NORMAL);
	}


void EditPatchMod::SetTessUI(HWND hDlg, TessApprox *tess)
{
	EnableWindow(GetDlgItem(hDlg, IDC_TESS_U), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_TESS_U_SPINNER), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_TESS_V), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_TESS_V_SPINNER), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_TESS_EDGE), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_TESS_EDGE_SPINNER), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_TESS_DIST), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_TESS_DIST_SPINNER), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_TESS_ANG), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_TESS_ANG_SPINNER), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_ADVANCED_PARAMETERS), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_TESS_NORMALS), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_TESS_VIEW_DEP), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_WELDTESS), FALSE);
	CheckDlgButton( hDlg, IDC_TESS_SET, FALSE);
	CheckDlgButton( hDlg, IDC_TESS_REGULAR, FALSE);
	CheckDlgButton( hDlg, IDC_TESS_PARAM, FALSE);
	CheckDlgButton( hDlg, IDC_TESS_SPATIAL, FALSE);
	CheckDlgButton( hDlg, IDC_TESS_CURV, FALSE);
	CheckDlgButton( hDlg, IDC_TESS_LDA, FALSE);

	ShowWindow(GetDlgItem(hDlg, IDC_TESS_VIEW_DEP), SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_TESS_NORMALS), SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_WELDTESS), SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_MESH), SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_DISP), SW_HIDE);

//watje 12-10-98
	if (tess->showInteriorFaces)
		CheckDlgButton( hDlg, IDC_SHOW_INTERIOR_FACES, TRUE);
	else CheckDlgButton( hDlg, IDC_SHOW_INTERIOR_FACES, FALSE);

	switch (tess->type) {
	case TESS_SET:
		CheckDlgButton( hDlg, IDC_TESS_SET, TRUE);
		mergeSpin->Disable();
		EnableWindow(GetDlgItem(hDlg, IDC_SHOW_INTERIOR_FACES), TRUE);
		break;

	case TESS_REGULAR:
		CheckDlgButton( hDlg, IDC_TESS_REGULAR, TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_U), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_U_SPINNER), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_V), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_V_SPINNER), TRUE);

		ShowWindow(GetDlgItem(hDlg, IDC_TESS_VIEW_DEP), SW_HIDE);
		mergeSpin->Enable();
		EnableWindow(GetDlgItem(hDlg, IDC_SHOW_INTERIOR_FACES), FALSE);
		break;

	case TESS_PARAM:
		CheckDlgButton( hDlg, IDC_TESS_PARAM, TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_U), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_U_SPINNER), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_V), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_V_SPINNER), TRUE);

		mergeSpin->Enable();
		EnableWindow(GetDlgItem(hDlg, IDC_SHOW_INTERIOR_FACES), FALSE);
		break;

	case TESS_SPATIAL:
		CheckDlgButton( hDlg, IDC_TESS_SPATIAL, TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_EDGE), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_EDGE_SPINNER), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_VIEW_DEP), TRUE);

		EnableWindow(GetDlgItem(hDlg, IDC_TESS_VIEW_DEP), !settingViewportTess);
		ShowWindow(GetDlgItem(hDlg, IDC_TESS_VIEW_DEP), settingViewportTess?SW_HIDE:SW_SHOW);
		EnableWindow(GetDlgItem(hDlg, IDC_ADVANCED_PARAMETERS), TRUE);
		mergeSpin->Enable();
		EnableWindow(GetDlgItem(hDlg, IDC_SHOW_INTERIOR_FACES), FALSE);
		break;

	case TESS_CURVE:
		CheckDlgButton( hDlg, IDC_TESS_CURV, TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_DIST), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_DIST_SPINNER), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_ANG), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_ANG_SPINNER), TRUE);

		EnableWindow(GetDlgItem(hDlg, IDC_TESS_VIEW_DEP), !settingViewportTess);
		ShowWindow(GetDlgItem(hDlg, IDC_TESS_VIEW_DEP), settingViewportTess?SW_HIDE:SW_SHOW);
		EnableWindow(GetDlgItem(hDlg, IDC_ADVANCED_PARAMETERS), TRUE);
		mergeSpin->Enable();
		EnableWindow(GetDlgItem(hDlg, IDC_SHOW_INTERIOR_FACES), FALSE);
		break;

	case TESS_LDA:
		CheckDlgButton( hDlg, IDC_TESS_LDA, TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_EDGE), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_EDGE_SPINNER), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_DIST), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_DIST_SPINNER), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_ANG), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_TESS_ANG_SPINNER), TRUE);

		EnableWindow(GetDlgItem(hDlg, IDC_TESS_VIEW_DEP), !settingViewportTess);
		ShowWindow(GetDlgItem(hDlg, IDC_TESS_VIEW_DEP), settingViewportTess?SW_HIDE:SW_SHOW);
		EnableWindow(GetDlgItem(hDlg, IDC_ADVANCED_PARAMETERS), TRUE);
		mergeSpin->Enable();
		EnableWindow(GetDlgItem(hDlg, IDC_SHOW_INTERIOR_FACES), FALSE);
		break;
	}


	if (settingViewportTess) {
		ShowWindow(GetDlgItem(hDlg, IDC_TESS_SET), SW_SHOW);

		if (tess->type != TESS_SET) {
			ShowWindow(GetDlgItem(hDlg, IDC_TESS_NORMALS), SW_SHOW);
			ShowWindow(GetDlgItem(hDlg, IDC_WELDTESS), SW_SHOW);
			EnableWindow(GetDlgItem(hDlg, IDC_TESS_NORMALS), !GetViewTessWeld());
			EnableWindow(GetDlgItem(hDlg, IDC_WELDTESS), tess->merge > 0.0f);
		}
	} else {
		if (settingDisp) {
			ShowWindow(GetDlgItem(hDlg, IDC_MESH), SW_SHOW);
			ShowWindow(GetDlgItem(hDlg, IDC_DISP), SW_SHOW);
		} else {
			if (tess->type != TESS_SET) {
				ShowWindow(GetDlgItem(hDlg, IDC_MESH), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_DISP), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_TESS_NORMALS), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_WELDTESS), SW_SHOW);
				EnableWindow(GetDlgItem(hDlg, IDC_TESS_NORMALS), !GetProdTessWeld());
				EnableWindow(GetDlgItem(hDlg, IDC_WELDTESS), tess->merge > 0.0f);
				CheckDlgButton( hDlg, IDC_MESH, TRUE);
			}
			ShowWindow(GetDlgItem(hDlg, IDC_TESS_SET), SW_SHOW);
		}
	}

	// now set all the settings
	uSpin->SetValue(tess->u, FALSE);
	vSpin->SetValue(tess->v, FALSE);
	edgeSpin->SetValue(tess->edge, FALSE);
	distSpin->SetValue(tess->dist, FALSE);
	angSpin->SetValue(tess->ang, FALSE);
	mergeSpin->SetValue(tess->merge, FALSE);
	CheckDlgButton( hDlg, IDC_TESS_VIEW_DEP, tess->view);
	if (settingViewportTess) {
		CheckDlgButton( hDlg, IDC_TESS_VIEW, TRUE);
		CheckDlgButton( hDlg, IDC_TESS_RENDERER, FALSE);
		CheckDlgButton( hDlg, IDC_TESS_NORMALS, GetViewTessNormals());
		CheckDlgButton( hDlg, IDC_WELDTESS, GetViewTessWeld());
	} else {
		CheckDlgButton( hDlg, IDC_TESS_VIEW, FALSE);
		CheckDlgButton( hDlg, IDC_TESS_RENDERER, TRUE);
		CheckDlgButton( hDlg, IDC_TESS_NORMALS, GetProdTessNormals());
		CheckDlgButton( hDlg, IDC_WELDTESS, GetProdTessWeld());
	}
	CheckDlgButton( hDlg, IDC_DISP, settingDisp);
}

int EditPatchMod::GetSelMatIndex()
	{
	ModContextList mcList;	
	INodeTab nodes;
	BOOL first = 1;
	int mat=-1;

	if (!ip) return -1;
	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);

	for (int i = 0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData) continue;
		if (patchData->GetFlag(EPD_BEENDONE)) continue;
		patchData->BeginEdit(ip->GetTime());
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
		if(!patch)
			continue;
		for (int j=0; j<patch->getNumPatches(); j++) {
			if (patch->patchSel[j]) {
				if (first) {
					first = FALSE;
					mat   = (int)patch->getPatchMtlIndex(j);					
				} else {
					if ((int)patch->getPatchMtlIndex(j) != mat) {
						return -1;
						}
					}
				}
			}
		}
	
	nodes.DisposeTemporary();
	return mat;
	}

void EditPatchMod::SetSelMatIndex(int index)
	{
	ModContextList mcList;	
	INodeTab nodes;
	BOOL holdNeeded = FALSE;

	if (!ip) return;
	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);	

	theHold.Begin();
	for (int i = 0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		BOOL altered = FALSE;
		if (!patchData) continue;
		if (patchData->GetFlag(EPD_BEENDONE)) continue;
		patchData->BeginEdit(ip->GetTime());
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());		
		if(!patch)
			continue;

		// Start a restore object...
		if (theHold.Holding()) {
			theHold.Put(new PatchRestore(patchData,this,patch,"SetSelMatIndex"));
			}

		for (int j=0; j<patch->getNumPatches(); j++) {			
			if (patch->patchSel[j]) {
				altered = holdNeeded = TRUE;
				patch->setPatchMtlIndex(j,(MtlID)index);			
				}
			}
		
		if(altered) {
			patchData->UpdateChanges(patch, FALSE);
			patchData->TempData(this)->Invalidate(PART_TOPO);
			}
		patchData->SetFlag(EPD_BEENDONE,TRUE);		
		}	
	
	if(holdNeeded)
		theHold.Accept(GetString(IDS_TH_PATCHMTLCHANGE));
	else {
		ip->DisplayTempPrompt(GetString(IDS_TH_NOPATCHESSEL),PROMPT_TIME);
		theHold.End();
		}

	nodes.DisposeTemporary();
	InvalidateSurfaceUI();
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
	}

void EditPatchMod::SelectByMat(int index,BOOL clear)
	{
	ModContextList mcList;	
	INodeTab nodes;

	if (!ip) return;
	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	
	theHold.Begin();

	for (int i = 0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData) continue;
		if (patchData->GetFlag(EPD_BEENDONE)) continue;
		patchData->BeginEdit(ip->GetTime());
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());		
		if(!patch)
			continue;

		// Start a restore object...
		if (theHold.Holding()) {
			theHold.Put(new PatchSelRestore(patchData,this,patch));
			}
		
		if (clear)
			patch->patchSel.ClearAll();

		for (int j=0; j<patch->getNumPatches(); j++) {			
			if (patch->getPatchMtlIndex(j)==index)
				patch->patchSel.Set(j);
			}
		
		patchData->UpdateChanges(patch, FALSE);
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		patchData->TempData(this)->Invalidate(PART_SELECT);
		}
		
	PatchSelChanged();
	theHold.Accept(GetString(IDS_RB_SELECTBYMATID));
	
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
	}

DWORD EditPatchMod::GetSelSmoothBits(DWORD &invalid)
	{
	BOOL first = 1;
	DWORD bits = 0;
	invalid = 0;
	ModContextList mcList;	
	INodeTab nodes;

	if (!ip) return 0;
	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	
	for (int i = 0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData) continue;
		if (patchData->GetFlag(EPD_BEENDONE)) continue;
		patchData->BeginEdit(ip->GetTime());
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());		
		if(!patch)
			continue;

		for (int j=0; j<patch->getNumPatches(); j++) {
			if (patch->patchSel[j]) {
				if (first) {
					first = FALSE;
					bits  = patch->patches[j].smGroup;					
				} else {
					if (patch->patches[j].smGroup != bits) {
						invalid |= patch->patches[j].smGroup^bits;
						}
					}
				}
			}

		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
		
	nodes.DisposeTemporary();
	return bits;
	}

DWORD EditPatchMod::GetUsedSmoothBits()
	{	
	DWORD bits = 0;
	ModContextList mcList;	
	INodeTab nodes;

	if (!ip) return 0;
	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	
	for (int i = 0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData) continue;
		if (patchData->GetFlag(EPD_BEENDONE)) continue;
		patchData->BeginEdit(ip->GetTime());
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());		
		if(!patch)
			continue;

		for (int j=0; j<patch->getNumPatches(); j++) {
			bits |= patch->patches[j].smGroup;
			}		

		patchData->SetFlag(EPD_BEENDONE,TRUE);
		}
		
	nodes.DisposeTemporary();
	return bits;
	}

void EditPatchMod::SelectBySmoothGroup(DWORD bits,BOOL clear)
	{
	ModContextList mcList;	
	INodeTab nodes;

	if (!ip) return;
	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	
	theHold.Begin();

	for (int i = 0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData) continue;
		if (patchData->GetFlag(EPD_BEENDONE)) continue;
		patchData->BeginEdit(ip->GetTime());
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());		
		if(!patch)
			continue;

		// Start a restore object...
		if (theHold.Holding()) {
			theHold.Put(new PatchSelRestore(patchData,this,patch));
			}
		
		if (clear)
			patch->patchSel.ClearAll();			
		for (int j=0; j<patch->getNumPatches(); j++) {			
			if (patch->patches[j].smGroup & bits) {
				patch->patchSel.Set(j);			
				}
			}
		
		patchData->UpdateChanges(patch, FALSE);
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		patchData->TempData(this)->Invalidate(PART_SELECT);
		}
		
	PatchSelChanged();
	theHold.Accept(GetString(IDS_RB_SELECTBYSMOOTH));
	
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
	}

void EditPatchMod::SetSelSmoothBits(DWORD bits,DWORD which)
	{
	ModContextList mcList;	
	INodeTab nodes;

	if (!ip) return;
	ip->GetModContexts(mcList,nodes);
	ClearPatchDataFlag(mcList,EPD_BEENDONE);
	
	theHold.Begin();

	for (int i = 0; i < mcList.Count(); i++) {
		EditPatchData *patchData = (EditPatchData*)mcList[i]->localData;
		if (!patchData) continue;
		if (patchData->GetFlag(EPD_BEENDONE)) continue;
		patchData->BeginEdit(ip->GetTime());
		PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());		
		if(!patch)
			continue;

		// Start a restore object...
		if (theHold.Holding()) {
			theHold.Put(new PatchSelRestore(patchData,this,patch));
			}
		
		for (int j=0; j<patch->getNumPatches(); j++) {			
			if (patch->patchSel[j]) {
				patch->patches[j].smGroup &= ~which;
				patch->patches[j].smGroup |= bits&which;			
				}
			}
		
		patchData->UpdateChanges(patch, FALSE);
		patchData->SetFlag(EPD_BEENDONE,TRUE);
		patchData->TempData(this)->Invalidate(PART_SELECT);
		}
		
	PatchSelChanged();
	theHold.Accept(GetString(IDS_RB_SETSMOOTHGROUP));
	
	nodes.DisposeTemporary();
	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	InvalidateSurfaceUI();
	ip->RedrawViews(ip->GetTime());
	}

void EditPatchMod::PatchSelChanged() {
	SelectionChanged();
	if (hSurfPanel && selLevel == EP_PATCH)
		InvalidateSurfaceUI();
	}

class AdvParams {
public:
	TessSubdivStyle mStyle;
	int mMin, mMax;
	int mTris;
};

static AdvParams sParams;

BOOL CALLBACK AdvParametersDialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam ); 

void EditPatchMod::SetSelDlgEnables() {
	if(!hSelectPanel)
		return;
	ICustButton *but = GetICustButton(GetDlgItem(hSelectPanel,IDC_NS_PASTE));
	but->Disable();
	switch(GetSubobjectLevel()) {
		case PO_VERTEX:
			if (GetPatchNamedSelClip(CLIP_P_VERT))
				but->Enable();
			break;
		case PO_EDGE:
			if (GetPatchNamedSelClip(CLIP_P_EDGE))
				but->Enable();
			break;
		case PO_PATCH:
			if (GetPatchNamedSelClip(CLIP_P_PATCH))
				but->Enable();
			break;
		}
	ReleaseICustButton(but);
	}

void EditPatchMod::SetOpsDlgEnables() {
	if(!hOpsPanel)
		return;
	
	assert(ip);
	
	// Disconnect right-click and delete mechanisms
	ip->GetRightClickMenuManager()->Unregister(&pMenu);
	ip->UnRegisterDeleteUser(&pDel);

	BOOL oType = (GetSubobjectLevel() == EP_OBJECT) ? TRUE : FALSE;
	BOOL vType = (GetSubobjectLevel() == EP_VERTEX) ? TRUE : FALSE;
	BOOL eType = (GetSubobjectLevel() == EP_EDGE) ? TRUE : FALSE;
	BOOL pType = (GetSubobjectLevel() == EP_PATCH) ? TRUE : FALSE;
	BOOL epType = (eType || pType) ? TRUE : FALSE;
	BOOL vepType = (vType || eType || pType) ? TRUE : FALSE;


	ICustButton *but;
	but = GetICustButton (GetDlgItem (hOpsPanel, IDC_BIND));
	but->Enable (vType);
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hOpsPanel, IDC_UNBIND));
	but->Enable (vType);
	ReleaseICustButton (but);

	but = GetICustButton (GetDlgItem (hOpsPanel, IDC_SUBDIVIDE));
	but->Enable (epType);
	ReleaseICustButton (but);
	EnableWindow (GetDlgItem (hOpsPanel, IDC_PROPAGATE), epType);
	but = GetICustButton (GetDlgItem (hOpsPanel, IDC_ADDTRI));
	but->Enable (eType);
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hOpsPanel, IDC_ADDQUAD));
	but->Enable (eType);
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hOpsPanel, IDC_WELD));
	but->Enable (vType);
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hOpsPanel, IDC_DETACH));
	but->Enable (pType);
	ReleaseICustButton (but);
	EnableWindow (GetDlgItem (hOpsPanel, IDC_DETACHREORIENT), pType);
	EnableWindow (GetDlgItem (hOpsPanel, IDC_DETACHCOPY), pType);
	but = GetICustButton (GetDlgItem (hOpsPanel, IDC_PATCH_DELETE));
	but->Enable (vepType);
	ReleaseICustButton (but);
	ISpinnerControl *spin;
	spin = GetISpinner(GetDlgItem(hOpsPanel,IDC_THRESHSPINNER));
	spin->Enable(vType);
	ReleaseISpinner(spin);

//3-1-99 watje
	spin = GetISpinner(GetDlgItem(hOpsPanel,IDC_HIDE));
	spin->Enable(vepType);
	ReleaseISpinner(spin);

	but = GetICustButton (GetDlgItem (hOpsPanel, IDC_EP_EXTRUDE));
	but->Enable (pType);
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hOpsPanel, IDC_EP_BEVEL));
	but->Enable (pType);
	ReleaseICustButton (but);

	spin = GetISpinner(GetDlgItem(hOpsPanel,IDC_EP_EXTRUDESPINNER));
	spin->Enable(pType);
	ReleaseISpinner(spin);

	spin = GetISpinner(GetDlgItem(hOpsPanel,IDC_EP_OUTLINESPINNER));
	spin->Enable(pType);
	ReleaseISpinner(spin);
	EnableWindow(GetDlgItem(hOpsPanel,IDC_EM_EXTYPE_A),pType);
	EnableWindow(GetDlgItem(hOpsPanel,IDC_EM_EXTYPE_B),pType);

	EnableWindow(GetDlgItem(hOpsPanel,IDC_EP_SM_SMOOTH4),pType);
	EnableWindow(GetDlgItem(hOpsPanel,IDC_EP_SM_SMOOTH5),pType);
	EnableWindow(GetDlgItem(hOpsPanel,IDC_EP_SM_SMOOTH6),pType);

	EnableWindow(GetDlgItem(hOpsPanel,IDC_EP_SM_SMOOTH),pType);
	EnableWindow(GetDlgItem(hOpsPanel,IDC_EP_SM_SMOOTH2),pType);
	EnableWindow(GetDlgItem(hOpsPanel,IDC_EP_SM_SMOOTH3),pType);

	// Enable/disable right-click and delete mechanisms
	if(!oType) {			
		pMenu.SetMod(this);
		ip->GetRightClickMenuManager()->Register(&pMenu);
		pDel.SetMod(this);
		ip->RegisterDeleteUser(&pDel);
		}
	}

void EditPatchMod::SetSurfDlgEnables() {
	if(!hSurfPanel)
		return;
	
	assert(ip);
	
	BOOL oType = (GetSubobjectLevel() == EP_OBJECT) ? TRUE : FALSE;
	BOOL pType = (GetSubobjectLevel() == EP_PATCH) ? TRUE : FALSE;

	if(oType)
		return;
	if(!pType)
		return;

	ICustButton *but;
	ISpinnerControl *spin;
	but = GetICustButton (GetDlgItem (hSurfPanel, IDC_SELECT_BYID));
	but->Enable (pType);
	ReleaseICustButton (but);
	spin = GetISpinner(GetDlgItem(hSurfPanel,IDC_MAT_IDSPIN));
	spin->Enable(pType);
	ReleaseISpinner(spin);
	for(int i = 0; i < 32; ++i) {
		but = GetICustButton (GetDlgItem (hSurfPanel, IDC_SMOOTH_GRP1+i));
		but->Enable (pType);
		ReleaseICustButton (but);
		}
	but = GetICustButton (GetDlgItem (hSurfPanel, IDC_SELECTBYSMOOTH));
	but->Enable (pType);
	ReleaseICustButton (but);
	but = GetICustButton (GetDlgItem (hSurfPanel, IDC_SMOOTH_CLEAR));
	but->Enable (pType);
	ReleaseICustButton (but);
	}

/*-------------------------------------------------------------------*/

BOOL CALLBACK PatchSelectDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
	{
	EditPatchMod *ep = (EditPatchMod *)GetWindowLong( hDlg, GWL_USERDATA );
	ICustToolbar *iToolbar;
	if ( !ep && message != WM_INITDIALOG ) return FALSE;
	
	switch ( message ) {
		case WM_INITDIALOG: {
		 	ep = (EditPatchMod *)lParam;
		 	ep->hSelectPanel = hDlg;
			SetWindowLong( hDlg, GWL_USERDATA, (LONG)ep );		 	
			// Set up the editing level selector
			LoadImages();
			iToolbar = GetICustToolbar(GetDlgItem(hDlg,IDC_SELTYPE));
			iToolbar->SetImage(hFaceImages);
			iToolbar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,0,3,0,3,24,23,24,23,EP_VERTEX));
			iToolbar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,1,4,1,4,24,23,24,23,EP_EDGE));
			iToolbar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,2,5,2,5,24,23,24,23,EP_PATCH));
			ReleaseICustToolbar(iToolbar);
			ep->RefreshSelType();
			CheckDlgButton( hDlg, IDC_DISPLATTICE, ep->displayLattice);
//			CheckDlgButton( hDlg, IDC_DISPSURFACE, ep->displaySurface);
			CheckDlgButton( hDlg, IDC_FILTVERTS, filterVerts);
			CheckDlgButton( hDlg, IDC_FILTVECS, filterVecs);
			CheckDlgButton( hDlg, IDC_LOCK_HANDLES, lockedHandles);
			ep->SetSelDlgEnables();
		 	return TRUE;
			}

		case WM_DESTROY:
			// Don't leave in one of our modes!
			ep->ip->ClearPickMode();
			CancelEditPatchModes(ep->ip);
			return FALSE;
		
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:   			
   			ep->ip->RollupMouseMessage(hDlg,message,wParam,lParam);
			return FALSE;		
		
		case WM_COMMAND: {
			BOOL needRedraw = FALSE;
			switch ( LOWORD(wParam) ) {				
				case EP_VERTEX:
					if (ep->GetSubobjectLevel() == EP_VERTEX)
						ep->ip->SetSubObjectLevel (PO_OBJECT);
					else ep->ip->SetSubObjectLevel (EP_VERTEX);
					needRedraw = TRUE;
					break;

				case EP_EDGE:
					if (ep->GetSubobjectLevel() == EP_EDGE)
						ep->ip->SetSubObjectLevel (PO_OBJECT);
					else ep->ip->SetSubObjectLevel (EP_EDGE);
					needRedraw = TRUE;
					break;

				case EP_PATCH:
					if (ep->GetSubobjectLevel() == EP_PATCH)
						ep->ip->SetSubObjectLevel (PO_OBJECT);
					else ep->ip->SetSubObjectLevel (EP_PATCH);
					needRedraw = TRUE;
					break;

				case IDC_DISPLATTICE:
					ep->SetDisplayLattice(IsDlgButtonChecked(hDlg, IDC_DISPLATTICE));
					needRedraw = TRUE;
					break;
				case IDC_DISPSURFACE:
					ep->SetDisplaySurface(IsDlgButtonChecked(hDlg, IDC_DISPSURFACE));
					needRedraw = TRUE;
					break;
				case IDC_FILTVERTS:
					filterVerts = IsDlgButtonChecked(hDlg, IDC_FILTVERTS);
					EnableWindow(GetDlgItem(hDlg,IDC_FILTVECS), filterVerts ? TRUE : FALSE);
					SetVertFilter();
					break;
				case IDC_FILTVECS:
					filterVecs = IsDlgButtonChecked(hDlg, IDC_FILTVECS);
					EnableWindow(GetDlgItem(hDlg,IDC_FILTVERTS), filterVecs ? TRUE : FALSE);
					SetVertFilter();
					break;
				case IDC_LOCK_HANDLES:
					lockedHandles = IsDlgButtonChecked( hDlg, IDC_LOCK_HANDLES);
					break;
				case IDC_NS_COPY:
					ep->NSCopy();
					break;
				case IDC_NS_PASTE:
					ep->NSPaste();
					break;
				}
			if(needRedraw) {
				ep->NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
				ep->ip->RedrawViews(ep->ip->GetTime(),REDRAW_NORMAL);
				}
			}
			break;
		case WM_NOTIFY:
			if(((LPNMHDR)lParam)->code == TTN_NEEDTEXT) {
				LPTOOLTIPTEXT lpttt;
				lpttt = (LPTOOLTIPTEXT)lParam;				
				switch (lpttt->hdr.idFrom) {
				case EP_VERTEX:
					lpttt->lpszText = GetString (IDS_TH_VERTEX);
					break;
				case EP_EDGE:
					lpttt->lpszText = GetString (IDS_TH_EDGE);
					break;
				case EP_PATCH:
					lpttt->lpszText = GetString(IDS_TH_PATCH);
					break;
				}
			}
			break;

		}
	
	return FALSE;
	}

static void SetSmoothButtonState (HWND hWnd,DWORD bits,DWORD invalid,DWORD unused=0) {
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
	switch (msg) {
	case WM_INITDIALOG:
		param = (DWORD*)lParam;
		int i;
		for (i=IDC_SMOOTH_GRP1; i<IDC_SMOOTH_GRP1+32; i++)
			SendMessage(GetDlgItem(hWnd,i),CC_COMMAND,CC_CMD_SET_TYPE,CBT_CHECK);
		SetSmoothButtonState(hWnd,param[0],0,param[2]);
		CheckDlgButton(hWnd,IDC_CLEARSELECTION,param[1]);
		CenterWindow(hWnd,GetParent(hWnd));
		break;

	case WM_COMMAND: 
		if (LOWORD(wParam)>=IDC_SMOOTH_GRP1 &&
			LOWORD(wParam)<=IDC_SMOOTH_GRP32) {
			ICustButton *iBut = GetICustButton(GetDlgItem(hWnd,LOWORD(wParam)));				
			int shift = LOWORD(wParam) - IDC_SMOOTH_GRP1;				
			if (iBut->IsChecked()) {
				param[0] |= 1<<shift;
			} else {
				param[0] &= ~(1<<shift);
			}				
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

	default:
		return FALSE;
	}
	return TRUE;
}

static BOOL CALLBACK SelectByMatDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	static int *param;
	switch (msg) {
		case WM_INITDIALOG:
			param = (int*)lParam;
			SetupIntSpinner(hWnd,IDC_MAT_IDSPIN,IDC_MAT_ID,1,MAX_MATID,param[0]);			
			CheckDlgButton(hWnd,IDC_CLEARSELECTION,param[1]);
			CenterWindow(hWnd,GetParent(hWnd));
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: {
					ISpinnerControl *spin = GetISpinner(GetDlgItem(hWnd,IDC_MAT_IDSPIN));
					param[0] = spin->GetIVal();
					param[1] = IsDlgButtonChecked(hWnd,IDC_CLEARSELECTION);
					ReleaseISpinner(spin);
					EndDialog(hWnd,1);					
					break;
					}

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

BOOL CALLBACK PatchOpsDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
	{
	EditPatchMod *ep = (EditPatchMod *)GetWindowLong( hDlg, GWL_USERDATA );
	if ( !ep && message != WM_INITDIALOG ) return FALSE;

	
	ISpinnerControl *spin;
	ICustButton *ebut;

	switch ( message ) {
		case WM_INITDIALOG: {

		 	ep = (EditPatchMod *)lParam;
		 	ep->hOpsPanel = hDlg;
			for (int i=IDC_SMOOTH_GRP1; i<IDC_SMOOTH_GRP1+32; i++)
				SendMessage(GetDlgItem(hDlg,i),CC_COMMAND,CC_CMD_SET_TYPE,CBT_CHECK);
			SetWindowLong( hDlg, GWL_USERDATA, (LONG)ep );		 	
			ICustButton *but = GetICustButton(GetDlgItem(hDlg,IDC_ATTACH));
			but->SetHighlightColor(GREEN_WASH);
			but->SetType(CBT_CHECK);
			ReleaseICustButton(but);
			CheckDlgButton( hDlg, IDC_ATTACHREORIENT, attachReorient);
			CheckDlgButton( hDlg, IDC_DETACHCOPY, patchDetachCopy);
			CheckDlgButton( hDlg, IDC_DETACHREORIENT, patchDetachReorient);
			CheckDlgButton( hDlg, IDC_PROPAGATE, ep->GetPropagate());
		 	ep->stepsSpin = GetISpinner(GetDlgItem(hDlg,IDC_STEPSSPINNER));
			ep->stepsSpin->SetLimits( 0, 100, FALSE );
			ep->stepsSpin->LinkToEdit( GetDlgItem(hDlg,IDC_STEPS), EDITTYPE_POS_INT );
			ep->stepsSpin->SetValue(ep->GetMeshSteps(),FALSE);

//3-18-99 to suport render steps and removal of the mental tesselator
		 	ep->stepsRenderSpin = GetISpinner(GetDlgItem(hDlg,IDC_STEPSRENDERSPINNER));
			ep->stepsRenderSpin->SetLimits( 0, 100, FALSE );
			ep->stepsRenderSpin->LinkToEdit( GetDlgItem(hDlg,IDC_STEPS_RENDER), EDITTYPE_POS_INT );
			ep->stepsRenderSpin->SetValue(ep->GetMeshStepsRender(),FALSE);
			CheckDlgButton( hDlg, IDC_SHOW_INTERIOR_FACES, ep->GetShowInterior());

		 	ep->weldSpin = GetISpinner(GetDlgItem(hDlg,IDC_THRESHSPINNER));
			ep->weldSpin->SetLimits( 0, 999999, FALSE );
			ep->weldSpin->LinkToEdit( GetDlgItem(hDlg,IDC_WELDTHRESH), EDITTYPE_UNIVERSE );
			ep->weldSpin->SetValue(weldThreshold,FALSE);

			CheckDlgButton( hDlg, IDC_EM_EXTYPE_B, TRUE);
			CheckDlgButton( hDlg, IDC_EP_SM_SMOOTH, TRUE);
			CheckDlgButton( hDlg, IDC_EP_SM_SMOOTH4, TRUE);

			ep->inExtrude  = FALSE;
			ep->inBevel  = FALSE;

		// Set up spinners
			spin = GetISpinner(GetDlgItem(hDlg,IDC_EP_EXTRUDESPINNER));
			spin->SetLimits(-9999999, 9999999, FALSE);
			spin->LinkToEdit (GetDlgItem (hDlg,IDC_EP_EXTRUDEAMOUNT), EDITTYPE_UNIVERSE);
			ReleaseISpinner (spin);

			spin = GetISpinner(GetDlgItem(hDlg,IDC_EP_OUTLINESPINNER));
			spin->SetLimits(-9999999, 9999999, FALSE);
			spin->LinkToEdit (GetDlgItem (hDlg,IDC_EP_OUTLINEAMOUNT), EDITTYPE_UNIVERSE);
			ReleaseISpinner (spin);


			ebut = GetICustButton(GetDlgItem(hDlg,IDC_EP_EXTRUDE));
			ebut->SetType(CBT_CHECK);
			ebut->SetHighlightColor(GREEN_WASH);
			ReleaseICustButton(ebut);

			ebut = GetICustButton(GetDlgItem(hDlg,IDC_EP_BEVEL));
			ebut->SetType(CBT_CHECK);
			ebut->SetHighlightColor(GREEN_WASH);
			ReleaseICustButton(ebut);

			ebut = GetICustButton(GetDlgItem(hDlg,IDC_BIND));
			ebut->SetType(CBT_CHECK);
			ebut->SetHighlightColor(GREEN_WASH);
			ReleaseICustButton(ebut);


			ep->matSpin = SetupIntSpinner(hDlg,IDC_MAT_IDSPIN,IDC_MAT_ID,1,MAX_MATID,0);
		 	ep->SetOpsDlgEnables();
			return TRUE;
			}

		case WM_DESTROY:
			if( ep->weldSpin ) {
				ReleaseISpinner(ep->weldSpin);
				ep->weldSpin = NULL;
				}
			if( ep->stepsSpin ) {
				ReleaseISpinner(ep->stepsSpin);
				ep->stepsSpin = NULL;
				}
//3-18-99 to suport render steps and removal of the mental tesselator
			if( ep->stepsRenderSpin ) {
				ReleaseISpinner(ep->stepsRenderSpin);
				ep->stepsRenderSpin = NULL;
				}

			// Don't leave in one of our modes!
			ep->ip->ClearPickMode();
			CancelEditPatchModes(ep->ip);
			ep->ip->UnRegisterDeleteUser(&pDel);
			ep->ip->GetRightClickMenuManager()->Unregister(&pMenu);
			return FALSE;
		
		case CC_SPINNER_CHANGE:
			switch ( LOWORD(wParam) ) {
				case IDC_STEPSSPINNER:
					ep->SetMeshSteps(ep->stepsSpin->GetIVal());
					ep->NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
					ep->ip->RedrawViews(ep->ip->GetTime(),REDRAW_NORMAL);
					break;
				case IDC_STEPSRENDERSPINNER:
					ep->SetMeshStepsRender(ep->stepsRenderSpin->GetIVal());
					break;

				case IDC_THRESHSPINNER:
					weldThreshold = ep->weldSpin->GetFVal();
					break;
				case IDC_EP_EXTRUDESPINNER:
					{
					bool enterKey;
					enterKey = FALSE;
					if (!HIWORD(wParam) && !ep->inExtrude) {
						enterKey = TRUE;
						ep->BeginExtrude(ep->ip->GetTime());
						}
					BOOL ln = IsDlgButtonChecked(hDlg,IDC_EM_EXTYPE_B);
					spin = GetISpinner(GetDlgItem(hDlg,IDC_EP_EXTRUDESPINNER));

					ep->Extrude (ep->ip->GetTime(),spin->GetFVal(),ln);
					if (enterKey) {
						ep->EndExtrude (ep->ip->GetTime(),TRUE);
						spin = GetISpinner(GetDlgItem(hDlg,IDC_EP_EXTRUDESPINNER));
						if (spin) {
							spin->SetValue(0,FALSE);
							ReleaseISpinner(spin);
							}

						ep->ip->RedrawViews (ep->ip->GetTime(), REDRAW_END);
						} else {
						ep->ip->RedrawViews (ep->ip->GetTime(),REDRAW_INTERACTIVE);
						}
					break;
					}
				case IDC_EP_OUTLINESPINNER:
					{
					bool enterKey;
					enterKey = FALSE;
					if (!HIWORD(wParam) && !ep->inBevel) {
						enterKey = TRUE;
						ep->BeginBevel (ep->ip->GetTime ());
						}
					int sm =0;
					int sm2 = 0;
					if (IsDlgButtonChecked(hDlg,IDC_EP_SM_SMOOTH)) sm = 0;					
					else if (IsDlgButtonChecked(hDlg,IDC_EP_SM_SMOOTH2)) sm = 1;					
					else if (IsDlgButtonChecked(hDlg,IDC_EP_SM_SMOOTH3)) sm = 2;					

					if (IsDlgButtonChecked(hDlg,IDC_EP_SM_SMOOTH4)) sm2 = 0;					
					else if (IsDlgButtonChecked(hDlg,IDC_EP_SM_SMOOTH5)) sm2 = 1;					
					else if (IsDlgButtonChecked(hDlg,IDC_EP_SM_SMOOTH6)) sm2 = 2;					

					spin = GetISpinner(GetDlgItem(hDlg,IDC_EP_OUTLINESPINNER));
					ep->Bevel (ep->ip->GetTime (), spin->GetFVal (),sm,sm2);
					if (enterKey) {
						ep->EndBevel (ep->ip->GetTime (), TRUE);
						spin = GetISpinner(GetDlgItem(hDlg,IDC_EP_OUTLINESPINNER));
						if (spin) {
							spin->SetValue(0,FALSE);
							ReleaseISpinner(spin);
							}

						ep->ip->RedrawViews (ep->ip->GetTime(), REDRAW_END);
						} else {
						ep->ip->RedrawViews (ep->ip->GetTime(),REDRAW_INTERACTIVE);
						}
					break;
					}

				}
			break;
		case CC_SPINNER_BUTTONDOWN:
			switch (LOWORD(wParam)) {
			case IDC_EP_EXTRUDESPINNER:
				ep->BeginExtrude (ep->ip->GetTime());
				break;
			case IDC_EP_OUTLINESPINNER:
				ep->BeginBevel (ep->ip->GetTime ());
				break;
			}
			break;

		case CC_SPINNER_BUTTONUP:
			switch( LOWORD(wParam) ) {
				case IDC_EP_EXTRUDESPINNER:
					ep->EndExtrude (ep->ip->GetTime(), HIWORD(wParam));
					spin = GetISpinner(GetDlgItem(hDlg,IDC_EP_EXTRUDESPINNER));
					if (spin) {
						spin->SetValue(0,FALSE);
						ReleaseISpinner(spin);
						}

					ep->ip->RedrawViews (ep->ip->GetTime(),REDRAW_END);
					break;
				case IDC_EP_OUTLINESPINNER:
					ep->EndBevel (ep->ip->GetTime(), HIWORD(wParam));
					spin = GetISpinner(GetDlgItem(hDlg,IDC_EP_OUTLINESPINNER));
					if (spin) {
						spin->SetValue(0,FALSE);
						ReleaseISpinner(spin);
						}

					ep->ip->RedrawViews (ep->ip->GetTime(),REDRAW_END);
					break;


				}
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:   			
   			ep->ip->RollupMouseMessage(hDlg,message,wParam,lParam);
			return FALSE;		
		
		case WM_COMMAND:			
			switch ( LOWORD(wParam) ) {				
				// Subdivision
//watje 3-18-99
				case IDC_SHOW_INTERIOR_FACES:
						ep->SetShowInterior(IsDlgButtonChecked(hDlg, IDC_SHOW_INTERIOR_FACES));
//						ep->InvalidateMesh();
//						ep->NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
//						ep->ip->RedrawViews (ep->ip->GetTime(),REDRAW_END);
						break;

//watje 12-10-98
				case IDC_HIDE:
					ep->DoHide(ep->GetSubobjectLevel());
					break;
				case IDC_UNHIDE:
					ep->DoUnHide();
					break;
				case IDC_BIND:
//			ep->DoAddHook();
					if (ep->ip->GetCommandMode()==ep->bindMode)
						ep->ip->SetStdCommandMode(CID_OBJMOVE);
					else ep->ip->SetCommandMode(ep->bindMode);
					break;

					break;
				case IDC_UNBIND:
					ep->DoRemoveHook();
					break;
//extrude and bevel stuff
//watje 12-10-98
				case IDC_EP_SM_SMOOTH:
					CheckDlgButton( hDlg, IDC_EP_SM_SMOOTH2, FALSE);
					CheckDlgButton( hDlg, IDC_EP_SM_SMOOTH3, FALSE);
					break;
				case IDC_EP_SM_SMOOTH2:
					CheckDlgButton( hDlg, IDC_EP_SM_SMOOTH, FALSE);
					CheckDlgButton( hDlg, IDC_EP_SM_SMOOTH3, FALSE);
					break;
				case IDC_EP_SM_SMOOTH3:
					CheckDlgButton( hDlg, IDC_EP_SM_SMOOTH2, FALSE);
					CheckDlgButton( hDlg, IDC_EP_SM_SMOOTH, FALSE);
					break;

				case IDC_EP_SM_SMOOTH4:
					CheckDlgButton( hDlg, IDC_EP_SM_SMOOTH5, FALSE);
					CheckDlgButton( hDlg, IDC_EP_SM_SMOOTH6, FALSE);
					break;
				case IDC_EP_SM_SMOOTH5:
					CheckDlgButton( hDlg, IDC_EP_SM_SMOOTH4, FALSE);
					CheckDlgButton( hDlg, IDC_EP_SM_SMOOTH6, FALSE);
					break;
				case IDC_EP_SM_SMOOTH6:
					CheckDlgButton( hDlg, IDC_EP_SM_SMOOTH4, FALSE);
					CheckDlgButton( hDlg, IDC_EP_SM_SMOOTH5, FALSE);
					break;


				case IDC_EP_EXTRUDE:
					if (ep->ip->GetCommandMode()==ep->extrudeMode)
						ep->ip->SetStdCommandMode(CID_OBJMOVE);
					else ep->ip->SetCommandMode(ep->extrudeMode);
					break;
				case IDC_EP_BEVEL:
					if (ep->ip->GetCommandMode()==ep->bevelMode)
						ep->ip->SetStdCommandMode(CID_OBJMOVE);
					else ep->ip->SetCommandMode(ep->bevelMode);
					break;


				case IDC_SUBDIVIDE:
					ep->DoSubdivide(ep->GetSubobjectLevel());
					break;
				case IDC_PROPAGATE:
					ep->SetPropagate(IsDlgButtonChecked(hDlg, IDC_PROPAGATE));
					break;
				// Topology
				case IDC_ADDTRI:
					if(ep->GetSubobjectLevel() == PO_EDGE)
						ep->DoPatchAdd(PATCH_TRI);
					break;
				case IDC_ADDQUAD:
					if(ep->GetSubobjectLevel() == PO_EDGE)
						ep->DoPatchAdd(PATCH_QUAD);
					break;
				case IDC_WELD:
					ep->DoVertWeld();
					break;
				case IDC_DETACH:
					ep->DoPatchDetach(patchDetachCopy, patchDetachReorient);
					break;
				case IDC_DETACHCOPY:
					patchDetachCopy = IsDlgButtonChecked( hDlg, IDC_DETACHCOPY);
					break;
				case IDC_DETACHREORIENT:
					patchDetachReorient = IsDlgButtonChecked( hDlg, IDC_DETACHREORIENT);
					break;
				case IDC_ATTACH: {
					ModContextList mcList;
					INodeTab nodes;
					// If the mode is on, turn it off and bail
					if (ep->ip->GetCommandMode()->ID() == CID_STDPICK) {
						ep->ip->SetStdCommandMode(CID_OBJMOVE);
						return FALSE;
						}
					// Want to turn on the mode.  Make sure we're valid first
					ep->ip->GetModContexts(mcList,nodes);
					ep->pickCB.ep = ep;
					ep->ip->SetPickMode(&ep->pickCB);
					nodes.DisposeTemporary();
					break;
					}
				case IDC_ATTACHREORIENT:
					attachReorient = IsDlgButtonChecked( hDlg, IDC_ATTACHREORIENT);
					break;
				case IDC_PATCH_DELETE:
					ep->DoDeleteSelected();
					break;
				}
			break;
		}
	
	return FALSE;
	}

BOOL CALLBACK PatchSurfDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
	{
	EditPatchMod *ep = (EditPatchMod *)GetWindowLong( hDlg, GWL_USERDATA );
	if ( !ep && message != WM_INITDIALOG ) return FALSE;
	
	switch ( message ) {
		case WM_INITDIALOG: {

		 	ep = (EditPatchMod *)lParam;
		 	ep->hSurfPanel = hDlg;
			for (int i=IDC_SMOOTH_GRP1; i<IDC_SMOOTH_GRP1+32; i++)
				SendMessage(GetDlgItem(hDlg,i),CC_COMMAND,CC_CMD_SET_TYPE,CBT_CHECK);
			SetWindowLong( hDlg, GWL_USERDATA, (LONG)ep );		 	
			ep->matSpin = SetupIntSpinner(hDlg,IDC_MAT_IDSPIN,IDC_MAT_ID,1,MAX_MATID,0);
		 	ep->SetSurfDlgEnables();
			return TRUE;
			}

		case WM_DESTROY:
			if( ep->matSpin ) {
				ReleaseISpinner(ep->matSpin);
				ep->matSpin = NULL;
				}
			return FALSE;
		
		case CC_SPINNER_CHANGE:
			switch ( LOWORD(wParam) ) {
				case IDC_MAT_IDSPIN: 
					if(HIWORD(wParam))
						break;		// No interactive action
					ep->SetSelMatIndex(ep->matSpin->GetIVal()-1);
					break;
				}
			break;

		case CC_SPINNER_BUTTONUP:
			switch( LOWORD(wParam) ) {
				case IDC_MAT_IDSPIN:
					ep->SetSelMatIndex(ep->matSpin->GetIVal()-1);
					ep->ip->RedrawViews(ep->ip->GetTime(),REDRAW_END);
					break;
				}
			break;

		case WM_PAINT:
			if (!ep->patchUIValid) {
				// Material index
				int mat = ep->GetSelMatIndex();
				if (mat == -1) {
					ep->matSpin->SetIndeterminate(TRUE);
				} else {
					ep->matSpin->SetIndeterminate(FALSE);
					ep->matSpin->SetValue(mat+1,FALSE);
					}
				// Smoothing groups
				DWORD invalid, bits;
				bits = ep->GetSelSmoothBits(invalid);
				SetSmoothButtonState(hDlg,bits,invalid);

				ep->patchUIValid = TRUE;
				}
			return FALSE;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:   			
   			ep->ip->RollupMouseMessage(hDlg,message,wParam,lParam);
			return FALSE;		
		
		case WM_COMMAND:			
			if (LOWORD(wParam)>=IDC_SMOOTH_GRP1 &&
				LOWORD(wParam)<=IDC_SMOOTH_GRP32) {
				ICustButton *iBut = GetICustButton(GetDlgItem(hDlg,LOWORD(wParam)));
				int bit = iBut->IsChecked() ? 1 : 0;
				int shift = LOWORD(wParam) - IDC_SMOOTH_GRP1;
				ep->SetSelSmoothBits(bit<<shift,1<<shift);
				ReleaseICustButton(iBut);
				break;
			}
			switch ( LOWORD(wParam) ) {				
				// Material
				case IDC_SELECT_BYID: {										
					if (DialogBoxParam(
						hInstance, 
						MAKEINTRESOURCE(IDD_SELECTBYMAT),
						ep->ip->GetMAXHWnd(), 
						SelectByMatDlgProc,
						(LPARAM)sbmParams)) {
					
						ep->SelectByMat(sbmParams[0]-1/*index*/,sbmParams[1]/*clear*/);
						}
					break;
					}
				// Smoothing groups
				case IDC_SELECTBYSMOOTH: {										
					sbsParams[2] = ~ep->GetUsedSmoothBits();
					if (DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EM_SELECTBYSMOOTH),
								ep->ip->GetMAXHWnd(), SelectBySmoothDlgProc, (LPARAM)sbsParams)) {
						ep->SelectBySmoothGroup(sbsParams[0],(BOOL)sbsParams[1]);
					}
					break;
					}
				case IDC_SMOOTH_CLEAR:
					ep->SetSelSmoothBits(0,0xffffffff);
					break;
				}
			break;
		}
	
	return FALSE;
	}

BOOL CALLBACK PatchObjSurfDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
	{
	EditPatchMod *ep = (EditPatchMod *)GetWindowLong( hDlg, GWL_USERDATA );
	if ( !ep && message != WM_INITDIALOG ) return FALSE;
	
	switch ( message ) {
		case WM_INITDIALOG: {

		 	ep = (EditPatchMod *)lParam;
		 	ep->hSurfPanel = hDlg;
			SetWindowLong( hDlg, GWL_USERDATA, (LONG)ep );		 	
			if (!ep->settingViewportTess  && ep->settingDisp && ep->GetProdTess().type == TESS_SET)
				ep->settingDisp = FALSE;
			TessApprox t;
			if (ep->settingViewportTess) {
				t = ep->GetViewTess();
			} else {
				if (ep->settingDisp)
					t = ep->GetDispTess();
				else
					t = ep->GetProdTess();
			}
			ep->uSpin = SetupIntSpinner( hDlg, IDC_TESS_U_SPINNER, IDC_TESS_U, 1, 100, t.u);
			ep->vSpin = SetupIntSpinner( hDlg, IDC_TESS_V_SPINNER, IDC_TESS_V, 1, 100, t.v);
#define MAX_F 1000.0f
			ep->edgeSpin = SetupFloatSpinner( hDlg, IDC_TESS_EDGE_SPINNER, IDC_TESS_EDGE, 0.0f, MAX_F, t.edge);
			ep->distSpin = SetupFloatSpinner( hDlg, IDC_TESS_DIST_SPINNER, IDC_TESS_DIST, 0.0f, MAX_F, t.dist);
			ep->angSpin =  SetupFloatSpinner( hDlg, IDC_TESS_ANG_SPINNER,  IDC_TESS_ANG, 0.0f, MAX_F, t.ang);
			ep->mergeSpin =  SetupFloatSpinner( hDlg, IDC_MERGE_SPINNER,  IDC_MERGE, 0.000f, MAX_F, t.merge);
			ep->SetTessUI(hDlg, &t);
		 	ep->SetSurfDlgEnables();
			return TRUE;
			}

		case WM_DESTROY:
			if( ep->uSpin ) {
				ReleaseISpinner(ep->uSpin);
				ep->uSpin = NULL;
				}
			if( ep->vSpin ) {
				ReleaseISpinner(ep->vSpin);
				ep->vSpin = NULL;
				}
			if( ep->edgeSpin ) {
				ReleaseISpinner(ep->edgeSpin);
				ep->edgeSpin = NULL;
				}
			if( ep->distSpin ) {
				ReleaseISpinner(ep->distSpin);
				ep->distSpin = NULL;
				}
			if( ep->angSpin ) {
				ReleaseISpinner(ep->angSpin);
				ep->angSpin = NULL;
				}
			if( ep->mergeSpin ) {
				ReleaseISpinner(ep->mergeSpin);
				ep->mergeSpin = NULL;
				}
			return FALSE;
		
		case CC_SPINNER_BUTTONUP: {
			TessApprox tess;
			if (ep->settingViewportTess) {
				tess = ep->GetViewTess();
			} else {
				if (ep->settingDisp)
					tess = ep->GetDispTess();
				else
					tess = ep->GetProdTess();
			}
			ep->SetTessUI(hDlg, &tess);
			}
			break;

		case CC_SPINNER_CHANGE:
			switch ( LOWORD(wParam) ) {
				case IDC_TESS_U_SPINNER:
				case IDC_TESS_V_SPINNER:
				case IDC_TESS_EDGE_SPINNER:
				case IDC_TESS_DIST_SPINNER:
				case IDC_TESS_ANG_SPINNER:
				case IDC_MERGE_SPINNER:
					{
					TessApprox tess;
					if (ep->settingViewportTess) {
						tess = ep->GetViewTess();
					} else {
						if (ep->settingDisp)
							tess = ep->GetDispTess();
						else
							tess = ep->GetProdTess();
					}
					switch(LOWORD(wParam) ) {
						case IDC_TESS_U_SPINNER:
							tess.u = ep->uSpin->GetIVal();
							break;
						case IDC_TESS_V_SPINNER:
							tess.v = ep->vSpin->GetIVal();
							break;
						case IDC_TESS_EDGE_SPINNER:
							tess.edge = ep->edgeSpin->GetFVal();
							break;
						case IDC_TESS_DIST_SPINNER:
							tess.dist = ep->distSpin->GetFVal();
							break;
						case IDC_TESS_ANG_SPINNER:
							tess.ang = ep->angSpin->GetFVal();
							break;
						case IDC_MERGE_SPINNER:
							tess.merge = ep->mergeSpin->GetFVal();
							break;
						}
					if (ep->settingViewportTess) {
						ep->SetViewTess(tess);
					} else {
						if (ep->settingDisp)
							ep->SetDispTess(tess);
						else
							ep->SetProdTess(tess);
					}
					if(!HIWORD(wParam))
						ep->SetTessUI(hDlg, &tess);
					break;
					}
				}
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:   			
   			ep->ip->RollupMouseMessage(hDlg,message,wParam,lParam);
			return FALSE;		
		
		case WM_COMMAND:			
			switch ( LOWORD(wParam) ) {				
				// Tessellation
				case IDC_TESS_VIEW:	{
					ep->settingViewportTess = TRUE;
					TessApprox t = ep->GetViewTess();
					ep->SetTessUI(hDlg, &t);
					EnableWindow(GetDlgItem(hDlg, IDC_TESS_VIEW_DEP), FALSE); // always off here
					break;}
				case IDC_TESS_RENDERER: {
					ep->settingViewportTess = FALSE;
					if (ep->settingDisp) {
						TessApprox t = ep->GetDispTess();
						ep->SetTessUI(hDlg, &t);
					} else {
						TessApprox t = ep->GetProdTess();
						ep->SetTessUI(hDlg, &t);
					}
					break;}
				case IDC_MESH:
					ep->settingDisp = FALSE;
					ep->SetTessUI(hDlg, &ep->GetProdTess());
					break;
				case IDC_DISP:
					ep->settingDisp = TRUE;
					ep->SetTessUI(hDlg, &ep->GetDispTess());
					break;
//watje 12-10-98
				case IDC_SHOW_INTERIOR_FACES:
				case IDC_TESS_SET:
				case IDC_TESS_REGULAR:
				case IDC_TESS_PARAM:
				case IDC_TESS_SPATIAL:
				case IDC_TESS_CURV:
				case IDC_TESS_LDA:
					{
					TessApprox tess;
					if (ep->settingViewportTess) {
						tess = ep->GetViewTess();
					} else {
						if (ep->settingDisp)
							tess = ep->GetDispTess();
						else
							tess = ep->GetProdTess();
					}
					switch (LOWORD(wParam)) {
//watje 12-10-98
					case IDC_SHOW_INTERIOR_FACES:
						tess.showInteriorFaces = IsDlgButtonChecked(hDlg, IDC_SHOW_INTERIOR_FACES);
						break;
					case IDC_TESS_SET:
						tess.type = TESS_SET;
						EnableWindow(GetDlgItem(hDlg, IDC_SHOW_INTERIOR_FACES), TRUE);
						break;
					case IDC_TESS_REGULAR:
						tess.type = TESS_REGULAR;
						EnableWindow(GetDlgItem(hDlg, IDC_SHOW_INTERIOR_FACES), TRUE);
						break;
					case IDC_TESS_PARAM:
						tess.type = TESS_PARAM;
						EnableWindow(GetDlgItem(hDlg, IDC_SHOW_INTERIOR_FACES), TRUE);
						break;
					case IDC_TESS_SPATIAL:
						tess.type = TESS_SPATIAL;
						EnableWindow(GetDlgItem(hDlg, IDC_SHOW_INTERIOR_FACES), TRUE);
						break;
					case IDC_TESS_CURV:
						tess.type = TESS_CURVE;
						EnableWindow(GetDlgItem(hDlg, IDC_SHOW_INTERIOR_FACES), TRUE);
						break;
					case IDC_TESS_LDA:
						tess.type = TESS_LDA;
						EnableWindow(GetDlgItem(hDlg, IDC_SHOW_INTERIOR_FACES), TRUE);
						break;
					}
					if (ep->settingViewportTess) {
						ep->SetViewTess(tess);
					} else {
						if (ep->settingDisp)
							ep->SetDispTess(tess);
						else
							ep->SetProdTess(tess);
					}
					ep->SetTessUI(hDlg, &tess);
					}
					break;
				case IDC_TESS_VIEW_DEP: {
					TessApprox tess;
					tess = ep->GetProdTess();
					tess.view = IsDlgButtonChecked(hDlg, IDC_TESS_VIEW_DEP);
					if (ep->settingDisp)
						ep->SetDispTess(tess);
					else
						ep->SetProdTess(tess);
					}
					break;
				case IDC_TESS_NORMALS:
					if (ep->settingViewportTess) {
						ep->SetViewTessNormals(IsDlgButtonChecked(hDlg, IDC_TESS_NORMALS));
						ep->SetTessUI(hDlg, &ep->GetViewTess());
					} else {
						ep->SetProdTessNormals(IsDlgButtonChecked(hDlg, IDC_TESS_NORMALS));
						if (ep->settingDisp)
							ep->SetTessUI(hDlg, &ep->GetDispTess());
						else
							ep->SetTessUI(hDlg, &ep->GetProdTess());
					}
					break;
				case IDC_WELDTESS:
					if (ep->settingViewportTess) {
						ep->SetViewTessWeld(IsDlgButtonChecked(hDlg, IDC_WELDTESS));
						ep->SetTessUI(hDlg, &ep->GetViewTess());
					} else {
						ep->SetProdTessWeld(IsDlgButtonChecked(hDlg, IDC_WELDTESS));
						if (ep->settingDisp)
							ep->SetTessUI(hDlg, &ep->GetDispTess());
						else
							ep->SetTessUI(hDlg, &ep->GetProdTess());
					}
					break;
				case IDC_ADVANCED_PARAMETERS: {
					TessApprox tess;
					if (ep->settingViewportTess) {
						tess = ep->GetViewTess();
					} else {
						if (ep->settingDisp)
							tess = ep->GetDispTess();
						else
							tess = ep->GetProdTess();
					}
					sParams.mStyle = tess.subdiv;
					sParams.mMin = tess.minSub;
					sParams.mMax = tess.maxSub;
					sParams.mTris = tess.maxTris;
					int retval = DialogBox( hInstance,
								MAKEINTRESOURCE(IDD_SURF_APPROX_ADV),
								ep->ip->GetMAXHWnd(), AdvParametersDialogProc);
					if (retval == 1) {
						BOOL confirm = FALSE;
						if ((sParams.mStyle == SUBDIV_DELAUNAY && sParams.mTris > 200000) ||
							(sParams.mStyle != SUBDIV_DELAUNAY && sParams.mMax > 5)) {
							// warning!
							TSTR title = GetString(IDS_ADV_SURF_APPROX_WARNING_TITLE),
								warning = GetString(IDS_ADV_SURF_APPROX_WARNING);
							if (MessageBox(hDlg, warning, title,
								MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2 ) == IDYES)
								confirm = TRUE;
 
						} else
							confirm = TRUE;
						if (confirm) {
							// do it, they've been warned!
							tess.subdiv = sParams.mStyle;
							tess.minSub = sParams.mMin;
							tess.maxSub = sParams.mMax;
							tess.maxTris = sParams.mTris;
							if (ep->settingViewportTess) {
								ep->SetViewTess(tess);
							} else {
								if (ep->settingDisp)
									ep->SetDispTess(tess);
								else
									ep->SetProdTess(tess);
							}
						}
					}
					break;
					}
				}
			break;
		}
	
	return FALSE;
	}

// Advanced TessApprox settings...

static ISpinnerControl* psMinSpin = NULL;
static ISpinnerControl* psMaxSpin = NULL;
static ISpinnerControl* psMaxTrisSpin = NULL;
// this max matches the MI max.
#define MAX_SUBDIV 7


static BOOL initing = FALSE; // this is a hack but CenterWindow causes bad commands

BOOL CALLBACK
AdvParametersDialogProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch (uMsg) {
    case WM_INITDIALOG: {
		initing = TRUE;
        CenterWindow(hDlg, GetCOREInterface()->GetMAXHWnd());
		initing = FALSE;
		psMinSpin = SetupIntSpinner( hDlg, IDC_TESS_MIN_REC_SPINNER, IDC_TESS_MIN_REC, 0, sParams.mMax, sParams.mMin);
		psMaxSpin = SetupIntSpinner( hDlg, IDC_TESS_MAX_REC_SPINNER, IDC_TESS_MAX_REC, sParams.mMin, MAX_SUBDIV, sParams.mMax);
		psMaxTrisSpin = SetupIntSpinner( hDlg, IDC_TESS_MAX_TRIS_SPINNER, IDC_TESS_MAX_TRIS, 0, 2000000, sParams.mTris);
		switch (sParams.mStyle) {
		case SUBDIV_GRID:
			CheckDlgButton( hDlg, IDC_GRID, TRUE);
			CheckDlgButton( hDlg, IDC_TREE, FALSE);
			CheckDlgButton( hDlg, IDC_DELAUNAY, FALSE);
			break;
		case SUBDIV_TREE:
			CheckDlgButton( hDlg, IDC_GRID, FALSE);
			CheckDlgButton( hDlg, IDC_TREE, TRUE);
			CheckDlgButton( hDlg, IDC_DELAUNAY, FALSE);
			break;
		case SUBDIV_DELAUNAY:
			CheckDlgButton( hDlg, IDC_GRID, FALSE);
			CheckDlgButton( hDlg, IDC_TREE, FALSE);
			CheckDlgButton( hDlg, IDC_DELAUNAY, TRUE);
			break;
		}
		break; }

    case WM_COMMAND:
		if (initing) return FALSE;
		switch ( LOWORD(wParam) ) {
		case IDOK:
			EndDialog(hDlg, 1);
			break;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			break;
		case IDC_GRID:
			sParams.mStyle = SUBDIV_GRID;
			CheckDlgButton( hDlg, IDC_GRID, TRUE);
			CheckDlgButton( hDlg, IDC_TREE, FALSE);
			CheckDlgButton( hDlg, IDC_DELAUNAY, FALSE);
			break;
		case IDC_TREE:
			sParams.mStyle = SUBDIV_TREE;
			CheckDlgButton( hDlg, IDC_GRID, FALSE);
			CheckDlgButton( hDlg, IDC_TREE, TRUE);
			CheckDlgButton( hDlg, IDC_DELAUNAY, FALSE);
			break;
		case IDC_DELAUNAY:
			sParams.mStyle = SUBDIV_DELAUNAY;
			CheckDlgButton( hDlg, IDC_GRID, FALSE);
			CheckDlgButton( hDlg, IDC_TREE, FALSE);
			CheckDlgButton( hDlg, IDC_DELAUNAY, TRUE);
			break;
		}
		break;

    case CC_SPINNER_CHANGE:
		switch ( LOWORD(wParam) ) {
		case IDC_TESS_MIN_REC_SPINNER:
			sParams.mMin = psMinSpin->GetIVal();
			psMinSpin->SetLimits(0, sParams.mMax, FALSE);
			psMaxSpin->SetLimits(sParams.mMin, MAX_SUBDIV, FALSE);
			break;
		case IDC_TESS_MAX_REC_SPINNER:
			sParams.mMax = psMaxSpin->GetIVal();
			psMinSpin->SetLimits(0, sParams.mMax, FALSE);
			psMaxSpin->SetLimits(sParams.mMin, MAX_SUBDIV, FALSE);
			break;
		case IDC_TESS_MAX_TRIS_SPINNER:
			sParams.mTris = psMaxTrisSpin->GetIVal();
			break;
		}
		break;

	case WM_DESTROY:
		if( psMinSpin ) {
			ReleaseISpinner(psMinSpin);
			psMinSpin = NULL;
		}
		if( psMaxSpin ) {
			ReleaseISpinner(psMaxSpin);
			psMaxSpin = NULL;
		}
		if( psMaxTrisSpin ) {
			ReleaseISpinner(psMaxTrisSpin);
			psMaxTrisSpin = NULL;
		}
		break;
	}

	return FALSE;
}

#define OLD_SEL_LEVEL_CHUNK 0x1000	// Original backwards ordering
#define SEL_LEVEL_CHUNK 0x1001
#define DISP_LATTICE_CHUNK 0x1010
#define DISP_SURFACE_CHUNK 0x1020
#define DISP_VERTS_CHUNK 0x1030
#define EPM_MESH_ATTRIB_CHUNK	0x1040
#define EPM_VTESS_ATTRIB_CHUNK	0x1090
#define EPM_PTESS_ATTRIB_CHUNK	0x10a0
#define EPM_DTESS_ATTRIB_CHUNK	0x10b0
#define EPM_NORMAL_TESS_ATTRIB_CHUNK	0x10c0
#define EPM_WELD_TESS_ATTRIB_CHUNK	0x10d0
#define EPM_RENDERSTEPS_CHUNK		0x10e0
#define EPM_SHOWINTERIOR_CHUNK		0x10f0
// The following chunk is written on r3 and later files
// If not present, named selection data structures need fixup
#define EPM_SEL_NAMES_OK 0x1100	

// Names of named selection sets
#define NAMEDVSEL_NAMES_CHUNK	0x1050
#define NAMEDESEL_NAMES_CHUNK	0x1060
#define NAMEDPSEL_NAMES_CHUNK	0x1070
#define NAMEDSEL_STRING_CHUNK	0x1080

static int namedSelID[] = {
	NAMEDVSEL_NAMES_CHUNK,
	NAMEDESEL_NAMES_CHUNK,
	NAMEDPSEL_NAMES_CHUNK};


IOResult EditPatchMod::Save(ISave *isave) {
	Modifier::Save(isave);
	Interval valid;
	ULONG nb;
	// In r3 and later, if the named sel names are OK, write this chunk
	if(!namedSelNeedsFixup) {
		isave->BeginChunk(EPM_SEL_NAMES_OK);
		isave->EndChunk();
		}
	isave->BeginChunk(SEL_LEVEL_CHUNK);
	isave->Write(&selLevel,sizeof(int),&nb);
	isave->	EndChunk();
	isave->BeginChunk(DISP_LATTICE_CHUNK);
	isave->Write(&displayLattice,sizeof(BOOL),&nb);
	isave->	EndChunk();
	isave->BeginChunk(DISP_SURFACE_CHUNK);
	isave->Write(&displaySurface,sizeof(BOOL),&nb);
	isave->	EndChunk();
	isave->BeginChunk(EPM_MESH_ATTRIB_CHUNK);
	isave->Write(&meshSteps,sizeof(int),&nb);
// Future use (Not used now)
	BOOL fakeAdaptive = FALSE;
	isave->Write(&fakeAdaptive,sizeof(BOOL),&nb);
//	isave->Write(&meshAdaptive,sizeof(BOOL),&nb);	// Future use (Not used now)
	isave->	EndChunk();

//3-18-99 to suport render steps and removal of the mental tesselator
	isave->BeginChunk(EPM_RENDERSTEPS_CHUNK);
	if ( (meshStepsRender < 0) || (meshStepsRender > 100))
		{
		meshStepsRender = 5;
		DbgAssert(0);
		}
	isave->Write(&meshStepsRender,sizeof(int),&nb);
	isave->	EndChunk();
	isave->BeginChunk(EPM_SHOWINTERIOR_CHUNK);
	isave->Write(&showInterior,sizeof(BOOL),&nb);
	isave->	EndChunk();

	isave->BeginChunk(EPM_VTESS_ATTRIB_CHUNK);
	viewTess.Save(isave);
	isave->	EndChunk();
	isave->BeginChunk(EPM_PTESS_ATTRIB_CHUNK);
	prodTess.Save(isave);
	isave->	EndChunk();
	isave->BeginChunk(EPM_DTESS_ATTRIB_CHUNK);
	dispTess.Save(isave);
	isave->	EndChunk();

	isave->BeginChunk(EPM_NORMAL_TESS_ATTRIB_CHUNK);
	isave->Write(&mViewTessNormals,sizeof(BOOL),&nb);
	isave->Write(&mProdTessNormals,sizeof(BOOL),&nb);
	isave->	EndChunk();
	isave->BeginChunk(EPM_WELD_TESS_ATTRIB_CHUNK);
	isave->Write(&mViewTessWeld,sizeof(BOOL),&nb);
	isave->Write(&mProdTessWeld,sizeof(BOOL),&nb);
	isave->	EndChunk();
	
	// Save names of named selection sets
	for (int j=0; j<3; j++) {
		if (namedSel[j].Count()) {
			isave->BeginChunk(namedSelID[j]);			
			for (int i=0; i<namedSel[j].Count(); i++) {
				isave->BeginChunk(NAMEDSEL_STRING_CHUNK);
				isave->WriteWString(*namedSel[j][i]);
				isave->EndChunk();
				}
			isave->EndChunk();
			}
		}
	return IO_OK;
	}

IOResult EditPatchMod::LoadNamedSelChunk(ILoad *iload,int level)
	{	
	IOResult res;
	
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case NAMEDSEL_STRING_CHUNK: {
				TCHAR *name;
				res = iload->ReadWStringChunk(&name);
				// Set the name in the modifier
				AddSet(TSTR(name),level+1);
				break;
				}
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

IOResult EditPatchMod::Load(ILoad *iload) {
	Modifier::Load(iload);
	IOResult res;
	ULONG nb;
	namedSelNeedsFixup = TRUE;	// Pre-r3 default
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case EPM_SEL_NAMES_OK:
				namedSelNeedsFixup = FALSE;
				break;
			case NAMEDVSEL_NAMES_CHUNK: {				
				res = LoadNamedSelChunk(iload,0);
				break;
				}
			case NAMEDESEL_NAMES_CHUNK: {
				res = LoadNamedSelChunk(iload,1);
				break;
				}
			case NAMEDPSEL_NAMES_CHUNK: {
				res = LoadNamedSelChunk(iload,2);
				break;
				}

			case OLD_SEL_LEVEL_CHUNK:	// Correct backwards ordering
				{
				short sl;
				res = iload->Read(&sl,sizeof(short),&nb);
				selLevel = sl;
				switch(selLevel) {
					case 1:
						selLevel = EP_PATCH;
						break;
					case 3:
						selLevel = EP_VERTEX;
						break;
					}
				}
				break;
			case SEL_LEVEL_CHUNK:
				res = iload->Read(&selLevel,sizeof(int),&nb);
				break;
			case DISP_LATTICE_CHUNK:
				res = iload->Read(&displayLattice,sizeof(BOOL),&nb);
				break;
			case DISP_SURFACE_CHUNK:
				res = iload->Read(&displaySurface,sizeof(BOOL),&nb);
				break;
			case DISP_VERTS_CHUNK:
				iload->SetObsolete();
				break;
			case EPM_MESH_ATTRIB_CHUNK:
				res = iload->Read(&meshSteps,sizeof(int),&nb);
				res = iload->Read(&meshAdaptive,sizeof(BOOL),&nb);
				break;
//3-18-99 to suport render steps and removal of the mental tesselator
			case EPM_RENDERSTEPS_CHUNK:
				res = iload->Read(&meshStepsRender,sizeof(int),&nb);
				if ( (meshStepsRender < 0) || (meshStepsRender > 100))
					{
					meshStepsRender = 5;
					DbgAssert(0);
					}
				break;
			case EPM_SHOWINTERIOR_CHUNK:
				res = iload->Read(&showInterior,sizeof(BOOL),&nb);
				break;

			case EPM_VTESS_ATTRIB_CHUNK:
				viewTess.Load(iload);
				break;
			case EPM_PTESS_ATTRIB_CHUNK:
				prodTess.Load(iload);
				break;
			case EPM_DTESS_ATTRIB_CHUNK:
				dispTess.Load(iload);
				break;
			case EPM_NORMAL_TESS_ATTRIB_CHUNK:
				res = iload->Read(&mViewTessNormals,sizeof(BOOL),&nb);
				res = iload->Read(&mProdTessNormals,sizeof(BOOL),&nb);
				break;
			case EPM_WELD_TESS_ATTRIB_CHUNK:
				res = iload->Read(&mViewTessWeld,sizeof(BOOL),&nb);
				res = iload->Read(&mProdTessWeld,sizeof(BOOL),&nb);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}


#define EDITPATCHDATA_CHUNK 0x1000

IOResult EditPatchMod::SaveLocalData(ISave *isave, LocalModData *ld) {
	EditPatchData *ep = (EditPatchData *)ld;

	isave->BeginChunk(EDITPATCHDATA_CHUNK);
	ep->Save(isave);
	isave->EndChunk();

	return IO_OK;
	}

IOResult EditPatchMod::LoadLocalData(ILoad *iload, LocalModData **pld) {
	IOResult res;
	EditPatchData *ep;
	if (*pld==NULL) {
		*pld =(LocalModData *) new EditPatchData(this);
		}
	ep = (EditPatchData *)*pld;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case EDITPATCHDATA_CHUNK:
				res = ep->Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}

// ------------------------------------------------------

int EPM_ExtrudeMouseProc::proc (HWND hwnd, int msg, int point, int flags, IPoint2 m ) {	
	ViewExp *vpt=ip->GetViewport (hwnd);
	Point3 p0, p1;
	ISpinnerControl *spin;
	BOOL ln;
	IPoint2 m2;
	float amount;
	switch (msg) {
	case MOUSE_PROPCLICK:
		ip->SetStdCommandMode(CID_OBJMOVE);
		break;

	case MOUSE_POINT:
		if (!point) {
			po->BeginExtrude(ip->GetTime());		
			om = m;
		} else {
			ip->RedrawViews(ip->GetTime(),REDRAW_END);
			po->EndExtrude(ip->GetTime(),TRUE);

		}
		break;

	case MOUSE_MOVE:
		p0 = vpt->MapScreenToView(om,float(-200));
		// sca 1999.02.24: Find m's projection in om's vertical axis:
		m2.x = om.x;
		m2.y = m.y;
		p1 = vpt->MapScreenToView(m2,float(-200));
		amount = Length (p1-p0);
		if (m.y > om.y) amount *= -1.0f;

		ln = IsDlgButtonChecked(po->hOpsPanel,IDC_EM_EXTYPE_B);
		po->Extrude (ip->GetTime(), amount, ln);

		spin = GetISpinner(GetDlgItem(po->hOpsPanel,IDC_EP_EXTRUDESPINNER));
		if (spin) {
			spin->SetValue(amount, FALSE);	// sca - use signed value here too.
			ReleaseISpinner(spin);
		}
		ip->RedrawViews(ip->GetTime(),REDRAW_INTERACTIVE);
		break;

	case MOUSE_ABORT:
		po->EndExtrude(ip->GetTime(),FALSE);			
		ip->RedrawViews(ip->GetTime(),REDRAW_END);
		break;
	}

	if (vpt) ip->ReleaseViewport(vpt);
	return TRUE;
}

HCURSOR EPM_ExtrudeSelectionProcessor::GetTransformCursor() { 
	static HCURSOR hCur = NULL;
	if ( !hCur ) hCur = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_EXTRUDECUR));
	return hCur; 
}

void EPM_ExtrudeCMode::EnterMode() {
	if (!po->hOpsPanel) return;
	ICustButton *but = GetICustButton(GetDlgItem(po->hOpsPanel,IDC_EP_EXTRUDE));
	but->SetCheck(TRUE);
	ReleaseICustButton(but);
}

void EPM_ExtrudeCMode::ExitMode() {
	if (!po->hOpsPanel) return;
	ICustButton *but = GetICustButton(GetDlgItem(po->hOpsPanel,IDC_EP_EXTRUDE));
	but->SetCheck(FALSE);
	ReleaseICustButton(but);
	ISpinnerControl *spin;
	spin = GetISpinner(GetDlgItem(po->hOpsPanel,IDC_EP_EXTRUDESPINNER));
	if (spin) {
		spin->SetValue(0.0f,FALSE);
		ReleaseISpinner(spin);
		}

}



// ------------------------------------------------------

int EPM_BevelMouseProc::proc (HWND hwnd, int msg, int point, int flags, IPoint2 m ) {	
	ViewExp *vpt=ip->GetViewport (hwnd);
	Point3 p0, p1;
	ISpinnerControl *spin;
	int ln,ln2;
	IPoint2 m2;
	float amount;

	switch (msg) {
	case MOUSE_PROPCLICK:
		ip->SetStdCommandMode(CID_OBJMOVE);
		break;

	case MOUSE_POINT:
		if (point==0) {
			po->BeginExtrude(ip->GetTime());		
			om = m;
			} 
		else if (point==1) {
			po->EndExtrude(ip->GetTime(),TRUE);
			po->BeginBevel(ip->GetTime());		
			om = m;
			} 
		else {
			ip->RedrawViews(ip->GetTime(),REDRAW_END);
			po->EndBevel(ip->GetTime(),TRUE);
		}
		break;

	case MOUSE_MOVE:
		if (point == 1)
			{
			p0 = vpt->MapScreenToView(om,float(-200));
			// sca 1999.02.24: find worldspace point with om's x value and m's y value
			m2.x = om.x;
			m2.y = m.y;
			p1 = vpt->MapScreenToView(m2, float(-200));
			amount = Length (p1-p0);
			ln = IsDlgButtonChecked(po->hOpsPanel,IDC_EM_EXTYPE_B);					
			if (om.y < m.y) amount *= -1.0f;
			po->Extrude (ip->GetTime(), amount, ln);

			spin = GetISpinner(GetDlgItem(po->hOpsPanel,IDC_EP_EXTRUDESPINNER));
			if (spin) {
				spin->SetValue (amount, FALSE);
				ReleaseISpinner(spin);
				}
			ip->RedrawViews(ip->GetTime(),REDRAW_INTERACTIVE);
			}
		else if (point == 2)
			{
			p0 = vpt->MapScreenToView(om,float(-200));
			// sca 1999.02.24: find worldspace point with om's x value and m's y value
			m2.x = om.x;
			m2.y = m.y;
			p1 = vpt->MapScreenToView(m2, float(-200));
			if (IsDlgButtonChecked(po->hOpsPanel,IDC_EP_SM_SMOOTH)) ln = 0;					
			else if (IsDlgButtonChecked(po->hOpsPanel,IDC_EP_SM_SMOOTH2)) ln = 1;					
			else if (IsDlgButtonChecked(po->hOpsPanel,IDC_EP_SM_SMOOTH3)) ln = 2;					

			if (IsDlgButtonChecked(po->hOpsPanel,IDC_EP_SM_SMOOTH4)) ln2 = 0;					
			else if (IsDlgButtonChecked(po->hOpsPanel,IDC_EP_SM_SMOOTH5)) ln2 = 1;					
			else if (IsDlgButtonChecked(po->hOpsPanel,IDC_EP_SM_SMOOTH6)) ln2 = 2;					

			amount = Length(p1-p0);
			if (om.y < m.y) amount *= -1.0f;
			po->Bevel (ip->GetTime(), amount, ln, ln2);

			spin = GetISpinner(GetDlgItem(po->hOpsPanel,IDC_EP_OUTLINESPINNER));
			if (spin) {
				spin->SetValue(amount,FALSE);
				ReleaseISpinner(spin);
				}
			ip->RedrawViews(ip->GetTime(),REDRAW_INTERACTIVE);
			}
		break;

	case MOUSE_ABORT:
		if (point==1)
			po->EndExtrude(ip->GetTime(),FALSE);			
		else if (point>1)
			po->EndBevel(ip->GetTime(),FALSE);			
			

		ip->RedrawViews(ip->GetTime(),REDRAW_END);
		break;
	}

	if (vpt) ip->ReleaseViewport(vpt);
	return TRUE;
}

HCURSOR EPM_BevelSelectionProcessor::GetTransformCursor() { 
	static HCURSOR hCur = NULL;
	if ( !hCur ) hCur = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_BEVEL));
	return hCur; 
}

void EPM_BevelCMode::EnterMode() {
	if (!po->hOpsPanel) return;
	ICustButton *but = GetICustButton(GetDlgItem(po->hOpsPanel,IDC_EP_BEVEL));
	but->SetCheck(TRUE);
	ReleaseICustButton(but);
}

void EPM_BevelCMode::ExitMode() {
	if (!po->hOpsPanel) return;
	ICustButton *but = GetICustButton(GetDlgItem(po->hOpsPanel,IDC_EP_BEVEL));
	but->SetCheck(FALSE);
	ReleaseICustButton(but);
	ISpinnerControl *spin;
	spin = GetISpinner(GetDlgItem(po->hOpsPanel,IDC_EP_OUTLINESPINNER));
	if (spin) {
		spin->SetValue(0.0f,FALSE);
		ReleaseISpinner(spin);
		}

}

// --------------------------------------------------------------------
// IPatchSelect and IPatchOps interfaces   (JBW 2/2/99)

void* EditPatchMod::GetInterface(ULONG id) 
{
	switch (id)
	{
		case I_PATCHSELECT: return (IPatchSelect*)this;
		case I_PATCHSELECTDATA: return (IPatchSelectData*)this;
		case I_PATCHOPS: return (IPatchOps*)this;
		case I_SUBMTLAPI: return (ISubMtlAPI*)this;
	}
	return Modifier::GetInterface(id);
}

void EditPatchMod::StartCommandMode(patchCommandMode mode)
{
	switch (mode)
	{
		case PcmAttach:
			if (hOpsPanel != NULL)
				PostMessage(hOpsPanel, WM_COMMAND, IDC_ATTACH, 0);
			break;
		case PcmExtrude:
			if (hOpsPanel != NULL)
				PostMessage(hOpsPanel, WM_COMMAND, IDC_EP_EXTRUDE, 0);
			break;
		case PcmBevel:
			if (hOpsPanel != NULL)
				PostMessage(hOpsPanel, WM_COMMAND, IDC_EP_BEVEL, 0);
			break;
		case PcmBind:
			if (hOpsPanel != NULL)
				PostMessage(hOpsPanel, WM_COMMAND, IDC_BIND, 0);
			break;
	}
}

void EditPatchMod::ButtonOp(patchButtonOp opcode)
{
	switch (opcode)
	{
		case PopUnbind:
			if (hOpsPanel != NULL)
				PostMessage(hOpsPanel, WM_COMMAND, IDC_UNBIND, 0);
			break;
		case PopHide:
			if (hOpsPanel != NULL)
				PostMessage(hOpsPanel, WM_COMMAND, IDC_HIDE, 0);
			break;
		case PopUnhideAll:
			if (hOpsPanel != NULL)
				PostMessage(hOpsPanel, WM_COMMAND, IDC_UNHIDE, 0);
			break;
		case PopWeld:
			if (hOpsPanel != NULL && GetSubobjectLevel() == PO_VERTEX)
				PostMessage(hOpsPanel, WM_COMMAND, IDC_WELD, 0);
			break;
		case PopDelete:
			if (hOpsPanel != NULL && GetSubobjectLevel() >= PO_VERTEX)
				PostMessage(hOpsPanel, WM_COMMAND, IDC_PATCH_DELETE, 0);
			break;
		case PopSubdivide:
			if (hOpsPanel != NULL && GetSubobjectLevel() >= PO_EDGE)
				PostMessage(hOpsPanel, WM_COMMAND, IDC_SUBDIVIDE, 0);
			break;
		case PopAddTri:
			if (hOpsPanel != NULL && GetSubobjectLevel() == PO_EDGE)
				PostMessage(hOpsPanel, WM_COMMAND, IDC_ADDTRI, 0);
			break;
		case PopAddQuad:
			if (hOpsPanel != NULL && GetSubobjectLevel() == PO_EDGE)
				PostMessage(hOpsPanel, WM_COMMAND, IDC_ADDQUAD, 0);
			break;
		case PopDetach:
			if (hOpsPanel != NULL && GetSubobjectLevel() == PO_PATCH)
				PostMessage(hOpsPanel, WM_COMMAND, IDC_DETACH, 0);
			break;
	}
}

DWORD EditPatchMod::GetSelLevel()
{
	return GetSubobjectLevel();
}

void EditPatchMod::SetSelLevel(DWORD level)
{
}

void EditPatchMod::LocalDataChanged()
{
}

MtlID EditPatchMod::GetNextAvailMtlID(ModContext* mc) {
	if(!mc)
		return 1;
	EditPatchData *patchData = (EditPatchData*)mc->localData;
	if ( !patchData ) return 1;

	// If the mesh isn't yet cache, this will cause it to get cached.
	PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
	if(!patch) return 1;
	
	int mtlID = GetSelFaceUniqueMtlID(mc);

	if (mtlID == -1) {
		int i;
 		
		MtlID min, max;
		BOOL first = TRUE;

		for(int p = 0; p < patch->numPatches; ++p) {
			MtlID thisID = patch->getPatchMtlIndex(p);
			if(first) {
				min = max = thisID;
				first = FALSE;
				}
			else
			if(thisID < min)
				min = thisID;
			else
			if(thisID > max)
				max = thisID;
			}
		// If room below, return it
		if(min > 0)
			return min - 1;
		// Build a bit array to find any gaps		
		BitArray b;
		int bits = max - min + 1;
		b.SetSize(bits);
		b.ClearAll();
		for(p = 0; p < patch->numPatches; ++p)
			b.Set(patch->getPatchMtlIndex(p) - min);
		for(i = 0; i < bits; ++i) {
			if(!b[i])
				return (MtlID)(i + min);
			}
		// No gaps!  If room above, return it
		if(max < 65535)
			return max + 1;
		}
	return (MtlID)mtlID;
	}

BOOL EditPatchMod::HasFaceSelection(ModContext* mc) {
	// Are we the edited object?
	if (ip == NULL)  return FALSE;

	EditPatchData *patchData = (EditPatchData*)mc->localData;
	if ( !patchData ) return FALSE;

	// If the mesh isn't yet cache, this will cause it to get cached.
	PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
	if(!patch) return FALSE;

	// Is Patch selection active?
	if (selLevel == EP_PATCH && patch->patchSel.NumberSet()) return TRUE;
	
	return FALSE;
	}

void EditPatchMod::SetSelFaceMtlID(ModContext* mc, MtlID id, BOOL bResetUnsel) {
	int altered = 0;
	EditPatchData *patchData = (EditPatchData*)mc->localData;
	if ( !patchData ) return;

	// If the mesh isn't yet cache, this will cause it to get cached.
	PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
	if(!patch) return;
	
	// If this is the first edit, then the delta arrays will be allocated
	patchData->BeginEdit(ip->GetTime());

	if ( theHold.Holding() )
		theHold.Put(new PatchRestore(patchData,this,patch));

	for(int p = 0; p < patch->numPatches; ++p) {
		if(patch->patchSel[p]) {
			altered = TRUE;
			patch->setPatchMtlIndex(p, id);
			}
		}

	if(altered)	{
		patchData->UpdateChanges(patch, FALSE);
		InvalidateSurfaceUI();
		}

	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
	}

int	EditPatchMod::GetSelFaceUniqueMtlID(ModContext* mc) {
	int	mtlID;

	mtlID = GetSelFaceAnyMtlID(mc);
	if (mtlID == -1) return mtlID;

	EditPatchData *patchData = (EditPatchData*)mc->localData;
	if ( !patchData ) return 1;

	// If the mesh isn't yet cache, this will cause it to get cached.
	PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
	if(!patch) return 1;

	for(int p = 0; p < patch->numPatches; ++p) {
		if(patch->patchSel[p])
			continue;
		if(patch->getPatchMtlIndex(p) != mtlID)
			continue;
		mtlID = -1;
		}
	return mtlID;
	}

int	EditPatchMod::GetSelFaceAnyMtlID(ModContext* mc) {
	int				mtlID = -1;
	BOOL			bGotFirst = FALSE;

	EditPatchData *patchData = (EditPatchData*)mc->localData;
	if ( !patchData ) return 1;

	// If the mesh isn't yet cache, this will cause it to get cached.
	PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
	if(!patch) return 1;

	for(int p = 0; p < patch->numPatches; ++p) {
		if(!patch->patchSel[p])
			continue;
		if (bGotFirst) {
			if (mtlID != patch->getPatchMtlIndex(p)) {
				mtlID = -1;
				break;
				}
			}
		else {
			mtlID = patch->getPatchMtlIndex(p);
			bGotFirst = TRUE;
			}
		}
	return mtlID;
	}

int	EditPatchMod::GetMaxMtlID(ModContext* mc) {
	MtlID mtlID = 0;

	EditPatchData *patchData = (EditPatchData*)mc->localData;
	if ( !patchData ) return 1;

	// If the mesh isn't yet cache, this will cause it to get cached.
	PatchMesh *patch = patchData->TempData(this)->GetPatch(ip->GetTime());
	if(!patch) return 1;

	for(int p = 0; p < patch->numPatches; ++p)
		mtlID = max(mtlID, patch->getPatchMtlIndex(p));

	return mtlID;
	}

/*-------------------------------------------------------------------*/

void EPM_BindCMode::EnterMode()
	{
	if ( pobj->hOpsPanel ) {
		ICustButton *but = GetICustButton(GetDlgItem(pobj->hOpsPanel, IDC_BIND));
		but->SetCheck(TRUE);
		ReleaseICustButton(but);
		}
	}

void EPM_BindCMode::ExitMode()
	{
	if ( pobj->hOpsPanel ) {
		ICustButton *but = GetICustButton(GetDlgItem(pobj->hOpsPanel, IDC_BIND));
		but->SetCheck(FALSE);
		ReleaseICustButton(but);
		}
	}
/*
void EditPatchMod::DoAddHook(int vert1, int seg1) {

	// If any bits are set in the selection set, let's DO IT!!
	if ( !ip ) return;
	theHold.Begin();
	POPatchGenRecord *rec = new POPatchGenRecord(this);
	if ( theHold.Holding() )
		theHold.Put(new PatchObjectRestore(this,rec));
		// Call the patch type change function

	patch.AddHook(vert1,seg1);
	patch.computeInteriors();
	patch.InvalidateGeomCache();
	InvalidateMesh();
	theHold.Accept(GetResString(IDS_TH_PATCHCHANGE));

	NotifyDependents(FOREVER, PART_TOPO, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime(),REDRAW_NORMAL);

	}
*./

/*-------------------------------------------------------------------*/

HCURSOR EPM_BindMouseProc::GetTransformCursor() 
	{ 
	static HCURSOR hCur = NULL;

	if ( !hCur ) {
		hCur = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_SEGREFINECUR)); 
		}

	return hCur; 
	}

BOOL EPM_BindMouseProc::HitTest( 
		ViewExp *vpt, IPoint2 *p, int type, int flags, int subType  )
	{
	vpt->ClearSubObjHitList();
	SetPatchHitOverride(subType);

	ip->SubObHitTest(ip->GetTime(),type,ip->GetCrossing(),flags,p,vpt);
	ClearPatchHitOverride();
	if ( vpt->NumSubObjHits() ) {
		return TRUE;
	} else {
		return FALSE;
		}			
	}

BOOL EPM_BindMouseProc::HitAKnot(ViewExp *vpt, IPoint2 *p, int *vert) {
	int first = 1;
	
	if(HitTest(vpt, p, HITTYPE_POINT, 0,1) ) {
		HitLog &hits = vpt->GetSubObjHitList();
		HitRecord *rec = hits.First();
		DWORD best = 9999;
		HitRecord *bestRec;
		while(rec) {
			PatchHitData *hit = ((PatchHitData *)rec->hitData);
			// If there's an exclusion shape, this must be a part of it!
//			if( patch == hit->patch) {
			pMesh = hit->patch;
			if( 1) {
				if (hit->type == PATCH_HIT_VERTEX)
					{

				// If there's an exclusion shape, the vert & poly can't be the same!
					if(first || rec->distance < best) 
						{
						first = 0;
						best = rec->distance;
						bestRec = rec;
						}
					}
				}
			rec = rec->Next();
			}
		if(!first) {
			PatchHitData *hit = ((PatchHitData *)bestRec->hitData);
			*vert = hit->index;
			return TRUE;
			}
		}
	return FALSE;
	}


BOOL EPM_BindMouseProc::HitASegment(ViewExp *vpt, IPoint2 *p, int *seg) {
	int first = 1;
	
	if(HitTest(vpt, p, HITTYPE_POINT, 0,2) ) {
		HitLog &hits = vpt->GetSubObjHitList();
		HitRecord *rec = hits.First();
		DWORD best = 9999;
		HitRecord *bestRec;
		while(rec) {
			PatchHitData *hit = ((PatchHitData *)rec->hitData);
			// If there's an exclusion shape, this must be a part of it!
			if( pMesh == hit->patch) {
				if (hit->type == PATCH_HIT_EDGE)
					{

				// If there's an exclusion shape, the vert & poly can't be the same!
					if(first || rec->distance < best) 
						{
						first = 0;
						best = rec->distance;
						bestRec = rec;
						}
					}
				}
			rec = rec->Next();
			}
		if(!first) {
			PatchHitData *hit = ((PatchHitData *)bestRec->hitData);
			*seg = hit->index;
			return TRUE;
			}
		}
	return FALSE;
	}


static void PatchXORDottedLine( HWND hwnd, IPoint2 p0, IPoint2 p1 )
	{
	HDC hdc;
	hdc = GetDC( hwnd );
	SetROP2( hdc, R2_XORPEN );
	SetBkMode( hdc, TRANSPARENT );
	SelectObject( hdc, CreatePen( PS_DOT, 0, RGB(255,255,255) ) );
	MoveToEx( hdc, p0.x, p0.y, NULL );
	LineTo( hdc, p1.x, p1.y );		
	DeleteObject( SelectObject( hdc, GetStockObject( BLACK_PEN ) ) );
	ReleaseDC( hwnd, hdc );
	}


int EPM_BindMouseProc::proc(
			HWND hwnd, 
			int msg, 
			int point, 
			int flags, 
			IPoint2 m )
	{
	ViewExp *vpt = ip->GetViewport(hwnd);	
	int res = TRUE;
	static PatchMesh *shape1 = NULL;
	static int poly1, vert1, seg1;
	static IPoint2 anchor, lastPoint;

	switch ( msg ) {
		case MOUSE_PROPCLICK:
			ip->SetStdCommandMode(CID_OBJMOVE);
			break;

		case MOUSE_POINT:
			switch(point) {
				case 0:
					{
					if(HitAKnot(vpt, &m,  &vert1))
						{
						res = TRUE;
						anchor = lastPoint = m;
						PatchXORDottedLine(hwnd, anchor, m);	// Draw it!
//get valid seg list
						knotList.SetSize(pMesh->numVerts);
						knotList.ClearAll();
						for (int i = 0; i < pMesh->numEdges; i++)
							{
							if (pMesh->edges[i].v1 == vert1) 
								{
								knotList.Set(pMesh->edges[i].v2);

								}
							if (pMesh->edges[i].v2 == vert1) 
								{
								knotList.Set(pMesh->edges[i].v1);
								}
							}
						}
					else res = FALSE;

					break;
					}
				case 1:
					PatchXORDottedLine(hwnd, anchor, lastPoint);	// Erase it!
//					if(HitAnEndpoint(vpt, &m, shape1, poly1, vert1, NULL, &poly2, &vert2))
//						ss->DoVertConnect(vpt, shape1, poly1, vert1, poly2, vert2); 
					if(HitASegment(vpt, &m,  &seg1))
						{
//if a valid segemtn change cursor
						int a = pMesh->edges[seg1].v1;
						int b = pMesh->edges[seg1].v2;
						if (knotList[a] && knotList[b])
							pobj->DoAddHook(pMesh,vert1,seg1);

						}
					res = FALSE;
					break;
				default:
					assert(0);
				}
			break;

		case MOUSE_MOVE:
			// Erase old dotted line
			PatchXORDottedLine(hwnd, anchor, lastPoint);
			// Draw new dotted line
			PatchXORDottedLine(hwnd, anchor, m);
			lastPoint = m;
			if(HitASegment(vpt, &m,  &seg1))
				{
//if a valid segemtn change cursor
				int a = pMesh->edges[seg1].v1;
				int b = pMesh->edges[seg1].v2;
				if (knotList[a] && knotList[b])
					SetCursor(LoadCursor(hInstance,MAKEINTRESOURCE(IDC_TH_SELCURSOR)));
				else SetCursor(LoadCursor(NULL,IDC_ARROW));

				}
			else {
				SetCursor(LoadCursor(NULL,IDC_ARROW));
				}

			break;
					
		case MOUSE_FREEMOVE:
			if(HitAKnot(vpt, &m,  &vert1))
				{
				SetCursor(LoadCursor(hInstance,MAKEINTRESOURCE(IDC_TH_SELCURSOR)));
				}
			else {
				SetCursor(LoadCursor(NULL,IDC_ARROW));
				}
/*
			if ( HitTest(vpt,&m,HITTYPE_POINT,HIT_ABORTONHIT,1) ) {
				HitLog &hits = vpt->GetSubObjHitList();
				HitRecord *rec = hits.First();
				if (rec )
					{
					SetCursor(LoadCursor(hInstance,MAKEINTRESOURCE(IDC_TH_SELCURSOR)));
					}
				}
			SetCursor(LoadCursor(NULL,IDC_ARROW));
*/
			break;
		
		case MOUSE_ABORT:
			// Erase old dotted line
			PatchXORDottedLine(hwnd, anchor, lastPoint);
			break;			
		}

	if ( vpt ) ip->ReleaseViewport(vpt);
	return res;
	}

/*-------------------------------------------------------------------*/

