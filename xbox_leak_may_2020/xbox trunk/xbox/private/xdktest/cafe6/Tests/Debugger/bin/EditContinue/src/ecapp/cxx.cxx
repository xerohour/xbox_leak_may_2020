#include "cxx.h"
#include "include.cpp"

class base
{
private:
	int m_private;

protected:
	int m_protected;

public: 
	base(void) {m_private = 1; m_protected = 2;}
	virtual ~base(void){};

	int m_public;
	char *m_pchar;
	virtual void virtual_func(int i);
	virtual int m_func(void){return 0;}
	int overloaded_func(int i){return i;}

	virtual void virtual_func1() { int i = 5; };
	virtual int  virtual_func2() { return 10; };
	virtual int  virtual_func3(int ind) { return ind*2; };
};

class derived1 : public base
{
private:
	char ch;

public:
	derived1(void){ch = 'a';}
};

class derived2 : public derived1
{
private:
	float f;

public:
	derived2(void){f = 1.0f;}
};

class A_CLASS
{
public:
	virtual void func(unsigned char ch){}
	int i;
};


class derived : public base
{
private:
	int private_value;
	int id;
	static int ref_cnt;

public:
	derived(); // c-tor
	~derived();

	void virtual_func1() { int i = 7; };
	int  virtual_func2(int j) { return j; };

	static int static_func() {return 9;};
	void clear() { derived::private_value=0;};
	int getID() { return id;};
	int getCount() {return ref_cnt;};
	operator int() { return (int)private_value; };
	inline void inline_func() { private_value++; };

	virtual int m_func(void){return m_public;}
	char overloaded_func(char ch){return ch;}
};

int derived::ref_cnt=1;

derived::derived()
{              
    id=ref_cnt;
    ref_cnt++;
    private_value=-2*id;
};

derived::~derived()
{             
    ref_cnt--;
};


base func_class(void)
{
	base b;
	b.m_public = 1;
	return b;
}

base *func_pclass(void)
{
	base *pb = new base;
	pb->m_public = 1;
	return pb;
}

void base::virtual_func(int i)
{
	m_private = i+1;
	m_public = i;
}

int global = 0;
int result = 0;
int iGlobal;
int *piGlobal = &iGlobal;

int Test_func2 ( )
{
	base1 b1;
	base2 b2;
	derivedm drvm;

	int ib1 = b1.base1_overloaded_func(1);
	ib1 = b1.base1_overridden_func(1);
	ib1 = b1.base1_virtual_func(1);

	int ib2 = b2.base2_virtual_func(1);

	int iderived = drvm.derivedm_public_func(1+2);
	iderived = drvm.base1_virtual_func(1+2);
	iderived = drvm.base1_overloaded_func('C');
	iderived = drvm.base1_overridden_func(1+2);

	return 0;
};

int Test_func ( )
{
	base b = func_class();
	base *pb = func_pclass();
	base *pd = new derived;
	pd->m_public = 1;
	pd->m_func();
	pb->overloaded_func(1);
	pb->overloaded_func('a');

	derived drv1;

	drv1.virtual_func1();
	result = drv1.base::virtual_func2();
	result = drv1.virtual_func2(7);
	result = drv1.virtual_func3(9);

	result = drv1.getID();
	result = drv1.getCount();

	//CALL_CONVERSION:
	result = drv1;

	result = derived::static_func();
	drv1.clear();  // clear private value

	drv1.inline_func();
	result = drv1;

	base* object_base = new base();
	base* object1 = new derived1();
	base* object2 = new derived2();
	base* object = object1;
	
	object = object2; // assignment
	
	A_CLASS obj;
	obj.i = 1;
	obj.func('a');

	iGlobal = 0;
	*piGlobal = 1;
	int *pint = &iGlobal;
	b.virtual_func(0);

	base *pb2;
	base *pb1 = new base;
	pb2 = pb1;
	*pb1 = *pb1;

	pb->m_public = 0;
	pb->virtual_func(0);

	b.m_public = 1;
	b.virtual_func(1);

	::global = 1;

//	return 0;
	return Test_func2();
};

int base1::base1_func(int i)
{
	return base1_var + i;
}

int base1::base1_virtual_func(int i)
{
	return base1_var + i;
}

int base1::base1_overloaded_func(int i)
{
	return base1_var + i;
}

int base1::base1_overridden_func(int i)
{
	return base1_var + i;
}

// inherited, non-overloaded, non-overridden member function
int base2::base2_func(int i)
{
	return base2_var + i;
}

int base2::base2_virtual_func(int i)
{
	return base2_var + i;
}

int derivedm::derivedm_static_var = 1;

derivedm::derivedm(void)
{
	derivedm_public_var = base1_var + base2_var;
	enum_class_var = two_class;
	derivedm_private_var = derivedm_public_var + 1;
	derivedm_protected_var = derivedm_private_var + 1;
}

// non-inherited, non-overloaded member function.
int derivedm::derivedm_public_func(int i)
{
	return derivedm_public_var + derivedm_protected_func(i);
}

// virtual inherited non-overloaded, overridden member function
int derivedm::base1_virtual_func(int i)
{
	return derivedm_public_var + i;
}

/*
int derivedm::derivedm_static_func(int i)
{
	return derived_static_var + i;
}
*/

// inherited, overloaded, non-overridden member function
int derivedm::base1_overloaded_func(char ch)
{
	return derivedm_public_var + ch;
}

// inherited, non-overloaded, overridden member function
int derivedm::base1_overridden_func(int i)
{
	return derivedm_public_var + i;
}

// protected member function.
int derivedm::derivedm_protected_func(int i)
{
	return derivedm_public_var + derivedm_private_func(i);
}

// private member function.
int derivedm::derivedm_private_func(int i)
{
	return derivedm_public_var + i;
}


int Result=0;

extern "C"
{
	void Cxx_Tests();
}

void Cxx_Tests()
{
  //CALL_CONSTRUCTOR:
  fooclass foo1;

  //CALL_MEMBER_FUNC:
  Result=foo1.getID(); // getID
  Result=foo1.getCount();

  //CALL_CONVERSION:
  Result=foo1;

  //CALL_STATIC:
  Result=fooclass::static_func();
  foo1.clear();  // clear private value

  //CALL_INLINE:
  foo1.inline_func();
  Result=foo1;
  foo1.chain1_func();

  int test_include = Test_func();
};

void fooclass::chain1_func()
{					
      unsigned char local_cpp_func='A';
      private_value++;                // increment private value
	  int chain2_return = chain2_func();
      private_value--;                // decrement private value
};
