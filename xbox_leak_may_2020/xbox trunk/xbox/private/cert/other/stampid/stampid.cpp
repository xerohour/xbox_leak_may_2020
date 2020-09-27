// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      stampid.cpp
// Contents:  Small utility to stamp a new titleid into an XBE file
// Revisions: 19-Sep-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "Precomp.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  DumpUsage
// Purpose:   Outputs usage information
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DumpUsage()
{
    fprintf(stderr, "Microsoft (R) Xbox TITLE ID Stamp Utility "
                    VER_PRODUCTVERSION_STR "\n\n");
    fprintf(stderr, "Usage: STAMPID XBEfile TITLEID\n\n");
    fprintf(stderr, "  XBEfile          The XBE file to stamp the new ID into\n");
    fprintf(stderr, "  TITLEID          The new TITLEID to stamp into the XBE\n"
                    "                   file.  Must be of the form '%%08x'\n\n");
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ParseTITLEID
// Purpose:   Convert the specified string representation of the TITLE ID to
//            to concordant DWORD representation.  The string must be of the
//            form '%08X'.
// Arguments: szTITLEID     -- String to convert
//            pdwTITLEID    -- Location to store DWORD equivalent of string
// Return:    'false' if malformed string, 'true' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool ParseTITLEID(char *szTITLEID, DWORD *pdwTITLEID)
{
    char ch;
    
    if (!szTITLEID || strlen(szTITLEID) != 8)
        return false;

    *pdwTITLEID = 0;

    while ((ch = *szTITLEID++) != '\0')
    {
        if      (ch >= 'a' && ch <= 'f')    ch = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'F')    ch = ch - 'A' + 10;
        else if (ch >= '0' && ch <= '9')    ch = ch - '0';
        else                                return false;
            
        *pdwTITLEID = ((*pdwTITLEID) << 4) + ch;
    }

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  main
// Purpose:   Main entry point into the stampid utility.
// Arguments: argc      -- Number of arguments on the command line
//            argv      -- List of argument strings on command line
// Return:    Application result.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int __cdecl main(int argc, char **argv)
{
    XBEIMAGE_CERTIFICATE cert;
    CCryptContainer      *pccSign = NULL;
    CDevkitCrypt         ccDev;
    CXBEFile             *pxbe;
    DWORD                dwOldTitleID, dwNewTitleID;
    
    // Verify command line arguments
    if (argc != 3)
    {
        DumpUsage();
        return -1;
    }

    if (!ParseTITLEID(argv[2], &dwNewTitleID))
    {
        fprintf(stderr, "Error: TITLEID must be of the form %%08X (ie 1AB4CE38).\n");
        return -1;
    }
    
    // Load the input XBE
    CDiskFile dfXBE(argv[1], GENERIC_READ);
    if(!dfXBE.FReopen(GENERIC_READ | GENERIC_WRITE))
    {
        fprintf(stderr, "Error: cannot open XBE file %s\n", argv[1]);
        return -1;
    }
    pxbe = new CXBEFile(&dfXBE);

    // Validate the input XBE
    pxbe->UseCryptContainer(&ccDev);
    if(!pxbe->FVerifySignature())
    {
        fprintf(stderr, "Error: invalid XBE file %s\n", argv[1]);
        return -1;
    }

    // Construct a new certificate
    memcpy(&cert, pxbe->GetCert(), sizeof cert);

    // Check if the user is trying to set the same title id.
    if (dwNewTitleID == cert.TitleID)
    {
        fprintf(stderr, "XBE file TITLEID is already set to %08X.\n",
                cert.TitleID);
        return -1;
    }

    // Set the TITLE ID
    dwOldTitleID = cert.TitleID;
    cert.TitleID = dwNewTitleID;

    // Set the time of the certificate update
    cert.TimeDateStamp = time(NULL);
    
    // Inject the modified certificate back into the XBE
    if(!pxbe->FInjectNewCertificate(&cert))
    {
cantsign:
        fprintf(stderr, "Error: could not sign XBE file\n");
        return -1;
    }

    // Sign the XBE
    if(!pxbe->FSignFile() || !pxbe->FVerifySignature())
        goto cantsign;

    // Commit the changes to disk
    if(!pxbe->FCommitChanges())
    {
        fprintf(stderr, "Error: Failed to write changes to XBE file\n");
        return -1;
    }

    // Output the results
    fprintf(stderr, "TITLEID successfully changed from %08X to %08X\n\n",
            dwOldTitleID, cert.TitleID);
    return 0;
}

