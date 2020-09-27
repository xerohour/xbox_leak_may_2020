///////////////////////////////////////////////////////////////////////////////
//	QCQP.H
//
//	Created by :			Date :
//		DavidGa					10/13/93
//
//	Description :
//		IDs for dockable windows, not found in dockids.h
//

#ifndef __QCQP_H__
#define __QCQP_H__

#include "dockids.h"

#ifndef IDDW_VCPP_BASE
	#error include 'dockids.h' before including this file
#endif

// the following are ids for the dockable windows
// I've munged this together from information in
// qcqp.c and qcqp.h (BrianCr)

#define IDW_OUTPUT_WIN	(IDDW_VCPP_BASE + 0)
#define IDW_WATCH_WIN	(IDDW_VCPP_BASE + 1)
#define IDW_LOCALS_WIN	(IDDW_VCPP_BASE + 2)
#define IDW_CPU_WIN		(IDDW_VCPP_BASE + 3)
#define IDW_MEMORY_WIN	(IDDW_VCPP_BASE + 4)
#define IDW_CALLS_WIN	(IDDW_VCPP_BASE + 5)
#define IDW_BROWSE_WIN	(IDDW_VCPP_BASE + 6)
#define IDW_DISASSY_WIN	(IDDW_VCPP_BASE + 7)
#define IDW_BREAKPT_WIN	(IDDW_VCPP_BASE + 8)

#endif //__QCQP_H__
