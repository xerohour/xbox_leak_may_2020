#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <xboxdbg.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
//#include <waveldr.h>
#include <float.h>
#include <dmusicc.h>
#include <dmusici.h>
#include "..\trans\macros.h"
#include "trans.h"
#include "util.h"
#include <typeinfo.h>

#define RUN( func )                                              \
{                                                                    \
	hr = func;                                                       \
}                                                                    


#define CHECKRUN( exp )                                                 \
{                                                                   \
    if ( SUCCEEDED( hr ) )                                               \
    {                                                                    \
	    RUN(exp);                                                       \
    }                                               \
}                                                   


LPSTR  PathFindFileNameA(LPSTR pszPath);
#define PathFindFileName  PathFindFileNameA



