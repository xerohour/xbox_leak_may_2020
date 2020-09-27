#include <windows.h>

__int64 GlobalInt64 = 0;

void set_GlobalInt64(int Hi, int Low);
void set_XMMRegister(int regid, char upper, int Hi, int Low);
void set_XMMRegister(int regid, int X3, int X2, int X1, int X0);

__int64 FloatVal[] =	//	32 bit register values for each integer
{
	0,				//	0
	0x3F800000,		//	1
	0x40000000,		//	2
	0x40400000,		//	3
	0x40800000,		//	4
	0x40A00000,		//	5
	0x40C00000,		//	6
	0x40E00000,		//	7	
	0x41000000,		//	8
	0x41100000,		//	9
	0x41200000,		//	10
	0x41300000,		//	11
	0x41400000,		//	12
	0x41500000,		//	13
	0x41600000,		//	14
	0x41700000,		//	15
	0x41800000,		//	16
	0x41880000,		//	17	
	0x41900000,		//	18
	0x41980000,		//	19
	0x41A00000,		//	20
	0x41A80000,		//	21
	0x41B00000,		//	22
	0x41B80000,		//	23
	0x41C00000,		//	24
	0x41C80000,		//	25
	0x41D00000,		//	26
	0x41D80000,		//	27	
	0x41E00000,		//	28
	0x41E80000,		//	29
	0x41F00000,		//	30
	0x41F80000,		//	31
	0x42000000,		//	32
};

void TestMMX()
{
	//	MMX test
	
	__int64 regVal = 0;

	regVal = 100;
	__asm { MOVQ MM0, regVal }

	regVal = 200;
	__asm { MOVQ MM1, regVal }

	regVal = 300;
	__asm { MOVQ MM2, regVal }

	regVal = 400;
	__asm { MOVQ MM3, regVal }

	regVal = 500;
	__asm { MOVQ MM4, regVal }

	regVal = 600;
	__asm { MOVQ MM5, regVal }

	regVal = 700;
	__asm { MOVQ MM6, regVal }

	regVal = 800;
	__asm { MOVQ MM7, regVal }

	regVal = 0;
	regVal = 1;
}

void Test3dNow()
{
	//	3dnow test
	
	__int64 regVal = 0;

	regVal = 4611686019492741120;
	__asm { MOVQ MM0, regVal }

	regVal = 4647714816524288000;
	__asm { MOVQ MM1, regVal }

	regVal = 4665729215040061440;
	__asm { MOVQ MM2, regVal }

	regVal = 4683743613553737728;
	__asm { MOVQ MM3, regVal }

	regVal = 4692750812811624448;
	__asm { MOVQ MM4, regVal }

	regVal = 4701758012068462592;
	__asm { MOVQ MM5, regVal }

	regVal = 4710765211325300736;
	__asm { MOVQ MM6, regVal }

	regVal = 4719772410582138880;
	__asm { MOVQ MM7, regVal }

	regVal = 0;
	regVal = 1;
}

void TestXMM()
{
	//	XMM test
	
	float fVal = 0;

	set_XMMRegister(0,4,3,2,1); 
	set_XMMRegister(1,8,7,6,5);
	set_XMMRegister(2,12,11,10,9); 
	set_XMMRegister(3,16,15,14,13);
	set_XMMRegister(4,20,19,18,17); 
	set_XMMRegister(5,24,23,22,21);
	set_XMMRegister(6,28,27,26,25); 
	set_XMMRegister(7,32,31,30,29);

	fVal = 1.0;
	fVal = 2.0;
	fVal = 3.0;
}


int FAR PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//	MMX & MMX2 tests for 3dNow, Katmai
	TestMMX();
	Test3dNow();
	TestXMM();

	return 1;							
}

void set_GlobalInt64(int Hi, int Low)
{
	__int64 HiVal = FloatVal[Hi];
	__int64 LowVal = FloatVal[Low];

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

