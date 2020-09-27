#ifndef MyDt_h
#define MyDt_h

// Dt API
#include <MDt.h>
#include <MDtExt.h>


struct Rep {
	int		m_iTexCoordIdx;		// index into lump of texture coordinates 
	int		m_iNormalIdx;		// index into lump of normals

	int		m_iNext;			// next repetition
	int		m_iFirst;			// can also be thought of as "index into lump of vertices";

	// IMPORTANT: the following members are only valid in the instances corresponging to the first rep's.
	int		m_cReps;					
};


struct Face {
	Face();
   ~Face();

	int		m_cIndices;			// number of vertices in this face
	int*	m_rgIndices;

	long	m_iGroup;			// material group
};



struct Group {
	char*	m_szMaterial;		// material name

	char*	m_szTextureFile;	// texture file name

	float	m_fDiffuseRed;		
	float	m_fDiffuseGreen;	// diffuse components
	float	m_fDiffuseBlue;		

	float	m_fSpecularRed;		
	float	m_fSpecularGreen;	// specular components
	float	m_fSpecularBlue;		

	float	m_fEmissiveRed;		
	float	m_fEmissiveGreen;	// emissive components
	float	m_fEmissiveBlue;		

	float	m_fShininess;		// specular power

	float	m_fTransparency;	// transparency
};


struct Bone {
	Bone();
   ~Bone();

	char*	m_szName;			// bone name

	int		m_cReps;			// number of rep's influenced

	int		m_cWeights;		// number of points influenced
	float*	m_rgfWeights;
	int*	m_rgiVertices;

	float	m_matOffset[4][4];	// inverse of bone's world coordinate transform at the time of binding
};


struct Mesh 
{
	Mesh();
   ~Mesh();

	enum ShapeType
	{
		UNKNOWN,
		POLY_MESH,
		PATCH_MESH,
		BONE
	};

	ShapeType	m_kType;

	// control point info
	
	int			m_cReps;
	Rep*		m_rgReps;
	
	int			m_cVertices;
	DtVec3f*	m_rgVertices;		// lump of vertices

	int			m_cVertexColors;
	DtRGBA*		m_rgVertexColors;	// lump of vertex colors

	int			m_cTexCoords;
	DtVec2f*	m_rgTexCoords;		// lump of texture coords

	int			m_cNormals;
	DtVec3f*	m_rgNormals;		// lump of normals


	// face info

	int			m_cFaces;
	Face*		m_rgFaces;

	int			m_cFaceIndices;	// total number of face indices (over all faces).

	// material info
	
	int			m_cGroups;
	Group*		m_rgGroups;			// material groups


	// skinning info

	int			m_cBones;
	Bone*		m_rgBones;

	int			m_cMaxBonesPerVertex;
	int			m_cMaxBonesPerFace;
};







		



struct Key 
{
	int		m_iFrame;

	float	m_rgfTRS[4][4];			// TRS transform matrix		- NOT USED

	float	m_rgfQuaternion[4];		// quaternion rotation
	float	m_rgfRotation[3];		// euler angles				- NOT USED
	float	m_rgfScale[3];			// scale
	float	m_rgfPosition[3];		// translation
};


struct Anim 
{
	char*	m_szName;

	int		m_cKeys;
	Key*	m_rgKeys;
};















































int	MyDtShapeGetParentID
	(
		int	iShape
	);


int MyDtShapeGetChildren
    (
		int     iShape, 
        int*    cChildren, 
        int**   rgiChildren
    );

int	MyDtShapeGetVertices
	(
		int			iShape,
		int*		pcVertices, 
		DtVec3f**	prgVertices
	);


int	MyDtShapeGetVertices
	(
		MObject&	objInput, 
		MObject&	objOutput, 
		int*		pcVertices, 
		DtVec3f**	prgVertices
	);

int	MyDtShapeGetControlPoints
	(
		MObject&	objInput, 
		MObject&	objOutput, 
		int*		pcVertices, 
		DtVec3f**	prgVertices
	);

int	MyDtShapeGetNormals
	(
		int			iShape,
		int*		pcNormals, 
		DtVec3f**	prgNormals
	);


int	MyDtShapeGetNormals
	(
		MObject&	objInput, 
		MObject&	objOutput, 
		int*		pcNormals, 
		DtVec3f**	prgNormals
	);

int	MyDtShapeGetTextureVertices
	(
		int			iShape,
		int*		pcTexCoords, 
		DtVec2f**	prgTexCoords
	);

int	MyDtShapeGetVerticesColor
	(
		int			iShape,
		int*		pcColors, 
		DtRGBA**	prgColors
	);

int	MyDtTextureGetFileName
	(
		char*	szMaterial, 
		char**	pszTextureFile
	);






bool	MyDtShapeIsJoint
		(
			int	iShape
		);


bool	MyDtShapeIsPatchMesh
		(
			int	iShape
		);
















#include <iostream.h>

class StringTable {
	public:
		StringTable() {};
	   ~StringTable() {};

		void reset();
		void clear();
		void add(char*);

	private:
		char**	m_rgszStrings;
		int		m_cStrings;
		int		m_cStringsMax;
};

inline void StringTable::reset() {
	m_cStrings		= 0;
	m_cStringsMax	= 64;
	m_rgszStrings	= new char*[m_cStringsMax];
}

inline void StringTable::clear() {
	for (int iString = 0; iString < m_cStrings; iString++)
		delete[] m_rgszStrings[iString];

	delete[] m_rgszStrings;
}

inline void StringTable::add(char* szString) {
	if (m_cStrings >= m_cStringsMax) {	// double the array size

		char**	rgszStrings	= m_rgszStrings;

		m_rgszStrings	= new char*[m_cStringsMax * 2];

        if (m_rgszStrings == NULL)
            return;

		m_cStringsMax *= 2;

		memcpy(m_rgszStrings, rgszStrings, m_cStrings * sizeof(char*));

		delete[] rgszStrings;
	}

	m_rgszStrings[m_cStrings]	= szString;

	m_cStrings++;
}












































#endif