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

#ifndef __KERNSKEL__H
#define __KERNSKEL__H

#include "max.h"
#include "render.h"
#include "iparamm.h"

#include "resource.h"


// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	KERN_CLASSID		Class_ID(0x11781c8a, 0x13d059bc)


extern ClassDesc* GetSkeletonKernDesc();
TCHAR *GetString(int id);



/*===========================================================================*\
 |	Parameter Block defines
\*===========================================================================*/

#define PB_S1	0
#define PB_S2	1

#define SKBLOCK_LENGTH 2

// Version definition
#define SKNUM_OLDVERSIONS	0
#define SKCURRENT_VERSION	1



/*===========================================================================*\
 |	SkeletonKernel class defn
\*===========================================================================*/

class SkeletonKernel: public FilterKernel {
	private:
		// We store 2 parameters
		double param1, param2;

	public:
		// Parameters
		IParamBlock *pblock;
		

		SkeletonKernel();
		~SkeletonKernel() { 	};
		void DeleteThis() {delete this;};


		// Animatable/Reference
		int NumSubs() {return 1;};
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);

		// References
		int NumRefs() {return 1;};
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	         PartID& partID,  RefMessage message);
		RefTargetHandle Clone( RemapDir &remap );

		// ClassID/ClassName identifiers
		Class_ID ClassID() {return KERN_CLASSID;};
		void GetClassName(TSTR& s) {s=GetString(IDS_CLASSNAME);};

		// Standard load/save
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);

		TSTR GetName() { return GetString(IDS_CLASSNAME); }

		// Kernel parameter setup and handling
		long GetNFilterParams();
		TCHAR * GetFilterParamName( long nParam );
		double GetFilterParam( long nParam );
		void SetFilterParam( long nParam, double val );
		TCHAR * GetDefaultComment();

		void Update(TimeValue t, Interval& valid);


		// Actual kernel function
		double KernelFn( double x, double y );

		// Kernel functionality queries
		long GetKernelSupport();
		long GetKernelSupportY();
		bool Is2DKernel();
		bool IsVariableSz();
		void SetKernelSz( double x, double y = 0.0 );
		void GetKernelSz( double& x, double& y );
		bool IsNormalized();
		bool HasNegativeLobes();
	};


#endif
