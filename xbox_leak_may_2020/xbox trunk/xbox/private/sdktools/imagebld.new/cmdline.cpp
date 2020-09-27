/*++

Copyright (c) 2001-2002  Microsoft Corporation

Module Name:

    cmdline.cpp

Abstract:

    This module implements command line related routines for the utility program
    to build an Xbox executable image.

--*/

#include "imgbldp.h"

//
// Stores the path to the input file.
//
LPSTR ImgbInputFilePath;

//
// Stores the path to the output file.
//
LPSTR ImgbOutputFilePath;

//
// Header node for the list of NOPRELOAD sections.
//
LIST_ENTRY ImgbNoPreloadList = {
    &ImgbNoPreloadList, &ImgbNoPreloadList
};

//
// Header node for the list of INSERTFILE sections.
//
LIST_ENTRY ImgbInsertFileList = {
    &ImgbInsertFileList, &ImgbInsertFileList
};

//
// Stores whether or not a copy of the original PE header should be emitted to
// the output file.
//
BOOLEAN ImgbEmitPEHeader;

//
// Stores the number of bytes to allocate for default sized stacks.
//
ULONG ImgbSizeOfStack;

//
// Stores the initialization flags to store in the image.
//
ULONG ImgbInitFlags = XINIT_MOUNT_UTILITY_DRIVE;

//
// Stores whether or not the system memory available to the image should be
// limited to 64 megabytes (/LIMITMEM).
//
BOOLEAN ImgbLimitMemory;

//
// Stores whether or not the hard disk should be initialized by the XAPI startup
// code (/NOSETUPHD).
//
BOOLEAN ImgbNoSetupHardDisk;

//
// Stores whether or not the hard disk should be modified by the XAPI startup
// code (/DONTMODIFYHD).
//
BOOLEAN ImgbDontModifyHardDisk;

//
// Stores whether or not the utility drive should be mounted during XAPI startup
// code (/DONTMOUNTUD).
//
BOOLEAN ImgbDontMountUtilityDrive;

//
// Stores whether or not the utility drive should be formatted during XAPI
// startup code (/FORMATUD).
//
BOOLEAN ImgbFormatUtilityDrive;

//
// Stores the size of a utility drive cluster in bytes.  Initialized to zero so
// that we only touch the initialization flags if this value is explicitly set
// on the command line.
//
ULONG ImgbUtilityDriveClusterSize;

//
// Stores the version number to store in the image certificate.
//
ULONG ImgbVersion;

//
// Stores the test game region to store in the image certificate.
//
ULONG ImgbTestGameRegion = XBEIMAGE_GAME_REGION_NA | XBEIMAGE_GAME_REGION_JAPAN |
    XBEIMAGE_GAME_REGION_RESTOFWORLD | XBEIMAGE_GAME_REGION_MANUFACTURING;

//
// Stores the test allowed media types bitmask to store in the image certificate.
//
ULONG ImgbTestAllowedMediaTypes = XBEIMAGE_MEDIA_TYPE_HARD_DISK |
    XBEIMAGE_MEDIA_TYPE_DVD_CD | XBEIMAGE_MEDIA_TYPE_MEDIA_BOARD;

//
// Stores the test game ratings to store in the image certificate.
//
ULONG ImgbTestGameRatings = MAXULONG;

//
// Stores the test title identifier to store in the image certificate.
//
ULONG ImgbTestTitleID;

//
// Stores the test alternate title identifiers to store in the image certificate.
//
ULONG ImgbTestAlternateTitleIDs[XBEIMAGE_ALTERNATE_TITLE_ID_COUNT];

//
// Stores the test alternate signature keys to store in the image certificate.
//
XBEIMAGE_CERTIFICATE_KEY ImgbTestAlternateSignatureKeys[XBEIMAGE_ALTERNATE_TITLE_ID_COUNT];

//
// Stores the number of test alternate title identifiers that have been claimed.
//
ULONG ImgbNumberOfTestAlternateTitleIDs;

//
// Stores the test title name to store in the image certificate.
//
WCHAR ImgbTestTitleName[XBEIMAGE_TITLE_NAME_LENGTH];

//
// Stores the test LAN key to store in the image certificate.
//
UCHAR ImgbTestLANKey[XBEIMAGE_CERTIFICATE_KEY_LENGTH] = {
    (UCHAR)'T', (UCHAR)'E', (UCHAR)'S', (UCHAR)'T',
    (UCHAR)'T', (UCHAR)'E', (UCHAR)'S', (UCHAR)'T',
    (UCHAR)'T', (UCHAR)'E', (UCHAR)'S', (UCHAR)'T',
    (UCHAR)'T', (UCHAR)'E', (UCHAR)'S', (UCHAR)'T',
};

//
// Stores the test signature key to store in the image certificate.
//
UCHAR ImgbTestSignatureKey[XBEIMAGE_CERTIFICATE_KEY_LENGTH] = {
    (UCHAR)'T', (UCHAR)'E', (UCHAR)'S', (UCHAR)'T',
    (UCHAR)'T', (UCHAR)'E', (UCHAR)'S', (UCHAR)'T',
    (UCHAR)'T', (UCHAR)'E', (UCHAR)'S', (UCHAR)'T',
    (UCHAR)'T', (UCHAR)'E', (UCHAR)'S', (UCHAR)'T',
};

//
// Stores the file name of the title image to store in the image.
//
LPSTR ImgbTitleImage;

//
// Stores the file name of the title info to store in the image.
//
LPSTR ImgbTitleInfo;

//
// Stores the file name of the default save image to store in the image.
//
LPSTR ImgbDefaultSaveImage;

LPSTR
ImgbDuplicateString(
    LPCSTR pszString
    )
{
    LPSTR pszDuplicatedString;

    pszDuplicatedString = (LPSTR)ImgbAllocateMemory(strlen(pszString) + 1);

    strcpy(pszDuplicatedString, pszString);

    return pszDuplicatedString;
}

BOOLEAN
ImgbMatchCommandOptionStringValue(
    LPCSTR pszCommandOption,
    LPCSTR pszMatchString,
    LPSTR *ppszStringValue
    )
{
    UINT MatchStringLength;

    MatchStringLength = strlen(pszMatchString);

    //
    // Check if the string is prefixed with the match string.
    //

    if (_strnicmp(pszCommandOption, pszMatchString, MatchStringLength) != 0) {
        return FALSE;
    }

    //
    // Check if the switch is followed by a command argument.  If there isn't a
    // colon and this isn't the end of the string, then this isn't really our
    // switch, so bail out.  If there isn't a colon or the colon is followed by
    // an empty string, then the switch's argument is missing, so error out.
    //

    if (pszCommandOption[MatchStringLength] != ':') {

        if (pszCommandOption[MatchStringLength] == '\0') {
            ImgbResourcePrintErrorAndExit(IDS_MISSING_OPTION_ARGUMENT,
                pszMatchString);
        }

        return FALSE;

    } else if (pszCommandOption[MatchStringLength + 1] == '\0') {
        ImgbResourcePrintErrorAndExit(IDS_MISSING_OPTION_ARGUMENT, pszMatchString);
    }

    //
    // If we've already seen the switch, then override it with the new value.
    //

    if (*ppszStringValue != NULL) {
        ImgbFreeMemory(*ppszStringValue);
    }

    //
    // Duplicate the switch's argument.
    //

    *ppszStringValue = ImgbDuplicateString(&pszCommandOption[MatchStringLength + 1]);

    return TRUE;
}

VOID
ImgbParseInteger(
    LPCSTR pszMatchString,
    LPCSTR pszInteger,
    ULONG *pulIntegerValue
    )
{
    LPSTR pszEndParse;

    //
    // Convert the string switch to an integer.
    //

    errno = 0;

    *pulIntegerValue = strtoul(pszInteger, &pszEndParse, 0);

    //
    // Verify that the entire string was consumed; if not, then there's invalid
    // data at the end of the switch, so error out.
    //
    // Also verify that no underflow or overflow error has occurred.
    //

    if (pszEndParse != pszInteger + strlen(pszInteger) || (errno == ERANGE)) {
        ImgbResourcePrintErrorAndExit(IDS_INVALID_NUMBER_OPTION, pszMatchString);
    }
}

BOOLEAN
ImgbMatchCommandOptionIntegerValue(
    LPCSTR pszCommandOption,
    LPCSTR pszMatchString,
    ULONG *pulIntegerValue
    )
{
    LPSTR pszStringValue;

    pszStringValue = NULL;

    //
    // Attempt to parse the switch as a string.
    //

    if (!ImgbMatchCommandOptionStringValue(pszCommandOption, pszMatchString,
        &pszStringValue)) {
        return FALSE;
    }

    //
    // Parse the string as an integer.
    //

    ImgbParseInteger(pszMatchString, pszStringValue, pulIntegerValue);

    ImgbFreeMemory(pszStringValue);

    return TRUE;
}

ULONG
ImgbHexCharacterToInteger(
    CHAR Character
    )
{
    if (isdigit(Character)) {
        return Character - '0';
    } else {
        return toupper(Character) - 'A' + 10;
    }
}

VOID
ImgbParseCertificateKey(
    LPCSTR pszMatchString,
    LPCSTR pszCertificateKey,
    PUCHAR CertificateKey
    )
{
    ULONG BytesRemaining;
    LPCSTR pszParse;

    //
    // Verify that the key parameter is the correct length.
    //

    if (strlen(pszCertificateKey) != XBEIMAGE_CERTIFICATE_KEY_LENGTH * 2) {
        ImgbResourcePrintErrorAndExit(IDS_INVALID_CERTKEY_OPTION, pszMatchString);
    }

    //
    // Parse the certificate key and store the result in the supplied buffer.
    //

    BytesRemaining = XBEIMAGE_CERTIFICATE_KEY_LENGTH;
    pszParse = pszCertificateKey;

    while (BytesRemaining > 0) {

        if (!isxdigit(*pszParse) || !isxdigit(*(pszParse + 1))) {
            ImgbResourcePrintErrorAndExit(IDS_INVALID_CERTKEY_OPTION, pszMatchString);
        }

        *CertificateKey++ = (UCHAR)((ImgbHexCharacterToInteger(*pszParse) << 4) +
            ImgbHexCharacterToInteger(*(pszParse + 1)));

        BytesRemaining--;
        pszParse += 2;
    }
}

BOOLEAN
ImgbMatchCommandOptionCertificateKey(
    LPCSTR pszCommandOption,
    LPCSTR pszMatchString,
    PUCHAR CertificateKey
    )
{
    LPSTR pszStringValue;

    pszStringValue = NULL;

    //
    // Attempt to parse the switch as a string.
    //

    if (!ImgbMatchCommandOptionStringValue(pszCommandOption, pszMatchString,
        &pszStringValue)) {
        return FALSE;
    }

    //
    // Parse the string as a certificate.
    //

    ImgbParseCertificateKey(pszMatchString, pszStringValue, CertificateKey);

    ImgbFreeMemory(pszStringValue);

    return TRUE;
}

VOID
ImgbProcessCommandOption(
    LPCSTR pszCommandOption
    )
{
    LPSTR pszStringValue;
    PIMGB_NOPRELOAD NoPreloadLink;
    LPSTR pszAlternateSignatureKey;
    PIMGB_INSERTFILE InsertFileLink;
    LPSTR pszInsertFileAttributes;

    pszStringValue = NULL;

    //
    // If this isn't a switch option, then this must be the input file path.
    //

    if (*pszCommandOption != '-' && *pszCommandOption != '/') {

        if (ImgbInputFilePath != NULL) {
            ImgbFreeMemory(ImgbInputFilePath);
        }

        ImgbInputFilePath = ImgbDuplicateString(pszCommandOption);

        return;
    }

    //
    // Skip past the switch character.
    //

    pszCommandOption++;

    //
    // Attempt to match the /IN:xxxx switch.
    //

    if (ImgbMatchCommandOptionStringValue(pszCommandOption, "IN", &ImgbInputFilePath)) {
        return;
    }

    //
    // Attempt to match the /OUT:xxxx switch.
    //

    if (ImgbMatchCommandOptionStringValue(pszCommandOption, "OUT", &ImgbOutputFilePath)) {
        return;
    }

    //
    // Attempt to match the /NOPRELOAD:xxxx switch.  If found, add the section
    // name to the NOPRELOAD list.
    //

    if (ImgbMatchCommandOptionStringValue(pszCommandOption, "NOPRELOAD", &pszStringValue)) {

        NoPreloadLink = (PIMGB_NOPRELOAD)ImgbAllocateMemory(sizeof(IMGB_NOPRELOAD));

        NoPreloadLink->SectionName = pszStringValue;

        InsertTailList(&ImgbNoPreloadList, &NoPreloadLink->ListEntry);

        return;
    }

    //
    // Attempt to match the /STACK:xxxx switch.
    //

    if (ImgbMatchCommandOptionIntegerValue(pszCommandOption, "STACK", &ImgbSizeOfStack)) {
        return;
    }

    //
    // Attempt to match the /INITFLAGS:xxxx switch.
    //

    if (ImgbMatchCommandOptionIntegerValue(pszCommandOption, "INITFLAGS", &ImgbInitFlags)) {
        return;
    }

    //
    // Attempt to match the /VERSION:xxxx switch.
    //

    if (ImgbMatchCommandOptionIntegerValue(pszCommandOption, "VERSION", &ImgbVersion)) {
        return;
    }

    //
    // Attempt to match the /TESTVERSION:xxxx switch.
    //

    if (ImgbMatchCommandOptionIntegerValue(pszCommandOption, "TESTVERSION", &ImgbVersion)) {
        return;
    }

    //
    // Attempt to match the /TESTREGION:xxxx switch.
    //

    if (ImgbMatchCommandOptionIntegerValue(pszCommandOption, "TESTREGION", &ImgbTestGameRegion)) {
        return;
    }

    //
    // Attempt to match the /TESTMEDIATYPES:xxxx switch.
    //

    if (ImgbMatchCommandOptionIntegerValue(pszCommandOption, "TESTMEDIATYPES", &ImgbTestAllowedMediaTypes)) {
        return;
    }

    //
    // Attempt to match the /TESTRATINGS:xxxx switch.
    //

    if (ImgbMatchCommandOptionIntegerValue(pszCommandOption, "TESTRATINGS", &ImgbTestGameRatings)) {
        return;
    }

    //
    // Attempt to match the /TESTID:xxxx switch.
    //

    if (ImgbMatchCommandOptionIntegerValue(pszCommandOption, "TESTID", &ImgbTestTitleID)) {
        return;
    }

    //
    // Attempt to match the /TESTALTID:xxxx switch
    //

    if (ImgbMatchCommandOptionStringValue(pszCommandOption, "TESTALTID", &pszStringValue)) {

        if (ImgbNumberOfTestAlternateTitleIDs >= XBEIMAGE_ALTERNATE_TITLE_ID_COUNT) {
            ImgbResourcePrintErrorAndExit(IDS_TOO_MANY_TESTALTIDS);
        }

        ImgbParseInteger("TESTALTID", strtok(pszStringValue, ","),
            &ImgbTestAlternateTitleIDs[ImgbNumberOfTestAlternateTitleIDs]);

        pszAlternateSignatureKey = strtok(NULL, ",");

        if (pszAlternateSignatureKey != NULL) {

            ImgbParseCertificateKey("TESTALTID", pszAlternateSignatureKey,
                ImgbTestAlternateSignatureKeys[ImgbNumberOfTestAlternateTitleIDs]);

            if (strtok(NULL, ",") != NULL) {
                ImgbResourcePrintErrorAndExit(IDS_INSERTFILE_TOO_MANY_OPTIONS);
            }
        }

        ImgbNumberOfTestAlternateTitleIDs++;

        ImgbFreeMemory(pszStringValue);

        return;
    }

    //
    // Attempt to match the /TESTNAME:xxxx switch.
    //

    if (ImgbMatchCommandOptionStringValue(pszCommandOption, "TESTNAME", &pszStringValue)) {

        if (MultiByteToWideChar(CP_ACP, 0, pszStringValue, -1, ImgbTestTitleName,
            XBEIMAGE_TITLE_NAME_LENGTH) == 0) {

            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
                ImgbResourcePrintErrorAndExit(IDS_INVALID_STRING_OPTION, "TESTNAME");
            }
        }

        ImgbFreeMemory(pszStringValue);

        return;
    }

    //
    // Attempt to match the /TESTLANKEY:xxxx switch.
    //

    if (ImgbMatchCommandOptionCertificateKey(pszCommandOption, "TESTLANKEY",
        ImgbTestLANKey)) {
        return;
    }

    //
    // Attempt to match the /TESTSIGNKEY:xxxx switch.
    //

    if (ImgbMatchCommandOptionCertificateKey(pszCommandOption, "TESTSIGNKEY",
        ImgbTestSignatureKey)) {
        return;
    }

    //
    // Attempt to match the /TITLEIMAGE:xxxx switch.
    //

    if (ImgbMatchCommandOptionStringValue(pszCommandOption, "TITLEIMAGE", &ImgbTitleImage)) {
        return;
    }

    //
    // Attempt to match the /TITLEINFO:xxxx switch.
    //

    if (ImgbMatchCommandOptionStringValue(pszCommandOption, "TITLEINFO", &ImgbTitleInfo)) {
        return;
    }

    //
    // Attempt to match the /DEFAULTSAVEIMAGE:xxxx switch.
    //

    if (ImgbMatchCommandOptionStringValue(pszCommandOption, "DEFAULTSAVEIMAGE", &ImgbDefaultSaveImage)) {
        return;
    }

    //
    // Attempt to match the /INSERTFILE:xxxx switch.
    //

    if (ImgbMatchCommandOptionStringValue(pszCommandOption, "INSERTFILE", &pszStringValue)) {

        InsertFileLink = (PIMGB_INSERTFILE)ImgbAllocateMemory(sizeof(IMGB_INSERTFILE));

        InsertFileLink->FilePath = strtok(pszStringValue, ",");
        InsertFileLink->SectionName = strtok(NULL, ",");

        if (InsertFileLink->SectionName == NULL) {
            ImgbResourcePrintErrorAndExit(IDS_INSERTFILE_MISSING_SECTION);
        }

        pszInsertFileAttributes = strtok(NULL, ",");

        if (pszInsertFileAttributes != NULL) {

            while (*pszInsertFileAttributes != '\0') {

                switch (toupper(*pszInsertFileAttributes)) {

                    case 'R':
                        InsertFileLink->ReadOnly = TRUE;
                        break;

                    case 'N':
                        InsertFileLink->NoPreload = TRUE;
                        break;

                    default:
                        ImgbResourcePrintErrorAndExit(IDS_INVALID_GENERIC_OPTION,
                            "INSERTFILE");
                }

                pszInsertFileAttributes++;
            }

            if (strtok(NULL, ",") != NULL) {
                ImgbResourcePrintErrorAndExit(IDS_INSERTFILE_TOO_MANY_OPTIONS);
            }
        }

        InsertTailList(&ImgbInsertFileList, &InsertFileLink->ListEntry);

        return;
    }

    //
    // Attempt to match the /NOLIBWARN switch.
    //

    if (_stricmp(pszCommandOption, "NOLIBWARN") == 0) {
        ImgbNoWarnLibraryApproval = TRUE;
        return;
    }

    //
    // Attempt to match the /PEHEADER switch.
    //

    if (_strnicmp(pszCommandOption, "PEHEADER", 8) == 0) {
        ImgbEmitPEHeader = TRUE;
        return;
    }

    //
    // Attempt to match the /LIMITMEM switch.
    //

    if (_stricmp(pszCommandOption, "LIMITMEM") == 0) {
        ImgbLimitMemory = TRUE;
        return;
    }

    //
    // Attempt to match the /NOSETUPHD switch.
    //

    if (_stricmp(pszCommandOption, "NOSETUPHD") == 0) {
        ImgbNoSetupHardDisk = TRUE;
        return;
    }

    //
    // Attempt to match the /DONTMODIFYHD switch.
    //

    if (_stricmp(pszCommandOption, "DONTMODIFYHD") == 0) {
        ImgbDontModifyHardDisk = TRUE;
        return;
    }

    //
    // Attempt to match the /DONTMOUNTUD switch.
    //

    if (_stricmp(pszCommandOption, "DONTMOUNTUD") == 0) {
        ImgbDontMountUtilityDrive = TRUE;

        if (ImgbFormatUtilityDrive) {
            ImgbResourcePrintErrorAndExit(IDS_FORMATUD_NOT_POSSIBLE);
        }

        return;
    }

    //
    // Attempt to match the /FORMATUD switch.
    //

    if (_stricmp(pszCommandOption, "FORMATUD") == 0) {
        ImgbFormatUtilityDrive = TRUE;

        if (ImgbDontMountUtilityDrive) {
            ImgbResourcePrintErrorAndExit(IDS_FORMATUD_NOT_POSSIBLE);
        }

        return;
    }

    //
    // Attempt to match the /UDCLUSTER switch.  Accept the integer as either
    // the number of bytes or the number of kilobytes per cluster.
    //

    if (ImgbMatchCommandOptionIntegerValue(pszCommandOption, "UDCLUSTER",
        &ImgbUtilityDriveClusterSize)) {

        switch (ImgbUtilityDriveClusterSize) {

            case 16:
            case 16384:
                ImgbUtilityDriveClusterSize = 16384;
                break;

            case 32:
            case 32768:
                ImgbUtilityDriveClusterSize = 32768;
                break;

            case 64:
            case 65536:
                ImgbUtilityDriveClusterSize = 65536;
                break;

            default:
                ImgbResourcePrintErrorAndExit(IDS_INVALID_UDCLUSTER_SIZE);
                break;
        }

        return;
    }

    //
    // Attempt to match the /NOLOGO switch.  This switch has no affect since we
    // don't display the banner unless we're displaying help text.  This switch
    // exists for compatibility with the old version of IMAGEBLD.
    //

    if (_stricmp(pszCommandOption, "NOLOGO") == 0) {
        return;
    }

    //
    // Attempt to match the /DEBUG switch.  This is equivalent to /PEHEADER.
    //

    if (_stricmp(pszCommandOption, "DEBUG") == 0) {
        ImgbEmitPEHeader = TRUE;
        return;
    }

    //
    // Attempt to match the help switches.  If a help switch is found, display
    // the tool's usage and exit the program.
    //

    if ((_stricmp(pszCommandOption, "?") == 0) ||
        (_stricmp(pszCommandOption, "HELP") == 0)) {
        ImgbResourcePrintLogoBanner();
        ImgbResourcePrintRange(stderr, IDS_IMAGEBLD_USAGE);
        ImgbExitProcess(0);
    }

    //
    // This is an unrecognized option, so error out.
    //

    ImgbResourcePrintErrorAndExit(IDS_UNRECOGNIZED_OPTION, pszCommandOption);
}

VOID
ImgbProcessCommandFile(
    LPSTR pszCommandFile
    )
{
    FILE *CommandFile;
    CHAR szLine[1024];
    LPSTR pszCurrent;
    LPSTR pszCommandOption;
    LPSTR pszCommandOptionEnd;

    //
    // Open the command file for read access.
    //

    CommandFile = fopen(pszCommandFile, "r");

    if (CommandFile == NULL) {
        ImgbResourcePrintErrorAndExit(IDS_CANNOT_OPEN_INPUT_FILE, pszCommandFile);
    }

    //
    // Process each line from the command file.
    //

    while (fgets(szLine, sizeof(szLine), CommandFile) != NULL) {

        pszCurrent = szLine;

        for (;;) {

            //
            // Advance past any whitespace and break out if we've reached the
            // end of the line.
            //

            while (isspace(*pszCurrent)) {
                pszCurrent++;
            }

            if (*pszCurrent == '\0') {
                break;
            }

            pszCommandOption = pszCurrent;
            pszCommandOptionEnd = pszCommandOption;

            while (*pszCurrent != '\0' && !isspace(*pszCurrent)) {

                if (*pszCurrent == '\"') {

                    pszCurrent++;

                    while (*pszCurrent != '\0' && *pszCurrent != '\"' && *pszCurrent != '\n') {
                        *pszCommandOptionEnd++ = *pszCurrent++;
                    }

                    if (*pszCurrent == '\"') {
                        pszCurrent++;
                    }

                } else {
                    *pszCommandOptionEnd++ = *pszCurrent++;
                }
            }

            //
            // If we hit whitespace and not the end of the line, then bump up
            // to the next character so that when we terminate the command
            // option string below, we don't think that we've reached the end of
            // the line above.
            //

            if (*pszCurrent != '\0') {
                pszCurrent++;
            }

            //
            // Terminate the command option string.
            //

            *pszCommandOptionEnd++ = '\0';

            //
            // Process the command option.
            //

            ImgbProcessCommandOption(pszCommandOption);
        }
    }

    //
    // If an error occurred when reading from the file, as opposed to reaching
    // the end of the file, then exit with an error.
    //

    if (ferror(CommandFile)) {
        ImgbResourcePrintErrorAndExit(IDS_CANNOT_READ_INPUT_FILE, pszCommandFile);
    }

    //
    // Close the command file.
    //

    fclose(CommandFile);
}

VOID
ImgbProcessCommandLineOptions(
    int argc,
    char *argv[]
    )
{
    PIMGB_INSERTFILE InsertFileLink;

    //
    // Skip past the executable file name parameter (argv[0]).
    //

    argv++;
    argc--;

    //
    // If no arguments were specified, then print out the tool's usage.
    //

    if (argc == 0) {
        ImgbResourcePrintLogoBanner();
        ImgbResourcePrintRange(stderr, IDS_IMAGEBLD_USAGE);
        ImgbExitProcess(0);
    }

    //
    // Check if the first switch is a request to run one of this tool's other
    // built-in tools.
    //

    if (argv[0][0] == '-' || argv[0][0] == '/') {

        //
        // Check if we should dump out the contents of an XBE file.
        //

        if (_stricmp(&argv[0][1], "DUMP") == 0) {
            ImgbDumpExecutable(argc, argv);
        }
    }

    //
    // Process the command line options.
    //

    do {

        if (argv[0][0] != '@') {
            ImgbProcessCommandOption(argv[0]);
        } else {
            ImgbProcessCommandFile(&argv[0][1]);
        }

        argv++;
        argc--;

    } while (argc > 0);

    //
    // If the /LIMITMEM switch was specified, then add the flag to the
    // initialization flags.
    //

    if (ImgbLimitMemory) {
        ImgbInitFlags |= XINIT_LIMIT_DEVKIT_MEMORY;
    }

    //
    // If the /NOSETUPHD switch was specified, then add the flag to the
    // initialization flags.
    //

    if (ImgbNoSetupHardDisk) {
        ImgbInitFlags |= XINIT_NO_SETUP_HARD_DISK;
    }

    //
    // If the /DONTMODIFYHD switch was specified, then add the flag to the
    // initialization flags.
    //

    if (ImgbDontModifyHardDisk) {
        ImgbInitFlags |= XINIT_DONT_MODIFY_HARD_DISK;
    }

    //
    // If the /FORMATUD switch was specified, then add the flag to the
    // initialization flags.
    //

    if (ImgbFormatUtilityDrive) {
        ImgbInitFlags |= XINIT_FORMAT_UTILITY_DRIVE;
    }

    //
    // If the /DONTMOUNTUD switch was specified, then remove the flag from the
    // initialization flags.
    //

    if (ImgbDontMountUtilityDrive) {
        ImgbInitFlags &= ~XINIT_MOUNT_UTILITY_DRIVE;
    }

    //
    // If the /UDCLUSTER switch was specified, then add the flag for desired
    // cluster size.  Note that if no cluster size was specified, then we'll
    // either use the default value of 16K or the value specified via the
    // /INITFLAGS switch.
    //

    switch (ImgbUtilityDriveClusterSize) {

        case 16384:
            ImgbInitFlags &= ~XINIT_UTILITY_DRIVE_CLUSTER_SIZE_MASK;
            ImgbInitFlags |= XINIT_UTILITY_DRIVE_16K_CLUSTER_SIZE;
            break;

        case 32768:
            ImgbInitFlags &= ~XINIT_UTILITY_DRIVE_CLUSTER_SIZE_MASK;
            ImgbInitFlags |= XINIT_UTILITY_DRIVE_32K_CLUSTER_SIZE;
            break;

        case 65536:
            ImgbInitFlags &= ~XINIT_UTILITY_DRIVE_CLUSTER_SIZE_MASK;
            ImgbInitFlags |= XINIT_UTILITY_DRIVE_64K_CLUSTER_SIZE;
            break;
    }

    //
    // If the /DEFAULTSAVEIMAGE switch was specified, then add an insert file
    // node for the file.
    //

    if (ImgbDefaultSaveImage != NULL) {

        InsertFileLink = (PIMGB_INSERTFILE)ImgbAllocateMemory(sizeof(IMGB_INSERTFILE));

        InsertFileLink->FilePath = ImgbDefaultSaveImage;
        InsertFileLink->SectionName = "$$XSIMAGE";
        InsertFileLink->NoPreload = TRUE;
        InsertFileLink->ReadOnly = TRUE;

        InsertHeadList(&ImgbInsertFileList, &InsertFileLink->ListEntry);
    }

    //
    // If the /TITLEIMAGE switch was specified, then add an insert file node for
    // the file.
    //

    if (ImgbTitleImage != NULL) {

        InsertFileLink = (PIMGB_INSERTFILE)ImgbAllocateMemory(sizeof(IMGB_INSERTFILE));

        InsertFileLink->FilePath = ImgbTitleImage;
        InsertFileLink->SectionName = "$$XTIMAGE";
        InsertFileLink->NoPreload = TRUE;
        InsertFileLink->ReadOnly = TRUE;

        InsertHeadList(&ImgbInsertFileList, &InsertFileLink->ListEntry);
    }

    //
    // If a /TITLEINFO switch was specified, then add an insert file node for
    // the file.
    //

    if (ImgbTitleInfo != NULL) {

        InsertFileLink = (PIMGB_INSERTFILE)ImgbAllocateMemory(sizeof(IMGB_INSERTFILE));

        InsertFileLink->FilePath = ImgbTitleInfo;
        InsertFileLink->SectionName = "$$XTINFO";
        InsertFileLink->NoPreload = TRUE;
        InsertFileLink->ReadOnly = TRUE;

        InsertHeadList(&ImgbInsertFileList, &InsertFileLink->ListEntry);
    }
}

BOOLEAN
ImgbSearchNoPreloadList(
    LPCSTR pszSectionName
    )
{
    PLIST_ENTRY NextListEntry;
    PIMGB_NOPRELOAD NoPreloadLink;

    NextListEntry = ImgbNoPreloadList.Flink;

    while (NextListEntry != &ImgbNoPreloadList) {

        NoPreloadLink = CONTAINING_RECORD(NextListEntry, IMGB_NOPRELOAD,
            ListEntry);

        if (_stricmp(NoPreloadLink->SectionName, pszSectionName) == 0) {
            return TRUE;
        }

        NextListEntry = NoPreloadLink->ListEntry.Flink;
    }

    return FALSE;
}
