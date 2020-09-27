 /**********************************************************************
 
	FILE: CommandModes.cpp

	DESCRIPTION:  Bones def varius command modes
				  Plus scripter access

	CREATED BY: Peter Watje

	HISTORY: 8/5/98




 *>	Copyright (c) 1998, All Rights Reserved.
 **********************************************************************/


#include "mods.h"
#include "iparamm.h"
#include "shape.h"
#include "spline3d.h"
#include "splshape.h"
#include "linshape.h"

// This uses the linked-list class templates
#include "linklist.h"
#include "decomp.h"
#include "bonesdef.h"

#include "Maxscrpt.h"
#include "Strings.h"
#include "arrays.h"
#include "3DMath.h"
#include "Numbers.h"
#include "definsfn.h"


// Maxscript stuff
//just returns the number of bones in the system

def_struct_primitive( getNumberBones,skinOps, "GetNumberBones" );

//def_visible_primitive (getNumberBones,			"skinGetNumberBones" );

//just returns the number of vertice in the system
def_struct_primitive (getNumberVertices,skinOps,	"GetNumberVertices" );
//skinGetVertexWeightCount vertexid
//returns the number of bones influencing that vertex
def_struct_primitive (getVertexWeightCount,	skinOps, "GetVertexWeightCount" );
//skinGetBoneName boneID
//just returns the name of the bone
def_struct_primitive (getBoneName, skinOps,			"GetBoneName" );


//skinGetVertexWeight vertexid nthbone
//returns the inlfuence of the nth bone affecting that vertex
def_struct_primitive (getVertexWeight,skinOps,		"GetVertexWeight" );
//skinGetVertexWeightBoneID vertexid nthbone
//returns the bone id of the nth bone affecting that vertex
def_struct_primitive (getVertexWeightBoneID,skinOps,	"GetVertexWeightBoneID" );


//skinSelectVertices number/array/bitarray
//selects the vertices specified
def_struct_primitive (selectSkinVerts, skinOps,			"SelectVertices" );

//skinSetVertexWeights VertexID BoneID Weights
//assigns vertex to BoneID with Weight n
//it does not erase any previous weight info
//BoneID and Weights can be arrays or just numbers but if they are arrays they need to be the same length
def_struct_primitive (setVertWeights,skinOps,				"SetVertexWeights" );
//skinReplaceVertexWeights VertexID BoneID Weights
//assigns vertex to BoneID with Weight n
//it erases any previous bone weight info that vertex before assignment
//BoneID and Weights can be arrays or just numbers but if they are arrays they need to be the same length
def_struct_primitive (replaceVertWeights,skinOps,			"ReplaceVertexWeights" );

//skinIsVertexModified vertID
//just returns if the vertex has been modified
def_struct_primitive (isVertexModified, skinOps,			"IsVertexModified" );


//skinSelectBone BoneID
//selects that bone
def_struct_primitive (selectBone, skinOps,			"SelectBone" );
//GetSelectedBone 
//get the current selected bone
def_struct_primitive (getSelectedBone, skinOps,			"GetSelectedBone" );


//getNumberCrossSections boneID
//returns the number of cross sections for that bone
def_struct_primitive (getNumberCrossSections, skinOps,			"GetNumberCrossSections" );

//getinnerradius boneid crossSectionID
//returns the inner crossscetion radius
def_struct_primitive (getInnerRadius, skinOps,			"GetInnerRadius" );
//getOuterRadius boneid crossSectionID
//returns the inner crossscetion radius
def_struct_primitive (getOuterRadius, skinOps,			"GetOuterRadius" );

//setinnerradius boneid crossSectionID radius
//sets the inner radius of a cross section
def_struct_primitive (setInnerRadius, skinOps,			"SetInnerRadius" );
//setOuterRadius boneid crossSectionID radius
//sets the outer radius of a cross section
def_struct_primitive (setOuterRadius, skinOps,			"SetOuterRadius" );



//IsVertexSelected vertID
//just returns if the vertex has been selected
def_struct_primitive (isVertexSelected, skinOps,		"IsVertexSelected" );




#define get_bonedef_mod()																\
	Modifier *mod = arg_list[0]->to_modifier();										\
	Class_ID id = mod->ClassID();													\
	if ( id != Class_ID(9815843,87654) )	\
		throw RuntimeError(GetString(IDS_PW_NOT_BONESDEF_ERROR), arg_list[0]);			\
	BonesDefMod *bmod = (BonesDefMod*)mod;			

static int 
check_bone_index(BonesDefMod* bmod, int index)
{
	int ct = 0;
	for (int i = 0; i < bmod->BoneData.Count(); i++)
		if (bmod->BoneData[i].Node) ct++;
	if (index < 0 || index >= ct) 
		throw RuntimeError(_T("Bone index out of range: "), Integer::intern(index + 1));
	return bmod->ConvertSelectedListToBoneID(index);
}

Value*
getNumberBones_cf(Value** arg_list, int count)
{
	check_arg_count(getNumberBones, 1, count);
	get_bonedef_mod();
	int ct = 0;
	for (int i = 0; i < bmod->BoneData.Count(); i++)
		if (bmod->BoneData[i].Node) ct++;

//	int ct = bmod->BoneData.Count();
	return Integer::intern(ct);	
}


Value*
getNumberVertices_cf(Value** arg_list, int count)
{
	check_arg_count(getNumberVertices, 1, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	int ct = 0;
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		ct = bmd->VertexData.Count();
		}
	return Integer::intern(ct);	
}

Value*
getVertexWeightCount_cf(Value** arg_list, int count)
{
	check_arg_count(getVertexWeightCount, 2, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	int ct = 0;
	int index = arg_list[1]->to_int()-1;
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		if (index >= bmd->VertexData.Count() ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
		if (index < 0 ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);

		ct = bmd->VertexData[index]->d.Count();
		}
	return Integer::intern(ct);	
}


Value*
getBoneName_cf(Value** arg_list, int count)
{
	check_arg_count(getVertexWeightCount, 3, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	int ct = 0;
	int index = arg_list[1]->to_int()-1;
	int listName = arg_list[2]->to_int();
	index = check_bone_index(bmod, index);

	if (bmod->BoneData[index].Node != NULL)
		{
		if (listName)
			{
			Class_ID bid(BONE_CLASS_ID,0);
			ObjectState os = bmod->BoneData[index].Node->EvalWorldState(bmod->RefFrame);
			if (( os.obj->ClassID() == bid) && (bmod->BoneData[index].name.Length()) )
				{
				return new String((TCHAR*)bmod->BoneData[index].name);			
				}
			else return new String((TCHAR*)bmod->BoneData[index].Node->GetName()); 
			
			}
		else return new String((TCHAR*)bmod->BoneData[index].Node->GetName()); 
		}
	else return &undefined;

}



Value*
getVertexWeight_cf(Value** arg_list, int count)
{
	check_arg_count(getVertexWeight, 3, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	float ct = 0.0f;
	int vindex = arg_list[1]->to_int()-1;
	int subindex = arg_list[2]->to_int()-1;
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		if (vindex >= bmd->VertexData.Count() ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
		if (subindex >= bmd->VertexData[vindex]->d.Count() ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_SUBVERTEX_COUNT), arg_list[0]);
		if (vindex < 0  ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
		if (subindex < 0 ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_SUBVERTEX_COUNT), arg_list[0]);

		ct = bmod->RetrieveNormalizedWeight(bmd,vindex,subindex);
		}
	return Float::intern(ct);	
}

Value*
getVertexWeightBoneID_cf(Value** arg_list, int count)
{
	check_arg_count(getVertexWeightBoneID, 3, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	int ct = 0;
	int vindex = arg_list[1]->to_int()-1;
	int subindex = arg_list[2]->to_int()-1;
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		if (vindex >= bmd->VertexData.Count() ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
		if (subindex >= bmd->VertexData[vindex]->d.Count() ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_SUBVERTEX_COUNT), arg_list[0]);
		if (vindex < 0  ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
		if (subindex < 0 ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_SUBVERTEX_COUNT), arg_list[0]);

		ct = bmd->VertexData[vindex]->d[subindex].Bones;
//watje 10-26-99 Skin fix 185932
		ct = bmod->ConvertSelectedBoneToListID(ct)+1;
		}
	return Integer::intern(ct);	
}




Value*
selectSkinVerts_cf(Value** arg_list, int count)
{
	check_arg_count(selectSkinVerts, 2, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	int ct = 0;
	Value* ival = arg_list[1];

//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		bmd->selected.ClearAll();

		int index;
		if (is_number(ival))   // single index
			{
			index = ival->to_int()-1;
			if ((index <0) || (index >= bmd->selected.GetSize()) )
				throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
			else bmd->selected.Set(index,TRUE);
			}

		else if (is_array(ival))   // array of indexes
			{
			Array* aval = (Array*)ival;
			for (int i = 0; i < aval->size; i++)
				{
				ival = aval->data[i];
				if (is_number(ival))   // single index
					{
					index = ival->to_int()-1;
					if ((index <0) || (index >= bmd->selected.GetSize()))
						throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
					else bmd->selected.Set(index,TRUE);

					}
				}

			}
		else if (is_BitArrayValue(ival))   // array of indexes
			{
			BitArrayValue *list = (BitArrayValue *) ival;
			for (int index = 0; index < list->bits.GetSize(); index++)
				{
				if (list->bits[index])
					{
					if ((index <0) || (index >= bmd->selected.GetSize()))
						throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
					else bmd->selected.Set(index,TRUE);
					}
				}

			}



		}

	bmod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
	bmod->ip->RedrawViews(bmod->ip->GetTime());


	return &ok;	
}


Value*
setVertWeights_cf(Value** arg_list, int count)
{
	check_arg_count(setVertWeights, 4, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	int ct = 0;

	Value* vertval = arg_list[1];
	Value* bonesval = arg_list[2];
	Value* weightsval = arg_list[3];

	int vertID;
	if (!is_number(vertval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else vertID = vertval->to_int()-1;


//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		if (vertID >= bmd->VertexData.Count() ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);

//		bmd->selected.ClearAll();

//		int index;
		if (is_number(weightsval) && is_number(bonesval))  // single index
			{
			int boneID = bonesval->to_int()-1;
			boneID = check_bone_index(bmod, boneID);

			float weight = weightsval->to_float();
			if (weight < 0.0f ) weight = 0.0f;
			if (weight > 1.0f ) weight = 1.0f;
			if (bmod->BoneData[boneID].Node != NULL)
				{
//				bmd->VertexData[vertID]->d.ZeroCount();
				bmod->SetVertex(bmd,vertID, boneID, weight);
				}
			}

		else if (is_array(weightsval) && is_array(bonesval))   // array of indexes
			{

			Array* wval = (Array*)weightsval;
			Array* bval = (Array*)bonesval;
			if (wval->size != bval->size) throw RuntimeError(GetString(IDS_PW_WEIGHT_BONE_COUNT), arg_list[0]);

//			bmd->VertexData[vertID]->d.ZeroCount();
			Tab<int> b;
			Tab<float> v;
			for (int i = 0; i < wval->size; i++)
				{
				Value *boneval = bval->data[i];
				Value *weightval = wval->data[i];
				if ( (is_number(boneval)) && (is_number(weightval)))  // single index
					{
					int boneID = boneval->to_int()-1;
//watje 10-26-99 Skin fix 185932
//					boneID = bmod->ConvertSelectedListToBoneID(boneID);
					boneID = check_bone_index(bmod, boneID);

					float weight = weightval->to_float();
					if (weight < 0.0f ) weight = 0.0f;
					if (weight > 1.0f ) weight = 1.0f;
					if (bmod->BoneData[boneID].Node != NULL)
						{
						b.Append(1,&boneID,1);
						v.Append(1,&weight,1);
						}

					}
				}
			bmod->SetVertices(bmd,vertID, b, v);


			}



		}
//watje 9-7-99  198721 
	bmod->Reevaluate(TRUE);
	bmod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
	bmod->ip->RedrawViews(bmod->ip->GetTime());


	return &ok;	
}


Value*
replaceVertWeights_cf(Value** arg_list, int count)
{
	check_arg_count(replaceVertWeights, 4, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();
	int ct = 0;

	Value* vertval = arg_list[1];
	Value* bonesval = arg_list[2];
	Value* weightsval = arg_list[3];

	int vertID;
	if (!is_number(vertval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else vertID = vertval->to_int()-1;


//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		if (vertID >= bmd->VertexData.Count() ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);

//		bmd->selected.ClearAll();

//		int index;
		if (is_number(weightsval) && is_number(bonesval))  // single index
			{
			int boneID = bonesval->to_int()-1;
			boneID = check_bone_index(bmod, boneID);
			float weight = weightsval->to_float();
			if (weight < 0.0f ) weight = 0.0f;
			if (weight > 1.0f ) weight = 1.0f;
			if (bmod->BoneData[boneID].Node != NULL)
				{
				bmd->VertexData[vertID]->d.ZeroCount();
				bmod->SetVertex(bmd,vertID, boneID, weight);
				}
			}

		else if (is_array(weightsval) && is_array(bonesval))   // array of indexes
			{

			Array* wval = (Array*)weightsval;
			Array* bval = (Array*)bonesval;
			if (wval->size != bval->size) throw RuntimeError(GetString(IDS_PW_WEIGHT_BONE_COUNT), arg_list[0]);
			bmd->VertexData[vertID]->d.ZeroCount();

			Tab<int> b;
			Tab<float> v;

			bmd->VertexData[vertID]->d.ZeroCount();

			for (int i = 0; i < wval->size; i++)
				{
				Value *boneval = bval->data[i];
				Value *weightval = wval->data[i];
				if ( (is_number(boneval)) && (is_number(weightval)))  // single index
					{
					int boneID = boneval->to_int()-1;
					boneID = check_bone_index(bmod, boneID);
					float weight = weightval->to_float();
					if (weight < 0.0f ) weight = 0.0f;
					if (weight > 1.0f ) weight = 1.0f;
					if (bmod->BoneData[boneID].Node != NULL)
						{
						b.Append(1,&boneID,1);
						v.Append(1,&weight,1);
//						bmod->SetVertex(bmd,vertID, boneID, weight);
						}

					}
				}
			bmod->SetVertices(bmd,vertID, b, v);


			}



		}
//watje 9-7-99  198721 
	bmod->Reevaluate(TRUE);
	bmod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
	bmod->ip->RedrawViews(bmod->ip->GetTime());


	return &ok;	
}


Value*
isVertexModified_cf(Value** arg_list, int count)
{
	check_arg_count(isVertexModified, 2, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();

	int ct = 0;

	Value* vertval = arg_list[1];

	int vertID;
	if (!is_number(vertval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else vertID = vertval->to_int()-1;


//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		if (vertID >= bmd->VertexData.Count() ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
		if (vertID < 0 ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
		ct = bmd->VertexData[vertID]->modified;
		}
	return Integer::intern(ct);	


}


Value*
isVertexSelected_cf(Value** arg_list, int count)
{
	check_arg_count(isVertexModified, 2, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();

	int ct = 0;

	Value* vertval = arg_list[1];

	int vertID;
	if (!is_number(vertval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else vertID = vertval->to_int()-1;


//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		if (vertID >= bmd->selected.GetSize() ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
		if (vertID < 0 ) throw RuntimeError(GetString(IDS_PW_EXCEEDED_VERTEX_COUNT), arg_list[0]);
		ct = bmd->selected[vertID];
		}
	return Integer::intern(ct);	


}


Value*
selectBone_cf(Value** arg_list, int count)
{
	check_arg_count(selectBone, 2, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();

	int ct = 0;

	Value* vertval = arg_list[1];

	int boneID;
	if (!is_number(vertval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else boneID = vertval->to_int()-1;
	boneID = check_bone_index(bmod, boneID);


//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		SendMessage(GetDlgItem(bmod->hParam,IDC_LIST1),
					LB_SETCURSEL ,boneID,0);
		bmod->ModeBoneIndex = boneID;
//		bmod->reevaluate =TRUE;
		bmod->UpdatePropInterface();

		bmod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);

//		SendMessage(GetDlgItem(bmod->hParam,IDC_LIST1),
//					WM_COMMAND ,MAKEWPARAM(IDC_LIST1,LBN_SELCHANGE),NULL);
		bmod->ip->RedrawViews(bmod->ip->GetTime());

		}

	return &ok;	

}


Value*
getSelectedBone_cf(Value** arg_list, int count)
{
	check_arg_count(getSelectedBone, 1, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();

	int ct = 0;

	ct = bmod->ModeBoneIndex+1;

	return Integer::intern(ct);		

}


Value*
getNumberCrossSections_cf(Value** arg_list, int count)
{
	check_arg_count(getNumberCrossSections, 2, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();

	int ct = 0;

	Value* vertval = arg_list[1];

	int boneID;
	if (!is_number(vertval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else boneID = vertval->to_int()-1;
	boneID = check_bone_index(bmod, boneID);


//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		if (bmod->BoneData[boneID].Node)
			ct = bmod->BoneData[boneID].CrossSectionList.Count();
		}

	return Integer::intern(ct);			

}


Value*
getInnerRadius_cf(Value** arg_list, int count)
{
	check_arg_count(getInnerRadius, 3, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();

	float w = 0;

	Value* vertval = arg_list[1];
	Value* crossval = arg_list[2];

	int boneID;
	int crossID;
	if (!is_number(vertval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else boneID = vertval->to_int()-1;
	boneID = check_bone_index(bmod, boneID);

	if (!is_number(crossval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else crossID = crossval->to_int()-1;


//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		if (bmod->BoneData[boneID].Node)
			{
			if ( (crossID < bmod->BoneData[boneID].CrossSectionList.Count()) &&
				 (crossID >= 0)
				 )
				{
				Interval v;
				bmod->BoneData[boneID].CrossSectionList[crossID].InnerControl->GetValue(0,&w,v);
				}
			else throw RuntimeError(GetString(IDS_PW_EXCEEDED_CROSS_COUNT), arg_list[0]);
			}
		}

	return Float::intern(w);			

}

Value*
getOuterRadius_cf(Value** arg_list, int count)
{
	check_arg_count(getOuterRadius, 3, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();

	float w = 0;

	Value* vertval = arg_list[1];
	Value* crossval = arg_list[2];

	int boneID;
	int crossID;
	if (!is_number(vertval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else boneID = vertval->to_int()-1;
	boneID = check_bone_index(bmod, boneID);

	if (!is_number(crossval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else crossID = crossval->to_int()-1;


//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		if (bmod->BoneData[boneID].Node)
			{
			if ( (crossID < bmod->BoneData[boneID].CrossSectionList.Count()) &&
				 (crossID >= 0)
				 )
				{
				Interval v;
				bmod->BoneData[boneID].CrossSectionList[crossID].OuterControl->GetValue(0,&w,v);
					
				}
			else throw RuntimeError(GetString(IDS_PW_EXCEEDED_CROSS_COUNT), arg_list[0]);
			}
		}

	return Float::intern(w);			

}


Value*
setInnerRadius_cf(Value** arg_list, int count)
{
	check_arg_count(setInnerRadius, 4, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();

	float w = 0;

	Value* vertval = arg_list[1];
	Value* crossval = arg_list[2];
	Value* radiusval = arg_list[3];

	int boneID;
	int crossID;
	float radius;
	if (!is_number(vertval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else boneID = vertval->to_int()-1;
	boneID = check_bone_index(bmod, boneID);

	if (!is_number(crossval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else crossID = crossval->to_int()-1;

	if (!is_number(radiusval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else radius = radiusval->to_float();


//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		if (bmod->BoneData[boneID].Node)
			{
			if ( (crossID < bmod->BoneData[boneID].CrossSectionList.Count()) &&
				 (crossID >= 0)
				 )
				{
				Interval v;
				bmod->BoneData[boneID].CrossSectionList[crossID].InnerControl->SetValue(0,&radius);
					
				}
			else throw RuntimeError(GetString(IDS_PW_EXCEEDED_CROSS_COUNT), arg_list[0]);
			}
		}

	if (bmod->ModeBoneIndex != boneID)
//watje 9-7-99  198721 
		bmod->Reevaluate(TRUE);

	bmod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
	bmod->ip->RedrawViews(bmod->ip->GetTime());


	return &ok;			

}

Value*
setOuterRadius_cf(Value** arg_list, int count)
{
	check_arg_count(setOuterRadius, 4, count);
	get_bonedef_mod();
//get first mod context not sure how this will work if multiple instanced are selected
//maybe should use COREinterface instead of local interface ????
	if ( !bmod->ip ) throw RuntimeError(GetString(IDS_PW_SKIN_NOT_SELECTED), arg_list[0]);

	ModContextList mcList;		
	INodeTab nodes;

	bmod->ip->GetModContexts(mcList,nodes);
	int objects = mcList.Count();

	float w = 0;

	Value* vertval = arg_list[1];
	Value* crossval = arg_list[2];
	Value* radiusval = arg_list[3];

	int boneID;
	int crossID;
	float radius;
	if (!is_number(vertval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else boneID = vertval->to_int()-1;
	boneID = check_bone_index(bmod, boneID);

	if (!is_number(crossval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else crossID = crossval->to_int()-1;

	if (!is_number(radiusval)) 
		{
		throw RuntimeError(GetString(IDS_PW_INVALID_DATA), arg_list[0]);
		}
	else radius = radiusval->to_float();


//	BitArray selList = arg_list[1]->to_bitarray();
	if (objects != 0)
		{
		BoneModData *bmd = (BoneModData*)mcList[0]->localData;
		if (bmod->BoneData[boneID].Node)
			{
			if ( (crossID < bmod->BoneData[boneID].CrossSectionList.Count()) &&
				 (crossID >= 0)
				 )
				{
				Interval v;
				bmod->BoneData[boneID].CrossSectionList[crossID].OuterControl->SetValue(0,&radius);
				}
			else throw RuntimeError(GetString(IDS_PW_EXCEEDED_CROSS_COUNT), arg_list[0]);
			}
		}

	if (bmod->ModeBoneIndex != boneID)
//watje 9-7-99  198721 
		bmod->Reevaluate(TRUE);

	bmod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
	bmod->ip->RedrawViews(bmod->ip->GetTime());


	return &ok;			

}



/*-------------------------------------------------------------------*/
/*																	*/
/*				Create Cross Section Command Mode					*/
/*																	*/
/*-------------------------------------------------------------------*/

HCURSOR CreateCrossSectionMouseProc::GetTransformCursor() 
        { 
        static HCURSOR hCur = NULL;

        if ( !hCur ) {
                hCur = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_SEGREFINECUR)); 
                }

        return hCur; 
        }



BOOL CreateCrossSectionMouseProc::HitTest( 
                ViewExp *vpt, IPoint2 *p, int type, int flags )

        {

//do a poly hit test
        int savedLimits, res = 0;
        GraphicsWindow *gw = vpt->getGW();


//		float scale = 1.0f;



        HitRegion hr;
        MakeHitRegion(hr,type, 1,8,p);
        gw->setHitRegion(&hr);
        gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);
        gw->setTransform(Matrix3(1));
        gw->clearHitCode();

        gw->setColor(LINE_COLOR, 1.0f,1.0f,1.0f);


		if (mod->BoneData[mod->ModeBoneIndex].flags & BONE_SPLINE_FLAG)
			{
			ShapeObject *pathOb = NULL;
			ObjectState os = mod->BoneData[mod->ModeBoneIndex].Node->EvalWorldState(mod->ip->GetTime());
			pathOb = (ShapeObject*)os.obj;
			Matrix3 tm = mod->BoneData[mod->ModeBoneIndex].Node->GetObjectTM(mod->ip->GetTime());
	        Point3 plist[2];
			SplineU = -1.0f;
			float u = 0.0f;
			for (int spid = 0; spid < 100; spid++)
				{
				plist[0] = pathOb->InterpCurve3D(mod->ip->GetTime(), 0,u) * tm;
				plist[1] = pathOb->InterpCurve3D(mod->ip->GetTime(), 0,u+0.01f) * tm;
				u += 0.01f;
		        gw->polyline(2, plist, NULL, NULL, 0);
			    if (gw->checkHitCode()) 
					{
			        res = TRUE;
				    gw->clearHitCode();
					a = plist[0];
					b = plist[1];
					SplineU = u;
					spid = 100;
					}
				gw->clearHitCode();
		
				}

			}
		else
			{
	        Point3 plist[2];
		    plist[0] = mod->Worldl1;
			plist[1] = mod->Worldl2;

	        gw->polyline(2, plist, NULL, NULL, 0);
		    if (gw->checkHitCode()) {
			        res = TRUE;
//              vpt->CtrlLogHit(NULL,gw->getHitDistance(),0,0);

				    gw->clearHitCode();
					}
			gw->clearHitCode();
			}

        gw->setRndLimits(savedLimits);

        return res;


        }

int CreateCrossSectionMouseProc::proc(
                        HWND hwnd, 
                        int msg, 
                        int point, 
                        int flags, 
                        IPoint2 m )
        {
        ViewExp *vpt = iObjParams->GetViewport(hwnd);   
        int res = TRUE;
		if ( !mod->ip ) return FALSE;


			switch ( msg ) {
                case MOUSE_PROPCLICK:
                        iObjParams->SetStdCommandMode(CID_OBJMOVE);
                        break;

                case MOUSE_POINT:
                        if(HitTest(vpt,&m,HITTYPE_POINT,0) ) {

//transfrom mouse point to world
								float u;
								if (mod->BoneData[mod->ModeBoneIndex].flags & BONE_SPLINE_FLAG)
									GetHit(u);
								else u = mod->GetU(vpt,mod->Worldl1,mod->Worldl2, m);
									
								if (u <= 0.0f) u = 0.0001f;
								if (u >= 1.0f) u = 0.9999f;
                                mod->AddCrossSection(u);
                                BOOL s = FALSE;

//watje 9-7-99  198721 
                                mod->Reevaluate(TRUE);
                                mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
                                mod->ip->RedrawViews(mod->ip->GetTime());
                                }
                        res = FALSE;
                        break;
                
                case MOUSE_FREEMOVE:
                        if ( HitTest(vpt,&m,HITTYPE_POINT,HIT_ABORTONHIT) ) {
                                SetCursor(LoadCursor(NULL,IDC_CROSS ));
                                 }
                        else {
                                SetCursor(LoadCursor(NULL,IDC_ARROW));
                                }
                        break;
                        
                }

        if ( vpt ) iObjParams->ReleaseViewport(vpt);
        return res;
        }


/*-------------------------------------------------------------------*/

void CreateCrossSectionMode::EnterMode()
        {
        mod->iCrossSectionButton->SetCheck(TRUE);
        }

void CreateCrossSectionMode::ExitMode()
        {
        mod->iCrossSectionButton->SetCheck(FALSE);
        }

void BonesDefMod::StartCrossSectionMode(int type)
        {
        if ( !ip ) return;

		if (ip->GetCommandMode() == CrossSectionMode) {
			ip->SetStdCommandMode(CID_OBJMOVE);
			return;
			}


        CrossSectionMode->SetType(type);
        ip->SetCommandMode(CrossSectionMode);
        }



/*-------------------------------------------------------------------*/
/*																	*/
/*				Paint Command Mode									*/
/*																	*/
/*-------------------------------------------------------------------*/



HCURSOR CreatePaintMouseProc::GetTransformCursor() 
        { 
        static HCURSOR hCur = NULL;

        if ( !hCur ) {
                hCur = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_SEGREFINECUR)); 
                }

        return hCur; 
        }



BOOL CreatePaintMouseProc::HitTest( 
                ViewExp *vpt, IPoint2 *p, int type, int flags, BoneModData *bmd, Object *obj)

        {

        Ray ray;
        BOOL res = FALSE;

		

        vpt->MapScreenToWorldRay((float)p->x, (float)p->y, ray);

		if (obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) return FALSE;

		
//now map it to our local space
		if (1)
			{
			ray.p   = bmd->InverseBaseTM * ray.p;
			ray.dir = VectorTransform(bmd->InverseBaseTM, ray.dir);	

			float at;
			Point3 norm;

			if (obj->IntersectRay(mod->ip->GetTime(),ray, at, norm)) 
				{
				bmd->hitPoint = ray.p + ray.dir * at;

				return TRUE;
				}

		
			}
        return FALSE;


 }




/*-------------------------------------------------------------------*/

static void BoneXORDottedLine( HWND hwnd, IPoint2 p0, IPoint2 p1 )
	{
	HDC hdc;
	hdc = GetDC( hwnd );
	SetROP2( hdc, R2_XORPEN );
	SetBkMode( hdc, TRANSPARENT );
	SelectObject( hdc, CreatePen( PS_DOT, 0, RGB(255,255,255) ) );
	MoveToEx( hdc, p0.x, p0.y, NULL );
	LineTo( hdc, p1.x, p1.y );		
	DeleteObject( SelectObject( hdc, GetStockObject( BLACK_PEN ) ) );
	ReleaseDC( hwnd, hdc );
	}

static void BoneXORDottedCircle( HWND hwnd, IPoint2 p0, float Radius )
	{
	HDC hdc;
	hdc = GetDC( hwnd );
	SetROP2( hdc, R2_XORPEN );
	SetBkMode( hdc, TRANSPARENT );
	SelectObject( hdc, CreatePen( PS_DOT, 0, RGB(255,255,255) ) );
	MoveToEx( hdc, p0.x +(int)Radius, p0.y, NULL );
	float angle = 0.0f;
	float inc = 2.0f*PI/20.f;
	IPoint2 p1;
	for (int i = 0; i < 20; i++)
		{
		angle += inc;
		p1.x = (int)(Radius * sin(angle) + Radius * cos(angle));
		p1.y = (int)(Radius * sin(angle) - Radius * cos(angle));
		LineTo( hdc, p0.x + p1.x, p0.y+p1.y );		
		}

	DeleteObject( SelectObject( hdc, GetStockObject( BLACK_PEN ) ) );
	ReleaseDC( hwnd, hdc );
	}


/*-------------------------------------------------------------------*/


BOOL CreatePaintMouseProc::BuildFalloffList( ViewExp *vpt, Tab<IPoint2> plist, 
											 BoneModData *bmd, Object *obj)

{
Box3 boundingBox;

Tab<Point3> hitList;

boundingBox.Init();

if (obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) return FALSE;

for (int pct = 0; pct < plist.Count(); pct++)
		{
        Ray ray;

		IPoint2 p = plist[pct];
        vpt->MapScreenToWorldRay((float)p.x, (float)p.y, ray);
		ray.p   = bmd->InverseBaseTM * ray.p;
		ray.dir = VectorTransform(bmd->InverseBaseTM, ray.dir);	

//now map it to our local space
		if (1)
			{

			float at;
			Point3 norm;

			if (obj->IntersectRay(mod->ip->GetTime(),ray, at, norm)) 
				{
				Point3 hitPoint;
				hitPoint = ray.p + ray.dir * at;
				hitList.Append(1,&hitPoint,1);
				boundingBox += hitPoint;					
				}
			}

		}
boundingBox.EnlargeBy(mod->Radius);
float feather, ifeather;
feather = 1.0f -  mod->Feather;
ifeather = mod->Feather;


if (hitList.Count() ==1 )
	{	
	for (int i = 0; i< bmd->VertexDataCount; i++)
		{
//		if (boundingBox.Contains(bmd->VertexData[i]->LocalPos))
		if (boundingBox.Contains(obj->GetPoint(i)))
			{
//			float dist = Length(bmd->VertexData[i]->LocalPos-hitList[0]);
			float dist = Length(obj->GetPoint(i)-hitList[0]);
			if (dist < mod->Radius)
				{
				float f;
		        if (dist < (mod->Radius*feather))
		           f = 1.0f;
			    else 
				   {
					f = 1.0f - (dist-(mod->Radius*feather))/(mod->Radius*ifeather);
					}
				mod->ComputeFalloff(f,mod->BoneData[mod->ModeBoneIndex].FalloffType);
				f = f * mod->paintStr;
				if (f > FalloffList[i]) 
					FalloffList[i] = f;
				}
			}
		}

	}
else
	{
	for (pct = 1; pct < hitList.Count(); pct++)
		{
		for (int i = 0; i< bmd->VertexDataCount; i++)
			{
//			if (boundingBox.Contains(bmd->VertexData[i]->LocalPos))
			if (boundingBox.Contains(obj->GetPoint(i)))
				{
				Point3 l1,l2;
				l1 = hitList[pct-1];
				l2 = hitList[pct];
//				float dist = Length(bmd->VertexData[i]->LocalPos-hitPoint);
				float u;
//		float dist = mod->LineToPoint(bmd->VertexData[i]->LocalPos,l1, l2,u);
				float dist = mod->LineToPoint(obj->GetPoint(i),l1, l2,u);

				if (dist < mod->Radius)
					{
					float f;
			        if (dist < (mod->Radius*feather))
			           f = 1.0f;
				    else 
					   {
						f = 1.0f - (dist-(mod->Radius*feather))/(mod->Radius*ifeather);
						}	
					mod->ComputeFalloff(f,mod->BoneData[mod->ModeBoneIndex].FalloffType);
					f = f * mod->paintStr;
					if (f > FalloffList[i]) 
						FalloffList[i] = f;

					}
				}
			}
		}

	}
 return 1;


 }



int CreatePaintMouseProc::proc(
                        HWND hwnd, 
                        int msg, 
                        int point, 
                        int flags, 
                        IPoint2 m )
        {

        ViewExp *vpt = iObjParams->GetViewport(hwnd);   
        int res = TRUE;
        float st = 1.0f;
        int flip = 0;

		ModContextList mcList;		
		INodeTab nodes;

		if (!mod->ip) return 0;

		mod->ip->GetModContexts(mcList,nodes);
		int objects = mcList.Count();

		objList.SetCount(nodes.Count());
		for (int nc = 0; nc < nodes.Count(); nc++)
			{
			ObjectState os = nodes[nc]->EvalWorldState(mod->ip->GetTime());
			objList[nc] = os.obj;
			}



		for ( int k = 0; k < objects; k++ ) 
			{

			BoneModData *bmd = (BoneModData*)mcList[k]->localData;
			INode *node = nodes[k];


	        switch ( msg ) {
                case MOUSE_PROPCLICK:
						if (mod->painting)
							theHold.Cancel();
						else iObjParams->SetStdCommandMode(CID_OBJMOVE);
						mod->painting = FALSE;
                        break;

                case MOUSE_POINT:
					if (point == 0)
						{
						mod->HoldWeights();
						mouseHitList.ZeroCount();
						mod->painting = TRUE;
						for (int i = 0; i<bmd->selected.GetSize(); i++)
							 bmd->selected.Set(i,FALSE);


						}
					else 
						{
//compute hit list
						mod->painting = FALSE;

                        st = 1.0f;
                        flip = 0;
                        if (flags & MOUSE_ALT) 
                                {
                                st = 0.0f;
                                flip = 1;
                                }
                        mod->pblock_param->GetValue(PB_RADIUS,0,mod->Radius,FOREVER);
                        mod->pblock_param->GetValue(PB_FEATHER,0,mod->Feather,FOREVER);
                        mod->pblock_param->GetValue(PB_PAINTSTR,0,mod->paintStr,FOREVER);
						FalloffList.SetCount(bmd->VertexData.Count());
						
						for (int i = 0; i < bmd->VertexData.Count(); i++)
							FalloffList[i] = -10.0f;
						BuildFalloffList( vpt, mouseHitList,bmd, objList[k]);

                        mod->IncrementVertices(bmd,mod->ModeBoneIndex,FalloffList,flip);
						for (i = 1; i < mouseHitList.Count(); i++)
							BoneXORDottedLine(hwnd, mouseHitList[i-1], mouseHitList[i]);	// Draw it!
						bmd->forceUpdate = TRUE;
						mod->AcceptWeights(TRUE);

                        mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
                        mod->ip->RedrawViews(mod->ip->GetTime());

						break;

						}


                case MOUSE_MOVE:
						res = TRUE;

						if (mouseHitList.Count() > 1)
							{
							if (mouseHitList[mouseHitList.Count()-1] != m)
								{
								if (mouseHitList.Count() > 1)
									{
									BoneXORDottedLine(hwnd, lastPoint, m);	// Draw it!
									}

		
								lastPoint = m;

						        if ( HitTest(vpt,&m,HITTYPE_CIRCLE,HIT_ABORTONHIT,bmd,objList[k]) ) 
									{

			                        mod->NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
									bmd->isHit = TRUE;
						            mod->ip->RedrawViews(mod->ip->GetTime());
	
									}
								else
									{
									if (bmd->isHit)
										{
				                        mod->NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
										bmd->isHit = FALSE;
							            mod->ip->RedrawViews(mod->ip->GetTime());
										}
									else bmd->isHit = FALSE;

									}	

								mouseHitList.Append(1,&m,1);
								}

							}
						else {
							mouseHitList.Append(1,&m,1);
							lastPoint = m;
							}



                        break;
                
                case MOUSE_FREEMOVE:

                        if ( HitTest(vpt,&m,HITTYPE_CIRCLE,HIT_ABORTONHIT,bmd,objList[k]) ) {
                                SetCursor(LoadCursor(NULL,IDC_CROSS ));
							bmd->isHit = TRUE;

                             mod->NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	                         mod->ip->RedrawViews(mod->ip->GetTime());

                             }
                        else {
                              SetCursor(LoadCursor(NULL,IDC_ARROW));

							  if (bmd->isHit)
								{
								  bmd->isHit = FALSE;
		                         mod->NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	                             mod->ip->RedrawViews(mod->ip->GetTime());
								}
	
							  else bmd->isHit = FALSE;

                              }

                        break;
                        
                }
			}

        if ( vpt ) iObjParams->ReleaseViewport(vpt);
        return res;

	return 0;
        }



/*-------------------------------------------------------------------*/

void CreatePaintMode::EnterMode()
        {
        mod->iPaintButton->SetCheck(TRUE);

		SpinnerOn(mod->hParam,IDC_FEATHERSPIN,IDC_FEATHER);
		SpinnerOn(mod->hParam,IDC_SRADIUSSPIN,IDC_SRADIUS);
		SpinnerOn(mod->hParam,IDC_PAINT_STR_SPIN2,IDC_PAINT_STR2);

		eproc.first = TRUE;		
		mod->inPaint = TRUE;
        mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);


        }

void CreatePaintMode::ExitMode()
        {
        mod->iPaintButton->SetCheck(FALSE);
		SpinnerOff(mod->hParam,IDC_FEATHERSPIN,IDC_FEATHER);
		SpinnerOff(mod->hParam,IDC_SRADIUSSPIN,IDC_SRADIUS);
		SpinnerOff(mod->hParam,IDC_PAINT_STR_SPIN2,IDC_PAINT_STR2);

		mod->inPaint = FALSE;
        mod->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);


//nuke mesh cache

        }

void BonesDefMod::StartPaintMode()
        {
        if ( !ip ) return;

//get mesh
		if (ip->GetCommandMode() == PaintMode) {
			ip->SetStdCommandMode(CID_OBJMOVE);
			return;
			}

        ip->SetCommandMode(PaintMode);
        }


//watje 9-7-99 198721 

class CacheModEnumProc : public ModContextEnumProc {
public:
	BonesDefMod *lm;
	CacheModEnumProc(BonesDefMod *l)
		{
		lm = l;
		}
private:
	BOOL proc (ModContext *mc);
};

BOOL CacheModEnumProc::proc (ModContext *mc) {
	if (mc->localData == NULL) return TRUE;

	BoneModData *bmd = (BoneModData *) mc->localData;
	bmd->CurrentCachePiece = -1;
	return TRUE;
}



/*-------------------------------------------------------------------*/
/*																	*/
/*				Select Bone Dialog Mode									*/
/*																	*/
/*-------------------------------------------------------------------*/



//This handles the output of particle info
//Hit Dialog


#define PERCENT_LENGTH	0.3f
#define PERCENT_LENGTH_CLOSED	0.1f
#define OUTER_MULT		3.5f


void DumpHitDialog::proc(INodeTab &nodeTab)

{

int nodeCount = nodeTab.Count(); 

if (nodeCount == 0) return;

for (int i=0;i<nodeTab.Count();i++)
	{


	ModContextList mcList;
	INodeTab nodes;
	eo->ip->GetModContexts(mcList,nodes);
	assert(nodes.Count());
//	Matrix3 ourTM;

//	ourTM = nodes[0]->GetObjectTM(eo->RefFrame);
//	eo->BaseTM = ourTM;

	Class_ID bid(BONE_CLASS_ID,0);




//need to add subcount for shapes also
	int subcount = 1;
	ObjectState os = nodeTab[i]->EvalWorldState(eo->RefFrame);
	if (os.obj->ClassID() == bid)  
		{
//		subcount = nodeTab[i]->NumberOfChildren();
//		if (subcount == 0) subcount = 1;
		}
	else if (os.obj->SuperClassID()==SHAPE_CLASS_ID)
		{
//get spline piecs
		}


	for (int j = 0; j < subcount; j++)
		{

//		Object *obj = nodes[0]->EvalWorldState(eo->RefFrame).obj;

		BoneDataClass t;
		t.Node = nodeTab[i];
		TCHAR title[200];
		_tcscpy(title,nodeTab[i]->GetName());


		int current=-1;
		BOOL found = FALSE;
		for (int bct = 0; bct < eo->BoneData.Count();bct++)
			{
			if (eo->BoneData[bct].Node == NULL) 
				{
				current = bct;
				found = TRUE;
				bct = eo->BoneData.Count();
				}
			}
		if (!found)
			current = eo->BoneData.Count();
		int BoneRefID = eo->GetOpenID();
		int End1RefID = eo->GetOpenID();
		int End2RefID = eo->GetOpenID();

		if (current != -1) {

			Matrix3 ntm = t.Node->GetObjectTM(eo->RefFrame);	


//append a new bone
			BoneDataClass t;
			if (!found)
				eo->BoneData.Append(t);
			eo->BoneData[current].Node = NULL;
			eo->BoneData[current].EndPoint1Control = NULL;
			eo->BoneData[current].EndPoint2Control = NULL;
		
			eo->BoneData[current].tm    = Inverse(ntm);
			eo->BoneData[current].CrossSectionList.ZeroCount();


			Point3 l1(0.0f,0.0f,0.0f),l2(0.0f,0.0f,0.0f);
		

//object is bone use its first child as the axis
			eo->BoneData[current].flags = 0;
			if (os.obj->ClassID() == bid) 
				{
				l1.x = 0.0f;
				l1.y = 0.0f;
				l1.z = 0.0f;
				l2.x = 0.0f;
				l2.y = 0.0f;
				l2.z = 0.0f;
//get child node
				INode* parent = nodeTab[i]->GetParentNode();
//				ntm = t.Node->GetObjectTM(eo->RefFrame);	
				ntm = parent->GetObjectTM(eo->RefFrame);	
				eo->BoneData[current].tm    = Inverse(ntm);

//				if (nodeTab[i]->NumberOfChildren() > 0)
				if (1)
					{
//					INode *child = nodeTab[i]->GetChildNode(j);
					Matrix3 ChildTM = nodeTab[i]->GetObjectTM(eo->RefFrame);

					_tcscpy(title,nodeTab[i]->GetName());


					l2 = l2 * ChildTM;
					l2 = l2 * Inverse(ntm); 
					Point3 Vec = (l2-l1);
					l1 += Vec * 0.1f;
					l2 -= Vec * 0.1f;
					}
				else 
					{
					l2.x = 0.0f;
					l2.y = 0.0f;
					l2.z = 50.0f;
					}
				float el1 = 0.0f,el2 = 0.0f;
				float d = Length(l2-l1);
				if (d < 0.1f) d = 10.f;
				el1 = d * PERCENT_LENGTH;



				float e_inner, e_outer;
				e_inner = el1 ;
				e_outer = e_inner *OUTER_MULT;
				eo->AddCrossSection(current, 0.0f, e_inner,e_outer);
				e_inner = el1 ;
				e_outer = e_inner *OUTER_MULT;
				eo->AddCrossSection(current, 1.0f, e_inner,e_outer);
				eo->BoneData[current].flags = BONE_BONE_FLAG;

				}
//object is bone use its first child as the axis
			else if (os.obj->SuperClassID()==SHAPE_CLASS_ID)
				{
//build distance based on spline
				eo->BoneData[current].flags = eo->BoneData[current].flags|BONE_SPLINE_FLAG;
				ShapeObject *pathOb = NULL;
				ObjectState os = nodeTab[i]->EvalWorldState(eo->RefFrame);


				BezierShape bShape;
				ShapeObject *shape = (ShapeObject *)os.obj;
				if(shape->CanMakeBezier())
//watje 9-7-99  195862 
					shape->MakeBezier(eo->RefFrame, bShape);
//					shape->MakeBezier(eo->ip->GetTime(), bShape);
				else {
					PolyShape pShape;
//watje 9-7-99  195862 
					shape->MakePolyShape(eo->RefFrame, pShape);
//					shape->MakePolyShape(eo->ip->GetTime(), pShape);
					bShape = pShape;	// UGH -- Convert it from a PolyShape -- not good!
					}

				pathOb = (ShapeObject*)os.obj;

				if (bShape.splines[0]->Closed() )
					eo->BoneData[current].flags = eo->BoneData[current].flags|BONE_SPLINECLOSED_FLAG;

//watje 9-7-99  195862 
				l1 = pathOb->InterpCurve3D(eo->RefFrame, 0, 0.0f, SPLINE_INTERP_SIMPLE);			
//				l1 = pathOb->InterpCurve3D(0, 0, 0.0f, SPLINE_INTERP_SIMPLE);			
//watje 9-7-99  195862 
				l2 = pathOb->InterpCurve3D(eo->RefFrame, 0, 1.0f, SPLINE_INTERP_SIMPLE);			
//				l2 = pathOb->InterpCurve3D(0, 0, 1.0f, SPLINE_INTERP_SIMPLE);			

				float el1 = 0.0f,el2 = 0.0f;
				float s1 = bShape.splines[0]-> SplineLength();
				if (s1< 0.1f) s1 = 10.f;
				if (bShape.splines[0]->Closed() )
					el1 = s1 * PERCENT_LENGTH_CLOSED;
				else el1 = s1 * PERCENT_LENGTH;

				float e_inner, e_outer;
				e_inner = el1;
				e_outer = e_inner *OUTER_MULT;
				eo->AddCrossSection(current, 0.0f, e_inner,e_outer);
				e_inner = el1;
				e_outer = e_inner *OUTER_MULT;
				eo->AddCrossSection(current, 1.0f, e_inner,e_outer);

//copy initial reference spline into our spline
//				SplineShape *shape = (SplineShape *)os.obj;

				eo->BoneData[current].referenceSpline = *bShape.splines[0];

				}

			else 
				{
				eo->BuildMajorAxis(nodeTab[i],l1,l2); 
				float el1 = 0.0f,el2 = 0.0f;
				float d = Length(l2-l1);
				if (d < 0.1f) d = 10.f;
				el1 = d * PERCENT_LENGTH;

				float e_inner, e_outer;
				e_inner = el1 ;
				e_outer = e_inner *OUTER_MULT;
				eo->AddCrossSection(current, 0.0f, e_inner,e_outer);
				e_inner = el1 ;
				e_outer = e_inner *OUTER_MULT;
				eo->AddCrossSection(current, 1.0f, e_inner,e_outer);

				}

			l1 = l1;// * Inverse(eo->BoneData[current].tm);
			l2 = l2;// * Inverse(eo->BoneData[current].tm);
			eo->BoneData[current].flags = eo->BoneData[current].flags|BONE_ABSOLUTE_FLAG;
			eo->BoneData[current].FalloffType = 0;

			eo->BoneData[current].BoneRefID = BoneRefID;
			eo->BoneData[current].RefEndPt1ID = End1RefID;
			eo->BoneData[current].RefEndPt2ID = End2RefID;

			eo->BoneData[current].end1Selected = FALSE;
			eo->BoneData[current].end2Selected = FALSE;


			if (os.obj->ClassID() == bid) 
				{
//get child node
				INode* parent = nodeTab[i]->GetParentNode();
				eo->ReplaceReference(BoneRefID,parent,FALSE);
				eo->BoneData[current].name = title;
				}
			else eo->ReplaceReference(BoneRefID,nodeTab[i],FALSE);


			eo->MakeRefByID(FOREVER,End1RefID,NewDefaultPoint3Controller());
			eo->MakeRefByID(FOREVER,End2RefID,NewDefaultPoint3Controller());
			eo->BoneData[current].EndPoint1Control->SetValue(0,&l1,TRUE,CTRL_ABSOLUTE);
			eo->BoneData[current].EndPoint2Control->SetValue(0,&l2,TRUE,CTRL_ABSOLUTE);


			int rsel = eo->ConvertSelectedBoneToListID(current);

			SendMessage(GetDlgItem(eo->hParam,IDC_LIST1),
				LB_INSERTSTRING,(WPARAM) rsel,(LPARAM)(TCHAR*)title);


			nodes.DisposeTemporary();
			}
		

		eo->ModeBoneIndex = current;
		eo->ModeBoneEndPoint = -1;
		eo->ModeBoneEnvelopeIndex = -1;
		eo->ModeBoneEnvelopeSubType = -1;
		SendMessage(GetDlgItem(eo->hParam,IDC_LIST1),
				LB_SETCURSEL ,current,0);

		if (eo->BoneData[eo->ModeBoneIndex].flags & BONE_LOCK_FLAG)
			eo->pblock_param->SetValue(PB_LOCK_BONE,0,1);
		else
			eo->pblock_param->SetValue(PB_LOCK_BONE,0,0);

		if (eo->BoneData[eo->ModeBoneIndex].flags & BONE_ABSOLUTE_FLAG)
			eo->pblock_param->SetValue(PB_ABSOLUTE_INFLUENCE,0,1);
		else
			eo->pblock_param->SetValue(PB_ABSOLUTE_INFLUENCE,0,0);
//watje 9-7-99  198721 
		eo->Reevaluate(TRUE);
		}
	
	}
	
if ( (eo->BoneData.Count() >0) && (eo->ip && eo->ip->GetSubObjectLevel() == 1) )
	{
	eo->EnableButtons();
	}

if (eo->BoneData[eo->ModeBoneIndex].flags & BONE_ABSOLUTE_FLAG)
	{
	eo->iAbsolute->SetCheck(FALSE);

	eo->pblock_param->SetValue(PB_ABSOLUTE_INFLUENCE,0,1);
	}
else
	{
	eo->iAbsolute->SetCheck(TRUE);
	eo->pblock_param->SetValue(PB_ABSOLUTE_INFLUENCE,0,0);
	}


if (eo->BoneData[eo->ModeBoneIndex].flags & BONE_DRAW_ENVELOPE_FLAG)
	{
	eo->pblock_param->SetValue(PB_DRAW_BONE_ENVELOPE,0,1);
	eo->iEnvelope->SetCheck(TRUE);

	}
else
	{
	eo->pblock_param->SetValue(PB_DRAW_BONE_ENVELOPE,0,0);
	eo->iEnvelope->SetCheck(FALSE);
	}

if (eo->BoneData[eo->ModeBoneIndex].FalloffType == BONE_FALLOFF_X_FLAG)
	eo->iFalloff->SetCurFlyOff(0,FALSE);
else if (eo->BoneData[eo->ModeBoneIndex].FalloffType == BONE_FALLOFF_SINE_FLAG)
	eo->iFalloff->SetCurFlyOff(1,FALSE);
else if (eo->BoneData[eo->ModeBoneIndex].FalloffType == BONE_FALLOFF_X3_FLAG)
	eo->iFalloff->SetCurFlyOff(3,FALSE);
else if (eo->BoneData[eo->ModeBoneIndex].FalloffType == BONE_FALLOFF_3X_FLAG)
	eo->iFalloff->SetCurFlyOff(2,FALSE);

eo->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
//eo->CurrentCachePiece = -1;
//watje 9-7-99  198721 
CacheModEnumProc lmdproc(eo);
eo->EnumModContexts(&lmdproc);

eo->cacheValid = FALSE;
}


int DumpHitDialog::filter(INode *node)

{

	TCHAR name1[200];
	_tcscpy(name1,node->GetName());

	node->BeginDependencyTest();
	eo->NotifyDependents(FOREVER,0,REFMSG_TEST_DEPENDENCY);
	if (node->EndDependencyTest()) 
		{		
		return FALSE;
		} 
	else 
		{
		ObjectState os = node->EvalWorldState(0);
		Class_ID bid(BONE_CLASS_ID,0);
		for (int i = 0;i < eo->BoneData.Count(); i++)
			{
			
			if (eo->BoneData[i].Node) 
				{
				ObjectState bos = eo->BoneData[i].Node->EvalWorldState(0);

				if ( (node == eo->BoneData[i].Node) &&
					 (os.obj->ClassID() != bid)  )
					return FALSE;

				}



			}


		if (os.obj->ClassID() == bid)  
				{

				int found = SendMessage(GetDlgItem(eo->hParam,IDC_LIST1),
							LB_FINDSTRING,(WPARAM) 0,(LPARAM)(TCHAR*)name1);
				if (found != LB_ERR ) return FALSE;

				}

			//}

//check for end nodes

		if (os.obj->ClassID() == bid)  
			{
//get parent if
			INode* parent = node->GetParentNode();
			if (parent->IsRootNode()) return FALSE;
			if (parent == NULL) return FALSE;
			
			ObjectState pos = parent->EvalWorldState(0);
			if (pos.obj->ClassID() != bid)  return FALSE;
		
//			int subcount = node->NumberOfChildren();
//			if (subcount == 0) return FALSE;
			}
		if (os.obj->SuperClassID()==SHAPE_CLASS_ID)
			{
			if ( (os.obj->ClassID()==EDITABLE_SURF_CLASS_ID)
				)
				return FALSE;
			}	



		}

	return TRUE;

}




