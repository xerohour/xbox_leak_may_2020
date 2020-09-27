/*===========================================================================*\
 | 
 |  FILE:	wM3_material.cpp
 |			Weighted Morpher for MAX R3
 |			Morph Material plugin component
 | 
 |  AUTH:   Harry Denholm
 |			Copyright(c) Kinetix 1998
 |			All Rights Reserved.
 |
 |  HIST:	Started 21-12-98
 | 
\*===========================================================================*/


#include "wM3.h"


extern HINSTANCE hInstance;

class M3MatClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading) {return new M3Mat(loading);}
	const TCHAR *	ClassName() {return GetString(IDS_MORPHMTL);}
	SClass_ID		SuperClassID() {return MATERIAL_CLASS_ID;}
	Class_ID 		ClassID() {return M3MatClassID;}
	const TCHAR* 	Category() {return _T("");}
	};
static M3MatClassDesc M3MatCD;
ClassDesc* GetM3MatDesc() {return &M3MatCD;}




M3Mat::M3Mat(BOOL loading)
	{	
	pblock = NULL;
	dlg = NULL;
	morphp = NULL;
	listSel = 0;

	int i;
	
	for(i=0;i<101;i++)
	{
		mTex[i] = NULL;
	}

	ivalid.SetEmpty();

	for (i=0; i<100; i++) 
		mapOn[i] = 1;

	if (!loading) 
		Reset();
	}

void M3Mat::Reset()
	{

//		char s[25];

		for(int i=0;i<100;i++)
		{
			DeleteReference(i);
			mTex[i] = NULL;
	//		ReplaceReference(i,NewDefaultStdMat());
	//		sprintf(s,GetString(IDS_MTL_CNAME),i+1);
	//		mTex[i]->SetName(s);
		}

		ReplaceReference(100,NewDefaultStdMat());
		mTex[100]->SetName(GetString(IDS_MTL_BASE));


	ParamBlockDescID *descVer = new ParamBlockDescID[101];

	for(int x=0;x<100;x++){

		ParamBlockDescID add;

		add.type=TYPE_FLOAT;

		add.user=NULL;

		add.animatable=TRUE;

		add.id=x;

	 descVer[x] = add;

	}

	ParamBlockDescID add;
	add.type=TYPE_INT;
	add.user=NULL;
	add.animatable=FALSE;
	add.id=x;
	descVer[x] = add;

	IParamBlock *pblock = (IParamBlock*)CreateParameterBlock(descVer,101,1);

	ReplaceReference(101,pblock);	
	//ReplaceReference(102,NULL);

	delete [] descVer;

	pblock->SetValue(100,0,0);

	}


static Color black(0,0,0);

Color M3Mat::GetAmbient(int mtlNum, BOOL backFace) { 
	return mTex[100]?mTex[100]->GetAmbient(mtlNum,backFace):black;
	}		
Color M3Mat::GetDiffuse(int mtlNum, BOOL backFace){ 
	return mTex[100]?mTex[100]->GetDiffuse(mtlNum,backFace):black;
	}				
Color M3Mat::GetSpecular(int mtlNum, BOOL backFace){
	return mTex[100]?mTex[100]->GetSpecular(mtlNum,backFace):black;
	}		
float M3Mat::GetXParency(int mtlNum, BOOL backFace) {
	return mTex[100]?mTex[100]->GetXParency(mtlNum,backFace):0.0f;
	}
float M3Mat::GetShininess(int mtlNum, BOOL backFace) {
	return mTex[100]?mTex[100]->GetXParency(mtlNum,backFace):0.0f;
	}		
float M3Mat::GetShinStr(int mtlNum, BOOL backFace) {
	return mTex[100]?mTex[100]->GetXParency(mtlNum,backFace):0.0f;
	}
float M3Mat::WireSize(int mtlNum, BOOL backFace) {
	return mTex[100]?mTex[100]->WireSize(mtlNum,backFace):0.0f;
	}
		
ParamDlg* M3Mat::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp)
	{
	dlg = new M3MatDlg(hwMtlEdit, imp, this);
	return dlg;
	}



void M3Mat::Shade(ShadeContext& sc) {
	int i; 

	TimeValue t = sc.CurTime();
	Interval valid = FOREVER;

	pblock->GetValue(100,t,i,FOREVER);

	Mtl *sm1 = mTex[100];

	// handle no base mat
	if(!sm1) 
	{
		sc.ResetOutput();
		sc.out.c = black;
		sc.out.t = black;
		return;
	}

	if(i==0||(i==1&&inRender))
	{

		ShadeOutput	sDatabase[100];
		float u[100];

		for( i=0;i<100;i++)
		{
			pblock->GetValue(i,t,u[i],valid);

			if(mTex[i]!=NULL&&u[i]!=0&&mapOn[i])
			{
				Mtl *comb = mTex[i];
				comb->Shade(sc);
				sDatabase[i] = sc.out;
				sc.ResetOutput();
			}
		}

		sc.ResetOutput();
		sm1->Shade(sc);

		for( i=0;i<100;i++)
		{
			if(mTex[i]!=NULL&&u[i]!=0&&mapOn[i])
			{
				float mI = u[i]/100.0f;

				// the old 'mix' fn that doesn't work for >2 mtls
				//sc.out.MixIn(sDatabase[i],1.0f-mI);

				float s = mI;
				sc.out.flags |= sDatabase[i].flags;
				sc.out.c = sc.out.c+ (s*sDatabase[i].c); 
				sc.out.t = sc.out.t+ (s*sDatabase[i].t); 

			}
		}

	}
	else sm1->Shade(sc);
}



void M3Mat::Update(TimeValue t, Interval& valid)
	{	
	ivalid = FOREVER;
	for(int i=0;i<101;i++)
	{
		if (mTex[i]) mTex[i]->Update(t,valid);
	}
	valid &= ivalid;
	}

Interval M3Mat::Validity(TimeValue t)
	{
	Interval valid = FOREVER;
	float f;
	int i;

	for(i=0;i<101;i++)
	{
		if (mTex[i]) valid &= mTex[i]->Validity(t);
	}
	for(i=0;i<101;i++)
	{
		pblock->GetValue(i,t,f,valid);
	}

	return valid;
	}




/*===========================================================================*\
 | Subanims and References setup
\*===========================================================================*/

Animatable* M3Mat::SubAnim(int i)
	{
	if(i<101) return mTex[i];
	return NULL;
	}

TSTR M3Mat::SubAnimName(int i)
	{
	 return GetSubMtlSlotName(i);
	}

RefTargetHandle M3Mat::GetReference(int i)
	{
	if(i<101) return mTex[i];
	if(i==101) return pblock;
	if(i==102) return morphp;
	return NULL;
	}

void M3Mat::SetReference(int i, RefTargetHandle rtarg)
	{
	if(i<101) mTex[i] = (Mtl*)rtarg;
	if(i==101) pblock = (IParamBlock*)rtarg;
	if(i==102) morphp = (MorphR3*)rtarg;
	}



/*===========================================================================*\
 | Duplicate myself
\*===========================================================================*/

RefTargetHandle M3Mat::Clone(RemapDir &remap)
	{
	M3Mat *mtl = new M3Mat(FALSE);
	*((MtlBase*)mtl) = *((MtlBase*)this);

	int i;

	for(i=0;i<101;i++)
	{
		if (mTex[i]) mtl->ReplaceReference(i,remap.CloneRef(mTex[i]));
	}

	for (i=0; i<100; i++)
		mtl->mapOn[i] = mapOn[i];

	//mtl->morphp = morphp;
	mtl->ReplaceReference(101,remap.CloneRef(pblock));
	mtl->ReplaceReference(102,remap.CloneRef(morphp));

	mtl->obName = obName;

	return (RefTargetHandle)mtl;
	}



/*===========================================================================*\
 | NotifyRefChanged
\*===========================================================================*/

RefResult M3Mat::NotifyRefChanged(
		Interval changeInt, 
		RefTargetHandle hTarget, 
		PartID& partID, 
		RefMessage message)
	{
	switch (message) {
		case REFMSG_CHANGE:
			if (dlg && dlg->theMtl==this) dlg->Invalidate();
			break;
				
		case REFMSG_GET_PARAM_DIM: {
			GetParamDim *gpd = (GetParamDim*)partID;
			gpd->dim = stdPercentDim;
			return REF_STOP; 
			}

		case REFMSG_GET_PARAM_NAME: {
			GetParamName *gpn = (GetParamName*)partID;
			char s[50];
			sprintf(s,GetString(IDS_MTL_CNAME),gpn->index);
			gpn->name = s;
			return REF_STOP; 
			}
		}
	return REF_SUCCEED;
	}



/*===========================================================================*\
 | Displacement support
\*===========================================================================*/

float M3Mat::EvalDisplacement(ShadeContext& sc) {

	int i; 

	TimeValue t = sc.CurTime();
	Interval valid = FOREVER;

	pblock->GetValue(100,t,i,FOREVER);

	Mtl *sm1 = mTex[100];

	int counter = 0;
	float final = 0.0f;

	// handle no base mat
	if(sm1==NULL) 
	{
		return 0.0f;
	}

	if(i==0||(i==1&&inRender))
	{

		float u[100];

		for( i=0;i<100;i++)
		{
			pblock->GetValue(i,t,u[i],valid);

			if(mTex[i]!=NULL&&u[i]!=0&&mapOn[i])
			{
				Mtl *comb = mTex[i];
				float mI = u[i]/100.0f;
				final += (comb->EvalDisplacement(sc)*mI);
				counter++;
			}
		}

		float tF = final;
		if(counter>0) tF /= (float)counter;

		return final;

	}
	else 
	{
		return sm1->EvalDisplacement(sc);
	}

}

Interval M3Mat::DisplacementValidity(TimeValue t) 
{
	
	Interval iv; iv.SetInfinite();

	Mtl *sm1 = mTex[100];
	if(sm1) iv &= sm1->DisplacementValidity(t);

	for( int i=0;i<100;i++)
	{
		if(mTex[i]!=NULL&&mapOn[i])
		{
			Mtl *comb = mTex[i];
			iv &= comb->DisplacementValidity(t);
		}
	}

	return iv;	
} 



/*===========================================================================*\
 | Loading and Saving of material data
\*===========================================================================*/

#define MTL_HDR_CHUNK 0x4000
#define MAPOFF_CHUNK 0x1000

IOResult M3Mat::Save(ISave *isave) { 
	IOResult res;
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK) return res;
	isave->EndChunk();

	for (int i=0; i<100; i++) {
		if (mapOn[i]==0) {
			isave->BeginChunk(MAPOFF_CHUNK+i);
			isave->EndChunk();
			}
		}

	return IO_OK;
	}	
	  

IOResult M3Mat::Load(ILoad *iload) { 
	int id;
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(id = iload->CurChunkID())  {
			case MTL_HDR_CHUNK:
				res = MtlBase::Load(iload);
				break;
			}

		for(int i=0;i<100;i++)
		{
			if(id==MAPOFF_CHUNK+i) mapOn[id-MAPOFF_CHUNK] = 0;
		}

		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
	}