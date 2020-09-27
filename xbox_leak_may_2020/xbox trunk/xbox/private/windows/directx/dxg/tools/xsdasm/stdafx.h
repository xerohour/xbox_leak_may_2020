// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>

#include <windows.h>
#include "d3d8-xbox.h"
#include "../../../private/inc/xboxverp.h"

#include "xgraphics.h"

#define RELEASE(x) \
    do { if(x) { x->Release(); x = NULL; } } while(0)
