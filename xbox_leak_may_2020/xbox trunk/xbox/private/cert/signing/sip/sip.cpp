/*
 *
 * sip.cpp
 *
 * Xbox code signing package
 *
 */

#include "precomp.h"
#include <malloc.h>

#define TESTKEY

CNCipherCrypt *pccSign;
CDevkitCrypt *pccRed;
//HINSTANCE g_hinstCrypt;

// {5581CF39-A767-4a14-B9D7-FA6121327A5E}
static GUID guidXSIP = 
{ 0x5581cf39, 0xa767, 0x4a14, { 0xb9, 0xd7, 0xfa, 0x61, 0x21, 0x32, 0x7a, 0x5e } };

XBEIMAGE_CERTIFICATE certDefault = {
    // size
    sizeof(XBEIMAGE_CERTIFICATE),
    // timestamp
    0,
    // title ID
    0xFFFD0001,
    // title name
    { 0 },
    // alternate title IDs
    { 0 },
    // allowed media
    XBEIMAGE_MEDIA_TYPE_HARD_DISK,
    // region
    XBEIMAGE_GAME_REGION_MANUFACTURING,
    // ratings
    0xFFFFFFFF,
    // the rest is zero
};

BOOL FReplaceCertificate(CXBEFile *pxbe, CCryptContainer *pcc)
{
    const XBEIMAGE_CERTIFICATE *pxcert;

    pxcert = pxbe->GetCert();
    if(!pxcert)
        return NULL;

    certDefault.TimeDateStamp = time(NULL);
    wcsncpy(certDefault.TitleName, pxcert->TitleName,
        XBEIMAGE_TITLE_NAME_LENGTH);
    return CXBEFile::FGenNewKeys(&certDefault, pcc) &&
        pxbe->FInjectNewCertificate(&certDefault);
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID lpv)
{
    switch(dwReason) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinst);
        pccRed = new CDevkitCrypt;
        break;
    case DLL_PROCESS_DETACH:
        if(pccSign) {
            delete pccSign;
            pccSign = NULL;
        }
        delete pccRed;
#if 0
        if(g_hinstCrypt) {
            FreeLibrary(g_hinstCrypt);
            g_hinstCrypt = NULL;
        }
#endif
        break;
    }

    return TRUE;
}

STDAPI DllRegisterServer(void)
{
    SIP_ADD_NEWPROVIDER sProv;
    HRESULT hr = S_OK;

    // must first init struct to 0
    memset(&sProv, 0x00, sizeof(SIP_ADD_NEWPROVIDER));

    // add registration info
    sProv.cbStruct               = sizeof(SIP_ADD_NEWPROVIDER);
    sProv.pwszDLLFileName        = L"xboxsip.dll";
    sProv.pgSubject              = &guidXSIP;
    sProv.pwszGetFuncName        = L"XSIPGetSignedDataMsg";
    sProv.pwszPutFuncName        = L"XSIPPutSignedDataMsg";
    sProv.pwszCreateFuncName     = L"XSIPCreateIndirectData";
    sProv.pwszVerifyFuncName     = L"XSIPVerifyIndirectData";
    sProv.pwszRemoveFuncName     = L"XSIPRemoveSignedDataMsg";
    sProv.pwszIsFunctionNameFmt2 = L"XSIPIsMyTypeOfFile";
    
    // register SIP provider with crypto
    return CryptSIPAddProvider(&sProv);
}

STDAPI DllUnregisterServer(void)
{
    return CryptSIPRemoveProvider(&guidXSIP);
}

BOOL FEnsureCryptContainer(HCRYPTPROV hprov)
{
#ifndef TESTKEY
    if(hprov && pccSign && pccSign->HProvider() != hprov) {
        delete pccSign;
        pccSign = NULL;
    }
#endif
    if(!pccSign) {
#ifndef TESTKEY
        if(!hprov) {
            SetLastError(NTE_BAD_PROVIDER);
            return FALSE;
        }
        pccSign = CNCipherCrypt::PccCreate(hprov);
#else
        pccSign = (CNCipherCrypt *)new CFunctestCrypt;
#endif
    }
    return pccSign != NULL;
}

BOOL WINAPI XSIPIsMyTypeOfFile(
    IN WCHAR *pwszFileName,
    OUT GUID *pgSubject
    )
{
    CDiskFileW *pstm;
    CXBEFile *pxbe = NULL;
    BOOL fRet = FALSE;

    /* We claim this is our type of file if we can verify the XBE signature */
    pstm = new CDiskFileW(pwszFileName, GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE);
    if(!pstm->FIsOpen())
        goto done;
    pxbe = new CXBEFile(pstm);
    pxbe->UseCryptContainer(pccRed);
    if(!pxbe->FVerifySignature()) {
        SetLastError(ERROR_BAD_FORMAT);
        goto done;
    }

    fRet = TRUE;

done:
    if(pxbe)
        delete pxbe;
    delete pstm;
    return fRet;
}

BOOL WINAPI XSIPGetSignedDataMsg(
    IN SIP_SUBJECTINFO *pSubjectInfo,
    OUT DWORD *pdwEncodingType,
    IN DWORD dwIndex,
    IN OUT DWORD *pdwDataLen,
    OUT BYTE *pbData
    )
{
    OutputDebugString("Get");
    return FALSE;
}

BOOL WINAPI XSIPPutSignedDataMsg(
    IN SIP_SUBJECTINFO *pSubjectInfo,
    IN DWORD dwEncodingType,
    OUT DWORD *pdwIndex,
    IN DWORD cbSignedDataMsg,
    IN BYTE *pbSignedDataMsg
    )
{
    OutputDebugString("Put");
    return FALSE;
}

BOOL WINAPI XSIPCreateIndirectData(
    IN SIP_SUBJECTINFO *pSubjectInfo,
    IN OUT DWORD *pcbIndirectData,
    OUT SIP_INDIRECT_DATA *pIndirectData
    )
{
    CFileStmRand *pstm = NULL;
    CXBEFile *pxbe = NULL;
    BOOL fRet = FALSE;
    PXBEIMAGE_HEADER pxihNew;
    PXBEIMAGE_CERTIFICATE pxcertNew;

    /* We do all of the actual signing in here.  The indirect data contains
     * the complete XBE base header, so we can contain both the signature and
     * the confounded fields, plus the XBE certificate */
    if(!pIndirectData) {
        /* Just want the byte count */
        *pcbIndirectData = sizeof(XBEIMAGE_HEADER) +
            sizeof(XBEIMAGE_CERTIFICATE);
        return TRUE;
    }
    if(!FEnsureCryptContainer(pSubjectInfo->hProv))
        goto done;
    if(pSubjectInfo->hFile && pSubjectInfo->hFile != INVALID_HANDLE_VALUE)
        pstm = new CHandleFile(pSubjectInfo->hFile);
    else
        pstm = new CDiskFileW(pSubjectInfo->pwsFileName);
    if(!pstm->FIsOpen())
        goto done;
    pxbe = new CXBEFile(pstm);
    pxbe->UseCryptContainer(pccRed);
    if(!pxbe->FVerifySignature()) {
        SetLastError(ERROR_BAD_FORMAT);
        goto done;
    }
    if(!FReplaceCertificate(pxbe, pccSign)) {
        SetLastError(ERROR_BAD_FORMAT);
        goto done;
    }
    pxbe->UseCryptContainer(pccSign);
    if(!pxbe->FSignFile()) {
        SetLastError(CRYPT_E_MSG_ERROR);
        goto done;
    }
    pxihNew = (PXBEIMAGE_HEADER)pIndirectData;
    memcpy(pxihNew, pxbe->GetHeader(), sizeof *pxihNew);

    pxcertNew = (PXBEIMAGE_CERTIFICATE)(pxihNew + 1);
    memcpy(pxcertNew, pxbe->GetCert(), sizeof *pxcertNew);
    
    fRet = TRUE;

done:
    if(pxbe)
        delete pxbe;
    if(pstm)
        delete pstm;
    return fRet;
}

BOOL WINAPI XSIPVerifyIndirectData(
    IN SIP_SUBJECTINFO *pSubjectInfo,
    IN SIP_INDIRECT_DATA *pIndirectData
    )
{
    OutputDebugString("Verify");
    return FALSE;
}

BOOL WINAPI XSIPRemoveSignedDataMsg(
    IN SIP_SUBJECTINFO *pSubjectInfo,
    IN DWORD dwIndex
    )
{
    OutputDebugString("Remove");
    return FALSE;
}
