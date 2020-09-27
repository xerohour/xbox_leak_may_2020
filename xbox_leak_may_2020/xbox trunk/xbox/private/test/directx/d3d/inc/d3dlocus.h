#ifdef UNDER_XBOX
#define _USE_XGMATH
#define D3DCOMPILE_BEGINSTATEBLOCK 1
#endif // UNDER_XBOX
#ifndef UNDER_XBOX
#include <windows.h>
#include <winsock2.h>
#else
#include <xtl.h>
#include <winsockx.h>
#include <xnetref.h>
#endif // UNDER_XBOX
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <d3dx8.h>
#ifdef UNDER_XBOX
#include <xgraphics.h>
#endif // UNDER_XBOX
#include "modulex.h"
#include "..\frame\d3dlocus\cd3d.h"
#include "..\frame\d3dlocus\dtypes.h"
#include "..\frame\d3dlocus\misc.h"
#include "..\frame\d3dlocus\log.h"
#include "..\frame\d3dlocus\util.h"
#include "..\frame\d3dlocus\input.h"
#include "..\frame\d3dlocus\dxconio.h"
#include "..\frame\d3dlocus\enum.h"
#include "..\frame\d3dlocus\display.h"
#include "..\frame\d3dlocus\light.h"
#include "..\frame\d3dlocus\mesh.h"
#include "..\frame\d3dlocus\noise.h"
#include "..\frame\d3dlocus\palette.h"
#include "..\frame\d3dlocus\rsrcdata.h"
#include "..\frame\d3dlocus\scene.h"
#include "..\frame\d3dlocus\texture.h"
#include "..\frame\d3dlocus\vibuffer.h"
#include "..\frame\d3dlocus\vpshader.h"
