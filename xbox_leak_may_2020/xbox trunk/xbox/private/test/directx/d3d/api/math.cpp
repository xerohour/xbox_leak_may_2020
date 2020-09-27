#include "d3dapi.hpp"
#include "xgmath.h"

//please pardon the macros. It saves a lot of copy/paste errors, and makes it really easy to change the testing algorithm.
//before preprocessing, this file is 28k. Afterwards, (not including the headers), it is 257k. 

//needed for the defines... Def(Float, variable)
typedef float XGFloat, D3DXFloat;

//repeat count, for speed tests
#define COUNT (1000)

//accuracty (like epsilon, but scaled instead of linear)
#define ACCURACY (((.99)))

LINKME(19);

#define EPSILON 0.0001f
static inline bool WithinEpsilon(float a, float b)
{
    float f = a - b;
    return -EPSILON <= f && f <= EPSILON;
}

//used in prepending the appropriate prefixes on function and type names
#define xg_(x) XG##x
#define dx_(x) D3DX##x

//used in creating variable names
#define _dx(x) x##_D3DX
#define _xg(x) x##_XG

//_P is postfixed to a variable, to imply that it is a pointer
#define _P(x) x##_P
//_Size1 is postfixed to a variable name, being a pointer to the size, or the size.
#define _S1(x) x##_Size1

//used to declare PLANE, VECTOR2, VECTOR3, for use when the address is to be passed as a parameter to the function being tested.
#define Dec(type, x)  xg_(type)   _xg(x); dx_(type)   _dx(x); int _S1(x) = sizeof(_dx(x))/sizeof(float); xg_(type)*    _xg(x)##_P = &_xg(x); dx_(type)*   _dx(x)##_P = &(_dx(x)); 

//used to declare a pointer to a type that will be used as a return type (like to Vec2Add)
#define DecP(type, x) xg_(type)*  _xg(x); dx_(type)*  _dx(x); int x##_Size = sizeof(*_dx(x))/sizeof(float); int* _S1(x)=&x##_Size; xg_(type)**  _xg(x)##_P = &(_xg(x)); dx_(type)**  _dx(x)##_P = &(_dx(x)); 

//this was used to declare XGMATRIXA, but since that has been replaced, it is just shorter than typing Dec(MATRIX, )
#define DecMat(x)    xg_(MATRIX) _xg(x); dx_(MATRIX) _dx(x); int _S1(x) = 16;                           xg_(MATRIX)* _xg(x)##_P = &_xg(x); dx_(MATRIX)* _dx(x)##_P = &(_dx(x)); 

//declares a float that will be passed as a paramter (instead of by address)
#define DecFlt(x)    float        _xg(x); float       _dx(x); int x##_Size = 1; int* _S1(x) = &x##_Size; float* _xg(x)##_P1 = &_xg(x); float* _dx(x)##_P1 = &(_dx(x)); float** _xg(x)##_P = (&_xg(x)##_P1); float** _dx(x)##_P = (&_dx(x)##_P1);

//declare a type, like BOOL, that doesn't have the XG/D3DX prefix, but will be used as a return type
#define DecTyp(type, x) type        _xg(x); type       _dx(x); int x##_Size = 1; int* _S1(x) = &x##_Size; type* _xg(x)##_P1 = &_xg(x); type* _dx(x)##_P1 = &(_dx(x)); type** _xg(x)##_P = (&_xg(x)##_P1); type** _dx(x)##_P = (&_dx(x)##_P1);

//for parameters, pFloatify expands to be the address of (x) that can be used to fill the variable x with random data
#define pFloatify(x) ((float*)(*(_P(x))))

//used by Pre, below.
//used to pass various parameters to a function, with the appropriate postfix to each variable
#define xgPre1(p1)                   _xg(p1)
#define xgPre2(p1,p2)                _xg(p1), _xg(p2)
#define xgPre3(p1,p2,p3)             _xg(p1), _xg(p2), _xg(p3)
#define xgPre4(p1,p2,p3,p4)          _xg(p1), _xg(p2), _xg(p3), _xg(p4)
#define xgPre5(p1,p2,p3,p4,p5)       _xg(p1), _xg(p2), _xg(p3), _xg(p4), _xg(p5)
#define xgPre6(p1,p2,p3,p4,p5,p6)    _xg(p1), _xg(p2), _xg(p3), _xg(p4), _xg(p5), _xg(p6)
#define xgPre7(p1,p2,p3,p4,p5,p6,p7) _xg(p1), _xg(p2), _xg(p3), _xg(p4), _xg(p5), _xg(p6), _xg(p7)
#define dxPre1(p1)                   _dx(p1)
#define dxPre2(p1,p2)                _dx(p1), _dx(p2)
#define dxPre3(p1,p2,p3)             _dx(p1), _dx(p2), _dx(p3)
#define dxPre4(p1,p2,p3,p4)          _dx(p1), _dx(p2), _dx(p3), _dx(p4)
#define dxPre5(p1,p2,p3,p4,p5)       _dx(p1), _dx(p2), _dx(p3), _dx(p4), _dx(p5)
#define dxPre6(p1,p2,p3,p4,p5,p6)    _dx(p1), _dx(p2), _dx(p3), _dx(p4), _dx(p5), _dx(p6)
#define dxPre7(p1,p2,p3,p4,p5,p6,p7) _dx(p1), _dx(p2), _dx(p3), _dx(p4), _dx(p5), _dx(p6), _dx(p7)

//used to check a list of parameters
#define CheckPre1(p1)                   CheckRes &= Check(p1)
#define CheckPre2(p1,p2)                CheckRes &= Check(p1) & Check(p2)
#define CheckPre3(p1,p2,p3)             CheckRes &= Check(p1) & Check(p2) & Check(p3)
#define CheckPre4(p1,p2,p3,p4)          CheckRes &= Check(p1) & Check(p2) & Check(p3) & Check(p4)
#define CheckPre5(p1,p2,p3,p4,p5)       CheckRes &= Check(p1) & Check(p2) & Check(p3) & Check(p4) & Check(p5)
#define CheckPre6(p1,p2,p3,p4,p5,p6)    CheckRes &= Check(p1) & Check(p2) & Check(p3) & Check(p4) & Check(p5) & Check(p6)
#define CheckPre7(p1,p2,p3,p4,p5,p6,p7) CheckRes &= Check(p1) & Check(p2) & Check(p3) & Check(p4) & Check(p5) & Check(p6) & Check(p7)

//used to fill various parameters
#define FillPre1(p1)                   FillBoth(p1);
#define FillPre2(p1,p2)                FillBoth(p1); FillBoth(p2);
#define FillPre3(p1,p2,p3)             FillBoth(p1); FillBoth(p2); FillBoth(p3);
#define FillPre4(p1,p2,p3,p4)          FillBoth(p1); FillBoth(p2); FillBoth(p3); FillBoth(p4);
#define FillPre5(p1,p2,p3,p4,p5)       FillBoth(p1); FillBoth(p2); FillBoth(p3); FillBoth(p4); FillBoth(p5);
#define FillPre6(p1,p2,p3,p4,p5,p6)    FillBoth(p1); FillBoth(p2); FillBoth(p3); FillBoth(p4); FillBoth(p5); FillBoth(p6);
#define FillPre7(p1,p2,p3,p4,p5,p6,p7) FillBoth(p1); FillBoth(p2); FillBoth(p3); FillBoth(p4); FillBoth(p5); FillBoth(p6); FillBoth(p7);

//prepares the parameter list _p_ for use with the macro with prefix pre (see list above). cnt specifies the number of parameters in teh list.
#define Pre(pre, cnt, _p_) pre##Pre##cnt##_p_

//suffex the paramters in (list _p_ with cnt elements) with the postfix (pre), and call func with those parameters
#define Call(func, pre, cnt, _p_) pre##_(func) (Pre(pre,cnt,_p_)) 

//compares the dx and xg versions of all elements in _p_ 
#define CheckAll(cnt,_p_) Pre(Check,cnt,_p_)

//fills all elements of the xg and dx versions of all elements in _p_ with random single-precision float values
#define FillAll(cnt,_p_) Pre(Fill,cnt,_p_)

/*
#define Callboth1(func,p1)          do { Call1(xg, func, p1)         ; Call1(dx, func, p1)          } while(0)
#define Callboth2(func,p1,p2)       do { Call2(xg, func, p1,p2)      ; Call2(dx, func, p1,p2)       } while(0)
#define Callboth3(func,p1,p2,p3)    do { Call3(xg, func, p1,p2,p3)   ; Call3(dx, func, p1,p2,p3)    } while(0)
#define Callboth4(func,p1,p2,p3,p4) do { Call4(xg, func, p1,p2,p3,p4); Call4(dx, func, p1,p2,p3,p4) } while(0)
*/

float randf() {
	//generate a totally random float. This can include infinity, but no other non-numbers.

	DWORD mantissa;
	DWORD expo = (rand() & 0x11f) + 0x6d; //exponent and sign
	//the 0x3f is added to keep the exponent in the range of (about) -32 to 32, in order to prevent overflows
/*
	if((expo & 255) == 255) //infinity or qnan
	{
		mantissa = 0;
	} else {
*/
	mantissa = (rand() + (rand() << 15)) & ((1 << 23) - 1);

	DWORD ret = mantissa | (expo << 23);
	return *(float*)&ret;	
}

bool CmpFloats(float*a, float*b, int count) {
	volatile int i; //force the optimizer to keep i in memory so we can actually read it in the debugger if we break
	if(a == NULL || b == NULL) {
		if (a == b) return true;
		else return false;
	}
	for(i = 0; i < count; i++) {
		if(*(DWORD*)(&a[i]) == *(DWORD*)(&b[i])) continue; //catch infinity
		if(((*(DWORD*)(&a[i]) & 0x7f800000) == 0x7f800000) && ((*(DWORD*)(&b[i]) & 0x7f800000) == 0x7f800000)) continue;
		if(WithinEpsilon(a[i],b[i])) continue;
		if(fabs(a[i]*ACCURACY)<fabs(b[i]) && fabs(b[i]*ACCURACY)<fabs(a[i])) continue;
//		_asm {int 3};
		return false;
	}
	return true;
}

//the number of floats in x
#define FloatCount(x) (*(_S1(x)))

//fill the dx and xg versions of x with floating-point values. 
//with matracies, vectors, etc, it fills all elements of those variables.
#define FillBoth(x)	do { for (int i = 0; i < FloatCount(x); i++) { ((pFloatify(_dx(x)))[i]) = ((pFloatify(_xg(x)))[i]) = randf(); }; } while (0)

//sticks a dword in the specified range into x, where x is a variable the size of a dword
#define FillDword(x, _min, _max) (((int*)(pFloatify(_dx(x))))[0]) = (((int*)(pFloatify(_xg(x))))[0]) = RandMM(_min, _max)
#define FillViewPort(x) do { FillVP((D3DVIEWPORT8*)_xg(x), (D3DVIEWPORT8*)_dx(x)); } while(0)

//return a random int between mi and ma
int RandMM(int mi, int ma) {
	int r = rand() + (rand() << 15);
	return (r % (ma + 1 - mi)) + mi;
}
void FillVP(D3DVIEWPORT8*pvp1,D3DVIEWPORT8*pvp2){
	pvp2->X =     pvp1->X = RandMM(0,630);
	pvp2->Width = pvp1->Width = RandMM(1,640-pvp1->X);
	pvp2->Y =     pvp1->Y = RandMM(0, 470);
	pvp2->Height =pvp1->Height = RandMM(1,480-pvp1->Y);
	float f1 = (float)fabs(randf());
	float f2 = (float)fabs(randf());
	pvp2->MinZ =  pvp1->MinZ = min(f1,f2);
	pvp2->MaxZ =  pvp1->MaxZ = max(f1,f2);
}


//compares the xg and dx versions of x
#define Check(x) CmpFloats(pFloatify(_dx(x)), pFloatify(_xg(x)), FloatCount(x))

//declare and initialize variables used in speed tests
#define InitSpd __int64 _dx(Start), _xg(Start), _dx(End)=0, _xg(End)=0; bool CheckRes = true; bool SpdRes = true; int Counter;

//wrap code (x) in speed-checking code. pre is either xg or dx.
#define SpdCheck(pre, code) do { _##pre(Start) = GetTsc(); code; _##pre(End) += GetTsc() - _##pre(Start); } while(0)

//only used if the return type is void
#define Test(func,cnt,_p_) do {													\
							xLog(hlog,XLL_INFO,#func);							\
							for(Counter = 0; Counter < COUNT; Counter++) {		\
								FillAll(cnt,_p_);								\
								Call(func,dx,cnt,_p_);							\
								Call(func,xg,cnt,_p_);							\
								CheckAll(cnt,_p_);								\
							}													\
							for(Counter = 0; Counter < COUNT; Counter++) {		\
								SpdCheck(dx, Call(func,dx,cnt,_p_););			\
								SpdCheck(xg, Call(func,xg,cnt,_p_););			\
							};													\
							CHECKRESULT((_xg(End) * .97 <= _dx(End)) && "speed test");	\
                            if(1/*WASBADRESULT()*/) xLog(hlog,XLL_INFO,"xg uS: %I64d. dx us: %I64d", _xg(End), _dx(End)); \
							CHECKRESULT(CheckRes && "Float Comparison");		\
						} while (0)

//grabs the return type. Ret must be declared as the type of the return-type of the function tested
#define TestRet(func,cnt,_p_) do {												\
							xLog(hlog,XLL_INFO,#func);							\
							for(Counter = 0; Counter < COUNT; Counter++) {		\
								FillAll(cnt,_p_);								\
								_dx(Ret) = Call(func,dx,cnt,_p_);				\
								_xg(Ret) = Call(func,xg,cnt,_p_);				\
								CheckAll(cnt,_p_);								\
								Check(Ret);										\
							}													\
							for(Counter = 0; Counter < COUNT; Counter++) {		\
								SpdCheck(dx, Call(func,dx,cnt,_p_););			\
								SpdCheck(xg, Call(func,xg,cnt,_p_););			\
							};													\
							CHECKRESULT((_xg(End) * .97 <= _dx(End)) && "spd");	\
                            if(1/*WASBADRESULT()*/) xLog(hlog,XLL_INFO,"xg uS: %I64d. dx us: %I64d", _xg(End), _dx(End)); \
							CHECKRESULT(CheckRes && "Float Comparison");		\
						} while (0)


//fpu accuracy == saved in the first test, restored in the end.
//0 = single. 1 == double. 3 == extended-double
int gFpuAccuracy; 

int GetFpuAccuracy()
{
	int i=0;
	_asm fstcw [i];
	return (i >> 8) & 3;
}

void SetFpuAccuracy(int a)
{
	int i=0;
	_asm fstcw [i];
	i &= ~0x300;
	i |= ((a & 3) << 8);
	_asm fldcw [i];
}

#pragma data_seg(".d3dapi$test026") //run nearly first
extern "C" void DebugPrint(const char*,...);
BEGINTEST(testMathnew)
{
	int* blah = new int[63];
	DebugPrint("new int[64]: %p\n", blah);
	int* bla  = new int[63];
	DebugPrint("new int[64]: %p\n", bla);
	delete[] bla, blah;

}
ENDTEST();

BEGINTEST(testMathVec2Inl)
{

	gFpuAccuracy = GetFpuAccuracy();
	SetFpuAccuracy(3);

	xStartVariation(hlog, "Vector2 Inline tests");

	{   InitSpd;
		DecFlt(Ret); Dec(VECTOR2, v);
		TestRet(Vec2Length, 1, (&v));	
	}
	{   InitSpd;
		DecFlt(Ret); Dec(VECTOR2, v);
		TestRet(Vec2LengthSq, 1, (&v));	
	}
	{   InitSpd;
		DecFlt(Ret); Dec(VECTOR2, v1); Dec(VECTOR2, v2);
		TestRet(Vec2Dot, 2, (&v1, &v2));	
	}
	{   InitSpd;
		DecFlt(Ret); Dec(VECTOR2, v1); Dec(VECTOR2, v2);
		TestRet(Vec2CCW, 2, (&v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR2, Ret); Dec(VECTOR2, Out); Dec(VECTOR2, v1); Dec(VECTOR2, v2);
		TestRet(Vec2Add, 3, (&Out, &v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR2, Ret); Dec(VECTOR2, Out); Dec(VECTOR2, v1); Dec(VECTOR2, v2);
		TestRet(Vec2Subtract, 3, (&Out, &v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR2, Ret); Dec(VECTOR2, Out); Dec(VECTOR2, v1); Dec(VECTOR2, v2);
		TestRet(Vec2Minimize, 3, (&Out, &v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR2, Ret); Dec(VECTOR2, Out); Dec(VECTOR2, v1); Dec(VECTOR2, v2);
		TestRet(Vec2Maximize, 3, (&Out, &v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR2, Ret); Dec(VECTOR2, Out); Dec(VECTOR2, v); DecFlt(s);
		TestRet(Vec2Scale, 3, (&Out, &v, s));	
	}
	{   InitSpd;
		DecP(VECTOR2, Ret); Dec(VECTOR2, Out); Dec(VECTOR2, v1); Dec(VECTOR2, v2); DecFlt(s);
		TestRet(Vec2Lerp, 4, (&Out, &v1, &v2, s));	
	}

	xEndVariation(hlog);

} 
ENDTEST();

BEGINTEST(testMathVec2NonInl)
{

	xStartVariation(hlog, "Vector2 non-inline tests");

	{   InitSpd;
		DecP(VECTOR2, Ret); Dec(VECTOR2, Out); Dec(VECTOR2, v);
		TestRet(Vec2Normalize, 2, (&Out, &v));	
	}
	{	InitSpd;
		DecP(VECTOR2, Ret); Dec(VECTOR2, Out); Dec(VECTOR2, v1); Dec(VECTOR2, t1); Dec(VECTOR2, v2); Dec(VECTOR2, t2); DecFlt(s);
		TestRet(Vec2Hermite, 6, (&Out,&v1,&t1,&v2,&t2,s)); 
	}
	{	InitSpd;
		DecP(VECTOR2, Ret); Dec(VECTOR2, Out); Dec(VECTOR2, v0); Dec(VECTOR2, v1); Dec(VECTOR2, v2); Dec(VECTOR2, v3); DecFlt(s);
		TestRet(Vec2CatmullRom, 6, (&Out, &v0, &v1, &v2, &v3, s)); 
	}
	{	InitSpd;
		DecP(VECTOR2, Ret); Dec(VECTOR2, Out); Dec(VECTOR2, v1); Dec(VECTOR2, v2); Dec(VECTOR2, v3); DecFlt(f); DecFlt(g);
		TestRet(Vec2BaryCentric, 6, (&Out, &v1, &v2, &v3, f, g)); 
	}
	{	InitSpd;
		DecP(VECTOR4, Ret); Dec(VECTOR4, Out); Dec(VECTOR2, v); DecMat(m);
		TestRet(Vec2Transform, 3, (&Out, &v, &m)); 
	}
	/*
	{	InitSpd;
		DecP(VECTOR2, Ret); Dec(VECTOR2, Out); Dec(VECTOR2, v); DecMat(m);
		TestRet(Vec2TransformCoord, 3, (&Out, &v, &m)); 
	}
	*/
	{	InitSpd;
		DecP(VECTOR2, Ret); Dec(VECTOR2, Out); Dec(VECTOR2, v); DecMat(m);
		TestRet(Vec2TransformNormal, 3, (&Out, &v, &m)); 
	}

	xEndVariation(hlog);
} 
ENDTEST();

BEGINTEST(testMathVec3Inl)
{
	xStartVariation(hlog, "Vector3 inline tests");

	{   InitSpd;
		DecFlt(Ret); Dec(VECTOR3, v);
		TestRet(Vec3Length, 1, (&v));	
	}
	{   InitSpd;
		DecFlt(Ret); Dec(VECTOR3, v);
		TestRet(Vec3LengthSq, 1, (&v));	
	}
	{   InitSpd;
		DecFlt(Ret); Dec(VECTOR3, v1); Dec(VECTOR3, v2);
		TestRet(Vec3Dot, 2, (&v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v1); Dec(VECTOR3, v2);
		TestRet(Vec3Add, 3, (&Out, &v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v1); Dec(VECTOR3, v2);
		TestRet(Vec3Cross, 3, (&Out, &v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v1); Dec(VECTOR3, v2);
		TestRet(Vec3Subtract, 3, (&Out, &v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v1); Dec(VECTOR3, v2);
		TestRet(Vec3Minimize, 3, (&Out, &v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v1); Dec(VECTOR3, v2);
		TestRet(Vec3Maximize, 3, (&Out, &v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v); DecFlt(s);
		TestRet(Vec3Scale, 3, (&Out, &v, s));	
	}
	{   InitSpd;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v1); Dec(VECTOR3, v2); DecFlt(s);
		TestRet(Vec3Lerp, 4, (&Out, &v1, &v2, s));	
	}

	xEndVariation(hlog);
} 
ENDTEST();

BEGINTEST(testMathVec3NonInl)
{

	xStartVariation(hlog, "Vector3 non-inline tests");

	{	InitSpd;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v);
		TestRet(Vec3Normalize, 2, (&Out, &v)); 
	}
	{	InitSpd;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v1); Dec(VECTOR3, t1); Dec(VECTOR3, v2); Dec(VECTOR3, t2); DecFlt(s);
		TestRet(Vec3Hermite, 6, (&Out,&v1,&t1,&v2,&t2,s)); 
	}
	{	InitSpd;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v0); Dec(VECTOR3, v1); Dec(VECTOR3, v2); Dec(VECTOR3, v3); DecFlt(s);
		TestRet(Vec3CatmullRom, 6, (&Out, &v0, &v1, &v2, &v3, s)); 
	}
	{	InitSpd;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v1); Dec(VECTOR3, v2); Dec(VECTOR3, v3); DecFlt(f); DecFlt(g);
		TestRet(Vec3BaryCentric, 6, (&Out, &v1, &v2, &v3, f, g)); 
	}
	{	InitSpd;
		DecP(VECTOR4, Ret); Dec(VECTOR4, Out); Dec(VECTOR3, v); DecMat(m);
		TestRet(Vec3Transform, 3, (&Out, &v, &m)); 
	}
	{	InitSpd;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v); DecMat(m);
		TestRet(Vec3TransformCoord, 3, (&Out, &v, &m)); 
	}
	{	InitSpd;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v); DecMat(m);
		TestRet(Vec3TransformNormal, 3, (&Out, &v, &m)); 
	}
	//viewport is a dword thing, not a float thing. FillPre6 must be redefined to fill it appropriately
	{	InitSpd;
		typedef D3DVIEWPORT8 XGVIEWPORT8, D3DXVIEWPORT8;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v); Dec(VIEWPORT8, vp); DecMat(proj); DecMat(view); DecMat(world);
		#undef FillPre6
		#define FillPre6(p1,p2,p3,p4,p5,p6) FillBoth(p1); FillBoth(p2); FillViewPort(p3); FillBoth(p4); FillBoth(p5); FillBoth(p6);
		TestRet(Vec3Project, 6, (&Out, &v, &vp, &proj, &view, &world)); 
		#undef FillPre6
		//reset FillPre6 the way it should be
		#define FillPre6(p1,p2,p3,p4,p5,p6) FillBoth(p1); FillBoth(p2); FillBoth(p3); FillBoth(p4); FillBoth(p5); FillBoth(p6);
	}
	{	InitSpd;
		typedef D3DVIEWPORT8 XGVIEWPORT8, D3DXVIEWPORT8;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(VECTOR3, v); Dec(VIEWPORT8, vp); DecMat(proj); DecMat(view); DecMat(world);
		#undef FillPre6
		#define FillPre6(p1,p2,p3,p4,p5,p6) FillBoth(p1); FillBoth(p2); FillDword(p3,0,640); FillBoth(p4); FillBoth(p5); FillBoth(p6);
		TestRet(Vec3Unproject, 6, (&Out, &v, &vp, &proj, &view, &world)); 
		#undef FillPre6
		#define FillPre6(p1,p2,p3,p4,p5,p6) FillBoth(p1); FillBoth(p2); FillBoth(p3); FillBoth(p4); FillBoth(p5); FillBoth(p6);
	}

	xEndVariation(hlog);
} 
ENDTEST();

BEGINTEST(testMathVec4Inl)
{
	xStartVariation(hlog, "Vector4 inline tests");

	{   InitSpd;
		DecFlt(Ret); Dec(VECTOR4, v);
		TestRet(Vec4Length, 1, (&v));	
	}
	{   InitSpd;
		DecFlt(Ret); Dec(VECTOR4, v);
		TestRet(Vec4LengthSq, 1, (&v));	
	}
	{   InitSpd;
		DecFlt(Ret); Dec(VECTOR4, v1); Dec(VECTOR4, v2);
		TestRet(Vec4Dot, 2, (&v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR4, Ret); Dec(VECTOR4, Out); Dec(VECTOR4, v1); Dec(VECTOR4, v2);
		TestRet(Vec4Add, 3, (&Out, &v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR4, Ret); Dec(VECTOR4, Out); Dec(VECTOR4, v1); Dec(VECTOR4, v2);
		TestRet(Vec4Subtract, 3, (&Out, &v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR4, Ret); Dec(VECTOR4, Out); Dec(VECTOR4, v1); Dec(VECTOR4, v2);
		TestRet(Vec4Minimize, 3, (&Out, &v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR4, Ret); Dec(VECTOR4, Out); Dec(VECTOR4, v1); Dec(VECTOR4, v2);
		TestRet(Vec4Maximize, 3, (&Out, &v1, &v2));	
	}
	{   InitSpd;
		DecP(VECTOR4, Ret); Dec(VECTOR4, Out); Dec(VECTOR4, v); DecFlt(s);
		TestRet(Vec4Scale, 3, (&Out, &v, s));	
	}
	{   InitSpd;
		DecP(VECTOR4, Ret); Dec(VECTOR4, Out); Dec(VECTOR4, v1); Dec(VECTOR4, v2); DecFlt(s);
		TestRet(Vec4Lerp, 4, (&Out, &v1, &v2, s));	
	}

	xEndVariation(hlog);
} 
ENDTEST();

BEGINTEST(testMathVec4NonInl)
{

	xStartVariation(hlog, "Vector4 non-inline tests");

	{	InitSpd;
		DecP(VECTOR4, Ret); Dec(VECTOR4, Out); Dec(VECTOR4, v);
		TestRet(Vec4Normalize, 2, (&Out, &v)); 
	}
	{	InitSpd;
		DecP(VECTOR4, Ret); Dec(VECTOR4, Out); Dec(VECTOR4, v1); Dec(VECTOR4, t1); Dec(VECTOR4, v2); Dec(VECTOR4, t2); DecFlt(s);
		TestRet(Vec4Hermite, 6, (&Out,&v1,&t1,&v2,&t2,s)); 
	}
	{	InitSpd;
		DecP(VECTOR4, Ret); Dec(VECTOR4, Out); Dec(VECTOR4, v0); Dec(VECTOR4, v1); Dec(VECTOR4, v2); Dec(VECTOR4, v3); DecFlt(s);
		TestRet(Vec4CatmullRom, 6, (&Out, &v0, &v1, &v2, &v3, s)); 
	}
	{	InitSpd;
		DecP(VECTOR4, Ret); Dec(VECTOR4, Out); Dec(VECTOR4, v1); Dec(VECTOR4, v2); Dec(VECTOR4, v3); DecFlt(f); DecFlt(g);
		TestRet(Vec4BaryCentric, 6, (&Out, &v1, &v2, &v3, f, g)); 
	}
	{	InitSpd;
		DecP(VECTOR4, Ret); Dec(VECTOR4, Out); Dec(VECTOR4, v); DecMat(m);
		TestRet(Vec4Transform, 3, (&Out, &v, &m)); 
	}
	
	xEndVariation(hlog);
}
ENDTEST();

BEGINTEST(testMathMatrixInl)
{
	xStartVariation(hlog, "Matrix inline tests");
	{	InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); 
		TestRet(MatrixIdentity, 1, (&Out)); 
	}
	//unimportant todo: give MatrixIsIdentity some good/bad cases
	{	InitSpd;
		DecTyp(BOOL, Ret); DecMat(m); 
		TestRet(MatrixIsIdentity, 1, (&m)); 
	}
	xEndVariation(hlog);
}
ENDTEST();

BEGINTEST(testMathMatrixNonInl)
{
	xStartVariation(hlog, "Matrix non-inline tests");
	{   InitSpd;
		DecFlt(Ret); DecMat(m);
		TestRet(MatrixfDeterminant, 1, (&m));
	}
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); DecMat(m1); DecMat(m2);
		TestRet(MatrixMultiply, 3, (&Out, &m1, &m2));
	}
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); DecMat(m);
		TestRet(MatrixTranspose, 2, (&Out, &m));
	}

	//TODO: give MatrixInverse some specific failure cases
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); Dec(Float, f); DecMat(m);
		TestRet(MatrixInverse, 3, (&Out, &f, &m));
	}
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); DecFlt(x); DecFlt(y); DecFlt(z);
		TestRet(MatrixScaling, 4, (&Out, x, y, z));
	}
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); DecFlt(x); DecFlt(y); DecFlt(z);
		TestRet(MatrixTranslation, 4, (&Out, x, y, z));
	}
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); DecFlt(x); 
		TestRet(MatrixRotationX, 2, (&Out, x));
	}
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); DecFlt(x); 
		TestRet(MatrixRotationY, 2, (&Out, x));
	}
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); DecFlt(x); 
		TestRet(MatrixRotationY, 2, (&Out, x));
	}
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); Dec(VECTOR3, v); DecFlt(a); 
		TestRet(MatrixRotationAxis, 3, (&Out, &v, a));
	}
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); Dec(QUATERNION, q); 
		TestRet(MatrixRotationQuaternion, 2, (&Out, &q));
	}
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); DecFlt(x); DecFlt(y); DecFlt(z);
		TestRet(MatrixRotationYawPitchRoll, 4, (&Out, x, y, z));
	}
	//todo: check this with NULL params
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); Dec(VECTOR3, ss); Dec(QUATERNION, sr); Dec(VECTOR3, sc); Dec(VECTOR3, rc); Dec(QUATERNION, r); Dec(VECTOR3, t); 
		TestRet(MatrixTransformation, 7, (&Out, &ss, &sr, &sc, &rc, &r, &t));
	}
	//todo: check this with null params
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); DecFlt(s); Dec(VECTOR3, rc); Dec(QUATERNION, r); Dec(VECTOR3, t); 
		TestRet(MatrixAffineTransformation, 5, (&Out, s, &rc, &r, &t));
	}
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); Dec(VECTOR3, eye); Dec(VECTOR3, at); Dec(VECTOR3, up); 
		TestRet(MatrixLookAtRH, 4, (&Out, &eye, &at, &up));
	}
	{   InitSpd;
		DecP(MATRIX, Ret); DecMat(Out); Dec(VECTOR3, eye); Dec(VECTOR3, at); Dec(VECTOR3, up); 
		TestRet(MatrixLookAtLH, 4, (&Out, &eye, &at, &up));
	}
	{   InitSpd;
		DecP(MATRIX, Ret);DecMat(Out); DecFlt(w);DecFlt(h);DecFlt(zn);DecFlt(zf);
		TestRet(MatrixPerspectiveRH, 5, (&Out, w,h,zn,zf));
	}
	{   InitSpd;
		DecP(MATRIX, Ret);DecMat(Out); DecFlt(w);DecFlt(h);DecFlt(zn);DecFlt(zf);
		TestRet(MatrixPerspectiveLH, 5, (&Out, w,h,zn,zf));
	}
	{   InitSpd;
		DecP(MATRIX, Ret);DecMat(Out); DecFlt(fovy);DecFlt(a);DecFlt(zn);DecFlt(zf);
		TestRet(MatrixPerspectiveFovRH, 5, (&Out, fovy,a,zn,zf));
	}
	{   InitSpd;
		DecP(MATRIX, Ret);DecMat(Out); DecFlt(fovy);DecFlt(a);DecFlt(zn);DecFlt(zf);
		TestRet(MatrixPerspectiveFovLH, 5, (&Out, fovy,a,zn,zf));
	}
	{   InitSpd;
		DecP(MATRIX, Ret);DecMat(Out); DecFlt(l);DecFlt(r);DecFlt(b);DecFlt(t);DecFlt(zn);DecFlt(zf);
		TestRet(MatrixPerspectiveOffCenterRH, 7, (&Out, l,r,b,t,zn,zf));
	}
	{   InitSpd;
		DecP(MATRIX, Ret);DecMat(Out); DecFlt(l);DecFlt(r);DecFlt(b);DecFlt(t);DecFlt(zn);DecFlt(zf);
		TestRet(MatrixPerspectiveOffCenterLH, 7, (&Out, l,r,b,t,zn,zf));
	}
	{   InitSpd;
		DecP(MATRIX, Ret);DecMat(Out); DecFlt(w);DecFlt(h);DecFlt(zn);DecFlt(zf);
		TestRet(MatrixOrthoRH, 5, (&Out, w,h,zn,zf));
	}
	{   InitSpd;
		DecP(MATRIX, Ret);DecMat(Out); DecFlt(w);DecFlt(h);DecFlt(zn);DecFlt(zf);
		TestRet(MatrixOrthoLH, 5, (&Out, w,h,zn,zf));
	}
	{   InitSpd;
		DecP(MATRIX, Ret);DecMat(Out); DecFlt(l);DecFlt(r);DecFlt(b);DecFlt(t);DecFlt(zn);DecFlt(zf);
		TestRet(MatrixOrthoOffCenterRH, 7, (&Out, l,r,b,t,zn,zf));
	}
	{   InitSpd;
		DecP(MATRIX, Ret);DecMat(Out); DecFlt(l);DecFlt(r);DecFlt(b);DecFlt(t);DecFlt(zn);DecFlt(zf);
		TestRet(MatrixOrthoOffCenterLH, 7, (&Out, l,r,b,t,zn,zf));
	}

	{   InitSpd;
		DecP(MATRIX, Ret);DecMat(Out); Dec(VECTOR4, light); Dec(PLANE, p);
		TestRet(MatrixShadow, 3, (&Out, &light, &p));
	}
	{   InitSpd;
		DecP(MATRIX, Ret);DecMat(Out); Dec(PLANE, p);
		TestRet(MatrixReflect, 2, (&Out, &p));
	}
	xEndVariation(hlog);
}
ENDTEST();

BEGINTEST(testMathQuatInl)
{
	xStartVariation(hlog,"Quaternion inl");
	{   InitSpd;
		DecFlt(Ret); Dec(QUATERNION, q);
		TestRet(QuaternionLength, 1, (&q));
	}
	{   InitSpd;
		DecFlt(Ret); Dec(QUATERNION, q);
		TestRet(QuaternionLengthSq, 1, (&q));
	}
	{   InitSpd;
		DecFlt(Ret); Dec(QUATERNION, q); Dec(QUATERNION, q2);
		TestRet(QuaternionDot, 2, (&q, &q2));
	}
	{   InitSpd;
		DecP(QUATERNION, Ret); Dec(QUATERNION, Out);
		TestRet(QuaternionIdentity, 1, (&Out));
	}
	//unimportant todo: check boundary and close conditions
	{   InitSpd;
		DecTyp(BOOL, Ret); Dec(QUATERNION, Out);
		TestRet(QuaternionIsIdentity, 1, (&Out));
	}
	{   InitSpd;
		DecP(QUATERNION, Ret); Dec(QUATERNION, q); Dec(QUATERNION, q2);
		TestRet(QuaternionConjugate, 2, (&q, &q2));
	}

	xEndVariation(hlog);
}
ENDTEST();

BEGINTEST(testMathQuatNonInl)
{
	xStartVariation(hlog,"Quaternion non-inl");
	{   InitSpd;
		Dec(QUATERNION, q); Dec(VECTOR3, ax); Dec(Float, an);
		Test(QuaternionToAxisAngle, 3, (&q, &ax, &an));
	}
	{   InitSpd;
		DecP(QUATERNION, Ret); Dec(QUATERNION, Out); DecMat(m);
		TestRet(QuaternionRotationMatrix, 2, (&Out, &m));
	}
	{   InitSpd;
		DecP(QUATERNION, Ret); Dec(QUATERNION, Out); Dec(VECTOR3, ax); DecFlt(an);
		TestRet(QuaternionRotationAxis, 3, (&Out, &ax, an));
	}
	{   InitSpd;
		DecP(QUATERNION, Ret); Dec(QUATERNION, Out); DecFlt(ya);DecFlt(pi);DecFlt(ro);
		TestRet(QuaternionRotationYawPitchRoll, 4, (&Out, ya,pi,ro));
	}
	{   InitSpd;
		DecP(QUATERNION, Ret); Dec(QUATERNION, Out); Dec(QUATERNION, q1); Dec(QUATERNION, q2); 
		TestRet(QuaternionMultiply, 3, (&Out, &q1, &q2));
	}
	{   InitSpd;
		DecP(QUATERNION, Ret); Dec(QUATERNION, Out); Dec(QUATERNION, q); 
		TestRet(QuaternionNormalize, 2, (&Out, &q));
	}
	{   InitSpd;
		DecP(QUATERNION, Ret); Dec(QUATERNION, Out); Dec(QUATERNION, q); 
		TestRet(QuaternionInverse, 2, (&Out, &q));
	}
	{   InitSpd;
		DecP(QUATERNION, Ret); Dec(QUATERNION, Out); Dec(QUATERNION, q); 
		TestRet(QuaternionLn, 2, (&Out, &q));
	}
	{   InitSpd;
		DecP(QUATERNION, Ret); Dec(QUATERNION, Out); Dec(QUATERNION, q); 
		TestRet(QuaternionExp, 2, (&Out, &q));
	}
	{   InitSpd;
		DecP(QUATERNION, Ret); Dec(QUATERNION, Out); Dec(QUATERNION, q1); Dec(QUATERNION, q2); DecFlt(t);
		TestRet(QuaternionSlerp, 4, (&Out, &q1, &q2, t));
	}
	{   InitSpd;
		DecP(QUATERNION, Ret); Dec(QUATERNION, Out); Dec(QUATERNION, q1); Dec(QUATERNION, q3); Dec(QUATERNION, q4); Dec(QUATERNION, q2); DecFlt(f); 
		TestRet(QuaternionSquad, 6, (&Out, &q1, &q2, &q3, &q4, f));
	}
	{   InitSpd;
		DecP(QUATERNION, Ret); Dec(QUATERNION, Out); Dec(QUATERNION, q1); Dec(QUATERNION, q3); Dec(QUATERNION, q2); DecFlt(f); DecFlt(g);
		TestRet(QuaternionBaryCentric, 6, (&Out, &q1, &q2, &q3, f,g));
	}

	xEndVariation(hlog);
}
ENDTEST();

BEGINTEST(testMathPlaneInl)
{
	xStartVariation(hlog, "Math Plane Inline");
	{   InitSpd;
		DecFlt(Ret); Dec(PLANE, p); Dec(VECTOR4, v);
		TestRet(PlaneDot, 2, (&p, &v));
	}
	{   InitSpd;
		DecFlt(Ret); Dec(PLANE, p); Dec(VECTOR3, v);
		TestRet(PlaneDotCoord, 2, (&p, &v));
	}
	{   InitSpd;
		DecFlt(Ret); Dec(PLANE, p); Dec(VECTOR3, v);
		TestRet(PlaneDotNormal, 2, (&p, &v));
	}
	xEndVariation(hlog);
}
ENDTEST();

BEGINTEST(testMathPlaneNonInl)
{
	xStartVariation(hlog, "Math Plane NonInline");
	{   InitSpd;
		DecP(PLANE, Ret); Dec(PLANE, Out); Dec(PLANE, p); 
		TestRet(PlaneNormalize, 2, (&Out, &p));
	}
	{   InitSpd;
		DecP(VECTOR3, Ret); Dec(VECTOR3, Out); Dec(PLANE, p);  Dec(VECTOR3, v1); Dec(VECTOR3, v2);
		TestRet(PlaneIntersectLine, 4, (&Out, &p,&v1,&v2));
	}
	{   InitSpd;
		DecP(PLANE, Ret); Dec(PLANE, Out);  Dec(VECTOR3, v1); Dec(VECTOR3, v2);
		TestRet(PlaneFromPointNormal, 3, (&Out, &v1,&v2));
	}
	{   InitSpd;
		DecP(PLANE, Ret); Dec(PLANE, Out); Dec(VECTOR3, v3);  Dec(VECTOR3, v1); Dec(VECTOR3, v2);
		TestRet(PlaneFromPoints, 4, (&Out, &v3,&v1,&v2));
	}
	{   InitSpd;
		DecP(PLANE, Ret); Dec(PLANE, Out); Dec(PLANE, p);  DecMat(m);
		TestRet(PlaneTransform, 3, (&Out, &p,&m));
	}
	xEndVariation(hlog);
}
ENDTEST();

BEGINTEST(testMathColor)
{
	xStartVariation(hlog, "Math Color Inline");
	{   InitSpd;
		DecP(COLOR, Ret); Dec(COLOR, Out); Dec(COLOR, c);
		TestRet(ColorNegative, 2, (&Out, &c));
	}
	{   InitSpd;
		DecP(COLOR, Ret); Dec(COLOR, Out); Dec(COLOR, c1);Dec(COLOR, c2);
		TestRet(ColorAdd, 3, (&Out, &c1, &c2));
	}
	{   InitSpd;
		DecP(COLOR, Ret); Dec(COLOR, Out); Dec(COLOR, c1);Dec(COLOR, c2);
		TestRet(ColorSubtract, 3, (&Out, &c1, &c2));
	}
	{   InitSpd;
		DecP(COLOR, Ret); Dec(COLOR, Out); Dec(COLOR, c);DecFlt(s);
		TestRet(ColorScale, 3, (&Out, &c, s));
	}
	{   InitSpd;
		DecP(COLOR, Ret); Dec(COLOR, Out); Dec(COLOR, c1);Dec(COLOR, c2);
		TestRet(ColorModulate, 3, (&Out, &c1, &c2));
	}
	{   InitSpd;
		DecP(COLOR, Ret); Dec(COLOR, Out); Dec(COLOR, c1);Dec(COLOR, c2);DecFlt(f)
		TestRet(ColorLerp, 4, (&Out, &c1, &c2,f));
	}
	{   InitSpd;
		DecP(COLOR, Ret); Dec(COLOR, Out); Dec(COLOR, c);DecFlt(f)
		TestRet(ColorAdjustSaturation, 3, (&Out, &c,f));
	}
	{   InitSpd;
		DecP(COLOR, Ret); Dec(COLOR, Out); Dec(COLOR, c);DecFlt(f)
		TestRet(ColorAdjustContrast, 3, (&Out, &c,f));
	}


	xEndVariation(hlog);
}
ENDTEST();

BEGINTEST(testMathCleanup)
{
	SetFpuAccuracy(gFpuAccuracy);
}
ENDTEST();
#pragma data_seg()

