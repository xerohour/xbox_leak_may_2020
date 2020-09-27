#pragma once 
#include <xtl.h>
#include <xgraphics.h>
#include "materiallibrary.h"
#include "texturecache.h"

class CNtIoctlCdromService;
class CInputHandle;
class XOApplication;
class CNetConfig;

extern LPDIRECT3DDEVICE8 g_pd3dDevice;
extern TCHAR* g_szAppDir;
extern float* g_fAppTime;

extern CMaterialLibrary g_MaterialLib;

extern CNtIoctlCdromService g_cdrom;
extern int g_nDiscType;

extern CInputHandle g_szUserInput;
extern XOApplication*    g_pXBApp;

extern CNetConfig g_NetConfig;
extern bool g_ResetAutomation;