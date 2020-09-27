//  MANIFEST.CPP
//
//  Created 13-Mar-2001 [JonT]

#include "packer.h"

//---------------------------------------------------------------------

//  CManifest::SetFile
//      Opens a new manifest (overwriting any existing file)

BOOL
CManifest::SetFile(
    LPTSTR lpManifest
    )
{
    Close();

    // Try to open the file
    m_handle = fopen(lpManifest, "w");
    if (m_handle == NULL)
    {
        m_error = errno;
        return FALSE;
    }

    return TRUE;
}


//  CManifest::~CManifest

CManifest::~CManifest()
{
    Close();
}


//  WriteCommand
//      Writes a variable-length command to the manifest
//      You must put a NULL at the end so we know it's complete.

BOOL
CManifest::WriteCommand(
    CLangID* pLangIDs,
    LPCTSTR lpCommand,
    ...
    )
{
    LPSTR lpArg;

    // Write out the list of langIDs
    pLangIDs->MoveFirstLangID();
    fprintf( m_handle, "%04X", pLangIDs->GetCurrentLangID() );
    while( pLangIDs->MoveNextLangID() )
        fprintf( m_handle, ":%04X", pLangIDs->GetCurrentLangID() );

    // Write out the command
    fprintf(m_handle, ",%s", lpCommand);

    va_list vararg;
    va_start(vararg, lpCommand);

    // Loop through all the arguments
    lpArg = va_arg(vararg, LPSTR);
    while (lpArg != NULL)
    {
        // Dump out the argument
        fprintf(m_handle, ",%s", lpArg);

        // Move to the next one
        lpArg = va_arg(vararg, LPSTR);
    }

    va_end(vararg);
    fprintf(m_handle, "\n");
    return TRUE;
}



void
CManifest::Close()
{
    if (m_handle)
        fclose(m_handle);
}
