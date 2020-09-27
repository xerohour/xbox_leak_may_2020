#include "knotbase.h"
#include "iparamm.h"
#include "texutil.h"

static Class_ID RINGWAVE_CLASS_ID(0x28e41f64, 0x124b5312);
const float HalfPI=1.570796327f;
const float QuarterPI=0.785398163f;
#define A_RENDER			A_PLUGIN1

class RingWaveObject : public SimpleObject {
	public:
		// Class vars
		static IParamMap *pmapParam;		
		static IObjParam *ip;
		int FToTick;
		static HWND hparam;
		static RingWaveObject *reditOb;
		RingWaveObject();
		// From Object
		int CanConvertToType(Class_ID obtype);
		Object* ConvertToType(TimeValue t, Class_ID obtype);
//		int IsRenderable() {return 0;};
		// From BaseObject
		CreateMouseCallBack* GetCreateMouseCallBack();
		void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);
		TCHAR *GetObjectName() {  return GetString(IDS_EP_RINGWAVE); }

		// Animatable methods
		void MapKeys(TimeMap *map,DWORD flags);
		void DeleteThis() { delete this; }
		Class_ID ClassID() { return RINGWAVE_CLASS_ID; }  
		
		// From ref
		RefTargetHandle Clone(RemapDir& remap = NoRemap());

		// From SimpleObject
		void BuildMesh(TimeValue t);
		BOOL OKtoDisplay(TimeValue t);
		void InvalidateUI();
		ParamDimension *GetParameterDim(int pbIndex);
		TSTR GetParameterName(int pbIndex);		
		//mine
	};				


//--- ClassDescriptor and class vars ---------------------------------

class RingWaveObjClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) {return new RingWaveObject;}
	const TCHAR *	ClassName() { return GetString(IDS_EP_RINGWAVE); }
	SClass_ID		SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	Class_ID		ClassID() { return RINGWAVE_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_AP_EXTENDED);}	
	void			ResetClassParams(BOOL fileReset);
	};

static RingWaveObjClassDesc RingWaveObjDesc;

ClassDesc* GetRingWaveObjDesc() { return &RingWaveObjDesc; }

extern HINSTANCE hInstance;

IObjParam *RingWaveObject::ip         = NULL;
IParamMap *RingWaveObject::pmapParam  = NULL;	
HWND RingWaveObject::hparam;
RingWaveObject *RingWaveObject::reditOb              = NULL;
		
#define PB_TIMEON		0
#define PB_TIMEGROW		1
#define PB_TIMEOFF		2
#define PB_REPEAT		3
#define PB_MAXD			4
#define PB_RWIDTH		5
#define PB_RES			6
#define PB_OEBREAKON	7
#define PB_OENCYC1		8
#define PB_OEDWID1		9
#define PB_OEPER1		10
#define PB_OENCYC2		11
#define PB_OEDWID2		12
#define PB_OEPER2		13
#define PB_IEBREAKON	14
#define PB_IENCYC1		15
#define PB_IEDWID1		16
#define PB_IEPER1		17
#define PB_IENCYC2		18
#define PB_IEDWID2		19
#define PB_IEPER2		20
#define PB_HEIGHT		21
#define PB_HEIGHTSEGS	22
#define PB_RADSEGS		23

static int repeattypeIDs[] = {IDC_WAVEREPEAT,IDC_WAVEGROWANDHOLD,IDC_WAVESTATIC};

static ParamUIDesc descParamRingWave[] = {

	// time on
	ParamUIDesc(
		PB_TIMEON,
		EDITTYPE_TIME,
		IDC_DW_FIRSTBLAST,IDC_DW_FIRSTBLASTSPIN,
		-999999999.0f,999999999.0f,
		10.0f),

	// grow time
	ParamUIDesc(
		PB_TIMEGROW,
		EDITTYPE_TIME,
		IDC_DW_GROWTIME,IDC_DW_GROWTIMESPIN,
		0.0f,999999999.0f,
		10.0f),

	// display until
	ParamUIDesc(
		PB_TIMEOFF,
		EDITTYPE_TIME,
		IDC_DW_OFFTIME,IDC_DW_OFFTIMESPIN,
		-999999999.0f,999999999.0f,
		10.0f),
	
	// Repeats On
	ParamUIDesc(PB_REPEAT,TYPE_RADIO,repeattypeIDs,3),

	// Max Diameter
	ParamUIDesc(
		PB_MAXD,
		EDITTYPE_UNIVERSE,
		IDC_EMITTER_DIAM,IDC_EMITTER_DIAMSPIN,
		0.0f,999999999.0f,
		1.0f),
		
	// Ring Width
	ParamUIDesc(
		PB_RWIDTH,
		EDITTYPE_UNIVERSE,
		IDC_RW_WIDTH,IDC_RW_WIDTHSPIN,
		0.0f,999999999.0f,
		1.0f),	

	// Ring Resolution
	ParamUIDesc(
		PB_RES,
		EDITTYPE_INT,
		IDC_RES,IDC_RESSPIN,
		3.0f,1000.0f,
		1.0f),

	// Outer Edge Break On
	ParamUIDesc(PB_OEBREAKON,TYPE_SINGLECHEKBOX,IDC_OE_BREAKSON),

	// Major Cycle Count
	ParamUIDesc(
		PB_OENCYC1,
		EDITTYPE_INT,
		IDC_OE_NCYC1,IDC_OE_NCYC1SPIN,
		0.0f,100000.0f,
		1.0f),
		
	// Major Cycle Width Flux
	ParamUIDesc(
		PB_OEDWID1,
		EDITTYPE_FLOAT,
		IDC_OE_WID1,IDC_OE_WID1SPIN,
		0.0f,100.0f,
		1.0f,
		stdPercentDim),	

	// Major Cycle Flux Period
	ParamUIDesc(
		PB_OEPER1,
		EDITTYPE_TIME,
		IDC_OE_CRAWLT1,IDC_OE_CRAWLT1SPIN,
		-99999999.0f,99999999.0f,
		10.0f),

	// Minor Cycle Count
	ParamUIDesc(
		PB_OENCYC2,
		EDITTYPE_INT,
		IDC_OE_NCYC2,IDC_OE_NCYC2SPIN,
		0.0f,100000.0f,
		1.0f),
		
	// Minor Cycle Width Flux
	ParamUIDesc(
		PB_OEDWID2,
		EDITTYPE_FLOAT,
		IDC_OE_WID2,IDC_OE_WID2SPIN,
		0.0f,100.0f,
		1.0f,
		stdPercentDim),	

	// Minor Cycle Flux Period
	ParamUIDesc(
		PB_OEPER2,
		EDITTYPE_TIME,
		IDC_OE_CRAWLT2,IDC_OE_CRAWLT2SPIN,
		-99999999.0f,99999999.0f,
		10.0f),

	// Inner Edge Break On
	ParamUIDesc(PB_IEBREAKON,TYPE_SINGLECHEKBOX,IDC_IE_BREAKSON),

	// Major Cycle Count
	ParamUIDesc(
		PB_IENCYC1,
		EDITTYPE_INT,
		IDC_IE_NCYC1,IDC_IE_NCYC1SPIN,
		0.0f,100000.0f,
		1.0f),
		
	// Major Cycle Width Flux
	ParamUIDesc(
		PB_IEDWID1,
		EDITTYPE_FLOAT,
		IDC_IE_WID1,IDC_IE_WID1SPIN,
		0.0f,100.0f,
		1.0f,
		stdPercentDim),	

	// Major Cycle Flux Period
	ParamUIDesc(
		PB_IEPER1,
		EDITTYPE_TIME,
		IDC_IE_CRAWLT1,IDC_IE_CRAWLT1SPIN,
		-99999999.0f,99999999.0f,
		10.0f),

	// Minor Cycle Count
	ParamUIDesc(
		PB_IENCYC2,
		EDITTYPE_INT,
		IDC_IE_NCYC2,IDC_IE_NCYC2SPIN,
		0.0f,100000.0f,
		1.0f),
		
	// Minor Cycle Width Flux
	ParamUIDesc(
		PB_IEDWID2,
		EDITTYPE_FLOAT,
		IDC_IE_WID2,IDC_IE_WID2SPIN,
		0.0f,100.0f,
		1.0f,
		stdPercentDim),	

	// Minor Cycle Flux Period
	ParamUIDesc(
		PB_IEPER2,
		EDITTYPE_TIME,
		IDC_IE_CRAWLT2,IDC_IE_CRAWLT2SPIN,
		-99999999.0f,99999999.0f,
		10.0f),

	// Height
	ParamUIDesc(
		PB_HEIGHT,
		EDITTYPE_UNIVERSE,
		IDC_HINNER,IDC_HINNERSPIN,
		0.0f,999999999.0f,
		1.0f),

	// Height Segs
	ParamUIDesc(
		PB_HEIGHTSEGS,
		EDITTYPE_INT,
		IDC_HSEGS,IDC_HSEGSSPIN,
		1.0f,1000.0f,
		1.0f),

	// Radius Segs
	ParamUIDesc(
		PB_RADSEGS,
		EDITTYPE_INT,
		IDC_RSEGS,IDC_RSEGSSPIN,
		1.0f,1000.0f,
		1.0f),
};

#define RINGWAVEPARAMDESC_LENGTH 24

static ParamBlockDescID descVer0[] = {
	{ TYPE_INT, NULL, TRUE, 0 }, // start time
	{ TYPE_INT, NULL, FALSE, 1 },// grow time
	{ TYPE_INT, NULL, TRUE, 2 },// display until
	{ TYPE_INT, NULL, FALSE, 3 },// repeats
	{ TYPE_FLOAT, NULL, TRUE, 4 },// max dia
	{ TYPE_FLOAT, NULL, TRUE, 5 },// ring width
	{ TYPE_INT, NULL, TRUE, 6 },// ring resolution

	{ TYPE_INT, NULL, FALSE, 7 },// outer edge breakup on
	{ TYPE_INT, NULL, FALSE, 8 },// major cycs
	{ TYPE_FLOAT, NULL, TRUE, 9 },// width flux
	{ TYPE_INT, NULL, TRUE, 10 },// crawl time
	{ TYPE_INT, NULL, FALSE, 11 },// minor cycs
	{ TYPE_FLOAT, NULL, TRUE, 12 },// width flux
	{ TYPE_INT, NULL, TRUE, 13 },// crawl time

	{ TYPE_INT, NULL, FALSE, 14 },// inner edge breakup on
	{ TYPE_INT, NULL, FALSE, 15 },// major cycs
	{ TYPE_FLOAT, NULL, TRUE, 16 },// width flux
	{ TYPE_INT, NULL, TRUE, 17 },// crawl time
	{ TYPE_INT, NULL, FALSE, 18 },// minor cycs
	{ TYPE_FLOAT, NULL, TRUE, 19 },// width flux
	{ TYPE_INT, NULL, TRUE, 20 },// crawl time

	{ TYPE_FLOAT, NULL, TRUE, 21 },// height 
	{ TYPE_INT, NULL, TRUE, 22 }, // height segs
	{ TYPE_INT, NULL, TRUE, 23 }, // rad segs
};

#define PBLOCK_LENGTH_RINGWAVE 24

void RingWaveObjClassDesc::ResetClassParams(BOOL fileReset){ }

//--- Parameter map/block descriptors -------------------------------

#define NUM_OLDVERSIONS	0
#define CURRENT_VERSION	0

static ParamVersionDesc curVersion(descVer0,PBLOCK_LENGTH_RINGWAVE,CURRENT_VERSION);

void OuterOn(HWND hwnd)
{ SpinnerOn(hwnd,IDC_OE_NCYC1SPIN);
  SpinnerOn(hwnd,IDC_OE_WID1SPIN);
  SpinnerOn(hwnd,IDC_OE_CRAWLT1SPIN);
  SpinnerOn(hwnd,IDC_OE_NCYC2SPIN);
  SpinnerOn(hwnd,IDC_OE_WID2SPIN);
  SpinnerOn(hwnd,IDC_OE_CRAWLT2SPIN);
}
void OuterOff(HWND hwnd)
{ SpinnerOff(hwnd,IDC_OE_NCYC1SPIN);
  SpinnerOff(hwnd,IDC_OE_WID1SPIN);
  SpinnerOff(hwnd,IDC_OE_CRAWLT1SPIN);
  SpinnerOff(hwnd,IDC_OE_NCYC2SPIN);
  SpinnerOff(hwnd,IDC_OE_WID2SPIN);
  SpinnerOff(hwnd,IDC_OE_CRAWLT2SPIN);
}
void InnerOn(HWND hwnd)
{ SpinnerOn(hwnd,IDC_IE_NCYC1SPIN);
  SpinnerOn(hwnd,IDC_IE_WID1SPIN);
  SpinnerOn(hwnd,IDC_IE_CRAWLT1SPIN);
  SpinnerOn(hwnd,IDC_IE_NCYC2SPIN);
  SpinnerOn(hwnd,IDC_IE_WID2SPIN);
  SpinnerOn(hwnd,IDC_IE_CRAWLT2SPIN);
}
void InnerOff(HWND hwnd)
{ SpinnerOff(hwnd,IDC_IE_NCYC1SPIN);
  SpinnerOff(hwnd,IDC_IE_WID1SPIN);
  SpinnerOff(hwnd,IDC_IE_CRAWLT1SPIN);
  SpinnerOff(hwnd,IDC_IE_NCYC2SPIN);
  SpinnerOff(hwnd,IDC_IE_WID2SPIN);
  SpinnerOff(hwnd,IDC_IE_CRAWLT2SPIN);
}

class RingParamDlgProc : public ParamMapUserDlgProc {
public:
  RingWaveObject *ob;

  RingParamDlgProc(RingWaveObject *o) {ob=o;}
  BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
  void Update(TimeValue t);
  void DeleteThis() {delete this;}
};

void RingParamDlgProc::Update(TimeValue t)
{ if (!ob->reditOb) return;
  int repeat,outeron,inneron;
  ob->pblock->GetValue(PB_REPEAT,0,repeat,FOREVER);
  if (repeat==2) SpinnerOff(ob->hparam,IDC_DW_GROWTIMESPIN);
  else  SpinnerOn(ob->hparam,IDC_DW_GROWTIMESPIN);
  ob->pblock->GetValue(PB_OEBREAKON,0,outeron,FOREVER);
  if (outeron) OuterOn(ob->hparam);else OuterOff(ob->hparam);
  ob->pblock->GetValue(PB_IEBREAKON,0,inneron,FOREVER);
  if (inneron) InnerOn(ob->hparam);else InnerOff(ob->hparam);  
}
static HWND hDlg;

BOOL RingParamDlgProc::DlgProc(TimeValue t,IParamMap *map,HWND
                                hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{    switch (msg) 
	  { case WM_INITDIALOG: 
			{ Update(t);
			  break;
			}
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{ case IDC_WAVESTATIC:
				{ SpinnerOff(hWnd,IDC_DW_GROWTIMESPIN);
				  return TRUE;
				}
			  case IDC_WAVEGROWANDHOLD:
			  case IDC_WAVEREPEAT:
				{ SpinnerOn(hWnd,IDC_DW_GROWTIMESPIN);
				  return TRUE;
				}
			  case IDC_OE_BREAKSON:
				{ int outeron;
				  ob->pblock->GetValue(PB_OEBREAKON,0,outeron,FOREVER);
				  if (outeron) OuterOn(ob->hparam);else OuterOff(ob->hparam);
				  return TRUE;
				}
			  case IDC_IE_BREAKSON:
				{ int inneron;
				  ob->pblock->GetValue(PB_IEBREAKON,0,inneron,FOREVER);
				  if (inneron) InnerOn(ob->hparam);else InnerOff(ob->hparam);
				  return TRUE;
				}
			}
	  }
	return FALSE;
}

RingWaveObject::RingWaveObject()
{	MakeRefByID(FOREVER, 0, CreateParameterBlock(descVer0, PBLOCK_LENGTH_RINGWAVE, CURRENT_VERSION));
	assert(pblock);
	
	FToTick=(int)((float)TIME_TICKSPERSEC/(float)GetFrameRate());
	pblock->SetValue(PB_TIMEON,0,(TimeValue)0);
	pblock->SetValue(PB_TIMEGROW,0,(TimeValue)60*FToTick);
	pblock->SetValue(PB_TIMEOFF,0,(TimeValue)100*FToTick);
	pblock->SetValue(PB_REPEAT,0,2);
	pblock->SetValue(PB_MAXD,0,500.0f);
	pblock->SetValue(PB_RWIDTH,0,1.0f);
	pblock->SetValue(PB_RES,0,200);

	pblock->SetValue(PB_OEBREAKON,0,0);
	pblock->SetValue(PB_OENCYC1,0,1);
	pblock->SetValue(PB_OEDWID1,0,0.0f);
	pblock->SetValue(PB_OEPER1,0,(TimeValue)100*FToTick);
	pblock->SetValue(PB_OENCYC2,0,1);
	pblock->SetValue(PB_OEDWID2,0,0.0f);
	pblock->SetValue(PB_OEPER2,0,-(TimeValue)100*FToTick);

	pblock->SetValue(PB_IEBREAKON,0,1);
	pblock->SetValue(PB_IENCYC1,0,11);
	pblock->SetValue(PB_IEDWID1,0,0.25f);
	pblock->SetValue(PB_IEPER1,0,(TimeValue)121*FToTick);
	pblock->SetValue(PB_IENCYC2,0,29);
	pblock->SetValue(PB_IEDWID2,0,0.1f);
	pblock->SetValue(PB_IEPER2,0,-(TimeValue)27*FToTick);
	pblock->SetValue(PB_HEIGHT,0,0.0f);
	pblock->SetValue(PB_HEIGHTSEGS,0,1);
	pblock->SetValue(PB_RADSEGS,0,1);
}

void RingWaveObject::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
	{
	SimpleObject::BeginEditParams(ip,flags,prev);
	reditOb = this;
	this->ip = ip;

	if (pmapParam) {
		
		// Left over from last Box ceated
		pmapParam->SetParamBlock(pblock);
	} else {
		
		// Gotta make a new one.

		pmapParam = CreateCPParamMap(
			descParamRingWave,RINGWAVEPARAMDESC_LENGTH,
			pblock,
			ip,
			hInstance,
			MAKEINTRESOURCE(IDD_RINGWAVE_UI),
			GetString(IDS_AP_PARAMS),
			0);
		}
	hparam=pmapParam->GetHWnd();
	if (pmapParam) pmapParam->SetUserDlgProc(new RingParamDlgProc(this));
}
		
void RingWaveObject::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
	{
	SimpleObject::EndEditParams(ip,flags,next);
	this->ip = NULL;
	reditOb=NULL;
	if (flags&END_EDIT_REMOVEUI ) {
		DestroyCPParamMap(pmapParam);
		pmapParam = NULL;
		}
	}

void RingWaveObject::MapKeys(TimeMap *map,DWORD flags)
{	Animatable::MapKeys(map,flags);
	TimeValue TempTime;
// remap values
	pblock->GetValue(PB_TIMEON,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(PB_TIMEON,0,TempTime);
	pblock->GetValue(PB_TIMEGROW,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(PB_TIMEGROW,0,TempTime);
	pblock->GetValue(PB_TIMEOFF,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(PB_TIMEOFF,0,TempTime);
	pblock->GetValue(PB_OEPER1,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(PB_OEPER1,0,TempTime);
	pblock->GetValue(PB_OEPER2,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(PB_OEPER2,0,TempTime);
	pblock->GetValue(PB_IEPER1,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(PB_IEPER1,0,TempTime);
	pblock->GetValue(PB_IEPER2,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(PB_IEPER2,0,TempTime);
}  

#define CYCLIC		0
#define GROWHOLD	1
#define STATIC		2

void RingWaveObject::BuildMesh(TimeValue t)
{	int FToTick=(int)((float)TIME_TICKSPERSEC/(float)GetFrameRate());
	int nverts,nfaces,res,i,j,repeat,hsegs, rsegs;	
	float radius,width,cyclepoint,irad,height,htop,hbot;
	TimeValue t1,t2,tg,tincycle;

	ivalid=FOREVER;
	pblock->GetValue(PB_TIMEON,t,t1,ivalid);
	pblock->GetValue(PB_TIMEGROW,t,tg,ivalid);
	pblock->GetValue(PB_TIMEOFF,t,t2,ivalid);
	pblock->GetValue(PB_REPEAT,t,repeat,ivalid);

	if (t<t1) ivalid.SetEnd(t1-1);
	else if (t>t2) ivalid.SetStart(t2);
	else ivalid.SetInstant(t);

	if ( (t<t1) || (t>t2) )
	{	mesh.setNumVerts(0);
		mesh.setNumFaces(0);
		mesh.setNumTVerts(0);
		mesh.setNumTVFaces(0);
		mesh.InvalidateGeomCache();
		return;
	}

	if (tg<FToTick) tg=FToTick;
	if (repeat==CYCLIC)
	{	tincycle=(t-t1)%tg;
		cyclepoint=(float)tincycle/(float)tg;
	}
	else if (repeat==GROWHOLD)
	{	if (t>=t1+tg)
		{	cyclepoint=1.0f;
		}
		else
		{	tincycle=(t-t1)%tg;
			cyclepoint=(float)tincycle/(float)tg;
		}
	}
	else
		cyclepoint=1.0f;

	pblock->GetValue(PB_MAXD,t,radius,ivalid);
	radius*=cyclepoint;

	pblock->GetValue(PB_RWIDTH,t,width,ivalid);
	irad=radius-width;

	pblock->GetValue(PB_HEIGHT,t,height,ivalid);
	pblock->GetValue(PB_HEIGHTSEGS,t,hsegs,ivalid);
	pblock->GetValue(PB_RADSEGS,t,rsegs,ivalid);
	htop=height*0.5f; hbot= -htop;

	int ibreakon,obreakon;
	float iwidthflux1,iwidthflux2;
	float owidthflux1,owidthflux2;
	int icycles1,icycles2;
	int ocycles1,ocycles2;
	float incycle1,incycle2;
	TimeValue iper1,iper2;
	TimeValue oper1,oper2;

	pblock->GetValue(PB_IEBREAKON,t,ibreakon,ivalid);
	pblock->GetValue(PB_OEBREAKON,t,obreakon,ivalid);

	if (ibreakon)
	{	pblock->GetValue(PB_IENCYC1,t,icycles1,ivalid);
		pblock->GetValue(PB_IEDWID1,t,iwidthflux1,ivalid);
		pblock->GetValue(PB_IEPER1,t,iper1,ivalid);
		if (fabs(iper1)<1) iper1=1;
		pblock->GetValue(PB_IENCYC2,t,icycles2,ivalid);
		pblock->GetValue(PB_IEDWID2,t,iwidthflux2,ivalid);
		pblock->GetValue(PB_IEPER2,t,iper2,ivalid);
		if (fabs(iper2)<1) iper2=1;
	}
	if (obreakon)
	{	pblock->GetValue(PB_OENCYC1,t,ocycles1,ivalid);
		pblock->GetValue(PB_OEDWID1,t,owidthflux1,ivalid);
		pblock->GetValue(PB_OEPER1,t,oper1,ivalid);
		if (fabs(oper1)<1) oper1=1;
		pblock->GetValue(PB_OENCYC2,t,ocycles2,ivalid);
		pblock->GetValue(PB_OEDWID2,t,owidthflux2,ivalid);
		pblock->GetValue(PB_OEPER2,t,oper2,ivalid);
		if (fabs(oper2)<1) oper2=1;
	}

	pblock->GetValue(PB_RES,t,res,ivalid);
	nverts=(res+1)*(rsegs+1);
	nfaces=2*res*rsegs;

	int topverts,topfaces,
		vertsinrow,facesinrow,
		mostverts,mostfaces,
		vertshere;

	BOOL isthick=FALSE;

	if (height>0.0f)
	{	isthick=TRUE;
	
		topverts=nverts;
		mostverts=topverts + 2*(res+1)*(hsegs-1);

		topfaces=nfaces;
		mostfaces=nfaces+4*res*hsegs;

		facesinrow=2*res;
		vertsinrow=2*res+2;

		nverts=topverts+mostverts;
		nfaces=topfaces+mostfaces;
	}

	mesh.setNumVerts(nverts);
	mesh.setNumFaces(nfaces);
	mesh.setNumTVerts(nverts);
	mesh.setNumTVFaces(nfaces);

	float a,b,cosu,sinu,ilocalrad,olocalrad,
		  iradcosu,oradcosu,iradsinu,oradsinu;

	float verticalportion, whereAmI, verticalmapping;

	float temp, rhere, xhere, yhere;

	for (i=0;i<=res;i++)
	{	ilocalrad=irad;
		olocalrad=radius;
		a=(float)i/(float)res;
		b=a*TWOPI;
  		cosu=(float)cos(b);
		sinu=(float)sin(b);

		if (ibreakon)
		{	incycle1=(float)(t-t1)/(float)iper1;
			incycle2=(float)(t-t1)/(float)iper2;
			ilocalrad+=width*iwidthflux1*(float)sin(TWOPI*(icycles1*a+incycle1));
			ilocalrad+=width*iwidthflux2*(float)sin(TWOPI*(icycles2*a+incycle2));
		}
		if (ilocalrad<0.0f) ilocalrad=0.0f;

		if (obreakon)
		{	incycle1=(float)(t-t1)/(float)oper1;
			incycle2=(float)(t-t1)/(float)oper2;
			olocalrad+=width*owidthflux1*(float)sin(TWOPI*(ocycles1*a+incycle1));
			olocalrad+=width*owidthflux2*(float)sin(TWOPI*(ocycles2*a+incycle2));
		}
		if (olocalrad<ilocalrad) olocalrad=ilocalrad;

		iradcosu=ilocalrad*cosu;
		iradsinu=ilocalrad*sinu;
		oradcosu=olocalrad*cosu;
		oradsinu=olocalrad*sinu;

		// top surface
		if (rsegs<2)
		{	mesh.setVert(      i, Point3(iradcosu,iradsinu,htop));
			mesh.setVert(res+1+i, Point3(oradcosu,oradsinu,htop));
			mesh.setTVert(      i,a,0.01f,0.999f);
			mesh.setTVert(res+1+i,a,0.99f,0.999f);
		}
		else
		{	for (j=0; j<=rsegs; j++)
			{	rhere = olocalrad-ilocalrad;
				temp = (float)j/(float)rsegs;
				xhere = iradcosu + temp*(oradcosu-iradcosu);
				yhere = iradsinu + temp*(oradsinu-iradsinu);
				vertshere = i+j*(res+1);
				mesh.setVert(vertshere, Point3(xhere,yhere,htop));
				mesh.setTVert(vertshere,a,temp,0.999f);
			}
		}

		// intermediate rows of vertices
		if ((isthick)&&(hsegs>1))
		{	for (j=1; j<hsegs; j++)
			{	verticalportion = (float)j/(float)hsegs;
				verticalmapping = 1.0f-verticalportion;
				whereAmI = htop-verticalportion*height;
				vertshere=topverts + vertsinrow*(j-1) + i;
				mesh.setVert(vertshere       , Point3(iradcosu,iradsinu,whereAmI));
				mesh.setVert(vertshere+res+1 , Point3(oradcosu,oradsinu,whereAmI));
				mesh.setTVert(vertshere      ,a,0.01f,verticalmapping);
				mesh.setTVert(vertshere+res+1,a,0.99f,verticalmapping);
			}
		}

		// bottom surface
		if (isthick)
		{	if (rsegs<2)
			{	mesh.setVert(mostverts+      i, Point3(iradcosu,iradsinu,hbot));
				mesh.setVert(mostverts+res+1+i, Point3(oradcosu,oradsinu,hbot));
				mesh.setTVert(mostverts+      i,a,0.01f,0.001f);
				mesh.setTVert(mostverts+res+1+i,a,0.99f,0.001f);
			}
			else
			{	for (j=0; j<=rsegs; j++)
				{	rhere = olocalrad-ilocalrad;
					temp = (float)j/(float)rsegs;
					xhere = iradcosu + temp*(oradcosu-iradcosu);
					yhere = iradsinu + temp*(oradsinu-iradsinu);
					vertshere = mostverts+i+j*(res+1);
					mesh.setVert(vertshere, Point3(xhere,yhere,hbot));
					mesh.setTVert(vertshere,a,temp,0.001f);
				}
			}
		}
    }

	int face1,face2,face3,face4,face5,face6,face7,face8;
	int vert1,vert2,vert3,vert4,
		vert1a,vert2a,vert3a,vert4a,
		vert1b,vert2b,vert3b,vert4b;

	for (i=0;i<res;i++)
	{	// top surface
		for (j=0; j<rsegs; j++)
		{	face1=2*i + 2*j*res;
			face2=face1+1;

//	vertshere = i+j*(res+1);

			vert1 = i + j*(res+1);
			vert2 = vert1+1;
			vert3 = vert2+res;
			vert4 = vert3+1;

			mesh.faces[face1].setSmGroup(1);
			mesh.faces[face1].setVerts(vert2,vert1,vert3);
			mesh.faces[face1].setEdgeVisFlags(1,1,0);
			mesh.tvFace[face1].setTVerts(vert2,vert1,vert3);

			mesh.faces[face2].setSmGroup(1);
			mesh.faces[face2].setVerts(vert3,vert4,vert2);
			mesh.faces[face2].setEdgeVisFlags(1,1,0);
			mesh.tvFace[face2].setTVerts(vert3,vert4,vert2);
		}

		// bottom surface
		if (isthick)
		{	for (j=0; j<rsegs; j++)
			{	vert1b = mostverts + i + j*(res+1);
				vert2b = vert1b+1;
				vert3b = vert2b+res;
				vert4b = vert3b+1;
	
				face7 = mostfaces + 2*i + 2*res*j;
				face8 = face7 + 1;
	
				mesh.faces[face7].setSmGroup(1);
				mesh.faces[face7].setVerts(vert3b,vert1b,vert2b);
				mesh.faces[face7].setEdgeVisFlags(1,1,0);
				mesh.tvFace[face7].setTVerts(vert3b,vert1b,vert2b);
	
				mesh.faces[face8].setSmGroup(1);
				mesh.faces[face8].setVerts(vert2b,vert4b,vert3b);
				mesh.faces[face8].setEdgeVisFlags(1,1,0);
				mesh.tvFace[face8].setTVerts(vert2b,vert4b,vert3b);
			}
		}

		// intermediate faces
		if (isthick)
		{	if (hsegs>1)
			{	for (j=1; j<=hsegs; j++)
				{	if (j==1)
					{	vert1a = i;
						vert2a = vert1a + 1;
						vert3a = i + rsegs*(res+1);
						vert4a = vert3a + 1;
	
						vert1b = i + topverts;
						vert2b = vert1b + 1;
						vert3b = vert2b + res;
						vert4b = vert3b + 1;
					}
					else if (j==hsegs)
					{	vert1b = mostverts + i;
						vert2b = vert1b + 1;
						vert1a = vert1b - vertsinrow;
						vert2a = vert2b - vertsinrow;
	
						vert3a = vert2a + res;
						vert4a = vert3a + 1;
						vert3b = vert1b + rsegs*(res+1);
						vert4b = vert3b + 1;
					}
					else
					{	vert1a = i + topverts + vertsinrow*(j-2);
						vert2a = vert1a + 1;
						vert3a = vert2a + res;
						vert4a = vert3a + 1;
	
						vert1b = vert1a + vertsinrow;
						vert2b = vert1b + 1;
						vert3b = vert2b + res;
						vert4b = vert3b + 1;
					}
			
					face3=topfaces + 2*facesinrow*(j-1) + 2*i;
					face4=face3 + 1;
					face5=face3 + facesinrow;
					face6=face5 + 1;

					mesh.faces[face3].setSmGroup(2);
					mesh.faces[face3].setVerts(vert4a,vert3a,vert3b);
					mesh.faces[face3].setEdgeVisFlags(1,1,0);
					mesh.tvFace[face3].setTVerts(vert4a,vert3a,vert3b);
		
					mesh.faces[face4].setSmGroup(2);
					mesh.faces[face4].setVerts(vert3b,vert4b,vert4a);
					mesh.faces[face4].setEdgeVisFlags(1,1,0);
					mesh.tvFace[face4].setTVerts(vert3b,vert4b,vert4a);
		
					mesh.faces[face5].setSmGroup(2);
					mesh.faces[face5].setVerts(vert1b,vert1a,vert2a);
					mesh.faces[face5].setEdgeVisFlags(1,1,0);
					mesh.tvFace[face5].setTVerts(vert1b,vert1a,vert2a);

					mesh.faces[face6].setSmGroup(2);
					mesh.faces[face6].setVerts(vert2a,vert2b,vert1b);
					mesh.faces[face6].setEdgeVisFlags(1,1,0);
					mesh.tvFace[face6].setTVerts(vert2a,vert2b,vert1b);
				}
			}
			else
			{	vert1a = i;
				vert2a = vert1a + 1;
				vert3a = i + rsegs*(res+1);
				vert4a = vert3a + 1;

				vert1b = mostverts + i;
				vert2b = vert1b + 1;
				vert3b = vert1b + rsegs*(res+1);
				vert4b = vert3b + 1;

				face3 = 2*i + topfaces;
				face4 = face3 + 1;
				face5 = face3 + facesinrow;
				face6 = face5 + 1;

				mesh.faces[face3].setSmGroup(2);
				mesh.faces[face3].setVerts(vert4a,vert3a,vert3b);
				mesh.faces[face3].setEdgeVisFlags(1,1,0);
				mesh.tvFace[face3].setTVerts(vert4a,vert3a,vert3b);
	
				mesh.faces[face4].setSmGroup(2);
				mesh.faces[face4].setVerts(vert3b,vert4b,vert4a);
				mesh.faces[face4].setEdgeVisFlags(1,1,0);
				mesh.tvFace[face4].setTVerts(vert3b,vert4b,vert4a);
	
				mesh.faces[face5].setSmGroup(2);
				mesh.faces[face5].setVerts(vert1b,vert1a,vert2a);
				mesh.faces[face5].setEdgeVisFlags(1,1,0);
				mesh.tvFace[face5].setTVerts(vert1b,vert1a,vert2a);

				mesh.faces[face6].setSmGroup(2);
				mesh.faces[face6].setVerts(vert2a,vert2b,vert1b);
				mesh.faces[face6].setEdgeVisFlags(1,1,0);
				mesh.tvFace[face6].setTVerts(vert2a,vert2b,vert1b);
			}
		}
	}

	mesh.InvalidateGeomCache();
}

Object* RingWaveObject::ConvertToType(TimeValue t, Class_ID obtype)
{	return SimpleObject::ConvertToType(t,obtype);
}

int RingWaveObject::CanConvertToType(Class_ID obtype)
{	return SimpleObject::CanConvertToType(obtype);
}

class RingWaveObjCreateCallBack : public CreateMouseCallBack {
	public:
		RingWaveObject *po;
		Point3 p0,p1,p2;
		IPoint2 sp0, sp1, sp2;
		int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	};


int RingWaveObjCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat)
{	float r,r2;
	Point3 center;
	#ifdef _OSNAP
	if (msg == MOUSE_FREEMOVE)
	{
		#ifdef _3D_CREATE
			vpt->SnapPreview(m,m,NULL, SNAP_IN_3D);
		#else
			vpt->SnapPreview(m,m,NULL, SNAP_IN_PLANE);
		#endif
	}
	#endif
	
	if (msg==MOUSE_POINT||msg==MOUSE_MOVE)
	{	switch(point)
		{	case 0:
				GetCOREInterface()->SetHideByCategoryFlags(
						GetCOREInterface()->GetHideByCategoryFlags() & ~(HIDE_OBJECTS|HIDE_PARTICLES));
				po->pblock->SetValue(PB_MAXD,0,0.0f);				
				po->suspendSnap = TRUE;				
				sp0 = m;
				#ifdef _3D_CREATE	
					p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_3D);
				#else	
					p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
				#endif
				mat.SetTrans(p0);
				break;
			case 1:
				mat.IdentityMatrix();
				sp1 = m;							   
				#ifdef _3D_CREATE	
					p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_3D);
				#else	
					p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
				#endif
				// radius	
				r = Length(p1-p0);
				mat.SetTrans(p0);
				
				if (msg==MOUSE_POINT)
				{ if (Length(sp1-sp0)<3 || (r<0.1f))
				   return CREATE_ABORT;	
				}
				po->pblock->SetValue(PB_MAXD,0,r);
				po->pmapParam->Invalidate();
				
				if (flags&MOUSE_CTRL) {
					float ang = (float)atan2(p1.y-p0.y,p1.x-p0.x);					
					mat.PreRotateZ(po->ip->SnapAngle(ang));
					}
				break;
			case 2:
				center = mat.GetTrans();
				mat.IdentityMatrix();
				mat.SetTrans(center);

				#ifdef _3D_CREATE	
					p2 = vpt->SnapPoint(m,m,NULL,SNAP_IN_3D);
				#else	
					p2 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
				#endif

				r   = Length(p1-p0);
				r2  = Length(p2-p1);

				po->pblock->SetValue(PB_MAXD,0,r);
				po->pblock->SetValue(PB_RWIDTH,0,r2);
				po->pmapParam->Invalidate();
				
				if (flags&MOUSE_CTRL) {
					float ang = (float)atan2(p2.y-p0.y,p2.x-p0.x);					
					mat.PreRotateZ(po->ip->SnapAngle(ang));
					}

				if (msg==MOUSE_POINT) {
					po->suspendSnap = FALSE;
					return CREATE_STOP;
					}
				break;					   
			}
		}
		else
	{	if (msg == MOUSE_ABORT)	return CREATE_ABORT;
	}
	return TRUE;
}

/*
	
	po->pmapParam->Invalidate();
				return CREATE_STOP;			
*/

static RingWaveObjCreateCallBack trackCreateCB;

CreateMouseCallBack* RingWaveObject::GetCreateMouseCallBack()
{	trackCreateCB.po=this;
	return(&trackCreateCB);
}


BOOL RingWaveObject::OKtoDisplay(TimeValue t) 
{  return (TRUE);
}



void RingWaveObject::InvalidateUI() 
{	if (pmapParam) pmapParam->Invalidate();
}

ParamDimension *RingWaveObject::GetParameterDim(int pbIndex) 
{	switch (pbIndex)
	{	case PB_MAXD:
		case PB_RWIDTH:
		case PB_HEIGHT:
			return stdWorldDim;
		case PB_TIMEON:
		case PB_TIMEGROW:
		case PB_TIMEOFF:
		case PB_OEPER1:
		case PB_OEPER2:
		case PB_IEPER1:
		case PB_IEPER2:
			return stdTimeDim;
		case PB_OEDWID1:
		case PB_OEDWID2:
		case PB_IEDWID1:
		case PB_IEDWID2:
			return stdPercentDim;
		default:
			return defaultDim;
	}
}

TSTR RingWaveObject::GetParameterName(int pbIndex)
{	switch (pbIndex)
	{	case PB_TIMEON:		return GetString(IDS_AP_TIMEON);
		case PB_TIMEGROW:	return GetString(IDS_AP_GROWTIME);
		case PB_TIMEOFF:	return GetString(IDS_AP_TIMEOFF);
		case PB_REPEAT:		return GetString(IDS_AP_REPEAT);
		case PB_MAXD:		return GetString(IDS_AP_MAXD);
		case PB_RWIDTH:		return GetString(IDS_AP_RINGW);
		case PB_RES:		return GetString(IDS_AP_RES);

		case PB_OEBREAKON:	return GetString(IDS_AP_OUTERB);
		case PB_OENCYC1:	return GetString(IDS_AP_ONCYCLES1);
		case PB_OEDWID1:	return GetString(IDS_AP_OWID1);
		case PB_OEPER1:		return GetString(IDS_AP_OPER1);
		case PB_OENCYC2:	return GetString(IDS_AP_ONCYCLES2);
		case PB_OEDWID2:	return GetString(IDS_AP_OWID2);
		case PB_OEPER2:		return GetString(IDS_AP_OPER2);

		case PB_IEBREAKON:	return GetString(IDS_AP_IUTERB);
		case PB_IENCYC1:	return GetString(IDS_AP_INCYCLES1);
		case PB_IEDWID1:	return GetString(IDS_AP_IWID1);
		case PB_IEPER1:		return GetString(IDS_AP_IPER1);
		case PB_IENCYC2:	return GetString(IDS_AP_INCYCLES2);
		case PB_IEDWID2:	return GetString(IDS_AP_IWID2);
		case PB_IEPER2:		return GetString(IDS_AP_IPER2);
		case PB_HEIGHT:		return GetString(IDS_AP_HEIGHT);
		case PB_HEIGHTSEGS:	return GetString(IDS_AP_HEIGHTSEGS);
		case PB_RADSEGS:	return GetString(IDS_AP_RADSEGS);
		break;

		default: 				return TSTR(_T(""));
	}
}

RefTargetHandle RingWaveObject::Clone(RemapDir& remap) 
{	RingWaveObject* newob = new RingWaveObject();
	newob->ReplaceReference(0,pblock->Clone(remap));	
	newob->ivalid.SetEmpty();	
	return(newob);
}

