/*===========================================================================*\
 | 
 |  FILE:	wM3_channel.cpp
 |			Weighted Morpher for MAX R3
 |			Stuff for channel management
 | 
 |  AUTH:   Harry Denholm
 |			Copyright(c) Kinetix 1999
 |			All Rights Reserved.
 |
 |  HIST:	Started 15-10-98
 | 
\*===========================================================================*/

#include "wM3.h"

// Construct the class with defaults
morphChannel::morphChannel()
{
	nPts = 0;
	nmPts = 0;
	mPoints = NULL;
	mWeights = NULL;
	mDeltas = NULL;
	mOptdata = NULL;
	mSel.ClearAll();
	//mSel.SetSize(0,0);
	mSel = NULL;
	mConnection = NULL;
	// default name '-empty-'
	mName = _T(GetString(IDS_EMPTY_CHANNEL));
	// channel starts 'inactive'
	mActive = FALSE;
	// channel value limits defaults
	mSpinmin = 0.0f;
	mSpinmax = 100.0f;
	mUseLimit = FALSE;
	mUseSel = FALSE;
	cblock = NULL;
	mModded = FALSE;
	mInvalid = FALSE;
	mActiveOverride = TRUE;
}

morphChannel::~morphChannel()
{
	if(mPoints) delete [] mPoints;
	if(mWeights) delete [] mWeights;
	if(mOptdata) delete [] mOptdata;
	if(mDeltas) delete [] mDeltas;
	mConnection = NULL;
	cblock = NULL;

	mSel.ClearAll();
//	mSel.SetSize(0,0);
}


void morphChannel::ResetMe()
{
	if(mPoints) delete [] mPoints;
	if(mWeights) delete [] mWeights;
	if(mOptdata) delete [] mOptdata;
	if(mDeltas) delete [] mDeltas;
	nPts = 0;
	nmPts = 0;
	mPoints = NULL;
	mWeights = NULL;
	mDeltas = NULL;
	mOptdata = NULL;
	mSel.ClearAll();
//	mSel.SetSize(0,0);
	mConnection = NULL;
	mName = _T(GetString(IDS_EMPTY_CHANNEL));
	mActive = FALSE;
	mSpinmin = 0.0f;
	mSpinmax = 100.0f;
	mUseLimit = FALSE;
	mUseSel = FALSE;
	cblock = NULL;
	mModded = FALSE;
	mInvalid = FALSE;
	mActiveOverride = TRUE;
}


void morphChannel::AllocBuffers( int sizeA, int sizeB )
{
	if(mPoints) delete [] mPoints;
	if(mWeights) delete [] mWeights;
	if(mDeltas) delete [] mDeltas;

	if(mOptdata) delete [] mOptdata;

	mPoints = new Point3[sizeA];
	mWeights = new double[sizeA];
	mDeltas = new Point3[sizeA];

	mOptdata = new int[sizeB];
}


// Do some rough calculations about how much space this channel
// takes up
// This isn't meant to be fast or terribly accurate!
float morphChannel::getMemSize()
{
	float msize = 0.0f;
	msize += (sizeof(Point3)*nPts);	// morphable points
	msize += (sizeof(Point3)*nPts);	// delta points
	msize += (sizeof(double)*nPts);	// Weighting points
	msize += (sizeof(int)*nmPts);	// Optimization data
	return msize;
}


// This = operator does everythinig BUT transfer paramblock references
morphChannel& morphChannel::operator=(morphChannel& from)
{
	// Don't allow self->self assignment
	if(&from == this)
		return (*this);

	int i;
	int s_nPts,s_nmPts;

	s_nPts = from.nPts;
	s_nmPts = from.nmPts;

	nPts = s_nPts;
	nmPts = s_nmPts;

	// Delete and reassign arrays
	if(mPoints) delete [] mPoints;
	if(mDeltas) delete [] mDeltas;
	if(mWeights) delete [] mWeights;
	if(mOptdata) delete [] mOptdata;

	mPoints = new Point3[s_nPts];
	mDeltas = new Point3[s_nPts];
	mWeights = new double[s_nPts];
	mOptdata = new int[s_nmPts];

	for(i = 0;i<s_nPts;i++)
	{
		mPoints[i] = from.mPoints[i];
		mDeltas[i] = from.mDeltas[i];
		mWeights[i] = from.mWeights[i];
	}
	for(i = 0;i<s_nmPts;i++)
	{
		mOptdata[i] = from.mOptdata[i];
	}

	mSel = from.mSel;
	mConnection = from.mConnection;

	mActive = from.mActive;
	mModded = from.mModded;
	mUseLimit = from.mUseLimit;
	mUseSel = from.mUseSel;
	mSpinmin = from.mSpinmin;
	mSpinmax = from.mSpinmax;
	mInvalid = from.mInvalid;
	mActiveOverride = from.mActiveOverride;

	TSTR trans3Name (from.mName);
	mName = trans3Name;

	mp = mp;

	return (*this);
}



// Reconstruct the optimization malarky using the current channel's point info
void morphChannel::rebuildChannel()
{
	int x,id = 0;
	Point3 DeltP;
	double wtmp;
	Point3 tVert;

	int tPc = nPts;
	if(tPc!=mp->MC_Local.Count) goto CantLoadThis;
	if(!mp->MC_Local.CacheValid) goto CantLoadThis;

	mInvalid = FALSE;

	nmPts = 0;

	for(x=0;x<tPc;x++)
	{
		tVert = mPoints[x];
		wtmp = mWeights[x];

		// Is this vertex different from the original mesh?
		if(
			(tVert.x!=mp->MC_Local.oPoints[x].x)||
			(tVert.y!=mp->MC_Local.oPoints[x].y)||
			(tVert.z!=mp->MC_Local.oPoints[x].z)||
			(wtmp!=mp->MC_Local.oWeights[x])
			)
		{
			mOptdata[id] = x;
			nmPts++;
			id++;
		}

			// calculate the delta cache
			DeltP.x=(tVert.x-mp->MC_Local.oPoints[x].x)/100.0f;
			DeltP.y=(tVert.y-mp->MC_Local.oPoints[x].y)/100.0f;
			DeltP.z=(tVert.z-mp->MC_Local.oPoints[x].z)/100.0f;
			mDeltas[x] = DeltP;

	}

	CantLoadThis:
	tPc=0;
}

// Generate all the optimzation and geometry data
void morphChannel::buildFromNode( INode *node , BOOL resetTime, TimeValue t )
{
	if(resetTime) t = GetCOREInterface()->GetTime();

	ObjectState os = node->EvalWorldState(t);

	int tPc = os.obj->NumPoints();
	int x,id = 0;
	Point3 DeltP;
	double wtmp;
	Point3 tVert;

	if(tPc!=mp->MC_Local.Count) goto CantLoadThis;
	if(!mp->MC_Local.CacheValid) goto CantLoadThis;


	mInvalid = FALSE;

	// if the channel hasn't been edited yet, change the 'empty'
	// name to that of the chosen object.
	if( !mModded ) mName = node->GetName();

	// Set the data into the morphChannel
	mActive = TRUE;
	mModded = TRUE;

	
	// Prepare the channel
	AllocBuffers(tPc, tPc);
	mSel.SetSize(tPc);
	mSel.ClearAll();

	nPts = 0;
	nmPts = 0;


	for(x=0;x<tPc;x++)
	{
		tVert = os.obj->GetPoint(x);
		wtmp = os.obj->GetWeight(x);

		// Is this vertex different from the original mesh?
		if(
			(tVert.x!=mp->MC_Local.oPoints[x].x)||
			(tVert.y!=mp->MC_Local.oPoints[x].y)||
			(tVert.z!=mp->MC_Local.oPoints[x].z)||
			(wtmp!=mp->MC_Local.oWeights[x])
			)
		{
			mOptdata[id] = x;
			nmPts++;
			id++;
		}

			// calculate the delta cache
			DeltP.x=(tVert.x-mp->MC_Local.oPoints[x].x)/100.0f;
			DeltP.y=(tVert.y-mp->MC_Local.oPoints[x].y)/100.0f;
			DeltP.z=(tVert.z-mp->MC_Local.oPoints[x].z)/100.0f;
			mDeltas[x] = DeltP;

			mWeights[x] = os.obj->GetWeight(x);
			mSel.Set( x, os.obj->IsPointSelected(x)?1:0);

			mPoints[x] = tVert;
			nPts++;
	}


	// Update *everything*
	mp->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	mp->NotifyDependents(FOREVER,PART_ALL,REFMSG_SUBANIM_STRUCTURE_CHANGED);
	mp->Update_channelFULL();
	mp->Update_channelParams();

	CantLoadThis:
	tPc=0;
}
