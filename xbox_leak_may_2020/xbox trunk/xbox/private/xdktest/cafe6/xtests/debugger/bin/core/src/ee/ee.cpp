#include <xtl.h> //xbox #include <windows.h>
#include "derived1.h"
#include "struct1.h"
#include "union1.h"
#include "global.h"
#include "pch.h"
//#include "dll.h"


namespace namespace1
	
{
	// namespace variable.
	int namespace1_var = 1;

	// namespace function.
	int namespace1_func(int i)
	{
		return namespace1_var + i;
	}		
}


// struct declared in .cpp file (rather than header).
struct struct_cpp
	
{
private:
	int struct_cpp_var;

public:

	struct_cpp(void){struct_cpp_var = 0;}

	int struct_cpp_func(int i)
	{
		struct_cpp_var = i;
		return struct_cpp_var;
	}
};


// classed declared in .cpp file (rather than header).
class class_cpp
	
{
private:
	int class_cpp_var;
	
public:

	// const primitive member.
	const int m_const_int;
	const int *m_pconst_int;
	int const m_int_const;
	int const *m_pint_const;

	// const user type member.
	const struct_cpp m_const_struct_cpp;
	const struct_cpp *m_pconst_struct_cpp;
	struct_cpp const m_struct_cpp_const;
	struct_cpp const *m_pstruct_cpp_const;

	class_cpp(void)	:
		m_const_int(0), 
		m_pconst_int(&m_const_int),
		m_int_const(0),
		m_pint_const(&m_int_const),
		m_pconst_struct_cpp(&m_const_struct_cpp),
		m_pstruct_cpp_const(&m_struct_cpp_const)
	{
		class_cpp_var = 0;

	}

	int class_cpp_func(int i)
	{
		class_cpp_var = i;
		return class_cpp_var;
	}

};


// union declared in .cpp file (rather than header).
union union_cpp
	
{
private:
	int union_cpp_var;

public:

	union_cpp(void){union_cpp_var = 0;}

	int union_cpp_func(int i)
	{
		union_cpp_var = i;
		return union_cpp_var;
	}
};


// enum defined in cpp file (rather than header).
enum enum_cpp{one_cpp = 1, two_cpp, three_cpp};


// pointer to member function.
int (derived1::*ptr_to_mem_func)(int i) = &derived1::derived1_public_func;	
// pointer to member variable.
int derived1::*ptr_to_mem_var = &derived1::derived1_public_var;			


// global const primitive.
const int gconst_int = 0;
const int *gpconst_int;
int const gint_const = 0;
int const *gpint_const;


// global const user type.
const struct_cpp gconst_struct_cpp;
const struct_cpp *gpconst_struct_cpp;
struct_cpp const gstruct_cpp_const;
struct_cpp const *gpstruct_cpp_const;


void __cdecl main()
	
{
	// global const primitive.
	gpconst_int = &gconst_int;
	gpint_const = &gint_const;

	// global const user type.
	gpconst_struct_cpp = &gconst_struct_cpp;
	gpstruct_cpp_const = &gstruct_cpp_const;

	// local class.
	class class_local
	{
	public:
		int class_local_var;
	};

	// local struct.
	struct struct_local
	{
	public:
		int struct_local_var;
	};

	// local union.
	union union_local
	{
	public:
		int union_local_var;
	};

	// local enum.
	enum enum_local{one_local = 1, two_local, three_local};

	// static and dynamic access to class declared in header.
	derived1 derived1_obj;
	derived1 *derived1_obj_ptr = new derived1;

	// static and dynamic access to class declared in .cpp file.
	class_cpp class_cpp_obj;
	class_cpp *class_cpp_obj_ptr = new class_cpp;

	// static and dynamic access to local class.
	class_local class_local_obj;
	class_local *class_local_obj_ptr = new class_local;
	class_local_obj.class_local_var = 0;
	class_local_obj_ptr->class_local_var = 0;

	// static and dynamic access to class declared in .pch file.
	class_pch class_pch_obj;
	class_pch *class_pch_obj_ptr = new class_pch;
	/* xbox vc7 */ class_pch_obj.class_pch_func(0);
	
	// static and dynamic access to struct declared in header.
	struct1 struct1_obj;
	struct1 *struct1_obj_ptr = new struct1;

	// static and dynamic access to struct declared in .cpp file.
	struct_cpp struct_cpp_obj;
	struct_cpp *struct_cpp_obj_ptr = new struct_cpp;

	// static and dynamic access to local struct.
	struct_local struct_local_obj;
	struct_local *struct_local_obj_ptr = new struct_local;
	struct_local_obj.struct_local_var = 0;
	struct_local_obj_ptr->struct_local_var = 0;

	// static and dynamic access to struct declared in .pch file.
	struct_pch struct_pch_obj;
	struct_pch *struct_pch_obj_ptr = new struct_pch;
	/* xbox vc7 */ struct_pch_obj.struct_pch_func(0);
	
	// static and dynamic access to union declared in header.
	union1 union1_obj;
	union1 *union1_obj_ptr = new union1;

	// static and dynamic access to union declared in .cpp file.
	union_cpp union_cpp_obj;
	union_cpp *union_cpp_obj_ptr = new union_cpp;

	// static and dynamic access to local union.
	union_local union_local_obj;
	union_local *union_local_obj_ptr = new union_local;
	union_local_obj.union_local_var = 0;
	union_local_obj_ptr->union_local_var = 0;

	// static and dynamic access to union declared in .pch file.
	union_pch union_pch_obj;
	union_pch *union_pch_obj_ptr = new union_pch;
	/* xbox vc7 */ union_pch_obj.union_pch_func(0);

 	// reference to enum declared in header (non-pch).
	enum1 enum1_var = two;
	/* xbox vc7 bug, use all enum values otherwise debugger will go, huh? */
	enum1_var = one; enum1_var = three; enum1_var = two;

	// reference to enum declared in .cpp file.
	enum_cpp enum_cpp_var = three_cpp;
	/* xbox vc7 */ enum_cpp_var = one_cpp; enum_cpp_var = two_cpp; enum_cpp_var = three_cpp;

	// reference to local enum.
	enum_local enum_local_var = one_local;

	// reference to enum declared in a .pch file.
	enum_pch enum_pch_var = two_pch;
	/* xbox vc7 */ enum_pch_var = one_pch; enum_pch_var = three_pch; enum_pch_var = two_pch;

	// static local variable.
	static int static_local_var = 0;

	// static and dynamic access to class defined in dll.
	//xbox dll_class dll_class_obj;
	//xbox dll_class *dll_class_obj_ptr = new dll_class;

	// local const primitive.
	const int const_int = 0;
	const int *pconst_int = &const_int;
	int const int_const = 0;
	int const *pint_const = &int_const;

	// local const user type.
	const struct_cpp const_struct_cpp;
	const struct_cpp *pconst_struct_cpp = &const_struct_cpp;
	struct_cpp const struct_cpp_const;
	struct_cpp const *pstruct_cpp_const = &struct_cpp_const;

	return; /* run to here before evaluating [do not delete this comment] */
}

