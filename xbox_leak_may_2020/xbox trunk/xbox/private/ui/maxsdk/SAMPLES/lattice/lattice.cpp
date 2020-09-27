/**********************************************************************
 *<
	FILE: lattice.cpp

	DESCRIPTION:  Converts a mesh to a lattice

	CREATED BY: Audrey Peterson

	HISTORY: created 6 January, 1997

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/
#include "mods2.h"
#include "windows.h"

//--- LatticeMod -----------------------------------------------------------
extern HINSTANCE hInstance;

#define MIN_AMOUNT		float(-1.0E30)
#define MAX_AMOUNT		float(1.0E30)
static Class_ID LATTICEOSM_CLASS_ID(0x148132a1, 0x2ed9401c);

typedef struct{
  DWORD vfrom,vto,face;
  int edgeindex;
  Point3 normal;
  UVVert uvfrom,uvto;
}fEdge;


const int row3size=3*sizeof(float);
typedef float Matrix4By4[4][4];
typedef float Matrix3By3[3][3];

void Mult1X4(float *A,Matrix4By4 B,float *C)
{
   C[0]=A[0]*B[0][0]+A[1]*B[1][0]+A[2]*B[2][0]+A[3]*B[3][0];
   C[1]=A[0]*B[0][1]+A[1]*B[1][1]+A[2]*B[2][1]+A[3]*B[3][1];
   C[2]=A[0]*B[0][2]+A[1]*B[1][2]+A[2]*B[2][2]+A[3]*B[3][2];
   C[3]=A[0]*B[0][3]+A[1]*B[1][3]+A[2]*B[2][3]+A[3]*B[3][3];
}

void Mult1X3(float *A,Matrix3By3 B,float *C)
{
   C[0]=A[0]*B[0][0]+A[1]*B[1][0]+A[2]*B[2][0];
   C[1]=A[0]*B[0][1]+A[1]*B[1][1]+A[2]*B[2][1];
   C[2]=A[0]*B[0][2]+A[1]*B[1][2]+A[2]*B[2][2];
}

void SetUpRotation(float *Q, float *W,float Theta,Matrix4By4 Rq)
{ float ww1,ww2,ww3,w12,w13,w23,CosTheta,SinTheta,MinCosTheta;
  Point3 temp;
  Matrix3By3 R;

 ww1=W[0]*W[0];ww2=W[1]*W[1];ww3=W[2]*W[2];
 w12=W[0]*W[1];w13=W[0]*W[2];w23=W[1]*W[2];
 CosTheta=(float)cos(Theta);MinCosTheta=1.0f-CosTheta;SinTheta=(float)sin(Theta);
 R[0][0]=ww1+(1.0f-ww1)*CosTheta;
 R[0][1]=w12*MinCosTheta+W[2]*SinTheta;
 R[0][2]=w13*MinCosTheta-W[1]*SinTheta;
 R[1][0]=w12*MinCosTheta-W[2]*SinTheta;
 R[1][1]=ww2+(1.0f-ww2)*CosTheta;
 R[1][2]=w23*MinCosTheta+W[0]*SinTheta;
 R[2][0]=w13*MinCosTheta+W[1]*SinTheta;
 R[2][1]=w23*MinCosTheta-W[0]*SinTheta;
 R[2][2]=ww3+(1.0f-ww3)*CosTheta;
 Mult1X3(Q,R,&temp.x);
 memcpy(Rq[0],R[0],row3size);memcpy(Rq[1],R[1],row3size);memcpy(Rq[2],R[2],row3size);
 Rq[3][0]=Q[0]-temp.x;Rq[3][1]=Q[1]-temp.y;Rq[3][2]=Q[2]-temp.z;
 Rq[0][3]=Rq[1][3]=Rq[2][3]=0.0f;Rq[3][3]=1.0f;
}
void RotateOnePoint(float *Pin,float *Q, float *W,float Theta)
{ Matrix4By4 Rq;
  float Pout[4],Pby4[4];

 SetUpRotation(Q,W,Theta,Rq);
 memcpy(Pby4,Pin,row3size);Pby4[3]=1.0f;
 Mult1X4(Pby4,Rq,Pout);
 memcpy(Pin,Pout,row3size);
}

class LatticeMod: public Modifier {
	
	protected:
		static IObjParam *ip;
		
	public:
		static IParamMap *pmapParam;
		static LatticeMod *lattMod;
		IParamBlock *pblock;
		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) { s= GetString(IDS_AP_LATTICE); }  
		virtual Class_ID ClassID() { return LATTICEOSM_CLASS_ID;}		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		TCHAR *GetObjectName() { return GetString(IDS_AP_LATTICE); }
		IOResult Load(ILoad *iload);

		LatticeMod();
		virtual ~LatticeMod();

		ChannelMask ChannelsUsed()  { return PART_GEOM|PART_TOPO; }
		// Possible GOTCHA -- Modifiers like this one, which completely change the type of
		// object change ALL channels!  Be sure to tell the system!
		ChannelMask ChannelsChanged() { return PART_ALL; }
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Class_ID InputType() {return triObjectClassID;}
		Interval LocalValidity(TimeValue t);

		// From BaseObject
		BOOL ChangeTopology() {return TRUE;}
		IParamArray *GetParamBlock() {return pblock;}
		int GetParamBlockIndex(int id) {return id;}

		int NumRefs() {return 1;}
		RefTargetHandle GetReference(int i) {return pblock;}
		void SetReference(int i, RefTargetHandle rtarg) {pblock=(IParamBlock*)rtarg;}

 		int NumSubs() { return 1; }  
		Animatable* SubAnim(int i) { return pblock; }
		TSTR SubAnimName(int i) { return TSTR(GetString(IDS_AP_PARAMETERS));}		

		RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message);

		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 

		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);

		void BuildMeshFromShape(TimeValue t,ModContext &mc, ObjectState * os, Mesh &mesh);

		void UpdateUI(TimeValue t) {}
		Interval GetValidity(TimeValue t);
		ParamDimension *GetParameterDim(int pbIndex);
		TSTR GetParameterName(int pbIndex);
	};

class LatticeClassDesc:public ClassDesc {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new LatticeMod; }
	const TCHAR *	ClassName() { return GetString(IDS_AP_LATTICE); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return  LATTICEOSM_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_AP_DEFDEFORMATIONS);}
	void			ResetClassParams(BOOL fileReset);
	};

static LatticeClassDesc LatticeDesc;
ClassDesc* GetLatticeModDesc() { return &LatticeDesc; }

IObjParam*		LatticeMod::ip          = NULL;
IParamMap *		LatticeMod::pmapParam = NULL;
LatticeMod *	LatticeMod::lattMod = NULL;

void LatticeClassDesc::ResetClassParams(BOOL fileReset)
	{
	}

//--- Parameter map/block descriptors -------------------------------

#define PB_GEOMETRY		0
#define PB_SRADIUS		1
#define PB_SSEGS		2
#define PB_SSIDES		3
#define PB_EDGES		4
#define PB_ENDCAPS		5
#define PB_SSMOOTH		6
#define PB_JRADIUS		7
#define PB_JSEGS		8
#define PB_GEOBASE		9
#define PB_JSMOOTH		10
#define PB_MAPPING		11
#define PB_STRUTMTL		12
#define PB_JUNCTMTL		13
#define PB_ENTIREOBJECT	14

static int geometryIDs[] = {IDC_AP_STRUTSON,IDC_AP_JUNCTSON,IDC_AP_BOTHON};
static int geobasicIDs[] = {IDC_AP_TYPETET,IDC_AP_TYPEOCT,IDC_AP_TYPEICOSA};
static int mappingIDs[] = {IDC_AP_MAPNONE,IDC_AP_MAPOLD,IDC_AP_MAPNEW};

//
//
// Parameters
static ParamUIDesc descParam[] = {
	// Geometry
	ParamUIDesc(PB_GEOMETRY,TYPE_RADIO,geometryIDs,3),

	// Amount
	ParamUIDesc(
		PB_SRADIUS,
		EDITTYPE_UNIVERSE,
		IDC_AP_STRUTRADIUS,IDC_AP_STRUTRADIUSSPIN,
		0.0f,MAX_AMOUNT,
		0.5f),

	// Segments
	ParamUIDesc(
		PB_SSEGS,
		EDITTYPE_INT,
		IDC_AP_STRUTSEGMENTS,IDC_AP_STRUTSEGMENTSSPIN,
		1.0f,100.0f,
		0.5f),

	// Sides
	ParamUIDesc(
		PB_SSIDES,
		EDITTYPE_INT,
		IDC_AP_STRUTSIDES,IDC_AP_STRUTSIDESSPIN,
		3.0f,100.0f,
		0.5f),

	// Edges
	ParamUIDesc(PB_EDGES,TYPE_SINGLECHEKBOX,IDC_AP_IGNOREHIDDEN),

	// EndCaps
	ParamUIDesc(PB_ENDCAPS,TYPE_SINGLECHEKBOX,IDC_AP_ENDCAPS),

	// Strut Smooth
	ParamUIDesc(PB_SSMOOTH,TYPE_SINGLECHEKBOX,IDC_AP_STRUTSMOOTH),

	// Amount
	ParamUIDesc(
		PB_JRADIUS,
		EDITTYPE_UNIVERSE,
		IDC_AP_JUNCTRADIUS,IDC_AP_JUNCTRADIUSSPIN,
		0.0f,MAX_AMOUNT,
		0.5f),

	// Segments
	ParamUIDesc(
		PB_JSEGS,
		EDITTYPE_INT,
		IDC_AP_JUNCTSEGMENTS,IDC_AP_JUNCTSEGMENTSSPIN,
		1.0f,100.0f,
		0.5f),

	// Geo base type
	ParamUIDesc(PB_GEOBASE,TYPE_RADIO,geobasicIDs,3),

	// Texture coords
	ParamUIDesc(PB_JSMOOTH,TYPE_SINGLECHEKBOX,IDC_AP_JUNCTSMOOTH),

	// Output type
	ParamUIDesc(PB_MAPPING,TYPE_RADIO,mappingIDs,3),

	// StrutMtl
	ParamUIDesc(
		PB_STRUTMTL,
		EDITTYPE_INT,
		IDC_AP_STRUTMATID,IDC_AP_STRUTMATIDSPIN,
		1.0f,255.0f,
		1.0f),

	// JunctMtl
	ParamUIDesc(
		PB_JUNCTMTL,
		EDITTYPE_INT,
		IDC_AP_JUNCTMATID,IDC_AP_JUNCTMATIDSPIN,
		1.0f,255.0f,
		1.0f),

	// Texture coords
	ParamUIDesc(PB_ENTIREOBJECT,TYPE_SINGLECHEKBOX,IDC_AP_ENTIREOBJECT),
};

#define PARAMDESC_LENGTH 15

static ParamBlockDescID descVer0[] = {
	{ TYPE_INT, NULL, FALSE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },
	{ TYPE_INT, NULL, TRUE, 2 },
	{ TYPE_INT, NULL, TRUE, 3 },
	{ TYPE_INT, NULL, FALSE, 4 },
	{ TYPE_INT, NULL, FALSE, 5 },
	{ TYPE_INT, NULL, FALSE, 6 },
	{ TYPE_FLOAT, NULL, TRUE, 7 },
	{ TYPE_INT, NULL, TRUE, 8 },
	{ TYPE_INT, NULL, FALSE, 9 },
	{ TYPE_INT, NULL, FALSE, 10 },
	{ TYPE_INT, NULL, FALSE, 11 },
	{ TYPE_INT, NULL, FALSE, 12 },
	{ TYPE_INT, NULL, FALSE, 13 },
 };
static ParamBlockDescID descVer1[] = {
	{ TYPE_INT, NULL, FALSE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },
	{ TYPE_INT, NULL, TRUE, 2 },
	{ TYPE_INT, NULL, TRUE, 3 },
	{ TYPE_INT, NULL, FALSE, 4 },
	{ TYPE_INT, NULL, FALSE, 5 },
	{ TYPE_INT, NULL, FALSE, 6 },
	{ TYPE_FLOAT, NULL, TRUE, 7 },
	{ TYPE_INT, NULL, TRUE, 8 },
	{ TYPE_INT, NULL, FALSE, 9 },
	{ TYPE_INT, NULL, FALSE, 10 },
	{ TYPE_INT, NULL, FALSE, 11 },
	{ TYPE_INT, NULL, FALSE, 12 },
	{ TYPE_INT, NULL, FALSE, 13 },
	{ TYPE_INT, NULL, FALSE, 14 },
 };

#define PBLOCK_LENGTH	15

static ParamVersionDesc versions[] = {
	ParamVersionDesc(descVer0,14,0),
	};
static ParamBlockDescID *currentver=descVer1;
#define NUM_OLDVERSIONS	1

// Current version
#define CURRENT_VERSION	1
static ParamVersionDesc curVersion(currentver,PBLOCK_LENGTH,CURRENT_VERSION);

LatticeMod::LatticeMod()
	{
	MakeRefByID(FOREVER, 0, 
		CreateParameterBlock(currentver, PBLOCK_LENGTH, CURRENT_VERSION));
	pblock->SetValue(PB_GEOMETRY, TimeValue(0), 2);
	pblock->SetValue(PB_SRADIUS, TimeValue(0), 2.0f);
	pblock->SetValue(PB_SSEGS	, TimeValue(0), 1);
	pblock->SetValue(PB_SSIDES, TimeValue(0), 4);
	pblock->SetValue(PB_EDGES, TimeValue(0), 1);
	pblock->SetValue(PB_ENDCAPS, TimeValue(0), 0);
	pblock->SetValue(PB_SSMOOTH, TimeValue(0), 0);
	pblock->SetValue(PB_JRADIUS, TimeValue(0), 5.0f);
	pblock->SetValue(PB_JSEGS, TimeValue(0), 1);
	pblock->SetValue(PB_GEOBASE, TimeValue(0), 1);
	pblock->SetValue(PB_JSMOOTH, TimeValue(0), 0);
	pblock->SetValue(PB_MAPPING, TimeValue(0), 1);
	pblock->SetValue(PB_STRUTMTL, TimeValue(0), 1);
	pblock->SetValue(PB_JUNCTMTL, TimeValue(0), 2);
	pblock->SetValue(PB_ENTIREOBJECT, 0,1);
	}

LatticeMod::~LatticeMod()
	{	
	}

const float HalfIntMax=16383.5f;
/* number between -1 and 1 */
float RND11()
{ float num;

   num=(float)rand()-HalfIntMax;
   return(num/HalfIntMax);
}

Interval LatticeMod::LocalValidity(TimeValue t)
	{
	// if being edited, return NEVER forces a cache to be built 
	// after previous modifier.
	if (TestAFlag(A_MOD_BEING_EDITED))
		return NEVER;  
	Interval valid = GetValidity(t);	
	return valid;
	}

RefTargetHandle LatticeMod::Clone(RemapDir& remap)
	{
	LatticeMod* newmod = new LatticeMod();	
	newmod->ReplaceReference(0,pblock->Clone(remap));
	return(newmod);
	}

 static TriObject *IsUseable(Object *pobj,TimeValue t)
{ 
  if (pobj->SuperClassID()==GEOMOBJECT_CLASS_ID)
  {	if (pobj->IsSubClassOf(triObjectClassID)) 
      return (TriObject*)pobj;
    else 
	{ if (pobj->CanConvertToType(triObjectClassID)) 
	  	return (TriObject*)pobj->ConvertToType(t,triObjectClassID);			
	}
  }
  return NULL;
}

void Checkfordup(DWORD *curface,int P0,int P1,fEdge *edgelst,int *edges,int face,Mesh &mesh,BOOL douv)
{ int i,found;

  found=i=0;
  while ((!found)&&(i<*edges))
   { found=((edgelst[i].vto==curface[P0])&&(edgelst[i].vfrom==curface[P1]))||
            ((edgelst[i].vfrom==curface[P0])&&(edgelst[i].vto==curface[P1]));
     if (!found) i++;
   }
   if (!found)
   { edgelst[i].vfrom=curface[P0];edgelst[i].vto=curface[P1];
     edgelst[i].face=face;edgelst[i].edgeindex=P0;
	 if (douv)
	 { edgelst[i].uvfrom=mesh.tVerts[mesh.tvFace[face].t[P0]];
	   edgelst[i].uvto=mesh.tVerts[mesh.tvFace[face].t[P1]];
	 }
    (*edges)++;
   }
}

void GetEdges(Mesh &mesh,fEdge *edgelst,int *edges,int face,BOOL useall,BOOL douv,BOOL all,BitArray subedgelst)
{ DWORD *curface=&(mesh.faces[face]).v[0],flags=mesh.faces[face].flags;
  int cnt=face*3;
  if (all?(useall||((flags & EDGE_A)>0)):subedgelst[cnt])
    Checkfordup(curface,0,1,edgelst,edges,face,mesh,douv);
  if (all ?(useall||((flags & EDGE_B)>0)):subedgelst[cnt+1])
    Checkfordup(curface,1,2,edgelst,edges,face,mesh,douv);
  if (all ?(useall||((flags & EDGE_C)>0)):subedgelst[cnt+2])
    Checkfordup(curface,2,0,edgelst,edges,face,mesh,douv);
}

void SphericalInterpolate (Mesh& amesh, int v1, int v2, int *current, int num)
{	int i;
	float theta, theta1, theta2, sn, cs, rad;
	Point3 a, b, c;

	if (num<2) { return; }
	a=amesh.getVert (v1);
	b=amesh.getVert (v2);
	rad = DotProd (a,a);
	if (rad==0) 
	{	for (i=1; i<num; i++) amesh.setVert ((*current)++, a);
		return;
	}
	cs = DotProd (a,b) / rad;
	LimitValue (cs, -1.0f, 1.0f);
	theta = (float) acos (cs);
	sn = (float) sin (theta);

	for (i=1; i<num; i++) 
	{	theta1 = (theta*i)/num;
		theta2 = (theta*(num-i))/num;
		c = (a*((float)sin(theta2))+b*((float)sin(theta1)))/sn;
		amesh.setVert ((*current)++, c);
	}
}

int tetra_find_vert_index (int s, int f, int r, int c)
{						// segs, face, row, column.
	if (r==0) {	return (f<3?0:1); }
	if (c==0) {	if (r==s)  return(f<3?f+1:3); 
	  return (f<3?4+(s-1)*f+r-1:4+(s-1)*4+r-1); 
	}
	if (c==r) 
	{	if (r==s) {	return (f<3?(f+1)%3+1:2); }
		return(f<3?4+(s-1)*((f+1)%3)+r-1:4+(s-1)*3+r-1); 
	}
	if (r==s)
	{switch (f) {
		case 0: return 4+(s-1)*3 + c-1;
		case 1: return 4+(s-1)*5 + c-1;
		case 2:	return 4+(s-1)*4 + s-1-c;
		case 3:	return 4+(s-1)*5 + s-1-c;
		}
	}
	return 4 + (s-1)*6 + f*(s-1)*(s-2)/2 + (r-2)*(r-1)/2 + c-1;
}

int octa_find_vert_index (int s, int f, int r, int c)
{						// segs, face, row, column.
	if (r==0)  // Top corner of face
	  	return (f<4?0:5);	
	if (r==s) 
	{  	if (((f<4)&&(c==0)) || ((f>3)&&(c==r))) return f%4+1;
		if (((f>3)&&(c==0)) || ((f<4)&&(c==r))) return (f+1)%4+1;
		if (f<4) return 6+(s-1)*(8+f)+c-1;
		return 6+(s-1)*(4+f)+s-1-c;
	}
	if (c==0)  // r is between 0 and s.
		return (f<4?6+(s-1)*f + r-1:6+(s-1)*((f+1)%4+4) + r-1);		
	if (c==r) return(f<4?6+(s-1)*((f+1)%4) + r-1:6+(s-1)*f + r-1);
	return 6 + (s-1)*12 + f*(s-1)*(s-2)/2 + (r-1)*(r-2)/2 + c-1;
}

int icosa_find_vert_index (int s, int f, int r, int c)
{							// segs, face, row, column
	if (r==0) 	// Top corner of face
		return(f<5?0:(f>14?11:f-4));
	if ((r==s) && (c==0)) { // Lower left corner of face
		if (f<5) { return f+1; }
		if (f<10) { return (f+4)%5 + 6; }
		if (f<15) { return (f+1)%5 + 1; }
		return (f+1)%5 + 6;
	}
	if ((r==s) && (c==s)) { // Lower right corner
		if (f<5) { return (f+1)%5+1; }
		if (f<10) { return f+1; }
		if (f<15) { return f-9; }
		return f-9;
	}
	if (r==s) { // Bottom edge
		if (f<5) { return 12 + (5+f)*(s-1) + c-1; }
		if (f<10) { return 12 + (20+(f+4)%5)*(s-1) + c-1; }
		if (f<15) { return 12 + (f-5)*(s-1) + s-1-c; }
		return 12 + (5+f)*(s-1) + s-1-c;
	}
	if (c==0) { // Left edge
		if (f<5) { return 12 + f*(s-1) + r-1; }
		if (f<10) { return 12 + (f%5+15)*(s-1) + r-1; }
		if (f<15) { return 12 + ((f+1)%5+15)*(s-1) + s-1-r; }
		return 12 + ((f+1)%5+25)*(s-1) + r-1;
	}
	if (c==r) { // Right edge
		if (f<5) { return 12 + ((f+1)%5)*(s-1) + r-1; }
		if (f<10) { return 12 + (f%5+10)*(s-1) + r-1; }
		if (f<15) { return 12 + (f%5+10)*(s-1) + s-1-r; }
		return 12 + (f%5+25)*(s-1) + r-1;
	}
	// Not an edge or corner.
	return 12 + 30*(s-1) + f*(s-1)*(s-2)/2 + (r-1)*(r-2)/2 + c-1;
}

int find_vert_index (int basetype, int segs, int face, int row, int column)
{ 	switch (basetype) {
	case 0: return tetra_find_vert_index (segs, face, row, column);
	case 1: return octa_find_vert_index (segs, face, row, column);
	default: return icosa_find_vert_index (segs, face, row, column);
	}
}

void DoHedron(float radius,Mesh& mesh,int nv,int segs,int basetype,int nfaces,BOOL smooth,int nf,int mtlid)
{int nsections,i,face,bv=nv,b1=nv+1;
 switch (basetype)
 {	case 0: // Based on tetrahedron
		// First four tetrahedral vertices 
	{	nsections=4;
		mesh.setVert (nv++, (float)0.0, (float)0.0, radius);
		mesh.setVert (nv++, radius*((float) sqrt(8./9.)), (float)0.0, -radius/((float)3.));
		mesh.setVert (nv++, -radius*((float) sqrt(2./9.)), radius*((float) sqrt(2./3.)), -radius/((float)3.));
		mesh.setVert (nv++, -radius*((float) sqrt(2./9.)), -radius*((float) sqrt(2./3.)), -radius/((float)3.));

		// Edge vertices
		int b2=bv+2,b3=bv+3;
		SphericalInterpolate (mesh, bv, b1, &nv, segs);
		SphericalInterpolate (mesh, bv, b2, &nv, segs);
		SphericalInterpolate (mesh, bv, b3, &nv, segs);
		SphericalInterpolate (mesh, b1, b2, &nv, segs);
		SphericalInterpolate (mesh, b1, b3, &nv, segs);
		SphericalInterpolate (mesh, b2, b3, &nv, segs);

		// Face vertices
		int b4=4+bv;
		for (i=1; i<segs-1; i++) SphericalInterpolate (mesh, b4+i, b4+(segs-1)+i, &nv, i+1);
		for (i=1; i<segs-1; i++) SphericalInterpolate (mesh, b4+(segs-1)+i, b4+2*(segs-1)+i, &nv, i+1);
		for (i=1; i<segs-1; i++) SphericalInterpolate (mesh, b4+2*(segs-1)+i, b4+i, &nv, i+1);
		for (i=1; i<segs-1; i++) SphericalInterpolate (mesh, b4+4*(segs-1)+i, b4+3*(segs-1)+i, &nv, i+1);
	}
		break;

	case 1: //Based on the Octahedron
		// First 6 octahedral vertices
	{	nsections=8;
		mesh.setVert (nv++, (float)0, (float)0, radius);
		mesh.setVert (nv++, radius, (float)0, (float)0);
		mesh.setVert (nv++, (float)0, radius, (float)0);
		mesh.setVert (nv++, -radius, (float)0, (float)0);
		mesh.setVert (nv++, (float)0, -radius, (float)0);
		mesh.setVert (nv++, (float)0, (float)0, -radius);

		// Edge vertices
		for (face=0; face<4; face++) SphericalInterpolate (mesh, bv, face+b1, &nv, segs);
		for (face=0; face<4; face++) SphericalInterpolate (mesh, bv+5, face+b1, &nv, segs);
		for (face=0; face<4; face++) SphericalInterpolate (mesh, face+b1, (face+1)%4+b1, &nv, segs);
		 int b6=bv+6;
		// Face vertices
		for (face=0; face<4; face++) 
		 for (i=1; i<segs-1; i++) 
			 SphericalInterpolate (mesh, b6+face*(segs-1)+i, b6+((face+1)%4)*(segs-1)+i, &nv, i+1);
		for (face=0; face<4; face++)
		 for (i=1; i<segs-1; i++)
			 SphericalInterpolate (mesh, b6+((face+1)%4+4)*(segs-1)+i, b6+(face+4)*(segs-1)+i, &nv, i+1);
	}
		break;

	case 2:  // Based on the Icosahedron
		// First 12 icosahedral vertices
	{	float subz,subrad,theta;
		nsections=20;
		mesh.setVert (nv++, (float)0, (float)0, radius);
		subz = (float) sqrt (.2) * radius;
		subrad = 2*subz;
		for (face=0; face<5; face++) {
			theta = 2*PI*face/5;
			mesh.setVert (nv++, subrad*((float)cos(theta)), subrad*((float)sin(theta)), subz);
		}
		for (face=0; face<5; face++) {
			theta = PI*(2*face+1)/5;
			mesh.setVert (nv++, subrad*((float)cos(theta)), subrad*((float)sin(theta)), -subz);
		}
 		mesh.setVert (nv++, (float)0, (float)0, -radius);

		int b6=bv+6,b12=bv+12;
		// Edge vertices: 6*5*(segs-1) of these.
		for (face=0; face<5; face++) SphericalInterpolate (mesh, bv, face+b1, &nv, segs);
		for (face=0; face<5; face++) SphericalInterpolate (mesh, face+b1, (face+1)%5+b1, &nv, segs);
		for (face=0; face<5; face++) SphericalInterpolate (mesh, face+b1, face+b6, &nv, segs);
		for (face=0; face<5; face++) SphericalInterpolate (mesh, face+b1, (face+4)%5+b6, &nv, segs);
		for (face=0; face<5; face++) SphericalInterpolate (mesh, face+b6, (face+1)%5+b6, &nv, segs);
		for (face=0; face<5; face++) SphericalInterpolate (mesh, bv+11, face+b6, &nv, segs);
		
		// Face vertices: 4 rows of 5 faces each.
		for (face=0; face<5; face++) 
			for (i=1; i<segs-1; i++) 
				SphericalInterpolate (mesh, b12+face*(segs-1)+i, b12+((face+1)%5)*(segs-1)+i, &nv, i+1);
		for (face=0; face<5; face++) 
			for (i=1; i<segs-1; i++) 
				SphericalInterpolate (mesh, b12+(face+15)*(segs-1)+i, b12+(face+10)*(segs-1)+i, &nv, i+1);
		for (face=0; face<5; face++) 
		 for (i=1; i<segs-1; i++) 
		   SphericalInterpolate (mesh, b12+((face+1)%5+15)*(segs-1)+segs-2-i, b12+(face+10)*(segs-1)+segs-2-i, &nv, i+1);
		for (face=0; face<5; face++) 
			for (i=1; i<segs-1; i++) 
				SphericalInterpolate (mesh, b12+((face+1)%5+25)*(segs-1)+i, b12+(face+25)*(segs-1)+i, &nv, i+1);
		}
		break;
	}

	// Now make faces 
	// Set all smoothing, edge flags
	int fcount=nf,sgroup(smooth?1:0);
	for (i=0; i<nfaces; i++) 
	{ if (mtlid>=0) mesh.faces[fcount].setMatID(mtlid);
	  mesh.faces[fcount].setEdgeVisFlags(1,1,1);
	  mesh.faces[fcount++].setSmGroup(sgroup);
	}
	int row,column,a,b,c,d;
	for (face=0; face<nsections; face++)
	{ for (row=0; row<segs; row++)
		{  for (column=0; column<=row; column++) 
			{	a = bv+find_vert_index (basetype, segs, face, row, column);
				b = bv+find_vert_index (basetype, segs, face, row+1, column);
				c = bv+find_vert_index (basetype, segs, face, row+1, column+1);
				mesh.faces[nf].setVerts (a,b,c);
				nf++;
				if (column<row)
				{	d = bv+find_vert_index (basetype, segs, face, row, column+1);
					mesh.faces[nf].setVerts (a,c,d);
					nf++;
				}
			}
		}
	}
}

BOOL VertexinWhichFace(int v,Face *flst,int infaces,int *aface,int *isv)
{ BOOL found;

  found=0;
  while ((*aface<infaces)&&(!found))
  { *isv=0;
    while ((!found)&&(*isv<3))
    {  found=(flst[*aface].v[*isv]==(DWORD)v);
      if (!found) (*isv)++;
    }
    if (!found) (*aface)++;
  }
  return(found);
}

void LatticeMod::ModifyObject(TimeValue t, ModContext &mc, ObjectState * os, INode *node) 
	{TriObject *triOb;
	if ((triOb=IsUseable(os->obj,t))==NULL) return;

	// Get our personal validity interval...
	Interval valid = GetValidity(t);
	// and intersect it with the channels we use as input (see ChannelsUsed)
	valid &= os->obj->ChannelValidity(t,TOPO_CHAN_NUM);
	valid &= os->obj->ChannelValidity(t,GEOM_CHAN_NUM);
	BOOL whole=FALSE;
	pblock->GetValue(PB_ENTIREOBJECT,0,whole,FOREVER); 
	int edges=0,inverts=triOb->GetMesh().getNumVerts(),infaces=triOb->GetMesh().getNumFaces();
	int numedges=infaces*3,tverts=triOb->GetMesh().getNumTVerts();
	int subvs=0;
	BOOL useall;
	int alledges;
	pblock->GetValue(PB_EDGES,0,alledges,FOREVER); useall=(alledges==0);
	BitArray subedgelst,vertSel,faceSel,fmuststay,vmuststay;
	int basev=0,basef=0;
	Face *fstaylst=NULL;TVFace *tvfstaylst=NULL;UVVert *tvstaylst=NULL;
	int addmap,oldtvs=0;
	pblock->GetValue(PB_MAPPING,0,addmap,FOREVER); 
	if ((addmap==1)&&(tverts==0)) addmap=0;
	BitArray savedtvlst;int selLevel=triOb->GetMesh().selLevel;
	BOOL submesh=(!whole)&&(selLevel!=MESH_OBJECT);
 	int edgesize=infaces*3;
	subedgelst.SetSize(edgesize);subedgelst.ClearAll();
	if (submesh)
	{ faceSel.SetSize(infaces);faceSel.ClearAll();
	  vertSel.SetSize(inverts);vertSel.ClearAll();
	  if (selLevel==MESH_FACE) faceSel=triOb->GetMesh().faceSel; 
	  if (selLevel==MESH_VERTEX) vertSel=triOb->GetMesh().vertSel; 
	  if (selLevel==MESH_EDGE) subedgelst=triOb->GetMesh().edgeSel;
	  int edgenum=0;
	  fmuststay.SetSize(infaces);fmuststay.ClearAll();
	  vmuststay.SetSize(inverts);vmuststay.ClearAll();
	  BOOL visonly=!useall;
	  BOOL cull=(subedgelst.NumberSet()) && visonly;
	  for (int fcnt=0;(fcnt<infaces);fcnt++)
	  { DWORD flags=triOb->GetMesh().faces[fcnt].flags;int e1=edgenum+1,e2=edgenum+2;
	    BOOL invis0=((flags & EDGE_A)==0),invis1=((flags & EDGE_B)==0),invis2=((flags & EDGE_C)==0);
		BOOL keepface=TRUE;
	    if (cull) 
		{ if (subedgelst[edgenum]&& invis0) subedgelst.Clear(edgenum);
		  if (subedgelst[e1]&& invis1) subedgelst.Clear(e1);
		  if (subedgelst[e2]&& invis2) subedgelst.Clear(e2);
		}
		if (faceSel[fcnt]) 
		{ if (useall ||(!invis0)) subedgelst.Set(edgenum);
		  if (useall ||(!invis1)) subedgelst.Set(e1);
		  if (useall ||(!invis2)) subedgelst.Set(e2);
		  keepface=FALSE;
		}
		else 
		{ keepface=!((subedgelst[edgenum]||(visonly&&invis0))&&((subedgelst[e1]||(visonly&&invis1))&&(subedgelst[e2]||(visonly&&invis2))));
		}
		int v0=triOb->GetMesh().faces[fcnt].v[0],v1=triOb->GetMesh().faces[fcnt].v[1];
		int v2=triOb->GetMesh().faces[fcnt].v[2];
		if (subedgelst[edgenum]) { vertSel.Set(v0);vertSel.Set(v1);}
		if (subedgelst[e1]) { vertSel.Set(v1);vertSel.Set(v2);}
		if (subedgelst[e2]) { vertSel.Set(v0);vertSel.Set(v2);}
		if (keepface)
		{ fmuststay.Set(fcnt);
		  vmuststay.Set(v0);vmuststay.Set(v1);vmuststay.Set(v2);
		}
		edgenum+=3;
	  }
	  subvs=vertSel.NumberSet();
	  vmuststay=vmuststay | (~vertSel);
	  basev=vmuststay.NumberSet();
	  basef=fmuststay.NumberSet();
	  fstaylst=new Face[basef];assert(fstaylst);
	  int cnt=0;
	  for (fcnt=0;fcnt<infaces;fcnt++) 
	  { if (fmuststay[fcnt]) fstaylst[cnt++]=triOb->GetMesh().faces[fcnt];}
	  if ((addmap>0)&&(tverts>0))
	  { savedtvlst.SetSize(tverts);
	    int cnt=0;tvfstaylst=new TVFace[basef];assert(tvfstaylst);
		for (int i=0;i<infaces;i++)
		  if (fmuststay[i])
		{ tvfstaylst[cnt]=triOb->GetMesh().tvFace[i];
		  savedtvlst.Set(tvfstaylst[cnt].t[0]);
		  savedtvlst.Set(tvfstaylst[cnt].t[1]);
		  savedtvlst.Set(tvfstaylst[cnt].t[2]);cnt++;
		}
		oldtvs=savedtvlst.NumberSet();
		tvstaylst=new UVVert[oldtvs];cnt=0;
	    for (i=0;i<tverts;i++)
		{ if (savedtvlst[i])
		  tvstaylst[cnt++]=triOb->GetMesh().tVerts[i];
		}
	  } else if (addmap) oldtvs=1;
	}
	Point3 *vlst=new Point3[inverts];
	assert(vlst);
    fEdge *edgelst=NULL;
	UVVert *tvlst=NULL;
	int nummtls=0,multi=0;
	if ((inverts==0)&&(infaces==0)) 
	{ triOb->GetMesh().setNumVerts(0);
	  triOb->GetMesh().setNumFaces(0);
	  triOb->GetMesh().setNumTVerts(0);
	  triOb->GetMesh().setNumTVFaces(0);
	}
	else
	{
	int type,smooth;
	pblock->GetValue(PB_GEOMETRY,0,type,FOREVER); 
	for (int i=0;i<inverts;i++)
	   vlst[i]=triOb->GetMesh().verts[i];
	assert((edgelst=new fEdge[numedges])!=NULL);
	if (addmap==1)
	{ tvlst=new UVVert[inverts];	}
	for (i=0;i<infaces;i++)
      GetEdges(triOb->GetMesh(),edgelst,&edges,i,useall,addmap==1,!submesh,subedgelst);
	int outverts=0,outfaces=0;
	int jsegs,geobase,ssegs,ssides,endcaps,veachv,veachf,eeachv,eeachf,nsegs;
	int toutv=0,strutID,junctID;
	int realverts=(submesh?subvs:inverts);
	if ((realverts>0)&&(type>0))
	{ pblock->GetValue(PB_JSEGS,t,jsegs,FOREVER); if (jsegs<1) jsegs=1;
	  pblock->GetValue(PB_GEOBASE,t,geobase,FOREVER); if (geobase<0) geobase=0;
	  pblock->GetValue(PB_JUNCTMTL,t,junctID,FOREVER); 
//	  if (junctID>nummtls) junctID=0; 
	  junctID-=1;
      veachf=(jsegs*jsegs*(nsegs=(geobase==0?4:(geobase==1?8:20))));
	  veachv=veachf/2 + 2;
	  outfaces=veachf*realverts;
	  outverts=veachv*realverts;
	  if (addmap==1) 
	  { toutv=inverts;
	    for (i=0;i<inverts;i++)
	    { int inwhich=0,vnum;
		  if (VertexinWhichFace(i,triOb->GetMesh().faces,infaces,&inwhich,&vnum))
		    tvlst[i]=triOb->GetMesh().tVerts[triOb->GetMesh().tvFace[inwhich].t[vnum]];
		  else tvlst[i]=Point3(0.0f,0.0f,0.0f);
		}
	  }
	}
	if ((edges>0)&&(type!=1))
	{ pblock->GetValue(PB_SSEGS,t,ssegs,FOREVER); if (ssegs<1) ssegs=1;
	  pblock->GetValue(PB_SSIDES,t,ssides,FOREVER); if (ssides<3) ssides=3;
	  pblock->GetValue(PB_ENDCAPS,t,endcaps,FOREVER); 
	  pblock->GetValue(PB_STRUTMTL,t,strutID,FOREVER); 
//	  if (strutID>nummtls) strutID=0;
	  strutID-=1;
      outverts+=edges*(eeachv=((ssegs+1)*ssides+(endcaps?2:0)));
	  outfaces+=edges*(eeachf=(ssegs*ssides*2+(endcaps?2*ssides:0)));
	  if (addmap==1) toutv+=(ssegs+1)*(ssides+1)*edges;
	  else if (addmap==2) toutv=(ssegs+1)*(ssides+1);
	}
	outverts+=basev;outfaces+=basef;toutv+=oldtvs;
	BOOL mapsphere=(realverts>0)&&((addmap==2)&&(type!=0));
	triOb->GetMesh().setNumVerts(mapsphere?veachv:outverts);
	triOb->GetMesh().setNumFaces(mapsphere?veachf:outfaces);
	triOb->GetMesh().setNumTVerts(((addmap==0)|| mapsphere)?0:toutv);
	triOb->GetMesh().setNumTVFaces(((addmap==0)|| mapsphere)?0:outfaces);
	int vertexnum=0,face=0,ntvs=0;
	if ((realverts>0)&&(type>0))
	{ float radius;
	  UVVert uv;
	  pblock->GetValue(PB_JRADIUS,t,radius,FOREVER); 
	  pblock->GetValue(PB_JSMOOTH,0,smooth,FOREVER); 
	  if (radius<0.0f) radius=0.0f;
	  DoHedron(radius,triOb->GetMesh(),vertexnum,jsegs,geobase,veachf,smooth,face,junctID);
	  int fcnt=0,vcnt=veachv;
	  if (addmap==2)
	  {	Matrix3 id(1);
		triOb->GetMesh().ApplyUVWMap (MAP_SPHERICAL, 1.0f, 1.0f, 1.0f, 0, 0, 0, 0, id);
		triOb->GetMesh().setNumVerts(outverts,TRUE);
		triOb->GetMesh().setNumFaces(outfaces,TRUE);
		triOb->GetMesh().setNumTVerts((ntvs=triOb->GetMesh().getNumTVerts())+toutv,TRUE);
		triOb->GetMesh().setNumTVFaces(outfaces,TRUE,veachf);
	  }
	  int latv=0,first=-1;
	  for (i=0;i<inverts;i++)
		if ((!submesh)||vertSel[i])
	  { if (first<0) first=i;
		for (int cnt=0;cnt<veachf;cnt++)
		{ if ((addmap==2)&&(i>0)) triOb->GetMesh().tvFace[fcnt]=triOb->GetMesh().tvFace[cnt];  
		  else if (addmap==1) triOb->GetMesh().tvFace[fcnt].setTVerts(latv,latv,latv); 
		  if (i>0) 
		  { triOb->GetMesh().faces[fcnt]=triOb->GetMesh().faces[cnt];
		    triOb->GetMesh().faces[fcnt].v[0]+=vertexnum;
		    triOb->GetMesh().faces[fcnt].v[1]+=vertexnum;
		    triOb->GetMesh().faces[fcnt].v[2]+=vertexnum;
		  }
		  fcnt++;
		}
		latv++;
	    if (i>first)
		for (int cnt=0;cnt<veachv;cnt++)
		    triOb->GetMesh().verts[vcnt++]=triOb->GetMesh().verts[cnt]+vlst[i];
	    face+=veachf;
		vertexnum+=veachv;
	  }
	  for (int cnt=0;cnt<veachv;cnt++)
		 triOb->GetMesh().verts[cnt]+=vlst[first];
	  if (addmap==1)
	  { int latv=0;
		for (i=0;i<inverts;i++)
		  if ((!submesh)||(vertSel[i])) triOb->GetMesh().tVerts[latv++]=tvlst[i];
		ntvs=latv;
	  }
	}
	if ((edges>0)&&(type!=1))
	{ float cyllen,sradius;
	  pblock->GetValue(PB_SRADIUS,t,sradius,FOREVER); 
	  pblock->GetValue(PB_SSMOOTH,0,smooth,FOREVER);
	  if (sradius<0.0f) sradius=0.0f;
	  int nf=face;
	  srand(12345);
	  Point3 rndv=Point3(RND11(),RND11(),RND11());	  
	  int startnt=ntvs;
	  for (i=0;i<edges;i++)
	  { Point3 vert,axis,center;
	    float ang;
		int startv=vertexnum;
		cyllen=Length(axis=(vlst[edgelst[i].vto]-vlst[edgelst[i].vfrom]));
		Point3 lincr=axis/(float)ssegs;
		float delta = TWOPI/(float)ssides;
		vert=(center=vlst[edgelst[i].vfrom])+sradius*Normalize(axis^rndv);
		ang=0.0f;
		axis=Normalize(axis);
	    for (int j=0;j<ssides;j++)
		{ for (int k=0;k<=ssegs;k++)
		    triOb->GetMesh().verts[vertexnum+k*ssides]=vert+(float)k*lincr;
		  RotateOnePoint(&vert.x,&center.x,&axis.x,-delta);
		  vertexnum++;
		}
		vertexnum+=ssegs*ssides;
		int sv,sv2,kcount,k,maxf=2*ssides,maxv=maxf-2,level=ssides-1,tvcnt=0,nlevel=ssides+1;
		BOOL odd=FALSE;
		if (addmap==1) startnt=ntvs;
		sv=startv;sv2=startv+ssides;
		float udiv,vdiv,u1=0.0f,v1=0.0f;
		if (addmap>0)
		{ float u=0.0f,v=0.0f,t1;
		  if (addmap==1)
		  { u=edgelst[i].uvfrom.x;v=edgelst[i].uvfrom.y;
			udiv=(edgelst[i].uvto.x-edgelst[i].uvfrom.x)/ssides;
		    vdiv=((t1=edgelst[i].uvto.y)-v)/ssegs;}
		  else if (addmap==2) {udiv=1.0f/(float)ssides;vdiv=1.0f/(float)ssegs;}
		  if ((addmap==1)||(i==0))
		  { for (k=0;k<=ssegs;k++)
		    { u=0.0f;
			  for (j=0;j<=ssides;j++)
			  { triOb->GetMesh().tVerts[ntvs++]=Point3(u,v,0.0f);
			    u+=udiv;
		      }
			  v+=vdiv;
		    }
		  }
		}
		int uincr=ssides+1,tvu1=0,tvv=0,sntvs=startnt,nt2=sntvs+nlevel;
		for (k=0;k<ssegs;k++)
		{ for (j=0;j<maxf;j++)
		{ triOb->GetMesh().faces[nf].setEdgeVisFlags(0,1,1);
		  if (strutID>=0) triOb->GetMesh().faces[nf].setMatID(strutID);
		  if (!smooth) triOb->GetMesh().faces[nf].setSmGroup(0);
		  else triOb->GetMesh().faces[nf].setSmGroup(1);
		  if (odd) 
		  { if (j<maxv) 
		     triOb->GetMesh().faces[nf].setVerts(sv,sv2+1,sv+1);
		    else triOb->GetMesh().faces[nf].setVerts(sv,sv2-level,sv-level);
		    if (addmap>0) triOb->GetMesh().tvFace[nf].setTVerts(sntvs,nt2+1,sntvs+1);
		    sv++;sv2++;sntvs++;nt2++; }
		  else
		  { if (j<maxv) triOb->GetMesh().faces[nf].setVerts(sv2+1,sv,sv2);
		    else triOb->GetMesh().faces[nf].setVerts(sv2-level,sv,sv2); 
		    if (addmap>0) triOb->GetMesh().tvFace[nf].setTVerts(nt2+1,sntvs,nt2);
		  }
		  odd=!odd;
		  nf++;
		}  sntvs++;nt2++;
		} 
		if (endcaps) 
		{ triOb->GetMesh().verts[vertexnum]=vlst[edgelst[i].vfrom];
		  sntvs=startnt;
		  sv=startv;kcount=nf;
		  for (k=0;k<ssides*2;k++)
		  {	triOb->GetMesh().faces[kcount].setEdgeVisFlags(0,1,0);
		    if (strutID>=0) triOb->GetMesh().faces[nf].setMatID(strutID);
			if (!smooth) triOb->GetMesh().faces[kcount].setSmGroup(0);
			else triOb->GetMesh().faces[kcount].setSmGroup(2);
			kcount++;
		  }
		  for (k=0;k<ssides-1;k++)
		  { triOb->GetMesh().faces[nf].setVerts(vertexnum,sv,sv+1);
		    if (addmap>0) triOb->GetMesh().tvFace[nf].setTVerts(startnt,sntvs,sntvs+1);
		    sv++;nf++;sntvs++;
		  }
		  if (addmap>0) {triOb->GetMesh().tvFace[nf].setTVerts(startnt,sntvs,sntvs+1);sntvs++;}
		  triOb->GetMesh().faces[nf++].setVerts(vertexnum,sv,startv);
		  startv=sv=vertexnum-ssides;
		  vertexnum++; int lasts=nt2-1;sntvs=lasts-ssides;
		  triOb->GetMesh().verts[vertexnum]=vlst[edgelst[i].vto];
		  for (k=0;k<ssides-1;k++)
		  { triOb->GetMesh().faces[nf].setVerts(vertexnum,sv+1,sv);
		    if (addmap>0) triOb->GetMesh().tvFace[nf].setTVerts(lasts,sntvs+1,sntvs);
		    sv++;nf++;sntvs++;
		  }
		  if (addmap>0) triOb->GetMesh().tvFace[nf].setTVerts(lasts,sntvs+1,sntvs);
		  triOb->GetMesh().faces[nf++].setVerts(vertexnum,startv,sv);
		  vertexnum++;
		}
	  }
	}
	if (submesh)
	{ int *newlst=new int[inverts];assert(newlst);
	  for (int i=0;i<inverts;i++)
		{ if (vmuststay[i]) 
		  { triOb->GetMesh().verts[vertexnum]=vlst[i];
			newlst[i]=vertexnum;
			vertexnum++;
		  }
		}
	  int nf=outfaces-basef;
	  for (i=0;i<basef;i++)
	  { triOb->GetMesh().faces[nf]=fstaylst[i];
	    triOb->GetMesh().faces[nf].v[0]=newlst[fstaylst[i].v[0]];
	    triOb->GetMesh().faces[nf].v[1]=newlst[fstaylst[i].v[1]];
	    triOb->GetMesh().faces[nf].v[2]=newlst[fstaylst[i].v[2]];
	    nf++;
	  }
	  if (newlst) delete[] newlst;
	  if (tvstaylst)
	  { int ntv=triOb->GetMesh().getNumTVerts()-oldtvs;
		for (i=0;i<oldtvs;i++)
		  triOb->GetMesh().tVerts[ntv++]=tvstaylst[i];  
		int *tvlst=new int[tverts];assert(tvlst);
		int count=0;
		for (i=0;i<tverts;i++)
		{  if (savedtvlst[i]) {tvlst[i]=count;count++;}
		}
		int nf=triOb->GetMesh().getNumFaces()-basef;
		for (i=0;i<basef;i++)
			triOb->GetMesh().tvFace[nf++].setTVerts(tvlst[tvfstaylst[i].t[0]],tvlst[tvfstaylst[i].t[1]],tvlst[tvfstaylst[i].t[2]]);
	    if (tvlst) delete[] tvlst;
	  }
	  else if (addmap) 
	  { int nv=triOb->GetMesh().getNumTVerts()-oldtvs;
		triOb->GetMesh().tVerts[nv]=Point3(0.0f,0.0f,0.0f);
		int nf=triOb->GetMesh().getNumFaces()-basef;
		for (i=0;i<basef;i++)
		  triOb->GetMesh().tvFace[nf++].setTVerts(nv,nv,nv);	 
	  }
	}
	}	
	if (tvstaylst) delete[] tvstaylst;
	if (tvfstaylst) delete[] tvfstaylst;
	if (tvlst) delete[] tvlst;
	if (edgelst) delete[] edgelst;
	if (vlst) delete[] vlst;
	if (fstaylst) delete[] fstaylst;
	triOb->GetMesh().faceSel.ClearAll();
	triOb->GetMesh().vertSel.ClearAll();
	triOb->GetMesh().edgeSel.ClearAll();
	triOb->SetChannelValidity(TOPO_CHAN_NUM, valid);
	triOb->SetChannelValidity(GEOM_CHAN_NUM, valid);
	triOb->SetChannelValidity(TEXMAP_CHAN_NUM, valid);
	triOb->SetChannelValidity(MTL_CHAN_NUM, valid);
	triOb->SetChannelValidity(SELECT_CHAN_NUM, valid);
	triOb->SetChannelValidity(SUBSEL_TYPE_CHAN_NUM, valid);
	triOb->SetChannelValidity(DISP_ATTRIB_CHAN_NUM, valid);
	triOb->FreeChannels(VERTCOLOR_CHANNEL);
	os->obj = triOb;
	os->obj->UnlockObject();
	}

void SpinnerOff(HWND hWnd,int SpinNum)
{ ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hWnd,SpinNum));
  spin2->Disable();
  ReleaseISpinner(spin2);
}
void SpinnerOn(HWND hWnd,int SpinNum)
{ ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hWnd,SpinNum));
  spin2->Enable();
  ReleaseISpinner(spin2);
}

void StrutsOn(LatticeMod *po,HWND hWnd,BOOL joff)
{ EnableWindow(GetDlgItem(hWnd,IDC_AP_EDGESVIS),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_EDGESALL),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_ENDCAPS),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_STRUTSMOOTH),TRUE);
  SpinnerOn(hWnd,IDC_AP_STRUTRADIUSSPIN);
  SpinnerOn(hWnd,IDC_AP_STRUTSEGMENTSSPIN);
  SpinnerOn(hWnd,IDC_AP_STRUTSIDESSPIN);
  SpinnerOn(hWnd,IDC_AP_STRUTMATIDSPIN);
  if (joff)
  { EnableWindow(GetDlgItem(hWnd,IDC_AP_TYPETET),FALSE);
    EnableWindow(GetDlgItem(hWnd,IDC_AP_TYPEOCT),FALSE);
    EnableWindow(GetDlgItem(hWnd,IDC_AP_TYPEICOSA),FALSE);
    EnableWindow(GetDlgItem(hWnd,IDC_AP_JUNCTSMOOTH),FALSE);
    SpinnerOff(hWnd,IDC_AP_JUNCTRADIUSSPIN);
    SpinnerOff(hWnd,IDC_AP_JUNCTSEGMENTSSPIN);
	SpinnerOff(hWnd,IDC_AP_JUNCTMATIDSPIN);
  }
}
void JunctionOn(LatticeMod *po,HWND hWnd,BOOL soff)
{ if (soff)
  { EnableWindow(GetDlgItem(hWnd,IDC_AP_EDGESVIS),FALSE);
    EnableWindow(GetDlgItem(hWnd,IDC_AP_EDGESALL),FALSE);
    EnableWindow(GetDlgItem(hWnd,IDC_AP_ENDCAPS),FALSE);
    EnableWindow(GetDlgItem(hWnd,IDC_AP_STRUTSMOOTH),FALSE);
    SpinnerOff(hWnd,IDC_AP_STRUTRADIUSSPIN);
    SpinnerOff(hWnd,IDC_AP_STRUTSEGMENTSSPIN);
    SpinnerOff(hWnd,IDC_AP_STRUTSIDESSPIN);
    SpinnerOff(hWnd,IDC_AP_STRUTMATIDSPIN);
  }
  EnableWindow(GetDlgItem(hWnd,IDC_AP_TYPETET),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_TYPEOCT),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_TYPEICOSA),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_JUNCTSMOOTH),TRUE);
  SpinnerOn(hWnd,IDC_AP_JUNCTRADIUSSPIN);
  SpinnerOn(hWnd,IDC_AP_JUNCTSEGMENTSSPIN);
  SpinnerOn(hWnd,IDC_AP_JUNCTMATIDSPIN);
}

class LatticeDlg : public ParamMapUserDlgProc {
	public:
		LatticeMod *po;

		LatticeDlg(LatticeMod *p) {po=p;}
		BOOL DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
		void Update(TimeValue t);
		void DeleteThis() {delete this;}
	};

void LatticeDlg::Update(TimeValue t)
{	if (po->lattMod)
{	HWND hWnd=po->pmapParam->GetHWnd();
	int geom;
	po->pblock->GetValue(PB_GEOMETRY,t,geom,FOREVER);
	if (geom==0) StrutsOn(po,hWnd,1);
	else if (geom==1) JunctionOn(po,hWnd,1);
	else
	{ StrutsOn(po,hWnd,0);
	  JunctionOn(po,hWnd,0);
	}
}
}

BOOL LatticeDlg::DlgProc(
		TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{	switch (msg) {
		case WM_INITDIALOG: {
			Update(t);
			return FALSE;	// stop default keyboard focus - DB 2/27  
			}
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{ case IDC_AP_STRUTSON:
				StrutsOn(po,hWnd,1);
				break;
			  case IDC_AP_JUNCTSON:
				JunctionOn(po,hWnd,1);
				break;
			  case IDC_AP_BOTHON:
				StrutsOn(po,hWnd,0);
				JunctionOn(po,hWnd,0);
				break;
			}
			break;	
		default:
			return FALSE;
		}
	return TRUE;
}
void LatticeMod::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{	this->ip = ip;
	lattMod = this;

	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
	SetAFlag(A_MOD_BEING_EDITED);
	pmapParam = CreateCPParamMap(
		descParam,PARAMDESC_LENGTH,
		pblock,
		ip,
		hInstance,
		MAKEINTRESOURCE(IDD_AP_LATTICE),
		GetString(IDS_AP_PARAMETERS),
		0);	
	  if (pmapParam) pmapParam->SetUserDlgProc(new LatticeDlg(this));
}

void LatticeMod::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next)
{	this->ip = NULL;
	lattMod=NULL;
	
	TimeValue t = ip->GetTime();

	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_END_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF);
	ClearAFlag(A_MOD_BEING_EDITED);

	DestroyCPParamMap(pmapParam);

	// Save these values in class variables so the next object created will inherit them.
/*	pblock->GetValue(PB_AMOUNT,ip->GetTime(),dlgAmount,FOREVER);
	pblock->GetValue(PB_SEGS,ip->GetTime(),dlgSegs,FOREVER);
	pblock->GetValue(PB_CAPSTART,ip->GetTime(),dlgCapStart,FOREVER);
	pblock->GetValue(PB_CAPEND,ip->GetTime(),dlgCapEnd,FOREVER);
	pblock->GetValue(PB_CAPTYPE,ip->GetTime(),dlgCapType,FOREVER);
	pblock->GetValue(PB_OUTPUT,ip->GetTime(),dlgOutput,FOREVER);
	pblock->GetValue(PB_MAPPING,ip->GetTime(),dlgMapping,FOREVER);	*/
	}

Interval LatticeMod::GetValidity(TimeValue t)
	{
	float f;
	int i;
	Interval valid = FOREVER;
	pblock->GetValue(PB_SRADIUS,t,f,valid);
	pblock->GetValue(PB_SSEGS,t,i,valid);	
	pblock->GetValue(PB_SSIDES,t,i,valid);
	pblock->GetValue(PB_JRADIUS,t,f,valid);
	pblock->GetValue(PB_JSEGS,t,i,valid);	
	return valid;
	}

RefResult LatticeMod::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
   		PartID& partID, 
   		RefMessage message ) 
   	{
	switch (message) {
		case REFMSG_CHANGE:
			if ((lattMod==this) && pmapParam) pmapParam->Invalidate();
			break;
		case REFMSG_GET_PARAM_DIM: {
			GetParamDim *gpd = (GetParamDim*)partID;
			gpd->dim = GetParameterDim(gpd->index);			
			return REF_STOP; 
			}

		case REFMSG_GET_PARAM_NAME: {
			GetParamName *gpn = (GetParamName*)partID;
			gpn->name = GetParameterName(gpn->index);			
			return REF_STOP; 
			}
		}
	return(REF_SUCCEED);
	}

ParamDimension *LatticeMod::GetParameterDim(int pbIndex)
	{
	switch (pbIndex) {
		case PB_SRADIUS: 	
		case PB_JRADIUS: 	return stdWorldDim;
		case PB_GEOMETRY: 
		case PB_SSEGS:		
		case PB_SSIDES:	
		case PB_EDGES:		
		case PB_ENDCAPS:
		case PB_SSMOOTH:
		case PB_JSEGS:
		case PB_GEOBASE:
		case PB_JSMOOTH:
		case PB_MAPPING:
							return defaultDim;
		default:			return defaultDim;
		}
	}

TSTR LatticeMod::GetParameterName(int pbIndex)
	{
	switch (pbIndex) {
		case PB_SRADIUS:	return TSTR(GetString(IDS_AP_SRADIUS));
		case PB_SSEGS:		return TSTR(GetString(IDS_AP_SSEGS));
		case PB_SSIDES:		return TSTR(GetString(IDS_AP_SIDES));
		case PB_JRADIUS:	return TSTR(GetString(IDS_AP_JRADIUS));
		case PB_JSEGS:		return TSTR(GetString(IDS_AP_JSEGS));
		default:			return TSTR(_T(""));
		}
	}
class LatPostLoadCallback : public PostLoadCallback {
	public:
		ParamBlockPLCB *cb;
		LatPostLoadCallback(ParamBlockPLCB *c) {cb=c;}
		void proc(ILoad *iload) {
			DWORD oldVer = ((LatticeMod*)(cb->targ))->pblock->GetVersion();
			ReferenceTarget *targ = cb->targ;
			cb->proc(iload);
			if (oldVer<1) 
			{	((LatticeMod*)targ)->pblock->SetValue(PB_ENTIREOBJECT,0,1);
				int whatsit;
				((LatticeMod*)targ)->pblock->GetValue(PB_EDGES,0,whatsit,FOREVER);
				((LatticeMod*)targ)->pblock->SetValue(PB_EDGES,0,(1-whatsit));
			}
			delete this;
		}
	};

IOResult LatticeMod::Load(ILoad *iload)
	{
	Modifier::Load(iload);
	iload->RegisterPostLoadCallback(
		new LatPostLoadCallback(
			new ParamBlockPLCB(versions,NUM_OLDVERSIONS,&curVersion,this,0)));
	return IO_OK;
	}
