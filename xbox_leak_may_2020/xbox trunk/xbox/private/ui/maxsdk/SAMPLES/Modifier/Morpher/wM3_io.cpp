/*===========================================================================*\
 | 
 |  FILE:	wM3_io.cpp
 |			Weighted Morpher for MAX R3
 |			Handles loading / saving
 | 
 |  AUTH:   Harry Denholm
 |			Copyright(c) Kinetix 1999
 |			All Rights Reserved.
 |
 |  HIST:	Started 21-9-98	
 |			BitArray native save, 3-3-99 HD
 | 
\*===========================================================================*/

#include "wM3.h"


IOResult morphChannel::Save(ISave* isave)
{
	ULONG nb;

	// Size of morph data arrays
	isave->BeginChunk(MR3_POINTCOUNT);
	isave->Write(&nPts,sizeof(int),&nb);
	isave->Write(&nmPts,sizeof(int),&nb);
	isave->EndChunk();


	// Save out the morph point data
	isave->BeginChunk(MR3_POINTDATA_MP);
	isave->Write(mPoints,sizeof(Point3)*nPts,&nb);
	isave->EndChunk();

	isave->BeginChunk(MR3_POINTDATA_MW);
	isave->Write(mWeights,sizeof(double)*nPts,&nb);
	isave->EndChunk();

	isave->BeginChunk(MR3_POINTDATA_MD);
	isave->Write(mDeltas,sizeof(Point3)*nPts,&nb);
	isave->EndChunk();

	isave->BeginChunk(MR3_POINTDATA_MO);
	isave->Write(mOptdata,sizeof(int)*nmPts,&nb);
	isave->EndChunk();


	// Misc stuff saving
	isave->BeginChunk(MR3_SELARRAY);
	mSel.Save(isave);
	//isave->Write(&mSel,sizeof(BitArray),&nb);
	isave->EndChunk();

	isave->BeginChunk(MR3_NAME);
	isave->WriteWString(mName.data());
	isave->EndChunk();

	isave->BeginChunk(MR3_PARAMS);
	isave->Write(&mActive,sizeof(BOOL),&nb);
	isave->Write(&mModded,sizeof(BOOL),&nb);
	isave->Write(&mUseLimit,sizeof(BOOL),&nb);
	isave->Write(&mUseSel,sizeof(BOOL),&nb);
	isave->Write(&mSpinmin,sizeof(float),&nb);
	isave->Write(&mSpinmax,sizeof(float),&nb);
	isave->Write(&mActiveOverride,sizeof(BOOL),&nb);
	isave->EndChunk();

	return IO_OK;
}

IOResult morphChannel::Load(ILoad* iload)
{
	ULONG nb;
	IOResult res = IO_OK;
	int tnPts, tnmPts;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case MR3_POINTCOUNT:
				res = iload->Read(&tnPts,sizeof(int),&nb);
				res = iload->Read(&tnmPts,sizeof(int),&nb);
				nPts = tnPts;
				nmPts = tnmPts;
			break;

			case MR3_POINTDATA_MP:
			if(mPoints) delete [] mPoints;
			mPoints = new Point3[nPts];
			res = iload->Read(mPoints,sizeof(Point3)*nPts,&nb);
			break;

			case MR3_POINTDATA_MW:
			if(mWeights) delete [] mWeights;
			mWeights = new double[nPts];
			res = iload->Read(mWeights,sizeof(double)*nPts,&nb);
			break;
			
			case MR3_POINTDATA_MD:
			if(mDeltas) delete [] mDeltas;
			mDeltas = new Point3[nPts];
			res = iload->Read(mDeltas,sizeof(Point3)*nPts,&nb);
			break;

			case MR3_POINTDATA_MO:
			if(mOptdata) delete [] mOptdata;
//			mOptdata = new int[nmPts];
			mOptdata = new int[nPts]; // mjm - 11.8.99
			res = iload->Read(mOptdata,sizeof(int)*nmPts,&nb);
			break;

			case MR3_SELARRAY:
				mSel.Load(iload);
				//res = iload->Read(&mSel,sizeof(BitArray),&nb);
			break;

			case MR3_NAME:{
				TCHAR *buf;
				res = iload->ReadWStringChunk(&buf);
				mName = TSTR(buf);
			break;}
		
			case MR3_PARAMS:
				res = iload->Read(&mActive,sizeof(BOOL),&nb);
				res = iload->Read(&mModded,sizeof(BOOL),&nb);
				res = iload->Read(&mUseLimit,sizeof(BOOL),&nb);
				res = iload->Read(&mUseSel,sizeof(BOOL),&nb);
				res = iload->Read(&mSpinmin,sizeof(float),&nb);
				res = iload->Read(&mSpinmax,sizeof(float),&nb);
				res = iload->Read(&mActiveOverride,sizeof(BOOL),&nb);
			break;
		}

		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}

	return IO_OK;
}

IOResult MorphR3::Load(ILoad *iload)
{
	Modifier::Load(iload);

	ULONG nb;

	IOResult res = IO_OK;

	while (IO_OK==(res=iload->OpenChunk())) {

		if((iload->CurChunkID()>=MR3_MC_SUBCHUNK) && (iload->CurChunkID()<MR3_MC_SUBCHUNK+100 ))
		{
			int mID = iload->CurChunkID()-MR3_MC_SUBCHUNK;
			res = chanBank[mID].Load(iload);
		}

		switch (iload->CurChunkID()) {
			case MR3_MARKERNAME:
				markerName.Load(iload);
				break;

			case MR3_MARKERINDEX:
				int n;
				res = iload->Read(&n,sizeof(int), &nb);
				markerIndex.SetCount(n);

				for(int x=0;x<n;x++)
				{
					res = iload->Read(&markerIndex[x],sizeof(int), &nb);
				}
				break;

		}

		iload->CloseChunk();
		if (res!=IO_OK)  return res;
		}	

	return IO_OK;
}

IOResult MorphR3::Save(ISave *isave)
{
	Modifier::Save(isave);

	ULONG nb;
	int n;

	for(int i=0;i<100;i++)
	{
		isave->BeginChunk(MR3_MC_SUBCHUNK+i);
		chanBank[i].Save(isave);
		isave->EndChunk();	
	}

	isave->BeginChunk(MR3_MARKERNAME);
	markerName.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(MR3_MARKERINDEX);
	n = markerIndex.Count();
	isave->Write(&n,sizeof(int), &nb);

	for(int x=0;x<n;x++)
	{
		isave->Write(&markerIndex[x],sizeof(int), &nb);
	}
	isave->EndChunk();

	return IO_OK;
}