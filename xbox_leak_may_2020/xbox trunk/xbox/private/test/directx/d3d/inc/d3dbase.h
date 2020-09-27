#ifdef UNDER_XBOX
#define _USE_XGMATH
#endif // UNDER_XBOX
#ifndef UNDER_XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif // UNDER_XBOX
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <d3dx8.h>
#ifdef UNDER_XBOX
#include <xgraphics.h>
#endif // UNDER_XBOX
#include "..\frame\d3dbase\object.h"
#include "..\frame\d3dbase\dtypes.h"
#include "..\frame\d3dbase\misc.h"
#include "..\frame\d3dbase\util.h"
#include "..\frame\d3dbase\input.h"
#include "..\frame\d3dbase\dxconio.h"
#include "..\frame\d3dbase\enum.h"
#include "..\frame\d3dbase\display.h"
#include "..\frame\d3dbase\light.h"
#include "..\frame\d3dbase\mesh.h"
#include "..\frame\d3dbase\palette.h"
#include "..\frame\d3dbase\noise.h"
#include "..\frame\d3dbase\rsrcdata.h"
#include "..\frame\d3dbase\scene.h"
#include "..\frame\d3dbase\texture.h"
#include "..\frame\d3dbase\vibuffer.h"
#include "..\frame\d3dbase\vpshader.h"

#ifdef UNDER_XBOX
#define DEMO_HACK
#endif

#ifdef DEMO_HACK
#define D3DFMT_D24X4S4          ((D3DFORMAT)0xFFFF0002)
#define D3DFMT_D15S1            ((D3DFORMAT)0xFFFF0003)
#define D3DFMT_D32              ((D3DFORMAT)0xFFFF0004)
#define D3DFMT_R3G3B2           ((D3DFORMAT)0xFFFF0005)
#define D3DFMT_A4L4             ((D3DFORMAT)0xFFFF0006)
#define D3DFMT_A8R3G3B2         ((D3DFORMAT)0xFFFF0007)
#define D3DFMT_X4R4G4B4         ((D3DFORMAT)0xFFFF0008)
#define D3DFMT_A8P8             ((D3DFORMAT)0xFFFF0009)
#define D3DFMT_R8G8B8           ((D3DFORMAT)0xFFFF000A)
#define D3DFMT_W11V11U10        ((D3DFORMAT)0xFFFF000B)
#endif
