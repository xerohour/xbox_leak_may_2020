#include <xtl.h>
// This compiles  fine with VC6 and gcc

class test1
{
public:
    void Foo(int) {}
    void Foo2(void) {}
};

class test2
{
public:
	/*
	template <class T>
		void Set(char* n, T* o, void (T::*func)(void)) { someVal = 1;}
	*/
	template <class T, class R>
		void Set(char* n, T* o, R (T::*func)(void)) { someVal = 1;}
	template <class T, class R, class P1>
		void Set(char* n, T* o, R (T::*func)(P1)) { someVal = 2; }
	template <class T, class R, class P1, class P2>
		void Set(char* n, T* o, R (T::*func)(P1,P2)) { someVal = 3; }

	int someVal;
};

void __cdecl main()
{
    test1 t1;
    test2 t2;

    t2.Set("blah", &t1, &test1::Foo);
    t2.Set("blah", &t1, &test1::Foo2);
}

