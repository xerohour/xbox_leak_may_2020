// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CreateXControlFile.cpp
// Contents:  Creates the XControl.dat file
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
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  GetGUID
// Purpose:   Gets the TapeMastering GUID out of the registry and stores it in the specified buffer
// Arguments: pby           -- Buffer to contain the contents of the GUID.
// Return:    'true' if key successfully obtained
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static bool GetGUID(BYTE *pby)
{
    // Length of GUID + NULL terminator == 39
    DWORD cbGUIDLen = 39;
    BYTE  rgbyGUID[255];
	HKEY  Key;
	DWORD dwType;
    GUID guid;

    // Get the mastering guid out of the registry
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\XboxSDK\\", 0, KEY_QUERY_VALUE | KEY_SET_VALUE,
                     &Key) != ERROR_SUCCESS)
        return false;

    if (RegQueryValueEx(Key,"TapeMastering", 0,&dwType, rgbyGUID,
        &cbGUIDLen) != ERROR_SUCCESS || dwType != REG_SZ ||
        RPC_S_OK != UuidFromString(rgbyGUID, &guid))
    {
        // Failed to read the tapemastering key out of the registry.  Create the GUID and put it
        // there ourselves.

        // Create the GUID
        if (CoCreateGuid(&guid) != S_OK)
        {
            RegCloseKey(Key);
            return false;
        }

        // Convert the GUID to string format
        BYTE *rgbyTemp;
        UuidToString(&guid, &rgbyTemp);

        // Write the string GUID to the registry
        if (RegSetValueEx(Key, "TapeMastering", 0, REG_SZ, rgbyTemp, strlen((char*)rgbyTemp)) != ERROR_SUCCESS)
        {
            RegCloseKey(Key);
            return false;
        }

        // We no longer need the allocated string so free it
        RpcStringFree(&rgbyTemp);

        // Reread the value from the registry.  If it fails this time, then we're screwed.
        if (RegQueryValueEx(Key,"TapeMastering", 0,&dwType, rgbyGUID, &cbGUIDLen) != ERROR_SUCCESS)
        {
            RegCloseKey(Key);
            return false;
        }
    }

    // We no longer need the registry key.
    RegCloseKey(Key);

    // copy the guid into the user buffer
    memcpy(pby, &guid, sizeof guid);

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CreateXcontrolFile
// Purpose:   Creates the xcontrol.dat file from the information in the specified fld file.
// Arguments: None
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CStep_Progress::CreateXcontrolFile()
{
    FILETIME fileTime;
    BYTE     rgbyXControl[32*2048];

    // XControl.dat format:
    //  30x2048 bytes of zero
    //  Xbox Control Data Frame
    //    CONTENT                           SIZE    DEFAULT VALUE
    //    Disc category and version #       1       0xD1
    //    Disc size and max transfer rate   1       0x0F
    //    Disc structure                    1       0x31
    //    Recording density                 1       0x10
    //    Reserved                          1       0x00
    //    First Xbox Physical Sector #      3       
    //    Reserved                          1       0x00
    //    Last Xbox Physical Sector #       3
    //    Reserved                          1       0x00
    //    Last Layer 0 Xbox Physical Sector 3       
    //    BCA descriptor                    1
    //    Reserved                          1038    0x00
    //    Content Source Time Stamp         8
    //    Reserved                          20      0x00
    //    Content Source Identifier         16
    //    Reserved                          949     0x00
    //  2048 bytes of zero

    // Because so much of the Data file is zero, we preinit the whole buffer to zero and fill in
    // the rest manually.
    memset(rgbyXControl, 0, sizeof(rgbyXControl));

    // Fill in the default values as described above
    rgbyXControl[30*2048 + 0] = 0xD1;
    rgbyXControl[30*2048 + 1] = 0x0F;
    rgbyXControl[30*2048 + 2] = 0x31;
    rgbyXControl[30*2048 + 3] = 0x10;

    // Add the first xbox physical sector # (on layer 0)
    rgbyXControl[30*2048 + 5] = 0x06;
    rgbyXControl[30*2048 + 6] = 0x06;
    rgbyXControl[30*2048 + 7] = 0x00;

    // Add the last xbox physical sector # (on layer 1)
    rgbyXControl[30*2048 +  9] = 0xF9;
    rgbyXControl[30*2048 + 10] = 0xF9;
    rgbyXControl[30*2048 + 11] = 0xFF;

    // Add the last Xbox physical sector # on Layer 0
    rgbyXControl[30*2048 + 13] = 0x20;
    rgbyXControl[30*2048 + 14] = 0x33;
    rgbyXControl[30*2048 + 15] = 0xAF;

    // Add the BCA descriptor.
    rgbyXControl[30*2048 + 16] = 0x00;

    // Get the current time
	CoFileTimeNow(&fileTime);
    
    // Add the file time
    memcpy(&rgbyXControl[30*2048 + 1055], &fileTime, sizeof fileTime);

	// Add the Content source identifier (GUID that represents the specific content provider that
    // create the IP contained on this disc). The GUID is provided by Microsoft and setup during
    // installation.  The key is stored in "Software\\Microsoft\\XboxSDK\\TapeMastering".
    if (GetGUID(&rgbyXControl[30*2048 + 1083]) == false)
    {
        // Failed to get the GUID from the registry.  Could be bad installation
        MessageBox(m_pdlg->GetHwnd(), "Failed to read the TapeMastering GUID from the Xbox SDK " \
                   "registry key.  Please verify that you have a valid XDK installation.",
                   "Failed to obtain GUID", MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
        return E_FAIL;
    }

    // Create the output file.  The path and filename are separated in case the destination
    // is tape media.
    CFile outfile(m_pdlg, this, m_media, m_szDestPath, "XCONTROL.DAT", false);
    if (!outfile.IsInited())
    {
        // Failed to create the file.  Inform the user
        outfile.InformError(m_pdlg->GetHwnd(), outfile.GetInitCode(), outfile.GetFilename());
        return E_FAIL;
    }

    // Finally, write the buffer to the file.
    HRESULT hr = outfile.Write(rgbyXControl, sizeof(rgbyXControl));
    if (hr != S_OK)
    {
        outfile.Close();
        outfile.InformError(m_pdlg->GetHwnd(), hr, outfile.GetFilename());
        return hr;
    }

    // We're done writing the file.
    hr = outfile.Close();
    if (hr != S_OK)
    {
        outfile.InformError(m_pdlg->GetHwnd(), hr, outfile.GetFilename());
        return hr;
    }

    // Return that the file was successfully generated
    return S_OK;
}
