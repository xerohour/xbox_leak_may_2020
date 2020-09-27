/*===========================================================================*\
 | 
 |  FILE:	PreFilter.cpp
 |			Skeleton project and code for a Prefilter Kernel
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 21-1-99
 | 
\*===========================================================================*/

#include "PreFilter.h"



/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonKernClassDesc:public ClassDesc {
	public:
	int 			IsPublic()					{ return TRUE; }
	void *			Create(BOOL loading)		{ return new SkeletonKernel; }
	const TCHAR *	ClassName()					{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()				{ return FILTER_KERNEL_CLASS_ID; }
	Class_ID 		ClassID()					{ return KERN_CLASSID; }
	const TCHAR* 	Category()					{ return _T("");  }
};

static SkeletonKernClassDesc SkeletonKernCD;
ClassDesc* GetSkeletonKernDesc() {return &SkeletonKernCD;}



/*===========================================================================*\
 |	Parameter Block
\*===========================================================================*/

static ParamBlockDescID SkeletonKernDescV0[] = {
	{ TYPE_FLOAT, NULL, TRUE, PB_S1 }, 
	{ TYPE_FLOAT, NULL, TRUE, PB_S2 } 
}; 	

static ParamVersionDesc curSkeletonVersion( SkeletonKernDescV0, SKBLOCK_LENGTH, SKCURRENT_VERSION );



/*===========================================================================*\
 |	Kernel Parameter returns - basic custom parameter support
\*===========================================================================*/

// Number of parameters we support
long SkeletonKernel::GetNFilterParams() { return 2; }

// Get the name of parameter #nParam
TCHAR * SkeletonKernel::GetFilterParamName( long nParam )
{ 
	return GetString( nParam ? IDS_PARAM2:IDS_PARAM1 );
}

// Get the value of parameter #nParam
double SkeletonKernel::GetFilterParam( long nParam )
{
	return nParam ? param2 : param1;
}

// Set our parameter variables
void SkeletonKernel::SetFilterParam( long nParam, double val )
{
	if (nParam) {  
		param2 = val;	
		pblock->SetValue( PB_S2, 0, float( val ) );		
 	} else { 
		param1 = val; 
		pblock->SetValue( PB_S1, 0, float( val ) );		
	}
}

void SkeletonKernel::Update(TimeValue t, Interval& valid){
	float val;
	pblock->GetValue( PB_S1, t, val, valid ); param1 = val;
	pblock->GetValue( PB_S2, t, val, valid ); param2 = val;
}



/*===========================================================================*\
 |	Kernel Description
\*===========================================================================*/

TCHAR * SkeletonKernel::GetDefaultComment() 
{
	return GetString(IDS_COMMENT); 
}



/*===========================================================================*\
 |	Constructor, Load & Save
\*===========================================================================*/

SkeletonKernel::SkeletonKernel()
{
	MakeRefByID(FOREVER, 0, CreateParameterBlock(SkeletonKernDescV0, SKBLOCK_LENGTH, SKCURRENT_VERSION));
	assert(pblock);

	param1 = 0.3f;
	param2 = 0.6f;
	pblock->SetValue(PB_S1, 0, (float)param1 );		
	pblock->SetValue(PB_S2, 0, (float)param2 );		
}

IOResult SkeletonKernel::Load(ILoad *iload)
{
	FilterKernel::Load(iload);
	return IO_OK;
}
IOResult SkeletonKernel::Save(ISave *isave)
{
	FilterKernel::Save(isave);
	return IO_OK;
}



/*===========================================================================*\
 |	Subanim & References support
\*===========================================================================*/

Animatable* SkeletonKernel::SubAnim(int i) 
{
	switch (i) {
		case 0: return pblock;
		default: return NULL;
	}
}

TSTR SkeletonKernel::SubAnimName(int i) 
{
	switch (i) {
		case 0: return GetString(IDS_PARAMETERS);
		default: return _T("");
	}
}

RefTargetHandle SkeletonKernel::GetReference(int i)
{
	switch (i) {
		case 0: return pblock;
		default: return NULL;
	}
}

void SkeletonKernel::SetReference(int i, RefTargetHandle rtarg)
{
	switch (i) {
		case 0: pblock = (IParamBlock*)rtarg; break;
	}
}

RefResult SkeletonKernel::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
{
	GetParamName * gpn;

	switch (message) {
		case REFMSG_CHANGE:
			break;

		case REFMSG_GET_PARAM_DIM: {
			GetParamDim * gpd = (GetParamDim*)partID;
				gpd->dim = defaultDim;
			return REF_STOP; 
		}

		case REFMSG_GET_PARAM_NAME: {
			gpn = (GetParamName*)partID;
			switch (gpn->index) {
				case PB_S1:			gpn->name = GetString(IDS_PARAM1_PN); break;
				case PB_S2:			gpn->name = GetString(IDS_PARAM2_PN); break;
				default:			gpn->name = _T(""); break;
			}
			return REF_STOP; 
		}
	}
	return REF_SUCCEED;
}

RefTargetHandle SkeletonKernel::Clone( RemapDir &remap )
{
	SkeletonKernel* skNew = new SkeletonKernel();
	skNew->ReplaceReference(0,remap.CloneRef(pblock));
	return (RefTargetHandle)skNew;
}



/*===========================================================================*\
 |	Calculate a result
\*===========================================================================*/

double SkeletonKernel::KernelFn( double x, double y )
{
	if ( x < param1 ) return 1.0f;
	if ( x > param2 ) return 1.0f;
	else return 0.0f;
}



/*===========================================================================*\
 |	Kernel functionality queries
\*===========================================================================*/

// Integer number of pixels from center to filter 0 edge.
// Must not truncate filter x dimension for 2D filters
long SkeletonKernel::GetKernelSupport(){ return 1; }

// For 2d returns y support, for 1d returns 0
long SkeletonKernel::GetKernelSupportY(){ return 0; }

// Are we 2D or Variable Size?
bool SkeletonKernel::Is2DKernel(){ return FALSE; }
bool SkeletonKernel::IsVariableSz(){ return FALSE; }

// 1-D filters ignore the y parameter, return it as 0.0
void SkeletonKernel::SetKernelSz( double x, double y ){}
void SkeletonKernel::GetKernelSz( double& x, double& y ){ x = 0.5; y = 0.0; }

// Returning true will disable the built-in normalizer
bool SkeletonKernel::IsNormalized(){ return FALSE; }

// This is for possible future optimizations
bool SkeletonKernel::HasNegativeLobes(){ return FALSE; }

