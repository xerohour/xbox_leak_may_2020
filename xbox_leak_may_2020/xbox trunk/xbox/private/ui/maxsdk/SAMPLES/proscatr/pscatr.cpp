/**********************************************************************
 *<
	FILE: PScatr.cpp

	DESCRIPTION:  Procedural Scatter Object Support Code

	CREATED BY: Audrey Peterson

	HISTORY: created 24 December 1996

 *>	Copyright (c) 1996,1997 All Rights Reserved.
 **********************************************************************/
#include "proscatr.h"
#include "stdmat.h"

HINSTANCE hInstance;
static int controlsInit = FALSE;

Point3 Zero=Point3(0.0f,0.0f,0.0f);
TCHAR *GetString(int id)
	{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
	}
//
//------------------------------------------------------
// This is the interface to Jaguar:
//------------------------------------------------------

__declspec( dllexport ) const TCHAR *
LibDescription() { return GetString(IDS_AP_PSCTRLIB); }

// This function returns the number of plug-in classes this DLL implements
__declspec( dllexport ) int 
LibNumberClasses() 
{ 
#ifndef DESIGN_VER
	return 2; 
#else
	return 1;
#endif
}

// This function return the ith class descriptor. We have one.
__declspec( dllexport ) ClassDesc* 
LibClassDesc(int i) {
	switch(i){
	case 0:return GetPScatObjDesc();
#ifndef DESIGN_VER
	case 1:return GetSWrapObjDesc();
#endif
    default:return 0;}
 }

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
	{	
	// Hang on to this DLL's instance handle.
	hInstance = hinstDLL;

  	if (!controlsInit) {
		controlsInit = TRUE;		

	// Initialize MAX's custom controls
	InitCustomControls(hInstance);
		
	// Initialize Win95 controls
	InitCommonControls();
	}
	
	return(TRUE);
	}


// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG 
LibVersion() { return VERSION_3DSMAX; }

/* rand returns a number between 0 and 32767 */
/* number between 0 and 1 */
const float IntMax=32767.0f;
const float IntMax1=32768.0f;
const float HalfIntMax=16383.5f;
typedef float Matrix3By3[3][3];
typedef float Matrix4By3[4][3];
int FloatEQ0(float number)
{return((FLOAT_EPSILON>=number)&&(number>=-FLOAT_EPSILON));
}
int SmallerEQ0(float number)
{return((SMALL_EPSILON>=number)&&(SMALL_EPSILON>=-FLOAT_EPSILON));
}
int FGT0(Point3 p1)
{
	return((fabs(p1[0])>SMALL_EPSILON)||(fabs(p1[1])>SMALL_EPSILON)||(fabs(p1[2])>SMALL_EPSILON));
}

void Mult1X4(float *A,Matrix4By4 B,float *C)
{
   C[0]=A[0]*B[0][0]+A[1]*B[1][0]+A[2]*B[2][0]+A[3]*B[3][0];
   C[1]=A[0]*B[0][1]+A[1]*B[1][1]+A[2]*B[2][1]+A[3]*B[3][1];
   C[2]=A[0]*B[0][2]+A[1]*B[1][2]+A[2]*B[2][2]+A[3]*B[3][2];
   C[3]=A[0]*B[0][3]+A[1]*B[1][3]+A[2]*B[2][3]+A[3]*B[3][3];
}

void Mult4X1(float *A,Matrix4By4 B,float *C)
{
   C[0]=A[0]*B[0][0]+A[1]*B[0][1]+A[2]*B[0][2]+A[3]*B[0][3];
   C[1]=A[0]*B[1][0]+A[1]*B[1][1]+A[2]*B[1][2]+A[3]*B[1][3];
   C[2]=A[0]*B[2][0]+A[1]*B[2][1]+A[2]*B[2][2]+A[3]*B[2][3];
   C[3]=A[0]*B[3][0]+A[1]*B[3][1]+A[2]*B[3][2]+A[3]*B[3][3];
}

void Mult4XPoint(Point3 *Pin,Matrix4By4 B)
{ float Pby4[4],Pout[4]; 
 memcpy(Pby4,Pin,row3size);Pby4[3]=1.0f;
 Mult4X1(Pby4,B,Pout);
 memcpy(Pin,Pout,row3size);
}

void Mult1X3(float *A,Matrix3By3 B,float *C)
{
   C[0]=A[0]*B[0][0]+A[1]*B[1][0]+A[2]*B[2][0];
   C[1]=A[0]*B[0][1]+A[1]*B[1][1]+A[2]*B[2][1];
   C[2]=A[0]*B[0][2]+A[1]*B[1][2]+A[2]*B[2][2];
}

void Mult3X4(Matrix3By4 A,Matrix4By4 B,Matrix3By4 C)
{
   C[0][0]=A[0][0]*B[0][0]+A[0][1]*B[1][0]+A[0][2]*B[2][0]+A[0][3]*B[3][0];
   C[0][1]=A[0][0]*B[0][1]+A[0][1]*B[1][1]+A[0][2]*B[2][1]+A[0][3]*B[3][1];
   C[0][2]=A[0][0]*B[0][2]+A[0][1]*B[1][2]+A[0][2]*B[2][2]+A[0][3]*B[3][2];
   C[0][3]=A[0][0]*B[0][3]+A[0][1]*B[1][3]+A[0][2]*B[2][3]+A[0][3]*B[3][3];
   C[1][0]=A[1][0]*B[0][0]+A[1][1]*B[1][0]+A[1][2]*B[2][0]+A[1][3]*B[3][0];
   C[1][1]=A[1][0]*B[0][1]+A[1][1]*B[1][1]+A[1][2]*B[2][1]+A[1][3]*B[3][1];
   C[1][2]=A[1][0]*B[0][2]+A[1][1]*B[1][2]+A[1][2]*B[2][2]+A[1][3]*B[3][2];
   C[1][3]=A[1][0]*B[0][3]+A[1][1]*B[1][3]+A[1][2]*B[2][3]+A[1][3]*B[3][3];
   C[2][0]=A[2][0]*B[0][0]+A[2][1]*B[1][0]+A[2][2]*B[2][0]+A[2][3]*B[3][0];
   C[2][1]=A[2][0]*B[0][1]+A[2][1]*B[1][1]+A[2][2]*B[2][1]+A[2][3]*B[3][1];
   C[2][2]=A[2][0]*B[0][2]+A[2][1]*B[1][2]+A[2][2]*B[2][2]+A[2][3]*B[3][2];
   C[2][3]=A[2][0]*B[0][3]+A[2][1]*B[1][3]+A[2][2]*B[2][3]+A[2][3]*B[3][3];
}

void Mult4X3(Matrix4By3 A,Matrix4By4 B,Matrix4By3 C)
{
   C[0][0]=A[0][0]*B[0][0]+A[1][0]*B[0][1]+A[2][0]*B[0][2]+A[3][0]*B[0][3];
   C[1][0]=A[0][0]*B[1][0]+A[1][0]*B[1][1]+A[2][0]*B[1][2]+A[3][0]*B[1][3];
   C[2][0]=A[0][0]*B[2][0]+A[1][0]*B[2][1]+A[2][0]*B[2][2]+A[3][0]*B[2][3];
   C[3][0]=A[0][0]*B[3][0]+A[1][0]*B[3][1]+A[2][0]*B[3][2]+A[3][0]*B[3][3];
   C[0][1]=A[0][1]*B[0][0]+A[1][1]*B[0][1]+A[2][1]*B[0][2]+A[3][1]*B[0][3];
   C[1][1]=A[0][1]*B[1][0]+A[1][1]*B[1][1]+A[2][1]*B[1][2]+A[3][1]*B[1][3];
   C[2][1]=A[0][1]*B[2][0]+A[1][1]*B[2][1]+A[2][1]*B[2][2]+A[3][1]*B[2][3];
   C[3][1]=A[0][1]*B[3][0]+A[1][1]*B[3][1]+A[2][1]*B[3][2]+A[3][1]*B[3][3];
   C[0][2]=A[0][2]*B[0][0]+A[1][2]*B[0][1]+A[2][2]*B[0][2]+A[3][2]*B[0][3];
   C[1][2]=A[0][2]*B[1][0]+A[1][2]*B[1][1]+A[2][2]*B[1][2]+A[3][2]*B[1][3];
   C[2][2]=A[0][2]*B[2][0]+A[1][2]*B[2][1]+A[2][2]*B[2][2]+A[3][2]*B[2][3];
   C[3][2]=A[0][2]*B[3][0]+A[1][2]*B[3][1]+A[2][2]*B[3][2]+A[3][2]*B[3][3];
}

void Mult4X4(Matrix4By4 A,Matrix4By4 B,Matrix4By4 C)
{
   C[0][0]=A[0][0]*B[0][0]+A[0][1]*B[1][0]+A[0][2]*B[2][0]+A[0][3]*B[3][0];
   C[0][1]=A[0][0]*B[0][1]+A[0][1]*B[1][1]+A[0][2]*B[2][1]+A[0][3]*B[3][1];
   C[0][2]=A[0][0]*B[0][2]+A[0][1]*B[1][2]+A[0][2]*B[2][2]+A[0][3]*B[3][2];
   C[0][3]=A[0][0]*B[0][3]+A[0][1]*B[1][3]+A[0][2]*B[2][3]+A[0][3]*B[3][3];
   C[1][0]=A[1][0]*B[0][0]+A[1][1]*B[1][0]+A[1][2]*B[2][0]+A[1][3]*B[3][0];
   C[1][1]=A[1][0]*B[0][1]+A[1][1]*B[1][1]+A[1][2]*B[2][1]+A[1][3]*B[3][1];
   C[1][2]=A[1][0]*B[0][2]+A[1][1]*B[1][2]+A[1][2]*B[2][2]+A[1][3]*B[3][2];
   C[1][3]=A[1][0]*B[0][3]+A[1][1]*B[1][3]+A[1][2]*B[2][3]+A[1][3]*B[3][3];
   C[2][0]=A[2][0]*B[0][0]+A[2][1]*B[1][0]+A[2][2]*B[2][0]+A[2][3]*B[3][0];
   C[2][1]=A[2][0]*B[0][1]+A[2][1]*B[1][1]+A[2][2]*B[2][1]+A[2][3]*B[3][1];
   C[2][2]=A[2][0]*B[0][2]+A[2][1]*B[1][2]+A[2][2]*B[2][2]+A[2][3]*B[3][2];
   C[2][3]=A[2][0]*B[0][3]+A[2][1]*B[1][3]+A[2][2]*B[2][3]+A[2][3]*B[3][3];
   C[3][0]=A[3][0]*B[0][0]+A[3][1]*B[1][0]+A[3][2]*B[2][0]+A[3][3]*B[3][0];
   C[3][1]=A[3][0]*B[0][1]+A[3][1]*B[1][1]+A[3][2]*B[2][1]+A[3][3]*B[3][1];
   C[3][2]=A[3][0]*B[0][2]+A[3][1]*B[1][2]+A[3][2]*B[2][2]+A[3][3]*B[3][2];
   C[3][3]=A[3][0]*B[0][3]+A[3][1]*B[1][3]+A[3][2]*B[2][3]+A[3][3]*B[3][3];
}

float det2x2(float a,float b,float c,float d)
{ return(a*d-b*c);
}
float det3x3(float a1,float a2,float a3,float b1,float b2,float b3,float c1,float c2,float c3)
{ return(a1*det2x2(b2,b3,c2,c3)-b1*det2x2(a2,a3,c2,c3)+c1*det2x2(a2,a3,b2,b3));
}

void Adjoint(Matrix4By4 in, Matrix4By4 out,float det)
{float a1,a2,a3,a4,b1,b2,b3,b4;
 float c1,c2,c3,c4,d1,d2,d3,d4;

 a1=in[0][0];b1=in[0][1];c1=in[0][2];d1=in[0][3];
 a2=in[1][0];b2=in[1][1];c2=in[1][2];d2=in[1][3];
 a3=in[2][0];b3=in[2][1];c3=in[2][2];d3=in[2][3];
 a4=in[3][0];b4=in[3][1];c4=in[3][2];d4=in[3][3];
 out[0][0]= det3x3(b2,b3,b4,c2,c3,c4,d2,d3,d4)/det;
 out[1][0]=-det3x3(a2,a3,a4,c2,c3,c4,d2,d3,d4)/det;
 out[2][0]= det3x3(a2,a3,a4,b2,b3,b4,d2,d3,d4)/det;
 out[3][0]=-det3x3(a2,a3,a4,b2,b3,b4,c2,c3,c4)/det;
 out[0][1]=-det3x3(b1,b3,b4,c1,c3,c4,d1,d3,d4)/det;
 out[1][1]= det3x3(a1,a3,a4,c1,c3,c4,d1,d3,d4)/det;
 out[2][1]=-det3x3(a1,a3,a4,b1,b3,b4,d1,d3,d4)/det;
 out[3][1]= det3x3(a1,a3,a4,b1,b3,b4,c1,c3,c4)/det;
 out[0][2]= det3x3(b1,b2,b4,c1,c2,c4,d1,d2,d4)/det;
 out[1][2]=-det3x3(a1,a2,a4,c1,c2,c4,d1,d2,d4)/det;
 out[2][2]= det3x3(a1,a2,a4,b1,b2,b4,d1,d2,d4)/det;
 out[3][2]=-det3x3(a1,a2,a4,b1,b2,b4,c1,c2,c4)/det;
 out[0][3]=-det3x3(b1,b2,b3,c1,c2,c3,d1,d2,d3)/det;
 out[1][3]= det3x3(a1,a2,a3,c1,c2,c3,d1,d2,d3)/det;
 out[2][3]=-det3x3(a1,a2,a3,b1,b2,b3,d1,d2,d3)/det;
 out[3][3]= det3x3(a1,a2,a3,b1,b2,b3,c1,c2,c3)/det;
}

float det4x4(Matrix4By4 m)
{float a1,a2,a3,a4,b1,b2,b3,b4;
 float c1,c2,c3,c4,d1,d2,d3,d4,ans;

 a1=m[0][0];b1=m[0][1];c1=m[0][2];d1=m[0][3];
 a2=m[1][0];b2=m[1][1];c2=m[1][2];d2=m[1][3];
 a3=m[2][0];b3=m[2][1];c3=m[2][2];d3=m[2][3];
 a4=m[3][0];b4=m[3][1];c4=m[3][2];d4=m[3][3];
 ans= a1*det3x3(b2,b3,b4,c2,c3,c4,d2,d3,d4)
     -b1*det3x3(a2,a3,a4,c2,c3,c4,d2,d3,d4)
     +c1*det3x3(a2,a3,a4,b2,b3,b4,d2,d3,d4)
     -d1*det3x3(a2,a3,a4,b2,b3,b4,c2,c3,c4);
 return(ans);
}

int MatrixInvert(Matrix4By4 in,Matrix4By4 out)
{ float det;

  det=det4x4(in);
  if (fabs(det)<PRECISION_LIMIT)  /* NO INVERSE */
    return(0);
  Adjoint(in,out,det);
  return(1);
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

void RotatePoint(Matrix3By4 Pin,float *Q, float *W,float Theta)
{ Matrix3By4 Pout;
  Matrix4By4 Rq;

 SetUpRotation(Q,W,Theta,Rq);
 Mult3X4(Pin,Rq,Pout);
 memcpy(Pin, Pout, sizeof(Matrix3By4));
}

void RotateOnePoint(float *Pin,float *Q, float *W,float Theta)
{ Matrix4By4 Rq;
  float Pout[4],Pby4[4];

 SetUpRotation(Q,W,Theta,Rq);
 memcpy(Pby4,Pin,row3size);Pby4[3]=1.0f;
 Mult1X4(Pby4,Rq,Pout);
 memcpy(Pin,Pout,row3size);
}

float RND01()
{ float num;

  num=(float)rand();
  return(num/IntMax);
}

/* number between -1 and 1 */
float RND11()
{ float num;

   num=(float)rand()-HalfIntMax;
   return(num/HalfIntMax);
}

int RNDSign()
{
  return((RND11()<0?-1:1));
}

float RND55()
{ float num;

  num=RND11();
  return(num/2);
}
int RND0x(int maxnum)
{ float num;
  int newnum;

   num=(float)rand();
   if (maxnum==0) return(0);
   newnum=(int)floor((++maxnum)*num/IntMax1);
   return(newnum>maxnum?maxnum:newnum);
}
BOOL CALLBACK DefaultSOTProc(
		HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
	IObjParam *ip = (IObjParam*)GetWindowLong(hWnd,GWL_USERDATA);

	switch (msg) {
		case WM_INITDIALOG:
			SetWindowLong(hWnd,GWL_USERDATA,lParam);
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			if (ip) ip->RollupMouseMessage(hWnd,msg,wParam,lParam);
			return FALSE;

		default:
			return FALSE;
		}
	return TRUE;
	}
void SpinnerOn(HWND hWnd,int SpinNum,int Winnum)
{	ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hWnd,SpinNum));
	spin2->Enable();
	EnableWindow(GetDlgItem(hWnd,Winnum),TRUE);
	ReleaseISpinner(spin2);

};
void SpinnerOff(HWND hWnd,int SpinNum,int Winnum)
{	ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hWnd,SpinNum));
	spin2->Disable();
	EnableWindow(GetDlgItem(hWnd,Winnum),FALSE);
	ReleaseISpinner(spin2);
};
BOOL SpinIsEnabled(HWND hWnd,int SpinNum)
{ BOOL ison;
  ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hWnd,SpinNum));
  ison=spin2->IsEnabled();
  ReleaseISpinner(spin2);
  return(ison);
}

Point3 CalcSpread(float divangle,Point3 oldnorm)
{ float Q[3];
  Point3 r;

  Q[0]=Q[1]=Q[2]=0.0f;
  r=Point3(RND11(),RND11(),RND11());
  r=Normalize(r^oldnorm);
  RotateOnePoint(&oldnorm.x,Q,&r.x,RND01()*divangle);
  return(oldnorm);
}

Mtl *CMaterials(Mtl *mat1, Mtl *mat2, int &c1, int &c2)
	{	
	MultiMtl *multi = NewDefaultMultiMtl();
	
	// First count the mats
	if (mat1->IsMultiMtl()) {		
		c1 = mat1->NumSubMtls();

		// Special case: If mat2 is present in mat1 then they 
		// are already combined. We just need to compute the offset.		
		for (int i=0; i<c1; i++) {
			if (mat1->GetSubMtl(i)==mat2) {
				c1 = i;
				return mat1;
				}
			}		

	} else {
		c1 = 1;
		}
	if (mat2->IsMultiMtl()) {
		c2 = mat2->NumSubMtls();		
	} else {
		c2 = 1;
		}

	// Set the number of slots
	multi->SetNumSubMtls(c1+c2);

	// Copy in materials from first mat
	int i=0;
	if (mat1->IsMultiMtl()) {
		for (; i<c1; i++) {
			multi->SetSubMtl(i,mat1->GetSubMtl(i));
			}
	} else {
		multi->SetSubMtl(0,mat1);
		i++;
		}
	
	// And from second
	if (mat2->IsMultiMtl()) {
		for (; i<c1+c2; i++) {
			multi->SetSubMtl(i,mat2->GetSubMtl(i-c1));
			}
	} else {
		multi->SetSubMtl(c1,mat2);
		}
		
	return multi;
	}

int nummaps(TriObject *wrap1,TriObject *wrap2,BOOL hdist,int &inmap,int &dmap,BOOL proxy)
{ inmap=wrap1->GetMesh().getNumMaps();
  if (wrap2) dmap=wrap2->GetMesh().getNumMaps();
/*  int tpre1=0,tpre2=0;BOOL t1[2],t2[2];
  for (int i=0;i<2;i++)
  { t1[i]=(wrap1->GetMesh().mapSupport(i));
    if (t1[i]) tpre1++;
	t2[i]=(wrap2->GetMesh().mapSupport(i));
	if (t2[i]) tpre2++;
  } 
  int m1=inmap-tpre1,m2=dmap-tpre2;
  int lowc=(t1[0]||t2[0]?1:0)+(t1[1]||t2[1]?1:0),stmap=0;
  if (proxy) dmap=0;else {stmap=dmap;dmap=m1+2;}
  if (hdist) {inmap=0;return stmap;}
  stmap=inmap;inmap=m2+2;if (dmap==0) return stmap;
  return m1+m2+lowc;*/
  if (proxy) dmap=0;
  if (hdist) inmap=0;
  if ((inmap>2)&&(dmap>2)) return (inmap+dmap-2);
  else return (inmap>dmap?inmap:dmap);
}
void setmaps(Mesh *newMesh,Mesh *mesh,int omp,int &cmp,int rep)
{ int newmp,fnum=mesh->getNumFaces(),k;
  int l,j,tfaces=newMesh->getNumFaces(),deltafaces=tfaces-fnum;
  BOOL firstblk=(rep==0);if (firstblk) rep=1;
  int cnt=2;
  for (int mp=2;(cnt<omp)&&(mp<mmap); mp++) 
   { int dtv=mesh->getNumMapVerts(mp);
	 if (!mesh->mapSupport(mp))
	 {// newMesh->setMapSupport (newmp, FALSE);
	   continue;
	 }
	 newmp=++cmp;
	 newMesh->setMapSupport (newmp, TRUE);
	 newMesh->setNumMapFaces(newmp, tfaces);
	 TVFace *mapf = mesh->mapFaces(mp);
	 UVVert *mapv = mesh->mapVerts(mp);
	 TVFace *nmapf = newMesh->mapFaces(newmp);
	 newMesh->setNumMapVerts (mp,dtv+1);
	 for (j=0,k=0; j<mesh->getNumMapVerts(mp); j++)
	 {	newMesh->setMapVert (newmp, k, mapv[j]);	}
	 newMesh->setMapVert(newmp,dtv,Zero);
	 Face *flist=mesh->faces;int cnt=0;
	 if (firstblk)
	 { cnt=deltafaces;for (l=0;l<deltafaces;l++) nmapf[l].setTVerts(dtv,dtv,dtv);}
	 else {for (l=fnum;l<tfaces;l++) nmapf[l].setTVerts(dtv,dtv,dtv);}
	 for (int l=0;l<rep;l++)
	   for (j=0; j<fnum; j++) 
		{ nmapf[cnt] = mapf[j];cnt++;}
	 cnt++;
   }
}
void mapstuff(Mesh *newMesh,TriObject *wrappee,TriObject *wrapper,int oldmap,int wrappermap,int rep)
{  Mesh *mesh=&(wrapper->GetMesh()),*meshee=&(wrappee->GetMesh());
   int cmp=1;
   setmaps(newMesh,mesh,wrappermap,cmp,0);
   setmaps(newMesh,meshee,oldmap,cmp,rep);
}
