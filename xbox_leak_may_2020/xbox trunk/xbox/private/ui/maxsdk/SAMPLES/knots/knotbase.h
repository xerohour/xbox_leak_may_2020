#ifndef __MAXTR__H
#define __MAXTR__H

#include "Max.h"
#include "knotbase.h"
#include "Simpobj.h"
#include "resource.h"
#include "windows.h"
#include "iparamm.h"
#include "frenet.h"

TCHAR *GetString(int id);
#define NUMNODES	40

extern ClassDesc* GetTorusKnotDesc();
extern ClassDesc* GetRingWaveObjDesc();

extern HINSTANCE hInstance;
const int isize=sizeof(int);
const int fsize=sizeof(float);
const int NLEN=16;
const int HLEN=4*isize;
typedef TCHAR AName[NLEN];
typedef float Matrix3By3[3][3];
typedef float Matrix3By4[3][4];
typedef float Matrix4By3[4][3];
typedef float Matrix4By4[4][4];
typedef struct{
	int on,off;
} Lpair;

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

const Point3 Zero=Point3(0.0f,0.0f,0.0f);
const int row3size=3*sizeof(float);
const float FLOAT_EPSILON=0.005f;
const float PRECISION_LIMIT=1.0e-15f;
const float SMALL_EPSILON=0.00002f;
extern int MatrixInvert(Matrix4By4 in,Matrix4By4 out);
extern void Mult4X1(float *A,Matrix4By4 B,float *C);
extern void RotateOnePoint(float *Pin,float *Q, float *W,float Theta);
extern int FloatEQ0(float number);
extern int SmallerEQ0(float number);
extern int FGT0(Point3 p1);
extern void TurnButton(HWND hWnd,int SpinNum,BOOL ison);
extern TriObject *IsTriUseable(ObjectState os,TimeValue t);
extern ShapeObject *IsShapeUseable(Object *pobj,TimeValue t);
extern void MakeNodeList(INode *node,INodeTab *ntab,TimeValue t);
extern void MakeGroupNodeList(INode *node,INodeTab *ntab,TimeValue t);
extern int IncS(int s,int max);
extern int DecS(int s,int max);
extern BOOL ReadInt(int *buf,FILE *f);
extern BOOL WriteInt(int *buf,FILE *f);
extern BOOL GenNewSaveFile(int osize,int size,int custsettings,FILE *f,TCHAR *filename,int vers);
extern void SpinnerOn(HWND hWnd,int SpinNum);
extern void SpinnerOff(HWND hWnd,int SpinNum);
extern BOOL LockBoxBad(IObjParam *ip);
extern void TimingOn(HWND hWnd);
extern void TimingOff(HWND hWnd);
#endif
