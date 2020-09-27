#include "knotbase.h"

HINSTANCE hInstance;
static int controlsInit = FALSE;

const float IntMax=32767.0f;
const float IntMax1=32768.0f;
const float HalfIntMax=16383.5f;
#define ALEN 20
static char astring[ALEN];
ParticleMtl::ParticleMtl():Material() {
	Kd[0] = PARTICLE_R;
	Kd[1] = PARTICLE_G;
	Kd[2] = PARTICLE_B;
	Ks[0] = PARTICLE_R;
	Ks[1] = PARTICLE_G;
	Ks[2] = PARTICLE_B;
	shininess = (float)0.0;
	shadeLimit = GW_WIREFRAME;
	selfIllum = (float)1.0;
	}

int FloatEQ0(float number)
{return((FLOAT_EPSILON>=number)&&(number>=-FLOAT_EPSILON));
}
int SmallerEQ0(float number)
{return((SMALL_EPSILON>=number)&&(number>=-SMALL_EPSILON));
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
/* rand returns a number between 0 and 32767 */
/* number between 0 and 1 */
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

//------------------------------------------------------
// This is the interface to Jaguar:
//------------------------------------------------------

__declspec( dllexport ) const TCHAR *
LibDescription() { return GetString(IDS_AP_PQTORUSKNOTS); }

// This function returns the number of plug-in classes this DLL implements
__declspec( dllexport ) int 
LibNumberClasses() { return 2; }

// This function return the ith class descriptor. We have one.
__declspec( dllexport ) ClassDesc* 
LibClassDesc(int i) {
	switch(i){
	case 0:return GetTorusKnotDesc();
	case 1:return GetRingWaveObjDesc();
    default:return 0;}
 }

/*
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
*/

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) {
	hInstance = hinstDLL;

	if ( !controlsInit ) {
		controlsInit = TRUE;
		
		// jaguar controls
		InitCustomControls(hInstance);

#ifdef OLD3DCONTROLS
		// initialize 3D controls
		Ctl3dRegister(hinstDLL);
		Ctl3dAutoSubclass(hinstDLL);
#endif
		
		// initialize Chicago controls
		InitCommonControls();
		}

	switch(fdwReason) {
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		}
	return(TRUE);
	}


// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.

__declspec( dllexport ) ULONG 
LibVersion() { return VERSION_3DSMAX; }

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 1;
}

TCHAR *GetString(int id)
	{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
	}

void TurnButton(HWND hWnd,int SpinNum,BOOL ison)
{	ICustButton *iBut;
	iBut=GetICustButton(GetDlgItem(hWnd,SpinNum));
	if (iBut) 
	{ if (ison) iBut->Enable(); else iBut->Disable();
	}
	ReleaseICustButton(iBut);
};

TriObject *IsTriUseable(ObjectState os,TimeValue t)
{ 
  if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID)
  {	if (os.obj->IsSubClassOf(triObjectClassID)) 
      return (TriObject*)os.obj;
    else 
	{ if (os.obj->CanConvertToType(triObjectClassID)) 
	  	return (TriObject*)os.obj->ConvertToType(t,triObjectClassID);			
	}
  }
  return NULL;
}
ShapeObject *IsShapeUseable(Object *pobj,TimeValue t)
{ 
  if (pobj->IsShapeObject())
    return (ShapeObject*)pobj;
  else return NULL;
}
void MakeNodeList(INode *node,INodeTab *ntab,TimeValue t)
{  Object *pobj=NULL;
  if ((!node->IsGroupHead())&&((IsShapeUseable(node->EvalWorldState(t).obj,t))!=NULL))
   (*ntab).Append(1,&node,0);
}
void MakeGroupNodeList(INode *node,INodeTab *ntab,TimeValue t)
{ int nc;
  if ((nc=node->NumberOfChildren())>0)
	for (int j=0;j<nc;j++)
	{ INode *nxtnode=node->GetChildNode(j);
	  if (nxtnode->IsGroupHead()) MakeGroupNodeList(nxtnode,ntab,t);
	  else if (nxtnode->IsGroupMember()) MakeNodeList(nxtnode,ntab,t);
	}
}

int IncS(int s,int max)
{ s++;
  if (s>=max) s=s % max;
  return s;
}
int DecS(int s,int max)
{ s--;
  if (s<0) s=max-1;
  return s;
}

BOOL ReadInt(int *buf,FILE *f)
{ return(fread(buf,isize,1,f)==1);
}
BOOL WriteInt(int *buf,FILE *f)
{ return(fwrite(buf,isize,1,f)==1);
}

BOOL GenNewSaveFile(int osize,int size,int custsettings,FILE *f,TCHAR *filename,int vers)
{ int future=0;BOOL ok=TRUE;
  fclose(f);
  if ((f = _tfopen(filename, _T("r+b"))) == NULL) return FALSE;
  ok=(WriteInt(&custsettings,f)&&WriteInt(&vers,f)&&WriteInt(&size,f)&&WriteInt(&future,f));
 if (custsettings>0)
 { int cset=(custsettings-1),odata=osize+NLEN;
   AName Name;int ofs=(size-osize),tofs=cset*ofs;
   BYTE *data=new BYTE[size];for (int x=0;x<size;x++) data[x]=0;
   cset*=odata;cset+=HLEN;
   while (cset>=HLEN)
   { fseek(f,cset,SEEK_SET);
      if (fread(Name,NLEN,1,f)==1)
	 { if (fread(data,osize,1,f)!=1) goto badend;
       fseek(f,cset+tofs,SEEK_SET);
		if (fwrite(Name,1,NLEN,f)!=NLEN) goto badend;
	   if (fwrite(data,size,1,f)!=1) goto badend;
	 } else goto badend;
	 cset-=odata;tofs-=ofs;
   }
   delete[] data;
   rewind(f);
 }
 return ok;
 badend: fclose(f);return FALSE;
}

void SpinnerOn(HWND hWnd,int SpinNum)
{	ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hWnd,SpinNum));
	spin2->Enable();
	ReleaseISpinner(spin2);

};
void SpinnerOff(HWND hWnd,int SpinNum)
{	ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hWnd,SpinNum));
	spin2->Disable();
	ReleaseISpinner(spin2);
};
static unsigned int goodval;
static BOOL isbad;
static FILE *f;
#define MAXSERNUM	16
static char applicationID[MAXSERNUM];

// Prints out the application ID
void OutputApplicationID (HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hDC;
    InvalidateRect (hWnd, NULL, TRUE);
    hDC = BeginPaint (hWnd, &ps);
    SetBkMode (hDC, TRANSPARENT);
    TextOut (hDC, 0, 0, applicationID, strlen(applicationID));
    EndPaint (hWnd, &ps);
}

