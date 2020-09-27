//***************************************************************************
// CJRender - [cjrender.h] Sample Plugin Renderer for 3D Studio MAX.
// 
// By Christer Janson - Kinetix
//
// May     11, 1996	CCJ Initial coding
// October 28, 1996	CCJ Texture mapping, raycasting etc.
// December    1996	CCJ General cleanup
// May         1997 CCJ Substantial rewrite for Version 2.0
//
// Description:
// Main renderer header.
//
//***************************************************************************

#ifndef __CJRENDER_H__
#define __CJRENDER_H__

// IMPORTANT: If you use this sample, don't forget to change the ClassID!!!
#define CCJREND_CLASS_ID	Class_ID(0x396b66d9, 0x429e6793)
#define DUMMTL_CLASS_ID		Class_ID(0x10707f49, 0x40f5940)

// A Ripoff of the scanline A-buffer render name.
// B-buffer being "Bogus", "Bad" or "Butteriks" (but only a Swede living in
// Gothenburg could possibly understand the last one)

#define RENDERNAME "Sample B-Buffer Renderer"

// By popular demand I'm including an explanation of the
// Butteriks-buffer name here:
// '"Butteriks" is a famous old Gothenburgian 
//  establishment that sells costumes and
//  gadgets to practitioners of the art of
//  comedy and magic. The items are typically
//  characterized as being totally fake,
//  nonsensical, or inherently inferior to
//  the real world thing, while often appearing
//  to be real. As such, one should be wary 
//  of anything carrying the "Butteriks" label.'

// Antialias levels
#define  AA_NONE	0x00
#define  AA_MEDIUM	0x01
#define  AA_HIGH	0x02

#define BIGFLOAT (float(1.0e30))

// G-Buffer flags
// I'm using a BitArray to keep track of the G buffer channels requested.
#define GBUF_Z		 1	// BMM_CHAN_Z      
#define GBUF_MTLID   2	// BMM_CHAN_MTL_ID 
#define GBUF_NODEID  3	// BMM_CHAN_NODE_ID
#define GBUF_UV      4	// BMM_CHAN_UV     
#define GBUF_NORMAL  5	// BMM_CHAN_NORMAL 
#define GBUF_REALPIX 6	// BMM_CHAN_REALPIX
#define GBUF_COVER   7	// BMM_CHAN_COVERAGE	// New for MAX 2.0

class CJRenderer;
class Instance;
class RenderLight;
class SContext;

// Render parameters. Add whatever parameters you need here.
// These are typically parameters that needs to be accessed during the
// setup of the renderer and during the rendering.
class CJRenderParams : public RenderGlobalContext {
public:
	RendType	rendType;				// View, blowup, region etc.
	int			nMinx;
	int			nMiny;
	int			nMaxx;
	int			nMaxy;
	int			nNumDefLights;			// The default lights passed into the renderer
	int			nRegxmin;				// Coords for render blowup etc.
	int			nRegxmax;				// Coords for render blowup etc.
	int			nRegymin;				// Coords for render blowup etc.
	int			nRegymax;				// Coords for render blowup etc.
	Point2		scrDUV;
	BitArray	gbufChan;				// The G buffer channels (bitflags)
	DefaultLight*	pDefaultLights;
	FrameRendParams*	pFrp;			// Frame specific members

	// Custom options
	// These options are specific to the sample renderer
	int			nMaxDepth;
	int			nAntiAliasLevel;
	BOOL		bReflectEnv;

	// Standard options
	// These options are configurable for all plugin renderers
	BOOL		bVideoColorCheck;
	BOOL		bForce2Sided;
	BOOL		bRenderHidden;
	BOOL		bSuperBlack;
	BOOL		bRenderFields;
	BOOL		bNetRender;

	CJRenderParams();
	void		SetDefaults();
	void		ComputeViewParams(const ViewParams&vp);
	Point3		RayDirection(float sx, float sy);

	int				NumRenderInstances();
	RenderInstance*	GetRenderInstance(int i);
};

// For calculation of view to screen coords
class MyView : public View {
public:
	CJRenderParams*	pRendParams;
	Point2		ViewToScreen(Point3 p);
};

// Information about closest hit
struct HitInfo {
	Instance*	instance;
	int			faceNum;
	Point3		baryCoord;
	Point3		normalAtHitPoint;
	Point3		hitPos;
};

class CJRenderMapsContext : public RenderMapsContext {
public:
	CJRenderMapsContext(CJRenderer* r, Instance* i) { cjr = r; inst = i; }
	INode *GetNode();
	int NodeRenderID();
	void GetCurrentViewParams(ViewParams &vp);
	void GetSubRendParams(SubRendParams &srp);
	int SubMtlIndex();
	void SetSubMtlIndex(int mindex);
	void FindMtlPlane(float pl[4]);
	void FindMtlScreenBox(Rect &sbox, Matrix3* viewTM=NULL,int mtlIndex=-1);
	Box3 CameraSpaceBoundingBox();
	Box3 ObjectSpaceBoundingBox();
	Matrix3 ObjectToWorldTM();
	RenderGlobalContext *GetGlobalContext();

	// ClipPlanes is a pointer to an array of Point4's,  each of which
	// represents a clip plane.  nClip Planes is the number of planes (up to 6);
	// The planes are in View space.
	int Render(Bitmap *bm, ViewParams &vp, SubRendParams &srp, Point4 *clipPlanes=NULL, int nClipPlanes=0);

private:
	int subMtl;
	CJRenderer* cjr;
	Instance* inst;
};


// This is the renderer class definition.
class CJRenderer: public Renderer {
public:
	BOOL		bOpen;					// Indicate that Open() has been called
	int			nCurNodeID;				// Node counter
	int			nNumFaces;				// Face counter
	INode*		pScene;					// Root node of scene
	INode*		pViewNode;				// Camera node if available
	ViewParams	view;					// View parameters
	MyView		theView;				// View to screen calculation
	MtlBaseLib	mtls;					// List of all materials

	// These are flags we use so if something is wrong we will report
	// errors once only (and not every frame)
	BOOL		bFirstFrame;			// Indicate that this is the first frame
	BOOL		bUvMessageDone;			// indicate that we have already warned the 
										// user that object requires UV coords
	// G-Buffer channels
	float*		pGbufZ;
	UBYTE*		pGbufMtlID;
	UWORD*		pGbufNodeID;
	Point2*		pGbufUV;
	ULONG*		pGbufNormal;
	RealPixel*	pGbufRealPix;
	UBYTE*		pGbufCov;

	Instance*	ilist;
	Tab<Instance *>		instTab;		// Keep track of instances
	Tab<RenderLight *>	lightTab;		// Keep track of lights

	CJRenderParams	rendParams;			// Render parameters

	CJRenderer();
	void		AddInstance(INode* node);
	void		NodeEnum(INode* node);
	int			RenderImage(CJRenderParams& rp, TimeValue t, Bitmap* tobm, RendProgressCallback *prog);
	BOOL		CastRay(CJRenderParams& rp, Ray* ray, SContext* sc, int depth, Color& col);
	void		GetViewParams(INode* vnode, ViewParams& vp, TimeValue t);
	void		BeginThings();			// Called before rendering has started
	void		EndThings();			// Called after rendering has finished
	int			LoadMapFiles(TimeValue t, HWND hWnd, BOOL firstFrame);
	int			BuildMapFiles(TimeValue t);
	IOResult	Save(ISave *isave);
	IOResult	Load(ILoad *iload);
	RefTargetHandle	Clone(RemapDir &remap);

	int			Open(INode* scene, INode* vnode, ViewParams* viewPar, RendParams& rpar, HWND hwnd, DefaultLight* defaultLights=NULL, int numDefLights=0);
	int			Render(TimeValue t, Bitmap* tobm, FrameRendParams &frp, HWND hwnd, RendProgressCallback* prog, ViewParams* viewPar);
	void		Close(HWND hwnd);

	// Create the use interface dialog
	RendParamDlg*	CreateParamDialog(IRendParams *ir,BOOL prog);
	// Called by Max when Max is reset
	void		ResetParams();
	void		DeleteThis();
	Class_ID	ClassID();
	void		GetClassName(TSTR& s);
};

class BoundingSphere {
public:
	void	Set(Point3 center, float radsq) { bsCenter = center; bsRadSq = radsq; }

	Point3	bsCenter;
	float	bsRadSq;
};

typedef Tab<BoundingSphere*> BoundingSphereTab;

class Instance : public RenderInstance {
public:

	Instance(INode* node, MtlBaseLib* mtls, int nodeID);
	~Instance();

	// Methods inherited from RenderInstance
	RenderInstance*	Next();
	Interval	MeshValidity();
	int			NumLights();
	LightDesc*	Light(int n);
	int			NumShadLights();
	LightDesc*	ShadLight(int n);
	INode*		GetINode();
	Object*		GetEvalObject();
	unsigned long	MtlRequirements(int mtlNum);
	Point3		GetFaceNormal(int faceNum);
	Point3		GetFaceVertNormal(int faceNum, int vertNum);
	void		GetFaceVertNormals(int faceNum, Point3 n[3]);
	Point3		GetCamVert(int vertNum);
	void		GetObjVerts(int fnum, Point3 obp[3]);
	void		GetCamVerts(int fnum, Point3 cp[3]);

	// New for R3
	int			CastsShadowsFrom(const ObjLightDesc& lt);

	// Access methods
	TCHAR*		GetName();

	int			Update(TimeValue t, View& gview, CJRenderer* pRenderer);
	void		UpdateViewTM(Matrix3 affineTM);
	void		TransformGeometry(Matrix3 pointMat, Matrix3 vecMat);
	void		CalcBoundingSphere();
	void		FreeAll();

	Matrix3		normalCamToObj;
	INode*		pNode;
	Object*		pObject;
	Matrix3		objToWorld;
	Box3		camBox;
	Instance*	next;
	BoundingSphereTab	faceBoundSpheres;
};

// The RenderLight is the container for all lights in the scene.
class RenderLight {
public:
        // Constructor for normal lights
        RenderLight(INode* node, MtlBaseLib* mtls);
        // Alternative constructor for default lights
        RenderLight(DefaultLight* light);
        ~RenderLight();

        void Update(TimeValue t, CJRenderer* renderer);
        void UpdateViewDepParams(Matrix3 world2cam);

        INode* pNode;
        ObjLightDesc* pDesc;
        LightObject *pLight;
};


//***************************************************************************
//* Dummy Material : Simple Phong shader using Node color
//* This material is assigned to each node that does not have a material
//* previously assigned. The diffuse color is assigned based on the 
//* wireframe color.
//* This way we can assume that all nodes have a material assigned.
//***************************************************************************

#define DUMSHINE .20f	//.25f
#define DUMSPEC .20f	//.50f

class DumMtl: public Mtl {
	Color diff, spec;
	float phongexp;
	public:
		DumMtl(Color c);
		void Update(TimeValue t, Interval& valid);
		void Reset();
		Interval Validity(TimeValue t);
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);
		Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);
		Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
		float GetShininess(int mtlNum=0, BOOL backFace=FALSE);
		float GetShinStr(int mtlNum=0, BOOL backFace=FALSE);
		float GetXParency(int mtlNum=0, BOOL backFace=FALSE);
		void SetAmbient(Color c, TimeValue t);
		void SetDiffuse(Color c, TimeValue t);
		void SetSpecular(Color c, TimeValue t);
		void SetShininess(float v, TimeValue t);
		Class_ID ClassID();
		void DeleteThis();
    	RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	    	PartID& partID,  RefMessage message);
		void Shade(ShadeContext& sc);
};


//***************************************************************************
//* RendContext is used to evaluate the lights
//***************************************************************************

 class RContext: public RendContext {
	public:
		RContext(CJRenderer *cjr) { renderer = cjr; }
		Matrix3 WorldToCam() const { return renderer->rendParams.worldToCam; }
		Color GlobalLightLevel() const;
		int Progress(int done, int total) {
			return 1;
		}
		CJRenderer *renderer;
};


//***************************************************************************
//* The is the Light descriptor object for default lights
//***************************************************************************
class DefObjLight : public ObjLightDesc 
{
	public:
		Color	intensCol;   // intens*color 
		Point3	lightDir;
		bool	bViewOriented;

		DefObjLight(DefaultLight *l);
		void	DeleteThis() {delete this;}
		int		Update(TimeValue t, const RendContext& rc, RenderGlobalContext* rgc, BOOL shadows, BOOL shadowGeomChanged);
		int		UpdateViewDepParams(const Matrix3& worldToCam);
		BOOL	Illuminate(ShadeContext& sc, Point3& normal, Color& color, Point3 &dir, float &dot_nl, float& diffuseCoef);
};

#endif // __CJRENDER_H__
