#include <afxwin.h>
#include <map>

//make all data known data types
int global_int = 1;
float global_float = 1.0;


void globalfunc(void){}

template<typename Type>
void globaltemplatefunc(Type var) //ENC: edit template function declaration here: any case, rebuild
{ // first line of globaltemplatefunc
	int var1 = 1; //ENC: edit template function definition here: any case, rebuild
	int var2 = 3;
	int var3 = var1 + var2 + (int)var;
}


template<> void globaltemplatefunc<float>(float var)
{ // first line of float specialization of globaltemplatefunc
	float var1 = 1.0; //ENC: edit explicit template function specialization here: any case, rebuild
	float var2 = 2.0;
	float var3 = 2.0*var1 + 2.0*var2 + var;
}

template void globaltemplatefunc<double>(double); //ENC: edit explicit template function instantiation here: any case, rebuild

template<typename Type> class SomeReallyLongSymbolName; //ENC: edit template class declaration here

template<typename Type> class SomeReallyLongSymbolName
{
    public:
		SomeReallyLongSymbolName(Type var)
		{
			m_Type = var;
		}
		
		void func(Type var)
		{ // first line of template function 'func'
			m_Type = var;
			globalfunc();
			while(0); // in template function 'func'
		}
	//ENC: edit template class definition here, add some variable
		Type m_Type;
};

SomeReallyLongSymbolName<int> intObject(0);
SomeReallyLongSymbolName<char> charObject('a');

template<> class SomeReallyLongSymbolName<double>{}; //ENC: edit explicit class template specialization, add parameter?

typedef std::map<DWORD, SomeReallyLongSymbolName<DWORD> > STLTypeWithLongName;


void FuncWithTemplateClassParam(SomeReallyLongSymbolName<int> intObjectParam)
{ // first line of FuncWithTemplateClassParam
	int test=0;
}


int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lszCmdLine, int nCmdShow)
{
	SomeReallyLongSymbolName<float> localfloatObject(1.0); //ENC: edit template class instantiation (ex change to double)
	int var1 = 8;
	intObject.func(1);
	while(0); //1
	intObject.func(2);
	while(0); //2

	charObject.func('b');
	while(0); //b
	charObject.func('c');
	while(0); //c

	FuncWithTemplateClassParam(intObject);
	while(0); //intObject

	globaltemplatefunc(1);
	while(0); //::1
	globaltemplatefunc('a');
	while(0); //::a
	globaltemplatefunc(global_float);
	while(0);// ::global_float

	STLTypeWithLongName STLObjectWithLongName;
	if(var1==0)
	{
		var1++;
	}
	else 
		if(var1==4)
	{
		var1=0;
	}
	return 0;
}

