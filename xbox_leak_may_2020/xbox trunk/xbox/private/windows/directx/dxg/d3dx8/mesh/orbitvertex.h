
const BYTE x_iAllFaces = 0;
const BYTE x_iClockwise = 1;
const BYTE x_iCounterClockwise = 2;

// -------------------------------------------------------------------------------
//  class    COrbitVertexIter
//
//   devnote    used to simplify orbiting a vertex
//
class COrbitVertexIter
{
public:
    COrbitVertexIter(CONST DWORD *rgdwAdjacency, PVOID rgIndices, UINT cbIndexSize)
        :m_iFace(UNUSED32), 
         m_wPoint(UNUSED32), 
         m_iCurFace(UNUSED32), 
         m_rgdwAdjacency(rgdwAdjacency), 
         m_rgIndices(rgIndices), 
         m_cbIndexSize(cbIndexSize),
         m_bClockwise(FALSE), 
         m_bStopOnBoundary(FALSE)
    {
        // used to allow creation on the stack, and then intialization later
    }

    void Init(DWORD iFace, DWORD wPoint, BYTE iWalkType)
    {
        GXASSERT(iWalkType >= 0 && iWalkType <= 2);

        m_iFace = iFace;
        m_wPoint = wPoint;
        m_iCurFace = iFace;
        m_bClockwise = (iWalkType != x_iCounterClockwise);
        m_bStopOnBoundary = (iWalkType != x_iAllFaces);

        // find the next edge to jump to the next triangle
        m_iNextEdge = FindWedge(m_iCurFace, m_wPoint);
        GXASSERT(m_iNextEdge < 3);

        if (!m_bClockwise)
        {
            m_iNextEdge = (m_iNextEdge + 2) % 3;
        }

        m_iCurEdge = m_iNextEdge;
    }

    DWORD FindWedge(DWORD iFace, DWORD dwPoint)
    {
        DWORD *rgdwIndices;
        WORD *rgwIndices;
        DWORD iEdge;

        if (m_cbIndexSize == 2)
        {
            rgwIndices = (WORD*)m_rgIndices;

            if (rgwIndices[iFace*3] == dwPoint)
            {
                iEdge = 0;
            }
            else if (rgwIndices[iFace*3+1] == dwPoint)
            {
                iEdge = 1;
            }
            else
            {
                GXASSERT(rgwIndices[iFace*3+2] == dwPoint);
                iEdge = 2;
            }
        }
        else
        {
            GXASSERT(m_cbIndexSize == 4);

            rgdwIndices = (DWORD*)m_rgIndices;

            if (rgdwIndices[iFace*3] == dwPoint)
            {
                iEdge = 0;
            }
            else if (rgdwIndices[iFace*3+1] == dwPoint)
            {
                iEdge = 1;
            }
            else
            {
                GXASSERT(rgdwIndices[iFace*3+2] == dwPoint);
                iEdge = 2;
            }
        }

        return iEdge;
    }

    BOOL BEndOfList() { return m_iCurFace == UNUSED32; }

    DWORD GetNextFace();
    //void GetNextEdgeFace(UINT_IDX &iEdge, UINT_IDX &iFace);
    BOOL MoveToCCWFace();

    DWORD IGetPointIndex() const { return m_bClockwise ? m_iCurEdge : (m_iCurEdge + 1) % 3; }

private:
    DWORD m_iFace;
    DWORD m_wPoint;

    DWORD m_iCurFace;
    DWORD m_iCurEdge;
    DWORD m_iNextEdge;

    CONST DWORD *m_rgdwAdjacency;
    PVOID m_rgIndices;
    DWORD m_cbIndexSize;

    BOOL m_bClockwise;
    BOOL m_bStopOnBoundary;
};


// -------------------------------------------------------------------------------
//  function    GetNextFace
//
//   devnote    returns the next face moving in the manner set in init/construction
//                  and as a side effect moves to the face beyond that.
//
//   returns    the index of the next face
//
DWORD COrbitVertexIter::GetNextFace()
{
    DWORD iRet;
    DWORD iPrevFace;

    GXASSERT(m_iCurFace != UNUSED32);
    iRet = m_iCurFace;
    m_iCurEdge = m_iNextEdge;

SwitchedDirectionRetry:

    // get the next face 
    iPrevFace = m_iCurFace;
    m_iCurFace = m_rgdwAdjacency[m_iCurFace * 3 + m_iNextEdge];

    
    // check to make sure that we haven't completely gone around the vertex
    if (m_iCurFace != m_iFace)
    {   
        // make sure that we haven't hit a boundary
        if (m_iCurFace != UNUSED32)
        {
            // find the next edge to jump to the next triangle
            if (m_rgdwAdjacency[m_iCurFace * 3] == iPrevFace)
            {
                m_iNextEdge = 0;
            }
            else if (m_rgdwAdjacency[m_iCurFace * 3 + 1] == iPrevFace)
            {
                m_iNextEdge = 1;
            }
            else
            {
                GXASSERT(m_rgdwAdjacency[m_iCurFace * 3 + 2] == iPrevFace);
                m_iNextEdge = 2;
            }
            GXASSERT(m_iNextEdge < 3);

            if (m_bClockwise)
            {
                m_iNextEdge = (m_iNextEdge + 1) % 3;
            }
            // if going counter clockwise, need to go to the other edge containing m_wPointRep;
            else  //if (!m_bClockwise)
            {
                m_iNextEdge = (m_iNextEdge + 2) % 3;
            }
        }
        else  // hit a boundary, either need to restart in the opposite direction, or we hit the end
        {
            // if going clockwise, need to try counter clockwise next
            if (m_bClockwise && !m_bStopOnBoundary)
            {
                m_bClockwise = false;

                m_iCurFace = m_iFace;

                // go back to the original face, and go counter clockwise
                m_iNextEdge = FindWedge(m_iCurFace, m_wPoint);
                GXASSERT(m_iNextEdge < 3);

                m_iNextEdge = (m_iNextEdge + 2) % 3;

				// modify iCurEdge to account for the fact that the direction has changed
				m_iCurEdge = (m_iCurEdge + 2) % 3;

                // now that it is setup to walk counter clockwise from the initial face, retry
                goto SwitchedDirectionRetry;
            }
            // else if counter clockwise, leave m_iCurFace as UNUSED to signify the end
        }
    }
    else  // wrapped around, need to set m_iCurFace to UNUSED to signify EOL
    {
        m_iCurFace = UNUSED32;
    }

    return iRet;
}

// Moves the starting position to be either at iFace if no boundaries, or the farthest
//   counter clockwise possible, so that one walk in the clockwise direction will hit
//   all the triangles in order
BOOL COrbitVertexIter::MoveToCCWFace()
{
    DWORD iPrevFace;
    DWORD iInitialNextEdge;
	BOOL bRet = FALSE;

    // start out at iFace and look for a boundary in the counter clockwise position
    m_iCurFace = m_iFace;

    // find the next edge to jump to the next triangle
    m_iNextEdge = FindWedge(m_iCurFace, m_wPoint);
    iInitialNextEdge = m_iNextEdge;
    GXASSERT(m_iNextEdge < 3);
    m_iNextEdge = (m_iNextEdge + 2) % 3;

    iPrevFace = m_iCurFace;
    do
    {
        // get the next face
        iPrevFace = m_iCurFace;
        m_iCurFace = m_rgdwAdjacency[m_iCurFace * 3 + m_iNextEdge];

        if (m_iCurFace != UNUSED32)
        {
            // find the next edge to jump to the next triangle
            if (m_rgdwAdjacency[m_iCurFace * 3] == iPrevFace)
            {
                m_iNextEdge = 0;
            }
            else if (m_rgdwAdjacency[m_iCurFace * 3 + 1] == iPrevFace)
            {
                m_iNextEdge = 1;
            }
            else
            {
                GXASSERT(m_rgdwAdjacency[m_iCurFace * 3 + 2] == iPrevFace);
                m_iNextEdge = 2;
            }
            GXASSERT(m_iNextEdge < 3);

            m_iNextEdge = (m_iNextEdge + 2) % 3;
        }
    }
    while ((m_iCurFace != m_iFace) && (m_iCurFace != UNUSED32));

    // if m_iCurFace is unused, hit counter clockwise most boundary, so start at the last face before boundary
    if (m_iCurFace == UNUSED32)
    {
        m_iCurFace = iPrevFace;

        m_iNextEdge = (m_iNextEdge + 1) % 3;

        // note: if choosing a new starting face, find the new wedge
        if (m_cbIndexSize == 2)
        {
            m_wPoint = ((WORD*)m_rgIndices)[m_iCurFace*3 + m_iNextEdge];
        }
        else
        {
            GXASSERT(m_cbIndexSize == 4);
            m_wPoint = ((DWORD*)m_rgIndices)[m_iCurFace*3 + m_iNextEdge];
        }

		// a CCW face was found, so return true
		bRet = TRUE;
    }
    else // m_iCurFace == iFace, which is as good a place to start as any
    {
        // Use the edge found before trying to go CCW
        m_iNextEdge = iInitialNextEdge;
    }

    m_bClockwise = TRUE;

    m_iCurEdge = m_iNextEdge;

	m_iFace = m_iCurFace;

	return bRet;
}

