/**********************************************************************
 *<
	FILE: reactor.cpp

	DESCRIPTION: A Controller plugin that reacts to changes in other controllers

	CREATED BY: Adam Felt

	HISTORY: 

 *>	Copyright (c) 1998, All Rights Reserved.
***********************************************************************/
//-----------------------------------------------------------------------------
#include "reactor.h"
#include "iparamm.h"
#include "ReactAPI.h"


//-----------------------------------------------------------------------
float Distance(Point3 p1, Point3 p2)
{
	p1 = p2-p1;
	return (float)sqrt((p1.x*p1.x)+(p1.y*p1.y)+(p1.z*p1.z));
}

//-----------------------------------------------------------------------


class ReactorDlg;

// scalar variables
class SVar {
public:
	TSTR	name;
	int		subNum;
	int		regNum;	// register number variable is assigned to
	int		refID;	// < 0 means constant
	float	influence, multiplier; 
	float strength;
	float falloff;

	//The type used here is the same as the controller type
	float	fstate;		//reaction state if it's a float
	Quat	qstate;		//reaction state if it's a quat
	Point3	pstate;		//reaction state if it's a point3
	
	//The type used here is the same as the client track
	Point3	pvalue;		//current value if it is a point3
	float	fvalue;		//current value if it's a float
	Quat	qvalue;		//current value if it's a quat

	SVar& operator=(const SVar& from){
		name = ((SVar)from).name;
		influence = ((SVar)from).influence;
		strength = ((SVar)from).strength;
		falloff = ((SVar)from).falloff;
		fstate = ((SVar)from).fstate;
		qstate = ((SVar)from).qstate;
		pstate = ((SVar)from).pstate;
		pvalue = ((SVar)from).pvalue;
		fvalue = ((SVar)from).fvalue;
		qvalue = ((SVar)from).qvalue;
		return *this;
	}



};
	
MakeTab(SVar);


class VarRef {
public:
	INode* client;
	int	refCt;
	int subnum;

	VarRef()	{ client = NULL; refCt = 0; }
	VarRef(INode* c)	{ client = c; refCt = 1; }
	
	VarRef& operator=(const VarRef& from){
		client = ((VarRef)from).client;
		refCt = ((VarRef)from).refCt;
		subnum = ((VarRef)from).subnum;
		return *this;
	}

};

class MyEnumProc : public DependentEnumProc 
	{
      public :
      virtual int proc(ReferenceMaker *rmaker); 
	  INodeTab nodes;
	};


int MyEnumProc::proc(ReferenceMaker *rmaker) 
{ 
		nodes.Append(1, (INode**)&rmaker);
		return 0;
}

class Reactor : public IReactor {
	public:

		int			type, selected, count, rtype;
		BOOL		editing;  // editing the reaction state
		BOOL		isBiped;
		SVarTab		reaction;
		Interval	ivalid;
		Interval	range;
		HWND		hParams;
		NameMaker*	nmaker;
	
		VarRef vrefs;
		Point3 curpval;
		float curfval;
		Quat curqval;
		BOOL blockGetNodeName; // RB 3/23/99: See imp of getNodeName()

		ReactorShortcutCB<Reactor >	*reactorShortcutCB;		// Shortcuts handler 		

		virtual int Elems()=0;
		static IObjParam *ip;
		static ReactorDlg *dlg;	
		
		Reactor(int t, Reactor &ctrl);
		Reactor(int t, BOOL loading);
		Reactor& operator=(const Reactor& from);
		~Reactor();

		BOOL	assignReactObj(INode* client, int subnum);
//		void	assignTo(TrackViewPick res);
		void	reactTo(Animatable* anim, TimeValue t = GetCOREInterface()->GetTime());
		void	updReactionCt(int val);
		BOOL	CreateReaction(TCHAR *buf=NULL, TimeValue t = GetCOREInterface()->GetTime());
		BOOL	DeleteReaction(int i=-1);
		int		getVarCount() { return reaction.Count(); }
		void	deleteAllVars();
		int		getSelected() {return selected;}
		void	setSelected(int i) {selected = i; NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE); return;}
		void	setrType(int i){ rtype = i; }
		int 	getrType(){ return rtype; }
		int 	getType(){ return type; }
		TCHAR*	getVarName(int i);
		void	setVarName(int i, TSTR name);
		void*	getReactionValue(int i);
		BOOL	setReactionValue(int i=-1, void *val=NULL, TimeValue t=NULL);
		float	getCurFloatValue(TimeValue t);
		Point3	getCurPoint3Value(TimeValue t);
		ScaleValue	getCurScaleValue(TimeValue t);
		Quat	getCurQuatValue(TimeValue t);
		BOOL	setInfluence(int num, float inf);
		float	getInfluence(int num);
		void	setMinInfluence(int x=-1);
		void	setMaxInfluence(int x=-1);
		BOOL	setStrength(int num, float inf);
		float	getStrength(int num);
		BOOL	setFalloff(int num, float inf);
		float	getFalloff(int num);
		BOOL	setEditing(BOOL edit);
		void*	getState(int num);
		BOOL	setState(int num, void *val=NULL, TimeValue t=NULL);
		void	getNodeName(ReferenceTarget *client, TSTR &name);

		void	Update(TimeValue t);
		void	ComputeMultiplier(TimeValue t);
		void	GetAbsoluteControlValue(INode *node,TimeValue t,void *pt,Interval &iv);
		BOOL	ChangeParents(TimeValue t,const Matrix3& oldP,const Matrix3& newP,const Matrix3& tm);
		void	isABiped(BOOL bip) { isBiped = bip; }

		// Animatable methods		
		void DeleteThis() {delete this;}		
		int IsKeyable() {return 0;}		
		BOOL IsAnimated() {if (reaction.Count() > 1) return true; else return false;}
		Interval GetTimeRange(DWORD flags) { return range; } 
		void EditTimeRange(Interval range,DWORD flags);
		void MapKeys(TimeMap *map,DWORD flags);

		void HoldTrack();
		void HoldAll();
		void HoldParams();
		void HoldRange();

		int NumSubs();
		BOOL AssignController(Animatable *control,int subAnim) {return false;}
		Animatable* SubAnim(int i){return NULL;}
		TSTR SubAnimName(int i){ return "";}

		int NumRefs();
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		
		void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev );
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next );

		void EditTrackParams(
			TimeValue t,
			ParamDimensionBase *dim,
			TCHAR *pname,
			HWND hParent,
			IObjParam *ip,
			DWORD flags);
		int TrackParamsType() {return TRACKPARAMS_WHOLE;}

		// Reference methods
		RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage);
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);

		// Control methods				
		void Copy(Control *from);
		BOOL IsLeaf() {return TRUE;}

		//These three default implementation are shared by Position, Point3 and Scale controllers
		void SetValue(TimeValue t, void *val, int commit, GetSetMethod method);
		void CommitValue(TimeValue t);
		void RestoreValue(TimeValue t);		
};

//---------------------------------------------------------------------------


class Point3Reactor : public Reactor {
	public:
		int Elems() {return 3;}

		Point3Reactor(Point3Reactor &ctrl) : Reactor(REACTORP3, ctrl) {}
		Point3Reactor(BOOL loading) : Reactor(REACTORP3, loading) {}
		~Point3Reactor() {}

		Class_ID ClassID() { return REACTORP3_CLASS_ID; }  
		SClass_ID SuperClassID() { return CTRL_POINT3_CLASS_ID; } 
		void GetClassName(TSTR& s) {s = GetString(IDS_AF_REACTORP3);}

		// Control methods
		RefTargetHandle Clone(RemapDir& remap);
		void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method);
	};


class Point3ReactorClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { return new Point3Reactor(loading); }
	const TCHAR *	ClassName() { return GetString(IDS_AF_REACTORP3); }
	SClass_ID		SuperClassID() { return CTRL_POINT3_CLASS_ID; }
	Class_ID		ClassID() { return REACTORP3_CLASS_ID; }
	const TCHAR* 	Category() { return _T("");  }
};
static Point3ReactorClassDesc point3ReactorCD;
ClassDesc* GetPoint3ReactorDesc() {return &point3ReactorCD;}

//-----------------------------------------------------------------------------

class PositionReactor : public Reactor {
	public:
		int Elems() {return 3;}

		PositionReactor(PositionReactor &ctrl) : Reactor(REACTORPOS, ctrl) {}
		PositionReactor(BOOL loading) : Reactor(REACTORPOS, loading) {}
		~PositionReactor() {}

		Class_ID ClassID() { return REACTORPOS_CLASS_ID; }  
		SClass_ID SuperClassID() { return CTRL_POSITION_CLASS_ID; } 
		void GetClassName(TSTR& s) {s = GetString(IDS_AF_REACTORPOS);}

		// Control methods
		RefTargetHandle Clone(RemapDir& remap);
		void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method); 
	};


class PositionReactorClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { return new PositionReactor(loading); }
	const TCHAR *	ClassName() { return GetString(IDS_AF_REACTORPOS); }
	SClass_ID		SuperClassID() { return CTRL_POSITION_CLASS_ID; }
	Class_ID		ClassID() { return REACTORPOS_CLASS_ID; }
	const TCHAR* 	Category() { return _T("");  }

	};
static PositionReactorClassDesc positionReactorCD;
ClassDesc* GetPositionReactorDesc() {return &positionReactorCD;}

//-----------------------------------------------------------------------------


class ScaleReactor : public Reactor {
	public:
		int Elems() {return 3;}

		ScaleReactor(ScaleReactor &ctrl) : Reactor(REACTORSCALE, ctrl) {}
		ScaleReactor(BOOL loading) : Reactor(REACTORSCALE, loading) {}
		~ScaleReactor() {}

		Class_ID ClassID() { return REACTORSCALE_CLASS_ID; }  
		SClass_ID SuperClassID() { return CTRL_SCALE_CLASS_ID; } 
		void GetClassName(TSTR& s) {s = GetString(IDS_AF_REACTORSCALE);}

		// Control methods
		RefTargetHandle Clone(RemapDir& remap);
		void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method);
		void SetValue(TimeValue t, void *val, int commit, GetSetMethod method);		
	};


class ScaleReactorClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { return new ScaleReactor(loading); }
	const TCHAR *	ClassName() { return GetString(IDS_AF_REACTORSCALE); }
	SClass_ID		SuperClassID() { return CTRL_SCALE_CLASS_ID; }
	Class_ID		ClassID() { return REACTORSCALE_CLASS_ID;}
	const TCHAR* 	Category() { return _T("");  }
	};

static ScaleReactorClassDesc scaleReactorCD;
ClassDesc* GetScaleReactorDesc() {return &scaleReactorCD;}

//-------------------------------------------------------------------

class RotationReactor : public Reactor {
	public:
		int Elems() {return 3;}

		RotationReactor(RotationReactor &ctrl) : Reactor(REACTORROT, ctrl) {}
		RotationReactor(BOOL loading) : Reactor(REACTORROT, loading) {}
		~RotationReactor() {}

		Class_ID ClassID() { return REACTORROT_CLASS_ID; }  
		SClass_ID SuperClassID() { return CTRL_ROTATION_CLASS_ID; } 
		void GetClassName(TSTR& s) {s = GetString(IDS_AF_REACTORROT);}

		// Control methods
		RefTargetHandle Clone(RemapDir& remap);
		void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method);
		void SetValue(TimeValue t, void *val, int commit, GetSetMethod method);		
		void CommitValue(TimeValue t);
		void RestoreValue(TimeValue t);
	};


class RotationReactorClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { return new RotationReactor(loading); }
	const TCHAR *	ClassName() { return GetString(IDS_AF_REACTORROT); }
	SClass_ID		SuperClassID() { return CTRL_ROTATION_CLASS_ID; }
	Class_ID		ClassID() { return REACTORROT_CLASS_ID; }
	const TCHAR* 	Category() { return _T("");  }
	};
static RotationReactorClassDesc rotationReactorCD;
ClassDesc* GetRotationReactorDesc() {return &rotationReactorCD;}

//-----------------------------------------------------------------------------


class FloatReactor : public Reactor {
	public:
		int Elems() {return 1;}
		
		FloatReactor(FloatReactor &ctrl) : Reactor(REACTORFLOAT, ctrl) {}
		FloatReactor(BOOL loading) : Reactor(REACTORFLOAT, loading) {}
		~FloatReactor() {}

		Class_ID ClassID() { return REACTORFLOAT_CLASS_ID; }  
		SClass_ID SuperClassID() { return CTRL_FLOAT_CLASS_ID; } 
		void GetClassName(TSTR& s) {s = GetString(IDS_AF_REACTORFLOAT);}

		// Control methods
		RefTargetHandle Clone(RemapDir& remap);
		void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method);
		void SetValue(TimeValue t, void *val, int commit, GetSetMethod method){}		
		void CommitValue(TimeValue t){}
		void RestoreValue(TimeValue t){}		
	};


class FloatReactorClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { return new FloatReactor(loading); }
	const TCHAR *	ClassName() { return GetString(IDS_AF_REACTORFLOAT); }
	SClass_ID		SuperClassID() { return CTRL_FLOAT_CLASS_ID; }
	Class_ID		ClassID() { return REACTORFLOAT_CLASS_ID; }
	const TCHAR* 	Category() { return _T("");  }
	//You only need to add the shortcut stuff to one Class Desc
	int             NumShortcutTables() { return 1; }
	ShortcutTable*  GetShortcutTable(int i) { return GetShortcuts(); }

	};
static FloatReactorClassDesc floatReactorCD;
ClassDesc* GetFloatReactorDesc() {return &floatReactorCD;}

//-----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////
//************************************************************

class ReactorDlg : public ReferenceMaker, public TimeChangeCallback {
	public:
		Reactor *cont;	
		ParamDimensionBase *dim;
		IObjParam *ip;
		HWND hWnd;
		BOOL valid;
		int elems;  //This is reserved in case I have a variable # of spinners
		ISpinnerControl *iFloatState;		
		ISpinnerControl *iInfluence;
		ISpinnerControl *iStrength;
		ISpinnerControl *iFalloff[1];  //Variable number of spinners (reserved)
		ICustButton *iCreateBut;
		ICustButton *iDeleteBut;
		ICustButton *iSetBut;
		ICustButton *iEditBut;
		ICustEdit	*iNameEdit;
		ICustEdit	*iValueStatus;

		ReactorShortcutCB<Reactor >	*reactorShortcutCB;		// Shortcuts handler 		
		
		ReactorDlg(
			Reactor *cont,
			ParamDimensionBase *dim,
			TCHAR *pname,
			IObjParam *ip,
			HWND hParent);
		~ReactorDlg();

		Class_ID ClassID() {return Class_ID(REACTORDLG_CLASS_ID,0x67053d10);}
		SClass_ID SuperClassID() {return REF_MAKER_CLASS_ID;}

		void MaybeCloseWindow();
		void TimeChanged(TimeValue t) {Invalidate();}
		void Invalidate();
		void Update();
		void UpdateNodeName();
		void UpdateVarList();
		void UpdateReactionValue();
		void SetupUI(HWND hWnd);
		void Change(BOOL redraw=FALSE);
		void WMCommand(int id, int notify, HWND hCtrl);
		void SpinnerChange(int id,BOOL drag);
		void SpinnerStart(int id);
		void SpinnerEnd(int id,BOOL cancel);

		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	         PartID& partID,  RefMessage message);
		int NumRefs() {return 1;}
		RefTargetHandle GetReference(int i) {return cont;}
		void SetReference(int i, RefTargetHandle rtarg) {cont=(Reactor*)rtarg;}
	};



//-----------------------------------------------------------------------

class FullRestore: public RestoreObj {
	public:		
		Control *sav;
		Control *cur;
		Control *redo; 
		FullRestore() { sav = cur = redo =  NULL; }
		FullRestore(Control *cont) {
			cur = cont;
			theHold.Suspend();
			sav = (Control*)cont->Clone();
			theHold.Resume();
			redo = NULL;
			}
		~FullRestore() {
			}		
		
		void Restore(int isUndo) {
			assert(cur); assert(sav);
			if (isUndo) {
				theHold.Suspend();
				redo = (Control *)cur->Clone();
				theHold.Resume();
				}
			cur->Copy(sav);
			}
		void Redo() {
			assert(cur); 
			if (redo) 
				cur->Copy(redo);
			}
		void EndHold() {}
		TSTR Description() { return TSTR(_T("FullReactorRestore")); }
	};


void Reactor::HoldAll()
	{
	if (theHold.Holding()) { 	
		theHold.Put(new FullRestore(this));
		}
	}


// A restore object to save the influence, strength, and falloff.
class SpinnerRestore : public RestoreObj {
	public:		
		Reactor *cont;
		Tab<SVar> ureaction, rreaction;
		float uselected, rselected;
		SpinnerRestore(Reactor *c) {
			cont=c;
			ureaction = cont->reaction;
			uselected = cont->selected;
		}
		void Restore(int isUndo) {
			// if we're undoing, save a redo state
			if (isUndo) {
				rreaction = cont->reaction;
				rselected = cont->selected;
			}
			cont->reaction = ureaction;
			cont->selected = uselected;
			cont->count = cont->reaction.Count();
			cont->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);

		}
		void Redo() {
			cont->reaction = rreaction;
			cont->selected = rselected;
			cont->count = cont->reaction.Count();
			cont->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
		}
		void EndHold()
		{
		}
		int Size()
		{
			return sizeof(cont->reaction) + sizeof(float);
		}

};


void Reactor::HoldParams()
{
	if (theHold.Holding()) {
		theHold.Put(new SpinnerRestore(this));
	}
}


class StateRestore : public RestoreObj {
	public:
		Reactor *cont;
		Point3 ucurpval, rcurpval;
		float ucurfval, rcurfval;
		Quat ucurqval, rcurqval;
		Tab<SVar> ureaction, rreaction;

		StateRestore(Reactor *c) 
			{
			cont = c;
			ucurpval = cont->curpval;
			ucurqval = cont->curqval;
			ucurfval = cont->curfval;
			ureaction = cont->reaction;
			}   		
		void Restore(int isUndo) 
			{
			rcurpval = cont->curpval;
			rcurqval = cont->curqval;
			rcurfval = cont->curfval;
			rreaction = cont->reaction;
			cont->curpval = ucurpval;
			cont->curqval = ucurqval;
			cont->curfval = ucurfval;
			cont->reaction = ureaction;
			cont->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
			}
		void Redo()
			{
			cont->curpval = rcurpval;
			cont->curqval = rcurqval;
			cont->curfval = rcurfval;
			cont->reaction = rreaction;
			cont->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
			}		
		void EndHold() 
			{ 
			cont->ClearAFlag(A_HELD);
			}
		TSTR Description() { return TSTR(_T("Reactor State")); }
};

void Reactor::HoldTrack()
	{
	if (theHold.Holding()&&!TestAFlag(A_HELD)) {		
		theHold.Put(new StateRestore(this));
		SetAFlag(A_HELD);
		}
	}

class RangeRestore : public RestoreObj {
	public:
		Reactor *cont;
		Interval ur, rr;
		RangeRestore(Reactor *c) 
			{
			cont = c;
			ur   = cont->range;
			}   		
		void Restore(int isUndo) 
			{
			rr = cont->range;
			cont->range = ur;
			cont->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
			}
		void Redo()
			{
			cont->range = rr;
			cont->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
			}		
		void EndHold() 
			{ 
			cont->ClearAFlag(A_HELD);
			}
		TSTR Description() { return TSTR(_T("Reactor control range")); }
	};


void Reactor::HoldRange()
{
	if (theHold.Holding() && !TestAFlag(A_HELD)) {
		SetAFlag(A_HELD);
		theHold.Put(new RangeRestore(this));
		}
}
		

IObjParam		*Reactor::ip = NULL;
ReactorDlg		*Reactor::dlg = NULL;

Reactor::Reactor(int t, Reactor &ctrl)
{
	type = t;
	DeleteAllRefsFromMe();
	MakeRefByID(FOREVER,0,vrefs.client);

	ip = ctrl.ip;
	hParams = ctrl.hParams;
	range = ctrl.range;
	ivalid = ctrl.ivalid;
	selected = ctrl.selected;
	count = ctrl.count;
	editing = ctrl.editing;
	isBiped = ctrl.isBiped;
	nmaker = ctrl.nmaker;
	curpval = ctrl.curpval;
	curfval = ctrl.curfval;
	curqval = ctrl.curqval;
	rtype = ctrl.rtype;
	type = ctrl.type;
	blockGetNodeName = FALSE;

}

Reactor::Reactor(int t, BOOL loading) 
{
	type = t;
	range.Set(GetAnimStart(), GetAnimEnd());
	count = 0;
	selected = 0;
	editing = FALSE;
	isBiped = FALSE;
	hParams = NULL;
	nmaker = NULL;
	curpval = Point3(1.0f,1.0f,1.0f);
	curfval = 0.0f;
	curqval.Identity();
	ivalid.SetEmpty();
	blockGetNodeName = FALSE;

}

Reactor::~Reactor()
{
	deleteAllVars();
	if(nmaker) delete nmaker;
	if (hParams)
	{
		DestroyWindow(hParams);
	}
	DeleteAllRefsFromMe();
}

void Reactor::deleteAllVars()
{
	reaction.SetCount(0);
	count = 0;
	selected = 0;
}

Reactor& Reactor::operator=(const Reactor& from)
	{
		
		type = from.type;
		rtype = from.rtype;		
		assignReactObj(from.vrefs.client, from.vrefs.subnum);
		reaction = from.reaction;

		count = from.count;	
		selected = from.selected;
		editing = from.editing;
		isBiped = from.isBiped;
		
		curfval = from.curfval;
		curpval = from.curpval;
		curqval = from.curqval;

		ivalid = from.ivalid;
		range = from.range;

		nmaker = from.nmaker;
		NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);

	return *this;
	}

int Reactor::NumSubs() 
	{
	return 0;
	}

void Reactor::EditTimeRange(Interval range,DWORD flags)
{
	if(!(flags&EDITRANGE_LINKTOKEYS)){
		HoldRange();
		this->range = range;
		NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	}
}

void Reactor::MapKeys(TimeMap *map,DWORD flags)
	{
	if (flags&TRACK_MAPRANGE) {
		HoldRange();
		TimeValue t0 = map->map(range.Start());
		TimeValue t1 = map->map(range.End());
		range.Set(t0,t1);
		NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
		}
	}

int Reactor::NumRefs() 
{
		return 1;
}

RefTargetHandle Reactor::GetReference(int i) 
{
	return vrefs.client;

}

void Reactor::SetReference(int i, RefTargetHandle rtarg) 
{
	vrefs.client = (INode*)rtarg;
}


RefResult Reactor::NotifyRefChanged(
		Interval iv, 
		RefTargetHandle hTarg, 
		PartID& partID, 
		RefMessage msg) 
{
	switch (msg) {
		case REFMSG_CHANGE:
			ivalid.SetEmpty();

			break;
	case REFMSG_GET_NODE_NAME:
		// RB 3/23/99: See comment at imp of getNodeName().
		if (blockGetNodeName) return REF_STOP;
		break;
		case REFMSG_TARGET_DELETED:
			if (hTarg==vrefs.client && vrefs.subnum < 0 )  //If it's a special case reference delete everything
			{
				vrefs.client = NULL;
				HoldParams();
				count = 0;
				reaction.ZeroCount();
				selected = -1;
			}
			break;
		case REFMSG_GET_CONTROL_DIM: {
//			ParamDimension **dim = (ParamDimension **)partID;
//			assert(dim);
//			*dim = stdWorldDim;
			}
		}
	return REF_SUCCEED;
}
 
void Reactor::Copy(Control *from)
{
	Point3 pointval;
	float floatval, f;
	Quat quatval;
	ScaleValue sv;
	
	if (from->ClassID() == ClassID()) (*this) = *((Reactor*)from);
	else {
		switch (type)
		{
			case REACTORPOS:
			case REACTORP3:
				from->GetValue(GetCOREInterface()->GetTime(), &pointval, ivalid);
				curpval = pointval;
				break;
			case REACTORROT:
				from->GetValue(GetCOREInterface()->GetTime(), &quatval, ivalid);
				curqval = quatval;
				break;
			case REACTORFLOAT:
				from->GetValue(GetCOREInterface()->GetTime(), &floatval, ivalid);
				f = floatval;
				curfval = f;
				break;
			case REACTORSCALE:
				from->GetValue(GetCOREInterface()->GetTime(), &sv, ivalid);
				curpval = sv.s;
				break;
			default: break;
		}
	}
}



#define REACTOR_VAR_RQUAT		0x5000
#define REACTOR_VAR_RVECTOR		0x5001
#define REACTOR_RTYPE_CHUNK		0x5002
#define REACTOR_VAR_STRENGTH	0x5003
#define REACTOR_VAR_FALLOFF		0x5004
#define REACTOR_ISBIPED_CHUNK	0x5005
#define REACTOR_RANGE_CHUNK		0x6001
#define REACTOR_VREFS_REFCT		0x6002
#define REACTOR_VREFS_SUBNUM	0x6003
#define REACTOR_SVAR_TABSIZE	0x6004
#define REACTOR_VVAR_TABSIZE	0x6005
#define REACTOR_VAR_NAME		0x6006
#define REACTOR_VAR_VAL			0x6007
#define REACTOR_VAR_INF			0x6008
#define REACTOR_VAR_MULT		0x6009
#define REACTOR_VAR_FNUM		0x7000
#define REACTOR_VAR_POS			0x7300
#define REACTOR_VAR_QVAL		0x7600
#define REACTOR_SVAR_ENTRY0		0x8000
#define REACTOR_SVAR_ENTRYN		0x8fff
#define REACTOR_VVAR_ENTRY0		0x9000
#define REACTOR_VVAR_ENTRYN		0x9fff


IOResult Reactor::Save(ISave *isave)
{		
	ULONG 	nb;
	int		i, ct, intVar;
 
	isave->BeginChunk(REACTOR_RTYPE_CHUNK);
	isave->Write(&rtype, sizeof(int), &nb);
 	isave->EndChunk();

	isave->BeginChunk(REACTOR_RANGE_CHUNK);
	isave->Write(&range, sizeof(range), &nb);
 	isave->EndChunk();

	isave->BeginChunk(REACTOR_ISBIPED_CHUNK);
	isave->Write(&isBiped, sizeof(BOOL), &nb);
 	isave->EndChunk();

	isave->BeginChunk(REACTOR_VREFS_REFCT);
	isave->Write(&vrefs.refCt, sizeof(int), &nb);
	isave->EndChunk();

	isave->BeginChunk(REACTOR_VREFS_SUBNUM);
	isave->Write(&vrefs.subnum, sizeof(int), &nb);
	isave->EndChunk();

	isave->BeginChunk(REACTOR_SVAR_TABSIZE);
	intVar = count;
	isave->Write(&intVar, sizeof(intVar), &nb);
 	isave->EndChunk();

	ct = count;
	for(i = 0; i < ct; i++) {
	 	isave->BeginChunk(REACTOR_SVAR_ENTRY0+i);
	 	 isave->BeginChunk(REACTOR_VAR_POS);
		 isave->Write(&reaction[i].pvalue, sizeof(Point3), &nb);
 		 isave->EndChunk();
	 	 isave->BeginChunk(REACTOR_VAR_NAME);
		 isave->WriteCString(reaction[i].name);
 		 isave->EndChunk();
	 	 isave->BeginChunk(REACTOR_VAR_VAL);
		 isave->Write(&reaction[i].fstate, sizeof(float), &nb);
 		 isave->EndChunk();
	 	 isave->BeginChunk(REACTOR_VAR_RQUAT);
		 isave->Write(&reaction[i].qstate, sizeof(Quat), &nb);
 		 isave->EndChunk();
	 	 isave->BeginChunk(REACTOR_VAR_RVECTOR);
		 isave->Write(&reaction[i].pstate, sizeof(Point3), &nb);
 		 isave->EndChunk();
	 	 isave->BeginChunk(REACTOR_VAR_INF);
		 isave->Write(&reaction[i].influence, sizeof(float), &nb);
 		 isave->EndChunk();
	 	 isave->BeginChunk(REACTOR_VAR_STRENGTH);
		 isave->Write(&reaction[i].strength, sizeof(float), &nb);
 		 isave->EndChunk();
	 	 isave->BeginChunk(REACTOR_VAR_FALLOFF);
		 isave->Write(&reaction[i].falloff, sizeof(float), &nb);
 		 isave->EndChunk();
	 	 isave->BeginChunk(REACTOR_VAR_MULT);
		 isave->Write(&reaction[i].multiplier, sizeof(float), &nb);
 		 isave->EndChunk();
	 	 isave->BeginChunk(REACTOR_VAR_FNUM);
		 isave->Write(&reaction[i].fvalue, sizeof(float), &nb);
 		 isave->EndChunk();
	 	 isave->BeginChunk(REACTOR_VAR_QVAL);
		 isave->Write(&reaction[i].qvalue, sizeof(Quat), &nb);
 		 isave->EndChunk();
	 	isave->EndChunk();
	}
	return IO_OK;
}

IOResult Reactor::Load(ILoad *iload)
	{
	ULONG 	nb;
	TCHAR	*cp;
	int		id, i, varIndex, intVar;
	IOResult res;
	VarRef	dummyVarRef;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch (id = iload->CurChunkID()) {
	
		case REACTOR_RTYPE_CHUNK:
			iload->Read(&rtype, sizeof(int), &nb);
			break;
		case REACTOR_RANGE_CHUNK:
			iload->Read(&range, sizeof(range), &nb);
			break;
		case REACTOR_ISBIPED_CHUNK:
			iload->Read(&isBiped, sizeof(BOOL), &nb);
			break;
		case REACTOR_VREFS_REFCT:
			iload->Read(&vrefs.refCt, sizeof(int), &nb);
			break;
		case REACTOR_VREFS_SUBNUM:
			iload->Read(&vrefs.subnum, sizeof(int), &nb);
			break;
		case REACTOR_SVAR_TABSIZE:
			iload->Read(&intVar, sizeof(intVar), &nb);
			reaction.SetCount(intVar);
			updReactionCt(intVar);
			for(i = 0; i < intVar; i++)
				memset(&reaction[i], 0, sizeof(SVar));
			break;
		}	
		if(id >= REACTOR_SVAR_ENTRY0 && id <= REACTOR_SVAR_ENTRYN) {
			varIndex = id - REACTOR_SVAR_ENTRY0;
			assert(varIndex < count);
			while (IO_OK == iload->OpenChunk()) {
				switch (iload->CurChunkID()) {
				case REACTOR_VAR_NAME:
					iload->ReadCStringChunk(&cp);
					reaction[varIndex].name = cp;
					break;
				case REACTOR_VAR_VAL:
					iload->Read(&reaction[varIndex].fstate, sizeof(float), &nb);
					break;
				case REACTOR_VAR_RQUAT:
					iload->Read(&reaction[varIndex].qstate, sizeof(Quat), &nb);
					break;
				case REACTOR_VAR_RVECTOR:
					iload->Read(&reaction[varIndex].pstate, sizeof(Point3), &nb);
					break;
				case REACTOR_VAR_INF:
					iload->Read(&reaction[varIndex].influence, sizeof(float), &nb);
					break;
				case REACTOR_VAR_STRENGTH:
					iload->Read(&reaction[varIndex].strength, sizeof(float), &nb);
					break;
				case REACTOR_VAR_FALLOFF:
					iload->Read(&reaction[varIndex].falloff, sizeof(float), &nb);
					break;
				case REACTOR_VAR_MULT:
					iload->Read(&reaction[varIndex].multiplier, sizeof(float), &nb);
					break;
				case REACTOR_VAR_FNUM:
					iload->Read(&reaction[varIndex].fvalue, sizeof(float), &nb);
					break;
				case REACTOR_VAR_POS:
					iload->Read(&reaction[varIndex].pvalue, sizeof(Point3), &nb);
					break;
				case REACTOR_VAR_QVAL:
					iload->Read(&reaction[varIndex].qvalue, sizeof(Quat), &nb);
					break;
				}	
				iload->CloseChunk();
			}
		}
		iload->CloseChunk();
	}
	return IO_OK;
}

// RB 3/23/99: To solve 75139 (the problem where a node name is found for variables that 
// are not associated with nodes such as globabl tracks) we need to block the propogation
// of this message through our reference to the client of the variable we're referencing.
// In the expression controller's imp of NotifyRefChanged() we're going to block the get
// node name message if the blockGetNodeName variable is TRUE.
void Reactor::getNodeName(ReferenceTarget *client, TSTR &name)
{
	blockGetNodeName = TRUE;
	if (client) client->NotifyDependents(FOREVER,(PartID)&name,REFMSG_GET_NODE_NAME);
	blockGetNodeName = FALSE;
}


void Reactor::ComputeMultiplier(TimeValue t)
{

	float m, mtemp, normval, total;
	int i, j;
	Tab<float> ftab, inftab;
	Point3 axis;

	ftab.ZeroCount();
	inftab.ZeroCount();
	total = 0.0f;
	if (!editing)
	{
		float mult;

		// Limit to in range
		if (t<range.Start()) t = range.Start();
		if (t>range.End()) t = range.End();	

		//Make sure there is always an influentual reaction
		//If not create a temp influence value that is large enough
		//First sum up all multiplier values
		for(i=0;i<count;i++){
			inftab.Append(1, &(reaction[i].influence)); 
			switch (rtype)
			{
				case FLOAT_VAR:
					mult = 1.0f-((float)fabs(getCurFloatValue(t)-reaction[i].fvalue)/(reaction[i].influence));
					break;
				case VECTOR_VAR: 
					mult = 1.0f-(Distance(reaction[i].pvalue, getCurPoint3Value(t))/(reaction[i].influence));
					break;
				case SCALE_VAR:
					mult = 1.0f-(Distance(reaction[i].pvalue, (getCurScaleValue(t)).s)/(reaction[i].influence));
					break;
				case QUAT_VAR: 
					mult = 1.0f-(QangAxis(reaction[i].qvalue, getCurQuatValue(t), axis)/reaction[i].influence);
					break;

				default: assert(0);
			}
			if (mult<0) mult = 0.0f;
			total += mult;
		}
		//Check to see if any are influencial, total > 0 if any influence
		if(total <= 0.0f) {
			//find the closest reaction
			int which;  
			float closest, closesttemp;
			closest = 10000000.0f;
			which = 0;
			for(i=0;i<count;i++)
			{
				switch (rtype)
				{
					case FLOAT_VAR:
						closesttemp = ((float)fabs(getCurFloatValue(t)-reaction[i].fvalue)<closest ? (float)fabs(getCurFloatValue(t)-reaction[i].fvalue) : closest);
						break;
					case VECTOR_VAR: 
						closesttemp = (Distance(reaction[i].pvalue, getCurPoint3Value(t))<closest ? Distance(reaction[i].pvalue, getCurPoint3Value(t)) : closest);
						break;
					case SCALE_VAR: 
						closesttemp = (Distance(reaction[i].pvalue, (getCurScaleValue(t)).s)<closest ? Distance(reaction[i].pvalue, (getCurScaleValue(t)).s) : closest);
						break;
					case QUAT_VAR: 
						closesttemp = (QangAxis(reaction[i].qvalue, getCurQuatValue(t), axis)<closest ? QangAxis(reaction[i].qvalue, getCurQuatValue(t), axis) : closest);
						break;
					default: closesttemp = 10000000.0f;
				}
				if (closesttemp < closest) 
				{
					which = i;	
					closest = closesttemp;
				}
			}
			if(count&&closest) 
			{
				inftab[which] = closest + 1.0f;  //make the influence a little more than the closest reaction
			}
		}


		//Get the initial multiplier by determining it's influence
		for(i=0;i<count;i++)
		{
			switch (rtype)
			{
				case FLOAT_VAR:
					m = 1.0f-((float)fabs(getCurFloatValue(t)-reaction[i].fvalue)/inftab[i]);
					break;
				case VECTOR_VAR: 
					m = 1.0f-(Distance(reaction[i].pvalue, getCurPoint3Value(t))/inftab[i]);
					break;
				case SCALE_VAR: 
					m = 1.0f-(Distance(reaction[i].pvalue, (getCurScaleValue(t)).s)/inftab[i]);
					break;
				case QUAT_VAR: 
					m = 1.0f-(QangAxis(reaction[i].qvalue, getCurQuatValue(t), axis)/inftab[i]);
					break;

				default: assert(0);
			}
			if(m<0) m=0;
			reaction[i].multiplier = m;
		}
		
		for(i=0;i<count;i++)
		{
			//add the strength
			reaction[i].multiplier  *= reaction[i].strength;
		}

		//Make an adjustment so that when a value is reached 
		//the state is also reached reguardless of the other influentual reactions 
		for(i=0;i<count;i++)
		{
			mtemp = 1.0f;
			for(j=0;j<count;j++)
			{
				BOOL is_same = false;
				switch (rtype)
				{
					case SCALE_VAR:
					case VECTOR_VAR:
						if((*((Point3*)getReactionValue(j))) == (*((Point3*)getReactionValue(i)))) 
							is_same = true;
						break;
					case QUAT_VAR:
						if((*((Quat*)getReactionValue(j))) == (*((Quat*)getReactionValue(i)))) 
							is_same = true;
						break;
					case FLOAT_VAR:
						if((*((float*)getReactionValue(j))) == (*((float*)getReactionValue(i)))) 
							is_same = true;
						break;
					default : is_same = false;
				}
				if (is_same ) mtemp *= reaction[j].multiplier; 
					else mtemp *= (1.0f - reaction[j].multiplier);
			}
			if(mtemp<0) mtemp = 0.0f;
			ftab.Append(1, &mtemp);
		}

		//update the Reaction multipliers
		for(i=0;i<count;i++)
		{
			reaction[i].multiplier = ftab[i];
			//compute the falloff
			reaction[i].multiplier = (float)pow(reaction[i].multiplier, (1/reaction[i].falloff));
		}
		//make sure they always add up to 1.0
		int valcount = 0;
		total = 0.0f;
		for(i=0;i<count;i++)
			total +=reaction[i].multiplier;
		if (!total) total = 1.0f; 
		normval = 1.0f/total;
		for(i=0;i<count;i++)
		reaction[i].multiplier *= normval;
		
	}

}


void Reactor::reactTo(Animatable *anim, TimeValue t)
{
	Animatable* nd;

	theHold.Begin();
	HoldAll();

	switch ( anim->SuperClassID() )
	{
		case CTRL_FLOAT_CLASS_ID:
			setrType(FLOAT_VAR); break;
		case CTRL_POINT3_CLASS_ID:
		case CTRL_POSITION_CLASS_ID:
			setrType(VECTOR_VAR); break;
		case CTRL_SCALE_CLASS_ID:
			setrType(SCALE_VAR); break;
		case CTRL_ROTATION_CLASS_ID:
			setrType(QUAT_VAR); break;
		default: setrType(VECTOR_VAR); 
	}

	if (anim->SuperClassID()==BASENODE_CLASS_ID)
	{
		nd = (INode*)anim;

		Control *c = ((INode*)nd)->GetTMController();
		if (c->ClassID() == BIPSLAVE_CONTROL_CLASS_ID || c->ClassID() == IKSLAVE_CLASSID)
		{
			isABiped(TRUE);
			setrType(QUAT_VAR);
			if (!(assignReactObj((INode*)nd, -2))) return;
		}else {
			setrType(VECTOR_VAR);
			if (!(assignReactObj((INode*)nd, -1))) return;
		}
	} 
	else {
		MyEnumProc dep;             
		((ReferenceTarget*)anim)->EnumDependents(&dep);

		for(int x=0; x<dep.nodes.Count(); x++)
		{
			for(int i=0; i<dep.nodes[x]->NumSubs(); i++)
			{
				Animatable* n = dep.nodes[x]->SubAnim(i);
				if ((Control*)n == (Control*)anim)
				{
					if (!(assignReactObj((INode*)dep.nodes[x], i))) return;
				}
			}
		}
	}
	CreateReaction();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);

	theHold.Accept(GetString(IDS_ASSIGN_TO));
}


/*
void Reactor::assignTo(TrackViewPick res)
{
	theHold.Begin();
	HoldAll();

	switch (res.anim->SuperClassID())
	{
		case CTRL_FLOAT_CLASS_ID:
			setrType(FLOAT_VAR); break;
		case CTRL_POINT3_CLASS_ID:
		case CTRL_POSITION_CLASS_ID:
			setrType(VECTOR_VAR); break;
		case CTRL_SCALE_CLASS_ID:
			setrType(SCALE_VAR); break;
		case CTRL_ROTATION_CLASS_ID:
			setrType(QUAT_VAR); break;
		default: setrType(VECTOR_VAR); //assert(0);
	}

	
	if (res.anim->SuperClassID()==BASENODE_CLASS_ID) {

		Control *c = ((INode*)res.anim)->GetTMController();
		if (c->ClassID() == BIPSLAVE_CONTROL_CLASS_ID || c->ClassID() == IKSLAVE_CLASSID)
		{
			isBiped = TRUE;
			setrType(QUAT_VAR);
			assignReactObj((INode*)res.anim, -2);
		}else {
			setrType(VECTOR_VAR);
			assignReactObj((INode*)res.anim, -1);
		}
	} 
	else {
		assignReactObj((INode*)res.client, res.subNum);
		}

	CreateReaction();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);

	theHold.Accept(GetString(IDS_ASSIGN_TO));

}

*/ 

BOOL Reactor::assignReactObj(INode* client, int subNum)
{
	if (!client) return FALSE;

	reaction.ZeroCount();
	count = 0;
	selected = 0;

	VarRef vr(client);
	vrefs = vr;
	vrefs.subnum = subNum;
	if(MakeRefByID(FOREVER, 0, client) != REF_SUCCEED) {
		vrefs = NULL;
		count = 0;
		TSTR s = GetString(IDS_AF_CIRCULAR_DEPENDENCY);
		MessageBox(hParams, s, GetString(IDS_AF_CANT_ASSIGN), 
		MB_ICONEXCLAMATION | MB_SYSTEMMODAL | MB_OK);
		theHold.Cancel();
		return FALSE;
	}

	return TRUE;
}

void Reactor::updReactionCt(int val)
{
	count += val;
}


BOOL Reactor::CreateReaction(TCHAR *buf, TimeValue t)
{
	TSTR mname(GetString(IDS_AF_VARNAME));

	theHold.Begin();
	HoldParams();  

	if (buf == NULL)
	{
		if (!nmaker) nmaker = GetCOREInterface()->NewNameMaker(FALSE);
		nmaker->MakeUniqueName(mname);
	}else mname = buf;

	int i;
	SVar sv;
	sv.refID = -1;

	sv.strength = 1.0f;
	sv.falloff = 2.0f;
	sv.influence = 100.0f;

	i = reaction.Append(1, &sv);
	reaction[i].name = mname;
	setState(i, NULL, t);
	setReactionValue(i, NULL, t);
	updReactionCt(1);
	selected = i;

	//Scheme to set influence to nearest reaction automatically (better defaults)
	if (i!=0)		//if its not the first reaction
	{
		if(i == 1)   //and if it is the second one update the first while your at it
			setMinInfluence(0);
		setMinInfluence(i);
	}

	theHold.Accept(GetString(IDS_CREATE_REACTION));
	return TRUE;
}

BOOL Reactor::DeleteReaction(int i)
{
	if(vrefs.client != NULL)
	{		
		if (i == -1) i = selected;
		if (count>1)  //can't delete the last sVar
		{
			theHold.Begin();
			HoldParams(); 

			reaction.Delete(i, 1);
			updReactionCt(-1); 
			if (selected >= getVarCount() ) selected -=1;

			ivalid.SetEmpty();
			NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
			GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
			theHold.Accept(GetString(IDS_DELETE_REACTION));
			return TRUE;
		}

	}return FALSE;
}



TCHAR *Reactor::getVarName(int i)
{
	if(i>=0&&i<count&&count>0)
		return reaction[i].name;
	return "error";
}

void Reactor::setVarName(int i, TSTR name)
{
	if (i>=0&&i<count&&count>0)
		reaction[i].name = name;
}

void* Reactor::getReactionValue(int i)
{
	if(getVarCount() > 0 && i >= 0 && i < getVarCount())
	{
		switch (rtype)
		{
			case FLOAT_VAR:
				return &reaction[i].fvalue;
			case QUAT_VAR:
				return &reaction[i].qvalue;
			case VECTOR_VAR:
			case SCALE_VAR:
				return &reaction[i].pvalue;
			default: assert(0);
		}
	}
	return NULL;
}


BOOL Reactor::setReactionValue(int i, void *val, TimeValue t)
{
	float f;
	Quat q;
	Point3 p;
	ScaleValue s;
	Control *c;
	
	if (t == NULL) t = GetCOREInterface()->GetTime();

	if (i == -1) i = selected;

	if (vrefs.client != NULL) {

		theHold.Begin();
		HoldParams();
		
		reaction[i].fvalue = 0.0f;
		reaction[i].pvalue = Point3(0,0,0);
		reaction[i].qvalue.Identity();

		if ( val == NULL )
		{
			if (vrefs.subnum < 0 )
			{
				if (isBiped)	
				{
					GetAbsoluteControlValue(vrefs.client, t, &(reaction[i].qvalue), FOREVER);
				}
				else {
					GetAbsoluteControlValue(vrefs.client, t, &(reaction[i].pvalue), FOREVER);
				}
			}
			else {
				c = (Control *)vrefs.client->SubAnim(vrefs.subnum);
				switch (rtype)
				{
					case FLOAT_VAR:
						c->GetValue(t, &f, FOREVER);
						reaction[i].fvalue = f;
						break;
					case VECTOR_VAR:
						c->GetValue(t, &p, FOREVER);
						reaction[i].pvalue = p;
						break;
					case SCALE_VAR:
						c->GetValue(t, &s, FOREVER);
						reaction[i].pvalue = s.s;
						break;
					case QUAT_VAR:
						c->GetValue(t, &q, FOREVER);
						reaction[i].qvalue = q;
						break;
				}
			}
		} else {
			switch (rtype)
			{
				case FLOAT_VAR:
					reaction[i].fvalue = (*(float*)val);
					break;
				case SCALE_VAR:
				case VECTOR_VAR:
					reaction[i].pvalue = (*(Point3*)val);
					break;
				case QUAT_VAR:
					reaction[i].qvalue = (*(Quat*)val);
					break;
			}
		}
		if( ip ) dlg->UpdateReactionValue();   //Updates the value field
		ivalid.SetEmpty();
		NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
		GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
		theHold.Accept(GetString(IDS_UNSET_VALUE));
		return TRUE;
	}
	return FALSE;
}

float Reactor::getCurFloatValue(TimeValue t)
{
	float f;
	Control *c;

	if (vrefs.client != NULL) {
		c = GetControlInterface(vrefs.client->SubAnim(vrefs.subnum));
		c->GetValue(t, &f, FOREVER);
		return f;
	}
	return 0.0f;
}

Point3 Reactor::getCurPoint3Value(TimeValue t)
{
	Point3 p = Point3(0,0,0);
	Control *c;

	if (vrefs.client != NULL) {
		if (vrefs.subnum < 0 )
		{
			GetAbsoluteControlValue(vrefs.client, t, &p, FOREVER);
		}else {
			c = GetControlInterface(vrefs.client->SubAnim(vrefs.subnum));
			c->GetValue(t, &p, FOREVER);
		}
	}
	return p;
}

ScaleValue Reactor::getCurScaleValue(TimeValue t)
{
	ScaleValue ss;
	Control *c;

	if (vrefs.client != NULL) {
		c = GetControlInterface(vrefs.client->SubAnim(vrefs.subnum));
		c->GetValue(t, &ss, FOREVER);
	}
	return ss;
}

Quat Reactor::getCurQuatValue(TimeValue t)
{
	Quat q;
	q.Identity();
	Control *c;

	if (vrefs.client != NULL) {
		if (vrefs.subnum < 0 )
		{
			GetAbsoluteControlValue(vrefs.client, t, &q, FOREVER);
		}else {
			c = GetControlInterface(vrefs.client->SubAnim(vrefs.subnum));
			c->GetValue(t, &q, FOREVER);
		}
	}
	return q;
}



float Reactor::getInfluence(int num)
{
	return reaction[num].influence;
}

BOOL Reactor::setInfluence(int num, float inf)
{
	BOOL hold_here = false;

	if (!theHold.Holding()) { hold_here=true; theHold.Begin(); HoldParams();}

	reaction[num].influence = inf;

	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	if (hold_here) theHold.Accept(GetString(IDS_AF_CHANGEINFLUENCE));
	return TRUE;
}

void Reactor::setMinInfluence(int x)
{
	BOOL hold_here = false;
	if (!theHold.Holding()) 
	{ 
		hold_here=true; theHold.Begin(); HoldParams();
	}
	
	if ( x == -1 ) x = selected;
	float dist = 1000000.0f;
	float disttemp = 100.0f;
	Point3 axis;

	if ( count && x >= 0 )
	{
		for(int i=0;i<count;i++)
		{
			if (i != x)
			{
				switch (rtype)
				{
					case FLOAT_VAR:
						disttemp = (float)fabs(reaction[x].fvalue - reaction[i].fvalue);
						break;
					case VECTOR_VAR: 
					case SCALE_VAR: 
						disttemp = Distance(reaction[i].pvalue, reaction[x].pvalue);
						break;
					case QUAT_VAR: 
						disttemp = QangAxis(reaction[i].qvalue, reaction[x].qvalue, axis);
						break;
					default: disttemp = 100.0f; break;
				} 
			}
			if (disttemp != 0.0f) 
			{
				dist = (dist <= disttemp && dist != 100 ? dist : disttemp);
			} else if (dist == 1000000.0f && i == count-1) dist = 100.0f;
		}
		reaction[x].influence = dist;
		NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
		if (hold_here) theHold.Accept(GetString(IDS_AF_CHANGEINFLUENCE));
	}
}

void Reactor::setMaxInfluence(int x)
{
	BOOL hold_here = false;
	if (!theHold.Holding()) { hold_here=true; theHold.Begin(); HoldParams();}
	
	if ( x == -1 ) x = selected;

	float dist;
	float disttemp;
	Point3 axis;

	if ( count && x >= 0 )
	{
		for(int i=0;i<count;i++)
		{
			switch (rtype)
			{
				case FLOAT_VAR:
					disttemp = (float)fabs(reaction[x].fvalue - reaction[i].fvalue);
					break;
				case VECTOR_VAR: 
				case SCALE_VAR: 
					disttemp = Distance(reaction[i].pvalue, reaction[x].pvalue);
					break;
				case QUAT_VAR: 
					disttemp = QangAxis(reaction[i].qvalue, reaction[x].qvalue, axis);
					break;
				default: disttemp = 100.0f; break;
			}
			if (i == 0) dist = disttemp;
			else dist = (dist >= disttemp ? dist : disttemp);
		}
		reaction[x].influence = dist;
		NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
		if (hold_here) theHold.Accept(GetString(IDS_AF_CHANGEINFLUENCE));
	}
}



float Reactor::getStrength(int num)
{
	return reaction[num].strength;
}

BOOL Reactor::setStrength(int num, float inf)
{
	BOOL hold_here = false;

	if (!theHold.Holding()) { hold_here=true; theHold.Begin(); HoldParams();}

	reaction[num].strength = inf;

	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	if(hold_here) theHold.Accept(GetString(IDS_AF_CHANGESTRENGTH));
	return TRUE;
}

float Reactor::getFalloff(int num)
{
	return reaction[num].falloff;
}

BOOL Reactor::setFalloff(int num, float inf)
{
	BOOL hold_here = false;

	if (!theHold.Holding()) { hold_here=true; theHold.Begin(); HoldParams();}

	reaction[num].falloff = inf;

	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	if (hold_here) theHold.Accept(GetString(IDS_AF_CHANGEFALLOFF));
	return TRUE;
}

BOOL Reactor::setEditing(BOOL ed)
{
	editing = ed;
	ivalid.SetEmpty();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
	return editing;
}

BOOL Reactor::setState(int num, void *val, TimeValue t)
{
	if (t == NULL) t = GetCOREInterface()->GetTime();
	switch (type)
	{
		case REACTORFLOAT: 
			if (val==NULL) 
				this->GetValue(t, &(reaction[num].fstate), FOREVER, CTRL_ABSOLUTE);
				else reaction[num].fstate = *((float*)val);
			break; 
		case REACTORROT: 
			 if (val==NULL) 
				 this->GetValue(t, &(reaction[num].qstate), FOREVER, CTRL_ABSOLUTE);
				else reaction[num].qstate = *((Quat*)val);
			break; 
		case REACTORP3: 
		case REACTORSCALE: 
		case REACTORPOS: 
			if (val==NULL) 
				this->GetValue(t, &(reaction[num].pstate), FOREVER, CTRL_ABSOLUTE);
				else reaction[num].pstate = *((Point3*)val);
			break; 
		default: return false;
	}
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	return true; 
}


void* Reactor::getState(int num)
{
	if(getVarCount() > 0 && num >= 0 && num < getVarCount())
		switch (type)
		{
			case REACTORFLOAT: 
				return &reaction[num].fstate; 
			case REACTORROT: 
				return &reaction[num].qstate;
			case REACTORP3: 
			case REACTORSCALE: 
			case REACTORPOS: 
				return &reaction[num].pstate;

		}
	return NULL;
}


void Reactor::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{
}

void Reactor::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
{
}


void Reactor::GetAbsoluteControlValue(
		INode *node,TimeValue t,void *pt,Interval &iv)
{
	if (node != NULL)
	{
		if (isBiped)
		{
			Matrix3 cur_mat = node->GetNodeTM(t,&iv);
			Matrix3 par_mat =  node->GetParentTM(t);
			Matrix3 relative_matrix = cur_mat * Inverse( par_mat);
			Quat q = Quat(relative_matrix);
			*(Quat*)pt = q;
		}else {	
			Matrix3 tm = node->GetNodeTM(t,&iv);
			*(Point3*)pt = tm.GetTrans();
		}
	}
}


//--------------------------------------------------------------------

BOOL Reactor::ChangeParents(TimeValue t,const Matrix3& oldP,const Matrix3& newP,const Matrix3& tm)
	{
		HoldAll();
		// Position and rotation controllers need their path counter rotated to
		// account for the new parent.
		Matrix3 rel = oldP * Inverse(newP);
		// Modify the controllers current value (the controllers cache)
		*((Point3*)(&curpval)) = *((Point3*)(&curpval)) * rel;
		*((Quat*)(&curqval)) = *((Quat*)(&curqval)) * rel;

		//Modify each reaction state 
		for (int i=0;i<count;i++)
		{
			*((Point3*)(&reaction[i].pstate)) = *((Point3*)(&reaction[i].pstate)) * rel;
			*((Quat*)(&reaction[i].qstate)) = *((Quat*)(&reaction[i].qstate)) * rel;
		}
		ivalid.SetEmpty();
		return TRUE;
	}

void Reactor::Update(TimeValue t)
{
	if (!ivalid.InInterval(t))
	{
		ivalid = FOREVER;		
		if (vrefs.client!=NULL)
		{
			float f;
			Quat q;
			Point3 p;
			ScaleValue s;
			//update the validity interval
			if (vrefs.subnum < 0 )
			{
				if (isBiped) GetAbsoluteControlValue(vrefs.client, t, &q, ivalid);
					else GetAbsoluteControlValue(vrefs.client, t, &p, ivalid);
			}else {
				switch (rtype)
				{
				case FLOAT_VAR:
					GetControlInterface(vrefs.client->SubAnim(vrefs.subnum))->GetValue(t, &f, ivalid);
					break;
				case VECTOR_VAR:
					GetControlInterface(vrefs.client->SubAnim(vrefs.subnum))->GetValue(t, &p, ivalid);
					break;
				case SCALE_VAR:
					GetControlInterface(vrefs.client->SubAnim(vrefs.subnum))->GetValue(t, &s, ivalid);
					break;
				case QUAT_VAR:
					GetControlInterface(vrefs.client->SubAnim(vrefs.subnum))->GetValue(t, &q, ivalid);
					break;
				}
			}
			curfval = reaction[selected].fstate;
			curpval = reaction[selected].pstate;
			curqval = reaction[selected].qstate;
		}
	}
}


void Reactor::SetValue(TimeValue t, void *val, int commit, GetSetMethod method)
{
	if (editing && count) {
		if (!TestAFlag(A_SET)) {				
			HoldTrack();
			tmpStore.PutBytes(sizeof(Point3),&curpval,this);
			SetAFlag(A_SET);
			}
		if (method == CTRL_RELATIVE) curpval += *((Point3*)val);
		else curpval = *((Point3*)val);

		ivalid.SetInstant(t);	
		if (commit) CommitValue(t);
		if (!commit) NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	}
}


void Reactor::CommitValue(TimeValue t) {
	if (TestAFlag(A_SET)) {		
		if (ivalid.InInterval(t)) {

			Point3 old;
			tmpStore.GetBytes(sizeof(Point3),&old,this);					
			reaction[selected].pstate = curpval;

			tmpStore.Clear(this);
			ivalid.SetEmpty();
			NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
		}
		ClearAFlag(A_SET);
	}
}

void Reactor::RestoreValue(TimeValue t) 
	{
	if (TestAFlag(A_SET)) {
		if (count) {
			tmpStore.GetBytes(sizeof(Point3),&curpval,this);
			reaction[selected].pstate = curpval;
			tmpStore.Clear(this);
			ivalid.SetInstant(t);
			}
		ClearAFlag(A_SET);
		}
	}



//------------------------------------------------------------

RefTargetHandle FloatReactor::Clone(RemapDir& remap)
	{
	// make a new reactor controller and give it our param values.
	FloatReactor *cont = new FloatReactor(TRUE);
	*cont = *this;
	CloneControl(cont,remap);
	return cont;
	}

void FloatReactor::GetValue(
		TimeValue t, void *val, Interval &valid, GetSetMethod method)
{
	Update(t);
	valid &= ivalid;  
	if (!editing)
	{
		ComputeMultiplier(t);
		//sum up all the weighted states
		float ray = 0.0f;
		for(int i=0;i<count;i++)
			ray +=((reaction[i].fstate)*reaction[i].multiplier);
		if (count) curfval = ray;  
	}

	if (method==CTRL_RELATIVE) {
		*((float*)val) += curfval;
	} else {
		*((float*)val) = curfval;
	}
		
}


//--------------------------------------------------------------------------

RefTargetHandle PositionReactor::Clone(RemapDir& remap)
{
	// make a new reactor controller and give it our param values.
	PositionReactor *cont = new PositionReactor(TRUE);
	*cont = *this;
	CloneControl(cont,remap);
	return cont;
}

void PositionReactor::GetValue(
		TimeValue t, void *val, Interval &valid, GetSetMethod method)
{
	Point3 ray = Point3(0,0,0);

	Update(t);
	valid &= ivalid;  
	if (!editing && count)
	{
		ComputeMultiplier(t);
		//sum up all the weighted states
		for(int i=0;i<count;i++)
			ray +=((reaction[i].pstate)*reaction[i].multiplier);
		curpval = ray;  
	}

	if (method==CTRL_RELATIVE) {
  		Matrix3 *mat = (Matrix3*)val;	
		mat->PreTranslate(curpval);
	} else {
		*((Point3*)val) = curpval;
	}
}
//---------------------------------------------------------

RefTargetHandle Point3Reactor::Clone(RemapDir& remap)
	{
	// make a new reactor controller and give it our param values.
	Point3Reactor *cont = new Point3Reactor(TRUE);
	*cont = *this;	
	CloneControl(cont,remap);
	return cont;
	}

void Point3Reactor::GetValue(
		TimeValue t, void *val, Interval &valid, GetSetMethod method)
	{
	Update(t);
	valid &= ivalid;  
	if (!editing && count)
	{
		ComputeMultiplier(t);
		//sum up all the weighted states
		Point3 ray = Point3(0,0,0);
		for(int i=0;i<count;i++)
			ray +=((reaction[i].pstate)*reaction[i].multiplier);
		curpval = ray;  
	}

	if (method==CTRL_RELATIVE) {
		*((Point3*)val) += curpval;
	} else {
		*((Point3*)val) = curpval;
	}
}



//--------------------------------------------------------------------------

RefTargetHandle ScaleReactor::Clone(RemapDir& remap)
	{
	// make a new reactor controller and give it our param values.
	ScaleReactor *cont = new ScaleReactor(TRUE);
	*cont = *this;
	CloneControl(cont,remap);
	return cont;
	}

void ScaleReactor::GetValue(
		TimeValue t, void *val, Interval &valid, GetSetMethod method)
{
	Update(t);
	valid &= ivalid;  
	if (!editing && count)
	{
		ComputeMultiplier(t);
		//sum up all the weighted states
		Point3 ray = Point3(0,0,0);
		for(int i=0;i<count;i++)
			ray +=((reaction[i].pstate)*reaction[i].multiplier);
		curpval = ray;  
	}

	if (method==CTRL_RELATIVE) {
  		Matrix3 *mat = (Matrix3*)val;
		ApplyScaling(*mat, curpval);
	} else {
		*((Point3*)val) = curpval;
	}
}


void ScaleReactor::SetValue(TimeValue t, void *val, int commit, GetSetMethod method)
{
	if (editing && count) {
		if (!TestAFlag(A_SET)) {				
			HoldTrack();
			tmpStore.PutBytes(sizeof(Point3),&curpval,this);
			SetAFlag(A_SET);
			}
		if (method == CTRL_RELATIVE) curpval *= *((Point3*)val);
		else curpval = *((Point3*)val);

		ivalid.SetInstant(t);	
		if (commit) CommitValue(t);
		if (!commit) NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	}
}


//------------------------------------------------------------------

RefTargetHandle RotationReactor::Clone(RemapDir& remap)
	{
	// make a new reactor controller and give it our param values.
	RotationReactor *cont = new RotationReactor(TRUE);
	*cont = *this;	
	CloneControl(cont,remap);
	return cont;
	}

void RotationReactor::GetValue(
		TimeValue t, void *val, Interval &valid, GetSetMethod method)
{
	float eulr[3];

	Update(t);
	valid &= ivalid;  
	if (!editing)
	{
		ComputeMultiplier(t);
		//sum up all the weighted states
		curqval.Identity();
		Quat ray;
		
		for(int i=0;i<count;i++)
		{
			ray = reaction[i].qstate;
			QuatToEuler(ray, eulr);
			eulr[0] *= reaction[i].multiplier;
			eulr[1] *= reaction[i].multiplier;
			eulr[2] *= reaction[i].multiplier;
			EulerToQuat(eulr, ray);

			curqval += ray;
		}
	}

	if (method==CTRL_RELATIVE) {
  	Matrix3 *mat = (Matrix3*)val;		//Why is mat always (0,0,0)???
	PreRotateMatrix(*mat, curqval);
	} else {
		*((Quat*)val) = curqval;
	}
		
}

void RotationReactor::SetValue(TimeValue t, void *val, int commit, GetSetMethod method)
{
	if (editing && count) {
		if (!TestAFlag(A_SET)) {				
			HoldTrack();
			tmpStore.PutBytes(sizeof(Point3),&curpval,this);
			SetAFlag(A_SET);
			}

		if (method == CTRL_RELATIVE) curqval *= Quat(*((AngAxis*)val));
		else curqval = Quat(*((AngAxis*)val));

		ivalid.SetInstant(t);	
		if (commit) CommitValue(t);
		if (!commit) NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	}
}


void RotationReactor::CommitValue(TimeValue t) {
	if (TestAFlag(A_SET)) {		
		if (ivalid.InInterval(t)) {

			Quat old;
			tmpStore.GetBytes(sizeof(Quat),&old,this);					
			reaction[selected].qstate = curqval;

			tmpStore.Clear(this);
			ivalid.SetEmpty();
			NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
		}
		ClearAFlag(A_SET);
	}
}

void RotationReactor::RestoreValue(TimeValue t) 
	{
	if (TestAFlag(A_SET)) {
		if (count) {
			tmpStore.GetBytes(sizeof(Quat),&curqval,this);
			reaction[selected].qstate = curqval;
			tmpStore.Clear(this);
			ivalid.SetInstant(t);
			}
		ClearAFlag(A_SET);
		}
	}


//--------------------------------------------------------------------------

class ReactionFilter : public TrackViewFilter {
public:
	BOOL proc(Animatable *anim, Animatable *client, int subNum)
	{ 
		
		if (anim->SuperClassID() == BASENODE_CLASS_ID) {
			INode *node = (INode*)anim;
			return !node->IsRootNode();
			}

		return anim->SuperClassID() == CTRL_FLOAT_CLASS_ID ||
			anim->SuperClassID() == CTRL_POSITION_CLASS_ID ||
			anim->SuperClassID() == CTRL_POINT3_CLASS_ID ||
			anim->SuperClassID() == CTRL_SCALE_CLASS_ID ||
			anim->SuperClassID() == CTRL_ROTATION_CLASS_ID || 
			anim->SuperClassID() == BASENODE_CLASS_ID; 
	}

};


//--------------------------------------------------------------
// UI Stuff
//--------------------------------------------------------------

void ReactorDlg::UpdateVarList()
{
	int i, ct;

	SendDlgItemMessage(hWnd, IDC_REACTION_LIST, LB_SETCURSEL, cont->selected, 0);
	if (cont->selected >= cont->getVarCount() ) cont->selected -=1;
	ct = cont->getVarCount();
	SendDlgItemMessage(hWnd, IDC_REACTION_LIST, LB_RESETCONTENT, 0, 0);
	for(i = 0; i < ct; i++)
	{
		SendDlgItemMessage(hWnd, IDC_REACTION_LIST, LB_ADDSTRING, 0, (LPARAM)cont->getVarName(i));
	}
	SendDlgItemMessage(hWnd, IDC_REACTION_LIST, LB_SETCURSEL, cont->selected, 0);

	i = cont->selected;
	if(i >=0 ) {
			
		iNameEdit->SetText(cont->reaction[i].name);
		UpdateReactionValue();
		iInfluence->SetValue(cont->reaction[i].influence, FALSE);
		iFalloff[0]->SetValue(cont->reaction[i].falloff, FALSE);
		iStrength->SetValue(cont->reaction[i].strength, FALSE);
	}
	cont->ivalid.SetEmpty();
	cont->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	if (ip) ip->RedrawViews(ip->GetTime());
}

void ReactorDlg::UpdateReactionValue()
{
	AngAxis aa;
	float f;
	TCHAR buf[256];
	_stprintf(buf, _T(""));
	int i = cont->selected;

	if (cont->vrefs.client)
	{
		switch (cont->rtype)
		{
			case FLOAT_VAR:
				assert(SetDlgItemFloat(hWnd, IDC_VALUE_STATUS, cont->reaction[i].fvalue));
				break;
			case VECTOR_VAR:
			case SCALE_VAR:
				_stprintf(buf, _T("( %g; %g; %g )"), cont->reaction[i].pvalue[0], cont->reaction[i].pvalue[1], cont->reaction[i].pvalue[2]);
				SetDlgItemText(hWnd, IDC_VALUE_STATUS, buf);	
				break;
			case QUAT_VAR:
				aa = cont->reaction[i].qvalue;
				f = RadToDeg(aa.angle);
				_stprintf(buf, _T("%g ( %g; %g; %g )"), f, aa.axis.x, aa.axis.y, aa.axis.z);
				SetDlgItemText(hWnd, IDC_VALUE_STATUS, buf);	
				break;
		}
	}
	else SetDlgItemText(hWnd, IDC_VALUE_STATUS, buf);	
}

//------------------------------------------------------------


static BOOL CALLBACK ReactorDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ReactorCtrlWindow {
	public:
		HWND hWnd;
		HWND hParent;
		Control *cont;
		ReactorCtrlWindow() {assert(0);}
		ReactorCtrlWindow(HWND hWnd,HWND hParent,Control *cont)
			{this->hWnd=hWnd; this->hParent=hParent; this->cont=cont;}
	};
static Tab<ReactorCtrlWindow> reactorCtrlWindows;

static void RegisterReactorCtrlWindow(HWND hWnd, HWND hParent, Control *cont)
	{
	ReactorCtrlWindow rec(hWnd,hParent,cont);
	reactorCtrlWindows.Append(1,&rec);
	}

static void UnRegisterReactorCtrlWindow(HWND hWnd)
	{	
	for (int i=0; i<reactorCtrlWindows.Count(); i++) {
		if (hWnd==reactorCtrlWindows[i].hWnd) {
			reactorCtrlWindows.Delete(i,1);
			return;
			}
		}	
	}

static HWND FindOpenReactorCtrlWindow(HWND hParent,Control *cont)
	{	
	for (int i=0; i<reactorCtrlWindows.Count(); i++) {
		if (hParent == reactorCtrlWindows[i].hParent &&
			cont    == reactorCtrlWindows[i].cont) {
			return reactorCtrlWindows[i].hWnd;
			}
		}
	return NULL;
	}



ReactorDlg::ReactorDlg(
		Reactor *cont,
		ParamDimensionBase *dim,
		TCHAR *pname,
		IObjParam *ip,
		HWND hParent)
	{
	this->cont = cont;
	this->ip   = ip;
	this->dim  = dim;
	valid = FALSE;
	elems = cont->Elems();
	MakeRefByID(FOREVER,0,cont);
	cont->hParams = hWnd = CreateDialogParam(
		hInstance,
		MAKEINTRESOURCE(IDD_REACTOR_PARAMS),
		hParent,
		ReactorDlgProc,
		(LPARAM)this);	
	TSTR title = TSTR(GetString(IDS_AF_REACTORTITLE)) + TSTR(pname);
	SetWindowText(hWnd,title);
	ip->RegisterTimeChangeCallback(this);
	reactorShortcutCB = new ReactorShortcutCB<Reactor>(cont);
	ip->ActivateShortcutTable(reactorShortcutCB, kReactorShortcuts);
	}

ReactorDlg::~ReactorDlg()
	{
	ip->DeactivateShortcutTable(reactorShortcutCB, kReactorShortcuts);
	delete reactorShortcutCB;

	UnRegisterReactorCtrlWindow(hWnd);
	ip->UnRegisterTimeChangeCallback(this);
	cont->hParams = NULL;
	cont->ip = NULL;
	cont->dlg = NULL;
	DeleteAllRefsFromMe();

	ReleaseISpinner(iFloatState);
	ReleaseISpinner(iInfluence);
	ReleaseISpinner(iStrength);
	ReleaseISpinner(iFalloff[0]);
	ReleaseICustEdit(iNameEdit);
	ReleaseICustEdit(iValueStatus);
	ReleaseICustButton(iCreateBut);
	ReleaseICustButton(iDeleteBut);
	ReleaseICustButton(iSetBut);
	ReleaseICustButton(iEditBut);

	}

void ReactorDlg::Invalidate()
	{
	valid = FALSE;
	InvalidateRect(hWnd,NULL,FALSE);
	}

void ReactorDlg::UpdateNodeName()
	{
		TSTR nname, pname;
		pname = "";
		
		if (cont->vrefs.client)
		{

			if(cont->vrefs.subnum < 0)	// special case: we're referencing a node
				pname = ((INode *)cont->vrefs.client)->GetName();
			else {
				cont->getNodeName(cont->vrefs.client,nname);
				if (nname.Length())
					pname = nname + TSTR(_T("\\")) + cont->vrefs.client->SubAnimName(cont->vrefs.subnum);
				else 
					pname = cont->vrefs.client->SubAnimName(cont->vrefs.subnum);
			}
			assert(SetDlgItemText(hWnd, IDC_TRACK_NAME, pname));
		} else{
			assert(SetDlgItemText(hWnd, IDC_TRACK_NAME, pname));
		}
	}

void ReactorDlg::Update()
{
	TCHAR buf[256];

	if (cont->count) {
		valid = FALSE;
		float fval, f;
		Point3 pval;
		Quat qval;
		AngAxis aa;
		ScaleValue sval;
		Control *c;

		iFloatState->Enable(TRUE);
		iInfluence->Enable(TRUE);
		iFalloff[0]->Enable(TRUE);
		iStrength->Enable(TRUE);
		iCreateBut->Enable(TRUE);
		iDeleteBut->Enable(TRUE);
		iSetBut->Enable(TRUE);
		iEditBut->Enable(TRUE);

		iFloatState->SetValue(cont->reaction[cont->selected].fstate, FALSE);
		iStrength->SetValue(cont->reaction[cont->selected].strength, FALSE);
		iFalloff[0]->SetValue(cont->reaction[cont->selected].falloff, FALSE);
		iInfluence->SetValue(cont->reaction[cont->selected].influence, FALSE);

		UpdateVarList();
		UpdateNodeName();
		iEditBut->SetCheck(cont->editing);

		if (cont->vrefs.subnum < 0 )
		{
			if (cont->isBiped)	
			{
				cont->GetAbsoluteControlValue(cont->vrefs.client, ip->GetTime(), &qval, FOREVER);
					aa = qval;
					f = RadToDeg(aa.angle);
					_stprintf(buf, _T("%g ( %g; %g; %g )"), f, aa.axis.x, aa.axis.y, aa.axis.z);
			}
			else {
				cont->GetAbsoluteControlValue(cont->vrefs.client, ip->GetTime(), &pval, FOREVER);
				_stprintf(buf, _T("( %g; %g; %g )"), pval.x, pval.y, pval.z);
			}
			SetDlgItemText(hWnd, IDC_TRACK_VALUE, buf);	
		}
		else {
			c = (Control *)cont->vrefs.client->SubAnim(cont->vrefs.subnum);
			switch (cont->rtype)
			{
				case FLOAT_VAR:
					c->GetValue(ip->GetTime(), &fval, FOREVER);
					assert(SetDlgItemFloat(hWnd, IDC_TRACK_VALUE, fval));
					break;
				case VECTOR_VAR:
					c->GetValue(ip->GetTime(), &pval, FOREVER);
					_stprintf(buf, _T("( %g; %g; %g )"), pval.x, pval.y, pval.z);
					SetDlgItemText(hWnd, IDC_TRACK_VALUE, buf);	
					break;
				case SCALE_VAR:
					c->GetValue(ip->GetTime(), &sval, FOREVER);
					_stprintf(buf, _T("( %g; %g; %g )"), sval.s.x, sval.s.y, sval.s.z);
					SetDlgItemText(hWnd, IDC_TRACK_VALUE, buf);	
					break;
				case QUAT_VAR:
					c->GetValue(ip->GetTime(), &qval, FOREVER);
					aa = qval;
					f = RadToDeg(aa.angle);
					_stprintf(buf, _T("%g ( %g; %g; %g )"), f, aa.axis.x, aa.axis.y, aa.axis.z);
					SetDlgItemText(hWnd, IDC_TRACK_VALUE, buf);	
					break;
			}
		}
		switch (cont->type)
		{
			case REACTORFLOAT:
				assert(SetDlgItemFloat(hWnd, IDC_OUTPUT_STATUS, cont->curfval));
				break;
			case REACTORPOS:
			case REACTORP3:
			case REACTORSCALE:
				_stprintf(buf, _T("( %g; %g; %g )"), cont->curpval.x, cont->curpval.y, cont->curpval.z);
				SetDlgItemText(hWnd, IDC_OUTPUT_STATUS, buf);	
				break;
			case REACTORROT:
				aa = cont->curqval;
				f = RadToDeg(aa.angle);
				_stprintf(buf, _T("%g ( %g; %g; %g )"), f, aa.axis.x, aa.axis.y, aa.axis.z);
				SetDlgItemText(hWnd, IDC_OUTPUT_STATUS, buf);	
				break;
		}		
	} else{
		_stprintf(buf, _T(""));
		iFloatState->Enable(FALSE);
		iInfluence->Enable(FALSE);
		iFalloff[0]->Enable(FALSE);
		iStrength->Enable(FALSE);
		iCreateBut->Enable(FALSE);
		iDeleteBut->Enable(FALSE);
		iSetBut->Enable(FALSE);
		iEditBut->Enable(FALSE);
		UpdateNodeName();
		UpdateVarList();
		UpdateReactionValue();
		SetDlgItemText(hWnd, IDC_TRACK_VALUE, buf);	
		SetDlgItemText(hWnd, IDC_OUTPUT_STATUS, buf);
		iNameEdit->SetText("");
	}
}


void ReactorDlg::SetupUI(HWND hWnd)
{
	this->hWnd = hWnd;

	iFloatState = GetISpinner(GetDlgItem(hWnd,IDC_FLOATSTATE_SPIN));
	iFloatState ->SetLimits(-99999,99999,FALSE);
	iFloatState ->SetAutoScale();
	iFloatState ->LinkToEdit(GetDlgItem(hWnd,IDC_FLOATSTATE_EDIT),EDITTYPE_FLOAT);	

	iStrength = GetISpinner(GetDlgItem(hWnd,IDC_STRENGTH_SPIN));
	iStrength ->SetLimits(0,99999,FALSE);
	iStrength ->SetAutoScale();
	iStrength ->LinkToEdit(GetDlgItem(hWnd,IDC_STRENGTH_EDIT),EDITTYPE_FLOAT);	

	iFalloff[0] = GetISpinner(GetDlgItem(hWnd,IDC_FALLOFF_SPIN));
	iFalloff[0] ->SetLimits(0.001f,10.0f,FALSE);
	iFalloff[0] ->SetAutoScale();
	iFalloff[0] ->LinkToEdit(GetDlgItem(hWnd,IDC_FALLOFF_EDIT),EDITTYPE_FLOAT);	

	iInfluence = GetISpinner(GetDlgItem(hWnd,IDC_INFLUENCE_SPIN));
	iInfluence ->SetLimits(0.0f,99999.0f,FALSE);
	iInfluence ->SetAutoScale();
	iInfluence ->LinkToEdit(GetDlgItem(hWnd,IDC_INFLUENCE_EDIT),EDITTYPE_FLOAT);	

	iNameEdit = GetICustEdit(GetDlgItem(hWnd,IDC_NAME_EDIT));
	iValueStatus = GetICustEdit(GetDlgItem(hWnd,IDC_VALUE_STATUS));
	
	iCreateBut = GetICustButton(GetDlgItem(hWnd,IDC_CREATE_BUTTON));
	iDeleteBut = GetICustButton(GetDlgItem(hWnd,IDC_DELETE_BUTTON));
	iSetBut    = GetICustButton(GetDlgItem(hWnd,IDC_SET_BUTTON));
	iEditBut   = GetICustButton(GetDlgItem(hWnd,IDC_EDIT_BUTTON));
	iCreateBut->SetType(CBT_PUSH);
	iDeleteBut->SetType(CBT_PUSH);
	iSetBut->SetType(CBT_PUSH);
	iEditBut->SetType(CBT_CHECK);
	iEditBut->SetHighlightColor(GREEN_WASH);

	if (cont->ClassID() == REACTORFLOAT_CLASS_ID){
		ShowWindow(GetDlgItem(hWnd, IDC_EDIT_BUTTON), SW_HIDE);
	}
	else{
		ShowWindow(GetDlgItem(hWnd, IDC_FLOATSTATE_SPIN), SW_HIDE);
		ShowWindow(GetDlgItem(hWnd, IDC_FLOATSTATE_EDIT), SW_HIDE);
		ShowWindow(GetDlgItem(hWnd, IDC_EDITSTATE_STATIC), SW_HIDE);
	}

	if (!cont->reaction.Count())
	{
		iFloatState->Enable(FALSE);
		iInfluence->Enable(FALSE);
		iFalloff[0]->Enable(FALSE);
		iStrength->Enable(FALSE);
		iCreateBut->Enable(FALSE);
		iDeleteBut->Enable(FALSE);
		iSetBut->Enable(FALSE);
		iEditBut->Enable(FALSE);
	}
}


void ReactorDlg::WMCommand(int id, int notify, HWND hCtrl)
	{
		Point3 pt;
		TrackViewPick res;
		ReactionFilter rf;

		switch (id) {
			case IDC_PICK_BUTTON:
				if(cont->ip->TrackViewPickDlg(hWnd, &res, &rf)) {
//				cont->assignTo(res);
				cont->reactTo(res.anim);
				}break;
			case IDC_CREATE_BUTTON:
					if(cont->vrefs.client != NULL)
						cont->CreateReaction();
				break;
			case IDC_REACTION_LIST:
				if(notify == LBN_SELCHANGE) {
					cont->selected = SendDlgItemMessage(hWnd, IDC_REACTION_LIST, LB_GETCURSEL, 0, 0);
					UpdateVarList();
				}break;
			case IDC_DELETE_BUTTON:
				cont->DeleteReaction();
				break;
			case IDC_SET_BUTTON:
				if(cont->vrefs.client != NULL)
					cont->setReactionValue(cont->selected);
				break;
			case IDC_EDIT_BUTTON:
				cont->setEditing(iEditBut->IsChecked());
				break;
			default: break;
		}
	}

void ReactorDlg::SpinnerChange(int id,BOOL drag)
	{

	if (!drag)
		if (!theHold.Holding()) {
		SpinnerStart(id);
		}
	
	switch (id) {
		case IDC_FALLOFF_SPIN:
			cont->setFalloff(cont->selected, (float)iFalloff[0]->GetFVal());
			break;
		case IDC_INFLUENCE_SPIN:
			cont->setInfluence(cont->selected, (float)iInfluence->GetFVal());
			break;

		case IDC_STRENGTH_SPIN:
			cont->setStrength(cont->selected, (float)iStrength->GetFVal());
			break;

		case IDC_FLOATSTATE_SPIN:
			cont->HoldParams();
			cont->reaction[cont->selected].fstate = (float)iFloatState->GetFVal();
			break;
		}
		Change(FALSE);
	}

void ReactorDlg::SpinnerStart(int id)
	{
	switch (id) {
		case IDC_FLOATSTATE_SPIN:
		case IDC_STRENGTH_SPIN:
		case IDC_INFLUENCE_SPIN:
		case IDC_FALLOFF_SPIN:
			theHold.Begin();
			cont->HoldParams();
			break;
		}
	}

void ReactorDlg::SpinnerEnd(int id,BOOL cancel)
{
	if (cancel) {
		theHold.Cancel();
	} else {
	switch (id) {
		case IDC_FLOATSTATE_SPIN:
		case IDC_FLOATSTATE_EDIT:
			theHold.Accept(GetString(IDS_AF_CHANGESTATE));
			break;
		case IDC_STRENGTH_SPIN:
		case IDC_STRENGTH_EDIT:
			theHold.Accept(GetString(IDS_AF_CHANGESTRENGTH));
			break;
		case IDC_INFLUENCE_SPIN:
		case IDC_INFLUENCE_EDIT:
			theHold.Accept(GetString(IDS_AF_CHANGEINFLUENCE));
			break;
		case IDC_FALLOFF_SPIN:
		case IDC_FALLOFF_EDIT:
			theHold.Accept(GetString(IDS_AF_CHANGEFALLOFF));
			break;
		}
	}
	ip->RedrawViews(ip->GetTime());
}

void ReactorDlg::Change(BOOL redraw)
	{
	cont->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	UpdateWindow(GetParent(hWnd));	
	if (redraw) ip->RedrawViews(ip->GetTime());
	}


class CheckForNonReactorDlg : public DependentEnumProc {
	public:		
		BOOL non;
		ReferenceMaker *me;
		CheckForNonReactorDlg(ReferenceMaker *m) {non = FALSE;me = m;}
		int proc(ReferenceMaker *rmaker) {
			if (rmaker==me) return 0;
			if (rmaker->SuperClassID()!=REF_MAKER_CLASS_ID &&
				rmaker->ClassID()!=Class_ID(REACTORDLG_CLASS_ID,0x67053d10)) {
				non = TRUE;
				return 1;
				}
			return 0;
			}
	};
void ReactorDlg::MaybeCloseWindow()
	{
	CheckForNonReactorDlg check(cont);
	cont->EnumDependents(&check);
	if (!check.non) {
		PostMessage(hWnd,WM_CLOSE,0,0);
		}
	}



RefResult ReactorDlg::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
     	PartID& partID,  
     	RefMessage message)
	{
	switch (message) {
		case REFMSG_CHANGE:
			Invalidate();			
			break;
		
		case REFMSG_REF_DELETED:
			MaybeCloseWindow();
			break;
		}
	return REF_SUCCEED;
	}


static BOOL CALLBACK ReactorDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	ReactorDlg *dlg = (ReactorDlg*)GetWindowLong(hWnd,GWL_USERDATA);
	int i;
	TCHAR buf[256];

	switch (msg) {
		case WM_INITDIALOG:
			dlg = (ReactorDlg*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			dlg->SetupUI(hWnd);
			if (dlg->cont->vrefs.client){
				dlg->UpdateNodeName();
				dlg->UpdateVarList();
				dlg->iEditBut->SetCheck(dlg->cont->editing);
			}
			break;

		case CC_SPINNER_BUTTONDOWN:
			dlg->SpinnerStart(LOWORD(wParam));
			break;

		case CC_SPINNER_CHANGE:
			dlg->SpinnerChange(LOWORD(wParam),HIWORD(wParam));
			break;

		case WM_CUSTEDIT_ENTER:
			switch (LOWORD(wParam)) 
			{
				case IDC_NAME_EDIT:
					i = SendDlgItemMessage(hWnd, IDC_REACTION_LIST, LB_GETCURSEL, 0, 0);
					if (i>=0)
					{
						dlg->iNameEdit->GetText(buf, 256);
						dlg->cont->reaction[dlg->cont->selected].name = buf;
						dlg->UpdateVarList();						
					}
					break;
				default: dlg->SpinnerEnd(LOWORD(wParam),FALSE);
			}

		case CC_SPINNER_BUTTONUP:
			dlg->SpinnerEnd(LOWORD(wParam),!HIWORD(wParam));
			break;

		case WM_COMMAND:
			dlg->WMCommand(LOWORD(wParam),HIWORD(wParam),(HWND)lParam);						
			break;

		case WM_PAINT:
			dlg->Update();
			return 0;			
		
		case WM_CLOSE:
			DestroyWindow(hWnd);			
			break;

		case WM_DESTROY:
			delete dlg;
			break;
		
		default:
			return FALSE;
		}
	return TRUE;
	}


void Reactor::EditTrackParams(
		TimeValue t,
		ParamDimensionBase *dim,
		TCHAR *pname,
		HWND hParent,
		IObjParam *ip,
		DWORD flags)
	{
	this->ip = ip;
	HWND hCur = FindOpenReactorCtrlWindow(hParent,this);
	if (hCur) 
	{
		SetForegroundWindow(hCur);
		return;
	}

	dlg = new ReactorDlg(this,dim,pname,ip,hParent);
	RegisterReactorCtrlWindow(dlg->hWnd,hParent,this);
	}


