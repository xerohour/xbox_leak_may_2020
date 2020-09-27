/**************************************************************************

                   NOTIFICATIONS FROM OSDEBUG

                   mapping id <==> notification


**************************************************************************/

#ifndef __MSVC_DEBUG__	// {
#define __MSVC_DEBUG__

	#define dbcBase       dbcNil

#define MAX_OSD_LEN       20

	#undef DECL_DBC
	#define DECL_DBC(name, fRequest, dbct)	"dbc" #name,

char far dbcStr[][MAX_OSD_LEN] = {
	#include <dbc.h>
    } ;

#undef DECL_DBC

#endif	// __MSVC_DEBUG__ }
