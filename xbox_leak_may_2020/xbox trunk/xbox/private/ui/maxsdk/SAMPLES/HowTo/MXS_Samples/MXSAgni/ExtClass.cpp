/**********************************************************************
 *<
	FILE: ExtClass.cpp

	DESCRIPTION: All the extension classes in MXSAgni.dlx

	CREATED BY: Ravi Karra, 1998

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "MAXScrpt.h"
#include "Numbers.h"
#include "MAXObj.h"
#include "Strings.h"
#include "LclClass.h"
#include "ExtClass.h"

#include "lclinsfn.h"
#	include "ExtKeys.h"

/* --------------------  local generics and names   --------------- */
#include "lclinsfn.h"
#	include "bmatpro.h"
#	include "phyblpro.h"
#	include "phymcpro.h"
//#	include "phyrgpro.h"

define_local_generic_class

local_visible_class_instance (AgniRootClassValue, _T("AgniRootClass"))

/* ------------------ BigMatrixValue indexing functions -------------------- */

Value*
BigMatrixValue::get_vf(Value** arg_list, int count)
{
	Value *arg, *result;
	int	  index;
	check_arg_count(get, 2, count + 1);
	arg = arg_list[0];
	if (!is_number(arg) || (index = arg->to_int()) < 1)
		throw TypeError ("array index must be +ve number, got: ", arg);

	if (index > bm.m)
		result = &undefined;
	else
	{
		rowArray->row = index - 1;
		result = rowArray;  
	}

	return result;
}

Value*
BigMatrixValue::put_vf(Value** arg_list, int count)
{
	throw RuntimeError (_T("Cannot directly set this value yet"));
	return arg_list[0];
}

local_visible_class_instance (BigMatrixRowArray, _T("BigMatrixRowArray"))

BigMatrixRowArray::BigMatrixRowArray(BigMatrixValue* bmv) 
{
	tag = class_tag(BigMatrixRowArray);
	this->bmv = bmv;	
}

void
BigMatrixRowArray::sprin1(CharStream* s)
{
	s->puts(_T("#("));
	for (int i = 0; i < bmv->bm.n; i++)
	{
		s->printf(_T("%.2f"), bmv->bm[row][i]);
		if (i < bmv->bm.n - 1)
			s->puts(_T(", "));
	}
	s->puts(_T(")"));	
}

Value*
BigMatrixRowArray::get_vf(Value** arg_list, int count)
{
	Value *arg, *result;
	int	  index;
	check_arg_count(get, 2, count + 1);
	arg = arg_list[0];
	if (!is_number(arg) || (index = arg->to_int()) < 1)
		throw TypeError ("array index must be +ve number, got: ", arg);

	if (index > bmv->bm.n)
		result = &undefined;
	else
		result = Float::intern(bmv->bm[row][index - 1]);  // array indexes are 1-based !!!

	return result;
}

Value*
BigMatrixRowArray::put_vf(Value** arg_list, int count)
{
	Value *arg;
	int	  index;

	check_arg_count(put, 3, count + 1);
	arg = arg_list[0];
	if (!is_number(arg) || (index = arg->to_int()) < 1)
		throw RuntimeError ("array index must be +ve number, got: ", arg);
	if (index > bmv->bm.n)
		throw RuntimeError ("array index must be < the number of columns: ", arg);
	bmv->bm[row][index - 1] = arg_list[1]->to_float();

	return arg_list[1];
}

void
BigMatrixRowArray::gc_trace()
{
	/* trace sub-objects & mark me */
	Value::gc_trace();
	if (bmv != NULL && bmv->is_not_marked())
		bmv->gc_trace();
}

/* ------------------- BigMatrixValue class instance -------------- */

// Instances of applyable classes can be created and returned by other methods or classes 
// in MaxScript and also by the user like bm = BigMatrix()

local_visible_class_instance (BigMatrixValue, _T("BigMatrix"))

Value*
BigMatrixValueClass::apply(Value** arg_list, int count)
{
	two_value_locals(a0, a1);
	Value* result;
	
	if (count == 2)
	{
		vl.a0 = arg_list[0]->eval();
		vl.a1 = arg_list[1]->eval();
		result = new BigMatrixValue (vl.a0->to_int(), vl.a1->to_int());
	}
	else
		check_arg_count(BigMatrix, 2, count);
	pop_value_locals();

	return result;
}

/* -------------------- BigMatrixValue methods ----------------------- */
BigMatrixValue::BigMatrixValue(const BigMatrix& from)
{
	tag = class_tag(BigMatrixValue);
	bm = from;
	rowArray = new BigMatrixRowArray(this);
}

BigMatrixValue::BigMatrixValue(int mm, int nn)
{
	tag = class_tag(BigMatrixValue);
	bm.SetSize(mm, nn);
	for (int m=0; m < mm; m++)
		for (int n=0; n < nn; n++)
			bm[m][n] = 0.0f;
	rowArray = new BigMatrixRowArray(this);
}

void
BigMatrixValue::gc_trace()
{
        /* trace sub-objects & mark me */
		Value::gc_trace();
		if (rowArray != NULL && rowArray->is_not_marked())
			rowArray->gc_trace();
}

void
BigMatrixValue::sprin1(CharStream* s)
{
	s->puts(_T("#BigMatrix(\n"));
	for (int m = 0; m < bm.m; m++)
	{
		s->puts(_T("\t["));
		for (int n = 0; n < bm.n; n++)
		{
			s->printf(_T("%.2f"), bm[m][n]);
			if (n < bm.n-1) 
				s->puts(",");
		}
		s->puts(_T("]"));
		if (m < bm.m-1) 
			s->puts(_T(","));
		s->puts(_T("\n"));
	}
	s->puts(_T(")\n"));
}

Value*
BigMatrixValue::get_property(Value** arg_list, int count)
{
	Value* prop = arg_list[0];
	if (prop == n_rows)
		return Integer::intern(bm.m);
	else if (prop == n_columns)
		return Integer::intern(bm.n);
	return Value::get_property(arg_list, count);
}

Value*
BigMatrixValue::set_property(Value** arg_list, int count)
{
	Value* prop = arg_list[1];	
	if (prop == n_rows)
		throw RuntimeError ("Can't directly set the row size of a BigMatrix");
	else if (prop == n_columns)
		throw RuntimeError ("Cannot directly set the column size of a BigMatrix");
	return Value::set_property(arg_list, count);
}

Value*
BigMatrixValue::plus_vf(Value** arg_list, int count)
{
	if (!is_bigmatrix(arg_list[0]))
		throw RuntimeError (_T("the right hand operand should be a BigMatrix"));
	BigMatrix& bmat = ((BigMatrixValue*)arg_list[0])->to_bigmatrix();
	if (bmat.m != bm.m || bmat.n != bm.n)
		throw RuntimeError (_T("The BigMatrix should have equal number of rows & columns"));
	
	one_value_local(result);
	vl.result = new BigMatrixValue(bm.m, bm.n);
		for (int m = 0; m < bm.m; m++)
			for (int n = 0; n < bm.n; n++)
				((BigMatrixValue*)vl.result)->bm[m][n] = bm[m][n] + bmat[m][n];
	return_value(vl.result);
}

#define Matrix3Value BigMatrixValue
#	include "lclimpfn.h"	
	def_mut_mat_fn( invert,			1, bm.Invert())
	def_mut_mat_fn( identity,		1, bm.Identity())
	def_mut_mat_fn( clear,			1, bm.Clear())
	def_mut_mat_fn( setTranspose,	2, bm.SetTranspose(((BigMatrixValue*)arg_list[0])->to_bigmatrix()))
	def_mut_mat_fn( setSize,		3, bm.SetSize(arg_list[0]->to_int(), arg_list[1]->to_int()))
#undef Matrix3Value

/* -------------------- TrackViewPickValue methods ----------------------- */
TrackViewPickValue::TrackViewPickValue(TrackViewPick pick)
{
	track = pick;
	name = track.client->SubAnimName(track.subNum);
}

void
TrackViewPickValue::sprin1(CharStream* s)
{	
	s->printf(_T("TrackViewPick:\"%s\""), name);
}

void
TrackViewPickValue::gc_trace()
{
	Value::gc_trace();
}

Value*
TrackViewPickValue::get_property(Value** arg_list, int count)
{
	Value* prop = arg_list[0];
	if (prop == n_name) return new String(name);
	else if (prop == n_anim) 
		return MAXClass::make_wrapper_for(track.anim);
	else if (prop == n_client) 
		return MAXClass::make_wrapper_for(track.client);
	else if (prop == n_subNum)
		return Integer::intern(track.subNum+1);
	return Value::get_property(arg_list, count);
}

Value*
TrackViewPickValue::set_property(Value** arg_list, int count)
{
	Value* val = arg_list[0];
	Value* prop = arg_list[1];
	if (prop == n_name || prop == n_anim || prop == n_client || prop == n_subNum)
		throw RuntimeError (GetString(IDS_READ_ONLY_PROPERTY), prop);	
	return Value::set_property(arg_list, count);
}

// Here's how you can define a struct primitive
// Can be called like trackView.pickTrackDlg() in MaxScript
def_struct_primitive( tvw_pickTrackDlg,		trackView,	"pickTrackDlg"); 

Value* 
tvw_pickTrackDlg_cf(Value** arg_list, int count)
{
	// trackView.pickTrackDlg [#mutliple]
	BOOL mult = FALSE;
	if (count == 1)
		if (arg_list[0] == n_multiple)
			mult = TRUE;
		else
			throw RuntimeError (_T("trackViewPickDlg: Unknown argument - "), arg_list[0]);
	if (mult)
	{
		Tab<TrackViewPick> picks;
		if(!MAXScript_interface->TrackViewPickMultiDlg(MAXScript_interface->GetMAXHWnd(), &picks))
			return &undefined;
		one_typed_value_local(Array* result); 
		vl.result = new Array(picks.Count());
		for(int n = 0; n < picks.Count(); n++)
			vl.result->append(new TrackViewPickValue(picks[n])); 
		return_value(vl.result);		
	}
	else
	{
		TrackViewPick pick;
		if(MAXScript_interface->TrackViewPickDlg(MAXScript_interface->GetMAXHWnd(), &pick))
			return new TrackViewPickValue(pick);
		return &undefined;
	}
}
