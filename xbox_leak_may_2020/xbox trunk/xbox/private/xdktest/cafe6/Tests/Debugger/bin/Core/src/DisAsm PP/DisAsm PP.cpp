#include <windows.h>

const float FLOAT_ZERO = 0;
__int64 GlobalInt64 = 0;

__int64 FloatVal[] =	//	32 bit register values for each integer
{
	3225419779,		//	-3
	3221225474,		//	-2	
	3212836866,		//	-1
	0,				//	0
	1065353218,		//	1
	1073741826,		//	2
	1077936131,		//	3
	1082130434,		//	4
	1084227586,		//	5
	1086324738,		//	6
	1088421890,		//	7	
	1090519041,		//	8
	1091567617,		//	9
	1092616193		//	10
};

__m128 Var128;

const int FLOATVAL_BASE = -3;

void ClearXMMReg(int regid);
void set_GlobalInt64(int Hi, int Low);
void set_XMMRegister(int regid, char upper, int Hi, int Low);
void set_XMMRegister(int regid, int X3, int X2, int X1, int X0);

void set_Register(int regid, __int64 regval);
void Test_MMX();
void Test_3dNow();
void Test_3dNowEnhanced();
void Test_Katmai();
void Test_WNI();

int FAR PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//	MMX & 3d Now! tests
	Test_MMX();
	Test_3dNow();
	Test_3dNowEnhanced();
	Test_Katmai();
	Test_WNI();
	
	return 0;							
}


void set_Register(int regid, __int64 regval)
{
	switch (regid)
	{
		case 0:		__asm { MOVQ MM0, regval }
					break;


		case 1:		__asm { MOVQ MM1, regval }
					break;


		case 2:		__asm { MOVQ MM2, regval }
					break;

		case 3:		__asm { MOVQ MM3, regval }
					break;


		case 4:		__asm { MOVQ MM4, regval }
					break;

		case 5:		__asm { MOVQ MM5, regval }
					break;

		case 6:		__asm { MOVQ MM6, regval }
					break;

		case 7:		__asm { MOVQ MM7, regval }
					break;

	}
}


void Test_MMX()
{
	//	MMX tests
	
	__int64 count = 0;
	__int32 iVar32 = 0;

	//	test all registers for visibility
	__asm {	MOVQ MM0,MM1 }
	__asm {	MOVQ MM1,MM2 }
	__asm {	MOVQ MM2,MM3 }
	__asm {	MOVQ MM3,MM4 }
	__asm {	MOVQ MM4,MM5 }
	__asm {	MOVQ MM5,MM6 }
	__asm {	MOVQ MM6,MM7 }
	__asm {	MOVQ MM7,MM0 }

	//	shift left
	__asm {	PSLLW MM0,count }
	__asm {	PSLLW MM0,MM1 }

	//	mov 32 bit
	__asm {	MOVD MM0,iVar32 }
	__asm {	MOVD iVar32,MM0 }
	__asm {	MOVQ MM0,count }	
	__asm {	MOVQ count,MM0 }	

	__asm {	MOVD MM0,eax }
	__asm {	MOVD eax,MM0 }
	__asm {	MOVQ MM0,MM1 }	
	
	//	Pack with Signed Saturation
	__asm {	PACKSSWB MM0,count }
	__asm {	PACKSSDW MM0,count }

	__asm {	PACKSSWB MM0,MM1 }
	__asm {	PACKSSDW MM0,MM1 }

	//	Pack with Unsigned Saturation	
	__asm {	PACKUSWB MM0,count }
	__asm {	PACKUSWB MM0,MM1 }

	//	Packed Add
	__asm {	PADDB MM0,count }
	__asm {	PADDW MM0,count }	
	__asm {	PADDD MM0,count }

	__asm {	PADDB MM0,MM1 }
	__asm {	PADDW MM0,MM1 }	
	__asm {	PADDD MM0,MM1 }

	//	Packed Add with Saturation
	__asm {	PADDSB MM0,count }
	__asm {	PADDSW MM0,count }

	__asm {	PADDSB MM0,MM1 }
	__asm {	PADDSW MM0,MM1 }

	//	Packed Add Unsigned with Saturation 
	__asm {	PADDUSB MM0,count }
	__asm {	PADDUSW MM0,count }

	__asm {	PADDUSB MM0,MM1 }
	__asm {	PADDUSW MM0,MM1 }

	//	Bitwise Logical And 
	__asm {	PAND MM0,count }
	__asm {	PAND MM0,MM1 }

	//	Bitwise Logical And Not 
	__asm {	PANDN MM0,count }
	__asm {	PANDN MM0,MM1 }

	//	Bitwise Logical Or
	__asm {	POR MM0,count }
	__asm {	POR MM0,MM1 }

	//	Compare for Equal 
	__asm {	PCMPEQB MM0,count }
	__asm {	PCMPEQW MM0,count }
	__asm {	PCMPEQD MM0,count }

	__asm {	PCMPEQB MM0,MM1 }
	__asm {	PCMPEQW MM0,MM1 }
	__asm {	PCMPEQD MM0,MM1 }

	//	Compare for Greater Than
	__asm {	PCMPGTB MM0,count }
	__asm {	PCMPGTW MM0,count }
	__asm {	PCMPGTD MM0,count }

	__asm {	PCMPGTB MM0,MM1 }
	__asm {	PCMPGTW MM0,MM1 }
	__asm {	PCMPGTD MM0,MM1 }

	//	Packed Multiply and Add 
	__asm {	PMADDWD MM0,count }
	__asm {	PMADDWD MM0,MM1 }

	//	Packed Multiply High
	__asm {	PMULHW MM0,count }
	__asm {	PMULHW MM0,MM1 }

	//	Packed Multiply Low
	__asm {	PMULLW MM0,count }
	__asm {	PMULLW MM0,MM1 }

	//	Packed Shift Left Logical
	__asm {	PSLLW MM0,count }
	__asm {	PSLLD MM0,count }
	__asm {	PSLLQ MM0,count }

	__asm {	PSLLW MM0,MM1 }
	__asm {	PSLLD MM0,MM1 }
	__asm {	PSLLQ MM0,MM1 }

	__asm {	PSLLW MM0,7 }
	__asm {	PSLLD MM0,7 }
	__asm {	PSLLQ MM0,7 }

	//	Packed Shift Right Arithmetic
	__asm {	PSRAW MM0,count }
	__asm {	PSRAD MM0,count }
	
	__asm {	PSRAW MM0,MM1 }
	__asm {	PSRAD MM0,MM1 }

	__asm {	PSRAW MM0,5 }
	__asm {	PSRAD MM0,5 }

	//	Packed Shift Right Logical
	__asm {	PSRLW MM0,count }
	__asm {	PSRLD MM0,count }
	__asm {	PSRLQ MM0,count }

	__asm {	PSRLW MM0,MM1 }
	__asm {	PSRLD MM0,MM1 }
	__asm {	PSRLQ MM0,MM1 }

	__asm {	PSRLW MM0,6 }
	__asm {	PSRLD MM0,6 }
	__asm {	PSRLQ MM0,6 }

	//	Packed Subtract
	__asm {	PSUBB MM0,count }
	__asm {	PSUBW MM0,count }
	__asm {	PSUBD MM0,count }

	__asm {	PSUBB MM0,MM1 }
	__asm {	PSUBW MM0,MM1 }
	__asm {	PSUBD MM0,MM1 }

	//	Packed Subtract with Saturation
	__asm {	PSUBSB MM0,count }
	__asm {	PSUBSW MM0,count }

	__asm {	PSUBSB MM0,MM1 }
	__asm {	PSUBSW MM0,MM1 }

	//	Packed Subtract Unsigned with Saturation
	__asm {	PSUBUSB MM0,count }
	__asm {	PSUBSW MM0,count }
	
	__asm {	PSUBUSB MM0,MM1 }
	__asm {	PSUBSW MM0,MM1 }
	
	//	Unpack High Packed Data
	__asm {	PUNPCKHBW MM0,count }	
	__asm {	PUNPCKHWD MM0,count }
	__asm {	PUNPCKHDQ MM0,count }
	
	__asm {	PUNPCKHBW MM0,MM1 }	
	__asm {	PUNPCKHWD MM0,MM1 }
	__asm {	PUNPCKHDQ MM0,MM1 }
	
	//	Unpack Low Packed Data
	__asm {	PUNPCKLBW MM0,iVar32 }
	__asm {	PUNPCKLWD MM0,iVar32 }
	__asm {	PUNPCKLDQ MM0,iVar32 }
	
	__asm {	PUNPCKLBW MM0,MM1 }
	__asm {	PUNPCKLWD MM0,MM1 }
	__asm {	PUNPCKLDQ MM0,MM1 }
	
	//	Bitwise Logical Exclusive OR
	__asm {	PXOR MM0,count }
	__asm {	PXOR MM0,MM1 }

}


void Test_3dNow()
{
	//	3d Now Tests

	__int64 count = 0;
	unsigned char int8 = 0;

	//	average of unsigned int bytes
	__asm {	PAVGUSB MM0,count }
	__asm {	PAVGUSB MM0,MM5 }

	//	float to 32 bit signed int
	__asm {	PF2ID MM0,count }
	__asm {	PF2ID MM0,MM5 }

	//	accumulator
	__asm {	PFACC MM0,count }
	__asm {	PFACC MM0,MM5 }

	//	add
	__asm {	PFADD MM0,count }
	__asm {	PFADD MM0,MM5 }

	//	compare for equal
	__asm {	PFCMPEQ MM0,count }
	__asm {	PFCMPEQ MM0,MM5 }

	//	compare for greater than or equal to
	__asm {	PFCMPGE MM0,count }
	__asm {	PFCMPGE MM0,MM5 }

	//	compare for greater than
	__asm {	PFCMPGT MM0,count }
	__asm {	PFCMPGT MM0,MM5 }
	
	//	maximum
	__asm {	PFMAX MM0,count }
	__asm {	PFMAX MM0,MM5 }

	//	minimum
	__asm {	PFMIN MM0,count }
	__asm {	PFMIN MM0,MM5 }

	//	multiply
	__asm {	PFMUL MM0,count }
	__asm {	PFMUL MM0,MM5 }
	
	//	reciprocal
	__asm {	PFRCP MM0,count }
	__asm {	PFRCP MM0,MM5 }

	//	reciprocal, reciprocal intermediate step 1, reciprocal intermediate step 2
	__asm {	PFRCPIT1 MM1,MM0 }
	__asm {	PFRCPIT2 MM0,MM1 }

	__asm {	PFRCPIT1 MM5,count }
	__asm {	PFRCPIT2 MM5,count }

	//	reciprocal sqrt
	__asm {	PFRSQRT MM0,count }
	__asm {	PFRSQRT MM0,MM5 }

	//	reciprocal sqrt, reciprocal sqrt intermediate step 1	
	__asm {	PFRSQIT1 MM1,MM0 }
	__asm {	PFRSQIT1 MM1,count }

	//	subtract
	__asm {	PFSUB MM0,count }
	__asm {	PFSUB MM0,MM5 }

	//	reverse subtraction
	__asm {	PFSUBR MM0,count }
	__asm {	PFSUBR MM0,MM5 }

	//	32 bit signed int to float
	__asm {	PI2FD MM0,count }
	__asm {	PI2FD MM0,MM5 }

	//	multiply high
	__asm {	PMULHRW MM0,count }
	__asm {	PMULHRW MM0,MM5 }

	int8 = 1;

	//	load a processor cache into data cache
	__asm {	PREFETCH int8 }

	//	same, sets MES state to modified
	__asm {	PREFETCHW int8 }

}


void Test_3dNowEnhanced()
{
	//	enhanced 3dnow tests

	__int64 count = 0;
	unsigned char count8 = 0;
	__int16 iVar16 = 0;

	//	packed float to int word with sign extend
	__asm {	PF2IW MM0,count }
	__asm {	PF2IW MM0,MM1 }

	//	packed float negative accumulate
	__asm {	PFNACC MM0,count }
	__asm {	PFNACC MM0,MM1 }

	//	packed float mixed +ve -ve accumulate
	__asm {	PFPNACC MM0,count }
 	__asm {	PFPNACC MM0,MM1 }
    
	//	packed int word to float
	//	somewhat erroneous values
	__asm {	PI2FW MM0,count }
	__asm {	PI2FW MM0,MM1 }

	//	packed swap double word
	__asm {	PSWAPD MM0,count }
	__asm {	PSWAPD MM0,MM1 }
	
	//	streaming store using byte mask
	//	DID NOT UNDERSTAND
	__asm {	MASKMOVQ MM0,MM1 }
	
	//	streaming store
	__asm {	MOVNTQ count,MM0 }
			
	//	packed avg of unsigned byte	
	__asm {	PAVGB MM0,count }
	__asm {	PAVGB MM0,MM1 }
	
	//	packed avg of unsigned word	
	__asm {	PAVGW MM0,count }
	__asm {	PAVGW MM0,MM1 }

	//	extract word into int register
	__asm {	PEXTRW EAX,MM0, 1 }

	//	insert word from int register
	__asm {	PINSRW MM0, EAX, 1 }
	__asm {	PINSRW MM0, iVar16, 1 }

	//	packed max signed word
	__asm {	PMAXSW MM0,count }	//	1431633926
	__asm {	PMAXSW MM0,MM1 }	//	1431633926
	
	//	packed max unsigned byte
	__asm {	PMAXUB MM0,count }	//	1834219928319
	__asm {	PMAXUB MM0,MM1 }	//	1834219928319
	
	//	packed min signed word
	__asm {	PMINSW MM0,count }	//	1431633921
	__asm {	PMINSW MM0,MM1 }	//	1431633921
	
	//	packed min unsigned byte	
	__asm {	PMINUB MM0,count }	//	730396487935
	__asm {	PMINUB MM0,MM1 }	//	730396487935
	
	//	move mask to integer register
	__asm {	PMOVMSKB EAX, MM0 }	//	EAX = 217
	
	//	packed multiply high unsigned word
	__asm {	PMULHUW MM0,count }	//	281474976841729
	__asm {	PMULHUW MM0,MM1 }	//	281474976841729

	//	prefetch non-temporal access
	__asm {	PREFETCHNTA count8 }
	
	//	prefetch to all cache levels
	__asm {	PREFETCHT0 count8 }
	
	//	prefetch to all cache levels except 0
	__asm {	PREFETCHT1 count8 }
	
	//	prefetch to all cache levels except 0 & 1
	__asm {	PREFETCHT2 count8 }

	//	packed sum of absolute byte differences
	__asm {	PSADBW MM0,count }	//	35
	__asm {	PSADBW MM0,MM1 }	//	35
	
	//	packed shuffle word
	__asm {	PSHUFW MM0,count,228 }	//	578437695752307201
	__asm {	PSHUFW MM0,MM1,225 }	//	578437695752307201
	
	// store fence
	__asm {	SFENCE }

}


void Test_Katmai()
{
	//	Katmai (PIII) tests
	
	float fVal = 0.0;
	__m128 mVar128 = {0,0,0,0};
	__int32 i32Val = 0;

	//	test all registers for visibility
	__asm { movss   xmm0, xmm1 }	//	0.0.0.9
	__asm { movss   xmm1, xmm2 }	//	0.0.0.9
	__asm { movss   xmm2, xmm3 }	//	0.0.0.9
	__asm { movss   xmm3, xmm4 }	//	0.0.0.9
	__asm { movss   xmm4, xmm5 }	//	0.0.0.9
	__asm { movss   xmm5, xmm6 }	//	0.0.0.9
	__asm { movss   xmm6, xmm7 }	//	0.0.0.9
	__asm { movss   xmm7, xmm0 }	//	0.0.0.9

	__asm { addps   xmm0, xmm1 }	//	XMM0 - 3,5,7,9
	__asm { addps   xmm0, mVar128 }	//	XMM0 - 3,5,7,9

	__asm { addss   xmm0, xmm1 }	//	1,2,3,9
	__asm { addss   xmm0, i32Val }	//	1,2,3,9

	__asm { andnps   xmm0, xmm1 }
	__asm { andnps   xmm0, mVar128 }

	__asm { andps   xmm0, xmm1 }	//	0
	__asm { andps   xmm0, mVar128 }	//	0

	__asm { cmpps   xmm0, xmm1, 0 }		//	eq
	__asm { cmpps   xmm0, xmm1, 1 }		//	lt
	__asm { cmpps   xmm0, xmm1, 2 }		//	le
	__asm { cmpps   xmm0, xmm1, 3 }		//	unord
	__asm { cmpps   xmm0, xmm1, 4 }		//	neq
	__asm { cmpps   xmm0, xmm1, 5 }		//	nlt
	__asm { cmpps   xmm0, xmm1, 6 }		//	nle
	__asm { cmpps   xmm0, xmm1, 7 }		//	ord
	
	__asm { cmpps   xmm0, mVar128, 0 }
	__asm { cmpps   xmm0, mVar128, 1 }
	__asm { cmpps   xmm0, mVar128, 2 }
	__asm { cmpps   xmm0, mVar128, 3 }
	__asm { cmpps   xmm0, mVar128, 4 }
	__asm { cmpps   xmm0, mVar128, 5 }
	__asm { cmpps   xmm0, mVar128, 6 }
	__asm { cmpps   xmm0, mVar128, 7 }

	__asm { cmpss   xmm0, xmm1, 0 }		//	eq
	__asm { cmpss   xmm0, xmm1, 1 }		//	lt
	__asm { cmpss   xmm0, xmm1, 2 }		//	le
	__asm { cmpss   xmm0, xmm1, 3 }		//	unord
	__asm { cmpss   xmm0, xmm1, 4 }		//	neq
	__asm { cmpss   xmm0, xmm1, 5 }		//	nlt
	__asm { cmpss   xmm0, xmm1, 6 }		//	nle
	__asm { cmpss   xmm0, xmm1, 7 }		//	ord
	
	__asm { cmpss   xmm0, i32Val, 0 }
	__asm { cmpss   xmm0, i32Val, 1 }
	__asm { cmpss   xmm0, i32Val, 2 }
	__asm { cmpss   xmm0, i32Val, 3 }
	__asm { cmpss   xmm0, i32Val, 4 }
	__asm { cmpss   xmm0, i32Val, 5 }
	__asm { cmpss   xmm0, i32Val, 6 }
	__asm { cmpss   xmm0, i32Val, 7 }

	__asm { cmpeqps   xmm0, xmm1 }	//	eq-eq-eq-eq
	__asm { cmpeqss   xmm0, xmm1 }	//	1,2,3,eq
	__asm { cmpltps   xmm0, xmm1 }	//	no-yes-no-no
	__asm { cmpltss   xmm0, xmm1 }	//	1,2,3,yes
	__asm { cmpleps   xmm0, xmm1 }	//	yes,yes,no,no
	__asm { cmpless   xmm0, xmm1 }	//	1,2,3,yes
	__asm { cmpneqps  xmm0, xmm1 }	//	no,y,y,n
	__asm { cmpneqss   xmm0, xmm1 }	//	1,2,3,n
	__asm { cmpnltps   xmm0, xmm1 }	//	n,y,n,y
	__asm { cmpnltss   xmm0, xmm1 }	//	1,2,3,y
	__asm { cmpnleps   xmm0, xmm1 }	//	n,n,n,y
	__asm { cmpnless   xmm0, xmm1 }	//	1,2,3,n

	__asm { comiss   xmm0, xmm1 }	//	1
	__asm { comiss   xmm0, i32Val }	//	1

	__asm { cvtpi2ps   xmm0, GlobalInt64 }	//	1,2,3,4
	__asm { cvtpi2ps   xmm0, mm0 }	//	1,2,3,4

	__asm { cvtps2pi   mm0, xmm0 }	//	MM0 has 2,5
	__asm { cvtps2pi   mm0, GlobalInt64 }	//	MM0 has 2,5

	__asm { cvtsi2ss   xmm0, eax }	//	1,2,3,17
	__asm { cvtsi2ss   xmm0, i32Val }	//	1,2,3,17

	__asm { cvtss2si   eax, xmm0 }	//	eax has 4
	__asm { cvtss2si   eax, i32Val }	//	eax has 4

	__asm { cvttps2pi   mm0, xmm0 }	//	MM0 has 7,5
	__asm { cvttps2pi   mm0, GlobalInt64 }	//	MM0 has 7,5

	__asm { cvttss2si   eax, xmm0 }	//	eax has 7
	__asm { cvttss2si   eax, i32Val }	//	eax has 7

	__asm { divps   xmm0, xmm1 }	//	0.5,1,3,4
	__asm { divps   xmm0, mVar128 }	//	0.5,1,3,4

	__asm { divss   xmm0, xmm1 }	//	1,2,3,5
	__asm { divss   xmm0, i32Val }	//	1,2,3,5

	__asm { maxps   xmm0, xmm1 }	//	5,2,9,5
	__asm { maxps   xmm0, mVar128 }	//	5,2,9,5

	__asm { maxss   xmm0, xmm1 }	//	5,2,7,6
	__asm { maxss   xmm0, i32Val }	//	5,2,7,6

	__asm { minps   xmm0, xmm1 }	//	4,2,7,3
	__asm { minps   xmm0, mVar128 }	//	4,2,7,3

	__asm { minss   xmm0, xmm1 }	//	1.2.3.4
	__asm { minss   xmm0, i32Val }	//	1.2.3.4

	__asm { movaps   xmm0, xmm1 }	//	5.5.5.5
	__asm { movaps   xmm0, mVar128 }	//	5.5.5.5
	__asm { movaps   mVar128, xmm1 }	//	5.5.5.5

	__asm { movhlps	 xmm0, xmm1 }	//	1,2,5,6

	__asm { movhps   xmm0, GlobalInt64 }	//	6.7.3.4
	__asm { movhps   GlobalInt64, xmm0 }	//	6.7.3.4

	__asm { movlps   xmm0, GlobalInt64 }	//	1.2.6.7
	__asm { movlps   GlobalInt64, xmm0 }	//	1.2.6.7

	__asm { movlhps   xmm0, xmm1 }	//	5.6.3.4

	__asm { movmskps   eax, xmm0 }	//	5

	__asm { movntps   mVar128, xmm0 }

	__asm { movss   xmm0, i32Val }	//	0.0.0.9
	__asm { movss   i32Val, xmm0 }	//	0.0.0.9
	__asm { movss   xmm0, xmm1 }	//	0.0.0.9

	__asm { movups   xmm0, xmm1 }	//	5,6,7,8
	__asm { movups   xmm0, mVar128 }	//	5,6,7,8
	__asm { movups   mVar128, xmm1 }	//	5,6,7,8

	__asm { mulps   xmm0, xmm1 }	//	2,6,0,10
	__asm { mulps   xmm0, mVar128 }	//	2,6,0,10

	__asm { mulss   xmm0, xmm1 }	//	1,2,3,8
	__asm { mulss   xmm0, i32Val }	//	1,2,3,8

	__asm { orps   xmm0, xmm1 }	//	1,x,y,z
	__asm { orps   xmm0, mVar128 }	//	1,x,y,z

	__asm { rcpps   xmm0, xmm1 }	//	1/2, 1/3, 1/4, 1/5
	__asm { rcpps   xmm0, mVar128 }	//	1/2, 1/3, 1/4, 1/5

	__asm { rcpss   xmm0, xmm1 }	//	1,2,3,1/2
	__asm { rcpss   xmm0, i32Val }	//	1,2,3,1/2

	__asm { rsqrtps   xmm0, xmm1 }	//	reciprocal 1,1.73,2,3
	__asm { rsqrtps   xmm0, mVar128 }	//	reciprocal 1,1.73,2,3

	__asm { rsqrtss   xmm0, xmm1 }	//	1,2,3, 1/3
	__asm { rsqrtss   xmm0, i32Val }	//	1,2,3, 1/3

	__asm { sfence }

	__asm { shufps   xmm0, xmm1, 114 }
	__asm { shufps   xmm0, mVar128, 115 }

	__asm { sqrtps   xmm0, xmm1 }	//	1,1.73,2,3
	__asm { sqrtps   xmm0, mVar128 }	//	1,1.73,2,3

	__asm { sqrtss   xmm0, xmm1 }	//	1,2,3,3
	__asm { sqrtss   xmm0, i32Val }	//	1,2,3,3

	__asm { stmxcsr  i32Val }
	__asm { ldmxcsr  i32Val }	//	1,2,3,5

	__asm { subps   xmm0, xmm1 }	//	-1,5,1,3
	__asm { subps   xmm0, mVar128 }	//	-1,5,1,3

	__asm { subss   xmm0, xmm1 }	//	1,8,5,-2
	__asm { subss   xmm0, i32Val }	//	1,8,5,-2

	__asm { ucomiss   xmm0, xmm1 }	//	0
	__asm { ucomiss   xmm0, i32Val }	//	0

	__asm { unpckhps   xmm0, xmm1 }	//	interleaved high - 5,1,6,2
	__asm { unpckhps   xmm0, mVar128 }	//	interleaved high - 5,1,6,2

	__asm { unpcklps   xmm0, xmm1 }	//	interleaved low - 7,3,8,4
	__asm { unpcklps   xmm0, mVar128 }	//	interleaved low - 7,3,8,4

	__asm { xorps   xmm0, xmm1 }	//	xor - 0,0,x,y
	__asm { xorps   xmm0, mVar128 }	//	xor - 0,0,x,y

}

void ClearXMMReg(int regid)
{

	switch(regid)
	{

	case 0:
		__asm { MOVSS XMM0, FLOAT_ZERO }
		break;

	case 1:
		__asm { MOVSS XMM1, FLOAT_ZERO }
		break;

	case 2:
		__asm { MOVSS XMM2, FLOAT_ZERO }
		break;

	case 3:
		__asm { MOVSS XMM3, FLOAT_ZERO }
		break;

	case 4:
		__asm { MOVSS XMM4, FLOAT_ZERO }
		break;

	case 5:
		__asm { MOVSS XMM5, FLOAT_ZERO }
		break;

	case 6:
		__asm { MOVSS XMM6, FLOAT_ZERO }
		break;

	case 7:
		__asm { MOVSS XMM7, FLOAT_ZERO }
		break;

	}

}

void set_GlobalInt64(int Hi, int Low)
{
	__int64 HiVal = FloatVal[Hi - FLOATVAL_BASE];
	__int64 LowVal = FloatVal[Low - FLOATVAL_BASE];

	GlobalInt64 = HiVal;
	GlobalInt64  = GlobalInt64 << 32;
	GlobalInt64 |= LowVal;
}

void set_XMMRegister(int regid, char upper, int Hi, int Low)
{

	switch (regid)
	{
		case 0:		
					set_GlobalInt64(Hi, Low);
					if (upper)
					{
						__asm { MOVHPS XMM0, GlobalInt64 }
					}
					else
					{
						__asm { MOVLPS XMM0, GlobalInt64 }
					}
					break;

		case 1:					
					set_GlobalInt64(Hi, Low);
					if (upper)
					{
						__asm { MOVHPS XMM1, GlobalInt64 }
					}
					else
					{
						__asm { MOVLPS XMM1, GlobalInt64 }
					}
					break;


		case 2:					
					set_GlobalInt64(Hi, Low);
					if (upper)
					{
						__asm { MOVHPS XMM2, GlobalInt64 }
					}
					else
					{
						__asm { MOVLPS XMM2, GlobalInt64 }
					}
					break;

		case 3:		
					set_GlobalInt64(Hi, Low);
					if (upper)
					{
						__asm { MOVHPS XMM3, GlobalInt64 }
					}
					else
					{
						__asm { MOVLPS XMM3, GlobalInt64 }
					}
					break;


		case 4:		
					set_GlobalInt64(Hi, Low);
					if (upper)
					{
						__asm { MOVHPS XMM4, GlobalInt64 }
					}
					else
					{
						__asm { MOVLPS XMM4, GlobalInt64 }
					}
					break;

		case 5:		
					set_GlobalInt64(Hi, Low);
					if (upper)
					{
						__asm { MOVHPS XMM5, GlobalInt64 }
					}
					else
					{
						__asm { MOVLPS XMM5, GlobalInt64 }
					}
					break;

		case 6:		
					set_GlobalInt64(Hi, Low);
					if (upper)
					{
						__asm { MOVHPS XMM6, GlobalInt64 }
					}
					else
					{
						__asm { MOVLPS XMM6, GlobalInt64 }
					}
					break;

		case 7:		
					set_GlobalInt64(Hi, Low);
					if (upper)
					{
						__asm { MOVHPS XMM7, GlobalInt64 }
					}
					else
					{
						__asm { MOVLPS XMM7, GlobalInt64 }
					}
					break;
	}
}


void set_XMMRegister(int regid, int X3, int X2, int X1, int X0)
{
	set_XMMRegister(regid, 1, X3, X2);	//	upper 64 bits
	set_XMMRegister(regid, 0, X1, X0);	//	lower 64 bits
}

void Test_WNI()
{
	__m128 mVar128 = {0,0,0,0};
	__m64 mVar64 = {0};

	__int8 mVar8;
	__int16 mVar16;
	__int32 mVar32;
	char arr[32];

	__asm
	{
		EMMS		
		
		ADDPD XMM0,XMM1
		ADDPD XMM2,XMM3
		ADDPD XMM4,XMM5
		ADDPD XMM6,XMM7
		ADDPD XMM1,XMM0
		ADDPD XMM3,XMM2
		ADDPD XMM5,XMM4
		ADDPD XMM7,XMM6
			
		ADDPD XMM0,XMM1
		ADDPD XMM0,mVar128
		
		ADDSD XMM0,XMM1
		ADDSD XMM0,mVar64

		ANDNPD XMM0,XMM1
		ANDNPD XMM0,mVar128
		
		ANDPD XMM0,XMM1
		ANDPD XMM0,mVar128
		
		CMPPD XMM0,XMM1, 0
		CMPPD XMM0,mVar128, 0
		CMPPD XMM0,XMM1, 1
		CMPPD XMM0,mVar128, 1
		CMPPD XMM0,XMM1, 2
		CMPPD XMM0,mVar128, 2
		CMPPD XMM0,XMM1, 3
		CMPPD XMM0,mVar128, 3
		CMPPD XMM0,XMM1, 4
		CMPPD XMM0,mVar128, 4
		CMPPD XMM0,XMM1, 5
		CMPPD XMM0,mVar128, 5
		CMPPD XMM0,XMM1, 6
		CMPPD XMM0,mVar128, 6
		CMPPD XMM0,XMM1, 7
		CMPPD XMM0,mVar128, 7
		
		CMPSD XMM0,XMM1, 0		
		CMPSD XMM0,mVar64, 0
		CMPSD XMM0,XMM1, 1		
		CMPSD XMM0,mVar64, 1
		CMPSD XMM0,XMM1, 2		
		CMPSD XMM0,mVar64, 2
		CMPSD XMM0,XMM1, 3		
		CMPSD XMM0,mVar64, 3
		CMPSD XMM0,XMM1, 4		
		CMPSD XMM0,mVar64, 4
		CMPSD XMM0,XMM1, 5		
		CMPSD XMM0,mVar64, 5
		CMPSD XMM0,XMM1, 6		
		CMPSD XMM0,mVar64, 6
		CMPSD XMM0,XMM1, 7		
		CMPSD XMM0,mVar64, 7
		
		COMISD XMM0,XMM1
		COMISD XMM0,mVar64
		
		CVTDQ2PD XMM0,XMM1
		CVTDQ2PD XMM0,mVar64

		CVTPD2PI MM0,XMM1	
		CVTPD2PI MM0,mVar128	
		
		CVTPD2DQ XMM0,XMM1	
		CVTPD2DQ XMM0,mVar128	

		
		CVTPD2PS XMM0,XMM1
		CVTPD2PS XMM0,mVar128
		
		CVTPI2PD XMM0,MM0
		CVTPI2PD XMM0,mVar64
		
		CVTPS2PD XMM0,XMM1
		CVTPS2PD XMM0,mVar64
		
		CVTSD2SI eax,XMM1
		CVTSD2SI eax,mVar64
		
		CVTSD2SS XMM0,XMM1
		CVTSD2SS XMM0,mVar64
		
		CVTSI2SD XMM0,eax
		CVTSI2SD XMM0,mVar32
		
		CVTSS2SD XMM0,XMM1
		CVTSS2SD XMM0,mVar32
		
		CVTTPD2PI MM0,XMM1
		CVTTPD2PI MM0,mVar128
		
		CVTTPD2DQ XMM0,XMM1
		CVTTPD2DQ XMM0,mVar128
		
		CVTTSD2SI eax,XMM1
		CVTTSD2SI eax,mVar64
		
		DIVPD XMM0,XMM1
		DIVPD XMM0,mVar128
		
		DIVSD XMM0,XMM1
		DIVSD XMM0,mVar64
		
		MAXPD XMM0,XMM1
		MAXPD XMM0,mVar128
		
		MAXSD XMM0,XMM1
		MAXSD XMM0,mVar64
		
		MINPD XMM0,XMM1
		MINPD XMM0,mVar128
		
		MINSD XMM0,XMM1
		MINSD XMM0,mVar64
		
		MOVAPD XMM0,XMM1
		MOVAPD XMM0,mVar128
		MOVAPD mVar128,XMM1
		
		MOVHPD XMM0,mVar64
		MOVHPD mVar64,XMM0
		
		MOVLPD XMM0,mVar64
		MOVLPD mVar64,XMM0
		
		MOVMSKPD eax,XMM1
		
		MOVSD XMM0,XMM1
		MOVSD XMM0,mVar64
		MOVSD mVar64,XMM1
		
		MOVUPD XMM0,XMM1
		MOVUPD XMM0,mVar128
		MOVUPD mVar128,XMM1
		
		MULPD XMM0,XMM1
		MULPD XMM0,mVar128
		
		MULSD XMM0,XMM1
		MULSD XMM0,mVar64
		
		ORPD XMM0,XMM1
		ORPD XMM0,mVar128
		
		SHUFPD XMM0,XMM1,8
		SHUFPD XMM0,mVar128,8
		
		SQRTPD XMM0,XMM1
		SQRTPD XMM0,mVar128
		
		SQRTSD XMM0,XMM1
		SQRTSD XMM0,mVar64
		
		SUBPD XMM0,XMM1
		SUBPD XMM0,mVar128
		
		SUBSD XMM0,XMM1
		SUBSD XMM0,mVar64
		
		UCOMISD XMM0,XMM1
		UCOMISD XMM0,mVar64
		
		UNPCKHPD XMM0,XMM1
		UNPCKHPD XMM0,mVar128
		
		UNPCKLPD XMM0,XMM1
		UNPCKLPD XMM0,mVar128
		
		XORPD XMM0,XMM1
		XORPD XMM0,mVar128

		
		CVTDQ2PS XMM0,XMM1
		CVTDQ2PS XMM0,mVar128

		CVTPS2DQ XMM0,XMM1
		CVTPS2DQ XMM0,mVar128

		CVTTPS2DQ XMM0,XMM1
		CVTTPS2DQ XMM0,mVar128

		//	not in manual
		CVTPI2PS XMM0,MM1
		CVTPI2PS XMM0,MM1

		//	not in manual
		CVTPS2PI MM0,XMM1
		CVTPS2PI MM0,XMM1
		
		//	not in manual
		CVTTPS2PI MM0,XMM1
		CVTTPS2PI MM0,XMM1
		
		MOVD XMM0,eax
		MOVD XMM0,mVar32
		MOVD eax,XMM1
		MOVD mVar32,XMM1
		
		MOVDQA XMM0,XMM1
		MOVDQA XMM0,mVar128
		MOVDQA mVar128,XMM1
		
		MOVDQU XMM0,XMM1
		MOVDQU XMM0,mVar128
		MOVDQU mVar128,XMM1
		
		MOVDQ2Q MM0,XMM1

		MOVQ2DQ XMM0,MM1
		
		MOVQ XMM0,XMM1
		MOVQ XMM0,mVar64
		MOVQ mVar64,XMM1

		PACKSSWB XMM0,XMM1
		PACKSSWB XMM0,mVar128
		
		PACKSSDW XMM0,XMM1
		PACKSSDW XMM0,mVar128
		
		PACKUSWB XMM0,XMM1
		PACKUSWB XMM0,mVar128
		
		PADDB XMM0,XMM1
		PADDB XMM0,mVar128
		
		PADDW XMM0,XMM1
		PADDW XMM0,mVar128
		
		PADDD XMM0,XMM1
		PADDD XMM0,mVar128
		
		PADDQ MM0,MM1
		PADDQ MM0,mVar64
		PADDQ XMM0,XMM1
		PADDQ XMM0,mVar128

		PADDSB XMM0,XMM1
		PADDSB XMM0,mVar128
		
		PADDSW XMM0,XMM1
		PADDSW XMM0,mVar128
		
		PADDUSB XMM0,XMM1
		PADDUSB XMM0,mVar128
		
		PADDUSW XMM0,XMM1
		PADDUSW XMM0,mVar128
		
		PAND XMM0,XMM1
		PAND XMM0,mVar128
		
		PANDN XMM0,XMM1
		PANDN XMM0,mVar128
		
		PAVGB XMM0,XMM1
		PAVGB XMM0,mVar128
		
		PAVGW XMM0,XMM1
		PAVGW XMM0,mVar128
		
		PCMPEQB XMM0,XMM1
		PCMPEQB XMM0,mVar128
		
		PCMPEQW XMM0,XMM1
		PCMPEQW XMM0,mVar128
		
		PCMPEQD XMM0,XMM1
		PCMPEQD XMM0,mVar128
		
		PCMPGTB XMM0,XMM1
		PCMPGTB XMM0,mVar128
		
		PCMPGTW XMM0,XMM1
		PCMPGTW XMM0,mVar128
		
		PCMPGTD XMM0,XMM1
		PCMPGTD XMM0,mVar128

		PEXTRW eax,XMM1,6

		PINSRW XMM0,mVar16,6
		PINSRW XMM0,eax,6
		
		PMADDWD XMM0,XMM1
		PMADDWD XMM0,mVar128

		PMAXSW XMM0,XMM1
		PMAXSW XMM0,mVar128

		PMAXUB XMM0,XMM1
		PMAXUB XMM0,mVar128
		
		PMINSW XMM0,XMM1
		PMINSW XMM0,mVar128
		
		PMINUB XMM0,XMM1
		PMINUB XMM0,mVar128
		
		PMOVMSKB eax,XMM1
		
		PMULHW XMM0,XMM1
		PMULHW XMM0,mVar128
		
		PMULHUW XMM0,XMM1
		PMULHUW XMM0,mVar128
		
		PMULLW XMM0,XMM1
		PMULLW XMM0,mVar128
		
		PMULUDQ MM0,MM1
		PMULUDQ MM0,mVar64
		PMULUDQ XMM0,XMM1
		PMULUDQ XMM0,mVar128
		
		POR XMM0,XMM1
		POR XMM0,mVar128
		
		PSADBW XMM0,XMM1
		PSADBW XMM0,mVar128
		
		PSHUFD XMM0,XMM1,8
		PSHUFD XMM0,mVar128,8
		
		PSHUFHW XMM0,XMM1,8
		PSHUFHW XMM0,mVar128,8
		
		PSHUFLW XMM0,XMM1,8
		PSHUFLW XMM0,mVar128,8
		
		PSLLDQ XMM0,8
		
		PSLLW XMM0,XMM1
		PSLLW XMM0,mVar128
		PSLLW XMM0,8
		
		PSLLD XMM0,XMM1
		PSLLD XMM0,mVar128
		PSLLD XMM0,8
		
		PSLLQ XMM0,XMM1
		PSLLQ XMM0,mVar128
		PSLLQ XMM0,8
		
		PSRAW XMM0,XMM1
		PSRAW XMM0,mVar128
		PSRAW XMM0,8
		
		PSRAD XMM0,XMM1
		PSRAD XMM0,mVar128
		PSRAD XMM0,8
		
		PSRLDQ XMM0,8
		
		PSRLW XMM0,XMM1
		PSRLW XMM0,mVar128
		PSRLW XMM0,8
		
		PSRLD XMM0,XMM1
		PSRLD XMM0,mVar128
		PSRLD XMM0,8
		
		PSRLQ XMM0,XMM1
		PSRLQ XMM0,mVar128
		PSRLQ XMM0,8
		
		PSUBB XMM0,XMM1
		PSUBB XMM0,mVar128
		
		PSUBW XMM0,XMM1
		PSUBW XMM0,mVar128
		
		PSUBD XMM0,XMM1
		PSUBD XMM0,mVar128

		PSUBQ MM0,MM1
		PSUBQ MM0,mVar64
		PSUBQ XMM0,XMM1
		PSUBQ XMM0,mVar128
		
		PSUBSB XMM0,XMM1
		PSUBSB XMM0,mVar128
		
		PSUBSW XMM0,XMM1
		PSUBSW XMM0,mVar128
		
		PSUBUSB XMM0,XMM1
		PSUBUSB XMM0,mVar128
		
		PSUBUSW XMM0,XMM1
		PSUBUSW XMM0,mVar128
		
		PUNPCKHBW XMM0,XMM1
		PUNPCKHBW XMM0,mVar128
		
		PUNPCKHWD XMM0,XMM1
		PUNPCKHWD XMM0,mVar128
		
		PUNPCKHDQ XMM0,XMM1
		PUNPCKHDQ XMM0,mVar128
		
		PUNPCKHQDQ XMM0,XMM1
		PUNPCKHQDQ XMM0,mVar128
		
		PUNPCKLBW XMM0,XMM1
		PUNPCKLBW XMM0,mVar128
		
		PUNPCKLWD XMM0,XMM1
		PUNPCKLWD XMM0,mVar128
		
		PUNPCKLDQ XMM0,XMM1
		PUNPCKLDQ XMM0,mVar128
		
		PUNPCKLQDQ XMM0,XMM1
		PUNPCKLQDQ XMM0,mVar128
		
		PXOR XMM0,XMM1
		PXOR XMM0,mVar128
		
		CLFLUSH mVar8
		push eax
		lea eax, dword ptr arr
		CLFLUSH  [eax]
		pop eax
		
		LFENCE

		MASKMOVDQU XMM0,XMM1
		
		MFENCE

		MOVNTPD mVar128,XMM1
		MOVNTDQ mVar128,XMM1

		MOVNTI mVar32, eax

		PAUSE
	}
}
