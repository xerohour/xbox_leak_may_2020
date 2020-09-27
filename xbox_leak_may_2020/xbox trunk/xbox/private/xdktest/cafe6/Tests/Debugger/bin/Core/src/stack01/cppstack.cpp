#include "cstack.h"
#include <windows.h>

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
{	MIPS_DMY_PROLOG
	return 0;
}

//****************************************************************************
// Purpose:     function taking typedef class & pointer to class
int paramtypedefclass(CLASS_DERIVED the_class, PAClass the_pointer)
{	MIPS_DMY_PROLOG
	return 0;
}

//****************************************************************************
// Purpose:     function taking pointers to a functions as params
PFNINT poniterstofunc(PFNINT first, PFNVAR second)
{	MIPS_DMY_PROLOG
	static PFNINT localfirst=first;
	PFNVAR localsecond=second;
	int i=0;
	return first;
}

//****************************************************************************
// Purpose:     overloaded function taking int
int overloaded_function(int first)
{	MIPS_DMY_PROLOG
	static int i=1;
	static int j=first;
	return first;
}

//****************************************************************************
// Purpose:     overloaded function taking long
int overloaded_function(long first)
{	MIPS_DMY_PROLOG

	return first;
}



//****************************************************************************
// Purpose:     fpo function
double __fastcall fastcall_function(int first,long second, float third)
{	MIPS_DMY_PROLOG

	return first + second + third;
}


//****************************************************************************
// Purpose:     stdcall function
double __stdcall stdcall_function(int first,long second, float third)
{	MIPS_DMY_PROLOG

	return fastcall_function(first, second, third);
}


//****************************************************************************
// Purpose:     cdecl function
double __cdecl cdecl_function(int first,long second, float third)
{	MIPS_DMY_PROLOG

	return stdcall_function(first, second, third);
}
static int foo()
{
	return 10;
}

int Cxx_Tests()
{
	poniterstofunc(afuncint, afuncvar);
	poniterstofunc(afuncint, NULL);

	Result+=overloaded_function(1);
	Result+=overloaded_function(1L);
	static dooubleresult=cdecl_function(1, 2, 3.0);
	Result+=(int)dooubleresult;
	Result += 3;
	return Result==0;
};
