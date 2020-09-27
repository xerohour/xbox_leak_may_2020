/**********************************************************************
 *<
	FILE: triobj.cpp

	DESCRIPTION:  Triangle Mesh Object

	CREATED BY: Dan Silva

	HISTORY: created 9 September 1994

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "core.h"
#include "Maxapi.h"
#include "object.h"
#include "triobj.h"
#include "patchobj.h"
#include "mouseman.h"
#include "coremain.h"
#include "tessint.h"
#include "MeshDelta.h"
#ifdef DESIGN_VER
#include "igeomimp.h"
#endif
#include "displace.h"

//#define TRIPIPE_DEBUG

// The first channel to just have a bit for its validity
#define BITVALIDITY_START	5

Class_ID triObjectClassID(TRIOBJ_CLASS_ID,0);

// LOCK
static int lockVar1 = 0;
static int lockVar2 = 0;
static DWORD *lockVarPtr = NULL;

class TriObjectClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 0; }
	void *			Create(BOOL loading) { return new TriObject; }
	const TCHAR *	ClassName() { return _T("MeshObject"); }
	SClass_ID		SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	Class_ID 		ClassID() { return triObjectClassID; }
	const TCHAR* 	Category() { return _T("Primitive");  }
	};

static TriObjectClassDesc triObjDesc;

CoreExport ClassDesc* GetTriObjDescriptor() 
	{ 
	lockVarPtr = COREGetCodeValue();
	return &triObjDesc; 
	}

static ClassDesc* editTriObjDesc = NULL;

void RegisterEditTriObjDesc(ClassDesc *desc)
	{
	editTriObjDesc = desc;
	}

ClassDesc* GetEditTriObjDesc()
	{		
	if (editTriObjDesc) return editTriObjDesc;
	else return &triObjDesc;
	}

TriObject *CreateNewTriObject()
	{
	return (TriObject*)GetEditTriObjDesc()->Create();
	}



void MakeHitRegion(HitRegion& hr, int type, int crossing, int epsi, IPoint2 *p){
	IPoint2 v;
	hr.epsilon = epsi;
	hr.crossing = crossing;
	switch ( type ) {
		case HITTYPE_SOLID:
		case HITTYPE_POINT:
			hr.type = POINT_RGN;
			hr.pt.x = p[0].x;
			hr.pt.y = p[0].y;
			break;
		
		case HITTYPE_BOX:
			hr.type        = RECT_RGN;
			hr.rect.left   = p[0].x;
			hr.rect.right  = p[1].x;
			hr.rect.top    = p[0].y;
			hr.rect.bottom = p[1].y;
			((Box2*)(&hr.rect))->Rectify();
			break;

		case HITTYPE_CIRCLE:
			v = p[0] - p[1];
			hr.type			= CIRCLE_RGN;
			hr.circle.x		= p[0].x;
			hr.circle.y		= p[0].y;
//			hr.circle.r		= (int)sqrt((double)(v.x*v.x + v.y*v.y));
			hr.circle.r		= max(abs(v.x), abs(v.y));	// this is how the circle is drawn!! DB 3/2
			break;

		case HITTYPE_FENCE: {
			int ct = 0;
			while (p[ct].x >= 0)
				ct++;
			hr.type			= FENCE_RGN;
			hr.epsilon		= ct;
			hr.pts			= (POINT *)p;
			break;
			}
		}
	}

#if TRI_MULTI_PROCESSING

int TriObject::refCount = 0;
HANDLE TriObject::defThread = 0;
HANDLE TriObject::defMutex = 0;
HANDLE TriObject::defStartEvent = 0;
HANDLE TriObject::defEndEvent = 0;

struct DefStuff {
	TriObject *		triobj;
	Deformer *		defProc;
	int				ct;
	int				useSel;
	BitArray 		sel;
	float *vssel;
	BOOL 			exit;
} defStuff;

static DWORD WINAPI defFunc(LPVOID ptr)
{
	while(1) {
		WaitForSingleObject(TriObject::defStartEvent, INFINITE);
		if (defStuff.exit) ExitThread(0);
		if(defStuff.useSel) {
			if (defStuff.vssel) {
				for (int i=0; i<defStuff.ct; i++) {
					if(defStuff.sel[i]) {
						defStuff.triobj->SetPoint(i, defStuff.defProc->Map(i,defStuff.triobj->GetPoint(i)));
						continue;
					}
					if (defStuff.vssel[i] == 0) continue;
					Point3 & A = defStuff.triobj->GetPoint(i);
					Point3 dir = defStuff.defProc->Map (i, A) - A;
					defStuff.triobj->SetPoint (i, A+defStuff.vssel[i]*dir);
				}
			} else {
				for(int i = 0; i < defStuff.ct; i++)
					if(defStuff.sel[i])
						defStuff.triobj->SetPoint(i, defStuff.defProc->Map(i,defStuff.triobj->GetPoint(i)));
			}
		}
		else {
			for(int i = 0; i < defStuff.ct; i++)
				defStuff.triobj->SetPoint(i, defStuff.defProc->Map(i,defStuff.triobj->GetPoint(i)));
		}
		SetEvent(TriObject::defEndEvent);
	}
	return 0;
}
#endif



TriObject::TriObject() {

	// LOCK
	if (lockVarPtr && *lockVarPtr & (1<<28)) lockVar1++;

	mesh.EnableEdgeList(1);
	geomValid.SetInfinite();
	vcolorValid.SetInfinite();
	gfxdataValid.SetInfinite();
	topoValid.SetInfinite();
	texmapValid.SetInfinite();
	selectValid.SetInfinite();
	validBits = 0xffffffff;
#if TRI_MULTI_PROCESSING
	if(!refCount++) {
		DWORD threadID;
		defStuff.exit = FALSE;
		defStartEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		defEndEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		defMutex = CreateMutex(NULL, FALSE, NULL);
	   	defThread = CreateThread(NULL, 0, defFunc, (LPVOID)defStartEvent, 0, &threadID);
		}
#endif		

	SetDisplacmentApproxToPreset(0);
	mSubDivideDisplacement = false;
	mSplitMesh = true;
	mDisableDisplacement = false;

	//DebugPrint("TriObject:0x%x\n",this);
	}

TriObject::~TriObject() {
#ifdef TRIPIPE_DEBUG
	DebugPrint ("TriObject(%08x)::Deleting (%08x)\n", this, ~GetChannelLocks());
#endif
	mesh.FreeChannels(~GetChannelLocks(),TRUE);

#if TRI_MULTI_PROCESSING
	if(--refCount == 0) {
		//TerminateThread(defThread, 0);		
		defStuff.exit = TRUE;
		SetEvent(defStartEvent);
		WaitForSingleObject(defThread, INFINITE);

		CloseHandle(defThread);
		CloseHandle(defEndEvent);
		CloseHandle(defStartEvent);
		CloseHandle(defMutex);		
		}
#endif
	
	//DebugPrint("~TriObject:0x%x\n",this);
	}

// From BaseObject

int TriObject::HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt) {
	
	// LOCK
	if (lockVarPtr && *lockVarPtr & (1<<29)) lockVar2++;

	HitRegion hitRegion;
	GraphicsWindow *gw = vpt->getGW();
	MakeHitRegion(hitRegion,type,crossing,4,p);	
	gw->setTransform(inode->GetObjectTM(t));
//	hitRegion.epsilon = 4;	 -- this is wrong! It screws up fence picking! DB 6/28/97
//	hitRegion.crossing = crossing;
	return mesh.select( gw, inode->Mtls(), &hitRegion, flags & HIT_ABORTONHIT, inode->NumMtls() );
	}

void TriObject::Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt) {
	Matrix3 tm = inode->GetObjectTM(t);	
	GraphicsWindow *gw = vpt->getGW();
	gw->setTransform(tm);
	// If we snap, transform the best snap world from its local coords to world
	mesh.snap( gw, snap, p, tm );
	}

int TriObject::Display(TimeValue t, INode *inode, ViewExp* vpt, int flags) {
	Matrix3 tm;
	GraphicsWindow *gw = vpt->getGW();
	gw->setTransform(inode->GetObjectTM(t));
	mesh.render( gw, inode->Mtls(),
		(flags&USE_DAMAGE_RECT) ? &vpt->GetDammageRect() : NULL, 
		COMP_ALL | ((flags&DISP_SHOWSUBOBJECT)?COMP_OBJSELECTED:0),
		inode->NumMtls());
		// RB: The mesh flag COMP_OBJSELECTED is sort of misnamed. When this bit is set, sub object things (like ticks and selected faces) will be drawn.
	
	// LOCK
	if (lockVar1<lockVar2) {
		for (int i=0; i<mesh.getNumVerts(); i++) {
			mesh.verts[i] *= 0.99f;
			}
		}


	return(0);
	}

// From Object

class TriObjCreateCallBack: public CreateMouseCallBack {
	public:
		virtual int proc( ViewExp* vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat );
	};

int TriObjCreateCallBack::proc(ViewExp* vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat) {
	switch ( msg ) {
		case MOUSE_POINT:
			if ( point == 0 ) {
				}
			break;
		case MOUSE_ABORT:
			break;
		case MOUSE_MOVE:			
			break;
		}
	return TRUE;
	}

static TriObjCreateCallBack triCreateCB;

CreateMouseCallBack* TriObject::GetCreateMouseCallBack() {
	return(&triCreateCB);
	}

RefTargetHandle TriObject::Clone(RemapDir& remap) {
	TriObject* newob = new TriObject;
	newob->mesh = mesh;
	newob->mDispApprox = mDispApprox;
	newob->mSubDivideDisplacement = mSubDivideDisplacement;
	newob->mSplitMesh = mSplitMesh;
	newob->mDisableDisplacement = mDisableDisplacement;
	return newob;
	}

// From GeomObject
int TriObject::IntersectRay(TimeValue t, Ray& ray, float& at, Point3& norm)
	{
	return mesh.IntersectRay(ray,at,norm);
	}

ObjectHandle TriObject::CreateTriObjRep(TimeValue t) {
	return(ObjectHandle(this));
	}

void TriObject::GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel ) 
	{	
	if ( tm || useSel ) {
		box.Init();
		BitArray sel;
		float *vssel = NULL;
		if ( useSel ) {
			sel = mesh.VertexTempSel();
			vssel = mesh.getVSelectionWeights ();
			}
		for ( int i = 0; i < mesh.getNumVerts(); i++ ) {
			if ( !useSel || sel[i] || (vssel&&vssel[i])) {
				if ( tm ) {
					box += *tm * mesh.getVert(i);
				} else {
					box += mesh.getVert(i);
					}
				}
			}
	} else {
		box = mesh.getBoundingBox();
		}
	}

void TriObject::GetLocalBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box ) {
	GetDeformBBox(t,box);
	}

void TriObject::GetWorldBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box )
	{
	Box3	meshBox;

	Matrix3 mat = inode->GetObjectTM(t);
	
	GetLocalBoundBox(t,inode,vpt,meshBox);
	if(meshBox.IsEmpty())
		box = meshBox;
	else {
		box.Init();
		for(int i = 0; i < 8; i++)
			box += mat * meshBox[i];
		}
	}

// Reference Managment.
//


// This is only called if the object MAKES references to other things.
RefResult TriObject::NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message ) {
	return(REF_SUCCEED);
	}

ObjectState TriObject::Eval(TimeValue time) {
	return(ObjectState(this));
	}

// get and set the validity interval for the nth channel
Interval TriObject::ChannelValidity(TimeValue t, int nchan) {
	switch(nchan) {
		case GEOM_CHAN_NUM: return geomValid; break;
		case VERT_COLOR_CHAN_NUM: return vcolorValid; break;
		case TOPO_CHAN_NUM: return topoValid; break;
		case TEXMAP_CHAN_NUM: return texmapValid; break;
		case SELECT_CHAN_NUM: return selectValid; break;
		case GFX_DATA_CHAN_NUM: return gfxdataValid; break;
		default:
			return((chMask[nchan]&validBits) ? FOREVER: NEVER);
		}
	}

// the modifier modifies.
void TriObject::SetChannelValidity(int nchan, Interval v) {
	switch(nchan) {
		case GEOM_CHAN_NUM: geomValid = v; break;
		case VERT_COLOR_CHAN_NUM: vcolorValid = v; break;
		case TOPO_CHAN_NUM: topoValid = v; break;
		case TEXMAP_CHAN_NUM: texmapValid = v; break;
		case SELECT_CHAN_NUM: selectValid = v; break;
		case GFX_DATA_CHAN_NUM: gfxdataValid = v; break;
		default :
			//if (v.InInterval(0)) validBits|= chMask[nchan];
			if (!(v==NEVER)) validBits|= chMask[nchan];
			else validBits &= ~chMask[nchan];
			break;
		}
	}	

void TriObject::InvalidateChannels(ChannelMask channels) {
	for (int i=0; i<NUM_OBJ_CHANS; i++) {
		if (channels&chMask[i]) {
			switch(i) {
				case GEOM_CHAN_NUM: geomValid.SetEmpty(); break;
				case VERT_COLOR_CHAN_NUM: vcolorValid.SetEmpty(); break;
				case TOPO_CHAN_NUM: topoValid.SetEmpty(); break;
				case TEXMAP_CHAN_NUM: texmapValid.SetEmpty(); break;
				case SELECT_CHAN_NUM: selectValid.SetEmpty(); break;
				case GFX_DATA_CHAN_NUM: gfxdataValid.SetEmpty(); break;
				default: validBits &= ~chMask[i]; break;
				}
			}
		}
	}

Interval TriObject::ObjectValidity(TimeValue t){
	Interval iv;
	iv.SetInfinite();
	iv &= geomValid;
	iv &= vcolorValid;
	iv &= topoValid;
	iv &= texmapValid;
	iv &= selectValid;
	iv &= gfxdataValid;
	
	if (!(validBits&chMask[SUBSEL_TYPE_CHAN_NUM])) iv.SetEmpty();
	if (!(validBits&chMask[DISP_ATTRIB_CHAN_NUM])) iv.SetEmpty();
	/*
	for (int i=BITVALIDITY_START; i<NUM_OBJ_CHANS; i++)
		if (!(validBits&chMask[i])) {
			iv.SetEmpty();
			break;
			}
	*/
	return iv;
	}

Interval TriObject::ConvertValidity(TimeValue t){
	Interval iv = FOREVER;	
	if (geomValid.InInterval(t)) iv &= geomValid;
	if (vcolorValid.InInterval(t)) iv &= vcolorValid;
	if (topoValid.InInterval(t)) iv &= topoValid;	
	if (texmapValid.InInterval(t)) iv &= texmapValid;	
	if (selectValid.InInterval(t)) iv &= selectValid;	
	return iv;
	}

int TriObject::CanConvertToType(Class_ID cid) {
	if (cid==defObjectClassID) return 1;
	if (cid==mapObjectClassID) return 1;
	if (cid==triObjectClassID) return 1;
	if (cid==patchObjectClassID) return 1;
#ifdef DESIGN_VER
	if (cid == GENERIC_AMSOLID_CLASS_ID) return 1;
#endif
	return 0;
	}


Object* TriObject::ConvertToType(TimeValue t, Class_ID cid) {
	if (cid==defObjectClassID) return this;
	if (cid==mapObjectClassID) return this;
	if (cid==triObjectClassID) return this;
	if (cid==patchObjectClassID) {
		PatchObject *patchob = new PatchObject();
		patchob->patch = mesh;
		patchob->SetChannelValidity(TOPO_CHAN_NUM,ConvertValidity(t));
		patchob->SetChannelValidity(GEOM_CHAN_NUM,ConvertValidity(t));
//		patchob->SetChannelValidity(TEXMAP_CHAN_NUM,ConvertValidity(t));
//		patchob->SetChannelValidity(SELECT_CHAN_NUM,ConvertValidity(t));
		return patchob;
		}
#ifdef DESIGN_VER
	if (cid == GENERIC_AMSOLID_CLASS_ID)
	{
		Object* solid = (Object*)CreateInstance(GEOMOBJECT_CLASS_ID, GENERIC_AMSOLID_CLASS_ID);
		assert(solid);
		if(solid)
		{
			IGeomImp* cacheptr = (IGeomImp*)(solid->GetInterface(I_GEOMIMP));
			assert(cacheptr);
			if(cacheptr)
			{
//				bool res = cacheptr->createConvexHull(mesh.verts, mesh.getNumVerts());
				cacheptr->Init((void*)&mesh, MAX_MESH_ID);
				bool res = !cacheptr->isNull();

				solid->ReleaseInterface(I_GEOMIMP, cacheptr);
				solid->SetChannelValidity(TOPO_CHAN_NUM,ConvertValidity(t));
				solid->SetChannelValidity(GEOM_CHAN_NUM,ConvertValidity(t));
				if(res)
					return solid;
			}
		}
	}
#endif
	return NULL;
	}

BOOL 
TriObject::PolygonCount(TimeValue t, int& numFaces, int& numVerts) 
{
    numFaces = GetMesh().getNumFaces();
    numVerts = GetMesh().getNumVerts();
    return TRUE;
}

typedef int (* GTess)(void *obj, SurfaceType type, Matrix3 *otm, Mesh *mesh,
							TessApprox *tess, TessApprox *disp, View *view,
							Mtl* mtl, BOOL dumpMiFile, BOOL splitMesh);
static GTess psGTessFunc = NULL;

// This function get the function to do GAP Tessellation from
// tessint.dll.  This is required because of the link order between
// core.dll and tessint.dll and gmi.dll.  -- Charlie Thaeler
static void
GetGTessFunction()
{
    if (psGTessFunc)
        return;
    // Get the library handle for tessint.dll
    HINSTANCE hInst = NULL;
	hInst = LoadLibraryEx(_T("tessint.dll"), NULL, 0);
    assert(hInst);

    psGTessFunc = (GTess)GetProcAddress(hInst, _T("GapTessellate"));
	assert(psGTessFunc);
}


Mesh*
TriObject::GetRenderMesh(TimeValue t, INode *inode, View &view, BOOL& needDelete)
{
	if (mDisableDisplacement || !(view.flags & RENDER_MESH_DISPLACEMENT_MAP)) {
		needDelete = FALSE;
		return &mesh;
	}
	// need to check the mesh and see if any face has a matId the requires displacment mapping
	BOOL needDisp = FALSE;

	// Get the material
	Mtl* pMtl = inode ? inode->GetMtl() : NULL;

	if (pMtl) {
		// does the mesh as a whole need it
		if (pMtl->Requirements(mesh.mtlIndex)&MTLREQ_DISPLACEMAP)
			needDisp = TRUE;

		if (!needDisp) {
			for (int f = 0; f < mesh.numFaces; f++) {
				if (pMtl->Requirements(mesh.getFaceMtlIndex(f))&MTLREQ_DISPLACEMAP) {
					needDisp = TRUE;
					break;
				}
			}
		}

		if (needDisp) {
            if (mesh.getNumFaces() == 0)
                return &mesh;

			Matrix3 otm;
			if (inode)
				otm = inode->GetObjectTM(t);
			else
				otm.IdentityMatrix();
			GetGTessFunction();
			if (mSubDivideDisplacement && psGTessFunc) {
				// if we have a material that does displacement mapping and if we can do it
				Mesh *pMesh = new Mesh();
				needDelete = TRUE;
				(*psGTessFunc)((void *)&mesh, MAX_MESH, &otm, pMesh, NULL,
								&mDispApprox, &view, pMtl, FALSE, mSplitMesh);
				needDelete = TRUE;
				return pMesh;
			} else {
				Mesh *pMesh = new Mesh(mesh);
				needDelete = TRUE;

                BOOL hasUVs = pMesh->tvFace != NULL;
				pMesh->buildRenderNormals();

				// now displace the verts
				BitArray vertsSet;
				vertsSet.SetSize(pMesh->numVerts);

				for (int f = 0; f < pMesh->numFaces; f++) {
					Face *pFace = &pMesh->faces[f];
					TVFace *pTVFace = &pMesh->tvFace[f];
					int matid = pFace->getMatID();
					for (int v = 0; v < 3; v++) {
						int vidx = pFace->v[v];
						if (vertsSet[vidx])
							continue; // displace only once
						Point3 norm = pMesh->getNormal(vidx);
						norm.Normalize();
						Point3& vert = pMesh->getVert(vidx);

						UVVert uvvert;
                        if (hasUVs)
                            uvvert = pMesh->getTVert(pTVFace->t[v]);
                        else {
                            uvvert.x = 0.0;
                            uvvert.y = 0.0;
                        }

						pMesh->buildBoundingBox();
						Box3 bbox = pMesh->getBoundingBox();
						float dispScale = Length(bbox.pmax - bbox.pmin)/10.0f;

						float disp = GetDisp(pMtl, pMesh, f, pFace->getMatID(), vert, uvvert.x, uvvert.y, otm) * dispScale;
						vert += (norm * disp);
						vertsSet.Set(vidx);
					}
				}
				return pMesh;
			}
		}
	}

	needDelete = FALSE;
	return &mesh;
}

BOOL
TriObject::CanDoDisplacementMapping()
{
	GetGTessFunction();
	if (psGTessFunc && !mDisableDisplacement)
		return TRUE;
	return FALSE;
}

void
TriObject::DisableDisplacementMapping(BOOL disable)
{
	mDisableDisplacement = disable?true:false;
}


static void
GetCfgFilename( TCHAR *filename ) {
	_tcscpy(filename, GetCOREInterface()->GetDir(APP_MAXROOT_DIR));
	int len = _tcslen(filename);
	if (len) {
	   if (_tcscmp(&filename[len-1],_T("\\")))
		 _tcscat(filename,_T("\\"));

}
	_tcscat(filename, _T("3dsmax.ini"));
}

// The Defaults
TessType sTypes[3] = {TESS_LDA, TESS_LDA, TESS_LDA};
TessSubdivStyle sSubDivs[3] = {SUBDIV_TREE, SUBDIV_TREE, SUBDIV_TREE};
BOOL sViewDeps[3] = {FALSE, FALSE, FALSE};
int sUSteps[3] = {2, 2, 2};
int sVSteps[3] = {2, 2, 2};
float sAngles[3] = {10.0f, 4.0f, 2.0f};
float sDists[3] = {20.0f, 10.0f, 5.0f};
float sEdges[3] = {20.0f, 10.0f, 5.0f};
int sMinSubs[3] = {0, 0, 0};
int sMaxSubs[3] = {2, 3, 4};
int sMaxTris[3] = {20000, 20000, 20000};


static TCHAR* sPresetNames[] = {
    _T("TessPreset1"),
    _T("TessPreset2"),
    _T("TessPreset3"),
};

#include "..\..\cfgmgr\cfgmgr.h"

void
TriObject::SetDisplacmentApproxToPreset(int preset)
{
    TCHAR sectionName[64];
    _stprintf(sectionName, _T("%s%s"), _T("MeshDisp"), sPresetNames[preset]);

    // If no entry in .ini file, then use defaults
    if (!getCfgMgr().sectionExists(sectionName)) {
		mDispApprox.type = sTypes[preset];
		mDispApprox.subdiv = sSubDivs[preset];
		mDispApprox.view = sViewDeps[preset];
		mDispApprox.u = sUSteps[preset];
		mDispApprox.v = sVSteps[preset];
		mDispApprox.ang = sAngles[preset];
		mDispApprox.dist = sDists[preset];
		mDispApprox.edge = sEdges[preset];
		mDispApprox.minSub = sMinSubs[preset];
		mDispApprox.maxSub = sMaxSubs[preset];
		mDispApprox.maxTris = sMaxTris[preset];
        NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
		return;
	}

    getCfgMgr().setSection(sectionName);

    assert(getCfgMgr().keyExists(_T("Type")));
    int t;
    getCfgMgr().getInt(_T("Type"), &t);
    mDispApprox.type = (TessType) t;
    
    assert(getCfgMgr().keyExists(_T("SubdivStyle")));
    int style;
    getCfgMgr().getInt(_T("SubdivStyle"), &style);
    mDispApprox.subdiv = (TessSubdivStyle) style;

    assert(getCfgMgr().keyExists(_T("ViewDep")));
    int viewdep;
    getCfgMgr().getInt(_T("ViewDep"), &viewdep);
    mDispApprox.view = (BOOL) viewdep;

    assert(getCfgMgr().keyExists(_T("USteps")));
    int usteps;
    getCfgMgr().getInt(_T("USteps"), &usteps);
    mDispApprox.u = usteps;

    assert(getCfgMgr().keyExists(_T("VSteps")));
    int vsteps;
    getCfgMgr().getInt(_T("VSteps"), &vsteps);
    mDispApprox.v = vsteps;

    assert(getCfgMgr().keyExists(_T("Angle")));
    float angle;
    getCfgMgr().getFloat(_T("Angle"), &angle);
    mDispApprox.ang = angle;

    assert(getCfgMgr().keyExists(_T("Dist")));
    float dist;
    getCfgMgr().getFloat(_T("Dist"), &dist);
    mDispApprox.dist = dist;

    assert(getCfgMgr().keyExists(_T("Edge")));
    float edge;
    getCfgMgr().getFloat(_T("Edge"), &edge);
    mDispApprox.edge = edge;

    assert(getCfgMgr().keyExists(_T("MinSub")));
    int minSub;
    getCfgMgr().getInt(_T("MinSub"), &minSub);
    mDispApprox.minSub = minSub;

    assert(getCfgMgr().keyExists(_T("MaxSub")));
    int maxSub;
    getCfgMgr().getInt(_T("MaxSub"), &maxSub);
    mDispApprox.maxSub = maxSub;

    assert(getCfgMgr().keyExists(_T("MaxTris")));
    int maxTris;
    getCfgMgr().getInt(_T("MaxTris"), &maxTris);
    mDispApprox.maxTris = maxTris;
    NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
}

void
SetDisplacmentPreset(int preset, TessApprox approx)
{
    TCHAR sectionName[64];
    _stprintf(sectionName, _T("%s%s"), _T("MeshDisp"), sPresetNames[preset]);

    getCfgMgr().setSection(sectionName);

    getCfgMgr().putInt(_T("Type"),        approx.type);
    getCfgMgr().putInt(_T("SubdivStyle"), approx.subdiv);
    getCfgMgr().putInt(_T("ViewDep"),     approx.view);
    getCfgMgr().putInt(_T("USteps"),      approx.u);
    getCfgMgr().putInt(_T("VSteps"),      approx.v);
    getCfgMgr().putFloat(_T("Angle"),     approx.ang);
    getCfgMgr().putFloat(_T("Dist"),      approx.dist);
    getCfgMgr().putFloat(_T("Edge"),      approx.edge);
    getCfgMgr().putInt(_T("MinSub"),      approx.minSub);
    getCfgMgr().putInt(_T("MaxSub"),      approx.maxSub);
    getCfgMgr().putInt(_T("MaxTris"),     approx.maxTris);
}

#define copyFlags(dest, source, mask) dest =  ((dest&(~mask))|(source&mask))

void TriObject::CopyValidity(TriObject *fromOb, ChannelMask channels) {
	if (channels&GEOM_CHANNEL) geomValid = fromOb->geomValid;
	if (channels&VERTCOLOR_CHANNEL) vcolorValid = fromOb->vcolorValid;
	if (channels&TOPO_CHANNEL) topoValid = fromOb->topoValid;
	if (channels&TEXMAP_CHANNEL) texmapValid = fromOb->texmapValid;
	if (channels&SELECT_CHANNEL) selectValid = fromOb->selectValid;
	if (channels&GFX_DATA_CHANNEL) gfxdataValid = fromOb->gfxdataValid;
	copyFlags(validBits, fromOb->validBits,channels);
	}
	 
Object *TriObject::MakeShallowCopy(ChannelMask channels) {
	TriObject* newob = CreateNewTriObject();
#ifdef TRIPIPE_DEBUG
	DebugPrint ("TriObject(%08x)::MakeShallowCopy (%08x): %08x\n", this, channels, newob);
#endif
	newob->mesh.ShallowCopy(&mesh,channels);
	newob->CopyValidity(this,channels);
	newob->mDispApprox = mDispApprox;
	newob->mSubDivideDisplacement = mSubDivideDisplacement;
	newob->mSplitMesh = mSplitMesh;
	newob->mDisableDisplacement = mDisableDisplacement;
	return newob;
	}

void TriObject::ShallowCopy(Object* fromOb, ChannelMask channels) {	
#ifdef TRIPIPE_DEBUG
	DebugPrint ("TriObject(%08x)::ShallowCopy (%08x, %08x)\n", this, fromOb, channels);
#endif
	if (fromOb->IsSubClassOf(triObjectClassID)) {
		TriObject *fob = (TriObject *)fromOb;
		mesh.ShallowCopy(&fob->mesh,channels);
		mDispApprox = fob->mDispApprox;
		mSubDivideDisplacement = fob->mSubDivideDisplacement;
		mSplitMesh = fob->mSplitMesh;
		mDisableDisplacement = fob->mDisableDisplacement;
		CopyValidity(fob,channels);
		}
	}

void TriObject::NewAndCopyChannels(ChannelMask channels) {
#ifdef TRIPIPE_DEBUG
	DebugPrint ("TriObject(%08x)::NewAndCopyChannels (%08x)\n", this, channels);
#endif
	if (channels)
		mesh.NewAndCopyChannels(channels);
	}

void TriObject::FreeChannels(ChannelMask chan) {
#ifdef TRIPIPE_DEBUG
	DebugPrint ("TriObject(%08x)::FreeChannels (%08x)\n", this, chan);
#endif
	mesh.FreeChannels(chan&(~GetChannelLocks()),0);
	}

DWORD TriObject::GetSubselState() {
	return ( (mesh.selLevel&MESH_VERTEX) |
	         (mesh.selLevel&MESH_FACE) |
		     (mesh.selLevel&MESH_EDGE) );
	}

void TriObject::SetSubSelState(DWORD s)
	{
	switch (s) {
		case 0:	mesh.selLevel = MESH_OBJECT; break;
		case 1: mesh.selLevel = MESH_VERTEX; break;
		case 2: mesh.selLevel = MESH_FACE; break;
		case 3: mesh.selLevel = MESH_EDGE; break;
		}
	}

BOOL TriObject::CheckObjectIntegrity()
	{
	for (int i=0; i<mesh.getNumFaces(); i++) {		
		for (int j=0; j<3; j++) {
			if (mesh.faces[i].v[j] >= (DWORD)mesh.getNumVerts()) {
				TSTR buf;
				buf.printf(GetResString(IDS_DB_TRIOBJ_DESC),
					i,j,mesh.faces[i].v[j],mesh.getNumVerts(), mesh.faces, mesh.getNumFaces());
				MessageBox(NULL,buf,GetResString(IDS_DB_INVALID_FACE),MB_ICONEXCLAMATION|MB_TASKMODAL|MB_OK);
				return FALSE;
				}
			}
		}
	
	if (mesh.tvFace) {
		for (int i=0; i<mesh.getNumFaces(); i++) {		
			for (int j=0; j<3; j++) {
				if (mesh.tvFace[i].t[j] >= (DWORD)mesh.numTVerts) {
					TSTR buf;
					buf.printf(GetResString(IDS_DB_TV_DESC), i,j,mesh.tvFace[i].t[j],mesh.numTVerts);
					MessageBox(NULL,buf,GetResString(IDS_DB_INVALID_TV_FACE),MB_ICONEXCLAMATION|MB_TASKMODAL|MB_OK);
					return FALSE;
					}
				}
			}
		}

	return TRUE;
	}

BOOL TriObject::HasUVW() {
	return mesh.tvFace?1:0;
}

BOOL TriObject::HasUVW (int mapChannel) {
	return mesh.mapSupport (mapChannel);
}


BOOL TriObject::IsPointSelected (int i) 
{ 
	BitArray sel = mesh.vertSel;
	return sel[i]?TRUE:FALSE;
}

float TriObject::PointSelection (int i) 
{
	// Get vertex selection weights, if present
	// otherwise, fall back to IsPointSelected
	// HarryD, april-2-99
	float *vssel = mesh.getVSelectionWeights ();
	if (vssel) return vssel[i];
	else return IsPointSelected(i)?1.0f:0.0f;
}

void TriObject::TopologyChanged() { mesh.InvalidateTopologyCache(); mesh.BuildStripsAndEdges(); }

void TriObject::Deform(Deformer *defProc,int useSel) {
	int nv = NumPoints();
	int i;
	if ( useSel ) {
		BitArray sel = mesh.VertexTempSel();
		float *vssel = mesh.getVSelectionWeights ();
#if TRI_MULTI_PROCESSING
		WaitForSingleObject(defMutex, INFINITE);
		defStuff.triobj = this;
		defStuff.defProc = defProc;
		defStuff.ct = nv / 2;
		defStuff.useSel = 1;
		defStuff.sel = sel;
		defStuff.vssel = vssel;
		SetEvent(defStartEvent);
		Sleep(0);
		if (vssel) {
			for (i=nv/2; i<nv; i++) {
				if(sel[i]) {
					SetPoint(i,defProc->Map(i,GetPoint(i)));
					continue;
				}
				if (vssel[i]==0) continue;
				Point3 & A = GetPoint(i);
				Point3 dir = defProc->Map(i,A) - A;
				SetPoint(i,A+vssel[i]*dir);
			}
		} else {
			for (i=nv/2; i<nv; i++) if(sel[i]) SetPoint(i,defProc->Map(i,GetPoint(i)));
		}
		WaitForSingleObject(defEndEvent, INFINITE);
		ReleaseMutex(defMutex);
#else
		if (vssel) {
			for (i=0; i<nv; i++) {
				if(sel[i]) {
					SetPoint(i,defProc->Map(i,GetPoint(i)));
					continue;
				}
				if (vssel[i]==0) continue;
				Point3 & A = GetPoint(i);
				Point3 dir = defProc->Map(i,A) - A;
				SetPoint(i,A+vssel[i]*dir);
			}
		} else {
			for (i=0; i<nv; i++) if (sel[i]) SetPoint(i,defProc->Map(i,GetPoint(i)));
		}
#endif
	} else {
#if TRI_MULTI_PROCESSING
		WaitForSingleObject(defMutex, INFINITE);
		defStuff.triobj = this;
		defStuff.defProc = defProc;
		defStuff.ct = nv / 2;
		defStuff.useSel = 0;
		SetEvent(defStartEvent);
		Sleep(0);
		for (i=nv/2; i<nv; i++) 
			SetPoint(i,defProc->Map(i,GetPoint(i)));
		WaitForSingleObject(defEndEvent, INFINITE);
		ReleaseMutex(defMutex);
#else
		for (i=0; i<nv; i++) 
			SetPoint(i,defProc->Map(i,GetPoint(i)));
#endif
	}
	PointsWereChanged();
} 

#define VALIDITY_CHUNK 	2301
#define VALIDITY_CHUNK2	2303
#define VALIDITY_CHUNK3	2304
#define MESH_CHUNK 2302
#define TESS_APPROX_CHUNK 2305
#define DO_SUBDIV_CHUNK 2306
#define DO_DISPMAP_CHUNK 2307
#define SPLITMESH_CHUNK 2308

IOResult TriObject::Save(ISave *isave) {
	ULONG nb;
	isave->BeginChunk(VALIDITY_CHUNK3);
	isave->Write(&geomValid,sizeof(Interval), &nb);
	isave->Write(&vcolorValid,sizeof(Interval), &nb);
	isave->Write(&topoValid,sizeof(Interval), &nb);
	isave->Write(&texmapValid,sizeof(Interval), &nb);
	isave->Write(&selectValid,sizeof(Interval), &nb);
	isave->Write(&validBits,sizeof(DWORD), &nb);
	isave->EndChunk();

	isave->BeginChunk(MESH_CHUNK);
	mesh.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(TESS_APPROX_CHUNK);
	mDispApprox.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(DO_SUBDIV_CHUNK);
	BOOL subdiv = mSubDivideDisplacement?TRUE:FALSE;
	isave->Write(&subdiv,sizeof(BOOL), &nb);
	isave->EndChunk();

	isave->BeginChunk(SPLITMESH_CHUNK);
	BOOL splitmesh = mSplitMesh?TRUE:FALSE;
	isave->Write(&splitmesh,sizeof(BOOL), &nb);
	isave->EndChunk();

	isave->BeginChunk(DO_DISPMAP_CHUNK);
	BOOL disDisp = mDisableDisplacement?TRUE:FALSE;
	isave->Write(&disDisp,sizeof(BOOL), &nb);
	isave->EndChunk();

	return IO_OK;
	}

IOResult  TriObject::Load(ILoad *iload) {
	ULONG nb;
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case VALIDITY_CHUNK:
				res = iload->Read(&geomValid,sizeof(Interval), &nb);
				res = iload->Read(&topoValid,sizeof(Interval), &nb);
				res = iload->Read(&validBits,sizeof(DWORD), &nb);
				texmapValid = FOREVER;
				selectValid = FOREVER;
				vcolorValid = FOREVER;
				iload->SetObsolete();
				break;
			case VALIDITY_CHUNK2:
				res = iload->Read(&geomValid,sizeof(Interval), &nb);
				res = iload->Read(&topoValid,sizeof(Interval), &nb);
				res = iload->Read(&texmapValid,sizeof(Interval), &nb);
				res = iload->Read(&selectValid,sizeof(Interval), &nb);
				res = iload->Read(&validBits,sizeof(DWORD), &nb);				
				vcolorValid = FOREVER;
				break;
			case VALIDITY_CHUNK3:
				res = iload->Read(&geomValid,sizeof(Interval), &nb);
				res = iload->Read(&vcolorValid,sizeof(Interval), &nb);
				res = iload->Read(&topoValid,sizeof(Interval), &nb);
				res = iload->Read(&texmapValid,sizeof(Interval), &nb);
				res = iload->Read(&selectValid,sizeof(Interval), &nb);
				res = iload->Read(&validBits,sizeof(DWORD), &nb);				
				break;
			case MESH_CHUNK:
				res = mesh.Load(iload);
				break;
			case TESS_APPROX_CHUNK:
				res = mDispApprox.Load(iload);
				break;
			case DO_SUBDIV_CHUNK: {
				BOOL subdiv;
				res = iload->Read(&subdiv, sizeof(BOOL), &nb);
				mSubDivideDisplacement = subdiv ? true:false;
				break; }
			case SPLITMESH_CHUNK: {
				BOOL splitmesh;
				res = iload->Read(&splitmesh, sizeof(BOOL), &nb);
				mSplitMesh = splitmesh ? true:false;
				break; }
			case DO_DISPMAP_CHUNK: {
				BOOL disDisp;
				res = iload->Read(&disDisp, sizeof(BOOL), &nb);				
				mDisableDisplacement = disDisp ? true:false;
				break; }
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	
	// RB 3-18-96:
	// this will strip out faces that either have indices
	// that are out of range or have two or more indices that
	// are equal.
	// Optionally we could put up a warning if either of these
	// two calls return TRUE.
	//	mesh.RemoveDegenerateFaces();	// Removed by SCA 1999.04.28: causes problems loading EMesh Teapots, for example.
	mesh.RemoveIllegalFaces();

	mesh.InvalidateGeomCache();
	mesh.BuildStripsAndEdges();

	return IO_OK;
	}




void TriObject::RescaleWorldUnits(float f) {
	if (TestAFlag(A_WORK1))
		return;
	SetAFlag(A_WORK1);
	for (int i=0; i<mesh.numVerts; i++)
		mesh.verts[i] *= f;	
	mesh.buildBoundingBox();
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}

void* TriObject::GetInterface(ULONG id)
{
#ifdef DESIGN_VER
	typedef void* (*CREATEFUNCPTR)();
	switch(id)
	{
	case I_GEOMIMP:
		{
			HINSTANCE hDllInst = ::LoadLibrary("geomimp.dll");
			if(hDllInst)
			{
				CREATEFUNCPTR CreateInstance = (CREATEFUNCPTR)::GetProcAddress(hDllInst, "CreateMeshAdapter");
				assert(CreateInstance != NULL);
				if(CreateInstance != NULL)
				{
					IGeomImp* ip = 	(IGeomImp*)CreateInstance();
					assert(ip);
					ip->Init(&mesh);
					return ip;
				}
			}
		}
		break;
	default:
		break;
	}
#endif
	return GeomObject::GetInterface(id);
}

void TriObject::ReleaseInterface(ULONG id,void *i)
{
#ifdef DESIGN_VER
	switch(id)
	{
	case I_GEOMIMP:
		{
			//this interface was a tearoff
			delete i;
			return;
		}
		break;
	default:
		break;
	}
#endif
	GeomObject::ReleaseInterface(id, i);
}
