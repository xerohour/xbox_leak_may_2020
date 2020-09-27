/*++

Copyright (c) 1989-2002  Microsoft Corporation

Module Name:

    imgbldrc.h

Abstract:

    This module contains the resource compiler constants for the utility program
    to build an Xbox executable image.

--*/

#ifndef _IMGBLDRC_
#define _IMGBLDRC_

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
#define IDS_NOT_MACHINE_X86                             1011
#define IDS_NOT_SUBSYSTEM_XBOX                          1012
#define IDS_CANNOT_IMPORT_BY_NAME                       1013
#define IDS_BOUND_IMAGES_UNSUPPORTED                    1014
#define IDS_IGNORING_NOPRELOAD                          1015
#define IDS_MISSING_RELOCATIONS                         1016
#define IDS_NO_VERSION_DETECTED                         1017
#define IDS_TOO_MANY_TESTALTIDS                         1018
#define IDS_INSERTFILE_MISSING_SECTION                  1019
#define IDS_INSERTFILE_TOO_MANY_OPTIONS                 1020
#define IDS_INSERTFILE_CONFLICTING_NAME                 1021
#define IDS_INSERTFILE_CONFLICTING_EXE                  1022
#define IDS_IMAGE_TOO_LARGE                             1023
#define IDS_INTERNAL_TOOL_ERROR                         1024
#define IDS_INVALID_CERTKEY_OPTION                      1025
#define IDS_INVALID_GENERIC_OPTION                      1026
#define IDS_TESTALTIDS_TOO_MANY_OPTIONS                 1027
#define IDS_POSSIBLY_UNAPPROVED                         1028
#define IDS_UNAPPROVED                                  1029
#define IDS_APPROVAL                                    1030
#define IDS_FORMATUD_NOT_POSSIBLE                       1031
#define IDS_EXPIRED                                     1032
#define IDS_INVALID_UDCLUSTER_SIZE                      1033

#define IDS_LOGO_NAME_AND_VERSION                       2000
#define IDS_LOGO_COPYRIGHT                              2001

#define IDS_IMAGEBLD_USAGE                              2200
#define IDS_DUMPXBE_USAGE                               2400

#endif // IMGBLDRC
