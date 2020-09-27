/*===========================================================================*\
 | 
 |  FILE:	Plugin.cpp
 |			MAX Script SDK Example - adding a value class to MAX Script
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 1-4-99
 | 
\*===========================================================================*/

#include "MAXScrpt.h"

// Various MAX and MXS includes
#include "Numbers.h"
#include "MAXclses.h"
#include "Streams.h"
#include "MSTime.h"
#include "MAXObj.h"
#include "Parser.h"

#include "max.h"
#include "stdmat.h"

// define the new primitives using macros from SDK
#include "definsfn.h"
#include "defimpfn.h"	


// Redefine so we can export our custom class
#ifdef ScripterExport
	#undef ScripterExport
#endif
#define ScripterExport __declspec( dllexport )



/*===========================================================================*\
 |	A very simple new class - just mimicks the Point3 storage class
 |  The type of class built is entirely subjective and up to the developer
\*===========================================================================*/

applyable_class (SimpleStore)

class SimpleStore : public Value
{

public:
	Point3 iValue;

	// The various ways to contruct this class
	ScripterExport	SimpleStore();
	ScripterExport	SimpleStore(float x,float y,float z);

	classof_methods(SimpleStore, Value);

	// Garbage collection
	void			collect() { delete this; }

	// Print out our contents in readable form
	ScripterExport	void	sprin1(CharStream* s);	

	// Make a new instance of this class, based on some parameters
	static Value*	make(Value**arg_list, int count);

	// Define some operators
	use_generic( eq,		"=");
	use_generic( ne,		"!=");

	// Define some functions (is zeroify a word?)
	def_visible_generic( zeroify,	"zeroify");			

	// Define some properties
	def_property ( x );
	def_property ( y );
	def_property ( z );


	// Cast back to MAXSDK value
	Point3&		to_max_point3() { return iValue; }
};


visible_class_instance (SimpleStore, _T("SimpleStore"))



/*===========================================================================*\
 |	Implimentations of any class functions
\*===========================================================================*/

Value*														
SimpleStore::zeroify_vf(Value** arg_list, int count)			
{															
	check_arg_count(isEmpty, 1, count + 1);	
	iValue = Point3(0,0,0);

	return &true_value;
}



/*===========================================================================*\
 |	Class-level constructors
\*===========================================================================*/

SimpleStore::SimpleStore()
{
	tag = class_tag(SimpleStore);	
}

SimpleStore::SimpleStore(float x,float y,float z)
{
	tag = class_tag(SimpleStore);
	iValue = Point3(x,y,z);
}



/*===========================================================================*\
 |	Accessor macros - 4 values needed:
 |	name of class to use | name of property | code to access our variable | code to set our variable
\*===========================================================================*/

def_fn_prop_accessors (SimpleStore, x, 
					   Float::intern(iValue.x), 
					   iValue.x = (val->to_float()))

def_fn_prop_accessors (SimpleStore, y, 
					   Float::intern(iValue.y), 
					   iValue.y = (val->to_float()))

def_fn_prop_accessors (SimpleStore, z, 
					   Float::intern(iValue.z), 
					   iValue.z = (val->to_float()))



/*===========================================================================*\
 |	Fill in the == and != operators
 |	Give the macro two values - our internal value, and a cast for the incoming Value
\*===========================================================================*/

def_eq_op(SimpleStore, to_point3, iValue)
def_ne_op(SimpleStore, to_point3, iValue)



/*===========================================================================*\
 |	Two important calls:
 |	'make' asks us to construct a basic new instance of the class
 |	'apply' is used to iterate variables, Eval()ing them as we go (notice different class)
\*===========================================================================*/

Value*
SimpleStore::make(Value**arg_list, int count)
{
	check_arg_count(make, 3, count);

	float x = arg_list[0]->to_float(), y = arg_list[1]->to_float(), z = arg_list[2]->to_float();
	return new SimpleStore (x, y, z);
}

Value*
SimpleStoreClass::apply(Value** arg_list, int count)
{
	init_thread_locals();
	three_value_locals(x1, y1, z1);
	Value* result;
	
	if (count == 3)
	{
		vl.x1 = arg_list[0]->eval();
		vl.y1 = arg_list[1]->eval();
		vl.z1 = arg_list[2]->eval();
		result =  new SimpleStore((vl.x1->to_float()), (vl.y1->to_float()), (vl.z1->to_float()));
	}
	else
		check_arg_count(SimpleStore, 3, count);
	pop_value_locals();	

	return result;
}


/*===========================================================================*\
 |	Print out the contents of the class
\*===========================================================================*/

void
SimpleStore::sprin1(CharStream* s)
{
	s->printf(_T("[X:%.2f  Y:%.2f  Z:%.2f]"), iValue.x, iValue.y, iValue.z);
}


/*===========================================================================*\
 |	MAXScript Plugin Initialization
\*===========================================================================*/

__declspec( dllexport ) void
LibInit() { 
}
