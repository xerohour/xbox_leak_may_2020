
#include "MyDt.h"
#include "MyAssert.h"

// Maya API
#include <maya/MObject.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include <maya/MDagPath.h>
#include <maya/MTime.h>
#include <maya/MAnimControl.h>
#include <maya/MFileObject.h>
#include <maya/MPlug.h>


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


bool	g_bRelativeTexFile;
bool	g_bExportAnimation;
bool	g_bKeyframeAnimation;
bool	g_bAnimateEverything;
int		g_iFrameStep;
int		g_iFlipU;
int		g_iFlipV;
bool	g_bExportPatches;

StringTable	g_Strings;



int	MyDtShapeGetParentID
	(
		int	iShape
	) 
{
	int	cShapes		= DtShapeGetCount();


	MObject	objTransform;

	DtExt_ShapeGetTransform(iShape, objTransform);

	MFnDagNode	fnNode(objTransform);

	int cParents	= fnNode.parentCount();

	for (int iParent = 0; iParent < cParents; iParent++) 
	{
		MFnDagNode	fnParent(fnNode.parent(iParent));

		for (int iShape_ = 0; iShape_ < cShapes; iShape_++) 
		{
			MObject	objTransform_;

			DtExt_ShapeGetTransform(iShape_, objTransform_);

			if (fnParent.fullPathName() == MFnDagNode(objTransform_).fullPathName())
				return iShape_;
		}
	}

	return -1;
}



int MyDtShapeGetChildren
    (
		int     iShape, 
        int*    cChildren, 
        int**   rgiChildren
    ) 
{
	int	cShapes		= DtShapeGetCount();

	MObject	objTransform;

	DtExt_ShapeGetTransform(iShape, objTransform);

	MFnDagNode	fnNode(objTransform);

	int cChildren_	= fnNode.childCount();

	*cChildren		= 0;

	if (cChildren_ > 0) 
	{
		*rgiChildren	= new int[cChildren_];

	
		for (int iChild = 0; iChild < cChildren_; iChild++) 
		{
			MFnDagNode fnChild(fnNode.child(iChild));

			for (int iShape_ = 0; iShape_ < cShapes; iShape_++) 
			{
				MObject	objTransform_;

				DtExt_ShapeGetTransform(iShape_, objTransform_);

				if (fnChild.fullPathName() == MFnDagNode(objTransform_).fullPathName()) 
				{
					(*rgiChildren)[*cChildren]	= iShape_;
					(*cChildren)++;

					break;
				}
			}
		}
	}
	else
		*rgiChildren	= new int[1];	// just in case delete doesn't like "int[0]"

	return 1;
}


int	MyDtShapeGetVertices
	(
		MObject&	objInput, 
		MObject&	objOutput, 
		int*		pcVertices, 
		DtVec3f**	prgVertices
	) 
{
	*pcVertices		= 0;
	*prgVertices	= NULL;

	
	assert(objInput.hasFn(MFn::kMesh) && objOutput.hasFn(MFn::kMesh));


	MFnMesh	fnOutput(objOutput);
	MFnMesh	fnInput(objInput);

	*pcVertices		= fnOutput.numVertices();
	*prgVertices	= new DtVec3f[*pcVertices];

	if (!*prgVertices) 
	{
		*pcVertices	= 0;

		return 0;
	}

	// check for tweaks
	MPlug	plgTweakLoc	= fnOutput.findPlug("tweakLocation");

	MObject	objTweakLocVal;

	plgTweakLoc.getValue(objTweakLocVal);

	if (!objTweakLocVal.isNull())	// tweak found
	{	
		MPlugArray	rgplgTweakLocConnections;

		plgTweakLoc.connectedTo(rgplgTweakLocConnections, true, false);		// get source plugs

		assert(rgplgTweakLocConnections.length() == 1);

		MObject	objTweak = rgplgTweakLocConnections[0].node();

		assert(objTweak.hasFn(MFn::kTweak));

		MFnGeometryFilter	fnTweak(objTweak);

		bool	bRelativeTweak;

		fnTweak.findPlug("relativeTweak").getValue(bRelativeTweak);

		if (!bRelativeTweak) 
			cout << "\t\tWARNING: Encountered an absolute tweak; treating as relative!" << endl;

		MPlug plgOffsets = fnTweak.findPlug("vlist")[0].child(0);


		//	WARNING: Seems like Maya doesn't initialize it's numElements properly!!
//		assert((int)plgOffsets.numElements() == cVertices);
//		if ((int)plgOffsets.numElements() != *pcVertices)
//			cout << "\t\tWARNING: tweak count doesn't match vertex count!" << endl;

		float	fEnvelope	= fnTweak.envelope();

		for (int iVertex = 0; iVertex < *pcVertices; iVertex++) 
		{
			plgOffsets.elementByLogicalIndex(iVertex).child(0).getValue((*prgVertices)[iVertex].vec[0]);
			plgOffsets.elementByLogicalIndex(iVertex).child(1).getValue((*prgVertices)[iVertex].vec[1]);
			plgOffsets.elementByLogicalIndex(iVertex).child(2).getValue((*prgVertices)[iVertex].vec[2]);

			(*prgVertices)[iVertex].vec[0]	*= fEnvelope;
			(*prgVertices)[iVertex].vec[1]	*= fEnvelope;
			(*prgVertices)[iVertex].vec[2]	*= fEnvelope;
		}
	}
	else 
	{
		for (int iVertex = 0; iVertex < *pcVertices; iVertex++) 
		{
			(*prgVertices)[iVertex].vec[0]	= 0.0f;
			(*prgVertices)[iVertex].vec[1]	= 0.0f;
			(*prgVertices)[iVertex].vec[2]	= 0.0f;
		}
    }
			
			

	// load vertices and add them to the tweaks
	assert(*pcVertices == fnInput.numVertices());

	MFloatPointArray rgOrigVertices;

	fnInput.getPoints(rgOrigVertices);

	assert(*pcVertices == (int)rgOrigVertices.length());

	for (int iVertex = 0; iVertex < *pcVertices; iVertex++) 
	{
		(*prgVertices)[iVertex].vec[0] += rgOrigVertices[iVertex][0];
		(*prgVertices)[iVertex].vec[1] += rgOrigVertices[iVertex][1];
		(*prgVertices)[iVertex].vec[2] += rgOrigVertices[iVertex][2];
	}


	return 1;
}


int	MyDtTextureGetFileName
	(
		char*	szMaterial, 
		char**	pszTextureFile
	)
{
	if (!DtTextureGetFileName(szMaterial, pszTextureFile))
		return 0;

	if (g_bRelativeTexFile && *pszTextureFile) 
	{
		// use a trick to get the file name without too much hassle
		MFileObject mFile;

		mFile.setFullName(MString(*pszTextureFile));

        *pszTextureFile = new char[256];

        if (*pszTextureFile == NULL)
            return 0;

		strcpy(*pszTextureFile, mFile.name().asChar());

		g_Strings.add(*pszTextureFile);
	}

	return 1;
}







bool	MyDtShapeIsJoint
		(
			int	iShape
		) 
{
	MObject	objShape;

	DtExt_ShapeGetShapeNode(iShape, objShape);

	return (objShape.apiType() == MFn::kInvalid);
}





bool	MyDtShapeIsPatchMesh
		(
			int	iShape
		) 
{
	MObject	objNurb;

	DtExt_ShapeGetShapeNode(iShape, objNurb);

	if (!objNurb.hasFn(MFn::kNurbsSurface))
		return	false;		// not a nurbs surface

	MFnNurbsSurface	fnNurb(objNurb);

	if (fnNurb.degreeU() != 3 || fnNurb.degreeV() != 3)	
		return false;		// not a bicubic surface

	int	kFormInU	= fnNurb.formInU();
	int	kFormInV	= fnNurb.formInV();

	if (kFormInU == MFnNurbsSurface::kInvalid || kFormInV == MFnNurbsSurface::kInvalid)
		return false;		// surface has invalid form

	if (kFormInU == MFnNurbsSurface::kPeriodic || kFormInV == MFnNurbsSurface::kPeriodic)
		return false;		// can't handle periodic surfaces
	
	int	cCVsInU	= fnNurb.numCVsInU();
	int	cCVsInV	= fnNurb.numCVsInV();

	if ((cCVsInU - 1) % 3 != 0 || (cCVsInV - 1) % 3 != 0)
		return false;		// invalid control point count (we only deal with quad patches)

	int	cSpansInU	= (cCVsInU - 1) / 3;
	int	cSpansInV	= (cCVsInV - 1) / 3;

	if (cSpansInU <= 0 || cSpansInV <= 0)
		return false;		// invalid span count


	MPointArray rgCVs;

	fnNurb.getCVs(rgCVs);

	if ((int)rgCVs.length() != cCVsInU * cCVsInV)
		return false;		// inconsistency in cv count

	return true;			// all tests passed
}




int MyDtShapeGetVertices
	(
		int			iShape, 
		int*		pcVertices, 
		DtVec3f**	prgVertices
	)
{
	*pcVertices		= 0;
	*prgVertices	= NULL;

	DtVec3f*	rgVertices;

	if (!DtShapeGetVertices(iShape, pcVertices, &rgVertices))
	{
		*pcVertices	= 0;

		return 0;
	}

	if (!(*prgVertices	= new DtVec3f[*pcVertices]))
	{
		*pcVertices	= 0;

		return 0;
	}

	memcpy(*prgVertices, rgVertices, *pcVertices * sizeof(DtVec3f));

	return 1;
}


int MyDtShapeGetNormals
	(
		int			iShape, 
		int*		pcNormals, 
		DtVec3f**	prgNormals
	)
{
	*pcNormals	= 0;
	*prgNormals	= NULL;

	DtVec3f*	rgNormals;

	if (!DtShapeGetNormals(iShape, pcNormals, &rgNormals))
	{
		*pcNormals	= 0;

		return 0;
	}

	if (!(*prgNormals	= new DtVec3f[*pcNormals]))
	{
		*pcNormals	= 0;

		return 0;
	}

	memcpy(*prgNormals, rgNormals, *pcNormals * sizeof(DtVec3f));

	return 1;
}




int	MyDtShapeGetNormals
	(
		MObject&	objInput, 
		MObject&	objOutput, 
		int*		pcNormals, 
		DtVec3f**	prgNormals
	) 
{
	*pcNormals	= 0;
	*prgNormals	= NULL;

	assert(objInput.hasFn(MFn::kMesh) && objOutput.hasFn(MFn::kMesh));

	MFnMesh	fnInput(objInput);
	MFnMesh	fnOutput(objOutput);

	assert(fnInput.numNormals() == fnOutput.numNormals());
	
	*pcNormals	= fnInput.numNormals();
	*prgNormals	= new DtVec3f[*pcNormals];

	if (!*prgNormals)
	{
		*pcNormals	= 0;

		return 0;
	}

	MFloatVectorArray	rgOrigNormals;

	fnInput.getNormals(rgOrigNormals);

	assert(*pcNormals == (int)rgOrigNormals.length());

	for (int iNormal = 0; iNormal < *pcNormals; iNormal++) 
	{
		(*prgNormals)[iNormal].vec[0]	= rgOrigNormals[iNormal][0];
		(*prgNormals)[iNormal].vec[1]	= rgOrigNormals[iNormal][1];
		(*prgNormals)[iNormal].vec[2]	= rgOrigNormals[iNormal][2];
	}

	return 1;
}







int MyDtShapeGetTextureVertices
	(
		int			iShape, 
		int*		pcTexCoords, 
		DtVec2f**	prgTexCoords
	)
{
	*pcTexCoords	= 0;
	*prgTexCoords	= NULL;

	DtVec2f*	rgTexCoords;

	if (!DtShapeGetTextureVertices(iShape, pcTexCoords, &rgTexCoords))
	{
		*pcTexCoords	= 0;

		return 0;
	}

	if (!(*prgTexCoords	= new DtVec2f[*pcTexCoords]))
	{
		*pcTexCoords	= 0;

		return 0;
	}
	
	memcpy(*prgTexCoords, rgTexCoords, *pcTexCoords * sizeof(DtVec2f));

	return 1;
}





int MyDtShapeGetVerticesColor
	(
		int			iShape, 
		int*		pcColors, 
		DtRGBA**	prgColors
	)
{
	*pcColors	= 0;
	*prgColors	= NULL;

	DtRGBA*	rgColors;

	if (!DtShapeGetVerticesColor(iShape, pcColors, &rgColors))
	{
		*pcColors	= 0;

		return 0;
	}

	if (!(*prgColors	= new DtRGBA[*pcColors]))
	{
		*pcColors	= 0;

		return 0;
	}
	
	memcpy(*prgColors, rgColors, *pcColors * sizeof(DtRGBA));

	return 1;
}









int	MyDtShapeGetControlPoints
	(
		MObject&	objInput, 
		MObject&	objOutput, 
		int*		pcVertices, 
		DtVec3f**	prgVertices
	) 
{
	*pcVertices		= 0;
	*prgVertices	= NULL;

	
	assert(objInput.hasFn(MFn::kNurbsSurface) && objOutput.hasFn(MFn::kNurbsSurface));


	MFnNurbsSurface	fnOutput(objOutput);
	MFnNurbsSurface	fnInput(objInput);



	MPointArray rgCVs;

	fnInput.getCVs(rgCVs);

	*pcVertices		= rgCVs.length();
	*prgVertices	= new DtVec3f[*pcVertices];

	if (!*prgVertices) 
	{
		*pcVertices	= 0;

		return 0;
	}

	// WARNING:  Is this homogeneous coordinates? Should I divide w?
	for (int iVertex = 0; iVertex < *pcVertices; iVertex++) 
	{
		(*prgVertices)[iVertex].vec[0]	= (float)rgCVs[iVertex][0];
		(*prgVertices)[iVertex].vec[1]	= (float)rgCVs[iVertex][1];
		(*prgVertices)[iVertex].vec[2]	= (float)rgCVs[iVertex][2];
	}


	// check for tweaks
	MPlug	plgTweakLoc	= fnOutput.findPlug("tweakLocation");

	MObject	objTweakLocVal;

	plgTweakLoc.getValue(objTweakLocVal);

	if (!objTweakLocVal.isNull())	// tweak found
	{	
		MPlugArray	rgplgTweakLocConnections;

		plgTweakLoc.connectedTo(rgplgTweakLocConnections, true, false);		// get source plugs

		assert(rgplgTweakLocConnections.length() == 1);

		MObject	objTweak = rgplgTweakLocConnections[0].node();

		assert(objTweak.hasFn(MFn::kTweak));

		MFnGeometryFilter	fnTweak(objTweak);

		bool	bRelativeTweak;

		fnTweak.findPlug("relativeTweak").getValue(bRelativeTweak);

		if (!bRelativeTweak) 
			cout << "\t\tWARNING: Encountered an absolute tweak; treating as relative!" << endl;

		MPlug plgOffsets = fnTweak.findPlug("plist")[0].child(0);


		//	WARNING: Seems like Maya doesn't initialize it's numElements properly!!
//		assert((int)plgOffsets.numElements() == cVertices);
		if ((int)plgOffsets.numElements() != *pcVertices)
			cout << "\t\tWARNING: tweak count doesn't match vertex count!" << endl;

		float	fEnvelope	= fnTweak.envelope();

		for (int iVertex = 0; iVertex < *pcVertices; iVertex++) 
		{
			DtVec3f	vecOffset;

			plgOffsets.elementByLogicalIndex(iVertex).child(0).getValue(vecOffset.vec[0]);
			plgOffsets.elementByLogicalIndex(iVertex).child(1).getValue(vecOffset.vec[1]);
			plgOffsets.elementByLogicalIndex(iVertex).child(2).getValue(vecOffset.vec[2]);

			(*prgVertices)[iVertex].vec[0]	+= fEnvelope * vecOffset.vec[0];
			(*prgVertices)[iVertex].vec[1]	+= fEnvelope * vecOffset.vec[1];
			(*prgVertices)[iVertex].vec[2]	+= fEnvelope * vecOffset.vec[2];
		}
	}
			
			

	return 1;
}

















Mesh::Mesh() 
{
	m_kType					= Mesh::UNKNOWN;

	m_cGroups				= 0;
	m_rgGroups				= NULL;

	m_cVertices				= 0;
	m_rgVertices			= NULL;

	m_cVertexColors			= 0;
	m_rgVertexColors		= NULL;

	m_cNormals				= 0;
	m_rgNormals				= NULL;

	m_cTexCoords			= 0;
	m_rgTexCoords			= NULL;

	m_cReps					= 0;
	m_rgReps				= NULL;

	m_cFaces				= 0;
	m_rgFaces				= NULL;

    m_cFaceIndices			= 0;

	m_cBones				= 0;
	m_rgBones				= NULL;

	m_cMaxBonesPerFace		= 0;
	m_cMaxBonesPerVertex	= 0;
}



Mesh::~Mesh() 
{
	delete[] m_rgReps;

	delete[] m_rgVertices;
	delete[] m_rgNormals;
	delete[] m_rgTexCoords;
	delete[] m_rgVertexColors;

	for (int iFace = 0; iFace < m_cFaces; iFace++)
	{
		delete[] m_rgFaces[iFace].m_rgIndices;
	}

	delete[] m_rgFaces;

	delete[] m_rgGroups;

	for (int iBone = 0; iBone < m_cBones; iBone++) 
	{
		delete[] m_rgBones[iBone].m_rgfWeights;
		delete[] m_rgBones[iBone].m_rgiVertices;
	}

	delete[] m_rgBones;
}


Face::Face()
{
	m_cIndices	= 0;
	m_rgIndices	= NULL;

	m_iGroup	= -1;
}

Face::~Face() 
{
}


Bone::Bone()
{
	m_szName		= NULL;

	m_cWeights		= 0;
	m_rgfWeights	= NULL;
	m_rgiVertices	= NULL;

	m_cReps			= 0;
}

Bone::~Bone()
{
}