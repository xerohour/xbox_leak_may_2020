/*++

Copyright (c) 1989-2002  Microsoft Corporation

Module Name:

    libverrc.h

Abstract:

    This module contains the resource compiler constants for the program
    to dump the version of an Xbox library.

--*/

#ifndef _LIBVERRC_
#define _LIBVERRC_

#include <xboxverp.h>

#define IDS_CANNOT_OPEN_INPUT_FILE                      1000
#define IDS_CANNOT_READ_INPUT_FILE                      1001
#define IDS_NO_MEMORY                                   1002
#define IDS_MISSING_OPTION_ARGUMENT                     1003
#define IDS_UNRECOGNIZED_OPTION                         1004
#define IDS_MISSING_REQUIRED_OPTION                     1005
#define IDS_INVALID_NUMBER_OPTION                       1006
#define IDS_INVALID_STRING_OPTION                       1007
#define IDS_CANNOT_CREATE_OUTPUT_FILE                   1008
#define IDS_CANNOT_WRITE_OUTPUT_FILE                    1009
#define IDS_INVALID_CORRUPT_INPUT_FILE                  1010
#define IDS_COULD_NOT_INVOKE_LINK                       1011
#define IDS_INTERNAL_TOOL_ERROR                         1012

#define IDS_NO_VERSION_DETECTED                         1017
#define IDS_POSSIBLY_UNAPPROVED                         1028
#define IDS_UNAPPROVED                                  1029
#define IDS_MULTIPLE_VERSIONS                           1030
#define IDS_VERSION_OUTPUT                              1031

#define IDS_LOGO_NAME_AND_VERSION                       2000
#define IDS_LOGO_COPYRIGHT                              2001

#define IDS_LIBVER_USAGE                                2200

#endif // LIBVERRC
