// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      util.h
// Contents:  
// Revisions: 7-Jul-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CObjList;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  DebugOutput
// Purpose:   Outputs a variable-argument string to the debugger.  Used like
//            printf.
// Arguments: sz            -- Format to use (ie the first parm to printf() )
//            ...           -- [optional] Variable number of arguments.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DebugOutput(char *sz, ...);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  DumpLayer
// Purpose:   Temp debug function
// Arguments: pol           -- List to dump
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DumpLayer(CObjList *pol);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  FormatByteString
// Purpose:   
// Arguments: 
// Return:   None 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FormatByteString(DWORD dwVal, char *szDest);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  FatalError
// Purpose:   Called when an unrecoverable error has occurred.  Forces exit
//            after notification
// Arguments: dwErr         -- The error that occurred
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FatalError(DWORD dwErr);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  MessageBoxResource
// Purpose:   Display a messagebox using string resources
// Arguments: hwnd        -- Handle to the window 'owning' the messagebox
//            dwTextId    -- String resource id of the text string
//            dwCaptionId -- String resource id of the caption string
//            dwFlags     -- Flags for the messagebox
// Return:    Button pressed
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD MessageBoxResource(HWND hwnd, DWORD dwTextId, DWORD dwCaptionId,
                        DWORD dwFlags);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  MessageBoxResourceParam
// Purpose:   Display a messagebox using string resources.  Expects one string
//            param for the text string.
// Arguments: hwnd        -- Handle to the window 'owning' the messagebox
//            dwTextId    -- String resource id of the text string
//            szParam     -- String param for text string.
//            dwCaptionId -- String resource id of the caption string
//            dwFlags     -- Flags for the messagebox
// Return:    Button pressed
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD MessageBoxResourceParam(HWND hwnd, DWORD dwTextId, char *szParam,
                             DWORD dwCaptionId, DWORD dwFlags);
