///////////////////////////////////////////////////////////////////////////////
//	IVUTIL.CPP
//
//	Created by :			Date :
//		MarcI					9/2/96
//
//	Description :
//		Implementation of IV Sniff utilities
//

#include "stdafx.h"



void	InitIV (void)
{
	COWorkSpace		theWorkSpace;

	if(!theWorkSpace.IsDockWindowVisible(IDDW_IVRESULTS)) {
		theWorkSpace.ShowToolbar(IDDW_IVRESULTS,TRUE);
	}
	ASSERT(theWorkSpace.IsDockWindowVisible(IDDW_IVRESULTS));


}