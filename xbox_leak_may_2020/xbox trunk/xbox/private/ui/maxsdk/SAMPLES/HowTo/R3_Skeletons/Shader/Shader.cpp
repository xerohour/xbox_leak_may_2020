/*===========================================================================*\
 | 
 |  FILE:	Shader.cpp
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


#include "Shader.h"



/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonShaderClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic()					{ return TRUE; }
	void *			Create(BOOL loading)		{ return new SkeletonShader(); }
	const TCHAR *	ClassName()					{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()				{ return SHADER_CLASS_ID; }
	Class_ID 		ClassID()					{ return SKELSHADER_CLASSID; }
	const TCHAR* 	Category()					{ return _T("");  }

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return _T("SkeletonShader"); }
	HINSTANCE		HInstance()					{ return hInstance; }
};

SkeletonShaderClassDesc SkelShaderCD;
ClassDesc * GetSkeletonShaderDesc(){ return &SkelShaderCD; }



/*===========================================================================*\
 |	Paramblock2 Descriptor
\*===========================================================================*/

static ParamBlockDesc2 shader_param_blk ( shader_params, _T("shaderParameters"),  0, &SkelShaderCD, P_AUTO_CONSTRUCT, 0, 
	// params
	sk_diffuse, _T("diffuse"), TYPE_RGBA, P_ANIMATABLE, IDS_MN_DIFFUSE, 
		p_default, Color(0.8f, 0.5f, 0.5f), 
		end,
	sk_brightness, _T("brightness"), TYPE_PCNT_FRAC, P_ANIMATABLE, IDS_MN_BRIGHTNESS,
		p_default,		0.2f,
		p_range,		0.0f, 1.0f,
		end,
	end
);



/*===========================================================================*\
 |	Constructor
\*===========================================================================*/

SkeletonShader::SkeletonShader() 
{ 
	pblock = NULL; 
	paramDlg = NULL; 
	curTime = 0;
	ivalid.SetEmpty(); 
}



/*===========================================================================*\
 |	Cloning and coping standard parameters
\*===========================================================================*/

void SkeletonShader::CopyStdParams( Shader* pFrom )
{
	// We don't want to see this parameter copying in macrorecorder
	macroRecorder->Disable(); 

		SetAmbientClr( pFrom->GetAmbientClr(0,0), curTime );
		SetDiffuseClr( pFrom->GetDiffuseClr(0,0), curTime );

	macroRecorder->Enable();
	ivalid.SetEmpty();	
}


RefTargetHandle SkeletonShader::Clone( RemapDir &remap )
{
	SkeletonShader* mnew = new SkeletonShader();
	mnew->ReplaceReference(0, remap.CloneRef(pblock));
	mnew->ivalid.SetEmpty();	
	mnew->diffuse = diffuse;
	mnew->brightness = brightness;
	return (RefTargetHandle)mnew;
}



/*===========================================================================*\
 |	Shader state
\*===========================================================================*/

void SkeletonShader::Update(TimeValue t, Interval &valid) {
	Point3 p;
	if (!ivalid.InInterval(t)) {
		ivalid.SetInfinite();

		pblock->GetValue( sk_diffuse, t, p, ivalid );
		diffuse= Bound(Color(p.x,p.y,p.z));
		pblock->GetValue( sk_brightness, t, brightness, ivalid );
		brightness = Bound(brightness );
		curTime = t;
	}
	valid &= ivalid;
}

void SkeletonShader::Reset()
{
	SkelShaderCD.MakeAutoParamBlocks(this);	// Create and intialize paramblock2

	ivalid.SetEmpty();
	SetDiffuseClr( Color(0.8f,0.5f,0.5f), 0 );
	SetBrightness( 0.2f,0);   
}



/*===========================================================================*\
 |	Shader load/save
\*===========================================================================*/


#define SHADER_VERS_CHUNK 0x6500

IOResult SkeletonShader::Save(ISave *isave) 
{ 
ULONG nb;

	isave->BeginChunk(SHADER_VERS_CHUNK);
	int version = 1;
	isave->Write(&version,sizeof(version),&nb);			
	isave->EndChunk();

	return IO_OK;
}		



IOResult SkeletonShader::Load(ILoad *iload) { 
	ULONG nb;
	int id;
	int version = 0;

	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(id = iload->CurChunkID())  {
			case SHADER_VERS_CHUNK:
				res = iload->Read(&version,sizeof(version), &nb);
				break;
		}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
	}

	return IO_OK;
}
