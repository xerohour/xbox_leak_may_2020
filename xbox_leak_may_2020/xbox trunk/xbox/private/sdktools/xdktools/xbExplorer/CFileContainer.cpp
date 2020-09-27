// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     cfilecontainer.cpp
// Contents: UNDONE-WARN: Add file description
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"
#include "CFileContainerFormats.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CFileContainer::CFileContainer() : m_cRef(1)
{
    m_cFiles = 0;
    m_fCut = false;
    m_fPaste = false;
    m_fPasteSucceeded = false;
}

CFileContainer::~CFileContainer()
{
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFileContainer::QueryInterface
// Purpose:   Obtain a particular interface to this object.
// Arguments: riid          -- The interface to this object being queried for.
//            ppvObject     -- Buffer to fill with obtained interface.
// Return:    S_OK if interface obtained; E_NOINTERFACE otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP CFileContainer::QueryInterface(REFIID riid, void **ppvObject)
{ 
    *ppvObject = NULL;

    if (riid == IID_IUnknown) 
        *ppvObject = this;
    else if (riid == IID_IDataObject) 
        *ppvObject = static_cast<IDataObject*>(this);
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
// Function:  CFileContainer::AddRef
// Purpose:   Add a reference to this object
// Arguments: None
// Return:    New reference count
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP_(ULONG) CFileContainer::AddRef()
{
    return (ULONG)InterlockedIncrement(&m_cRef);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFileContainer::Release
// Purpose:   Subtract a reference to this object.  If the new number of references is zero, then
//            delete the object.
// Arguments: None
// Return:    New reference count.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STDMETHODIMP_(ULONG) CFileContainer::Release()
{
    if (0 == InterlockedDecrement(&m_cRef))
    {
        bool fQuery = false;
        DM_FILE_ATTRIBUTES dmfa;

        // If a cut originated from us, and a paste finished us, then remove the original files!
        if (m_fCut && m_fPasteSucceeded)
        {
            for (int i = 0; i < m_cFiles; i++)
            {
                xbfu.GetFileAttributes(m_listFiles[i], &dmfa);
                xbfu.Del(m_listFiles[i], &fQuery, NULL);
            }
            listview.SortListView(-1, NULL);
        }
        g_progress.EndThread();
    
        delete this;
        return 0;
    }

    return (ULONG)m_cRef;
}

// ==== IDataObject functions ====
STDMETHODIMP CFileContainer::GetData(FORMATETC *pformatetcIn, STGMEDIUM *pstgm)
{
    int nSize;
    bool bHere = false;
    HGLOBAL hGlobal;
    LPSTR pData = NULL;
    FILEGROUPDESCRIPTOR *pfgd;

    if (!bHere) {
        pstgm->tymed = NULL;
        pstgm->hGlobal = NULL;
        pstgm->pstm = NULL;
        pstgm->pUnkForRelease = NULL;
    }

    if (pformatetcIn->cfFormat == g_suXboxObject)
    {
        // Asking for Xbox-specific information about us.
        if (pformatetcIn->tymed != TYMED_HGLOBAL)
            return DV_E_FORMATETC;
        pstgm->pUnkForRelease = 0;
        pstgm->tymed = TYMED_HGLOBAL ;

        if (!bHere) { 
            // Allocate space for the file descriptor table and lock it so we can write to it
            hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, sizeof(sSourceInfo));
            if (!hGlobal)
                return E_FAIL;
        }
        else
            hGlobal = pstgm->hGlobal;

        sSourceInfo *si = (sSourceInfo*)GlobalLock(hGlobal);

        // Copy information about 'us' to the sSourceInfo block.
        
        // Mark that we're an Xbox (NOTE: It's possible to construe the simple acknowledgement of
        // the g_suXboxObject request as us being an Xbox).  Left as-is to ease sSourceInfo
        // construction on the 'droptarget' side.
        si->fXbox = true;
        
        // Copy information into the sSourceInfo block that uniquely identifies which Xbox we're
        // currently pointing at.
        strcpy(si->szName, g_TargetXbox);

        // Track which window this objeect originated from.  This is so that we can track xbox->xbox
        // drag/drop from both same and different windows..
        si->hwnd = g_hwnd;

        // Unlock the memory
        GlobalUnlock(hGlobal);

        if (!bHere)
        {
            pstgm->tymed   = TYMED_HGLOBAL;
            pstgm->hGlobal = hGlobal;
        }

        return S_OK;
    }
    else if (pformatetcIn->cfFormat == g_suFileGroupDesc)
    {
        // Asking for the file group descriptor.  Note that the FGD request
        // will only come in HGLOBAL format...
        if (pformatetcIn->tymed != TYMED_HGLOBAL)
            return DV_E_FORMATETC;

		// Expand the list of files to include subfiles (if not already done)
		m_listFiles.IncludeSubDirs();
		m_cFiles = m_listFiles.GetNumFiles();

        nSize = sizeof(FILEGROUPDESCRIPTOR) + sizeof(FILEDESCRIPTOR) * m_cFiles;

        if (!bHere) { 
            // Allocate space for the file descriptor table and lock it so we can write to it
            pstgm->hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, nSize);
            if (!pstgm->hGlobal)
                return E_FAIL;
            pstgm->tymed = TYMED_HGLOBAL;
        }
        else
            hGlobal = pstgm->hGlobal;

		// UNDONE-FUTURE: This is where the 'calculating time to copy' dialog would pop up if we supported them.

		// Add all of the files to the groupdesc
        pfgd = (FILEGROUPDESCRIPTOR*)GlobalLock(pstgm->hGlobal);
        for (int i = 0; i < m_cFiles; i++)
		{
			DM_FILE_ATTRIBUTES dmfa;

			FILEDESCRIPTOR *pfd = &pfgd->fgd[i];
			pfd->dwFlags = FD_ATTRIBUTES | FD_FILESIZE;
			strcpy(pfd->cFileName, m_listFiles[i] + MyStrlen(g_szCopyDir));

			xbfu.GetFileAttributes(m_listFiles[i], &dmfa);
			pfd->nFileSizeHigh = dmfa.SizeHigh;
			pfd->nFileSizeLow = dmfa.SizeLow;
			pfd->dwFileAttributes = dmfa.Attributes;
		}

        pfgd->cItems = m_cFiles;

        // Start the thread that handles the progress dialog.  It won't actually display anything for
        // a couple of seconds to avoid displaying it for small copies...
        g_progress.StartThread();
        g_progress.SetError();
        return S_OK;
    }
    else if (pformatetcIn->cfFormat == g_suFileContents)
    {
        g_progress.ClearError();
        CXboxStream *pxboxstream = new CXboxStream(m_listFiles[pformatetcIn->lindex], m_fCut);
        // undone-err: check if failed to open file
        pstgm->tymed = TYMED_ISTREAM;
        pstgm->pstm = pxboxstream;
        m_fPaste = true;
        ULARGE_INTEGER ul;
        ul.QuadPart = 0;
        g_progress.SetFileInfo(strrchr(m_listFiles[pformatetcIn->lindex], '\\') + 1, ul);
        return S_OK;
    }
    else if (pformatetcIn->cfFormat == g_suXboxFiles)
    {
        // Simple HDROP.  We do this through our own format rather than CF_HDROP because
        // we must be sure reply ONLY if we're the destination.  We have to force the shell
        // through the filecontents approach above since we need to stream the data over.
        if (!bHere) { 
            int nMemSize = (MAX_PATH + 1) * m_cFiles + 1;
            hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, nMemSize);
            if (!hGlobal) return FALSE;
        } else {
            hGlobal = pstgm->hGlobal;
        }

        DROPFILES *pdf = (DROPFILES*)GlobalLock(hGlobal);
        if (pdf) {
            pdf->pFiles = sizeof(DROPFILES);
            pdf->fWide = false;

            // Add all of the files to the list
            BYTE *pby = (BYTE*)pdf+pdf->pFiles;
            for (int i = 0; i < m_cFiles; i++)
            {
                memcpy(pby, m_listFiles[i], MyStrlen(m_listFiles[i]));
                pby += MyStrlen(m_listFiles[i]);
                *pby++ = '\0';
            }

            *pby++ = '\0';

            GlobalUnlock(hGlobal);
        }
        if (!bHere)
        {
            pstgm->tymed   = TYMED_HGLOBAL;
            pstgm->hGlobal = hGlobal;
        }
        m_fPaste = true;
        return S_OK;
    }
    else if (pformatetcIn->cfFormat == g_suPrefDropEffect && m_fCut)
    {
        if (!bHere) { 
            int nMemSize = 4;
            hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, nMemSize);
            if (!hGlobal) return FALSE;
        } else {
            hGlobal = pstgm->hGlobal;
        }
        DWORD *pdw = (DWORD*)GlobalLock(hGlobal);
        if (m_fCut)
            *pdw = DROPEFFECT_MOVE;
        GlobalUnlock(hGlobal);

        if (!bHere)
        {
            pstgm->tymed   = TYMED_HGLOBAL;
            pstgm->hGlobal = hGlobal;
        }
        return S_OK;
    }
    else if (pformatetcIn->cfFormat == g_suPerfDropEffect && m_fCut)
    {
        if (!bHere) { 
            int nMemSize = 4;
            hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, nMemSize);
            if (!hGlobal) return FALSE;
        } else {
            hGlobal = pstgm->hGlobal;
        }
        DWORD *pdw = (DWORD*)GlobalLock(hGlobal);
        if (m_fCut)
            *pdw = DROPEFFECT_MOVE;
        else
            *pdw = DROPEFFECT_COPY;
        GlobalUnlock(hGlobal);


        DM_FILE_ATTRIBUTES dmfa;
        bool fQuery = false;
        for (int i = 0; i < m_cFiles; i++)
        {
            xbfu.GetFileAttributes(m_listFiles[i], &dmfa);
            xbfu.Del(m_listFiles[i], &fQuery, NULL);
        }
        m_cFiles = 0;
        listview.SortListView(-1, NULL);

        if (!bHere)
        {
            pstgm->tymed   = TYMED_HGLOBAL;
            pstgm->hGlobal = hGlobal;
        }
        return S_OK;
    }
    else if (pformatetcIn->cfFormat == g_suPasteSucceeded)
    {
        m_fPasteSucceeded = true;
        return S_OK;
    }

    return DV_E_FORMATETC;
}

STDMETHODIMP CFileContainer::GetDataHere(FORMATETC *pformatetc, STGMEDIUM *pmedium)
{
    return E_NOTIMPL;
}

STDMETHODIMP CFileContainer::QueryGetData(FORMATETC *pformatetc) 
{
    if (pformatetc->cfFormat == g_suFileGroupDesc)
    {
        if (pformatetc->ptd == NULL  && (pformatetc->dwAspect & DVASPECT_CONTENT) &&
            pformatetc->lindex == -1 && (pformatetc->tymed    & TYMED_HGLOBAL))
            return S_OK;

        return E_INVALIDARG;
    }

    if (pformatetc->cfFormat == g_suFileContents)
    {
        if (pformatetc->ptd == NULL && (pformatetc->dwAspect & DVASPECT_CONTENT) &&
            (pformatetc->tymed & TYMED_ISTREAM))
            return S_OK;

        return E_INVALIDARG;
    }

    if (pformatetc->cfFormat == g_suXboxFiles)
    {
        if (pformatetc->ptd == NULL && (pformatetc->dwAspect & DVASPECT_CONTENT) &&
            (pformatetc->tymed & TYMED_HGLOBAL))
            return S_OK;

        return E_INVALIDARG;
    }
#if 0
    if (pformatetc->cfFormat == g_suPrefDropEffect)
    {
        if (pformatetc->ptd == NULL && (pformatetc->dwAspect & DVASPECT_CONTENT) &&
            (pformatetc->tymed & TYMED_HGLOBAL))
            return S_OK;

        return E_INVALIDARG;
    }
#endif
    // UNDONE- this should not be allowed!  Removing this may make shell send us proper filecontent requests
    if (pformatetc->cfFormat == CF_HDROP)
        return S_OK;

    // We don't handle the specified format
    return E_NOTIMPL;
}

STDMETHODIMP CFileContainer::GetCanonicalFormatEtc(FORMATETC *pformatectIn, FORMATETC *pformatetcOut)
{
    return E_NOTIMPL;
}

STDMETHODIMP CFileContainer::SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease)
{
    HRESULT hr = S_OK;
    IDataObject *pidataobj;
    if (pformatetc->cfFormat == g_suPerfDropEffect)
    {
        DWORD *pdw = (DWORD*)GlobalLock(pmedium->hGlobal);
        bool fCut = (*pdw == DROPEFFECT_MOVE);        
        GlobalUnlock(pmedium->hGlobal);
        
        // If this was from a cut, then we need to clear out the clipboard so that it will release
        // the final reference TO US.  Be sure to just return afterwards
        OleGetClipboard(&pidataobj);
        
        // check if it was a cut from us; if so, flush the clipboard 
        sSourceInfo sourceinfo;
        GetDropSourceInfo(pidataobj, &sourceinfo);
        if (sourceinfo.fXbox && fCut)
            OleSetClipboard(NULL);

        pidataobj->Release();
        g_progress.EndThread();
        m_fPasteSucceeded = true;
        goto done;
    }
    else if (pformatetc->cfFormat == g_suPasteSucceeded)
    {
        m_fPasteSucceeded = true;
        goto done;
    }
    hr = DV_E_FORMATETC;

done:
    if (fRelease)
        ReleaseStgMedium(pmedium);

    return hr;
}


STDMETHODIMP CFileContainer::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppienumformatetc)
{
    FORMATETC fmte[] = {
        {(USHORT)g_suFileContents,  NULL, DVASPECT_CONTENT, -1, TYMED_ISTREAM},
        {(USHORT)g_suFileGroupDesc, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL},
        {(USHORT)g_suXboxObject,    NULL, DVASPECT_CONTENT, -1, TYMED_NULL},
        {(USHORT)g_suXboxFiles,     NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL},        
        {(USHORT)g_suPrefDropEffect,NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL},        
        {(USHORT)g_suPerfDropEffect,NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL},        
        {(USHORT)g_suPasteSucceeded,NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL},        
    };

    // Create a new file container format object.  This derives from IEnumFORMATETC and defines
    // all of the necessary behaviours.
    CFileContainerFormats *pfcf = new CFileContainerFormats(7, fmte);
    if (!pfcf)
        return E_OUTOFMEMORY;

    // Obtain a IEnumFORMATETC interface to our file container formats object.
    HRESULT hRes = pfcf->QueryInterface(IID_IEnumFORMATETC, (void **)ppienumformatetc);
    return hRes;
}

STDMETHODIMP CFileContainer::DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
    return E_NOTIMPL;
}

STDMETHODIMP CFileContainer::DUnadvise(DWORD dwConnection) 
{
    return E_NOTIMPL;
}

STDMETHODIMP CFileContainer::EnumDAdvise(IEnumSTATDATA **ppenumAdvise)  
{
    return E_NOTIMPL;
}


bool CFileContainer::AddDir(char *szDirName)
{
    return AddFile(szDirName);
}

bool CFileContainer::AddFile(char *szFileName)
{
    //Add the file to the list of files to drag
    m_listFiles.Add(szFileName);
    m_cFiles++;
    return true;
}

#include <assert.h>
CList::CList()
{
    m_pnodeHead = NULL;
    m_pnodeTail = NULL;
	m_fExpanded = false;
	m_cFiles = 0;
}

CList::~CList()
{
    // Delete all the entries
    while (m_pnodeHead)
    {
        CNode *pnodeNext = m_pnodeHead->m_pnodeNext;
        free(m_pnodeHead->m_sz);
        delete m_pnodeHead;
        m_pnodeHead = pnodeNext;
    }
    m_pnodeTail = NULL;
}


void CList::Add(char *sz)
{
    CNode *pnodeNew = new CNode(MyStrdup(sz));
    if (m_pnodeTail)
        m_pnodeTail->m_pnodeNext = pnodeNew;
    else
        m_pnodeHead = pnodeNew;

    m_pnodeTail = pnodeNew;
	m_cFiles++;
}

// UNDONE-OPT: Using this approach since the code above uses it.  Ideally I would replace all
// enumerations of the list with normal list handling, but that change, while simple, would touch
// enough code to potentially introduce bugs.  At this stage of the release, we'll take the minimal
// speed hit.
char *CList::operator[] (int iIndex) const
{
    CNode *pnode = m_pnodeHead;
    for (int i = 0; i < iIndex; i++)
        pnode = pnode->m_pnodeNext;

    return pnode->m_sz;
}

void CList::IncludeSingle(char *szFile)
{
	DM_FILE_ATTRIBUTES dmfa;
	xbfu.GetFileAttributes(szFile, &dmfa);

	// If szFile is dir,then recurse into it.
	if (dmfa.Attributes & FILE_ATTRIBUTE_DIRECTORY)
	{
 		HANDLE hfile = xbfu.FindFirstFile(szFile, &dmfa, true);
		if (hfile != INVALID_HANDLE_VALUE)
		{
			do
			{
				char szSourceSubDir[MAX_PATH];
				wsprintf(szSourceSubDir, "%s\\%s", szFile, dmfa.Name);

				if (dmfa.Attributes & FILE_ATTRIBUTE_DIRECTORY)
					IncludeSingle(szSourceSubDir);
				else
					Add(szSourceSubDir);
			}
			while (xbfu.FindNextFile(hfile, &dmfa, true));
		}
		xbfu.FindClose(hfile, true);
	}
}

void CList::IncludeSubDirs()
{
	// Only do this if we haven't already expanded
	if (m_fExpanded)
		return;

	CNode *pnodeEnd = m_pnodeTail;
	CNode *pnodeCur = m_pnodeHead;

	// note that end doesn't extend.
	while (pnodeCur != pnodeEnd)
	{
		IncludeSingle(pnodeCur->m_sz);
		pnodeCur = pnodeCur->m_pnodeNext;
	}
	IncludeSingle(pnodeCur->m_sz);	// include end node

	m_fExpanded = true;
}
