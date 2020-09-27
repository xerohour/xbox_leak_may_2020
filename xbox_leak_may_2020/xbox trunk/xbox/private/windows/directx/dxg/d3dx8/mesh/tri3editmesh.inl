

template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
// mark the face as unused
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::MarkAsUnused(UINT_IDX iFace)
{
    UINT iEdge;

    // for each edge, mark the points and neighbors as UNUSED
    for (iEdge = 0; iEdge < 3; iEdge++)
    {
        m_pFaces[iFace].m_wIndices[iEdge] = UNUSED;
        m_rgpniNeighbors[iFace].m_iNeighbors[iEdge] = UNUSED;
    }
}


// -------------------------------------------------------------------------------
//  Implementation of CFaceListIter
// -------------------------------------------------------------------------------

// -------------------------------------------------------------------------------
//  function    GetNextFace
//
//   devnote    returns the next face moving in the manner set in init/construction
//                  and as a side effect moves to the face beyond that.
//
//   returns    the index of the next face
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
UINT_IDX 
CFaceListIter<UINT_IDX,b16BitIndex,UNUSED>::GetNextFace()
{
    UINT_IDX iRet;
    UINT_IDX iPrevFace;

    GXASSERT(m_iCurFace != UNUSED);
    iRet = m_iCurFace;
    m_iCurEdge = m_iNextEdge;

SwitchedDirectionRetry:

    // get the next face 
    iPrevFace = m_iCurFace;
    m_iCurFace = m_ptmTriMesh->m_rgpniNeighbors[m_iCurFace].m_iNeighbors[m_iNextEdge];

    
    // check to make sure that we haven't completely gone around the vertex
    if (m_iCurFace != m_iFace)
    {   
        // make sure that we haven't hit a boundary
        if (m_iCurFace != UNUSED)
        {
            // find the next edge to jump to the next triangle
            m_iNextEdge = FindEdge(m_ptmTriMesh->m_rgpniNeighbors[m_iCurFace].m_iNeighbors, iPrevFace);
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
                m_iNextEdge = m_ptmTriMesh->FindWedge(m_ptmTriMesh->m_pFaces[m_iCurFace].m_wIndices, m_wPoint);
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
        m_iCurFace = UNUSED;
    }

    return iRet;
}

// -------------------------------------------------------------------------------
//  function    GetNextEdgeFace
//
//   devnote    returns the next edge moving in the manner set in init/construction
//                  and as a side effect moves to the face beyond that.
//
//              NOTE: the difference between this and GetNextFace is that a face
//                  may be returned more than once in order to visit all the edges
//
//   returns    the index of the next face
//
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
void
CFaceListIter<UINT_IDX,b16BitIndex,UNUSED>::GetNextEdgeFace(UINT_IDX &iEdge, UINT_IDX &iFace)
{
    UINT_IDX iPrevFace;
    GXASSERT(m_iCurFace != UNUSED);

    iFace = m_iCurFace;
    iEdge = m_iNextEdge;
    m_iCurEdge = iEdge;

    // get the next face 
    iPrevFace = m_iCurFace;
    m_iCurFace = m_ptmTriMesh->m_rgpniNeighbors[m_iCurFace].m_iNeighbors[m_iNextEdge];

    
    // check to make sure that we haven't completely gone around the vertex
    if (m_iCurFace != m_iFace)
    {   
        // make sure that we haven't hit a boundary
        if (m_iCurFace != UNUSED)
        {
            // find the next edge to jump to the next triangle
            m_iNextEdge = FindEdge(m_ptmTriMesh->m_rgpniNeighbors[m_iCurFace].m_iNeighbors, iPrevFace);
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
                m_iNextEdge = m_ptmTriMesh->FindWedge(m_ptmTriMesh->m_pFaces[m_iCurFace].m_wIndices, m_wPoint);
                GXASSERT(m_iNextEdge < 3);

                m_iNextEdge = (m_iNextEdge + 2) % 3;

				// modify iCurEdge to account for the fact that the direction has changed
				m_iCurEdge = (m_iCurEdge + 2) % 3;
            }
            // else if counter clockwise, leave m_iCurFace as UNUSED to signify the end
        }
    }
    else  // wrapped around, need to set m_iCurFace to UNUSED to signify EOL
    {
        m_iCurFace = UNUSED;
    }
}


// Moves the starting position to be either at iFace if no boundaries, or the farthest
//   counter clockwise possible, so that one walk in the clockwise direction will hit
//   all the triangles in order
template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
bool 
CFaceListIter<UINT_IDX,b16BitIndex,UNUSED>::MoveToCCWFace()
{
    UINT_IDX iPrevFace;
    UINT_IDX iInitialNextEdge;
	bool bRet = false;

    // start out at iFace and look for a boundary in the counter clockwise position
    m_iCurFace = m_iFace;

    // find the next edge to jump to the next triangle
    m_iNextEdge = m_ptmTriMesh->FindWedge(m_ptmTriMesh->m_pFaces[m_iCurFace].m_wIndices, m_wPoint);
    iInitialNextEdge = m_iNextEdge;
    GXASSERT(m_iNextEdge < 3);
    m_iNextEdge = (m_iNextEdge + 2) % 3;

    iPrevFace = m_iCurFace;
    do
    {
        // get the next face
        iPrevFace = m_iCurFace;
        m_iCurFace = m_ptmTriMesh->m_rgpniNeighbors[m_iCurFace].m_iNeighbors[m_iNextEdge];

        if (m_iCurFace != UNUSED)
        {
            // find the next edge to jump to the next triangle
            m_iNextEdge = FindEdge(m_ptmTriMesh->m_rgpniNeighbors[m_iCurFace].m_iNeighbors, iPrevFace);
            GXASSERT(m_iNextEdge < 3);

            m_iNextEdge = (m_iNextEdge + 2) % 3;
        }
    }
    while ((m_iCurFace != m_iFace) && (m_iCurFace != UNUSED));

    // if m_iCurFace is unused, hit counter clockwise most boundary, so start at the last face before boundary
    if (m_iCurFace == UNUSED)
    {
        m_iCurFace = iPrevFace;

        m_iNextEdge = (m_iNextEdge + 1) % 3;

        // note: if choosing a new starting face, find the new wedge
        m_wPoint = m_ptmTriMesh->m_pFaces[m_iCurFace].m_wIndices[m_iNextEdge];

		// a CCW face was found, so return true
		bRet = true;
    }
    else // m_iCurFace == iFace, which is as good a place to start as any
    {
        // Use the edge found before trying to go CCW
        m_iNextEdge = iInitialNextEdge;
    }

    m_bClockwise = true;

    m_iCurEdge = m_iNextEdge;

	m_iFace = m_iCurFace;

	return bRet;
}




// IUnknown method implementations


template <class UINT_IDX, int b16BitIndex, unsigned int UNUSED>
STDMETHODIMP 
GXTri3Mesh<UINT_IDX,b16BitIndex,UNUSED>::QueryInterface(REFIID riid, LPVOID *ppv)
{
    // if this is contained in another object defer the call to that object
    if (m_punkOuter != NULL)
        return m_punkOuter->QueryInterface(riid, ppv);

    *ppv=NULL;
    if (riid == IID_IUnknown)
        *ppv=(IUnknown*)(ID3DXBaseMesh*)this;
    else if (riid == IID_ID3DXBaseMesh)
        *ppv=(ID3DXBaseMesh*)this;
    else if (riid == IID_ID3DXMesh)
        *ppv=(ID3DXMesh*)this;
    else
        return E_NOINTERFACE;

    ((LPUNKNOWN)*ppv)->AddRef();

    return S_OK;
}
