// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     CDropSource.cpp
// Contents: UNDONE-WARN: Add file description
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDropSource::CDropSource
// Purpose:   CDropSource constructor. Sole purpose is to preset the ref count to 1.
// Arguments: None.
// Return:    None.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CDropSource::CDropSource() : m_cRef(1)
{
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDropSource::QueryInterface
// Purpose:   Obtain a particular interface to this object.
// Arguments: riid          -- The interface to this object being queried for.
//            ppvObject     -- Buffer to fill with obtained interface.
// Return:    S_OK if interface obtained; E_NOINTERFACE otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CDropSource::QueryInterface(REFIID riid, void **ppvObject)
{ 
    *ppvObject = NULL;

    if (riid == IID_IUnknown) 
        *ppvObject = this;
    else if (riid == IID_IDropSource) 
        *ppvObject = static_cast<IDropSource*>(this);
    else
        return E_NOINTERFACE;

    if (*ppvObject)
    {
        static_cast<LPUNKNOWN>(*ppvObject)->AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}   

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDropSource::AddRef
// Purpose:   Add a reference to this object
// Arguments: None
// Return:    New reference count
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP_(ULONG) CDropSource::AddRef()
{
    return (ULONG)InterlockedIncrement(&m_cRef);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDropSource::Release
// Purpose:   Subtract a reference to this object.  If the new number of references is zero, then
//            delete the object.
// Arguments: None
// Return:    New reference count.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP_(ULONG) CDropSource::Release()
{
    if (0 == InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return (ULONG)m_cRef;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDropSource::QueryDragContinue
// Purpose:   Determines whether to continue a drag operation or cancel it. 
// Arguments: fEsc          -- 'true' if the esc key was pressed.
//            dwKeyState    -- Provides state of the keys and mouse buttons.
// Return:    DROPDROP_S_CANCEL to stop the drag, DRAGDROP_S_DROP to drop the data where it is,
//            or NOERROR to continue
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CDropSource::QueryContinueDrag(BOOL fEsc, DWORD dwKeyState)
{
    if (fEsc)
        return ResultFromScode(DRAGDROP_S_CANCEL);
    if (!(dwKeyState & MK_LBUTTON))
        return ResultFromScode(DRAGDROP_S_DROP);
    return NOERROR;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDropSource::GiveFeedback
// Purpose:   Provides cursor feedback to the user since the source task always has the mouse
//            capture.  We can also provide any other type of feedback above cursors if we so
//            desire. 
// Arguments: dwEffect      -- Effect flags returned from the last target.
// Return:    DRAGDROP_S_USEDEFAULTCURSORS so that OLE does all the work for us.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CDropSource::GiveFeedback(DWORD dwEffect)
{
    return ResultFromScode(DRAGDROP_S_USEDEFAULTCURSORS);
}