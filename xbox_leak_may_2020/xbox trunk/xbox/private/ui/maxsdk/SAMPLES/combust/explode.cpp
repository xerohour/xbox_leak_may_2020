/**********************************************************************
 *<
	FILE: explode.cpp

	DESCRIPTION: A fire/explosion atmospheric effect

	CREATED BY: Rolf Berteig

	HISTORY: 4-15-96

 *>	Copyright (c) 1996 Rolf Berteig, All Rights Reserved.
 **********************************************************************/

#include "inferno.h"
#include "imtl.h"
#include "render.h"  
#include <bmmlib.h>
#include "iparamm.h"
#include "texutil.h"
#include "gizmo.h"
#include "gizmoimp.h"
#include "istdplug.h"


static Class_ID volumeExplode(0x28b00147, 0x37c19db);

#define EXPLODE_CLASSNAME GetString(IDS_RB_COMBUSTION)

#define PBLOCK_REF	0
#define OBJECT_REF	1


#define SPHERE_GIZMO	0
#define CYL_GIZMO		1
#define BOX_GIZMO		2

class ExplodeDlgProc;

class ExplodeSource {
	public:
		Matrix3 tm;
		int hemi, type;
		Point3 seedPt;
		float radius, radius2, hemiRange, hemiLen;
		float width, length, height, height2;

		ExplodeSource(INode *node,SphereGizmoObject *obj,
			float drift, float rfact,
			TimeValue t, Interval &valid);
		ExplodeSource();
		float ComputeTaper(Point3 p);
	};

class ExplodeAtmos : public Atmospheric {
	public:
		// Parameters
		IParamBlock *pblock;  // Ref #0
		Tab<INode*> nodes;	  // Ref #1-n

		// Caches
		Tab<ExplodeSource> sources;		
		float stretch, regularity, drift, rfact;
		float density, scale, phase, levels;
		Color color1, color2, color3;
		int samples, invert;
		Interval valid;		
		CRITICAL_SECTION csect;

#ifndef DESIGN_VER
		float fury;
		int explode, smoke;
#endif

		static ExplodeDlgProc *dlg;

		ExplodeAtmos();
		~ExplodeAtmos() {DeleteCriticalSection(&csect);}

		void UpdateCaches(TimeValue t);

		// Animatable/Reference
		int NumSubs() {return 1;}
		Animatable* SubAnim(int i) {return pblock;}
		TSTR SubAnimName(int i) {return _T("");}
		int NumRefs();
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		Class_ID ClassID() {return volumeExplode;}
		void GetClassName(TSTR& s) {s=EXPLODE_CLASSNAME;}
		void DeleteThis() {delete this;}
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	         PartID& partID,  RefMessage message);
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);

		// Atmospheric
		TSTR GetName() {return EXPLODE_CLASSNAME;}
		AtmosParamDlg *CreateParamDialog(IRendParams *ip);
		int RenderBegin(TimeValue t, ULONG flags);
		int RenderEnd(TimeValue t);
		void Update(TimeValue t, Interval& valid);
		void Shade(ShadeContext& sc,const Point3& p0,const Point3& p1,Color& color, Color& trans, BOOL isBG);

		int NumGizmos() {return nodes.Count();}
		INode *GetGizmo(int i) {return nodes[i];}
		void DeleteGizmo(int i);
		void InsertGizmo(int i, INode *node);
		void AppendGizmo(INode *node);
		BOOL OKGizmo(INode *node); // approve a node for possible use as gizmo
 		void EditGizmo(INode *node); // selects this gizmo & displays params for it if any

		void TraceExplosion(
			ExplodeSource &src, Ray ray, float len,
			Color &c, float &o);
	};

class ExplodeParamDlg : public AtmosParamDlg {
	public:
		ExplodeAtmos *atmos;
		IRendParams *ip;
		IParamMap *pmap;

		ExplodeParamDlg(ExplodeAtmos *a,IRendParams *i);
		Class_ID ClassID() {return volumeExplode;}
		ReferenceTarget* GetThing() {return atmos;}
		void SetThing(ReferenceTarget *m);		
		void DeleteThis();
	};


static Interval explodeIV(0,16000);

class ExplodeClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { return new ExplodeAtmos; }
	const TCHAR *	ClassName() { return EXPLODE_CLASSNAME; }
	SClass_ID		SuperClassID() { return ATMOSPHERIC_CLASS_ID; }
	Class_ID 		ClassID() { return volumeExplode; }
	const TCHAR* 	Category() { return _T("");  }
	void			ResetClassParams(BOOL fileReset) {explodeIV = Interval(0,16000);}
	};

static ExplodeClassDesc explodeCD;
ClassDesc* GetExplodeDesc() {return &explodeCD;}

#define PB_COLOR1			0
#define PB_COLOR2			1
#define PB_COLOR3			2
#define PB_FLAMETYPE		3
#define PB_STRETCH			4
#define PB_REGULARITY		5
#define PB_SCALE			6
#define PB_LEVELS			7
#define PB_DENSITY			8
#define PB_SAMPLES			9
#define PB_PHASE			10
#define PB_DRIFT			11

#ifndef DESIGN_VER
#define PB_EXPLODE			12
#define PB_SMOKE			13
#define PB_FURY				14
#endif

static int typeIDs[] = {IDC_EXPLODE_FIREBALL,IDC_EXPLODE_TENDRIL};

static ParamUIDesc descParam[] = {
	
	// Color 1
	ParamUIDesc(PB_COLOR1,TYPE_COLORSWATCH,IDC_EXPLODE_COLOR1),

	// Color 2
	ParamUIDesc(PB_COLOR2,TYPE_COLORSWATCH,IDC_EXPLODE_COLOR2),

	// Color 3
	ParamUIDesc(PB_COLOR3,TYPE_COLORSWATCH,IDC_EXPLODE_COLOR3),

	// Type
	ParamUIDesc(PB_FLAMETYPE,TYPE_RADIO,typeIDs,2),

	// Stretch
	ParamUIDesc(
		PB_STRETCH,
		EDITTYPE_FLOAT,
		IDC_EXPLODE_STRETCH,IDC_EXPLODE_STRETCHSPIN,
		0.0f,999999999.0f,
		0.1f),	
	
	// Regularity
	ParamUIDesc(
		PB_REGULARITY,
		EDITTYPE_FLOAT,
		IDC_EXPLODE_REG,IDC_EXPLODE_REGSPIN,
		0.0f,1.0f,
		0.01f),
		
	// Scale
	ParamUIDesc(
		PB_SCALE,
		EDITTYPE_FLOAT,
		IDC_EXPLODE_SCALE,IDC_EXPLODE_SCALESPIN,
		0.0f,999999999.0f,
		0.01f),
	
	// Levels
	ParamUIDesc(
		PB_LEVELS,
		EDITTYPE_FLOAT,
		IDC_EXPLODE_LEVELS,IDC_EXPLODE_LEVELSSPIN,
		1.0f,10.0f,
		0.01f),

	// Density
	ParamUIDesc(
		PB_DENSITY,
		EDITTYPE_FLOAT,
		IDC_EXPLODE_DENISITY,IDC_EXPLODE_DENISITYSPIN,
		0.0f,999999999.0f,
		0.1f),	

	// Samples
	ParamUIDesc(
		PB_SAMPLES,
		EDITTYPE_INT,
		IDC_EXPLODE_SAMPLES,IDC_EXPLODE_SAMPLESSPIN,
		1.0f,100.0f,
		0.5f),		

	// Phase
	ParamUIDesc(
		PB_PHASE,
		EDITTYPE_FLOAT,
		IDC_EXPLODE_PHASE,IDC_EXPLODE_PHASESPIN,
		0.0f,999999999.0f,
		0.01f),
	
	// Drift
	ParamUIDesc(
		PB_DRIFT,
		EDITTYPE_FLOAT,
		IDC_EXPLODE_DRIFT,IDC_EXPLODE_DRIFTSPIN,
		0.0f,999999999.0f,
		0.01f),

#ifndef DESIGN_VER
	// Explode
	ParamUIDesc(PB_EXPLODE,TYPE_SINGLECHEKBOX,IDC_EXPLODE_ON),

	// Smoke
	ParamUIDesc(PB_SMOKE,TYPE_SINGLECHEKBOX,IDC_EXPLODE_SMOKE),
	
	// Fury
	ParamUIDesc(
		PB_FURY,
		EDITTYPE_FLOAT,
		IDC_EXPLODE_FURY,IDC_EXPLODE_FURYSPIN,
		0.0f,999999999.0f,
		0.01f),	
#endif // !DESIGN_VER
	};

#ifndef DESIGN_VER
#define PARAMDESC_LENGH 15
#else
#define PARAMDESC_LENGH 12
#endif

static ParamBlockDescID descVer1[] = {
	// RB 2/15/99: These should be color controllers.
	{ TYPE_RGBA/*TYPE_POINT3*/,	NULL,	TRUE,	0 },	// color 1
	{ TYPE_RGBA/*TYPE_POINT3*/,	NULL,	TRUE,	1 },	// color 2
	{ TYPE_RGBA/*TYPE_POINT3*/,	NULL,	TRUE,	2 },	// color 3
	{ TYPE_INT,		NULL,	FALSE,	3 },	// type
	{ TYPE_FLOAT,	NULL,	TRUE,	4 },	// stretch
	{ TYPE_FLOAT,	NULL,	TRUE,	5 },	// regularity
	{ TYPE_FLOAT,	NULL,	TRUE,	6 },	// scale
	{ TYPE_FLOAT,	NULL,	TRUE,	7 },	// levels
	{ TYPE_FLOAT,	NULL,	TRUE,	8 },	// density	
	{ TYPE_INT,		NULL,	TRUE,	9 },	// samples	
	{ TYPE_FLOAT,	NULL,	TRUE,	10 },	// phase
	{ TYPE_FLOAT,	NULL,	TRUE,	11 },	// drift
#ifndef DESIGN_VER
	{ TYPE_INT,		NULL,	FALSE,	12 },	// explode
	{ TYPE_INT,		NULL,	FALSE,	13 },	// smoke
	{ TYPE_FLOAT,	NULL,	FALSE,	14 },	// fury
#endif // !DESIGN_VER
	};

#define CURRENT_DESCRIPTOR descVer1

#ifndef DESIGN_VER
#define PBLOCK_LENGTH	15
#else
#define PBLOCK_LENGTH   12
#endif

#define CURRENT_VERSION	1


//--- ExplodeDlgProc ----------------------------------------------------------

class ExplodeDlgProc : 
			public ParamMapUserDlgProc,
			public RendPickProc {
	public:
		IParamMap *pmap;
		ExplodeAtmos *atmos;
		IRendParams *ip;
		ICustButton *iPick, *iRemove;		
		ISpinnerControl *iFury;
		HWND hWnd;
		HFONT hFont;

		ExplodeDlgProc(IParamMap *pmap,ExplodeAtmos *v,IRendParams *i);
		
		void Init(HWND hWnd);		
		BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
		void DeleteThis();
		void UpdateNames();		
		void SetStates(HWND hWnd);
		void Invalidate() {pmap->Invalidate();}
		void SetupExplosion(HWND hWnd);

		// From RendPickProc
		BOOL Pick(INode *node);
		void Remove();
		BOOL Filter(INode *node);
		void EnterMode() {iPick->SetCheck(TRUE);}
		void ExitMode() {iPick->SetCheck(FALSE);}
		BOOL AllowMultiSelect() {return TRUE;}
	};

ExplodeDlgProc::ExplodeDlgProc(IParamMap *pmap,ExplodeAtmos *v,IRendParams *i) 
	{
	this->pmap = pmap;
	atmos   = v;
	ip      = i;
	iPick   = NULL;
	iFury   = NULL;
	iRemove = NULL;	
	atmos->dlg=this;
	}

void ExplodeDlgProc::DeleteThis() 
	{	
	atmos->dlg = NULL;
	ip->EndPickMode();
	ReleaseICustButton(iPick);
	ReleaseICustButton(iRemove);	
	ReleaseISpinner(iFury);
	DeleteObject(hFont);
	delete this;
	}

 // approve a node for possible use as gizmo
BOOL ExplodeAtmos::OKGizmo(INode *node) {	
	for (int i=0; i<nodes.Count(); i++) {
		if (nodes[i]==node) return FALSE;
		}
	ObjectState os = node->EvalWorldState(GetCOREInterface()->GetTime());
	if (os.obj->ClassID()==SPHEREGIZMO_CLASSID) return TRUE;
	if (os.obj->ClassID()==CYLGIZMO_CLASSID) return TRUE;
	if (os.obj->ClassID()==BOXGIZMO_CLASSID) return TRUE;
	return FALSE;
	}

 // selects this gizmo & displays params for it if any
void ExplodeAtmos::EditGizmo(INode *node) {
	for (int i=0; i<nodes.Count(); i++) {
		if ((nodes[i]==node)&&dlg) {
			SendMessage(GetDlgItem(dlg->hWnd,IDC_EXPLODE_OBJECTNAME),CB_SETCURSEL,i,0);
			}
		}
	}

void ExplodeAtmos::DeleteGizmo(int i)
	{
	if (theHold.Holding())
		theHold.Put(new DeleteGizmoRestore(this,nodes[i],i));
	theHold.Suspend();
	DeleteReference(i+OBJECT_REF);
	nodes.Delete(i,1);
	NotifyDependents(FOREVER, PART_ALL, REFMSG_SFX_CHANGE);
	if (dlg) dlg->UpdateNames();
	theHold.Resume();
	}

void ExplodeAtmos::InsertGizmo(int i, INode *node)
	{
	INode *nz = NULL;
	nodes.Insert(i, 1, &nz);
	ReplaceReference(i+OBJECT_REF,node);
	NotifyDependents(FOREVER, PART_ALL, REFMSG_SFX_CHANGE);
	if (dlg) dlg->UpdateNames();
	}

void ExplodeAtmos::AppendGizmo(INode *node)
	{
	theHold.Suspend();
	nodes.SetCount(nodes.Count()+1);
	nodes[nodes.Count()-1] = NULL;
	ReplaceReference(OBJECT_REF+nodes.Count()-1,node);
	if (dlg) dlg->UpdateNames();
	theHold.Resume();
	NotifyDependents(FOREVER, PART_ALL, REFMSG_SFX_CHANGE);
	if (theHold.Holding())
		theHold.Put(new AppendGizmoRestore(this,node));
	}

BOOL ExplodeDlgProc::Pick(INode *node)
	{
	theHold.Begin();
	atmos->AppendGizmo(node);
	theHold.Accept(GetString(IDS_DS_ADDGIZMO));
	return TRUE;
	}

void ExplodeDlgProc::Remove()
	{
	int sel = SendMessage(GetDlgItem(hWnd,IDC_EXPLODE_OBJECTNAME),CB_GETCURSEL,0,0);
	if (sel!=CB_ERR) {
		int c=0;
		for (int i=0; i<atmos->nodes.Count(); i++) {
			if (atmos->nodes[i]) {
				if (sel==c) {
					theHold.Begin();
					atmos->DeleteGizmo(i);
					theHold.Accept(GetString(IDS_DS_DELGIZMO));
//					atmos->DeleteReference(i+OBJECT_REF);
//					atmos->nodes.Delete(i,1);
//					UpdateNames();
					break;
					}
				c++;
				}
			}
		}
	}

BOOL ExplodeDlgProc::Filter(INode *node)
	{
	for (int i=0; i<atmos->nodes.Count(); i++) {
		if (atmos->nodes[i]==node) return FALSE;
		}
	ObjectState os = node->EvalWorldState(ip->GetTime());
	//return os.obj->ClassID()==SPHEREGIZMO_CLASSID;
	if (os.obj->ClassID()==SPHEREGIZMO_CLASSID) return TRUE;
	if (os.obj->ClassID()==CYLGIZMO_CLASSID) return TRUE;
	if (os.obj->ClassID()==BOXGIZMO_CLASSID) return TRUE;
	return FALSE;
	}

void ExplodeDlgProc::UpdateNames()
	{
	int c=0;
	SendMessage(GetDlgItem(hWnd,IDC_EXPLODE_OBJECTNAME),CB_RESETCONTENT,0,0);
	for (int i=0; i<atmos->nodes.Count(); i++) {
		if (atmos->nodes[i]) {
			c++;
			SendMessage(GetDlgItem(hWnd,IDC_EXPLODE_OBJECTNAME),
				CB_ADDSTRING,0,(LPARAM)(const TCHAR*)atmos->nodes[i]->GetName());
			}
		}
	SendMessage(GetDlgItem(hWnd,IDC_EXPLODE_OBJECTNAME),CB_SETCURSEL,0,0);
	if (c) {
		iRemove->Enable();
	} else {
		iRemove->Disable();
		}
	}

void ExplodeDlgProc::SetStates(HWND hWnd)
	{
#ifndef DESIGN_VER
	int explode;
	atmos->pblock->GetValue(PB_EXPLODE,0,explode,FOREVER);
	if (explode) {		
		iFury->Enable();
		EnableWindow(GetDlgItem(hWnd,IDC_EXPLODE_SMOKE),TRUE);
		EnableWindow(GetDlgItem(hWnd,IDC_EXPLODE_SETUPPHASE),TRUE);		
		EnableWindow(GetDlgItem(hWnd,IDC_EXPLODE_FURYLABEL),TRUE);		
	} else {		
		iFury->Disable();
		EnableWindow(GetDlgItem(hWnd,IDC_EXPLODE_SMOKE),FALSE);
		EnableWindow(GetDlgItem(hWnd,IDC_EXPLODE_SETUPPHASE),FALSE);		
		EnableWindow(GetDlgItem(hWnd,IDC_EXPLODE_FURYLABEL),FALSE);
		}
#endif // !DESIGN_VER
	}


void ExplodeDlgProc::Init(HWND hWnd)
	{
	this->hWnd = hWnd;
	iPick = GetICustButton(GetDlgItem(hWnd,IDC_EXPLODE_PICK));
	iPick->SetType(CBT_CHECK);
	iPick->SetHighlightColor(GREEN_WASH);
	iRemove = GetICustButton(GetDlgItem(hWnd,IDC_EXPLODE_REMOVE));	
	iFury   = GetISpinner(GetDlgItem(hWnd,IDC_EXPLODE_FURYSPIN));
	hFont   =  CreateFont(24,0,0,0,FW_BOLD,0,0,0,0,0,0,0, VARIABLE_PITCH | FF_SWISS, _T(""));
	SendDlgItemMessage(hWnd,IDC_INFERNO_TITLE,WM_SETFONT,(WPARAM)hFont,TRUE);
	UpdateNames();
	}


BOOL ExplodeDlgProc::DlgProc(
		TimeValue t,IParamMap *map,HWND hWnd,
		UINT msg,WPARAM wParam,LPARAM lParam)
	{
	switch (msg) {
		case WM_INITDIALOG:
			Init(hWnd);
			SetStates(hWnd);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_EXPLODE_PICK:
					ip->SetPickMode(this);
					break;

				case IDC_EXPLODE_REMOVE:
					Remove();
					break;

				case IDC_EXPLODE_SETUPPHASE:
					SetupExplosion(hWnd);
					break;

				case IDC_EXPLODE_ON:
					SetStates(hWnd);
					break;
				}
			break;
		}
	
	return FALSE;
	}

static BOOL CALLBACK SetupExplodeDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	static Interval *range = NULL;

	switch (msg) {
		case WM_INITDIALOG: {
			range = (Interval*)lParam;
			ISpinnerControl *spin;
			spin = GetISpinner(GetDlgItem(hWnd,IDC_EXPLODE_STARTSPIN));
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_EXPLODE_START),EDITTYPE_TIME);
			spin->SetLimits(TIME_NegInfinity,TIME_PosInfinity, FALSE);
			spin->SetValue(range->Start(),FALSE);
			spin->SetScale(10.0f);
			ReleaseISpinner(spin);

			spin = GetISpinner(GetDlgItem(hWnd,IDC_EXPLODE_ENDSPIN));
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_EXPLODE_END),EDITTYPE_TIME);
			spin->SetLimits(TIME_NegInfinity,TIME_PosInfinity, FALSE);
			spin->SetValue(range->End(),FALSE);
			spin->SetScale(10.0f);
			ReleaseISpinner(spin);

			CenterWindow(hWnd, GetParent(hWnd));
			break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: {
					ISpinnerControl *spin;
					TimeValue start, end;
					spin  = GetISpinner(GetDlgItem(hWnd,IDC_EXPLODE_STARTSPIN));
					start = spin->GetIVal();
					ReleaseISpinner(spin);
					spin  = GetISpinner(GetDlgItem(hWnd,IDC_EXPLODE_ENDSPIN));
					end   = spin->GetIVal();
					ReleaseISpinner(spin);
					range->Set(start,end);
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

void ExplodeDlgProc::SetupExplosion(HWND hWnd)
	{	
	if (!DialogBoxParam(
		hInstance,
		MAKEINTRESOURCE(IDD_EXPLODE_SETUP),
		hWnd,
		SetupExplodeDlgProc,
		(LONG)&explodeIV)) return;	
	
	// Create a fresh bezier float controller.
	Control *cont = (Control*)CreateInstance(
		CTRL_FLOAT_CLASS_ID,
		Class_ID(HYBRIDINTERP_FLOAT_CLASS_ID,0));
	if (!cont) return;
	IKeyControl *ikey = GetKeyControlInterface(cont);
	if (!ikey) {
		cont->DeleteThis();
		return;
		}
	
	// Setup a key structure with slow in and fast out tangents
	IBezFloatKey key;
	SetInTanType(key.flags,BEZKEY_SLOW);
	SetOutTanType(key.flags,BEZKEY_FAST);
	key.intan = key.outtan = 0.0f;

	// Make the first key
	key.val  = 0.0f;
	key.time = explodeIV.Start();
	ikey->AppendKey(&key);

	// Make the second key
	key.val  = 300.0f;
	key.time = explodeIV.End();
	ikey->AppendKey(&key);

	// Sort the table
	ikey->SortKeys();

	// Replace the controller in the param block with this controller.
	atmos->pblock->
		SetController(PB_PHASE,cont,FALSE);
	}

//--- ExplodeParamDlg -------------------------------------------------------

ExplodeParamDlg::ExplodeParamDlg(ExplodeAtmos *a,IRendParams *i) 
	{
	atmos = a;
	ip    = i;	
	pmap  = CreateRParamMap(
		descParam,PARAMDESC_LENGH,
		atmos->pblock,
		i,
		hInstance,
#ifndef DESIGN_VER
		MAKEINTRESOURCE(IDD_EXPLODE_PARAMS),
#else
		MAKEINTRESOURCE(IDD_VIZ_EXPLODE_PARAMS),
#endif
		GetString(IDS_RB_COMBUSTPARAMS),
		0);	
	
	pmap->SetUserDlgProc(new ExplodeDlgProc(pmap,atmos,ip));
	}

void ExplodeParamDlg::SetThing(ReferenceTarget *m)
	{
	assert(m->ClassID()==atmos->ClassID());
	atmos = (ExplodeAtmos*)m;
	pmap->SetParamBlock(atmos->pblock);	
	if (atmos->dlg) {
		atmos->dlg->atmos = atmos;
		atmos->dlg->UpdateNames();
		atmos->dlg->SetStates(atmos->dlg->hWnd);
		}
	}

void ExplodeParamDlg::DeleteThis()
	{
	DestroyRParamMap(pmap);
	delete this;
	}



//--- ExplodeAtmos -------------------------------------------------------

ExplodeDlgProc *ExplodeAtmos::dlg = NULL;

#define CF(c) (float(c)/float(255))

ExplodeAtmos::ExplodeAtmos()
	{
	InitializeCriticalSection(&csect);
	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(
			CURRENT_DESCRIPTOR, 
			PBLOCK_LENGTH, 
			CURRENT_VERSION));
	assert(pblock);		

	pblock->SetValue(PB_COLOR1,0,Point3(CF(252),CF(202),CF(  0)));
	pblock->SetValue(PB_COLOR2,0,Point3(CF(225),CF( 30),CF( 30)));	
	pblock->SetValue(PB_COLOR3,0,Point3(0.1,0.1,0.1));
	pblock->SetValue(PB_FLAMETYPE,0,0);
	pblock->SetValue(PB_STRETCH,0,1.0f);
	pblock->SetValue(PB_REGULARITY,0,0.2f);
	pblock->SetValue(PB_SCALE,0,35.0f);	
	pblock->SetValue(PB_LEVELS,0,3.0f);
	pblock->SetValue(PB_DENSITY,0,15.0f);	
	pblock->SetValue(PB_SAMPLES,0,15);			
	pblock->SetValue(PB_PHASE,0,0.0f);
	pblock->SetValue(PB_DRIFT,0,0.0f);
#ifndef DESIGN_VER
	pblock->SetValue(PB_EXPLODE,0,0);
	pblock->SetValue(PB_SMOKE,0,1);
	pblock->SetValue(PB_FURY,0,1.0f);
#endif // !DESIGN_VER
	valid.SetEmpty();
	}

#define NUMOBJECTS_CHUNK	0x0010

IOResult ExplodeAtmos::Load(ILoad *iload)
	{
	Atmospheric::Load(iload);

	ULONG nb;
	int num;
	IOResult res = IO_OK;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch (iload->CurChunkID()) {
			case NUMOBJECTS_CHUNK: {
				res = iload->Read(&num,sizeof(num),&nb);
				nodes.SetCount(num);
				for (int i=0; i<num; i++) nodes[i] = NULL;
				break;
				}
			}
		iload->CloseChunk();
		if (res!=IO_OK)  return res;
		}	
	
	return IO_OK;
	}

IOResult ExplodeAtmos::Save(ISave *isave)
	{
	Atmospheric::Save(isave);

	ULONG nb;
	int num = nodes.Count();
		
	isave->BeginChunk(NUMOBJECTS_CHUNK);
	isave->Write(&num,sizeof(num),&nb);
	isave->EndChunk();	
	
	return IO_OK;
	}

int ExplodeAtmos::NumRefs() 
	{
	return 1+nodes.Count();
	}

RefTargetHandle ExplodeAtmos::GetReference(int i) 
	{
	switch (i) {
		case PBLOCK_REF:	return pblock;		
		default:
			if (i>=OBJECT_REF) return nodes[i-OBJECT_REF];
			else return NULL;			
		}
	}

void ExplodeAtmos::SetReference(int i, RefTargetHandle rtarg) 
	{	
	switch (i) {
		case PBLOCK_REF:	pblock = (IParamBlock*)rtarg; break;
		default:
			if (i>=OBJECT_REF) 
				nodes[i-OBJECT_REF] = (INode*)rtarg; 
			break;
		}
	}
			
RefResult ExplodeAtmos::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
	{
	switch (message) {
		case REFMSG_TARGET_DELETED: {
			for (int i=0; i<nodes.Count(); i++) {
				if (hTarget==nodes[i]) {
					nodes[i] = NULL;					
					}
				}
			if (dlg) dlg->UpdateNames();
			break;
			}

		case REFMSG_CHANGE:
			valid.SetEmpty();
			if (dlg)
				dlg->Invalidate();
			break;

		case REFMSG_SUBANIM_STRUCTURE_CHANGED:
		case REFMSG_NODE_NAMECHANGE:
			if (dlg) dlg->UpdateNames();
			break;

		case REFMSG_GET_PARAM_DIM: {
			GetParamDim *gpd = (GetParamDim*)partID;
			switch (gpd->index) {			
				case PB_COLOR1: 	gpd->dim = stdColor255Dim; break;
				case PB_COLOR2: 	gpd->dim = stdColor255Dim; break;
				case PB_COLOR3: 	gpd->dim = stdColor255Dim; break;
				default: 			gpd->dim = defaultDim;
				}
			return REF_STOP; 
			}

		case REFMSG_GET_PARAM_NAME: {
			GetParamName *gpn = (GetParamName*)partID;
			switch (gpn->index) {				
				case PB_COLOR1:		gpn->name = GetString(IDS_RB_INNERCOLOR); break;
				case PB_COLOR2:		gpn->name = GetString(IDS_RB_OUTERCOLOR); break;
				case PB_COLOR3:		gpn->name = GetString(IDS_RB_SMOKECOLOR); break;
				case PB_STRETCH:	gpn->name = GetString(IDS_RB_STRETCH); break;
				case PB_REGULARITY:	gpn->name = GetString(IDS_RB_REGULARITY); break;
				case PB_SCALE:		gpn->name = GetString(IDS_RB_FLAMESIZE); break;
				case PB_LEVELS:		gpn->name = GetString(IDS_RB_FLAMEDETAIL); break;
				case PB_DENSITY:	gpn->name = GetString(IDS_RB_DENSITY); break;				
				case PB_PHASE:		gpn->name = GetString(IDS_RB_PHASE); break;
				case PB_DRIFT:		gpn->name = GetString(IDS_RB_DRIFT); break;
				case PB_FLAMETYPE:	gpn->name = GetString(IDS_RB_FLAMETYPE); break;
				case PB_SAMPLES:	gpn->name = GetString(IDS_RB_SAMPLES); break;
#ifndef DESIGN_VER
				case PB_EXPLODE:	gpn->name = GetString(IDS_RB_EXPLOSION); break;
				case PB_SMOKE:		gpn->name = GetString(IDS_RB_SMOKE); break;
				case PB_FURY:		gpn->name = GetString(IDS_RB_FURY); break;
#endif // !DESIGN_VER
				default:			gpn->name = _T("?????"); break;
				}
			return REF_STOP; 
			}
		}
	return REF_SUCCEED;
	}

AtmosParamDlg *ExplodeAtmos::CreateParamDialog(IRendParams *ip)
	{
	return new ExplodeParamDlg(this,ip);
	}

void ExplodeAtmos::UpdateCaches(TimeValue t)
	{		
	EnterCriticalSection(&csect);
	if (!valid.InInterval(t)) {
		valid = FOREVER;				
		pblock->GetValue(PB_COLOR1,t,color1,valid);
		pblock->GetValue(PB_COLOR2,t,color2,valid);
		pblock->GetValue(PB_COLOR3,t,color3,valid);
		pblock->GetValue(PB_FLAMETYPE,t,invert,valid);
		pblock->GetValue(PB_STRETCH,t,stretch,valid);
		pblock->GetValue(PB_REGULARITY,t,regularity,valid);
		pblock->GetValue(PB_SCALE,t,scale,valid);
		pblock->GetValue(PB_LEVELS,t,levels,valid);
		pblock->GetValue(PB_DENSITY,t,density,valid);
		pblock->GetValue(PB_SAMPLES,t,samples,valid);		
		pblock->GetValue(PB_PHASE,t,phase,valid);
		pblock->GetValue(PB_DRIFT,t,drift,valid);
#ifndef DESIGN_VER
		pblock->GetValue(PB_EXPLODE,t,explode,valid);
		pblock->GetValue(PB_SMOKE,t,smoke,valid);
		pblock->GetValue(PB_FURY,t,fury,valid);
#endif
		density /= 50.0f;
		if (scale != 0.0f) scale = 1.0f/scale;
		if (stretch !=0.0f) stretch = 1.0f/stretch;

#ifndef DESIGN_VER
		if (explode && (phase<0.0f || phase>300.0f)) {
			// Outside of explosion.
			rfact   = 0.0f;
			density = 0.0f;

		} else if (explode) {
			// In explosion			

			// First compute radius factor			
			rfact = (float)sqrt(phase/100.0f);

			// next density
			if (phase>100.0f) {
				float u = 1.0f - (phase-100.0f)/200.0f;				
				density *= (u*u * (3.0f - 2.0f*u));
				}

			// Interpolate to smoke color
			if (smoke) {
				if (phase>200.0f) {
					// Solid smoke
					color1 = color2 = color3;
				} else if (phase>100.0f) {
					// Interpolate to smoke color
					float u = (phase-100.0f)/100.0f;
					u = (u*u * (3.0f - 2.0f*u));
					color1 += color3*u - color1*u;
					color2 += color3*u - color2*u;
					}
				}

			// Convert phase to a reasonable value and
			// add in fury factor
			phase = float(sqrt(phase/75.0f))/2.0f * 5.0f * fury;
		
		} else 
#endif // !DESIGN_VER
		{			
			// Not doing an explosion
			rfact = 1.0f;
			phase = phase/300.0f * 5.0f;
			}
		
		}
	LeaveCriticalSection(&csect);
	}

int ExplodeAtmos::RenderBegin(TimeValue t,ULONG flags)
	{		
  	return 0;
	}

int ExplodeAtmos::RenderEnd(TimeValue t)
	{	
	sources.Resize(0);
	return 0;
	}


ExplodeSource::ExplodeSource(
		INode *node,SphereGizmoObject *obj, 
		float drift, float rfact,
		TimeValue t, Interval &valid)
	{
	int seed;
	//tm = Inverse(node->GetNodeTM(t,&valid));
	tm = Inverse(node->GetObjTMAfterWSM(t,&valid));

	if (obj->ClassID()==SPHEREGIZMO_CLASSID)
		type = SPHERE_GIZMO;
	else if (obj->ClassID()==CYLGIZMO_CLASSID)
		type = CYL_GIZMO;
	else if (obj->ClassID()==BOXGIZMO_CLASSID)
		type = BOX_GIZMO;
	
	switch (type) {
		case SPHERE_GIZMO:
			obj->pblock->GetValue(PB_GIZMO_RADIUS,t,radius,valid);
			obj->pblock->GetValue(PB_GIZMO_HEMI,t,hemi,valid);
			obj->pblock->GetValue(PB_GIZMO_SEED,t,seed,valid);
			radius   *= rfact;
			radius2   = radius*radius;
			hemiRange = radius/5.0f;
			hemiLen   = 2.0f*hemiRange;
			break;

		case CYL_GIZMO:
			obj->pblock->GetValue(PB_CYLGIZMO_RADIUS,t,radius,valid);
			obj->pblock->GetValue(PB_CYLGIZMO_HEIGHT,t,height,valid);
			obj->pblock->GetValue(PB_CYLGIZMO_SEED,t,seed,valid);
			radius *= rfact;
			radius2 = radius*radius;
			height2 = height*0.5f;
			break;

		case BOX_GIZMO:			
			obj->pblock->GetValue(PB_BOXGIZMO_LENGTH,t,length,valid);
			obj->pblock->GetValue(PB_BOXGIZMO_WIDTH, t,width,valid);
			obj->pblock->GetValue(PB_BOXGIZMO_HEIGHT,t,height,valid);
			obj->pblock->GetValue(PB_BOXGIZMO_SEED,t,seed,valid);			
			length *= 0.5f * rfact;
			width  *= 0.5f * rfact;
			height *= rfact;
			height2 = height*0.5f;
			break;
		}
			
	srand(seed);
	seedPt.x  = float(rand())/float(RAND_MAX) * 1000.0f;
	seedPt.y  = float(rand())/float(RAND_MAX) * 1000.0f;
	seedPt.z  = float(rand())/float(RAND_MAX) * 1000.0f;
	seedPt.z -= drift;
	}

float ExplodeSource::ComputeTaper(Point3 p)
	{
	switch (type) {
		case SPHERE_GIZMO: {
			float u = DotProd(p,p)/radius2;
			if (hemi) {
				//float u2 = 1.0f - p.z/radius;
				float u2 = 1.0f - (p.z + hemiRange)/hemiLen;
				if (u2>u) u = u2;
				}
			return u;
			}
		
		case CYL_GIZMO: {
			Point3 pp = p;			
			pp.z = 0.0f;
			float u  = DotProd(pp,pp)/radius2;
			float u2 = p.z/height2 - 1.0f;
			float u3 = -u2;
			if (u2>u) u = u2;
			if (u3>u) u = u3;
			return u;
			}

		case BOX_GIZMO: {
			float u2;
			float u  = (float)fabs(p.x/width);			
			u2 = (float)fabs(p.y/length);
			if (u2>u) u = u2;			
			u2 = (float)fabs(p.z/height2 - 1.0f);
			if (u2>u) u = u2;			
			return u;
			}
		}
	return 0.0f;
	}

void ExplodeAtmos::Update(TimeValue t, Interval& valid)
	{		
	UpdateCaches(t);
	sources.Resize(0);
	for (int i=0; i<nodes.Count(); i++) {
		if (!nodes[i]) continue;
		ObjectState os = nodes[i]->EvalWorldState(t);
		if (os.obj->ClassID()==SPHEREGIZMO_CLASSID ||
			os.obj->ClassID()==CYLGIZMO_CLASSID ||
			os.obj->ClassID()==BOXGIZMO_CLASSID ) {
			ExplodeSource es(
				nodes[i],
				(SphereGizmoObject*)os.obj,
				drift, rfact,
				t, valid);
			sources.Append(1,&es,10);
			}
		}
	}

// Basically a turbulence function
inline float NoiseFunc(
		Point3 p,float phase,float levels,int invert) {
	float sum = 0.0f;
	float l,f = 1.0f;
	for (l = levels; l>=1.0f; l-=1.0f) {
		sum += (float)fabs(noise4(p*f,phase))/f;
		f *= 2.0f;
		}
	if (l>0.0f)
		sum += l*(float)fabs(noise4(p*f,phase))/f;
	if (invert) {
		sum = 0.5f-sum;
		return sum>0.0f?sum:0.0f;
	} else {
		return sum;
		}
	}


#define Dx (ray.dir.x)
#define Dy (ray.dir.y)
#define Dz (ray.dir.z)
#define Px (ray.p.x)
#define Py (ray.p.y)
#define Pz (ray.p.z)

#define DIR(i) ray.dir[i]
#define POS(i)   ray.p[i]
#define BIGFLOAT 1.0e15f


inline BOOL IntersectSphere(
		Ray &ray,float &t0,float &t1,float r, int hemi, float hemiLen)
	{	
	float a, b, c, ac4, b2;
	float root;	

	a = DotProd(ray.dir,ray.dir);
	b = DotProd(ray.dir,ray.p) * 2.0f;
	c = DotProd(ray.p,ray.p) - r*r;
	
	ac4 = 4.0f * a * c;
	b2 = b*b;

	if (ac4 > b2) return FALSE;
	
	root = float(sqrt(b2-ac4));
	t0 = (-b + root) / (2.0f * a);
	t1 = (-b - root) / (2.0f * a);
	if (t0 > t1) {float temp=t0;t0=t1;t1=temp;}

	if (hemi) {
		int behind  = 0;	
		if (Pz + t0*Dz < -hemiLen) behind = 1;
		if (Pz + t1*Dz < -hemiLen) behind +=2;	
		switch (behind) { 		
			case 1: t0 = -(Pz+hemiLen)/Dz; 	break;  // intersect with plane z==-hemiRange
			case 2: t1 = -(Pz+hemiLen)/Dz; 	break;	// intersect with plane z==-hemiRange
			case 3: return FALSE;
			}
		}

	return TRUE;
	}

inline BOOL IntersectCyl(Ray &ray,float &t0,float &t1, float Rsq, float height) 
{
	// mjm - 1.21.99 - reworked function to handle cylinders with a negative height
	float A,B,C;
	A = (Dx*Dx + Dy*Dy);
	B = 2.0f*(Dx*Px + Dy*Py);
	C = Px*Px + Py*Py - Rsq;
	if (fabs(A)<.0000001) 
		return FALSE;
	float d = B*B-4.0f*A*C;
	if (d<0.0f) 
		return FALSE;
	float s = (float)sqrt(d);
	t0 = (-B + s)/(2.0f*A);
	t1 = (-B - s)/(2.0f*A);
	if (t0 < 0.0f && t1 < 0.0f)		// volume is totally behind camera
		return FALSE;

	if (t0 > t1)
		{ float temp=t0; t0=t1; t1=temp; }

	float cylBotZ(min(0.0f, height)), cylTopZ(max(0.0f, height)), rayNearZ(Pz + t0*Dz), rayFarZ(Pz + t1*Dz);

	if (rayNearZ > cylTopZ)
	{
		if (rayFarZ > cylTopZ)
			return FALSE;			// ray passes over top of cylinder
		else if (rayFarZ < cylBotZ)
			t1 = (cylBotZ-Pz)/Dz;	// far intersection with cylinder bottom
		t0 = (cylTopZ-Pz)/Dz;		// near intersection with cylinder top
	}
	else if (rayNearZ < cylBotZ)
	{
		if (rayFarZ > cylTopZ)
			t1 = (cylTopZ-Pz)/Dz;	// far intersection with cylinder top
		else if (rayFarZ < cylBotZ)
			return FALSE;			// ray passes under bottom of cylinder
		t0 = (cylBotZ-Pz)/Dz;		// near intersection with cylinder bottom
	}
	else							// near intersection with side of cylinder
	{
		if (rayFarZ > cylTopZ)
			t1 = (cylTopZ-Pz)/Dz;	// far intersection with cylinder top
		else if (rayFarZ < cylBotZ)
			t1 = (cylBotZ-Pz)/Dz;	// far intersection with cylinder top
	}

	return TRUE;
}

inline BOOL IntersectBox(Ray &ray,float &t0,float &t1, Box3& b) 
	{
	float q0 = -BIGFLOAT;
	float q1 =  BIGFLOAT;
	for (int i=0; i<3; i++) {
		if (DIR(i)==0.0f) {	// parallel to this pair of planes
			if ( POS(i)<b.pmin[i] || POS(i)>b.pmax[i] ) return FALSE;
			}
		else {
			float r0 = (b.pmin[i]-POS(i))/DIR(i);
			float r1 = (b.pmax[i]-POS(i))/DIR(i);
			if (r0 > r1) {float temp=r0; r0=r1; r1=temp;}
			if (r0 > q0) q0 = r0;					
			if (r1 < q1) q1 = r1;					
			if (q0>q1) return FALSE;   // missed box
			if (q1<0.0f) return FALSE; // Box is behind ray origin.
			}
		}
	t0 = q0;
	t1 = q1;
	return TRUE;
	}



void ExplodeAtmos::TraceExplosion(
		ExplodeSource &src, Ray ray, float len,
		Color &c, float &o)
	{
	float t0, t1, dist, dt, opac, u, n;
	Point3 dpt, pt, npt;

	// If the radius is 0, then there's nothing to do
	if (src.type==SPHERE_GIZMO && src.radius<=0.0f) return;

	// Transform the ray into object space;
	ray.p   = ray.p * src.tm;
	ray.dir = VectorTransform(src.tm,ray.dir);

	// Intersect the ray with the explosion sphere
#ifdef OLDWAY
	if (!IntersectSphere(
		ray, t0, t1, src.radius)) return;
#else
	// Intersect it with the gizmo
	switch (src.type) {
		case CYL_GIZMO:
			if (!IntersectCyl(ray,t0,t1,src.radius2,src.height)) {
				return;
				}
			break;
		
		case BOX_GIZMO: {
			Box3 b;
			b.pmin = Point3(-src.width,-src.length, 0.0f);
			b.pmax = Point3( src.width, src.length, src.height);
			if (!IntersectBox(ray,t0,t1,b)) {
				return;
				}
			break;
			}

		case SPHERE_GIZMO:							
			if (!IntersectSphere(ray,t0,t1,src.radius,src.hemi,src.hemiLen)) {
				return;
				}
			break;
		}
#endif

	// We may be inside the sphere
	if (t0<0.0f) t0 = 0.0f;
	if (t1>len)  t1 = len;

	// The sphere may be in front, out of reach
	if (t0>len)
		return;

	// The sphere may be behind us
	if (t1<0.0f)
		return;
	 
	// Setup everything
	dist = t1-t0;
	dt   = dist/float(samples);
	pt   = ray.p + t0*ray.dir;
	dpt  = ray.dir * dt;

	// March along the ray
	for (int i=0; i<samples; i++,pt += dpt) {
		
#ifdef OLDWAY
		// Below the hemisphere range
		if (src.hemi && pt.z<-src.hemiRange) continue;

		// Distance from the origin
		u = DotProd(pt,pt)/src.radius2;		

		// Check hemisphere
		if (src.hemi && pt.z<src.hemiRange) {			
			// Use the falloff due to the hemisphere if
			// it's larger.
			float u2 = 1.0f-(pt.z + src.hemiRange)/src.hemiLen;
			if (u2>u) u = u2;
			}
#else
		// Compute percentage toward edge of volume		
		u = src.ComputeTaper(pt);		
#endif

		// Noise at this point
		npt = pt + src.seedPt; // offset by seed point
		npt.z *= stretch;      // apply stretch factor
		npt   *= scale;        // apply scaling
		n = NoiseFunc(npt,phase,levels,invert);
		
		// Make the noise more sparse as we reach the outer
		// parts of the explosion.
		if (u>regularity) {
			n -= u-regularity;
			if (n<0.0f) n = 0.0f;
			}

		// Compute opacity of this segment
		opac = (1.0f-u)*n*dt*density;

		// Add to color
		c += (color1*n + (1.0f-n)*color2)*opac;

		// Add to opacity
		o += opac;
		}
	}

void ExplodeAtmos::Shade(
		ShadeContext& sc,const Point3& p0,const Point3& p1,
		Color& color, Color& trans, BOOL isBG)
	{
	if (sc.ProjType()==PROJ_PARALLEL) return;
	if (!sources.Count()) return;
	UpdateCaches(sc.CurTime());	

	if (density<=0.0f) return;

	Point3 wp0, wp1, v;
	float len;
	Ray ray;

	// Setup the ray
	wp0   = sc.PointTo(p0,REF_WORLD);
	wp1   = sc.PointTo(p1,REF_WORLD);
	ray.p = wp0;
	v     = wp1-wp0;
	len   = Length(v);
	if (len==0.0f) return;
	ray.dir = v/len;

	// Start out color is clear and black
	Color c;
	c.Black();
	float o = 0.0f;

	// Trace each explosion sphere
	for (int i=0; i<sources.Count(); i++) {
		TraceExplosion(sources[i],ray,len,c,o);
		}

	// Exponentiate
	c.r = 1.0f - (float)exp(-c.r);
	c.g = 1.0f - (float)exp(-c.g);
	c.b = 1.0f - (float)exp(-c.b);
	o   = 1.0f - (float)exp(-o);
	
	// Combine with incoming color.
	color += c-(color*o);
	trans *= 1.0f-o;
	}

