/*===========================================================================*\
 | 
 |  FILE:	wM3_main.cpp
 |			Weighted Morpher for MAX R3
 |			Main class and plugin code
 | 
 |  AUTH:   Harry Denholm
 |			Copyright(c) Kinetix 1999
 |			All Rights Reserved.
 |
 |  HIST:	Started 22-5-98
 | 
\*===========================================================================*/


/*===========================================================================*\
 | Includes and global/macro setup
\*===========================================================================*/
#include "wM3.h"

ClassDesc* GetMorphR3Desc();

IObjParam *MorphR3::ip			= NULL;


/*===========================================================================*\
 | Parameter Blocks
\*===========================================================================*/


// Global parameter description
static ParamBlockDescID GlobalParams[] = {
	{ TYPE_INT, NULL, FALSE, 0 },	// overrides: Use Limits
	{ TYPE_FLOAT, NULL, FALSE, 1 },	// overrides: Spinner Min
	{ TYPE_FLOAT, NULL, FALSE, 2 },	// overrides: Spinner Max
	{ TYPE_INT, NULL, FALSE, 3 },	// overrides: Use Selection
	{ TYPE_INT, NULL, FALSE, 4 },	// advanced:  Value increments
	{ TYPE_INT, NULL, FALSE, 5 },	// clist:	  Auto load
};
#define MR3_SIZE_GLOBALS	6


MorphR3::MorphR3()
{
	// Load the channels
	chanBank = new morphChannel[MR3_NUM_CHANNELS];
	chanNum = 0;
	chanSel = 0;

	for(int q=0;q<100;q++) chanBank[q].mp = this;

	newname = NULL;
	tccI = FALSE;

	// Zero and init the marker system
	markerName.ZeroCount();
	markerIndex.ZeroCount();
	markerSel		= -1;

	hwLegend = hwGlobalParams = hwChannelList = hwChannelParams = hwAdvanced = NULL;

	// Don't record anything yet!
	recordModifications = FALSE;
	recordTarget = 0;


	// Create the parameter block
	MakeRefByID(FOREVER, 0,CreateParameterBlock(GlobalParams,MR3_SIZE_GLOBALS,1));	
	assert(pblock);	


	// Assign some global defaults
	pblock->SetValue(PB_OV_USELIMITS,	0,	1);
	pblock->SetValue(PB_OV_SPINMIN,		0,	0.0f);
	pblock->SetValue(PB_OV_SPINMAX,		0,	100.0f);
	pblock->SetValue(PB_OV_USESEL,		0,	0);
	pblock->SetValue(PB_AD_VALUEINC,	0,	1);
	pblock->SetValue(PB_CL_AUTOLOAD,	0,	0);


	// Build the multiple set of pblocks
	for( int a=0;a<MR3_NUM_CHANNELS;a++){
	
		// Paramblock / morph channel so we have more organised TV support
		ParamBlockDescID *channelParams = new ParamBlockDescID[1];

		ParamBlockDescID add;
		add.type=TYPE_FLOAT;
		add.user=NULL;
		add.animatable=TRUE;
		add.id=1;
		channelParams[0] = add;

		MakeRefByID(FOREVER, 1+a, CreateParameterBlock(channelParams,1,1));	
		assert(chanBank[a].cblock);

		// TCB controller as default
		// This helps with the very erratic curve results you get when
		// animating with normal bezier float controllers
		Control *c = (Control*)CreateInstance(CTRL_FLOAT_CLASS_ID,GetDefaultController(CTRL_FLOAT_CLASS_ID)->ClassID());

		chanBank[a].cblock->SetValue(0,0,0.0f);
		chanBank[a].cblock->SetController(0,c);

		delete channelParams;
	}
}

MorphR3::~MorphR3()
{
	markerName.ZeroCount();
	markerIndex.ZeroCount();
	DeleteAllRefsFromMe();

	if(chanBank) delete [] chanBank;
	chanBank = NULL;

	Interface *Cip = GetCOREInterface();
	if(Cip&&tccI) { Cip->UnRegisterTimeChangeCallback(this); tccI=FALSE; }
}

void MorphR3::NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc)
{
	if( (partID&PART_TOPO) || (partID&PART_GEOM) || (partID&PART_SELECT) )
	{
		if(MC_Local.AreWeCached()) MC_Local.NukeCache();
		NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);
	}
}

Interval MorphR3::LocalValidity(TimeValue t)
{
	Interval iv = FOREVER;
	float ftmp=0;
	int itmp=0;

	for(int i=0;i<100;i++)
	{
		chanBank[i].cblock->GetValue(0,t,ftmp,iv);	

		if(chanBank[i].mConnection)
		{
			chanBank[i].mConnection->GetNodeTM(t,&iv);
		}
	}

	int ALoad; 
	pblock->GetValue(PB_CL_AUTOLOAD, 0, ALoad, iv);
	if(ALoad==1) iv = Interval(t,t);

	return iv; 
}

RefTargetHandle MorphR3::Clone(RemapDir& remap)
{
	int x;

	MorphR3* newmod = new MorphR3();	

	if (pblock) newmod->ReplaceReference (0, pblock->Clone());

	for(x=1;x<=100;x++)		
	{
		newmod->ReplaceReference (x, chanBank[x-1].cblock->Clone());
		newmod->chanBank[x-1] = chanBank[x-1];
	}
	for(x=101;x<=200;x++)	newmod->ReplaceReference (x, chanBank[x-101].mConnection);

	newmod->markerName = markerName;
	newmod->markerIndex = markerIndex;

	return(newmod);
}



//From ReferenceMaker 
RefResult MorphR3::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
{
	TSTR outputStr;

	switch (message) {


		/* AUTOLOAD
		case REFMSG_CHANGE:
		{
			if(pblock)
			{
			int itmp; Interval valid = FOREVER;
			pblock->GetValue(PB_CL_AUTOLOAD, 0, itmp, valid);

			if(itmp==1)
			{
					for(int k=0;k<100;k++)
					{
						if((INode*)hTarget==(INode*)chanBank[k].mConnection) 
						{
							chanBank[k].buildFromNode((INode*)hTarget);
						}
					}
			}
			}
			break;
		}*/


		case REFMSG_GET_PARAM_DIM: {
			GetParamDim *gpd = (GetParamDim*)partID;
			gpd->dim = defaultDim; 
			return REF_STOP; 
			}

		case REFMSG_GET_PARAM_NAME: {
			GetParamName *gpn = (GetParamName*)partID;

			switch (gpn->index) {

			case 0:	{		
					if(hTarget==pblock) gpn->name = TSTR(GetString(IDS_PBN_USELIMITS+gpn->index));

					for(int k=0;k<100;k++)
					{
						if(hTarget==chanBank[k].cblock&&chanBank[k].mActive!=FALSE) {	
							outputStr.printf(_T("[%d] %s  (%s)"), 
								k+1,
								chanBank[k].mName,
								chanBank[k].mConnection?GetString(IDS_ONCON):GetString(IDS_NOCON)
								);
							gpn->name = outputStr;
						}
					}
					break;}

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				{
				gpn->name = TSTR(GetString(IDS_PBN_USELIMITS+gpn->index));
				break;}


				default:
					{
						// 'should' never show up. catch what index it is asking
						// for if it ever does
						char s[25];
						sprintf(s,"debug_bad_index [%i]",gpn->index);
						gpn->name = TSTR(_T(s)); 
						break;
					}
				}
			return REF_STOP; 
			}

		// Handle the deletion of a morph target
		case REFMSG_TARGET_DELETED:{
			for(int u=0;u<100;u++){
			if (hTarget==chanBank[u].mConnection) {
				DeleteReference(101+u);
				chanBank[u].mConnection = NULL;
				}
			}
			Update_channelFULL();
			Update_channelParams();
			NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
			NotifyDependents(FOREVER, PART_ALL, REFMSG_SUBANIM_STRUCTURE_CHANGED);
			break;
		}
	}

	return REF_SUCCEED;
}



// =====================================================================================
// SCARY REFERENCE AND SUBANIM STUFF




int MorphR3::NumRefs() 
{
	return 1+(MR3_NUM_CHANNELS*2);
}


// TODO: link this to the MR3_ defines

RefTargetHandle MorphR3::GetReference(int i) 
{
	if(i==0) return pblock;
	else if(i>0&&i<=100) return chanBank[i-1].cblock;
	else if(i>100&&i<=200) return chanBank[i-101].mConnection;
	else return NULL;
}

void MorphR3::SetReference(int i, RefTargetHandle rtarg)
{
	if(i==0) pblock = (IParamBlock*)rtarg;
	if(i>0&&i<=100) chanBank[i-1].cblock = (IParamBlock*)rtarg;
	if(i>100&&i<=200) chanBank[i-101].mConnection = (INode*)rtarg;
}



int MorphR3::NumSubs() 
{ 
	return 1+(MR3_NUM_CHANNELS);
}  
Animatable* MorphR3::SubAnim(int i) 
{ 
	if(i==0) return pblock;
	else if( chanBank[i-1].mActive == TRUE ) return chanBank[i-1].cblock;
	else return NULL; 
}

TSTR MorphR3::SubAnimName(int i) 
{ 
	if(i==0) return GetString(IDS_SUBANIMPARAM);
	else return _T(chanBank[i-1].mName);
}


class MorphR3PostLoadCallback : public PostLoadCallback {
	public:
		MorphR3PostLoadCallback(ParamBlockPLCB *c) {};
		void proc(ILoad *iload) {};
};


