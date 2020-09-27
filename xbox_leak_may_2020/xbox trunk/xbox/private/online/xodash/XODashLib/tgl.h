/*************************************************************************************************\
TGL.h		        : Interface for TG_Shape and TG_TypeShape -- the main rendered objects for the
                        dash, these objects are created with ASEConv.  The scene class holds a 
                        root node for all of them, and calls the load functions.  ASEScene calls 
                        the save function.
                        
Creation Date		: 1/8/2002 11:08:29 AM
Copyright Notice	: (C) 2000 Microsoft
Author				: Heidi Gaertner
//-------------------------------------------------------------------------------------------------
Notes				: 
\*************************************************************************************************/


#ifndef TGL_H
#define TGL_H

#include "std.h"

class File;

//-----------------------------------------------------------------------------
// Mimicking resource structures as the Xbox will see them
//-----------------------------------------------------------------------------
#define D3DCOMMON_TYPE_VERTEXBUFFER  0x00000000
#define D3DCOMMON_TYPE_INDEXBUFFER   0x00010000

// The magic number to identify .xbg files
#define XBG_FILE_ID (((DWORD)'X'<<0)|(((DWORD)'B'<<8))|(((DWORD)'A'<<16))|(2<<24))

class TG_Shape;

// for finding memory leaks
#ifdef FIND_MEMORY_LEAK
void* __cdecl operator new (size_t size );
void __cdecl operator delete( void* pMem );
//void* operator new [] ( size_t size );
#endif

//-----------------------------------------------------------------------------
// Name: struct XBG_HEADER
// Desc: Header for a .xbg file
//-----------------------------------------------------------------------------
struct XBG_HEADER
{
    DWORD dwMagic;
    DWORD dwNumMeshFrames;
    DWORD dwSysMemSize;
    DWORD dwVidMemSize;
    DWORD dwAnimMemSize;
};


//-----------------------------------------------------------------------------
// Name: struct XBMESH_SUBSET
// Desc: Struct to hold data for rendering a mesh
//-----------------------------------------------------------------------------
struct XBMESH_SUBSET
{
    DWORD m_ID;		// from our resource MGR
    DWORD m_numUsed; // range of vertices
};

//-----------------------------------------------------------------------------
// Name: struct XBMESH_DATA
// Desc: Struct to hold data for a mesh
//-----------------------------------------------------------------------------
#pragma pack(1) 
struct XBMESH_DATA
{
    D3DVertexBuffer    m_VB;           // Mesh geometry
    DWORD              m_dwNumVertices;
    D3DIndexBuffer     m_IB;
    DWORD              m_dwNumIndices;
    
    DWORD              m_dwFVF;         // Mesh vertex info
    DWORD              m_dwVertexSize;
    D3DPRIMITIVETYPE   m_dwPrimType;

    DWORD              m_dwNumSubsets;  // Subset info, for rendering
    XBMESH_SUBSET*     m_pSubsets;

   	DWORD						m_numFrames;				//Number of Frames of animation.
	float 						m_frameRate;				//Number of Frames Per Second.
	float						m_tickRate;				    //Number of Ticks Per Second.
    D3DXVECTOR3*                m_pPos;
    D3DXQUATERNION*             m_pQuats;


};

#pragma pack()


//-----------------------------------------------------------------------------
// Name: struct XBMESH_FRAME
// Desc: Struct to provide a hierarchial layout of meshes. Note: in order for
//       Xbox fast math (via xgmath.h) to work, all D3DXMATRIX's must be
//       16-byte aligned.
//-----------------------------------------------------------------------------
__declspec(align(16)) struct XBMESH_FRAME
{
    D3DXVECTOR3        m_translation;  // Make sure this is 16-byte aligned!
    D3DXMATRIX         m_transform;
    
    XBMESH_DATA        m_MeshData;
    
    CHAR               m_strName[64];
    
    XBMESH_FRAME*      m_pChild;
    XBMESH_FRAME*      m_pNext;
};

struct TG_Animation
{
	DWORD						m_numFrames;				//Number of Frames of animation.
    DWORD                       m_firstFrame;
	float 						m_frameRate;				//Number of Frames Per Second.
	float						m_tickRate;				//Number of Ticks Per Second.
	D3DXQUATERNION*		        m_pQuat;					//Stores animation offset in Quaternion rotation.
	D3DXVECTOR3* 				m_pPos;					//Stores Positional offsets if present.  OTHERWISE NULL!!!!!!!!
		
};


//-------------------------------------------------------------------------------
// Structs used by layer.
//
typedef DWORD* DWORDPtr;

#define MAX_CHILD_OBJECTS   32

#define TG_SHAPE_TOO_MANY_CHILDREN 0x0001000

// empty TG_Shape, has a name, maybe a transform
class TG_TypeNode
{
	//---------------
	// Data Members
	protected:
		D3DXVECTOR3				m_nodeCenter;
		char					m_strNodeId[32];
		
	//---------------------
	// Member Functions
    protected:
        TG_TypeNode( TG_TypeNode& );

    private:
        // suppresed
        TG_TypeNode& operator=( const TG_TypeNode& );

	public:
	     TG_TypeNode( XBMESH_FRAME* pFrameData );
         TG_TypeNode() { init(); }
         virtual ~TG_TypeNode(){}

         virtual TG_TypeNode* Copy(); // virtual copy c'tor

         virtual long GetVertexCount() const { return 0; }

			
 		virtual void init (void)
		{
			m_strNodeId[0] = '\0';
			m_nodeCenter.x = m_nodeCenter.y = m_nodeCenter.z = 0.0f;
		}
		
		virtual void destroy (void);
		
		char *getNodeId (void)
		{
			return m_strNodeId;
		}

		D3DVECTOR GetNodeCenter (void) const
		{
			return m_nodeCenter;
		}

        virtual void GetRectDims(float& width, float& height){}
        virtual void GetRectDimsAndRecenter(float& width, float& height, float& depth){}

        virtual long SwapMaterials (DWORD indexToReplace, DWORD replaceItWithThis) { return -1; }
        virtual void SetMaterials (DWORD replaceItWithThis) {}
        virtual bool isShape() { return 0; }    // hack instead of RTTI
        virtual float GetMinZ() { return 0.f; }

        virtual HRESULT Render (){ return S_OK; } \

        friend class TG_Shape;

};

typedef TG_TypeNode* TG_TypeNodePtr;

class TG_MultiShape;
class TG_TypeMultiShape;
class TG_Shape;

#define TEXTURE_VERTEX_FVF D3DFVF_XYZ |D3DFVF_NORMAL|D3DFVF_TEX1 
#define NORMAL_VERTEX_FVF			D3DFVF_XYZ |D3DFVF_NORMAL	
#define COLOR_VERTEX_FVF			D3DFVF_XYZ | D3DFVF_DIFFUSE
//-------------------------------------------------------------------------------
// The meat and Potatoes part.
// TG_Shape
class TG_TypeShape : public TG_TypeNode
{
	//-------------------------------------------------------
	// This class runs the shape.  Knows how to load/import
	// an ASE file.  Can dig information out of the file for
	// User use (i.e. Texture Names).  Transforms, lights, clips
	// and renders the shape.

	friend TG_TypeMultiShape;
	friend TG_MultiShape;
	friend TG_Shape;

	
	//-------------
	//Data Members
	protected:
		DWORD					m_nNumVertices;			//Number of vertices in Shape
		D3DVertexBuffer	        m_VertexBuffer;
		D3DIndexBuffer*	        m_pIndexBuffer;			// not necessarily used
		DWORD					m_nNumFaces;
		XBMESH_SUBSET*			m_pMaterialIDs;				//Number of textures in Shape
		DWORD					m_nMaterialCount;
		DWORD					m_nFVF;
		DWORD					m_nFVFSize;

        BYTE*                   m_pMem;                 // for ASE parsing only, or copy c'tors
        float                   m_fMinZDepth;           // also for ase parsing...


		//-------------------------------------------------------------------------------
		// TG_TypeVertex
		//This Structure stores information for each vertex of the shape which DOES NOT CHANGE by instance
		struct TG_NormalVertex
		{
			//Only changes at load time.
			D3DXVECTOR3	position;				//Position of vertex relative to base position of shape.
			D3DXVECTOR3	normal;
		};

		struct TG_UVVertex
		{
			D3DXVECTOR3	position; // The 3D position for the vertex
			D3DXVECTOR3	normal;
			float u,v;
		};

		struct TG_ColorVertex
		{
			D3DXVECTOR3	position;				//Position of vertex relative to base position of shape.
			DWORD		argb;
		};


	//-----------------
	//Member Functions
	protected:

	public:

        virtual TG_TypeNode* Copy(); // virtual copy c'tor


		virtual void init (void);
		
		TG_TypeShape (void)
		{
			init();
		}

        TG_TypeShape( XBMESH_FRAME* pFrameData, void* pVBData );
        TG_TypeShape( TG_TypeShape& );


		virtual void destroy (void);

		virtual ~TG_TypeShape (void)
		{
			destroy();
		}

         virtual bool isShape() { return 1; }    // hack instead of RTTI

 		//Function return 0 is OK.  -1 if file is not ASE Format or missing data.
		//This function simply parses the ASE buffers handed to it.  This allows
		//users to load the ase file themselves and manage their own memory for it.
		//It allocates memory for internal Lists.  These are straight mallocs at present.
		//
		// NOTE: Only takes the first GEOMOBJECT from the ASE file.  Multi-object
		// Files will require user intervention to parse!!
		long ParseASEFile (BYTE *aseBuffer, const char *filename, const D3DVECTOR& parentLoc);	//filename for error reporting ONLY


		long ParseASENoTextures (BYTE *aseBuffer, const char *fileName, const D3DVECTOR& parentLoc);

		//Function returns 0 if OK.  -1 if file not found or file not ASE Format.		
		//This function loads the ASE file into the TG_Triangle and TG_Vertex lists.
		//It allocates memory.  These are straight news at present.
		//
		// NOTE: Only takes the first GEOMOBJECT from the ASE file.  Multi-object
		// Files will require user intervention to parse!!
		long LoadTGShapeFromASE (const char* buffer,  const char *fileName, const D3DVECTOR& parentLoc);

		// pre-lit vertices
		long ParseASEColors (BYTE *aseBuffer, const char *fileName, const D3DVECTOR& parentLoc);


		//Function returns 0 if OK.  -1 if textureNum is out of range of numTextures.
		//This function takes the gosTextureHandle passed in and assigns it to the
		//textureNum entry of the listOfTextures;
        long SwapMaterials (DWORD indexToReplace, DWORD replaceItWithThis);
        void SetMaterials( DWORD newMaterial );


        virtual void GetRectDims( float& width, float& height );
        virtual void GetRectDimsAndRecenter(float& width, float& height, float& depth);
       
   		virtual HRESULT Render ( );

        virtual long GetVertexCount() const { return m_nNumVertices; }
        virtual float GetMinZ() { return m_fMinZDepth; }


		struct TG_TVert
		{
			float	u, v;
		};

		struct TG_TFace
		{
			long   index0, index1, index2;
		};

  

    private:

    bool CreateNewNormal( const D3DXVECTOR3& newNormal, TG_NormalVertex* pListOfVertices, 
        long* normalCounts, long& index, long* altIndex  );

};

typedef TG_TypeShape* TG_TypeShapePtr;

//-------------------------------------------------------------------------------------
// The OTHER meat and Potatoes part.  This is the actual instance the game draws with.
// TG_Shape
class TG_Shape
{
	//-------------------------------------------------------
	// This class runs the shape instance.
	// Transforms, lights, clips and renders the shape.

	friend TG_MultiShape;
	friend TG_TypeShape;
	friend TG_TypeNode;
	friend TG_TypeMultiShape;

	
	
	//-------------
	//Data Members
	protected:
		TG_TypeNode*			m_pMyType;						//Pointer to the instance of the shape.
		D3DXMATRIX				m_transform;
		
		TG_Shape*				m_pChild;					    // array of kids
		TG_Shape*				m_pParent;
        TG_Shape*               m_pNext;

        TG_Animation            m_Animation;


        bool                    m_bCommonMemory;                  // if from ASE, we alloc ourseleves, otherwise need to clean up
        DWORD                   m_dwEnumeratedID;

        bool                    m_bRender;                      // turn off things like text regions
        long                    m_dwFrameID;                    // only needed when saving ASE;s

        D3DXVECTOR3             m_Scale;                       // too hard to put in and out of the transform
        float                   m_curTime;                     // animate

        static DWORD            s_dwNumFrames;
        static DWORD            s_dwFrameSpace;
        static DWORD            s_dwSubsetSpace;
        static DWORD            s_dwIndicesSpace;
        static DWORD            s_dwVerticesSpace;
        static DWORD            s_dwAnimSpace;
        static DWORD            s_dwMeshFileOffset;
        static DWORD            s_dwSubsetFileOffset;
        static DWORD            s_dwIndicesFileOffset;
        static DWORD            s_dwVerticesFileOffset;
        static DWORD            s_dwAnimFileOffset;


        
		

	public:

	//-----------------
	//Member Functions
	protected:

	public:

		void init (void)
		{
			m_pMyType = NULL;
			D3DXMatrixIdentity(&m_transform);
			m_pChild = NULL;
			m_pParent = NULL;
            m_pNext = NULL;
            m_dwEnumeratedID = 0;
            m_bRender = 1;
            m_Scale.x = m_Scale.y = m_Scale.z = 1.f;
            memset( &m_Animation, 0, sizeof( m_Animation  ) );
            m_curTime = 0.f;
            m_bCommonMemory = 0;

		}
		
		TG_Shape (void)
		{
			init();
		}

        TG_Shape( TG_Shape& src );

        TG_Shape( LPDIRECT3DDEVICE8 pd3dDevice, TG_Shape* pParent, 
                      XBMESH_FRAME* pFrameData, void* pVBData );
        
		// add a child to the heirarchy
        HRESULT AddChild( TG_Shape* pKid );
        
		void Destroy (void);

		~TG_Shape (void)
		{
			Destroy();
		}

		void Dump(bool bAllData = false);


		// ASE parsing routine
        long        Load(const char* buffer, const char* fileName, const D3DVECTOR& parentLoc, TG_Animation* pHeader );

        // Move this thing around with these
		void        SetTransform( const D3DXMATRIX& newTransform );
   		void		LocalRotateY( float angle );
		void		LocalRotateZ( float angle );
        void        LocalRotateX( float angle );

        // scale it with this
        void        SetScale( float newX, float newY, float newZ );

        void        GetLocalTransform( D3DXMATRIX& transform );
        void        GetWorldTransform( D3DXMATRIX& transform );


		// draw
        long        Render (  );
        HRESULT     FrameMove( float frameLength );

        // Use these to iterate through the tree
        TG_Shape*   GetFirstChild();
        TG_Shape*   GetNextChild(TG_Shape* pLast);

		
		char *      GetNodeName (void) const;

        TG_Shape*   GetParent( ) {return m_pParent;}


        // set the name of this object
        void        SetNodeName(const char*);

        // search for subobject with this name, will recurse
        TG_Shape*   FindObject( const char* pName );

        HRESULT     SaveBinary(File* pFile);

        // returns the width and height of the first 4 vertices of this object
        void        GetRectDims( float& width, float& height );
        // does the above, and moves the center point
        void        GetRectDimsAndRecenter(float& width, float& height);
        // does the above, and builds a rotation into the transform -- this is for text
        void        GetRectDimsRecenterBuildRotation(float& width, float& height);

        void        GetNodeCenter( float& x, float& y, float& z );

        // swap the old with the new
        void        SwapMaterials( long oldMaterial, long newMaterial );
        // set all materials to the new one
        void        SetMaterials( long newMaterial );

        void setVisible( bool bVisible )
        {
            m_bRender = bVisible;
        }

        bool isVisible() const { return m_bRender; }

        // sort all the children.. first by Z, then by the names that are passed in
        void        SortShapes( const char** strNames, long nameCount );
        void        SortShapesName( const char** strNames, long nameCount );
        // sort by Z
        void        SortShapesZ( );

        HRESULT     BeginAnimation(bool bLoop);
        HRESULT     EndAnimation( );

        
        static long    ParseAnimationHeader( char* aseContents, TG_Animation* pHeader );
        static long    LoadAnimationFromASE (char *aseContents, const char* pNodeName, 
                                     D3DXVECTOR3 nodeCenter, TG_Animation* pHeader, TG_Animation* pOneToFill );


// HELPER FUNCTIONS
    private:

    



    static BOOL ComputeMemoryRequirementsCB( TG_Shape* pFrame, VOID* );
    static BOOL WriteSubsetsCB( TG_Shape* pFrame, VOID* pData );
    static BOOL WriteIndicesCB( TG_Shape* pFrame, VOID* pData );
    static BOOL WriteVerticesCB( TG_Shape* pFrame, VOID* pData );
    static BOOL WriteMeshInfoCB( TG_Shape* pFrame, VOID* pData );
    static BOOL WriteAnimationCB( TG_Shape* pFrame, VOID* pData );


    void        GetZVals(float& numberOfVertices, float& zVal);
    void        GetMinZ(float& curCount, float& z);
    float       GetMinZ();





      // HELPERS
        BOOL EnumFrames( BOOL (*EnumFramesCB)(TG_Shape*,VOID*),
                            VOID* pData );

        // suppressed
         TG_Shape& operator= ( const TG_Shape& src );


		
};

typedef TG_Shape* TG_ShapePtr;


//-------------------------------------------------------------------------------
#endif
