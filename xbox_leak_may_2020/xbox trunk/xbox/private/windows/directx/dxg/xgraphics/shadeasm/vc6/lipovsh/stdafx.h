#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#ifdef XBOX
#include <xtl.h>
#else
#include <windows.h>
#include "d3d8-xbox.h"
#endif

#include "xgraphics.h"

#define RELEASE(x) \
    do { if(x) { x->Release(); x = NULL; } } while(0)


