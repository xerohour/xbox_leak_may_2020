/**********************************************************************
 *<
	FILE: VertexPaint.cpp

	DESCRIPTION:	Modifier implementation	

	CREATED BY: Christer Janson, Nikolai Sander

	HISTORY: 

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/

#include "VertexPaint.h"
#include "meshdelta.h"

// flags:
#define VP_DISP_END_RESULT 0x01

static WNDPROC colorSwatchOriginalWndProc;

static	HIMAGELIST hButtonImages = NULL;

static void LoadImages() {
	if (hButtonImages) return;
	HBITMAP hBitmap, hMask;
	hButtonImages = ImageList_Create(15, 14, ILC_MASK, 2, 0);	// 17 is kluge to center square. -SA
	hBitmap     = LoadBitmap (hInstance,MAKEINTRESOURCE(IDB_BUTTONS));
	hMask       = LoadBitmap (hInstance,MAKEINTRESOURCE(IDB_BUTTON_MASK));
	ImageList_Add(hButtonImages, hBitmap, hMask);
	DeleteObject(hBitmap);
	DeleteObject(hMask);
}

ClassDesc* GetVertexPaintDesc();


class VertexPaintClassDesc:public ClassDesc {
	public:
	int 			IsPublic()					{return 1;}
	void *			Create(BOOL loading = FALSE){return new VertexPaint();}
	const TCHAR *	ClassName()					{return GetString(IDS_CLASS_NAME);}
	SClass_ID		SuperClassID()				{return OSM_CLASS_ID;}
	Class_ID		ClassID()					{return VERTEXPAINT_CLASS_ID;}
	const TCHAR* 	Category()					{return GetString(IDS_CATEGORY);}
	void			ResetClassParams(BOOL fileReset) {}
	};

static VertexPaintClassDesc VertexPaintDesc;
ClassDesc* GetVertexPaintDesc() {return &VertexPaintDesc;}

static BOOL CALLBACK VertexPaintDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	int numPoints;
	VertexPaint *mod = (VertexPaint*)GetWindowLong(hWnd,GWL_USERDATA);
	if (!mod && msg!=WM_INITDIALOG) return FALSE;

	
	if (((msg==CC_SPINNER_BUTTONUP) && HIWORD(wParam)) ||
		((msg==CC_SPINNER_CHANGE) ))
	{
		ISpinnerControl *spin;
		spin = (ISpinnerControl *) lParam;
		
		switch (LOWORD(wParam)) 
		{
		case IDC_TINT_SPIN:
			if ((msg == CC_SPINNER_CHANGE))
			{
				mod->fTint = spin->GetFVal()/100;
			}
			break;
		}
	}

	switch (msg) {
		case WM_INITDIALOG:
			LoadImages();
			mod = (VertexPaint*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			mod->hParams = hWnd;
			mod->iPaintButton = GetICustButton(GetDlgItem(hWnd, IDC_PAINT));
			mod->iPaintButton->SetType(CBT_CHECK);
			mod->iPaintButton->SetHighlightColor(GREEN_WASH);
			mod->iPaintButton->SetCheck(mod->ip->GetCommandMode()->ID() == CID_PAINT && 
				!((PaintMouseProc *)mod->ip->GetCommandMode()->MouseProc(&numPoints))->GetPickMode());
			mod->iPaintButton->SetImage(hButtonImages,0,0,0,0,15,14);
			mod->iPaintButton->SetTooltip (TRUE, GetString (IDS_PAINT));

			mod->iPickButton = GetICustButton(GetDlgItem(hWnd, IDC_PICK));
			mod->iPickButton->SetType(CBT_CHECK);
			mod->iPickButton->SetHighlightColor(GREEN_WASH);
			mod->iPickButton->SetCheck(mod->ip->GetCommandMode()->ID() == CID_PAINT && 
				((PaintMouseProc *)mod->ip->GetCommandMode()->MouseProc(&numPoints))->GetPickMode());
			mod->iPickButton->SetImage(hButtonImages,1,1,1,1,15,14);
			mod->iPickButton->SetTooltip (TRUE, GetString (IDS_PICK));


			mod->iColor = GetIColorSwatch(GetDlgItem(hWnd, IDC_COLOR));
			mod->iColor->SetColor(mod->lastColor);
			break;

		case WM_POSTINIT:
			mod->InitPalettes();
			break;

		case CC_COLOR_CHANGE:
			if (LOWORD(wParam) == IDC_COLOR) {
				IColorSwatch* iCol = (IColorSwatch*)lParam;
				mod->lastColor = iCol->GetColor();
				}
			break;
		case WM_DESTROY:
			mod->SavePalettes();
			mod->iPaintButton = NULL;
			mod->iPickButton = NULL;
			mod->iColor = NULL;
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDC_PAINT:
					mod->ActivatePaint(mod->iPaintButton->IsChecked());
					break;
				case IDC_PICK:
					mod->ActivatePaint(mod->iPickButton->IsChecked(),TRUE);
					break;

				case IDC_VC_ON:
					mod->TurnVCOn(FALSE);
					break;
				case IDC_SHADED:
					mod->TurnVCOn(TRUE);
					break;
				}
			break;

		default:
			return FALSE;
		}
	return TRUE;
	}

// Subclass procedure 
LRESULT APIENTRY colorSwatchSubclassWndProc(
    HWND hwnd, 
    UINT uMsg, 
    WPARAM wParam, 
    LPARAM lParam) 
{
	switch (uMsg) {
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK: {
			HWND hPanel = GetParent(hwnd);
			LONG mod = GetWindowLong(hPanel,GWL_USERDATA);
			if (mod) {
				((VertexPaint*)mod)->PaletteButton(hwnd);
				}
			}
			break;
		case WM_DESTROY:
			SetWindowLong(hwnd, GWL_WNDPROC, (LONG) colorSwatchOriginalWndProc); 
			// Fallthrough...
		default:
			return CallWindowProc(colorSwatchOriginalWndProc, hwnd, uMsg, wParam, lParam); 
			break;
		}
	return 0;
	}
 

IObjParam *VertexPaint::ip			= NULL;
HWND VertexPaint::hParams			= NULL;		
VertexPaint* VertexPaint::editMod	= NULL;
ICustButton* VertexPaint::iPaintButton	= NULL;
ICustButton* VertexPaint::iPickButton	= NULL;
IColorSwatch* VertexPaint::iColor	= NULL;
COLORREF VertexPaint::lastColor		= RGB(255,255,255);
COLORREF VertexPaint::palColors[]	= {
	RGB(255,  0,  0),	RGB(  0,255,  0),	RGB(  0,  0,255),	RGB(255,255,255),
	RGB(255,255,  0),	RGB(  0,255,255),	RGB(255,  0,255),	RGB(170,170,170),
	RGB(128,  0,  0),	RGB(  0,128,  0),	RGB(  0,  0,128),	RGB( 85, 85, 85),
	RGB(128,128,  0),	RGB(  0,128,128),	RGB(128,  0,128),	RGB(  0,  0,  0)
	};


//--- VertexPaint -------------------------------------------------------
VertexPaint::VertexPaint() : iTint(NULL), fTint(1.0f)
	{
	flags = 0x0;
	}

VertexPaint::~VertexPaint()
	{
	}

Interval VertexPaint::LocalValidity(TimeValue t)
	{
	return FOREVER;
	}

BOOL VertexPaint::DependOnTopology(ModContext &mc)
	{
	return TRUE;
	}

RefTargetHandle VertexPaint::Clone(RemapDir& remap)
	{
	VertexPaint* newmod = new VertexPaint();	
	return(newmod);
	}

void VertexPaint::NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc)
	{
	if (!mc->localData) return;
	((VertexPaintData*)mc->localData)->FreeCache();

	}

void VertexPaint::ModifyObject(TimeValue t, ModContext &mc, ObjectState * os, INode *node) 
	{
	if (!os->obj->IsSubClassOf(triObjectClassID)) return;
	
	os->obj->ReadyChannelsForMod(GEOM_CHANNEL|TOPO_CHANNEL|VERTCOLOR_CHANNEL|TEXMAP_CHANNEL);
	
	TriObject *tobj = (TriObject*)os->obj;
	VertexPaintData *d  = (VertexPaintData*)mc.localData;
		
	Mesh* mesh = &tobj->GetMesh();
	
	if (mesh) 
	{
		// We don't have any VColors yet, so we allocate the vcfaces
		// and set all vcolors to black (index 0)

		if (!mesh->vcFace) 
		{
			mesh->setNumVCFaces(mesh->getNumFaces());
			mesh->setNumVertCol(1);
			
			mesh->vertCol[0] = Color(1,1,1);

			for (int f=0; f<mesh->getNumFaces(); f++) 
			{
				mesh->vcFace[f].t[0] = 0;
				mesh->vcFace[f].t[1] = 0;
				mesh->vcFace[f].t[2] = 0;
			}
		}

		if (!d) mc.localData = d = new VertexPaintData(tobj->GetMesh());	
		if (!d->GetMesh()) d->SetCache(*mesh);	
		
		// If we're not in face SO mode, it's easier !

		if(mesh->selLevel == MESH_OBJECT || mesh->selLevel == MESH_VERTEX )
		{
			MeshDelta md(*mesh);
			//MeshDelta mdc;
			//if(cache) mdc.InitToMesh(*cache);

			// If the incoming Mesh had no vertex colors, this will add a default map to start with.
			// The default map has the same topology as the Mesh (so one color per vertex),
			// with all colors set to white.
			if (!mesh->mapSupport(0)) md.AddVertexColors ();
			//if (cache && !cache->mapSupport(0)) mdc.AddVertexColors ();
			
			// We used two routines -- VCreate to add new map vertices, and FRemap to make the
			// existing map faces use the new verts.  frFlags tell FRemap which vertices on a face
			// should be "remapped", and the ww array contains the new locations.			
			VertColor nvc;
			int j;
			for (int v=0; v < d->GetNumColors(); v++) 
			{
				ColorData cd = d->GetColorData(v);
				
				
				if(  cd.color == 0xffffffff )
					continue;

				if(mesh->selLevel == MESH_VERTEX && !mesh->VertSel()[v] )
					continue;
				
				// Blend it together with the VertexColor of the incoming mesh
				nvc = (1.0f-cd.bary)*mesh->vertCol[mesh->vcFace[cd.fi].t[cd.vi]] + cd.bary*Color(cd.color);

				DWORD ww[3], frFlags;
				
				md.map->VCreate (&nvc);
				
				// increase the number of vcol's and set the vcfaces as well	
				for(int i = 0 ; i < d->GetNVert(v).faces.Count() ; i++)
				{		
					j = d->GetNVert(v).whichVertex[i];
					frFlags = (1<<j);
					ww[j] = md.map->outVNum()-1;
					md.map->FRemap(d->GetNVert(v).faces[i], frFlags, ww);
					
				}
			} 

			md.Apply(*mesh);
		}

		else if(mesh->selLevel == MESH_FACE)
		{			
			MeshDelta md(*mesh);
			MeshDelta mdc;

			// If the incoming Mesh had no vertex colors, this will add a default map to start with.
			// The default map has the same topology as the Mesh (so one color per vertex),
			// with all colors set to white.
			if (!mesh->mapSupport(0)) md.AddVertexColors ();
			
			// We used two routines -- VCreate to add new map vertices, and FRemap to make the
			// existing map faces use the new verts.  frFlags tell FRemap which vertices on a face
			// should be "remapped", and the ww array contains the new locations.			
			VertColor nvc;
			int j;	
			for (int v=0; v < d->GetNumColors(); v++) 
			{
				ColorData cd = d->GetColorData(v);
				
				if(  cd.color == 0xffffffff )
					continue;
				
				if(!mesh->FaceSel()[cd.fi] )
					continue;
				
				// Blend it together with the VertexColor of the incoming mesh
				nvc = (1.0f-cd.bary)*mesh->vertCol[mesh->vcFace[cd.fi].t[cd.vi]] + cd.bary*Color(cd.color);

				DWORD ww[3], frFlags;
				DWORD ivc;
				BOOL firstround = TRUE;
				BOOL fsel = FALSE;
				
				Tab<DWORD> selFaceTab;
				Tab<DWORD> unselFaceTab;
				Tab<DWORD> vcSelTab;
				Tab<DWORD> vcSelOnlyTab;
				
				BOOL NoFaceSelected = TRUE;
				
				// Check, if any of the faces is selected, if not continue
				for(int fc = 0; fc < d->GetNVert(v).faces.Count() && NoFaceSelected; fc++)
				{		
						if(mesh->FaceSel()[d->GetNVert(v).faces[fc]])
							NoFaceSelected = FALSE;
				}
				
				if(NoFaceSelected)
					continue;		
				
				
				fsel = TRUE;
				DWORD ivtx;
				BOOL SelFaceColsAllSame = TRUE;

				// For all selected faces, that reference this vertex
				for(fc = 0; fc < d->GetNVert(v).faces.Count() && SelFaceColsAllSame ; fc++)
				{
					if(!mesh->FaceSel()[d->GetNVert(v).faces[fc]])
						continue;
					
					int face = d->GetNVert(v).faces[fc];
					
					// get the vertex color of this face
					ivtx = mesh->vcFace[face].t[d->GetNVert(v).whichVertex[fc]];
					
					if(firstround)
					{
						// Just store the vertex color
						ivc = ivtx;
						firstround = FALSE;
					}
					else
					{
						// Are the vertex colors the same ?
						if(ivc != ivtx)
						{
							// No they are not.
							SelFaceColsAllSame = FALSE;
						}
					}
					vcSelTab.Append(1,&ivtx);
				}// End For all selected faces, that reference this vertex

				if(SelFaceColsAllSame)
				{
					BOOL CreateNewColor = FALSE;
					
					// Check, if the ivc is referenced by any face, that is not selected
					// and shares the vertex, or does not share the vertex at all
					
					for(int vcfc = 0; vcfc < d->GetNVCVert(ivc).faces.Count() && CreateNewColor == FALSE; vcfc++)
					{
						BOOL shared = FALSE;
						
						// Is one of faces that reference the physical vertex equal to the faces, 
						// that reference the vc vertex ?
						for(fc = 0; fc < d->GetNVert(v).faces.Count() && CreateNewColor == FALSE ; fc++)
						{
							if(d->GetNVert(v).faces[fc] == d->GetNVCVert(ivc).faces[vcfc])
							{
								if(!mesh->FaceSel()[d->GetNVCVert(ivc).faces[vcfc]])
									CreateNewColor = TRUE;
							}
							else
								CreateNewColor = TRUE;

						}
					}
						
					if(CreateNewColor)
					{
						// Create a new Color
						md.map->VCreate (&nvc);
										
						// Assign the color of the selected faces to the new color
						for(fc = 0;  fc < d->GetNVert(v).faces.Count() ; fc++)
						{
							if(!mesh->FaceSel()[d->GetNVert(v).faces[fc]])
								continue;

							j = d->GetNVert(v).whichVertex[fc];
							frFlags = (1<<j);
							ww[j] = md.map->outVNum()-1;
							md.map->FRemap(d->GetNVert(v).faces[fc], frFlags, ww);
						}

					}
					else // OneUnselFaceHasSameColor
					{
						// Set the vcolor to the new value
						mesh->vertCol[ivtx] = nvc;
					}
				}
				else // (SelFaceColsAllSame) Not all selected faces have the same color
				{
					// Collect all vcolors, that are only referenced by selected faces
					BOOL VColorIsReferencedByUnselectedFace = FALSE;
					
					// For all VColors, that are on selected faces
					for(int v2 = 0 ; v2 < vcSelTab.Count() ; v2++)
					{
						DWORD ivcv = vcSelTab[v2];
						// Check these vertex colors, if they are on any face, that 
						// is not selected
						for(int vfc = 0 ; vfc < d->GetNVCVert(ivcv).faces.Count() && !VColorIsReferencedByUnselectedFace ; vfc++)
						{							
							if(mesh->FaceSel()[d->GetNVCVert(ivcv).faces[vfc]])
								VColorIsReferencedByUnselectedFace = TRUE;		
								
						}
						if(!VColorIsReferencedByUnselectedFace)
							vcSelOnlyTab.Append(1,&ivcv);
					}

					if(vcSelOnlyTab.Count() > 0)
					{
						// Set the vcolor to the new value
						mesh->vertCol[ivtx] = nvc;
						
						for(fc = 0;  fc < d->GetNVert(v).faces.Count() ; fc++)
						{
							if(!mesh->FaceSel()[d->GetNVert(v).faces[fc]])
							continue;
							
							j = d->GetNVert(v).whichVertex[fc];
							frFlags = (1<<j);

							ww[j] = ivtx;
							md.map->FRemap(d->GetNVert(v).faces[fc], frFlags, ww);
						}
					}
					else
					{
						// Create a new Color
						md.map->VCreate (&nvc);
											
						// Assign the color of the selected faces to the new color
						for(fc = 0;  fc < d->GetNVert(v).faces.Count() ; fc++)
						{
							if(!mesh->FaceSel()[d->GetNVert(v).faces[fc]])
								continue;

							j = d->GetNVert(v).whichVertex[fc];
							frFlags = (1<<j);
							ww[j] = md.map->outVNum()-1;

							md.map->FRemap(d->GetNVert(v).faces[fc], frFlags, ww);
						}
					}
				}
			}	
			md.Apply(*mesh);
		}// end if(mesh->selLevel == MESH_FACE)
	
		NotifyDependents(FOREVER, PART_VERTCOLOR, REFMSG_CHANGE);
		os->obj->UpdateValidity(VERT_COLOR_CHAN_NUM, Interval(t,t));
	}
}

static bool oldShowEnd;

void VertexPaint::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
	{
	
	this->ip = ip;
	editMod = this;
	if (!hParams) {
		hParams = ip->AddRollupPage( 
				hInstance, 
				MAKEINTRESOURCE(IDD_PANEL),
				VertexPaintDlgProc, 
				GetString(IDS_PARAMS), 
				(LPARAM)this);

		// Subclass the palette controls
		hPaletteWnd[ 0] = GetDlgItem(hParams, IDC_PALETTE_1);
		hPaletteWnd[ 1] = GetDlgItem(hParams, IDC_PALETTE_2);
		hPaletteWnd[ 2] = GetDlgItem(hParams, IDC_PALETTE_3);
		hPaletteWnd[ 3] = GetDlgItem(hParams, IDC_PALETTE_4);
		hPaletteWnd[ 4] = GetDlgItem(hParams, IDC_PALETTE_5);
		hPaletteWnd[ 5] = GetDlgItem(hParams, IDC_PALETTE_6);
		hPaletteWnd[ 6] = GetDlgItem(hParams, IDC_PALETTE_7);
		hPaletteWnd[ 7] = GetDlgItem(hParams, IDC_PALETTE_8);
		hPaletteWnd[ 8] = GetDlgItem(hParams, IDC_PALETTE_9);
		hPaletteWnd[ 9] = GetDlgItem(hParams, IDC_PALETTE_10);
		hPaletteWnd[10] = GetDlgItem(hParams, IDC_PALETTE_11);
		hPaletteWnd[11] = GetDlgItem(hParams, IDC_PALETTE_12);
		hPaletteWnd[12] = GetDlgItem(hParams, IDC_PALETTE_13);
		hPaletteWnd[13] = GetDlgItem(hParams, IDC_PALETTE_14);
		hPaletteWnd[14] = GetDlgItem(hParams, IDC_PALETTE_15);
		hPaletteWnd[15] = GetDlgItem(hParams, IDC_PALETTE_16);

		for (int i=0; i<NUMPALETTES; i++) {
			colorSwatchOriginalWndProc = (WNDPROC) SetWindowLong(hPaletteWnd[i], GWL_WNDPROC, (LONG) colorSwatchSubclassWndProc); 
			}

		SendMessage(hParams, WM_POSTINIT, 0, 0);
		}
	else {
		SetWindowLong(hParams,GWL_USERDATA,(LONG)this);
		}
	iTint = SetupIntSpinner (hParams, IDC_TINT_SPIN, IDC_TINT, 0, 100, (int) (fTint*100.0f));

	// Set show end result.
	oldShowEnd = ip->GetShowEndResult() ? TRUE : FALSE;
	ip->SetShowEndResult (GetFlag (VP_DISP_END_RESULT));

	// Force an eval to update caches.
	NotifyDependents(FOREVER, PART_VERTCOLOR, REFMSG_CHANGE);
	}

void VertexPaint::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next)
	{
	ActivatePaint(FALSE);
	
	ReleaseISpinner (iTint);
	
	ModContextList list;
	INodeTab nodes;
	ip->GetModContexts(list,nodes);
	for (int i=0; i<list.Count(); i++) {
		VertexPaintData *vd = (VertexPaintData*)list[i]->localData;
		if (vd) vd->FreeCache();
	}
	nodes.DisposeTemporary();

	// Reset show end result
	SetFlag (VP_DISP_END_RESULT, ip->GetShowEndResult() ? TRUE : FALSE);
	ip->SetShowEndResult(oldShowEnd);


	ip->DeleteRollupPage(hParams);
	hParams = NULL;
	editMod = NULL;
	iTint = NULL;
	this->ip = NULL;
	}


//From ReferenceMaker 
RefResult VertexPaint::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
	{
	return REF_SUCCEED;
	}

int VertexPaint::NumRefs() 
	{
	return 0;
	}

RefTargetHandle VertexPaint::GetReference(int i) 
	{
	return NULL;
	}

void VertexPaint::SetReference(int i, RefTargetHandle rtarg)
	{
	}

int VertexPaint::NumSubs() 
	{ 
	return 0;
	}  

Animatable* VertexPaint::SubAnim(int i) 
	{ 
	return NULL; 
	}

TSTR VertexPaint::SubAnimName(int i) 
	{ 
	return _T("");
	}


#define VERSION_CHUNKID			0x100
#define COLORLIST_CHUNKID		0x120

static int currentVersion = 1;

IOResult VertexPaint::Load(ILoad *iload)
	{
	IOResult res;
	ULONG nb;
	int version = 1;
	Modifier::Load(iload);

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case VERSION_CHUNKID:
				iload->Read (&version, sizeof(version), &nb);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) return res;
		}

	return IO_OK;
	}

IOResult VertexPaint::Save(ISave *isave)
	{
	IOResult res;
	ULONG nb;

	Modifier::Save(isave);

	isave->BeginChunk(VERSION_CHUNKID);
	res = isave->Write (&currentVersion, sizeof(int), &nb);
	isave->EndChunk();

	return IO_OK;
	}

IOResult VertexPaint::SaveLocalData(ISave *isave, LocalModData *ld)
	{
	VertexPaintData*	d = (VertexPaintData*)ld;
	IOResult	res;
	ULONG		nb;
	int			numColors;
	int			maxNumColors;
	ColorData	col;
	
	isave->BeginChunk(VERSION_CHUNKID);
	res = isave->Write (&currentVersion, sizeof(int), &nb);
	isave->EndChunk();
	
	isave->BeginChunk(COLORLIST_CHUNKID);
	numColors = d->GetNumColors();
	res = isave->Write(&numColors, sizeof(int), &nb);
	maxNumColors = d->GetMaxNumColors();
	res = isave->Write(&maxNumColors, sizeof(int), &nb);
	for (int i=0; i<maxNumColors; i++) {
		col = d->GetColorData(i);
		isave->Write(&col.color,sizeof(col.color),&nb);
		isave->Write(&col.bary,sizeof(col.bary),&nb);
		isave->Write(&col.fi,sizeof(col.fi),&nb);
		isave->Write(&col.vi,sizeof(col.vi),&nb);
		}

	isave->EndChunk();
	return IO_OK;
	}

IOResult VertexPaint::LoadLocalData(ILoad *iload, LocalModData **pld) {
	VertexPaintData *d = new VertexPaintData;
	IOResult	res;	
	ULONG		nb;
	int			version = 1;
	int			numColors;
	int			maxNumColors;
	ColorData	col;

	*pld = d;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
		case VERSION_CHUNKID:
				iload->Read (&version, sizeof(version), &nb);
				break;
		case COLORLIST_CHUNKID:
				{
					iload->Read(&numColors,sizeof(int), &nb);
					iload->Read(&maxNumColors,sizeof(int), &nb);
					d->AllocColorData(maxNumColors);
					for (int i=0; i<maxNumColors; i++) {
						iload->Read(&col.color,sizeof(col.color), &nb);
						iload->Read(&col.bary,sizeof(col.bary), &nb);
						iload->Read(&col.fi,sizeof(col.fi), &nb);
						iload->Read(&col.vi,sizeof(col.vi), &nb);
						d->SetColor(i, col.bary, col.fi,col.vi,col.color);
					}
					d->AllocColorData(numColors);
				}
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) return res;
		}
	return IO_OK;
	}

void VertexPaint::PaletteButton(HWND hWnd)
	{
	IColorSwatch* iPal = GetIColorSwatch(hWnd);
	if (iPal && iColor) {
		iColor->SetColor(iPal->GetColor(), TRUE);
		}
	}

void VertexPaint::InitPalettes()
	{
	IColorSwatch* c;
	for (int i=0; i<NUMPALETTES; i++) {
		c = GetIColorSwatch(hPaletteWnd[i]);
		c->SetColor(palColors[i]);
		ReleaseIColorSwatch(c);
		}
	}

void VertexPaint::SavePalettes()
	{
	IColorSwatch* c;
	for (int i=0; i<NUMPALETTES; i++) {
		c = GetIColorSwatch(hPaletteWnd[i]);
		palColors[i] = c->GetColor();
		ReleaseIColorSwatch(c);
		}
	}

void VertexPaint::TurnVCOn(BOOL shaded)
{
	ModContextList list;
	INodeTab NodeTab;
	
	// Only the selected nodes will be affected
	ip->GetModContexts(list,NodeTab);

	for( int i = 0 ; i < NodeTab.Count() ; i++)
	{
		if(shaded)
			NodeTab[i]->SetShadeCVerts(!NodeTab[i]->GetShadeCVerts());
		else
			NodeTab[i]->SetCVertMode(!NodeTab[i]->GetCVertMode());	
		
	}
	NotifyDependents(FOREVER, PART_VERTCOLOR, REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
}

VertexPaintData::VertexPaintData(Mesh& m) : mesh(NULL), colordata(NULL), nverts(NULL), 
nvcverts(NULL), numColors(0), numnverts(0), numnvcverts(0), maxNumColors(0)
{
	SetCache(m);
}

VertexPaintData::VertexPaintData() : mesh(NULL), colordata(NULL), nverts(NULL), 
nvcverts(NULL), numColors(0), numnverts(0), numnvcverts(0), maxNumColors(0)
{

}

VertexPaintData::~VertexPaintData()
	{
	FreeCache();

	if (colordata) delete [] colordata;
	if(nverts) delete [] nverts;
	if(nvcverts) delete [] nvcverts;

	nverts = NULL;
	nvcverts = NULL;
	colordata = NULL;
	
	maxNumColors = 0;
	numColors = 0;
	numnverts = 0;
	numnvcverts = 0;
	}

void VertexPaintData::SetCache(Mesh& m)
{
	FreeCache();
	mesh = new Mesh(m);
	SynchVerts(m);
	AllocColorData(mesh->getNumVerts());
				
}

void VertexPaintData::FreeCache()
	{
	if (mesh) delete mesh;
	if(nverts) delete [] nverts;			 
	if(nvcverts) delete [] nvcverts;

	mesh = NULL;
	nverts = NULL;
	nvcverts = NULL;
	numnverts = 0;
	numnvcverts = 0;
	}

Mesh* VertexPaintData::GetMesh()
	{
	return mesh;
	}

NVert&  VertexPaintData::GetNVert(int i)
{
	static NVert nv;
	
	if (numnverts > i)
		return nverts[i];
	else
		return nv;
}

NVert& VertexPaintData::GetNVCVert(int i)
{
	static NVert nv;
	
	if (numnvcverts > i)
		return nvcverts[i];
	else
		return nv;
}

COLORREF& VertexPaintData::GetColor(int i)
	{
	static COLORREF c = RGB(1,1,1);
	if (maxNumColors > i)
		return colordata[i].color;
	else
		return c;
	}

ColorData& VertexPaintData::GetColorData(int i)
	{
	static ColorData c;

	if (maxNumColors > i)
		return colordata[i];
	else
		return c;
	}

void VertexPaintData::SetColor(int i, float bary, DWORD fi, int vi, COLORREF c)
{
	
	if (colordata && maxNumColors > i) 
	{
		
		if(colordata[i].color != 0xffffffff)
		{
			// This color was set before !
			
			float oldbary = colordata[i].bary;
			float alpha = (1.0f-bary);
			
			float weight;
			
			if(bary+(alpha*oldbary) > 0)
				weight = 1/(bary+(alpha*oldbary));
			else
				weight = 0;

			colordata[i].color = (DWORD) (weight*alpha*oldbary*Color(colordata[i].color) + weight*bary*Color(c));
			colordata[i].bary = alpha*oldbary+bary;

			colordata[i].fi = fi;
			colordata[i].vi = vi;
		}
		else
		{
			colordata[i].color = c;
			colordata[i].bary = bary;
			colordata[i].fi = fi;
			colordata[i].vi = vi;
		}
	}
}

int VertexPaintData::GetNumColors()
	{
	return numColors;
	}

int VertexPaintData::GetMaxNumColors()
	{
	return maxNumColors;
	}

void VertexPaintData::AllocColorData(int numcols)
	{
	ColorData* newColorData;

	// Colors already exist.
	if (numColors == numcols)
		return;
	
	if (numColors > 0 ) 
	{
		// If the new number of colors is bigger than what we have in the colordata array
		if(numcols > maxNumColors)
		{
			// Allocate a new color list and fill in as many as
			// we have from the previous set
			newColorData = new ColorData[numcols];
			
			for (int i=0; i<numcols; i++) 
			{
				if (i < maxNumColors) 
				{
					newColorData[i] = colordata[i];
					
					if(mesh && newColorData[i].fi >= (DWORD) mesh->getNumFaces())
					{
						BOOL found = FALSE;
						for(int j = 0 ; j < nverts[i].faces.Count() && !found; j++)
						{
							if(nverts[i].faces[j] < mesh->getNumFaces())
							{
								newColorData[i].fi = nverts[i].faces[j];
								newColorData[i].vi = nverts[i].whichVertex[j];
							}
							
						}
						if(!found)
							newColorData[i].vi = 0;
					}
				}
			}
			delete [] colordata;
			
			colordata = newColorData;
			
			maxNumColors = numColors = numcols;
			
		}
		else
		{
			// If the new number of colors is smaller than what we have in the colordata array
			for (int i=0; i<numcols; i++) 
			{
				if(mesh && colordata[i].fi >= (DWORD) mesh->getNumFaces())
				{
					BOOL found = FALSE;
					for(int j = 0 ; j < nverts[i].faces.Count() && !found; j++)
					{
						if(nverts[i].faces[j] < mesh->getNumFaces())
						{
							colordata[i].fi = nverts[i].faces[j];
							colordata[i].vi = nverts[i].whichVertex[j];
						}
						
					}
					if(!found)
						colordata[i].vi = 0;
				}
			}
			numColors = numcols;
		}
	}
	else
	{
		// Allocate a complete new set of colors
		numColors = numcols;
		maxNumColors = numColors;
		colordata = new ColorData[numColors];
		

	}
}

LocalModData* VertexPaintData::Clone()
	{
	VertexPaintData* d = new VertexPaintData();

	if (colordata) {
		d->colordata = new ColorData[maxNumColors];
		d->numColors = numColors;
		d->maxNumColors = maxNumColors;
		for (int i=0; i<maxNumColors; i++) {
			d->colordata[i] = colordata[i];
			}
		}
	if(nverts)
	{
		d->nverts = new NVert[numnverts];
		for(int i = 0 ; i < numnverts ; i++ ) {
			d->nverts[i] = nverts[i];
		}

	}
	if(nvcverts)
	{
		d->nvcverts = new NVert[numnvcverts];
		for(int i = 0 ; i < numnvcverts ; i++ ) {
			d->nvcverts[i] = nvcverts[i];
		}

	}

	return d;
	}


void VertexPaintData::SynchVerts(Mesh &m)
{

	if(nverts)
		delete [] nverts;
	
	numnverts = m.getNumVerts();
	
	nverts = new NVert[numnverts];

	if(nvcverts)
		delete [] nvcverts;
	
	numnvcverts = m.getNumVertCol();

	nvcverts = new NVert[numnvcverts];
	
	for(int i = 0 ; i < mesh->getNumFaces() ; i++)
	{	
		// for each vertex of each face
		for(int j = 0 ; j < 3 ; j++)
		{		
			int iCur = nverts[mesh->faces[i].v[j]].faces.Count();
			
			// Tell the vertex, which to which face it belongs and which 
			// of the three face v-indices corresponds to the vertex
			
			nverts[mesh->faces[i].v[j]].faces.SetCount(iCur+1);
			nverts[mesh->faces[i].v[j]].whichVertex.SetCount(iCur+1);

			nverts[mesh->faces[i].v[j]].faces[iCur] = i;
			nverts[mesh->faces[i].v[j]].whichVertex[iCur] = j;
			
			
			if(mesh->vcFace)
			{
				// Do the same for texture vertices
			iCur = nvcverts[mesh->vcFace[i].t[j]].faces.Count();
			
			nvcverts[mesh->vcFace[i].t[j]].faces.SetCount(iCur+1);
			nvcverts[mesh->vcFace[i].t[j]].whichVertex.SetCount(iCur+1);
			
			nvcverts[mesh->vcFace[i].t[j]].faces[iCur] = i;
			nvcverts[mesh->vcFace[i].t[j]].whichVertex[iCur] = j;

			}
			else
				assert(0);
		}
	}
}


//***************************************************************************
//**
//** NVert
//**
//***************************************************************************



NVert::NVert()
{
	faces.SetCount(0);
	whichVertex.SetCount(0);
}

NVert& NVert::operator= (NVert &nvert)
{
	faces = nvert.faces;
	whichVertex = nvert.whichVertex;
	return *this;
}

//***************************************************************************
//**
//** ColorData 
//**
//***************************************************************************


ColorData::ColorData(DWORD col) : color(col), bary(0.0f), fi(0), vi(0)
{
}

ColorData::ColorData() : color(0xffffffff), bary(0.0f), fi(0), vi(0)
{
}

//***************************************************************************
//**
//** VertexPaintRestore : public RestoreObj
//**
//***************************************************************************

VertexPaintRestore::VertexPaintRestore(VertexPaintData *pLocalData, VertexPaint *pVPaint) 
: pMod(pVPaint), pPaintData(pLocalData), redoColordata(NULL)
{
	colordata = new ColorData[pPaintData->maxNumColors];
	for(int i = 0; i < pPaintData->maxNumColors ; i++)
	{
		colordata[i] = pPaintData->colordata[i];
	}
	numcolors = pPaintData->numColors;
	maxnumcolors = pPaintData->maxNumColors;

}

VertexPaintRestore::~VertexPaintRestore()
{
	if(colordata)
		delete [] colordata;
	
	if(redoColordata)
		delete [] redoColordata;
}

void VertexPaintRestore::Restore(int isUndo)
{
	if(isUndo)
	{
		assert(pPaintData->colordata);

		redoColordata = pPaintData->colordata;
		redonumcolors = pPaintData->numColors;
		redomaxnumcolors = pPaintData->maxNumColors;

		pPaintData->colordata = colordata;
		pPaintData->numColors = numcolors;
		pPaintData->maxNumColors = maxnumcolors;

		colordata = NULL;
		
		pMod->NotifyDependents(FOREVER, PART_VERTCOLOR, REFMSG_CHANGE);
		GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	}
}

void VertexPaintRestore::Redo()
{
	assert(pPaintData->colordata);
	
	colordata = pPaintData->colordata;
	numcolors = pPaintData->numColors;
	maxnumcolors = pPaintData->maxNumColors;
	
	pPaintData->colordata = redoColordata;
	pPaintData->numColors = redonumcolors;
	pPaintData->maxNumColors = redomaxnumcolors;
	
	redoColordata = NULL;

	pMod->NotifyDependents(FOREVER, PART_VERTCOLOR, REFMSG_CHANGE);
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	
}

int  VertexPaintRestore::Size()
{
	int iSize = 0;
	
	if(colordata)
		iSize += sizeof(ColorData) * maxnumcolors;
	
	if(redoColordata)
		iSize += sizeof(ColorData) * redonumcolors;

	return iSize;
}

