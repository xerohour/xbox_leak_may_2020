/*-----------------------------------------------------------------------------
Microsoft VSEE

Microsoft Confidential
Copyright 1995-1999 Microsoft Corporation. All Rights Reserved.

@doc external
@module VapiGuids - GUIDs' declarations for VAPI related components

@owner a-PavelT
-----------------------------------------------------------------------------*/
#if !defined(VS_COMMON_INC_VSEE_VAPI_GUIDS_H_INCLUDED_) // {
#define VS_COMMON_INC_VSEE_VAPI_GUIDS_H_INCLUDED_
/*#pragma once ends up in .rgi, which is bad, so do not do it*/

#include "VseeGuidDataToXxx.h"

/*-----------------------------------------------------------------------------
Guid "data" and guids that have only one needed form here
This form of guids is approximately available from uuidgen -s, then remove the braces and "0x"s.
-----------------------------------------------------------------------------*/
#define CLSID_TS_VapiEnumeratorLibrary_data     (68403BDE, 10BC, 11d3, AD,36,00,C0,4F,A3,29,BB)
#define CLSID_TS_VapiEnumerator_data            (72480079, 10BC, 11d3, AD,36,00,C0,4F,A3,29,BB)
#define CLSID_TS_MergeEngineLibrary_data        (63D0C15E, 1C7E, 11D3, A1,C4,00,50,04,0A,E0,B0)
#define CLSID_TS_MergeEngine_data               (E86ACAEA, 1C7E, 11D3, A1,C4,00,50,04,0A,E0,B0)
#define CLSID_TS_MergeBrokerLibrary_data        (EBC1D0F9, 559D, 11D3, A1,E7,00,50,04,0A,E0,B0)
#define CLSID_TS_MergeBroker_data               (F8CE0F83, 559D, 11D3, A1,E7,00,50,04,0A,E0,B0)

#define   IID_TS_IMergeEngine_S                 B0317688-1C7E-11D3-A1C4-0050040AE0B0
#define   IID_TS_IMergeBroker_S                 CA490D75-559D-11D3-A1E7-0050040AE0B0

/*-----------------------------------------------------------------------------
other forms of guids generated below, no guids below, no guid "data" below
-----------------------------------------------------------------------------*/
#define      CLSID_TS_VapiEnumeratorLibrary_S     VSEE_GUID_DATA_TO_DASHED             CLSID_TS_VapiEnumeratorLibrary_data
#define CHAR_CLSID_TS_VapiEnumeratorLibrary       VSEE_GUID_DATA_TO_BRACED_DASHED_CHAR CLSID_TS_VapiEnumeratorLibrary_data

#define      CLSID_TS_VapiEnumerator_S            VSEE_GUID_DATA_TO_DASHED             CLSID_TS_VapiEnumerator_data
#define CHAR_CLSID_TS_VapiEnumerator              VSEE_GUID_DATA_TO_BRACED_DASHED_CHAR CLSID_TS_VapiEnumerator_data
#define      CLSID_TS_VapiEnumerator_Str          VSEE_GUID_DATA_TO_STRUCT_INITIALIZER CLSID_TS_VapiEnumerator_data

#define      CLSID_TS_MergeEngineLibrary_S        VSEE_GUID_DATA_TO_DASHED             CLSID_TS_MergeEngineLibrary_data
#define CHAR_CLSID_TS_MergeEngineLibrary          VSEE_GUID_DATA_TO_BRACED_DASHED_CHAR CLSID_TS_MergeEngineLibrary_data

#define      CLSID_TS_MergeEngine_S               VSEE_GUID_DATA_TO_DASHED             CLSID_TS_MergeEngine_data
#define CHAR_CLSID_TS_MergeEngine                 VSEE_GUID_DATA_TO_BRACED_DASHED_CHAR CLSID_TS_MergeEngine_data
#define      CLSID_TS_MergeEngine_Str             VSEE_GUID_DATA_TO_STRUCT_INITIALIZER CLSID_TS_MergeEngine_data

#define      CLSID_TS_MergeBrokerLibrary_S        VSEE_GUID_DATA_TO_DASHED             CLSID_TS_MergeBrokerLibrary_data
#define CHAR_CLSID_TS_MergeBrokerLibrary          VSEE_GUID_DATA_TO_BRACED_DASHED_CHAR CLSID_TS_MergeBrokerLibrary_data

#define      CLSID_TS_MergeBroker_S               VSEE_GUID_DATA_TO_DASHED             CLSID_TS_MergeBroker_data
#define CHAR_CLSID_TS_MergeBroker                 VSEE_GUID_DATA_TO_BRACED_DASHED_CHAR CLSID_TS_MergeBroker_data
#define      CLSID_TS_MergeBroker_Str             VSEE_GUID_DATA_TO_STRUCT_INITIALIZER CLSID_TS_MergeBroker_data

#endif // }
