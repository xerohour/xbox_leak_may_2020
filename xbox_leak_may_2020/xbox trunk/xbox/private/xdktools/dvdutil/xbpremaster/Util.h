// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      Util.h
// Contents:  Simple utilities for the xbPremaster application.
// Revisions: 31-May-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ ENUMERATIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// eError   -- Application-specific error codes.
typedef enum eError
{
    ERROR_INIT,

    ERROR_UNEXPECTED,

    ERROR_OUTOFMEM,
};

// eMedia           -- The options the user has for write/verify media.
typedef enum eMedia
{
    MEDIA_DISK,
        
    // MEDIA_TAPE       -- Tape drive.
    MEDIA_TAPE,

    // MEDIA_ELECSUB    -- Electronic submission
    MEDIA_ELECSUB,
};

// eSource          -- The different sources which premaster can source off of.
typedef enum eSource
{
    // Source file is and FLD
    SOURCE_FLD,

    // Source file is a package file.
    SOURCE_PACKAGE,
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ReportError
// Purpose:   Display appropriate error message to user
// Arguments: error         -- The error message to report
// Return:    None - the application will exit at the end of the function.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
extern void ReportError(eError error);
