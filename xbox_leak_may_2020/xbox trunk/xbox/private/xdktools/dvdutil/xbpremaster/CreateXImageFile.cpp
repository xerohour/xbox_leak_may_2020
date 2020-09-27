// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CreateXImageFile.cpp
// Contents:  Contains code to create the Ximage*.dat files.
// Revisions: 6-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"
#include <xboxverp.h>

struct VERSION_INFO 
{
    BYTE rgbySig[32];
    BYTE rgbyLayoutVersion[8];
    BYTE rgbyPremasterVersion[8];
    BYTE rgPadding[2048-32-8-8];    // fill with zeros
};
#define VERSION_SIG "XBOX_DVD_LAYOUT_TOOL_SIG"


CCheckSum g_xsum;
bool g_fInPlaceholder = false, g_fInXbe = false, g_fInMedia = false;
int g_nMinAppLevel = 255;

bool g_fDontWarnInvalidXBE = false;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ DEFINES  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ONE_BLOCK        -- Number of bytes in one block.
#define ONE_BLOCK 2048

// QUEUE            -- Number of blocks to queue up for the write for performance
#define QUEUE 16


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  DebugOutput
// Purpose:   Debugging aid.  Outputs the specified vararg string to the debugger
// Arguments: lpszFormat        -- Format of the string to output
//            ...               -- Optional variables for the format string.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DebugOutput(LPCTSTR lpszFormat, ...)
{
#ifdef _DEBUG
    va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	TCHAR szBuffer[512];

	nBuf = vsprintf(szBuffer, lpszFormat, args);

    OutputDebugString(szBuffer);

	va_end(args);
#endif
}

extern DWORD g_cbyTotalWritten;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::CheckInPlaceholder
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CStep_Progress::CheckInPlaceholder(CFile *pfileXImage)
{
    // Look at how many blocks have been written; are we currently in a placeholder?

    // Convert the location to block number
    DWORD dwCurBlock = g_cbyTotalWritten / 2048;

    // Compare the block number against the next placeholder block to see
    // if the block number is inside (or past) it.
CheckPH:
    if (dwCurBlock >= m_rgdwPH[m_iCurPH])
    {
        if (dwCurBlock >= m_rgdwPH[m_iCurPH] + 4096)
        {
            m_iCurPH++;
            goto CheckPH;
        }
        g_fInPlaceholder = true;
    }
    else
        g_fInPlaceholder = false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::ClearInPlaceholder
// Purpose:   
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CStep_Progress::ClearInPlaceholder()
{
    g_fInPlaceholder = false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::FillRandomBlocks
// Purpose:   Writes out the specified number of blocks to the specified file.  Filled with psuedo-
//            random data.  Each block is 2048 bytes.
// Arguments: pfileOut          -- The file to write the random blocks to.
//            nBlocks           -- The number of blocks to write.
// Return:    S_OK on success, E_FAIL on failure (or cancellation)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CStep_Progress::FillRandomBlocks(CFile *pfileOut, int nBlocks)
{
    HRESULT hr;
    BYTE by[ONE_BLOCK];

    DebugOutput("Filling %d random blocks.  Total written: %d blocks\r\n", nBlocks, m_cBlocksWritten + nBlocks);

    for (int i = 0; i < nBlocks; i++)
    {
        // Fill the buffer with random data.
        g_rand.RandBytes(by, ONE_BLOCK);

        // If the current block is in a placeholder, then we need to be sure that
        // these random bytes are not added to the checksum (calculated in Write)
        CheckInPlaceholder(pfileOut);

        // Write the next block of random data
        hr = pfileOut->Write((BYTE*)by, ONE_BLOCK);
        if (hr != S_OK)
        {
            pfileOut->InformError(m_pdlg->GetHwnd(), hr, pfileOut->GetFilename());
            goto done;
        }

        if (IncProgressBar(1) == true)
        {
            hr = E_FAIL;
            goto done;
        }
    }

    // If here, then we were successful
    hr = S_OK;
    
done:

    ClearInPlaceholder();

    return hr;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::FillRandomBytes
// Purpose:   Writes out the specified number of bytes to the specified file.  Filled with psuedo-
//            random data.
// Arguments: pfileOut          -- The file to write the random bytes to.
//            nBytes            -- The number of bytes to write
// Return:    S_OK on success, error code otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CStep_Progress::FillRandomBytes(CFile *pfileOut, int nBytes)
{
    BYTE by[ONE_BLOCK];
    HRESULT hr;

    // Batch blocks together for performance.
    if (nBytes > ONE_BLOCK)
    {
        hr = FillRandomBlocks(pfileOut, nBytes / ONE_BLOCK);
        if (hr != S_OK)
            goto done;
        
        nBytes %= ONE_BLOCK;
    }

    if (nBytes)
    {
        DebugOutput("Filling %d random BYTES.\r\n", nBytes);

        // Fill the buffer with random data.
        g_rand.RandBytes(by, nBytes);

        // If the current block is in a placeholder, then we need to be sure that
        // these random bytes are not added to the checksum (calculated in Write)
        CheckInPlaceholder(pfileOut);

        hr = pfileOut->Write((BYTE*)by, nBytes);
        if (hr != S_OK)
        {
            pfileOut->InformError(m_pdlg->GetHwnd(), hr, pfileOut->GetFilename());
            goto done;
        }
    }

    // If here, then we were successful
    hr = S_OK;

done:

    ClearInPlaceholder();

    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::IncProgressBar
// Purpose:   Increase the number of blocks written and update the progress bar.  Also checks to
//            see if the user has pressed the cancel button.
// Arguments: nBlocksWritten        -- The number of blocks written since the last call to
//            IncProgressBar.
// Return:    'true' if the user cancelled
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CStep_Progress::IncProgressBar(int nBlocksWritten)
{
    m_cBlocksWritten += nBlocksWritten;

    // Update the progress bar
    m_pdlg->SetProgressPos(IDC_PROGRESS_PROGRESSBAR, m_cBlocksWritten);

//    sprintf(sz, "%d %% (%d blocks written)", m_cBlocksWritten * 100 / NUM_XDATA_SECTORS, m_cBlocksWritten);
  //  m_pdlg->SetControlText(IDC_PROGRESS_TEXT_PROGRESS, sz);

    // Check if we should cancel
    return CheckCancelled();
}

sString *g_rgstrInvalidXbeName;
int g_cInvalidXbes;
int g_cMaxInvalidXbes;
int g_nInvalidXbeLevel;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  InvalidXbeProc
// Purpose:   Message handler for InvalidXBE dialog. This function is automatically called by Windows
//            whenever a message needs to be sent to the dialog box (ie 'paint', 'move', etc).
// Arguments: hDlg          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK InvalidXbeProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    char szBuf[1024];

	switch (message)
	{
	case WM_INITDIALOG:
        if (g_nInvalidXbeLevel == 0)
            sprintf(szBuf, "The XBE file '%s' is linked with unapproved " \
                           "libraries, and will not be accepted for "\
                           "certification", g_rgstrInvalidXbeName[g_cInvalidXbes].sz);
        else
            sprintf(szBuf, "The XBE file '%s' is linked with potentially " \
                           "unapproved libraries, and may not be accepted for",
                           " certification", g_rgstrInvalidXbeName[g_cInvalidXbes].sz);

        SetDlgItemText(hDlg, IDC_TEXT, szBuf);
		return true;

	case WM_COMMAND:
        if (wParam == ID_OK)
        {
            // Did the user check the 'go away' checkbox?
            g_fDontWarnInvalidXBE = IsDlgButtonChecked(hDlg, IDC_DONTTELL) ? true : false;
		    EndDialog(hDlg, 0);
        }
		return false;
	}
    return false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::CreateXImageFile
// Purpose:   Creates the ximage*.dat files.
// Arguments: nLayer                -- Layer (0 or 1) to create
//            pfst                  -- Pointer to the object that contains the list of files to add
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CStep_Progress::CreateXImageFile(int nLayer, CFST *pfst, CDX2MLFile *pxml)
{    
    char szName[MAX_PATH];
    int nWritten;
    HRESULT hr;

    // Clear the "we're currently in a placeholder" flag.  This flag is used to
    // determine whether or not a particular block being written out needs to
    // be added to the submission checksum
    ClearInPlaceholder();
    g_fInXbe = false;
    g_fInMedia = true;
    g_cbyTotalWritten = 0;

    // Walk the specifed xml object to determine where the placeholders are.
    DXML *pdxml = pxml->PdxmlGet();
    for (int i = 0; i < 8; i++)
        m_rgdwPH[i] = pdxml->rgdxl[nLayer].rgxph[i].lsnStart;
    m_rgdwPH[8] = (DWORD)-1;
    m_iCurPH = 0;

    // Create the name of this ximage*.dat file.
    sprintf(szName, "XIMAGE%d.DAT", nLayer);

    // Create the output file.  The path and filename are separated in case the destination is tape
    CFile outfile(m_pdlg, this, m_media, m_szDestPath, szName, false);
    if (!outfile.IsInited())
    {
        // Failed to create the file.  Inform the user
        outfile.InformError(m_pdlg->GetHwnd(), outfile.GetInitCode(), outfile.GetFilename());
        return outfile.GetInitCode();
    }

    // Keep track of how many blocks we've written so far (used for updating the progress bar)
    if (nLayer == 0) m_cBlocksWritten = 0;

    int nPrevLSN = 0;
    int iObject  = 0;

    // Repeat until we've reached the end of the XDATA region
    while (nPrevLSN < NUM_XDATA_SECTORS)
    {
        int nStartLSN, nEndLSN, nSectorsToWrite;
        DWORD dwOffset;
        char szName[MAX_PATH];

        g_fInXbe = false;

        // Get the next object in the FST.
        if (!pfst->GetEntry(nLayer, iObject, szName, &nStartLSN, &nEndLSN,
                   &dwOffset, m_source == SOURCE_PACKAGE, m_szPackageDVDRoot))
        {
            // There are no more objects in the FST.  Fill the rest of the sectors with random.
            DebugOutput("No more objects in FST. Filling remaining %d blocks with random\r\n",
                        NUM_XDATA_SECTORS - nPrevLSN);
            hr = FillRandomBlocks(&outfile, NUM_XDATA_SECTORS - nPrevLSN);
            if (hr != S_OK)
                goto done;

            nEndLSN = NUM_XDATA_SECTORS;
        }
        else
        {
            // Found another object in the FST.
            DebugOutput("File '%s' found in FST at LSN %d.\r\n",szName, nStartLSN);
            if (nStartLSN > nPrevLSN)
            {
                // Fill in the blocks between the current object and the previous object with random data
                DebugOutput("  Filling previous blocks [%d-%d] with random data \r\n", nPrevLSN,
                            nStartLSN - 1);
                hr = FillRandomBlocks(&outfile, nStartLSN - nPrevLSN);
                if (hr != S_OK)
                    goto done;
            }

            // Calculate how many sectors are used up by this FST entry
            nSectorsToWrite = nEndLSN - nStartLSN + 1;

            DebugOutput("   Preparing to fill blocks [%d-%d] with file data \r\n", nStartLSN, nEndLSN);

            // Write out the new object to the outfile

            // Load the FST object (file)
            CFile *pfileNext = new CFile(m_pdlg, this, szName);
            if (!pfileNext->IsInited())
            {
                // Failed to load the specified FST object
                hr = pfileNext->GetInitCode();
                pfileNext->InformError(m_pdlg->GetHwnd(), hr, pfileNext->GetFilename());
                goto done;
            }
            
            // Is the object an xbe file?  If so, we need to update the black
            // xcheck checksum, and check the xbe's library validation state
            if (!lstrcmpi(szName + strlen(szName) - 4, ".xbe"))
            {
               // Create a new CXBEFile to point at the file
                CFileStmRand *pstm;
                pfileNext->CreateFileStm(&pstm);
                CXBEFile xf(pstm);
                xf.FCalcChecksums(&g_xsum, NULL, NULL);
                int nLibApprovalLevel = xf.GetLibApprovalLevel();
                g_nMinAppLevel = min(g_nMinAppLevel, nLibApprovalLevel);

                if (nLibApprovalLevel <= 1)
                {
                    // uh oh, we don't support it.  Warn the user

                    // Need to allocate more space for the list of invalid
                    // xbe names?
                    if (g_cInvalidXbes == g_cMaxInvalidXbes)
                    {
                        g_cMaxInvalidXbes += BATCH;
                        g_rgstrInvalidXbeName = (sString*)realloc(
                                        g_rgstrInvalidXbeName,
                                        g_cMaxInvalidXbes*sizeof(sString));
                    }

                    strcpy(g_rgstrInvalidXbeName[g_cInvalidXbes].sz, szName);

                    // Pop up the modal dialog to warn the user
                    if (!g_fDontWarnInvalidXBE)
                    {
                        g_nInvalidXbeLevel = nLibApprovalLevel;
#if 1
                        // Just add the information to the dialog so that the
                        // user can see that something bad happened
                        char sz[MAX_PATH + 50];
                        sprintf(sz, "Warning: Invalid xbe encountered (%s)",
                                szName);
                        m_pdlg->SetControlText(IDC_PROGRESS_BADXBE, sz);
#else
                        // Do a modal dialog to force user intervention
                        DialogBox(m_pdlg->GetInst(), (LPCTSTR)IDD_INVALIDXBE,
                                  m_pdlg->GetHwnd(), (DLGPROC)InvalidXbeProc);
#endif
                    }
                    g_cInvalidXbes++;
                }

                delete pstm;
                g_fInXbe = true;
            }

            // Seek to the specified offset within the file.
            LARGE_INTEGER liDist;
            liDist.QuadPart = dwOffset;
            DebugOutput("   Seeking to %d\r\n", liDist.LowPart);
            hr = pfileNext->SeekTo(liDist);
            if (hr != S_OK)
            {
                pfileNext->InformError(m_pdlg->GetHwnd(), hr, pfileNext->GetFilename());
                pfileNext->Close();
                goto done;
            }

            DebugOutput("   Copying %d File BYTEs\r\n", nSectorsToWrite * 2048);
            
            // Is the object the volume descriptor?  If so, special case it so
            // that we can inject our version information.  If the volume
            // descriptor is only 1 sector long, then the FLD came from an
            // old version of xbLayout and should be relaid out.
            if (nStartLSN == 32)
            {
                if (nSectorsToWrite == 1)
                {
                    MessageBox(m_pdlg->GetHwnd(), "The specified FLD file was generated in an obsolete version\n"
                               "of xbLayout.  Please reload your FLD file in the latest version of\nxbLayout and "
                               "click 'Premaster' again to generate updated FST and BIN files.",
                               "Obsolete layout encountered", MB_ICONEXCLAMATION | MB_OK);
                    pfileNext->Close();
                    hr = E_FAIL;
                    goto done;
                }
                BYTE rgby[4096];
                int nRead;
                hr = pfileNext->Read(rgby, 4096, &nRead);
                if (!SUCCEEDED(hr) || nRead != 4096)
                {
                    outfile.InformError(m_pdlg->GetHwnd(), hr, pfileNext->GetFilename());
                    pfileNext->Close();
                    goto done;
                }

                if (memcmp(rgby+2048, VERSION_SIG, strlen(VERSION_SIG)))
                {
                    MessageBox(m_pdlg->GetHwnd(), "An invalid version signature was encountered.",
                               "Invalid layout encountered", MB_ICONEXCLAMATION | MB_OK);
                    pfileNext->Close();
                    hr = E_FAIL;
                    goto done;
                }
                
                // Inject our version number
                VERSION_INFO *pverinfo = (VERSION_INFO*)&rgby[2048];
                int nMajor, nMinor, nBuild, nQFE;
                sscanf(VER_PRODUCTVERSION_STR, "%d.%d.%d.%d", &nMajor, &nMinor, &nBuild,
                                                              &nQFE);
                *(WORD*)(&pverinfo->rgbyPremasterVersion[0]) = (WORD)nMajor;
                *(WORD*)(&pverinfo->rgbyPremasterVersion[2]) = (WORD)nMinor;
                *(WORD*)(&pverinfo->rgbyPremasterVersion[4]) = (WORD)nBuild;
                *(WORD*)(&pverinfo->rgbyPremasterVersion[6]) = (WORD)nQFE;

                // Write the bytes to the destination
                hr = outfile.Write(rgby, 4096);
                if (!SUCCEEDED(hr))
                {
                    outfile.InformError(m_pdlg->GetHwnd(), hr, outfile.GetFilename());
                    pfileNext->Close();
                    goto done;
                }
                nWritten = 4096;
            }
            else
            {
                // Write the specified number of bytes ((end - start + 1) * sector size) of the file to
                // the outfile.  The CopyFileBytes will copy from the current location of the source
                // file, and will copy the requested number of bytes;  it will return the number of
                // bytes written.
                if (!outfile.CopyFileBytes(pfileNext, nSectorsToWrite * 2048, &nWritten, &hr))
                {
                    // An error occurred or the user cancelled.  Only report error if not cancelled
                    if (hr != S_OK)
                        outfile.InformError(m_pdlg->GetHwnd(), hr, outfile.GetFilename());
                    pfileNext->Close();
                    goto done;
                }
            }

            // If the file is smaller than the requested number of sectors then we need to (a)
            // calculate how many bytes were not written, and (b) fill them with random data.

            // Calculate the number of bytes remaining (if any)
            int nRemaining = (nSectorsToWrite * 2048 - nWritten);
            if (nRemaining)
            {
                // The file size wasn't exactly divisible by the sector size.  Fill the remaining
                // bytes with random data.
                DebugOutput("   Filling %d remaining random BYTEs\r\n", nRemaining);
                hr = FillRandomBytes(&outfile, nRemaining);
                if (hr != S_OK)
                {
                    pfileNext->Close();
                    goto done;
                }
            }

            // Increment the progress bar and check if the user clicked 'cancel'
            if (IncProgressBar(nSectorsToWrite) == true)
            {
                // User cancelled
                pfileNext->Close();
                goto done;
            }
            
            DebugOutput("   %d total sectors written out so far\r\n", m_cBlocksWritten);

            // We're done with this FST file object
            hr = pfileNext->Close();
            if (hr != S_OK)
                goto done;

            delete pfileNext;
            iObject++;
        }
        nPrevLSN = nEndLSN + 1;
    }

    // If here, then file was successfully created
    hr = S_OK;

done:

    // We're done writing the file.
    outfile.Close();

    g_fInMedia = false;
    return hr;
}
