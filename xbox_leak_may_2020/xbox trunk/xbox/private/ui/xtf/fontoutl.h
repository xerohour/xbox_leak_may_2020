#include "Glyph.h"

#define WFO_FAILURE   FALSE 
#define WFO_SUCCESS   TRUE

#define PI      3.141592653589793
#define TWO_PI  2.0*PI   

#define ZERO_EPS    0.00000001

#define WARNING(s) OutputDebugString(s)
#define ASSERTOPENGL(a, b) {}
#define DBGPRINT(s) OutputDebugString(s)
#define ALLOC(size) malloc(size)
#define ALLOCZ(size) calloc(size, 1)
#define REALLOC(pb, size) realloc(pb, size)
#define __GL_ATAN2F(a, b) atan2(a, b)
#define __GL_SQRTF(a) sqrtf(a)
                    
#define FREE(pb) free(pb);

struct SFCVertex
{
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vNormal;
};

struct SFCFace16
{
    WORD rgwIndices[3];
};

// default starting size for arrays in a face context
const int x_cFaceContextDefaultSize = 100;

class CFaceContext
{
public:
    CFaceContext();
    ~CFaceContext();

    HRESULT Init();

    void SetCurrentNormal(D3DXVECTOR3 *pvNormalNew)
    {
        m_vCurrentNormal = *pvNormalNew;
    }

    void SetCurrentZ(float fZNew)
    {
        m_fCurrentZ = fZNew;
    }

    HRESULT AddStrip(D3DXVECTOR2 *rgvVertices, UINT cVertices, BOOL bCCW);
    HRESULT AddTriangle(D3DXVECTOR2 *rgvVertices, UINT cVertices, BOOL bCCW);
    HRESULT AddFan(D3DXVECTOR2 *rgvVertices, UINT cVertices, BOOL bCCW);
    HRESULT AddQuad(D3DXVECTOR3 *rgvVertices, D3DXVECTOR3 *rgvNormal, BOOL bCCW);
    HRESULT AddQuadStrip(D3DXVECTOR3 *rgvVertices, D3DXVECTOR3 *rgvNormal, UINT cVertices, BOOL bCCW);

    HRESULT CopyMesh(SFCFace16 *rgFaces, SFCVertex *rgVertices);
    HRESULT Translate(D3DXVECTOR3 *pvTrans);

    UINT CFaces() { return m_cFaces;}
    UINT CVertices() { return m_cVertices;}

//private:
    HRESULT ResizeFaces(UINT cNewMax);
    HRESULT ResizeVertices(UINT cNewMax);
    void AddFace(SFCFace16 *pface);
    HRESULT SetupPrim(D3DXVECTOR2 *rgvVertices, UINT cVertices, UINT cTriangles,UINT *piFirstIndex);

    SFCVertex   *m_rgvVertices;
    UINT        m_cVertices;
    UINT        m_cVerticesMax;

    SFCFace16   *m_rgFaces;
    UINT        m_cFaces;
    UINT        m_cFacesMax;

    // primitive generation data
    D3DXVECTOR3 m_vCurrentNormal;
    float       m_fCurrentZ;

	WCHAR m_wch;
};

// concatenate the letters together into one mesh
HRESULT 
GenerateMesh
(
    CFaceContext **rgpfcLetters,
    UINT cLetters,
    CGlyphObject* pGlyphObject
);

static const double   CoplanarThresholdAngle = PI/180.0/2.0; // 0.5 degreees

// outline prim types
#define PRIM_LINE     3
#define PRIM_CURVE    4

typedef struct {
    FLOAT x,y;
} POINT2D;

typedef struct {
    FLOAT x,y,z;
} POINT3D;

typedef struct {
    DWORD   primType;
    DWORD   nVerts;
    DWORD   VertIndex;// index into Loop's VertBuf
    POINT2D *pVert;   // ptr to vertex list in Loop's VertBuf
    POINT3D *pFNorm;  // face normals
    POINT3D *pVNorm;  // vertex normals
} PRIM;


typedef struct {
    PRIM    *PrimBuf;  // array of prims
    DWORD   nPrims;
    DWORD   PrimBufSize;
    POINT2D *VertBuf;  // buffer of vertices for the loop
    DWORD   nVerts;
    DWORD   VertBufSize;
    POINT3D *FNormBuf;  // buffer of face normals
    POINT3D *VNormBuf;  // buffer of vertex normals
} LOOP;

typedef struct {
    LOOP    *LoopBuf;  // array of loops
    DWORD   nLoops;
    DWORD   LoopBufSize;
} LOOP_LIST;

typedef struct {
    FLOAT        zExtrusion;
    INT          extrType;
    FLOAT*       FaceBuf;
    DWORD        FaceBufSize;
    DWORD        FaceBufIndex;
    DWORD        FaceVertexCountIndex;
#ifdef FONT_DEBUG
    BOOL         bSidePolys;
    BOOL         bFacePolys;
#endif
    HRESULT      hrTessErrorOccurred;
    CFaceContext        *pfc;
} EXTRContext;

// Memory pool for tesselation Combine callback
#define POOL_SIZE 50
typedef struct MEM_POOL MEM_POOL;

struct MEM_POOL {
    int      index;             // next free space in pool
    POINT2D  pool[POOL_SIZE];   // memory pool
    MEM_POOL *next;             // next pool
};


typedef struct {
    HRESULT             hrTessErrorOccurred;
    FLOAT               chordalDeviation;
    FLOAT               scale;
    int                 format;
    UCHAR*              glyphBuf;
    DWORD               glyphSize;
    HFONT               hfontOld;
    GLUtesselator*      tess;
    MEM_POOL            combinePool;     // start of MEM_POOL chain
    MEM_POOL            *curCombinePool; // currently active MEM_POOL
    EXTRContext         *ec;
} OFContext;  // Outline Font Context

EXTRContext*   extr_Init(                FLOAT       extrusion,  
                                         INT         format ); 

void           extr_Finish(              EXTRContext *ec );

void           extr_DrawLines(           EXTRContext *ec, 
                                         LOOP_LIST   *pLoopList );

BOOL           extr_DrawPolygons(        EXTRContext *ec,
                                         LOOP_LIST   *pLoopList );


BOOL           extr_PolyInit(            EXTRContext *ec );

void           extr_PolyFinish(          EXTRContext *ec );

void CALLBACK  extr_glBegin(             GLenum      primType,
                                         void        *data );

void CALLBACK  extr_glVertex(            GLfloat     *v,
                                         void        *data );

void CALLBACK  extr_glEnd(               void );

double         CalcAngle(                POINT2D     *v1, 
                                         POINT2D     *v2 );
