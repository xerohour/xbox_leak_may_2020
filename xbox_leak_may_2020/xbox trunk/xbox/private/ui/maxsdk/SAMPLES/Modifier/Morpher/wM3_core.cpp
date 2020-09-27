/*===========================================================================*\
 | 
 |  FILE:	wM3_core.cpp
 |			Weighted Morpher for MAX R3
 |			ModifyObject
 | 
 |  AUTH:   Harry Denholm
 |			Copyright(c) Kinetix 1999
 |			All Rights Reserved.
 |
 |  HIST:	Started 27-8-98
 | 
\*===========================================================================*/

#include "wM3.h"

void MorphR3::ModifyObject(TimeValue t, ModContext &mc, ObjectState * os, INode *node) 
{
	// This will see if the local cached object is valid and update it if not
	// It will now also call a full channel rebuild to make sure their deltas are
	// accurate to the new cached object
	if(!MC_Local.AreWeCached())
	{
		UI_MAKEBUSY

		MC_Local.MakeCache(os->obj);

		for(int i=0;i<100;i++)
		{
			if(chanBank[i].mActive)
			{
				chanBank[i].rebuildChannel();
			}
		}

		UI_MAKEFREE
	}

	Interval valid=FOREVER;


	// AUTOLOAD
	int itmp; 
	pblock->GetValue(PB_CL_AUTOLOAD, 0, itmp, valid);

	if(itmp==1)
	{
			for(int k=0;k<100;k++)
			{
				if(chanBank[k].mConnection)
					chanBank[k].buildFromNode(chanBank[k].mConnection,FALSE,t);
			}
	}



	// Get count from host
	int hmCount = os->obj->NumPoints();

	int i,x,k,mIndex;
	// to hold percentage
	float mPct;

	// some worker variables
	float deltX,deltY,deltZ;
	float tmX,tmY,tmZ;
	Point3 vert,mVert,fVert;
	double weight,deltW,decay;

	// These are our morph deltas / point
	// They get built by cycling through the points and generating
	// the difference data, summing it into these tables and then
	// appling the changes at the end.
	// This will leave us with the total differences per point on the 
	// local mesh. We can then rip through and apply them quickly
	float *difX = new float[hmCount];
	float *difY = new float[hmCount];
	float *difZ = new float[hmCount];
	double *wgts = new double[hmCount];

	// this is the indicator of what points on the host
	// to update after all deltas have been summed
	BitArray PointsMOD(hmCount);
	PointsMOD.ClearAll();

	int glUsesel;
	pblock->GetValue( PB_OV_USESEL, t, glUsesel, valid);

	BOOL glUseLimit; float glMAX,glMIN;
	pblock->GetValue( PB_OV_USELIMITS, t, glUseLimit, valid);
	pblock->GetValue( PB_OV_SPINMAX, t, glMAX, valid);
	pblock->GetValue( PB_OV_SPINMIN, t, glMIN, valid);

	// --------------------------------------------------- MORPHY BITS
	// cycle through channels, searching for ones to use
	for(i=0;i<100;i++){

		if( chanBank[i].mActive )
		{
			// temp fix for diff. pt counts
			if(chanBank[i].nPts!=hmCount) 
			{
				chanBank[i].mInvalid = TRUE;
				continue;
			};

			
			// This channel is considered okay to use
			chanBank[i].mInvalid = FALSE;

			
			// Is this channel flagged as inactive?
			if(chanBank[i].mActiveOverride==FALSE) continue;


			// get morph percentage for this channel
			chanBank[i].cblock->GetValue(0,t,mPct,valid);

			// Clamp the channel values to the limits
			if(chanBank[i].mUseLimit||glUseLimit)
			{
				int Pmax;
				int Pmin;
				if(glUseLimit)
				{
					Pmax = glMAX; Pmin = glMIN;
				}
				else
				{
					Pmax = chanBank[i].mSpinmax;
					Pmin = chanBank[i].mSpinmin;
				}
				

				if(mPct>Pmax) mPct = Pmax;
				if(mPct<Pmin) mPct = Pmin;
			}

			// cycle through all morphable points, build delta arrays
			for(x=0;x<chanBank[i].nmPts;x++)
			{
				// this is the point to morph on the object
				mIndex = chanBank[i].mOptdata[x];
				
				if(chanBank[i].mSel[mIndex]||(!chanBank[i].mUseSel&&glUsesel==0))
				{

					// get the points to morph between
					//vert = os->obj->GetPoint(mIndex);
					//weight = os->obj->GetWeight(mIndex);
					vert = MC_Local.oPoints[mIndex];
					weight = MC_Local.oWeights[mIndex];

					// Get softselection, if applicable
					decay = 1.0f;
					if(os->obj->GetSubselState()!=0) decay = os->obj->PointSelection(mIndex);

					tmX = vert.x;
					tmY = vert.y;
					tmZ = vert.z;

					mVert = chanBank[i].mPoints[mIndex];

					// Add the previous point data into the delta table
					// if its not already been done
					if(!PointsMOD[mIndex])
					{
						difX[mIndex]=vert.x;
						difY[mIndex]=vert.y;
						difZ[mIndex]=vert.z;
						wgts[mIndex]=weight;
					}
		
					// calculate the differences
					// decay by the weighted vertex amount, to support soft selection
					deltX=((chanBank[i].mDeltas[mIndex].x)*mPct)*decay;
					deltY=((chanBank[i].mDeltas[mIndex].y)*mPct)*decay;
					deltZ=((chanBank[i].mDeltas[mIndex].z)*mPct)*decay;
					deltW=(chanBank[i].mWeights[mIndex]-weight)/100.0f*(double)mPct;

					difX[mIndex]+=deltX;
					difY[mIndex]+=deltY;
					difZ[mIndex]+=deltZ;
					wgts[mIndex]+=deltW;

					// We've modded this point
					PointsMOD.Set(mIndex);

				} // msel check

			} // nmPts cycle

		}

	}


	// Cycle through all modified points and apply delta arrays
	for(k=0;k<hmCount;k++)
	{
		if(PointsMOD[k]&&(MC_Local.sel[k]||os->obj->GetSubselState()==0))
		{
			fVert.x = difX[k];
			fVert.y = difY[k];
			fVert.z = difZ[k];
			os->obj->SetPoint(k,fVert);
			os->obj->SetWeight(k,wgts[k]);
		}


		// Captain Hack Returns...
		// Support for saving of modifications to a channel
		// Most of this is just duped from buildFromNode (delta/point/calc)
		if (recordModifications)
		{
			int tChan = recordTarget;

			int tPc = hmCount;

			// Prepare the channel
			chanBank[tChan].AllocBuffers(tPc, tPc);
			chanBank[tChan].nPts = 0;
			chanBank[tChan].nmPts = 0;

			int id = 0;
			Point3 DeltP;
			double wtmp;
			Point3 tVert;

			for(int x=0;x<tPc;x++)
			{

				if(PointsMOD[x])
				{
					tVert.x = difX[x];
					tVert.y = difY[x];
					tVert.z = difZ[x];

					wtmp = wgts[x];
				}
				else
				{
					tVert = os->obj->GetPoint(x);
					wtmp = os->obj->GetWeight(x);
				}


				// Is this vertex different from the original mesh?
				if(
					(tVert.x!=MC_Local.oPoints[x].x)||
					(tVert.y!=MC_Local.oPoints[x].y)||
					(tVert.z!=MC_Local.oPoints[x].z)||
					(wtmp!=MC_Local.oWeights[x])
					)
				{
					chanBank[tChan].mOptdata[id] = x;
					chanBank[tChan].nmPts++;
					id++;
				}

					// calculate the delta cache
					DeltP.x=(tVert.x-MC_Local.oPoints[x].x)/100.0f;
					DeltP.y=(tVert.y-MC_Local.oPoints[x].y)/100.0f;
					DeltP.z=(tVert.z-MC_Local.oPoints[x].z)/100.0f;
					chanBank[tChan].mDeltas[x] = DeltP;

					chanBank[tChan].mWeights[x] = wtmp;

					chanBank[tChan].mPoints[x] = tVert;
					chanBank[tChan].nPts++;
			}

			recordModifications = FALSE;
			recordTarget = 0;
			chanBank[tChan].mInvalid = FALSE;

		}
		// End of record

	}

	// clean up
	if(difX) delete [] difX;
	if(difY) delete [] difY;
	if(difZ) delete [] difZ;
	if(wgts) delete [] wgts;

	if(itmp==1) valid = Interval(t,t);

	// Update all the caches etc
	os->obj->UpdateValidity(GEOM_CHAN_NUM,valid);
	os->obj->PointsWereChanged();
}
