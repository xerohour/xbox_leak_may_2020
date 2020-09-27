/*===========================================================================*\
 | 
 |  FILE:	MtlSampler.h
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

#ifndef __SAMPSKEL__H
#define __SAMPSKEL__H

#include "max.h"
#include "texutil.h"
#include "samplers.h"

#include "resource.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	SAMP_CLASSID		Class_ID(0x5a081789, 0x358b0a20)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonSamplerDesc();



/*===========================================================================*\
 |	Parameter Block defines
\*===========================================================================*/

#define	PB_QUALITY			0
#define	PB_ENABLE			1
#define	PB_ADAPT_ENABLE		2
#define	PB_ADAPT_THRESHOLD	3

#define SKSPBLOCK_LENGTH	4
#define SKSCURRENT_VERSION	1



/*===========================================================================*\
 |	SkeletonSampler class defn
\*===========================================================================*/

class SkeletonSampler: public Sampler {

	// Temporary shadecontext pointer, used during DoSamples
	ShadeContext* pSC;

	public:
		IParamBlock *pblock;

		// fragment mask storage
		MASK	tmask;

		SkeletonSampler();
		void DeleteThis() { delete this; };
		TCHAR* GetDefaultComment();

		RefTargetHandle Clone( RemapDir &remap );


		// Animatable/Reference
		int NumSubs() {return 1;}
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);

		int NumRefs() { return 1;};
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
							         PartID& partID,  RefMessage message);


		// ClassID and Name
		Class_ID ClassID() {return SAMP_CLASSID;};
		TSTR GetName() { return GetString( IDS_CLASSNAME ); }
		void GetClassName(TSTR& s) { s = GetName(); }


		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);


		// This samples a sequence for the area
		void DoSamples( Color& cOut, Color&tOut, SamplingCallback* cb, ShadeContext* sc, MASK mask=NULL );
		// This is the function that is called to get the next sample 
		// returns FALSE when out of samples
		BOOL NextSample( Point2* pOutPt, float* pSampleSz, int n );
		// Integer number of samples for current quality setting
		int GetNSamples();	


		// Get/Set quality and enable status
		int SupportsQualityLevels();
		void SetQuality( float q );
		float GetQuality();

		void SetEnable( BOOL on );
		BOOL GetEnable();

		// Return that we support adaptive
		ULONG SupportsStdParams(){ return R3_ADAPTIVE; }

		void SetAdaptiveOn( BOOL on );
		BOOL IsAdaptiveOn();

		void SetAdaptiveThreshold( float val );
		float GetAdaptiveThreshold();
	};


#endif