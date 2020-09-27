/*===========================================================================*\
 | 
 |  FILE:	Plugin.cpp
 |			MAX Script SDK Example - adding a new function to the language
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




/*===========================================================================*\
 |	Define our new functions
 |	These will turn on texture map display at all levels for a given object
\*===========================================================================*/

def_visible_primitive( all_tex_on,			"TurnAllTexturesOn");
def_visible_primitive( all_tex_off,			"TurnAllTexturesOff");




/*===========================================================================*\
 |	This is used below, as our recursive function to turn on texture maps
\*===========================================================================*/

class CMtlEnum 
{
	public:

		virtual void  proc(MtlBase *m, BOOL state, Interface *ip)
		{
			// Check for stdmaterial, get the diffuse map and turn it on/off
			if(m->ClassID()==Class_ID(DMTL_CLASS_ID, 0)) 
			{
				StdMat *smat = (StdMat*)m;
				Texmap *bt = smat->GetSubTexmap(ID_DI);

				if(state) ip->ActivateTexture(bt, smat); 
				else ip->DeActivateTexture(bt, smat);
			}
		}
};

// Static instance of our recursive function
CMtlEnum CEnym;


// Do the material enumeration
void CEnumMtlTree(MtlBase *mb, CMtlEnum &tenum, BOOL state, Interface *ip) 
{
	// Call the recursion
	tenum.proc(mb,state,ip);
	if (IsMtl(mb)) 
	{
		// Go through all the sub materials
		Mtl *m = (Mtl *)mb;
		for (int i=0; i<m->NumSubMtls(); i++) 
		{
			Mtl *sm = m->GetSubMtl(i);
			if (sm) 
				CEnumMtlTree(sm,tenum,state,ip);
		}
	}
}




/*===========================================================================*\
 |	Implimentations of our new function calls
 |	notice the appended '_cf' to show its the function implimentation
\*===========================================================================*/

Value*
all_tex_on_cf(Value** arg_list, int count)
{
	// Make sure we have the correct number of arguments (2)
	check_arg_count(all_tex_on, 1, count);

	// Check to see if the arguments match up to what we expect
	// We want to use 'TurnAllTexturesOn <object to use>'
	type_check(arg_list[0], MAXNode, "TurnAllTexturesOn [Object]");

	// Get a good interface pointer
	Interface *ip = MAXScript_interface;

	// Get a INode pointer from the argument passed to us
	INode *node = arg_list[0]->to_node();

	// Recurse and turn all textures on
	MtlBase* mat = (MtlBase*)node->GetMtl();
	if (mat)
		CEnumMtlTree(mat,CEnym,TRUE,ip);

	// redraw and update
	node->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE); 
	ip->RedrawViews(ip->GetTime());

	return &true_value;
}


// For code notes, see above
Value*
all_tex_off_cf(Value** arg_list, int count)
{
	check_arg_count(all_tex_on, 1, count);
	type_check(arg_list[0], MAXNode, "TurnAllTexturesOff [Object]");

	Interface *ip = MAXScript_interface;
	INode *node = arg_list[0]->to_node();

	// The only difference is the FALSE state flag
	MtlBase* mat = (MtlBase*)node->GetMtl();
	if (mat)
		CEnumMtlTree(mat,CEnym,FALSE,ip);

	node->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE); 
	ip->RedrawViews(ip->GetTime());

	return &true_value;
}


/*===========================================================================*\
 |	MAXScript Plugin Initialization
\*===========================================================================*/

__declspec( dllexport ) void
LibInit() { 
}

