#ifndef __SUPERPRTS__H
#define __SUPERPRTS__H

#include "Max.h"
#include "sprts.h"
#include "Simpobj.h"

TCHAR *GetString(int id);

extern ClassDesc* GetPArrayDesc();
extern ClassDesc* GetPBombObjDesc();
extern ClassDesc* GetPBombModDesc();
extern ClassDesc* GetSphereDefDesc();
extern ClassDesc* GetSphereDefModDesc();
extern ClassDesc* GetSuprSprayDesc();
extern ClassDesc* GetBlizzardDesc();
extern ClassDesc* GetPFollowDesc();
extern ClassDesc* GetPFollowModDesc();
extern ClassDesc* GetUniDefDesc();
extern ClassDesc* GetUniDefModDesc();
extern ClassDesc* GetPCloudDesc();
#define A_RENDER			A_PLUGIN1
#define A_NOTREND			A_PLUGIN2

#define APRTS_ROLLUP1_OPEN  (1<<0)
#define APRTS_ROLLUP2_OPEN  (1<<1)
#define APRTS_ROLLUP3_OPEN  (1<<2)
#define APRTS_ROLLUP4_OPEN  (1<<3)
#define APRTS_ROLLUP5_OPEN  (1<<4)
#define APRTS_ROLLUP6_OPEN  (1<<5)
#define APRTS_ROLLUP7_OPEN  (1<<6)
#define APRTS_ROLLUP8_OPEN  (1<<7)

#define APRTS_ROLLUP_FLAGS (APRTS_ROLLUP1_OPEN)

extern HINSTANCE hInstance;
extern int RNDSign();
extern float RND01();
extern float RND11();
extern int RND0x(int maxnum);
extern void slrand (unsigned int seed);
extern int hrand (void);
const float FLOAT_EPSILON=0.005f;
const float HalfPI=1.570796327f;
const float PIOver5=0.62831853f;
const float FTOIEPS=0.000001f;
const float PRECISION_LIMIT=1.0e-15f;
const float SQR2=1.1414f;
//const float SMALL_EPSILON=0.00002f;
const float SMALL_EPSILON=0.005f;
const int row3size=3*sizeof(float);
const Point3 v111=Point3(0.450f,0.218f,0.732f);
const Point3 Zero=Point3(0.0f,0.0f,0.0f); 
const Point3 deftex=Point3(0.5f,0.5f,0.0f);
const int NoAni=-9999;

typedef float Matrix4By4[4][4];
typedef float Matrix3By4[3][4];
typedef struct{
  Point3 spos1,svel1;
  Point3 spos2,svel2;
  int min,min2;
  TimeValue mintime;
  int collide;
} mindata;
typedef struct{
  int lastmin,lastcollide;
}oldipc;
typedef struct
{ Point3 vel;
  int K;
  TimeValue oneframe;
  BOOL inaxis;
}InDirInfo;
typedef struct{
 TimeValue tl;
 int gennum;
} TimeAndGen;
typedef struct{
  Tab<TimeAndGen> tl;
} TimeLst;
typedef Tab<Mtl *>Mtllst;
typedef struct
{ Point3 pts[8];
  Point3 Suboffset;}
ptlst;
typedef struct
{  Tab<ptlst> bpts;
   int numboxes;
} boxlst;
typedef struct
{ float dirchaos,spchaos,scchaos;
  int spsign,scsign,invel,spconst,scconst,axisentered;
  Point3 Axis;
  float axisvar;
} SpawnVars;
typedef struct{
 float val;
 int num;
} limdata;
typedef struct{
 int p1,p2;
} pairs;
typedef struct{
 Box3 bbox;
 int refnum;
}boxplus;
typedef struct{
  TimeValue stuckt;
  Tab<pairs> ppts;
} stuck;

class CollideParticle {
public:
	CollideParticle() {xlst=NULL;bbox=NULL;minpt.collide=-1;stucklist.stuckt=-1;stucklist.ppts.SetCount(5);
						sused=0;ssize=5;}
	~CollideParticle() {stucklist.ppts.SetCount(0);stucklist.ppts.Shrink();}
	pairs *xlst;
	boxplus *bbox;
	int xmaxlen,count,xcnt;
	mindata minpt;
	stuck stucklist;
	int sused,ssize;
	mindata InterPartCollide(ParticleSys &parts,Tab<CollisionObject*> &cobjs,int &remtime,int &stepnum,float B,float Vb,int &t,oldipc &l);
	TimeValue DetectParticleCollsion(ParticleSys &parts,pairs x,int t,float B,float Vb,mindata &tmpdata);
	TimeValue FindMinPartCollide(ParticleSys &parts,int remtime,float B,float Vb);
	int PredetectParticleCollisions();
	void PossibleCollide(int *active,BOOL *used,int acount);
	int FindXLst(int &pos,int num);
	void PossibleYZCollide(int *active,BOOL *used,int acount,int num);
	void RemoveThisPart(int num);
};


class NullView: public View {
	public:
		Point2 ViewToScreen(Point3 p) { return Point2(p.x,p.y); }
		NullView() { worldToView.IdentityMatrix(); screenW=640.0f; screenH = 480.0f; }
	};
class ParticleMtl: public Material {
	public:
	ParticleMtl();
	};
static ParticleMtl particleMtl;
static Matrix3 ident(1);

#define PARTICLE_R	float(1.0)
#define PARTICLE_G	float(1.0)
#define PARTICLE_B	float(0.0)
#define COLLIDE 2
#define ONDEATH 3
#define EMIT 4

#define NUMNODES 40
#define LOTSOFACES	2000
const int isize=sizeof(int);
const int fsize=sizeof(float);
const int bsize=sizeof(BOOL);
const int NLEN=16;
const int HLEN=4*isize;
typedef TCHAR AName[NLEN];
const float coursedivider=6.0f;


extern void Mult4X4(Matrix4By4 A,Matrix4By4 B,Matrix4By4 C);
extern void RotatePoint(Matrix3By4 Pin,float *Q, float *W,float Theta);
extern void RotateOnePoint(float *Pin,float *Q, float *W,float Theta);
extern float sign(float sval);
extern void MakeInterpRotXform(Matrix3 InTmBegin,Matrix3 InTmEnd,float portion,Matrix3& OutTm);
extern int FloatEQ0(float number);
extern int SmallerEQ0(float number);
extern int FGT0(Point3 p1);
extern void Mult4X1(float *A,Matrix4By4 B,float *C);
extern int MatrixInvert(Matrix4By4 in,Matrix4By4 out);
extern BOOL CALLBACK DefaultSOTProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
extern void TurnButton(HWND hWnd,int SpinNum,BOOL ison);
extern void SpinnerOn(HWND hWnd,int SpinNum,int Winnum);
extern void SpinnerOff(HWND hWnd,int SpinNum,int Winnum);
extern Point3 CalcSpread(float divangle,Point3 oldnorm);
const float M=0.1f;
const int RENDTYPE1=0;
const int RENDTYPE2=1;
const int RENDTYPE3=2;
const int RENDTYPE5=3;
const int RENDTYPE6=4;
const int RENDTET=5;
const int REND6PT=6;
const int RENDSPHERE=7;
#define MAXNAME 24
extern float FigureOutSize(TimeValue age,float size,TimeValue grow,TimeValue fade,TimeValue life,float grate,float frate);
extern void AddFace(int a, int b, int c,int face,Mesh *pm,int curmtl);
extern void PlotSpecial(float radius,int vertexnum,int face,Mesh *pm,float Angle,float *W,int curmtl,Point3 *pt,InDirInfo indir);
extern void PlotCube8(float radius,int vertexnum,int face, Mesh *pm, float Angle,float *W,int curmtl,Point3* pt,InDirInfo indir);
extern void Plot6PT(float radius,int vertexnum,int face, Mesh *pm, float Angle,float *W,int curmtl,Point3* pt,InDirInfo indir);
extern void PlotTet(float radius,int vertexnum,int face, Mesh *pm, float Angle,Point3 W,int curmtl,Point3* pt,InDirInfo indir);
extern void PlotTriangle(float radius,int vertexnum,int face, Mesh *pm, float Angle,float *W,int curmtl,Point3 *pt,InDirInfo indir);
extern void PlotCustom(float radius,int i,int vertexnum,Mesh *pm, float Angle,float *W,Mesh *clst,Point3* pt,int nv,InDirInfo indir);
extern void GetMeshInfo(int type,int count,Mesh *pm,int *numF,int *numV);
extern void PlotFacing(int type,float radius,int vertexnum,int face,Mesh *pm,float Angle,int curmtl,Point3* pt,Point3 camV,Point3 a,Point3 b);
extern void PlotSphere(float radius,int vertexnum,int face, Mesh *pm, float Angle,float *W,int curmtl,Point3 *pt,InDirInfo indir);
extern Point3 RotateAboutAxis(float Angle,Point3 C,Point3 L,Point3 W,InDirInfo indir);
extern void CacheData(ParticleSys *p0,ParticleSys *p1);
extern int TimeFound(TimeLst times,int showframe,int gen);
extern TimeValue GetCurTime(TimeValue showframe,TimeValue ages,int anifr);
extern float Smallest(Point3 pmin);
extern float Largest(Point3 pmax);
extern TriObject *TriIsUseable(Object *pobj,TimeValue t);
extern void VectorVar(Point3 *vel,float R,float MaxAngle);
extern Point3 DoSpawnVars(SpawnVars spvars,Point3 pv,Point3 holdv,float *radius,Point3 *sW);
extern void SpinStuff(HWND hWnd,BOOL ison,BOOL isphase);
extern void StdStuff(HWND hWnd,BOOL ison);
extern void MetaOff(HWND hWnd);
extern void InstStuff(HWND hWnd,BOOL ison,HWND hparam,HWND spawn,BOOL dist=0);
extern void SpawnStuff(HWND hWnd,int stype);
extern void SpinMainStuff(HWND hWnd,BOOL ison);
extern BOOL IsStdMtl(INode *cnode);
extern void MakeNodeList(INode *node,INodeTab *ntab,int subtree,TimeValue t);
extern void MakeGroupNodeList(INode *node,INodeTab *ntab,int subtree,TimeValue t);
extern void FormatName(TCHAR *name);
extern void SpawnWithStype(int stype,HWND spawn,int repi);
extern void ObjectMutQueOn(int stype,HWND spawn,int repi);
extern void ObjectMutQueOff(HWND spawn);
extern void SetFlag(ULONG &flags,ULONG f, ULONG val);
extern void ZeroMesh(Mesh *pm);
extern BOOL ReadInt(int *buf,FILE *f);
extern BOOL WriteInt(int *buf,FILE *f);
extern BOOL GenNewSaveFile(int osize,int size,int custsettings,FILE *f,TCHAR *filename,int vers);
extern void SwitchVerts(Mesh *pm);
extern void AllSpawnBad(HWND hWnd,int stype,BOOL notbad);
extern BOOL CheckMtlChange(Mtl *mtl,BOOL wasmulti);
extern float GetLen(Point3 vels,int K);
extern void IPCControls(HWND hWnd,HWND spwnd,int stype,BOOL ison);
extern BOOL IsGEOM(Object *obj);
extern void MirrorFace(Face *f);
extern void MirrorTVs(TVFace *f);
#endif
