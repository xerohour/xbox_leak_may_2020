/**********************************************************************
 *<
	FILE: ExtClass.h

	DESCRIPTION: All the extension classes in MXSAgni.dlx

	CREATED BY: Ravi Karra, 1998

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef _H_EXT_CLASS
#define _H_EXT_CLASS

#include "Funcs.h"
#include "MNBigMat.h"
#include "PhyExp.h"
#include "notetrck.h"
#include "resource.h"

declare_local_generic_class

// base class for all classes declared in MxsAgni
local_visible_class (AgniRootClassValue)
class AgniRootClassValue : public Value
{
public:
						AgniRootClassValue() { }

	ValueMetaClass*		local_base_class() { return class_tag(AgniRootClassValue); }			
						classof_methods (AgniRootClassValue, Value);
	void				collect() { delete this; }
	void				sprin1(CharStream* s) { s->puts(_T("AgniRootClassValue\n")); }
	void				gc_trace() { Value::gc_trace(); }
#	define				is_agniroot(p) ((p)->tag == class_tag(AgniRootClassValue))

#include "lclabsfn.h"
#	include "bmatpro.h"
#	include "phyblpro.h"
#	include "phymcpro.h"
//#	include "phyrgpro.h"
};

class BigMatrixValue;

// Instances of visible classes can only created and returned by other methods 
// or classes in MaxScript but cannot be created by the user. 
local_visible_class (BigMatrixRowArray)

class BigMatrixRowArray : public Value
{
public:
	BigMatrixValue	*bmv;
	int				row;
					BigMatrixRowArray(BigMatrixValue* bmv);
	void			collect() { delete this; }
	void			sprin1(CharStream* s);
	void			gc_trace();
	
#include "defimpfn.h"
	def_generic	(get,		"get");
	def_generic	(put,		"put");
};

// Instances of applyable classes can be created and returned by other methods or classes 
// in MaxScript and also by the user like bm = BigMatrix()

local_applyable_class (BigMatrixValue)

class BigMatrixValue : public AgniRootClassValue
{
public:
	BigMatrix			bm;
	BigMatrixRowArray	*rowArray;

						BigMatrixValue(int mm, int nn);
						BigMatrixValue(const BigMatrix& from);

//	ValueMetaClass*		local_base_class() { return class_tag(BigMatrixValue); }			
						classof_methods (BigMatrixValue, Value);
	
	void				collect() { delete this; }
	void				sprin1(CharStream* s);
	void				gc_trace();
#	define				is_bigmatrix(p) ((p)->tag == class_tag(BigMatrixValue))
	Value*				get_property(Value** arg_list, int count);
	Value*				set_property(Value** arg_list, int count);
	BigMatrix&			to_bigmatrix() { return bm; }

	/* operations */

#include "lclimpfn.h"
#	include "bmatpro.h"
	
};

local_visible_class (TrackViewPickValue)

class TrackViewPickValue : public Value
{

public:
	TrackViewPick		track;
	TSTR				name;
						TrackViewPickValue(TrackViewPick pick);
	void				collect() { delete this; }
	void				sprin1(CharStream* s);
	void				gc_trace();
#	define				is_trackviewpick(p) ((p)->tag == class_tag(TrackViewPick))
	Value*				get_property(Value** arg_list, int count);
	Value*				set_property(Value** arg_list, int count);	
};
#endif //_H_EXT_CLASS
