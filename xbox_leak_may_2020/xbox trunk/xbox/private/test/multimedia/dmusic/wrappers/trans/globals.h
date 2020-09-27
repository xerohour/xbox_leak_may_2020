#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
//#include <waveldr.h>
#include <float.h>
#include <dmusicc.h>
#include <dmusici.h>
#include <macros.h>

#define RUN( func )                                              \
{                                                                    \
	hr = func;                                                       \
}                                                                    


#define CHECKRUN( exp )                                                 \
if ( SUCCEEDED( hr ) )                                               \
{                                                                    \
	RUN(exp);                                                       \
}

