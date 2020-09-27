
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef BIG_ENDIAN
#define BIG_ENDIAN 0
#endif
#include "coreaddr.h" 
#include "maout.h"

#define False	0
#define True	1

// ************************************************************
// IsMemoryP		
// ************************************************************ 
	int
IsMemoryP(int id)
{
return 
   (memory_map_p  == id ||
    memory_map_pa == id ||
    memory_map_pb == id ||
    memory_map_pe == id ||
    memory_map_pi == id ||
    memory_map_pr == id);
}	// ---- end IsMemoryP() ----

// ************************************************************
// IsMemoryX		
// ************************************************************ 
	int
IsMemoryX(int id)
{
return 
   (memory_map_x  == id ||
    memory_map_xa == id ||
    memory_map_xb == id ||
    memory_map_xe == id ||
    memory_map_xi == id ||
    memory_map_xr == id);
}	// ---- end IsMemoryX() ----

// ************************************************************
// IsMemoryY		
// ************************************************************ 
	int
IsMemoryY(int id)
{
return 
   (memory_map_y  == id ||
    memory_map_ya == id ||
    memory_map_yb == id ||
    memory_map_ye == id ||
    memory_map_yi == id ||
    memory_map_yr == id);
}	// ---- end IsMemoryY() ----

// ************************************************************
// TranslateMemoryMapID		
// ************************************************************ 
	char *
TranslateMemoryMapID(int id)
{
switch (id)
	{
	case memory_map_p:
	return ("memory_map_p");
	case memory_map_x:
	return ("memory_map_x");
	case memory_map_y:
	return ("memory_map_y");
	case memory_map_l:
	return ("memory_map_l");
	case memory_map_none:
	return ("memory_map_none");

	case memory_map_laa:
	return ("memory_map_laa");
	case memory_map_lab:
	return ("memory_map_lab");
	case memory_map_lba:
	return ("memory_map_lba");
	case memory_map_lbb:
	return ("memory_map_lbb");
	case memory_map_le:
	return ("memory_map_le");
	case memory_map_li:
	return ("memory_map_li");

	case memory_map_pa:
	return ("memory_map_pa");
	case memory_map_pb:
	return ("memory_map_pb");
	case memory_map_pe:
	return ("memory_map_pe");
	case memory_map_pi:
	return ("memory_map_pi");
	case memory_map_pr:
	return ("memory_map_pr");
		
	case memory_map_xa:
	return ("memory_map_xa");
	case memory_map_xb:
	return ("memory_map_xb");
	case memory_map_xe:
	return ("memory_map_xe");
	case memory_map_xi:
	return ("memory_map_xi");
	case memory_map_xr:
	return ("memory_map_xr");
	
	case memory_map_ya:
	return ("memory_map_ya");
	case memory_map_yb:
	return ("memory_map_yb");
	case memory_map_ye:
	return ("memory_map_ye");
	case memory_map_yi:
	return ("memory_map_yi");
	case memory_map_yr:
	return ("memory_map_yr");

	case memory_map_pt:
	return ("memory_map_pt");
	case memory_map_pf:
	return ("memory_map_pf");

	case memory_map_emi:
	return ("memory_map_emi");
	case memory_map_dm:
	return ("memory_map_dm");

	case memory_map_p8:
	return ("memory_map_p8");
	case memory_map_u:
	return ("memory_map_u");
	case memory_map_u8:
	return ("memory_map_u8");
	case memory_map_u16:
	return ("memory_map_u16");

	case memory_map_xs:
	return ("memory_map_xs");
	case memory_map_ys:
	return ("memory_map_ys");
	case memory_map_maxp1:
	return ("memory_map_maxp1");

	case memory_map_error:
	return ("memory_map_error");
	}

if (id >= memory_map_e0 && id <= memory_map_e255)
	{
// Hey static memory is bad !!
	static char s[20];
	sprintf(s, "memory_map_e%d\n", id - memory_map_e0);
	return (s);
	}
return ("Dunno");
}	// ---- end TranslateMemoryMapID() ----
