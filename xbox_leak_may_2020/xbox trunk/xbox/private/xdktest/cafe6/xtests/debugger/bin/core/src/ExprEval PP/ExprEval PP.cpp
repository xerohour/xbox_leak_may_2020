#include <xtl.h> //xbox #include <windows.h>
#include <xmmintrin.h>

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

const int FLOATVAL_BASE = -3;
__m128 Var128;

void ClearXMMReg(int regid);
void set_GlobalInt64(int Hi, int Low);
void set_XMMRegister(int regid, char upper, int Hi, int Low);
void set_XMMRegister(int regid, int X3, int X2, int X1, int X0);

void set_Register(int regid, __int64 regval);
void test_MMX();
void test_3dNow();
void test_3dNowEnhanced();
void test_Katmai();

/*************************************************************************************************
*
*	main()
*
*************************************************************************************************/

void __cdecl main()
{
	
	//	3d Now! register tests
	test_MMX();
	test_3dNow();
	test_3dNowEnhanced();
	test_Katmai();

	return;
}


/********************************************
*
*	3d Now! - function to set a MMi register
*
*********************************************/

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

void test_MMX()
{
	//	MMX tests
	
	__int64 count = 0;

	//	test all registers for operability
	count = 1000; 		__asm {	MOVQ MM0,count }
	count = 2000; 		__asm {	MOVQ MM1,count }
	count = 3000; 		__asm {	MOVQ MM2,count }
	count = 4000; 		__asm {	MOVQ MM3,count }
	count = 5000; 		__asm {	MOVQ MM4,count }
	count = 6000; 		__asm {	MOVQ MM5,count }
	count = 7000; 		__asm {	MOVQ MM6,count }
	count = 8000; 		__asm {	MOVQ MM7,count }

	//	shift left
	set_Register(0,400);	count = 2; 	__asm {	PSLLW MM0,count }
	set_Register(0,9);		count = 6; 	__asm {	PSLLW MM0,count }	


	//	mov 32 bit
	count = 123456789; 		__asm {	MOVD MM0,count }	

	count = 4294967296; 	__asm {	MOVD MM0,count }	//	32 bits	
	count = 4294967295; 	__asm {	MOVD MM0,count }	//	32 bits - 1
	count = 4294967297; 	__asm {	MOVD MM0,count }	//	32 bits + 1
	count = 8589934592; 	__asm {	MOVD MM0,count }	//	33 bits


	count = 123456789; 		__asm {	MOVQ MM0,count }
	count = 4294967296; 	__asm {	MOVQ MM0,count }	
	count = 4294967295; 	__asm {	MOVQ MM0,count }	
	count = 4294967297; 	__asm {	MOVQ MM0,count }	
	count = 8589934592; 	__asm {	MOVQ MM0,count }	

	
	//	Pack with Signed Saturation
	set_Register(0,1);	count = 1; 	__asm {	PACKSSWB MM0,count }
	set_Register(0,1);	count = 1; 	__asm {	PACKSSDW MM0,count }

	//	Pack with Unsigned Saturation	
	set_Register(0,1);	count = 1; 	__asm {	PACKUSWB MM0,count }

	//	Packed Add
	set_Register(0,100);	count = 1; 	__asm {	PADDB MM0,count }
	set_Register(0,127);	count = 127; 	__asm {	PADDB MM0,count }
	set_Register(0,128);	count = 128; 	__asm {	PADDB MM0,count }
	set_Register(0,256);	count = 256; 	__asm {	PADDB MM0,count }

	set_Register(0,100);	count = 1; 	__asm {	PADDW MM0,count }
	set_Register(0,65535);	count = 1; 	__asm {	PADDW MM0,count }
	set_Register(0,65536);	count = 65536; 	__asm {	PADDW MM0,count }
	
	set_Register(0,100);	count = 1; 	__asm {	PADDD MM0,count }
	set_Register(0,1);	count = 4294967295; 	__asm {	PADDD MM0,count }
	set_Register(0,4294967296);	count = 4294967296; 	__asm {	PADDD MM0,count }

	//	Packed Add with Saturation
	set_Register(0,150);	count = 100; 	__asm {	PADDSB MM0,count }
	set_Register(0,65470);	count = 100; 	__asm {	PADDSW MM0,count }

	//	Packed Add Unsigned with Saturation 
	set_Register(0,200);	count = 100; 	__asm {	PADDUSB MM0,count }
	set_Register(0,65530);	count = 100; 	__asm {	PADDUSW MM0,count }

	//	Bitwise Logical And 
	set_Register(0,256);	count = 32; 	__asm {	PAND MM0,count }
	set_Register(0,255);	count = 15; 	__asm {	PAND MM0,count }

	//	Bitwise Logical And Not 
	set_Register(0,15);	count = 15; 	__asm {	PANDN MM0,count }
	set_Register(0,1);	count = 15; 	__asm {	PANDN MM0,count }

	//	Bitwise Logical Or
	set_Register(0,64);	count = 15; 	__asm {	POR MM0,count }

	//	Compare for Equal 
	set_Register(0,400);	count = 100; 	__asm {	PCMPEQB MM0,count }
	set_Register(0,200);	count = 100; 	__asm {	PCMPEQB MM0,count }
	set_Register(0,120);	count = 120; 	__asm {	PCMPEQB MM0,count }

	set_Register(0,400);	count = 100; 	__asm {	PCMPEQW MM0,count }
	set_Register(0,120);	count = 120; 	__asm {	PCMPEQW MM0,count }
	
	set_Register(0,400);	count = 100; 	__asm {	PCMPEQD MM0,count }
	set_Register(0,120);	count = 120; 	__asm {	PCMPEQD MM0,count }

	//	Compare for Greater Than
	set_Register(0,200);	count = 100; 	__asm {	PCMPGTB MM0,count }
	set_Register(0,127);	count = 100; 	__asm {	PCMPGTB MM0,count }
	set_Register(0,128);	count = 127; 	__asm {	PCMPGTB MM0,count }
	set_Register(0,127);	count = 128; 	__asm {	PCMPGTB MM0,count }	
	set_Register(0,256);	count = 0; 		__asm {	PCMPGTB MM0,count }
	set_Register(0,350);	count = 100; 		__asm {	PCMPGTB MM0,count }
	
	set_Register(0,400);	count = 100; 	__asm {	PCMPGTW MM0,count }
	set_Register(0,65536);	count = 0; 		__asm {	PCMPGTW MM0,count }
	
	set_Register(0,400);	count = 100; 	__asm {	PCMPGTD MM0,count }
	set_Register(0,4294967296);	count = 0; 	__asm {	PCMPGTD MM0,count }

	//	Packed Multiply and Add 
	set_Register(0,5);	count = 260; 	__asm {	PMADDWD MM0,count }
	set_Register(0,65536);	count = 65536; 	__asm {	PMADDWD MM0,count }

	//	Packed Multiply High
	set_Register(0,400);	count = 100; 	__asm {	PMULHW MM0,count }
	set_Register(0,6556);	count = 10; 	__asm {	PMULHW MM0,count }

	//	Packed Multiply Low
	set_Register(0,6556);	count = 10; 	__asm {	PMULLW MM0,count }
	set_Register(0,655);	count = 10; 	__asm {	PMULLW MM0,count }

	//	Packed Shift Left Logical
	set_Register(0,23);	count = 4; 	__asm {	PSLLW MM0,count }
	set_Register(0,65537);	count = 1; 	__asm {	PSLLW MM0,count }
	set_Register(0,2147483648);	count = 1; 	__asm {	PSLLW MM0,count }
	
	set_Register(0,23);	count = 4; 	__asm {	PSLLD MM0,count }
	set_Register(0,2147483649);	count = 1; 	__asm {	PSLLD MM0,count }
	set_Register(0,4294967296);	count = 1; 	__asm {	PSLLD MM0,count }
	set_Register(0,2147483648);	count = 1; 	__asm {	PSLLD MM0,count }
	
	set_Register(0,23);	count = 4; 	__asm {	PSLLQ MM0,count }

	//	Packed Shift Right Arithmetic
	set_Register(0,400);	count = 3; 		__asm {	PSRAW MM0,count }
	set_Register(0,5);		count = 3; 		__asm {	PSRAW MM0,count }
	set_Register(0,65538);	count = 1; 		__asm {	PSRAW MM0,count }
	set_Register(0,65536);	count = 1; 		__asm {	PSRAW MM0,count }
	set_Register(0,65537);	count = 1; 		__asm {	PSRAW MM0,count }
	
	set_Register(0,400);	count = 3; 		__asm {	PSRAD MM0,count }
	set_Register(0,5);	count = 3; 		__asm {	PSRAD MM0,count }
	set_Register(0,4294967298);	count = 1; 		__asm {	PSRAD MM0,count }
	set_Register(0,4294967296);	count = 1; 		__asm {	PSRAD MM0,count }
	set_Register(0,4294967297);	count = 1; 		__asm {	PSRAD MM0,count }
	
	//	Packed Shift Right Logical
	set_Register(0,256);	count = 4; 		__asm {	PSRLW MM0,count }
	set_Register(0,6);		count = 4; 		__asm {	PSRLW MM0,count }
	set_Register(0,65536);	count = 1; 		__asm {	PSRLW MM0,count }
	set_Register(0,65537);	count = 1; 		__asm {	PSRLW MM0,count }
	set_Register(0,65538);	count = 1; 		__asm {	PSRLW MM0,count }
	
	set_Register(0,256);	count = 4; 		__asm {	PSRLD MM0,count }
	set_Register(0,10);	count = 4; 		__asm {	PSRLD MM0,count }
	set_Register(0,4294967296);	count = 1; 		__asm {	PSRLD MM0,count }
	set_Register(0,4294967298);	count = 1; 		__asm {	PSRLD MM0,count }
	set_Register(0,4294967297);	count = 1; 		__asm {	PSRLD MM0,count }
	
	set_Register(0,256);	count = 4; 		__asm {	PSRLQ MM0,count }
	set_Register(0,15);		count = 4; 		__asm {	PSRLQ MM0,count }
//	set_Register(0,18446744073709551616);	count = 1; 	__asm {	PSLLQ MM0,count }
//	set_Register(0,18446744073709551617);	count = 1; 	__asm {	PSLLQ MM0,count }
//	set_Register(0,18446744073709551618);	count = 1; 	__asm {	PSLLQ MM0,count }
	
	//	Packed Subtract
	set_Register(0,240);	count = 100; 	__asm {	PSUBB MM0,count }
	set_Register(0,340);	count = 100; 	__asm {	PSUBB MM0,count }
	set_Register(0,65736);	count = 100; 	__asm {	PSUBB MM0,count }
	
	set_Register(0,400);	count = 100; 	__asm {	PSUBW MM0,count }
	set_Register(0,65750);	count = 100; 	__asm {	PSUBW MM0,count }
	set_Register(0,4294967496);	count = 100; 	__asm {	PSUBW MM0,count }
	set_Register(0,4294967390);	count = 100; 	__asm {	PSUBW MM0,count }

	set_Register(0,400);	count = 100; 	__asm {	PSUBD MM0,count }
	set_Register(0,4294967496);	count = 100; 	__asm {	PSUBD MM0,count }
//	set_Register(0,18446744073709551816);	count = 100; 	__asm {	PSUBD MM0,count }
	
	//	Packed Subtract with Saturation
	set_Register(0,240);	count = 100; 	__asm {	PSUBSB MM0,count }
	set_Register(0,400);	count = 100; 	__asm {	PSUBSB MM0,count }
	set_Register(0,65736);	count = 100; 	__asm {	PSUBSB MM0,count }
	
	set_Register(0,400);	count = 100; 	__asm {	PSUBSW MM0,count }
	set_Register(0,65750);	count = 100; 	__asm {	PSUBSW MM0,count }
	set_Register(0,4294967496);	count = 100; 	__asm {	PSUBSW MM0,count }

	//	Packed Subtract Unsigned with Saturation
	set_Register(0,240);	count = 100; 	__asm {	PSUBUSB MM0,count }	//	all "correct" values
	set_Register(0,400);	count = 100; 	__asm {	PSUBUSB MM0,count }
	set_Register(0,65736);	count = 100; 	__asm {	PSUBUSB MM0,count }
	
	set_Register(0,400);	count = 100; 	__asm {	PSUBSW MM0,count }
	set_Register(0,65750);	count = 100; 	__asm {	PSUBSW MM0,count }
	set_Register(0,4294967496);	count = 100; 	__asm {	PSUBSW MM0,count }

	
	//	Unpack High Packed Data
	set_Register(0,1);	count = 400; 	__asm {	PUNPCKHBW MM0,count }
	set_Register(0,1);	count = 24600; 	__asm {	PUNPCKHBW MM0,count }
	set_Register(0,1);	count = 4294967496; 	__asm {	PUNPCKHBW MM0,count }
	set_Register(0,4294967496);	count = 1; 	__asm {	PUNPCKHBW MM0,count }
	set_Register(0,4294967496);	count = 4294967496; 	__asm {	PUNPCKHBW MM0,count }
	
	set_Register(0,1);	count = 400; 	__asm {	PUNPCKHWD MM0,count }
	set_Register(0,1);	count = 24600; 	__asm {	PUNPCKHWD MM0,count }
	set_Register(0,1);	count = 4294967296; 	__asm {	PUNPCKHWD MM0,count }
	set_Register(0,4294967296);	count = 1; 	__asm {	PUNPCKHWD MM0,count }
	set_Register(0,4294967296);	count = 4294967296; 	__asm {	PUNPCKHWD MM0,count }
	
	set_Register(0,1);	count = 400; 	__asm {	PUNPCKHDQ MM0,count }
	set_Register(0,1);	count = 24600; 	__asm {	PUNPCKHDQ MM0,count }
	set_Register(0,1);	count = 4294967296; 	__asm {	PUNPCKHDQ MM0,count }
	set_Register(0,4294967296);	count = 1; 	__asm {	PUNPCKHDQ MM0,count }
	set_Register(0,4294967296);	count = 4294967296; 	__asm {	PUNPCKHDQ MM0,count }
	
	//	Unpack Low Packed Data
	set_Register(0,1);	count = 2; 	__asm {	PUNPCKLBW MM0,count }
	set_Register(0,2);	count = 1; 	__asm {	PUNPCKLBW MM0,count }

	set_Register(0,1);	count = 2; 	__asm {	PUNPCKLWD MM0,count }
	set_Register(0,2);	count = 1; 	__asm {	PUNPCKLWD MM0,count }
	
	set_Register(0,1);	count = 2; 	__asm {	PUNPCKLDQ MM0,count }
	set_Register(0,2);	count = 1; 	__asm {	PUNPCKLDQ MM0,count }
	
	//	Bitwise Logical Exclusive OR
	set_Register(0,255);	count = 15; 	__asm {	PXOR MM0,count }
	set_Register(0,15);		count = 15; 	__asm {	PXOR MM0,count }

}

void test_3dNow()
{
	//	3dNow Tests

	__int64 count = 0;
	unsigned char int8 = 0;

	//	average of unsigned int bytes
	set_Register(0,1);	count = 1; 	__asm {	PAVGUSB MM0,count }	//	1
	set_Register(0,1);	count = 0; 	__asm {	PAVGUSB MM0,count }	//	1
	set_Register(0,0);	count = 1; 	__asm {	PAVGUSB MM0,count }	//	1
	set_Register(0,807437600515);	count = 661408385285; 	__asm {	PAVGUSB MM0,count }	//	734423025668

	//	float to 32 bit signed int
	set_Register(0,1);	count = 1610612738; 	__asm {	PF2ID MM0,count }	//	2147483647
	set_Register(0,1);	count = 6917529037841629186; 	__asm {	PF2ID MM0,count }	//	9223372034707292159
	set_Register(0,1);	count = 1065353218; 	__asm {	PF2ID MM0,count }	//	1
	set_Register(0,1);	count = 1082130434; 	__asm {	PF2ID MM0,count }	//	4

	//	accumulator
	set_Register(0,1082130434);	count = 1065353218; 	__asm {	PFACC MM0,count }	//	4575657231080488962
	set_Register(0,1065353218);	count = 1; 	__asm {	PFACC MM0,count }	//	1065353218
	set_Register(0,1);	count = 1065353218; 	__asm {	PFACC MM0,count }	//	4575657229998358528
//	set_Register(0,4575657231080488962);	count = 1065353218; 	__asm {	PFACC MM0,count }	// not understood

	//	add
	set_Register(0,0);	count = 1065353218; 	__asm {	PFADD MM0,count }
	set_Register(0,1065353218);	count = 1073741826; 	__asm {	PFADD MM0,count }	//	1 + 2
	set_Register(0,1077936131);	count = 1082130434; 	__asm {	PFADD MM0,count }	//	3 + 4

	//	compare for equal
	set_Register(0,1);	count = 1; 	__asm {	PFCMPEQ MM0,count }	//	18446744073709551615
	set_Register(0,1065353218);	count = 1; 	__asm {	PFCMPEQ MM0,count }	//	18446744069414584320
	set_Register(0,1065353218);	count = 1065353218; 	__asm {	PFCMPEQ MM0,count } //	18446744073709551615
	set_Register(0,4575657231063711746);	count = 1065353218; 	__asm {	PFCMPEQ MM0,count }	//	4294967295
	set_Register(0,4575657231063711746);	count = 1077936131; 	__asm {	PFCMPEQ MM0,count }	//	0

	//	compare for greater than or equal to
	set_Register(0,1);	count = 1; 	__asm {	PFCMPGE MM0,count }	//	 //	18446744073709551615
	set_Register(0,1065353218);	count = 1065353218; 	__asm {	PFCMPGE MM0,count } //	18446744073709551615
	set_Register(0,4575657231063711746);	count = 1065353218; 	__asm {	PFCMPGE MM0,count }	//	18446744073709551615
	set_Register(0,4575657231063711746);	count = 1077936131; 	__asm {	PFCMPGE MM0,count }	//	18446744069414584320

	//	compare for greater than
	set_Register(0,1);	count = 1; 	__asm {	PFCMPGT MM0,count }
	set_Register(0,1065353218);	count = 1065353218; 	__asm {	PFCMPGT MM0,count } //	0
	set_Register(0,4575657231063711746);	count = 1065353218; 	__asm {	PFCMPGT MM0,count }	//	18446744069414584320
	set_Register(0,4575657231076294659);	count = 1065353218; 	__asm {	PFCMPGT MM0,count }	//	18446744073709551615
	
	//	maximum
	set_Register(0,1);	count = 1; 	__asm {	PFMAX MM0,count }
	set_Register(0,1);	count = 4575657231063711746; 	__asm {	PFMAX MM0,count }
	set_Register(0,4575657231063711746);	count = 1077936131; 	__asm {	PFMAX MM0,count }	//	4575657231076294659
	set_Register(0,4575657231076294659);	count = 4611686028091064322; 	__asm {	PFMAX MM0,count }	//	4611686028095258627

	//	minimum
	set_Register(0,1);	count = 1; 	__asm {	PFMIN MM0,count }
	set_Register(0,1);	count = 4575657231063711746; 	__asm {	PFMIN MM0,count }
	set_Register(0,4575657231076294659);	count = 4611686028091064322; 	__asm {	PFMIN MM0,count }	//	4575657231072100354
	set_Register(0,4575657231076294659);	count = 1065353218; 	__asm {	PFMIN MM0,count }	//	1065353218

	//	multiply
	set_Register(0,1);	count = 1; 	__asm {	PFMUL MM0,count }
	set_Register(0,1);	count = 4575657231076294659; 	__asm {	PFMUL MM0,count }
	set_Register(0,4575657231076294659);	count = 4611686028091064322; 	__asm {	PFMUL MM0,count }
	
	//	reciprocal
	set_Register(0,1);	count = 1; 	__asm {	PFRCP MM0,count } //	9187343237679939583

	set_Register(0,1);	set_Register(1,1073741826);	 __asm {	PFRCP MM0,MM1 }	//	reciprocal of 2 - 4539626226423168512
	count = 1092616193; 	__asm {	PFMUL MM0,count }	//	multiply 10 by 0.5 - 1084227265

	//	reciprocal, reciprocal intermediate step 1, reciprocal intermediate step 2
	set_Register(0,1);	set_Register(1,1073741826);	 __asm {	PFRCP MM0,MM1 }	//	reciprocal of 2 - 4539626226423168512
	__asm {	PFRCPIT1 MM1,MM0 }	//	MM1 - 1065418239
	__asm {	PFRCPIT2 MM0,MM1 }	//	1057029630
	count = 1092616193; 	__asm {	PFMUL MM0,count }	//	multiply 10 by 0.5 - 1084308863
		
	//	reciprocal sqrt
	set_Register(0,1);	count = 1; 	__asm {	PFRSQRT MM0,count }		//	9187343237679939583
	set_Register(0,1);	count = 1082130434; 	__asm {	PFRSQRT MM0,count }	//	4 - 4539627325934796544 (not same as 1/2)
	set_Register(0,1);	count = 1091567617; 	__asm {	PFRSQRT MM0,count }	//	9 - 4515608494420437504 - exact!!

	//	reciprocal sqrt, reciprocal sqrt intermediate step 1	
	set_Register(0,1);	set_Register(1,1082130434); 	__asm {	PFRSQRT MM0,MM1 }	//	4 - 4539627325934796544 (not same as 1/2)
	__asm {	PFRSQIT1 MM1,MM0 }	//	1065369343 - off

	set_Register(0,1);	set_Register(1,1091567617); 	__asm {	PFRSQRT MM0,MM1 }	//	9 - 4515608494420437504 (not same as 1/2)
	__asm {	PFRSQIT1 MM1,MM0 }	//	1065365418 - off
	
	//	subtract
	set_Register(0,1);	count = 1; 	__asm {	PFSUB MM0,count }
	set_Register(0,1065353218);	count = 1065353218; 	__asm {	PFSUB MM0,count }
	set_Register(0,1082130434);	count = 1065353218; 	__asm {	PFSUB MM0,count } //	 4-1 = 1077936131
	set_Register(0,4575657231076294659);	count =1065353218; 	__asm {	PFSUB MM0,count } //	4575657231072100354

	//	reverse subtraction
	set_Register(0,1);	count = 1; 	__asm {	PFSUBR MM0,count }
	set_Register(0,1065353218);	count = 1065353218; 	__asm {	PFSUBR MM0,count }
	set_Register(0,1065353218);	count = 1082130434; 	__asm {	PFSUBR MM0,count } //	 4-1 = 1077936131
	set_Register(0,1065353218);	count =4575657231076294659; 	__asm {	PFSUBR MM0,count } //	4575657231072100354

	//	32 bit signed int to float
	set_Register(0,1);	count = 1; 	__asm {	PI2FD MM0,count }	//	1065353216
	set_Register(0,1);	count = 12884901890; 	__asm {	PI2FD MM0,count }	//	4629700418010611712
	set_Register(0,1);	count = 8589934593; 	__asm {	PI2FD MM0,count }	//	4611686019492741120

	//	multiply high
	set_Register(0,1);	count = 1; 	__asm {	PMULHRW MM0,count }
	set_Register(0,1688858450395145);	count = 1970346312335364; 	__asm {	PMULHRW MM0,count }	//	0!
	set_Register(0,2816897267531808);	count = 28148786178032640; 	__asm {	PMULHRW MM0,count }	//	8590000129!

	int8 = 1;

	//	load a processor cache into data cache
	__asm {	PREFETCH int8 }

	//	same, sets MES state to modified
	__asm {	PREFETCHW int8 }

}


void test_3dNowEnhanced()
{
	//	enhanced 3dnow tests

	__int64 count = 0;
	unsigned char count8 = 0;

	//	packed float to int word with sign extend
	set_Register(0,1);	count = 1; 	__asm {	PF2IW MM0,count }	//	0
	set_Register(0,1);	count = 4674736422882639874; 	__asm {	PF2IW MM0,count }	//	74 - 30064771076
	set_Register(0,1);	count = 4692750816015024128; 	__asm {	PF2IW MM0,count }	//	10-0 - 42949672960
	set_Register(0,1);	count = 13835058067084935170; 	__asm {	PF2IW MM0,count }	//	-2-1 - 18446744069414584319
	set_Register(0,1);	count = 3338666241; 	__asm {	PF2IW MM0,count }		//	4294934528

	//	packed float negative accumulate
	set_Register(0,1);	count = 1; 	__asm {	PFNACC MM0,count }	//	0
	set_Register(0,1);	count = 4647714825124708354; 	__asm {	PFNACC MM0,count }	//	0&47 - 4629700416936869888
	set_Register(0,4647714825120514050);	count = 4665729223629996034; 	__asm {	PFNACC MM0,count }	//	45&65 - 13799029259328552960

	//	packed float mixed +ve -ve accumulate
	set_Register(0,1);	count = 1; 	__asm {	PFPNACC MM0,count }
	set_Register(0,4647714825124708354);	count = 1; 	__asm {	PFPNACC MM0,count }	//	1077936128
	set_Register(0,4647714825124708354);	count = 4647714825120514050; 	__asm {	PFPNACC MM0,count }	//	47,45 - 4688247221760557056
   
	//	packed int word to float
	//	somewhat erroneous values

	set_Register(0,1);	count = 1; 	__asm {	PI2FW MM0,count }	//	1065353216
	set_Register(0,1);	count = 281466386841599; 	__asm {	PI2FW MM0,count }	//	13835058058495000576
	set_Register(0,1);	count = 30064771076; 	__asm {	PI2FW MM0,count }	//	4674736414292705280
 	set_Register(0,1);	count = 42949672960; 	__asm {	PI2FW MM0,count }	//	4692750811720056832
 	set_Register(0,1);	count = 32768; 	__asm {	PI2FW MM0,count }		//	3338665984

	//	packed swap double word
	set_Register(0,1);	count = 1; 	__asm {	PSWAPD MM0,count }  // 4294967296
	set_Register(0,1);	count = 4294967302; 	__asm {	PSWAPD MM0,count }  //	25769803777
	set_Register(0,1);	count = 25769803776; 	__asm {	PSWAPD MM0,count }	//	6

	//	streaming store using byte mask
	set_Register(0,1);	set_Register(1,255); 	__asm {	MASKMOVQ MM0,MM1 }
	set_Register(0,258);	set_Register(1,65535); 	__asm {	MASKMOVQ MM0,MM1 }
	set_Register(0,137561200);	set_Register(1,16777215); 	__asm {	MASKMOVQ MM0,MM1 }
	
	//	streaming store
	set_Register(0,10);	count = 1; 	__asm {	MOVNTQ count,MM0 }	//	count == mm0
	set_Register(0,34359738373);	count = 1; 	__asm {	MOVNTQ count,MM0 }

			
	//	packed avg of unsigned byte	
	set_Register(0,1);	count = 1; 	__asm {	PAVGB MM0,count }	//	1
	set_Register(0,1);	count = 0; 	__asm {	PAVGB MM0,count }	//	1
	set_Register(0,0);	count = 1; 	__asm {	PAVGB MM0,count }	//	1
	set_Register(0,807437600515);	count = 661408385285; 	__asm {	PAVGB MM0,count }	//	734423025668
	
	//	packed avg of unsigned word	
	set_Register(0,1);	count = 1; 	__asm {	PAVGW MM0,count }
	set_Register(0,13527687376553903363);	count = -7388155192895340543; 	__asm {	PAVGW MM0,count }	//	12293138128684057218
	set_Register(0,60000);	count = 50000; 	__asm {	PAVGW MM0,count }	//	55000


	//	extract word into int register
	set_Register(0,1);	__asm {	PEXTRW EAX,MM0,1 }	//	eax = 0
	set_Register(0,65538);	__asm {	PEXTRW EAX,MM0,1 }	//	eax = 1 


	//	insert word from int register

	set_Register(0,65540);	_asm { mov EAX, 2 }	 __asm {	PINSRW MM0, EAX, 1 }	//	MM0 = 131076

	//	packed max signed word
	set_Register(0,533958026541989887);	count = 576742218707042305; 	__asm {	PMAXSW MM0,count }	//	576742223002009601
	set_Register(0,1431633921);	count = 1431633926; 	__asm {	PMAXSW MM0,count }	//	1431633926
	
	//	packed max unsigned byte
	set_Register(0,730413330687);	count = 1834203085567; 	__asm {	PMAXUB MM0,count }	//	1834219928319
	
	//	packed min signed word
	set_Register(0,576742218707042305);	count = 533958026541989887; 	__asm {	PMINSW MM0,count }	//	533958022247022591
	set_Register(0,1431633926);	count = 1431633921; 	__asm {	PMINSW MM0,count }	//	1431633921
	
	//	packed min unsigned byte	
	set_Register(0,1834203085567);	count = 730413330687; 	__asm {	PMINUB MM0,count }	//	730396487935
	
	//	move mask to integer register
	set_Register(0,17356997142132031728);	__asm {	PMOVMSKB EAX, MM0 }	//	EAX = 217
	
	//	packed multiply high unsigned word
	set_Register(0,18446744069427435932);	count = 562954313924618; 	__asm {	PMULHUW MM0,count }	//	281474976841729

	//	prefetch non-temporal access
	__asm {	PREFETCHNTA count8 }
	
	//	prefetch to all cache levels
	__asm {	PREFETCHT0 count8 }
	
	//	prefetch to all cache levels except 0
	__asm {	PREFETCHT1 count8 }
	
	//	prefetch to all cache levels except 0 & 1
	__asm {	PREFETCHT2 count8 }

	//	packed sum of absolute byte differences
	set_Register(0,1300991683403908355);	count = 722835462628311810; 	__asm {	PSADBW MM0,count }	//	35
	set_Register(0,722835462628311810);	count = 1300991683403908355; 	__asm {	PSADBW MM0,count }	//	35
	
	//	packed shuffle word
	set_Register(0,1);	count = 1; 	__asm {	PSHUFW MM0,count,0 }	//	1 in every word 281479271743489
	set_Register(0,1);	count = 578437695752307201; 	__asm {	PSHUFW MM0,count,228 }	//	578437695752307201
	
	// store fence
	__asm {	SFENCE }
}

void test_Katmai()
{
	//	Katmai (PIII) Tests

	float fVal = 0.0;
	__int32 i32Val = 0;

	//	verifying each register for expression evaluation
	set_XMMRegister(0,1,2,3,4);
	set_XMMRegister(1,2,3,4,5);
	set_XMMRegister(2,3,4,5,6);
	set_XMMRegister(3,4,5,6,7);
	set_XMMRegister(4,5,6,7,8);
	set_XMMRegister(5,6,7,8,9);
	set_XMMRegister(6,7,8,9,10);
	set_XMMRegister(7,-1,-2,-3,6);


	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,3,4,5);
	__asm { addps   xmm0, xmm1 }	//	XMM0 - 3,5,7,9

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,3,4,5);
	__asm { addss   xmm0, xmm1 }	//	1,2,3,9

	ClearXMMReg(0); set_XMMRegister(1,0,4,0,1);
	__asm { andnps   xmm0, xmm1 }

	set_XMMRegister(0,1,1,1,1); set_XMMRegister(1,2,2,6,7);
	__asm { andnps   xmm0, xmm1 }

	set_XMMRegister(0,1,2,3,4); ClearXMMReg(1);
	__asm { andps   xmm0, xmm1 }	//	0

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,1,2,3,5);
	__asm { andps   xmm0, xmm1 }	//	1,2,3,4

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,1,2,3,5);
	__asm { cmpeqps   xmm0, xmm1 }	//	eq-eq-eq-neq (neq=>0)

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,1,2,3,4);
	__asm { cmpeqps   xmm0, xmm1 }	//	eq-eq-eq-eq

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,6,2,3,5);
	__asm { cmpeqss   xmm0, xmm1 }	//	1,2,3,0

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,6,2,3,4);
	__asm { cmpeqss   xmm0, xmm1 }	//	1,2,3,eq

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,1,3,1,0);
	__asm { cmpltps   xmm0, xmm1 }	//	no-yes-no-no

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,0,4,0);
	__asm { cmpltss   xmm0, xmm1 }	//	1,2,3,no

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,0,4,4);
	__asm { cmpltss   xmm0, xmm1 }	//	1,2,3,no

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,0,4,5);
	__asm { cmpltss   xmm0, xmm1 }	//	1,2,3,yes

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,1,3,0,0);
	__asm { cmpleps   xmm0, xmm1 }	//	yes,yes,no,no

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,0,4,0);
	__asm { cmpless   xmm0, xmm1 }	//	1,2,3,no

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,0,4,5);
	__asm { cmpless   xmm0, xmm1 }	//	1,2,3,yes

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,0,4,4);
	__asm { cmpless   xmm0, xmm1 }	//	1,2,3,yes

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,1,3,4,4);
	__asm { cmpneqps  xmm0, xmm1 }	//	no,y,y,n

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,3,4,4);
	__asm { cmpneqss   xmm0, xmm1 }	//	1,2,3,n

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,3,4,3);
	__asm { cmpneqss   xmm0, xmm1 }	//	1,2,3,y


	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,2,4,1);
	__asm { cmpnltps   xmm0, xmm1 }	//	n,y,n,y

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,1,4,1);
	__asm { cmpnltss   xmm0, xmm1 }	//	1,2,3,y

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,1,4,5);
	__asm { cmpnltss   xmm0, xmm1 }	//	1,2,3,n

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,1,4,4);
	__asm { cmpnltss   xmm0, xmm1 }	//	1,2,3,y

	set_XMMRegister(0,1,2,3,7); set_XMMRegister(1,2,3,3,5);
	__asm { cmpnleps   xmm0, xmm1 }	//	n,n,n,y

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,3,1,1);
	__asm { cmpnless   xmm0, xmm1 }	//	1,2,3,y

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,3,1,5);
	__asm { cmpnless   xmm0, xmm1 }	//	1,2,3,n

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,3,1,4);
	__asm { cmpnless   xmm0, xmm1 }	//	1,2,3,n


	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,3,4,5);
	__asm { comiss   xmm0, xmm1 }	//	0 (eq last no)

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,3,4,4);
	__asm { comiss   xmm0, xmm1 }	//	1

	set_XMMRegister(0,1,2,0,0); 
	GlobalInt64 = 3;  GlobalInt64  = GlobalInt64 << 32;	GlobalInt64 |= 4;
	__asm { cvtpi2ps   xmm0, GlobalInt64 }	//	1,2,3,4


	set_XMMRegister(0,1,2,2,5);
	__asm { cvtps2pi   mm0, xmm0 }	//	MM0 has 2,5

	set_XMMRegister(0,1,2,3,4); __asm { mov eax, 17 }
	__asm { cvtsi2ss   xmm0, eax }	//	1,2,3,17

	set_XMMRegister(0,1,2,3,4);
	__asm { cvtss2si   eax, xmm0 }	//	eax has 4

	set_XMMRegister(0,1,2,7,5);
	__asm { cvttps2pi   mm0, xmm0 }	//	MM0 has 7,5

	set_XMMRegister(0,1,2,3,7);
	__asm { cvttss2si   eax, xmm0 }	//	eax has 7

	set_XMMRegister(0,1,2,9,8); set_XMMRegister(1,2,2,3,2);
	__asm { divps   xmm0, xmm1 }	//	0.5,1,3,4

	set_XMMRegister(0,1,2,3,10); set_XMMRegister(1,2,2,3,2);
	__asm { divss   xmm0, xmm1 }	//	1,2,3,5

	set_XMMRegister(0,5,2,7,4); set_XMMRegister(1,4,2,9,5);
	__asm { maxps   xmm0, xmm1 }	//	5,2,9,5

	set_XMMRegister(0,5,2,7,4); set_XMMRegister(1,4,2,9,6);
	__asm { maxss   xmm0, xmm1 }	//	5,2,7,6

	set_XMMRegister(0,5,2,7,4); set_XMMRegister(1,4,2,9,2);
	__asm { maxss   xmm0, xmm1 }	//	5,2,7,4


	set_XMMRegister(0,5,2,7,4); set_XMMRegister(1,4,2,9,3);
	__asm { minps   xmm0, xmm1 }	//	4,2,7,3

	set_XMMRegister(0,1,2,3,7); set_XMMRegister(1,4,2,9,6);
	__asm { minss   xmm0, xmm1 }	//	1,2,3,6

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,4,2,9,6);
	__asm { minss   xmm0, xmm1 }	//	1.2.3.4

	set_XMMRegister(0,1,2,3,4);	set_XMMRegister(1,5,6,7,8);
	__asm { movaps   xmm0, xmm1 }	//	5,6,7,8

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,5,6,2,3);
	__asm { movhlps   xmm0, xmm1 }	//	1,2,5,6

	set_XMMRegister(0,1,2,3,4); set_GlobalInt64(6,7);
	__asm { movhps   xmm0, GlobalInt64 }	//	6.7.3.4

	set_XMMRegister(0,1,2,3,4); set_GlobalInt64(6,7);
	__asm { movlps   xmm0, GlobalInt64 }	//	1.2.6.7

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,3,5,6);
	__asm { movlhps   xmm0, xmm1 }	//	5.6.3.4

	set_XMMRegister(0,1,-2,3,-1);
	__asm { movmskps   eax, xmm0 }	//	5

	
	set_XMMRegister(0,7,8,9,-1);
	__asm { movntps	Var128, xmm0 }

	set_XMMRegister(0,1,2,3,4); fVal = 9;
	__asm { movss   xmm0, fVal }	//	0.0.0.9

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,5,6,7,8);
	__asm { movups   xmm0, xmm1 }	//	5,6,7,8

	set_XMMRegister(0,1,2,3,2); set_XMMRegister(1,2,3,0,5);
	__asm { mulps   xmm0, xmm1 }	//	2,6,0,10

	set_XMMRegister(0,1,2,3,2); set_XMMRegister(1,2,3,4,4);
	__asm { mulss   xmm0, xmm1 }	//	1,2,3,8

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,0,3,4,5);
	__asm { orps   xmm0, xmm1 }	//	1,3,6,5

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,3,4,5);
	__asm { rcpps   xmm0, xmm1 }	//	1/2, 1/3, 1/4, 1/5

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,3,4,2);
	__asm { rcpss   xmm0, xmm1 }	//	1,2,3,1/2

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,1,3,4,9);
	__asm { rsqrtps   xmm0, xmm1 }	//	reciprocal 1,1.73,2,3

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,1,3,4,9);
	__asm { rsqrtss   xmm0, xmm1 }	//	1,2,3, 1/3


	__asm { sfence }

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,5,6,7,8);
	__asm { shufps   xmm0, xmm1, 114 }	//	7,5,4,2

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,1,3,4,9);
	__asm { sqrtps   xmm0, xmm1 }	//	1,1.73,2,3

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,1,3,4,9);
	__asm { sqrtss   xmm0, xmm1 }	//	1,2,3,3

	__asm { stmxcsr   i32Val  }		//	8096 - not important

	set_XMMRegister(0,1,8,5,4); set_XMMRegister(1,2,3,4,1);
	__asm { subps   xmm0, xmm1 }	//	-1,5,1,3

	set_XMMRegister(0,1,8,5,4); set_XMMRegister(1,2,3,4,6);
	__asm { subss   xmm0, xmm1 }	//	1,8,5,-2

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,2,3,4,5);
	__asm { ucomiss   xmm0, xmm1 }	//	0

	set_XMMRegister(0,1,2,3,5); set_XMMRegister(1,2,3,4,5);
	__asm { ucomiss   xmm0, xmm1 }	//	1


	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,5,6,7,8);
	__asm { unpckhps   xmm0, xmm1 }	//	interleaved high - 5,1,6,2

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,5,6,7,8);
	__asm { unpcklps   xmm0, xmm1 }	//	interleaved low - 7,3,8,4

	set_XMMRegister(0,1,2,3,4); set_XMMRegister(1,1,2,4,5);
	__asm { xorps   xmm0, xmm1 }	//	xor - 0,0,1.76324e-038,2.93874e-039

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

