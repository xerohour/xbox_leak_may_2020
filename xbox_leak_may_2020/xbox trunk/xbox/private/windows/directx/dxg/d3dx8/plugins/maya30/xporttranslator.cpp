// TBD	
//		- magic number "xof";
//		- skinning for bezier patches????
//		- adjust normals according to tweaks
//		- test animation with different models (hierarchies, etc.)
//		- support for single-sided vs. double sided
//		- check whether there really is vertex coloring info
//		- make sure that shape names are unique and valid
//		- option to export only selected items
//		- uncomment vertex coloring info


// DONE
//		- multiply envelope by tweaks
//		- export bicubic bezier patches
//		- relative pathnames for textures
//		- make animation optional
//		- speed up animation by rearranging loops
//		- error handling
//		- free Mesh::rgBones[iBone].m_szName
//		- option to flip UV tex coords
//		- option for animation transforms a) ONLY at keyframes or b) at EVERY frame
//		- dialog box interface
//		- option for Text/Binary/Compressed mode

// BUGS
//		- wierd errors loading cowboy.		- Dt library crashes during it's initialization





#include "MyDt.h"
#include "MyAssert.h"

#include "xportTranslator.h"

#include <iostream.h>



// DirectX File Format
#include <dxfile.h>

#include <initguid.h>
#include <rmxfguid.h>
#include <rmxftmpl.h>

// Additional X-file Templates
#include "xskinexptemplates.h"

const GUID* aIds[] = {&DXFILEOBJ_XSkinMeshHeader,
					  &DXFILEOBJ_VertexDuplicationIndices,
					  &DXFILEOBJ_SkinWeights};


// Maya API
#include <maya/MFnPlugin.h>
#include <maya/MSimple.h>
#include <maya/MObject.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include <maya/MDagPath.h>
#include <maya/MTime.h>
#include <maya/MAnimControl.h>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>

#include <maya/MIntArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MPointArray.h>
#include <maya/MObjectArray.h>
#include <maya/MDagPathArray.h>
#include <maya/MPlugArray.h>
#include <maya/MSelectionList.h>

#include <maya/MFnMatrixData.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>

#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MFnDoubleIndexedComponent.h>
#include <maya/MFnSet.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MFnWeightGeometryFilter.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnIKJoint.h>
#include <maya/MFnMesh.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnLambertShader.h>

#include <maya/MItDependencyNodes.h>
#include <maya/MItGeometry.h>



extern	StringTable g_Strings;


// Dt API
#include <MDt.h>
#include <MDtExt.h>


// Options

DXFILEFORMAT			g_FileFormat;
extern	bool			g_bExportAnimation;
extern	bool			g_bKeyframeAnimation;
extern	bool			g_bAnimateEverything;
extern	int				g_iFrameStep;
extern	int				g_iFlipU;
extern	int				g_iFlipV;
extern	bool			g_bRelativeTexFile;
extern	bool			g_bExportPatches;












HRESULT	AddAnim
		(
			Anim*					pAnim, 
			LPDIRECTXFILEDATA		pAnimSetObject, 
			LPDIRECTXFILESAVEOBJECT	pxofSave
		) 
{
	HRESULT				hr							= S_OK;
	
	LPDIRECTXFILEDATA	pAnimDataObject				= NULL;

	LPDIRECTXFILEDATA	pQuaternionKeyDataObject	= NULL;
	LPDIRECTXFILEDATA	pScaleKeyDataObject			= NULL;
	LPDIRECTXFILEDATA	pPositionKeyDataObject		= NULL;

	PBYTE				pbQuaternionKeyData			= NULL;
	PBYTE				pbScaleKeyData				= NULL;
	PBYTE				pbPositionKeyData			= NULL;



	INIT;



	HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMAnimation, NULL, NULL, 0, NULL, &pAnimDataObject),
				"Could not create pAnimDataObject.");


	int		cbQuaternionKeySize	= sizeof(DWORD)												// keyType
								+ sizeof(DWORD)												// nKeys
								+ pAnim->m_cKeys 
									* (sizeof(DWORD) + sizeof(DWORD) + 4 * sizeof(float));	// keys[nKeys]


	PBYTE	pbQuaternionKeyCurr	= pbQuaternionKeyData	= new BYTE[cbQuaternionKeySize];

	ASSERT(pbQuaternionKeyData,
				"Could not allocate memory for pbQuaternionKeyData.");


	int		cbScaleKeySize	= sizeof(DWORD)													// keyType
							+ sizeof(DWORD)													// nKeys
							+ pAnim->m_cKeys 
								* (sizeof(DWORD) + sizeof(DWORD) + 3 * sizeof(float));		// keys[nKeys]

	PBYTE	pbScaleKeyCurr	= pbScaleKeyData	= new BYTE[cbScaleKeySize];

	ASSERT(pbScaleKeyData,
				"Could not allocate memory for pbScaleKeyData.");


	int		cbPositionKeySize	= sizeof(DWORD)												// keyType
								+ sizeof(DWORD)												// nKeys
								+ pAnim->m_cKeys 
									* (sizeof(DWORD) + sizeof(DWORD) + 3 * sizeof(float));	// keys[nKeys]

	PBYTE	pbPositionKeyCurr	= pbPositionKeyData	= new BYTE[cbPositionKeySize];

	ASSERT(pbPositionKeyData,
				"Could not allocate memory for pbPositionKeyData.");


	// keyType
	WRITE_DWORD(pbQuaternionKeyCurr,	((DWORD)0));
	WRITE_DWORD(pbScaleKeyCurr,			((DWORD)1));
	WRITE_DWORD(pbPositionKeyCurr,		((DWORD)2));

	// nKeys
	WRITE_DWORD(pbQuaternionKeyCurr,	((DWORD)pAnim->m_cKeys));
	WRITE_DWORD(pbScaleKeyCurr,			((DWORD)pAnim->m_cKeys));
	WRITE_DWORD(pbPositionKeyCurr,		((DWORD)pAnim->m_cKeys));

	// keys[nKeys]
	for (int iKey = 0; iKey < pAnim->m_cKeys; iKey++) 
	{
		// time
		WRITE_DWORD(pbQuaternionKeyCurr,	((DWORD)pAnim->m_rgKeys[iKey].m_iFrame));
		WRITE_DWORD(pbScaleKeyCurr,			((DWORD)pAnim->m_rgKeys[iKey].m_iFrame));
		WRITE_DWORD(pbPositionKeyCurr,		((DWORD)pAnim->m_rgKeys[iKey].m_iFrame));


		// nValues
		WRITE_DWORD(pbQuaternionKeyCurr,	((DWORD)4));
		WRITE_DWORD(pbScaleKeyCurr,			((DWORD)3));
		WRITE_DWORD(pbPositionKeyCurr,		((DWORD)3));

		// values
		WRITE_FLOAT(pbQuaternionKeyCurr,   -pAnim->m_rgKeys[iKey].m_rgfQuaternion[0]);
		WRITE_FLOAT(pbQuaternionKeyCurr,	pAnim->m_rgKeys[iKey].m_rgfQuaternion[1]);
		WRITE_FLOAT(pbQuaternionKeyCurr,	pAnim->m_rgKeys[iKey].m_rgfQuaternion[2]);
		WRITE_FLOAT(pbQuaternionKeyCurr,	pAnim->m_rgKeys[iKey].m_rgfQuaternion[3]);

		WRITE_FLOAT(pbScaleKeyCurr,	pAnim->m_rgKeys[iKey].m_rgfScale[0]);
		WRITE_FLOAT(pbScaleKeyCurr,	pAnim->m_rgKeys[iKey].m_rgfScale[1]);
		WRITE_FLOAT(pbScaleKeyCurr,	pAnim->m_rgKeys[iKey].m_rgfScale[2]);

		WRITE_FLOAT(pbPositionKeyCurr,	pAnim->m_rgKeys[iKey].m_rgfPosition[0]);
		WRITE_FLOAT(pbPositionKeyCurr,	pAnim->m_rgKeys[iKey].m_rgfPosition[1]);
		WRITE_FLOAT(pbPositionKeyCurr,	pAnim->m_rgKeys[iKey].m_rgfPosition[2]);
	}

	HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMAnimationKey, NULL, NULL, cbQuaternionKeySize, pbQuaternionKeyData, &pQuaternionKeyDataObject),
				"Could not create pQuaternionKeyDataObject.");


	HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMAnimationKey, NULL, NULL, cbScaleKeySize, pbScaleKeyData, &pScaleKeyDataObject),
				"Could not create pScaleKeyDataObject.");


	HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMAnimationKey, NULL, NULL, cbPositionKeySize, pbPositionKeyData, &pPositionKeyDataObject),
				"Could not create pPositionKeyDataObject.");


	HR_ATTEMPT(pAnimDataObject->AddDataReference(pAnim->m_szName, NULL),
				"Could not add data reference to pAnimDataObject.");

    HR_ATTEMPT(pAnimDataObject->AddDataObject(pQuaternionKeyDataObject),
				"Could not add pQuaternionKeyDataObject to pAnimDataObject.");


	HR_ATTEMPT(pAnimDataObject->AddDataObject(pScaleKeyDataObject),
				"Could not add pScaleKeyDataObject to pAnimDataObject.");

	
	HR_ATTEMPT(pAnimDataObject->AddDataObject(pPositionKeyDataObject),
				"Could not add pPositionKeyDataObject to pAnimDataObject.");



	HR_ATTEMPT(pAnimSetObject->AddDataObject(pAnimDataObject),
				"Could not add pAnimDataObject to pAnimSetObject.");



	EXIT;



	delete[] pbQuaternionKeyData;

	delete[] pbScaleKeyData;
	
	delete[] pbPositionKeyData;


	if (pQuaternionKeyDataObject)
		pQuaternionKeyDataObject->Release();

	if (pScaleKeyDataObject)
		pScaleKeyDataObject->Release();

	if (pPositionKeyDataObject)
		pPositionKeyDataObject->Release();

	if (pAnimDataObject)
		pAnimDataObject->Release();


	return hr;
}










HRESULT	loadAllAnims
		(
			Anim*	rgAnims
		) 
{
	HRESULT	hr	= S_OK;

	cout << "\treading at intervals of " << g_iFrameStep << " frame(s)" << endl;

	// calculate the frames per second
	int	iFPS	= 1;

	switch(MTime::uiUnit()) 
	{
		case MTime::kSeconds:		// 1 fps
			iFPS	= 1;
			break;
		case MTime::kMilliseconds:	// 1000 fps
			iFPS	= 1000;
			break;
		case MTime::kGames:			// 15 fps
			iFPS	= 15;
			break;
		case MTime::kFilm:			// 24 fps
			iFPS	= 24;
			break;
		case MTime::kPALFrame:		// 25 fps
			iFPS	= 25;
			break;
		case MTime::kNTSCFrame:		// 30 fps
			iFPS	= 30;
			break;
		case MTime::kShowScan:		// 48 fps
			iFPS	= 48;
			break;
		case MTime::kPALField:		// 50 fps
			iFPS	= 50;
			break;
		case MTime::kNTSCField:		// 60 fps
			iFPS	= 60;
			break;
		default:
			iFPS	= 1;
			break;
	};

	float fTimeFactor	= 3600.0f / (float)iFPS;


	MTime	timeStart(MAnimControl::minTime().value(), MTime::uiUnit());
	MTime	timeEnd(MAnimControl::maxTime().value(), MTime::uiUnit());
	MTime	timeCurrent(MAnimControl::currentTime().value(), MTime::uiUnit());


	DtFrameSetStart((int)timeStart.value());
	DtFrameSetEnd((int)timeEnd.value());


	int cShapes	= DtShapeGetCount();


	MIntArray*	rgrgiKeys	= new MIntArray[cShapes];


	for (int iShape = 0; iShape < cShapes; iShape++) 
	{
		rgAnims[iShape].m_szName	= new char[256];
		rgAnims[iShape].m_cKeys		= 0;
		rgAnims[iShape].m_rgKeys	= new Key[1 + (DtFrameGetEnd() - DtFrameGetStart() + 1) / g_iFrameStep];

		g_Strings.add(rgAnims[iShape].m_szName);

		char* szName;

		DtShapeGetName(iShape, &szName);

		strcpy(rgAnims[iShape].m_szName, szName);

		DtShapeGetTRSAnimKeys(iShape, &rgrgiKeys[iShape]);
	}

	for (int iFrame = DtFrameGetStart(); iFrame <= DtFrameGetEnd(); iFrame += g_iFrameStep) 
	{
		DtFrameSet(iFrame);

		for (int iShape = 0; iShape < cShapes; iShape++) 
		{
			if (rgrgiKeys[iShape].length() > 0 || g_bAnimateEverything) 
			{
				rgAnims[iShape].m_rgKeys[rgAnims[iShape].m_cKeys].m_iFrame	= (int)((float)iFrame * fTimeFactor);

				float*	rgfTRS;

				DtShapeGetMatrix(iShape, &rgfTRS);

				DtMatrixGetTransforms(rgfTRS, 
									  rgAnims[iShape].m_rgKeys[rgAnims[iShape].m_cKeys].m_rgfPosition, 
									  rgAnims[iShape].m_rgKeys[rgAnims[iShape].m_cKeys].m_rgfScale, 
									  rgAnims[iShape].m_rgKeys[rgAnims[iShape].m_cKeys].m_rgfQuaternion, 
									  rgAnims[iShape].m_rgKeys[rgAnims[iShape].m_cKeys].m_rgfRotation);

				rgAnims[iShape].m_cKeys++;
			}
		}
	}

	DtFrameSet((int)timeCurrent.value());

	delete[] rgrgiKeys;

	return hr;
}



void	freeAllAnims
		(
			Anim*	rgAnims
		) 
{
	for (int iShape = 0; iShape < DtShapeGetCount(); iShape++)
		delete[] rgAnims[iShape].m_rgKeys;
}


HRESULT	loadAnim
		(
			int		iShape, 
			Anim*	pAnim
		) 
{
	HRESULT	hr	= S_OK;


	char*	szName;

	DtShapeGetName(iShape, &szName);


	cout << "\treading " << szName << endl;

	int		cKeys	= 0;
	Key*	rgKeys	= NULL;


	MTime	timeStart(MAnimControl::minTime().value(), MTime::uiUnit());
	MTime	timeEnd(MAnimControl::maxTime().value(), MTime::uiUnit());
	MTime	timeCurrent(MAnimControl::currentTime().value(), MTime::uiUnit());

	MIntArray	rgiKeys;

	DtShapeGetTRSAnimKeys(iShape, &rgiKeys);


	cKeys	= rgiKeys.length();

	if (cKeys > 0) 
	{
		DtFrameSetStart(rgiKeys[0]);
		DtFrameSetEnd(rgiKeys[cKeys - 1]);

		// calculate the frames per second
		int	iFPS	= 1;

		switch(MTime::uiUnit()) 
		{
			case MTime::kSeconds:		// 1 fps
				iFPS	= 1;
				break;
			case MTime::kMilliseconds:	// 1000 fps
				iFPS	= 1000;
				break;
			case MTime::kGames:			// 15 fps
				iFPS	= 15;
				break;
			case MTime::kFilm:			// 24 fps
				iFPS	= 24;
				break;
			case MTime::kPALFrame:		// 25 fps
				iFPS	= 25;
				break;
			case MTime::kNTSCFrame:		// 30 fps
				iFPS	= 30;
				break;
			case MTime::kShowScan:		// 48 fps
				iFPS	= 48;
				break;
			case MTime::kPALField:		// 50 fps
				iFPS	= 50;
				break;
			case MTime::kNTSCField:		// 60 fps
				iFPS	= 60;
				break;
			default:
				iFPS	= 1;
				break;
		};


		float fTimeFactor	= 3600.0f / (float)iFPS;

		
		rgKeys	= new Key[cKeys];

		for (int iKey = 0; iKey < cKeys; iKey++) 
		{
			rgKeys[iKey].m_iFrame	= (int)((float)rgiKeys[iKey] * fTimeFactor);

			DtFrameSet(rgiKeys[iKey]);

			float*	rgfTRS;

			DtShapeGetMatrix(iShape, &rgfTRS);

			DtMatrixGetTransforms(rgfTRS, rgKeys[iKey].m_rgfPosition, rgKeys[iKey].m_rgfScale, rgKeys[iKey].m_rgfQuaternion, rgKeys[iKey].m_rgfRotation);
		}

		DtFrameSetStart((int)timeStart.value());
		DtFrameSetEnd((int)timeEnd.value());
		DtFrameSet((int)timeCurrent.value());
	}


	pAnim->m_szName	= new char[256];

	g_Strings.add(pAnim->m_szName);

	strcpy(pAnim->m_szName, szName);

	pAnim->m_cKeys	= cKeys; 
	pAnim->m_rgKeys	= rgKeys;


	return hr;
}











void	freeAnim
		(
			Anim*	pAnim
		) 
{
	delete[] pAnim->m_rgKeys;
}




//	Assumes that the shape is indeed a patch mesh.  This can be checked using MyDtShapeIsPatchMesh

HRESULT	LoadPatchMesh
		(
			int		iShape, 
			Mesh*	pShape
		) 
{
	HRESULT	hr		= S_OK;
	MStatus	mStat	= MS::kSuccess;

	
	// set up references

	Mesh::ShapeType&	kType		= pShape->m_kType;

	int&		cReps				= pShape->m_cReps;
	Rep*&		rgReps				= pShape->m_rgReps;

	int&		cVertices			= pShape->m_cVertices;
	DtVec3f*&	rgVertices			= pShape->m_rgVertices;

	int&		cNormals			= pShape->m_cNormals;
	DtVec3f*&	rgNormals			= pShape->m_rgNormals;

	int&		cTexCoords			= pShape->m_cTexCoords;
	DtVec2f*&	rgTexCoords			= pShape->m_rgTexCoords;

	int&		cVertexColors		= pShape->m_cVertexColors;
	DtRGBA*&	rgVertexColors		= pShape->m_rgVertexColors;


	int&		cFaces				= pShape->m_cFaces;
	Face*&		rgFaces				= pShape->m_rgFaces;

	int&		cFaceIndices		= pShape->m_cFaceIndices;

	int&		cGroups				= pShape->m_cGroups;
	Group*&		rgGroups			= pShape->m_rgGroups;


	int&		cBones				= pShape->m_cBones;
	Bone*&		rgBones				= pShape->m_rgBones;

	int&		cMaxBonesPerVertex	= pShape->m_cMaxBonesPerVertex;
	int&		cMaxBonesPerFace	= pShape->m_cMaxBonesPerFace;



	INIT;		// begin error checking

	// ensure that shape is a nurbs surface
	MObject	objNurb;

	DT_ATTEMPT(DtExt_ShapeGetShapeNode(iShape, objNurb));

	ASSERT(objNurb.hasFn(MFn::kNurbsSurface),	
				"Not a nurb surface");

	MFnNurbsSurface	fnNurb(objNurb);

	// ensure that surface is bicubic
	ASSERT(fnNurb.degreeU() == 3 && fnNurb.degreeV() == 3,	
				"Not a bicubic surface");

	// ensure correct form in U and V 
	int	kFormInU	= fnNurb.formInU();
	int	kFormInV	= fnNurb.formInV();

	ASSERT(kFormInU	== MFnNurbsSurface::kOpen || kFormInU == MFnNurbsSurface::kClosed,
				"Invalid form in U");

	ASSERT(kFormInV == MFnNurbsSurface::kOpen || kFormInV == MFnNurbsSurface::kClosed,
				"Invalid form in V");

	// ensure that surface is a quad mesh
	int	cCVsInU	= fnNurb.numCVsInU();
	int	cCVsInV	= fnNurb.numCVsInV();

	ASSERT((cCVsInU - 1) % 3 == 0 && (cCVsInV - 1) % 3 == 0,
				"Invalid CV count");

	// ensure that there is at least one patch
	int	cSpansInU	= (cCVsInU - 1) / 3;
	int	cSpansInV	= (cCVsInV - 1) / 3;

	ASSERT(cSpansInU > 0 && cSpansInV > 0, 
				"Invalid span count");


	// control vertices

	MPointArray rgCVs;

	fnNurb.getCVs(rgCVs);

	cVertices	= (int)rgCVs.length();
	rgVertices	= new DtVec3f[cVertices];

	ASSERT(rgVertices, 
				"Can't allocate memory for vertex array");

	for (int iVertex = 0; iVertex < cVertices; iVertex++) 
	{
		rgVertices[iVertex].vec[0]	= (float)rgCVs[iVertex][0];
		rgVertices[iVertex].vec[1]	= (float)rgCVs[iVertex][1];
		rgVertices[iVertex].vec[2]	= (float)rgCVs[iVertex][2];
	}



	// texture coordinates
	cTexCoords	= (int)rgCVs.length();
	rgTexCoords	= new DtVec2f[cTexCoords];

	ASSERT(rgTexCoords, 
				"Can't allocate memory for texture coordinate array");

	for (int iCVInU = 0, iTexCoord = 0; iCVInU < cCVsInU; iCVInU++)
	{
		for (int iCVInV = 0; iCVInV < cCVsInV; iCVInV++, iTexCoord++)
		{
			rgTexCoords[iTexCoord].vec[0] =  ((float)iCVInU) / ((float)(cCVsInU - 1));
			rgTexCoords[iTexCoord].vec[1] =  ((float)iCVInV) / ((float)(cCVsInV - 1));
		}
	}



	// face info
	cFaces	= cSpansInU * cSpansInV;
	rgFaces	= new Face[cFaces];

	ASSERT(rgFaces, 
				"Can't allocate memory for patch array");


	cFaceIndices	= 0;
			
	for (int iSpanInU = 0, iPatch = 0; iSpanInU < cSpansInU; iSpanInU++) 
	{
		int iCVIndexInU	= iSpanInU * 3;

		for (int iSpanInV = 0; iSpanInV < cSpansInV; iSpanInV++, iPatch++) 
		{
			int iCVIndexInV	= iSpanInV * 3;

			rgFaces[iPatch].m_cIndices		= 16;
            rgFaces[iPatch].m_rgIndices		= new int[rgFaces[iPatch].m_cIndices];

			ASSERT(rgFaces[iPatch].m_rgIndices,
						"Could not allocate memory for patch indices");

            rgFaces[iPatch].m_rgIndices[0]	= cCVsInV * (iCVIndexInU + 0) + (iCVIndexInV + 0);
            rgFaces[iPatch].m_rgIndices[1]	= cCVsInV * (iCVIndexInU + 1) + (iCVIndexInV + 0);
			rgFaces[iPatch].m_rgIndices[2]	= cCVsInV * (iCVIndexInU + 2) + (iCVIndexInV + 0);

            rgFaces[iPatch].m_rgIndices[3]	= cCVsInV * (iCVIndexInU + 3) + (iCVIndexInV + 0);
			rgFaces[iPatch].m_rgIndices[4]	= cCVsInV * (iCVIndexInU + 3) + (iCVIndexInV + 1);
			rgFaces[iPatch].m_rgIndices[5]	= cCVsInV * (iCVIndexInU + 3) + (iCVIndexInV + 2);

			rgFaces[iPatch].m_rgIndices[6]	= cCVsInV * (iCVIndexInU + 3) + (iCVIndexInV + 3);
			rgFaces[iPatch].m_rgIndices[7]	= cCVsInV * (iCVIndexInU + 2) + (iCVIndexInV + 3);
			rgFaces[iPatch].m_rgIndices[8]	= cCVsInV * (iCVIndexInU + 1) + (iCVIndexInV + 3);

			rgFaces[iPatch].m_rgIndices[9]	= cCVsInV * (iCVIndexInU + 0) + (iCVIndexInV + 3);
			rgFaces[iPatch].m_rgIndices[10]	= cCVsInV * (iCVIndexInU + 0) + (iCVIndexInV + 2);
			rgFaces[iPatch].m_rgIndices[11]	= cCVsInV * (iCVIndexInU + 0) + (iCVIndexInV + 1);

			rgFaces[iPatch].m_rgIndices[12]	= cCVsInV * (iCVIndexInU + 1) + (iCVIndexInV + 1);
			rgFaces[iPatch].m_rgIndices[13]	= cCVsInV * (iCVIndexInU + 2) + (iCVIndexInV + 1);

			rgFaces[iPatch].m_rgIndices[14]	= cCVsInV * (iCVIndexInU + 2) + (iCVIndexInV + 2);
			rgFaces[iPatch].m_rgIndices[15]	= cCVsInV * (iCVIndexInU + 1) + (iCVIndexInV + 2);

			rgFaces[iPatch].m_iGroup		= 0;			//	WARNING: assumes only 1 material per surface


			cFaceIndices	+= rgFaces[iPatch].m_cIndices;
		}
	}



	// material info

	cGroups		= DtGroupGetCount(iShape);
	rgGroups	= new Group[cGroups];

	ASSERT(rgGroups, 
				"Can't allocate memory for material group array");


	ASSERT(cGroups == 1,
				"Assumption was made that NURBS surfaces have only 1 material");


	for (int iGroup	= 0; iGroup < cGroups; iGroup++)
	{

		// material name
		DT_ATTEMPT(DtMtlGetName(iShape, iGroup, &rgGroups[iGroup].m_szMaterial));

		// texture file name
		DT_ATTEMPT(MyDtTextureGetFileName(rgGroups[iGroup].m_szMaterial, &rgGroups[iGroup].m_szTextureFile));

		// diffuse color 
		if (!rgGroups[iGroup].m_szTextureFile)
		{
			DT_ATTEMPT(DtMtlGetDiffuseClr(rgGroups[iGroup].m_szMaterial, 0, 
									   &rgGroups[iGroup].m_fDiffuseRed, 
									   &rgGroups[iGroup].m_fDiffuseGreen,
									   &rgGroups[iGroup].m_fDiffuseBlue));
		}
		else 	// material has a texture
		{
			//	load the diffuse factor into the diffuse components

			int		iMaterial;

			DT_ATTEMPT(DtMtlGetID(iShape, iGroup, &iMaterial));

			MObject	mShader;

			DT_ATTEMPT(DtExt_MtlGetShader(iMaterial, mShader));

			MFnLambertShader	fnShader;

			fnShader.setObject(mShader);

			float	fDiffuseFactor	= fnShader.diffuseCoeff();

			rgGroups[iGroup].m_fDiffuseRed	= fDiffuseFactor;
			rgGroups[iGroup].m_fDiffuseGreen	= fDiffuseFactor;
			rgGroups[iGroup].m_fDiffuseBlue	= fDiffuseFactor;
		}

		// specular color
		DT_ATTEMPT(DtMtlGetSpecularClr(rgGroups[iGroup].m_szMaterial, 0, 
									&rgGroups[iGroup].m_fSpecularRed, 
									&rgGroups[iGroup].m_fSpecularGreen, 
									&rgGroups[iGroup].m_fSpecularBlue));

		// emissive color
		DT_ATTEMPT(DtMtlGetEmissiveClr(rgGroups[iGroup].m_szMaterial, 0, 
									&rgGroups[iGroup].m_fEmissiveRed, 
									&rgGroups[iGroup].m_fEmissiveGreen, 
									&rgGroups[iGroup].m_fEmissiveBlue));

		// power / shininess
		DT_ATTEMPT(DtMtlGetShininess(rgGroups[iGroup].m_szMaterial, 0, &rgGroups[iGroup].m_fShininess));

		// transparency / alpha
		DT_ATTEMPT(DtMtlGetTransparency(rgGroups[iGroup].m_szMaterial, 0, &rgGroups[iGroup].m_fTransparency));
	}
	




	// vertex duplication info (very simple for patch meshes)

	cReps	= (int)rgCVs.length();
	rgReps	= new Rep[cReps];

	ASSERT(rgReps, 
				"Can't allocate memory for rep array");

	for (int iRep = 0; iRep < cReps; iRep++) 
	{
		rgReps[iRep].m_iNormalIdx	= -1;			// patches don't export normal info
		rgReps[iRep].m_iTexCoordIdx	= iRep;
		rgReps[iRep].m_iFirst		= iRep;
		rgReps[iRep].m_iNext		= iRep;

		rgReps[iRep].m_cReps		= 1;
	}



	// skinning info
			

	MObject	objShape;
	MObject objTransform;
	MObject	objInput;

	DT_ATTEMPT(DtExt_ShapeGetShapeNode(iShape, objShape));
	DT_ATTEMPT(DtExt_ShapeGetTransform(iShape, objTransform));




	// load the mesh's world transform (needed if skinning info is found)
	MDagPath	pathTransform;

	MFnDagNode(objTransform).getPath(pathTransform);

	MMatrix		matMeshWorldTransform	= pathTransform.inclusiveMatrix();





	cBones				= 0;
	rgBones				= NULL;

	cMaxBonesPerVertex	= 0;
	cMaxBonesPerFace	= 0;


	MObjectArray	rgobjBones;







	// smooth skinning

	bool*	rgbNonZeroFlagTable	= NULL;		// table of influences vs. vertices
	int*	rgcNonZeros			= NULL;		// array of influence counts 

	bool bFoundSmoothSkin	= false;

	if (objShape.hasFn(MFn::kNurbsSurface)) 		// if this shape is a mesh
	{ 
		// loop through skin clusters
		for (MItDependencyNodes itSkin(MFn::kSkinClusterFilter); !itSkin.isDone(); itSkin.next()) 
		{
			MFnSkinCluster fnSkin(itSkin.item());

			// load input and output geometries
			MObjectArray	rgInputs;
			MObjectArray	rgOutputs;

			fnSkin.getInputGeometry(rgInputs);
			fnSkin.getOutputGeometry(rgOutputs);

			assert(rgInputs.length() == rgOutputs.length());		// ensure that input geometry count 
																	// equals output geometry count

			int	cInputs, cOutputs;

			cInputs	= cOutputs	= (int)rgOutputs.length();

			// loop through the output geometries
			for (int iOutput = 0, iInput = 0; iOutput < cOutputs; iOutput++, iInput++) 
			{
				assert(iOutput == iInput);		// sanity check

				
				if (rgOutputs[iOutput] == objShape) 		// if our shape is one of the output geometries
				{
					MDagPathArray	rgdagpathInfluences;
					
					cBones	= (int)fnSkin.influenceObjects(rgdagpathInfluences, &mStat);

					rgBones	= new Bone[cBones];

					ASSERT(rgBones,
								"Could not allocate memory for bone array");


					// initialize bones
					for (int iBone = 0; iBone < cBones; iBone++) 
					{	// WARNING: not checking for new failure
						rgBones[iBone].m_szName			= new char[256];
						rgBones[iBone].m_cReps			= 0;
						rgBones[iBone].m_cWeights		= 0;
						rgBones[iBone].m_rgiVertices	= new int[cVertices];
						rgBones[iBone].m_rgfWeights		= new float[cVertices];

						g_Strings.add(rgBones[iBone].m_szName);		// housekeeping

						// bone name
						strcpy(rgBones[iBone].m_szName, rgdagpathInfluences[iBone].partialPathName().asChar());

						// matrix offset
						MFnIkJoint fnBone(rgdagpathInfluences[iBone]);

						MObject objBindPose;

						fnBone.findPlug("bindPose").getValue(objBindPose);

						MFnMatrixData fnBindPose(objBindPose);

						(matMeshWorldTransform * fnBindPose.matrix().inverse()).get(rgBones[iBone].m_matOffset);



						rgobjBones.append(rgdagpathInfluences[iBone].node());
					}

					rgcNonZeros			= new int[cVertices];

					ASSERT(rgcNonZeros,
								"Could not allocate memory for non zero count array");

					rgbNonZeroFlagTable	= new bool[cVertices * cBones];

					ASSERT(rgbNonZeroFlagTable,
								"Could not allocate memory for non zero table");

					// bone info; calculate max number of bones per vertex
					cMaxBonesPerVertex = 0;

					int iVertex = 0;

					MFnNurbsSurface fnOutput(rgOutputs[iOutput]);

					MDagPath dagpathOutputShape;
				
					fnOutput.getPath(dagpathOutputShape);

					// loop through the vertices
					for (MItGeometry itGeom(rgOutputs[iOutput]); !itGeom.isDone(); itGeom.next()) 
					{
	
						MFloatArray rgfWeights;

						unsigned cInfs;

						fnSkin.getWeights(dagpathOutputShape, itGeom.component(), rgfWeights, cInfs);

						int a = rgdagpathInfluences.length();
						int b = rgfWeights.length();
						int c = itGeom.count();

						assert(rgdagpathInfluences.length() == rgfWeights.length());
						assert(rgfWeights.length() == cInfs);

						rgcNonZeros[iVertex] = 0;


						float fWeightSum = 0.0f;

						for (int iBone = 0; iBone < cBones; iBone++)
							fWeightSum += rgfWeights[iBone];

						assert(fWeightSum > 0.00001f);

						for (iBone = 0; iBone < cBones; iBone++) 
						{
							rgbNonZeroFlagTable[iBone * cVertices + iVertex]	= false;

							rgfWeights[iBone] = rgfWeights[iBone] / fWeightSum;		// normalize the weight

							if (rgfWeights[iBone] != 0.0f) 
							{
								rgcNonZeros[iVertex]++;

								rgBones[iBone].m_cReps += rgReps[iVertex].m_cReps;
		
								rgBones[iBone].m_rgiVertices[rgBones[iBone].m_cWeights]	= iVertex;							
								rgBones[iBone].m_rgfWeights[rgBones[iBone].m_cWeights]	= rgfWeights[iBone];

								rgbNonZeroFlagTable[iBone * cVertices + iVertex]	= true;

								rgBones[iBone].m_cWeights++;
							}
						}


						if (rgcNonZeros[iVertex] > cMaxBonesPerVertex)
							cMaxBonesPerVertex = rgcNonZeros[iVertex];

						iVertex++;
					}



					// calculate max number of bones per vertex

					cMaxBonesPerFace	= 0;

					for (int iFace = 0; iFace < cFaces; iFace++) 
					{
						int	cBonesPerFace	= 0;

						for (int iBone = 0; iBone < cBones; iBone++) 
						{
							for (int iIndex = 0; iIndex < rgFaces[iFace].m_cIndices; iIndex++) 
							{
								if (rgbNonZeroFlagTable[iBone * cVertices + rgReps[rgFaces[iFace].m_rgIndices[iIndex]].m_iFirst]) 
								{
									cBonesPerFace++;

									break;
								}
							}
						}

						if (cBonesPerFace > cMaxBonesPerFace)
							cMaxBonesPerFace = cBonesPerFace;
					}




					objInput = rgInputs[iInput];

					bFoundSmoothSkin = true;

					break;
				}
			}

			if (bFoundSmoothSkin)
				break;
		}
	}


	delete[] rgcNonZeros;
	delete[] rgbNonZeroFlagTable;



















	// rigid skinning

	rgbNonZeroFlagTable	= NULL;

	bool	bFoundRigidSkin	= false;

	if (!bFoundSmoothSkin) 	
	{
		cBones			= 1;						// zero'th bone is the extra "fake" bone
		int cBonesMax	= 64;
		rgBones			= new Bone[cBonesMax];

		ASSERT(rgBones,
					"Could not allocate memory for bone array");

		rgbNonZeroFlagTable	= new bool[cBonesMax * cVertices];

		ASSERT(rgbNonZeroFlagTable,
					"Could not allocate memory for non-zero flag table");

		// fill non zero table with 0's
		memset(rgbNonZeroFlagTable, 0, cBonesMax * cVertices * sizeof(bool));

		// initialize "fake" iBone
		// WARNING: not checking for new failure
		rgBones[0].m_szName			= new char[256];
		rgBones[0].m_cReps			= 0;
		rgBones[0].m_cWeights		= 0;
		rgBones[0].m_rgfWeights		= new float[cVertices];
		rgBones[0].m_rgiVertices	= new int[cVertices];

		g_Strings.add(rgBones[0].m_szName);							// housekeeping

		strcpy(rgBones[0].m_szName, SCENE_ROOT);					// bone name
	
		matMeshWorldTransform.get(rgBones[0].m_matOffset);			// "fake" bone has identity matrix


		// loop through joint clusters
		for (MItDependencyNodes itCluster(MFn::kJointCluster); !itCluster.isDone(); itCluster.next()) 
		{
			MFnWeightGeometryFilter fnCluster(itCluster.item());

			// load input and output geometries
			MObjectArray	rgInputs;
			MObjectArray	rgOutputs;

			fnCluster.getInputGeometry(rgInputs);
			fnCluster.getOutputGeometry(rgOutputs);

			assert(rgInputs.length() == rgOutputs.length());	// ensure input geometry count equals 
																// output geometry count

			int	cInputs, cOutputs;

			cInputs	= cOutputs	
				= (int)rgOutputs.length();

			// loop through the output geometries
			for (int iOutput = 0, iInput = 0; iOutput < cOutputs; iOutput++, iInput++) 
			{
				assert(iOutput == iInput);
				
				if (rgOutputs[iOutput] == objShape) 	// our shape is one of the output geometries
				{
					bFoundRigidSkin	= true;
		
					assert(rgInputs[iInput] == fnCluster.inputShapeAtIndex(iInput));	// sanity check

					objInput	= rgInputs[iInput];

					// get bone
					MPlug		plgMatrix	= fnCluster.findPlug("matrix", &mStat);

					MPlugArray	rgplgMatrixConnections;

					plgMatrix.connectedTo(rgplgMatrixConnections, true, false);			// get source plugs
					assert(rgplgMatrixConnections.length() == 1);

					MObject	objBone	= rgplgMatrixConnections[0].node();

					assert(objBone.hasFn(MFn::kJoint));

					MFnIkJoint fnBone(objBone);

					char	szBone[64];

					strcpy(szBone, fnBone.name().asChar());

					// find bone's index in current bone list
					for (int iBone = 1; iBone < cBones;	iBone++) 
					{
						if (!strcmp(rgBones[iBone].m_szName, szBone))
							break;
					}
	
					if (iBone == cBones) 	// bone was not found in current bone list
					{
						// add bone
						if (cBones >= cBonesMax) 
						{
							// double array size
							cBonesMax  += cBonesMax;

							Bone*	rgNewBones	= new Bone[cBonesMax];

							ASSERT(rgNewBones, 
										"Could not allocate memory for new bone array");

							memcpy(rgNewBones, rgBones, cBones * sizeof(Bone));

							delete[] rgBones;

							rgBones	= rgNewBones;


							bool*	rgbNewNonZeroFlagTable	= new bool[cBonesMax * cVertices];

							ASSERT(rgbNewNonZeroFlagTable, 
										"Could not allocate memory for new non-zero flag table");

							memset(rgbNewNonZeroFlagTable, 0, cBonesMax * cVertices * sizeof(bool));
							memcpy(rgbNewNonZeroFlagTable, rgbNonZeroFlagTable, cBones * cVertices * sizeof(bool));

							delete[] rgbNonZeroFlagTable;

							rgbNonZeroFlagTable	= rgbNewNonZeroFlagTable;
						}
		
						// initialize iBone
						// WARNING: not checking for new failure
						rgBones[iBone].m_szName			= new char[256];
						rgBones[iBone].m_cReps			= 0;
						rgBones[iBone].m_cWeights		= 0;
						rgBones[iBone].m_rgiVertices	= new int[cVertices];
						rgBones[iBone].m_rgfWeights		= new float[cVertices];

						g_Strings.add(rgBones[iBone].m_szName);							// housekeeping

						strcpy(rgBones[iBone].m_szName, szBone);						// bone name
	
						// matrix info
						MObject objBindPose;

						fnBone.findPlug("bindPose").getValue(objBindPose);

						MFnMatrixData fnBindPose(objBindPose);

						(matMeshWorldTransform * fnBindPose.matrix().inverse()).get(rgBones[iBone].m_matOffset);


						rgobjBones.append(objBone);

						cBones++;
					}



					char	szParent[64];

					bool	bFoundParent	= false;
					MObject	objParent; 

					for (int iParent = 0; iParent < (int)fnBone.parentCount(); iParent++) 
					{
						objParent	= fnBone.parent(iParent);

						MFnDagNode	fnParent(objParent);
						
						strcpy(szParent, fnParent.name().asChar());		// parent's name

						for (int iShape_ = 0; iShape_ < DtShapeGetCount(); iShape_++) 
						{
							char*	szShape;

							DT_ATTEMPT(DtShapeGetName(iShape_, &szShape));

							if (!strcmp(szParent, szShape)) 
							{
								bFoundParent	= true;

								break;
							}
						}

						if (bFoundParent)
							break;
					}
  
					iParent	= 0;

					if (bFoundParent) 	// parent shape found
					{
						// find parent bone's index in current bone list
						for (iParent = 1; iParent < cBones;	iParent++) 
						{
							if (!strcmp(rgBones[iParent].m_szName, szParent)) 
							{
								break;
							}
						}


						if (iParent == cBones) 		// parent bone was not found in current bone list
						{
							// add parent bone
							if (cBones >= cBonesMax) 
							{
								// double array size
								cBonesMax  += cBonesMax;

								Bone*	rgNewBones	= new Bone[cBonesMax];

								ASSERT(rgNewBones, 
											"Could not allocate memory for new bone array");
	
								memcpy(rgNewBones, rgBones, cBones * sizeof(Bone));
	
								delete[] rgBones;

								rgBones	= rgNewBones;

								
								bool*	rgbNewNonZeroFlagTable	= new bool[cBonesMax * cVertices];

								ASSERT(rgbNewNonZeroFlagTable, 
											"Could not allocate memory for new non-zero flag table");

								memset(rgbNewNonZeroFlagTable, 0, cBonesMax * cVertices * sizeof(bool));
								memcpy(rgbNewNonZeroFlagTable, rgbNonZeroFlagTable, cBones * cVertices * sizeof(bool));

								delete[] rgbNonZeroFlagTable;

								rgbNonZeroFlagTable	= rgbNewNonZeroFlagTable;
							}
			
							// initialize iBone
							// WARNING: not checking for new failure
							rgBones[iParent].m_szName		= new char[256];
							rgBones[iParent].m_cReps		= 0;
							rgBones[iParent].m_cWeights		= 0;
							rgBones[iParent].m_rgiVertices	= new int[cVertices];
							rgBones[iParent].m_rgfWeights	= new float[cVertices];
	
							g_Strings.add(rgBones[iParent].m_szName);			// housekeeping
	
							strcpy(rgBones[iParent].m_szName, szParent);		// bone name

							// matrix info
							MObject	objBindPose;
	
							assert(objParent.hasFn(MFn::kJoint));

							MFnIkJoint(objParent).findPlug("bindPose").getValue(objBindPose);
	
							MFnMatrixData fnBindPose(objBindPose);
	
							(matMeshWorldTransform * fnBindPose.matrix().inverse()).get(rgBones[iParent].m_matOffset);


							rgobjBones.append(objParent);

							cBones++;
						}
					}


					// load weights
					MPlug		plgMessage	= fnCluster.findPlug("message");

					MPlugArray	rgplgMessageConnections;

					plgMessage.connectedTo(rgplgMessageConnections, false, true);	// get destination plugs

					assert(rgplgMessageConnections.length() == 1);
					assert(rgplgMessageConnections[0].node().hasFn(MFn::kSet));

					MFnSet fnSet(rgplgMessageConnections[0].node());
				
					MSelectionList list;

					fnSet.getMembers(list, false);

					assert(list.length() == 1);

					MDagPath	path;
					MObject		objComponents;

					list.getDagPath(0, path, objComponents);

					MFloatArray	rgWeights;

					fnCluster.getWeights(path, objComponents, rgWeights);

					assert(objComponents.hasFn(MFn::kDoubleIndexedComponent));


					MFnDoubleIndexedComponent fnComponent(objComponents);

					assert(fnComponent.elementCount() == (int)rgWeights.length());

					// loop through the weights
					for (int iWeight = 0; iWeight < (int)rgWeights.length(); iWeight++) 
					{
						assert(rgWeights[iWeight] <= 1.0f);

						int	iU, iV;

						fnComponent.getElement(iWeight, iU, iV);

						// WARNING: check calculation of iVertex
						int	iVertex	= iU * cCVsInV + iV;
						
						rgBones[iBone].m_rgfWeights[rgBones[iBone].m_cWeights]	= rgWeights[iWeight];
						rgBones[iBone].m_rgiVertices[rgBones[iBone].m_cWeights]	= iVertex;

						rgBones[iBone].m_cReps	 += rgReps[iVertex].m_cReps;
						rgBones[iBone].m_cWeights++;

						rgbNonZeroFlagTable[iBone * cVertices + iVertex]	= true;

						if (rgWeights[iWeight] != 1.0f) 
						{
							rgBones[iParent].m_rgfWeights[rgBones[iParent].m_cWeights]	= 1.0f - rgWeights[iWeight];
							rgBones[iParent].m_rgiVertices[rgBones[iParent].m_cWeights]	= iVertex;

							rgBones[iParent].m_cReps   += rgReps[iVertex].m_cReps;
							rgBones[iParent].m_cWeights++;				// IMPORTANT: Don't change line position

							rgbNonZeroFlagTable[iParent * cVertices + iVertex]	= true;
						}
					}

					break;
				}	// if found our mesh
			}	// loop thru geom's
		}	// loop thru joint clusters


		if (cBones == 1) 		// no rigid skinning found
		{
			delete[] rgBones[0].m_rgfWeights;
			delete[] rgBones[0].m_rgiVertices;

			cBones	= 0;
		}
		else 
		{
			// at most 2 bones per vertex in rigid skinning (i.e. bone + parent)
			cMaxBonesPerVertex	= 2;

			// calculate max number of bones per vertex
			cMaxBonesPerFace	= 0;

			for (int iFace = 0; iFace < cFaces; iFace++) 
			{
				int	cBonesPerFace	= 0;

				for (int iBone = 0; iBone < cBones; iBone++) 
				{
					for (int iIndex = 0; iIndex < rgFaces[iFace].m_cIndices; iIndex++) 
					{
						if (rgbNonZeroFlagTable[iBone * cVertices + rgReps[rgFaces[iFace].m_rgIndices[iIndex]].m_iFirst]) 
						{
							cBonesPerFace++;

							break;
						}
					}
				}

				if (cBonesPerFace > cMaxBonesPerFace)
					cMaxBonesPerFace = cBonesPerFace;
			}
			
		}
	}
				
				
	delete[] rgbNonZeroFlagTable;



















	// reload control vertices if skinning info was found

	if (bFoundSmoothSkin || bFoundRigidSkin)
	{
		delete[]	rgVertices;

		MyDtShapeGetControlPoints(objInput, objShape, &cVertices, &rgVertices);
	}





	// mesh type
	pShape->m_kType			= Mesh::PATCH_MESH;


	EXIT;

	return hr;	
}










HRESULT	LoadPolyMesh
		(
			int		iShape, 
			Mesh*	pShape
		) 
{
	HRESULT	hr		= S_OK;
	MStatus	mStat	= MStatus::kSuccess;


	// set up references

	Mesh::ShapeType&	kType		= pShape->m_kType;

	int&		cReps				= pShape->m_cReps;
	Rep*&		rgReps				= pShape->m_rgReps;

	int&		cVertices			= pShape->m_cVertices;
	DtVec3f*&	rgVertices			= pShape->m_rgVertices;

	int&		cNormals			= pShape->m_cNormals;
	DtVec3f*&	rgNormals			= pShape->m_rgNormals;

	int&		cTexCoords			= pShape->m_cTexCoords;
	DtVec2f*&	rgTexCoords			= pShape->m_rgTexCoords;

	int&		cVertexColors		= pShape->m_cVertexColors;
	DtRGBA*&	rgVertexColors		= pShape->m_rgVertexColors;


	int&		cFaces				= pShape->m_cFaces;
	Face*&		rgFaces				= pShape->m_rgFaces;

	int&		cFaceIndices		= pShape->m_cFaceIndices;

	int&		cGroups				= pShape->m_cGroups;
	Group*&		rgGroups			= pShape->m_rgGroups;


	int&		cBones				= pShape->m_cBones;
	Bone*&		rgBones				= pShape->m_rgBones;

	int&		cMaxBonesPerVertex	= pShape->m_cMaxBonesPerVertex;
	int&		cMaxBonesPerFace	= pShape->m_cMaxBonesPerFace;




	INIT;




	cGroups		= DtGroupGetCount(iShape);
	rgGroups	= new Group[cGroups];

	ASSERT(rgGroups,
				"Could not allocate memory for group array");

	DT_ATTEMPT(DtShapeGetVertices(iShape, &cVertices, &rgVertices));

	DT_ATTEMPT(DtShapeGetNormals(iShape, &cNormals, &rgNormals));

	DT_ATTEMPT(DtShapeGetTextureVertices(iShape, &cTexCoords, &rgTexCoords));
	
	DT_ATTEMPT(DtShapeGetVerticesColor(iShape, &cVertexColors, &rgVertexColors));	
	
	ASSERT(cVertexColors == cVertices,
				"Vertex color count does not match vertex count");


	int	cRepsMax	= cReps	
					= cVertices;
	rgReps			= new Rep[cRepsMax];

	ASSERT(rgReps,
				"Could not allocate memory for rep array");
	
	// initialize reps
	for (int iRep = 0; iRep < cReps; iRep++) 
	{
		rgReps[iRep].m_iTexCoordIdx	= -1;						// signifies "unvisited"
		rgReps[iRep].m_iNormalIdx	= -1;						// signifies "unvisited"
		rgReps[iRep].m_iNext		= iRep;
		rgReps[iRep].m_iFirst		= iRep;

		rgReps[iRep].m_cReps		= 1;
	}


	int	cFacesMax	= cFaces
					= 0;
	rgFaces			= new Face[cFacesMax];

	ASSERT(rgFaces,
				"Could not allocate memory for face array");

	cFaceIndices	= 0;				// total number of indices over all faces


	// go through each group of polygons (grouped by material)
	for (int iGroup = 0; iGroup < cGroups; iGroup++) 
	{
		// material name
		DT_ATTEMPT(DtMtlGetName(iShape, iGroup, &rgGroups[iGroup].m_szMaterial));

		// texture file name

		DT_ATTEMPT(MyDtTextureGetFileName(rgGroups[iGroup].m_szMaterial, &rgGroups[iGroup].m_szTextureFile));


		// diffuse color 
		if (!rgGroups[iGroup].m_szTextureFile)
		{
			DT_ATTEMPT(DtMtlGetDiffuseClr(rgGroups[iGroup].m_szMaterial, 0, 
									   &rgGroups[iGroup].m_fDiffuseRed, 
									   &rgGroups[iGroup].m_fDiffuseGreen, 
									   &rgGroups[iGroup].m_fDiffuseBlue));
		}
		else 
		{	// if the material has a texture then load the diffuse factor into the diffuse components
			int		iMaterial;

			DT_ATTEMPT(DtMtlGetID(iShape, iGroup, &iMaterial));

			MObject	objShader;

			DT_ATTEMPT(DtExt_MtlGetShader(iMaterial, objShader));

			MFnLambertShader	fnShader(objShader);

			float	fDiffuseFactor	= fnShader.diffuseCoeff();

			rgGroups[iGroup].m_fDiffuseRed		= fDiffuseFactor;
			rgGroups[iGroup].m_fDiffuseGreen	= fDiffuseFactor;
			rgGroups[iGroup].m_fDiffuseBlue		= fDiffuseFactor;
		}

		// specular color
		DT_ATTEMPT(DtMtlGetSpecularClr(rgGroups[iGroup].m_szMaterial, 0, 
										&rgGroups[iGroup].m_fSpecularRed, 
										&rgGroups[iGroup].m_fSpecularGreen, 
										&rgGroups[iGroup].m_fSpecularBlue));

		// emissive color
		DT_ATTEMPT(DtMtlGetEmissiveClr(rgGroups[iGroup].m_szMaterial, 0, 
										&rgGroups[iGroup].m_fEmissiveRed, 
										&rgGroups[iGroup].m_fEmissiveGreen, 
										&rgGroups[iGroup].m_fEmissiveBlue));

		// power / shininess
		DT_ATTEMPT(DtMtlGetShininess(rgGroups[iGroup].m_szMaterial, 0, &rgGroups[iGroup].m_fShininess));

		// transparency / alpha
		DT_ATTEMPT(DtMtlGetTransparency(rgGroups[iGroup].m_szMaterial, 0, &rgGroups[iGroup].m_fTransparency));


		int	cPolygons;

		DT_ATTEMPT(DtPolygonGetCount(iShape, iGroup, &cPolygons));


		// increase array size
		cFacesMax  += cPolygons;

		Face*	rgNewFaces	= new Face[cFacesMax];
		
		ASSERT(rgNewFaces,
					"Could not allocate memory for new face array");

		memcpy(rgNewFaces, rgFaces, cFaces * sizeof(Face));

		delete[] rgFaces;

		rgFaces	= rgNewFaces;

		// loop through the polygons in this shape
		for (int iPolygon = 0; iPolygon < cPolygons; iPolygon++) 
		{
			int		cIndices;

			long*	rglVertexIndices;
			long*	rglNormalIndices;
			long*	rglTexCoordIndices;

			DT_ATTEMPT(DtPolygonGetIndices(iPolygon, &cIndices, &rglVertexIndices, &rglNormalIndices, &rglTexCoordIndices));

			cFaceIndices	+=	cIndices;

			// initialize face
			rgFaces[cFaces].m_cIndices	= cIndices;
			rgFaces[cFaces].m_rgIndices	= new int[cIndices];

			ASSERT(rgFaces[cFaces].m_rgIndices,
						"Could not allocate memory for face indices array");
			
			rgFaces[cFaces].m_iGroup	= iGroup;

			// no need for the following 2 lines
////		memcpy(rgFaces[cFaces].m_rgNormalIndices, rgNormalIndices, cIndices * sizeof(long));
//			memcpy(rgFaces[cFaces].m_rgIndices, rgVertexIndices, cIndices * sizeof(long));

			// create repetition if texture coords differ
			for (int iIndex = 0; iIndex < cIndices; iIndex++) 
			{
				int iRep	= rglVertexIndices[iIndex];
				int iLastRep;

				bool bFound = false;

				do 
				{
					if (rglTexCoordIndices[iIndex] == rgReps[iRep].m_iTexCoordIdx && rglNormalIndices[iIndex] == rgReps[iRep].m_iNormalIdx)
						bFound = true;

					iLastRep	= iRep;
					iRep		= rgReps[iRep].m_iNext;
				} while (!bFound && rgReps[iRep].m_iNext != rgReps[iRep].m_iFirst);


				if (bFound) 
				{
					rgFaces[cFaces].m_rgIndices[iIndex]	= iLastRep;	// update face indices
				}
				else 
				{
					if (rgReps[rgReps[iRep].m_iFirst].m_iTexCoordIdx == -1) 	// if first time through this rep
					{
						// initialize index into lump of texture coordinates
						rgReps[rgReps[iRep].m_iFirst].m_iTexCoordIdx	= rglTexCoordIndices[iIndex];
						rgReps[rgReps[iRep].m_iFirst].m_iNormalIdx		= rglNormalIndices[iIndex];

						// update face indices
						rgFaces[cFaces].m_rgIndices[iIndex] = rgReps[iRep].m_iFirst;
					}
					else 
					{
						// append new rep 

						if (cReps >= cRepsMax) 
						{
							// double array size
							cRepsMax += cRepsMax;

							Rep*	rgNewReps	= new Rep[cRepsMax];

							ASSERT(rgNewReps,
										"Could not allocate memory for new rep array");

							memcpy(rgNewReps, rgReps, cReps * sizeof(Rep));

							delete[] rgReps;

							rgReps	  = rgNewReps;
						}

						// create new rep at the end of the array
						rgReps[cReps].m_iTexCoordIdx	= rglTexCoordIndices[iIndex];
						rgReps[cReps].m_iNormalIdx		= rglNormalIndices[iIndex];
						rgReps[cReps].m_iFirst			= rgReps[iRep].m_iFirst;
						rgReps[cReps].m_iNext			= rgReps[iRep].m_iFirst;

						rgReps[iRep].m_iNext			= cReps;

						// increment rep count at the first rep
						rgReps[rgReps[iRep].m_iFirst].m_cReps++;


						// update face indices
						rgFaces[cFaces].m_rgIndices[iIndex] = cReps;	

						cReps++;
					}
				}
			}

			cFaces++;
		}
	}







			

	
	// skinning info
			

	MObject	objShape;
	MObject objTransform;
	MObject	objInput;

	DT_ATTEMPT(DtExt_ShapeGetShapeNode(iShape, objShape));
	DT_ATTEMPT(DtExt_ShapeGetTransform(iShape, objTransform));




	// load the mesh's world transform (needed if skinning info is found)
	MDagPath	pathTransform;

	MFnDagNode(objTransform).getPath(pathTransform);

	MMatrix		matMeshWorldTransform	= pathTransform.inclusiveMatrix();





	cBones				= 0;
	rgBones				= NULL;

	cMaxBonesPerVertex	= 0;
	cMaxBonesPerFace	= 0;


	MObjectArray	rgobjBones;







	// smooth skinning

	bool*	rgbNonZeroFlagTable	= NULL;		// table of influences vs. vertices
	int*	rgcNonZeros			= NULL;		// array of influence counts 

	bool bFoundSmoothSkin	= false;

	if (objShape.hasFn(MFn::kMesh)) 		// if this shape is a mesh
	{
		// loop through skin clusters
		for (MItDependencyNodes itSkin(MFn::kSkinClusterFilter); !itSkin.isDone(); itSkin.next()) 
		{
			MFnSkinCluster fnSkin(itSkin.item());

			// load input and output geometries
			MObjectArray	rgInputs;
			MObjectArray	rgOutputs;

			fnSkin.getInputGeometry(rgInputs);
			fnSkin.getOutputGeometry(rgOutputs);

			assert(rgInputs.length() == rgOutputs.length());		// ensure that input geometry count 
																	// equals output geometry count

			int	cInputs, cOutputs;

			cInputs	= cOutputs	= (int)rgOutputs.length();

			// loop through the output geometries
			for (int iOutput = 0, iInput = 0; iOutput < cOutputs; iOutput++, iInput++) 
			{
				assert(iOutput == iInput);		// sanity check

				
				if (rgOutputs[iOutput] == objShape) 		// if our shape is one of the output geometries
				{
					MDagPathArray	rgdagpathInfluences;
					
					cBones	= (int)fnSkin.influenceObjects(rgdagpathInfluences, &mStat);

					rgBones	= new Bone[cBones];

					ASSERT(rgBones,
								"Could not allocate memory for bone array");


					// initialize bones
					for (int iBone = 0; iBone < cBones; iBone++) 
					{	// WARNING: not checking for new failure
						rgBones[iBone].m_szName			= new char[256];
						rgBones[iBone].m_cReps			= 0;
						rgBones[iBone].m_cWeights		= 0;
						rgBones[iBone].m_rgiVertices	= new int[cVertices];
						rgBones[iBone].m_rgfWeights		= new float[cVertices];

						g_Strings.add(rgBones[iBone].m_szName);		// housekeeping

						// bone name
						strcpy(rgBones[iBone].m_szName, rgdagpathInfluences[iBone].partialPathName().asChar());

						// matrix offset
						MFnIkJoint fnBone(rgdagpathInfluences[iBone]);

						MObject objBindPose;

						fnBone.findPlug("bindPose").getValue(objBindPose);

						MFnMatrixData fnBindPose(objBindPose);

						(matMeshWorldTransform * fnBindPose.matrix().inverse()).get(rgBones[iBone].m_matOffset);



						rgobjBones.append(rgdagpathInfluences[iBone].node());
					}

					rgcNonZeros			= new int[cVertices];

					ASSERT(rgcNonZeros,
								"Could not allocate memory for non zero count array");

					rgbNonZeroFlagTable	= new bool[cVertices * cBones];

					ASSERT(rgbNonZeroFlagTable,
								"Could not allocate memory for non zero table");

					// bone info; calculate max number of bones per vertex
					cMaxBonesPerVertex = 0;

					int iVertex = 0;

					MFnMesh fnOutput(rgOutputs[iOutput]);

					MDagPath dagpathOutputShape;
				
					fnOutput.getPath(dagpathOutputShape);

					// loop through the vertices
					for (MItGeometry itGeom(rgOutputs[iOutput]); !itGeom.isDone(); itGeom.next()) 
					{
						MFloatArray rgfWeights;

						unsigned cInfs;

						fnSkin.getWeights(dagpathOutputShape, itGeom.component(), rgfWeights, cInfs);

						assert(rgdagpathInfluences.length() == rgfWeights.length());
						assert(rgfWeights.length() == cInfs);

						rgcNonZeros[iVertex] = 0;


						float fWeightSum = 0.0f;

						for (int iBone = 0; iBone < cBones; iBone++)
							fWeightSum += rgfWeights[iBone];

						assert(fWeightSum > 0.00001f);

						for (iBone = 0; iBone < cBones; iBone++) 
						{
							rgbNonZeroFlagTable[iBone * cVertices + iVertex]	= false;

							rgfWeights[iBone] = rgfWeights[iBone] / fWeightSum;		// normalize the weight

							if (rgfWeights[iBone] != 0.0f) 
							{
								rgcNonZeros[iVertex]++;

								rgBones[iBone].m_cReps += rgReps[iVertex].m_cReps;
		
								rgBones[iBone].m_rgiVertices[rgBones[iBone].m_cWeights]	= iVertex;							
								rgBones[iBone].m_rgfWeights[rgBones[iBone].m_cWeights]	= rgfWeights[iBone];

								rgbNonZeroFlagTable[iBone * cVertices + iVertex]	= true;

								rgBones[iBone].m_cWeights++;
							}
						}


						if (rgcNonZeros[iVertex] > cMaxBonesPerVertex)
							cMaxBonesPerVertex = rgcNonZeros[iVertex];

						iVertex++;
					}



					// calculate max number of bones per vertex

					cMaxBonesPerFace	= 0;

					for (int iFace = 0; iFace < cFaces; iFace++) 
					{
						int	cBonesPerFace	= 0;

						for (int iBone = 0; iBone < cBones; iBone++) 
						{
							for (int iIndex = 0; iIndex < rgFaces[iFace].m_cIndices; iIndex++) 
							{
								if (rgbNonZeroFlagTable[iBone * cVertices + rgReps[rgFaces[iFace].m_rgIndices[iIndex]].m_iFirst]) 
								{
									cBonesPerFace++;

									break;
								}
							}
						}

						if (cBonesPerFace > cMaxBonesPerFace)
							cMaxBonesPerFace = cBonesPerFace;
					}





					objInput = rgInputs[iInput];

					bFoundSmoothSkin = true;

					break;
				}
			}

			if (bFoundSmoothSkin)
				break;
		}
	}


	delete[] rgcNonZeros;
	delete[] rgbNonZeroFlagTable;

















	// rigid skinning

	rgbNonZeroFlagTable	= NULL;

	bool	bFoundRigidSkin	= false;

	if (!bFoundSmoothSkin && objShape.hasFn(MFn::kMesh)) 	// shape is a mesh 
	{
		cBones			= 1;						// zero'th bone is the extra "fake" bone
		int cBonesMax	= 64;
		rgBones			= new Bone[cBonesMax];

		ASSERT(rgBones,
					"Could not allocate memory for bone array");

		rgbNonZeroFlagTable	= new bool[cBonesMax * cVertices];

		ASSERT(rgbNonZeroFlagTable,
					"Could not allocate memory for non-zero flag table");

		// fill non zero table with 0's
		memset(rgbNonZeroFlagTable, 0, cBonesMax * cVertices * sizeof(bool));

		// initialize "fake" iBone
		// WARNING: not checking for new failure
		rgBones[0].m_szName			= new char[256];
		rgBones[0].m_cReps			= 0;
		rgBones[0].m_cWeights		= 0;
		rgBones[0].m_rgfWeights		= new float[cVertices];
		rgBones[0].m_rgiVertices	= new int[cVertices];

		g_Strings.add(rgBones[0].m_szName);							// housekeeping

		strcpy(rgBones[0].m_szName, SCENE_ROOT);					// bone name
	
		matMeshWorldTransform.get(rgBones[0].m_matOffset);			// "fake" bone has identity matrix


		// loop through joint clusters
		for (MItDependencyNodes itCluster(MFn::kJointCluster); !itCluster.isDone(); itCluster.next()) 
		{
			MFnWeightGeometryFilter fnCluster(itCluster.item());

			// load input and output geometries
			MObjectArray	rgInputs;
			MObjectArray	rgOutputs;

			fnCluster.getInputGeometry(rgInputs);
			fnCluster.getOutputGeometry(rgOutputs);

			assert(rgInputs.length() == rgOutputs.length());	// ensure input geometry count equals 
																// output geometry count

			int	cInputs, cOutputs;

			cInputs	= cOutputs	
				= (int)rgOutputs.length();

			// loop through the output geometries
			for (int iOutput = 0, iInput = 0; iOutput < cOutputs; iOutput++, iInput++) 
			{
				assert(iOutput == iInput);
				
				if (rgOutputs[iOutput] == objShape) 	// our shape is one of the output geometries
				{
					bFoundRigidSkin	= true;
		
					assert(rgInputs[iInput] == fnCluster.inputShapeAtIndex(iInput));	// sanity check

					objInput	= rgInputs[iInput];

					// get bone
					MPlug		plgMatrix	= fnCluster.findPlug("matrix", &mStat);

					MPlugArray	rgplgMatrixConnections;

					plgMatrix.connectedTo(rgplgMatrixConnections, true, false);			// get source plugs
					assert(rgplgMatrixConnections.length() == 1);

					MObject	objBone	= rgplgMatrixConnections[0].node();

					assert(objBone.hasFn(MFn::kJoint));

					MFnIkJoint fnBone(objBone);

					char	szBone[64];

					strcpy(szBone, fnBone.name().asChar());

					// find bone's index in current bone list
					for (int iBone = 1; iBone < cBones;	iBone++) 
					{
						if (!strcmp(rgBones[iBone].m_szName, szBone))
							break;
					}
	
					if (iBone == cBones) 	// bone was not found in current bone list
					{
						// add bone
						if (cBones >= cBonesMax) 
						{
							// double array size
							cBonesMax  += cBonesMax;

							Bone*	rgNewBones	= new Bone[cBonesMax];

							ASSERT(rgNewBones, 
										"Could not allocate memory for new bone array");

							memcpy(rgNewBones, rgBones, cBones * sizeof(Bone));

							delete[] rgBones;

							rgBones	= rgNewBones;


							bool*	rgbNewNonZeroFlagTable	= new bool[cBonesMax * cVertices];

							ASSERT(rgbNewNonZeroFlagTable, 
										"Could not allocate memory for new non-zero flag table");

							memset(rgbNewNonZeroFlagTable, 0, cBonesMax * cVertices * sizeof(bool));
							memcpy(rgbNewNonZeroFlagTable, rgbNonZeroFlagTable, cBones * cVertices * sizeof(bool));

							delete[] rgbNonZeroFlagTable;

							rgbNonZeroFlagTable	= rgbNewNonZeroFlagTable;
						}
		
						// initialize iBone
						// WARNING: not checking for new failure
						rgBones[iBone].m_szName			= new char[256];
						rgBones[iBone].m_cReps			= 0;
						rgBones[iBone].m_cWeights		= 0;
						rgBones[iBone].m_rgiVertices	= new int[cVertices];
						rgBones[iBone].m_rgfWeights		= new float[cVertices];

						g_Strings.add(rgBones[iBone].m_szName);							// housekeeping

						strcpy(rgBones[iBone].m_szName, szBone);						// bone name
	
						// matrix info
						MObject objBindPose;

						fnBone.findPlug("bindPose").getValue(objBindPose);

						MFnMatrixData fnBindPose(objBindPose);

						(matMeshWorldTransform * fnBindPose.matrix().inverse()).get(rgBones[iBone].m_matOffset);


						rgobjBones.append(objBone);

						cBones++;
					}



					char	szParent[64];

					bool	bFoundParent	= false;
					MObject	objParent;

					for (int iParent = 0; iParent < (int)fnBone.parentCount(); iParent++) 
					{
						objParent	= fnBone.parent(iParent);

						MFnDagNode	fnParent(objParent);
						
						strcpy(szParent, fnParent.name().asChar());		// parent's name

						for (int iShape_ = 0; iShape_ < DtShapeGetCount(); iShape_++) 
						{
							char*	szShape;

							DT_ATTEMPT(DtShapeGetName(iShape_, &szShape));

							if (!strcmp(szParent, szShape)) 
							{
								bFoundParent	= true;

								break;
							}
						}

						if (bFoundParent)
							break;
					}

					iParent	= 0;

					if (bFoundParent) 	// parent shape found
					{
						// find parent bone's index in current bone list
						for (iParent = 1; iParent < cBones;	iParent++) 
						{
							if (!strcmp(rgBones[iParent].m_szName, szParent)) 
							{
								break;
							}
						}


						if (iParent == cBones) 		// parent bone was not found in current bone list
						{
							// add parent bone
							if (cBones >= cBonesMax) 
							{
								// double array size
								cBonesMax  += cBonesMax;

								Bone*	rgNewBones	= new Bone[cBonesMax];

								ASSERT(rgNewBones, 
											"Could not allocate memory for new bone array");
	
								memcpy(rgNewBones, rgBones, cBones * sizeof(Bone));
	
								delete[] rgBones;

								rgBones	= rgNewBones;

								
								bool*	rgbNewNonZeroFlagTable	= new bool[cBonesMax * cVertices];

								ASSERT(rgbNewNonZeroFlagTable, 
											"Could not allocate memory for new non-zero flag table");

								memset(rgbNewNonZeroFlagTable, 0, cBonesMax * cVertices * sizeof(bool));
								memcpy(rgbNewNonZeroFlagTable, rgbNonZeroFlagTable, cBones * cVertices * sizeof(bool));

								delete[] rgbNonZeroFlagTable;

								rgbNonZeroFlagTable	= rgbNewNonZeroFlagTable;
							}
			
							// initialize iBone
							// WARNING: not checking for new failure
							rgBones[iParent].m_szName		= new char[256];
							rgBones[iParent].m_cReps		= 0;
							rgBones[iParent].m_cWeights		= 0;
							rgBones[iParent].m_rgiVertices	= new int[cVertices];
							rgBones[iParent].m_rgfWeights	= new float[cVertices];
	
							g_Strings.add(rgBones[iParent].m_szName);						// housekeeping
	
							strcpy(rgBones[iParent].m_szName, szParent);		// bone name

							// matrix info
							MObject	objBindPose;
	
							assert(objParent.hasFn(MFn::kJoint));

							MFnIkJoint(objParent).findPlug("bindPose").getValue(objBindPose);
	
							MFnMatrixData fnBindPose(objBindPose);
	
							(matMeshWorldTransform * fnBindPose.matrix().inverse()).get(rgBones[iParent].m_matOffset);


							rgobjBones.append(objParent);

							cBones++;
						}
					}


					// load weights
					MPlug		plgMessage	= fnCluster.findPlug("message");

					MPlugArray	rgplgMessageConnections;

					plgMessage.connectedTo(rgplgMessageConnections, false, true);	// get destination plugs

					assert(rgplgMessageConnections.length() == 1);
					assert(rgplgMessageConnections[0].node().hasFn(MFn::kSet));

					MFnSet fnSet(rgplgMessageConnections[0].node());
				
					MSelectionList list;

					fnSet.getMembers(list, false);

					assert(list.length() == 1);

					MDagPath	path;
					MObject		objComponents;

					list.getDagPath(0, path, objComponents);

					MFloatArray	rgWeights;

					fnCluster.getWeights(path, objComponents, rgWeights);

					MFnSingleIndexedComponent fnComponent(objComponents);

					assert (fnComponent.elementCount() == (int)rgWeights.length());

					// loop through the weights
					for (int iWeight = 0; iWeight < (int)rgWeights.length(); iWeight++) 
					{
						assert(rgWeights[iWeight] <= 1.0f);

						int	iVertex	= fnComponent.element(iWeight);
						
						rgBones[iBone].m_rgfWeights[rgBones[iBone].m_cWeights]	= rgWeights[iWeight];
						rgBones[iBone].m_rgiVertices[rgBones[iBone].m_cWeights]	= iVertex;

						rgBones[iBone].m_cReps	 += rgReps[iVertex].m_cReps;
						rgBones[iBone].m_cWeights++;

						rgbNonZeroFlagTable[iBone * cVertices + iVertex]	= true;

						if (rgWeights[iWeight] != 1.0f) 
						{
							rgBones[iParent].m_rgfWeights[rgBones[iParent].m_cWeights]	= 1.0f - rgWeights[iWeight];
							rgBones[iParent].m_rgiVertices[rgBones[iParent].m_cWeights]	= iVertex;

							rgBones[iParent].m_cReps   += rgReps[iVertex].m_cReps;
							rgBones[iParent].m_cWeights++;				// IMPORTANT: Don't change line position

							rgbNonZeroFlagTable[iParent * cVertices + iVertex]	= true;
						}
					}

					break;
				}	// if found our mesh
			}	// loop thru geom's
		}	// loop thru joint clusters


		if (cBones == 1) 		// no rigid skinning found
		{
			delete[] rgBones[0].m_rgfWeights;
			delete[] rgBones[0].m_rgiVertices;

			cBones	= 0;
		}
		else 
		{
			// at most 2 bones per vertex in rigid skinning (i.e. bone + parent)
			cMaxBonesPerVertex	= 2;

			// calculate max number of bones per vertex
			cMaxBonesPerFace	= 0;

			for (int iFace = 0; iFace < cFaces; iFace++) 
			{
				int	cBonesPerFace	= 0;

				for (int iBone = 0; iBone < cBones; iBone++) 
				{
					for (int iIndex = 0; iIndex < rgFaces[iFace].m_cIndices; iIndex++) 
					{
						if (rgbNonZeroFlagTable[iBone * cVertices + rgReps[rgFaces[iFace].m_rgIndices[iIndex]].m_iFirst]) 
						{
							cBonesPerFace++;

							break;
						}
					}
				}

				if (cBonesPerFace > cMaxBonesPerFace)
					cMaxBonesPerFace = cBonesPerFace;
			}



			
			
			
			
		}
	}
				
				
	delete[] rgbNonZeroFlagTable;



	// load lumps of control vertex data

	if (cBones == 0)		// no skinning found
	{
		DT_ATTEMPT(MyDtShapeGetVertices(iShape, &cVertices, &rgVertices));				// lump of vertices
		DT_ATTEMPT(MyDtShapeGetNormals(iShape, &cNormals, &rgNormals));					// lump of normals
	}
	else					// skinning found
	{
		DT_ATTEMPT(MyDtShapeGetVertices(objInput, objShape, &cVertices, &rgVertices));	// lump of vertices
		DT_ATTEMPT(MyDtShapeGetNormals(objInput, objShape, &cNormals, &rgNormals));		// lump of normals
	}

	DT_ATTEMPT(MyDtShapeGetTextureVertices(iShape, &cTexCoords, &rgTexCoords));			// lump of texture coords
	DT_ATTEMPT(MyDtShapeGetVerticesColor(iShape, &cVertexColors, &rgVertexColors));		// lump of vertex colors



	// mesh type

	pShape->m_kType				= Mesh::POLY_MESH;


	EXIT;


	return	hr;
}
















HRESULT	LoadShape
		(
			int		iShape, 
			Mesh*	pShape
		)
{
	HRESULT	hr	= S_OK;

	INIT;

	if (MyDtShapeIsJoint(iShape)) 
	{
		pShape->m_kType	= Mesh::BONE;
	}
	else if (g_bExportPatches && MyDtShapeIsPatchMesh(iShape)) 
	{
		HR_ATTEMPT(LoadPatchMesh(iShape, pShape),
			"Could not load patch-mesh");
	}
	else 
	{
		HR_ATTEMPT(LoadPolyMesh(iShape, pShape),
			"Could not load poly-mesh");
	}

	EXIT;

	return	hr;
}
























HRESULT	AddSkin
		(
			Mesh*					pShape, 
			LPDIRECTXFILEDATA		pShapeDataObject, 
			LPDIRECTXFILESAVEOBJECT	pxofSave
		) 
{
	HRESULT	hr	= S_OK;


	LPDIRECTXFILEDATA	pSkinDataObject	= NULL;


	PBYTE	pbSkinData	= NULL;



	INIT;



	int		cbSkinSize	= sizeof(WORD)		// nMaxSkinWeightsPerVertex
						+ sizeof(WORD)		// nMaxSkinWeightsPerFace
						+ sizeof(WORD);		// nBones

	PBYTE	pbSkinCurr	= pbSkinData	= new BYTE[cbSkinSize];

	ASSERT(pbSkinData,
				"Could not allocate memory for pbSkinData");


	// nMaxSkinWeightsPerVertex
	WRITE_WORD(pbSkinCurr, ((WORD)pShape->m_cMaxBonesPerVertex));

	// nMaxSkinWeightsPerFace
	WRITE_WORD(pbSkinCurr, ((WORD)pShape->m_cMaxBonesPerFace));

	// nBones
	WRITE_WORD(pbSkinCurr, ((WORD)pShape->m_cBones));


	HR_ATTEMPT(pxofSave->CreateDataObject(DXFILEOBJ_XSkinMeshHeader, NULL, NULL, cbSkinSize, pbSkinData, &pSkinDataObject),
				"Could not create pSkinDataObject");


	HR_ATTEMPT(pShapeDataObject->AddDataObject(pSkinDataObject),
				"Could not add pSkinDataObject to pShapeDataObject");



	EXIT;



	delete[] pbSkinData;

	if (pSkinDataObject)
		pSkinDataObject->Release();


	if (FAILED(hr))
		return hr;


	// SkinWeights
	for (int iBone = 0; iBone < pShape->m_cBones; iBone++) 
	{
		LPDIRECTXFILEDATA	pBoneDataObject	= NULL;

		PBYTE	pbBoneData	= NULL;


		INIT;

		int		cbBoneSize	= sizeof(char*)									// transformNodeName
							+ sizeof(DWORD)									// nWeights
							+ sizeof(DWORD) * pShape->m_rgBones[iBone].m_cReps	// vertexIndices[nWeights]
							+ sizeof(float) * pShape->m_rgBones[iBone].m_cReps	// weights[nWeights]
							+ sizeof(float) * 16;							// matrixOffset

		PBYTE	pbBoneCurr	= pbBoneData	= new BYTE[cbBoneSize];

		ASSERT(pbBoneData,
					"Could not allocate memory for pbBoneData.");


		// transformNodeName
		WRITE_PCHAR(pbBoneCurr, ((char*)pShape->m_rgBones[iBone].m_szName));

		// nWeights
		WRITE_DWORD(pbBoneCurr, ((DWORD)pShape->m_rgBones[iBone].m_cReps));

		// vertexIndices[nWeights]
		for (int iVertex = 0; iVertex < pShape->m_rgBones[iBone].m_cWeights; iVertex++) 
		{
			int iRep = pShape->m_rgBones[iBone].m_rgiVertices[iVertex];

			do 
			{
				WRITE_DWORD(pbBoneCurr, ((DWORD)iRep));

				iRep = pShape->m_rgReps[iRep].m_iNext;
			} while (iRep != pShape->m_rgReps[iRep].m_iFirst);
		}

		// weights[nWeights]
		for (iVertex = 0; iVertex < pShape->m_rgBones[iBone].m_cWeights; iVertex++)
		{
			for (int iRep = 0; iRep < pShape->m_rgReps[pShape->m_rgBones[iBone].m_rgiVertices[iVertex]].m_cReps; iRep++)
			{
				WRITE_FLOAT(pbBoneCurr, pShape->m_rgBones[iBone].m_rgfWeights[iVertex]);
			}
		}

		// matrixOffset
		for (int iRow = 0; iRow < 4; iRow++)
		{
			for (int iCol = 0; iCol < 4; iCol++)
			{
				WRITE_FLOAT(pbBoneCurr, pShape->m_rgBones[iBone].m_matOffset[iRow][iCol]);
			}
		}


		HR_ATTEMPT(pxofSave->CreateDataObject(DXFILEOBJ_SkinWeights, NULL, NULL, cbBoneSize, pbBoneData, &pBoneDataObject),
					"Could not create pBoneDataObject");


		HR_ATTEMPT(pShapeDataObject->AddDataObject(pBoneDataObject),
					"Could not add pBoneDataObject to pShapeDataObject");



		EXIT;



		delete[] pbBoneData;

		if (pBoneDataObject)
			pBoneDataObject->Release();


		if (FAILED(hr))
			return hr;
	}


	return hr;
}










HRESULT	AddNormals
		(
			Mesh*					pShape, 
			LPDIRECTXFILEDATA		pShapeDataObject, 
			LPDIRECTXFILESAVEOBJECT pxofSave
		) 
{
	HRESULT	hr	= S_OK;

	LPDIRECTXFILEDATA	pNormalsDataObject	= NULL;

	PBYTE	pbNormalsData	= NULL;

	INIT;
	
	int		cbNormalsSize	= sizeof(DWORD)													// nNormals
							+ pShape->m_cReps * (3 * sizeof(float))							// normals
							+ sizeof(DWORD)													// nFaceNormals
							+ (pShape->m_cFaces + pShape->m_cFaceIndices) * sizeof(DWORD);	// faceNormals

	PBYTE	pbNormalsCurr	= pbNormalsData	
							= new BYTE[cbNormalsSize];

	ASSERT(pbNormalsCurr, "Could not allocate memory for pbNormalsData");


	// nNormals
	WRITE_DWORD(pbNormalsCurr, ((DWORD)pShape->m_cReps));

	// normals
	int iRep;

	for (iRep = 0; iRep < pShape->m_cReps; iRep++) 
	{
		if (pShape->m_rgReps[iRep].m_iNormalIdx == -1)		// no normal index found
        {
			WRITE_FLOAT(pbNormalsCurr, 1.0f);
			WRITE_FLOAT(pbNormalsCurr, 0.0f);
			WRITE_FLOAT(pbNormalsCurr, 0.0f);
		}
		else 
        {
			WRITE_FLOAT(pbNormalsCurr, pShape->m_rgNormals[pShape->m_rgReps[iRep].m_iNormalIdx].vec[0]);
			WRITE_FLOAT(pbNormalsCurr, pShape->m_rgNormals[pShape->m_rgReps[iRep].m_iNormalIdx].vec[1]);
			WRITE_FLOAT(pbNormalsCurr, pShape->m_rgNormals[pShape->m_rgReps[iRep].m_iNormalIdx].vec[2]);
		}
	}

	// nFaceNormals
	WRITE_DWORD(pbNormalsCurr, ((DWORD)pShape->m_cFaces));

	// faceNormals
	int iFace;

	for (iFace = 0; iFace < pShape->m_cFaces; iFace++) 
	{
		WRITE_DWORD(pbNormalsCurr, ((DWORD)pShape->m_rgFaces[iFace].m_cIndices));

		for (int iIndex = 0; iIndex < pShape->m_rgFaces[iFace].m_cIndices; iIndex++)
			WRITE_DWORD(pbNormalsCurr, ((DWORD)pShape->m_rgFaces[iFace].m_rgIndices[iIndex]));
	}


	HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMMeshNormals, NULL, NULL, cbNormalsSize, pbNormalsData, &pNormalsDataObject),
				"Could not create normals data object")


	HR_ATTEMPT(pShapeDataObject->AddDataObject(pNormalsDataObject),
				"Could not add pNormalsDataObject to pShapeDataObject")



	EXIT;

	// clean up
	delete[] pbNormalsData;

	if (pNormalsDataObject)
		pNormalsDataObject->Release();

	return hr;
}










HRESULT	AddTexCoords
		(
			Mesh*					pShape, 
			LPDIRECTXFILEDATA		pShapeDataObject, 
			LPDIRECTXFILESAVEOBJECT	pxofSave
		) 
{
	HRESULT	hr	= S_OK;

	LPDIRECTXFILEDATA	pTexCoordsDataObject	= NULL;

	PBYTE	pbTexCoordsData	= NULL;


	INIT;


	int		cbTexCoordsSize	= sizeof(DWORD)							// nTextureCoords
							+ pShape->m_cReps * (2 * sizeof(float));	// textureCoords

	PBYTE	pbTexCoordsCurr	= pbTexCoordsData	= new BYTE[cbTexCoordsSize];

	ASSERT(pbTexCoordsData,
				"Could not allocate memory for pbTexCoordsData");


	// nTextureCoords
	WRITE_DWORD(pbTexCoordsCurr, ((DWORD)pShape->m_cReps));

	// textureCoords
	for (int iRep = 0; iRep < pShape->m_cReps; iRep++) 
	{
		if (pShape->m_rgReps[iRep].m_iTexCoordIdx == -1)
		{
			WRITE_FLOAT(pbTexCoordsCurr, 0.0f);
			WRITE_FLOAT(pbTexCoordsCurr, 0.0f);
		}
		else 
		{
			WRITE_FLOAT(pbTexCoordsCurr, g_iFlipU * pShape->m_rgTexCoords[pShape->m_rgReps[iRep].m_iTexCoordIdx].vec[0]);
			WRITE_FLOAT(pbTexCoordsCurr, g_iFlipV * pShape->m_rgTexCoords[pShape->m_rgReps[iRep].m_iTexCoordIdx].vec[1]);
		}
	}

	HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMMeshTextureCoords, NULL, NULL, cbTexCoordsSize, pbTexCoordsData, &pTexCoordsDataObject),
				"Could not create pTexCoordsDataObject");


	HR_ATTEMPT(pShapeDataObject->AddDataObject(pTexCoordsDataObject),
				"Could not add data object");


	EXIT;


	delete[] pbTexCoordsData;

	if (pTexCoordsDataObject)
		pTexCoordsDataObject->Release();

	return hr;
}







HRESULT	AddMaterialList
		(
			Mesh*					pShape, 
			LPDIRECTXFILEDATA		pShapeDataObject, 
			LPDIRECTXFILESAVEOBJECT	pxofSave
		) 
{
	HRESULT				hr						= S_OK;

	LPDIRECTXFILEDATA	pMaterialsDataObject	= NULL;

	PBYTE				pbMaterialsData			= NULL;
	
	INIT;

	int		cbMaterialsSize	= sizeof(DWORD)						// nMaterials
							+ sizeof(DWORD)						// nFaceIndexes
							+ pShape->m_cFaces * sizeof(DWORD);	// FaceIndexes

	PBYTE	pbMaterialsCurr	= pbMaterialsData	= new BYTE[cbMaterialsSize];

	ASSERT(pbMaterialsCurr, 
				"Could not allocate memory for pbMaterialsData");


	// nMaterials
	WRITE_DWORD(pbMaterialsCurr, ((DWORD)pShape->m_cGroups));

	// nFaceIndexes
	WRITE_DWORD(pbMaterialsCurr, ((DWORD)pShape->m_cFaces));

	// FaceIndexes
	for (int iFace = 0; iFace < pShape->m_cFaces; iFace++)
		WRITE_DWORD(pbMaterialsCurr, pShape->m_rgFaces[iFace].m_iGroup);


	HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMMeshMaterialList, NULL, NULL, cbMaterialsSize, pbMaterialsData, &pMaterialsDataObject),
				"Could not create pMaterialsDataObject");

	// material data
	for (int iGroup = 0; iGroup < pShape->m_cGroups; iGroup++) 
	{
		LPDIRECTXFILEDATA	pMaterialDataObject	= NULL;
		LPDIRECTXFILEDATA	pTextureDataObject	= NULL;

		PBYTE				pbMaterialData		= NULL;

		INIT;

		int		cbMaterialSize	= 4 * sizeof(float)		// faceColor
								+ sizeof(float)			// power
								+ 3 * sizeof(float)		// specularColor
								+ 3 * sizeof(float);	// emissiveColor

		PBYTE	pbMaterialCurr	= pbMaterialData	= new BYTE[cbMaterialSize];

		ASSERT(pbMaterialCurr,
					"Could not allocate memory for pbMaterialData");


		// faceColor
		WRITE_FLOAT(pbMaterialCurr, pShape->m_rgGroups[iGroup].m_fDiffuseRed);
		WRITE_FLOAT(pbMaterialCurr, pShape->m_rgGroups[iGroup].m_fDiffuseGreen);
		WRITE_FLOAT(pbMaterialCurr, pShape->m_rgGroups[iGroup].m_fDiffuseBlue);
		WRITE_FLOAT(pbMaterialCurr, (1.0f - pShape->m_rgGroups[iGroup].m_fTransparency));

		// power
		WRITE_FLOAT(pbMaterialCurr, pShape->m_rgGroups[iGroup].m_fShininess);
		
		// specularColor
		WRITE_FLOAT(pbMaterialCurr, pShape->m_rgGroups[iGroup].m_fSpecularRed);
		WRITE_FLOAT(pbMaterialCurr, pShape->m_rgGroups[iGroup].m_fSpecularGreen);
		WRITE_FLOAT(pbMaterialCurr, pShape->m_rgGroups[iGroup].m_fSpecularBlue);

		// emissiveColor
		WRITE_FLOAT(pbMaterialCurr, pShape->m_rgGroups[iGroup].m_fEmissiveRed);
		WRITE_FLOAT(pbMaterialCurr, pShape->m_rgGroups[iGroup].m_fEmissiveGreen);
		WRITE_FLOAT(pbMaterialCurr, pShape->m_rgGroups[iGroup].m_fEmissiveBlue);


		HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMMaterial, NULL, NULL, cbMaterialSize, pbMaterialData, &pMaterialDataObject),
					"Could not create pMaterialDataObject");


		// TextureFilename		
		if (pShape->m_rgGroups[iGroup].m_szTextureFile) 
		{
			int		cbTextureSize	= sizeof(char**);

			HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMTextureFilename, NULL, NULL, cbTextureSize, &pShape->m_rgGroups[iGroup].m_szTextureFile, &pTextureDataObject),
						"Could not create pMaterialDataObject");

			HR_ATTEMPT(pMaterialDataObject->AddDataObject(pTextureDataObject),
						"Could not add pTextureDataObject to pMaterialDataObject");
		}


	    HR_ATTEMPT(pMaterialsDataObject->AddDataObject(pMaterialDataObject),
					"Could not add pMaterialDataObject to pMaterialsDataObject");


		EXIT;


		delete[] pbMaterialData;

		if (pMaterialDataObject)
			pMaterialDataObject->Release();

		if (pTextureDataObject)
			pTextureDataObject->Release();


		ASSERT(SUCCEEDED(hr),	
					"Error occured while adding materials");
	}


	HR_ATTEMPT(pShapeDataObject->AddDataObject(pMaterialsDataObject),
				"Could not add pMaterialsDataObject to pShapeDataObject");


	EXIT;


	delete[] pbMaterialsData;

	if (pMaterialsDataObject)
		pMaterialsDataObject->Release();	
	
		
	return hr;
}








HRESULT	AddVertexColors
		(
			Mesh*					pShape, 
			LPDIRECTXFILEDATA		pShapeDataObject, 
			LPDIRECTXFILESAVEOBJECT	pxofSave
		) 
{
	HRESULT	hr	= S_OK;

	LPDIRECTXFILEDATA	pColorsDataObject	= NULL;

	PBYTE	pbColorsData	= NULL;
	

	INIT;

	int		cbColorsSize	= sizeof(DWORD)												// nVertexColors
							+ pShape->m_cReps * (sizeof(DWORD) + 4 * sizeof(float));		// vertexColors

	PBYTE	pbColorsCurr	= pbColorsData	= new BYTE[cbColorsSize];

	ASSERT(pbColorsData,
				"Could not allocate memory for pbColorsData");


	// nVertexColors
	WRITE_DWORD(pbColorsCurr, ((DWORD)pShape->m_cReps));

	// vertexColors
	for (int iRep = 0; iRep < pShape->m_cReps; iRep++) 
	{
		// index
		WRITE_DWORD(pbColorsCurr, ((DWORD)iRep));

		// indexedColor
		WRITE_FLOAT(pbColorsCurr, pShape->m_rgVertexColors[pShape->m_rgReps[iRep].m_iFirst].r);	// red
		WRITE_FLOAT(pbColorsCurr, pShape->m_rgVertexColors[pShape->m_rgReps[iRep].m_iFirst].g);	// green
		WRITE_FLOAT(pbColorsCurr, pShape->m_rgVertexColors[pShape->m_rgReps[iRep].m_iFirst].b);	// blue
		WRITE_FLOAT(pbColorsCurr, pShape->m_rgVertexColors[pShape->m_rgReps[iRep].m_iFirst].a);	// alpha
	}


	HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMMeshVertexColors, NULL, NULL, cbColorsSize, pbColorsData, &pColorsDataObject),
				"Could not create pColorsDataObject");


	HR_ATTEMPT(pShapeDataObject->AddDataObject(pColorsDataObject),
				"Could not add pColorsDataObject to pShapeDataObject");


	EXIT;


	delete[] pbColorsData;

	if (pColorsDataObject)
		pColorsDataObject->Release();


	return hr;
}











HRESULT	AddRepInfo
		(
			Mesh*					pShape, 
			LPDIRECTXFILEDATA		pShapeDataObject, 
			LPDIRECTXFILESAVEOBJECT	pxofSave
		) 
{
	HRESULT	hr	= S_OK;


	LPDIRECTXFILEDATA	pRepsDataObject	= NULL;


	PBYTE	pbRepsData	= NULL;
	
	INIT;

	int		cbRepsSize	= sizeof(DWORD)						// nIndices
						+ sizeof(DWORD)						// nOriginalVertices
						+ pShape->m_cReps * sizeof(DWORD);	// indices

	PBYTE	pbRepsCurr	= pbRepsData	= new BYTE[cbRepsSize];

	ASSERT(pbRepsData,
				"Could not allocate memory for pbRepsData");


	// nIndices
	WRITE_DWORD(pbRepsCurr, ((DWORD)pShape->m_cReps));

	// nOriginalVertices
	WRITE_DWORD(pbRepsCurr, ((DWORD)pShape->m_cVertices));

	// indices
	for (int iRep = 0; iRep < pShape->m_cReps; iRep++)
		WRITE_DWORD(pbRepsCurr, ((DWORD)pShape->m_rgReps[iRep].m_iFirst));


	HR_ATTEMPT(pxofSave->CreateDataObject(DXFILEOBJ_VertexDuplicationIndices, NULL, NULL, cbRepsSize, pbRepsData, &pRepsDataObject),
				"Could not create pRepsDataObject");


	HR_ATTEMPT(pShapeDataObject->AddDataObject(pRepsDataObject),
				"Could not add pRepsDataObject to pShapeDataObject");


	EXIT;


	delete[] pbRepsData;

	if (pRepsDataObject)
		pRepsDataObject->Release();


	return hr;
}













HRESULT	AddPatchMesh
		(
			Mesh*					pShape, 
			LPDIRECTXFILEDATA		pFrameDataObject, 
			LPDIRECTXFILESAVEOBJECT	pxofSave
		) 
{

	HRESULT	hr	= S_OK;

	LPDIRECTXFILEDATA	pShapeDataObject	= NULL;

	PBYTE	pbShapeData	= NULL;


	INIT;


	int		cbShapeSize	= sizeof(DWORD)													// nVertices
						+ pShape->m_cReps * (3 * sizeof(float))							// vertices
						+ sizeof(DWORD)													// nPatches
						+ (pShape->m_cFaces + pShape->m_cFaceIndices) * sizeof(DWORD);	// patches

	PBYTE	pbShapeCurr	= pbShapeData	= new BYTE[cbShapeSize];

	ASSERT(pbShapeData,
				"Could not allocate memory for pbpShapeData");
	

	// nVertices
	WRITE_DWORD(pbShapeCurr, ((DWORD)pShape->m_cVertices));

	// vertices
	for (int iRep = 0; iRep < pShape->m_cReps; iRep++) 
	{
		WRITE_FLOAT(pbShapeCurr, pShape->m_rgVertices[pShape->m_rgReps[iRep].m_iFirst].vec[0]);
		WRITE_FLOAT(pbShapeCurr, pShape->m_rgVertices[pShape->m_rgReps[iRep].m_iFirst].vec[1]);
		WRITE_FLOAT(pbShapeCurr, pShape->m_rgVertices[pShape->m_rgReps[iRep].m_iFirst].vec[2]);
	}

	// nPatches
	WRITE_DWORD(pbShapeCurr, ((DWORD)pShape->m_cFaces));

	// faces
	for (int iFace = 0; iFace < pShape->m_cFaces; iFace++) 
	{
		WRITE_DWORD(pbShapeCurr, ((DWORD)16));

		for (int iIndex = 0; iIndex < 16; iIndex++)
			WRITE_DWORD(pbShapeCurr, pShape->m_rgFaces[iFace].m_rgIndices[iIndex]);
	}


	HR_ATTEMPT(pxofSave->CreateDataObject(DXFILEOBJ_PatchMesh, NULL, NULL, cbShapeSize, pbShapeData, &pShapeDataObject),
				"Could not create pShapeDataObject");



	// MeshTextureCoords
	if (pShape->m_cTexCoords > 0) 
	{
		HR_ATTEMPT(AddTexCoords(pShape, pShapeDataObject, pxofSave),
					"Could not add texture coordinate info");
	}

	// MeshVertexColors
//	HR_ATTEMPT(AddVertexColors(pShape, pShapeDataObject, pxofSave),
//				"Could not add vertex color info");
	
	// MeshMaterialList
	HR_ATTEMPT(AddMaterialList(pShape, pShapeDataObject, pxofSave),
				"Could not add materials info");

	// VertexDuplicationIndices
	HR_ATTEMPT(AddRepInfo(pShape, pShapeDataObject, pxofSave),
				"Could not add rep info");

	// XSkinMeshHeader
	if (pShape->m_cBones > 0) 
	{
		HR_ATTEMPT(AddSkin(pShape, pShapeDataObject, pxofSave),
					"Could not add skin info");
	}



	HR_ATTEMPT(pFrameDataObject->AddDataObject(pShapeDataObject),
				"Could not add pShapeDataObject to pFrameDataObject");


	EXIT;


	delete[] pbShapeData;

	if (pShapeDataObject)
		pShapeDataObject->Release();


	return hr;
}
















HRESULT	AddPolyMesh
		(
			Mesh*					pShape, 
			LPDIRECTXFILEDATA		pFrameDataObject, 
			LPDIRECTXFILESAVEOBJECT	pxofSave
		) 
{

	HRESULT				hr				= S_OK;

	LPDIRECTXFILEDATA	pShapeDataObject	= NULL;

	PBYTE				pbMeshData		= NULL;

	INIT;

	int		cbMeshSize	= sizeof(DWORD)													// nVertices
						+ pShape->m_cReps * (3 * sizeof(float))							// vertices
						+ sizeof(DWORD)													// nFaces
						+ (pShape->m_cFaces + pShape->m_cFaceIndices) * sizeof(DWORD);	// faces

	PBYTE	pbMeshCurr	= pbMeshData	= new BYTE[cbMeshSize];

	ASSERT(pbMeshData,
				"Could not allocate memory for pbMeshData");
	

	// nVertices
	WRITE_DWORD(pbMeshCurr, ((DWORD)pShape->m_cReps));

	// vertices
	for (int iRep = 0; iRep < pShape->m_cReps; iRep++) 
	{
		WRITE_FLOAT(pbMeshCurr, pShape->m_rgVertices[pShape->m_rgReps[iRep].m_iFirst].vec[0]);
		WRITE_FLOAT(pbMeshCurr, pShape->m_rgVertices[pShape->m_rgReps[iRep].m_iFirst].vec[1]);
		WRITE_FLOAT(pbMeshCurr, pShape->m_rgVertices[pShape->m_rgReps[iRep].m_iFirst].vec[2]);
	}

	// nFaces
	WRITE_DWORD(pbMeshCurr, ((DWORD)pShape->m_cFaces));

	// faces
	for (int iFace = 0; iFace < pShape->m_cFaces; iFace++) 
	{
		WRITE_DWORD(pbMeshCurr, ((DWORD)pShape->m_rgFaces[iFace].m_cIndices));

		for (int iIndex = 0; iIndex < pShape->m_rgFaces[iFace].m_cIndices; iIndex++)
			WRITE_DWORD(pbMeshCurr, ((DWORD)pShape->m_rgFaces[iFace].m_rgIndices[iIndex]));
	}


	HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMMesh, NULL, NULL, cbMeshSize, pbMeshData, &pShapeDataObject),
				"Could not create pShapeDataObject");


	// MeshNormals
	if (pShape->m_cNormals > 0) 
	{
		HR_ATTEMPT(AddNormals(pShape, pShapeDataObject, pxofSave),
					"Could not add normal info");
	}


	// MeshTextureCoords
	if (pShape->m_cTexCoords > 0) 
	{
		HR_ATTEMPT(AddTexCoords(pShape, pShapeDataObject, pxofSave),
					"Could not add texture coordinate info");
	}

	// MeshVertexColors
//	HR_ATTEMPT(AddVertexColors(pShape, pShapeDataObject, pxofSave),
//				"Could not add vertex color info");

	
	// MeshMaterialList
	HR_ATTEMPT(AddMaterialList(pShape, pShapeDataObject, pxofSave),
				"Could not add materials info");


	// VertexDuplicationIndices
	HR_ATTEMPT(AddRepInfo(pShape, pShapeDataObject, pxofSave),
				"Could not add rep info");


	// XSkinMeshHeader
	if (pShape->m_cBones > 0) 
	{
		HR_ATTEMPT(AddSkin(pShape, pShapeDataObject, pxofSave),
					"Could not add skin info");
	}


	HR_ATTEMPT(pFrameDataObject->AddDataObject(pShapeDataObject),
				"Could not add pShapeDataObject to pFrameDataObject");



	EXIT;



	delete[] pbMeshData;

	if (pShapeDataObject)
		pShapeDataObject->Release();


	return hr;
}









HRESULT	AddShape
		(
			int						iShape, 
			LPDIRECTXFILEDATA		pParentFrameDataObject, 
			LPDIRECTXFILESAVEOBJECT	pxofSave
		) 
{

	HRESULT				hr					= S_OK;

    LPDIRECTXFILEDATA	pFrameDataObject	= NULL;
	LPDIRECTXFILEDATA	pMatrixDataObject	= NULL;


	INIT;


	// shape name
	char*	szName;

	DT_ATTEMPT(DtShapeGetName(iShape, &szName));


	cout << "\treading " << szName << endl;


	// local transform
	float*	rgfLocalTransform;

	DT_ATTEMPT(DtShapeGetMatrix(iShape, &rgfLocalTransform));


	// Frame
	HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMFrame, szName, NULL, 0, NULL, &pFrameDataObject),
				"Could not create pFrameDataObject");

	// FrameTransformMatrix
	HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMFrameTransformMatrix, NULL, NULL, 16 * sizeof(float), rgfLocalTransform, &pMatrixDataObject),
				"Could not create pMatrixDataObject");



	HR_ATTEMPT(pFrameDataObject->AddDataObject(pMatrixDataObject),
				"Could not add pMatrixDataObject to pFrameDataObject");


	// the reason for the braces here is to place Mesh in a local scope so that will be deleted quickly 
	{	
		Mesh	mesh;

		LoadShape(iShape, &mesh);

		switch (mesh.m_kType) 
		{
			case Mesh::PATCH_MESH:
				HR_ATTEMPT(AddPatchMesh(&mesh, pFrameDataObject, pxofSave),
							"Could not add patch mesh");
				break;
			case Mesh::POLY_MESH:
				// DtShapeGetVertexCount returns an error code if cVertices == 0
				HR_ATTEMPT(AddPolyMesh(&mesh, pFrameDataObject, pxofSave),
							"Could not add mesh");
				break;
			case Mesh::BONE:
				break;
			default:
				ASSERT(false, "Unknown shape type");
				break;
		};
	}	

	// add children
	int		cChildren;
	int*	rgiChildren;

	DT_ATTEMPT(MyDtShapeGetChildren(iShape, &cChildren, &rgiChildren));

	for (int iChild = 0; iChild < cChildren; iChild++) 
	{
		HR_ATTEMPT(AddShape(rgiChildren[iChild], pFrameDataObject, pxofSave),
					"Could not add shape");
	}

	delete[] rgiChildren;

	HR_ATTEMPT(pParentFrameDataObject->AddDataObject(pFrameDataObject),
				"Could not add pFrameDataObject to pParentFrameDataObject");

	EXIT;

	
	if (pMatrixDataObject)
		pMatrixDataObject->Release();

	if (pFrameDataObject)
		pFrameDataObject->Release();
	

	return hr;
}






HRESULT	AddScene
		(
			const char*	szFile
		) 
{
	HRESULT	hr	= S_OK;


    LPDIRECTXFILE			pxofApi					= NULL;
    LPDIRECTXFILESAVEOBJECT	pxofSave				= NULL; 

	LPDIRECTXFILEDATA		pAnimSetObject			= NULL;
	LPDIRECTXFILEDATA		pRootFrameObject		= NULL;
	LPDIRECTXFILEDATA		pRootTransformObject	= NULL;

	INIT;


	// Initialize the Dt database 
	DtExt_SceneInit("scene");

	DtExt_setJointHierarchy(true);
	DtExt_setParents(true);
	DtExt_setOutputTransforms(kTRANSFORMALL);
	DtExt_setTesselate(kTESSTRI);
	DtExt_setWalkMode(0);
	DtExt_setInlineTextures(0);			    // jimn; 9/25/00; Don't convert textures
	DtExt_setOriginalTexture(1);			// jimn; 9/25/00; Use original textures.
//    DtExt_setSoftTextures(1);
    DtExt_setOutputCameras(0);

	DtExt_dbInit();


	cout << "Exporting to " << szFile << " ..." << endl;


	// create xofapi object.
	HR_ATTEMPT(DirectXFileCreate(&pxofApi),
				"Could not create xofapi object");


	// register templates for d3drm.
	HR_ATTEMPT(pxofApi->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES),
				"Could not register D3D templates");


	// register extra templates for skinning info and vertex duplication
	HR_ATTEMPT(pxofApi->RegisterTemplates((LPVOID)XSKINEXP_TEMPLATES, strlen(XSKINEXP_TEMPLATES)),
				"Could not register Skinning and/or Vertex Duplication templates");


	// create save object.
	HR_ATTEMPT(pxofApi->CreateSaveObject(szFile, g_FileFormat, &pxofSave),
				"Could not create save object");


	// save templates
	HR_ATTEMPT(pxofSave->SaveTemplates(3, aIds),
				"Could not save templates.");


	// save file data to the file

	// first create the SCENE_ROOT Frame
	HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMFrame, SCENE_ROOT, NULL, 0, NULL, &pRootFrameObject),
				"Could not create pRootFrameObject");


	// next create the SCENE_ROOT FrameTransformMatrix as the Identity 
	float rgfIdentity[16] = {1.0f, 0.0f, 0.0f, 0.0f,
							 0.0f, 1.0f, 0.0f, 0.0f,
							 0.0f, 0.0f, 1.0f, 0.0f,
							 0.0f, 0.0f, 0.0f, 1.0f};


	HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMFrameTransformMatrix, NULL, NULL, 16 * sizeof(float), rgfIdentity, &pRootTransformObject),
				"Could not create pRootTransformObject");


	HR_ATTEMPT(pRootFrameObject->AddDataObject(pRootTransformObject),
				"Could not add pRootTransformObject to pRootFrameObject");


	cout << endl << "Loading scene data..." << endl;

	int cShapes = DtShapeGetCount();

	// add all shapes which have no parents
	for (int iShape = 0; iShape < cShapes; iShape++) 
	{
		if (MyDtShapeGetParentID(iShape) == -1) 
		{
			HR_ATTEMPT(AddShape(iShape, pRootFrameObject, pxofSave),
						"Could not add top level shape");
		}
	}


	cout << "Writing scene data..." << endl;

	HR_ATTEMPT(pxofSave->SaveData(pRootFrameObject),
				"Could not save scene data to file");




	if (g_bExportAnimation) 
	{
		HR_ATTEMPT(pxofSave->CreateDataObject(TID_D3DRMAnimationSet, NULL, NULL, 0, NULL, &pAnimSetObject),
					"Could not create pAnimSetObject");

	
		cout << endl << "Loading animation data..." << endl;

		bool	bAnimDataFound	= false;

		if (g_bKeyframeAnimation) 
		{
			for (iShape = 0; iShape < cShapes; iShape++) 
			{
				Anim	anim;

				loadAnim(iShape, &anim);

				if (anim.m_cKeys > 0) 
				{
					hr = AddAnim(&anim, pAnimSetObject, pxofSave);

					if (FAILED(hr))
						break;

					bAnimDataFound	= true;
				}


				freeAnim(&anim);

				ASSERT(SUCCEEDED(hr), "Could not add animation data");
			}
		}
		else 		// regular animation
		{
			Anim*	rgAnims	= new Anim[cShapes];

			ASSERT(rgAnims,
						"Could not allocate memory for large animation array");

			loadAllAnims(rgAnims);

			for (int iShape = 0; iShape < cShapes; iShape++) 
			{
				if (rgAnims[iShape].m_cKeys > 0) 
				{
					hr = AddAnim(&rgAnims[iShape], pAnimSetObject, pxofSave);

					if (FAILED(hr))
						break; 

					bAnimDataFound	= true;
				}
			}


			freeAllAnims(rgAnims);

			delete[] rgAnims;

			ASSERT(SUCCEEDED(hr), "Could not add animation data");
		}

		if (bAnimDataFound) 
		{
			cout << "Writing animation data..." << endl;

			HR_ATTEMPT(pxofSave->SaveData(pAnimSetObject),
						"Could not save animation data to file");
		}
		else 
		{
			cout << "Writing skipped (no animation data found)..." << endl;
		}
	}
	else 
	{
		cout << endl << "Ignoring animation..." << endl;
	}

	EXIT;


	if (pRootFrameObject)
		pRootFrameObject->Release();

	if (pRootTransformObject)
		pRootTransformObject->Release();

	if (pAnimSetObject)
		pAnimSetObject->Release();


	if (pxofSave) 
		pxofSave->Release();

	if (pxofApi)
		pxofApi->Release();



	cout << endl;

	if (FAILED(hr))
		cout << "There were errors.";
	else 
		cout << "Completed successfully.";

	cout << endl << "...................................................." << endl << endl;


	cout.flush();

	// Clean up the allocated memory and internal storage
	DtExt_CleanUp();

	return hr;
}





void	ParseOptions
		(
			MString	sOptions
		) 
{
	if (sOptions.length() > 0) 
	{
		MStringArray	optionList;

		sOptions.split(';', optionList);

		// break out all the options.
		for (int iOption = 0; iOption < (int)optionList.length(); iOption++) 
		{
			MStringArray    theOption;

			optionList[iOption].split('=', theOption);

			if (theOption.length() > 1) 
			{
				if (theOption[0] == "fileFormat") 
				{
					if (theOption[1] == "binary") 
					{
						g_FileFormat	= DXFILEFORMAT_BINARY;
					}
					else if (theOption[1] == "compressed") 
					{
						g_FileFormat	= DXFILEFORMAT_COMPRESSED;
					}
					else	// "text" 
					{	
						g_FileFormat	= DXFILEFORMAT_TEXT;
					}
				}
				else if (theOption[0] == "exportAnimation") 
				{
					if (theOption[1] == "false") 
					{
						g_bExportAnimation	= false;
					}
					else 	// "true"
					{
						g_bExportAnimation	= true;
					}
				}
				else if (theOption[0] == "keyframeAnimation") 
				{
					if (theOption[1] == "false") 
					{
						g_bKeyframeAnimation	= false;
					}
					else 	// "true"
					{
						g_bKeyframeAnimation	= true;
					}
				}
				else if (theOption[0] == "animateEverything") 
				{
					if (theOption[1] == "false") 
					{
						g_bAnimateEverything	= false;
					}
					else 	// "true"
					{
						g_bAnimateEverything	= true;
					}
				}
				else if (theOption[0] == "frameStep") 
				{
					g_iFrameStep	= theOption[1].asInt();
				}
				else if (theOption[0] == "flipU") 
				{
					if (theOption[1] == "true") 
					{
						g_iFlipU	= -1;
					}
					else 	// "false"
					{
						g_iFlipU	= 1;
					}
				}
				else if (theOption[0] == "flipV") 
				{
					if (theOption[1] == "true") 
					{
						g_iFlipV	= 1;
					}
					else 	// "false"
					{
						g_iFlipV	= -1;
					}
				}
				else if (theOption[0] == "exportPatches") 
				{
					if (theOption[1] == "true") 
					{
						g_bExportPatches	= true;
					}
					else 	// "false"
					{
						g_bExportPatches	= false;
					}
				}
				else if (theOption[0] == "relTexFilename")
				{
					if (theOption[1] == "true") 
					{
						g_bRelativeTexFile	= true;
					}
					else 	// "false"
					{
						g_bRelativeTexFile	= false;
					}
				}
			}
		}
	}
}



MStatus	xfileTranslator::writer
		(	// parameters
			const MFileObject& file, 
			const MString& sOptions, 
			MPxFileTranslator::FileAccessMode mode
		) 
{
	g_Strings.reset();

	ParseOptions(sOptions);

	MString	sFile	= file.fullName();

	int		iExt	= sFile.rindex('.');
	MString	sExt	= sFile.substring(iExt, sFile.length() - 1);

	sFile	= (sExt == ".x" || sExt == ".X") ? sFile : (sFile + ".x");

	AddScene(sFile.asChar());

	g_Strings.clear();

	return MS::kSuccess;
}





MStatus	initializePlugin 
		(	
			MObject	obj
		) 
{
	MStatus         status;
	char			version[256];

	strcpy(version, "0.3");				// plug-in version
	strcat(version, ".");
	strcat(version, DtAPIVersion());
	
	MFnPlugin       plugin(obj, "XFile Translator for Maya", version, "Any");

	// register the translator
	status = plugin.registerFileTranslator("XFile", "xfileTranslator.rgb", xfileTranslator::creator, "xfileTranslatorOpts", "", true);

	if (!status)
		status.perror("registerFileTranslator");

	return status;
}


MStatus	uninitializePlugin
		(	
			MObject obj
		) 
{
	MStatus         status;
	MFnPlugin       plugin(obj);

	status = plugin.deregisterFileTranslator("XFile");

	if (!status)
		status.perror("deregisterFileTranslator");

	return status;
}





/*** TOOLS

	// print out the attributes of a dependency node
	for (unsigned iAttr = 0; iAttr < fnNode.attributeCount(); iAttr++) 
	{
		MFnAttribute fnAttr(fnNode.attribute(iAttr));

		cout << fnNode.name() << "." << fnAttr.name() << ": " << fnNode.attribute(iAttr).apiTypeStr() << endl;
	}





	// print the node containing the corresponging plug to 'plug'
	MPlugArray rgPlugs;

	plug.connectedTo(rgPlugs, true, true);

	for (int i = 0; i < (int)rgPlugs.length(); i++) 
	{
		MFnDependencyNode fnNode(rgPlugs[i].node());

		cout << fnNode.name() << "\t" << rgPlugs[i].name() << endl;
	}
 ***/