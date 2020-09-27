/*-----------------------------------------------------------------------------
Microsoft VSEE

Microsoft Confidential
Copyright 1995-2000 Microsoft Corporation. All Rights Reserved.

@doc external
@module VapiEmpGuids.h - GUIDs for the Versioning Enlistment Manager Proxy
Put all guids here, and only put them in here once; there should be no
guids in .idl, .rgs, .ctc, .cpp, .c, or .h files, except those generated
by Midl, indirectly from this file.

@owner JayK
-----------------------------------------------------------------------------*/
#if !defined(VS_COMMON_INC_VSEE_VAPI_EMP_GUIDS_H_INCLUDED_) // {
#define VS_COMMON_INC_VSEE_VAPI_EMP_GUIDS_H_INCLUDED_
/*#pragma once ends up in .rgi, which is bad, so do not do it*/
#include "VseeGuidDataToXxx.h"

/*-----------------------------------------------------------------------------
Guid "data" and guids that have only one needed form here
This form of guids is approximately available from uuidgen -s, then remove the braces and "0x"s.
-----------------------------------------------------------------------------*/
#define CLSID_CVapiEMPUrl_data                               (03696223, 1DD6, 11d3, 85,CF,00,A0,C9,CF,CC,16)
#define CLSID_CVapiEMPSession_data                           (03696224, 1DD6, 11d3, 85,CF,00,A0,C9,CF,CC,16)
#define LIBID_NVseeVersioningEnlistmentManagerProxy_data     (03696225, 1DD6, 11d3, 85,CF,00,A0,C9,CF,CC,16)
#define CLSID_CVapiEMPDataSource_data                        (03696226, 1DD6, 11d3, 85,CF,00,A0,C9,CF,CC,16)
#define CLSID_CVapiEMPWecFactory_data                        (03696227, 1DD6, 11d3, 85,CF,00,A0,C9,CF,CC,16)

/*-----------------------------------------------------------------------------
then various transforms of the guid "data", without ever restating the "data",
follow down below..
-----------------------------------------------------------------------------*/

// CUrl and CSession are not CoCreatable, and the rest are; that's
// why they are the worst offenders in terms of name/type choice
// FUTURE We should probably eliminate completely the generic CLSID_Foo identifiers
// and go with only Hungarian ones.
//
// also realize that instead of CLSID_CVapiEMPSession, you should use
// __uuidof(CVapiEMPSession) or similar if the __declspec(uuid()) is visible to you.

#define szCLSID_CVapiEMPUrl                                    VSEE_GUID_DATA_TO_DASHED_STRING     CLSID_CVapiEMPUrl_data
#define szCLSID_CVapiEMPSession                                VSEE_GUID_DATA_TO_DASHED_STRING     CLSID_CVapiEMPSession_data
                                                                        
// @deprecated FUTURE..                                                    
#define CLSID_CVapiEMPUrl        szCLSID_CVapiEMPUrl                        
#define CLSID_CVapiEMPSession    szCLSID_CVapiEMPSession                    
                                                                        
#define      LIBID_NVseeVersioningEnlistmentManagerProxy       VSEE_GUID_DATA_TO_DASHED             LIBID_NVseeVersioningEnlistmentManagerProxy_data
#define CHAR_LIBID_NVseeVersioningEnlistmentManagerProxy       VSEE_GUID_DATA_TO_DASHED_CHAR        LIBID_NVseeVersioningEnlistmentManagerProxy_data
                                                                        
#define      CLSID_CVapiEMPDataSource                          VSEE_GUID_DATA_TO_DASHED             CLSID_CVapiEMPDataSource_data
#define    szCLSID_CVapiEMPDataSource                          VSEE_GUID_DATA_TO_DASHED_STRING      CLSID_CVapiEMPDataSource_data
#define CHAR_CLSID_CVapiEMPDataSource                          VSEE_GUID_DATA_TO_BRACED_DASHED_CHAR CLSID_CVapiEMPDataSource_data
#define   rgsCLSID_CVapiEMPDataSource                          VSEE_GUID_DATA_TO_BRACED_DASHED      CLSID_CVapiEMPDataSource_data
#define   ctcCLSID_CVapiEMPDataSource /*currently unused*/     VSEE_GUID_DATA_TO_STRUCT_INITIALIZER CLSID_CVapiEMPDataSource_data

#define      CLSID_CVapiEMPWecFactory                          VSEE_GUID_DATA_TO_DASHED             CLSID_CVapiEMPWecFactory_data
#define    szCLSID_CVapiEMPWecFactory                          VSEE_GUID_DATA_TO_DASHED_STRING      CLSID_CVapiEMPWecFactory_data
#define CHAR_CLSID_CVapiEMPWecFactory                          VSEE_GUID_DATA_TO_BRACED_DASHED_CHAR CLSID_CVapiEMPWecFactory_data
#define   rgsCLSID_CVapiEMPWecFactory                          VSEE_GUID_DATA_TO_BRACED_DASHED      CLSID_CVapiEMPWecFactory_data
#define      ctcCLSID_CVapiEMPWecFactory /*currently unused*/  VSEE_GUID_DATA_TO_STRUCT_INITIALIZER CLSID_CVapiEMPWecFactory_data

#if defined(RGS_INVOKED) // {

// For compiling .rgs files into .rgi files.
// FUTURE make this cleaner..

#undef CLSID_CVapiEMPDataSource
#undef CLSID_CVapiEMPWecFactory

#define CLSID_CVapiEMPDataSource rgsCLSID_CVapiEMPDataSource
#define CLSID_CVapiEMPWecFactory rgsCLSID_CVapiEMPWecFactory

#endif // }

#endif // }
