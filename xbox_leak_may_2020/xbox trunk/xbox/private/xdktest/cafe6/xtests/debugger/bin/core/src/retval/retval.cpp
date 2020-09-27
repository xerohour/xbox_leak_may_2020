#include <xtl.h>

void func_void(void)
	{
	}

char func_char(void)
	{
	return CHAR_MAX;
	}

unsigned char func_uchar(void)
	{
	return UCHAR_MAX;
	}

int func_int(void)
	{
	return INT_MAX;
	}

unsigned int func_uint(void)
	{
	return UINT_MAX;
	}

long func_long(void)
	{
	return LONG_MAX;
	}

unsigned long func_ulong(void)
	{
	return ULONG_MAX;
	}

float func_float(void)
	{
	return FLT_MAX;
	}
	
double func_double(void)
	{
	return DBL_MAX;
	}

long double func_ldouble(void)
	{
	return LDBL_MAX;
	}

class base
	{
	public: 
		int m_var;
		virtual int m_func(void){return 0;}
		int overloaded_func(int i){return i;}
	};

class derived : public base
	{
	public:
		virtual int m_func(void){return m_var;}
		char overloaded_func(char ch){return ch;}
	};

base func_class(void)
	{
	base b;
	b.m_var = 1;
	return b;
	}

base *func_pclass(void)
	{
	base *pb = new base;
	pb->m_var = 1;
	return pb;
	}

void func_throw(void)
	{
	throw 0;
	}

int recursive_func(int i)
	
	{
	if(i == 5)
		return i; // most nested return
	else
		{
		recursive_func(i + 1);
		return i;
		} 
	}

#ifndef _MPPC_
int __fastcall fastcall_func(void)
	{
	return 1;
	}
#endif // _MPPC_
		
int	no_debug_info(void);
int	line_numbers_only(void);
//int dll_func(void);

int overloaded_func(int i){return i;}
char overloaded_func(char ch){return ch;}


void __cdecl main()
	
	{
	func_void();

	for(0;0;0);
	while(0);

	char ch = func_char();
	unsigned char uch = func_uchar();
	int i = func_int();
	unsigned int ui = func_uint();
	long l = func_long();
	unsigned long ul = func_ulong();
	float flt = func_float();
	double dbl = func_double();
	long double ldbl = func_ldouble();

	base b = func_class();
	base *pb = func_pclass();

	GetLastError();
	no_debug_info();
	line_numbers_only();
	recursive_func(1);

#ifndef _MPPC_
	fastcall_func();
#endif // _MPPC_

	abs(-1);
	while(0);

	base *pd = new derived;
	pd->m_var = 1;
	pd->m_func();
	while(0);

	pb->overloaded_func(1);
	pb->overloaded_func('a');
	overloaded_func(2);
	overloaded_func('b');

#ifndef _68K_
	//dll_func();
#endif // _68K_

	try
		{
		func_throw();
		}

	catch(...)
		{
		}

	while (1); return; //xbox never exit main
	}
