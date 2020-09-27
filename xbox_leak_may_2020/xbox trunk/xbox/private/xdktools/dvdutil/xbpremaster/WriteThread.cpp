// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      WriteThread.cpp
// Contents:  This file contains the code related to writing the premastered files out.
// Revisions: 1-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"

DWORD g_dwTapeChecksum = 0;
extern char g_szDefaultOutputFile[MAX_PATH];

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ EXTERNAL FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// GetFSTandBINFromFLD -- Given an FLD file/path, generates the FST and BIN file/path names.
extern bool GetFSTandBINFromFLD(CDlg *pdlg, CStep *pstep, char *szFLDFilePathName, char *szFSTName, char *szBINName);


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::WriteFiles
// Purpose:   Handles writing out the files
// Arguments: None
// Returns:   '0' if successfully written; '-1' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD CStep_Progress::WriteFiles()
{
    char szResult[200];
    int  nResult = -1;
    char szFSTName[MAX_PATH];
    char szBinName[MAX_PATH];
    CFST *pfst = NULL;
    CFLD *pfld = NULL;
    CDX2MLFile *pxml = NULL;
    if (m_fCancelled)
    {
        strcpy(szResult, "User cancelled operation.");
        goto NextStep;
    }

    // Coming into here, we are assured that:
    //  1. The destination directory is valid and created.
    //  2. We have pointers to valid a fld file.

    // Get the name of the FST file.
    if (m_source == SOURCE_PACKAGE)
    {
        strcpy(szFSTName, m_szSourceFile);
        char *szFLDName = strrchr(szFSTName, '\\');
        if (!szFLDName)
            szFLDName = strrchr(szFSTName, '/');
        if (!szFLDName)
            return -1;
        strcpy(szFLDName, "\\_amc\\main.fst");
        strcpy(szBinName, szFSTName);
        strcpy(&szBinName[strlen(szBinName)-8], "gdfs.bin");
    }
    else
    {
        if (!GetFSTandBINFromFLD(m_pdlg, this, m_szSourceFile, szFSTName, szBinName))
            return -1;
    }

    // Create the FST object.  This object contains full source file information for the ximage*
    // files.
    pfst = new CFST(m_pdlg, this, szFSTName);
    if (!pfst->IsInited())
    {
        // Failed to load the fst file
        sprintf(szResult, "Failed to open the FST file '%s'.  Please verify that it is a valid "\
                "Win32 file, and that it is not currently opened by any other applications.",
                szFSTName);

        MessageBox(m_pdlg->GetHwnd(), szResult, "Failed to open FST file", MB_ICONEXCLAMATION | MB_OK);

        // Set the result string and go to the completion step
        strcpy(szResult, "Failed to load FST file");
        goto NextStep;
    }

    // Create the FLD object.  This object will contain all layout information necessary for the
    // dx2ml*.xml files (placeholder information, etc).
    pfld = new CFLD(m_pdlg, this, m_szSourceFile);
    if (!pfld->IsInited())
    {
        // Failed to load the FLD file for some reason.
        if (pfld->IsWrongVersion())
        {
            sprintf(szResult, "The FLD file '%s' is in an obsolete format.  Please rerun xbLayout "\
                    "and save the FLD file again.", m_szSourceFile);
        }
        else
        {
            sprintf(szResult, "Failed to open the FLD file '%s'.  Please verify that it is a valid "\
                    "Win32 file, and that it is not currently opened by any other applications.",
                    m_szSourceFile);
        }
        MessageBox(m_pdlg->GetHwnd(), szResult, "Failed to open FLD file", MB_ICONEXCLAMATION | MB_OK);

        // Set the result string and go to the completion step
        strcpy(szResult, "Failed to open FLD file");
        goto NextStep;
    }

    // If the output media type is 'electronic submission', then zip up all the
    // appropriate files for future premastering.  Otherwise create the xml
    // files (et al)
    if (m_media == MEDIA_ELECSUB)
    {
        char szRoot[MAX_PATH];
        sprintf(szRoot, pfld->m_szDVDRoot);

        // We no longer need the FLD object, and need to release our lock on the
        // FLD file since we'll want to package it up with the other files.
        delete pfld;
        pfld = NULL;

        // Package the files for future premastering.
        // WARNING: PackageFiles will delete the pfst object!
        if ((PackageFiles(pfst, szRoot, m_szSourceFile, szFSTName, szBinName) != S_OK) || (m_fCancelled == true))
        {
            // Failed to package the files!  The function has already informed the user of the problem
            strcpy(szResult, "Failed to create electronic submission package");
            pfst = NULL;
            goto NextStep;
        }
        pfst = NULL;
    }
    else
    {
        // Create xml object
        pxml = new CDX2MLFile;
        if (pxml == NULL)
            ReportError(ERROR_OUTOFMEM);

        // Create the dx2ml.xml file
        if (CreateDx2mlFile(pfld, pxml) != S_OK)
        {
            // Failed to create the file!  The function has already informed the user of the problem
            strcpy(szResult, "Failed to create file \"dx2ml.xml\"");
            goto NextStep;
        }

        // Create the xcontrol.xml file
        if (CreateXcontrolFile() != S_OK)
        {
            // Failed to create the file!  The function has already informed the user of the problem
            strcpy(szResult, "Failed to create file \"xcontrol.dat\"");
            goto NextStep;
        }

        g_cInvalidXbes = 0;
        g_cMaxInvalidXbes = BATCH;
        g_rgstrInvalidXbeName = (sString*)malloc(g_cMaxInvalidXbes*sizeof(sString));

        // Create the ximage0.dat file.
        if (CreateXImageFile(0, pfst, pxml) != S_OK)
        {
            // Failed to create the file!  The function has already informed the user of the problem
            strcpy(szResult, "Failed to create file \"ximage0.dat\"");
            goto NextStep;
        }

        // Create the ximage1.dat file
        if (CreateXImageFile(1, pfst, pxml) != S_OK)
        {
            // Failed to create the file!  The function has already informed the user of the problem
            strcpy(szResult, "Failed to create file \"ximage1.dat\"");
            goto NextStep;
        }
    }

    // The files were successfully written.  Move on to the next step (the 'completion' step).
    strcpy(szResult, "The operation completed successfully");
    nResult = 0;

NextStep:

    g_dwTapeChecksum = m_td.DwCheckSum();

    if (pfst)
        delete pfst;
    if (pfld)
        delete pfld;
    if (pxml)
        delete pxml;

    CStep *pstepNext = new CStep_Completion(m_pdlg, m_source, m_media, m_szDestPath, m_szSourceFile,
                                            szResult);
    if (pstepNext == NULL)
        ReportError(ERROR_OUTOFMEM);

    // If we were command-line driven & successful, then exit now.
    if (nResult == 0 && lstrcmpi(g_szDefaultOutputFile, ""))
        pstepNext = NULL;

    m_pdlg->SetNextStep(pstepNext);

    return nResult;
}
