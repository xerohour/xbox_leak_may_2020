//
// Implementation for CCursorManager class
//
// History:
// Date				Who			What
// 01/26/94			colint			created
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop
#include "cursor.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//  CCursorManager
//      Deals with the drag-drop cursors.

CCursorManager g_Cursors;

static UINT BASED_CODE aCursorIDs[] =
{
    IDC_DDFILE, IDC_DDCOPYFILE, IDC_DDMULTIFILE, IDC_DDCOPYMULTIFILE,
};

CCursorManager::CCursorManager()
{
    m_phCurs = (HCURSOR*) new char [sizeof(HCURSOR)*sizeof(aCursorIDs)/sizeof(UINT)];
    m_nAllocCurs = sizeof(aCursorIDs)/sizeof(UINT);

    for ( int i = 0; i < m_nAllocCurs; i++ )
        m_phCurs[i] = NULL;
}

CCursorManager::~CCursorManager()
{
    Cleanup();
}

HCURSOR CCursorManager::GetCursor( int iCursor )
{
    ASSERT( m_phCurs != NULL );
    ASSERT( iCursor >= 0 && iCursor < m_nAllocCurs );

    if ( m_phCurs[iCursor] == NULL )
    {
        m_phCurs[iCursor] = ::LoadCursor(GetResourceHandle(),
            MAKEINTRESOURCE(aCursorIDs[iCursor]));

        if ( m_phCurs[iCursor] == NULL )
            return ::LoadCursor( NULL, IDC_ARROW );
    }

    return m_phCurs[iCursor];
}

void CCursorManager::Flush()
{
    ASSERT( m_phCurs != NULL );

    for ( int i = 0; i < m_nAllocCurs; i++ )
    {
        if ( m_phCurs[i] != NULL )
            DestroyCursor( m_phCurs[i] );
    }
}

void CCursorManager::Cleanup()
{
    if ( m_phCurs != NULL )
    {
        Flush();
        delete ((void*)m_phCurs);
        m_phCurs = NULL;
    }
}
