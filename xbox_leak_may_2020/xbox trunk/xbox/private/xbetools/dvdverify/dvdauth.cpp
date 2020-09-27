// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      dvdauth.cpp
// Contents:  Contains DVD authentication-related functionality.  Code is
//            99% cut-and-paste from ntos\idex\cdrom.c.
// Revisions: 23-Aug-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "recovpch.h"
#include "imagefile.h"
#include "xsum.h"
#include <idex.h>
#include <assert.h>

// Imagebld public key stolen from sdktools\imagebld.new\keydata.cpp.  Used
// as a red key.
UCHAR RedPublicKeyData[] = {
    0x52,0x53,0x41,0x31,0x08,0x01,0x00,0x00,
    0x00,0x08,0x00,0x00,0xFF,0x00,0x00,0x00,
    0x01,0x00,0x01,0x00,0x9B,0x83,0xD4,0xD5,
    0xDE,0x16,0x25,0x8E,0xE5,0x15,0xF2,0x18,
    0x9D,0x19,0x1C,0xF8,0xFE,0x91,0xA5,0x83,
    0xAE,0xA5,0xA8,0x95,0x3F,0x01,0xB2,0xC9,
    0x34,0xFB,0xC7,0x51,0x2D,0xAC,0xFF,0x38,
    0xE6,0xB6,0x7B,0x08,0x4A,0xDF,0x98,0xA3,
    0xFD,0x31,0x81,0xBF,0xAA,0xD1,0x62,0x58,
    0xC0,0x6C,0x8F,0x8E,0xCD,0x96,0xCE,0x6D,
    0x03,0x44,0x59,0x93,0xCE,0xEA,0x8D,0xF4,
    0xD4,0x6F,0x6F,0x34,0x5D,0x50,0xF1,0xAE,
    0x99,0x7F,0x1D,0x92,0x15,0xF3,0x6B,0xDB,
    0xF9,0x95,0x8B,0x3F,0x54,0xAD,0x37,0xB5,
    0x4F,0x0A,0x58,0x7B,0x48,0xA2,0x9F,0x9E,
    0xA3,0x16,0xC8,0xBD,0x37,0xDA,0x9A,0x37,
    0xE6,0x3F,0x10,0x1B,0xA8,0x4F,0xA3,0x14,
    0xFA,0xBE,0x12,0xFB,0xD7,0x19,0x4C,0xED,
    0xAD,0xA2,0x95,0x8F,0x39,0x8C,0xC4,0x69,
    0x0F,0x7D,0xB8,0x84,0x0A,0x99,0x5C,0x53,
    0x2F,0xDE,0xF2,0x1B,0xC5,0x1D,0x4C,0x43,
    0x3C,0x97,0xA7,0xBA,0x8F,0xC3,0x22,0x67,
    0x39,0xC2,0x62,0x74,0x3A,0x0C,0xB5,0x57,
    0x01,0x3A,0x67,0xC6,0xDE,0x0C,0x0B,0xF6,
    0x08,0x01,0x64,0xDB,0xBD,0x81,0xE4,0xDC,
    0x09,0x2E,0xD0,0xF1,0xD0,0xD6,0x1E,0xBA,
    0x38,0x36,0xF4,0x4A,0xDD,0xCA,0x39,0xEB,
    0x76,0xCF,0x95,0xDC,0x48,0x4C,0xF2,0x43,
    0x8C,0xD9,0x44,0x26,0x7A,0x9E,0xEB,0x99,
    0xA3,0xD8,0xFB,0x30,0xA8,0x14,0x42,0x82,
    0x8D,0xB4,0x31,0xB3,0x1A,0xD5,0x2B,0xF6,
    0x32,0xBC,0x62,0xC0,0xFE,0x81,0x20,0x49,
    0xE7,0xF7,0x58,0x2F,0x2D,0xA6,0x1B,0x41,
    0x62,0xC7,0xE0,0x32,0x02,0x5D,0x82,0xEC,
    0xA3,0xE4,0x6C,0x9B,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
};

BYTE rgbyGreenHash[] =
{
    0x31,  0xE6,  0xF5,  0x27,  0x56,
    0xFA,  0x7A,  0x66,  0xA5,  0x13,
    0x82,  0x14,  0x12,  0x4F,  0xB2,
    0x12,  0x57,  0xC9,  0x24,  0xC5
};

extern TCHAR g_szAuthCheckMatch[100];

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



/*++

Routine Description:

    This routine verifies the authoring signature of the supplied DVD-X2 control
    data structure.

Arguments:

    ControlData - Specifies the control data structure to be verified.

Return Value:

    Status of operation.

--*/
NTSTATUS VerifyDVDX2AuthoringSignature(IN PDVDX2_CONTROL_DATA ControlData,
                                       TCHAR *szResult)
{
    UCHAR AuthoringDigest[XC_DIGEST_LEN];
    PUCHAR Workspace;
    NTSTATUS status;
    A_SHA_CTX SHAHash;
    UCHAR SHADigest[A_SHA_DIGEST_LEN];


    //
    // Calculate the digest for bytes 0 to 1226 of the control data structure
    // and verify that this matches the authoring hash stored in the control
    // data structure.
    //

    XCCalcDigest((PUCHAR)&ControlData->LayerDescriptor,
        FIELD_OFFSET(DVDX2_CONTROL_DATA, AuthoringHash) -
        FIELD_OFFSET(DVDX2_CONTROL_DATA, LayerDescriptor), AuthoringDigest);

    if (!RtlEqualMemory(AuthoringDigest, ControlData->AuthoringHash,
        XC_DIGEST_LEN)) {
        wsprintf(szResult, TEXT("Failed to verify control data"));
        return STATUS_TOO_MANY_SECRETS;
    }

    //
    // Allocate a workspace to do the digest verification.
    //

    ASSERT(XCCalcKeyLen(XePublicKeyData) == XCCalcKeyLen(RedPublicKeyData));
    Workspace = (PUCHAR)ExAllocatePoolWithTag(XCCalcKeyLen(RedPublicKeyData) * 2,
        'sWxI');

    if (Workspace == NULL) {
        wsprintf(szResult, TEXT("Failed to verify control data"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    
    // Determine which key the disc is signed with.

    // Is it signed with the red key?  Compare it against the known-red key.
    if (XCVerifyDigest(ControlData->AuthoringSignature, RedPublicKeyData,
                       Workspace, AuthoringDigest))
    {
        // Red key
        status = STATUS_SUCCESS;
        wsprintf(szResult, TEXT("[Red-signed DVD-X2]"));
        goto done;
    }


    // It's not the red key; before we check against the green key, first check
    // to see if the xbox itself is signed with the green key (if it's not,
    // then it's not a retail box, and couldn't read green discs anyways)

    // Compute the SHA digest of the public key to determine if it's the
    // green key
    XcSHAInit((PUCHAR)&SHAHash);
    XcSHAUpdate((PUCHAR)&SHAHash, (PUCHAR)XePublicKeyData,
        sizeof RedPublicKeyData);
    XcSHAFinal((PUCHAR)&SHAHash, SHADigest);

    if (memcmp(SHADigest, rgbyGreenHash, sizeof(rgbyGreenHash)))
    {
        // The rom was not signed with the green key; we don't know what it is
        goto unknownSig;
    }

    // The Xbox is indeed a retail xbox; compare the ROM key with the DVD to
    // see if the DVD is green-signed
    if (!XCVerifyDigest(ControlData->AuthoringSignature, XePublicKeyData,
                       Workspace, AuthoringDigest))
    {
        // Unknown disc signature
unknownSig:
        wsprintf(szResult, TEXT("Unknown disc signature"));
        status = STATUS_SUCCESS;
        goto done;
    }

    // If here, then the dvd is signed with the green (retail) key
    status = STATUS_SUCCESS;
    wsprintf(szResult, TEXT("[Green-signed DVD-X2]"));

done:

    ExFreePool(Workspace);

    return status;
}


/*++

Routine Description:

    This routine decrypts the host challenge response table of the supplied
    DVD-X2 control data structure.

Arguments:

    ControlData - Specifies the control data structure that contains the host
        challenge response table to be decrypted.

Return Value:

    None.

--*/
VOID DecryptHostChallengeResponseTable(IN PDVDX2_CONTROL_DATA ControlData)
{
    A_SHA_CTX SHAHash;
    UCHAR SHADigest[A_SHA_DIGEST_LEN];
    struct RC4_KEYSTRUCT RC4KeyStruct;

    //
    // Compute the SHA-1 hash of the data between bytes 1183 to 1226 of the
    // control data structure.
    //

    XcSHAInit((PUCHAR)&SHAHash);
    XcSHAUpdate((PUCHAR)&SHAHash, (PUCHAR)&ControlData->AuthoringTimeStamp,
        FIELD_OFFSET(DVDX2_CONTROL_DATA, AuthoringHash) -
        FIELD_OFFSET(DVDX2_CONTROL_DATA, AuthoringTimeStamp));
    XcSHAFinal((PUCHAR)&SHAHash, SHADigest);

    //
    // Compute a 56-bit RC4 session key from the SHA-1 hash.
    //

    XcRC4Key((PUCHAR)&RC4KeyStruct, 56 / 8, SHADigest);

    //
    // Decrypt the host challenge response table in place using the RC4 session
    // key.
    //

    XcRC4Crypt((PUCHAR)&RC4KeyStruct,
        sizeof(ControlData->HostChallengeResponseTable.Entries),
        (PUCHAR)&ControlData->HostChallengeResponseTable.Entries);
}

/*++

Routine Description:

    This routine issues the supplied challenge to the drive and checks the
    response from the drive.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    ControlData - Specifies the control data structure.

    HostChallengeResponseEntry - Specifies the challenge to issue.

    FirstChallenge - Specifies TRUE if this is the first challenge being made to
        the drive.

    FinalChallenge - Specifies TRUE if this is the final challenge being made to
        the drive.

Return Value:

    Status of operation.

--*/
NTSTATUS AuthenticationChallenge(
    IN PDEVICE_OBJECT DeviceObject,
    IN PDVDX2_CONTROL_DATA ControlData,
    IN PDVDX2_HOST_CHALLENGE_RESPONSE_ENTRY HostChallengeResponseEntry,
    IN BOOLEAN FirstChallenge,
    IN BOOLEAN FinalChallenge,
    TCHAR *szResult
    )
{
    NTSTATUS status;
    SCSI_PASS_THROUGH_DIRECT PassThrough;
    PCDB Cdb = (PCDB)&PassThrough.Cdb;
    DVDX2_AUTHENTICATION Authentication;

    ASSERT(HostChallengeResponseEntry->ChallengeLevel == 1);

    //
    // Prepare the SCSI pass through structure.
    //

    RtlZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));

    PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);

    //
    // Prepare the authentication page.
    //

    RtlZeroMemory(&Authentication, sizeof(DVDX2_AUTHENTICATION));

    *((PUSHORT)&Authentication.Header.ModeDataLength) =
        IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION) -
        FIELD_OFFSET(DVDX2_AUTHENTICATION, Header.MediumType));
    Authentication.AuthenticationPage.PageCode = DVDX2_MODE_PAGE_AUTHENTICATION;
    Authentication.AuthenticationPage.PageLength = sizeof(DVDX2_AUTHENTICATION_PAGE) -
        FIELD_OFFSET(DVDX2_AUTHENTICATION_PAGE, PartitionArea);
    Authentication.AuthenticationPage.CDFValid = 1;
    Authentication.AuthenticationPage.DiscCategoryAndVersion =
        (ControlData->LayerDescriptor.BookType << 4) +
        ControlData->LayerDescriptor.BookVersion;
    Authentication.AuthenticationPage.DrivePhaseLevel = 1;
    Authentication.AuthenticationPage.ChallengeID = HostChallengeResponseEntry->ChallengeID;
    Authentication.AuthenticationPage.ChallengeValue = HostChallengeResponseEntry->ChallengeValue;

    if (!FirstChallenge) {
        Authentication.AuthenticationPage.Authentication = 1;
    }

    if (FinalChallenge) {
        Authentication.AuthenticationPage.PartitionArea = 1;
    }

    //
    // Issue the challenge to the DVD-X2 drive.
    //

    PassThrough.DataIn = SCSI_IOCTL_DATA_OUT;
    PassThrough.DataBuffer = &Authentication;
    PassThrough.DataTransferLength = sizeof(DVDX2_AUTHENTICATION);

    RtlZeroMemory(Cdb, sizeof(CDB));

    Cdb->MODE_SENSE10.OperationCode = SCSIOP_MODE_SELECT10;
    *((PUSHORT)&Cdb->MODE_SENSE10.AllocationLength) =
        (USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION));

    status = IoSynchronousDeviceIoControlRequest(IOCTL_SCSI_PASS_THROUGH_DIRECT,
        DeviceObject, &PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT), NULL, 0,
        NULL, FALSE);

    if (!NT_SUCCESS(status)) {
        wsprintf(szResult, TEXT("Challenge operation failed ") \
                           TEXT("(status=%08x)"), status);
        return status;
    }

    //
    // Read the response from the DVD-X2 drive.
    //

    PassThrough.DataIn = SCSI_IOCTL_DATA_IN;

    Cdb->MODE_SENSE10.OperationCode = SCSIOP_MODE_SENSE10;
    Cdb->MODE_SENSE10.PageCode = DVDX2_MODE_PAGE_AUTHENTICATION;
    *((PUSHORT)&Cdb->MODE_SENSE10.AllocationLength) =
        (USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION));

    status = IoSynchronousDeviceIoControlRequest(IOCTL_SCSI_PASS_THROUGH_DIRECT,
        DeviceObject, &PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT), NULL, 0,
        NULL, FALSE);

    if (!NT_SUCCESS(status)) {
        wsprintf(szResult, TEXT("Response operation failed (status=%08x)"),
                           status);
        return status;
    }

    //
    // Check that the drive's response matches the expected response.
    //

    if (!Authentication.AuthenticationPage.Authentication ||
        (Authentication.AuthenticationPage.ResponseValue !=
        HostChallengeResponseEntry->ResponseValue)) {
        wsprintf(szResult, TEXT("Invalid challenge response from drive."));
        return STATUS_TOO_MANY_SECRETS;
    }

    return STATUS_SUCCESS;
}
bool AuthenticateDisc(DVDX2_AUTHENTICATION Authentication,
                      DVDX2_CONTROL_DATA ControlData,
                      PDEVICE_OBJECT DeviceObject,
                      TCHAR *szResult)
{
    NTSTATUS Status;
    LONG StartingIndex;
    LONG Index;
    LONG EndingIndex;
    BOOLEAN FirstChallenge;
    PDVDX2_HOST_CHALLENGE_RESPONSE_ENTRY HostChallengeResponseEntry;

    // Verify that the disc category and version from the authentication
    // page matches the control data structure.
    if (Authentication.AuthenticationPage.DiscCategoryAndVersion !=
        (ControlData.LayerDescriptor.BookType << 4) +
         ControlData.LayerDescriptor.BookVersion)
    {
        wsprintf(szResult, TEXT("Disc category and version mismatch.\n"));
        return false;
    }

    // Verify that the encrypted digest stored in the control data structure
    // matches the digest of the structure.

    Status = VerifyDVDX2AuthoringSignature(&ControlData, szResult);
    if (!NT_SUCCESS(Status)) {
        //szResult already filled in by VerifyDVDX2AuthoringSignature
        return false;
    }

    // Decrypt the contents of the host challenge response table.
    DecryptHostChallengeResponseTable(&ControlData);

    // Validate the the host challenge response table is the expected version
    // and that there are the expected number of entries in the table.
    if ((ControlData.HostChallengeResponseTable.Version != 1) ||
        (ControlData.HostChallengeResponseTable.NumberOfEntries == 0) ||
        (ControlData.HostChallengeResponseTable.NumberOfEntries >
            DVDX2_HOST_CHALLENGE_RESPONSE_ENTRY_COUNT)) {
        wsprintf(szResult, TEXT("Invalid host challenge response table"));
        return false;
    }

    // Walk through the host challenge response starting at a random starting
    // index and issue challenge response values.
    StartingIndex = KeQueryPerformanceCounter().LowPart %
                    ControlData.HostChallengeResponseTable.NumberOfEntries;
    FirstChallenge = TRUE;

    DWORD dwAuthAttempted = 0, dwAuthSucceeded = 0;
    for(Index = 0; Index < ControlData.HostChallengeResponseTable.NumberOfEntries; Index++) {
        HostChallengeResponseEntry = &ControlData.HostChallengeResponseTable.Entries[Index];

        TCHAR sz[100];
        wsprintf(sz, TEXT("Challenge on index %d -"), Index);
        OutputDebugString(sz);

        // Check if this is a supported challenge level.  If so, issue the
        // authentication challenge to the DVD-X2 drive.
        if (HostChallengeResponseEntry->ChallengeLevel == 1) {
            dwAuthAttempted |= 1 << Index;

            Status = AuthenticationChallenge(DeviceObject, &ControlData,
                            HostChallengeResponseEntry, FirstChallenge, FALSE,
                            szResult);

            if (!NT_SUCCESS(Status))
            {
                wsprintf(sz, TEXT("Failed authentication on index %d!\r\n"), Index);
                OutputDebugString(sz);
            } else
                dwAuthSucceeded |= 1 << Index;
            
            wsprintf(sz, TEXT("passed.\r\n"), Index);
            OutputDebugString(sz);

            EndingIndex = Index;
            FirstChallenge = FALSE;
        }
        else {
            wsprintf(sz, TEXT("skipped.\r\n"), Index);
            OutputDebugString(sz);
        }
    }
    
    // Generate the checkmatch string
    wsprintf(g_szAuthCheckMatch, TEXT("Check: 0x%06X Match: 0x%06X"),
            dwAuthAttempted, dwAuthSucceeded);
    
    // If we found a challenge response table with no entries that we can
    // process, then fail authentication.  We need at least one entry in order
    // to issue the final request to switch to the Xbox partition.
    if (EndingIndex == -1) {
        wsprintf(szResult, TEXT("No usable challenge response entries."));
        return false;
    }

    // Issue the last challenge response entry again, but this time switch to
    // the Xbox partition.
    HostChallengeResponseEntry = &ControlData.HostChallengeResponseTable.Entries[EndingIndex];

    Status = AuthenticationChallenge(DeviceObject, &ControlData,
                                     HostChallengeResponseEntry, FALSE, TRUE,
                                     szResult);

    if (!NT_SUCCESS(Status))
        return false;

    return true;
}

