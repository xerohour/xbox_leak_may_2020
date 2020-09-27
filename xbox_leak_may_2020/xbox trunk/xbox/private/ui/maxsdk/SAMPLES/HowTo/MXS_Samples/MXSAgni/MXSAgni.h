/**********************************************************************
 *<
	FILE: MXSAgni.h

	DESCRIPTION: Main header file for MXSAgni.dlx

	CREATED BY: Ravi Karra, 1998

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/


#ifndef __MXSAGNI__H__
#define __MXSAGNI__H__


#include "MAXScrpt.h"
#include "Numbers.h"
#include "MAXclses.h"
#include "Streams.h"
#include "MSTime.h"
#include "MAXObj.h"
#include "Parser.h"

#define redraw_views()														\
	MAXScript_interface->RedrawViews(MAXScript_interface->GetTime())

#define get_valid_node(_node, _fn)											\
	MAXNode* nv = _node;													\
	if (is_node(nv))														\
	{																		\
		if(nv->ref_deleted)													\
			throw RuntimeError("Attempted to access to deleted object");	\
	}																		\
	else																	\
	{																		\
		throw RuntimeError (#_fn##" requires a node");						\
	}																		\
	INode* node = nv->to_node()

#endif //__MXSAGNI__H__

