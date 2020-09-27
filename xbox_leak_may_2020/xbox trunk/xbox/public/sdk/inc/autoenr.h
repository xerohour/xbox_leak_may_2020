/////////////////////////////////////////////////////////////////////////////
//  FILE          : autoenrl.h                                             //
//  DESCRIPTION   : Auto Enrollment functions                              //
//  AUTHOR        :                                                        //
//  HISTORY       :                                                        //
//                                                                         //
//  Copyright (C) 1993-1999 Microsoft Corporation   All Rights Reserved    //
/////////////////////////////////////////////////////////////////////////////

#ifndef __AUTOENR_H__
#define __AUTOENR_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _AUTO_ENROLL_INFO_
{
    LPSTR               pszAutoEnrollProvider;
    LPWSTR              pwszCertType;
    LPCWSTR             pwszAutoEnrollmentID;
    HCERTSTORE          hMYStore;
    BOOL                fRenewal;
    PCCERT_CONTEXT      pOldCert;
    DWORD               dwProvType;
    DWORD               dwKeySpec;
    DWORD               dwGenKeyFlags;
    CERT_EXTENSIONS     CertExtensions;
    LPWSTR              pwszCAMachine;
    LPWSTR              pwszCAAuthority;
} AUTO_ENROLL_INFO, *PAUTO_ENROLL_INFO;

DWORD
AutoEnrollWrapper(
	PVOID CallbackState
	);


BOOL ProvAutoEnrollment(
                        IN BOOL fMachineEnrollment,
                        IN PAUTO_ENROLL_INFO pInfo
                        );

typedef struct _CA_HASH_ENTRY_
{
    DWORD   cbHash;
    BYTE    rgbHash[32];
} CA_HASH_ENTRY, *PCA_HASH_ENTRY;

#define MACHINE_AUTOENROLLMENT_TRIGGER_EVENT TEXT("AUTOENRL:TriggerMachineEnrollment")


#define USER_AUTOENROLLMENT_TRIGGER_EVENT TEXT("AUTOENRL:TriggerUserEnrollment")


#ifdef __cplusplus
}       // Balance extern "C" above
#endif

#endif // __AUTOENR_H__