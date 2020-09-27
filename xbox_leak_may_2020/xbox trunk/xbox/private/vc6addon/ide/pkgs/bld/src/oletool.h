#ifndef _OLETOOL_H_
#define _OLETOOL_H_

#include "bldapi.h"

class CActionSlobList;
class CActionSlob;
class CPlat;
class CDeps;
class CDefaultConfig;
class CTool;
class CProj;
class CScanner;
class CIEnum;

#if 0
typedef CTool *LPBUILDTOOL;
typedef CIEnum *LPENUMBUILDTOOLS;

typedef CDefaultConfig* LPDEFAULTCONFIG;
typedef CIEnum *LPENUMDEFAULTCONFIGS;

typedef CIEnum *LPENUMOPTIONSTRINGS;
typedef CIEnum *LPENUMOPTIONTYPES;

typedef CProj *LPPROJTYPE;
typedef CIEnum *LPENUMPROJTYPES;

typedef CDeps		*LPDEPMANAGER;
typedef CScanner	*LPDEPSCANNER;

typedef CActionSlobList *LPENUMBSACTION;
typedef CActionSlob 	*LPACTION;

typedef CPath	*LPBUILDFILE;
typedef void	*LPBUILDFILESET;

typedef CPlat		*LPPLATFORM;
typedef CIEnum		*LPENUMPLATFORMS;
#endif


class CIEnum{
public:
	virtual void Next( void ) {};
	virtual void Clone( void ){};
	virtual void Reset( void ){};
	virtual void Skip( void ){};
};

class CBldAddOn{
public:
	virtual void GetName( void ) {};
	virtual void SetClient( LPBUILDSYSTEM ){};
	virtual void EnumBuildTools( LPENUMBUILDTOOLS *, LPBUILDTOOL * ){};
	virtual void EnumProjectTypes( LPENUMPROJECTTYPES *, LPPROJECTTYPE * ){};
	virtual void EnumPlatforms( LPENUMPLATFORMS *, LPPLATFORM * ){};
	virtual void ModifyProjectTypes( LPENUMPROJECTTYPES ){};
};

class CProj{
public:
	virtual void GetName( CString *str ){};
	virtual void GetType( UINT *id ){};
	virtual void GetPlatform( LPPLATFORM *){};
	virtual BOOL IsSupported(){ return FALSE; };

	virtual void PickTool( LPBUILDFILE pFile, LPBUILDTOOL *pBuildTool){};
	virtual void AddTool( LPBUILDTOOL pBuildTool){};
	virtual void EnumBuildTools( LPENUMBUILDTOOLS *, LPBUILDTOOL * ){};

//	virtual void EnumDefaultConfigs( LPENUMDEFAULTCONFIGS *, LPDEFAULTCONFIG * ){};
};

class CTool{
public:
	virtual BOOL IsCollectorTool(){return 0;};
	virtual BOOL Filter(LPBUILDFILE){return 0;};
	virtual void PerformBuild( int type, int stage,LPENUMBSACTIONS, int *result ){};
	virtual BOOL GenerateCommandLines( LPENUMBSACTIONS, CStringList &, CStringList & ){return 0;};
	virtual BOOL GenerateOutputs( LPENUMBSACTIONS ){return 0;};
	virtual BOOL GetDependencies( LPENUMBSACTIONS ){return 0;};
	virtual void EnumOptionStrings( LPENUMOPTIONSTRINGS * ){};
	virtual void EnumOptionTypes( LPENUMOPTIONTYPES * ){};
};

class CScanner{
public:
	virtual void PrepareForScan( HCONFIGURATION ){};
	virtual void ScanComplete( HCONFIGURATION ){};
	virtual BOOL GenerateDependencies( LPBSACTION ){return 0;};
};

class COptHandler {
	virtual void GetFakeStrProp( UINT PropID, LPCOLESTR * ){};
	virtual void SetFakeStrProp( UINT PropID, LPCOLESTR * ){};
};

class CDefaultConfig {
	virtual void GetName( LPCOLESTR * ){};
	virtual void GetOutDir( LPCOLESTR * ){};
	virtual void GetSettings( LPBUILDTOOL, LPCOLESTR * ){};
};

class CPlat {
	virtual void GetName( LPCOLESTR * ){};
	virtual BOOL IsSupported(){ return FALSE; };
	virtual void GetEnvPath( LPCOLESTR *, LPCOLESTR * ){};
};

#endif
