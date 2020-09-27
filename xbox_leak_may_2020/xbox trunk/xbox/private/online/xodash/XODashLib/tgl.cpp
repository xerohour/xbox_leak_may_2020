/*************************************************************************************************\
tgl.cpp		    : IMplementation for the TG_Shape and TG_TypeShape classes.  These do the brunt of the
                    the work for our graphics.  They render, and load
                    and save themselves to files.
Creation Date		: 1/8/2002 11:08:29 AM
Copyright Notice	: (C) 2000 Microsoft
Author				: Heidi Gaertner
//-------------------------------------------------------------------------------------------------
Notes				: 
\*************************************************************************************************/
#include "std.h"
#include <stdio.h>
#include <io.h>

#ifndef TGL_H


#include "tgl.h"
#endif


#include "utilities.h"
#include "globals.h"
#include "File.h"
#define MAX_SCAN_LENGTH 256

// global data used during save time
DWORD TG_Shape::            s_dwNumFrames= 0;
DWORD TG_Shape::            s_dwFrameSpace= 0;
DWORD TG_Shape::            s_dwSubsetSpace= 0;
DWORD TG_Shape::            s_dwIndicesSpace= 0;
DWORD TG_Shape::            s_dwVerticesSpace= 0;
DWORD TG_Shape::            s_dwAnimSpace= 0;
DWORD TG_Shape::            s_dwMeshFileOffset= 0;
DWORD TG_Shape::            s_dwSubsetFileOffset= 0;
DWORD TG_Shape::            s_dwIndicesFileOffset= 0;
DWORD TG_Shape::            s_dwVerticesFileOffset = 0;
DWORD TG_Shape::            s_dwAnimFileOffset = 0;



// stuff to help with memory problems
#ifdef FIND_MEMORY_LEAK
void* __cdecl operator new(size_t nSize, const char* lpszFileName, int nLine);
void* __cdecl operator new[](size_t nSize, const char* lpszFileName, int nLine);

#define DEBUG_NEW new(__FILE__, __LINE__)
#undef new
#define new DEBUG_NEW
#endif
		


//-------------------------------------------------------------------------------
// ASE File Parse String Macros
#define ASE_OBJECT				"*GEOMOBJECT {"
#define ASE_HELP_OBJECT			"*HELPEROBJECT {"
#define ASE_MESH				"*MESH {"
#define ASE_NUM_VERTEX			"*MESH_NUMVERTEX"
#define ASE_NUM_FACE			"*MESH_NUMFACES"
#define ASE_VERTEX_LIST			"*MESH_VERTEX_LIST {"
#define ASE_FACE_LIST			"*MESH_FACE_LIST {"
#define ASE_NUM_TVERTEX			"*MESH_NUMTVERTEX"
#define ASE_TVERTLIST			"*MESH_TVERTLIST {"
#define ASE_NUM_TVFACES			"*MESH_NUMTVFACES"
#define ASE_TFACELIST			"*MESH_TFACELIST {"
#define ASE_NUM_CVERTEX			"*MESH_NUMCVERTEX"
#define ASE_CVERTLIST			"*MESH_CVERTLIST {"
#define ASE_NUM_CVFACES			"*MESH_NUMCVFACES"
#define	ASE_CFACELIST			"*MESH_CFACELIST {"
#define ASE_MESH_NORMALS		"*MESH_NORMALS {"

#define ASE_NODE_NAME			"*NODE_NAME"
#define ASE_GROUP_NAME          "GROUP"
#define ASE_NODE_POS			"*TM_POS"

#define ASE_ANIMATION			"*TM_ANIMATION {"
#define ASE_ANIM_ROT_HEADER		"*CONTROL_ROT_TRACK {"
#define ASE_ANIM_ROT_SAMPLE		"*CONTROL_ROT_SAMPLE"
#define ASE_ANIM_POS_HEADER		"*CONTROL_POS_TRACK {"
#define ASE_ANIM_POS_SAMPLE		"*CONTROL_POS_SAMPLE"
#define ASE_ANIM_FIRST_FRAME	"*SCENE_FIRSTFRAME"
#define ASE_ANIM_LAST_FRAME		"*SCENE_LASTFRAME"
#define ASE_ANIM_FRAME_SPEED	"*SCENE_FRAMESPEED"
#define ASE_ANIM_TICKS_FRAME	"*SCENE_TICKSPERFRAME"

#define ASE_FACE_NORMAL_ID		"*MESH_FACENORMAL"
#define ASE_VERTEX_NORMAL_ID	"*MESH_VERTEXNORMAL"
#define ASE_MESH_CFACE_ID		"*MESH_CFACE"
#define ASE_MESH_VERTCOL_ID		"*MESH_VERTCOL"
#define ASE_MESH_TFACE_ID		"*MESH_TFACE"
#define ASE_MESH_TVERT_ID		"*MESH_TVERT"
#define ASE_MESH_FACE_ID		"*MESH_FACE"
#define ASE_MESH_VERTEX_ID		"*MESH_VERTEX"

#define ASE_MATERIAL_COUNT		"*MATERIAL_COUNT"
#define ASE_SUBMATERIAL_COUNT	"*NUMSUBMTLS"
#define ASE_MATERIAL_ID			"*MATERIAL"
#define ASE_MATERIAL_BITMAP_ID	"*BITMAP "
#define ASE_MATERIAL_CLASS		"*MATERIAL_CLASS"
#define ASE_MATERIAL_TWOSIDED	"*MATERIAL_TWOSIDED"
#define ASE_FACE_MATERIAL_ID	"*MESH_MTLID"
#define ASE_MATERIAL_REF		"*MATERIAL_REF"
#define ASE_ROTATION_AXIS       "*TM_ROTAXIS"
#define ASE_ROTATION_ANGLE            "*TM_ROTANGLE"



//-------------------------------------------------------------------------------
/*extern bool useVertexLighting;
extern bool useFaceLighting;
extern bool hasGuardBand;
extern bool useFog;
extern DWORD BaseVertexColor;
bool drawOldWay = false;
extern bool useShadows;
bool useLocalShadows = false;

bool renderTGLShapes = true;

bool silentMode s false;*/		//Used for automated builds to keep errors from popping up.




//-------------------------------------------------------------------------------
// Frees memory and resets locals to defaults.
void TG_TypeNode::destroy (void)
{
	init();
}

//-------------------------------------------------------------------------------
TG_TypeNode::TG_TypeNode( TG_TypeNode& src )
{
    m_nodeCenter = src.m_nodeCenter;
	strcpy( m_strNodeId, src.m_strNodeId );
}

//-------------------------------------------------------------------------------
// virtual copy c'tor
TG_TypeNode* TG_TypeNode::Copy()
{
    return new TG_TypeNode( *this );
}

//-------------------------------------------------------------------------------
TG_TypeNode::TG_TypeNode( XBMESH_FRAME* pFrameData )
{
    init();

    // Frame info
    strcpy( m_strNodeId, pFrameData->m_strName );

    // Mesh info
    m_nodeCenter          = D3DXVECTOR3(0,0,0);
}



//-------------------------------------------------------------------------------
TG_TypeShape::TG_TypeShape( TG_TypeShape& src )
: TG_TypeNode( src )
{
    m_nNumVertices  = src.m_nNumVertices;
    m_nFVF          = src.m_nFVF;
    m_nFVFSize      = src.m_nFVFSize;
    m_nNumFaces     = src.m_nNumFaces;
    m_nMaterialCount= src.m_nMaterialCount;
    m_pMem          = NULL;
    m_pIndexBuffer  = NULL;

    m_VertexBuffer.Common =  D3DCOMMON_TYPE_VERTEXBUFFER | D3DCOMMON_VIDEOMEMORY | 1;


    if ( m_nNumVertices )
    {
        m_pMem = (BYTE*)D3D_AllocContiguousMemory( m_nFVFSize * m_nNumVertices, D3DVERTEXBUFFER_ALIGNMENT );
        BYTE* pMem = NULL;
        src.m_VertexBuffer.Lock( 0, 0, &pMem, 0 );
        memcpy( m_pMem, pMem, m_nFVFSize * m_nNumVertices );
        m_VertexBuffer.Data = 0;
        m_VertexBuffer.Register( m_pMem );
        src.m_VertexBuffer.Unlock();
    }

    if ( src.m_pIndexBuffer )
    {
        g_pd3dDevice->CreateIndexBuffer( sizeof ( WORD ) * m_nNumFaces * 3,  0, D3DFMT_D16, 0, &m_pIndexBuffer );
        WORD* pOldIndices;
        WORD* pIndices;
        src.m_pIndexBuffer->Lock( 0, 0, (BYTE**)&pOldIndices, 0 );
        m_pIndexBuffer->Lock( 0, 0, (BYTE**)&pIndices, 0 );
        memcpy( pIndices, pOldIndices, m_nNumFaces * 3*sizeof(WORD) );
        m_pIndexBuffer->Unlock();
        src.m_pIndexBuffer->Unlock();

    }

    m_pMaterialIDs = new XBMESH_SUBSET[m_nMaterialCount];

    for ( unsigned long i = 0; i < m_nMaterialCount; i++ )
    {
        m_pMaterialIDs[i] = src.m_pMaterialIDs[i];
    }

}

//-------------------------------------------------------------------------------
// virtual copy c'tor
TG_TypeNode* TG_TypeShape::Copy()
{
    return new TG_TypeShape( *this );
}



//-------------------------------------------------------------------------------
//Frees memory and resets locals to defaults.
void TG_TypeShape::destroy (void)

{
    // do not release the vertex buffer, since QueryInterface was never called
    if ( m_pMaterialIDs )
		delete [] m_pMaterialIDs;

	if ( m_pIndexBuffer )
		m_pIndexBuffer->Release();

	m_pMaterialIDs = NULL;
	m_pIndexBuffer = NULL;

    if ( m_pMem ) // for aseconv only ... otherwise the scene cleans up ALL the memory
    {
        D3D_FreeContiguousMemory( m_pMem );
        m_pMem = NULL;

    }

}	

//-------------------------------------------------------------------------------
//Frees memory and resets locals to defaults.
void TG_Shape::Destroy (void)
{

    TG_Shape* pShape = m_pChild;
    while( pShape )
    {
        TG_Shape* pTmpShape = pShape->m_pNext;
        delete pShape;
        pShape = pTmpShape;
    }

    m_pChild = NULL;
    //THIS IS CORRECT!!!!
	// They come from a common pool now, Deleteing them would be BAD!!
    // unless we read from an ASE
//    if ( m_bDeleteType )
      delete m_pMyType;

    m_pMyType = NULL;

    if ( !m_bCommonMemory )
    {
         if ( m_Animation.m_pQuat )
                delete [] m_Animation.m_pQuat;

         if ( m_Animation.m_pPos )
                delete [] m_Animation.m_pPos;
    }

    m_Animation.m_pQuat = NULL;
    m_Animation.m_pPos = NULL;

}	

//-------------------------------------------------------------------------------


HRESULT GetTextureIndices( BYTE* aseBuffer, long faceNum, long& t0, long& t1, long& t2 )
{
	char faceId[256];
	sprintf(faceId,"%s",ASE_NUM_TVFACES);

	char numberData[256];

	char* faceData = strstr((char *)aseBuffer,faceId);
	if (faceData)
	{
		faceData += strlen(faceId);
 		GetNumberData(faceData,numberData);

		sprintf(faceId,"%s %d",ASE_MESH_TFACE_ID,faceNum);

		faceData = strstr(faceData,faceId);
		_ASSERT(faceData != NULL);

		faceData += strlen(faceId);
			
		GetNumberData(faceData,numberData);
		t0 = atol(numberData);
		faceData += strlen(numberData)+1;
			
		GetNumberData(faceData,numberData);
		t1 = atol(numberData);
		faceData += strlen(numberData)+1;

		GetNumberData(faceData,numberData);
		t2 = atol(numberData);

		return S_OK;
	}

	return -1;
}

HRESULT GetTextureUVs( BYTE* aseBuffer, long index, float& u, float& v )
{
	char numberData[256];
	
	u = 0.f;
	v = 0.f;
	char tmpBuffer[255];
	//Load up the TVERT0
	sprintf(tmpBuffer,"%s",ASE_NUM_TVERTEX);

	char* faceData = strstr((char *)aseBuffer,tmpBuffer);
	_ASSERT(faceData != NULL);

	sprintf(tmpBuffer,"%s %d",ASE_MESH_TVERT_ID,index);

	faceData = strstr(faceData,tmpBuffer);
	_ASSERT(faceData != NULL);

	faceData += strlen(tmpBuffer);
		
	GetNumberData(faceData,numberData);
	u = (float)atof(numberData);

	if ((u > 100.0f) || (u < -100.0f))
	{
#ifdef _DEBUG
//		if (!silentMode)
//			PAUSE(("WARNING: U0 is %f which is out of range in Shape %s!",u,fileName));
#endif
		u = 0.0f;
	}

	faceData += strlen(numberData)+1;
		
	GetNumberData(faceData,numberData);
	v = 1.0f - (float)atof(numberData);

	if ((v > 100.0f) || (v < -100.0f))
	{
#ifdef _DEBUG
//		if (!silentMode)
//			PAUSE(("WARNING: V0 is %f which is out of range in Shape %s!",listOfTypeTriangles[i].uvdata.v0,fileName));
#endif
		v = 0.0f;
	}

	return 0;
}


// Read in color data
HRESULT getColorData( BYTE* aseBuffer, long cIndex1, long cIndex2, long cIndex3, unsigned long& color1,	
						unsigned long& color2, unsigned long& color3 )
{
	long alpha = 0xa8; // this needs to be passed in
	char faceId[256];

	char numberData[256];

	sprintf(faceId,"%s",ASE_NUM_CVERTEX);

	char* faceData = strstr((char *)aseBuffer,faceId);
	_ASSERT(faceData != NULL);

	sprintf(faceId,"%s %d",ASE_MESH_VERTCOL_ID,(long)cIndex1);

	faceData = strstr(faceData,faceId);
	_ASSERT(faceData != NULL);

	faceData += strlen(faceId);
		
	GetNumberData(faceData,numberData);
	float red = (float)atof(numberData);

	faceData += strlen(numberData)+1;
		
	GetNumberData(faceData,numberData);
	float green = (float)atof(numberData);

	faceData += strlen(numberData)+1;

	GetNumberData(faceData,numberData);
	float blue = (float)atof(numberData);

	DWORD redC = (DWORD)(red * 255.f + .5);
	DWORD greenC = (DWORD)(green * 255.f + .5);
	DWORD blueC = (DWORD)(blue * 255.f + .5);

	color1 = (alpha << 24) + (redC << 16) + (greenC << 8) + blueC;

	//Load up the VERTCOL1
	sprintf(faceId,"%s",ASE_NUM_CVERTEX);

	faceData = strstr((char *)aseBuffer,faceId);
	_ASSERT(faceData != NULL);

	sprintf(faceId,"%s %d",ASE_MESH_VERTCOL_ID,(long)cIndex2);

	faceData = strstr(faceData,faceId);
	_ASSERT(faceData != NULL);

	faceData += strlen(faceId);
		
	GetNumberData(faceData,numberData);
	red = (float)atof(numberData);

	faceData += strlen(numberData)+1;
		
	GetNumberData(faceData,numberData);
	green = (float)atof(numberData);

	faceData += strlen(numberData)+1;

	GetNumberData(faceData,numberData);
	blue = (float)atof(numberData);

	redC = (DWORD)(red * 255.f + .5);
	greenC = (DWORD)(green * 255.f + .5);
	blueC = (DWORD)(blue * 255.f + .5);
	
	color2 = (alpha << 24) + (redC << 16) + (greenC << 8) + blueC;

	//Load up the VERTCOL2
	sprintf(faceId,"%s",ASE_NUM_CVERTEX);

	faceData = strstr((char *)aseBuffer,faceId);
	_ASSERT(faceData != NULL);

	sprintf(faceId,"%s %d",ASE_MESH_VERTCOL_ID,(long)cIndex3);

	faceData = strstr(faceData,faceId);
	_ASSERT(faceData != NULL);

	faceData += strlen(faceId);
		
	GetNumberData(faceData,numberData);
	red = (float)atof(numberData);

	faceData += strlen(numberData)+1;
		
	GetNumberData(faceData,numberData);
	green = (float)atof(numberData);

	faceData += strlen(numberData)+1;

	GetNumberData(faceData,numberData);
	blue = (float)atof(numberData);

	redC = (DWORD)(red * 255.f + .5);
	greenC = (DWORD)(green * 255.f + .5);
	blueC = (DWORD)(blue * 255.f + .5);
	
	color3 = (alpha << 24) + (redC << 16) + (greenC << 8) + blueC;

	return 0;
}

////////////////////////////////////////////////////////////////////////////
// Hack for figuring out how much room we have for text
// this will never work for seriously rotated text...
void        TG_TypeShape::GetRectDims( float& width, float& height )
{
    width = 0;
    height = 0;

    ASSERT( m_nNumVertices > 3 );
    
    if ( m_nNumVertices > 3 )
    {
        BYTE* pBuffer = NULL;
        m_VertexBuffer.Lock(0, 0, &pBuffer, 0);

        // need to figure out the size of the vertex
        D3DVECTOR* v1 = (D3DVECTOR*)pBuffer;
        D3DVECTOR* v2 = (D3DVECTOR*)(pBuffer + m_nFVFSize);
        D3DVECTOR* v3 = (D3DVECTOR*)(pBuffer + 2*m_nFVFSize);
        D3DVECTOR* v4 = (D3DVECTOR*)(pBuffer + 3*m_nFVFSize);

        width = (float)fabs(v1->x - v2->x);
        float fWidth2 = (float)fabs(v3->x - v1->x);

        height = (float)fabs(v1->y - v2->y);
        float fHeight2 = (float)fabs(v3->y - v1->y);

        if ( fWidth2 > width )
        {
            width =  fWidth2;
        }
        if ( fHeight2 > height )
        {
            height = fHeight2;

        }


        m_VertexBuffer.Unlock();

    }
}
////////////////////////////////////////////////////////////////////////////
// this function will never work on under than the x/y plane...
void        TG_TypeShape::GetRectDimsAndRecenter(float& width, float& height, float& depth)
{
    width = 0;
    height = 0;

    ASSERT( m_nNumVertices > 3 );
    
    if ( m_nNumVertices > 3 )
    {
        BYTE* pBuffer = NULL;
        m_VertexBuffer.Lock(0, 0, &pBuffer, 0);

        // need to figure out the size of the vertex
        D3DVECTOR* v1 = (D3DVECTOR*)pBuffer;
        D3DVECTOR* v2 = (D3DVECTOR*)(pBuffer + m_nFVFSize);
        D3DVECTOR* v3 = (D3DVECTOR*)(pBuffer + 2*m_nFVFSize);
        D3DVECTOR* v4 = (D3DVECTOR*)(pBuffer + 3*m_nFVFSize);

        width = (float)fabs(v1->x - v2->x);
        float fNewCenterX = (v1->x + v2->x)/2.f;

        float fWidth2 = (float)fabs(v3->x - v1->x);

        height = (float)fabs(v1->z - v2->z);
        float fHeight2 = (float)fabs(v3->z - v1->z);
        float fNewCenterZ = (v1->z + v2->z)/2.f;

        depth = (float)fabs(v1->y - v2->y);
        float fDepth2 = (float)fabs(v3->y - v1->y);
        float fNewCenterY = (v1->y + v2->y)/2.f;


        if ( fWidth2 > width )
        {
            fNewCenterX = (v3->x + v1->x)/2.f;
            width =  fWidth2;
        }
        if ( fHeight2 > height )
        {
            height = fHeight2;
            fNewCenterZ = (v1->z + v3->z)/2.f;

        }
        if ( fDepth2 > depth )
        {
            depth = fDepth2;
            fNewCenterY = (v1->z + v3->z)/2.f;
        }

        m_nodeCenter.x += fNewCenterX;
        m_nodeCenter.z += fNewCenterZ;
        m_nodeCenter.y += fNewCenterY;


        m_VertexBuffer.Unlock();

    }
}


long TG_TypeShape::ParseASENoTextures (BYTE *aseBuffer, const char *fileName, const D3DVECTOR& parentLoc)
{
	m_nFVF = NORMAL_VERTEX_FVF;
	m_nFVFSize = sizeof( TG_NormalVertex );
	
	//------------------------------------------
	// Store off the Node Names.
	char *nodeName = strstr((char *)aseBuffer,ASE_NODE_NAME);
	if (!nodeName )
    {
        nodeName = strstr((char *)aseBuffer,ASE_GROUP_NAME);
      	nodeName += strlen(ASE_GROUP_NAME)+1;

    }
    else
    {
       	nodeName += strlen(ASE_NODE_NAME)+1;
    }

	_ASSERT(nodeName != NULL);
	char nodeString[1024];
	GetNameData(nodeName,nodeString);

    ASSERT( strlen( nodeString ) < 32 );

	strncpy(m_strNodeId,nodeString,32);
    CharUpperA( m_strNodeId );
	unsigned long maxMaterialCount = 0;

	//----------------------------------------------------
	// Store off NODE ABS position for heirarchy
	nodeName = strstr((char *)aseBuffer,ASE_NODE_POS);
    if ( !nodeName ) // this is a group
        return -1;

	_ASSERT(nodeName != NULL);

	nodeName += strlen(ASE_NODE_POS)+1;

	char numData[512];

	GetNumberData(nodeName,numData);
	m_nodeCenter.x = (float)atof(numData);
	nodeName += strlen(numData)+1;

	GetNumberData(nodeName,numData);
	m_nodeCenter.y = (float)atof(numData);
	nodeName += strlen(numData)+1;

	GetNumberData(nodeName,numData);
	m_nodeCenter.z = (float)atof(numData);

//    m_nodeCenter += parentLoc;

    m_fMinZDepth = -99999999999.f;



	//----------------------------------------
	// Find the Number of Vertices
	char *vertexCount = strstr((char *)aseBuffer,ASE_NUM_VERTEX);

    if ( !vertexCount ) // dummy object, probably just animates
        return -1;
	_ASSERT(vertexCount != NULL);
		
	vertexCount += strlen(ASE_NUM_FACE)+1;
	long numTmpVertices = atol(vertexCount);
	if (numTmpVertices == 0)			//NO Data for this node.  WARN and then do NOT allocate anything!
	{
#ifdef _DEBUG
//		if (!silentMode)
//			PAUSE(("WARNING: No Vertices for Shape Node %s in Shape %s!",m_strNodeID,fileName));
#endif
		init();

		return (0);
	}

	//---------------------------------------
	// Find the material index of the object
	//---------------------------------------
	char *matIndex = strstr((char *)aseBuffer,ASE_MATERIAL_REF);
    unsigned long nMaterialNum = 0;

	if ( matIndex )
    {
    // Pull the ASE specific material index
	    matIndex += strlen(ASE_MATERIAL_REF)+1;
	    nMaterialNum = atol(matIndex);
    }

	// Now find the cooresponding material index in the Material Library
	long nMaterialIndex = g_MaterialLib.ReturnIndex(nMaterialNum);

	//---------------------------------------
	// Find the Number of faces
	//---------------------------------------
	char *faceCount = strstr((char *)aseBuffer,ASE_NUM_FACE);
	_ASSERT(faceCount != NULL);
			
	faceCount += strlen(ASE_NUM_FACE)+1;
	long numFaces = atol(faceCount);
	if ( numFaces )
	{
		m_nNumVertices = numTmpVertices;
        m_nNumFaces = numFaces;

		WORD* pIndices = new WORD[numFaces*3];

        // going to deliberately allocate too much memory here, we can't always share normals, 
        // too many square objects
        BYTE* pVertexMem = (BYTE*)D3D_AllocContiguousMemory( numFaces * 3 * sizeof( TG_NormalVertex ), D3DVERTEXBUFFER_ALIGNMENT );

		g_pd3dDevice->CreateIndexBuffer( numFaces * 3 * sizeof( WORD ), 0, D3DFMT_D16, 0, &m_pIndexBuffer);

		// going to store materials, and then sort them later
		long*	tmpMaterialArray = new long[ numFaces ];
		memset( tmpMaterialArray, 0, sizeof ( long ) * numFaces );
		
		TG_NormalVertex* finalVertices = (TG_NormalVertex*)pVertexMem;
		memset(finalVertices,0xff,sizeof(TG_NormalVertex) * numFaces * 3);

		long*	normalCounts = new long[ numFaces * 3];
		memset( normalCounts, 0, sizeof( long ) *  numFaces * 3 );

        long*   altVertex = new long[numFaces * 3];
        memset( altVertex, 0, sizeof( long ) *  numFaces * 3 );

		for (long i=0;i<numTmpVertices;i++)
		{
			char numberData[256];

			//------------------------------------------------
			// First the Vertex Position
			char vertexID[256];
			sprintf(vertexID,"%s% 5d",ASE_MESH_VERTEX_ID,i);

			char *vertexData = strstr((char *)aseBuffer,vertexID);
			_ASSERT(vertexData != NULL);
				
			vertexData += strlen(vertexID)+1;

			GetNumberData(vertexData,numberData);
			vertexData += strlen(numberData)+1;
			finalVertices[i].position.x = (float)atof(numberData);
			finalVertices[i].position.x -= m_nodeCenter.x;

			GetNumberData(vertexData,numberData);
			vertexData += strlen(numberData)+1;
			finalVertices[i].position.y = (float)atof(numberData);
			finalVertices[i].position.y -= m_nodeCenter.y;

			GetNumberData(vertexData,numberData);
			finalVertices[i].position.z = (float)atof(numberData);
			finalVertices[i].position.z -= m_nodeCenter.z;
            finalVertices[i].normal.x = 0.f;
            finalVertices[i].normal.y = 0.f;
            finalVertices[i].normal.z = 0.f;

            if ( finalVertices[i].position.y > m_fMinZDepth )
                m_fMinZDepth = finalVertices[i].position.y;

		}


        char* prevFace = (char*)aseBuffer;
        char* prevNormal = (char*)aseBuffer;

		for (i=0;i<numFaces;i++)
		{
			char numberData[256];

			//-----------------------------------------------
			// First the Vertices for the face
			char faceId[256];
			sprintf(faceId,"%s% 5d:",ASE_MESH_FACE_ID,i);

			char *faceData = strstr((char *)prevFace,faceId);
			_ASSERT(faceData != NULL);

            prevFace = faceData;

			faceData = strstr(faceData,"A:");
			_ASSERT(faceData != NULL);
				
			GetNumberData(faceData,numberData);
			long index0 = atol(numberData);

			faceData = strstr(faceData,"B:");
			_ASSERT(faceData != NULL);

			GetNumberData(faceData,numberData);
			long index1 = atol(numberData);

			faceData = strstr(faceData,"C:");
			_ASSERT(faceData != NULL);
		
			GetNumberData(faceData,numberData);
			long index2 = atol(numberData);

			faceData = strstr( faceData, "MESH_MTLID " );
			GetNumberData(faceData,numberData);
			unsigned long matrlID = atol(numberData );

//			tmpMaterialArray[i] = matrlID;
			tmpMaterialArray[i] = nMaterialIndex;
//			if ( matrlID >= maxMaterialCount ) // store the number so we can make the array
//				maxMaterialCount = matrlID + 1;


            D3DXVECTOR3 v1, v2, tmpNormal;
            v1 = finalVertices[index2].position - finalVertices[index1].position;
            v2 = finalVertices[index0].position - finalVertices[index1].position;
            

            D3DXVec3Cross( &tmpNormal, &v2, &v1 );
            D3DXVec3Normalize( &tmpNormal, &tmpNormal );

            // figure out what the avg normal is now, if this one is more than ~10deg off
            // then make a new vertex with a new normal, move around the index
            if ( CreateNewNormal( tmpNormal, finalVertices, normalCounts, index0, altVertex ) )
            {
                finalVertices[m_nNumVertices] = finalVertices[index0];
                finalVertices[m_nNumVertices].normal.x = finalVertices[m_nNumVertices].normal.y = 
                        finalVertices[m_nNumVertices].normal.z = 0.f;

                altVertex[index0] = m_nNumVertices;
                index0 = m_nNumVertices;
            
                m_nNumVertices++;
            }
           

            
            if ( CreateNewNormal( tmpNormal, finalVertices, normalCounts, index1, altVertex ) )
            {
                finalVertices[m_nNumVertices] = finalVertices[index1];
                finalVertices[m_nNumVertices].normal.x = finalVertices[m_nNumVertices].normal.y = 
                        finalVertices[m_nNumVertices].normal.z = 0.f;
                altVertex[index1] = m_nNumVertices;
                index1 = m_nNumVertices;

                m_nNumVertices++;
            }

            if ( CreateNewNormal( tmpNormal, finalVertices, normalCounts, index2, altVertex ) )
            {
                finalVertices[m_nNumVertices] = finalVertices[index2];
                finalVertices[m_nNumVertices].normal.x = finalVertices[m_nNumVertices].normal.y = 
                        finalVertices[m_nNumVertices].normal.z = 0.f;
                altVertex[index2] = m_nNumVertices;
                index2 = m_nNumVertices;

                m_nNumVertices++;
            }
  
            finalVertices[index0].normal += tmpNormal;
            finalVertices[index1].normal += tmpNormal;
            finalVertices[index2].normal += tmpNormal;


            normalCounts[index0]++;
            normalCounts[index1]++;
            normalCounts[index2]++;


   			// set up the vertices
			pIndices[3*i]	=	(WORD)index0;
			pIndices[3*i+1] =	(WORD)index1;
			pIndices[3*i+2] =	(WORD)index2;


			//------------------------------------------------------------		
			//------------------------------------------------------------
			//------------------------------------------------------------

		}

		// now I need to go back and calculate the average normals
		for ( unsigned long j = 0; j < m_nNumVertices; j++ )
		{
			if ( normalCounts[j] > 0 )
			{
				finalVertices[j].normal.x/=(float)normalCounts[j];
				finalVertices[j].normal.y/=(float)normalCounts[j];
				finalVertices[j].normal.z/=(float)normalCounts[j];

                D3DXVec3Normalize( &finalVertices[j].normal,  &finalVertices[j].normal );
                
			}
		}


		// OK, now need to sort the faces based on materials, saves on state changes later
		m_pMaterialIDs = new XBMESH_SUBSET[maxMaterialCount+1];

		WORD* pFinalIndices = NULL;
		m_pIndexBuffer->Lock(0, 0, (BYTE**)&pFinalIndices, 0);

		for ( int i = 0; i < numFaces; i++ )
		{	
			*pFinalIndices++ = pIndices[3*i];
			*pFinalIndices++ = pIndices[3*i+1];
			*pFinalIndices++ = pIndices[3*i+2];		
		}
		m_nMaterialCount = 1;
		m_pMaterialIDs[0].m_ID = nMaterialIndex;
		m_pMaterialIDs[0].m_numUsed = numFaces;

		m_pIndexBuffer->Unlock();
        m_VertexBuffer.Data = 0;
        m_VertexBuffer.Register( pVertexMem );
        m_pMem = pVertexMem; // so we free later



		delete [] pIndices;
		delete [] tmpMaterialArray;
		delete [] normalCounts;
        delete [] altVertex;

        m_nodeCenter -= parentLoc;

	}
	else // no faces
	{
		return -1;
	}

	return(S_OK);
}

// helper function to determine wether the new vector is significantly different than the old one
bool TG_TypeShape::CreateNewNormal( const D3DXVECTOR3& newNormal, TG_NormalVertex* pListOfVertices, 
                                   long* normalCounts, long& index, long* altIndex  )
{
    while(true)
    {
        if ( normalCounts[index] )
        {
            D3DXVECTOR3 oldNormal = pListOfVertices[index].normal/((float)normalCounts[index]);
            if ( (float)(fabs( D3DXVec3Dot( &oldNormal, &newNormal )) < cos(20.f * D3DX_PI/180.f) ) )
            {
                if ( altIndex[index]  )
                {
                    index = altIndex[index];
                }
                else
                {
                    return true;
                }
            }
            else
                break;
        }
        else
            break;
    }

    return false;

}


long TG_TypeShape::ParseASEColors (BYTE *aseBuffer, const char *fileName, const D3DVECTOR& parentLoc)
{
	m_nFVF = COLOR_VERTEX_FVF;
	m_nFVFSize = sizeof( TG_ColorVertex );

    m_fMinZDepth = -9999999.f;

	//------------------------------------------
	// Store off the Node Names.
	char *nodeName = strstr((char *)aseBuffer,ASE_NODE_NAME);
	if (!nodeName )
    {
        nodeName = strstr((char *)aseBuffer,ASE_GROUP_NAME);
      	nodeName += strlen(ASE_GROUP_NAME)+1;

    }
    else
    {
       	nodeName += strlen(ASE_NODE_NAME)+1;
    }

	_ASSERT(nodeName != NULL);


	char nodeString[1024];
	GetNameData(nodeName,nodeString);

    ASSERT( strlen( nodeString ) < 32 );

	strncpy(m_strNodeId,nodeString,32);
    CharUpperA( m_strNodeId );
	
	//----------------------------------------------------
	// Store off NODE ABS position for heirarchy
	nodeName = strstr((char *)aseBuffer,ASE_NODE_POS);
	_ASSERT(nodeName != NULL);
    if ( !nodeName ) // this is a group
       return -1;


	nodeName += strlen(ASE_NODE_POS)+1;

	char numData[512];

	GetNumberData(nodeName,numData);
	m_nodeCenter.x = (float)atof(numData);
	nodeName += strlen(numData)+1;

	GetNumberData(nodeName,numData);
	m_nodeCenter.y = (float)atof(numData);
	nodeName += strlen(numData)+1;

	GetNumberData(nodeName,numData);
	m_nodeCenter.z = (float)atof(numData);

    //m_nodeCenter += parentLoc;

	//----------------------------------------
	// Find the Number of Vertices
	char *vertexCount = strstr((char *)aseBuffer,ASE_NUM_VERTEX);
    if ( !vertexCount )
        return -1;
	_ASSERT(vertexCount != NULL);
		
	vertexCount += strlen(ASE_NUM_FACE)+1;
	long numTmpVertices = atol(vertexCount);
	if (numTmpVertices == 0)			//NO Data for this node.  WARN and then do NOT allocate anything!
	{
#ifdef _DEBUG
//		if (!silentMode)
//			PAUSE(("WARNING: No Vertices for Shape Node %s in Shape %s!",m_strNodeID,fileName));
#endif
		init();

		return (0);
	}


	//---------------------------------------
	// Find the Number of faces
	char *faceCount = strstr((char *)aseBuffer,ASE_NUM_FACE);
	_ASSERT(faceCount != NULL);
			
	faceCount += strlen(ASE_NUM_FACE)+1;
	long numFaces = atol(faceCount);
	if ( numFaces )
	{

		TG_NormalVertex* pTmpVertices = new TG_NormalVertex[numTmpVertices];
		memset( pTmpVertices, 0xff, numTmpVertices * sizeof( TG_NormalVertex ) );

	
		m_nMaterialCount = 1;
		m_pMaterialIDs = new XBMESH_SUBSET;
		m_pMaterialIDs[0].m_ID = -1;
		m_pMaterialIDs[0].m_numUsed = numFaces;
		//-------------------
		// Vertex Data Next.

		m_nNumVertices = numFaces * 3;

        BYTE* pVertexMem = (BYTE*)D3D_AllocContiguousMemory( m_nNumVertices * sizeof( TG_ColorVertex ), D3DVERTEXBUFFER_ALIGNMENT );
		TG_ColorVertex* finalVertices = (TG_ColorVertex*)pVertexMem;

		memset(finalVertices,0xff,sizeof(TG_ColorVertex) * m_nNumVertices);

		for (long i=0;i<numTmpVertices;i++)
		{
			char numberData[256];

			//------------------------------------------------
			// First the Vertex Position
			char vertexID[256];
			sprintf(vertexID,"%s% 5d",ASE_MESH_VERTEX_ID,i);

			char *vertexData = strstr((char *)aseBuffer,vertexID);
			_ASSERT(vertexData != NULL);
				
			vertexData += strlen(vertexID)+1;

			GetNumberData(vertexData,numberData);
			vertexData += strlen(numberData)+1;
			pTmpVertices[i].position.x = (float)atof(numberData);
			pTmpVertices[i].position.x -= m_nodeCenter.x;

			GetNumberData(vertexData,numberData);
			vertexData += strlen(numberData)+1;
			pTmpVertices[i].position.y = (float)atof(numberData);
			pTmpVertices[i].position.y -= m_nodeCenter.y;

			GetNumberData(vertexData,numberData);
			pTmpVertices[i].position.z = (float)atof(numberData);
			pTmpVertices[i].position.z -= m_nodeCenter.z;

            if ( pTmpVertices[i].position.y > m_fMinZDepth )
                m_fMinZDepth = pTmpVertices[i].position.y;



		}



        char* prevFace = (char*)aseBuffer;
        char* prevColor = (char*)aseBuffer;

		for (i=0;i<numFaces;i++)
		{
			char numberData[256];

			//-----------------------------------------------
			// First the Vertices for the face
			char faceId[256];
			sprintf(faceId,"%s% 5d:",ASE_MESH_FACE_ID,i);

			char *faceData = strstr((char *)prevFace,faceId);
			_ASSERT(faceData != NULL);
            prevFace = faceData;

			faceData = strstr(faceData,"A:");
			_ASSERT(faceData != NULL);
				
			GetNumberData(faceData,numberData);
			long index0 = atol(numberData);

			faceData = strstr(faceData,"B:");
			_ASSERT(faceData != NULL);

			GetNumberData(faceData,numberData);
			long index1 = atol(numberData);

			faceData = strstr(faceData,"C:");
			_ASSERT(faceData != NULL);
		
			GetNumberData(faceData,numberData);
			long index2 = atol(numberData);

			// set up the vertices
			finalVertices[3*i].position.x = pTmpVertices[index0].position.x;
			finalVertices[3*i].position.y = pTmpVertices[index0].position.y;
			finalVertices[3*i].position.z = pTmpVertices[index0].position.z;

			finalVertices[3*i+1].position.x = pTmpVertices[index1].position.x;
			finalVertices[3*i+1].position.y = pTmpVertices[index1].position.y;
			finalVertices[3*i+1].position.z = pTmpVertices[index1].position.z;

			finalVertices[3*i+2].position.x = pTmpVertices[index2].position.x;
			finalVertices[3*i+2].position.y = pTmpVertices[index2].position.y;
			finalVertices[3*i+2].position.z = pTmpVertices[index2].position.z;

			//------------------------------------------------------------

			//---------------------------------------------------------------------
			// Color Data for the face.
			//Load up the CFaces
			// Shelley says she doesn't do this.
			
			unsigned long color1, color2, color3;
			color1 = color2 = color3 = 0xffffffff;
			
			sprintf(faceId,"%s",ASE_NUM_CVFACES);


			faceData = strstr((char *)aseBuffer,faceId);
			if (faceData)
			{
				_ASSERT(faceData != NULL);

				faceData += strlen(faceId);
				GetNumberData(faceData,numberData);

				long cIndex1, cIndex2, cIndex3;
				sprintf(faceId,"%s %d",ASE_MESH_CFACE_ID,i);

				faceData = strstr(prevColor,faceId);
                prevColor = faceData;
				_ASSERT(faceData != NULL);

				faceData += strlen(faceId);
					
				GetNumberData(faceData,numberData);
				cIndex1 = atol(numberData);
				faceData += strlen(numberData)+1;
					
				GetNumberData(faceData,numberData);
				cIndex2 = atol(numberData);
				faceData += strlen(numberData)+1;

				GetNumberData(faceData,numberData);
				cIndex3 = atol(numberData);

				//Load up the VERTCOL0

				getColorData( aseBuffer, cIndex1, cIndex2, cIndex3, color1, color2, color3 );
				
			}
			
			finalVertices[3*i].argb = color1;
			finalVertices[3*i+1].argb = color2;
			finalVertices[3*i+2].argb = color3;

		
		}	

        m_VertexBuffer.Data = 0;
        m_VertexBuffer.Register( pVertexMem );
        m_pMem = pVertexMem; // so we free later

		delete [] pTmpVertices;

        m_nodeCenter -= parentLoc;
		
	}
	else // no faces
	{
		return -1;
	}

	return(S_OK);
}	

//-------------------------------------------------------------------------------
//Function return 0 is OK.  -1 if file is not ASE Format or missing data.
//This function simply parses the ASE buffers handed to it.  This allows
//users to load the ase file themselves and manage their own memory for it.
//It allocates memory for internal Lists.  These are straight tglHeap->Mallocs at present.
//
// NOTE: Only takes the first GEOMOBJECT from the ASE file.  Multi-object
// Files will require user intervention to parse!!


long TG_TypeShape::ParseASEFile (BYTE *aseBuffer, const char *fileName, const D3DVECTOR& parentLoc)
{
	m_nFVF = TEXTURE_VERTEX_FVF;
	m_nFVFSize = sizeof( TG_UVVertex );

    m_fMinZDepth = -9999999.f;

	unsigned long maxMaterialCount = 0;


	//------------------------------------------
	// Store off the Node Names.
	char *nodeName = strstr((char *)aseBuffer,ASE_NODE_NAME);
	if (!nodeName )
    {
        nodeName = strstr((char *)aseBuffer,ASE_GROUP_NAME);
      	nodeName += strlen(ASE_GROUP_NAME)+1;

    }
    else
    {
       	nodeName += strlen(ASE_NODE_NAME)+1;
    }

	_ASSERT(nodeName != NULL);

	char nodeString[1024];
	GetNameData(nodeName,nodeString);

    ASSERT( strlen( nodeString ) < 32 );

	strncpy(m_strNodeId,nodeString,32);
    CharUpperA( m_strNodeId );
	//if ( strstr( m_strNodeId, "Text" ) )
	//	return -1;
	

	//----------------------------------------------------
	// Store off NODE ABS position for heirarchy
	nodeName = strstr((char *)aseBuffer,ASE_NODE_POS);
	_ASSERT(nodeName != NULL);
    if ( !nodeName ) // this is a group
        return -1;


	nodeName += strlen(ASE_NODE_POS)+1;

	char numData[512];

	GetNumberData(nodeName,numData);
	m_nodeCenter.x = (float)atof(numData);
	nodeName += strlen(numData)+1;

	GetNumberData(nodeName,numData);
	m_nodeCenter.y = (float)atof(numData);
	nodeName += strlen(numData)+1;

	GetNumberData(nodeName,numData);
	m_nodeCenter.z = (float)atof(numData);

    //m_nodeCenter += parentLoc;

	//----------------------------------------
	// Find the Number of Vertices
	char *vertexCount = strstr((char *)aseBuffer,ASE_NUM_VERTEX);
    if ( !vertexCount )
        return -1;
	_ASSERT(vertexCount != NULL);
		
	vertexCount += strlen(ASE_NUM_FACE)+1;
	long numTmpVertices = atol(vertexCount);
	if (numTmpVertices == 0)			//NO Data for this node.  WARN and then do NOT allocate anything!
	{
#ifdef _DEBUG
//		if (!silentMode)
//			PAUSE(("WARNING: No Vertices for Shape Node %s in Shape %s!",m_strNodeID,fileName));
#endif
		init();

		return (0);
	}

	//---------------------------------------
	// Find the material index of the object
	//---------------------------------------
    unsigned long nMaterialNum = 0;
	char *matIndex = strstr((char *)aseBuffer,ASE_MATERIAL_REF);

    if ( matIndex )
    {
	    // Pull the ASE specific material index
	    matIndex += strlen(ASE_MATERIAL_REF)+1;
	    nMaterialNum = atol(matIndex);
    }

	// Now find the cooresponding material index in the Material Library
	long nMaterialIndex = g_MaterialLib.ReturnIndex(nMaterialNum);

    // if we don't use textures, don't allocate them
    if ( !g_MaterialLib.MaterialUsesTexture( nMaterialIndex ) )
        return ParseASENoTextures( aseBuffer, fileName, parentLoc );


	//---------------------------------------
	// Find the Number of faces
	//---------------------------------------
	char *faceCount = strstr((char *)aseBuffer,ASE_NUM_FACE);
	_ASSERT(faceCount != NULL);
			
	faceCount += strlen(ASE_NUM_FACE)+1;
	long numFaces = atol(faceCount);
    char* prevFace = (char*)aseBuffer;
	if ( numFaces )
	{

		TG_NormalVertex* pTmpVertices = new TG_NormalVertex[numTmpVertices];
		memset( pTmpVertices, 0xff, numTmpVertices * sizeof( TG_NormalVertex ) );

		// going to store materials, and then sort them later
		long*	tmpMaterialArray = new long[ numFaces ];
		memset( tmpMaterialArray, 0, sizeof ( long ) * numFaces );


		//-------------------
		// Vertex Data Next.

		m_nNumVertices = numFaces * 3;
		
		TG_UVVertex* finalVertices = new TG_UVVertex[m_nNumVertices];
		memset(finalVertices,0xff,sizeof(TG_UVVertex) * m_nNumVertices);

		for (long i=0;i<numTmpVertices;i++)
		{
			char numberData[256];

			//------------------------------------------------
			// First the Vertex Position
			char vertexID[256];
			sprintf(vertexID,"%s% 5d",ASE_MESH_VERTEX_ID,i);

			char *vertexData = strstr((char *)prevFace,vertexID);
			_ASSERT(vertexData != NULL);
            prevFace = vertexData;
				
			vertexData += strlen(vertexID)+1;

			GetNumberData(vertexData,numberData);
			vertexData += strlen(numberData)+1;
			pTmpVertices[i].position.x = (float)atof(numberData);
			pTmpVertices[i].position.x -= m_nodeCenter.x;

			GetNumberData(vertexData,numberData);
			vertexData += strlen(numberData)+1;
			pTmpVertices[i].position.y = (float)atof(numberData);
			pTmpVertices[i].position.y -= m_nodeCenter.y;


			GetNumberData(vertexData,numberData);
			pTmpVertices[i].position.z = (float)atof(numberData);
			pTmpVertices[i].position.z -= m_nodeCenter.z;

            if ( pTmpVertices[i].position.y > m_fMinZDepth )
                m_fMinZDepth = pTmpVertices[i].position.y;


		}


		//-----------------------------------------------
		// Find the number of Texture Vertex List entries
		//-----------------------------------------------
		char *tVertCount = strstr((char *)aseBuffer,ASE_NUM_TVERTEX);
		_ASSERT(tVertCount != NULL);
				
		tVertCount += strlen(ASE_NUM_TVERTEX)+1;
		long numTVertices = atol(tVertCount);

		// Read in the Texture Vertex List
		TG_TVert *TVertexList = new TG_TVert[numTVertices];
		for(i = 0; i < numTVertices; i++)
		{
			char numberData[256];

			char TvertexID[256];
			sprintf(TvertexID,"%s %d",ASE_MESH_TVERT_ID,i);

			char *TvertexData = strstr((char *)tVertCount, TvertexID);
			_ASSERT(TvertexData != NULL);
				
			TvertexData += strlen(TvertexID)+1;

			GetNumberData(TvertexData, numberData);
			TvertexData += strlen(numberData)+1;
			TVertexList[i].u = (float)atof(numberData);

			GetNumberData(TvertexData, numberData);
			TvertexData += strlen(numberData)+1;
			TVertexList[i].v = (float)atof(numberData);
			TVertexList[i].v = 1.0f - TVertexList[i].v;
		}

		//-----------------------------------------------
		// Find the number of Texture Face List entries
		//-----------------------------------------------
		char *tVFaceCount = strstr((char *)aseBuffer,ASE_NUM_TVFACES);
		_ASSERT(tVFaceCount != NULL);
				
		tVFaceCount += strlen(ASE_NUM_TVFACES)+1;
		long numTFaces = atol(tVFaceCount);

		// Read in the Texture Face List
		TG_TFace *TFaceList = new TG_TFace[numTFaces];
        char* prevTextFace = (char*)aseBuffer;
		for(i = 0; i < numTFaces; i++)
		{
			char numberData[256];

			char TfaceID[256];
			sprintf(TfaceID,"%s %d",ASE_MESH_TFACE_ID,i);

			char *TfaceData = strstr((char *)prevTextFace, TfaceID);
            prevTextFace = TfaceData;
			_ASSERT(TfaceData != NULL);
				
			TfaceData += strlen(TfaceID)+1;

			GetNumberData(TfaceData, numberData);
			TfaceData += strlen(numberData)+1;
			TFaceList[i].index0 = atol(numberData);

			GetNumberData(TfaceData, numberData);
			TfaceData += strlen(numberData)+1;
			TFaceList[i].index1 = atol(numberData);

			GetNumberData(TfaceData, numberData);
			TfaceData += strlen(numberData)+1;
			TFaceList[i].index2 = atol(numberData);
		}

        char* prevFace = (char*)aseBuffer;

		for (i=0;i<numFaces;i++)
		{
			char numberData[256];

			//-----------------------------------------------
			// First the Vertices for the face
			//-----------------------------------------------
			char faceId[256];
			sprintf(faceId,"%s% 5d:",ASE_MESH_FACE_ID,i);

			char *faceData = strstr((char *)prevFace,faceId);
            prevFace = faceData;
			_ASSERT(faceData != NULL);

			faceData = strstr(faceData,"A:");
			_ASSERT(faceData != NULL);
				
			GetNumberData(faceData,numberData);
			long index0 = atol(numberData);

			faceData = strstr(faceData,"B:");
			_ASSERT(faceData != NULL);

			GetNumberData(faceData,numberData);
			long index1 = atol(numberData);

			faceData = strstr(faceData,"C:");
			_ASSERT(faceData != NULL);
		
			GetNumberData(faceData,numberData);
			long index2 = atol(numberData);

			faceData = strstr( faceData, "MESH_MTLID " );
			GetNumberData(faceData,numberData);
			long matrlID = atol(numberData );

			//tmpMaterialArray[i] = matrlID;
			tmpMaterialArray[i] = nMaterialNum;
            //if ( matrlID >= maxMaterialCount )
            //    maxMaterialCount = matrlID;

			// set up the vertices
			finalVertices[3*i].position.x = pTmpVertices[index0].position.x;
			finalVertices[3*i].position.y = pTmpVertices[index0].position.y;
			finalVertices[3*i].position.z = pTmpVertices[index0].position.z;

			finalVertices[3*i+1].position.x = pTmpVertices[index1].position.x;
			finalVertices[3*i+1].position.y = pTmpVertices[index1].position.y;
			finalVertices[3*i+1].position.z = pTmpVertices[index1].position.z;

			finalVertices[3*i+2].position.x = pTmpVertices[index2].position.x;
			finalVertices[3*i+2].position.y = pTmpVertices[index2].position.y;
			finalVertices[3*i+2].position.z = pTmpVertices[index2].position.z;


            D3DXVECTOR3 v1, v2, tmpNormal;
            v1 = finalVertices[3*i+2].position - finalVertices[3*i+1].position;
            v2 = finalVertices[3*i].position - finalVertices[3*i+1].position;
            

            D3DXVec3Cross( &tmpNormal, &v2, &v1 );
            D3DXVec3Normalize( &tmpNormal, &tmpNormal );


  
            finalVertices[3*i].normal = tmpNormal;
            finalVertices[3*i+1].normal = tmpNormal;
            finalVertices[3*i+2].normal = tmpNormal;



			// set up the texture coordinates
			finalVertices[3*i].u = TVertexList[TFaceList[i].index0].u;
			finalVertices[3*i].v = TVertexList[TFaceList[i].index0].v;

			finalVertices[3*i+1].u = TVertexList[TFaceList[i].index1].u;
			finalVertices[3*i+1].v = TVertexList[TFaceList[i].index1].v;

			finalVertices[3*i+2].u = TVertexList[TFaceList[i].index2].u;
			finalVertices[3*i+2].v = TVertexList[TFaceList[i].index2].v;

			//------------------------------------------------------------
		}

		// OK, now need to sort the faces based on materials, saves on state changes later
		m_pMaterialIDs = new XBMESH_SUBSET[maxMaterialCount+1];

		BYTE* pVertexMem = (BYTE*)D3D_AllocContiguousMemory( m_nNumVertices * sizeof( TG_UVVertex ), D3DVERTEXBUFFER_ALIGNMENT );

		BYTE* pTmp = NULL;
		TG_UVVertex* pVertices = (TG_UVVertex*)pVertexMem;
		TG_UVVertex*pCurVertex = pVertices;

        char outputStr[256];
		for ( int i = 0; i < numFaces; i++ )
		{	
				*pCurVertex++ = finalVertices[3*i];
				*pCurVertex++ = finalVertices[3*i+1];
				*pCurVertex++ = finalVertices[3*i+2];

     	}
		m_nMaterialCount = 1;
		m_pMaterialIDs[0].m_ID = nMaterialIndex;
		m_pMaterialIDs[0].m_numUsed = numFaces;


        m_VertexBuffer.Data = 0;

        m_VertexBuffer.Register( pVertexMem );
        m_pMem = pVertexMem; // so we free later


		delete [] pTmpVertices;
		delete [] tmpMaterialArray;
		delete [] finalVertices;
        delete [] TVertexList;
        delete [] TFaceList;

        m_nodeCenter -= parentLoc;
	}
	else // no faces
	{
		return -1;
	}

	return(S_OK);
}	

//-------------------------------------------------------------------------------
//Function figures out what format to keep the data in		
//
long TG_TypeShape::LoadTGShapeFromASE (const char* aseContents, const char* fileName, const D3DVECTOR& parentLoc )
{

	// figure out if this has textures or not
	// no textures, use index buffer,
	// textures, use vertex buffer
	//----------------------------------------
	char* pTextureInfo = strstr( aseContents, ASE_NUM_TVERTEX );
	char numberData[32];
	GetNumberData(pTextureInfo,numberData);

	long textureCount = atol( numberData );
	long parseResult = -1;

	// figure out if it uses colors or not
	char* pColorInfo = strstr( aseContents, ASE_NUM_CVERTEX );
	GetNumberData(pColorInfo,numberData);
	long colorCount = atol( numberData );

	if ( textureCount )
	{
		parseResult = ParseASEFile((BYTE*)aseContents,fileName, parentLoc);
//        parseResult = ParseASENoTextures((BYTE*)aseContents,fileName, parentLoc );

	}
	else if ( colorCount )
	{
		parseResult = ParseASEColors( (BYTE*)aseContents,fileName, parentLoc);
	}
	else
	{
		//parseResult = ParseASEFile((BYTE*)aseContents,fileName);
        parseResult = ParseASENoTextures((BYTE*)aseContents,fileName, parentLoc );
	}

	return(parseResult);
}	


//-------------------------------------------------------------------------------
// looks for the existing material, and replaces it with the new one
long TG_TypeShape::SwapMaterials (DWORD indexToReplace, DWORD replaceItWithThis)
{

    for ( unsigned long i = 0; i < m_nMaterialCount; i++ )
    {
        if ( m_pMaterialIDs[i].m_ID == indexToReplace )
        {
            m_pMaterialIDs[i].m_ID = replaceItWithThis;
            return S_OK;
        }
    }

	return -1;
}	

//-------------------------------------------------------------------------------
// just sets the new material
void TG_TypeShape::SetMaterials (DWORD replaceItWithThis)
{

    for ( unsigned long i = 0; i < m_nMaterialCount; i++ )
    {
        m_pMaterialIDs[i].m_ID = replaceItWithThis;
    }

}	


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
// initialization
void TG_TypeShape::init (void)
{
	m_nodeCenter.x = m_nodeCenter.y = m_nodeCenter.z = 0.0f;
	m_pMaterialIDs = NULL;
	m_pIndexBuffer = NULL;			// not necessarily used
	m_nNumFaces = 0;
    m_nNumVertices = 0;
    m_pMem = 0;
    m_fMinZDepth = 0;           // also for ase parsing...

    m_nMaterialCount = 0;

    memset( &m_VertexBuffer, 0, sizeof( m_VertexBuffer ) );
    m_VertexBuffer.Common =  D3DCOMMON_TYPE_VERTEXBUFFER |    // Type
            D3DCOMMON_VIDEOMEMORY |          // VB lives in video memory
            1;                               // Initial Refcount  

}

extern CMaterialLibrary g_MaterialLib;
static temp = 0;

HRESULT TG_TypeShape::Render()
{
	if ( m_nNumVertices < 1 )
		return -1;

	
    g_pd3dDevice->SetStreamSource( 0, &m_VertexBuffer, m_nFVFSize );
	g_pd3dDevice->SetIndices( m_pIndexBuffer, 0 ); 

	int curOffset = 0;
	for ( DWORD j = 0; j < m_nMaterialCount; j++ )
	{
		if ( m_pMaterialIDs[j].m_numUsed )
		{			
			if(FAILED(g_MaterialLib.Setup( m_pMaterialIDs[j].m_ID, m_nFVF )))
				return E_FAIL;

			if ( m_pIndexBuffer )
			{
				g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 
					m_nNumVertices, curOffset*3, m_pMaterialIDs[j].m_numUsed );
			}
			else
			{
               g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, curOffset*3, m_pMaterialIDs[j].m_numUsed );
			}
			curOffset += m_pMaterialIDs[j].m_numUsed;
		}
	}

    return S_OK;
}


//-------------------------------------------------------------------------------

TG_Shape::TG_Shape( TG_Shape& src )
{
    m_pMyType       = NULL;
    m_transform     = src.m_transform;
    m_bRender       = src.m_bRender;                // turn off things like text regions
    m_bCommonMemory = false;

    m_pChild = m_pParent = m_pNext = NULL;

    if ( src.m_pMyType )
        m_pMyType = src.m_pMyType->Copy();			//Pointer to the instance of the shape.
    

    if ( src.m_pChild )
        m_pChild = new TG_Shape( *src.m_pChild );

    if ( src.m_pNext )
        m_pNext = new TG_Shape( *src.m_pNext); 

}

static long childCounter = 0;
//-------------------------------------------------------------------------------
void TG_Shape::Dump(bool bAllData)
{
	if ( m_pMyType )
	{
		DbgPrint( "TG_Shape:: Node Name = %s\nChildren=\n", m_pMyType->m_strNodeId );
	}

	// render all children
    TG_Shape* pShape = m_pChild;
    childCounter++;

    while( pShape )
    {   
        for ( int i = 0; i < childCounter; i++ )
		    DbgPrint( "\t" );
        pShape->Dump();
        pShape = pShape->m_pNext;
    }
    childCounter--;
}

//-------------------------------------------------------------------------------
long TG_Shape::Render ( )
{
		

    if ( !m_bRender )
        return 0; // nothing to render, not really a problem

	TG_TypeNode* theShape = m_pMyType;
        
	D3DXMATRIX curWorld;
	g_pd3dDevice->GetTransform( D3DTS_WORLD, &curWorld );

	D3DXMATRIX finalWorld;
	D3DXMatrixMultiply( &finalWorld, &curWorld, &m_transform );
   
    D3DXMATRIX scaleMatrix;
    D3DXMatrixScaling( &scaleMatrix, m_Scale.x, m_Scale.y, m_Scale.z );

    D3DXMatrixMultiply( &finalWorld, &scaleMatrix, &finalWorld );

   

	g_pd3dDevice->SetTransform( D3DTS_WORLD, &finalWorld );
    
  	if ( m_pMyType != NULL )
	    m_pMyType->Render(); 

    // render all children
    TG_Shape* pShape = m_pChild;
    while( pShape )
    {
        pShape->Render( );
        pShape = pShape->m_pNext;
    }

 
    // clean up after ourselves
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &curWorld );




	// otherwise do this

	return 0;


}

HRESULT TG_Shape::FrameMove (float frameLength)
{
    HRESULT hr = S_OK;
    // are we animating?
    if ( m_Animation.m_numFrames )
    {
         m_curTime += frameLength;
     
         float totalLen = (m_Animation.m_numFrames/m_Animation.m_frameRate );
         while ( m_curTime > totalLen  )
                m_curTime -= totalLen;

         DWORD curFrame = (DWORD)(m_curTime * (float)m_Animation.m_frameRate);
  
 
        // build current animation position & rotation info
        D3DXMATRIX anim;
        D3DXMatrixTransformation( &m_transform, NULL, NULL, NULL, NULL, 
            m_Animation.m_pQuat ? &m_Animation.m_pQuat[curFrame] : 0,
            m_Animation.m_pPos ? &m_Animation.m_pPos[curFrame]  : &m_pMyType->m_nodeCenter );

        // TO DO: return different hr if anim is over
    }

    // render all children
    TG_Shape* pShape = m_pChild;
    while( pShape )
    {
        pShape->FrameMove( frameLength );
        pShape = pShape->m_pNext;
    }

    return hr;
}

////////////////////////////////////////////////////////////////
// this is load from an ASE, need to write a load from Binary
long TG_Shape::Load(const char* buffer, const char* path, const D3DVECTOR& parentLoc, TG_Animation* pAnimationHeader )
{
	if ( NULL != m_pMyType )
	{
		m_pMyType->destroy();
	}

	if ( NULL == m_pMyType )
		m_pMyType = new TG_TypeShape;

    m_bCommonMemory = false;

    // I know I can do this cast because I just allocated it a few lines up

	long retVal = ((TG_TypeShape*)m_pMyType)->LoadTGShapeFromASE(buffer, path, parentLoc );

	D3DXMATRIX translation;
	D3DXMatrixTranslation( &m_transform, m_pMyType->m_nodeCenter.x, m_pMyType->m_nodeCenter.y, m_pMyType->m_nodeCenter.z );

    // easily could fail if no animation info
    if ( pAnimationHeader )
    {
        long animRetVal = 
            LoadAnimationFromASE( (char*)buffer, m_pMyType->m_strNodeId, m_pMyType->m_nodeCenter, pAnimationHeader, &m_Animation );

        if ( retVal == -1 ) // you can have animation info without mesh info and vice versa
            retVal = animRetVal;
    }

  	return retVal;



}

//////////////////////////////////////////////////////////////
// explicitly set a transform
// 
void TG_Shape::SetTransform( const D3DXMATRIX& newTransform )
{
	m_transform = newTransform;
}

//-------------------------------------------------------------------------------
void TG_Shape::LocalRotateY( float angle )
{
	D3DXMATRIX rotationMat;
	D3DXMatrixRotationY( &rotationMat, angle );
    D3DXMatrixMultiply( &m_transform, &m_transform, &rotationMat );
}

//-------------------------------------------------------------------------------
void TG_Shape::LocalRotateX( float angle )
{
	D3DXMATRIX rotationMat;
	D3DXMatrixRotationX( &rotationMat, angle );
    D3DXMatrixMultiply( &m_transform, &m_transform, &rotationMat );
}


//-------------------------------------------------------------------------------
void TG_Shape::LocalRotateZ( float angle )
{
	D3DXMATRIX rotationMat;
	D3DXMatrixRotationZ( &rotationMat, angle );
    D3DXMatrixMultiply( &m_transform, &rotationMat, &m_transform );
	
}

//-------------------------------------------------------------------------------
void        TG_Shape::SetScale( float newX, float newY, float newZ )
{
    m_Scale.x = newX;
    m_Scale.y = newY;
    m_Scale.z = newZ;
}
//////////////////////////////////////////////////////////////
// add a child node to this object, it'll go through the 
// root node's transform
HRESULT TG_Shape::AddChild( TG_Shape* pChild )
{
	if (!m_pChild )
	{
		// if we're loading an ASE, this is OK, if not
        // we're in trouble
        m_pChild = pChild;
        pChild->m_pParent = this;
        pChild->m_dwEnumeratedID = 0;

	}
    else
    {
        unsigned long dwCounter = 0;
        TG_Shape* pKid = m_pChild;
        while( pKid->m_pNext )
        {
            pKid = pKid->m_pNext;
            dwCounter++;
        }
        pKid->m_pNext = pChild;
        pChild->m_pNext = NULL;
  
        pChild->m_pParent = this;
        pChild->m_dwEnumeratedID = dwCounter;
    }

    return S_OK;

	
}

//////////////////////////////////////////////////////////////
// set the name of this object
//////////////////////////////////////////////////////////////
void  TG_Shape::SetNodeName(const char* pNewName )
{
    ASSERT( strlen( pNewName ) <= 32 );
    if ( m_pMyType )
    {
        strcpy( m_pMyType->m_strNodeId, pNewName );
        CharUpperA( m_pMyType->m_strNodeId );
    }
    else
    {
        m_pMyType = new TG_TypeNode; // needs to have a name
        strcpy( m_pMyType->m_strNodeId, pNewName );  
    }

}

	
//////////////////////////////////////////////////////////////
// get the name of this object
//////////////////////////////////////////////////////////////
char * TG_Shape::GetNodeName (void) const
{
	if ( m_pMyType != NULL )
	{
		return m_pMyType->getNodeId();
	}
	return NULL;
}

// search for subobject with this name
TG_Shape*    TG_Shape::FindObject( const char* pName )
{
    TG_Shape* pRetVal = NULL;
    if ( m_pMyType )
    {
        if ( _stricmp( m_pMyType->getNodeId(), pName ) == 0 )
            pRetVal = this;
    }
    if ( !pRetVal )
    {
        TG_Shape* pKid = m_pChild;   
       while( pKid && !pRetVal )
       {
           pRetVal = pKid->FindObject( pName );
           pKid = pKid->m_pNext;
       }
    }

    return pRetVal;

}
//-----------------------------------------------------------------------------
// Name: ComputeMemoryRequirementsCB()
// Desc: Frame enumeration callback to compute memory requirements
//-----------------------------------------------------------------------------
BOOL TG_Shape::ComputeMemoryRequirementsCB( TG_Shape* pFrame, VOID* )
{
    pFrame->m_dwFrameID = s_dwNumFrames;
    s_dwNumFrames++;

    // Compute memory requirements
    s_dwFrameSpace    += sizeof(XBMESH_FRAME);

    if ( pFrame->m_pMyType && pFrame->m_pMyType->isShape() )
    {
        TG_TypeShape* pTypeShape = (TG_TypeShape*)pFrame->m_pMyType;
        
        s_dwSubsetSpace   += sizeof(XBMESH_SUBSET) * pTypeShape->m_nMaterialCount;
        s_dwIndicesSpace  += sizeof(WORD) * pTypeShape->m_nNumFaces * 3;
        s_dwVerticesSpace += pTypeShape->m_nFVFSize * pTypeShape->m_nNumVertices;
        if ( pFrame->m_Animation.m_pPos )
            s_dwAnimSpace += pFrame->m_Animation.m_numFrames * sizeof( D3DXVECTOR3 );

        if ( pFrame->m_Animation.m_pQuat )
            s_dwAnimSpace += pFrame->m_Animation.m_numFrames * sizeof( D3DXQUATERNION );
    }  

    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: WriteMeshInfoCB()
// Desc: Writes mesh info to a file
//-----------------------------------------------------------------------------
BOOL TG_Shape::WriteMeshInfoCB( TG_Shape* pFrame, VOID* pData )
{
    File* file = (File*)pData;

    // Set up mesh info to be written. Note that, in order for Xbox fast math
	// (via xgmath.h) to work, all D3DXMATRIX's must be 16-byte aligned.
    XBMESH_FRAME frame;
    memset( &frame, 0, sizeof( XBMESH_FRAME ) );
    frame.m_pChild       = NULL;
    frame.m_pNext        = NULL;
    frame.m_MeshData.m_VB.Common     = 1 | D3DCOMMON_TYPE_VERTEXBUFFER;
    frame.m_MeshData.m_VB.Data       = 0L;
//    frame.m_MeshData.m_VB.Lock       = 0L;
    frame.m_MeshData.m_dwNumVertices = 0;
    frame.m_MeshData.m_IB.Common     = 1 | D3DCOMMON_TYPE_INDEXBUFFER;
    frame.m_MeshData.m_IB.Data       = 0L;
//    frame.m_MeshData.m_IB.Lock       = 0L;
    frame.m_MeshData.m_dwNumIndices  = 0L;
    frame.m_MeshData.m_dwFVF         = 0;
    frame.m_MeshData.m_dwVertexSize  = 0;
    frame.m_MeshData.m_dwNumSubsets  = 0;
    frame.m_MeshData.m_pSubsets      = NULL;
    frame.m_MeshData.m_numFrames    = pFrame->m_Animation.m_numFrames;
    frame.m_MeshData.m_frameRate    = pFrame->m_Animation.m_frameRate;
    frame.m_MeshData.m_tickRate     = pFrame->m_Animation.m_tickRate;
    frame.m_transform = pFrame->m_transform;

    if ( pFrame->m_pMyType )
    {
        strcpy( frame.m_strName, pFrame->m_pMyType->m_strNodeId );

        frame.m_translation = pFrame->m_pMyType->m_nodeCenter;


        if (pFrame->m_pMyType->isShape() )
        {
            TG_TypeShape* pShape =    (TG_TypeShape*)pFrame->m_pMyType;
            frame.m_MeshData.m_dwNumVertices = pShape->m_nNumVertices;
            frame.m_MeshData.m_dwNumIndices  = pShape->m_nNumFaces*3;
            frame.m_MeshData.m_dwFVF         = pShape->m_nFVF;
            frame.m_MeshData.m_dwVertexSize  = pShape->m_nFVFSize;
            frame.m_MeshData.m_dwPrimType    = D3DPT_TRIANGLELIST;
            frame.m_MeshData.m_dwNumSubsets  = pShape->m_nMaterialCount;
            frame.m_MeshData.m_pSubsets      = NULL;
        }
    }

	DWORD size = sizeof(frame);


    // Write pointers as file offsets
    if( pFrame->m_pChild )  
    {
       s_dwMeshFileOffset = sizeof( XBMESH_FRAME )* (pFrame->m_pChild->m_dwFrameID);
       frame.m_pChild = (XBMESH_FRAME*)( s_dwMeshFileOffset );
    }
    if( pFrame->m_pNext )   
    {
       s_dwMeshFileOffset = sizeof( XBMESH_FRAME )* (pFrame->m_pNext->m_dwFrameID);
        frame.m_pNext  = (XBMESH_FRAME*)( s_dwMeshFileOffset );
    }
    if ( pFrame->m_pMyType && pFrame->m_pMyType->isShape())
    {
        TG_TypeShape* pType = (TG_TypeShape*)(pFrame->m_pMyType);
        if( pType->m_nMaterialCount )
            frame.m_MeshData.m_pSubsets = (XBMESH_SUBSET*)s_dwSubsetFileOffset;
        if( frame.m_MeshData.m_dwNumIndices )
            frame.m_MeshData.m_IB.Data  = (DWORD)s_dwIndicesFileOffset;
        if( frame.m_MeshData.m_dwNumVertices )
            frame.m_MeshData.m_VB.Data  = (DWORD)s_dwVerticesFileOffset;

        s_dwSubsetFileOffset   += sizeof(XBMESH_SUBSET) * pType->m_nMaterialCount;
        s_dwIndicesFileOffset  += sizeof(WORD) * pType->m_nNumFaces * 3;
        s_dwVerticesFileOffset += pType->m_nFVFSize * pType->m_nNumVertices;

        if ( pFrame->m_Animation.m_pPos )
        {
            frame.m_MeshData.m_pPos = (D3DXVECTOR3*)s_dwAnimFileOffset;
            s_dwAnimFileOffset += pFrame->m_Animation.m_numFrames * sizeof( D3DXVECTOR3 );
        }
    
        if ( pFrame->m_Animation.m_pQuat )
        {
            frame.m_MeshData.m_pQuats = (D3DXQUATERNION*)s_dwAnimFileOffset;
            s_dwAnimFileOffset += pFrame->m_Animation.m_numFrames * sizeof( D3DXQUATERNION );
        }
        

    }


    // Write out mesh info
    file->write( (BYTE*)&frame,sizeof(XBMESH_FRAME) ); 

    return TRUE;
}


    

//-----------------------------------------------------------------------------
// Name: WriteSubsetsCB()
// Desc: Write out the mesh subsets
//-----------------------------------------------------------------------------
BOOL TG_Shape::WriteSubsetsCB( TG_Shape* pFrame, VOID* pData )
{
    File* file = (File*)pData;

    VOID* pTexturePtr = NULL;

    if ( pFrame->m_pMyType && pFrame->m_pMyType->isShape() )
    {
        TG_TypeShape* pShape =    (TG_TypeShape*)pFrame->m_pMyType;


        for( DWORD i=0; i<pShape->m_nMaterialCount; i++ )
        {
            file->write( (BYTE*)&pShape->m_pMaterialIDs[i], sizeof(XBMESH_SUBSET) );
        }
    }

    return TRUE;
}


    

//-----------------------------------------------------------------------------
// Name: WriteIndicesCB()
// Desc: Write out the mesh indices
//-----------------------------------------------------------------------------
BOOL TG_Shape::WriteIndicesCB( TG_Shape* pFrame, VOID* pData )
{
    File* file = (File*)pData;

    if ( pFrame->m_pMyType && pFrame->m_pMyType->isShape() )
    {
        TG_TypeShape* pShape =    (TG_TypeShape*)pFrame->m_pMyType;
        
        if( pShape->m_pIndexBuffer )
        {
            BYTE* pIndexData;
            pShape->m_pIndexBuffer->Lock( 0, 0, &pIndexData, 0 );
            file->write( pIndexData, sizeof(WORD) * pShape->m_nNumFaces * 3 ); 
            pShape->m_pIndexBuffer->Unlock();
        }
     }

    return TRUE;
}

//-----------------------------------------------------------------------------
// Name: WriteIndicesCB()
// Desc: Write out the mesh indices
//-----------------------------------------------------------------------------
BOOL TG_Shape::WriteAnimationCB( TG_Shape* pFrame, VOID* pData )
{
    File* file = (File*)pData;

    if ( pFrame->m_Animation.m_pPos )
    {
       file->write( (BYTE*)pFrame->m_Animation.m_pPos, pFrame->m_Animation.m_numFrames * sizeof( D3DXVECTOR3 ) );
    }

    if ( pFrame->m_Animation.m_pQuat )
    {
       file->write( (BYTE*)pFrame->m_Animation.m_pQuat, pFrame->m_Animation.m_numFrames * sizeof( D3DXQUATERNION ) );
    }
 
   
   return TRUE;
}


    


    

//-----------------------------------------------------------------------------
// Name: WriteVerticesCB()
// Desc: Write out the mesh vertices
//-----------------------------------------------------------------------------
BOOL TG_Shape::WriteVerticesCB( TG_Shape* pFrame, VOID* pData )
{
    File* file = (File*)pData;

    if ( pFrame->m_pMyType && pFrame->m_pMyType->isShape() )
    {
        TG_TypeShape* pShape =    (TG_TypeShape*)pFrame->m_pMyType;
        if( pShape->m_nNumVertices )
        {
            BYTE* pVertexData;
            pShape->m_VertexBuffer.Lock( 0, 0, &pVertexData, 0 );
            file->write( pVertexData, pShape->m_nFVFSize * pShape->m_nNumVertices ); 
            pShape->m_VertexBuffer.Unlock();
        }
    }

    return TRUE;
}






//-----------------------------------------------------------------------------
// Name: EnumFrames()
// Desc: Called recursively to walk the frame hierarchy, calling a user 
//       supplied callback function for every frame.
//-----------------------------------------------------------------------------
BOOL TG_Shape::EnumFrames( BOOL (*EnumFramesCB)(TG_Shape*,VOID*),
                            VOID* pData )
{
    EnumFramesCB( this, pData );

    if( m_pChild )
        m_pChild->EnumFrames( EnumFramesCB, pData );
    
    if( m_pNext )
        m_pNext->EnumFrames( EnumFramesCB, pData );

    return TRUE;
}



//-----------------------------------------------------------------------------
// Name: CD3DFrame()
// Desc: Construct a new CD3DFrame from the data in a XBMESH_FRAME structure.
//-----------------------------------------------------------------------------
TG_Shape::TG_Shape( LPDIRECT3DDEVICE8 pd3dDevice, TG_Shape* pParent, 
                      XBMESH_FRAME* pFrameData, void* pVBData )
{
    init();
    m_pParent         = pParent;
    m_pChild          = NULL;
    m_pNext           = NULL;

    m_Animation.m_numFrames = pFrameData->m_MeshData.m_numFrames;
    m_Animation.m_firstFrame = 0;
    m_Animation.m_frameRate = pFrameData->m_MeshData.m_frameRate;
    m_Animation.m_tickRate = pFrameData->m_MeshData.m_tickRate;
    m_Animation.m_pPos = pFrameData->m_MeshData.m_pPos;
    m_Animation.m_pQuat = pFrameData->m_MeshData.m_pQuats;

    m_bCommonMemory = true;

     
    
    // Copy info from XBMESH_FRAME structure
    m_transform = pFrameData->m_transform;
  
    if ( pFrameData->m_MeshData.m_dwNumVertices || pFrameData->m_MeshData.m_dwNumIndices )
    {
        m_pMyType = new TG_TypeShape( pFrameData, pVBData );
    }
    else
    {
        m_pMyType = new TG_TypeNode( pFrameData );
    }

    m_pMyType->m_nodeCenter = pFrameData->m_translation;


    // Create the child frame
    if( pFrameData->m_pChild )
        m_pChild = new TG_Shape( pd3dDevice, this, pFrameData->m_pChild, pVBData );

    // Create the sibling frame
    if( pFrameData->m_pNext )
        m_pNext = new TG_Shape( pd3dDevice, pParent, pFrameData->m_pNext, pVBData );


}

TG_TypeShape::TG_TypeShape( XBMESH_FRAME* pFrameData, void* pVBData )
{
    init();

    // Frame info
    strcpy( m_strNodeId, pFrameData->m_strName );

    // Mesh info
    m_nodeCenter          = D3DXVECTOR3(0,0,0);

    m_nNumVertices           = pFrameData->m_MeshData.m_dwNumVertices;
    m_pIndexBuffer           = NULL;
    m_nNumFaces              = pFrameData->m_MeshData.m_dwNumIndices/3;
    m_nFVF                   = pFrameData->m_MeshData.m_dwFVF;
    m_nFVFSize               = pFrameData->m_MeshData.m_dwVertexSize;
    m_nMaterialCount         = pFrameData->m_MeshData.m_dwNumSubsets;
    m_pMaterialIDs           = NULL;

    // Convet the primitive type
  //  if( pFrameData->m_MeshData.m_dwPrimType == (D3DPRIMITIVETYPE)5 )
  //      m_dwMeshPrimType = D3DPT_TRIANGLELIST;
  //  else
  //      m_dwMeshPrimType = D3DPT_TRIANGLESTRIP;


    // Create the vertex buffer
    if( m_nNumVertices )
    {
      
        m_VertexBuffer = pFrameData->m_MeshData.m_VB;
        m_VertexBuffer.Register( pVBData );
    }
        

    // Create the index buffer
    if( m_nNumFaces )
    {
        g_pd3dDevice->CreateIndexBuffer( m_nNumFaces * 3 *sizeof(WORD), 
                                       D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, 
                                       &m_pIndexBuffer );
        WORD* pIndices;
        m_pIndexBuffer->Lock( 0, 0, (BYTE**)&pIndices, 0 );
        memcpy( pIndices, (VOID*)pFrameData->m_MeshData.m_IB.Data, m_nNumFaces * 3*sizeof(WORD) );
        m_pIndexBuffer->Unlock();
    }

    // Create the subsets
    if( m_nMaterialCount )
    {
        m_pMaterialIDs = new XBMESH_SUBSET[m_nMaterialCount];
        memcpy( m_pMaterialIDs, pFrameData->m_MeshData.m_pSubsets, m_nMaterialCount*sizeof(XBMESH_SUBSET) );

    }



}



//-----------------------------------------------------------------------------
// Name: WriteToXBG()
// Desc: Writes the geometry objects to a file
//-----------------------------------------------------------------------------
HRESULT TG_Shape::SaveBinary( File* file  )
{
    s_dwNumFrames = 0;

    // Before writing the file, walk the nodes to compute memory requirements
    s_dwFrameSpace    = 0;
    s_dwSubsetSpace   = 0;
    s_dwIndicesSpace  = 0;
    s_dwVerticesSpace = 0;
    s_dwAnimSpace = 0;
    EnumFrames( ComputeMemoryRequirementsCB, NULL );

    // As parts of the file are written, these global file offset variables
    // are used to convert object pointers to file offsets
    s_dwMeshFileOffset     = sizeof(XBG_HEADER);
    s_dwSubsetFileOffset   = s_dwFrameSpace;
    s_dwIndicesFileOffset  = s_dwSubsetFileOffset + s_dwSubsetSpace;
    s_dwVerticesFileOffset = 0;
    s_dwAnimFileOffset = sizeof(XBG_HEADER);


    // Setup the file header
    XBG_HEADER xbgHeader;
    xbgHeader.dwMagic         = XBG_FILE_ID;
    xbgHeader.dwNumMeshFrames = s_dwNumFrames;
    xbgHeader.dwSysMemSize    = s_dwFrameSpace + s_dwSubsetSpace + s_dwIndicesSpace;
    xbgHeader.dwVidMemSize    = s_dwVerticesSpace;
    xbgHeader.dwAnimMemSize    = s_dwAnimSpace;


    // Write out the header
    file->write( (BYTE*)&xbgHeader, sizeof(XBG_HEADER) ); 

    // Write the mesh's parts. Note that, starting at this file offset, in
	// order for Xbox fast math (via xgmath.h) to work, all D3DXMATRIX's must
	// be 16-byte aligned.
    EnumFrames( WriteMeshInfoCB, file );
    EnumFrames( WriteSubsetsCB,  file );
    EnumFrames( WriteIndicesCB,  file );
    EnumFrames( WriteVerticesCB, file );
    EnumFrames( WriteAnimationCB, file );

    
    return S_OK;
}

//-----------------------------------------------------------------------------
TG_Shape*   TG_Shape::GetFirstChild()
{
    return m_pChild;
}


//-----------------------------------------------------------------------------
TG_Shape*   TG_Shape::GetNextChild(TG_Shape* pLast)
{
    ASSERT( pLast );
    if ( pLast )
        return pLast->m_pNext;
    return NULL;
}

//-----------------------------------------------------------------------------
void        TG_Shape::GetRectDims( float& width, float& height )
{
    width = 0;
    height = 0;
    m_pMyType->GetRectDims( width, height );
}

//-----------------------------------------------------------------------------
void        TG_Shape::GetRectDimsAndRecenter(float& width, float& height)
{
    float depth = 0.f;
    m_pMyType->GetRectDimsAndRecenter( width, height, depth );
	D3DXMatrixTranslation( &m_transform, m_pMyType->m_nodeCenter.x, m_pMyType->m_nodeCenter.y, m_pMyType->m_nodeCenter.z );

}

//-----------------------------------------------------------------------------
void TG_Shape::GetRectDimsRecenterBuildRotation(float& width, float& height)
{
    float depth = 0.f;
    m_pMyType->GetRectDimsAndRecenter( width, height, depth );
	D3DXMatrixTranslation( &m_transform, m_pMyType->m_nodeCenter.x, m_pMyType->m_nodeCenter.y, m_pMyType->m_nodeCenter.z );
    
    float theta = 0.f;
    if ( width )
    {
        theta = (float)atan(depth/width);
    }
    D3DXMATRIX rotMat;
    D3DXMatrixRotationZ( &rotMat, -theta  );

    D3DXMatrixMultiply( &m_transform, &rotMat, &m_transform );

}



//-----------------------------------------------------------------------------

void        TG_Shape::GetLocalTransform( D3DXMATRIX& transform )
{
    transform = m_transform;
}


//-----------------------------------------------------------------------------
void        TG_Shape::GetWorldTransform( D3DXMATRIX& transform )
{
    // OK, need to multiply up the parent change
    D3DXMATRIX worldTransform;

    TG_Shape* pParent = m_pParent;
    if ( pParent )
    {
        D3DXMATRIX parentTransform;
        pParent->GetWorldTransform(parentTransform);
	    D3DXMatrixMultiply( &transform, &parentTransform, &m_transform );

    }
    else
    {
        transform = m_transform;
    }
}

//-----------------------------------------------------------------------------
void        TG_Shape::GetNodeCenter( float& x, float& y, float& z )
{
    if ( m_pMyType )
    {
        x = m_pMyType->m_nodeCenter.x;
        y = m_pMyType->m_nodeCenter.y;
        z = m_pMyType->m_nodeCenter.z;
    }
    else
    {
        x = y = z = 0.f;
    }
}

//-----------------------------------------------------------------------------
void        TG_Shape::SwapMaterials( long oldMaterial, long newMaterial )
{
    if ( m_pMyType )
    {
        m_pMyType->SwapMaterials( oldMaterial, newMaterial );
    }
}

//-----------------------------------------------------------------------------
void        TG_Shape::SetMaterials(long newMaterial )
{
    if ( m_pMyType )
    {
        m_pMyType->SetMaterials( newMaterial );
    }
}

//-----------------------------------------------------------------------------
void       TG_Shape::GetZVals(float& numberOfVertices, float& zVal)
{
   D3DVECTOR v = m_pMyType->GetNodeCenter(  );
   zVal = v.y;
   zVal += m_pMyType->GetMinZ();
   
   float VCount = (float)m_pMyType->GetVertexCount();
   numberOfVertices = VCount;
}

//-----------------------------------------------------------------------------
void       TG_Shape::GetMinZ(float& curCount, float& z)
{
    float tmpZ;
    float tmpCount;
    GetZVals( tmpCount, tmpZ );


    if ( tmpCount )
    {
        if ( tmpZ > z )
            z = tmpZ;

        curCount += tmpCount;

        TG_Shape* pKid = m_pChild;
        while( pKid )
        {
            pKid->GetMinZ( curCount, z );
            pKid = pKid->m_pNext;
        }
    }

}

//-----------------------------------------------------------------------------
float TG_Shape::GetMinZ( )
{
    float vertexCount = 0;
    float zVal = -999999999.f;

    GetMinZ( vertexCount, zVal );

    return zVal;
}

//-----------------------------------------------------------------------------
void        TG_Shape::SortShapesZ( )
{
    
    TG_Shape* pPrev = NULL;

    TG_Shape* pKid = m_pChild;
      // first sort by z
    while( pKid )
    {
       pKid->SortShapesZ();
       pKid = pKid->m_pNext;
    }

    pKid = m_pChild;
    pPrev = NULL;
    while( pKid )
    {
        float kidZ = pKid->GetMinZ(); 
        TG_Shape* pCompare = m_pChild;
        TG_Shape* pCompPrev = NULL;
        while( pCompare && pCompare != pKid )
        {
            float compZ = pCompare->GetMinZ();
            if ( compZ < kidZ)
            {
                ASSERT( pPrev );

                // remove this kid frm the list
                pPrev->m_pNext = pKid->m_pNext;
                pKid->m_pNext = pCompare;

                
                // insert it again before kid to compare
                if ( pCompPrev )
                {
                    pCompPrev->m_pNext = pKid;
                }
                else
                {
                    m_pChild = pKid;
                }

                if ( pCompPrev )
                    pKid = pCompPrev; // so when we move the pointer forward, we're on pKid again...
                break;
            }

            pCompPrev = pCompare;
            pCompare = pCompare->m_pNext;

        }
        pPrev = pKid;
        pKid = pKid->m_pNext;
       
    }


}

//-----------------------------------------------------------------------------
void        TG_Shape::SortShapesName( const char** strNames, long nameCount )
{

    TG_Shape* pKid = m_pChild;
    while( pKid )
    {
        pKid->SortShapesName( strNames, nameCount );
        pKid = pKid->m_pNext;
    }

 
    TG_Shape* pFirstChild = m_pChild;
    TG_Shape* pPrev = NULL;
    m_pChild = NULL;

    for ( int i = nameCount-1; i > -1; i-- )
    {
        pKid = pFirstChild;
        pPrev = NULL;
        while( pKid )
        {
            const char* pName = pKid->GetNodeName();
            CharUpperA( (char*)pName );

            if ( strstr( pName, strNames[i] ) )
            {
                 // take out of the temp list;
                if ( pPrev )
                {
                    pPrev->m_pNext = pKid->m_pNext;
                }
                else
                {
                    pFirstChild = pKid->m_pNext;
                }

                TG_Shape* pAdded = pKid;
                pKid = pKid->m_pNext;

                // put in the final
                pAdded->m_pNext = m_pChild;
                m_pChild = pAdded; 

            }
            else

            {
                pPrev = pKid; 
                pKid = pKid->m_pNext;
            }

        }
    }

    // insert untagged ones before the rest...
    if ( pFirstChild )
    {
        TG_Shape* pCurFirst = m_pChild;
        m_pChild = pFirstChild;

        TG_Shape* pCurLast = m_pChild;
        while( pCurLast->m_pNext )
        {
            pCurLast = pCurLast->m_pNext;
        }
        pCurLast->m_pNext = pCurFirst;
    }
   
}

//-----------------------------------------------------------------------------
void        TG_Shape::SortShapes( const char** strNames, long nameCount )
{
    SortShapesZ();
    SortShapesName( strNames, nameCount );  
}

long TG_Shape::ParseAnimationHeader( char* aseContents, TG_Animation* pAnim )
{
  		//------------------------------------------
		// Get first frame of animation from header
		long firstFrame, lastFrame;
		char *frameId = strstr((char *)aseContents,ASE_ANIM_FIRST_FRAME);

        if ( !frameId )
        {
            // no big deal, this file doesn't have animation info
            return E_FAIL;
        }
		ASSERT(frameId != NULL);
		frameId += strlen(ASE_ANIM_FIRST_FRAME)+1;
	
		char numData[512];
		GetNumberData(frameId,numData);
		firstFrame = atol(numData);
	
		frameId = strstr((char *)aseContents,ASE_ANIM_LAST_FRAME);
		ASSERT(frameId != NULL);
	
		frameId += strlen(ASE_ANIM_LAST_FRAME)+1;
	
		GetNumberData(frameId,numData);
		lastFrame = atol(numData);
	
		//ASSERT(firstFrame == 0);
		ASSERT(firstFrame <= lastFrame);
	
		//if (firstFrame == lastFrame)
			//No Animation data at all. Possible?

        pAnim->m_firstFrame = firstFrame;
	
		pAnim->m_numFrames = (lastFrame - firstFrame) + 1;
	
		frameId = strstr((char *)aseContents,ASE_ANIM_FRAME_SPEED);
		ASSERT(frameId != NULL);
	
		frameId += strlen(ASE_ANIM_FRAME_SPEED)+1;
	
		GetNumberData(frameId,numData);
		pAnim->m_frameRate = (float)atof(numData);
	
		frameId = strstr((char *)aseContents,ASE_ANIM_TICKS_FRAME);
		ASSERT(frameId != NULL);
	
		frameId += strlen(ASE_ANIM_TICKS_FRAME)+1;
	
		GetNumberData(frameId,numData);
		pAnim->m_tickRate = (float)atof(numData);
	
    return S_OK;
}

//-------------------------------------------------------------------------------
void GetNextLine (char *rawData, char *result)
{
	long startIndex = 0;
	long endIndex = 0;
	while (	(rawData[startIndex] != '\n') )
	{
		startIndex++;
	}

	startIndex++;
	endIndex = startIndex;
	while (	(rawData[endIndex] != '\n') )
	{
		endIndex++;
	}

	strncpy(result,&rawData[startIndex],endIndex - startIndex);
	result[endIndex-startIndex] = 0;
}

long TG_Shape::LoadAnimationFromASE (char *aseContents, const char* pNodeName, 
                                     D3DXVECTOR3 nodeCenter, TG_Animation* pHeader, TG_Animation* pOneToFill )
{
    long actualCount = 0;

    long retVal = -1;
	
	char *animScan = (char *)aseContents;
	animScan = strstr(animScan,ASE_ANIMATION);

	char nodeName[512];
	sprintf(nodeName,"*NODE_NAME \"%s\"", pNodeName );

    bool countUp = false;
        
	while (animScan != NULL)
	{
		animScan += strlen(ASE_ANIMATION)+1;

		//------------------------------------------------------
		// We found a TM_ANIMATION Section.
		// Check if the VERY NEXT LINE is the correct NodeName
		char nextLine[1024];
		GetNextLine(animScan,nextLine);
        CharUpperA( nextLine );

		if (strstr(nextLine,nodeName) == NULL)
		{
			animScan = strstr(animScan,ASE_ANIMATION);
		}
		else
		{
			animScan += strlen(nodeName)+1;
			break;
		}
	}

	if (animScan != NULL)
	{
        pOneToFill->m_numFrames = pHeader->m_numFrames;
		pOneToFill->m_frameRate = pHeader->m_frameRate;
		pOneToFill->m_tickRate = pHeader->m_tickRate;
        pOneToFill->m_firstFrame = pHeader->m_firstFrame;
		pOneToFill->m_pQuat = NULL;
		pOneToFill->m_pPos = NULL;

        retVal = 0; // OK, we found data
		
		//---------------------------------
		// Check for positional data first!
		char* scanStart = animScan;

		char numData[512];
		char nextLine[1024];
		float timeStamp = (pOneToFill->m_firstFrame+1) * pOneToFill->m_tickRate;

		//----------------------------------------------------
		// Then the very NEXT LINE most be POS_TRACK data OR
		// there is ONLY rotational Data for this node.
		GetNextLine(animScan,nextLine);
		if (strstr(nextLine,ASE_ANIM_POS_HEADER) != NULL)
		{
			animScan = strstr(animScan,ASE_ANIM_POS_HEADER);
			if (animScan)
			{
				countUp = true;
				actualCount++;
				animScan += strlen(ASE_ANIM_POS_HEADER);

				D3DXVECTOR3 thisOffset = nodeCenter;

//				char LineData[1024]; 
//				GetNextLine(animScan,LineData);
//				animScan += strlen(LineData)+1;

                // find close brace, and set to NULL
                char* pClose = strstr( animScan, "}" );
                if ( pClose )
                    *pClose = 0;

  
                for (DWORD j=0;j<pOneToFill->m_numFrames;j++)
				{
					sprintf(nodeName,"%s %d",ASE_ANIM_POS_SAMPLE,(long)timeStamp);
					char* scanData = strstr(animScan,nodeName);

					if (scanData)
					{
						scanData += strlen(nodeName)+1;

						GetNumberData(scanData,numData);
						thisOffset.x = (float)atof(numData);
						scanData += strlen(numData)+1;

						GetNumberData(scanData,numData);
						thisOffset.y = (float)atof(numData);
						scanData += strlen(numData)+1;

						GetNumberData(scanData,numData);
						thisOffset.z = (float)atof(numData);
						scanData += strlen(numData)+1;

//						GetNextLine(animScan,LineData);
//						animScan += strlen(LineData)+1;
					}

                    if ( !pOneToFill->m_pPos )
                    {
                        pOneToFill->m_pPos = new D3DXVECTOR3[pOneToFill->m_numFrames];
                    }

					pOneToFill->m_pPos[j] = thisOffset;
					timeStamp += pOneToFill->m_tickRate;
				}

                if ( pClose )
                    *pClose = '}';
            }
		}
									   
		//-------------------------------------------------------------
		// Check for rotational data. Again, use nextLine.
		
		//----------------------------------------------------
		// Then the very NEXT LINE most be POS_TRACK data OR
		// there is ONLY rotational Data for this node.
		GetNextLine(animScan,nextLine);
		if (strstr(nextLine,ASE_ANIM_ROT_HEADER) != NULL)
		{
			animScan = scanStart;
			timeStamp = (pOneToFill->m_firstFrame+1) * pOneToFill->m_tickRate;

			animScan = strstr(animScan,ASE_ANIM_ROT_HEADER);

			if (animScan)
			{
				countUp = true;
				actualCount++;
				animScan += strlen(ASE_ANIM_ROT_HEADER);

                char* pClose = strstr( animScan, "}" );
                if ( pClose )
                    *pClose = 0;


   			
				for (DWORD j=0;j<pOneToFill->m_numFrames;j++)
				{
					sprintf(nodeName,"%s %d",ASE_ANIM_ROT_SAMPLE,(long)timeStamp);
					char* scanData = strstr(animScan,nodeName);
                    if ( !scanData )
                    {
                        pOneToFill->m_numFrames = j;
                        if ( pClose )
                            *pClose = '}';

                        break;
                    }
                    animScan = scanData;

					D3DXQUATERNION thisFrame;

					float b=0.0f,c=0.0f,d=0.0f,phi=0.0f;

					if (scanData)
					{
						scanData += strlen(nodeName)+1;

						GetNumberData(scanData,numData);
						b = (float)atof(numData);
						scanData += strlen(numData)+1;

						GetNumberData(scanData,numData);
						c = (float)atof(numData);
						scanData += strlen(numData)+1;

						GetNumberData(scanData,numData);
						d = (float)atof(numData);
						scanData += strlen(numData)+1;

						GetNumberData(scanData,numData);
						phi = (float)atof(numData);
	
						//--------------------------------------------
						// MAX Writes out Quaternions as Angle, Axis.
						// Must Convert to real quaternion here.
						thisFrame.w = (float)cos(phi / 2.0f);
						thisFrame.x = b * (float)sin(-phi / 2.0f);
						thisFrame.y = c * (float)sin(-phi / 2.0f);
						thisFrame.z = d * (float)sin(-phi / 2.0f);
					}
					else
					{
						//Otherwise rotation is 0.
						thisFrame.w = 1.0f;
						thisFrame.x = 0.0f;
						thisFrame.y = 0.0f;
						thisFrame.z = 0.0f;
					}

                    D3DXQuaternionNormalize( &thisFrame, &thisFrame );

                    if ( !pOneToFill->m_pQuat )
                    {
                        // We have rotational data at least.  Store everything off.
				        pOneToFill->m_pQuat = new D3DXQUATERNION[pOneToFill->m_numFrames];
				        ASSERT(pOneToFill->m_pQuat != NULL);
                    }

					if (!j)
					{
						pOneToFill->m_pQuat[j] = thisFrame;
					}
					else
					{
                        D3DXQuaternionMultiply( &pOneToFill->m_pQuat[j], &pOneToFill->m_pQuat[j-1], &thisFrame );
						D3DXQuaternionNormalize( &pOneToFill->m_pQuat[j], &pOneToFill->m_pQuat[j] );
					}

					timeStamp += pOneToFill->m_tickRate;
				}

                if ( pClose )
                    *pClose = '}';

			}
		}
		
		countUp = false;
	}

    return retVal;
}
	





#ifdef FIND_MEMORY_LEAK

int fLogMemory = 1;       // Perform logging (0=no; nonzero=yes)?
int cBlocksAllocated = 0;  // Count of blocks allocated.
#undef new


void* __cdecl operator new[](size_t nSize, const char* lpszFileName, int nLine)
{
    return operator new( nSize, lpszFileName, nLine );
}


void* __cdecl operator new(size_t nSize, const char* lpszFileName, int nLine)
{
    static fInOpNew = 0;    // Guard flag.
    void* pRetVal = malloc( nSize );

    if( fLogMemory && !fInOpNew )
    {
    //    fInOpNew = 1;
        char outputStr[256];
        sprintf( outputStr,"MemoryBlock %ld: at %ld allocated %ld bytes, file = %s, line = %ld\n", 
            ++cBlocksAllocated, pRetVal, nSize, lpszFileName, nLine);
        DbgPrint( outputStr );
       
        fInOpNew = 0;
    }

    return pRetVal;

}

/*void* __cdecl operator new(size_t nSize)
{
    static fInOpNew = 0;    // Guard flag.
    void* pRetVal = malloc( nSize );

    if( fLogMemory && !fInOpNew )
    {
    //    fInOpNew = 1;
        char outputStr[256];
        sprintf( outputStr,"MemoryBlock %ld: at %ld allocated %ld bytes\n", 
            ++cBlocksAllocated, pRetVal, nSize);
        DbgPrint( outputStr );
       
        fInOpNew = 0;
    }

    return pRetVal;

}*/

void __cdecl operator delete( void* pMem, const char* lpszFileName, int nLine )
{
    static fInOpDelete = 0;    // Guard flag.
    if( fLogMemory && !fInOpDelete )
    {
    //    fInOpDelete = 1;        
        char outputStr[256];
        sprintf( outputStr,"MemoryBlock %ld: at %ld freed n file = %s, line = %ld\n",
            --cBlocksAllocated, pMem, lpszFileName, nLine );
        DbgPrint( outputStr );
    }

    free( pMem );

}

void __cdecl operator delete( void* pMem )
{
    static fInOpDelete = 0;    // Guard flag.
    if( fLogMemory && !fInOpDelete )
    {
  //      fInOpDelete = 1;        
        char outputStr[256];
        sprintf( outputStr,"MemoryBlock %ld: at %ld freed\n", --cBlocksAllocated, pMem );
        DbgPrint( outputStr );
    }

    free( pMem );

}



#endif



//-------------------------------------------------------------------------------
