/**********************************************************************
 *<
	FILE: PhyExprt.h

	DESCRIPTION: Physique Interface classes

	CREATED BY: Ravi Karra, 1998

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef _H_PHY_EXPRT
#define _H_PHY_EXPRT

#include "ExtClass.h"
#include "PhyExp.h"

class PhyRigidVertexValue;
class PhyBlendingRigidVertexValue;

local_visible_class (PhyContextExportValue)

class PhyContextExportValue : public AgniRootClassValue
{
public:
	IPhysiqueExport		*phyExport;
	IPhyContextExport	*mcExport;

						PhyContextExportValue(IPhysiqueExport *phy, IPhyContextExport* mc);
						classof_methods (PhyContextExportValue, Value);

	void				collect() { delete this; }
	void				sprin1(CharStream* s);
	void				gc_trace();
	Value*				get_property(Value** arg_list, int count);
	Value*				set_property(Value** arg_list, int count);
	IPhyContextExport*	to_phycontextexport() { return mcExport; }

#include "lclimpfn.h"
#	include "phymcpro.h"

};

local_visible_class (PhyRigidVertexValue)

class PhyRigidVertexValue : public AgniRootClassValue
{
public:
	IPhyContextExport	*mcExport;
	IPhyRigidVertex		*vtxExport;

						PhyRigidVertexValue(IPhyContextExport *mc, IPhyRigidVertex *vx);
						classof_methods (PhyRigidVertexValue, Value);

	void				collect() { delete this; }
	void				sprin1(CharStream* s);
	void				gc_trace();
	IPhyRigidVertex*	to_phyrigidvertex() { return vtxExport; }

#include "lclimpfn.h"
#	include "phyrgpro.h"

};

local_visible_class (PhyBlendedRigidVertexValue )

class PhyBlendedRigidVertexValue : public AgniRootClassValue
{
public:
	IPhyContextExport		*mcExport;
	IPhyBlendedRigidVertex	*vtxExport;

							PhyBlendedRigidVertexValue(IPhyContextExport *mc, IPhyBlendedRigidVertex *vx);
							classof_methods (PhyBlendedRigidVertexValue, Value);

	void					collect() { delete this; }
	void					sprin1(CharStream* s);
	void					gc_trace();
	Value*					get_property(Value** arg_list, int count);
	Value*					set_property(Value** arg_list, int count);
	IPhyBlendedRigidVertex*	to_phyblendedrigidvertex() { return vtxExport; }

#include "lclimpfn.h"
#	include "phyblpro.h"

};
#endif //_H_PHY_EXPRT