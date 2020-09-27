/**********************************************************************
 *<
	FILE: stdmtl2.h

	DESCRIPTION:

	CREATED BY: Dan Silva modified for shiva by Kells Elmquist

	HISTORY: modified for shiva by Kells Elmquist
	         modified to use ParamBlock2, John Wainwright 11/16/98

 *>	Copyright (c) 1998, All Rights Reserved.
 **********************************************************************/

#ifndef __STDMTL2__H
#define __STDMTL2__H

#include "shaders.h"
#include "samplers.h"
#include "iparamm2.h"
#include "texmaps.h"

// StdMtl2 flags values
#define STDMTL_ADD_TRANSP   (1<<0)
#define STDMTL_FALLOFF_OUT  (1<<1)
#define STDMTL_WIRE		  	(1<<2)
#define STDMTL_2SIDE		(1<<3)
#define STDMTL_SOFTEN       (1<<4)
#define STDMTL_FILT_TRANSP 	(1<<5)
#define STDMTL_WIRE_UNITS	(1<<6)
#define STDMTL_LOCK_AD      (1<<8)
#define STDMTL_LOCK_DS      (1<<9)
#define STDMTL_UNUSED1		(1<<10)
#define STDMTL_LOCK_ADTEX   (1<<11)
#define STDMTL_FACEMAP		(1<<12)
#define STDMTL_OLDSPEC      (1<<13)
#define STDMTL_SSAMP_ON		(1<<14)
#define STDMTL_COLOR_SI		(1<<15)
#define STDMTL_FACETED		(1<<16)

#define STDMTL_ROLLUP0_OPEN  (1<<27)	// shader
#define STDMTL_ROLLUP1_OPEN  (1<<28)	// basic
#define STDMTL_ROLLUP2_OPEN  (1<<29)	// extra
#define STDMTL_ROLLUP3_OPEN  (1<<30)	// maps
#define STDMTL_ROLLUP4_OPEN  (1<<26)	// sampling
#define STDMTL_ROLLUP5_OPEN  (1<<25)	// dynamics
#define STDMTL_ROLLUP6_OPEN  (1<<24)	// effects

// only needed if the constant shader is included in shaders
#define  CONSTClassID (STDSHADERS_CLASS_ID+1)

#define STDMTL_ROLLUP_FLAGS (STDMTL_ROLLUP0_OPEN|STDMTL_ROLLUP1_OPEN|STDMTL_ROLLUP2_OPEN|STDMTL_ROLLUP3_OPEN \
							|STDMTL_ROLLUP4_OPEN|STDMTL_ROLLUP5_OPEN|STDMTL_ROLLUP6_OPEN)

class StdMtl2Dlg;


// IDs for all the ParamBlocks and their parameters.  One block UI per rollout.
enum { std2_shader, std2_extended, std2_sampling, std_maps, std2_dynamics, };  // pblock IDs
// std2_shader param IDs
enum 
{ 
	std2_shader_type, std2_wire, std2_two_sided, std2_face_map, std2_faceted,
	std2_shader_by_name,  // virtual param for accessing shader type by name
};
// std2_extended param IDs
enum 
{ 
	std2_opacity_type, std2_opacity, std2_filter_color, std2_ep_filter_map,
	std2_falloff_type, std2_falloff_amnt, 
	std2_ior,
	std2_wire_size, std2_wire_units,
	std2_apply_refl_dimming, std2_dim_lvl, std2_refl_lvl,
};

// std2_sampling param IDs
enum 
{ 
	std2_ssampler, std2_ssampler_qual, std2_ssampler_enable, 
		std2_ssampler_adapt_on, std2_ssampler_adapt_threshold, std2_ssampler_advanced,
		std2_ssampler_subsample_tex_on, std2_ssampler_by_name, 
		std2_ssampler_param0, std2_ssampler_param1,
};
// std_maps param IDs
enum 
{
	std2_map_enables, std2_maps, std2_map_amnts, std2_mp_ad_texlock, 
};
// std2_dynamics param IDs
enum 
{
	std2_bounce, std2_static_friction, std2_sliding_friction,
};


// paramblock2 block and parameter IDs for the standard shaders
// NB these are duplicated in shaders/stdShaders.cpp...
enum { shdr_params, };
// shdr_params param IDs
enum 
{ 
	shdr_ambient, shdr_diffuse, shdr_specular,
	shdr_ad_texlock, shdr_ad_lock, shdr_ds_lock, 
	shdr_use_self_illum_color, shdr_self_illum_amnt, shdr_self_illum_color, 
	shdr_spec_lvl, shdr_glossiness, shdr_soften,
};



#define NUM_REFS		9

// refs
#define OLD_PBLOCK_REF	0		// reference number assignments
#define TEXMAPS_REF		1
#define SHADER_REF		2
#define SHADER_PB_REF	3
#define EXTENDED_PB_REF	4
#define SAMPLING_PB_REF	5
#define MAPS_PB_REF		6
#define DYNMAICS_PB_REF	7
#define SAMPLER_REF		8

// sub anims
#define NUM_SUB_ANIMS	5
//#define OLD_PARAMS_SUB		0
#define TEXMAPS_SUB			0
#define SHADER_SUB			1
#define EXTRA_PB_SUB		2
#define SAMPLING_PB_SUB		3
#define DYNAMICS_PB_SUB		4


class StdMtl2 : public StdMat2 {
	// Animatable parameters
	public:
		// current UI if open
		static ShaderParamDlg* pShaderDlg;
		static IAutoMParamDlg* masterDlg;
		static IAutoMParamDlg* texmapDlg;
		static IAutoMParamDlg* extendedDlg;
		static IAutoMParamDlg* samplingDlg;
		static HWND			   curHwmEdit;
		static IMtlParams*	   curImp;
		static Tab<ClassDesc*> shaderList;
		static Tab<ClassDesc*> samplerList;

		IParamBlock *old_pblock;    // ref 0, for old version loading
		Texmaps* maps;				// ref 1
		Interval ivalid;
		ULONG flags;
		int shaderId;
		Shader *pShader;			// ref 2
		// new PB2 paramblocks, one per rollout
		IParamBlock2 *pb_shader;	// ref 3, 4, ...
		IParamBlock2 *pb_extended;	
		IParamBlock2 *pb_sampling;	
		IParamBlock2 *pb_maps;	
		IParamBlock2 *pb_dynamics;	

		Color filter;
		float opacity;	
		float opfall;
		float wireSize;
		float ioRefract;
		float dimIntens;
		float dimMult;
		BOOL dimReflect;

		// sampling 
		int samplerId;
		Sampler* pixelSampler;	// ref 8

		// composite of shader/mtl channel types
		int channelTypes[ STD2_NMAX_TEXMAPS ];
		int stdIDToChannel[ N_ID_CHANNELS ];

		// experiment: override filter
		BOOL	filterOverrideOn;
		float	filterSz;

		void SetFlag(ULONG f, ULONG val);
		void EnableMap(int i, BOOL onoff);
		BOOL IsMapEnabled(int i) { return (*maps)[i].mapOn; }
		BOOL KeyAtTime(int id,TimeValue t) { return (id == OPACITY_PARAM) ? pb_extended->KeyFrameAtTime(std2_opacity, t) : FALSE; }
		BOOL AmtKeyAtTime(int i, TimeValue t);
		int  GetMapState( int indx ); //returns 0 = no map, 1 = disable, 2 = mapon
		TSTR  GetMapName( int indx ); 
		void SyncADTexLock( BOOL lockOn );

		// from StdMat
		// these set Approximate colors into the plug in shader
		BOOL IsSelfIllumColorOn();
		void SetSelfIllumColorOn( BOOL on );
		void SetSelfIllumColor(Color c, TimeValue t);		
		void SetAmbient(Color c, TimeValue t);		
		void SetDiffuse(Color c, TimeValue t);		
		void SetSpecular(Color c, TimeValue t);
		void SetShininess(float v, TimeValue t);		
		void SetShinStr(float v, TimeValue t);		
		void SetSelfIllum(float v, TimeValue t);	
		void SetSoften(BOOL onoff) { SetFlag(STDMTL_SOFTEN,onoff); }
		
		void SetTexmapAmt(int imap, float amt, TimeValue t);
		void LockAmbDiffTex(BOOL onoff) { SetFlag(STDMTL_LOCK_ADTEX,onoff); }

		void SetWire(BOOL onoff){ pb_shader->SetValue(std2_wire,0, (onoff!=0) ); }//SetFlag(STDMTL_WIRE,onoff); }
		void SetWireSize(float s, TimeValue t);
		void SetWireUnits(BOOL onoff) { pb_extended->SetValue(std2_wire_units,0, (onoff!=0) ); } //SetFlag(STDMTL_WIRE_UNITS,onoff); }
		
		void SetFaceMap(BOOL onoff) { pb_shader->SetValue(std2_face_map,0, (onoff!=0) ); } //SetFlag(STDMTL_FACEMAP,onoff); }
		void SetTwoSided(BOOL onoff) { pb_shader->SetValue(std2_two_sided,0, (onoff!=0) ); } //SetFlag(STDMTL_2SIDE,onoff); }
		void SetFalloffOut(BOOL outOn) { pb_extended->SetValue(std2_falloff_type,0, (outOn!=0) ); } //SetFlag(STDMTL_FALLOFF_OUT,onoff); }
		void SetTransparencyType(int type);

		void SetFilter(Color c, TimeValue t);
		void SetOpacity(float v, TimeValue t);		
		void SetOpacFalloff(float v, TimeValue t);		
		void SetIOR(float v, TimeValue t);
		void SetDimIntens(float v, TimeValue t);
		void SetDimMult(float v, TimeValue t);
		
	    int GetFlag(ULONG f) { return (flags&f)?1:0; }

		// >>>Shaders

		// these 3 internal only
		void SetShaderIndx( long shaderId, BOOL update=TRUE );
		long GetShaderIndx(){ return shaderId; }
		void SetShader( Shader* pNewShader );
		void ShuffleTexMaps( Shader* newShader, Shader* oldShader );
		void ShuffleShaderParams( Shader* newShader, Shader* oldShader );

		Shader* GetShader(){ return pShader; }
		void SwitchShader(Shader* pNewShader, BOOL loadDlg = FALSE);
		void SwitchShader(ClassDesc* pNewCD);
		BOOL SwitchShader(Class_ID shaderId);
		int FindShader( Class_ID& findId, ClassDesc** ppCD=NULL );
		BOOL IsShaderInUI() { return pb_shader && pb_shader->GetMap() && pShader && pShader->GetParamDlg(); }

		static void StdMtl2::LoadShaderList();
		static int StdMtl2::NumShaders();
		static ClassDesc* StdMtl2::GetShaderCD(int i);
		static void StdMtl2::LoadSamplerList();
		static int StdMtl2::NumSamplers();
		static ClassDesc* StdMtl2::GetSamplerCD(int i);

		BOOL IsFaceted(){ return GetFlag(STDMTL_FACETED); }
		void SetFaceted( BOOL on ){	pb_shader->SetValue(std2_faceted,0, (on!=0) ); }

		// These utilitys provide R2.5 shaders, ONLY used for Translators
		// Does not & will not work for plug-in shaders
		void SetShading(int s);
		int GetShading();

		// from Mtl
		Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);		
	    Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);		
		Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
		float GetShininess(int mtlNum=0, BOOL backFace=FALSE);	
		float GetShinStr(int mtlNum=0, BOOL backFace=FALSE) ;
		float GetXParency(int mtlNum=0, BOOL backFace=FALSE) { if(opacity>0.9f && opfall>0.0f) return 0.1f; return 1.0f-opacity; }
		float WireSize(int mtlNum=0, BOOL backFace=FALSE) { return wireSize; }

		// >>>> Self Illumination 
		float GetSelfIllum(int mtlNum, BOOL backFace) ;
		BOOL  GetSelfIllumColorOn(int mtlNum, BOOL backFace);
		Color GetSelfIllumColor(int mtlNum, BOOL backFace);
		
		// >>>> sampling
		void SetSamplingOn( BOOL on )
		{	
			pb_sampling->SetValue(std2_ssampler_enable, 0, on!=0 );
		}	
		BOOL GetSamplingOn()
		{	Interval iv; 
			BOOL on;
			pb_sampling->GetValue(std2_ssampler_enable, 0, on, iv );
			return on;
		}	
		void SetSamplingQuality( float quality )
		{	 
			pb_sampling->SetValue(std2_ssampler_qual, 0, quality );
		}	
		float GetSamplingQuality()
		{	Interval iv; 
			float q;
			pb_sampling->GetValue(std2_ssampler_qual, 0, q, iv );
			return q;
		}

		void SwitchSampler(ClassDesc* pNewCD);
		void SwitchSampler(Sampler* pNewSampler);
		BOOL SwitchSampler(Class_ID samplerId);
		int FindSampler( Class_ID findId, ClassDesc** pNewCD=NULL );
		Sampler * GetPixelSampler(int mtlNum=0, BOOL backFace=FALSE){  return pixelSampler; }	

		// these 2 internal only
		void SetSamplerIndx( long indx, BOOL update=TRUE );
		long  GetSamplerIndx(){ return samplerId; }
		void SetPixelSampler( Sampler * sampler );

		// Dynamics properties
		float GetDynamicsProperty(TimeValue t, int mtlNum, int propID);
		void SetDynamicsProperty(TimeValue t, int mtlNum, int propID, float value);

		// from StdMat
		BOOL GetSoften() { return GetFlag(STDMTL_SOFTEN); }
		BOOL GetFaceMap() { return GetFlag(STDMTL_FACEMAP); }
		BOOL GetTwoSided() { return GetFlag(STDMTL_2SIDE); }
		BOOL GetWire() { return GetFlag(STDMTL_WIRE); }
		BOOL GetWireUnits() { return GetFlag(STDMTL_WIRE_UNITS); }
		BOOL GetFalloffOut() { return GetFlag(STDMTL_FALLOFF_OUT); }  // 1: out, 0: in
		BOOL GetAmbDiffTexLock(){ return GetFlag(STDMTL_LOCK_ADTEX); } 
		int GetTransparencyType() {
			return (flags&STDMTL_FILT_TRANSP)?TRANSP_FILTER:
				flags&STDMTL_ADD_TRANSP ? TRANSP_ADDITIVE: TRANSP_SUBTRACTIVE;
			}
		Color GetFilter(TimeValue t);

		// these are stubs till i figure out scripting
		Color GetAmbient(TimeValue t);		
		Color GetDiffuse(TimeValue t);		
		Color GetSpecular(TimeValue t);
		float GetShininess( TimeValue t);		
		float GetShinStr(TimeValue t);	
		float GetSelfIllum(TimeValue t);
		BOOL  GetSelfIllumColorOn();
		Color GetSelfIllumColor(TimeValue t); 

		float GetOpacity( TimeValue t);		
		float GetOpacFalloff(TimeValue t);		
		float GetWireSize(TimeValue t);
		float GetIOR( TimeValue t);
		float GetDimIntens( TimeValue t);
		float GetDimMult( TimeValue t);
		float GetSoftenLevel( TimeValue t);
		BOOL MapEnabled(int i);
		float GetTexmapAmt(int imap, TimeValue t);

		// internal
		float GetOpacity() { return opacity; }		
		float GetOpacFalloff() { return opfall; }		
		float GetTexmapAmt(int imap);
		Color GetFilter();
		float GetIOR() { return ioRefract; }

		StdMtl2(BOOL loading = FALSE);
		BOOL ParamWndProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		BOOL SetDlgThing(ParamDlg* dlg);
		void UpdateTexmaps();
		void UpdateMapButtons();
		void UpdateExtendedMapButtons();
		void UpdateMtlDisplay();
		void UpdateLockADTex( BOOL passOn );
		void UpdateExtraParams( ULONG stdParams );
		void UpdateSamplingParams();


		Color TranspColor(ShadeContext& sc, float opac, Color& diff);
		void Shade(ShadeContext& sc);
		float EvalDisplacement(ShadeContext& sc); 
		Interval DisplacementValidity(TimeValue t); 
		void Update(TimeValue t, Interval& validr);
		void Reset();
		void OldVerFix(int loadVer);
		void BumpFix();
		Interval Validity(TimeValue t);
		void NotifyChanged();

		// Requirements
		ULONG Requirements(int subMtlNum);
		void MappingsRequired(int subMtlNum, BitArray & mapreq, BitArray &bumpreq);

		// Methods to access texture maps of material
		int NumSubTexmaps() { return STD2_NMAX_TEXMAPS; }
		Texmap* GetSubTexmap(int i) { return (*maps)[i].map; }
		int MapSlotType(int i);
		void SetSubTexmap(int i, Texmap *m);
		TSTR GetSubTexmapSlotName(int i);
		int SubTexmapOn(int i) { return  MAPACTIVE(i); } 
		long StdIDToChannel( long id ){ return stdIDToChannel[id]; }

		Class_ID ClassID();
		SClass_ID SuperClassID() { return MATERIAL_CLASS_ID; }
		void GetClassName(TSTR& s) { s = GetString(IDS_KE_STANDARD2); }  

		void DeleteThis();

		int NumSubs() { return NUM_SUB_ANIMS; }  
	    Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum);

		// JBW: add direct ParamBlock access
		int	NumParamBlocks() { return 5; }
		IParamBlock2* GetParamBlock(int i);
		IParamBlock2* GetParamBlockByID(BlockID id);

		// From ref
 		int NumRefs() { return NUM_REFS; }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		RefTargetHandle Clone(RemapDir &remap = NoRemap());
		RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message );

		// IO
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);

	};

Mtl* CreateStdMtl2();

#endif
