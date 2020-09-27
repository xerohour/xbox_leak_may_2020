#include "base1.h"
#include "base2.h"

class derived1 : public base1, public base2
{
private:
	// private member variable.
	int derived1_private_var;
	// private member function.
	int derived1_private_func(int i);
protected:
	// protected member variable.
	int derived1_protected_var;
	// protected member function.
	int derived1_protected_func(int i);
public:
	// public member variable.
	int derived1_public_var;
	// class-nested enum.
	enum enum_class {one_class = 1, two_class, three_class};
	// declaration of class-nested enum var.
	enum_class enum_class_var;
	// static member variable.
	static int derived1_static_var;
	derived1(void);
	// non-inherited, non-overloaded member function.
	// public member function.
	int derived1_public_func(int i);
	// virtual inherited non-overloaded, overridden member function
	virtual int base1_virtual_func(int i);
	//static int derived1_static_func(int i);
	// inherited, overloaded, non-overridden member function
	int base1_overloaded_func(char ch);
	// inherited, non-overloaded, overridden member function
	int base1_overridden_func(int i);
};

