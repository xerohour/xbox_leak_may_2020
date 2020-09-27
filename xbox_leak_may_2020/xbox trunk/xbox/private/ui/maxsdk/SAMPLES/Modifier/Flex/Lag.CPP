/**********************************************************************
 *<
	FILE: lag.cpp

	DESCRIPTION:  Lag modifier

	CREATED BY: Peter Watje
				Now fully supports param2 except the paint mode
	HISTORY: 8/13/98

 *>	Copyright (c) 1998, All Rights Reserved.
 **********************************************************************/

#include "mods.h"
#include "iparamm2.h"
#include "tvnode.h"
#include "modstack.h"
#include "notify.h"

#include "Maxscrpt.h"
#include "Strings.h"
#include "arrays.h"
#include "3DMath.h"
#include "Numbers.h"
#include "definsfn.h"


#define PBLOCK_REF	0
#define POINT1_REF	1

#define A_RENDER			A_PLUGIN1


#define LAZYID 0xDE17A34f, 0x8A41E2B0

#define CID_CREATEPAINT CID_USER + 204


#define SPRINGMODEL

//defines to mark chunks so I can save the weights and the nail pieces

#define POS_WEIGHT_COUNT_CHUNK	100
#define POS_WEIGHT_CHUNK		150
#define NAIL_CHUNK				200
#define GRAVITY_COUNT_CHUNK		250

#define VERTLIMIT	2000
#define PATCHLIMIT	500


#define LAGCONTAINERMASTER_TVNODE_CLASS_ID Class_ID(0xe27e0f2b, 0x74fad494)
#define LAGCONTAINER_TVNODE_CLASS_ID Class_ID(0xe27e0f2b, 0x74fad483)
#define LAGNODE_TVNODE_CLASS_ID Class_ID(0xe27e0f2b, 0x74fad484)


class SpringClass
{
public:
	Point3 vel, pos, init_pos;
	float InheritVel;
	BOOL modified;
	Point3 LocalPt;
};

class CacheClass
{
public:
Point3 vel, pos;

};


class LagModData : public LocalModData {
	public:
		int id;
		INode *SelfNode;
		Tab<SpringClass> SpringList;
		Tab<CacheClass> WholeFrameCache;

		Tab<BYTE> esel;  //selection for edges vertices
		Tab<BYTE> wsel;		//vertex weight selection
		Tab<BYTE> psel;		
		Matrix3 InverseTM;


		BOOL isMesh;
		BOOL isPatch;

		Point3 hitPoint;
		BOOL isHit;
		TimeValue lastFrame;
		BOOL nukeRenderCache;
		Tab<Point3> pointCache;

		LagModData()
			{
			SelfNode = NULL;
			id = -1;
			isMesh = FALSE;
			isPatch = FALSE;
			lastFrame = 999999999;
			nukeRenderCache = TRUE;

			}
		LagModData(int i, INode *n)
			{
			id = i;
			SelfNode = n;
			isMesh = FALSE;
			isPatch = FALSE;
			lastFrame = 999999999;
			nukeRenderCache = TRUE;

			}
		~LagModData()
			{
			SelfNode = NULL;
			}	
		LocalModData*	Clone()
			{
			LagModData* d = new LagModData();
			d->id = id;
			d->SelfNode = NULL;
			isMesh = FALSE;
			isPatch = FALSE;



			return d;

			}	


	};

class LagNodeNotify;


class LagMod;

class LagValidatorClass : public PBValidator
{
public:
LagMod *mod;
private:
BOOL Validate(PB2Value &v) 
	{
	INode *node = (INode*) v.r;

	if (node->TestForLoop(FOREVER,(ReferenceMaker *) mod)!=REF_SUCCEED) return FALSE;

	const ObjectState& os = node->EvalWorldState(0);
	Object* ob = os.obj;
	if (ob!=NULL) 
			{	
			int id=(os.obj?os.obj->SuperClassID():SClass_ID(0));
			if (id==WSM_OBJECT_CLASS_ID)
				{
				WSMObject *obref=(WSMObject*)node->GetObjectRef();
				BOOL ShouldBeHere=obref->SupportsDynamics();

				return ShouldBeHere;
				}
			else return FALSE;
				  
			}
	return FALSE;

	};
};


class CreatePaintMode;

class LagMod : public Modifier {	
	public:

		BOOL inPaint;
		BOOL painting;

		IParamBlock2 *pblock2;

		ITrackViewNode *container;

		Control *p1;
		static HWND hParams;

		LagNodeNotify *notify;


// data cache
		LagValidatorClass validator;


		BYTE sel[1];    //seclection for center

		Tab<SpringClass> oldSpringList;
		Tab<BYTE> oldesel;  //selection for edges vertices


		float weightDelta;
		ApplyDelta() ;
	
		Point3 lagCenter;


		static IObjParam *ip;
		static IParamMap *pmapParam;
		static LagMod *editMod;
		static MoveModBoxCMode *moveMode;
		ISpinnerControl *iVWeight;
		
//Paint controlls
        float PaintStrength;
        float Radius;
        float Feather;
        int ProjectThrough;
		int absoluteWeight;
        static CreatePaintMode *PaintMode;
		void StartPaintMode();
        ICustButton* iPaintButton;
		BOOL updating;
		void ResetSprings();

		LagMod();
		~LagMod();
		Point3 GetForce(TimeValue t, Point3 p, Point3 v, Matrix3 tm);


		Tab<ForceField*> ff;
		Class_ID cid;
		void AddForceField();

		TimeValue ReferenceFrame;
		float falloff, strength, dampening;
		
		
										
// JBW: direct ParamBlock access is added
		int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { return pblock2; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; } // return id'd ParamBlock


		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) {s = GetString(IDS_RB_LAGMOD);}  
		virtual Class_ID ClassID() { return Class_ID(LAZYID);}		
		void BeginEditParams(IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip,ULONG flags,Animatable *next);		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() {return GetString(IDS_RB_LAGMOD);}

		// From modifier
		ChannelMask ChannelsUsed()  {return PART_GEOM|PART_SELECT|PART_SUBSEL_TYPE;}
//		ChannelMask ChannelsChanged() {return PART_GEOM|PART_TOPO;}
		ChannelMask ChannelsChanged() {return PART_GEOM;}
		Class_ID InputType() {return defObjectClassID;}
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Interval LocalValidity(TimeValue t);


		void DrawCrossSectionNoMarkers(Point3 a, Point3 align, float length, GraphicsWindow *gw);

		// From BaseObject
		int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc);
		int Display(TimeValue t, INode* inode, ViewExp *vpt, int flagst, ModContext *mc);
		void GetWorldBoundBox(TimeValue t,INode* inode, ViewExp *vpt, Box3& box, ModContext *mc);		
		void Move( TimeValue t, Matrix3& partm, Matrix3& tmAxis, Point3& val, BOOL localOrigin=FALSE );
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 
		void GetSubObjectCenters(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		void GetSubObjectTMs(SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc);
		void ActivateSubobjSel(int level, XFormModes& modes);
		void SelectSubComponent(HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert=FALSE);
		void ClearSelection(int selLevel);
		void SelectAll(int selLevel);
		void InvertSelection(int selLevel);

		int NumRefs() {return 2 ;
//			+ GravityNode.Count();
						}
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		int NumSubs() {return 2;}
		Animatable* SubAnim(int i); 
		TSTR SubAnimName(int i);

		RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message);


		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);

		IOResult SaveLocalData(ISave *isave, LocalModData *pld);
		IOResult LoadLocalData(ILoad *iload, LocalModData **pld);

		Matrix3 CompMatrix(TimeValue t,INode *inode,ModContext *mc);


		int RenderBegin(TimeValue t, ULONG flags);		
		int RenderEnd(TimeValue t);
		BOOL nukeCache;
		HoldWeights(LagModData *lmd);

		BOOL updateWeights;
		void ComputeCenterFalloff(LagModData *lmd, TimeValue t, ObjectState *os);

		EnableModsAbove(LagModData *lmd, BOOL enable);
		Tab<BOOL> enableStates;
		Tab<BOOL> enableViewStates;
		BOOL aboutToRender;

		Tab<LagModData*> lmdData;

		int CheckCache(TimeValue t);

	};

// block IDs
enum { lag_params };
// lag_param param IDs
enum { lag_flex, lag_strength, lag_sway, lag_referenceframe, lag_paint_strength,
		lag_paint_radius,lag_paint_feather,lag_paint_backface, lag_force_node, lag_absolute };


class LagNodeNotify  : public TVNodeNotify 
{
public:
LagMod *s;
LagNodeNotify(LagMod *smod)
	{
	s = smod;
	}

RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message)
	{
	if (message == REFMSG_CHANGE) 
		{
		if (partID & PART_TM)
			{
			if (s->ip)
				s->pblock2->SetValue(lag_referenceframe,0,s->ReferenceFrame/GetTicksPerFrame());
			}
		}
	return REF_SUCCEED ;
	}
};



class LagDeformer : public Deformer {
	public:		

		LagModData *lmd;
		LagMod *mod;
		LagDeformer(LagMod *m, LagModData *l) {mod = m; lmd = l;}
		Point3 Map(int i, Point3 p);
	};


//--- ClassDescriptor and class vars ---------------------------------

IParamMap       *LagMod::pmapParam = NULL;
IObjParam       *LagMod::ip        = NULL;
LagMod          *LagMod::editMod   = NULL;
MoveModBoxCMode *LagMod::moveMode  = NULL;
HWND			LagMod::hParams  = NULL;
CreatePaintMode* LagMod::PaintMode   = NULL;
		

//class LagClassDesc:public ClassDesc {
class LagClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new LagMod; }
	const TCHAR *	ClassName() { return GetString(IDS_RB_LAGMOD); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return Class_ID(LAZYID); }
	const TCHAR* 	Category() { return GetString(IDS_RB_DEFDEFORMATIONS);}

// JBW: new descriptor data accessors added.  Note that the 
//      internal name is hardwired since it must not be localized.
	const TCHAR*	InternalName() { return _T("Flex"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle

	};

static LagClassDesc lagDesc;
extern ClassDesc* GetLagModDesc() {return &lagDesc;}


//--- Parameter map/block descriptors -------------------------------
//new parm blocks
//
//
// Parameters

// JBW: Here follows the new parameter block descriptors.  There are now 3, 
//      two new STATIC ones to hold the old class var parameters, one for the main
//		per-instance parameters.  Apart from all the extra 
//      metadata you see in the definitions, one important new idea is the
//      folding of ParamMap description info into the parameter descriptor and
//      providing a semi-automatic rollout desipaly mechanism.
//      

// Parameter Block definitions

// JBW: First come the position and version independent IDs for each
//      of the blocks and the parameters in each block.  These IDs are used
//	    in subsequent Get/SetValue() parameter access, etc. and for version-independent
//      load and save



// JBW: this descriptor defines the main per-instance parameter block.  It is flagged as AUTO_CONSTRUCT which
//      means that the CreateInstance() will automatically create one of these blocks and set it to the reference
//      number given (0 in this case, as seen at the end of the line).

// per instance geosphere block
static ParamBlockDesc2 lag_param_blk ( lag_params, _T("FlexParameters"),  0, &lagDesc, P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 
	//rollout
	IDD_LAGPARAM, IDS_RB_PARAMETERS, 0, 0, NULL,
	// params
	lag_flex,  _T("flex"), 			TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_LAG_FALLOFF, 
		p_default, 		1.0,	
		p_range, 		0.0f, 1000.0f, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_LAG_FALLOFF,IDC_LAG_FALLOFFSPIN, .1f, 
		end, 
	lag_strength, 	_T("strength"), 	TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_LAG_STRENGTH, 
		p_default, 		3.0f, 
		p_range, 		0.0f, 100.0f, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_LAG_SETTLE, IDC_LAG_SETTLE_SPIN, 0.01f,  
		end, 

	lag_sway, 	_T("sway"), 	TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_LAG_SWAY, 
		p_default, 		7.0f, 
		p_range, 		0.0f, 100.0f, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_LAG_WAVES, IDC_LAG_WAVES_SPIN, 0.01f,  
		end, 

	lag_referenceframe, 	_T("referenceFrame"), 	TYPE_INT, 	0, 	IDS_LAG_REFERENCEFRAME, 
		p_default, 		0, 
		p_range, 		-99999, 99999, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_INT, IDC_REFERENCEFRAME, IDC_REFERENCEFRAME_SPIN, SPIN_AUTOSCALE,  
		end, 

	lag_paint_strength,  _T("paintStrength"),		TYPE_FLOAT, 	0, 	IDS_LAG_STRENGTH, 
		p_default, 		0.1,	
		p_range, 		-1.0f, 1.0f, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_PAINT_STRENGTH,IDC_PAINTSTRENGTHSPIN, .1f, 
		end, 

	lag_paint_radius,  _T("paintRadius"),		TYPE_FLOAT, 	0, 	IDS_LAG_STRENGTH, 
		p_default, 		36.0f,	
		p_range, 		0.001f, 99999.0f, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_RADIUS,IDC_RADIUSSPIN, 1.0f, 
		end, 

	lag_paint_feather,  _T("paintFeather"),	TYPE_FLOAT, 	0, 	IDS_LAG_STRENGTH, 
		p_default, 		0.70f,	
		p_range, 		0.001f, 1.0f, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_FLOAT, IDC_FEATHER,IDC_FEATHERSPIN, SPIN_AUTOSCALE, 
		end, 

	lag_paint_backface, _T("paintBackface"),	TYPE_BOOL, 		0,	IDS_LAG_STRENGTH,
		p_default, 		TRUE, 
		p_ui, 			TYPE_SINGLECHEKBOX, 	IDC_PROJECT_THROUGH_CHECK, 
		end, 
	lag_force_node,    _T("forceNode"),  TYPE_INODE_TAB,		0,	P_AUTO_UI,	IDS_LAG_FORCE_NODES,
		p_ui,			TYPE_NODELISTBOX, IDC_LIST1,IDC_GRAVITY_PICKNODE,0,IDC_REMOVE_GRAV,
		end,
	lag_absolute, _T("absolute"),	TYPE_BOOL, 		0,	IDS_LAG_ABSOLUTE,
		p_default, 		FALSE, 
		p_ui, 			TYPE_SINGLECHEKBOX, 	IDC_ABSOLUTE, 
		end, 
	end
	);



class LagWeightRestore : public RestoreObj {
	public:
		LagMod *mod;
		LagModData *bmd;
		Tab<float> uInflu;
		Tab<float> rInflu;

		LagWeightRestore(LagMod *c, LagModData *md) 
			{
			mod = c;
			bmd = md;
			uInflu.SetCount(md->SpringList.Count());
			for (int i = 0; i < uInflu.Count(); i++)
				{
				uInflu[i] = md->SpringList[i].InheritVel;
				}

			}   		
		void Restore(int isUndo) 
			{
			if (isUndo) 
				{
				rInflu.SetCount(bmd->SpringList.Count());
				for (int i = 0; i < uInflu.Count(); i++)
					{
					rInflu[i] = bmd->SpringList[i].InheritVel;
					}

				}

			for (int i = 0; i < uInflu.Count(); i++)
				{
				bmd->SpringList[i].InheritVel = uInflu[i];
				}

			mod->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
			}
		void Redo()
			{
			for (int i = 0; i < uInflu.Count(); i++)
				{
				bmd->SpringList[i].InheritVel = rInflu[i];
				}

			mod->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
			}		
		void EndHold() 
			{ 
			mod->ClearAFlag(A_HELD);
			}
		TSTR Description() { return TSTR(_T(GetString(IDS_PW_CHANGE_WEIGHT))); }
	};

LagMod::HoldWeights(LagModData *lmd)
{
//if (theHold.Holding()) 
	theHold.Put (new LagWeightRestore (this, lmd));

}

/*-------------------------------------------------------------------*/
/*																	*/
/*				Paint Command Mode									*/
/*																	*/
/*-------------------------------------------------------------------*/

class CreatePaintMouseProc : public MouseCallBack {
        private:
                LagMod *mod;
                IObjParam *iObjParams;
				IPoint2 lastPoint;
                IPoint2 om;
                Tab<float> FalloffList;
				Tab<IPoint2> mouseHitList;
        
        protected:
                BOOL HitTest(LagModData *lmd, ViewExp *vpt, IPoint2 *p, int type, int flags, Object *obj );
                BOOL AnyHits( ViewExp *vpt ) { return vpt->NumSubObjHits(); }           
				BOOL BuildFalloffList( ViewExp *vpt, Tab<IPoint2> plist, 
											 LagModData *lmd, Object *obj);


        public:
                CreatePaintMouseProc(LagMod* bmod, IObjParam *i) { mod=bmod; iObjParams=i; }
                int proc( 
                        HWND hwnd, 
                        int msg, 
                        int point, 
                        int flags, 
                        IPoint2 m );
        };



class CreatePaintMode : public CommandMode {
        private:
                ChangeFGObject fgProc;
                CreatePaintMouseProc eproc;
                LagMod* mod;

        public:
                CreatePaintMode(LagMod* bmod, IObjParam *i) :
                        fgProc(bmod), eproc(bmod,i) {mod=bmod;}

                int Class() { return MODIFY_COMMAND; }
                int ID() { return CID_CREATEPAINT; }
                MouseCallBack *MouseProc(int *numPoints) { *numPoints=2; return &eproc; }
                ChangeForegroundCallback *ChangeFGProc() { return &fgProc; }
                BOOL ChangeFG( CommandMode *oldMode ) { return oldMode->ChangeFGProc() != &fgProc; }
                void EnterMode();
                void ExitMode();
        };





float LineToPoint(Point3 p1, Ray r, float &u)

{
Point3 VectorA,VectorB,VectorC;
float Angle;
float dist = 0.0f;

VectorA = r.dir;
VectorB = p1-r.p;
Angle = (float)acos(DotProd(Normalize(VectorA),Normalize(VectorB)));

float hyp;
hyp = Length(VectorB);
dist = (float) sin(Angle) * hyp;

u = (float) cos(Angle) * hyp;

return dist;

}



BOOL CreatePaintMouseProc::HitTest(LagModData *lmd, 
                ViewExp *vpt, IPoint2 *p, int type, int flags, Object *obj )

        {

		if (lmd == NULL ) return FALSE;

        Ray                     ray;
        BOOL res = FALSE;

        vpt->MapScreenToWorldRay((float)p->x, (float)p->y, ray);

//now map it to our local space
		float feather, ifeather;

        feather = 1.0f -  mod->Feather;
	    ifeather = mod->Feather;

		if (1)
			{
			ray.p   = lmd->InverseTM * ray.p;
			ray.dir = VectorTransform(lmd->InverseTM, ray.dir);	

			float at;
			Point3 norm;

		    for (int i = 0; i<lmd->psel.Count(); i++)
			        lmd->psel[i] = 0;


			if (obj->IntersectRay(mod->ip->GetTime(),ray, at, norm)) 
				{
				lmd->hitPoint = ray.p + ray.dir * at;
				float rad = mod->Radius * mod->Radius;
				Box3 boundingBox;
				boundingBox.Init();
				boundingBox += lmd->hitPoint;					
				boundingBox.EnlargeBy(rad);

		        if ((lmd->isMesh)&& (obj->NumPoints() < VERTLIMIT)) FalloffList.ZeroCount();
				else if ((lmd->isPatch)&& (obj->NumPoints() < PATCHLIMIT)) FalloffList.ZeroCount();

				for (i = 0; i<lmd->SpringList.Count(); i++)
		            {
					if (boundingBox.Contains(lmd->SpringList[i].LocalPt))
						{
					    float dist = LengthSquared(lmd->SpringList[i].LocalPt-lmd->hitPoint);
						if (dist <= rad)
			                {
				            lmd->psel[i] = 1;
							if( ( (lmd->isMesh) && (obj->NumPoints() < VERTLIMIT) ) ||
							    ( (lmd->isPatch) && (obj->NumPoints() < PATCHLIMIT) )
							  )
								{
                                float f;
								dist = (float) sqrt(dist);
                                if (dist < mod->Radius*feather)
									f = 1.0f;
                                else 
                                    {
                                    f = 1.0f - (dist-(mod->Radius*feather))/(mod->Radius*ifeather);
                                    }
								f = f * mod->PaintStrength;
                                FalloffList.Append(1,&f,1);

								}



							}
						}
					}


				return TRUE;
				}

		
			}

        return res;


        }


float LagLineToPoint(Point3 p1, Point3 l1, Point3 l2, float &u)
{
Point3 VectorA,VectorB,VectorC;
double Angle;
double dist = 0.0f;
VectorA = l2-l1;
VectorB = p1-l1;
Angle =  acos(DotProd(Normalize(VectorA),Normalize(VectorB)));
if (Angle > (3.14/2.0))
	{
	dist = Length(p1-l1);
	u = 0.0f;
	}
else
	{
	VectorA = l1-l2;
	VectorB = p1-l2;
	Angle = acos(DotProd(Normalize(VectorA),Normalize(VectorB)));
	if (Angle > (3.14/2.0))
		{
		dist = Length(p1-l2);
		u = 1.0f;
		}
		else
		{
		double hyp;
		hyp = Length(VectorB);
		dist =  sin(Angle) * hyp;
		double du =  (cos(Angle) * hyp);
		double a = Length(VectorA);
		if ( a== 0.0f)
			return 0.0f;
		else u = (float)((a-du) / a);

		}

	}

return (float) dist;

}


BOOL CreatePaintMouseProc::BuildFalloffList( ViewExp *vpt, Tab<IPoint2> plist, 
											 LagModData *lmd, Object *obj)

{
Box3 boundingBox;

if (lmd == NULL ) return FALSE;

Tab<Point3> hitList;

boundingBox.Init();

for (int pct = 0; pct < plist.Count(); pct++)
		{
        Ray ray;

		IPoint2 p = plist[pct];
        vpt->MapScreenToWorldRay((float)p.x, (float)p.y, ray);
		ray.p   = lmd->InverseTM * ray.p;
		ray.dir = VectorTransform(lmd->InverseTM, ray.dir);	

//now map it to our local space
		if (1)
			{

			float at;
			Point3 norm;

			if (obj->IntersectRay(mod->ip->GetTime(),ray, at, norm)) 
				{
				Point3 hitPoint;
				hitPoint = ray.p + ray.dir * at;
				hitList.Append(1,&hitPoint,1);
				boundingBox += hitPoint;					
				}
			}

		}
boundingBox.EnlargeBy(mod->Radius);
float feather, ifeather;
feather = 1.0f -  mod->Feather;
ifeather = mod->Feather;


if (hitList.Count() ==1 )
	{	
	for (int i = 0; i< lmd->SpringList.Count(); i++)
		{
		if (boundingBox.Contains(lmd->SpringList[i].LocalPt))
			{
			float dist = Length(lmd->SpringList[i].LocalPt-hitList[0]);
			if (dist < mod->Radius)
				{
				float f;
	            if (dist < mod->Radius*feather)
					f = 1.0f;
                else 
                   {
		           f = 1.0f - (dist-(mod->Radius*feather))/(mod->Radius*ifeather);
                   }
				f = f * mod->PaintStrength;
				if (f > FalloffList[i]) 
					FalloffList[i] = f;
				}
			}
		}

	}
else
	{
	for (pct = 1; pct < hitList.Count(); pct++)
		{
		for (int i = 0; i< lmd->SpringList.Count(); i++)
			{
			if (boundingBox.Contains(lmd->SpringList[i].LocalPt))
				{
				Point3 l1,l2;
				l1 = hitList[pct-1];
				l2 = hitList[pct];
				float u;
				float dist = LagLineToPoint(lmd->SpringList[i].LocalPt,l1, l2,u);

				if (dist < mod->Radius)
					{
					float f;
			        if (dist < (mod->Radius*feather))
			           f = 1.0f;
				    else 
					   {
						f = 1.0f - (dist-(mod->Radius*feather))/(mod->Radius*ifeather);
						}	
					f = f * mod->PaintStrength;
					if (f > FalloffList[i]) 
						FalloffList[i] = f;

					}
				}
			}
		}

	}
return 1;
}

static void LagXORDottedLine( HWND hwnd, IPoint2 p0, IPoint2 p1 )
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

int CreatePaintMouseProc::proc(
                        HWND hwnd, 
                        int msg, 
                        int point, 
                        int flags, 
                        IPoint2 m )
        {
        ViewExp *vpt = iObjParams->GetViewport(hwnd);   
        int res = TRUE;
        float st = 1.0f;
        int flip = 0;
		int i;

		ModContextList mcList;		
		INodeTab nodes;

		if (!mod->ip) return 0;

		mod->ip->GetModContexts(mcList,nodes);
		int objects = mcList.Count();

		for ( int k = 0; k < objects; k++ ) 
			{

			LagModData *lmd = (LagModData*)mcList[k]->localData;

			ObjectState os = nodes[k]->EvalWorldState(mod->ip->GetTime());
			Object *obj = os.obj;

			if (lmd)
				{

		        switch ( msg ) {
		            case MOUSE_PROPCLICK:
			                iObjParams->SetStdCommandMode(CID_OBJMOVE);
							if (mod->painting)
								theHold.Cancel();
							mod->painting = FALSE;
				            break;

					case MOUSE_POINT:
						if (point == 0)
							{
							mouseHitList.ZeroCount();
							mod->painting = TRUE;

							theHold.Begin();
							mod->HoldWeights(lmd);

							}
						else if (mouseHitList.Count() != 0)
							{
//compute hit list
							mod->painting = FALSE;

		                    st = 1.0f;
			                flip = 0;
				            if (flags & MOUSE_ALT) 
					            {
						        st = -1.0f;
							    flip = 1;
								}
		                    mod->pblock2->GetValue(lag_paint_strength,0,mod->PaintStrength,FOREVER);
			                mod->pblock2->GetValue(lag_paint_radius,0,mod->Radius,FOREVER);
				            mod->pblock2->GetValue(lag_paint_feather,0,mod->Feather,FOREVER);
							FalloffList.SetCount(lmd->SpringList.Count());
						
							for (int i = 0; i < lmd->SpringList.Count(); i++)
								FalloffList[i] = -10.0f;
							BuildFalloffList( vpt, mouseHitList,lmd,obj);
						    for (i=0;i<lmd->SpringList.Count();i++)
								{
								if (FalloffList[i] > 0.0f )
									{
									float currentv = FalloffList[i];
									if (st == 1.0f)
										lmd->SpringList[i].InheritVel -= currentv;
									else lmd->SpringList[i].InheritVel += currentv;
	
									lmd->SpringList[i].modified = TRUE;
									if (lmd->SpringList[i].InheritVel <0.0f) lmd->SpringList[i].InheritVel = 0.0f;
									if (lmd->SpringList[i].InheritVel >1.0f) lmd->SpringList[i].InheritVel = 1.0f;
									}
								}


							for (i = 1; i < mouseHitList.Count(); i++)
								LagXORDottedLine(hwnd, mouseHitList[i-1], mouseHitList[i]);	// Draw it!


			                mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
				            mod->ip->RedrawViews(mod->ip->GetTime());
							}
						
						theHold.Accept(GetString(IDS_PW_CHANGE_WEIGHT));

						break;



					case MOUSE_MOVE:
						if ( ((lmd->isMesh) && (obj->NumPoints() < VERTLIMIT)) ||
							  ((lmd->isPatch) && (obj->NumPoints() < PATCHLIMIT)) 
							  )
							{
	                        st = 1.0f;
		                    flip = 0;
			                if (flags & MOUSE_ALT) 
				                    {
					                st = -1.0f;
						            flip = 1;
							        }
	                        mod->pblock2->GetValue(lag_paint_strength,0,mod->PaintStrength,FOREVER);
		                    mod->pblock2->GetValue(lag_paint_radius,0,mod->Radius,FOREVER);
			                mod->pblock2->GetValue(lag_paint_feather,0,mod->Feather,FOREVER);
//change this to a radius type hit
				            if(HitTest(lmd, vpt,&m,HITTYPE_CIRCLE,0, obj) ) {

//transfrom mouse point to world
								int ct = 0;
							    for (i=0;i<lmd->SpringList.Count();i++)
									{
									if (lmd->psel[i] == 1 )
										{
										float currentv = FalloffList[ct++];
										if (st == 1.0f)
											lmd->SpringList[i].InheritVel -= currentv;
										else lmd->SpringList[i].InheritVel += currentv;

										lmd->SpringList[i].modified = TRUE;
										if (lmd->SpringList[i].InheritVel <0.0f) lmd->SpringList[i].InheritVel = 0.0f;
										if (lmd->SpringList[i].InheritVel >1.0f) lmd->SpringList[i].InheritVel = 1.0f;
										}
									}
								mod->painting = FALSE;

	                            mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
		                        mod->ip->RedrawViews(mod->ip->GetTime());

			                    }
							}
						else
							{
							if (mouseHitList.Count() > 1)
								{
								if (mouseHitList[mouseHitList.Count()-1] != m)
									{
									if (mouseHitList.Count() > 1)
										{
										LagXORDottedLine(hwnd, lastPoint, m);	// Draw it!
										}

		
									lastPoint = m;

							        if ( HitTest(lmd,vpt,&m,HITTYPE_CIRCLE,HIT_ABORTONHIT,obj) ) 
										{

				                        mod->NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
										lmd->isHit = TRUE;
							            mod->ip->RedrawViews(mod->ip->GetTime());
	
										}
									else
										{
										if (lmd->isHit)
											{
											mod->NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
											lmd->isHit = FALSE;
								            mod->ip->RedrawViews(mod->ip->GetTime());
											}
										else lmd->isHit = FALSE;

										}	

									mouseHitList.Append(1,&m,1);
									}

								}
							else {
								mouseHitList.Append(1,&m,1);
								lastPoint = m;
								}

							}
					       break;
                
	                case MOUSE_FREEMOVE:
                        if ( HitTest(lmd,vpt,&m,HITTYPE_CIRCLE,HIT_ABORTONHIT,obj) ) {
                                SetCursor(LoadCursor(NULL,IDC_CROSS ));
							lmd->isHit = TRUE;
							mod->painting = FALSE;

                             mod->NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
                             mod->ip->RedrawViews(mod->ip->GetTime());

                             }
                        else {
                              SetCursor(LoadCursor(NULL,IDC_ARROW));

							  if (lmd->isHit)
								{
								  lmd->isHit = FALSE;
		                         mod->NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	                             mod->ip->RedrawViews(mod->ip->GetTime());
								}
	
							  else lmd->isHit = FALSE;

								mod->painting = FALSE;
                              }
                        break;
					}
                        
                }
			}

        if ( vpt ) iObjParams->ReleaseViewport(vpt);

        return res;
        }



/*-------------------------------------------------------------------*/

void CreatePaintMode::EnterMode()
        {
        mod->iPaintButton->SetCheck(TRUE);
		mod->painting = FALSE;
		mod->inPaint = TRUE;

		BOOL en = mod->IsEnabled();
		BOOL env = mod->IsEnabledInViews();

		if ((!en) || (!env))
			{	
			ModContextList mcList;		
			INodeTab nodes;
			
			if (mod->ip)
				{


				mod->ip->GetModContexts(mcList,nodes);
				int objects = mcList.Count();
/*				BOOL en = TRUE;
				if (!mod->IsEnabled())
					en = FALSE;
*/

				for ( int k = 0; k < objects; k++ ) 
					{
					if (en)
						mod->EnableModInViews();	
					else mod->EnableMod();	
					ObjectState os = nodes[k]->EvalWorldState(mod->ip->GetTime());
					if (en)
						mod->DisableModInViews();	
					else mod->DisableMod();	
					}	
				}
			}

        }

void CreatePaintMode::ExitMode()
        {
        mod->iPaintButton->SetCheck(FALSE);
		mod->painting = FALSE;
		mod->inPaint = FALSE;

//nuke mesh
        }

//dialog stuff to get the Set Ref button
class MapDlgProc : public ParamMap2UserDlgProc {
//public ParamMapUserDlgProc {
	public:
		LagMod *mod;		
		MapDlgProc(LagMod *m) {mod = m;}		
		BOOL DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);		
		void DeleteThis() {delete this;}
	};


LagMod::ApplyDelta() 

{
ModContextList mcList;		
INodeTab nodes;

if (!ip) return 0;

ip->GetModContexts(mcList,nodes);
int objects = mcList.Count();

theHold.Begin();
for ( int k = 0; k < objects; k++ ) 
	{

	LagModData *lmd = (LagModData*)mcList[k]->localData;
	if (lmd)
		{
		HoldWeights(lmd);

		for (int i = 0; i < lmd->wsel.Count(); i++)
			{
			if (lmd->wsel[i] == 1)
				{
				if (absoluteWeight) 
					lmd->SpringList[i].InheritVel = 1.0f- weightDelta;
				else lmd->SpringList[i].InheritVel -= weightDelta ;
				if (lmd->SpringList[i].InheritVel < 0.0f) lmd->SpringList[i].InheritVel = 0.0f;
				if (lmd->SpringList[i].InheritVel > 1.0f) lmd->SpringList[i].InheritVel = 1.0f;
				lmd->SpringList[i].modified = TRUE;
				}

			}
		}
	}
theHold.Accept(GetString(IDS_PW_CHANGE_WEIGHT));

return 1;
}

BOOL MapDlgProc::DlgProc(
		TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
	switch (msg) {
		case WM_INITDIALOG: {
			mod->hParams = hWnd;

			mod->iPaintButton = GetICustButton(GetDlgItem(hWnd,IDC_PAINT));
			mod->iPaintButton->SetType(CBT_CHECK);
			mod->iPaintButton->SetHighlightColor(GREEN_WASH);
			mod->iPaintButton->Enable(FALSE);

			mod->iVWeight = GetISpinner(GetDlgItem(hWnd,IDC_VWEIGHT_SPIN));
			mod->iVWeight->LinkToEdit(GetDlgItem(hWnd,IDC_VWEIGHT),EDITTYPE_FLOAT);
			mod->iVWeight->SetLimits(-100.0f, 100.0f, FALSE);

			
			break;
			}

		case CC_SPINNER_BUTTONDOWN:
			if (!mod->absoluteWeight) 
				{
				mod->iVWeight->SetValue(0.0f,TRUE);
				mod->weightDelta = 0.0f;
				}
			else mod->weightDelta = mod->iVWeight->GetFVal() * 0.01f;

			break;
		case CC_SPINNER_CHANGE:
			mod->weightDelta = mod->iVWeight->GetFVal() * 0.01f;
            mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
			break;

		case WM_CUSTEDIT_ENTER:
		case CC_SPINNER_BUTTONUP:
			if (HIWORD(wParam) || msg==WM_CUSTEDIT_ENTER) 
				{
				//apply

				mod->ApplyDelta();
				}
			else
				{
				mod->weightDelta = 0.0f;
				}
			if (!mod->absoluteWeight) 
				mod->iVWeight->SetValue(0.0f,TRUE);
            mod->ip->RedrawViews(mod->ip->GetTime());
            mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
			break;


		case WM_COMMAND:
			switch (LOWORD(wParam)) 
				{

				case IDC_ABSOLUTE:
					{
					Interval iv;
					mod->pblock2->GetValue(lag_absolute,0,mod->absoluteWeight,iv);

					if (!mod->absoluteWeight) 
						{
						mod->iVWeight->SetValue(0.0f,TRUE);
						mod->weightDelta = 0.0f;
						}
					break;

					}
				case IDC_SETREFERENCE_BUTTON:
					{
//set time to ref time
					Interface *ip = GetCOREInterface();
					TimeValue CurrentTime;
					Interval CurrentSeg,ReferenceSeg;
				
					CurrentTime = ip->GetTime();
					CurrentSeg = ip->GetAnimRange();
					ReferenceSeg = CurrentSeg;
//set new range to include reference frame
					ReferenceSeg.Set(mod->ReferenceFrame,mod->ReferenceFrame+GetTicksPerFrame());
					ip->SetAnimRange(ReferenceSeg);
					ip->SetTime(mod->ReferenceFrame,FALSE);

//set back old rangle
					ip->SetAnimRange(CurrentSeg);
//set back to original time
					ip->SetTime(CurrentTime,FALSE);
					break;
					}
				case IDC_RESET_BUTTON:
					{
					mod->ResetSprings();
//set time to ref time
					Interface *ip = GetCOREInterface();
					TimeValue CurrentTime;
					Interval CurrentSeg,ReferenceSeg;
				
					CurrentTime = ip->GetTime();
					CurrentSeg = ip->GetAnimRange();
					ReferenceSeg = CurrentSeg;
//set new range to include reference frame
					ReferenceSeg.Set(mod->ReferenceFrame,mod->ReferenceFrame+GetTicksPerFrame());
					ip->SetAnimRange(ReferenceSeg);
					ip->SetTime(mod->ReferenceFrame,FALSE);

//set back old rangle
					ip->SetAnimRange(CurrentSeg);
//set back to original time
					ip->SetTime(CurrentTime,FALSE);
                    mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);

                    ip->RedrawViews(ip->GetTime());

					break;
					}
				case IDC_PAINT:
					{
					mod->StartPaintMode();
					break;
					}

				}
			break;


		}
	return FALSE;
	}


	class MyEnumProc : public DependentEnumProc 
	{
      public :
      virtual int proc(ReferenceMaker *rmaker); 
      INodeTab Nodes;              
	  BOOL nukeME;
	  BOOL xRefFound;
	  int count;
	};

int MyEnumProc::proc(ReferenceMaker *rmaker) 
	{ 
	if (rmaker->ClassID()==Class_ID(0x5c2417bd, 0x44050964))    
		{
		IParamBlock *pb;
		pb = (IParamBlock *) rmaker->GetReference(4);
		Interval ivalid;
		int anioff;
		pb->GetValue(32,0,anioff,ivalid);
		if (anioff != 0)
			nukeME = TRUE;
		}
//7-1-99
	if (rmaker->SuperClassID()==BASENODE_CLASS_ID)    
		{
		Nodes.Append(1, (INode **)&rmaker);  
		count++;
		}
	if (rmaker->ClassID()==Class_ID(XREFOBJ_CLASS_ID,0))    
		{
		xRefFound = TRUE;
		}


	return 0;
	}	



// Declare the callback function
static void RenderFrameStart(void *param, NotifyInfo *info) {
	// Handle the units changing...
	LagMod *mod =  (LagMod*) param;
	TimeValue *t = (TimeValue*) info->callParam;
	mod->CheckCache(*t);

}


//--- Lag mod methods -------------------------------

LagMod::LagMod() 
	{
	GetLagModDesc()->MakeAutoParamBlocks(this);
	p1 = NULL;
	ff.ZeroCount();
	MakeRefByID(FOREVER,POINT1_REF,NewDefaultPoint3Controller()); 
	sel[0] = 1;

	Point3 p(0.0f,0.0f,0.0f);
	p1->SetValue(
			0,p,
			TRUE,CTRL_ABSOLUTE);

	iPaintButton = NULL;
	iVWeight = NULL;
	ReferenceFrame = 0;
	absoluteWeight = 0;
	weightDelta = 0.0f;
	container = NULL;

	notify = new LagNodeNotify(this);

	validator.mod = this;
	painting = FALSE;
	inPaint = FALSE;
	updating = FALSE;
	nukeCache = FALSE;
	updateWeights = FALSE;
	aboutToRender = FALSE;
// Register the callback
	RegisterNotification(RenderFrameStart,this,	NOTIFY_RENDER_PREEVAL);


	}


LagMod::~LagMod() 
{
	DeleteAllRefsFromMe();
	for (int i = 0; i< ff.Count(); i++)
	{
		if (ff[i]) 
			ff[i]->DeleteThis();
	}

// mjm - begin - 5.10.99
	if (container && notify)
		{
		ITrackViewNode *tvr = GetCOREInterface()->GetTrackViewRootNode();
		ITrackViewNode *global = tvr->GetNode(Class_ID(0xb27e9f2a, 0x73fad370));
		ITrackViewNode *tvroot = global->GetNode(LAGCONTAINERMASTER_TVNODE_CLASS_ID);
		if (tvroot) 
			{
			int ct = tvroot->NumItems();
			for (int i = 0; i <ct; i++)
				{
				ITrackViewNode *n = tvroot->GetNode(i);
				if (container == n)
					container->UnRegisterTVNodeNotify(notify);
				}
			}
		else
			{
//			ITrackViewNode *tvroot = tvr->GetNode(LAGCONTAINER_TVNODE_CLASS_ID);
			int ct = tvr->NumItems();
			for (int i = 0; i <ct; i++)
				{
				ITrackViewNode *n = tvr->GetNode(i);
				if (container == n)
					container->UnRegisterTVNodeNotify(notify);
				}


			}	
		}
//		container->UnRegisterTVNodeNotify(notify);
	if (notify)
		delete notify;
// mjm - end


	if (iPaintButton)
		ReleaseICustButton(iPaintButton);
	if (iVWeight)
		ReleaseISpinner(iVWeight);
// When done, unregister the callback
	UnRegisterNotification(RenderFrameStart,this,	NOTIFY_RENDER_PREEVAL);

}



void LagMod::BeginEditParams(
		IObjParam  *ip, ULONG flags,Animatable *prev)
	{
	this->ip = ip;
	editMod  = this;

	// Add our sub object type
	TSTR type1(GetString(IDS_RB_LAGPOINTS));
	TSTR type2(GetString(IDS_RB_LAG_EDGE));
	TSTR type3(GetString(IDS_LAG_WEIGHTS));
	const TCHAR *ptype[] = {type1,type2,type3};
	ip->RegisterSubObjectTypes(ptype, 3);

	// Create sub object editing modes.
	moveMode    = new MoveModBoxCMode(this,ip);
	PaintMode = new CreatePaintMode(this,ip);


	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
	SetAFlag(A_MOD_BEING_EDITED);

	lagDesc.BeginEditParams(ip, this, flags, prev);
	lag_param_blk.SetUserDlgProc(new MapDlgProc(this));
	lag_param_blk.ParamOption(lag_force_node,p_validator,&validator);

	float f;
	Interval iv;
	pblock2->GetValue(lag_paint_strength,ip->GetTime(),f,iv);
	pblock2->SetValue(lag_paint_strength,ip->GetTime(),f);



	}

void LagMod::EndEditParams(
		IObjParam *ip,ULONG flags,Animatable *next)
	{
	this->ip = NULL;
	editMod  = NULL;

	TimeValue t = ip->GetTime();

	// NOTE: This flag must be cleared before sending the REFMSG_END_EDIT
	ClearAFlag(A_MOD_BEING_EDITED);

	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_END_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF);

	ip->DeleteMode(moveMode);	
	if (moveMode) delete moveMode;
	moveMode = NULL;	

	ip->DeleteMode(PaintMode);	
	if (PaintMode) delete PaintMode;
	PaintMode = NULL;
	
	ip->ClearPickMode();

	lagDesc.EndEditParams(ip, this, flags, next);
	}

RefTargetHandle LagMod::Clone(RemapDir& remap)
	{
	LagMod *mod = new LagMod();
	mod->ReplaceReference(PBLOCK_REF,pblock2->Clone(remap));
	mod->ReplaceReference(POINT1_REF,p1->Clone(remap));

	return mod;
	}

class LagModEnumProc : public ModContextEnumProc {
public:
	LagMod *lm;
	LagModEnumProc(LagMod *l)
		{
		lm = l;
		lm->lmdData.ZeroCount();
		}
private:
	BOOL proc (ModContext *mc);
};

BOOL LagModEnumProc::proc (ModContext *mc) {
	if (mc->localData == NULL) return TRUE;

	LagModData *lmd = (LagModData *) mc->localData;

	BOOL found = FALSE;
	for (int i = 0; i < lm->lmdData.Count(); i++)
		{
		if (lmd == lm->lmdData[i])
			{
			found = TRUE;
			i = lm->lmdData.Count();
			}
		}
	if (!found)
		lm->lmdData.Append(1,&lmd,1);

	return TRUE;
}



int LagMod::CheckCache(TimeValue t)
{
//back step now if needed

	int tps = GetTicksPerFrame();
    MyEnumProc dep;              
	dep.nukeME = FALSE;
	dep.xRefFound = FALSE;
	dep.count = 0;
	EnumDependents(&dep);

	int instanced = dep.count;


	LagModEnumProc lmdproc(this);
	EnumModContexts(&lmdproc);

	for (int li = 0; li < lmdData.Count(); li++)
		{
		LagModData *lmd = lmdData[li];

		BOOL rebuildSystem = FALSE;
		
		if ((TestAFlag(A_RENDER)) && (aboutToRender))
			{
			lmd->lastFrame = 999999999;
			}

		if (t>=ReferenceFrame)
			{

			int fract ;
			int frames ;
			TimeValue StartFrame;
			if ( (t<lmd->lastFrame) )
//				||
//				 ( (t==lmd->lastFrame) && (rebuildSystem) )
//				)
				{

				fract = (t) % tps;
				frames =  t / tps;
				StartFrame = ReferenceFrame;
				int nv = lmd->SpringList.Count();
				for (int i=0;i<nv;i++)
					{
					Point3 v(0.0f,0.0f,0.0f);
					lmd->SpringList[i].pos = lmd->SpringList[i].init_pos;
					lmd->SpringList[i].vel = v;
					}
				}
			else
				{
				fract = t %tps;
				frames =  t / tps;
				StartFrame = lmd->lastFrame+tps;
				int nv = lmd->SpringList.Count();
				for (int i=0;i<nv;i++)
					{
					lmd->SpringList[i].pos = lmd->WholeFrameCache[i].pos;
					lmd->SpringList[i].vel = lmd->WholeFrameCache[i].vel;
					}

				}
//	DebugPrint("time t %d start frame %d\n",t,StartFrame);
			for (int i = StartFrame; i < (frames*tps); i+=tps)
				{
//				Matrix3 tm = lmd->SelfNode->GetObjectTM(i);



				Point3 pp,p,force;

				ObjectState tos;
				int nv = lmd->SpringList.Count();
				int tnv = nv;
				if (i != t)
					{
					if (TestAFlag(A_RENDER)) 
						{
						if (instanced==1)
							EnableModsAbove(lmd,FALSE);
						tos = lmd->SelfNode->EvalWorldState(i);
						if (instanced==1)
							EnableModsAbove(lmd,TRUE);
						}
					}
				}
			}
		}
	return 1;

}




int LagMod::RenderBegin(TimeValue t, ULONG flags)
	{
	SetAFlag(A_RENDER);
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	nukeCache = TRUE;


/*
//back step now if needed

	int tps = GetTicksPerFrame();
    MyEnumProc dep;              
	dep.nukeME = FALSE;
	dep.xRefFound = FALSE;
	dep.count = 0;
	EnumDependents(&dep);

	int instanced = dep.count;


	LagModEnumProc lmdproc(this);
	EnumModContexts(&lmdproc);

	for (int li = 0; li < lmdData.Count(); li++)
		{
		LagModData *lmd = lmdData[li];

		BOOL rebuildSystem = FALSE;
		
//		if ((TestAFlag(A_RENDER)) && (aboutToRender))
//			{
			lmd->lastFrame = 999999999;
//			}

		if (t>=ReferenceFrame)
			{

			int fract ;
			int frames ;
			TimeValue StartFrame;
			if ( (t<lmd->lastFrame) )
//				||
//				 ( (t==lmd->lastFrame) && (rebuildSystem) )
//				)
				{

				fract = (t) % tps;
				frames =  t / tps;
				StartFrame = ReferenceFrame;
				int nv = lmd->SpringList.Count();
				for (int i=0;i<nv;i++)
					{
					Point3 v(0.0f,0.0f,0.0f);
					lmd->SpringList[i].pos = lmd->SpringList[i].init_pos;
					lmd->SpringList[i].vel = v;
					}
				}
			else
				{
				fract = t %tps;
				frames =  t / tps;
				StartFrame = lmd->lastFrame+tps;
				int nv = lmd->SpringList.Count();
				for (int i=0;i<nv;i++)
					{
					lmd->SpringList[i].pos = lmd->WholeFrameCache[i].pos;
					lmd->SpringList[i].vel = lmd->WholeFrameCache[i].vel;
					}

				}

			for (int i = StartFrame; i < (frames*tps); i+=tps)
				{
				Matrix3 tm = lmd->SelfNode->GetObjectTM(i);



				Point3 pp,p,force;

				ObjectState tos;
				int nv = lmd->SpringList.Count();
				int tnv = nv;
				if (i != t)
					{
					if (TestAFlag(A_RENDER)) 
						{
						if (instanced==1)
							EnableModsAbove(lmd,FALSE);
						tos = lmd->SelfNode->EvalWorldState(i);
						if (instanced==1)
							EnableModsAbove(lmd,TRUE);
						}
					}
				}
			}
		}
*/
	return 0;
	}

int LagMod::RenderEnd(TimeValue t)
	{
	ClearAFlag(A_RENDER);
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	nukeCache = FALSE;

	return 0;
	}




Interval LagMod::LocalValidity(TimeValue t)
	{
	Interval iv = NEVER;
	iv.Set(t,t);
	return iv;
	}


Matrix3 LagMod::CompMatrix(TimeValue t,INode *inode,ModContext *mc)
	{
	Interval iv;
	Matrix3 tm(1);	
	if (inode) 
		tm = tm * inode->GetObjTMBeforeWSM(t,&iv);
	return tm;
	}

int LagMod::HitTest(
		TimeValue t, INode* inode, 
		int type, int crossing, int flags, 
		IPoint2 *p, ViewExp *vpt, ModContext* mc)
	{

	
	LagModData *lmd = (LagModData *) mc->localData;

	if (lmd == NULL) return 0;


	GraphicsWindow *gw = vpt->getGW();
	Point3 pt;
	HitRegion hr;
	int savedLimits, res = 0;
	Matrix3 tm = CompMatrix(t,inode,mc);

	MakeHitRegion(hr,type, crossing,4,p);
	gw->setHitRegion(&hr);	
	gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);	
	gw->setTransform(tm);

	// Hit test start point
	if (ip && ip->GetSubObjectLevel()==1) {

		if ((flags&HIT_SELONLY   &&  sel[0]) ||
			(flags&HIT_UNSELONLY && !sel[0]) ||
			!(flags&(HIT_UNSELONLY|HIT_SELONLY)) ) {
	
			gw->clearHitCode();
			p1->GetValue(t,&pt,FOREVER,CTRL_ABSOLUTE);
			gw->marker(&pt,HOLLOW_BOX_MRKR);
			if (gw->checkHitCode()) {
				vpt->LogHit(inode, mc, gw->getHitDistance(), 0, NULL); 
				res = 1;
				}
			}
		}
	else if ((ip && ip->GetSubObjectLevel()==2) )
		{


//		ObjectState os;

//		os = inode->EvalWorldState(t);
//loop through points checking for selection and then marking for points
//		int nv = os.obj->NumPoints(); 
		int nv = lmd->SpringList.Count();;
		for (int i=0;i<nv;i++)
			{
			if ((flags&HIT_SELONLY   &&  lmd->esel[i]) ||
				(flags&HIT_UNSELONLY && !lmd->esel[i]) ||
				!(flags&(HIT_UNSELONLY|HIT_SELONLY)) ) 
				{

				Point3 pt;
				gw->clearHitCode();
//				pt = os.obj->GetPoint(i);
				pt = lmd->SpringList[i].LocalPt;
				gw->setColor(LINE_COLOR, 1.0f,1.0f,1.0f);
				gw->marker(&pt,POINT_MRKR);
				if (gw->checkHitCode()) {
					vpt->LogHit(inode, mc, gw->getHitDistance(), i, NULL); 
					res = 1;
					}
				}
			}

		}
	else if ( (ip && ip->GetSubObjectLevel()==3) )
		{


//		ObjectState os;

//		os = inode->EvalWorldState(t);
//loop through points checking for selection and then marking for points
//		int nv = os.obj->NumPoints(); 
		int nv = lmd->SpringList.Count();;

		for (int i=0;i<nv;i++)
			{
			if ((flags&HIT_SELONLY   &&  lmd->wsel[i]) ||
				(flags&HIT_UNSELONLY && !lmd->wsel[i]) ||
				!(flags&(HIT_UNSELONLY|HIT_SELONLY)) ) 
				{

				Point3 pt;
				gw->clearHitCode();
				pt = lmd->SpringList[i].LocalPt;
//				pt = os.obj->GetPoint(i);
				gw->setColor(LINE_COLOR, 1.0f,1.0f,1.0f);
				gw->marker(&pt,POINT_MRKR);
				if (gw->checkHitCode()) {
					vpt->LogHit(inode, mc, gw->getHitDistance(), i, NULL); 
					res = 1;
					}
				}
			}

		}


	gw->setRndLimits(savedLimits);
	
	return res;

		return 0;
	}


void LagMod::DrawCrossSectionNoMarkers(Point3 a, Point3 align, float length, GraphicsWindow *gw)

{

#define NNUM_SEGS	8

Point3 plist[NNUM_SEGS+1];
Point3 mka,mkb,mkc,mkd;

align = Normalize(align);
	{
	int ct = 0;
	float angle = TWOPI/float(NNUM_SEGS) ;
	Matrix3 rtm = RotAngleAxisMatrix(align, angle);
	Point3 p(0.0f,0.0f,0.0f);
	if (align.x == 1.0f)
		{
		p.z = length;
		}
	else if (align.y == 1.0f)
		{
		p.x = length;
		}
	else if (align.z == 1.0f)
		{
		p.y = length;
		}
	else if (align.x == -1.0f)
		{
		p.z = -length;
		}
	else if (align.y == -1.0f)
		{
		p.x = -length;
		}
	else if (align.z == -1.0f)
		{
		p.y = -length;
		}
	else 
		{
		p = Normalize(align^Point3(1.0f,0.0f,0.0f))*length;
		}

	for (int i=0; i<NNUM_SEGS; i++) {
		p = p * rtm;
		plist[ct++] = p;
		}

	p = p * rtm;
	plist[ct++] = p;


	for (i=0; i<NNUM_SEGS+1; i++) 
		{
		plist[i].x += a.x;
		plist[i].y += a.y;
		plist[i].z += a.z;
		}
	}
mka = plist[15];
mkb = plist[3];
mkc = plist[7];
mkd = plist[11];

gw->polyline(NNUM_SEGS+1, plist, NULL, NULL, 0);

}



int LagMod::Display(
		TimeValue t, INode* inode, ViewExp *vpt, 
		int flagst, ModContext *mc)
	{

	if (inode)
		{
		if (ip)
			{
			int nodeCount = ip->GetSelNodeCount();
			BOOL found = FALSE;
			for (int nct =0; nct < nodeCount; nct++)
				{
				if (inode == ip->GetSelNode(nct))
					{
					found = TRUE;
					nct = nodeCount;
					}
				}
			if (!found) return 0;
			}

		}
	GraphicsWindow *gw = vpt->getGW();
	Point3 pt[4];
	Matrix3 tm = CompMatrix(t,inode,mc);
	int savedLimits;

	LagModData *lmd = (LagModData *) mc->localData;

	if (lmd == NULL) return 0;

	gw->setRndLimits((savedLimits = gw->getRndLimits()) & ~GW_ILLUM);
	gw->setTransform(tm);
	
	// Draw start point
	if (ip && ip->GetSubObjectLevel() == 1) {
		if (sel[0]) 
			 gw->setColor(LINE_COLOR, (float)1.0, (float)0.0, (float)0.0);			 
		 else 
			gw->setColor(LINE_COLOR,(float)0.0, (float)0.0, (float)1.0);
			
	
	
		p1->GetValue(t,&pt[0],FOREVER,CTRL_ABSOLUTE);
		gw->marker(&pt[0],HOLLOW_BOX_MRKR);
		}
	else if (ip && ip->GetSubObjectLevel() == 2) {
//		ObjectState os;

//		os = inode->EvalWorldState(t);
		int nv = lmd->SpringList.Count();;
		for (int i = 0; i<nv;i++)
			{
			if (lmd->esel[i] == 1)
				{
				Point3 pt;
				pt = lmd->SpringList[i].LocalPt;
//				pt = os.obj->GetPoint(i);
				gw->setColor(LINE_COLOR, 1.0f,0.0f,1.0f);
				gw->marker(&pt,HOLLOW_BOX_MRKR);

				}
			}

		}
	else if (ip && ip->GetSubObjectLevel() == 3) {
//		ObjectState os;

//		os = inode->EvalWorldState(t);
//		int nv = os.obj->NumPoints();
		int nv = lmd->SpringList.Count();;
		for (int i = 0; i<nv;i++)
			{
			if (lmd->wsel[i] == 1)
				{
				Point3 pt;
				pt = lmd->SpringList[i].LocalPt;
//				pt = os.obj->GetPoint(i);
				gw->setColor(LINE_COLOR, 1.0f,1.0f,1.0f);
				gw->marker(&pt,HOLLOW_BOX_MRKR);

				}
			}

		}

	if (ip && (ip->GetSubObjectLevel() != 0) ) 
		{

		if (inPaint)
			{

			if (lmd->isHit)
				{
//draw 3d cursor
				Point3 x(1.0f,0.0f,0.0f),y(0.0f,1.0f,0.0f),z(0.0f,0.0f,1.0f);
				gw->setColor(LINE_COLOR, 1.0f,1.0f,0.0f);

				DrawCrossSectionNoMarkers(lmd->hitPoint, x, Radius, gw); 
				DrawCrossSectionNoMarkers(lmd->hitPoint, y, Radius, gw); 
				DrawCrossSectionNoMarkers(lmd->hitPoint, z, Radius, gw); 

				}

			}
		}

	if ((ip && (ip->GetSubObjectLevel() != 0) ) && (this->IsEnabled()) && (this->IsEnabledInViews()))
		{


		ObjectState os;

		os = inode->EvalWorldState(t);
//loop through points checking for selection and then marking
		float r,g,b;

		for (int i=0;i<lmd->SpringList.Count();i++)
			{
			Point3 pt;
//			pt = os.obj->GetPoint(i);
			
			pt = lmd->SpringList[i].LocalPt;
//gte red is strongest,green, blue is weakest based on influence
			float infl;
			if (lmd->wsel[i]==1)
				{
				if (absoluteWeight)
					infl = weightDelta;
					else infl = 1.0f - lmd->SpringList[i].InheritVel + weightDelta;
				}
			else infl = 1.0f - lmd->SpringList[i].InheritVel;
			Point3 selColor(0.0f,0.0f,0.0f);
			Point3 selSoft = GetUIColor(COLOR_SUBSELECTION_SOFT);
			Point3 selMedium = GetUIColor(COLOR_SUBSELECTION_MEDIUM);
			Point3 selHard = GetUIColor(COLOR_SUBSELECTION_HARD);

			if (infl <= 0.0f) selColor = GetUIColor(COLOR_SUBSELECTION_SOFT);
			else if ( (infl<0.5) && (infl >= 0.0f))
				{
				selColor = selSoft + ( (selMedium-selSoft) * (infl/0.5f));
				}
			else if (infl<=1.0)
				{
				selColor = selMedium + ( (selHard-selMedium) * ((infl-0.5f)/0.5f));
				}
			else 
				{
				selColor = GetUIColor(COLOR_SUBSELECTION);
				}
			r = selColor.x;
			g = selColor.y;
			b = selColor.z;

			gw->setColor(LINE_COLOR, r,g,b);
			gw->marker(&pt,PLUS_SIGN_MRKR);

			if (inPaint)
				{
				if (lmd->psel[i] == 1)
					{
					gw->setColor(LINE_COLOR, 1.0f,1.0f,1.0f);
					gw->marker(&pt,HOLLOW_BOX_MRKR);
					}

				}



			}
		}

//	gw->marker(&pt[1],HOLLOW_BOX_MRKR);

	gw->setRndLimits(savedLimits);

	return 0;

	}

void LagMod::GetWorldBoundBox(
		TimeValue t,INode* inode, ViewExp *vpt, 
		Box3& box, ModContext *mc)
	{
	
	if (inPaint)
		{
		Matrix3 tm = CompMatrix(t,inode,mc);
		Point3 pt1, pt2;
		box.Init();
		LagModData *lmd = (LagModData *) mc->localData;
		if (lmd == NULL) return;

		pt1 = lmd->hitPoint;
		box += pt1 * tm;
		box.EnlargeBy(Radius*2.5f);
		}

	
	}

void LagMod::Move(TimeValue t, Matrix3& partm, Matrix3& tmAxis, 
		Point3& val, BOOL localOrigin)
	{
	if (ip && ip->GetSubObjectLevel() == 1) {
		if (sel[0]) {
			p1->SetValue(
				ReferenceFrame,VectorTransform(tmAxis*Inverse(partm),val),
				TRUE,CTRL_RELATIVE);
			updateWeights = TRUE;
			}
		}
	else if (ip && ip->GetSubObjectLevel() == 2) {
		}
	else if (ip && ip->GetSubObjectLevel() == 3) {
		}

	}

void LagMod::GetSubObjectCenters(
		SubObjAxisCallback *cb,TimeValue t,
		INode *node,ModContext *mc)
	{

	Matrix3 tm = CompMatrix(t,node,mc);
	Point3 pt(0,0,0), p;
	int c=0;
	if (sel[0]) {
		p1->GetValue(t,&p,FOREVER,CTRL_ABSOLUTE);
		pt += p;
		c++;
		}
	if (c) pt /= float(c);
	tm.PreTranslate(pt);
	cb->Center(tm.GetTrans(),0);

	}

void LagMod::GetSubObjectTMs(
		SubObjAxisCallback *cb,TimeValue t,
		INode *node,ModContext *mc)
	{
	Matrix3 tm = CompMatrix(t,node,mc);
	cb->TM(tm,0);
	
	}


void LagMod::SelectSubComponent(
		HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert)
	{

	LagModData *lmd = (LagModData*)hitRec->modContext->localData;

	if (lmd == NULL) return;

	if (ip && ip->GetSubObjectLevel() == 1) {
	
		while (hitRec) {
			assert(hitRec->hitInfo<=1);
			BOOL state = selected;
			if (invert) state = !sel[hitRec->hitInfo];
			if (state) sel[hitRec->hitInfo] = 1;
			else       sel[hitRec->hitInfo] = 0;
			if (!all) break;
			hitRec = hitRec->Next();
			}	
		}
	else if ( (ip && ip->GetSubObjectLevel() == 2) || (ip && ip->GetSubObjectLevel() == 3)){
		BOOL add = GetKeyState(VK_CONTROL)<0;
		BOOL sub = GetKeyState(VK_MENU)<0;
		updateWeights = TRUE;

		if (!add && !sub) 
			{
			if (ip && ip->GetSubObjectLevel() == 2)
				{
				for (int i =0;i<lmd->esel.Count();i++)
					lmd->esel[i] = FALSE;
				}
			else
				{
				for (int i =0;i<lmd->wsel.Count();i++)
					lmd->wsel[i] = FALSE;
				}
			}

		int Count = 0;
		BOOL state = selected;
		while (hitRec) {
			state = hitRec->hitInfo;
			if (sub)
				{
				if (ip && ip->GetSubObjectLevel() == 2)
					lmd->esel[state] = FALSE;
				else lmd->wsel[state] = FALSE;
				}
			else 
				{
				if (ip && ip->GetSubObjectLevel() == 2)
					lmd->esel[state] = TRUE;
				else lmd->wsel[state] = TRUE;
				}

		
			hitRec = hitRec->Next();
			Count++;
			}	
		}

	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	
	}

void LagMod::ClearSelection(int selLevel)
	{
	ModContextList mcList;		
	INodeTab nodes;

	if (ip)
		{
		ip->GetModContexts(mcList,nodes);
		int objects = mcList.Count();

		for ( int i = 0; i < objects; i++ ) 
			{
			LagModData *lmd = (LagModData*)mcList[i]->localData;
			if (lmd)
				{
				if (ip->GetSubObjectLevel() == 2)
					{
					for (int i =0 ; i < lmd->esel.Count(); i++)
						lmd->esel[i] = 0;
					}
				else if (ip->GetSubObjectLevel() == 3)
					{
					for (int i =0 ; i < lmd->wsel.Count(); i++)
						lmd->wsel[i] = 0;
					}
				}	
			}
		}

	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	}

void LagMod::SelectAll(int selLevel)
	{

	ModContextList mcList;		
	INodeTab nodes;

	if (ip)
		{
		ip->GetModContexts(mcList,nodes);
		int objects = mcList.Count();

		for ( int i = 0; i < objects; i++ ) 
			{
			LagModData *lmd = (LagModData*)mcList[i]->localData;
			if (lmd)
				{
				if (ip->GetSubObjectLevel() == 2)
					{
					for (int i =0 ; i < lmd->esel.Count(); i++)
						lmd->esel[i] = 1;
					}
				else if (ip->GetSubObjectLevel() == 3)
					{
					for (int i =0 ; i < lmd->wsel.Count(); i++)
						lmd->wsel[i] = 1;
					}
				}
			}
		}

	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	}

void LagMod::InvertSelection(int selLevel)
	{
	ModContextList mcList;		
	INodeTab nodes;

	if (ip)
		{
		ip->GetModContexts(mcList,nodes);
		int objects = mcList.Count();

		for ( int i = 0; i < objects; i++ ) 
			{
			LagModData *lmd = (LagModData*)mcList[i]->localData;
			if (lmd)
				{
				if (ip->GetSubObjectLevel() == 2)
					{
					for (int i =0 ; i < lmd->esel.Count(); i++)
						lmd->esel[i] = !lmd->esel[i];
					}
				else if (ip->GetSubObjectLevel() == 3)
					{
					for (int i =0 ; i < lmd->wsel.Count(); i++)
						lmd->wsel[i] = !lmd->wsel[i];
					}
				}
			}
		}

	NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
	}

void LagMod::ActivateSubobjSel(int level, XFormModes& modes)
	{
	switch (level) {
		case 0:
			if (ip->GetCommandMode() == PaintMode) {
				ip->SetStdCommandMode(CID_OBJMOVE);
				return;
				}
			iPaintButton->Enable(FALSE);

			break;
		case 1: // Center Point
			iPaintButton->Enable(TRUE);
			modes = XFormModes(moveMode,NULL,NULL,NULL,NULL,NULL);
			break;		
		case 2: // Edge
			iPaintButton->Enable(TRUE);
			modes = XFormModes(moveMode,NULL,NULL,NULL,NULL,NULL);
			break;		
		case 3: // weigths
			iPaintButton->Enable(TRUE);
			modes = XFormModes(moveMode,NULL,NULL,NULL,NULL,NULL);
			break;		
		}
	NotifyDependents(FOREVER,PART_DISPLAY,REFMSG_CHANGE);
	}


void LagMod::StartPaintMode()
{
if ( !ip ) return;
if (ip && ( (ip->GetSubObjectLevel()==1) || (ip->GetSubObjectLevel()==2) || (ip->GetSubObjectLevel()==3) ) )
	{

//get mesh
	if (ip->GetCommandMode() == PaintMode) {
		ip->SetStdCommandMode(CID_OBJMOVE);
		return;
		}

    ip->SetCommandMode(PaintMode);

    }
}


RefTargetHandle LagMod::GetReference(int i)
	{
	switch (i) {
		case PBLOCK_REF: return (RefTargetHandle)pblock2;

		case POINT1_REF: return (RefTargetHandle)p1;

		default : return NULL;
		}
	return NULL;
	}

void LagMod::SetReference(int i, RefTargetHandle rtarg)
	{
	switch (i) {
		case PBLOCK_REF: pblock2 = (IParamBlock2*)rtarg; break;
		case POINT1_REF: p1     = (Control*)rtarg; break;
		}
	}


Animatable* LagMod::SubAnim(int i)
	{
	switch (i) {
		case PBLOCK_REF: 		return pblock2;
		case POINT1_REF: 		return p1;
		default: 			return NULL;   
		}
	}

TSTR LagMod::SubAnimName(int i)
	{
	if (i==POINT1_REF) return GetString(IDS_RB_LAGPOINTS);
	return _T(""); 

	}

RefResult LagMod::NotifyRefChanged(
		Interval changeInt,RefTargetHandle hTarget, 
		PartID& partID, RefMessage message)
	{
	switch (message) {
		case REFMSG_CHANGE:	
			{ 
			if (hTarget == pblock2)
				{
				ParamID changing_param = pblock2->LastNotifyParamID();

				lag_param_blk.InvalidateUI(changing_param);
				}

			break;
			}


		}
	return REF_SUCCEED;
	}


class TempStorageClass
{
public:
	Point3 pos;
	float IVel;
	BYTE sel;
	BOOL modified;
};


#define BACKPATCH_CHUNK		0x2120


IOResult LagMod::Save(ISave *isave)
	{
	Modifier::Save(isave);
	ULONG nb;

	ULONG id = isave->GetRefID(container);

	isave->BeginChunk(BACKPATCH_CHUNK);
	isave->Write(&id,sizeof(ULONG),&nb);
	isave->EndChunk();



	return IO_OK;
	}



class LagModPostLoad : public PostLoadCallback {
	public:
		LagMod *n;
		LagModPostLoad(LagMod *ns) {n = ns;}
		void proc(ILoad *iload) {  
			if (n->container != NULL)
				{
				n->container->RegisterTVNodeNotify(n->notify);
				}
			delete this; 


			} 
	};


IOResult LagMod::Load(ILoad *iload)
	{

	IOResult res = IO_OK;
	int NodeID = 0;
	ULONG nb;



	Modifier::Load(iload);

	TempStorageClass tsc;


	int ct = 0;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case BACKPATCH_CHUNK:
				ULONG id;
				iload->Read(&id,sizeof(ULONG), &nb);
				if (id!=0xffffffff)
					{
					iload->RecordBackpatch(id,(void**)&container);
					}

				break;
			case POS_WEIGHT_CHUNK:
				Point3 Zero(0.0f,0.0f,0.0f);
			
//read in data
				int ct;
				iload->Read(&ct, sizeof(ct), &nb);
				oldSpringList.SetCount(ct);
				oldesel.SetCount(ct);
				for (int i=0;i<ct;i++)
					{
					iload->Read(&tsc, sizeof(tsc), &nb);

					oldSpringList[i].pos = tsc.pos;
					oldSpringList[i].init_pos = tsc.pos;
					oldSpringList[i].vel = Zero;
					oldSpringList[i].InheritVel = tsc.IVel;
					oldSpringList[i].modified = tsc.modified;
					oldesel[i] = tsc.sel;
				
					}

				break;

			}


		iload->CloseChunk();
		if (res!=IO_OK)
			return res;
		

		}

	iload->RegisterPostLoadCallback(new LagModPostLoad(this));

	return IO_OK;
}

Point3 LagDeformer::Map(int i, Point3 p)
	{

	if (lmd)
		{
		if (i < lmd->SpringList.Count())
			{
			float IVel = 0.0f;
		
			if (lmd->wsel[i] == 1)
				{
				if (mod->absoluteWeight) 
					IVel = (1.0f-mod->weightDelta) * mod->falloff;
				else IVel = (lmd->SpringList[i].InheritVel-mod->weightDelta) * mod->falloff;
				}
			else IVel = lmd->SpringList[i].InheritVel * mod->falloff;
			p = (lmd->SpringList[i].pos * lmd->InverseTM * IVel)+(p *(1.0f-IVel));
			lmd->SpringList[i].LocalPt = p;
			}
		}
	return p;


	}

void LagMod::ResetSprings()

{
ModContextList mcList;		
INodeTab nodes;

if (!ip) return;

ip->GetModContexts(mcList,nodes);
int objects = mcList.Count();

for ( int k = 0; k < objects; k++ ) 
	{

	LagModData *lmd = (LagModData*)mcList[k]->localData;
	
	if (lmd)
		{

		for (int i = 0; i < lmd->SpringList.Count(); i++)
			lmd->SpringList[i].modified = FALSE;
		}
	}
}


void LagMod::AddForceField()

{
int ct = pblock2->Count(lag_force_node);
ff.SetCount(ct);
for (int i = 0; i < ct; i++)
	{
	INode *node = NULL;
	Interval iv;
	pblock2->GetValue(lag_force_node,0,node,iv, i);
	if (node != NULL)
		{
		WSMObject *obref=(WSMObject*)node->GetObjectRef();
		ff[i]=obref->GetForceField(node);
		}
	else ff[i]=NULL;
	}
}

Point3 LagMod::GetForce(TimeValue t, Point3 p, Point3 v, Matrix3 tm)
{

Point3 f(0.0f,0.0f,0.0f);
	


int ct = pblock2->Count(lag_force_node);
for (int i = 0; i < ct; i++)
	{
	INode *node = NULL;
	Interval iv;
	pblock2->GetValue(lag_force_node,0,node,iv, i);
	if ((node != NULL) && (ff[i] != NULL))
		f += 16000.0f * ff[i]->Force(t,p,v,0);
	}

return f;
}


void LagMod::ComputeCenterFalloff(LagModData *lmd, TimeValue t, ObjectState *os)
{
	Point3 Center;
	Interval iv;
	float Dist =0.0f;

	p1->GetValue(t,&Center,iv,CTRL_ABSOLUTE);
	int nv = os->obj->NumPoints(); 

//build a temp edge list here 
	Tab<Point3> EdgeList;
	EdgeList.Append(1,&Center,1);

	Tab<int> EdgeIndex;
	EdgeIndex.SetCount(nv);

	for (int i = 0; i< lmd->esel.Count();i++)
		{
		if (lmd->esel[i]==1)
			{
			Point3 ep = os->obj->GetPoint(i);
			EdgeList.Append(1,&ep,1);
			}

		}


	for (i=0;i<nv;i++)
		{
//loop through all edges find closest
		float temp = Length(EdgeList[0] - os->obj->GetPoint(i));
		EdgeIndex[i] = 0;
		for (int j = 1; j < EdgeList.Count(); j++)
			{
			float etemp;
			etemp = Length(EdgeList[j] - os->obj->GetPoint(i));
			if (etemp < temp) 
				{
				temp = etemp;
				EdgeIndex[i] = j;
				}
			}
//				temp = Length(Center - os->obj->GetPoint(i));
		if (temp>Dist)
			Dist = temp;
		}
	BOOL nuke = FALSE;

	for (i=0;i<nv;i++)
		{
		float temp;
		temp = Length(EdgeList[EdgeIndex[i]] - os->obj->GetPoint(i))/Dist;
		temp = (temp*temp);
		if (!lmd->SpringList[i].modified) 
			{
			lmd->SpringList[i].InheritVel = temp;
			}
					
		}
	EdgeList.ZeroCount();

}



LagMod::EnableModsAbove(LagModData *lmd, BOOL enable)
	{

	int				i;
	SClass_ID		sc;
	IDerivedObject* dobj;

// return the indexed modifier in the mod chain
	INode *node = lmd->SelfNode;
	BOOL found = FALSE;

// then osm stack
	Object* obj = node->GetObjectRef();
	int ct = 0;

	if ((sc = obj->SuperClassID()) == GEN_DERIVOB_CLASS_ID)
		{
		dobj = (IDerivedObject*)obj;
		if (!enable) 
			{
			enableStates.ZeroCount();
			enableViewStates.ZeroCount();
			}

		while (sc == GEN_DERIVOB_CLASS_ID)
			{
			for (i = 0; i < dobj->NumModifiers(); i++)
				{
				TSTR name;
				Modifier *m = dobj->GetModifier(i);
				m->GetClassName(name);
				if (this == dobj->GetModifier(i))
					found = TRUE;


				BOOL en = m->IsEnabled();
				BOOL env = m->IsEnabledInViews();
//				DebugPrint("%s enable state %d view %d\n",name,en,env);
				if (!enable)
					{
					if (!found)
						{
						enableStates.Append(1,&en,1);
						enableViewStates.Append(1,&env,1);
						m->DisableMod();
						m->DisableModInViews();
						}
					}
				else 
					{
					if (!found)
						{
						if (!enableStates[ct])
							m->DisableMod();
						else m->EnableMod();
						if (!enableViewStates[ct++])
							m->DisableModInViews();
						else m->EnableModInViews();
						}
					}

				}
			dobj = (IDerivedObject*)dobj->GetObjRef();
			sc = dobj->SuperClassID();
			}
		}

	if ((dobj = node->GetWSMDerivedObject()) != NULL)
		for (i = 0; i < dobj->NumModifiers(); i++)
			{
			Modifier *m = dobj->GetModifier(i);
			BOOL en = m->IsEnabled();
			BOOL env = m->IsEnabledInViews();
			if (!enable)
				{
				enableStates.Append(1,&en,1);
				enableViewStates.Append(1,&env,1);
				}
			else
				{
				if (!enableStates[ct])
					m->DisableMod();
				else m->EnableMod();
				if (!enableViewStates[ct++])
					m->DisableModInViews();
				else m->EnableModInViews();

				}

			}


	}

/*

LagMod::EnableModsAbove(LagModData *lmd, BOOL enable)
	{

	int				i;
	SClass_ID		sc;
	IDerivedObject* dobj;

// return the indexed modifier in the mod chain
	INode *node = lmd->SelfNode;
	BOOL found = FALSE;

// then osm stack
	Object* obj = node->GetObjectRef();
	int ct = 0;

	if ((sc = obj->SuperClassID()) == GEN_DERIVOB_CLASS_ID)
		{
		dobj = (IDerivedObject*)obj;
		if (!enable) enableStates.ZeroCount();

		while (sc == GEN_DERIVOB_CLASS_ID)
			{
			for (i = 0; i < dobj->NumModifiers(); i++)
				{
				TSTR name;
				Modifier *m = dobj->GetModifier(i);
				m->GetClassName(name);
				DebugPrint("%s\n",name);
				if (this == dobj->GetModifier(i))
					found = TRUE;


				BOOL en = m->IsEnabled();
				if (!enable)
					{
					enableStates.Append(1,&en,1);
					if (!found)
						m->DisableMod();
					}
				else 
					{
					if (enableStates[ct++])
						m->DisableMod();
					else m->EnableMod();
					}

				}
			dobj = (IDerivedObject*)dobj->GetObjRef();
			sc = dobj->SuperClassID();
			}
		}

	if ((dobj = node->GetWSMDerivedObject()) != NULL)
		for (i = 0; i < dobj->NumModifiers(); i++)
			{
			Modifier *m = dobj->GetModifier(i);
			BOOL en = m->IsEnabled();
			if (!enable)
				{
				enableStates.Append(1,&en,1);
				}
			else
				{
				if (enableStates[ct++])
					m->DisableMod();
				else m->EnableMod();

				}

			}


	}

*/

void LagMod::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
	{	


//this is a hack to handle if scatter is in my dependancy
//scatter and lag do not get along so when scatter using its animationn offset i turn my self off
//to prevent an infintie loop of lag and scatter trying to update themselves 
    MyEnumProc dep;              
	dep.nukeME = FALSE;
	dep.xRefFound = FALSE;
	dep.count = 0;
	EnumDependents(&dep);
	if (dep.nukeME) return;

	int instanced = dep.count;

	if (os->obj->IsWorldSpaceObject()) return;

	int nv = os->obj->NumPoints();

	int tps = GetTicksPerFrame();

//7-1-99
	BOOL isXRefObj = dep.xRefFound;

	
	if (ip != NULL && editMod == this)
		{
		ModContextList mcList;
		INodeTab nodes;
		if (mc.localData == NULL)
			{

			ip->GetModContexts(mcList,nodes);

//add a new inode to trackview
			ITrackViewNode *tvr = ip->GetTrackViewRootNode();
			ITrackViewNode *global = tvr->GetNode(Class_ID(0xb27e9f2a, 0x73fad370));
			ITrackViewNode *tvroot = global->GetNode(LAGCONTAINERMASTER_TVNODE_CLASS_ID);
			if (!tvroot) 
				{
				ITrackViewNode *mcontainer =  CreateITrackViewNode();
				global->AddNode(mcontainer,GetString(IDS_PW_FLEXDATA),LAGCONTAINERMASTER_TVNODE_CLASS_ID);
				tvroot = mcontainer;
				}

//add a new a container
			container =  CreateITrackViewNode(TRUE);
			tvroot->AddNode(container,GetString(IDS_PW_FLEXDATA),LAGCONTAINER_TVNODE_CLASS_ID);
			for (int i = 0; i < nodes.Count(); i++)
				{
				container->AddController(nodes[i]->GetTMController(),nodes[i]->GetName(),LAGNODE_TVNODE_CLASS_ID);
				LagModData *d  = new LagModData(i,nodes[i]);
				mcList[i]->localData = d;
				}
			container->RegisterTVNodeNotify(notify);
//load up old pre beta files
			if (oldSpringList.Count() != 0)
				{
				LagModData *d = (LagModData *) mcList[0]->localData;

				d->SpringList = oldSpringList;
				d->esel = oldesel;
				d->psel.SetCount(oldesel.Count());
				d->wsel.SetCount(oldesel.Count());
				for (int esel_index = 0; esel_index < nv; esel_index++)
					{
					d->psel[esel_index] = 0;
					d->wsel[esel_index] = 0;
	
					}


				}

			NotifyDependents(FOREVER, OBJ_CHANNELS, REFMSG_CHANGE);
			Interval valid;
			valid.SetEmpty();
			os->obj->UpdateValidity(GEOM_CHAN_NUM,valid); // Have to do this to get it to evaluate

			return;
					
			}
		else
			{

			LagModData *d = (LagModData *) mc.localData;
			if (d->SelfNode == NULL)
				{

				ip->GetModContexts(mcList,nodes);

//add a new inode to trackview
				ITrackViewNode *tvr = ip->GetTrackViewRootNode();
				ITrackViewNode *global = tvr->GetNode(Class_ID(0xb27e9f2a, 0x73fad370));
				ITrackViewNode *tvroot = global->GetNode(LAGCONTAINERMASTER_TVNODE_CLASS_ID);
				if (!tvroot) 
					{
					ITrackViewNode *mcontainer =  CreateITrackViewNode();
					global->AddNode(mcontainer,GetString(IDS_PW_FLEXDATA),LAGCONTAINERMASTER_TVNODE_CLASS_ID);
					tvroot = mcontainer;
					}
//add a new a container
				container = CreateITrackViewNode(TRUE);
				tvroot->AddNode(container,GetString(IDS_PW_FLEXDATA),LAGCONTAINER_TVNODE_CLASS_ID);
				for (int i = 0; i < nodes.Count(); i++)
					{
					container->AddController(nodes[i]->GetTMController(),nodes[i]->GetName(),LAGNODE_TVNODE_CLASS_ID);
					d->SelfNode = nodes[i];
					}
				container->RegisterTVNodeNotify(notify);

				NotifyDependents(FOREVER, OBJ_CHANNELS, REFMSG_CHANGE);
				Interval valid;
				valid.SetEmpty();
				os->obj->UpdateValidity(GEOM_CHAN_NUM,valid); // Have to do this to get it to evaluate
				return;
				}


					
			}

//		return;

		}



	LagModData *lmd = (LagModData *) mc.localData;

//7-1-99 special acase code to handle xrefs since xrefs don't save thus the backpointer is wrong
	if (isXRefObj)
		{
		lmd->SelfNode = NULL;
		if (dep.Nodes.Count() > 0)
			lmd->SelfNode = dep.Nodes[0];
		}
//this is here to handle a merge you will have a lmd nad self node but no container which must be recreated
	else if ((container == NULL) && (lmd) && (lmd->SelfNode))
		{
//add a new inode to trackview
		Interface *tip = ip;
		if (!tip) tip = GetCOREInterface();
		ITrackViewNode *tvr = tip->GetTrackViewRootNode();
		ITrackViewNode *global = tvr->GetNode(Class_ID(0xb27e9f2a, 0x73fad370));
		ITrackViewNode *tvroot = global->GetNode(LAGCONTAINERMASTER_TVNODE_CLASS_ID);
		if (!tvroot) 
			{
			ITrackViewNode *mcontainer =  CreateITrackViewNode();
			global->AddNode(mcontainer,GetString(IDS_PW_FLEXDATA),LAGCONTAINERMASTER_TVNODE_CLASS_ID);
			tvroot = mcontainer;
			}
//add a new a container
		container = CreateITrackViewNode(TRUE);
		tvroot->AddNode(container,GetString(IDS_PW_FLEXDATA),LAGCONTAINER_TVNODE_CLASS_ID);
		container->AddController(lmd->SelfNode->GetTMController(),lmd->SelfNode->GetName(),LAGNODE_TVNODE_CLASS_ID);
		container->RegisterTVNodeNotify(notify);
		}
	else 	if (container == NULL)
		{
		return;
		}


	if ((ip && editMod == this) && (lmd) && (lmd->SelfNode == NULL))
		{
		NotifyDependents(FOREVER, OBJ_CHANNELS, REFMSG_CHANGE);
		Interval valid;
		valid.SetEmpty();
		os->obj->UpdateValidity(GEOM_CHAN_NUM,valid); // Have to do this to get it to evaluate
		return;
		}


	Point3 Center;
	Interval iv = FOREVER;
	pblock2->GetValue(lag_absolute,t,absoluteWeight,iv);
	pblock2->GetValue(lag_flex,t,falloff,iv);
	
	pblock2->GetValue(lag_strength,t,strength,iv);

	pblock2->GetValue(lag_sway,t,dampening,iv);

	pblock2->GetValue(lag_paint_strength,t,PaintStrength,iv);
	pblock2->GetValue(lag_paint_radius,t,Radius,iv);
	pblock2->GetValue(lag_paint_feather,t,Feather,iv);
	pblock2->GetValue(lag_paint_backface,t,ProjectThrough,iv);



	strength = strength/100.0f;
	dampening = dampening/100.0f;

	pblock2->GetValue(lag_referenceframe,t,ReferenceFrame,iv);
	ReferenceFrame = ReferenceFrame * GetTicksPerFrame();


//check if SpringList is zero or does not match current vertex count nuke are and rebuild

	if (lmd != NULL)
		{
		}


	if ((lmd != NULL) && (lmd->SelfNode != NULL))
		{

		if (TestAFlag(A_RENDER)) 
			{
			if (lmd->nukeRenderCache)
				{
				lmd->lastFrame = 999999999;
				lmd->nukeRenderCache = FALSE;
				}
			}
		else
			{
			lmd->nukeRenderCache = TRUE;
			}

		if (lmd->esel.Count() != nv)
			{
			lmd->esel.ZeroCount();
			lmd->psel.ZeroCount();
			lmd->wsel.ZeroCount();
			lmd->esel.SetCount(nv);
			lmd->psel.SetCount(nv);
			lmd->wsel.SetCount(nv);

			for (int esel_index = 0; esel_index < nv; esel_index++)
				{
				lmd->esel[esel_index] = 0;
				lmd->psel[esel_index] = 0;
				lmd->wsel[esel_index] = 0;

				}
			}

		Point3 v(0.0f,0.0f,0.0f);
		lmd->InverseTM = Inverse(lmd->SelfNode->GetObjectTM(t));
		float Dist =0.0f;

		int ect = 0;
		for (int i = 0; i< lmd->esel.Count();i++)
			{
			if (lmd->esel[i]==1)
				ect++;
			}

		if ((nv == ect) || (updating)) 
			{
			if (updating)
				{
				if (lmd->pointCache.Count() != os->obj->NumPoints())
					lmd->pointCache.SetCount(os->obj->NumPoints());
				for (int i = 0; i < lmd->pointCache.Count(); i++)
					lmd->pointCache[i] = os->obj->GetPoint(i);
				}
			os->obj->UpdateValidity(GEOM_CHAN_NUM, LocalValidity(t));
//			os->obj->UpdateValidity(TOPO_CHAN_NUM, LocalValidity(t)); // Have to do this to get it to evaluate
			return;
			}

//		if ((TestAFlag(A_RENDER)) && (aboutToRender))
//			{
//			lmd->lastFrame = 999999999;
//			}



		BOOL rebuildSystem = FALSE;
		
		if (lmd->pointCache.Count() != os->obj->NumPoints())
			rebuildSystem = TRUE;
	
		if ((!rebuildSystem) && (t==lmd->lastFrame))
			{
			for (int i = 0; i < lmd->pointCache.Count(); i++)
				{
				if (lmd->pointCache[i] != os->obj->GetPoint(i))
					{
					rebuildSystem = TRUE;
					i = lmd->pointCache.Count();
					}
				}
			}

		Point3 tCenter;
		p1->GetValue(ReferenceFrame,&tCenter,iv,CTRL_ABSOLUTE);
		if (tCenter != lagCenter)
			updateWeights = TRUE;


		if ( (t==ReferenceFrame) || (lmd->SpringList.Count() == 0) || (lmd->SpringList.Count() != nv) || updateWeights)
			{    

			Matrix3 tm = lmd->SelfNode->GetObjectTM(ReferenceFrame);
			p1->GetValue(ReferenceFrame,&Center,iv,CTRL_ABSOLUTE);
			lagCenter = Center;

			updateWeights = FALSE;

//build a temp edge list here 
			Tab<Point3> EdgeList;
			EdgeList.Append(1,&Center,1);

			Tab<int> EdgeIndex;
			EdgeIndex.SetCount(nv);

			for (int i = 0; i< lmd->esel.Count();i++)
				{
				if (lmd->esel[i]==1)
					{
					Point3 ep = os->obj->GetPoint(i);
					EdgeList.Append(1,&ep,1);

					}

				}


			for (i=0;i<nv;i++)
				{
//loop through all edges find closest
				float temp = Length(EdgeList[0] - os->obj->GetPoint(i));
				EdgeIndex[i] = 0;
				for (int j = 1; j < EdgeList.Count(); j++)
					{
					float etemp;
					etemp = Length(EdgeList[j] - os->obj->GetPoint(i));
					if (etemp < temp) 
						{
						temp = etemp;
						EdgeIndex[i] = j;
						}
					}
//				temp = Length(Center - os->obj->GetPoint(i));
				if (temp>Dist)
					Dist = temp;
				}
			BOOL nuke = FALSE;
			if (lmd->SpringList.Count() != nv)
				{
				lmd->SpringList.ZeroCount();
				lmd->SpringList.SetCount(nv);
				lmd->WholeFrameCache.SetCount(nv);
				nuke = TRUE;
				}

 
// fix for bug 207988 9/8/99
 			if (lmd->pointCache.Count() != nv)
				lmd->pointCache.SetCount(nv);


			for (i=0;i<nv;i++)
				{
				float temp;
				temp = Length(EdgeList[EdgeIndex[i]] - os->obj->GetPoint(i))/Dist;
				temp = (temp*temp);
				SpringClass sc;
				sc.pos = os->obj->GetPoint(i) * tm;
				sc.init_pos = sc.pos;
				sc.vel = v;
				if ( (nuke) || (!lmd->SpringList[i].modified) )
					{
					sc.InheritVel = temp;// * falloff;
					sc.modified = FALSE;
					}
				else 
					{
					if (lmd->wsel[i] == 1)
						{
						if (absoluteWeight)
							sc.InheritVel = 1.0f-weightDelta;
						else sc.InheritVel = lmd->SpringList[i].InheritVel -weightDelta;
						}
					else sc.InheritVel = lmd->SpringList[i].InheritVel;// * falloff;
					sc.modified = lmd->SpringList[i].modified;
					
					}
				lmd->SpringList[i]=sc;
				lmd->WholeFrameCache[i].pos = sc.pos;
				lmd->WholeFrameCache[i].vel = sc.vel;
// fix for bug 207988 9/8/99
				lmd->pointCache[i] = os->obj->GetPoint(i);
				}
			EdgeList.ZeroCount();
			lmd->lastFrame = ReferenceFrame;
			}
//		else ComputeCenterFalloff(lmd,t,os);



//for now we will do no cacheing and recompute the system from the ref, frame everytime
//		char ts[200];
//		wsprintf(ts,"nv %d sp %d cach %d pcache %d\n",nv,lmd->SpringList.Count(),lmd->WholeFrameCache.Count(),lmd->pointCache.Count());
//		if ((TestAFlag(A_RENDER)) && (t==ReferenceFrame))
//			MessageBox(NULL,ts,"Error",MB_OK);


		if (t>=ReferenceFrame)
			{

			int fract ;
			int frames ;
			TimeValue StartFrame;
//			if ( (t<lmd->lastFrame) )
			if ( (t<lmd->lastFrame) ||
				 ( (t==lmd->lastFrame) && (rebuildSystem) )
				)
				{

				fract = (t) % tps;
				frames =  t / tps;
				StartFrame = ReferenceFrame;
				for (int i=0;i<nv;i++)
					{
					Point3 v(0.0f,0.0f,0.0f);
					lmd->SpringList[i].pos = lmd->SpringList[i].init_pos;
					lmd->SpringList[i].vel = v;
					}
				}
			else
				{
				fract = t %tps;
				frames =  t / tps;
				StartFrame = lmd->lastFrame+tps;
				for (int i=0;i<nv;i++)
					{
					lmd->SpringList[i].pos = lmd->WholeFrameCache[i].pos;
					lmd->SpringList[i].vel = lmd->WholeFrameCache[i].vel;
					}

				}

			for (int i = StartFrame; i <= (frames*tps); i+=tps)
				{
				Matrix3 tm = lmd->SelfNode->GetObjectTM(i);

				for (int k=0;k<ff.Count();k++)
					{
					if (ff[k]) 
						{
						ff[k]->DeleteThis();
						}

					ff[k] = NULL;
					}

//compute gravity
				AddForceField();

				Point3 pp,p,force;

				ObjectState tos;
				int tnv = nv;

				nv = lmd->SpringList.Count();
				for (int j=0;j<nv;j++)
					{
					Point3 g(0.0f,0.0f,0.0f);
					g = GetForce(i, lmd->SpringList[j].pos, lmd->SpringList[j].vel, tm);
			
					if (i != t)
						{
						if (TestAFlag(A_RENDER)) 
							{
							if (j < tnv) p = lmd->pointCache[j];
							}
						else 
							{
							if (j < nv) p = os->obj->GetPoint(j);
							}
						}
					else p = os->obj->GetPoint(j);
					force = lmd->SpringList[j].pos-(p * tm);
					lmd->SpringList[j].vel += g;
					lmd->SpringList[j].vel += ((-strength*force)-(dampening*lmd->SpringList[j].vel));
					lmd->SpringList[j].pos += lmd->SpringList[j].vel;

					}

				}
//copy that last whole frame to cache
			for (i=0;i<nv;i++)
				{
				lmd->WholeFrameCache[i].pos = lmd->SpringList[i].pos;
				lmd->WholeFrameCache[i].vel = lmd->SpringList[i].vel;
				}
			lmd->lastFrame = frames*tps;
			

//compute fraction incase of motion blur, fields, or realtime playback
			if (fract != 0)
				{
				float per = (float)fract/(float)tps;

				Matrix3 tm = lmd->SelfNode->GetObjectTM(t);
				Matrix3 itm = Inverse(tm);

				for (int k=0;k<ff.Count();k++)
					{
					if (ff[k]) 
						{
						ff[k]->DeleteThis();
						}

					ff[k] = NULL;
					}

				AddForceField();





				for (int j=0;j<nv;j++)
					{
					Point3 g(0.0f,0.0f,0.0f);
					g = GetForce(i, lmd->SpringList[j].pos, lmd->SpringList[j].vel, tm);
	
					Point3 p,force;
//					force = lmd->SpringList[j].pos-(tos.obj->GetPoint(j) * tm);
					force = lmd->SpringList[j].pos-(os->obj->GetPoint(j) * tm);

					lmd->SpringList[j].vel += g;
					lmd->SpringList[j].vel += ((-strength*force)-(dampening*(lmd->SpringList[j].vel)));

					lmd->SpringList[j].pos += lmd->SpringList[j].vel * per;

					}

				}


			}

		if (lmd->pointCache.Count() != os->obj->NumPoints())
			lmd->pointCache.SetCount(os->obj->NumPoints());
		for (i = 0; i < lmd->pointCache.Count(); i++)
			lmd->pointCache[i] = os->obj->GetPoint(i);
  
		if (t>=ReferenceFrame)
			{
			LagDeformer deformer(this,lmd);

			os->obj->Deform(&deformer, TRUE);
			}
		

		if ((inPaint) && (!painting))
			{
			if (os->obj->IsSubClassOf(triObjectClassID))
				{
				lmd->isMesh = TRUE;
				lmd->isPatch = FALSE;
				}
			else if (os->obj->IsSubClassOf(patchObjectClassID))
				{
				lmd->isMesh = FALSE;
				lmd->isPatch = TRUE;
				}


			else 

				{
//ask if can convert to mesh
				if (os->obj->CanConvertToType(triObjectClassID))
					{
					lmd->isMesh = TRUE;
					lmd->isPatch = FALSE;
					}
				else
					{
					lmd->isMesh = FALSE;
					lmd->isPatch = FALSE;
					}

				}
			}
		else if (!inPaint)
			{
			}
		}


	os->obj->UpdateValidity(GEOM_CHAN_NUM, LocalValidity(t));

//	os->obj->UpdateValidity(TOPO_CHAN_NUM, LocalValidity(t)); // Have to do this to get it to evaluate

	// Update our user interface parameter
	if (TestAFlag(A_RENDER)) 
		{
		aboutToRender = FALSE;

		}
	else aboutToRender = TRUE;

	}



#define ID_CHUNK 0x1000
#define NODE_CHUNK 0x1010

IOResult LagMod::SaveLocalData(ISave *isave, LocalModData *pld)
{
LagModData *p;
IOResult	res;
ULONG		nb;

p = (LagModData*)pld;

isave->BeginChunk(ID_CHUNK);
res = isave->Write(&p->id, sizeof(int), &nb);
isave->EndChunk();

ULONG id = isave->GetRefID(p->SelfNode);

isave->BeginChunk(NODE_CHUNK);
isave->Write(&id,sizeof(ULONG),&nb);
isave->EndChunk();



TempStorageClass tsc;
int ct = p->SpringList.Count();

isave->BeginChunk(POS_WEIGHT_CHUNK);
isave->Write(&ct, sizeof(ct), &nb);
for (int i =0;i<ct;i++)
		{
		tsc.pos = p->SpringList[i].init_pos;
		tsc.IVel = p->SpringList[i].InheritVel;
		tsc.sel = p->esel[i];
		tsc.modified = p->SpringList[i].modified;
		isave->Write(&tsc, sizeof(tsc), &nb);
		}

isave->EndChunk();



return IO_OK;
}

IOResult LagMod::LoadLocalData(ILoad *iload, LocalModData **pld)

{
	IOResult	res;
	ULONG		nb;
	LagModData *p= new LagModData();
	*pld = p;

//	lmdData.Append(1,&p,1);

	int id;
//	INode *n;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case ID_CHUNK:
				iload->Read(&id,sizeof(int), &nb);
				p->id = id;
				break;
			case NODE_CHUNK:
				ULONG id;
				iload->Read(&id,sizeof(ULONG), &nb);
				if (id!=0xffffffff)
					{
					iload->RecordBackpatch(id,(void**)&p->SelfNode);
					}
				break;

			case POS_WEIGHT_CHUNK:
				{
				Point3 Zero(0.0f,0.0f,0.0f);
			
//read in data
				int ct;
				iload->Read(&ct, sizeof(ct), &nb);
				p->SpringList.SetCount(ct);
				p->psel.ZeroCount();
				p->WholeFrameCache.SetCount(ct);
				p->esel.SetCount(ct);
				p->psel.SetCount(ct);
				p->wsel.SetCount(ct);
				for (int i=0;i<ct;i++)
					{
					TempStorageClass tsc;

					iload->Read(&tsc, sizeof(tsc), &nb);

					p->SpringList[i].pos = tsc.pos;
					p->SpringList[i].init_pos = tsc.pos;
					p->SpringList[i].vel = Zero;
					p->SpringList[i].InheritVel = tsc.IVel;
					p->SpringList[i].modified = tsc.modified;
					p->esel[i] = tsc.sel;
					p->psel[i] = 0;
					p->wsel[i] = 0;
				
					}
				break;

				}


			}
		iload->CloseChunk();
		if (res!=IO_OK) return res;
		}



return IO_OK;

}



/************************* scripter access stuff *****************************************/

#define get_lagdef_mod()																\
	Modifier *mod = arg_list[0]->to_modifier();										\
	Class_ID id = mod->ClassID();													\
	if ( id != Class_ID(LAZYID) )	\
		throw RuntimeError(GetString(IDS_PW_NOT_LAGDEF_ERROR), arg_list[0]);			\
	LagMod *lmod = (LagMod*)mod;			


// Maxscript stuff


//just returns the number of vertices in the system
def_struct_primitive( flexOps_getNumberVertices,flexOps, "GetNumberVertices" );
//SelectVertices number/array/bitarray
//selects the vertices specified
def_struct_primitive (flexOps_selectVerts, flexOps,			"SelectVertices" );
//GetVertexWeightCount vertexid
//returns the inlfuence of that vertex
def_struct_primitive (flexOps_getVertexWeight,flexOps,		"GetVertexWeight" );
//SetVertexWeights VerticesID  Weights
//assigns an array of vertices to an array of weights
//VerticesID and Weights can be arrays or just numbers but if they are arrays they need to be the same length
def_struct_primitive (flexOps_setVertWeights,flexOps,		"SetVertexWeights" );

//isVertexEdge vertid
//return is a vertex is an edge edge 
def_struct_primitive (flexOps_isVertEdge,flexOps,		"isEdgeVertex" );
//SetVertexEdges vertidlist
//sets the vertices to edge vertices
def_struct_primitive (flexOps_setVertEdges,flexOps,	"SetEdgeVertices" );
//ClearVertexEdges vertidlist
//clears the vertices to edge vertices
def_struct_primitive (flexOps_clearVertEdges,flexOps,	"ClearEdgeVertices" );




Value*
flexOps_getNumberVertices_cf(Value** arg_list, int count)
{
	check_arg_count(getNumberVertices, 1, count);
	get_lagdef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !lmod->ip ) throw RuntimeError(GetString(IDS_PW_LAG_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	lmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	int ct = 0;
	if (objects != 0)
		{
		LagModData *lmd = (LagModData*)mcList[0]->localData;
		ct = lmd->SpringList.Count();
		}
	return Integer::intern(ct);	
}


Value*
flexOps_selectVerts_cf(Value** arg_list, int count)
{
	check_arg_count(selectVerts, 2, count);
	get_lagdef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !lmod->ip ) throw RuntimeError(GetString(IDS_PW_LAG_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	lmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	int ct = 0;
	Value* ival = arg_list[1];

//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		LagModData *lmd = (LagModData*)mcList[0]->localData;
		for (int i = 0; i < lmd->wsel.Count(); i++)
			lmd->wsel[i] = 0;

		int index;
		if (is_number(ival))   // single index
			{
			index = ival->to_int()-1;
			if ((index <0) || (index >= lmd->wsel.Count()) )
				throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
			else lmd->wsel[index] = 1;
			}

		else if (is_array(ival))   // array of indexes
			{
			Array* aval = (Array*)ival;
			for (int i = 0; i < aval->size; i++)
				{
				ival = aval->data[i];
				if (is_number(ival))   // single index
					{
					index = ival->to_int()-1;
					if ((index <0) || (index >= lmd->wsel.Count()))
						throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
					else lmd->wsel[index] = 1;

					}
				}

			}
		else if (is_BitArrayValue(ival))   // array of indexes
			{
			BitArrayValue *list = (BitArrayValue *) ival;
			for (int index = 0; index < list->bits.GetSize(); index++)
				{
				if (list->bits[index])
					{
					if ((index <0) || (index >= lmd->wsel.Count()))
						throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
					else lmd->wsel[index] = 1;
					}
				}

			}



		}

	lmod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
	lmod->ip->RedrawViews(lmod->ip->GetTime());


	return &ok;	
}


Value*
flexOps_getVertexWeight_cf(Value** arg_list, int count)
{
	check_arg_count(getVertexWeight, 2, count);
	get_lagdef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !lmod->ip ) throw RuntimeError(GetString(IDS_PW_LAG_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	lmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	float w = 0;
	Value* ival = arg_list[1];

	if (objects != 0)
		{
		LagModData *lmd = (LagModData*)mcList[0]->localData;
		int index;
		if (is_number(ival))   // single index
			{
			index = ival->to_int()-1;
			if ((index <0) || (index >= lmd->SpringList.Count()) )
				throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
			else w =1.0f - lmd->SpringList[index].InheritVel;
			}

		}
	return Float::intern(w);	





}


Value*
flexOps_setVertWeights_cf(Value** arg_list, int count)
{
	check_arg_count(setVertWeights, 3, count);
	get_lagdef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !lmod->ip ) throw RuntimeError(GetString(IDS_PW_LAG_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	lmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	int ct = 0;

	Value* vertsval = arg_list[1];
	Value* weightsval = arg_list[2];



//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		LagModData *lmd = (LagModData*)mcList[0]->localData;
//		if (vertID >= bmd->VertexData.Count() ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);

		if (is_number(weightsval) && is_number(vertsval))  // single index
			{
			int vertID = vertsval->to_int()-1;
			float weight = weightsval->to_float();
			weight = 1.0f-weight;
			if (weight < 0.0f ) weight = 0.0f;
			if (weight > 1.0f ) weight = 1.0f;
			if (vertID >= lmd->SpringList.Count()) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
			lmd->SpringList[vertID].InheritVel = weight;
			lmd->SpringList[vertID].modified = TRUE;
			}

		else if (is_array(weightsval) && is_array(vertsval))   // array of indexes
			{

			Array* wval = (Array*)weightsval;
			Array* vval = (Array*)vertsval;
			if (wval->size != vval->size) throw RuntimeError(GetString(IDS_PW_ERROR_COUNT), arg_list[0]);

			for (int i = 0; i < wval->size; i++)
				{
				Value *vertval = vval->data[i];
				Value *weightval = wval->data[i];
				if ( (is_number(vertval)) && (is_number(weightval)))  // single index
					{
					int vertID = vertval->to_int()-1;
					float weight = weightval->to_float();
					weight = 1.0f - weight;
					if (weight < 0.0f ) weight = 0.0f;
					if (weight > 1.0f ) weight = 1.0f;

					if (vertID >= lmd->SpringList.Count()) throw RuntimeError(GetString(IDS_PW_ERROR_COUNT), arg_list[0]);
					lmd->SpringList[vertID].InheritVel = weight;
					lmd->SpringList[vertID].modified = TRUE;

					}
				}


			}



		}
	lmod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
	lmod->ip->RedrawViews(lmod->ip->GetTime());


	return &ok;	
}


Value*
flexOps_isVertEdge_cf(Value** arg_list, int count)
{
	check_arg_count(isVertEdge, 2, count);
	get_lagdef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !lmod->ip ) throw RuntimeError(GetString(IDS_PW_LAG_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	lmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	int ct = 0;
	Value* vertsval = arg_list[1];

	if (objects != 0)
		{

		LagModData *lmd = (LagModData*)mcList[0]->localData;
		if (is_number(vertsval))  // single index
			{
			int vertID = vertsval->to_int()-1;
			ct = lmd->esel[vertID];
			}
		}
	return Integer::intern(ct);	
}


Value*
flexOps_setVertEdges_cf(Value** arg_list, int count)
{
	check_arg_count(setVertEdges, 2, count);
	get_lagdef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !lmod->ip ) throw RuntimeError(GetString(IDS_PW_LAG_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	lmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	int ct = 0;

	Value* vertsval = arg_list[1];



//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		LagModData *lmd = (LagModData*)mcList[0]->localData;
//		if (vertID >= bmd->VertexData.Count() ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);

		if (is_number(vertsval))  // single index
			{
			int vertID = vertsval->to_int()-1;
			if (vertID >= lmd->SpringList.Count()) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
			lmd->esel[vertID] = 1;
			}

		else if (is_array(vertsval))   // array of indexes
			{

			Array* vval = (Array*)vertsval;

			for (int i = 0; i < vval->size; i++)
				{
				Value *vertval = vval->data[i];
				if ( (is_number(vertval)) )  // single index
					{
					int vertID = vertval->to_int()-1;
					if (vertID >= lmd->SpringList.Count()) throw RuntimeError(GetString(IDS_PW_ERROR_COUNT), arg_list[0]);
					lmd->esel[vertID] = 1;

					}
				}


			}



		}
	lmod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
	lmod->ip->RedrawViews(lmod->ip->GetTime());


	return &ok;	
}

Value*
flexOps_clearVertEdges_cf(Value** arg_list, int count)
{
	check_arg_count(clearVertEdges, 2, count);
	get_lagdef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !lmod->ip ) throw RuntimeError(GetString(IDS_PW_LAG_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	lmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	int ct = 0;

	Value* vertsval = arg_list[1];



//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		LagModData *lmd = (LagModData*)mcList[0]->localData;
//		if (vertID >= bmd->VertexData.Count() ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);

		if (is_number(vertsval))  // single index
			{
			int vertID = vertsval->to_int()-1;
			if (vertID >= lmd->SpringList.Count()) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
			lmd->esel[vertID] = 0;
			}

		else if (is_array(vertsval))   // array of indexes
			{

			Array* vval = (Array*)vertsval;

			for (int i = 0; i < vval->size; i++)
				{
				Value *vertval = vval->data[i];
				if ( (is_number(vertval)) )  // single index
					{
					int vertID = vertval->to_int()-1;
					if (vertID >= lmd->SpringList.Count()) throw RuntimeError(GetString(IDS_PW_ERROR_COUNT), arg_list[0]);
					lmd->esel[vertID] = 0;

					}
				}


			}



		}
	lmod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
	lmod->ip->RedrawViews(lmod->ip->GetTime());


	return &ok;	
}
