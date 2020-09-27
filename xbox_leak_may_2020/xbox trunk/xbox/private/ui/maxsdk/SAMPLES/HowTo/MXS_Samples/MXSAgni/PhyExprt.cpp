/**********************************************************************
 *<
	FILE: PhyExprt.cpp

	DESCRIPTION: Physique Interface classes for MaxScript

	CREATED BY: Ravi Karra, 1998

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "MAXScrpt.h"
#include "Numbers.h"
#include "MAXObj.h"
#include "3DMath.h"
#include "LclClass.h"

#include "PhyExprt.h"

#include "defextfn.h"
#	include "ExtKeys.h"


Value*
get_phy_context_export_cf(Value** arg_list, int count)
{
	check_arg_count(GetPhyContextExport, 2, count);
	INode *node = arg_list[0]->to_node();
	Modifier *mod = arg_list[1]->to_modifier();

	if (mod->ClassID() != Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
		throw RuntimeError("modifier should be a Physique modifier, got:", mod->GetName());

	IPhysiqueExport *phyExport = (IPhysiqueExport *)mod->GetInterface(I_PHYINTERFACE);
	
	if (!phyExport) return &undefined;
	return new PhyContextExportValue(
		phyExport,
		(IPhyContextExport *)phyExport->GetContextInterface(node));
}


/* -------------------- PhyContextExportValue methods ----------------------- */

local_visible_class_instance (PhyContextExportValue, _T("PhyContextExport"))

PhyContextExportValue::PhyContextExportValue(IPhysiqueExport *phy, IPhyContextExport* mc)
{
	tag = class_tag(PhyContextExportValue);
	phyExport = phy;
	mcExport = mc;
}

void
PhyContextExportValue::gc_trace()
{
	phyExport->ReleaseContextInterface(mcExport);
	Value::gc_trace();
}

void
PhyContextExportValue::sprin1(CharStream* s)
{
	s->puts(_T("PhyContextExport"));
}

Value*
PhyContextExportValue::get_property(Value** arg_list, int count)
{
	Value* prop = arg_list[0];
	if (prop == n_numverts)
		return Integer::intern(mcExport->GetNumberVertices());
	return Value::get_property(arg_list, count);
}

Value*
PhyContextExportValue::set_property(Value** arg_list, int count)
{
	Value* prop = arg_list[0];
	if (prop == n_numverts)
		throw RuntimeError ("numVerts is a read only property");
	return Value::set_property(arg_list, count);
}

Value*
PhyContextExportValue::ConvertToRigid_vf(Value** arg_list, int count)
{
	check_arg_count(ConvertToRigid, 2, count + 1);
	mcExport->ConvertToRigid(arg_list[0]->to_bool());
	return arg_list[0];
}

Value*
PhyContextExportValue::AllowBlending_vf(Value** arg_list, int count)
{
	check_arg_count(AllowBlending, 2, count + 1);
	mcExport->AllowBlending(arg_list[0]->to_bool());
	return arg_list[0];
}

Value*
PhyContextExportValue::GetVertexInterface_vf(Value** arg_list, int count)
{
	check_arg_count(GetVertexInterface, 2, count + 1);
	
	Value *arg = arg_list[0];
	int	  index;
	
	if (!is_number(arg) || (index = arg->to_int()) < 1)
		throw RuntimeError ("vertex index must be +ve number, got: ", arg);
	
	if (index > mcExport->GetNumberVertices())
		throw RuntimeError ("vertex index cannot be > the number of vertices: ", arg);
	
	IPhyVertexExport* vi = mcExport->GetVertexInterface(index - 1 );
	
	if (!vi) return &undefined;

	if (vi->GetVertexType()&BLENDED_TYPE)
		return new PhyBlendedRigidVertexValue(mcExport, (IPhyBlendedRigidVertex*)vi);
	else
		return new PhyRigidVertexValue(mcExport, (IPhyRigidVertex*)vi);
}


/* -------------------- PhyRigidVertexValue methods ----------------------- */

local_visible_class_instance (PhyRigidVertexValue, _T("PhyRigidVertex"))

PhyRigidVertexValue::PhyRigidVertexValue(IPhyContextExport* mc, IPhyRigidVertex *vx)
{
	tag = class_tag(PhyRigidVertexValue);
	mcExport = mc;
	vtxExport = vx;
}

void
PhyRigidVertexValue::gc_trace()
{
	mcExport->ReleaseVertexInterface(vtxExport);
	Value::gc_trace();
}

void
PhyRigidVertexValue::sprin1(CharStream* s)
{
	s->puts(_T("PhyRigidVertex"));
}

Value*
PhyRigidVertexValue::GetNode_vf(Value** arg_list, int count)
{
	check_arg_count(GetNode, 1, count + 1);
	return new MAXNode(vtxExport->GetNode());
}

Value*
PhyRigidVertexValue::GetOffsetVector_vf(Value** arg_list, int count)
{
	check_arg_count(GetOffsetVector, 1, count + 1);
	return new Point3Value(vtxExport->GetOffsetVector());
}

/* -------------------- PhyBlendedRigidVertexValue methods ----------------------- */

local_visible_class_instance (PhyBlendedRigidVertexValue, _T("PhyBlendedRigidVertex"))

PhyBlendedRigidVertexValue::PhyBlendedRigidVertexValue(IPhyContextExport* mc, IPhyBlendedRigidVertex *vx)
{
	tag = class_tag(PhyBlendedRigidVertexValue);
	mcExport = mc;
	vtxExport = vx;
}

void
PhyBlendedRigidVertexValue::gc_trace()
{
	mcExport->ReleaseVertexInterface(vtxExport);
	Value::gc_trace();
}

void
PhyBlendedRigidVertexValue::sprin1(CharStream* s)
{
	s->puts(_T("PhyBlendedRigidVertex"));
}

Value*
PhyBlendedRigidVertexValue::get_property(Value** arg_list, int count)
{
	Value* prop = arg_list[0];
	if (prop == n_numNodes)
		return Integer::intern(vtxExport->GetNumberNodes());
	return Value::get_property(arg_list, count);
}

Value*
PhyBlendedRigidVertexValue::set_property(Value** arg_list, int count)
{
	Value* prop = arg_list[0];
	if (prop == n_numNodes)
		throw RuntimeError ("numNodes is a read only property");
	return Value::set_property(arg_list, count);
}

Value*
PhyBlendedRigidVertexValue::GetNode_vf(Value** arg_list, int count)
{
	check_arg_count(GetNode, 2, count + 1);
	Value *arg = arg_list[0];
	int	  index;
	
	if (!is_number(arg) || (index = arg->to_int()) < 1)
		throw RuntimeError ("node index must be +ve number, got: ", arg);
	
	if (index > vtxExport->GetNumberNodes())
		throw RuntimeError ("node index cannot be > the number of nodes: ", arg);

	return new MAXNode(vtxExport->GetNode(index-1));
}

Value*
PhyBlendedRigidVertexValue::GetOffsetVector_vf(Value** arg_list, int count)
{
	check_arg_count(GetOffsetVector, 2, count + 1);
	Value *arg = arg_list[0];
	int	  index;
	
	if (!is_number(arg) || (index = arg->to_int()) < 1)
		throw RuntimeError ("node index must be +ve number, got: ", arg);
	
	if (index > vtxExport->GetNumberNodes())
		throw RuntimeError ("node index cannot be > the number of nodes: ", arg);

	return new Point3Value(vtxExport->GetOffsetVector(index - 1));
}

Value*
PhyBlendedRigidVertexValue::GetWeight_vf(Value** arg_list, int count)
{
	check_arg_count(GetWeight, 2, count + 1);

	Value *arg = arg_list[0];
	int	  index;
	
	if (!is_number(arg) || (index = arg->to_int()) < 1)
		throw RuntimeError ("node index must be +ve number, got: ", arg);
	
	if (index > vtxExport->GetNumberNodes())
		throw RuntimeError ("node index cannot be > the number of nodes: ", arg);

	return new Point3Value(vtxExport->GetOffsetVector(index - 1));
}

