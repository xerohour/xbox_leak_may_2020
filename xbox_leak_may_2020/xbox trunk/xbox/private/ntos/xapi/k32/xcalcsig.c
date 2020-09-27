/*++

Copyright (c) Microsoft Corporation

Description:
    Implementation of routines to calculate the signature
    given a series of piecemeal data blobs

Module Name:

    xcalcsig.c

--*/

#include "basedll.h"
#pragma hdrstop

#include <sha.h>
#include <shahmac.h>

//
// Define the actual context structure
//
typedef struct
{
    DWORD                dwFlags;    // Flags
    PBYTE                pbTitleKey;    // Per-title key 
    XSHAHMAC_CONTEXT    shactx;        // SHA1 context

} XCALCSIG_CONTEXT, *PXCALCSIG_CONTEXT;

//
// Define some macros to abstract memory management
//
#define XCalcSigAlloc(n)        LocalAlloc(LMEM_FIXED, (n))
#define XCalcSigFree(p)            LocalFree(p)

//
// Define some debug facilities to track invalid contexts
//
#if DBG
#define XCALCSIG_FLAG_INVALID_CONTEXT    (0x80000000)
#endif

//
// Define the K padding size for SHA1-Hmac
//
#define HMAC_K_PADSIZE              64

//
// Define the well-known XOR factors for Kipad and Kopad
//
#define HMAC_KI_XOR_FACTOR            ((DWORD)0x36363636)
#define HMAC_KO_XOR_FACTOR            ((DWORD)0x5C5C5C5C)

//
// Function to initialize a piecemeal SHA1-Hmac evaluation
//
VOID WINAPI XShaHmacInitialize(
            IN PBYTE                pbKey,
            IN DWORD                cbKey,
            IN OUT XSHAHMAC_CONTEXT Shactx
            )
{
    HRESULT        hr = S_OK;
    BYTE        rgbKipad[HMAC_K_PADSIZE];
    ULONG        dwBlock;

    RIP_ON_NOT_TRUE("XShaHmacInitialize", (pbKey != NULL));
    RIP_ON_NOT_TRUE("XShaHmacInitialize", (cbKey >= sizeof(DWORD)));
    RIP_ON_NOT_TRUE("XShaHmacInitialize", (Shactx != NULL));
    
    // Shorten length if longer than our K padding
    if (cbKey > HMAC_K_PADSIZE)
        cbKey = HMAC_K_PADSIZE;

    // Build our Kipad
    memset(rgbKipad, 0, HMAC_K_PADSIZE);
    memcpy(rgbKipad, pbKey, cbKey);
    for (dwBlock = 0; 
         dwBlock < (HMAC_K_PADSIZE/sizeof(DWORD)); 
         dwBlock++)
    {
        ((DWORD*)rgbKipad)[dwBlock] ^= HMAC_KI_XOR_FACTOR;
    }

    // Initialize our SHA1 Hmac context
    XcSHAInit(Shactx);

    // Run our Kipad through this ...
    XcSHAUpdate(Shactx, rgbKipad, HMAC_K_PADSIZE);
}

//
// Function to update the Hmac
//
VOID WINAPI XShaHmacUpdate(
            IN XSHAHMAC_CONTEXT Shactx,
            IN PBYTE            pbData,
            IN DWORD            cbData
            )
{
    RIP_ON_NOT_TRUE("XShaHmacUpdate", (Shactx != NULL));
    RIP_ON_NOT_TRUE("XShaHmacUpdate", (pbData != NULL));
    RIP_ON_NOT_TRUE("XShaHmacUpdate", (cbData > 0));

    XcSHAUpdate(Shactx, pbData, cbData);
}
            
//
// Function to calculate the final Hmac
//
VOID WINAPI XShaHmacComputeFinal(
            IN XSHAHMAC_CONTEXT Shactx,
            IN PBYTE            pbKey,
            IN DWORD            cbKey,
            OUT PBYTE           pbHmac
            )
{
    HRESULT        hr = S_OK;
    BYTE        rgbKopad[HMAC_K_PADSIZE + A_SHA_DIGEST_LEN];
    ULONG        dwBlock;

    RIP_ON_NOT_TRUE("XShaHmacComputeFinal", (Shactx != NULL));
    RIP_ON_NOT_TRUE("XShaHmacComputeFinal", (pbHmac != NULL));
    
    // Shorten length if longer than our K padding
    if (cbKey > HMAC_K_PADSIZE)
        cbKey = HMAC_K_PADSIZE;

    // Build our Kipad
    memset(rgbKopad, 0, HMAC_K_PADSIZE);
    memcpy(rgbKopad, pbKey, cbKey);
    for (dwBlock = 0; 
         dwBlock < (HMAC_K_PADSIZE/sizeof(DWORD)); 
         dwBlock++)
    {
        ((DWORD*)rgbKopad)[dwBlock] ^= HMAC_KO_XOR_FACTOR;
    }

    // Finish off the hash, and place the result right
    // after the Kopad data
    XcSHAFinal(Shactx, rgbKopad + HMAC_K_PADSIZE);

    // Do another hash, now with the Kopad data
    XcSHAInit(Shactx);
    XcSHAUpdate(Shactx, rgbKopad, sizeof(rgbKopad));
    XcSHAFinal(Shactx, pbHmac);
}            

//
// Function to make a signature non-roamable. The two
// signature buffers must be different and must not overlap
//
BOOL XapiConvertSignatureToNonRoamable(
            PXCALCSIG_CONTEXT    pCalcSig,
            PXCALCSIG_SIGNATURE  psignatureRoamable,
            PXCALCSIG_SIGNATURE  psignatureNonRoamable
            )
{
    // Compute the Keyed SHA Hmac
    XShaHmacInitialize((LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH, 
                pCalcSig->shactx);
    
    XShaHmacUpdate(pCalcSig->shactx, 
                psignatureRoamable->Signature, 
                XCALCSIG_SIGNATURE_SIZE);
                
    XShaHmacComputeFinal(pCalcSig->shactx, 
                (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH,
                psignatureNonRoamable->Signature);

    return(TRUE);
}

//
// Wrapper to XCalculateSignatureBeginEx() that does not take a
// title ID parameter (uses the current title's ID implicitly)
//
HANDLE WINAPI XCalculateSignatureBegin(
            IN DWORD dwFlags
            )
{
    return XCalculateSignatureBeginEx(dwFlags, XeImageHeader()->Certificate->TitleID);
}


//
// Function to begin the piecemeal process to calculate the
// signature of a blob of data
//
// Arguments:
//    dwFlags - Optional flags. Currently defined flags are:
//        XCALCSIG_FLAG_NON_ROAMABLE - this forces the resulting
//                signature to be unusable on any XBox other than
//                the one on which the signature is computed.
//    dwAltTitleId - Alternate title id or the current title id
//
// Return Values:
//    On success, this function returns a HANDLE that can be used
//    in subsequent calls to calculate a signature. On failure,
//    function returns INVALID_HANDLE_VALUE, extended error
//    information can be retrieved using GetLastError().
//
// Remarks:
//    On success, this fucntion allocates memory for the
//    returned HANDLE. The caller must explicitly call
//    XCalculateSignatureEnd to free the handle resources,
//    regardless of any intermediate failures.
//
HANDLE WINAPI XCalculateSignatureBeginEx(
            IN DWORD dwFlags,
            IN DWORD dwAltTitleId
            )
{
    PXCALCSIG_CONTEXT pCalcSig;
    PXBEIMAGE_CERTIFICATE Certificate = XeImageHeader()->Certificate;

    RIP_ON_NOT_TRUE("XCalculateSignatureBegin",
                    ((dwFlags & (~XCALCSIG_FLAG_NON_ROAMABLE)) == 0));
    
    // Allocate the context
    pCalcSig = XCalcSigAlloc(sizeof(XCALCSIG_CONTEXT));
    if (!pCalcSig)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Error;
    }

    // Initialize 
    pCalcSig->dwFlags = dwFlags;

    //
    // Get the per-title key
    // If dwAltTitleId is the current title's id, use XboxSignatureKey
    // Otherwise, try to user XboxAlternateSignatureKeys
    //
    
    if (Certificate->TitleID != dwAltTitleId)
    {
        int i;
        for (i = 0; i < ARRAYSIZE(Certificate->AlternateTitleIDs); i++)
        {
            if (0 == Certificate->AlternateTitleIDs[i])
            {
                SetLastError(ERROR_ACCESS_DENIED);
                goto Error;
            }

            if (dwAltTitleId == Certificate->AlternateTitleIDs[i])
            {
                break;
            }
        }

        if (i >= sizeof(Certificate->AlternateTitleIDs))
        {
            SetLastError(ERROR_ACCESS_DENIED);
            goto Error;
        }

        ASSERT(i < XBEIMAGE_ALTERNATE_TITLE_ID_COUNT);
        
        pCalcSig->pbTitleKey = (PBYTE)XboxAlternateSignatureKeys[i];
        
    }
    else
    {
        pCalcSig->pbTitleKey = (PBYTE)XboxSignatureKey;
    }

    // Initialize SHA Hmac
    XShaHmacInitialize(pCalcSig->pbTitleKey, 
                       XC_SYMMETRIC_KEY_SIZE,
                       pCalcSig->shactx);

    // Return context as opaque handle
    return((HANDLE)pCalcSig);

Error:
    return(INVALID_HANDLE_VALUE);
}


//
// This function continues to to update the calculation
// for the specified signature context handle. 
//
// Arguments:
//    hCalcSig - the context to update
//    pbData - the next chunk of data to update with
//    cbData - the size of the data chunk
//
// Return Values:
//    This function returns ERROR_SUCCESS on success, and if this
//    function failed, an appropriate Win32 error code is returned.
//    Either case, XCalculateSignatureEnd must be called to free 
//    any resources associated with the HANDLE.
//
DWORD WINAPI XCalculateSignatureUpdate(
            IN HANDLE hCalcSig,
            IN const BYTE *pbData,
            IN ULONG cbData
            )
{
    PXCALCSIG_CONTEXT pCalcSig = (PXCALCSIG_CONTEXT)hCalcSig;

    RIP_ON_NOT_TRUE("XCalculateSignatureUpdate", (pCalcSig != NULL));
    RIP_ON_NOT_TRUE("XCalculateSignatureUpdate", (pbData != NULL));

#if DBG
    RIP_ON_NOT_TRUE("XCalculateSignatureUpdate", 
                    ((pCalcSig->dwFlags & XCALCSIG_FLAG_INVALID_CONTEXT) == 0));
#endif

    // Call update
    XShaHmacUpdate(pCalcSig->shactx, (PBYTE)pbData, cbData);
    return(ERROR_SUCCESS);
}

//
// This function performs the last bit of precessing
// required to calculate the signature, returns the 
// signature blob, and releases the context.
//
// Arguments:
//    hCalcSig - the context to close
//    Signature - the buffer to receive the final signature
//
// Return Values:
//    On success, this function returns ERROR_SUCCESS, and the
//    final computed signature is returned in signature. If this 
//    function failed, an appropriate Win32 error code is returned.
//
// Remarks:
//    The context specified by hCalcSig will be released 
//    regardless of whether this function call succeeded
//    or not. hCalcSig should not be used after this function
//    returns unless it is reinitialized by XCalculateSignatureBegin.
//    This function can be used to just free the context. In
//    this case, NULL should be specified for the Signature argument.
//
DWORD WINAPI XCalculateSignatureEnd(
            IN HANDLE hCalcSig,
            OUT PXCALCSIG_SIGNATURE pSignature
            )
{
    BOOL                fResult = TRUE;
    PXCALCSIG_CONTEXT   pCalcSig = (PXCALCSIG_CONTEXT)hCalcSig;
    XCALCSIG_SIGNATURE  xcsSignature;
    BOOL                fNonRoamable;

    RIP_ON_NOT_TRUE("XCalculateSignatureEnd", (pCalcSig != NULL));

#if DBG
    RIP_ON_NOT_TRUE("XCalculateSignatureEnd", 
            ((pCalcSig->dwFlags & XCALCSIG_FLAG_INVALID_CONTEXT) == 0));
#endif

    fNonRoamable = ((pCalcSig->dwFlags & XCALCSIG_FLAG_NON_ROAMABLE) != 0);

    // Finish the calculation
    if (pSignature)
    {
        // Figure out the Hmac
        XShaHmacComputeFinal(pCalcSig->shactx, 
                    pCalcSig->pbTitleKey, 
                    XC_SYMMETRIC_KEY_SIZE,
                    (fNonRoamable)?xcsSignature.Signature:pSignature->Signature);
                        
        if (fNonRoamable)
        {
            fResult = XapiConvertSignatureToNonRoamable(
                        pCalcSig, &xcsSignature, pSignature);
        }
    }

    // Well, free the context either case
#if DBG
    pCalcSig->dwFlags |= XCALCSIG_FLAG_INVALID_CONTEXT;
#endif
    XCalcSigFree(pCalcSig);
    
    return(fResult?ERROR_SUCCESS:GetLastError());
}


