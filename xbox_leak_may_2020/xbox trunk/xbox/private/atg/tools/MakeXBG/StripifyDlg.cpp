//-----------------------------------------------------------------------------
// File: StripifyDlg.cpp
//
// Desc: Code to implement a dialog to let the user stripify a mesh.
//
// Hist: 03.01.01 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "MakeXBG.h"
#include "StripifyDlg.h"
#include "fvf.h"
#include "TriStripper.h"




//-----------------------------------------------------------------------------
// Name: XBPerf_CalcCacheHits()
// Desc: Calculate the number of cache hits and degenerate triangles
//-----------------------------------------------------------------------------
HRESULT XBPerf_CalcCacheHits( D3DPRIMITIVETYPE dwPrimType, DWORD dwVertexSize,
                              LPDIRECT3DINDEXBUFFER8 pIB, 
                              DWORD dwFirstIndex, DWORD dwNumIndices,
                              DWORD* pdwNumDegenerateTris,
                              DWORD* pdwNumCacheMisses,
                              DWORD* pdwNumPagesCrossed )
{
    // Check arguments
    if( NULL == pdwNumDegenerateTris || NULL == pdwNumCacheMisses || 
        NULL == pdwNumPagesCrossed )
        return E_INVALIDARG;

    // Initialize results
    (*pdwNumDegenerateTris) = 0;
    (*pdwNumCacheMisses)    = 0;
    (*pdwNumPagesCrossed)   = 1;

    // Gain access to indices
    WORD* pIndices;
    pIB->Lock( 0, 0, (BYTE**)&pIndices, 0 );
        
    // Simulate a vertex cache
    static const int CACHE_SIZE = 18;
    static const int PAGE_SIZE  = 4096;
    DWORD rgdwCache[CACHE_SIZE];
    INT   iCachePtr      = 0;
    BOOL  bIsTriStrip    = (dwPrimType == D3DPT_TRIANGLESTRIP);
    DWORD dwLastPageAddr = 0;
    memset( rgdwCache, 0xff, sizeof(rgdwCache) );

    // Run all vertices through the sumilated vertex cache, tallying cache hits,
    // degenerate triangles, and pages crossed.
    for( DWORD i = dwFirstIndex; i < dwFirstIndex+dwNumIndices; i++ )
    {
        // This makes all kinds of assumptions such as page size is 4k,
        // page across then back is ok, etc etc. Seems to be an ok
        // estimate on data locality though.
        DWORD dwPage = dwVertexSize * pIndices[i] / PAGE_SIZE;

        if( ( dwPage > dwLastPageAddr ) || ( dwPage+1 < dwLastPageAddr ) )
        {
            (*pdwNumPagesCrossed)++;
            dwLastPageAddr = dwPage;
        }

        // Update our count of degenerate tris
        if( bIsTriStrip && (i > 1) )
            if( ( pIndices[i-0] == pIndices[i-1] ) ||
                ( pIndices[i-0] == pIndices[i-2] ) ||
                ( pIndices[i-1] == pIndices[i-2] ) )
                (*pdwNumDegenerateTris)++;

        // Check to see if the vertex would be in the cache
        BOOL bVertexInCache = FALSE;
        for( int cache_index = 0; cache_index < CACHE_SIZE; cache_index++ )
        {
            if( pIndices[i] == rgdwCache[cache_index] )
            {
                bVertexInCache = TRUE;
                break;
            }
        }

        if( bVertexInCache )
        {
            // Do nothing
        }
        else 
        {
            // Keep track of cache misses
            (*pdwNumCacheMisses)++;

            // Add vertex to simulated cache
            rgdwCache[iCachePtr] = pIndices[i];
            iCachePtr = (iCachePtr + 1) % CACHE_SIZE;
        }
    }

    // Done with the index buffer
    pIB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: StripifyDlg()
// Desc: Constructor
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(StripifyDlg, CDialog)

StripifyDlg::StripifyDlg( CWnd* pParent /*=NULL*/ )
          :CDialog(StripifyDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(ViewCacheHitsDlg)
    //}}AFX_DATA_INIT

	m_dwOptimizeFlag = OPTIMIZE_FOR_CACHE;
	m_dwOutputFlag = OUTPUT_TRISTRIP;
}




//-----------------------------------------------------------------------------
// Name: DoDataExchange()
// Desc: 
//-----------------------------------------------------------------------------
void StripifyDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(StripifyDlg)
    //}}AFX_DATA_MAP

    if( pDX->m_bSaveAndValidate )
    {
    }
    else
    {
        // Figure out how many degenerate triangles and cache hits we've got
        DWORD dwNumTotalIndices        = 0;
        DWORD dwNumTotalVertices       = 0;
        DWORD dwNumTotalTriangles      = 0;
        DWORD dwNumTotalDegenerateTris = 0;
        DWORD dwNumTotalCacheMisses    = 0;
        DWORD dwNumTotalPagesCrossed   = 0;

        for( DWORD i=0; i<m_pFrame->m_dwNumMeshSubsets; i++ )
        {
            DWORD dwDegenerateTris;
            DWORD dwCacheMisses;
            DWORD dwPagesCrossed;

            XBPerf_CalcCacheHits( m_pFrame->m_dwMeshPrimType, m_pFrame->m_dwMeshVertexSize,
                                  m_pFrame->m_pMeshIB, 
                                  m_pFrame->m_pMeshSubsets[i].dwIndexStart,
                                  m_pFrame->m_pMeshSubsets[i].dwIndexCount,
                                  &dwDegenerateTris, &dwCacheMisses, &dwPagesCrossed );

            dwNumTotalIndices   += m_pFrame->m_pMeshSubsets[i].dwIndexCount;
            dwNumTotalVertices  += m_pFrame->m_pMeshSubsets[i].dwVertexCount;

			if( D3DPT_TRIANGLESTRIP == m_pFrame->m_dwMeshPrimType )
	            dwNumTotalTriangles += m_pFrame->m_pMeshSubsets[i].dwIndexCount - 2;
			else
				dwNumTotalTriangles += m_pFrame->m_pMeshSubsets[i].dwIndexCount/3;

            dwNumTotalDegenerateTris += dwDegenerateTris;
            dwNumTotalCacheMisses    += dwCacheMisses;
            dwNumTotalPagesCrossed   += dwPagesCrossed;
        }

        // Update the UI
        TCHAR strText[20];
        _stprintf( strText, "%ld", dwNumTotalTriangles );
        GetDlgItem(IDC_ORIGINAL_NUMTRIANGLES)->SetWindowText( strText );
        _stprintf( strText, "%ld", dwNumTotalVertices );
        GetDlgItem(IDC_ORIGINAL_NUMVERTICES)->SetWindowText( strText );
        _stprintf( strText, "%ld", dwNumTotalIndices );
        GetDlgItem(IDC_ORIGINAL_NUMINDICES)->SetWindowText( strText );

        _stprintf( strText, "%6.3f", ((FLOAT)dwNumTotalVertices)/(dwNumTotalTriangles-dwNumTotalDegenerateTris) );
        GetDlgItem(IDC_ORIGINAL_VERTSPERTRI)->SetWindowText( strText );
        _stprintf( strText, "%6.3f", ((FLOAT)dwNumTotalCacheMisses)/(dwNumTotalTriangles-dwNumTotalDegenerateTris) );
        GetDlgItem(IDC_ORIGINAL_MISSESPERTRI)->SetWindowText( strText );

        _stprintf( strText, "%ld", dwNumTotalDegenerateTris );
        GetDlgItem(IDC_ORIGINAL_NUMDEGENERATETRIS)->SetWindowText( strText );
        _stprintf( strText, "%ld", dwNumTotalCacheMisses );
        GetDlgItem(IDC_ORIGINAL_NUMCACHEMISSES)->SetWindowText( strText );
        _stprintf( strText, "%ld", dwNumTotalPagesCrossed );
        GetDlgItem(IDC_ORIGINAL_NUMPAGESCROSSED)->SetWindowText( strText );

        GetDlgItem(IDC_STRIPPED_NUMTRIANGLES)->EnableWindow( FALSE );
        GetDlgItem(IDC_STRIPPED_NUMVERTICES)->EnableWindow( FALSE );
        GetDlgItem(IDC_STRIPPED_NUMINDICES)->EnableWindow( FALSE );
        GetDlgItem(IDC_STRIPPED_NUMDEGENERATETRIS)->EnableWindow( FALSE );
        GetDlgItem(IDC_STRIPPED_NUMCACHEMISSES)->EnableWindow( FALSE );
        GetDlgItem(IDC_STRIPPED_NUMPAGESCROSSED)->EnableWindow( FALSE );

		GetDlgItem(IDC_RADIO2)->SendMessage( BM_SETCHECK, BST_CHECKED, 0 );
		GetDlgItem(IDC_RADIO3)->SendMessage( BM_SETCHECK, BST_CHECKED, 0 );
    }
}




BEGIN_MESSAGE_MAP(StripifyDlg, CDialog)
    //{{AFX_MSG_MAP(StripifyDlg)
    ON_BN_CLICKED(IDC_STRIP, OnStrip)
	ON_BN_CLICKED(IDC_RADIO1, OnOptForIndices)
	ON_BN_CLICKED(IDC_RADIO2, OnOptForCache)
	ON_BN_CLICKED(IDC_RADIO4, OnOutputTriList)
	ON_BN_CLICKED(IDC_RADIO3, OnOutputTriStrip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




//-----------------------------------------------------------------------------
// Name: OnStrip()
// Desc: 
//-----------------------------------------------------------------------------
void StripifyDlg::OnStrip() 
{
	// Turn off the button right away.
    GetDlgItem(IDC_STRIP)->EnableWindow( FALSE );
	HCURSOR oldCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );

    // Run the stripper
    m_pFrame->Stripify( m_dwOptimizeFlag | m_dwOutputFlag );

    // Figure out how many degenerate triangles and cache hits we've got
    DWORD dwNumTotalIndices        = 0;
    DWORD dwNumTotalVertices       = 0;
    DWORD dwNumTotalTriangles      = 0;
    DWORD dwNumTotalDegenerateTris = 0;
    DWORD dwNumTotalCacheMisses    = 0;
    DWORD dwNumTotalPagesCrossed   = 0;

    for( DWORD i=0; i<m_pFrame->m_dwNumMeshSubsets; i++ )
    {
        DWORD dwDegenerateTris;
        DWORD dwCacheMisses;
        DWORD dwPagesCrossed;

        XBPerf_CalcCacheHits( m_pFrame->m_dwMeshPrimType, m_pFrame->m_dwMeshVertexSize,
                              m_pFrame->m_pMeshIB, 
                              m_pFrame->m_pMeshSubsets[i].dwIndexStart,
                              m_pFrame->m_pMeshSubsets[i].dwIndexCount,
                              &dwDegenerateTris, &dwCacheMisses, &dwPagesCrossed );

        dwNumTotalIndices   += m_pFrame->m_pMeshSubsets[i].dwIndexCount;
        dwNumTotalVertices  += m_pFrame->m_pMeshSubsets[i].dwVertexCount;

		if( D3DPT_TRIANGLESTRIP == m_pFrame->m_dwMeshPrimType )
	        dwNumTotalTriangles += m_pFrame->m_pMeshSubsets[i].dwIndexCount - 2;
		else
			dwNumTotalTriangles += m_pFrame->m_pMeshSubsets[i].dwIndexCount/3;

        dwNumTotalDegenerateTris += dwDegenerateTris;
        dwNumTotalCacheMisses    += dwCacheMisses;
        dwNumTotalPagesCrossed   += dwPagesCrossed;
    }

    // Activate controls
    GetDlgItem(IDC_STRIPPED_NUMTRIANGLES)->EnableWindow( TRUE );
    GetDlgItem(IDC_STRIPPED_NUMVERTICES)->EnableWindow( TRUE );
    GetDlgItem(IDC_STRIPPED_NUMINDICES)->EnableWindow( TRUE );
    GetDlgItem(IDC_STRIPPED_NUMDEGENERATETRIS)->EnableWindow( TRUE );
    GetDlgItem(IDC_STRIPPED_NUMCACHEMISSES)->EnableWindow( TRUE );
    GetDlgItem(IDC_STRIPPED_NUMPAGESCROSSED)->EnableWindow( TRUE );

    // Write out results
    TCHAR strText[20];
    _stprintf( strText, "%ld", dwNumTotalTriangles );
    GetDlgItem(IDC_STRIPPED_NUMTRIANGLES)->SetWindowText( strText );
    _stprintf( strText, "%ld", dwNumTotalVertices );
    GetDlgItem(IDC_STRIPPED_NUMVERTICES)->SetWindowText( strText );
    _stprintf( strText, "%ld", dwNumTotalIndices );
    GetDlgItem(IDC_STRIPPED_NUMINDICES)->SetWindowText( strText );

    _stprintf( strText, "%6.3f", ((FLOAT)dwNumTotalVertices)/(dwNumTotalTriangles-dwNumTotalDegenerateTris) );
    GetDlgItem(IDC_STRIPPED_VERTSPERTRI)->SetWindowText( strText );
    _stprintf( strText, "%6.3f", ((FLOAT)dwNumTotalCacheMisses)/(dwNumTotalTriangles-dwNumTotalDegenerateTris) );
    GetDlgItem(IDC_STRIPPED_MISSESPERTRI)->SetWindowText( strText );
    
    _stprintf( strText, "%ld", dwNumTotalDegenerateTris );
    GetDlgItem(IDC_STRIPPED_NUMDEGENERATETRIS)->SetWindowText( strText );
    _stprintf( strText, "%ld", dwNumTotalCacheMisses );
    GetDlgItem(IDC_STRIPPED_NUMCACHEMISSES)->SetWindowText( strText );
    _stprintf( strText, "%ld", dwNumTotalPagesCrossed );
    GetDlgItem(IDC_STRIPPED_NUMPAGESCROSSED)->SetWindowText( strText );

	// Turn button back on when we are done.
    GetDlgItem(IDC_STRIP)->EnableWindow( TRUE );
	SetCursor( oldCursor );
}




//-----------------------------------------------------------------------------
// Name: OnOptForIndices()
// Desc: 
//-----------------------------------------------------------------------------
void StripifyDlg::OnOptForIndices() 
{
	m_dwOptimizeFlag = OPTIMIZE_FOR_INDICES;
}




//-----------------------------------------------------------------------------
// Name: OnOptForCache()
// Desc: 
//-----------------------------------------------------------------------------
void StripifyDlg::OnOptForCache() 
{
	m_dwOptimizeFlag = OPTIMIZE_FOR_CACHE;
}




//-----------------------------------------------------------------------------
// Name: OnOutputTriList()
// Desc: 
//-----------------------------------------------------------------------------
void StripifyDlg::OnOutputTriList() 
{
	m_dwOutputFlag = OUTPUT_TRILIST;
}




//-----------------------------------------------------------------------------
// Name: OnOutputTriStrip()
// Desc: 
//-----------------------------------------------------------------------------
void StripifyDlg::OnOutputTriStrip() 
{
	m_dwOutputFlag = OUTPUT_TRISTRIP;
}
