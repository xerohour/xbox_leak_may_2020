#include "stdafx.h"

#include <math.h>
#include <malloc.h>
#include "Glyph.h"

#define PRINT Print

CFile* g_pFile;
CStdioFile* g_pWrlFile;

void Print(const TCHAR* szMsg, ...)
{
	if (g_pWrlFile == NULL)
		return;

	va_list args;
	va_start(args, szMsg);

	TCHAR szBuffer [512];
	_vsntprintf(szBuffer, sizeof(szBuffer)/sizeof(TCHAR), szMsg, args);

	g_pWrlFile->WriteString(szBuffer);

	va_end(args);
}



#define D3DXASSERT ASSERT
#define RELEASE(p) { if ((p) != NULL) (p)->Release(); }

namespace D3DX
{
#include <GL\glu.h>  // doesn't link against GL, but needs the headers
#include "fontoutl.h"

static HRESULT CreateOFContext(      HDC         hdc,
                                        FLOAT       chordalDeviation,
                                        FLOAT       extrusion, 
                                        int         type,
                                        OFContext   **ppofc );

static BOOL  ScaleFont(                 HDC         hdc, 
                                        OFContext*  ofc,
                                        BOOL        bUnicode );

static void  DestroyOFContext(          HDC         hdc,
                                        OFContext*  ofc );

static BOOL  DrawGlyph(                 OFContext*  ofc );

static HRESULT  MakeDisplayListFromGlyph(  OFContext*     ofc
                                        );


static BOOL  MakeLinesFromArc(          OFContext*  ofc, 
                                        LOOP*       pLoop,
                                        PRIM*       pPrim,
                                        POINT2D     p0,
                                        POINT2D     p1,
                                        POINT2D     p2,
                                        FLOAT       chordalDeviationSquared);

static HRESULT MakeLinesFromGlyph(   OFContext*  ofc, LOOP_LIST** ppLoopList);

static BOOL  MakeLinesFromTTLine(       OFContext*  ofc, 
                                        LOOP*       pLoop,
                                        PRIM*       pPrim,
                                        UCHAR**     pp,
                                        WORD        pointCount );

static BOOL  MakeLinesFromTTPolycurve(  OFContext*  ofc, 
                                        LOOP*       pLoop,
                                        UCHAR**     pp );

static BOOL  MakeLinesFromTTPolygon(    OFContext*  ofc, 
                                        LOOP_LIST*  pLoopList,
                                        UCHAR**     pp );

static BOOL  MakeLinesFromTTQSpline(    OFContext*  ofc, 
                                        LOOP*       pLoop,
                                        PRIM*       pPrim,
                                        UCHAR**     pp,
                                        WORD        pointCount );

static void CALLBACK TessError(         GLenum      error,
                                        void        *data);

static void CALLBACK TessCombine(       GLdouble    coord[3], 
                                        POINT2D*    data[4], 
                                        GLfloat     w[4],
                                        POINT2D**   dataOut,
                                        void        *userData);

static void FreeCombinePool(            MEM_POOL    *combinePool );

static void ApplyVertexFilter(          LOOP_LIST   *pLoopList );

static void CheckRedundantVertices(     LOOP*       pLoop );

static BOOL PointsColinear(             POINT2D     *p1, 
                                        POINT2D     *p2, 
                                        POINT2D     *p3 ); 

static FLOAT      GetFixed(             UCHAR**     p );

static LOOP_LIST* InitLoopBuf(          void );

static LOOP*      NewLoop(              LOOP_LIST   *Loops, 
                                        POINT2D     *pFirstPoint );

static void       FreeLoopList(         LOOP_LIST   *pLoopList );

static PRIM*      NewPrim(              LOOP        *pLoop, 
                                        DWORD       primType );

static void       CalcVertPtrs(         LOOP        *pLoop );

static BOOL       AppendToVertBuf(      LOOP*       pLoop,
                                        PRIM*       pPrim,
                                        POINT2D     *p );


// macros to access data from byte streams:

// get WORD from byte stream, increment stream ptr by WORD
//#define GetWord( p ) \
    //( *( ((WORD *) *p)++ ) ) 

// get DWORD from byte stream, increment stream ptr by DWORD
//#define GetDWord( p ) \
    //( *( ((DWORD *) *p)++ ) ) 
static inline DWORD 
GetDWord(UCHAR **p)
{
    DWORD dwRet;
    dwRet = *((DWORD*)*p);
    *p += sizeof(DWORD);
    return dwRet;
}

static inline WORD 
GetWord(UCHAR **p)
{
    WORD wRet;
    wRet = *((WORD*)*p);
    *p += sizeof(WORD);
    return wRet;
}

static inline SHORT 
GetSignedWord(UCHAR **p)
{
    SHORT wRet;
    wRet = *((WORD*)*p);
    *p += sizeof(SHORT);
    return wRet;
}

// get signed word (SHORT) from byte stream, increment stream ptr by SHORT
//#define GetSignedWord( p ) \
    //( *( ((SHORT *) *p)++ ) ) 


#define POINT2DEQUAL( p1, p2 ) \
    ( (p1->x == p2->x) && (p1->y == p2->y) )





/*****************************************************************************
 * MakeDisplayListFromGlyph
 * 
 * Converts the outline of a glyph to OpenGL drawing primitives, tessellating
 * as needed, and then draws the glyph.  Tessellation of the quadratic splines
 * in the outline is controlled by "chordalDeviation", and the drawing
 * primitives (lines or polygons) are selected by "format".
 *
 * Return value is nonzero for success, zero for failure.
 *
 * Does not check for OpenGL errors, so if the caller needs to know about them,
 * it should call glGetError().

 * History:
 *  26-Sep-1995 -by- Marc Fortier [marcfo]
 * Use extrusioniser to draw polygonal faces with extrusion=0

*****************************************************************************/


static
HRESULT
MakeDisplayListFromGlyph
( 
    OFContext *ofc 
)
{
    HRESULT             hr = S_OK;
    DWORD               nLoops;
    DWORD               nVerts;
    LOOP_LIST           *pLoopList;
    LOOP                *pLoop;
    POINT2D             *p;
    MEM_POOL            *mp = NULL;

    /*
     * Convert the glyph outlines to a set of polyline loops.
     * (See MakeLinesFromGlyph() for the format of the loop data
     * structure.)
     */
    hr = MakeLinesFromGlyph(ofc, &pLoopList);
    if (FAILED(hr))
        goto e_Exit;

    /*
     * Filter out unnecessary vertices
     */
    ApplyVertexFilter( pLoopList );

    D3DXASSERT(ofc->format == WGL_FONT_POLYGONS);

    {
        GLdouble v[3];

        /*
         * This is the hard case.  We have to set up a tessellator
         * to convert the outlines into a set of polygonal
         * primitives, which the tessellator passes to some
         * auxiliary routines for drawing.
         */

        /* Initialize polygon extrusion for the glyph.
         * This prepares for tracking of the tesselation in order to
         * build the Back-facing polygons.
         */

        mp = &ofc->combinePool;
        ofc->curCombinePool = mp;
        mp->index = 0;
        mp->next = NULL;

        if( ofc->ec ) 
        {
            if( !extr_PolyInit( ofc->ec ) )
                goto e_Exit;

        }

        ofc->hrTessErrorOccurred = S_OK;
        v[2] = 0.0;
        gluTessBeginPolygon( ofc->tess, ofc );

        /*
         * Each loop returned from MakeLinesFromGlyph is closed (first and 
         * last points are the same).  The old tesselator had trouble with
         * this.  Since the tesselator automatically closes all loops,
         * we skip the last point to be on the safe side.
         */

        nLoops = pLoopList->nLoops;
        pLoop = pLoopList->LoopBuf;
        for( ; nLoops; nLoops--, pLoop++ )
        {
            gluTessBeginContour( ofc->tess );
                
            nVerts = pLoop->nVerts - 1;  // skip last point

            p = pLoop->VertBuf;
            for( ; nVerts; nVerts--, p++ )
            {
                v[0] = p->x;
                v[1] = p->y;
                gluTessVertex(ofc->tess, v, p);
            }
            gluTessEndContour( ofc->tess );
        }

        gluTessEndPolygon( ofc->tess );

        if (FAILED(ofc->hrTessErrorOccurred))
        {
            hr = ofc->hrTessErrorOccurred;
            goto e_Exit;
        }

        if( ofc->ec ) {
            /* check for OUT_OF_MEMORY_ERROR in extrusion lib, that might
             * have occured during tesselation tracking.
             */
            if( FAILED(ofc->ec->hrTessErrorOccurred) )
            {
                hr = ofc->ec->hrTessErrorOccurred;
                goto e_Exit;
            }

            if( !extr_DrawPolygons( ofc->ec, pLoopList ) ) 
                goto e_Exit; 
        }
    }

e_Exit:
    /*
     * Putting PolyFinish here means PolyInit may not have been called.
     * This is ok.
     */
    if( mp )
        FreeCombinePool( mp );
    if( pLoopList )
        FreeLoopList( pLoopList );
    if( ofc->ec )
        extr_PolyFinish( ofc->ec );

    return hr;
}

/*****************************************************************************
 * TessCombine
 *
 * Tesselation callback for loop intersection.  We have to allocate a vertex
 * and return it to tesselator.  Allocation is from the context's static pool.
 * If this runs dry, then a linked list of MEM_POOL blocks is used.

*****************************************************************************/
 
static void CALLBACK
TessCombine( GLdouble coord[3], POINT2D *data[4], GLfloat w[4],
             POINT2D **dataOut, void *userData )
{
    OFContext *ofc = (OFContext *) userData;
    MEM_POOL *mp = ofc->curCombinePool;
    POINT2D *p;

    // make sure there's room available in the current pool block
    if( mp->index >=  POOL_SIZE )
    {
        // we need to allocate another MEM_POOL block
        MEM_POOL *newPool;

        newPool = (MEM_POOL *) ALLOC( sizeof(MEM_POOL) );
        if( !newPool )
            // tesselator will handle any problem with this
            return;

        newPool->index = 0;
        newPool->next = NULL;
        mp->next = newPool;
        mp = newPool;
        ofc->curCombinePool = mp; // new pool becomes the current pool
    }

    p = mp->pool + mp->index;
    p->x = (GLfloat) coord[0];        
    p->y = (GLfloat) coord[1];        
    mp->index ++;

    *dataOut = p;
}

/*****************************************************************************
 * FreeCombinePool
 *
 * Frees any pools of memory allocated by TessCombine callback

*****************************************************************************/
static void
FreeCombinePool( MEM_POOL *memPool )
{
    MEM_POOL *nextPool;

    memPool = memPool->next;  // first pool in list is static part of context
    while( memPool ) {
        nextPool = memPool->next;
        FREE( memPool );
        memPool = nextPool;
    }
}

/*****************************************************************************
 * TessError
 *
 * Saves the last tessellator error code in ofc->TessErrorOccurred.

*****************************************************************************/
 
static void CALLBACK
TessError(GLenum error, void *data)
{
    OFContext *ofc = (OFContext *) data;

    // Only some of these errors are fatal:
    switch( error ) {
        case GLU_TESS_COORD_TOO_LARGE:
        case GLU_TESS_NEED_COMBINE_CALLBACK:
            ofc->hrTessErrorOccurred = E_OUTOFMEMORY;
            break;
        default:
            break;
    }
}



/*****************************************************************************
 * MakeLinesFromGlyph
 * 
 * Converts the outline of a glyph from the TTPOLYGON format into
 * structures of Loops, Primitives and Vertices.
 *
 * Line segments from the TTPOLYGON are transferred to the output array in
 * the obvious way.  Quadratic splines in the TTPOLYGON are converted to
 * collections of line segments

*****************************************************************************/


static HRESULT
MakeLinesFromGlyph
( 
    OFContext* ofc,
    LOOP_LIST **ppLoopList
)
{
    UCHAR*  p;
    HRESULT hr = S_OK;
    LOOP_LIST *pLoopList;

    *ppLoopList = NULL;

    /*
     * Initialize the buffer into which we place the loop data:
     */
    if( !(pLoopList = InitLoopBuf()) )
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    p = ofc->glyphBuf;
    while (p < ofc->glyphBuf + ofc->glyphSize)
    {
        if( !MakeLinesFromTTPolygon( ofc, pLoopList, &p) )
        {
            WARNING("D3DXMakeLinesFromTTPolygon Failed!\n");
            hr = E_FAIL;
            goto e_Exit;
        }
    }

    *ppLoopList = pLoopList;
    pLoopList = NULL;
e_Exit:
    if (pLoopList != NULL) 
    {
        FreeLoopList( pLoopList );
    }
    
    return hr;
}



/*****************************************************************************
 * MakeLinesFromTTPolygon
 *
 * Converts a TTPOLYGONHEADER and its associated curve structures into a
 * LOOP structure.

*****************************************************************************/

static BOOL
MakeLinesFromTTPolygon( IN      OFContext*  ofc, 
                        IN      LOOP_LIST*  pLoopList,
                        IN OUT  UCHAR**     pp)
{
    DWORD   polySize;
    UCHAR*  polyStart;
    POINT2D *pFirstP, *pLastP, firstPoint;
    LOOP    *pLoop;
    PRIM    *pPrim;

    /*
     * Record where the polygon data begins.
     */
    polyStart = *pp;

    /*
     * Extract relevant data from the TTPOLYGONHEADER:
     */
    polySize = GetDWord(pp);
    if( GetDWord(pp) != TT_POLYGON_TYPE )  /* polygon type */
        return WFO_FAILURE;
    firstPoint.x = ofc->scale * GetFixed(pp); // 1st X coord
    firstPoint.y = ofc->scale * GetFixed(pp); // 1st Y coord

    /* 
     * Initialize a new LOOP struct in the LoopBuf, with the first point
     */
    if( !(pLoop = NewLoop( pLoopList, &firstPoint )) )
        return WFO_FAILURE;
    
    /*
     * Process each of the TTPOLYCURVE structures in the polygon:
     */

    while (*pp < polyStart + polySize) {
        if( !MakeLinesFromTTPolycurve(  ofc, pLoop, pp ) )
            return WFO_FAILURE;
    }

    /* Now have to fix up end of loop : after studying the chars, it
     * was determined that if a curve started with a line, and ended with
     * a qspline, AND the first and last point were not the same, then there
     * is an implied line joining the two.
     * In any case, we also make sure here that first and last points are
     * coincident.
     */
    
    pLastP = (POINT2D *) (pLoop->VertBuf+pLoop->nVerts-1);
    pFirstP = &firstPoint;

    if( !POINT2DEQUAL( pLastP, pFirstP ) ) {
        // add 1-vertex line prim at the end

        if( !(pPrim = NewPrim( pLoop, TT_PRIM_LINE)) )
            return WFO_FAILURE;

        if ( !AppendToVertBuf( pLoop, pPrim, pFirstP) )
            return WFO_FAILURE;
    }

    /* At end of each loop, calculate pVert for each PRIM from its
     * VertIndex value (for convenience later).
     */
    CalcVertPtrs( pLoop );

    return WFO_SUCCESS;
}


/*****************************************************************************
 * MakeLinesFromTTPolyCurve
 *
 * Converts the lines and splines in a single TTPOLYCURVE structure to points
 * in the Loop.

*****************************************************************************/

static BOOL
MakeLinesFromTTPolycurve( IN     OFContext* ofc, 
                          IN     LOOP*      pLoop,
                          IN OUT UCHAR**    pp )
{
    WORD type;
    WORD pointCount;
    PRIM *pPrim;

    /*
     * Pick up the relevant fields of the TTPOLYCURVE structure:
     */
    type = GetWord(pp);
    pointCount = GetWord(pp);

    if( !(pPrim = NewPrim( pLoop, type )) )
        return WFO_FAILURE;

    /*
     * Convert the "curve" to line segments:
     */
    if (type == TT_PRIM_LINE) {
        return MakeLinesFromTTLine( ofc, pLoop, pPrim, pp, pointCount);

    } else if (type == TT_PRIM_QSPLINE) {
        return MakeLinesFromTTQSpline( ofc, pLoop, pPrim, pp, pointCount );

    } else
        return WFO_FAILURE;
}



/*****************************************************************************
 * MakeLinesFromTTLine
 *
 * Converts points from the polyline in a TT_PRIM_LINE structure to
 * equivalent points in the Loop.

*****************************************************************************/
static BOOL
MakeLinesFromTTLine(    IN     OFContext* ofc, 
                        IN     LOOP*      pLoop,
                        IN     PRIM*      pPrim,
                        IN OUT UCHAR**    pp,
                        IN     WORD       pointCount)
{
    POINT2D p;

    /*
     * Just copy the line segments into the vertex buffer (converting
     * type as we go):
     */

    while (pointCount--)
    {
        p.x = ofc->scale * GetFixed(pp); // X coord 
        p.y = ofc->scale * GetFixed(pp); // Y coord
        if( !AppendToVertBuf( pLoop, pPrim, &p ) )
            return WFO_FAILURE;
    }

    return WFO_SUCCESS;
}


/*****************************************************************************
 * MakeLinesFromTTQSpline
 *
 * Converts points from the poly quadratic spline in a TT_PRIM_QSPLINE
 * structure to polyline points in the Loop. 

*****************************************************************************/

static BOOL
MakeLinesFromTTQSpline( IN      OFContext*  ofc, 
                        IN      LOOP*       pLoop,
                        IN      PRIM*       pPrim,
                        IN  OUT UCHAR**     pp,
                        IN      WORD        pointCount )
{
    POINT2D p0, p1, p2;
    WORD point;
    POINT2D p;

    /*
     * Process each of the non-interpolated points in the outline.
     * To do this, we need to generate two interpolated points (the
     * start and end of the arc) for each non-interpolated point.
     * The first interpolated point is always the one most recently
     * stored in VertBuf, so we just extract it from there.  The
     * second interpolated point is either the average of the next
     * two points in the QSpline, or the last point in the QSpline
     * if only one remains.
     */

    // Start with last generated point in VertBuf
    p0 = *(pLoop->VertBuf + pLoop->nVerts - 1);

    // pointCount should be >=2, but in case it's not...
    p1 = p2 = p0;

    for (point = 0; point < pointCount - 1; ++point)
    {
        p1.x = ofc->scale * GetFixed(pp);
        p1.y = ofc->scale * GetFixed(pp);

        if (point == pointCount - 2)
        {
            /*
             * This is the last arc in the QSpline.  The final
             * point is the end of the arc.
             */
            p2.x = ofc->scale * GetFixed(pp);
            p2.y = ofc->scale * GetFixed(pp);
        }
        else
        {
            /*
             * Peek at the next point in the input to compute
             * the end of the arc:
             */
            p.x = ofc->scale * GetFixed(pp);
            p.y = ofc->scale * GetFixed(pp);
            p2.x = 0.5f * (p1.x + p.x);
            p2.y = 0.5f * (p1.y + p.y);
            /*
             * Push the point back onto the input so it will
             * be reused as the next off-curve point:
             */
            *pp -= 2*sizeof(FIXED); // x and y
        }

        if( !MakeLinesFromArc(  ofc,
                                pLoop,
                                pPrim,
                                p0,
                                p1,
                                p2,
                                ofc->chordalDeviation * ofc->chordalDeviation))
            return WFO_FAILURE;

        // p0 is now the last interpolated point (p2)
        p0 = p2;
    }

    // put in last point in arc
    if( !AppendToVertBuf( pLoop, pPrim, &p2 ) )
        return WFO_FAILURE;

    return WFO_SUCCESS;
}


/*****************************************************************************
 * MakeLinesFromArc
 *
 * Subdivides one arc of a quadratic spline until the chordal deviation
 * tolerance requirement is met, then places the resulting set of line
 * segments in the Loop.

*****************************************************************************/

static BOOL
MakeLinesFromArc(   IN OFContext *ofc, 
                    IN LOOP*     pLoop,
                    IN PRIM*     pPrim,
                    IN POINT2D   p0,
                    IN POINT2D   p1,
                    IN POINT2D   p2,
                    IN FLOAT     chordalDeviationSquared)
{
    POINT2D p01;
    POINT2D p12;
    POINT2D midPoint;
    FLOAT   deltaX;
    FLOAT   deltaY;

    /*
     * Calculate midpoint of the curve by de Casteljau:
     */
    p01.x = 0.5f * (p0.x + p1.x);
    p01.y = 0.5f * (p0.y + p1.y);
    p12.x = 0.5f * (p1.x + p2.x);
    p12.y = 0.5f * (p1.y + p2.y);
    midPoint.x = 0.5f * (p01.x + p12.x);
    midPoint.y = 0.5f * (p01.y + p12.y);


    /*
     * Estimate chordal deviation by the distance from the midpoint
     * of the curve to its non-interpolated control point.  If this
     * distance is greater than the specified chordal deviation
     * constraint, then subdivide.  Otherwise, generate polylines
     * from the three control points.
     */
    deltaX = midPoint.x - p1.x;
    deltaY = midPoint.y - p1.y;
    if (deltaX * deltaX + deltaY * deltaY > chordalDeviationSquared)
    {
        if( !MakeLinesFromArc( ofc, pLoop, pPrim, 
                               p0,
                               p01,
                               midPoint,
                               chordalDeviationSquared) )
            return WFO_FAILURE;

        if( !MakeLinesFromArc( ofc, pLoop, pPrim, 
                               midPoint,
                               p12,
                               p2,
                               chordalDeviationSquared) )
            return WFO_FAILURE;
    }
    else
    {
        /*
         * The "pen" is already at (x0, y0), so we don't need to
         * add that point to the LineBuf.
         */
        if( !AppendToVertBuf( pLoop, pPrim, &p1 ) )
            return WFO_FAILURE;
    }

    return WFO_SUCCESS;
}


/*****************************************************************************
 * ApplyVertexFilter
 *
 * Filter the vertex buffer to get rid of redundant vertices.
 * These can occur on Primitive boundaries.

*****************************************************************************/
static void ApplyVertexFilter( LOOP_LIST *pLoopList )
{
    DWORD nLoops;
    LOOP *pLoop;

    nLoops = pLoopList->nLoops;
    pLoop = pLoopList->LoopBuf;

    for( ; nLoops; nLoops--, pLoop++ ) {
        CheckRedundantVertices( pLoop );
    }
}

/*****************************************************************************
 * CheckRedundantVertices
 *
 * Check for redundant vertices on Curve-Curve boundaries (including loop
 * closure), and get rid of them, using in-place algorithm.

*****************************************************************************/

static void CheckRedundantVertices( LOOP  *pLoop )
{
    PRIM *pPrim, *pNextPrim; 
    DWORD nVerts;
    BOOL bEliminate, bLastEliminate;
    DWORD nEliminated=0, nPrims;
    POINT2D *pVert, *pVert2ndToLast;
    
    nPrims = pLoop->nPrims;
    if( nPrims < 2 )
        return;

    pPrim = pLoop->PrimBuf;
    pNextPrim = pPrim + 1;
    
    nPrims--; // the last prim is dealt with afterwards
    for( ; nPrims; nPrims--, pPrim = pNextPrim++ ) {
        bEliminate = FALSE;
        nVerts = pPrim->nVerts;

        // check spline<->* boundaries
        if( (pPrim->nVerts >= 2) &&
            ((pPrim->primType     == PRIM_CURVE ) || 
             (pNextPrim->primType == PRIM_CURVE )) ) {

            /* get ptr to 2nd-to-last vertex in current prim 
             * !! Note that last vertex in current prim and first vertex in
             *  next prim are the same.
             */
            pVert2ndToLast = pPrim->pVert + pPrim->nVerts - 2;
            if( PointsColinear( pVert2ndToLast, 
                                pVert2ndToLast+1,
                                pNextPrim->pVert+1 ) ) {
                // we eliminate last vertex in current prim
                bEliminate = TRUE;
                pPrim->nVerts--; 
                nVerts--;
            }
        }

        /* move vertices up in vertBuf if necessary (if any vertices
         * were PREVIOUSLY eliminated)
         */
        if( nEliminated ) {
            pVert = pPrim->pVert - nEliminated; // new pVert
            memcpy( pVert+1, pPrim->pVert+1, (nVerts-1)*sizeof(POINT2D));
            pPrim->pVert = pVert;
        }
        if( bEliminate ) {
            nEliminated += 1;
        }
    }

    /* also check for redundancy at closure:
     * - replace firstPrim's first vertex with 2nd-to-last of last prim
     * - eliminate last vertex in last prim
     */
    bLastEliminate = bEliminate;
    bEliminate = FALSE;
    nVerts = pPrim->nVerts;
    pNextPrim = pLoop->PrimBuf; // first prim in loop

    if( (pPrim->nVerts >= 2) &&
        ((pPrim->primType     == PRIM_CURVE ) || 
         (pNextPrim->primType == PRIM_CURVE )) ) {

        POINT2D *pVertLast;

        pVert2ndToLast = pPrim->pVert + pPrim->nVerts - 2; // always >=2 verts
        pVertLast = pVert2ndToLast + 1;

        if( (pPrim->nVerts == 2) && bLastEliminate )
            /* 2ndToLast vert (same as first vert) of this prim has
             * been eliminated.  Deal with it by backing up the ptr.
             * This didn't matter in above loop, because there wasn't the
             * possibility of munging the first vertex in the loop
             */
            pVert2ndToLast--;

        // point to 2nd-to-last vertex in prim
        if( PointsColinear( pVert2ndToLast, 
                            pVertLast,
                            pNextPrim->pVert+1 ) ) {
            bEliminate = TRUE;
            pPrim->nVerts--; 
            // munge first prim's first vertex
            /* problem here if have 2 eliminations in a row, and pPrim was
             * a 2 vertex prim - then pVert2ndToLast is pointing to an
             * eliminated vertex
             */
            *(pNextPrim->pVert) = *(pVert2ndToLast);
            nVerts--;
        }
    }

    // move up last prim's vertices if necessary
    if( nEliminated ) {
        pVert = pPrim->pVert - nEliminated; // new pVert
        memcpy( pVert+1, pPrim->pVert+1, (nVerts-1)*sizeof(POINT2D) );
        // This misses copying one vertex
        pPrim->pVert = pVert;
    }

    if( bEliminate ) {
        nEliminated += 1;
    }

    // now update vertex count in Loop
    pLoop->nVerts -= nEliminated;

    // Check for prims with nVerts=1 (invalidated), and remove them

    nPrims = pLoop->nPrims;
    pPrim = pLoop->PrimBuf;
    nEliminated = 0;
    for( ; nPrims; nPrims--, pPrim++ ) {
        if( pPrim->nVerts == 1 ) {
            nEliminated++;
            continue;
        }
        *(pPrim-nEliminated) = *pPrim;
    }
    pLoop->nPrims -= nEliminated;
}

/*****************************************************************************
 * PointsColinear
 *
 * Returns TRUE if the 3 points are colinear enough.

*****************************************************************************/

static BOOL PointsColinear( POINT2D *p1,
                            POINT2D *p2,
                            POINT2D *p3 )
{
    POINT2D v1, v2;

    // compare slopes of the 2 vectors? - optimize later
    if( POINT2DEQUAL( p1, p2 ) || POINT2DEQUAL( p2, p3 ) )
        // avoid sending 0 vector to CalcAngle (generates FPE)
        return TRUE;

    v1.x = p2->x - p1->x;
    v1.y = p2->y - p1->y;
    v2.x = p3->x - p2->x;
    v2.y = p3->y - p2->y;
    if( fabs(CalcAngle( &v1, &v2 )) < CoplanarThresholdAngle )
        return TRUE;

    return FALSE;
}


/*****************************************************************************
 * CreateOFContext
 *
 * Create and initialize the outline font context.
 *
 * History:
 *  26-Sep-1995 -by- Marc Fortier [marcfo]
 * Use extrusioniser to draw polygonal faces with extrusion=0

*****************************************************************************/

static HRESULT CreateOFContext( HDC    hdc,
                                   FLOAT  chordalDeviation,
                                   FLOAT  extrusion, 
                                   INT    format,
                                   OFContext **ppofc
                                   )
{
    OFContext *ofc = (OFContext *) NULL;
    HRESULT hr = S_OK;
 
    // validate parameters

    if( (format != WGL_FONT_LINES) && (format != WGL_FONT_POLYGONS) ) 
    {
        WARNING("D3DXUseFontOutlines: invalid format parameter\n");
        hr = E_INVALIDARG;
        goto e_Exit;
    }


    ofc = (OFContext *) ALLOCZ( sizeof(OFContext) );
    if (ofc == NULL) 
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    ofc->format = format;
    ofc->chordalDeviation = chordalDeviation;

    if( !ScaleFont( hdc, ofc, TRUE) )
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // handle extrusion
    ofc->ec = extr_Init( extrusion, format );
    if(ofc->ec == NULL) 
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // init a tess obj
    ofc->tess = NULL;
    //if( ofc->format == WGL_FONT_POLYGONS ) 
    D3DXASSERT(ofc->format == WGL_FONT_POLYGONS);
    {
        GLUtesselator *tess;

        tess = gluNewTess();
        if (tess == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // insert own callbacks here
        gluTessCallback(tess, GLU_TESS_BEGIN_DATA,  
                                (void(CALLBACK*)()) extr_glBegin);
        gluTessCallback(tess, GLU_TESS_END,    
                                (void(CALLBACK*)()) extr_glEnd);
        gluTessCallback(tess, GLU_TESS_VERTEX_DATA, 
                                (void(CALLBACK*)()) extr_glVertex);

        gluTessCallback(tess, GLU_TESS_ERROR_DATA,        
                                        (void(CALLBACK*)()) TessError);
        gluTessCallback(tess, GLU_TESS_COMBINE_DATA, 
                                        (void(CALLBACK*)()) TessCombine);

        // set tesselator normal and winding rule

        gluTessNormal( tess, 0.0, 0.0, 1.0 );
        gluTessProperty( tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);

        ofc->tess = tess;
    }

    *ppofc = ofc;
    ofc = NULL;

e_Exit:
    if (ofc != NULL)
    {
        DestroyOFContext( hdc, ofc );
    }

    return hr;
}

/*****************************************************************************
* ScaleFont
*
* To get the best representation of the font, we use its design height, or
* the emSquare size.  We then scale emSquare to 1.0.
* A maxChordTolerance value is set, otherwise it was found that some
* glyphs displayed ugly loop intersections.  The value .035f was chosen
* after cursory examination of the glyphs. 
*
* History:
*  31-Jul-1995 -by- [marcfo]
* Get rid of unicode functions - since we're just accessing text metrics,
* the default 'string' functions should work on all platforms.
*****************************************************************************/

static BOOL
ScaleFont( HDC hdc, OFContext *ofc, BOOL bUnicode )
{
    OUTLINETEXTMETRIC otm;
    HFONT       hfont;
    LOGFONT    lf;
    FLOAT       scale, maxChordTolerance=0.035f;
    UINT        otmEMSquare;

    // Query font metrics

    if( GetOutlineTextMetrics( hdc, sizeof(otm), &otm) <= 0 )
    {
        WARNING("D3DXGetOutlineTextMetrics failed.  Probably no font selected\n");
        // cmd failed, or buffer size=0
        return WFO_FAILURE;
    }

    otmEMSquare = otm.otmEMSquare;

    /*
     * The font data is scaled, so that 1.0 maps to the font's em square
     * size.  Note that it is still possible for glyphs to extend beyond
     * this square.
     */
    scale = 1.0f / (FLOAT) otmEMSquare;

    // create new font object, using largest size

    hfont = (HFONT)GetCurrentObject( hdc, OBJ_FONT );
    GetObject( hfont, sizeof(LOGFONT), &lf );
    lf.lfHeight = otmEMSquare;
    lf.lfWidth = 0;  // this will choose default width for the height
    hfont = CreateFontIndirect(&lf);

    // select new font into DC, and save current font
    ofc->hfontOld = (HFONT)SelectObject( hdc, hfont );

    // set ofc values

    ofc->scale = scale;

    /* check chord tolerance: in design space, minimum chord tolerance is
     * ~1 logical unit, = ofc->scale.
     */
    if( ofc->chordalDeviation == 0.0f ) {
        // select minimum tolerance in this case
        ofc->chordalDeviation = ofc->scale;
    }
    /* also impose a maximum, or things can get ugly */
    else if( ofc->chordalDeviation > maxChordTolerance ) {
        // XXX might want to change maxChordTolerance based on scale ?
        ofc->chordalDeviation = maxChordTolerance;
    }

    return WFO_SUCCESS;
}

/*****************************************************************************
 * DestroyOFContext
 *
*****************************************************************************/

static void 
DestroyOFContext( HDC hdc, OFContext* ofc )
{
    HFONT hfont;

    if( ofc->ec ) {
        extr_Finish( ofc->ec );
    }

    // put back original font object
    if( ofc->hfontOld ) {
        hfont = (HFONT)SelectObject( hdc, ofc->hfontOld );
        DeleteObject( hfont );
    }

    if( ofc->format == WGL_FONT_POLYGONS ) {
        if( ofc->tess )
            gluDeleteTess( ofc->tess );
    }

    FREE( ofc );
}

/*****************************************************************************
 * InitLoopBuf
 *
 * Initializes a LOOP_LIST structure for the Loops of each glyph.

*****************************************************************************/

static LOOP_LIST*
InitLoopBuf( void )
{
    LOOP *pLoop;
    LOOP_LIST *pLoopList;
    DWORD initSize = 10;

    pLoopList = (LOOP_LIST*) ALLOC( sizeof(LOOP_LIST) );
    if( !pLoopList )
        return( (LOOP_LIST *) NULL );

    pLoop = (LOOP*) ALLOC( initSize * sizeof(LOOP) );
    if( !pLoop ) {
        FREE( pLoopList );
        return( (LOOP_LIST *) NULL );
    }

    pLoopList->LoopBuf = pLoop;
    pLoopList->nLoops = 0;
    pLoopList->LoopBufSize = initSize;

    return pLoopList; 
}

/*****************************************************************************
 * NewLoop
 * 
 * Create a new LOOP structure.  The first point in the loop is supplied.

*****************************************************************************/

static LOOP*
NewLoop( LOOP_LIST *pLoopList, POINT2D *pFirstPoint )
{
    LOOP    *pNewLoop;
    PRIM    *pPrim;
    POINT2D *pVert;
    DWORD   size = 50;

    if( pLoopList->nLoops >=  pLoopList->LoopBufSize)
    {
        // need to increase size of LoopBuf
        LOOP *pLoop;

        pLoop = (LOOP*) REALLOC(pLoopList->LoopBuf,  
                                (pLoopList->LoopBufSize += size) *
                                sizeof(LOOP));
        if( !pLoop )
            return (LOOP *) NULL;
        pLoopList->LoopBuf = pLoop;
    }

    pNewLoop = pLoopList->LoopBuf + pLoopList->nLoops;

    // give the loop a block of prims to work with
    pPrim = (PRIM *) ALLOC( size * sizeof(PRIM) );
    if( !pPrim )
        return (LOOP *) NULL;
    pNewLoop->PrimBuf = pPrim;
    pNewLoop->nPrims = 0;
    pNewLoop->PrimBufSize = size;

    // give the loop a block of vertices to work with
    pVert = (POINT2D*) ALLOC( size * sizeof(POINT2D) );
    if( !pVert ) {
        FREE( pPrim );
        return (LOOP *) NULL;
    }
    pNewLoop->VertBuf = pVert;
    pNewLoop->nVerts = 0;
    pNewLoop->VertBufSize = size;

    // stick that first point in
    pVert->x = pFirstPoint->x;
    pVert->y = pFirstPoint->y;
    pNewLoop->nVerts++;

    // normal buffers - used by extrusion
    pNewLoop->FNormBuf = (POINT3D *) NULL;
    pNewLoop->VNormBuf = (POINT3D *) NULL;

    pLoopList->nLoops++; // increment loop count

    return pNewLoop;
}

/*****************************************************************************
 * NewPrim
 *
 * Create a new PRIM structure.  The primType is supplied.

*****************************************************************************/

static PRIM*
NewPrim( LOOP *pLoop, DWORD primType )
{
    PRIM    *pNewPrim;
    DWORD   size = 50;

    if( pLoop->nPrims >=  pLoop->PrimBufSize)
    {
        // need to increase size of PrimBuf
        PRIM *pPrim;

        pPrim = (PRIM *) REALLOC(pLoop->PrimBuf,  
                                 (pLoop->PrimBufSize += size) * sizeof(PRIM));
        if( !pPrim )
            return (PRIM *) NULL;
        pLoop->PrimBuf = pPrim;
    }

    pNewPrim = pLoop->PrimBuf + pLoop->nPrims;
    // translate primType to extrusion prim type
    primType = (primType == TT_PRIM_LINE) ? PRIM_LINE : PRIM_CURVE;
    pNewPrim->primType = primType;
    pNewPrim->nVerts = 1;  // since we include last point:
    /* 
     * VertIndex must point to the last point of the previous prim
     */
    pNewPrim->VertIndex = pLoop->nVerts - 1;
    // normal pointers - used by extrusion
    pNewPrim->pFNorm = (POINT3D *) NULL;
    pNewPrim->pVNorm = (POINT3D *) NULL;

    pLoop->nPrims++; // increment prim count

    return pNewPrim;
}

/*****************************************************************************
 * FreeLoopList
 *
 * Free up all memory associated with processing a glyph.
 *
*****************************************************************************/

static void
FreeLoopList( LOOP_LIST *pLoopList )
{
    DWORD nLoops;

    if( !pLoopList )
        return;

    if( pLoopList->LoopBuf ) {
        // free up each loop
        LOOP *pLoop = pLoopList->LoopBuf;

        nLoops = pLoopList->nLoops;
        for( ; nLoops; nLoops--, pLoop++ ) {
            if( pLoop->PrimBuf )
                FREE( pLoop->PrimBuf );
            if( pLoop->VertBuf )
                FREE( pLoop->VertBuf );
        } 
        FREE( pLoopList->LoopBuf );
    }
    FREE( pLoopList );
}

/*****************************************************************************
 * AppendToVertBuf
 *
 * Append a vertex to the Loop's VertBuf

*****************************************************************************/

static BOOL
AppendToVertBuf( LOOP      *pLoop,
                 PRIM      *pPrim,
                 POINT2D   *p )
{
    if( pLoop->nVerts >=  pLoop->VertBufSize)
    {
        POINT2D *vertBuf;
        DWORD   size = 100;

        vertBuf = (POINT2D *) REALLOC(pLoop->VertBuf,
                                      (pLoop->VertBufSize += size) *
                                      sizeof(POINT2D));
        if( !vertBuf )
            return WFO_FAILURE;
        pLoop->VertBuf = vertBuf;
    }
    pLoop->VertBuf[pLoop->nVerts] = *p;
    pLoop->nVerts++;
    pPrim->nVerts++;
    return WFO_SUCCESS;
}

/*****************************************************************************
 * CalcVertPtrs
 *
 * Calculate vertex ptrs from index values for the prims in a loop.

*****************************************************************************/

static void
CalcVertPtrs( LOOP *pLoop )
{
    DWORD nPrims;
    PRIM  *pPrim;

    nPrims = pLoop->nPrims;
    pPrim = pLoop->PrimBuf;

    for( ; nPrims; pPrim++, nPrims-- ) {
        pPrim->pVert = pLoop->VertBuf + pPrim->VertIndex;
    }
}


/*****************************************************************************
 * GetFixed
 *
 * Fetch the next 32-bit fixed-point value from a little-endian byte stream,
 * convert it to floating-point, and increment the stream pointer to the next
 * unscanned byte.

*****************************************************************************/

static FLOAT GetFixed(UCHAR** p)
{
    FLOAT value;
    FLOAT fraction;

    fraction = ((FLOAT) (UINT) GetWord(p)) / 65536.0f;
    value    = (FLOAT) GetSignedWord(p);

    return value+fraction;
}

/*****************************************************************************
 *      CFaceContext implementation
*****************************************************************************/

/*****************************************************************************
 * CFaceContext Constructor, just setup so that delete works
 *
*****************************************************************************/
CFaceContext::CFaceContext()
   :m_rgvVertices(NULL),
    m_cVertices(0),
    m_cVerticesMax(0),
    m_rgFaces(NULL),
    m_cFaces(0),
    m_cFacesMax(0),
    m_vCurrentNormal(0.0f,0.0f,0.0f),
    m_fCurrentZ(0.0f),
	m_wch(0)
{
}

/*****************************************************************************
 * CFaceContext Destructor, just blow away allocated memory
 *
*****************************************************************************/
CFaceContext::~CFaceContext()
{
    delete []m_rgvVertices;
    delete []m_rgFaces;
}

/*****************************************************************************
 * CFaceContext Init - resets face and vertex counts
 *                  if this is the first init it also pre-allocates arrays
 *
*****************************************************************************/
HRESULT 
CFaceContext::Init()
{
    m_cFaces = 0;
    m_cVertices = 0;

    if (m_cFacesMax == 0)
    {
        m_cFacesMax = x_cFaceContextDefaultSize;
        m_rgFaces = new SFCFace16[m_cFacesMax];
        if (m_rgFaces == NULL)
            return E_OUTOFMEMORY;
    }

    if (m_cVerticesMax == 0)
    {
        m_cVerticesMax = x_cFaceContextDefaultSize;
        m_rgvVertices = new SFCVertex[m_cVerticesMax];
        if (m_rgvVertices == NULL)
            return E_OUTOFMEMORY;
    }

    return S_OK;
}

/*****************************************************************************
 * CFaceContext ResizeFaces - resize the face array to a new maximum
 *
*****************************************************************************/
HRESULT CFaceContext::ResizeFaces(UINT cNewMax)
{
    SFCFace16 *rgFacesNew;
    D3DXASSERT(cNewMax > m_cFacesMax);

    m_cFacesMax = cNewMax;
    rgFacesNew = new SFCFace16[m_cFacesMax];
    if (m_rgFaces == NULL)
        return E_OUTOFMEMORY;

    memcpy(rgFacesNew, m_rgFaces, sizeof(SFCFace16) * m_cFaces);

    delete []m_rgFaces;
    m_rgFaces = rgFacesNew;

    return S_OK;
}

/*****************************************************************************
 * CFaceContext ResizeVertices - resize the vertex array to a new maximum
 *
*****************************************************************************/
HRESULT 
CFaceContext::ResizeVertices(UINT cNewMax)
{
    SFCVertex *rgVerticesNew;
    D3DXASSERT(cNewMax > m_cVerticesMax);

    m_cVerticesMax = cNewMax;
    rgVerticesNew = new SFCVertex[m_cVerticesMax];
    if (rgVerticesNew == NULL)
        return E_OUTOFMEMORY;

    memcpy(rgVerticesNew, m_rgvVertices, sizeof(SFCVertex) * m_cVertices);

    delete []m_rgvVertices;
    m_rgvVertices = rgVerticesNew;

    return S_OK;
}

/*****************************************************************************
 * CFaceContext AddFace - adds the face to the face array
 *
*****************************************************************************/
void
CFaceContext::AddFace
(
    SFCFace16 *pface
)
{
    D3DXASSERT(m_cFaces < m_cFacesMax);
    m_rgFaces[m_cFaces] = *pface;
    m_cFaces++;
}

/*****************************************************************************
 * CFaceContext::SetupPrim - Helper function for the triangle prim methods
 *              Adds given vertices and resizes the arrays
 *
*****************************************************************************/
HRESULT CFaceContext::SetupPrim
(
    D3DXVECTOR2 *rgvVertices, 
    UINT cVertices,
    UINT cTriangles,
    UINT *piFirstIndex
)
{
    UINT iVertex;
    UINT iCurIndex;
    HRESULT hr;

    if (cVertices + m_cVertices > m_cVerticesMax)
    {
        hr = ResizeVertices(cVertices + m_cVertices);
        if (FAILED(hr))
            return hr;
    }

    if (cTriangles + m_cFaces > m_cFacesMax)
    {
        hr = ResizeFaces(cTriangles + m_cFaces);
        if (FAILED(hr))
            return hr;
    }

    *piFirstIndex = m_cVertices;

    // load the data into the vertex buffer
    for (iVertex = 0, iCurIndex = m_cVertices; iVertex < cVertices; iVertex++, iCurIndex++)
    {
        m_rgvVertices[iCurIndex].vPosition.x = rgvVertices[iVertex].x;
        m_rgvVertices[iCurIndex].vPosition.y = rgvVertices[iVertex].y;
        m_rgvVertices[iCurIndex].vPosition.z = m_fCurrentZ;

        m_rgvVertices[iCurIndex].vNormal = m_vCurrentNormal;
    }

    m_cVertices += cVertices;

    return S_OK;
}

// arrays used to remap based on triangle orientation
const UINT x_rgiCW[3]  = { 0, 1, 2 };
const UINT x_rgiCCW[3] = { 0, 2, 1 };

/*****************************************************************************
 * CFaceContext::AddStrip - Adds the given strip to the face and vertex array
 *
*****************************************************************************/
HRESULT 
CFaceContext::AddStrip
(
    D3DXVECTOR2 *rgvVertices, 
    UINT cVertices,
    BOOL bCCW
)
{
    HRESULT hr;
    UINT cTriangles = cVertices - 2;
    UINT iFirstIndex;
    UINT iCurIndex;
    UINT iTriangle;
    SFCFace16 face;
    const UINT *rgiIndex = bCCW ? x_rgiCCW : x_rgiCW;

    hr = SetupPrim(rgvVertices, cVertices, cTriangles, &iFirstIndex);
    if (FAILED(hr))
        return hr;

    for (iTriangle = 0, iCurIndex = iFirstIndex; iTriangle < cTriangles; iTriangle++, iCurIndex++)
    {
        // if even
        if ((iTriangle & 0x1) == 1)
        {
            face.rgwIndices[rgiIndex[0]] = static_cast<WORD>(iCurIndex + 1);
            face.rgwIndices[rgiIndex[1]] = static_cast<WORD>(iCurIndex + 2);
            face.rgwIndices[rgiIndex[2]] = static_cast<WORD>(iCurIndex);
        }
        else // if odd
        {
            face.rgwIndices[rgiIndex[0]] = static_cast<WORD>(iCurIndex);
            face.rgwIndices[rgiIndex[1]] = static_cast<WORD>(iCurIndex + 2);
            face.rgwIndices[rgiIndex[2]] = static_cast<WORD>(iCurIndex + 1);
        }

        AddFace(&face);
    }

    return S_OK;
}

/*****************************************************************************
 * CFaceContext::AddTriangle - Adds the triangle list to the face and vertex array
 *
*****************************************************************************/
HRESULT 
CFaceContext::AddTriangle
(
    D3DXVECTOR2 *rgvVertices, 
    UINT cVertices,
    BOOL bCCW
)
{
    HRESULT hr;
    UINT cTriangles = cVertices / 3;
    UINT iFirstIndex;
    UINT iCurIndex;
    UINT iTriangle;
    SFCFace16 face;
    const UINT *rgiIndex = bCCW ? x_rgiCCW : x_rgiCW;

    hr = SetupPrim(rgvVertices, cVertices, cTriangles, &iFirstIndex);
    if (FAILED(hr))
        return hr;

    for (iTriangle = 0, iCurIndex = iFirstIndex; iTriangle < cTriangles; iTriangle++, iCurIndex += 3)
    {
        face.rgwIndices[rgiIndex[0]] = static_cast<WORD>(iCurIndex);
        face.rgwIndices[rgiIndex[1]] = static_cast<WORD>(iCurIndex + 2);
        face.rgwIndices[rgiIndex[2]] = static_cast<WORD>(iCurIndex + 1);

        AddFace(&face);
    }

    return S_OK;
}

/*****************************************************************************
 * CFaceContext::AddFan - Adds the given fan to the face and vertex array
 *
*****************************************************************************/
HRESULT 
CFaceContext::AddFan
(
    D3DXVECTOR2 *rgvVertices, 
    UINT cVertices,
    BOOL bCCW
)
{
    HRESULT hr;
    UINT cTriangles = cVertices - 2;
    UINT iFirstIndex;
    UINT iCurIndex;
    UINT iTriangle;
    SFCFace16 face;
    const UINT *rgiIndex = bCCW ? x_rgiCCW : x_rgiCW;

    hr = SetupPrim(rgvVertices, cVertices, cTriangles, &iFirstIndex);
    if (FAILED(hr))
        return hr;

    for (iTriangle = 0, iCurIndex = iFirstIndex + 1; iTriangle < cTriangles; iTriangle++, iCurIndex++)
    {
        face.rgwIndices[rgiIndex[0]] = static_cast<WORD>(iFirstIndex);
        face.rgwIndices[rgiIndex[1]] = static_cast<WORD>(iCurIndex + 1);
        face.rgwIndices[rgiIndex[2]] = static_cast<WORD>(iCurIndex);

        AddFace(&face);
    }

    return S_OK;
}

/*****************************************************************************
 * CFaceContext::AddQuad - Adds the given quad to the face and vertex array
 *
*****************************************************************************/
HRESULT 
CFaceContext::AddQuad
(
    D3DXVECTOR3 *rgvVertices, 
    D3DXVECTOR3 *rgvNorm,
    BOOL bCCW
)
{
    HRESULT hr;
    UINT iFirstIndex = m_cVertices;
    UINT iCurIndex;
    UINT iVertex;
    const UINT *rgiIndex = bCCW ? x_rgiCCW : x_rgiCW;

    if (4 + m_cVertices > m_cVerticesMax)
    {
        hr = ResizeVertices(4 + m_cVertices);
        if (FAILED(hr))
            return hr;
    }

    if (2 + m_cFaces > m_cFacesMax)
    {
        hr = ResizeFaces(2 + m_cFaces);
        if (FAILED(hr))
            return hr;
    }

    // load the data into the vertex buffer
    for (iVertex = 0, iCurIndex = m_cVertices; iVertex < 4; iVertex++, iCurIndex++)
    {
        m_rgvVertices[iCurIndex].vPosition = rgvVertices[iVertex];
        m_rgvVertices[iCurIndex].vNormal = rgvNorm[iVertex];
    }

    m_rgFaces[m_cFaces].rgwIndices[rgiIndex[0]] = static_cast<WORD>(m_cVertices);
    m_rgFaces[m_cFaces].rgwIndices[rgiIndex[1]] = static_cast<WORD>(m_cVertices + 2);
    m_rgFaces[m_cFaces].rgwIndices[rgiIndex[2]] = static_cast<WORD>(m_cVertices + 1);
    m_cFaces++;

    m_rgFaces[m_cFaces].rgwIndices[rgiIndex[0]] = static_cast<WORD>(m_cVertices);
    m_rgFaces[m_cFaces].rgwIndices[rgiIndex[1]] = static_cast<WORD>(m_cVertices + 3);
    m_rgFaces[m_cFaces].rgwIndices[rgiIndex[2]] = static_cast<WORD>(m_cVertices + 2);
    m_cFaces++;

    m_cVertices += 4;

    return S_OK;
}

/*****************************************************************************
 * CFaceContext::AddStrip - Adds the given quad strip to the face and vertex array
 *
*****************************************************************************/
HRESULT 
CFaceContext::AddQuadStrip
(
    D3DXVECTOR3 *rgvVertices, 
    D3DXVECTOR3 *rgvNorm,
    UINT cVertices,
    BOOL bCCW
)
{
    HRESULT hr;
    UINT iFirstIndex = m_cVertices;
    UINT iCurIndex;
    UINT iVertex;
    UINT cTriangles = cVertices - 2;
    UINT cQuads = cTriangles / 2;
    UINT iQuad;
    const UINT *rgiIndex = bCCW ? x_rgiCCW : x_rgiCW;

    if (cVertices + m_cVertices > m_cVerticesMax)
    {
        hr = ResizeVertices(cVertices + m_cVertices);
        if (FAILED(hr))
            return hr;
    }

    if (cTriangles + m_cFaces > m_cFacesMax)
    {
        hr = ResizeFaces(cTriangles + m_cFaces);
        if (FAILED(hr))
            return hr;
    }

    // load the data into the vertex buffer
    for (iVertex = 0, iCurIndex = m_cVertices; iVertex < cVertices; iVertex++, iCurIndex++)
    {
        m_rgvVertices[iCurIndex].vPosition = rgvVertices[iVertex];
        m_rgvVertices[iCurIndex].vNormal = rgvNorm[iVertex];
    }

    for (iQuad = 0, iCurIndex = m_cVertices; iQuad < cQuads; iQuad++, iCurIndex += 2)
    {
        m_rgFaces[m_cFaces].rgwIndices[rgiIndex[0]] = static_cast<WORD>(iCurIndex);
        m_rgFaces[m_cFaces].rgwIndices[rgiIndex[1]] = static_cast<WORD>(iCurIndex + 3);
        m_rgFaces[m_cFaces].rgwIndices[rgiIndex[2]] = static_cast<WORD>(iCurIndex + 1);
        m_cFaces++;

        m_rgFaces[m_cFaces].rgwIndices[rgiIndex[0]] = static_cast<WORD>(iCurIndex);
        m_rgFaces[m_cFaces].rgwIndices[rgiIndex[1]] = static_cast<WORD>(iCurIndex + 2);
        m_rgFaces[m_cFaces].rgwIndices[rgiIndex[2]] = static_cast<WORD>(iCurIndex + 3);
        m_cFaces++;
    }

    m_cVertices += cVertices;
    return S_OK;
}

/*****************************************************************************
 * CFaceContext::Translate - Translates the vertices by the provided vector
 *
*****************************************************************************/
HRESULT 
CFaceContext::Translate
(
    D3DXVECTOR3 *pvTrans
)
{
    UINT iVertex;

    // just add the vector to all positions
    for (iVertex = 0; iVertex < m_cVertices; iVertex++)
    {
        m_rgvVertices[iVertex].vPosition += *pvTrans;
    }

    return S_OK;
}

/*****************************************************************************
 * CFaceContext::CopyMesh - Copy the face and vertex data into the provided buffers
 *
*****************************************************************************/
HRESULT 
CFaceContext::CopyMesh
(
    SFCFace16 *rgFaces, 
    SFCVertex *rgvVertices
)
{
    memcpy(rgFaces, m_rgFaces, sizeof(SFCFace16) * m_cFaces);
    memcpy(rgvVertices, m_rgvVertices, sizeof(SFCVertex) * m_cVertices);

    return S_OK;
}
/*****************************************************************************
 * GenerateMesh - Generates a mesh that is the concatenation of all the
 *                  face contexts provided
 *
*****************************************************************************/
HRESULT 
GenerateMesh
(
    CFaceContext **rgpfcLetters,
    UINT cLetters,
    CGlyphObject* pGlyphObject
)
{
    HRESULT hr = S_OK;
/*
    LPD3DXMESH ptmMesh = NULL;
    UINT cFaces = 0;
    UINT cVertices = 0;
    LPDIRECT3DVERTEXBUFFER8 pVertexBuffer = NULL;
    SFCVertex *pvVertices = NULL;
    SFCVertex *pvVerticesCur;
    SFCFace16 *rgFaces = NULL;
    SFCFace16 *rgFacesCur;
    UINT iCurVertOffset;
    UINT cFacesCur;
    UINT cVerticesCur;
    UINT iFace;
*/
    UINT iLetter;

    D3DXASSERT(rgpfcLetters != NULL);
    D3DXASSERT(cLetters != 0);
/*
    D3DXASSERT(pDevice != NULL);
    D3DXASSERT(pptmMesh != NULL);

    // first find the face/vertex totals
    for (iLetter = 0; iLetter < cLetters; iLetter++)
    {
        D3DXASSERT(rgpfcLetters[iLetter] != NULL);

        cFaces += rgpfcLetters[iLetter]->CFaces();
        cVertices += rgpfcLetters[iLetter]->CVertices();
    }

    // next create the mesh and get the vertex buffer to copy into
    hr = D3DXCreateMeshFVF(cFaces, cVertices, D3DXMESH_MANAGED, D3DFVF_XYZ | D3DFVF_NORMAL, pDevice, &ptmMesh);
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmMesh->GetVertexBuffer(&pVertexBuffer);
    if (FAILED(hr))
        goto e_Exit;

    hr = pVertexBuffer->Lock(0,0, (PBYTE*)&pvVertices, D3DLOCK_NOSYSLOCK );
    if (FAILED(hr))
        goto e_Exit;

    hr = ptmMesh->LockIndexBuffer(0, (PBYTE*)&rgFaces);
    if (FAILED(hr))
        goto e_Exit;
*/

    // now actually get the data from the face contexts into the mesh
/*
    rgFacesCur = rgFaces;
    pvVerticesCur = pvVertices;
    iCurVertOffset = 0;
*/
    for (iLetter = 0; iLetter < cLetters; iLetter++)
    {
		PRINT("DEF ch_%04x IndexedFaceSet\n{\n", rgpfcLetters[iLetter]->m_wch);

		PRINT("\tcoord Coordinate\n\t{\n\t\tpoint\n\t\t[\n");
		for (UINT i = 0; i < rgpfcLetters[iLetter]->m_cVertices; i += 1)
		{
			PRINT("\t\t\t%f %f %f,\n", rgpfcLetters[iLetter]->m_rgvVertices[i].vPosition.x, rgpfcLetters[iLetter]->m_rgvVertices[i].vPosition.y, rgpfcLetters[iLetter]->m_rgvVertices[i].vPosition.z);
		}
		PRINT("\t\t]\n\t}\n");

		PRINT("\tnormal Normal\n\t{\n\t\tvector\n\t\t[\n");
		for (i = 0; i < rgpfcLetters[iLetter]->m_cVertices; i += 1)
		{
			PRINT("\t\t\t%f %f %f,\n", rgpfcLetters[iLetter]->m_rgvVertices[i].vNormal.x, rgpfcLetters[iLetter]->m_rgvVertices[i].vNormal.y, rgpfcLetters[iLetter]->m_rgvVertices[i].vNormal.z);
		}
		PRINT("\t\t]\n\t}\n");

		PRINT("\tcoordIndex\n\t[\n");
		for (i = 0; i < rgpfcLetters[iLetter]->m_cFaces; i += 1)
		{
			PRINT("\t\t%d %d %d\n", 
				rgpfcLetters[iLetter]->m_rgFaces[i].rgwIndices[0],
				rgpfcLetters[iLetter]->m_rgFaces[i].rgwIndices[1],
				rgpfcLetters[iLetter]->m_rgFaces[i].rgwIndices[2]
				);
		}
		PRINT("\t]\n");

/*
        cFacesCur = rgpfcLetters[iLetter]->CFaces();
        cVerticesCur = rgpfcLetters[iLetter]->CVertices();

        // first get the data out of the face context into the mesh
        hr = rgpfcLetters[iLetter]->CopyMesh(rgFacesCur, pvVerticesCur);
        if (FAILED(hr))
            goto e_Exit;

        // now fixup the faces to point at the current vertices
        for (iFace = 0; iFace < cFacesCur; iFace++)
        {
            rgFacesCur[iFace].rgwIndices[0] += static_cast<WORD>(iCurVertOffset);
            rgFacesCur[iFace].rgwIndices[1] += static_cast<WORD>(iCurVertOffset);
            rgFacesCur[iFace].rgwIndices[2] += static_cast<WORD>(iCurVertOffset);
        }

        rgFacesCur += cFacesCur;
        pvVerticesCur += cVerticesCur;
        iCurVertOffset += cVerticesCur;
*/
		PRINT("}\n\n");

		CGlyphShape* pGlyphShape = new CGlyphShape;
		pGlyphObject->m_pGlyphShape = pGlyphShape;

		pGlyphShape->m_nVertexCount = (WORD)rgpfcLetters[iLetter]->m_cVertices;
		pGlyphShape->m_vertices = new CGlyphVertex [pGlyphShape->m_nVertexCount];
		for (i = 0; i < pGlyphShape->m_nVertexCount; i += 1)
		{
			pGlyphShape->m_vertices[i].x = rgpfcLetters[iLetter]->m_rgvVertices[i].vPosition.x;
			pGlyphShape->m_vertices[i].y = rgpfcLetters[iLetter]->m_rgvVertices[i].vPosition.y;
		}

		pGlyphShape->m_nIndexCount = (WORD)rgpfcLetters[iLetter]->m_cFaces * 3;
		pGlyphShape->m_indices = new WORD [pGlyphShape->m_nIndexCount];
		for (i = 0; i < rgpfcLetters[iLetter]->m_cFaces; i += 1)
		{
			pGlyphShape->m_indices[i * 3] = rgpfcLetters[iLetter]->m_rgFaces[i].rgwIndices[0];
			pGlyphShape->m_indices[i * 3 + 1] = rgpfcLetters[iLetter]->m_rgFaces[i].rgwIndices[1];
			pGlyphShape->m_indices[i * 3 + 2] = rgpfcLetters[iLetter]->m_rgFaces[i].rgwIndices[2];
		}
    }

/*
    D3DXASSERT(iCurVertOffset == cVertices);
    D3DXASSERT(pvVerticesCur == pvVertices + cVertices);
    D3DXASSERT(rgFacesCur == rgFaces + cFaces);

    *pptmMesh = ptmMesh;
    ptmMesh->AddRef();

e_Exit:
    if (rgFaces != NULL)
    {
        ptmMesh->UnlockIndexBuffer();
    }

    RELEASE(ptmMesh);
    
    if (pvVertices != NULL)
    {
        pVertexBuffer->Unlock();
    }
    RELEASE(pVertexBuffer);
*/

    return hr;
}

} // namespace D3DX



/*****************************************************************************
 * wglUseFontOutlinesAW
 * 
 * Converts a subrange of the glyphs in a TrueType font to OpenGL display
 * lists.
 *
 * History:
 *  15-Dec-1994 -by- Marc Fortier [marcfo]
 * Wrote it.
*****************************************************************************/

HRESULT WINAPI MyCreateTextW
( 
    HDC   hDC,
    LPCWSTR wszText,
    FLOAT chordalDeviation,
    FLOAT extrusion,
    CGlyphObject* pGlyphOut
)
{
    HRESULT     hr = S_OK;
    DWORD       glyphIndex;
    UCHAR*      glyphBuf = NULL;
    DWORD       glyphBufSize = 0;
    DWORD       glyphSize, error;
    D3DX::OFContext*  ofc = NULL;
    GLYPHMETRICS    glyphMetrics;
    static MAT2 matrix =
    {
        {0, 1}, {0, 0},
        {0, 0}, {0, 1}
    };
    int format = WGL_FONT_POLYGONS; // UNDONE UNDONE should other formats be supported?
    D3DX::CFaceContext **rgpfcLetters = NULL;
    D3DXVECTOR3 vTrans(0.0f, 0.0f, 0.0f);

    UINT iIndex;
    UINT cchText = 0;
    

    if ((wszText == NULL) || (wszText[0] == L'\0'))
    {
        WARNING("D3DXUseFontOutlinesAW: No text specified\n");
        hr = E_INVALIDARG;
        goto e_Exit;
    }

    if( chordalDeviation < 0.0f ) {
        WARNING("D3DXUseFontOutlines: invalid deviation parameter\n");
        hr = E_INVALIDARG;
        goto e_Exit;
    }

    if( extrusion < 0.0f ) {
        WARNING("D3DXUseFontOutlines: invalid extrusion parameter\n");
        hr = E_INVALIDARG;
        goto e_Exit;
    }
    
    cchText = wcslen(wszText);
    D3DXASSERT(cchText > 0);

    rgpfcLetters = new D3DX::CFaceContext*[cchText];
    if (rgpfcLetters == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memset(rgpfcLetters, 0, sizeof(D3DX::CFaceContext*) * cchText);

    /*
     * Create font outline context
     */
    hr = D3DX::CreateOFContext(hDC, chordalDeviation, extrusion, format, &ofc);
    if (FAILED(hr)) 
    {
        WARNING("D3DXCreateOFContext failed\n");
        goto e_Exit;
    }

    /*
     * Process each glyph in the given range:
    */
    for (iIndex = 0; iIndex < cchText; iIndex++)
    {
        glyphIndex = wszText[iIndex];

        /*
         * Determine how much space is needed to store the glyph's
         * outlines.  If our glyph buffer isn't large enough,
         * resize it.
         */
        // UNDONE UNDONE - how to do unicode
        glyphSize = GetGlyphOutlineW( hDC, glyphIndex, GGO_NATIVE,
                                      &glyphMetrics, 0, NULL, &matrix );

        if( glyphSize == GDI_ERROR ) 
        {
            WARNING("D3DXGetGlyphOutline() failed\n");
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        if (glyphSize > glyphBufSize)
        {
            if (glyphBuf != NULL)
                FREE(glyphBuf);

            glyphBufSize = glyphSize;
            glyphBuf = (UCHAR*) ALLOC(glyphSize);
            if (glyphBuf == NULL) 
            {
                WARNING("D3DX: Alloc of glyphBuf failed\n");
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
        }


        /*
         * Get the glyph's outlines.
         */
        // UNDONE UNDONE - how to do unicode
        error = GetGlyphOutlineW( hDC, glyphIndex, GGO_NATIVE, 
                    &glyphMetrics, glyphBufSize, glyphBuf, &matrix );

        if( error == GDI_ERROR ) 
        {
            WARNING("D3DX: GetGlyphOutline() failed\n");
            hr = E_FAIL;
            goto e_Exit;
        }

        /*
         * Turn the glyph into a display list:
         */
        ofc->glyphBuf = glyphBuf;
        ofc->glyphSize = glyphSize;

        hr = D3DX::MakeDisplayListFromGlyph(ofc);
        if (FAILED(hr))
        {
            WARNING("D3DXMakeDisplayListFromGlyph() failed\n");
            goto e_Exit;
        }

        // translate the mesh to the correct position, will be 
        //  combined with other characters in the string later
        ofc->ec->pfc->Translate(&vTrans);

        // save off the face context, so that we can generate as many
        //   letters as needed, then generate the final mesh in one go
        rgpfcLetters[iIndex] = ofc->ec->pfc;
		rgpfcLetters[iIndex]->m_wch = (WCHAR)glyphIndex;

        ofc->ec->pfc = new D3DX::CFaceContext();
        if (ofc->ec->pfc == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // update the translation for the next string
        vTrans.x += ofc->scale * (FLOAT) glyphMetrics.gmCellIncX;
        vTrans.y += ofc->scale * (FLOAT) glyphMetrics.gmCellIncY;

        /*
         * Supply scaled glyphMetrics if requested
         */
//        if( pGlyphOut ) 
//        {
            pGlyphOut->m_metrics.gmfBlackBoxX         = ofc->scale * (FLOAT) glyphMetrics.gmBlackBoxX;
            pGlyphOut->m_metrics.gmfBlackBoxY         = ofc->scale * (FLOAT) glyphMetrics.gmBlackBoxY;
            pGlyphOut->m_metrics.gmfptGlyphOrigin.x   = ofc->scale * (FLOAT) glyphMetrics.gmptGlyphOrigin.x;
            pGlyphOut->m_metrics.gmfptGlyphOrigin.y   = ofc->scale * (FLOAT) glyphMetrics.gmptGlyphOrigin.y;
            pGlyphOut->m_metrics.gmfCellIncX          = ofc->scale * (FLOAT) glyphMetrics.gmCellIncX;
            pGlyphOut->m_metrics.gmfCellIncY          = ofc->scale * (FLOAT) glyphMetrics.gmCellIncY;

//            pGlyphOut++;
//        }
    }

    // now actually generate the mesh from each of the letters face contexts
    hr = D3DX::GenerateMesh(rgpfcLetters, cchText, pGlyphOut);
    if (FAILED(hr))
        goto e_Exit;

    /*
     * Clean up temporary storage and return.  If an error occurred,
     * set error flags and return FAILURE status;
     * otherwise just return SUCCESS.
     */

e_Exit:
    if (glyphBuf != NULL)
        FREE(glyphBuf);

    if (ofc != NULL)
        D3DX::DestroyOFContext( hDC, ofc);

    if (rgpfcLetters != NULL)
    {
        for (iIndex = 0; iIndex < cchText; iIndex++)
        {
            delete rgpfcLetters[iIndex];
        }
    }

    delete []rgpfcLetters;


    return hr;
}
