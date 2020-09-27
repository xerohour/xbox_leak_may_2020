/*===========================================================================*\
 | 
 |  FILE:	Shader.h
 |			Skeleton project and code for a Material Shader
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 26-1-99
 | 
\*===========================================================================*/

#ifndef __SAMPSKEL__H
#define __SAMPSKEL__H

#include "max.h"
#include "texutil.h"
#include "shaders.h"
#include "imtl.h"
#include "macrorec.h"
#include "gport.h"

#include "resource.h"
#include "shadertools.h"



// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	SKELSHADER_CLASSID		Class_ID(0x670a56d0, 0x23fc5c6e)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonShaderDesc();



// Paramblock2 name
enum { shader_params, };
// Paramblock2 parameter list
enum 
{ 
	sk_diffuse, sk_brightness,  
};



/*===========================================================================*\
 |	Definition of our UI and map parameters for the shader
 |  For this skeleton we support 3 minimal channels
\*===========================================================================*/

// Number of Map Buttons on our UI and number of texmaps
#define NMBUTS 3
#define SHADER_NTEXMAPS	3
// Channels used by this shader
#define S_DI	0
#define S_BR	1
#define S_TR	2


// Texture Channel number --> IDC resource ID
static int texMButtonsIDC[] = {
	IDC_MAPON_CLR, IDC_MAPON_BR, IDC_MAPON_TR,
};
		
// Map Button --> Texture Map number
static int texmapFromMBut[] = { 0, 1, 2 };


// Channel Name array
static int texNameIDS[STD2_NMAX_TEXMAPS] = {
	IDS_MN_DIFFUSE,		IDS_MN_BRIGHTNESS,	IDS_MN_OPACITY,		IDS_MN_NONE, 
	IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,
	IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,
	IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,
	IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,
	IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,
};	

// Channel Name array (INTERNAL NAMES)
static int intertexNameIDS[STD2_NMAX_TEXMAPS] = {
	IDS_MN_DIFFUSE_I,	IDS_MN_BRIGHTNESS_I,IDS_MN_OPACITY_I,		IDS_MN_NONE, 
	IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,
	IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,
	IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,
	IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,
	IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,		IDS_MN_NONE,
};	

// Internal channel names
static TCHAR* texInternalNames[STD2_NMAX_TEXMAPS] = {
	_T("diffuseMap"),_T("BrightnessMap"), _T("opacityMap"), _T(""), 	
	_T(""), _T(""), _T(""), _T(""), 
	_T(""), _T(""), _T(""), _T(""), 
	_T(""), _T(""), _T(""), _T(""),
	_T(""), _T(""), _T(""), _T(""), 
	_T(""), _T(""), _T(""), _T(""),
};	

// Type of map channels supported
static int chanType[STD2_NMAX_TEXMAPS] = {
	CLR_CHANNEL, MONO_CHANNEL, MONO_CHANNEL, UNSUPPORTED_CHANNEL, 
	UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, 
	UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, 
	UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, 
	UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, 
	UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, UNSUPPORTED_CHANNEL, 
};	


// What channel in our shader maps to one of the StdMat channel IDs?
//
//
// The following are the StdMat channel IDs:
//
// 0   --	ambient
// 1   --	diffuse
// 2   --	specular
// 3   --	shininesNs
// 4   --	shininess strength
// 5   --	self-illumination
// 6   --	opacity
// 7   --	filter color
// 8   --	bump 
// 9   --	reflection
// 10  --	refraction 
// 11  --	displacement

static int stdIDToChannel[N_ID_CHANNELS] = { -1, 0, -1, -1, -1, -1, 2, 3, -1, -1, -1, -1 };



/*===========================================================================*\
 |	Class definition for the shader itself
\*===========================================================================*/

#define SHADER_PARAMS (STD_EXTRA)

class SkeletonShaderDlg;

class SkeletonShader : public Shader {
friend class SkeletonShaderDlg;
protected:

	// The parameter block
	IParamBlock2	*pblock; 
	Interval		ivalid;
	TimeValue		curTime;


	// Pointer to the dialog handler
	SkeletonShaderDlg*	paramDlg;


	// Storage for our parameters
	Color			diffuse;
	float			brightness;


public:

	SkeletonShader();

	// Main plugin identity support
	Class_ID ClassID() { return SKELSHADER_CLASSID; }
	SClass_ID SuperClassID() { return SHADER_CLASS_ID; }
	TSTR GetName() { return GetString( IDS_CLASSNAME ); }
	void GetClassName(TSTR& s) { s = GetName(); }  
	void DeleteThis(){ delete this; }		

	// Tell MAX what standard parameters that we support
    ULONG SupportStdParams(){ return SHADER_PARAMS; }

	// copy std params, for switching shaders
    void CopyStdParams( Shader* pFrom );
	void ConvertParamBlk( ParamBlockDescID *oldPBDesc, int oldCount, IParamBlock *oldPB ){};

	// Texture map channel support
	long nTexChannelsSupported(){ return SHADER_NTEXMAPS; }
	TSTR GetTexChannelName( long nTex ){ return GetString( texNameIDS[ nTex ] ); }
	TSTR GetTexChannelInternalName( long nTex ){ return GetString( intertexNameIDS[ nTex ] ); }
	long ChannelType( long nChan ) { return chanType[nChan]; }
	long StdIDToChannel( long stdID ){ return stdIDToChannel[stdID]; }

	// Find out if we have a key at time value t
	BOOL KeyAtTime(int id,TimeValue t) { return pblock->KeyFrameAtTime(id,t); }

	// Get the requirements for this material (supersampling, etc)
	ULONG GetRequirements( int subMtlNum ){ return MTLREQ_PHONG; }

	// Support for the dialog UI
	ShaderParamDlg* CreateParamDialog(HWND hOldRollup, HWND hwMtlEdit, IMtlParams *imp, StdMat2* theMtl, int rollupOpen );
	ShaderParamDlg* GetParamDlg(){ return (ShaderParamDlg*)paramDlg; }
	void SetParamDlg( ShaderParamDlg* newDlg ){ paramDlg = (SkeletonShaderDlg*)newDlg; }


	// Animatables and References
	int NumSubs() { return 1; }  
	Animatable* SubAnim(int i){ return (i==0)? pblock : NULL; }
	TSTR SubAnimName(int i){ return TSTR(GetString( IDS_PARAMETERS)); };
	int SubNumToRefNum(int subNum) { return subNum;	}

	int NumRefs() { return 1; }
	RefTargetHandle GetReference(int i){ return (i==0)? pblock : NULL; }
	void SetReference(int i, RefTargetHandle rtarg) 
		{ if (i==0) pblock = (IParamBlock2*)rtarg; }
	void NotifyChanged(){ NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE); }
	RefTargetHandle Clone( RemapDir &remap=NoRemap() );
	RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
	                            PartID& partID, RefMessage message );


	// Direct ParamBlock2 access
	int	NumParamBlocks() { return 1; }
	IParamBlock2* GetParamBlock(int i) { return pblock; }
	IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } 


	// Shader state methods
	void Update(TimeValue t, Interval& valid);
	void Reset();


	// Standard load / save
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);



	// ----------------------------------------------------------
	// The following methods are implimented in Eval.cpp

	// Fill the IllumParams with our data
	void GetIllumParams( ShadeContext &sc, IllumParams &ip );

	// Shader specific implimentations
	void Illum(ShadeContext &sc, IllumParams &ip);
	void AffectReflection(ShadeContext &sc, IllumParams &ip, Color &rcol);

	void CombineComponents( ShadeContext &sc, IllumParams& ip );

	float EvalHiliteCurve(float x);

	// Metal support
	BOOL IsMetal();



	// ----------------------------------------------------------
	// The following transactions are implimented in GetSet.cpp

	// Skeleton Shader specific
	void SetBrightness(float v, TimeValue t);
	float GetBrightness(int mtlNum=0, BOOL backFace=FALSE);
	float GetBrightness( TimeValue t);

	// DIFFUSE
	void SetDiffuseClr(Color c, TimeValue t);
    Color GetDiffuseClr(int mtlNum=0, BOOL backFace=FALSE);		
	Color GetDiffuseClr(TimeValue t);		

	// AMBIENT
	void SetAmbientClr(Color c, TimeValue t);
	Color GetAmbientClr(int mtlNum=0, BOOL backFace=FALSE);		
	Color GetAmbientClr(TimeValue t);		

	// SPECULAR
	void SetSpecularClr(Color c, TimeValue t);
	void SetSpecularLevel(float v, TimeValue t);		
	Color GetSpecularClr(int mtlNum=0, BOOL backFace=FALSE);
	float GetSpecularLevel(int mtlNum=0, BOOL backFace=FALSE);
	Color GetSpecularClr(TimeValue t);
	float GetSpecularLevel(TimeValue t);

	// SELFILLUM
	void SetSelfIllum(float v, TimeValue t);
	float GetSelfIllum(int mtlNum=0, BOOL backFace=FALSE);
	void SetSelfIllumClrOn( BOOL on );
	BOOL IsSelfIllumClrOn();
	BOOL IsSelfIllumClrOn(int mtlNum, BOOL backFace);
	void SetSelfIllumClr(Color c, TimeValue t);
	Color GetSelfIllumClr(int mtlNum=0, BOOL backFace=FALSE);
	float GetSelfIllum(TimeValue t);	
	Color GetSelfIllumClr(TimeValue t);		

	// SOFTEN
	void SetSoftenLevel(float v, TimeValue t);
	float GetSoftenLevel(int mtlNum=0, BOOL backFace=FALSE);
	float GetSoftenLevel(TimeValue t);


	void SetGlossiness(float v, TimeValue t);
	float GetGlossiness(int mtlNum, BOOL backFace);
	float GetGlossiness( TimeValue t);


	// Standard locks not supported (Diffuse+Specular)/(Ambient+Diffuse)
	void SetLockDS(BOOL lock){ }
	BOOL GetLockDS(){ return FALSE; }
	void SetLockAD(BOOL lock){ }
	BOOL GetLockAD(){ return FALSE; }
	void SetLockADTex(BOOL lock){ }
	BOOL GetLockADTex(){ return FALSE; }

};



#endif