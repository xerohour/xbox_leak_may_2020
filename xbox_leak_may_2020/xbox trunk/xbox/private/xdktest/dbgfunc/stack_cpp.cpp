//
// \\vsqaauto\VC6Snaps\Debugger\Core\src\stack01\cppstack.cpp

#include "stack.h"

typedef void (*PFNINT)(void*, unsigned long);
typedef int (*PFNVAR)(void*, ...);

void afuncint(void*, DWORD)
{
}

static int afuncvar(void*, ...)
{
	return 0;
}

class AClass
{
public:
	PFNINT pfnint;
	PFNVAR pfnvar;
	static int m_static;
	virtual void __fastcall virt_method() {};
} the_Aclass;

int AClass::m_static = -1;
static AClass static_Aclass;

typedef AClass* PAClass;
typedef class ADerived: public AClass{
	void __fastcall virt_method() {
		int in_derived = 0;
	};
} CLASS_DERIVED;



CLASS_DERIVED the_derived;


static int Result=0;

static PFNVAR globalsecond1;
PFNVAR globalsecond2;

PAClass pclass = &the_Aclass;

//****************************************************************************
// Purpose:     function taking class & pointer to class
int paramclass(AClass the_class, AClass *the_pointer)
{
	return 0;
}

//****************************************************************************
// Purpose:     function taking typedef class & pointer to class
int paramtypedefclass(CLASS_DERIVED the_class, PAClass the_pointer)
{
	return 0;
}

//****************************************************************************
// Purpose:     function taking pointers to a functions as params
PFNINT pointerstofunc(PFNINT first, PFNVAR second)
{
	static PFNINT localfirst=first;
	PFNVAR localsecond=second;
	int i=0;
	return first;
}

char *szdummy;

//****************************************************************************
// Purpose:     overloaded function taking int
int overloaded_function(int first)
{
	static int i=1;
	static int j=first;
    szdummy = "overloaded function taking int";
	return first;
}

//****************************************************************************
// Purpose:     overloaded function taking long
int overloaded_function(long first)
{
    szdummy = "overloaded function taking long";
	return first;
}



//****************************************************************************
// Purpose:     fpo function
double __fastcall fastcall_function(int first,long second, float third)
{

	return first + second + third;
}


//****************************************************************************
// Purpose:     stdcall function
double __stdcall stdcall_function(int first,long second, float third)
{

	return fastcall_function(first, second, third);
}


//****************************************************************************
// Purpose:     cdecl function
double __cdecl cdecl_function(int first,long second, float third)
{
	return stdcall_function(first, second, third);
}
static int foo()
{
	return 10;
}

// disable conversion
#pragma warning ( disable : 4244 )

int Cxx_Tests()
{
	pointerstofunc(afuncint, afuncvar);
	pointerstofunc(afuncint, NULL);

	Result += overloaded_function(1);
	Result += overloaded_function(1L);
	static double doubleresult=cdecl_function(1, 2, 3.0);
	Result += (int) doubleresult;
	Result += 3;

    // try and set a breakpoint inside a virtual method
    // see if we stop inside of it

    CLASS_DERIVED *pDerived = new CLASS_DERIVED;
    PAClass pClass = pDerived;
    pClass->virt_method();

    // clean up

    delete pDerived;

	return Result==0;
};
