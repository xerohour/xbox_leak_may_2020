///////////////////////////////////////////////////////////////////////////////
//	EECASE.CPP
//											 
//	Created by :			
//		VCBU QA		
//
//	Description :								 
//		TestWizard Generated script		 

#include "stdafx.h"
#include "eecase.h"	

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
				 	 
IMPLEMENT_TEST(CEEIDETest, CDbgTestBase, "General", -1, CEESubSuite)
									   
												 
void CEEIDETest::PreRun(void)
{
	// call the base class
	CTest::PreRun();
}


char* data[][2] = 

{
	// note: the class "derived1" and its base classes are all declared in headers
	//       and implemented in cpp files.

	// non-inherited, non-overloaded member function.
	// public member function.
	"derived1_obj.derived1_public_func",		"ADDRESS derived1::derived1_public_func(int)",         
	"derived1_obj_ptr->derived1_public_func",	"ADDRESS derived1::derived1_public_func(int)",
	    
	// inherited, non-overloaded, non-overridden member function.
	"derived1_obj.base2_func",		"ADDRESS base2::base2_func(int)",                                
	"derived1_obj_ptr->base2_func",	"ADDRESS base2::base2_func(int)",                           
	
	// inherited, overloaded, non-overridden member function.
	"derived1_obj.base1_overloaded_func",				"ADDRESS derived1::base1_overloaded_func(char)",      
	"derived1_obj_ptr->base1_overloaded_func",			"ADDRESS derived1::base1_overloaded_func(char)", 
	"derived1_obj.base1_overloaded_func(char)",			"ADDRESS derived1::base1_overloaded_func(char)",     
	"derived1_obj_ptr->base1_overloaded_func(char)",	"ADDRESS derived1::base1_overloaded_func(char)",
	// TODO: can't get at (int) version; can't even step into it.
	//"derived1_obj.base1_overloaded_func(int)",		"ADDRESS base1::base1_overloaded_func(int)",     
	//"derived1_obj_ptr->base1_overloaded_func(int)",	"ADDRESS base1::base1_overloaded_func(int)",
	
	// inherited, non-overloaded, overridden member function.
	"derived1_obj.base1_overridden_func",		"ADDRESS derived1::base1_overridden_func(int)",
 	"derived1_obj_ptr->base1_overridden_func",	"ADDRESS derived1::base1_overridden_func(int)",

	// virtual inherited non-overloaded, overridden member function.
	// TODO: only getting addresses when these are evaluated.
	//"derived1_obj.base1_virtual_func",		"ADDRESS derived1::base1_virtual_func(int)",
	//"derived1_obj_ptr->base1_virtual_func", "ADDRESS derived1::base1_virtual_func(int)",

	// static member function.
	// TODO: can't link when defined in class.
	//"derived1::derived1_static_func", "ADDRESS derived1::derived1_static_func(int)",
	
	// static member variable.
	"derived1::derived1_static_var", "1",

	// pointer to member variable.
	// TODO: seems to point to wrong place.
	//"*ptr_to_mem_var", "3",

	// pointer to member function.
	// TODO: just get address.
	// "*ptr_to_mem_func", "ADDRESS derived1::derived1_public_func(int)",

	// class-nested enum.
	"derived1_obj.enum_class_var",		"two_class",
	"derived1_obj_ptr->enum_class_var", "two_class",
	"derived1::one_class",				"1",
	"derived1::two_class",				"2",
	"derived1::three_class",			"3",

	// non-inherited, non-static, member variable.
	// public member variable.
	"derived1_obj.derived1_public_var",			"3",
	"derived1_obj_ptr->derived1_public_var",	"3",

	// inherited member variable.
	"derived1_obj.base1_var",		"1",
	"derived1_obj_ptr->base1_var",	"1",

	// protected member function.
	"derived1_obj.derived1_protected_func",			"ADDRESS derived1::derived1_protected_func(int)",         
	"derived1_obj_ptr->derived1_protected_func",	"ADDRESS derived1::derived1_protected_func(int)",

	// private member function.
	"derived1_obj.derived1_private_func",		"ADDRESS derived1::derived1_private_func(int)",         
	"derived1_obj_ptr->derived1_private_func",	"ADDRESS derived1::derived1_private_func(int)",

	// private member variable.
	"derived1_obj.derived1_private_var",		"4",
	"derived1_obj_ptr->derived1_private_var",	"4",

	// protected member variable.
	"derived1_obj.derived1_protected_var",		"5",
	"derived1_obj_ptr->derived1_protected_var",	"5",

	// class defined in dll.
	// TODO: get "member function not present".
	//"dll_class_obj.dll_class_func",			"ADDRESS dll_class::dll_class_func(int)",
	//"dll_class_obj_ptr->dll_class_func",	"ADDRESS dll_class::dll_class_func(int)",
	//xbox "dll_class_obj.dll_class_var",			"0",
	//xbox "dll_class_obj_ptr->dll_class_var",		"0",

	// struct (declared in header, implemented in cpp file).
	"struct1_obj.struct1_func",			"ADDRESS struct1::struct1_func(int)",
	"struct1_obj_ptr->struct1_func",	"ADDRESS struct1::struct1_func(int)",
	"struct1_obj.struct1_var",			"0",
	"struct1_obj_ptr->struct1_var",		"0",

	// union (declared in header, implemented in cpp file).
	"union1_obj.union1_func",		"ADDRESS union1::union1_func(int)",
	"union1_obj_ptr->union1_func",	"ADDRESS union1::union1_func(int)",
	"union1_obj.union1_var",		"0",
	"union1_obj_ptr->union1_var",	"0",

	// class (declared and implemented in cpp file).
	// TODO: get "member function not present"
	//"class_cpp_obj.class_cpp_func",			"ADDRESS class_cpp::class_cpp_func(int)",
	//"class_cpp_obj_ptr->class_cpp_func",	"ADDRESS class_cpp::class_cpp_func(int)",
	"class_cpp_obj.class_cpp_var",			"0",
	"class_cpp_obj_ptr->class_cpp_var",		"0",

	// struct (declared and implemented in cpp file).
	// TODO: get "member function not present"
	//"struct_cpp_obj.struct_cpp_func",		"ADDRESS struct_cpp::struct_cpp_func(int)",
	//"struct_cpp_obj_ptr->struct_cpp_func",	"ADDRESS struct_cpp::struct_cpp_func(int)",
	"struct_cpp_obj.struct_cpp_var",		"0",
	"struct_cpp_obj_ptr->struct_cpp_var",	"0",

	// union (declared and implemented in cpp file).
	// TODO: get "member function not present"
	//"union_cpp_obj.union_cpp_func",			"ADDRESS union_cpp::union_cpp_func(int)",
	//"union_cpp_obj_ptr->union_cpp_func",	"ADDRESS union_cpp::union_cpp_func(int)",
	"union_cpp_obj.union_cpp_var",			"0",
	"union_cpp_obj_ptr->union_cpp_var",		"0",

	// enum (defined in cpp file)
	"enum_cpp_var",	"three_cpp",
	"one_cpp",		"1",
	"two_cpp",		"2",
	"three_cpp",	"3",

	// class (declared in pch file, implemented in cpp file).
	"class_pch_obj.class_pch_func",			"ADDRESS class_pch::class_pch_func(int)",
	"class_pch_obj_ptr->class_pch_func",	"ADDRESS class_pch::class_pch_func(int)",
	"class_pch_obj.class_pch_var",			"0",
	"class_pch_obj_ptr->class_pch_var",		"0",

	// struct (declared in pch file, implemented in cpp file).
	"struct_pch_obj.struct_pch_func",		"ADDRESS struct_pch::struct_pch_func(int)",
	"struct_pch_obj_ptr->struct_pch_func",	"ADDRESS struct_pch::struct_pch_func(int)",
	"struct_pch_obj.struct_pch_var",		"0",
	"struct_pch_obj_ptr->struct_pch_var",	"0",

	// union (declared in pch file, implemented in cpp file).
	"union_pch_obj.union_pch_func",			"ADDRESS union_pch::union_pch_func(int)",
	"union_pch_obj_ptr->union_pch_func",	"ADDRESS union_pch::union_pch_func(int)",
	"union_pch_obj.union_pch_var",			"0",
	"union_pch_obj_ptr->union_pch_var",		"0",

	// enum (defined in pch file)
	"enum_pch_var",	"two_pch",
	"one_pch",		"1",
	"two_pch",		"2",
	"three_pch",	"3",

	// class (defined locally).
	"class_local_obj.class_local_var",		"0",
	"class_local_obj_ptr->class_local_var",	"0",

	// struct (defined locally).
	"struct_local_obj.struct_local_var",		"0",
	"struct_local_obj_ptr->struct_local_var",	"0",

	// union (defined locally).
	"union_local_obj.union_local_var",		"0",
	"union_local_obj_ptr->union_local_var",	"0",

	// enum (defined locally)
	"enum_local_var",	"one_local",
	"one_local",		"1",
	"two_local",		"2",
	"three_local",		"3",

	// non-overloaded global function.
	"global_func", "ADDRESS global_func(int)",

	// overloaded global function.
	"overloaded_global_func",		"ADDRESS overloaded_global_func(char)",
	"overloaded_global_func(int)",	"ADDRESS overloaded_global_func(int)",
	"overloaded_global_func(char)",	"ADDRESS overloaded_global_func(char)",
	
	// global variable.
	"global_var", "0",

	// static local variable.
	"static_local_var", "0",

	// namespace function.
	"namespace1::namespace1_func", "ADDRESS namespace1::namespace1_func(int)",

	// namespace variable.
	"namespace1::namespace1_var", "1",
	
	// in-line function (defined in header)
	// TODO: can't find this symbol.
	//"inline_func", "ADDRESS inline_func(int)",
	
	// enum (defined in header)
	"enum1_var",	"two",
	"one",			"1",
	"two",			"2",
	"three",		"3" 
};


// in order to initialize the "tests" array like we do below we need to mimmic the EXPR_INFO
// class which uses CStrings instead of LPCSTRs. CStrings prevent such initialization.
struct EXPR_INFO2
{
	EXPR_STATE	state;
	LPCSTR		type;
	LPCSTR		name;
	LPCSTR		value;
};


// initialize the data for all the tests (state, type, name, and value of each expression).
EXPR_INFO2 tests[] =

{
	// local const primitive.
	{NOT_EXPANDABLE,	"const int",	"const_int",	"0"},
	{COLLAPSED,			"const int *",	"pconst_int",	"ADDRESS"},
	{NOT_EXPANDABLE,	"const int",	"*pconst_int",	"0"},
	{NOT_EXPANDABLE,	"const int",	"int_const",	"0"},
	{COLLAPSED,			"const int *",	"pint_const",	"ADDRESS"},
	{NOT_EXPANDABLE,	"const int",	"*pint_const",	"0"},

	// global const primitive.
	{NOT_EXPANDABLE,	"const int",	"gconst_int",	"0"},
	{COLLAPSED,			"const int *",	"gpconst_int",	"ADDRESS gconst_int"},
	{NOT_EXPANDABLE,	"const int",	"*gpconst_int", "0"},
	{NOT_EXPANDABLE,	"const int",	"gint_const",	"0"},
	{COLLAPSED,			"const int *",	"gpint_const",	"ADDRESS gint_const"},
	{NOT_EXPANDABLE,	"const int",	"*gpint_const", "0"},

	// member const primitive.
	// TODO(michma - 2/13/98): fails due to bug VS98: 30574.
	//{NOT_EXPANDABLE,	"const int",	"class_cpp_obj.m_const_int",	"0"},
	{COLLAPSED,			"const int *",	"class_cpp_obj.m_pconst_int",	"ADDRESS"},
	{NOT_EXPANDABLE,	"const int",	"*class_cpp_obj.m_pconst_int",	"0"},
	// TODO(michma - 2/13/98): fails due to bug VS98: 30574.
	//{NOT_EXPANDABLE,	"const int",	"class_cpp_obj.m_int_const",	"0"},
	{COLLAPSED,			"const int *",	"class_cpp_obj.m_pint_const",	"ADDRESS"},
	{NOT_EXPANDABLE,	"const int",	"*class_cpp_obj.m_pint_const",	"0"},

	// local const struct.
	// TODO(michma - 2/13/98): fails due to bug VS98:30574.
	//{COLLAPSED, "const struct_cpp",	"const_struct_cpp",		"{...}"},
	{COLLAPSED, "const struct_cpp *",	"pconst_struct_cpp",	"ADDRESS"},
	{COLLAPSED, "const struct_cpp",		"*pconst_struct_cpp",	"{...}"},
	// TODO(michma - 2/13/98): fails due to bug VS98: 30574.
	//{COLLAPSED, "const struct_cpp",	"struct_cpp_const",		"{...}"},
	{COLLAPSED, "const struct_cpp *",	"pstruct_cpp_const",	"ADDRESS"},
	{COLLAPSED, "const struct_cpp",		"*pstruct_cpp_const",	"{...}"},

	// global const struct.
	// TODO(michma - 2/13/98): fails due to bug VS98: 30574.
	//{COLLAPSED, "const struct_cpp",	"gconst_struct_cpp",	"{...}"},
	{COLLAPSED, "const struct_cpp *",	"gpconst_struct_cpp",	"ADDRESS gconst_struct_cpp"},
	{COLLAPSED, "const struct_cpp",		"*gpconst_struct_cpp",	"{...}"},
	// TODO(michma - 2/13/98): fails due to bug VS98: 30574.
	//{COLLAPSED, "const struct_cpp",	"gstruct_cpp_const",	"{...}"},
	{COLLAPSED, "const struct_cpp *",	"gpstruct_cpp_const",	"ADDRESS gstruct_cpp_const"},
	{COLLAPSED, "const struct_cpp",		"*gpstruct_cpp_const",	"{...}"},

	// member const struct.
	// TODO(michma - 2/13/98): fails due to bug VS98: 30574.
	//{COLLAPSED, "const struct_cpp",	"class_cpp_obj.m_const_struct_cpp",		"{...}"},
	{COLLAPSED, "const struct_cpp *",	"class_cpp_obj.m_pconst_struct_cpp",	"ADDRESS"},
	{COLLAPSED, "const struct_cpp",		"*class_cpp_obj.m_pconst_struct_cpp",	"{...}"},
	// TODO(michma - 2/13/98): fails due to bug VS98:30574.
	//{COLLAPSED, "const struct_cpp",	"class_cpp_obj.m_struct_cpp_const",		"{...}"},
	{COLLAPSED, "const struct_cpp *",	"class_cpp_obj.m_pstruct_cpp_const",	"ADDRESS"},
	{COLLAPSED, "const struct_cpp",		"*class_cpp_obj.m_pstruct_cpp_const",	"{...}"}
};


void CEEIDETest::Run(void)
	
{
	/******************
	 * INITIALIZATION *
	 ******************/

	/*xbox
	if(!InitProject("ee\\dll", PROJECT_DLL))
	{
		m_pLog->RecordInfo("ERROR: could not init dll project");
		return;
	}
	*/

	if(!InitProject("ee\\ee", PROJECT_XBE))
	{
		m_pLog->RecordInfo("ERROR: could not init ee project");
		return;
	}

	// step to the "return" in ee.exe (all objects will be in scope).
	src.Open("ee.cpp");
	src.Find("run to here before evaluating");	
	dbg.StepToCursor();	
	cxx.Enable();

	// evaluate all the expressions.
	for(int i = 0; i < (sizeof(data) / (sizeof(CString) * 2)); i++)
	{
		if(cxx.ExpressionValueIs(data[i][0], data[i][1]))
			LOG->RecordSuccess("%s\n", data[i][0]);
		else
			LOG->RecordFailure("%s\n", data[i][0]);
	}

	cxx.Disable();

	// we need to enable the ee differently for these tests since we want to check all expression info, not just values.
	cxx.Enable(EE_ALL_INFO);
	// we only want to randomly run each test once.
	RandomNumberCheckList rncl(sizeof(tests) / sizeof(EXPR_INFO2));
	// indexes into the test array.
	int iTest;
	
	// randomly run each test once.
	while((iTest = rncl.GetNext()) != -1)
	
	{
		// transfer EXPR_INFO2 data to EXPR_INFO object.
		EXPR_INFO expr_info;
		cxx.FillExprInfo(expr_info, tests[iTest].state, tests[iTest].type, tests[iTest].name, tests[iTest].value);
		
		if(cxx.VerifyExprInfo(expr_info))
			LOG->RecordSuccess("%s\n", expr_info.name);
		else
			LOG->RecordFailure("%s\n", expr_info.name);
	}

	cxx.Disable();
	prj.Close();
}

	     

