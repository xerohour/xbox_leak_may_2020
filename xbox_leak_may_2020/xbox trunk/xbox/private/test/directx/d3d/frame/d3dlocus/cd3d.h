#if defined(_DEBUG) || defined(DBG)
#ifndef DEBUG
#define DEBUG
#endif
#endif

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

#ifndef UNDER_XBOX
#define POOL_DEFAULT    D3DPOOL_MANAGED
#else
#define POOL_DEFAULT    D3DPOOL_DEFAULT
#endif

class CObject;
class CDirect3D8;
class CDevice8;
class CSurface8;
class CVolume8;
class CSwapChain8;
class CResource8;
class CVertexBuffer8;
class CIndexBuffer8;
class CPalette8;
class CBaseTexture8;
class CTexture8;
class CCubeTexture8;
class CVolumeTexture8;

#include "util.h"
#include "transprt.h"
#include "client.h"
#include "typetran.h"
#include "object.h"
#include "direct3d.h"
#include "device.h"
#include "surface.h"
#include "volume.h"
#include "swapchn.h"
#include "resource.h"
#include "vertbuff.h"
#include "indxbuff.h"
#include "pal.h"
#include "basetex.h"
#include "tex.h"
#include "cubetex.h"
#include "voltex.h"
