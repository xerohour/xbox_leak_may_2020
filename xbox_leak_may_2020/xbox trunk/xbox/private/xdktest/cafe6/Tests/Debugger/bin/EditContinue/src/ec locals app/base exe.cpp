#include <windows.h>


enum ENUM1{ENUM1_VAL1, ENUM1_VAL2, ENUM1_VAL3};
ENUM1 ENUM1Global = ENUM1_VAL1;


struct STRUCT1
{
	int m_intSTRUCT1;
} STRUCT1Global;


union UNION1
{
	int m_intUNION1;
} UNION1Global;


class CBase
{
public:
	int m_intCBase;
	CBase(void){m_intCBase = 0;}
};


class CDerived : public CBase
{
public:
	int m_intCDerived;
	CDerived(void){m_intCDerived = 0;}
};


void ExeFunc1(void);
void ExeFunc2(void);
void ExeFunc3(void);
void ExeFunc4(void);
void ExeFunc5(void);
void ExeFunc6(void);
void ExeFunc7(void);
void ExeFunc8(void);
void ExeFunc9(void);
void ExeFunc10(void);

/*
extern "C"
{

__declspec(dllimport) void DefaultDllFunc(void);
__declspec(dllimport) void DllFunc1(void);
__declspec(dllimport) void DllFunc2(void);
__declspec(dllimport) void DllFunc3(void);
__declspec(dllimport) void DllFunc4(void);
__declspec(dllimport) void DllFunc5(void);

}
*/

int intGlobal = 0;


int FAR PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)

{	// first line of WinMain().
	
	//DefaultDllFunc();
	ExeFunc1();
	while(0);	// line after call to ExeFunc1().
	ExeFunc2();
	while(0);	// line after call to ExeFunc2().
	ExeFunc3();
	while(0);	// line after call to ExeFunc3().
	ExeFunc4();
	while(0);	// line after call to ExeFunc4().
	ExeFunc5();
	while(0);	// line after call to ExeFunc5().

	return 0;	// last line of WinMain().
}


void ExeFunc1(void)
{	// first line of ExeFunc1().
	ExeFunc6();
	while(0);	// line after call to ExeFunc6().
	// DllFunc1();
}	// last line of ExeFunc1().


void ExeFunc2(void)
{	// first line of ExeFunc2().
	ExeFunc7();
	while(0);	// line after call to ExeFunc7().	
	// DllFunc2();
}	// last line of ExeFunc2().


void ExeFunc3(void)
{	// first line of ExeFunc3().
	ExeFunc8();
	while(0);	// line after call to ExeFunc8().
	// DllFunc3();
}	// last line of ExeFunc3().


void ExeFunc4(void)
{	// first line of ExeFunc4().
	ExeFunc9();
	while(0);	// line after call to ExeFunc9().
	// DllFunc4();
}	// last line of ExeFunc4().


void ExeFunc5(void)
{	// first line of ExeFunc5().
	ExeFunc10();
	while(0);	// line after call to ExeFunc10().
	// DllFunc5();
}	// last line of ExeFunc5().


void ExeFunc6(void)
{	// first line of ExeFunc6().
	while(0);
}	// last line of ExeFunc6().


void ExeFunc7(void)
{	// first line of ExeFunc7().
	while(0);
}	// last line of ExeFunc7().


void ExeFunc8(void)
{	// first line of ExeFunc8().
	while(0);
}	// last line of ExeFunc8().


void ExeFunc9(void)
{	// first line of ExeFunc9().
	while(0);
}	// last line of ExeFunc9().


void ExeFunc10(void)
{	// first line of ExeFunc10().
	while(0);
}	// last line of ExeFunc10().
