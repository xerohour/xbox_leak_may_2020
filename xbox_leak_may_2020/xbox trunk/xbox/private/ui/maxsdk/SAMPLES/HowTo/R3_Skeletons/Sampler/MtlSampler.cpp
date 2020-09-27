/*===========================================================================*\
 | 
 |  FILE:	MtlSampler.cpp
 |			Skeleton project and code for a Material Sampler
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 21-1-99
 | 
\*===========================================================================*/

#include "MtlSampler.h"



/*===========================================================================*\
 |	Class Descriptor
\*===========================================================================*/

class SkeletonSamplerClassDesc : public ClassDesc {
	public:
	int 			IsPublic()					{ return TRUE; }
	void *			Create(BOOL loading)		{ return new SkeletonSampler; }
	const TCHAR *	ClassName()					{ return GetString(IDS_CLASSNAME); }
	SClass_ID		SuperClassID()				{ return SAMPLER_CLASS_ID; }
	Class_ID 		ClassID()					{ return SAMP_CLASSID; }
	const TCHAR* 	Category()					{ return _T("");  }
};

static SkeletonSamplerClassDesc SkelSamplerCD;
ClassDesc* GetSkeletonSamplerDesc() { return &SkelSamplerCD; }



/*===========================================================================*\
 |	Parameter Block
\*===========================================================================*/

static ParamBlockDescID sksDesc[ SKSPBLOCK_LENGTH ] = {
	{ TYPE_FLOAT, NULL, FALSE, PB_QUALITY },			// Quality
	{ TYPE_BOOL,  NULL, FALSE, PB_ENABLE },				// Enable
	{ TYPE_BOOL,  NULL, FALSE, PB_ADAPT_ENABLE },		// AdaptiveEnable
	{ TYPE_FLOAT, NULL, FALSE, PB_ADAPT_THRESHOLD },	// Threshold
}; 	



/*===========================================================================*\
 |	Sampler Implimentation
\*===========================================================================*/


SkeletonSampler::SkeletonSampler()
{
	MakeRefByID(FOREVER, 0, CreateParameterBlock(sksDesc, SKSPBLOCK_LENGTH, SKSCURRENT_VERSION));
	assert(pblock);
	pblock->SetValue(PB_ENABLE,				0,	FALSE );	
	pblock->SetValue(PB_ADAPT_ENABLE,		0,	TRUE );	
	pblock->SetValue(PB_QUALITY,			0,	0.5f );	
	pblock->SetValue(PB_ADAPT_THRESHOLD,	0,	0.020f );	
}


RefTargetHandle SkeletonSampler::Clone( RemapDir &remap )
{
	SkeletonSampler* sksNew = new SkeletonSampler();
	sksNew->ReplaceReference(0,remap.CloneRef(pblock));
	return (RefTargetHandle)sksNew;
}


IOResult SkeletonSampler::Load(ILoad *iload)
{
	Sampler::Load(iload);
	return IO_OK;
}


IOResult SkeletonSampler::Save(ISave *isave)
{
	Sampler::Save(isave);
	return IO_OK;
}

TCHAR* SkeletonSampler::GetDefaultComment()
{
	return GetString(IDS_COMMENT);
}



/*===========================================================================*\
 |	Subanim & References support
\*===========================================================================*/


Animatable* SkeletonSampler::SubAnim(int i)
{ 
	switch (i) {
		case 0: return pblock;
		default: return NULL;
		}
}

TSTR SkeletonSampler::SubAnimName(int i)
{ 
	switch (i) {
		case 0: return GetString(IDS_PARAMETERS);
		default: return _T("");
		}
}

RefTargetHandle SkeletonSampler::GetReference(int i)
{ 
	switch (i) {
		case 0: return pblock;
		default: return NULL;
		}
}

void SkeletonSampler::SetReference(int i, RefTargetHandle rtarg)
{ 
	switch (i) {
		case 0: pblock = (IParamBlock*)rtarg; break;
		}
}

RefResult SkeletonSampler::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
{
	GetParamName * gpn;

	switch (message) {

		case REFMSG_GET_PARAM_DIM: {
			GetParamDim * gpd = (GetParamDim*)partID;
				gpd->dim = defaultDim; break;
			return REF_STOP; 
		}

		case REFMSG_GET_PARAM_NAME: {
			gpn = (GetParamName*)partID;
			switch (gpn->index) {
				case PB_QUALITY:			gpn->name = _T( GetString(IDS_QUALITY) );	break;
				case PB_ENABLE:				gpn->name = _T( GetString(IDS_ENABLE) );	break;
				case PB_ADAPT_ENABLE:		gpn->name = _T( GetString(IDS_AD_ENABLE) ); break;
				case PB_ADAPT_THRESHOLD:	gpn->name = _T( GetString(IDS_AD_THRESH) ); break;
				default:		  gpn->name = _T(""); break;
			}
			return REF_STOP; 
		}
	}
	return REF_SUCCEED;
}



/*===========================================================================*\
 |	Mask management functions (from ShaderUtil.cpp)
\*===========================================================================*/

inline float bound( float v, float min=0.0f, float max=1.0f )
{ 
	return (v < min)? min : (v > max)? max : v; 
}

inline int bound( int v, int min=0, int max=1 )
{
	return (v < min)? min : (v > max)? max : v; 
}

inline void setMask( MASK m, ULONG v ) { m[0] = m[1] = v; }
inline void copyMask( MASK to, MASK from ) { to[0] = from[0]; to[1]=from[1];}

#define ALL_ONES	0xffffffffL

BOOL sampleInMask( Point2& sample,  MASK m )
{
	int x = int( sample.x * 8.0f );
	x = bound( x, 0, 7 );
	int y = int( sample.y * 8.0f );
	y = bound( y, 0, 7 );
	BYTE * pMask = (BYTE*)m;
	BYTE b = pMask[ y ];

	BYTE in = b & (0x80 >> x) ;
	return in > 0;
}


/*===========================================================================*\
 |  Perform the actual sampling of a point on the surface
 |
 |	We ask NextSample(...) to provide us with a point to use, check whether its
 |	within the current mask and if so, then sample the colour.
 |	
\*===========================================================================*/

#define N_SAMPLES	1

void SkeletonSampler::DoSamples( Color& cOut, Color &tOut, 
			 SamplingCallback* cb, ShadeContext* sc, MASK mask )
{
	int n = 0;
	pSC = sc;

	if ( mask ) 
		copyMask( tmask, mask );
	else
		setMask( tmask, ALL_ONES );

	float sampleScale;
	Point2	sample;
	NextSample( &sample, &sampleScale, n );

	cOut.r = cOut.g = cOut.b = tOut.r = tOut.g = tOut.b = 0;

	if ( sampleInMask( sample, tmask ) )
		cb->SampleAtOffset( cOut, tOut, sample, sampleScale );
}

/*===========================================================================*\
 |  Find next sample point
 |	We just choose the centre of the fragment
\*===========================================================================*/

BOOL SkeletonSampler::NextSample( Point2* pOut, float* pSampleSz, int n )
{
	*pOut = pSC->SurfacePtScreen(); 
	pOut->x = frac( pOut->x ); pOut->y = frac( pOut->y );

	// Sample size is the whole pixel
	*pSampleSz = 1.0f;

	++n;

	return TRUE;
}

// Number of samples
int SkeletonSampler::GetNSamples()
{
	return 1;
}




/*===========================================================================*\
 |	Quality and Enabling support
\*===========================================================================*/

void SkeletonSampler::SetQuality( float q )
{ 
	pblock->SetValue( PB_QUALITY, 0, q );
}

float SkeletonSampler::GetQuality()
{ 
	float q; Interval valid;
	pblock->GetValue( PB_QUALITY, 0, q, valid );
	return q;
}

int SkeletonSampler::SupportsQualityLevels() 
{
	return 1; 
}


void SkeletonSampler::SetEnable( BOOL on )
{ 
	pblock->SetValue( PB_ENABLE, 0, on );
}
BOOL SkeletonSampler::GetEnable()
{ 
	BOOL b; Interval valid;
	pblock->GetValue( PB_ENABLE, 0, b, valid );
	return b;
}



/*===========================================================================*\
 |	Adaptive Sampling support
\*===========================================================================*/


void SkeletonSampler::SetAdaptiveOn( BOOL on )
{ 
	pblock->SetValue( PB_ADAPT_ENABLE, 0, on ); 
}

BOOL SkeletonSampler::IsAdaptiveOn()
{ 
	BOOL b; Interval valid;
	pblock->GetValue( PB_ADAPT_ENABLE, 0, b, valid );
	return b;
}

void SkeletonSampler::SetAdaptiveThreshold( float val )
{ 
	pblock->SetValue( PB_ADAPT_THRESHOLD, 0, val ); 
}

float SkeletonSampler::GetAdaptiveThreshold() 
{ 
	float q; Interval valid;
	pblock->GetValue( PB_ADAPT_THRESHOLD, 0, q, valid );
	return q;
}
