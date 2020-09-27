//#define _UNICODE


#include <afxwin.h>
#include "nopdb.h"
#include "lineonly.h"
#include "test.h"


int global = 0;


class BASE_CLASS
	{
	private:
		int i_priv;
	protected:
		int i_prot;
	public:
		BASE_CLASS(void){i_priv = 1; i_prot = 2;}
		virtual ~BASE_CLASS(void){};
	};


class DERIVED_CLASS1 : public BASE_CLASS
	{
	private:
		char ch;
	public:
		DERIVED_CLASS1(void){ch = 'a';}
	};


class DERIVED_CLASS2 : public DERIVED_CLASS1
	{
	private:
		float f;
	public:
		DERIVED_CLASS2(void){f = 1.0f;}
	};


struct A_STRUCT
	{
	int i;
	};


class A_CLASS
	{
	public:
		virtual void func(unsigned char ch){}
		int i;
	};


void no_debug_func(void);
void diff_seg_func(void);


unsigned long __stdcall thread_func(void * pdata)
		{
		MessageBeep(0xffffffff);       
		while(1);
		return 0;
		}


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lparam)
	{
	static i = 0;
	i++;
	if(i == 2)
		return FALSE;
	else
		return TRUE;
	}	


int empty_func(void)
	{
	return 0;
	}


void many_params_func(int* i, CString &cstring, _TCHAR* tchar_array, 
					  float* f, A_STRUCT* a_struct)
	{
	*i = 1;
	cstring = "abc";
	lstrcpy(tchar_array, _TEXT("xyz"));
	*f = 1.0f;
	a_struct->i = 1;
	}


int set_ints_func(int i)
	{
	i++;
	global = 1;
	diff_seg_func();
	return i;
	}


void exception_func(void)	
	{
	int zero = 0;
	int i = 1 / zero;
	}

void overloaded_func(void){}
void overloaded_func(int i){}

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


int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, 
				   LPSTR lszCmdLine, int nCmdShow)
	
	{
	recursive_func(0);
	header_func();
	inline_func();
	overloaded_func();
	














	
	overloaded_func(0);
		  
	CString cstring = _TEXT("abc");
	_TCHAR tchar_array[sizeof(_TCHAR) * 4] = _TEXT("xyz");
	int i = 1;

	for(int x = 0; x < 100; x++)
		{
		while(0);
		}

	float f = 1.0f;
	
	struct A_STRUCT a_struct;
	a_struct.i = 1;
	
	BASE_CLASS* object_base = new BASE_CLASS();
	BASE_CLASS* object1 = new DERIVED_CLASS1();
	BASE_CLASS* object2 = new DERIVED_CLASS2();
	BASE_CLASS*	object = object1;

	i++;
	
	cstring = "1234";
	cstring = "5678";
	
	lstrcpy(tchar_array, _TEXT("pdz"));
	lstrcpy(tchar_array, _TEXT("pdq"));
	
	empty_func();
	many_params_func(&i, cstring, tchar_array, &f, &a_struct);
	
	object = object2;
	
	char array[4] = "abc";
	strcpy(array, "xyz");
	strcpy(array, "xy3");
	
	A_CLASS obj;
	obj.i = 1;
	obj.func('a');
	
	int (*pfunc)(int) = set_ints_func;
	i = set_ints_func(0);
	i = (*pfunc)(1);
	
	no_debug_func();
	diff_seg_func(); 
	
	HANDLE thread_hdl;
	DWORD thread_id;
	thread_hdl = CreateThread(NULL, 0, thread_func, 0, 0, &thread_id); 

	call_dlls:

	//HINSTANCE hinst_dll = GetModuleHandle("nopdb.dll");
	//BOOL ret = FreeLibrary(hinst_dll);
	//no_pdb_dll_func();

	HINSTANCE hinst_dll = GetModuleHandle("nopdb.dll");
	BOOL ret = FreeLibrary(hinst_dll);
	LoadLibrary("nopdb.dll");
	no_pdb_dll_func();

	line_only_dll_func();
	
	//exception_func();

	EnumWindows(EnumWindowsProc, 0);

	//while(1);

	return 0;
	}


