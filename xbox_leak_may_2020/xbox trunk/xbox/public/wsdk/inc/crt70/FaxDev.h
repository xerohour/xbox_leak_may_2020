/*++

Copyright (c) 1996-1999  Microsoft Corporation

Module Name:

    faxdev.h

Abstract:

    This file contains the prototypes, etc for the
    FAX device provider API and for the Extended Fax Service Provider API.

    Note: To use the extended FSPI one should define USE_EXTENDED_FSPI. Otherwise
          the EFSPI related elements will not be compiled.


--*/

#include <commctrl.h>


#ifndef _FAXDEV_
#define _FAXDEV_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// FAX status constants
//

#define FS_INITIALIZING         0x20000000
#define FS_DIALING              0x20000001
#define FS_TRANSMITTING         0x20000002
#define FS_RECEIVING            0x20000004
#define FS_COMPLETED            0x20000008
#define FS_HANDLED              0x20000010
#define FS_LINE_UNAVAILABLE     0x20000020
#define FS_BUSY                 0x20000040
#define FS_NO_ANSWER            0x20000080
#define FS_BAD_ADDRESS          0x20000100
#define FS_NO_DIAL_TONE         0x20000200
#define FS_DISCONNECTED         0x20000400
#define FS_FATAL_ERROR          0x20000800  // see the stringid for the real status
#define FS_NOT_FAX_CALL         0x20001000
#define FS_CALL_DELAYED         0x20002000
#define FS_CALL_BLACKLISTED     0x20004000
#define FS_USER_ABORT           0x20200000
#define FS_ANSWERED             0x20800000


//
// data structures
//

typedef struct _FAX_SEND {
    DWORD   SizeOfStruct;
    LPWSTR  FileName;
    LPWSTR  CallerName;
    LPWSTR  CallerNumber;
    LPWSTR  ReceiverName;
    LPWSTR  ReceiverNumber;
    BOOL    Branding;
    HCALL   CallHandle;
    DWORD   Reserved[3];
} FAX_SEND, *PFAX_SEND;

typedef struct _FAX_RECEIVE {
    DWORD   SizeOfStruct;
    LPWSTR  FileName;
    LPWSTR  ReceiverName;
    LPWSTR  ReceiverNumber;
    DWORD   Reserved[4];
} FAX_RECEIVE, *PFAX_RECEIVE;

typedef struct _FAX_DEV_STATUS {
    DWORD   SizeOfStruct;
    DWORD   StatusId;
    DWORD   StringId;
    DWORD   PageCount;
    LPWSTR  CSI;
    LPWSTR  CallerId;
    LPWSTR  RoutingInfo;
    DWORD   ErrorCode;
    DWORD   Reserved[3];
} FAX_DEV_STATUS, *PFAX_DEV_STATUS;

typedef BOOL
(CALLBACK *PFAX_SERVICE_CALLBACK)(
    IN HANDLE FaxHandle,
    IN DWORD  DeviceId,
    IN DWORD_PTR  Param1,
    IN DWORD_PTR  Param2,
    IN DWORD_PTR  Param3
    );

typedef void
(CALLBACK *PFAX_LINECALLBACK)(
    IN HANDLE FaxHandle,
    IN DWORD hDevice,
    IN DWORD dwMessage,
    IN DWORD_PTR dwInstance,
    IN DWORD_PTR dwParam1,
    IN DWORD_PTR dwParam2,
    IN DWORD_PTR dwParam3
    );





BOOL WINAPI
FaxDevInitialize(
    IN  HLINEAPP LineAppHandle,
    IN  HANDLE HeapHandle,
    OUT PFAX_LINECALLBACK *LineCallbackFunction,
    IN  PFAX_SERVICE_CALLBACK FaxServiceCallback
    );

BOOL WINAPI
FaxDevVirtualDeviceCreation(
    OUT LPDWORD DeviceCount,
    OUT LPWSTR DeviceNamePrefix,
    OUT LPDWORD DeviceIdPrefix,
    IN  HANDLE CompletionPort,
    IN  ULONG_PTR CompletionKey
    );

BOOL WINAPI
FaxDevStartJob(
    IN  HLINE LineHandle,
    IN  DWORD DeviceId,
    OUT PHANDLE FaxHandle,
    IN  HANDLE CompletionPortHandle,
    IN  ULONG_PTR CompletionKey
    );

BOOL WINAPI
FaxDevEndJob(
    IN  HANDLE FaxHandle
    );

typedef BOOL
(CALLBACK *PFAX_SEND_CALLBACK)(
    IN HANDLE FaxHandle,
    IN HCALL CallHandle,
    IN DWORD Reserved1,
    IN DWORD Reserved2
    );

BOOL WINAPI
FaxDevSend(
    IN  HANDLE FaxHandle,
    IN  PFAX_SEND FaxSend,
    IN  PFAX_SEND_CALLBACK FaxSendCallback
    );

#define FAXDEVRECEIVE_SIZE 4096

BOOL WINAPI
FaxDevReceive(
    IN  HANDLE FaxHandle,
    IN  HCALL CallHandle,
    IN OUT PFAX_RECEIVE FaxReceive
    );

#define FAXDEVREPORTSTATUS_SIZE 4096

BOOL WINAPI
FaxDevReportStatus(
    IN  HANDLE FaxHandle OPTIONAL,
    OUT PFAX_DEV_STATUS FaxStatus,
    IN  DWORD FaxStatusSize,
    OUT LPDWORD FaxStatusSizeRequired
    );

BOOL WINAPI
FaxDevAbortOperation(
    IN  HANDLE FaxHandle
    );

BOOL WINAPI
FaxDevConfigure(
    OUT HPROPSHEETPAGE *PropSheetPage
    );


typedef BOOL (WINAPI *PFAXDEVINITIALIZE)                (HLINEAPP,HANDLE,PFAX_LINECALLBACK*,PFAX_SERVICE_CALLBACK);
typedef BOOL (WINAPI *PFAXDEVVIRTUALDEVICECREATION)     (LPDWORD,LPWSTR,LPDWORD,HANDLE,ULONG_PTR);
typedef BOOL (WINAPI *PFAXDEVSTARTJOB)                  (HLINE,DWORD,PHANDLE,HANDLE,ULONG_PTR);
typedef BOOL (WINAPI *PFAXDEVENDJOB)                    (HANDLE);
typedef BOOL (WINAPI *PFAXDEVSEND)                      (HANDLE,PFAX_SEND,PFAX_SEND_CALLBACK);
typedef BOOL (WINAPI *PFAXDEVRECEIVE)                   (HANDLE,HCALL,PFAX_RECEIVE);
typedef BOOL (WINAPI *PFAXDEVREPORTSTATUS)              (HANDLE,PFAX_DEV_STATUS,DWORD,LPDWORD);
typedef BOOL (WINAPI *PFAXDEVABORTOPERATION)            (HANDLE);
typedef BOOL (WINAPI *PFAXDEVCONFIGURE)                 (HPROPSHEETPAGE*);



#ifdef USE_EXTENDED_FSPI

#include <oleauto.h>

//
// Extended Fax Service Provider Interface
//


//
// Maximum string length constants
//
#define FSPI_MAX_FRIENDLY_NAME  256

//
// FaxServiceCallbackEx() message types
//
#define FSPI_MSG_VIRTUAL_DEVICE_STATUS              0x00000001
#define FSPI_MSG_VIRTUAL_DEVICE_LIST_CHANGED        0x00000002
#define FSPI_MSG_JOB_STATUS                         0x00000003

//
// FSPI_MSG_VIRTUAL_DEVICE_STATUS status codes
//
#define FSPI_DEVSTATUS_READY_TO_SEND                 0x00000001
#define FSPI_DEVSTATUS_CAN_NOT_SEND                  0x00000002
#define FSPI_DEVSTATUS_NEW_INBOUND_MESSAGE           0x00000003
#define FSPI_DEVSTATUS_RINGING                       0x00000004

//
// Job Queue Status codes
//

#define FSPI_JS_UNKNOWN             0x00000001
#define FSPI_JS_PENDING             0x00000002
#define FSPI_JS_INPROGRESS          0x00000003
#define FSPI_JS_SUSPENDING          0x00000004
#define FSPI_JS_SUSPENDED           0x00000005
#define FSPI_JS_RESUMING            0x00000006
#define FSPI_JS_ABORTING            0x00000007
#define FSPI_JS_ABORTED             0x00000008
#define FSPI_JS_COMPLETED           0x00000009
#define FSPI_JS_RETRY               0x0000000A
#define FSPI_JS_FAILED              0x0000000B
#define FSPI_JS_FAILED_NO_RETRY     0x0000000C
#define FSPI_JS_DELETED             0x0000000D


//
// Extended job status codes
//

#define FSPI_ES_DISCONNECTED        0x00000001
#define FSPI_ES_INITIALIZING        0x00000002
#define FSPI_ES_DIALING             0x00000003
#define FSPI_ES_TRANSMITTING        0x00000004
#define FSPI_ES_ANSWERED            0x00000005
#define FSPI_ES_RECEIVING           0x00000006
#define FSPI_ES_LINE_UNAVAILABLE    0x00000007
#define FSPI_ES_BUSY                0x00000008
#define FSPI_ES_NO_ANSWER           0x00000009
#define FSPI_ES_BAD_ADDRESS         0x0000000A
#define FSPI_ES_NO_DIAL_TONE        0x0000000B
#define FSPI_ES_FATAL_ERROR         0x0000000C
#define FSPI_ES_CALL_DELAYED        0x0000000D
#define FSPI_ES_CALL_BLACKLISTED    0x0000000E
#define FSPI_ES_NOT_FAX_CALL        0x0000000F
#define FSPI_ES_PARTIALLY_RECEIVED  0x00000010
#define FSPI_ES_HANDLED             0x00000011
#define FSPI_ES_PROPRIETARY         0x01000000

//
// Status information fields availability flags
//
#define FSPI_JOB_STATUS_INFO_REMOTE_STATION_ID     0x00000001
#define FSPI_JOB_STATUS_INFO_CALLERID              0x00000002
#define FSPI_JOB_STATUS_INFO_ROUTINGINFO           0x00000004
#define FSPI_JOB_STATUS_INFO_PAGECOUNT             0x00000008
#define FSPI_JOB_STATUS_INFO_TRANSMISSION_START    0x00000010
#define FSPI_JOB_STATUS_INFO_TRANSMISSION_END      0x00000020




//
// EFSPI success and error HRESULT codes
//
#define FSPI_S_OK                       S_OK
#define FSPI_E_INVALID_GUID             MAKE_HRESULT(1,FACILITY_ITF,0x0001)
#define FSPI_E_DUPLICATE_IMAGE          MAKE_HRESULT(1,FACILITY_ITF,0x0002)
#define FSPI_E_DUPLICATE_TSP            MAKE_HRESULT(1,FACILITY_ITF,0x0003)
#define FSPI_E_INVALID_LOG_INFO         MAKE_HRESULT(1,FACILITY_ITF,0x0004)
#define FSPI_E_FSP_NOT_FOUND            MAKE_HRESULT(1,FACILITY_ITF,0x0005)
#define FSPI_E_INVALID_COVER_PAGE       MAKE_HRESULT(1,FACILITY_ITF,0x0006)
#define FSPI_E_CAN_NOT_CREATE_FILE      MAKE_HRESULT(1,FACILITY_ITF,0x0007)
#define FSPI_E_CAN_NOT_OPEN_FILE        MAKE_HRESULT(1,FACILITY_ITF,0x0008)
#define FSPI_E_CAN_NOT_WRITE_FILE       MAKE_HRESULT(1,FACILITY_ITF,0x0009)
#define FSPI_E_NO_DISK_SPACE            MAKE_HRESULT(1,FACILITY_ITF,0x000A)
#define FSPI_E_NOMEM                    MAKE_HRESULT(1,FACILITY_ITF,0x000B)
#define FSPI_E_FAILED                   MAKE_HRESULT(1,FACILITY_ITF,0x000C)
#define FSPI_E_INVALID_MESSAGE_ID       MAKE_HRESULT(1,FACILITY_ITF,0x000E)
#define FSPI_E_INVALID_JOB_HANDLE       MAKE_HRESULT(1,FACILITY_ITF,0x000F)
#define FSPI_E_INVALID_MSG              MAKE_HRESULT(1,FACILITY_ITF,0x0010)
#define FSPI_E_INVALID_PARAM1           MAKE_HRESULT(1,FACILITY_ITF,0x0011)
#define FSPI_E_INVALID_PARAM2           MAKE_HRESULT(1,FACILITY_ITF,0x0012)
#define FSPI_E_INVALID_PARAM3           MAKE_HRESULT(1,FACILITY_ITF,0x0013)
#define FSPI_E_INVALID_EFSP             MAKE_HRESULT(1,FACILITY_ITF,0x0014)
#define FSPI_E_BUFFER_OVERFLOW          MAKE_HRESULT(1,FACILITY_ITF,0x0015)

//
// EFSP capability flags
//
#define  FSPI_CAP_BROADCAST                         0x00000001
#define  FSPI_CAP_MULTISEND                         0x00000002
#define  FSPI_CAP_SCHEDULING                        0x00000004
#define  FSPI_CAP_ABORT_RECIPIENT                   0x00000008
#define  FSPI_CAP_ABORT_PARENT                      0x00000010
#define  FSPI_CAP_AUTO_RETRY                        0x00000020
#define  FSPI_CAP_SIMULTANEOUS_SEND_RECEIVE         0x00000040


//
// Fax Log Identifiers
//
#define  FSPI_LOG_RECEIVE           0x00000001
#define  FSPI_LOG_SEND              0x00000002

//
// Cover page format constants
//

#define FSPI_COVER_PAGE_FMT_COV     0x00000001
#define EFSPI_MAX_DEVICE_COUNT      (DEFAULT_REGVAL_PROVIDER_DEVICE_ID_PREFIX_STEP - 1) // Max devices a single FSP can export

//
// data structures
//

typedef struct _FSPI_PERSONAL_PROFILE {
    DWORD      dwSizeOfStruct;
    LPWSTR     lpwstrName;
    LPWSTR     lpwstrFaxNumber;
    LPWSTR     lpwstrCompany;
    LPWSTR     lpwstrStreetAddress;
    LPWSTR     lpwstrCity;
    LPWSTR     lpwstrState;
    LPWSTR     lpwstrZip;
    LPWSTR     lpwstrCountry;
    LPWSTR     lpwstrTitle;
    LPWSTR     lpwstrDepartment;
    LPWSTR     lpwstrOfficeLocation;
    LPWSTR     lpwstrHomePhone;
    LPWSTR     lpwstrOfficePhone;
    LPWSTR     lpwstrEmail;
    LPWSTR     lpwstrBillingCode;
    LPWSTR     lpwstrTSID;
} FSPI_PERSONAL_PROFILE;

typedef FSPI_PERSONAL_PROFILE * LPFSPI_PERSONAL_PROFILE;
typedef const FSPI_PERSONAL_PROFILE * LPCFSPI_PERSONAL_PROFILE;

typedef struct _FSPI_COVERPAGE_INFO {
    DWORD   dwSizeOfStruct;
    DWORD   dwCoverPageFormat;
    LPWSTR  lpwstrCoverPageFileName;
    DWORD   dwNumberOfPages;
    LPWSTR  lpwstrNote;
    LPWSTR  lpwstrSubject;
} FSPI_COVERPAGE_INFO;


typedef FSPI_COVERPAGE_INFO * LPFSPI_COVERPAGE_INFO;
typedef const FSPI_COVERPAGE_INFO * LPCFSPI_COVERPAGE_INFO;


typedef struct _FSPI_MESSAGE_ID {
    DWORD   dwSizeOfStruct;
    DWORD   dwIdSize;
    LPBYTE  lpbId;
} FSPI_MESSAGE_ID;

typedef FSPI_MESSAGE_ID * LPFSPI_MESSAGE_ID;
typedef const FSPI_MESSAGE_ID * LPCFSPI_MESSAGE_ID;

typedef struct _FSPI_DEVICE_INFO {
    DWORD dwSizeOfStruct;
    WCHAR szFriendlyName[FSPI_MAX_FRIENDLY_NAME];
    DWORD dwId;
} FSPI_DEVICE_INFO;

typedef FSPI_DEVICE_INFO * LPFSPI_DEVICE_INFO;
typedef const FSPI_DEVICE_INFO * LPCFSPI_DEVICE_INFO;

typedef struct _FSPI_JOB_STATUS {
    DWORD dwSizeOfStruct;
    DWORD fAvailableStatusInfo;
    DWORD dwJobStatus;
    DWORD dwExtendedStatus;
    DWORD dwExtendedStatusStringId;
    LPWSTR lpwstrRemoteStationId;
    LPWSTR lpwstrCallerId;
    LPWSTR lpwstrRoutingInfo;
    DWORD dwPageCount;
    SYSTEMTIME tmTransmissionStart;
    SYSTEMTIME tmTransmissionEnd;
} FSPI_JOB_STATUS;

typedef FSPI_JOB_STATUS * LPFSPI_JOB_STATUS;
typedef const FSPI_JOB_STATUS * LPCFSPI_JOB_STATUS;


//
// EFSPI Interface Functions
//

typedef HRESULT (CALLBACK * PFAX_SERVICE_CALLBACK_EX)(
    IN HANDLE hFSP,
    IN DWORD  dwMsgType,
    IN DWORD  Param1,
    IN DWORD  Param2,
    IN DWORD  Param3
) ;


HRESULT WINAPI FaxDevInitializeEx(
    IN  HANDLE                      hFSP,
    IN  HLINEAPP                    LineAppHandle,
    OUT PFAX_LINECALLBACK *         LineCallbackFunction,
    IN  PFAX_SERVICE_CALLBACK_EX    FaxServiceCallbackEx,
    OUT LPDWORD                     lpdwMaxMessageIdSize
);


HRESULT WINAPI FaxDevSendEx(
    IN  HLINE                       hTapiLine,
    IN  DWORD                       dwDeviceId,
    IN  LPCWSTR                     lpcwstrBodyFileName,
    IN  LPCFSPI_COVERPAGE_INFO      lpcCoverPageInfo,
    IN  BOOL                        bAddBranding,
    IN  SYSTEMTIME                  tmSchedule,
    IN  LPCFSPI_PERSONAL_PROFILE    lpcSenderProfile,
    IN  DWORD                       dwNumRecipients,
    IN  LPCFSPI_PERSONAL_PROFILE    lpcRecipientProfiles,
    OUT LPFSPI_MESSAGE_ID           lpRecipientMessageIds,
    OUT PHANDLE                     lphRecipientJobs,
    OUT LPFSPI_MESSAGE_ID           lpParentMessageId,
    OUT LPHANDLE                    lphParentJob

);

HRESULT WINAPI FaxDevReestablishJobContext(
    IN  HLINE               hTapiLine,
    IN  DWORD               dwDeviceId,
    IN  LPCFSPI_MESSAGE_ID  lpcParentMessageId,
    OUT PHANDLE             lphParentJob,
    IN  DWORD               dwRecipientCount,
    IN  LPCFSPI_MESSAGE_ID  lpcRecipientMessageIds,
    OUT PHANDLE             lpRecipientJobs
);

HRESULT WINAPI FaxDevReportStatusEx(
  IN         HANDLE hJob,
  IN OUT     LPFSPI_JOB_STATUS lpStatus,
  IN         DWORD dwStatusSize,
  OUT        LPDWORD lpdwRequiredStatusSize
);


HRESULT WINAPI FaxDevEnumerateDevices(
    IN      DWORD dwDeviceIdBase,
    IN OUT  LPDWORD lpdwDeviceCount,
    OUT     LPFSPI_DEVICE_INFO lpDevices
);

HRESULT WINAPI FaxDevShutdown(
    void
);

HRESULT WINAPI FaxDevGetLogData(
    IN  HANDLE hFaxHandle,
    OUT VARIANT * lppLogData
);

typedef HRESULT (WINAPI * PFAXDEVINITIALIZEEX)
(   
    IN  HANDLE                      hFSP,
    IN  HLINEAPP                    LineAppHandle,
    OUT PFAX_LINECALLBACK *         LineCallbackFunction,
    IN  PFAX_SERVICE_CALLBACK_EX    FaxServiceCallbackEx,
    OUT LPDWORD                     lpdwMaxMessageIdSize
);

typedef HRESULT (WINAPI * PFAXDEVSENDEX)
(
    IN  HLINE                       hTapiLine,
    IN  DWORD                       dwDeviceId,
    IN  LPCWSTR                     lpcwstrBodyFileName,
    IN  LPCFSPI_COVERPAGE_INFO      lpcCoverPageInfo,
    IN  BOOL                        bAddBranding,
    IN  SYSTEMTIME                  tmSchedule,
    IN  LPCFSPI_PERSONAL_PROFILE    lpcSenderProfile,
    IN  DWORD                       dwNumRecipients,
    IN  LPCFSPI_PERSONAL_PROFILE    lpcRecipientProfiles,
    OUT LPFSPI_MESSAGE_ID           lpRecipientMessageIds,
    OUT PHANDLE                     lphRecipientJobs,
    OUT LPFSPI_MESSAGE_ID           lpParentMessageId,
    OUT LPHANDLE                    lphParentJob
);


typedef HRESULT (WINAPI * PFAXDEVREESTABLISHJOBCONTEXT)
(
    IN  HLINE               hTapiLine,
    IN  DWORD               dwDeviceId,
    IN  LPCFSPI_MESSAGE_ID  lpcParentMessageId,
    OUT PHANDLE             lphParentJob,
    IN  DWORD               dwRecipientCount,
    IN  LPCFSPI_MESSAGE_ID  lpcRecipientMessageIds,
    OUT PHANDLE             lpRecipientJobs
);

typedef HRESULT (WINAPI * PFAXDEVREPORTSTATUSEX)
(
  IN         HANDLE hJob,
  IN OUT     LPFSPI_JOB_STATUS lpStatus,
  IN         DWORD dwStatusSize,
  OUT        LPDWORD lpdwRequiredStatusSize
);

typedef HRESULT (WINAPI * PFAXDEVENUMERATEDEVICES)
(
    IN      DWORD dwDeviceIdBase,
    IN OUT  LPDWORD lpdwDeviceCount,
    OUT     LPFSPI_DEVICE_INFO lpDevices
);

typedef HRESULT (WINAPI * PFAXDEVSHUTDOWN)
(
    void
);

typedef HRESULT (WINAPI * PFAXDEVGETLOGDATA)
(
    IN  HANDLE hFaxHandle,
    OUT VARIANT * lppLogData
);

//
// Microsoft Fax Cover Page Version 5 signature.
//
#define FAX_COVER_PAGE_V5_SIGNATURE \
                    {0x46, 0x41, 0x58, 0x43, \
                     0x4F, 0x56, 0x45, 0x52, \
                     0x2D, 0x56, 0x45, 0x52, \
                     0x30, 0x30, 0x35, 0x77, \
                     0x87, 0x00, 0x00, 0x00};

//
// Cover page header section
//
typedef struct
{
    BYTE    abSignature[20];
    DWORD   dwEMFSize;
    DWORD   nTextBoxes;
    SIZE    sizeCoverPage;
} FAXCOVERPAGETEMPLATEHEADER;

//
// Cover page text box.
//
typedef struct
{
    RECT        rectPosition;
    COLORREF    colorrefColor;
    LONG        lAlignment;
    LOGFONTW    logfontFont;
    WORD        wTextBoxId;
    DWORD       dwStringBytes;
} FAXCOVERPAGETEMPLATETEXTBOX;

//
// Cover page miscellaneous data.
//
typedef struct
{
    short       sScale;
    short       sPaperSize;
    short       sOrientation;
    COLORREF    colorrefPaperColor;
} FAXCOVERPAGETEMPLATEMISCDATA;

//
// Cover page text box IDs.
//
#define IDS_PROP_RP_NAME                2001 // Recipient Name
#define IDS_PROP_RP_FXNO                2003 // Recipient Fax Number
#define IDS_PROP_RP_COMP                2005 // Recipient's Company
#define IDS_PROP_RP_ADDR                2007 // Recipient's Street Address
#define IDS_PROP_RP_TITL                2009 // Recipient's Title
#define IDS_PROP_RP_DEPT                2011 // Recipient's Department
#define IDS_PROP_RP_OFFI                2013 // Recipient's Office Location
#define IDS_PROP_RP_HTEL                2015 // Recipient's Home Telephone #
#define IDS_PROP_RP_OTEL                2017 // Recipient's Office Telephone #
#define IDS_PROP_RP_TOLS                2019 // To: List
#define IDS_PROP_RP_CCLS                2021 // Cc: List
#define IDS_PROP_SN_NAME                2023 // Sender Name
#define IDS_PROP_SN_FXNO                2025 // Sender Fax #
#define IDS_PROP_SN_COMP                2027 // Sender's Company
#define IDS_PROP_SN_ADDR                2029 // Sender's Address
#define IDS_PROP_SN_TITL                2031 // Sender's Title
#define IDS_PROP_SN_DEPT                2033 // Sender's Department
#define IDS_PROP_SN_OFFI                2035 // Sender's Office Location
#define IDS_PROP_SN_HTEL                2037 // Sender's Home Telephone #
#define IDS_PROP_SN_OTEL                2039 // Sender's Office Telephone #
#define IDS_PROP_MS_SUBJ                2041 // Subject
#define IDS_PROP_MS_TSNT                2043 // Time Sent
#define IDS_PROP_MS_NOPG                2045 // # of Pages
#define IDS_PROP_MS_NOAT                2047 // # of Attachments
#define IDS_PROP_MS_BCOD                2049 // Billing Code
#define IDS_PROP_RP_CITY                2053 // Recipient's City
#define IDS_PROP_RP_STAT                2055 // Recipient's State
#define IDS_PROP_RP_ZIPC                2057 // Recipient's Zip Code
#define IDS_PROP_RP_CTRY                2059 // Recipient's Country
#define IDS_PROP_RP_POBX                2061 // Recipient's Post Office Box
#define IDS_PROP_MS_NOTE                2063 // Note





#endif // USE_EXTENDED_FSPI

#ifdef __cplusplus
}
#endif

#endif
