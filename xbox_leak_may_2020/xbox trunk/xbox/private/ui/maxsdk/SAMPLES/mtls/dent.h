/**********************************************************************
 *<
	FILE:			Dent.h

	DESCRIPTION:	Dent 3D Texture map class decl.

	CREATED BY:		Suryan Stalin, on 4th April 1996

	HISTORY:		Modified from Marble.cpp by adding IPAS dent stuff
	*>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/


#ifndef __DENT__H
#define __DENT__H

#include "iparamm2.h"

//	defines
#define DENT_CLASS_ID 		0x0000218
#define NSUBTEX				2
#define NCOLS				2
#define DENT_VERSION		1

#define DENT_VERS			0xDE01
#define MTL_HDR_CHUNK		0x4000
#define DENTVERS1_CHUNK		0x4001
#define DENT_NOISE_CHUNK	0x4003
#define DENTSIZE			50.0f
#define NOISE_DIM			20    
#define FNOISE_DIM			20.0


//	C Prototypes
ClassDesc*		GetDentDesc();



class Dent: public Tex3D 
{ 
	friend class DentPostLoad;
	XYZGen *xyzGen;		   // ref #0
	static ParamDlg *xyzGenDlg;
	Texmap* subTex[NSUBTEX];  // More refs
	Interval ivalid;
	int rollScroll;
	float DentFunc(Point3 p);

	public:
		int vers;
//		int seed;
		BOOL Param1;
		Color col[NCOLS];
		float km,size;
		int	nits;
		IParamBlock2 *pblock;   // ref #1
		BOOL mapOn[NSUBTEX];
		Dent();
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		void Update(TimeValue t, Interval& valid);
		void Init();
		void Reset();
		Interval Validity(TimeValue t) { Interval v; Update(t,v); return ivalid; }

		void SetColor(int i, Color c, TimeValue t);
		void SetSize(float f, TimeValue t);
		void SetKm(float f, TimeValue t);
		void SetNits(int f, TimeValue t);
		void NotifyChanged();
		void SwapInputs(); 

		void ReadSXPData(TCHAR *name, void *sxpdata);

		// Dent noise functions
		void	InitNoise();
		float	DentNoise(float x, float y, float z);
		void	LerpColor(RGBA *c, RGBA *a, RGBA *b, float f);
		float	SmoothStep(float x0, float x1, float v);

		// Evaluate the color of map for the context.
		RGBA EvalColor(ShadeContext& sc);

		// For Bump mapping, need a perturbation to apply to a normal.
		// Leave it up to the Texmap to determine how to do this.
		Point3 EvalNormalPerturb(ShadeContext& sc);

		// Requirements
		ULONG LocalRequirements(int subMtlNum) { return xyzGen->Requirements(subMtlNum); }
		void LocalMappingsRequired(int subMtlNum, BitArray & mapreq, BitArray &bumpreq) {  
			xyzGen->MappingsRequired(subMtlNum,mapreq,bumpreq); 
			}

		// Methods to access texture maps of material
		int NumSubTexmaps() { return NSUBTEX; }
		Texmap* GetSubTexmap(int i) { return subTex[i]; }
		void SetSubTexmap(int i, Texmap *m);
		TSTR GetSubTexmapSlotName(int i);

		XYZGen *GetTheXYZGen() { return xyzGen; }

		Class_ID ClassID();
		SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
		void GetClassName(TSTR& s) { s= GetString(IDS_DS_DENT); }  
		void DeleteThis() { delete this; }	

		int NumSubs() { return 2+NSUBTEX; }  
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum) { return subNum; }

		// From ref
 		int NumRefs() { return 2+NSUBTEX; }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		RefTargetHandle Clone(RemapDir &remap = NoRemap());
		RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message );

		// IO
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
// JBW: direct ParamBlock access is added
		int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock
		BOOL SetDlgThing(ParamDlg* dlg);



};

class DentClassDesc:public ClassDesc2 
{
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading) { 	return new Dent; }
	const TCHAR *	ClassName() { return GetString(IDS_DS_DENT_CDESC); } // mjm - 2.10.99
	SClass_ID		SuperClassID() { return TEXMAP_CLASS_ID; }
	Class_ID 		ClassID();
	const TCHAR* 	Category() { return TEXMAP_CAT_3D;  }
// PW: new descriptor data accessors added.  Note that the 
//      internal name is hardwired since it must not be localized.
	const TCHAR*	InternalName() { return _T("dents"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle

};






#pragma pack(1)
struct DentState 
{
	ulong	version;
	float	size, km;
	Col24	col1, col2;
	int		nits;
};
#pragma pack()

#endif
