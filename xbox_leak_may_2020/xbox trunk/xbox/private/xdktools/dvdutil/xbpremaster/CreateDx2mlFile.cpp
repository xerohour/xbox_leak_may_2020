// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CreateDx2mlFile.cpp
// Contents:  Creates the Dx2ml*.xml files.
// Revisions: 6-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ LOCAL VARIABLES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Start of valid data on layer 0 and 1.  Taken from Appendix B of the DVD-X2 Tape format spec V1.0
DWORD g_rgdwLayerMainBandStart[] = { 0x060600,  // Layer 0
                                     0xDFCC50}; // Layer 1


bool g_fOutOfMemory = false;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::AddMedia
// Purpose:   Adds the 'media' element to an XML tree.
// Arguments: pxnInput          -- The '<Input>' element node to add the media subtree to.
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CStep_Progress::AddMedia(DXML *pdxml)
{
    // We don't need to check for Out of Memory errors since all allocations simply set a global
    // flag and throw an exception that is caught and reported above.  Saves us from a lot of
    // NULL checks...

    // TODO: distinguish tape from disk files

    // Add the XControl.dat file
    pdxml->dxiXControl.szNameOnTape = _strdup("XCONTROL.DAT");
    pdxml->dxiXControl.szRefName = _strdup("XLeadout");
    pdxml->dxiXControl.fUsedLyr0 = TRUE;

    // Add the layer 0 Xbox input stream
    pdxml->rgdxi[2].szNameOnTape = _strdup("XIMAGE0.DAT");
    pdxml->rgdxi[2].szRefName = _strdup("XboxLayer0Data");
    pdxml->rgdxi[2].dwImageSize = NUM_XDATA_SECTORS;
    pdxml->rgdxi[2].fUsedLyr0 = TRUE;

    // Add the layer 1 Xbox input stream
    pdxml->rgdxi[3].szNameOnTape = _strdup("XIMAGE1.DAT");
    pdxml->rgdxi[3].szRefName = _strdup("XboxLayer1Data");
    pdxml->rgdxi[3].dwImageSize = NUM_XDATA_SECTORS;
    pdxml->rgdxi[3].fUsedLyr0 = TRUE;

    return S_OK;
} 

#define OBJ_SEC  0x0001
#define OBJ_FILE 0x0008
#define OBJ_ENDOFLIST 0x0080

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::AddLayer
// Purpose:   Adds a <layer> subtree to the specified XML tree.
// Arguments: 
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CStep_Progress::AddLayer(CFLD *pfld, DXML *pdxml, int nLayer)
{
    DXL *pdxl;
    ULONG lsnCur = 0;

    // We don't need to check for Out of Memory errors since all allocations simply set a global
    // flag and throw an exception that is caught and reported above.  Saves us from a lot of
    // NULL checks...

    pdxl = &pdxml->rgdxl[nLayer];
    pdxl->dxrXData.idxiImage = 2 + nLayer;

    // Define where the main band starts on the disc (PSN).  This number is constant across all DVDs
    pdxl->psnStart = pdxl->dxrXData.psnStart = g_rgdwLayerMainBandStart[nLayer];
    pdxl->dxrXData.cpsnSize = NUM_XDATA_SECTORS;

#if 1 // new xbLayout FLD file format support
	DWORD dwType, dwLSN, dwBlockSize;
	char  szName[MAX_PATH];
    int   cxbi;
    XBI   *pxbiBase;

	while (1) {
        // Read information about the next object in the fld file
        pfld->ReadNextObjectInfo(&dwType, &dwLSN, &dwBlockSize, szName);
        if (dwType == OBJ_ENDOFLIST)
            break;

        // Does the object belong on the current layer we're working on?
        if (nLayer == (int)(dwLSN / 1715632)) {
		    if (dwType == OBJ_SEC) {
                /* If this placeholder starts on an odd block, we need to
                 * advance by one, and if it's not the right size, we're in
                 * big trouble */
                if(dwLSN & 1) {
                    if(dwBlockSize < 4098)
                        return E_FAIL;
                    ++dwLSN;
                }
                cxbi        = pdxl->cxph++;
                pxbiBase    = (XBI *)pdxl->rgxph;
                dwBlockSize = XPH::phNone;
            } else if (dwType == OBJ_FILE) {
                // Is the file an xbe file?  If not, ignore it
                if (!lstrcmpi(szName + strlen(szName) - 4, ".xbe")) {
                    cxbi = pdxl->ixbiMac++;
                    if(cxbi >= pdxl->ixbiMax) {
                        XBI *pxbiPrev = pdxl->pxbi;

                        pdxl->pxbi = new XBI[pdxl->ixbiMax += 32];
                        if(pxbiPrev) {
                            memcpy(pdxl->pxbi, pxbiPrev, cxbi * sizeof(XBI));
                            delete pxbiPrev;
                        }
                    }
                    pxbiBase = pdxl->pxbi;
                }
                else
                    continue;
            }
            else
                continue;

            // The stored LSN needs to be relative to the start of the layer
            if (nLayer == 1)
                dwLSN -= 1715632;

            int ixbi = cxbi;
            if(ixbi > 0 && dwLSN < pxbiBase[ixbi-1].lsnStart) {
                /* Somehow we're seeing these blocks out of order.
                 * We need to insert this one in the right spot */
                for(ixbi = 0; ixbi < cxbi; ++ixbi) {
                    if(dwLSN < pxbiBase[ixbi].lsnStart)
                        break;
                }
                memmove(&pxbiBase[ixbi+1], &pxbiBase[ixbi],
                    (cxbi - ixbi) * sizeof(XBI));
            }

            pxbiBase[ixbi].lsnStart = dwLSN;
            pxbiBase[ixbi].cpsnSize = dwBlockSize;
        }
    }

#else // old AMC-based approach - retained for comparison purposes.
    // Now add the data to the X-Data region.  Each data block in the FLD describes a placeholder,
    // XBE file, or block of data.  Iterate over each of the blocks adding the ones that belong
    // to this layer.
    int nElement = pfld->GetNumDataBlock();

    // Iterate over each data block in the FLD file, adding all of the ones that belong to the
    // current layer.
    for (int i = 0; i < nElement; i++)
    {
        DWORD dwBlocks;
        UINT  nBlockLayer;
        IFld::tNodeIdx nodeId = pfld->GetDataBlockIndex(i);
        ULONG lsn;

        // Determine which layer the current data block belongs on.  If it's not the current layer,
        // then skip it.
        pfld->GetLayer(nodeId, nBlockLayer);
        if (nBlockLayer == nLayer)
        {
            int ixbi;
            int cxbi;
            XBI *pxbiBase;

            pfld->GetStartLsn(nodeId, lsn);
            /* Only certain files contain their actual LSN; we need to guess
             * the rest */
            if(lsn == 0)
                lsn = lsnCur;
            else
                lsnCur = lsn;
            pfld->GetBlockSize(nodeId, dwBlocks);
            lsnCur += dwBlocks;

            /* Keep LSNs relative to this layer's data */
            if(nLayer)
                lsn -= NUM_XDATA_SECTORS;

            /* If this is a placeholder, we add it to the placeholder table.
             * If this is an XBE, we'll add it to the image table */
            if(pfld->IsPlaceHolder(nodeId)) {
                if(pdxl->cxph >= MAX_PLACEHOLDER_PER_LAYER)
                    /* Somehow we have too many placeholders on this layer.
                     * We'll just treat this like normal data */
                    continue;
                cxbi = pdxl->cxph++;
                pxbiBase = (XBI *)pdxl->rgxph;
                dwBlocks = XPH::phNone;
            } else if(pfld->IsXbe(nodeId)) {
                
                cxbi = pdxl->ixbiMac++;
                if(cxbi >= pdxl->ixbiMax) {
                    XBI *pxbiPrev = pdxl->pxbi;

                    pdxl->pxbi = new XBI[pdxl->ixbiMax += 32];
                    if(pxbiPrev) {
                        memcpy(pdxl->pxbi, pxbiPrev, cxbi * sizeof(XBI));
                        delete pxbiPrev;
                    }
                }
                pxbiBase = pdxl->pxbi;
            } else
                /* Data file, no need to track it */
                continue;

            ixbi = cxbi;
            if(ixbi > 0 && lsn < pxbiBase[ixbi-1].lsnStart) {
                /* Somehow we're seeing these blocks out of order.
                 * We need to insert this one in the right spot */
                for(ixbi = 0; ixbi < cxbi; ++ixbi) {
                    if(lsn < pxbiBase[ixbi].lsnStart)
                        break;
                }
                memmove(&pxbiBase[ixbi+1], &pxbiBase[ixbi],
                    (cxbi - ixbi) * sizeof(XBI));
            }

            pxbiBase[ixbi].lsnStart = lsn;
            pxbiBase[ixbi].cpsnSize = dwBlocks;
        }
    }
#endif
    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::CreateDx2mlFile
// Purpose:   Creates either dx2ml0.xml or dx2ml1.xml depending on the specified layer.
// Arguments: 
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CStep_Progress::CreateDx2mlFile(CFLD *pfldfile, CDX2MLFile *pxml)
{
    HRESULT hr;
    CFileStmRand *pstm;

    // We don't need to check for Out of Memory errors since all allocations simply set a global
    // flag and throw an exception that is caught and reported above.  Saves us from a lot of
    // NULL checks...

    // Create the output file.  The path and filename are separated in case the destination
    // is tape media.
    CFile outfile(m_pdlg, this, m_media, m_szDestPath, "DX2ML.XML", false);
    if (!outfile.IsInited())
    {
        // Failed to create the file.  Inform the user
        outfile.InformError(m_pdlg->GetHwnd(), outfile.GetInitCode(), outfile.GetFilename());
        return E_FAIL;
    }

    try
    {
        pxml->PdxmlGet()->fLayoutTape = TRUE;
        // Add the media tag(s) -- this tells the mastering tool where the files (dx2ml.xml, etc) exist
        hr = AddMedia(pxml->PdxmlGet());
        if (hr != S_OK)
            return hr;

        // Add the two layers to the side
        if(FAILED(AddLayer(pfldfile, pxml->PdxmlGet(), 0)) ||
            FAILED(AddLayer(pfldfile, pxml->PdxmlGet(), 1)) ||
            !pxml->FValidatePlaceholderLocations(FALSE, FALSE, FALSE))
        {
            MessageBox(NULL, "Invalid placeholders encountered.  Please reopen your FLD file in "
                             "xbLayout to resolve this issue.",
                             "Invalid Placeholder information",
                             MB_ICONEXCLAMATION | MB_APPLMODAL | MB_OK);
            return E_FAIL;
        }

        // Output the xml tree to the outfile.
        hr = outfile.CreateFileStm(&pstm);
        if(hr == S_OK && !pxml->FWriteFile(pstm))
            hr = GetLastError();
        if (hr != S_OK)
        {
            // Failed to output the XML file.  Inform the user
            outfile.InformError(m_pdlg->GetHwnd(), hr, outfile.GetFilename());
            return E_FAIL;
        }

        // We're done writing the file.
        outfile.Close();
    }
    catch(...)
    {
        if (g_fOutOfMemory)
            ReportError(ERROR_OUTOFMEM);
        else
            ReportError(ERROR_UNEXPECTED);
    }

    // Return that the file was successfully generated
    return S_OK;
}
