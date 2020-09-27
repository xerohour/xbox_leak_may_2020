#ifndef __PROSCATR__H
#define __PROSCATR__H

#include "Max.h"
#include "pscatr1.h"
#include "iparamm.h"
#include "simpobj.h"

TCHAR *GetString(int id);

extern ClassDesc* GetPScatObjDesc();
extern ClassDesc* GetSWrapObjDesc();

extern HINSTANCE hInstance;
extern int RNDSign();
extern float RND01();
extern float RND11();
extern int RND0x(int maxnum);
const float FLOAT_EPSILON=0.005f;
const float HalfPI=1.570796327f;
const float PIOver5=0.62831853f;
const float PIOver180=0.017453292f;
const float PRECISION_LIMIT=1.0e-15f;
//const float SMALL_EPSILON=0.00002f;
const float SMALL_EPSILON=0.005f;
const int row3size=3*sizeof(float);
const int mmap=MAX_MESHMAPS-1;
typedef float Matrix4By4[4][4];
typedef float Matrix3By4[3][4];
void Mult4X4(Matrix4By4 A,Matrix4By4 B,Matrix4By4 C);
extern void RotatePoint(Matrix3By4 Pin,float *Q, float *W,float Theta);
extern void RotateOnePoint(float *Pin,float *Q, float *W,float Theta);
extern int FloatEQ0(float number);
extern int SmallerEQ0(float number);
extern int FGT0(Point3 p1);
extern void Mult4X1(float *A,Matrix4By4 B,float *C);
extern int MatrixInvert(Matrix4By4 in,Matrix4By4 out);
extern BOOL CALLBACK DefaultSOTProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
extern void SpinnerOn(HWND hWnd,int SpinNum,int Winnum);
extern void SpinnerOff(HWND hWnd,int SpinNum,int Winnum);
extern Point3 CalcSpread(float divangle,Point3 oldnorm);
extern void Mult4XPoint(Point3 *Pin,Matrix4By4 B);
extern BOOL SpinIsEnabled(HWND hWnd,int SpinNum);
extern Mtl *CMaterials(Mtl *mat1, Mtl *mat2, int &c1, int &c2);
extern int nummaps(TriObject *wrap1,TriObject *wrap2,BOOL hdist,int &inmap,int &dmap,BOOL Proxy=FALSE);
extern void mapstuff(Mesh *newMesh,TriObject *wrappee,TriObject *wrapper,int oldmap,int wrappermap,int rep=1);
#endif
